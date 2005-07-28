/*****************************************************************************

 @(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.57 $) $Date: 2005/07/28 14:45:45 $

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

 Last Modified $Date: 2005/07/28 14:45:45 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.57 $) $Date: 2005/07/28 14:45:45 $"

static char const ident[] =
    "$RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.57 $) $Date: 2005/07/28 14:45:45 $";

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
#include "src/kernel/strsched.h"		/* for current_context() */
#include "src/kernel/strutil.h"		/* for q locking and puts and gets */

static inline int
db_ref(dblk_t * db)
{
	return (db->db_ref = atomic_read(&db->db_users));
}
static inline void
db_set(dblk_t * db, int val)
{
	atomic_set(&db->db_users, val);
	db_ref(db);
}
static inline void
db_inc(dblk_t * db)
{
	atomic_inc(&db->db_users);
	db_ref(db);
}
static inline void
db_dec(dblk_t * db)
{
	atomic_dec(&db->db_users);
	db_ref(db);
}
static inline int
db_dec_and_test(dblk_t * db)
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

static inline mblk_t *
db_to_mb(dblk_t * db)
{
	return ((mblk_t *) ((unsigned char *) db - datablk_offset + msgblk_offset));
}
static inline unsigned char *
db_to_buf(dblk_t * db)
{
	return ((unsigned char *) db - datablk_offset + databuf_offset);
}
static inline unsigned char *
mb_to_buf(mblk_t *mb)
{
	return ((unsigned char *) mb - msgblk_offset + databuf_offset);
}
static inline dblk_t *
mb_to_db(mblk_t *mb)
{
	return ((dblk_t *) ((unsigned char *) mb - msgblk_offset + datablk_offset));
}
static inline struct mdbblock *
mb_to_mdb(mblk_t *mb)
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
int
adjmsg(mblk_t *mp, ssize_t length)
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
mblk_t *
allocb(size_t size, uint priority)
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
mblk_t *
copyb(mblk_t *bp)
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
mblk_t *
copymsg(mblk_t *msg)
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

EXPORT_SYMBOL(datamsg);

/**
 *  dupb:	- duplicates a message block
 *  @bp:	message block to duplicate
 */
mblk_t *
dupb(mblk_t *bp)
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
mblk_t *
dupmsg(mblk_t *msg)
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
mblk_t *
esballoc(unsigned char *base, size_t size, uint priority, frtn_t *freeinfo)
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
void
freeb(mblk_t *mp)
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

EXPORT_SYMBOL(freeb);

/**
 *  freemsg:	- free a message
 *  @mp:	the message to free
 */
__EXTERN_INLINE void freemsg(mblk_t *mp);

EXPORT_SYMBOL(freemsg);

/**
 *  isdatablk:	- test data block for data type
 *  @dp:	data block to test
 */
__EXTERN_INLINE int isdatablk(dblk_t * dp);

EXPORT_SYMBOL(isdatablk);

/**
 *  isdatamsg:	- test a message block for data type
 *  @mp:	message block to test
 */
__EXTERN_INLINE int isdatamsg(mblk_t *mp);

EXPORT_SYMBOL(isdatamsg);

/**
 *  pcmsg:	- data block type for priority
 *  @type:	the type to check
 */
__EXTERN_INLINE int pcmsg(unsigned char type);

EXPORT_SYMBOL(pcmsg);

/**
 *  linkb:	- link message block onto message
 *  @mp1:	message onto which to link
 *  @mp2:	message block to link
 */
__EXTERN_INLINE void linkb(mblk_t *mp1, mblk_t *mp2);

EXPORT_SYMBOL(linkb);

/**
 *  linkmsg:	- link messages
 *  @mp1:	message onto which to link
 *  @mp2:	message to link
 */
__EXTERN_INLINE mblk_t *linkmsg(mblk_t *mp1, mblk_t *mp2);

EXPORT_SYMBOL(linkmsg);

/**
 *  msgdsize:	- calculate size of data in message
 *  @mp:	message across which to calculate data bytes
 */
__EXTERN_INLINE size_t msgdsize(mblk_t *mp);

EXPORT_SYMBOL(msgdsize);

/**
 *  msgpullup:	- pull up bytes into a message
 *  @mp:	message to pull up
 *  @length:	number of bytes to pull up
 *
 *  Pulls up @length  bytes into the returned message.  This is for handling headers as a contiguous
 *  range of bytes.
 */
mblk_t *
msgpullup(mblk_t *msg, ssize_t length)
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

EXPORT_SYMBOL(msgsize);

/**
 *  pullupmsg:	- pull up bytes into first data block in message
 *  @mp:	message to pull up
 *  @len:	number of bytes to pull up
 *
 *  Pulls up @length  bytes into the initial data block in message @mp.  This is for handling headers
 *  as a contiguous range of bytes.
 */
