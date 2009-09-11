/* Disassembler structures definitions for the ARC.
   Copyright 2009
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef ARCOMPACT_DIS_H
#define ARCOMPACT_DIS_H


void print_arc_disassembler_options (FILE *stream);

disassembler_ftype arcompact_get_disassembler (void *);

struct arcDisState
arcAnalyzeInstr(bfd_vma           address,
                disassemble_info* info);

int ARCompact_decodeInstr (bfd_vma address, disassemble_info* info);


#endif /* ARCOMPACT_DIS_H */
