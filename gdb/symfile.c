/* Generic symbol file reading for the GNU debugger, GDB.

   Copyright (C) 1990-2012 Free Software Foundation, Inc.

   Contributed by Cygnus Support, using pieces from other GDB modules.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "arch-utils.h"
#include "bfdlink.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcore.h"
#include "frame.h"
#include "target.h"
#include "value.h"
#include "symfile.h"
#include "objfiles.h"
#include "source.h"
#include "gdbcmd.h"
#include "breakpoint.h"
#include "language.h"
#include "complaints.h"
#include "demangle.h"
#include "inferior.h"
#include "regcache.h"
#include "filenames.h"		/* for DOSish file names */
#include "gdb-stabs.h"
#include "gdb_obstack.h"
#include "completer.h"
#include "bcache.h"
#include "hashtab.h"
#include "readline/readline.h"
#include "gdb_assert.h"
#include "block.h"
#include "observer.h"
#include "exec.h"
#include "parser-defs.h"
#include "varobj.h"
#include "elf-bfd.h"
#include "solib.h"
#include "remote.h"
#include "stack.h"
#include "gdb_bfd.h"

#include <sys/types.h>
#include <fcntl.h>
#include "gdb_string.h"
#include "gdb_stat.h"
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

#include "psymtab.h"

int (*deprecated_ui_load_progress_hook) (const char *section,
					 unsigned long num);
void (*deprecated_show_load_progress) (const char *section,
			    unsigned long section_sent,
			    unsigned long section_size,
			    unsigned long total_sent,
			    unsigned long total_size);
void (*deprecated_pre_add_symbol_hook) (const char *);
void (*deprecated_post_add_symbol_hook) (void);

static void clear_symtab_users_cleanup (void *ignore);

/* Global variables owned by this file.  */
int readnow_symbol_files;	/* Read full symbols immediately.  */

/* External variables and functions referenced.  */

extern void report_transfer_performance (unsigned long, time_t, time_t);

/* Functions this file defines.  */

static void load_command (char *, int);

static void symbol_file_add_main_1 (char *args, int from_tty, int flags);

static void add_symbol_file_command (char *, int);

bfd *symfile_bfd_open (char *);

int get_section_index (struct objfile *, char *);

static const struct sym_fns *find_sym_fns (bfd *);

static void decrement_reading_symtab (void *);

static void overlay_invalidate_all (void);

void list_overlays_command (char *, int);

void map_overlay_command (char *, int);

void unmap_overlay_command (char *, int);

static void overlay_auto_command (char *, int);

static void overlay_manual_command (char *, int);

static void overlay_off_command (char *, int);

static void overlay_load_command (char *, int);

static void overlay_command (char *, int);

static void simple_free_overlay_table (void);

static void read_target_long_array (CORE_ADDR, unsigned int *, int, int,
				    enum bfd_endian);

static int simple_read_overlay_table (void);

static int simple_overlay_update_1 (struct obj_section *);

static void add_filename_language (char *ext, enum language lang);

static void info_ext_lang_command (char *args, int from_tty);

static void init_filename_language_table (void);

static void symfile_find_segment_sections (struct objfile *objfile);

void _initialize_symfile (void);

/* List of all available sym_fns.  On gdb startup, each object file reader
   calls add_symtab_fns() to register information on each format it is
   prepared to read.  */

typedef const struct sym_fns *sym_fns_ptr;
DEF_VEC_P (sym_fns_ptr);

static VEC (sym_fns_ptr) *symtab_fns = NULL;

/* If non-zero, shared library symbols will be added automatically
   when the inferior is created, new libraries are loaded, or when
   attaching to the inferior.  This is almost always what users will
   want to have happen; but for very large programs, the startup time
   will be excessive, and so if this is a problem, the user can clear
   this flag and then add the shared library symbols as needed.  Note
   that there is a potential for confusion, since if the shared
   library symbols are not loaded, commands like "info fun" will *not*
   report all the functions that are actually present.  */

int auto_solib_add = 1;


/* Make a null terminated copy of the string at PTR with SIZE characters in
   the obstack pointed to by OBSTACKP .  Returns the address of the copy.
   Note that the string at PTR does not have to be null terminated, I.e. it
   may be part of a larger string and we are only saving a substring.  */

char *
obsavestring (const char *ptr, int size, struct obstack *obstackp)
{
  char *p = (char *) obstack_alloc (obstackp, size + 1);
  /* Open-coded memcpy--saves function call time.  These strings are usually
     short.  FIXME: Is this really still true with a compiler that can
     inline memcpy?  */
  {
    const char *p1 = ptr;
    char *p2 = p;
    const char *end = ptr + size;

    while (p1 != end)
      *p2++ = *p1++;
  }
  p[size] = 0;
  return p;
}

/* Concatenate NULL terminated variable argument list of `const char *'
   strings; return the new string.  Space is found in the OBSTACKP.
   Argument list must be terminated by a sentinel expression `(char *)
   NULL'.  */

char *
obconcat (struct obstack *obstackp, ...)
{
  va_list ap;

  va_start (ap, obstackp);
  for (;;)
    {
      const char *s = va_arg (ap, const char *);

      if (s == NULL)
	break;

      obstack_grow_str (obstackp, s);
    }
  va_end (ap);
  obstack_1grow (obstackp, 0);

  return obstack_finish (obstackp);
}

/* True if we are reading a symbol table.  */

int currently_reading_symtab = 0;

static void
decrement_reading_symtab (void *dummy)
{
  currently_reading_symtab--;
}

/* Increment currently_reading_symtab and return a cleanup that can be
   used to decrement it.  */
struct cleanup *
increment_reading_symtab (void)
{
  ++currently_reading_symtab;
  return make_cleanup (decrement_reading_symtab, NULL);
}

/* Remember the lowest-addressed loadable section we've seen.
   This function is called via bfd_map_over_sections.

   In case of equal vmas, the section with the largest size becomes the
   lowest-addressed loadable section.

   If the vmas and sizes are equal, the last section is considered the
   lowest-addressed loadable section.  */

void
find_lowest_section (bfd *abfd, asection *sect, void *obj)
{
  asection **lowest = (asection **) obj;

  if (0 == (bfd_get_section_flags (abfd, sect) & (SEC_ALLOC | SEC_LOAD)))
    return;
  if (!*lowest)
    *lowest = sect;		/* First loadable section */
  else if (bfd_section_vma (abfd, *lowest) > bfd_section_vma (abfd, sect))
    *lowest = sect;		/* A lower loadable section */
  else if (bfd_section_vma (abfd, *lowest) == bfd_section_vma (abfd, sect)
	   && (bfd_section_size (abfd, (*lowest))
	       <= bfd_section_size (abfd, sect)))
    *lowest = sect;
}

/* Create a new section_addr_info, with room for NUM_SECTIONS.  */

struct section_addr_info *
alloc_section_addr_info (size_t num_sections)
{
  struct section_addr_info *sap;
  size_t size;

  size = (sizeof (struct section_addr_info)
	  +  sizeof (struct other_sections) * (num_sections - 1));
  sap = (struct section_addr_info *) xmalloc (size);
  memset (sap, 0, size);
  sap->num_sections = num_sections;

  return sap;
}

/* Build (allocate and populate) a section_addr_info struct from
   an existing section table.  */

extern struct section_addr_info *
build_section_addr_info_from_section_table (const struct target_section *start,
                                            const struct target_section *end)
{
  struct section_addr_info *sap;
  const struct target_section *stp;
  int oidx;

  sap = alloc_section_addr_info (end - start);

  for (stp = start, oidx = 0; stp != end; stp++)
    {
      if (bfd_get_section_flags (stp->bfd,
				 stp->the_bfd_section) & (SEC_ALLOC | SEC_LOAD)
	  && oidx < end - start)
	{
	  sap->other[oidx].addr = stp->addr;
	  sap->other[oidx].name
	    = xstrdup (bfd_section_name (stp->bfd, stp->the_bfd_section));
	  sap->other[oidx].sectindex = stp->the_bfd_section->index;
	  oidx++;
	}
    }

  return sap;
}

/* Create a section_addr_info from section offsets in ABFD.  */

static struct section_addr_info *
build_section_addr_info_from_bfd (bfd *abfd)
{
  struct section_addr_info *sap;
  int i;
  struct bfd_section *sec;

  sap = alloc_section_addr_info (bfd_count_sections (abfd));
  for (i = 0, sec = abfd->sections; sec != NULL; sec = sec->next)
    if (bfd_get_section_flags (abfd, sec) & (SEC_ALLOC | SEC_LOAD))
      {
	sap->other[i].addr = bfd_get_section_vma (abfd, sec);
	sap->other[i].name = xstrdup (bfd_get_section_name (abfd, sec));
	sap->other[i].sectindex = sec->index;
	i++;
      }
  return sap;
}

/* Create a section_addr_info from section offsets in OBJFILE.  */

struct section_addr_info *
build_section_addr_info_from_objfile (const struct objfile *objfile)
{
  struct section_addr_info *sap;
  int i;

  /* Before reread_symbols gets rewritten it is not safe to call:
     gdb_assert (objfile->num_sections == bfd_count_sections (objfile->obfd));
     */
  sap = build_section_addr_info_from_bfd (objfile->obfd);
  for (i = 0; i < sap->num_sections && sap->other[i].name; i++)
    {
      int sectindex = sap->other[i].sectindex;

      sap->other[i].addr += objfile->section_offsets->offsets[sectindex];
    }
  return sap;
}

/* Free all memory allocated by build_section_addr_info_from_section_table.  */

extern void
free_section_addr_info (struct section_addr_info *sap)
{
  int idx;

  for (idx = 0; idx < sap->num_sections; idx++)
    if (sap->other[idx].name)
      xfree (sap->other[idx].name);
  xfree (sap);
}


/* Initialize OBJFILE's sect_index_* members.  */
static void
init_objfile_sect_indices (struct objfile *objfile)
{
  asection *sect;
  int i;

  sect = bfd_get_section_by_name (objfile->obfd, ".text");
  if (sect)
    objfile->sect_index_text = sect->index;

  sect = bfd_get_section_by_name (objfile->obfd, ".data");
  if (sect)
    objfile->sect_index_data = sect->index;

  sect = bfd_get_section_by_name (objfile->obfd, ".bss");
  if (sect)
    objfile->sect_index_bss = sect->index;

  sect = bfd_get_section_by_name (objfile->obfd, ".rodata");
  if (sect)
    objfile->sect_index_rodata = sect->index;

  /* This is where things get really weird...  We MUST have valid
     indices for the various sect_index_* members or gdb will abort.
     So if for example, there is no ".text" section, we have to
     accomodate that.  First, check for a file with the standard
     one or two segments.  */

  symfile_find_segment_sections (objfile);

  /* Except when explicitly adding symbol files at some address,
     section_offsets contains nothing but zeros, so it doesn't matter
     which slot in section_offsets the individual sect_index_* members
     index into.  So if they are all zero, it is safe to just point
     all the currently uninitialized indices to the first slot.  But
     beware: if this is the main executable, it may be relocated
     later, e.g. by the remote qOffsets packet, and then this will
     be wrong!  That's why we try segments first.  */

  for (i = 0; i < objfile->num_sections; i++)
    {
      if (ANOFFSET (objfile->section_offsets, i) != 0)
	{
	  break;
	}
    }
  if (i == objfile->num_sections)
    {
      if (objfile->sect_index_text == -1)
	objfile->sect_index_text = 0;
      if (objfile->sect_index_data == -1)
	objfile->sect_index_data = 0;
      if (objfile->sect_index_bss == -1)
	objfile->sect_index_bss = 0;
      if (objfile->sect_index_rodata == -1)
	objfile->sect_index_rodata = 0;
    }
}

/* The arguments to place_section.  */

struct place_section_arg
{
  struct section_offsets *offsets;
  CORE_ADDR lowest;
};

/* Find a unique offset to use for loadable section SECT if
   the user did not provide an offset.  */

static void
place_section (bfd *abfd, asection *sect, void *obj)
{
  struct place_section_arg *arg = obj;
  CORE_ADDR *offsets = arg->offsets->offsets, start_addr;
  int done;
  ULONGEST align = ((ULONGEST) 1) << bfd_get_section_alignment (abfd, sect);

  /* We are only interested in allocated sections.  */
  if ((bfd_get_section_flags (abfd, sect) & SEC_ALLOC) == 0)
    return;

  /* If the user specified an offset, honor it.  */
  if (offsets[sect->index] != 0)
    return;

  /* Otherwise, let's try to find a place for the section.  */
  start_addr = (arg->lowest + align - 1) & -align;

  do {
    asection *cur_sec;

    done = 1;

    for (cur_sec = abfd->sections; cur_sec != NULL; cur_sec = cur_sec->next)
      {
	int indx = cur_sec->index;

	/* We don't need to compare against ourself.  */
	if (cur_sec == sect)
	  continue;

	/* We can only conflict with allocated sections.  */
	if ((bfd_get_section_flags (abfd, cur_sec) & SEC_ALLOC) == 0)
	  continue;

	/* If the section offset is 0, either the section has not been placed
	   yet, or it was the lowest section placed (in which case LOWEST
	   will be past its end).  */
	if (offsets[indx] == 0)
	  continue;

	/* If this section would overlap us, then we must move up.  */
	if (start_addr + bfd_get_section_size (sect) > offsets[indx]
	    && start_addr < offsets[indx] + bfd_get_section_size (cur_sec))
	  {
	    start_addr = offsets[indx] + bfd_get_section_size (cur_sec);
	    start_addr = (start_addr + align - 1) & -align;
	    done = 0;
	    break;
	  }

	/* Otherwise, we appear to be OK.  So far.  */
      }
    }
  while (!done);

  offsets[sect->index] = start_addr;
  arg->lowest = start_addr + bfd_get_section_size (sect);
}

/* Store struct section_addr_info as prepared (made relative and with SECTINDEX
   filled-in) by addr_info_make_relative into SECTION_OFFSETS of NUM_SECTIONS
   entries.  */

void
relative_addr_info_to_section_offsets (struct section_offsets *section_offsets,
				       int num_sections,
				       struct section_addr_info *addrs)
{
  int i;

  memset (section_offsets, 0, SIZEOF_N_SECTION_OFFSETS (num_sections));

  /* Now calculate offsets for section that were specified by the caller.  */
  for (i = 0; i < addrs->num_sections && addrs->other[i].name; i++)
    {
      struct other_sections *osp;

      osp = &addrs->other[i];
      if (osp->sectindex == -1)
  	continue;

      /* Record all sections in offsets.  */
      /* The section_offsets in the objfile are here filled in using
         the BFD index.  */
      section_offsets->offsets[osp->sectindex] = osp->addr;
    }
}

/* Transform section name S for a name comparison.  prelink can split section
   `.bss' into two sections `.dynbss' and `.bss' (in this order).  Similarly
   prelink can split `.sbss' into `.sdynbss' and `.sbss'.  Use virtual address
   of the new `.dynbss' (`.sdynbss') section as the adjacent new `.bss'
   (`.sbss') section has invalid (increased) virtual address.  */

static const char *
addr_section_name (const char *s)
{
  if (strcmp (s, ".dynbss") == 0)
    return ".bss";
  if (strcmp (s, ".sdynbss") == 0)
    return ".sbss";

  return s;
}

/* qsort comparator for addrs_section_sort.  Sort entries in ascending order by
   their (name, sectindex) pair.  sectindex makes the sort by name stable.  */

