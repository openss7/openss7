/*****************************************************************************

 @(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/03/15 22:11:07 $

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

 Last Modified $Date: 2004/03/15 22:11:07 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/03/15 22:11:07 $"

static char const ident[] = "$RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/03/15 22:11:07 $";

#define __NO_VERSION__

#include <linux/config.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <asm/atomic.h>		/* for atomic operations */
#include <asm/bitops.h>		/* for atomic bit operations */
#include <linux/compiler.h>	/* for expected/unexpected */
#include <linux/spinlock.h>	/* for spinlocks */
#include <linux/kernel.h>	/* for vsnprintf */
#include <linux/sched.h>	/* for wakeup functions */
#include <linux/wait.h>		/* for wait queues */
#include <linux/types.h>	/* for various types */
#include <linux/interrupt.h>	/* for in_interrupt() */

#include <sys/cmn_err.h>	/* for CE_ constants */
#include <sys/strlog.h>		/* for SL_ constants */
#include <sys/kmem.h>		/* for kmem_alloc */

#include <sys/stropts.h>	/* streams definitions */
#include <sys/stream.h>	/* streams definitions */
#include <sys/strsubr.h>	/* for implementation definitions */
#include <sys/strconf.h>	/* for syscontrols */
#include <sys/ddi.h>

#include "strdebug.h"
#include "strhead.h"		/* for str_minfo */
#include "strsysctl.h"		/* for sysctl_str_ defs */
#include "strsched.h"		/* for current_context() */
#include "strutil.h"		/* for q locking and puts and gets */

static inline int db_ref(dblk_t * db)
{
	return (db->db_ref = atomic_read(&db->db_users));
}
static inline void db_set(dblk_t * db, int val)
{
	atomic_set(&db->db_users, val);
	db_ref(db);
}
static inline void db_inc(dblk_t * db)
{
	atomic_inc(&db->db_users);
	db_ref(db);
}
static inline void db_dec(dblk_t * db)
{
	atomic_dec(&db->db_users);
	db_ref(db);
}
static inline int db_dec_and_test(dblk_t * db)
{
	int ret = atomic_dec_and_test(&db->db_users);
	db_ref(db);
	return ret;
}

#define msgblk_offset \
	(((unsigned char *)&((struct mdbblock *)0)->msgblk) - ((unsigned char *)0))
#define datablk_offset \
	(((unsigned char *)&((struct mdbblock *)0)->datablk) - ((unsigned char *)0))
#define databuf_offset \
	(((unsigned char *)&((struct mdbblock *)0)->databuf) - ((unsigned char *)0))

static inline mblk_t *db_to_mb(dblk_t * db)
{
	return ((mblk_t *) ((unsigned char *) db - datablk_offset + msgblk_offset));
}
static inline unsigned char *db_to_buf(dblk_t * db)
{
	return ((unsigned char *) db - datablk_offset + databuf_offset);
}
static inline unsigned char *mb_to_buf(mblk_t *mb)
{
	return ((unsigned char *) mb - msgblk_offset + databuf_offset);
}
static inline dblk_t *mb_to_db(mblk_t *mb)
{
	return ((dblk_t *) ((unsigned char *) mb - msgblk_offset + datablk_offset));
}
static inline struct mdbblock *mb_to_mdb(mblk_t *mb)
{
	return ((struct mdbblock *) ((unsigned char *) mb - msgblk_offset));
}

/* 
 *  ADJMSG
 *  -------------------------------------------------------------------------
 */
int adjmsg(mblk_t *mp, ssize_t length)
{
	mblk_t *bp, *sp;
	int type;
	ssize_t blen, size;
	if (!mp)
		goto error;
	type = mp->b_datap->db_type;
	if (length == 0)
		return (1);	/* success */
	else if (length < 0) {
		/* trim from head of message */
		/* check if we can do the trim */
		for (size = length, bp = mp; bp; bp = bp->b_cont) {
			if (bp->b_datap->db_type != type)
				goto error;
			if ((blen = bp->b_wptr - bp->b_rptr) <= 0)
				continue;
			if ((size -= blen) <= 0)
				goto trim_head;
		}
		goto error;
	      trim_head:
		/* do the trimming */
		for (size = length, bp = mp; bp; bp = bp->b_cont) {
			if ((blen = bp->b_wptr - bp->b_rptr) <= 0)
				continue;
			blen = size < blen ? size : blen;
			bp->b_rptr += blen;
			if ((size -= blen) <= 0)
				return (1);	/* success */
		}
	} else if (length > 0) {
		/* trim from tail of message */
		/* check if we can do the trim */
		for (size = 0, sp = NULL, bp = mp; bp; bp = bp->b_cont) {
			if (bp->b_datap->db_type != type) {
				type = bp->b_datap->db_type;
				sp = bp;
				size = 0;
			}
			if ((blen = bp->b_wptr - bp->b_rptr) <= 0)
				continue;
			size += blen;
		}
		if (size >= 0)
			goto trim_tail;
		goto error;
	      trim_tail:
		/* do the trimming */
		for (bp = sp; bp; bp = bp->b_cont) {
			if ((blen = bp->b_wptr - bp->b_rptr) <= 0)
				continue;
			blen = size < blen ? size : blen;
			bp->b_wptr = bp->b_rptr + blen;
			if ((size -= blen) <= 0)
				return (1);	/* success */
		}
	}
      error:
	return (0);
}

/* 
 *  ALLOCB
 *  -------------------------------------------------------------------------
 */
mblk_t *allocb(size_t size, uint priority)
{
	mblk_t *mp;
	// if (size > sysctl_str_strmsgsz)
	// goto error;
	if ((mp = mdbblock_alloc(priority, &allocb))) {
		struct mdbblock *md = mb_to_mdb(mp);
		unsigned char *base = md->databuf;
		dblk_t *db = &md->datablk.d_dblock;
		if (size <= FASTBUF)
			size = FASTBUF;
		else if (!(base = kmem_alloc(size, KM_NOSLEEP)))
			goto buf_alloc_error;
		/* set up data block */
		db->db_base = base;
		db->db_lim = base + size;
		db_set(db, 1);
		/* set up message block */
		mp->b_rptr = mp->b_wptr = db->db_base;
		mp->b_datap = db;
	}
	return (mp);
      buf_alloc_error:
	mdbblock_free(mp);
	// error:
	return (NULL);
}

/* 
 *  ALLOCB_PHYSREQ
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE mblk_t *allocb_physreq(size_t size, uint priority, void *physreq_ptr);

/* 
 *  COPYB
 *  -------------------------------------------------------------------------
 *  Unlike LiS we do not align the copy.  The driver must me wary of
 *  alignment.
 */
mblk_t *copyb(mblk_t *bp)
{
	mblk_t *mp = NULL;
	if (bp) {
		ssize_t size = bp->b_wptr > bp->b_rptr ? bp->b_wptr - bp->b_rptr : 0;
		if ((mp = allocb(size, BPRI_MED))) {
			bcopy(bp->b_rptr, mp->b_wptr, size);
			mp->b_wptr += size;
			mp->b_datap->db_type = bp->b_datap->db_type;
			mp->b_band = bp->b_band;
			mp->b_flag = bp->b_flag;
		}
	}
	return (mp);
}

/* 
 *  COPYMSG
 *  -------------------------------------------------------------------------
 */
mblk_t *copymsg(mblk_t *msg)
{
	mblk_t *mp = NULL;
	register mblk_t *bp, **mpp = &mp;
	for (bp = msg; bp; bp = bp->b_cont, mpp = &(*mpp)->b_cont)
		if (!(*mpp = copyb(bp)))
			goto error;
	return (mp);
      error:
	freemsg(mp);
	return (NULL);
}

/* 
 *  DATAMSG
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int datamsg(unsigned char type);

/* 
 *  DUPB
 *  -------------------------------------------------------------------------
 */
mblk_t *dupb(mblk_t *bp)
{
	mblk_t *mp;
	if ((mp = mdbblock_alloc(BPRI_HI, &dupb))) {
		struct mdbblock *md = (struct mdbblock *) mp;
		dblk_t *db = bp->b_datap;
		db_inc(db);
		*mp = *bp;
		mp->b_next = mp->b_prev = mp->b_cont = NULL;
		/* mark datab unused */
		db = &md->datablk.d_dblock;
		db_set(db, 0);
	}
	return (mp);
}

/* 
 *  DUPMSG
 *  -------------------------------------------------------------------------
 */
