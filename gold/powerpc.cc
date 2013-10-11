// powerpc.cc -- powerpc target support for gold.

// Copyright 2008, 2009, 2010, 2011, 2012, 2013 Free Software Foundation, Inc.
// Written by David S. Miller <davem@davemloft.net>
//        and David Edelsohn <edelsohn@gnu.org>

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

#include <set>
#include <algorithm>
#include "elfcpp.h"
#include "dwarf.h"
#include "parameters.h"
#include "reloc.h"
#include "powerpc.h"
#include "object.h"
#include "symtab.h"
#include "layout.h"
#include "output.h"
#include "copy-relocs.h"
#include "target.h"
#include "target-reloc.h"
#include "target-select.h"
#include "tls.h"
#include "errors.h"
#include "gc.h"

namespace
{

using namespace gold;

template<int size, bool big_endian>
class Output_data_plt_powerpc;

template<int size, bool big_endian>
class Output_data_brlt_powerpc;

template<int size, bool big_endian>
class Output_data_got_powerpc;

template<int size, bool big_endian>
class Output_data_glink;

template<int size, bool big_endian>
class Stub_table;

inline bool
is_branch_reloc(unsigned int r_type);

template<int size, bool big_endian>
class Powerpc_relobj : public Sized_relobj_file<size, big_endian>
{
public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  typedef Unordered_set<Section_id, Section_id_hash> Section_refs;
  typedef Unordered_map<Address, Section_refs> Access_from;

  Powerpc_relobj(const std::string& name, Input_file* input_file, off_t offset,
		 const typename elfcpp::Ehdr<size, big_endian>& ehdr)
    : Sized_relobj_file<size, big_endian>(name, input_file, offset, ehdr),
      special_(0), has_small_toc_reloc_(false), opd_valid_(false),
      opd_ent_(), access_from_map_(), has14_(), stub_table_()
  { }

  ~Powerpc_relobj()
  { }

  // The .got2 section shndx.
  unsigned int
  got2_shndx() const
  {
    if (size == 32)
      return this->special_;
    else
      return 0;
  }

  // The .opd section shndx.
  unsigned int
  opd_shndx() const
  {
    if (size == 32)
      return 0;
    else
      return this->special_;
  }

  // Init OPD entry arrays.
  void
  init_opd(size_t opd_size)
  {
    size_t count = this->opd_ent_ndx(opd_size);
    this->opd_ent_.resize(count);
  }

  // Return section and offset of function entry for .opd + R_OFF.
  unsigned int
  get_opd_ent(Address r_off, Address* value = NULL) const
  {
    size_t ndx = this->opd_ent_ndx(r_off);
    gold_assert(ndx < this->opd_ent_.size());
    gold_assert(this->opd_ent_[ndx].shndx != 0);
    if (value != NULL)
      *value = this->opd_ent_[ndx].off;
    return this->opd_ent_[ndx].shndx;
  }

  // Set section and offset of function entry for .opd + R_OFF.
  void
  set_opd_ent(Address r_off, unsigned int shndx, Address value)
  {
    size_t ndx = this->opd_ent_ndx(r_off);
    gold_assert(ndx < this->opd_ent_.size());
    this->opd_ent_[ndx].shndx = shndx;
    this->opd_ent_[ndx].off = value;
  }

  // Return discard flag for .opd + R_OFF.
  bool
  get_opd_discard(Address r_off) const
  {
    size_t ndx = this->opd_ent_ndx(r_off);
    gold_assert(ndx < this->opd_ent_.size());
    return this->opd_ent_[ndx].discard;
  }

  // Set discard flag for .opd + R_OFF.
  void
  set_opd_discard(Address r_off)
  {
    size_t ndx = this->opd_ent_ndx(r_off);
    gold_assert(ndx < this->opd_ent_.size());
    this->opd_ent_[ndx].discard = true;
  }

  bool
  opd_valid() const
  { return this->opd_valid_; }

  void
  set_opd_valid()
  { this->opd_valid_ = true; }

  // Examine .rela.opd to build info about function entry points.
  void
  scan_opd_relocs(size_t reloc_count,
		  const unsigned char* prelocs,
		  const unsigned char* plocal_syms);

  // Perform the Sized_relobj_file method, then set up opd info from
  // .opd relocs.
  void
  do_read_relocs(Read_relocs_data*);

  bool
  do_find_special_sections(Read_symbols_data* sd);

  // Adjust this local symbol value.  Return false if the symbol
  // should be discarded from the output file.
  bool
  do_adjust_local_symbol(Symbol_value<size>* lv) const
  {
    if (size == 64 && this->opd_shndx() != 0)
      {
	bool is_ordinary;
	if (lv->input_shndx(&is_ordinary) != this->opd_shndx())
	  return true;
	if (this->get_opd_discard(lv->input_value()))
	  return false;
      }
    return true;
  }

  Access_from*
  access_from_map()
  { return &this->access_from_map_; }

  // Add a reference from SRC_OBJ, SRC_INDX to this object's .opd
  // section at DST_OFF.
  void
  add_reference(Object* src_obj,
		unsigned int src_indx,
		typename elfcpp::Elf_types<size>::Elf_Addr dst_off)
  {
    Section_id src_id(src_obj, src_indx);
    this->access_from_map_[dst_off].insert(src_id);
  }

  // Add a reference to the code section specified by the .opd entry
  // at DST_OFF
  void
  add_gc_mark(typename elfcpp::Elf_types<size>::Elf_Addr dst_off)
  {
    size_t ndx = this->opd_ent_ndx(dst_off);
    if (ndx >= this->opd_ent_.size())
      this->opd_ent_.resize(ndx + 1);
    this->opd_ent_[ndx].gc_mark = true;
  }

  void
  process_gc_mark(Symbol_table* symtab)
  {
    for (size_t i = 0; i < this->opd_ent_.size(); i++)
      if (this->opd_ent_[i].gc_mark)
	{
	  unsigned int shndx = this->opd_ent_[i].shndx;
	  symtab->gc()->worklist().push(Section_id(this, shndx));
	}
  }

  // Return offset in output GOT section that this object will use
  // as a TOC pointer.  Won't be just a constant with multi-toc support.
  Address
  toc_base_offset() const
  { return 0x8000; }

  void
  set_has_small_toc_reloc()
  { has_small_toc_reloc_ = true; }

  bool
  has_small_toc_reloc() const
  { return has_small_toc_reloc_; }

  void
  set_has_14bit_branch(unsigned int shndx)
  {
    if (shndx >= this->has14_.size())
      this->has14_.resize(shndx + 1);
    this->has14_[shndx] = true;
  }

  bool
  has_14bit_branch(unsigned int shndx) const
  { return shndx < this->has14_.size() && this->has14_[shndx];  }

  void
  set_stub_table(unsigned int shndx, Stub_table<size, big_endian>* stub_table)
  {
    if (shndx >= this->stub_table_.size())
      this->stub_table_.resize(shndx + 1);
    this->stub_table_[shndx] = stub_table;
  }

  Stub_table<size, big_endian>*
  stub_table(unsigned int shndx)
  {
    if (shndx < this->stub_table_.size())
      return this->stub_table_[shndx];
    return NULL;
  }

private:
  struct Opd_ent
  {
    unsigned int shndx;
    bool discard : 1;
    bool gc_mark : 1;
    Address off;
  };

  // Return index into opd_ent_ array for .opd entry at OFF.
  // .opd entries are 24 bytes long, but they can be spaced 16 bytes
  // apart when the language doesn't use the last 8-byte word, the
  // environment pointer.  Thus dividing the entry section offset by
  // 16 will give an index into opd_ent_ that works for either layout
  // of .opd.  (It leaves some elements of the vector unused when .opd
  // entries are spaced 24 bytes apart, but we don't know the spacing
  // until relocations are processed, and in any case it is possible
  // for an object to have some entries spaced 16 bytes apart and
  // others 24 bytes apart.)
  size_t
  opd_ent_ndx(size_t off) const
  { return off >> 4;}

  // For 32-bit the .got2 section shdnx, for 64-bit the .opd section shndx.
  unsigned int special_;

  // For 64-bit, whether this object uses small model relocs to access
  // the toc.
  bool has_small_toc_reloc_;

  // Set at the start of gc_process_relocs, when we know opd_ent_
  // vector is valid.  The flag could be made atomic and set in
  // do_read_relocs with memory_order_release and then tested with
  // memory_order_acquire, potentially resulting in fewer entries in
  // access_from_map_.
  bool opd_valid_;

  // The first 8-byte word of an OPD entry gives the address of the
  // entry point of the function.  Relocatable object files have a
  // relocation on this word.  The following vector records the
  // section and offset specified by these relocations.
  std::vector<Opd_ent> opd_ent_;

  // References made to this object's .opd section when running
  // gc_process_relocs for another object, before the opd_ent_ vector
  // is valid for this object.
  Access_from access_from_map_;

  // Whether input section has a 14-bit branch reloc.
  std::vector<bool> has14_;

  // The stub table to use for a given input section.
  std::vector<Stub_table<size, big_endian>*> stub_table_;
};

template<int size, bool big_endian>
class Powerpc_dynobj : public Sized_dynobj<size, big_endian>
{
public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;

  Powerpc_dynobj(const std::string& name, Input_file* input_file, off_t offset,
		 const typename elfcpp::Ehdr<size, big_endian>& ehdr)
    : Sized_dynobj<size, big_endian>(name, input_file, offset, ehdr),
      opd_shndx_(0), opd_ent_()
  { }

  ~Powerpc_dynobj()
  { }

  // Call Sized_dynobj::do_read_symbols to read the symbols then
  // read .opd from a dynamic object, filling in opd_ent_ vector,
  void
  do_read_symbols(Read_symbols_data*);

  // The .opd section shndx.
  unsigned int
  opd_shndx() const
  {
    return this->opd_shndx_;
  }

  // The .opd section address.
  Address
  opd_address() const
  {
    return this->opd_address_;
  }

  // Init OPD entry arrays.
  void
  init_opd(size_t opd_size)
  {
    size_t count = this->opd_ent_ndx(opd_size);
    this->opd_ent_.resize(count);
  }

  // Return section and offset of function entry for .opd + R_OFF.
  unsigned int
  get_opd_ent(Address r_off, Address* value = NULL) const
  {
    size_t ndx = this->opd_ent_ndx(r_off);
    gold_assert(ndx < this->opd_ent_.size());
    gold_assert(this->opd_ent_[ndx].shndx != 0);
    if (value != NULL)
      *value = this->opd_ent_[ndx].off;
    return this->opd_ent_[ndx].shndx;
  }

  // Set section and offset of function entry for .opd + R_OFF.
  void
  set_opd_ent(Address r_off, unsigned int shndx, Address value)
  {
    size_t ndx = this->opd_ent_ndx(r_off);
    gold_assert(ndx < this->opd_ent_.size());
    this->opd_ent_[ndx].shndx = shndx;
    this->opd_ent_[ndx].off = value;
  }

private:
  // Used to specify extent of executable sections.
  struct Sec_info
  {
    Sec_info(Address start_, Address len_, unsigned int shndx_)
      : start(start_), len(len_), shndx(shndx_)
    { }

    bool
    operator<(const Sec_info& that) const
    { return this->start < that.start; }

    Address start;
    Address len;
    unsigned int shndx;
  };

  struct Opd_ent
  {
    unsigned int shndx;
    Address off;
  };

  // Return index into opd_ent_ array for .opd entry at OFF.
  size_t
  opd_ent_ndx(size_t off) const
  { return off >> 4;}

  // For 64-bit the .opd section shndx and address.
  unsigned int opd_shndx_;
  Address opd_address_;

  // The first 8-byte word of an OPD entry gives the address of the
  // entry point of the function.  Records the section and offset
  // corresponding to the address.  Note that in dynamic objects,
  // offset is *not* relative to the section.
  std::vector<Opd_ent> opd_ent_;
};

template<int size, bool big_endian>
class Target_powerpc : public Sized_target<size, big_endian>
{
 public:
  typedef
    Output_data_reloc<elfcpp::SHT_RELA, true, size, big_endian> Reloc_section;
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  typedef typename elfcpp::Elf_types<size>::Elf_Swxword Signed_address;
  static const Address invalid_address = static_cast<Address>(0) - 1;
  // Offset of tp and dtp pointers from start of TLS block.
  static const Address tp_offset = 0x7000;
  static const Address dtp_offset = 0x8000;

  Target_powerpc()
    : Sized_target<size, big_endian>(&powerpc_info),
      got_(NULL), plt_(NULL), iplt_(NULL), brlt_section_(NULL),
      glink_(NULL), rela_dyn_(NULL), copy_relocs_(elfcpp::R_POWERPC_COPY),
      tlsld_got_offset_(-1U),
      stub_tables_(), branch_lookup_table_(), branch_info_(),
      plt_thread_safe_(false)
  {
  }

  // Process the relocations to determine unreferenced sections for
  // garbage collection.
  void
  gc_process_relocs(Symbol_table* symtab,
		    Layout* layout,
		    Sized_relobj_file<size, big_endian>* object,
		    unsigned int data_shndx,
		    unsigned int sh_type,
		    const unsigned char* prelocs,
		    size_t reloc_count,
		    Output_section* output_section,
		    bool needs_special_offset_handling,
		    size_t local_symbol_count,
		    const unsigned char* plocal_symbols);

  // Scan the relocations to look for symbol adjustments.
  void
  scan_relocs(Symbol_table* symtab,
	      Layout* layout,
	      Sized_relobj_file<size, big_endian>* object,
	      unsigned int data_shndx,
	      unsigned int sh_type,
	      const unsigned char* prelocs,
	      size_t reloc_count,
	      Output_section* output_section,
	      bool needs_special_offset_handling,
	      size_t local_symbol_count,
	      const unsigned char* plocal_symbols);

  // Map input .toc section to output .got section.
  const char*
  do_output_section_name(const Relobj*, const char* name, size_t* plen) const
  {
    if (size == 64 && strcmp(name, ".toc") == 0)
      {
	*plen = 4;
	return ".got";
      }
    return NULL;
  }

  // Provide linker defined save/restore functions.
  void
  define_save_restore_funcs(Layout*, Symbol_table*);

  // No stubs unless a final link.
  bool
  do_may_relax() const
  { return !parameters->options().relocatable(); }

  bool
  do_relax(int, const Input_objects*, Symbol_table*, Layout*, const Task*);

  void
  do_plt_fde_location(const Output_data*, unsigned char*,
		      uint64_t*, off_t*) const;

  // Stash info about branches, for stub generation.
  void
  push_branch(Powerpc_relobj<size, big_endian>* ppc_object,
	      unsigned int data_shndx, Address r_offset,
	      unsigned int r_type, unsigned int r_sym, Address addend)
  {
    Branch_info info(ppc_object, data_shndx, r_offset, r_type, r_sym, addend);
    this->branch_info_.push_back(info);
    if (r_type == elfcpp::R_POWERPC_REL14
	|| r_type == elfcpp::R_POWERPC_REL14_BRTAKEN
	|| r_type == elfcpp::R_POWERPC_REL14_BRNTAKEN)
      ppc_object->set_has_14bit_branch(data_shndx);
  }

  Stub_table<size, big_endian>*
  new_stub_table();

  void
  do_define_standard_symbols(Symbol_table*, Layout*);

  // Finalize the sections.
  void
  do_finalize_sections(Layout*, const Input_objects*, Symbol_table*);

  // Return the value to use for a dynamic which requires special
  // treatment.
  uint64_t
  do_dynsym_value(const Symbol*) const;

  // Return the PLT address to use for a local symbol.
  uint64_t
  do_plt_address_for_local(const Relobj*, unsigned int) const;

  // Return the PLT address to use for a global symbol.
  uint64_t
  do_plt_address_for_global(const Symbol*) const;

  // Return the offset to use for the GOT_INDX'th got entry which is
  // for a local tls symbol specified by OBJECT, SYMNDX.
  int64_t
  do_tls_offset_for_local(const Relobj* object,
			  unsigned int symndx,
			  unsigned int got_indx) const;

  // Return the offset to use for the GOT_INDX'th got entry which is
  // for global tls symbol GSYM.
  int64_t
  do_tls_offset_for_global(Symbol* gsym, unsigned int got_indx) const;

  void
  do_function_location(Symbol_location*) const;

  bool
  do_can_check_for_function_pointers() const
  { return true; }

  // Relocate a section.
  void
  relocate_section(const Relocate_info<size, big_endian>*,
		   unsigned int sh_type,
		   const unsigned char* prelocs,
		   size_t reloc_count,
		   Output_section* output_section,
		   bool needs_special_offset_handling,
		   unsigned char* view,
		   Address view_address,
		   section_size_type view_size,
		   const Reloc_symbol_changes*);

  // Scan the relocs during a relocatable link.
  void
  scan_relocatable_relocs(Symbol_table* symtab,
			  Layout* layout,
			  Sized_relobj_file<size, big_endian>* object,
			  unsigned int data_shndx,
			  unsigned int sh_type,
			  const unsigned char* prelocs,
			  size_t reloc_count,
			  Output_section* output_section,
			  bool needs_special_offset_handling,
			  size_t local_symbol_count,
			  const unsigned char* plocal_symbols,
			  Relocatable_relocs*);

  // Emit relocations for a section.
  void
  relocate_relocs(const Relocate_info<size, big_endian>*,
		  unsigned int sh_type,
		  const unsigned char* prelocs,
		  size_t reloc_count,
		  Output_section* output_section,
		  typename elfcpp::Elf_types<size>::Elf_Off
                    offset_in_output_section,
		  const Relocatable_relocs*,
		  unsigned char*,
		  Address view_address,
		  section_size_type,
		  unsigned char* reloc_view,
		  section_size_type reloc_view_size);

  // Return whether SYM is defined by the ABI.
  bool
  do_is_defined_by_abi(const Symbol* sym) const
  {
    return strcmp(sym->name(), "__tls_get_addr") == 0;
  }

  // Return the size of the GOT section.
  section_size_type
  got_size() const
  {
    gold_assert(this->got_ != NULL);
    return this->got_->data_size();
  }

  // Get the PLT section.
  const Output_data_plt_powerpc<size, big_endian>*
  plt_section() const
  {
    gold_assert(this->plt_ != NULL);
    return this->plt_;
  }

  // Get the IPLT section.
  const Output_data_plt_powerpc<size, big_endian>*
  iplt_section() const
  {
    gold_assert(this->iplt_ != NULL);
    return this->iplt_;
  }

  // Get the .glink section.
  const Output_data_glink<size, big_endian>*
  glink_section() const
  {
    gold_assert(this->glink_ != NULL);
    return this->glink_;
  }

  bool has_glink() const
  { return this->glink_ != NULL; }

  // Get the GOT section.
  const Output_data_got_powerpc<size, big_endian>*
  got_section() const
  {
    gold_assert(this->got_ != NULL);
    return this->got_;
  }

  // Get the GOT section, creating it if necessary.
  Output_data_got_powerpc<size, big_endian>*
  got_section(Symbol_table*, Layout*);

  Object*
  do_make_elf_object(const std::string&, Input_file*, off_t,
		     const elfcpp::Ehdr<size, big_endian>&);

  // Return the number of entries in the GOT.
  unsigned int
  got_entry_count() const
  {
    if (this->got_ == NULL)
      return 0;
    return this->got_size() / (size / 8);
  }

  // Return the number of entries in the PLT.
  unsigned int
  plt_entry_count() const;

  // Return the offset of the first non-reserved PLT entry.
  unsigned int
  first_plt_entry_offset() const;

  // Return the size of each PLT entry.
  unsigned int
  plt_entry_size() const;

  // Add any special sections for this symbol to the gc work list.
  // For powerpc64, this adds the code section of a function
  // descriptor.
  void
  do_gc_mark_symbol(Symbol_table* symtab, Symbol* sym) const;

  // Handle target specific gc actions when adding a gc reference from
  // SRC_OBJ, SRC_SHNDX to a location specified by DST_OBJ, DST_SHNDX
  // and DST_OFF.  For powerpc64, this adds a referenc to the code
  // section of a function descriptor.
  void
  do_gc_add_reference(Symbol_table* symtab,
		      Object* src_obj,
		      unsigned int src_shndx,
		      Object* dst_obj,
		      unsigned int dst_shndx,
		      Address dst_off) const;

  typedef std::vector<Stub_table<size, big_endian>*> Stub_tables;
  const Stub_tables&
  stub_tables() const
  { return this->stub_tables_; }

  const Output_data_brlt_powerpc<size, big_endian>*
  brlt_section() const
  { return this->brlt_section_; }

  void
  add_branch_lookup_table(Address to)
  {
    unsigned int off = this->branch_lookup_table_.size() * (size / 8);
    this->branch_lookup_table_.insert(std::make_pair(to, off));
  }

  Address
  find_branch_lookup_table(Address to)
  {
    typename Branch_lookup_table::const_iterator p
      = this->branch_lookup_table_.find(to);
    return p == this->branch_lookup_table_.end() ? invalid_address : p->second;
  }

  void
  write_branch_lookup_table(unsigned char *oview)
  {
    for (typename Branch_lookup_table::const_iterator p
	   = this->branch_lookup_table_.begin();
	 p != this->branch_lookup_table_.end();
	 ++p)
      {
	elfcpp::Swap<size, big_endian>::writeval(oview + p->second, p->first);
      }
  }

  bool
  plt_thread_safe() const
  { return this->plt_thread_safe_; }

 private:

  class Track_tls
  {
  public:
    enum Tls_get_addr
    {
      NOT_EXPECTED = 0,
      EXPECTED = 1,
      SKIP = 2,
      NORMAL = 3
    };

    Track_tls()
      : tls_get_addr_(NOT_EXPECTED),
	relinfo_(NULL), relnum_(0), r_offset_(0)
    { }

    ~Track_tls()
    {
      if (this->tls_get_addr_ != NOT_EXPECTED)
	this->missing();
    }

    void
    missing(void)
    {
      if (this->relinfo_ != NULL)
	gold_error_at_location(this->relinfo_, this->relnum_, this->r_offset_,
			       _("missing expected __tls_get_addr call"));
    }

    void
    expect_tls_get_addr_call(
	const Relocate_info<size, big_endian>* relinfo,
	size_t relnum,
	Address r_offset)
    {
      this->tls_get_addr_ = EXPECTED;
      this->relinfo_ = relinfo;
      this->relnum_ = relnum;
      this->r_offset_ = r_offset;
    }

    void
    expect_tls_get_addr_call()
    { this->tls_get_addr_ = EXPECTED; }

    void
    skip_next_tls_get_addr_call()
    {this->tls_get_addr_ = SKIP; }

    Tls_get_addr
    maybe_skip_tls_get_addr_call(unsigned int r_type, const Symbol* gsym)
    {
      bool is_tls_call = ((r_type == elfcpp::R_POWERPC_REL24
			   || r_type == elfcpp::R_PPC_PLTREL24)
			  && gsym != NULL
			  && strcmp(gsym->name(), "__tls_get_addr") == 0);
      Tls_get_addr last_tls = this->tls_get_addr_;
      this->tls_get_addr_ = NOT_EXPECTED;
      if (is_tls_call && last_tls != EXPECTED)
	return last_tls;
      else if (!is_tls_call && last_tls != NOT_EXPECTED)
	{
	  this->missing();
	  return EXPECTED;
	}
      return NORMAL;
    }

  private:
    // What we're up to regarding calls to __tls_get_addr.
    // On powerpc, the branch and link insn making a call to
    // __tls_get_addr is marked with a relocation, R_PPC64_TLSGD,
    // R_PPC64_TLSLD, R_PPC_TLSGD or R_PPC_TLSLD, in addition to the
    // usual R_POWERPC_REL24 or R_PPC_PLTREL25 relocation on a call.
    // The marker relocation always comes first, and has the same
    // symbol as the reloc on the insn setting up the __tls_get_addr
    // argument.  This ties the arg setup insn with the call insn,
    // allowing ld to safely optimize away the call.  We check that
    // every call to __tls_get_addr has a marker relocation, and that
    // every marker relocation is on a call to __tls_get_addr.
    Tls_get_addr tls_get_addr_;
    // Info about the last reloc for error message.
    const Relocate_info<size, big_endian>* relinfo_;
    size_t relnum_;
    Address r_offset_;
  };

  // The class which scans relocations.
  class Scan : protected Track_tls
  {
  public:
    typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;

    Scan()
      : Track_tls(), issued_non_pic_error_(false)
    { }

    static inline int
    get_reference_flags(unsigned int r_type);

    inline void
    local(Symbol_table* symtab, Layout* layout, Target_powerpc* target,
	  Sized_relobj_file<size, big_endian>* object,
	  unsigned int data_shndx,
	  Output_section* output_section,
	  const elfcpp::Rela<size, big_endian>& reloc, unsigned int r_type,
	  const elfcpp::Sym<size, big_endian>& lsym,
	  bool is_discarded);

    inline void
    global(Symbol_table* symtab, Layout* layout, Target_powerpc* target,
	   Sized_relobj_file<size, big_endian>* object,
	   unsigned int data_shndx,
	   Output_section* output_section,
	   const elfcpp::Rela<size, big_endian>& reloc, unsigned int r_type,
	   Symbol* gsym);

    inline bool
    local_reloc_may_be_function_pointer(Symbol_table* , Layout* ,
					Target_powerpc* ,
					Sized_relobj_file<size, big_endian>* ,
					unsigned int ,
					Output_section* ,
					const elfcpp::Rela<size, big_endian>& ,
					unsigned int r_type,
					const elfcpp::Sym<size, big_endian>&)
    {
      // PowerPC64 .opd is not folded, so any identical function text
      // may be folded and we'll still keep function addresses distinct.
      // That means no reloc is of concern here.
      if (size == 64)
	return false;
      // For 32-bit, conservatively assume anything but calls to
      // function code might be taking the address of the function.
      return !is_branch_reloc(r_type);
    }

    inline bool
    global_reloc_may_be_function_pointer(Symbol_table* , Layout* ,
					 Target_powerpc* ,
					 Sized_relobj_file<size, big_endian>* ,
					 unsigned int ,
					 Output_section* ,
					 const elfcpp::Rela<size, big_endian>& ,
					 unsigned int r_type,
					 Symbol*)
    {
      // As above.
      if (size == 64)
	return false;
      return !is_branch_reloc(r_type);
    }

    static bool
    reloc_needs_plt_for_ifunc(Sized_relobj_file<size, big_endian>* object,
			      unsigned int r_type, bool report_err);

  private:
    static void
    unsupported_reloc_local(Sized_relobj_file<size, big_endian>*,
			    unsigned int r_type);

    static void
    unsupported_reloc_global(Sized_relobj_file<size, big_endian>*,
			     unsigned int r_type, Symbol*);

    static void
    generate_tls_call(Symbol_table* symtab, Layout* layout,
		      Target_powerpc* target);

    void
    check_non_pic(Relobj*, unsigned int r_type);

    // Whether we have issued an error about a non-PIC compilation.
    bool issued_non_pic_error_;
  };

  Address
  symval_for_branch(const Symbol_table* symtab, Address value,
		    const Sized_symbol<size>* gsym,
		    Powerpc_relobj<size, big_endian>* object,
		    unsigned int *dest_shndx);

  // The class which implements relocation.
  class Relocate : protected Track_tls
  {
   public:
    // Use 'at' branch hints when true, 'y' when false.
    // FIXME maybe: set this with an option.
    static const bool is_isa_v2 = true;

    Relocate()
      : Track_tls()
    { }

    // Do a relocation.  Return false if the caller should not issue
    // any warnings about this relocation.
    inline bool
    relocate(const Relocate_info<size, big_endian>*, Target_powerpc*,
	     Output_section*, size_t relnum,
	     const elfcpp::Rela<size, big_endian>&,
	     unsigned int r_type, const Sized_symbol<size>*,
	     const Symbol_value<size>*,
	     unsigned char*,
	     typename elfcpp::Elf_types<size>::Elf_Addr,
	     section_size_type);
  };

  class Relocate_comdat_behavior
  {
   public:
    // Decide what the linker should do for relocations that refer to
    // discarded comdat sections.
    inline Comdat_behavior
    get(const char* name)
    {
      gold::Default_comdat_behavior default_behavior;
      Comdat_behavior ret = default_behavior.get(name);
      if (ret == CB_WARNING)
	{
	  if (size == 32
	      && (strcmp(name, ".fixup") == 0
		  || strcmp(name, ".got2") == 0))
	    ret = CB_IGNORE;
	  if (size == 64
	      && (strcmp(name, ".opd") == 0
		  || strcmp(name, ".toc") == 0
		  || strcmp(name, ".toc1") == 0))
	    ret = CB_IGNORE;
	}
      return ret;
    }
  };

  // A class which returns the size required for a relocation type,
  // used while scanning relocs during a relocatable link.
  class Relocatable_size_for_reloc
  {
   public:
    unsigned int
    get_size_for_reloc(unsigned int, Relobj*)
    {
      gold_unreachable();
      return 0;
    }
  };

  // Optimize the TLS relocation type based on what we know about the
  // symbol.  IS_FINAL is true if the final address of this symbol is
  // known at link time.

  tls::Tls_optimization
  optimize_tls_gd(bool is_final)
  {
    // If we are generating a shared library, then we can't do anything
    // in the linker.
    if (parameters->options().shared())
      return tls::TLSOPT_NONE;

    if (!is_final)
      return tls::TLSOPT_TO_IE;
    return tls::TLSOPT_TO_LE;
  }

  tls::Tls_optimization
  optimize_tls_ld()
  {
    if (parameters->options().shared())
      return tls::TLSOPT_NONE;

    return tls::TLSOPT_TO_LE;
  }

  tls::Tls_optimization
  optimize_tls_ie(bool is_final)
  {
    if (!is_final || parameters->options().shared())
      return tls::TLSOPT_NONE;

    return tls::TLSOPT_TO_LE;
  }

  // Create glink.
  void
  make_glink_section(Layout*);

  // Create the PLT section.
  void
  make_plt_section(Symbol_table*, Layout*);

  void
  make_iplt_section(Symbol_table*, Layout*);

  void
  make_brlt_section(Layout*);

  // Create a PLT entry for a global symbol.
  void
  make_plt_entry(Symbol_table*, Layout*, Symbol*);

  // Create a PLT entry for a local IFUNC symbol.
  void
  make_local_ifunc_plt_entry(Symbol_table*, Layout*,
			     Sized_relobj_file<size, big_endian>*,
			     unsigned int);


  // Create a GOT entry for local dynamic __tls_get_addr.
  unsigned int
  tlsld_got_offset(Symbol_table* symtab, Layout* layout,
		   Sized_relobj_file<size, big_endian>* object);

  unsigned int
  tlsld_got_offset() const
  {
    return this->tlsld_got_offset_;
  }

  // Get the dynamic reloc section, creating it if necessary.
  Reloc_section*
  rela_dyn_section(Layout*);

  // Similarly, but for ifunc symbols get the one for ifunc.
  Reloc_section*
  rela_dyn_section(Symbol_table*, Layout*, bool for_ifunc);

