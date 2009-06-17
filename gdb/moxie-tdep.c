/* Target-dependent code for Moxie.

   Copyright (C) 2009 Free Software Foundation, Inc.

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
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdb_string.h"
#include "value.h"
#include "inferior.h"
#include "symfile.h"
#include "objfiles.h"
#include "osabi.h"
#include "language.h"
#include "arch-utils.h"
#include "regcache.h"
#include "trad-frame.h"
#include "dis-asm.h"

#include "gdb_assert.h"

#include "moxie-tdep.h"

/* Local functions.  */

extern void _initialize_moxie_tdep (void);

/* Use an invalid address value as 'not available' marker.  */
enum { REG_UNAVAIL = (CORE_ADDR) -1 };

struct moxie_frame_cache
{
  /* Base address.  */
  CORE_ADDR base;
  CORE_ADDR pc;
  LONGEST framesize;
  CORE_ADDR saved_regs[MOXIE_NUM_REGS];
  CORE_ADDR saved_sp;
};

/* Implement the "frame_align" gdbarch method.  */

static CORE_ADDR
moxie_frame_align (struct gdbarch *gdbarch, CORE_ADDR sp)
{
  /* Align to the size of an instruction (so that they can safely be
     pushed onto the stack.  */
  return sp & ~1;
}

/* Implement the "breakpoint_from_pc" gdbarch method.  */

const static unsigned char *
moxie_breakpoint_from_pc (struct gdbarch *gdbarch, 
			  CORE_ADDR *pcptr, int *lenptr)
{
  static unsigned char breakpoint[] = { 0x35, 0x00 };

  *lenptr = sizeof (breakpoint);
  return breakpoint;
}

/* Moxie register names.  */

char *moxie_register_names[] = {
  "$fp",  "$sp",  "$r0",  "$r1",  "$r2",
  "$r3",  "$r4",  "$r5", "$r6", "$r7",
  "$r8", "$r9", "$r10", "$r11", "$r12",
  "$r13", "$pc", "$cc" };

/* Implement the "register_name" gdbarch method.  */

static const char *
moxie_register_name (struct gdbarch *gdbarch, int reg_nr)
{
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= MOXIE_NUM_REGS)
    return NULL;
  return moxie_register_names[reg_nr];
}

/* Implement the "register_type" gdbarch method.  */

static struct type *
moxie_register_type (struct gdbarch *gdbarch, int reg_nr)
{
  if (reg_nr == MOXIE_PC_REGNUM)
    return  builtin_type (gdbarch)->builtin_func_ptr;
  else if (reg_nr == MOXIE_SP_REGNUM || reg_nr == MOXIE_FP_REGNUM)
    return builtin_type (gdbarch)->builtin_data_ptr;
  else
    return builtin_type_int32;
}

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.  */

static void
moxie_store_return_value (struct type *type, struct regcache *regcache,
			 const void *valbuf)
{
  CORE_ADDR regval;
  int len = TYPE_LENGTH (type);

  /* Things always get returned in RET1_REGNUM, RET2_REGNUM.  */
  regval = extract_unsigned_integer (valbuf, len > 4 ? 4 : len);
  regcache_cooked_write_unsigned (regcache, RET1_REGNUM, regval);
  if (len > 4)
    {
      regval = extract_unsigned_integer ((gdb_byte *) valbuf + 4, len - 4);
      regcache_cooked_write_unsigned (regcache, RET1_REGNUM + 1, regval);
    }
}

/* Decode the instructions within the given address range.  Decide
   when we must have reached the end of the function prologue.  If a
   frame_info pointer is provided, fill in its saved_regs etc.

   Returns the address of the first instruction after the prologue.  */

static CORE_ADDR
moxie_analyze_prologue (CORE_ADDR start_addr, CORE_ADDR end_addr,
		      struct moxie_frame_cache *cache,
		      struct frame_info *this_frame)
{
  CORE_ADDR next_addr;
  ULONGEST inst, inst2;
  LONGEST offset;
  int regnum;