mblk_t *dupmsg(mblk_t *bp)
{
	mblk_t *mp = NULL, **mpp = &mp;
	for (; bp; bp = bp->b_cont, mpp = &(*mpp)->b_cont)
		if (!(*mpp = dupb(bp)))
			goto error;
	return (mp);
      error:
	freemsg(mp);
	return (NULL);
}

/* 
 *  ESBALLOC
 *  -------------------------------------------------------------------------
 */
mblk_t *esballoc(unsigned char *base, size_t size, uint priority, frtn_t *freeinfo)
{
	mblk_t *mp;
	if ((mp = mdbblock_alloc(priority, &esballoc))) {
		struct mdbblock *md = (struct mdbblock *) mp;
		dblk_t *db = &md->datablk.d_dblock;
		/* set up internal buffer with free routine info */
		*(struct free_rtn *) md->databuf = *freeinfo;
		/* set up data block */
		db->db_frtnp = (struct free_rtn *) md->databuf;
		db->db_base = base;
		db->db_lim = base + size;
		db_set(db, 1);
		db->db_type = M_DATA;
		/* set up message block */
		mp->b_rptr = mp->b_wptr = db->db_base;
		mp->b_datap = db;
	}
	return (mp);
}

/* 
 *  FREEB
 *  -------------------------------------------------------------------------
 */
void freeb(mblk_t *mp)
{
	dblk_t *dp, *db;
	/* check null ptr, message still on queue, double free */
	if (!mp || mp->b_queue || !(db = xchg(&mp->b_datap, NULL)))
		goto bug;
	/* message block marked free above */
	if (db_dec_and_test(db)) {
		/* free data block */
		mblk_t *mb = db_to_mb(db);
		if (db->db_base != db_to_buf(db)) {
			/* handle external data buffer */
			if (!db->db_frtnp)
				kmem_free(db->db_base, db->db_lim - db->db_base);
			else
				db->db_frtnp->free_func(db->db_frtnp->free_arg);
		}
		/* the entire mdbblock can go if the associated msgb is also unused */
		if (!mb->b_datap)
			mdbblock_free(mb);
	}
	/* if the message block refers to the associated data block then we have already freed the
	   mdbblock above when necessary; otherwise the entire mdbblock can go if the datab is also 
	   unused */
	if (db != (dp = mb_to_db(mp)) && !db_ref(dp))
		mdbblock_free(mp);
	return;
      bug:
	swerr();
	return;
}

/* 
 *  FREEMSG
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE void freemsg(mblk_t *mp);

/* 
 *  ISDATABLK
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int isdatablk(dblk_t * dp);

/* 
 *  ISDATAMSG
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int isdatamsg(mblk_t *mp);

/* 
 *  LINKB
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE void linkb(mblk_t *mp1, mblk_t *mp2);

/* 
 *  LINKMSG
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE mblk_t *linkmsg(mblk_t *mp1, mblk_t *mp2);

/* 
 *  MSGDSIZE
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE size_t msgdsize(mblk_t *mp);

/* 
 *  MSGPULLUP
 *  -------------------------------------------------------------------------
 */
mblk_t *msgpullup(mblk_t *msg, ssize_t length)
{
	mblk_t *mp = NULL, **mpp = &mp;
	register mblk_t *bp = NULL;
	register ssize_t size, blen, type, len;
	if (!msg)
		goto error;
	if (!(len = length))
		goto copy_rest;
	type = msg->b_datap->db_type;
	size = 0;
	for (bp = msg; bp; bp = bp->b_cont) {
		if ((blen = bp->b_wptr - bp->b_rptr) <= 0)
			continue;
		if (bp->b_datap->db_type == type)
			break;
		if ((size += blen) > len && len > 0)
			goto copy_len;
	}
	if (size <= len)
		goto error;
	if (len < 0)
		len = size;
      copy_len:
	if (!(mp = allocb(len, BPRI_MED)))
		goto error;
	mpp = &mp->b_cont;
	for (bp = msg; bp; bp = bp->b_cont) {
		if ((blen = bp->b_wptr - bp->b_rptr) <= 0)
			continue;
		if (bp->b_datap->db_type != type)
			break;
		if ((size = blen - len) <= 0) {
			bcopy(bp->b_rptr, mp->b_wptr, blen);
			mp->b_wptr += blen;
			len -= blen;
			continue;
		} else {
			bcopy(bp->b_rptr, mp->b_wptr, len);
			mp->b_wptr += len;
			if (!(*mpp = copyb(bp)))
				goto error;
			(*mpp)->b_datap->db_type = bp->b_datap->db_type;
			(*mpp)->b_rptr += size;
			mpp = &(*mpp)->b_cont;
			bp = bp->b_cont;
			break;
		}
	}
      copy_rest:
	if (bp)			/* just copy rest of message */
		if (!(*mpp = copymsg(bp)))
			goto error;
	return (mp);
      error:
	if (mp)
		freemsg(mp);
	return (mp);
}

/* 
 *  MSGSIZE
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE size_t msgsize(mblk_t *mp);

/* 
 *  PULLUPMSG
 *  -------------------------------------------------------------------------
 */
int pullupmsg(mblk_t *mp, ssize_t len)
{
	dblk_t *db, *dp;
	ssize_t size, blen, type;
	mblk_t *bp, **mpp;
	unsigned char *base;
	struct mdbblock *md;
	if (!mp || len < -1)
		goto error;
	if (!len || ((blen = mp->b_wptr - mp->b_rptr) >= len && len >= 0))
		return (1);	/* success */
	size = 0;
	type = mp->b_datap->db_type;
	for (bp = mp; bp; bp = bp->b_cont) {
		if ((blen = bp->b_wptr - bp->b_rptr) <= 0)
			continue;
		if (bp->b_datap->db_type != type)
			break;
		if ((size += blen) > len && len >= 0)
			goto pull_len;
	}
	if (size <= len)
		goto error;
	if (len < 0)
		len = size;
      pull_len:
	db = mp->b_datap;
	if (!(md = (struct mdbblock *) mdbblock_alloc(BPRI_MED, &pullupmsg)))
		goto error;
	if (!(base = kmem_alloc(size, KM_NOSLEEP)))
		goto free_error;
	/* msgb unused */
	dp = &md->datablk.d_dblock;
	dp->db_base = base;
	dp->db_lim = base + size;
	db_set(dp, 1);
	dp->db_type = db->db_type;
	/* copy from old initial datab */
	if ((blen = bp->b_wptr > bp->b_rptr ? bp->b_wptr - bp->b_rptr : 0)) {
		bcopy(mp->b_rptr, base, blen);
		size -= blen;
	}
	/* point old msgb at new datab */
	mp->b_rptr = dp->db_base;
	mp->b_wptr = mp->b_rptr + blen;
	/* remove a reference from old initial datab */
	mp->b_datap = dp;
	db_dec(db);
	for (mpp = &mp->b_cont; (bp = *mpp);) {
		if ((blen = bp->b_wptr > bp->b_rptr ? bp->b_wptr - bp->b_rptr : 0) > 0 &&
		    bp->b_datap->db_type != type)
			break;
		if (size >= blen) {	/* use whole block (even if zero) */
			bcopy(bp->b_rptr, mp->b_wptr, blen);
			mp->b_wptr += blen;
			*mpp = bp->b_cont;
			freeb(bp);
			if ((size -= blen) <= 0)
				break;
			continue;
		} else {	/* use partial block */
			bcopy(bp->b_rptr, mp->b_wptr, size);
			mp->b_wptr += size;
			bp->b_rptr += size;
			size = 0;
			break;
		}
	}
	/* size should be zero here unless there is a bug */
	return (size == 0);
      free_error:
	mdbblock_free((mblk_t *) md);
      error:
	return (0);
}

/* 
 *  RMVB
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE mblk_t *rmvb(mblk_t *mp, mblk_t *bp);

/* 
 *  TESTB
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int testb(size_t size, uint priority);

/* 
 *  UNLINKB
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE mblk_t *unlinkb(mblk_t *mp);

/* 
 *  XMSGSIZE
 *  -------------------------------------------------------------------------
 *  This is not bug-to-bug compatible with LiS.  Some differences: LiS wraps
 *  at a message size of 65636 and cannot handle message blocks larger than
 *  65636.  LiS will consider a non-zero initial block (such as that left by
 *  adjmsg()) as the first message block type when it should not.  This
 *  implementation does not wrap the size, and skips initial zero-length
 *  message blocks.  This implementation of xmsgsize does not span non-zero
 *  blocks of different types.
 */
