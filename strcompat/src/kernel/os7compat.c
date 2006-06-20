/*****************************************************************************

 @(#) $RCSfile: os7compat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2006/06/19 20:51:27 $

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

 Last Modified $Date: 2006/06/19 20:51:27 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: os7compat.c,v $
 Revision 0.9.2.17  2006/06/19 20:51:27  brian
 - more optimizations

 Revision 0.9.2.16  2006/06/18 20:54:02  brian
 - minor optimizations from profiling

 Revision 0.9.2.15  2006/06/14 10:37:18  brian
 - defeat a lot of debug traces in debug mode for testing
 - changes to allow strinet to compile under LiS (why???)

 Revision 0.9.2.14  2006/05/25 08:39:08  brian
 - added noinline in strategic places

 Revision 0.9.2.13  2006/05/24 10:50:25  brian
 - optimizations

 Revision 0.9.2.12  2006/04/24 05:00:47  brian
 - call interface corrections

 Revision 0.9.2.11  2006/04/23 18:11:05  brian
 - minor corrections

 Revision 0.9.2.11  2006/04/23 18:08:19  brian
 - minor corrections

 Revision 0.9.2.10  2006/04/22 01:05:36  brian
 - updated headers and optimitization

 Revision 0.9.2.9  2006/04/18 17:54:43  brian
 - added some strategic prefetches

 Revision 0.9.2.8  2006/03/30 10:45:48  brian
 - rationalized to working drivers

 Revision 0.9.2.7  2005/12/22 10:28:54  brian
 - no symbol mangling for 2.4 kernels

 Revision 0.9.2.6  2005/12/19 12:44:41  brian
 - locking down for release

 Revision 0.9.2.5  2005/12/19 03:26:02  brian
 - wend for simple streamscall

 Revision 0.9.2.4  2005/12/17 08:39:21  brian
 - modifications for master build

 Revision 0.9.2.3  2005/07/18 12:25:42  brian
 - standard indentation

 Revision 0.9.2.2  2005/07/13 12:01:49  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 0.9.2.1  2005/07/12 13:54:46  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ident "@(#) $RCSfile: os7compat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2006/06/19 20:51:27 $"

static char const ident[] =
    "$RCSfile: os7compat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2006/06/19 20:51:27 $";

/* 
 *  This is my solution for those who don't want to inline GPL'ed functions or
 *  who don't use optimizations when compiling or specifies
 *  -fnoinline-functions or something of the like.  This file implements all
 *  of the extern inlines from the header files by just including the header
 *  files with the functions declared 'inline' instead of 'extern inline'.
 *
 *  There are implemented here in a separate object, out of the way of the
 *  modules that don't use them.
 */

#define __OS7_EXTERN_INLINE inline

#define _OS7_SOURCE

#include "sys/os7/compat.h"

#define OS7COMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define OS7COMP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define OS7COMP_REVISION	"LfS $RCSfile: os7compat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2006/06/19 20:51:27 $"
#define OS7COMP_DEVICE		"OpenSS7 Compatibility"
#define OS7COMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define OS7COMP_LICENSE		"GPL"
#define OS7COMP_BANNER		OS7COMP_DESCRIP		"\n" \
				OS7COMP_COPYRIGHT	"\n" \
				OS7COMP_REVISION	"\n" \
				OS7COMP_DEVICE		"\n" \
				OS7COMP_CONTACT		"\n"
#define OS7COMP_SPLASH		OS7COMP_DEVICE		" - " \
				OS7COMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_OS7_MODULE
