/*****************************************************************************

 @(#) $RCSfile: strutil.h,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2005/07/26 12:50:50 $

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

 Last Modified $Date: 2005/07/26 12:50:50 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRUTIL_H__
#define __LOCAL_STRUTIL_H__

/* global synchq */
extern struct syncq *global_syncq;

/* common inlines */

/* queue structure read/write locks */
static __inline__ void
qrlock(queue_t *q)
{
	if (q->q_owner == current)
		q->q_nest++;
	else
		read_lock(&q->q_rwlock);
	return;
}
static __inline__ void
qrunlock(queue_t *q)
{
	if (q->q_owner == current)
		q->q_nest--;
	else
		read_unlock(&q->q_rwlock);
	return;
}
static __inline__ void
qrlock_irqsave(queue_t *q, ulong *flagsp)
{
	if (flagsp)
		local_irq_save(*flagsp);
	if (q->q_owner == current)
		q->q_nest++;
	else
		read_lock(&q->q_rwlock);
	return;
}
static __inline__ void
qrunlock_irqrestore(queue_t *q, ulong *flagsp)
{
	if (q->q_owner == current)
		q->q_nest--;
	else
		read_unlock(&q->q_rwlock);
	if (flagsp)
		local_irq_restore(*flagsp);
	return;
}
static __inline__ void
qrlock_bh(queue_t *q)
{
	if (q->q_owner == current)
		q->q_nest++;
	else
		read_lock_bh(&q->q_rwlock);
	return;
}
static __inline__ void
qrunlock_bh(queue_t *q)
{
	if (q->q_owner == current)
		q->q_nest--;
	else
		read_unlock_bh(&q->q_rwlock);
	return;
}
static __inline__ void
qwlock(queue_t *q)
{
	if (q->q_owner == current)
		q->q_nest++;
	else {
		write_lock(&q->q_rwlock);
		q->q_owner = current;
		q->q_nest = 0;
	}
	return;
}
static __inline__ void
qwunlock(queue_t *q)
{
	if (q->q_owner == current) {
		if (q->q_nest > 0)
			q->q_nest--;
		else {
			q->q_owner = NULL;
			q->q_nest = 0;
			write_unlock(&q->q_rwlock);
		}
		return;
	}
	swerr();
}
static __inline__ void
qwlock_irqsave(queue_t *q, ulong *flagsp)
{
	if (flagsp)
		local_irq_save(*flagsp);
	if (q->q_owner == current)
		q->q_nest++;
	else {
		write_lock(&q->q_rwlock);
		q->q_owner = current;
		q->q_nest = 0;
	}
	return;
}
static __inline__ void
qwunlock_irqrestore(queue_t *q, ulong *flagsp)
{
	if (q->q_owner == current) {
		if (q->q_nest > 0)
			q->q_nest--;
		else {
			q->q_owner = NULL;
			q->q_nest = 0;
			write_unlock(&q->q_rwlock);
		}
		if (flagsp)
			local_irq_restore(*flagsp);
		return;
	}
	swerr();
}
static __inline__ void
qwlock_bh(queue_t *q)
{
	if (q->q_owner == current)
		q->q_nest++;
	else {
		write_lock_bh(&q->q_rwlock);
		q->q_owner = current;
		q->q_nest = 0;
	}
	return;
}
static __inline__ void
qwunlock_bh(queue_t *q)
{
	if (q->q_owner == current) {
		if (q->q_nest > 0)
			q->q_nest--;
		else {
			q->q_owner = NULL;
			q->q_nest = 0;
			write_unlock_bh(&q->q_rwlock);
		}
		return;
	}
	swerr();
}
static __inline__ void
qlockinit(queue_t *q)
{
	q->q_owner = NULL;
	q->q_nest = 0;
	rwlock_init(&q->q_rwlock);
}