int
pullupmsg(mblk_t *mp, ssize_t len)
{
	dblk_t *db, *dp;
	ssize_t size, blen, type;
	mblk_t *bp, **mpp;
	unsigned char *base;
	struct mdbblock *md;

	if (!mp || len < -1)
		goto error;
	/* There actually is a way on 2.4 and 2.6 kernels to determine if the memory is suitable
	   for DMA if it was allocated with kmalloc, but that's for later, and only if necessary.
	   If you need ISA DMA memory, please use esballoc. */
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
		if ((blen = bp->b_wptr > bp->b_rptr ? bp->b_wptr - bp->b_rptr : 0) > 0
		    && bp->b_datap->db_type != type)
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

EXPORT_SYMBOL(rmvb);

/**
 *  testb:	- test allocate of a message block
 *  @size:	size of buffer for which to test
 *  @priority:	allocation priority to test
 */
__EXTERN_INLINE int testb(size_t size, uint priority);

EXPORT_SYMBOL(testb);

/**
 *  unlinkb:	- unlink first block of message
 *  @mp:	message to unlink
 */
__EXTERN_INLINE mblk_t *unlinkb(mblk_t *mp);

EXPORT_SYMBOL(unlinkb);

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

EXPORT_SYMBOL(xmsgsize);

static int __insq(queue_t *q, mblk_t *emp, mblk_t *nmp);

/**
 *  appq:	- append a message onto a queue
 *  @q:		the queue to append to
 *  @emp:	existing message on queue
 *  @nmp:	the message to append
 *
 *  CONTEXT: appq() can be called from any context; however, the caller is responsibile for
 *  exclusive access to and validity of the passed in message pointers.  This requires freezing the
 *  stream or otherwise locking the queue (e.g. MPSTR_QLOCK) in advance.
 *
 *  MP-STREAMS: The qwlock() that we take on the queue protects the queue elements, but this is a
 *  recursive lock so that if the queue has already been locked by us with freezestr() or
 *  MPSTR_QLOCK() then we will not fail to acquire the additional queue write lock.
 */
int
appq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	int result;
	unsigned long flags;

	qwlock_irqsave(q, &flags);
	result = __insq(q, emp ? emp->b_next : emp, nmp);
	qwunlock_irqrestore(q, &flags);
	/* do back enabling outside the locks */
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

EXPORT_SYMBOL(appq);

/**
 *  backq:	- find the queue upstream from this one
 *  @q:		this queue
 */
__EXTERN_INLINE queue_t *backq(queue_t *q);

EXPORT_SYMBOL(backq);

/**
 *  qbackenable: - backenable a queue
 *  @q:		the queue to backenable
 *
 *  CONTEXT: qbackenable() can be called from any context.
 */
void
qbackenable(queue_t *q)
{
	queue_t *qb, *q_back;

	for (q_back = qget(backq(q)); (qb = q_back) && !qb->q_qinfo->qi_srvp;
	     q_back = qget(backq(q)), qput(&qb)) ;
	if (qb)
		enableq(qb);	/* normal enable */
	qput(&qb);
	return;
}

EXPORT_SYMBOL(qbackenable);

/*
 *  bcangetany:		- check whether messages are in any (non-zero) band
 *  @q:			the queue to check for messages
 *
 *  bcangetany() operates like bcanget(); however, it checks for messages in any (non-zero)  band,
 *  not just in a specified band.  This is needed by strpoll() processing in the stream head to know
 *  when to set the POLLRDBAND flags.  Also, bcanputany() returns the band number of the highest
 *  priority band with messages.
 *
 *  IMPLEMENTATION: The current implementation is much faster than the older method of walking the
 *  queue bands, even considering that there were probably few, if any, queue bands.
 */
int
bcangetany(queue_t *q)
{
	int found = 0;
	unsigned long flags;
	mblk_t *b;

	qrlock_irqsave(q, &flags);
	b = q->q_first;
	/* find normal messages */
	for (; b && b->b_datap->db_type >= QPCTL; b = b->b_next) ;
	/* did we find it? */
	if (b)
		found = b->b_band;
	qrunlock_irqrestore(q, &flags);
	return (found);
}

EXPORT_SYMBOL(bcangetany);

/**
 *  bcanget:	- check whether messages are on a queue
 *  @q:		queue to check
 *  @band:	band to check
 *
 *  IMPLEMENTATION: For banded checks, we are pretty much forced to either walk the message queue or
 *  walk the queue bands.  Because banded messages are rare, there are likely fewer banded messages
 *  on the queue than there are queue bands and walking the queue is probably faster.  Also, walking
 *  the queue works for band zero (0) messages as well as long as we always skip high priority
 *  messages. That is what we do.
 */
int
bcanget(queue_t *q, unsigned char band)
{
	int found = 0;
	unsigned long flags;

	qrlock_irqsave(q, &flags);
	{
		mblk_t *b;

		b = q->q_first;
		/* find normal messages */
		for (; b && b->b_datap->db_type >= QPCTL; b = b->b_next) ;
		/* find band we are looking for */
		for (; b && b->b_band > band; b = b->b_next) ;
		/* did we find it? */
		if (b && b->b_band == band)
			found = 1;
	}
	qrunlock_irqrestore(q, &flags);
	return (found);
}

EXPORT_SYMBOL(bcanget);		/* include/sys/streams/stream.h */

/**
 *  bcanputany:		- check whether a message can be put to any (non-zero) band on a queue
 *  @q:			the queue to check
 *
 *  CONTEXT: STREAMS context (stream head locked).
 *
 *  LOCKING: None.
 */
int
bcanputany(queue_t *q)
{
	queue_t *q_next;

	if ((q_next = q)) {
		struct qband *qb;
		unsigned long flags;

		/* find first queue with service procedure or no q_next pointer */
		while ((q = q_next) && !q->q_qinfo->qi_srvp && (q_next = q->q_next)) ;
		qrlock_irqsave(q, &flags);
		for (qb = q->q_bandp; qb && test_bit(QB_FULL_BIT, &qb->qb_flag); qb = qb->qb_next) ;
		qrunlock_irqrestore(q, &flags);
		/* a non-existent band is considered unwritable */
		return (qb ? 1 : 0);
	}
	swerr();
	return (0);
}

EXPORT_SYMBOL(bcanputany);	/* include/sys/streams/stream.h */

/**
 *  bcanputnextany:	- check whether a mesage can be put to any (non-zero) band on the next queue
 *  @q:			the queue whose next queue to check
 *
 *  bcanputnextany() checks the next queue from the specified queue to see whether a message for any
 *  (existing) message band can be written to the queue.  Message bands to which no messages have
 *  been written to at least once are not checked.  This is the same as POLLWRBAND, S_WRBAND, and
 *  I_CHKBAND with ANYBAND as an argument.
 *
 *  NOTICES: bcanputnextany() is not a standard STREAMS function, but it is used by stream heads
 *  (for POLLWRBAND, S_WRBAND and I_CKBAND) and so we export it.
 *
 *  CONTEXT: bcanputnextany() can be called from STREAMS scheduler context, or from any context that
 *  holds a stream head read or write lock across the call.
 */
__EXTERN_INLINE int bcanputnextany(queue_t *q);

EXPORT_SYMBOL(bcanputnextany);	/* include/sys/streams/stream.h */

/**
 *  bcanput:		- check whether message of a given band can be put to a queue
 *  @q:			the queue to check
 *  @band:		the band to check
 *
 *  CONTEXT: STREAMS context (stream head locked).
 *
 *  LOCKING: None.
 */
int
bcanput(queue_t *q, unsigned char band)
{
	queue_t *q_next = q;

	assert(q);

	/* find first queue with service procedure or no q_next pointer */
	while ((q = q_next) && !q->q_qinfo->qi_srvp && (q_next = q->q_next)) ;
	if (band == 0) {
		if (test_bit(QFULL_BIT, &q->q_flag)) {
			set_bit(QWANTW_BIT, &q->q_flag);
			return (0);
		}
	} else {
		struct qband *qb;
		unsigned long flags;

		qrlock_irqsave(q, &flags);
		/* bands are sorted in decending priority so that we can quit the search early for
		   higher priority bands */
		for (qb = q->q_bandp; qb && (qb->qb_band > band); qb = qb->qb_next) ;
		if (qb && qb->qb_band == band && test_bit(QB_FULL_BIT, &qb->qb_flag)) {
			set_bit(QB_WANTW_BIT, &qb->qb_flag);
			qrunlock_irqrestore(q, &flags);
			return (0);
		}
		/* Note: a non-existent band is considered empty */
		qrunlock_irqrestore(q, &flags);
	}
	return (1);
}

EXPORT_SYMBOL(bcanput);

/**
 *  bcanputnext: - check whether messages can be put to queue after this one
 *  @q:		this queue
 *  @band:	band to check
 *
 *  CONTEXT: STREAMS context (stream head locked).
 */
__EXTERN_INLINE int bcanputnext(queue_t *q, unsigned char band);

EXPORT_SYMBOL(bcanputnext);

/**
 *  canenable:	- check whether service procedure will run
 *  @q:		queue to check
 */
__EXTERN_INLINE int canenable(queue_t *q);

EXPORT_SYMBOL(canenable);

/**
 *  canget:	- check whether normal band zero (0) messages are on queue
 *  @q:		queue to check
 */
__EXTERN_INLINE int canget(queue_t *q);

EXPORT_SYMBOL(canget);		/* include/sys/streams/stream.h */

/**
 *  canput:		- check wheter message can be put to a queue
 *  @q:			the queue to check
 *
 *  CONTEXT: canput() can be called from STREAMS context, or any context given that the stream head
 *  read or write lock is held across the call.  Care should be taken to acquire the necessary lock
 *  when this function is called from process context in a stream head.
 */
__EXTERN_INLINE int canput(queue_t *q);

EXPORT_SYMBOL(canput);		/* include/sys/streams/stream.h */

/**
 *  canputnext:		- check whether messages can be put to the queue after this one
 *  @q:			the queue whose next queue to check
 */
__EXTERN_INLINE int canputnext(queue_t *q);

EXPORT_SYMBOL(canputnext);

/*
 *  __find_qband:
 *
 * Find a queue band.  This must be called with the queue read or write locked.
 */
static inline struct qband *
__find_qband(queue_t *q, unsigned char band)
{
	struct qband *qb;

	for (qb = q->q_bandp; qb && qb->qb_band > band; qb = qb->qb_next) ;
	if (qb && qb->qb_band == band)
		return (qb);
	return (NULL);
}

/*
 *  __flushband: - flush messages from a queue band
 *  @q:		the queue to flush
 *  @band:	the band to flush
 *  @flag:	how, %FLUSHDATA or %FLUSHALL
 *  @mppp:	pointer to a pointer to the end of a message chain
 *
 *  NOTICES: This function must be called with a queue write lock held across the call.
 *
 *  IMPLEMENTATION: __flushband() uses a fast freeing technique where it removes an entire chain of
 *  messages where possible and schedules their being freed back to the message poll to the
 *  freechains() task under the STREAMS scheduler.  Flushing of long chains is more efficient for
 *  %FLUSHALL than for %FLUSHDATA.
 */
static int
__flushband(queue_t *q, unsigned char band, int flag, mblk_t ***mppp)
{
	int backenable = 0;

	if (likely(flag == FLUSHALL)) {
		if (likely(band == 0)) {
			mblk_t *b;

			/* Find first band zero message */
			b = q->q_first;
			for (; b && b->b_datap->db_type >= QPCTL; b = b->b_next) ;
			for (; b && b->b_band > 0; b = b->b_next) ;
			if ((**mppp = b)) {
				/* link around entire list */
				if (b->b_prev)
					b->b_prev->b_next = NULL;
				/* fix up markers */
				if (q->q_first == b)
					q->q_first = NULL;
				if (q->q_last == b)
					q->q_last = b->b_prev;
				*mppp = &q->q_last->b_next;
				**mppp = NULL;
				q->q_count = 0;
				q->q_msgs = 0;
				clear_bit(QFULL_BIT, &q->q_flag);
				clear_bit(QWANTW_BIT, &q->q_flag);
				backenable = 1;	/* always backenable when band empty */
			}
		} else {
			struct qband *qb;

			if (!(qb = __find_qband(q, band)))
				goto done;
			/* This is faster.  For flushall, we link the qband chain onto the free
			   list and null out qband counts and markers. */
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
		}
	} else if (likely(flag == FLUSHDATA)) {
		mblk_t *b, *b_next;

		if (likely(band == 0)) {
			/* Find first band zero message */
			b = q->q_first;
			for (; b && b->b_datap->db_type >= QPCTL; b = b->b_next) ;
			for (; b && b->b_band > 0; b = b->b_next) ;
			b_next = b;
			while ((b = b_next)) {
				b_next = b->b_next;
				if (isdatamsg(b)) {
					backenable |= __rmvq(q, b);
					**mppp = b;
					*mppp = &b->b_next;
					**mppp = NULL;
				}
			}
		} else {
			struct qband *qb;

			if (!(qb = __find_qband(q, band)))
				goto done;
			b_next = qb->qb_first;
			while ((b = b_next)) {
				b_next = b->b_next;
				if (isdatamsg(b)) {
					backenable |= __rmvq(q, b);
					**mppp = b;
					*mppp = &b->b_next;
					**mppp = NULL;
				}
			}
		}
	} else
		never();
      done:
	return (backenable);
}

/**
 *  flushband:	- flush messages from a queue band
 *  @q:		the queue to flush
 *  @band:	the band to flush
 *  @flag:	how to flush, %FLUSHALL or %FLUSHDATA
 *
 *  NOTICES: flushband(0, flag) and flushq(flag) are two different things.
 */
void
flushband(queue_t *q, int band, int flag)
{
	int backenable;
	unsigned long flags;
	mblk_t *mp = NULL, **mpp = &mp;

	assert(q);
	assert(flag == FLUSHDATA || flag == FLUSHALL);

	qwlock_irqsave(q, &flags);
	backenable = __flushband(q, flag, band, &mpp);
	qwunlock_irqrestore(q, &flags);
	if (backenable)
		qbackenable(q);
	/* we want to free messages with the locks off so that other CPUs can process this queue
	   and we don't block interrupts too long */
	mb();
	freechain(mp, mpp);
}

EXPORT_SYMBOL(flushband);

/*
 *  __flushq:	- flush messages from a queue
 *  @q:		queue from which to flush messages
 *  @flag:	how, %FLUSHDATA or %FLUSHALL
 *  @mppp:	pointer to a pointer to the end of a message chain
 *
 *  NOTICES: This function must be called with a queue write lock held across the call.
 *
 *  IMPLEMENTATION: __flushq() uses a fast freeing technique where it removes an entire chain of
 *  messages where possible and schedules their being freed back to the message poll to the
 *  freechains() task under the STREAMS scheduler.  Flushing of long chains is more efficient for
 *  %FLUSHALL than for %FLUSHDATA.
 */
int
__flushq(queue_t *q, int flag, mblk_t ***mppp)
{
	int backenable = 0;

	if (likely(flag == FLUSHALL)) {
		/* This is fast! For flushall, we link the whole chain onto the free list and null
		   out counts and markers */
		if ((**mppp = q->q_first)) {
			struct qband *qb;

			*mppp = &q->q_last->b_next;
			**mppp = NULL;
			q->q_first = q->q_last = NULL;
			q->q_count = 0;
			q->q_msgs = 0;
			clear_bit(QFULL_BIT, &q->q_flag);
			clear_bit(QWANTW_BIT, &q->q_flag);
			for (qb = q->q_bandp; qb; qb = qb->qb_next) {
				qb->qb_first = qb->qb_last = NULL;
				qb->qb_count = 0;
				qb->qb_msgs = 0;
				clear_bit(QB_FULL_BIT, &qb->qb_flag);
				clear_bit(QB_WANTW_BIT, &qb->qb_flag);
			}
			backenable = 1;	/* always backenable when queue empty */
		}
	} else if (likely(flag == FLUSHDATA)) {
		mblk_t *b, *b_next;

		b_next = q->q_first;
		while ((b = b_next)) {
			b_next = b->b_next;
			if (isdatamsg(b)) {
				backenable |= __rmvq(q, b);
				**mppp = b;
				*mppp = &b->b_next;
				**mppp = NULL;
			}
		}
	} else
		never();
	return (backenable);
}

/**
 *  flushq:	- flush messages from a queue
 *  @q:		the queue to flush
 *  @flag:	how to flush: %FLUSHDATA or %FLUSHALL
 *
 *  NOTICES: flushband(0, flag) and flushq(flag) are two different things.
 */
void
flushq(queue_t *q, int flag)
{
	int backenable;
	unsigned long flags;
	mblk_t *mp = NULL, **mpp = &mp;

	assert(q);
	assert(flag == FLUSHDATA || flag == FLUSHALL);

	qwlock_irqsave(q, &flags);
	backenable = __flushq(q, flag, &mpp);
	qwunlock_irqrestore(q, &flags);
	if (backenable)
		qbackenable(q);
	/* we want to free messages with the locks off so that other CPUs can process this queue
	   and we don't block interrupts too long */
	mb();
	freechain(mp, mpp);
}

EXPORT_SYMBOL(flushq);		/* include/sys/streams/stream.h */

/**
 *  freezestr:	- freeze a stream for direct queue access
 *  @q:		queue to freeze
 */
unsigned long
freezestr(queue_t *q)
{
	unsigned long flags = 0;

	/* XXX: are these strict locks necessary? */
	qwlock_irqsave(q, &flags);
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
qi_qadmin_t
getadmin(modID_t modid)
{
	qi_qadmin_t qadmin = NULL;
	struct fmodsw *fmod;

	if ((fmod = fmod_get(modid))) {
		struct streamtab *st;
		struct qinit *qi;

		if ((st = fmod->f_str) && (qi = st->st_rdinit))
			qadmin = qi->qi_qadmin;
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
modID_t
getmid(const char *name)
{
	struct fmodsw *fmod;
	struct cdevsw *cdev;

	if ((fmod = fmod_find(name))) {
		modID_t modid = fmod->f_modid;

		fmod_put(fmod);
		return (modid);
	}
	if ((cdev = cdev_find(name))) {
		modID_t modid = cdev->d_modid;

		sdev_put(cdev);
		return (modid);
	}
	return (0);
}

EXPORT_SYMBOL(getmid);

/*
 *  __getq:	- get next message off a queue
 *  @q:		the queue from which to get the message
 *
 *  CONTEXT:	This function must be called with the queue write locked.
 *
 *  RETURN VALUE: Returns a pointer to the next message, removed from the queue, or NULL if there is
 *  no message on the queue.
 */
static mblk_t *
__getq(queue_t *q, int *be)
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
mblk_t *
getq(queue_t *q)
{
	mblk_t *mp;
	int backenable = 0;
	unsigned long flags;

	ensure(q, return (NULL));
	/* XXX: are these strict locks necessary? */
	qwlock_irqsave(q, &flags);
	mp = __getq(q, &backenable);
	qwunlock_irqrestore(q, &flags);
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
static struct qband *
__get_qband(queue_t *q, unsigned char band)
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

static int __putq(queue_t *q, mblk_t *mp);

/*
 *  __insq:
 */
static int
__insq(queue_t *q, mblk_t *emp, mblk_t *nmp)
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
	} else {
		if (emp->b_datap->db_type >= QPCTL || emp->b_band < nmp->b_band)
			goto out_of_order;
		if (emp->b_prev && emp->b_prev->b_datap->db_type < QPCTL
		    && emp->b_prev->b_band > nmp->b_band)
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
int
insq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	int result;
	unsigned long flags;

	/* XXX: are these strict locks necessary? */
	qwlock_irqsave(q, &flags);
	result = __insq(q, emp, nmp);
	qwunlock_irqrestore(q, &flags);
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

EXPORT_SYMBOL(OTHERQ);

/**
 *  putnext:	- put a message on the queue next to this one
 *  @q:		this queue
 *  @mp:	message to put
 *
 *  CONTEXT: STREAMS context (stream head locked).
 *
 *  MP-STREAMS: Calling this function is MP-safe provided that it is called from the correct
 *  context, or if the caller can guarantee the validity of the q->q_next pointer.
 *
 *  NOTICES: If this function is called from an interrupt service routine (hard irq), use
 *  MPSTR_STPLOCK(9) and MPSTR_STPRELE(9) to hold a stream head write lock across the call.  The
 *  put(9) function will be called on the next queue invoking the queue's put procedure if
 *  syncrhonization has passed.  This can also happer for the next queue if the put procedure also
 *  does a putnext().  It might be necessary for the put procedure of all queues in the stream to
 *  know that they might be called at hard irq, if only so that they do not perform excessively long
 *  operations and impact system performance.
 *
 *  For a driver interrupt service routine, put(9) is a better choice, with a brief put procedure
 *  that does little more than a put(q).  Operations on the message can them be performed from the
 *  queue's service procedure, that is guaranteed to run at STREAMS scheduler context, with
 *  hard interrupts enabled.
 *
 */
__EXTERN_INLINE void putnext(queue_t *q, mblk_t *mp);

EXPORT_SYMBOL(putnext);		/* include/sys/streams/stream.h */

/*
 *  __putbq:
 */
static int
__putbq(queue_t *q, mblk_t *mp)
{
	int enable = 0;
	mblk_t *b_next, *b_prev;

	/* fast path for normal messages */
	if (likely(mp->b_datap->db_type < QPCTL)) {
		b_prev = NULL;
		b_next = q->q_first;
		/* skip high priority */
		while (unlikely(b_next && b_next->b_datap->db_type >= QPCTL)) {
			b_prev = b_next;
			b_next = b_prev->b_next;
		}
		/* skip higher bands */
		while (unlikely(b_next && b_next->b_band > mp->b_band)) {
			b_prev = b_next;
			b_next = b_prev->b_next;
		}
		if (likely(mp->b_band == 0)) {
		      hipri:
			if ((q->q_count += (mp->b_size = msgsize(mp))) > q->q_hiwat)
				set_bit(QFULL_BIT, &q->q_flag);
			mp->b_bandp = NULL;
		      banded:
			if (q->q_last == b_prev)
				q->q_last = mp;
			if (q->q_first == b_next)
				q->q_first = mp;
			q->q_msgs++;
			if ((mp->b_next = b_next))
				b_next->b_prev = mp;
			if ((mp->b_prev = b_prev))
				b_prev->b_next = mp;
			mp->b_queue = qget(q);
			return (1 + enable);
		} else {
			struct qband *qb;

			if (unlikely((qb = __get_qband(q, mp->b_band)) == NULL))
				return (0);
			if (qb->qb_last == b_prev)
				qb->qb_last = mp;
			if (qb->qb_first == b_next)
				qb->qb_first = mp;
			qb->qb_msgs++;
			if ((qb->qb_count += (mp->b_size = msgsize(mp))) > qb->qb_hiwat)
				set_bit(QB_FULL_BIT, &qb->qb_flag);
			mp->b_bandp = bget(qb);
			goto banded;
		}
	} else {
		b_prev = NULL;
		b_next = q->q_first;
		/* modules should never put priority messages back on a queue */
		enable = 2;	/* and this is why */
		goto hipri;
	}
}

/**
 *  putbq:	- put a message back on a queue
 *  @q:		queue to place back message
 *  @mp:	message to place back
 */
int
putbq(queue_t *q, mblk_t *mp)
{
	int result;
	unsigned long flags;

	qwlock_irqsave(q, &flags);
	result = __putbq(q, mp);
	qwunlock_irqrestore(q, &flags);
	switch (result) {
	case 3:		/* success - priority enable */
		qenable(q);
		return (1);
	case 2:		/* success - normal enable */
		never();
		if (test_bit(QWANTR_BIT, &q->q_flag))
			enableq(q);
	case 1:		/* success */
		return (1);
	default:
		never();
	case 0:		/* failure */
		/* This should never happen, because it takes a qband structure allocation failure
		   to get here, and since we are putting the message back on the queue, there
		   should already be a qband structure. Unless, however, putbq() is just used to
		   insert messages ahead of others rather than really putting them back.
		   Nevertheless, a way to avoid this error is to always ensure that a qband
		   structure exists (e.g., with strqset) before calling putbq on a band for the
		   first time. */
		return (0);
	}
}

EXPORT_SYMBOL(putbq);

/**
 *  putctl:	- put a control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 */
__EXTERN_INLINE int putctl(queue_t *q, int type);

EXPORT_SYMBOL(putctl);

/**
 *  putctl1:	- put a 1-byte control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 *  @param:	the 1 byte parameter
 */
__EXTERN_INLINE int putctl1(queue_t *q, int type, int param);

EXPORT_SYMBOL(putctl1);

/**
 *  putctl2:	- put a 2-byte control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 *  @param1:	the first 1 byte parameter
 *  @param2:	the second 1 byte parameter
 */
__EXTERN_INLINE int putctl2(queue_t *q, int type, int param1, int param2);

EXPORT_SYMBOL(putctl2);

/**
 *  putnextctl:	- put a control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 */
__EXTERN_INLINE int putnextctl(queue_t *q, int type);

EXPORT_SYMBOL(putnextctl);

/**
 *  putnextctl1: - put a 1-byte control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 *  @param:	the 1 byte parameter
 */
__EXTERN_INLINE int putnextctl1(queue_t *q, int type, int param);

EXPORT_SYMBOL(putnextctl1);

/**
 *  putnextctl2: - put a 2-byte control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 *  @param1:	the first 1 byte parameter
 *  @param2:	the second 1 byte parameter
 */
__EXTERN_INLINE int putnextctl2(queue_t *q, int type, int param1, int param2);

EXPORT_SYMBOL(putnextctl2);

/*
 *  __putq:	- put a message block to a queue
 *  @q:		queue to which to put the message
 *  @mp:	message to put
 *
 *
 *  __putq() is a non-locking version of putq().
 *
 *  Optomized for normal messags arriving at an empty queue.  This is because in a smoothly running
 *  system queues should be empty and high-priority and banded messages are rare.
 *
 *  NOTICES: If the queue has a service procedure and the %QNOENB flag is not set, putq(9) enables
 *  queues when they are empty, they have the %QWANTR flag set meaning that getq() failed to read
 *  from the queue, and a message arrives.  putq(9) also enables queues whenever a high-priority
 *  message arrives.
 *
 *  1) When a banded message arrives at an empty queue band, should the queue be enabled?
 *
 */
static int
__putq(queue_t *q, mblk_t *mp)
{
	int enable;
	mblk_t *b_prev, *b_next;

	/* fast path for normal messages */
	if (likely(mp->b_datap->db_type < QPCTL && mp->b_band == 0)) {
		b_prev = q->q_last;
		b_next = NULL;
		enable = b_prev ? 0 : 1;
	      hipri:
		if ((q->q_count += (mp->b_size = msgsize(mp))) > q->q_hiwat)
			set_bit(QFULL_BIT, &q->q_flag);
		mp->b_bandp = NULL;
	      banded:
		if (q->q_last == b_prev)
			q->q_last = mp;
		if (q->q_first == b_next)
			q->q_first = mp;
		q->q_msgs++;
		if ((mp->b_next = b_next))
			b_next->b_prev = mp;
		if ((mp->b_prev = b_prev))
			b_prev->b_next = mp;
		mp->b_queue = qget(q);
		return (1 + enable);	/* success */
	}
	/* find position of priority messages */
	b_prev = NULL;
	b_next = q->q_first;
	while (unlikely(b_next && b_next->b_datap->db_type >= QPCTL)) {
		b_prev = b_next;
		b_next = b_prev->b_next;
	}
	if (likely(mp->b_datap->db_type >= QPCTL)) {
		enable = 2;
		goto hipri;
	} else {
		struct qband *qb;

		if (unlikely((qb = __get_qband(q, mp->b_band)) == NULL))
			return (0);
		/* find position for our message */
		while (unlikely(b_next && b_next->b_band > mp->b_band)) {
			b_prev = b_next;
			b_next = b_prev->b_next;
		}
		enable = q->q_first ? 0 : 1;
		if (qb->qb_last == b_prev)
			qb->qb_last = mp;
		if (qb->qb_first == b_next)
			qb->qb_first = mp;
		qb->qb_msgs++;
		if ((qb->qb_count += (mp->b_size = msgsize(mp))) > qb->qb_hiwat)
			set_bit(QB_FULL_BIT, &qb->qb_flag);
		mp->b_bandp = bget(qb);
		goto banded;
	}
}

/**
 *  putq:	- put a message block to a queue
 *  @q:		queue to which to put the message
 *  @mp:	message to put
 */
int
putq(queue_t *q, mblk_t *mp)
{
	int result;
	unsigned long flags;

	qwlock_irqsave(q, &flags);
	result = __putq(q, mp);
	qwunlock_irqrestore(q, &flags);
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
		assert(result == 0);
	case 0:		/* failure */
		/* This can happen and it is bad.  We use the return value to putq but it is
		   typically ignored by the module.  One way to ensure that this never happens is
		   to call strqset() for the band before calling putq on the band for the first
		   time. (See also putbq()) */
		return (0);
	}
}

EXPORT_SYMBOL(putq);

static int __setsq(queue_t *q, struct fmodsw *fmod, int mux);
int setsq(queue_t *q, struct fmodsw *fmod, int mux);

/**
 *  qattach: - attach a stream head, module or driver queue pair to a stream head
 *  @sd:	stream head data structure identifying stream
 *  @fmod:	&struct fmodsw pointer identifying module or driver
 *  @devp:	&dev_t pointer providing opening device number
 *  @oflag:	open flags
 *  @sflag:	streams flag, can be %DRVOPEN, %CLONEOPEN, %MODOPEN
 *  @crp:	&cred_t pointer to credentials of opening task
 *
 *  CONTEXT: Must only be called from stream head or qopen()/qclose() procedures.
 */
int
qattach(struct stdata *sd, struct fmodsw *fmod, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct streamtab *st;
	queue_t *q;
	dev_t odev;
	struct cdevsw *cdev;
	int err;

	err = -ENOMEM;
	if (!(q = allocq()))
		goto error;
	(q + 0)->q_flag |= QHLIST | QNOENB;
	(q + 1)->q_flag |= QHLIST | QNOENB;
	if ((err = __setsq(q, fmod, 0)) < 0)
		goto freeq_error;
	qinsert(sd, q);		/* half insert under stream head */
	odev = *devp;		/* remember calling device number */
	if ((err = qopen(q, devp, oflag, sflag, crp))) {
		if (err > 0)
			err = -err;
		goto qerror;
	}
	if (sflag != MODOPEN) {
		/* XXX: Magic Garden say that if we are opening and the major device number
		   returned from qopen() is not the same as the major number passed, that we need
		   to do a setq on the queue from the streamtab associated with the new major
		   device number. */
		/* this just doesn't work with locking.... FIXME */
		if (getmajor(odev) != getmajor(*devp)) {
			err = -ENOENT;
			if (!(cdev = cdrv_get(getmajor(*devp))))
				goto enoent;
			if ((struct fmodsw *) cdev != fmod) {
				err = -ENOENT;
				if (!(st = cdev->d_str))
					goto put_noent;
				if ((err = setsq(q, (struct fmodsw *) cdev, 0)) < 0)
					goto put_noent;
			}
			cdrv_put(cdev);
		}
	} else if (odev != *devp)
		/* module is supposed to ignore devp */
		*devp = odev;
	qprocson(q);		/* in case qopen() forgot */
	return (0);
      put_noent:
	cdrv_put(cdev);
      enoent:
	qclose(q, oflag, crp);	/* need to call close */
      qerror:
	*devp = odev;
	qprocsoff(q);		/* in case qopen called qprocson */
	qdelete(q);		/* half delete */
      freeq_error:
	qput(&q);
      error:
	return (err);
}

EXPORT_SYMBOL(qattach);

/**
 *  qdelete:	- delete a queue pair from a stream
 *  @rq:	read queue of queue pair to delete
 *
 *  qdelete() half-deletes the queue pair identified by @rq from the stream to which it belongs.
 *  The q->q_next pointers of the queue pair to be deleted, @rq, are adjusted, but the stream
 *  remains unaffected.  qprocsoff() must be called before calling qdelete() to properly remove the
 *  queue pair from the stream.
 *
 *  CONTEXT: qdelete() should only be called from the qattach() or qdetach() procedure or a stream
 *  head open or close procedure.
 *
 *  NOTICES: rq should have already been removed from a queue with qprocsoff() (but we check again
 *  anyway) and must be a valid pointer or bad things will happen.
 */
void
qdelete(queue_t *q)
{
	struct queinfo *qu = (typeof(qu)) q;
	struct stdata *sd = qu->qu_str;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);
	unsigned long flags = 0;

	wq = rq + 1;
	/* XXX: are these strict locks necessary? */
	swlock_irqsave(sd, &flags);
	qput(&rq->q_next);
	qput(&wq->q_next);
	qu->qu_str = NULL;
	swunlock_irqrestore(sd, &flags);
	qput(&q);
	sd_put(sd);
}

EXPORT_SYMBOL(qdelete);

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
int
qdetach(queue_t *q, int flags, cred_t *crp)
{
	int err;

	err = qclose(q, flags, crp);
	qprocsoff(q);		/* in case qclose forgot */
	qdelete(q);		/* half delete */
	return (err);
}

EXPORT_SYMBOL(qdetach);

/**
 *  qinsert:	- insert a queue pair below another in a stream
 *  @sd:	stream head under which to insert
 *  @irq:	read queue of queue pair to insert
 *
 *  qinsert() half-inserts the queue pair identified by @irq beneath the queue pair on the stream
 *  identified by @sd.  This is only a half-insert.  The q->q_next pointers of the queue pair to be
 *  inserted, @irq, are adjusted, but the stream remains unaffected.  qprocson() must be called on
 *  @irq to complete the insertion and properly set flags.
 *
 *  Context: qinsert() is only meant to be called from the qattach() procedure or a stream head open
 *  procedure.
 *
 *  Notices: irq should not already be inserted on a queue or bad things will happen.
 *
 *  Locking: This function needs to be called with a read lock on the stream head.  Because brq
 *  (bottom read queue) must be protected, the read lock on the stream head must be taken by the
 *  caller.
 */
void
qinsert(struct stdata *sd, queue_t *irq)
{
	queue_t *iwq, *srq, *swq;
	struct queinfo *iqu, *squ;

	srlock_bh(sd);
	srq = sd->sd_rq;
	iqu = (typeof(iqu)) irq;
	iwq = irq + 1;
	squ = (typeof(squ)) srq;
	swq = srq + 1;
	iqu->qu_str = sd_get(squ->qu_str);
	irq->q_next = qget(srq);
	if (swq->q_next != srq) {	/* not a fifo */
		iwq->q_next = qget(swq->q_next);
	} else {		/* is a fifo */
		iwq->q_next = qget(irq);
	}
	srunlock_bh(sd);
}

EXPORT_SYMBOL(qinsert);

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
void
qprocsoff(queue_t *q)
{
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);

	assert(current_context() <= CTX_STREAMS);
	if (!test_and_set_bit(QHLIST_BIT, &rq->q_flag)) {
		unsigned long flags = 0;

		set_bit(QHLIST_BIT, &wq->q_flag);
		set_bit(QNOENB_BIT, &rq->q_flag);	/* XXX */
		set_bit(QNOENB_BIT, &wq->q_flag);	/* XXX */
		/* spin here waiting for queue procedures to exit */
		hwlock_irqsave(rq, &flags);
		/* bypass this module: works for FIFOs and PIPEs too */
		if ((bq = backq(rq))) {
			queue_t *qn = bq->q_next;

			bq->q_next = NULL;
			bq->q_next = qget(rq->q_next);
			qput(&qn);
		}
		if ((bq = backq(wq))) {
			queue_t *qn = bq->q_next;

			bq->q_next = NULL;
			bq->q_next = qget(wq->q_next);
			qput(&qn);
		}
		hwunlock_irqrestore(rq, &flags);
		/* XXX: put procs must check QHLIST bit after acquiring hrlock */
		/* XXX: srv procs must check QNOENB bit after acquiring hrlock */
	}
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
 *  Context: qprocson() should only be called from qattach(), qdetach() or a stream head open
 *  procedure.  The user should call qprocson() from the qopen() procedure before returning.
 *
 *  Notices: qprocson() fully inserts the queue pair into the stream.  It must be half-inserted with
 *  qinsert() before qprocson() can be called.
 *
 *  Locking:  The module's qopen() procedure is called with no stream head locks held.  Before linking
 *  the queue pair in, qprocson takes a write lock on the stream head.  This means that all queue
 *  synchronous procedures must exit before the lock is acquired.  We are holding the stream head
 *  open bit, so no other open or close can occur.  Stream head write locks are only be taken from
 *  user context (we just try to take them from interrupt level (in_interrupt() && !in_irq())),
 *  therefore, the write lock does not need to disable the bottom half.
 */
void
qprocson(queue_t *q)
{
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);

	assert(current_context() <= CTX_STREAMS);
	if (test_and_clear_bit(QHLIST_BIT, &rq->q_flag)) {
		unsigned long flags = 0;

		clear_bit(QHLIST_BIT, &wq->q_flag);
		clear_bit(QNOENB_BIT, &rq->q_flag);	/* XXX */
		clear_bit(QNOENB_BIT, &wq->q_flag);	/* XXX */
		/* spin here waiting for queue procedures to exit */
		hwlock_irqsave(rq, &flags);
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
		hwunlock_irqrestore(rq, &flags);
	}
}

EXPORT_SYMBOL(qprocson);

/**
 *  qpop:	- pop a moudle from a stream
 *  @sd:	stream head
 *  @oflag:	open flags to call module open procedure
 *  @crp:	credential pointer for module open procedure
 */
int
qpop(struct stdata *sd, int oflag, cred_t *crp)
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

EXPORT_SYMBOL(qpop);

/**
 *  qpush:	- push a module onto a stream
 *  @sd:	stream head
 *  @name:	name of the module to push
 *  @devp:	device number pointer
 *  @oflag:	open flags to call module open procedure
 *  @crp:	credential pointer for module open procedure
 */
int
qpush(struct stdata *sd, const char *name, dev_t *devp, int oflag, cred_t *crp)
{
	int err;
	struct fmodsw *fmod;

	if (sd->sd_pushcnt >= sysctl_str_nstrpush)
		goto enosr;
	if (!(fmod = fmod_find(name)))
		goto einval;
	if ((err = qattach(sd, fmod, devp, oflag, MODOPEN, crp)) != 0)
		goto error;
	sd->sd_pushcnt++;
      error:
	fmod_put(fmod);
	return (err);
      einval:
	return (-EINVAL);
      enosr:
	return (-ENOSR);
}

EXPORT_SYMBOL(qpush);

/**
 *  qreply:	- reply with a message
 *  @q:		queue from which to reply
 *  @mp:	message reply
 */
__EXTERN_INLINE void qreply(queue_t *q, mblk_t *mp);

EXPORT_SYMBOL(qreply);

/**
 *  qsize:	- calculate number of messages on a queue
 *  @q:		queue to count messages
 */
__EXTERN_INLINE ssize_t qsize(queue_t *q);

EXPORT_SYMBOL(qsize);

/**
 *  qcountstrm:	- count the numer of messages along a stream
 *  @q:		queue to begin with
 *
 *  NOTICES: qcountstrm() is only for LiS compatibility.  Note that the count may be invalidated
 *  before it is completed being calculated, which is rather useless unless per-stream
 *  syncrhonization is being performed.
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: Loose.  We only take references to queue structures along the way to ensure that they
 *  do not disappear while we are in the middle of checking them.
 */
ssize_t
qcountstrm(queue_t *q)
{
	ssize_t count = 0;
	queue_t *q_next;

	if ((q_next = qget(q)))
		for (; (q = q_next) && SAMESTR(q); q_next = qget(q->q_next), qput(&q))
			count += q->q_count;
	return (count);
}

EXPORT_SYMBOL(qcountstrm);

/**
 *  RD:		- find read queue from write queu
 *  @q:		write queue pointer
 */
__EXTERN_INLINE queue_t *RD(queue_t *q);

EXPORT_SYMBOL(RD);

/*
 *  __rmvq:	- remove a message from a queue
 *  @q:		the queue from which to remove the message
 *  @mp:	the message to removed
 *
 *  __rmvq() is a version of rmvq(9) that takes no locks.
 *
 *  CONTEXT: This function takes no locks and must be called with the queue write locked, either
 *  explicitly or by calling freezestr().
 *
 *  RETURN VALUE: Returns an integer indicating whether back enabling should be performed.  A return
 *  value of zero (0) indicates that back enabling is not necessary.  A return value of one (1)
 *  indicates that back enabling of queues is required.
 */
int
__rmvq(queue_t *q, mblk_t *mp)
{
	int backenable = 0;
	mblk_t *b_next, *b_prev;

	assert(q == mp->b_queue);
	/* We know which queue the message belongs too, make sure its still there. */

	b_prev = mp->b_prev;
	b_next = mp->b_next;
	if (b_prev)
		b_prev->b_next = b_next;
	if (b_next)
		b_next->b_prev = b_prev;
	mp->b_next = mp->b_prev = NULL;
	if (q->q_first == mp)
		q->q_first = b_next;
	if (q->q_last == mp)
		q->q_last = b_prev;
	q->q_msgs--;
	if (likely(mp->b_bandp == NULL)) {
		q->q_count -= mp->b_size;
		if (q->q_count < q->q_hiwat)
			clear_bit(QFULL_BIT, &q->q_flag);
		if (q->q_count <= q->q_lowat && test_and_clear_bit(QWANTW_BIT, &q->q_flag))
			backenable = 1;
	} else {
		struct qband *qb = mp->b_bandp;

		bput(&mp->b_bandp);
		if (qb->qb_first == mp)
			qb->qb_first = b_next;
		if (qb->qb_last == mp)
			qb->qb_last = b_prev;
		qb->qb_msgs--;
		qb->qb_count -= mp->b_size;
		if (qb->qb_count < qb->qb_hiwat)
			clear_bit(QB_FULL_BIT, &qb->qb_flag);
		if (qb->qb_count <= qb->qb_lowat && test_and_clear_bit(QB_WANTW_BIT, &qb->qb_flag))
			backenable = 1;
	}
	qput(&mp->b_queue);
	return (backenable);
}

/**
 *  rmvq:	- remove a messge from a queue
 *  @q:		queue from which to remove message
 *  @mp:	message to remove
 *
 *  CONTEXT: rmvq() must be called with the queue write locked (e.g. using freezestr(9) or
 *  MPSTR_QLOCK(9)), or some other mutual exclusion mechanism.
 *
 *  LOCKING: We take our own write locks to protect the queue structure in case the caller has not.
 *
 *  NOTICES: rmvq() panics when passed null pointers.  rmvq() panics if a write lock has not been
 *  taken on the queue.  rmvq() panics if the message is not a queue, or not on the specified queue.
 */
void
rmvq(queue_t *q, mblk_t *mp)
{
	int backenable = 0;
	unsigned long flags;

	assert(q);
	assert(mp);

	qwlock_irqsave(q, &flags);
	{
		assert(q->q_owner == current);
		assert(q->q_nest >= 1);
		assert(mp->b_queue);
		assert(q == mp->b_queue);

		backenable = __rmvq(q, mp);
	}
	qwunlock_irqrestore(q, &flags);
	if (backenable)
		qbackenable(q);
}

EXPORT_SYMBOL(rmvq);

/**
 *  SAMESTR:	- check whether this and next queue have the same stream head
 *  @q:		this queue
 */
__EXTERN_INLINE int SAMESTR(queue_t *q);

EXPORT_SYMBOL(SAMESTR);

/*
 *  __setq:
 */
static void
__setq(queue_t *q, struct qinit *rinit, struct qinit *winit)
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
void
setq(queue_t *q, struct qinit *rinit, struct qinit *winit)
{
	queue_t *rq = q;
	queue_t *wq = q + 1;
	unsigned long flags;

	/* always take read lock before write lock */
	/* nobody else takes two locks */
	local_irq_save(flags);
	qwlock(rq);
	qwlock(wq);
	__setq(q, rinit, winit);
	/* unlock in reverse order */
	qwunlock(wq);
	qwunlock(rq);
	local_irq_restore(flags);
}

EXPORT_SYMBOL(setq);

#if defined CONFIG_STREAMS_SYNCQS
struct syncq *global_syncq = NULL;
#endif

/*
 *  __setsq:	- set synchronization queues for a new queue pair
 *  @fmod:	fmodsw table entry for this module
 *  @mux:	are we being linked under a multiplexing driver?
 *
 *  This function establishes the links to the necessary syncrhonization queues for a newly created
 *  queue pair.  Both outer and inner perimiters are established.  D_MP modules have no perimiters.
 *  SQLVL_NOP modules have no inner perimeter.  D_MTOUTPERIM modules have an outer perimeter.
 *  Modules cannot have an outer perimeter and an inner perimeter of D_MTPERMOD or SQLVL_MODULE or
 *  wider.
 *
 *  If the inner perimeter is SQLVL_MODULE or SQLVL_ELSEWHERE, then it is the responsibility of the
 *  registration function to find or allocate a synchronization queue and attach it to fmod->f_syncq
 *  for use by this function.  This makes the algorithm for locating an "elsewhere" module
 *  independent of this function.  Although this function could allocate synchronization queues for
 *  the SQLVL_MODULE case, to avoid races it should only be performed in the registration functions.
 */
static int
__setsq(queue_t *q, struct fmodsw *fmod, int mux)
{
#if defined CONFIG_STREAMS_SYNCQS
	/* make sure there is none to start */
	sq_put(&(q + 0)->q_syncq);
	sq_put(&(q + 1)->q_syncq);
	if (fmod == NULL)
		return (0);	/* just remove */
	if (!(fmod->f_flag & D_MP)) {
		struct syncq *sqr, *sqw, *sqo = NULL;

		/* propagate flags to f_sqlvl, this should have already been done by the
		   registration function */
		if (fmod->f_sqlvl == SQLVL_DEFAULT) {
			if (fmod->f_flag & D_MTPERQ)
				fmod->f_sqlvl = SQLVL_QUEUE;
			else if (fmod->f_flag & D_MTQPAIR)
				fmod->f_sqlvl = SQLVL_QUEUEPAIR;
			else if (fmod->f_flag & D_MTPERMOD)
				fmod->f_sqlvl = SQLVL_MODULE;
		}
		if (fmod->f_flag & D_MTOUTPERIM) {
			/* find the outer perimeter syncq for the module */
			if ((fmod->f_flag & (D_MTPERMOD | D_MP))
			    || (!((1 << fmod->f_sqlvl)
				  & ((1 << SQLVL_NOP) | (1 << SQLVL_QUEUE) |
				     (1 << SQLVL_QUEUEPAIR)))))
				pswerr(("invalid flags\n"));
			if (!(sqo = sq_get(fmod->f_syncq))) {
				pswerr(("registration function forgot to allocate syncq\n"));
				if (!(sqo = sq_alloc()))
					goto enomem;
				fmod->f_syncq = sqo;
				sqo->sq_level = fmod->f_sqlvl;
				sqo->sq_flag = fmod->f_flag | D_MTOUTPERIM;
			}
			/* we have a reference to sqo */
		}
		switch (fmod->f_sqlvl) {
		case SQLVL_NOP:	/* none */
			sqr = sqo;
			sqw = sq_get(sqr);
			break;
		case SQLVL_QUEUE:
			/* allocate one syncq for each queue */
			if (!(sqr = sq_alloc())) {
				sq_put(&sqo);
				goto enomem;
			}
			if (!(sqw = sq_alloc())) {
				sq_put(&sqo);
				sq_put(&sqr);
				goto enomem;
			}
			sqr->sq_level = fmod->f_sqlvl;
			sqr->sq_flag = fmod->f_flag & ~(D_MTOUTPERIM);
			sqr->sq_outer = sqo;
			sqw->sq_level = fmod->f_sqlvl;
			sqw->sq_flag = fmod->f_flag & ~(D_MTOUTPERIM);
			sqw->sq_outer = sq_get(sqo);
			break;
		case SQLVL_QUEUEPAIR:
			/* allocate one syncq for the queue pair */
			if (!(sqr = sq_alloc())) {
				sq_put(&sqo);
				goto enomem;
			}
			sqr->sq_level = fmod->f_sqlvl;
			sqr->sq_flag = fmod->f_flag & ~(D_MTOUTPERIM);
			sqr->sq_outer = sqo;
			sqw = sq_get(sqr);
			break;
		default:
			swerr();
		case SQLVL_DEFAULT:
		case SQLVL_MODULE:	/* default */
			/* find the module and use its syncq */
			/* The registration function is responsible for allocating the module
			   synchronization queue and attaching it to the module structure */
			if (!(sqr = sq_get(fmod->f_syncq))) {
				pswerr(("registration function forgot to allocate syncq\n"));
				if (!(sqr = sq_alloc())) {
					sq_put(&sqo);
					goto enomem;
				}
				fmod->f_syncq = sqr;
				sqr->sq_level = fmod->f_sqlvl;
				sqr->sq_flag = fmod->f_flag & ~(D_MTOUTPERIM);
				sqr->sq_outer = sqo;
			}
			sqw = sq_get(sqr);
			break;
		case SQLVL_ELSEWHERE:
			/* find the elsewhere syncq and use it */
			/* The registration function is responsible for finding and allocating the
			   external synchronization queue and attaching it to the module structure */
			if (!(sqr = sq_get(fmod->f_syncq))) {
				pswerr(("registration function forgot to allocate syncq\n"));
				if (!(sqr = sq_alloc())) {
					sq_put(&sqo);
					goto enomem;
				}
				fmod->f_syncq = sqr;
				sqr->sq_level = fmod->f_sqlvl;
				sqr->sq_flag = fmod->f_flag & ~(D_MTOUTPERIM);
			}
			sqw = sq_get(sqr);
			/* cannot have outer perimeter at this level */
			sq_put(&sqo);
			break;
		case SQLVL_GLOBAL:	/* for testing */
			/* use the global syncq */
			if (!(sqr = sq_get(global_syncq))) {
				if (!(sqr = sq_alloc())) {
					sq_put(&sqo);
					goto enomem;
				}
				sqr->sq_level = fmod->f_sqlvl;
				sqr->sq_flag = fmod->f_flag & ~(D_MTOUTPERIM);
			}
			sqw = sq_get(sqr);
			/* cannot have outer perimeter at this level */
			sq_put(&sqo);
			break;
		}
		(q + 0)->q_syncq = sqr;
		(q + 1)->q_syncq = sqw;
	}
#endif
	if (fmod->f_str) {
		if (!mux)
			setq(q, fmod->f_str->st_rdinit, fmod->f_str->st_wrinit);
		else
			setq(q, fmod->f_str->st_muxrinit, fmod->f_str->st_muxwinit);
	} else
		swerr();
	return (0);
#if defined CONFIG_STREAMS_SYNCQS
      enomem:
	return (-ENOMEM);
#endif
}

/**
 *  setsq:	- set synchornization charateristics on a queue pair
 *  @q:		read queue in queue pair to set
 *  @fmod:	module to which queue pair belongs
 *  @mux:	non-zero for lower multiplexing driver queue pair
 *
 *  Set synchronization queue associated with a new queue pair.
 */
int
setsq(queue_t *q, struct fmodsw *fmod, int mux)
{
	int result;
	queue_t *rq = q;
	queue_t *wq = q + 1;
	unsigned long flags;

	/* always take read lock before write lock */
	/* nobody else takes two locks */
	local_irq_save(flags);
	qwlock(rq);
	qwlock(wq);
	result = __setsq(q, fmod, mux);
	/* unlock in reverse order */
	qwunlock(wq);
	qwunlock(rq);
	local_irq_restore(flags);
	return (result);
}

/**
 *  strqget:	- get characteristics of a queue
 *  @q:		queue to query
 *  @what:	what characteristic to get
 *  @band:	from which queue band
 *  @val:	location of return value
 */
int
strqget(queue_t *q, qfields_t what, unsigned char band, long *val)
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

		qrlock_irqsave(q, &flags);
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
		qrunlock_irqrestore(q, &flags);
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
int
strqset(queue_t *q, qfields_t what, unsigned char band, long val)
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

		/* XXX: are these strict locks necessary? */
		qwlock_irqsave(q, &flags);
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
		qwunlock_irqrestore(q, &flags);
	}
	return (-err);
}