static int
addrs_section_compar (const void *ap, const void *bp)
{
  const struct other_sections *a = *((struct other_sections **) ap);
  const struct other_sections *b = *((struct other_sections **) bp);
  int retval;

  retval = strcmp (addr_section_name (a->name), addr_section_name (b->name));
  if (retval)
    return retval;

  return a->sectindex - b->sectindex;
}

/* Provide sorted array of pointers to sections of ADDRS.  The array is
   terminated by NULL.  Caller is responsible to call xfree for it.  */

static struct other_sections **
addrs_section_sort (struct section_addr_info *addrs)
{
  struct other_sections **array;
  int i;

  /* `+ 1' for the NULL terminator.  */
  array = xmalloc (sizeof (*array) * (addrs->num_sections + 1));
  for (i = 0; i < addrs->num_sections && addrs->other[i].name; i++)
    array[i] = &addrs->other[i];
  array[i] = NULL;

  qsort (array, i, sizeof (*array), addrs_section_compar);

  return array;
}

/* Relativize absolute addresses in ADDRS into offsets based on ABFD.  Fill-in
   also SECTINDEXes specific to ABFD there.  This function can be used to
   rebase ADDRS to start referencing different BFD than before.  */

void
addr_info_make_relative (struct section_addr_info *addrs, bfd *abfd)
{
  asection *lower_sect;
  CORE_ADDR lower_offset;
  int i;
  struct cleanup *my_cleanup;
  struct section_addr_info *abfd_addrs;
  struct other_sections **addrs_sorted, **abfd_addrs_sorted;
  struct other_sections **addrs_to_abfd_addrs;

  /* Find lowest loadable section to be used as starting point for
     continguous sections.  */
  lower_sect = NULL;
  bfd_map_over_sections (abfd, find_lowest_section, &lower_sect);
  if (lower_sect == NULL)
    {
      warning (_("no loadable sections found in added symbol-file %s"),
	       bfd_get_filename (abfd));
      lower_offset = 0;
    }
  else
    lower_offset = bfd_section_vma (bfd_get_filename (abfd), lower_sect);

  /* Create ADDRS_TO_ABFD_ADDRS array to map the sections in ADDRS to sections
     in ABFD.  Section names are not unique - there can be multiple sections of
     the same name.  Also the sections of the same name do not have to be
     adjacent to each other.  Some sections may be present only in one of the
     files.  Even sections present in both files do not have to be in the same
     order.

     Use stable sort by name for the sections in both files.  Then linearly
     scan both lists matching as most of the entries as possible.  */

  addrs_sorted = addrs_section_sort (addrs);
  my_cleanup = make_cleanup (xfree, addrs_sorted);

  abfd_addrs = build_section_addr_info_from_bfd (abfd);
  make_cleanup_free_section_addr_info (abfd_addrs);
  abfd_addrs_sorted = addrs_section_sort (abfd_addrs);
  make_cleanup (xfree, abfd_addrs_sorted);

  /* Now create ADDRS_TO_ABFD_ADDRS from ADDRS_SORTED and
     ABFD_ADDRS_SORTED.  */

  addrs_to_abfd_addrs = xzalloc (sizeof (*addrs_to_abfd_addrs)
				 * addrs->num_sections);
  make_cleanup (xfree, addrs_to_abfd_addrs);

  while (*addrs_sorted)
    {
      const char *sect_name = addr_section_name ((*addrs_sorted)->name);

      while (*abfd_addrs_sorted
	     && strcmp (addr_section_name ((*abfd_addrs_sorted)->name),
			sect_name) < 0)
	abfd_addrs_sorted++;

      if (*abfd_addrs_sorted
	  && strcmp (addr_section_name ((*abfd_addrs_sorted)->name),
		     sect_name) == 0)
	{
	  int index_in_addrs;

	  /* Make the found item directly addressable from ADDRS.  */
	  index_in_addrs = *addrs_sorted - addrs->other;
	  gdb_assert (addrs_to_abfd_addrs[index_in_addrs] == NULL);
	  addrs_to_abfd_addrs[index_in_addrs] = *abfd_addrs_sorted;

	  /* Never use the same ABFD entry twice.  */
	  abfd_addrs_sorted++;
	}

      addrs_sorted++;
    }

  /* Calculate offsets for the loadable sections.
     FIXME! Sections must be in order of increasing loadable section
     so that contiguous sections can use the lower-offset!!!

     Adjust offsets if the segments are not contiguous.
     If the section is contiguous, its offset should be set to
     the offset of the highest loadable section lower than it
     (the loadable section directly below it in memory).
     this_offset = lower_offset = lower_addr - lower_orig_addr */

  for (i = 0; i < addrs->num_sections && addrs->other[i].name; i++)
    {
      struct other_sections *sect = addrs_to_abfd_addrs[i];

      if (sect)
	{
	  /* This is the index used by BFD.  */
	  addrs->other[i].sectindex = sect->sectindex;

	  if (addrs->other[i].addr != 0)
	    {
	      addrs->other[i].addr -= sect->addr;
	      lower_offset = addrs->other[i].addr;
	    }
	  else
	    addrs->other[i].addr = lower_offset;
	}
      else
	{
	  /* addr_section_name transformation is not used for SECT_NAME.  */
	  const char *sect_name = addrs->other[i].name;

	  /* This section does not exist in ABFD, which is normally
	     unexpected and we want to issue a warning.

	     However, the ELF prelinker does create a few sections which are
	     marked in the main executable as loadable (they are loaded in
	     memory from the DYNAMIC segment) and yet are not present in
	     separate debug info files.  This is fine, and should not cause
	     a warning.  Shared libraries contain just the section
	     ".gnu.liblist" but it is not marked as loadable there.  There is
	     no other way to identify them than by their name as the sections
	     created by prelink have no special flags.

	     For the sections `.bss' and `.sbss' see addr_section_name.  */

	  if (!(strcmp (sect_name, ".gnu.liblist") == 0
		|| strcmp (sect_name, ".gnu.conflict") == 0
		|| (strcmp (sect_name, ".bss") == 0
		    && i > 0
		    && strcmp (addrs->other[i - 1].name, ".dynbss") == 0
		    && addrs_to_abfd_addrs[i - 1] != NULL)
		|| (strcmp (sect_name, ".sbss") == 0
		    && i > 0
		    && strcmp (addrs->other[i - 1].name, ".sdynbss") == 0
		    && addrs_to_abfd_addrs[i - 1] != NULL)))
	    warning (_("section %s not found in %s"), sect_name,
		     bfd_get_filename (abfd));

	  addrs->other[i].addr = 0;
	  addrs->other[i].sectindex = -1;
	}
    }

  do_cleanups (my_cleanup);
}

/* Parse the user's idea of an offset for dynamic linking, into our idea
   of how to represent it for fast symbol reading.  This is the default
   version of the sym_fns.sym_offsets function for symbol readers that
   don't need to do anything special.  It allocates a section_offsets table
   for the objectfile OBJFILE and stuffs ADDR into all of the offsets.  */

void
default_symfile_offsets (struct objfile *objfile,
			 struct section_addr_info *addrs)
{
  objfile->num_sections = bfd_count_sections (objfile->obfd);
  objfile->section_offsets = (struct section_offsets *)
    obstack_alloc (&objfile->objfile_obstack,
		   SIZEOF_N_SECTION_OFFSETS (objfile->num_sections));
  relative_addr_info_to_section_offsets (objfile->section_offsets,
					 objfile->num_sections, addrs);

  /* For relocatable files, all loadable sections will start at zero.
     The zero is meaningless, so try to pick arbitrary addresses such
     that no loadable sections overlap.  This algorithm is quadratic,
     but the number of sections in a single object file is generally
     small.  */
  if ((bfd_get_file_flags (objfile->obfd) & (EXEC_P | DYNAMIC)) == 0)
    {
      struct place_section_arg arg;
      bfd *abfd = objfile->obfd;
      asection *cur_sec;

      for (cur_sec = abfd->sections; cur_sec != NULL; cur_sec = cur_sec->next)
	/* We do not expect this to happen; just skip this step if the
	   relocatable file has a section with an assigned VMA.  */
	if (bfd_section_vma (abfd, cur_sec) != 0)
	  break;

      if (cur_sec == NULL)
	{
	  CORE_ADDR *offsets = objfile->section_offsets->offsets;

	  /* Pick non-overlapping offsets for sections the user did not
	     place explicitly.  */
	  arg.offsets = objfile->section_offsets;
	  arg.lowest = 0;
	  bfd_map_over_sections (objfile->obfd, place_section, &arg);

	  /* Correctly filling in the section offsets is not quite
	     enough.  Relocatable files have two properties that
	     (most) shared objects do not:

	     - Their debug information will contain relocations.  Some
	     shared libraries do also, but many do not, so this can not
	     be assumed.

	     - If there are multiple code sections they will be loaded
	     at different relative addresses in memory than they are
	     in the objfile, since all sections in the file will start
	     at address zero.

	     Because GDB has very limited ability to map from an
	     address in debug info to the correct code section,
	     it relies on adding SECT_OFF_TEXT to things which might be
	     code.  If we clear all the section offsets, and set the
	     section VMAs instead, then symfile_relocate_debug_section
	     will return meaningful debug information pointing at the
	     correct sections.

	     GDB has too many different data structures for section
	     addresses - a bfd, objfile, and so_list all have section
	     tables, as does exec_ops.  Some of these could probably
	     be eliminated.  */

	  for (cur_sec = abfd->sections; cur_sec != NULL;
	       cur_sec = cur_sec->next)
	    {
	      if ((bfd_get_section_flags (abfd, cur_sec) & SEC_ALLOC) == 0)
		continue;

	      bfd_set_section_vma (abfd, cur_sec, offsets[cur_sec->index]);
	      exec_set_section_address (bfd_get_filename (abfd),
					cur_sec->index,
					offsets[cur_sec->index]);
	      offsets[cur_sec->index] = 0;
	    }
	}
    }

  /* Remember the bfd indexes for the .text, .data, .bss and
     .rodata sections.  */
  init_objfile_sect_indices (objfile);
}


/* Divide the file into segments, which are individual relocatable units.
   This is the default version of the sym_fns.sym_segments function for
   symbol readers that do not have an explicit representation of segments.
   It assumes that object files do not have segments, and fully linked
   files have a single segment.  */

struct symfile_segment_data *
default_symfile_segments (bfd *abfd)
{
  int num_sections, i;
  asection *sect;
  struct symfile_segment_data *data;
  CORE_ADDR low, high;

  /* Relocatable files contain enough information to position each
     loadable section independently; they should not be relocated
     in segments.  */
  if ((bfd_get_file_flags (abfd) & (EXEC_P | DYNAMIC)) == 0)
    return NULL;

  /* Make sure there is at least one loadable section in the file.  */
  for (sect = abfd->sections; sect != NULL; sect = sect->next)
    {
      if ((bfd_get_section_flags (abfd, sect) & SEC_ALLOC) == 0)
	continue;

      break;
    }
  if (sect == NULL)
    return NULL;

  low = bfd_get_section_vma (abfd, sect);
  high = low + bfd_get_section_size (sect);

  data = XZALLOC (struct symfile_segment_data);
  data->num_segments = 1;
  data->segment_bases = XCALLOC (1, CORE_ADDR);
  data->segment_sizes = XCALLOC (1, CORE_ADDR);

  num_sections = bfd_count_sections (abfd);
  data->segment_info = XCALLOC (num_sections, int);

  for (i = 0, sect = abfd->sections; sect != NULL; i++, sect = sect->next)
    {
      CORE_ADDR vma;

      if ((bfd_get_section_flags (abfd, sect) & SEC_ALLOC) == 0)
	continue;

      vma = bfd_get_section_vma (abfd, sect);
      if (vma < low)
	low = vma;
      if (vma + bfd_get_section_size (sect) > high)
	high = vma + bfd_get_section_size (sect);

      data->segment_info[i] = 1;
    }

  data->segment_bases[0] = low;
  data->segment_sizes[0] = high - low;

  return data;
}

/* Process a symbol file, as either the main file or as a dynamically
   loaded file.

   OBJFILE is where the symbols are to be read from.

   ADDRS is the list of section load addresses.  If the user has given
   an 'add-symbol-file' command, then this is the list of offsets and
   addresses he or she provided as arguments to the command; or, if
   we're handling a shared library, these are the actual addresses the
   sections are loaded at, according to the inferior's dynamic linker
   (as gleaned by GDB's shared library code).  We convert each address
   into an offset from the section VMA's as it appears in the object
   file, and then call the file's sym_offsets function to convert this
   into a format-specific offset table --- a `struct section_offsets'.
   If ADDRS is non-zero, OFFSETS must be zero.

   OFFSETS is a table of section offsets already in the right
   format-specific representation.  NUM_OFFSETS is the number of
   elements present in OFFSETS->offsets.  If OFFSETS is non-zero, we
   assume this is the proper table the call to sym_offsets described
   above would produce.  Instead of calling sym_offsets, we just dump
   it right into objfile->section_offsets.  (When we're re-reading
   symbols from an objfile, we don't have the original load address
   list any more; all we have is the section offset table.)  If
   OFFSETS is non-zero, ADDRS must be zero.

   ADD_FLAGS encodes verbosity level, whether this is main symbol or
   an extra symbol file such as dynamically loaded code, and wether
   breakpoint reset should be deferred.  */

void
syms_from_objfile (struct objfile *objfile,
                   struct section_addr_info *addrs,
                   struct section_offsets *offsets,
                   int num_offsets,
		   int add_flags)
{
  struct section_addr_info *local_addr = NULL;
  struct cleanup *old_chain;
  const int mainline = add_flags & SYMFILE_MAINLINE;

  gdb_assert (! (addrs && offsets));

  init_entry_point_info (objfile);
  objfile->sf = find_sym_fns (objfile->obfd);

  if (objfile->sf == NULL)
    return;	/* No symbols.  */

  /* Make sure that partially constructed symbol tables will be cleaned up
     if an error occurs during symbol reading.  */
  old_chain = make_cleanup_free_objfile (objfile);

  /* If ADDRS and OFFSETS are both NULL, put together a dummy address
     list.  We now establish the convention that an addr of zero means
     no load address was specified.  */
  if (! addrs && ! offsets)
    {
      local_addr
	= alloc_section_addr_info (bfd_count_sections (objfile->obfd));
      make_cleanup (xfree, local_addr);
      addrs = local_addr;
    }

  /* Now either addrs or offsets is non-zero.  */

  if (mainline)
    {
      /* We will modify the main symbol table, make sure that all its users
         will be cleaned up if an error occurs during symbol reading.  */
      make_cleanup (clear_symtab_users_cleanup, 0 /*ignore*/);

      /* Since no error yet, throw away the old symbol table.  */

      if (symfile_objfile != NULL)
	{
	  free_objfile (symfile_objfile);
	  gdb_assert (symfile_objfile == NULL);
	}

      /* Currently we keep symbols from the add-symbol-file command.
         If the user wants to get rid of them, they should do "symbol-file"
         without arguments first.  Not sure this is the best behavior
         (PR 2207).  */

      (*objfile->sf->sym_new_init) (objfile);
    }

  /* Convert addr into an offset rather than an absolute address.
     We find the lowest address of a loaded segment in the objfile,
     and assume that <addr> is where that got loaded.

     We no longer warn if the lowest section is not a text segment (as
     happens for the PA64 port.  */
  if (addrs && addrs->other[0].name)
    addr_info_make_relative (addrs, objfile->obfd);

  /* Initialize symbol reading routines for this objfile, allow complaints to
     appear for this new file, and record how verbose to be, then do the
     initial symbol reading for this file.  */

  (*objfile->sf->sym_init) (objfile);
  clear_complaints (&symfile_complaints, 1, add_flags & SYMFILE_VERBOSE);

  if (addrs)
    (*objfile->sf->sym_offsets) (objfile, addrs);
  else
    {
      size_t size = SIZEOF_N_SECTION_OFFSETS (num_offsets);

      /* Just copy in the offset table directly as given to us.  */
      objfile->num_sections = num_offsets;
      objfile->section_offsets
        = ((struct section_offsets *)
           obstack_alloc (&objfile->objfile_obstack, size));
      memcpy (objfile->section_offsets, offsets, size);

      init_objfile_sect_indices (objfile);
    }

  (*objfile->sf->sym_read) (objfile, add_flags);

  if ((add_flags & SYMFILE_NO_READ) == 0)
    require_partial_symbols (objfile, 0);

  /* Discard cleanups as symbol reading was successful.  */

  discard_cleanups (old_chain);
  xfree (local_addr);
}

