/* IBM S/390-specific support for 32-bit ELF
   Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
   Contributed by Carl B. Pedersen and Martin Schwidefsky.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "elf-bfd.h"

static reloc_howto_type *elf_s390_reloc_type_lookup
  PARAMS ((bfd *, bfd_reloc_code_real_type));
static void elf_s390_info_to_howto
  PARAMS ((bfd *, arelent *, Elf_Internal_Rela *));
static bfd_boolean elf_s390_is_local_label_name
  PARAMS ((bfd *, const char *));
static struct bfd_hash_entry *link_hash_newfunc
  PARAMS ((struct bfd_hash_entry *, struct bfd_hash_table *, const char *));
static struct bfd_link_hash_table *elf_s390_link_hash_table_create
  PARAMS ((bfd *));
static bfd_boolean create_got_section
  PARAMS((bfd *, struct bfd_link_info *));
static bfd_boolean elf_s390_create_dynamic_sections
  PARAMS((bfd *, struct bfd_link_info *));
static void elf_s390_copy_indirect_symbol
  PARAMS ((struct elf_backend_data *, struct elf_link_hash_entry *,
	   struct elf_link_hash_entry *));
static bfd_boolean elf_s390_check_relocs
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   const Elf_Internal_Rela *));
static asection *elf_s390_gc_mark_hook
  PARAMS ((asection *, struct bfd_link_info *, Elf_Internal_Rela *,
	   struct elf_link_hash_entry *, Elf_Internal_Sym *));
static bfd_boolean elf_s390_gc_sweep_hook
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   const Elf_Internal_Rela *));
struct elf_s390_link_hash_entry;
static void elf_s390_adjust_gotplt
  PARAMS ((struct elf_s390_link_hash_entry *));
static bfd_boolean elf_s390_adjust_dynamic_symbol
  PARAMS ((struct bfd_link_info *, struct elf_link_hash_entry *));
static bfd_boolean allocate_dynrelocs
  PARAMS ((struct elf_link_hash_entry *, PTR));
static bfd_boolean readonly_dynrelocs
  PARAMS ((struct elf_link_hash_entry *, PTR));
static bfd_boolean elf_s390_size_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));
static bfd_boolean elf_s390_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *, bfd_byte *,
	   Elf_Internal_Rela *, Elf_Internal_Sym *, asection **));
static bfd_boolean elf_s390_finish_dynamic_symbol
  PARAMS ((bfd *, struct bfd_link_info *, struct elf_link_hash_entry *,
	   Elf_Internal_Sym *));
static enum elf_reloc_type_class elf_s390_reloc_type_class
  PARAMS ((const Elf_Internal_Rela *));
static bfd_boolean elf_s390_finish_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));
static bfd_boolean elf_s390_mkobject
  PARAMS ((bfd *));
static bfd_boolean elf_s390_object_p
  PARAMS ((bfd *));
static bfd_boolean elf_s390_grok_prstatus
  PARAMS ((bfd *, Elf_Internal_Note *));
static int elf_s390_tls_transition
  PARAMS ((struct bfd_link_info *, int, int));
static bfd_reloc_status_type s390_tls_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));
static bfd_vma dtpoff_base
  PARAMS ((struct bfd_link_info *));
static bfd_vma tpoff
  PARAMS ((struct bfd_link_info *, bfd_vma));
static void invalid_tls_insn
  PARAMS ((bfd *, asection *, Elf_Internal_Rela *));

#include "elf/s390.h"

/* The relocation "howto" table.  */