EXPORT_SYMBOL(strqset);

/*
 *  This is a default implementation for strlog(9).  When SL_CONSOLE is set, we print directly to
 *  the console using printk(9).  For SL_ERROR and SL_TRACE, we have no STREAMS error or trace
 *  loggers running, so we marks those messages as unseen by those loggers.  We also provide a hook
 *  here so that the strutil package can hook into this call.  Because we cannot filter, only
 *  SL_CONSOLE messages are printed to the system logs.  This follows the rules for setting the
 *  priority according described in log(4).
 */
static int
vstrlog_default(short mid, short sid, char level, unsigned short flag, char *fmt, va_list args)
{
	int rval = 1;

	if (flag & SL_CONSOLE) {
		static spinlock_t str_err_lock = SPIN_LOCK_UNLOCKED;
		static char str_err_buf[LOGMSGSZ];
		unsigned long flags;
		short lev = (short) level;

		/* XXX: are these strict locks necessary? */
		spin_lock_irqsave(&str_err_lock, flags);
		vsnprintf(str_err_buf, sizeof(str_err_buf), fmt, args);
#define STRLOG_PFX "strlog(%hd)[%hd,%hd]: %s\n"
		if (flag & SL_FATAL)
			printk(KERN_CRIT STRLOG_PFX, lev, mid, sid, str_err_buf);
		else if (flag & SL_ERROR)
			printk(KERN_ERR STRLOG_PFX, lev, mid, sid, str_err_buf);
		else if (flag & SL_WARN)
			printk(KERN_WARNING STRLOG_PFX, lev, mid, sid, str_err_buf);
		else if (flag & SL_NOTE)
			printk(KERN_NOTICE STRLOG_PFX, lev, mid, sid, str_err_buf);
		else if (flag & SL_TRACE)
			printk(KERN_DEBUG STRLOG_PFX, lev, mid, sid, str_err_buf);
		else
			printk(KERN_INFO STRLOG_PFX, lev, mid, sid, str_err_buf);
#undef STRLOG_PFX
		spin_unlock_irqrestore(&str_err_lock, flags);
	}
	if (flag & SL_ERROR)
		rval = 0;	/* no error logger */
	if (flag & SL_TRACE)
		rval = 0;	/* no trace logger */
	return (rval);
}