/* queue pair nesting read/write locks */
static __inline__ void
qprlock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;

	if (qu->qu_owner == current)
		qu->qu_nest++;
	else
		read_lock(&qu->qu_lock);
	return;
}
static __inline__ void
qprunlock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;

	if (qu->qu_owner == current)
		qu->qu_nest--;
	else
		read_unlock(&qu->qu_lock);
	return;
}
static __inline__ int
qprtrylock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;

	if (qu->qu_owner == current)
		qu->qu_nest++;
	else {
#if CONFIG_SMP && HAVE_READ_TRYLOCK
		return read_trylock(&qu->qu_lock);
#else
		read_lock(&qu->qu_lock);
#endif

	}
	return (1);
}
static __inline__ void
qpwlock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;

	if (qu->qu_owner == current)
		qu->qu_nest++;
	else {
		write_lock(&qu->qu_lock);
		qu->qu_nest = 0;
		qu->qu_owner = current;
	}
	return;
}
static __inline__ void
qpwunlock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;

	if (qu->qu_owner == current) {
		if (qu->qu_nest > 0)
			qu->qu_nest--;
		else {
			qu->qu_owner = NULL;
			qu->qu_nest = 0;
			write_unlock(&qu->qu_lock);
		}
		return;
	}
	swerr();
}
static __inline__ int
qpwtrylock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;
	int result = 1;

	if (qu->qu_owner == current)
		qu->qu_nest++;
	else if ((result = write_trylock(&qu->qu_lock))) {
		qu->qu_nest = 0;
		qu->qu_owner = current;
	}
	return (result);
}
static __inline__ void
qplockinit(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;

	qu->qu_owner = NULL;
	qu->qu_nest = 0;
	rwlock_init(&qu->qu_lock);
}

#if 0
static __inline__ void
qpupgrade(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;

	if (qu->qu_owner == current) {
		return;
	}
	read_unlock(&qu->qu_lock);
	/* this is not a fully atomic upgrade - but that's ok too */
	write_lock(&qu->qu_lock);
	qu->qu_nest = 0;
	qu->qu_owner = current;
}
#endif

