/* Low level interface to ptrace, for the remote server for GDB.
   Copyright (C) 1995, 1996, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
   2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.

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
#include "linux-low.h"

#include <sys/wait.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sched.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#ifndef ELFMAG0
/* Don't include <linux/elf.h> here.  If it got included by gdb_proc_service.h
   then ELFMAG0 will have been defined.  If it didn't get included by
   gdb_proc_service.h then including it will likely introduce a duplicate
   definition of elf_fpregset_t.  */
#include <elf.h>
#endif

#ifndef SPUFS_MAGIC
#define SPUFS_MAGIC 0x23c9b64e
#endif

#ifndef PTRACE_GETSIGINFO
# define PTRACE_GETSIGINFO 0x4202
# define PTRACE_SETSIGINFO 0x4203
#endif

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

/* If the system headers did not provide the constants, hard-code the normal
   values.  */
#ifndef PTRACE_EVENT_FORK

#define PTRACE_SETOPTIONS	0x4200
#define PTRACE_GETEVENTMSG	0x4201

/* options set using PTRACE_SETOPTIONS */
#define PTRACE_O_TRACESYSGOOD	0x00000001
#define PTRACE_O_TRACEFORK	0x00000002
#define PTRACE_O_TRACEVFORK	0x00000004
#define PTRACE_O_TRACECLONE	0x00000008
#define PTRACE_O_TRACEEXEC	0x00000010
#define PTRACE_O_TRACEVFORKDONE	0x00000020
#define PTRACE_O_TRACEEXIT	0x00000040

/* Wait extended result codes for the above trace options.  */
#define PTRACE_EVENT_FORK	1
#define PTRACE_EVENT_VFORK	2
#define PTRACE_EVENT_CLONE	3
#define PTRACE_EVENT_EXEC	4
#define PTRACE_EVENT_VFORK_DONE	5
#define PTRACE_EVENT_EXIT	6

#endif /* PTRACE_EVENT_FORK */

/* We can't always assume that this flag is available, but all systems
   with the ptrace event handlers also have __WALL, so it's safe to use
   in some contexts.  */
#ifndef __WALL
#define __WALL          0x40000000 /* Wait for any child.  */
#endif

#ifndef W_STOPCODE
#define W_STOPCODE(sig) ((sig) << 8 | 0x7f)
#endif

#ifdef __UCLIBC__
#if !(defined(__UCLIBC_HAS_MMU__) || defined(__ARCH_HAS_MMU__))
#define HAS_NOMMU
#endif
#endif

/* ``all_threads'' is keyed by the LWP ID, which we use as the GDB protocol
   representation of the thread ID.

   ``all_lwps'' is keyed by the process ID - which on Linux is (presently)
   the same as the LWP ID.

   ``all_processes'' is keyed by the "overall process ID", which
   GNU/Linux calls tgid, "thread group ID".  */

struct inferior_list all_lwps;

/* A list of all unknown processes which receive stop signals.  Some other
   process will presumably claim each of these as forked children
   momentarily.  */

struct inferior_list stopped_pids;

/* FIXME this is a bit of a hack, and could be removed.  */
int stopping_threads;

/* FIXME make into a target method?  */
int using_threads = 1;

/* This flag is true iff we've just created or attached to our first
   inferior but it has not stopped yet.  As soon as it does, we need
   to call the low target's arch_setup callback.  Doing this only on
   the first inferior avoids reinializing the architecture on every
   inferior, and avoids messing with the register caches of the
   already running inferiors.  NOTE: this assumes all inferiors under
   control of gdbserver have the same architecture.  */
static int new_inferior;

static void linux_resume_one_lwp (struct lwp_info *lwp,
				  int step, int signal, siginfo_t *info);
static void linux_resume (struct thread_resume *resume_info, size_t n);
static void stop_all_lwps (void);
static int linux_wait_for_event (ptid_t ptid, int *wstat, int options);
static void *add_lwp (ptid_t ptid);
static int linux_stopped_by_watchpoint (void);
static void mark_lwp_dead (struct lwp_info *lwp, int wstat);
static int linux_core_of_thread (ptid_t ptid);
static void proceed_all_lwps (void);
static void unstop_all_lwps (struct lwp_info *except);
static int finish_step_over (struct lwp_info *lwp);
static CORE_ADDR get_stop_pc (struct lwp_info *lwp);
static int kill_lwp (unsigned long lwpid, int signo);

/* True if the low target can hardware single-step.  Such targets
   don't need a BREAKPOINT_REINSERT_ADDR callback.  */

static int
can_hardware_single_step (void)
{
  return (the_low_target.breakpoint_reinsert_addr == NULL);
}

/* True if the low target supports memory breakpoints.  If so, we'll
   have a GET_PC implementation.  */

static int
supports_breakpoints (void)
{
  return (the_low_target.get_pc != NULL);
}

struct pending_signals
{
  int signal;
  siginfo_t info;
  struct pending_signals *prev;
};

#define PTRACE_ARG3_TYPE void *
#define PTRACE_ARG4_TYPE void *
#define PTRACE_XFER_TYPE long

#ifdef HAVE_LINUX_REGSETS
static char *disabled_regsets;
static int num_regsets;
#endif

/* The read/write ends of the pipe registered as waitable file in the
   event loop.  */
static int linux_event_pipe[2] = { -1, -1 };

/* True if we're currently in async mode.  */
#define target_is_async_p() (linux_event_pipe[0] != -1)

static void send_sigstop (struct inferior_list_entry *entry);
static void wait_for_sigstop (struct inferior_list_entry *entry);

/* Accepts an integer PID; Returns a string representing a file that
   can be opened to get info for the child process.
   Space for the result is malloc'd, caller must free.  */

char *
linux_child_pid_to_exec_file (int pid)
{
  char *name1, *name2;

  name1 = xmalloc (MAXPATHLEN);
  name2 = xmalloc (MAXPATHLEN);
  memset (name2, 0, MAXPATHLEN);

  sprintf (name1, "/proc/%d/exe", pid);
  if (readlink (name1, name2, MAXPATHLEN) > 0)
    {
      free (name1);
      return name2;
    }
  else
    {
      free (name2);
      return name1;
    }
}

/* Return non-zero if HEADER is a 64-bit ELF file.  */

static int
elf_64_header_p (const Elf64_Ehdr *header)
{
  return (header->e_ident[EI_MAG0] == ELFMAG0
          && header->e_ident[EI_MAG1] == ELFMAG1
          && header->e_ident[EI_MAG2] == ELFMAG2
          && header->e_ident[EI_MAG3] == ELFMAG3
          && header->e_ident[EI_CLASS] == ELFCLASS64);
}

/* Return non-zero if FILE is a 64-bit ELF file,
   zero if the file is not a 64-bit ELF file,
   and -1 if the file is not accessible or doesn't exist.  */

int
elf_64_file_p (const char *file)
{
  Elf64_Ehdr header;
  int fd;

  fd = open (file, O_RDONLY);
  if (fd < 0)
    return -1;

  if (read (fd, &header, sizeof (header)) != sizeof (header))
    {
      close (fd);
      return 0;
    }
  close (fd);

  return elf_64_header_p (&header);
}

static void
delete_lwp (struct lwp_info *lwp)
{
  remove_thread (get_lwp_thread (lwp));
  remove_inferior (&all_lwps, &lwp->head);
  free (lwp->arch_private);
  free (lwp);
}

/* Add a process to the common process list, and set its private
   data.  */

static struct process_info *
linux_add_process (int pid, int attached)
{
  struct process_info *proc;

  /* Is this the first process?  If so, then set the arch.  */
  if (all_processes.head == NULL)
    new_inferior = 1;

  proc = add_process (pid, attached);
  proc->private = xcalloc (1, sizeof (*proc->private));

  if (the_low_target.new_process != NULL)
    proc->private->arch_private = the_low_target.new_process ();

  return proc;
}

/* Remove a process from the common process list,
   also freeing all private data.  */

static void
linux_remove_process (struct process_info *process)
{
  struct process_info_private *priv = process->private;

  free (priv->arch_private);
  free (priv);
  remove_process (process);
}

/* Wrapper function for waitpid which handles EINTR, and emulates
   __WALL for systems where that is not available.  */

static int
my_waitpid (int pid, int *status, int flags)
{
  int ret, out_errno;

  if (debug_threads)
    fprintf (stderr, "my_waitpid (%d, 0x%x)\n", pid, flags);

  if (flags & __WALL)
    {
      sigset_t block_mask, org_mask, wake_mask;
      int wnohang;

      wnohang = (flags & WNOHANG) != 0;
      flags &= ~(__WALL | __WCLONE);
      flags |= WNOHANG;

      /* Block all signals while here.  This avoids knowing about
	 LinuxThread's signals.  */
      sigfillset (&block_mask);
      sigprocmask (SIG_BLOCK, &block_mask, &org_mask);

      /* ... except during the sigsuspend below.  */
      sigemptyset (&wake_mask);

      while (1)
	{
	  /* Since all signals are blocked, there's no need to check
	     for EINTR here.  */
	  ret = waitpid (pid, status, flags);
	  out_errno = errno;

	  if (ret == -1 && out_errno != ECHILD)
	    break;
	  else if (ret > 0)
	    break;

	  if (flags & __WCLONE)
	    {
	      /* We've tried both flavors now.  If WNOHANG is set,
		 there's nothing else to do, just bail out.  */
	      if (wnohang)
		break;

	      if (debug_threads)
		fprintf (stderr, "blocking\n");

	      /* Block waiting for signals.  */
	      sigsuspend (&wake_mask);
	    }

	  flags ^= __WCLONE;
	}

      sigprocmask (SIG_SETMASK, &org_mask, NULL);
    }
  else
    {
      do
	ret = waitpid (pid, status, flags);
      while (ret == -1 && errno == EINTR);
      out_errno = errno;
    }

  if (debug_threads)
    fprintf (stderr, "my_waitpid (%d, 0x%x): status(%x), %d\n",
	     pid, flags, status ? *status : -1, ret);

  errno = out_errno;
  return ret;
}

/* Handle a GNU/Linux extended wait response.  If we see a clone
   event, we need to add the new LWP to our list (and not report the
   trap to higher layers).  */

static void
handle_extended_wait (struct lwp_info *event_child, int wstat)
{
  int event = wstat >> 16;
  struct lwp_info *new_lwp;

  if (event == PTRACE_EVENT_CLONE)
    {
      ptid_t ptid;
      unsigned long new_pid;
      int ret, status = W_STOPCODE (SIGSTOP);

      ptrace (PTRACE_GETEVENTMSG, lwpid_of (event_child), 0, &new_pid);

      /* If we haven't already seen the new PID stop, wait for it now.  */
      if (! pull_pid_from_list (&stopped_pids, new_pid))
	{
	  /* The new child has a pending SIGSTOP.  We can't affect it until it
	     hits the SIGSTOP, but we're already attached.  */

	  ret = my_waitpid (new_pid, &status, __WALL);

	  if (ret == -1)
	    perror_with_name ("waiting for new child");
	  else if (ret != new_pid)
	    warning ("wait returned unexpected PID %d", ret);
	  else if (!WIFSTOPPED (status))
	    warning ("wait returned unexpected status 0x%x", status);
	}

      ptrace (PTRACE_SETOPTIONS, new_pid, 0, (PTRACE_ARG4_TYPE) PTRACE_O_TRACECLONE);

      ptid = ptid_build (pid_of (event_child), new_pid, 0);
      new_lwp = (struct lwp_info *) add_lwp (ptid);
      add_thread (ptid, new_lwp);

      /* Either we're going to immediately resume the new thread
	 or leave it stopped.  linux_resume_one_lwp is a nop if it
	 thinks the thread is currently running, so set this first
	 before calling linux_resume_one_lwp.  */
      new_lwp->stopped = 1;

      /* Normally we will get the pending SIGSTOP.  But in some cases
	 we might get another signal delivered to the group first.
	 If we do get another signal, be sure not to lose it.  */
      if (WSTOPSIG (status) == SIGSTOP)
	{
	  if (stopping_threads)
	    new_lwp->stop_pc = get_stop_pc (new_lwp);
	  else
	    linux_resume_one_lwp (new_lwp, 0, 0, NULL);
	}
      else
	{
	  new_lwp->stop_expected = 1;

	  if (stopping_threads)
	    {
	      new_lwp->stop_pc = get_stop_pc (new_lwp);
	      new_lwp->status_pending_p = 1;
	      new_lwp->status_pending = status;
	    }
	  else
	    /* Pass the signal on.  This is what GDB does - except
	       shouldn't we really report it instead?  */
	    linux_resume_one_lwp (new_lwp, 0, WSTOPSIG (status), NULL);
	}

      /* Always resume the current thread.  If we are stopping
	 threads, it will have a pending SIGSTOP; we may as well
	 collect it now.  */
      linux_resume_one_lwp (event_child, event_child->stepping, 0, NULL);
    }
}

/* Return the PC as read from the regcache of LWP, without any
   adjustment.  */

static CORE_ADDR
get_pc (struct lwp_info *lwp)
{
  struct thread_info *saved_inferior;
  struct regcache *regcache;
  CORE_ADDR pc;

  if (the_low_target.get_pc == NULL)
    return 0;

  saved_inferior = current_inferior;
  current_inferior = get_lwp_thread (lwp);

  regcache = get_thread_regcache (current_inferior, 1);
  pc = (*the_low_target.get_pc) (regcache);

  if (debug_threads)
    fprintf (stderr, "pc is 0x%lx\n", (long) pc);

  current_inferior = saved_inferior;
  return pc;
}

/* This function should only be called if LWP got a SIGTRAP.
   The SIGTRAP could mean several things.

   On i386, where decr_pc_after_break is non-zero:
   If we were single-stepping this process using PTRACE_SINGLESTEP,
   we will get only the one SIGTRAP (even if the instruction we
   stepped over was a breakpoint).  The value of $eip will be the
   next instruction.
   If we continue the process using PTRACE_CONT, we will get a
   SIGTRAP when we hit a breakpoint.  The value of $eip will be
   the instruction after the breakpoint (i.e. needs to be
   decremented).  If we report the SIGTRAP to GDB, we must also
   report the undecremented PC.  If we cancel the SIGTRAP, we
   must resume at the decremented PC.

   (Presumably, not yet tested) On a non-decr_pc_after_break machine
   with hardware or kernel single-step:
   If we single-step over a breakpoint instruction, our PC will
   point at the following instruction.  If we continue and hit a
   breakpoint instruction, our PC will point at the breakpoint
   instruction.  */

static CORE_ADDR
get_stop_pc (struct lwp_info *lwp)
{
  CORE_ADDR stop_pc;

  if (the_low_target.get_pc == NULL)
    return 0;

  stop_pc = get_pc (lwp);

  if (WSTOPSIG (lwp->last_status) == SIGTRAP
      && !lwp->stepping
      && !lwp->stopped_by_watchpoint
      && lwp->last_status >> 16 == 0)
    stop_pc -= the_low_target.decr_pc_after_break;

  if (debug_threads)
    fprintf (stderr, "stop pc is 0x%lx\n", (long) stop_pc);

  return stop_pc;
}

static void *
add_lwp (ptid_t ptid)
{
  struct lwp_info *lwp;

  lwp = (struct lwp_info *) xmalloc (sizeof (*lwp));
  memset (lwp, 0, sizeof (*lwp));

  lwp->head.id = ptid;

  lwp->last_resume_kind = resume_continue;

  if (the_low_target.new_thread != NULL)
    lwp->arch_private = the_low_target.new_thread ();

  add_inferior_to_list (&all_lwps, &lwp->head);

  return lwp;
}

/* Start an inferior process and returns its pid.
   ALLARGS is a vector of program-name and args. */

