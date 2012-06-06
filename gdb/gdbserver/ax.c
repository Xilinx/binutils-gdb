/* Agent expression code for remote server.
   Copyright (C) 2009-2012 Free Software Foundation, Inc.

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

#include "server.h"
#include "ax.h"

static void ax_vdebug (const char *, ...) ATTR_FORMAT (printf, 1, 2);

#ifdef IN_PROCESS_AGENT
int debug_agent = 0;
#endif

static void
ax_vdebug (const char *fmt, ...)
{
  char buf[1024];
  va_list ap;

  va_start (ap, fmt);
  vsprintf (buf, fmt, ap);
  fprintf (stderr, PROG "/ax: %s\n", buf);
  va_end (ap);
}

#define ax_debug_1(level, fmt, args...)	\
  do {						\
    if (level <= debug_threads)			\
      ax_vdebug ((fmt), ##args);		\
  } while (0)

#define ax_debug(FMT, args...)		\
  ax_debug_1 (1, FMT, ##args)

/* This enum must exactly match what is documented in
   gdb/doc/agentexpr.texi, including all the numerical values.  */

enum gdb_agent_op
  {
#define DEFOP(NAME, SIZE, DATA_SIZE, CONSUMED, PRODUCED, VALUE)  \
    gdb_agent_op_ ## NAME = VALUE,
#include "ax.def"
#undef DEFOP
    gdb_agent_op_last
  };

static const char *gdb_agent_op_names [gdb_agent_op_last] =
  {
    "?undef?"
#define DEFOP(NAME, SIZE, DATA_SIZE, CONSUMED, PRODUCED, VALUE)  , # NAME
#include "ax.def"
#undef DEFOP
  };

static const unsigned char gdb_agent_op_sizes [gdb_agent_op_last] =
  {
    0
#define DEFOP(NAME, SIZE, DATA_SIZE, CONSUMED, PRODUCED, VALUE)  , SIZE
#include "ax.def"
#undef DEFOP
  };

/* A wrapper for gdb_agent_op_names that does some bounds-checking.  */

static const char *
gdb_agent_op_name (int op)
{
  if (op < 0 || op >= gdb_agent_op_last || gdb_agent_op_names[op] == NULL)
    return "?undef?";
  return gdb_agent_op_names[op];
}

#ifndef IN_PROCESS_AGENT

/* The packet form of an agent expression consists of an 'X', number
   of bytes in expression, a comma, and then the bytes.  */

struct agent_expr *
gdb_parse_agent_expr (char **actparm)
{
  char *act = *actparm;
  ULONGEST xlen;
  struct agent_expr *aexpr;

  ++act;  /* skip the X */
  act = unpack_varlen_hex (act, &xlen);
  ++act;  /* skip a comma */
  aexpr = xmalloc (sizeof (struct agent_expr));
  aexpr->length = xlen;
  aexpr->bytes = xmalloc (xlen);
  convert_ascii_to_int (act, aexpr->bytes, xlen);
  *actparm = act + (xlen * 2);
  return aexpr;
}

/* Convert the bytes of an agent expression back into hex digits, so
   they can be printed or uploaded.  This allocates the buffer,
   callers should free when they are done with it.  */

char *
gdb_unparse_agent_expr (struct agent_expr *aexpr)
{
  char *rslt;

  rslt = xmalloc (2 * aexpr->length + 1);
  convert_int_to_ascii (aexpr->bytes, rslt, aexpr->length);
  return rslt;
}

/* Bytecode compilation.  */

CORE_ADDR current_insn_ptr;

int emit_error;

struct bytecode_address
{
  int pc;
  CORE_ADDR address;
  int goto_pc;
  /* Offset and size of field to be modified in the goto block.  */
  int from_offset, from_size;
  struct bytecode_address *next;
} *bytecode_address_table;

void
emit_prologue (void)
{
  target_emit_ops ()->emit_prologue ();
}

void
emit_epilogue (void)
{
  target_emit_ops ()->emit_epilogue ();
}

static void
emit_add (void)
{
  target_emit_ops ()->emit_add ();
}

static void
emit_sub (void)
{
  target_emit_ops ()->emit_sub ();
}

static void
emit_mul (void)
{
  target_emit_ops ()->emit_mul ();
}

static void
emit_lsh (void)
{
  target_emit_ops ()->emit_lsh ();
}