/* Perform required actions after either reading in the initial
   symbols for a new objfile, or mapping in the symbols from a reusable
   objfile.  ADD_FLAGS is a bitmask of enum symfile_add_flags.  */

void
new_symfile_objfile (struct objfile *objfile, int add_flags)
{
  /* If this is the main symbol file we have to clean up all users of the
     old main symbol file.  Otherwise it is sufficient to fixup all the
     breakpoints that may have been redefined by this symbol file.  */
  if (add_flags & SYMFILE_MAINLINE)
    {
      /* OK, make it the "real" symbol file.  */
      symfile_objfile = objfile;

      clear_symtab_users (add_flags);
    }
  else if ((add_flags & SYMFILE_DEFER_BP_RESET) == 0)
    {
      breakpoint_re_set ();
    }

  /* We're done reading the symbol file; finish off complaints.  */
  clear_complaints (&symfile_complaints, 0, add_flags & SYMFILE_VERBOSE);
}

/* Process a symbol file, as either the main file or as a dynamically
   loaded file.

   ABFD is a BFD already open on the file, as from symfile_bfd_open.
   A new reference is acquired by this function.

   ADD_FLAGS encodes verbosity, whether this is main symbol file or
   extra, such as dynamically loaded code, and what to do with breakpoins.

   ADDRS, OFFSETS, and NUM_OFFSETS are as described for
   syms_from_objfile, above.
   ADDRS is ignored when SYMFILE_MAINLINE bit is set in ADD_FLAGS.

   PARENT is the original objfile if ABFD is a separate debug info file.
   Otherwise PARENT is NULL.

   Upon success, returns a pointer to the objfile that was added.
   Upon failure, jumps back to command level (never returns).  */

static struct objfile *
symbol_file_add_with_addrs_or_offsets (bfd *abfd,
                                       int add_flags,
                                       struct section_addr_info *addrs,
                                       struct section_offsets *offsets,
                                       int num_offsets,
                                       int flags, struct objfile *parent)
{
  struct objfile *objfile;
  const char *name = bfd_get_filename (abfd);
  const int from_tty = add_flags & SYMFILE_VERBOSE;
  const int mainline = add_flags & SYMFILE_MAINLINE;
  const int should_print = ((from_tty || info_verbose)
			    && (readnow_symbol_files
				|| (add_flags & SYMFILE_NO_READ) == 0));

  if (readnow_symbol_files)
    {
      flags |= OBJF_READNOW;
      add_flags &= ~SYMFILE_NO_READ;
    }

  /* Give user a chance to burp if we'd be
     interactively wiping out any existing symbols.  */

  if ((have_full_symbols () || have_partial_symbols ())
      && mainline
      && from_tty
      && !query (_("Load new symbol table from \"%s\"? "), name))
    error (_("Not confirmed."));

  objfile = allocate_objfile (abfd, flags | (mainline ? OBJF_MAINLINE : 0));

  if (parent)
    add_separate_debug_objfile (objfile, parent);

  /* We either created a new mapped symbol table, mapped an existing
     symbol table file which has not had initial symbol reading
     performed, or need to read an unmapped symbol table.  */
  if (should_print)
    {
      if (deprecated_pre_add_symbol_hook)
	deprecated_pre_add_symbol_hook (name);
      else
	{
	  printf_unfiltered (_("Reading symbols from %s..."), name);
	  wrap_here ("");
	  gdb_flush (gdb_stdout);
	}
    }
  syms_from_objfile (objfile, addrs, offsets, num_offsets,
		     add_flags);

  /* We now have at least a partial symbol table.  Check to see if the
     user requested that all symbols be read on initial access via either
     the gdb startup command line or on a per symbol file basis.  Expand
     all partial symbol tables for this objfile if so.  */

  if ((flags & OBJF_READNOW))
    {
      if (should_print)
	{
	  printf_unfiltered (_("expanding to full symbols..."));
	  wrap_here ("");
	  gdb_flush (gdb_stdout);
	}

      if (objfile->sf)
	objfile->sf->qf->expand_all_symtabs (objfile);
    }

  if (should_print && !objfile_has_symbols (objfile))
    {
      wrap_here ("");
      printf_unfiltered (_("(no debugging symbols found)..."));
      wrap_here ("");
    }

  if (should_print)
    {
      if (deprecated_post_add_symbol_hook)
	deprecated_post_add_symbol_hook ();
      else
	printf_unfiltered (_("done.\n"));
    }

  /* We print some messages regardless of whether 'from_tty ||
     info_verbose' is true, so make sure they go out at the right
     time.  */
  gdb_flush (gdb_stdout);

  if (objfile->sf == NULL)
    {
      observer_notify_new_objfile (objfile);
      return objfile;	/* No symbols.  */
    }

  new_symfile_objfile (objfile, add_flags);

  observer_notify_new_objfile (objfile);

  bfd_cache_close_all ();
  return (objfile);
}

/* Add BFD as a separate debug file for OBJFILE.  */

void
symbol_file_add_separate (bfd *bfd, int symfile_flags, struct objfile *objfile)
{
  struct objfile *new_objfile;
  struct section_addr_info *sap;
  struct cleanup *my_cleanup;

  /* Create section_addr_info.  We can't directly use offsets from OBJFILE
     because sections of BFD may not match sections of OBJFILE and because
     vma may have been modified by tools such as prelink.  */
  sap = build_section_addr_info_from_objfile (objfile);
  my_cleanup = make_cleanup_free_section_addr_info (sap);

  new_objfile = symbol_file_add_with_addrs_or_offsets
    (bfd, symfile_flags,
     sap, NULL, 0,
     objfile->flags & (OBJF_REORDERED | OBJF_SHARED | OBJF_READNOW
		       | OBJF_USERLOADED),
     objfile);

  do_cleanups (my_cleanup);
}

/* Process the symbol file ABFD, as either the main file or as a
   dynamically loaded file.

   See symbol_file_add_with_addrs_or_offsets's comments for
   details.  */
struct objfile *
symbol_file_add_from_bfd (bfd *abfd, int add_flags,
                          struct section_addr_info *addrs,
                          int flags, struct objfile *parent)
{
  return symbol_file_add_with_addrs_or_offsets (abfd, add_flags, addrs, 0, 0,
                                                flags, parent);
}


/* Process a symbol file, as either the main file or as a dynamically
   loaded file.  See symbol_file_add_with_addrs_or_offsets's comments
   for details.  */
struct objfile *
symbol_file_add (char *name, int add_flags, struct section_addr_info *addrs,
		 int flags)
{
  bfd *bfd = symfile_bfd_open (name);
  struct cleanup *cleanup = make_cleanup_bfd_unref (bfd);
  struct objfile *objf;

  objf = symbol_file_add_from_bfd (bfd, add_flags, addrs, flags, NULL);
  do_cleanups (cleanup);
  return objf;
}


/* Call symbol_file_add() with default values and update whatever is
   affected by the loading of a new main().
   Used when the file is supplied in the gdb command line
   and by some targets with special loading requirements.
   The auxiliary function, symbol_file_add_main_1(), has the flags
   argument for the switches that can only be specified in the symbol_file
   command itself.  */

void
symbol_file_add_main (char *args, int from_tty)
{
  symbol_file_add_main_1 (args, from_tty, 0);
}

static void
symbol_file_add_main_1 (char *args, int from_tty, int flags)
{
  const int add_flags = (current_inferior ()->symfile_flags
			 | SYMFILE_MAINLINE | (from_tty ? SYMFILE_VERBOSE : 0));

  symbol_file_add (args, add_flags, NULL, flags);

  /* Getting new symbols may change our opinion about
     what is frameless.  */
  reinit_frame_cache ();

  if ((flags & SYMFILE_NO_READ) == 0)
    set_initial_language ();
}

void
symbol_file_clear (int from_tty)
{
  if ((have_full_symbols () || have_partial_symbols ())
      && from_tty
      && (symfile_objfile
	  ? !query (_("Discard symbol table from `%s'? "),
		    symfile_objfile->name)
	  : !query (_("Discard symbol table? "))))
    error (_("Not confirmed."));

  /* solib descriptors may have handles to objfiles.  Wipe them before their
     objfiles get stale by free_all_objfiles.  */
  no_shared_libraries (NULL, from_tty);

  free_all_objfiles ();

  gdb_assert (symfile_objfile == NULL);
  if (from_tty)
    printf_unfiltered (_("No symbol file now.\n"));
}

static char *
get_debug_link_info (struct objfile *objfile, unsigned long *crc32_out)
{
  asection *sect;
  bfd_size_type debuglink_size;
  unsigned long crc32;
  char *contents;
  int crc_offset;

  sect = bfd_get_section_by_name (objfile->obfd, ".gnu_debuglink");

  if (sect == NULL)
    return NULL;

  debuglink_size = bfd_section_size (objfile->obfd, sect);

  contents = xmalloc (debuglink_size);
  bfd_get_section_contents (objfile->obfd, sect, contents,
			    (file_ptr)0, (bfd_size_type)debuglink_size);

  /* Crc value is stored after the filename, aligned up to 4 bytes.  */
  crc_offset = strlen (contents) + 1;
  crc_offset = (crc_offset + 3) & ~3;

  crc32 = bfd_get_32 (objfile->obfd, (bfd_byte *) (contents + crc_offset));

  *crc32_out = crc32;
  return contents;
}

/* Return 32-bit CRC for ABFD.  If successful store it to *FILE_CRC_RETURN and
   return 1.  Otherwise print a warning and return 0.  ABFD seek position is
   not preserved.  */

static int
get_file_crc (bfd *abfd, unsigned long *file_crc_return)
{
  unsigned long file_crc = 0;

  if (bfd_seek (abfd, 0, SEEK_SET) != 0)
    {
      warning (_("Problem reading \"%s\" for CRC: %s"),
	       bfd_get_filename (abfd), bfd_errmsg (bfd_get_error ()));
      return 0;
    }

  for (;;)
    {
      gdb_byte buffer[8 * 1024];
      bfd_size_type count;

      count = bfd_bread (buffer, sizeof (buffer), abfd);
      if (count == (bfd_size_type) -1)
	{
	  warning (_("Problem reading \"%s\" for CRC: %s"),
		   bfd_get_filename (abfd), bfd_errmsg (bfd_get_error ()));
	  return 0;
	}
      if (count == 0)
	break;
      file_crc = gnu_debuglink_crc32 (file_crc, buffer, count);
    }

  *file_crc_return = file_crc;
  return 1;
}

static int
separate_debug_file_exists (const char *name, unsigned long crc,
			    struct objfile *parent_objfile)
{
  unsigned long file_crc;
  int file_crc_p;
  bfd *abfd;
  struct stat parent_stat, abfd_stat;
  int verified_as_different;

  /* Find a separate debug info file as if symbols would be present in
     PARENT_OBJFILE itself this function would not be called.  .gnu_debuglink
     section can contain just the basename of PARENT_OBJFILE without any
     ".debug" suffix as "/usr/lib/debug/path/to/file" is a separate tree where
     the separate debug infos with the same basename can exist.  */

  if (filename_cmp (name, parent_objfile->name) == 0)
    return 0;

  abfd = gdb_bfd_open_maybe_remote (name);

  if (!abfd)
    return 0;

  /* Verify symlinks were not the cause of filename_cmp name difference above.

     Some operating systems, e.g. Windows, do not provide a meaningful
     st_ino; they always set it to zero.  (Windows does provide a
     meaningful st_dev.)  Do not indicate a duplicate library in that
     case.  While there is no guarantee that a system that provides
     meaningful inode numbers will never set st_ino to zero, this is
     merely an optimization, so we do not need to worry about false
     negatives.  */

  if (bfd_stat (abfd, &abfd_stat) == 0
      && abfd_stat.st_ino != 0
      && bfd_stat (parent_objfile->obfd, &parent_stat) == 0)
    {
      if (abfd_stat.st_dev == parent_stat.st_dev
	  && abfd_stat.st_ino == parent_stat.st_ino)
	{
	  gdb_bfd_unref (abfd);
	  return 0;
	}
      verified_as_different = 1;
    }
  else
    verified_as_different = 0;

  file_crc_p = get_file_crc (abfd, &file_crc);

  gdb_bfd_unref (abfd);

  if (!file_crc_p)
    return 0;

  if (crc != file_crc)
    {
      /* If one (or both) the files are accessed for example the via "remote:"
	 gdbserver way it does not support the bfd_stat operation.  Verify
	 whether those two files are not the same manually.  */

      if (!verified_as_different && !parent_objfile->crc32_p)
	{
	  parent_objfile->crc32_p = get_file_crc (parent_objfile->obfd,
						  &parent_objfile->crc32);
	  if (!parent_objfile->crc32_p)
	    return 0;
	}

      if (verified_as_different || parent_objfile->crc32 != file_crc)
	warning (_("the debug information found in \"%s\""
		   " does not match \"%s\" (CRC mismatch).\n"),
		 name, parent_objfile->name);

      return 0;
    }

  return 1;
}

char *debug_file_directory = NULL;
static void
show_debug_file_directory (struct ui_file *file, int from_tty,
			   struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file,
		    _("The directory where separate debug "
		      "symbols are searched for is \"%s\".\n"),
		    value);
}

#if ! defined (DEBUG_SUBDIRECTORY)
#define DEBUG_SUBDIRECTORY ".debug"
#endif

/* Find a separate debuginfo file for OBJFILE, using DIR as the directory
   where the original file resides (may not be the same as
   dirname(objfile->name) due to symlinks), and DEBUGLINK as the file we are
   looking for.  Returns the name of the debuginfo, of NULL.  */

