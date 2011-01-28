/* Tracepoint code for remote server for GDB.
   Copyright (C) 2009, 2010, 2011 Free Software Foundation, Inc.

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
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <stddef.h>
#if HAVE_STDINT_H
#include <stdint.h>
#endif

/* This file is built for both both GDBserver, and the in-process
   agent (IPA), a shared library that includes a tracing agent that is
   loaded by the inferior to support fast tracepoints.  Fast
   tracepoints (or more accurately, jump based tracepoints) are
   implemented by patching the tracepoint location with a jump into a
   small trampoline function whose job is to save the register state,
   call the in-process tracing agent, and then execute the original
   instruction that was under the tracepoint jump (possibly adjusted,
   if PC-relative, or some such).

   The current synchronization design is pull based.  That means,
   GDBserver does most of the work, by peeking/poking at the inferior
   agent's memory directly for downloading tracepoint and associated
   objects, and for uploading trace frames.  Whenever the IPA needs
   something from GDBserver (trace buffer is full, tracing stopped for
   some reason, etc.) the IPA calls a corresponding hook function
   where GDBserver has placed a breakpoint.

   Each of the agents has its own trace buffer.  When browsing the
   trace frames built from slow and fast tracepoints from GDB (tfind
   mode), there's no guarantee the user is seeing the trace frames in
   strict chronological creation order, although, GDBserver tries to
   keep the order relatively reasonable, by syncing the trace buffers
   at appropriate times.

*/

static void trace_vdebug (const char *, ...) ATTR_FORMAT (printf, 1, 2);

static void
trace_vdebug (const char *fmt, ...)
{
  char buf[1024];
  va_list ap;

  va_start (ap, fmt);
  vsprintf (buf, fmt, ap);
  fprintf (stderr, "gdbserver/tracepoint: %s\n", buf);
  va_end (ap);
}

