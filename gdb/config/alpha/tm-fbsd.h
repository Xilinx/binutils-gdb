/* Target-dependent definitions for FreeBSD/Alpha.
   Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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

#ifndef TM_FBSD_H
#define TM_FBSD_H

#include "alpha/tm-alpha.h"

/* Number of traps that happen between exec'ing the shell to run an
   inferior, and when we finally get to the inferior code.  The
   default is right for FreeBSD.  */

#undef START_INFERIOR_TRAPS_EXPECTED

#endif /* TM_FBSD_H */