static char *
find_separate_debug_file (const char *dir,
			  const char *canon_dir,
			  const char *debuglink,
			  unsigned long crc32, struct objfile *objfile)
{
  char *debugdir;
  char *debugfile;
  int i;
  VEC (char_ptr) *debugdir_vec;
  struct cleanup *back_to;
  int ix;

  /* Set I to max (strlen (canon_dir), strlen (dir)).  */
  i = strlen (dir);
  if (canon_dir != NULL && strlen (canon_dir) > i)
    i = strlen (canon_dir);

  debugfile = xmalloc (strlen (debug_file_directory) + 1
		       + i
		       + strlen (DEBUG_SUBDIRECTORY)
		       + strlen ("/")
		       + strlen (debuglink)
		       + 1);

  /* First try in the same directory as the original file.  */
  strcpy (debugfile, dir);
  strcat (debugfile, debuglink);

  if (separate_debug_file_exists (debugfile, crc32, objfile))
    return debugfile;

  /* Then try in the subdirectory named DEBUG_SUBDIRECTORY.  */
  strcpy (debugfile, dir);
  strcat (debugfile, DEBUG_SUBDIRECTORY);
  strcat (debugfile, "/");
  strcat (debugfile, debuglink);

  if (separate_debug_file_exists (debugfile, crc32, objfile))
    return debugfile;

  /* Then try in the global debugfile directories.

     Keep backward compatibility so that DEBUG_FILE_DIRECTORY being "" will
     cause "/..." lookups.  */

  debugdir_vec = dirnames_to_char_ptr_vec (debug_file_directory);
  back_to = make_cleanup_free_char_ptr_vec (debugdir_vec);

  for (ix = 0; VEC_iterate (char_ptr, debugdir_vec, ix, debugdir); ++ix)
    {
      strcpy (debugfile, debugdir);
      strcat (debugfile, "/");
      strcat (debugfile, dir);
      strcat (debugfile, debuglink);

      if (separate_debug_file_exists (debugfile, crc32, objfile))
	return debugfile;

      /* If the file is in the sysroot, try using its base path in the
	 global debugfile directory.  */
      if (canon_dir != NULL
	  && filename_ncmp (canon_dir, gdb_sysroot,
			    strlen (gdb_sysroot)) == 0
	  && IS_DIR_SEPARATOR (canon_dir[strlen (gdb_sysroot)]))
	{
	  strcpy (debugfile, debugdir);
	  strcat (debugfile, canon_dir + strlen (gdb_sysroot));
	  strcat (debugfile, "/");
	  strcat (debugfile, debuglink);

	  if (separate_debug_file_exists (debugfile, crc32, objfile))
	    return debugfile;
	}
    }

  do_cleanups (back_to);
  xfree (debugfile);
  return NULL;
}

/* Modify PATH to contain only "directory/" part of PATH.
   If there were no directory separators in PATH, PATH will be empty
   string on return.  */

static void
terminate_after_last_dir_separator (char *path)
{
  int i;

  /* Strip off the final filename part, leaving the directory name,
     followed by a slash.  The directory can be relative or absolute.  */
  for (i = strlen(path) - 1; i >= 0; i--)
    if (IS_DIR_SEPARATOR (path[i]))
      break;

  /* If I is -1 then no directory is present there and DIR will be "".  */
  path[i + 1] = '\0';
}

/* Find separate debuginfo for OBJFILE (using .gnu_debuglink section).
   Returns pathname, or NULL.  */

char *
find_separate_debug_file_by_debuglink (struct objfile *objfile)
{
  char *debuglink;
  char *dir, *canon_dir;
  char *debugfile;
  unsigned long crc32;
  struct cleanup *cleanups;

  debuglink = get_debug_link_info (objfile, &crc32);

  if (debuglink == NULL)
    {
      /* There's no separate debug info, hence there's no way we could
	 load it => no warning.  */
      return NULL;
    }

  cleanups = make_cleanup (xfree, debuglink);
  dir = xstrdup (objfile->name);
  make_cleanup (xfree, dir);
  terminate_after_last_dir_separator (dir);
  canon_dir = lrealpath (dir);

  debugfile = find_separate_debug_file (dir, canon_dir, debuglink,
					crc32, objfile);
  xfree (canon_dir);

  if (debugfile == NULL)
    {
#ifdef HAVE_LSTAT
      /* For PR gdb/9538, try again with realpath (if different from the
	 original).  */

      struct stat st_buf;

      if (lstat (objfile->name, &st_buf) == 0 && S_ISLNK(st_buf.st_mode))
	{
	  char *symlink_dir;

	  symlink_dir = lrealpath (objfile->name);
	  if (symlink_dir != NULL)
	    {
	      make_cleanup (xfree, symlink_dir);
	      terminate_after_last_dir_separator (symlink_dir);
	      if (strcmp (dir, symlink_dir) != 0)
		{
		  /* Different directory, so try using it.  */
		  debugfile = find_separate_debug_file (symlink_dir,
							symlink_dir,
							debuglink,
							crc32,
							objfile);
		}
	    }
	}
#endif  /* HAVE_LSTAT  */
    }

  do_cleanups (cleanups);
  return debugfile;
}


/* This is the symbol-file command.  Read the file, analyze its
   symbols, and add a struct symtab to a symtab list.  The syntax of
   the command is rather bizarre:

   1. The function buildargv implements various quoting conventions
   which are undocumented and have little or nothing in common with
   the way things are quoted (or not quoted) elsewhere in GDB.

   2. Options are used, which are not generally used in GDB (perhaps
   "set mapped on", "set readnow on" would be better)

   3. The order of options matters, which is contrary to GNU
   conventions (because it is confusing and inconvenient).  */

void
symbol_file_command (char *args, int from_tty)
{
  dont_repeat ();

  if (args == NULL)
    {
      symbol_file_clear (from_tty);
    }
  else
    {
      char **argv = gdb_buildargv (args);
      int flags = OBJF_USERLOADED;
      struct cleanup *cleanups;
      char *name = NULL;

      cleanups = make_cleanup_freeargv (argv);
      while (*argv != NULL)
	{
	  if (strcmp (*argv, "-readnow") == 0)
	    flags |= OBJF_READNOW;
	  else if (**argv == '-')
	    error (_("unknown option `%s'"), *argv);
	  else
	    {
	      symbol_file_add_main_1 (*argv, from_tty, flags);
	      name = *argv;
	    }

	  argv++;
	}

      if (name == NULL)
	error (_("no symbol file name was specified"));

      do_cleanups (cleanups);
    }
}

/* Set the initial language.

   FIXME: A better solution would be to record the language in the
   psymtab when reading partial symbols, and then use it (if known) to
   set the language.  This would be a win for formats that encode the
   language in an easily discoverable place, such as DWARF.  For
   stabs, we can jump through hoops looking for specially named
   symbols or try to intuit the language from the specific type of
   stabs we find, but we can't do that until later when we read in
   full symbols.  */

void
set_initial_language (void)
{
  enum language lang = language_unknown;

  if (language_of_main != language_unknown)
    lang = language_of_main;
  else
    {
      const char *filename;

      filename = find_main_filename ();
      if (filename != NULL)
	lang = deduce_language_from_filename (filename);
    }

  if (lang == language_unknown)
    {
      /* Make C the default language */
      lang = language_c;
    }

  set_language (lang);
  expected_language = current_language; /* Don't warn the user.  */
}

/* If NAME is a remote name open the file using remote protocol, otherwise
   open it normally.  Returns a new reference to the BFD.  On error,
   returns NULL with the BFD error set.  */

bfd *
gdb_bfd_open_maybe_remote (const char *name)
{
  bfd *result;

  if (remote_filename_p (name))
    result = remote_bfd_open (name, gnutarget);
  else
    result = gdb_bfd_openr (name, gnutarget);

  return result;
}


/* Open the file specified by NAME and hand it off to BFD for
   preliminary analysis.  Return a newly initialized bfd *, which
   includes a newly malloc'd` copy of NAME (tilde-expanded and made
   absolute).  In case of trouble, error() is called.  */

bfd *
symfile_bfd_open (char *name)
{
  bfd *sym_bfd;
  int desc;
  char *absolute_name;

  if (remote_filename_p (name))
    {
      sym_bfd = remote_bfd_open (name, gnutarget);
      if (!sym_bfd)
	error (_("`%s': can't open to read symbols: %s."), name,
	       bfd_errmsg (bfd_get_error ()));

      if (!bfd_check_format (sym_bfd, bfd_object))
	{
	  make_cleanup_bfd_unref (sym_bfd);
	  error (_("`%s': can't read symbols: %s."), name,
		 bfd_errmsg (bfd_get_error ()));
	}

      return sym_bfd;
    }

  name = tilde_expand (name);	/* Returns 1st new malloc'd copy.  */

  /* Look down path for it, allocate 2nd new malloc'd copy.  */
  desc = openp (getenv ("PATH"), OPF_TRY_CWD_FIRST, name,
		O_RDONLY | O_BINARY, &absolute_name);
#if defined(__GO32__) || defined(_WIN32) || defined (__CYGWIN__)
  if (desc < 0)
    {
      char *exename = alloca (strlen (name) + 5);

      strcat (strcpy (exename, name), ".exe");
      desc = openp (getenv ("PATH"), OPF_TRY_CWD_FIRST, exename,
		    O_RDONLY | O_BINARY, &absolute_name);
    }
#endif
  if (desc < 0)
    {
      make_cleanup (xfree, name);
      perror_with_name (name);
    }

  xfree (name);
  name = absolute_name;
  make_cleanup (xfree, name);

  sym_bfd = gdb_bfd_fopen (name, gnutarget, FOPEN_RB, desc);
  if (!sym_bfd)
    {
      make_cleanup (xfree, name);
      error (_("`%s': can't open to read symbols: %s."), name,
	     bfd_errmsg (bfd_get_error ()));
    }
  bfd_set_cacheable (sym_bfd, 1);

  if (!bfd_check_format (sym_bfd, bfd_object))
    {
      make_cleanup_bfd_unref (sym_bfd);
      error (_("`%s': can't read symbols: %s."), name,
	     bfd_errmsg (bfd_get_error ()));
    }

  return sym_bfd;
}

/* Return the section index for SECTION_NAME on OBJFILE.  Return -1 if
   the section was not found.  */

int
get_section_index (struct objfile *objfile, char *section_name)
{
  asection *sect = bfd_get_section_by_name (objfile->obfd, section_name);

  if (sect)
    return sect->index;
  else
    return -1;
}

/* Link SF into the global symtab_fns list.  Called on startup by the
   _initialize routine in each object file format reader, to register
   information about each format the reader is prepared to handle.  */

void
add_symtab_fns (const struct sym_fns *sf)
{
  VEC_safe_push (sym_fns_ptr, symtab_fns, sf);
}

/* Initialize OBJFILE to read symbols from its associated BFD.  It
   either returns or calls error().  The result is an initialized
   struct sym_fns in the objfile structure, that contains cached
   information about the symbol file.  */

static const struct sym_fns *
find_sym_fns (bfd *abfd)
{
  const struct sym_fns *sf;
  enum bfd_flavour our_flavour = bfd_get_flavour (abfd);
  int i;

  if (our_flavour == bfd_target_srec_flavour
      || our_flavour == bfd_target_ihex_flavour
      || our_flavour == bfd_target_tekhex_flavour)
    return NULL;	/* No symbols.  */

  for (i = 0; VEC_iterate (sym_fns_ptr, symtab_fns, i, sf); ++i)
    if (our_flavour == sf->sym_flavour)
      return sf;

  error (_("I'm sorry, Dave, I can't do that.  Symbol format `%s' unknown."),
	 bfd_get_target (abfd));
}


/* This function runs the load command of our current target.  */

static void
load_command (char *arg, int from_tty)
{
  dont_repeat ();

  /* The user might be reloading because the binary has changed.  Take
     this opportunity to check.  */
  reopen_exec_file ();
  reread_symbols ();

  if (arg == NULL)
    {
      char *parg;
      int count = 0;

      parg = arg = get_exec_file (1);

      /* Count how many \ " ' tab space there are in the name.  */
      while ((parg = strpbrk (parg, "\\\"'\t ")))
	{
	  parg++;
	  count++;
	}

      if (count)
	{
	  /* We need to quote this string so buildargv can pull it apart.  */
	  char *temp = xmalloc (strlen (arg) + count + 1 );
	  char *ptemp = temp;
	  char *prev;

	  make_cleanup (xfree, temp);

	  prev = parg = arg;
	  while ((parg = strpbrk (parg, "\\\"'\t ")))
	    {
	      strncpy (ptemp, prev, parg - prev);
	      ptemp += parg - prev;
	      prev = parg++;
	      *ptemp++ = '\\';
	    }
	  strcpy (ptemp, prev);

	  arg = temp;
	}
    }

  target_load (arg, from_tty);

  /* After re-loading the executable, we don't really know which
     overlays are mapped any more.  */
  overlay_cache_invalid = 1;
}

/* This version of "load" should be usable for any target.  Currently
   it is just used for remote targets, not inftarg.c or core files,
   on the theory that only in that case is it useful.

   Avoiding xmodem and the like seems like a win (a) because we don't have
   to worry about finding it, and (b) On VMS, fork() is very slow and so
   we don't want to run a subprocess.  On the other hand, I'm not sure how
   performance compares.  */

static int validate_download = 0;

/* Callback service function for generic_load (bfd_map_over_sections).  */

static void
add_section_size_callback (bfd *abfd, asection *asec, void *data)
{
  bfd_size_type *sum = data;

  *sum += bfd_get_section_size (asec);
}

/* Opaque data for load_section_callback.  */
struct load_section_data {
  unsigned long load_offset;
  struct load_progress_data *progress_data;
  VEC(memory_write_request_s) *requests;
};

/* Opaque data for load_progress.  */
struct load_progress_data {
  /* Cumulative data.  */
  unsigned long write_count;
  unsigned long data_count;
  bfd_size_type total_size;
};

/* Opaque data for load_progress for a single section.  */
struct load_progress_section_data {
  struct load_progress_data *cumulative;

  /* Per-section data.  */
  const char *section_name;
  ULONGEST section_sent;
  ULONGEST section_size;
  CORE_ADDR lma;
  gdb_byte *buffer;
};

/* Target write callback routine for progress reporting.  */

static void
load_progress (ULONGEST bytes, void *untyped_arg)
{
  struct load_progress_section_data *args = untyped_arg;
  struct load_progress_data *totals;

  if (args == NULL)
    /* Writing padding data.  No easy way to get at the cumulative
       stats, so just ignore this.  */
    return;

  totals = args->cumulative;

  if (bytes == 0 && args->section_sent == 0)
    {
      /* The write is just starting.  Let the user know we've started
	 this section.  */
      ui_out_message (current_uiout, 0, "Loading section %s, size %s lma %s\n",
		      args->section_name, hex_string (args->section_size),
		      paddress (target_gdbarch, args->lma));
      return;
    }

  if (validate_download)
    {
      /* Broken memories and broken monitors manifest themselves here
	 when bring new computers to life.  This doubles already slow
	 downloads.  */
      /* NOTE: cagney/1999-10-18: A more efficient implementation
	 might add a verify_memory() method to the target vector and
	 then use that.  remote.c could implement that method using
	 the ``qCRC'' packet.  */
      gdb_byte *check = xmalloc (bytes);
      struct cleanup *verify_cleanups = make_cleanup (xfree, check);

      if (target_read_memory (args->lma, check, bytes) != 0)
	error (_("Download verify read failed at %s"),
	       paddress (target_gdbarch, args->lma));
      if (memcmp (args->buffer, check, bytes) != 0)
	error (_("Download verify compare failed at %s"),
	       paddress (target_gdbarch, args->lma));
      do_cleanups (verify_cleanups);
    }
  totals->data_count += bytes;
  args->lma += bytes;
  args->buffer += bytes;
  totals->write_count += 1;
  args->section_sent += bytes;
  if (quit_flag
      || (deprecated_ui_load_progress_hook != NULL
	  && deprecated_ui_load_progress_hook (args->section_name,
					       args->section_sent)))
    error (_("Canceled the download"));

  if (deprecated_show_load_progress != NULL)
    deprecated_show_load_progress (args->section_name,
				   args->section_sent,
				   args->section_size,
				   totals->data_count,
				   totals->total_size);
}

/* Callback service function for generic_load (bfd_map_over_sections).  */

