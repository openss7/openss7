/*****************************************************************************

 @(#) $RCSfile: strutil.h,v $ $Name:  $($Revision: 0.9.2.39 $) $Date: 2005/10/22 19:58:17 $

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

 Last Modified $Date: 2005/10/22 19:58:17 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRUTIL_H__
#define __LOCAL_STRUTIL_H__

#include <stdbool.h>

/* global synchq */
extern struct syncq *global_syncq;

#if 0
extern bool __rmvq(queue_t *q, mblk_t *mp);
#endif
extern bool __flushq(queue_t *q, int flag, mblk_t ***mppp, char bands[]);

/* queue band gets and puts */
extern qband_t *STREAMS_FASTCALL(bget(qband_t *qb));
extern void STREAMS_FASTCALL(bput(qband_t **bp));

/* queue gets and puts */
extern queue_t *STREAMS_FASTCALL(qget(queue_t *q));
extern void STREAMS_FASTCALL(qput(queue_t **qp));

#if 0
/* kernel locks */
extern void STREAMS_FASTCALL(klockinit(klock_t *kl));
extern void STREAMS_FASTCALL(kwlock(klock_t *kl));
extern int STREAMS_FASTCALL(kwtrylock(klock_t *kl));
extern void STREAMS_FASTCALL(kwlock_wait(klock_t *kl));
extern int STREAMS_FASTCALL(kwlock_wait_sig(klock_t *kl));
extern void STREAMS_FASTCALL(kwunlock(klock_t *kl));
extern void STREAMS_FASTCALL(krlock(klock_t *kl));
extern void STREAMS_FASTCALL(krunlock(klock_t *kl));
extern void STREAMS_FASTCALL(krlock_irqsave(klock_t *kl, unsigned long *flagsp));
extern void STREAMS_FASTCALL(krunlock_irqrestore(klock_t *kl, unsigned long *flagsp));

/* for stream heads */
#define slockinit(__sd)			klockinit(&(__sd)->sd_klock)
#define swlock(__sd)			kwlock(&(__sd)->sd_klock)
#define swlock(__sd)			kwlock(&(__sd)->sd_klock)
#define swtrylock(__sd)			kwtrylock(&(__sd)->sd_klock)
#define swlock_wait(__sd)		kwlock_wait(&(__sd)->sd_klock)
#define swlock_wait_sig(__sd)		kwlock_wait_sig(&(__sd)->sd_klock)
#define swunlock(__sd)			kwunlock(&(__sd)->sd_klock)
#define srlock(__sd)			krlock(&(__sd)->sd_klock)
#define srunlock(__sd)			krunlock(&(__sd)->sd_klock)

#if 0
/* for stream heads from queues */
#define hwlock(__q)			kwlock(&(qstream(__q))->sd_klock)
#define hwtrylock(__q)			kwtrylock(&(qstream(__q))->sd_klock)
#define hwlock_wait(__q)		kwlock_wait(&(qstream(__q))->sd_klock)
#define hwlock_wait_sig(__q)		kwlock_wait_sig(&(qstream(__q))->sd_klock)
#define hwunlock(__q)			kwunlock(&(qstream(__q))->sd_klock)
#define hrlock(__q)			krlock(&(qstream(__q))->sd_klock)
#define hrunlock(__q)			krunlock(&(qstream(__q))->sd_klock)
#endif

#if 0
/* for queue pairs */
#define qplockinit(__rq)		klockinit(&((struct queinfo*)(__rq))->qu_klock)
#define qpwlock(__rq)			kwlock(&((struct queinfo*)(__rq))->qu_klock)
#define qpwtrylock(__rq)		kwtrylock(&((struct queinfo*)(__rq))->qu_klock)
#define qpwlock_wait(__rq)		kwlock_wait(&((struct queinfo*)(__rq))->qu_klock)
#define qpwlock_wait_sig(__rq)		kwlock_wait_sig(&((struct queinfo*)(__rq))->qu_klock)
#define qpwunlock(__rq)			kwunlock(&((struct queinfo*)(__rq))->qu_klock)
#define qprlock(__rq)			krlock(&((struct queinfo*)(__rq))->qu_klock)
#define qprunlock(__rq)			krunlock(&((struct queinfo*)(__rq))->qu_klock)
#endif

