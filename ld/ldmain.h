/* ldmain.h -
   Copyright 1991, 1992, 1993, 1994, 1995, 1996, 1999, 2002, 2003, 2004,
   2005, 2007, 2008 Free Software Foundation, Inc.

   This file is part of the GNU Binutils.

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

#ifndef LDMAIN_H
#define LDMAIN_H

extern char *program_name;
extern const char *ld_sysroot;
extern char *ld_canon_sysroot;
extern int ld_canon_sysroot_len;
extern FILE *saved_script_handle;
extern FILE *previous_script_handle;
extern bfd_boolean force_make_executable;
extern char *default_target;
extern bfd_boolean trace_files;
extern bfd_boolean trace_file_tries;
extern bfd_boolean version_printed;
extern bfd_boolean whole_archive;
extern bfd_boolean as_needed;
extern bfd_boolean add_needed;
extern bfd_boolean demangling;
extern int g_switch_value;
extern const char *output_filename;
extern struct bfd_link_info link_info;
extern int overflow_cutoff_limit;

extern void add_ysym (const char *);
extern void add_wrap (const char *);
extern void add_keepsyms_file (const char *);

#endif