/* stream head wread/write locks */
static __inline__ void
srlock(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current)
			sd->sd_nest++;
		else
			read_lock(&sd->sd_qlock);
		return;
	}
	swerr();
}
static __inline__ void
srunlock(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current)
			sd->sd_nest--;
		else
			read_unlock(&sd->sd_qlock);
		return;
	}
	swerr();
}
static __inline__ void
srlock_bh(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current)
			sd->sd_nest++;
		else
			read_lock_bh(&sd->sd_qlock);
		return;
	}
	swerr();
}
static __inline__ void
srunlock_bh(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current)
			sd->sd_nest--;
		else
			read_unlock_bh(&sd->sd_qlock);
		return;
	}
	swerr();
}
static __inline__ void
swlock_irqsave(struct stdata *sd, unsigned long *flagsp)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current)
			sd->sd_nest++;
		else {
			write_lock_irqsave(&sd->sd_qlock, *flagsp);
			sd->sd_nest = 0;
			sd->sd_owner = current;
		}
		return;
	}
	swerr();
}
static __inline__ void
swunlock_irqrestore(struct stdata *sd, unsigned long *flagsp)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current)
			if (--sd->sd_nest > 0)
				return;
		sd->sd_owner = NULL;
		sd->sd_nest = 0;
		write_unlock_irqrestore(&sd->sd_qlock, *flagsp);
		return;
	}
	swerr();
}
static __inline__ void
swlock(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current)
			sd->sd_nest++;
		else {
			write_lock(&sd->sd_qlock);
			sd->sd_nest = 0;
			sd->sd_owner = current;
		}
		return;
	}
	swerr();
}
static __inline__ void
swunlock(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current)
			if (--sd->sd_nest > 0)
				return;
		sd->sd_owner = NULL;
		sd->sd_nest = 0;
		write_unlock(&sd->sd_qlock);
		return;
	}
	swerr();
}
static __inline__ void
swlock_bh(struct stdata *sd)
{
	local_bh_disable();
	swlock(sd);
}
static __inline__ void
swunlock_bh(struct stdata *sd)
{
	swunlock(sd);
	local_bh_enable();
}
static __inline__ void
slockinit(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		rwlock_init(&sd->sd_qlock);
		sd->sd_nest = 0;
		sd->sd_owner = NULL;
		return;
	}
	swerr();
}
static __inline__ void
hrlock(queue_t *q)
{
	trace();
	ensure(q, return);
	srlock(((struct queinfo *) RD(q))->qu_str);
}
static __inline__ void
hrunlock(queue_t *q)
{
	trace();
	ensure(q, return);
	srunlock(((struct queinfo *) RD(q))->qu_str);
}
static __inline__ void
hwlock_irqsave(queue_t *q, unsigned long *flagsp)
{
	trace();
	swlock_irqsave(((struct queinfo *) RD(q))->qu_str, flagsp);
}
static __inline__ void
hwunlock_irqrestore(queue_t *q, unsigned long *flagsp)
{
	trace();
	swunlock_irqrestore(((struct queinfo *) RD(q))->qu_str, flagsp);
}
static __inline__ void
hwlock(queue_t *q)
{
	swlock(((struct queinfo *) RD(q))->qu_str);
}
static __inline__ void
hwunlock(queue_t *q)
{
	swunlock(((struct queinfo *) RD(q))->qu_str);
}
static __inline__ void
hwlock_bh(queue_t *q)
{
	local_bh_disable();
	hwlock(q);
}
static __inline__ void
hwunlock_bh(queue_t *q)
{
	hwunlock(q);
	local_bh_enable();
}

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

	ptrace(("%s: getting queue %p\n", __FUNCTION__, q));
	if (q) {
		qu = (typeof(qu)) RD(q);
		if (atomic_read(&qu->qu_refs) < 1)
			swerr();
		atomic_inc(&qu->qu_refs);
		printd(("%s: queue %p count is now %d\n", __FUNCTION__, qu,
			atomic_read(&qu->qu_refs)));
	}
	return (q);
}
static __inline__ void
qput(queue_t **qp)
{
	queue_t *q;

	ptrace(("%s: putting queue %p\n", __FUNCTION__, *qp));
	ensure(qp, return);
	if ((q = xchg(qp, NULL))) {
		queue_t *rq = RD(q);
		struct queinfo *qu = (typeof(qu)) rq;

		if (atomic_read(&qu->qu_refs) >= 1) {
			if (atomic_dec_and_test(&qu->qu_refs)) {
				printd(("%s: queue %p is being freed\n", __FUNCTION__, qu));
				freeq(rq);
			} else
				printd(("%s: queue %p count is now %d\n", __FUNCTION__, qu,
					atomic_read(&qu->qu_refs)));
		} else
			swerr();
	}
}

#if defined CONFIG_STREAMS_SYNCQS
/* 
 *  Syncrhronization queues...
 */
static __inline__ int
shared_tryentersq(queue_t *q)
{
	struct syncq *sq;

	if ((sq = q->q_syncq)) {
		unsigned long flags;
		int result = 0;

		/* XXX: are these strict locks necessary? */
		spin_lock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_count >= 0) {
			sq->sq_count++;
			result = 1;
		}
		spin_unlock_irqrestore(&sq->sq_lock, flags);
		return (result);
	}
	return (1);
}
static __inline__ int
exclus_tryentersq(queue_t *q)
{
	struct syncq *sq;

	if ((sq = q->q_syncq)) {
		unsigned long flags;
		int result = 0;

		/* XXX: are these strict locks necessary? */
		spin_lock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_count == 0) {
			sq->sq_count = -1;
			result = 1;
		}
		spin_unlock_irqrestore(&sq->sq_lock, flags);
		return (result);
	}
	return (1);
}

