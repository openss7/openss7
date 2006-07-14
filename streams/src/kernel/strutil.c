/*****************************************************************************

 @(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.128 $) $Date: 2006/06/22 01:17:11 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/06/22 01:17:11 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strutil.c,v $
 Revision 0.9.2.128  2006/06/22 01:17:11  brian
 - syncing notebook, latest changes are not stable yet

 Revision 0.9.2.127  2006/06/19 20:51:28  brian
 - more optimizations

 Revision 0.9.2.126  2006/06/18 20:54:05  brian
 - minor optimizations from profiling

 Revision 0.9.2.125  2006/06/14 10:37:25  brian
 - defeat a lot of debug traces in debug mode for testing
 - changes to allow strinet to compile under LiS (why???)

 Revision 0.9.2.124  2006/06/05 02:53:34  brian
 - working up udp zero-copy

 Revision 0.9.2.119  2006/02/20 10:59:22  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ident "@(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.128 $) $Date: 2006/06/22 01:17:11 $"

static char const ident[] =
    "$RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.128 $) $Date: 2006/06/22 01:17:11 $";

#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>

#include <asm/atomic.h>		/* for atomic operations */
#include <asm/bitops.h>		/* for atomic bit operations */
#include <linux/compiler.h>	/* for expected/unexpected */
#include <linux/spinlock.h>	/* for spinlocks */

#include <linux/kernel.h>	/* for vsnprintf, FASTCALL(), fastcall */
#include <linux/sched.h>	/* for wakeup functions */
#include <linux/wait.h>		/* for wait queues */
#include <linux/types.h>	/* for various types */
#include <linux/interrupt.h>	/* for in_interrupt() */

#include <linux/skbuff.h>	/* for sk_buffs */

#include <asm/cache.h>		/* for L1_CACHE_BYTES */

#include <stdbool.h>		/* for bool, true and false */

#ifndef __STRUTIL_EXTERN_INLINE
#define __STRUTIL_EXTERN_INLINE inline streams_fastcall
#endif

#include "sys/strdebug.h"

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
#include "src/kernel/strsched.h"	/* for current_context() */
#include "src/kernel/strutil.h"	/* for q locking and puts and gets */

/* some faster macros for known input */

#define _WR(__rq) ((__rq) + 1)
#define _RD(__wq) ((__wq) - 1)

STATIC spinlock_t db_ref_lock = SPIN_LOCK_UNLOCKED;

STATIC streams_noinline streams_fastcall __unlikely void
db_inc_slow(register dblk_t *db)
{
	unsigned long flags;

	spin_lock_irqsave(&db_ref_lock, flags);
	++db->db_ref;
	spin_unlock_irqrestore(&db_ref_lock, flags);
}

STATIC streams_inline streams_fastcall __hot_get void
db_inc(register dblk_t * db)
{
	/* When the number of references is 1 the buffer is exclusive to the
	 * caller and locking is not required. */
	if (likely(db->db_ref == 1))
		return (void) (db->db_ref = 2);
	return db_inc_slow(db);
}

STATIC streams_noinline streams_fastcall __unlikely int
db_dec_and_test_slow(register dblk_t * db)
{
	unsigned long flags;
	register bool ret;

	spin_lock_irqsave(&db_ref_lock, flags);
	ret = (--db->db_ref == 0);
	spin_unlock_irqrestore(&db_ref_lock, flags);
	return ret;
}

STATIC streams_inline streams_fastcall __hot_in int
db_dec_and_test(register dblk_t * db)
{
	/* When the number of references is 1 the buffer is exclusive to the caller and locking is
	   not required. */
	if (likely(db->db_ref == 1))
		return ((db->db_ref = 0) == 0);
	return (db_dec_and_test_slow(db));
}

#if 0
#define msgblk_offset \
	(((unsigned char *)&((struct mdbblock *)0)->msgblk) - ((unsigned char *)0))
#define datablk_offset \
	(((unsigned char *)&((struct mdbblock *)0)->datablk) - ((unsigned char *)0))
#define databuf_offset \
	(((unsigned char *)&((struct mdbblock *)0)->databuf[0]) - ((unsigned char *)0))

STATIC streams_inline streams_fastcall __hot_in mblk_t *
db_to_mb(dblk_t * db)
{
	return ((mblk_t *) ((unsigned char *) db - datablk_offset + msgblk_offset));
}
STATIC streams_inline streams_fastcall __hot_in unsigned char *
db_to_buf(dblk_t * db)
{
	return ((unsigned char *) db - datablk_offset + databuf_offset);
}

STATIC streams_inline streams_fastcall __hot_in dblk_t *
mb_to_db(mblk_t *mb)
{
	return ((dblk_t *) ((unsigned char *) mb - msgblk_offset + datablk_offset));
}
STATIC streams_inline streams_fastcall __hot_out struct mdbblock *
mb_to_mdb(mblk_t *mb)
{
	return ((struct mdbblock *) ((unsigned char *) mb - msgblk_offset));
}
#else
STATIC streams_inline streams_fastcall __hot_out struct mdbblock *
mb_to_mdb(register mblk_t *mb)
{
	return ((struct mdbblock *) mb);
}
STATIC streams_inline streams_fastcall __hot_in mblk_t *
db_to_mb(register dblk_t * db)
{
	return ((mblk_t *) ((struct mbinfo *) db - 1));
}
STATIC streams_inline streams_fastcall __hot_in unsigned char *
db_to_buf(register dblk_t * db)
{
	return (&mb_to_mdb(db_to_mb(db))->databuf[0]);
}

STATIC streams_inline streams_fastcall __hot_in dblk_t *
mb_to_db(register mblk_t *mb)
{
	return (&mb_to_mdb(mb)->datablk.d_dblock);
}
#endif

/**
 *  adjmsg:	- adjust a message
 *  @mp:	message to adjust
 *  @length:	length to trim
 *
 *  Trims -@length bytes from the head of the message or @length bytes from the end of the message.
 *
 *  Return Values: (1) - success; (0) - failure
 */
streams_fastcall __unlikely int
adjmsg(mblk_t *mp, register ssize_t length)
{
	mblk_t *b, *bp;
	int type;
	ssize_t blen, size;

	if (!mp)
		goto error;
	type = mp->b_datap->db_type;
	if (length == 0)
		return (1);	/* success */
	else if (length > 0) {
		/* trim from head of message */
		/* check if we can do the trim */
		for (size = length, b = mp; b; b = b->b_cont) {
			if (b->b_datap->db_type != type)
				goto error;
			if ((blen = b->b_wptr - b->b_rptr) <= 0)
				continue;
			if ((size -= blen) <= 0)
				goto trim_head;
		}
		goto error;
	      trim_head:
		/* do the trimming */
		for (size = length, b = mp; b; b = b->b_cont) {
			if ((blen = b->b_wptr - b->b_rptr) <= 0)
				continue;
			if ((size -= blen) < 0) {
				blen += size;
				b->b_rptr += blen;
				return (1);	/* success */
			}
			b->b_rptr += blen;
		}
	} else if (length < 0) {
		/* trim from tail of message */
		/* check if we can do the trim */
		for (size = 0, bp = NULL, b = mp; b; b = b->b_cont) {
			if (b->b_datap->db_type != type) {
				type = b->b_datap->db_type;
				bp = b;
				size = 0;
			}
			if ((blen = b->b_wptr - b->b_rptr) <= 0)
				continue;
			size += blen;
		}
		if (size >= -length)
			goto trim_tail;
		goto error;
	      trim_tail:
		/* do the trimming */
		for (b = bp; b; b = b->b_cont) {
			if ((blen = b->b_wptr - b->b_rptr) <= 0)
				continue;
			if (size < -length) {
				size -= blen;
				b->b_wptr = b->b_rptr;
				continue;
			}
			if ((size -= blen) < -length)
				b->b_wptr += length + size;
		}
	}
      error:
	return (0);
}

EXPORT_SYMBOL_NOVERS(adjmsg);		/* include/sys/streams/stream.h */

streams_fastcall __hot_out void
freeb_skb(caddr_t arg)
{
	struct sk_buff *skb = (typeof(skb)) arg;

	dassert(skb != NULL);
	kfree_skb(skb);
}

/**
 *  skballoc:	- allocate a message block with a socket buffer
 *  @skb:	socket buffer
 *  @priority:	priority of message block header allocation
 */