#define trace_debug_1(level, fmt, args...)	\
  do {						\
    if (level <= debug_threads)			\
      trace_vdebug ((fmt), ##args);		\
  } while (0)

#define trace_debug(FMT, args...)		\
  trace_debug_1 (1, FMT, ##args)

#if defined(__GNUC__)
#  define ATTR_USED __attribute__((used))
#  define ATTR_NOINLINE __attribute__((noinline))
#  define ATTR_CONSTRUCTOR __attribute__ ((constructor))
#else
#  define ATTR_USED
#  define ATTR_NOINLINE
#  define ATTR_CONSTRUCTOR
#endif

/* Make sure the functions the IPA needs to export (symbols GDBserver
   needs to query GDB about) are exported.  */

#ifdef IN_PROCESS_AGENT
# if defined _WIN32 || defined __CYGWIN__
#   define IP_AGENT_EXPORT __declspec(dllexport) ATTR_USED
# else
#   if __GNUC__ >= 4
#     define IP_AGENT_EXPORT \
  __attribute__ ((visibility("default"))) ATTR_USED
#   else
#     define IP_AGENT_EXPORT ATTR_USED
#   endif
# endif
#else
#  define IP_AGENT_EXPORT
#endif

/* Prefix exported symbols, for good citizenship.  All the symbols
   that need exporting are defined in this module.  */
#ifdef IN_PROCESS_AGENT
# define gdb_tp_heap_buffer gdb_agent_gdb_tp_heap_buffer
# define gdb_jump_pad_buffer gdb_agent_gdb_jump_pad_buffer
# define gdb_jump_pad_buffer_end gdb_agent_gdb_jump_pad_buffer_end
# define collecting gdb_agent_collecting
# define gdb_collect gdb_agent_gdb_collect
# define stop_tracing gdb_agent_stop_tracing
# define flush_trace_buffer gdb_agent_flush_trace_buffer
# define about_to_request_buffer_space gdb_agent_about_to_request_buffer_space
# define trace_buffer_is_full gdb_agent_trace_buffer_is_full
# define stopping_tracepoint gdb_agent_stopping_tracepoint
# define expr_eval_result gdb_agent_expr_eval_result
# define error_tracepoint gdb_agent_error_tracepoint
# define tracepoints gdb_agent_tracepoints
# define tracing gdb_agent_tracing
# define trace_buffer_ctrl gdb_agent_trace_buffer_ctrl
# define trace_buffer_ctrl_curr gdb_agent_trace_buffer_ctrl_curr
# define trace_buffer_lo gdb_agent_trace_buffer_lo
# define trace_buffer_hi gdb_agent_trace_buffer_hi
# define traceframe_read_count gdb_agent_traceframe_read_count
# define traceframe_write_count gdb_agent_traceframe_write_count
# define traceframes_created gdb_agent_traceframes_created
# define trace_state_variables gdb_agent_trace_state_variables
# define get_raw_reg gdb_agent_get_raw_reg
# define get_trace_state_variable_value \
  gdb_agent_get_trace_state_variable_value
# define set_trace_state_variable_value \
  gdb_agent_set_trace_state_variable_value
# define ust_loaded gdb_agent_ust_loaded
# define helper_thread_id gdb_agent_helper_thread_id
# define cmd_buf gdb_agent_cmd_buf
#endif

#ifndef IN_PROCESS_AGENT

/* Addresses of in-process agent's symbols GDBserver cares about.  */

struct ipa_sym_addresses
{
  CORE_ADDR addr_gdb_tp_heap_buffer;
  CORE_ADDR addr_gdb_jump_pad_buffer;
  CORE_ADDR addr_gdb_jump_pad_buffer_end;
  CORE_ADDR addr_collecting;
  CORE_ADDR addr_gdb_collect;
  CORE_ADDR addr_stop_tracing;
  CORE_ADDR addr_flush_trace_buffer;
  CORE_ADDR addr_about_to_request_buffer_space;
  CORE_ADDR addr_trace_buffer_is_full;
  CORE_ADDR addr_stopping_tracepoint;
  CORE_ADDR addr_expr_eval_result;
  CORE_ADDR addr_error_tracepoint;
  CORE_ADDR addr_tracepoints;
  CORE_ADDR addr_tracing;
  CORE_ADDR addr_trace_buffer_ctrl;
  CORE_ADDR addr_trace_buffer_ctrl_curr;
  CORE_ADDR addr_trace_buffer_lo;
  CORE_ADDR addr_trace_buffer_hi;
  CORE_ADDR addr_traceframe_read_count;
  CORE_ADDR addr_traceframe_write_count;
  CORE_ADDR addr_traceframes_created;
  CORE_ADDR addr_trace_state_variables;
  CORE_ADDR addr_get_raw_reg;
  CORE_ADDR addr_get_trace_state_variable_value;
  CORE_ADDR addr_set_trace_state_variable_value;
  CORE_ADDR addr_ust_loaded;
  CORE_ADDR addr_helper_thread_id;
  CORE_ADDR addr_cmd_buf;
};

#define STRINGIZE_1(STR) #STR
#define STRINGIZE(STR) STRINGIZE_1(STR)
#define IPA_SYM(SYM)					\
  {							\
    STRINGIZE (gdb_agent_ ## SYM),			\
    offsetof (struct ipa_sym_addresses, addr_ ## SYM)	\
  }

static struct
{
  const char *name;
  int offset;
  int required;
} symbol_list[] = {
  IPA_SYM(gdb_tp_heap_buffer),
  IPA_SYM(gdb_jump_pad_buffer),
  IPA_SYM(gdb_jump_pad_buffer_end),
  IPA_SYM(collecting),
  IPA_SYM(gdb_collect),
  IPA_SYM(stop_tracing),
  IPA_SYM(flush_trace_buffer),
  IPA_SYM(about_to_request_buffer_space),
  IPA_SYM(trace_buffer_is_full),
  IPA_SYM(stopping_tracepoint),
  IPA_SYM(expr_eval_result),
  IPA_SYM(error_tracepoint),
  IPA_SYM(tracepoints),
  IPA_SYM(tracing),
  IPA_SYM(trace_buffer_ctrl),
  IPA_SYM(trace_buffer_ctrl_curr),
  IPA_SYM(trace_buffer_lo),
  IPA_SYM(trace_buffer_hi),
  IPA_SYM(traceframe_read_count),
  IPA_SYM(traceframe_write_count),
  IPA_SYM(traceframes_created),
  IPA_SYM(trace_state_variables),
  IPA_SYM(get_raw_reg),
  IPA_SYM(get_trace_state_variable_value),
  IPA_SYM(set_trace_state_variable_value),
  IPA_SYM(ust_loaded),
  IPA_SYM(helper_thread_id),
  IPA_SYM(cmd_buf),
};

struct ipa_sym_addresses ipa_sym_addrs;

int all_tracepoint_symbols_looked_up;

int
in_process_agent_loaded (void)
{
  return all_tracepoint_symbols_looked_up;
}

static int read_inferior_integer (CORE_ADDR symaddr, int *val);

/* Returns true if both the in-process agent library and the static
   tracepoints libraries are loaded in the inferior.  */

static int
in_process_agent_loaded_ust (void)
{
  int loaded = 0;

  if (!in_process_agent_loaded ())
    {
      warning ("In-process agent not loaded");
      return 0;
    }

  if (read_inferior_integer (ipa_sym_addrs.addr_ust_loaded, &loaded))
    {
      warning ("Error reading ust_loaded in lib");
      return 0;
    }

  return loaded;
}

static void
write_e_ipa_not_loaded (char *buffer)
{
  sprintf (buffer,
	   "E.In-process agent library not loaded in process.  "
	   "Fast and static tracepoints unavailable.");
}

/* Write an error to BUFFER indicating that UST isn't loaded in the
   inferior.  */

static void
write_e_ust_not_loaded (char *buffer)
{
#ifdef HAVE_UST
  sprintf (buffer,
	   "E.UST library not loaded in process.  "
	   "Static tracepoints unavailable.");
#else
  sprintf (buffer, "E.GDBserver was built without static tracepoints support");
#endif
}

/* If the in-process agent library isn't loaded in the inferior, write
   an error to BUFFER, and return 1.  Otherwise, return 0.  */

static int
maybe_write_ipa_not_loaded (char *buffer)
{
  if (!in_process_agent_loaded ())
    {
      write_e_ipa_not_loaded (buffer);
      return 1;
    }
  return 0;
}

/* If the in-process agent library and the ust (static tracepoints)
   library aren't loaded in the inferior, write an error to BUFFER,
   and return 1.  Otherwise, return 0.  */

static int
maybe_write_ipa_ust_not_loaded (char *buffer)
{
  if (!in_process_agent_loaded ())
    {
      write_e_ipa_not_loaded (buffer);
      return 1;
    }
  else if (!in_process_agent_loaded_ust ())
    {
      write_e_ust_not_loaded (buffer);
      return 1;
    }
  return 0;
}

/* Cache all future symbols that the tracepoints module might request.
   We can not request symbols at arbitrary states in the remote
   protocol, only when the client tells us that new symbols are
   available.  So when we load the in-process library, make sure to
   check the entire list.  */

void
tracepoint_look_up_symbols (void)
{
  int all_ok;
  int i;

  if (all_tracepoint_symbols_looked_up)
    return;

  all_ok = 1;
  for (i = 0; i < sizeof (symbol_list) / sizeof (symbol_list[0]); i++)
    {
      CORE_ADDR *addrp =
	(CORE_ADDR *) ((char *) &ipa_sym_addrs + symbol_list[i].offset);

      if (look_up_one_symbol (symbol_list[i].name, addrp, 1) == 0)
	{
	  if (debug_threads)
	    fprintf (stderr, "symbol `%s' not found\n", symbol_list[i].name);
	  all_ok = 0;
	}
    }

  all_tracepoint_symbols_looked_up = all_ok;
}

#endif

/* GDBserver places a breakpoint on the IPA's version (which is a nop)
   of the "stop_tracing" function.  When this breakpoint is hit,
   tracing stopped in the IPA for some reason.  E.g., due to
   tracepoint reaching the pass count, hitting conditional expression
   evaluation error, etc.

   The IPA's trace buffer is never in circular tracing mode: instead,
   GDBserver's is, and whenever the in-process buffer fills, it calls
   "flush_trace_buffer", which triggers an internal breakpoint.
   GDBserver reacts to this breakpoint by pulling the meanwhile
   collected data.  Old frames discarding is always handled on the
   GDBserver side.  */

#ifdef IN_PROCESS_AGENT
int debug_threads = 0;

int
read_inferior_memory (CORE_ADDR memaddr, unsigned char *myaddr, int len)
{
  memcpy (myaddr, (void *) (uintptr_t) memaddr, len);
  return 0;
}

/* Call this in the functions where GDBserver places a breakpoint, so
   that the compiler doesn't try to be clever and skip calling the
   function at all.  This is necessary, even if we tell the compiler
   to not inline said functions.  */

#if defined(__GNUC__)
#  define UNKNOWN_SIDE_EFFECTS() asm ("")
#else
#  define UNKNOWN_SIDE_EFFECTS() do {} while (0)
#endif

IP_AGENT_EXPORT void ATTR_USED ATTR_NOINLINE
stop_tracing (void)
{
  /* GDBserver places breakpoint here.  */
  UNKNOWN_SIDE_EFFECTS();
}

IP_AGENT_EXPORT void ATTR_USED ATTR_NOINLINE
flush_trace_buffer (void)
{
  /* GDBserver places breakpoint here.  */
  UNKNOWN_SIDE_EFFECTS();
}

#endif

#ifndef IN_PROCESS_AGENT
static int
tracepoint_handler (CORE_ADDR address)
{
  trace_debug ("tracepoint_handler: tracepoint at 0x%s hit",
	       paddress (address));
  return 0;
}

/* Breakpoint at "stop_tracing" in the inferior lib.  */
struct breakpoint *stop_tracing_bkpt;
static int stop_tracing_handler (CORE_ADDR);

/* Breakpoint at "flush_trace_buffer" in the inferior lib.  */
struct breakpoint *flush_trace_buffer_bkpt;
static int flush_trace_buffer_handler (CORE_ADDR);

static void download_tracepoints (void);
static void download_trace_state_variables (void);
static void upload_fast_traceframes (void);

static int run_inferior_command (char *cmd);

static int
read_inferior_integer (CORE_ADDR symaddr, int *val)
{
  return read_inferior_memory (symaddr, (unsigned char *) val,
			       sizeof (*val));
}

static int
read_inferior_uinteger (CORE_ADDR symaddr, unsigned int *val)
{
  return read_inferior_memory (symaddr, (unsigned char *) val,
			       sizeof (*val));
}

static int
read_inferior_data_pointer (CORE_ADDR symaddr, CORE_ADDR *val)
{
  void *pval = (void *) (uintptr_t) val;
  int ret;

  ret = read_inferior_memory (symaddr, (unsigned char *) &pval, sizeof (pval));
  *val = (uintptr_t) pval;
  return ret;
}

static int
write_inferior_data_pointer (CORE_ADDR symaddr, CORE_ADDR val)
{
  void *pval = (void *) (uintptr_t) val;
  return write_inferior_memory (symaddr,
				(unsigned char *) &pval, sizeof (pval));
}

static int
write_inferior_integer (CORE_ADDR symaddr, int val)
{
  return write_inferior_memory (symaddr, (unsigned char *) &val, sizeof (val));
}

static int
write_inferior_uinteger (CORE_ADDR symaddr, unsigned int val)
{
  return write_inferior_memory (symaddr, (unsigned char *) &val, sizeof (val));
}

#endif

/* This enum must exactly match what is documented in
   gdb/doc/agentexpr.texi, including all the numerical values.  */

enum gdb_agent_op
  {
    gdb_agent_op_float = 0x01,
    gdb_agent_op_add = 0x02,
    gdb_agent_op_sub = 0x03,
    gdb_agent_op_mul = 0x04,
    gdb_agent_op_div_signed = 0x05,
    gdb_agent_op_div_unsigned = 0x06,
    gdb_agent_op_rem_signed = 0x07,
    gdb_agent_op_rem_unsigned = 0x08,
    gdb_agent_op_lsh = 0x09,
    gdb_agent_op_rsh_signed = 0x0a,
    gdb_agent_op_rsh_unsigned = 0x0b,
    gdb_agent_op_trace = 0x0c,
    gdb_agent_op_trace_quick = 0x0d,
    gdb_agent_op_log_not = 0x0e,
    gdb_agent_op_bit_and = 0x0f,
    gdb_agent_op_bit_or = 0x10,
    gdb_agent_op_bit_xor = 0x11,
    gdb_agent_op_bit_not = 0x12,
    gdb_agent_op_equal = 0x13,
    gdb_agent_op_less_signed = 0x14,
    gdb_agent_op_less_unsigned = 0x15,
    gdb_agent_op_ext = 0x16,
    gdb_agent_op_ref8 = 0x17,
    gdb_agent_op_ref16 = 0x18,
    gdb_agent_op_ref32 = 0x19,
    gdb_agent_op_ref64 = 0x1a,
    gdb_agent_op_ref_float = 0x1b,
    gdb_agent_op_ref_double = 0x1c,
    gdb_agent_op_ref_long_double = 0x1d,
    gdb_agent_op_l_to_d = 0x1e,
    gdb_agent_op_d_to_l = 0x1f,
    gdb_agent_op_if_goto = 0x20,
    gdb_agent_op_goto = 0x21,
    gdb_agent_op_const8 = 0x22,
    gdb_agent_op_const16 = 0x23,
    gdb_agent_op_const32 = 0x24,
    gdb_agent_op_const64 = 0x25,
    gdb_agent_op_reg = 0x26,
    gdb_agent_op_end = 0x27,
    gdb_agent_op_dup = 0x28,
    gdb_agent_op_pop = 0x29,
    gdb_agent_op_zero_ext = 0x2a,
    gdb_agent_op_swap = 0x2b,
    gdb_agent_op_getv = 0x2c,
    gdb_agent_op_setv = 0x2d,
    gdb_agent_op_tracev = 0x2e,
    gdb_agent_op_trace16 = 0x30,
    gdb_agent_op_last
  };

static const char *gdb_agent_op_names [gdb_agent_op_last] =
  {
    "?undef?",
    "float",
    "add",
    "sub",
    "mul",
    "div_signed",
    "div_unsigned",
    "rem_signed",
    "rem_unsigned",
    "lsh",
    "rsh_signed",
    "rsh_unsigned",
    "trace",
    "trace_quick",
    "log_not",
    "bit_and",
    "bit_or",
    "bit_xor",
    "bit_not",
    "equal",
    "less_signed",
    "less_unsigned",
    "ext",
    "ref8",
    "ref16",
    "ref32",
    "ref64",
    "ref_float",
    "ref_double",
    "ref_long_double",
    "l_to_d",
    "d_to_l",
    "if_goto",
    "goto",
    "const8",
    "const16",
    "const32",
    "const64",
    "reg",
    "end",
    "dup",
    "pop",
    "zero_ext",
    "swap",
    "getv",
    "setv",
    "tracev",
    "?undef?",
    "trace16",
  };

struct agent_expr
{
  int length;

  unsigned char *bytes;
};

/* Base action.  Concrete actions inherit this.  */

struct tracepoint_action
{
  char type;
};

/* An 'M' (collect memory) action.  */
struct collect_memory_action
{
  struct tracepoint_action base;

  ULONGEST addr;
  ULONGEST len;
  int basereg;
};

/* An 'R' (collect registers) action.  */

struct collect_registers_action
{
  struct tracepoint_action base;
};

/* An 'X' (evaluate expression) action.  */

struct eval_expr_action
{
  struct tracepoint_action base;

  struct agent_expr *expr;
};

/* An 'L' (collect static trace data) action.  */
struct collect_static_trace_data_action
{
  struct tracepoint_action base;
};

/* This structure describes a piece of the source-level definition of
   the tracepoint.  The contents are not interpreted by the target,
   but preserved verbatim for uploading upon reconnection.  */

struct source_string
{
  /* The type of string, such as "cond" for a conditional.  */
  char *type;

  /* The source-level string itself.  For the sake of target
     debugging, we store it in plaintext, even though it is always
     transmitted in hex.  */
  char *str;

  /* Link to the next one in the list.  We link them in the order
     received, in case some make up an ordered list of commands or
     some such.  */
  struct source_string *next;
};

enum tracepoint_type
{
  /* Trap based tracepoint.  */
  trap_tracepoint,

  /* A fast tracepoint implemented with a jump instead of a trap.  */
  fast_tracepoint,

  /* A static tracepoint, implemented by a program call into a tracing
     library.  */
  static_tracepoint
};

struct tracepoint_hit_ctx;

typedef enum eval_result_type (*condfn) (struct tracepoint_hit_ctx *,
					 ULONGEST *);

/* The definition of a tracepoint.  */

/* Tracepoints may have multiple locations, each at a different
   address.  This can occur with optimizations, template
   instantiation, etc.  Since the locations may be in different
   scopes, the conditions and actions may be different for each
   location.  Our target version of tracepoints is more like GDB's
   notion of "breakpoint locations", but we have almost nothing that
   is not per-location, so we bother having two kinds of objects.  The
   key consequence is that numbers are not unique, and that it takes
   both number and address to identify a tracepoint uniquely.  */

struct tracepoint
{
  /* The number of the tracepoint, as specified by GDB.  Several
     tracepoint objects here may share a number.  */
  int number;

  /* Address at which the tracepoint is supposed to trigger.  Several
     tracepoints may share an address.  */
  CORE_ADDR address;

  /* Tracepoint type.  */
  enum tracepoint_type type;

  /* True if the tracepoint is currently enabled.  */
  int enabled;

  /* The number of single steps that will be performed after each
     tracepoint hit.  */
  long step_count;

  /* The number of times the tracepoint may be hit before it will
     terminate the entire tracing run.  */
  long pass_count;

  /* Pointer to the agent expression that is the tracepoint's
     conditional, or NULL if the tracepoint is unconditional.  */
  struct agent_expr *cond;

  /* The list of actions to take when the tracepoint triggers.  */
  int numactions;
  struct tracepoint_action **actions;

  /* Count of the times we've hit this tracepoint during the run.
     Note that while-stepping steps are not counted as "hits".  */
  long hit_count;

  CORE_ADDR compiled_cond;

  /* Link to the next tracepoint in the list.  */
  struct tracepoint *next;

#ifndef IN_PROCESS_AGENT
  /* The list of actions to take when the tracepoint triggers, in
     string/packet form.  */
  char **actions_str;

  /* The collection of strings that describe the tracepoint as it was
     entered into GDB.  These are not used by the target, but are
     reported back to GDB upon reconnection.  */
  struct source_string *source_strings;

  /* The number of bytes displaced by fast tracepoints. It may subsume
     multiple instructions, for multi-byte fast tracepoints.  This
     field is only valid for fast tracepoints.  */
  int orig_size;

  /* Only for fast tracepoints.  */
  CORE_ADDR obj_addr_on_target;

  /* Address range where the original instruction under a fast
     tracepoint was relocated to.  (_end is actually one byte past
     the end).  */
  CORE_ADDR adjusted_insn_addr;
  CORE_ADDR adjusted_insn_addr_end;

  /* The address range of the piece of the jump pad buffer that was
     assigned to this fast tracepoint.  (_end is actually one byte
     past the end).*/
  CORE_ADDR jump_pad;
  CORE_ADDR jump_pad_end;

  /* The list of actions to take while in a stepping loop.  These
     fields are only valid for patch-based tracepoints.  */
  int num_step_actions;
  struct tracepoint_action **step_actions;
  /* Same, but in string/packet form.  */
  char **step_actions_str;

  /* Handle returned by the breakpoint or tracepoint module when we
     inserted the trap or jump, or hooked into a static tracepoint.
     NULL if we haven't inserted it yet.  */
  void *handle;
#endif

};

#ifndef IN_PROCESS_AGENT

/* Given `while-stepping', a thread may be collecting data for more
   than one tracepoint simultaneously.  On the other hand, the same
   tracepoint with a while-stepping action may be hit by more than one
   thread simultaneously (but not quite, each thread could be handling
   a different step).  Each thread holds a list of these objects,
   representing the current step of each while-stepping action being
   collected.  */

struct wstep_state
{
  struct wstep_state *next;

  /* The tracepoint number.  */
  int tp_number;
  /* The tracepoint's address.  */
  CORE_ADDR tp_address;

  /* The number of the current step in this 'while-stepping'
     action.  */
  long current_step;
};

#endif

/* The linked list of all tracepoints.  Marked explicitly as used as
   the in-process library doesn't use it for the fast tracepoints
   support.  */
IP_AGENT_EXPORT struct tracepoint *tracepoints ATTR_USED;

#ifndef IN_PROCESS_AGENT

/* Pointer to the last tracepoint in the list, new tracepoints are
   linked in at the end.  */

static struct tracepoint *last_tracepoint;
#endif

/* The first tracepoint to exceed its pass count.  */

IP_AGENT_EXPORT struct tracepoint *stopping_tracepoint;

/* True if the trace buffer is full or otherwise no longer usable.  */

IP_AGENT_EXPORT int trace_buffer_is_full;

/* Enumeration of the different kinds of things that can happen during
   agent expression evaluation.  */

enum eval_result_type
  {
    expr_eval_no_error,
    expr_eval_empty_expression,
    expr_eval_empty_stack,
    expr_eval_stack_overflow,
    expr_eval_stack_underflow,
    expr_eval_unhandled_opcode,
    expr_eval_unrecognized_opcode,
    expr_eval_divide_by_zero,
    expr_eval_invalid_goto
  };

static enum eval_result_type expr_eval_result = expr_eval_no_error;

#ifndef IN_PROCESS_AGENT

static const char *eval_result_names[] =
  {
    "terror:in the attic",  /* this should never be reported */
    "terror:empty expression",
    "terror:empty stack",
    "terror:stack overflow",
    "terror:stack underflow",
    "terror:unhandled opcode",
    "terror:unrecognized opcode",
    "terror:divide by zero"
  };

#endif

/* The tracepoint in which the error occurred.  */

static struct tracepoint *error_tracepoint;

struct trace_state_variable
{
  /* This is the name of the variable as used in GDB.  The target
     doesn't use the name, but needs to have it for saving and
     reconnection purposes.  */
  char *name;

  /* This number identifies the variable uniquely.  Numbers may be
     assigned either by the target (in the case of builtin variables),
     or by GDB, and are presumed unique during the course of a trace
     experiment.  */
  int number;

  /* The variable's initial value, a 64-bit signed integer always.  */
  LONGEST initial_value;

  /* The variable's value, a 64-bit signed integer always.  */
  LONGEST value;

  /* Pointer to a getter function, used to supply computed values.  */
  LONGEST (*getter) (void);

  /* Link to the next variable.  */
  struct trace_state_variable *next;
};

/* Linked list of all trace state variables.  */

#ifdef IN_PROCESS_AGENT
struct trace_state_variable *alloced_trace_state_variables;
#endif

IP_AGENT_EXPORT struct trace_state_variable *trace_state_variables;

/* The results of tracing go into a fixed-size space known as the
   "trace buffer".  Because usage follows a limited number of
   patterns, we manage it ourselves rather than with malloc.  Basic
   rules are that we create only one trace frame at a time, each is
   variable in size, they are never moved once created, and we only
   discard if we are doing a circular buffer, and then only the oldest
   ones.  Each trace frame includes its own size, so we don't need to
   link them together, and the trace frame number is relative to the
   first one, so we don't need to record numbers.  A trace frame also
   records the number of the tracepoint that created it.  The data
   itself is a series of blocks, each introduced by a single character
   and with a defined format.  Each type of block has enough
   type/length info to allow scanners to jump quickly from one block
   to the next without reading each byte in the block.  */

/* Trace buffer management would be simple - advance a free pointer
   from beginning to end, then stop - were it not for the circular
   buffer option, which is a useful way to prevent a trace run from
   stopping prematurely because the buffer filled up.  In the circular
   case, the location of the first trace frame (trace_buffer_start)
   moves as old trace frames are discarded.  Also, since we grow trace
   frames incrementally as actions are performed, we wrap around to
   the beginning of the trace buffer.  This is per-block, so each
   block within a trace frame remains contiguous.  Things get messy
   when the wrapped-around trace frame is the one being discarded; the
   free space ends up in two parts at opposite ends of the buffer.  */

#ifndef ATTR_PACKED
#  if defined(__GNUC__)
#    define ATTR_PACKED __attribute__ ((packed))
#  else
#    define ATTR_PACKED /* nothing */
#  endif
#endif

/* The data collected at a tracepoint hit.  This object should be as
   small as possible, since there may be a great many of them.  We do
   not need to keep a frame number, because they are all sequential
   and there are no deletions; so the Nth frame in the buffer is
   always frame number N.  */

struct traceframe
{
  /* Number of the tracepoint that collected this traceframe.  A value
     of 0 indicates the current end of the trace buffer.  We make this
     a 16-bit field because it's never going to happen that GDB's
     numbering of tracepoints reaches 32,000.  */
  int tpnum : 16;

  /* The size of the data in this trace frame.  We limit this to 32
     bits, even on a 64-bit target, because it's just implausible that
     one is validly going to collect 4 gigabytes of data at a single
     tracepoint hit.  */
  unsigned int data_size : 32;

  /* The base of the trace data, which is contiguous from this point.  */
  unsigned char data[0];

} ATTR_PACKED;

/* The traceframe to be used as the source of data to send back to
   GDB.  A value of -1 means to get data from the live program.  */

int current_traceframe = -1;

/* This flag is true if the trace buffer is circular, meaning that
   when it fills, the oldest trace frames are discarded in order to
   make room.  */

#ifndef IN_PROCESS_AGENT
static int circular_trace_buffer;
#endif

/* Pointer to the block of memory that traceframes all go into.  */

static unsigned char *trace_buffer_lo;

/* Pointer to the end of the trace buffer, more precisely to the byte
   after the end of the buffer.  */

static unsigned char *trace_buffer_hi;

/* Control structure holding the read/write/etc. pointers into the
   trace buffer.  We need more than one of these to implement a
   transaction-like mechanism to garantees that both GDBserver and the
   in-process agent can try to change the trace buffer
   simultaneously.  */

struct trace_buffer_control
{
  /* Pointer to the first trace frame in the buffer.  In the
     non-circular case, this is equal to trace_buffer_lo, otherwise it
     moves around in the buffer.  */
  unsigned char *start;

  /* Pointer to the free part of the trace buffer.  Note that we clear
     several bytes at and after this pointer, so that traceframe
     scans/searches terminate properly.  */
  unsigned char *free;

  /* Pointer to the byte after the end of the free part.  Note that
     this may be smaller than trace_buffer_free in the circular case,
     and means that the free part is in two pieces.  Initially it is
     equal to trace_buffer_hi, then is generally equivalent to
     trace_buffer_start.  */
  unsigned char *end_free;

  /* Pointer to the wraparound.  If not equal to trace_buffer_hi, then
     this is the point at which the trace data breaks, and resumes at
     trace_buffer_lo.  */
  unsigned char *wrap;
};

/* Same as above, to be used by GDBserver when updating the in-process
   agent.  */
struct ipa_trace_buffer_control
{
  uintptr_t start;
  uintptr_t free;
  uintptr_t end_free;
  uintptr_t wrap;
};


/* We have possibly both GDBserver and an inferior thread accessing
   the same IPA trace buffer memory.  The IPA is the producer (tries
   to put new frames in the buffer), while GDBserver occasionally
   consumes them, that is, flushes the IPA's buffer into its own
   buffer.  Both sides need to update the trace buffer control
   pointers (current head, tail, etc.).  We can't use a global lock to
   synchronize the accesses, as otherwise we could deadlock GDBserver
   (if the thread holding the lock stops for a signal, say).  So
   instead of that, we use a transaction scheme where GDBserver writes
   always prevail over the IPAs writes, and, we have the IPA detect
   the commit failure/overwrite, and retry the whole attempt.  This is
   mainly implemented by having a global token object that represents
   who wrote last to the buffer control structure.  We need to freeze
   any inferior writing to the buffer while GDBserver touches memory,
   so that the inferior can correctly detect that GDBserver had been
   there, otherwise, it could mistakingly think its commit was
   successful; that's implemented by simply having GDBserver set a
   breakpoint the inferior hits if it is the critical region.

   There are three cycling trace buffer control structure copies
   (buffer head, tail, etc.), with the token object including an index
   indicating which is current live copy.  The IPA tentatively builds
   an updated copy in a non-current control structure, while GDBserver
   always clobbers the current version directly.  The IPA then tries
   to atomically "commit" its version; if GDBserver clobbered the
   structure meanwhile, that will fail, and the IPA restarts the
   allocation process.

   Listing the step in further detail, we have:

  In-process agent (producer):

  - passes by `about_to_request_buffer_space' breakpoint/lock

  - reads current token, extracts current trace buffer control index,
    and starts tentatively updating the rightmost one (0->1, 1->2,
    2->0).  Note that only one inferior thread is executing this code
    at any given time, due to an outer lock in the jump pads.

  - updates counters, and tries to commit the token.

  - passes by second `about_to_request_buffer_space' breakpoint/lock,
    leaving the sync region.

  - checks if the update was effective.

  - if trace buffer was found full, hits flush_trace_buffer
    breakpoint, and restarts later afterwards.

  GDBserver (consumer):

  - sets `about_to_request_buffer_space' breakpoint/lock.

  - updates the token unconditionally, using the current buffer
    control index, since it knows that the IP agent always writes to
    the rightmost, and due to the breakpoint, at most one IP thread
    can try to update the trace buffer concurrently to GDBserver, so
    there will be no danger of trace buffer control index wrap making
    the IPA write to the same index as GDBserver.

  - flushes the IP agent's trace buffer completely, and updates the
    current trace buffer control structure.  GDBserver *always* wins.

  - removes the `about_to_request_buffer_space' breakpoint.

The token is stored in the `trace_buffer_ctrl_curr' variable.
Internally, it's bits are defined as:

 |-------------+-----+-------------+--------+-------------+--------------|
 | Bit offsets |  31 |   30 - 20   |   19   |    18-8     |     7-0      |
 |-------------+-----+-------------+--------+-------------+--------------|
 | What        | GSB | PC (11-bit) | unused | CC (11-bit) | TBCI (8-bit) |
 |-------------+-----+-------------+--------+-------------+--------------|

 GSB  - GDBserver Stamp Bit
 PC   - Previous Counter
 CC   - Current Counter
 TBCI - Trace Buffer Control Index


An IPA update of `trace_buffer_ctrl_curr' does:

    - read CC from the current token, save as PC.
    - updates pointers
    - atomically tries to write PC+1,CC

A GDBserver update of `trace_buffer_ctrl_curr' does:

    - reads PC and CC from the current token.
    - updates pointers
    - writes GSB,PC,CC
*/

/* These are the bits of `trace_buffer_ctrl_curr' that are reserved
   for the counters described below.  The cleared bits are used to
   hold the index of the items of the `trace_buffer_ctrl' array that
   is "current".  */
#define GDBSERVER_FLUSH_COUNT_MASK        0xfffffff0

/* `trace_buffer_ctrl_curr' contains two counters.  The `previous'
   counter, and the `current' counter.  */

#define GDBSERVER_FLUSH_COUNT_MASK_PREV   0x7ff00000
#define GDBSERVER_FLUSH_COUNT_MASK_CURR   0x0007ff00

/* When GDBserver update the IP agent's `trace_buffer_ctrl_curr', it
   always stamps this bit as set.  */
#define GDBSERVER_UPDATED_FLUSH_COUNT_BIT 0x80000000

#ifdef IN_PROCESS_AGENT
IP_AGENT_EXPORT struct trace_buffer_control trace_buffer_ctrl[3];
IP_AGENT_EXPORT unsigned int trace_buffer_ctrl_curr;

# define TRACE_BUFFER_CTRL_CURR \
  (trace_buffer_ctrl_curr & ~GDBSERVER_FLUSH_COUNT_MASK)

#else

/* The GDBserver side agent only needs one instance of this object, as
   it doesn't need to sync with itself.  Define it as array anyway so
   that the rest of the code base doesn't need to care for the
   difference.  */
struct trace_buffer_control trace_buffer_ctrl[1];
# define TRACE_BUFFER_CTRL_CURR 0
#endif

/* These are convenience macros used to access the current trace
   buffer control in effect.  */
#define trace_buffer_start (trace_buffer_ctrl[TRACE_BUFFER_CTRL_CURR].start)
#define trace_buffer_free (trace_buffer_ctrl[TRACE_BUFFER_CTRL_CURR].free)
#define trace_buffer_end_free \
  (trace_buffer_ctrl[TRACE_BUFFER_CTRL_CURR].end_free)
#define trace_buffer_wrap (trace_buffer_ctrl[TRACE_BUFFER_CTRL_CURR].wrap)


/* Macro that returns a pointer to the first traceframe in the buffer.  */

#define FIRST_TRACEFRAME() ((struct traceframe *) trace_buffer_start)

/* Macro that returns a pointer to the next traceframe in the buffer.
   If the computed location is beyond the wraparound point, subtract
   the offset of the wraparound.  */

#define NEXT_TRACEFRAME_1(TF) \
  (((unsigned char *) (TF)) + sizeof (struct traceframe) + (TF)->data_size)

#define NEXT_TRACEFRAME(TF) \
  ((struct traceframe *) (NEXT_TRACEFRAME_1 (TF)  \
			  - ((NEXT_TRACEFRAME_1 (TF) >= trace_buffer_wrap) \
			     ? (trace_buffer_wrap - trace_buffer_lo)	\
			     : 0)))

/* The difference between these counters represents the total number
   of complete traceframes present in the trace buffer.  The IP agent
   writes to the write count, GDBserver writes to read count.  */

IP_AGENT_EXPORT unsigned int traceframe_write_count;
IP_AGENT_EXPORT unsigned int traceframe_read_count;

/* Convenience macro.  */

#define traceframe_count \
  ((unsigned int) (traceframe_write_count - traceframe_read_count))

/* The count of all traceframes created in the current run, including
   ones that were discarded to make room.  */

IP_AGENT_EXPORT int traceframes_created;

#ifndef IN_PROCESS_AGENT

/* Read-only regions are address ranges whose contents don't change,
   and so can be read from target memory even while looking at a trace
   frame.  Without these, disassembly for instance will likely fail,
   because the program code is not usually collected into a trace
   frame.  This data structure does not need to be very complicated or
   particularly efficient, it's only going to be used occasionally,
   and only by some commands.  */

struct readonly_region
{
  /* The bounds of the region.  */
  CORE_ADDR start, end;

  /* Link to the next one.  */
  struct readonly_region *next;
};

/* Linked list of readonly regions.  This list stays in effect from
   one tstart to the next.  */

static struct readonly_region *readonly_regions;

#endif

/* The global that controls tracing overall.  */

IP_AGENT_EXPORT int tracing;

#ifndef IN_PROCESS_AGENT

/* Controls whether tracing should continue after GDB disconnects.  */

int disconnected_tracing;

/* The reason for the last tracing run to have stopped.  We initialize
   to a distinct string so that GDB can distinguish between "stopped
   after running" and "stopped because never run" cases.  */

static const char *tracing_stop_reason = "tnotrun";

static int tracing_stop_tpnum;

#endif

/* Functions local to this file.  */

/* Base "class" for tracepoint type specific data to be passed down to
   collect_data_at_tracepoint.  */
struct tracepoint_hit_ctx
{
  enum tracepoint_type type;
};

#ifdef IN_PROCESS_AGENT

/* Fast/jump tracepoint specific data to be passed down to
   collect_data_at_tracepoint.  */
struct fast_tracepoint_ctx
{
  struct tracepoint_hit_ctx base;

  struct regcache regcache;
  int regcache_initted;
  unsigned char *regspace;

  unsigned char *regs;
  struct tracepoint *tpoint;
};

/* Static tracepoint specific data to be passed down to
   collect_data_at_tracepoint.  */
struct static_tracepoint_ctx
{
  struct tracepoint_hit_ctx base;

  /* The regcache corresponding to the registers state at the time of
     the tracepoint hit.  Initialized lazily, from REGS.  */
  struct regcache regcache;
  int regcache_initted;

  /* The buffer space REGCACHE above uses.  We use a separate buffer
     instead of letting the regcache malloc for both signal safety and
     performance reasons; this is allocated on the stack instead.  */
  unsigned char *regspace;

  /* The register buffer as passed on by lttng/ust.  */
  struct registers *regs;

  /* The "printf" formatter and the args the user passed to the marker
     call.  We use this to be able to collect "static trace data"
     ($_sdata).  */
  const char *fmt;
  va_list *args;

  /* The GDB tracepoint matching the probed marker that was "hit".  */
  struct tracepoint *tpoint;
};

#else

/* Static tracepoint specific data to be passed down to
   collect_data_at_tracepoint.  */
struct trap_tracepoint_ctx
{
  struct tracepoint_hit_ctx base;

  struct regcache *regcache;
};

#endif

#ifndef IN_PROCESS_AGENT
static struct agent_expr *parse_agent_expr (char **actparm);
static char *unparse_agent_expr (struct agent_expr *aexpr);
#endif
static enum eval_result_type eval_agent_expr (struct tracepoint_hit_ctx *ctx,
					      struct traceframe *tframe,
					      struct agent_expr *aexpr,
					      ULONGEST *rslt);

static int agent_mem_read (struct traceframe *tframe,
			   unsigned char *to, CORE_ADDR from, ULONGEST len);
static int agent_tsv_read (struct traceframe *tframe, int n);

#ifndef IN_PROCESS_AGENT
static CORE_ADDR traceframe_get_pc (struct traceframe *tframe);
static int traceframe_read_tsv (int num, LONGEST *val);
#endif

static int condition_true_at_tracepoint (struct tracepoint_hit_ctx *ctx,
					 struct tracepoint *tpoint);

#ifndef IN_PROCESS_AGENT
static void clear_readonly_regions (void);
static void clear_installed_tracepoints (void);
#endif

static void collect_data_at_tracepoint (struct tracepoint_hit_ctx *ctx,
					CORE_ADDR stop_pc,
					struct tracepoint *tpoint);
#ifndef IN_PROCESS_AGENT
static void collect_data_at_step (struct tracepoint_hit_ctx *ctx,
				  CORE_ADDR stop_pc,
				  struct tracepoint *tpoint, int current_step);
static void compile_tracepoint_condition (struct tracepoint *tpoint,
					  CORE_ADDR *jump_entry);
#endif
static void do_action_at_tracepoint (struct tracepoint_hit_ctx *ctx,
				     CORE_ADDR stop_pc,
				     struct tracepoint *tpoint,
				     struct traceframe *tframe,
				     struct tracepoint_action *taction);

#ifndef IN_PROCESS_AGENT
static struct tracepoint *fast_tracepoint_from_ipa_tpoint_address (CORE_ADDR);
#endif

#if defined(__GNUC__)
#  define memory_barrier() asm volatile ("" : : : "memory")
#else
#  define memory_barrier() do {} while (0)
#endif

/* We only build the IPA if this builtin is supported, and there are
   no uses of this in GDBserver itself, so we're safe in defining this
   unconditionally.  */
#define cmpxchg(mem, oldval, newval) \
  __sync_val_compare_and_swap (mem, oldval, newval)

/* The size in bytes of the buffer used to talk to the IPA helper
   thread.  */
#define CMD_BUF_SIZE 1024

/* Record that an error occurred during expression evaluation.  */

static void
record_tracepoint_error (struct tracepoint *tpoint, const char *which,
			 enum eval_result_type rtype)
{
  trace_debug ("Tracepoint %d at %s %s eval reports error %d",
	       tpoint->number, paddress (tpoint->address), which, rtype);

#ifdef IN_PROCESS_AGENT
  /* Only record the first error we get.  */
  if (cmpxchg (&expr_eval_result,
	       expr_eval_no_error,
	       rtype) != expr_eval_no_error)
    return;
#else
  if (expr_eval_result != expr_eval_no_error)
    return;
#endif

  error_tracepoint = tpoint;
}

/* Trace buffer management.  */

static void
clear_trace_buffer (void)
{
  trace_buffer_start = trace_buffer_lo;
  trace_buffer_free = trace_buffer_lo;
  trace_buffer_end_free = trace_buffer_hi;
  trace_buffer_wrap = trace_buffer_hi;
  /* A traceframe with zeroed fields marks the end of trace data.  */
  ((struct traceframe *) trace_buffer_free)->tpnum = 0;
  ((struct traceframe *) trace_buffer_free)->data_size = 0;
  traceframe_read_count = traceframe_write_count = 0;
  traceframes_created = 0;
}

#ifndef IN_PROCESS_AGENT

static void
clear_inferior_trace_buffer (void)
{
  CORE_ADDR ipa_trace_buffer_lo;
  CORE_ADDR ipa_trace_buffer_hi;
  struct traceframe ipa_traceframe = { 0 };
  struct ipa_trace_buffer_control ipa_trace_buffer_ctrl;

  read_inferior_data_pointer (ipa_sym_addrs.addr_trace_buffer_lo,
			      &ipa_trace_buffer_lo);
  read_inferior_data_pointer (ipa_sym_addrs.addr_trace_buffer_hi,
			      &ipa_trace_buffer_hi);

  ipa_trace_buffer_ctrl.start = ipa_trace_buffer_lo;
  ipa_trace_buffer_ctrl.free = ipa_trace_buffer_lo;
  ipa_trace_buffer_ctrl.end_free = ipa_trace_buffer_hi;
  ipa_trace_buffer_ctrl.wrap = ipa_trace_buffer_hi;

  /* A traceframe with zeroed fields marks the end of trace data.  */
  write_inferior_memory (ipa_sym_addrs.addr_trace_buffer_ctrl,
			 (unsigned char *) &ipa_trace_buffer_ctrl,
			 sizeof (ipa_trace_buffer_ctrl));

  write_inferior_uinteger (ipa_sym_addrs.addr_trace_buffer_ctrl_curr, 0);

  /* A traceframe with zeroed fields marks the end of trace data.  */
  write_inferior_memory (ipa_trace_buffer_lo,
			 (unsigned char *) &ipa_traceframe,
			 sizeof (ipa_traceframe));

  write_inferior_uinteger (ipa_sym_addrs.addr_traceframe_write_count, 0);
  write_inferior_uinteger (ipa_sym_addrs.addr_traceframe_read_count, 0);
  write_inferior_integer (ipa_sym_addrs.addr_traceframes_created, 0);
}

#endif

static void
init_trace_buffer (unsigned char *buf, int bufsize)
{
  trace_buffer_lo = buf;
  trace_buffer_hi = trace_buffer_lo + bufsize;

  clear_trace_buffer ();
}

#ifdef IN_PROCESS_AGENT

IP_AGENT_EXPORT void ATTR_USED ATTR_NOINLINE
about_to_request_buffer_space (void)
{
  /* GDBserver places breakpoint here while it goes about to flush
     data at random times.  */
  UNKNOWN_SIDE_EFFECTS();
}

#endif

/* Carve out a piece of the trace buffer, returning NULL in case of
   failure.  */

static void *
trace_buffer_alloc (size_t amt)
{
  unsigned char *rslt;
  struct trace_buffer_control *tbctrl;
  unsigned int curr;
#ifdef IN_PROCESS_AGENT
  unsigned int prev, prev_filtered;
  unsigned int commit_count;
  unsigned int commit;
  unsigned int readout;
#else
  struct traceframe *oldest;
  unsigned char *new_start;
#endif

  trace_debug ("Want to allocate %ld+%ld bytes in trace buffer",
	       (long) amt, (long) sizeof (struct traceframe));

  /* Account for the EOB marker.  */
  amt += sizeof (struct traceframe);

#ifdef IN_PROCESS_AGENT
 again:
  memory_barrier ();

  /* Read the current token and extract the index to try to write to,
     storing it in CURR.  */
  prev = trace_buffer_ctrl_curr;
  prev_filtered = prev & ~GDBSERVER_FLUSH_COUNT_MASK;
  curr = prev_filtered + 1;
  if (curr > 2)
    curr = 0;

  about_to_request_buffer_space ();

  /* Start out with a copy of the current state.  GDBserver may be
     midway writing to the PREV_FILTERED TBC, but, that's OK, we won't
     be able to commit anyway if that happens.  */
  trace_buffer_ctrl[curr]
    = trace_buffer_ctrl[prev_filtered];
  trace_debug ("trying curr=%u", curr);
#else
  /* The GDBserver's agent doesn't need all that syncing, and always
     updates TCB 0 (there's only one, mind you).  */
  curr = 0;
#endif
  tbctrl = &trace_buffer_ctrl[curr];

  /* Offsets are easier to grok for debugging than raw addresses,
     especially for the small trace buffer sizes that are useful for
     testing.  */
  trace_debug ("Trace buffer [%d] start=%d free=%d endfree=%d wrap=%d hi=%d",
	       curr,
	       (int) (tbctrl->start - trace_buffer_lo),
	       (int) (tbctrl->free - trace_buffer_lo),
	       (int) (tbctrl->end_free - trace_buffer_lo),
	       (int) (tbctrl->wrap - trace_buffer_lo),
	       (int) (trace_buffer_hi - trace_buffer_lo));

  /* The algorithm here is to keep trying to get a contiguous block of
     the requested size, possibly discarding older traceframes to free
     up space.  Since free space might come in one or two pieces,
     depending on whether discarded traceframes wrapped around at the
     high end of the buffer, we test both pieces after each
     discard.  */
  while (1)
    {
      /* First, if we have two free parts, try the upper one first.  */
      if (tbctrl->end_free < tbctrl->free)
	{
	  if (tbctrl->free + amt <= trace_buffer_hi)
	    /* We have enough in the upper part.  */
	    break;
	  else
	    {
	      /* Our high part of free space wasn't enough.  Give up
		 on it for now, set wraparound.  We will recover the
		 space later, if/when the wrapped-around traceframe is
		 discarded.  */
	      trace_debug ("Upper part too small, setting wraparound");
	      tbctrl->wrap = tbctrl->free;
	      tbctrl->free = trace_buffer_lo;
	    }
	}

      /* The normal case.  */
      if (tbctrl->free + amt <= tbctrl->end_free)
	break;

#ifdef IN_PROCESS_AGENT
      /* The IP Agent's buffer is always circular.  It isn't used
	 currently, but `circular_trace_buffer' could represent
	 GDBserver's mode.  If we didn't find space, ask GDBserver to
	 flush.  */

      flush_trace_buffer ();
      memory_barrier ();
      if (tracing)
	{
	  trace_debug ("gdbserver flushed buffer, retrying");
	  goto again;
	}

      /* GDBserver cancelled the tracing.  Bail out as well.  */
      return NULL;
#else
      /* If we're here, then neither part is big enough, and
	 non-circular trace buffers are now full.  */
      if (!circular_trace_buffer)
	{
	  trace_debug ("Not enough space in the trace buffer");
	  return NULL;
	}

      trace_debug ("Need more space in the trace buffer");

      /* If we have a circular buffer, we can try discarding the
	 oldest traceframe and see if that helps.  */
      oldest = FIRST_TRACEFRAME ();
      if (oldest->tpnum == 0)
	{
	  /* Not good; we have no traceframes to free.  Perhaps we're
	     asking for a block that is larger than the buffer?  In
	     any case, give up.  */
	  trace_debug ("No traceframes to discard");
	  return NULL;
	}

      /* We don't run this code in the in-process agent currently.
	 E.g., we could leave the in-process agent in autonomous
	 circular mode if we only have fast tracepoints.  If we do
	 that, then this bit becomes racy with GDBserver, which also
	 writes to this counter.  */
      --traceframe_write_count;

      new_start = (unsigned char *) NEXT_TRACEFRAME (oldest);
      /* If we freed the traceframe that wrapped around, go back
	 to the non-wrap case.  */
      if (new_start < tbctrl->start)
	{
	  trace_debug ("Discarding past the wraparound");
	  tbctrl->wrap = trace_buffer_hi;
	}
      tbctrl->start = new_start;
      tbctrl->end_free = tbctrl->start;

      trace_debug ("Discarded a traceframe\n"
		   "Trace buffer [%d], start=%d free=%d "
		   "endfree=%d wrap=%d hi=%d",
		   curr,
		   (int) (tbctrl->start - trace_buffer_lo),
		   (int) (tbctrl->free - trace_buffer_lo),
		   (int) (tbctrl->end_free - trace_buffer_lo),
		   (int) (tbctrl->wrap - trace_buffer_lo),
		   (int) (trace_buffer_hi - trace_buffer_lo));

      /* Now go back around the loop.  The discard might have resulted
	 in either one or two pieces of free space, so we want to try
	 both before freeing any more traceframes.  */
#endif
    }

  /* If we get here, we know we can provide the asked-for space.  */

  rslt = tbctrl->free;

  /* Adjust the request back down, now that we know we have space for
     the marker, but don't commit to AMT yet, we may still need to
     restart the operation if GDBserver touches the trace buffer
     (obviously only important in the in-process agent's version).  */
  tbctrl->free += (amt - sizeof (struct traceframe));

  /* Or not.  If GDBserver changed the trace buffer behind our back,
     we get to restart a new allocation attempt.  */

#ifdef IN_PROCESS_AGENT
  /* Build the tentative token.  */
  commit_count = (((prev & 0x0007ff00) + 0x100) & 0x0007ff00);
  commit = (((prev & 0x0007ff00) << 12)
	    | commit_count
	    | curr);

  /* Try to commit it.  */
  readout = cmpxchg (&trace_buffer_ctrl_curr, prev, commit);
  if (readout != prev)
    {
      trace_debug ("GDBserver has touched the trace buffer, restarting."
		   " (prev=%08x, commit=%08x, readout=%08x)",
		   prev, commit, readout);
      goto again;
    }

  /* Hold your horses here.  Even if that change was committed,
     GDBserver could come in, and clobber it.  We need to hold to be
     able to tell if GDBserver clobbers before or after we committed
     the change.  Whenever GDBserver goes about touching the IPA
     buffer, it sets a breakpoint in this routine, so we have a sync
     point here.  */
  about_to_request_buffer_space ();

  /* Check if the change has been effective, even if GDBserver stopped
     us at the breakpoint.  */

  {
    unsigned int refetch;

    memory_barrier ();

    refetch = trace_buffer_ctrl_curr;

    if ((refetch == commit
	 || ((refetch & 0x7ff00000) >> 12) == commit_count))
      {
	/* effective */
	trace_debug ("change is effective: (prev=%08x, commit=%08x, "
		     "readout=%08x, refetch=%08x)",
		     prev, commit, readout, refetch);
      }
    else
      {
	trace_debug ("GDBserver has touched the trace buffer, not effective."
		     " (prev=%08x, commit=%08x, readout=%08x, refetch=%08x)",
		     prev, commit, readout, refetch);
	goto again;
      }
  }
#endif

  /* We have a new piece of the trace buffer.  Hurray!  */

  /* Add an EOB marker just past this allocation.  */
  ((struct traceframe *) tbctrl->free)->tpnum = 0;
  ((struct traceframe *) tbctrl->free)->data_size = 0;

  /* Adjust the request back down, now that we know we have space for
     the marker.  */
  amt -= sizeof (struct traceframe);

  if (debug_threads)
    {
      trace_debug ("Allocated %d bytes", (int) amt);
      trace_debug ("Trace buffer [%d] start=%d free=%d "
		   "endfree=%d wrap=%d hi=%d",
		   curr,
		   (int) (tbctrl->start - trace_buffer_lo),
		   (int) (tbctrl->free - trace_buffer_lo),
		   (int) (tbctrl->end_free - trace_buffer_lo),
		   (int) (tbctrl->wrap - trace_buffer_lo),
		   (int) (trace_buffer_hi - trace_buffer_lo));
    }

  return rslt;
}

#ifndef IN_PROCESS_AGENT

/* Return the total free space.  This is not necessarily the largest
   block we can allocate, because of the two-part case.  */

static int
free_space (void)
{
  if (trace_buffer_free <= trace_buffer_end_free)
    return trace_buffer_end_free - trace_buffer_free;
  else
    return ((trace_buffer_end_free - trace_buffer_lo)
	    + (trace_buffer_hi - trace_buffer_free));
}

/* An 'S' in continuation packets indicates remainder are for
   while-stepping.  */

static int seen_step_action_flag;

/* Create a tracepoint (location) with given number and address.  */

static struct tracepoint *
add_tracepoint (int num, CORE_ADDR addr)
{
  struct tracepoint *tpoint;

  tpoint = xmalloc (sizeof (struct tracepoint));
  tpoint->number = num;
  tpoint->address = addr;
  tpoint->numactions = 0;
  tpoint->actions = NULL;
  tpoint->actions_str = NULL;
  tpoint->cond = NULL;
  tpoint->num_step_actions = 0;
  tpoint->step_actions = NULL;
  tpoint->step_actions_str = NULL;
  /* Start all off as regular (slow) tracepoints.  */
  tpoint->type = trap_tracepoint;
  tpoint->orig_size = -1;
  tpoint->source_strings = NULL;
  tpoint->compiled_cond = 0;
  tpoint->handle = NULL;
  tpoint->next = NULL;

  if (!last_tracepoint)
    tracepoints = tpoint;
  else
    last_tracepoint->next = tpoint;
  last_tracepoint = tpoint;

  seen_step_action_flag = 0;

  return tpoint;
}

#ifndef IN_PROCESS_AGENT

/* Return the tracepoint with the given number and address, or NULL.  */

static struct tracepoint *
find_tracepoint (int id, CORE_ADDR addr)
{
  struct tracepoint *tpoint;

  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    if (tpoint->number == id && tpoint->address == addr)
      return tpoint;

  return NULL;
}

/* There may be several tracepoints with the same number (because they
   are "locations", in GDB parlance); return the next one after the
   given tracepoint, or search from the beginning of the list if the
   first argument is NULL.  */

static struct tracepoint *
find_next_tracepoint_by_number (struct tracepoint *prev_tp, int num)
{
  struct tracepoint *tpoint;

  if (prev_tp)
    tpoint = prev_tp->next;
  else
    tpoint = tracepoints;
  for (; tpoint; tpoint = tpoint->next)
    if (tpoint->number == num)
      return tpoint;

  return NULL;
}

#endif

static char *
save_string (const char *str, size_t len)
{
  char *s;

  s = xmalloc (len + 1);
  memcpy (s, str, len);
  s[len] = '\0';

  return s;
}

/* Append another action to perform when the tracepoint triggers.  */

static void
add_tracepoint_action (struct tracepoint *tpoint, char *packet)
{
  char *act;

  if (*packet == 'S')
    {
      seen_step_action_flag = 1;
      ++packet;
    }

  act = packet;

  while (*act)
    {
      char *act_start = act;
      struct tracepoint_action *action = NULL;

      switch (*act)
	{
	case 'M':
	  {
	    struct collect_memory_action *maction;
	    ULONGEST basereg;
	    int is_neg;

	    maction = xmalloc (sizeof *maction);
	    maction->base.type = *act;
	    action = &maction->base;

	    ++act;
	    is_neg = (*act == '-');
	    if (*act == '-')
	      ++act;
	    act = unpack_varlen_hex (act, &basereg);
	    ++act;
	    act = unpack_varlen_hex (act, &maction->addr);
	    ++act;
	    act = unpack_varlen_hex (act, &maction->len);
	    maction->basereg = (is_neg
				? - (int) basereg
				: (int) basereg);
	    trace_debug ("Want to collect %s bytes at 0x%s (basereg %d)",
			 pulongest (maction->len),
			 paddress (maction->addr), maction->basereg);
	    break;
	  }
	case 'R':
	  {
	    struct collect_registers_action *raction;

	    raction = xmalloc (sizeof *raction);
	    raction->base.type = *act;
	    action = &raction->base;

	    trace_debug ("Want to collect registers");
	    ++act;
	    /* skip past hex digits of mask for now */
	    while (isxdigit(*act))
	      ++act;
	    break;
	  }
	case 'L':
	  {
	    struct collect_static_trace_data_action *raction;

	    raction = xmalloc (sizeof *raction);
	    raction->base.type = *act;
	    action = &raction->base;

	    trace_debug ("Want to collect static trace data");
	    ++act;
	    break;
	  }
	case 'S':
	  trace_debug ("Unexpected step action, ignoring");
	  ++act;
	  break;
	case 'X':
	  {
	    struct eval_expr_action *xaction;

	    xaction = xmalloc (sizeof (*xaction));
	    xaction->base.type = *act;
	    action = &xaction->base;

	    trace_debug ("Want to evaluate expression");
	    xaction->expr = parse_agent_expr (&act);
	    break;
	  }
	default:
	  trace_debug ("unknown trace action '%c', ignoring...", *act);
	  break;
	case '-':
	  break;
	}

      if (action == NULL)
	break;

      if (seen_step_action_flag)
	{
	  tpoint->num_step_actions++;

	  tpoint->step_actions
	    = xrealloc (tpoint->step_actions,
			(sizeof (*tpoint->step_actions)
			 * tpoint->num_step_actions));
	  tpoint->step_actions_str
	    = xrealloc (tpoint->step_actions_str,
			(sizeof (*tpoint->step_actions_str)
			 * tpoint->num_step_actions));
	  tpoint->step_actions[tpoint->num_step_actions - 1] = action;
	  tpoint->step_actions_str[tpoint->num_step_actions - 1]
	    = save_string (act_start, act - act_start);
	}
      else
	{
	  tpoint->numactions++;
	  tpoint->actions
	    = xrealloc (tpoint->actions,
			sizeof (*tpoint->actions) * tpoint->numactions);
	  tpoint->actions_str
	    = xrealloc (tpoint->actions_str,
			sizeof (*tpoint->actions_str) * tpoint->numactions);
	  tpoint->actions[tpoint->numactions - 1] = action;
	  tpoint->actions_str[tpoint->numactions - 1]
	    = save_string (act_start, act - act_start);
	}
    }
}

#endif

/* Find or create a trace state variable with the given number.  */

static struct trace_state_variable *
get_trace_state_variable (int num)
{
  struct trace_state_variable *tsv;

#ifdef IN_PROCESS_AGENT
  /* Search for an existing variable.  */
  for (tsv = alloced_trace_state_variables; tsv; tsv = tsv->next)
    if (tsv->number == num)
      return tsv;
#endif

  /* Search for an existing variable.  */
  for (tsv = trace_state_variables; tsv; tsv = tsv->next)
    if (tsv->number == num)
      return tsv;

  return NULL;
}

/* Find or create a trace state variable with the given number.  */

static struct trace_state_variable *
create_trace_state_variable (int num, int gdb)
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);
  if (tsv != NULL)
    return tsv;

  /* Create a new variable.  */
  tsv = xmalloc (sizeof (struct trace_state_variable));
  tsv->number = num;
  tsv->initial_value = 0;
  tsv->value = 0;
  tsv->getter = NULL;
  tsv->name = NULL;
#ifdef IN_PROCESS_AGENT
  if (!gdb)
    {
      tsv->next = alloced_trace_state_variables;
      alloced_trace_state_variables = tsv;
    }
  else
#endif
    {
      tsv->next = trace_state_variables;
      trace_state_variables = tsv;
    }
  return tsv;
}

IP_AGENT_EXPORT LONGEST
get_trace_state_variable_value (int num)
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);

  if (!tsv)
    {
      trace_debug ("No trace state variable %d, skipping value get", num);
      return 0;
    }

  /* Call a getter function if we have one.  While it's tempting to
     set up something to only call the getter once per tracepoint hit,
     it could run afoul of thread races. Better to let the getter
     handle it directly, if necessary to worry about it.  */
  if (tsv->getter)
    tsv->value = (tsv->getter) ();

  trace_debug ("get_trace_state_variable_value(%d) ==> %s",
	       num, plongest (tsv->value));

  return tsv->value;
}

