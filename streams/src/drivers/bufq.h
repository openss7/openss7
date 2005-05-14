/*****************************************************************************

 @(#) $Id: bufq.h,v 0.9.2.5 2005/05/14 08:34:40 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/05/14 08:34:40 $ by $Author: brian $

 *****************************************************************************/

#ifndef __BUFQ_H__
#define __BUFQ_H__

#ident "@(#) $RCSfile: bufq.h,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/05/14 08:34:40 $"

typedef struct bufq {
	spinlock_t q_lock;
	mblk_t *q_head;
	mblk_t *q_tail;
	size_t q_msgs;
	size_t q_count;
} bufq_t;

static inline void
bufq_init(bufq_t * q)
{
	spin_lock_init(&q->q_lock);
	q->q_head = NULL;
	q->q_tail = NULL;
	q->q_msgs = 0;
	q->q_count = 0;
}
static inline void
bufq_lock(bufq_t * q, unsigned long *flags)
{
	spin_lock_irqsave(&q->q_lock, *flags);
}
static inline void
bufq_unlock(bufq_t * q, unsigned long *flags)
{
	spin_unlock_irqrestore(&q->q_lock, *flags);
}

static inline size_t
bufq_length(bufq_t * q)
{
	return q->q_msgs;
}

static inline size_t
bufq_size(bufq_t * q)
{
	return q->q_count;
}

static inline mblk_t *
bufq_head(bufq_t * q)
{
	return q->q_head;
}

static inline mblk_t *
bufq_tail(bufq_t * q)
{
	return q->q_tail;
}

static inline void
__bufq_add(bufq_t * q, mblk_t *mp)
{
	mblk_t *md = mp;
	q->q_msgs++;
	while (md) {
		if (md->b_wptr > md->b_rptr)
			q->q_count += md->b_wptr - md->b_rptr;
		md = md->b_cont;
	}
	assure(q->q_head);
	assure(q->q_tail);
	assure(q->q_msgs != 1 || !q->q_head->b_next);
	assure(q->q_msgs != 1 || !q->q_tail->b_prev);
}

static inline void
__bufq_sub(bufq_t * q, mblk_t *mp)
{
	mblk_t *md = mp;
	while (md) {
		if (md->b_wptr > md->b_rptr) {
			if (q->q_count >= md->b_wptr - md->b_rptr)
				q->q_count -= md->b_wptr - md->b_rptr;
			else
				q->q_count = 0;
		}
		md = md->b_cont;
	}
	if (!(--q->q_msgs))
		q->q_count = 0;
	assure(q->q_msgs || !q->q_head);
	assure(q->q_msgs || !q->q_tail);
}

static inline void
__bufq_queue(bufq_t * q, mblk_t *mp)
{
	if ((mp->b_prev = q->q_tail))
		mp->b_prev->b_next = mp;
	else
		q->q_head = mp;
	mp->b_next = NULL;
	q->q_tail = mp;
	__bufq_add(q, mp);
}

static inline void
bufq_queue(bufq_t * q, mblk_t *mp)
{
	unsigned long flags;
	ensure(q && mp, return);
	bufq_lock(q, &flags);
	__bufq_queue(q, mp);
	bufq_unlock(q, &flags);
}

static inline void
bufq_queue_head(bufq_t * q, mblk_t *mp)
{
	unsigned long flags;
	ensure(q && mp, return);
	bufq_lock(q, &flags);
	if ((mp->b_next = q->q_head))
		mp->b_next->b_prev = mp;
	else
		q->q_tail = mp;
	mp->b_prev = NULL;
	q->q_head = mp;
	__bufq_add(q, mp);
	bufq_unlock(q, &flags);
}

static inline void
bufq_insert(bufq_t * q, mblk_t *mp, mblk_t *np)
{
	unsigned long flags;
	bufq_lock(q, &flags);
	ensure(q && mp && np, return);
	if ((np->b_prev = mp->b_prev))
		np->b_prev->b_next = np;
	else
		q->q_head = np;
	mp->b_prev = np;
	np->b_next = mp;
	__bufq_add(q, np);
	bufq_unlock(q, &flags);
}

static inline void
bufq_append(bufq_t * q, mblk_t *mp, mblk_t *np)
{
	unsigned long flags;
	ensure(q && mp && np, return);
	bufq_lock(q, &flags);
	if ((np->b_next = mp->b_next))
		np->b_next->b_prev = np;
	else
		q->q_tail = np;
	mp->b_next = np;
	np->b_prev = mp;
	__bufq_add(q, np);
	bufq_unlock(q, &flags);
}