static void
load_section_callback (bfd *abfd, asection *asec, void *data)
{
  struct memory_write_request *new_request;
  struct load_section_data *args = data;
  struct load_progress_section_data *section_data;
  bfd_size_type size = bfd_get_section_size (asec);
  gdb_byte *buffer;
  const char *sect_name = bfd_get_section_name (abfd, asec);

  if ((bfd_get_section_flags (abfd, asec) & SEC_LOAD) == 0)
    return;

  if (size == 0)
    return;

  new_request = VEC_safe_push (memory_write_request_s,
			       args->requests, NULL);
  memset (new_request, 0, sizeof (struct memory_write_request));
  section_data = xcalloc (1, sizeof (struct load_progress_section_data));
  new_request->begin = bfd_section_lma (abfd, asec) + args->load_offset;
  new_request->end = new_request->begin + size; /* FIXME Should size
						   be in instead?  */
  new_request->data = xmalloc (size);
  new_request->baton = section_data;

  buffer = new_request->data;

  section_data->cumulative = args->progress_data;
  section_data->section_name = sect_name;
  section_data->section_size = size;
  section_data->lma = new_request->begin;
  section_data->buffer = buffer;

  bfd_get_section_contents (abfd, asec, buffer, 0, size);
}

/* Clean up an entire memory request vector, including load
   data and progress records.  */

static void
clear_memory_write_data (void *arg)
{
  VEC(memory_write_request_s) **vec_p = arg;
  VEC(memory_write_request_s) *vec = *vec_p;
  int i;
  struct memory_write_request *mr;

  for (i = 0; VEC_iterate (memory_write_request_s, vec, i, mr); ++i)
    {
      xfree (mr->data);
      xfree (mr->baton);
    }
  VEC_free (memory_write_request_s, vec);
}

void
generic_load (char *args, int from_tty)
{
  bfd *loadfile_bfd;
  struct timeval start_time, end_time;
  char *filename;
  struct cleanup *old_cleanups = make_cleanup (null_cleanup, 0);
  struct load_section_data cbdata;
  struct load_progress_data total_progress;
  struct ui_out *uiout = current_uiout;

  CORE_ADDR entry;
  char **argv;

  memset (&cbdata, 0, sizeof (cbdata));
  memset (&total_progress, 0, sizeof (total_progress));
  cbdata.progress_data = &total_progress;

  make_cleanup (clear_memory_write_data, &cbdata.requests);

  if (args == NULL)
    error_no_arg (_("file to load"));

  argv = gdb_buildargv (args);
  make_cleanup_freeargv (argv);

  filename = tilde_expand (argv[0]);
  make_cleanup (xfree, filename);

  if (argv[1] != NULL)
    {
      char *endptr;

      cbdata.load_offset = strtoul (argv[1], &endptr, 0);

      /* If the last word was not a valid number then
         treat it as a file name with spaces in.  */
      if (argv[1] == endptr)
        error (_("Invalid download offset:%s."), argv[1]);

      if (argv[2] != NULL)
	error (_("Too many parameters."));
    }

  /* Open the file for loading.  */
  loadfile_bfd = gdb_bfd_openr (filename, gnutarget);
  if (loadfile_bfd == NULL)
    {
      perror_with_name (filename);
      return;
    }

  make_cleanup_bfd_unref (loadfile_bfd);

  if (!bfd_check_format (loadfile_bfd, bfd_object))
    {
      error (_("\"%s\" is not an object file: %s"), filename,
	     bfd_errmsg (bfd_get_error ()));
    }

  bfd_map_over_sections (loadfile_bfd, add_section_size_callback,
			 (void *) &total_progress.total_size);

  bfd_map_over_sections (loadfile_bfd, load_section_callback, &cbdata);

  gettimeofday (&start_time, NULL);

  if (target_write_memory_blocks (cbdata.requests, flash_discard,
				  load_progress) != 0)
    error (_("Load failed"));

  gettimeofday (&end_time, NULL);

  entry = bfd_get_start_address (loadfile_bfd);
  ui_out_text (uiout, "Start address ");
  ui_out_field_fmt (uiout, "address", "%s", paddress (target_gdbarch, entry));
  ui_out_text (uiout, ", load size ");
  ui_out_field_fmt (uiout, "load-size", "%lu", total_progress.data_count);
  ui_out_text (uiout, "\n");
  /* We were doing this in remote-mips.c, I suspect it is right
     for other targets too.  */
  regcache_write_pc (get_current_regcache (), entry);

  /* Reset breakpoints, now that we have changed the load image.  For
     instance, breakpoints may have been set (or reset, by
     post_create_inferior) while connected to the target but before we
     loaded the program.  In that case, the prologue analyzer could
     have read instructions from the target to find the right
     breakpoint locations.  Loading has changed the contents of that
     memory.  */

  breakpoint_re_set ();

  /* FIXME: are we supposed to call symbol_file_add or not?  According
     to a comment from remote-mips.c (where a call to symbol_file_add
     was commented out), making the call confuses GDB if more than one
     file is loaded in.  Some targets do (e.g., remote-vx.c) but
     others don't (or didn't - perhaps they have all been deleted).  */

  print_transfer_performance (gdb_stdout, total_progress.data_count,
			      total_progress.write_count,
			      &start_time, &end_time);

  do_cleanups (old_cleanups);
}

/* Report how fast the transfer went.  */

/* DEPRECATED: cagney/1999-10-18: report_transfer_performance is being
   replaced by print_transfer_performance (with a very different
   function signature).  */

void
report_transfer_performance (unsigned long data_count, time_t start_time,
			     time_t end_time)
{
  struct timeval start, end;

  start.tv_sec = start_time;
  start.tv_usec = 0;
  end.tv_sec = end_time;
  end.tv_usec = 0;

  print_transfer_performance (gdb_stdout, data_count, 0, &start, &end);
}

void
print_transfer_performance (struct ui_file *stream,
			    unsigned long data_count,
			    unsigned long write_count,
			    const struct timeval *start_time,
			    const struct timeval *end_time)
{
  ULONGEST time_count;
  struct ui_out *uiout = current_uiout;

  /* Compute the elapsed time in milliseconds, as a tradeoff between
     accuracy and overflow.  */
  time_count = (end_time->tv_sec - start_time->tv_sec) * 1000;
  time_count += (end_time->tv_usec - start_time->tv_usec) / 1000;

  ui_out_text (uiout, "Transfer rate: ");
  if (time_count > 0)
    {
      unsigned long rate = ((ULONGEST) data_count * 1000) / time_count;

      if (ui_out_is_mi_like_p (uiout))
	{
	  ui_out_field_fmt (uiout, "transfer-rate", "%lu", rate * 8);
	  ui_out_text (uiout, " bits/sec");
	}
      else if (rate < 1024)
	{
	  ui_out_field_fmt (uiout, "transfer-rate", "%lu", rate);
	  ui_out_text (uiout, " bytes/sec");
	}
      else
	{
	  ui_out_field_fmt (uiout, "transfer-rate", "%lu", rate / 1024);
	  ui_out_text (uiout, " KB/sec");
	}
    }
  else
    {
      ui_out_field_fmt (uiout, "transferred-bits", "%lu", (data_count * 8));
      ui_out_text (uiout, " bits in <1 sec");
    }
  if (write_count > 0)
    {
      ui_out_text (uiout, ", ");
      ui_out_field_fmt (uiout, "write-rate", "%lu", data_count / write_count);
      ui_out_text (uiout, " bytes/write");
    }
  ui_out_text (uiout, ".\n");
}

/* This function allows the addition of incrementally linked object files.
   It does not modify any state in the target, only in the debugger.  */
/* Note: ezannoni 2000-04-13 This function/command used to have a
   special case syntax for the rombug target (Rombug is the boot
   monitor for Microware's OS-9 / OS-9000, see remote-os9k.c). In the
   rombug case, the user doesn't need to supply a text address,
   instead a call to target_link() (in target.c) would supply the
   value to use.  We are now discontinuing this type of ad hoc syntax.  */

static void
add_symbol_file_command (char *args, int from_tty)
{
  struct gdbarch *gdbarch = get_current_arch ();
  char *filename = NULL;
  int flags = OBJF_USERLOADED;
  char *arg;
  int section_index = 0;
  int argcnt = 0;
  int sec_num = 0;
  int i;
  int expecting_sec_name = 0;
  int expecting_sec_addr = 0;
  char **argv;

  struct sect_opt
  {
    char *name;
    char *value;
  };

  struct section_addr_info *section_addrs;
  struct sect_opt *sect_opts = NULL;
  size_t num_sect_opts = 0;
  struct cleanup *my_cleanups = make_cleanup (null_cleanup, NULL);

  num_sect_opts = 16;
  sect_opts = (struct sect_opt *) xmalloc (num_sect_opts
					   * sizeof (struct sect_opt));

  dont_repeat ();

  if (args == NULL)
    error (_("add-symbol-file takes a file name and an address"));

  argv = gdb_buildargv (args);
  make_cleanup_freeargv (argv);

  for (arg = argv[0], argcnt = 0; arg != NULL; arg = argv[++argcnt])
    {
      /* Process the argument.  */
      if (argcnt == 0)
	{
	  /* The first argument is the file name.  */
	  filename = tilde_expand (arg);
	  make_cleanup (xfree, filename);
	}
      else
	if (argcnt == 1)
	  {
	    /* The second argument is always the text address at which
               to load the program.  */
	    sect_opts[section_index].name = ".text";
	    sect_opts[section_index].value = arg;
	    if (++section_index >= num_sect_opts)
	      {
		num_sect_opts *= 2;
		sect_opts = ((struct sect_opt *)
			     xrealloc (sect_opts,
				       num_sect_opts
				       * sizeof (struct sect_opt)));
	      }
	  }
	else
	  {
	    /* It's an option (starting with '-') or it's an argument
	       to an option.  */

	    if (*arg == '-')
	      {
		if (strcmp (arg, "-readnow") == 0)
		  flags |= OBJF_READNOW;
		else if (strcmp (arg, "-s") == 0)
		  {
		    expecting_sec_name = 1;
		    expecting_sec_addr = 1;
		  }
	      }
	    else
	      {
		if (expecting_sec_name)
		  {
		    sect_opts[section_index].name = arg;
		    expecting_sec_name = 0;
		  }
		else
		  if (expecting_sec_addr)
		    {
		      sect_opts[section_index].value = arg;
		      expecting_sec_addr = 0;
		      if (++section_index >= num_sect_opts)
			{
			  num_sect_opts *= 2;
			  sect_opts = ((struct sect_opt *)
				       xrealloc (sect_opts,
						 num_sect_opts
						 * sizeof (struct sect_opt)));
			}
		    }
		  else
		    error (_("USAGE: add-symbol-file <filename> <textaddress>"
			     " [-readnow] [-s <secname> <addr>]*"));
	      }
	  }
    }

  /* This command takes at least two arguments.  The first one is a
     filename, and the second is the address where this file has been
     loaded.  Abort now if this address hasn't been provided by the
     user.  */
  if (section_index < 1)
    error (_("The address where %s has been loaded is missing"), filename);

  /* Print the prompt for the query below.  And save the arguments into
     a sect_addr_info structure to be passed around to other
     functions.  We have to split this up into separate print
     statements because hex_string returns a local static
     string.  */

  printf_unfiltered (_("add symbol table from file \"%s\" at\n"), filename);
  section_addrs = alloc_section_addr_info (section_index);
  make_cleanup (xfree, section_addrs);
  for (i = 0; i < section_index; i++)
    {
      CORE_ADDR addr;
      char *val = sect_opts[i].value;
      char *sec = sect_opts[i].name;

      addr = parse_and_eval_address (val);

      /* Here we store the section offsets in the order they were
         entered on the command line.  */
      section_addrs->other[sec_num].name = sec;
      section_addrs->other[sec_num].addr = addr;
      printf_unfiltered ("\t%s_addr = %s\n", sec,
			 paddress (gdbarch, addr));
      sec_num++;

      /* The object's sections are initialized when a
	 call is made to build_objfile_section_table (objfile).
	 This happens in reread_symbols.
	 At this point, we don't know what file type this is,
	 so we can't determine what section names are valid.  */
    }

  if (from_tty && (!query ("%s", "")))
    error (_("Not confirmed."));

  symbol_file_add (filename, from_tty ? SYMFILE_VERBOSE : 0,
                   section_addrs, flags);

  /* Getting new symbols may change our opinion about what is
     frameless.  */
  reinit_frame_cache ();
  do_cleanups (my_cleanups);
}


typedef struct objfile *objfilep;

DEF_VEC_P (objfilep);