static void
emit_rsh_signed (void)
{
  target_emit_ops ()->emit_rsh_signed ();
}

static void
emit_rsh_unsigned (void)
{
  target_emit_ops ()->emit_rsh_unsigned ();
}

static void
emit_ext (int arg)
{
  target_emit_ops ()->emit_ext (arg);
}

static void
emit_log_not (void)
{
  target_emit_ops ()->emit_log_not ();
}

static void
emit_bit_and (void)
{
  target_emit_ops ()->emit_bit_and ();
}

static void
emit_bit_or (void)
{
  target_emit_ops ()->emit_bit_or ();
}

static void
emit_bit_xor (void)
{
  target_emit_ops ()->emit_bit_xor ();
}

static void
emit_bit_not (void)
{
  target_emit_ops ()->emit_bit_not ();
}

static void
emit_equal (void)
{
  target_emit_ops ()->emit_equal ();
}

static void
emit_less_signed (void)
{
  target_emit_ops ()->emit_less_signed ();
}

static void
emit_less_unsigned (void)
{
  target_emit_ops ()->emit_less_unsigned ();
}

static void
emit_ref (int size)
{
  target_emit_ops ()->emit_ref (size);
}

static void
emit_if_goto (int *offset_p, int *size_p)
{
  target_emit_ops ()->emit_if_goto (offset_p, size_p);
}

static void
emit_goto (int *offset_p, int *size_p)
{
  target_emit_ops ()->emit_goto (offset_p, size_p);
}

static void
write_goto_address (CORE_ADDR from, CORE_ADDR to, int size)
{
  target_emit_ops ()->write_goto_address (from, to, size);
}

static void
emit_const (LONGEST num)
{
  target_emit_ops ()->emit_const (num);
}

static void
emit_reg (int reg)
{
  target_emit_ops ()->emit_reg (reg);
}

static void
emit_pop (void)
{
  target_emit_ops ()->emit_pop ();
}

static void
emit_stack_flush (void)
{
  target_emit_ops ()->emit_stack_flush ();
}

static void
emit_zero_ext (int arg)
{
  target_emit_ops ()->emit_zero_ext (arg);
}

static void
emit_swap (void)
{
  target_emit_ops ()->emit_swap ();
}

static void
emit_stack_adjust (int n)
{
  target_emit_ops ()->emit_stack_adjust (n);
}

/* FN's prototype is `LONGEST(*fn)(int)'.  */

static void
emit_int_call_1 (CORE_ADDR fn, int arg1)
{
  target_emit_ops ()->emit_int_call_1 (fn, arg1);
}

/* FN's prototype is `void(*fn)(int,LONGEST)'.  */

static void
emit_void_call_2 (CORE_ADDR fn, int arg1)
{
  target_emit_ops ()->emit_void_call_2 (fn, arg1);
}

static void
emit_eq_goto (int *offset_p, int *size_p)
{
  target_emit_ops ()->emit_eq_goto (offset_p, size_p);
}

static void
emit_ne_goto (int *offset_p, int *size_p)
{
  target_emit_ops ()->emit_ne_goto (offset_p, size_p);
}

static void
emit_lt_goto (int *offset_p, int *size_p)
{
  target_emit_ops ()->emit_lt_goto (offset_p, size_p);
}

static void
emit_ge_goto (int *offset_p, int *size_p)
{
  target_emit_ops ()->emit_ge_goto (offset_p, size_p);
}

static void
emit_gt_goto (int *offset_p, int *size_p)
{
  target_emit_ops ()->emit_gt_goto (offset_p, size_p);
}

static void
emit_le_goto (int *offset_p, int *size_p)
{
  target_emit_ops ()->emit_le_goto (offset_p, size_p);
}

/* Scan an agent expression for any evidence that the given PC is the
   target of a jump bytecode in the expression.  */

int
is_goto_target (struct agent_expr *aexpr, int pc)
{
  int i;
  unsigned char op;

  for (i = 0; i < aexpr->length; i += 1 + gdb_agent_op_sizes[op])
    {
      op = aexpr->bytes[i];

      if (op == gdb_agent_op_goto || op == gdb_agent_op_if_goto)
	{
	  int target = (aexpr->bytes[i + 1] << 8) + aexpr->bytes[i + 2];
	  if (target == pc)
	    return 1;
	}
    }

  return 0;
}