__EXTERN_INLINE size_t xmsgsize(mblk_t *mp);

/* 
 *  APPQ
 *  -------------------------------------------------------------------------
 */
static int __insq(queue_t *q, mblk_t *emp, mblk_t *nmp);
int appq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	int result;
	unsigned long flags;
	qwlock(q, &flags);
	result = __insq(q, emp ? emp->b_next : emp, nmp);
	qwunlock(q, &flags);
	switch (result) {
	case 3:		/* success - priority enable */
		qenable(q);
		return (1);
	case 2:		/* success - normal enable */
		if (test_bit(QWANTR_BIT, &q->q_flag))
			enableq(q);
	case 1:		/* success */
		return (1);
	default:
		never();
	case 0:		/* failure */
		return (0);
	}
}

/* 
 *  BACKQ
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE queue_t *backq(queue_t *q);

/* 
 *  QBACKENABLE
 *  -------------------------------------------------------------------------
 */
void qbackenable(queue_t *q)
{
	hrlock(q);		/* read lock queue chain */
	for (q = backq(q); q; q = backq(q))
		if (q->q_qinfo->qi_srvp) {
			enableq(q);	/* normal enable */
			break;
		}
	hrunlock(q);
	return;
}

/* 
 *  BCANGET
 *  -------------------------------------------------------------------------
 */
static int __bcangetany(queue_t *q)
{
	struct qband *qb;
	for (qb = q->q_bandp; qb && qb->qb_first; qb = qb->qb_next) ;
	return (qb ? qb->qb_band : 0);
}
static int __bcanget(queue_t *q, unsigned char band)
{
	struct qband *qb;
	for (qb = q->q_bandp; qb && (qb->qb_band > band); qb = qb->qb_next) ;
	if (qb && qb->qb_band == band && !qb->qb_count)
		qb = NULL;
	return (qb ? band : 0);
}
static int __canget(queue_t *q);
int bcanget(queue_t *q, int band)
{
	int result;
	unsigned long flags;
	qrlock(q, &flags);
	switch (band) {
	case 0:
		result = (__canget(q) & 1);	/* mask off backenable bit */
		break;
	case ANYBAND:
		result = __bcangetany(q);
		break;
	default:
		result = __bcanget(q, band);
		break;
	}
	qrunlock(q, &flags);
	return (result);
}

/* 
 *  BCANPUT
 *  -------------------------------------------------------------------------
 *  Don't call these functions with NULL q pointers!  To walk the list of
 *  queues we take a reader lock on the stream head.  We can use simple spins
 *  on the queue because we have already locked out interrupts if outside and
 *  interrupt handler.  We only need a read lock because the bit semantics on
 *  the queue band are atomic.
 *
 *  Use bcanput or bcanputnext with band -1 to check for any writable non-zero band.
 */
static int __bcanputany(queue_t *q)
{
	queue_t *q_next;
	if ((q_next = q)) {
		struct qband *qb;
		unsigned long flags;
		/* find first queue with service procedure or no q_next pointer */
		while ((q = q_next) && !q->q_qinfo->qi_srvp && (q_next = q->q_next)) ;
		qrlock(q, &flags);
		for (qb = q->q_bandp; qb && test_bit(QB_FULL_BIT, &qb->qb_flag); qb = qb->qb_next) ;
		qrunlock(q, &flags);
		/* a non-existent band is considered unwritable */
		return (qb ? 1 : 0);
	}
	swerr();
	return (0);
}
static int __bcanput(queue_t *q, unsigned char band)
{
	queue_t *q_next;
	if ((q_next = q)) {
		struct qband *qb;
		unsigned long flags;
		/* find first queue with service procedure or no q_next pointer */
		while ((q = q_next) && !q->q_qinfo->qi_srvp && (q_next = q->q_next)) ;
		qrlock(q, &flags);
		/* bands are sorted in decending priority so that we can quit the search early for
		   higher priority bands */
		for (qb = q->q_bandp; qb && (qb->qb_band > band); qb = qb->qb_next) ;
		if (qb && qb->qb_band == band && test_bit(QB_FULL_BIT, &qb->qb_flag)) {
			set_bit(QB_WANTW_BIT, &qb->qb_flag);
			qrunlock(q, &flags);
			return (0);
		}
		qrunlock(q, &flags);
		/* a non-existent band is considered empty */
		return (1);
	}
	swerr();
	return (0);
}
static int __canput(queue_t *q);
int bcanput(queue_t *q, int band)
{
	int result;
	hrlock(q);		/* read lock queue chain */
	switch (band) {
	case 0:
		result = __canput(q);
		break;
	case ANYBAND:
		result = __bcanputany(q);
		break;
	default:
		result = __bcanput(q, band);
		break;
	}
	hrunlock(q);
	return (result);
}

/* 
 *  BCANPUTNEXT
 *  -------------------------------------------------------------------------
 */
int bcanputnext(queue_t *q, int band)
{
	int result;
	hrlock(q);		/* read lock queue chain */
	switch (band) {
	case 0:
		result = __canput(q->q_next);
		break;
	case ANYBAND:
		result = __bcanputany(q->q_next);
		break;
	default:
		result = __bcanput(q->q_next, band);
		break;
	}
	hrunlock(q);
	return (result);
}

/* 
 *  CANENABLE
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int canenable(queue_t *q);

/* 
 *  CANGET
 *  -------------------------------------------------------------------------
 *  We treat a canget that fails like a getq that returns null visa vi
 *  backenable.  That way poll and strread that use this will properly
 *  backenable the queue.
 */
static int __canget(queue_t *q)
{
	int result = 1;
	if (!q->q_first) {
		result = 0;
		if (!test_and_set_bit(QWANTR_BIT, &q->q_flag))
			result = 2;
	}
	return (result);
}
int canget(queue_t *q)
{
	int result;
	unsigned long flags;
	qrlock(q, &flags);
	result = __canget(q);
	qrunlock(q, &flags);
	if (result & 2)
		qbackenable(q);
	return (result & 1);
}

/* 
 *  CANPUT
 *  -------------------------------------------------------------------------
 *  Don't call these functions with NULL q pointers!  To walk the list of
 *  queues we take a reader lock on the stream head.
 */
static int __canput(queue_t *q)
{
	queue_t *q_next;
	if ((q_next = q)) {
		/* find first queue with service procedure or no q_next pointer */
		while ((q = q_next) && !q->q_qinfo->qi_srvp && (q_next = q->q_next)) ;
		if (test_bit(QFULL_BIT, &q->q_flag)) {
			set_bit(QWANTW_BIT, &q->q_flag);
			return (0);
		}
		return (1);
	}
	swerr();
	return (0);
}
int canput(queue_t *q)
{
	int result;
	hrlock(q);		/* read lock queue chain */
	result = __canput(q);
	hrunlock(q);
	return (result);
}

/* 
 *  CANPUTNEXT
 *  -------------------------------------------------------------------------
 */
int canputnext(queue_t *q)
{
	int result;
	hrlock(q);		/* read lock queue chain */
	result = __canput(q->q_next);
	hrunlock(q);
	return (result);
}

/* 
 *  FLUSHBAND
 *  -------------------------------------------------------------------------
 */