IP_AGENT_EXPORT void
set_trace_state_variable_value (int num, LONGEST val)
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);

  if (!tsv)
    {
      trace_debug ("No trace state variable %d, skipping value set", num);
      return;
    }

  tsv->value = val;
}

static void
set_trace_state_variable_name (int num, const char *name)
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);

  if (!tsv)
    {
      trace_debug ("No trace state variable %d, skipping name set", num);
      return;
    }

  tsv->name = (char *) name;
}

static void
set_trace_state_variable_getter (int num, LONGEST (*getter) (void))
{
  struct trace_state_variable *tsv;

  tsv = get_trace_state_variable (num);

  if (!tsv)
    {
      trace_debug ("No trace state variable %d, skipping getter set", num);
      return;
    }

  tsv->getter = getter;
}

/* Add a raw traceframe for the given tracepoint.  */

static struct traceframe *
add_traceframe (struct tracepoint *tpoint)
{
  struct traceframe *tframe;

  tframe = trace_buffer_alloc (sizeof (struct traceframe));

  if (tframe == NULL)
    return NULL;

  tframe->tpnum = tpoint->number;
  tframe->data_size = 0;

  return tframe;
}

/* Add a block to the traceframe currently being worked on.  */

static unsigned char *
add_traceframe_block (struct traceframe *tframe, int amt)
{
  unsigned char *block;

  if (!tframe)
    return NULL;

  block = trace_buffer_alloc (amt);

  if (!block)
    return NULL;

  tframe->data_size += amt;

  return block;
}

/* Flag that the current traceframe is finished.  */

static void
finish_traceframe (struct traceframe *tframe)
{
  ++traceframe_write_count;
  ++traceframes_created;
}

#ifndef IN_PROCESS_AGENT

/* Given a traceframe number NUM, find the NUMth traceframe in the
   buffer.  */

static struct traceframe *
find_traceframe (int num)
{
  struct traceframe *tframe;
  int tfnum = 0;

  for (tframe = FIRST_TRACEFRAME ();
       tframe->tpnum != 0;
       tframe = NEXT_TRACEFRAME (tframe))
    {
      if (tfnum == num)
	return tframe;
      ++tfnum;
    }

  return NULL;
}

static CORE_ADDR
get_traceframe_address (struct traceframe *tframe)
{
  CORE_ADDR addr;
  struct tracepoint *tpoint;

  addr = traceframe_get_pc (tframe);

  if (addr)
    return addr;

  /* Fallback strategy, will be incorrect for while-stepping frames
     and multi-location tracepoints.  */
  tpoint = find_next_tracepoint_by_number (NULL, tframe->tpnum);
  return tpoint->address;
}

/* Search for the next traceframe whose address is inside or outside
   the given range.  */

static struct traceframe *
find_next_traceframe_in_range (CORE_ADDR lo, CORE_ADDR hi, int inside_p,
			       int *tfnump)
{
  struct traceframe *tframe;
  CORE_ADDR tfaddr;

  *tfnump = current_traceframe + 1;
  tframe = find_traceframe (*tfnump);
  /* The search is not supposed to wrap around.  */
  if (!tframe)
    {
      *tfnump = -1;
      return NULL;
    }

  for (; tframe->tpnum != 0; tframe = NEXT_TRACEFRAME (tframe))
    {
      tfaddr = get_traceframe_address (tframe);
      if (inside_p
	  ? (lo <= tfaddr && tfaddr <= hi)
	  : (lo > tfaddr || tfaddr > hi))
	return tframe;
      ++*tfnump;
    }

  *tfnump = -1;
  return NULL;
}

/* Search for the next traceframe recorded by the given tracepoint.
   Note that for multi-location tracepoints, this will find whatever
   location appears first.  */

static struct traceframe *
find_next_traceframe_by_tracepoint (int num, int *tfnump)
{
  struct traceframe *tframe;

  *tfnump = current_traceframe + 1;
  tframe = find_traceframe (*tfnump);
  /* The search is not supposed to wrap around.  */
  if (!tframe)
    {
      *tfnump = -1;
      return NULL;
    }

  for (; tframe->tpnum != 0; tframe = NEXT_TRACEFRAME (tframe))
    {
      if (tframe->tpnum == num)
	return tframe;
      ++*tfnump;
    }

  *tfnump = -1;
  return NULL;
}

#endif

#ifndef IN_PROCESS_AGENT

/* Clear all past trace state.  */

static void
cmd_qtinit (char *packet)
{
  struct trace_state_variable *tsv, *prev, *next;

  /* Make sure we don't try to read from a trace frame.  */
  current_traceframe = -1;

  trace_debug ("Initializing the trace");

  clear_installed_tracepoints ();
  clear_readonly_regions ();

  tracepoints = NULL;
  last_tracepoint = NULL;

  /* Clear out any leftover trace state variables.  Ones with target
     defined getters should be kept however.  */
  prev = NULL;
  tsv = trace_state_variables;
  while (tsv)
    {
      trace_debug ("Looking at var %d", tsv->number);
      if (tsv->getter == NULL)
	{
	  next = tsv->next;
	  if (prev)
	    prev->next = next;
	  else
	    trace_state_variables = next;
	  trace_debug ("Deleting var %d", tsv->number);
	  free (tsv);
	  tsv = next;
	}
      else
	{
	  prev = tsv;
	  tsv = tsv->next;
	}
    }

  clear_trace_buffer ();
  clear_inferior_trace_buffer ();

  write_ok (packet);
}

/* Unprobe the UST marker at ADDRESS.  */

static void
unprobe_marker_at (CORE_ADDR address)
{
  char cmd[CMD_BUF_SIZE];

  sprintf (cmd, "unprobe_marker_at:%s", paddress (address));
  run_inferior_command (cmd);
}

/* Restore the program to its pre-tracing state.  This routine may be called
   in error situations, so it needs to be careful about only restoring
   from known-valid bits.  */

static void
clear_installed_tracepoints (void)
{
  struct tracepoint *tpoint;
  struct tracepoint *prev_stpoint;

  pause_all (1);
  cancel_breakpoints ();

  prev_stpoint = NULL;

  /* Restore any bytes overwritten by tracepoints.  */
  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    {
      if (!tpoint->enabled)
	continue;

      /* Catch the case where we might try to remove a tracepoint that
	 was never actually installed.  */
      if (tpoint->handle == NULL)
	{
	  trace_debug ("Tracepoint %d at 0x%s was "
		       "never installed, nothing to clear",
		       tpoint->number, paddress (tpoint->address));
	  continue;
	}

      switch (tpoint->type)
	{
	case trap_tracepoint:
	  delete_breakpoint (tpoint->handle);
	  break;
	case fast_tracepoint:
	  delete_fast_tracepoint_jump (tpoint->handle);
	  break;
	case static_tracepoint:
	  if (prev_stpoint != NULL
	      && prev_stpoint->address == tpoint->address)
	    /* Nothing to do.  We already unprobed a tracepoint set at
	       this marker address (and there can only be one probe
	       per marker).  */
	    ;
	  else
	    {
	      unprobe_marker_at (tpoint->address);
	      prev_stpoint = tpoint;
	    }
	  break;
	}

      tpoint->handle = NULL;
    }

  unpause_all (1);
}

/* Parse a packet that defines a tracepoint.  */

static void
cmd_qtdp (char *own_buf)
{
  int tppacket;
  ULONGEST num;
  ULONGEST addr;
  ULONGEST count;
  struct tracepoint *tpoint;
  char *actparm;
  char *packet = own_buf;

  packet += strlen ("QTDP:");

  /* A hyphen at the beginning marks a packet specifying actions for a
     tracepoint already supplied.  */
  tppacket = 1;
  if (*packet == '-')
    {
      tppacket = 0;
      ++packet;
    }
  packet = unpack_varlen_hex (packet, &num);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &addr);
  ++packet; /* skip a colon */

  /* See if we already have this tracepoint.  */
  tpoint = find_tracepoint (num, addr);

  if (tppacket)
    {
      /* Duplicate tracepoints are never allowed.  */
      if (tpoint)
	{
	  trace_debug ("Tracepoint error: tracepoint %d"
		       " at 0x%s already exists",
		       (int) num, paddress (addr));
	  write_enn (own_buf);
	  return;
	}

      tpoint = add_tracepoint (num, addr);

      tpoint->enabled = (*packet == 'E');
      ++packet; /* skip 'E' */
      ++packet; /* skip a colon */
      packet = unpack_varlen_hex (packet, &count);
      tpoint->step_count = count;
      ++packet; /* skip a colon */
      packet = unpack_varlen_hex (packet, &count);
      tpoint->pass_count = count;
      /* See if we have any of the additional optional fields.  */
      while (*packet == ':')
	{
	  ++packet;
	  if (*packet == 'F')
	    {
	      tpoint->type = fast_tracepoint;
	      ++packet;
	      packet = unpack_varlen_hex (packet, &count);
	      tpoint->orig_size = count;
	    }
	  else if (*packet == 'S')
	    {
	      tpoint->type = static_tracepoint;
	      ++packet;
	    }
	  else if (*packet == 'X')
	    {
	      actparm = (char *) packet;
	      tpoint->cond = parse_agent_expr (&actparm);
	      packet = actparm;
	    }
	  else if (*packet == '-')
	    break;
	  else if (*packet == '\0')
	    break;
	  else
	    trace_debug ("Unknown optional tracepoint field");
	}
      if (*packet == '-')
	trace_debug ("Also has actions\n");

      trace_debug ("Defined %stracepoint %d at 0x%s, "
		   "enabled %d step %ld pass %ld",
		   tpoint->type == fast_tracepoint ? "fast "
		   : "",
		   tpoint->number, paddress (tpoint->address), tpoint->enabled,
		   tpoint->step_count, tpoint->pass_count);
    }
  else if (tpoint)
    add_tracepoint_action (tpoint, packet);
  else
    {
      trace_debug ("Tracepoint error: tracepoint %d at 0x%s not found",
		   (int) num, paddress (addr));
      write_enn (own_buf);
      return;
    }

  write_ok (own_buf);
}

