/*****************************************************************************

 @(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.39 $) $Date: 2005/07/01 20:17:30 $

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

 Last Modified $Date: 2005/07/01 20:17:30 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.39 $) $Date: 2005/07/01 20:17:30 $"

static char const ident[] = "$RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.39 $) $Date: 2005/07/01 20:17:30 $";

#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>

#include <asm/atomic.h>		/* for atomic operations */
#include <asm/bitops.h>		/* for atomic bit operations */
#include <linux/compiler.h>	/* for expected/unexpected */
#include <linux/spinlock.h>	/* for spinlocks */
#include <linux/kernel.h>	/* for vsnprintf */
#include <linux/sched.h>	/* for wakeup functions */
#include <linux/wait.h>		/* for wait queues */
#include <linux/types.h>	/* for various types */
#include <linux/interrupt.h>	/* for in_interrupt() */
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif
#include <asm/cache.h>		/* for L1_CACHE_BYTES */

#include <sys/cmn_err.h>	/* for CE_ constants */
#include <sys/strlog.h>		/* for SL_ constants */
#include <sys/kmem.h>		/* for kmem_alloc */

#include <sys/stream.h>		/* streams definitions */
#include <sys/strsubr.h>	/* for implementation definitions */
#include <sys/strconf.h>	/* for syscontrols */
#include <sys/ddi.h>

#include "sys/config.h"
#include "src/modules/sth.h"	/* for str_minfo */
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

/**
 *  adjmsg:	- adjust a message
 *  @mp:	message to adjust
 *  @length:	length to trim
 *
 *  Trims -@length bytes from the head of the message or @length bytes from the end of the message.
 *
 *  Return Values: (1) - success; (0) - failure
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

EXPORT_SYMBOL(adjmsg);

/**
 *  allocb:	- allocate a message block
 *  @size:	size of message block in bytes
 *  @priority:	priority of the allocation
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

EXPORT_SYMBOL(allocb);

/**
 *  copyb:	- copy a message block
 *  @bp:	the message block to copy
 *
 *  Return Values: Returns the copy of the message or %NULL on failure.
 *
 *  Notices: Unlike LiS we do not align the copy.  The driver must me wary of alignment.
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

EXPORT_SYMBOL(copyb);

/**
 *  copymsg:	- copy a message
 *  @msg:	message to copy
 *
 *  Copies all the message blocks in message @msg and returns a pointer to the copied message.
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

EXPORT_SYMBOL(copymsg);

/**
 *  datamsg:	- test for data message type
 *  @type:	type to test
 */
__EXTERN_INLINE int datamsg(unsigned char type);
//EXPORT_SYMBOL(datamsg);

/**
 *  dupb:	- duplicates a message block
 *  @bp:	message block to duplicate
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

EXPORT_SYMBOL(dupb);

/**
 *  dupmsg:	- duplicate a message
 *  @msg:	message to duplicate
 *
 *  Duplicates an entire message using dupb() to duplicate each of the message blocks in the
 *  message.  Returns a pointer to the duplicate message.
 */
mblk_t *dupmsg(mblk_t *msg)
{
	mblk_t *mp = NULL;
	register mblk_t *bp, **mpp = &mp;
	for (bp = msg; bp; bp = bp->b_cont, mpp = &(*mpp)->b_cont)
		if (!(*mpp = dupb(bp)))
			goto error;
	return (mp);
      error:
	freemsg(mp);
	return (NULL);
}

EXPORT_SYMBOL(dupmsg);

/**
 *  esballoc:	- allocate a message block with an external buffer
 *  @base:	base address of message buffer
 *  @size:	size of the message buffer
 *  @priority:	priority of message block header allocation
 *  @freeinfo:	free routine callback
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

EXPORT_SYMBOL(esballoc);

/**
 *  freeb:	- free a message block
 *  @mp:	message block to free
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
	/* if the message block refers to the associated data block then we have already freed the mdbblock above when
	   necessary; otherwise the entire mdbblock can go if the datab is also unused */
	if (db != (dp = mb_to_db(mp)) && !db_ref(dp))
		mdbblock_free(mp);
	return;
      bug:
	swerr();
	return;
}

EXPORT_SYMBOL(freeb);

/**
 *  freemsg:	- free a message
 *  @mp:	the message to free
 */
__EXTERN_INLINE void freemsg(mblk_t *mp);
//EXPORT_SYMBOL(freemsg);

/**
 *  isdatablk:	- test data block for data type
 *  @dp:	data block to test
 */
__EXTERN_INLINE int isdatablk(dblk_t * dp);
//EXPORT_SYMBOL_GPL(isdatablk);

/**
 *  isdatamsg:	- test a message block for data type
 *  @mp:	message block to test
 */
__EXTERN_INLINE int isdatamsg(mblk_t *mp);
//EXPORT_SYMBOL_GPL(isdatamsg);

/**
 *  pcmsg:	- data block type for priority
 *  @type:	the type to check
 */
__EXTERN_INLINE int pcmsg(unsigned char type);
//EXPORT_SYMBOL(pcmsg);

/**
 *  linkb:	- link message block onto message
 *  @mp1:	message onto which to link
 *  @mp2:	message block to link
 */
__EXTERN_INLINE void linkb(mblk_t *mp1, mblk_t *mp2);
//EXPORT_SYMBOL(linkb);

/**
 *  linkmsg:	- link messages
 *  @mp1:	message onto which to link
 *  @mp2:	message to link
 */
__EXTERN_INLINE mblk_t *linkmsg(mblk_t *mp1, mblk_t *mp2);
//EXPORT_SYMBOL_GPL(linkmsg);

/**
 *  msgdsize:	- calculate size of data in message
 *  @mp:	message across which to calculate data bytes
 */
__EXTERN_INLINE size_t msgdsize(mblk_t *mp);
//EXPORT_SYMBOL(msgdsize);

