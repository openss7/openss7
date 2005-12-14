/*****************************************************************************

 @(#) $RCSfile: strutil.h,v $ $Name:  $($Revision: 0.9.2.47 $) $Date: 2005/12/14 11:43:20 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/12/14 11:43:20 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRUTIL_H__
#define __LOCAL_STRUTIL_H__

#ifndef BIG_STATIC
#define BIG_STATIC
#endif

#ifndef BIG_STATIC_INLINE
#define BIG_STATIC_INLINE
#endif

#include <stdbool.h>

/* global synchq */
extern struct syncq *global_syncq;

#if 0
extern bool __rmvq(queue_t *q, mblk_t *mp);
#endif
extern bool __flushq(queue_t *q, int flag, mblk_t ***mppp, char bands[]);

/*
 *  Simple locks, no nesting (not required).  Write locks suppress interrupts; read locks only
 *  suppress the STREAMS softirq for stream heads, but also suppresses local interrupts for freeze
 *  and queue read locks.  This allows read locks to be taken on the stream head in all contexts,
 *  and write locks to be taken in process context and STREAMS softirq.
 *
 *  Freeze write locks can be taken from hard irq context, so both freeze read and write locks
 *  suppress hard interrupts, but read locks are only taken for a later write lock on a queue, and
 *  queue write locks can be taken from hard irq.  This also means tha queue read locks need to
 *  suppress interrupts.
 *
 *  This means that it is safe to call any queue handling functions from in_irq().  There are a
 *  couple of notes: putq() can be called from in_irq().  put() can be called from in_irq(), but it
 *  might defer calling the put() procedure until STREAMS next executes (at bottom half).
 *
 *  It is safe to call queue handling utilities (and a host of other queue specific utilities) from
 *  bottom half, atomic or process context (!in_streams() && !in_irq)).  None of the xxxnext
 *  functions can be called from this context.  To call xxxnext utilities, first enter STREAMS
 *  context by taking a STREAM head read lock (this is what the STREAM head itself does).
 *
 *  It is safe to call all queue handling functions from within a queue open, close, put or service
 *  procedure.  Callbacks (bufcall, esballoc, timeout) that were invoked from within a queue open,
 *  close, put or service procedure are also safe.  bufcall(), timeout() callbacks invoked from
 *  outside of one of these queue procedures or queue syncrhonized callbacks, will run within the
 *  STREAMS environment, but will lack the STREAM head read locks necessary to use the xxxnext
 *  functions.
 *
 *  The same is true when transferring across a multiplexing driver.  When an upper or lower queue
 *  procedure executes queue handling functions on the queue pair opposite the side of the
 *  procedure, the xxxnext utilities cannot be used.  The same situation is true across the twist in
 *  a FIFO or pipe, pseudo-terminal, or any other pair of welded queues.  In some of these
 *  circumstances it is possible to take a STREAM head lock with the more portable MPSTR_STPLOCK()
 *  functions compatible with UnixWare.  (Basically a swlock().)
 *
 *  Also, getq() and putbq() must only be used from with a queue service procedure.
 */

/* The following are like _bh, but _str for STREAMS scheduler.  These lock out the STREAMS scheduler
 * from interrupting the current thread and attempting to take a plumbing write lock for a weldq(9)
 * or unweldq(9) operation while the current context is holding a plumbing read lock.  Because the
 * STREAMS scheduler can only interrupt a process context when it runs at bottom half, there is no
 * need to lock out the STREAMS scheduler when running as a kernel thread.  This is good for kernel
 * threads on UP because taking a plumbing read lock becomes a no-op. */

#if defined CONFIG_STREAMS_KTHREADS

#define read_str_disable()		do { } while (0)
#define read_str_enable()		do { } while (0)

#else				/* defined CONFIG_STREAMS_KTHREADS */

#define read_str_disable()		do { local_str_disable(); } while (0)
#define read_str_enable()		do { local_str_enable();  } while (0)

#endif				/* defined CONFIG_STREAMS_KTHREADS */

#define read_lock_str(__lk)		do { read_str_disable();  read_lock((__lk)); } while (0)
#define read_unlock_str(__lk)		do { read_unlock((__lk)); read_str_enable(); } while (0)

/* freeze locks */

/* Freeze locks are for the freezestr(9)/unfreezestr(9) mechanism.  Threads entering a put or
 * service procedure take and release a read lock.  Read or write locks of a queue structure
 * take a freeze read lock.  freezestr(9) takes a freeze write lock, blocking other threads
 * from entering a put or service procedure and keeping other threads from reading or writing
 * any queue structure in the Stream.  These locks are recursive in the sense that the caller of the
 * write lock can acquire the read lock as well, permitting many functions that would otherwise
 * not be able to be called on a frozen Stream, may be called under Linux Fast-STREAMS.
 *
 * Note that locking out interrupts and the STREAMS scheduler prevents concurrent attempts on the
 * same processor to reaqcuire the lock.
 */

#define frozen_by_caller(__q)		(bool)({ ((qstream((__q)))->sd_freezer == current); })
#define not_frozen_by_caller(__q)	(bool)({ ((qstream((__q)))->sd_freezer != current); })

