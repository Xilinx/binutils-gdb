/* Definitions to make GDB run on a vax under 4.2bsd.
   Copyright 1986, 1987, 1989, 1991, 1993, 1994, 1996, 1998, 1999, 2000, 2002
   Free Software Foundation, Inc.

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

#define GDB_MULTI_ARCH GDB_MULTI_ARCH_PARTIAL

#define TARGET_UPAGES 14
#define TARGET_NBPG 512
#define STACK_END_ADDR (0x80000000 - (TARGET_UPAGES * TARGET_NBPG))

/* On the VAX, sigtramp is in the u area.  Can't check the exact
   addresses because for cross-debugging we don't have VAX include
   files around.  This should be close enough.  */
#define SIGTRAMP_START(pc)	STACK_END_ADDR
#define SIGTRAMP_END(pc)	0x80000000

/* Sequence of bytes for breakpoint instruction.  */
#define BREAKPOINT {3}

#define AP_REGNUM 12	/* XXXJRT */

/* Offset to saved PC in sigcontext, from <sys/signal.h>.  */
/* XXXJRT should go away */
#define SIGCONTEXT_PC_OFFSET 12

/* XXXJRT not yet under gdbarch control */
#define FRAME_ARGS_ADDRESS_CORRECT(fi) vax_frame_args_address_correct ((fi))
extern CORE_ADDR vax_frame_args_address_correct (struct frame_info *);

/* If vax pcc says CHAR or SHORT, it provides the correct address.  */
#define BELIEVE_PCC_PROMOTION 1