  /* Record where the jsra instruction saves the PC and FP.  */
  cache->saved_regs[MOXIE_PC_REGNUM] = -4;
  cache->saved_regs[MOXIE_FP_REGNUM] = 0;
  cache->framesize = 0;

  if (start_addr >= end_addr)
    return end_addr;

  for (next_addr = start_addr; next_addr < end_addr; )
    {
      inst = read_memory_unsigned_integer (next_addr, 2);

      /* Match "push $rN" where N is between 2 and 13 inclusive.  */
      if (inst >= 0x0614 && inst <= 0x061f)
	{
	  regnum = inst & 0x000f;
	  cache->framesize += 4;
	  cache->saved_regs[regnum] = cache->framesize;
	  next_addr += 2;
	}

      /* Optional stack allocation for args and local vars <= 4
	 byte.  */
      else if (inst == 0x01f0)           /* ldi.l $r12, X */
	{
	  offset = read_memory_integer (next_addr + 2, 4);
	  inst2 = read_memory_unsigned_integer (next_addr + 6, 2);

	  if (inst2 == 0x051f)           /* add.l $sp, $r12 */
	    {
	      cache->framesize += offset;
	    }

	  return (next_addr + 8);
	}
      else  /* This is not a prologue instruction.  */
	break;
    }

  return next_addr;
}

/* Find the end of function prologue.  */

static CORE_ADDR
moxie_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  CORE_ADDR func_addr = 0, func_end = 0;
  char *func_name;

  /* See if we can determine the end of the prologue via the symbol table.
     If so, then return either PC, or the PC after the prologue, whichever
     is greater.  */
  if (find_pc_partial_function (pc, &func_name, &func_addr, &func_end))
    {
      CORE_ADDR post_prologue_pc
	= skip_prologue_using_sal (gdbarch, func_addr);
      if (post_prologue_pc != 0)
	return max (pc, post_prologue_pc);
      else
	{
	  /* Can't determine prologue from the symbol table, need to examine
	     instructions.  */
	  struct symtab_and_line sal;
	  struct symbol *sym;
	  struct moxie_frame_cache cache;
	  CORE_ADDR plg_end;
	  
	  memset (&cache, 0, sizeof cache);
	  
	  plg_end = moxie_analyze_prologue (func_addr, 
					    func_end, &cache, NULL);
	  /* Found a function.  */
	  sym = lookup_symbol (func_name, NULL, VAR_DOMAIN, NULL);
	  /* Don't use line number debug info for assembly source
	     files. */
	  if (sym && SYMBOL_LANGUAGE (sym) != language_asm)
	    {
	      sal = find_pc_line (func_addr, 0);
	      if (sal.end && sal.end < func_end)
		{
		  /* Found a line number, use it as end of
		     prologue.  */
		  return sal.end;
		}
	    }
	  /* No useable line symbol.  Use result of prologue parsing
	     method.  */
	  return plg_end;
	}
    }

  /* No function symbol -- just return the PC.  */
  return (CORE_ADDR) pc;
}

struct moxie_unwind_cache
{
  /* The previous frame's inner most stack address.  Used as this
     frame ID's stack_addr.  */
  CORE_ADDR prev_sp;
  /* The frame's base, optionally used by the high-level debug info.  */
  CORE_ADDR base;
  int size;
  /* How far the SP and r13 (FP) have been offset from the start of
     the stack frame (as defined by the previous frame's stack
     pointer).  */
  LONGEST sp_offset;
  LONGEST r13_offset;
  int uses_frame;
  /* Table indicating the location of each and every register.  */
  struct trad_frame_saved_reg *saved_regs;
};

/* Implement the "read_pc" gdbarch method.  */

static CORE_ADDR
moxie_read_pc (struct regcache *regcache)
{
  ULONGEST pc;

  regcache_cooked_read_unsigned (regcache, MOXIE_PC_REGNUM, &pc);
  return pc;
}

/* Implement the "write_pc" gdbarch method.  */

static void
moxie_write_pc (struct regcache *regcache, CORE_ADDR val)
{
  regcache_cooked_write_unsigned (regcache, MOXIE_PC_REGNUM, val);
}