static int
linux_create_inferior (char *program, char **allargs)
{
  struct lwp_info *new_lwp;
  int pid;
  ptid_t ptid;

#if defined(__UCLIBC__) && defined(HAS_NOMMU)
  pid = vfork ();
#else
  pid = fork ();
#endif
  if (pid < 0)
    perror_with_name ("fork");

  if (pid == 0)
    {
      ptrace (PTRACE_TRACEME, 0, 0, 0);

#ifdef __SIGRTMIN /* Bionic doesn't use SIGRTMIN the way glibc does.  */
      signal (__SIGRTMIN + 1, SIG_DFL);
#endif

      setpgid (0, 0);

      execv (program, allargs);
      if (errno == ENOENT)
	execvp (program, allargs);

      fprintf (stderr, "Cannot exec %s: %s.\n", program,
	       strerror (errno));
      fflush (stderr);
      _exit (0177);
    }

  linux_add_process (pid, 0);

  ptid = ptid_build (pid, pid, 0);
  new_lwp = add_lwp (ptid);
  add_thread (ptid, new_lwp);
  new_lwp->must_set_ptrace_flags = 1;

  return pid;
}

/* Attach to an inferior process.  */

static void
linux_attach_lwp_1 (unsigned long lwpid, int initial)
{
  ptid_t ptid;
  struct lwp_info *new_lwp;

  if (ptrace (PTRACE_ATTACH, lwpid, 0, 0) != 0)
    {
      if (!initial)
	{
	  /* If we fail to attach to an LWP, just warn.  */
	  fprintf (stderr, "Cannot attach to lwp %ld: %s (%d)\n", lwpid,
		   strerror (errno), errno);
	  fflush (stderr);
	  return;
	}
      else
	/* If we fail to attach to a process, report an error.  */
	error ("Cannot attach to lwp %ld: %s (%d)\n", lwpid,
	       strerror (errno), errno);
    }

  if (initial)
    /* NOTE/FIXME: This lwp might have not been the tgid.  */
    ptid = ptid_build (lwpid, lwpid, 0);
  else
    {
      /* Note that extracting the pid from the current inferior is
	 safe, since we're always called in the context of the same
	 process as this new thread.  */
      int pid = pid_of (get_thread_lwp (current_inferior));
      ptid = ptid_build (pid, lwpid, 0);
    }

  new_lwp = (struct lwp_info *) add_lwp (ptid);
  add_thread (ptid, new_lwp);

  /* We need to wait for SIGSTOP before being able to make the next
     ptrace call on this LWP.  */
  new_lwp->must_set_ptrace_flags = 1;

  /* The next time we wait for this LWP we'll see a SIGSTOP as PTRACE_ATTACH
     brings it to a halt.

     There are several cases to consider here:

     1) gdbserver has already attached to the process and is being notified
	of a new thread that is being created.
	In this case we should ignore that SIGSTOP and resume the
	process.  This is handled below by setting stop_expected = 1,
	and the fact that add_lwp sets last_resume_kind ==
	resume_continue.

     2) This is the first thread (the process thread), and we're attaching
	to it via attach_inferior.
	In this case we want the process thread to stop.
	This is handled by having linux_attach set last_resume_kind ==
	resume_stop after we return.
	??? If the process already has several threads we leave the other
	threads running.

     3) GDB is connecting to gdbserver and is requesting an enumeration of all
	existing threads.
	In this case we want the thread to stop.
	FIXME: This case is currently not properly handled.
	We should wait for the SIGSTOP but don't.  Things work apparently
	because enough time passes between when we ptrace (ATTACH) and when
	gdb makes the next ptrace call on the thread.

     On the other hand, if we are currently trying to stop all threads, we
     should treat the new thread as if we had sent it a SIGSTOP.  This works
     because we are guaranteed that the add_lwp call above added us to the
     end of the list, and so the new thread has not yet reached
     wait_for_sigstop (but will).  */
  new_lwp->stop_expected = 1;
}

void
linux_attach_lwp (unsigned long lwpid)
{
  linux_attach_lwp_1 (lwpid, 0);
}

int
linux_attach (unsigned long pid)
{
  struct lwp_info *lwp;

  linux_attach_lwp_1 (pid, 1);

  linux_add_process (pid, 1);

  if (!non_stop)
    {
      /* Don't ignore the initial SIGSTOP if we just attached to this
	 process.  It will be collected by wait shortly.  */
      lwp = (struct lwp_info *) find_inferior_id (&all_lwps,
						  ptid_build (pid, pid, 0));
      lwp->last_resume_kind = resume_stop;
    }

  return 0;
}

struct counter
{
  int pid;
  int count;
};

static int
second_thread_of_pid_p (struct inferior_list_entry *entry, void *args)
{
  struct counter *counter = args;

  if (ptid_get_pid (entry->id) == counter->pid)
    {
      if (++counter->count > 1)
	return 1;
    }

  return 0;
}

static int
last_thread_of_process_p (struct thread_info *thread)
{
  ptid_t ptid = ((struct inferior_list_entry *)thread)->id;
  int pid = ptid_get_pid (ptid);
  struct counter counter = { pid , 0 };

  return (find_inferior (&all_threads,
			 second_thread_of_pid_p, &counter) == NULL);
}

/* Kill the inferior lwp.  */

static int
linux_kill_one_lwp (struct inferior_list_entry *entry, void *args)
{
  struct thread_info *thread = (struct thread_info *) entry;
  struct lwp_info *lwp = get_thread_lwp (thread);
  int wstat;
  int pid = * (int *) args;

  if (ptid_get_pid (entry->id) != pid)
    return 0;

  /* We avoid killing the first thread here, because of a Linux kernel (at
     least 2.6.0-test7 through 2.6.8-rc4) bug; if we kill the parent before
     the children get a chance to be reaped, it will remain a zombie
     forever.  */

  if (lwpid_of (lwp) == pid)
    {
      if (debug_threads)
	fprintf (stderr, "lkop: is last of process %s\n",
		 target_pid_to_str (entry->id));
      return 0;
    }

  /* If we're killing a running inferior, make sure it is stopped
     first, as PTRACE_KILL will not work otherwise.  */
  if (!lwp->stopped)
    send_sigstop (&lwp->head);

  do
    {
      ptrace (PTRACE_KILL, lwpid_of (lwp), 0, 0);

      /* Make sure it died.  The loop is most likely unnecessary.  */
      pid = linux_wait_for_event (lwp->head.id, &wstat, __WALL);
    } while (pid > 0 && WIFSTOPPED (wstat));

  return 0;
}

static int
linux_kill (int pid)
{
  struct process_info *process;
  struct lwp_info *lwp;
  struct thread_info *thread;
  int wstat;
  int lwpid;

  process = find_process_pid (pid);
  if (process == NULL)
    return -1;

  find_inferior (&all_threads, linux_kill_one_lwp, &pid);

  /* See the comment in linux_kill_one_lwp.  We did not kill the first
     thread in the list, so do so now.  */
  lwp = find_lwp_pid (pid_to_ptid (pid));
  thread = get_lwp_thread (lwp);

  if (debug_threads)
    fprintf (stderr, "lk_1: killing lwp %ld, for pid: %d\n",
	     lwpid_of (lwp), pid);

  /* If we're killing a running inferior, make sure it is stopped
     first, as PTRACE_KILL will not work otherwise.  */
  if (!lwp->stopped)
    send_sigstop (&lwp->head);

  do
    {
      ptrace (PTRACE_KILL, lwpid_of (lwp), 0, 0);

      /* Make sure it died.  The loop is most likely unnecessary.  */
      lwpid = linux_wait_for_event (lwp->head.id, &wstat, __WALL);
    } while (lwpid > 0 && WIFSTOPPED (wstat));

#ifdef USE_THREAD_DB
  thread_db_free (process, 0);
#endif
  delete_lwp (lwp);
  linux_remove_process (process);
  return 0;
}

static int
linux_detach_one_lwp (struct inferior_list_entry *entry, void *args)
{
  struct thread_info *thread = (struct thread_info *) entry;
  struct lwp_info *lwp = get_thread_lwp (thread);
  int pid = * (int *) args;

  if (ptid_get_pid (entry->id) != pid)
    return 0;

  /* If we're detaching from a running inferior, make sure it is
     stopped first, as PTRACE_DETACH will not work otherwise.  */
  if (!lwp->stopped)
    {
      int lwpid = lwpid_of (lwp);

      stopping_threads = 1;
      send_sigstop (&lwp->head);

      /* If this detects a new thread through a clone event, the new
	 thread is appended to the end of the lwp list, so we'll
	 eventually detach from it.  */
      wait_for_sigstop (&lwp->head);
      stopping_threads = 0;

      /* If LWP exits while we're trying to stop it, there's nothing
	 left to do.  */
      lwp = find_lwp_pid (pid_to_ptid (lwpid));
      if (lwp == NULL)
	return 0;
    }

  /* If this process is stopped but is expecting a SIGSTOP, then make
     sure we take care of that now.  This isn't absolutely guaranteed
     to collect the SIGSTOP, but is fairly likely to.  */
  if (lwp->stop_expected)
    {
      int wstat;
      /* Clear stop_expected, so that the SIGSTOP will be reported.  */
      lwp->stop_expected = 0;
      if (lwp->stopped)
	linux_resume_one_lwp (lwp, 0, 0, NULL);
      linux_wait_for_event (lwp->head.id, &wstat, __WALL);
    }

  /* Flush any pending changes to the process's registers.  */
  regcache_invalidate_one ((struct inferior_list_entry *)
			   get_lwp_thread (lwp));

  /* Finally, let it resume.  */
  ptrace (PTRACE_DETACH, lwpid_of (lwp), 0, 0);

  delete_lwp (lwp);
  return 0;
}

static int
any_thread_of (struct inferior_list_entry *entry, void *args)
{
  int *pid_p = args;

  if (ptid_get_pid (entry->id) == *pid_p)
    return 1;

  return 0;
}

static int
linux_detach (int pid)
{
  struct process_info *process;

  process = find_process_pid (pid);
  if (process == NULL)
    return -1;

#ifdef USE_THREAD_DB
  thread_db_free (process, 1);
#endif

  current_inferior =
    (struct thread_info *) find_inferior (&all_threads, any_thread_of, &pid);

  delete_all_breakpoints ();
  find_inferior (&all_threads, linux_detach_one_lwp, &pid);
  linux_remove_process (process);
  return 0;
}

static void
linux_join (int pid)
{
  int status, ret;
  struct process_info *process;

  process = find_process_pid (pid);
  if (process == NULL)
    return;

  do {
    ret = my_waitpid (pid, &status, 0);
    if (WIFEXITED (status) || WIFSIGNALED (status))
      break;
  } while (ret != -1 || errno != ECHILD);
}

/* Return nonzero if the given thread is still alive.  */
static int
linux_thread_alive (ptid_t ptid)
{
  struct lwp_info *lwp = find_lwp_pid (ptid);

  /* We assume we always know if a thread exits.  If a whole process
     exited but we still haven't been able to report it to GDB, we'll
     hold on to the last lwp of the dead process.  */
  if (lwp != NULL)
    return !lwp->dead;
  else
    return 0;
}

/* Return 1 if this lwp has an interesting status pending.  */
static int
status_pending_p_callback (struct inferior_list_entry *entry, void *arg)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;
  ptid_t ptid = * (ptid_t *) arg;
  struct thread_info *thread = get_lwp_thread (lwp);

  /* Check if we're only interested in events from a specific process
     or its lwps.  */
  if (!ptid_equal (minus_one_ptid, ptid)
      && ptid_get_pid (ptid) != ptid_get_pid (lwp->head.id))
    return 0;

  thread = get_lwp_thread (lwp);

  /* If we got a `vCont;t', but we haven't reported a stop yet, do
     report any status pending the LWP may have.  */
  if (lwp->last_resume_kind == resume_stop
      && thread->last_status.kind == TARGET_WAITKIND_STOPPED)
    return 0;

  return lwp->status_pending_p;
}

static int
same_lwp (struct inferior_list_entry *entry, void *data)
{
  ptid_t ptid = *(ptid_t *) data;
  int lwp;

  if (ptid_get_lwp (ptid) != 0)
    lwp = ptid_get_lwp (ptid);
  else
    lwp = ptid_get_pid (ptid);

  if (ptid_get_lwp (entry->id) == lwp)
    return 1;

  return 0;
}

struct lwp_info *
find_lwp_pid (ptid_t ptid)
{
  return (struct lwp_info*) find_inferior (&all_lwps, same_lwp, &ptid);
}

static struct lwp_info *
linux_wait_for_lwp (ptid_t ptid, int *wstatp, int options)
{
  int ret;
  int to_wait_for = -1;
  struct lwp_info *child = NULL;

  if (debug_threads)
    fprintf (stderr, "linux_wait_for_lwp: %s\n", target_pid_to_str (ptid));

  if (ptid_equal (ptid, minus_one_ptid))
    to_wait_for = -1;			/* any child */
  else
    to_wait_for = ptid_get_lwp (ptid);	/* this lwp only */

  options |= __WALL;

retry:

  ret = my_waitpid (to_wait_for, wstatp, options);
  if (ret == 0 || (ret == -1 && errno == ECHILD && (options & WNOHANG)))
    return NULL;
  else if (ret == -1)
    perror_with_name ("waitpid");

  if (debug_threads
      && (!WIFSTOPPED (*wstatp)
	  || (WSTOPSIG (*wstatp) != 32
	      && WSTOPSIG (*wstatp) != 33)))
    fprintf (stderr, "Got an event from %d (%x)\n", ret, *wstatp);

  child = find_lwp_pid (pid_to_ptid (ret));

  /* If we didn't find a process, one of two things presumably happened:
     - A process we started and then detached from has exited.  Ignore it.
     - A process we are controlling has forked and the new child's stop
     was reported to us by the kernel.  Save its PID.  */
  if (child == NULL && WIFSTOPPED (*wstatp))
    {
      add_pid_to_list (&stopped_pids, ret);
      goto retry;
    }
  else if (child == NULL)
    goto retry;

  child->stopped = 1;

  child->last_status = *wstatp;

  /* Architecture-specific setup after inferior is running.
     This needs to happen after we have attached to the inferior
     and it is stopped for the first time, but before we access
     any inferior registers.  */
  if (new_inferior)
    {
      the_low_target.arch_setup ();
#ifdef HAVE_LINUX_REGSETS
      memset (disabled_regsets, 0, num_regsets);
#endif
      new_inferior = 0;
    }

  /* Fetch the possibly triggered data watchpoint info and store it in
     CHILD.

     On some archs, like x86, that use debug registers to set
     watchpoints, it's possible that the way to know which watched
     address trapped, is to check the register that is used to select
     which address to watch.  Problem is, between setting the
     watchpoint and reading back which data address trapped, the user
     may change the set of watchpoints, and, as a consequence, GDB
     changes the debug registers in the inferior.  To avoid reading
     back a stale stopped-data-address when that happens, we cache in
     LP the fact that a watchpoint trapped, and the corresponding data
     address, as soon as we see CHILD stop with a SIGTRAP.  If GDB
     changes the debug registers meanwhile, we have the cached data we
     can rely on.  */

  if (WIFSTOPPED (*wstatp) && WSTOPSIG (*wstatp) == SIGTRAP)
    {
      if (the_low_target.stopped_by_watchpoint == NULL)
	{
	  child->stopped_by_watchpoint = 0;
	}
      else
	{
	  struct thread_info *saved_inferior;

	  saved_inferior = current_inferior;
	  current_inferior = get_lwp_thread (child);

	  child->stopped_by_watchpoint
	    = the_low_target.stopped_by_watchpoint ();

	  if (child->stopped_by_watchpoint)
	    {
	      if (the_low_target.stopped_data_address != NULL)
		child->stopped_data_address
		  = the_low_target.stopped_data_address ();
	      else
		child->stopped_data_address = 0;
	    }

	  current_inferior = saved_inferior;
	}
    }

  /* Store the STOP_PC, with adjustment applied.  This depends on the
     architecture being defined already (so that CHILD has a valid
     regcache), and on LAST_STATUS being set (to check for SIGTRAP or
     not).  */
  if (WIFSTOPPED (*wstatp))
    child->stop_pc = get_stop_pc (child);

  if (debug_threads
      && WIFSTOPPED (*wstatp)
      && the_low_target.get_pc != NULL)
    {
      struct thread_info *saved_inferior = current_inferior;
      struct regcache *regcache;
      CORE_ADDR pc;

      current_inferior = get_lwp_thread (child);
      regcache = get_thread_regcache (current_inferior, 1);
      pc = (*the_low_target.get_pc) (regcache);
      fprintf (stderr, "linux_wait_for_lwp: pc is 0x%lx\n", (long) pc);
      current_inferior = saved_inferior;
    }

  return child;
}

