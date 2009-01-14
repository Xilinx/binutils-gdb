/* Profiling definitions for the fr550 model of the FRV simulator
   Copyright (C) 2003, 2007, 2008, 2009 Free Software Foundation, Inc.
   Contributed by Red Hat.

This file is part of the GNU Simulators.

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

#ifndef PROFILE_FR550_H
#define PROFILE_FR550_H

void fr550_model_insn_before (SIM_CPU *, int);
void fr550_model_insn_after (SIM_CPU *, int, int);

#endif /* PROFILE_FR550_H */