vstrlog_t vstrlog = &vstrlog_default;

EXPORT_SYMBOL(vstrlog);

/**
 *  strlog:	- log a STREAMS message
 *  @mid:	module id
 *  @sid:	stream id
 *  @level:	severity level
 *  @flag:	flags controlling distribution
 *  @fmt:	printf(3) format
 *  @...:	format specific arguments
 */
int
strlog(short mid, short sid, char level, unsigned short flag, char *fmt, ...)
{
	int result = 0;

	if (vstrlog != NULL) {
		va_list args;

		va_start(args, fmt);
		result = (*vstrlog) (mid, sid, level, flag, fmt, args);
		va_end(args);
	}
	return (result);
}

EXPORT_SYMBOL(strlog);

/**
 *  unfreezestr:	- thaw a stream frozen with freezestr()
 *  @q:			the queue in the stream to thaw
 *  @flags:		spl flags
 */
void
unfreezestr(queue_t *q, unsigned long flags)
{
	flags = q->q_iflags;
	qwunlock_irqrestore(q, &flags);
}

EXPORT_SYMBOL(unfreezestr);

/**
 *  WR:		- get write queue in queue pair
 *  @q:		read queue pointer
 */
__EXTERN_INLINE queue_t *WR(queue_t *q);

EXPORT_SYMBOL(WR);