static void
cmd_qtdpsrc (char *own_buf)
{
  ULONGEST num, addr, start, slen;
  struct tracepoint *tpoint;
  char *packet = own_buf;
  char *saved, *srctype, *src;
  size_t nbytes;
  struct source_string *last, *newlast;

  packet += strlen ("QTDPsrc:");

  packet = unpack_varlen_hex (packet, &num);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &addr);
  ++packet; /* skip a colon */

  /* See if we already have this tracepoint.  */
  tpoint = find_tracepoint (num, addr);

  if (!tpoint)
    {
      trace_debug ("Tracepoint error: tracepoint %d at 0x%s not found",
		   (int) num, paddress (addr));
      write_enn (own_buf);
      return;
    }

  saved = packet;
  packet = strchr (packet, ':');
  srctype = xmalloc (packet - saved + 1);
  memcpy (srctype, saved, packet - saved);
  srctype[packet - saved] = '\0';
  ++packet;
  packet = unpack_varlen_hex (packet, &start);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &slen);
  ++packet; /* skip a colon */
  src = xmalloc (slen + 1);
  nbytes = unhexify (src, packet, strlen (packet) / 2);
  src[nbytes] = '\0';

  newlast = xmalloc (sizeof (struct source_string));
  newlast->type = srctype;
  newlast->str = src;
  newlast->next = NULL;
  /* Always add a source string to the end of the list;
     this keeps sequences of actions/commands in the right
     order.  */
  if (tpoint->source_strings)
    {
      for (last = tpoint->source_strings; last->next; last = last->next)
	;
      last->next = newlast;
    }
  else
    tpoint->source_strings = newlast;

  write_ok (own_buf);
}

static void
cmd_qtdv (char *own_buf)
{
  ULONGEST num, val, builtin;
  char *varname;
  size_t nbytes;
  struct trace_state_variable *tsv;
  char *packet = own_buf;

  packet += strlen ("QTDV:");

  packet = unpack_varlen_hex (packet, &num);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &val);
  ++packet; /* skip a colon */
  packet = unpack_varlen_hex (packet, &builtin);
  ++packet; /* skip a colon */

  nbytes = strlen (packet) / 2;
  varname = xmalloc (nbytes + 1);
  nbytes = unhexify (varname, packet, nbytes);
  varname[nbytes] = '\0';

  tsv = create_trace_state_variable (num, 1);
  tsv->initial_value = (LONGEST) val;
  tsv->name = varname;

  set_trace_state_variable_value (num, (LONGEST) val);

  write_ok (own_buf);
}

static void
cmd_qtv (char *own_buf)
{
  ULONGEST num;
  LONGEST val;
  int err;
  char *packet = own_buf;

  packet += strlen ("qTV:");
  packet = unpack_varlen_hex (packet, &num);

  if (current_traceframe >= 0)
    {
      err = traceframe_read_tsv ((int) num, &val);
      if (err)
	{
	  strcpy (own_buf, "U");
	  return;
	}
    }
  /* Only make tsv's be undefined before the first trace run.  After a
     trace run is over, the user might want to see the last value of
     the tsv, and it might not be available in a traceframe.  */
  else if (!tracing && strcmp (tracing_stop_reason, "tnotrun") == 0)
    {
      strcpy (own_buf, "U");
      return;
    }
  else
    val = get_trace_state_variable_value (num);

  sprintf (own_buf, "V%s", phex_nz (val, 0));
}

/* Clear out the list of readonly regions.  */

static void
clear_readonly_regions (void)
{
  struct readonly_region *roreg;

  while (readonly_regions)
    {
      roreg = readonly_regions;
      readonly_regions = readonly_regions->next;
      free (roreg);
    }
}

/* Parse the collection of address ranges whose contents GDB believes
   to be unchanging and so can be read directly from target memory
   even while looking at a traceframe.  */

static void
cmd_qtro (char *own_buf)
{
  ULONGEST start, end;
  struct readonly_region *roreg;
  char *packet = own_buf;

  trace_debug ("Want to mark readonly regions");

  clear_readonly_regions ();

  packet += strlen ("QTro");

  while (*packet == ':')
    {
      ++packet;  /* skip a colon */
      packet = unpack_varlen_hex (packet, &start);
      ++packet;  /* skip a comma */
      packet = unpack_varlen_hex (packet, &end);
      roreg = xmalloc (sizeof (struct readonly_region));
      roreg->start = start;
      roreg->end = end;
      roreg->next = readonly_regions;
      readonly_regions = roreg;
      trace_debug ("Added readonly region from 0x%s to 0x%s",
		   paddress (roreg->start), paddress (roreg->end));
    }

  write_ok (own_buf);
}

/* Test to see if the given range is in our list of readonly ranges.
   We only test for being entirely within a range, GDB is not going to
   send a single memory packet that spans multiple regions.  */

int
in_readonly_region (CORE_ADDR addr, ULONGEST length)
{
  struct readonly_region *roreg;

  for (roreg = readonly_regions; roreg; roreg = roreg->next)
    if (roreg->start <= addr && (addr + length - 1) <= roreg->end)
      return 1;

  return 0;
}

/* The maximum size of a jump pad entry.  */
static const int max_jump_pad_size = 0x100;

static CORE_ADDR gdb_jump_pad_head;

/* Return the address of the next free jump space.  */

static CORE_ADDR
get_jump_space_head (void)
{
  if (gdb_jump_pad_head == 0)
    {
      if (read_inferior_data_pointer (ipa_sym_addrs.addr_gdb_jump_pad_buffer,
				      &gdb_jump_pad_head))
	fatal ("error extracting jump_pad_buffer");
    }

  return gdb_jump_pad_head;
}

/* Reserve USED bytes from the jump space.  */

static void
claim_jump_space (ULONGEST used)
{
  trace_debug ("claim_jump_space reserves %s bytes at %s",
	       pulongest (used), paddress (gdb_jump_pad_head));
  gdb_jump_pad_head += used;
}

/* Sort tracepoints by PC, using a bubble sort.  */

static void
sort_tracepoints (void)
{
  struct tracepoint *lst, *tmp, *prev = NULL;
  int i, j, n = 0;

  if (tracepoints == NULL)
    return;

  /* Count nodes.  */
  for (tmp = tracepoints; tmp->next; tmp = tmp->next)
    n++;

  for (i = 0; i < n - 1; i++)
    for (j = 0, lst = tracepoints;
	 lst && lst->next && (j <= n - 1 - i);
	 j++)
      {
	/* If we're at beginning, the start node is the prev
	   node.  */
	if (j == 0)
	  prev = lst;

	/* Compare neighbors.  */
	if (lst->next->address < lst->address)
	  {
	    struct tracepoint *p;

	    /* Swap'em.  */
	    tmp = (lst->next ? lst->next->next : NULL);

	    if (j == 0 && prev == tracepoints)
	      tracepoints = lst->next;

	    p = lst->next;
	    prev->next = lst->next;
	    lst->next->next = lst;
	    lst->next = tmp;
	    prev = p;
	  }
	else
	  {
	    lst = lst->next;
	    /* Keep track of the previous node.  We need it if we need
	       to swap nodes.  */
	    if (j != 0)
	      prev = prev->next;
	  }
      }
}

/* Ask the IPA to probe the marker at ADDRESS.  Returns -1 if running
   the command fails, or 0 otherwise.  If the command ran
   successfully, but probing the marker failed, ERROUT will be filled
   with the error to reply to GDB, and -1 is also returned.  This
   allows directly passing IPA errors to GDB.  */

static int
probe_marker_at (CORE_ADDR address, char *errout)
{
  char cmd[CMD_BUF_SIZE];
  int err;

  sprintf (cmd, "probe_marker_at:%s", paddress (address));
  err = run_inferior_command (cmd);

  if (err == 0)
    {
      if (*cmd == 'E')
	{
	  strcpy (errout, cmd);
	  return -1;
	}
    }

  return err;
}

#define MAX_JUMP_SIZE 20

static void
cmd_qtstart (char *packet)
{
  struct tracepoint *tpoint, *prev_ftpoint, *prev_stpoint;
  int slow_tracepoint_count, fast_count;
  CORE_ADDR jump_entry;

  /* The jump to the jump pad of the last fast tracepoint
     installed.  */
  unsigned char fjump[MAX_JUMP_SIZE];
  ULONGEST fjump_size;

  trace_debug ("Starting the trace");

  slow_tracepoint_count = fast_count = 0;

  /* Sort tracepoints by ascending address.  This makes installing
     fast tracepoints at the same address easier to handle. */
  sort_tracepoints ();

  /* Pause all threads temporarily while we patch tracepoints.  */
  pause_all (0);

  /* Get threads out of jump pads.  Safe to do here, since this is a
     top level command.  And, required to do here, since we're
     deleting/rewriting jump pads.  */

  stabilize_threads ();

  /* Freeze threads.  */
  pause_all (1);

  /* Sync the fast tracepoints list in the inferior ftlib.  */
  if (in_process_agent_loaded ())
    {
      download_tracepoints ();
      download_trace_state_variables ();
    }

  /* No previous fast tpoint yet.  */
  prev_ftpoint = NULL;

  /* No previous static tpoint yet.  */
  prev_stpoint = NULL;

  *packet = '\0';

  /* Install tracepoints.  */
  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    {
      /* Ensure all the hit counts start at zero.  */
      tpoint->hit_count = 0;

      if (!tpoint->enabled)
	continue;

      if (tpoint->type == trap_tracepoint)
	{
	  ++slow_tracepoint_count;

	  /* Tracepoints are installed as memory breakpoints.  Just go
	     ahead and install the trap.  The breakpoints module
	     handles duplicated breakpoints, and the memory read
	     routine handles un-patching traps from memory reads.  */
	  tpoint->handle = set_breakpoint_at (tpoint->address,
					      tracepoint_handler);
	}
      else if (tpoint->type == fast_tracepoint)
	{
	  ++fast_count;

	  if (maybe_write_ipa_not_loaded (packet))
	    {
	      trace_debug ("Requested a fast tracepoint, but fast "
			   "tracepoints aren't supported.");
	      break;
	    }

	  if (prev_ftpoint != NULL && prev_ftpoint->address == tpoint->address)
	    {
	      tpoint->handle = set_fast_tracepoint_jump (tpoint->address,
							 fjump,
							 fjump_size);
	      tpoint->jump_pad = prev_ftpoint->jump_pad;
	      tpoint->jump_pad_end = prev_ftpoint->jump_pad_end;
	      tpoint->adjusted_insn_addr = prev_ftpoint->adjusted_insn_addr;
	      tpoint->adjusted_insn_addr_end
		= prev_ftpoint->adjusted_insn_addr_end;
	    }
	  else
	    {
	      CORE_ADDR jentry;
	      int err = 0;

	      prev_ftpoint = NULL;

	      jentry = jump_entry = get_jump_space_head ();

	      /* Install the jump pad.  */
	      err = install_fast_tracepoint_jump_pad
		(tpoint->obj_addr_on_target,
		 tpoint->address,
		 ipa_sym_addrs.addr_gdb_collect,
		 ipa_sym_addrs.addr_collecting,
		 tpoint->orig_size,
		 &jentry,
		 fjump, &fjump_size,
		 &tpoint->adjusted_insn_addr,
		 &tpoint->adjusted_insn_addr_end);

	      /* Wire it in.  */
	      if (!err)
		tpoint->handle = set_fast_tracepoint_jump (tpoint->address,
							   fjump, fjump_size);

	      if (tpoint->handle != NULL)
		{
		  tpoint->jump_pad = jump_entry;
		  tpoint->jump_pad_end = jentry;

		  /* Pad to 8-byte alignment.  */
		  jentry = ((jentry + 7) & ~0x7);
		  claim_jump_space (jentry - jump_entry);

		  /* So that we can handle multiple fast tracepoints
		     at the same address easily.  */
		  prev_ftpoint = tpoint;
		}
	    }
	}
      else if (tpoint->type == static_tracepoint)
	{
	  if (maybe_write_ipa_ust_not_loaded (packet))
	    {
	      trace_debug ("Requested a static tracepoint, but static "
			   "tracepoints are not supported.");
	      break;
	    }

	  /* Can only probe a given marker once.  */
	  if (prev_stpoint != NULL && prev_stpoint->address == tpoint->address)
	    {
	      tpoint->handle = (void *) -1;
	    }
	  else
	    {
	      if (probe_marker_at (tpoint->address, packet) == 0)
		{
		  tpoint->handle = (void *) -1;

		  /* So that we can handle multiple static tracepoints
		     at the same address easily.  */
		  prev_stpoint = tpoint;
		}
	    }
	}

      /* Any failure in the inner loop is sufficient cause to give
	 up.  */
      if (tpoint->handle == NULL)
	break;
    }

  /* Any error in tracepoint insertion is unacceptable; better to
     address the problem now, than end up with a useless or misleading
     trace run.  */
  if (tpoint != NULL)
    {
      clear_installed_tracepoints ();
      if (*packet == '\0')
	write_enn (packet);
      unpause_all (1);
      return;
    }

  stopping_tracepoint = NULL;
  trace_buffer_is_full = 0;
  expr_eval_result = expr_eval_no_error;
  error_tracepoint = NULL;

  /* Tracing is now active, hits will now start being logged.  */
  tracing = 1;

  if (in_process_agent_loaded ())
    {
      if (write_inferior_integer (ipa_sym_addrs.addr_tracing, 1))
	fatal ("Error setting tracing variable in lib");

      if (write_inferior_data_pointer (ipa_sym_addrs.addr_stopping_tracepoint,
				       0))
	fatal ("Error clearing stopping_tracepoint variable in lib");

      if (write_inferior_integer (ipa_sym_addrs.addr_trace_buffer_is_full, 0))
	fatal ("Error clearing trace_buffer_is_full variable in lib");

      stop_tracing_bkpt = set_breakpoint_at (ipa_sym_addrs.addr_stop_tracing,
					     stop_tracing_handler);
      if (stop_tracing_bkpt == NULL)
	error ("Error setting stop_tracing breakpoint");

      flush_trace_buffer_bkpt
	= set_breakpoint_at (ipa_sym_addrs.addr_flush_trace_buffer,
			     flush_trace_buffer_handler);
      if (flush_trace_buffer_bkpt == NULL)
	error ("Error setting flush_trace_buffer breakpoint");
    }

  unpause_all (1);

  write_ok (packet);
}

/* End a tracing run, filling in a stop reason to report back to GDB,
   and removing the tracepoints from the code.  */

void
stop_tracing (void)
{
  if (!tracing)
    {
      trace_debug ("Tracing is already off, ignoring");
      return;
    }

  trace_debug ("Stopping the trace");

  /* Pause all threads before removing fast jumps from memory,
     breakpoints, and touching IPA state variables (inferior memory).
     Some thread may hit the internal tracing breakpoints, or be
     collecting this moment, but that's ok, we don't release the
     tpoint object's memory or the jump pads here (we only do that
     when we're sure we can move all threads out of the jump pads).
     We can't now, since we may be getting here due to the inferior
     agent calling us.  */
  pause_all (1);
  /* Since we're removing breakpoints, cancel breakpoint hits,
     possibly related to the breakpoints we're about to delete.  */
  cancel_breakpoints ();

  /* Stop logging. Tracepoints can still be hit, but they will not be
     recorded.  */
  tracing = 0;
  if (in_process_agent_loaded ())
    {
      if (write_inferior_integer (ipa_sym_addrs.addr_tracing, 0))
	fatal ("Error clearing tracing variable in lib");
    }

  tracing_stop_reason = "t???";
  tracing_stop_tpnum = 0;
  if (stopping_tracepoint)
    {
      trace_debug ("Stopping the trace because "
		   "tracepoint %d was hit %ld times",
		   stopping_tracepoint->number,
		   stopping_tracepoint->pass_count);
      tracing_stop_reason = "tpasscount";
      tracing_stop_tpnum = stopping_tracepoint->number;
    }
  else if (trace_buffer_is_full)
    {
      trace_debug ("Stopping the trace because the trace buffer is full");
      tracing_stop_reason = "tfull";
    }
  else if (expr_eval_result != expr_eval_no_error)
    {
      trace_debug ("Stopping the trace because of an expression eval error");
      tracing_stop_reason = eval_result_names[expr_eval_result];
      tracing_stop_tpnum = error_tracepoint->number;
    }
#ifndef IN_PROCESS_AGENT
  else if (!gdb_connected ())
    {
      trace_debug ("Stopping the trace because GDB disconnected");
      tracing_stop_reason = "tdisconnected";
    }
#endif
  else
    {
      trace_debug ("Stopping the trace because of a tstop command");
      tracing_stop_reason = "tstop";
    }

  stopping_tracepoint = NULL;
  error_tracepoint = NULL;

  /* Clear out the tracepoints.  */
  clear_installed_tracepoints ();

  if (in_process_agent_loaded ())
    {
      /* Pull in fast tracepoint trace frames from the inferior lib
	 buffer into our buffer, even if our buffer is already full,
	 because we want to present the full number of created frames
	 in addition to what fit in the trace buffer.  */
      upload_fast_traceframes ();
    }

  if (stop_tracing_bkpt != NULL)
    {
      delete_breakpoint (stop_tracing_bkpt);
      stop_tracing_bkpt = NULL;
    }

  if (flush_trace_buffer_bkpt != NULL)
    {
      delete_breakpoint (flush_trace_buffer_bkpt);
      flush_trace_buffer_bkpt = NULL;
    }

  unpause_all (1);
}

static int
stop_tracing_handler (CORE_ADDR addr)
{
  trace_debug ("lib hit stop_tracing");

  /* Don't actually handle it here.  When we stop tracing we remove
     breakpoints from the inferior, and that is not allowed in a
     breakpoint handler (as the caller is walking the breakpoint
     list).  */
  return 0;
}

static int
flush_trace_buffer_handler (CORE_ADDR addr)
{
  trace_debug ("lib hit flush_trace_buffer");
  return 0;
}

static void
cmd_qtstop (char *packet)
{
  stop_tracing ();
  write_ok (packet);
}

static void
cmd_qtdisconnected (char *own_buf)
{
  ULONGEST setting;
  char *packet = own_buf;

  packet += strlen ("QTDisconnected:");

  unpack_varlen_hex (packet, &setting);

  write_ok (own_buf);

  disconnected_tracing = setting;
}

static void
cmd_qtframe (char *own_buf)
{
  ULONGEST frame, pc, lo, hi, num;
  int tfnum, tpnum;
  struct traceframe *tframe;
  char *packet = own_buf;

  packet += strlen ("QTFrame:");

  if (strncmp (packet, "pc:", strlen ("pc:")) == 0)
    {
      packet += strlen ("pc:");
      packet = unpack_varlen_hex (packet, &pc);
      trace_debug ("Want to find next traceframe at pc=0x%s", paddress (pc));
      tframe = find_next_traceframe_in_range (pc, pc, 1, &tfnum);
    }
  else if (strncmp (packet, "range:", strlen ("range:")) == 0)
    {
      packet += strlen ("range:");
      packet = unpack_varlen_hex (packet, &lo);
      ++packet;
      packet = unpack_varlen_hex (packet, &hi);
      trace_debug ("Want to find next traceframe in the range 0x%s to 0x%s",
		   paddress (lo), paddress (hi));
      tframe = find_next_traceframe_in_range (lo, hi, 1, &tfnum);
    }
  else if (strncmp (packet, "outside:", strlen ("outside:")) == 0)
    {
      packet += strlen ("outside:");
      packet = unpack_varlen_hex (packet, &lo);
      ++packet;
      packet = unpack_varlen_hex (packet, &hi);
      trace_debug ("Want to find next traceframe "
		   "outside the range 0x%s to 0x%s",
		   paddress (lo), paddress (hi));
      tframe = find_next_traceframe_in_range (lo, hi, 0, &tfnum);
    }
  else if (strncmp (packet, "tdp:", strlen ("tdp:")) == 0)
    {
      packet += strlen ("tdp:");
      packet = unpack_varlen_hex (packet, &num);
      tpnum = (int) num;
      trace_debug ("Want to find next traceframe for tracepoint %d", tpnum);
      tframe = find_next_traceframe_by_tracepoint (tpnum, &tfnum);
    }
  else
    {
      unpack_varlen_hex (packet, &frame);
      tfnum = (int) frame;
      if (tfnum == -1)
	{
	  trace_debug ("Want to stop looking at traceframes");
	  current_traceframe = -1;
	  write_ok (own_buf);
	  return;
	}
      trace_debug ("Want to look at traceframe %d", tfnum);
      tframe = find_traceframe (tfnum);
    }

  if (tframe)
    {
      current_traceframe = tfnum;
      sprintf (own_buf, "F%xT%x", tfnum, tframe->tpnum);
    }
  else
    sprintf (own_buf, "F-1");
}

static void
cmd_qtstatus (char *packet)
{
  char *stop_reason_rsp = NULL;

  trace_debug ("Returning trace status as %d, stop reason %s",
	       tracing, tracing_stop_reason);

  if (in_process_agent_loaded ())
    {
      pause_all (1);

      upload_fast_traceframes ();

      unpause_all (1);
   }

  stop_reason_rsp = (char *) tracing_stop_reason;

  /* The user visible error string in terror needs to be hex encoded.
     We leave it as plain string in `tracepoint_stop_reason' to ease
     debugging.  */
  if (strncmp (stop_reason_rsp, "terror:", strlen ("terror:")) == 0)
    {
      const char *result_name;
      int hexstr_len;
      char *p;

      result_name = stop_reason_rsp + strlen ("terror:");
      hexstr_len = strlen (result_name) * 2;
      p = stop_reason_rsp = alloca (strlen ("terror:") + hexstr_len + 1);
      strcpy (p, "terror:");
      p += strlen (p);
      convert_int_to_ascii ((gdb_byte *) result_name, p, strlen (result_name));
    }

  sprintf (packet,
	   "T%d;"
	   "%s:%x;"
	   "tframes:%x;tcreated:%x;"
	   "tfree:%x;tsize:%s;"
	   "circular:%d;"
	   "disconn:%d",
	   tracing ? 1 : 0,
	   stop_reason_rsp, tracing_stop_tpnum,
	   traceframe_count, traceframes_created,
	   free_space (), phex_nz (trace_buffer_hi - trace_buffer_lo, 0),
	   circular_trace_buffer,
	   disconnected_tracing);
}

/* State variables to help return all the tracepoint bits.  */
static struct tracepoint *cur_tpoint;
static int cur_action;
static int cur_step_action;
static struct source_string *cur_source_string;
static struct trace_state_variable *cur_tsv;

/* Compose a response that is an imitation of the syntax by which the
   tracepoint was originally downloaded.  */

static void
response_tracepoint (char *packet, struct tracepoint *tpoint)
{
  char *buf;

  sprintf (packet, "T%x:%s:%c:%lx:%lx", tpoint->number,
	   paddress (tpoint->address),
	   (tpoint->enabled ? 'E' : 'D'), tpoint->step_count,
	   tpoint->pass_count);
  if (tpoint->type == fast_tracepoint)
    sprintf (packet + strlen (packet), ":F%x", tpoint->orig_size);
  else if (tpoint->type == static_tracepoint)
    sprintf (packet + strlen (packet), ":S");

  if (tpoint->cond)
    {
      buf = unparse_agent_expr (tpoint->cond);
      sprintf (packet + strlen (packet), ":X%x,%s",
	       tpoint->cond->length, buf);
      free (buf);
    }
}

/* Compose a response that is an imitation of the syntax by which the
   tracepoint action was originally downloaded (with the difference
   that due to the way we store the actions, this will output a packet
   per action, while GDB could have combined more than one action
   per-packet.  */

static void
response_action (char *packet, struct tracepoint *tpoint,
		 char *taction, int step)
{
  sprintf (packet, "%c%x:%s:%s",
	   (step ? 'S' : 'A'), tpoint->number, paddress (tpoint->address),
	   taction);
}

/* Compose a response that is an imitation of the syntax by which the
   tracepoint source piece was originally downloaded.  */

static void
response_source (char *packet,
		 struct tracepoint *tpoint, struct source_string *src)
{
  char *buf;
  int len;

  len = strlen (src->str);
  buf = alloca (len * 2 + 1);
  convert_int_to_ascii ((gdb_byte *) src->str, buf, len);

  sprintf (packet, "Z%x:%s:%s:%x:%x:%s",
	   tpoint->number, paddress (tpoint->address),
	   src->type, 0, len, buf);
}

/* Return the first piece of tracepoint definition, and initialize the
   state machine that will iterate through all the tracepoint
   bits.  */

static void
cmd_qtfp (char *packet)
{
  trace_debug ("Returning first tracepoint definition piece");

  cur_tpoint = tracepoints;
  cur_action = cur_step_action = -1;
  cur_source_string = NULL;

  if (cur_tpoint)
    response_tracepoint (packet, cur_tpoint);
  else
    strcpy (packet, "l");
}

/* Return additional pieces of tracepoint definition.  Each action and
   stepping action must go into its own packet, because of packet size
   limits, and so we use state variables to deliver one piece at a
   time.  */

