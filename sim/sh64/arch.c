/* Simulator support for sh.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

This file is part of the GNU Simulators.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#include "sim-main.h"
#include "bfd.h"

const MACH *sim_machs[] =
{
#ifdef HAVE_CPU_SH64
  & sh2_mach,
#endif
#ifdef HAVE_CPU_SH64
  & sh3_mach,
#endif
#ifdef HAVE_CPU_SH64
  & sh3e_mach,
#endif
#ifdef HAVE_CPU_SH64
  & sh4_mach,
#endif
#ifdef HAVE_CPU_SH64
  & sh5_mach,
#endif
  0
};