static reloc_howto_type elf_howto_table[] =
{
  HOWTO (R_390_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_390_NONE",		/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  HOWTO(R_390_8,         0, 0,  8, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_8",        FALSE, 0,0x000000ff, FALSE),
  HOWTO(R_390_12,        0, 1, 12, FALSE, 0, complain_overflow_dont,
	bfd_elf_generic_reloc, "R_390_12",       FALSE, 0,0x00000fff, FALSE),
  HOWTO(R_390_16,        0, 1, 16, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_16",       FALSE, 0,0x0000ffff, FALSE),
  HOWTO(R_390_32,        0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_32",       FALSE, 0,0xffffffff, FALSE),
  HOWTO(R_390_PC32,	 0, 2, 32,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_PC32",     FALSE, 0,0xffffffff, TRUE),
  HOWTO(R_390_GOT12,	 0, 1, 12, FALSE, 0, complain_overflow_dont,
	bfd_elf_generic_reloc, "R_390_GOT12",    FALSE, 0,0x00000fff, FALSE),
  HOWTO(R_390_GOT32,	 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOT32",    FALSE, 0,0xffffffff, FALSE),
  HOWTO(R_390_PLT32,	 0, 2, 32,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_PLT32",    FALSE, 0,0xffffffff, TRUE),
  HOWTO(R_390_COPY,      0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_COPY",     FALSE, 0,0xffffffff, FALSE),
  HOWTO(R_390_GLOB_DAT,  0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GLOB_DAT", FALSE, 0,0xffffffff, FALSE),
  HOWTO(R_390_JMP_SLOT,  0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_JMP_SLOT", FALSE, 0,0xffffffff, FALSE),
  HOWTO(R_390_RELATIVE,  0, 2, 32,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_RELATIVE", FALSE, 0,0xffffffff, FALSE),
  HOWTO(R_390_GOTOFF32,  0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOTOFF32", FALSE, 0,0xffffffff, FALSE),
  HOWTO(R_390_GOTPC,     0, 2, 32,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOTPC",    FALSE, 0,0xffffffff, TRUE),
  HOWTO(R_390_GOT16,     0, 1, 16, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOT16",    FALSE, 0,0x0000ffff, FALSE),
  HOWTO(R_390_PC16,      0, 1, 16,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_PC16",     FALSE, 0,0x0000ffff, TRUE),
  HOWTO(R_390_PC16DBL,   1, 1, 16,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_PC16DBL",  FALSE, 0,0x0000ffff, TRUE),
  HOWTO(R_390_PLT16DBL,  1, 1, 16,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_PLT16DBL", FALSE, 0,0x0000ffff, TRUE),
  HOWTO(R_390_PC32DBL,	 1, 2, 32,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_PC32DBL",  FALSE, 0,0xffffffff, TRUE),
  HOWTO(R_390_PLT32DBL,	 1, 2, 32,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_PLT32DBL", FALSE, 0,0xffffffff, TRUE),
  HOWTO(R_390_GOTPCDBL,  1, 2, 32,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOTPCDBL", FALSE, 0,0xffffffff, TRUE),
  EMPTY_HOWTO (R_390_64),	/* Empty entry for R_390_64.  */
  EMPTY_HOWTO (R_390_PC64),	/* Empty entry for R_390_PC64.  */
  EMPTY_HOWTO (R_390_GOT64),	/* Empty entry for R_390_GOT64.  */
  EMPTY_HOWTO (R_390_PLT64),	/* Empty entry for R_390_PLT64.  */
  HOWTO(R_390_GOTENT,	 1, 2, 32,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOTENT",   FALSE, 0,0xffffffff, TRUE),
  HOWTO(R_390_GOTOFF16,  0, 1, 16, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOTOFF16", FALSE, 0,0x0000ffff, FALSE),
  EMPTY_HOWTO (R_390_GOTOFF64),	/* Empty entry for R_390_GOTOFF64.  */
  HOWTO(R_390_GOTPLT12,  0, 1, 12, FALSE, 0, complain_overflow_dont,
	bfd_elf_generic_reloc, "R_390_GOTPLT12", FALSE, 0,0x00000fff, FALSE),
  HOWTO(R_390_GOTPLT16,  0, 1, 16, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOTPLT16", FALSE, 0,0x0000ffff, FALSE),
  HOWTO(R_390_GOTPLT32,  0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOTPLT32", FALSE, 0,0xffffffff, FALSE),
  EMPTY_HOWTO (R_390_GOTPLT64),	/* Empty entry for R_390_GOTPLT64.  */
  HOWTO(R_390_GOTPLTENT, 1, 2, 32,  TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_GOTPLTENT",FALSE, 0,0xffffffff, TRUE),
  HOWTO(R_390_PLTOFF16,  0, 1, 16, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_PLTOFF16", FALSE, 0,0x0000ffff, FALSE),
  HOWTO(R_390_PLTOFF32,  0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_PLTOFF32", FALSE, 0,0xffffffff, FALSE),
  EMPTY_HOWTO (R_390_PLTOFF64),	/* Empty entry for R_390_PLTOFF64.  */
  HOWTO(R_390_TLS_LOAD, 0, 0, 0, FALSE, 0, complain_overflow_dont,
	s390_tls_reloc, "R_390_TLS_LOAD", FALSE, 0, 0, FALSE),
  HOWTO(R_390_TLS_GDCALL, 0, 0, 0, FALSE, 0, complain_overflow_dont,
	s390_tls_reloc, "R_390_TLS_GDCALL", FALSE, 0, 0, FALSE),
  HOWTO(R_390_TLS_LDCALL, 0, 0, 0, FALSE, 0, complain_overflow_dont,
	s390_tls_reloc, "R_390_TLS_LDCALL", FALSE, 0, 0, FALSE),
  HOWTO(R_390_TLS_GD32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_GD32", FALSE, 0, 0xffffffff, FALSE),
  EMPTY_HOWTO (R_390_TLS_GD64),	/* Empty entry for R_390_TLS_GD64.  */
  HOWTO(R_390_TLS_GOTIE12, 0, 1, 12, FALSE, 0, complain_overflow_dont,
	bfd_elf_generic_reloc, "R_390_TLS_GOTIE12", FALSE, 0, 0x00000fff, FALSE),
  HOWTO(R_390_TLS_GOTIE32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_GOTIE32", FALSE, 0, 0xffffffff, FALSE),
  EMPTY_HOWTO (R_390_TLS_GOTIE64),	/* Empty entry for R_390_TLS_GOTIE64.  */
  HOWTO(R_390_TLS_LDM32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_LDM32", FALSE, 0, 0xffffffff, FALSE),
  EMPTY_HOWTO (R_390_TLS_LDM64),	/* Empty entry for R_390_TLS_LDM64.  */
  HOWTO(R_390_TLS_IE32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_IE32", FALSE, 0, 0xffffffff, FALSE),
  EMPTY_HOWTO (R_390_TLS_IE64),	/* Empty entry for R_390_TLS_IE64.  */
  HOWTO(R_390_TLS_IEENT, 1, 2, 32, TRUE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_IEENT", FALSE, 0, 0xffffffff, TRUE),
  HOWTO(R_390_TLS_LE32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_LE32", FALSE, 0, 0xffffffff, FALSE),
  EMPTY_HOWTO (R_390_TLS_LE64),	/* Empty entry for R_390_TLS_LE64.  */
  HOWTO(R_390_TLS_LDO32, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_LDO32", FALSE, 0, 0xffffffff, FALSE),
  EMPTY_HOWTO (R_390_TLS_LDO64),	/* Empty entry for R_390_TLS_LDO64.  */
  HOWTO(R_390_TLS_DTPMOD, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_DTPMOD", FALSE, 0, 0xffffffff, FALSE),
  HOWTO(R_390_TLS_DTPOFF, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_DTPOFF", FALSE, 0, 0xffffffff, FALSE),
  HOWTO(R_390_TLS_TPOFF, 0, 2, 32, FALSE, 0, complain_overflow_bitfield,
	bfd_elf_generic_reloc, "R_390_TLS_TPOFF", FALSE, 0, 0xffffffff, FALSE),
};

/* GNU extension to record C++ vtable hierarchy.  */
static reloc_howto_type elf32_s390_vtinherit_howto =
  HOWTO (R_390_GNU_VTINHERIT, 0,2,0,FALSE,0,complain_overflow_dont, NULL, "R_390_GNU_VTINHERIT", FALSE,0, 0, FALSE);
static reloc_howto_type elf32_s390_vtentry_howto =
  HOWTO (R_390_GNU_VTENTRY, 0,2,0,FALSE,0,complain_overflow_dont, _bfd_elf_rel_vtable_reloc_fn,"R_390_GNU_VTENTRY", FALSE,0,0, FALSE);

static reloc_howto_type *
elf_s390_reloc_type_lookup (abfd, code)
     bfd *abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  switch (code)
    {
    case BFD_RELOC_NONE:
      return &elf_howto_table[(int) R_390_NONE];
    case BFD_RELOC_8:
      return &elf_howto_table[(int) R_390_8];
    case BFD_RELOC_390_12:
      return &elf_howto_table[(int) R_390_12];
    case BFD_RELOC_16:
      return &elf_howto_table[(int) R_390_16];
    case BFD_RELOC_32:
      return &elf_howto_table[(int) R_390_32];
    case BFD_RELOC_CTOR:
      return &elf_howto_table[(int) R_390_32];
    case BFD_RELOC_32_PCREL:
      return &elf_howto_table[(int) R_390_PC32];
    case BFD_RELOC_390_GOT12:
      return &elf_howto_table[(int) R_390_GOT12];
    case BFD_RELOC_32_GOT_PCREL:
      return &elf_howto_table[(int) R_390_GOT32];
    case BFD_RELOC_390_PLT32:
      return &elf_howto_table[(int) R_390_PLT32];
    case BFD_RELOC_390_COPY:
      return &elf_howto_table[(int) R_390_COPY];
    case BFD_RELOC_390_GLOB_DAT:
      return &elf_howto_table[(int) R_390_GLOB_DAT];
    case BFD_RELOC_390_JMP_SLOT:
      return &elf_howto_table[(int) R_390_JMP_SLOT];
    case BFD_RELOC_390_RELATIVE:
      return &elf_howto_table[(int) R_390_RELATIVE];
    case BFD_RELOC_32_GOTOFF:
      return &elf_howto_table[(int) R_390_GOTOFF32];
    case BFD_RELOC_390_GOTPC:
      return &elf_howto_table[(int) R_390_GOTPC];
    case BFD_RELOC_390_GOT16:
      return &elf_howto_table[(int) R_390_GOT16];
    case BFD_RELOC_16_PCREL:
      return &elf_howto_table[(int) R_390_PC16];
    case BFD_RELOC_390_PC16DBL:
      return &elf_howto_table[(int) R_390_PC16DBL];
    case BFD_RELOC_390_PLT16DBL:
      return &elf_howto_table[(int) R_390_PLT16DBL];
    case BFD_RELOC_390_PC32DBL:
      return &elf_howto_table[(int) R_390_PC32DBL];
    case BFD_RELOC_390_PLT32DBL:
      return &elf_howto_table[(int) R_390_PLT32DBL];
    case BFD_RELOC_390_GOTPCDBL:
      return &elf_howto_table[(int) R_390_GOTPCDBL];
    case BFD_RELOC_390_GOTENT:
      return &elf_howto_table[(int) R_390_GOTENT];
    case BFD_RELOC_16_GOTOFF:
      return &elf_howto_table[(int) R_390_GOTOFF16];
    case BFD_RELOC_390_GOTPLT12:
      return &elf_howto_table[(int) R_390_GOTPLT12];
    case BFD_RELOC_390_GOTPLT16:
      return &elf_howto_table[(int) R_390_GOTPLT16];
    case BFD_RELOC_390_GOTPLT32:
      return &elf_howto_table[(int) R_390_GOTPLT32];
    case BFD_RELOC_390_GOTPLTENT:
      return &elf_howto_table[(int) R_390_GOTPLTENT];
    case BFD_RELOC_390_PLTOFF16:
      return &elf_howto_table[(int) R_390_PLTOFF16];
    case BFD_RELOC_390_PLTOFF32:
      return &elf_howto_table[(int) R_390_PLTOFF32];
    case BFD_RELOC_390_TLS_LOAD:
      return &elf_howto_table[(int) R_390_TLS_LOAD];
    case BFD_RELOC_390_TLS_GDCALL:
      return &elf_howto_table[(int) R_390_TLS_GDCALL];
    case BFD_RELOC_390_TLS_LDCALL:
      return &elf_howto_table[(int) R_390_TLS_LDCALL];
    case BFD_RELOC_390_TLS_GD32:
      return &elf_howto_table[(int) R_390_TLS_GD32];
    case BFD_RELOC_390_TLS_GOTIE12:
      return &elf_howto_table[(int) R_390_TLS_GOTIE12];
    case BFD_RELOC_390_TLS_GOTIE32:
      return &elf_howto_table[(int) R_390_TLS_GOTIE32];
    case BFD_RELOC_390_TLS_LDM32:
      return &elf_howto_table[(int) R_390_TLS_LDM32];
    case BFD_RELOC_390_TLS_IE32:
      return &elf_howto_table[(int) R_390_TLS_IE32];
    case BFD_RELOC_390_TLS_IEENT:
      return &elf_howto_table[(int) R_390_TLS_IEENT];
    case BFD_RELOC_390_TLS_LE32:
      return &elf_howto_table[(int) R_390_TLS_LE32];
    case BFD_RELOC_390_TLS_LDO32:
      return &elf_howto_table[(int) R_390_TLS_LDO32];
    case BFD_RELOC_390_TLS_DTPMOD:
      return &elf_howto_table[(int) R_390_TLS_DTPMOD];
    case BFD_RELOC_390_TLS_DTPOFF:
      return &elf_howto_table[(int) R_390_TLS_DTPOFF];
    case BFD_RELOC_390_TLS_TPOFF:
      return &elf_howto_table[(int) R_390_TLS_TPOFF];
    case BFD_RELOC_VTABLE_INHERIT:
      return &elf32_s390_vtinherit_howto;
    case BFD_RELOC_VTABLE_ENTRY:
      return &elf32_s390_vtentry_howto;
    default:
      break;
    }
  return 0;
}

/* We need to use ELF32_R_TYPE so we have our own copy of this function,
   and elf32-s390.c has its own copy.  */

static void
elf_s390_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *cache_ptr;
     Elf_Internal_Rela *dst;
{
  switch (ELF32_R_TYPE(dst->r_info))
    {
    case R_390_GNU_VTINHERIT:
      cache_ptr->howto = &elf32_s390_vtinherit_howto;
      break;

    case R_390_GNU_VTENTRY:
      cache_ptr->howto = &elf32_s390_vtentry_howto;
      break;

    default:
      BFD_ASSERT (ELF32_R_TYPE(dst->r_info) < (unsigned int) R_390_max);
      cache_ptr->howto = &elf_howto_table[ELF32_R_TYPE(dst->r_info)];
    }
}

/* A relocation function which doesn't do anything.  */
static bfd_reloc_status_type
s390_tls_reloc (abfd, reloc_entry, symbol, data, input_section,
		output_bfd, error_message)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *reloc_entry;
     asymbol *symbol ATTRIBUTE_UNUSED;
     PTR data ATTRIBUTE_UNUSED;
     asection *input_section;
     bfd *output_bfd;
     char **error_message ATTRIBUTE_UNUSED;
{
  if (output_bfd)
    reloc_entry->address += input_section->output_offset;
  return bfd_reloc_ok;
}

static bfd_boolean
elf_s390_is_local_label_name (abfd, name)
     bfd *abfd;
     const char *name;
{
  if (name[0] == '.' && (name[1] == 'X' || name[1] == 'L'))
    return TRUE;

  return _bfd_elf_is_local_label_name (abfd, name);
}

/* Functions for the 390 ELF linker.  */

/* The name of the dynamic interpreter.  This is put in the .interp
   section.  */

#define ELF_DYNAMIC_INTERPRETER "/usr/lib/ld.so.1"

/* If ELIMINATE_COPY_RELOCS is non-zero, the linker will try to avoid
   copying dynamic variables from a shared lib into an app's dynbss
   section, and instead use a dynamic relocation to point into the
   shared lib.  */
#define ELIMINATE_COPY_RELOCS 1

/* The size in bytes of the first entry in the procedure linkage table.  */
#define PLT_FIRST_ENTRY_SIZE 32
/* The size in bytes of an entry in the procedure linkage table.  */
#define PLT_ENTRY_SIZE 32

#define GOT_ENTRY_SIZE 4

/* The first three entries in a procedure linkage table are reserved,
   and the initial contents are unimportant (we zero them out).
   Subsequent entries look like this.  See the SVR4 ABI 386
   supplement to see how this works.  */

/* For the s390, simple addr offset can only be 0 - 4096.
   To use the full 2 GB address space, several instructions
   are needed to load an address in a register and execute
   a branch( or just saving the address)

   Furthermore, only r 0 and 1 are free to use!!!  */

/* The first 3 words in the GOT are then reserved.
   Word 0 is the address of the dynamic table.
   Word 1 is a pointer to a structure describing the object
   Word 2 is used to point to the loader entry address.

   The code for position independand PLT entries looks like this:

   r12 holds addr of the current GOT at entry to the PLT

   The GOT holds the address in the PLT to be executed.
   The loader then gets:
   24(15) =  Pointer to the structure describing the object.
   28(15) =  Offset in symbol table

   The loader  must  then find the module where the function is
   and insert the address in the GOT.

  Note: 390 can only address +- 64 K relative.
        We check if offset > 65536, then make a relative branch -64xxx
        back to a previous defined branch

PLT1: BASR 1,0         # 2 bytes
      L    1,22(1)     # 4 bytes  Load offset in GOT in r 1
      L    1,(1,12)    # 4 bytes  Load address from GOT in r1
      BCR  15,1        # 2 bytes  Jump to address
RET1: BASR 1,0         # 2 bytes  Return from GOT 1st time
      L    1,14(1)     # 4 bytes  Load offset in symol table in r1
      BRC  15,-x       # 4 bytes  Jump to start of PLT
      .word 0          # 2 bytes filler
      .long ?          # 4 bytes  offset in GOT
      .long ?          # 4 bytes  offset into symbol table

  This was the general case. There are two additional, optimizes PLT
  definitions. One for GOT offsets < 4096 and one for GOT offsets < 32768.
  First the one for GOT offsets < 4096:

PLT1: L    1,<offset>(12) # 4 bytes  Load address from GOT in R1
      BCR  15,1           # 2 bytes  Jump to address
      .word 0,0,0         # 6 bytes  filler
RET1: BASR 1,0            # 2 bytes  Return from GOT 1st time
      L    1,14(1)        # 4 bytes  Load offset in symbol table in r1
      BRC  15,-x          # 4 bytes  Jump to start of PLT
      .word 0,0,0         # 6 bytes  filler
      .long ?             # 4 bytes  offset into symbol table

  Second the one for GOT offsets < 32768:

PLT1: LHI  1,<offset>     # 4 bytes  Load offset in GOT to r1
      L    1,(1,12)       # 4 bytes  Load address from GOT to r1
      BCR  15,1           # 2 bytes  Jump to address
      .word 0             # 2 bytes  filler
RET1: BASR 1,0            # 2 bytes  Return from GOT 1st time
      L    1,14(1)        # 4 bytes  Load offset in symbol table in r1
      BRC  15,-x          # 4 bytes  Jump to start of PLT
      .word 0,0,0         # 6 bytes  filler
      .long ?             # 4 bytes  offset into symbol table

Total = 32 bytes per PLT entry

   The code for static build PLT entries looks like this:

PLT1: BASR 1,0         # 2 bytes
      L    1,22(1)     # 4 bytes  Load address of GOT entry
      L    1,0(0,1)    # 4 bytes  Load address from GOT in r1
      BCR  15,1        # 2 bytes  Jump to address
RET1: BASR 1,0         # 2 bytes  Return from GOT 1st time
      L    1,14(1)     # 4 bytes  Load offset in symbol table in r1
      BRC  15,-x       # 4 bytes  Jump to start of PLT
      .word 0          # 2 bytes  filler
      .long ?          # 4 bytes  address of GOT entry
      .long ?          # 4 bytes  offset into symbol table  */

#define PLT_PIC_ENTRY_WORD0 0x0d105810
#define PLT_PIC_ENTRY_WORD1 0x10165811
#define PLT_PIC_ENTRY_WORD2 0xc00007f1
#define PLT_PIC_ENTRY_WORD3 0x0d105810
#define PLT_PIC_ENTRY_WORD4 0x100ea7f4

#define PLT_PIC12_ENTRY_WORD0 0x5810c000
#define PLT_PIC12_ENTRY_WORD1 0x07f10000
#define PLT_PIC12_ENTRY_WORD2 0x00000000
#define PLT_PIC12_ENTRY_WORD3 0x0d105810
#define PLT_PIC12_ENTRY_WORD4 0x100ea7f4

#define PLT_PIC16_ENTRY_WORD0 0xa7180000
#define PLT_PIC16_ENTRY_WORD1 0x5811c000
#define PLT_PIC16_ENTRY_WORD2 0x07f10000
#define PLT_PIC16_ENTRY_WORD3 0x0d105810
#define PLT_PIC16_ENTRY_WORD4 0x100ea7f4

#define PLT_ENTRY_WORD0     0x0d105810
#define PLT_ENTRY_WORD1     0x10165810
#define PLT_ENTRY_WORD2     0x100007f1
#define PLT_ENTRY_WORD3     0x0d105810
#define PLT_ENTRY_WORD4     0x100ea7f4

/* The first PLT entry pushes the offset into the symbol table
   from R1 onto the stack at 8(15) and the loader object info
   at 12(15), loads the loader address in R1 and jumps to it.  */

/* The first entry in the PLT for PIC code:

PLT0:
   ST   1,28(15)  # R1 has offset into symbol table
   L    1,4(12)   # Get loader ino(object struct address)
   ST   1,24(15)  # Store address
   L    1,8(12)   # Entry address of loader in R1
   BR   1         # Jump to loader

   The first entry in the PLT for static code:

PLT0:
   ST   1,28(15)      # R1 has offset into symbol table
   BASR 1,0
   L    1,18(0,1)     # Get address of GOT
   MVC  24(4,15),4(1) # Move loader ino to stack
   L    1,8(1)        # Get address of loader
   BR   1             # Jump to loader
   .word 0            # filler
   .long got          # address of GOT  */

#define PLT_PIC_FIRST_ENTRY_WORD0 0x5010f01c
#define PLT_PIC_FIRST_ENTRY_WORD1 0x5810c004
#define PLT_PIC_FIRST_ENTRY_WORD2 0x5010f018
#define PLT_PIC_FIRST_ENTRY_WORD3 0x5810c008
#define PLT_PIC_FIRST_ENTRY_WORD4 0x07f10000

#define PLT_FIRST_ENTRY_WORD0     0x5010f01c
#define PLT_FIRST_ENTRY_WORD1     0x0d105810
#define PLT_FIRST_ENTRY_WORD2     0x1012D203
#define PLT_FIRST_ENTRY_WORD3     0xf0181004
#define PLT_FIRST_ENTRY_WORD4     0x58101008
#define PLT_FIRST_ENTRY_WORD5     0x07f10000

/* The s390 linker needs to keep track of the number of relocs that it
   decides to copy as dynamic relocs in check_relocs for each symbol.
   This is so that it can later discard them if they are found to be
   unnecessary.  We store the information in a field extending the
   regular ELF linker hash table.  */

struct elf_s390_dyn_relocs
{
  struct elf_s390_dyn_relocs *next;

  /* The input section of the reloc.  */
  asection *sec;

  /* Total number of relocs copied for the input section.  */
  bfd_size_type count;

  /* Number of pc-relative relocs copied for the input section.  */
  bfd_size_type pc_count;
};

/* s390 ELF linker hash entry.  */

struct elf_s390_link_hash_entry
{
  struct elf_link_hash_entry elf;

  /* Track dynamic relocs copied for this symbol.  */
  struct elf_s390_dyn_relocs *dyn_relocs;

  /* Number of GOTPLT references for a function.  */
  bfd_signed_vma gotplt_refcount;

#define GOT_UNKNOWN	0
#define GOT_NORMAL	1
#define GOT_TLS_GD	2
#define GOT_TLS_IE	3
#define GOT_TLS_IE_NLT	4
  unsigned char tls_type;
};

#define elf_s390_hash_entry(ent) \
  ((struct elf_s390_link_hash_entry *)(ent))

struct elf_s390_obj_tdata
{
  struct elf_obj_tdata root;

  /* tls_type for each local got entry.  */
  char *local_got_tls_type;
};

#define elf_s390_tdata(abfd) \
  ((struct elf_s390_obj_tdata *) (abfd)->tdata.any)

#define elf_s390_local_got_tls_type(abfd) \
  (elf_s390_tdata (abfd)->local_got_tls_type)

static bfd_boolean
elf_s390_mkobject (abfd)
     bfd *abfd;
{
  bfd_size_type amt = sizeof (struct elf_s390_obj_tdata);
  abfd->tdata.any = bfd_zalloc (abfd, amt);
  if (abfd->tdata.any == NULL)
    return FALSE;
  return TRUE;
}

static bfd_boolean
elf_s390_object_p (abfd)
     bfd *abfd;
{
  /* Allocate our special target data.  */
  struct elf_s390_obj_tdata *new_tdata;
  bfd_size_type amt = sizeof (struct elf_s390_obj_tdata);
  new_tdata = bfd_zalloc (abfd, amt);
  if (new_tdata == NULL)
    return FALSE;
  new_tdata->root = *abfd->tdata.elf_obj_data;
  abfd->tdata.any = new_tdata;
  /* Set the right machine number for an s390 elf32 file.  */
  return bfd_default_set_arch_mach (abfd, bfd_arch_s390, bfd_mach_s390_31);
}

/* s390 ELF linker hash table.  */

struct elf_s390_link_hash_table
{
  struct elf_link_hash_table elf;

  /* Short-cuts to get to dynamic linker sections.  */
  asection *sgot;
  asection *sgotplt;
  asection *srelgot;
  asection *splt;
  asection *srelplt;
  asection *sdynbss;
  asection *srelbss;

  union {
    bfd_signed_vma refcount;
    bfd_vma offset;
  } tls_ldm_got;

  /* Small local sym to section mapping cache.  */
  struct sym_sec_cache sym_sec;
};

/* Get the s390 ELF linker hash table from a link_info structure.  */

#define elf_s390_hash_table(p) \
  ((struct elf_s390_link_hash_table *) ((p)->hash))

/* Create an entry in an s390 ELF linker hash table.  */

static struct bfd_hash_entry *
link_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (entry == NULL)
    {
      entry = bfd_hash_allocate (table,
				 sizeof (struct elf_s390_link_hash_entry));
      if (entry == NULL)
	return entry;
    }

  /* Call the allocation method of the superclass.  */
  entry = _bfd_elf_link_hash_newfunc (entry, table, string);
  if (entry != NULL)
    {
      struct elf_s390_link_hash_entry *eh;

      eh = (struct elf_s390_link_hash_entry *) entry;
      eh->dyn_relocs = NULL;
      eh->gotplt_refcount = 0;
      eh->tls_type = GOT_UNKNOWN;
    }

  return entry;
}

/* Create an s390 ELF linker hash table.  */

static struct bfd_link_hash_table *
elf_s390_link_hash_table_create (abfd)
     bfd *abfd;
{
  struct elf_s390_link_hash_table *ret;
  bfd_size_type amt = sizeof (struct elf_s390_link_hash_table);

  ret = (struct elf_s390_link_hash_table *) bfd_malloc (amt);
  if (ret == NULL)
    return NULL;

  if (! _bfd_elf_link_hash_table_init (&ret->elf, abfd, link_hash_newfunc))
    {
      free (ret);
      return NULL;
    }

  ret->sgot = NULL;
  ret->sgotplt = NULL;
  ret->srelgot = NULL;
  ret->splt = NULL;
  ret->srelplt = NULL;
  ret->sdynbss = NULL;
  ret->srelbss = NULL;
  ret->tls_ldm_got.refcount = 0;
  ret->sym_sec.abfd = NULL;

  return &ret->elf.root;
}

/* Create .got, .gotplt, and .rela.got sections in DYNOBJ, and set up
   shortcuts to them in our hash table.  */

static bfd_boolean
create_got_section (dynobj, info)
     bfd *dynobj;
     struct bfd_link_info *info;
{
  struct elf_s390_link_hash_table *htab;

  if (! _bfd_elf_create_got_section (dynobj, info))
    return FALSE;

  htab = elf_s390_hash_table (info);
  htab->sgot = bfd_get_section_by_name (dynobj, ".got");
  htab->sgotplt = bfd_get_section_by_name (dynobj, ".got.plt");
  if (!htab->sgot || !htab->sgotplt)
    abort ();

  htab->srelgot = bfd_make_section (dynobj, ".rela.got");
  if (htab->srelgot == NULL
      || ! bfd_set_section_flags (dynobj, htab->srelgot,
				  (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS
				   | SEC_IN_MEMORY | SEC_LINKER_CREATED
				   | SEC_READONLY))
      || ! bfd_set_section_alignment (dynobj, htab->srelgot, 2))
    return FALSE;
  return TRUE;
}

/* Create .plt, .rela.plt, .got, .got.plt, .rela.got, .dynbss, and
   .rela.bss sections in DYNOBJ, and set up shortcuts to them in our
   hash table.  */

static bfd_boolean
elf_s390_create_dynamic_sections (dynobj, info)
     bfd *dynobj;
     struct bfd_link_info *info;
{
  struct elf_s390_link_hash_table *htab;

  htab = elf_s390_hash_table (info);
  if (!htab->sgot && !create_got_section (dynobj, info))
    return FALSE;

  if (!_bfd_elf_create_dynamic_sections (dynobj, info))
    return FALSE;

  htab->splt = bfd_get_section_by_name (dynobj, ".plt");
  htab->srelplt = bfd_get_section_by_name (dynobj, ".rela.plt");
  htab->sdynbss = bfd_get_section_by_name (dynobj, ".dynbss");
  if (!info->shared)
    htab->srelbss = bfd_get_section_by_name (dynobj, ".rela.bss");

  if (!htab->splt || !htab->srelplt || !htab->sdynbss
      || (!info->shared && !htab->srelbss))
    abort ();

  return TRUE;
}

/* Copy the extra info we tack onto an elf_link_hash_entry.  */

static void
elf_s390_copy_indirect_symbol (bed, dir, ind)
     struct elf_backend_data *bed;
     struct elf_link_hash_entry *dir, *ind;
{
  struct elf_s390_link_hash_entry *edir, *eind;

  edir = (struct elf_s390_link_hash_entry *) dir;
  eind = (struct elf_s390_link_hash_entry *) ind;

  if (eind->dyn_relocs != NULL)
    {
      if (edir->dyn_relocs != NULL)
	{
	  struct elf_s390_dyn_relocs **pp;
	  struct elf_s390_dyn_relocs *p;

	  if (ind->root.type == bfd_link_hash_indirect)
	    abort ();

	  /* Add reloc counts against the weak sym to the strong sym
	     list.  Merge any entries against the same section.  */
	  for (pp = &eind->dyn_relocs; (p = *pp) != NULL; )
	    {
	      struct elf_s390_dyn_relocs *q;

	      for (q = edir->dyn_relocs; q != NULL; q = q->next)
		if (q->sec == p->sec)
		  {
		    q->pc_count += p->pc_count;
		    q->count += p->count;
		    *pp = p->next;
		    break;
		  }
	      if (q == NULL)
		pp = &p->next;
	    }
	  *pp = edir->dyn_relocs;
	}

      edir->dyn_relocs = eind->dyn_relocs;
      eind->dyn_relocs = NULL;
    }

  if (ind->root.type == bfd_link_hash_indirect
      && dir->got.refcount <= 0)
    {
      edir->tls_type = eind->tls_type;
      eind->tls_type = GOT_UNKNOWN;
    }

  if (ELIMINATE_COPY_RELOCS
      && ind->root.type != bfd_link_hash_indirect
      && (dir->elf_link_hash_flags & ELF_LINK_HASH_DYNAMIC_ADJUSTED) != 0)
    /* If called to transfer flags for a weakdef during processing
       of elf_adjust_dynamic_symbol, don't copy ELF_LINK_NON_GOT_REF.
       We clear it ourselves for ELIMINATE_COPY_RELOCS.  */
    dir->elf_link_hash_flags |=
      (ind->elf_link_hash_flags & (ELF_LINK_HASH_REF_DYNAMIC
				   | ELF_LINK_HASH_REF_REGULAR
				   | ELF_LINK_HASH_REF_REGULAR_NONWEAK));
  else
    _bfd_elf_link_hash_copy_indirect (bed, dir, ind);
}

static int
elf_s390_tls_transition (info, r_type, is_local)
     struct bfd_link_info *info;
     int r_type;
     int is_local;
{
  if (info->shared)
    return r_type;

  switch (r_type)
    {
    case R_390_TLS_GD32:
    case R_390_TLS_IE32:
      if (is_local)
	return R_390_TLS_LE32;
      return R_390_TLS_IE32;
    case R_390_TLS_GOTIE32:
      if (is_local)
	return R_390_TLS_LE32;
      return R_390_TLS_GOTIE32;
    case R_390_TLS_LDM32:
      return R_390_TLS_LE32;
    }

  return r_type;
}

/* Look through the relocs for a section during the first phase, and
   allocate space in the global offset table or procedure linkage
   table.  */

static bfd_boolean
elf_s390_check_relocs (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  struct elf_s390_link_hash_table *htab;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;
  asection *sreloc;
  bfd_signed_vma *local_got_refcounts;
  int tls_type, old_tls_type;

  if (info->relocatable)
    return TRUE;

  htab = elf_s390_hash_table (info);
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  local_got_refcounts = elf_local_got_refcounts (abfd);

  sreloc = NULL;

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      unsigned int r_type;
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;

      r_symndx = ELF32_R_SYM (rel->r_info);

      if (r_symndx >= NUM_SHDR_ENTRIES (symtab_hdr))
	{
	  (*_bfd_error_handler) (_("%s: bad symbol index: %d"),
				 bfd_archive_filename (abfd),
				 r_symndx);
	  return FALSE;
	}

      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	h = sym_hashes[r_symndx - symtab_hdr->sh_info];

      /* Create got section and local_got_refcounts array if they
	 are needed.  */
      r_type = elf_s390_tls_transition (info,
					ELF32_R_TYPE (rel->r_info),
					h == NULL);
      switch (r_type)
	{
	case R_390_GOT12:
	case R_390_GOT16:
	case R_390_GOT32:
	case R_390_GOTENT:
	case R_390_GOTPLT12:
	case R_390_GOTPLT16:
	case R_390_GOTPLT32:
	case R_390_GOTPLTENT:
	case R_390_TLS_GD32:
	case R_390_TLS_GOTIE12:
	case R_390_TLS_GOTIE32:
	case R_390_TLS_IEENT:
	case R_390_TLS_IE32:
	case R_390_TLS_LDM32:
	  if (h == NULL
	      && local_got_refcounts == NULL)
	    {
	      bfd_size_type size;

	      size = symtab_hdr->sh_info;
	      size *= (sizeof (bfd_signed_vma) + sizeof(char));
	      local_got_refcounts = ((bfd_signed_vma *)
				     bfd_zalloc (abfd, size));
	      if (local_got_refcounts == NULL)
		return FALSE;
	      elf_local_got_refcounts (abfd) = local_got_refcounts;
	      elf_s390_local_got_tls_type (abfd)
		= (char *) (local_got_refcounts + symtab_hdr->sh_info);
	    }
	  /* Fall through.  */
	case R_390_GOTOFF16:
	case R_390_GOTOFF32:
	case R_390_GOTPC:
	case R_390_GOTPCDBL:
	  if (htab->sgot == NULL)
	    {
	      if (htab->elf.dynobj == NULL)
		htab->elf.dynobj = abfd;
	      if (!create_got_section (htab->elf.dynobj, info))
		return FALSE;
	    }
	}

      switch (r_type)
	{
	case R_390_GOTOFF16:
	case R_390_GOTOFF32:
	case R_390_GOTPC:
	case R_390_GOTPCDBL:
	  /* Got is created, nothing to be done.  */
	  break;

	case R_390_PLT16DBL:
	case R_390_PLT32DBL:
	case R_390_PLT32:
	case R_390_PLTOFF16:
	case R_390_PLTOFF32:
	  /* This symbol requires a procedure linkage table entry.  We
	     actually build the entry in adjust_dynamic_symbol,
	     because this might be a case of linking PIC code which is
	     never referenced by a dynamic object, in which case we
	     don't need to generate a procedure linkage table entry
	     after all.  */

	  /* If this is a local symbol, we resolve it directly without
	     creating a procedure linkage table entry.  */
	  if (h != NULL)
	    {
	      h->elf_link_hash_flags |= ELF_LINK_HASH_NEEDS_PLT;
	      h->plt.refcount += 1;
	    }
	  break;

	case R_390_GOTPLT12:
	case R_390_GOTPLT16:
	case R_390_GOTPLT32:
	case R_390_GOTPLTENT:
	  /* This symbol requires either a procedure linkage table entry
	     or an entry in the local got. We actually build the entry
	     in adjust_dynamic_symbol because whether this is really a
	     global reference can change and with it the fact if we have
	     to create a plt entry or a local got entry. To be able to
	     make a once global symbol a local one we have to keep track
	     of the number of gotplt references that exist for this
	     symbol.  */
	  if (h != NULL)
	    {
	      ((struct elf_s390_link_hash_entry *) h)->gotplt_refcount++;
	      h->elf_link_hash_flags |= ELF_LINK_HASH_NEEDS_PLT;
	      h->plt.refcount += 1;
	    }
	  else
	    local_got_refcounts[r_symndx] += 1;
	  break;

	case R_390_TLS_LDM32:
	  htab->tls_ldm_got.refcount += 1;
	  break;

	case R_390_TLS_IE32:
	case R_390_TLS_GOTIE12:
	case R_390_TLS_GOTIE32:
	case R_390_TLS_IEENT:
	  if (info->shared)
	    info->flags |= DF_STATIC_TLS;
	  /* Fall through.  */

	case R_390_GOT12:
	case R_390_GOT16:
	case R_390_GOT32:
	case R_390_GOTENT:
	case R_390_TLS_GD32:
	  /* This symbol requires a global offset table entry.  */
	  switch (r_type)
	    {
	    default:
	    case R_390_GOT12:
	    case R_390_GOT16:
	    case R_390_GOT32:
	    case R_390_GOTENT:
	      tls_type = GOT_NORMAL;
	      break;
	    case R_390_TLS_GD32:
	      tls_type = GOT_TLS_GD;
	      break;
	    case R_390_TLS_IE32:
	    case R_390_TLS_GOTIE32:
	      tls_type = GOT_TLS_IE;
	      break;
	    case R_390_TLS_GOTIE12:
	    case R_390_TLS_IEENT:
	      tls_type = GOT_TLS_IE_NLT;
	      break;
	    }

	  if (h != NULL)
	    {
	      h->got.refcount += 1;
	      old_tls_type = elf_s390_hash_entry(h)->tls_type;
	    }
	  else
	    {
	      local_got_refcounts[r_symndx] += 1;
	      old_tls_type = elf_s390_local_got_tls_type (abfd) [r_symndx];
	    }
	  /* If a TLS symbol is accessed using IE at least once,
	     there is no point to use dynamic model for it.  */
	  if (old_tls_type != tls_type && old_tls_type != GOT_UNKNOWN)
	    {
	      if (old_tls_type == GOT_NORMAL || tls_type == GOT_NORMAL)
		{
		  (*_bfd_error_handler)
		    (_("%s: `%s' accessed both as normal and thread local symbol"),
		     bfd_archive_filename (abfd), h->root.root.string);
		  return FALSE;
		}
	      if (old_tls_type > tls_type)
		tls_type = old_tls_type;
	    }

	  if (old_tls_type != tls_type)
	    {
	      if (h != NULL)
		elf_s390_hash_entry (h)->tls_type = tls_type;
	      else
		elf_s390_local_got_tls_type (abfd) [r_symndx] = tls_type;
	    }

	  if (r_type != R_390_TLS_IE32)
	    break;
	  /* Fall through.  */

	case R_390_TLS_LE32:
	  if (!info->shared)
	    break;
	  info->flags |= DF_STATIC_TLS;
	  /* Fall through.  */

	case R_390_8:
	case R_390_16:
	case R_390_32:
	case R_390_PC16:
	case R_390_PC16DBL:
	case R_390_PC32DBL:
	case R_390_PC32:
	  if (h != NULL && !info->shared)
	    {
	      /* If this reloc is in a read-only section, we might
		 need a copy reloc.  We can't check reliably at this
		 stage whether the section is read-only, as input
		 sections have not yet been mapped to output sections.
		 Tentatively set the flag for now, and correct in
		 adjust_dynamic_symbol.  */
	      h->elf_link_hash_flags |= ELF_LINK_NON_GOT_REF;

	      /* We may need a .plt entry if the function this reloc
		 refers to is in a shared lib.  */
	      h->plt.refcount += 1;
	    }

	  /* If we are creating a shared library, and this is a reloc
	     against a global symbol, or a non PC relative reloc
	     against a local symbol, then we need to copy the reloc
	     into the shared library.  However, if we are linking with
	     -Bsymbolic, we do not need to copy a reloc against a
	     global symbol which is defined in an object we are
	     including in the link (i.e., DEF_REGULAR is set).  At
	     this point we have not seen all the input files, so it is
	     possible that DEF_REGULAR is not set now but will be set
	     later (it is never cleared).  In case of a weak definition,
	     DEF_REGULAR may be cleared later by a strong definition in
	     a shared library. We account for that possibility below by
	     storing information in the relocs_copied field of the hash
	     table entry.  A similar situation occurs when creating
	     shared libraries and symbol visibility changes render the
	     symbol local.

	     If on the other hand, we are creating an executable, we
	     may need to keep relocations for symbols satisfied by a
	     dynamic library if we manage to avoid copy relocs for the
	     symbol.  */
	  if ((info->shared
	       && (sec->flags & SEC_ALLOC) != 0
	       && ((ELF32_R_TYPE (rel->r_info) != R_390_PC16
		    && ELF32_R_TYPE (rel->r_info) != R_390_PC16DBL
		    && ELF32_R_TYPE (rel->r_info) != R_390_PC32DBL
		    && ELF32_R_TYPE (rel->r_info) != R_390_PC32)
		   || (h != NULL
		       && (! info->symbolic
			   || h->root.type == bfd_link_hash_defweak
			   || (h->elf_link_hash_flags
			       & ELF_LINK_HASH_DEF_REGULAR) == 0))))
	      || (ELIMINATE_COPY_RELOCS
		  && !info->shared
		  && (sec->flags & SEC_ALLOC) != 0
		  && h != NULL
		  && (h->root.type == bfd_link_hash_defweak
		      || (h->elf_link_hash_flags
			  & ELF_LINK_HASH_DEF_REGULAR) == 0)))
	    {
	      struct elf_s390_dyn_relocs *p;
	      struct elf_s390_dyn_relocs **head;

	      /* We must copy these reloc types into the output file.
		 Create a reloc section in dynobj and make room for
		 this reloc.  */
	      if (sreloc == NULL)
		{
		  const char *name;
		  bfd *dynobj;

		  name = (bfd_elf_string_from_elf_section
			  (abfd,
			   elf_elfheader (abfd)->e_shstrndx,
			   elf_section_data (sec)->rel_hdr.sh_name));
		  if (name == NULL)
		    return FALSE;

		  if (strncmp (name, ".rela", 5) != 0
		      || strcmp (bfd_get_section_name (abfd, sec),
				 name + 5) != 0)
		    {
		      (*_bfd_error_handler)
			(_("%s: bad relocation section name `%s\'"),
			 bfd_archive_filename (abfd), name);
		    }

		  if (htab->elf.dynobj == NULL)
		    htab->elf.dynobj = abfd;

		  dynobj = htab->elf.dynobj;
		  sreloc = bfd_get_section_by_name (dynobj, name);
		  if (sreloc == NULL)
		    {
		      flagword flags;

		      sreloc = bfd_make_section (dynobj, name);
		      flags = (SEC_HAS_CONTENTS | SEC_READONLY
			       | SEC_IN_MEMORY | SEC_LINKER_CREATED);
		      if ((sec->flags & SEC_ALLOC) != 0)
			flags |= SEC_ALLOC | SEC_LOAD;
		      if (sreloc == NULL
			  || ! bfd_set_section_flags (dynobj, sreloc, flags)
			  || ! bfd_set_section_alignment (dynobj, sreloc, 2))
			return FALSE;
		    }
		  elf_section_data (sec)->sreloc = sreloc;
		}

	      /* If this is a global symbol, we count the number of
		 relocations we need for this symbol.  */
	      if (h != NULL)
		{
		  head = &((struct elf_s390_link_hash_entry *) h)->dyn_relocs;
		}
	      else
		{
		  /* Track dynamic relocs needed for local syms too.
		     We really need local syms available to do this
		     easily.  Oh well.  */
		  asection *s;

		  s = bfd_section_from_r_symndx (abfd, &htab->sym_sec,
						 sec, r_symndx);
		  if (s == NULL)
		    return FALSE;

		  head = ((struct elf_s390_dyn_relocs **)
			  &elf_section_data (s)->local_dynrel);
		}

	      p = *head;
	      if (p == NULL || p->sec != sec)
		{
		  bfd_size_type amt = sizeof *p;

		  p = ((struct elf_s390_dyn_relocs *)
		       bfd_alloc (htab->elf.dynobj, amt));
		  if (p == NULL)
		    return FALSE;
		  p->next = *head;
		  *head = p;
		  p->sec = sec;
		  p->count = 0;
		  p->pc_count = 0;
		}

	      p->count += 1;
	      if (ELF32_R_TYPE (rel->r_info) == R_390_PC16
		  || ELF32_R_TYPE (rel->r_info) == R_390_PC16DBL
		  || ELF32_R_TYPE (rel->r_info) == R_390_PC32DBL
		  || ELF32_R_TYPE (rel->r_info) == R_390_PC32)
		p->pc_count += 1;
	    }
	  break;

	  /* This relocation describes the C++ object vtable hierarchy.
	     Reconstruct it for later use during GC.  */
	case R_390_GNU_VTINHERIT:
	  if (!_bfd_elf32_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
	    return FALSE;
	  break;

	  /* This relocation describes which C++ vtable entries are actually
	     used.  Record for later use during GC.  */
	case R_390_GNU_VTENTRY:
	  if (!_bfd_elf32_gc_record_vtentry (abfd, sec, h, rel->r_addend))
	    return FALSE;
	  break;

	default:
	  break;
	}
    }

  return TRUE;
}

/* Return the section that should be marked against GC for a given
   relocation.  */

static asection *
elf_s390_gc_mark_hook (sec, info, rel, h, sym)
     asection *sec;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
     Elf_Internal_Rela *rel;
     struct elf_link_hash_entry *h;
     Elf_Internal_Sym *sym;
{
  if (h != NULL)
    {
      switch (ELF32_R_TYPE (rel->r_info))
	{
	case R_390_GNU_VTINHERIT:
	case R_390_GNU_VTENTRY:
	  break;

	default:
	  switch (h->root.type)
	    {
	    case bfd_link_hash_defined:
	    case bfd_link_hash_defweak:
	      return h->root.u.def.section;

	    case bfd_link_hash_common:
	      return h->root.u.c.p->section;

	    default:
	      break;
	    }
	}
    }
  else
    return bfd_section_from_elf_index (sec->owner, sym->st_shndx);

  return NULL;
}

/* Update the got entry reference counts for the section being removed.  */

static bfd_boolean
elf_s390_gc_sweep_hook (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  bfd_signed_vma *local_got_refcounts;
  const Elf_Internal_Rela *rel, *relend;

  elf_section_data (sec)->local_dynrel = NULL;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  local_got_refcounts = elf_local_got_refcounts (abfd);

  relend = relocs + sec->reloc_count;
  for (rel = relocs; rel < relend; rel++)
    {
      unsigned long r_symndx;
      unsigned int r_type;
      struct elf_link_hash_entry *h = NULL;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx >= symtab_hdr->sh_info)
	{
	  struct elf_s390_link_hash_entry *eh;
	  struct elf_s390_dyn_relocs **pp;
	  struct elf_s390_dyn_relocs *p;

	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  eh = (struct elf_s390_link_hash_entry *) h;

	  for (pp = &eh->dyn_relocs; (p = *pp) != NULL; pp = &p->next)
	    if (p->sec == sec)
	      {
		/* Everything must go for SEC.  */
		*pp = p->next;
		break;
	      }
	}

      r_type = ELF32_R_TYPE (rel->r_info);
      r_type = elf_s390_tls_transition (info, r_type, h != NULL);
      switch (r_type)
	{
	case R_390_TLS_LDM32:
	  if (elf_s390_hash_table (info)->tls_ldm_got.refcount > 0)
	    elf_s390_hash_table (info)->tls_ldm_got.refcount -= 1;
	  break;

	case R_390_TLS_GD32:
	case R_390_TLS_IE32:
	case R_390_TLS_GOTIE12:
	case R_390_TLS_GOTIE32:
	case R_390_TLS_IEENT:
	case R_390_GOT12:
	case R_390_GOT16:
	case R_390_GOT32:
	case R_390_GOTOFF16:
	case R_390_GOTOFF32:
	case R_390_GOTPC:
	case R_390_GOTPCDBL:
	case R_390_GOTENT:
	  if (h != NULL)
	    {
	      if (h->got.refcount > 0)
		h->got.refcount -= 1;
	    }
	  else if (local_got_refcounts != NULL)
	    {
	      if (local_got_refcounts[r_symndx] > 0)
		local_got_refcounts[r_symndx] -= 1;
	    }
	  break;

	case R_390_8:
	case R_390_12:
	case R_390_16:
	case R_390_32:
	case R_390_PC16:
	case R_390_PC16DBL:
	case R_390_PC32DBL:
	case R_390_PC32:
	  if (info->shared)
	    break;
	  /* Fall through.  */

	case R_390_PLT16DBL:
	case R_390_PLT32DBL:
	case R_390_PLT32:
	case R_390_PLTOFF16:
	case R_390_PLTOFF32:
	  if (h != NULL)
	    {
	      if (h->plt.refcount > 0)
		h->plt.refcount -= 1;
	    }
	  break;

	case R_390_GOTPLT12:
	case R_390_GOTPLT16:
	case R_390_GOTPLT32:
	case R_390_GOTPLTENT:
	  if (h != NULL)
	    {
	      if (h->plt.refcount > 0)
		{
		  ((struct elf_s390_link_hash_entry *) h)->gotplt_refcount--;
		  h->plt.refcount -= 1;
		}
	    }
	  else if (local_got_refcounts != NULL)
	    {
	      if (local_got_refcounts[r_symndx] > 0)
		local_got_refcounts[r_symndx] -= 1;
	    }
	  break;

	default:
	  break;
	}
    }

  return TRUE;
}

/* Make sure we emit a GOT entry if the symbol was supposed to have a PLT
   entry but we found we will not create any.  Called when we find we will
   not have any PLT for this symbol, by for example
   elf_s390_adjust_dynamic_symbol when we're doing a proper dynamic link,
   or elf_s390_size_dynamic_sections if no dynamic sections will be
   created (we're only linking static objects).  */

static void
elf_s390_adjust_gotplt (h)
     struct elf_s390_link_hash_entry *h;
{
  if (h->elf.root.type == bfd_link_hash_warning)
    h = (struct elf_s390_link_hash_entry *) h->elf.root.u.i.link;

  if (h->gotplt_refcount <= 0)
    return;

  /* We simply add the number of gotplt references to the number
   * of got references for this symbol.  */
  h->elf.got.refcount += h->gotplt_refcount;
  h->gotplt_refcount = -1;
}

/* Adjust a symbol defined by a dynamic object and referenced by a
   regular object.  The current definition is in some section of the
   dynamic object, but we're not including those sections.  We have to
   change the definition to something the rest of the link can
   understand.  */

static bfd_boolean
elf_s390_adjust_dynamic_symbol (info, h)
     struct bfd_link_info *info;
     struct elf_link_hash_entry *h;
{
  struct elf_s390_link_hash_table *htab;
  asection *s;
  unsigned int power_of_two;

  /* If this is a function, put it in the procedure linkage table.  We
     will fill in the contents of the procedure linkage table later
     (although we could actually do it here).  */
  if (h->type == STT_FUNC
      || (h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT) != 0)
    {
      if (h->plt.refcount <= 0
	  || (! info->shared
	      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) == 0
	      && (h->elf_link_hash_flags & ELF_LINK_HASH_REF_DYNAMIC) == 0
	      && h->root.type != bfd_link_hash_undefweak
	      && h->root.type != bfd_link_hash_undefined))
	{
	  /* This case can occur if we saw a PLT32 reloc in an input
	     file, but the symbol was never referred to by a dynamic
	     object, or if all references were garbage collected.  In
	     such a case, we don't actually need to build a procedure
	     linkage table, and we can just do a PC32 reloc instead.  */
	  h->plt.offset = (bfd_vma) -1;
	  h->elf_link_hash_flags &= ~ELF_LINK_HASH_NEEDS_PLT;
	  elf_s390_adjust_gotplt((struct elf_s390_link_hash_entry *) h);
	}

      return TRUE;
    }
  else
    /* It's possible that we incorrectly decided a .plt reloc was
       needed for an R_390_PC32 reloc to a non-function sym in
       check_relocs.  We can't decide accurately between function and
       non-function syms in check-relocs;  Objects loaded later in
       the link may change h->type.  So fix it now.  */
    h->plt.offset = (bfd_vma) -1;

  /* If this is a weak symbol, and there is a real definition, the
     processor independent code will have arranged for us to see the
     real definition first, and we can just use the same value.  */
  if (h->weakdef != NULL)
    {
      BFD_ASSERT (h->weakdef->root.type == bfd_link_hash_defined
		  || h->weakdef->root.type == bfd_link_hash_defweak);
      h->root.u.def.section = h->weakdef->root.u.def.section;
      h->root.u.def.value = h->weakdef->root.u.def.value;
      if (ELIMINATE_COPY_RELOCS || info->nocopyreloc)
	h->elf_link_hash_flags
	  = ((h->elf_link_hash_flags & ~ELF_LINK_NON_GOT_REF)
	     | (h->weakdef->elf_link_hash_flags & ELF_LINK_NON_GOT_REF));
      return TRUE;
    }

  /* This is a reference to a symbol defined by a dynamic object which
     is not a function.  */

  /* If we are creating a shared library, we must presume that the
     only references to the symbol are via the global offset table.
     For such cases we need not do anything here; the relocations will
     be handled correctly by relocate_section.  */
  if (info->shared)
    return TRUE;

  /* If there are no references to this symbol that do not use the
     GOT, we don't need to generate a copy reloc.  */
  if ((h->elf_link_hash_flags & ELF_LINK_NON_GOT_REF) == 0)
    return TRUE;

  /* If -z nocopyreloc was given, we won't generate them either.  */
  if (info->nocopyreloc)
    {
      h->elf_link_hash_flags &= ~ELF_LINK_NON_GOT_REF;
      return TRUE;
    }

  if (ELIMINATE_COPY_RELOCS)
    {
      struct elf_s390_link_hash_entry * eh;
      struct elf_s390_dyn_relocs *p;

      eh = (struct elf_s390_link_hash_entry *) h;
      for (p = eh->dyn_relocs; p != NULL; p = p->next)
	{
	  s = p->sec->output_section;
	  if (s != NULL && (s->flags & SEC_READONLY) != 0)
	    break;
	}

      /* If we didn't find any dynamic relocs in read-only sections, then
	 we'll be keeping the dynamic relocs and avoiding the copy reloc.  */
      if (p == NULL)
	{
	  h->elf_link_hash_flags &= ~ELF_LINK_NON_GOT_REF;
	  return TRUE;
	}
    }

  /* We must allocate the symbol in our .dynbss section, which will
     become part of the .bss section of the executable.  There will be
     an entry for this symbol in the .dynsym section.  The dynamic
     object will contain position independent code, so all references
     from the dynamic object to this symbol will go through the global
     offset table.  The dynamic linker will use the .dynsym entry to
     determine the address it must put in the global offset table, so
     both the dynamic object and the regular object will refer to the
     same memory location for the variable.  */

  htab = elf_s390_hash_table (info);

  /* We must generate a R_390_COPY reloc to tell the dynamic linker to
     copy the initial value out of the dynamic object and into the
     runtime process image.  */
  if ((h->root.u.def.section->flags & SEC_ALLOC) != 0)
    {
      htab->srelbss->_raw_size += sizeof (Elf32_External_Rela);
      h->elf_link_hash_flags |= ELF_LINK_HASH_NEEDS_COPY;
    }

  /* We need to figure out the alignment required for this symbol.  I
     have no idea how ELF linkers handle this.  */
  power_of_two = bfd_log2 (h->size);
  if (power_of_two > 3)
    power_of_two = 3;

  /* Apply the required alignment.  */
  s = htab->sdynbss;
  s->_raw_size = BFD_ALIGN (s->_raw_size, (bfd_size_type) (1 << power_of_two));
  if (power_of_two > bfd_get_section_alignment (htab->elf.dynobj, s))
    {
      if (! bfd_set_section_alignment (htab->elf.dynobj, s, power_of_two))
	return FALSE;
    }

  /* Define the symbol as being at this point in the section.  */
  h->root.u.def.section = s;
  h->root.u.def.value = s->_raw_size;

  /* Increment the section size to make room for the symbol.  */
  s->_raw_size += h->size;

  return TRUE;
}

/* This is the condition under which elf_s390_finish_dynamic_symbol
   will be called from elflink.h.  If elflink.h doesn't call our
   finish_dynamic_symbol routine, we'll need to do something about
   initializing any .plt and .got entries in elf_s390_relocate_section.  */
#define WILL_CALL_FINISH_DYNAMIC_SYMBOL(DYN, SHARED, H) \
  ((DYN)								\
   && ((SHARED)								\
       || ((H)->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL) == 0)	\
   && ((H)->dynindx != -1						\
       || ((H)->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL) != 0))

/* Allocate space in .plt, .got and associated reloc sections for
   dynamic relocs.  */

static bfd_boolean
allocate_dynrelocs (h, inf)
     struct elf_link_hash_entry *h;
     PTR inf;
{
  struct bfd_link_info *info;
  struct elf_s390_link_hash_table *htab;
  struct elf_s390_link_hash_entry *eh;
  struct elf_s390_dyn_relocs *p;

  if (h->root.type == bfd_link_hash_indirect)
    return TRUE;

  if (h->root.type == bfd_link_hash_warning)
    /* When warning symbols are created, they **replace** the "real"
       entry in the hash table, thus we never get to see the real
       symbol in a hash traversal.  So look at it now.  */
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  info = (struct bfd_link_info *) inf;
  htab = elf_s390_hash_table (info);

  if (htab->elf.dynamic_sections_created
      && h->plt.refcount > 0
      && (ELF_ST_VISIBILITY (h->other) == STV_DEFAULT
	  || h->root.type != bfd_link_hash_undefweak))
    {
      /* Make sure this symbol is output as a dynamic symbol.
	 Undefined weak syms won't yet be marked as dynamic.  */
      if (h->dynindx == -1
	  && (h->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL) == 0)
	{
	  if (! bfd_elf32_link_record_dynamic_symbol (info, h))
	    return FALSE;
	}

      if (info->shared
	  || WILL_CALL_FINISH_DYNAMIC_SYMBOL (1, 0, h))
	{
	  asection *s = htab->splt;

	  /* If this is the first .plt entry, make room for the special
	     first entry.  */
	  if (s->_raw_size == 0)
	    s->_raw_size += PLT_FIRST_ENTRY_SIZE;

	  h->plt.offset = s->_raw_size;

	  /* If this symbol is not defined in a regular file, and we are
	     not generating a shared library, then set the symbol to this
	     location in the .plt.  This is required to make function
	     pointers compare as equal between the normal executable and
	     the shared library.  */
	  if (! info->shared
	      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
	    {
	      h->root.u.def.section = s;
	      h->root.u.def.value = h->plt.offset;
	    }

	  /* Make room for this entry.  */
	  s->_raw_size += PLT_ENTRY_SIZE;

	  /* We also need to make an entry in the .got.plt section, which
	     will be placed in the .got section by the linker script.  */
	  htab->sgotplt->_raw_size += GOT_ENTRY_SIZE;

	  /* We also need to make an entry in the .rela.plt section.  */
	  htab->srelplt->_raw_size += sizeof (Elf32_External_Rela);
	}
      else
	{
 	  h->plt.offset = (bfd_vma) -1;
	  h->elf_link_hash_flags &= ~ELF_LINK_HASH_NEEDS_PLT;
	  elf_s390_adjust_gotplt((struct elf_s390_link_hash_entry *) h);
	}
    }
  else
    {
      h->plt.offset = (bfd_vma) -1;
      h->elf_link_hash_flags &= ~ELF_LINK_HASH_NEEDS_PLT;
      elf_s390_adjust_gotplt((struct elf_s390_link_hash_entry *) h);
    }

  /* If R_390_TLS_{IE32,GOTIE32,GOTIE12,IEENT} symbol is now local to
     the binary, we can optimize a bit. IE32 and GOTIE32 get converted
     to R_390_TLS_LE32 requiring no TLS entry. For GOTIE12 and IEENT
     we can save the dynamic TLS relocation.  */
  if (h->got.refcount > 0
      && !info->shared
      && h->dynindx == -1
      && elf_s390_hash_entry(h)->tls_type >= GOT_TLS_IE)
    {
      if (elf_s390_hash_entry(h)->tls_type == GOT_TLS_IE_NLT)
	/* For the GOTIE access without a literal pool entry the offset has
	   to be stored somewhere. The immediate value in the instruction
	   is not bit enough so the value is stored in the got.  */
	{
	  h->got.offset = htab->sgot->_raw_size;
	  htab->sgot->_raw_size += GOT_ENTRY_SIZE;
	}
      else
	h->got.offset = (bfd_vma) -1;
    }
  else if (h->got.refcount > 0)
   {
      asection *s;
      bfd_boolean dyn;
      int tls_type = elf_s390_hash_entry(h)->tls_type;

      /* Make sure this symbol is output as a dynamic symbol.
	 Undefined weak syms won't yet be marked as dynamic.  */
      if (h->dynindx == -1
	  && (h->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL) == 0)
	{
	  if (! bfd_elf32_link_record_dynamic_symbol (info, h))
	    return FALSE;
	}

      s = htab->sgot;
      h->got.offset = s->_raw_size;
      s->_raw_size += GOT_ENTRY_SIZE;
      /* R_390_TLS_GD32 needs 2 consecutive GOT slots.  */
      if (tls_type == GOT_TLS_GD)
	s->_raw_size += GOT_ENTRY_SIZE;
      dyn = htab->elf.dynamic_sections_created;
      /* R_390_TLS_IE32 needs one dynamic relocation,
	 R_390_TLS_GD32 needs one if local symbol and two if global.  */
      if ((tls_type == GOT_TLS_GD && h->dynindx == -1)
	  || tls_type >= GOT_TLS_IE)
	htab->srelgot->_raw_size += sizeof (Elf32_External_Rela);
      else if (tls_type == GOT_TLS_GD)
	htab->srelgot->_raw_size += 2 * sizeof (Elf32_External_Rela);
      else if ((ELF_ST_VISIBILITY (h->other) == STV_DEFAULT
		|| h->root.type != bfd_link_hash_undefweak)
	       && (info->shared
		   || WILL_CALL_FINISH_DYNAMIC_SYMBOL (dyn, 0, h)))
	htab->srelgot->_raw_size += sizeof (Elf32_External_Rela);
    }
  else
    h->got.offset = (bfd_vma) -1;

  eh = (struct elf_s390_link_hash_entry *) h;
  if (eh->dyn_relocs == NULL)
    return TRUE;

  /* In the shared -Bsymbolic case, discard space allocated for
     dynamic pc-relative relocs against symbols which turn out to be
     defined in regular objects.  For the normal shared case, discard
     space for pc-relative relocs that have become local due to symbol
     visibility changes.  */

  if (info->shared)
    {
      if ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) != 0
	  && ((h->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL) != 0
	      || info->symbolic))
	{
	  struct elf_s390_dyn_relocs **pp;

	  for (pp = &eh->dyn_relocs; (p = *pp) != NULL; )
	    {
	      p->count -= p->pc_count;
	      p->pc_count = 0;
	      if (p->count == 0)
		*pp = p->next;
	      else
		pp = &p->next;
	    }
	}

      /* Also discard relocs on undefined weak syms with non-default
	 visibility.  */
      if (ELF_ST_VISIBILITY (h->other) != STV_DEFAULT
	  && h->root.type == bfd_link_hash_undefweak)
	eh->dyn_relocs = NULL;
    }
  else if (ELIMINATE_COPY_RELOCS)
    {
      /* For the non-shared case, discard space for relocs against
	 symbols which turn out to need copy relocs or are not
	 dynamic.  */

      if ((h->elf_link_hash_flags & ELF_LINK_NON_GOT_REF) == 0
	  && (((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) != 0
	       && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
	      || (htab->elf.dynamic_sections_created
		  && (h->root.type == bfd_link_hash_undefweak
		      || h->root.type == bfd_link_hash_undefined))))
	{
	  /* Make sure this symbol is output as a dynamic symbol.
	     Undefined weak syms won't yet be marked as dynamic.  */
	  if (h->dynindx == -1
	      && (h->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL) == 0)
	    {
	      if (! bfd_elf32_link_record_dynamic_symbol (info, h))
		return FALSE;
	    }

	  /* If that succeeded, we know we'll be keeping all the
	     relocs.  */
	  if (h->dynindx != -1)
	    goto keep;
	}

      eh->dyn_relocs = NULL;

    keep: ;
    }

  /* Finally, allocate space.  */
  for (p = eh->dyn_relocs; p != NULL; p = p->next)
    {
      asection *sreloc = elf_section_data (p->sec)->sreloc;

      sreloc->_raw_size += p->count * sizeof (Elf32_External_Rela);
    }

  return TRUE;
}

/* Find any dynamic relocs that apply to read-only sections.  */

static bfd_boolean
readonly_dynrelocs (h, inf)
     struct elf_link_hash_entry *h;
     PTR inf;
{
  struct elf_s390_link_hash_entry *eh;
  struct elf_s390_dyn_relocs *p;

  if (h->root.type == bfd_link_hash_warning)
    h = (struct elf_link_hash_entry *) h->root.u.i.link;

  eh = (struct elf_s390_link_hash_entry *) h;
  for (p = eh->dyn_relocs; p != NULL; p = p->next)
    {
      asection *s = p->sec->output_section;

      if (s != NULL && (s->flags & SEC_READONLY) != 0)
	{
	  struct bfd_link_info *info = (struct bfd_link_info *) inf;

	  info->flags |= DF_TEXTREL;

	  /* Not an error, just cut short the traversal.  */
	  return FALSE;
	}
    }
  return TRUE;
}

/* Set the sizes of the dynamic sections.  */

static bfd_boolean
elf_s390_size_dynamic_sections (output_bfd, info)
     bfd *output_bfd ATTRIBUTE_UNUSED;
     struct bfd_link_info *info;
{
  struct elf_s390_link_hash_table *htab;
  bfd *dynobj;
  asection *s;
  bfd_boolean relocs;
  bfd *ibfd;

  htab = elf_s390_hash_table (info);
  dynobj = htab->elf.dynobj;
  if (dynobj == NULL)
    abort ();

  if (htab->elf.dynamic_sections_created)
    {
      /* Set the contents of the .interp section to the interpreter.  */
      if (info->executable)
	{
	  s = bfd_get_section_by_name (dynobj, ".interp");
	  if (s == NULL)
	    abort ();
	  s->_raw_size = sizeof ELF_DYNAMIC_INTERPRETER;
	  s->contents = (unsigned char *) ELF_DYNAMIC_INTERPRETER;
	}
    }

  /* Set up .got offsets for local syms, and space for local dynamic
     relocs.  */
  for (ibfd = info->input_bfds; ibfd != NULL; ibfd = ibfd->link_next)
    {
      bfd_signed_vma *local_got;
      bfd_signed_vma *end_local_got;
      char *local_tls_type;
      bfd_size_type locsymcount;
      Elf_Internal_Shdr *symtab_hdr;
      asection *srela;

      if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour)
	continue;

      for (s = ibfd->sections; s != NULL; s = s->next)
	{
	  struct elf_s390_dyn_relocs *p;

	  for (p = *((struct elf_s390_dyn_relocs **)
		     &elf_section_data (s)->local_dynrel);
	       p != NULL;
	       p = p->next)
	    {
	      if (!bfd_is_abs_section (p->sec)
		  && bfd_is_abs_section (p->sec->output_section))
		{
		  /* Input section has been discarded, either because
		     it is a copy of a linkonce section or due to
		     linker script /DISCARD/, so we'll be discarding
		     the relocs too.  */
		}
	      else if (p->count != 0)
		{
		  srela = elf_section_data (p->sec)->sreloc;
		  srela->_raw_size += p->count * sizeof (Elf32_External_Rela);
		  if ((p->sec->output_section->flags & SEC_READONLY) != 0)
		    info->flags |= DF_TEXTREL;
		}
	    }
	}

      local_got = elf_local_got_refcounts (ibfd);
      if (!local_got)
	continue;

      symtab_hdr = &elf_tdata (ibfd)->symtab_hdr;
      locsymcount = symtab_hdr->sh_info;
      end_local_got = local_got + locsymcount;
      local_tls_type = elf_s390_local_got_tls_type (ibfd);
      s = htab->sgot;
      srela = htab->srelgot;
      for (; local_got < end_local_got; ++local_got, ++local_tls_type)
	{
	  if (*local_got > 0)
	    {
	      *local_got = s->_raw_size;
	      s->_raw_size += GOT_ENTRY_SIZE;
	      if (*local_tls_type == GOT_TLS_GD)
		s->_raw_size += GOT_ENTRY_SIZE;
	      if (info->shared)
		srela->_raw_size += sizeof (Elf32_External_Rela);
	    }
	  else
	    *local_got = (bfd_vma) -1;
	}
    }

  if (htab->tls_ldm_got.refcount > 0)
    {
      /* Allocate 2 got entries and 1 dynamic reloc for R_390_TLS_LDM32
	 relocs.  */
      htab->tls_ldm_got.offset = htab->sgot->_raw_size;
      htab->sgot->_raw_size += 2 * GOT_ENTRY_SIZE;
      htab->srelgot->_raw_size += sizeof (Elf32_External_Rela);
    }
  else
    htab->tls_ldm_got.offset = -1;

  /* Allocate global sym .plt and .got entries, and space for global
     sym dynamic relocs.  */
  elf_link_hash_traverse (&htab->elf, allocate_dynrelocs, (PTR) info);

  /* We now have determined the sizes of the various dynamic sections.
     Allocate memory for them.  */
  relocs = FALSE;
  for (s = dynobj->sections; s != NULL; s = s->next)
    {
      if ((s->flags & SEC_LINKER_CREATED) == 0)
	continue;

      if (s == htab->splt
	  || s == htab->sgot
	  || s == htab->sgotplt)
	{
	  /* Strip this section if we don't need it; see the
	     comment below.  */
	}
      else if (strncmp (bfd_get_section_name (dynobj, s), ".rela", 5) == 0)
	{
	  if (s->_raw_size != 0)
	    relocs = TRUE;

	  /* We use the reloc_count field as a counter if we need
	     to copy relocs into the output file.  */
	  s->reloc_count = 0;
	}
      else
	{
	  /* It's not one of our sections, so don't allocate space.  */
	  continue;
	}

      if (s->_raw_size == 0)
	{
	  /* If we don't need this section, strip it from the
	     output file.  This is to handle .rela.bss and
	     .rela.plt.  We must create it in
	     create_dynamic_sections, because it must be created
	     before the linker maps input sections to output
	     sections.  The linker does that before
	     adjust_dynamic_symbol is called, and it is that
	     function which decides whether anything needs to go
	     into these sections.  */

	  _bfd_strip_section_from_output (info, s);
	  continue;
	}

      /* Allocate memory for the section contents.  We use bfd_zalloc
	 here in case unused entries are not reclaimed before the
	 section's contents are written out.  This should not happen,
	 but this way if it does, we get a R_390_NONE reloc instead
	 of garbage.  */
      s->contents = (bfd_byte *) bfd_zalloc (dynobj, s->_raw_size);
      if (s->contents == NULL)
	return FALSE;
    }

  if (htab->elf.dynamic_sections_created)
    {
      /* Add some entries to the .dynamic section.  We fill in the
	 values later, in elf_s390_finish_dynamic_sections, but we
	 must add the entries now so that we get the correct size for
	 the .dynamic section.  The DT_DEBUG entry is filled in by the
	 dynamic linker and used by the debugger.  */
#define add_dynamic_entry(TAG, VAL) \
  bfd_elf32_add_dynamic_entry (info, (bfd_vma) (TAG), (bfd_vma) (VAL))

      if (info->executable)
	{
	  if (!add_dynamic_entry (DT_DEBUG, 0))
	    return FALSE;
	}

      if (htab->splt->_raw_size != 0)
	{
	  if (!add_dynamic_entry (DT_PLTGOT, 0)
	      || !add_dynamic_entry (DT_PLTRELSZ, 0)
	      || !add_dynamic_entry (DT_PLTREL, DT_RELA)
	      || !add_dynamic_entry (DT_JMPREL, 0))
	    return FALSE;
	}

      if (relocs)
	{
	  if (!add_dynamic_entry (DT_RELA, 0)
	      || !add_dynamic_entry (DT_RELASZ, 0)
	      || !add_dynamic_entry (DT_RELAENT, sizeof (Elf32_External_Rela)))
	    return FALSE;

	  /* If any dynamic relocs apply to a read-only section,
	     then we need a DT_TEXTREL entry.  */
	  if ((info->flags & DF_TEXTREL) == 0)
	    elf_link_hash_traverse (&htab->elf, readonly_dynrelocs,
				    (PTR) info);

	  if ((info->flags & DF_TEXTREL) != 0)
	    {
	      if (!add_dynamic_entry (DT_TEXTREL, 0))
		return FALSE;
	    }
	}
    }
#undef add_dynamic_entry

  return TRUE;
}

/* Return the base VMA address which should be subtracted from real addresses
   when resolving @dtpoff relocation.
   This is PT_TLS segment p_vaddr.  */

static bfd_vma
dtpoff_base (info)
     struct bfd_link_info *info;
{
  /* If tls_segment is NULL, we should have signalled an error already.  */
  if (elf_hash_table (info)->tls_segment == NULL)
    return 0;
  return elf_hash_table (info)->tls_segment->start;
}

/* Return the relocation value for @tpoff relocation
   if STT_TLS virtual address is ADDRESS.  */

static bfd_vma
tpoff (info, address)
     struct bfd_link_info *info;
     bfd_vma address;
{
  struct elf_link_tls_segment *tls_segment
    = elf_hash_table (info)->tls_segment;

  /* If tls_segment is NULL, we should have signalled an error already.  */
  if (tls_segment == NULL)
    return 0;
  return (align_power (tls_segment->size, tls_segment->align)
	  + tls_segment->start - address);
}

/* Complain if TLS instruction relocation is against an invalid
   instruction.  */

static void
invalid_tls_insn (input_bfd, input_section, rel)
     bfd *input_bfd;
     asection *input_section;
     Elf_Internal_Rela *rel;
{
  reloc_howto_type *howto;

  howto = elf_howto_table + ELF32_R_TYPE (rel->r_info);
  (*_bfd_error_handler)
    (_("%s(%s+0x%lx): invalid instruction for TLS relocation %s"),
     bfd_archive_filename (input_bfd),
     bfd_get_section_name (input_bfd, input_section),
     (long) rel->r_offset,
     howto->name);
}

/* Relocate a 390 ELF section.  */

static bfd_boolean
elf_s390_relocate_section (output_bfd, info, input_bfd, input_section,
			      contents, relocs, local_syms, local_sections)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *relocs;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
{
  struct elf_s390_link_hash_table *htab;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  bfd_vma *local_got_offsets;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  if (info->relocatable)
    return TRUE;

  htab = elf_s390_hash_table (info);
  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  local_got_offsets = elf_local_got_offsets (input_bfd);

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      unsigned int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;
      Elf_Internal_Sym *sym;
      asection *sec;
      bfd_vma off;
      bfd_vma relocation;
      bfd_boolean unresolved_reloc;
      bfd_reloc_status_type r;
      int tls_type;

      r_type = ELF32_R_TYPE (rel->r_info);
      if (r_type == (int) R_390_GNU_VTINHERIT
	  || r_type == (int) R_390_GNU_VTENTRY)
	continue;
      if (r_type >= (int) R_390_max)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return FALSE;
	}

      howto = elf_howto_table + r_type;
      r_symndx = ELF32_R_SYM (rel->r_info);

      /* This is a final link.  */
      h = NULL;
      sym = NULL;
      sec = NULL;
      unresolved_reloc = FALSE;
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, sec, rel);
	}
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;

	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
	      if (sec->output_section == NULL)
		{
		  /* Set a flag that will be cleared later if we find a
		     relocation value for this symbol.  output_section
		     is typically NULL for symbols satisfied by a shared
		     library.  */
		  unresolved_reloc = TRUE;
		  relocation = 0;
		}
	      else
		relocation = (h->root.u.def.value
			      + sec->output_section->vma
			      + sec->output_offset);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
	  else if (info->shared
		   && !info->no_undefined
		   && ELF_ST_VISIBILITY (h->other) == STV_DEFAULT)
	    relocation = 0;
	  else
	    {
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, h->root.root.string, input_bfd,
		      input_section, rel->r_offset,
		      (!info->shared || info->no_undefined
		       || ELF_ST_VISIBILITY (h->other)))))
		return FALSE;
	      relocation = 0;
	    }
	}

      switch (r_type)
	{
	case R_390_GOTPLT12:
	case R_390_GOTPLT16:
	case R_390_GOTPLT32:
	case R_390_GOTPLTENT:
	  /* There are three cases for a GOTPLT relocation. 1) The
	     relocation is against the jump slot entry of a plt that
	     will get emitted to the output file. 2) The relocation
	     is against the jump slot of a plt entry that has been
	     removed. elf_s390_adjust_gotplt has created a GOT entry
	     as replacement. 3) The relocation is against a local symbol.
	     Cases 2) and 3) are the same as the GOT relocation code
	     so we just have to test for case 1 and fall through for
	     the other two.  */
	  if (h != NULL && h->plt.offset != (bfd_vma) -1)
	    {
	      bfd_vma plt_index;

	      /* Calc. index no.
		 Current offset - size first entry / entry size.  */
	      plt_index = (h->plt.offset - PLT_FIRST_ENTRY_SIZE) /
		PLT_ENTRY_SIZE;

	      /* Offset in GOT is PLT index plus GOT headers(3) times 4,
		 addr & GOT addr.  */
	      relocation = (plt_index + 3) * GOT_ENTRY_SIZE;
	      unresolved_reloc = FALSE;

	      if (r_type == R_390_GOTPLTENT)
		relocation += htab->sgot->output_section->vma;
	      break;
	    }
	  /* Fall through.  */

	case R_390_GOT12:
	case R_390_GOT16:
	case R_390_GOT32:
	case R_390_GOTENT:
	  /* Relocation is to the entry for this symbol in the global
	     offset table.  */
	  if (htab->sgot == NULL)
	    abort ();

	  if (h != NULL)
	    {
	      bfd_boolean dyn;

	      off = h->got.offset;
	      dyn = htab->elf.dynamic_sections_created;
	      if (! WILL_CALL_FINISH_DYNAMIC_SYMBOL (dyn, info->shared, h)
		  || (info->shared
		      && (info->symbolic
			  || h->dynindx == -1
			  || (h->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL))
		      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR))
		  || (ELF_ST_VISIBILITY (h->other)
		      && h->root.type == bfd_link_hash_undefweak))
		{
		  /* This is actually a static link, or it is a
		     -Bsymbolic link and the symbol is defined
		     locally, or the symbol was forced to be local
		     because of a version file.  We must initialize
		     this entry in the global offset table.  Since the
		     offset must always be a multiple of 2, we use the
		     least significant bit to record whether we have
		     initialized it already.

		     When doing a dynamic link, we create a .rel.got
		     relocation entry to initialize the value.  This
		     is done in the finish_dynamic_symbol routine.  */
		  if ((off & 1) != 0)
		    off &= ~1;
		  else
		    {
		      bfd_put_32 (output_bfd, relocation,
				  htab->sgot->contents + off);
		      h->got.offset |= 1;
		    }
		}
	      else
		unresolved_reloc = FALSE;
	    }
	  else
	    {
	      if (local_got_offsets == NULL)
		abort ();

	      off = local_got_offsets[r_symndx];

	      /* The offset must always be a multiple of 4.  We use
		 the least significant bit to record whether we have
		 already generated the necessary reloc.  */
	      if ((off & 1) != 0)
		off &= ~1;
	      else
		{
		  bfd_put_32 (output_bfd, relocation,
			      htab->sgot->contents + off);

		  if (info->shared)
		    {
		      asection *srelgot;
		      Elf_Internal_Rela outrel;
		      bfd_byte *loc;

		      srelgot = htab->srelgot;
		      if (srelgot == NULL)
			abort ();

		      outrel.r_offset = (htab->sgot->output_section->vma
					 + htab->sgot->output_offset
					 + off);
		      outrel.r_info = ELF32_R_INFO (0, R_390_RELATIVE);
		      outrel.r_addend = relocation;
		      loc = srelgot->contents;
		      loc += srelgot->reloc_count++ * sizeof (Elf32_External_Rela);
		      bfd_elf32_swap_reloca_out (output_bfd, &outrel, loc);
		    }

		  local_got_offsets[r_symndx] |= 1;
		}
	    }

	  if (off >= (bfd_vma) -2)
	    abort ();

	  relocation = htab->sgot->output_offset + off;

	  /* For @GOTENT the relocation is against the offset between
	     the instruction and the symbols entry in the GOT and not
	     between the start of the GOT and the symbols entry. We
	     add the vma of the GOT to get the correct value.  */
	  if (   r_type == R_390_GOTENT
	      || r_type == R_390_GOTPLTENT)
	    relocation += htab->sgot->output_section->vma;

	  break;

	case R_390_GOTOFF16:
	case R_390_GOTOFF32:
	  /* Relocation is relative to the start of the global offset
	     table.  */

	  /* Note that sgot->output_offset is not involved in this
	     calculation.  We always want the start of .got.  If we
	     defined _GLOBAL_OFFSET_TABLE in a different way, as is
	     permitted by the ABI, we might have to change this
	     calculation.  */
	  relocation -= htab->sgot->output_section->vma;
	  break;

	case R_390_GOTPC:
	case R_390_GOTPCDBL:
	  /* Use global offset table as symbol value.  */
	  relocation = htab->sgot->output_section->vma;
	  unresolved_reloc = FALSE;
	  break;

	case R_390_PLT16DBL:
	case R_390_PLT32DBL:
	case R_390_PLT32:
	  /* Relocation is to the entry for this symbol in the
	     procedure linkage table.  */

	  /* Resolve a PLT32 reloc against a local symbol directly,
	     without using the procedure linkage table.  */
	  if (h == NULL)
	    break;

	  if (h->plt.offset == (bfd_vma) -1
	      || htab->splt == NULL)
	    {
	      /* We didn't make a PLT entry for this symbol.  This
		 happens when statically linking PIC code, or when
		 using -Bsymbolic.  */
	      break;
	    }

	  relocation = (htab->splt->output_section->vma
			+ htab->splt->output_offset
			+ h->plt.offset);
	  unresolved_reloc = FALSE;
	  break;

	case R_390_PLTOFF16:
	case R_390_PLTOFF32:
	  /* Relocation is to the entry for this symbol in the
	     procedure linkage table relative to the start of the GOT.  */

	  /* For local symbols or if we didn't make a PLT entry for
	     this symbol resolve the symbol directly.  */
	  if (   h == NULL
	      || h->plt.offset == (bfd_vma) -1
	      || htab->splt == NULL)
	    {
	      relocation -= htab->sgot->output_section->vma;
	      break;
	    }

	  relocation = (htab->splt->output_section->vma
			+ htab->splt->output_offset
			+ h->plt.offset
			- htab->sgot->output_section->vma);
	  unresolved_reloc = FALSE;
	  break;

	case R_390_8:
	case R_390_16:
	case R_390_32:
	case R_390_PC16:
	case R_390_PC16DBL:
	case R_390_PC32DBL:
	case R_390_PC32:
	  /* r_symndx will be zero only for relocs against symbols
	     from removed linkonce sections, or sections discarded by
	     a linker script.  */
	  if (r_symndx == 0
	      || (input_section->flags & SEC_ALLOC) == 0)
	    break;

	  if ((info->shared
	       && (h == NULL
		   || ELF_ST_VISIBILITY (h->other) == STV_DEFAULT
		   || h->root.type != bfd_link_hash_undefweak)
	       && ((r_type != R_390_PC16
		    && r_type != R_390_PC16DBL
		    && r_type != R_390_PC32DBL
		    && r_type != R_390_PC32)
		   || (h != NULL
		       && h->dynindx != -1
		       && (! info->symbolic
			   || (h->elf_link_hash_flags
			       & ELF_LINK_HASH_DEF_REGULAR) == 0))))
	      || (ELIMINATE_COPY_RELOCS
		  && !info->shared
		  && h != NULL
		  && h->dynindx != -1
		  && (h->elf_link_hash_flags & ELF_LINK_NON_GOT_REF) == 0
		  && (((h->elf_link_hash_flags
		       & ELF_LINK_HASH_DEF_DYNAMIC) != 0
		       && (h->elf_link_hash_flags
			   & ELF_LINK_HASH_DEF_REGULAR) == 0)
		      || h->root.type == bfd_link_hash_undefweak
		      || h->root.type == bfd_link_hash_undefined)))
	    {
	      Elf_Internal_Rela outrel;
	      bfd_boolean skip, relocate;
	      asection *sreloc;
	      bfd_byte *loc;

	      /* When generating a shared object, these relocations
		 are copied into the output file to be resolved at run
		 time.  */

	      skip = FALSE;
	      relocate = FALSE;

	      outrel.r_offset =
		_bfd_elf_section_offset (output_bfd, info, input_section,
					 rel->r_offset);
	      if (outrel.r_offset == (bfd_vma) -1)
		skip = TRUE;
	      else if (outrel.r_offset == (bfd_vma) -2)
		skip = TRUE, relocate = TRUE;
	      outrel.r_offset += (input_section->output_section->vma
				  + input_section->output_offset);

	      if (skip)
		memset (&outrel, 0, sizeof outrel);
	      else if (h != NULL
		       && h->dynindx != -1
		       && (r_type == R_390_PC16
			   || r_type == R_390_PC16DBL
			   || r_type == R_390_PC32DBL
			   || r_type == R_390_PC32
			   || !info->shared
			   || !info->symbolic
			   || (h->elf_link_hash_flags
			       & ELF_LINK_HASH_DEF_REGULAR) == 0))
		{
		  outrel.r_info = ELF32_R_INFO (h->dynindx, r_type);
		  outrel.r_addend = rel->r_addend;
		}
	      else
		{
		  /* This symbol is local, or marked to become local.  */
		  relocate = TRUE;
		  outrel.r_info = ELF32_R_INFO (0, R_390_RELATIVE);
		  outrel.r_addend = relocation + rel->r_addend;
		}

	      sreloc = elf_section_data (input_section)->sreloc;
	      if (sreloc == NULL)
		abort ();

	      loc = sreloc->contents;
	      loc += sreloc->reloc_count++ * sizeof (Elf32_External_Rela);
	      bfd_elf32_swap_reloca_out (output_bfd, &outrel, loc);

	      /* If this reloc is against an external symbol, we do
		 not want to fiddle with the addend.  Otherwise, we
		 need to include the symbol value so that it becomes
		 an addend for the dynamic reloc.  */
	      if (! relocate)
		continue;
	    }
	  break;

	  /* Relocations for tls literal pool entries.  */
	case R_390_TLS_IE32:
	  if (info->shared)
	    {
	      Elf_Internal_Rela outrel;
	      asection *sreloc;
	      bfd_byte *loc;

	      outrel.r_offset = rel->r_offset
				+ input_section->output_section->vma
				+ input_section->output_offset;
	      outrel.r_info = ELF32_R_INFO (0, R_390_RELATIVE);
	      sreloc = elf_section_data (input_section)->sreloc;
	      if (sreloc == NULL)
		abort ();
	      loc = sreloc->contents;
	      loc += sreloc->reloc_count++ * sizeof (Elf32_External_Rela);
	      bfd_elf32_swap_reloc_out (output_bfd, &outrel, loc);
	    }
	  /* Fall through.  */

	case R_390_TLS_GD32:
	case R_390_TLS_GOTIE32:
	  r_type = elf_s390_tls_transition (info, r_type, h == NULL);
	  tls_type = GOT_UNKNOWN;
	  if (h == NULL && local_got_offsets)
	    tls_type = elf_s390_local_got_tls_type (input_bfd) [r_symndx];
	  else if (h != NULL)
	    {
	      tls_type = elf_s390_hash_entry(h)->tls_type;
	      if (!info->shared && h->dynindx == -1 && tls_type >= GOT_TLS_IE)
		r_type = R_390_TLS_LE32;
	    }
	  if (r_type == R_390_TLS_GD32 && tls_type >= GOT_TLS_IE)
	    r_type = R_390_TLS_IE32;

	  if (r_type == R_390_TLS_LE32)
	    {
	      /* This relocation gets optimized away by the local exec
		 access optimization.  */
	      BFD_ASSERT (! unresolved_reloc);
	      bfd_put_32 (output_bfd, -tpoff (info, relocation),
			  contents + rel->r_offset);
	      continue;
	    }

	  if (htab->sgot == NULL)
	    abort ();

	  if (h != NULL)
	    off = h->got.offset;
	  else
	    {
	      if (local_got_offsets == NULL)
		abort ();

	      off = local_got_offsets[r_symndx];
	    }

	emit_tls_relocs:

	  if ((off & 1) != 0)
	    off &= ~1;
	  else
	    {
	      Elf_Internal_Rela outrel;
	      bfd_byte *loc;
	      int dr_type, indx;

	      if (htab->srelgot == NULL)
		abort ();

	      outrel.r_offset = (htab->sgot->output_section->vma
				 + htab->sgot->output_offset + off);

	      indx = h && h->dynindx != -1 ? h->dynindx : 0;
	      if (r_type == R_390_TLS_GD32)
		dr_type = R_390_TLS_DTPMOD;
	      else
		dr_type = R_390_TLS_TPOFF;
	      if (dr_type == R_390_TLS_TPOFF && indx == 0)
		outrel.r_addend = relocation - dtpoff_base (info);
	      else
		outrel.r_addend = 0;
	      outrel.r_info = ELF32_R_INFO (indx, dr_type);
	      loc = htab->srelgot->contents;
	      loc += htab->srelgot->reloc_count++
		* sizeof (Elf32_External_Rela);
	      bfd_elf32_swap_reloca_out (output_bfd, &outrel, loc);

	      if (r_type == R_390_TLS_GD32)
		{
		  if (indx == 0)
		    {
		      BFD_ASSERT (! unresolved_reloc);
		      bfd_put_32 (output_bfd,
				  relocation - dtpoff_base (info),
				  htab->sgot->contents + off + GOT_ENTRY_SIZE);
		    }
		  else
		    {
		      outrel.r_info = ELF32_R_INFO (indx, R_390_TLS_DTPOFF);
		      outrel.r_offset += GOT_ENTRY_SIZE;
		      outrel.r_addend = 0;
		      htab->srelgot->reloc_count++;
		      loc += sizeof (Elf32_External_Rela);
		      bfd_elf32_swap_reloca_out (output_bfd, &outrel, loc);
		    }
		}

	      if (h != NULL)
		h->got.offset |= 1;
	      else
		local_got_offsets[r_symndx] |= 1;
	    }

	  if (off >= (bfd_vma) -2)
	    abort ();
	  if (r_type == ELF32_R_TYPE (rel->r_info))
	    {
	      relocation = htab->sgot->output_offset + off;
	      if (r_type == R_390_TLS_IE32 || r_type == R_390_TLS_IEENT)
		relocation += htab->sgot->output_section->vma;
	      unresolved_reloc = FALSE;
	    }
	  else
	    {
	      bfd_put_32 (output_bfd, htab->sgot->output_offset + off,
			  contents + rel->r_offset);
	      continue;
	    }
	  break;

	case R_390_TLS_GOTIE12:
	case R_390_TLS_IEENT:
	  if (h == NULL)
	    {
	      if (local_got_offsets == NULL)
		abort();
	      off = local_got_offsets[r_symndx];
	      if (info->shared)
		goto emit_tls_relocs;
	    }
	  else
	    {
	      off = h->got.offset;
	      tls_type = elf_s390_hash_entry(h)->tls_type;
	      if (info->shared || h->dynindx != -1 || tls_type < GOT_TLS_IE)
		goto emit_tls_relocs;
	    }

	  if (htab->sgot == NULL)
	    abort ();

	  BFD_ASSERT (! unresolved_reloc);
	  bfd_put_32 (output_bfd, -tpoff (info, relocation),
		      htab->sgot->contents + off);
	  relocation = htab->sgot->output_offset + off;
	  if (r_type == R_390_TLS_IEENT)
	    relocation += htab->sgot->output_section->vma;
	  unresolved_reloc = FALSE;
	  break;

	case R_390_TLS_LDM32:
	  if (! info->shared)
	    /* The literal pool entry this relocation refers to gets ignored
	       by the optimized code of the local exec model. Do nothing
	       and the value will turn out zero.  */
	    continue;

	  if (htab->sgot == NULL)
	    abort ();

	  off = htab->tls_ldm_got.offset;
	  if (off & 1)
	    off &= ~1;
	  else
	    {
	      Elf_Internal_Rela outrel;
	      bfd_byte *loc;

	      if (htab->srelgot == NULL)
		abort ();

	      outrel.r_offset = (htab->sgot->output_section->vma
				 + htab->sgot->output_offset + off);

	      bfd_put_32 (output_bfd, 0,
			  htab->sgot->contents + off + GOT_ENTRY_SIZE);
	      outrel.r_info = ELF32_R_INFO (0, R_390_TLS_DTPMOD);
	      outrel.r_addend = 0;
	      loc = htab->srelgot->contents;
	      loc += htab->srelgot->reloc_count++
		* sizeof (Elf32_External_Rela);
	      bfd_elf32_swap_reloca_out (output_bfd, &outrel, loc);
	      htab->tls_ldm_got.offset |= 1;
	    }
	  relocation = htab->sgot->output_offset + off;
	  unresolved_reloc = FALSE;
	  break;

	case R_390_TLS_LE32:
	  if (info->shared)
	    {
	      /* Linking a shared library with non-fpic code requires
		 a R_390_TLS_TPOFF relocation.  */
	      Elf_Internal_Rela outrel;
	      asection *sreloc;
	      bfd_byte *loc;
	      int indx;

	      outrel.r_offset = rel->r_offset
				+ input_section->output_section->vma
				+ input_section->output_offset;
	      if (h != NULL && h->dynindx != -1)
		indx = h->dynindx;
	      else
		indx = 0;
	      outrel.r_info = ELF32_R_INFO (indx, R_390_TLS_TPOFF);
	      if (indx == 0)
		outrel.r_addend = relocation - dtpoff_base (info);
	      else
		outrel.r_addend = 0;
	      sreloc = elf_section_data (input_section)->sreloc;
	      if (sreloc == NULL)
		abort ();
	      loc = sreloc->contents;
	      loc += sreloc->reloc_count++ * sizeof (Elf32_External_Rela);
	      bfd_elf32_swap_reloca_out (output_bfd, &outrel, loc);
	    }
	  else
	    {
	      BFD_ASSERT (! unresolved_reloc);
	      bfd_put_32 (output_bfd, -tpoff (info, relocation),
			  contents + rel->r_offset);
	    }
	  continue;

	case R_390_TLS_LDO32:
	  if (info->shared || (input_section->flags & SEC_CODE) == 0)
	    relocation -= dtpoff_base (info);
	  else
	    /* When converting LDO to LE, we must negate.  */
	    relocation = -tpoff (info, relocation);
	  break;

	  /* Relocations for tls instructions.  */
	case R_390_TLS_LOAD:
	case R_390_TLS_GDCALL:
	case R_390_TLS_LDCALL:
	  tls_type = GOT_UNKNOWN;
	  if (h == NULL && local_got_offsets)
	    tls_type = elf_s390_local_got_tls_type (input_bfd) [r_symndx];
	  else if (h != NULL)
	    tls_type = elf_s390_hash_entry(h)->tls_type;

	  if (tls_type == GOT_TLS_GD)
	    continue;

	  if (r_type == R_390_TLS_LOAD)
	    {
	      if (!info->shared && (h == NULL || h->dynindx == -1))
		{
		  /* IE->LE transition. Four valid cases:
		     l %rx,0(0,%ry)    -> lr %rx,%ry + bcr 0,0
		     l %rx,0(%ry,0)    -> lr %rx,%ry + bcr 0,0
		     l %rx,0(%ry,%r12) -> lr %rx,%ry + bcr 0,0
		     l %rx,0(%r12,%ry) -> lr %rx,%ry + bcr 0,0 */
		  unsigned int insn, ry;

		  insn = bfd_get_32 (input_bfd, contents + rel->r_offset);
		  ry = 0;
		  if ((insn & 0xff00f000) == 0x58000000)
		    /* l %rx,0(%ry,0) -> lr %rx,%ry + bcr 0,0  */
		    ry = (insn & 0x000f0000);
		  else if ((insn & 0xff0f0000) == 0x58000000)
		    /* l %rx,0(0,%ry) -> lr %rx,%ry + bcr 0,0  */
		    ry = (insn & 0x0000f000) << 4;
		  else if ((insn & 0xff00f000) == 0x5800c000)
		    /* l %rx,0(%ry,%r12) -> lr %rx,%ry + bcr 0,0  */
		    ry = (insn & 0x000f0000);
		  else if ((insn & 0xff0f0000) == 0x580c0000)
		    /* l %rx,0(%r12,%ry) -> lr %rx,%ry + bcr 0,0  */
		    ry = (insn & 0x0000f000) << 4;
		  else
		    invalid_tls_insn (input_bfd, input_section, rel);
		  insn = 0x18000700 | (insn & 0x00f00000) | ry;
		  bfd_put_32 (output_bfd, insn, contents + rel->r_offset);
		}
	    }
	  else if (r_type == R_390_TLS_GDCALL)
	    {
	      unsigned int insn;

	      insn = bfd_get_32 (input_bfd, contents + rel->r_offset);
	      if ((insn & 0xff000fff) != 0x4d000000)
		invalid_tls_insn (input_bfd, input_section, rel);
	      if (!info->shared && (h == NULL || h->dynindx == -1))
		/* GD->LE transition.
		   bas %r14,0(%rx,%r13) -> bc 0,0  */
		insn = 0x47000000;
	      else
		/* GD->IE transition.
		   bas %r14,0(%rx,%r13) -> l %r2,0(%r2,%r12)  */
		insn = 0x5822c000;
	      bfd_put_32 (output_bfd, insn, contents + rel->r_offset);
	    }
	  else if (r_type == R_390_TLS_LDCALL)
	    {
	      if (!info->shared)
		{
		  unsigned int insn;

		  insn = bfd_get_32 (input_bfd, contents + rel->r_offset);
		  if ((insn & 0xff000fff) != 0x4d000000)
		    invalid_tls_insn (input_bfd, input_section, rel);
		  /* LD->LE transition.
		     bas %r14,0(%rx,%r13) -> bc 0,0  */
		  insn = 0x47000000;
		  bfd_put_32 (output_bfd, insn, contents + rel->r_offset);
		}
	    }
	  continue;

	default:
	  break;
	}

      /* Dynamic relocs are not propagated for SEC_DEBUGGING sections
	 because such sections are not SEC_ALLOC and thus ld.so will
	 not process them.  */
      if (unresolved_reloc
	  && !((input_section->flags & SEC_DEBUGGING) != 0
	       && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) != 0))
	(*_bfd_error_handler)
	  (_("%s(%s+0x%lx): unresolvable relocation against symbol `%s'"),
	   bfd_archive_filename (input_bfd),
	   bfd_get_section_name (input_bfd, input_section),
	   (long) rel->r_offset,
	   h->root.root.string);

      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, rel->r_offset,
				    relocation, rel->r_addend);

      if (r != bfd_reloc_ok)
	{
	  const char *name;

	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = bfd_elf_string_from_elf_section (input_bfd,
						      symtab_hdr->sh_link,
						      sym->st_name);
	      if (name == NULL)
		return FALSE;
	      if (*name == '\0')
		name = bfd_section_name (input_bfd, sec);
	    }

	  if (r == bfd_reloc_overflow)
	    {

	      if (! ((*info->callbacks->reloc_overflow)
		     (info, name, howto->name, (bfd_vma) 0,
		      input_bfd, input_section, rel->r_offset)))
		return FALSE;
	    }
	  else
	    {
	      (*_bfd_error_handler)
		(_("%s(%s+0x%lx): reloc against `%s': error %d"),
		 bfd_archive_filename (input_bfd),
		 bfd_get_section_name (input_bfd, input_section),
		 (long) rel->r_offset, name, (int) r);
	      return FALSE;
	    }
	}
    }

  return TRUE;
}

/* Finish up dynamic symbol handling.  We set the contents of various
   dynamic sections here.  */

static bfd_boolean
elf_s390_finish_dynamic_symbol (output_bfd, info, h, sym)
     bfd *output_bfd;
     struct bfd_link_info *info;
     struct elf_link_hash_entry *h;
     Elf_Internal_Sym *sym;
{
  struct elf_s390_link_hash_table *htab;

  htab = elf_s390_hash_table (info);

  if (h->plt.offset != (bfd_vma) -1)
    {
      bfd_vma plt_index;
      bfd_vma got_offset;
      Elf_Internal_Rela rela;
      bfd_byte *loc;
      bfd_vma relative_offset;

      /* This symbol has an entry in the procedure linkage table.  Set
	 it up.  */
      if (h->dynindx == -1
	  || htab->splt == NULL
	  || htab->sgotplt == NULL
	  || htab->srelplt == NULL)
	abort ();

      /* Calc. index no.
	 Current offset - size first entry / entry size.  */
      plt_index = (h->plt.offset - PLT_FIRST_ENTRY_SIZE) / PLT_ENTRY_SIZE;

      /* Offset in GOT is PLT index plus GOT headers(3) times 4,
	 addr & GOT addr.  */
      got_offset = (plt_index + 3) * GOT_ENTRY_SIZE;

      /* S390 uses halfwords for relative branch calc!  */
      relative_offset = - ((PLT_FIRST_ENTRY_SIZE +
			    (PLT_ENTRY_SIZE * plt_index) + 18) / 2);
      /* If offset is > 32768, branch to a previous branch
	 390 can only handle +-64 K jumps.  */
      if ( -32768 > (int) relative_offset )
	relative_offset
	  = -(unsigned) (((65536 / PLT_ENTRY_SIZE - 1) * PLT_ENTRY_SIZE) / 2);

      /* Fill in the entry in the procedure linkage table.  */
      if (!info->shared)
	{
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_ENTRY_WORD0,
		      htab->splt->contents + h->plt.offset);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_ENTRY_WORD1,
		      htab->splt->contents + h->plt.offset + 4);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_ENTRY_WORD2,
		      htab->splt->contents + h->plt.offset + 8);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_ENTRY_WORD3,
		      htab->splt->contents + h->plt.offset + 12);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_ENTRY_WORD4,
		      htab->splt->contents + h->plt.offset + 16);
	  bfd_put_32 (output_bfd, (bfd_vma) 0+(relative_offset << 16),
		      htab->splt->contents + h->plt.offset + 20);
	  bfd_put_32 (output_bfd,
		      (htab->sgotplt->output_section->vma
		       + htab->sgotplt->output_offset
		       + got_offset),
		      htab->splt->contents + h->plt.offset + 24);
	}
      else if (got_offset < 4096)
	{
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC12_ENTRY_WORD0 + got_offset,
		      htab->splt->contents + h->plt.offset);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC12_ENTRY_WORD1,
		      htab->splt->contents + h->plt.offset + 4);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC12_ENTRY_WORD2,
		      htab->splt->contents + h->plt.offset + 8);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC12_ENTRY_WORD3,
		      htab->splt->contents + h->plt.offset + 12);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC12_ENTRY_WORD4,
		      htab->splt->contents + h->plt.offset + 16);
	  bfd_put_32 (output_bfd, (bfd_vma) 0+(relative_offset << 16),
		      htab->splt->contents + h->plt.offset + 20);
	  bfd_put_32 (output_bfd, (bfd_vma) 0,
		      htab->splt->contents + h->plt.offset + 24);
	}
      else if (got_offset < 32768)
	{
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC16_ENTRY_WORD0 + got_offset,
		      htab->splt->contents + h->plt.offset);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC16_ENTRY_WORD1,
		      htab->splt->contents + h->plt.offset + 4);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC16_ENTRY_WORD2,
		      htab->splt->contents + h->plt.offset + 8);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC16_ENTRY_WORD3,
		      htab->splt->contents + h->plt.offset + 12);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC16_ENTRY_WORD4,
		      htab->splt->contents + h->plt.offset + 16);
	  bfd_put_32 (output_bfd, (bfd_vma) 0+(relative_offset << 16),
		      htab->splt->contents + h->plt.offset + 20);
	  bfd_put_32 (output_bfd, (bfd_vma) 0,
		      htab->splt->contents + h->plt.offset + 24);
	}
      else
	{
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_ENTRY_WORD0,
		      htab->splt->contents + h->plt.offset);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_ENTRY_WORD1,
		      htab->splt->contents + h->plt.offset + 4);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_ENTRY_WORD2,
		      htab->splt->contents + h->plt.offset + 8);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_ENTRY_WORD3,
		      htab->splt->contents + h->plt.offset + 12);
	  bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_ENTRY_WORD4,
		      htab->splt->contents + h->plt.offset + 16);
	  bfd_put_32 (output_bfd, (bfd_vma) 0+(relative_offset << 16),
		      htab->splt->contents + h->plt.offset + 20);
	  bfd_put_32 (output_bfd, got_offset,
		      htab->splt->contents + h->plt.offset + 24);
	}
      /* Insert offset into  reloc. table here.  */
      bfd_put_32 (output_bfd, plt_index * sizeof (Elf32_External_Rela),
		  htab->splt->contents + h->plt.offset + 28);

      /* Fill in the entry in the global offset table.
	 Points to instruction after GOT offset.  */
      bfd_put_32 (output_bfd,
		  (htab->splt->output_section->vma
		   + htab->splt->output_offset
		   + h->plt.offset
		   + 12),
		  htab->sgotplt->contents + got_offset);

      /* Fill in the entry in the .rela.plt section.  */
      rela.r_offset = (htab->sgotplt->output_section->vma
		       + htab->sgotplt->output_offset
		       + got_offset);
      rela.r_info = ELF32_R_INFO (h->dynindx, R_390_JMP_SLOT);
      rela.r_addend = 0;
      loc = htab->srelplt->contents + plt_index * sizeof (Elf32_External_Rela);
      bfd_elf32_swap_reloca_out (output_bfd, &rela, loc);

      if ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
	{
	  /* Mark the symbol as undefined, rather than as defined in
	     the .plt section.  Leave the value alone.  This is a clue
	     for the dynamic linker, to make function pointer
	     comparisons work between an application and shared
	     library.  */
	  sym->st_shndx = SHN_UNDEF;
	}
    }

  if (h->got.offset != (bfd_vma) -1
      && elf_s390_hash_entry(h)->tls_type != GOT_TLS_GD
      && elf_s390_hash_entry(h)->tls_type != GOT_TLS_IE
      && elf_s390_hash_entry(h)->tls_type != GOT_TLS_IE_NLT)
    {
      Elf_Internal_Rela rela;
      bfd_byte *loc;

      /* This symbol has an entry in the global offset table.  Set it
	 up.  */

      if (htab->sgot == NULL || htab->srelgot == NULL)
	abort ();

      rela.r_offset = (htab->sgot->output_section->vma
		       + htab->sgot->output_offset
		       + (h->got.offset &~ (bfd_vma) 1));

      /* If this is a static link, or it is a -Bsymbolic link and the
	 symbol is defined locally or was forced to be local because
	 of a version file, we just want to emit a RELATIVE reloc.
	 The entry in the global offset table will already have been
	 initialized in the relocate_section function.  */
      if (info->shared
	  && (info->symbolic
	      || h->dynindx == -1
	      || (h->elf_link_hash_flags & ELF_LINK_FORCED_LOCAL))
	  && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR))
	{
	  BFD_ASSERT((h->got.offset & 1) != 0);
	  rela.r_info = ELF32_R_INFO (0, R_390_RELATIVE);
	  rela.r_addend = (h->root.u.def.value
			   + h->root.u.def.section->output_section->vma
			   + h->root.u.def.section->output_offset);
	}
      else
	{
	  BFD_ASSERT((h->got.offset & 1) == 0);
	  bfd_put_32 (output_bfd, (bfd_vma) 0, htab->sgot->contents + h->got.offset);
	  rela.r_info = ELF32_R_INFO (h->dynindx, R_390_GLOB_DAT);
	  rela.r_addend = 0;
	}

      loc = htab->srelgot->contents;
      loc += htab->srelgot->reloc_count++ * sizeof (Elf32_External_Rela);
      bfd_elf32_swap_reloca_out (output_bfd, &rela, loc);
    }

  if ((h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_COPY) != 0)
    {
      Elf_Internal_Rela rela;
      bfd_byte *loc;

      /* This symbols needs a copy reloc.  Set it up.  */

      if (h->dynindx == -1
	  || (h->root.type != bfd_link_hash_defined
	      && h->root.type != bfd_link_hash_defweak)
	  || htab->srelbss == NULL)
	abort ();

      rela.r_offset = (h->root.u.def.value
		       + h->root.u.def.section->output_section->vma
		       + h->root.u.def.section->output_offset);
      rela.r_info = ELF32_R_INFO (h->dynindx, R_390_COPY);
      rela.r_addend = 0;
      loc = htab->srelbss->contents;
      loc += htab->srelbss->reloc_count++ * sizeof (Elf32_External_Rela);
      bfd_elf32_swap_reloca_out (output_bfd, &rela, loc);
    }

  /* Mark some specially defined symbols as absolute.  */
  if (strcmp (h->root.root.string, "_DYNAMIC") == 0
      || strcmp (h->root.root.string, "_GLOBAL_OFFSET_TABLE_") == 0
      || strcmp (h->root.root.string, "_PROCEDURE_LINKAGE_TABLE_") == 0)
    sym->st_shndx = SHN_ABS;

  return TRUE;
}