/* Re-read symbols if a symbol-file has changed.  */
void
reread_symbols (void)
{
  struct objfile *objfile;
  long new_modtime;
  struct stat new_statbuf;
  int res;
  VEC (objfilep) *new_objfiles = NULL;
  struct cleanup *all_cleanups;

  all_cleanups = make_cleanup (VEC_cleanup (objfilep), &new_objfiles);

  /* With the addition of shared libraries, this should be modified,
     the load time should be saved in the partial symbol tables, since
     different tables may come from different source files.  FIXME.
     This routine should then walk down each partial symbol table
     and see if the symbol table that it originates from has been changed.  */

  for (objfile = object_files; objfile; objfile = objfile->next)
    {
      /* solib-sunos.c creates one objfile with obfd.  */
      if (objfile->obfd == NULL)
	continue;

      /* Separate debug objfiles are handled in the main objfile.  */
      if (objfile->separate_debug_objfile_backlink)
	continue;

      /* If this object is from an archive (what you usually create with
	 `ar', often called a `static library' on most systems, though
	 a `shared library' on AIX is also an archive), then you should
	 stat on the archive name, not member name.  */
      if (objfile->obfd->my_archive)
	res = stat (objfile->obfd->my_archive->filename, &new_statbuf);
      else
	res = stat (objfile->name, &new_statbuf);
      if (res != 0)
	{
	  /* FIXME, should use print_sys_errmsg but it's not filtered.  */
	  printf_unfiltered (_("`%s' has disappeared; keeping its symbols.\n"),
			     objfile->name);
	  continue;
	}
      new_modtime = new_statbuf.st_mtime;
      if (new_modtime != objfile->mtime)
	{
	  struct cleanup *old_cleanups;
	  struct section_offsets *offsets;
	  int num_offsets;
	  char *obfd_filename;

	  printf_unfiltered (_("`%s' has changed; re-reading symbols.\n"),
			     objfile->name);

	  /* There are various functions like symbol_file_add,
	     symfile_bfd_open, syms_from_objfile, etc., which might
	     appear to do what we want.  But they have various other
	     effects which we *don't* want.  So we just do stuff
	     ourselves.  We don't worry about mapped files (for one thing,
	     any mapped file will be out of date).  */

	  /* If we get an error, blow away this objfile (not sure if
	     that is the correct response for things like shared
	     libraries).  */
	  old_cleanups = make_cleanup_free_objfile (objfile);
	  /* We need to do this whenever any symbols go away.  */
	  make_cleanup (clear_symtab_users_cleanup, 0 /*ignore*/);

	  if (exec_bfd != NULL
	      && filename_cmp (bfd_get_filename (objfile->obfd),
			       bfd_get_filename (exec_bfd)) == 0)
	    {
	      /* Reload EXEC_BFD without asking anything.  */

	      exec_file_attach (bfd_get_filename (objfile->obfd), 0);
	    }

	  /* Keep the calls order approx. the same as in free_objfile.  */

	  /* Free the separate debug objfiles.  It will be
	     automatically recreated by sym_read.  */
	  free_objfile_separate_debug (objfile);

	  /* Remove any references to this objfile in the global
	     value lists.  */
	  preserve_values (objfile);

	  /* Nuke all the state that we will re-read.  Much of the following
	     code which sets things to NULL really is necessary to tell
	     other parts of GDB that there is nothing currently there.

	     Try to keep the freeing order compatible with free_objfile.  */

	  if (objfile->sf != NULL)
	    {
	      (*objfile->sf->sym_finish) (objfile);
	    }

	  clear_objfile_data (objfile);

	  /* Clean up any state BFD has sitting around.  We don't need
	     to close the descriptor but BFD lacks a way of closing the
	     BFD without closing the descriptor.  */
	  {
	    struct bfd *obfd = objfile->obfd;

	    obfd_filename = bfd_get_filename (objfile->obfd);
	    /* Open the new BFD before freeing the old one, so that
	       the filename remains live.  */
	    objfile->obfd = gdb_bfd_open_maybe_remote (obfd_filename);
	    gdb_bfd_unref (obfd);
	  }

	  if (objfile->obfd == NULL)
	    error (_("Can't open %s to read symbols."), objfile->name);
	  /* bfd_openr sets cacheable to true, which is what we want.  */
	  if (!bfd_check_format (objfile->obfd, bfd_object))
	    error (_("Can't read symbols from %s: %s."), objfile->name,
		   bfd_errmsg (bfd_get_error ()));

	  /* Save the offsets, we will nuke them with the rest of the
	     objfile_obstack.  */
	  num_offsets = objfile->num_sections;
	  offsets = ((struct section_offsets *)
		     alloca (SIZEOF_N_SECTION_OFFSETS (num_offsets)));
	  memcpy (offsets, objfile->section_offsets,
		  SIZEOF_N_SECTION_OFFSETS (num_offsets));

	  /* FIXME: Do we have to free a whole linked list, or is this
	     enough?  */
	  if (objfile->global_psymbols.list)
	    xfree (objfile->global_psymbols.list);
	  memset (&objfile->global_psymbols, 0,
		  sizeof (objfile->global_psymbols));
	  if (objfile->static_psymbols.list)
	    xfree (objfile->static_psymbols.list);
	  memset (&objfile->static_psymbols, 0,
		  sizeof (objfile->static_psymbols));

	  /* Free the obstacks for non-reusable objfiles.  */
	  psymbol_bcache_free (objfile->psymbol_cache);
	  objfile->psymbol_cache = psymbol_bcache_init ();
	  bcache_xfree (objfile->macro_cache);
	  objfile->macro_cache = bcache_xmalloc (NULL, NULL);
	  bcache_xfree (objfile->filename_cache);
	  objfile->filename_cache = bcache_xmalloc (NULL,NULL);
	  if (objfile->demangled_names_hash != NULL)
	    {
	      htab_delete (objfile->demangled_names_hash);
	      objfile->demangled_names_hash = NULL;
	    }
	  obstack_free (&objfile->objfile_obstack, 0);
	  objfile->sections = NULL;
	  objfile->symtabs = NULL;
	  objfile->psymtabs = NULL;
	  objfile->psymtabs_addrmap = NULL;
	  objfile->free_psymtabs = NULL;
	  objfile->template_symbols = NULL;
	  objfile->msymbols = NULL;
	  objfile->deprecated_sym_private = NULL;
	  objfile->minimal_symbol_count = 0;
	  memset (&objfile->msymbol_hash, 0,
		  sizeof (objfile->msymbol_hash));
	  memset (&objfile->msymbol_demangled_hash, 0,
		  sizeof (objfile->msymbol_demangled_hash));

	  /* obstack_init also initializes the obstack so it is
	     empty.  We could use obstack_specify_allocation but
	     gdb_obstack.h specifies the alloc/dealloc functions.  */
	  obstack_init (&objfile->objfile_obstack);
	  build_objfile_section_table (objfile);
	  terminate_minimal_symbol_table (objfile);

	  /* We use the same section offsets as from last time.  I'm not
	     sure whether that is always correct for shared libraries.  */
	  objfile->section_offsets = (struct section_offsets *)
	    obstack_alloc (&objfile->objfile_obstack,
			   SIZEOF_N_SECTION_OFFSETS (num_offsets));
	  memcpy (objfile->section_offsets, offsets,
		  SIZEOF_N_SECTION_OFFSETS (num_offsets));
	  objfile->num_sections = num_offsets;

	  /* What the hell is sym_new_init for, anyway?  The concept of
	     distinguishing between the main file and additional files
	     in this way seems rather dubious.  */
	  if (objfile == symfile_objfile)
	    {
	      (*objfile->sf->sym_new_init) (objfile);
	    }

	  (*objfile->sf->sym_init) (objfile);
	  clear_complaints (&symfile_complaints, 1, 1);
	  /* Do not set flags as this is safe and we don't want to be
             verbose.  */
	  (*objfile->sf->sym_read) (objfile, 0);
	  if ((objfile->flags & OBJF_PSYMTABS_READ) != 0)
	    {
	      objfile->flags &= ~OBJF_PSYMTABS_READ;
	      require_partial_symbols (objfile, 0);
	    }

	  if (!objfile_has_symbols (objfile))
	    {
	      wrap_here ("");
	      printf_unfiltered (_("(no debugging symbols found)\n"));
	      wrap_here ("");
	    }

	  /* We're done reading the symbol file; finish off complaints.  */
	  clear_complaints (&symfile_complaints, 0, 1);

	  /* Getting new symbols may change our opinion about what is
	     frameless.  */

	  reinit_frame_cache ();

	  /* Discard cleanups as symbol reading was successful.  */
	  discard_cleanups (old_cleanups);

	  /* If the mtime has changed between the time we set new_modtime
	     and now, we *want* this to be out of date, so don't call stat
	     again now.  */
	  objfile->mtime = new_modtime;
	  init_entry_point_info (objfile);

	  VEC_safe_push (objfilep, new_objfiles, objfile);
	}
    }

  if (new_objfiles)
    {
      int ix;

      /* Notify objfiles that we've modified objfile sections.  */
      objfiles_changed ();

      clear_symtab_users (0);

      /* clear_objfile_data for each objfile was called before freeing it and
	 observer_notify_new_objfile (NULL) has been called by
	 clear_symtab_users above.  Notify the new files now.  */
      for (ix = 0; VEC_iterate (objfilep, new_objfiles, ix, objfile); ix++)
	observer_notify_new_objfile (objfile);

      /* At least one objfile has changed, so we can consider that
         the executable we're debugging has changed too.  */
      observer_notify_executable_changed ();
    }

  do_cleanups (all_cleanups);
}



typedef struct
{
  char *ext;
  enum language lang;
}
filename_language;

static filename_language *filename_language_table;
static int fl_table_size, fl_table_next;

static void
add_filename_language (char *ext, enum language lang)
{
  if (fl_table_next >= fl_table_size)
    {
      fl_table_size += 10;
      filename_language_table =
	xrealloc (filename_language_table,
		  fl_table_size * sizeof (*filename_language_table));
    }

  filename_language_table[fl_table_next].ext = xstrdup (ext);
  filename_language_table[fl_table_next].lang = lang;
  fl_table_next++;
}

static char *ext_args;
static void
show_ext_args (struct ui_file *file, int from_tty,
	       struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file,
		    _("Mapping between filename extension "
		      "and source language is \"%s\".\n"),
		    value);
}

static void
set_ext_lang_command (char *args, int from_tty, struct cmd_list_element *e)
{
  int i;
  char *cp = ext_args;
  enum language lang;

  /* First arg is filename extension, starting with '.'  */
  if (*cp != '.')
    error (_("'%s': Filename extension must begin with '.'"), ext_args);

  /* Find end of first arg.  */
  while (*cp && !isspace (*cp))
    cp++;

  if (*cp == '\0')
    error (_("'%s': two arguments required -- "
	     "filename extension and language"),
	   ext_args);

  /* Null-terminate first arg.  */
  *cp++ = '\0';

  /* Find beginning of second arg, which should be a source language.  */
  while (*cp && isspace (*cp))
    cp++;

  if (*cp == '\0')
    error (_("'%s': two arguments required -- "
	     "filename extension and language"),
	   ext_args);

  /* Lookup the language from among those we know.  */
  lang = language_enum (cp);

  /* Now lookup the filename extension: do we already know it?  */
  for (i = 0; i < fl_table_next; i++)
    if (0 == strcmp (ext_args, filename_language_table[i].ext))
      break;

  if (i >= fl_table_next)
    {
      /* New file extension.  */
      add_filename_language (ext_args, lang);
    }
  else
    {
      /* Redefining a previously known filename extension.  */

      /* if (from_tty) */
      /*   query ("Really make files of type %s '%s'?", */
      /*          ext_args, language_str (lang));           */

      xfree (filename_language_table[i].ext);
      filename_language_table[i].ext = xstrdup (ext_args);
      filename_language_table[i].lang = lang;
    }
}

static void
info_ext_lang_command (char *args, int from_tty)
{
  int i;

  printf_filtered (_("Filename extensions and the languages they represent:"));
  printf_filtered ("\n\n");
  for (i = 0; i < fl_table_next; i++)
    printf_filtered ("\t%s\t- %s\n",
		     filename_language_table[i].ext,
		     language_str (filename_language_table[i].lang));
}

static void
init_filename_language_table (void)
{
  if (fl_table_size == 0)	/* Protect against repetition.  */
    {
      fl_table_size = 20;
      fl_table_next = 0;
      filename_language_table =
	xmalloc (fl_table_size * sizeof (*filename_language_table));
      add_filename_language (".c", language_c);
      add_filename_language (".d", language_d);
      add_filename_language (".C", language_cplus);
      add_filename_language (".cc", language_cplus);
      add_filename_language (".cp", language_cplus);
      add_filename_language (".cpp", language_cplus);
      add_filename_language (".cxx", language_cplus);
      add_filename_language (".c++", language_cplus);
      add_filename_language (".java", language_java);
      add_filename_language (".class", language_java);
      add_filename_language (".m", language_objc);
      add_filename_language (".f", language_fortran);
      add_filename_language (".F", language_fortran);
      add_filename_language (".for", language_fortran);
      add_filename_language (".FOR", language_fortran);
      add_filename_language (".ftn", language_fortran);
      add_filename_language (".FTN", language_fortran);
      add_filename_language (".fpp", language_fortran);
      add_filename_language (".FPP", language_fortran);
      add_filename_language (".f90", language_fortran);
      add_filename_language (".F90", language_fortran);
      add_filename_language (".f95", language_fortran);
      add_filename_language (".F95", language_fortran);
      add_filename_language (".f03", language_fortran);
      add_filename_language (".F03", language_fortran);
      add_filename_language (".f08", language_fortran);
      add_filename_language (".F08", language_fortran);
      add_filename_language (".s", language_asm);
      add_filename_language (".sx", language_asm);
      add_filename_language (".S", language_asm);
      add_filename_language (".pas", language_pascal);
      add_filename_language (".p", language_pascal);
      add_filename_language (".pp", language_pascal);
      add_filename_language (".adb", language_ada);
      add_filename_language (".ads", language_ada);
      add_filename_language (".a", language_ada);
      add_filename_language (".ada", language_ada);
      add_filename_language (".dg", language_ada);
    }
}

enum language
deduce_language_from_filename (const char *filename)
{
  int i;
  char *cp;

  if (filename != NULL)
    if ((cp = strrchr (filename, '.')) != NULL)
      for (i = 0; i < fl_table_next; i++)
	if (strcmp (cp, filename_language_table[i].ext) == 0)
	  return filename_language_table[i].lang;

  return language_unknown;
}

/* allocate_symtab:

   Allocate and partly initialize a new symbol table.  Return a pointer
   to it.  error() if no space.

   Caller must set these fields:
   LINETABLE(symtab)
   symtab->blockvector
   symtab->dirname
   symtab->free_code
   symtab->free_ptr
 */

struct symtab *
allocate_symtab (const char *filename, struct objfile *objfile)
{
  struct symtab *symtab;

  symtab = (struct symtab *)
    obstack_alloc (&objfile->objfile_obstack, sizeof (struct symtab));
  memset (symtab, 0, sizeof (*symtab));
  symtab->filename = (char *) bcache (filename, strlen (filename) + 1,
				      objfile->filename_cache);
  symtab->fullname = NULL;
  symtab->language = deduce_language_from_filename (filename);
  symtab->debugformat = "unknown";

  /* Hook it to the objfile it comes from.  */

  symtab->objfile = objfile;
  symtab->next = objfile->symtabs;
  objfile->symtabs = symtab;

  if (symtab_create_debug)
    {
      /* Be a bit clever with debugging messages, and don't print objfile
	 every time, only when it changes.  */
      static char *last_objfile_name = NULL;

      if (last_objfile_name == NULL
	  || strcmp (last_objfile_name, objfile->name) != 0)
	{
	  xfree (last_objfile_name);
	  last_objfile_name = xstrdup (objfile->name);
	  fprintf_unfiltered (gdb_stdlog,
			      "Creating one or more symtabs for objfile %s ...\n",
			      last_objfile_name);
	}
      fprintf_unfiltered (gdb_stdlog,
			  "Created symtab %s for module %s.\n",
			  host_address_to_string (symtab), filename);
    }

  return (symtab);
}


/* Reset all data structures in gdb which may contain references to symbol
   table data.  ADD_FLAGS is a bitmask of enum symfile_add_flags.  */

void
clear_symtab_users (int add_flags)
{
  /* Someday, we should do better than this, by only blowing away
     the things that really need to be blown.  */

  /* Clear the "current" symtab first, because it is no longer valid.
     breakpoint_re_set may try to access the current symtab.  */
  clear_current_source_symtab_and_line ();

  clear_displays ();
  if ((add_flags & SYMFILE_DEFER_BP_RESET) == 0)
    breakpoint_re_set ();
  clear_last_displayed_sal ();
  clear_pc_function_cache ();
  observer_notify_new_objfile (NULL);

  /* Clear globals which might have pointed into a removed objfile.
     FIXME: It's not clear which of these are supposed to persist
     between expressions and which ought to be reset each time.  */
  expression_context_block = NULL;
  innermost_block = NULL;

  /* Varobj may refer to old symbols, perform a cleanup.  */
  varobj_invalidate ();

}

static void
clear_symtab_users_cleanup (void *ignore)
{
  clear_symtab_users (0);
}

/* OVERLAYS:
   The following code implements an abstraction for debugging overlay sections.

   The target model is as follows:
   1) The gnu linker will permit multiple sections to be mapped into the
   same VMA, each with its own unique LMA (or load address).
   2) It is assumed that some runtime mechanism exists for mapping the
   sections, one by one, from the load address into the VMA address.
   3) This code provides a mechanism for gdb to keep track of which
   sections should be considered to be mapped from the VMA to the LMA.
   This information is used for symbol lookup, and memory read/write.
   For instance, if a section has been mapped then its contents
   should be read from the VMA, otherwise from the LMA.

   Two levels of debugger support for overlays are available.  One is
   "manual", in which the debugger relies on the user to tell it which
   overlays are currently mapped.  This level of support is
   implemented entirely in the core debugger, and the information about
   whether a section is mapped is kept in the objfile->obj_section table.

   The second level of support is "automatic", and is only available if
   the target-specific code provides functionality to read the target's
   overlay mapping table, and translate its contents for the debugger
   (by updating the mapped state information in the obj_section tables).

   The interface is as follows:
   User commands:
   overlay map <name>   -- tell gdb to consider this section mapped
   overlay unmap <name> -- tell gdb to consider this section unmapped
   overlay list         -- list the sections that GDB thinks are mapped
   overlay read-target  -- get the target's state of what's mapped
   overlay off/manual/auto -- set overlay debugging state
   Functional interface:
   find_pc_mapped_section(pc):    if the pc is in the range of a mapped
   section, return that section.
   find_pc_overlay(pc):       find any overlay section that contains
   the pc, either in its VMA or its LMA
   section_is_mapped(sect):       true if overlay is marked as mapped
   section_is_overlay(sect):      true if section's VMA != LMA
   pc_in_mapped_range(pc,sec):    true if pc belongs to section's VMA
   pc_in_unmapped_range(...):     true if pc belongs to section's LMA
   sections_overlap(sec1, sec2):  true if mapped sec1 and sec2 ranges overlap
   overlay_mapped_address(...):   map an address from section's LMA to VMA
   overlay_unmapped_address(...): map an address from section's VMA to LMA
   symbol_overlayed_address(...): Return a "current" address for symbol:
   either in VMA or LMA depending on whether
   the symbol's section is currently mapped.  */