/**
 *  msgpullup:	- pull up bytes into a message
 *  @mp:	message to pull up
 *  @length:	number of bytes to pull up
 *
 *  Pulls up @length  bytes into the returned message.  This is for handling headers as a contiguous
 *  range of bytes.
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

EXPORT_SYMBOL(msgpullup);

/**
 *  msgsize:	- calculate size of a message
 *  @mp:	message for which to calculate size
 */
__EXTERN_INLINE size_t msgsize(mblk_t *mp);
//EXPORT_SYMBOL_GPL(msgsize);

/**
 *  pullupmsg:	- pull up bytes into first data block in message
 *  @mp:	message to pull up
 *  @len:	number of bytes to pull up
 *
 *  Pulls up @length  bytes into the initial data block in message @mp.  This is for handling headers
 *  as a contiguous range of bytes.
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
	/* There actually is a way on 2.4 and 2.6 kernels to determine if the memory is suitable for DMA if it was
	   allocated with kmalloc, but that's for later, and only if necessary. If you need ISA DMA memory, please use
	   esballoc. */
	if (!len || ((blen = mp->b_wptr - mp->b_rptr) >= len && len >= 0
		     && !((ulong) (mp->b_rptr) & (L1_CACHE_BYTES - 1))))
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
	if (!(base = kmem_alloc(size, KM_NOSLEEP | KM_CACHEALIGN | KM_DMA)))
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
	mp->b_datap = dp;
	/* remove a reference from old initial datab */
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
	for (mpp = &mp->b_cont; (bp = *mpp);) {
		if ((blen = bp->b_wptr > bp->b_rptr ? bp->b_wptr - bp->b_rptr : 0) > 0 && bp->b_datap->db_type != type)
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

EXPORT_SYMBOL(pullupmsg);

/**
 *  rmvb:   - remove a message block from a message
 *  @mp:    message from which to remove the block
 *  @bp:    the block to remove
 */
__EXTERN_INLINE mblk_t *rmvb(mblk_t *mp, mblk_t *bp);
//EXPORT_SYMBOL(rmvb);

/**
 *  testb:	- test allocate of a message block
 *  @size:	size of buffer for which to test
 *  @priority:	allocation priority to test
 */
__EXTERN_INLINE int testb(size_t size, uint priority);
//EXPORT_SYMBOL(testb);

/**
 *  unlinkb:	- unlink first block of message
 *  @mp:	message to unlink
 */
__EXTERN_INLINE mblk_t *unlinkb(mblk_t *mp);
//EXPORT_SYMBOL(unlinkb);

/**
 *  xmsgsize:	- calculate size in message of same type as first data block
 *
 *  Notices: This is not bug-to-bug compatible with LiS.  Some differences: LiS wraps at a message
 *  size of 65636 and cannot handle message blocks larger than 65636.  LiS will consider a non-zero
 *  initial block (such as that left by adjmsg()) as the first message block type when it should
 *  not.  This implementation does not wrap the size, and skips initial zero-length message blocks.
 *  This implementation of xmsgsize does not span non-zero blocks of different types.
 */
__EXTERN_INLINE size_t xmsgsize(mblk_t *mp);
//EXPORT_SYMBOL_GPL(xmsgsize);

static int __insq(queue_t *q, mblk_t *emp, mblk_t *nmp);
/**
 *  appq:	- append a message onto a queue
 *  @q:		the queue to append to
 *  @emp:	existing message on queue
 *  @nmp:	the message to append
 */
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

EXPORT_SYMBOL_GPL(appq);

/**
 *  backq:	- find the queue upstream from this one
 *  @q:		this queue
 */
__EXTERN_INLINE queue_t *backq(queue_t *q);
//EXPORT_SYMBOL(backq);

/**
 *  qbackenable: - backenable a queue
 *  @q:		the queue to backenable
 */
void qbackenable(queue_t *q)
{
	queue_t *qp;
	ptrace(("%s; back-enabling queue %p\n", __FUNCTION__, q));
	ensure(q, return);
	ptrace(("%s; locking queue %p\n", __FUNCTION__, q));
	hrlock(q);		/* read lock queue chain */
	for (qp = backq(q); qp; qp = backq(qp)) {
		ensure(qp->q_qinfo, continue);
		ptrace(("%s; checking queue %p for service procedure\n", __FUNCTION__, qp));
		if (qp->q_qinfo->qi_srvp) {
			ptrace(("%s; found queue to enable %p\n", __FUNCTION__, qp));
			enableq(qp);	/* normal enable */
			break;
		}
	}
	ptrace(("%s; unlocking queue %p\n", __FUNCTION__, q));
	hrunlock(q);
	return;
}

/*
 *  __bcangetany:
 */
static int __bcangetany(queue_t *q)
{
	struct qband *qb;
	for (qb = q->q_bandp; qb && qb->qb_first; qb = qb->qb_next) ;
	return (qb ? qb->qb_band : 0);
}

/*
 *  __bcanget:
 */
static int __bcanget(queue_t *q, unsigned char band)
{
	struct qband *qb;
	for (qb = q->q_bandp; qb && (qb->qb_band > band); qb = qb->qb_next) ;
	if (qb && qb->qb_band == band && !qb->qb_count)
		qb = NULL;
	return (qb ? band : 0);
}

static int __canget(queue_t *q);
/**
 *  bcanget:	- check whether messages are on a queue
 *  @q:		queue to check
 *  @band:	band to check
 */
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

EXPORT_SYMBOL_GPL(bcanget);

/*
 *  __bcanputany:
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

/*
 *  __bcanput:
 */
static int __bcanput(queue_t *q, unsigned char band)
{
	queue_t *q_next;
	if ((q_next = q)) {
		struct qband *qb;
		unsigned long flags;
		/* find first queue with service procedure or no q_next pointer */
		while ((q = q_next) && !q->q_qinfo->qi_srvp && (q_next = q->q_next)) ;
		qrlock(q, &flags);
		/* bands are sorted in decending priority so that we can quit the search early for higher priority
		   bands */
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
/**
 *  bcanput:	- check whether message can be put to a queue
 *  @q:		queue to check
 *  @band:	band to check
 *
 *  Notices: Don't call these functions with NULL q pointers!  To walk the list of queues we take a
 *  reader lock on the stream head.  We can use simple spins on the queue because we have already
 *  locked out interrupts if outside and interrupt handler.  We only need a read lock because the
 *  bit semantics on the queue band are atomic.
 *
 *  Use bcanput or bcanputnext with band -1 to check for any writable non-zero band.
 */
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

EXPORT_SYMBOL(bcanput);

/**
 *  bcanputnext: - check whether messages can be put to queue after this one
 *  @q:		this queue
 *  @band:	band to check
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

EXPORT_SYMBOL(bcanputnext);

/**
 *  canenable:	- check whether service procedure will run
 *  @q:		queue to check
 */
__EXTERN_INLINE int canenable(queue_t *q);
//EXPORT_SYMBOL(canenable);

/*
 *  __canget:
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

/**
 *  canget:	- check whether messages are on queue
 *  @q:		queue to check
 *
 *  We treat a canget that fails like a getq that returns null visa vi backenable.  That way poll
 *  and strread that use this will properly backenable the queue.
 */
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

EXPORT_SYMBOL_GPL(canget);

/*
 *  __canput:
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

/**
 *  canput:	- check wheter message can be put to a queue
 *  @q:		the queue to check
 *
 *  Don't call these functions with NULL q pointers!  To walk the list of queues we take a reader
 *  lock on the stream head.
 */
int canput(queue_t *q)
{
	int result;
	hrlock(q);		/* read lock queue chain */
	result = __canput(q);
	hrunlock(q);
	return (result);
}

EXPORT_SYMBOL(canput);

/**
 *  canputnext: - check whether messages can be put to the queue after this one
 *  @q:		this queue
 */
int canputnext(queue_t *q)
{
	int result;
	hrlock(q);		/* read lock queue chain */
	result = __canput(q->q_next);
	hrunlock(q);
	return (result);
}

EXPORT_SYMBOL(canputnext);

/*
 *  __find_qband:
 *
 * Find a queue band.  This must be called with the queue read or write locked.
 */
static inline struct qband *__find_qband(queue_t *q, unsigned char band)
{
	struct qband *qb;
	for (qb = q->q_bandp; qb && qb->qb_band > band; qb = qb->qb_next) ;
	if (qb && qb->qb_band == band)
		return (qb);
	return (NULL);
}

static int __rmvq(queue_t *q, mblk_t *mp);
/*
 *  __flushband:
 */
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
				/* This is faster.  For flushall, we link the qband chain onto the free list and null
				   out qband counts and markers. */
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

/**
 *  flushband:	- flush message from a queue band
 *  @q:		the queue to flush
 *  @band:	the band to flush
 *  @flag:	how to flush, %FLUSHALL or %FLUSHDATA
 */
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
	/* we want to free messages with the locks off so that other CPUs can process this queue and we don't block
	   interrupts too long */
	mb();
	freechain(mp, mpp);
}

EXPORT_SYMBOL(flushband);

/**
 *  freezestr:	- freeze a stream for direct queue access
 *  @q:		queue to freeze
 */
unsigned long freezestr(queue_t *q)
{
	unsigned long flags = 0;
	hwlock(q, &flags);
	qwlock(q, NULL);
	return ((q->q_iflags = flags));
}

EXPORT_SYMBOL(freezestr);

/**
 *  getadmin: - get the administrative function associated with a module identifier
 *  @modid: the module identifier
 *
 *  Obtains the qi_qadmin function pointer for the module identifier by the module identifier
 *  @modid.
 *
 *  Return Value: Returns a function pointer to the qi_qadmin() procedure for the module, which may be
 *  %NULL, or returns %NULL on failure.
 *
 *  Context: Can be called from any context.  When called from a blocking context, the function has
 *  the side-effect that the identified module may be loaded by module identifier.  The kernel
 *  module demand loaded will have the module name or alias "streams-modid-%u".
 */
qi_qadmin_t getadmin(modID_t modid)
{
	qi_qadmin_t qadmin = NULL;
	struct fmodsw *fmod;
	if ((fmod = fmod_get(modid))) {
		struct streamtab *st;
		struct qinit *qi;
		printd(("%s: %s: got module\n", __FUNCTION__, fmod->f_name));
		if ((st = fmod->f_str) && (qi = st->st_rdinit))
			qadmin = qi->qi_qadmin;
		printd(("%s: %s: putting module\n", __FUNCTION__, fmod->f_name));
		fmod_put(fmod);
	}
	return (qadmin);
}

EXPORT_SYMBOL(getadmin);

/**
 *  getmid: - get the module identifier associated with a module name
 *  @name: the name of the module
 *
 *  Obtains the module id of the named module.
 *
 *  Return Value: Returns the module identifier associated with the named module or zero (0) if no
 *  module of the specified name can be found on the system.
 *
 *  Context: Can be called from any context.  When called from a blocking context, the function has
 *  the side-effect that the named module may be loaded by module name.  The kernel module demand
 *  loaded will have the module name or alias "streams-%s".
 */
modID_t getmid(const char *name)
{
	struct fmodsw *fmod;
	struct cdevsw *cdev;
	if ((fmod = fmod_find(name))) {
		modID_t modid = fmod->f_modid;
		printd(("%s: %s: found module\n", __FUNCTION__, fmod->f_name));
		printd(("%s: %s: putting module\n", __FUNCTION__, fmod->f_name));
		fmod_put(fmod);
		return (modid);
	}
	if ((cdev = cdev_find(name))) {
		modID_t modid = cdev->d_modid;
		printd(("%s: %s: found device\n", __FUNCTION__, cdev->d_name));
		printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
		sdev_put(cdev);
		return (modid);
	}
	return (0);
}

EXPORT_SYMBOL(getmid);

/*
 *  __getq:
 *
 *  Faster than __rmvq, same idea
 */
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

/**
 *  getq:	- get messags from a queue
 *  @q:		the queue from which to get messages
 */
mblk_t *getq(queue_t *q)
{
	mblk_t *mp;
	unsigned long flags;
	int backenable = 0;
	ensure(q, return (NULL));
	qwlock(q, &flags);
	mp = __getq(q, &backenable);
	qwunlock(q, &flags);
	if (backenable)
		qbackenable(q);
	return (mp);
}

EXPORT_SYMBOL(getq);

/*
 *  __get_qband:
 *
 *  Find or create a queue band.  This must be called with the queue write locked.
 */
static struct qband *__get_qband(queue_t *q, unsigned char band)
{
	struct qband *qb, *qp, **qbp;
	/* find insertion point for band */
	for (qp = NULL, qbp = &q->q_bandp; *qbp && (*qbp)->qb_band > band; qp = *qbp, qbp = &(*qbp)->qb_next) ;
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
/*
 *  __insq:
 */
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
	if (nmp->b_datap->db_type >= QPCTL) {
		if (emp->b_prev && emp->b_prev->b_datap->db_type < QPCTL)
			goto out_of_order;
		enable = 2;	/* always enable on high priority */
		nmp->b_band = 0;
		if (!q->q_pctl)
			q->q_pctl = nmp;
	} else {
		if (emp->b_datap->db_type >= QPCTL || emp->b_band < nmp->b_band)
			goto out_of_order;
		if (emp->b_prev && emp->b_prev->b_datap->db_type < QPCTL && emp->b_prev->b_band > nmp->b_band)
			goto out_of_order;
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
	size = msgsize(nmp);
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
	goto failure;
      out_of_order:
	/* insertion would misorder the queue */
	goto failure;
      failure:
	return (0);		/* failure */
      putq:
	/* insert at end */
	return __putq(q, nmp);
}

/**
 *  insq:	- insert a message before another on a queue
 *  @q:		the queue into which to insert
 *  @emp:	the existing message before which to insert
 *  @nmp:	the new message to insert
 */
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

EXPORT_SYMBOL(insq);

/**
 *  OTHERQ:	- find the other queue in a queue pair
 *  @q:		one queue
 */
__EXTERN_INLINE queue_t *OTHERQ(queue_t *q);
//EXPORT_SYMBOL(OTHERQ);

/*
 *  __put:
 */
static void __put(queue_t *q, mblk_t *mp)
{
	struct queinfo *qu = (typeof(qu)) RD(q);
	q->q_qinfo->qi_putp(q, mp);
	if (unlikely(waitqueue_active(&qu->qu_qwait)))
		wake_up_all(&qu->qu_qwait);
}

/*
 *  _put:
 */
static void _put(queue_t *q, mblk_t *mp)
{
	struct syncq *isq;
	unsigned long flags;
#if defined CONFIG_STREAMS_COMPAT_AIX || defined CONFIG_STREAMS_COMPAT_AIX_MODULE
	while (q->q_ftmsg && !(*q->q_ftmsg) (mp) && (q = q->q_next)) ;
#endif
	if (!(isq = q->q_syncq))
		__put(q, mp);
	else {
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

/**
 *  put:	- call a queue's qi_putq() procedure
 *  @q:		the queue's procedure to call
 *  @mp:	the message to place on the queue
 *
 *  Your put routines must be MT-safe.  That simple.  Don't put to put routines that don't exist.
 */
void put(queue_t *q, mblk_t *mp)
{
	hrlock(q);
	_put(q, mp);
	hrunlock(q);
}

EXPORT_SYMBOL(put);

/**
 *  putnext:	- put a message on the queue next to this one
 *  @q:		this queue
 *  @mp:	message to put
 */
void putnext(queue_t *q, mblk_t *mp)
{
	hrlock(q);
	_put(q->q_next, mp);
	hrunlock(q);
}

EXPORT_SYMBOL(putnext);

/*
 *  __putbq:
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

/**
 *  putbq:	- put a message back on a queue
 *  @q:		queue to place back message
 *  @mp:	message to place back
 */
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
		/* This should never happen, because it takes a qband structure allocation failure to get here, and
		   since we are putting the message back on the queue, there should already be a qband structure.
		   Unless, however, putbq() is just used to insert messages ahead of others rather than really putting
		   them back. Nevertheless, a way to avoid this error is to always ensure that a qband structure exists 
		   (e.g., with strqset) before calling putbq on a band for the first time. */
		return (0);
	}
}

EXPORT_SYMBOL(putbq);

/**
 *  putctl1:	- put a 1-byte control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 *  @param:	the 1 byte parameter
 */
__EXTERN_INLINE int putctl1(queue_t *q, int type, int param);
//EXPORT_SYMBOL(putctl1);

/**
 *  putctl2:	- put a 2-byte control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 *  @param1:	the first 1 byte parameter
 *  @param2:	the second 1 byte parameter
 */
__EXTERN_INLINE int putctl2(queue_t *q, int type, int param1, int param2);
//EXPORT_SYMBOL(putctl2);

/**
 *  putctl:	- put a control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 */
__EXTERN_INLINE int putctl(queue_t *q, int type);
//EXPORT_SYMBOL(putctl);

/**
 *  putnextctl:	- put a control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 */
__EXTERN_INLINE int putnextctl(queue_t *q, int type);
//EXPORT_SYMBOL(putnextctl);

/**
 *  putnextctl1: - put a 1-byte control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 *  @param:	the 1 byte parameter
 */
__EXTERN_INLINE int putnextctl1(queue_t *q, int type, int param);
//EXPORT_SYMBOL(putnextctl1);

/**
 *  putnextctl2: - put a 2-byte control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 *  @param1:	the first 1 byte parameter
 *  @param2:	the second 1 byte parameter
 */
__EXTERN_INLINE int putnextctl2(queue_t *q, int type, int param1, int param2);
//EXPORT_SYMBOL(putnextctl2);

/*
 *  __putq:	- put a message block to a queue
 *  @q:		queue to which to put the message
 *  @mp:	message to put
 *
 *  Optomized for arriving at an empty queue.  This is because in a smoothly running system queues
 *  should be empty.  This is a little better than LiS in that it does not examine the band of
 *  messages on the queue.
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

/**
 *  putq:	- put a message block to a queue
 *  @q:		queue to which to put the message
 *  @mp:	message to put
 */
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
		/* This can happen and it is bad.  We use the return value to putq but it is typically ignored by the
		   module.  One way to ensure that this never happens is to call strqset() for the band before calling
		   putq on the band for the first time. (See also putbq()) */
		return (0);
	}
}

EXPORT_SYMBOL(putq);

int setsq(queue_t *q, struct fmodsw *fmod, int mux);
/**
 *  qattach: - attach a stream head, module or driver queue pair to a stream head
 *  @sd:	stream head data structure identifying stream
 *  @fmod:	&struct fmodsw pointer identifying module or driver
 *  @devp:	&dev_t pointer providing opening device number
 *  @oflag:	open flags
 *  @sflag:	streams flag, can be %DRVOPEN, %CLONEOPEN, %MODOPEN
 *  @crp:	&cred_t pointer to credentials of opening task
 */
int qattach(struct stdata *sd, struct fmodsw *fmod, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct streamtab *st;
	queue_t *q;
	dev_t odev;
	struct cdevsw *cdev;
	int err;
	err = -ENOMEM;
	ptrace(("%s: attaching module %s\n", __FUNCTION__, fmod->f_name));
	if (!(q = allocq())) {
		printd(("%s: could not alloca queue pair\n", __FUNCTION__));
		goto error;
	}
	if ((err = setsq(q, fmod, 0)) < 0) {
		printd(("%s: could not set queues\n", __FUNCTION__));
		goto freeq_error;
	}
	qinsert(sd->sd_rq, q);	/* half insert under stream head */
	qprocsoff(q);		/* does not alter q_next pointers, just flags */
	odev = *devp;		/* remember calling device number */
	if ((err = qopen(q, devp, oflag, sflag, crp))) {
		if (err > 0)
			err = -err;
		printd(("%s: error from qopen, errno %d\n", __FUNCTION__, -err));
		goto qerror;
	}
	if (sflag != MODOPEN) {
		/* magic garden */
		/* this just doesn't work with locking.... FIXME */
		if (getmajor(odev) != getmajor(*devp)) {
			printd(("%s: returned major %hu is not called major %hu\n", __FUNCTION__,
				getmajor(*devp), getmajor(odev)));
			err = -ENOENT;
			if (!(cdev = cdrv_get(getmajor(*devp)))) {
				printd(("%s: could not find new major %hu\n", __FUNCTION__, getmajor(*devp)));
				goto enoent;
			}
			printd(("%s: %s: got driver\n", __FUNCTION__, cdev->d_name));
			err = -ENOENT;
			if (!(st = cdev->d_str)) {
				pswerr(("%s: device has no streamtab, should not happen\n", __FUNCTION__));
				goto put_noent;
			}
			if ((err = setsq(q, (struct fmodsw *) cdev, 0)) < 0) {
				printd(("%s: could not set queues to new streamtab\n", __FUNCTION__));
				goto put_noent;
			}
			printd(("%s: %s: putting driver\n", __FUNCTION__, cdev->d_name));
			cdrv_put(cdev);
		}
	} else if (odev != *devp) {
		/* module is supposed to ignore devp */
		*devp = odev;
		pswerr(("%s: module altered device number\n", __FUNCTION__));
	}
	qprocson(q);		/* in case qopen() forgot */
	return (0);
      put_noent:
	printd(("%s: %s: putting driver\n", __FUNCTION__, cdev->d_name));
	cdrv_put(cdev);
      enoent:
	qdetach(q, oflag, crp);	/* need to call close */
	goto error;
      qerror:
	*devp = odev;
	qprocsoff(q);		/* doesn't alter anything if procs still turned off */
	qdelete(q);		/* remove half insert */
      freeq_error:
	qput(&q);
      error:
	return (err);
}

EXPORT_SYMBOL_GPL(qattach);

/**
 *  qclose:	- invoke a queue pair's qi_qclose entry point
 *  @q:		read queue of the queue pair to close
 *  @oflag:	open flags
 *  @crp:	credentials of closing task
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

EXPORT_SYMBOL_GPL(qclose);

/**
 *  qdelete:	- delete a queue pair from a stream
 *  @rq:	read queue of queue pair to delete
 *
 *  qdelete() half-deletes the queue pair identified by @rq from the stream to which it belongs.
 *  The q->q_next pointers of the queue pair to be deleted, @rq, are adjusted, but the stream
 *  remains unaffected.  qprocsoff() must be called before calling qdelete() to properly remove the
 *  queue pair from the stream.
 *
 *  Context: qdelete() should only be called from the qattach() or qdetach() procedure or a stream
 *  head open or close procedure.
 *
 *  Notices: rq should have already been removed from a queue with qprocsoff() and must be a valid
 *  pointer or bad things will happen.
 */
void qdelete(queue_t *q)
{
	unsigned long flags = 0;
	struct queinfo *qu = (typeof(qu)) q;
	struct stdata *sd = qu->qu_str;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);
	ptrace(("%s: deleting queue from stream\n", __FUNCTION__));
	wq = rq + 1;
	swlock(sd, &flags);
	qput(&rq->q_next);
	qput(&wq->q_next);
	qu->qu_str = NULL;
	swunlock(sd, &flags);
	qput(&q);
	sd_put(sd);
}

