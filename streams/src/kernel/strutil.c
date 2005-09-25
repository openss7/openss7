/*****************************************************************************

 @(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.74 $) $Date: 2005/09/25 06:27:29 $

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

 Last Modified $Date: 2005/09/25 06:27:29 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.74 $) $Date: 2005/09/25 06:27:29 $"

static char const ident[] =
    "$RCSfile: strutil.c,v $ $Name:  $($Revision: 0.9.2.74 $) $Date: 2005/09/25 06:27:29 $";

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
#if 0
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif
#endif
#include <asm/cache.h>		/* for L1_CACHE_BYTES */

#include <stdbool.h>		/* for bool, true and false */

#define __STRUTIL_EXTERN_INLINE inline

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

#if 0
/*
 *  This file (also) contains a set of specialized STREAMS kernel locks that have the following
 *  charactersistics:
 *
 *  - they are read/write locks
 *  - taking a write lock suppresses local soft interrupts
 *  - taking a read lock suppresses suppresses local STREAMS soft interrupts
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
 *  - write locks can be taken from all contexts except hard irq
 *  - read locks can be taken from all contexts except hard irq
 *
 *  Write locks are expensive, seldom used and not held for long.
 */

streams_fastcall void
klockinit(klock_t *kl)
{
//      kl->kl_isrflags = 0;
	rwlock_init(&kl->kl_lock);
	kl->kl_owner = NULL;
	kl->kl_nest = 0;
	init_waitqueue_head(&kl->kl_waitq);
}
streams_fastcall void
kwlock(klock_t *kl)
{
	if (kl->kl_owner == current)
		kl->kl_nest++;
	else {
//              unsigned long flags;

//              local_irq_save(flags);
		local_bh_disable();
//              local_str_disable();
		write_lock(&kl->kl_lock);
//              kl->kl_isrflags = flags;
		kl->kl_owner = current;
		kl->kl_nest = 0;
	}
}
streams_fastcall int
kwtrylock(klock_t *kl)
{
	int locked = 1;

	if (kl->kl_owner == current)
		kl->kl_nest++;
	else {
//              unsigned long flags;

//              local_irq_save(flags);
		local_bh_disable();
//              local_str_disable();
		if (write_trylock(&kl->kl_lock)) {
//                      kl->kl_isrflags = flags;
			kl->kl_owner = current;
			kl->kl_nest = 0;
		} else {
//                      local_irq_restore(flags);
//                      local_str_enable();
			local_bh_enable();
			locked = 0;
		}
	}
	return (locked);
}
streams_fastcall void
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
streams_fastcall int
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
streams_fastcall void
kwunlock(klock_t *kl)
{
	if (kl->kl_nest > 0)
		kl->kl_nest--;
	else {
//              unsigned long flags = kl->kl_isrflags;

		kl->kl_owner = NULL;
		kl->kl_nest = 0;
		if (waitqueue_active(&kl->kl_waitq))
			wake_up(&kl->kl_waitq);
		write_unlock(&kl->kl_lock);
//              local_str_enable();
		local_bh_enable();
//              local_irq_restore(flags);
	}
}
streams_fastcall void
krlock(klock_t *kl)
{
	if (kl->kl_owner == current)
		kl->kl_nest++;
	else {
		local_str_disable();
		read_lock(&kl->kl_lock);
	}
}
streams_fastcall void
krunlock(klock_t *kl)
{
	if (kl->kl_nest > 0)
		kl->kl_nest--;
	else {
		if (waitqueue_active(&kl->kl_waitq))
			wake_up(&kl->kl_waitq);
		read_unlock(&kl->kl_lock);
		local_str_enable();
	}
}

#if 0
void
krlock_irqsave(klock_t *kl, unsigned long *flagsp)
{
	if (kl->kl_owner == current) {
		kl->kl_nest++;
		*flagsp = 0;
	} else
		read_lock_irqsave(&kl->kl_lock, *flagsp);
}

void
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
#endif
#endif

/* queue band gets and puts */
streams_fastcall qband_t *
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
streams_fastcall void
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
streams_fastcall queue_t *
qget(queue_t *q)
{
	struct queinfo *qu;

	if (q) {
		qu = (typeof(qu)) RD(q);
		if (atomic_read(&qu->qu_refs) < 1)
			swerr();
		atomic_inc(&qu->qu_refs);
		printd(("%s: queue pair %p ref count is now %d\n",
			__FUNCTION__, qu, atomic_read(&qu->qu_refs)));
	}
	return (q);
}
streams_fastcall void
qput(queue_t **qp)
{
	queue_t *q;

	trace();
	if ((q = xchg(qp, NULL))) {
		queue_t *rq = RD(q);
		struct queinfo *qu = (typeof(qu)) rq;

		assert(atomic_read(&qu->qu_refs) >= 1);
		printd(("%s: queue pair %p ref count is now %d\n",
			__FUNCTION__, qu, atomic_read(&qu->qu_refs) - 1));
		if (atomic_dec_and_test(&qu->qu_refs))
			freeq(rq);
	}
}

static int
db_ref(dblk_t * db)
{
	return (db->db_ref = atomic_read(&db->db_users));
}
static void
db_set(dblk_t * db, int val)
{
	atomic_set(&db->db_users, val);
	db_ref(db);
}
static void
db_inc(dblk_t * db)
{
	atomic_inc(&db->db_users);
	db_ref(db);
}

#if 0
static void
db_dec(dblk_t * db)
{
	atomic_dec(&db->db_users);
	db_ref(db);
}
#endif
static int
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
	(((unsigned char *)&((struct mdbblock *)0)->databuf[0]) - ((unsigned char *)0))

static mblk_t *
db_to_mb(dblk_t * db)
{
	return ((mblk_t *) ((unsigned char *) db - datablk_offset + msgblk_offset));
}
static unsigned char *
db_to_buf(dblk_t * db)
{
	return ((unsigned char *) db - datablk_offset + databuf_offset);
}

#if 0
static unsigned char *
mb_to_buf(mblk_t *mb)
{
	return ((unsigned char *) mb - msgblk_offset + databuf_offset);
}
#endif
static dblk_t *
mb_to_db(mblk_t *mb)
{
	return ((dblk_t *) ((unsigned char *) mb - msgblk_offset + datablk_offset));
}
static struct mdbblock *
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
streams_fastcall int
adjmsg(mblk_t *mp, ssize_t length)
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

EXPORT_SYMBOL(adjmsg);		/* include/sys/streams/stream.h */

/**
 *  allocb:	- allocate a message block
 *  @size:	size of message block in bytes
 *  @priority:	priority of the allocation
 */
streams_fastcall mblk_t *
allocb(size_t size, uint priority)
{
	mblk_t *mp;

	trace();
	if ((mp = mdbblock_alloc(priority, &allocb))) {
		struct mdbblock *md = mb_to_mdb(mp);
		unsigned char *base = md->databuf;
		dblk_t *db = &md->datablk.d_dblock;

		if (size <= FASTBUF)
			size = FASTBUF;
		else if (!(base = kmem_alloc(size, (priority == BPRI_WAITOK)
					     ? KM_SLEEP : KM_NOSLEEP)))
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
	return (NULL);
}

EXPORT_SYMBOL(allocb);

/**
 *  copyb:	- copy a message block
 *  @bp:	the message block to copy
 *
 *  Return Value: Returns the copy of the message or %NULL on failure.
 *
 *  Notices: Unlike LiS we do not align the copy.  The driver must me wary of alignment.
 */
extern streams_fastcall mblk_t *
copyb(mblk_t *mp)
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
		}
	}
	return (b);
}