static void
cmd_qtsp (char *packet)
{
  trace_debug ("Returning subsequent tracepoint definition piece");

  if (!cur_tpoint)
    {
      /* This case would normally never occur, but be prepared for
	 GDB misbehavior.  */
      strcpy (packet, "l");
    }
  else if (cur_action < cur_tpoint->numactions - 1)
    {
      ++cur_action;
      response_action (packet, cur_tpoint,
		       cur_tpoint->actions_str[cur_action], 0);
    }
  else if (cur_step_action < cur_tpoint->num_step_actions - 1)
    {
      ++cur_step_action;
      response_action (packet, cur_tpoint,
		       cur_tpoint->step_actions_str[cur_step_action], 1);
    }
  else if ((cur_source_string
	    ? cur_source_string->next
	    : cur_tpoint->source_strings))
    {
      if (cur_source_string)
	cur_source_string = cur_source_string->next;
      else
	cur_source_string = cur_tpoint->source_strings;
      response_source (packet, cur_tpoint, cur_source_string);
    }
  else
    {
      cur_tpoint = cur_tpoint->next;
      cur_action = cur_step_action = -1;
      cur_source_string = NULL;
      if (cur_tpoint)
	response_tracepoint (packet, cur_tpoint);
      else
	strcpy (packet, "l");
    }
}

/* Compose a response that is an imitation of the syntax by which the
   trace state variable was originally downloaded.  */

static void
response_tsv (char *packet, struct trace_state_variable *tsv)
{
  char *buf = (char *) "";
  int namelen;

  if (tsv->name)
    {
      namelen = strlen (tsv->name);
      buf = alloca (namelen * 2 + 1);
      convert_int_to_ascii ((gdb_byte *) tsv->name, buf, namelen);
    }

  sprintf (packet, "%x:%s:%x:%s", tsv->number, phex_nz (tsv->initial_value, 0),
	   tsv->getter ? 1 : 0, buf);
}

/* Return the first trace state variable definition, and initialize
   the state machine that will iterate through all the tsv bits.  */

static void
cmd_qtfv (char *packet)
{
  trace_debug ("Returning first trace state variable definition");

  cur_tsv = trace_state_variables;

  if (cur_tsv)
    response_tsv (packet, cur_tsv);
  else
    strcpy (packet, "l");
}

/* Return additional trace state variable definitions. */

static void
cmd_qtsv (char *packet)
{
  trace_debug ("Returning first trace state variable definition");

  if (!cur_tpoint)
    {
      /* This case would normally never occur, but be prepared for
	 GDB misbehavior.  */
      strcpy (packet, "l");
    }
  else if (cur_tsv)
    {
      cur_tsv = cur_tsv->next;
      if (cur_tsv)
	response_tsv (packet, cur_tsv);
      else
	strcpy (packet, "l");
    }
  else
    strcpy (packet, "l");
}

/* Return the first static tracepoint marker, and initialize the state
   machine that will iterate through all the static tracepoints
   markers.  */

static void
cmd_qtfstm (char *packet)
{
  if (!maybe_write_ipa_ust_not_loaded (packet))
    run_inferior_command (packet);
}

/* Return additional static tracepoints markers.  */

static void
cmd_qtsstm (char *packet)
{
  if (!maybe_write_ipa_ust_not_loaded (packet))
    run_inferior_command (packet);
}

/* Return the definition of the static tracepoint at a given address.
   Result packet is the same as qTsST's.  */

static void
cmd_qtstmat (char *packet)
{
  if (!maybe_write_ipa_ust_not_loaded (packet))
    run_inferior_command (packet);
}

/* Respond to qTBuffer packet with a block of raw data from the trace
   buffer.  GDB may ask for a lot, but we are allowed to reply with
   only as much as will fit within packet limits or whatever.  */

static void
cmd_qtbuffer (char *own_buf)
{
  ULONGEST offset, num, tot;
  unsigned char *tbp;
  char *packet = own_buf;

  packet += strlen ("qTBuffer:");

  packet = unpack_varlen_hex (packet, &offset);
  ++packet; /* skip a comma */
  packet = unpack_varlen_hex (packet, &num);

  trace_debug ("Want to get trace buffer, %d bytes at offset 0x%s",
	       (int) num, pulongest (offset));

  tot = (trace_buffer_hi - trace_buffer_lo) - free_space ();

  /* If we're right at the end, reply specially that we're done.  */
  if (offset == tot)
    {
      strcpy (own_buf, "l");
      return;
    }

  /* Object to any other out-of-bounds request.  */
  if (offset > tot)
    {
      write_enn (own_buf);
      return;
    }

  /* Compute the pointer corresponding to the given offset, accounting
     for wraparound.  */
  tbp = trace_buffer_start + offset;
  if (tbp >= trace_buffer_wrap)
    tbp -= (trace_buffer_wrap - trace_buffer_lo);

  /* Trim to the remaining bytes if we're close to the end.  */
  if (num > tot - offset)
    num = tot - offset;

  /* Trim to available packet size.  */
  if (num >= (PBUFSIZ - 16) / 2 )
    num = (PBUFSIZ - 16) / 2;

  convert_int_to_ascii (tbp, own_buf, num);
  own_buf[num] = '\0';
}

static void
cmd_bigqtbuffer (char *own_buf)
{
  ULONGEST val;
  char *packet = own_buf;

  packet += strlen ("QTBuffer:");

  if (strncmp ("circular:", packet, strlen ("circular:")) == 0)
    {
      packet += strlen ("circular:");
      packet = unpack_varlen_hex (packet, &val);
      circular_trace_buffer = val;
      trace_debug ("Trace buffer is now %s",
		   circular_trace_buffer ? "circular" : "linear");
      write_ok (own_buf);
    }
  else
    write_enn (own_buf);
}

int
handle_tracepoint_general_set (char *packet)
{
  if (strcmp ("QTinit", packet) == 0)
    {
      cmd_qtinit (packet);
      return 1;
    }
  else if (strncmp ("QTDP:", packet, strlen ("QTDP:")) == 0)
    {
      cmd_qtdp (packet);
      return 1;
    }
  else if (strncmp ("QTDPsrc:", packet, strlen ("QTDPsrc:")) == 0)
    {
      cmd_qtdpsrc (packet);
      return 1;
    }
  else if (strncmp ("QTDV:", packet, strlen ("QTDV:")) == 0)
    {
      cmd_qtdv (packet);
      return 1;
    }
  else if (strncmp ("QTro:", packet, strlen ("QTro:")) == 0)
    {
      cmd_qtro (packet);
      return 1;
    }
  else if (strcmp ("QTStart", packet) == 0)
    {
      cmd_qtstart (packet);
      return 1;
    }
  else if (strcmp ("QTStop", packet) == 0)
    {
      cmd_qtstop (packet);
      return 1;
    }
  else if (strncmp ("QTDisconnected:", packet,
		    strlen ("QTDisconnected:")) == 0)
    {
      cmd_qtdisconnected (packet);
      return 1;
    }
  else if (strncmp ("QTFrame:", packet, strlen ("QTFrame:")) == 0)
    {
      cmd_qtframe (packet);
      return 1;
    }
  else if (strncmp ("QTBuffer:", packet, strlen ("QTBuffer:")) == 0)
    {
      cmd_bigqtbuffer (packet);
      return 1;
    }

  return 0;
}

int
handle_tracepoint_query (char *packet)
{
  if (strcmp ("qTStatus", packet) == 0)
    {
      cmd_qtstatus (packet);
      return 1;
    }
  else if (strcmp ("qTfP", packet) == 0)
    {
      cmd_qtfp (packet);
      return 1;
    }
  else if (strcmp ("qTsP", packet) == 0)
    {
      cmd_qtsp (packet);
      return 1;
    }
  else if (strcmp ("qTfV", packet) == 0)
    {
      cmd_qtfv (packet);
      return 1;
    }
  else if (strcmp ("qTsV", packet) == 0)
    {
      cmd_qtsv (packet);
      return 1;
    }
  else if (strncmp ("qTV:", packet, strlen ("qTV:")) == 0)
    {
      cmd_qtv (packet);
      return 1;
    }
  else if (strncmp ("qTBuffer:", packet, strlen ("qTBuffer:")) == 0)
    {
      cmd_qtbuffer (packet);
      return 1;
    }
  else if (strcmp ("qTfSTM", packet) == 0)
    {
      cmd_qtfstm (packet);
      return 1;
    }
  else if (strcmp ("qTsSTM", packet) == 0)
    {
      cmd_qtsstm (packet);
      return 1;
    }
  else if (strncmp ("qTSTMat:", packet, strlen ("qTSTMat:")) == 0)
    {
      cmd_qtstmat (packet);
      return 1;
    }

  return 0;
}

#endif
#ifndef IN_PROCESS_AGENT

/* Call this when thread TINFO has hit the tracepoint defined by
   TP_NUMBER and TP_ADDRESS, and that tracepoint has a while-stepping
   action.  This adds a while-stepping collecting state item to the
   threads' collecting state list, so that we can keep track of
   multiple simultaneous while-stepping actions being collected by the
   same thread.  This can happen in cases like:

    ff0001  INSN1 <-- TP1, while-stepping 10 collect $regs
    ff0002  INSN2
    ff0003  INSN3 <-- TP2, collect $regs
    ff0004  INSN4 <-- TP3, while-stepping 10 collect $regs
    ff0005  INSN5

   Notice that when instruction INSN5 is reached, the while-stepping
   actions of both TP1 and TP3 are still being collected, and that TP2
   had been collected meanwhile.  The whole range of ff0001-ff0005
   should be single-stepped, due to at least TP1's while-stepping
   action covering the whole range.  */

static void
add_while_stepping_state (struct thread_info *tinfo,
			  int tp_number, CORE_ADDR tp_address)
{
  struct wstep_state *wstep;

  wstep = xmalloc (sizeof (*wstep));
  wstep->next = tinfo->while_stepping;

  wstep->tp_number = tp_number;
  wstep->tp_address = tp_address;
  wstep->current_step = 0;

  tinfo->while_stepping = wstep;
}

/* Release the while-stepping collecting state WSTEP.  */

static void
release_while_stepping_state (struct wstep_state *wstep)
{
  free (wstep);
}

/* Release all while-stepping collecting states currently associated
   with thread TINFO.  */

void
release_while_stepping_state_list (struct thread_info *tinfo)
{
  struct wstep_state *head;

  while (tinfo->while_stepping)
    {
      head = tinfo->while_stepping;
      tinfo->while_stepping = head->next;
      release_while_stepping_state (head);
    }
}

/* If TINFO was handling a 'while-stepping' action, the step has
   finished, so collect any step data needed, and check if any more
   steps are required.  Return true if the thread was indeed
   collecting tracepoint data, false otherwise.  */

int
tracepoint_finished_step (struct thread_info *tinfo, CORE_ADDR stop_pc)
{
  struct tracepoint *tpoint;
  struct wstep_state *wstep;
  struct wstep_state **wstep_link;
  struct trap_tracepoint_ctx ctx;

  /* Pull in fast tracepoint trace frames from the inferior lib buffer into
     our buffer.  */
  if (in_process_agent_loaded ())
    upload_fast_traceframes ();

  /* Check if we were indeed collecting data for one of more
     tracepoints with a 'while-stepping' count.  */
  if (tinfo->while_stepping == NULL)
    return 0;

  if (!tracing)
    {
      /* We're not even tracing anymore.  Stop this thread from
	 collecting.  */
      release_while_stepping_state_list (tinfo);

      /* The thread had stopped due to a single-step request indeed
	 explained by a tracepoint.  */
      return 1;
    }

  wstep = tinfo->while_stepping;
  wstep_link = &tinfo->while_stepping;

  trace_debug ("Thread %s finished a single-step for tracepoint %d at 0x%s",
	       target_pid_to_str (tinfo->entry.id),
	       wstep->tp_number, paddress (wstep->tp_address));

  ctx.base.type = trap_tracepoint;
  ctx.regcache = get_thread_regcache (tinfo, 1);

  while (wstep != NULL)
    {
      tpoint = find_tracepoint (wstep->tp_number, wstep->tp_address);
      if (tpoint == NULL)
	{
	  trace_debug ("NO TRACEPOINT %d at 0x%s FOR THREAD %s!",
		       wstep->tp_number, paddress (wstep->tp_address),
		       target_pid_to_str (tinfo->entry.id));

	  /* Unlink.  */
	  *wstep_link = wstep->next;
	  release_while_stepping_state (wstep);
	  continue;
	}

      /* We've just finished one step.  */
      ++wstep->current_step;

      /* Collect data.  */
      collect_data_at_step ((struct tracepoint_hit_ctx *) &ctx,
			    stop_pc, tpoint, wstep->current_step);

      if (wstep->current_step >= tpoint->step_count)
	{
	  /* The requested numbers of steps have occurred.  */
	  trace_debug ("Thread %s done stepping for tracepoint %d at 0x%s",
		       target_pid_to_str (tinfo->entry.id),
		       wstep->tp_number, paddress (wstep->tp_address));

	  /* Unlink the wstep.  */
	  *wstep_link = wstep->next;
	  release_while_stepping_state (wstep);
	  wstep = *wstep_link;

	  /* Only check the hit count now, which ensure that we do all
	     our stepping before stopping the run.  */
	  if (tpoint->pass_count > 0
	      && tpoint->hit_count >= tpoint->pass_count
	      && stopping_tracepoint == NULL)
	    stopping_tracepoint = tpoint;
	}
      else
	{
	  /* Keep single-stepping until the requested numbers of steps
	     have occurred.  */
	  wstep_link = &wstep->next;
	  wstep = *wstep_link;
	}

      if (stopping_tracepoint
	  || trace_buffer_is_full
	  || expr_eval_result != expr_eval_no_error)
	{
	  stop_tracing ();
	  break;
	}
    }

  return 1;
}

/* Handle any internal tracing control breakpoint hits.  That means,
   pull traceframes from the IPA to our buffer, and syncing both
   tracing agents when the IPA's tracing stops for some reason.  */

int
handle_tracepoint_bkpts (struct thread_info *tinfo, CORE_ADDR stop_pc)
{
  /* Pull in fast tracepoint trace frames from the inferior in-process
     agent's buffer into our buffer.  */

  if (!in_process_agent_loaded ())
    return 0;

  upload_fast_traceframes ();

  /* Check if the in-process agent had decided we should stop
     tracing.  */
  if (stop_pc == ipa_sym_addrs.addr_stop_tracing)
    {
      int ipa_trace_buffer_is_full;
      CORE_ADDR ipa_stopping_tracepoint;
      int ipa_expr_eval_result;
      CORE_ADDR ipa_error_tracepoint;

      trace_debug ("lib stopped at stop_tracing");

      read_inferior_integer (ipa_sym_addrs.addr_trace_buffer_is_full,
			     &ipa_trace_buffer_is_full);

      read_inferior_data_pointer (ipa_sym_addrs.addr_stopping_tracepoint,
				  &ipa_stopping_tracepoint);
      write_inferior_data_pointer (ipa_sym_addrs.addr_stopping_tracepoint, 0);

      read_inferior_data_pointer (ipa_sym_addrs.addr_error_tracepoint,
				  &ipa_error_tracepoint);
      write_inferior_data_pointer (ipa_sym_addrs.addr_error_tracepoint, 0);

      read_inferior_integer (ipa_sym_addrs.addr_expr_eval_result,
			     &ipa_expr_eval_result);
      write_inferior_integer (ipa_sym_addrs.addr_expr_eval_result, 0);

      trace_debug ("lib: trace_buffer_is_full: %d, "
		   "stopping_tracepoint: %s, "
		   "ipa_expr_eval_result: %d, "
		   "error_tracepoint: %s, ",
		   ipa_trace_buffer_is_full,
		   paddress (ipa_stopping_tracepoint),
		   ipa_expr_eval_result,
		   paddress (ipa_error_tracepoint));

      if (debug_threads)
	{
	  if (ipa_trace_buffer_is_full)
	    trace_debug ("lib stopped due to full buffer.");
	  if (ipa_stopping_tracepoint)
	    trace_debug ("lib stopped due to tpoint");
	  if (ipa_stopping_tracepoint)
	    trace_debug ("lib stopped due to error");
	}

      if (ipa_stopping_tracepoint != 0)
	{
	  stopping_tracepoint
	    = fast_tracepoint_from_ipa_tpoint_address (ipa_stopping_tracepoint);
	}
      else if (ipa_expr_eval_result != expr_eval_no_error)
	{
	  expr_eval_result = ipa_expr_eval_result;
	  error_tracepoint
	    = fast_tracepoint_from_ipa_tpoint_address (ipa_error_tracepoint);
	}
      stop_tracing ();
      return 1;
    }
  else if (stop_pc == ipa_sym_addrs.addr_flush_trace_buffer)
    {
      trace_debug ("lib stopped at flush_trace_buffer");
      return 1;
    }

  return 0;
}

/* Return true if TINFO just hit a tracepoint.  Collect data if
   so.  */

int
tracepoint_was_hit (struct thread_info *tinfo, CORE_ADDR stop_pc)
{
  struct tracepoint *tpoint;
  int ret = 0;
  struct trap_tracepoint_ctx ctx;

  /* Not tracing, don't handle.  */
  if (!tracing)
    return 0;

  ctx.base.type = trap_tracepoint;
  ctx.regcache = get_thread_regcache (tinfo, 1);

  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    {
      /* Note that we collect fast tracepoints here as well.  We'll
	 step over the fast tracepoint jump later, which avoids the
	 double collect.  */
      if (tpoint->enabled && stop_pc == tpoint->address)
	{
	  trace_debug ("Thread %s at address of tracepoint %d at 0x%s",
		       target_pid_to_str (tinfo->entry.id),
		       tpoint->number, paddress (tpoint->address));

	  /* Test the condition if present, and collect if true.  */
	  if (!tpoint->cond
	      || (condition_true_at_tracepoint
		  ((struct tracepoint_hit_ctx *) &ctx, tpoint)))
	    collect_data_at_tracepoint ((struct tracepoint_hit_ctx *) &ctx,
					stop_pc, tpoint);

	  if (stopping_tracepoint
	      || trace_buffer_is_full
	      || expr_eval_result != expr_eval_no_error)
	    {
	      stop_tracing ();
	    }
	  /* If the tracepoint had a 'while-stepping' action, then set
	     the thread to collect this tracepoint on the following
	     single-steps.  */
	  else if (tpoint->step_count > 0)
	    {
	      add_while_stepping_state (tinfo,
					tpoint->number, tpoint->address);
	    }

	  ret = 1;
	}
    }

  return ret;
}

#endif

#if defined IN_PROCESS_AGENT && defined HAVE_UST
struct ust_marker_data;
static void collect_ust_data_at_tracepoint (struct tracepoint_hit_ctx *ctx,
					    CORE_ADDR stop_pc,
					    struct tracepoint *tpoint,
					    struct traceframe *tframe);
#endif

/* Create a trace frame for the hit of the given tracepoint in the
   given thread.  */

static void
collect_data_at_tracepoint (struct tracepoint_hit_ctx *ctx, CORE_ADDR stop_pc,
			    struct tracepoint *tpoint)
{
  struct traceframe *tframe;
  int acti;

  /* Only count it as a hit when we actually collect data.  */
  tpoint->hit_count++;

  /* If we've exceeded a defined pass count, record the event for
     later, and finish the collection for this hit.  This test is only
     for nonstepping tracepoints, stepping tracepoints test at the end
     of their while-stepping loop.  */
  if (tpoint->pass_count > 0
      && tpoint->hit_count >= tpoint->pass_count
      && tpoint->step_count == 0
      && stopping_tracepoint == NULL)
    stopping_tracepoint = tpoint;

  trace_debug ("Making new traceframe for tracepoint %d at 0x%s, hit %ld",
	       tpoint->number, paddress (tpoint->address), tpoint->hit_count);

  tframe = add_traceframe (tpoint);

  if (tframe)
    {
      for (acti = 0; acti < tpoint->numactions; ++acti)
	{
#ifndef IN_PROCESS_AGENT
	  trace_debug ("Tracepoint %d at 0x%s about to do action '%s'",
		       tpoint->number, paddress (tpoint->address),
		       tpoint->actions_str[acti]);
#endif

	  do_action_at_tracepoint (ctx, stop_pc, tpoint, tframe,
				   tpoint->actions[acti]);
	}

      finish_traceframe (tframe);
    }

  if (tframe == NULL && tracing)
    trace_buffer_is_full = 1;
}

#ifndef IN_PROCESS_AGENT

static void
collect_data_at_step (struct tracepoint_hit_ctx *ctx,
		      CORE_ADDR stop_pc,
		      struct tracepoint *tpoint, int current_step)
{
  struct traceframe *tframe;
  int acti;

  trace_debug ("Making new step traceframe for "
	       "tracepoint %d at 0x%s, step %d of %ld, hit %ld",
	       tpoint->number, paddress (tpoint->address),
	       current_step, tpoint->step_count,
	       tpoint->hit_count);

  tframe = add_traceframe (tpoint);

  if (tframe)
    {
      for (acti = 0; acti < tpoint->num_step_actions; ++acti)
	{
	  trace_debug ("Tracepoint %d at 0x%s about to do step action '%s'",
		       tpoint->number, paddress (tpoint->address),
		       tpoint->step_actions_str[acti]);

	  do_action_at_tracepoint (ctx, stop_pc, tpoint, tframe,
				   tpoint->step_actions[acti]);
	}

      finish_traceframe (tframe);
    }

  if (tframe == NULL && tracing)
    trace_buffer_is_full = 1;
}

#endif

static struct regcache *
get_context_regcache (struct tracepoint_hit_ctx *ctx)
{
  struct regcache *regcache = NULL;

#ifdef IN_PROCESS_AGENT
  if (ctx->type == fast_tracepoint)
    {
      struct fast_tracepoint_ctx *fctx = (struct fast_tracepoint_ctx *) ctx;
      if (!fctx->regcache_initted)
	{
	  fctx->regcache_initted = 1;
	  init_register_cache (&fctx->regcache, fctx->regspace);
	  supply_regblock (&fctx->regcache, NULL);
	  supply_fast_tracepoint_registers (&fctx->regcache, fctx->regs);
	}
      regcache = &fctx->regcache;
    }
#ifdef HAVE_UST
  if (ctx->type == static_tracepoint)
    {
      struct static_tracepoint_ctx *sctx
	= (struct static_tracepoint_ctx *) ctx;

      if (!sctx->regcache_initted)
	{
	  sctx->regcache_initted = 1;
	  init_register_cache (&sctx->regcache, sctx->regspace);
	  supply_regblock (&sctx->regcache, NULL);
	  /* Pass down the tracepoint address, because REGS doesn't
	     include the PC, but we know what it must have been.  */
	  supply_static_tracepoint_registers (&sctx->regcache,
					      (const unsigned char *)
					      sctx->regs,
					      sctx->tpoint->address);
	}
      regcache = &sctx->regcache;
    }
#endif
#else
  if (ctx->type == trap_tracepoint)
    {
      struct trap_tracepoint_ctx *tctx = (struct trap_tracepoint_ctx *) ctx;
      regcache = tctx->regcache;
    }
#endif

  gdb_assert (regcache != NULL);

  return regcache;
}

static void
do_action_at_tracepoint (struct tracepoint_hit_ctx *ctx,
			 CORE_ADDR stop_pc,
			 struct tracepoint *tpoint,
			 struct traceframe *tframe,
			 struct tracepoint_action *taction)
{
  enum eval_result_type err;

  switch (taction->type)
    {
    case 'M':
      {
	struct collect_memory_action *maction;

	maction = (struct collect_memory_action *) taction;

	trace_debug ("Want to collect %s bytes at 0x%s (basereg %d)",
		     pulongest (maction->len),
		     paddress (maction->addr), maction->basereg);
	/* (should use basereg) */
	agent_mem_read (tframe, NULL,
			(CORE_ADDR) maction->addr, maction->len);
	break;
      }
    case 'R':
      {
	unsigned char *regspace;
	struct regcache tregcache;
	struct regcache *context_regcache;


	trace_debug ("Want to collect registers");

	/* Collect all registers for now.  */
	regspace = add_traceframe_block (tframe,
					 1 + register_cache_size ());
	if (regspace == NULL)
	  {
	    trace_debug ("Trace buffer block allocation failed, skipping");
	    break;
	  }
	/* Identify a register block.  */
	*regspace = 'R';

	context_regcache = get_context_regcache (ctx);

	/* Wrap the regblock in a register cache (in the stack, we
	   don't want to malloc here).  */
	init_register_cache (&tregcache, regspace + 1);

	/* Copy the register data to the regblock.  */
	regcache_cpy (&tregcache, context_regcache);

#ifndef IN_PROCESS_AGENT
	/* On some platforms, trap-based tracepoints will have the PC
	   pointing to the next instruction after the trap, but we
	   don't want the user or GDB trying to guess whether the
	   saved PC needs adjusting; so always record the adjusted
	   stop_pc.  Note that we can't use tpoint->address instead,
	   since it will be wrong for while-stepping actions.  This
	   adjustment is a nop for fast tracepoints collected from the
	   in-process lib (but not if GDBserver is collecting one
	   preemptively), since the PC had already been adjusted to
	   contain the tracepoint's address by the jump pad.  */
	trace_debug ("Storing stop pc (0x%s) in regblock",
		     paddress (tpoint->address));

	/* This changes the regblock, not the thread's
	   regcache.  */
	regcache_write_pc (&tregcache, stop_pc);
#endif
      }
      break;
    case 'X':
      {
	struct eval_expr_action *eaction;

	eaction = (struct eval_expr_action *) taction;

	trace_debug ("Want to evaluate expression");

	err = eval_agent_expr (ctx, tframe, eaction->expr, NULL);

	if (err != expr_eval_no_error)
	  {
	    record_tracepoint_error (tpoint, "action expression", err);
	    return;
	  }
      }
      break;
    case 'L':
      {
#if defined IN_PROCESS_AGENT && defined HAVE_UST
	trace_debug ("Want to collect static trace data");
	collect_ust_data_at_tracepoint (ctx, stop_pc,
					tpoint, tframe);
#else
	trace_debug ("warning: collecting static trace data, "
		     "but static tracepoints are not supported");
#endif
      }
      break;
    default:
      trace_debug ("unknown trace action '%c', ignoring", taction->type);
      break;
    }
}