/* for queues */
#define qlockinit(__q)			klockinit(&(__q)->q_klock)
#define qwlock(__q)			kwlock(&(__q)->q_klock)
#define qwtrylock(__q)			kwtrylock(&(__q)->q_klock)
#define qwlock_wait(__q)		kwlock_wait(&(__q)->q_klock)
#define qwlock_wait_sig(__q)		kwlock_wait_sig(&(__q)->q_klock)
#define qwunlock(__q)			kwunlock(&(__q)->q_klock)
#define qrlock(__q)			krlock(&(__q)->q_klock)
#define qrunlock(__q)			krunlock(&(__q)->q_klock)
#if 0
#define qrlock_irqsave(__q,__f)		krlock_irqsave(&(__q)->q_klock,(__f))
#define qrunlock_irqrestore(__q,__f)	krunlock_irqrestore(&(__q)->q_klock,(__f))
#endif

#else

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

#if 0				/* make these all functions now */
//#ifdef CONFIG_STREAMS_DEBUG
/* loosen locks for debugging */
//#define write_str_disable()   do { local_str_disable(); } while (0)
//#define write_str_enable()    do { local_str_enable(); } while (0)
//#else                         /* CONFIG_STREAMS_DEBUG */
//#define write_str_disable()   do { local_bh_disable(); local_str_disable(); } while (0)
//#define write_str_enable()    do { local_str_enable(); local_bh_enable(); } while (0)
//#endif                                /* CONFIG_STREAMS_DEBUG */
#define read_str_disable()	do { local_bh_disable(); } while (0)
#define read_str_enable()	do { local_bh_enable(); } while (0)

//#define write_lock_str(__l)   do { write_str_disable(); write_lock(__l);  } while (0)
//#define write_unlock_str(__l) do { write_unlock(__l); write_str_enable(); } while (0)
#define read_lock_str(__l)	do { read_str_disable();  read_lock(__l); } while (0)
#define read_unlock_str(__l)	do { read_unlock(__l); read_str_enable(); } while (0)
//#define zread_lock_str(__l)   do { write_str_disable();  read_lock(__l); } while (0)
//#define zread_unlock_str(__l) do { read_unlock(__l); write_str_enable(); } while (0)

/*
 *  Just a little bit of nesting on freeze locks, if the caller holds a write lock on the freeze
 *  lock, it can take a read lock on the freeze lock without deadlocking.  This allows the freezer
 *  to do things that others are not permitted to do.
 */
#ifdef CONFIG_SMP

#define frozen_by_caller(__q)		(qstream(__q)->sd_freezer == current)
#define not_frozen_by_caller(__q)	(qstream(__q)->sd_freezer != current)

#define zlockinit(__sd)		rwlock_init(&(__sd)->sd_freeze)
#define zwlock(__sd)		(unsigned long) ({ struct stdata *_z_sd = (__sd); unsigned long pl; write_lock_irqsave(&_z_sd->sd_freeze, pl); _z_sd->sd_freezer = current; pl; })
#define zwunlock(__sd, __pl)	(void) ({ struct stdata *_z_sd = (__sd); _z_sd->sd_freezer = NULL; write_unlock_irqrestore(&_z_sd->sd_freeze, (__pl)); })
#define zrlock(__sd)		(unsigned long) ({ struct stdata *_z_sd = (__sd); unsigned long pl; local_irq_save(pl); if (_z_sd->sd_freezer != current) read_lock(&_z_sd->sd_freeze); pl; })
#define zrunlock(__sd, __pl)	(void) ({ struct stdata *_z_sd = (__sd); if (_z_sd->sd_freezer != current) read_unlock(&_z_sd->sd_freeze); local_irq_restore(__pl); })

#else				/* CONFIG_SMP */

#define frozen_by_caller(__q)		(1)
#define not_frozen_by_caller(__q)	(1)