/* Overlay debugging state: */

enum overlay_debugging_state overlay_debugging = ovly_off;
int overlay_cache_invalid = 0;	/* True if need to refresh mapped state.  */

/* Function: section_is_overlay (SECTION)
   Returns true if SECTION has VMA not equal to LMA, ie.
   SECTION is loaded at an address different from where it will "run".  */

int
section_is_overlay (struct obj_section *section)
{
  if (overlay_debugging && section)
    {
      bfd *abfd = section->objfile->obfd;
      asection *bfd_section = section->the_bfd_section;

      if (bfd_section_lma (abfd, bfd_section) != 0
	  && bfd_section_lma (abfd, bfd_section)
	     != bfd_section_vma (abfd, bfd_section))
	return 1;
    }

  return 0;
}

/* Function: overlay_invalidate_all (void)
   Invalidate the mapped state of all overlay sections (mark it as stale).  */

static void
overlay_invalidate_all (void)
{
  struct objfile *objfile;
  struct obj_section *sect;

  ALL_OBJSECTIONS (objfile, sect)
    if (section_is_overlay (sect))
      sect->ovly_mapped = -1;
}

/* Function: section_is_mapped (SECTION)
   Returns true if section is an overlay, and is currently mapped.

   Access to the ovly_mapped flag is restricted to this function, so
   that we can do automatic update.  If the global flag
   OVERLAY_CACHE_INVALID is set (by wait_for_inferior), then call
   overlay_invalidate_all.  If the mapped state of the particular
   section is stale, then call TARGET_OVERLAY_UPDATE to refresh it.  */

int
section_is_mapped (struct obj_section *osect)
{
  struct gdbarch *gdbarch;

  if (osect == 0 || !section_is_overlay (osect))
    return 0;

  switch (overlay_debugging)
    {
    default:
    case ovly_off:
      return 0;			/* overlay debugging off */
    case ovly_auto:		/* overlay debugging automatic */
      /* Unles there is a gdbarch_overlay_update function,
         there's really nothing useful to do here (can't really go auto).  */
      gdbarch = get_objfile_arch (osect->objfile);
      if (gdbarch_overlay_update_p (gdbarch))
	{
	  if (overlay_cache_invalid)
	    {
	      overlay_invalidate_all ();
	      overlay_cache_invalid = 0;
	    }
	  if (osect->ovly_mapped == -1)
	    gdbarch_overlay_update (gdbarch, osect);
	}
      /* fall thru to manual case */
    case ovly_on:		/* overlay debugging manual */
      return osect->ovly_mapped == 1;
    }
}

/* Function: pc_in_unmapped_range
   If PC falls into the lma range of SECTION, return true, else false.  */

CORE_ADDR
pc_in_unmapped_range (CORE_ADDR pc, struct obj_section *section)
{
  if (section_is_overlay (section))
    {
      bfd *abfd = section->objfile->obfd;
      asection *bfd_section = section->the_bfd_section;

      /* We assume the LMA is relocated by the same offset as the VMA.  */
      bfd_vma size = bfd_get_section_size (bfd_section);
      CORE_ADDR offset = obj_section_offset (section);

      if (bfd_get_section_lma (abfd, bfd_section) + offset <= pc
	  && pc < bfd_get_section_lma (abfd, bfd_section) + offset + size)
	return 1;
    }

  return 0;
}

/* Function: pc_in_mapped_range
   If PC falls into the vma range of SECTION, return true, else false.  */

CORE_ADDR
pc_in_mapped_range (CORE_ADDR pc, struct obj_section *section)
{
  if (section_is_overlay (section))
    {
      if (obj_section_addr (section) <= pc
	  && pc < obj_section_endaddr (section))
	return 1;
    }

  return 0;
}


/* Return true if the mapped ranges of sections A and B overlap, false
   otherwise.  */
static int
sections_overlap (struct obj_section *a, struct obj_section *b)
{
  CORE_ADDR a_start = obj_section_addr (a);
  CORE_ADDR a_end = obj_section_endaddr (a);
  CORE_ADDR b_start = obj_section_addr (b);
  CORE_ADDR b_end = obj_section_endaddr (b);

  return (a_start < b_end && b_start < a_end);
}

/* Function: overlay_unmapped_address (PC, SECTION)
   Returns the address corresponding to PC in the unmapped (load) range.
   May be the same as PC.  */

CORE_ADDR
overlay_unmapped_address (CORE_ADDR pc, struct obj_section *section)
{
  if (section_is_overlay (section) && pc_in_mapped_range (pc, section))
    {
      bfd *abfd = section->objfile->obfd;
      asection *bfd_section = section->the_bfd_section;

      return pc + bfd_section_lma (abfd, bfd_section)
		- bfd_section_vma (abfd, bfd_section);
    }

  return pc;
}

/* Function: overlay_mapped_address (PC, SECTION)
   Returns the address corresponding to PC in the mapped (runtime) range.
   May be the same as PC.  */

CORE_ADDR
overlay_mapped_address (CORE_ADDR pc, struct obj_section *section)
{
  if (section_is_overlay (section) && pc_in_unmapped_range (pc, section))
    {
      bfd *abfd = section->objfile->obfd;
      asection *bfd_section = section->the_bfd_section;

      return pc + bfd_section_vma (abfd, bfd_section)
		- bfd_section_lma (abfd, bfd_section);
    }

  return pc;
}


/* Function: symbol_overlayed_address
   Return one of two addresses (relative to the VMA or to the LMA),
   depending on whether the section is mapped or not.  */

CORE_ADDR
symbol_overlayed_address (CORE_ADDR address, struct obj_section *section)
{
  if (overlay_debugging)
    {
      /* If the symbol has no section, just return its regular address.  */
      if (section == 0)
	return address;
      /* If the symbol's section is not an overlay, just return its
	 address.  */
      if (!section_is_overlay (section))
	return address;
      /* If the symbol's section is mapped, just return its address.  */
      if (section_is_mapped (section))
	return address;
      /*
       * HOWEVER: if the symbol is in an overlay section which is NOT mapped,
       * then return its LOADED address rather than its vma address!!
       */
      return overlay_unmapped_address (address, section);
    }
  return address;
}

/* Function: find_pc_overlay (PC)
   Return the best-match overlay section for PC:
   If PC matches a mapped overlay section's VMA, return that section.
   Else if PC matches an unmapped section's VMA, return that section.
   Else if PC matches an unmapped section's LMA, return that section.  */

struct obj_section *
find_pc_overlay (CORE_ADDR pc)
{
  struct objfile *objfile;
  struct obj_section *osect, *best_match = NULL;

  if (overlay_debugging)
    ALL_OBJSECTIONS (objfile, osect)
      if (section_is_overlay (osect))
      {
	if (pc_in_mapped_range (pc, osect))
	  {
	    if (section_is_mapped (osect))
	      return osect;
	    else
	      best_match = osect;
	  }
	else if (pc_in_unmapped_range (pc, osect))
	  best_match = osect;
      }
  return best_match;
}

/* Function: find_pc_mapped_section (PC)
   If PC falls into the VMA address range of an overlay section that is
   currently marked as MAPPED, return that section.  Else return NULL.  */

struct obj_section *
find_pc_mapped_section (CORE_ADDR pc)
{
  struct objfile *objfile;
  struct obj_section *osect;

  if (overlay_debugging)
    ALL_OBJSECTIONS (objfile, osect)
      if (pc_in_mapped_range (pc, osect) && section_is_mapped (osect))
	return osect;

  return NULL;
}

/* Function: list_overlays_command
   Print a list of mapped sections and their PC ranges.  */

void
list_overlays_command (char *args, int from_tty)
{
  int nmapped = 0;
  struct objfile *objfile;
  struct obj_section *osect;

  if (overlay_debugging)
    ALL_OBJSECTIONS (objfile, osect)
      if (section_is_mapped (osect))
      {
	struct gdbarch *gdbarch = get_objfile_arch (objfile);
	const char *name;
	bfd_vma lma, vma;
	int size;

	vma = bfd_section_vma (objfile->obfd, osect->the_bfd_section);
	lma = bfd_section_lma (objfile->obfd, osect->the_bfd_section);
	size = bfd_get_section_size (osect->the_bfd_section);
	name = bfd_section_name (objfile->obfd, osect->the_bfd_section);

	printf_filtered ("Section %s, loaded at ", name);
	fputs_filtered (paddress (gdbarch, lma), gdb_stdout);
	puts_filtered (" - ");
	fputs_filtered (paddress (gdbarch, lma + size), gdb_stdout);
	printf_filtered (", mapped at ");
	fputs_filtered (paddress (gdbarch, vma), gdb_stdout);
	puts_filtered (" - ");
	fputs_filtered (paddress (gdbarch, vma + size), gdb_stdout);
	puts_filtered ("\n");

	nmapped++;
      }
  if (nmapped == 0)
    printf_filtered (_("No sections are mapped.\n"));
}

/* Function: map_overlay_command
   Mark the named section as mapped (ie. residing at its VMA address).  */

void
map_overlay_command (char *args, int from_tty)
{
  struct objfile *objfile, *objfile2;
  struct obj_section *sec, *sec2;

  if (!overlay_debugging)
    error (_("Overlay debugging not enabled.  Use "
	     "either the 'overlay auto' or\n"
	     "the 'overlay manual' command."));

  if (args == 0 || *args == 0)
    error (_("Argument required: name of an overlay section"));

  /* First, find a section matching the user supplied argument.  */
  ALL_OBJSECTIONS (objfile, sec)
    if (!strcmp (bfd_section_name (objfile->obfd, sec->the_bfd_section), args))
    {
      /* Now, check to see if the section is an overlay.  */
      if (!section_is_overlay (sec))
	continue;		/* not an overlay section */

      /* Mark the overlay as "mapped".  */
      sec->ovly_mapped = 1;

      /* Next, make a pass and unmap any sections that are
         overlapped by this new section: */
      ALL_OBJSECTIONS (objfile2, sec2)
	if (sec2->ovly_mapped && sec != sec2 && sections_overlap (sec, sec2))
	{
	  if (info_verbose)
	    printf_unfiltered (_("Note: section %s unmapped by overlap\n"),
			     bfd_section_name (objfile->obfd,
					       sec2->the_bfd_section));
	  sec2->ovly_mapped = 0;	/* sec2 overlaps sec: unmap sec2.  */
	}
      return;
    }
  error (_("No overlay section called %s"), args);
}

/* Function: unmap_overlay_command
   Mark the overlay section as unmapped
   (ie. resident in its LMA address range, rather than the VMA range).  */

void
unmap_overlay_command (char *args, int from_tty)
{
  struct objfile *objfile;
  struct obj_section *sec;

  if (!overlay_debugging)
    error (_("Overlay debugging not enabled.  "
	     "Use either the 'overlay auto' or\n"
	     "the 'overlay manual' command."));

  if (args == 0 || *args == 0)
    error (_("Argument required: name of an overlay section"));

  /* First, find a section matching the user supplied argument.  */
  ALL_OBJSECTIONS (objfile, sec)
    if (!strcmp (bfd_section_name (objfile->obfd, sec->the_bfd_section), args))
    {
      if (!sec->ovly_mapped)
	error (_("Section %s is not mapped"), args);
      sec->ovly_mapped = 0;
      return;
    }
  error (_("No overlay section called %s"), args);
}

/* Function: overlay_auto_command
   A utility command to turn on overlay debugging.
   Possibly this should be done via a set/show command.  */

static void
overlay_auto_command (char *args, int from_tty)
{
  overlay_debugging = ovly_auto;
  enable_overlay_breakpoints ();
  if (info_verbose)
    printf_unfiltered (_("Automatic overlay debugging enabled."));
}

/* Function: overlay_manual_command
   A utility command to turn on overlay debugging.
   Possibly this should be done via a set/show command.  */

static void
overlay_manual_command (char *args, int from_tty)
{
  overlay_debugging = ovly_on;
  disable_overlay_breakpoints ();
  if (info_verbose)
    printf_unfiltered (_("Overlay debugging enabled."));
}

/* Function: overlay_off_command
   A utility command to turn on overlay debugging.
   Possibly this should be done via a set/show command.  */

static void
overlay_off_command (char *args, int from_tty)
{
  overlay_debugging = ovly_off;
  disable_overlay_breakpoints ();
  if (info_verbose)
    printf_unfiltered (_("Overlay debugging disabled."));
}

static void
overlay_load_command (char *args, int from_tty)
{
  struct gdbarch *gdbarch = get_current_arch ();

  if (gdbarch_overlay_update_p (gdbarch))
    gdbarch_overlay_update (gdbarch, NULL);
  else
    error (_("This target does not know how to read its overlay state."));
}

/* Function: overlay_command
   A place-holder for a mis-typed command.  */

/* Command list chain containing all defined "overlay" subcommands.  */
static struct cmd_list_element *overlaylist;

static void
overlay_command (char *args, int from_tty)
{
  printf_unfiltered
    ("\"overlay\" must be followed by the name of an overlay command.\n");
  help_list (overlaylist, "overlay ", -1, gdb_stdout);
}


/* Target Overlays for the "Simplest" overlay manager:

   This is GDB's default target overlay layer.  It works with the
   minimal overlay manager supplied as an example by Cygnus.  The
   entry point is via a function pointer "gdbarch_overlay_update",
   so targets that use a different runtime overlay manager can
   substitute their own overlay_update function and take over the
   function pointer.

   The overlay_update function pokes around in the target's data structures
   to see what overlays are mapped, and updates GDB's overlay mapping with
   this information.

   In this simple implementation, the target data structures are as follows:
   unsigned _novlys;            /# number of overlay sections #/
   unsigned _ovly_table[_novlys][4] = {
   {VMA, SIZE, LMA, MAPPED},    /# one entry per overlay section #/
   {..., ...,  ..., ...},
   }
   unsigned _novly_regions;     /# number of overlay regions #/
   unsigned _ovly_region_table[_novly_regions][3] = {
   {VMA, SIZE, MAPPED_TO_LMA},  /# one entry per overlay region #/
   {..., ...,  ...},
   }
   These functions will attempt to update GDB's mappedness state in the
   symbol section table, based on the target's mappedness state.

   To do this, we keep a cached copy of the target's _ovly_table, and
   attempt to detect when the cached copy is invalidated.  The main
   entry point is "simple_overlay_update(SECT), which looks up SECT in
   the cached table and re-reads only the entry for that section from
   the target (whenever possible).  */