static int
condition_true_at_tracepoint (struct tracepoint_hit_ctx *ctx,
			      struct tracepoint *tpoint)
{
  ULONGEST value = 0;
  enum eval_result_type err;

  /* Presently, gdbserver doesn't run compiled conditions, only the
     IPA does.  If the program stops at a fast tracepoint's address
     (e.g., due to a breakpoint, trap tracepoint, or stepping),
     gdbserver preemptively collect the fast tracepoint.  Later, on
     resume, gdbserver steps over the fast tracepoint like it steps
     over breakpoints, so that the IPA doesn't see that fast
     tracepoint.  This avoids double collects of fast tracepoints in
     that stopping scenario.  Having gdbserver itself handle the fast
     tracepoint gives the user a consistent view of when fast or trap
     tracepoints are collected, compared to an alternative where only
     trap tracepoints are collected on stop, and fast tracepoints on
     resume.  When a fast tracepoint is being processed by gdbserver,
     it is always the non-compiled condition expression that is
     used.  */
#ifdef IN_PROCESS_AGENT
  if (tpoint->compiled_cond)
    err = ((condfn) (uintptr_t) (tpoint->compiled_cond)) (ctx, &value);
  else
#endif
    err = eval_agent_expr (ctx, NULL, tpoint->cond, &value);

  if (err != expr_eval_no_error)
    {
      record_tracepoint_error (tpoint, "condition", err);
      /* The error case must return false.  */
      return 0;
    }

  trace_debug ("Tracepoint %d at 0x%s condition evals to %s",
	       tpoint->number, paddress (tpoint->address),
	       pulongest (value));
  return (value ? 1 : 0);
}

#ifndef IN_PROCESS_AGENT

/* The packet form of an agent expression consists of an 'X', number
   of bytes in expression, a comma, and then the bytes.  */

static struct agent_expr *
parse_agent_expr (char **actparm)
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

static char *
unparse_agent_expr (struct agent_expr *aexpr)
{
  char *rslt;

  rslt = xmalloc (2 * aexpr->length + 1);
  convert_int_to_ascii (aexpr->bytes, rslt, aexpr->length);
  return rslt;
}

#endif

/* The agent expression evaluator, as specified by the GDB docs. It
   returns 0 if everything went OK, and a nonzero error code
   otherwise.  */