  // Copy a relocation against a global symbol.
  void
  copy_reloc(Symbol_table* symtab, Layout* layout,
	     Sized_relobj_file<size, big_endian>* object,
	     unsigned int shndx, Output_section* output_section,
	     Symbol* sym, const elfcpp::Rela<size, big_endian>& reloc)
  {
    this->copy_relocs_.copy_reloc(symtab, layout,
				  symtab->get_sized_symbol<size>(sym),
				  object, shndx, output_section,
				  reloc, this->rela_dyn_section(layout));
  }

  // Look over all the input sections, deciding where to place stubs.
  void
  group_sections(Layout*, const Task*);

  // Sort output sections by address.
  struct Sort_sections
  {
    bool
    operator()(const Output_section* sec1, const Output_section* sec2)
    { return sec1->address() < sec2->address(); }
  };

  class Branch_info
  {
   public:
    Branch_info(Powerpc_relobj<size, big_endian>* ppc_object,
		unsigned int data_shndx,
		Address r_offset,
		unsigned int r_type,
		unsigned int r_sym,
		Address addend)
      : object_(ppc_object), shndx_(data_shndx), offset_(r_offset),
	r_type_(r_type), r_sym_(r_sym), addend_(addend)
    { }

    ~Branch_info()
    { }

    // If this branch needs a plt call stub, or a long branch stub, make one.
    void
    make_stub(Stub_table<size, big_endian>*,
	      Stub_table<size, big_endian>*,
	      Symbol_table*) const;

   private:
    // The branch location..
    Powerpc_relobj<size, big_endian>* object_;
    unsigned int shndx_;
    Address offset_;
    // ..and the branch type and destination.
    unsigned int r_type_;
    unsigned int r_sym_;
    Address addend_;
  };

  // Information about this specific target which we pass to the
  // general Target structure.
  static Target::Target_info powerpc_info;

  // The types of GOT entries needed for this platform.
  // These values are exposed to the ABI in an incremental link.
  // Do not renumber existing values without changing the version
  // number of the .gnu_incremental_inputs section.
  enum Got_type
  {
    GOT_TYPE_STANDARD,
    GOT_TYPE_TLSGD,	// double entry for @got@tlsgd
    GOT_TYPE_DTPREL,	// entry for @got@dtprel
    GOT_TYPE_TPREL	// entry for @got@tprel
  };

  // The GOT section.
  Output_data_got_powerpc<size, big_endian>* got_;
  // The PLT section.  This is a container for a table of addresses,
  // and their relocations.  Each address in the PLT has a dynamic
  // relocation (R_*_JMP_SLOT) and each address will have a
  // corresponding entry in .glink for lazy resolution of the PLT.
  // ppc32 initialises the PLT to point at the .glink entry, while
  // ppc64 leaves this to ld.so.  To make a call via the PLT, the
  // linker adds a stub that loads the PLT entry into ctr then
  // branches to ctr.  There may be more than one stub for each PLT
  // entry.  DT_JMPREL points at the first PLT dynamic relocation and
  // DT_PLTRELSZ gives the total size of PLT dynamic relocations.
  Output_data_plt_powerpc<size, big_endian>* plt_;
  // The IPLT section.  Like plt_, this is a container for a table of
  // addresses and their relocations, specifically for STT_GNU_IFUNC
  // functions that resolve locally (STT_GNU_IFUNC functions that
  // don't resolve locally go in PLT).  Unlike plt_, these have no
  // entry in .glink for lazy resolution, and the relocation section
  // does not have a 1-1 correspondence with IPLT addresses.  In fact,
  // the relocation section may contain relocations against
  // STT_GNU_IFUNC symbols at locations outside of IPLT.  The
  // relocation section will appear at the end of other dynamic
  // relocations, so that ld.so applies these relocations after other
  // dynamic relocations.  In a static executable, the relocation
  // section is emitted and marked with __rela_iplt_start and
  // __rela_iplt_end symbols.
  Output_data_plt_powerpc<size, big_endian>* iplt_;
  // Section holding long branch destinations.
  Output_data_brlt_powerpc<size, big_endian>* brlt_section_;
  // The .glink section.
  Output_data_glink<size, big_endian>* glink_;
  // The dynamic reloc section.
  Reloc_section* rela_dyn_;
  // Relocs saved to avoid a COPY reloc.
  Copy_relocs<elfcpp::SHT_RELA, size, big_endian> copy_relocs_;
  // Offset of the GOT entry for local dynamic __tls_get_addr calls.
  unsigned int tlsld_got_offset_;

  Stub_tables stub_tables_;
  typedef Unordered_map<Address, unsigned int> Branch_lookup_table;
  Branch_lookup_table branch_lookup_table_;

  typedef std::vector<Branch_info> Branches;
  Branches branch_info_;

  bool plt_thread_safe_;
};

template<>
Target::Target_info Target_powerpc<32, true>::powerpc_info =
{
  32,			// size
  true,			// is_big_endian
  elfcpp::EM_PPC,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/usr/lib/ld.so.1",	// dynamic_linker
  0x10000000,		// default_text_segment_address
  64 * 1024,		// abi_pagesize (overridable by -z max-page-size)
  4 * 1024,		// common_pagesize (overridable by -z common-page-size)
  false,		// isolate_execinstr
  0,			// rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL,			// attributes_vendor
  "_start"		// entry_symbol_name
};

template<>
Target::Target_info Target_powerpc<32, false>::powerpc_info =
{
  32,			// size
  false,		// is_big_endian
  elfcpp::EM_PPC,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/usr/lib/ld.so.1",	// dynamic_linker
  0x10000000,		// default_text_segment_address
  64 * 1024,		// abi_pagesize (overridable by -z max-page-size)
  4 * 1024,		// common_pagesize (overridable by -z common-page-size)
  false,		// isolate_execinstr
  0,			// rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL,			// attributes_vendor
  "_start"		// entry_symbol_name
};

template<>
Target::Target_info Target_powerpc<64, true>::powerpc_info =
{
  64,			// size
  true,			// is_big_endian
  elfcpp::EM_PPC64,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/usr/lib/ld.so.1",	// dynamic_linker
  0x10000000,		// default_text_segment_address
  64 * 1024,		// abi_pagesize (overridable by -z max-page-size)
  4 * 1024,		// common_pagesize (overridable by -z common-page-size)
  false,		// isolate_execinstr
  0,			// rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL,			// attributes_vendor
  "_start"		// entry_symbol_name
};

template<>
Target::Target_info Target_powerpc<64, false>::powerpc_info =
{
  64,			// size
  false,		// is_big_endian
  elfcpp::EM_PPC64,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  false,		// has_code_fill
  true,			// is_default_stack_executable
  false,		// can_icf_inline_merge_sections
  '\0',			// wrap_char
  "/usr/lib/ld.so.1",	// dynamic_linker
  0x10000000,		// default_text_segment_address
  64 * 1024,		// abi_pagesize (overridable by -z max-page-size)
  4 * 1024,		// common_pagesize (overridable by -z common-page-size)
  false,		// isolate_execinstr
  0,			// rosegment_gap
  elfcpp::SHN_UNDEF,	// small_common_shndx
  elfcpp::SHN_UNDEF,	// large_common_shndx
  0,			// small_common_section_flags
  0,			// large_common_section_flags
  NULL,			// attributes_section
  NULL,			// attributes_vendor
  "_start"		// entry_symbol_name
};

inline bool
is_branch_reloc(unsigned int r_type)
{
  return (r_type == elfcpp::R_POWERPC_REL24
	  || r_type == elfcpp::R_PPC_PLTREL24
	  || r_type == elfcpp::R_PPC_LOCAL24PC
	  || r_type == elfcpp::R_POWERPC_REL14
	  || r_type == elfcpp::R_POWERPC_REL14_BRTAKEN
	  || r_type == elfcpp::R_POWERPC_REL14_BRNTAKEN
	  || r_type == elfcpp::R_POWERPC_ADDR24
	  || r_type == elfcpp::R_POWERPC_ADDR14
	  || r_type == elfcpp::R_POWERPC_ADDR14_BRTAKEN
	  || r_type == elfcpp::R_POWERPC_ADDR14_BRNTAKEN);
}

// If INSN is an opcode that may be used with an @tls operand, return
// the transformed insn for TLS optimisation, otherwise return 0.  If
// REG is non-zero only match an insn with RB or RA equal to REG.
uint32_t
at_tls_transform(uint32_t insn, unsigned int reg)
{
  if ((insn & (0x3f << 26)) != 31 << 26)
    return 0;

  unsigned int rtra;
  if (reg == 0 || ((insn >> 11) & 0x1f) == reg)
    rtra = insn & ((1 << 26) - (1 << 16));
  else if (((insn >> 16) & 0x1f) == reg)
    rtra = (insn & (0x1f << 21)) | ((insn & (0x1f << 11)) << 5);
  else
    return 0;

  if ((insn & (0x3ff << 1)) == 266 << 1)
    // add -> addi
    insn = 14 << 26;
  else if ((insn & (0x1f << 1)) == 23 << 1
	   && ((insn & (0x1f << 6)) < 14 << 6
	       || ((insn & (0x1f << 6)) >= 16 << 6
		   && (insn & (0x1f << 6)) < 24 << 6)))
    // load and store indexed -> dform
    insn = (32 | ((insn >> 6) & 0x1f)) << 26;
  else if ((insn & (((0x1a << 5) | 0x1f) << 1)) == 21 << 1)
    // ldx, ldux, stdx, stdux -> ld, ldu, std, stdu
    insn = ((58 | ((insn >> 6) & 4)) << 26) | ((insn >> 6) & 1);
  else if ((insn & (((0x1f << 5) | 0x1f) << 1)) == 341 << 1)
    // lwax -> lwa
    insn = (58 << 26) | 2;
  else
    return 0;
  insn |= rtra;
  return insn;
}

// Modified version of symtab.h class Symbol member
// Given a direct absolute or pc-relative static relocation against
// the global symbol, this function returns whether a dynamic relocation
// is needed.

template<int size>
bool
needs_dynamic_reloc(const Symbol* gsym, int flags)
{
  // No dynamic relocations in a static link!
  if (parameters->doing_static_link())
    return false;

  // A reference to an undefined symbol from an executable should be
  // statically resolved to 0, and does not need a dynamic relocation.
  // This matches gnu ld behavior.
  if (gsym->is_undefined() && !parameters->options().shared())
    return false;

  // A reference to an absolute symbol does not need a dynamic relocation.
  if (gsym->is_absolute())
    return false;

  // An absolute reference within a position-independent output file
  // will need a dynamic relocation.
  if ((flags & Symbol::ABSOLUTE_REF)
      && parameters->options().output_is_position_independent())
    return true;

  // A function call that can branch to a local PLT entry does not need
  // a dynamic relocation.
  if ((flags & Symbol::FUNCTION_CALL) && gsym->has_plt_offset())
    return false;

  // A reference to any PLT entry in a non-position-independent executable
  // does not need a dynamic relocation.
  // Except due to having function descriptors on powerpc64 we don't define
  // functions to their plt code in an executable, so this doesn't apply.
  if (size == 32
      && !parameters->options().output_is_position_independent()
      && gsym->has_plt_offset())
    return false;

  // A reference to a symbol defined in a dynamic object or to a
  // symbol that is preemptible will need a dynamic relocation.
  if (gsym->is_from_dynobj()
      || gsym->is_undefined()
      || gsym->is_preemptible())
    return true;

  // For all other cases, return FALSE.
  return false;
}

// Modified version of symtab.h class Symbol member
// Whether we should use the PLT offset associated with a symbol for
// a relocation.  FLAGS is a set of Reference_flags.

template<int size>
bool
use_plt_offset(const Symbol* gsym, int flags)
{
  // If the symbol doesn't have a PLT offset, then naturally we
  // don't want to use it.
  if (!gsym->has_plt_offset())
    return false;

  // For a STT_GNU_IFUNC symbol we always have to use the PLT entry.
  if (gsym->type() == elfcpp::STT_GNU_IFUNC)
    return true;

  // If we are going to generate a dynamic relocation, then we will
  // wind up using that, so no need to use the PLT entry.
  if (needs_dynamic_reloc<size>(gsym, flags))
    return false;

  // If the symbol is from a dynamic object, we need to use the PLT
  // entry.
  if (gsym->is_from_dynobj())
    return true;

  // If we are generating a shared object, and this symbol is
  // undefined or preemptible, we need to use the PLT entry.
  if (parameters->options().shared()
      && (gsym->is_undefined() || gsym->is_preemptible()))
    return true;

  // If this is a call to a weak undefined symbol, we need to use
  // the PLT entry; the symbol may be defined by a library loaded
  // at runtime.
  if ((flags & Symbol::FUNCTION_CALL) && gsym->is_weak_undefined())
    return true;

  // Otherwise we can use the regular definition.
  return false;
}

template<int size, bool big_endian>
class Powerpc_relocate_functions
{
public:
  enum Overflow_check
  {
    CHECK_NONE,
    CHECK_SIGNED,
    CHECK_BITFIELD
  };

  enum Status
  {
    STATUS_OK,
    STATUS_OVERFLOW
  };

private:
  typedef Powerpc_relocate_functions<size, big_endian> This;
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;

  template<int valsize>
  static inline bool
  has_overflow_signed(Address value)
  {
    // limit = 1 << (valsize - 1) without shift count exceeding size of type
    Address limit = static_cast<Address>(1) << ((valsize - 1) >> 1);
    limit <<= ((valsize - 1) >> 1);
    limit <<= ((valsize - 1) - 2 * ((valsize - 1) >> 1));
    return value + limit > (limit << 1) - 1;
  }

  template<int valsize>
  static inline bool
  has_overflow_bitfield(Address value)
  {
    Address limit = static_cast<Address>(1) << ((valsize - 1) >> 1);
    limit <<= ((valsize - 1) >> 1);
    limit <<= ((valsize - 1) - 2 * ((valsize - 1) >> 1));
    return value > (limit << 1) - 1 && value + limit > (limit << 1) - 1;
  }

  template<int valsize>
  static inline Status
  overflowed(Address value, Overflow_check overflow)
  {
    if (overflow == CHECK_SIGNED)
      {
	if (has_overflow_signed<valsize>(value))
	  return STATUS_OVERFLOW;
      }
    else if (overflow == CHECK_BITFIELD)
      {
	if (has_overflow_bitfield<valsize>(value))
	  return STATUS_OVERFLOW;
      }
    return STATUS_OK;
  }

  // Do a simple RELA relocation
  template<int valsize>
  static inline Status
  rela(unsigned char* view, Address value, Overflow_check overflow)
  {
    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    elfcpp::Swap<valsize, big_endian>::writeval(wv, value);
    return overflowed<valsize>(value, overflow);
  }

  template<int valsize>
  static inline Status
  rela(unsigned char* view,
       unsigned int right_shift,
       typename elfcpp::Valtype_base<valsize>::Valtype dst_mask,
       Address value,
       Overflow_check overflow)
  {
    typedef typename elfcpp::Swap<valsize, big_endian>::Valtype Valtype;
    Valtype* wv = reinterpret_cast<Valtype*>(view);
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);
    Valtype reloc = value >> right_shift;
    val &= ~dst_mask;
    reloc &= dst_mask;
    elfcpp::Swap<valsize, big_endian>::writeval(wv, val | reloc);
    return overflowed<valsize>(value >> right_shift, overflow);
  }

  // Do a simple RELA relocation, unaligned.
  template<int valsize>
  static inline Status
  rela_ua(unsigned char* view, Address value, Overflow_check overflow)
  {
    elfcpp::Swap_unaligned<valsize, big_endian>::writeval(view, value);
    return overflowed<valsize>(value, overflow);
  }

  template<int valsize>
  static inline Status
  rela_ua(unsigned char* view,
	  unsigned int right_shift,
	  typename elfcpp::Valtype_base<valsize>::Valtype dst_mask,
	  Address value,
	  Overflow_check overflow)
  {
    typedef typename elfcpp::Swap_unaligned<valsize, big_endian>::Valtype
      Valtype;
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(view);
    Valtype reloc = value >> right_shift;
    val &= ~dst_mask;
    reloc &= dst_mask;
    elfcpp::Swap_unaligned<valsize, big_endian>::writeval(view, val | reloc);
    return overflowed<valsize>(value >> right_shift, overflow);
  }

public:
  // R_PPC64_ADDR64: (Symbol + Addend)
  static inline void
  addr64(unsigned char* view, Address value)
  { This::template rela<64>(view, value, CHECK_NONE); }

  // R_PPC64_UADDR64: (Symbol + Addend) unaligned
  static inline void
  addr64_u(unsigned char* view, Address value)
  { This::template rela_ua<64>(view, value, CHECK_NONE); }

  // R_POWERPC_ADDR32: (Symbol + Addend)
  static inline Status
  addr32(unsigned char* view, Address value, Overflow_check overflow)
  { return This::template rela<32>(view, value, overflow); }

  // R_POWERPC_UADDR32: (Symbol + Addend) unaligned
  static inline Status
  addr32_u(unsigned char* view, Address value, Overflow_check overflow)
  { return This::template rela_ua<32>(view, value, overflow); }

  // R_POWERPC_ADDR24: (Symbol + Addend) & 0x3fffffc
  static inline Status
  addr24(unsigned char* view, Address value, Overflow_check overflow)
  {
    Status stat = This::template rela<32>(view, 0, 0x03fffffc, value, overflow);
    if (overflow != CHECK_NONE && (value & 3) != 0)
      stat = STATUS_OVERFLOW;
    return stat;
  }

  // R_POWERPC_ADDR16: (Symbol + Addend) & 0xffff
  static inline Status
  addr16(unsigned char* view, Address value, Overflow_check overflow)
  { return This::template rela<16>(view, value, overflow); }

  // R_POWERPC_ADDR16: (Symbol + Addend) & 0xffff, unaligned
  static inline Status
  addr16_u(unsigned char* view, Address value, Overflow_check overflow)
  { return This::template rela_ua<16>(view, value, overflow); }

  // R_POWERPC_ADDR16_DS: (Symbol + Addend) & 0xfffc
  static inline Status
  addr16_ds(unsigned char* view, Address value, Overflow_check overflow)
  {
    Status stat = This::template rela<16>(view, 0, 0xfffc, value, overflow);
    if (overflow != CHECK_NONE && (value & 3) != 0)
      stat = STATUS_OVERFLOW;
    return stat;
  }

  // R_POWERPC_ADDR16_HI: ((Symbol + Addend) >> 16) & 0xffff
  static inline void
  addr16_hi(unsigned char* view, Address value)
  { This::template rela<16>(view, 16, 0xffff, value, CHECK_NONE); }

  // R_POWERPC_ADDR16_HA: ((Symbol + Addend + 0x8000) >> 16) & 0xffff
  static inline void
  addr16_ha(unsigned char* view, Address value)
  { This::addr16_hi(view, value + 0x8000); }

  // R_POWERPC_ADDR16_HIGHER: ((Symbol + Addend) >> 32) & 0xffff
  static inline void
  addr16_hi2(unsigned char* view, Address value)
  { This::template rela<16>(view, 32, 0xffff, value, CHECK_NONE); }

  // R_POWERPC_ADDR16_HIGHERA: ((Symbol + Addend + 0x8000) >> 32) & 0xffff
  static inline void
  addr16_ha2(unsigned char* view, Address value)
  { This::addr16_hi2(view, value + 0x8000); }

  // R_POWERPC_ADDR16_HIGHEST: ((Symbol + Addend) >> 48) & 0xffff
  static inline void
  addr16_hi3(unsigned char* view, Address value)
  { This::template rela<16>(view, 48, 0xffff, value, CHECK_NONE); }

  // R_POWERPC_ADDR16_HIGHESTA: ((Symbol + Addend + 0x8000) >> 48) & 0xffff
  static inline void
  addr16_ha3(unsigned char* view, Address value)
  { This::addr16_hi3(view, value + 0x8000); }

  // R_POWERPC_ADDR14: (Symbol + Addend) & 0xfffc
  static inline Status
  addr14(unsigned char* view, Address value, Overflow_check overflow)
  {
    Status stat = This::template rela<32>(view, 0, 0xfffc, value, overflow);
    if (overflow != CHECK_NONE && (value & 3) != 0)
      stat = STATUS_OVERFLOW;
    return stat;
  }
};

// Stash away the index of .got2 or .opd in a relocatable object, if
// such a section exists.

template<int size, bool big_endian>
bool
Powerpc_relobj<size, big_endian>::do_find_special_sections(
    Read_symbols_data* sd)
{
  const unsigned char* const pshdrs = sd->section_headers->data();
  const unsigned char* namesu = sd->section_names->data();
  const char* names = reinterpret_cast<const char*>(namesu);
  section_size_type names_size = sd->section_names_size;
  const unsigned char* s;

  s = this->template find_shdr<size, big_endian>(pshdrs,
						 size == 32 ? ".got2" : ".opd",
						 names, names_size, NULL);
  if (s != NULL)
    {
      unsigned int ndx = (s - pshdrs) / elfcpp::Elf_sizes<size>::shdr_size;
      this->special_ = ndx;
    }
  return Sized_relobj_file<size, big_endian>::do_find_special_sections(sd);
}

// Examine .rela.opd to build info about function entry points.

template<int size, bool big_endian>
void
Powerpc_relobj<size, big_endian>::scan_opd_relocs(
    size_t reloc_count,
    const unsigned char* prelocs,
    const unsigned char* plocal_syms)
{
  if (size == 64)
    {
      typedef typename Reloc_types<elfcpp::SHT_RELA, size, big_endian>::Reloc
	Reltype;
      const int reloc_size
	= Reloc_types<elfcpp::SHT_RELA, size, big_endian>::reloc_size;
      const int sym_size = elfcpp::Elf_sizes<size>::sym_size;
      Address expected_off = 0;
      bool regular = true;
      unsigned int opd_ent_size = 0;

      for (size_t i = 0; i < reloc_count; ++i, prelocs += reloc_size)
	{
	  Reltype reloc(prelocs);
	  typename elfcpp::Elf_types<size>::Elf_WXword r_info
	    = reloc.get_r_info();
	  unsigned int r_type = elfcpp::elf_r_type<size>(r_info);
	  if (r_type == elfcpp::R_PPC64_ADDR64)
	    {
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(r_info);
	      typename elfcpp::Elf_types<size>::Elf_Addr value;
	      bool is_ordinary;
	      unsigned int shndx;
	      if (r_sym < this->local_symbol_count())
		{
		  typename elfcpp::Sym<size, big_endian>
		    lsym(plocal_syms + r_sym * sym_size);
		  shndx = lsym.get_st_shndx();
		  shndx = this->adjust_sym_shndx(r_sym, shndx, &is_ordinary);
		  value = lsym.get_st_value();
		}
	      else
		shndx = this->symbol_section_and_value(r_sym, &value,
						       &is_ordinary);
	      this->set_opd_ent(reloc.get_r_offset(), shndx,
				value + reloc.get_r_addend());
	      if (i == 2)
		{
		  expected_off = reloc.get_r_offset();
		  opd_ent_size = expected_off;
		}
	      else if (expected_off != reloc.get_r_offset())
		regular = false;
	      expected_off += opd_ent_size;
	    }
	  else if (r_type == elfcpp::R_PPC64_TOC)
	    {
	      if (expected_off - opd_ent_size + 8 != reloc.get_r_offset())
		regular = false;
	    }
	  else
	    {
	      gold_warning(_("%s: unexpected reloc type %u in .opd section"),
			   this->name().c_str(), r_type);
	      regular = false;
	    }
	}
      if (reloc_count <= 2)
	opd_ent_size = this->section_size(this->opd_shndx());
      if (opd_ent_size != 24 && opd_ent_size != 16)
	regular = false;
      if (!regular)
	{
	  gold_warning(_("%s: .opd is not a regular array of opd entries"),
		       this->name().c_str());
	  opd_ent_size = 0;
	}
    }
}

template<int size, bool big_endian>
void
Powerpc_relobj<size, big_endian>::do_read_relocs(Read_relocs_data* rd)
{
  Sized_relobj_file<size, big_endian>::do_read_relocs(rd);
  if (size == 64)
    {
      for (Read_relocs_data::Relocs_list::iterator p = rd->relocs.begin();
	   p != rd->relocs.end();
	   ++p)
	{
	  if (p->data_shndx == this->opd_shndx())
	    {
	      uint64_t opd_size = this->section_size(this->opd_shndx());
	      gold_assert(opd_size == static_cast<size_t>(opd_size));
	      if (opd_size != 0)
		{
		  this->init_opd(opd_size);
		  this->scan_opd_relocs(p->reloc_count, p->contents->data(),
					rd->local_symbols->data());
		}
	      break;
	    }
	}
    }
}

// Call Sized_dynobj::do_read_symbols to read the symbols then
// read .opd from a dynamic object, filling in opd_ent_ vector,

template<int size, bool big_endian>
void
Powerpc_dynobj<size, big_endian>::do_read_symbols(Read_symbols_data* sd)
{
  Sized_dynobj<size, big_endian>::do_read_symbols(sd);
  if (size == 64)
    {
      const int shdr_size = elfcpp::Elf_sizes<size>::shdr_size;
      const unsigned char* const pshdrs = sd->section_headers->data();
      const unsigned char* namesu = sd->section_names->data();
      const char* names = reinterpret_cast<const char*>(namesu);
      const unsigned char* s = NULL;
      const unsigned char* opd;
      section_size_type opd_size;

      // Find and read .opd section.
      while (1)
	{
	  s = this->template find_shdr<size, big_endian>(pshdrs, ".opd", names,
							 sd->section_names_size,
							 s);
	  if (s == NULL)
	    return;

	  typename elfcpp::Shdr<size, big_endian> shdr(s);
	  if (shdr.get_sh_type() == elfcpp::SHT_PROGBITS
	      && (shdr.get_sh_flags() & elfcpp::SHF_ALLOC) != 0)
	    {
	      this->opd_shndx_ = (s - pshdrs) / shdr_size;
	      this->opd_address_ = shdr.get_sh_addr();
	      opd_size = convert_to_section_size_type(shdr.get_sh_size());
	      opd = this->get_view(shdr.get_sh_offset(), opd_size,
				   true, false);
	      break;
	    }
	}

      // Build set of executable sections.
      // Using a set is probably overkill.  There is likely to be only
      // a few executable sections, typically .init, .text and .fini,
      // and they are generally grouped together.
      typedef std::set<Sec_info> Exec_sections;
      Exec_sections exec_sections;
      s = pshdrs;
      for (unsigned int i = 1; i < this->shnum(); ++i, s += shdr_size)
	{
	  typename elfcpp::Shdr<size, big_endian> shdr(s);
	  if (shdr.get_sh_type() == elfcpp::SHT_PROGBITS
	      && ((shdr.get_sh_flags()
		   & (elfcpp::SHF_ALLOC | elfcpp::SHF_EXECINSTR))
		  == (elfcpp::SHF_ALLOC | elfcpp::SHF_EXECINSTR))
	      && shdr.get_sh_size() != 0)
	    {
	      exec_sections.insert(Sec_info(shdr.get_sh_addr(),
					    shdr.get_sh_size(), i));
	    }
	}
      if (exec_sections.empty())
	return;

      // Look over the OPD entries.  This is complicated by the fact
      // that some binaries will use two-word entries while others
      // will use the standard three-word entries.  In most cases
      // the third word (the environment pointer for languages like
      // Pascal) is unused and will be zero.  If the third word is
      // used it should not be pointing into executable sections,
      // I think.
      this->init_opd(opd_size);
      for (const unsigned char* p = opd; p < opd + opd_size; p += 8)
	{
	  typedef typename elfcpp::Swap<64, big_endian>::Valtype Valtype;
	  const Valtype* valp = reinterpret_cast<const Valtype*>(p);
	  Valtype val = elfcpp::Swap<64, big_endian>::readval(valp);
	  if (val == 0)
	    // Chances are that this is the third word of an OPD entry.
	    continue;
	  typename Exec_sections::const_iterator e
	    = exec_sections.upper_bound(Sec_info(val, 0, 0));
	  if (e != exec_sections.begin())
	    {
	      --e;
	      if (e->start <= val && val < e->start + e->len)
		{
		  // We have an address in an executable section.
		  // VAL ought to be the function entry, set it up.
		  this->set_opd_ent(p - opd, e->shndx, val);
		  // Skip second word of OPD entry, the TOC pointer.
		  p += 8;
		}
	    }
	  // If we didn't match any executable sections, we likely
	  // have a non-zero third word in the OPD entry.
	}
    }
}

// Set up some symbols.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::do_define_standard_symbols(
    Symbol_table* symtab,
    Layout* layout)
{
  if (size == 32)
    {
      // Define _GLOBAL_OFFSET_TABLE_ to ensure it isn't seen as
      // undefined when scanning relocs (and thus requires
      // non-relative dynamic relocs).  The proper value will be
      // updated later.
      Symbol *gotsym = symtab->lookup("_GLOBAL_OFFSET_TABLE_", NULL);
      if (gotsym != NULL && gotsym->is_undefined())
	{
	  Target_powerpc<size, big_endian>* target =
	    static_cast<Target_powerpc<size, big_endian>*>(
		parameters->sized_target<size, big_endian>());
	  Output_data_got_powerpc<size, big_endian>* got
	    = target->got_section(symtab, layout);
	  symtab->define_in_output_data("_GLOBAL_OFFSET_TABLE_", NULL,
					Symbol_table::PREDEFINED,
					got, 0, 0,
					elfcpp::STT_OBJECT,
					elfcpp::STB_LOCAL,
					elfcpp::STV_HIDDEN, 0,
					false, false);
	}

      // Define _SDA_BASE_ at the start of the .sdata section + 32768.
      Symbol *sdasym = symtab->lookup("_SDA_BASE_", NULL);
      if (sdasym != NULL && sdasym->is_undefined())
	{
	  Output_data_space* sdata = new Output_data_space(4, "** sdata");
	  Output_section* os
	    = layout->add_output_section_data(".sdata", 0,
					      elfcpp::SHF_ALLOC
					      | elfcpp::SHF_WRITE,
					      sdata, ORDER_SMALL_DATA, false);
	  symtab->define_in_output_data("_SDA_BASE_", NULL,
					Symbol_table::PREDEFINED,
					os, 32768, 0, elfcpp::STT_OBJECT,
					elfcpp::STB_LOCAL, elfcpp::STV_HIDDEN,
					0, false, false);
	}
    }
}

// Set up PowerPC target specific relobj.

template<int size, bool big_endian>
Object*
Target_powerpc<size, big_endian>::do_make_elf_object(
    const std::string& name,
    Input_file* input_file,
    off_t offset, const elfcpp::Ehdr<size, big_endian>& ehdr)
{
  int et = ehdr.get_e_type();
  // ET_EXEC files are valid input for --just-symbols/-R,
  // and we treat them as relocatable objects.
  if (et == elfcpp::ET_REL
      || (et == elfcpp::ET_EXEC && input_file->just_symbols()))
    {
      Powerpc_relobj<size, big_endian>* obj =
	new Powerpc_relobj<size, big_endian>(name, input_file, offset, ehdr);
      obj->setup();
      return obj;
    }
  else if (et == elfcpp::ET_DYN)
    {
      Powerpc_dynobj<size, big_endian>* obj =
	new Powerpc_dynobj<size, big_endian>(name, input_file, offset, ehdr);
      obj->setup();
      return obj;
    }
  else
    {
      gold_error(_("%s: unsupported ELF file type %d"), name.c_str(), et);
      return NULL;
    }
}