/* Cached, dynamically allocated copies of the target data structures: */
static unsigned (*cache_ovly_table)[4] = 0;
static unsigned cache_novlys = 0;
static CORE_ADDR cache_ovly_table_base = 0;
enum ovly_index
  {
    VMA, SIZE, LMA, MAPPED
  };

/* Throw away the cached copy of _ovly_table.  */
static void
simple_free_overlay_table (void)
{
  if (cache_ovly_table)
    xfree (cache_ovly_table);
  cache_novlys = 0;
  cache_ovly_table = NULL;
  cache_ovly_table_base = 0;
}

/* Read an array of ints of size SIZE from the target into a local buffer.
   Convert to host order.  int LEN is number of ints.  */
static void
read_target_long_array (CORE_ADDR memaddr, unsigned int *myaddr,
			int len, int size, enum bfd_endian byte_order)
{
  /* FIXME (alloca): Not safe if array is very large.  */
  gdb_byte *buf = alloca (len * size);
  int i;

  read_memory (memaddr, buf, len * size);
  for (i = 0; i < len; i++)
    myaddr[i] = extract_unsigned_integer (size * i + buf, size, byte_order);
}

/* Find and grab a copy of the target _ovly_table
   (and _novlys, which is needed for the table's size).  */
static int
simple_read_overlay_table (void)
{
  struct minimal_symbol *novlys_msym, *ovly_table_msym;
  struct gdbarch *gdbarch;
  int word_size;
  enum bfd_endian byte_order;

  simple_free_overlay_table ();
  novlys_msym = lookup_minimal_symbol ("_novlys", NULL, NULL);
  if (! novlys_msym)
    {
      error (_("Error reading inferior's overlay table: "
             "couldn't find `_novlys' variable\n"
             "in inferior.  Use `overlay manual' mode."));
      return 0;
    }

  ovly_table_msym = lookup_minimal_symbol ("_ovly_table", NULL, NULL);
  if (! ovly_table_msym)
    {
      error (_("Error reading inferior's overlay table: couldn't find "
             "`_ovly_table' array\n"
             "in inferior.  Use `overlay manual' mode."));
      return 0;
    }

  gdbarch = get_objfile_arch (msymbol_objfile (ovly_table_msym));
  word_size = gdbarch_long_bit (gdbarch) / TARGET_CHAR_BIT;
  byte_order = gdbarch_byte_order (gdbarch);

  cache_novlys = read_memory_integer (SYMBOL_VALUE_ADDRESS (novlys_msym),
				      4, byte_order);
  cache_ovly_table
    = (void *) xmalloc (cache_novlys * sizeof (*cache_ovly_table));
  cache_ovly_table_base = SYMBOL_VALUE_ADDRESS (ovly_table_msym);
  read_target_long_array (cache_ovly_table_base,
                          (unsigned int *) cache_ovly_table,
                          cache_novlys * 4, word_size, byte_order);

  return 1;			/* SUCCESS */
}

/* Function: simple_overlay_update_1
   A helper function for simple_overlay_update.  Assuming a cached copy
   of _ovly_table exists, look through it to find an entry whose vma,
   lma and size match those of OSECT.  Re-read the entry and make sure
   it still matches OSECT (else the table may no longer be valid).
   Set OSECT's mapped state to match the entry.  Return: 1 for
   success, 0 for failure.  */

static int
simple_overlay_update_1 (struct obj_section *osect)
{
  int i, size;
  bfd *obfd = osect->objfile->obfd;
  asection *bsect = osect->the_bfd_section;
  struct gdbarch *gdbarch = get_objfile_arch (osect->objfile);
  int word_size = gdbarch_long_bit (gdbarch) / TARGET_CHAR_BIT;
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);

  size = bfd_get_section_size (osect->the_bfd_section);
  for (i = 0; i < cache_novlys; i++)
    if (cache_ovly_table[i][VMA] == bfd_section_vma (obfd, bsect)
	&& cache_ovly_table[i][LMA] == bfd_section_lma (obfd, bsect)
	/* && cache_ovly_table[i][SIZE] == size */ )
      {
	read_target_long_array (cache_ovly_table_base + i * word_size,
				(unsigned int *) cache_ovly_table[i],
				4, word_size, byte_order);
	if (cache_ovly_table[i][VMA] == bfd_section_vma (obfd, bsect)
	    && cache_ovly_table[i][LMA] == bfd_section_lma (obfd, bsect)
	    /* && cache_ovly_table[i][SIZE] == size */ )
	  {
	    osect->ovly_mapped = cache_ovly_table[i][MAPPED];
	    return 1;
	  }
	else	/* Warning!  Warning!  Target's ovly table has changed!  */
	  return 0;
      }
  return 0;
}

/* Function: simple_overlay_update
   If OSECT is NULL, then update all sections' mapped state
   (after re-reading the entire target _ovly_table).
   If OSECT is non-NULL, then try to find a matching entry in the
   cached ovly_table and update only OSECT's mapped state.
   If a cached entry can't be found or the cache isn't valid, then
   re-read the entire cache, and go ahead and update all sections.  */

void
simple_overlay_update (struct obj_section *osect)
{
  struct objfile *objfile;

  /* Were we given an osect to look up?  NULL means do all of them.  */
  if (osect)
    /* Have we got a cached copy of the target's overlay table?  */
    if (cache_ovly_table != NULL)
      {
	/* Does its cached location match what's currently in the
	   symtab?  */
	struct minimal_symbol *minsym
	  = lookup_minimal_symbol ("_ovly_table", NULL, NULL);

	if (minsym == NULL)
	  error (_("Error reading inferior's overlay table: couldn't "
		   "find `_ovly_table' array\n"
		   "in inferior.  Use `overlay manual' mode."));
	
	if (cache_ovly_table_base == SYMBOL_VALUE_ADDRESS (minsym))
	  /* Then go ahead and try to look up this single section in
	     the cache.  */
	  if (simple_overlay_update_1 (osect))
	    /* Found it!  We're done.  */
	    return;
      }

  /* Cached table no good: need to read the entire table anew.
     Or else we want all the sections, in which case it's actually
     more efficient to read the whole table in one block anyway.  */

  if (! simple_read_overlay_table ())
    return;

  /* Now may as well update all sections, even if only one was requested.  */
  ALL_OBJSECTIONS (objfile, osect)
    if (section_is_overlay (osect))
    {
      int i, size;
      bfd *obfd = osect->objfile->obfd;
      asection *bsect = osect->the_bfd_section;

      size = bfd_get_section_size (bsect);
      for (i = 0; i < cache_novlys; i++)
	if (cache_ovly_table[i][VMA] == bfd_section_vma (obfd, bsect)
	    && cache_ovly_table[i][LMA] == bfd_section_lma (obfd, bsect)
	    /* && cache_ovly_table[i][SIZE] == size */ )
	  { /* obj_section matches i'th entry in ovly_table.  */
	    osect->ovly_mapped = cache_ovly_table[i][MAPPED];
	    break;		/* finished with inner for loop: break out.  */
	  }
    }
}

/* Set the output sections and output offsets for section SECTP in
   ABFD.  The relocation code in BFD will read these offsets, so we
   need to be sure they're initialized.  We map each section to itself,
   with no offset; this means that SECTP->vma will be honored.  */

static void
symfile_dummy_outputs (bfd *abfd, asection *sectp, void *dummy)
{
  sectp->output_section = sectp;
  sectp->output_offset = 0;
}

/* Default implementation for sym_relocate.  */


bfd_byte *
default_symfile_relocate (struct objfile *objfile, asection *sectp,
                          bfd_byte *buf)
{
  /* Use sectp->owner instead of objfile->obfd.  sectp may point to a
     DWO file.  */
  bfd *abfd = sectp->owner;

  /* We're only interested in sections with relocation
     information.  */
  if ((sectp->flags & SEC_RELOC) == 0)
    return NULL;

  /* We will handle section offsets properly elsewhere, so relocate as if
     all sections begin at 0.  */
  bfd_map_over_sections (abfd, symfile_dummy_outputs, NULL);

  return bfd_simple_get_relocated_section_contents (abfd, sectp, buf, NULL);
}

/* Relocate the contents of a debug section SECTP in ABFD.  The
   contents are stored in BUF if it is non-NULL, or returned in a
   malloc'd buffer otherwise.

   For some platforms and debug info formats, shared libraries contain
   relocations against the debug sections (particularly for DWARF-2;
   one affected platform is PowerPC GNU/Linux, although it depends on
   the version of the linker in use).  Also, ELF object files naturally
   have unresolved relocations for their debug sections.  We need to apply
   the relocations in order to get the locations of symbols correct.
   Another example that may require relocation processing, is the
   DWARF-2 .eh_frame section in .o files, although it isn't strictly a
   debug section.  */

bfd_byte *
symfile_relocate_debug_section (struct objfile *objfile,
                                asection *sectp, bfd_byte *buf)
{
  gdb_assert (objfile->sf->sym_relocate);

  return (*objfile->sf->sym_relocate) (objfile, sectp, buf);
}

struct symfile_segment_data *
get_symfile_segment_data (bfd *abfd)
{
  const struct sym_fns *sf = find_sym_fns (abfd);

  if (sf == NULL)
    return NULL;

  return sf->sym_segments (abfd);
}

void
free_symfile_segment_data (struct symfile_segment_data *data)
{
  xfree (data->segment_bases);
  xfree (data->segment_sizes);
  xfree (data->segment_info);
  xfree (data);
}


/* Given:
   - DATA, containing segment addresses from the object file ABFD, and
     the mapping from ABFD's sections onto the segments that own them,
     and
   - SEGMENT_BASES[0 .. NUM_SEGMENT_BASES - 1], holding the actual
     segment addresses reported by the target,
   store the appropriate offsets for each section in OFFSETS.

   If there are fewer entries in SEGMENT_BASES than there are segments
   in DATA, then apply SEGMENT_BASES' last entry to all the segments.

   If there are more entries, then ignore the extra.  The target may
   not be able to distinguish between an empty data segment and a
   missing data segment; a missing text segment is less plausible.  */
int
symfile_map_offsets_to_segments (bfd *abfd, struct symfile_segment_data *data,
				 struct section_offsets *offsets,
				 int num_segment_bases,
				 const CORE_ADDR *segment_bases)
{
  int i;
  asection *sect;

  /* It doesn't make sense to call this function unless you have some
     segment base addresses.  */
  gdb_assert (num_segment_bases > 0);

  /* If we do not have segment mappings for the object file, we
     can not relocate it by segments.  */
  gdb_assert (data != NULL);
  gdb_assert (data->num_segments > 0);

  for (i = 0, sect = abfd->sections; sect != NULL; i++, sect = sect->next)
    {
      int which = data->segment_info[i];

      gdb_assert (0 <= which && which <= data->num_segments);

      /* Don't bother computing offsets for sections that aren't
         loaded as part of any segment.  */
      if (! which)
        continue;

      /* Use the last SEGMENT_BASES entry as the address of any extra
         segments mentioned in DATA->segment_info.  */
      if (which > num_segment_bases)
        which = num_segment_bases;

      offsets->offsets[i] = (segment_bases[which - 1]
                             - data->segment_bases[which - 1]);
    }

  return 1;
}

static void
symfile_find_segment_sections (struct objfile *objfile)
{
  bfd *abfd = objfile->obfd;
  int i;
  asection *sect;
  struct symfile_segment_data *data;

  data = get_symfile_segment_data (objfile->obfd);
  if (data == NULL)
    return;

  if (data->num_segments != 1 && data->num_segments != 2)
    {
      free_symfile_segment_data (data);
      return;
    }

  for (i = 0, sect = abfd->sections; sect != NULL; i++, sect = sect->next)
    {
      int which = data->segment_info[i];

      if (which == 1)
	{
	  if (objfile->sect_index_text == -1)
	    objfile->sect_index_text = sect->index;

	  if (objfile->sect_index_rodata == -1)
	    objfile->sect_index_rodata = sect->index;
	}
      else if (which == 2)
	{
	  if (objfile->sect_index_data == -1)
	    objfile->sect_index_data = sect->index;

	  if (objfile->sect_index_bss == -1)
	    objfile->sect_index_bss = sect->index;
	}
    }

  free_symfile_segment_data (data);
}

void
_initialize_symfile (void)
{
  struct cmd_list_element *c;

  c = add_cmd ("symbol-file", class_files, symbol_file_command, _("\
Load symbol table from executable file FILE.\n\
The `file' command can also load symbol tables, as well as setting the file\n\
to execute."), &cmdlist);
  set_cmd_completer (c, filename_completer);

  c = add_cmd ("add-symbol-file", class_files, add_symbol_file_command, _("\
Load symbols from FILE, assuming FILE has been dynamically loaded.\n\
Usage: add-symbol-file FILE ADDR [-s <SECT> <SECT_ADDR> -s <SECT> <SECT_ADDR>\
 ...]\nADDR is the starting address of the file's text.\n\
The optional arguments are section-name section-address pairs and\n\
should be specified if the data and bss segments are not contiguous\n\
with the text.  SECT is a section name to be loaded at SECT_ADDR."),
	       &cmdlist);
  set_cmd_completer (c, filename_completer);

  c = add_cmd ("load", class_files, load_command, _("\
Dynamically load FILE into the running program, and record its symbols\n\
for access from GDB.\n\
A load OFFSET may also be given."), &cmdlist);
  set_cmd_completer (c, filename_completer);

  add_prefix_cmd ("overlay", class_support, overlay_command,
		  _("Commands for debugging overlays."), &overlaylist,
		  "overlay ", 0, &cmdlist);

  add_com_alias ("ovly", "overlay", class_alias, 1);
  add_com_alias ("ov", "overlay", class_alias, 1);

  add_cmd ("map-overlay", class_support, map_overlay_command,
	   _("Assert that an overlay section is mapped."), &overlaylist);

  add_cmd ("unmap-overlay", class_support, unmap_overlay_command,
	   _("Assert that an overlay section is unmapped."), &overlaylist);

  add_cmd ("list-overlays", class_support, list_overlays_command,
	   _("List mappings of overlay sections."), &overlaylist);

  add_cmd ("manual", class_support, overlay_manual_command,
	   _("Enable overlay debugging."), &overlaylist);
  add_cmd ("off", class_support, overlay_off_command,
	   _("Disable overlay debugging."), &overlaylist);
  add_cmd ("auto", class_support, overlay_auto_command,
	   _("Enable automatic overlay debugging."), &overlaylist);
  add_cmd ("load-target", class_support, overlay_load_command,
	   _("Read the overlay mapping state from the target."), &overlaylist);

  /* Filename extension to source language lookup table: */
  init_filename_language_table ();
  add_setshow_string_noescape_cmd ("extension-language", class_files,
				   &ext_args, _("\
Set mapping between filename extension and source language."), _("\
Show mapping between filename extension and source language."), _("\
Usage: set extension-language .foo bar"),
				   set_ext_lang_command,
				   show_ext_args,
				   &setlist, &showlist);

  add_info ("extensions", info_ext_lang_command,
	    _("All filename extensions associated with a source language."));

  add_setshow_optional_filename_cmd ("debug-file-directory", class_support,
				     &debug_file_directory, _("\
Set the directories where separate debug symbols are searched for."), _("\
Show the directories where separate debug symbols are searched for."), _("\
Separate debug symbols are first searched for in the same\n\
directory as the binary, then in the `" DEBUG_SUBDIRECTORY "' subdirectory,\n\
and lastly at the path of the directory of the binary with\n\
each global debug-file-directory component prepended."),
				     NULL,
				     show_debug_file_directory,
				     &setlist, &showlist);
}
