# This shell script emits a C file. -*- C -*-
#   Copyright 2004
#   Free Software Foundation, Inc.
#
# This file is part of GLD, the Gnu Linker.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

# This file is sourced from elf32.em, and defines extra crx-elf
# specific routines.
#
cat >>e${EMULATION_NAME}.c <<EOF

#include "ldctor.h"

static void crxelf_after_parse (void);

static void
crxelf_after_parse (void)
{
  /* Always behave as if called with --sort-common command line
     option.
     This is to emulate the CRTools' method of keeping variables
     of different alignment in separate sections.  */
  config.sort_common = TRUE;

  /* Don't create a demand-paged executable, since this feature isn't
     meaninful in CR16C embedded systems. Moreover, when magic_demand_paged
     is true the link sometimes fails.  */
  config.magic_demand_paged = FALSE;
}

EOF

# Put these extra crx-elf routines in ld_${EMULATION_NAME}_emulation
#
LDEMUL_AFTER_PARSE=crxelf_after_parse