/* Find a queue band.  This must be called with the queue read or write locked.  */
static inline struct qband *__find_qband(queue_t *q, unsigned char band)
{
	struct qband *qb;
	for (qb = q->q_bandp; qb && qb->qb_band > band; qb = qb->qb_next) ;
	if (qb && qb->qb_band == band)
		return (qb);
	return (NULL);
}
static int __rmvq(queue_t *q, mblk_t *mp);
static int __flushband(queue_t *q, unsigned char band, int flag, mblk_t ***mppp)
{
	mblk_t *mp, *mp_next;
	struct qband *qb;
	int backenable = 0;
	if (band != 0) {
		if ((qb = __find_qband(q, band))) {
			switch (flag) {
			case FLUSHDATA:
				mp_next = qb->qb_first;
				while ((mp = mp_next)) {
					mp_next = mp->b_next;
					if (isdatamsg(mp)) {
						backenable |= __rmvq(q, mp);
						**mppp = mp;
						*mppp = &mp->b_next;
						**mppp = NULL;
					}
				}
				break;
			default:
				swerr();
			case FLUSHALL:
				/* This is faster.  For flushall, we link the qband chain onto the
				   free list and null out qband counts and markers. */
				if ((**mppp = qb->qb_first)) {
					/* link around entire band */
					if (qb->qb_first->b_prev)
						qb->qb_first->b_prev->b_next = qb->qb_last->b_next;
					if (qb->qb_last->b_next)
						qb->qb_last->b_next->b_prev = qb->qb_first->b_prev;
					/* fix up markers */
					if (q->q_first == qb->qb_first)
						q->q_first = qb->qb_last->b_next;
					if (q->q_last == qb->qb_last)
						q->q_last = qb->qb_first->b_prev;
					*mppp = &qb->qb_last->b_next;
					**mppp = NULL;
					qb->qb_count = 0;
					q->q_msgs -= qb->qb_msgs;
					qb->qb_msgs = 0;
					qb->qb_first = qb->qb_last = NULL;
					clear_bit(QB_FULL_BIT, &qb->qb_flag);
					clear_bit(QB_WANTW_BIT, &qb->qb_flag);
					backenable = 1;	/* always backenable when band empty */
				}
				break;
			}
		}
	} else {
		/* Need to treat band zero flushes separately */
		/* Find first band zero message */
		for (mp = q->q_first; mp && mp->b_band > 0; mp = mp->b_next) ;
		switch (flag) {
		case FLUSHDATA:
			mp_next = mp;
			while ((mp = mp_next)) {
				if (isdatamsg(mp)) {
					backenable |= __rmvq(q, mp);
					**mppp = mp;
					*mppp = &mp->b_next;
					**mppp = NULL;
				}
			}
			break;
		default:
			swerr();
		case FLUSHALL:
			if ((**mppp = mp)) {
				/* link around entire list */
				if (mp->b_prev)
					mp->b_prev->b_next = NULL;
				/* fix up markers */
				if (q->q_first == mp)
					q->q_first = NULL;
				if (q->q_last == mp)
					q->q_last = mp->b_prev;
				q->q_pctl = NULL;
				*mppp = &q->q_last->b_next;
				**mppp = NULL;
				q->q_count = 0;
				q->q_msgs = 0;
				clear_bit(QFULL_BIT, &q->q_flag);
				clear_bit(QWANTW_BIT, &q->q_flag);
				backenable = 1;	/* always backenable when band empty */
			}
			break;
		}
	}
	return (backenable);
}
void flushband(queue_t *q, int band, int flag)
{
	int backenable;
	mblk_t *mp = NULL, **mpp = &mp;
	unsigned long flags;
	qwlock(q, &flags);
	backenable = __flushband(q, flag, band, &mpp);
	qwunlock(q, &flags);
	if (backenable)
		qbackenable(q);
	/* we want to free messages with the locks off so that other CPUs can process this queue
	   and we don't block interrupts too long */
	mb();
	freechain(mp, mpp);
}

/* 
 *  FREEZESTR
 *  -------------------------------------------------------------------------
 */
unsigned long freezestr(queue_t *q)
{
	unsigned long flags;
	hwlock(q, &flags);
	write_lock(&q->q_rwlock);
	q->q_iflags = flags;
	return (flags);
}

/* 
 *  GETQ
 *  -------------------------------------------------------------------------
 */
/* faster than __rmvq, same idea */
mblk_t *__getq(queue_t *q, int *be)
{
	mblk_t *mp;
	if ((mp = q->q_first)) {
		clear_bit(QWANTR_BIT, &q->q_flag);
		*be = __rmvq(q, mp);
	} else if (!test_and_set_bit(QWANTR_BIT, &q->q_flag))
		*be = 1;
	return (mp);
}
mblk_t *getq(queue_t *q)
{
	mblk_t *mp;
	unsigned long flags;
	int backenable = 0;
	qwlock(q, &flags);
	mp = __getq(q, &backenable);
	qwunlock(q, &flags);
	if (backenable)
		qbackenable(q);
	return (mp);
}

/* 
 *  INSQ
 *  -------------------------------------------------------------------------
 */
/* Find or create a queue band.  This must be called with the queue write locked.  */
static struct qband *__get_qband(queue_t *q, unsigned char band)
{
	struct qband *qb, *qp, **qbp;
	/* find insertion point for band */
	for (qp = NULL, qbp = &q->q_bandp; *qbp && (*qbp)->qb_band > band;
	     qp = *qbp, qbp = &(*qbp)->qb_next) ;
	if (!(qb = *qbp) || qb->qb_band < band) {
		/* not found, create one */
		if ((qb = allocqb())) {
			if ((qb->qb_next = xchg(qbp, qb)))
				qb->qb_next->qb_prev = qb;
			if ((qb->qb_prev = qp))
				qp->qb_next = qb;
			qb->qb_hiwat = q->q_qinfo->qi_minfo->mi_hiwat;
			qb->qb_lowat = q->q_qinfo->qi_minfo->mi_lowat;
			qb->qb_band = band;
		}
	}
	return (qb);
}
int __putq(queue_t *q, mblk_t *mp);
static int __insq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	int enable = 0;
	struct qband *qb = NULL;
	size_t size;
	if (!emp)
		goto putq;
	if (q != emp->b_queue)
		goto bug;
	/* insert before emp */
	size = msgsize(nmp);
	if (nmp->b_datap->db_type >= QPCTL) {
		enable = 2;	/* always enable on high priority */
		nmp->b_band = 0;
		if (!q->q_pctl)
			q->q_pctl = nmp;
	} else {
		enable = q->q_first ? 0 : 1;	/* on empty queue */
		if (unlikely(nmp->b_band)) {
			if (!(nmp->b_bandp = qb = __get_qband(q, nmp->b_band)))
				goto enomem;
			if (!qb->qb_last)
				qb->qb_first = qb->qb_last = nmp;
			else if (qb->qb_first == emp)
				qb->qb_first = nmp;
		}
	}
	if (q->q_first == emp)
		q->q_first = nmp;
	if ((nmp->b_prev = emp->b_prev))
		nmp->b_prev->b_next = nmp;
	nmp->b_next = emp;
	emp->b_prev = nmp;
	nmp->b_queue = qget(q);
	/* some adding to do */
	q->q_msgs++;
	if (!qb) {
		if ((q->q_count += size) > q->q_hiwat)
			set_bit(QFULL_BIT, &q->q_flag);
	} else {
		qb->qb_msgs++;
		if ((qb->qb_count += size) > qb->qb_hiwat)
			set_bit(QB_FULL_BIT, &qb->qb_flag);
	}
	nmp->b_size = size;
	return (1 + enable);	/* success */
      bug:
	swerr();
      enomem:
	/* couldn't allocate a band structure! */
	return (0);		/* failure */
      putq:
	/* insert at end */
	return __putq(q, nmp);
}
int insq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	int result;
	unsigned long flags;
	qwlock(q, &flags);
	result = __insq(q, emp, nmp);
	qwunlock(q, &flags);
	switch (result) {
	case 3:		/* success - priority enable */
		qenable(q);
		return (1);
	case 2:		/* success - normal enable */
		if (test_bit(QWANTR_BIT, &q->q_flag))
			enableq(q);
	case 1:		/* success */
		return (1);
	default:
		never();
	case 0:		/* failure */
		return (0);
	}
}

/* 
 *  OTHERQ
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE queue_t *OTHERQ(queue_t *q);

/* 
 *  PUT
 *  -------------------------------------------------------------------------
 *  Your put routines must be MT-safe.  That simple.  Don't put to put
 *  routines that don't exist.
 */
static void __put(queue_t *q, mblk_t *mp)
{
	struct queinfo *qu = (typeof(qu)) RD(q);
	q->q_qinfo->qi_putp(q, mp);
	if (unlikely(waitqueue_active(&qu->qu_qwait)))
		wake_up_all(&qu->qu_qwait);
}
static void _put(queue_t *q, mblk_t *mp)
{
	struct syncq *isq;
	unsigned long flags;
#if defined(CONFIG_STREAMS_COMPAT_AIX)
	while (q->q_ftmsg && !(*q->q_ftmsg) (mp) && (q = q->q_next)) ;
#endif
	if ((isq = q->q_syncq)) {
		struct syncq *osq = isq->sq_outer;
		if (enter_shared(osq, &flags)) {
			if (isq->sq_flag & D_MTPUTSHARED) {
				if (enter_shared(isq, &flags)) {
					__put(q, mp);
					leave_shared(isq);
				} else {
					/* can't enter inner perimeter, defer */
					__defer_put(isq, q, mp);
					unlock_syncq(isq, &flags);
				}
			} else {
				if (enter_exclus(isq, &flags)) {
					__put(q, mp);
					leave_exclus(isq);
				} else {
					/* can't enter inner perimeter, defer */
					__defer_put(isq, q, mp);
					unlock_syncq(isq, &flags);
				}
			}
			leave_shared(osq);
		} else {
			/* can't enter outer perimeter, defer */
			__defer_put(osq, q, mp);
			unlock_syncq(osq, &flags);
		}
	}
}
void put(queue_t *q, mblk_t *mp)
{
	hrlock(q);
	_put(q, mp);
	hrunlock(q);
}

