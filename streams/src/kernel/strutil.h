/*****************************************************************************

 @(#) $RCSfile: strutil.h,v $ $Name:  $($Revision: 0.9.2.23 $) $Date: 2005/07/28 14:13:56 $

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

 Last Modified $Date: 2005/07/28 14:13:56 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRUTIL_H__
#define __LOCAL_STRUTIL_H__

/* global synchq */
extern struct syncq *global_syncq;

extern int __rmvq(queue_t *q, mblk_t *mp);
extern int __flushq(queue_t *q, int flag, mblk_t ***mppp);

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
	struct stdata *sd = qstream(q);
	srlock(sd);
	if (sd->sd_other)
		srlock(sd->sd_other);
}
static __inline__ void
hrunlock(queue_t *q)
{
	struct stdata *sd = qstream(q);
	if (sd->sd_other)
		srunlock(sd->sd_other);
	srunlock(sd);
}
static __inline__ void
hwlock(queue_t *q)
{
	struct stdata *sd = qstream(q);
	swlock(sd);
	if (sd->sd_other)
		swlock(sd->sd_other);
}
static __inline__ void
hwunlock(queue_t *q)
{
	struct stdata *sd = qstream(q);
	if (sd->sd_other)
		swunlock(sd->sd_other);
	swunlock(sd);
}
static __inline__ void
hwlock_irqsave(queue_t *q, unsigned long *flagsp)
{
	local_irq_save(*flagsp);
	hwlock(q);
}
static __inline__ void
hwunlock_irqrestore(queue_t *q, unsigned long *flagsp)
{
	hwunlock(q);
	local_irq_restore(*flagsp);
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

#endif				/* __LOCAL_STRUTIL_H__ */