/* Implement the "unwind_pc" gdbarch method.  */

static CORE_ADDR
moxie_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_unsigned (next_frame, MOXIE_SP_REGNUM);
}

/* Given a return value in `regbuf' with a type `valtype', 
   extract and copy its value into `valbuf'.  */

static void
moxie_extract_return_value (struct type *type, struct regcache *regcache,
			   void *dst)
{
  bfd_byte *valbuf = dst;
  int len = TYPE_LENGTH (type);
  ULONGEST tmp;

  /* By using store_unsigned_integer we avoid having to do
     anything special for small big-endian values.  */
  regcache_cooked_read_unsigned (regcache, RET1_REGNUM, &tmp);
  store_unsigned_integer (valbuf, (len > 4 ? len - 4 : len), tmp);

  /* Ignore return values more than 8 bytes in size because the moxie
     returns anything more than 8 bytes in the stack.  */
  if (len > 4)
    {
      regcache_cooked_read_unsigned (regcache, RET1_REGNUM + 1, &tmp);
      store_unsigned_integer (valbuf + len - 4, 4, tmp);
    }
}

/* Implement the "return_value" gdbarch method.  */

static enum return_value_convention
moxie_return_value (struct gdbarch *gdbarch, struct type *func_type,
		   struct type *valtype, struct regcache *regcache,
		   gdb_byte *readbuf, const gdb_byte *writebuf)
{
  if (TYPE_LENGTH (valtype) > 8)
    return RETURN_VALUE_STRUCT_CONVENTION;
  else
    {
      if (readbuf != NULL)
	moxie_extract_return_value (valtype, regcache, readbuf);
      if (writebuf != NULL)
	moxie_store_return_value (valtype, regcache, writebuf);
      return RETURN_VALUE_REGISTER_CONVENTION;
    }
}

/* Allocate and initialize a moxie_frame_cache object.  */

static struct moxie_frame_cache *
moxie_alloc_frame_cache (void)
{
  struct moxie_frame_cache *cache;
  int i;

  cache = FRAME_OBSTACK_ZALLOC (struct moxie_frame_cache);

  cache->base = 0;
  cache->saved_sp = 0;
  cache->pc = 0;
  cache->framesize = 0;
  for (i = 0; i < MOXIE_NUM_REGS; ++i)
    cache->saved_regs[i] = REG_UNAVAIL;

  return cache;
}

/* Populate a moxie_frame_cache object for this_frame.  */

static struct moxie_frame_cache *
moxie_frame_cache (struct frame_info *this_frame, void **this_cache)
{
  struct moxie_frame_cache *cache;
  CORE_ADDR current_pc;
  int i;

  if (*this_cache)
    return *this_cache;

  cache = moxie_alloc_frame_cache ();
  *this_cache = cache;

  cache->base = get_frame_register_unsigned (this_frame, MOXIE_FP_REGNUM);
  if (cache->base == 0)
    return cache;

  cache->pc = get_frame_func (this_frame);
  current_pc = get_frame_pc (this_frame);
  if (cache->pc)
    moxie_analyze_prologue (cache->pc, current_pc, cache, this_frame);

  cache->saved_sp = cache->base - cache->framesize;

  for (i = 0; i < MOXIE_NUM_REGS; ++i)
    if (cache->saved_regs[i] != REG_UNAVAIL)
      cache->saved_regs[i] = cache->base - cache->saved_regs[i];

  return cache;
}

/* Implement the "unwind_pc" gdbarch method.  */

static CORE_ADDR
moxie_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_unsigned (next_frame, MOXIE_PC_REGNUM);
}

/* Given a GDB frame, determine the address of the calling function's
   frame.  This will be used to create a new GDB frame struct.  */

static void
moxie_frame_this_id (struct frame_info *this_frame,
		    void **this_prologue_cache, struct frame_id *this_id)
{
  struct moxie_frame_cache *cache = moxie_frame_cache (this_frame,
						   this_prologue_cache);

  /* This marks the outermost frame.  */
  if (cache->base == 0)
    return;

  *this_id = frame_id_build (cache->saved_sp, cache->pc);
}