streams_fastcall __hot_in mblk_t *
skballoc(struct sk_buff *skb, uint priority)
{
	mblk_t *mp;

	if (likely((mp = mdbblock_alloc(priority, &skballoc)) != NULL)) {
		struct mdbblock *md = mb_to_mdb(mp);
		dblk_t *db = &md->datablk.d_dblock;
		struct free_rtn *db_frtnp = (struct free_rtn *)md->databuf;

		/* set up internal buffer with free routine info */
		db_frtnp->free_func = &freeb_skb;
		db_frtnp->free_arg = (caddr_t) skb;
		/* set up data block */
		db->db_frntp = db_frtnp;
		db->db_base = skb->head;
		db->db_lim = skb->end;
		db->db_ref = 1;
		db->db_type = M_DATA;
		db->db_size = skb->end - skb->head;
		db->db_flag = DB_SKBUFF;
		/* set up message block */
		mp->b_next = mp->b_prev = mp->b_cont = NULL;
		mp->b_rptr = skb->data;
		mp->b_wptr = skb->tail;
		mp->b_datap = db;
		mp->b_band = 0;
		mp->b_flag = 0;
		/* TODO: if the socket buffer contains checksum information we can copy it to the
		 * message block. */
		mp->b_csum = 0;
		return (mp);
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(skballoc);

/**
 *  esballoc:	- allocate a message block with an external buffer
 *  @base:	base address of message buffer
 *  @size:	size of the message buffer
 *  @priority:	priority of message block header allocation
 *  @freeinfo:	free routine callback
 */
streams_fastcall __hot_in mblk_t *
esballoc(unsigned char *base, size_t size, uint priority, frtn_t *freeinfo)
{
	mblk_t *mp;

	if (likely((mp = mdbblock_alloc(priority, &esballoc)) != NULL)) {
		struct mdbblock *md = mb_to_mdb(mp);
		dblk_t *db = &md->datablk.d_dblock;

		/* set up internal buffer with free routine info */
		*(struct free_rtn *) md->databuf = *freeinfo;
		/* set up data block */
		db->db_frtnp = (struct free_rtn *) md->databuf;
		db->db_base = base;
		db->db_lim = base + size;
		db->db_ref = 1;
		db->db_type = M_DATA;
		db->db_size = size;
		db->db_flag = 0;
		/* set up message block */
		mp->b_next = mp->b_prev = mp->b_cont = NULL;
		mp->b_rptr = mp->b_wptr = db->db_base;
		mp->b_datap = db;
		mp->b_band = 0;
		mp->b_flag = 0;
		mp->b_csum = 0;
		return (mp);
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(esballoc);

/**
 *  allocb_skb:	- allocate a message block with a socket buffer
 *  @size:	size of message block in bytes
 *  @priority:	priority of the allocation
 */
streams_fastcall __hot_out mblk_t *
allocb_skb(size_t size, uint priority)
{
	struct sk_buff *skb;

	if (likely((skb = alloc_skb(size, (priority == BPRI_WAITOK)
				      ? GFP_KERNEL : GFP_ATOMIC)) != NULL)) {
		mblk_t *mp;

		if (likely((mp = skballoc(skb, priority)) != NULL))
			return (mp);
		kfree_skb(skb);
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(allocb_skb);

/**
 *  allocb:	- allocate a message block
 *  @size:	size of message block in bytes
 *  @priority:	priority of the allocation
 */
streams_fastcall __hot_out mblk_t *
allocb(size_t size, uint priority)
{
	mblk_t *mp;

	if (likely((mp = mdbblock_alloc(priority, &allocb)) != NULL)) {
		struct mdbblock *md = mb_to_mdb(mp);
		unsigned char *base = md->databuf;
		dblk_t *db = &md->datablk.d_dblock;

		if (unlikely(size <= FASTBUF))
			size = FASTBUF;
		else if (unlikely((base = kmem_alloc(size, (priority == BPRI_WAITOK)
					     ? KM_SLEEP : KM_NOSLEEP)) == NULL))
			goto buf_alloc_error;
		/* set up data block */
		db->db_frtnp = NULL;
		db->db_base = base;
		db->db_lim = base + size;
		db->db_ref = 1;
		db->db_type = M_DATA;
		db->db_size = size;
		db->db_flag = 0;
		/* set up message block */
		mp->b_next = mp->b_prev = mp->b_cont = NULL;
		mp->b_rptr = mp->b_wptr = db->db_base;
		mp->b_datap = db;
		mp->b_band = 0;
		mp->b_flag = 0;
		mp->b_csum = 0;
	}
	_printd(("%s: allocated mblk %p, refs %d\n", __FUNCTION__, mp, (int) mp->b_datap->db_ref));
	return (mp);
      buf_alloc_error:
	mdbblock_free(mp);
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(allocb);

/**
 *  copyb:	- copy a message block
 *  @bp:	the message block to copy
 *
 *  Return Value: Returns the copy of the message or %NULL on failure.
 *
 *  Notices: Unlike LiS we do not align the copy.  The driver must me wary of alignment.
 */
streams_fastcall __unlikely mblk_t *
copyb(register mblk_t *mp)
{
	mblk_t *b = NULL;

	if (mp) {
		ssize_t size = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;

		if ((b = allocb(size, BPRI_MED))) {
			bcopy(mp->b_rptr, b->b_wptr, size);
			b->b_wptr += size;
			b->b_datap->db_type = mp->b_datap->db_type;
			b->b_band = mp->b_band;
			b->b_flag = mp->b_flag;
			b->b_csum = mp->b_csum;
		}
	}
	return (b);
}

EXPORT_SYMBOL_NOVERS(copyb);		/* include/sys/streams/stream.h */

/**
 *  copymsg:	- copy a message
 *  @msg:	message to copy
 *
 *  Copies all the message blocks in message @msg and returns a pointer to the copied message.
 */
__STRUTIL_EXTERN_INLINE mblk_t *copymsg(register mblk_t *mp);

EXPORT_SYMBOL_NOVERS(copymsg);		/* include/sys/streams/stream.h */

/**
 *  ctlmsg:	- test for control message type
 *  @type:	type to test
 */
streams_fastcall __unlikely int
ctlmsg(unsigned char type)
{
	unsigned char mod = (type & ~QPCTL);

	/* just so happens there is a gap in the QNORM messages right at M_PCPROTO */
	return (((1 << mod) & ((1 << M_DATA) | (1 << M_PROTO) | (1 << (M_PCPROTO & ~QPCTL)))) == 0);
}

EXPORT_SYMBOL_NOVERS(ctlmsg);		/* include/sys/streams/stream.h */

/**
 *  datamsg:	- test for data message type
 *  @type:	type to test
 */
streams_fastcall __unlikely int
datamsg(unsigned char type)
{
	unsigned char mod = (type & ~QPCTL);

	/* just so happens there is a gap in the QNORM messages right at M_PCPROTO */
	return (((1 << mod) &
		 ((1 << M_DATA) | (1 << M_PROTO) | (1 << (M_PCPROTO & ~QPCTL)) | (1 << M_DELAY))) !=
		0);
}

EXPORT_SYMBOL_NOVERS(datamsg);

/**
 *  dupb:	- duplicates a message block
 *  @bp:	message block to duplicate
 */
streams_fastcall __hot mblk_t *
dupb(mblk_t *bp)
{
	mblk_t *mp;

	if (likely((mp = mdbblock_alloc(BPRI_MED, &dupb)) != NULL)) {
		struct mdbblock *md = (struct mdbblock *) mp;
		dblk_t *db = bp->b_datap;

		db_inc(db);
#if 0
		*mp = *bp;
		mp->b_next = mp->b_prev = mp->b_cont = NULL;
#else
		mp->b_next = NULL;
		mp->b_prev = NULL;
		mp->b_cont = NULL;
		mp->b_rptr = bp->b_rptr;
		mp->b_wptr = bp->b_wptr;
		mp->b_datap = bp->b_datap;
		mp->b_band = bp->b_band;
		mp->b_flag = bp->b_flag;
		mp->b_csum = bp->b_csum;

#endif
		/* mark datab unused */
		db = &md->datablk.d_dblock;
		db->db_ref = 0;
		return (mp);
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(dupb);

/**
 *  dupmsg:	- duplicate a message
 *  @msg:	message to duplicate
 *
 *  Duplicates an entire message using dupb() to duplicate each of the message blocks in the
 *  message.  Returns a pointer to the duplicate message.
 */
__STRUTIL_EXTERN_INLINE mblk_t *dupmsg(mblk_t *mp);

EXPORT_SYMBOL_NOVERS(dupmsg);		/* include/sys/streams/stream.h */

/**
 *  freeb:	- free a message block
 *  @mp:	message block to free
 */
streams_fastcall __hot void
freeb(mblk_t *mp)
{
	dblk_t *dp, *db;

	/* check null ptr, message still on queue */
	dassert(mp != NULL);
	/* we always null these when we take a message off a queue */
	dassert(mp->b_prev == NULL);
	dassert(mp->b_next == NULL);

	db = mp->b_datap;
	mp->b_datap = NULL;

	_printd(("%s: freeing mblk %p, refs %d\n", __FUNCTION__, mp, (int) (db ? db->db_ref : 0)));

	/* check double free */
	dassert(db != NULL);
	dassert(db->db_ref > 0);

	/* message block marked free above */
	if (likely(db_dec_and_test(db) != 0)) {
		/* free data block */
		mblk_t *mb = db_to_mb(db);

		if (likely(db->db_base != db_to_buf(db))) {
			register struct free_rtn *frtnp;

			/* handle external data buffer */
			if ((frtnp = db->db_frtnp)) {
				register void streamscall (*free_func) (caddr_t);

				if ((free_func = frtnp->free_func))
					free_func(frtnp->free_arg);
			} else
				kmem_free(db->db_base, db->db_size);
		}
		/* the entire mdbblock can go if the associated msgb is also unused */
		if (likely(mb->b_datap == NULL))
			mdbblock_free(mb);
	}
	/* if the message block refers to the associated data block then we have already freed the
	   mdbblock above when necessary; otherwise the entire mdbblock can go if the datab is also 
	   unused */
	if (unlikely(db != (dp = mb_to_db(mp)) && !dp->db_ref)) {
		mdbblock_free(mp);
		return;
	}
	return;
}

EXPORT_SYMBOL_NOVERS(freeb);

/**
 *  freemsg:	- free a message
 *  @mp:	the message to free
 */
__STRUTIL_EXTERN_INLINE void freemsg(mblk_t *mp);

EXPORT_SYMBOL_NOVERS(freemsg);

/**
 *  isdatablk:	- test data block for data type
 *  @dp:	data block to test
 */
__STRUTIL_EXTERN_INLINE int isdatablk(dblk_t * dp);

EXPORT_SYMBOL_NOVERS(isdatablk);

/**
 *  isdatamsg:	- test a message block for data type
 *  @mp:	message block to test
 */
__STRUTIL_EXTERN_INLINE int isdatamsg(mblk_t *mp);

EXPORT_SYMBOL_NOVERS(isdatamsg);

/**
 *  pcmsg:	- data block type for priority
 *  @type:	the type to check
 */
__STRUTIL_EXTERN_INLINE int pcmsg(unsigned char type);

EXPORT_SYMBOL_NOVERS(pcmsg);

/**
 *  linkb:	- link message block onto message
 *  @mp1:	message onto which to link
 *  @mp2:	message block to link
 */
__STRUTIL_EXTERN_INLINE void linkb(register mblk_t *mp1, register mblk_t *mp2);

EXPORT_SYMBOL_NOVERS(linkb);

/**
 *  linkmsg:	- link messages
 *  @mp1:	message onto which to link
 *  @mp2:	message to link
 */
__STRUTIL_EXTERN_INLINE mblk_t *linkmsg(mblk_t *mp1, mblk_t *mp2);

EXPORT_SYMBOL_NOVERS(linkmsg);

/**
 *  msgdsize:	- calculate size of data in message
 *  @mp:	message across which to calculate data bytes
 */
__STRUTIL_EXTERN_INLINE size_t msgdsize(register mblk_t *mp);

EXPORT_SYMBOL_NOVERS(msgdsize);

/**
 *  msgpullup:	- pull up bytes into a message
 *  @mp:	message to pull up
 *  @length:	number of bytes to pull up
 *
 *  Pulls up @length  bytes into the returned message.  This is for handling headers as a contiguous
 *  range of bytes.
 */
streams_fastcall __unlikely mblk_t *
msgpullup(mblk_t *mp, ssize_t length)
{
	mblk_t *msg = NULL, **bp = &msg;
	register mblk_t *b = NULL;
	register ssize_t size, blen, type, len;

	if (!msg)
		goto error;
	if (!(len = length))
		goto copy_rest;
	type = msg->b_datap->db_type;
	size = 0;
	for (b = msg; b; b = b->b_cont) {
		if ((blen = b->b_wptr - b->b_rptr) <= 0)
			continue;
		if (b->b_datap->db_type == type)
			break;
		if ((size += blen) > len && len > 0)
			goto copy_len;
	}
	if (size <= len)
		goto error;
	if (len < 0)
		len = size;
      copy_len:
	if (!(msg = allocb(len, BPRI_MED)))
		goto error;
	bp = &msg->b_cont;
	for (b = msg; b; b = b->b_cont) {
		if ((blen = b->b_wptr - b->b_rptr) <= 0)
			continue;
		if (b->b_datap->db_type != type)
			break;
		if ((size = blen - len) <= 0) {
			bcopy(b->b_rptr, msg->b_wptr, blen);
			msg->b_wptr += blen;
			len -= blen;
			continue;
		} else {
			bcopy(b->b_rptr, msg->b_wptr, len);
			msg->b_wptr += len;
			if (!(*bp = copyb(b)))
				goto error;
			(*bp)->b_datap->db_type = b->b_datap->db_type;
			(*bp)->b_rptr += size;
			bp = &(*bp)->b_cont;
			b = b->b_cont;
			break;
		}
	}
      copy_rest:
	if (b)			/* just copy rest of message */
		if (!(*bp = copymsg(b)))
			goto error;
	return (msg);
      error:
	if (msg)
		freemsg(msg);
	return (msg);
}

EXPORT_SYMBOL_NOVERS(msgpullup);

/**
 *  msgsize:	- calculate size of a message
 *  @mp:	message for which to calculate size
 */
__STRUTIL_EXTERN_INLINE size_t msgsize(mblk_t *mp);

EXPORT_SYMBOL_NOVERS(msgsize);

/**
 *  pullupmsg:	- pull up bytes into first data block in message
 *  @mp:	message to pull up
 *  @len:	number of bytes to pull up
 *
 *  Pulls up @length  bytes into the initial data block in message @mp.  This is for handling headers
 *  as a contiguous range of bytes.
 */
streams_fastcall __unlikely int
pullupmsg(mblk_t *mp, register ssize_t len)
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
	/* mark msgb unused */
	md->msgblk.m_mblock.b_datap = NULL;
	/* fill out data block */
	dp = &md->datablk.d_dblock;
	dp->db_frtnp = NULL;
	dp->db_base = base;
	dp->db_lim = base + size;
	dp->db_ref = 1;
	dp->db_type = db->db_type;
	dp->db_size = size;
	dp->db_flag = 0;
	/* copy from old initial datab */
	if ((blen = bp->b_wptr > bp->b_rptr ? bp->b_wptr - bp->b_rptr : 0)) {
		bcopy(mp->b_rptr, base, blen);
		size -= blen;
	}
	/* point old msgb at new datab */
	mp->b_rptr = mp->b_wptr = db->db_base;
	mp->b_wptr += blen;
	mp->b_datap = dp;
	/* remove a reference from old initial datab */
	if (likely(db_dec_and_test(db) != 0)) {
		/* free data block */
		mblk_t *mb = db_to_mb(db);

		if (likely(db->db_base != db_to_buf(db))) {
			register struct free_rtn *frtnp;

			/* handle external data buffer */
			if ((frtnp = db->db_frtnp)) {
				register void streamscall (*free_func) (caddr_t);

				if ((free_func = frtnp->free_func))
					free_func(frtnp->free_arg);

			} else
				kmem_free(db->db_base, db->db_size);
		}
		/* the entire mdbblock can go if the associated msgb is also unused */
		if (likely(mb->b_datap == NULL))
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

EXPORT_SYMBOL_NOVERS(pullupmsg);

/**
 *  rmvb:   - remove a message block from a message
 *  @mp:    message from which to remove the block
 *  @bp:    the block to remove
 */
__STRUTIL_EXTERN_INLINE mblk_t *rmvb(register mblk_t *mp, register mblk_t *bp);

EXPORT_SYMBOL_NOVERS(rmvb);

/**
 *  testb:	- test allocate of a message block
 *  @size:	size of buffer for which to test
 *  @priority:	allocation priority to test
 */
streams_fastcall __unlikely int
testb(register size_t size, uint priority)
{
	mblk_t *mp;

	(void) priority;
	if ((mp = allocb(size, priority)))
		freeb(mp);
	return (mp != NULL);
}

EXPORT_SYMBOL_NOVERS(testb);

/**
 *  unlinkb:	- unlink first block of message
 *  @mp:	message to unlink
 */
__STRUTIL_EXTERN_INLINE mblk_t *unlinkb(register mblk_t *mp);

EXPORT_SYMBOL_NOVERS(unlinkb);

__STRUTIL_EXTERN_INLINE mblk_t *unlinkmsg(register mblk_t *mp, register mblk_t *bp);

EXPORT_SYMBOL_NOVERS(unlinkmsg);

/**
 *  xmsgsize:	- calculate size in message of same type as first data block
 *
 *  Notices: This is not bug-to-bug compatible with LiS.  Some differences: LiS wraps at a message
 *  size of 65636 and cannot handle message blocks larger than 65636.  LiS will consider a non-zero
 *  initial block (such as that left by adjmsg()) as the first message block type when it should
 *  not.  This implementation does not wrap the size, and skips initial zero-length message blocks.
 *  This implementation of xmsgsize does not span non-zero blocks of different types.
 */
streams_fastcall __unlikely size_t
xmsgsize(mblk_t *mp)
{
	register mblk_t *bp = mp;
	register int type = 0;
	register size_t size = 0, blen;	/* find first non-zero length block for type */

	for (; bp; bp = bp->b_cont)
		if ((blen = bp->b_wptr - bp->b_rptr) > 0) {
			type = bp->b_datap->db_type;
			size += blen;
			break;
		}		/* finish counting rest of message */
	for (; bp; bp = bp->b_cont)
		if ((blen = bp->b_wptr - bp->b_rptr) > 0) {
			if (bp->b_datap->db_type == type)
				size += blen;
			else
				break;
		}
	return (size);
}

EXPORT_SYMBOL_NOVERS(xmsgsize);

/**
 *  backq:	- find the queue upstream from this one
 *  @q:		this queue
 *
 *  CONTEXT: STREAMS only.
 */
__STRUTIL_EXTERN_INLINE queue_t *backq(register queue_t *q);

EXPORT_SYMBOL_NOVERS(backq);

STATIC struct qband *__get_qband(queue_t *q, unsigned char band);

/**
 *  qbackenable: - backenable a queue
 *  @q:		the queue to backenable
 *  @band:	(highest) band number to backenable
 *  @bands:	array of bands to backenable (if more than one)
 *
 *  CONTEXT: qbackenable() can be called from any context.
 *
 *  MP-STREAMS: Because qbackenable() can be invoked from outside streams (i.e. from getq()), it
 *  takes a Stream head read lock.  This is a little bit overkill for intermediate modules, so we
 *  now only take a Stream head read lock if the queue is a Stream end (i.e., no q->q_next pointer).
 */
streams_noinline streams_fastcall void
qbackenable(queue_t *q, const unsigned char band, const char bands[])
{
	queue_t *q_back;
	struct stdata *sd;

	dassert(q);
	sd = qstream(q);
	dassert(sd);
	prlock(sd);

	for (q_back = backq(q); (q = q_back) && (q_back = backq(q)) && !q->q_qinfo->qi_srvp;) ;

	if (likely(q != NULL)) {
		/* If we are backenabling a Stream end queue then we will be specific about why it
		   was backenabled, this gives the Stream head or driver information about for
		   which specific bands flow control has subsided. */
		if (unlikely(q_back == NULL)) {
			unsigned long pl;
			struct qband *qb;

			pl = qwlock(q);
			if (likely(bands == NULL)) {
				if (band == 0)
					set_bit(QBACK_BIT, &q->q_flag);
				else if ((qb = __get_qband(q, band)))
					set_bit(QB_BACK_BIT, &qb->qb_flag);
			} else {
				int bnum;

				if (bands[0])
					set_bit(QBACK_BIT, &q->q_flag);
				for (bnum = band; bnum > 0; bnum--)
					if (bands[bnum] && (qb = __get_qband(q, bnum)))
						set_bit(QB_BACK_BIT, &qb->qb_flag);
			}
			qwunlock(q, pl);
		}
		/* SVR4 SPG - noenable() does not prevent a queue from being back enabled by flow
		   control */
		qenable(q);	/* always enable if a service procedure exists */
	}
	prunlock(sd);
}

EXPORT_SYMBOL_NOVERS(qbackenable);

/*
 *  bcangetany:		- check whether messages are in any (non-zero) band
 *  @q:			the queue to check for messages
 *
 *  bcangetany() operates like bcanget(); however, it checks for messages in any (non-zero)  band,
 *  not just in a specified band.  This is needed by strpoll() processing in the stream head to know
 *  when to set the POLLRDBAND flags.  Also, bcangetany() returns the band number of the highest
 *  priority band with messages.
 *
 *  IMPLEMENTATION: The current implementation is much faster than the older method of walking the
 *  queue bands, even considering that there were probably few, if any, queue bands.
 */
streams_inline streams_fastcall __hot int
bcangetany(queue_t *q)
{
	int found = 0;
	mblk_t *b;
	unsigned long pl;

	dassert(q);

	pl = qrlock(q);
	b = q->q_first;
	/* find normal messages */
	for (; b && b->b_datap->db_type >= QPCTL; b = b->b_next) ;
	/* did we find it? */
	if (b)
		found = b->b_band;
	qrunlock(q, pl);
	return (found);
}

EXPORT_SYMBOL_NOVERS(bcangetany);

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
 *
 *  NOTICES: The caller is responsible for the validity of the passed in queue pointer.
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: No locks are required across the call.
 */
streams_fastcall __hot int
bcanget(queue_t *q, unsigned char band)
{
	int found = 0;
	unsigned long pl;

	dassert(q);

	pl = qrlock(q);
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
	qrunlock(q, pl);
	return (found);
}

EXPORT_SYMBOL_NOVERS(bcanget);		/* include/sys/streams/stream.h */

STATIC streams_inline streams_fastcall __hot int
__bcanputany(queue_t *q)
{
	bool result;
	unsigned long pl;

	/* find first queue with service procedure or no q_next pointer */
	for (; !q->q_qinfo->qi_srvp && q->q_next; q = q->q_next) ;

	pl = qrlock(q);
	result = (q->q_blocked < q->q_nband);
	_ptrace(("queue bands blocked %d, available %d\n", q->q_blocked, q->q_nband));
	qrunlock(q, pl);

	return (result);
}

/**
 *  bcanputany:		- check whether a message can be put to any (non-zero) band on a queue
 *  @q:			the queue to check
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: Takes a Stream head read lock.
 */
streams_fastcall int
bcanputany(queue_t *q)
{
	bool result;
	struct stdata *sd;

	dassert(q);
	sd = qstream(q);
	dassert(sd);

	prlock(sd);
	result = __bcanputany(q);
	prunlock(sd);

	return (result);
}

EXPORT_SYMBOL_NOVERS(bcanputany);	/* include/sys/streams/stream.h */

/**
 *  bcanputnextany:	- check whether a mesage can be put to any (non-zero) band on the next queue
 *  @q:			the queue whose next queue to check
 *
 *  bcanputnextany() checks the next queue from the specified queue to see whether a message for any
 *  (existing) message band can be written to the queue.  Message bands to which no messages have
 *  been written to at least once are not checked.  This is the same as POLLWRBAND, S_WRBAND, and
 *  I_CANPUT with ANYBAND as an argument.
 *
 *  NOTICES: bcanputnextany() is not a standard STREAMS function, but it is used by stream heads
 *  (for POLLWRBAND, S_WRBAND and I_CANPUT) and so we export it.
 *
 *  CONTEXT: bcanputnextany() can be called from STREAMS scheduler context, or from any context that
 *  holds a stream head read or write lock across the call.
 */
streams_inline streams_fastcall __hot int
bcanputnextany(queue_t *q)
{
	register int result;
	struct stdata *sd;

	dassert(q);
	sd = qstream(q);
	dassert(sd);

	prlock(sd);
	result = __bcanputany(q->q_next);
	prunlock(sd);

	return (result);
}

EXPORT_SYMBOL_NOVERS(bcanputnextany);	/* include/sys/streams/stream.h */

/*
 *  __find_qband:
 *
 * Find a queue band.  This must be called with the queue read or write locked.
 */
STATIC struct qband *
__find_qband(queue_t *q, unsigned char band)
{
	struct qband *qb;
	unsigned char q_nband;

	for (q_nband = q->q_nband, qb = q->q_bandp; qb && q_nband > band;
	     qb = qb->qb_next, --q_nband) ;
	return (qb);
}

/*
 *  __get_qband:
 *
 *  Find or create a queue band.  This must be called with the queue write locked.  This function is
 *  used by putq(9), putbq(9), insq(9), strqget(9) and strqset(9).  Unforntunately this function
 *  does not operate as described in the SVR 4 STREAMS Programmer's Guide: the SVR 4 SPG says that
 *  "[i]f a messages is passed to putq() with a b_band value that is greater than the number of
 *  qband structures associated with the queue, putq() tries to alloctate a new qband structure for
 *  each band up to and including the band of the message."  Also, SVR 4 SPG describs the q_nband
 *  member that holds the highest allocated qband structure according to this approach.
 *
 *  Unfortunately, the qbinfo structure is about 50 bytes (on 32-bit architecture), 64 bytes cache
 *  aligned, and allocating 255 of them would take 16,320 bytes or 4 kmem cache pages per queue!
 *  The technique would allow a module put procedure to examine the q_nband member and determine whether
 *  a putq() will be succesful, however, that is undocumented too.  So, we simply allocate the
 *  necessary qband structure and leave the rest unallocated.  This needs to be documented in
 *  putq(9), putbq(9), insq(9), strqset(9) and strqget(9).
 */
STATIC struct qband *
__get_qband(queue_t *q, unsigned char band)
{
	struct qband *qb;

	if (band <= q->q_nband) {
		qb = __find_qband(q, band);
		assert(qb);
	} else {
		do {
			if (!(qb = allocqb()))
				break;
			qb->qb_next = q->q_bandp;
			q->q_bandp = qb;
			qb->qb_hiwat = q->q_hiwat;
			qb->qb_lowat = q->q_lowat;
			++q->q_nband;
		} while (band > q->q_nband);
	}
	return (qb);
}

STATIC streams_noinline streams_fastcall __unlikely int
__bcanput_slow(queue_t *q, unsigned char band)
{
	unsigned long pl;

	pl = qrlock(q);
	if (likely(band <= q->q_nband && q->q_blocked > 0)) {
		struct qband *qb;

		if ((qb = __find_qband(q, band)) && test_bit(QB_FULL_BIT, &qb->qb_flag)) {
			set_bit(QB_WANTW_BIT, &qb->qb_flag);
			qrunlock(q, pl);
			return (0);
		}
	}
	/* Note: a non-existent band is considered empty */
	qrunlock(q, pl);
	return (1);
}

/*
 *  __bcanput:
 *
 *  A version without locks, called by bcanput() and bcanputnext() after locks taken.
 *
 *  Some confusion here.  UnixWare and other say when we hit the end of the logical stream bcanput
 *  returns 1.  Others (Solaris and the SVR 4 STREAMS Programmer's Guide) says that bcanput uses the
 *  queue and the end of the logical Stream.  It might be moot.  If a queue and the end of the
 *  Stream does not have a service procedure and never queues messages to the message queue with
 *  putq(9) then bcanput will always return 1.  SVR 4 SPG also says that any qi_putp(9) procedure
 *  that does putq(9) must have a qi_srvp(9) procedure.  
 *
 *  LOCKING: __bcanput() takes a queue read lock so that it can walk queue bands.
 *
 *  MP-STREAMS: Of course, because the locks are released before retuning, the result of the test
 *  can change before the result is used.  If the result is true (1) and the queue becomes full, we
 *  will pass an extra message: no problem.  If the result is false (0) and getq(9) on another
 *  processor back-enables and runs our service procedure before we call putq(9) no problem: putq(9)
 *  will enable the queue if necessary.  If it back-enables before we call putbq(9) then the service
 *  procedure will go for another run anyway.
 */
STATIC streams_inline streams_fastcall __hot int
__bcanput(queue_t *q, unsigned char band)
{
	unsigned long pl;
	queue_t *q_next;

	/* It might be an idea to cache the forward queue with a service procedure as Solaris does.
	   Solaris uses a q_nfsrv pointer for this that is adjusted when the driver is attached and
	   when modules are pushed or popped.  It's just so much trouble to go to... */

	/* find first queue with service procedure or no q_next pointer */
	for (q_next = q; !q->q_qinfo->qi_srvp && (q_next = q->q_next); q = q_next) ;

	if (likely(band == 0)) {
		pl = qrlock(q);
		if (likely(test_bit(QFULL_BIT, &q->q_flag) == 0)) {
			qrunlock(q, pl);
			return (1);
		}
		set_bit(QWANTW_BIT, &q->q_flag);
		qrunlock(q, pl);
		return (0);
	}
	return __bcanput_slow(q, band);
}

/**
 *  bcanput:		- check whether message of a given band can be put to a queue
 *  @q:			the queue to check
 *  @band:		the band to check
 *
 *  CONTEXT: Any.
 *
 *  NOTICES: The q or q->q_next pointer can be passed from a callout or syncrhonous callback for q.
 *  A driver can pass an upper multiplex read queue pointer or a lower mutliplex write queue
 *  pointer, provided that it guarantees the validity of @q across the call: that is that @q will
 *  neither be closed, nor unlinked.  The caller can pass any other @q whose validity it can
 *  guarantee across the call.
 *
 *  LOCKING: Takes a Stream head plumb read lock to permit this function to be called from outside a
 *  queue procedure belonging to @q and from process context.  The Stream head plumb read lock
 *  prevents any queue pair from either being inserted into or deleted from the stream while held
 *  allowing MP-safe walking of the Stream.
 *
 *  MP-STREAMS: bcanput() needs a Stream head plumb read lock to
 *  walk the stream as well as a queue read lock to walk the band structure.  Permitting this
 *  function to take these locks from an ISR or bottom-half would require that the Stream head plumb
 *  write locks and queue write locks suppress all interrupts, which is too strict for the most
 *  part.  One possible solution is to only suppress local interrupts for Stream head plumb write
 *  locks on Streams that contain driver read queues, and suppress local interrupts for driver queue
 *  pairs.  This would make all queue functions ISR safe on the bottommost queue pairs, but not on
 *  others.  put() and putq() would be okay, but putnext() would have to defer.  Another possibility
 *  is to upgrade write locks to be bottom-half locks, and only allows this function to be called
 *  from bottom-half and not hard interrupt.  But as most ISRs defer the bulk of their execution to
 *  bottom-half, blocking bottom-halves is almost as bad as suppressing hard interrupts.
 *
 *  In the end, I decided to suppress interrupts for Stream head plumb write locks and queue read
 *  and write locks, permitting almost all functions to be executed from an ISR.
 *
 *  canput() and bcanput() are really only intended on being called for a queue that is an upper mux
 *  read queue or lower mux write queue (i.e., Stream ends).  The call on a Stream end is only
 *  really useful when the queue has a service procedure, a fact that the driver designer can know.
 *  Therefore we don't take a plumb read lock and expect a service procedure and not to have to walk
 *  the Stream in __bcanput().  Unfortunately __bcanput() is shared by bcanputnext() or I would have
 *  put checks in __bcanput().
 *
 */
streams_fastcall int
bcanput(register queue_t *q, unsigned char band)
{
	register int result;
	struct stdata *sd;

	dassert(q);
	sd = qstream(q);

	dassert(sd);
	prlock(sd);

	result = __bcanput(q, band);

	prunlock(sd);
	return (result);
}

EXPORT_SYMBOL_NOVERS(bcanput);

/**
 *  bcanputnext: - check whether messages can be put to queue after this one
 *  @q:		this queue
 *  @band:	band to check
 *
 *  CONTEXT: Any.
 *
 *  NOTICES: The caller is responsible for ensuring that q->q_next is not NULL across the call.  A
 *  module can be sure that both its q->q_next pointers are non-NULL, a driver can be sure that its
 *  RD(q)->q_next pointer is non-NULL and that its WR(q)->q_next pointer is NULL, a Stream head
 *  which is not hanged up can be sure that its WR(q)->q_next pointer is non-NULL.
 *
 *  MP-STREAMS: If called from outside the STREAMS context, the caller is responsible for taking
 *  a Stream head read lock across the call.  (This is what the Stream head does.)  In general, this
 *  function should only be called from outside the STREAMS context by the Stream head.  Drivers
 *  should use bcanput().
 *
 *  CONTEXT: bcanputnext() can only be called from within a queue or module procedure, and must be
 *  passed a queue in that queue pair.
 *
 *  MP-STREAMS: The caller is responsible for the validity of the passed in q pointer.  A reference
 *  to a queue is generally valid from after qprocson(9) returns until qprocsoff(9) is called for q.
 *  Note that, when called from outside of the STREAMS context, the result might not reflect the
 *  state of the Stream.  From outside the STREAMS context, the caller an bracket freezestr(q) and
 *  unfreezestr(q) around the call.  The result will reflect the actual state of the Stream until
 *  unfreezestr(q) is called.
 *
 *  Again.
 *
 *  Solaris allows bcanputnext() to be called from an asyncrhonous context.  HP-UX does not.  For
 *  compatibility there is little choice but to make bcanputnext() safe from an asynchronous
 *  context by taking a plumb read lock.
 */
streams_fastcall __hot int
bcanputnext(register queue_t *q, unsigned char band)
{
	register int result;
	struct stdata *sd;

	dassert(q);
	sd = qstream(q);
	dassert(sd);
	prlock(sd);

	dassert(q);
	dassert(q->q_next);
	result = __bcanput(q->q_next, band);

	prunlock(sd);

	return (result);
}

EXPORT_SYMBOL_NOVERS(bcanputnext);

/**
 *  canenable:	- check whether service procedure will run
 *  @q:		queue to check
 */
__STRUTIL_EXTERN_INLINE int canenable(queue_t *q);

EXPORT_SYMBOL_NOVERS(canenable);

/**
 *  canget:	- check whether normal band zero (0) messages are on queue
 *  @q:		queue to check
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: None.
 */
__STRUTIL_EXTERN_INLINE int canget(queue_t *q);

EXPORT_SYMBOL_NOVERS(canget);		/* include/sys/streams/stream.h */

/**
 *  canput:		- check wheter message can be put to a queue
 *  @q:			the queue to check
 *
 *  Simply implemented as bcanput(q, 0).  See bcanput() for details.
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: None.
 */
__STRUTIL_EXTERN_INLINE int canput(queue_t *q);

EXPORT_SYMBOL_NOVERS(canput);		/* include/sys/streams/stream.h */

/**
 *  canputnext:		- check whether messages can be put to the queue after this one
 *  @q:			the queue whose next queue to check
 *
 *  Simply implemented as bcanputnext(q, 0).  See bcanputnext() for details.
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: Stream head read lock when called from !in_streams() context.
 */
__STRUTIL_EXTERN_INLINE int canputnext(register queue_t *q);

EXPORT_SYMBOL_NOVERS(canputnext);

/**
 *  freezestr:	- freeze a stream for direct queue access
 *  @q:		queue to freeze
 *
 *  Any function that wants to alter queue state (that is takes a write lock on the Stream head or a
 *  write lock on a queue in the stream), first takes a read lock on the Stream freeze lock.
 *  Because of this, taking a write lock on the Stream freeze lock probihits any other thread from
 *  putting message on or taking message off of any queue in the Stream.  This meets the purposes of
 *  STREAMS functions that need to be protected by freezestr() (insq(9), rmvq(9), appq(9),
 *  strqget(9), and strqset(9)).
 *
 *  The purpose of this function is only to protect the queue members and block put and service
 *  procedures from manipulating the queue so that rmvq and insq functions can be called.
 */
streams_fastcall __unlikely unsigned long
freezestr(queue_t *q)
{
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);

	return zwlock(sd);
}

EXPORT_SYMBOL_NOVERS(freezestr);

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
streams_fastcall __unlikely qi_qadmin_t
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

EXPORT_SYMBOL_NOVERS(getadmin);

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
streams_fastcall __unlikely modID_t
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

		_ctrace(sdev_put(cdev));
		return (modid);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(getmid);

/**
 *  OTHERQ:	- find the other queue in a queue pair
 *  @q:		one queue
 */
__STRUTIL_EXTERN_INLINE queue_t *OTHERQ(queue_t *q);

EXPORT_SYMBOL_NOVERS(OTHERQ);

/**
 *  qready:	- test if queue procedures are scheduled
 */
streams_fastcall __unlikely int
qready(void)
{
	struct strthread *t = this_thread;

	return (test_bit(qrunflag, &t->flags) != 0);
}

EXPORT_SYMBOL_NOVERS(qready);		/* include/sys/streams/stream.h */

/**
 *  setqsched:	- schedule execution of queue procedures
 */
streams_inline streams_fastcall void
setqsched(void)
{
	struct strthread *t = this_thread;

	if (!test_and_set_bit(qrunflag, &t->flags))
		__raise_streams();
}

EXPORT_SYMBOL_NOVERS(setqsched);	/* include/sys/streams/stream.h */

/**
 *  qschedule:	- schedule a queue for service
 *  @q:		queue to schedule for service
 *
 */
STATIC streams_inline streams_fastcall void
qschedule(queue_t *q)
{
	if (!test_and_set_bit(QENAB_BIT, &q->q_flag)) {
		struct strthread *t = this_thread;

		/* put ourselves on the run list */
		prefetchw(t);
		q->q_link = NULL;
		{
			unsigned long flags;

			local_irq_save(flags);
			*XCHG(&t->qtail, &q->q_link) = qget(q);
			local_irq_restore(flags);
		}
		setqsched();
	}
}

/**
 *  qenable:	- schedule a queue for execution
 *  @q:		queue to schedule for service
 *
 *  Another name for qschedule(9), qenable() schedules a queue for service regardless of the setting
 *  of the %QNOENB_BIT, but has to check for the existence of a service procedure.
 */
streams_fastcall void
qenable(register queue_t *q)
{
	if (likely(q->q_qinfo->qi_srvp != NULL))
		qschedule(q);
}

EXPORT_SYMBOL_NOVERS(qenable);		/* include/sys/streams/stream.h */

/**
 *  enableq:	- enable a queue service procedure
 *  @q:		queue for which service procedure is to be enabled
 *
 *  Schedule a queue's service procedure for execution.  enableq() only schedules a queue service
 *  procedure if a service procedure exists for @q, and if the queue has not been previously
 *  noenabled with noenable() (i.e. the %QNOENB flag is set on the queue).
 */
streams_fastcall int
enableq(queue_t *q)
{
	if (likely(q->q_qinfo->qi_srvp && !test_bit(QNOENB_BIT, &q->q_flag))) {
		qenable(q);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(enableq);		/* include/sys/streams/stream.h */

/**
 *  enableok:	- permit scheduling of a queue service procedure
 *  @q:		queue to permit service procedure scheduling
 *
 *  This function simply clears the %QNOENB flag on the queue.  It does not schedule the queue.
 *  That must be done with a separate call to enableq() or qenable().  It is not supposed to be
 *  called by a thread that froze the Stream with freezestr(9); but, it will still work.
 */
streams_fastcall __unlikely void
enableok(queue_t *q)
{
	struct stdata *sd;
	unsigned long pl;

	assert(q);
	assure(not_frozen_by_caller(q));

	sd = qstream(q);
	assert(sd);

	/* block on frozen stream unless stream frozen by caller */
	pl = zrlock(sd);
	clear_bit(QNOENB_BIT, &q->q_flag);
	zrunlock(sd, pl);
}

EXPORT_SYMBOL_NOVERS(enableok);	/* include/sys/streams/stream.h */

/**
 *  noenable:	- defer scheduling of a queue service procedure
 *  @q:		queue to defer service procedure scheduling
 *
 *  This function simply sets the %QNOENB flag on the queue.  It is not supposed to be called by a
 *  thread that froze the Stream with freezestr(9); but, it will still work.
 */
streams_fastcall __unlikely void
noenable(queue_t *q)
{
	struct stdata *sd;
	unsigned long pl;

	assert(q);
	assure(not_frozen_by_caller(q));

	sd = qstream(q);
	assert(sd);

	/* block on frozen stream unless stream frozen by caller */
	pl = zrlock(sd);
	set_bit(QNOENB_BIT, &q->q_flag);
	zrunlock(sd, pl);
}

EXPORT_SYMBOL_NOVERS(noenable);	/* include/sys/streams/stream.h */

/*
 *  __putbq:
 */
STATIC streams_fastcall int
__putbq(queue_t *q, mblk_t *mp)
{ /* IRQ DISABLED */
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
			enable = test_bit(QWANTR_BIT, &q->q_flag) ? 1 : 0;
		      hipri:
			if ((q->q_count += msgsize(mp)) > q->q_hiwat)
				set_bit(QFULL_BIT, &q->q_flag);
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
			return (1 + enable);
		} else {
			struct qband *qb;

			if (unlikely((qb = __get_qband(q, mp->b_band)) == NULL))
				return (0);
			enable = (test_bit(QWANTR_BIT, &q->q_flag)) ? 1 : 0;
			if (qb->qb_last == b_prev || qb->qb_last == NULL)
				qb->qb_last = mp;
			if (qb->qb_first == b_next || qb->qb_first == NULL)
				qb->qb_first = mp;
			assert(qb->qb_first != NULL);
			assert(qb->qb_last != NULL);
			qb->qb_msgs++;
			if ((qb->qb_count += msgsize(mp)) > qb->qb_hiwat)
				if (!test_and_set_bit(QB_FULL_BIT, &qb->qb_flag))
					++q->q_blocked;
			goto banded;
		}
	} else {
		b_prev = NULL;
		b_next = q->q_first;
		/* modules should never put priority messages back on a queue */
		enable = 2;	/* and this is why */
		/* SVR 4 SPG says to zero b_band when hipri messages placed on queue */
		mp->b_band = 0;
		goto hipri;
	}
}

STATIC streams_noinline streams_fastcall int
putbq_result(queue_t *q, const int result)
{
	switch (result) {
	case 3:		/* success - high priority enable */
		qenable(q);
		break;
	case 2:		/* success - enable if not noenabled */
		enableq(q);
		break;
	case 1:		/* success - don't enable */
		break;
	default:
		assert(result == 0);
	case 0:		/* failure */
		assert(0);
		/* This should never happen, because it takes a qband structure allocation failure
		   to get here, and since we are putting the message back on the queue, there
		   should already be a qband structure. Unless, however, putbq() is just used to
		   insert messages ahead of others rather than really putting them back.
		   Nevertheless, a way to avoid this error is to always ensure that a qband
		   structure exists (e.g., with strqset) before calling putbq on a band for the
		   first time. */
		return (0);
	}
	return (1);
}

/**
 *  putbq:	- put a message back on a queue
 *  @q:		queue to place back message
 *  @mp:	message to place back
 */
streams_fastcall int
putbq(register queue_t *q, register mblk_t *mp)
{
	int result;
	unsigned long pl;

	assert(q);
	assert(mp);

	assure(not_frozen_by_caller(q));

	pl = qwlock(q);
	result = __putbq(q, mp);
	qwunlock(q, pl);
	if (likely(result == 1))
		return (1);
	return putbq_result(q, result);
}

EXPORT_SYMBOL_NOVERS(putbq);

/**
 *  putctl:	- put a control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 */
streams_fastcall int
putctl(queue_t *q, int type)
{
	mblk_t *mp;

	assert(q);
	if (ctlmsg(type) && (mp = allocb(0, BPRI_HI))) {
		mp->b_datap->db_type = type;
		put(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(putctl);

/**
 *  putctl1:	- put a 1-byte control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 *  @param:	the 1 byte parameter
 */
streams_fastcall int
putctl1(queue_t *q, int type, int param)
{
	mblk_t *mp;

	assert(q);
	if (ctlmsg(type) && (mp = allocb(1, BPRI_HI))) {
		mp->b_datap->db_type = type;
		mp->b_wptr[0] = (unsigned char) param;
		mp->b_wptr++;
		put(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(putctl1);

/**
 *  putctl2:	- put a 2-byte control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 *  @param1:	the first 1 byte parameter
 *  @param2:	the second 1 byte parameter
 */
streams_fastcall int
putctl2(queue_t *q, int type, int param1, int param2)
{
	mblk_t *mp;

	assert(q);
	if (ctlmsg(type) && (mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = type;
		mp->b_wptr[0] = (unsigned char) param1;
		mp->b_wptr++;
		mp->b_wptr[0] = (unsigned char) param2;
		mp->b_wptr++;
		put(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(putctl2);

/**
 *  putnextctl:	- put a control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 */
streams_fastcall int
putnextctl(queue_t *q, int type)
{
	mblk_t *mp;

	assert(q);
	assert(q->q_next);
	if (!datamsg(type) && (mp = allocb(0, BPRI_HI))) {
		mp->b_datap->db_type = type;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(putnextctl);

/**
 *  putnextctl1: - put a 1-byte control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 *  @param:	the 1 byte parameter
 */
streams_fastcall int
putnextctl1(queue_t *q, int type, int param)
{
	mblk_t *mp;

	assert(q);
	assert(q->q_next);
	if (ctlmsg(type) && (mp = allocb(1, BPRI_HI))) {
		mp->b_datap->db_type = type;
		mp->b_wptr[0] = (unsigned char) param;
		mp->b_wptr++;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(putnextctl1);

/**
 *  putnextctl2: - put a 2-byte control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 *  @param1:	the first 1 byte parameter
 *  @param2:	the second 1 byte parameter
 */
streams_fastcall int
putnextctl2(queue_t *q, int type, int param1, int param2)
{
	mblk_t *mp;

	assert(q);
	assert(q->q_next);
	if (ctlmsg(type) && (mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = type;
		mp->b_wptr[0] = (unsigned char) param1;
		mp->b_wptr++;
		mp->b_wptr[0] = (unsigned char) param2;
		mp->b_wptr++;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(putnextctl2);

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
STATIC streams_inline streams_fastcall __hot int
__putq(queue_t *q, mblk_t *mp)
{
	int enable;
	mblk_t *b_prev, *b_next;

	/* fast path for normal messages */
	if (likely(mp->b_datap->db_type < QPCTL) && likely(mp->b_band == 0)) {
		b_prev = q->q_last;
		b_next = NULL;
		/* enable if will be first message in queue, or requested by getq() */
		enable = ((q->q_first == b_next)
			  || test_bit(QWANTR_BIT, &q->q_flag)) ? 1 : 0;
	      hipri:
		if (unlikely((q->q_count += msgsize(mp)) > q->q_hiwat))
			set_bit(QFULL_BIT, &q->q_flag);
	      banded:
		if (likely(q->q_last == b_prev))
			q->q_last = mp;
		if (unlikely(q->q_first == b_next))
			q->q_first = mp;
		q->q_msgs++;
		if (unlikely((mp->b_next = b_next) != NULL))
			b_next->b_prev = mp;
		if (likely((mp->b_prev = b_prev) != NULL))
			b_prev->b_next = mp;
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
		enable = 2;	/* always enable on high priority */
		/* SVR 4 SPG says to zero b_band when hipri messages placed on queue */
		mp->b_band = 0;
		goto hipri;
	} else {
		struct qband *qb;

		if (unlikely((qb = __get_qband(q, mp->b_band)) == NULL))
			return (0);
		/* find position for our message */
		while (unlikely(b_next && b_next->b_band >= mp->b_band)) {
			b_prev = b_next;
			b_next = b_prev->b_next;
		}
		/* enable if will be first message in queue, or requested by getq() */
		enable = ((q->q_first == b_next)
			  || test_bit(QWANTR_BIT, &q->q_flag)) ? 1 : 0;
		if (likely(qb->qb_last == b_prev || qb->qb_last == NULL))
			qb->qb_last = mp;
		if (unlikely(qb->qb_first == b_next || qb->qb_first == NULL))
			qb->qb_first = mp;
		assert(qb->qb_first != NULL);
		assert(qb->qb_last != NULL);
		qb->qb_msgs++;
		if ((qb->qb_count += msgsize(mp)) > qb->qb_hiwat)
			if (!test_and_set_bit(QB_FULL_BIT, &qb->qb_flag))
				++q->q_blocked;
		goto banded;
	}
}

STATIC streams_noinline streams_fastcall int
putq_result(queue_t *q, mblk_t *mp, const int result)
{
	switch (result) {
	case 3:		/* success - high priority enable */
		qenable(q);
		break;
	case 2:		/* success - enable if not noenabled */
		enableq(q);
		break;
	case 1:		/* success - don't enable */
		break;
	default:
		assert(result == 0);
	case 0:		/* failure */
		assert(mp->b_band != 0);
		/* This can happen and it is bad.  We use the return value to putq but it is
		   typically ignored by the module.  One way to ensure that this never happens is
		   to call strqset() for the band before calling putq on the band for the first
		   time. (See also putbq()) */
		return (0);
	}
	return (1);
}

/**
 *  putq:	- put a message block to a queue
 *  @q:		queue to which to put the message
 *  @mp:	message to put
 *
 *  CONTEXT: Any.  It is safe to call this function directly from an ISR to place messages on a
 *  driver's lowest read queue.  Should not be frozen by the caller.
 */
streams_fastcall __hot int
putq(register queue_t *q, register mblk_t *mp)
{
	register int result;
	unsigned long pl;

	assert(q);
	assert(mp);

	assure(not_frozen_by_caller(q));

	pl = qwlock(q);
	result = __putq(q, mp);
	qwunlock(q, pl);
	if (likely(result == 1))
		return (1);
	return putq_result(q, mp, result);
}

EXPORT_SYMBOL_NOVERS(putq);

/*
 *  __insq:
 */
STATIC streams_fastcall int
__insq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	int enable = 0;
	struct qband *qb = NULL;
	size_t size;

	if (likely(emp == NULL))
		goto putq;
	/* ingnore message class for insq() */
	enable = ((q->q_first == emp) || test_bit(QWANTR_BIT, &q->q_flag)) ? 1 : 0;
	/* insert before emp */
	if (nmp->b_datap->db_type >= QPCTL) {
		if (emp->b_prev && emp->b_prev->b_datap->db_type < QPCTL)
			goto out_of_order;
		/* SVR 4 SPG says to zero b_band when hipri messages placed on queue */
		nmp->b_band = 0;
	} else {
		if (emp->b_datap->db_type >= QPCTL || emp->b_band < nmp->b_band)
			goto out_of_order;
		if (emp->b_prev && emp->b_prev->b_datap->db_type < QPCTL
		    && emp->b_prev->b_band > nmp->b_band)
			goto out_of_order;
		if (unlikely(nmp->b_band)) {
			if (!(qb = __get_qband(q, nmp->b_band)))
				goto enomem;
			enable = ((q->q_first == emp)
				  || test_bit(QWANTR_BIT, &q->q_flag)) ? 1 : 0;
			if (qb->qb_last == emp || qb->qb_last == NULL)
				qb->qb_last = nmp;
			if (qb->qb_first == emp->b_next || qb->qb_first == NULL)
				qb->qb_first = nmp;
			dassert(qb->qb_first != NULL);
			dassert(qb->qb_last != NULL);
		}
	}
	if (q->q_first == emp)
		q->q_first = nmp;
	if ((nmp->b_prev = emp->b_prev))
		nmp->b_prev->b_next = nmp;
	nmp->b_next = emp;
	emp->b_prev = nmp;
	/* some adding to do */
	q->q_msgs++;
	size = msgsize(nmp);
	if (!qb) {
		if ((q->q_count += size) > q->q_hiwat)
			set_bit(QFULL_BIT, &q->q_flag);
	} else {
		qb->qb_msgs++;
		if ((qb->qb_count += size) > qb->qb_hiwat) {
			if (!test_and_set_bit(QB_FULL_BIT, &qb->qb_flag))
				++q->q_blocked;
		}
	}
	return (1 + enable);	/* success */
#if 0
      bug:
#endif
      enomem:
	/* couldn't allocate a band structure! */
	goto failure;
      out_of_order:
	/* insertion would misorder the queue */
	goto failure;
      failure:
	return (0);		/* failure */
      putq:
	{
		int be;

		/* insert at end */
		if ((be = __putq(q, nmp)) == 3)
			be = 2;
		return (be);
	}
}

STATIC streams_noinline streams_fastcall int
insq_result(queue_t *q, const int result)
{
	switch (result) {
	case 3:		/* success - high priority enable */
		assert(0);
		qenable(q);
		break;
	case 2:		/* success - enable if not noenabled */
		enableq(q);
		break;
	case 1:		/* success - don't enable */
		break;
	default:
		never();
	case 0:		/* failure */
		return (0);
	}
	return (1);
}

/**
 *  insq:	- insert a message before another on a queue
 *  @q:		the queue into which to insert
 *  @emp:	the existing message before which to insert
 *  @nmp:	the new message to insert
 *
 *  CONTEXT: Any, but frozen by the caller.
 *
 *  LOCKING: The caller must lock the queue with MPSTR_QLOCK() or freezestr() across the call.
 */
streams_fastcall int
insq(register queue_t *q, register mblk_t *emp, register mblk_t *nmp)
{
	register int result;
	unsigned long pl;

	assert(q);
	assert(nmp);

	assure(frozen_by_caller(q));

	pl = qwlock(q);
	result = __insq(q, emp, nmp);
	qwunlock(q, pl);
	if (likely(result == 1))
		return (1);
	return insq_result(q, result);
}

EXPORT_SYMBOL_NOVERS(insq);

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
 *  MP-STREAMS: The Stream needs to be frozen by the caller with freezestr() or the call will fail
 *  under assertions.
 */
streams_fastcall __unlikely int
appq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	register int result;
	unsigned long pl;

	assert(q);
	assert(nmp);

	assure(frozen_by_caller(q));

	pl = qwlock(q);
	result = __insq(q, emp ? emp->b_next : emp, nmp);
	qwunlock(q, pl);
	/* do enabling outside the locks */
	if (likely(result == 1))
		return (1);
	return insq_result(q, result);
}

EXPORT_SYMBOL_NOVERS(appq);

STATIC int __setsq(queue_t *q, struct fmodsw *fmod);
STATIC void __setq(queue_t *q, struct qinit *rinit, struct qinit *winit);

/**
 *  qalloc: - allocate and initialize a queue pair
 *  @sd:	Stream head to which the queue pair belongs
 *  @fmod:	STREAMS module to which the queue pair belongs
 *
 *  Allocates and initializes a queue pair for use by STREAMS.
 */
STATIC streams_fastcall __unlikely queue_t *
qalloc(struct stdata *sd, struct fmodsw *fmod)
{
	queue_t *q;

	if ((q = allocq())) {
		/* start life qprocsoff() */
		(q + 0)->q_flag |= QPROCS | QNOENB;
		(q + 1)->q_flag |= QPROCS | QNOENB;
		if (!__setsq(q, fmod)) {
			struct streamtab *st = fmod->f_str;
			struct queinfo *qu = (typeof(qu)) q;

			__setq(q, st->st_rdinit, st->st_wrinit);
			_ctrace(qu->qu_str = sd_get(sd));
		} else {
			(q + 0)->q_flag = QUSE | QREADR;
			(q + 1)->q_flag = QUSE;
			_ctrace(qput(&q));
			assert(q == NULL);
		}
	}
	return (q);
}

int streams_fastcall setsq(queue_t *q, struct fmodsw *fmod);
void streams_fastcall setq(queue_t *q, struct qinit *rinit, struct qinit *winit);

/**
 *  qattach: - attach a stream head, module or driver queue pair to a stream head
 *  @sd:	stream head data structure identifying stream
 *  @fmod:	&struct fmodsw pointer identifying module or driver
 *  @devp:	&dev_t pointer providing opening device number
 *  @oflag:	open flags
 *  @sflag:	streams flag, can be %DRVOPEN, %CLONEOPEN, %MODOPEN
 *  @crp:	&cred_t pointer to credentials of opening task
 *
 *  qattach() allocates a new queue pair, calls qinsert() to half-insert the queue pair into the
 *  Stream and then calls qopen() to call the qi_qopen() procedure of the module or driver.
 *
 *  CONTEXT:  Must only be called from stream head or qopen()/qclose() procedures.
 *
 *  LOCKING:  Must be called with no locks held. The call to qalloc() might sleep.
 *
 *  NOTICES: "Magic Garden" says that if we are opening and the major device number returned from
 *  qopen() is not the same as the major number passed, that we need to do a setq on the queue from
 *  the streamtab associated with the new major device number.
 */
streams_fastcall __unlikely int
qattach(struct stdata *sd, struct fmodsw *fmod, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct streamtab *st;
	queue_t *q;
	dev_t dev;
	struct cdevsw *cdev;
	int err;

	err = -ENOMEM;
	if (!(q = qalloc(sd, fmod)))
		goto error;

	qinsert(sd, q);		/* half insert under stream head */
	dev = *devp;		/* remember calling device number */
	if ((err = qopen(q, &dev, oflag, sflag, crp))) {
		err = err > 0 ? -err : err;
		goto qerror;
	}
	/* module is supposed to ignore devp */
	if (sflag != MODOPEN) {
		if (dev != *devp && getmajor(dev) != getmajor(*devp)) {
			err = -ENOENT;
			if (!(cdev = cdrv_get(getmajor(*devp))))
				goto enoent;
			if ((struct fmodsw *) cdev != fmod) {
				if (!(st = cdev->d_str))
					goto put_noent;
				if ((err = setsq(q, (struct fmodsw *) cdev)) < 0)
					goto put_noent;
				setq(q, st->st_rdinit, st->st_wrinit);
			}
			_ctrace(cdrv_put(cdev));
			err = 0;
		}
		*devp = dev;
	}
	qprocson(q);		/* in case qopen() forgot */
	return (0);
      put_noent:
	_ctrace(cdrv_put(cdev));
      enoent:
	qclose(q, oflag, crp);	/* need to call close */
      qerror:
	qprocsoff(q);		/* in case qopen called qprocson, yet returned an error */
	qdelete(q);		/* half delete */
      error:
	return (err);
}

EXPORT_SYMBOL_NOVERS(qattach);

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
 *  NOTICES: rq should have already been removed from a queue with qprocsoff() (but check again
 *  anyway) and must be a valid pointer or bad things will happen.
 *
 *  Don't do gets and puts on the Stream head when adding or removing queue pairs from the stream
 *  because the Stream head reference count falling to zero is used to deallocate the Stream head
 *  queue pair.
 */
streams_fastcall __unlikely void
qdelete(queue_t *q)
{
	struct stdata *sd;
	unsigned long pl;

	assert(q);
	sd = qstream(q);
	assert(sd);

	_ptrace(("final half-delete of stream %p queue pair %p\n", sd, q));

	pl = pwlock(sd);

	(q + 0)->q_next = NULL;
	(q + 1)->q_next = NULL;

	pwunlock(sd, pl);

	_printd(("%s: cancelling initial allocation reference queue pair %p\n", __FUNCTION__, q));
	_ctrace(qput(&q));	/* cancel initial allocation reference */
}

EXPORT_SYMBOL_NOVERS(qdelete);

/**
 *  qdetach:	- detach a queue pair from a stream
 *  @q:		read queue pointer of queue pair to detach
 *  @flags:	open flags of closing task
 *  @crp:	credentials of closing task
 *
 *  qdetach() calls the module queue pair qi_qclose procedure and then removes the queue pair from
 *  the stream.
 *
 *  It is the responsibility of the module qi_qclose() procedure to call qprocsoff() before
 *  returning; however, many modules do not, so we use the QPROCS flag in the queue pairs to
 *  determine whether a qprocsoff() has been called.  qprocsoff() half-deletes the queue pair from
 *  the Stream under Stream head write lock.  The call to qdelete() completes this operation and
 *  destroys the queue pair.
 *
 *  Return: qdetach() returns any error returned by the module's qi_qclose procedure.
 *
 *  Errors: qdetach() can return any error returned by the module's qi_qclose procedure.  This error
 *  is not returned to the user.
 *
 *  Context: qdetach() is meant to be called in user context.
 *
 *  Locking; qdetach() is called with no locks held.  In particular the qclose() procedure needs to
 *  be called with no locks held so that the procedure may sleep.
 *
 */
streams_fastcall __unlikely int
qdetach(queue_t *q, int flags, cred_t *crp)
{
	int err;

	assert(q);

	_ptrace(("detaching stream %p queue pair %p\n", qstream(q), q));

	err = _ctrace(qclose(q, flags, crp));
	_ctrace(qprocsoff(q));	/* in case qclose forgot */
	_ctrace(qdelete(q));	/* half delete */
	return (err);
}

EXPORT_SYMBOL_NOVERS(qdetach);

/**
 *  qinsert:	- insert a queue pair below another in a stream
 *  @sd:	stream head under which to insert
 *  @irq:	read queue of queue pair to insert
 *
 *  DESCRIPTION: qinsert() half-inserts the queue pair identified by @irq beneath the queue pair on
 *  the stream identified by @sd.  This is only a half-insert.  The q->q_next pointers of the queue
 *  pair to be inserted, @irq, are adjusted, but the stream remains unaffected.  qprocson() must be
 *  called on @irq to complete the insertion and properly set flags.
 *
 *  CONTEXT: qinsert() is only meant to be called from the qattach() procedure or a stream head open
 *  procedure.
 *
 *  NOTICES: @irq should not already be inserted on a queue or bad things will happen.  @sd must
 *  already have its initial queue pair attached or bad things will happen.
 *
 *  LOCKING:  qinsert() is called with no locks held.  The function takes a read lock on the Stream
 *  head to protect queue pointers from weldq() and unweldq() and other operations that manipulate
 *  queue pointers outside of the qattach() and qdetach() procedures called when a queue is opened
 *  or closed.  Only one qinsert() can occur at a time for a given queue, because the Stream head
 *  holds the STWOPEN bit accross the call.
 *
 *  Because this is used to insert modules under the stream head, a driver can only be permitted to
 *  modify its downward queue pointer (WR(q)->q_next), and then only from its qi_qopen() procedure
 *  before qprocson() is called, from its qi_qclose() procedure after qprocsoff() is called, or
 *  using weldq() or unweldq().
 */
streams_fastcall __unlikely void
qinsert(struct stdata *sd, queue_t *irq)
{
	queue_t *iwq, *srq, *swq;

	_ptrace(("initial  half-insert of stream %p queue pair %p\n", sd, irq));

	prlock(sd);
	srq = sd->sd_rq;
	iwq = OTHERQ(irq);
	swq = OTHERQ(srq);
	irq->q_next = srq;
	iwq->q_next = (swq->q_next != srq) ? swq->q_next : irq;
	prunlock(sd);
}

EXPORT_SYMBOL_NOVERS(qinsert);

/**
 *  qprocsoff:	- turn off qi_putp and qi_srvp procedures for a queue pair
 *  @q:		read queue pointer for the queue pair to turn procs off
 *
 *  qprocsoff() marks the queue pair as being owned by the Stream head (disabling further put
 *  procedures), marks it as being noenabled, (which disables further srv procedures), and bypasses
 *  the module by adjusting the q->q_next pointers of upstream modules for each queue in the queue
 *  pair.  This effectively bypasses the module.
 *
 *  Context: qprocsoff() should only be called from qattach() or a Stream head head open procedure.
 *  The user should call qprocsoff() from the qclose() procedure before returning.
 *
 *  Notices: qprocsoff() does not fully delete the queue pair from the Stream.  It is still
 *  half-attached.  Use qdelete() to complete the final removal of the queue pair from the Stream.
 *
 *  Cache the packet sizes of the queue below the Stream head in the sd_minpsz and sd_maxpsz member.
 *  This saves a little pointer dereferencing in the Stream head later.
 *
 *  Locking: The modules's qclose() procedure is called with no Stream head locks held.  Before
 *  unlinking the queue pair, qprocsoff() takes a write lock on the Stream head.  This means that
 *  all queue synchronous procedures must exit before the lock is acquired.  The Stream head is
 *  holding the STRCLOSE bit, so no other close can occur, and all other operations on the Stream
 *  will fail.
 */
streams_fastcall __unlikely void
qprocsoff(queue_t *q)
{
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);

#if 0
	assert(current_context() <= CTX_STREAMS);
#endif
	/* only one qprocsoff() happens at a time */
	if (!test_bit(QPROCS_BIT, &rq->q_flag)) {
		struct queinfo *qu = ((typeof(qu)) rq);
		struct stdata *sd = qu->qu_str;
		unsigned long pl;

		assert(sd);

		/* spin here waiting for queue procedures to exit */
		pl = pwlock(sd);

		set_bit(QPROCS_BIT, &rq->q_flag);
		set_bit(QPROCS_BIT, &wq->q_flag);
		/* disable queue enabling */
		set_bit(QNOENB_BIT, &rq->q_flag);
		set_bit(QNOENB_BIT, &wq->q_flag);
		/* cancel queue service calls */
		clear_bit(QENAB_BIT, &rq->q_flag);
		clear_bit(QENAB_BIT, &wq->q_flag);
		/* clear queue putq enable bit */
		clear_bit(QWANTR_BIT, &rq->q_flag);
		clear_bit(QWANTR_BIT, &wq->q_flag);
		/* clear queue back enable bit */
		clear_bit(QWANTW_BIT, &rq->q_flag);
		clear_bit(QWANTW_BIT, &wq->q_flag);
		{
			struct qband *qb;

			for (qb = rq->q_bandp; qb; qb = qb->qb_next)
				clear_bit(QB_WANTW_BIT, &qb->qb_flag);
			for (qb = wq->q_bandp; qb; qb = qb->qb_next)
				clear_bit(QB_WANTW_BIT, &qb->qb_flag);
		}

		_ptrace(("initial half-delete of stream %p queue pair %p\n", sd, q));

		/* bypass this module: works for pipe, FIFO and other Stream heads queues too */
		if ((bq = backq(rq)))
			bq->q_next = rq->q_next;
		if ((bq = backq(wq)))
			bq->q_next = wq->q_next;
		/* cache new packet sizes (next module or stream head) */
		if ((wq = sd->sd_wq->q_next) || (wq = sd->sd_wq)) {
			sd->sd_minpsz = wq->q_minpsz;
			sd->sd_maxpsz = wq->q_maxpsz;
		}

		pwunlock(sd, pl);

		/* XXX: put procs must check QPROCS bit after acquiring prlock */
		/* XXX: srv procs must check QPROCS bit after acquiring prlock */
	}
}

EXPORT_SYMBOL_NOVERS(qprocsoff);

/**
 *  qprocson:	- trun on qi_putp and qi_srvp procedure for a queeu pair
 *  @q:		read queue pointer for the queue pair to turn procs on
 *
 *  qprocson() marks the queue pair as being not owned by the Stream head (enabling put procedures),
 *  marks it as being enabled (enabling srv procedures), and intalls the module by adjusting the
 *  q->q_next pointers of the upstream modules for each queue in the queue pair.  This effectively
 *  undoes the bypass created by qprocsoff().
 *
 *  Context: qprocson() should only be called from qattach(), qdetach() or a Stream head open
 *  procedure.  The user should call qprocson() from the qopen() procedure before returning.
 *
 *  Notices: qprocson() fully inserts the queue pair into the Stream.  It must be half-inserted with
 *  qinsert() before qprocson() can be called.
 *
 *  Cache the packet sizes of the queue below the Stream head in the sd_minpsz and sd_maxpsz member.
 *  This saves a little pointer dereferencing in the Stream head later.
 *
 *  Locking:  The module's qopen() procedure is called with no Stream head locks held.  Before
 *  linking the queue pair in, qprocson() takes a write lock on the Stream head.  This means that
 *  all queue synchronous procedures must exit before the lock is acquired.  The Stream head is
 *  holding STWOPEN bit, so no other open can occur.  Because the Stream head has not yet been
 *  published to a file pointer or inode, no other operation can occur on the Stream.
 */
streams_fastcall __unlikely void
qprocson(queue_t *q)
{
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);

#if 0
	assert(current_context() <= CTX_STREAMS);
#endif
	/* only one qprocson() happens at a time */
	if (test_bit(QPROCS_BIT, &rq->q_flag)) {
		struct queinfo *qu = ((typeof(qu)) rq);
		struct stdata *sd = qu->qu_str;
		unsigned long pl;

		/* spin here waiting for queue procedures to exit */
		pl = pwlock(sd);

		clear_bit(QPROCS_BIT, &rq->q_flag);
		clear_bit(QPROCS_BIT, &wq->q_flag);
		/* allow queues to be enabled */
		clear_bit(QNOENB_BIT, &rq->q_flag);
		clear_bit(QNOENB_BIT, &wq->q_flag);
		/* schedule service procedure on first message */
		set_bit(QWANTR_BIT, &rq->q_flag);
		set_bit(QWANTR_BIT, &wq->q_flag);

		/* join this module: works for FIFOs and PIPEs too */
		if ((bq = backq(rq)))
			bq->q_next = rq;
		if ((bq = backq(wq)))
			bq->q_next = wq;

		/* cache new packet sizes (this module) */
		sd->sd_minpsz = wq->q_minpsz;
		sd->sd_maxpsz = wq->q_maxpsz;

		pwunlock(sd, pl);
	}
}

EXPORT_SYMBOL_NOVERS(qprocson);

/**
 *  qreply:	- reply with a message
 *  @q:		queue from which to reply
 *  @mp:	message reply
 */
__STRUTIL_EXTERN_INLINE void qreply(register queue_t *q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(qreply);

/**
 *  qsize:	- calculate number of messages on a queue
 *  @q:		queue to count messages
 */
__STRUTIL_EXTERN_INLINE ssize_t qsize(register queue_t *q);

EXPORT_SYMBOL_NOVERS(qsize);

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
 *  LOCKING:  Take a Stream head plumb read lock to protect q_next pointers while walking the
 *  Stream.
 */
streams_fastcall __unlikely ssize_t
qcountstrm(queue_t *q)
{
	ssize_t count = 0;

	if (q) {
		struct stdata *sd;

		sd = qstream(q);
		assert(sd);
		prlock(sd);

		for (; q && SAMESTR(q); q = q->q_next)
			count += q->q_count;

		prunlock(sd);
	}
	return (count);
}

EXPORT_SYMBOL_NOVERS(qcountstrm);

/**
 *  RD:		- find read queue from write queu
 *  @q:		write queue pointer
 */
__STRUTIL_EXTERN_INLINE queue_t *RD(queue_t *q);

EXPORT_SYMBOL_NOVERS(RD);

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
STATIC streams_noinline streams_fastcall __hot_read bool
__rmvq(queue_t *q, mblk_t *mp)
{
	bool backenable = false;
	mblk_t *b_next, *b_prev;

	assert(q);
	assert(mp);

	b_prev = mp->b_prev;	/* NULL for getq */
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
	assert(q->q_msgs >= 0);
	if (likely(mp->b_band == 0)) {
		q->q_count -= msgsize(mp);
		assert(q->q_count >= 0);
		if (q->q_count == 0) {
			clear_bit(QFULL_BIT, &q->q_flag);
			clear_bit(QWANTW_BIT, &q->q_flag);
			backenable = true;
		} else if (q->q_count < q->q_lowat) {
			clear_bit(QFULL_BIT, &q->q_flag);
			if (test_and_clear_bit(QWANTW_BIT, &q->q_flag))
				backenable = true;
		}
		/* no longer want to read band zero */
		clear_bit(QWANTR_BIT, &q->q_flag);
	} else {
		struct qband *qb;

		{
			unsigned char q_nband, band;

			for (band = mp->b_band, q_nband = q->q_nband, qb = q->q_bandp;
			     qb && q_nband > band; qb = qb->qb_next, --q_nband) ;
		}
		assert(qb);
		if (qb->qb_first == mp && qb->qb_last == mp)
			qb->qb_first = qb->qb_last = NULL;
		else {
			if (qb->qb_first == mp)
				qb->qb_first = b_next;
			if (qb->qb_last == mp)
				qb->qb_last = b_prev;
		}
		qb->qb_msgs--;
		assert(qb->qb_msgs >= 0);
		qb->qb_count -= msgsize(mp);
		assert(qb->qb_count >= 0);
		if (qb->qb_count == 0 || qb->qb_count < qb->qb_lowat) {
			if (test_and_clear_bit(QB_FULL_BIT, &qb->qb_flag))
				q->q_blocked--;
			if (test_and_clear_bit(QB_WANTW_BIT, &qb->qb_flag))
				backenable = true;
		}
		/* no longer want to read */
		clear_bit(QWANTR_BIT, &q->q_flag);
	}
	return (backenable);
}

/**
 *  rmvq:	- remove a messge from a queue
 *  @q:		queue from which to remove message
 *  @mp:	message to remove
 *
 *  CONTEXT: rmvq() can be called from any context.  rmvq() must be called with the queue write
 *  locked (e.g. using freezestr(9) or MPSTR_QLOCK(9)), or some other mutual exclusion mechanism.
 *
 *  MP-STREAMS: Note that qbackenable() will take its own Stream head read lock for Stream ends,
 *  making this function safe to be called from outside of STREAMS for Stream ends only.
 *
 *  LOCKING: We take our own write locks to protect the queue structure in case the caller has not.
 *
 *  NOTICES: rmvq() panics when passed null pointers.  rmvq() panics if a write lock has not been
 *  taken on the queue.  rmvq() panics if the message is not a queue, or not on the specified queue.
 */
streams_fastcall __hot_in void
rmvq(register queue_t *q, register mblk_t *mp)
{ /* IRQ DISABLED */
	bool backenable;
	unsigned long pl;

	assert(q);
	assert(mp);

	assure(frozen_by_caller(q));

	pl = qwlock(q);
	backenable = __rmvq(q, mp);
	qwunlock(q, pl);

	/* Backenabling under locks is not so severe when write locks only suppress soft irq. */
	if (likely(backenable == 0))
		return;
	qbackenable(q, mp->b_band, NULL);
}

EXPORT_SYMBOL_NOVERS(rmvq);

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
STATIC __unlikely bool
__flushband(queue_t *q, unsigned char band, int flag, mblk_t ***mppp)
{
	bool backenable = false;

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
				backenable = true;	/* always backenable when band empty */
			}
		} else {
			struct qband *qb;

			if (!(qb = __find_qband(q, band)))
				goto done;
			/* This is faster.  For flushall, we link the qband chain onto the free
			   list and null out qband counts and markers. */
			if ((**mppp = qb->qb_first)) {
				_ptrace(("queue %p, band %d, flag %d\n", q, (int) band, flag));
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
				assert(q->q_msgs >= 0);
				qb->qb_msgs = 0;
				qb->qb_first = qb->qb_last = NULL;
				if (test_and_clear_bit(QB_FULL_BIT, &qb->qb_flag))
					--q->q_blocked;
				clear_bit(QB_WANTW_BIT, &qb->qb_flag);
				backenable = true;	/* always backenable when band empty */
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
 *
 *  MP-STREAMS: Note that qbackenable() will take its own Stream head read lock for Stream ends
 *  making this function safe to be called from outside of STREAMS for Stream ends only.
 *
 *  This function is not supposed to be called on a Stream that is frozen by the calling thread.
 */
streams_fastcall __unlikely void
flushband(register queue_t *q, int band, int flag)
{
	bool backenable;
	mblk_t *mp = NULL, **mpp = &mp;
	unsigned long pl;

	assert(q);
	assert(flag == FLUSHDATA || flag == FLUSHALL);

	assure(not_frozen_by_caller(q));

	pl = qwlock(q);
	backenable = __flushband(q, flag, band, &mpp);
	qwunlock(q, pl);

	if (unlikely(backenable != 0))
		qbackenable(q, band, NULL);

	/* we want to free messages with the locks off so that other CPUs can process this queue
	   and we don't block interrupts too long */
	mb();
	if (unlikely(mp != 0))
		freechain(mp, mpp);
}

EXPORT_SYMBOL_NOVERS(flushband);

/*
 *  __flushq:	- flush messages from a queue
 *  @q:		queue from which to flush messages
 *  @flag:	how, %FLUSHDATA or %FLUSHALL
 *  @mppp:	pointer to a pointer to the end of a message chain
 *  @bands:	array of band backenable flags
 *
 *  NOTICES: This function must be called with a queue write lock held across the call.
 *
 *  IMPLEMENTATION: __flushq() uses a fast freeing technique where it removes an entire chain of
 *  messages where possible and schedules their being freed back to the message poll to the
 *  freechains() task under the STREAMS scheduler.  Flushing of long chains is more efficient for
 *  %FLUSHALL than for %FLUSHDATA.
 *
 *  Implementation Notes: OpenSolaris and LiS remove all messages in both cases, unlock the queue
 *  and then start putting them back if they are data messages or freeing them, etc, then relock the
 *  queue and then fix up WANTW flags, and then backenable outside locks.  We remove all only under
 *  the FLUSHALL condition, otherwise we remove them one by one using __rmvq() (which is fairly
 *  fast) and then release the locks.  This is better atomicity of WANTW flags and ordering of
 *  messages on the queue if other messages are arriving.  We backenable and free buffer chains
 *  later outside the locks.  We are much faster for FLUSHALL of deep queues.
 *
 *  Note that the putq() deferral chain is flushed as well.
 */
streams_fastcall __unlikely bool
__flushq(queue_t *q, int flag, mblk_t ***mppp, char bands[])
{
	bool backenable = false;
	mblk_t *b;

	if ((b = q->q_first)) {
		if (likely(flag == FLUSHALL)) {
			struct qband *qb;

			/* This is fast! For flushall, we link the whole chain onto the free list
			   and null out counts and markers */
			**mppp = b;
			*mppp = &q->q_last->b_next;
			**mppp = NULL;
			q->q_first = q->q_last = NULL;
			q->q_count = 0;
			q->q_msgs = 0;
			clear_bit(QFULL_BIT, &q->q_flag);
			clear_bit(QWANTW_BIT, &q->q_flag);
			set_bit(QWANTR_BIT, &q->q_flag);
			for (qb = q->q_bandp; qb; qb = qb->qb_next) {
				qb->qb_first = qb->qb_last = NULL;
				qb->qb_count = 0;
				qb->qb_msgs = 0;
				if (test_and_clear_bit(QB_FULL_BIT, &qb->qb_flag))
					--q->q_blocked;
				clear_bit(QB_WANTW_BIT, &qb->qb_flag);
			}
			q->q_blocked = 0;
			backenable = true;	/* always backenable when queue empty */
			if (bands)
				do {
					bands[b->b_band] = true;
				} while ((b = b->b_next));
		} else if (likely(flag == FLUSHDATA)) {
			mblk_t *b_next;

			do {
				b_next = b->b_next;
				if (isdatamsg(b)) {
					if (__rmvq(q, b)) {
						backenable = true;
						if (bands)
							bands[b->b_band] = true;
					}
					**mppp = b;
					*mppp = &b->b_next;
					**mppp = NULL;
				}
			}
			while ((b = b_next));
		} else
			never();
	}
	return (backenable);
}

/**
 *  flushq:	- flush messages from a queue
 *  @q:		the queue to flush
 *  @flag:	how to flush: %FLUSHDATA or %FLUSHALL
 *
 *  NOTICES: flushband(0, flag) and flushq(flag) are two different things.
 *
 *  MP-STREAMS: Note that qbackenable() will take its own Stream head read lock for Stream ends
 *  making this function safe to be called from outside of STREAMS for Stream ends only.
 */
streams_fastcall __unlikely void
flushq(register queue_t *q, int flag)
{
	bool backenable;
	mblk_t *mp = NULL, **mpp = &mp;
	unsigned long pl;
	int q_nband;
	unsigned char back[NBAND] = { 0, };

	assert(q);
	assert(flag == FLUSHDATA || flag == FLUSHALL);

	assure(not_frozen_by_caller(q));

	pl = qwlock(q);
	q_nband = q->q_nband;
	backenable = __flushq(q, flag, &mpp, back);
	qwunlock(q, pl);

	if (unlikely(backenable != 0))
		qbackenable(q, q_nband, back);

	/* we want to free messages with the locks off so that other CPUs can process this queue
	   and we don't block interrupts too long */
	mb();
	if (unlikely(mp != 0))
		freechain(mp, mpp);
}

EXPORT_SYMBOL_NOVERS(flushq);		/* include/sys/streams/stream.h */

/*
 *  __getq:	- get next message off a queue
 *  @q:		the queue from which to get the message
 *
 *  CONTEXT:	This function must be called with the queue write locked.
 *
 *  RETURN VALUE: Returns a pointer to the next message, removed from the queue, or NULL if there is
 *  no message on the queue.
 */
STATIC streams_inline streams_fastcall __hot mblk_t *
__getq(queue_t *q, bool *be)
{
	mblk_t *mp;

	if (likely((mp = q->q_first) != NULL)) {
		mblk_t *b_next;

		*be = false;
		/* hand optimized version of above */
		if ((b_next = mp->b_next))
			b_next->b_prev = NULL;
		mp->b_next = NULL;
		q->q_first = b_next;
		if (q->q_last == mp)
			q->q_last = NULL;
		q->q_msgs--;
		assert(q->q_msgs >= 0);
		if (likely(mp->b_band == 0)) {
			q->q_count -= msgsize(mp);
			assert(q->q_count >= 0);
			if (q->q_count == 0) {
				clear_bit(QFULL_BIT, &q->q_flag);
				clear_bit(QWANTW_BIT, &q->q_flag);
				*be = true;
			} else if (q->q_count < q->q_lowat) {
				clear_bit(QFULL_BIT, &q->q_flag);
				if (test_and_clear_bit(QWANTW_BIT, &q->q_flag))
					*be = true;
			}
			/* no longer want to read band zero */
			clear_bit(QWANTR_BIT, &q->q_flag);
		} else {
			struct qband *qb;

			{
				unsigned char q_nband, band;

				for (band = mp->b_band, q_nband = q->q_nband, qb = q->q_bandp;
				     qb && q_nband > band; qb = qb->qb_next, --q_nband) ;
			}
			assert(qb);
			qb->qb_first = b_next;
			if (qb->qb_last == mp)
				qb->qb_last = NULL;
			qb->qb_msgs--;
			assert(qb->qb_msgs >= 0);
			qb->qb_count -= msgsize(mp);
			assert(qb->qb_count >= 0);
			if (qb->qb_count == 0 || qb->qb_count < qb->qb_lowat) {
				if (test_and_clear_bit(QB_FULL_BIT, &qb->qb_flag))
					q->q_blocked--;
				if (test_and_clear_bit(QB_WANTW_BIT, &qb->qb_flag))
					*be = true;
			}
			/* including band zero */
			clear_bit(QWANTR_BIT, &q->q_flag);
		}
	} else {
		*be = false;
		if (!test_and_set_bit(QWANTR_BIT, &q->q_flag))
			*be = true;
	}
	return (mp);
}

/**
 *  getq:	- get messags from a queue
 *  @q:		the queue from which to get messages
 *
 *  CONTEXT: Any, but should not be frozen by caller.
 *
 *  MP-STREAMS: Note that qbackenable() will take its own Stream head read lock for Stream ends
 *  making this function safe to be called from outside of STREAMS for Stream ends only.
 */
streams_fastcall __hot mblk_t *
getq(register queue_t *q)
{
	mblk_t *mp;
	bool backenable = false;
	unsigned long pl;

	assert(q);

	assure(not_frozen_by_caller(q));

	pl = qwlock(q);
	mp = __getq(q, &backenable);
	qwunlock(q, pl);
	if (likely(backenable == 0))
		return (mp);
	qbackenable(q, mp ? mp->b_band : 0, NULL);
	return (mp);
}

EXPORT_SYMBOL_NOVERS(getq);

/**
 *  SAMESTR:	- check whether this and next queue have the same stream head
 *  @q:		this queue
 *
 *  NOTICES: SAMESTR() must not be called from outside of STREAMS context (i.e., without being under
 *  a Stream head plumb read lock).  STREAMS procedures have this lock.  MPSTR_STPLOCK() and
 *  MPSTR_STPRELE() can be used from outside Streams to acquire the necessary lock to use this
 *  function.  freezestr() and unfreezestr() are not sufficient.
 */
__STRUTIL_EXTERN_INLINE int SAMESTR(queue_t *q);

EXPORT_SYMBOL_NOVERS(SAMESTR);

/*
 *  __setq:
 */
STATIC __unlikely void
__setq(queue_t *q, struct qinit *rinit, struct qinit *winit)
{
	q->q_qinfo = rinit;
	q->q_maxpsz = rinit->qi_minfo->mi_maxpsz;
	q->q_minpsz = rinit->qi_minfo->mi_minpsz;
	q->q_hiwat = rinit->qi_minfo->mi_hiwat;
	q->q_lowat = rinit->qi_minfo->mi_lowat;
#if defined CONFIG_STREAMS_SYNCQS
	if (q->q_syncq)
		set_bit(QSYNCH_BIT, &q->q_flag);
#endif
	q++;
	q->q_qinfo = winit;
	q->q_maxpsz = winit->qi_minfo->mi_maxpsz;
	q->q_minpsz = winit->qi_minfo->mi_minpsz;
	q->q_hiwat = winit->qi_minfo->mi_hiwat;
	q->q_lowat = winit->qi_minfo->mi_lowat;
#if defined CONFIG_STREAMS_SYNCQS
	if (q->q_syncq)
		set_bit(QSYNCH_BIT, &q->q_flag);
#endif
}

/**
 *  setq:	- set queue characteristics
 *  @q:		read queue in queue pair to set
 *  @rinit:	read queue init structure
 *  @winit:	write queue initi structure
 *
 *  setq() knows about syncrhonization queues.  If there are sycnchronization queues allocated it
 *  sets the QSYNCH bit to indicate so.  The proper sequence of events with syncrhonization queues
 *  is to allocate the syncrhonization queues with setsq() and the set the queues with setq().
 *  Syncrhonization queues from a multiplexed queue pair can be removed with setsq(q, NULL).
 */
streams_fastcall __unlikely void
setq(queue_t *q, struct qinit *rinit, struct qinit *winit)
{
	struct stdata *sd;
	unsigned long pl;

	assert(q);
	assert(not_frozen_by_caller(q));

	sd = qstream(q);

	assert(sd);

	pl = zwlock(sd);
	__setq(q, rinit, winit);
	/* unlock in reverse order */
	zwunlock(sd, pl);
}

EXPORT_SYMBOL_NOVERS(setq);

#if defined CONFIG_STREAMS_SYNCQS
struct syncq *global_syncq = NULL;
#endif

/*
 *  __setsq:	- set synchronization queues for a new queue pair
 *  @fmod:	fmodsw table entry for this module
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
STATIC __unlikely int
__setsq(queue_t *q, struct fmodsw *fmod)
{
#if defined CONFIG_STREAMS_SYNCQS
	struct syncq *sqr, *sqw, *sqo;

	/* make sure there is none to start */
	if ((sqr = (q + 0)->q_syncq)) {
		sq_put(&sqr->sq_outer);
		sq_put(&(q + 0)->q_syncq);
	}
	if ((sqw = (q + 1)->q_syncq)) {
		sq_put(&sqw->sq_outer);
		sq_put(&(q + 1)->q_syncq);
	}
	if (fmod == NULL) {
#endif
		/* just clear them */
		clear_bit(QSYNCH_BIT, &(q + 0)->q_flag);
		clear_bit(QSYNCH_BIT, &(q + 1)->q_flag);
		return (0);
#if defined CONFIG_STREAMS_SYNCQS
	}
	if (!(fmod->f_flag & D_MP)) {
		if (fmod->f_flag & D_MTOUTPERIM)
			sqo = sq_get(fmod->f_syncq);
		else
			sqo = NULL;

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
			sqr->sq_flag = SQ_INNER | ((fmod->f_flag & D_MTPUTSHARED) ? SQ_SHARED : 0);
			sqr->sq_outer = sqo;
			sqw->sq_level = fmod->f_sqlvl;
			sqw->sq_flag = SQ_INNER | ((fmod->f_flag & D_MTPUTSHARED) ? SQ_SHARED : 0);
			sqw->sq_outer = sq_get(sqo);
			break;
		case SQLVL_QUEUEPAIR:
			/* allocate one syncq for the queue pair */
			if (!(sqr = sq_alloc())) {
				sq_put(&sqo);
				goto enomem;
			}
			sqr->sq_level = fmod->f_sqlvl;
			sqr->sq_flag = SQ_INNER | ((fmod->f_flag & D_MTPUTSHARED) ? SQ_SHARED : 0);
			sqr->sq_outer = sqo;
			sqw = sq_get(sqr);
			break;
		default:
		case SQLVL_DEFAULT:
		case SQLVL_MODULE:	/* default */
			/* find the module and use its syncq */
			/* The registration function is responsible for allocating the module
			   synchronization queue and attaching it to the module structure */
			if (!(sqr = sq_get(fmod->f_syncq))) {
				if (!(sqr = sq_alloc())) {
					sq_put(&sqo);
					goto enomem;
				}
				fmod->f_syncq = sqr;
				sqr->sq_level = fmod->f_sqlvl;
				sqr->sq_flag =
				    SQ_INNER | ((fmod->f_flag & D_MTPUTSHARED) ? SQ_SHARED : 0);
				sqr->sq_outer = sqo;
			}
			sqw = sq_get(sqr);
			break;
		case SQLVL_ELSEWHERE:
			/* find the elsewhere syncq and use it */
			/* The registration function is responsible for finding and allocating the
			   external synchronization queue and attaching it to the module structure */
			if (!(sqr = sq_get(fmod->f_syncq))) {
				if (!(sqr = sq_alloc())) {
					sq_put(&sqo);
					goto enomem;
				}
				fmod->f_syncq = sqr;
				sqr->sq_level = fmod->f_sqlvl;
				sqr->sq_flag =
				    SQ_INNER | ((fmod->f_flag & D_MTPUTSHARED) ? SQ_SHARED : 0);
			}
			sqw = sq_get(sqr);
			/* cannot have outer perimeter at this level */
			sq_put(&sqo);
			break;
		case SQLVL_GLOBAL:	/* for testing */
			/* use the global syncq */
			sqr = sq_get(global_syncq);
			sqw = sq_get(global_syncq);
			/* cannot have outer perimeter at this level */
			sq_put(&sqo);
			break;
		}
		(q + 0)->q_syncq = sqr;
		(q + 1)->q_syncq = sqw;
	}
	return (0);
      enomem:
	return (-ENOMEM);	/* XXX: This should probably be ENOSR or EAGAIN. */
#endif
}

/**
 *  setsq:	- set synchornization charateristics on a queue pair
 *  @q:		read queue in queue pair to set
 *  @fmod:	module to which queue pair belongs
 *
 *  Set synchronization queue associated with a new queue pair, or a queue pair being newly linked
 *  under a multiplexing driver.  If @fmod is NULL, synchronization queues will be removed if
 *  present.  Setting synchronization queues will not set the QSYNCH bits.  A later invocation of
 *  setq() will do that.  Clearing syncrhonization queues, however, will always clear the QSYHCH
 *  bits.
 *
 *  Locking: A stream head write lock should be maintained across the call to ensure that there are
 *  no STREAMS coroutines running while the queues are being manipulated.
 */
streams_fastcall __unlikely int
setsq(queue_t *q, struct fmodsw *fmod)
{
	int result;
	struct stdata *sd;
	unsigned long pl;

	assert(q);
	assert(not_frozen_by_caller(q));

	sd = qstream(q);

	assert(sd);

	pl = zwlock(sd);
	result = __setsq(q, fmod);
	zwunlock(sd, pl);
	return (result);
}

EXPORT_SYMBOL_NOVERS(setsq);		/* for stream head include/sys/streams/strsubr.h */

/**
 *  strqget:	- get characteristics of a queue
 *  @q:		queue to query
 *  @what:	what characteristic to get
 *  @band:	from which queue band
 *  @val:	location of return value
 */
streams_fastcall __unlikely int
strqget(register queue_t *q, qfields_t what, register unsigned char band, long *val)
{
	int err = 0;
	unsigned long pl;

	assure(frozen_by_caller(q));

	pl = qrlock(q);
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
	}
	qrunlock(q, pl);
	return (-err);
}

EXPORT_SYMBOL_NOVERS(strqget);

/**
 *  strqset:	- set characteristics of a queue
 *  @q:		queue to set
 *  @what:	what characteristic to set
 *  @band:	to which queue band
 *  @val:	value to set
 *
 *  MP-STREAMS: The caller must freeze the Stream with freezestr(9) across the call to this
 *  function.  On UP it is not necessary unless strqset(9) is to be called from outside of the
 *  STREAMS context.
 */
streams_fastcall __unlikely int
strqset(register queue_t *q, qfields_t what, register unsigned char band, long val)
{
	int err = 0;
	unsigned long pl;

	assure(frozen_by_caller(q));

	pl = qwlock(q);
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
		struct qband *qb;

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
	}
	qwunlock(q, pl);
	return (-err);
}

EXPORT_SYMBOL_NOVERS(strqset);

STATIC spinlock_t str_err_lock = SPIN_LOCK_UNLOCKED;
STATIC char str_err_buf[LOGMSGSZ];

/*
 *  This is a default implementation for strlog(9).  When SL_CONSOLE is set, we print directly to
 *  the console using printk(9).  For SL_ERROR and SL_TRACE, we have no STREAMS error or trace
 *  loggers running, so we mark those messages as unseen by those loggers.  We also provide a hook
 *  here so that the strutil package can hook into this call.  Because we cannot filter, only
 *  SL_CONSOLE messages are printed to the system logs.  This follows the rules for setting the
 *  priority according described in log(4).
 */
STATIC __unlikely int
vstrlog_default(short mid, short sid, char level, unsigned short flag, char *fmt, va_list args)
{
	int rval = 1;

	if (flag & SL_CONSOLE) {
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

STATIC rwlock_t strlog_reg_lock = RW_LOCK_UNLOCKED;
STATIC vstrlog_t vstrlog_hook = &vstrlog_default;

/**
 *  register_strlog:	- register a new STREAMS logger
 *  @newlog:	new vstrlog function pointer
 *
 *  DESCRIPTION: register_strlog() registers a new STREAMS logger callback function and returns the
 *  previous callback function.  Suitable locks are taken to protect module unloading.
 *
 *  CONTEXT: register_strlog() is intended to be called from a STREAMS module or driver qi_qopen() or
 *  qi_qclose() procedure.  It must be called from process context.
 *
 *  LOCKING: This function holds a write lock on strlog_reg_lock to keep others from calling a
 *  strlog() implementation function that is about to be unloaded for safe log driver unloading.
 */
streams_fastcall __unlikely vstrlog_t
register_strlog(vstrlog_t newlog)
{
	unsigned long flags;
	vstrlog_t oldlog;

	write_lock_irqsave(&strlog_reg_lock, flags);
	oldlog = vstrlog_hook;
	vstrlog_hook = newlog;
	write_unlock_irqrestore(&strlog_reg_lock, flags);
	return (oldlog);
}

EXPORT_SYMBOL_NOVERS(register_strlog);

/**
 *  vstrlog:	- log a STREAMS message
 *  @mid:	module id
 *  @sid:	stream id
 *  @level:	severity level
 *  @flag:	flags controlling distribution
 *  @fmt:	printf(3) format
 *  @args:	format specific arguments
 */
streams_fastcall int
vstrlog(short mid, short sid, char level, unsigned short flag, char *fmt, va_list args)
{
	int result = 0;

	read_lock(&strlog_reg_lock);
	if (vstrlog_hook != NULL) {
		result = (*vstrlog_hook) (mid, sid, level, flag, fmt, args);
	}
	read_unlock(&strlog_reg_lock);
	return (result);
}

EXPORT_SYMBOL_NOVERS(vstrlog);

/**
 *  strlog:	- log a STREAMS message
 *  @mid:	module id
 *  @sid:	stream id
 *  @level:	severity level
 *  @flag:	flags controlling distribution
 *  @fmt:	printf(3) format
 *  @...:	format specific arguments
 *
 *  CONTEXT: strlog() can be called from any context, however, the caller should be aware that this
 *  function is complex and should only be called from in_interrupt() context sparingly.
 *
 *  LOCKING: This function holds a read lock on strlog_reg_lock to keep de-registrations from
 *  occurring while the function is being called for safe log driver unloading.
 */
streams_fastcall int
strlog(short mid, short sid, char level, unsigned short flag, char *fmt, ...)
{
	int result = 0;

	read_lock(&strlog_reg_lock);
	if (vstrlog_hook != NULL) {
		va_list args;

		va_start(args, fmt);
		result = (*vstrlog_hook) (mid, sid, level, flag, fmt, args);
		va_end(args);
	}
	read_unlock(&strlog_reg_lock);
	return (result);
}

EXPORT_SYMBOL_NOVERS(strlog);

/**
 *  unfreezestr:	- thaw a stream frozen with freezestr()
 *  @q:			the queue in the stream to thaw
 *  @flags:		spl flags
 */
streams_fastcall __unlikely void
unfreezestr(queue_t *q, unsigned long flags)
{
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);

	(void) flags;
	zwunlock(sd, flags);
}

EXPORT_SYMBOL_NOVERS(unfreezestr);

/**
 *  WR:		- get write queue in queue pair
 *  @q:		read queue pointer
 */
__STRUTIL_EXTERN_INLINE queue_t *WR(queue_t *q);

EXPORT_SYMBOL_NOVERS(WR);

/*
 *  vcmn_err:
 */
streams_fastcall void
vcmn_err(int err_lvl, const char *fmt, va_list args)
{
	unsigned long flags;
	char *cmn_err_ptr = str_err_buf;

	/* XXX: are these strict locks necessary? */
	spin_lock_irqsave(&str_err_lock, flags);
	vsnprintf(str_err_buf, sizeof(str_err_buf), fmt, args);
	if (str_err_buf[0] == '^' || str_err_buf[0] == '!')
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
		spin_unlock_irqrestore(&str_err_lock, flags);
		panic("%s\n", cmn_err_ptr);
		return;
	case CE_DEBUG:		/* IRIX 6.5 */
		printk(KERN_DEBUG "%s\n", cmn_err_ptr);
		break;
	case CE_ALERT:		/* IRIX 6.5 */
		printk(KERN_ALERT "%s \n", cmn_err_ptr);
		break;
	}
	spin_unlock_irqrestore(&str_err_lock, flags);
	return;
}

EXPORT_SYMBOL_NOVERS(vcmn_err);

/**
 *  cmn_err:	- print a command error
 *  @err_lvl:	severity
 *  @fmt:	printf(3) format
 *  @...:	format arguments
 */
streams_fastcall void
cmn_err(int err_lvl, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vcmn_err(err_lvl, fmt, args);
	va_end(args);
	return;
}

EXPORT_SYMBOL_NOVERS(cmn_err);

__STRUTIL_EXTERN_INLINE int copyin(const void *from, void *to, size_t len);

EXPORT_SYMBOL_NOVERS(copyin);

__STRUTIL_EXTERN_INLINE int copyout(const void *from, void *to, size_t len);

EXPORT_SYMBOL_NOVERS(copyout);

__STRUTIL_EXTERN_INLINE void delay(unsigned long ticks);

EXPORT_SYMBOL_NOVERS(delay);

streams_fastcall int
drv_getparm(const unsigned int parm, void *value_p)
{
	switch (parm) {
	case LBOLT:
		*(unsigned long *) value_p = jiffies;
		return (0);
	case PPGP:
#if defined HAVE_KFUNC_PROCESS_GROUP
		*(pid_t *) value_p = process_group(current);
#else
#if defined HAVE_KMEMB_STRUCT_TASK_STRUCT_PGRP
		*(pid_t *) value_p = current->pgrp;
#else
		*(pid_t *) value_p = current->signal->pgrp;
#endif
#endif
		return (0);
	case UPROCP:
		*(ulong *) value_p = (ulong) current->files;
		return (0);
	case PPID:
		*(pid_t *) value_p = current->pid;
		return (0);
	case PSID:
#if defined HAVE_KMEMB_STRUCT_TASK_STRUCT_SESSION
		*(pid_t *) value_p = current->session;
#else
		*(pid_t *) value_p = current->signal->session;
#endif
		return (0);
	case TIME:
	{
		struct timeval tv;

		do_gettimeofday(&tv);
		*(time_t *) value_p = tv.tv_sec;
		return (0);
	}
	case UCRED:
		*(cred_t **) value_p = current_creds;
		return (0);
	case STRMSGSIZE:
		*(int *) value_p = (int) sysctl_str_strmsgsz;
		return (0);
	case HW_PROVIDER:
		*(char **) value_p = "Linux " UTS_RELEASE " " UTS_VERSION;
		return (0);
	case DRV_MAXBIOSIZE:
	case SYSCRED:
		return (-1);
	}
	return (-1);
}

EXPORT_SYMBOL_NOVERS(drv_getparm);

__STRUTIL_EXTERN_INLINE unsigned long drv_hztomsec(unsigned long hz);

EXPORT_SYMBOL_NOVERS(drv_hztomsec);

__STRUTIL_EXTERN_INLINE unsigned long drv_hztousec(unsigned long hz);

EXPORT_SYMBOL_NOVERS(drv_hztousec);

__STRUTIL_EXTERN_INLINE unsigned long drv_msectohz(unsigned long msec);

EXPORT_SYMBOL_NOVERS(drv_msectohz);

__STRUTIL_EXTERN_INLINE int drv_priv(cred_t *crp);

EXPORT_SYMBOL_NOVERS(drv_priv);

__STRUTIL_EXTERN_INLINE unsigned long drv_usectohz(unsigned long usec);

EXPORT_SYMBOL_NOVERS(drv_usectohz);

__STRUTIL_EXTERN_INLINE void drv_usecwait(unsigned long usec);

EXPORT_SYMBOL_NOVERS(drv_usecwait);

__STRUTIL_EXTERN_INLINE major_t getmajor(dev_t dev);

EXPORT_SYMBOL_NOVERS(getmajor);

__STRUTIL_EXTERN_INLINE minor_t getminor(dev_t dev);

EXPORT_SYMBOL_NOVERS(getminor);

__STRUTIL_EXTERN_INLINE dev_t makedevice(major_t major, minor_t minor);

EXPORT_SYMBOL_NOVERS(makedevice);
