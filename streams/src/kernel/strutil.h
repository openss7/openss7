/*****************************************************************************

 @(#) $RCSfile: strutil.h,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/04/22 12:08:34 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/22 12:08:34 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRUTIL_H__
#define __LOCAL_STRUTIL_H__

/* common inlines */

/* queue structure read/write locks */
static __inline__ void qrlock(queue_t *q, ulong *flagsp)
{
	if (flagsp)
		local_irq_save(flagsp);
	if (q->q_owner != current)
		read_lock(&q->q_rwlock);
	else {
		assure(!in_interrupt() || in_streams());
		q->q_nest++;
	}
	return;
}
static __inline__ void qrunlock(queue_t *q, ulong *flagsp)
{
	if (q->q_owner != current)
		read_unlock(&q->q_rwlock);
	else {
		assure(!in_interrupt() || in_streams());
		q->q_nest--;
	}
	if (flagsp)
		local_irq_restore(flagsp);
	return;
}
static __inline__ void qwlock(queue_t *q, ulong *flagsp)
{
	if (flagsp)
		local_irq_save(flagsp);
	if (q->q_owner != current) {
		write_lock(&q->q_rwlock);
		q->q_owner = current;
		q->q_nest = 0;
	} else {
		assure(!in_interrupt() || in_streams());
		q->q_nest++;
	}
	return;
}
static __inline__ void qwunlock(queue_t *q, ulong *flagsp)
{
	if (q->q_owner == current) {
		if (q->q_nest <= 0) {
			q->q_owner = NULL;
			q->q_nest = 0;
			write_unlock(&q->q_rwlock);
		} else {
			assure(!in_interrupt() || in_streams());
			q->q_nest--;
		}
		if (flagsp)
			local_irq_restore(flagsp);
		return;
	}
	swerr();
}
static __inline__ void qlockinit(queue_t *q)
{
	q->q_owner = NULL;
	q->q_nest = 0;
	rwlock_init(&q->q_rwlock);
}

/* queue pair nesting read/write locks */
static __inline__ void qprlock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;
	if (qu->qu_owner != current)
		read_lock(&qu->qu_lock);
	else {
		assure(!in_interrupt() || in_streams());
		qu->qu_nest++;
	}
	return;
}
static __inline__ void qprunlock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;
	if (qu->qu_owner != current)
		read_unlock(&qu->qu_lock);
	else {
		assure(!in_interrupt() || in_streams());
		qu->qu_nest--;
	}
	return;
}
static __inline__ void qpwlock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;
	if (qu->qu_owner != current) {
		write_lock(&qu->qu_lock);
		qu->qu_nest = 0;
		qu->qu_owner = current;
	} else {
		assure(!in_interrupt() || in_streams());
		qu->qu_nest++;
	}
	return;
}
static __inline__ void qpwunlock(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;
	if (qu->qu_owner == current) {
		if (qu->qu_nest <= 0) {
			qu->qu_owner = NULL;
			qu->qu_nest = 0;
			write_unlock(&qu->qu_lock);
		} else {
			assure(!in_interrupt() || in_streams());
			qu->qu_nest--;
		}
		return;
	}
	swerr();
}
static __inline__ void qpupgrade(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;
	if (qu->qu_owner == current) {
		assure(!in_interrupt() || in_streams());
		return;
	}
	read_unlock(&qu->qu_lock);
	/* this is not a fully atomic upgrade - but that's ok too */
	write_lock(&qu->qu_lock);
	qu->qu_nest = 0;
	qu->qu_owner = current;
}
static __inline__ void qplockinit(queue_t *rq)
{
	struct queinfo *qu = (struct queinfo *) rq;
	qu->qu_owner = NULL;
	qu->qu_nest = 0;
	rwlock_init(&qu->qu_lock);
}