#if 0
static __inline__ void
shared_leavesq(queue_t *q)
{
	struct syncq *sq;

	if ((sq = q->q_syncq)) {
		unsigned long flags;

		/* XXX: are these strict locks necessary? */
		spin_lock_irqsave(&sq->sq_lock, flags);
		if ((--sq->sq_count) <= 0) {
			struct strevent *se;

			sq->sq_count = 0;
			if ((se = sq->sq_head)) {
				if (in_streams()) {
					/* if we are in streams, run them now */
					struct strevent *se_next;
					
					sq->sq_head = NULL;
					sq->sq_tail = &sq->sq_head;
					for (se_next = se; (se = se_next);) {
						se_next = se->se_next;
						spin_unlock_irqrestore(&sq->sq_lock, flags);
						qrunevent(q, se);
						/* XXX: are these strict locks necessary? */
						spin_lock_irqsave(&sq->sq_lock, flags);
					}
				} else
					/* if not in streams; run them later */
					qschedevents(q);
			}
#if 0
			if (waitqueue_active(&sq->sq_waitq))
				wake_up_all(&sq->sq_waitq);
#endif
			sq->sq_count = 0;
		}
		spin_unlock_irqrestore(&sq->sq_lock, flags);
		return;
	}
}
static __inline__ void
exclus_leavesq(queue_t *q)
{
	struct syncq *sq;

	if ((sq = q->q_syncq)) {
		unsigned long flags;
		struct strevent *se, *se_next;

		/* XXX: are these strict locks necessary? */
		spin_lock_irqsave(&sq->sq_lock, flags);
		if ((se = sq->sq_head)) {
			if (in_streams()) {
				sq->sq_head = NULL;
				sq->sq_tail = &sq->sq_head;
				for (se_next = se; (se = se_next);) {
					se_next = se->se_next;
					/* if we in streams, run them now */
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					qrunevent(q, se);
					/* XXX: are these strict locks necessary? */
					spin_lock_irqsave(&sq->sq_lock, flags);
				}
			} else
				/* if not in streams; run them later */
				qschedevents(q);
		}
#if 0
		if (waitqueue_active(&sq->sq_waitq))
			wake_up_all(&sq->sq_waitq);
#endif
		sq->sq_count = 0;
		spin_unlock_irqrestore(&sq->sq_lock, flags);
	}
}

static __inline__ void
leavesq(queue_t *q, struct syncq *sq)
{
	unsigned long flags;

	if (sq == NULL)
		goto exit;
	/* XXX: are these strict locks necessary? */
	spin_lock_irqsave(&sq->sq_lock, flags);
	if (sq->sq_count > 1) {
		/* more shared threads inside */
		sq->sq_count--;
		goto unlock_exit;
	} else if (sq->sq_count == 1 || sq->sq_count == -1) {
		/* no threads inside */
		sq->sq_count = 0;
		/* check for pending events */
		if (in_streams()) {
			struct strevent *se;

			while ((se = sq->sq_head)) {
				if (!(sq->sq_head = se->se_next))
					sq->sq_tail = &sq->sq_head;
				if (EVENT_IS_SHARED(se)) {
					/* event is shared */
					sq->sq_count = 1;
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					qrunevent(q, se);
					/* XXX: are these strict locks necessary? */
					spin_lock_irqsave(&sq->sq_lock, flags);
					/* need to check for other readers now */
					if ((--sq->sq_count) > 1)
						break;
				} else {
					/* event is exclusive */
					sq->sq_count = -1;
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					doevent(se);
					/* XXX: are these strict locks necessary? */
					spin_lock_irqsave(&sq->sq_lock, flags);
				}
				sq->sq_count = 0;;
			}
		} else
			qschedevents(q);
	} else
		swerr();
      unlock_exit:
	spin_unlock_irqrestore(&sq->sq_lock, flags);
      exit:
	return;
}

static __inline__ void
qleavesq(queue_t *q, const int type)
{
	struct syncq *sq = q->q_syncq;

	switch (type) {
	case PERIM_INNER:
		return leavesq(q, sq);
	case PERIM_OUTER:
		if (sq)
			return leavesq(q, sq->sq_outer);
		swerr();
		return;
	}
	never();
}
#endif