/* Arrange for a breakpoint to be hit again later.  We don't keep the
   SIGTRAP status and don't forward the SIGTRAP signal to the LWP.  We
   will handle the current event, eventually we will resume this LWP,
   and this breakpoint will trap again.  */

static int
cancel_breakpoint (struct lwp_info *lwp)
{
  struct thread_info *saved_inferior;
  struct regcache *regcache;

  /* There's nothing to do if we don't support breakpoints.  */
  if (!supports_breakpoints ())
    return 0;

  regcache = get_thread_regcache (get_lwp_thread (lwp), 1);

  /* breakpoint_at reads from current inferior.  */
  saved_inferior = current_inferior;
  current_inferior = get_lwp_thread (lwp);

  if ((*the_low_target.breakpoint_at) (lwp->stop_pc))
    {
      if (debug_threads)
	fprintf (stderr,
		 "CB: Push back breakpoint for %s\n",
		 target_pid_to_str (lwp->head.id));

      /* Back up the PC if necessary.  */
      if (the_low_target.decr_pc_after_break)
	{
	  struct regcache *regcache
	    = get_thread_regcache (get_lwp_thread (lwp), 1);
	  (*the_low_target.set_pc) (regcache, lwp->stop_pc);
	}

      current_inferior = saved_inferior;
      return 1;
    }
  else
    {
      if (debug_threads)
	fprintf (stderr,
		 "CB: No breakpoint found at %s for [%s]\n",
		 paddress (lwp->stop_pc),
		 target_pid_to_str (lwp->head.id));
    }

  current_inferior = saved_inferior;
  return 0;
}

/* When the event-loop is doing a step-over, this points at the thread
   being stepped.  */
ptid_t step_over_bkpt;

/* Wait for an event from child PID.  If PID is -1, wait for any
   child.  Store the stop status through the status pointer WSTAT.
   OPTIONS is passed to the waitpid call.  Return 0 if no child stop
   event was found and OPTIONS contains WNOHANG.  Return the PID of
   the stopped child otherwise.  */

static int
linux_wait_for_event_1 (ptid_t ptid, int *wstat, int options)
{
  struct lwp_info *event_child, *requested_child;

  event_child = NULL;
  requested_child = NULL;

  /* Check for a lwp with a pending status.  */

  if (ptid_equal (ptid, minus_one_ptid)
      || ptid_equal (pid_to_ptid (ptid_get_pid (ptid)), ptid))
    {
      event_child = (struct lwp_info *)
	find_inferior (&all_lwps, status_pending_p_callback, &ptid);
      if (debug_threads && event_child)
	fprintf (stderr, "Got a pending child %ld\n", lwpid_of (event_child));
    }
  else
    {
      requested_child = find_lwp_pid (ptid);

      if (requested_child->status_pending_p)
	event_child = requested_child;
    }

  if (event_child != NULL)
    {
      if (debug_threads)
	fprintf (stderr, "Got an event from pending child %ld (%04x)\n",
		 lwpid_of (event_child), event_child->status_pending);
      *wstat = event_child->status_pending;
      event_child->status_pending_p = 0;
      event_child->status_pending = 0;
      current_inferior = get_lwp_thread (event_child);
      return lwpid_of (event_child);
    }

  /* We only enter this loop if no process has a pending wait status.  Thus
     any action taken in response to a wait status inside this loop is
     responding as soon as we detect the status, not after any pending
     events.  */
  while (1)
    {
      event_child = linux_wait_for_lwp (ptid, wstat, options);

      if ((options & WNOHANG) && event_child == NULL)
	{
	  if (debug_threads)
	    fprintf (stderr, "WNOHANG set, no event found\n");
	  return 0;
	}

      if (event_child == NULL)
	error ("event from unknown child");

      current_inferior = get_lwp_thread (event_child);

      /* Check for thread exit.  */
      if (! WIFSTOPPED (*wstat))
	{
	  if (debug_threads)
	    fprintf (stderr, "LWP %ld exiting\n", lwpid_of (event_child));

	  /* If the last thread is exiting, just return.  */
	  if (last_thread_of_process_p (current_inferior))
	    {
	      if (debug_threads)
		fprintf (stderr, "LWP %ld is last lwp of process\n",
			 lwpid_of (event_child));
	      return lwpid_of (event_child);
	    }

	  if (!non_stop)
	    {
	      current_inferior = (struct thread_info *) all_threads.head;
	      if (debug_threads)
		fprintf (stderr, "Current inferior is now %ld\n",
			 lwpid_of (get_thread_lwp (current_inferior)));
	    }
	  else
	    {
	      current_inferior = NULL;
	      if (debug_threads)
		fprintf (stderr, "Current inferior is now <NULL>\n");
	    }

	  /* If we were waiting for this particular child to do something...
	     well, it did something.  */
	  if (requested_child != NULL)
	    {
	      int lwpid = lwpid_of (event_child);

	      /* Cancel the step-over operation --- the thread that
		 started it is gone.  */
	      if (finish_step_over (event_child))
		unstop_all_lwps (event_child);
	      delete_lwp (event_child);
	      return lwpid;
	    }

	  delete_lwp (event_child);

	  /* Wait for a more interesting event.  */
	  continue;
	}

      if (event_child->must_set_ptrace_flags)
	{
	  ptrace (PTRACE_SETOPTIONS, lwpid_of (event_child),
		  0, (PTRACE_ARG4_TYPE) PTRACE_O_TRACECLONE);
	  event_child->must_set_ptrace_flags = 0;
	}

      if (WIFSTOPPED (*wstat) && WSTOPSIG (*wstat) == SIGTRAP
	  && *wstat >> 16 != 0)
	{
	  handle_extended_wait (event_child, *wstat);
	  continue;
	}

      /* If GDB is not interested in this signal, don't stop other
	 threads, and don't report it to GDB.  Just resume the
	 inferior right away.  We do this for threading-related
	 signals as well as any that GDB specifically requested we
	 ignore.  But never ignore SIGSTOP if we sent it ourselves,
	 and do not ignore signals when stepping - they may require
	 special handling to skip the signal handler.  */
      /* FIXME drow/2002-06-09: Get signal numbers from the inferior's
	 thread library?  */
      if (WIFSTOPPED (*wstat)
	  && !event_child->stepping
	  && (
#if defined (USE_THREAD_DB) && defined (__SIGRTMIN)
	      (current_process ()->private->thread_db != NULL
	       && (WSTOPSIG (*wstat) == __SIGRTMIN
		   || WSTOPSIG (*wstat) == __SIGRTMIN + 1))
	      ||
#endif
	      (pass_signals[target_signal_from_host (WSTOPSIG (*wstat))]
	       && !(WSTOPSIG (*wstat) == SIGSTOP
		    && event_child->stop_expected))))
	{
	  siginfo_t info, *info_p;

	  if (debug_threads)
	    fprintf (stderr, "Ignored signal %d for LWP %ld.\n",
		     WSTOPSIG (*wstat), lwpid_of (event_child));

	  if (ptrace (PTRACE_GETSIGINFO, lwpid_of (event_child), 0, &info) == 0)
	    info_p = &info;
	  else
	    info_p = NULL;
	  linux_resume_one_lwp (event_child, event_child->stepping,
				WSTOPSIG (*wstat), info_p);
	  continue;
	}

      if (WIFSTOPPED (*wstat)
	  && WSTOPSIG (*wstat) == SIGSTOP
	  && event_child->stop_expected)
	{
	  int should_stop;

	  if (debug_threads)
	    fprintf (stderr, "Expected stop.\n");
	  event_child->stop_expected = 0;

	  should_stop = (event_child->last_resume_kind == resume_stop
			 || stopping_threads);

	  if (!should_stop)
	    {
	      linux_resume_one_lwp (event_child,
				    event_child->stepping, 0, NULL);
	      continue;
	    }
	}

      return lwpid_of (event_child);
    }

  /* NOTREACHED */
  return 0;
}

static int
linux_wait_for_event (ptid_t ptid, int *wstat, int options)
{
  ptid_t wait_ptid;

  if (ptid_is_pid (ptid))
    {
      /* A request to wait for a specific tgid.  This is not possible
	 with waitpid, so instead, we wait for any child, and leave
	 children we're not interested in right now with a pending
	 status to report later.  */
      wait_ptid = minus_one_ptid;
    }
  else
    wait_ptid = ptid;

  while (1)
    {
      int event_pid;

      event_pid = linux_wait_for_event_1 (wait_ptid, wstat, options);

      if (event_pid > 0
	  && ptid_is_pid (ptid) && ptid_get_pid (ptid) != event_pid)
	{
	  struct lwp_info *event_child = find_lwp_pid (pid_to_ptid (event_pid));

	  if (! WIFSTOPPED (*wstat))
	    mark_lwp_dead (event_child, *wstat);
	  else
	    {
	      event_child->status_pending_p = 1;
	      event_child->status_pending = *wstat;
	    }
	}
      else
	return event_pid;
    }
}


/* Count the LWP's that have had events.  */

static int
count_events_callback (struct inferior_list_entry *entry, void *data)
{
  struct lwp_info *lp = (struct lwp_info *) entry;
  int *count = data;

  gdb_assert (count != NULL);

  /* Count only resumed LWPs that have a SIGTRAP event pending that
     should be reported to GDB.  */
  if (get_lwp_thread (lp)->last_status.kind == TARGET_WAITKIND_IGNORE
      && lp->last_resume_kind != resume_stop
      && lp->status_pending_p
      && WIFSTOPPED (lp->status_pending)
      && WSTOPSIG (lp->status_pending) == SIGTRAP
      && !breakpoint_inserted_here (lp->stop_pc))
    (*count)++;

  return 0;
}

/* Select the LWP (if any) that is currently being single-stepped.  */

static int
select_singlestep_lwp_callback (struct inferior_list_entry *entry, void *data)
{
  struct lwp_info *lp = (struct lwp_info *) entry;

  if (get_lwp_thread (lp)->last_status.kind == TARGET_WAITKIND_IGNORE
      && lp->last_resume_kind == resume_step
      && lp->status_pending_p)
    return 1;
  else
    return 0;
}

/* Select the Nth LWP that has had a SIGTRAP event that should be
   reported to GDB.  */

static int
select_event_lwp_callback (struct inferior_list_entry *entry, void *data)
{
  struct lwp_info *lp = (struct lwp_info *) entry;
  int *selector = data;

  gdb_assert (selector != NULL);

  /* Select only resumed LWPs that have a SIGTRAP event pending. */
  if (lp->last_resume_kind != resume_stop
      && get_lwp_thread (lp)->last_status.kind == TARGET_WAITKIND_IGNORE
      && lp->status_pending_p
      && WIFSTOPPED (lp->status_pending)
      && WSTOPSIG (lp->status_pending) == SIGTRAP
      && !breakpoint_inserted_here (lp->stop_pc))
    if ((*selector)-- == 0)
      return 1;

  return 0;
}

static int
cancel_breakpoints_callback (struct inferior_list_entry *entry, void *data)
{
  struct lwp_info *lp = (struct lwp_info *) entry;
  struct lwp_info *event_lp = data;

  /* Leave the LWP that has been elected to receive a SIGTRAP alone.  */
  if (lp == event_lp)
    return 0;

  /* If a LWP other than the LWP that we're reporting an event for has
     hit a GDB breakpoint (as opposed to some random trap signal),
     then just arrange for it to hit it again later.  We don't keep
     the SIGTRAP status and don't forward the SIGTRAP signal to the
     LWP.  We will handle the current event, eventually we will resume
     all LWPs, and this one will get its breakpoint trap again.

     If we do not do this, then we run the risk that the user will
     delete or disable the breakpoint, but the LWP will have already
     tripped on it.  */

  if (lp->last_resume_kind != resume_stop
      && get_lwp_thread (lp)->last_status.kind == TARGET_WAITKIND_IGNORE
      && lp->status_pending_p
      && WIFSTOPPED (lp->status_pending)
      && WSTOPSIG (lp->status_pending) == SIGTRAP
      && !lp->stepping
      && !lp->stopped_by_watchpoint
      && cancel_breakpoint (lp))
    /* Throw away the SIGTRAP.  */
    lp->status_pending_p = 0;

  return 0;
}

/* Select one LWP out of those that have events pending.  */

static void
select_event_lwp (struct lwp_info **orig_lp)
{
  int num_events = 0;
  int random_selector;
  struct lwp_info *event_lp;

  /* Give preference to any LWP that is being single-stepped.  */
  event_lp
    = (struct lwp_info *) find_inferior (&all_lwps,
					 select_singlestep_lwp_callback, NULL);
  if (event_lp != NULL)
    {
      if (debug_threads)
	fprintf (stderr,
		 "SEL: Select single-step %s\n",
		 target_pid_to_str (ptid_of (event_lp)));
    }
  else
    {
      /* No single-stepping LWP.  Select one at random, out of those
         which have had SIGTRAP events.  */

      /* First see how many SIGTRAP events we have.  */
      find_inferior (&all_lwps, count_events_callback, &num_events);

      /* Now randomly pick a LWP out of those that have had a SIGTRAP.  */
      random_selector = (int)
	((num_events * (double) rand ()) / (RAND_MAX + 1.0));

      if (debug_threads && num_events > 1)
	fprintf (stderr,
		 "SEL: Found %d SIGTRAP events, selecting #%d\n",
		 num_events, random_selector);

      event_lp = (struct lwp_info *) find_inferior (&all_lwps,
						    select_event_lwp_callback,
						    &random_selector);
    }

  if (event_lp != NULL)
    {
      /* Switch the event LWP.  */
      *orig_lp = event_lp;
    }
}

/* Set this inferior LWP's state as "want-stopped".  We won't resume
   this LWP until the client gives us another action for it.  */

static void
gdb_wants_lwp_stopped (struct inferior_list_entry *entry)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;
  struct thread_info *thread = get_lwp_thread (lwp);

  /* Most threads are stopped implicitly (all-stop); tag that with
     signal 0.  The thread being explicitly reported stopped to the
     client, gets it's status fixed up afterwards.  */
  thread->last_status.kind = TARGET_WAITKIND_STOPPED;
  thread->last_status.value.sig = TARGET_SIGNAL_0;

  lwp->last_resume_kind = resume_stop;
}

/* Set all LWP's states as "want-stopped".  */

static void
gdb_wants_all_stopped (void)
{
  for_each_inferior (&all_lwps, gdb_wants_lwp_stopped);
}

/* Wait for process, returns status.  */

static ptid_t
linux_wait_1 (ptid_t ptid,
	      struct target_waitstatus *ourstatus, int target_options)
{
  int w;
  struct thread_info *thread = NULL;
  struct lwp_info *event_child = NULL;
  int options;
  int pid;
  int step_over_finished;
  int bp_explains_trap;
  int maybe_internal_trap;
  int report_to_gdb;