/* Used to decide how to sort relocs in an optimal manner for the
   dynamic linker, before writing them out.  */

static enum elf_reloc_type_class
elf_s390_reloc_type_class (rela)
     const Elf_Internal_Rela *rela;
{
  switch ((int) ELF32_R_TYPE (rela->r_info))
    {
    case R_390_RELATIVE:
      return reloc_class_relative;
    case R_390_JMP_SLOT:
      return reloc_class_plt;
    case R_390_COPY:
      return reloc_class_copy;
    default:
      return reloc_class_normal;
    }
}

/* Finish up the dynamic sections.  */

static bfd_boolean
elf_s390_finish_dynamic_sections (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  struct elf_s390_link_hash_table *htab;
  bfd *dynobj;
  asection *sdyn;

  htab = elf_s390_hash_table (info);
  dynobj = htab->elf.dynobj;
  sdyn = bfd_get_section_by_name (dynobj, ".dynamic");

  if (htab->elf.dynamic_sections_created)
    {
      Elf32_External_Dyn *dyncon, *dynconend;

      if (sdyn == NULL || htab->sgot == NULL)
	abort ();

      dyncon = (Elf32_External_Dyn *) sdyn->contents;
      dynconend = (Elf32_External_Dyn *) (sdyn->contents + sdyn->_raw_size);
      for (; dyncon < dynconend; dyncon++)
	{
	  Elf_Internal_Dyn dyn;
	  asection *s;

	  bfd_elf32_swap_dyn_in (dynobj, dyncon, &dyn);

	  switch (dyn.d_tag)
	    {
	    default:
	      continue;

	    case DT_PLTGOT:
	      dyn.d_un.d_ptr = htab->sgot->output_section->vma;
	      break;

	    case DT_JMPREL:
	      dyn.d_un.d_ptr = htab->srelplt->output_section->vma;
	      break;

	    case DT_PLTRELSZ:
	      s = htab->srelplt->output_section;
	      if (s->_cooked_size != 0)
		dyn.d_un.d_val = s->_cooked_size;
	      else
		dyn.d_un.d_val = s->_raw_size;
	      break;
	    }

	  bfd_elf32_swap_dyn_out (output_bfd, &dyn, dyncon);
	}

      /* Fill in the special first entry in the procedure linkage table.  */
      if (htab->splt && htab->splt->_raw_size > 0)
	{
	  memset (htab->splt->contents, 0, PLT_FIRST_ENTRY_SIZE);
	  if (info->shared)
	    {
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_FIRST_ENTRY_WORD0,
			  htab->splt->contents );
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_FIRST_ENTRY_WORD1,
			  htab->splt->contents +4 );
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_FIRST_ENTRY_WORD2,
			  htab->splt->contents +8 );
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_FIRST_ENTRY_WORD3,
			  htab->splt->contents +12 );
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_PIC_FIRST_ENTRY_WORD4,
			  htab->splt->contents +16 );
	   }
	  else
	   {
	      bfd_put_32 (output_bfd, (bfd_vma)PLT_FIRST_ENTRY_WORD0,
			  htab->splt->contents );
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_FIRST_ENTRY_WORD1,
			  htab->splt->contents +4 );
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_FIRST_ENTRY_WORD2,
			  htab->splt->contents +8 );
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_FIRST_ENTRY_WORD3,
			  htab->splt->contents +12 );
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_FIRST_ENTRY_WORD4,
			  htab->splt->contents +16 );
	      bfd_put_32 (output_bfd, (bfd_vma) PLT_FIRST_ENTRY_WORD5,
			  htab->splt->contents +20 );
	      bfd_put_32 (output_bfd,
			  htab->sgotplt->output_section->vma
			  + htab->sgotplt->output_offset,
			  htab->splt->contents + 24);
	   }
	  elf_section_data (htab->splt->output_section)
	    ->this_hdr.sh_entsize = 4;
	}

    }

  if (htab->sgotplt)
    {
      /* Fill in the first three entries in the global offset table.  */
      if (htab->sgotplt->_raw_size > 0)
	{
	  bfd_put_32 (output_bfd,
		      (sdyn == NULL ? (bfd_vma) 0
		       : sdyn->output_section->vma + sdyn->output_offset),
		      htab->sgotplt->contents);
	  /* One entry for shared object struct ptr.  */
	  bfd_put_32 (output_bfd, (bfd_vma) 0, htab->sgotplt->contents + 4);
	  /* One entry for _dl_runtime_resolve.  */
	  bfd_put_32 (output_bfd, (bfd_vma) 0, htab->sgotplt->contents + 8);
	}

      elf_section_data (htab->sgotplt->output_section)
	->this_hdr.sh_entsize = 4;
    }
  return TRUE;
}

