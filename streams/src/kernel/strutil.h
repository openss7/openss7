/*****************************************************************************

 @(#) $RCSfile: strutil.h,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2005/07/29 12:58:43 $

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

 Last Modified $Date: 2005/07/29 12:58:43 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRUTIL_H__
#define __LOCAL_STRUTIL_H__

/* global synchq */
extern struct syncq *global_syncq;

extern int __rmvq(queue_t *q, mblk_t *mp);
extern int __flushq(queue_t *q, int flag, mblk_t ***mppp);

/* common inlines */

/* queue band gets and puts */
static __inline__ qband_t *
bget(qband_t *qb)
{
	struct qbinfo *qbi;

	if (qb) {
		qbi = (typeof(qbi)) qb;
		if (atomic_read(&qbi->qbi_refs) < 1)
			swerr();
		atomic_inc(&qbi->qbi_refs);
	}
	return (qb);
}
static __inline__ void
bput(qband_t **bp)
{
	qband_t *qb;

	if ((qb = xchg(bp, NULL))) {
		struct qbinfo *qbi = (typeof(qbi)) qb;

		if (atomic_read(&qbi->qbi_refs) >= 1) {
			if (atomic_dec_and_test(&qbi->qbi_refs))
				freeqb(qb);
		} else
			swerr();
	}
}

/* queue gets and puts */
static __inline__ queue_t *
qget(queue_t *q)
{
	struct queinfo *qu;

	if (q) {
		qu = (typeof(qu)) RD(q);
		if (atomic_read(&qu->qu_refs) < 1)
			swerr();
		atomic_inc(&qu->qu_refs);
	}
	return (q);
}
static __inline__ void
qput(queue_t **qp)
{
	queue_t *q;

	if ((q = xchg(qp, NULL))) {
		queue_t *rq = RD(q);
		struct queinfo *qu = (typeof(qu)) rq;

		if (atomic_read(&qu->qu_refs) >= 1) {
			if (atomic_dec_and_test(&qu->qu_refs))
				freeq(rq);
		} else
			swerr();
	}
}

/*
 *  This file contains a set of specialized STREAMS kernel locks that have the following
 *  charactersistics:
 *
 *  - they are read/write locks
 *  - taking a write lock suppresses local hard interrupts
 *  - taking a read lock suppresses local soft interrupts
 *  - a processor can take a write lock on a lock that it has already write locked and it remains
 *    write locked for that processor
 *  - a processor can take a read lock on a lock that it has already write locked and it remains
 *    write locked for that processor
 *  - a processor can take a read lock on a lock that is unlocked or already read locked
 *  - the caller does not have to supply a "flags" argument, flags are saved internally
 *  - a blockable process can choose to wait on a write lock rather than spinning.
 *  - a blockable process can choose to wait on a write lock or signals rather than spinning.
 *
 *  These characteristics meet the needs of STREAMS as follows:
 *
 *  - write locks can be taken from all contexts except hard irq.
 *  - read locks can be taken from all contexts including hard irq.
 */

#if 0
/* defined in include/sys/streams/dki.h */
typedef struct klock {
	unsigned long kl_isrflags;
	rwlock_t kl_lock;
	struct task_struct *kl_owner;
	uint kl_nest;
	wait_queue_head_t kl_waitq;
} klock_t;
#endif