  /* Translate generic target options into linux options.  */
  options = __WALL;
  if (target_options & TARGET_WNOHANG)
    options |= WNOHANG;

retry:
  ourstatus->kind = TARGET_WAITKIND_IGNORE;

  /* If we were only supposed to resume one thread, only wait for
     that thread - if it's still alive.  If it died, however - which
     can happen if we're coming from the thread death case below -
     then we need to make sure we restart the other threads.  We could
     pick a thread at random or restart all; restarting all is less
     arbitrary.  */
  if (!non_stop
      && !ptid_equal (cont_thread, null_ptid)
      && !ptid_equal (cont_thread, minus_one_ptid))
    {
      thread = (struct thread_info *) find_inferior_id (&all_threads,
							cont_thread);

      /* No stepping, no signal - unless one is pending already, of course.  */
      if (thread == NULL)
	{
	  struct thread_resume resume_info;
	  resume_info.thread = minus_one_ptid;
	  resume_info.kind = resume_continue;
	  resume_info.sig = 0;
	  linux_resume (&resume_info, 1);
	}
      else
	ptid = cont_thread;
    }

  if (ptid_equal (step_over_bkpt, null_ptid))
    pid = linux_wait_for_event (ptid, &w, options);
  else
    {
      if (debug_threads)
	fprintf (stderr, "step_over_bkpt set [%s], doing a blocking wait\n",
		 target_pid_to_str (step_over_bkpt));
      pid = linux_wait_for_event (step_over_bkpt, &w, options & ~WNOHANG);
    }

  if (pid == 0) /* only if TARGET_WNOHANG */
    return null_ptid;

  event_child = get_thread_lwp (current_inferior);

  /* If we are waiting for a particular child, and it exited,
     linux_wait_for_event will return its exit status.  Similarly if
     the last child exited.  If this is not the last child, however,
     do not report it as exited until there is a 'thread exited' response
     available in the remote protocol.  Instead, just wait for another event.
     This should be safe, because if the thread crashed we will already
     have reported the termination signal to GDB; that should stop any
     in-progress stepping operations, etc.

     Report the exit status of the last thread to exit.  This matches
     LinuxThreads' behavior.  */

  if (last_thread_of_process_p (current_inferior))
    {
      if (WIFEXITED (w) || WIFSIGNALED (w))
	{
	  int pid = pid_of (event_child);
	  struct process_info *process = find_process_pid (pid);

#ifdef USE_THREAD_DB
	  thread_db_free (process, 0);
#endif
	  delete_lwp (event_child);
	  linux_remove_process (process);

	  current_inferior = NULL;

	  if (WIFEXITED (w))
	    {
	      ourstatus->kind = TARGET_WAITKIND_EXITED;
	      ourstatus->value.integer = WEXITSTATUS (w);

	      if (debug_threads)
		fprintf (stderr, "\nChild exited with retcode = %x \n", WEXITSTATUS (w));
	    }
	  else
	    {
	      ourstatus->kind = TARGET_WAITKIND_SIGNALLED;
	      ourstatus->value.sig = target_signal_from_host (WTERMSIG (w));

	      if (debug_threads)
		fprintf (stderr, "\nChild terminated with signal = %x \n", WTERMSIG (w));

	    }

	  return pid_to_ptid (pid);
	}
    }
  else
    {
      if (!WIFSTOPPED (w))
	goto retry;
    }

  /* If this event was not handled before, and is not a SIGTRAP, we
     report it.  SIGILL and SIGSEGV are also treated as traps in case
     a breakpoint is inserted at the current PC.  If this target does
     not support internal breakpoints at all, we also report the
     SIGTRAP without further processing; it's of no concern to us.  */
  maybe_internal_trap
    = (supports_breakpoints ()
       && (WSTOPSIG (w) == SIGTRAP
	   || ((WSTOPSIG (w) == SIGILL
		|| WSTOPSIG (w) == SIGSEGV)
	       && (*the_low_target.breakpoint_at) (event_child->stop_pc))));

  if (maybe_internal_trap)
    {
      /* Handle anything that requires bookkeeping before deciding to
	 report the event or continue waiting.  */

      /* First check if we can explain the SIGTRAP with an internal
	 breakpoint, or if we should possibly report the event to GDB.
	 Do this before anything that may remove or insert a
	 breakpoint.  */
      bp_explains_trap = breakpoint_inserted_here (event_child->stop_pc);

      /* We have a SIGTRAP, possibly a step-over dance has just
	 finished.  If so, tweak the state machine accordingly,
	 reinsert breakpoints and delete any reinsert (software
	 single-step) breakpoints.  */
      step_over_finished = finish_step_over (event_child);

      /* Now invoke the callbacks of any internal breakpoints there.  */
      check_breakpoints (event_child->stop_pc);

      if (bp_explains_trap)
	{
	  /* If we stepped or ran into an internal breakpoint, we've
	     already handled it.  So next time we resume (from this
	     PC), we should step over it.  */
	  if (debug_threads)
	    fprintf (stderr, "Hit a gdbserver breakpoint.\n");

	  if (breakpoint_here (event_child->stop_pc))
	    event_child->need_step_over = 1;
	}
    }
  else
    {
      /* We have some other signal, possibly a step-over dance was in
	 progress, and it should be cancelled too.  */
      step_over_finished = finish_step_over (event_child);
    }

  /* We have all the data we need.  Either report the event to GDB, or
     resume threads and keep waiting for more.  */

  /* Check If GDB would be interested in this event.  If GDB wanted
     this thread to single step, we always want to report the SIGTRAP,
     and let GDB handle it.  Watchpoints should always be reported.
     So should signals we can't explain.  A SIGTRAP we can't explain
     could be a GDB breakpoint --- we may or not support Z0
     breakpoints.  If we do, we're be able to handle GDB breakpoints
     on top of internal breakpoints, by handling the internal
     breakpoint and still reporting the event to GDB.  If we don't,
     we're out of luck, GDB won't see the breakpoint hit.  */
  report_to_gdb = (!maybe_internal_trap
		   || event_child->last_resume_kind == resume_step
		   || event_child->stopped_by_watchpoint
		   || (!step_over_finished && !bp_explains_trap)
		   || gdb_breakpoint_here (event_child->stop_pc));

  /* We found no reason GDB would want us to stop.  We either hit one
     of our own breakpoints, or finished an internal step GDB
     shouldn't know about.  */
  if (!report_to_gdb)
    {
      if (debug_threads)
	{
	  if (bp_explains_trap)
	    fprintf (stderr, "Hit a gdbserver breakpoint.\n");
	  if (step_over_finished)
	    fprintf (stderr, "Step-over finished.\n");
	}

      /* We're not reporting this breakpoint to GDB, so apply the
	 decr_pc_after_break adjustment to the inferior's regcache
	 ourselves.  */

      if (the_low_target.set_pc != NULL)
	{
	  struct regcache *regcache
	    = get_thread_regcache (get_lwp_thread (event_child), 1);
	  (*the_low_target.set_pc) (regcache, event_child->stop_pc);
	}

      /* We've finished stepping over a breakpoint.  We've stopped all
	 LWPs momentarily except the stepping one.  This is where we
	 resume them all again.  We're going to keep waiting, so use
	 proceed, which handles stepping over the next breakpoint.  */
      if (debug_threads)
	fprintf (stderr, "proceeding all threads.\n");
      proceed_all_lwps ();
      goto retry;
    }

  if (debug_threads)
    {
      if (event_child->last_resume_kind == resume_step)
	fprintf (stderr, "GDB wanted to single-step, reporting event.\n");
      if (event_child->stopped_by_watchpoint)
	fprintf (stderr, "Stopped by watchpoint.\n");
      if (gdb_breakpoint_here (event_child->stop_pc))
	fprintf (stderr, "Stopped by GDB breakpoint.\n");
      if (debug_threads)
	fprintf (stderr, "Hit a non-gdbserver trap event.\n");
    }

  /* Alright, we're going to report a stop.  */

  if (!non_stop)
    {
      /* In all-stop, stop all threads.  */
      stop_all_lwps ();

      /* If we're not waiting for a specific LWP, choose an event LWP
	 from among those that have had events.  Giving equal priority
	 to all LWPs that have had events helps prevent
	 starvation.  */
      if (ptid_equal (ptid, minus_one_ptid))
	{
	  event_child->status_pending_p = 1;
	  event_child->status_pending = w;

	  select_event_lwp (&event_child);

	  event_child->status_pending_p = 0;
	  w = event_child->status_pending;
	}

      /* Now that we've selected our final event LWP, cancel any
	 breakpoints in other LWPs that have hit a GDB breakpoint.
	 See the comment in cancel_breakpoints_callback to find out
	 why.  */
      find_inferior (&all_lwps, cancel_breakpoints_callback, event_child);
    }
  else
    {
      /* If we just finished a step-over, then all threads had been
	 momentarily paused.  In all-stop, that's fine, we want
	 threads stopped by now anyway.  In non-stop, we need to
	 re-resume threads that GDB wanted to be running.  */
      if (step_over_finished)
	unstop_all_lwps (event_child);
    }

  ourstatus->kind = TARGET_WAITKIND_STOPPED;

  /* Do this before the gdb_wants_all_stopped calls below, since they
     always set last_resume_kind to resume_stop.  */
  if (event_child->last_resume_kind == resume_stop && WSTOPSIG (w) == SIGSTOP)
    {
      /* A thread that has been requested to stop by GDB with vCont;t,
	 and it stopped cleanly, so report as SIG0.  The use of
	 SIGSTOP is an implementation detail.  */
      ourstatus->value.sig = TARGET_SIGNAL_0;
    }
  else if (event_child->last_resume_kind == resume_stop && WSTOPSIG (w) != SIGSTOP)
    {
      /* A thread that has been requested to stop by GDB with vCont;t,
	 but, it stopped for other reasons.  */
      ourstatus->value.sig = target_signal_from_host (WSTOPSIG (w));
    }
  else
    {
      ourstatus->value.sig = target_signal_from_host (WSTOPSIG (w));
    }

  gdb_assert (ptid_equal (step_over_bkpt, null_ptid));

  if (!non_stop)
    {
      /* From GDB's perspective, all-stop mode always stops all
	 threads implicitly.  Tag all threads as "want-stopped".  */
      gdb_wants_all_stopped ();
    }
  else
    {
      /* We're reporting this LWP as stopped.  Update it's
      	 "want-stopped" state to what the client wants, until it gets
      	 a new resume action.  */
      gdb_wants_lwp_stopped (&event_child->head);
    }

  if (debug_threads)
    fprintf (stderr, "linux_wait ret = %s, %d, %d\n",
	     target_pid_to_str (ptid_of (event_child)),
	     ourstatus->kind,
	     ourstatus->value.sig);

  get_lwp_thread (event_child)->last_status = *ourstatus;
  return ptid_of (event_child);
}

/* Get rid of any pending event in the pipe.  */
static void
async_file_flush (void)
{
  int ret;
  char buf;

  do
    ret = read (linux_event_pipe[0], &buf, 1);
  while (ret >= 0 || (ret == -1 && errno == EINTR));
}

/* Put something in the pipe, so the event loop wakes up.  */
static void
async_file_mark (void)
{
  int ret;

  async_file_flush ();

  do
    ret = write (linux_event_pipe[1], "+", 1);
  while (ret == 0 || (ret == -1 && errno == EINTR));

  /* Ignore EAGAIN.  If the pipe is full, the event loop will already
     be awakened anyway.  */
}

static ptid_t
linux_wait (ptid_t ptid,
	    struct target_waitstatus *ourstatus, int target_options)
{
  ptid_t event_ptid;

  if (debug_threads)
    fprintf (stderr, "linux_wait: [%s]\n", target_pid_to_str (ptid));

  /* Flush the async file first.  */
  if (target_is_async_p ())
    async_file_flush ();

  event_ptid = linux_wait_1 (ptid, ourstatus, target_options);

  /* If at least one stop was reported, there may be more.  A single
     SIGCHLD can signal more than one child stop.  */
  if (target_is_async_p ()
      && (target_options & TARGET_WNOHANG) != 0
      && !ptid_equal (event_ptid, null_ptid))
    async_file_mark ();

  return event_ptid;
}

/* Send a signal to an LWP.  */

static int
kill_lwp (unsigned long lwpid, int signo)
{
  /* Use tkill, if possible, in case we are using nptl threads.  If tkill
     fails, then we are not using nptl threads and we should be using kill.  */

#ifdef __NR_tkill
  {
    static int tkill_failed;

    if (!tkill_failed)
      {
	int ret;

	errno = 0;
	ret = syscall (__NR_tkill, lwpid, signo);
	if (errno != ENOSYS)
	  return ret;
	tkill_failed = 1;
      }
  }
#endif

  return kill (lwpid, signo);
}

static void
send_sigstop (struct inferior_list_entry *entry)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;
  int pid;

  if (lwp->stopped)
    return;

  pid = lwpid_of (lwp);

  /* If we already have a pending stop signal for this process, don't
     send another.  */
  if (lwp->stop_expected)
    {
      if (debug_threads)
	fprintf (stderr, "Have pending sigstop for lwp %d\n", pid);

      return;
    }

  if (debug_threads)
    fprintf (stderr, "Sending sigstop to lwp %d\n", pid);

  lwp->stop_expected = 1;
  kill_lwp (pid, SIGSTOP);
}

static void
mark_lwp_dead (struct lwp_info *lwp, int wstat)
{
  /* It's dead, really.  */
  lwp->dead = 1;

  /* Store the exit status for later.  */
  lwp->status_pending_p = 1;
  lwp->status_pending = wstat;

  /* Prevent trying to stop it.  */
  lwp->stopped = 1;

  /* No further stops are expected from a dead lwp.  */
  lwp->stop_expected = 0;
}

static void
wait_for_sigstop (struct inferior_list_entry *entry)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;
  struct thread_info *saved_inferior;
  int wstat;
  ptid_t saved_tid;
  ptid_t ptid;
  int pid;

  if (lwp->stopped)
    {
      if (debug_threads)
	fprintf (stderr, "wait_for_sigstop: LWP %ld already stopped\n",
		 lwpid_of (lwp));
      return;
    }

  saved_inferior = current_inferior;
  if (saved_inferior != NULL)
    saved_tid = ((struct inferior_list_entry *) saved_inferior)->id;
  else
    saved_tid = null_ptid; /* avoid bogus unused warning */

  ptid = lwp->head.id;

  if (debug_threads)
    fprintf (stderr, "wait_for_sigstop: pulling one event\n");

  pid = linux_wait_for_event (ptid, &wstat, __WALL);

  /* If we stopped with a non-SIGSTOP signal, save it for later
     and record the pending SIGSTOP.  If the process exited, just
     return.  */
  if (WIFSTOPPED (wstat))
    {
      if (debug_threads)
	fprintf (stderr, "LWP %ld stopped with signal %d\n",
		 lwpid_of (lwp), WSTOPSIG (wstat));

      if (WSTOPSIG (wstat) != SIGSTOP)
	{
	  if (debug_threads)
	    fprintf (stderr, "LWP %ld stopped with non-sigstop status %06x\n",
		     lwpid_of (lwp), wstat);

	  lwp->status_pending_p = 1;
	  lwp->status_pending = wstat;
	}
    }
  else
    {
      if (debug_threads)
	fprintf (stderr, "Process %d exited while stopping LWPs\n", pid);

      lwp = find_lwp_pid (pid_to_ptid (pid));
      if (lwp)
	{
	  /* Leave this status pending for the next time we're able to
	     report it.  In the mean time, we'll report this lwp as
	     dead to GDB, so GDB doesn't try to read registers and
	     memory from it.  This can only happen if this was the
	     last thread of the process; otherwise, PID is removed
	     from the thread tables before linux_wait_for_event
	     returns.  */
	  mark_lwp_dead (lwp, wstat);
	}
    }

  if (saved_inferior == NULL || linux_thread_alive (saved_tid))
    current_inferior = saved_inferior;
  else
    {
      if (debug_threads)
	fprintf (stderr, "Previously current thread died.\n");

      if (non_stop)
	{
	  /* We can't change the current inferior behind GDB's back,
	     otherwise, a subsequent command may apply to the wrong
	     process.  */
	  current_inferior = NULL;
	}
      else
	{
	  /* Set a valid thread as current.  */
	  set_desired_inferior (0);
	}
    }
}