/* stream head wread/write locks */
static __inline__ void srlock(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current) {
			assure(!in_interrupt() || in_streams());
			sd->sd_nest++;
			return;
		}
		read_lock(&sd->sd_qlock);
		return;
	}
	swerr();
}
static __inline__ void srunlock(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current) {
			assure(!in_interrupt() || in_streams());
			sd->sd_nest--;
			return;
		}
		read_unlock(&sd->sd_qlock);
		return;
	}
	swerr();
}
static __inline__ void swlock(struct stdata *sd, unsigned long *flagsp)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current) {
			assure(!in_interrupt() || in_streams());
			sd->sd_nest++;
			return;
		}
		write_lock_irqsave(&sd->sd_qlock, flagsp);
		sd->sd_nest = 0;
		sd->sd_owner = current;
		return;
	}
	swerr();
}
static __inline__ void swunlock(struct stdata *sd, unsigned long *flagsp)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current) {
			assure(!in_interrupt() || in_streams());
			if (--sd->sd_nest > 0)
				return;
		}
		sd->sd_owner = NULL;
		sd->sd_nest = 0;
		write_unlock_irqrestore(&sd->sd_qlock, flagsp);
		return;
	}
	swerr();
}
static __inline__ void __swlock(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current) {
			assure(!in_interrupt() || in_streams());
			sd->sd_nest++;
			return;
		}
		write_lock(&sd->sd_qlock);
		sd->sd_nest = 0;
		sd->sd_owner = current;
		return;
	}
	swerr();
}
static __inline__ void __swunlock(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		if (sd->sd_owner == current) {
			assure(!in_interrupt() || in_streams());
			if (--sd->sd_nest > 0)
				return;
		}
		sd->sd_owner = NULL;
		sd->sd_nest = 0;
		write_unlock(&sd->sd_qlock);
		return;
	}
	swerr();
}
static __inline__ void slockinit(struct stdata *sd)
{
	if (likely(sd != NULL)) {
		rwlock_init(&sd->sd_qlock);
		sd->sd_nest = 0;
		sd->sd_owner = NULL;
	}
	swerr();
}
static __inline__ void hrlock(queue_t *rq)
{
	srlock(((struct queinfo *) rq)->qu_str);
}
static __inline__ void hrunlock(queue_t *rq)
{
	srunlock(((struct queinfo *) rq)->qu_str);
}
static __inline__ void hwlock(queue_t *rq, unsigned long *flagsp)
{
	swlock(((struct queinfo *) rq)->qu_str, flagsp);
}
static __inline__ void hwunlock(queue_t *rq, unsigned long *flagsp)
{
	swunlock(((struct queinfo *) rq)->qu_str, flagsp);
}
static __inline__ void __hwlock(queue_t *rq)
{
	__swlock(((struct queinfo *) rq)->qu_str);
}
static __inline__ void __hwunlock(queue_t *rq)
{
	__swunlock(((struct queinfo *) rq)->qu_str);
}

/* queue band gets and puts */
static __inline__ qband_t *bget(qband_t *qb)
{
	if (qb) {
		struct qbinfo *qbi = (typeof(qbi)) qb;
		atomic_inc(&qbi->qbi_refs);
	}
	return (qb);
}
static __inline__ void bput(qband_t **bp)
{
	qband_t *qb;
	if ((qb = xchg(bp, NULL))) {
		struct qbinfo *qbi = (typeof(qbi)) qb;
		if (atomic_dec_and_test(&qbi->qbi_refs))
			freeqb(qb);
	}
}

/* queue gets and puts */
static __inline__ queue_t *qget(queue_t *q)
{
	if (q) {
		struct queinfo *qu = (typeof(qu)) RD(q);
		atomic_inc(&qu->qu_refs);
	}
	return (q);
}
static __inline__ void qput(queue_t **qp)
{
	queue_t *q;
	if ((q = xchg(qp, NULL))) {
		queue_t *rq = RD(q);
		struct queinfo *qu = (typeof(qu)) rq;
		if (atomic_dec_and_test(&qu->qu_refs))
			freeq(rq);
	}
}

/* 
 *  Syncrhronization queues...
 */