EXPORT_SYMBOL(copyb);		/* include/sys/streams/stream.h */

/**
 *  copymsg:	- copy a message
 *  @msg:	message to copy
 *
 *  Copies all the message blocks in message @msg and returns a pointer to the copied message.
 */
streams_fastcall mblk_t *
copymsg(mblk_t *mp)
{
	mblk_t *msg = NULL;
	register mblk_t *b, **bp = &msg;

	for (b = mp; b; b = b->b_cont, bp = &(*bp)->b_cont)
		if (!(*bp = copyb(b)))
			goto error;
	return (msg);
      error:
	freemsg(mp);
	return (NULL);
}

EXPORT_SYMBOL(copymsg);		/* include/sys/streams/stream.h */

/**
 *  ctlmsg:	- test for control message type
 *  @type:	type to test
 */
streams_fastcall int
ctlmsg(unsigned char type)
{
	unsigned char mod = (type & ~QPCTL);

	/* just so happens there is a gap in the QNORM messages right at M_PCPROTO */
	return (((1 << mod) & ((1 << M_DATA) | (1 << M_PROTO) | (1 << (M_PCPROTO & ~QPCTL)))) == 0);
}

EXPORT_SYMBOL(ctlmsg);		/* include/sys/streams/stream.h */

/**
 *  datamsg:	- test for data message type
 *  @type:	type to test
 */
streams_fastcall int
datamsg(unsigned char type)
{
	unsigned char mod = (type & ~QPCTL);

	/* just so happens there is a gap in the QNORM messages right at M_PCPROTO */
	return (((1 << mod) &
		 ((1 << M_DATA) | (1 << M_PROTO) | (1 << (M_PCPROTO & ~QPCTL)) | (1 << M_DELAY))) !=
		0);
}

EXPORT_SYMBOL(datamsg);

/**
 *  dupb:	- duplicates a message block
 *  @bp:	message block to duplicate
 */