/* Given an agent expression, turn it into native code.  */

enum eval_result_type
compile_bytecodes (struct agent_expr *aexpr)
{
  int pc = 0;
  int done = 0;
  unsigned char op, next_op;
  int arg;
  /* This is only used to build 64-bit value for constants.  */
  ULONGEST top;
  struct bytecode_address *aentry, *aentry2;

#define UNHANDLED					\
  do							\
    {							\
      ax_debug ("Cannot compile op 0x%x\n", op);	\
      return expr_eval_unhandled_opcode;		\
    } while (0)

  if (aexpr->length == 0)
    {
      ax_debug ("empty agent expression\n");
      return expr_eval_empty_expression;
    }

  bytecode_address_table = NULL;

  while (!done)
    {
      op = aexpr->bytes[pc];

      ax_debug ("About to compile op 0x%x, pc=%d\n", op, pc);

      /* Record the compiled-code address of the bytecode, for use by
	 jump instructions.  */
      aentry = xmalloc (sizeof (struct bytecode_address));
      aentry->pc = pc;
      aentry->address = current_insn_ptr;
      aentry->goto_pc = -1;
      aentry->from_offset = aentry->from_size = 0;
      aentry->next = bytecode_address_table;
      bytecode_address_table = aentry;

      ++pc;

      emit_error = 0;

      switch (op)
	{
	case gdb_agent_op_add:
	  emit_add ();
	  break;

	case gdb_agent_op_sub:
	  emit_sub ();
	  break;

	case gdb_agent_op_mul:
	  emit_mul ();
	  break;

	case gdb_agent_op_div_signed:
	  UNHANDLED;
	  break;

	case gdb_agent_op_div_unsigned:
	  UNHANDLED;
	  break;

	case gdb_agent_op_rem_signed:
	  UNHANDLED;
	  break;

	case gdb_agent_op_rem_unsigned:
	  UNHANDLED;
	  break;

	case gdb_agent_op_lsh:
	  emit_lsh ();
	  break;

	case gdb_agent_op_rsh_signed:
	  emit_rsh_signed ();
	  break;

	case gdb_agent_op_rsh_unsigned:
	  emit_rsh_unsigned ();
	  break;

	case gdb_agent_op_trace:
	  UNHANDLED;
	  break;

	case gdb_agent_op_trace_quick:
	  UNHANDLED;
	  break;

	case gdb_agent_op_log_not:
	  emit_log_not ();
	  break;

	case gdb_agent_op_bit_and:
	  emit_bit_and ();
	  break;

	case gdb_agent_op_bit_or:
	  emit_bit_or ();
	  break;

	case gdb_agent_op_bit_xor:
	  emit_bit_xor ();
	  break;

	case gdb_agent_op_bit_not:
	  emit_bit_not ();
	  break;

	case gdb_agent_op_equal:
	  next_op = aexpr->bytes[pc];
	  if (next_op == gdb_agent_op_if_goto
	      && !is_goto_target (aexpr, pc)
	      && target_emit_ops ()->emit_eq_goto)
	    {
	      ax_debug ("Combining equal & if_goto");
	      pc += 1;
	      aentry->pc = pc;
	      arg = aexpr->bytes[pc++];
	      arg = (arg << 8) + aexpr->bytes[pc++];
	      aentry->goto_pc = arg;
	      emit_eq_goto (&(aentry->from_offset), &(aentry->from_size));
	    }
	  else if (next_op == gdb_agent_op_log_not
		   && (aexpr->bytes[pc + 1] == gdb_agent_op_if_goto)
		   && !is_goto_target (aexpr, pc + 1)
		   && target_emit_ops ()->emit_ne_goto)
	    {
	      ax_debug ("Combining equal & log_not & if_goto");
	      pc += 2;
	      aentry->pc = pc;
	      arg = aexpr->bytes[pc++];
	      arg = (arg << 8) + aexpr->bytes[pc++];
	      aentry->goto_pc = arg;
	      emit_ne_goto (&(aentry->from_offset), &(aentry->from_size));
	    }
	  else
	    emit_equal ();
	  break;

	case gdb_agent_op_less_signed:
	  next_op = aexpr->bytes[pc];
	  if (next_op == gdb_agent_op_if_goto
	      && !is_goto_target (aexpr, pc))
	    {
	      ax_debug ("Combining less_signed & if_goto");
	      pc += 1;
	      aentry->pc = pc;
	      arg = aexpr->bytes[pc++];
	      arg = (arg << 8) + aexpr->bytes[pc++];
	      aentry->goto_pc = arg;
	      emit_lt_goto (&(aentry->from_offset), &(aentry->from_size));
	    }
	  else if (next_op == gdb_agent_op_log_not
		   && !is_goto_target (aexpr, pc)
		   && (aexpr->bytes[pc + 1] == gdb_agent_op_if_goto)
		   && !is_goto_target (aexpr, pc + 1))
	    {
	      ax_debug ("Combining less_signed & log_not & if_goto");
	      pc += 2;
	      aentry->pc = pc;
	      arg = aexpr->bytes[pc++];
	      arg = (arg << 8) + aexpr->bytes[pc++];
	      aentry->goto_pc = arg;
	      emit_ge_goto (&(aentry->from_offset), &(aentry->from_size));
	    }
	  else
	    emit_less_signed ();
	  break;

	case gdb_agent_op_less_unsigned:
	  emit_less_unsigned ();
	  break;

	case gdb_agent_op_ext:
	  arg = aexpr->bytes[pc++];
	  if (arg < (sizeof (LONGEST) * 8))
	    emit_ext (arg);
	  break;

	case gdb_agent_op_ref8:
	  emit_ref (1);
	  break;

	case gdb_agent_op_ref16:
	  emit_ref (2);
	  break;

	case gdb_agent_op_ref32:
	  emit_ref (4);
	  break;

	case gdb_agent_op_ref64:
	  emit_ref (8);
	  break;

	case gdb_agent_op_if_goto:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  aentry->goto_pc = arg;
	  emit_if_goto (&(aentry->from_offset), &(aentry->from_size));
	  break;

	case gdb_agent_op_goto:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  aentry->goto_pc = arg;
	  emit_goto (&(aentry->from_offset), &(aentry->from_size));
	  break;

	case gdb_agent_op_const8:
	  emit_stack_flush ();
	  top = aexpr->bytes[pc++];
	  emit_const (top);
	  break;

	case gdb_agent_op_const16:
	  emit_stack_flush ();
	  top = aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  emit_const (top);
	  break;

	case gdb_agent_op_const32:
	  emit_stack_flush ();
	  top = aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  emit_const (top);
	  break;

	case gdb_agent_op_const64:
	  emit_stack_flush ();
	  top = aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  emit_const (top);
	  break;

	case gdb_agent_op_reg:
	  emit_stack_flush ();
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  emit_reg (arg);
	  break;

	case gdb_agent_op_end:
	  ax_debug ("At end of expression\n");

	  /* Assume there is one stack element left, and that it is
	     cached in "top" where emit_epilogue can get to it.  */
	  emit_stack_adjust (1);

	  done = 1;
	  break;

	case gdb_agent_op_dup:
	  /* In our design, dup is equivalent to stack flushing.  */
	  emit_stack_flush ();
	  break;

	case gdb_agent_op_pop:
	  emit_pop ();
	  break;

	case gdb_agent_op_zero_ext:
	  arg = aexpr->bytes[pc++];
	  if (arg < (sizeof (LONGEST) * 8))
	    emit_zero_ext (arg);
	  break;

	case gdb_agent_op_swap:
	  next_op = aexpr->bytes[pc];
	  /* Detect greater-than comparison sequences.  */
	  if (next_op == gdb_agent_op_less_signed
	      && !is_goto_target (aexpr, pc)
	      && (aexpr->bytes[pc + 1] == gdb_agent_op_if_goto)
	      && !is_goto_target (aexpr, pc + 1))
	    {
	      ax_debug ("Combining swap & less_signed & if_goto");
	      pc += 2;
	      aentry->pc = pc;
	      arg = aexpr->bytes[pc++];
	      arg = (arg << 8) + aexpr->bytes[pc++];
	      aentry->goto_pc = arg;
	      emit_gt_goto (&(aentry->from_offset), &(aentry->from_size));
	    }
	  else if (next_op == gdb_agent_op_less_signed
		   && !is_goto_target (aexpr, pc)
		   && (aexpr->bytes[pc + 1] == gdb_agent_op_log_not)
		   && !is_goto_target (aexpr, pc + 1)
		   && (aexpr->bytes[pc + 2] == gdb_agent_op_if_goto)
		   && !is_goto_target (aexpr, pc + 2))
	    {
	      ax_debug ("Combining swap & less_signed & log_not & if_goto");
	      pc += 3;
	      aentry->pc = pc;
	      arg = aexpr->bytes[pc++];
	      arg = (arg << 8) + aexpr->bytes[pc++];
	      aentry->goto_pc = arg;
	      emit_le_goto (&(aentry->from_offset), &(aentry->from_size));
	    }
	  else
	    emit_swap ();
	  break;

	case gdb_agent_op_getv:
	  emit_stack_flush ();
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  emit_int_call_1 (get_get_tsv_func_addr (),
			   arg);
	  break;

	case gdb_agent_op_setv:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  emit_void_call_2 (get_set_tsv_func_addr (),
			    arg);
	  break;

	case gdb_agent_op_tracev:
	  UNHANDLED;
	  break;

	  /* GDB never (currently) generates any of these ops.  */
	case gdb_agent_op_float:
	case gdb_agent_op_ref_float:
	case gdb_agent_op_ref_double:
	case gdb_agent_op_ref_long_double:
	case gdb_agent_op_l_to_d:
	case gdb_agent_op_d_to_l:
	case gdb_agent_op_trace16:
	  UNHANDLED;
	  break;

	default:
	  ax_debug ("Agent expression op 0x%x not recognized\n", op);
	  /* Don't struggle on, things will just get worse.  */
	  return expr_eval_unrecognized_opcode;
	}

      /* This catches errors that occur in target-specific code
	 emission.  */
      if (emit_error)
	{
	  ax_debug ("Error %d while emitting code for %s\n",
		    emit_error, gdb_agent_op_name (op));
	  return expr_eval_unhandled_opcode;
	}

      ax_debug ("Op %s compiled\n", gdb_agent_op_name (op));
    }

  /* Now fill in real addresses as goto destinations.  */
  for (aentry = bytecode_address_table; aentry; aentry = aentry->next)
    {
      int written = 0;

      if (aentry->goto_pc < 0)
	continue;

      /* Find the location that we are going to, and call back into
	 target-specific code to write the actual address or
	 displacement.  */
      for (aentry2 = bytecode_address_table; aentry2; aentry2 = aentry2->next)
	{
	  if (aentry2->pc == aentry->goto_pc)
	    {
	      ax_debug ("Want to jump from %s to %s\n",
			paddress (aentry->address),
			paddress (aentry2->address));
	      write_goto_address (aentry->address + aentry->from_offset,
				  aentry2->address, aentry->from_size);
	      written = 1;
	      break;
	    }
	}

      /* Error out if we didn't find a destination.  */
      if (!written)
	{
	  ax_debug ("Destination of goto %d not found\n",
		    aentry->goto_pc);
	  return expr_eval_invalid_goto;
	}
    }

  return expr_eval_no_error;
}