/**
 *  qdetach:	- detach a queue pair from a stream
 *  @q:		read queue pointer of queue pair to detach
 *  @flags:	open flags of closing task
 *  @crp:	credentials of closing task
 *
 *  qdetach() calls the module queue pair qi_qclose procedure and then removes the queue pair from
 *  the stream.
 *
 *  Return: qdetach() returns any error returned by the module's qi_qclose procedure.
 *
 *  Errors: qdetach() can return any error returned by the module's qi_qclose procedure.  This error
 *  is not returned to the user.
 *
 *  Context: qdetach() is meant to be called in user context.
 */
int qdetach(queue_t *q, int flags, cred_t *crp)
{
	int err;
	ptrace(("%s: detaching queue\n", __FUNCTION__));
	err = qclose(q, flags, crp);
	if (err)
		printd(("%s: error on qclose, errno %d\n", __FUNCTION__, -err));
	qprocsoff(q);		/* in case qi_qclose procedure forgot */
	qdelete(q);
	return (err);
}

EXPORT_SYMBOL_GPL(qdetach);

/**
 *  qinsert:	- insert a queue pair below another in a stream
 *  @brq:	read queue of queue pair beneath which to insert
 *  @irq:	read queue of queue pair to insert
 *
 *  qinsert() half-inserts the queue pair identified by @irq beneath the queue pair on the stream
 *  identified by @brq.  This is only a half-insert.  The q->q_next pointers of the queue pair to be
 *  inserted, @irq, are adjusted, but the stream remains unaffected.  qprocson() must be called on
 *  @irq to complete the insertion and properly set flags.
 *
 *  Context: qinsert() is only meant to be called from the qattach() procedure or a stream head open
 *  procedure.
 *
 *  Notices: irq should not already be inserted on a queue or bad things will happen.
 */