/* 
 *  PUTNEXT
 *  -------------------------------------------------------------------------
 */
void putnext(queue_t *q, mblk_t *mp)
{
	hrlock(q);
	_put(q->q_next, mp);
	hrunlock(q);
}

/* 
 *  PUTBQ
 *  -------------------------------------------------------------------------
 */
static int __putbq(queue_t *q, mblk_t *mp)
{
	int enable = 0;
	mblk_t *fmp;
	struct qband *qb = NULL;
	size_t size = msgsize(mp);
	if (mp->b_datap->db_type < QPCTL) {
#if 0
		enable = q->q_first ? 0 : 1;	/* enable on empty queue XXX */
#endif
		if (!mp->b_band) {
			fmp = q->q_first;
		} else {
			struct qband *qp;
			if (!(fmp = q->q_first))
				goto insert;	/* place at head of queue */
			if (!(qb = __get_qband(q, mp->b_band)))
				goto enomem;
			if ((fmp = qb->qb_first))
				goto insert;	/* place at head of existing band */
			/* need to find next higher priority band */
			for (qp = qb->qb_prev; qp && !qb->qb_first; qp = qp->qb_prev) ;
			fmp = qp ? qp->qb_first : q->q_first;
			goto insert;	/* insert before higher priority band */
		}
	      insert:
		/* insert before fmp (first message pointer) */
		if ((mp->b_prev = fmp ? fmp->b_prev : fmp))
			mp->b_prev->b_next = mp;
		if ((mp->b_next = fmp))
			mp->b_next->b_prev = mp;
		/* pull priority marker */
		if (mp->b_datap->db_type >= QPCTL) {
			if (!q->q_pctl)
				q->q_pctl = mp;
		}
		mp->b_queue = qget(q);
		/* pull queue head */
		if (q->q_first == fmp) {
			q->q_first = mp;
			if (!q->q_last)
				q->q_last = mp;
		}
		mp->b_bandp = bget(qb);
		/* pull band head */
		if (qb->qb_first == fmp) {
			qb->qb_first = mp;
			if (!qb->qb_last)
				qb->qb_last = mp;
		}
	} else {
		/* modules should never put priority messages back on a queue */
		enable = 2;	/* and this is why */
		/* always needs to go at head of queue */
		if ((mp->b_next = q->q_first))
			mp->b_next->b_prev = mp;
		mp->b_prev = NULL;
		mp->b_queue = qget(q);
		mp->b_bandp = NULL;
		mp->b_band = 0;
		q->q_first = mp;
		if (!q->q_last)
			q->q_last = mp;
		if (!q->q_pctl)
			q->q_pctl = mp;
	}
	/* some adding to do */
	q->q_msgs++;
	if (!qb) {
		if ((q->q_count += size) > q->q_hiwat)
			set_bit(QFULL_BIT, &q->q_flag);
	} else {
		qb->qb_msgs++;
		if ((qb->qb_count += size) > qb->qb_hiwat)
			set_bit(QB_FULL_BIT, &qb->qb_flag);
	}
	mp->b_size = size;
	return (1 + enable);	/* success */
      enomem:
	/* couldn't allocate a band structure! */
	return (0);		/* failure */
}
int putbq(queue_t *q, mblk_t *mp)
{
	int result;
	unsigned long flags;
	qwlock(q, &flags);
	result = __putbq(q, mp);
	qwunlock(q, &flags);
	switch (result) {
	case 3:		/* success - priority enable */
		qenable(q);
		return (1);
	case 2:		/* success - normal enable */
		if (test_bit(QWANTR_BIT, &q->q_flag))
			enableq(q);
	case 1:		/* success */
		return (1);
	default:
		never();
	case 0:		/* failure */
		/* This should never happen, because it takes a qband structure allocation failure
		   to get here, and since we are putting the message back on the queue, there
		   should already be a qband structure.  Unless, however, putbq() is just used to
		   insert messages ahead of others rather than really putting them back.
		   Nevertheless, a way to avoid this error is to always ensure that a qband
		   structure exists (e.g., with strqset) before calling putbq on a band for the
		   first time. */

		return (0);
	}
}

/* 
 *  PUTCTL1
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int putctl1(queue_t *q, int type, int param);

/* 
 *  PUTCTL2
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int putctl2(queue_t *q, int type, int param1, int param2);

/* 
 *  PUTCTL
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int putctl(queue_t *q, int type);

/* 
 *  PUTNEXTCTL
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int putnextctl(queue_t *q, int type);

/* 
 *  PUTNEXTCTL1
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int putnextctl1(queue_t *q, int type, int param);

/* 
 *  PUTNEXTCTL2
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int putnextctl2(queue_t *q, int type, int param1, int param2);

/* 
 *  PUTQ
 *  -------------------------------------------------------------------------
 *  Optomized for arriving at an empty queue.  This is because in a smoothly
 *  running system queues should be empty.  This is a little better than LiS
 *  in that it does not examine the band of messages on the queue.
 */
int __putq(queue_t *q, mblk_t *mp)
{
	int enable;
	mblk_t *lmp;
	struct qband *qb = NULL;
	size_t size = msgsize(mp);
	if (mp->b_datap->db_type >= QPCTL) {
		enable = 2;	/* always enable on high priority */
		if ((lmp = q->q_pctl))
			goto append;
		/* no priority messages in queue, needs to go at head of queue */
		if ((mp->b_next = q->q_first))
			mp->b_next->b_prev = mp;
		mp->b_prev = NULL;
		mp->b_queue = qget(q);
		mp->b_bandp = NULL;
		mp->b_band = 0;
		q->q_first = mp;
		if (!q->q_last)
			q->q_last = mp;
		q->q_pctl = mp;
	} else {
		enable = q->q_first ? 0 : 1;	/* enable on empty queue */
		if (!mp->b_band) {
			lmp = q->q_last;
		} else {
			struct qband *qp;
			if (!(lmp = q->q_last))
				goto append;	/* place at end of queue */
			if (!(qb = __get_qband(q, mp->b_band)))
				goto enomem;
			if ((lmp = qb->qb_last))
				goto append;	/* place at end of existing band */
			/* need to find next lower priority band */
			for (qp = qb->qb_next; qp && !qp->qb_last; qp = qp->qb_next) ;
			lmp = qp ? qp->qb_last : q->q_last;
			goto append;	/* append after lower priority band */
		}
	      append:
		/* append after lmp (last message pointer) */
		if ((mp->b_next = lmp ? lmp->b_next : lmp))
			mp->b_next->b_prev = mp;
		if ((mp->b_prev = lmp))
			mp->b_prev->b_next = mp;
		/* push priority marker */
		if (mp->b_datap->db_type >= QPCTL) {
			if (q->q_pctl == lmp)
				q->q_pctl = mp;
		}
		mp->b_queue = qget(q);
		/* push queue tail */
		if (q->q_last == lmp) {
			q->q_last = mp;
			if (!q->q_first)
				q->q_first = mp;
		}
		mp->b_bandp = bget(qb);
		/* push band tail */
		if (qb->qb_last == lmp) {
			qb->qb_last = mp;
			if (!qb->qb_first)
				qb->qb_first = mp;
		}
	}
	/* some adding to do */
	q->q_msgs++;
	if (!qb) {
		if ((q->q_count += size) > q->q_hiwat)
			set_bit(QFULL_BIT, &q->q_flag);
	} else {
		qb->qb_msgs++;
		if ((qb->qb_count += size) > qb->qb_hiwat)
			set_bit(QB_FULL_BIT, &qb->qb_flag);
	}
	mp->b_size = size;
	return (1 + enable);	/* success */
      enomem:
	/* couldn't allocate a band structure! */
	return (0);		/* failure */
}
int putq(queue_t *q, mblk_t *mp)
{
	int result;
	unsigned long flags;
	qwlock(q, &flags);
	result = __putq(q, mp);
	qwunlock(q, &flags);
	switch (result) {
	case 3:		/* success - priority enable */
		qenable(q);
		return (1);
	case 2:		/* success - normal enable */
		if (test_bit(QWANTR_BIT, &q->q_flag))
			enableq(q);
	case 1:		/* success */
		return (1);
	default:
		never();
	case 0:		/* failure */
		/* This can happen and it is bad.  We use the return value to putq but it is
		   typically ignored by the module.  One way to ensure that this never happens is
		   to call strqset() for the band before calling putq on the band for the first
		   time. (See also putbq()) */
		return (0);
	}
}