template<int size, bool big_endian>
class Output_data_got_powerpc : public Output_data_got<size, big_endian>
{
public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Valtype;
  typedef Output_data_reloc<elfcpp::SHT_RELA, true, size, big_endian> Rela_dyn;

  Output_data_got_powerpc(Symbol_table* symtab, Layout* layout)
    : Output_data_got<size, big_endian>(),
      symtab_(symtab), layout_(layout),
      header_ent_cnt_(size == 32 ? 3 : 1),
      header_index_(size == 32 ? 0x2000 : 0)
  { }

  class Got_entry;

  // Create a new GOT entry and return its offset.
  unsigned int
  add_got_entry(Got_entry got_entry)
  {
    this->reserve_ent();
    return Output_data_got<size, big_endian>::add_got_entry(got_entry);
  }

  // Create a pair of new GOT entries and return the offset of the first.
  unsigned int
  add_got_entry_pair(Got_entry got_entry_1, Got_entry got_entry_2)
  {
    this->reserve_ent(2);
    return Output_data_got<size, big_endian>::add_got_entry_pair(got_entry_1,
								 got_entry_2);
  }

  unsigned int
  add_constant_pair(Valtype c1, Valtype c2)
  {
    this->reserve_ent(2);
    unsigned int got_offset = this->add_constant(c1);
    this->add_constant(c2);
    return got_offset;
  }

  // Offset of _GLOBAL_OFFSET_TABLE_.
  unsigned int
  g_o_t() const
  {
    return this->got_offset(this->header_index_);
  }

  // Offset of base used to access the GOT/TOC.
  // The got/toc pointer reg will be set to this value.
  Valtype
  got_base_offset(const Powerpc_relobj<size, big_endian>* object) const
  {
    if (size == 32)
      return this->g_o_t();
    else
      return (this->output_section()->address()
	      + object->toc_base_offset()
	      - this->address());
  }

  // Ensure our GOT has a header.
  void
  set_final_data_size()
  {
    if (this->header_ent_cnt_ != 0)
      this->make_header();
    Output_data_got<size, big_endian>::set_final_data_size();
  }

  // First word of GOT header needs some values that are not
  // handled by Output_data_got so poke them in here.
  // For 32-bit, address of .dynamic, for 64-bit, address of TOCbase.
  void
  do_write(Output_file* of)
  {
    Valtype val = 0;
    if (size == 32 && this->layout_->dynamic_data() != NULL)
      val = this->layout_->dynamic_section()->address();
    if (size == 64)
      val = this->output_section()->address() + 0x8000;
    this->replace_constant(this->header_index_, val);
    Output_data_got<size, big_endian>::do_write(of);
  }

private:
  void
  reserve_ent(unsigned int cnt = 1)
  {
    if (this->header_ent_cnt_ == 0)
      return;
    if (this->num_entries() + cnt > this->header_index_)
      this->make_header();
  }

  void
  make_header()
  {
    this->header_ent_cnt_ = 0;
    this->header_index_ = this->num_entries();
    if (size == 32)
      {
	Output_data_got<size, big_endian>::add_constant(0);
	Output_data_got<size, big_endian>::add_constant(0);
	Output_data_got<size, big_endian>::add_constant(0);

	// Define _GLOBAL_OFFSET_TABLE_ at the header
	Symbol *gotsym = this->symtab_->lookup("_GLOBAL_OFFSET_TABLE_", NULL);
	if (gotsym != NULL)
	  {
	    Sized_symbol<size>* sym = static_cast<Sized_symbol<size>*>(gotsym);
	    sym->set_value(this->g_o_t());
	  }
	else
	  this->symtab_->define_in_output_data("_GLOBAL_OFFSET_TABLE_", NULL,
					       Symbol_table::PREDEFINED,
					       this, this->g_o_t(), 0,
					       elfcpp::STT_OBJECT,
					       elfcpp::STB_LOCAL,
					       elfcpp::STV_HIDDEN, 0,
					       false, false);
      }
    else
      Output_data_got<size, big_endian>::add_constant(0);
  }

  // Stashed pointers.
  Symbol_table* symtab_;
  Layout* layout_;

  // GOT header size.
  unsigned int header_ent_cnt_;
  // GOT header index.
  unsigned int header_index_;
};

// Get the GOT section, creating it if necessary.

template<int size, bool big_endian>
Output_data_got_powerpc<size, big_endian>*
Target_powerpc<size, big_endian>::got_section(Symbol_table* symtab,
					      Layout* layout)
{
  if (this->got_ == NULL)
    {
      gold_assert(symtab != NULL && layout != NULL);

      this->got_
	= new Output_data_got_powerpc<size, big_endian>(symtab, layout);

      layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
				      elfcpp::SHF_ALLOC | elfcpp::SHF_WRITE,
				      this->got_, ORDER_DATA, false);
    }

  return this->got_;
}

// Get the dynamic reloc section, creating it if necessary.

template<int size, bool big_endian>
typename Target_powerpc<size, big_endian>::Reloc_section*
Target_powerpc<size, big_endian>::rela_dyn_section(Layout* layout)
{
  if (this->rela_dyn_ == NULL)
    {
      gold_assert(layout != NULL);
      this->rela_dyn_ = new Reloc_section(parameters->options().combreloc());
      layout->add_output_section_data(".rela.dyn", elfcpp::SHT_RELA,
				      elfcpp::SHF_ALLOC, this->rela_dyn_,
				      ORDER_DYNAMIC_RELOCS, false);
    }
  return this->rela_dyn_;
}

// Similarly, but for ifunc symbols get the one for ifunc.

template<int size, bool big_endian>
typename Target_powerpc<size, big_endian>::Reloc_section*
Target_powerpc<size, big_endian>::rela_dyn_section(Symbol_table* symtab,
						   Layout* layout,
						   bool for_ifunc)
{
  if (!for_ifunc)
    return this->rela_dyn_section(layout);

  if (this->iplt_ == NULL)
    this->make_iplt_section(symtab, layout);
  return this->iplt_->rel_plt();
}

class Stub_control
{
 public:
  // Determine the stub group size.  The group size is the absolute
  // value of the parameter --stub-group-size.  If --stub-group-size
  // is passed a negative value, we restrict stubs to be always before
  // the stubbed branches.
  Stub_control(int32_t size)
    : state_(NO_GROUP), stub_group_size_(abs(size)),
      stub14_group_size_(abs(size)),
      stubs_always_before_branch_(size < 0), suppress_size_errors_(false),
      group_end_addr_(0), owner_(NULL), output_section_(NULL)
  {
    if (stub_group_size_ == 1)
      {
	// Default values.
	if (stubs_always_before_branch_)
	  {
	    stub_group_size_ = 0x1e00000;
	    stub14_group_size_ = 0x7800;
	  }
	else
	  {
	    stub_group_size_ = 0x1c00000;
	    stub14_group_size_ = 0x7000;
	  }
	suppress_size_errors_ = true;
      }
  }

  // Return true iff input section can be handled by current stub
  // group.
  bool
  can_add_to_stub_group(Output_section* o,
			const Output_section::Input_section* i,
			bool has14);

  const Output_section::Input_section*
  owner()
  { return owner_; }

  Output_section*
  output_section()
  { return output_section_; }

 private:
  typedef enum
  {
    NO_GROUP,
    FINDING_STUB_SECTION,
    HAS_STUB_SECTION
  } State;

  State state_;
  uint32_t stub_group_size_;
  uint32_t stub14_group_size_;
  bool stubs_always_before_branch_;
  bool suppress_size_errors_;
  uint64_t group_end_addr_;
  const Output_section::Input_section* owner_;
  Output_section* output_section_;
};

// Return true iff input section can be handled by current stub
// group.

bool
Stub_control::can_add_to_stub_group(Output_section* o,
				    const Output_section::Input_section* i,
				    bool has14)
{
  uint32_t group_size
    = has14 ? this->stub14_group_size_ : this->stub_group_size_;
  bool whole_sec = o->order() == ORDER_INIT || o->order() == ORDER_FINI;
  uint64_t this_size;
  uint64_t start_addr = o->address();

  if (whole_sec)
    // .init and .fini sections are pasted together to form a single
    // function.  We can't be adding stubs in the middle of the function.
    this_size = o->data_size();
  else
    {
      start_addr += i->relobj()->output_section_offset(i->shndx());
      this_size = i->data_size();
    }
  uint64_t end_addr = start_addr + this_size;
  bool toobig = this_size > group_size;

  if (toobig && !this->suppress_size_errors_)
    gold_warning(_("%s:%s exceeds group size"),
		 i->relobj()->name().c_str(),
		 i->relobj()->section_name(i->shndx()).c_str());

  if (this->state_ != HAS_STUB_SECTION
      && (!whole_sec || this->output_section_ != o)
      && (this->state_ == NO_GROUP
	  || this->group_end_addr_ - end_addr < group_size))
    {
      this->owner_ = i;
      this->output_section_ = o;
    }

  if (this->state_ == NO_GROUP)
    {
      this->state_ = FINDING_STUB_SECTION;
      this->group_end_addr_ = end_addr;
    }
  else if (this->group_end_addr_ - start_addr < group_size)
    ;
  // Adding this section would make the group larger than GROUP_SIZE.
  else if (this->state_ == FINDING_STUB_SECTION
	   && !this->stubs_always_before_branch_
	   && !toobig)
    {
      // But wait, there's more!  Input sections up to GROUP_SIZE
      // bytes before the stub table can be handled by it too.
      this->state_ = HAS_STUB_SECTION;
      this->group_end_addr_ = end_addr;
    }
  else
    {
      this->state_ = NO_GROUP;
      return false;
    }
  return true;
}

// Look over all the input sections, deciding where to place stubs.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::group_sections(Layout* layout,
						 const Task*)
{
  Stub_control stub_control(parameters->options().stub_group_size());

  // Group input sections and insert stub table
  Stub_table<size, big_endian>* stub_table = NULL;
  Layout::Section_list section_list;
  layout->get_executable_sections(&section_list);
  std::stable_sort(section_list.begin(), section_list.end(), Sort_sections());
  for (Layout::Section_list::reverse_iterator o = section_list.rbegin();
       o != section_list.rend();
       ++o)
    {
      typedef Output_section::Input_section_list Input_section_list;
      for (Input_section_list::const_reverse_iterator i
	     = (*o)->input_sections().rbegin();
	   i != (*o)->input_sections().rend();
	   ++i)
	{
	  if (i->is_input_section())
	    {
	      Powerpc_relobj<size, big_endian>* ppcobj = static_cast
		<Powerpc_relobj<size, big_endian>*>(i->relobj());
	      bool has14 = ppcobj->has_14bit_branch(i->shndx());
	      if (!stub_control.can_add_to_stub_group(*o, &*i, has14))
		{
		  stub_table->init(stub_control.owner(),
				   stub_control.output_section());
		  stub_table = NULL;
		}
	      if (stub_table == NULL)
		stub_table = this->new_stub_table();
	      ppcobj->set_stub_table(i->shndx(), stub_table);
	    }
	}
    }
  if (stub_table != NULL)
    {
      const Output_section::Input_section* i = stub_control.owner();
      if (!i->is_input_section())
	{
	  // Corner case.  A new stub group was made for the first
	  // section (last one looked at here) for some reason, but
	  // the first section is already being used as the owner for
	  // a stub table for following sections.  Force it into that
	  // stub group.
	  gold_assert(this->stub_tables_.size() >= 2);
	  this->stub_tables_.pop_back();
	  delete stub_table;
	  Powerpc_relobj<size, big_endian>* ppcobj = static_cast
	    <Powerpc_relobj<size, big_endian>*>(i->relobj());
	  ppcobj->set_stub_table(i->shndx(), this->stub_tables_.back());
	}
      else
	stub_table->init(i, stub_control.output_section());
    }
}

// If this branch needs a plt call stub, or a long branch stub, make one.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::Branch_info::make_stub(
    Stub_table<size, big_endian>* stub_table,
    Stub_table<size, big_endian>* ifunc_stub_table,
    Symbol_table* symtab) const
{
  Symbol* sym = this->object_->global_symbol(this->r_sym_);
  if (sym != NULL && sym->is_forwarder())
    sym = symtab->resolve_forwards(sym);
  const Sized_symbol<size>* gsym = static_cast<const Sized_symbol<size>*>(sym);
  if (gsym != NULL
      ? use_plt_offset<size>(gsym, Scan::get_reference_flags(this->r_type_))
      : this->object_->local_has_plt_offset(this->r_sym_))
    {
      if (stub_table == NULL)
	stub_table = this->object_->stub_table(this->shndx_);
      if (stub_table == NULL)
	{
	  // This is a ref from a data section to an ifunc symbol.
	  stub_table = ifunc_stub_table;
	}
      gold_assert(stub_table != NULL);
      if (gsym != NULL)
	stub_table->add_plt_call_entry(this->object_, gsym,
				       this->r_type_, this->addend_);
      else
	stub_table->add_plt_call_entry(this->object_, this->r_sym_,
				       this->r_type_, this->addend_);
    }
  else
    {
      unsigned int max_branch_offset;
      if (this->r_type_ == elfcpp::R_POWERPC_REL14
	  || this->r_type_ == elfcpp::R_POWERPC_REL14_BRTAKEN
	  || this->r_type_ == elfcpp::R_POWERPC_REL14_BRNTAKEN)
	max_branch_offset = 1 << 15;
      else if (this->r_type_ == elfcpp::R_POWERPC_REL24
	       || this->r_type_ == elfcpp::R_PPC_PLTREL24
	       || this->r_type_ == elfcpp::R_PPC_LOCAL24PC)
	max_branch_offset = 1 << 25;
      else
	return;
      Address from = this->object_->get_output_section_offset(this->shndx_);
      gold_assert(from != invalid_address);
      from += (this->object_->output_section(this->shndx_)->address()
	       + this->offset_);
      Address to;
      if (gsym != NULL)
	{
	  switch (gsym->source())
	    {
	    case Symbol::FROM_OBJECT:
	      {
		Object* symobj = gsym->object();
		if (symobj->is_dynamic()
		    || symobj->pluginobj() != NULL)
		  return;
		bool is_ordinary;
		unsigned int shndx = gsym->shndx(&is_ordinary);
		if (shndx == elfcpp::SHN_UNDEF)
		  return;
	      }
	      break;

	    case Symbol::IS_UNDEFINED:
	      return;

	    default:
	      break;
	    }
	  Symbol_table::Compute_final_value_status status;
	  to = symtab->compute_final_value<size>(gsym, &status);
	  if (status != Symbol_table::CFVS_OK)
	    return;
	}
      else
	{
	  const Symbol_value<size>* psymval
	    = this->object_->local_symbol(this->r_sym_);
	  Symbol_value<size> symval;
	  typedef Sized_relobj_file<size, big_endian> ObjType;
	  typename ObjType::Compute_final_local_value_status status
	    = this->object_->compute_final_local_value(this->r_sym_, psymval,
						       &symval, symtab);
	  if (status != ObjType::CFLV_OK
	      || !symval.has_output_value())
	    return;
	  to = symval.value(this->object_, 0);
	}
      to += this->addend_;
      if (stub_table == NULL)
	stub_table = this->object_->stub_table(this->shndx_);
      if (size == 64 && is_branch_reloc(this->r_type_))
	{
	  unsigned int dest_shndx;
	  Target_powerpc<size, big_endian>* target =
	    static_cast<Target_powerpc<size, big_endian>*>(
		parameters->sized_target<size, big_endian>());
	  to = target->symval_for_branch(symtab, to, gsym,
					 this->object_, &dest_shndx);
	}
      Address delta = to - from;
      if (delta + max_branch_offset >= 2 * max_branch_offset)
	{
	  if (stub_table == NULL)
	    {
	      gold_warning(_("%s:%s: branch in non-executable section,"
			     " no long branch stub for you"),
			   this->object_->name().c_str(),
			   this->object_->section_name(this->shndx_).c_str());
	      return;
	    }
	  stub_table->add_long_branch_entry(this->object_, to);
	}
    }
}

// Relaxation hook.  This is where we do stub generation.

template<int size, bool big_endian>
bool
Target_powerpc<size, big_endian>::do_relax(int pass,
					   const Input_objects*,
					   Symbol_table* symtab,
					   Layout* layout,
					   const Task* task)
{
  unsigned int prev_brlt_size = 0;
  if (pass == 1)
    {
      bool thread_safe = parameters->options().plt_thread_safe();
      if (size == 64 && !parameters->options().user_set_plt_thread_safe())
	{
	  static const char* const thread_starter[] =
	    {
	      "pthread_create",
	      /* libstdc++ */
	      "_ZNSt6thread15_M_start_threadESt10shared_ptrINS_10_Impl_baseEE",
	      /* librt */
	      "aio_init", "aio_read", "aio_write", "aio_fsync", "lio_listio",
	      "mq_notify", "create_timer",
	      /* libanl */
	      "getaddrinfo_a",
	      /* libgomp */
	      "GOMP_parallel_start",
	      "GOMP_parallel_loop_static_start",
	      "GOMP_parallel_loop_dynamic_start",
	      "GOMP_parallel_loop_guided_start",
	      "GOMP_parallel_loop_runtime_start",
	      "GOMP_parallel_sections_start",
	    };

	  if (parameters->options().shared())
	    thread_safe = true;
	  else
	    {
	      for (unsigned int i = 0;
		   i < sizeof(thread_starter) / sizeof(thread_starter[0]);
		   i++)
		{
		  Symbol* sym = symtab->lookup(thread_starter[i], NULL);
		  thread_safe = (sym != NULL
				 && sym->in_reg()
				 && sym->in_real_elf());
		  if (thread_safe)
		    break;
		}
	    }
	}
      this->plt_thread_safe_ = thread_safe;
      this->group_sections(layout, task);
    }

  // We need address of stub tables valid for make_stub.
  for (typename Stub_tables::iterator p = this->stub_tables_.begin();
       p != this->stub_tables_.end();
       ++p)
    {
      const Powerpc_relobj<size, big_endian>* object
	= static_cast<const Powerpc_relobj<size, big_endian>*>((*p)->relobj());
      Address off = object->get_output_section_offset((*p)->shndx());
      gold_assert(off != invalid_address);
      Output_section* os = (*p)->output_section();
      (*p)->set_address_and_size(os, off);
    }

  if (pass != 1)
    {
      // Clear plt call stubs, long branch stubs and branch lookup table.
      prev_brlt_size = this->branch_lookup_table_.size();
      this->branch_lookup_table_.clear();
      for (typename Stub_tables::iterator p = this->stub_tables_.begin();
	   p != this->stub_tables_.end();
	   ++p)
	{
	  (*p)->clear_stubs();
	}
    }

  // Build all the stubs.
  Stub_table<size, big_endian>* ifunc_stub_table
    = this->stub_tables_.size() == 0 ? NULL : this->stub_tables_[0];
  Stub_table<size, big_endian>* one_stub_table
    = this->stub_tables_.size() != 1 ? NULL : ifunc_stub_table;
  for (typename Branches::const_iterator b = this->branch_info_.begin();
       b != this->branch_info_.end();
       b++)
    {
      b->make_stub(one_stub_table, ifunc_stub_table, symtab);
    }

  // Did anything change size?
  unsigned int num_huge_branches = this->branch_lookup_table_.size();
  bool again = num_huge_branches != prev_brlt_size;
  if (size == 64 && num_huge_branches != 0)
    this->make_brlt_section(layout);
  if (size == 64 && again)
    this->brlt_section_->set_current_size(num_huge_branches);

  typedef Unordered_set<Output_section*> Output_sections;
  Output_sections os_need_update;
  for (typename Stub_tables::iterator p = this->stub_tables_.begin();
       p != this->stub_tables_.end();
       ++p)
    {
      if ((*p)->size_update())
	{
	  again = true;
	  (*p)->add_eh_frame(layout);
	  os_need_update.insert((*p)->output_section());
	}
    }

  // Set output section offsets for all input sections in an output
  // section that just changed size.  Anything past the stubs will
  // need updating.
  for (typename Output_sections::iterator p = os_need_update.begin();
       p != os_need_update.end();
       p++)
    {
      Output_section* os = *p;
      Address off = 0;
      typedef Output_section::Input_section_list Input_section_list;
      for (Input_section_list::const_iterator i = os->input_sections().begin();
	   i != os->input_sections().end();
	   ++i)
	{
	  off = align_address(off, i->addralign());
	  if (i->is_input_section() || i->is_relaxed_input_section())
	    i->relobj()->set_section_offset(i->shndx(), off);
	  if (i->is_relaxed_input_section())
	    {
	      Stub_table<size, big_endian>* stub_table
		= static_cast<Stub_table<size, big_endian>*>(
		    i->relaxed_input_section());
	      off += stub_table->set_address_and_size(os, off);
	    }
	  else
	    off += i->data_size();
	}
      // If .branch_lt is part of this output section, then we have
      // just done the offset adjustment.
      os->clear_section_offsets_need_adjustment();
    }

  if (size == 64
      && !again
      && num_huge_branches != 0
      && parameters->options().output_is_position_independent())
    {
      // Fill in the BRLT relocs.
      this->brlt_section_->reset_brlt_sizes();
      for (typename Branch_lookup_table::const_iterator p
	     = this->branch_lookup_table_.begin();
	   p != this->branch_lookup_table_.end();
	   ++p)
	{
	  this->brlt_section_->add_reloc(p->first, p->second);
	}
      this->brlt_section_->finalize_brlt_sizes();
    }
  return again;
}

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::do_plt_fde_location(const Output_data* plt,
						      unsigned char* oview,
						      uint64_t* paddress,
						      off_t* plen) const
{
  uint64_t address = plt->address();
  off_t len = plt->data_size();

  if (plt == this->glink_)
    {
      // See Output_data_glink::do_write() for glink contents.
      if (size == 64)
	{
	  // There is one word before __glink_PLTresolve
	  address += 8;
	  len -= 8;
	}
      else if (parameters->options().output_is_position_independent())
	{
	  // There are two FDEs for a position independent glink.
	  // The first covers the branch table, the second
	  // __glink_PLTresolve at the end of glink.
	  off_t resolve_size = this->glink_->pltresolve_size;
	  if (oview[9] == 0)
	    len -= resolve_size;
	  else
	    {
	      address += len - resolve_size;
	      len = resolve_size;
	    }
	}
    }
  else
    {
      // Must be a stub table.
      const Stub_table<size, big_endian>* stub_table
	= static_cast<const Stub_table<size, big_endian>*>(plt);
      uint64_t stub_address = stub_table->stub_address();
      len -= stub_address - address;
      address = stub_address;
    }

  *paddress = address;
  *plen = len;
}

// A class to handle the PLT data.

template<int size, bool big_endian>
class Output_data_plt_powerpc : public Output_section_data_build
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_RELA, true,
			    size, big_endian> Reloc_section;

  Output_data_plt_powerpc(Target_powerpc<size, big_endian>* targ,
			  Reloc_section* plt_rel,
			  unsigned int reserved_size,
			  const char* name)
    : Output_section_data_build(size == 32 ? 4 : 8),
      rel_(plt_rel),
      targ_(targ),
      initial_plt_entry_size_(reserved_size),
      name_(name)
  { }

  // Add an entry to the PLT.
  void
  add_entry(Symbol*);

  void
  add_ifunc_entry(Symbol*);

  void
  add_local_ifunc_entry(Sized_relobj_file<size, big_endian>*, unsigned int);

  // Return the .rela.plt section data.
  Reloc_section*
  rel_plt() const
  {
    return this->rel_;
  }

  // Return the number of PLT entries.
  unsigned int
  entry_count() const
  {
    if (this->current_data_size() == 0)
      return 0;
    return ((this->current_data_size() - this->initial_plt_entry_size_)
	    / plt_entry_size);
  }

  // Return the offset of the first non-reserved PLT entry.
  unsigned int
  first_plt_entry_offset()
  { return this->initial_plt_entry_size_; }

  // Return the size of a PLT entry.
  static unsigned int
  get_plt_entry_size()
  { return plt_entry_size; }

 protected:
  void
  do_adjust_output_section(Output_section* os)
  {
    os->set_entsize(0);
  }

  // Write to a map file.
  void
  do_print_to_mapfile(Mapfile* mapfile) const
  { mapfile->print_output_data(this, this->name_); }

 private:
  // The size of an entry in the PLT.
  static const int plt_entry_size = size == 32 ? 4 : 24;

  // Write out the PLT data.
  void
  do_write(Output_file*);

  // The reloc section.
  Reloc_section* rel_;
  // Allows access to .glink for do_write.
  Target_powerpc<size, big_endian>* targ_;
  // The size of the first reserved entry.
  int initial_plt_entry_size_;
  // What to report in map file.
  const char *name_;
};

// Add an entry to the PLT.

template<int size, bool big_endian>
void
Output_data_plt_powerpc<size, big_endian>::add_entry(Symbol* gsym)
{
  if (!gsym->has_plt_offset())
    {
      section_size_type off = this->current_data_size();
      if (off == 0)
	off += this->first_plt_entry_offset();
      gsym->set_plt_offset(off);
      gsym->set_needs_dynsym_entry();
      unsigned int dynrel = elfcpp::R_POWERPC_JMP_SLOT;
      this->rel_->add_global(gsym, dynrel, this, off, 0);
      off += plt_entry_size;
      this->set_current_data_size(off);
    }
}

// Add an entry for a global ifunc symbol that resolves locally, to the IPLT.

template<int size, bool big_endian>
void
Output_data_plt_powerpc<size, big_endian>::add_ifunc_entry(Symbol* gsym)
{
  if (!gsym->has_plt_offset())
    {
      section_size_type off = this->current_data_size();
      gsym->set_plt_offset(off);
      unsigned int dynrel = elfcpp::R_POWERPC_IRELATIVE;
      if (size == 64)
	dynrel = elfcpp::R_PPC64_JMP_IREL;
      this->rel_->add_symbolless_global_addend(gsym, dynrel, this, off, 0);
      off += plt_entry_size;
      this->set_current_data_size(off);
    }
}

// Add an entry for a local ifunc symbol to the IPLT.

template<int size, bool big_endian>
void
Output_data_plt_powerpc<size, big_endian>::add_local_ifunc_entry(
    Sized_relobj_file<size, big_endian>* relobj,
    unsigned int local_sym_index)
{
  if (!relobj->local_has_plt_offset(local_sym_index))
    {
      section_size_type off = this->current_data_size();
      relobj->set_local_plt_offset(local_sym_index, off);
      unsigned int dynrel = elfcpp::R_POWERPC_IRELATIVE;
      if (size == 64)
	dynrel = elfcpp::R_PPC64_JMP_IREL;
      this->rel_->add_symbolless_local_addend(relobj, local_sym_index, dynrel,
					      this, off, 0);
      off += plt_entry_size;
      this->set_current_data_size(off);
    }
}

static const uint32_t add_0_11_11	= 0x7c0b5a14;
static const uint32_t add_2_2_11	= 0x7c425a14;
static const uint32_t add_3_3_2		= 0x7c631214;
static const uint32_t add_3_3_13	= 0x7c636a14;
static const uint32_t add_11_0_11	= 0x7d605a14;
static const uint32_t add_12_2_11	= 0x7d825a14;
static const uint32_t add_12_12_11	= 0x7d8c5a14;
static const uint32_t addi_11_11	= 0x396b0000;
static const uint32_t addi_12_12	= 0x398c0000;
static const uint32_t addi_2_2		= 0x38420000;
static const uint32_t addi_3_2		= 0x38620000;
static const uint32_t addi_3_3		= 0x38630000;
static const uint32_t addis_0_2		= 0x3c020000;
static const uint32_t addis_0_13	= 0x3c0d0000;
static const uint32_t addis_11_11	= 0x3d6b0000;
static const uint32_t addis_11_30	= 0x3d7e0000;
static const uint32_t addis_12_12	= 0x3d8c0000;
static const uint32_t addis_12_2	= 0x3d820000;
static const uint32_t addis_3_2		= 0x3c620000;
static const uint32_t addis_3_13	= 0x3c6d0000;
static const uint32_t b			= 0x48000000;
static const uint32_t bcl_20_31		= 0x429f0005;
static const uint32_t bctr		= 0x4e800420;
static const uint32_t blr		= 0x4e800020;
static const uint32_t blrl		= 0x4e800021;
static const uint32_t bnectr_p4		= 0x4ce20420;
static const uint32_t cmpldi_2_0	= 0x28220000;
static const uint32_t cror_15_15_15	= 0x4def7b82;
static const uint32_t cror_31_31_31	= 0x4ffffb82;
static const uint32_t ld_0_1		= 0xe8010000;
static const uint32_t ld_0_12		= 0xe80c0000;
static const uint32_t ld_11_12		= 0xe96c0000;
static const uint32_t ld_11_2		= 0xe9620000;
static const uint32_t ld_2_1		= 0xe8410000;
static const uint32_t ld_2_11		= 0xe84b0000;
static const uint32_t ld_2_12		= 0xe84c0000;
static const uint32_t ld_2_2		= 0xe8420000;
static const uint32_t lfd_0_1		= 0xc8010000;
static const uint32_t li_0_0		= 0x38000000;
static const uint32_t li_12_0		= 0x39800000;
static const uint32_t lis_0_0		= 0x3c000000;
static const uint32_t lis_11		= 0x3d600000;
static const uint32_t lis_12		= 0x3d800000;
static const uint32_t lwz_0_12		= 0x800c0000;
static const uint32_t lwz_11_11		= 0x816b0000;
static const uint32_t lwz_11_30		= 0x817e0000;
static const uint32_t lwz_12_12		= 0x818c0000;
static const uint32_t lwzu_0_12		= 0x840c0000;
static const uint32_t lvx_0_12_0	= 0x7c0c00ce;
static const uint32_t mflr_0		= 0x7c0802a6;
static const uint32_t mflr_11		= 0x7d6802a6;
static const uint32_t mflr_12		= 0x7d8802a6;
static const uint32_t mtctr_0		= 0x7c0903a6;
static const uint32_t mtctr_11		= 0x7d6903a6;
static const uint32_t mtctr_12		= 0x7d8903a6;
static const uint32_t mtlr_0		= 0x7c0803a6;
static const uint32_t mtlr_12		= 0x7d8803a6;
static const uint32_t nop		= 0x60000000;
static const uint32_t ori_0_0_0		= 0x60000000;
static const uint32_t std_0_1		= 0xf8010000;
static const uint32_t std_0_12		= 0xf80c0000;
static const uint32_t std_2_1		= 0xf8410000;
static const uint32_t stfd_0_1		= 0xd8010000;
static const uint32_t stvx_0_12_0	= 0x7c0c01ce;
static const uint32_t sub_11_11_12	= 0x7d6c5850;
static const uint32_t xor_11_11_11	= 0x7d6b5a78;

