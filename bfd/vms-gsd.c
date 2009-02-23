/* vms-gsd.c -- BFD back-end for VAX (openVMS/VAX) and
   EVAX (openVMS/Alpha) files.
   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
   2007, 2009 Free Software Foundation, Inc.

   GSD record handling functions
   EGSD record handling functions

   Go and read the openVMS linker manual (esp. appendix B)
   if you don't know what's going on here :-)

   Written by Klaus K"ampf (kkaempf@rmi.de)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "libbfd.h"

#include "vms.h"

/* Typical sections for vax object files.  */

#define VAX_CODE_NAME		"$CODE"
#define VAX_DATA_NAME		"$DATA"
#define VAX_ADDRESS_DATA_NAME	"$ADDRESS_DATA"

/* Typical sections for evax object files.  */

#define EVAX_ABS_NAME		"$ABS$"
#define EVAX_CODE_NAME		"$CODE$"
#define EVAX_LINK_NAME		"$LINK$"
#define EVAX_DATA_NAME		"$DATA$"
#define EVAX_BSS_NAME		"$BSS$"
#define EVAX_READONLYADDR_NAME	"$READONLY_ADDR$"
#define EVAX_READONLY_NAME	"$READONLY$"
#define EVAX_LITERAL_NAME	"$LITERAL$"
#define EVAX_LITERALS_NAME	"$LITERALS"
#define EVAX_COMMON_NAME	"$COMMON$"
#define EVAX_LOCAL_NAME		"$LOCAL$"

struct sec_flags_struct
{
  char *name;			/* Name of section.  */
  int vflags_always;
  flagword flags_always;	/* Flags we set always.  */
  int vflags_hassize;
  flagword flags_hassize;	/* Flags we set if the section has a size > 0.  */
};

/* These flags are deccrtl/vaxcrtl (openVMS 6.2 VAX) compatible.  */

static struct sec_flags_struct vax_section_flags[] =
  {
    { VAX_CODE_NAME,
      (GPS_S_M_PIC | GPS_S_M_REL | GPS_S_M_SHR | GPS_S_M_EXE | GPS_S_M_RD),
      (SEC_CODE),
      (GPS_S_M_PIC | GPS_S_M_REL | GPS_S_M_SHR | GPS_S_M_EXE | GPS_S_M_RD),
      (SEC_IN_MEMORY | SEC_CODE | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD) },
    { VAX_DATA_NAME,
      (GPS_S_M_PIC | GPS_S_M_REL | GPS_S_M_RD | GPS_S_M_WRT),
      (SEC_DATA),
      (GPS_S_M_PIC | GPS_S_M_REL | GPS_S_M_RD | GPS_S_M_WRT),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD) },
    { VAX_ADDRESS_DATA_NAME,
      (GPS_S_M_PIC | GPS_S_M_REL | GPS_S_M_RD),
      (SEC_DATA | SEC_READONLY),
      (GPS_S_M_PIC | GPS_S_M_REL | GPS_S_M_RD),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_READONLY | SEC_LOAD) },
    { NULL,
      (GPS_S_M_PIC | GPS_S_M_OVR | GPS_S_M_REL | GPS_S_M_GBL | GPS_S_M_RD | GPS_S_M_WRT),
      (SEC_DATA),
      (GPS_S_M_PIC | GPS_S_M_OVR | GPS_S_M_REL | GPS_S_M_GBL | GPS_S_M_RD | GPS_S_M_WRT),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD) }
  };

/* These flags are deccrtl/vaxcrtl (openVMS 6.2 Alpha) compatible.  */