/**
 *  QATTACH - attach a stream head, module or driver queue pair to a stream head
 *
 *  @sd:stream head data structure identifying stream
 *  @fmod:&struct fmodsw pointer identifying module or driver
 *  @devp:&dev_t pointer providing opening device number
 *  @oflag:open flags
 *  @sflag:streams flag, can be DRVOPEN, CLONEOPEN, MODOPEN
 *  @crp:&cred_t pointer to credentials of opening task
 */
int setsq(queue_t *q, struct fmodsw *fmod, int mux);
int qattach(struct stdata *sd, struct fmodsw *fmod, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct streamtab *st;
	queue_t *q;
	dev_t odev;
	struct cdevsw *sdev;
	int err;
	if (!(q = allocq()))
		goto enomem;
	if ((err = setsq(q, fmod, 0)) < 0)
		goto freeq_error;
	qinsert(sd->sd_rq, q);	/* half insert under stream head */
	qprocsoff(q);		/* does not alter q_next pointers, just flags */
	odev = *devp;
	if ((err = qopen(q, devp, oflag, sflag, crp)))
		goto error;
	if (sflag != MODOPEN) {
		/* magic garden */
		/* this just doesn't work with locking.... FIXME */
		if (getmajor(odev) != getmajor(*devp)) {
			if (!(sdev = sdev_get(*devp)))
				goto enoent;
			if (!(st = sdev->d_str))
				goto put_noent;
			setsq(q, (struct fmodsw *) sdev, 0);
			sdev_put(sdev);
		}
	} else if (odev != *devp)
		swerr();
	qprocson(q);		/* in case qopen() forgot */
	return (0);
      put_noent:
	sdev_put(sdev);
      enoent:
	err = -ENOENT;
	qdetach(q, oflag, crp);	/* need to call close */
	return (err);
      error:
	*devp = odev;
	qprocsoff(q);		/* doesn't alter anything if procs still turned off */
	qdelete(q);		/* remove half insert */
	qput(&q);
	return (err);
      freeq_error:
	qput(&q);
	return (err);
      enomem:
	return (-ENOMEM);
}

/**
 *  QCLOSE - invoke a queue pair's qi_qclose entry point
 *
 *  @q:read queue of the queue pair to close
 *  @oflag:open flags
 *  @crp:credentials of closing task
 */
int qclose(queue_t *q, int oflag, cred_t *crp)
{
	int result = ENXIO;
	int (*q_close) (queue_t *, int, cred_t *);
	if ((q_close = q->q_qinfo->qi_qclose))
		if (!(result = q_close(q, oflag, crp)))
			qprocsoff(q);
	return (result);
}

/**
 *  QDELETE - delete a queue pair from a stream
 *
 *  @rq:read queue of queue pair to delete
 *
 *  DESCRIPTION:QDELETE half-deletes the queue pair identified by @rq from the
 *  stream to which it belongs.  The q->q_next pointers of the queue pair to
 *  be deleted, @rq, are adjusted, but the stream remains unaffected.
 *  qprocsoff() must be called before calling QDELETE to properly remove the
 *  queue pair from the stream.
 *
 *  CONTEXT:QDELETE should only be called from the qattach() or qdetach()
 *  procedure or a stream head open or close procedure.
 *
 *  NOTICES:@rq should have already been removed from a queue with qprocsoff()
 *  and must be a valid pointer or bad things will happen.
 */
void qdelete(queue_t *q)
{
	unsigned long flags;
	struct queinfo *qu = (typeof(qu)) q;
	struct stdata *sd = qu->qu_str;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);
	wq = rq + 1;
	swlock(sd, &flags);
	qput(&rq->q_next);
	qput(&wq->q_next);
	qu->qu_str = NULL;
	swunlock(sd, &flags);
	sd_put(sd);
}

/**
 *  QDETACH - detach a queue pair from a stream
 *  @q:read queue pointer of queue pair to detach
 *  @flags:open flags of closing task
 *  @crp:credentials of closing task
 *
 *  DESCRIPTION:QDETACH calls the module queue pair qi_qclose procedure and
 *  then removes the queue pair from the stream.
 *
 *  RETURN:QDETACH returns any error returned by the module's qi_qclose
 *  procedure.
 *
 *  ERRORS:QDETACH can return any error returned by the module's qi_qclose
 *  procedure.  This error is not returned to the user.
 *
 *  CONTEXT:QDETACH is meant to be called in user context.
 */
int qdetach(queue_t *q, int flags, cred_t *crp)
{
	int err = qclose(q, flags, crp);
	qprocsoff(q); /* in case qi_qclose procedure forgot */
	qdelete(q);
	return (err);
}

/**
 *  QINSERT - insert a queue pair below another in a stream
 *  @brq:read queue of queue pair beneath which to insert
 *  @irq:read queue of queue pair to insert
 *
 *  DESCRIPTION:QINSERT half-inserts the queue pair identified by @irq beneath
 *  the queue pair on the stream identified by @brq.  This is only a
 *  half-insert.  The q->q_next pointers of the queue pair to be inserted,
 *  @irq, are adjusted, but the stream remains unaffected.  qprocson() must be
 *  called on @irq to complete the insertion and properly set flags.
 *
 *  CONTEXT:QINSERT is only meant to be called from the qattach() procedure or
 *  a stream head open procedure.
 *
 *  NOTICES:@irq should not already be inserted on a queue or bad things will
 *  happen.
 */
void qinsert(queue_t *brq, queue_t *irq)
{
	struct queinfo *iqu = (typeof(iqu)) irq;
	queue_t *iwq = irq + 1;
	struct queinfo *bqu = (typeof(bqu)) brq;
	queue_t *bwq = brq + 1;
	unsigned long flags;
	hwlock(brq, &flags);
	iqu->qu_str = sd_get(bqu->qu_str);
	irq->q_next = qget(brq);
	if (bwq->q_next != brq) {	/* not a fifo */
		iwq->q_next = qget(bwq->q_next);
	} else {		/* is a fifo */
		iwq->q_next = qget(irq);
	}
	hwunlock(brq, &flags);
}

/**
 *  QOPEN - call a module's qi_qopen entry point
 *  @q:the read queue of the module queue pair to open
 *  @devp:pointer to the opening and returned device number
 *  @oflag:open flags
 *  @sflag:stream flag, can be DRVOPEN, MODOPEN, CLONEOPEN
 *  @crp:pointer to the opening task's credential structure
 */
int qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int (*q_open) (queue_t *, dev_t *, int, int, cred_t *);
	if ((q_open = q->q_qinfo->qi_qopen))
		return q_open(q, devp, oflag, sflag, crp);
	return (-ENOPKG);
}

/**
 *  QPROCSOFF - turn off qi_putp and qi_srvp procedures for a queue pair
 *  @q:read queue pointer for the queue pair to turn procs off
 *
 *  DESCRIPTION:QPROCSOFF marks the queue pair as being owned by the stream
 *  head (disabling further put procedures), marks it as being noenabled,
 *  (which disables further srv procedures), and bypasses the module by
 *  adjusting the q->q_next pointers of upstream modules for each queue in the
 *  queue pair.  This effectively bypasses the module.
 *
 *  CONTEXT:QPROCSOFF should only be called from qattach() or a stream head 
 *  head open procedure.  The user should call QPROCSOFF from the qclose()
 *  procedure before returning.
 *
 *  NOTICES:QPROCSOFF does not fully delete the queue pair from the stream.
 *  It is still half-attached.  Use qdelete() to complete the final removal of
 *  the queue pair from the stream.
 */