MODULE_AUTHOR(OS7COMP_CONTACT);
MODULE_DESCRIPTION(OS7COMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(OS7COMP_DEVICE);
MODULE_LICENSE(OS7COMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-os7compat");
#endif
#endif

__OS7_EXTERN_INLINE void streamscall ss7_bufsrv(long data);

EXPORT_SYMBOL_NOVERS(ss7_bufsrv);
__OS7_EXTERN_INLINE streamscall void ss7_unbufcall(str_t * s);

EXPORT_SYMBOL_NOVERS(ss7_unbufcall);
__OS7_EXTERN_INLINE streamscall void ss7_bufcall(queue_t *q, size_t size, int prior);

EXPORT_SYMBOL_NOVERS(ss7_bufcall);
__OS7_EXTERN_INLINE streamscall void ss7_esbbcall(queue_t *q, int prior);

EXPORT_SYMBOL_NOVERS(ss7_esbbcall);
__OS7_EXTERN_INLINE streamscall mblk_t *ss7_allocb(queue_t *q, size_t size, int prior);

EXPORT_SYMBOL_NOVERS(ss7_allocb);
__OS7_EXTERN_INLINE streamscall mblk_t *ss7_esballoc(queue_t *q, unsigned char *base, size_t size,
						     int prior, frtn_t *frtn);
EXPORT_SYMBOL_NOVERS(ss7_esballoc);
__OS7_EXTERN_INLINE streamscall int ss7_pullupmsg(queue_t *q, mblk_t *mp, int size);

EXPORT_SYMBOL_NOVERS(ss7_pullupmsg);
__OS7_EXTERN_INLINE streamscall mblk_t *ss7_dupb(queue_t *q, mblk_t *bp);

EXPORT_SYMBOL_NOVERS(ss7_dupb);
__OS7_EXTERN_INLINE streamscall mblk_t *ss7_dupmsg(queue_t *q, mblk_t *bp);

EXPORT_SYMBOL_NOVERS(ss7_dupmsg);
__OS7_EXTERN_INLINE streamscall mblk_t *ss7_copyb(queue_t *q, mblk_t *bp);

EXPORT_SYMBOL_NOVERS(ss7_copyb);
__OS7_EXTERN_INLINE streamscall mblk_t *ss7_copymsg(queue_t *q, mblk_t *bp);

EXPORT_SYMBOL_NOVERS(ss7_copymsg);

__OS7_EXTERN_INLINE streamscall mblk_t *__ss7_fast_allocb(struct ss7_bufpool *pool, size_t size,
							  int prior);

EXPORT_SYMBOL_NOVERS(__ss7_fast_allocb);
__OS7_EXTERN_INLINE streamscall mblk_t *ss7_fast_allocb(struct ss7_bufpool *pool, size_t size,
							int prior);

EXPORT_SYMBOL_NOVERS(ss7_fast_allocb);
__OS7_EXTERN_INLINE streamscall mblk_t *ss7_fast_allocb_bh(struct ss7_bufpool *pool, size_t size,
							   int prior);

EXPORT_SYMBOL_NOVERS(ss7_fast_allocb_bh);
__OS7_EXTERN_INLINE streamscall void __ss7_fast_freeb(struct ss7_bufpool *pool, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(__ss7_fast_freeb);
__OS7_EXTERN_INLINE streamscall void ss7_fast_freeb(struct ss7_bufpool *pool, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(ss7_fast_freeb);
__OS7_EXTERN_INLINE streamscall void ss7_fast_freeb_bh(struct ss7_bufpool *pool, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(ss7_fast_freeb_bh);
__OS7_EXTERN_INLINE streamscall void __ss7_fast_freemsg(struct ss7_bufpool *pool, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(__ss7_fast_freemsg);
__OS7_EXTERN_INLINE streamscall void ss7_fast_freemsg(struct ss7_bufpool *pool, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(ss7_fast_freemsg);
__OS7_EXTERN_INLINE streamscall void ss7_fast_freemsg_bh(struct ss7_bufpool *pool, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(ss7_fast_freemsg_bh);
__OS7_EXTERN_INLINE void ss7_bufpool_init(struct ss7_bufpool *pool);

EXPORT_SYMBOL_NOVERS(ss7_bufpool_init);
__OS7_EXTERN_INLINE void ss7_bufpool_reserve(struct ss7_bufpool *pool, int n);

EXPORT_SYMBOL_NOVERS(ss7_bufpool_reserve);
__OS7_EXTERN_INLINE void ss7_bufpool_release(struct ss7_bufpool *pool, int n);

EXPORT_SYMBOL_NOVERS(ss7_bufpool_release);
__OS7_EXTERN_INLINE void ss7_bufpool_term(struct ss7_bufpool *pool);

EXPORT_SYMBOL_NOVERS(ss7_bufpool_term);

__OS7_EXTERN_INLINE void bufq_init(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_init);
__OS7_EXTERN_INLINE streamscall void bufq_lock(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_lock);
__OS7_EXTERN_INLINE streamscall void bufq_unlock(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_unlock);
__OS7_EXTERN_INLINE streamscall size_t bufq_length(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_length);
__OS7_EXTERN_INLINE streamscall size_t bufq_size(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_size);
__OS7_EXTERN_INLINE streamscall mblk_t *bufq_head(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_head);
__OS7_EXTERN_INLINE streamscall mblk_t *bufq_tail(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_tail);
__OS7_EXTERN_INLINE void __bufq_add(bufq_t * q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(__bufq_add);
__OS7_EXTERN_INLINE void __bufq_sub(bufq_t * q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(__bufq_sub);
__OS7_EXTERN_INLINE void __bufq_queue(bufq_t * q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(__bufq_queue);
__OS7_EXTERN_INLINE streamscall void bufq_queue(bufq_t * q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(bufq_queue);
__OS7_EXTERN_INLINE streamscall void bufq_queue_head(bufq_t * q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(bufq_queue_head);
__OS7_EXTERN_INLINE streamscall void bufq_insert(bufq_t * q, mblk_t *mp, mblk_t *np);

EXPORT_SYMBOL_NOVERS(bufq_insert);
__OS7_EXTERN_INLINE streamscall void bufq_append(bufq_t * q, mblk_t *mp, mblk_t *np);

EXPORT_SYMBOL_NOVERS(bufq_append);
__OS7_EXTERN_INLINE mblk_t *__bufq_dequeue(bufq_t * q);

EXPORT_SYMBOL_NOVERS(__bufq_dequeue);
__OS7_EXTERN_INLINE mblk_t *bufq_dequeue(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_dequeue);
__OS7_EXTERN_INLINE mblk_t *__bufq_dequeue_tail(bufq_t * q);

EXPORT_SYMBOL_NOVERS(__bufq_dequeue_tail);
__OS7_EXTERN_INLINE streamscall mblk_t *bufq_dequeue_tail(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_dequeue_tail);
__OS7_EXTERN_INLINE mblk_t *__bufq_unlink(bufq_t * q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(__bufq_unlink);
__OS7_EXTERN_INLINE streamscall mblk_t *bufq_unlink(bufq_t * q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(bufq_unlink);
__OS7_EXTERN_INLINE streamscall void bufq_splice_head(bufq_t * q1, bufq_t * q2);

EXPORT_SYMBOL_NOVERS(bufq_splice_head);
__OS7_EXTERN_INLINE streamscall void bufq_splice_tail(bufq_t * q1, bufq_t * q2);

EXPORT_SYMBOL_NOVERS(bufq_splice_tail);
__OS7_EXTERN_INLINE streamscall void bufq_freehead(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_freehead);
__OS7_EXTERN_INLINE streamscall void bufq_purge(bufq_t * q);

EXPORT_SYMBOL_NOVERS(bufq_purge);
__OS7_EXTERN_INLINE void __bufq_supply(bufq_t * q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(__bufq_supply);
__OS7_EXTERN_INLINE streamscall void bufq_supply(bufq_t * q, mblk_t *mp);

EXPORT_SYMBOL_NOVERS(bufq_supply);
__OS7_EXTERN_INLINE streamscall mblk_t *bufq_resupply(bufq_t * q, mblk_t *mp, int maxsize,
						      int maxcount);

EXPORT_SYMBOL_NOVERS(bufq_resupply);

/**
 * ss7_trylockq: - try to lock a queue pair for exclusive operation
 * @q: queue in queue pair
 *
 * Most of the purpose for these locking arrangements was that LiS had no STREAMS syncrhonization
 * mechanisms, and this approximates queue-pair synchronization.  If this succeeds, the procedure
 * has access exclusive to the private structure.  Note that this approach cannot ever attempt to
 * take the qlock twice in the same thread (leading to single party deadlock).  This, protects even
 * against an implicit recursive put procedure (one that calls a put procedure that calls another
 * that calls the original put procedure).  Locking is strict and safe against interrupts.
 */
INLINE streamscall __hot int
ss7_trylockq(queue_t *q)
{
	int res;
	str_t *s = STR_PRIV(q);
	unsigned long flags;

	spin_lock_irqsave(&s->qlock, flags);
	if (!(res = !s->users++)) {
		if (q == s->iq)
			s->iwait = q;
		if (q == s->oq)
			s->owait = q;
		--s->users;
	}
	spin_unlock_irqrestore(&s->qlock, flags);
	return (res);
}

EXPORT_SYMBOL_NOVERS(ss7_trylockq);

#ifdef LIS
extern int enableq(queue_t *q);
#endif
/**
 * ss7_unlockq: - unlock a queue pair, enabling waiting queues
 * @q: queue whose put or service procedure is running
 *
 * Most of the purpose for these locking arrangement was that LiS had no STREAMS synchronization
 * mechanisms, and this approximates queue-pair synchronization.  This is the reverse of a
 * successful ss7_trylock(), above.  Note that the approach cannot ever attempt to take the qlock
 * twice in the same thread (leading to single party deadlock).  Note also, that if a waiting queue
 * does not have a service procedure, one will still be called directly to drain deferred messages
 * from the queue.
 */
INLINE streamscall __hot void
ss7_unlockq(queue_t *q)
{
	str_t *s = STR_PRIV(q);
	queue_t *wait;
	unsigned long flags;

	int streamscall (*isrv) (queue_t *) = NULL;
	int streamscall (*osrv) (queue_t *) = NULL;

	spin_lock_irqsave(&s->qlock, flags);
	if ((wait = XCHG(&s->iwait, NULL)) && !enableq(wait)) {
		if (wait->q_qinfo && wait->q_qinfo->qi_srvp)
			qenable(wait);
		else
			isrv = &ss7_isrv;
	}
	if ((wait = XCHG(&s->owait, NULL)) && !enableq(wait)) {
		if (wait->q_qinfo && wait->q_qinfo->qi_srvp)
			qenable(wait);
		else
			osrv = &ss7_osrv;
	}
	s->users = 0;
	spin_unlock_irqrestore(&s->qlock, flags);

	if (isrv)
		isrv(s->iq);
	if (osrv)
		osrv(s->iq);
}

EXPORT_SYMBOL_NOVERS(ss7_unlockq);

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */

/**
 * ss7_flushq: - cannonical flushing of a queue in the queue pair
 * @q: queue to flush
 * @mp: the M_FLUSH message
 * @flag: FLUSHR for read queue, FLUSHW for write queue
 *
 * Performs canonical flushing of one queue in the queue pair.  If the corresponding flag is set in
 * the M_FLUSH message, this queue is flushed.  The message is passed along if there is a subsequent
 * queue in the flow and QR_ABSORBED returned, otherwise, the directional flag is cleared and
 * QR_DONE returned.
 */
STATIC INLINE fastcall int
ss7_flushq(queue_t *q, mblk_t *mp, const unsigned char flag)
{
	if (mp->b_rptr[0] & flag) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	if ((mp->b_rptr[0] & (FLUSHR | FLUSHW))) {
		if (q->q_next) {
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		mp->b_rptr[0] &= ~flag;
	}
	return (QR_DONE);
}

/**
 * ss7_w_flush: - canoncial flushing of the WR queue
 * @q: write queue
 * @mp: M_FLUSH message
 *
 * Performs canonical flushing from a write queue.  The mesage is passed along or looped if
 * required and QR_ABSORBED returned, or QR_DONE returned if the message is to be discarded.
 */
noinline streamscall int
ss7_w_flush(queue_t *q, mblk_t *mp)
{
	int rtn;

	if ((rtn = ss7_flushq(q, mp, FLUSHW)) == QR_DONE)
		rtn = ss7_flushq(RD(q), mp, FLUSHR);
	return (rtn);
}

EXPORT_SYMBOL_NOVERS(ss7_w_flush);

/**
 * ss7_r_flush: - canoncial flushing of the RD queue
 * @q: read queue
 * @mp: M_FLUSH message
 *
 * Performs canonical flushing from a read queue.  The mesage is passed along or looped if
 * required and QR_ABSORBED returned, or QR_DONE returned if the message is to be discarded.
 */
noinline streamscall int
ss7_r_flush(queue_t *q, mblk_t *mp)
{
	int rtn;

	if ((rtn = ss7_flushq(q, mp, FLUSHR)) == QR_DONE)
		rtn = ss7_flushq(WR(q), mp, FLUSHW);
	return (rtn);
}

EXPORT_SYMBOL_NOVERS(ss7_r_flush);

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */

static noinline streams_fastcall void
ss7_putq_slow(queue_t *q, mblk_t *mp, int rtn)
{
	switch (rtn) {
	case QR_DONE:
		freemsg(mp);
	case QR_ABSORBED:
		break;
	case QR_STRIP:
		if (mp->b_cont)
			if (unlikely(putq(q, mp->b_cont) == 0))
				freemsg(mp->b_cont);
	case QR_TRIMMED:
		freeb(mp);
		break;
	case QR_LOOP:
		if (!q->q_next) {
			qreply(q, mp);
			break;
		}
	case QR_PASSALONG:
		if (q->q_next) {
			putnext(q, mp);
			break;
		}
		rtn = -EOPNOTSUPP;
	default:
		printd(("%s: %p: ERROR: (q dropping) %d\n", q->q_qinfo->qi_minfo->mi_idname,
			q->q_ptr, rtn));
		freemsg(mp);
		break;
	case QR_DISABLE:
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
		break;
	case QR_PASSFLOW:
		if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			break;
		}
	case -ENOBUFS:
	case -EBUSY:
	case -ENOMEM:
	case -EAGAIN:
	case QR_RETRY:
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
		break;
	}
	return;
}

/**
 * ss7_putq: - canonical put (qi_putp_t) procedure
 * @q: queue to put
 * @mp: message to put
 * @proc: procedure to process the message
 *
 * Notes: M_FLUSH messages are processed without locking and are not queued.  All other normal
 * priority messages are queued if there exists messages on the queue, or if the service procedure
 * is running.  This maintains ordering of normal priority messages, but not high priority messages.
 * Most service interface specifications (NPI, TPI, etc.) require that if the user issues a priority
 * message that it wait for an acknowledgement before issuing any other (priority) messages.  Note
 * also that the Stream head only allows one oustanding high priority message and will discard
 * others arriving at the Stream head.  Priority messages are, therefore, a real one-shot thing.
 *
 * Priority messages other than M_FLUSH will be queued if requested by the message handling
 * procedure, or if locks are in contention on the queue pair.
 *
 * The message processing procedure proc() is invoked with the queue pair locked and the procedure
 * has exclusive access to the queue pair private structure.
 */
streamscall __hot int
ss7_putq(queue_t *q, mblk_t *mp, int streamscall (*proc) (queue_t *, mblk_t *))
{
	int locked = 0;

	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));

	if (likely(mp->b_datap->db_type < QPCTL) && unlikely(q->q_first || q->q_flag & QSVCBUSY)) {
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
		return (0);
	}
	if (likely((locked = ss7_trylockq(q))) || unlikely(mp->b_datap->db_type == M_FLUSH)) {
		int rtn;

		rtn = proc(q, mp);
		/* Fast Paths */
		if (likely(rtn == QR_TRIMMED)) {
			freeb(mp);
		      unlock_exit:
			if (locked)
				ss7_unlockq(q);
			return (0);
		}
		if (likely(rtn == QR_DONE)) {
			freemsg(mp);
			goto unlock_exit;
		}
		ss7_putq_slow(q, mp, rtn);
		goto unlock_exit;
	} else {
		seldom();
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(ss7_putq);

static noinline streams_fastcall int
ss7_srvq_slow(queue_t *q, mblk_t *mp, int rtn)
{
	switch (rtn) {
	case QR_DONE:
		freemsg(mp);
	case QR_ABSORBED:
		return (1);
	case QR_STRIP:
		if (mp->b_cont)
			if (!putbq(q, mp->b_cont))
				freemsg(mp->b_cont);
	case QR_TRIMMED:
		freeb(mp);
		return (1);
	case QR_LOOP:
		if (!q->q_next) {
			qreply(q, mp);
			return (1);
		}
	case QR_PASSALONG:
		if (q->q_next) {
			putnext(q, mp);
			return (1);
		}
		rtn = -EOPNOTSUPP;
	default:
		printd(("%s: %p: ERROR: (q dropping) %d\n", q->q_qinfo->qi_minfo->mi_idname,
			q->q_ptr, rtn));
		freemsg(mp);
		return (1);
	case QR_DISABLE:
		printd(("%s: %p: ERROR: (q disabling) %d\n", q->q_qinfo->qi_minfo->mi_idname,
			q->q_ptr, rtn));
		noenable(q);
		if (!putbq(q, mp))
			freemsg(mp);
		rtn = 0;
		return (0);
	case QR_PASSFLOW:
		if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (1);
		}
	case -ENOBUFS:		/* proc must have scheduled bufcall */
	case -EBUSY:		/* proc must have failed canput */
	case -ENOMEM:		/* proc must have scheduled bufcall */
	case -EAGAIN:		/* proc must re-enable on some event */
		if (mp->b_datap->db_type < QPCTL) {
			printd(("%s: %p: ERROR: (q stalled) %d\n", q->q_qinfo->qi_minfo->mi_idname,
				q->q_ptr, rtn));
			if (!putbq(q, mp))
				freemsg(mp);
			return (0);
		}
		/* 
		 *  Be careful not to put a priority message back on the queue.
		 */
		switch (mp->b_datap->db_type) {
		case M_PCPROTO:
			mp->b_datap->db_type = M_PROTO;
			break;
		case M_PCRSE:
			mp->b_datap->db_type = M_RSE;
			break;
		case M_PCCTL:
			mp->b_datap->db_type = M_CTL;
			break;
		default:
			printd(("%s: %p: ERROR: (q dropping) %d\n", q->q_qinfo->qi_minfo->mi_idname,
				q->q_ptr, rtn));
			freemsg(mp);
			return (1);
		}
		mp->b_band = 255;
		if (unlikely(putq(q, mp) == 0))
			freemsg(mp);
		return (0);
	case QR_RETRY:
		if (!putbq(q, mp))
			freemsg(mp);
		return (1);
	}
}

/**
 * ss7_srvq: - canonical service procedure
 * @q: queue to service
 * @proc: procedure to process messages
 * @procwake: procedure to call after queue is processed
 *
 * Again, much of the locking and flakiness here was to handle LiS poor locking, synchronization,
 * lacking qprocson()/qprocsoff() and other poor behaviour.  Only the synchronization is necessary
 * for running D_MP under Linux Fast-STREAMS.
 */
streamscall __hot int
ss7_srvq(queue_t *q, int streamscall (*proc) (queue_t *, mblk_t *),
	 void streamscall (*procwake) (queue_t *))
{
	int rtn = 0;

	ensure(q, return (-EFAULT));
	if (likely(ss7_trylockq(q))) {
		mblk_t *mp;

		while (likely((mp = getq(q)) != NULL)) {

			prefetch(mp);
#if 0
			prefetch(mp->b_datap);
			prefetch(mp->b_rptr);
			prefetch(mp->b_cont);
#endif

			rtn = proc(q, mp);
			/* Fast Path */
			if (likely(rtn == QR_TRIMMED)) {
				freeb(mp);
				continue;
			}
			if (likely(rtn == QR_DONE)) {
				freemsg(mp);
				continue;
			}
			if (likely(ss7_srvq_slow(q, mp, rtn)))
				continue;
			break;
		}
		/* perform wakeups */
		if (procwake)
			procwake(q);
		ss7_unlockq(q);
		return (0);
	} else {
		rare();
		return (0);
	}
}

EXPORT_SYMBOL_NOVERS(ss7_srvq);

static streams_noinline streams_fastcall int
ss7_oput_slow(str_t * s, mblk_t *mp)
{
	if (likely(s->oq != NULL)) {
		swerr();
		if (s->oq->q_qinfo->qi_srvp && likely(putq(s->oq, mp) != 0))
			return (0);	/* recovered */
	}
	swerr();
	freemsg(mp);
	return (-EFAULT);
}

/**
 * ss7_oput: - canonical put procedure for the output side
 * @q: queue to put
 * @mp: message to put
 *
 * The output queue is a read queue on the upper multiplex or a write queue on the lower multiplex.
 * The checks for the existence of a queue and the existence of a processing routine are to handle
 * the older LiS lack of a proper qprocson()/qprocsoff() mechanism.
 */
streamscall __hot_out int
ss7_oput(queue_t *q, mblk_t *mp)
{
	str_t *s = STR_PRIV(q);

	prefetch(mp);
#if 0
	prefetch(mp->b_datap);
	prefetch(mp->b_rptr);
	prefetch(mp->b_cont);
#endif

	if (likely(s->oq != NULL)) {
		if (likely(s->o_prim != NULL)) {
			ss7_putq(s->oq, mp, s->o_prim);
			return (0);
		}
	}
	return ss7_oput_slow(s, mp);
}

EXPORT_SYMBOL_NOVERS(ss7_oput);

static streams_noinline streams_fastcall int
ss7_osrv_slow(str_t * s, queue_t *q)
{
	if (likely(s->oq != NULL)) {
		if (s->o_wakeup) {
			s->o_wakeup(s->oq);
			return (0);
		}
	}
	swerr();
	noenable(q);
	return (-EFAULT);
}

/**
 * ss7_osrv: - canoncial service procedure for the output side
 * @q: queue to service
 *
 * Note that this function can be called directly even if there is no service procedure.  This is
 * done by the deferral mechanism for bufcalls.  The message is deferred to the queue and this
 * procedure invoked on callback when there is no service procedure to enable.
 */
streamscall __hot_out int
ss7_osrv(queue_t *q)
{
	str_t *s = STR_PRIV(q);

	if (likely(s->oq != NULL)) {
		if (likely(s->o_prim != NULL)) {
			ss7_srvq(s->oq, s->o_prim, s->o_wakeup);
			return (0);
		}
	}
	return ss7_osrv_slow(s, q);
}

EXPORT_SYMBOL_NOVERS(ss7_osrv);

static streams_noinline streams_fastcall int
ss7_iput_slow(str_t * s, mblk_t *mp)
{
	if (likely(s->iq != NULL)) {
		swerr();
		if (s->iq->q_qinfo->qi_srvp && likely(putq(s->iq, mp) != 0))
			return (0);	/* recovered */
	}
	swerr();
	freemsg(mp);
	return (-EFAULT);
}

/**
 * ss7_iput: - canonical put procedure for the input side
 * @q: queue to put
 * @mp: message to put
 *
 * The input queue is a write queue on the upper multiplex or a read queue on the lower multiplex.
 * The checks for the existence of a queue and the existence of a processing routine are to handle
 * the older LiS lack of a proper qprocson()/qprocsoff() mechanism.
 */
streamscall __hot_in int
ss7_iput(queue_t *q, mblk_t *mp)
{
	str_t *s = STR_PRIV(q);

	prefetch(mp);
#if 0
	prefetch(mp->b_datap);
	prefetch(mp->b_rptr);
	prefetch(mp->b_cont);
#endif

	if (likely(s->iq != NULL)) {
		if (likely(s->i_prim != NULL)) {
			ss7_putq(s->iq, mp, s->i_prim);
			return (0);
		}
	}
	return ss7_iput_slow(s, mp);
}

EXPORT_SYMBOL_NOVERS(ss7_iput);

static streams_noinline streams_fastcall int
ss7_isrv_slow(str_t * s, queue_t *q)
{
	if (likely(s->iq != NULL)) {
		if (s->i_wakeup) {
			s->i_wakeup(s->iq);
			return (0);
		}
	}
	swerr();
	noenable(q);
	return (-EFAULT);
}

/**
 * ss7_isrv: - canoncial service procedure for the input side
 * @q: queue to service
 *
 * Note that this function can be called directly even if there is no service procedure.  This is
 * done by the deferral mechanism for bufcalls.  The message is deferred to the queue and this
 * procedure invoked on callback when there is no service procedure to enable.
 */
streamscall __hot_in int
ss7_isrv(queue_t *q)
{
	str_t *s = STR_PRIV(q);

	if (likely(s->iq != NULL)) {
		if (likely(s->i_prim != NULL)) {
			ss7_srvq(s->iq, s->i_prim, s->i_wakeup);
			return (0);
		}
	}
	return ss7_isrv_slow(s, q);
}

EXPORT_SYMBOL_NOVERS(ss7_isrv);

__OS7_EXTERN_INLINE streamscall void ss7_do_timeout(caddr_t data, const char *timer,
						    const char *mod, ulong *timeo,
						    int (*to_fnc) (struct head *),
						    void streamscall (*exp_func) (caddr_t));
EXPORT_SYMBOL_NOVERS(ss7_do_timeout);
__OS7_EXTERN_INLINE streamscall void ss7_stop_timer(struct head *h, const char *timer,
						    const char *mod, ulong *timeo);
EXPORT_SYMBOL_NOVERS(ss7_stop_timer);
__OS7_EXTERN_INLINE streamscall void ss7_start_timer(struct head *h, const char *timer,
						     const char *mod, ulong *timeo,
						     void streamscall (*exp_func) (caddr_t),
						     ulong val);
EXPORT_SYMBOL_NOVERS(ss7_start_timer);

#ifdef CONFIG_STREAMS_COMPAT_OS7_MODULE
static
#endif
int __init
os7comp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_OS7_MODULE
	printk(KERN_INFO OS7COMP_BANNER);
#else
	printk(KERN_INFO OS7COMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_OS7_MODULE
static
#endif
void __exit
os7comp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_OS7_MODULE
module_init(os7comp_init);
module_exit(os7comp_exit);
#endif