static void
stop_all_lwps (void)
{
  stopping_threads = 1;
  for_each_inferior (&all_lwps, send_sigstop);
  for_each_inferior (&all_lwps, wait_for_sigstop);
  stopping_threads = 0;
}

/* Resume execution of the inferior process.
   If STEP is nonzero, single-step it.
   If SIGNAL is nonzero, give it that signal.  */

static void
linux_resume_one_lwp (struct lwp_info *lwp,
		      int step, int signal, siginfo_t *info)
{
  struct thread_info *saved_inferior;

  if (lwp->stopped == 0)
    return;

  /* If we have pending signals or status, and a new signal, enqueue the
     signal.  Also enqueue the signal if we are waiting to reinsert a
     breakpoint; it will be picked up again below.  */
  if (signal != 0
      && (lwp->status_pending_p || lwp->pending_signals != NULL
	  || lwp->bp_reinsert != 0))
    {
      struct pending_signals *p_sig;
      p_sig = xmalloc (sizeof (*p_sig));
      p_sig->prev = lwp->pending_signals;
      p_sig->signal = signal;
      if (info == NULL)
	memset (&p_sig->info, 0, sizeof (siginfo_t));
      else
	memcpy (&p_sig->info, info, sizeof (siginfo_t));
      lwp->pending_signals = p_sig;
    }

  if (lwp->status_pending_p)
    {
      if (debug_threads)
	fprintf (stderr, "Not resuming lwp %ld (%s, signal %d, stop %s);"
		 " has pending status\n",
		 lwpid_of (lwp), step ? "step" : "continue", signal,
		 lwp->stop_expected ? "expected" : "not expected");
      return;
    }

  saved_inferior = current_inferior;
  current_inferior = get_lwp_thread (lwp);

  if (debug_threads)
    fprintf (stderr, "Resuming lwp %ld (%s, signal %d, stop %s)\n",
	     lwpid_of (lwp), step ? "step" : "continue", signal,
	     lwp->stop_expected ? "expected" : "not expected");

  /* This bit needs some thinking about.  If we get a signal that
     we must report while a single-step reinsert is still pending,
     we often end up resuming the thread.  It might be better to
     (ew) allow a stack of pending events; then we could be sure that
     the reinsert happened right away and not lose any signals.

     Making this stack would also shrink the window in which breakpoints are
     uninserted (see comment in linux_wait_for_lwp) but not enough for
     complete correctness, so it won't solve that problem.  It may be
     worthwhile just to solve this one, however.  */
  if (lwp->bp_reinsert != 0)
    {
      if (debug_threads)
	fprintf (stderr, "  pending reinsert at 0x%s\n",
		 paddress (lwp->bp_reinsert));

      if (lwp->bp_reinsert != 0 && can_hardware_single_step ())
	{
	  if (step == 0)
	    fprintf (stderr, "BAD - reinserting but not stepping.\n");

	  step = 1;
	}

      /* Postpone any pending signal.  It was enqueued above.  */
      signal = 0;
    }

  if (debug_threads && the_low_target.get_pc != NULL)
    {
      struct regcache *regcache = get_thread_regcache (current_inferior, 1);
      CORE_ADDR pc = (*the_low_target.get_pc) (regcache);
      fprintf (stderr, "  resuming from pc 0x%lx\n", (long) pc);
    }

  /* If we have pending signals, consume one unless we are trying to reinsert
     a breakpoint.  */
  if (lwp->pending_signals != NULL && lwp->bp_reinsert == 0)
    {
      struct pending_signals **p_sig;

      p_sig = &lwp->pending_signals;
      while ((*p_sig)->prev != NULL)
	p_sig = &(*p_sig)->prev;

      signal = (*p_sig)->signal;
      if ((*p_sig)->info.si_signo != 0)
	ptrace (PTRACE_SETSIGINFO, lwpid_of (lwp), 0, &(*p_sig)->info);

      free (*p_sig);
      *p_sig = NULL;
    }

  if (the_low_target.prepare_to_resume != NULL)
    the_low_target.prepare_to_resume (lwp);

  regcache_invalidate_one ((struct inferior_list_entry *)
			   get_lwp_thread (lwp));
  errno = 0;
  lwp->stopped = 0;
  lwp->stopped_by_watchpoint = 0;
  lwp->stepping = step;
  ptrace (step ? PTRACE_SINGLESTEP : PTRACE_CONT, lwpid_of (lwp), 0,
	  /* Coerce to a uintptr_t first to avoid potential gcc warning
	     of coercing an 8 byte integer to a 4 byte pointer.  */
	  (PTRACE_ARG4_TYPE) (uintptr_t) signal);

  current_inferior = saved_inferior;
  if (errno)
    {
      /* ESRCH from ptrace either means that the thread was already
	 running (an error) or that it is gone (a race condition).  If
	 it's gone, we will get a notification the next time we wait,
	 so we can ignore the error.  We could differentiate these
	 two, but it's tricky without waiting; the thread still exists
	 as a zombie, so sending it signal 0 would succeed.  So just
	 ignore ESRCH.  */
      if (errno == ESRCH)
	return;

      perror_with_name ("ptrace");
    }
}

struct thread_resume_array
{
  struct thread_resume *resume;
  size_t n;
};

/* This function is called once per thread.  We look up the thread
   in RESUME_PTR, and mark the thread with a pointer to the appropriate
   resume request.

   This algorithm is O(threads * resume elements), but resume elements
   is small (and will remain small at least until GDB supports thread
   suspension).  */
static int
linux_set_resume_request (struct inferior_list_entry *entry, void *arg)
{
  struct lwp_info *lwp;
  struct thread_info *thread;
  int ndx;
  struct thread_resume_array *r;

  thread = (struct thread_info *) entry;
  lwp = get_thread_lwp (thread);
  r = arg;

  for (ndx = 0; ndx < r->n; ndx++)
    {
      ptid_t ptid = r->resume[ndx].thread;
      if (ptid_equal (ptid, minus_one_ptid)
	  || ptid_equal (ptid, entry->id)
	  || (ptid_is_pid (ptid)
	      && (ptid_get_pid (ptid) == pid_of (lwp)))
	  || (ptid_get_lwp (ptid) == -1
	      && (ptid_get_pid (ptid) == pid_of (lwp))))
	{
	  if (r->resume[ndx].kind == resume_stop
	      && lwp->last_resume_kind == resume_stop)
	    {
	      if (debug_threads)
		fprintf (stderr, "already %s LWP %ld at GDB's request\n",
			 thread->last_status.kind == TARGET_WAITKIND_STOPPED
			 ? "stopped"
			 : "stopping",
			 lwpid_of (lwp));

	      continue;
	    }

	  lwp->resume = &r->resume[ndx];
	  lwp->last_resume_kind = lwp->resume->kind;
	  return 0;
	}
    }

  /* No resume action for this thread.  */
  lwp->resume = NULL;

  return 0;
}


/* Set *FLAG_P if this lwp has an interesting status pending.  */
static int
resume_status_pending_p (struct inferior_list_entry *entry, void *flag_p)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;

  /* LWPs which will not be resumed are not interesting, because
     we might not wait for them next time through linux_wait.  */
  if (lwp->resume == NULL)
    return 0;

  if (lwp->status_pending_p)
    * (int *) flag_p = 1;

  return 0;
}

/* Return 1 if this lwp that GDB wants running is stopped at an
   internal breakpoint that we need to step over.  It assumes that any
   required STOP_PC adjustment has already been propagated to the
   inferior's regcache.  */

static int
need_step_over_p (struct inferior_list_entry *entry, void *dummy)
{
  struct lwp_info *lwp = (struct lwp_info *) entry;
  struct thread_info *saved_inferior;
  CORE_ADDR pc;

  /* LWPs which will not be resumed are not interesting, because we
     might not wait for them next time through linux_wait.  */

  if (!lwp->stopped)
    {
      if (debug_threads)
	fprintf (stderr,
		 "Need step over [LWP %ld]? Ignoring, not stopped\n",
		 lwpid_of (lwp));
      return 0;
    }

  if (lwp->last_resume_kind == resume_stop)
    {
      if (debug_threads)
	fprintf (stderr,
		 "Need step over [LWP %ld]? Ignoring, should remain stopped\n",
		 lwpid_of (lwp));
      return 0;
    }

  if (!lwp->need_step_over)
    {
      if (debug_threads)
	fprintf (stderr,
		 "Need step over [LWP %ld]? No\n", lwpid_of (lwp));
    }

  if (lwp->status_pending_p)
    {
      if (debug_threads)
	fprintf (stderr,
		 "Need step over [LWP %ld]? Ignoring, has pending status.\n",
		 lwpid_of (lwp));
      return 0;
    }

  /* Note: PC, not STOP_PC.  Either GDB has adjusted the PC already,
     or we have.  */
  pc = get_pc (lwp);

  /* If the PC has changed since we stopped, then don't do anything,
     and let the breakpoint/tracepoint be hit.  This happens if, for
     instance, GDB handled the decr_pc_after_break subtraction itself,
     GDB is OOL stepping this thread, or the user has issued a "jump"
     command, or poked thread's registers herself.  */
  if (pc != lwp->stop_pc)
    {
      if (debug_threads)
	fprintf (stderr,
		 "Need step over [LWP %ld]? Cancelling, PC was changed.  "
		 "Old stop_pc was 0x%s, PC is now 0x%s\n",
		 lwpid_of (lwp), paddress (lwp->stop_pc), paddress (pc));

      lwp->need_step_over = 0;
      return 0;
    }

  saved_inferior = current_inferior;
  current_inferior = get_lwp_thread (lwp);

  /* We can only step over breakpoints we know about.  */
  if (breakpoint_here (pc))
    {
      /* Don't step over a breakpoint that GDB expects to hit
	 though.  */
      if (gdb_breakpoint_here (pc))
	{
	  if (debug_threads)
	    fprintf (stderr,
		     "Need step over [LWP %ld]? yes, but found"
		     " GDB breakpoint at 0x%s; skipping step over\n",
		     lwpid_of (lwp), paddress (pc));

	  current_inferior = saved_inferior;
	  return 0;
	}
      else
	{
	  if (debug_threads)
	    fprintf (stderr,
		     "Need step over [LWP %ld]? yes, found breakpoint at 0x%s\n",
		     lwpid_of (lwp), paddress (pc));

	  /* We've found an lwp that needs stepping over --- return 1 so
	     that find_inferior stops looking.  */
	  current_inferior = saved_inferior;

	  /* If the step over is cancelled, this is set again.  */
	  lwp->need_step_over = 0;
	  return 1;
	}
    }

  current_inferior = saved_inferior;

  if (debug_threads)
    fprintf (stderr,
	     "Need step over [LWP %ld]? No, no breakpoint found at 0x%s\n",
	     lwpid_of (lwp), paddress (pc));

  return 0;
}

/* Start a step-over operation on LWP.  When LWP stopped at a
   breakpoint, to make progress, we need to remove the breakpoint out
   of the way.  If we let other threads run while we do that, they may
   pass by the breakpoint location and miss hitting it.  To avoid
   that, a step-over momentarily stops all threads while LWP is
   single-stepped while the breakpoint is temporarily uninserted from
   the inferior.  When the single-step finishes, we reinsert the
   breakpoint, and let all threads that are supposed to be running,
   run again.

   On targets that don't support hardware single-step, we don't
   currently support full software single-stepping.  Instead, we only
   support stepping over the thread event breakpoint, by asking the
   low target where to place a reinsert breakpoint.  Since this
   routine assumes the breakpoint being stepped over is a thread event
   breakpoint, it usually assumes the return address of the current
   function is a good enough place to set the reinsert breakpoint.  */

static int
start_step_over (struct lwp_info *lwp)
{
  struct thread_info *saved_inferior;
  CORE_ADDR pc;
  int step;

  if (debug_threads)
    fprintf (stderr,
	     "Starting step-over on LWP %ld.  Stopping all threads\n",
	     lwpid_of (lwp));

  stop_all_lwps ();

  if (debug_threads)
    fprintf (stderr, "Done stopping all threads for step-over.\n");

  /* Note, we should always reach here with an already adjusted PC,
     either by GDB (if we're resuming due to GDB's request), or by our
     caller, if we just finished handling an internal breakpoint GDB
     shouldn't care about.  */
  pc = get_pc (lwp);

  saved_inferior = current_inferior;
  current_inferior = get_lwp_thread (lwp);

  lwp->bp_reinsert = pc;
  uninsert_breakpoints_at (pc);

  if (can_hardware_single_step ())
    {
      step = 1;
    }
  else
    {
      CORE_ADDR raddr = (*the_low_target.breakpoint_reinsert_addr) ();
      set_reinsert_breakpoint (raddr);
      step = 0;
    }

  current_inferior = saved_inferior;

  linux_resume_one_lwp (lwp, step, 0, NULL);

  /* Require next event from this LWP.  */
  step_over_bkpt = lwp->head.id;
  return 1;
}

/* Finish a step-over.  Reinsert the breakpoint we had uninserted in
   start_step_over, if still there, and delete any reinsert
   breakpoints we've set, on non hardware single-step targets.  */

static int
finish_step_over (struct lwp_info *lwp)
{
  if (lwp->bp_reinsert != 0)
    {
      if (debug_threads)
	fprintf (stderr, "Finished step over.\n");

      /* Reinsert any breakpoint at LWP->BP_REINSERT.  Note that there
	 may be no breakpoint to reinsert there by now.  */
      reinsert_breakpoints_at (lwp->bp_reinsert);

      lwp->bp_reinsert = 0;

      /* Delete any software-single-step reinsert breakpoints.  No
	 longer needed.  We don't have to worry about other threads
	 hitting this trap, and later not being able to explain it,
	 because we were stepping over a breakpoint, and we hold all
	 threads but LWP stopped while doing that.  */
      if (!can_hardware_single_step ())
	delete_reinsert_breakpoints ();

      step_over_bkpt = null_ptid;
      return 1;
    }
  else
    return 0;
}

/* This function is called once per thread.  We check the thread's resume
   request, which will tell us whether to resume, step, or leave the thread
   stopped; and what signal, if any, it should be sent.

   For threads which we aren't explicitly told otherwise, we preserve
   the stepping flag; this is used for stepping over gdbserver-placed
   breakpoints.

   If pending_flags was set in any thread, we queue any needed
   signals, since we won't actually resume.  We already have a pending
   event to report, so we don't need to preserve any step requests;
   they should be re-issued if necessary.  */