static spinlock_t cmn_err_lock = SPIN_LOCK_UNLOCKED;

/*
 *  vcmn_err:
 */
void
vcmn_err(int err_lvl, const char *fmt, va_list args)
{
	unsigned long flags;
	static char cmn_err_buf[1024], *cmn_err_ptr = cmn_err_buf;

	/* XXX: are these strict locks necessary? */
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

EXPORT_SYMBOL(vcmn_err);

/**
 *  cmn_err:	- print a command error
 *  @err_lvl:	severity
 *  @fmt:	printf(3) format
 *  @...:	format arguments
 */
void
cmn_err(int err_lvl, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vcmn_err(err_lvl, fmt, args);
	va_end(args);
	return;
}

EXPORT_SYMBOL(cmn_err);

__EXTERN_INLINE int copyin(const void *from, void *to, size_t len);

EXPORT_SYMBOL(copyin);

__EXTERN_INLINE int copyout(const void *from, void *to, size_t len);

EXPORT_SYMBOL(copyout);

__EXTERN_INLINE void delay(unsigned long ticks);

EXPORT_SYMBOL(delay);

__EXTERN_INLINE int drv_getparm(const unsigned int parm, void *value_p);

EXPORT_SYMBOL(drv_getparm);

__EXTERN_INLINE unsigned long drv_hztomsec(unsigned long hz);

EXPORT_SYMBOL(drv_hztomsec);

__EXTERN_INLINE unsigned long drv_hztousec(unsigned long hz);

EXPORT_SYMBOL(drv_hztousec);

__EXTERN_INLINE unsigned long drv_msectohz(unsigned long msec);

EXPORT_SYMBOL(drv_msectohz);

__EXTERN_INLINE int drv_priv(cred_t *crp);

EXPORT_SYMBOL(drv_priv);

__EXTERN_INLINE unsigned long drv_usectohz(unsigned long usec);

EXPORT_SYMBOL(drv_usectohz);

__EXTERN_INLINE void drv_usecwait(unsigned long usec);

EXPORT_SYMBOL(drv_usecwait);

__EXTERN_INLINE major_t getmajor(dev_t dev);

EXPORT_SYMBOL(getmajor);

__EXTERN_INLINE minor_t getminor(dev_t dev);

EXPORT_SYMBOL(getminor);

__EXTERN_INLINE dev_t makedevice(major_t major, minor_t minor);

EXPORT_SYMBOL(makedevice);