static struct sec_flags_struct evax_section_flags[] =
  {
    { EVAX_ABS_NAME,
      (EGPS_S_V_SHR),
      (SEC_DATA),
      (EGPS_S_V_SHR),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD) },
    { EVAX_CODE_NAME,
      (EGPS_S_V_PIC | EGPS_S_V_REL | EGPS_S_V_SHR | EGPS_S_V_EXE),
      (SEC_CODE),
      (EGPS_S_V_PIC | EGPS_S_V_REL | EGPS_S_V_SHR | EGPS_S_V_EXE),
      (SEC_IN_MEMORY | SEC_CODE | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD) },
    { EVAX_LITERAL_NAME,
      (EGPS_S_V_PIC | EGPS_S_V_REL | EGPS_S_V_SHR | EGPS_S_V_RD | EGPS_S_V_NOMOD),
      (SEC_DATA | SEC_READONLY),
      (EGPS_S_V_PIC | EGPS_S_V_REL | EGPS_S_V_SHR | EGPS_S_V_RD),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_READONLY | SEC_LOAD) },
    { EVAX_LINK_NAME,
      (EGPS_S_V_REL | EGPS_S_V_RD),
      (SEC_DATA | SEC_READONLY),
      (EGPS_S_V_REL | EGPS_S_V_RD),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_READONLY | SEC_LOAD) },
    { EVAX_DATA_NAME,
      (EGPS_S_V_REL | EGPS_S_V_RD | EGPS_S_V_WRT | EGPS_S_V_NOMOD),
      (SEC_DATA),
      (EGPS_S_V_REL | EGPS_S_V_RD | EGPS_S_V_WRT),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD) },
    { EVAX_BSS_NAME,
      (EGPS_S_V_REL | EGPS_S_V_RD | EGPS_S_V_WRT | EGPS_S_V_NOMOD),
      (SEC_NO_FLAGS),
      (EGPS_S_V_REL | EGPS_S_V_RD | EGPS_S_V_WRT | EGPS_S_V_NOMOD),
      (SEC_IN_MEMORY | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD) },
    { EVAX_READONLYADDR_NAME,
      (EGPS_S_V_PIC | EGPS_S_V_REL | EGPS_S_V_RD),
      (SEC_DATA | SEC_READONLY),
      (EGPS_S_V_PIC | EGPS_S_V_REL | EGPS_S_V_RD),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_READONLY | SEC_LOAD) },
    { EVAX_READONLY_NAME,
      (EGPS_S_V_PIC | EGPS_S_V_REL | EGPS_S_V_SHR | EGPS_S_V_RD | EGPS_S_V_NOMOD),
      (SEC_DATA | SEC_READONLY),
      (EGPS_S_V_PIC | EGPS_S_V_REL | EGPS_S_V_SHR | EGPS_S_V_RD),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_READONLY | SEC_LOAD) },
    { EVAX_LOCAL_NAME,
      (EGPS_S_V_REL | EGPS_S_V_RD | EGPS_S_V_WRT),
      (SEC_DATA),
      (EGPS_S_V_REL | EGPS_S_V_RD | EGPS_S_V_WRT),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD) },
    { EVAX_LITERALS_NAME,
      (EGPS_S_V_PIC | EGPS_S_V_OVR),
      (SEC_DATA | SEC_READONLY),
      (EGPS_S_V_PIC | EGPS_S_V_OVR),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_READONLY | SEC_LOAD) },
    { NULL,
      (EGPS_S_V_REL | EGPS_S_V_RD | EGPS_S_V_WRT),
      (SEC_DATA),
      (EGPS_S_V_REL | EGPS_S_V_RD | EGPS_S_V_WRT),
      (SEC_IN_MEMORY | SEC_DATA | SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD) }
  };

/* Retrieve bfd section flags by name and size.  */

static flagword
vms_secflag_by_name (bfd *abfd,
		     struct sec_flags_struct *section_flags,
		     char *name,
		     int hassize)
{
  int i = 0;

  while (section_flags[i].name != NULL)
    {
      if ((PRIV (is_vax)?
	   strcasecmp (name, section_flags[i].name):
	   strcmp (name, section_flags[i].name)) == 0)
	{
	  if (hassize)
	    return section_flags[i].flags_hassize;
	  else
	    return section_flags[i].flags_always;
	}
      i++;
    }
  if (hassize)
    return section_flags[i].flags_hassize;
  return section_flags[i].flags_always;
}

/* Retrieve vms section flags by name and size.  */

static flagword
vms_esecflag_by_name (struct sec_flags_struct *section_flags,
		      char *name,
		      int hassize)
{
  int i = 0;

  while (section_flags[i].name != NULL)
    {
      if (strcmp (name, section_flags[i].name) == 0)
	{
	  if (hassize)
	    return section_flags[i].vflags_hassize;
	  else
	    return section_flags[i].vflags_always;
	}
      i++;
    }
  if (hassize)
    return section_flags[i].vflags_hassize;
  return section_flags[i].vflags_always;
}

#if VMS_DEBUG

struct flagdescstruct { char *name; flagword value; };

static const struct flagdescstruct gpsflagdesc[] =
{
  { "PIC", GPS_S_M_PIC },
  { "LIB", GPS_S_M_LIB },
  { "OVR", GPS_S_M_OVR },
  { "REL", GPS_S_M_REL },
  { "GBL", GPS_S_M_GBL },
  { "SHR", GPS_S_M_SHR },
  { "EXE", GPS_S_M_EXE },
  { "RD",  GPS_S_M_RD },
  { "WRT", GPS_S_M_WRT },
  { "VEC", GPS_S_M_VEC },
  { "NOMOD", EGPS_S_V_NOMOD },
  { "COM", EGPS_S_V_COM },
  { NULL, 0 }
};

static const struct flagdescstruct gsyflagdesc[] =
{
  { "WEAK", GSY_S_M_WEAK },
  { "DEF",  GSY_S_M_DEF },
  { "UNI",  GSY_S_M_UNI },
  { "REL",  GSY_S_M_REL },
  { "COMM", EGSY_S_V_COMM },
  { "VECEP", EGSY_S_V_VECEP },
  { "NORM", EGCY_S_V_NORM },
  { NULL, 0 }
};