static enum eval_result_type
eval_agent_expr (struct tracepoint_hit_ctx *ctx,
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
      trace_debug ("empty agent expression");
      return expr_eval_empty_expression;
    }

  /* Cache the stack top in its own variable. Much of the time we can
     operate on this variable, rather than dinking with the stack. It
     needs to be copied to the stack when sp changes.  */
  top = 0;

  while (1)
    {
      op = aexpr->bytes[pc++];

      trace_debug ("About to interpret byte 0x%x", op);

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
	      trace_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = ((LONGEST) stack[--sp]) / ((LONGEST) top);
	  break;

	case gdb_agent_op_div_unsigned:
	  if (top == 0)
	    {
	      trace_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = stack[--sp] / top;
	  break;

	case gdb_agent_op_rem_signed:
	  if (top == 0)
	    {
	      trace_debug ("Attempted to divide by zero");
	      return expr_eval_divide_by_zero;
	    }
	  top = ((LONGEST) stack[--sp]) % ((LONGEST) top);
	  break;

	case gdb_agent_op_rem_unsigned:
	  if (top == 0)
	    {
	      trace_debug ("Attempted to divide by zero");
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
	    struct regcache *regcache;

	    regcache = get_context_regcache (ctx);

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
	  trace_debug ("At end of expression, sp=%d, stack top cache=0x%s",
		       sp, pulongest (top));
	  if (rslt)
	    {
	      if (sp <= 0)
		{
		  /* This should be an error */
		  trace_debug ("Stack is empty, nothing to return");
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
	  top = get_trace_state_variable_value (arg);
	  break;

	case gdb_agent_op_setv:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  set_trace_state_variable_value (arg, top);
	  /* Note that we leave the value on the stack, for the
	     benefit of later/enclosing expressions.  */
	  break;

	case gdb_agent_op_tracev:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  agent_tsv_read (tframe, arg);
	  break;

	  /* GDB never (currently) generates any of these ops.  */
	case gdb_agent_op_float:
	case gdb_agent_op_ref_float:
	case gdb_agent_op_ref_double:
	case gdb_agent_op_ref_long_double:
	case gdb_agent_op_l_to_d:
	case gdb_agent_op_d_to_l:
	case gdb_agent_op_trace16:
	  trace_debug ("Agent expression op 0x%x valid, but not handled",
		       op);
	  /* If ever GDB generates any of these, we don't have the
	     option of ignoring.  */
	  return 1;

	default:
	  trace_debug ("Agent expression op 0x%x not recognized", op);
	  /* Don't struggle on, things will just get worse.  */
	  return expr_eval_unrecognized_opcode;
	}

      /* Check for stack badness.  */
      if (sp >= (STACK_MAX - 1))
	{
	  trace_debug ("Expression stack overflow");
	  return expr_eval_stack_overflow;
	}

      if (sp < 0)
	{
	  trace_debug ("Expression stack underflow");
	  return expr_eval_stack_underflow;
	}

      trace_debug ("Op %s -> sp=%d, top=0x%s",
		   gdb_agent_op_names[op], sp, pulongest (top));
    }
}

/* Do memory copies for bytecodes.  */
/* Do the recording of memory blocks for actions and bytecodes.  */

static int
agent_mem_read (struct traceframe *tframe,
		unsigned char *to, CORE_ADDR from, ULONGEST len)
{
  unsigned char *mspace;
  ULONGEST remaining = len;
  unsigned short blocklen;

  /* If a 'to' buffer is specified, use it.  */
  if (to != NULL)
    {
      read_inferior_memory (from, to, len);
      return 0;
    }

  /* Otherwise, create a new memory block in the trace buffer.  */
  while (remaining > 0)
    {
      size_t sp;

      blocklen = (remaining > 65535 ? 65535 : remaining);
      sp = 1 + sizeof (from) + sizeof (blocklen) + blocklen;
      mspace = add_traceframe_block (tframe, sp);
      if (mspace == NULL)
	return 1;
      /* Identify block as a memory block.  */
      *mspace = 'M';
      ++mspace;
      /* Record address and size.  */
      memcpy (mspace, &from, sizeof (from));
      mspace += sizeof (from);
      memcpy (mspace, &blocklen, sizeof (blocklen));
      mspace += sizeof (blocklen);
      /* Record the memory block proper.  */
      read_inferior_memory (from, mspace, blocklen);
      trace_debug ("%d bytes recorded", blocklen);
      remaining -= blocklen;
      from += blocklen;
    }
  return 0;
}

/* Record the value of a trace state variable.  */

static int
agent_tsv_read (struct traceframe *tframe, int n)
{
  unsigned char *vspace;
  LONGEST val;

  vspace = add_traceframe_block (tframe,
				 1 + sizeof (n) + sizeof (LONGEST));
  if (vspace == NULL)
    return 1;
  /* Identify block as a variable.  */
  *vspace = 'V';
  /* Record variable's number and value.  */
  memcpy (vspace + 1, &n, sizeof (n));
  val = get_trace_state_variable_value (n);
  memcpy (vspace + 1 + sizeof (n), &val, sizeof (val));
  trace_debug ("Variable %d recorded", n);
  return 0;
}

#ifndef IN_PROCESS_AGENT

static unsigned char *
traceframe_find_block_type (unsigned char *database, unsigned int datasize,
			    int tfnum, char type_wanted)
{
  unsigned char *dataptr;

  if (datasize == 0)
    {
      trace_debug ("traceframe %d has no data", tfnum);
      return NULL;
    }

  /* Iterate through a traceframe's blocks, looking for a block of the
     requested type.  */
  for (dataptr = database;
       dataptr < database + datasize;
       /* nothing */)
    {
      char blocktype;
      unsigned short mlen;

      if (dataptr == trace_buffer_wrap)
	{
	  /* Adjust to reflect wrapping part of the frame around to
	     the beginning.  */
	  datasize = dataptr - database;
	  dataptr = database = trace_buffer_lo;
	}
      blocktype = *dataptr++;

      if (type_wanted == blocktype)
	return dataptr;

      switch (blocktype)
	{
	case 'R':
	  /* Skip over the registers block.  */
	  dataptr += register_cache_size ();
	  break;
	case 'M':
	  /* Skip over the memory block.  */
	  dataptr += sizeof (CORE_ADDR);
	  memcpy (&mlen, dataptr, sizeof (mlen));
	  dataptr += (sizeof (mlen) + mlen);
	  break;
	case 'V':
	  /* Skip over the TSV block.  */
	  dataptr += (sizeof (int) + sizeof (LONGEST));
	  break;
	case 'S':
	  /* Skip over the static trace data block.  */
	  memcpy (&mlen, dataptr, sizeof (mlen));
	  dataptr += (sizeof (mlen) + mlen);
	  break;
	default:
	  trace_debug ("traceframe %d has unknown block type 0x%x",
		       tfnum, blocktype);
	  return NULL;
	}
    }

  return NULL;
}

static unsigned char *
traceframe_find_regblock (struct traceframe *tframe, int tfnum)
{
  unsigned char *regblock;

  regblock = traceframe_find_block_type (tframe->data,
					 tframe->data_size,
					 tfnum, 'R');

  if (regblock == NULL)
    trace_debug ("traceframe %d has no register data", tfnum);

  return regblock;
}

/* Get registers from a traceframe.  */

int
fetch_traceframe_registers (int tfnum, struct regcache *regcache, int regnum)
{
  unsigned char *dataptr;
  struct tracepoint *tpoint;
  struct traceframe *tframe;

  tframe = find_traceframe (tfnum);

  if (tframe == NULL)
    {
      trace_debug ("traceframe %d not found", tfnum);
      return 1;
    }

  dataptr = traceframe_find_regblock (tframe, tfnum);
  if (dataptr == NULL)
    {
      /* Mark registers unavailable.  */
      supply_regblock (regcache, NULL);

      /* We can generally guess at a PC, although this will be
	 misleading for while-stepping frames and multi-location
	 tracepoints.  */
      tpoint = find_next_tracepoint_by_number (NULL, tframe->tpnum);
      if (tpoint != NULL)
	regcache_write_pc (regcache, tpoint->address);
    }
  else
    supply_regblock (regcache, dataptr);

  return 0;
}

static CORE_ADDR
traceframe_get_pc (struct traceframe *tframe)
{
  struct regcache regcache;
  unsigned char *dataptr;

  dataptr = traceframe_find_regblock (tframe, -1);
  if (dataptr == NULL)
    return 0;

  init_register_cache (&regcache, dataptr);
  return regcache_read_pc (&regcache);
}

/* Read a requested block of memory from a trace frame.  */

int
traceframe_read_mem (int tfnum, CORE_ADDR addr,
		     unsigned char *buf, ULONGEST length,
		     ULONGEST *nbytes)
{
  struct traceframe *tframe;
  unsigned char *database, *dataptr;
  unsigned int datasize;
  CORE_ADDR maddr;
  unsigned short mlen;

  trace_debug ("traceframe_read_mem");

  tframe = find_traceframe (tfnum);

  if (!tframe)
    {
      trace_debug ("traceframe %d not found", tfnum);
      return 1;
    }

  datasize = tframe->data_size;
  database = dataptr = &tframe->data[0];

  /* Iterate through a traceframe's blocks, looking for memory.  */
  while ((dataptr = traceframe_find_block_type (dataptr,
						datasize
						- (dataptr - database),
						tfnum, 'M')) != NULL)
    {
      memcpy (&maddr, dataptr, sizeof (maddr));
      dataptr += sizeof (maddr);
      memcpy (&mlen, dataptr, sizeof (mlen));
      dataptr += sizeof (mlen);
      trace_debug ("traceframe %d has %d bytes at %s",
		   tfnum, mlen, paddress (maddr));

      /* Check that requested data is in bounds.  */
      if (maddr <= addr && (addr + length) <= (maddr + mlen))
	{
	  /* Block includes the requested range, copy it out.  */
	  memcpy (buf, dataptr + (addr - maddr), length);
	  *nbytes = length;
	  return 0;
	}

      /* Skip over this block.  */
      dataptr += mlen;
    }

  trace_debug ("traceframe %d has no memory data for the desired region",
	       tfnum);

  *nbytes = 0;
  return 0;
}

static int
traceframe_read_tsv (int tsvnum, LONGEST *val)
{
  int tfnum;
  struct traceframe *tframe;
  unsigned char *database, *dataptr;
  unsigned int datasize;
  int vnum;

  trace_debug ("traceframe_read_tsv");

  tfnum = current_traceframe;

  if (tfnum < 0)
    {
      trace_debug ("no current traceframe");
      return 1;
    }

  tframe = find_traceframe (tfnum);

  if (tframe == NULL)
    {
      trace_debug ("traceframe %d not found", tfnum);
      return 1;
    }

  datasize = tframe->data_size;
  database = dataptr = &tframe->data[0];

  /* Iterate through a traceframe's blocks, looking for the tsv.  */
  while ((dataptr = traceframe_find_block_type (dataptr,
						datasize
						- (dataptr - database),
						tfnum, 'V')) != NULL)
    {
      memcpy (&vnum, dataptr, sizeof (vnum));
      dataptr += sizeof (vnum);

      trace_debug ("traceframe %d has variable %d", tfnum, vnum);

      /* Check that this is the variable we want.  */
      if (tsvnum == vnum)
	{
	  memcpy (val, dataptr, sizeof (*val));
	  return 0;
	}

      /* Skip over this block.  */
      dataptr += sizeof (LONGEST);
    }

  trace_debug ("traceframe %d has no data for variable %d",
	       tfnum, tsvnum);
  return 1;
}

/* Read a requested block of static tracepoint data from a trace
   frame.  */

int
traceframe_read_sdata (int tfnum, ULONGEST offset,
		       unsigned char *buf, ULONGEST length,
		       ULONGEST *nbytes)
{
  struct traceframe *tframe;
  unsigned char *database, *dataptr;
  unsigned int datasize;
  unsigned short mlen;

  trace_debug ("traceframe_read_sdata");

  tframe = find_traceframe (tfnum);

  if (!tframe)
    {
      trace_debug ("traceframe %d not found", tfnum);
      return 1;
    }

  datasize = tframe->data_size;
  database = &tframe->data[0];

  /* Iterate through a traceframe's blocks, looking for static
     tracepoint data.  */
  dataptr = traceframe_find_block_type (database, datasize,
					tfnum, 'S');
  if (dataptr != NULL)
    {
      memcpy (&mlen, dataptr, sizeof (mlen));
      dataptr += sizeof (mlen);
      if (offset < mlen)
	{
	  if (offset + length > mlen)
	    length = mlen - offset;

	  memcpy (buf, dataptr, length);
	  *nbytes = length;
	}
      else
	*nbytes = 0;
      return 0;
    }

  trace_debug ("traceframe %d has no static trace data", tfnum);

  *nbytes = 0;
  return 0;
}

/* Return the first fast tracepoint whose jump pad contains PC.  */

static struct tracepoint *
fast_tracepoint_from_jump_pad_address (CORE_ADDR pc)
{
  struct tracepoint *tpoint;

  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    if (tpoint->type == fast_tracepoint)
      if (tpoint->jump_pad <= pc && pc < tpoint->jump_pad_end)
	return tpoint;

  return NULL;
}

/* Return GDBserver's tracepoint that matches the IP Agent's
   tracepoint object that lives at IPA_TPOINT_OBJ in the IP Agent's
   address space.  */

static struct tracepoint *
fast_tracepoint_from_ipa_tpoint_address (CORE_ADDR ipa_tpoint_obj)
{
  struct tracepoint *tpoint;

  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    if (tpoint->type == fast_tracepoint)
      if (tpoint->obj_addr_on_target == ipa_tpoint_obj)
	return tpoint;

  return NULL;
}

#endif

/* The type of the object that is used to synchronize fast tracepoint
   collection.  */

typedef struct collecting_t
{
  /* The fast tracepoint number currently collecting.  */
  uintptr_t tpoint;

  /* A number that GDBserver can use to identify the thread that is
     presently holding the collect lock.  This need not (and usually
     is not) the thread id, as getting the current thread ID usually
     requires a system call, which we want to avoid like the plague.
     Usually this is thread's TCB, found in the TLS (pseudo-)
     register, which is readable with a single insn on several
     architectures.  */
  uintptr_t thread_area;
} collecting_t;

#ifndef IN_PROCESS_AGENT

void
force_unlock_trace_buffer (void)
{
  write_inferior_data_pointer (ipa_sym_addrs.addr_collecting, 0);
}

/* Check if the thread identified by THREAD_AREA which is stopped at
   STOP_PC, is presently locking the fast tracepoint collection, and
   if so, gather some status of said collection.  Returns 0 if the
   thread isn't collecting or in the jump pad at all.  1, if in the
   jump pad (or within gdb_collect) and hasn't executed the adjusted
   original insn yet (can set a breakpoint there and run to it).  2,
   if presently executing the adjusted original insn --- in which
   case, if we want to move the thread out of the jump pad, we need to
   single-step it until this function returns 0.  */

int
fast_tracepoint_collecting (CORE_ADDR thread_area,
			    CORE_ADDR stop_pc,
			    struct fast_tpoint_collect_status *status)
{
  CORE_ADDR ipa_collecting;
  CORE_ADDR ipa_gdb_jump_pad_buffer, ipa_gdb_jump_pad_buffer_end;
  struct tracepoint *tpoint;
  int needs_breakpoint;

  /* The thread THREAD_AREA is either:

      0. not collecting at all, not within the jump pad, or within
	 gdb_collect or one of its callees.

      1. in the jump pad and haven't reached gdb_collect

      2. within gdb_collect (out of the jump pad) (collect is set)

      3. we're in the jump pad, after gdb_collect having returned,
	 possibly executing the adjusted insns.

      For cases 1 and 3, `collecting' may or not be set.  The jump pad
      doesn't have any complicated jump logic, so we can tell if the
      thread is executing the adjust original insn or not by just
      matching STOP_PC with known jump pad addresses.  If we it isn't
      yet executing the original insn, set a breakpoint there, and let
      the thread run to it, so to quickly step over a possible (many
      insns) gdb_collect call.  Otherwise, or when the breakpoint is
      hit, only a few (small number of) insns are left to be executed
      in the jump pad.  Single-step the thread until it leaves the
      jump pad.  */

 again:
  tpoint = NULL;
  needs_breakpoint = 0;
  trace_debug ("fast_tracepoint_collecting");

  if (read_inferior_data_pointer (ipa_sym_addrs.addr_gdb_jump_pad_buffer,
				  &ipa_gdb_jump_pad_buffer))
    fatal ("error extracting `gdb_jump_pad_buffer'");
  if (read_inferior_data_pointer (ipa_sym_addrs.addr_gdb_jump_pad_buffer_end,
				  &ipa_gdb_jump_pad_buffer_end))
    fatal ("error extracting `gdb_jump_pad_buffer_end'");

  if (ipa_gdb_jump_pad_buffer <= stop_pc
      && stop_pc < ipa_gdb_jump_pad_buffer_end)
    {
      /* We can tell which tracepoint(s) the thread is collecting by
	 matching the jump pad address back to the tracepoint.  */
      tpoint = fast_tracepoint_from_jump_pad_address (stop_pc);
      if (tpoint == NULL)
	{
	  warning ("in jump pad, but no matching tpoint?");
	  return 0;
	}
      else
	{
	  trace_debug ("in jump pad of tpoint (%d, %s); jump_pad(%s, %s); "
		       "adj_insn(%s, %s)",
		       tpoint->number, paddress (tpoint->address),
		       paddress (tpoint->jump_pad),
		       paddress (tpoint->jump_pad_end),
		       paddress (tpoint->adjusted_insn_addr),
		       paddress (tpoint->adjusted_insn_addr_end));
	}

      /* Definitely in the jump pad.  May or may not need
	 fast-exit-jump-pad breakpoint.  */
      if (tpoint->jump_pad <= stop_pc
	  && stop_pc < tpoint->adjusted_insn_addr)
	needs_breakpoint =  1;
    }
  else
    {
      collecting_t ipa_collecting_obj;

      /* If `collecting' is set/locked, then the THREAD_AREA thread
	 may or not be the one holding the lock.  We have to read the
	 lock to find out.  */

      if (read_inferior_data_pointer (ipa_sym_addrs.addr_collecting,
				      &ipa_collecting))
	{
	  trace_debug ("fast_tracepoint_collecting:"
		       " failed reading 'collecting' in the inferior");
	  return 0;
	}

      if (!ipa_collecting)
	{
	  trace_debug ("fast_tracepoint_collecting: not collecting"
		       " (and nobody is).");
	  return 0;
	}

      /* Some thread is collecting.  Check which.  */
      if (read_inferior_memory (ipa_collecting,
				(unsigned char *) &ipa_collecting_obj,
				sizeof (ipa_collecting_obj)) != 0)
	goto again;

      if (ipa_collecting_obj.thread_area != thread_area)
	{
	  trace_debug ("fast_tracepoint_collecting: not collecting "
		       "(another thread is)");
	  return 0;
	}

      tpoint
	= fast_tracepoint_from_ipa_tpoint_address (ipa_collecting_obj.tpoint);
      if (tpoint == NULL)
	{
	  warning ("fast_tracepoint_collecting: collecting, "
		   "but tpoint %s not found?",
		   paddress ((CORE_ADDR) ipa_collecting_obj.tpoint));
	  return 0;
	}

      /* The thread is within `gdb_collect', skip over the rest of
	 fast tracepoint collection quickly using a breakpoint.  */
      needs_breakpoint = 1;
    }

  /* The caller wants a bit of status detail.  */
  if (status != NULL)
    {
      status->tpoint_num = tpoint->number;
      status->tpoint_addr = tpoint->address;
      status->adjusted_insn_addr = tpoint->adjusted_insn_addr;
      status->adjusted_insn_addr_end = tpoint->adjusted_insn_addr_end;
    }

  if (needs_breakpoint)
    {
      /* Hasn't executed the original instruction yet.  Set breakpoint
	 there, and wait till it's hit, then single-step until exiting
	 the jump pad.  */

      trace_debug ("\
fast_tracepoint_collecting, returning continue-until-break at %s",
		   paddress (tpoint->adjusted_insn_addr));

      return 1; /* continue */
    }
  else
    {
      /* Just single-step until exiting the jump pad.  */

      trace_debug ("fast_tracepoint_collecting, returning "
		   "need-single-step (%s-%s)",
		   paddress (tpoint->adjusted_insn_addr),
		   paddress (tpoint->adjusted_insn_addr_end));

      return 2; /* single-step */
    }
}

#endif

#ifdef IN_PROCESS_AGENT

/* The global fast tracepoint collect lock.  Points to a collecting_t
   object built on the stack by the jump pad, if presently locked;
   NULL if it isn't locked.  Note that this lock *must* be set while
   executing any *function other than the jump pad.  See
   fast_tracepoint_collecting.  */
static collecting_t * ATTR_USED collecting;

/* This routine, called from the jump pad (in asm) is designed to be
   called from the jump pads of fast tracepoints, thus it is on the
   critical path.  */

IP_AGENT_EXPORT void ATTR_USED
gdb_collect (struct tracepoint *tpoint, unsigned char *regs)
{
  struct fast_tracepoint_ctx ctx;

  /* Don't do anything until the trace run is completely set up.  */
  if (!tracing)
    return;

  ctx.base.type = fast_tracepoint;
  ctx.regs = regs;
  ctx.regcache_initted = 0;
  ctx.tpoint = tpoint;

  /* Wrap the regblock in a register cache (in the stack, we don't
     want to malloc here).  */
  ctx.regspace = alloca (register_cache_size ());
  if (ctx.regspace == NULL)
    {
      trace_debug ("Trace buffer block allocation failed, skipping");
      return;
    }

  /* Test the condition if present, and collect if true.  */
  if (tpoint->cond == NULL
      || condition_true_at_tracepoint ((struct tracepoint_hit_ctx *) &ctx,
				       tpoint))
    {
      collect_data_at_tracepoint ((struct tracepoint_hit_ctx *) &ctx,
				  tpoint->address, tpoint);

      /* Note that this will cause original insns to be written back
	 to where we jumped from, but that's OK because we're jumping
	 back to the next whole instruction.  This will go badly if
	 instruction restoration is not atomic though.  */
      if (stopping_tracepoint
	  || trace_buffer_is_full
	  || expr_eval_result != expr_eval_no_error)
	stop_tracing ();
    }
  else
    {
      /* If there was a condition and it evaluated to false, the only
	 way we would stop tracing is if there was an error during
	 condition expression evaluation.  */
      if (expr_eval_result != expr_eval_no_error)
	stop_tracing ();
    }
}

#endif

#ifndef IN_PROCESS_AGENT

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

CORE_ADDR
get_raw_reg_func_addr (void)
{
  return ipa_sym_addrs.addr_get_raw_reg;
}

static void
emit_prologue (void)
{
  target_emit_ops ()->emit_prologue ();
}

static void
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

static enum eval_result_type compile_bytecodes (struct agent_expr *aexpr);

static void
compile_tracepoint_condition (struct tracepoint *tpoint,
			      CORE_ADDR *jump_entry)
{
  CORE_ADDR entry_point = *jump_entry;
  enum eval_result_type err;

  trace_debug ("Starting condition compilation for tracepoint %d\n",
	       tpoint->number);

  /* Initialize the global pointer to the code being built.  */
  current_insn_ptr = *jump_entry;

  emit_prologue ();

  err = compile_bytecodes (tpoint->cond);

  if (err == expr_eval_no_error)
    {
      emit_epilogue ();

      /* Record the beginning of the compiled code.  */
      tpoint->compiled_cond = entry_point;

      trace_debug ("Condition compilation for tracepoint %d complete\n",
		   tpoint->number);
    }
  else
    {
      /* Leave the unfinished code in situ, but don't point to it.  */

      tpoint->compiled_cond = 0;

      trace_debug ("Condition compilation for tracepoint %d failed, "
		   "error code %d",
		   tpoint->number, err);
    }

  /* Update the code pointer passed in.  Note that we do this even if
     the compile fails, so that we can look at the partial results
     instead of letting them be overwritten.  */
  *jump_entry = current_insn_ptr;

  /* Leave a gap, to aid dump decipherment.  */
  *jump_entry += 16;
}

/* Given an agent expression, turn it into native code.  */

static enum eval_result_type
compile_bytecodes (struct agent_expr *aexpr)
{
  int pc = 0;
  int done = 0;
  unsigned char op;
  int arg;
  /* This is only used to build 64-bit value for constants.  */
  ULONGEST top;
  struct bytecode_address *aentry, *aentry2;

#define UNHANDLED					\
  do							\
    {							\
      trace_debug ("Cannot compile op 0x%x\n", op);	\
      return expr_eval_unhandled_opcode;		\
    } while (0)

  if (aexpr->length == 0)
    {
      trace_debug ("empty agent expression\n");
      return expr_eval_empty_expression;
    }

  bytecode_address_table = NULL;

  while (!done)
    {
      op = aexpr->bytes[pc];

      trace_debug ("About to compile op 0x%x, pc=%d\n", op, pc);

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
	  emit_equal ();
	  break;

	case gdb_agent_op_less_signed:
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
	  trace_debug ("At end of expression\n");

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
	  emit_swap ();
	  break;

	case gdb_agent_op_getv:
	  emit_stack_flush ();
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  emit_int_call_1 (ipa_sym_addrs.addr_get_trace_state_variable_value,
			   arg);
	  break;

	case gdb_agent_op_setv:
	  arg = aexpr->bytes[pc++];
	  arg = (arg << 8) + aexpr->bytes[pc++];
	  emit_void_call_2 (ipa_sym_addrs.addr_set_trace_state_variable_value,
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
	  trace_debug ("Agent expression op 0x%x not recognized\n", op);
	  /* Don't struggle on, things will just get worse.  */
	  return expr_eval_unrecognized_opcode;
	}

      /* This catches errors that occur in target-specific code
	 emission.  */
      if (emit_error)
	{
	  trace_debug ("Error %d while emitting code for %s\n",
		       emit_error, gdb_agent_op_names[op]);
	  return expr_eval_unhandled_opcode;
	}

      trace_debug ("Op %s compiled\n", gdb_agent_op_names[op]);
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
	      trace_debug ("Want to jump from %s to %s\n",
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
	  trace_debug ("Destination of goto %d not found\n",
		       aentry->goto_pc);
	  return expr_eval_invalid_goto;
	}
    }

  return expr_eval_no_error;
}

/* We'll need to adjust these when we consider bi-arch setups, and big
   endian machines.  */

static int
write_inferior_data_ptr (CORE_ADDR where, CORE_ADDR ptr)
{
  return write_inferior_memory (where,
				(unsigned char *) &ptr, sizeof (void *));
}

/* The base pointer of the IPA's heap.  This is the only memory the
   IPA is allowed to use.  The IPA should _not_ call the inferior's
   `malloc' during operation.  That'd be slow, and, most importantly,
   it may not be safe.  We may be collecting a tracepoint in a signal
   handler, for example.  */
static CORE_ADDR target_tp_heap;

/* Allocate at least SIZE bytes of memory from the IPA heap, aligned
   to 8 bytes.  */

static CORE_ADDR
target_malloc (ULONGEST size)
{
  CORE_ADDR ptr;

  if (target_tp_heap == 0)
    {
      /* We have the pointer *address*, need what it points to.  */
      if (read_inferior_data_pointer (ipa_sym_addrs.addr_gdb_tp_heap_buffer,
				      &target_tp_heap))
	fatal ("could get target heap head pointer");
    }

  ptr = target_tp_heap;
  target_tp_heap += size;

  /* Pad to 8-byte alignment.  */
  target_tp_heap = ((target_tp_heap + 7) & ~0x7);

  return ptr;
}

static CORE_ADDR
download_agent_expr (struct agent_expr *expr)
{
  CORE_ADDR expr_addr;
  CORE_ADDR expr_bytes;

  expr_addr = target_malloc (sizeof (*expr));
  write_inferior_memory (expr_addr, (unsigned char *) expr, sizeof (*expr));

  expr_bytes = target_malloc (expr->length);
  write_inferior_data_ptr (expr_addr + offsetof (struct agent_expr, bytes),
			   expr_bytes);
  write_inferior_memory (expr_bytes, expr->bytes, expr->length);

  return expr_addr;
}

/* Align V up to N bits.  */
#define UALIGN(V, N) (((V) + ((N) - 1)) & ~((N) - 1))

static void
download_tracepoints (void)
{
  CORE_ADDR tpptr = 0, prev_tpptr = 0;
  struct tracepoint *tpoint;

  /* Start out empty.  */
  write_inferior_data_ptr (ipa_sym_addrs.addr_tracepoints, 0);

  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    {
      struct tracepoint target_tracepoint;

      if (tpoint->type != fast_tracepoint
	  && tpoint->type != static_tracepoint)
	continue;

      /* Maybe download a compiled condition.  */
      if (tpoint->cond != NULL && target_emit_ops () != NULL)
	{
	  CORE_ADDR jentry, jump_entry;

	  jentry = jump_entry = get_jump_space_head ();

	  if (tpoint->cond != NULL)
	    {
	      /* Pad to 8-byte alignment. (needed?)  */
	      /* Actually this should be left for the target to
		 decide.  */
	      jentry = UALIGN (jentry, 8);

	      compile_tracepoint_condition (tpoint, &jentry);
	    }

	  /* Pad to 8-byte alignment.  */
	  jentry = UALIGN (jentry, 8);
	  claim_jump_space (jentry - jump_entry);
	}

      target_tracepoint = *tpoint;

      prev_tpptr = tpptr;
      tpptr = target_malloc (sizeof (*tpoint));
      tpoint->obj_addr_on_target = tpptr;

      if (tpoint == tracepoints)
	{
	  /* First object in list, set the head pointer in the
	     inferior.  */
	  write_inferior_data_ptr (ipa_sym_addrs.addr_tracepoints, tpptr);
	}
      else
	{
	  write_inferior_data_ptr (prev_tpptr + offsetof (struct tracepoint,
							  next),
				   tpptr);
	}

      /* Write the whole object.  We'll fix up its pointers in a bit.
	 Assume no next for now.  This is fixed up above on the next
	 iteration, if there's any.  */
      target_tracepoint.next = NULL;
      /* Need to clear this here too, since we're downloading the
	 tracepoints before clearing our own copy.  */
      target_tracepoint.hit_count = 0;

      write_inferior_memory (tpptr, (unsigned char *) &target_tracepoint,
			     sizeof (target_tracepoint));

      if (tpoint->cond)
	write_inferior_data_ptr (tpptr + offsetof (struct tracepoint,
						   cond),
				 download_agent_expr (tpoint->cond));

      if (tpoint->numactions)
	{
	  int i;
	  CORE_ADDR actions_array;

	  /* The pointers array.  */
	  actions_array
	    = target_malloc (sizeof (*tpoint->actions) * tpoint->numactions);
	  write_inferior_data_ptr (tpptr + offsetof (struct tracepoint,
						     actions),
				   actions_array);

	  /* Now for each pointer, download the action.  */
	  for (i = 0; i < tpoint->numactions; i++)
	    {
	      CORE_ADDR ipa_action = 0;
	      struct tracepoint_action *action = tpoint->actions[i];

	      switch (action->type)
		{
		case 'M':
		  ipa_action
		    = target_malloc (sizeof (struct collect_memory_action));
		  write_inferior_memory (ipa_action,
					 (unsigned char *) action,
					 sizeof (struct collect_memory_action));
		  break;
		case 'R':
		  ipa_action
		    = target_malloc (sizeof (struct collect_registers_action));
		  write_inferior_memory (ipa_action,
					 (unsigned char *) action,
					 sizeof (struct collect_registers_action));
		  break;
		case 'X':
		  {
		    CORE_ADDR expr;
		    struct eval_expr_action *eaction
		      = (struct eval_expr_action *) action;

		    ipa_action = target_malloc (sizeof (*eaction));
		    write_inferior_memory (ipa_action,
					   (unsigned char *) eaction,
					   sizeof (*eaction));

		    expr = download_agent_expr (eaction->expr);
		    write_inferior_data_ptr
		      (ipa_action + offsetof (struct eval_expr_action, expr),
		       expr);
		    break;
		  }
		case 'L':
		  ipa_action = target_malloc
		    (sizeof (struct collect_static_trace_data_action));
		  write_inferior_memory
		    (ipa_action,
		     (unsigned char *) action,
		     sizeof (struct collect_static_trace_data_action));
		  break;
		default:
		  trace_debug ("unknown trace action '%c', ignoring",
			       action->type);
		  break;
		}

	      if (ipa_action != 0)
		write_inferior_data_ptr
		  (actions_array + i * sizeof (sizeof (*tpoint->actions)),
		   ipa_action);
	    }
	}
    }
}

static void
download_trace_state_variables (void)
{
  CORE_ADDR ptr = 0, prev_ptr = 0;
  struct trace_state_variable *tsv;

  /* Start out empty.  */
  write_inferior_data_ptr (ipa_sym_addrs.addr_trace_state_variables, 0);

  for (tsv = trace_state_variables; tsv != NULL; tsv = tsv->next)
    {
      struct trace_state_variable target_tsv;

      /* TSV's with a getter have been initialized equally in both the
	 inferior and GDBserver.  Skip them.  */
      if (tsv->getter != NULL)
	continue;

      target_tsv = *tsv;

      prev_ptr = ptr;
      ptr = target_malloc (sizeof (*tsv));

      if (tsv == trace_state_variables)
	{
	  /* First object in list, set the head pointer in the
	     inferior.  */

	  write_inferior_data_ptr (ipa_sym_addrs.addr_trace_state_variables,
				   ptr);
	}
      else
	{
	  write_inferior_data_ptr (prev_ptr
				   + offsetof (struct trace_state_variable,
					       next),
				   ptr);
	}

      /* Write the whole object.  We'll fix up its pointers in a bit.
	 Assume no next, fixup when needed.  */
      target_tsv.next = NULL;

      write_inferior_memory (ptr, (unsigned char *) &target_tsv,
			     sizeof (target_tsv));

      if (tsv->name != NULL)
	{
	  size_t size = strlen (tsv->name) + 1;
	  CORE_ADDR name_addr = target_malloc (size);
	  write_inferior_memory (name_addr,
				 (unsigned char *) tsv->name, size);
	  write_inferior_data_ptr (ptr
				   + offsetof (struct trace_state_variable,
					       name),
				   name_addr);
	}

      if (tsv->getter != NULL)
	{
	  fatal ("what to do with these?");
	}
    }

  if (prev_ptr != 0)
    {
      /* Fixup the next pointer in the last item in the list.  */
      write_inferior_data_ptr (prev_ptr
			       + offsetof (struct trace_state_variable,
					   next), 0);
    }
}

/* Upload complete trace frames out of the IP Agent's trace buffer
   into GDBserver's trace buffer.  This always uploads either all or
   no trace frames.  This is the counter part of
   `trace_alloc_trace_buffer'.  See its description of the atomic
   synching mechanism.  */

static void
upload_fast_traceframes (void)
{
  unsigned int ipa_traceframe_read_count, ipa_traceframe_write_count;
  unsigned int ipa_traceframe_read_count_racy, ipa_traceframe_write_count_racy;
  CORE_ADDR tf;
  struct ipa_trace_buffer_control ipa_trace_buffer_ctrl;
  unsigned int curr_tbctrl_idx;
  unsigned int ipa_trace_buffer_ctrl_curr;
  unsigned int ipa_trace_buffer_ctrl_curr_old;
  CORE_ADDR ipa_trace_buffer_ctrl_addr;
  struct breakpoint *about_to_request_buffer_space_bkpt;
  CORE_ADDR ipa_trace_buffer_lo;
  CORE_ADDR ipa_trace_buffer_hi;

  if (read_inferior_uinteger (ipa_sym_addrs.addr_traceframe_read_count,
			      &ipa_traceframe_read_count_racy))
    {
      /* This will happen in most targets if the current thread is
	 running.  */
      return;
    }

  if (read_inferior_uinteger (ipa_sym_addrs.addr_traceframe_write_count,
			      &ipa_traceframe_write_count_racy))
    return;

  trace_debug ("ipa_traceframe_count (racy area): %d (w=%d, r=%d)",
	       ipa_traceframe_write_count_racy
	       - ipa_traceframe_read_count_racy,
	       ipa_traceframe_write_count_racy,
	       ipa_traceframe_read_count_racy);

  if (ipa_traceframe_write_count_racy == ipa_traceframe_read_count_racy)
    return;

  about_to_request_buffer_space_bkpt
    = set_breakpoint_at (ipa_sym_addrs.addr_about_to_request_buffer_space,
			 NULL);

  if (read_inferior_uinteger (ipa_sym_addrs.addr_trace_buffer_ctrl_curr,
			      &ipa_trace_buffer_ctrl_curr))
    return;

  ipa_trace_buffer_ctrl_curr_old = ipa_trace_buffer_ctrl_curr;

  curr_tbctrl_idx = ipa_trace_buffer_ctrl_curr & ~GDBSERVER_FLUSH_COUNT_MASK;

  {
    unsigned int prev, counter;

    /* Update the token, with new counters, and the GDBserver stamp
       bit.  Alway reuse the current TBC index.  */
    prev = ipa_trace_buffer_ctrl_curr & 0x0007ff00;
    counter = (prev + 0x100) & 0x0007ff00;

    ipa_trace_buffer_ctrl_curr = (0x80000000
				  | (prev << 12)
				  | counter
				  | curr_tbctrl_idx);
  }

  if (write_inferior_uinteger (ipa_sym_addrs.addr_trace_buffer_ctrl_curr,
			       ipa_trace_buffer_ctrl_curr))
    return;

  trace_debug ("Lib: Committed %08x -> %08x",
	       ipa_trace_buffer_ctrl_curr_old,
	       ipa_trace_buffer_ctrl_curr);

  /* Re-read these, now that we've installed the
     `about_to_request_buffer_space' breakpoint/lock.  A thread could
     have finished a traceframe between the last read of these
     counters and setting the breakpoint above.  If we start
     uploading, we never want to leave this function with
     traceframe_read_count != 0, otherwise, GDBserver could end up
     incrementing the counter tokens more than once (due to event loop
     nesting), which would break the IP agent's "effective" detection
     (see trace_alloc_trace_buffer).  */
  if (read_inferior_uinteger (ipa_sym_addrs.addr_traceframe_read_count,
			      &ipa_traceframe_read_count))
    return;
  if (read_inferior_uinteger (ipa_sym_addrs.addr_traceframe_write_count,
			      &ipa_traceframe_write_count))
    return;

  if (debug_threads)
    {
      trace_debug ("ipa_traceframe_count (blocked area): %d (w=%d, r=%d)",
		   ipa_traceframe_write_count - ipa_traceframe_read_count,
		   ipa_traceframe_write_count, ipa_traceframe_read_count);

      if (ipa_traceframe_write_count != ipa_traceframe_write_count_racy
	  || ipa_traceframe_read_count != ipa_traceframe_read_count_racy)
	trace_debug ("note that ipa_traceframe_count's parts changed");
    }

  /* Get the address of the current TBC object (the IP agent has an
     array of 3 such objects).  The index is stored in the TBC
     token.  */
  ipa_trace_buffer_ctrl_addr = ipa_sym_addrs.addr_trace_buffer_ctrl;
  ipa_trace_buffer_ctrl_addr
    += sizeof (struct ipa_trace_buffer_control) * curr_tbctrl_idx;

  if (read_inferior_memory (ipa_trace_buffer_ctrl_addr,
			    (unsigned char *) &ipa_trace_buffer_ctrl,
			    sizeof (struct ipa_trace_buffer_control)))
    return;

  if (read_inferior_data_pointer (ipa_sym_addrs.addr_trace_buffer_lo,
				  &ipa_trace_buffer_lo))
    return;
  if (read_inferior_data_pointer (ipa_sym_addrs.addr_trace_buffer_hi,
				  &ipa_trace_buffer_hi))
    return;

  /* Offsets are easier to grok for debugging than raw addresses,
     especially for the small trace buffer sizes that are useful for
     testing.  */
  trace_debug ("Lib: Trace buffer [%d] start=%d free=%d "
	       "endfree=%d wrap=%d hi=%d",
	       curr_tbctrl_idx,
	       (int) (ipa_trace_buffer_ctrl.start - ipa_trace_buffer_lo),
	       (int) (ipa_trace_buffer_ctrl.free - ipa_trace_buffer_lo),
	       (int) (ipa_trace_buffer_ctrl.end_free - ipa_trace_buffer_lo),
	       (int) (ipa_trace_buffer_ctrl.wrap - ipa_trace_buffer_lo),
	       (int) (ipa_trace_buffer_hi - ipa_trace_buffer_lo));

  /* Note that the IPA's buffer is always circular.  */

#define IPA_FIRST_TRACEFRAME() (ipa_trace_buffer_ctrl.start)

#define IPA_NEXT_TRACEFRAME_1(TF, TFOBJ)		\
  ((TF) + sizeof (struct traceframe) + (TFOBJ)->data_size)

#define IPA_NEXT_TRACEFRAME(TF, TFOBJ)					\
  (IPA_NEXT_TRACEFRAME_1 (TF, TFOBJ)					\
   - ((IPA_NEXT_TRACEFRAME_1 (TF, TFOBJ) >= ipa_trace_buffer_ctrl.wrap) \
      ? (ipa_trace_buffer_ctrl.wrap - ipa_trace_buffer_lo)		\
      : 0))

  tf = IPA_FIRST_TRACEFRAME ();

  while (ipa_traceframe_write_count - ipa_traceframe_read_count)
    {
      struct tracepoint *tpoint;
      struct traceframe *tframe;
      unsigned char *block;
      struct traceframe ipa_tframe;

      if (read_inferior_memory (tf, (unsigned char *) &ipa_tframe,
				offsetof (struct traceframe, data)))
	error ("Uploading: couldn't read traceframe at %s\n", paddress (tf));

      if (ipa_tframe.tpnum == 0)
	fatal ("Uploading: No (more) fast traceframes, but "
	       "ipa_traceframe_count == %u??\n",
	       ipa_traceframe_write_count - ipa_traceframe_read_count);

      /* Note that this will be incorrect for multi-location
	 tracepoints...  */
      tpoint = find_next_tracepoint_by_number (NULL, ipa_tframe.tpnum);

      tframe = add_traceframe (tpoint);
      if (tframe == NULL)
	{
	  trace_buffer_is_full = 1;
	  trace_debug ("Uploading: trace buffer is full");
	}
      else
	{
	  /* Copy the whole set of blocks in one go for now.  FIXME:
	     split this in smaller blocks.  */
	  block = add_traceframe_block (tframe, ipa_tframe.data_size);
	  if (block != NULL)
	    {
	      if (read_inferior_memory (tf
					+ offsetof (struct traceframe, data),
					block, ipa_tframe.data_size))
		error ("Uploading: Couldn't read traceframe data at %s\n",
		       paddress (tf + offsetof (struct traceframe, data)));
	    }

	  trace_debug ("Uploading: traceframe didn't fit");
	  finish_traceframe (tframe);
	}

      tf = IPA_NEXT_TRACEFRAME (tf, &ipa_tframe);

      /* If we freed the traceframe that wrapped around, go back
	 to the non-wrap case.  */
      if (tf < ipa_trace_buffer_ctrl.start)
	{
	  trace_debug ("Lib: Discarding past the wraparound");
	  ipa_trace_buffer_ctrl.wrap = ipa_trace_buffer_hi;
	}
      ipa_trace_buffer_ctrl.start = tf;
      ipa_trace_buffer_ctrl.end_free = ipa_trace_buffer_ctrl.start;
      ++ipa_traceframe_read_count;

      if (ipa_trace_buffer_ctrl.start == ipa_trace_buffer_ctrl.free
	  && ipa_trace_buffer_ctrl.start == ipa_trace_buffer_ctrl.end_free)
	{
	  trace_debug ("Lib: buffer is fully empty.  "
		       "Trace buffer [%d] start=%d free=%d endfree=%d",
		       curr_tbctrl_idx,
		       (int) (ipa_trace_buffer_ctrl.start
			      - ipa_trace_buffer_lo),
		       (int) (ipa_trace_buffer_ctrl.free
			      - ipa_trace_buffer_lo),
		       (int) (ipa_trace_buffer_ctrl.end_free
			      - ipa_trace_buffer_lo));

	  ipa_trace_buffer_ctrl.start = ipa_trace_buffer_lo;
	  ipa_trace_buffer_ctrl.free = ipa_trace_buffer_lo;
	  ipa_trace_buffer_ctrl.end_free = ipa_trace_buffer_hi;
	  ipa_trace_buffer_ctrl.wrap = ipa_trace_buffer_hi;
	}

      trace_debug ("Uploaded a traceframe\n"
		   "Lib: Trace buffer [%d] start=%d free=%d "
		   "endfree=%d wrap=%d hi=%d",
		   curr_tbctrl_idx,
		   (int) (ipa_trace_buffer_ctrl.start - ipa_trace_buffer_lo),
		   (int) (ipa_trace_buffer_ctrl.free - ipa_trace_buffer_lo),
		   (int) (ipa_trace_buffer_ctrl.end_free
			  - ipa_trace_buffer_lo),
		   (int) (ipa_trace_buffer_ctrl.wrap - ipa_trace_buffer_lo),
		   (int) (ipa_trace_buffer_hi - ipa_trace_buffer_lo));
    }

  if (write_inferior_memory (ipa_trace_buffer_ctrl_addr,
			     (unsigned char *) &ipa_trace_buffer_ctrl,
			     sizeof (struct ipa_trace_buffer_control)))
    return;

  write_inferior_integer (ipa_sym_addrs.addr_traceframe_read_count,
			  ipa_traceframe_read_count);

  trace_debug ("Done uploading traceframes [%d]\n", curr_tbctrl_idx);

  pause_all (1);
  cancel_breakpoints ();

  delete_breakpoint (about_to_request_buffer_space_bkpt);
  about_to_request_buffer_space_bkpt = NULL;

  unpause_all (1);

  if (trace_buffer_is_full)
    stop_tracing ();
}
#endif

#ifdef IN_PROCESS_AGENT

IP_AGENT_EXPORT int ust_loaded;
IP_AGENT_EXPORT char cmd_buf[CMD_BUF_SIZE];

#ifdef HAVE_UST

/* Static tracepoints.  */

/* UST puts a "struct tracepoint" in the global namespace, which
   conflicts with our tracepoint.  Arguably, being a library, it
   shouldn't take ownership of such a generic name.  We work around it
   here.  */
#define tracepoint ust_tracepoint
#include <ust/ust.h>
#undef tracepoint

extern int serialize_to_text (char *outbuf, int bufsize,
			      const char *fmt, va_list ap);

#define GDB_PROBE_NAME "gdb"

/* We dynamically search for the UST symbols instead of linking them
   in.  This lets the user decide if the application uses static
   tracepoints, instead of always pulling libust.so in.  This vector
   holds pointers to all functions we care about.  */

static struct
{
  int (*serialize_to_text) (char *outbuf, int bufsize,
			    const char *fmt, va_list ap);

  int (*ltt_probe_register) (struct ltt_available_probe *pdata);
  int (*ltt_probe_unregister) (struct ltt_available_probe *pdata);

  int (*ltt_marker_connect) (const char *channel, const char *mname,
			     const char *pname);
  int (*ltt_marker_disconnect) (const char *channel, const char *mname,
				const char *pname);

  void (*marker_iter_start) (struct marker_iter *iter);
  void (*marker_iter_next) (struct marker_iter *iter);
  void (*marker_iter_stop) (struct marker_iter *iter);
  void (*marker_iter_reset) (struct marker_iter *iter);
} ust_ops;

#include <dlfcn.h>

/* Cast through typeof to catch incompatible API changes.  Since UST
   only builds with gcc, we can freely use gcc extensions here
   too.  */
#define GET_UST_SYM(SYM)					\
  do								\
    {								\
      if (ust_ops.SYM == NULL)					\
	ust_ops.SYM = (typeof (&SYM)) dlsym (RTLD_DEFAULT, #SYM);	\
      if (ust_ops.SYM == NULL)					\
	return 0;						\
    } while (0)

#define USTF(SYM) ust_ops.SYM

/* Get pointers to all libust.so functions we care about.  */

static int
dlsym_ust (void)
{
  GET_UST_SYM (serialize_to_text);

  GET_UST_SYM (ltt_probe_register);
  GET_UST_SYM (ltt_probe_unregister);
  GET_UST_SYM (ltt_marker_connect);
  GET_UST_SYM (ltt_marker_disconnect);

  GET_UST_SYM (marker_iter_start);
  GET_UST_SYM (marker_iter_next);
  GET_UST_SYM (marker_iter_stop);
  GET_UST_SYM (marker_iter_reset);

  ust_loaded = 1;
  return 1;
}

/* Given an UST marker, return the matching gdb static tracepoint.
   The match is done by address.  */

static struct tracepoint *
ust_marker_to_static_tracepoint (const struct marker *mdata)
{
  struct tracepoint *tpoint;

  for (tpoint = tracepoints; tpoint; tpoint = tpoint->next)
    {
      if (!tpoint->enabled || tpoint->type != static_tracepoint)
	continue;

      if (tpoint->address == (uintptr_t) mdata->location)
	return tpoint;
    }

  return NULL;
}

/* The probe function we install on lttng/ust markers.  Whenever a
   probed ust marker is hit, this function is called.  This is similar
   to gdb_collect, only for static tracepoints, instead of fast
   tracepoints.  */

static void
gdb_probe (const struct marker *mdata, void *probe_private,
	   struct registers *regs, void *call_private,
	   const char *fmt, va_list *args)
{
  struct tracepoint *tpoint;
  struct static_tracepoint_ctx ctx;

  /* Don't do anything until the trace run is completely set up.  */
  if (!tracing)
    {
      trace_debug ("gdb_probe: not tracing\n");
      return;
    }

  ctx.base.type = static_tracepoint;
  ctx.regcache_initted = 0;
  ctx.regs = regs;
  ctx.fmt = fmt;
  ctx.args = args;

  /* Wrap the regblock in a register cache (in the stack, we don't
     want to malloc here).  */
  ctx.regspace = alloca (register_cache_size ());
  if (ctx.regspace == NULL)
    {
      trace_debug ("Trace buffer block allocation failed, skipping");
      return;
    }

  tpoint = ust_marker_to_static_tracepoint (mdata);
  if (tpoint == NULL)
    {
      trace_debug ("gdb_probe: marker not known: "
		   "loc:0x%p, ch:\"%s\",n:\"%s\",f:\"%s\"",
		   mdata->location, mdata->channel,
		   mdata->name, mdata->format);
      return;
    }

  ctx.tpoint = tpoint;

  trace_debug ("gdb_probe: collecting marker: "
	       "loc:0x%p, ch:\"%s\",n:\"%s\",f:\"%s\"",
	       mdata->location, mdata->channel,
	       mdata->name, mdata->format);

  /* Test the condition if present, and collect if true.  */
  if (tpoint->cond == NULL
      || condition_true_at_tracepoint ((struct tracepoint_hit_ctx *) &ctx,
				       tpoint))
    {
      collect_data_at_tracepoint ((struct tracepoint_hit_ctx *) &ctx,
				  tpoint->address, tpoint);

      if (stopping_tracepoint
	  || trace_buffer_is_full
	  || expr_eval_result != expr_eval_no_error)
	stop_tracing ();
    }
  else
    {
      /* If there was a condition and it evaluated to false, the only
	 way we would stop tracing is if there was an error during
	 condition expression evaluation.  */
      if (expr_eval_result != expr_eval_no_error)
	stop_tracing ();
    }
}

/* Called if the gdb static tracepoint requested collecting "$_sdata",
   static tracepoint string data.  This is a string passed to the
   tracing library by the user, at the time of the tracepoint marker
   call.  E.g., in the UST marker call:

     trace_mark (ust, bar33, "str %s", "FOOBAZ");

   the collected data is "str FOOBAZ".
*/

static void
collect_ust_data_at_tracepoint (struct tracepoint_hit_ctx *ctx,
				CORE_ADDR stop_pc,
				struct tracepoint *tpoint,
				struct traceframe *tframe)
{
  struct static_tracepoint_ctx *umd = (struct static_tracepoint_ctx *) ctx;
  unsigned char *bufspace;
  int size;
  va_list copy;
  unsigned short blocklen;

  if (umd == NULL)
    {
      trace_debug ("Wanted to collect static trace data, "
		   "but there's no static trace data");
      return;
    }

  va_copy (copy, *umd->args);
  size = USTF(serialize_to_text) (NULL, 0, umd->fmt, copy);
  va_end (copy);

  trace_debug ("Want to collect ust data");

  /* 'S' + size + string */
  bufspace = add_traceframe_block (tframe,
				   1 + sizeof (blocklen) + size + 1);
  if (bufspace == NULL)
    {
      trace_debug ("Trace buffer block allocation failed, skipping");
      return;
    }

  /* Identify a static trace data block.  */
  *bufspace = 'S';

  blocklen = size + 1;
  memcpy (bufspace + 1, &blocklen, sizeof (blocklen));

  va_copy (copy, *umd->args);
  USTF(serialize_to_text) ((char *) bufspace + 1 + sizeof (blocklen),
			   size + 1, umd->fmt, copy);
  va_end (copy);

  trace_debug ("Storing static tracepoint data in regblock: %s",
	       bufspace + 1 + sizeof (blocklen));
}

/* The probe to register with lttng/ust.  */
static struct ltt_available_probe gdb_ust_probe =
  {
    GDB_PROBE_NAME,
    NULL,
    gdb_probe,
  };

#endif /* HAVE_UST */
#endif /* IN_PROCESS_AGENT */

#ifdef HAVE_UST

#include <sys/socket.h>
#include <sys/un.h>

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX sizeof(((struct sockaddr_un *) NULL)->sun_path)
#endif

/* Where we put the socked used for synchronization.  */
#define SOCK_DIR P_tmpdir

#endif /* HAVE_UST */

#ifndef IN_PROCESS_AGENT

#ifdef HAVE_UST

static int
gdb_ust_connect_sync_socket (int pid)
{
  struct sockaddr_un addr;
  int res, fd;
  char path[UNIX_PATH_MAX];

  res = xsnprintf (path, UNIX_PATH_MAX, "%s/gdb_ust%d", SOCK_DIR, pid);
  if (res >= UNIX_PATH_MAX)
    {
      trace_debug ("string overflow allocating socket name");
      return -1;
    }

  res = fd = socket (PF_UNIX, SOCK_STREAM, 0);
  if (res == -1)
    {
      warning ("error opening sync socket: %s\n", strerror (errno));
      return -1;
    }

  addr.sun_family = AF_UNIX;

  res = xsnprintf (addr.sun_path, UNIX_PATH_MAX, "%s", path);
  if (res >= UNIX_PATH_MAX)
    {
      warning ("string overflow allocating socket name\n");
      close (fd);
      return -1;
    }

  res = connect (fd, (struct sockaddr *) &addr, sizeof (addr));
  if (res == -1)
    {
      warning ("error connecting sync socket (%s): %s. "
	       "Make sure the directory exists and that it is writable.",
	       path, strerror (errno));
      close (fd);
      return -1;
    }

  return fd;
}

/* Resume thread PTID.  */

static void
resume_thread (ptid_t ptid)
{
  struct thread_resume resume_info;

  resume_info.thread = ptid;
  resume_info.kind = resume_continue;
  resume_info.sig = TARGET_SIGNAL_0;
  (*the_target->resume) (&resume_info, 1);
}

/* Stop thread PTID.  */

static void
stop_thread (ptid_t ptid)
{
  struct thread_resume resume_info;

  resume_info.thread = ptid;
  resume_info.kind = resume_stop;
  resume_info.sig = TARGET_SIGNAL_0;
  (*the_target->resume) (&resume_info, 1);
}

/* Ask the in-process agent to run a command.  Since we don't want to
   have to handle the IPA hitting breakpoints while running the
   command, we pause all threads, remove all breakpoints, and then set
   the helper thread re-running.  We communicate with the helper
   thread by means of direct memory xfering, and a socket for
   synchronization.  */

static int
run_inferior_command (char *cmd)
{
  int err = -1;
  int fd = -1;
  int pid = ptid_get_pid (current_inferior->entry.id);
  int tid;
  ptid_t ptid = null_ptid;

  trace_debug ("run_inferior_command: running: %s", cmd);

  pause_all (0);
  uninsert_all_breakpoints ();

  if (read_inferior_integer (ipa_sym_addrs.addr_helper_thread_id, &tid))
    {
      warning ("Error reading helper thread's id in lib");
      goto out;
    }

  if (tid == 0)
    {
      warning ("helper thread not initialized yet");
      goto out;
    }

  if (write_inferior_memory (ipa_sym_addrs.addr_cmd_buf,
			     (unsigned char *) cmd, strlen (cmd) + 1))
    {
      warning ("Error writing command");
      goto out;
    }

  ptid = ptid_build (pid, tid, 0);

  resume_thread (ptid);

  fd = gdb_ust_connect_sync_socket (pid);
  if (fd >= 0)
    {
      char buf[1] = "";
      int ret;

      trace_debug ("signalling helper thread");

      do
	{
	  ret = write (fd, buf, 1);
	} while (ret == -1 && errno == EINTR);

      trace_debug ("waiting for helper thread's response");

      do
	{
	  ret = read (fd, buf, 1);
	} while (ret == -1 && errno == EINTR);

      close (fd);

      trace_debug ("helper thread's response received");
    }

 out:

  /* Need to read response with the inferior stopped.  */
  if (!ptid_equal (ptid, null_ptid))
    {
      int was_non_stop = non_stop;
      struct target_waitstatus status;

      stop_thread (ptid);
      non_stop = 1;
      mywait (ptid, &status, 0, 0);
      non_stop = was_non_stop;
    }

  if (fd >= 0)
    {
      if (read_inferior_memory (ipa_sym_addrs.addr_cmd_buf,
				(unsigned char *) cmd, CMD_BUF_SIZE))
	{
	  warning ("Error reading command response");
	}
      else
	{
	  err = 0;
	  trace_debug ("run_inferior_command: response: %s", cmd);
	}
    }

  reinsert_all_breakpoints ();
  unpause_all (0);

  return err;
}

#else /* HAVE_UST */

static int
run_inferior_command (char *cmd)
{
  return -1;
}

#endif /* HAVE_UST */

#else /* !IN_PROCESS_AGENT */

/* Thread ID of the helper thread.  GDBserver reads this to know which
   is the help thread.  This is an LWP id on Linux.  */
int helper_thread_id;

#ifdef HAVE_UST

static int
init_named_socket (const char *name)
{
  int result, fd;
  struct sockaddr_un addr;

  result = fd = socket (PF_UNIX, SOCK_STREAM, 0);
  if (result == -1)
    {
      warning ("socket creation failed: %s", strerror (errno));
      return -1;
    }

  addr.sun_family = AF_UNIX;

  strncpy (addr.sun_path, name, UNIX_PATH_MAX);
  addr.sun_path[UNIX_PATH_MAX - 1] = '\0';

  result = access (name, F_OK);
  if (result == 0)
    {
      /* File exists.  */
      result = unlink (name);
      if (result == -1)
	{
	  warning ("unlink failed: %s", strerror (errno));
	  close (fd);
	  return -1;
	}
      warning ("socket %s already exists; overwriting", name);
    }

  result = bind (fd, (struct sockaddr *) &addr, sizeof (addr));
  if (result == -1)
    {
      warning ("bind failed: %s", strerror (errno));
      close (fd);
      return -1;
    }

  result = listen (fd, 1);
  if (result == -1)
    {
      warning ("listen: %s", strerror (errno));
      close (fd);
      return -1;
    }

  return fd;
}

static int
gdb_ust_socket_init (void)
{
  int result, fd;
  char name[UNIX_PATH_MAX];

  result = xsnprintf (name, UNIX_PATH_MAX, "%s/gdb_ust%d",
		      SOCK_DIR, getpid ());
  if (result >= UNIX_PATH_MAX)
    {
      trace_debug ("string overflow allocating socket name");
      return -1;
    }

  fd = init_named_socket (name);
  if (fd < 0)
    warning ("Error initializing named socket (%s) for communication with the "
	     "ust helper thread. Check that directory exists and that it "
	     "is writable.", name);

  return fd;
}

/* Return an hexstr version of the STR C string, fit for sending to
   GDB.  */

static char *
cstr_to_hexstr (const char *str)
{
  int len = strlen (str);
  char *hexstr = xmalloc (len * 2 + 1);
  convert_int_to_ascii ((gdb_byte *) str, hexstr, len);
  return hexstr;
}

/* The next marker to be returned on a qTsSTM command.  */
static const struct marker *next_st;

/* Returns the first known marker.  */

struct marker *
first_marker (void)
{
  struct marker_iter iter;

  USTF(marker_iter_reset) (&iter);
  USTF(marker_iter_start) (&iter);

  return iter.marker;
}

/* Returns the marker following M.  */

const struct marker *
next_marker (const struct marker *m)
{
  struct marker_iter iter;

  USTF(marker_iter_reset) (&iter);
  USTF(marker_iter_start) (&iter);

  for (; iter.marker != NULL; USTF(marker_iter_next) (&iter))
    {
      if (iter.marker == m)
	{
	  USTF(marker_iter_next) (&iter);
	  return iter.marker;
	}
    }

  return NULL;
}

/* Compose packet that is the response to the qTsSTM/qTfSTM/qTSTMat
   packets.  */

static void
response_ust_marker (char *packet, const struct marker *st)
{
  char *strid, *format, *tmp;

  next_st = next_marker (st);

  tmp = xmalloc (strlen (st->channel) + 1 +
		 strlen (st->name) + 1);
  sprintf (tmp, "%s/%s", st->channel, st->name);

  strid = cstr_to_hexstr (tmp);
  free (tmp);

  format = cstr_to_hexstr (st->format);

  sprintf (packet, "m%s:%s:%s",
	   paddress ((uintptr_t) st->location),
	   strid,
	   format);

  free (strid);
  free (format);
}

/* Return the first static tracepoint, and initialize the state
   machine that will iterate through all the static tracepoints.  */

static void
cmd_qtfstm (char *packet)
{
  trace_debug ("Returning first trace state variable definition");

  if (first_marker ())
    response_ust_marker (packet, first_marker ());
  else
    strcpy (packet, "l");
}

/* Return additional trace state variable definitions. */

static void
cmd_qtsstm (char *packet)
{
  trace_debug ("Returning static tracepoint");

  if (next_st)
    response_ust_marker (packet, next_st);
  else
    strcpy (packet, "l");
}

/* Disconnect the GDB probe from a marker at a given address.  */

static void
unprobe_marker_at (char *packet)
{
  char *p = packet;
  ULONGEST address;
  struct marker_iter iter;

  p += sizeof ("unprobe_marker_at:") - 1;

  p = unpack_varlen_hex (p, &address);

  USTF(marker_iter_reset) (&iter);
  USTF(marker_iter_start) (&iter);
  for (; iter.marker != NULL; USTF(marker_iter_next) (&iter))
    if ((uintptr_t ) iter.marker->location == address)
      {
	int result;

	result = USTF(ltt_marker_disconnect) (iter.marker->channel,
					      iter.marker->name,
					      GDB_PROBE_NAME);
	if (result < 0)
	  warning ("could not disable marker %s/%s",
		   iter.marker->channel, iter.marker->name);
	break;
      }
}

/* Connect the GDB probe to a marker at a given address.  */

static int
probe_marker_at (char *packet)
{
  char *p = packet;
  ULONGEST address;
  struct marker_iter iter;
  struct marker *m;

  p += sizeof ("probe_marker_at:") - 1;

  p = unpack_varlen_hex (p, &address);

  USTF(marker_iter_reset) (&iter);

  for (USTF(marker_iter_start) (&iter), m = iter.marker;
       m != NULL;
       USTF(marker_iter_next) (&iter), m = iter.marker)
    if ((uintptr_t ) m->location == address)
      {
	int result;

	trace_debug ("found marker for address.  "
		     "ltt_marker_connect (marker = %s/%s)",
		     m->channel, m->name);

	result = USTF(ltt_marker_connect) (m->channel, m->name,
					   GDB_PROBE_NAME);
	if (result && result != -EEXIST)
	  trace_debug ("ltt_marker_connect (marker = %s/%s, errno = %d)",
		       m->channel, m->name, -result);

	if (result < 0)
	  {
	    sprintf (packet, "E.could not connect marker: channel=%s, name=%s",
		     m->channel, m->name);
	    return -1;
	  }

	strcpy (packet, "OK");
	return 0;
      }

  sprintf (packet, "E.no marker found at 0x%s", paddress (address));
  return -1;
}

static int
cmd_qtstmat (char *packet)
{
  char *p = packet;
  ULONGEST address;
  struct marker_iter iter;
  struct marker *m;

  p += sizeof ("qTSTMat:") - 1;

  p = unpack_varlen_hex (p, &address);

  USTF(marker_iter_reset) (&iter);

  for (USTF(marker_iter_start) (&iter), m = iter.marker;
       m != NULL;
       USTF(marker_iter_next) (&iter), m = iter.marker)
    if ((uintptr_t ) m->location == address)
      {
	response_ust_marker (packet, m);
	return 0;
      }

  strcpy (packet, "l");
  return -1;
}

static void *
gdb_ust_thread (void *arg)
{
  int listen_fd;

  while (1)
    {
      listen_fd = gdb_ust_socket_init ();

#ifdef SYS_gettid
      if (helper_thread_id == 0)
	helper_thread_id = syscall (SYS_gettid);
#endif

      if (listen_fd == -1)
	{
	  warning ("could not create sync socket\n");
	  break;
	}

      while (1)
	{
	  socklen_t tmp;
	  struct sockaddr_un sockaddr;
	  int fd;
	  char buf[1];
	  int ret;

	  tmp = sizeof (sockaddr);

	  do
	    {
	      fd = accept (listen_fd, &sockaddr, &tmp);
	    }
	  /* It seems an ERESTARTSYS can escape out of accept.  */
	  while (fd == -512 || (fd == -1 && errno == EINTR));

	  if (fd < 0)
	    {
	      warning ("Accept returned %d, error: %s\n",
		       fd, strerror (errno));
	      break;
	    }

	  do
	    {
	      ret = read (fd, buf, 1);
	    } while (ret == -1 && errno == EINTR);

	  if (ret == -1)
	    {
	      warning ("reading socket (fd=%d) failed with %s",
		       fd, strerror (errno));
	      close (fd);
	      break;
	    }

	  if (cmd_buf[0])
	    {
	      if (strcmp ("qTfSTM", cmd_buf) == 0)
		{
		  cmd_qtfstm (cmd_buf);
		}
	      else if (strcmp ("qTsSTM", cmd_buf) == 0)
		{
		  cmd_qtsstm (cmd_buf);
		}
	      else if (strncmp ("unprobe_marker_at:",
				cmd_buf,
				sizeof ("unprobe_marker_at:") - 1) == 0)
		{
		  unprobe_marker_at (cmd_buf);
		}
	      else if (strncmp ("probe_marker_at:",
				cmd_buf,
				sizeof ("probe_marker_at:") - 1) == 0)
		{
		  probe_marker_at (cmd_buf);
		}
	      else if (strncmp ("qTSTMat:",
				cmd_buf,
				sizeof ("qTSTMat:") - 1) == 0)
		{
		  cmd_qtstmat (cmd_buf);
		}
	      else if (strcmp (cmd_buf, "help") == 0)
		{
		  strcpy (cmd_buf, "for help, press F1\n");
		}
	      else
		strcpy (cmd_buf, "");
	    }

	  write (fd, buf, 1);
	  close (fd);
	}
    }

  return NULL;
}

#include <signal.h>

static void
gdb_ust_init (void)
{
  int res;
  pthread_t thread;
  sigset_t new_mask;
  sigset_t orig_mask;

  if (!dlsym_ust ())
    return;

  /* We want the helper thread to be as transparent as possible, so
     have it inherit an all-signals-blocked mask.  */

  sigfillset (&new_mask);
  res = pthread_sigmask (SIG_SETMASK, &new_mask, &orig_mask);
  if (res)
    fatal ("pthread_sigmask (1) failed: %s", strerror (res));

  res = pthread_create (&thread,
			NULL,
			gdb_ust_thread,
			NULL);

  res = pthread_sigmask (SIG_SETMASK, &orig_mask, NULL);
  if (res)
    fatal ("pthread_sigmask (2) failed: %s", strerror (res));

  while (helper_thread_id == 0)
    usleep (1);

  USTF(ltt_probe_register) (&gdb_ust_probe);
}

#endif /* HAVE_UST */

#include <sys/mman.h>
#include <fcntl.h>

IP_AGENT_EXPORT char *gdb_tp_heap_buffer;
IP_AGENT_EXPORT char *gdb_jump_pad_buffer;
IP_AGENT_EXPORT char *gdb_jump_pad_buffer_end;

static void __attribute__ ((constructor))
initialize_tracepoint_ftlib (void)
{
  initialize_tracepoint ();

#ifdef HAVE_UST
  gdb_ust_init ();
#endif
}

#endif /* IN_PROCESS_AGENT */

static LONGEST
tsv_get_timestamp (void)
{
   struct timeval tv;

   if (gettimeofday (&tv, 0) != 0)
     return -1;
   else
     return (LONGEST) tv.tv_sec * 1000000 + tv.tv_usec;
}

void
initialize_tracepoint (void)
{
  /* There currently no way to change the buffer size.  */
  const int sizeOfBuffer = 5 * 1024 * 1024;
  unsigned char *buf = xmalloc (sizeOfBuffer);
  init_trace_buffer (buf, sizeOfBuffer);

  /* Wire trace state variable 1 to be the timestamp.  This will be
     uploaded to GDB upon connection and become one of its trace state
     variables.  (In case you're wondering, if GDB already has a trace
     variable numbered 1, it will be renumbered.)  */
  create_trace_state_variable (1, 0);
  set_trace_state_variable_name (1, "trace_timestamp");
  set_trace_state_variable_getter (1, tsv_get_timestamp);

#ifdef IN_PROCESS_AGENT
  {
    int pagesize;
    pagesize = sysconf (_SC_PAGE_SIZE);
    if (pagesize == -1)
      fatal ("sysconf");

    gdb_tp_heap_buffer = xmalloc (5 * 1024 * 1024);

    /* Allocate scratch buffer aligned on a page boundary.  */
    gdb_jump_pad_buffer = memalign (pagesize, pagesize * 20);
    gdb_jump_pad_buffer_end = gdb_jump_pad_buffer + pagesize * 20;

    /* Make it writable and executable.  */
    if (mprotect (gdb_jump_pad_buffer, pagesize * 20,
		  PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
      fatal ("\
initialize_tracepoint: mprotect(%p, %d, PROT_READ|PROT_EXEC) failed with %s",
	     gdb_jump_pad_buffer, pagesize * 20, strerror (errno));
  }

  initialize_low_tracepoint ();
#endif
}