// Write out the PLT.

template<int size, bool big_endian>
void
Output_data_plt_powerpc<size, big_endian>::do_write(Output_file* of)
{
  if (size == 32 && this->name_[3] != 'I')
    {
      const section_size_type offset = this->offset();
      const section_size_type oview_size
	= convert_to_section_size_type(this->data_size());
      unsigned char* const oview = of->get_output_view(offset, oview_size);
      unsigned char* pov = oview;
      unsigned char* endpov = oview + oview_size;

      // The address of the .glink branch table
      const Output_data_glink<size, big_endian>* glink
	= this->targ_->glink_section();
      elfcpp::Elf_types<32>::Elf_Addr branch_tab = glink->address();

      while (pov < endpov)
	{
	  elfcpp::Swap<32, big_endian>::writeval(pov, branch_tab);
	  pov += 4;
	  branch_tab += 4;
	}

      of->write_output_view(offset, oview_size, oview);
    }
}

// Create the PLT section.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::make_plt_section(Symbol_table* symtab,
						   Layout* layout)
{
  if (this->plt_ == NULL)
    {
      if (this->got_ == NULL)
	this->got_section(symtab, layout);

      if (this->glink_ == NULL)
	make_glink_section(layout);

      // Ensure that .rela.dyn always appears before .rela.plt  This is
      // necessary due to how, on PowerPC and some other targets, .rela.dyn
      // needs to include .rela.plt in its range.
      this->rela_dyn_section(layout);

      Reloc_section* plt_rel = new Reloc_section(false);
      layout->add_output_section_data(".rela.plt", elfcpp::SHT_RELA,
				      elfcpp::SHF_ALLOC, plt_rel,
				      ORDER_DYNAMIC_PLT_RELOCS, false);
      this->plt_
	= new Output_data_plt_powerpc<size, big_endian>(this, plt_rel,
							size == 32 ? 0 : 24,
							"** PLT");
      layout->add_output_section_data(".plt",
				      (size == 32
				       ? elfcpp::SHT_PROGBITS
				       : elfcpp::SHT_NOBITS),
				      elfcpp::SHF_ALLOC | elfcpp::SHF_WRITE,
				      this->plt_,
				      (size == 32
				       ? ORDER_SMALL_DATA
				       : ORDER_SMALL_BSS),
				      false);
    }
}

// Create the IPLT section.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::make_iplt_section(Symbol_table* symtab,
						    Layout* layout)
{
  if (this->iplt_ == NULL)
    {
      this->make_plt_section(symtab, layout);

      Reloc_section* iplt_rel = new Reloc_section(false);
      this->rela_dyn_->output_section()->add_output_section_data(iplt_rel);
      this->iplt_
	= new Output_data_plt_powerpc<size, big_endian>(this, iplt_rel,
							0, "** IPLT");
      this->plt_->output_section()->add_output_section_data(this->iplt_);
    }
}

// A section for huge long branch addresses, similar to plt section.

template<int size, bool big_endian>
class Output_data_brlt_powerpc : public Output_section_data_build
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  typedef Output_data_reloc<elfcpp::SHT_RELA, true,
			    size, big_endian> Reloc_section;

  Output_data_brlt_powerpc(Target_powerpc<size, big_endian>* targ,
			   Reloc_section* brlt_rel)
    : Output_section_data_build(size == 32 ? 4 : 8),
      rel_(brlt_rel),
      targ_(targ)
  { }

  void
  reset_brlt_sizes()
  {
    this->reset_data_size();
    this->rel_->reset_data_size();
  }

  void
  finalize_brlt_sizes()
  {
    this->finalize_data_size();
    this->rel_->finalize_data_size();
  }

  // Add a reloc for an entry in the BRLT.
  void
  add_reloc(Address to, unsigned int off)
  { this->rel_->add_relative(elfcpp::R_POWERPC_RELATIVE, this, off, to); }

  // Update section and reloc section size.
  void
  set_current_size(unsigned int num_branches)
  {
    this->reset_address_and_file_offset();
    this->set_current_data_size(num_branches * 16);
    this->finalize_data_size();
    Output_section* os = this->output_section();
    os->set_section_offsets_need_adjustment();
    if (this->rel_ != NULL)
      {
	unsigned int reloc_size
	  = Reloc_types<elfcpp::SHT_RELA, size, big_endian>::reloc_size;
	this->rel_->reset_address_and_file_offset();
	this->rel_->set_current_data_size(num_branches * reloc_size);
	this->rel_->finalize_data_size();
	Output_section* os = this->rel_->output_section();
	os->set_section_offsets_need_adjustment();
      }
  }

 protected:
  void
  do_adjust_output_section(Output_section* os)
  {
    os->set_entsize(0);
  }

  // Write to a map file.
  void
  do_print_to_mapfile(Mapfile* mapfile) const
  { mapfile->print_output_data(this, "** BRLT"); }

 private:
  // Write out the BRLT data.
  void
  do_write(Output_file*);

  // The reloc section.
  Reloc_section* rel_;
  Target_powerpc<size, big_endian>* targ_;
};

// Make the branch lookup table section.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::make_brlt_section(Layout* layout)
{
  if (size == 64 && this->brlt_section_ == NULL)
    {
      Reloc_section* brlt_rel = NULL;
      bool is_pic = parameters->options().output_is_position_independent();
      if (is_pic)
	{
	  // When PIC we can't fill in .branch_lt (like .plt it can be
	  // a bss style section) but must initialise at runtime via
	  // dynamic relocats.
	  this->rela_dyn_section(layout);
	  brlt_rel = new Reloc_section(false);
	  this->rela_dyn_->output_section()->add_output_section_data(brlt_rel);
	}
      this->brlt_section_
	= new Output_data_brlt_powerpc<size, big_endian>(this, brlt_rel);
      if (this->plt_ && is_pic)
	this->plt_->output_section()
	  ->add_output_section_data(this->brlt_section_);
      else
	layout->add_output_section_data(".branch_lt",
					(is_pic ? elfcpp::SHT_NOBITS
					 : elfcpp::SHT_PROGBITS),
					elfcpp::SHF_ALLOC | elfcpp::SHF_WRITE,
					this->brlt_section_,
					(is_pic ? ORDER_SMALL_BSS
					 : ORDER_SMALL_DATA),
					false);
    }
}

// Write out .branch_lt when non-PIC.

template<int size, bool big_endian>
void
Output_data_brlt_powerpc<size, big_endian>::do_write(Output_file* of)
{
  if (size == 64 && !parameters->options().output_is_position_independent())
    {
      const section_size_type offset = this->offset();
      const section_size_type oview_size
	= convert_to_section_size_type(this->data_size());
      unsigned char* const oview = of->get_output_view(offset, oview_size);

      this->targ_->write_branch_lookup_table(oview);
      of->write_output_view(offset, oview_size, oview);
    }
}

static inline uint32_t
l(uint32_t a)
{
  return a & 0xffff;
}

static inline uint32_t
hi(uint32_t a)
{
  return l(a >> 16);
}

static inline uint32_t
ha(uint32_t a)
{
  return hi(a + 0x8000);
}

template<int size>
struct Eh_cie
{
  static const unsigned char eh_frame_cie[12];
};

template<int size>
const unsigned char Eh_cie<size>::eh_frame_cie[] =
{
  1,					// CIE version.
  'z', 'R', 0,				// Augmentation string.
  4,					// Code alignment.
  0x80 - size / 8 ,			// Data alignment.
  65,					// RA reg.
  1,					// Augmentation size.
  (elfcpp::DW_EH_PE_pcrel
   | elfcpp::DW_EH_PE_sdata4),		// FDE encoding.
  elfcpp::DW_CFA_def_cfa, 1, 0		// def_cfa: r1 offset 0.
};

// Describe __glink_PLTresolve use of LR, 64-bit version.
static const unsigned char glink_eh_frame_fde_64[] =
{
  0, 0, 0, 0,				// Replaced with offset to .glink.
  0, 0, 0, 0,				// Replaced with size of .glink.
  0,					// Augmentation size.
  elfcpp::DW_CFA_advance_loc + 1,
  elfcpp::DW_CFA_register, 65, 12,
  elfcpp::DW_CFA_advance_loc + 4,
  elfcpp::DW_CFA_restore_extended, 65
};

// Describe __glink_PLTresolve use of LR, 32-bit version.
static const unsigned char glink_eh_frame_fde_32[] =
{
  0, 0, 0, 0,				// Replaced with offset to .glink.
  0, 0, 0, 0,				// Replaced with size of .glink.
  0,					// Augmentation size.
  elfcpp::DW_CFA_advance_loc + 2,
  elfcpp::DW_CFA_register, 65, 0,
  elfcpp::DW_CFA_advance_loc + 4,
  elfcpp::DW_CFA_restore_extended, 65
};

static const unsigned char default_fde[] =
{
  0, 0, 0, 0,				// Replaced with offset to stubs.
  0, 0, 0, 0,				// Replaced with size of stubs.
  0,					// Augmentation size.
  elfcpp::DW_CFA_nop,			// Pad.
  elfcpp::DW_CFA_nop,
  elfcpp::DW_CFA_nop
};

template<bool big_endian>
static inline void
write_insn(unsigned char* p, uint32_t v)
{
  elfcpp::Swap<32, big_endian>::writeval(p, v);
}

// Stub_table holds information about plt and long branch stubs.
// Stubs are built in an area following some input section determined
// by group_sections().  This input section is converted to a relaxed
// input section allowing it to be resized to accommodate the stubs

template<int size, bool big_endian>
class Stub_table : public Output_relaxed_input_section
{
 public:
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  static const Address invalid_address = static_cast<Address>(0) - 1;

  Stub_table(Target_powerpc<size, big_endian>* targ)
    : Output_relaxed_input_section(NULL, 0, 0),
      targ_(targ), plt_call_stubs_(), long_branch_stubs_(),
      orig_data_size_(0), plt_size_(0), last_plt_size_(0),
      branch_size_(0), last_branch_size_(0), eh_frame_added_(false)
  { }

  // Delayed Output_relaxed_input_section init.
  void
  init(const Output_section::Input_section*, Output_section*);

  // Add a plt call stub.
  void
  add_plt_call_entry(const Sized_relobj_file<size, big_endian>*,
		     const Symbol*,
		     unsigned int,
		     Address);

  void
  add_plt_call_entry(const Sized_relobj_file<size, big_endian>*,
		     unsigned int,
		     unsigned int,
		     Address);

  // Find a given plt call stub.
  Address
  find_plt_call_entry(const Symbol*) const;

  Address
  find_plt_call_entry(const Sized_relobj_file<size, big_endian>*,
		      unsigned int) const;

  Address
  find_plt_call_entry(const Sized_relobj_file<size, big_endian>*,
		      const Symbol*,
		      unsigned int,
		      Address) const;

  Address
  find_plt_call_entry(const Sized_relobj_file<size, big_endian>*,
		      unsigned int,
		      unsigned int,
		      Address) const;

  // Add a long branch stub.
  void
  add_long_branch_entry(const Powerpc_relobj<size, big_endian>*, Address);

  Address
  find_long_branch_entry(const Powerpc_relobj<size, big_endian>*,
			 Address) const;

  void
  clear_stubs()
  {
    this->plt_call_stubs_.clear();
    this->plt_size_ = 0;
    this->long_branch_stubs_.clear();
    this->branch_size_ = 0;
  }

  Address
  set_address_and_size(const Output_section* os, Address off)
  {
    Address start_off = off;
    off += this->orig_data_size_;
    Address my_size = this->plt_size_ + this->branch_size_;
    if (my_size != 0)
      off = align_address(off, this->stub_align());
    // Include original section size and alignment padding in size
    my_size += off - start_off;
    this->reset_address_and_file_offset();
    this->set_current_data_size(my_size);
    this->set_address_and_file_offset(os->address() + start_off,
				      os->offset() + start_off);
    return my_size;
  }

  Address
  stub_address() const
  {
    return align_address(this->address() + this->orig_data_size_,
			 this->stub_align());
  }

  Address
  stub_offset() const
  {
    return align_address(this->offset() + this->orig_data_size_,
			 this->stub_align());
  }

  section_size_type
  plt_size() const
  { return this->plt_size_; }

  bool
  size_update()
  {
    Output_section* os = this->output_section();
    if (os->addralign() < this->stub_align())
      {
	os->set_addralign(this->stub_align());
	// FIXME: get rid of the insane checkpointing.
	// We can't increase alignment of the input section to which
	// stubs are attached;  The input section may be .init which
	// is pasted together with other .init sections to form a
	// function.  Aligning might insert zero padding resulting in
	// sigill.  However we do need to increase alignment of the
	// output section so that the align_address() on offset in
	// set_address_and_size() adds the same padding as the
	// align_address() on address in stub_address().
	// What's more, we need this alignment for the layout done in
	// relaxation_loop_body() so that the output section starts at
	// a suitably aligned address.
	os->checkpoint_set_addralign(this->stub_align());
      }
    if (this->last_plt_size_ != this->plt_size_
	|| this->last_branch_size_ != this->branch_size_)
      {
	this->last_plt_size_ = this->plt_size_;
	this->last_branch_size_ = this->branch_size_;
	return true;
      }
    return false;
  }

  // Add .eh_frame info for this stub section.  Unlike other linker
  // generated .eh_frame this is added late in the link, because we
  // only want the .eh_frame info if this particular stub section is
  // non-empty.
  void
  add_eh_frame(Layout* layout)
  {
    if (!this->eh_frame_added_)
      {
	if (!parameters->options().ld_generated_unwind_info())
	  return;

	// Since we add stub .eh_frame info late, it must be placed
	// after all other linker generated .eh_frame info so that
	// merge mapping need not be updated for input sections.
	// There is no provision to use a different CIE to that used
	// by .glink.
	if (!this->targ_->has_glink())
	  return;

	layout->add_eh_frame_for_plt(this,
				     Eh_cie<size>::eh_frame_cie,
				     sizeof (Eh_cie<size>::eh_frame_cie),
				     default_fde,
				     sizeof (default_fde));
	this->eh_frame_added_ = true;
      }
  }

  Target_powerpc<size, big_endian>*
  targ() const
  { return targ_; }

 private:
  class Plt_stub_ent;
  class Plt_stub_ent_hash;
  typedef Unordered_map<Plt_stub_ent, unsigned int,
			Plt_stub_ent_hash> Plt_stub_entries;

  // Alignment of stub section.
  unsigned int
  stub_align() const
  {
    if (size == 32)
      return 16;
    unsigned int min_align = 32;
    unsigned int user_align = 1 << parameters->options().plt_align();
    return std::max(user_align, min_align);
  }

  // Return the plt offset for the given call stub.
  Address
  plt_off(typename Plt_stub_entries::const_iterator p, bool* is_iplt) const
  {
    const Symbol* gsym = p->first.sym_;
    if (gsym != NULL)
      {
	*is_iplt = (gsym->type() == elfcpp::STT_GNU_IFUNC
		    && gsym->can_use_relative_reloc(false));
	return gsym->plt_offset();
      }
    else
      {
	*is_iplt = true;
	const Sized_relobj_file<size, big_endian>* relobj = p->first.object_;
	unsigned int local_sym_index = p->first.locsym_;
	return relobj->local_plt_offset(local_sym_index);
      }
  }

  // Size of a given plt call stub.
  unsigned int
  plt_call_size(typename Plt_stub_entries::const_iterator p) const
  {
    if (size == 32)
      return 16;

    bool is_iplt;
    Address plt_addr = this->plt_off(p, &is_iplt);
    if (is_iplt)
      plt_addr += this->targ_->iplt_section()->address();
    else
      plt_addr += this->targ_->plt_section()->address();
    Address got_addr = this->targ_->got_section()->output_section()->address();
    const Powerpc_relobj<size, big_endian>* ppcobj = static_cast
      <const Powerpc_relobj<size, big_endian>*>(p->first.object_);
    got_addr += ppcobj->toc_base_offset();
    Address off = plt_addr - got_addr;
    bool static_chain = parameters->options().plt_static_chain();
    bool thread_safe = this->targ_->plt_thread_safe();
    unsigned int bytes = (4 * 5
			  + 4 * static_chain
			  + 8 * thread_safe
			  + 4 * (ha(off) != 0)
			  + 4 * (ha(off + 8 + 8 * static_chain) != ha(off)));
    unsigned int align = 1 << parameters->options().plt_align();
    if (align > 1)
      bytes = (bytes + align - 1) & -align;
    return bytes;
  }

  // Return long branch stub size.
  unsigned int
  branch_stub_size(Address to)
  {
    Address loc
      = this->stub_address() + this->last_plt_size_ + this->branch_size_;
    if (to - loc + (1 << 25) < 2 << 25)
      return 4;
    if (size == 64 || !parameters->options().output_is_position_independent())
      return 16;
    return 32;
  }

  // Write out stubs.
  void
  do_write(Output_file*);

  // Plt call stub keys.
  class Plt_stub_ent
  {
  public:
    Plt_stub_ent(const Symbol* sym)
      : sym_(sym), object_(0), addend_(0), locsym_(0)
    { }

    Plt_stub_ent(const Sized_relobj_file<size, big_endian>* object,
		 unsigned int locsym_index)
      : sym_(NULL), object_(object), addend_(0), locsym_(locsym_index)
    { }

    Plt_stub_ent(const Sized_relobj_file<size, big_endian>* object,
		 const Symbol* sym,
		 unsigned int r_type,
		 Address addend)
      : sym_(sym), object_(0), addend_(0), locsym_(0)
    {
      if (size != 32)
	this->addend_ = addend;
      else if (parameters->options().output_is_position_independent()
	       && r_type == elfcpp::R_PPC_PLTREL24)
	{
	  this->addend_ = addend;
	  if (this->addend_ >= 32768)
	    this->object_ = object;
	}
    }

    Plt_stub_ent(const Sized_relobj_file<size, big_endian>* object,
		 unsigned int locsym_index,
		 unsigned int r_type,
		 Address addend)
      : sym_(NULL), object_(object), addend_(0), locsym_(locsym_index)
    {
      if (size != 32)
	this->addend_ = addend;
      else if (parameters->options().output_is_position_independent()
	       && r_type == elfcpp::R_PPC_PLTREL24)
	this->addend_ = addend;
    }

    bool operator==(const Plt_stub_ent& that) const
    {
      return (this->sym_ == that.sym_
	      && this->object_ == that.object_
	      && this->addend_ == that.addend_
	      && this->locsym_ == that.locsym_);
    }

    const Symbol* sym_;
    const Sized_relobj_file<size, big_endian>* object_;
    typename elfcpp::Elf_types<size>::Elf_Addr addend_;
    unsigned int locsym_;
  };

  class Plt_stub_ent_hash
  {
  public:
    size_t operator()(const Plt_stub_ent& ent) const
    {
      return (reinterpret_cast<uintptr_t>(ent.sym_)
	      ^ reinterpret_cast<uintptr_t>(ent.object_)
	      ^ ent.addend_
	      ^ ent.locsym_);
    }
  };

  // Long branch stub keys.
  class Branch_stub_ent
  {
  public:
    Branch_stub_ent(const Powerpc_relobj<size, big_endian>* obj, Address to)
      : dest_(to), toc_base_off_(0)
    {
      if (size == 64)
	toc_base_off_ = obj->toc_base_offset();
    }

    bool operator==(const Branch_stub_ent& that) const
    {
      return (this->dest_ == that.dest_
	      && (size == 32
		  || this->toc_base_off_ == that.toc_base_off_));
    }

    Address dest_;
    unsigned int toc_base_off_;
  };

  class Branch_stub_ent_hash
  {
  public:
    size_t operator()(const Branch_stub_ent& ent) const
    { return ent.dest_ ^ ent.toc_base_off_; }
  };

  // In a sane world this would be a global.
  Target_powerpc<size, big_endian>* targ_;
  // Map sym/object/addend to stub offset.
  Plt_stub_entries plt_call_stubs_;
  // Map destination address to stub offset.
  typedef Unordered_map<Branch_stub_ent, unsigned int,
			Branch_stub_ent_hash> Branch_stub_entries;
  Branch_stub_entries long_branch_stubs_;
  // size of input section
  section_size_type orig_data_size_;
  // size of stubs
  section_size_type plt_size_, last_plt_size_, branch_size_, last_branch_size_;
  // Whether .eh_frame info has been created for this stub section.
  bool eh_frame_added_;
};

// Make a new stub table, and record.

template<int size, bool big_endian>
Stub_table<size, big_endian>*
Target_powerpc<size, big_endian>::new_stub_table()
{
  Stub_table<size, big_endian>* stub_table
    = new Stub_table<size, big_endian>(this);
  this->stub_tables_.push_back(stub_table);
  return stub_table;
}

// Delayed stub table initialisation, because we create the stub table
// before we know to which section it will be attached.

template<int size, bool big_endian>
void
Stub_table<size, big_endian>::init(
    const Output_section::Input_section* owner,
    Output_section* output_section)
{
  this->set_relobj(owner->relobj());
  this->set_shndx(owner->shndx());
  this->set_addralign(this->relobj()->section_addralign(this->shndx()));
  this->set_output_section(output_section);
  this->orig_data_size_ = owner->current_data_size();

  std::vector<Output_relaxed_input_section*> new_relaxed;
  new_relaxed.push_back(this);
  output_section->convert_input_sections_to_relaxed_sections(new_relaxed);
}

// Add a plt call stub, if we do not already have one for this
// sym/object/addend combo.

template<int size, bool big_endian>
void
Stub_table<size, big_endian>::add_plt_call_entry(
    const Sized_relobj_file<size, big_endian>* object,
    const Symbol* gsym,
    unsigned int r_type,
    Address addend)
{
  Plt_stub_ent ent(object, gsym, r_type, addend);
  Address off = this->plt_size_;
  std::pair<typename Plt_stub_entries::iterator, bool> p
    = this->plt_call_stubs_.insert(std::make_pair(ent, off));
  if (p.second)
    this->plt_size_ = off + this->plt_call_size(p.first);
}

template<int size, bool big_endian>
void
Stub_table<size, big_endian>::add_plt_call_entry(
    const Sized_relobj_file<size, big_endian>* object,
    unsigned int locsym_index,
    unsigned int r_type,
    Address addend)
{
  Plt_stub_ent ent(object, locsym_index, r_type, addend);
  Address off = this->plt_size_;
  std::pair<typename Plt_stub_entries::iterator, bool> p
    = this->plt_call_stubs_.insert(std::make_pair(ent, off));
  if (p.second)
    this->plt_size_ = off + this->plt_call_size(p.first);
}

// Find a plt call stub.

template<int size, bool big_endian>
typename Stub_table<size, big_endian>::Address
Stub_table<size, big_endian>::find_plt_call_entry(
    const Sized_relobj_file<size, big_endian>* object,
    const Symbol* gsym,
    unsigned int r_type,
    Address addend) const
{
  Plt_stub_ent ent(object, gsym, r_type, addend);
  typename Plt_stub_entries::const_iterator p = this->plt_call_stubs_.find(ent);
  return p == this->plt_call_stubs_.end() ? invalid_address : p->second;
}

template<int size, bool big_endian>
typename Stub_table<size, big_endian>::Address
Stub_table<size, big_endian>::find_plt_call_entry(const Symbol* gsym) const
{
  Plt_stub_ent ent(gsym);
  typename Plt_stub_entries::const_iterator p = this->plt_call_stubs_.find(ent);
  return p == this->plt_call_stubs_.end() ? invalid_address : p->second;
}

template<int size, bool big_endian>
typename Stub_table<size, big_endian>::Address
Stub_table<size, big_endian>::find_plt_call_entry(
    const Sized_relobj_file<size, big_endian>* object,
    unsigned int locsym_index,
    unsigned int r_type,
    Address addend) const
{
  Plt_stub_ent ent(object, locsym_index, r_type, addend);
  typename Plt_stub_entries::const_iterator p = this->plt_call_stubs_.find(ent);
  return p == this->plt_call_stubs_.end() ? invalid_address : p->second;
}

template<int size, bool big_endian>
typename Stub_table<size, big_endian>::Address
Stub_table<size, big_endian>::find_plt_call_entry(
    const Sized_relobj_file<size, big_endian>* object,
    unsigned int locsym_index) const
{
  Plt_stub_ent ent(object, locsym_index);
  typename Plt_stub_entries::const_iterator p = this->plt_call_stubs_.find(ent);
  return p == this->plt_call_stubs_.end() ? invalid_address : p->second;
}

// Add a long branch stub if we don't already have one to given
// destination.

template<int size, bool big_endian>
void
Stub_table<size, big_endian>::add_long_branch_entry(
    const Powerpc_relobj<size, big_endian>* object,
    Address to)
{
  Branch_stub_ent ent(object, to);
  Address off = this->branch_size_;
  if (this->long_branch_stubs_.insert(std::make_pair(ent, off)).second)
    {
      unsigned int stub_size = this->branch_stub_size(to);
      this->branch_size_ = off + stub_size;
      if (size == 64 && stub_size != 4)
	this->targ_->add_branch_lookup_table(to);
    }
}

// Find long branch stub.

template<int size, bool big_endian>
typename Stub_table<size, big_endian>::Address
Stub_table<size, big_endian>::find_long_branch_entry(
    const Powerpc_relobj<size, big_endian>* object,
    Address to) const
{
  Branch_stub_ent ent(object, to);
  typename Branch_stub_entries::const_iterator p
    = this->long_branch_stubs_.find(ent);
  return p == this->long_branch_stubs_.end() ? invalid_address : p->second;
}

// A class to handle .glink.

template<int size, bool big_endian>
class Output_data_glink : public Output_section_data
{
 public:
  static const int pltresolve_size = 16*4;

  Output_data_glink(Target_powerpc<size, big_endian>* targ)
    : Output_section_data(16), targ_(targ)
  { }

  void
  add_eh_frame(Layout* layout)
  {
    if (!parameters->options().ld_generated_unwind_info())
      return;

    if (size == 64)
      layout->add_eh_frame_for_plt(this,
				   Eh_cie<64>::eh_frame_cie,
				   sizeof (Eh_cie<64>::eh_frame_cie),
				   glink_eh_frame_fde_64,
				   sizeof (glink_eh_frame_fde_64));
    else
      {
	// 32-bit .glink can use the default since the CIE return
	// address reg, LR, is valid.
	layout->add_eh_frame_for_plt(this,
				     Eh_cie<32>::eh_frame_cie,
				     sizeof (Eh_cie<32>::eh_frame_cie),
				     default_fde,
				     sizeof (default_fde));
	// Except where LR is used in a PIC __glink_PLTresolve.
	if (parameters->options().output_is_position_independent())
	  layout->add_eh_frame_for_plt(this,
				       Eh_cie<32>::eh_frame_cie,
				       sizeof (Eh_cie<32>::eh_frame_cie),
				       glink_eh_frame_fde_32,
				       sizeof (glink_eh_frame_fde_32));
      }
  }

 protected:
  // Write to a map file.
  void
  do_print_to_mapfile(Mapfile* mapfile) const
  { mapfile->print_output_data(this, _("** glink")); }

 private:
  void
  set_final_data_size();

  // Write out .glink
  void
  do_write(Output_file*);

  // Allows access to .got and .plt for do_write.
  Target_powerpc<size, big_endian>* targ_;
};

template<int size, bool big_endian>
void
Output_data_glink<size, big_endian>::set_final_data_size()
{
  unsigned int count = this->targ_->plt_entry_count();
  section_size_type total = 0;

  if (count != 0)
    {
      if (size == 32)
	{
	  // space for branch table
	  total += 4 * (count - 1);

	  total += -total & 15;
	  total += this->pltresolve_size;
	}
      else
	{
	  total += this->pltresolve_size;

	  // space for branch table
	  total += 8 * count;
	  if (count > 0x8000)
	    total += 4 * (count - 0x8000);
	}
    }

  this->set_data_size(total);
}

// Write out plt and long branch stub code.