static char *flag2str (struct flagdescstruct *, flagword);

/* Convert flag to printable string.  */

static char *
flag2str (struct flagdescstruct * flagdesc, flagword flags)
{
  static char res[64];
  int next = 0;

  res[0] = 0;
  while (flagdesc->name != NULL)
    {
      if ((flags & flagdesc->value) != 0)
	{
	  if (next)
	    strcat (res, ",");
	  else
	    next = 1;
	  strcat (res, flagdesc->name);
	}
      flagdesc++;
    }
  return res;
}
#endif

/* Input routines.  */

static int register_universal_symbol (bfd *abfd, asymbol *symbol,
				      int vms_flags);

/* Process GSD/EGSD record
   return 0 on success, -1 on error.  */

int
_bfd_vms_slurp_gsd (bfd * abfd, int objtype)
{
  int gsd_type, gsd_size;
  asection *section;
  unsigned char *vms_rec;
  flagword new_flags, old_flags;
  char *name;
  asymbol *symbol;
  vms_symbol_entry *entry;
  unsigned long base_addr;
  unsigned long align_addr;
  static unsigned int psect_idx = 0;

#if VMS_DEBUG
  vms_debug (2, "GSD/EGSD (%d/%x)\n", objtype, objtype);
#endif

  switch (objtype)
    {
    case EOBJ_S_C_EGSD:
      PRIV (vms_rec) += 8;	/* Skip type, size, l_temp.  */
      PRIV (rec_size) -= 8;
      break;
    case OBJ_S_C_GSD:
      PRIV (vms_rec) += 1;
      PRIV (rec_size) -= 1;
      break;
    default:
      return -1;
    }

  /* Calculate base address for each section.  */
  base_addr = 0L;

  abfd->symcount = 0;

  while (PRIV (rec_size) > 0)
    {
      vms_rec = PRIV (vms_rec);

      if (objtype == OBJ_S_C_GSD)
	gsd_type = vms_rec[0];
      else
	{
	  _bfd_vms_get_header_values (abfd, vms_rec, &gsd_type, &gsd_size);
	  gsd_type += EVAX_OFFSET;
	}

#if VMS_DEBUG
      vms_debug (3, "gsd_type %d\n", gsd_type);
#endif

      switch (gsd_type)
	{
	case GSD_S_C_PSC:
	  {
	    /* Program section definition.  */
	    asection *old_section = 0;

#if VMS_DEBUG
	    vms_debug (4, "GSD_S_C_PSC\n");
#endif
	    /* If this section isn't a bfd section.  */
	    if (PRIV (is_vax) && (psect_idx < (abfd->section_count - 1)))
	      {
		/* Check for temporary section from TIR record.  */
		if (psect_idx < PRIV (section_count))
		  old_section = PRIV (sections)[psect_idx];
		else
		  old_section = 0;
	      }

	    name = _bfd_vms_save_counted_string (vms_rec + 8);
	    section = bfd_make_section (abfd, name);
	    if (!section)
	      {
		(*_bfd_error_handler) (_("bfd_make_section (%s) failed"),
				       name);
		return -1;
	      }
	    old_flags = bfd_getl16 (vms_rec + 2);
	    section->size = bfd_getl32 (vms_rec + 4);  /* allocation */
	    new_flags = vms_secflag_by_name (abfd, vax_section_flags, name,
					     section->size > 0);
	    if (old_flags & EGPS_S_V_REL)
	      new_flags |= SEC_RELOC;
	    if (old_flags & GPS_S_M_OVR)
	      new_flags |= SEC_IS_COMMON;
	    if (!bfd_set_section_flags (abfd, section, new_flags))
	      {
		(*_bfd_error_handler)
		  (_("bfd_set_section_flags (%s, %x) failed"),
		   name, new_flags);
		return -1;
	      }
	    section->alignment_power = vms_rec[1];
	    align_addr = (1 << section->alignment_power);
	    if ((base_addr % align_addr) != 0)
	      base_addr += (align_addr - (base_addr % align_addr));
	    section->vma = (bfd_vma)base_addr;
	    base_addr += section->size;

	    /* Global section is common symbol.  */
	    if (old_flags & GPS_S_M_GBL)
	      {
		entry = _bfd_vms_enter_symbol (abfd, name);
		if (entry == NULL)
		  {
		    bfd_set_error (bfd_error_no_memory);
		    return -1;
		  }
		symbol = entry->symbol;

		symbol->value = 0;
		symbol->section = section;
		symbol->flags = (BSF_GLOBAL | BSF_SECTION_SYM | BSF_OLD_COMMON);
	      }

	    /* Copy saved contents if old_section set.  */
	    if (old_section != 0)
	      {
		section->contents = old_section->contents;
		if (section->size < old_section->size)
		  {
		    (*_bfd_error_handler)
		      (_("Size mismatch section %s=%lx, %s=%lx"),
		       old_section->name,
		       (unsigned long) old_section->size,
		       section->name,
		       (unsigned long) section->size);
		    return -1;
		  }
		else if (section->size > old_section->size)
		  {
		    section->contents = bfd_realloc (old_section->contents,
						     section->size);
		    if (section->contents == NULL)
		      {
			bfd_set_error (bfd_error_no_memory);
			return -1;
		      }
		  }
	      }
	    else
	      {
		section->contents = bfd_zmalloc (section->size);
		if (section->contents == NULL)
		  {
		    bfd_set_error (bfd_error_no_memory);
		    return -1;
		  }
	      }
#if VMS_DEBUG
	    vms_debug (4, "gsd psc %d (%s, flags %04x=%s) ",
		       section->index, name, old_flags, flag2str (gpsflagdesc, old_flags));
	    vms_debug (4, "%d bytes at 0x%08lx (mem %p)\n",
		       section->size, section->vma, section->contents);
#endif

	    gsd_size = vms_rec[8] + 9;

	    psect_idx++;
	  }
	  break;

	case GSD_S_C_EPM:
	case GSD_S_C_EPMW:
#if VMS_DEBUG
	  vms_debug (4, "gsd epm\n");
#endif
	  /* Fall through.  */
	case GSD_S_C_SYM:
	case GSD_S_C_SYMW:
	  {
	    int name_offset = 0, value_offset = 0;

	    /* Symbol specification (definition or reference).  */
#if VMS_DEBUG
	    vms_debug (4, "GSD_S_C_SYM(W)\n");
#endif
	    old_flags = bfd_getl16 (vms_rec + 2);
	    new_flags = BSF_NO_FLAGS;

	    if (old_flags & GSY_S_M_WEAK)
	      new_flags |= BSF_WEAK;

	    switch (gsd_type)
	      {
	      case GSD_S_C_EPM:
		name_offset = 11;
		value_offset = 5;
		new_flags |= BSF_FUNCTION;
		break;
	      case GSD_S_C_EPMW:
		name_offset = 12;
		value_offset = 6;
		new_flags |= BSF_FUNCTION;
		break;
	      case GSD_S_C_SYM:
		if (old_flags & GSY_S_M_DEF)	/* Symbol definition.  */
		  name_offset = 9;
		else
		  name_offset = 4;
		value_offset = 5;
		break;
	      case GSD_S_C_SYMW:
		if (old_flags & GSY_S_M_DEF)	/* Symbol definition.  */
		  name_offset = 10;
		else
		  name_offset = 5;
		value_offset = 6;
		break;
	      }

	    /* Save symbol in vms_symbol_table.  */
	    entry = _bfd_vms_enter_symbol
	      (abfd, _bfd_vms_save_counted_string (vms_rec + name_offset));
	    if (entry == NULL)
	      {
		bfd_set_error (bfd_error_no_memory);
		return -1;
	      }
	    symbol = entry->symbol;

	    if (old_flags & GSY_S_M_DEF)
	      {
		/* Symbol definition.  */
		int psect;

		symbol->value = bfd_getl32 (vms_rec + value_offset);
		if ((gsd_type == GSD_S_C_SYMW)
		    || (gsd_type == GSD_S_C_EPMW))
		  psect = bfd_getl16 (vms_rec + value_offset - 2);
		else
		  psect = vms_rec[value_offset-1];

		symbol->section = (asection *)(unsigned long)psect;
#if VMS_DEBUG
		vms_debug (4, "gsd sym def #%d (%s, %ld, %04x=%s)\n", abfd->symcount,
			  symbol->name, (long)symbol->section, old_flags, flag2str(gsyflagdesc, old_flags));
#endif
	      }
	    else
	      {
		/* Symbol reference.  */
#if VMS_DEBUG
		vms_debug (4, "gsd sym ref #%d (%s, %04x=%s)\n", abfd->symcount,
			   symbol->name, old_flags, flag2str (gsyflagdesc, old_flags));
#endif
		symbol->section = (asection *)(unsigned long)-1;
	      }

	    gsd_size = vms_rec[name_offset] + name_offset + 1;
	    symbol->flags = new_flags;
	  }

	  break;

	case GSD_S_C_PRO:
	case GSD_S_C_PROW:
#if VMS_DEBUG
	  vms_debug (4, "gsd pro\n");
#endif
	  break;
	case GSD_S_C_IDC:
#if VMS_DEBUG
	  vms_debug (4, "gsd idc\n");
#endif
	  break;
	case GSD_S_C_ENV:
#if VMS_DEBUG
	  vms_debug (4, "gsd env\n");
#endif
	  break;
	case GSD_S_C_LSY:
#if VMS_DEBUG
	  vms_debug (4, "gsd lsy\n");
#endif
	  break;
	case GSD_S_C_LEPM:
#if VMS_DEBUG
	  vms_debug (4, "gsd lepm\n");
#endif
	  break;
	case GSD_S_C_LPRO:
#if VMS_DEBUG
	  vms_debug (4, "gsd lpro\n");
#endif
	  break;
	case GSD_S_C_SPSC:
#if VMS_DEBUG
	  vms_debug (4, "gsd spsc\n");
#endif
	  break;
	case GSD_S_C_SYMV:
#if VMS_DEBUG
	  vms_debug (4, "gsd symv\n");
#endif
	  break;
	case GSD_S_C_EPMV:
#if VMS_DEBUG
	  vms_debug (4, "gsd epmv\n");
#endif
	  break;
	case GSD_S_C_PROV:
#if VMS_DEBUG
	  vms_debug (4, "gsd prov\n");
#endif
	  break;

	case EGSD_S_C_PSC + EVAX_OFFSET:
	  {
	    /* Program section definition.  */
	    name = _bfd_vms_save_counted_string (vms_rec + EGPS_S_B_NAMLNG);
	    section = bfd_make_section (abfd, name);
	    if (!section)
	      return -1;
	    old_flags = bfd_getl16 (vms_rec + EGPS_S_W_FLAGS);
	    section->size = bfd_getl32 (vms_rec + EGPS_S_L_ALLOC);
	    new_flags = vms_secflag_by_name (abfd, evax_section_flags, name,
					     section->size > 0);
	    if (old_flags & EGPS_S_V_REL)
	      new_flags |= SEC_RELOC;
	    if (!bfd_set_section_flags (abfd, section, new_flags))
	      return -1;
	    section->alignment_power = vms_rec[EGPS_S_B_ALIGN];
	    align_addr = (1 << section->alignment_power);
	    if ((base_addr % align_addr) != 0)
	      base_addr += (align_addr - (base_addr % align_addr));
	    section->vma = (bfd_vma)base_addr;
	    base_addr += section->size;
	    section->contents = bfd_zmalloc (section->size);
	    if (section->contents == NULL)
	      return -1;
	    section->filepos = (unsigned int)-1;
#if VMS_DEBUG
	    vms_debug (4, "EGSD P-section %d (%s, flags %04x=%s) ",
		       section->index, name, old_flags, flag2str(gpsflagdesc, old_flags));
	    vms_debug (4, "%d bytes at 0x%08lx (mem %p)\n",
		       section->size, section->vma, section->contents);
#endif
	  }
	  break;

	case EGSD_S_C_SYM + EVAX_OFFSET:
	  {
	    /* Global symbol specification (definition or reference).  */
	    symbol = bfd_make_empty_symbol (abfd);
	    if (symbol == 0)
	      return -1;

	    old_flags = bfd_getl16 (vms_rec + EGSY_S_W_FLAGS);
	    new_flags = BSF_NO_FLAGS;

	    if (old_flags & EGSY_S_V_WEAK)
	      new_flags |= BSF_WEAK;

	    if (old_flags & EGSY_S_V_DEF)
	      {
		/* Symbol definition.  */
		if (old_flags & EGSY_S_V_NORM)
		  new_flags |= BSF_FUNCTION;
		symbol->name =
		  _bfd_vms_save_counted_string (vms_rec + ESDF_S_B_NAMLNG);
		symbol->value = bfd_getl64 (vms_rec + ESDF_S_L_VALUE);
		symbol->section =
		  (asection *)(unsigned long) bfd_getl32 (vms_rec + ESDF_S_L_PSINDX);
#if VMS_DEBUG
		vms_debug (4, "EGSD sym def #%d (%s, %ld, %04x=%s)\n",
			   abfd->symcount, symbol->name, (long)symbol->section,
			   old_flags, flag2str (gsyflagdesc, old_flags));
#endif
	      }
	    else
	      {
		/* Symbol reference.  */
		symbol->name =
		  _bfd_vms_save_counted_string (vms_rec + ESRF_S_B_NAMLNG);
#if VMS_DEBUG
		vms_debug (4, "EGSD sym ref #%d (%s, %04x=%s)\n",
			   abfd->symcount, symbol->name, old_flags,
			   flag2str (gsyflagdesc, old_flags));
#endif
		symbol->section = (asection *)(unsigned long)-1;
	      }

	    symbol->flags = new_flags;

	    /* Register symbol in VMS symbol table.  */
	    entry = (vms_symbol_entry *) bfd_hash_lookup
	      (PRIV (vms_symbol_table), symbol->name, TRUE, FALSE);

	    if (entry == NULL)
	      {
		bfd_set_error (bfd_error_no_memory);
		return -1;
	      }

	    if (entry->symbol != NULL)
	      {
		/* FIXME ?, DEC C generates this.  */
#if VMS_DEBUG
		vms_debug (4, "EGSD_S_C_SYM: duplicate \"%s\"\n", symbol->name);
#endif
	      }
	    else
	      {
		entry->symbol = symbol;
		PRIV (gsd_sym_count)++;
		abfd->symcount++;
	      }
	  }
	  break;

	case EGSD_S_C_SYMG + EVAX_OFFSET:
	  {
	    /* Universal symbol specification (definition).  */
	    symbol = bfd_make_empty_symbol (abfd);
	    if (symbol == 0)
	      return -1;

	    old_flags = bfd_getl16 (vms_rec + EGST_S_W_FLAGS);
	    new_flags = BSF_NO_FLAGS;

	    if (old_flags & EGSY_S_V_WEAK)
	      new_flags |= BSF_WEAK;

	    if (old_flags & EGSY_S_V_DEF) /* symbol definition */
	      {
		if (old_flags & EGSY_S_V_NORM)
		  new_flags |= BSF_FUNCTION;

		symbol->name =
		  _bfd_vms_save_counted_string (vms_rec + EGST_S_B_NAMLNG);

		/* For BSF_FUNCTION symbols, the entry point is in LP_1
		   and the descriptor in LP_2.  For other symbols, the
		   unique value is in LP_2.  */
		symbol->value = bfd_getl64 (vms_rec + EGST_S_Q_LP_2);

		/* Adding this offset is necessary in order for GDB to
		   read the DWARF-2 debug info from shared libraries.  */
		if (abfd->flags & DYNAMIC
		    && strstr (symbol->name, "$DWARF2.DEBUG") != 0)
		  symbol->value += PRIV (symvva);
	      }
	    else /* symbol reference */
	      (*_bfd_error_handler) ("Invalid EGST reference");

	    symbol->flags = new_flags;

	    if (register_universal_symbol (abfd, symbol, old_flags) < 0)
	      return -1;

	    /* Make a second symbol for the entry point.  */
	    if (symbol->flags & BSF_FUNCTION)
	      {
		asymbol *en_sym;
		char *name = bfd_alloc (abfd, strlen (symbol->name) + 5);

		en_sym = bfd_make_empty_symbol (abfd);
		if (en_sym == 0)
		  return -1;

		strcpy (name, symbol->name);
		strcat (name, "..en");

		en_sym->name = name;
		en_sym->value = bfd_getl64 (vms_rec + EGST_S_Q_LP_1);

		if (register_universal_symbol (abfd, en_sym, old_flags) < 0)
		  return -1;
	      }
	  }
	  break;

	case EGSD_S_C_IDC + EVAX_OFFSET:
  	  break;

	default:
	  (*_bfd_error_handler) (_("Unknown GSD/EGSD subtype %d"), gsd_type);
	  bfd_set_error (bfd_error_bad_value);
	  return -1;
	}

      PRIV (rec_size) -= gsd_size;
      PRIV (vms_rec) += gsd_size;
    }

  if (abfd->symcount > 0)
    abfd->flags |= HAS_SYMS;

  return 0;
}