static bfd_boolean
elf_s390_grok_prstatus (abfd, note)
     bfd * abfd;
     Elf_Internal_Note * note;
{
  int offset;
  unsigned int raw_size;

  switch (note->descsz)
    {
      default:
	return FALSE;

      case 224:		/* S/390 Linux.  */
	/* pr_cursig */
	elf_tdata (abfd)->core_signal = bfd_get_16 (abfd, note->descdata + 12);

	/* pr_pid */
	elf_tdata (abfd)->core_pid = bfd_get_32 (abfd, note->descdata + 24);

	/* pr_reg */
	offset = 72;
	raw_size = 144;
	break;
    }

  /* Make a ".reg/999" section.  */
  return _bfd_elfcore_make_pseudosection (abfd, ".reg",
					  raw_size, note->descpos + offset);
}

#define TARGET_BIG_SYM	bfd_elf32_s390_vec
#define TARGET_BIG_NAME	"elf32-s390"
#define ELF_ARCH	bfd_arch_s390
#define ELF_MACHINE_CODE EM_S390
#define ELF_MACHINE_ALT1 EM_S390_OLD
#define ELF_MAXPAGESIZE 0x1000

#define elf_backend_can_gc_sections	1
#define elf_backend_can_refcount	1
#define elf_backend_want_got_plt	1
#define elf_backend_plt_readonly	1
#define elf_backend_want_plt_sym	0
#define elf_backend_got_header_size	12
#define elf_backend_plt_header_size	PLT_ENTRY_SIZE
#define elf_backend_rela_normal		1