static __inline__ int
entex_exclus_irqsave(syncq_t *sq, unsigned long *flagsp)
{
	if (sq) {
		spin_lock_irqsave(&sq->sq_lock, *flagsp);
		if (sq->sq_head || sq->sq_count > 0)
			return (0);	/* with sq locked */
		sq->sq_count--;
		spin_unlock_irqrestore(&sq->sq_lock, *flagsp);
	}
	return (1);		/* with sq unlocked */
}
static __inline__ int
enter_shared_irqsave(syncq_t *sq, unsigned long *flagsp)
{
	if (sq) {
		spin_lock_irqsave(&sq->sq_lock, *flagsp);
		if (sq->sq_head || sq->sq_count < 0)
			return (0);	/* with sq locked */
		sq->sq_count++;
		spin_unlock_irqrestore(&sq->sq_lock, *flagsp);
	}
	return (1);
}
static __inline__ void
unlock_synq_irqrestore(syncq_t *sq, unsigned long *flagsp)
{
	if (sq) {
		spin_unlock_irqrestore(&sq->sq_lock, *flagsp);
	}
}

static __inline__ void
leave_exclus(syncq_t *sq)
{
	if (sq) {
		unsigned long flags;

		/* XXX: do these locks have to be so severe? */
		spin_lock_irqsave(&sq->sq_lock, flags);
		if (!++sq->sq_count) {
			sq->sq_owner = NULL;
			/* If we have a backlog, schedule the thread to process the backlog.  I
			   don't think that we should do this this way, we might as well process
			   the backlog now. */
			if (sq->sq_head)
				sqsched(sq);
		}
		spin_unlock_irqrestore(&sq->sq_lock, flags);
	}
}
static __inline__ void
leave_shared(syncq_t *sq)
{
	if (sq) {
		unsigned long flags;

		/* XXX: do these locks have to be so severe? */
		spin_lock_irqsave(&sq->sq_lock, flags);
		if (!--sq->sq_count) {
			/* If we have a backlog, schedule the thread to process the backlog.  I
			   don't think that we should do this this way, we might as well process
			   the backlog now. */
			if (sq->sq_head)
				sqsched(sq);
		}
		spin_unlock_irqrestore(&sq->sq_lock, flags);
	}
}

/*
 *  enter_inner_syncq_exclus: - enter the inner barrier exclusive
 *  @q:	    the queue against which to synchronize
 *  @se:    the STREAMS event to synchronize
 *  @sqp:   a pointer to the place to return the syncq reference
 */
static __inline__ int
enter_inner_syncq_exclus(queue_t *q, struct strevent *se, struct syncq **sqp)
{
	struct syncq *osq = NULL, *isq = NULL;
	int rval = 1;

	if (unlikely(q == NULL))
		goto done;
	if (unlikely((osq = q->q_syncq) == NULL))
		goto done;
	if (unlikely((osq->sq_flag & D_MTOUTPERIM) == 0)) {
		isq = osq;
		osq = isq->sq_outer;
	}
	{
		unsigned long flags;

		if (osq) {
			spinlock_irqsave(&osq->sq_lock, flags);
			if (osq->sq_owner == current)
				osq->sq_nest++;
			else if (osq->sq_count >= 0) {
				osq->sq_count++;
			} else {
				if (se) {
					/* queue up deferred event */
					se->se_prev = osq->sq_tail;
					se->se_next = *(osq->sq_tail);
					*(osq->sq_tail) = se;
				}
				rval = 0;
			}
			spinunlock_irqrestore(&osq->sq_lock, flags);
		}
		if (rval) {
			spinlock_irqsave(&isq->sq_lock, flags);
			if (isq->sq_owner == current)
				isq->sq_nest++;
			else if (isq->sq_count == 0) {
				isq->sq_owner = current;
				--isq->sq_count;
			} else {
				if (se) {
					/* queue up deferred event */
					se->se_prev = isq->sq_tail;
					se->se_next = *(isq->sq_tail);
					*(isq->sq_tail) = se;
				}
				rval = 0;
			}
			spinunlock_irqrestore(&isq->sq_lock, flags);
		}
	}
      done:
	if (sqp != NULL)
		*sqp = isq;
	/* either in the barriers (rval == 1) or queued up against syncq (rval == 0) */
	return (rval);

}

/*
 *  enter_outer_syncq_wait: = enter outer barrier shared or exclusive from process context
 *  @q:	    the queue against which to synchronize
 *  @se:    the STREAMS even to syncrhonize
 *  @sqp:   a pointer to the place to return the syncq reference
 */