/* Register a universal symbol in the VMS symbol table.  */

static int
register_universal_symbol (bfd *abfd, asymbol *symbol, int vms_flags)
{
  bfd_vma sbase = 0;
  asection *s, *sec = NULL;
  vms_symbol_entry *entry;

  /* A universal symbol is by definition global...  */
  symbol->flags |= BSF_GLOBAL;

  /* ...and dynamic in shared libraries.  */
  if (abfd->flags & DYNAMIC)
    symbol->flags |= BSF_DYNAMIC;

  /* Find containing section.  */
  for (s = abfd->sections; s; s = s->next)
    {
      if (symbol->value >= s->vma
	  && s->vma > sbase
	  && !(s->flags & SEC_COFF_SHARED_LIBRARY)
	  && (s->size > 0 || !(vms_flags & EGSY_S_V_REL)))
	{
	  sbase = s->vma;
	  sec = s;
	}
    }

  symbol->value -= sbase;
  symbol->section = sec;

#if VMS_DEBUG
  vms_debug (4, "EGST sym def #%d (%s, 0x%llx => 0x%llx, %04x=%s)\n",
	     abfd->symcount, symbol->name, symbol->value + sbase,
	     symbol->value, vms_flags,
	     flag2str(gsyflagdesc, vms_flags));
#endif

  entry = (vms_symbol_entry *) bfd_hash_lookup (PRIV (vms_symbol_table),
						symbol->name,
						TRUE, FALSE);

  if (entry == NULL)
    {
      bfd_set_error (bfd_error_no_memory);
      return -1;
    }

  if (entry->symbol) /* FIXME: DEC C generates this */
    {
#if VMS_DEBUG
      vms_debug (4, "EGSD_S_C_SYMG: duplicate \"%s\"\n", symbol->name);
#endif
    }
  else
    {
      entry->symbol = symbol;
      PRIV (gsd_sym_count)++;
      abfd->symcount++;
    }

  return 0;
}