static int
linux_resume_one_thread (struct inferior_list_entry *entry, void *arg)
{
  struct lwp_info *lwp;
  struct thread_info *thread;
  int step;
  int leave_all_stopped = * (int *) arg;
  int leave_pending;

  thread = (struct thread_info *) entry;
  lwp = get_thread_lwp (thread);

  if (lwp->resume == NULL)
    return 0;

  if (lwp->resume->kind == resume_stop)
    {
      if (debug_threads)
	fprintf (stderr, "resume_stop request for LWP %ld\n", lwpid_of (lwp));

      if (!lwp->stopped)
	{
	  if (debug_threads)
	    fprintf (stderr, "stopping LWP %ld\n", lwpid_of (lwp));

	  /* Stop the thread, and wait for the event asynchronously,
	     through the event loop.  */
	  send_sigstop (&lwp->head);
	}
      else
	{
	  if (debug_threads)
	    fprintf (stderr, "already stopped LWP %ld\n",
		     lwpid_of (lwp));

	  /* The LWP may have been stopped in an internal event that
	     was not meant to be notified back to GDB (e.g., gdbserver
	     breakpoint), so we should be reporting a stop event in
	     this case too.  */

	  /* If the thread already has a pending SIGSTOP, this is a
	     no-op.  Otherwise, something later will presumably resume
	     the thread and this will cause it to cancel any pending
	     operation, due to last_resume_kind == resume_stop.  If
	     the thread already has a pending status to report, we
	     will still report it the next time we wait - see
	     status_pending_p_callback.  */
	  send_sigstop (&lwp->head);
	}

      /* For stop requests, we're done.  */
      lwp->resume = NULL;
      get_lwp_thread (lwp)->last_status.kind = TARGET_WAITKIND_IGNORE;
      return 0;
    }

  /* If this thread which is about to be resumed has a pending status,
     then don't resume any threads - we can just report the pending
     status.  Make sure to queue any signals that would otherwise be
     sent.  In all-stop mode, we do this decision based on if *any*
     thread has a pending status.  If there's a thread that needs the
     step-over-breakpoint dance, then don't resume any other thread
     but that particular one.  */
  leave_pending = (lwp->status_pending_p || leave_all_stopped);

  if (!leave_pending)
    {
      if (debug_threads)
	fprintf (stderr, "resuming LWP %ld\n", lwpid_of (lwp));

      step = (lwp->resume->kind == resume_step);
      linux_resume_one_lwp (lwp, step, lwp->resume->sig, NULL);
      get_lwp_thread (lwp)->last_status.kind = TARGET_WAITKIND_IGNORE;
    }
  else
    {
      if (debug_threads)
	fprintf (stderr, "leaving LWP %ld stopped\n", lwpid_of (lwp));

      /* If we have a new signal, enqueue the signal.  */
      if (lwp->resume->sig != 0)
	{
	  struct pending_signals *p_sig;
	  p_sig = xmalloc (sizeof (*p_sig));
	  p_sig->prev = lwp->pending_signals;
	  p_sig->signal = lwp->resume->sig;
	  memset (&p_sig->info, 0, sizeof (siginfo_t));

	  /* If this is the same signal we were previously stopped by,
	     make sure to queue its siginfo.  We can ignore the return
	     value of ptrace; if it fails, we'll skip
	     PTRACE_SETSIGINFO.  */
	  if (WIFSTOPPED (lwp->last_status)
	      && WSTOPSIG (lwp->last_status) == lwp->resume->sig)
	    ptrace (PTRACE_GETSIGINFO, lwpid_of (lwp), 0, &p_sig->info);

	  lwp->pending_signals = p_sig;
	}
    }

  lwp->resume = NULL;
  return 0;
}

static void
linux_resume (struct thread_resume *resume_info, size_t n)
{
  struct thread_resume_array array = { resume_info, n };
  struct lwp_info *need_step_over = NULL;
  int any_pending;
  int leave_all_stopped;

  find_inferior (&all_threads, linux_set_resume_request, &array);

  /* If there is a thread which would otherwise be resumed, which has
     a pending status, then don't resume any threads - we can just
     report the pending status.  Make sure to queue any signals that
     would otherwise be sent.  In non-stop mode, we'll apply this
     logic to each thread individually.  We consume all pending events
     before considering to start a step-over (in all-stop).  */
  any_pending = 0;
  if (!non_stop)
    find_inferior (&all_lwps, resume_status_pending_p, &any_pending);

  /* If there is a thread which would otherwise be resumed, which is
     stopped at a breakpoint that needs stepping over, then don't
     resume any threads - have it step over the breakpoint with all
     other threads stopped, then resume all threads again.  Make sure
     to queue any signals that would otherwise be delivered or
     queued.  */
  if (!any_pending && supports_breakpoints ())
    need_step_over
      = (struct lwp_info *) find_inferior (&all_lwps,
					   need_step_over_p, NULL);

  leave_all_stopped = (need_step_over != NULL || any_pending);

  if (debug_threads)
    {
      if (need_step_over != NULL)
	fprintf (stderr, "Not resuming all, need step over\n");
      else if (any_pending)
	fprintf (stderr,
		 "Not resuming, all-stop and found "
		 "an LWP with pending status\n");
      else
	fprintf (stderr, "Resuming, no pending status or step over needed\n");
    }

  /* Even if we're leaving threads stopped, queue all signals we'd
     otherwise deliver.  */
  find_inferior (&all_threads, linux_resume_one_thread, &leave_all_stopped);

  if (need_step_over)
    start_step_over (need_step_over);
}

/* This function is called once per thread.  We check the thread's
   last resume request, which will tell us whether to resume, step, or
   leave the thread stopped.  Any signal the client requested to be
   delivered has already been enqueued at this point.

   If any thread that GDB wants running is stopped at an internal
   breakpoint that needs stepping over, we start a step-over operation
   on that particular thread, and leave all others stopped.  */

static void
proceed_one_lwp (struct inferior_list_entry *entry)
{
  struct lwp_info *lwp;
  int step;

  lwp = (struct lwp_info *) entry;

  if (debug_threads)
    fprintf (stderr,
	     "proceed_one_lwp: lwp %ld\n", lwpid_of (lwp));

  if (!lwp->stopped)
    {
      if (debug_threads)
	fprintf (stderr, "   LWP %ld already running\n", lwpid_of (lwp));
      return;
    }

  if (lwp->last_resume_kind == resume_stop)
    {
      if (debug_threads)
	fprintf (stderr, "   client wants LWP %ld stopped\n", lwpid_of (lwp));
      return;
    }

  if (lwp->status_pending_p)
    {
      if (debug_threads)
	fprintf (stderr, "   LWP %ld has pending status, leaving stopped\n",
		 lwpid_of (lwp));
      return;
    }

  if (lwp->suspended)
    {
      if (debug_threads)
	fprintf (stderr, "   LWP %ld is suspended\n", lwpid_of (lwp));
      return;
    }

  step = lwp->last_resume_kind == resume_step;
  linux_resume_one_lwp (lwp, step, 0, NULL);
}

/* When we finish a step-over, set threads running again.  If there's
   another thread that may need a step-over, now's the time to start
   it.  Eventually, we'll move all threads past their breakpoints.  */

static void
proceed_all_lwps (void)
{
  struct lwp_info *need_step_over;

  /* If there is a thread which would otherwise be resumed, which is
     stopped at a breakpoint that needs stepping over, then don't
     resume any threads - have it step over the breakpoint with all
     other threads stopped, then resume all threads again.  */

  if (supports_breakpoints ())
    {
      need_step_over
	= (struct lwp_info *) find_inferior (&all_lwps,
					     need_step_over_p, NULL);

      if (need_step_over != NULL)
	{
	  if (debug_threads)
	    fprintf (stderr, "proceed_all_lwps: found "
		     "thread %ld needing a step-over\n",
		     lwpid_of (need_step_over));

	  start_step_over (need_step_over);
	  return;
	}
    }

  if (debug_threads)
    fprintf (stderr, "Proceeding, no step-over needed\n");

  for_each_inferior (&all_lwps, proceed_one_lwp);
}

/* Stopped LWPs that the client wanted to be running, that don't have
   pending statuses, are set to run again, except for EXCEPT, if not
   NULL.  This undoes a stop_all_lwps call.  */

static void
unstop_all_lwps (struct lwp_info *except)
{
  if (debug_threads)
    {
      if (except)
	fprintf (stderr,
		 "unstopping all lwps, except=(LWP %ld)\n", lwpid_of (except));
      else
	fprintf (stderr,
		 "unstopping all lwps\n");
    }

  /* Make sure proceed_one_lwp doesn't try to resume this thread.  */
  if (except != NULL)
    ++except->suspended;

  for_each_inferior (&all_lwps, proceed_one_lwp);

  if (except != NULL)
    --except->suspended;
}

#ifdef HAVE_LINUX_USRREGS

int
register_addr (int regnum)
{
  int addr;

  if (regnum < 0 || regnum >= the_low_target.num_regs)
    error ("Invalid register number %d.", regnum);

  addr = the_low_target.regmap[regnum];

  return addr;
}

/* Fetch one register.  */
static void
fetch_register (struct regcache *regcache, int regno)
{
  CORE_ADDR regaddr;
  int i, size;
  char *buf;
  int pid;

  if (regno >= the_low_target.num_regs)
    return;
  if ((*the_low_target.cannot_fetch_register) (regno))
    return;

  regaddr = register_addr (regno);
  if (regaddr == -1)
    return;

  pid = lwpid_of (get_thread_lwp (current_inferior));
  size = ((register_size (regno) + sizeof (PTRACE_XFER_TYPE) - 1)
	  & - sizeof (PTRACE_XFER_TYPE));
  buf = alloca (size);
  for (i = 0; i < size; i += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      *(PTRACE_XFER_TYPE *) (buf + i) =
	ptrace (PTRACE_PEEKUSER, pid,
		/* Coerce to a uintptr_t first to avoid potential gcc warning
		   of coercing an 8 byte integer to a 4 byte pointer.  */
		(PTRACE_ARG3_TYPE) (uintptr_t) regaddr, 0);
      regaddr += sizeof (PTRACE_XFER_TYPE);
      if (errno != 0)
	error ("reading register %d: %s", regno, strerror (errno));
    }

  if (the_low_target.supply_ptrace_register)
    the_low_target.supply_ptrace_register (regcache, regno, buf);
  else
    supply_register (regcache, regno, buf);
}

/* Fetch all registers, or just one, from the child process.  */
static void
usr_fetch_inferior_registers (struct regcache *regcache, int regno)
{
  if (regno == -1)
    for (regno = 0; regno < the_low_target.num_regs; regno++)
      fetch_register (regcache, regno);
  else
    fetch_register (regcache, regno);
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */
static void
usr_store_inferior_registers (struct regcache *regcache, int regno)
{
  CORE_ADDR regaddr;
  int i, size;
  char *buf;
  int pid;

  if (regno >= 0)
    {
      if (regno >= the_low_target.num_regs)
	return;

      if ((*the_low_target.cannot_store_register) (regno) == 1)
	return;

      regaddr = register_addr (regno);
      if (regaddr == -1)
	return;
      errno = 0;
      size = (register_size (regno) + sizeof (PTRACE_XFER_TYPE) - 1)
	     & - sizeof (PTRACE_XFER_TYPE);
      buf = alloca (size);
      memset (buf, 0, size);

      if (the_low_target.collect_ptrace_register)
	the_low_target.collect_ptrace_register (regcache, regno, buf);
      else
	collect_register (regcache, regno, buf);

      pid = lwpid_of (get_thread_lwp (current_inferior));
      for (i = 0; i < size; i += sizeof (PTRACE_XFER_TYPE))
	{
	  errno = 0;
	  ptrace (PTRACE_POKEUSER, pid,
		/* Coerce to a uintptr_t first to avoid potential gcc warning
		   about coercing an 8 byte integer to a 4 byte pointer.  */
		  (PTRACE_ARG3_TYPE) (uintptr_t) regaddr,
		  (PTRACE_ARG4_TYPE) *(PTRACE_XFER_TYPE *) (buf + i));
	  if (errno != 0)
	    {
	      /* At this point, ESRCH should mean the process is
		 already gone, in which case we simply ignore attempts
		 to change its registers.  See also the related
		 comment in linux_resume_one_lwp.  */
	      if (errno == ESRCH)
		return;

	      if ((*the_low_target.cannot_store_register) (regno) == 0)
		error ("writing register %d: %s", regno, strerror (errno));
	    }
	  regaddr += sizeof (PTRACE_XFER_TYPE);
	}
    }
  else
    for (regno = 0; regno < the_low_target.num_regs; regno++)
      usr_store_inferior_registers (regcache, regno);
}
#endif /* HAVE_LINUX_USRREGS */



#ifdef HAVE_LINUX_REGSETS

static int
regsets_fetch_inferior_registers (struct regcache *regcache)
{
  struct regset_info *regset;
  int saw_general_regs = 0;
  int pid;

  regset = target_regsets;

  pid = lwpid_of (get_thread_lwp (current_inferior));
  while (regset->size >= 0)
    {
      void *buf;
      int res;

      if (regset->size == 0 || disabled_regsets[regset - target_regsets])
	{
	  regset ++;
	  continue;
	}

      buf = xmalloc (regset->size);
#ifndef __sparc__
      res = ptrace (regset->get_request, pid, 0, buf);
#else
      res = ptrace (regset->get_request, pid, buf, 0);
#endif
      if (res < 0)
	{
	  if (errno == EIO)
	    {
	      /* If we get EIO on a regset, do not try it again for
		 this process.  */
	      disabled_regsets[regset - target_regsets] = 1;
	      free (buf);
	      continue;
	    }
	  else
	    {
	      char s[256];
	      sprintf (s, "ptrace(regsets_fetch_inferior_registers) PID=%d",
		       pid);
	      perror (s);
	    }
	}
      else if (regset->type == GENERAL_REGS)
	saw_general_regs = 1;
      regset->store_function (regcache, buf);
      regset ++;
      free (buf);
    }
  if (saw_general_regs)
    return 0;
  else
    return 1;
}

static int
regsets_store_inferior_registers (struct regcache *regcache)
{
  struct regset_info *regset;
  int saw_general_regs = 0;
  int pid;

  regset = target_regsets;

  pid = lwpid_of (get_thread_lwp (current_inferior));
  while (regset->size >= 0)
    {
      void *buf;
      int res;

      if (regset->size == 0 || disabled_regsets[regset - target_regsets])
	{
	  regset ++;
	  continue;
	}

      buf = xmalloc (regset->size);

      /* First fill the buffer with the current register set contents,
	 in case there are any items in the kernel's regset that are
	 not in gdbserver's regcache.  */
#ifndef __sparc__
      res = ptrace (regset->get_request, pid, 0, buf);
#else
      res = ptrace (regset->get_request, pid, buf, 0);
#endif

      if (res == 0)
	{
	  /* Then overlay our cached registers on that.  */
	  regset->fill_function (regcache, buf);

	  /* Only now do we write the register set.  */
#ifndef __sparc__
	  res = ptrace (regset->set_request, pid, 0, buf);
#else
	  res = ptrace (regset->set_request, pid, buf, 0);
#endif
	}

      if (res < 0)
	{
	  if (errno == EIO)
	    {
	      /* If we get EIO on a regset, do not try it again for
		 this process.  */
	      disabled_regsets[regset - target_regsets] = 1;
	      free (buf);
	      continue;
	    }
	  else if (errno == ESRCH)
	    {
	      /* At this point, ESRCH should mean the process is
		 already gone, in which case we simply ignore attempts
		 to change its registers.  See also the related
		 comment in linux_resume_one_lwp.  */
	      free (buf);
	      return 0;
	    }
	  else
	    {
	      perror ("Warning: ptrace(regsets_store_inferior_registers)");
	    }
	}
      else if (regset->type == GENERAL_REGS)
	saw_general_regs = 1;
      regset ++;
      free (buf);
    }
  if (saw_general_regs)
    return 0;
  else
    return 1;
  return 0;
}

#endif /* HAVE_LINUX_REGSETS */


void
linux_fetch_registers (struct regcache *regcache, int regno)
{
#ifdef HAVE_LINUX_REGSETS
  if (regsets_fetch_inferior_registers (regcache) == 0)
    return;
#endif
#ifdef HAVE_LINUX_USRREGS
  usr_fetch_inferior_registers (regcache, regno);
#endif
}

void
linux_store_registers (struct regcache *regcache, int regno)
{
#ifdef HAVE_LINUX_REGSETS
  if (regsets_store_inferior_registers (regcache) == 0)
    return;
#endif
#ifdef HAVE_LINUX_USRREGS
  usr_store_inferior_registers (regcache, regno);
#endif
}


/* Copy LEN bytes from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.  */

static int
linux_read_memory (CORE_ADDR memaddr, unsigned char *myaddr, int len)
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & -(CORE_ADDR) sizeof (PTRACE_XFER_TYPE);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
    = (((memaddr + len) - addr) + sizeof (PTRACE_XFER_TYPE) - 1)
      / sizeof (PTRACE_XFER_TYPE);
  /* Allocate buffer of that many longwords.  */
  register PTRACE_XFER_TYPE *buffer
    = (PTRACE_XFER_TYPE *) alloca (count * sizeof (PTRACE_XFER_TYPE));
  int fd;
  char filename[64];
  int pid = lwpid_of (get_thread_lwp (current_inferior));

  /* Try using /proc.  Don't bother for one word.  */
  if (len >= 3 * sizeof (long))
    {
      /* We could keep this file open and cache it - possibly one per
	 thread.  That requires some juggling, but is even faster.  */
      sprintf (filename, "/proc/%d/mem", pid);
      fd = open (filename, O_RDONLY | O_LARGEFILE);
      if (fd == -1)
	goto no_proc;

      /* If pread64 is available, use it.  It's faster if the kernel
	 supports it (only one syscall), and it's 64-bit safe even on
	 32-bit platforms (for instance, SPARC debugging a SPARC64
	 application).  */
#ifdef HAVE_PREAD64
      if (pread64 (fd, myaddr, len, memaddr) != len)
#else
      if (lseek (fd, memaddr, SEEK_SET) == -1 || read (fd, myaddr, len) != len)
#endif
	{
	  close (fd);
	  goto no_proc;
	}

      close (fd);
      return 0;
    }

 no_proc:
  /* Read all the longwords */
  for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      /* Coerce the 3rd arg to a uintptr_t first to avoid potential gcc warning
	 about coercing an 8 byte integer to a 4 byte pointer.  */
      buffer[i] = ptrace (PTRACE_PEEKTEXT, pid,
			  (PTRACE_ARG3_TYPE) (uintptr_t) addr, 0);
      if (errno)
	return errno;
    }

  /* Copy appropriate bytes out of the buffer.  */
  memcpy (myaddr,
	  (char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)),
	  len);

  return 0;
}