void qprocsoff(queue_t *q)
{
	unsigned long flags;
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);
	assert(current_context() <= CTX_STREAMS);
	if (!test_and_set_bit(QHLIST_BIT, &rq->q_flag)) {
		set_bit(QHLIST_BIT, &wq->q_flag);
		set_bit(QNOENB_BIT, &rq->q_flag);
		set_bit(QNOENB_BIT, &wq->q_flag);
		/* spin here until put or srv procedures exit */
		hwlock(rq, &flags);
		/* bypass this module: works for FIFOs and PIPEs too */
		if ((bq = backq(rq))) {
			queue_t *qn = xchg(&bq->q_next, NULL);
			bq->q_next = qget(rq->q_next);
			qput(&qn);
		}
		if ((bq = backq(wq))) {
			queue_t *qn = xchg(&bq->q_next, NULL);
			bq->q_next = qget(wq->q_next);
			qput(&qn);
		}
		hwunlock(rq, &flags);
		/* put procs must check QHLIST bit after acquiring hrlock */
		/* srv procs must check QNOENB bit after acquiring hrlock */
	}
}

/**
 *  QPROCSON - trun on qi_putp and qi_srvp procedure for a queeu pair
 *  @q:read queue pointer for the queue pair to turn procs on
 *
 *  DESCRIPTION:QPROCSON marks the queue pair as being not owned by the stream
 *  head (enabling put procedures), marks it as being enabled (enabling srv
 *  procedures), and intalls the module by adjusting the q->q_next pointers of
 *  the upstream modules for each queue in the queue pair.  This effectively
 *  undoes the bypass created by qprocsoff().
 *
 *  CONTEXT:QPROCSON should only be called from qattach(), qdetach() or a
 *  stream head close procedure.  The user should call QPROCSON from the
 *  qopen() procedure before returning.
 *
 *  NOTICES:QPROCSON does fully inserts the queue pair into the stream.  It
 *  must be half-inserted with qinsert() before QPROCSON can be called.
 */
void qprocson(queue_t *q)
{
	unsigned long flags;
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);
	assert(current_context() <= CTX_STREAMS);
	if (test_and_clear_bit(QHLIST_BIT, &rq->q_flag)) {
		clear_bit(QHLIST_BIT, &wq->q_flag);
		clear_bit(QNOENB_BIT, &rq->q_flag);
		clear_bit(QNOENB_BIT, &wq->q_flag);
		hwlock(rq, &flags);
		/* join this module: works for FIFOs and PIPEs too */
		if ((bq = backq(rq))) {
			queue_t *qn = xchg(&bq->q_next, NULL);
			bq->q_next = qget(rq);
			qput(&qn);
		}
		if ((bq = backq(wq))) {
			queue_t *qn = xchg(&bq->q_next, NULL);
			bq->q_next = qget(wq);
			qput(&qn);
		}
		hwunlock(rq, &flags);
	}
}

/* 
 *  QPOP
 *  -------------------------------------------------------------------------
 */
int qpop(struct stdata *sd, int oflag, cred_t *crp)
{
	if (sd->sd_pushcnt <= 0)
		goto einval;
	/* TODO: should use capabilities here */
	if (sd->sd_pushcnt <= sd->sd_nanchor && current_creds->cr_uid != 0)
		goto eperm;
	sd->sd_pushcnt--;
	qdetach(sd->sd_rq, oflag, crp);
	return (0);
      einval:
	return (-EINVAL);
      eperm:
	return (-EPERM);
}

/* 
 *  QPUSH
 *  -------------------------------------------------------------------------
 */
int qpush(struct stdata *sd, const char *name, dev_t *devp, int oflag, cred_t *crp)
{
	int err;
	struct fmodsw *fmod;
	if (sd->sd_pushcnt >= sysctl_str_nstrpush)
		goto enosr;
	if (!(fmod = smod_get(name)))
		goto einval;
	if ((err = qattach(sd, fmod, devp, oflag, MODOPEN, crp)) != 0)
		goto error;
	sd->sd_pushcnt++;
      error:
	smod_put(fmod);
	return (err);
      einval:
	return (-EINVAL);
      enosr:
	return (-ENOSR);
}

/* 
 *  QREPLY
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE void qreply(queue_t *q, mblk_t *mp);

/* 
 *  QSIZE
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE ssize_t qsize(queue_t *q);

/* 
 *  QCOUNTSTRM (only for LiS compatibility)
 *  -------------------------------------------------------------------------
 */
ssize_t qcountstrm(queue_t *q)
{
	int count = 0;
	hrlock(q);
	while (SAMESTR(q)) {
		q = q->q_next;
		count += q->q_count;
	}
	hrunlock(q);
	return (count);
}

/* 
 *  RD
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE queue_t *RD(queue_t *q);

/* 
 *  RMVQ
 *  -------------------------------------------------------------------------
 */
static int __rmvq(queue_t *q, mblk_t *mp)
{
	int backenable = 0;
	struct qband *qb;
	if (mp->b_prev)
		mp->b_prev->b_next = mp->b_next;
	if (mp->b_next)
		mp->b_next->b_prev = mp->b_prev;
	if (q->q_first == mp)
		q->q_first = mp->b_next;
	if (q->q_last == mp)
		q->q_last = mp->b_prev;
	if (q->q_pctl == mp)
		q->q_pctl = mp->b_prev;
	if (!(qb = mp->b_bandp)) {
		q->q_count -= mp->b_size;
		if (q->q_count < q->q_hiwat)
			clear_bit(QFULL_BIT, &q->q_flag);
		if (q->q_count <= q->q_lowat && test_and_clear_bit(QWANTW_BIT, &q->q_flag))
			backenable = 1;
	} else {
		if (qb->qb_first == mp)
			qb->qb_first = mp->b_next;
		if (qb->qb_last == mp)
			qb->qb_last = mp->b_prev;
		qb->qb_count -= mp->b_size;
		qb->qb_msgs--;
		if (qb->qb_count < qb->qb_hiwat)
			clear_bit(QB_FULL_BIT, &qb->qb_flag);
		if (qb->qb_count <= qb->qb_lowat && test_and_clear_bit(QB_WANTW_BIT, &qb->qb_flag))
			backenable = 1;
	}
	q->q_msgs--;
	qput(&mp->b_queue);
	bput(&mp->b_bandp);
	mp->b_next = mp->b_prev = NULL;
	return (backenable);
}
void rmvq(queue_t *q, mblk_t *mp)
{
	unsigned long flags;
	int backenable;
	q = mp->b_queue;
	/* We ignore the queue pointer provided by the user because we know which queue the message 
	   belongs to (if any). */
	qwlock(q, &flags);
	backenable = __rmvq(q, mp);
	qwunlock(q, &flags);
	if (backenable)
		qbackenable(q);
}

/* 
 *  SAMESTR
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE int SAMESTR(queue_t *q);

/* 
 *  SETQ
 *  -------------------------------------------------------------------------
 */
static void __setq(queue_t *q, struct qinit *rinit, struct qinit *winit)
{
	q->q_qinfo = rinit;
	q->q_maxpsz = rinit->qi_minfo->mi_maxpsz;
	q->q_minpsz = rinit->qi_minfo->mi_minpsz;
	q->q_hiwat = rinit->qi_minfo->mi_hiwat;
	q->q_lowat = rinit->qi_minfo->mi_lowat;
	q++;
	q->q_qinfo = winit;
	q->q_maxpsz = winit->qi_minfo->mi_maxpsz;
	q->q_minpsz = winit->qi_minfo->mi_minpsz;
	q->q_hiwat = winit->qi_minfo->mi_hiwat;
	q->q_lowat = winit->qi_minfo->mi_lowat;
}
void setq(queue_t *q, struct qinit *rinit, struct qinit *winit)
{
	unsigned long flags;
	queue_t *rq = q;
	queue_t *wq = q + 1;
	/* always take read lock before write lock */
	/* nobody else takes two locks */
	qwlock(rq, &flags);
	qwlock(wq, &flags);
	__setq(q, rinit, winit);
	/* unlock in reverse order */
	qwunlock(wq, &flags);
	qwunlock(rq, &flags);
}

struct syncq syncq_global;

/* 
 *  SETSQ
 *  -------------------------------------------------------------------------
 *  Set synchronization queue associated with a new queue pair.
 */