void qinsert(queue_t *brq, queue_t *irq)
{
	struct queinfo *iqu = (typeof(iqu)) irq;
	queue_t *iwq = irq + 1;
	struct queinfo *bqu = (typeof(bqu)) brq;
	queue_t *bwq = brq + 1;
	unsigned long flags = 0;
	ptrace(("%s: half insert of queue pair under stream head\n", __FUNCTION__));
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
 *  qopen:	- call a module's qi_qopen entry point
 *  @q:		the read queue of the module queue pair to open
 *  @devp:	pointer to the opening and returned device number
 *  @oflag:	open flags
 *  @sflag:	stream flag, can be %DRVOPEN, %MODOPEN, %CLONEOPEN
 *  @crp:	pointer to the opening task's credential structure
 */
int qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int (*q_open) (queue_t *, dev_t *, int, int, cred_t *);
	if ((q_open = q->q_qinfo->qi_qopen))
		return q_open(q, devp, oflag, sflag, crp);
	return (-ENOPKG);
}

EXPORT_SYMBOL_GPL(qopen);

/**
 *  qprocsoff:	- turn off qi_putp and qi_srvp procedures for a queue pair
 *  @q:		read queue pointer for the queue pair to turn procs off
 *
 *  qprocsoff() marks the queue pair as being owned by the stream head (disabling further put
 *  procedures), marks it as being noenabled, (which disables further srv procedures), and bypasses
 *  the module by adjusting the q->q_next pointers of upstream modules for each queue in the queue
 *  pair.  This effectively bypasses the module.
 *
 *  Context: qprocsoff() should only be called from qattach() or a stream head head open procedure.
 *  The user should call qprocsoff() from the qclose() procedure before returning.
 *
 *  Notices: qprocsoff() does not fully delete the queue pair from the stream.  It is still
 *  half-attached.  Use qdelete() to complete the final removal of the queue pair from the stream.
 */