/* Get the value of register regnum in the previous stack frame.  */

static struct value *
moxie_frame_prev_register (struct frame_info *this_frame,
			  void **this_prologue_cache, int regnum)
{
  struct moxie_frame_cache *cache = moxie_frame_cache (this_frame,
						   this_prologue_cache);

  gdb_assert (regnum >= 0);

  if (regnum == MOXIE_SP_REGNUM && cache->saved_sp)
    return frame_unwind_got_constant (this_frame, regnum, cache->saved_sp);

  if (regnum < MOXIE_NUM_REGS && cache->saved_regs[regnum] != REG_UNAVAIL)
    return frame_unwind_got_memory (this_frame, regnum,
				    cache->saved_regs[regnum]);

  return frame_unwind_got_register (this_frame, regnum, regnum);
}

static const struct frame_unwind moxie_frame_unwind = {
  NORMAL_FRAME,
  moxie_frame_this_id,
  moxie_frame_prev_register,
  NULL,
  default_frame_sniffer
};

/* Return the base address of this_frame.  */

static CORE_ADDR
moxie_frame_base_address (struct frame_info *this_frame, void **this_cache)
{
  struct moxie_frame_cache *cache = moxie_frame_cache (this_frame,
						       this_cache);

  return cache->base;
}

static const struct frame_base moxie_frame_base = {
  &moxie_frame_unwind,
  moxie_frame_base_address,
  moxie_frame_base_address,
  moxie_frame_base_address
};

static struct frame_id
moxie_dummy_id (struct gdbarch *gdbarch, struct frame_info *this_frame)
{
  CORE_ADDR sp = get_frame_register_unsigned (this_frame, MOXIE_SP_REGNUM);

  return frame_id_build (sp, get_frame_pc (this_frame));
}

/* Allocate and initialize the moxie gdbarch object.  */

static struct gdbarch *
moxie_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;

  /* If there is already a candidate, use it.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  /* Allocate space for the new architecture.  */
  tdep = XMALLOC (struct gdbarch_tdep);
  gdbarch = gdbarch_alloc (&info, tdep);

  set_gdbarch_read_pc (gdbarch, moxie_read_pc);
  set_gdbarch_write_pc (gdbarch, moxie_write_pc);
  set_gdbarch_unwind_sp (gdbarch, moxie_unwind_sp);

  set_gdbarch_num_regs (gdbarch, MOXIE_NUM_REGS);
  set_gdbarch_sp_regnum (gdbarch, MOXIE_SP_REGNUM);
  set_gdbarch_register_name (gdbarch, moxie_register_name);
  set_gdbarch_register_type (gdbarch, moxie_register_type);

  set_gdbarch_return_value (gdbarch, moxie_return_value);

  set_gdbarch_skip_prologue (gdbarch, moxie_skip_prologue);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_breakpoint_from_pc (gdbarch, moxie_breakpoint_from_pc);
  set_gdbarch_frame_align (gdbarch, moxie_frame_align);

  frame_base_set_default (gdbarch, &moxie_frame_base);

  /* Methods for saving / extracting a dummy frame's ID.  The ID's
     stack address must match the SP value returned by
     PUSH_DUMMY_CALL, and saved by generic_save_dummy_frame_tos.  */
  set_gdbarch_dummy_id (gdbarch, moxie_dummy_id);

  set_gdbarch_unwind_pc (gdbarch, moxie_unwind_pc);

  set_gdbarch_print_insn (gdbarch, print_insn_moxie);

  /* Hook in ABI-specific overrides, if they have been registered.  */
  gdbarch_init_osabi (info, gdbarch);

  /* Hook in the default unwinders.  */
  frame_unwind_append_unwinder (gdbarch, &moxie_frame_unwind);

  /* Support simple overlay manager.  */
  set_gdbarch_overlay_update (gdbarch, simple_overlay_update);

  return gdbarch;
}

/* Register this machine's init routine.  */

void
_initialize_moxie_tdep (void)
{
  register_gdbarch_init (bfd_arch_moxie, moxie_gdbarch_init);
}
