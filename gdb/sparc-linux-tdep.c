/* Target-dependent code for GNU/Linux SPARC.

   Copyright 2003 Free Software Foundation, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "gdbarch.h"
#include "osabi.h"
#include "solib-svr4.h"

#include "sparc-tdep.h"

static struct link_map_offsets *
sparc32_linux_svr4_fetch_link_map_offsets (void)
{
  static struct link_map_offsets lmo;
  static struct link_map_offsets *lmp = NULL;

  if (lmp == NULL)
    {
      lmp = &lmo;

      /* Everything we need is in the first 8 bytes.  */
      lmo.r_debug_size = 8;
      lmo.r_map_offset = 4;
      lmo.r_map_size   = 4;

      /* Everything we need is in the first 20 bytes.  */
      lmo.link_map_size = 20;
      lmo.l_addr_offset = 0;
      lmo.l_addr_size   = 4;
      lmo.l_name_offset = 4;
      lmo.l_name_size   = 4;
      lmo.l_next_offset = 12;
      lmo.l_next_size   = 4;
      lmo.l_prev_offset = 16;
      lmo.l_prev_size   = 4;
    }

  return lmp;
}

static void
sparc32_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  /* GNU/Linux is very similar to Solaris ...  */
  sparc32_sol2_init_abi (info, gdbarch);

  /* ... but doesn't have kernel-assisted single-stepping support.  */
  set_gdbarch_software_single_step (gdbarch, sparc_software_single_step);

  set_solib_svr4_fetch_link_map_offsets
    (gdbarch, sparc32_linux_svr4_fetch_link_map_offsets);
}

/* Provide a prototype to silence -Wmissing-prototypes.  */
extern void _initialize_sparc_linux_tdep (void);

void
_initialize_sparc_linux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_sparc, 0, GDB_OSABI_LINUX,
			  sparc32_linux_init_abi);
}