void qprocsoff(queue_t *q)
{
	unsigned long flags = 0;
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);
	ptrace(("%s: turning off procs\n", __FUNCTION__));
	assert(current_context() <= CTX_STREAMS);
	if (!test_and_set_bit(QHLIST_BIT, &rq->q_flag)) {
		set_bit(QHLIST_BIT, &wq->q_flag);
		set_bit(QNOENB_BIT, &rq->q_flag);	/* XXX */
		set_bit(QNOENB_BIT, &wq->q_flag);	/* XXX */
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
	} else
		printd(("%s: procs already turned off\n", __FUNCTION__));
}

EXPORT_SYMBOL(qprocsoff);

/**
 *  qprocson:	- trun on qi_putp and qi_srvp procedure for a queeu pair
 *  @q:		read queue pointer for the queue pair to turn procs on
 *
 *  qprocson() marks the queue pair as being not owned by the stream head (enabling put procedures),
 *  marks it as being enabled (enabling srv procedures), and intalls the module by adjusting the
 *  q->q_next pointers of the upstream modules for each queue in the queue pair.  This effectively
 *  undoes the bypass created by qprocsoff().
 *
 *  Context: qprocson() should only be called from qattach(), qdetach() or a stream head close
 *  procedure.  The user should call qprocson() from the qopen() procedure before returning.
 *
 *  Notices: qprocson() does fully inserts the queue pair into the stream.  It must be half-inserted
 *  with qinsert() before qprocson() can be called.
 */