template<int size, bool big_endian>
void
Stub_table<size, big_endian>::do_write(Output_file* of)
{
  if (this->plt_call_stubs_.empty()
      && this->long_branch_stubs_.empty())
    return;

  const section_size_type start_off = this->offset();
  const section_size_type off = this->stub_offset();
  const section_size_type oview_size =
    convert_to_section_size_type(this->data_size() - (off - start_off));
  unsigned char* const oview = of->get_output_view(off, oview_size);
  unsigned char* p;

  if (size == 64)
    {
      const Output_data_got_powerpc<size, big_endian>* got
	= this->targ_->got_section();
      Address got_os_addr = got->output_section()->address();

      if (!this->plt_call_stubs_.empty())
	{
	  // The base address of the .plt section.
	  Address plt_base = this->targ_->plt_section()->address();
	  Address iplt_base = invalid_address;

	  // Write out plt call stubs.
	  typename Plt_stub_entries::const_iterator cs;
	  for (cs = this->plt_call_stubs_.begin();
	       cs != this->plt_call_stubs_.end();
	       ++cs)
	    {
	      bool is_iplt;
	      Address pltoff = this->plt_off(cs, &is_iplt);
	      Address plt_addr = pltoff;
	      if (is_iplt)
		{
		  if (iplt_base == invalid_address)
		    iplt_base = this->targ_->iplt_section()->address();
		  plt_addr += iplt_base;
		}
	      else
		plt_addr += plt_base;
	      const Powerpc_relobj<size, big_endian>* ppcobj = static_cast
		<const Powerpc_relobj<size, big_endian>*>(cs->first.object_);
	      Address got_addr = got_os_addr + ppcobj->toc_base_offset();
	      Address off = plt_addr - got_addr;

	      if (off + 0x80008000 > 0xffffffff || (off & 7) != 0)
		gold_error(_("%s: linkage table error against `%s'"),
			   cs->first.object_->name().c_str(),
			   cs->first.sym_->demangled_name().c_str());

	      bool static_chain = parameters->options().plt_static_chain();
	      bool thread_safe = this->targ_->plt_thread_safe();
	      bool use_fake_dep = false;
	      Address cmp_branch_off = 0;
	      if (thread_safe)
		{
		  unsigned int pltindex
		    = ((pltoff - this->targ_->first_plt_entry_offset())
		       / this->targ_->plt_entry_size());
		  Address glinkoff
		    = (this->targ_->glink_section()->pltresolve_size
		       + pltindex * 8);
		  if (pltindex > 32768)
		    glinkoff += (pltindex - 32768) * 4;
		  Address to
		    = this->targ_->glink_section()->address() + glinkoff;
		  Address from
		    = (this->stub_address() + cs->second + 24
		       + 4 * (ha(off) != 0)
		       + 4 * (ha(off + 8 + 8 * static_chain) != ha(off))
		       + 4 * static_chain);
		  cmp_branch_off = to - from;
		  use_fake_dep = cmp_branch_off + (1 << 25) >= (1 << 26);
		}

	      p = oview + cs->second;
	      if (ha(off) != 0)
		{
		  write_insn<big_endian>(p, std_2_1 + 40),		p += 4;
		  write_insn<big_endian>(p, addis_12_2 + ha(off)),	p += 4;
		  write_insn<big_endian>(p, ld_11_12 + l(off)),		p += 4;
		  if (ha(off + 8 + 8 * static_chain) != ha(off))
		    {
		      write_insn<big_endian>(p, addi_12_12 + l(off)),	p += 4;
		      off = 0;
		    }
		  write_insn<big_endian>(p, mtctr_11),			p += 4;
		  if (use_fake_dep)
		    {
		      write_insn<big_endian>(p, xor_11_11_11),		p += 4;
		      write_insn<big_endian>(p, add_12_12_11),		p += 4;
		    }
		  write_insn<big_endian>(p, ld_2_12 + l(off + 8)),	p += 4;
		  if (static_chain)
		    write_insn<big_endian>(p, ld_11_12 + l(off + 16)),	p += 4;
		}
	      else
		{
		  write_insn<big_endian>(p, std_2_1 + 40),		p += 4;
		  write_insn<big_endian>(p, ld_11_2 + l(off)),		p += 4;
		  if (ha(off + 8 + 8 * static_chain) != ha(off))
		    {
		      write_insn<big_endian>(p, addi_2_2 + l(off)),	p += 4;
		      off = 0;
		    }
		  write_insn<big_endian>(p, mtctr_11),			p += 4;
		  if (use_fake_dep)
		    {
		      write_insn<big_endian>(p, xor_11_11_11),		p += 4;
		      write_insn<big_endian>(p, add_2_2_11),		p += 4;
		    }
		  if (static_chain)
		    write_insn<big_endian>(p, ld_11_2 + l(off + 16)),	p += 4;
		  write_insn<big_endian>(p, ld_2_2 + l(off + 8)),	p += 4;
		}
	      if (thread_safe && !use_fake_dep)
		{
		  write_insn<big_endian>(p, cmpldi_2_0),		p += 4;
		  write_insn<big_endian>(p, bnectr_p4),			p += 4;
		  write_insn<big_endian>(p, b | (cmp_branch_off & 0x3fffffc));
		}
	      else
		write_insn<big_endian>(p, bctr);
	    }
	}

      // Write out long branch stubs.
      typename Branch_stub_entries::const_iterator bs;
      for (bs = this->long_branch_stubs_.begin();
	   bs != this->long_branch_stubs_.end();
	   ++bs)
	{
	  p = oview + this->plt_size_ + bs->second;
	  Address loc = this->stub_address() + this->plt_size_ + bs->second;
	  Address delta = bs->first.dest_ - loc;
	  if (delta + (1 << 25) < 2 << 25)
	    write_insn<big_endian>(p, b | (delta & 0x3fffffc));
	  else
	    {
	      Address brlt_addr
		= this->targ_->find_branch_lookup_table(bs->first.dest_);
	      gold_assert(brlt_addr != invalid_address);
	      brlt_addr += this->targ_->brlt_section()->address();
	      Address got_addr = got_os_addr + bs->first.toc_base_off_;
	      Address brltoff = brlt_addr - got_addr;
	      if (ha(brltoff) == 0)
		{
		  write_insn<big_endian>(p, ld_11_2 + l(brltoff)),	p += 4;
		}
	      else
		{
		  write_insn<big_endian>(p, addis_12_2 + ha(brltoff)),	p += 4;
		  write_insn<big_endian>(p, ld_11_12 + l(brltoff)),	p += 4;
		}
	      write_insn<big_endian>(p, mtctr_11),			p += 4;
	      write_insn<big_endian>(p, bctr);
	    }
	}
    }
  else
    {
      if (!this->plt_call_stubs_.empty())
	{
	  // The base address of the .plt section.
	  Address plt_base = this->targ_->plt_section()->address();
	  Address iplt_base = invalid_address;
	  // The address of _GLOBAL_OFFSET_TABLE_.
	  Address g_o_t = invalid_address;

	  // Write out plt call stubs.
	  typename Plt_stub_entries::const_iterator cs;
	  for (cs = this->plt_call_stubs_.begin();
	       cs != this->plt_call_stubs_.end();
	       ++cs)
	    {
	      bool is_iplt;
	      Address plt_addr = this->plt_off(cs, &is_iplt);
	      if (is_iplt)
		{
		  if (iplt_base == invalid_address)
		    iplt_base = this->targ_->iplt_section()->address();
		  plt_addr += iplt_base;
		}
	      else
		plt_addr += plt_base;

	      p = oview + cs->second;
	      if (parameters->options().output_is_position_independent())
		{
		  Address got_addr;
		  const Powerpc_relobj<size, big_endian>* ppcobj
		    = (static_cast<const Powerpc_relobj<size, big_endian>*>
		       (cs->first.object_));
		  if (ppcobj != NULL && cs->first.addend_ >= 32768)
		    {
		      unsigned int got2 = ppcobj->got2_shndx();
		      got_addr = ppcobj->get_output_section_offset(got2);
		      gold_assert(got_addr != invalid_address);
		      got_addr += (ppcobj->output_section(got2)->address()
				   + cs->first.addend_);
		    }
		  else
		    {
		      if (g_o_t == invalid_address)
			{
			  const Output_data_got_powerpc<size, big_endian>* got
			    = this->targ_->got_section();
			  g_o_t = got->address() + got->g_o_t();
			}
		      got_addr = g_o_t;
		    }

		  Address off = plt_addr - got_addr;
		  if (ha(off) == 0)
		    {
		      write_insn<big_endian>(p +  0, lwz_11_30 + l(off));
		      write_insn<big_endian>(p +  4, mtctr_11);
		      write_insn<big_endian>(p +  8, bctr);
		    }
		  else
		    {
		      write_insn<big_endian>(p +  0, addis_11_30 + ha(off));
		      write_insn<big_endian>(p +  4, lwz_11_11 + l(off));
		      write_insn<big_endian>(p +  8, mtctr_11);
		      write_insn<big_endian>(p + 12, bctr);
		    }
		}
	      else
		{
		  write_insn<big_endian>(p +  0, lis_11 + ha(plt_addr));
		  write_insn<big_endian>(p +  4, lwz_11_11 + l(plt_addr));
		  write_insn<big_endian>(p +  8, mtctr_11);
		  write_insn<big_endian>(p + 12, bctr);
		}
	    }
	}

      // Write out long branch stubs.
      typename Branch_stub_entries::const_iterator bs;
      for (bs = this->long_branch_stubs_.begin();
	   bs != this->long_branch_stubs_.end();
	   ++bs)
	{
	  p = oview + this->plt_size_ + bs->second;
	  Address loc = this->stub_address() + this->plt_size_ + bs->second;
	  Address delta = bs->first.dest_ - loc;
	  if (delta + (1 << 25) < 2 << 25)
	    write_insn<big_endian>(p, b | (delta & 0x3fffffc));
	  else if (!parameters->options().output_is_position_independent())
	    {
	      write_insn<big_endian>(p +  0, lis_12 + ha(bs->first.dest_));
	      write_insn<big_endian>(p +  4, addi_12_12 + l(bs->first.dest_));
	      write_insn<big_endian>(p +  8, mtctr_12);
	      write_insn<big_endian>(p + 12, bctr);
	    }
	  else
	    {
	      delta -= 8;
	      write_insn<big_endian>(p +  0, mflr_0);
	      write_insn<big_endian>(p +  4, bcl_20_31);
	      write_insn<big_endian>(p +  8, mflr_12);
	      write_insn<big_endian>(p + 12, addis_12_12 + ha(delta));
	      write_insn<big_endian>(p + 16, addi_12_12 + l(delta));
	      write_insn<big_endian>(p + 20, mtlr_0);
	      write_insn<big_endian>(p + 24, mtctr_12);
	      write_insn<big_endian>(p + 28, bctr);
	    }
	}
    }
}

// Write out .glink.

template<int size, bool big_endian>
void
Output_data_glink<size, big_endian>::do_write(Output_file* of)
{
  const section_size_type off = this->offset();
  const section_size_type oview_size =
    convert_to_section_size_type(this->data_size());
  unsigned char* const oview = of->get_output_view(off, oview_size);
  unsigned char* p;

  // The base address of the .plt section.
  typedef typename elfcpp::Elf_types<size>::Elf_Addr Address;
  Address plt_base = this->targ_->plt_section()->address();

  if (size == 64)
    {
      // Write pltresolve stub.
      p = oview;
      Address after_bcl = this->address() + 16;
      Address pltoff = plt_base - after_bcl;

      elfcpp::Swap<64, big_endian>::writeval(p, pltoff),	p += 8;

      write_insn<big_endian>(p, mflr_12),			p += 4;
      write_insn<big_endian>(p, bcl_20_31),			p += 4;
      write_insn<big_endian>(p, mflr_11),			p += 4;
      write_insn<big_endian>(p, ld_2_11 + l(-16)),		p += 4;
      write_insn<big_endian>(p, mtlr_12),			p += 4;
      write_insn<big_endian>(p, add_12_2_11),			p += 4;
      write_insn<big_endian>(p, ld_11_12 + 0),			p += 4;
      write_insn<big_endian>(p, ld_2_12 + 8),			p += 4;
      write_insn<big_endian>(p, mtctr_11),			p += 4;
      write_insn<big_endian>(p, ld_11_12 + 16),			p += 4;
      write_insn<big_endian>(p, bctr),				p += 4;
      while (p < oview + this->pltresolve_size)
	write_insn<big_endian>(p, nop), p += 4;

      // Write lazy link call stubs.
      uint32_t indx = 0;
      while (p < oview + oview_size)
	{
	  if (indx < 0x8000)
	    {
	      write_insn<big_endian>(p, li_0_0 + indx),			p += 4;
	    }
	  else
	    {
	      write_insn<big_endian>(p, lis_0_0 + hi(indx)),		p += 4;
	      write_insn<big_endian>(p, ori_0_0_0 + l(indx)),		p += 4;
	    }
	  uint32_t branch_off = 8 - (p - oview);
	  write_insn<big_endian>(p, b + (branch_off & 0x3fffffc)),	p += 4;
	  indx++;
	}
    }
  else
    {
      const Output_data_got_powerpc<size, big_endian>* got
	= this->targ_->got_section();
      // The address of _GLOBAL_OFFSET_TABLE_.
      Address g_o_t = got->address() + got->g_o_t();

      // Write out pltresolve branch table.
      p = oview;
      unsigned int the_end = oview_size - this->pltresolve_size;
      unsigned char* end_p = oview + the_end;
      while (p < end_p - 8 * 4)
	write_insn<big_endian>(p, b + end_p - p), p += 4;
      while (p < end_p)
	write_insn<big_endian>(p, nop), p += 4;

      // Write out pltresolve call stub.
      if (parameters->options().output_is_position_independent())
	{
	  Address res0_off = 0;
	  Address after_bcl_off = the_end + 12;
	  Address bcl_res0 = after_bcl_off - res0_off;

	  write_insn<big_endian>(p +  0, addis_11_11 + ha(bcl_res0));
	  write_insn<big_endian>(p +  4, mflr_0);
	  write_insn<big_endian>(p +  8, bcl_20_31);
	  write_insn<big_endian>(p + 12, addi_11_11 + l(bcl_res0));
	  write_insn<big_endian>(p + 16, mflr_12);
	  write_insn<big_endian>(p + 20, mtlr_0);
	  write_insn<big_endian>(p + 24, sub_11_11_12);

	  Address got_bcl = g_o_t + 4 - (after_bcl_off + this->address());

	  write_insn<big_endian>(p + 28, addis_12_12 + ha(got_bcl));
	  if (ha(got_bcl) == ha(got_bcl + 4))
	    {
	      write_insn<big_endian>(p + 32, lwz_0_12 + l(got_bcl));
	      write_insn<big_endian>(p + 36, lwz_12_12 + l(got_bcl + 4));
	    }
	  else
	    {
	      write_insn<big_endian>(p + 32, lwzu_0_12 + l(got_bcl));
	      write_insn<big_endian>(p + 36, lwz_12_12 + 4);
	    }
	  write_insn<big_endian>(p + 40, mtctr_0);
	  write_insn<big_endian>(p + 44, add_0_11_11);
	  write_insn<big_endian>(p + 48, add_11_0_11);
	  write_insn<big_endian>(p + 52, bctr);
	  write_insn<big_endian>(p + 56, nop);
	  write_insn<big_endian>(p + 60, nop);
	}
      else
	{
	  Address res0 = this->address();

	  write_insn<big_endian>(p + 0, lis_12 + ha(g_o_t + 4));
	  write_insn<big_endian>(p + 4, addis_11_11 + ha(-res0));
	  if (ha(g_o_t + 4) == ha(g_o_t + 8))
	    write_insn<big_endian>(p + 8, lwz_0_12 + l(g_o_t + 4));
	  else
	    write_insn<big_endian>(p + 8, lwzu_0_12 + l(g_o_t + 4));
	  write_insn<big_endian>(p + 12, addi_11_11 + l(-res0));
	  write_insn<big_endian>(p + 16, mtctr_0);
	  write_insn<big_endian>(p + 20, add_0_11_11);
	  if (ha(g_o_t + 4) == ha(g_o_t + 8))
	    write_insn<big_endian>(p + 24, lwz_12_12 + l(g_o_t + 8));
	  else
	    write_insn<big_endian>(p + 24, lwz_12_12 + 4);
	  write_insn<big_endian>(p + 28, add_11_0_11);
	  write_insn<big_endian>(p + 32, bctr);
	  write_insn<big_endian>(p + 36, nop);
	  write_insn<big_endian>(p + 40, nop);
	  write_insn<big_endian>(p + 44, nop);
	  write_insn<big_endian>(p + 48, nop);
	  write_insn<big_endian>(p + 52, nop);
	  write_insn<big_endian>(p + 56, nop);
	  write_insn<big_endian>(p + 60, nop);
	}
      p += 64;
    }

  of->write_output_view(off, oview_size, oview);
}


// A class to handle linker generated save/restore functions.

template<int size, bool big_endian>
class Output_data_save_res : public Output_section_data_build
{
 public:
  Output_data_save_res(Symbol_table* symtab);

 protected:
  // Write to a map file.
  void
  do_print_to_mapfile(Mapfile* mapfile) const
  { mapfile->print_output_data(this, _("** save/restore")); }

  void
  do_write(Output_file*);

 private:
  // The maximum size of save/restore contents.
  static const unsigned int savres_max = 218*4;

  void
  savres_define(Symbol_table* symtab,
		const char *name,
		unsigned int lo, unsigned int hi,
		unsigned char* write_ent(unsigned char*, int),
		unsigned char* write_tail(unsigned char*, int));

  unsigned char *contents_;
};