/* Set section VMS flags.  */

void
bfd_vms_set_section_flags (bfd *abfd ATTRIBUTE_UNUSED,
			   asection *sec, flagword flags)
{
  vms_section_data (sec)->vflags = flags;
}

/* Write section and symbol directory of bfd abfd.  */

int
_bfd_vms_write_gsd (bfd *abfd, int objtype ATTRIBUTE_UNUSED)
{
  asection *section;
  asymbol *symbol;
  unsigned int symnum;
  int last_index = -1;
  char dummy_name[10];
  char *sname;
  flagword new_flags, old_flags;
  int abs_section_index = 0;

#if VMS_DEBUG
  vms_debug (2, "vms_write_gsd (%p, %d)\n", abfd, objtype);
#endif

  /* Output sections.  */
  section = abfd->sections;
#if VMS_DEBUG
  vms_debug (3, "%d sections found\n", abfd->section_count);
#endif

  /* Egsd is quadword aligned.  */
  _bfd_vms_output_alignment (abfd, 8);

  _bfd_vms_output_begin (abfd, EOBJ_S_C_EGSD, -1);
  _bfd_vms_output_long (abfd, 0);
  /* Prepare output for subrecords.  */
  _bfd_vms_output_push (abfd);

  while (section != 0)
    {
#if VMS_DEBUG
      vms_debug (3, "Section #%d %s, %d bytes\n", section->index, section->name, (int)section->size);
#endif

      /* Don't write out the VMS debug info section since it is in the
         ETBT and EDBG sections in etir. */
      if (!strcmp (section->name, ".vmsdebug"))
        goto done;

      /* 13 bytes egsd, max 31 chars name -> should be 44 bytes.  */
      if (_bfd_vms_output_check (abfd, 64) < 0)
	{
	  _bfd_vms_output_pop (abfd);
	  _bfd_vms_output_end (abfd);
	  _bfd_vms_output_begin (abfd, EOBJ_S_C_EGSD, -1);
	  _bfd_vms_output_long (abfd, 0);
	  /* Prepare output for subrecords.  */
	  _bfd_vms_output_push (abfd);
	}

      /* Create dummy sections to keep consecutive indices.  */
      while (section->index - last_index > 1)
	{
#if VMS_DEBUG
	  vms_debug (3, "index %d, last %d\n", section->index, last_index);
#endif
	  _bfd_vms_output_begin (abfd, EGSD_S_C_PSC, -1);
	  _bfd_vms_output_short (abfd, 0);
	  _bfd_vms_output_short (abfd, 0);
	  _bfd_vms_output_long (abfd, 0);
	  sprintf (dummy_name, ".DUMMY%02d", last_index);
	  _bfd_vms_output_counted (abfd, dummy_name);
	  _bfd_vms_output_flush (abfd);
	  last_index++;
	}

      /* Don't know if this is necessary for the linker but for now it keeps
	 vms_slurp_gsd happy  */
      sname = (char *)section->name;
      if (*sname == '.')
	{
	  sname++;
	  if ((*sname == 't') && (strcmp (sname, "text") == 0))
	    sname = PRIV (is_vax)?VAX_CODE_NAME:EVAX_CODE_NAME;
	  else if ((*sname == 'd') && (strcmp (sname, "data") == 0))
	    sname = PRIV (is_vax)?VAX_DATA_NAME:EVAX_DATA_NAME;
	  else if ((*sname == 'b') && (strcmp (sname, "bss") == 0))
	    sname = EVAX_BSS_NAME;
	  else if ((*sname == 'l') && (strcmp (sname, "link") == 0))
	    sname = EVAX_LINK_NAME;
	  else if ((*sname == 'r') && (strcmp (sname, "rdata") == 0))
	    sname = EVAX_READONLY_NAME;
	  else if ((*sname == 'l') && (strcmp (sname, "literal") == 0))
	    sname = EVAX_LITERAL_NAME;
	  else if ((*sname == 'l') && (strcmp (sname, "literals") == 0))
	    {
	      sname = EVAX_LITERALS_NAME;
	      abs_section_index = section->index;
	    }
	  else if ((*sname == 'c') && (strcmp (sname, "comm") == 0))
	    sname = EVAX_COMMON_NAME;
	  else if ((*sname == 'l') && (strcmp (sname, "lcomm") == 0))
	    sname = EVAX_LOCAL_NAME;
	}
      else
	sname = _bfd_vms_length_hash_symbol (abfd, sname, EOBJ_S_C_SECSIZ);

      _bfd_vms_output_begin (abfd, EGSD_S_C_PSC, -1);
      _bfd_vms_output_short (abfd, section->alignment_power & 0xff);

      if (bfd_is_com_section (section))
	new_flags = (EGPS_S_V_OVR | EGPS_S_V_REL | EGPS_S_V_GBL | EGPS_S_V_RD
		     | EGPS_S_V_WRT | EGPS_S_V_NOMOD | EGPS_S_V_COM);
      else
	new_flags = vms_esecflag_by_name (evax_section_flags, sname,
					  section->size > 0);

      /* Modify them as directed.  */
      if (section->flags & SEC_READONLY)
	new_flags &= ~EGPS_S_V_WRT;

      new_flags |= vms_section_data (section)->vflags & 0xffff;
      new_flags &=
	~((vms_section_data (section)->vflags >> EGPS_S_V_NO_SHIFT) & 0xffff);

#if VMS_DEBUG
      vms_debug (3, "sec flags %x\n", section->flags);
      vms_debug (3, "new_flags %x, _raw_size %d\n", new_flags, section->size);
#endif

      _bfd_vms_output_short (abfd, new_flags);
      _bfd_vms_output_long (abfd, (unsigned long) section->size);
      _bfd_vms_output_counted (abfd, sname);
      _bfd_vms_output_flush (abfd);

      last_index = section->index;
done:
      section = section->next;
    }

  /* Output symbols.  */
#if VMS_DEBUG
  vms_debug (3, "%d symbols found\n", abfd->symcount);
#endif

  bfd_set_start_address (abfd, (bfd_vma) -1);

  for (symnum = 0; symnum < abfd->symcount; symnum++)
    {
      char *hash;

      symbol = abfd->outsymbols[symnum];
      if (*(symbol->name) == '_')
	{
	  if (strcmp (symbol->name, "__main") == 0)
	    bfd_set_start_address (abfd, (bfd_vma)symbol->value);
	}
      old_flags = symbol->flags;

      if (old_flags & BSF_FILE)
	continue;

      if ((old_flags & BSF_GLOBAL) == 0		   /* Not xdef...  */
	  && !bfd_is_und_section (symbol->section) /* and not xref... */
	  && !((old_flags & BSF_SECTION_SYM) != 0  /* and not LIB$INITIALIZE.  */
	       && strcmp (symbol->section->name, "LIB$INITIALIZE") == 0))
	continue;

      /* 13 bytes egsd, max 64 chars name -> should be 77 bytes.  */
      if (_bfd_vms_output_check (abfd, 80) < 0)
	{
	  _bfd_vms_output_pop (abfd);
	  _bfd_vms_output_end (abfd);
	  _bfd_vms_output_begin (abfd, EOBJ_S_C_EGSD, -1);
	  _bfd_vms_output_long (abfd, 0);
	  /* Prepare output for subrecords.  */
	  _bfd_vms_output_push (abfd);
	}

      _bfd_vms_output_begin (abfd, EGSD_S_C_SYM, -1);

      /* Data type, alignment.  */
      _bfd_vms_output_short (abfd, 0);

      new_flags = 0;

      if (old_flags & BSF_WEAK)
	new_flags |= EGSY_S_V_WEAK;
      if (bfd_is_com_section (symbol->section))		/* .comm  */
	new_flags |= (EGSY_S_V_WEAK | EGSY_S_V_COMM);

      if (old_flags & BSF_FUNCTION)
	{
	  new_flags |= EGSY_S_V_NORM;
	  new_flags |= EGSY_S_V_REL;
	}
      if (old_flags & BSF_GLOBAL)
	{
	  new_flags |= EGSY_S_V_DEF;
	  if (!bfd_is_abs_section (symbol->section))
	    new_flags |= EGSY_S_V_REL;
	}
      _bfd_vms_output_short (abfd, new_flags);

      if (old_flags & BSF_GLOBAL)
	{
	  /* Symbol definition.  */
	  uquad code_address = 0;
	  unsigned long ca_psindx = 0;
	  unsigned long psindx;

	  if ((old_flags & BSF_FUNCTION) && symbol->udata.p != NULL)
	    {
	      asymbol *sym;

	      if (bfd_get_flavour (abfd) == bfd_target_evax_flavour)
	        sym = ((struct evax_private_udata_struct *)symbol->udata.p)->enbsym;
	      else
	        sym = (asymbol *)symbol->udata.p;
	      code_address = sym->value;
	      ca_psindx = sym->section->index;
	    }
	  if (bfd_is_abs_section (symbol->section))
	    psindx = abs_section_index;
	  else
	    psindx = symbol->section->index;

	  _bfd_vms_output_quad (abfd, symbol->value);
	  _bfd_vms_output_quad (abfd, code_address);
	  _bfd_vms_output_long (abfd, ca_psindx);
	  _bfd_vms_output_long (abfd, psindx);
	}
      hash = _bfd_vms_length_hash_symbol (abfd, symbol->name, EOBJ_S_C_SYMSIZ);
      _bfd_vms_output_counted (abfd, hash);

      _bfd_vms_output_flush (abfd);

    }

  _bfd_vms_output_alignment (abfd, 8);
  _bfd_vms_output_pop (abfd);
  _bfd_vms_output_end (abfd);

  return 0;
}