static inline mblk_t *
__bufq_dequeue(bufq_t * q)
{
	mblk_t *mp;
	if ((mp = q->q_head)) {
		if ((q->q_head = mp->b_next))
			mp->b_next->b_prev = NULL;
		else
			q->q_tail = NULL;
		mp->b_next = NULL;
		mp->b_prev = NULL;
		__bufq_sub(q, mp);
	}
	return mp;
}

static inline mblk_t *
bufq_dequeue(bufq_t * q)
{
	unsigned long flags;
	mblk_t *mp;
	ensure(q, return (NULL));
	bufq_lock(q, &flags);
	mp = __bufq_dequeue(q);
	bufq_unlock(q, &flags);
	return mp;
}

static inline mblk_t *
__bufq_dequeue_tail(bufq_t * q)
{
	mblk_t *mp;
	if ((mp = q->q_tail)) {
		if ((q->q_tail = mp->b_prev))
			mp->b_prev->b_next = NULL;
		else
			q->q_head = NULL;
		mp->b_next = NULL;
		mp->b_prev = NULL;
		__bufq_sub(q, mp);
	}
	return (mp);
}

static inline mblk_t *
bufq_dequeue_tail(bufq_t * q)
{
	unsigned long flags;
	mblk_t *mp;
	ensure(q, return (NULL));
	bufq_lock(q, &flags);
	mp = __bufq_dequeue_tail(q);
	bufq_unlock(q, &flags);
	return mp;
}

static inline mblk_t *
__bufq_unlink(bufq_t * q, mblk_t *mp)
{
	ensure(q && mp, return (NULL));
	if (mp->b_next)
		mp->b_next->b_prev = mp->b_prev;
	else
		q->q_tail = mp->b_prev;
	if (mp->b_prev)
		mp->b_prev->b_next = mp->b_next;
	else
		q->q_head = mp->b_next;
	mp->b_next = NULL;
	mp->b_prev = NULL;
	__bufq_sub(q, mp);
	return mp;
}

static inline mblk_t *
bufq_unlink(bufq_t * q, mblk_t *mp)
{
	unsigned long flags;
	ensure(q && mp, return (NULL));
	bufq_lock(q, &flags);
	__bufq_unlink(q, mp);
	bufq_unlock(q, &flags);
	return (mp);
}

/*
   splice bufq2 onto the head of bufq1 
 */
static inline void
bufq_splice_head(bufq_t * q1, bufq_t * q2)
{
	mblk_t *mp;
	ensure(q1 && q2, return);
	while ((mp = bufq_dequeue_tail(q2)))
		bufq_queue_head(q1, mp);
	return;
}

/*
   splice bufq2 onto the tail of bufq1 
 */
static inline void
bufq_splice_tail(bufq_t * q1, bufq_t * q2)
{
	mblk_t *mp;
	ensure(q1 && q2, return);
	while ((mp = bufq_dequeue(q2)))
		bufq_queue(q1, mp);
}

static inline void
bufq_freehead(bufq_t * q)
{
	unsigned long flags;
	bufq_lock(q, &flags);
	if (q->q_head)
		freemsg(__bufq_dequeue(q));
	bufq_unlock(q, &flags);
}

static inline void
bufq_purge(bufq_t * q)
{
	unsigned long flags;
	bufq_lock(q, &flags);
	while (q->q_head)
		freemsg(__bufq_dequeue(q));
	bufq_unlock(q, &flags);
}

static inline void
__bufq_supply(bufq_t * q, mblk_t *mp)
{
	mblk_t *md = mp;

	while (md) {
		md->b_datap->db_type = M_DATA;
		md->b_rptr = md->b_wptr = md->b_datap->db_base;
		__bufq_queue(q, md);
		md = unlinkb(md);
	}
}

static inline void
bufq_supply(bufq_t * q, mblk_t *mp)
{
	unsigned long flags;
	bufq_lock(q, &flags);
	__bufq_supply(q, mp);
	bufq_unlock(q, &flags);
}

static inline mblk_t *
bufq_resupply(bufq_t * q, mblk_t *mp, int maxsize, int maxcount)
{
	unsigned long flags;
	bufq_lock(q, &flags);
	if (bufq_length(q) > maxcount || bufq_size(q) > maxsize) {
		bufq_unlock(q, &flags);
		return mp;
	}
	__bufq_supply(q, mp);
	bufq_unlock(q, &flags);
	return NULL;
}

static inline void
freechunks(mblk_t *mp)
{
	mblk_t *dp, *dp_next;
	for (dp = mp; dp; dp_next = dp->b_next, freemsg(dp), dp = dp_next) ;
}

#endif				/* __BUFQ_H__ */