static __inline__ int
enter_outer_syncq_wait(queue_t *q, struct syncq **sqp)
{
	struct syncq *sq = NULL;
	int err = 0;

	if (unlikely(q == NULL))
		goto done;
	if (unlikely((sq = q->q_syncq) == NULL))
		goto done;
	if (unlikely((sq->sq_flag & D_MTOUTPERIM) == 0)) {
		sq = NULL;
		goto done;
	}
	{
		unsigned long flags;
		struct stdata *sd;

		spinlock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_owner == current)
			sq->sq_nest++;
		else if (sq->sq_count == 0) {
			sq->sq_owner = current;
			--sq->sq_count;
		} else {
			DECLARE_WAITQUEUE(wait, current);
			add_wait_queue(&sq->sq_waitq, &wait);
			for (;;) {
				set_current_state(TASK_INTERRUPTIBLE);
				sd = ((struct queinfo *) q)->qu_str;
				if (!sd) {
					err = -ENOSTR;
					break;
				}
				if (sd->sd_flag & (STPLEX | STRCLOSE | STRHUP)) {
					if (test_bit(STPLEX_BIT, &sd->sd_flag)) {
						err = -EINVAL;
						break;
					}
					if (test_bit(STRCLOSE_BIT, &sd->sd_flag)) {
						err = -EIO;
						break;
					}
					if (test_bit(STRHUP_BIT, &sd->sd_flag)) {
						if (sd->sd_flag & (STRISFIFO | STRISPIPE))
							err = -EPIPE;
						else
							err = -ENXIO;
						break;
					}
				}
				if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
					if (sd->sd_other == NULL) {
						err = -EPIPE;
						break;
					}
					if (test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag)) {
						err = -EPIPE;
						break;
					}
				}
				if (signal_pending(current)) {
					err = -EINTR;
					break;
				}
				if (sq->sq_flag & D_MTOCEXCL) {
					if (sq->sq_count == 0
					    || (sq->sq_count == -1 && sq->sq_owner == NULL)) {
						/* available or left for us by leave function */
						/* set exclusive */
						sq->sq_owner = current;
						sq->sq_count = -1;
						err = 0;
						break;
					}
				} else {
					if (sq->sq_count >= 0
					    || (sq->sq_count == -1 && sq->sq_owner == NULL)) {
						/* available or left for us by leave function */
						/* set shared */
						sq->sq_owner = NOLL;
						sq->sq_count =
						    (sq->sq_count == -1) ? 1 : sq->sq_count + 1;
						err = 0;
						break;
					}
				}
				spinunlock_irqrestore(&sq->sq_lock, flags);
				schedule();
				spinlock_irqsave(&sq->sq_lock, flags);
			}
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&sq->sq_waitq, &wait);
		}
		spinunlock_irqrestore(&sq->sq_lock, flags);
	}
      done:
	if (sqp != NULL)
		*sqp = sq;
	return (err);
}