#define zlockinit(__sd)		rwlock_init(&(__sd)->sd_freeze)
#define zwlock(__sd)		(unsigned long) ({ unsigned long pl; write_lock_irqsave(&(__sd)->sd_freeze, pl); pl; })
#define zwunlock(__sd, __pl)	(void) ({ write_unlock_irqrestore(&(__sd)->sd_freeze, (__pl)); })
#define zrlock(__sd)		(unsigned long) ({ unsigned long pl; read_lock_irqsave(&(__sd)->sd_freeze, pl); pl; })
#define zrunlock(__sd, __pl)	(void) ({ read_unlock_irqrestore(&(__sd)->sd_freeze, (__pl)); })

#endif				/* CONFIG_SMP */

#define stream_barrier(__sd)	do { struct stdata *_b_sd = (__sd); unsigned long pl; pl = zrlock(_b_sd); zrunlock((_b_sd), pl); } while (0)
#define freeze_barrier(__q)	do { struct stdata *_f_sd = qstream(__q); stream_barrier(_f_sd); } while (0)

/*
 *  Plumbing locks.
 *
 *  A write plumb lock suppresses local interrupts, spins on a stream frozen by the another
 *  processor until it thaws, and spins waiting for put and service procedures on another processor
 *  to exit.  Plumb write locks are used by the Stream head and by STREAMS to provide exclusive
 *  access to the Stream exclusive of put and service procedures, and permits exclusive access to
 *  the sd->sd_rq, sd->sd_wq and all q->q_next pointers in the Stream (thus the name plumb lock).
 *  Write plumb locks can be acquired only by process or STREAMS (scheduler) context.
 *
 *  Read plumb locks are used to provide shared access to the q->q_next pointers in the stream and
 *  are taken on entry to and released on exit from syncrhonous procedures and callback functions.
 *  Read locks only suppress STREAMS (scheduler) execution from process context.  Read plumb locks
 *  can be acquired by all contexts.
 */
#define plockinit(__sd)		rwlock_init(&(__sd)->sd_plumb)
#define pwlock(__sd)		(unsigned long) ({ struct stdata *_p_sd = (__sd); unsigned long pl; pl = zrlock(_p_sd); write_lock(&_p_sd->sd_plumb); pl; })
#define pwunlock(__sd, __pl)	(void) ({ struct stdata *_p_sd = (__sd); write_unlock(&_p_sd->sd_plumb); zrunlock(_p_sd, (__pl)); })
#define prlock(__sd)		read_lock_str(&(__sd)->sd_plumb)
#define prunlock(__sd)		read_unlock_str(&(__sd)->sd_plumb)

/*
 *  Stream head structure lock.
 *
 *  A structure lock protecting shared and exclusive access to other members of the Stream head
 *  structure that need protection.  A write lock suppresses no interrupts.  These are simple spins.
 *  A read lock suppresses no interrupts.  A write lock can be taken by the Stream head at process
 *  context.  A read lock can be taken by any context.
 */
#define slockinit(__sd)		rwlock_init(&(__sd)->sd_lock)
#define swlock(__sd)		write_lock(&(__sd)->sd_lock)
#define swunlock(__sd)		write_unlock(&(__sd)->sd_lock)
#define srlock(__sd)		read_lock(&(__sd)->sd_lock)
#define srunlock(__sd)		read_unlock(&(__sd)->sd_lock)

#define qlockinit(__q)		rwlock_init(&(__q)->q_lock)
#define qwlock(__q)		(unsigned long) ({ queue_t *_q = (__q); unsigned long pl; pl = zrlock(qstream(_q)); write_lock(&_q->q_lock); pl; })
#define qwunlock(__q, __pl)	(void) ({ write_unlock(&(__q)->q_lock); zrunlock(qstream(__q), (__pl)); })
#define qrlock(__q)		(unsigned long) ({ unsigned long pl; read_lock_irqsave(&(__q)->q_lock, pl); pl; })
#define qrunlock(__q, __pl)	do { read_unlock_irqrestore(&(__q)->q_lock, (__pl)); } while (0)

#else

static inline void
read_str_disable(void)
{
	local_str_disable();
}
static inline void
read_str_enable(void)
{
	local_str_enable();
}