template<bool big_endian>
static unsigned char*
savegpr0(unsigned char* p, int r)
{
  uint32_t insn = std_0_1 + (r << 21) + (1 << 16) - (32 - r) * 8;
  write_insn<big_endian>(p, insn);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
savegpr0_tail(unsigned char* p, int r)
{
  p = savegpr0<big_endian>(p, r);
  uint32_t insn = std_0_1 + 16;
  write_insn<big_endian>(p, insn);
  p = p + 4;
  write_insn<big_endian>(p, blr);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
restgpr0(unsigned char* p, int r)
{
  uint32_t insn = ld_0_1 + (r << 21) + (1 << 16) - (32 - r) * 8;
  write_insn<big_endian>(p, insn);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
restgpr0_tail(unsigned char* p, int r)
{
  uint32_t insn = ld_0_1 + 16;
  write_insn<big_endian>(p, insn);
  p = p + 4;
  p = restgpr0<big_endian>(p, r);
  write_insn<big_endian>(p, mtlr_0);
  p = p + 4;
  if (r == 29)
    {
      p = restgpr0<big_endian>(p, 30);
      p = restgpr0<big_endian>(p, 31);
    }
  write_insn<big_endian>(p, blr);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
savegpr1(unsigned char* p, int r)
{
  uint32_t insn = std_0_12 + (r << 21) + (1 << 16) - (32 - r) * 8;
  write_insn<big_endian>(p, insn);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
savegpr1_tail(unsigned char* p, int r)
{
  p = savegpr1<big_endian>(p, r);
  write_insn<big_endian>(p, blr);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
restgpr1(unsigned char* p, int r)
{
  uint32_t insn = ld_0_12 + (r << 21) + (1 << 16) - (32 - r) * 8;
  write_insn<big_endian>(p, insn);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
restgpr1_tail(unsigned char* p, int r)
{
  p = restgpr1<big_endian>(p, r);
  write_insn<big_endian>(p, blr);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
savefpr(unsigned char* p, int r)
{
  uint32_t insn = stfd_0_1 + (r << 21) + (1 << 16) - (32 - r) * 8;
  write_insn<big_endian>(p, insn);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
savefpr0_tail(unsigned char* p, int r)
{
  p = savefpr<big_endian>(p, r);
  write_insn<big_endian>(p, std_0_1 + 16);
  p = p + 4;
  write_insn<big_endian>(p, blr);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
restfpr(unsigned char* p, int r)
{
  uint32_t insn = lfd_0_1 + (r << 21) + (1 << 16) - (32 - r) * 8;
  write_insn<big_endian>(p, insn);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
restfpr0_tail(unsigned char* p, int r)
{
  write_insn<big_endian>(p, ld_0_1 + 16);
  p = p + 4;
  p = restfpr<big_endian>(p, r);
  write_insn<big_endian>(p, mtlr_0);
  p = p + 4;
  if (r == 29)
    {
      p = restfpr<big_endian>(p, 30);
      p = restfpr<big_endian>(p, 31);
    }
  write_insn<big_endian>(p, blr);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
savefpr1_tail(unsigned char* p, int r)
{
  p = savefpr<big_endian>(p, r);
  write_insn<big_endian>(p, blr);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
restfpr1_tail(unsigned char* p, int r)
{
  p = restfpr<big_endian>(p, r);
  write_insn<big_endian>(p, blr);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
savevr(unsigned char* p, int r)
{
  uint32_t insn = li_12_0 + (1 << 16) - (32 - r) * 16;
  write_insn<big_endian>(p, insn);
  p = p + 4;
  insn = stvx_0_12_0 + (r << 21);
  write_insn<big_endian>(p, insn);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
savevr_tail(unsigned char* p, int r)
{
  p = savevr<big_endian>(p, r);
  write_insn<big_endian>(p, blr);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
restvr(unsigned char* p, int r)
{
  uint32_t insn = li_12_0 + (1 << 16) - (32 - r) * 16;
  write_insn<big_endian>(p, insn);
  p = p + 4;
  insn = lvx_0_12_0 + (r << 21);
  write_insn<big_endian>(p, insn);
  return p + 4;
}

template<bool big_endian>
static unsigned char*
restvr_tail(unsigned char* p, int r)
{
  p = restvr<big_endian>(p, r);
  write_insn<big_endian>(p, blr);
  return p + 4;
}


template<int size, bool big_endian>
Output_data_save_res<size, big_endian>::Output_data_save_res(
    Symbol_table* symtab)
  : Output_section_data_build(4),
    contents_(NULL)
{
  this->savres_define(symtab,
		      "_savegpr0_", 14, 31,
		      savegpr0<big_endian>, savegpr0_tail<big_endian>);
  this->savres_define(symtab,
		      "_restgpr0_", 14, 29,
		      restgpr0<big_endian>, restgpr0_tail<big_endian>);
  this->savres_define(symtab,
		      "_restgpr0_", 30, 31,
		      restgpr0<big_endian>, restgpr0_tail<big_endian>);
  this->savres_define(symtab,
		      "_savegpr1_", 14, 31,
		      savegpr1<big_endian>, savegpr1_tail<big_endian>);
  this->savres_define(symtab,
		      "_restgpr1_", 14, 31,
		      restgpr1<big_endian>, restgpr1_tail<big_endian>);
  this->savres_define(symtab,
		      "_savefpr_", 14, 31,
		      savefpr<big_endian>, savefpr0_tail<big_endian>);
  this->savres_define(symtab,
		      "_restfpr_", 14, 29,
		      restfpr<big_endian>, restfpr0_tail<big_endian>);
  this->savres_define(symtab,
		      "_restfpr_", 30, 31,
		      restfpr<big_endian>, restfpr0_tail<big_endian>);
  this->savres_define(symtab,
		      "._savef", 14, 31,
		      savefpr<big_endian>, savefpr1_tail<big_endian>);
  this->savres_define(symtab,
		      "._restf", 14, 31,
		      restfpr<big_endian>, restfpr1_tail<big_endian>);
  this->savres_define(symtab,
		      "_savevr_", 20, 31,
		      savevr<big_endian>, savevr_tail<big_endian>);
  this->savres_define(symtab,
		      "_restvr_", 20, 31,
		      restvr<big_endian>, restvr_tail<big_endian>);
}

template<int size, bool big_endian>
void
Output_data_save_res<size, big_endian>::savres_define(
    Symbol_table* symtab,
    const char *name,
    unsigned int lo, unsigned int hi,
    unsigned char* write_ent(unsigned char*, int),
    unsigned char* write_tail(unsigned char*, int))
{
  size_t len = strlen(name);
  bool writing = false;
  char sym[16];

  memcpy(sym, name, len);
  sym[len + 2] = 0;

  for (unsigned int i = lo; i <= hi; i++)
    {
      sym[len + 0] = i / 10 + '0';
      sym[len + 1] = i % 10 + '0';
      Symbol* gsym = symtab->lookup(sym);
      bool refd = gsym != NULL && gsym->is_undefined();
      writing = writing || refd;
      if (writing)
	{
	  if (this->contents_ == NULL)
	    this->contents_ = new unsigned char[this->savres_max];

	  section_size_type value = this->current_data_size();
	  unsigned char* p = this->contents_ + value;
	  if (i != hi)
	    p = write_ent(p, i);
	  else
	    p = write_tail(p, i);
	  section_size_type cur_size = p - this->contents_;
	  this->set_current_data_size(cur_size);
	  if (refd)
	    symtab->define_in_output_data(sym, NULL, Symbol_table::PREDEFINED,
					  this, value, cur_size - value,
					  elfcpp::STT_FUNC, elfcpp::STB_GLOBAL,
					  elfcpp::STV_HIDDEN, 0, false, false);
	}
    }
}

// Write out save/restore.

template<int size, bool big_endian>
void
Output_data_save_res<size, big_endian>::do_write(Output_file* of)
{
  const section_size_type off = this->offset();
  const section_size_type oview_size =
    convert_to_section_size_type(this->data_size());
  unsigned char* const oview = of->get_output_view(off, oview_size);
  memcpy(oview, this->contents_, oview_size);
  of->write_output_view(off, oview_size, oview);
}


// Create the glink section.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::make_glink_section(Layout* layout)
{
  if (this->glink_ == NULL)
    {
      this->glink_ = new Output_data_glink<size, big_endian>(this);
      this->glink_->add_eh_frame(layout);
      layout->add_output_section_data(".text", elfcpp::SHT_PROGBITS,
				      elfcpp::SHF_ALLOC | elfcpp::SHF_EXECINSTR,
				      this->glink_, ORDER_TEXT, false);
    }
}

// Create a PLT entry for a global symbol.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::make_plt_entry(Symbol_table* symtab,
						 Layout* layout,
						 Symbol* gsym)
{
  if (gsym->type() == elfcpp::STT_GNU_IFUNC
      && gsym->can_use_relative_reloc(false))
    {
      if (this->iplt_ == NULL)
	this->make_iplt_section(symtab, layout);
      this->iplt_->add_ifunc_entry(gsym);
    }
  else
    {
      if (this->plt_ == NULL)
	this->make_plt_section(symtab, layout);
      this->plt_->add_entry(gsym);
    }
}

// Make a PLT entry for a local STT_GNU_IFUNC symbol.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::make_local_ifunc_plt_entry(
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj_file<size, big_endian>* relobj,
    unsigned int r_sym)
{
  if (this->iplt_ == NULL)
    this->make_iplt_section(symtab, layout);
  this->iplt_->add_local_ifunc_entry(relobj, r_sym);
}

// Return the number of entries in the PLT.

template<int size, bool big_endian>
unsigned int
Target_powerpc<size, big_endian>::plt_entry_count() const
{
  if (this->plt_ == NULL)
    return 0;
  return this->plt_->entry_count();
}

// Return the offset of the first non-reserved PLT entry.

template<int size, bool big_endian>
unsigned int
Target_powerpc<size, big_endian>::first_plt_entry_offset() const
{
  return this->plt_->first_plt_entry_offset();
}

// Return the size of each PLT entry.

template<int size, bool big_endian>
unsigned int
Target_powerpc<size, big_endian>::plt_entry_size() const
{
  return Output_data_plt_powerpc<size, big_endian>::get_plt_entry_size();
}

// Create a GOT entry for local dynamic __tls_get_addr calls.

template<int size, bool big_endian>
unsigned int
Target_powerpc<size, big_endian>::tlsld_got_offset(
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj_file<size, big_endian>* object)
{
  if (this->tlsld_got_offset_ == -1U)
    {
      gold_assert(symtab != NULL && layout != NULL && object != NULL);
      Reloc_section* rela_dyn = this->rela_dyn_section(layout);
      Output_data_got_powerpc<size, big_endian>* got
	= this->got_section(symtab, layout);
      unsigned int got_offset = got->add_constant_pair(0, 0);
      rela_dyn->add_local(object, 0, elfcpp::R_POWERPC_DTPMOD, got,
			  got_offset, 0);
      this->tlsld_got_offset_ = got_offset;
    }
  return this->tlsld_got_offset_;
}

// Get the Reference_flags for a particular relocation.

template<int size, bool big_endian>
int
Target_powerpc<size, big_endian>::Scan::get_reference_flags(unsigned int r_type)
{
  switch (r_type)
    {
    case elfcpp::R_POWERPC_NONE:
    case elfcpp::R_POWERPC_GNU_VTINHERIT:
    case elfcpp::R_POWERPC_GNU_VTENTRY:
    case elfcpp::R_PPC64_TOC:
      // No symbol reference.
      return 0;

    case elfcpp::R_PPC64_ADDR64:
    case elfcpp::R_PPC64_UADDR64:
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_UADDR16:
    case elfcpp::R_POWERPC_ADDR16_LO:
    case elfcpp::R_POWERPC_ADDR16_HI:
    case elfcpp::R_POWERPC_ADDR16_HA:
      return Symbol::ABSOLUTE_REF;

    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
      return Symbol::FUNCTION_CALL | Symbol::ABSOLUTE_REF;

    case elfcpp::R_PPC64_REL64:
    case elfcpp::R_POWERPC_REL32:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_POWERPC_REL16_HA:
      return Symbol::RELATIVE_REF;

    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      return Symbol::FUNCTION_CALL | Symbol::RELATIVE_REF;

    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_GOT16_LO:
    case elfcpp::R_POWERPC_GOT16_HI:
    case elfcpp::R_POWERPC_GOT16_HA:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_GOT16_LO_DS:
    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
      // Absolute in GOT.
      return Symbol::ABSOLUTE_REF;

    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_TLS:
      return Symbol::TLS_REF;

    case elfcpp::R_POWERPC_COPY:
    case elfcpp::R_POWERPC_GLOB_DAT:
    case elfcpp::R_POWERPC_JMP_SLOT:
    case elfcpp::R_POWERPC_RELATIVE:
    case elfcpp::R_POWERPC_DTPMOD:
    default:
      // Not expected.  We will give an error later.
      return 0;
    }
}

// Report an unsupported relocation against a local symbol.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::Scan::unsupported_reloc_local(
    Sized_relobj_file<size, big_endian>* object,
    unsigned int r_type)
{
  gold_error(_("%s: unsupported reloc %u against local symbol"),
	     object->name().c_str(), r_type);
}

// We are about to emit a dynamic relocation of type R_TYPE.  If the
// dynamic linker does not support it, issue an error.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::Scan::check_non_pic(Relobj* object,
						      unsigned int r_type)
{
  gold_assert(r_type != elfcpp::R_POWERPC_NONE);

  // These are the relocation types supported by glibc for both 32-bit
  // and 64-bit powerpc.
  switch (r_type)
    {
    case elfcpp::R_POWERPC_NONE:
    case elfcpp::R_POWERPC_RELATIVE:
    case elfcpp::R_POWERPC_GLOB_DAT:
    case elfcpp::R_POWERPC_DTPMOD:
    case elfcpp::R_POWERPC_DTPREL:
    case elfcpp::R_POWERPC_TPREL:
    case elfcpp::R_POWERPC_JMP_SLOT:
    case elfcpp::R_POWERPC_COPY:
    case elfcpp::R_POWERPC_IRELATIVE:
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_UADDR16:
    case elfcpp::R_POWERPC_ADDR16_LO:
    case elfcpp::R_POWERPC_ADDR16_HI:
    case elfcpp::R_POWERPC_ADDR16_HA:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
    case elfcpp::R_POWERPC_REL32:
    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_TPREL16_LO:
    case elfcpp::R_POWERPC_TPREL16_HI:
    case elfcpp::R_POWERPC_TPREL16_HA:
      return;

    default:
      break;
    }

  if (size == 64)
    {
      switch (r_type)
	{
	  // These are the relocation types supported only on 64-bit.
	case elfcpp::R_PPC64_ADDR64:
	case elfcpp::R_PPC64_UADDR64:
	case elfcpp::R_PPC64_JMP_IREL:
	case elfcpp::R_PPC64_ADDR16_DS:
	case elfcpp::R_PPC64_ADDR16_LO_DS:
	case elfcpp::R_PPC64_ADDR16_HIGHER:
	case elfcpp::R_PPC64_ADDR16_HIGHEST:
	case elfcpp::R_PPC64_ADDR16_HIGHERA:
	case elfcpp::R_PPC64_ADDR16_HIGHESTA:
	case elfcpp::R_PPC64_REL64:
	case elfcpp::R_POWERPC_ADDR30:
	case elfcpp::R_PPC64_TPREL16_DS:
	case elfcpp::R_PPC64_TPREL16_LO_DS:
	case elfcpp::R_PPC64_TPREL16_HIGHER:
	case elfcpp::R_PPC64_TPREL16_HIGHEST:
	case elfcpp::R_PPC64_TPREL16_HIGHERA:
	case elfcpp::R_PPC64_TPREL16_HIGHESTA:
	  return;

	default:
	  break;
	}
    }
  else
    {
      switch (r_type)
	{
	  // These are the relocation types supported only on 32-bit.
	  // ??? glibc ld.so doesn't need to support these.
	case elfcpp::R_POWERPC_DTPREL16:
	case elfcpp::R_POWERPC_DTPREL16_LO:
	case elfcpp::R_POWERPC_DTPREL16_HI:
	case elfcpp::R_POWERPC_DTPREL16_HA:
	  return;

	default:
	  break;
	}
    }

  // This prevents us from issuing more than one error per reloc
  // section.  But we can still wind up issuing more than one
  // error per object file.
  if (this->issued_non_pic_error_)
    return;
  gold_assert(parameters->options().output_is_position_independent());
  object->error(_("requires unsupported dynamic reloc; "
		  "recompile with -fPIC"));
  this->issued_non_pic_error_ = true;
  return;
}

// Return whether we need to make a PLT entry for a relocation of the
// given type against a STT_GNU_IFUNC symbol.

template<int size, bool big_endian>
bool
Target_powerpc<size, big_endian>::Scan::reloc_needs_plt_for_ifunc(
     Sized_relobj_file<size, big_endian>* object,
     unsigned int r_type,
     bool report_err)
{
  // In non-pic code any reference will resolve to the plt call stub
  // for the ifunc symbol.
  if (size == 32 && !parameters->options().output_is_position_independent())
    return true;

  switch (r_type)
    {
    // Word size refs from data sections are OK, but don't need a PLT entry.
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
      if (size == 32)
	return false;
      break;

    case elfcpp::R_PPC64_ADDR64:
    case elfcpp::R_PPC64_UADDR64:
      if (size == 64)
	return false;
      break;

    // GOT refs are good, but also don't need a PLT entry.
    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_GOT16_LO:
    case elfcpp::R_POWERPC_GOT16_HI:
    case elfcpp::R_POWERPC_GOT16_HA:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_GOT16_LO_DS:
      return false;

    // Function calls are good, and these do need a PLT entry.
    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      return true;

    default:
      break;
    }

  // Anything else is a problem.
  // If we are building a static executable, the libc startup function
  // responsible for applying indirect function relocations is going
  // to complain about the reloc type.
  // If we are building a dynamic executable, we will have a text
  // relocation.  The dynamic loader will set the text segment
  // writable and non-executable to apply text relocations.  So we'll
  // segfault when trying to run the indirection function to resolve
  // the reloc.
  if (report_err)
    gold_error(_("%s: unsupported reloc %u for IFUNC symbol"),
	       object->name().c_str(), r_type);
  return false;
}

// Scan a relocation for a local symbol.

template<int size, bool big_endian>
inline void
Target_powerpc<size, big_endian>::Scan::local(
    Symbol_table* symtab,
    Layout* layout,
    Target_powerpc<size, big_endian>* target,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    Output_section* output_section,
    const elfcpp::Rela<size, big_endian>& reloc,
    unsigned int r_type,
    const elfcpp::Sym<size, big_endian>& lsym,
    bool is_discarded)
{
  this->maybe_skip_tls_get_addr_call(r_type, NULL);

  if ((size == 64 && r_type == elfcpp::R_PPC64_TLSGD)
      || (size == 32 && r_type == elfcpp::R_PPC_TLSGD))
    {
      this->expect_tls_get_addr_call();
      const tls::Tls_optimization tls_type = target->optimize_tls_gd(true);
      if (tls_type != tls::TLSOPT_NONE)
	this->skip_next_tls_get_addr_call();
    }
  else if ((size == 64 && r_type == elfcpp::R_PPC64_TLSLD)
	   || (size == 32 && r_type == elfcpp::R_PPC_TLSLD))
    {
      this->expect_tls_get_addr_call();
      const tls::Tls_optimization tls_type = target->optimize_tls_ld();
      if (tls_type != tls::TLSOPT_NONE)
	this->skip_next_tls_get_addr_call();
    }

  Powerpc_relobj<size, big_endian>* ppc_object
    = static_cast<Powerpc_relobj<size, big_endian>*>(object);

  if (is_discarded)
    {
      if (size == 64
	  && data_shndx == ppc_object->opd_shndx()
	  && r_type == elfcpp::R_PPC64_ADDR64)
	ppc_object->set_opd_discard(reloc.get_r_offset());
      return;
    }

  // A local STT_GNU_IFUNC symbol may require a PLT entry.
  bool is_ifunc = lsym.get_st_type() == elfcpp::STT_GNU_IFUNC;
  if (is_ifunc && this->reloc_needs_plt_for_ifunc(object, r_type, true))
    {
      unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
      target->push_branch(ppc_object, data_shndx, reloc.get_r_offset(),
			  r_type, r_sym, reloc.get_r_addend());
      target->make_local_ifunc_plt_entry(symtab, layout, object, r_sym);
    }

  switch (r_type)
    {
    case elfcpp::R_POWERPC_NONE:
    case elfcpp::R_POWERPC_GNU_VTINHERIT:
    case elfcpp::R_POWERPC_GNU_VTENTRY:
    case elfcpp::R_PPC64_TOCSAVE:
    case elfcpp::R_PPC_EMB_MRKREF:
    case elfcpp::R_POWERPC_TLS:
      break;

    case elfcpp::R_PPC64_TOC:
      {
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	if (parameters->options().output_is_position_independent())
	  {
	    Address off = reloc.get_r_offset();
	    if (size == 64
		&& data_shndx == ppc_object->opd_shndx()
		&& ppc_object->get_opd_discard(off - 8))
	      break;

	    Reloc_section* rela_dyn = target->rela_dyn_section(layout);
	    Powerpc_relobj<size, big_endian>* symobj = ppc_object;
	    rela_dyn->add_output_section_relative(got->output_section(),
						  elfcpp::R_POWERPC_RELATIVE,
						  output_section,
						  object, data_shndx, off,
						  symobj->toc_base_offset());
	  }
      }
      break;

    case elfcpp::R_PPC64_ADDR64:
    case elfcpp::R_PPC64_UADDR64:
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_ADDR16_LO:
    case elfcpp::R_POWERPC_ADDR16_HI:
    case elfcpp::R_POWERPC_ADDR16_HA:
    case elfcpp::R_POWERPC_UADDR16:
    case elfcpp::R_PPC64_ADDR16_HIGHER:
    case elfcpp::R_PPC64_ADDR16_HIGHERA:
    case elfcpp::R_PPC64_ADDR16_HIGHEST:
    case elfcpp::R_PPC64_ADDR16_HIGHESTA:
    case elfcpp::R_PPC64_ADDR16_DS:
    case elfcpp::R_PPC64_ADDR16_LO_DS:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
      // If building a shared library (or a position-independent
      // executable), we need to create a dynamic relocation for
      // this location.
      if (parameters->options().output_is_position_independent()
	  || (size == 64 && is_ifunc))
	{
	  Reloc_section* rela_dyn = target->rela_dyn_section(symtab, layout,
							     is_ifunc);
	  if ((size == 32 && r_type == elfcpp::R_POWERPC_ADDR32)
	      || (size == 64 && r_type == elfcpp::R_PPC64_ADDR64))
	    {
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	      unsigned int dynrel = (is_ifunc ? elfcpp::R_POWERPC_IRELATIVE
				     : elfcpp::R_POWERPC_RELATIVE);
	      rela_dyn->add_local_relative(object, r_sym, dynrel,
					   output_section, data_shndx,
					   reloc.get_r_offset(),
					   reloc.get_r_addend(), false);
	    }
	  else
	    {
	      check_non_pic(object, r_type);
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	      rela_dyn->add_local(object, r_sym, r_type, output_section,
				  data_shndx, reloc.get_r_offset(),
				  reloc.get_r_addend());
	    }
	}
      break;

    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      if (!is_ifunc)
	target->push_branch(ppc_object, data_shndx, reloc.get_r_offset(),
			    r_type, elfcpp::elf_r_sym<size>(reloc.get_r_info()),
			    reloc.get_r_addend());
      break;

    case elfcpp::R_PPC64_REL64:
    case elfcpp::R_POWERPC_REL32:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_POWERPC_REL16_HA:
    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_POWERPC_SECTOFF_LO:
    case elfcpp::R_POWERPC_TPREL16_LO:
    case elfcpp::R_POWERPC_DTPREL16_LO:
    case elfcpp::R_POWERPC_SECTOFF_HI:
    case elfcpp::R_POWERPC_TPREL16_HI:
    case elfcpp::R_POWERPC_DTPREL16_HI:
    case elfcpp::R_POWERPC_SECTOFF_HA:
    case elfcpp::R_POWERPC_TPREL16_HA:
    case elfcpp::R_POWERPC_DTPREL16_HA:
    case elfcpp::R_PPC64_DTPREL16_HIGHER:
    case elfcpp::R_PPC64_TPREL16_HIGHER:
    case elfcpp::R_PPC64_DTPREL16_HIGHERA:
    case elfcpp::R_PPC64_TPREL16_HIGHERA:
    case elfcpp::R_PPC64_DTPREL16_HIGHEST:
    case elfcpp::R_PPC64_TPREL16_HIGHEST:
    case elfcpp::R_PPC64_DTPREL16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_TPREL16_LO_DS:
    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_LO_DS:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_PPC64_SECTOFF_LO_DS:
    case elfcpp::R_PPC64_TLSGD:
    case elfcpp::R_PPC64_TLSLD:
      break;

    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_GOT16_LO:
    case elfcpp::R_POWERPC_GOT16_HI:
    case elfcpp::R_POWERPC_GOT16_HA:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_GOT16_LO_DS:
      {
	// The symbol requires a GOT entry.
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());

	if (!parameters->options().output_is_position_independent())
	  {
	    if (size == 32 && is_ifunc)
	      got->add_local_plt(object, r_sym, GOT_TYPE_STANDARD);
	    else
	      got->add_local(object, r_sym, GOT_TYPE_STANDARD);
	  }
	else if (!object->local_has_got_offset(r_sym, GOT_TYPE_STANDARD))
	  {
	    // If we are generating a shared object or a pie, this
	    // symbol's GOT entry will be set by a dynamic relocation.
	    unsigned int off;
	    off = got->add_constant(0);
	    object->set_local_got_offset(r_sym, GOT_TYPE_STANDARD, off);

	    Reloc_section* rela_dyn = target->rela_dyn_section(symtab, layout,
							       is_ifunc);
	    unsigned int dynrel = (is_ifunc ? elfcpp::R_POWERPC_IRELATIVE
				   : elfcpp::R_POWERPC_RELATIVE);
	    rela_dyn->add_local_relative(object, r_sym, dynrel,
					 got, off, 0, false);
	  }
      }
      break;

    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
      // We need a GOT section.
      target->got_section(symtab, layout);
      break;

    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TLSGD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HA:
      {
	const tls::Tls_optimization tls_type = target->optimize_tls_gd(true);
	if (tls_type == tls::TLSOPT_NONE)
	  {
	    Output_data_got_powerpc<size, big_endian>* got
	      = target->got_section(symtab, layout);
	    unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	    Reloc_section* rela_dyn = target->rela_dyn_section(layout);
	    got->add_local_tls_pair(object, r_sym, GOT_TYPE_TLSGD,
				    rela_dyn, elfcpp::R_POWERPC_DTPMOD);
	  }
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TLSLD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HA:
      {
	const tls::Tls_optimization tls_type = target->optimize_tls_ld();
	if (tls_type == tls::TLSOPT_NONE)
	  target->tlsld_got_offset(symtab, layout, object);
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	    if (parameters->options().emit_relocs())
	      {
		Output_section* os = layout->tls_segment()->first_section();
		gold_assert(os != NULL);
		os->set_needs_symtab_index();
	      }
	  }
	else
	  gold_unreachable();
      }
      break;

    case elfcpp::R_POWERPC_GOT_DTPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16_LO:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HI:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HA:
      {
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	got->add_local_tls(object, r_sym, GOT_TYPE_DTPREL);
      }
      break;

    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_GOT_TPREL16_LO:
    case elfcpp::R_POWERPC_GOT_TPREL16_HI:
    case elfcpp::R_POWERPC_GOT_TPREL16_HA:
      {
	const tls::Tls_optimization tls_type = target->optimize_tls_ie(true);
	if (tls_type == tls::TLSOPT_NONE)
	  {
	    unsigned int r_sym = elfcpp::elf_r_sym<size>(reloc.get_r_info());
	    if (!object->local_has_got_offset(r_sym, GOT_TYPE_TPREL))
	      {
		Output_data_got_powerpc<size, big_endian>* got
		  = target->got_section(symtab, layout);
		unsigned int off = got->add_constant(0);
		object->set_local_got_offset(r_sym, GOT_TYPE_TPREL, off);

		Reloc_section* rela_dyn = target->rela_dyn_section(layout);
		rela_dyn->add_symbolless_local_addend(object, r_sym,
						      elfcpp::R_POWERPC_TPREL,
						      got, off, 0);
	      }
	  }
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    default:
      unsupported_reloc_local(object, r_type);
      break;
    }

  switch (r_type)
    {
    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16:
    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_DS:
      ppc_object->set_has_small_toc_reloc();
    default:
      break;
    }
}

// Report an unsupported relocation against a global symbol.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::Scan::unsupported_reloc_global(
    Sized_relobj_file<size, big_endian>* object,
    unsigned int r_type,
    Symbol* gsym)
{
  gold_error(_("%s: unsupported reloc %u against global symbol %s"),
	     object->name().c_str(), r_type, gsym->demangled_name().c_str());
}

// Scan a relocation for a global symbol.

template<int size, bool big_endian>
inline void
Target_powerpc<size, big_endian>::Scan::global(
    Symbol_table* symtab,
    Layout* layout,
    Target_powerpc<size, big_endian>* target,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    Output_section* output_section,
    const elfcpp::Rela<size, big_endian>& reloc,
    unsigned int r_type,
    Symbol* gsym)
{
  if (this->maybe_skip_tls_get_addr_call(r_type, gsym) == Track_tls::SKIP)
    return;

  if ((size == 64 && r_type == elfcpp::R_PPC64_TLSGD)
      || (size == 32 && r_type == elfcpp::R_PPC_TLSGD))
    {
      this->expect_tls_get_addr_call();
      const bool final = gsym->final_value_is_known();
      const tls::Tls_optimization tls_type = target->optimize_tls_gd(final);
      if (tls_type != tls::TLSOPT_NONE)
	this->skip_next_tls_get_addr_call();
    }
  else if ((size == 64 && r_type == elfcpp::R_PPC64_TLSLD)
	   || (size == 32 && r_type == elfcpp::R_PPC_TLSLD))
    {
      this->expect_tls_get_addr_call();
      const tls::Tls_optimization tls_type = target->optimize_tls_ld();
      if (tls_type != tls::TLSOPT_NONE)
	this->skip_next_tls_get_addr_call();
    }

  Powerpc_relobj<size, big_endian>* ppc_object
    = static_cast<Powerpc_relobj<size, big_endian>*>(object);

  // A STT_GNU_IFUNC symbol may require a PLT entry.
  bool is_ifunc = gsym->type() == elfcpp::STT_GNU_IFUNC;
  if (is_ifunc && this->reloc_needs_plt_for_ifunc(object, r_type, true))
    {
      target->push_branch(ppc_object, data_shndx, reloc.get_r_offset(),
			  r_type, elfcpp::elf_r_sym<size>(reloc.get_r_info()),
			  reloc.get_r_addend());
      target->make_plt_entry(symtab, layout, gsym);
    }

  switch (r_type)
    {
    case elfcpp::R_POWERPC_NONE:
    case elfcpp::R_POWERPC_GNU_VTINHERIT:
    case elfcpp::R_POWERPC_GNU_VTENTRY:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_PPC_EMB_MRKREF:
    case elfcpp::R_POWERPC_TLS:
      break;

    case elfcpp::R_PPC64_TOC:
      {
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	if (parameters->options().output_is_position_independent())
	  {
	    Address off = reloc.get_r_offset();
	    if (size == 64
		&& data_shndx == ppc_object->opd_shndx()
		&& ppc_object->get_opd_discard(off - 8))
	      break;

	    Reloc_section* rela_dyn = target->rela_dyn_section(layout);
	    Powerpc_relobj<size, big_endian>* symobj = ppc_object;
	    if (data_shndx != ppc_object->opd_shndx())
	      symobj = static_cast
		<Powerpc_relobj<size, big_endian>*>(gsym->object());
	    rela_dyn->add_output_section_relative(got->output_section(),
						  elfcpp::R_POWERPC_RELATIVE,
						  output_section,
						  object, data_shndx, off,
						  symobj->toc_base_offset());
	  }
      }
      break;

    case elfcpp::R_PPC64_ADDR64:
      if (size == 64
	  && data_shndx == ppc_object->opd_shndx()
	  && (gsym->is_defined_in_discarded_section()
	      || gsym->object() != object))
	{
	  ppc_object->set_opd_discard(reloc.get_r_offset());
	  break;
	}
      // Fall thru
    case elfcpp::R_PPC64_UADDR64:
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_ADDR16_LO:
    case elfcpp::R_POWERPC_ADDR16_HI:
    case elfcpp::R_POWERPC_ADDR16_HA:
    case elfcpp::R_POWERPC_UADDR16:
    case elfcpp::R_PPC64_ADDR16_HIGHER:
    case elfcpp::R_PPC64_ADDR16_HIGHERA:
    case elfcpp::R_PPC64_ADDR16_HIGHEST:
    case elfcpp::R_PPC64_ADDR16_HIGHESTA:
    case elfcpp::R_PPC64_ADDR16_DS:
    case elfcpp::R_PPC64_ADDR16_LO_DS:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
      {
	// Make a PLT entry if necessary.
	if (gsym->needs_plt_entry())
	  {
	    if (!is_ifunc)
	      {
		target->push_branch(ppc_object, data_shndx,
				    reloc.get_r_offset(), r_type,
				    elfcpp::elf_r_sym<size>(reloc.get_r_info()),
				    reloc.get_r_addend());
		target->make_plt_entry(symtab, layout, gsym);
	      }
	    // Since this is not a PC-relative relocation, we may be
	    // taking the address of a function. In that case we need to
	    // set the entry in the dynamic symbol table to the address of
	    // the PLT call stub.
	    if (size == 32
		&& gsym->is_from_dynobj()
		&& !parameters->options().output_is_position_independent())
	      gsym->set_needs_dynsym_value();
	  }
	// Make a dynamic relocation if necessary.
	if (needs_dynamic_reloc<size>(gsym, Scan::get_reference_flags(r_type))
	    || (size == 64 && is_ifunc))
	  {
	    if (gsym->may_need_copy_reloc())
	      {
		target->copy_reloc(symtab, layout, object,
				   data_shndx, output_section, gsym, reloc);
	      }
	    else if ((size == 32
		      && r_type == elfcpp::R_POWERPC_ADDR32
		      && gsym->can_use_relative_reloc(false)
		      && !(gsym->visibility() == elfcpp::STV_PROTECTED
			   && parameters->options().shared()))
		     || (size == 64
			 && r_type == elfcpp::R_PPC64_ADDR64
			 && (gsym->can_use_relative_reloc(false)
			     || data_shndx == ppc_object->opd_shndx())))
	      {
		Reloc_section* rela_dyn
		  = target->rela_dyn_section(symtab, layout, is_ifunc);
		unsigned int dynrel = (is_ifunc ? elfcpp::R_POWERPC_IRELATIVE
				       : elfcpp::R_POWERPC_RELATIVE);
		rela_dyn->add_symbolless_global_addend(
		    gsym, dynrel, output_section, object, data_shndx,
		    reloc.get_r_offset(), reloc.get_r_addend());
	      }
	    else
	      {
		Reloc_section* rela_dyn
		  = target->rela_dyn_section(symtab, layout, is_ifunc);
		check_non_pic(object, r_type);
		rela_dyn->add_global(gsym, r_type, output_section,
				     object, data_shndx,
				     reloc.get_r_offset(),
				     reloc.get_r_addend());
	      }
	  }
      }
      break;

    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_POWERPC_REL24:
      if (!is_ifunc)
	{
	  target->push_branch(ppc_object, data_shndx, reloc.get_r_offset(),
			      r_type,
			      elfcpp::elf_r_sym<size>(reloc.get_r_info()),
			      reloc.get_r_addend());
	  if (gsym->needs_plt_entry()
	      || (!gsym->final_value_is_known()
		  && (gsym->is_undefined()
		      || gsym->is_from_dynobj()
		      || gsym->is_preemptible())))
	    target->make_plt_entry(symtab, layout, gsym);
	}
      // Fall thru

    case elfcpp::R_PPC64_REL64:
    case elfcpp::R_POWERPC_REL32:
      // Make a dynamic relocation if necessary.
      if (needs_dynamic_reloc<size>(gsym, Scan::get_reference_flags(r_type)))
	{
	  if (gsym->may_need_copy_reloc())
	    {
	      target->copy_reloc(symtab, layout, object,
				 data_shndx, output_section, gsym,
				 reloc);
	    }
	  else
	    {
	      Reloc_section* rela_dyn
		= target->rela_dyn_section(symtab, layout, is_ifunc);
	      check_non_pic(object, r_type);
	      rela_dyn->add_global(gsym, r_type, output_section, object,
				   data_shndx, reloc.get_r_offset(),
				   reloc.get_r_addend());
	    }
	}
      break;

    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      if (!is_ifunc)
	target->push_branch(ppc_object, data_shndx, reloc.get_r_offset(),
			    r_type, elfcpp::elf_r_sym<size>(reloc.get_r_info()),
			    reloc.get_r_addend());
      break;

    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_POWERPC_REL16_HA:
    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_POWERPC_SECTOFF_LO:
    case elfcpp::R_POWERPC_TPREL16_LO:
    case elfcpp::R_POWERPC_DTPREL16_LO:
    case elfcpp::R_POWERPC_SECTOFF_HI:
    case elfcpp::R_POWERPC_TPREL16_HI:
    case elfcpp::R_POWERPC_DTPREL16_HI:
    case elfcpp::R_POWERPC_SECTOFF_HA:
    case elfcpp::R_POWERPC_TPREL16_HA:
    case elfcpp::R_POWERPC_DTPREL16_HA:
    case elfcpp::R_PPC64_DTPREL16_HIGHER:
    case elfcpp::R_PPC64_TPREL16_HIGHER:
    case elfcpp::R_PPC64_DTPREL16_HIGHERA:
    case elfcpp::R_PPC64_TPREL16_HIGHERA:
    case elfcpp::R_PPC64_DTPREL16_HIGHEST:
    case elfcpp::R_PPC64_TPREL16_HIGHEST:
    case elfcpp::R_PPC64_DTPREL16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_TPREL16_LO_DS:
    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_LO_DS:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_PPC64_SECTOFF_LO_DS:
    case elfcpp::R_PPC64_TLSGD:
    case elfcpp::R_PPC64_TLSLD:
      break;

    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_GOT16_LO:
    case elfcpp::R_POWERPC_GOT16_HI:
    case elfcpp::R_POWERPC_GOT16_HA:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_GOT16_LO_DS:
      {
	// The symbol requires a GOT entry.
	Output_data_got_powerpc<size, big_endian>* got;

	got = target->got_section(symtab, layout);
	if (gsym->final_value_is_known())
	  {
	    if (size == 32 && is_ifunc)
	      got->add_global_plt(gsym, GOT_TYPE_STANDARD);
	    else
	      got->add_global(gsym, GOT_TYPE_STANDARD);
	  }
	else if (!gsym->has_got_offset(GOT_TYPE_STANDARD))
	  {
	    // If we are generating a shared object or a pie, this
	    // symbol's GOT entry will be set by a dynamic relocation.
	    unsigned int off = got->add_constant(0);
	    gsym->set_got_offset(GOT_TYPE_STANDARD, off);

	    Reloc_section* rela_dyn
	      = target->rela_dyn_section(symtab, layout, is_ifunc);

	    if (gsym->can_use_relative_reloc(false)
		&& !(size == 32
		     && gsym->visibility() == elfcpp::STV_PROTECTED
		     && parameters->options().shared()))
	      {
		unsigned int dynrel = (is_ifunc ? elfcpp::R_POWERPC_IRELATIVE
				       : elfcpp::R_POWERPC_RELATIVE);
		rela_dyn->add_global_relative(gsym, dynrel, got, off, 0, false);
	      }
	    else
	      {
		unsigned int dynrel = elfcpp::R_POWERPC_GLOB_DAT;
		rela_dyn->add_global(gsym, dynrel, got, off, 0);
	      }
	  }
      }
      break;

    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
      // We need a GOT section.
      target->got_section(symtab, layout);
      break;

    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TLSGD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HA:
      {
	const bool final = gsym->final_value_is_known();
	const tls::Tls_optimization tls_type = target->optimize_tls_gd(final);
	if (tls_type == tls::TLSOPT_NONE)
	  {
	    Output_data_got_powerpc<size, big_endian>* got
	      = target->got_section(symtab, layout);
	    Reloc_section* rela_dyn = target->rela_dyn_section(layout);
	    got->add_global_pair_with_rel(gsym, GOT_TYPE_TLSGD, rela_dyn,
					  elfcpp::R_POWERPC_DTPMOD,
					  elfcpp::R_POWERPC_DTPREL);
	  }
	else if (tls_type == tls::TLSOPT_TO_IE)
	  {
	    if (!gsym->has_got_offset(GOT_TYPE_TPREL))
	      {
		Output_data_got_powerpc<size, big_endian>* got
		  = target->got_section(symtab, layout);
		Reloc_section* rela_dyn = target->rela_dyn_section(layout);
		if (gsym->is_undefined()
		    || gsym->is_from_dynobj())
		  {
		    got->add_global_with_rel(gsym, GOT_TYPE_TPREL, rela_dyn,
					     elfcpp::R_POWERPC_TPREL);
		  }
		else
		  {
		    unsigned int off = got->add_constant(0);
		    gsym->set_got_offset(GOT_TYPE_TPREL, off);
		    unsigned int dynrel = elfcpp::R_POWERPC_TPREL;
		    rela_dyn->add_symbolless_global_addend(gsym, dynrel,
							   got, off, 0);
		  }
	      }
	  }
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TLSLD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HA:
      {
	const tls::Tls_optimization tls_type = target->optimize_tls_ld();
	if (tls_type == tls::TLSOPT_NONE)
	  target->tlsld_got_offset(symtab, layout, object);
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	    if (parameters->options().emit_relocs())
	      {
		Output_section* os = layout->tls_segment()->first_section();
		gold_assert(os != NULL);
		os->set_needs_symtab_index();
	      }
	  }
	else
	  gold_unreachable();
      }
      break;

    case elfcpp::R_POWERPC_GOT_DTPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16_LO:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HI:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HA:
      {
	Output_data_got_powerpc<size, big_endian>* got
	  = target->got_section(symtab, layout);
	if (!gsym->final_value_is_known()
	    && (gsym->is_from_dynobj()
		|| gsym->is_undefined()
		|| gsym->is_preemptible()))
	  got->add_global_with_rel(gsym, GOT_TYPE_DTPREL,
				   target->rela_dyn_section(layout),
				   elfcpp::R_POWERPC_DTPREL);
	else
	  got->add_global_tls(gsym, GOT_TYPE_DTPREL);
      }
      break;

    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_GOT_TPREL16_LO:
    case elfcpp::R_POWERPC_GOT_TPREL16_HI:
    case elfcpp::R_POWERPC_GOT_TPREL16_HA:
      {
	const bool final = gsym->final_value_is_known();
	const tls::Tls_optimization tls_type = target->optimize_tls_ie(final);
	if (tls_type == tls::TLSOPT_NONE)
	  {
	    if (!gsym->has_got_offset(GOT_TYPE_TPREL))
	      {
		Output_data_got_powerpc<size, big_endian>* got
		  = target->got_section(symtab, layout);
		Reloc_section* rela_dyn = target->rela_dyn_section(layout);
		if (gsym->is_undefined()
		    || gsym->is_from_dynobj())
		  {
		    got->add_global_with_rel(gsym, GOT_TYPE_TPREL, rela_dyn,
					     elfcpp::R_POWERPC_TPREL);
		  }
		else
		  {
		    unsigned int off = got->add_constant(0);
		    gsym->set_got_offset(GOT_TYPE_TPREL, off);
		    unsigned int dynrel = elfcpp::R_POWERPC_TPREL;
		    rela_dyn->add_symbolless_global_addend(gsym, dynrel,
							   got, off, 0);
		  }
	      }
	  }
	else if (tls_type == tls::TLSOPT_TO_LE)
	  {
	    // no GOT relocs needed for Local Exec.
	  }
	else
	  gold_unreachable();
      }
      break;

    default:
      unsupported_reloc_global(object, r_type, gsym);
      break;
    }

  switch (r_type)
    {
    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16:
    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_DS:
      ppc_object->set_has_small_toc_reloc();
    default:
      break;
    }
}

// Process relocations for gc.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::gc_process_relocs(
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    unsigned int,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    size_t local_symbol_count,
    const unsigned char* plocal_symbols)
{
  typedef Target_powerpc<size, big_endian> Powerpc;
  typedef typename Target_powerpc<size, big_endian>::Scan Scan;
  Powerpc_relobj<size, big_endian>* ppc_object
    = static_cast<Powerpc_relobj<size, big_endian>*>(object);
  if (size == 64)
    ppc_object->set_opd_valid();
  if (size == 64 && data_shndx == ppc_object->opd_shndx())
    {
      typename Powerpc_relobj<size, big_endian>::Access_from::iterator p;
      for (p = ppc_object->access_from_map()->begin();
	   p != ppc_object->access_from_map()->end();
	   ++p)
	{
	  Address dst_off = p->first;
	  unsigned int dst_indx = ppc_object->get_opd_ent(dst_off);
	  typename Powerpc_relobj<size, big_endian>::Section_refs::iterator s;
	  for (s = p->second.begin(); s != p->second.end(); ++s)
	    {
	      Object* src_obj = s->first;
	      unsigned int src_indx = s->second;
	      symtab->gc()->add_reference(src_obj, src_indx,
					  ppc_object, dst_indx);
	    }
	  p->second.clear();
	}
      ppc_object->access_from_map()->clear();
      ppc_object->process_gc_mark(symtab);
      // Don't look at .opd relocs as .opd will reference everything.
      return;
    }

  gold::gc_process_relocs<size, big_endian, Powerpc, elfcpp::SHT_RELA, Scan,
			  typename Target_powerpc::Relocatable_size_for_reloc>(
    symtab,
    layout,
    this,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    local_symbol_count,
    plocal_symbols);
}

// Handle target specific gc actions when adding a gc reference from
// SRC_OBJ, SRC_SHNDX to a location specified by DST_OBJ, DST_SHNDX
// and DST_OFF.  For powerpc64, this adds a referenc to the code
// section of a function descriptor.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::do_gc_add_reference(
    Symbol_table* symtab,
    Object* src_obj,
    unsigned int src_shndx,
    Object* dst_obj,
    unsigned int dst_shndx,
    Address dst_off) const
{
  if (size != 64 || dst_obj->is_dynamic())
    return;

  Powerpc_relobj<size, big_endian>* ppc_object
    = static_cast<Powerpc_relobj<size, big_endian>*>(dst_obj);
  if (dst_shndx != 0 && dst_shndx == ppc_object->opd_shndx())
    {
      if (ppc_object->opd_valid())
	{
	  dst_shndx = ppc_object->get_opd_ent(dst_off);
	  symtab->gc()->add_reference(src_obj, src_shndx, dst_obj, dst_shndx);
	}
      else
	{
	  // If we haven't run scan_opd_relocs, we must delay
	  // processing this function descriptor reference.
	  ppc_object->add_reference(src_obj, src_shndx, dst_off);
	}
    }
}

// Add any special sections for this symbol to the gc work list.
// For powerpc64, this adds the code section of a function
// descriptor.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::do_gc_mark_symbol(
    Symbol_table* symtab,
    Symbol* sym) const
{
  if (size == 64)
    {
      Powerpc_relobj<size, big_endian>* ppc_object
	= static_cast<Powerpc_relobj<size, big_endian>*>(sym->object());
      bool is_ordinary;
      unsigned int shndx = sym->shndx(&is_ordinary);
      if (is_ordinary && shndx != 0 && shndx == ppc_object->opd_shndx())
	{
	  Sized_symbol<size>* gsym = symtab->get_sized_symbol<size>(sym);
	  Address dst_off = gsym->value();
	  if (ppc_object->opd_valid())
	    {
	      unsigned int dst_indx = ppc_object->get_opd_ent(dst_off);
	      symtab->gc()->worklist().push(Section_id(ppc_object, dst_indx));
	    }
	  else
	    ppc_object->add_gc_mark(dst_off);
	}
    }
}

// For a symbol location in .opd, set LOC to the location of the
// function entry.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::do_function_location(
    Symbol_location* loc) const
{
  if (size == 64 && loc->shndx != 0)
    {
      if (loc->object->is_dynamic())
	{
	  Powerpc_dynobj<size, big_endian>* ppc_object
	    = static_cast<Powerpc_dynobj<size, big_endian>*>(loc->object);
	  if (loc->shndx == ppc_object->opd_shndx())
	    {
	      Address dest_off;
	      Address off = loc->offset - ppc_object->opd_address();
	      loc->shndx = ppc_object->get_opd_ent(off, &dest_off);
	      loc->offset = dest_off;
	    }
	}
      else
	{
	  const Powerpc_relobj<size, big_endian>* ppc_object
	    = static_cast<const Powerpc_relobj<size, big_endian>*>(loc->object);
	  if (loc->shndx == ppc_object->opd_shndx())
	    {
	      Address dest_off;
	      loc->shndx = ppc_object->get_opd_ent(loc->offset, &dest_off);
	      loc->offset = dest_off;
	    }
	}
    }
}

// Scan relocations for a section.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::scan_relocs(
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    size_t local_symbol_count,
    const unsigned char* plocal_symbols)
{
  typedef Target_powerpc<size, big_endian> Powerpc;
  typedef typename Target_powerpc<size, big_endian>::Scan Scan;

  if (sh_type == elfcpp::SHT_REL)
    {
      gold_error(_("%s: unsupported REL reloc section"),
		 object->name().c_str());
      return;
    }

  gold::scan_relocs<size, big_endian, Powerpc, elfcpp::SHT_RELA, Scan>(
    symtab,
    layout,
    this,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    local_symbol_count,
    plocal_symbols);
}

// Functor class for processing the global symbol table.
// Removes symbols defined on discarded opd entries.

template<bool big_endian>
class Global_symbol_visitor_opd
{
 public:
  Global_symbol_visitor_opd()
  { }

  void
  operator()(Sized_symbol<64>* sym)
  {
    if (sym->has_symtab_index()
	|| sym->source() != Symbol::FROM_OBJECT
	|| !sym->in_real_elf())
      return;

    if (sym->object()->is_dynamic())
      return;

    Powerpc_relobj<64, big_endian>* symobj
      = static_cast<Powerpc_relobj<64, big_endian>*>(sym->object());
    if (symobj->opd_shndx() == 0)
      return;

    bool is_ordinary;
    unsigned int shndx = sym->shndx(&is_ordinary);
    if (shndx == symobj->opd_shndx()
	&& symobj->get_opd_discard(sym->value()))
      sym->set_symtab_index(-1U);
  }
};

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::define_save_restore_funcs(
    Layout* layout,
    Symbol_table* symtab)
{
  if (size == 64)
    {
      Output_data_save_res<64, big_endian>* savres
	= new Output_data_save_res<64, big_endian>(symtab);
      layout->add_output_section_data(".text", elfcpp::SHT_PROGBITS,
				      elfcpp::SHF_ALLOC | elfcpp::SHF_EXECINSTR,
				      savres, ORDER_TEXT, false);
    }
}

// Sort linker created .got section first (for the header), then input
// sections belonging to files using small model code.

template<bool big_endian>
class Sort_toc_sections
{
 public:
  bool
  operator()(const Output_section::Input_section& is1,
	     const Output_section::Input_section& is2) const
  {
    if (!is1.is_input_section() && is2.is_input_section())
      return true;
    bool small1
      = (is1.is_input_section()
	 && (static_cast<const Powerpc_relobj<64, big_endian>*>(is1.relobj())
	     ->has_small_toc_reloc()));
    bool small2
      = (is2.is_input_section()
	 && (static_cast<const Powerpc_relobj<64, big_endian>*>(is2.relobj())
	     ->has_small_toc_reloc()));
    return small1 && !small2;
  }
};

// Finalize the sections.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::do_finalize_sections(
    Layout* layout,
    const Input_objects*,
    Symbol_table* symtab)
{
  if (parameters->doing_static_link())
    {
      // At least some versions of glibc elf-init.o have a strong
      // reference to __rela_iplt marker syms.  A weak ref would be
      // better..
      if (this->iplt_ != NULL)
	{
	  Reloc_section* rel = this->iplt_->rel_plt();
	  symtab->define_in_output_data("__rela_iplt_start", NULL,
					Symbol_table::PREDEFINED, rel, 0, 0,
					elfcpp::STT_NOTYPE, elfcpp::STB_GLOBAL,
					elfcpp::STV_HIDDEN, 0, false, true);
	  symtab->define_in_output_data("__rela_iplt_end", NULL,
					Symbol_table::PREDEFINED, rel, 0, 0,
					elfcpp::STT_NOTYPE, elfcpp::STB_GLOBAL,
					elfcpp::STV_HIDDEN, 0, true, true);
	}
      else
	{
	  symtab->define_as_constant("__rela_iplt_start", NULL,
				     Symbol_table::PREDEFINED, 0, 0,
				     elfcpp::STT_NOTYPE, elfcpp::STB_GLOBAL,
				     elfcpp::STV_HIDDEN, 0, true, false);
	  symtab->define_as_constant("__rela_iplt_end", NULL,
				     Symbol_table::PREDEFINED, 0, 0,
				     elfcpp::STT_NOTYPE, elfcpp::STB_GLOBAL,
				     elfcpp::STV_HIDDEN, 0, true, false);
	}
    }

  if (size == 64)
    {
      typedef Global_symbol_visitor_opd<big_endian> Symbol_visitor;
      symtab->for_all_symbols<64, Symbol_visitor>(Symbol_visitor());

      if (!parameters->options().relocatable())
	{
	  this->define_save_restore_funcs(layout, symtab);

	  // Annoyingly, we need to make these sections now whether or
	  // not we need them.  If we delay until do_relax then we
	  // need to mess with the relaxation machinery checkpointing.
	  this->got_section(symtab, layout);
	  this->make_brlt_section(layout);

	  if (parameters->options().toc_sort())
	    {
	      Output_section* os = this->got_->output_section();
	      if (os != NULL && os->input_sections().size() > 1)
		std::stable_sort(os->input_sections().begin(),
				 os->input_sections().end(),
				 Sort_toc_sections<big_endian>());
	    }
	}
    }

  // Fill in some more dynamic tags.
  Output_data_dynamic* odyn = layout->dynamic_data();
  if (odyn != NULL)
    {
      const Reloc_section* rel_plt = (this->plt_ == NULL
				      ? NULL
				      : this->plt_->rel_plt());
      layout->add_target_dynamic_tags(false, this->plt_, rel_plt,
				      this->rela_dyn_, true, size == 32);

      if (size == 32)
	{
	  if (this->got_ != NULL)
	    {
	      this->got_->finalize_data_size();
	      odyn->add_section_plus_offset(elfcpp::DT_PPC_GOT,
					    this->got_, this->got_->g_o_t());
	    }
	}
      else
	{
	  if (this->glink_ != NULL)
	    {
	      this->glink_->finalize_data_size();
	      odyn->add_section_plus_offset(elfcpp::DT_PPC64_GLINK,
					    this->glink_,
					    (this->glink_->pltresolve_size
					     - 32));
	    }
	}
    }

  // Emit any relocs we saved in an attempt to avoid generating COPY
  // relocs.
  if (this->copy_relocs_.any_saved_relocs())
    this->copy_relocs_.emit(this->rela_dyn_section(layout));
}

// Return TRUE iff INSN is one we expect on a _LO variety toc/got
// reloc.

static bool
ok_lo_toc_insn(uint32_t insn)
{
  return ((insn & (0x3f << 26)) == 14u << 26 /* addi */
	  || (insn & (0x3f << 26)) == 32u << 26 /* lwz */
	  || (insn & (0x3f << 26)) == 34u << 26 /* lbz */
	  || (insn & (0x3f << 26)) == 36u << 26 /* stw */
	  || (insn & (0x3f << 26)) == 38u << 26 /* stb */
	  || (insn & (0x3f << 26)) == 40u << 26 /* lhz */
	  || (insn & (0x3f << 26)) == 42u << 26 /* lha */
	  || (insn & (0x3f << 26)) == 44u << 26 /* sth */
	  || (insn & (0x3f << 26)) == 46u << 26 /* lmw */
	  || (insn & (0x3f << 26)) == 47u << 26 /* stmw */
	  || (insn & (0x3f << 26)) == 48u << 26 /* lfs */
	  || (insn & (0x3f << 26)) == 50u << 26 /* lfd */
	  || (insn & (0x3f << 26)) == 52u << 26 /* stfs */
	  || (insn & (0x3f << 26)) == 54u << 26 /* stfd */
	  || ((insn & (0x3f << 26)) == 58u << 26 /* lwa,ld,lmd */
	      && (insn & 3) != 1)
	  || ((insn & (0x3f << 26)) == 62u << 26 /* std, stmd */
	      && ((insn & 3) == 0 || (insn & 3) == 3))
	  || (insn & (0x3f << 26)) == 12u << 26 /* addic */);
}

// Return the value to use for a branch relocation.

template<int size, bool big_endian>
typename Target_powerpc<size, big_endian>::Address
Target_powerpc<size, big_endian>::symval_for_branch(
    const Symbol_table* symtab,
    Address value,
    const Sized_symbol<size>* gsym,
    Powerpc_relobj<size, big_endian>* object,
    unsigned int *dest_shndx)
{
  *dest_shndx = 0;
  if (size == 32)
    return value;

  // If the symbol is defined in an opd section, ie. is a function
  // descriptor, use the function descriptor code entry address
  Powerpc_relobj<size, big_endian>* symobj = object;
  if (gsym != NULL
      && gsym->source() != Symbol::FROM_OBJECT)
    return value;
  if (gsym != NULL)
    symobj = static_cast<Powerpc_relobj<size, big_endian>*>(gsym->object());
  unsigned int shndx = symobj->opd_shndx();
  if (shndx == 0)
    return value;
  Address opd_addr = symobj->get_output_section_offset(shndx);
  if (opd_addr == invalid_address)
    return value;
  opd_addr += symobj->output_section_address(shndx);
  if (value >= opd_addr && value < opd_addr + symobj->section_size(shndx))
    {
      Address sec_off;
      *dest_shndx = symobj->get_opd_ent(value - opd_addr, &sec_off);
      if (symtab->is_section_folded(symobj, *dest_shndx))
	{
	  Section_id folded
	    = symtab->icf()->get_folded_section(symobj, *dest_shndx);
	  symobj = static_cast<Powerpc_relobj<size, big_endian>*>(folded.first);
	  *dest_shndx = folded.second;
	}
      Address sec_addr = symobj->get_output_section_offset(*dest_shndx);
      gold_assert(sec_addr != invalid_address);
      sec_addr += symobj->output_section(*dest_shndx)->address();
      value = sec_addr + sec_off;
    }
  return value;
}

// Perform a relocation.

template<int size, bool big_endian>
inline bool
Target_powerpc<size, big_endian>::Relocate::relocate(
    const Relocate_info<size, big_endian>* relinfo,
    Target_powerpc* target,
    Output_section* os,
    size_t relnum,
    const elfcpp::Rela<size, big_endian>& rela,
    unsigned int r_type,
    const Sized_symbol<size>* gsym,
    const Symbol_value<size>* psymval,
    unsigned char* view,
    Address address,
    section_size_type view_size)
{
  if (view == NULL)
    return true;

  switch (this->maybe_skip_tls_get_addr_call(r_type, gsym))
    {
    case Track_tls::NOT_EXPECTED:
      gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			     _("__tls_get_addr call lacks marker reloc"));
      break;
    case Track_tls::EXPECTED:
      // We have already complained.
      break;
    case Track_tls::SKIP:
      return true;
    case Track_tls::NORMAL:
      break;
    }

  typedef Powerpc_relocate_functions<size, big_endian> Reloc;
  typedef typename elfcpp::Swap<32, big_endian>::Valtype Insn;
  Powerpc_relobj<size, big_endian>* const object
    = static_cast<Powerpc_relobj<size, big_endian>*>(relinfo->object);
  Address value = 0;
  bool has_plt_value = false;
  unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
  if ((gsym != NULL
       ? use_plt_offset<size>(gsym, Scan::get_reference_flags(r_type))
       : object->local_has_plt_offset(r_sym))
      && (!psymval->is_ifunc_symbol()
	  || Scan::reloc_needs_plt_for_ifunc(object, r_type, false)))
    {
      Stub_table<size, big_endian>* stub_table
	= object->stub_table(relinfo->data_shndx);
      if (stub_table == NULL)
	{
	  // This is a ref from a data section to an ifunc symbol.
	  if (target->stub_tables().size() != 0)
	    stub_table = target->stub_tables()[0];
	}
      gold_assert(stub_table != NULL);
      Address off;
      if (gsym != NULL)
	off = stub_table->find_plt_call_entry(object, gsym, r_type,
					      rela.get_r_addend());
      else
	off = stub_table->find_plt_call_entry(object, r_sym, r_type,
					      rela.get_r_addend());
      gold_assert(off != invalid_address);
      value = stub_table->stub_address() + off;
      has_plt_value = true;
    }

  if (r_type == elfcpp::R_POWERPC_GOT16
      || r_type == elfcpp::R_POWERPC_GOT16_LO
      || r_type == elfcpp::R_POWERPC_GOT16_HI
      || r_type == elfcpp::R_POWERPC_GOT16_HA
      || r_type == elfcpp::R_PPC64_GOT16_DS
      || r_type == elfcpp::R_PPC64_GOT16_LO_DS)
    {
      if (gsym != NULL)
	{
	  gold_assert(gsym->has_got_offset(GOT_TYPE_STANDARD));
	  value = gsym->got_offset(GOT_TYPE_STANDARD);
	}
      else
	{
	  unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
	  gold_assert(object->local_has_got_offset(r_sym, GOT_TYPE_STANDARD));
	  value = object->local_got_offset(r_sym, GOT_TYPE_STANDARD);
	}
      value -= target->got_section()->got_base_offset(object);
    }
  else if (r_type == elfcpp::R_PPC64_TOC)
    {
      value = (target->got_section()->output_section()->address()
	       + object->toc_base_offset());
    }
  else if (gsym != NULL
	   && (r_type == elfcpp::R_POWERPC_REL24
	       || r_type == elfcpp::R_PPC_PLTREL24)
	   && has_plt_value)
    {
      if (size == 64)
	{
	  typedef typename elfcpp::Swap<32, big_endian>::Valtype Valtype;
	  Valtype* wv = reinterpret_cast<Valtype*>(view);
	  bool can_plt_call = false;
	  if (rela.get_r_offset() + 8 <= view_size)
	    {
	      Valtype insn = elfcpp::Swap<32, big_endian>::readval(wv);
	      Valtype insn2 = elfcpp::Swap<32, big_endian>::readval(wv + 1);
	      if ((insn & 1) != 0
		  && (insn2 == nop
		      || insn2 == cror_15_15_15 || insn2 == cror_31_31_31))
		{
		  elfcpp::Swap<32, big_endian>::writeval(wv + 1, ld_2_1 + 40);
		  can_plt_call = true;
		}
	    }
	  if (!can_plt_call)
	    {
	      // If we don't have a branch and link followed by a nop,
	      // we can't go via the plt because there is no place to
	      // put a toc restoring instruction.
	      // Unless we know we won't be returning.
	      if (strcmp(gsym->name(), "__libc_start_main") == 0)
		can_plt_call = true;
	    }
	  if (!can_plt_call)
	    {
	      // g++ as of 20130507 emits self-calls without a
	      // following nop.  This is arguably wrong since we have
	      // conflicting information.  On the one hand a global
	      // symbol and on the other a local call sequence, but
	      // don't error for this special case.
	      // It isn't possible to cheaply verify we have exactly
	      // such a call.  Allow all calls to the same section.
	      bool ok = false;
	      Address code = value;
	      if (gsym->source() == Symbol::FROM_OBJECT
		  && gsym->object() == object)
		{
		  Address addend = rela.get_r_addend();
		  unsigned int dest_shndx;
		  Address opdent = psymval->value(object, addend);
		  code = target->symval_for_branch(relinfo->symtab, opdent,
						   gsym, object, &dest_shndx);
		  bool is_ordinary;
		  if (dest_shndx == 0)
		    dest_shndx = gsym->shndx(&is_ordinary);
		  ok = dest_shndx == relinfo->data_shndx;
		}
	      if (!ok)
		{
		  gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
					 _("call lacks nop, can't restore toc; "
					   "recompile with -fPIC"));
		  value = code;
		}
	    }
	}
    }
  else if (r_type == elfcpp::R_POWERPC_GOT_TLSGD16
	   || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_LO
	   || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_HI
	   || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_HA)
    {
      // First instruction of a global dynamic sequence, arg setup insn.
      const bool final = gsym == NULL || gsym->final_value_is_known();
      const tls::Tls_optimization tls_type = target->optimize_tls_gd(final);
      enum Got_type got_type = GOT_TYPE_STANDARD;
      if (tls_type == tls::TLSOPT_NONE)
	got_type = GOT_TYPE_TLSGD;
      else if (tls_type == tls::TLSOPT_TO_IE)
	got_type = GOT_TYPE_TPREL;
      if (got_type != GOT_TYPE_STANDARD)
	{
	  if (gsym != NULL)
	    {
	      gold_assert(gsym->has_got_offset(got_type));
	      value = gsym->got_offset(got_type);
	    }
	  else
	    {
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
	      gold_assert(object->local_has_got_offset(r_sym, got_type));
	      value = object->local_got_offset(r_sym, got_type);
	    }
	  value -= target->got_section()->got_base_offset(object);
	}
      if (tls_type == tls::TLSOPT_TO_IE)
	{
	  if (r_type == elfcpp::R_POWERPC_GOT_TLSGD16
	      || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_LO)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	      insn &= (1 << 26) - (1 << 16); // extract rt,ra from addi
	      if (size == 32)
		insn |= 32 << 26; // lwz
	      else
		insn |= 58 << 26; // ld
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	    }
	  r_type += (elfcpp::R_POWERPC_GOT_TPREL16
		     - elfcpp::R_POWERPC_GOT_TLSGD16);
	}
      else if (tls_type == tls::TLSOPT_TO_LE)
	{
	  if (r_type == elfcpp::R_POWERPC_GOT_TLSGD16
	      || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_LO)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = addis_3_13;
	      if (size == 32)
		insn = addis_3_2;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_TPREL16_HA;
	      value = psymval->value(object, rela.get_r_addend());
	    }
	  else
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = nop;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_NONE;
	    }
	}
    }
  else if (r_type == elfcpp::R_POWERPC_GOT_TLSLD16
	   || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_LO
	   || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_HI
	   || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_HA)
    {
      // First instruction of a local dynamic sequence, arg setup insn.
      const tls::Tls_optimization tls_type = target->optimize_tls_ld();
      if (tls_type == tls::TLSOPT_NONE)
	{
	  value = target->tlsld_got_offset();
	  value -= target->got_section()->got_base_offset(object);
	}
      else
	{
	  gold_assert(tls_type == tls::TLSOPT_TO_LE);
	  if (r_type == elfcpp::R_POWERPC_GOT_TLSLD16
	      || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_LO)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = addis_3_13;
	      if (size == 32)
		insn = addis_3_2;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_TPREL16_HA;
	      value = dtp_offset;
	    }
	  else
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = nop;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_NONE;
	    }
	}
    }
  else if (r_type == elfcpp::R_POWERPC_GOT_DTPREL16
	   || r_type == elfcpp::R_POWERPC_GOT_DTPREL16_LO
	   || r_type == elfcpp::R_POWERPC_GOT_DTPREL16_HI
	   || r_type == elfcpp::R_POWERPC_GOT_DTPREL16_HA)
    {
      // Accesses relative to a local dynamic sequence address,
      // no optimisation here.
      if (gsym != NULL)
	{
	  gold_assert(gsym->has_got_offset(GOT_TYPE_DTPREL));
	  value = gsym->got_offset(GOT_TYPE_DTPREL);
	}
      else
	{
	  unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
	  gold_assert(object->local_has_got_offset(r_sym, GOT_TYPE_DTPREL));
	  value = object->local_got_offset(r_sym, GOT_TYPE_DTPREL);
	}
      value -= target->got_section()->got_base_offset(object);
    }
  else if (r_type == elfcpp::R_POWERPC_GOT_TPREL16
	   || r_type == elfcpp::R_POWERPC_GOT_TPREL16_LO
	   || r_type == elfcpp::R_POWERPC_GOT_TPREL16_HI
	   || r_type == elfcpp::R_POWERPC_GOT_TPREL16_HA)
    {
      // First instruction of initial exec sequence.
      const bool final = gsym == NULL || gsym->final_value_is_known();
      const tls::Tls_optimization tls_type = target->optimize_tls_ie(final);
      if (tls_type == tls::TLSOPT_NONE)
	{
	  if (gsym != NULL)
	    {
	      gold_assert(gsym->has_got_offset(GOT_TYPE_TPREL));
	      value = gsym->got_offset(GOT_TYPE_TPREL);
	    }
	  else
	    {
	      unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
	      gold_assert(object->local_has_got_offset(r_sym, GOT_TYPE_TPREL));
	      value = object->local_got_offset(r_sym, GOT_TYPE_TPREL);
	    }
	  value -= target->got_section()->got_base_offset(object);
	}
      else
	{
	  gold_assert(tls_type == tls::TLSOPT_TO_LE);
	  if (r_type == elfcpp::R_POWERPC_GOT_TPREL16
	      || r_type == elfcpp::R_POWERPC_GOT_TPREL16_LO)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	      insn &= (1 << 26) - (1 << 21); // extract rt from ld
	      if (size == 32)
		insn |= addis_0_2;
	      else
		insn |= addis_0_13;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_TPREL16_HA;
	      value = psymval->value(object, rela.get_r_addend());
	    }
	  else
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = nop;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_NONE;
	    }
	}
    }
  else if ((size == 64 && r_type == elfcpp::R_PPC64_TLSGD)
	   || (size == 32 && r_type == elfcpp::R_PPC_TLSGD))
    {
      // Second instruction of a global dynamic sequence,
      // the __tls_get_addr call
      this->expect_tls_get_addr_call(relinfo, relnum, rela.get_r_offset());
      const bool final = gsym == NULL || gsym->final_value_is_known();
      const tls::Tls_optimization tls_type = target->optimize_tls_gd(final);
      if (tls_type != tls::TLSOPT_NONE)
	{
	  if (tls_type == tls::TLSOPT_TO_IE)
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view);
	      Insn insn = add_3_3_13;
	      if (size == 32)
		insn = add_3_3_2;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_NONE;
	    }
	  else
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view);
	      Insn insn = addi_3_3;
	      elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	      r_type = elfcpp::R_POWERPC_TPREL16_LO;
	      view += 2 * big_endian;
	      value = psymval->value(object, rela.get_r_addend());
	    }
	  this->skip_next_tls_get_addr_call();
	}
    }
  else if ((size == 64 && r_type == elfcpp::R_PPC64_TLSLD)
	   || (size == 32 && r_type == elfcpp::R_PPC_TLSLD))
    {
      // Second instruction of a local dynamic sequence,
      // the __tls_get_addr call
      this->expect_tls_get_addr_call(relinfo, relnum, rela.get_r_offset());
      const tls::Tls_optimization tls_type = target->optimize_tls_ld();
      if (tls_type == tls::TLSOPT_TO_LE)
	{
	  Insn* iview = reinterpret_cast<Insn*>(view);
	  Insn insn = addi_3_3;
	  elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	  this->skip_next_tls_get_addr_call();
	  r_type = elfcpp::R_POWERPC_TPREL16_LO;
	  view += 2 * big_endian;
	  value = dtp_offset;
	}
    }
  else if (r_type == elfcpp::R_POWERPC_TLS)
    {
      // Second instruction of an initial exec sequence
      const bool final = gsym == NULL || gsym->final_value_is_known();
      const tls::Tls_optimization tls_type = target->optimize_tls_ie(final);
      if (tls_type == tls::TLSOPT_TO_LE)
	{
	  Insn* iview = reinterpret_cast<Insn*>(view);
	  Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	  unsigned int reg = size == 32 ? 2 : 13;
	  insn = at_tls_transform(insn, reg);
	  gold_assert(insn != 0);
	  elfcpp::Swap<32, big_endian>::writeval(iview, insn);
	  r_type = elfcpp::R_POWERPC_TPREL16_LO;
	  view += 2 * big_endian;
	  value = psymval->value(object, rela.get_r_addend());
	}
    }
  else if (!has_plt_value)
    {
      Address addend = 0;
      unsigned int dest_shndx;
      if (r_type != elfcpp::R_PPC_PLTREL24)
	addend = rela.get_r_addend();
      value = psymval->value(object, addend);
      if (size == 64 && is_branch_reloc(r_type))
	value = target->symval_for_branch(relinfo->symtab, value,
					  gsym, object, &dest_shndx);
      unsigned int max_branch_offset = 0;
      if (r_type == elfcpp::R_POWERPC_REL24
	  || r_type == elfcpp::R_PPC_PLTREL24
	  || r_type == elfcpp::R_PPC_LOCAL24PC)
	max_branch_offset = 1 << 25;
      else if (r_type == elfcpp::R_POWERPC_REL14
	       || r_type == elfcpp::R_POWERPC_REL14_BRTAKEN
	       || r_type == elfcpp::R_POWERPC_REL14_BRNTAKEN)
	max_branch_offset = 1 << 15;
      if (max_branch_offset != 0
	  && value - address + max_branch_offset >= 2 * max_branch_offset)
	{
	  Stub_table<size, big_endian>* stub_table
	    = object->stub_table(relinfo->data_shndx);
	  if (stub_table != NULL)
	    {
	      Address off = stub_table->find_long_branch_entry(object, value);
	      if (off != invalid_address)
		value = (stub_table->stub_address() + stub_table->plt_size()
			 + off);
	    }
	}
    }

  switch (r_type)
    {
    case elfcpp::R_PPC64_REL64:
    case elfcpp::R_POWERPC_REL32:
    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_POWERPC_REL16_HA:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      value -= address;
      break;

    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
      // Subtract the TOC base address.
      value -= (target->got_section()->output_section()->address()
		+ object->toc_base_offset());
      break;

    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_SECTOFF_LO:
    case elfcpp::R_POWERPC_SECTOFF_HI:
    case elfcpp::R_POWERPC_SECTOFF_HA:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_PPC64_SECTOFF_LO_DS:
      if (os != NULL)
	value -= os->address();
      break;

    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_TPREL16_LO_DS:
      if (size != 64)
	// R_PPC_TLSGD and R_PPC_TLSLD
	break;
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_TPREL16_LO:
    case elfcpp::R_POWERPC_TPREL16_HI:
    case elfcpp::R_POWERPC_TPREL16_HA:
    case elfcpp::R_POWERPC_TPREL:
    case elfcpp::R_PPC64_TPREL16_HIGHER:
    case elfcpp::R_PPC64_TPREL16_HIGHERA:
    case elfcpp::R_PPC64_TPREL16_HIGHEST:
    case elfcpp::R_PPC64_TPREL16_HIGHESTA:
      // tls symbol values are relative to tls_segment()->vaddr()
      value -= tp_offset;
      break;

    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_LO_DS:
    case elfcpp::R_PPC64_DTPREL16_HIGHER:
    case elfcpp::R_PPC64_DTPREL16_HIGHERA:
    case elfcpp::R_PPC64_DTPREL16_HIGHEST:
    case elfcpp::R_PPC64_DTPREL16_HIGHESTA:
      if (size != 64)
	// R_PPC_EMB_NADDR32, R_PPC_EMB_NADDR16, R_PPC_EMB_NADDR16_LO
	// R_PPC_EMB_NADDR16_HI, R_PPC_EMB_NADDR16_HA, R_PPC_EMB_SDAI16
	break;
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_POWERPC_DTPREL16_LO:
    case elfcpp::R_POWERPC_DTPREL16_HI:
    case elfcpp::R_POWERPC_DTPREL16_HA:
    case elfcpp::R_POWERPC_DTPREL:
      // tls symbol values are relative to tls_segment()->vaddr()
      value -= dtp_offset;
      break;

    default:
      break;
    }

  Insn branch_bit = 0;
  switch (r_type)
    {
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
      branch_bit = 1 << 21;
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      {
	Insn* iview = reinterpret_cast<Insn*>(view);
	Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	insn &= ~(1 << 21);
	insn |= branch_bit;
	if (this->is_isa_v2)
	  {
	    // Set 'a' bit.  This is 0b00010 in BO field for branch
	    // on CR(BI) insns (BO == 001at or 011at), and 0b01000
	    // for branch on CTR insns (BO == 1a00t or 1a01t).
	    if ((insn & (0x14 << 21)) == (0x04 << 21))
	      insn |= 0x02 << 21;
	    else if ((insn & (0x14 << 21)) == (0x10 << 21))
	      insn |= 0x08 << 21;
	    else
	      break;
	  }
	else
	  {
	    // Invert 'y' bit if not the default.
	    if (static_cast<Signed_address>(value) < 0)
	      insn ^= 1 << 21;
	  }
	elfcpp::Swap<32, big_endian>::writeval(iview, insn);
      }
      break;

    default:
      break;
    }

  if (size == 64)
    {
      // Multi-instruction sequences that access the TOC can be
      // optimized, eg. addis ra,r2,0; addi rb,ra,x;
      // to             nop;           addi rb,r2,x;
      switch (r_type)
	{
	default:
	  break;

	case elfcpp::R_POWERPC_GOT_TLSLD16_HA:
	case elfcpp::R_POWERPC_GOT_TLSGD16_HA:
	case elfcpp::R_POWERPC_GOT_TPREL16_HA:
	case elfcpp::R_POWERPC_GOT_DTPREL16_HA:
	case elfcpp::R_POWERPC_GOT16_HA:
	case elfcpp::R_PPC64_TOC16_HA:
	  if (parameters->options().toc_optimize())
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	      if ((insn & ((0x3f << 26) | 0x1f << 16))
		  != ((15u << 26) | (2 << 16)) /* addis rt,2,imm */)
		gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
				       _("toc optimization is not supported "
					 "for %#08x instruction"), insn);
	      else if (value + 0x8000 < 0x10000)
		{
		  elfcpp::Swap<32, big_endian>::writeval(iview, nop);
		  return true;
		}
	    }
	  break;

	case elfcpp::R_POWERPC_GOT_TLSLD16_LO:
	case elfcpp::R_POWERPC_GOT_TLSGD16_LO:
	case elfcpp::R_POWERPC_GOT_TPREL16_LO:
	case elfcpp::R_POWERPC_GOT_DTPREL16_LO:
	case elfcpp::R_POWERPC_GOT16_LO:
	case elfcpp::R_PPC64_GOT16_LO_DS:
	case elfcpp::R_PPC64_TOC16_LO:
	case elfcpp::R_PPC64_TOC16_LO_DS:
	  if (parameters->options().toc_optimize())
	    {
	      Insn* iview = reinterpret_cast<Insn*>(view - 2 * big_endian);
	      Insn insn = elfcpp::Swap<32, big_endian>::readval(iview);
	      if (!ok_lo_toc_insn(insn))
		gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
				       _("toc optimization is not supported "
					 "for %#08x instruction"), insn);
	      else if (value + 0x8000 < 0x10000)
		{
		  if ((insn & (0x3f << 26)) == 12u << 26 /* addic */)
		    {
		      // Transform addic to addi when we change reg.
		      insn &= ~((0x3f << 26) | (0x1f << 16));
		      insn |= (14u << 26) | (2 << 16);
		    }
		  else
		    {
		      insn &= ~(0x1f << 16);
		      insn |= 2 << 16;
		    }
		  elfcpp::Swap<32, big_endian>::writeval(iview, insn);
		}
	    }
	  break;
	}
    }

  typename Reloc::Overflow_check overflow = Reloc::CHECK_NONE;
  switch (r_type)
    {
    case elfcpp::R_POWERPC_ADDR32:
    case elfcpp::R_POWERPC_UADDR32:
      if (size == 64)
	overflow = Reloc::CHECK_BITFIELD;
      break;

    case elfcpp::R_POWERPC_REL32:
      if (size == 64)
	overflow = Reloc::CHECK_SIGNED;
      break;

    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_UADDR16:
    case elfcpp::R_PPC64_ADDR16_DS:
    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
      overflow = Reloc::CHECK_BITFIELD;
      break;

    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_PPC_LOCAL24PC:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16:
      overflow = Reloc::CHECK_SIGNED;
      break;
    }

  typename Powerpc_relocate_functions<size, big_endian>::Status status
    = Powerpc_relocate_functions<size, big_endian>::STATUS_OK;
  switch (r_type)
    {
    case elfcpp::R_POWERPC_NONE:
    case elfcpp::R_POWERPC_TLS:
    case elfcpp::R_POWERPC_GNU_VTINHERIT:
    case elfcpp::R_POWERPC_GNU_VTENTRY:
    case elfcpp::R_PPC_EMB_MRKREF:
      break;

    case elfcpp::R_PPC64_ADDR64:
    case elfcpp::R_PPC64_REL64:
    case elfcpp::R_PPC64_TOC:
      Reloc::addr64(view, value);
      break;

    case elfcpp::R_POWERPC_TPREL:
    case elfcpp::R_POWERPC_DTPREL:
      if (size == 64)
	Reloc::addr64(view, value);
      else
	status = Reloc::addr32(view, value, overflow);
      break;

    case elfcpp::R_PPC64_UADDR64:
      Reloc::addr64_u(view, value);
      break;

    case elfcpp::R_POWERPC_ADDR32:
      status = Reloc::addr32(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_REL32:
    case elfcpp::R_POWERPC_UADDR32:
      status = Reloc::addr32_u(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_ADDR24:
    case elfcpp::R_POWERPC_REL24:
    case elfcpp::R_PPC_PLTREL24:
    case elfcpp::R_PPC_LOCAL24PC:
      status = Reloc::addr24(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_GOT_DTPREL16:
    case elfcpp::R_POWERPC_GOT_DTPREL16_LO:
      if (size == 64)
	{
	  status = Reloc::addr16_ds(view, value, overflow);
	  break;
	}
    case elfcpp::R_POWERPC_ADDR16:
    case elfcpp::R_POWERPC_REL16:
    case elfcpp::R_PPC64_TOC16:
    case elfcpp::R_POWERPC_GOT16:
    case elfcpp::R_POWERPC_SECTOFF:
    case elfcpp::R_POWERPC_TPREL16:
    case elfcpp::R_POWERPC_DTPREL16:
    case elfcpp::R_POWERPC_GOT_TLSGD16:
    case elfcpp::R_POWERPC_GOT_TLSLD16:
    case elfcpp::R_POWERPC_GOT_TPREL16:
    case elfcpp::R_POWERPC_ADDR16_LO:
    case elfcpp::R_POWERPC_REL16_LO:
    case elfcpp::R_PPC64_TOC16_LO:
    case elfcpp::R_POWERPC_GOT16_LO:
    case elfcpp::R_POWERPC_SECTOFF_LO:
    case elfcpp::R_POWERPC_TPREL16_LO:
    case elfcpp::R_POWERPC_DTPREL16_LO:
    case elfcpp::R_POWERPC_GOT_TLSGD16_LO:
    case elfcpp::R_POWERPC_GOT_TLSLD16_LO:
    case elfcpp::R_POWERPC_GOT_TPREL16_LO:
      status = Reloc::addr16(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_UADDR16:
      status = Reloc::addr16_u(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_ADDR16_HI:
    case elfcpp::R_POWERPC_REL16_HI:
    case elfcpp::R_PPC64_TOC16_HI:
    case elfcpp::R_POWERPC_GOT16_HI:
    case elfcpp::R_POWERPC_SECTOFF_HI:
    case elfcpp::R_POWERPC_TPREL16_HI:
    case elfcpp::R_POWERPC_DTPREL16_HI:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HI:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HI:
    case elfcpp::R_POWERPC_GOT_TPREL16_HI:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HI:
      Reloc::addr16_hi(view, value);
      break;

    case elfcpp::R_POWERPC_ADDR16_HA:
    case elfcpp::R_POWERPC_REL16_HA:
    case elfcpp::R_PPC64_TOC16_HA:
    case elfcpp::R_POWERPC_GOT16_HA:
    case elfcpp::R_POWERPC_SECTOFF_HA:
    case elfcpp::R_POWERPC_TPREL16_HA:
    case elfcpp::R_POWERPC_DTPREL16_HA:
    case elfcpp::R_POWERPC_GOT_TLSGD16_HA:
    case elfcpp::R_POWERPC_GOT_TLSLD16_HA:
    case elfcpp::R_POWERPC_GOT_TPREL16_HA:
    case elfcpp::R_POWERPC_GOT_DTPREL16_HA:
      Reloc::addr16_ha(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_HIGHER:
      if (size == 32)
	// R_PPC_EMB_NADDR16_LO
	goto unsupp;
    case elfcpp::R_PPC64_ADDR16_HIGHER:
    case elfcpp::R_PPC64_TPREL16_HIGHER:
      Reloc::addr16_hi2(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_HIGHERA:
      if (size == 32)
	// R_PPC_EMB_NADDR16_HI
	goto unsupp;
    case elfcpp::R_PPC64_ADDR16_HIGHERA:
    case elfcpp::R_PPC64_TPREL16_HIGHERA:
      Reloc::addr16_ha2(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_HIGHEST:
      if (size == 32)
	// R_PPC_EMB_NADDR16_HA
	goto unsupp;
    case elfcpp::R_PPC64_ADDR16_HIGHEST:
    case elfcpp::R_PPC64_TPREL16_HIGHEST:
      Reloc::addr16_hi3(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_HIGHESTA:
      if (size == 32)
	// R_PPC_EMB_SDAI16
	goto unsupp;
    case elfcpp::R_PPC64_ADDR16_HIGHESTA:
    case elfcpp::R_PPC64_TPREL16_HIGHESTA:
      Reloc::addr16_ha3(view, value);
      break;

    case elfcpp::R_PPC64_DTPREL16_DS:
    case elfcpp::R_PPC64_DTPREL16_LO_DS:
      if (size == 32)
	// R_PPC_EMB_NADDR32, R_PPC_EMB_NADDR16
	goto unsupp;
    case elfcpp::R_PPC64_TPREL16_DS:
    case elfcpp::R_PPC64_TPREL16_LO_DS:
      if (size == 32)
	// R_PPC_TLSGD, R_PPC_TLSLD
	break;
    case elfcpp::R_PPC64_ADDR16_DS:
    case elfcpp::R_PPC64_ADDR16_LO_DS:
    case elfcpp::R_PPC64_TOC16_DS:
    case elfcpp::R_PPC64_TOC16_LO_DS:
    case elfcpp::R_PPC64_GOT16_DS:
    case elfcpp::R_PPC64_GOT16_LO_DS:
    case elfcpp::R_PPC64_SECTOFF_DS:
    case elfcpp::R_PPC64_SECTOFF_LO_DS:
      status = Reloc::addr16_ds(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_ADDR14:
    case elfcpp::R_POWERPC_ADDR14_BRTAKEN:
    case elfcpp::R_POWERPC_ADDR14_BRNTAKEN:
    case elfcpp::R_POWERPC_REL14:
    case elfcpp::R_POWERPC_REL14_BRTAKEN:
    case elfcpp::R_POWERPC_REL14_BRNTAKEN:
      status = Reloc::addr14(view, value, overflow);
      break;

    case elfcpp::R_POWERPC_COPY:
    case elfcpp::R_POWERPC_GLOB_DAT:
    case elfcpp::R_POWERPC_JMP_SLOT:
    case elfcpp::R_POWERPC_RELATIVE:
    case elfcpp::R_POWERPC_DTPMOD:
    case elfcpp::R_PPC64_JMP_IREL:
    case elfcpp::R_POWERPC_IRELATIVE:
      gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			     _("unexpected reloc %u in object file"),
			     r_type);
      break;

    case elfcpp::R_PPC_EMB_SDA21:
      if (size == 32)
	goto unsupp;
      else
	{
	  // R_PPC64_TOCSAVE.  For the time being this can be ignored.
	}
      break;

    case elfcpp::R_PPC_EMB_SDA2I16:
    case elfcpp::R_PPC_EMB_SDA2REL:
      if (size == 32)
	goto unsupp;
      // R_PPC64_TLSGD, R_PPC64_TLSLD
      break;

    case elfcpp::R_POWERPC_PLT32:
    case elfcpp::R_POWERPC_PLTREL32:
    case elfcpp::R_POWERPC_PLT16_LO:
    case elfcpp::R_POWERPC_PLT16_HI:
    case elfcpp::R_POWERPC_PLT16_HA:
    case elfcpp::R_PPC_SDAREL16:
    case elfcpp::R_POWERPC_ADDR30:
    case elfcpp::R_PPC64_PLT64:
    case elfcpp::R_PPC64_PLTREL64:
    case elfcpp::R_PPC64_PLTGOT16:
    case elfcpp::R_PPC64_PLTGOT16_LO:
    case elfcpp::R_PPC64_PLTGOT16_HI:
    case elfcpp::R_PPC64_PLTGOT16_HA:
    case elfcpp::R_PPC64_PLT16_LO_DS:
    case elfcpp::R_PPC64_PLTGOT16_DS:
    case elfcpp::R_PPC64_PLTGOT16_LO_DS:
    case elfcpp::R_PPC_EMB_RELSEC16:
    case elfcpp::R_PPC_EMB_RELST_LO:
    case elfcpp::R_PPC_EMB_RELST_HI:
    case elfcpp::R_PPC_EMB_RELST_HA:
    case elfcpp::R_PPC_EMB_BIT_FLD:
    case elfcpp::R_PPC_EMB_RELSDA:
    case elfcpp::R_PPC_TOC16:
    default:
    unsupp:
      gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			     _("unsupported reloc %u"),
			     r_type);
      break;
    }
  if (status != Powerpc_relocate_functions<size, big_endian>::STATUS_OK)
    gold_error_at_location(relinfo, relnum, rela.get_r_offset(),
			   _("relocation overflow"));

  return true;
}

// Relocate section data.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::relocate_section(
    const Relocate_info<size, big_endian>* relinfo,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    unsigned char* view,
    Address address,
    section_size_type view_size,
    const Reloc_symbol_changes* reloc_symbol_changes)
{
  typedef Target_powerpc<size, big_endian> Powerpc;
  typedef typename Target_powerpc<size, big_endian>::Relocate Powerpc_relocate;
  typedef typename Target_powerpc<size, big_endian>::Relocate_comdat_behavior
    Powerpc_comdat_behavior;

  gold_assert(sh_type == elfcpp::SHT_RELA);

  gold::relocate_section<size, big_endian, Powerpc, elfcpp::SHT_RELA,
			 Powerpc_relocate, Powerpc_comdat_behavior>(
    relinfo,
    this,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    view,
    address,
    view_size,
    reloc_symbol_changes);
}

class Powerpc_scan_relocatable_reloc
{
public:
  // Return the strategy to use for a local symbol which is not a
  // section symbol, given the relocation type.
  inline Relocatable_relocs::Reloc_strategy
  local_non_section_strategy(unsigned int r_type, Relobj*, unsigned int r_sym)
  {
    if (r_type == 0 && r_sym == 0)
      return Relocatable_relocs::RELOC_DISCARD;
    return Relocatable_relocs::RELOC_COPY;
  }

  // Return the strategy to use for a local symbol which is a section
  // symbol, given the relocation type.
  inline Relocatable_relocs::Reloc_strategy
  local_section_strategy(unsigned int, Relobj*)
  {
    return Relocatable_relocs::RELOC_ADJUST_FOR_SECTION_RELA;
  }

  // Return the strategy to use for a global symbol, given the
  // relocation type, the object, and the symbol index.
  inline Relocatable_relocs::Reloc_strategy
  global_strategy(unsigned int r_type, Relobj*, unsigned int)
  {
    if (r_type == elfcpp::R_PPC_PLTREL24)
      return Relocatable_relocs::RELOC_SPECIAL;
    return Relocatable_relocs::RELOC_COPY;
  }
};

// Scan the relocs during a relocatable link.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::scan_relocatable_relocs(
    Symbol_table* symtab,
    Layout* layout,
    Sized_relobj_file<size, big_endian>* object,
    unsigned int data_shndx,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    bool needs_special_offset_handling,
    size_t local_symbol_count,
    const unsigned char* plocal_symbols,
    Relocatable_relocs* rr)
{
  gold_assert(sh_type == elfcpp::SHT_RELA);

  gold::scan_relocatable_relocs<size, big_endian, elfcpp::SHT_RELA,
				Powerpc_scan_relocatable_reloc>(
    symtab,
    layout,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    output_section,
    needs_special_offset_handling,
    local_symbol_count,
    plocal_symbols,
    rr);
}

// Emit relocations for a section.
// This is a modified version of the function by the same name in
// target-reloc.h.  Using relocate_special_relocatable for
// R_PPC_PLTREL24 would require duplication of the entire body of the
// loop, so we may as well duplicate the whole thing.

template<int size, bool big_endian>
void
Target_powerpc<size, big_endian>::relocate_relocs(
    const Relocate_info<size, big_endian>* relinfo,
    unsigned int sh_type,
    const unsigned char* prelocs,
    size_t reloc_count,
    Output_section* output_section,
    typename elfcpp::Elf_types<size>::Elf_Off offset_in_output_section,
    const Relocatable_relocs* rr,
    unsigned char*,
    Address view_address,
    section_size_type,
    unsigned char* reloc_view,
    section_size_type reloc_view_size)
{
  gold_assert(sh_type == elfcpp::SHT_RELA);

  typedef typename Reloc_types<elfcpp::SHT_RELA, size, big_endian>::Reloc
    Reltype;
  typedef typename Reloc_types<elfcpp::SHT_RELA, size, big_endian>::Reloc_write
    Reltype_write;
  const int reloc_size
    = Reloc_types<elfcpp::SHT_RELA, size, big_endian>::reloc_size;

  Powerpc_relobj<size, big_endian>* const object
    = static_cast<Powerpc_relobj<size, big_endian>*>(relinfo->object);
  const unsigned int local_count = object->local_symbol_count();
  unsigned int got2_shndx = object->got2_shndx();
  Address got2_addend = 0;
  if (got2_shndx != 0)
    {
      got2_addend = object->get_output_section_offset(got2_shndx);
      gold_assert(got2_addend != invalid_address);
    }

  unsigned char* pwrite = reloc_view;
  bool zap_next = false;
  for (size_t i = 0; i < reloc_count; ++i, prelocs += reloc_size)
    {
      Relocatable_relocs::Reloc_strategy strategy = rr->strategy(i);
      if (strategy == Relocatable_relocs::RELOC_DISCARD)
	continue;

      Reltype reloc(prelocs);
      Reltype_write reloc_write(pwrite);

      Address offset = reloc.get_r_offset();
      typename elfcpp::Elf_types<size>::Elf_WXword r_info = reloc.get_r_info();
      unsigned int r_sym = elfcpp::elf_r_sym<size>(r_info);
      unsigned int r_type = elfcpp::elf_r_type<size>(r_info);
      const unsigned int orig_r_sym = r_sym;
      typename elfcpp::Elf_types<size>::Elf_Swxword addend
	= reloc.get_r_addend();
      const Symbol* gsym = NULL;

      if (zap_next)
	{
	  // We could arrange to discard these and other relocs for
	  // tls optimised sequences in the strategy methods, but for
	  // now do as BFD ld does.
	  r_type = elfcpp::R_POWERPC_NONE;
	  zap_next = false;
	}

      // Get the new symbol index.
      if (r_sym < local_count)
	{
	  switch (strategy)
	    {
	    case Relocatable_relocs::RELOC_COPY:
	    case Relocatable_relocs::RELOC_SPECIAL:
	      if (r_sym != 0)
		{
		  r_sym = object->symtab_index(r_sym);
		  gold_assert(r_sym != -1U);
		}
	      break;

	    case Relocatable_relocs::RELOC_ADJUST_FOR_SECTION_RELA:
	      {
		// We are adjusting a section symbol.  We need to find
		// the symbol table index of the section symbol for
		// the output section corresponding to input section
		// in which this symbol is defined.
		gold_assert(r_sym < local_count);
		bool is_ordinary;
		unsigned int shndx =
		  object->local_symbol_input_shndx(r_sym, &is_ordinary);
		gold_assert(is_ordinary);
		Output_section* os = object->output_section(shndx);
		gold_assert(os != NULL);
		gold_assert(os->needs_symtab_index());
		r_sym = os->symtab_index();
	      }
	      break;

	    default:
	      gold_unreachable();
	    }
	}
      else
	{
	  gsym = object->global_symbol(r_sym);
	  gold_assert(gsym != NULL);
	  if (gsym->is_forwarder())
	    gsym = relinfo->symtab->resolve_forwards(gsym);

	  gold_assert(gsym->has_symtab_index());
	  r_sym = gsym->symtab_index();
	}

      // Get the new offset--the location in the output section where
      // this relocation should be applied.
      if (static_cast<Address>(offset_in_output_section) != invalid_address)
	offset += offset_in_output_section;
      else
	{
	  section_offset_type sot_offset =
	    convert_types<section_offset_type, Address>(offset);
	  section_offset_type new_sot_offset =
	    output_section->output_offset(object, relinfo->data_shndx,
					  sot_offset);
	  gold_assert(new_sot_offset != -1);
	  offset = new_sot_offset;
	}

      // In an object file, r_offset is an offset within the section.
      // In an executable or dynamic object, generated by
      // --emit-relocs, r_offset is an absolute address.
      if (!parameters->options().relocatable())
	{
	  offset += view_address;
	  if (static_cast<Address>(offset_in_output_section) != invalid_address)
	    offset -= offset_in_output_section;
	}

      // Handle the reloc addend based on the strategy.
      if (strategy == Relocatable_relocs::RELOC_COPY)
	;
      else if (strategy == Relocatable_relocs::RELOC_ADJUST_FOR_SECTION_RELA)
	{
	  const Symbol_value<size>* psymval = object->local_symbol(orig_r_sym);
	  addend = psymval->value(object, addend);
	}
      else if (strategy == Relocatable_relocs::RELOC_SPECIAL)
	{
	  if (addend >= 32768)
	    addend += got2_addend;
	}
      else
	gold_unreachable();

      if (!parameters->options().relocatable())
	{
	  if (r_type == elfcpp::R_POWERPC_GOT_TLSGD16
	      || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_LO
	      || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_HI
	      || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_HA)
	    {
	      // First instruction of a global dynamic sequence,
	      // arg setup insn.
	      const bool final = gsym == NULL || gsym->final_value_is_known();
	      switch (this->optimize_tls_gd(final))
		{
		case tls::TLSOPT_TO_IE:
		  r_type += (elfcpp::R_POWERPC_GOT_TPREL16
			     - elfcpp::R_POWERPC_GOT_TLSGD16);
		  break;
		case tls::TLSOPT_TO_LE:
		  if (r_type == elfcpp::R_POWERPC_GOT_TLSGD16
		      || r_type == elfcpp::R_POWERPC_GOT_TLSGD16_LO)
		    r_type = elfcpp::R_POWERPC_TPREL16_HA;
		  else
		    {
		      r_type = elfcpp::R_POWERPC_NONE;
		      offset -= 2 * big_endian;
		    }
		  break;
		default:
		  break;
		}
	    }
	  else if (r_type == elfcpp::R_POWERPC_GOT_TLSLD16
		   || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_LO
		   || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_HI
		   || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_HA)
	    {
	      // First instruction of a local dynamic sequence,
	      // arg setup insn.
	      if (this->optimize_tls_ld() == tls::TLSOPT_TO_LE)
		{
		  if (r_type == elfcpp::R_POWERPC_GOT_TLSLD16
		      || r_type == elfcpp::R_POWERPC_GOT_TLSLD16_LO)
		    {
		      r_type = elfcpp::R_POWERPC_TPREL16_HA;
		      const Output_section* os = relinfo->layout->tls_segment()
			->first_section();
		      gold_assert(os != NULL);
		      gold_assert(os->needs_symtab_index());
		      r_sym = os->symtab_index();
		      addend = dtp_offset;
		    }
		  else
		    {
		      r_type = elfcpp::R_POWERPC_NONE;
		      offset -= 2 * big_endian;
		    }
		}
	    }
	  else if (r_type == elfcpp::R_POWERPC_GOT_TPREL16
		   || r_type == elfcpp::R_POWERPC_GOT_TPREL16_LO
		   || r_type == elfcpp::R_POWERPC_GOT_TPREL16_HI
		   || r_type == elfcpp::R_POWERPC_GOT_TPREL16_HA)
	    {
	      // First instruction of initial exec sequence.
	      const bool final = gsym == NULL || gsym->final_value_is_known();
	      if (this->optimize_tls_ie(final) == tls::TLSOPT_TO_LE)
		{
		  if (r_type == elfcpp::R_POWERPC_GOT_TPREL16
		      || r_type == elfcpp::R_POWERPC_GOT_TPREL16_LO)
		    r_type = elfcpp::R_POWERPC_TPREL16_HA;
		  else
		    {
		      r_type = elfcpp::R_POWERPC_NONE;
		      offset -= 2 * big_endian;
		    }
		}
	    }
	  else if ((size == 64 && r_type == elfcpp::R_PPC64_TLSGD)
		   || (size == 32 && r_type == elfcpp::R_PPC_TLSGD))
	    {
	      // Second instruction of a global dynamic sequence,
	      // the __tls_get_addr call
	      const bool final = gsym == NULL || gsym->final_value_is_known();
	      switch (this->optimize_tls_gd(final))
		{
		case tls::TLSOPT_TO_IE:
		  r_type = elfcpp::R_POWERPC_NONE;
		  zap_next = true;
		  break;
		case tls::TLSOPT_TO_LE:
		  r_type = elfcpp::R_POWERPC_TPREL16_LO;
		  offset += 2 * big_endian;
		  zap_next = true;
		  break;
		default:
		  break;
		}
	    }
	  else if ((size == 64 && r_type == elfcpp::R_PPC64_TLSLD)
		   || (size == 32 && r_type == elfcpp::R_PPC_TLSLD))
	    {
	      // Second instruction of a local dynamic sequence,
	      // the __tls_get_addr call
	      if (this->optimize_tls_ld() == tls::TLSOPT_TO_LE)
		{
		  const Output_section* os = relinfo->layout->tls_segment()
		    ->first_section();
		  gold_assert(os != NULL);
		  gold_assert(os->needs_symtab_index());
		  r_sym = os->symtab_index();
		  addend = dtp_offset;
		  r_type = elfcpp::R_POWERPC_TPREL16_LO;
		  offset += 2 * big_endian;
		  zap_next = true;
		}
	    }
	  else if (r_type == elfcpp::R_POWERPC_TLS)
	    {
	      // Second instruction of an initial exec sequence
	      const bool final = gsym == NULL || gsym->final_value_is_known();
	      if (this->optimize_tls_ie(final) == tls::TLSOPT_TO_LE)
		{
		  r_type = elfcpp::R_POWERPC_TPREL16_LO;
		  offset += 2 * big_endian;
		}
	    }
	}

      reloc_write.put_r_offset(offset);
      reloc_write.put_r_info(elfcpp::elf_r_info<size>(r_sym, r_type));
      reloc_write.put_r_addend(addend);

      pwrite += reloc_size;
    }

  gold_assert(static_cast<section_size_type>(pwrite - reloc_view)
	      == reloc_view_size);
}

// Return the value to use for a dynamic symbol which requires special
// treatment.  This is how we support equality comparisons of function
// pointers across shared library boundaries, as described in the
// processor specific ABI supplement.

template<int size, bool big_endian>
uint64_t
Target_powerpc<size, big_endian>::do_dynsym_value(const Symbol* gsym) const
{
  if (size == 32)
    {
      gold_assert(gsym->is_from_dynobj() && gsym->has_plt_offset());
      for (typename Stub_tables::const_iterator p = this->stub_tables_.begin();
	   p != this->stub_tables_.end();
	   ++p)
	{
	  Address off = (*p)->find_plt_call_entry(gsym);
	  if (off != invalid_address)
	    return (*p)->stub_address() + off;
	}
    }
  gold_unreachable();
}

// Return the PLT address to use for a local symbol.
template<int size, bool big_endian>
uint64_t
Target_powerpc<size, big_endian>::do_plt_address_for_local(
    const Relobj* object,
    unsigned int symndx) const
{
  if (size == 32)
    {
      const Sized_relobj<size, big_endian>* relobj
	= static_cast<const Sized_relobj<size, big_endian>*>(object);
      for (typename Stub_tables::const_iterator p = this->stub_tables_.begin();
	   p != this->stub_tables_.end();
	   ++p)
	{
	  Address off = (*p)->find_plt_call_entry(relobj->sized_relobj(),
						  symndx);
	  if (off != invalid_address)
	    return (*p)->stub_address() + off;
	}
    }
  gold_unreachable();
}

// Return the PLT address to use for a global symbol.
template<int size, bool big_endian>
uint64_t
Target_powerpc<size, big_endian>::do_plt_address_for_global(
    const Symbol* gsym) const
{
  if (size == 32)
    {
      for (typename Stub_tables::const_iterator p = this->stub_tables_.begin();
	   p != this->stub_tables_.end();
	   ++p)
	{
	  Address off = (*p)->find_plt_call_entry(gsym);
	  if (off != invalid_address)
	    return (*p)->stub_address() + off;
	}
    }
  gold_unreachable();
}

// Return the offset to use for the GOT_INDX'th got entry which is
// for a local tls symbol specified by OBJECT, SYMNDX.
template<int size, bool big_endian>
int64_t
Target_powerpc<size, big_endian>::do_tls_offset_for_local(
    const Relobj* object,
    unsigned int symndx,
    unsigned int got_indx) const
{
  const Powerpc_relobj<size, big_endian>* ppc_object
    = static_cast<const Powerpc_relobj<size, big_endian>*>(object);
  if (ppc_object->local_symbol(symndx)->is_tls_symbol())
    {
      for (Got_type got_type = GOT_TYPE_TLSGD;
	   got_type <= GOT_TYPE_TPREL;
	   got_type = Got_type(got_type + 1))
	if (ppc_object->local_has_got_offset(symndx, got_type))
	  {
	    unsigned int off = ppc_object->local_got_offset(symndx, got_type);
	    if (got_type == GOT_TYPE_TLSGD)
	      off += size / 8;
	    if (off == got_indx * (size / 8))
	      {
		if (got_type == GOT_TYPE_TPREL)
		  return -tp_offset;
		else
		  return -dtp_offset;
	      }
	  }
    }
  gold_unreachable();
}

// Return the offset to use for the GOT_INDX'th got entry which is
// for global tls symbol GSYM.
template<int size, bool big_endian>
int64_t
Target_powerpc<size, big_endian>::do_tls_offset_for_global(
    Symbol* gsym,
    unsigned int got_indx) const
{
  if (gsym->type() == elfcpp::STT_TLS)
    {
      for (Got_type got_type = GOT_TYPE_TLSGD;
	   got_type <= GOT_TYPE_TPREL;
	   got_type = Got_type(got_type + 1))
	if (gsym->has_got_offset(got_type))
	  {
	    unsigned int off = gsym->got_offset(got_type);
	    if (got_type == GOT_TYPE_TLSGD)
	      off += size / 8;
	    if (off == got_indx * (size / 8))
	      {
		if (got_type == GOT_TYPE_TPREL)
		  return -tp_offset;
		else
		  return -dtp_offset;
	      }
	  }
    }
  gold_unreachable();
}

// The selector for powerpc object files.

template<int size, bool big_endian>
class Target_selector_powerpc : public Target_selector
{
public:
  Target_selector_powerpc()
    : Target_selector(size == 64 ? elfcpp::EM_PPC64 : elfcpp::EM_PPC,
		      size, big_endian,
		      (size == 64
		       ? (big_endian ? "elf64-powerpc" : "elf64-powerpcle")
		       : (big_endian ? "elf32-powerpc" : "elf32-powerpcle")),
		      (size == 64
		       ? (big_endian ? "elf64ppc" : "elf64lppc")
		       : (big_endian ? "elf32ppc" : "elf32lppc")))
  { }

  virtual Target*
  do_instantiate_target()
  { return new Target_powerpc<size, big_endian>(); }
};

Target_selector_powerpc<32, true> target_selector_ppc32;
Target_selector_powerpc<32, false> target_selector_ppc32le;
Target_selector_powerpc<64, true> target_selector_ppc64;
Target_selector_powerpc<64, false> target_selector_ppc64le;

// Instantiate these constants for -O0
template<int size, bool big_endian>
const int Output_data_glink<size, big_endian>::pltresolve_size;
template<int size, bool big_endian>
const typename Stub_table<size, big_endian>::Address
  Stub_table<size, big_endian>::invalid_address;
template<int size, bool big_endian>
const typename Target_powerpc<size, big_endian>::Address
  Target_powerpc<size, big_endian>::invalid_address;

} // End anonymous namespace.