/* Copy LEN bytes of data from debugger memory at MYADDR to inferior's
   memory at MEMADDR.  On failure (cannot write to the inferior)
   returns the value of errno.  */

static int
linux_write_memory (CORE_ADDR memaddr, const unsigned char *myaddr, int len)
{
  register int i;
  /* Round starting address down to longword boundary.  */
  register CORE_ADDR addr = memaddr & -(CORE_ADDR) sizeof (PTRACE_XFER_TYPE);
  /* Round ending address up; get number of longwords that makes.  */
  register int count
  = (((memaddr + len) - addr) + sizeof (PTRACE_XFER_TYPE) - 1) / sizeof (PTRACE_XFER_TYPE);
  /* Allocate buffer of that many longwords.  */
  register PTRACE_XFER_TYPE *buffer = (PTRACE_XFER_TYPE *) alloca (count * sizeof (PTRACE_XFER_TYPE));
  int pid = lwpid_of (get_thread_lwp (current_inferior));

  if (debug_threads)
    {
      /* Dump up to four bytes.  */
      unsigned int val = * (unsigned int *) myaddr;
      if (len == 1)
	val = val & 0xff;
      else if (len == 2)
	val = val & 0xffff;
      else if (len == 3)
	val = val & 0xffffff;
      fprintf (stderr, "Writing %0*x to 0x%08lx\n", 2 * ((len < 4) ? len : 4),
	       val, (long)memaddr);
    }

  /* Fill start and end extra bytes of buffer with existing memory data.  */

  errno = 0;
  /* Coerce the 3rd arg to a uintptr_t first to avoid potential gcc warning
     about coercing an 8 byte integer to a 4 byte pointer.  */
  buffer[0] = ptrace (PTRACE_PEEKTEXT, pid,
		      (PTRACE_ARG3_TYPE) (uintptr_t) addr, 0);
  if (errno)
    return errno;

  if (count > 1)
    {
      errno = 0;
      buffer[count - 1]
	= ptrace (PTRACE_PEEKTEXT, pid,
		  /* Coerce to a uintptr_t first to avoid potential gcc warning
		     about coercing an 8 byte integer to a 4 byte pointer.  */
		  (PTRACE_ARG3_TYPE) (uintptr_t) (addr + (count - 1)
						  * sizeof (PTRACE_XFER_TYPE)),
		  0);
      if (errno)
	return errno;
    }

  /* Copy data to be written over corresponding part of buffer.  */

  memcpy ((char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)), myaddr, len);

  /* Write the entire buffer.  */

  for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      ptrace (PTRACE_POKETEXT, pid,
	      /* Coerce to a uintptr_t first to avoid potential gcc warning
		 about coercing an 8 byte integer to a 4 byte pointer.  */
	      (PTRACE_ARG3_TYPE) (uintptr_t) addr,
	      (PTRACE_ARG4_TYPE) buffer[i]);
      if (errno)
	return errno;
    }

  return 0;
}

/* Non-zero if the kernel supports PTRACE_O_TRACEFORK.  */
static int linux_supports_tracefork_flag;

/* Helper functions for linux_test_for_tracefork, called via clone ().  */

static int
linux_tracefork_grandchild (void *arg)
{
  _exit (0);
}

#define STACK_SIZE 4096

static int
linux_tracefork_child (void *arg)
{
  ptrace (PTRACE_TRACEME, 0, 0, 0);
  kill (getpid (), SIGSTOP);

#if !(defined(__UCLIBC__) && defined(HAS_NOMMU))

  if (fork () == 0)
    linux_tracefork_grandchild (NULL);

#else /* defined(__UCLIBC__) && defined(HAS_NOMMU) */

#ifdef __ia64__
  __clone2 (linux_tracefork_grandchild, arg, STACK_SIZE,
	    CLONE_VM | SIGCHLD, NULL);
#else
  clone (linux_tracefork_grandchild, arg + STACK_SIZE,
	 CLONE_VM | SIGCHLD, NULL);
#endif

#endif /* defined(__UCLIBC__) && defined(HAS_NOMMU) */

  _exit (0);
}

/* Determine if PTRACE_O_TRACEFORK can be used to follow fork events.  Make
   sure that we can enable the option, and that it had the desired
   effect.  */

static void
linux_test_for_tracefork (void)
{
  int child_pid, ret, status;
  long second_pid;
#if defined(__UCLIBC__) && defined(HAS_NOMMU)
  char *stack = xmalloc (STACK_SIZE * 4);
#endif /* defined(__UCLIBC__) && defined(HAS_NOMMU) */

  linux_supports_tracefork_flag = 0;

#if !(defined(__UCLIBC__) && defined(HAS_NOMMU))

  child_pid = fork ();
  if (child_pid == 0)
    linux_tracefork_child (NULL);

#else /* defined(__UCLIBC__) && defined(HAS_NOMMU) */

  /* Use CLONE_VM instead of fork, to support uClinux (no MMU).  */
#ifdef __ia64__
  child_pid = __clone2 (linux_tracefork_child, stack, STACK_SIZE,
			CLONE_VM | SIGCHLD, stack + STACK_SIZE * 2);
#else /* !__ia64__ */
  child_pid = clone (linux_tracefork_child, stack + STACK_SIZE,
		     CLONE_VM | SIGCHLD, stack + STACK_SIZE * 2);
#endif /* !__ia64__ */

#endif /* defined(__UCLIBC__) && defined(HAS_NOMMU) */

  if (child_pid == -1)
    perror_with_name ("clone");

  ret = my_waitpid (child_pid, &status, 0);
  if (ret == -1)
    perror_with_name ("waitpid");
  else if (ret != child_pid)
    error ("linux_test_for_tracefork: waitpid: unexpected result %d.", ret);
  if (! WIFSTOPPED (status))
    error ("linux_test_for_tracefork: waitpid: unexpected status %d.", status);

  ret = ptrace (PTRACE_SETOPTIONS, child_pid, 0,
		(PTRACE_ARG4_TYPE) PTRACE_O_TRACEFORK);
  if (ret != 0)
    {
      ret = ptrace (PTRACE_KILL, child_pid, 0, 0);
      if (ret != 0)
	{
	  warning ("linux_test_for_tracefork: failed to kill child");
	  return;
	}

      ret = my_waitpid (child_pid, &status, 0);
      if (ret != child_pid)
	warning ("linux_test_for_tracefork: failed to wait for killed child");
      else if (!WIFSIGNALED (status))
	warning ("linux_test_for_tracefork: unexpected wait status 0x%x from "
		 "killed child", status);

      return;
    }

  ret = ptrace (PTRACE_CONT, child_pid, 0, 0);
  if (ret != 0)
    warning ("linux_test_for_tracefork: failed to resume child");

  ret = my_waitpid (child_pid, &status, 0);

  if (ret == child_pid && WIFSTOPPED (status)
      && status >> 16 == PTRACE_EVENT_FORK)
    {
      second_pid = 0;
      ret = ptrace (PTRACE_GETEVENTMSG, child_pid, 0, &second_pid);
      if (ret == 0 && second_pid != 0)
	{
	  int second_status;

	  linux_supports_tracefork_flag = 1;
	  my_waitpid (second_pid, &second_status, 0);
	  ret = ptrace (PTRACE_KILL, second_pid, 0, 0);
	  if (ret != 0)
	    warning ("linux_test_for_tracefork: failed to kill second child");
	  my_waitpid (second_pid, &status, 0);
	}
    }
  else
    warning ("linux_test_for_tracefork: unexpected result from waitpid "
	     "(%d, status 0x%x)", ret, status);

  do
    {
      ret = ptrace (PTRACE_KILL, child_pid, 0, 0);
      if (ret != 0)
	warning ("linux_test_for_tracefork: failed to kill child");
      my_waitpid (child_pid, &status, 0);
    }
  while (WIFSTOPPED (status));

#if defined(__UCLIBC__) && defined(HAS_NOMMU)
  free (stack);
#endif /* defined(__UCLIBC__) && defined(HAS_NOMMU) */
}


static void
linux_look_up_symbols (void)
{
#ifdef USE_THREAD_DB
  struct process_info *proc = current_process ();

  if (proc->private->thread_db != NULL)
    return;

  /* If the kernel supports tracing forks then it also supports tracing
     clones, and then we don't need to use the magic thread event breakpoint
     to learn about threads.  */
  thread_db_init (!linux_supports_tracefork_flag);
#endif
}

static void
linux_request_interrupt (void)
{
  extern unsigned long signal_pid;

  if (!ptid_equal (cont_thread, null_ptid)
      && !ptid_equal (cont_thread, minus_one_ptid))
    {
      struct lwp_info *lwp;
      int lwpid;

      lwp = get_thread_lwp (current_inferior);
      lwpid = lwpid_of (lwp);
      kill_lwp (lwpid, SIGINT);
    }
  else
    kill_lwp (signal_pid, SIGINT);
}

/* Copy LEN bytes from inferior's auxiliary vector starting at OFFSET
   to debugger memory starting at MYADDR.  */

static int
linux_read_auxv (CORE_ADDR offset, unsigned char *myaddr, unsigned int len)
{
  char filename[PATH_MAX];
  int fd, n;
  int pid = lwpid_of (get_thread_lwp (current_inferior));

  snprintf (filename, sizeof filename, "/proc/%d/auxv", pid);

  fd = open (filename, O_RDONLY);
  if (fd < 0)
    return -1;

  if (offset != (CORE_ADDR) 0
      && lseek (fd, (off_t) offset, SEEK_SET) != (off_t) offset)
    n = -1;
  else
    n = read (fd, myaddr, len);

  close (fd);

  return n;
}

/* These breakpoint and watchpoint related wrapper functions simply
   pass on the function call if the target has registered a
   corresponding function.  */

static int
linux_insert_point (char type, CORE_ADDR addr, int len)
{
  if (the_low_target.insert_point != NULL)
    return the_low_target.insert_point (type, addr, len);
  else
    /* Unsupported (see target.h).  */
    return 1;
}

static int
linux_remove_point (char type, CORE_ADDR addr, int len)
{
  if (the_low_target.remove_point != NULL)
    return the_low_target.remove_point (type, addr, len);
  else
    /* Unsupported (see target.h).  */
    return 1;
}

static int
linux_stopped_by_watchpoint (void)
{
  struct lwp_info *lwp = get_thread_lwp (current_inferior);

  return lwp->stopped_by_watchpoint;
}

static CORE_ADDR
linux_stopped_data_address (void)
{
  struct lwp_info *lwp = get_thread_lwp (current_inferior);

  return lwp->stopped_data_address;
}

#if defined(__UCLIBC__) && defined(HAS_NOMMU)
#if defined(__mcoldfire__)
/* These should really be defined in the kernel's ptrace.h header.  */
#define PT_TEXT_ADDR 49*4
#define PT_DATA_ADDR 50*4
#define PT_TEXT_END_ADDR  51*4
#endif

/* Under uClinux, programs are loaded at non-zero offsets, which we need
   to tell gdb about.  */

static int
linux_read_offsets (CORE_ADDR *text_p, CORE_ADDR *data_p)
{
#if defined(PT_TEXT_ADDR) && defined(PT_DATA_ADDR) && defined(PT_TEXT_END_ADDR)
  unsigned long text, text_end, data;
  int pid = lwpid_of (get_thread_lwp (current_inferior));

  errno = 0;

  text = ptrace (PTRACE_PEEKUSER, pid, (long)PT_TEXT_ADDR, 0);
  text_end = ptrace (PTRACE_PEEKUSER, pid, (long)PT_TEXT_END_ADDR, 0);
  data = ptrace (PTRACE_PEEKUSER, pid, (long)PT_DATA_ADDR, 0);

  if (errno == 0)
    {
      /* Both text and data offsets produced at compile-time (and so
	 used by gdb) are relative to the beginning of the program,
	 with the data segment immediately following the text segment.
	 However, the actual runtime layout in memory may put the data
	 somewhere else, so when we send gdb a data base-address, we
	 use the real data base address and subtract the compile-time
	 data base-address from it (which is just the length of the
	 text segment).  BSS immediately follows data in both
	 cases.  */
      *text_p = text;
      *data_p = data - (text_end - text);

      return 1;
    }
#endif
 return 0;
}
#endif

static int
compare_ints (const void *xa, const void *xb)
{
  int a = *(const int *)xa;
  int b = *(const int *)xb;

  return a - b;
}

static int *
unique (int *b, int *e)
{
  int *d = b;
  while (++b != e)
    if (*d != *b)
      *++d = *b;
  return ++d;
}

/* Given PID, iterates over all threads in that process.

   Information about each thread, in a format suitable for qXfer:osdata:thread
   is printed to BUFFER, if it's not NULL.  BUFFER is assumed to be already
   initialized, and the caller is responsible for finishing and appending '\0'
   to it.

   The list of cores that threads are running on is assigned to *CORES, if it
   is not NULL.  If no cores are found, *CORES will be set to NULL.  Caller
   should free *CORES.  */