#define elf_info_to_howto		      elf_s390_info_to_howto

#define bfd_elf32_bfd_is_local_label_name     elf_s390_is_local_label_name
#define bfd_elf32_bfd_link_hash_table_create  elf_s390_link_hash_table_create
#define bfd_elf32_bfd_reloc_type_lookup	      elf_s390_reloc_type_lookup

#define elf_backend_adjust_dynamic_symbol     elf_s390_adjust_dynamic_symbol
#define elf_backend_check_relocs	      elf_s390_check_relocs
#define elf_backend_copy_indirect_symbol      elf_s390_copy_indirect_symbol
#define elf_backend_create_dynamic_sections   elf_s390_create_dynamic_sections
#define elf_backend_finish_dynamic_sections   elf_s390_finish_dynamic_sections
#define elf_backend_finish_dynamic_symbol     elf_s390_finish_dynamic_symbol
#define elf_backend_gc_mark_hook	      elf_s390_gc_mark_hook
#define elf_backend_gc_sweep_hook	      elf_s390_gc_sweep_hook
#define elf_backend_reloc_type_class	      elf_s390_reloc_type_class
#define elf_backend_relocate_section	      elf_s390_relocate_section
#define elf_backend_size_dynamic_sections     elf_s390_size_dynamic_sections
#define elf_backend_reloc_type_class	      elf_s390_reloc_type_class
#define elf_backend_grok_prstatus	      elf_s390_grok_prstatus

#define bfd_elf32_mkobject		elf_s390_mkobject
#define elf_backend_object_p		elf_s390_object_p

#include "elf32-target.h"