#endif

/* The agent expression evaluator, as specified by the GDB docs. It
   returns 0 if everything went OK, and a nonzero error code
   otherwise.  */

enum eval_result_type
gdb_eval_agent_expr (struct regcache *regcache,
		     struct traceframe *tframe,
		     struct agent_expr *aexpr,
		     ULONGEST *rslt)
{
  int pc = 0;
#define STACK_MAX 100
  ULONGEST stack[STACK_MAX], top;
  int sp = 0;
  unsigned char op;
  int arg;

  /* This union is a convenient way to convert representations.  For
     now, assume a standard architecture where the hardware integer
     types have 8, 16, 32, 64 bit types.  A more robust solution would
     be to import stdint.h from gnulib.  */
  union
  {
    union
    {
      unsigned char bytes[1];
      unsigned char val;
    } u8;
    union
    {
      unsigned char bytes[2];
      unsigned short val;
    } u16;
    union
    {
      unsigned char bytes[4];
      unsigned int val;
    } u32;
    union
    {
      unsigned char bytes[8];
      ULONGEST val;
    } u64;
  } cnv;

  if (aexpr->length == 0)
    {
      ax_debug ("empty agent expression");
      return expr_eval_empty_expression;
    }

  /* Cache the stack top in its own variable. Much of the time we can
     operate on this variable, rather than dinking with the stack. It
     needs to be copied to the stack when sp changes.  */
  top = 0;

  while (1)
    {
      op = aexpr->bytes[pc++];

      ax_debug ("About to interpret byte 0x%x", op);

      switch (op)
	{
	case gdb_agent_op_add:
	  top += stack[--sp];
	  break;

	case gdb_agent_op_sub:
	  top = stack[--sp] - top;
	  break;

	case gdb_agent_op_mul:
	  top *= stack[--sp];
	  break;

	case gdb_agent_op_div_signed:
	  if (top == 0)
	    {
	      ax_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = ((LONGEST) stack[--sp]) / ((LONGEST) top);
	  break;

	case gdb_agent_op_div_unsigned:
	  if (top == 0)
	    {
	      ax_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = stack[--sp] / top;
	  break;

	case gdb_agent_op_rem_signed:
	  if (top == 0)
	    {
	      ax_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = ((LONGEST) stack[--sp]) % ((LONGEST) top);
	  break;

	case gdb_agent_op_rem_unsigned:
	  if (top == 0)
	    {
	      ax_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = stack[--sp] % top;
	  break;

	case gdb_agent_op_lsh:
	  top = stack[--sp] << top;
	  break;

	case gdb_agent_op_rsh_signed:
	  top = ((LONGEST) stack[--sp]) >> top;
	  break;

	case gdb_agent_op_rsh_unsigned:
	  top = stack[--sp] >> top;
	  break;

	case gdb_agent_op_trace:
	  agent_mem_read (tframe,
			  NULL, (CORE_ADDR) stack[--sp], (ULONGEST) top);
	  if (--sp >= 0)
	    top = stack[sp];
	  break;

	case gdb_agent_op_trace_quick:
	  arg = aexpr->bytes[pc++];
	  agent_mem_read (tframe, NULL, (CORE_ADDR) top, (ULONGEST) arg);
	  break;

	case gdb_agent_op_log_not:
	  top = !top;
	  break;

	case gdb_agent_op_bit_and:
	  top &= stack[--sp];
	  break;

	case gdb_agent_op_bit_or:
	  top |= stack[--sp];
	  break;

	case gdb_agent_op_bit_xor:
	  top ^= stack[--sp];
	  break;

	case gdb_agent_op_bit_not:
	  top = ~top;
	  break;

	case gdb_agent_op_equal:
	  top = (stack[--sp] == top);
	  break;

	case gdb_agent_op_less_signed:
	  top = (((LONGEST) stack[--sp]) < ((LONGEST) top));
	  break;

	case gdb_agent_op_less_unsigned:
	  top = (stack[--sp] < top);
	  break;

	case gdb_agent_op_ext:
	  arg = aexpr->bytes[pc++];
	  if (arg < (sizeof (LONGEST) * 8))
	    {
	      LONGEST mask = 1 << (arg - 1);
	      top &= ((LONGEST) 1 << arg) - 1;
	      top = (top ^ mask) - mask;
	    }
	  break;

	case gdb_agent_op_ref8:
	  agent_mem_read (tframe, cnv.u8.bytes, (CORE_ADDR) top, 1);
	  top = cnv.u8.val;
	  break;

	case gdb_agent_op_ref16:
	  agent_mem_read (tframe, cnv.u16.bytes, (CORE_ADDR) top, 2);
	  top = cnv.u16.val;
	  break;

	case gdb_agent_op_ref32:
	  agent_mem_read (tframe, cnv.u32.bytes, (CORE_ADDR) top, 4);
	  top = cnv.u32.val;
	  break;

	case gdb_agent_op_ref64:
	  agent_mem_read (tframe, cnv.u64.bytes, (CORE_ADDR) top, 8);
	  top = cnv.u64.val;
	  break;

	case gdb_agent_op_if_goto:
	  if (top)
	    pc = (aexpr->bytes[pc] << 8) + (aexpr->bytes[pc + 1]);
	  else
	    pc += 2;
	  if (--sp >= 0)
	    top = stack[sp];
	  break;

	case gdb_agent_op_goto:
	  pc = (aexpr->bytes[pc] << 8) + (aexpr->bytes[pc + 1]);
	  break;

	case gdb_agent_op_const8:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  top = aexpr->bytes[pc++];
	  break;

	case gdb_agent_op_const16:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  top = aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  break;

	case gdb_agent_op_const32:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  top = aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  break;

	case gdb_agent_op_const64:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  top = aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  top = (top << 8) + aexpr->bytes[pc++];
	  break;

	case gdb_agent_op_reg:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  {
	    int regnum = arg;

	    switch (register_size (regnum))
	      {
	      case 8:
		collect_register (regcache, regnum, cnv.u64.bytes);
		top = cnv.u64.val;
		break;
	      case 4:
		collect_register (regcache, regnum, cnv.u32.bytes);
		top = cnv.u32.val;
		break;
	      case 2:
		collect_register (regcache, regnum, cnv.u16.bytes);
		top = cnv.u16.val;
		break;
	      case 1:
		collect_register (regcache, regnum, cnv.u8.bytes);
		top = cnv.u8.val;
		break;
	      default:
		internal_error (__FILE__, __LINE__,
				"unhandled register size");
	      }
	  }
	  break;

	case gdb_agent_op_end:
	  ax_debug ("At end of expression, sp=%d, stack top cache=0x%s",
		    sp, pulongest (top));
	  if (rslt)
	    {
	      if (sp <= 0)
		{
		  /* This should be an error */
		  ax_debug ("Stack is empty, nothing to return");
		  return expr_eval_empty_stack;
		}
	      *rslt = top;
	    }
	  return expr_eval_no_error;

	case gdb_agent_op_dup:
	  stack[sp++] = top;
	  break;

	case gdb_agent_op_pop:
	  if (--sp >= 0)
	    top = stack[sp];
	  break;

	case gdb_agent_op_pick:
	  arg = aexpr->bytes[pc++];
	  stack[sp] = top;
	  top = stack[sp - arg];
	  ++sp;
	  break;

	case gdb_agent_op_rot:
	  {
	    ULONGEST tem = stack[sp - 1];

	    stack[sp - 1] = stack[sp - 2];
	    stack[sp - 2] = top;
	    top = tem;
	  }
	  break;

	case gdb_agent_op_zero_ext:
	  arg = aexpr->bytes[pc++];
	  if (arg < (sizeof (LONGEST) * 8))
	    top &= ((LONGEST) 1 << arg) - 1;
	  break;

	case gdb_agent_op_swap:
	  /* Interchange top two stack elements, making sure top gets
	     copied back onto stack.  */
	  stack[sp] = top;
	  top = stack[sp - 1];
	  stack[sp - 1] = stack[sp];
	  break;

	case gdb_agent_op_getv:
	  /* Flush the cached stack top.  */
	  stack[sp++] = top;
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  top = agent_get_trace_state_variable_value (arg);
	  break;

	case gdb_agent_op_setv:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  agent_set_trace_state_variable_value (arg, top);
	  /* Note that we leave the value on the stack, for the
	     benefit of later/enclosing expressions.  */
	  break;

	case gdb_agent_op_tracev:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  agent_tsv_read (tframe, arg);
	  break;

	case gdb_agent_op_tracenz:
	  agent_mem_read_string (tframe, NULL, (CORE_ADDR) stack[--sp],
				 (ULONGEST) top);
	  if (--sp >= 0)
	    top = stack[sp];
	  break;

	  /* GDB never (currently) generates any of these ops.  */
	case gdb_agent_op_float:
	case gdb_agent_op_ref_float:
	case gdb_agent_op_ref_double:
	case gdb_agent_op_ref_long_double:
	case gdb_agent_op_l_to_d:
	case gdb_agent_op_d_to_l:
	case gdb_agent_op_trace16:
	  ax_debug ("Agent expression op 0x%x valid, but not handled",
		    op);
	  /* If ever GDB generates any of these, we don't have the
	     option of ignoring.  */
	  return 1;

	default:
	  ax_debug ("Agent expression op 0x%x not recognized", op);
	  /* Don't struggle on, things will just get worse.  */
	  return expr_eval_unrecognized_opcode;
	}

      /* Check for stack badness.  */
      if (sp >= (STACK_MAX - 1))
	{
	  ax_debug ("Expression stack overflow");
	  return expr_eval_stack_overflow;
	}

      if (sp < 0)
	{
	  ax_debug ("Expression stack underflow");
	  return expr_eval_stack_underflow;
	}

      ax_debug ("Op %s -> sp=%d, top=0x%s",
		gdb_agent_op_name (op), sp, phex_nz (top, 0));
    }
}