static __inline__ int
leave_outer_syncq_wait(struct syncq *sq)
{
	struct syncq *osq = NULL;

	/* not inside barrier */
	if (likely(sq == NULL))
		return;
	/* no sychronization */
	if (likely((osq = global_sync) == NULL && (osq = q->q_sync) == NULL))
		return;
	/* no outer barrier */
	if (likely((osq->sq_flag & D_MTOUTPERIM) == 0 && (osq = osq->sq_outer) == NULL))
		return;
	if (unlikely(osq != sq))
		swerr();
	{
		unsigned long flags;

		spinlock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_flag & D_MTOCEXCL) {
			if (sq->sq_owner == current && --sq->sq_nest < 0) {
				register struct strevent *se, *se_next;

				sq->sq_nest = 0;
				/* we are still in the barrier exclusive */
				/* just run them all exclusive: anything that wanted to enter the
				   outer perimeter shared and the inner perimeter exclusive will
				   run nicely with just the outer perimeter exclusive; this is
				   because the outer perimeter is always more restrictive than the 
				   inner perimeter. */
				while ((se_next = xchg(&sq->sq_head, NULL))) {	/* MP-SAFE */
					sq->sq_tail = &sq->sq_head;
					/* fake it out that we are STREAMS executive */
					local_bh_disable();
					set_bit(queueflag, &this_thread->flags);
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&sq->sq_lock, flags);
					/* remove fakeout */
					clear_bit(queueflag, &this_thread->flags);
					local_bh_enable();
				}
				/* finally unlock the queue */
				sq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				   owner, and one of the waiters will pick it up, the others will
				   wait some more */
				if (unlikely(waitqueue_active(&sq->sq_waitq)))
					wake_up_all(&sq->sq_waitq);
				else
					sq->sq_count = 0;
			}
		} else {
			if (--sq->sq_count == 0) {
				struct strthread *t = current_thread;
				struct strevent *se;

				/* just left shared for the last time, the last guy out is
				   responsible for processing the backlog */
				sq->sq_nest = 0;
				sq->sq_owner == current;
				sq->sq_count = -1;
				/* we are now in the barrier exclusive */
				/* just run them all exclusive: anything that wanted to enter the
				   outer perimeter shared and the inner perimeter exclusive will
				   run nicely with just the outer perimeter exclusive; this is
				   because the outer perimeter is always more restrictive than the
				   inner perimeter. */
				while ((se_next = xchg(&sq->sq_head, NULL))) {	/* MP-SAFE */
					sq->sq_tail = &sq->sq_head;
					/* fake it out that we are STREAMS executive */
					local_bh_disable();
					set_bit(queueflag, &this_thread->flags);
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&sq->sq_lock, flags);
					/* remove fakeout */
					clear_bit(queueflag, &this_thread->flags);
					local_bh_enable();
				}
				/* finally unlock the queue */
				sq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				   owner, and one of the waiters will pick it up, the others will
				   wait some more */
				if (unlikely(waitqueue_active(&sq->sq_waitq)))
					wake_up_all(&sq->sq_waitq);
				else
					sq->sq_count = 0;
			}
		}
		spinunlock_irqrestore(&sq->sq_lock, flags);
	}
}

/*
 *  enter_outer_syncq_exclus: - enter the outer barrier exclusive
 *  @q:	    the queue against which to synchronize
 *  @se:    the STREAMS event to synchronize
 *  @sqp:   a pointer to the place to return the syncq reference
 */
static __inline__ int
enter_outer_syncq_exclus(queue_t *q, struct strevent *se, struct syncq **sqp)
{
	struct syncq *sq = NULL;
	int rval = 1;

	if (unlikely(q == NULL))
		goto done;
	if (unlikely((sq = q->q_syncq) == NULL))
		goto done;
	if (unlikely((sq->sq_flag & D_MTOUTPERIM) == 0)) {
		sq = NULL;
		goto done;
	}
	{
		unsigned long flags;

		spinlock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_owner == current)
			sq->sq_nest++;
		else if (sq->sq_count == 0) {
			sq->sq_owner = current;
			--sq->sq_count;
		} else {
			if (se) {
				/* queue up deferred event */
				se->se_prev = sq->sq_tail;
				se->se_next = *(sq->sq_tail);
				*(sq->sq_tail) = se;
			}
			rval = 0;
		}
		spinunlock_irqrestore(&sq->sq_lock, flags);
	}
      done:
	if (sqp != NULL)
		*sqp = sq;
	return (rval);
}

/*
 *  leave_outer_syncq_exclus: - leave the outer barrier that was entered exclusive
 *  @sq:    the synchronization queue to leave
 */