void qprocson(queue_t *q)
{
	unsigned long flags = 0;
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);
	ptrace(("%s: turning on procs\n", __FUNCTION__));
	assert(current_context() <= CTX_STREAMS);
	if (test_and_clear_bit(QHLIST_BIT, &rq->q_flag)) {
		clear_bit(QHLIST_BIT, &wq->q_flag);
		clear_bit(QNOENB_BIT, &rq->q_flag);	/* XXX */
		clear_bit(QNOENB_BIT, &wq->q_flag);	/* XXX */
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
	} else
		printd(("%s: procs already turned on\n", __FUNCTION__));
}

EXPORT_SYMBOL(qprocson);

/**
 *  qpop:	- pop a moudle from a stream
 *  @sd:	stream head
 *  @oflag:	open flags to call module open procedure
 *  @crp:	credential pointer for module open procedure
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

EXPORT_SYMBOL_GPL(qpop);

/**
 *  qpush:	- push a module onto a stream
 *  @sd:	stream head
 *  @name:	name of the module to push
 *  @devp:	device number pointer
 *  @oflag:	open flags to call module open procedure
 *  @crp:	credential pointer for module open procedure
 */
int qpush(struct stdata *sd, const char *name, dev_t *devp, int oflag, cred_t *crp)
{
	int err;
	struct fmodsw *fmod;
	if (sd->sd_pushcnt >= sysctl_str_nstrpush)
		goto enosr;
	if (!(fmod = fmod_find(name)))
		goto einval;
	printd(("%s: %s: found module\n", __FUNCTION__, fmod->f_name));
	if ((err = qattach(sd, fmod, devp, oflag, MODOPEN, crp)) != 0)
		goto error;
	sd->sd_pushcnt++;
      error:
	printd(("%s: %s: putting module\n", __FUNCTION__, fmod->f_name));
	fmod_put(fmod);
	return (err);
      einval:
	return (-EINVAL);
      enosr:
	return (-ENOSR);
}