static void
list_threads (int pid, struct buffer *buffer, char **cores)
{
  int count = 0;
  int allocated = 10;
  int *core_numbers = xmalloc (sizeof (int) * allocated);
  char pathname[128];
  DIR *dir;
  struct dirent *dp;
  struct stat statbuf;

  sprintf (pathname, "/proc/%d/task", pid);
  if (stat (pathname, &statbuf) == 0 && S_ISDIR (statbuf.st_mode))
    {
      dir = opendir (pathname);
      if (!dir)
	{
	  free (core_numbers);
	  return;
	}

      while ((dp = readdir (dir)) != NULL)
	{
	  unsigned long lwp = strtoul (dp->d_name, NULL, 10);

	  if (lwp != 0)
	    {
	      unsigned core = linux_core_of_thread (ptid_build (pid, lwp, 0));

	      if (core != -1)
		{
		  char s[sizeof ("4294967295")];
		  sprintf (s, "%u", core);

		  if (count == allocated)
		    {
		      allocated *= 2;
		      core_numbers = realloc (core_numbers,
					      sizeof (int) * allocated);
		    }
		  core_numbers[count++] = core;
		  if (buffer)
		    buffer_xml_printf (buffer,
				       "<item>"
				       "<column name=\"pid\">%d</column>"
				       "<column name=\"tid\">%s</column>"
				       "<column name=\"core\">%s</column>"
				       "</item>", pid, dp->d_name, s);
		}
	      else
		{
		  if (buffer)
		    buffer_xml_printf (buffer,
				       "<item>"
				       "<column name=\"pid\">%d</column>"
				       "<column name=\"tid\">%s</column>"
				       "</item>", pid, dp->d_name);
		}
	    }
	}
    }

  if (cores)
    {
      *cores = NULL;
      if (count > 0)
	{
	  struct buffer buffer2;
	  int *b;
	  int *e;
	  qsort (core_numbers, count, sizeof (int), compare_ints);

	  /* Remove duplicates. */
	  b = core_numbers;
	  e = unique (b, core_numbers + count);

	  buffer_init (&buffer2);

	  for (b = core_numbers; b != e; ++b)
	    {
	      char number[sizeof ("4294967295")];
	      sprintf (number, "%u", *b);
	      buffer_xml_printf (&buffer2, "%s%s",
				 (b == core_numbers) ? "" : ",", number);
	    }
	  buffer_grow_str0 (&buffer2, "");

	  *cores = buffer_finish (&buffer2);
	}
    }
  free (core_numbers);
}

static void
show_process (int pid, const char *username, struct buffer *buffer)
{
  char pathname[128];
  FILE *f;
  char cmd[MAXPATHLEN + 1];

  sprintf (pathname, "/proc/%d/cmdline", pid);

  if ((f = fopen (pathname, "r")) != NULL)
    {
      size_t len = fread (cmd, 1, sizeof (cmd) - 1, f);
      if (len > 0)
	{
	  char *cores = 0;
	  int i;
	  for (i = 0; i < len; i++)
	    if (cmd[i] == '\0')
	      cmd[i] = ' ';
	  cmd[len] = '\0';

	  buffer_xml_printf (buffer,
			     "<item>"
			     "<column name=\"pid\">%d</column>"
			     "<column name=\"user\">%s</column>"
			     "<column name=\"command\">%s</column>",
			     pid,
			     username,
			     cmd);

	  /* This only collects core numbers, and does not print threads.  */
	  list_threads (pid, NULL, &cores);

	  if (cores)
	    {
	      buffer_xml_printf (buffer,
				 "<column name=\"cores\">%s</column>", cores);
	      free (cores);
	    }

	  buffer_xml_printf (buffer, "</item>");
	}
      fclose (f);
    }
}

static int
linux_qxfer_osdata (const char *annex,
		    unsigned char *readbuf, unsigned const char *writebuf,
		    CORE_ADDR offset, int len)
{
  /* We make the process list snapshot when the object starts to be
     read.  */
  static const char *buf;
  static long len_avail = -1;
  static struct buffer buffer;
  int processes = 0;
  int threads = 0;

  DIR *dirp;

  if (strcmp (annex, "processes") == 0)
    processes = 1;
  else if (strcmp (annex, "threads") == 0)
    threads = 1;
  else
    return 0;

  if (!readbuf || writebuf)
    return 0;

  if (offset == 0)
    {
      if (len_avail != -1 && len_avail != 0)
       buffer_free (&buffer);
      len_avail = 0;
      buf = NULL;
      buffer_init (&buffer);
      if (processes)
	buffer_grow_str (&buffer, "<osdata type=\"processes\">");
      else if (threads)
	buffer_grow_str (&buffer, "<osdata type=\"threads\">");

      dirp = opendir ("/proc");
      if (dirp)
       {
	 struct dirent *dp;
	 while ((dp = readdir (dirp)) != NULL)
	   {
	     struct stat statbuf;
	     char procentry[sizeof ("/proc/4294967295")];

	     if (!isdigit (dp->d_name[0])
		 || strlen (dp->d_name) > sizeof ("4294967295") - 1)
	       continue;

	     sprintf (procentry, "/proc/%s", dp->d_name);
	     if (stat (procentry, &statbuf) == 0
		 && S_ISDIR (statbuf.st_mode))
	       {
		 int pid = (int) strtoul (dp->d_name, NULL, 10);

		 if (processes)
		   {
		     struct passwd *entry = getpwuid (statbuf.st_uid);
		     show_process (pid, entry ? entry->pw_name : "?", &buffer);
		   }
		 else if (threads)
		   {
		     list_threads (pid, &buffer, NULL);
		   }
	       }
	   }

	 closedir (dirp);
       }
      buffer_grow_str0 (&buffer, "</osdata>\n");
      buf = buffer_finish (&buffer);
      len_avail = strlen (buf);
    }

  if (offset >= len_avail)
    {
      /* Done.  Get rid of the data.  */
      buffer_free (&buffer);
      buf = NULL;
      len_avail = 0;
      return 0;
    }

  if (len > len_avail - offset)
    len = len_avail - offset;
  memcpy (readbuf, buf + offset, len);

  return len;
}

/* Convert a native/host siginfo object, into/from the siginfo in the
   layout of the inferiors' architecture.  */

static void
siginfo_fixup (struct siginfo *siginfo, void *inf_siginfo, int direction)
{
  int done = 0;

  if (the_low_target.siginfo_fixup != NULL)
    done = the_low_target.siginfo_fixup (siginfo, inf_siginfo, direction);

  /* If there was no callback, or the callback didn't do anything,
     then just do a straight memcpy.  */
  if (!done)
    {
      if (direction == 1)
	memcpy (siginfo, inf_siginfo, sizeof (struct siginfo));
      else
	memcpy (inf_siginfo, siginfo, sizeof (struct siginfo));
    }
}

static int
linux_xfer_siginfo (const char *annex, unsigned char *readbuf,
		    unsigned const char *writebuf, CORE_ADDR offset, int len)
{
  int pid;
  struct siginfo siginfo;
  char inf_siginfo[sizeof (struct siginfo)];

  if (current_inferior == NULL)
    return -1;

  pid = lwpid_of (get_thread_lwp (current_inferior));

  if (debug_threads)
    fprintf (stderr, "%s siginfo for lwp %d.\n",
	     readbuf != NULL ? "Reading" : "Writing",
	     pid);

  if (offset > sizeof (siginfo))
    return -1;

  if (ptrace (PTRACE_GETSIGINFO, pid, 0, &siginfo) != 0)
    return -1;

  /* When GDBSERVER is built as a 64-bit application, ptrace writes into
     SIGINFO an object with 64-bit layout.  Since debugging a 32-bit
     inferior with a 64-bit GDBSERVER should look the same as debugging it
     with a 32-bit GDBSERVER, we need to convert it.  */
  siginfo_fixup (&siginfo, inf_siginfo, 0);

  if (offset + len > sizeof (siginfo))
    len = sizeof (siginfo) - offset;

  if (readbuf != NULL)
    memcpy (readbuf, inf_siginfo + offset, len);
  else
    {
      memcpy (inf_siginfo + offset, writebuf, len);

      /* Convert back to ptrace layout before flushing it out.  */
      siginfo_fixup (&siginfo, inf_siginfo, 1);

      if (ptrace (PTRACE_SETSIGINFO, pid, 0, &siginfo) != 0)
	return -1;
    }

  return len;
}

/* SIGCHLD handler that serves two purposes: In non-stop/async mode,
   so we notice when children change state; as the handler for the
   sigsuspend in my_waitpid.  */

static void
sigchld_handler (int signo)
{
  int old_errno = errno;

  if (debug_threads)
    /* fprintf is not async-signal-safe, so call write directly.  */
    write (2, "sigchld_handler\n", sizeof ("sigchld_handler\n") - 1);

  if (target_is_async_p ())
    async_file_mark (); /* trigger a linux_wait */

  errno = old_errno;
}

static int
linux_supports_non_stop (void)
{
  return 1;
}

static int
linux_async (int enable)
{
  int previous = (linux_event_pipe[0] != -1);

  if (previous != enable)
    {
      sigset_t mask;
      sigemptyset (&mask);
      sigaddset (&mask, SIGCHLD);

      sigprocmask (SIG_BLOCK, &mask, NULL);

      if (enable)
	{
	  if (pipe (linux_event_pipe) == -1)
	    fatal ("creating event pipe failed.");

	  fcntl (linux_event_pipe[0], F_SETFL, O_NONBLOCK);
	  fcntl (linux_event_pipe[1], F_SETFL, O_NONBLOCK);

	  /* Register the event loop handler.  */
	  add_file_handler (linux_event_pipe[0],
			    handle_target_event, NULL);

	  /* Always trigger a linux_wait.  */
	  async_file_mark ();
	}
      else
	{
	  delete_file_handler (linux_event_pipe[0]);

	  close (linux_event_pipe[0]);
	  close (linux_event_pipe[1]);
	  linux_event_pipe[0] = -1;
	  linux_event_pipe[1] = -1;
	}

      sigprocmask (SIG_UNBLOCK, &mask, NULL);
    }

  return previous;
}

static int
linux_start_non_stop (int nonstop)
{
  /* Register or unregister from event-loop accordingly.  */
  linux_async (nonstop);
  return 0;
}

static int
linux_supports_multi_process (void)
{
  return 1;
}


/* Enumerate spufs IDs for process PID.  */
static int
spu_enumerate_spu_ids (long pid, unsigned char *buf, CORE_ADDR offset, int len)
{
  int pos = 0;
  int written = 0;
  char path[128];
  DIR *dir;
  struct dirent *entry;

  sprintf (path, "/proc/%ld/fd", pid);
  dir = opendir (path);
  if (!dir)
    return -1;

  rewinddir (dir);
  while ((entry = readdir (dir)) != NULL)
    {
      struct stat st;
      struct statfs stfs;
      int fd;

      fd = atoi (entry->d_name);
      if (!fd)
        continue;

      sprintf (path, "/proc/%ld/fd/%d", pid, fd);
      if (stat (path, &st) != 0)
        continue;
      if (!S_ISDIR (st.st_mode))
        continue;

      if (statfs (path, &stfs) != 0)
        continue;
      if (stfs.f_type != SPUFS_MAGIC)
        continue;

      if (pos >= offset && pos + 4 <= offset + len)
        {
          *(unsigned int *)(buf + pos - offset) = fd;
          written += 4;
        }
      pos += 4;
    }

  closedir (dir);
  return written;
}

/* Implements the to_xfer_partial interface for the TARGET_OBJECT_SPU
   object type, using the /proc file system.  */
static int
linux_qxfer_spu (const char *annex, unsigned char *readbuf,
		 unsigned const char *writebuf,
		 CORE_ADDR offset, int len)
{
  long pid = lwpid_of (get_thread_lwp (current_inferior));
  char buf[128];
  int fd = 0;
  int ret = 0;

  if (!writebuf && !readbuf)
    return -1;

  if (!*annex)
    {
      if (!readbuf)
	return -1;
      else
	return spu_enumerate_spu_ids (pid, readbuf, offset, len);
    }

  sprintf (buf, "/proc/%ld/fd/%s", pid, annex);
  fd = open (buf, writebuf? O_WRONLY : O_RDONLY);
  if (fd <= 0)
    return -1;

  if (offset != 0
      && lseek (fd, (off_t) offset, SEEK_SET) != (off_t) offset)
    {
      close (fd);
      return 0;
    }

  if (writebuf)
    ret = write (fd, writebuf, (size_t) len);
  else
    ret = read (fd, readbuf, (size_t) len);

  close (fd);
  return ret;
}

static int
linux_core_of_thread (ptid_t ptid)
{
  char filename[sizeof ("/proc//task//stat")
		 + 2 * 20 /* decimal digits for 2 numbers, max 2^64 bit each */
		 + 1];
  FILE *f;
  char *content = NULL;
  char *p;
  char *ts = 0;
  int content_read = 0;
  int i;
  int core;

  sprintf (filename, "/proc/%d/task/%ld/stat",
	   ptid_get_pid (ptid), ptid_get_lwp (ptid));
  f = fopen (filename, "r");
  if (!f)
    return -1;

  for (;;)
    {
      int n;
      content = realloc (content, content_read + 1024);
      n = fread (content + content_read, 1, 1024, f);
      content_read += n;
      if (n < 1024)
	{
	  content[content_read] = '\0';
	  break;
	}
    }

  p = strchr (content, '(');
  p = strchr (p, ')') + 2; /* skip ")" and a whitespace. */

  p = strtok_r (p, " ", &ts);
  for (i = 0; i != 36; ++i)
    p = strtok_r (NULL, " ", &ts);

  if (sscanf (p, "%d", &core) == 0)
    core = -1;

  free (content);
  fclose (f);

  return core;
}

static struct target_ops linux_target_ops = {
  linux_create_inferior,
  linux_attach,
  linux_kill,
  linux_detach,
  linux_join,
  linux_thread_alive,
  linux_resume,
  linux_wait,
  linux_fetch_registers,
  linux_store_registers,
  linux_read_memory,
  linux_write_memory,
  linux_look_up_symbols,
  linux_request_interrupt,
  linux_read_auxv,
  linux_insert_point,
  linux_remove_point,
  linux_stopped_by_watchpoint,
  linux_stopped_data_address,
#if defined(__UCLIBC__) && defined(HAS_NOMMU)
  linux_read_offsets,
#else
  NULL,
#endif
#ifdef USE_THREAD_DB
  thread_db_get_tls_address,
#else
  NULL,
#endif
  linux_qxfer_spu,
  hostio_last_error_from_errno,
  linux_qxfer_osdata,
  linux_xfer_siginfo,
  linux_supports_non_stop,
  linux_async,
  linux_start_non_stop,
  linux_supports_multi_process,
#ifdef USE_THREAD_DB
  thread_db_handle_monitor_command,
#else
  NULL,
#endif
  linux_core_of_thread
};

static void
linux_init_signals ()
{
  /* FIXME drow/2002-06-09: As above, we should check with LinuxThreads
     to find what the cancel signal actually is.  */
#ifdef __SIGRTMIN /* Bionic doesn't use SIGRTMIN the way glibc does.  */
  signal (__SIGRTMIN+1, SIG_IGN);
#endif
}

void
initialize_low (void)
{
  struct sigaction sigchld_action;
  memset (&sigchld_action, 0, sizeof (sigchld_action));
  set_target_ops (&linux_target_ops);
  set_breakpoint_data (the_low_target.breakpoint,
		       the_low_target.breakpoint_len);
  linux_init_signals ();
  linux_test_for_tracefork ();
#ifdef HAVE_LINUX_REGSETS
  for (num_regsets = 0; target_regsets[num_regsets].size >= 0; num_regsets++)
    ;
  disabled_regsets = xmalloc (num_regsets);
#endif

  sigchld_action.sa_handler = sigchld_handler;
  sigemptyset (&sigchld_action.sa_mask);
  sigchld_action.sa_flags = SA_RESTART;
  sigaction (SIGCHLD, &sigchld_action, NULL);
}