int setsq(queue_t *q, struct fmodsw *fmod, int mux)
{
	struct syncq *sqr, *sqw;
	/* make sure there is none to start */
	sq_put(&(q + 0)->q_syncq);
	sq_put(&(q + 1)->q_syncq);
	if (fmod == NULL)
		return (0);	/* just remove */
	switch (fmod->f_sqlvl) {
	case SQLVL_NOP:	/* none */
		break;
	case SQLVL_QUEUE:
		/* allocate one syncq for each queue */
		if (!(sqr = sq_get()))
			goto enomem;
		if (!(sqw = sq_get())) {
			sq_put(&sqr);
			goto enomem;
		}
		sqr->sq_level = fmod->f_sqlvl;
		sqw->sq_level = fmod->f_sqlvl;
		(q + 0)->q_syncq = sqr;
		(q + 1)->q_syncq = sqw;
		break;
	case SQLVL_QUEUEPAIR:
		/* allocate one syncq for the queue pair */
		if (!(sqr = sq_get()))
			goto enomem;
		sqw = sq_grab(sqr);
		sqr->sq_level = fmod->f_sqlvl;
		sqw->sq_level = fmod->f_sqlvl;
		(q + 0)->q_syncq = sqr;
		(q + 1)->q_syncq = sqw;
		break;
	default:
		swerr();
	case SQLVL_DEFAULT:
	case SQLVL_MODULE:	/* default */
		/* find the module and use its syncq */
		(q + 0)->q_syncq = sq_grab(fmod->f_syncq);
		(q + 1)->q_syncq = sq_grab(fmod->f_syncq);
		break;
	case SQLVL_ELSEWHERE:	/* not fully supported */
		/* find the elsewhere syncq and use it */
		break;
	case SQLVL_GLOBAL:	/* for testing */
		/* use the gloable syncq */
		(q + 0)->q_syncq = sq_grab(&syncq_global);
		(q + 1)->q_syncq = sq_grab(&syncq_global);
		break;
	}
	if (fmod->f_str) {
		if (!mux)
			setq(q, fmod->f_str->st_rdinit, fmod->f_str->st_wrinit);
		else
			setq(q, fmod->f_str->st_muxrinit, fmod->f_str->st_muxwinit);
	} else
		swerr();
	return (0);
      enomem:
	return (-ENOMEM);
}

/* 
 *  STRQGET
 *  -------------------------------------------------------------------------
 */
int strqget(queue_t *q, qfields_t what, unsigned char band, long *val)
{
	int err = 0;
	if (!band) {
		switch (what) {
		case QHIWAT:
			*val = q->q_hiwat;
			break;
		case QLOWAT:
			*val = q->q_lowat;
			break;
		case QMAXPSZ:
			*val = q->q_maxpsz;
			break;
		case QMINPSZ:
			*val = q->q_minpsz;
			break;
		case QCOUNT:
			*val = q->q_count;
			break;
		case QFIRST:
			*val = (long) q->q_first;
			break;
		case QLAST:
			*val = (long) q->q_last;
			break;
		case QFLAG:
			*val = q->q_flag;
			break;
		default:
			err = -EINVAL;
			break;
		}
	} else {
		struct qband *qb;
		unsigned long flags;
		qrlock(q, &flags);
		do {
			if (!(qb = __find_qband(q, band)))
				goto einval;
			switch (what) {
			case QHIWAT:
				*val = qb->qb_hiwat;
				break;
			case QLOWAT:
				*val = qb->qb_lowat;
				break;
			case QMAXPSZ:
				*val = q->q_maxpsz;
				break;
			case QMINPSZ:
				*val = q->q_minpsz;
				break;
			case QCOUNT:
				*val = qb->qb_count;
				break;
			case QFIRST:
				*val = (long) qb->qb_first;
				break;
			case QLAST:
				*val = (long) qb->qb_last;
				break;
			case QFLAG:
				*val = q->q_flag;
				break;
			default:
			      einval:
				err = -EINVAL;
				break;
			}
			break;
		} while (0);
		qrunlock(q, &flags);
	}
	return (-err);
}

/* 
 *  STRQSET
 *  -------------------------------------------------------------------------
 */
int strqset(queue_t *q, qfields_t what, unsigned char band, long val)
{
	int err = 0;
	if (!band) {
		switch (what) {
		case QMAXPSZ:
			q->q_maxpsz = val;
			break;
		case QMINPSZ:
			q->q_minpsz = val;
			break;
		case QHIWAT:
			q->q_hiwat = val;
			break;
		case QLOWAT:
			q->q_lowat = val;
			break;
		case QCOUNT:
		case QFIRST:
		case QLAST:
		case QFLAG:
			err = -EPERM;
			break;
		default:
			err = -EINVAL;
			break;
		}
	} else {
		unsigned long flags;
		struct qband *qb;
		qwlock(q, &flags);
		do {
			if (!(qb = __get_qband(q, band)))
				goto enomem;
			switch (what) {
			case QMAXPSZ:
				q->q_maxpsz = val;
				break;
			case QMINPSZ:
				q->q_minpsz = val;
				break;
			case QHIWAT:
				qb->qb_lowat = val;
				break;
			case QLOWAT:
				qb->qb_lowat = val;
				break;
			case QCOUNT:
			case QFIRST:
			case QLAST:
			case QFLAG:
				err = -EPERM;
				break;
			default:
				err = -EINVAL;
				break;
			}
			break;
		      enomem:
			err = -ENOMEM;
			break;
		} while (0);
		qwunlock(q, &flags);
	}
	return (-err);
}

/* 
 *  STRLOG
 *  -------------------------------------------------------------------------
 */
static spinlock_t str_err_lock = SPIN_LOCK_UNLOCKED;
int strlog(short mid, short sid, char level, unsigned short flag, char *fmt, ...)
{
	unsigned long flags;
	static char str_err_buf[1024];
	va_list args;
	va_start(args, fmt);
	spin_lock_irqsave(&str_err_lock, flags);
	vsnprintf(str_err_buf, sizeof(str_err_buf), fmt, args);
	if (flag & SL_FATAL) {
		printk(KERN_CRIT "strlog(%d)[%d,%d]: %s\n",
		       (int) level, (int) mid, (int) sid, str_err_buf);
	} else if (flag & SL_ERROR) {
		printk(KERN_ERR "strlog(%d)[%d,%d]: %s\n",
		       (int) level, (int) mid, (int) sid, str_err_buf);
	} else if (flag & SL_WARN) {
		printk(KERN_WARNING "strlog(%d)[%d,%d]: %s\n",
		       (int) level, (int) mid, (int) sid, str_err_buf);
	} else if (flag & SL_NOTE) {
		printk(KERN_NOTICE "strlog(%d)[%d,%d]: %s\n",
		       (int) level, (int) mid, (int) sid, str_err_buf);
	} else if (flag & SL_CONSOLE) {
		printk(KERN_INFO "strlog(%d)[%d,%d]: %s\n",
		       (int) level, (int) mid, (int) sid, str_err_buf);
	} else {		/* SL_TRACE */
		printk(KERN_DEBUG "strlog(%d)[%d,%d]: %s\n",
		       (int) level, (int) mid, (int) sid, str_err_buf);
	}
	spin_unlock_irqrestore(&str_err_lock, flags);
	va_end(args);
	return (1);
}

/* 
 *  UNFREEZESTR
 *  -------------------------------------------------------------------------
 */
void unfreezestr(queue_t *q, unsigned long flags)
{
	flags = q->q_iflags;
	write_unlock(&q->q_rwlock);
	hwunlock(q, &flags);
}

/* 
 *  WR
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE queue_t *WR(queue_t *q);

/* 
 *  CMN_ERR
 *  -------------------------------------------------------------------------
 */
static spinlock_t cmn_err_lock = SPIN_LOCK_UNLOCKED;
void vcmn_err(int err_lvl, const char *fmt, va_list args)
{
	unsigned long flags;
	static char cmn_err_buf[1024];
	spin_lock_irqsave(&cmn_err_lock, flags);
	vsnprintf(cmn_err_buf, sizeof(cmn_err_buf), fmt, args);
	switch (err_lvl) {
	case CE_CONT:
		printk("%s", cmn_err_buf);
		break;
	default:
	case CE_NOTE:
		/* gets default log level */
		printk(KERN_NOTICE "%s\n", cmn_err_buf);
		break;
	case CE_WARN:
		printk(KERN_WARNING "%s\n", cmn_err_buf);
		break;
	case CE_PANIC:
		spin_unlock_irqrestore(&cmn_err_lock, flags);
		panic("%s\n", cmn_err_buf);
		return;
	}
	spin_unlock_irqrestore(&cmn_err_lock, flags);
	return;
}
void cmn_err(int err_lvl, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vcmn_err(err_lvl, fmt, args);
	va_end(args);
	return;
}