static __inline__ void
leave_outer_syncq_exclus(struct syncq *sq)
{
	struct syncq *osq = NULL;

	/* not inside barrier */
	if (likely(sq == NULL))
		return;
	/* no sychronization */
	if (likely((osq = global_sync) == NULL && (osq = q->q_sync) == NULL))
		return;
	/* no outer barrier */
	if (likely((osq->sq_flag & D_MTOUTPERIM) == 0 && (osq = osq->sq_outer) == NULL))
		return;
	if (unlikely(osq != sq))
		swerr();
	{
		unsigned long flags;

		spinlock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_owner == current) {
			if (--sq->sq_nest < 0) {
				register struct strevent *se, *se_next;

				sq->sq_nest = 0;
				/* we are still in the barrier exclusive */
				while ((se_next = xchg(&sq->sq_head, NULL))) {	/* MP-SAFE */
					sq->sq_tail = &sq->sq_head;
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						/* just run them all exclusive: anything that wanted
						 * to enter the outer perimeter shared and the inner
						 * perimeter exclusive will run nicely with just the
						 * outer perimeter exclusive; this is because the
						 * outer perimeter is always more restrictive than
						 * the inner perimeter. */
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&sq->sq_lock, flags);
				}
				/* finally unlock the queue */
				sq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				 * owner, and one of the waiters will pick it up, the others will
				 * wait some more */
				if (unlikely(waitqueue_active(&sq->sq_waitq)))
					wake_up_all(&sq->sq_waitq);
				else
					sq->sq_count = 0;
			}
		} else
			swerr();
		spinunlock_irqrestore(&sq->sq_lock, flags);
	}
}

/*
 *  leave_inner_syncq_exclus: - leave the inner barrier that was entered exclusive
 *  @sq:    the synchronization queue to leave
 */
static __inline__ void
leave_inner_syncq_exclus(struct syncq *sq)
{
	struct syncq *osq = NULL, *isq = NULL;

	if (unlikely(sq == NULL))
		return;
	if (unlikely((osq = q->q_syncq) == NULL))
		return;
	if (unlikely((osq->sq_flag & D_MTOUTPERIM) == 0)) {
		isq = osq;
		osq = isq->sq_outer;
	}
	{
		unsigned long flags;

		spinlock_irqsave(&isq->sq_lock, flags);
		if (isq->sq_owner == current) {
			if (--isq->sq_nest < 0) {
				struct strevent *se;

				isq->sq_nest = 0;
				/* we are still in the barrier exclusive */
				while ((se_next = xchg(&isq->sq_head, NULL))) {	/* MP-SAFE */
					isq->sq_tail = &isq->sq_head;
					spin_unlock_irqrestore(&isq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						/* just run them all exclusive: anything that
						   wanted to enter the inner perimeter shared will
						   run nicely with the inner perimeter exclusive. */
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&isq->sq_lock, flags);
				}
				/* finally unlock the queue */
				isq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				 * owner, and one of the waiters will pick it up, the others will
				 * wait some more */
				if (unlikely(waitqueue_active(&isq->sq_waitq)))
					wake_up_all(&isq->sq_waitq);
				else
					isq->sq_count = 0;
			}
		} else
			swerr();
		spinunlock_irqrestore(&isq->sq_lock, flags);
		if (osq) {
			spinlock_irqsave(&osq->sq_lock, flags);
			if (--osq->sq_count == 0) {
				/* just left shared for the last time */
				struct strthread *t = current_thread;
				struct strevent *se;

				osq->sq_nest = 0;
				osq->sq_owner == current;
				osq->sq_count = -1;
				/* we are now in the barrier exclusive */
				while ((se_next = xchg(&osq->sq_head, NULL))) {	/* MP-SAFE */
					osq->sq_tail = &osq->sq_head;
					spin_unlock_irqrestore(&osq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						/* just run them all exclusive: anything that
						   wanted to enter the outer perimeter shared and
						   the inner perimeter exclusive will run nicely
						   with just the outer perimeter exclusive; this is 
						   because the outer perimeter is always more
						   restrictive than the inner perimeter. */
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&osq->sq_lock, flags);
				}
				/* finally unlock the queue */
				osq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				 * owner, and one of the waiters will pick it up, the others will
				 * wait some more */
				if (unlikely(waitqueue_active(&osq->sq_waitq)))
					wake_up_all(&osq->sq_waitq);
				else
					osq->sq_count = 0;
			}
			spinunlock_irqrestore(&osq->sq_lock, flags);
		}
	}
}
#endif

#endif				/* __LOCAL_STRUTIL_H__ */