/* no IRQ suppression (but BH) for profiling */
#ifdef CONFIG_STREAMS_TEST
#define zlockinit(__sd)			do { rwlock_init(&(__sd)->sd_freeze); } while (0)
#define zwlock(__sd)			(unsigned long)({ write_lock_bh(&(__sd)->sd_freeze); (__sd)->sd_freezer = current; 0; })
#define zwunlock(__sd,__pl)		do { (__sd)->sd_freezer = NULL; write_unlock_bh(&(__sd)->sd_freeze); (void)(__pl); } while (0)
#define zrlock(__sd)			(unsigned long)({ local_bh_disable(); if ((__sd)->sd_freezer != current) read_lock(&(__sd)->sd_freeze); 0; })
#define zrunlock(__sd,__pl)		do { if ((__sd)->sd_freezer != current) read_unlock(&(__sd)->sd_freeze); local_bh_enable(); (void)(__pl); } while (0)
#else
#define zlockinit(__sd)			do { rwlock_init(&(__sd)->sd_freeze); } while (0)
#define zwlock(__sd)			(unsigned long)({ unsigned long __pl; write_lock_irqsave(&(__sd)->sd_freeze, __pl); (__sd)->sd_freezer = current; __pl; })
#define zwunlock(__sd,__pl)		do { (__sd)->sd_freezer = NULL; write_unlock_irqrestore(&(__sd)->sd_freeze, (__pl)); } while (0)
#define zrlock(__sd)			(unsigned long)({ unsigned long __pl; local_irq_save(__pl); if ((__sd)->sd_freezer != current) read_lock(&(__sd)->sd_freeze); __pl; })
#define zrunlock(__sd,__pl)		do { if ((__sd)->sd_freezer != current) read_unlock(&(__sd)->sd_freeze); local_irq_restore((__pl)); } while (0)
#endif

#define stream_barrier(__sd)		do { unsigned long __pl; __pl = zrlock((__sd)); zrunlock((__sd),__pl); } while (0)
#define freeze_barrier(__q)		do { struct stdata *__sd = qstream((__q)); stream_barrier(__sd); } while (0)

/* plumbing locks */

/* Plumbing locks protect the q_next pointers in a Stream.  All utilities that need q_next
 * protection take a read lock.  A read lock is taken before a put or service procedure is called
 * and released afterward.  Stream head open()/close(), I_(P)LINK/I_(P)UNLINK operations take a
 * plumbing write lock (at user context) before replumbing a Stream.  (Note that open()/close(),
 * I_(P)LINK/I_(P)UNLINK operations effective have a read lock because they hold the STRWOPEN bit so
 * that no other open()/close(), I_(P)LINK/I_(P)UNLINK operations can occur on the Stream.)
 *
 * Unfortunately, there are two instances where a plumbing write lock will be taken by the STREAMS
 * scheduler: those are when performing weldq(9)/unweldq(9) operations.  Therefore, whenever we take
 * a plumbing read lock, we need to lockout the STREAMS scheduler temporarily.
 */

#define plockinit(__sd)			do { rwlock_init(&(__sd)->sd_plumb); } while (0)
#define pwlock(__sd)			(unsigned long)({ unsigned long __pl; __pl = zrlock((__sd)); write_lock(&(__sd)->sd_plumb); __pl; })
#define pwunlock(__sd,__pl)		do { write_unlock(&(__sd)->sd_plumb); zrunlock((__sd),(__pl)); } while (0)
#define prlock(__sd)			do { read_lock_str(&(__sd)->sd_plumb);   } while (0)
#define prunlock(__sd)			do { read_unlock_str(&(__sd)->sd_plumb); } while (0)

/* stream head structure locks */

#define slockinit(__sd)			do { rwlock_init(&(__sd)->sd_lock);  } while (0)
#define swlock(__sd)			do { write_lock(&(__sd)->sd_lock);   } while (0)
#define swunlock(__sd)			do { write_unlock(&(__sd)->sd_lock); } while (0)
#define srlock(__sd)			do { read_lock(&(__sd)->sd_lock);    } while (0)
#define srunlock(__sd)			do { read_unlock(&(__sd)->sd_lock);  } while (0)

/* queue structure locks */

/* Queue structure locks are basic reader-writer locks with the exception that a freeze read lock is
 * acquired while we hold a write lock.  This allows a freeze write lock to block any other thread
 * from putting messages on or off a queue or otherwise affecting the state of a queue in the frozen
 * Stream.
 *
 * Whether queue read locks should take a freeze read lock is a matter of whether freezestr(9)
 * really allows the caller to manipulate thing about a queue other than simply putting messages on
 * and off it.  Because strqset(9) cannot change much, and it is the only recommended way of
 * accessing queue or queue band data structures, we don't take a freeze read lock. */

#define qlockinit(__q)			do { rwlock_init(&(__q)->q_lock); } while (0)
#define qwlock(__q)			(unsigned long)({ unsigned long __pl; struct stdata *__sd = qstream((__q)); __pl = zrlock(__sd); write_lock(&(__q)->q_lock); __pl; })
#define qwunlock(__q,__pl)		do { struct stdata *__sd = qstream((__q)); write_unlock(&(__q)->q_lock); zrunlock((__sd),(__pl)); }  while (0)

/* no IRQ suppression (but BH) for profiling */
#ifdef CONFIG_STREAMS_TEST
#define qrlock(__q)			(unsigned long)({ read_lock_bh(&(__q)->q_lock); 0; })
#define qrunlock(__q,__pl)		do { read_unlock_bh(&(__q)->q_lock); (void)(__pl); }  while (0)
#else
#define qrlock(__q)			(unsigned long)({ unsigned long __pl; read_lock_irqsave(&(__q)->q_lock, __pl);  __pl;  })
#define qrunlock(__q,__pl)		do { read_unlock_irqrestore(&(__q)->q_lock,(__pl)); }  while (0)
#endif

#endif				/* __LOCAL_STRUTIL_H__ */