static __inline__ void
klockinit(klock_t *kl)
{
	kl->kl_isrflags = 0;
	rwlock_init(&kl->kl_lock);
	kl->kl_owner = NULL;
	kl->kl_nest = 0;
	init_waitqueue_head(&kl->kl_waitq);
}
static __inline__ void
kwlock(klock_t *kl)
{
	if (kl->kl_owner == current)
		kl->kl_nest++;
	else {
		unsigned long flags;

		local_irq_save(flags);
		write_lock(&kl->kl_lock);
		kl->kl_isrflags = flags;
		kl->kl_owner = current;
		kl->kl_nest = 0;
	}
}
static __inline__ int
kwtrylock(klock_t *kl)
{
	int locked = 1;

	if (kl->kl_owner == current)
		kl->kl_nest++;
	else {
		unsigned long flags;

		local_irq_save(flags);
		if (write_trylock(&kl->kl_lock)) {
			kl->kl_isrflags = flags;
			kl->kl_owner = current;
			kl->kl_nest = 0;
		} else {
			local_irq_restore(flags);
			locked = 0;
		}
	}
	return (locked);
}
static __inline__ void
kwlock_wait(klock_t *kl)
{
	if (kl->kl_owner == current)
		kl->kl_nest++;
	else if (!kwtrylock(kl)) {
		DECLARE_WAITQUEUE(wait, current);
		add_wait_queue_exclusive(&kl->kl_waitq, &wait);
		for (;;) {
			set_current_state(TASK_UNINTERRUPTIBLE);
			if (kwtrylock(kl))
				break;
			schedule();
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&kl->kl_waitq, &wait);
	}
}
static __inline__ int
kwlock_wait_sig(klock_t *kl)
{
	int err = 0;

	if (kl->kl_owner == current)
		kl->kl_nest++;
	else if (!kwtrylock(kl)) {
		DECLARE_WAITQUEUE(wait, current);
		add_wait_queue_exclusive(&kl->kl_waitq, &wait);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (signal_pending(current)) {
				err = -EINTR;
				break;
			}
			if (kwtrylock(kl))
				break;
			schedule();
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&kl->kl_waitq, &wait);
	}
	return (err);
}
static __inline__ void
kwunlock(klock_t *kl)
{
	if (kl->kl_nest > 0)
		kl->kl_nest--;
	else {
		unsigned long flags = kl->kl_isrflags;

		kl->kl_owner = NULL;
		kl->kl_nest = 0;
		if (waitqueue_active(&kl->kl_waitq))
			wake_up(&kl->kl_waitq);
		write_unlock(&kl->kl_lock);
		local_irq_restore(flags);
	}
}
static __inline__ void
krlock(klock_t *kl)
{
	if (kl->kl_owner == current)
		kl->kl_nest++;
	else
		read_lock_bh(&kl->kl_lock);
}
static __inline__ void
krunlock(klock_t *kl)
{
	if (kl->kl_nest > 0)
		kl->kl_nest--;
	else {
		if (waitqueue_active(&kl->kl_waitq))
			wake_up(&kl->kl_waitq);
		read_unlock_bh(&kl->kl_lock);
	}
}
static __inline__ void
krlock_irqsave(klock_t *kl, unsigned long *flagsp)
{
	if (kl->kl_owner == current) {
		kl->kl_nest++;
		*flagsp = 0;
	} else
		read_lock_irqsave(&kl->kl_lock, *flagsp);
}
static __inline__ void
krunlock_irqrestore(klock_t *kl, unsigned long *flagsp)
{
	if (kl->kl_nest > 0)
		kl->kl_nest--;
	else {
		if (waitqueue_active(&kl->kl_waitq))
			wake_up(&kl->kl_waitq);
		read_unlock_irqrestore(&kl->kl_lock, *flagsp);
	}
}

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

/* for stream heads from queues */
#define hwlock(__q)			kwlock(&(qstream(__q))->sd_klock)
#define hwtrylock(__q)			kwtrylock(&(qstream(__q))->sd_klock)
#define hwlock_wait(__q)		kwlock_wait(&(qstream(__q))->sd_klock)
#define hwlock_wait_sig(__q)		kwlock_wait_sig(&(qstream(__q))->sd_klock)
#define hwunlock(__q)			kwunlock(&(qstream(__q))->sd_klock)
#define hrlock(__q)			krlock(&(qstream(__q))->sd_klock)
#define hrunlock(__q)			krunlock(&(qstream(__q))->sd_klock)

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
#define qrlock_irqsave(__q,__f)		krlock_irqsave(&(__q)->q_klock,(__f))
#define qrunlock_irqrestore(__q,__f)	krunlock_irqrestore(&(__q)->q_klock,(__f))


#endif				/* __LOCAL_STRUTIL_H__ */
