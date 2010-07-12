// compressed_output.cc -- manage compressed debug sections for gold

// Copyright 2007, 2008, 2010 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#include "gold.h"

#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#include "parameters.h"
#include "options.h"
#include "compressed_output.h"

namespace gold
{

#ifdef HAVE_ZLIB_H

// Compress UNCOMPRESSED_DATA of size UNCOMPRESSED_SIZE.  Returns true
// if it successfully compressed, false if it failed for any reason
// (including not having zlib support in the library).  If it returns
// true, it allocates memory for the compressed data using new, and
// sets *COMPRESSED_DATA and *COMPRESSED_SIZE to appropriate values.
// It also writes a header before COMPRESSED_DATA: 4 bytes saying
// "ZLIB", and 8 bytes indicating the uncompressed size, in big-endian
// order.

static bool
zlib_compress(const unsigned char* uncompressed_data,
              unsigned long uncompressed_size,
              unsigned char** compressed_data,
              unsigned long* compressed_size)
{
  const int header_size = 12;
  *compressed_size = uncompressed_size + uncompressed_size / 1000 + 128;
  *compressed_data = new unsigned char[*compressed_size + header_size];

  int compress_level;
  if (parameters->options().optimize() >= 1)
    compress_level = 9;
  else
    compress_level = 1;

  int rc = compress2(reinterpret_cast<Bytef*>(*compressed_data) + header_size,
                     compressed_size,
                     reinterpret_cast<const Bytef*>(uncompressed_data),
                     uncompressed_size,
                     compress_level);
  if (rc == Z_OK)
    {
      memcpy(*compressed_data, "ZLIB", 4);
      elfcpp::Swap_unaligned<64, true>::writeval(*compressed_data + 4,
						 uncompressed_size);
      *compressed_size += header_size;
      return true;
    }
  else
    {
      delete[] *compressed_data;
      *compressed_data = NULL;
      return false;
    }
}

// Decompress COMPRESSED_DATA of size COMPRESSED_SIZE, into a buffer
// UNCOMPRESSED_DATA of size UNCOMPRESSED_SIZE.  Returns TRUE if it
// decompressed successfully, false if it failed.  The buffer, of
// appropriate size, is provided by the caller, and is typically part
// of the memory-mapped output file.

static bool
zlib_decompress(const unsigned char* compressed_data,
		unsigned long compressed_size,
		unsigned char* uncompressed_data,
		unsigned long uncompressed_size)
{
  z_stream strm;
  int rc;

  /* It is possible the section consists of several compressed
     buffers concatenated together, so we uncompress in a loop.  */
  strm.zalloc = NULL;
  strm.zfree = NULL;
  strm.opaque = NULL;
  strm.avail_in = compressed_size;
  strm.next_in = const_cast<Bytef*>(compressed_data);
  strm.avail_out = uncompressed_size;

  rc = inflateInit(&strm);
  while (strm.avail_in > 0)
    {
      if (rc != Z_OK)
        return false;
      strm.next_out = ((Bytef*) uncompressed_data
                       + (uncompressed_size - strm.avail_out));
      rc = inflate(&strm, Z_FINISH);
      if (rc != Z_STREAM_END)
        return false;
      rc = inflateReset(&strm);
    }
  rc = inflateEnd(&strm);
  if (rc != Z_OK || strm.avail_out != 0)
    return false;

  return true;
}

#else // !defined(HAVE_ZLIB_H)

static bool
zlib_compress(const unsigned char*, unsigned long,
              unsigned char**, unsigned long*)
{
  return false;
}

static bool
zlib_decompress(const unsigned char*, unsigned long,
		unsigned char**, unsigned long*)
{
  return false;
}

#endif // !defined(HAVE_ZLIB_H)

// Read the compression header of a compressed debug section and return
// the uncompressed size.

uint64_t
get_uncompressed_size(const unsigned char* compressed_data,
		      section_size_type compressed_size)
{
  const unsigned int zlib_header_size = 12;

  /* Verify the compression header.  Currently, we support only zlib
     compression, so it should be "ZLIB" followed by the uncompressed
     section size, 8 bytes in big-endian order.  */
  if (compressed_size >= zlib_header_size
      && strncmp(reinterpret_cast<const char*>(compressed_data),
		 "ZLIB", 4) == 0)
    return elfcpp::Swap_unaligned<64, true>::readval(compressed_data + 4);
  return -1ULL;
}

// Decompress a compressed debug section directly into the output file.

bool
decompress_input_section(const unsigned char* compressed_data,
			 unsigned long compressed_size,
			 unsigned char* uncompressed_data,
			 unsigned long uncompressed_size)
{
  const unsigned int zlib_header_size = 12;

  /* Verify the compression header.  Currently, we support only zlib
     compression, so it should be "ZLIB" followed by the uncompressed
     section size, 8 bytes in big-endian order.  */
  if (compressed_size >= zlib_header_size
      && strncmp(reinterpret_cast<const char*>(compressed_data),
		 "ZLIB", 4) == 0)
    {
      unsigned long uncompressed_size_check =
	  elfcpp::Swap_unaligned<64, true>::readval(compressed_data + 4);
      gold_assert(uncompressed_size_check == uncompressed_size);
      return zlib_decompress(compressed_data + zlib_header_size,
			     compressed_size - zlib_header_size,
			     uncompressed_data,
			     uncompressed_size);
    }
  return false;
}

// Class Output_compressed_section.

// Set the final data size of a compressed section.  This is where
// we actually compress the section data.

void
Output_compressed_section::set_final_data_size()
{
  off_t uncompressed_size = this->postprocessing_buffer_size();

  // (Try to) compress the data.
  unsigned long compressed_size;
  unsigned char* uncompressed_data = this->postprocessing_buffer();

  // At this point the contents of all regular input sections will
  // have been copied into the postprocessing buffer, and relocations
  // will have been applied.  Now we need to copy in the contents of
  // anything other than a regular input section.
  this->write_to_postprocessing_buffer();

  bool success = false;
  if (strcmp(this->options_->compress_debug_sections(), "zlib") == 0)
    success = zlib_compress(uncompressed_data, uncompressed_size,
                            &this->data_, &compressed_size);
  if (success)
    {
      // This converts .debug_foo to .zdebug_foo
      this->new_section_name_ = std::string(".z") + (this->name() + 1);
      this->set_name(this->new_section_name_.c_str());
      this->set_data_size(compressed_size);
    }
  else
    {
      gold_warning(_("not compressing section data: zlib error"));
      gold_assert(this->data_ == NULL);
      this->set_data_size(uncompressed_size);
    }
}

// Write out a compressed section.  If we couldn't compress, we just
// write it out as normal, uncompressed data.

void
Output_compressed_section::do_write(Output_file* of)
{
  off_t offset = this->offset();
  off_t data_size = this->data_size();
  unsigned char* view = of->get_output_view(offset, data_size);
  if (this->data_ == NULL)
    memcpy(view, this->postprocessing_buffer(), data_size);
  else
    memcpy(view, this->data_, data_size);
  of->write_output_view(offset, data_size, view);
}

} // End namespace gold.