static inline void
read_lock_str(rwlock_t *lk)
{
	read_str_disable();
	read_lock(lk);
}
static inline void
read_unlock_str(rwlock_t *lk)
{
	read_unlock(lk);
	read_str_enable();
}

static inline int
frozen_by_caller(queue_t *q)
{
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);
	return (sd->sd_freezer == current);
}
static inline int
not_frozen_by_caller(queue_t *q)
{
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);
	return (sd->sd_freezer != current);
}

static inline void
zlockinit(struct stdata *sd)
{
	assert(sd);
	rwlock_init(&sd->sd_freeze);
}
static inline unsigned long
zwlock(struct stdata *sd)
{
	unsigned long pl;

	assert(sd);
	write_lock_irqsave(&sd->sd_freeze, pl);
	sd->sd_freezer = current;
	return (pl);
}
static inline void
zwunlock(struct stdata *sd, unsigned long pl)
{
	assert(sd);
	sd->sd_freezer = NULL;
	write_unlock_irqrestore(&sd->sd_freeze, pl);
}
static inline unsigned long
zrlock(struct stdata *sd)
{
	unsigned long pl;

	assert(sd);
	local_irq_save(pl);
	if (sd->sd_freezer != current)
		read_lock(&sd->sd_freeze);
	return (pl);
}
static inline void
zrunlock(struct stdata *sd, unsigned long pl)
{
	assert(sd);
	if (sd->sd_freezer != current)
		read_unlock(&sd->sd_freeze);
	local_irq_restore(pl);
}

static inline void
stream_barrier(struct stdata *sd)
{
	unsigned long pl;

	assert(sd);
	pl = zrlock(sd);
	zrunlock(sd, pl);
}
static inline void
freeze_barrier(queue_t *q)
{
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);
	stream_barrier(sd);
}

static inline void
plockinit(struct stdata *sd)
{
	assert(sd);
	rwlock_init(&sd->sd_plumb);
}
static inline unsigned long
pwlock(struct stdata *sd)
{
	unsigned long pl;

	assert(sd);
	pl = zrlock(sd);
	write_lock(&sd->sd_plumb);
	return (pl);
}
static inline void
pwunlock(struct stdata *sd, unsigned long pl)
{
	assert(sd);
	write_unlock(&sd->sd_plumb);
	zrunlock(sd, pl);
}
static inline void
prlock(struct stdata *sd)
{
	assert(sd);
	read_lock_str(&sd->sd_plumb);
}
static inline void
prunlock(struct stdata *sd)
{
	assert(sd);
	read_unlock_str(&sd->sd_plumb);
}

static inline void
slockinit(struct stdata *sd)
{
	assert(sd);
	rwlock_init(&sd->sd_lock);
}
static inline void
swlock(struct stdata *sd)
{
	assert(sd);
	write_lock(&sd->sd_lock);
}
static inline void
swunlock(struct stdata *sd)
{
	assert(sd);
	write_unlock(&sd->sd_lock);
}
static inline void
srlock(struct stdata *sd)
{
	assert(sd);
	read_lock(&sd->sd_lock);
}
static inline void
srunlock(struct stdata *sd)
{
	assert(sd);
	read_unlock(&sd->sd_lock);
}

static inline void
qlockinit(queue_t *q)
{
	assert(q);
	rwlock_init(&q->q_lock);
}
static inline unsigned long
qwlock(queue_t *q)
{
	unsigned long pl;
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);
	pl = zrlock(sd);
	write_lock(&q->q_lock);
	return (pl);
}
static inline void
qwunlock(queue_t *q, unsigned long pl)
{
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);
	write_unlock(&q->q_lock);
	zrunlock(sd, pl);
}
static inline unsigned long
qrlock(queue_t *q)
{
	unsigned long pl;

	assert(q);
	read_lock_irqsave(&q->q_lock, pl);
	return (pl);
}
static inline void
qrunlock(queue_t *q, unsigned long pl)
{
	assert(q);
	read_unlock_irqrestore(&q->q_lock, pl);
}

#endif

#endif

#endif				/* __LOCAL_STRUTIL_H__ */