streams_fastcall mblk_t *
dupb(mblk_t *bp)
{
	mblk_t *mp;

	if ((mp = mdbblock_alloc(BPRI_MED, &dupb))) {
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
streams_fastcall mblk_t *
dupmsg(mblk_t *mp)
{
	mblk_t *msg = NULL;
	register mblk_t *b, **bp = &msg;

	for (b = msg; b; b = b->b_cont, bp = &(*bp)->b_cont)
		if (!(*bp = dupb(b)))
			goto error;
	return (msg);
      error:
	freemsg(msg);
	return (NULL);
}

EXPORT_SYMBOL(dupmsg);		/* include/sys/streams/stream.h */

/**
 *  esballoc:	- allocate a message block with an external buffer
 *  @base:	base address of message buffer
 *  @size:	size of the message buffer
 *  @priority:	priority of message block header allocation
 *  @freeinfo:	free routine callback
 */
streams_fastcall mblk_t *
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
streams_fastcall void
freeb(mblk_t *mp)
{
	dblk_t *dp, *db;

	trace();
	/* check null ptr, message still on queue */
	assert(mp);
	assert(!mp->b_queue);

	db = xchg(&mp->b_datap, NULL);

	/* check double free */
	assert(db);

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
}

EXPORT_SYMBOL(freeb);

/**
 *  freemsg:	- free a message
 *  @mp:	the message to free
 */
streams_fastcall void
freemsg(mblk_t *mp)
{
	register mblk_t *b, *bp;

	for (b = mp; (bp = b); b = b->b_cont, freeb(bp)) ;
}

EXPORT_SYMBOL(freemsg);

/**
 *  isdatablk:	- test data block for data type
 *  @dp:	data block to test
 */
__STRUTIL_EXTERN_INLINE int isdatablk(dblk_t * dp);

EXPORT_SYMBOL(isdatablk);

/**
 *  isdatamsg:	- test a message block for data type
 *  @mp:	message block to test
 */
__STRUTIL_EXTERN_INLINE int isdatamsg(mblk_t *mp);

EXPORT_SYMBOL(isdatamsg);

/**
 *  pcmsg:	- data block type for priority
 *  @type:	the type to check
 */
__STRUTIL_EXTERN_INLINE int pcmsg(unsigned char type);

EXPORT_SYMBOL(pcmsg);

/**
 *  linkb:	- link message block onto message
 *  @mp1:	message onto which to link
 *  @mp2:	message block to link
 */
__STRUTIL_EXTERN_INLINE void linkb(mblk_t *mp1, mblk_t *mp2);

EXPORT_SYMBOL(linkb);

/**
 *  linkmsg:	- link messages
 *  @mp1:	message onto which to link
 *  @mp2:	message to link
 */
__STRUTIL_EXTERN_INLINE mblk_t *linkmsg(mblk_t *mp1, mblk_t *mp2);

EXPORT_SYMBOL(linkmsg);

/**
 *  msgdsize:	- calculate size of data in message
 *  @mp:	message across which to calculate data bytes
 */
streams_fastcall size_t
msgdsize(mblk_t *mp)
{
	register mblk_t *b;
	size_t size = 0;

	for (b = mp; b; b = b->b_cont)
		if (b->b_datap->db_type == M_DATA)
			if (b->b_wptr > b->b_rptr)
				size += b->b_wptr - b->b_rptr;
	return (size);
}

EXPORT_SYMBOL(msgdsize);

/**
 *  msgpullup:	- pull up bytes into a message
 *  @mp:	message to pull up
 *  @length:	number of bytes to pull up
 *
 *  Pulls up @length  bytes into the returned message.  This is for handling headers as a contiguous
 *  range of bytes.
 */
streams_fastcall mblk_t *
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

EXPORT_SYMBOL(msgpullup);

/**
 *  msgsize:	- calculate size of a message
 *  @mp:	message for which to calculate size
 */
streams_fastcall size_t
msgsize(mblk_t *mp)
{
	register mblk_t *b;
	size_t size = 0;

	for (b = mp; b; b = b->b_cont)
		if (b->b_wptr > b->b_rptr)
			size += b->b_wptr - b->b_rptr;
	return (size);
}

EXPORT_SYMBOL(msgsize);

/**
 *  pullupmsg:	- pull up bytes into first data block in message
 *  @mp:	message to pull up
 *  @len:	number of bytes to pull up
 *
 *  Pulls up @length  bytes into the initial data block in message @mp.  This is for handling headers
 *  as a contiguous range of bytes.
 */
streams_fastcall int
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
streams_fastcall mblk_t *
rmvb(mblk_t *mp, mblk_t *bp)
{
	mblk_t **mpp;

	if (likely(bp != NULL)) {
		for (mpp = &mp; *mpp && *mpp != bp; mpp = &(*mpp)->b_cont) ;
		if (likely(*mpp != NULL)) {
			*mpp = xchg(&bp->b_cont, NULL);
			return (mp);
		}
	}
	return ((mblk_t *) (-1));
}

EXPORT_SYMBOL(rmvb);

/**
 *  testb:	- test allocate of a message block
 *  @size:	size of buffer for which to test
 *  @priority:	allocation priority to test
 */
streams_fastcall int
testb(size_t size, uint priority)
{
	mblk_t *mp;

	(void) priority;
	if ((mp = allocb(size, priority)))
		freeb(mp);
	return (mp != NULL);
}

EXPORT_SYMBOL(testb);

/**
 *  unlinkb:	- unlink first block of message
 *  @mp:	message to unlink
 */
__STRUTIL_EXTERN_INLINE mblk_t *unlinkb(mblk_t *mp);

EXPORT_SYMBOL(unlinkb);

__STRUTIL_EXTERN_INLINE mblk_t *unlinkmsg(mblk_t *mp, mblk_t *bp);

EXPORT_SYMBOL(unlinkmsg);

/**
 *  xmsgsize:	- calculate size in message of same type as first data block
 *
 *  Notices: This is not bug-to-bug compatible with LiS.  Some differences: LiS wraps at a message
 *  size of 65636 and cannot handle message blocks larger than 65636.  LiS will consider a non-zero
 *  initial block (such as that left by adjmsg()) as the first message block type when it should
 *  not.  This implementation does not wrap the size, and skips initial zero-length message blocks.
 *  This implementation of xmsgsize does not span non-zero blocks of different types.
 */
streams_fastcall size_t
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
 *  MP-STREAMS: The Stream needs to be frozen by the caller with freezestr() or the call will fail
 *  under assertions.
 */
streams_fastcall int
appq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	int result;
	unsigned long pl;

	assert(q);
	assert(nmp);

	assure(frozen_by_caller(q));

	pl = qwlock(q);
	result = __insq(q, emp ? emp->b_next : emp, nmp);
	qwunlock(q, pl);
	/* do enabling outside the locks */
	switch (result) {
	case 3:		/* success - high priority enable */
		assert(0);
		qenable(q);
		return (1);
	case 2:		/* success - enable if not noenabled */
		enableq(q);
	case 1:		/* success - don't enable */
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
 *
 *  CONTEXT: STREAMS only.
 */
__STRUTIL_EXTERN_INLINE queue_t *backq(queue_t *q);

EXPORT_SYMBOL(backq);

/**
 *  qbackenable: - backenable a queue
 *  @q:		the queue to backenable
 *
 *  CONTEXT: qbackenable() can be called from any context.
 *
 *  MP-STREAMS: Because qbackenable() can be invoked from outside streams (i.e. from getq()), it
 *  takes a STREAM head read lock.
 */
streams_fastcall void
qbackenable(queue_t *q)
{
	struct stdata *sd;
	queue_t *q_back;

	assert(q);

	sd = qstream(q);

	assert(sd);

	prlock(sd);
	q_back = backq(q);
	while ((q = q_back) && !q->q_qinfo->qi_srvp && (q_back = backq(q))) ;
	if (q)
		enableq(q);	/* normal enable */
	prunlock(sd);
}

EXPORT_SYMBOL(qbackenable);

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
streams_fastcall int
bcangetany(queue_t *q)
{
	int found = 0;
	mblk_t *b;
	unsigned long pl;

	assert(q);

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
 *
 *  NOTICES: The caller is responsible for the validity of the passed in queue pointer.
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: No locks are required across the call.
 */
streams_fastcall int
bcanget(queue_t *q, unsigned char band)
{
	int found = 0;
	unsigned long pl;

	assert(q);

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

EXPORT_SYMBOL(bcanget);		/* include/sys/streams/stream.h */

static streams_fastcall int
_bcanputany(queue_t *q)
{
	bool result;
	unsigned long pl;

	/* find first queue with service procedure or no q_next pointer */
	for (; !q->q_qinfo->qi_srvp && q->q_next; q = q->q_next) ;

	pl = qrlock(q);
#if 0
	{
		struct qband *qb;

		for (qb = q->q_bandp; qb && test_bit(QB_FULL_BIT, &qb->qb_flag); qb = qb->qb_next) ;
		/* a non-existent band is considered unwritable */
		result = (qb != NULL);
	}
#else
	result = (q->q_blocked < q->q_nband);
#endif
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

	assert(q);
	sd = qstream(q);
	assert(sd);

	prlock(sd);
	result = _bcanputany(q);
	prunlock(sd);

	return (result);
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
streams_fastcall int
bcanputnextany(queue_t *q)
{
	int result;
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);
	prlock(sd);
	result = _bcanputany(q->q_next);
	prunlock(sd);

	return (result);
}

EXPORT_SYMBOL(bcanputnextany);	/* include/sys/streams/stream.h */

/*
 *  __find_qband:
 *
 * Find a queue band.  This must be called with the queue read or write locked.
 */
#if 0
static struct qband *
__find_qband(queue_t *q, unsigned char band)
{
	struct qband *qb;

	/* bands are sorted in decending priority so that we can quit the search early for higher
	   priority bands */
	for (qb = q->q_bandp; qb && qb->qb_band > band; qb = qb->qb_next) ;
	if (qb && qb->qb_band == band)
		return (qb);
	return (NULL);
}
#else
static struct qband *
__find_qband(queue_t *q, unsigned char band)
{
	struct qband *qb;
	unsigned char q_nband;

	for (q_nband = q->q_nband, qb = q->q_bandp; qb && q_nband > band;
	     qb = qb->qb_next, --q_nband) ;
	return (qb);
}
#endif

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
#if 0
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
#else
static struct qband *
__get_qband(queue_t *q, unsigned char band)
{
	struct qband *qb;

	if (band <= q->q_nband) {
		qb = __find_qband(q, band);
		assert(qb);
	} else {
		unsigned char q_nband = q->q_nband;

		do {
			if (!(qb = allocqb()))
				break;
			qb->qb_next = xchg(&q->q_bandp, qb);
			qb->qb_hiwat = q->q_hiwat;
			qb->qb_lowat = q->q_lowat;
			qb->qb_flag = QB_WANTR;
			++q->q_nband;
		} while (band > q_nband);
	}
	return (qb);
}
#endif

/*
 *  Version without locks.
 */
static streams_fastcall inline int
_bcanput(queue_t *q, unsigned char band)
{
	int result = 1;
	unsigned long pl;

	/* find first queue with service procedure or no q_next pointer */
	for (; !q->q_qinfo->qi_srvp && q->q_next; q = q->q_next) ;

	pl = qwlock(q);

	if (band == 0) {
		if (test_bit(QFULL_BIT, &q->q_flag)) {
			set_bit(QWANTW_BIT, &q->q_flag);
			result = 0;
		}
	} else {
		struct qband *qb;

		if (band <= q->q_nband && q->q_blocked > 0) {
			if ((qb = __find_qband(q, band)) && test_bit(QB_FULL_BIT, &qb->qb_flag)) {
				set_bit(QB_WANTW_BIT, &qb->qb_flag);
				result = 0;
			}
		}
		/* Note: a non-existent band is considered empty */
	}

	qwunlock(q, pl);

	return (result);
}

/**
 *  bcanput:		- check whether message of a given band can be put to a queue
 *  @q:			the queue to check
 *  @band:		the band to check
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: Takes a Stream head plumb read lock to permit this function to be called from outside a
 *  queue procedure belonging to @q and from process context.  The Stream head plumb read lock
 *  prevents any queue pair from either being inserted into or deleted from the stream while held.
 *
 *  MP-STREAMS: bcanput() is complicated because it really needs a Stream head plumb read lock to
 *  walk the stream as well as a queue read lock to walk the band structure.  Permitting this
 *  function to take these locks from an ISR or bottom-half would require that the Stream head plumb
 *  write locks and queue write locks suppress all interrupts, which is too strict for the most
 *  part.  One possible solution is to only suppress local interrupts for Stream head plumb write
 *  locks on Streams that contain driver read queues, and suppress local interrupts for driver queue
 *  pairs.  This would make all queue functions ISR safe on the bottommost queue pairs, but not on
 *  others.  put() and putq() would be okay, but putnext() would have to defer.  Another possibility
 *  is to upgrade write locks to be bottom-half locks, and only allows this function to be called
 *  from bottom-half and hot hard interrupt.  But as most ISRs defer the bulk of their execution to
 *  bottom-half, blocking bottom-halves is almost as bad as suppressing hard interrupts.
 *
 *  In the end, I decided to suppress interrupts for Stream head plumb write locks and queue read
 *  and write locks, permitting almost all functions to be executed from an ISR.
 */
streams_fastcall int
bcanput(queue_t *q, unsigned char band)
{
	int result;
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);
	prlock(sd);
	result = _bcanput(q, band);
	prunlock(sd);
	return (result);
}

EXPORT_SYMBOL(bcanput);

/**
 *  bcanputnext: - check whether messages can be put to queue after this one
 *  @q:		this queue
 *  @band:	band to check
 *
 *  CONTEXT: Any.
 *
 *  NOTICES: The caller is responsible for ensuring that q->q_next is not NULL across the call.  A
 *  module can be sure that both its q->q_next pointers are non-NULL, a driver can be sure that its
 *  RD(q)->q_next pointer is non-NULL and that its WR(q)->q_next pointer is NULL, a STREAM head
 *  which is not hanged up can be sure that its WR(q)->q_next pointer is non-NULL.
 *
 *  MP-STREAMS: If called from outside the STREAMS context, the caller is responsible for taking
 *  a STREAM head read lock across the call.  (This is what the STREAM head does.)  In general, this
 *  function should only be called from outside the STREAMS context by the STREAM head.  Drivers
 *  should use bcanput().
 *
 *  CONTEXT: A Stream head plumb read lock is taken, so bcanputnext() can be called from any
 *  context, inside and outside of STREAMS.
 *
 *  MP-STREAMS: The caller is responsible for the validity of the passed in q pointer.  A reference
 *  to a queue is generally valid from after qprocson(9) returns until qprocsoff(9) is called for q.
 *  Note that, when called from outside of the STREAMS context, the result might not reflect the
 *  state of the Stream.  From outside the STREAMS context, the caller an bracket freezestr(q) and
 *  unfreezestr(q) around the call.  The result will reflect the actual state of the Stream until
 *  unfreezestr(q) is called.
 */
streams_fastcall int
bcanputnext(queue_t *q, unsigned char band)
{
	int result;
	struct stdata *sd;

	assert(q);
	sd = qstream(q);
	assert(sd);
	prlock(sd);
	result = _bcanput(q->q_next, band);
	prunlock(sd);
	return (result);
}

EXPORT_SYMBOL(bcanputnext);

/**
 *  canenable:	- check whether service procedure will run
 *  @q:		queue to check
 */
__STRUTIL_EXTERN_INLINE int canenable(queue_t *q);

EXPORT_SYMBOL(canenable);

/**
 *  canget:	- check whether normal band zero (0) messages are on queue
 *  @q:		queue to check
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: None.
 */
__STRUTIL_EXTERN_INLINE int canget(queue_t *q);

EXPORT_SYMBOL(canget);		/* include/sys/streams/stream.h */

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

EXPORT_SYMBOL(canput);		/* include/sys/streams/stream.h */

/**
 *  canputnext:		- check whether messages can be put to the queue after this one
 *  @q:			the queue whose next queue to check
 *
 *  Simply implemented as bcanputnext(q, 0).  See bcanputnext() for details.
 *
 *  CONTEXT: Any.
 *
 *  LOCKING: STREAM head read lock when called from !in_streams() context.
 */
__STRUTIL_EXTERN_INLINE int canputnext(queue_t *q);

EXPORT_SYMBOL(canputnext);

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
static bool
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
 *  MP-STREAMS: Note that qbackenable() will take its own STREAM head read lock making this function
 *  safe to be called from outside of STREAMS.
 *
 *  This function is not supposed to be called on a Stream that is frozen by the calling thread.
 */
streams_fastcall void
flushband(queue_t *q, int band, int flag)
{
	bool backenable;
	mblk_t *mp = NULL, **mpp = &mp;
	unsigned long pl;

	assert(q);
	assert(flag == FLUSHDATA || flag == FLUSHALL);

	assert(not_frozen_by_caller(q));

	trace();
	pl = qwlock(q);
	backenable = __flushband(q, flag, band, &mpp);
	qwunlock(q, pl);
	trace();
	if (backenable) {
		trace();
		qbackenable(q);
	}
	trace();
	/* we want to free messages with the locks off so that other CPUs can process this queue
	   and we don't block interrupts too long */
	mb();
	if (mp)
		freechain(mp, mpp);
	trace();
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
bool
__flushq(queue_t *q, int flag, mblk_t ***mppp)
{
	bool backenable = false;

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
			set_bit(QWANTR_BIT, &q->q_flag);
			for (qb = q->q_bandp; qb; qb = qb->qb_next) {
				qb->qb_first = qb->qb_last = NULL;
				qb->qb_count = 0;
				qb->qb_msgs = 0;
				if (test_and_clear_bit(QB_FULL_BIT, &qb->qb_flag))
					--q->q_blocked;
				clear_bit(QB_WANTW_BIT, &qb->qb_flag);
				set_bit(QB_WANTR_BIT, &qb->qb_flag);
			}
			q->q_blocked = 0;
			backenable = true;	/* always backenable when queue empty */
		}
	} else if (likely(flag == FLUSHDATA)) {
		mblk_t *b, *b_next;

		if ((b = q->q_first)) {
			do {
				b_next = b->b_next;
				if (isdatamsg(b)) {
					backenable |= __rmvq(q, b);
					**mppp = b;
					*mppp = &b->b_next;
					**mppp = NULL;
				}
			}
			while ((b = b_next));
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
 *
 *  MP-STREAMS: Note that qbackenable() will take its own STREAM head read lock making this function
 *  safe to be called from outside of STREAMS.
 */
streams_fastcall void
flushq(queue_t *q, int flag)
{
	bool backenable;
	mblk_t *mp = NULL, **mpp = &mp;
	unsigned long pl;

	assert(q);
	assert(flag == FLUSHDATA || flag == FLUSHALL);

	assert(not_frozen_by_caller(q));

	trace();
	pl = qwlock(q);
	backenable = __flushq(q, flag, &mpp);
	qwunlock(q, pl);
	trace();
	if (backenable) {
		trace();
		qbackenable(q);
	}
	trace();
	/* we want to free messages with the locks off so that other CPUs can process this queue
	   and we don't block interrupts too long */
	mb();
	if (mp)
		freechain(mp, mpp);
	trace();
}

EXPORT_SYMBOL(flushq);		/* include/sys/streams/stream.h */

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
unsigned long
freezestr(queue_t *q)
{
	struct stdata *sd;
	
	assert(q);
	sd = qstream(q);
	assert(sd);

	return zwlock(sd);
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
 *  IMPLEMENTATION: A slight variation on the SVR 4 QWANTR theme: getq() sets QB_WANTR flag in bands
 *  as well when it did not find a message of a higher priority band.  This indicates to putq(),
 *  putbq() and insq() finer control of enabling on priority band messages.
 *
 *  RETURN VALUE: Returns a pointer to the next message, removed from the queue, or NULL if there is
 *  no message on the queue.
 */
static mblk_t *
__getq(queue_t *q, bool * be)
{
	mblk_t *mp;

	if ((mp = q->q_first))
		*be = __rmvq(q, mp);
	else {
		struct qband *qb;

		*be = false;
		if (!test_and_set_bit(QWANTR_BIT, &q->q_flag))
			*be = true;
		/* wanted to read all bands */
		for (qb = q->q_bandp; qb; qb = qb->qb_next)
			if (!test_and_set_bit(QB_WANTR_BIT, &qb->qb_flag))
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
 *  MP-STREAMS: Note that qbackenable() will take its own STREAM head read lock making this function
 *  safe to be called from outside of STREAMS.
 */
streams_fastcall mblk_t *
getq(queue_t *q)
{
	mblk_t *mp;
	bool backenable = false;
	unsigned long pl;

	assert(q);

	assure(not_frozen_by_caller(q));

	pl = qwlock(q);
	mp = __getq(q, &backenable);
	qwunlock(q, pl);
	if (backenable)
		qbackenable(q);
	return (mp);
}

EXPORT_SYMBOL(getq);

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
	/* ingnore message class for insq() */
	enable = ((q->q_first == emp)
		  || test_bit(QWANTR_BIT, &q->q_flag)) ? 1 : 0;
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
			// nmp->b_bandp = qb;
			enable = ((q->q_first == emp)
				  || test_bit(QWANTR_BIT, &q->q_flag)
				  || test_bit(QB_WANTR_BIT, &qb->qb_flag)) ? 1 : 0;
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
	nmp->b_queue = ctrace(qget(q));
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
	nmp->b_size = size;
	return (1 + enable);	/* success */
      bug:
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
 *
 *  CONTEXT: Any, but frozen by the caller.
 *
 *  LOCKING: The caller must lock the queue with MPSTR_QLOCK() or freezestr() across the call.
 */
streams_fastcall int
insq(queue_t *q, mblk_t *emp, mblk_t *nmp)
{
	int result;
	unsigned long pl;

	assert(q);
	assert(nmp);

	assert(frozen_by_caller(q));

	pl = qwlock(q);
	result = __insq(q, emp, nmp);
	qwunlock(q, pl);
	switch (result) {
	case 3:		/* success - high priority enable */
		assert(0);
		qenable(q);
		return (1);
	case 2:		/* success - enable if not noenabled */
		enableq(q);
	case 1:		/* success - don't enable */
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
__STRUTIL_EXTERN_INLINE queue_t *OTHERQ(queue_t *q);

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
 *  CONTEXT: Any. putnext() takes a Stream head plumb read lock so that the function can be called
 *  from outside of STREAMS context.  Caller is responsible for the validity of the q pointer across
 *  the call.  The put() procedure of the next queue will be executed in the same context as
 *  putnext() was called.
 *
 */
streams_fastcall void
putnext(queue_t *q, mblk_t *mp)
{
	struct stdata *sd;
	queue_t *q_next;

	assert(mp);
	assert(q);
	assert(q->q_next);

	sd = qstream(q);

	assert(sd);

	prlock(sd);
	q_next = q->q_next;

	assert(q_next);

	ctrace(put(q->q_next, mp));
	trace();

	prunlock(sd);

	trace();
}

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
			enable = test_bit(QWANTR_BIT, &q->q_flag) ? 1 : 0;
		      hipri:
			if ((q->q_count += (mp->b_size = msgsize(mp))) > q->q_hiwat)
				set_bit(QFULL_BIT, &q->q_flag);
			// mp->b_bandp = NULL;
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
			mp->b_queue = ctrace(qget(q));
			return (1 + enable);
		} else {
			struct qband *qb;

			if (unlikely((qb = __get_qband(q, mp->b_band)) == NULL))
				return (0);
			enable = (test_bit(QWANTR_BIT, &q->q_flag)
				  || test_bit(QB_WANTR_BIT, &qb->qb_flag)) ? 1 : 0;
			if (qb->qb_last == b_prev)
				qb->qb_last = mp;
			if (qb->qb_first == b_next)
				qb->qb_first = mp;
			qb->qb_msgs++;
			if ((qb->qb_count += (mp->b_size = msgsize(mp))) > qb->qb_hiwat)
				if (!test_and_set_bit(QB_FULL_BIT, &qb->qb_flag))
					++q->q_blocked;
			// mp->b_bandp = bget(qb);
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

/**
 *  putbq:	- put a message back on a queue
 *  @q:		queue to place back message
 *  @mp:	message to place back
 */
streams_fastcall int
putbq(queue_t *q, mblk_t *mp)
{
	int result;
	unsigned long pl;

	assert(q);
	assert(mp);

	assert(not_frozen_by_caller(q));

	pl = qwlock(q);
	result = __putbq(q, mp);
	qwunlock(q, pl);
	switch (result) {
	case 3:		/* success - high priority enable */
		qenable(q);
		return (1);
	case 2:		/* success - enable if not noenabled */
		enableq(q);
	case 1:		/* success - don't enable */
		return (1);
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
}

EXPORT_SYMBOL(putbq);

/**
 *  putctl:	- put a control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 */
int
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

EXPORT_SYMBOL(putctl);

/**
 *  putctl1:	- put a 1-byte control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 *  @param:	the 1 byte parameter
 */
int
putctl1(queue_t *q, int type, int param)
{
	mblk_t *mp;

	assert(q);
	if (ctlmsg(type) && (mp = allocb(1, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param;
		put(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL(putctl1);

/**
 *  putctl2:	- put a 2-byte control message to a queue
 *  @q:		the queue to put to
 *  @type:	the message type
 *  @param1:	the first 1 byte parameter
 *  @param2:	the second 1 byte parameter
 */
int
putctl2(queue_t *q, int type, int param1, int param2)
{
	mblk_t *mp;

	assert(q);
	if (ctlmsg(type) && (mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param1;
		*mp->b_wptr++ = (unsigned char) param2;
		put(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL(putctl2);

/**
 *  putnextctl:	- put a control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 */
int
putnextctl(queue_t *q, int type)
{
	mblk_t *mp;

	assert(q);
	assert(q->q_next);
	if (ctlmsg(type) && (mp = allocb(0, BPRI_HI))) {
		mp->b_datap->db_type = type;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL(putnextctl);

/**
 *  putnextctl1: - put a 1-byte control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 *  @param:	the 1 byte parameter
 */
int
putnextctl1(queue_t *q, int type, int param)
{
	mblk_t *mp;

	assert(q);
	assert(q->q_next);
	if (ctlmsg(type) && (mp = allocb(1, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL(putnextctl1);

/**
 *  putnextctl2: - put a 2-byte control message to the queue after this one
 *  @q:		this queue
 *  @type:	the message type
 *  @param1:	the first 1 byte parameter
 *  @param2:	the second 1 byte parameter
 */
int
putnextctl2(queue_t *q, int type, int param1, int param2)
{
	mblk_t *mp;

	assert(q);
	assert(q->q_next);
	if (ctlmsg(type) && (mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = type;
		*mp->b_wptr++ = (unsigned char) param1;
		*mp->b_wptr++ = (unsigned char) param2;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

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
		/* enable if will be first message in queue, or requested by getq() */
		enable = ((q->q_first == b_next)
			  || test_bit(QWANTR_BIT, &q->q_flag)) ? 1 : 0;
	      hipri:
		if ((q->q_count += (mp->b_size = msgsize(mp))) > q->q_hiwat)
			set_bit(QFULL_BIT, &q->q_flag);
		// mp->b_bandp = NULL;
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
		mp->b_queue = ctrace(qget(q));
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
		while (unlikely(b_next && b_next->b_band > mp->b_band)) {
			b_prev = b_next;
			b_next = b_prev->b_next;
		}
		/* enable if will be first message in queue, or requested by getq() */
		enable = ((q->q_first == b_next)
			  || test_bit(QWANTR_BIT, &q->q_flag)
			  || test_bit(QB_WANTR_BIT, &qb->qb_flag)) ? 1 : 0;
		if (qb->qb_last == b_prev)
			qb->qb_last = mp;
		if (qb->qb_first == b_next)
			qb->qb_first = mp;
		qb->qb_msgs++;
		if ((qb->qb_count += (mp->b_size = msgsize(mp))) > qb->qb_hiwat)
			if (!test_and_set_bit(QB_FULL_BIT, &qb->qb_flag))
				++q->q_blocked;
		// mp->b_bandp = bget(qb);
		goto banded;
	}
}

/**
 *  putq:	- put a message block to a queue
 *  @q:		queue to which to put the message
 *  @mp:	message to put
 *
 *  CONTEXT: Any.  It is safe to call this function directly from an ISR to place messages on a
 *  driver's lowest read queue.  Should not be frozen by the caller.
 */
streams_fastcall int
putq(queue_t *q, mblk_t *mp)
{
	int result;
	unsigned long pl;

	assert(q);
	assert(mp);

	assure(not_frozen_by_caller(q));

	pl = qwlock(q);
	result = __putq(q, mp);
	qwunlock(q, pl);
	switch (result) {
	case 3:		/* success - high priority enable */
		qenable(q);
		return (1);
	case 2:		/* success - enable if not noenabled */
		enableq(q);
	case 1:		/* success - don't enable */
		return (1);
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
}

EXPORT_SYMBOL(putq);

static int __setsq(queue_t *q, struct fmodsw *fmod);
static void __setq(queue_t *q, struct qinit *rinit, struct qinit *winit);

/**
 *  qalloc: - allocate and initialize a queue pair
 *  @sd:	STREAM head to which the queue pair belongs
 *  @fmod:	STREAMS module to which the queue pair belongs
 *
 *  Allocates and initializes a queue pair for use by STREAMS.
 */
STATIC inline streams_fastcall queue_t *
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
			qu->qu_str = sd_get(sd);
		} else {
			(q + 0)->q_flag = QUSE | QREADR;
			(q + 1)->q_flag = QUSE;
			ctrace(qput(&q));
			assert(q == NULL);
		}
	}
	return (q);
}

int setsq(queue_t *q, struct fmodsw *fmod);
void setq(queue_t *q, struct qinit *rinit, struct qinit *winit);

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
 *  STREAM and then calls qopen() to call the qi_qopen() procedure of the module or driver.
 *
 *  CONTEXT:  Must only be called from stream head or qopen()/qclose() procedures.
 *
 *  LOCKING:  Must be called with no locks held. The call to qalloc() might sleep.
 *
 *  NOTICES: "Magic Garden" says that if we are opening and the major device number returned from
 *  qopen() is not the same as the major number passed, that we need to do a setq on the queue from
 *  the streamtab associated with the new major device number.
 */
streams_fastcall int
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
			cdrv_put(cdev);
			err = 0;
		}
		*devp = dev;
	}
	qprocson(q);		/* in case qopen() forgot */
	return (0);
      put_noent:
	cdrv_put(cdev);
      enoent:
	qclose(q, oflag, crp);	/* need to call close */
      qerror:
	qprocsoff(q);		/* in case qopen called qprocson, yet returned an error */
	qdelete(q);		/* half delete */
	ctrace(qput(&q));
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
 *  NOTICES: rq should have already been removed from a queue with qprocsoff() (but check again
 *  anyway) and must be a valid pointer or bad things will happen.
 *
 *  Don't do gets and puts on the STREAM head when adding or removing queue pairs from the stream
 *  because the STREAM head reference count falling to zero is used to deallocate the STREAM head
 *  queue pair.
 */
void
qdelete(queue_t *q)
{
	struct stdata *sd;
	queue_t *rq;
	queue_t *wq;
	unsigned long pl;

	assert(q);

	rq = (q + 0);
	wq = (q + 1);

	sd = qstream(q);

	assert(sd);

	ptrace(("final half-delete of stream %p queue pair %p\n", sd, q));

	pl = pwlock(sd);
	ctrace(qput(&rq->q_next));
	ctrace(qput(&wq->q_next));
	ctrace(pwunlock(sd, pl));

	printd(("%s: cancelling initial allocation reference queue pair %p\n", __FUNCTION__, q));
	ctrace(qput(&q));	/* cancel initial allocation reference */
	trace();
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
 *  It is the responsibility of the module qi_qclose() procedure to call qprocsoff() before
 *  returning; however, many modules do not, so we use the QPROCS flag in the queue pairs to
 *  determine whether a qprocsoff() has been called.  qprocsoff() half-deletes the queue pair from
 *  the STREAM under STREAM head write lock.  The call to qdelete() completes this operation and
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
streams_fastcall int
qdetach(queue_t *q, int flags, cred_t *crp)
{
	int err;

	assert(q);

	ptrace(("detaching stream %p queue pair %p\n", qstream(q), q));

	err = ctrace(qclose(q, flags, crp));
	ctrace(qprocsoff(q));		/* in case qclose forgot */
	ctrace(qdelete(q));		/* half delete */
	trace();
	return (err);
}

EXPORT_SYMBOL(qdetach);

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
 *  LOCKING:  qinsert() is called with no locks held.  The function takes a read lock on the STREAM
 *  head to protect queue pointers from weldq() and unweldq() and other operations that manipulate
 *  queue pointers outside of the qattach() and qdetach() procedures called when a queue is opened
 *  or closed.  Only one qinsert() can occur at a time for a given queue, because the STREAM head
 *  holds the STWOPEN bit accross the call.
 *
 *  Because this is used to insert modules under the stream head, a driver can only be permitted to
 *  modify its downward queue pointer (WR(q)->q_next), and then only from its qi_qopen() procedure
 *  before qprocson() is called, from its qi_qclose() procedure after qprocsoff() is called, or
 *  using weldq() or unweldq().
 */
void
qinsert(struct stdata *sd, queue_t *irq)
{
	queue_t *iwq, *srq, *swq;

	ptrace(("initial  half-insert of stream %p queue pair %p\n", sd, irq));

	prlock(sd);
	srq = sd->sd_rq;
	iwq = OTHERQ(irq);
	swq = OTHERQ(srq);
	irq->q_next = ctrace(qget(srq));
	if (swq->q_next != srq) {	/* not a fifo */
		iwq->q_next = ctrace(qget(swq->q_next));
	} else {		/* is a fifo */
		iwq->q_next = ctrace(qget(irq));
	}
	prunlock(sd);
}

EXPORT_SYMBOL(qinsert);

/**
 *  qprocsoff:	- turn off qi_putp and qi_srvp procedures for a queue pair
 *  @q:		read queue pointer for the queue pair to turn procs off
 *
 *  qprocsoff() marks the queue pair as being owned by the STREAM head (disabling further put
 *  procedures), marks it as being noenabled, (which disables further srv procedures), and bypasses
 *  the module by adjusting the q->q_next pointers of upstream modules for each queue in the queue
 *  pair.  This effectively bypasses the module.
 *
 *  Context: qprocsoff() should only be called from qattach() or a STREAM head head open procedure.
 *  The user should call qprocsoff() from the qclose() procedure before returning.
 *
 *  Notices: qprocsoff() does not fully delete the queue pair from the STREAM.  It is still
 *  half-attached.  Use qdelete() to complete the final removal of the queue pair from the STREAM.
 *
 *  Cache the packet sizes of the queue below the STREAM head in the sd_minpsz and sd_maxpsz member.
 *  This saves a little pointer dereferencing in the STREAM head later.
 *
 *  Locking: The modules's qclose() procedure is called with no STREAM head locks held.  Before
 *  unlinking the queue pair, qprocsoff() takes a write lock on the STREAM head.  This means that
 *  all queue synchronous procedures must exit before the lock is acquired.  The STREAM head is
 *  holding the STRCLOSE bit, so no other close can occur, and all other operations on the STREAM
 *  will fail.
 */
void
qprocsoff(queue_t *q)
{
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);

	assert(current_context() <= CTX_STREAMS);
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

			for (qb = rq->q_bandp; qb; qb = qb->qb_next) {
				clear_bit(QB_WANTR_BIT, &qb->qb_flag);
				clear_bit(QB_WANTW_BIT, &qb->qb_flag);
			}
			for (qb = wq->q_bandp; qb; qb = qb->qb_next) {
				clear_bit(QB_WANTR_BIT, &qb->qb_flag);
				clear_bit(QB_WANTW_BIT, &qb->qb_flag);
			}
		}

		ptrace(("initial half-delete of stream %p queue pair %p\n", sd, q));

		/* bypass this module: works for pipe, FIFO and other STREAM heads queues too */
		if ((bq = backq(rq))) {
			ctrace(qput(&bq->q_next));
			bq->q_next = ctrace(qget(rq->q_next));
		}
		if ((bq = backq(wq))) {
			ctrace(qput(&bq->q_next));
			bq->q_next = ctrace(qget(wq->q_next));
		}
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

EXPORT_SYMBOL(qprocsoff);

/**
 *  qprocson:	- trun on qi_putp and qi_srvp procedure for a queeu pair
 *  @q:		read queue pointer for the queue pair to turn procs on
 *
 *  qprocson() marks the queue pair as being not owned by the STREAM head (enabling put procedures),
 *  marks it as being enabled (enabling srv procedures), and intalls the module by adjusting the
 *  q->q_next pointers of the upstream modules for each queue in the queue pair.  This effectively
 *  undoes the bypass created by qprocsoff().
 *
 *  Context: qprocson() should only be called from qattach(), qdetach() or a STREAM head open
 *  procedure.  The user should call qprocson() from the qopen() procedure before returning.
 *
 *  Notices: qprocson() fully inserts the queue pair into the STREAM.  It must be half-inserted with
 *  qinsert() before qprocson() can be called.
 *
 *  Cache the packet sizes of the queue below the STREAM head in the sd_minpsz and sd_maxpsz member.
 *  This saves a little pointer dereferencing in the STREAM head later.
 *
 *  Locking:  The module's qopen() procedure is called with no STREAM head locks held.  Before
 *  linking the queue pair in, qprocson() takes a write lock on the STREAM head.  This means that
 *  all queue synchronous procedures must exit before the lock is acquired.  The STREAM head is
 *  holding STWOPEN bit, so no other open can occur.  Because the STREAM head has not yet been
 *  published to a file pointer or inode, no other operation can occur on the STREAM.
 */
void
qprocson(queue_t *q)
{
	queue_t *bq;
	queue_t *rq = (q + 0);
	queue_t *wq = (q + 1);

	assert(current_context() <= CTX_STREAMS);
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
		{
			struct qband *qb;

			for (qb = rq->q_bandp; qb; qb = qb->qb_next)
				set_bit(QB_WANTR_BIT, &qb->qb_flag);
			for (qb = wq->q_bandp; qb; qb = qb->qb_next)
				set_bit(QB_WANTR_BIT, &qb->qb_flag);
		}

		/* join this module: works for FIFOs and PIPEs too */
		if ((bq = backq(rq))) {
			ctrace(qput(&bq->q_next));
			bq->q_next = ctrace(qget(rq));
		}
		if ((bq = backq(wq))) {
			ctrace(qput(&bq->q_next));
			bq->q_next = ctrace(qget(wq));
		}
		/* cache new packet sizes (this module) */
		sd->sd_minpsz = wq->q_minpsz;
		sd->sd_maxpsz = wq->q_maxpsz;

		pwunlock(sd, pl);
	}
}

EXPORT_SYMBOL(qprocson);

/**
 *  qreply:	- reply with a message
 *  @q:		queue from which to reply
 *  @mp:	message reply
 */
__STRUTIL_EXTERN_INLINE void qreply(queue_t *q, mblk_t *mp);

EXPORT_SYMBOL(qreply);

/**
 *  qsize:	- calculate number of messages on a queue
 *  @q:		queue to count messages
 */
__STRUTIL_EXTERN_INLINE ssize_t qsize(queue_t *q);

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
 *  LOCKING:  Take a Stream head plumb read lock to protect q_next pointers while walking the
 *  Stream.
 */
streams_fastcall ssize_t
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

EXPORT_SYMBOL(qcountstrm);

/**
 *  RD:		- find read queue from write queu
 *  @q:		write queue pointer
 */
__STRUTIL_EXTERN_INLINE queue_t *RD(queue_t *q);

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
 *
 *  IMPLEMENTATION: A slight variation on the them of SVR QWANTR, we also set and clear QB_WANTR for
 *  queue bands.  When a message is removed from the queue, the QWANTR flag is reset, and the
 *  QB_WANTR flag is reset for the band retrieved and all lower priority bands.  The QB_WANTR flag
 *  is set for all the priority bands higher than the message retrieved.  This indicates to putq(),
 *  putbq() and insq() when to enable the queue on priority band messages being added to the queue.
 *
 *  This way, if a priority band message is placed back on the queue with putbq() due to flow
 *  control restrictions, another priority band message of the same or lower priority being enqueued
 *  will not enable the queue, however, a higher priority message will.
 */
bool
__rmvq(queue_t *q, mblk_t *mp)
{
	bool backenable = false;
	mblk_t *b_next, *b_prev;

	assert(q);
	assert(mp);

	/* We know which queue the message belongs too, make sure its still there. */
	assert(q == mp->b_queue);

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
	if (likely(mp->b_band == 0)) {
		if (mp->b_datap->db_type < QPCTL) {
			struct qband *qb;

			/* wanted to read all bands other than band zero */
			for (qb = q->q_bandp; qb;
			     set_bit(QB_WANTR_BIT, &qb->qb_flag), qb = qb->qb_next) ;
		}
		q->q_count -= mp->b_size;
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
#if 0
		struct qband *qb = mp->b_bandp;

		bput(&mp->b_bandp);
#else
		struct qband *qb;

		{
			unsigned char q_nband, band;

			/* wanted to read all bands of higher priority than this */
			for (band = mp->b_band, q_nband = q->q_nband, qb = q->q_bandp;
			     qb && q_nband > band;
			     set_bit(QB_WANTR_BIT, &qb->qb_flag), qb = qb->qb_next, --q_nband) ;
		}
#endif
		assert(qb);
		if (qb->qb_first == mp)
			qb->qb_first = b_next;
		if (qb->qb_last == mp)
			qb->qb_last = b_prev;
		qb->qb_msgs--;
		qb->qb_count -= mp->b_size;
		if (qb->qb_count == 0 || qb->qb_count < qb->qb_lowat) {
			if (test_and_clear_bit(QB_FULL_BIT, &qb->qb_flag))
				--q->q_blocked;
			if (test_and_clear_bit(QB_WANTW_BIT, &qb->qb_flag))
				backenable = true;
		}
		/* no longer want to read this or lower bands */
		for (; qb; qb = qb->qb_next)
			clear_bit(QB_WANTR_BIT, &qb->qb_flag);
		/* including band zero */
		clear_bit(QWANTR_BIT, &q->q_flag);
	}
	ctrace(qput(&mp->b_queue));
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
 *  MP-STREAMS: Note that qbackenable() will take its own STREAM head read lock making this function
 *  safe to be called from outside of STREAMS.
 *
 *  LOCKING: We take our own write locks to protect the queue structure in case the caller has not.
 *
 *  NOTICES: rmvq() panics when passed null pointers.  rmvq() panics if a write lock has not been
 *  taken on the queue.  rmvq() panics if the message is not a queue, or not on the specified queue.
 */
streams_fastcall void
rmvq(queue_t *q, mblk_t *mp)
{
	bool backenable;
	unsigned long pl;

	assert(q);
	assert(mp);

	assure(frozen_by_caller(q));

	pl = qwlock(q);

	assert(mp->b_queue);
	assert(q == mp->b_queue);

	backenable = __rmvq(q, mp);

	qwunlock(q, pl);

	/* Backenabling under locks is not so severe now that write locks only suppress soft irq. */
	if (backenable)
		qbackenable(q);
}

EXPORT_SYMBOL(rmvq);

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
void
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

EXPORT_SYMBOL(setq);

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
static int
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
	return (-ENOMEM);
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
int
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

EXPORT_SYMBOL(setsq);		/* for stream head include/sys/streams/strsubr.h */

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
	unsigned long pl;

	assert(frozen_by_caller(q));

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

EXPORT_SYMBOL(strqget);

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
int
strqset(queue_t *q, qfields_t what, unsigned char band, long val)
{
	int err = 0;
	unsigned long pl;

	assert(frozen_by_caller(q));

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

EXPORT_SYMBOL(strqset);

static spinlock_t str_err_lock = SPIN_LOCK_UNLOCKED;
static char str_err_buf[LOGMSGSZ];

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
	struct stdata *sd;
	
	assert(q);
	sd = qstream(q);
	assert(sd);

	(void) flags;
	zwunlock(sd, flags);
}

EXPORT_SYMBOL(unfreezestr);

/**
 *  WR:		- get write queue in queue pair
 *  @q:		read queue pointer
 */
__STRUTIL_EXTERN_INLINE queue_t *WR(queue_t *q);

EXPORT_SYMBOL(WR);

/*
 *  vcmn_err:
 */
void
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

__STRUTIL_EXTERN_INLINE int copyin(const void *from, void *to, size_t len);

EXPORT_SYMBOL(copyin);

__STRUTIL_EXTERN_INLINE int copyout(const void *from, void *to, size_t len);

EXPORT_SYMBOL(copyout);

__STRUTIL_EXTERN_INLINE void delay(unsigned long ticks);

EXPORT_SYMBOL(delay);

__STRUTIL_EXTERN_INLINE int drv_getparm(const unsigned int parm, void *value_p);

EXPORT_SYMBOL(drv_getparm);

__STRUTIL_EXTERN_INLINE unsigned long drv_hztomsec(unsigned long hz);

EXPORT_SYMBOL(drv_hztomsec);

__STRUTIL_EXTERN_INLINE unsigned long drv_hztousec(unsigned long hz);

EXPORT_SYMBOL(drv_hztousec);

__STRUTIL_EXTERN_INLINE unsigned long drv_msectohz(unsigned long msec);

EXPORT_SYMBOL(drv_msectohz);

__STRUTIL_EXTERN_INLINE int drv_priv(cred_t *crp);

EXPORT_SYMBOL(drv_priv);

__STRUTIL_EXTERN_INLINE unsigned long drv_usectohz(unsigned long usec);

EXPORT_SYMBOL(drv_usectohz);

__STRUTIL_EXTERN_INLINE void drv_usecwait(unsigned long usec);

EXPORT_SYMBOL(drv_usecwait);

__STRUTIL_EXTERN_INLINE major_t getmajor(dev_t dev);

EXPORT_SYMBOL(getmajor);

__STRUTIL_EXTERN_INLINE minor_t getminor(dev_t dev);

EXPORT_SYMBOL(getminor);

__STRUTIL_EXTERN_INLINE dev_t makedevice(major_t major, minor_t minor);

EXPORT_SYMBOL(makedevice);