EXPORT_SYMBOL_GPL(qpush);

/**
 *  qreply:	- reply with a message
 *  @q:		queue from which to reply
 *  @mp:	message reply
 */
__EXTERN_INLINE void qreply(queue_t *q, mblk_t *mp);
//EXPORT_SYMBOL(qreply);

/**
 *  qsize:	- calculate number of messages on a queue
 *  @q:		queue to count messages
 */
__EXTERN_INLINE ssize_t qsize(queue_t *q);
//EXPORT_SYMBOL(qsize);

/**
 *  qcountstrm:	- count the numer of messages along a stream
 *  @q:		queue to begin with
 *
 *  Notices: qcountstrm() is only for LiS compatibility.
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

EXPORT_SYMBOL_GPL(qcountstrm);

/**
 *  RD:		- find read queue from write queu
 *  @q:		write queue pointer
 */
__EXTERN_INLINE queue_t *RD(queue_t *q);
//EXPORT_SYMBOL(RD);

/*
 *  __rmvq:
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

/**
 *  rmvq:	- remove a messge from a queue
 *  @q:		queue from which to remove message
 *  @mp:	message to remove
 */
void rmvq(queue_t *q, mblk_t *mp)
{
	unsigned long flags;
	int backenable;
	q = mp->b_queue;
	ensure(q, return);
	/* We ignore the queue pointer provided by the user because we know which queue the message belongs to (if
	   any). */
	qwlock(q, &flags);
	backenable = __rmvq(q, mp);
	qwunlock(q, &flags);
	if (backenable)
		qbackenable(q);
}

EXPORT_SYMBOL(rmvq);

/**
 *  SAMESTR:	- check whether this and next queue have the same stream head
 *  @q:		this queue
 */
__EXTERN_INLINE int SAMESTR(queue_t *q);
//EXPORT_SYMBOL(SAMESTR);

/*
 *  __setq:
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

/**
 *  setq:	- set queue characteristics
 *  @q:		read queue in queue pair to set
 *  @rinit:	read queue init structure
 *  @winit:	write queue initi structure
 */
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

EXPORT_SYMBOL_GPL(setq);

struct syncq syncq_global;

/**
 *  setsq:	- set synchornization charateristics on a queue pair
 *  @q:		read queue in queue pair to set
 *  @fmod:	module to which queue pair belongs
 *  @mux:	non-zero for lower multiplexing driver queue pair
 *
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
		if (!(sqr = sq_alloc()))
			goto enomem;
		if (!(sqw = sq_alloc())) {
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
		if (!(sqr = sq_alloc()))
			goto enomem;
		sqw = sq_get(sqr);
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
		(q + 0)->q_syncq = sq_get(fmod->f_syncq);
		(q + 1)->q_syncq = sq_get(fmod->f_syncq);
		break;
	case SQLVL_ELSEWHERE:	/* not fully supported */
		/* find the elsewhere syncq and use it */
		break;
	case SQLVL_GLOBAL:	/* for testing */
		/* use the gloable syncq */
		(q + 0)->q_syncq = sq_get(&syncq_global);
		(q + 1)->q_syncq = sq_get(&syncq_global);
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

/**
 *  strqget:	- get characteristics of a queue
 *  @q:		queue to query
 *  @what:	what characteristic to get
 *  @band:	from which queue band
 *  @val:	location of return value
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
			if (!(qb = __get_qband(q, band)))
				goto enomem;
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
				err = -EINVAL;
				break;
			}
			break;
		      enomem:
			err = -ENOMEM;
			break;
		} while (0);
		qrunlock(q, &flags);
	}
	return (-err);
}

EXPORT_SYMBOL(strqget);

/**
 *  strqset:	- set characteristics of a queue
 *  @q:		queue to set
 *  @what:	what characteristic to set
 *  @band:	to which queue band
 *  @val:	value to set
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

EXPORT_SYMBOL(strqset);

static spinlock_t str_err_lock = SPIN_LOCK_UNLOCKED;
/**
 *  strlog:	- log a STREAMS message
 *  @mid:	module id
 *  @sid:	stream id
 *  @level:	severity level
 *  @flag:	flags controlling distribution
 *  @fmt:	printf(3) format
 *  @...:	format specific arguments
 */