static __inline__ int shared_tryentersq(queue_t *q)
{
	struct syncq *sq;
	if ((sq = q->q_syncq)) {
		unsigned long flags;
		int result = 0;
		spin_lock_irqsave(&sq->sq_lock, flags);
		spin_unlock_irqrestore(&sq->q_lock, flags);
		return (result);
	}
	return (1);
}
static __inline__ int exclus_tryentersq(queue_t *q)
{
	struct syncq *sq;
	if ((sq = q->q_syncq)) {
		unsigned long flags;
		int result = 0;
		spin_lock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_count == 0) {
			sq->sq_count = -1;
			result = 1;
		}
		spin_unlock_irqrestore(&sq->q_lock, flags);
		return (result);
	}
	return (1);
}
#if 0
static __inline__ void shared_leavesq(queue_t *q)
{
	struct syncq *sq;
	if ((sq = q->q_syncq)) {
		unsigned long flags;
		struct strevent *se, *se_next;
		spin_lock_irqsave(&sq->sq_lock, flags);
		if ((--sq->sq_count) <= 0) {
			sq->sq_count = 0;
			if (sq->sq_head) {
				if (in_streams()) {
					sq->sq_head = NULL;
					sq->sq_tail = &sq->sq_head;
					for (se_next = se; (se = se_next);) {
						se_next = se->se_next;
						/* if we in streams, run them now */
						spin_unlock_irqrestore(&sq->sq_lock, flags);
						qrunevent(q, se);
						spin_lock_irqsave(&sq->sq_lock, flags);
					}
				} else
					/* if not in streams; run them later */
					qschedevents(q);
			}
			if (waitqueue_active(&sq->sq_waitq))
				wake_up_all(&sq->sq_waitq);
			sq->sq_count = 0;
		}
		spin_unlock_irqrestore(&sq->q_lock, flags);
		return;
	}
}
static __inline__ void exclus_leavesq(queue_t *q)
{
	struct syncq *sq;
	if ((sq = q->q_syncq)) {
		unsigned long flags;
		struct strevent *se, *se_next;
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
					spin_lock_irqsave(&sq->sq_lock, flags);
				}
			} else
				/* if not in streams; run them later */
				qschedevents(q);
		}
		if (waitqueue_active(&sq->sq_waitq))
			wake_up_all(&sq->sq_waitq);
		sq->sq_count = 0;
		spin_unlock_irqrestore(&sq->q_lock, flags);
	}
}

static __inline__ void leavesq(queue_t *q, struct syncq *sq)
{
	unsigned long flags;
	if (sq == NULL)
		goto exit;
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
					spin_lock_irqsave(&sq->sq_lock, flags);
					/* need to check for other readers now */
					if ((--sq->sq_count) > 1)
						break;
				} else {
					/* event is exclusive */
					sq->sq_count = -1;
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					doevent(se);
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

static __inline__ void qleavesq(queue_t *q, const int type)
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

static __inline__ int enter_exclus(syncq_t *sq, unsigned long *flagsp)
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
static __inline__ int enter_shared(syncq_t *sq, unsigned long *flagsp)
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
static __inline__ void unlock_syncq(syncq_t *sq, unsigned long *flagsp)
{
	if (sq) {
		spin_unlock_irqrestore(&sq->sq_lock, *flagsp);
	}
}

static __inline__ void leave_exclus(syncq_t *sq)
{
	if (sq) {
		unsigned long flags;
		spin_lock_irqsave(&sq->sq_lock, flags);
		if (!++sq->sq_count) {
			sq->sq_owner = NULL;
			if (sq->sq_head)
				sqsched(sq);
		}
		spin_unlock_irqrestore(&sq->sq_lock, flags);
	}
}
static __inline__ void leave_shared(syncq_t *sq)
{
	if (sq) {
		unsigned long flags;
		spin_lock_irqsave(&sq->sq_lock, flags);
		if (!--sq->sq_count) {
			if (sq->sq_head)
				sqsched(sq);
		}
		spin_unlock_irqrestore(&sq->sq_lock, flags);
	}
}

#endif				/* __LOCAL_STRUTIL_H__ */