int strlog(short mid, short sid, char level, unsigned short flag, char *fmt, ...)
{
	unsigned long flags;
	static char str_err_buf[1024];
	va_list args;
	va_start(args, fmt);
	spin_lock_irqsave(&str_err_lock, flags);
	vsnprintf(str_err_buf, sizeof(str_err_buf), fmt, args);
	if (flag & SL_FATAL) {
		printk(KERN_CRIT "strlog(%d)[%d,%d]: %s\n", (int) level, (int) mid, (int) sid, str_err_buf);
	} else if (flag & SL_ERROR) {
		printk(KERN_ERR "strlog(%d)[%d,%d]: %s\n", (int) level, (int) mid, (int) sid, str_err_buf);
	} else if (flag & SL_WARN) {
		printk(KERN_WARNING "strlog(%d)[%d,%d]: %s\n", (int) level, (int) mid, (int) sid, str_err_buf);
	} else if (flag & SL_NOTE) {
		printk(KERN_NOTICE "strlog(%d)[%d,%d]: %s\n", (int) level, (int) mid, (int) sid, str_err_buf);
	} else if (flag & SL_CONSOLE) {
		printk(KERN_INFO "strlog(%d)[%d,%d]: %s\n", (int) level, (int) mid, (int) sid, str_err_buf);
	} else {		/* SL_TRACE */
		printk(KERN_DEBUG "strlog(%d)[%d,%d]: %s\n", (int) level, (int) mid, (int) sid, str_err_buf);
	}
	spin_unlock_irqrestore(&str_err_lock, flags);
	va_end(args);
	return (1);
}

EXPORT_SYMBOL(strlog);

/**
 *  unfreezestr:	- thaw a stream frozen with freezestr()
 *  @q:			the queue in the stream to thaw
 *  @flags:		spl flags
 */
void unfreezestr(queue_t *q, unsigned long flags)
{
	flags = q->q_iflags;
	qwunlock(q, NULL);
	hwunlock(q, &flags);
}

EXPORT_SYMBOL(unfreezestr);

/**
 *  WR:		- get write queue in queue pair
 *  @q:		read queue pointer
 */
__EXTERN_INLINE queue_t *WR(queue_t *q);
//EXPORT_SYMBOL(WR);

static spinlock_t cmn_err_lock = SPIN_LOCK_UNLOCKED;
/*
 *  vcmn_err:
 */
void vcmn_err(int err_lvl, const char *fmt, va_list args)
{
	unsigned long flags;
	static char cmn_err_buf[1024], *cmn_err_ptr = cmn_err_buf;
	spin_lock_irqsave(&cmn_err_lock, flags);
	vsnprintf(cmn_err_buf, sizeof(cmn_err_buf), fmt, args);
	if (cmn_err_buf[0] == '^' || cmn_err_buf[0] == '!')
		cmn_err_ptr++;
	switch (err_lvl) {
	case CE_CONT:
		printk("%s", cmn_err_ptr);
		break;
	default:
	case CE_NOTE:
		/* gets default log level */
		printk(KERN_NOTICE "%s\n", cmn_err_ptr);
		break;
	case CE_WARN:
		printk(KERN_WARNING "%s\n", cmn_err_ptr);
		break;
	case CE_PANIC:
		spin_unlock_irqrestore(&cmn_err_lock, flags);
		panic("%s\n", cmn_err_ptr);
		return;
	case CE_DEBUG:		/* IRIX 6.5 */
		printk(KERN_DEBUG "%s\n", cmn_err_ptr);
		break;
	case CE_ALERT:		/* IRIX 6.5 */
		printk(KERN_ALERT "%s \n", cmn_err_ptr);
		break;
	}
	spin_unlock_irqrestore(&cmn_err_lock, flags);
	return;
}

#if defined CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SYMBOL_GPL(vcmn_err);
#endif

/**
 *  cmn_err:	- print a command error
 *  @err_lvl:	severity
 *  @fmt:	printf(3) format
 *  @...:	format arguments
 */
void cmn_err(int err_lvl, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vcmn_err(err_lvl, fmt, args);
	va_end(args);
	return;
}

EXPORT_SYMBOL(cmn_err);

__EXTERN_INLINE int copyin(const void *from, void *to, size_t len);
//EXPORT_SYMBOL(copyin);

__EXTERN_INLINE int copyout(const void *from, void *to, size_t len);
//EXPORT_SYMBOL(copyout);

__EXTERN_INLINE void delay(unsigned long ticks);
//EXPORT_SYMBOL(delay);

__EXTERN_INLINE int drv_getparm(const unsigned int parm, void *value_p);
//EXPORT_SYMBOL(drv_getparm);

__EXTERN_INLINE unsigned long drv_hztomsec(unsigned long hz);
//EXPORT_SYMBOL(drv_hztomsec);

__EXTERN_INLINE unsigned long drv_hztousec(unsigned long hz);
//EXPORT_SYMBOL(drv_hztousec);

__EXTERN_INLINE unsigned long drv_msectohz(unsigned long msec);
//EXPORT_SYMBOL(drv_msectohz);

__EXTERN_INLINE int drv_priv(cred_t *crp);
//EXPORT_SYMBOL(drv_priv);

__EXTERN_INLINE unsigned long drv_usectohz(unsigned long usec);
//EXPORT_SYMBOL(drv_usectohz);

__EXTERN_INLINE void drv_usecwait(unsigned long usec);
//EXPORT_SYMBOL(drv_usecwait);

__EXTERN_INLINE major_t getmajor(dev_t dev);
//EXPORT_SYMBOL(getmajor);

__EXTERN_INLINE minor_t getminor(dev_t dev);
//EXPORT_SYMBOL(getminor);

__EXTERN_INLINE dev_t makedevice(major_t major, minor_t minor);
//EXPORT_SYMBOL(makedevice);
