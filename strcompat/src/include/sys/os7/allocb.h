/*****************************************************************************

 @(#) $Id: allocb.h,v 0.9.2.16 2008-09-10 03:49:41 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-09-10 03:49:41 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: allocb.h,v $
 Revision 0.9.2.16  2008-09-10 03:49:41  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.15  2008-04-28 16:47:10  brian
 - updates for release

 Revision 0.9.2.14  2007/08/12 15:51:10  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.13  2006/11/03 10:39:21  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __OS7_ALLOCB_H__
#define __OS7_ALLOCB_H__

#ident "@(#) $RCSfile: allocb.h,v $ $Name:  $($Revision: 0.9.2.16 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 *  =========================================================================
 *
 *  BUFFER Allocation
 *
 *  =========================================================================
 */
/**
 * ss7_bufsrv: - buffer callback service procedure
 * @data: queue to enable
 *
 * This bufcall callback routine simply enables the queue for which the callback was generated.
 * Note that messages might be deferred on the queue even if the queue does not have a service
 * procedure.  If there is no defined service procedure, we reinvoke message processing here.
 *
 * The expensive atomic exchanges here are for Linux STREAMS (LiS) that has horribly unsafe bufcall,
 * unbufcall and callback mechanisms.
 */
static __inline__ streamscall void
ss7_bufsrv(long data)
{
	str_t *s;
	queue_t *q;

	q = (queue_t *) data;
	ensure(q, return);
	s = STR_PRIV(q);
	ensure(s, return);

	if (q == s->iq) {
		if (xchg(&s->ibid, 0) != 0)
			atomic_dec(&s->refcnt);
		if (q->q_qinfo && q->q_qinfo->qi_srvp)
			qenable(q);
		else
			ss7_isrv(q);
		return;
	}
	if (q == s->oq) {
		if (xchg(&s->obid, 0) != 0)
			atomic_dec(&s->refcnt);
		if (q->q_qinfo && q->q_qinfo->qi_srvp)
			qenable(q);
		else
			ss7_osrv(q);
		return;
	}
	return;
}

/**
 * ss7_unbufcall: - reliably cancel a buffer callbacks
 * @s: private structure of queue pair
 *
 * Cancel the qenable() buffer callback associated with each queue in the queue pair.
 * ss7_unbufcall() effectively undoes the actions performed by ss7_bufcall().  It is intended to be
 * called from the module close procedure to cancel these pending buffer callbacks.
 *
 * NOTICES: Cancellation of buffer callbacks on LiS using unbufcall() is unreliable.  The callback
 * function could execute some time shortly after the call to unbufcall() has returned.   LiS
 * abbrogates the SVR 4 STREAMS principles for unbufcall().  This is why you will find atomic
 * exchanges here and in the callback function and why reference counting is performed on the
 * structure.
 */
static __inline__ streamscall void
ss7_unbufcall(str_t * s)
{
	bufcall_id_t bid;

	if ((bid = xchg(&s->ibid, 0))) {
		unbufcall(bid);
		atomic_dec(&s->refcnt);
	}
	if ((bid = xchg(&s->obid, 0))) {
		unbufcall(bid);
		atomic_dec(&s->refcnt);
	}
}

/**
 *  ss7_bufcall: - generate a buffer callback to enable a queue
 *  @q: the queue to enable on callback
 *  @size: size of the allocation
 *  @prior: priority of the allocation
 *
 *  Maintain one buffer call for each queue in the queue pair.  The callback function will simply
 *  perform a qenable(9).
 *
 *  NOTICES: One of the reasons for going to such extents is that LiS has completely unsafe buffer
 *  callbacks.  The buffer callback function can be invoked (shortly) after unbufcall() returns
 *  under LiS in abrogation of SVR 4 STREAMS principles.  This is why you will find atomic exchanges
 *  here and in the callback function and why reference counting is performed on the structure and
 *  queue pointers are checked for NULL.
 */
static __inline__ streamscall void
ss7_bufcall(queue_t *q, size_t size, int prior)
{
	if (q) {
		str_t *s = STR_PRIV(q);
		bufcall_id_t bid, *bidp = NULL;

		if (q == s->iq)
			bidp = &s->ibid;
		if (q == s->oq)
			bidp = &s->obid;

		if (bidp) {
			atomic_inc(&s->refcnt);
			if ((bid = xchg(bidp, bufcall(size, prior, &ss7_bufsrv, (long) q)))) {
				unbufcall(bid);	/* Unsafe on LiS without atomic exchange above. */
				atomic_dec(&s->refcnt);
			}
			return;
		}
	}
	swerr();
	return;
}

/**
 *  ss7_esbcall: - generate a buffer callback to enable a queue
 *  @q: the queue to enable on callback
 *  @prior: priority of the allocation
 *
 *  Maintain one buffer call for each queue in the queue pair.  The callback function will simply
 *  perform a qenable(9).
 *
 *  NOTICES: One of the reasons for going to such extents is that LiS has completely unsafe buffer
 *  callbacks.  The buffer callback function can be invoked (shortly) after unbufcall() returns
 *  under LiS in abrogation of SVR 4 STREAMS principles.  This is why you will find atomic exchanges
 *  here and in the callback function and why reference counting is performed on the structure and
 *  queue pointers are checked for NULL.
 */
static __inline__ streamscall void
ss7_esbbcall(queue_t *q, int prior)
{
	if (q) {
		str_t *s = STR_PRIV(q);
		bufcall_id_t bid, *bidp = NULL;

		if (q == s->iq)
			bidp = &s->ibid;
		if (q == s->oq)
			bidp = &s->obid;

		if (bidp) {
			atomic_inc(&s->refcnt);
			if ((bid = xchg(bidp, esbbcall(prior, &ss7_bufsrv, (long) q)))) {
				unbufcall(bid);	/* Unsafe on LiS without atomic exchange above. */
				atomic_dec(&s->refcnt);
			}
			return;
		}
	}
	swerr();
	return;
}

/**
 *  ss7_allocb: - reliable allocb()
 *  @q: the queue to enable when allocation can succeed
 *  @size: the size to allocate
 *  @prior: the priority of the allocation
 *
 *  This helper function can be used by most STREAMS message handling procedures that allocate a
 *  buffer in response to an incoming message.  If the allocation fails, this routine will return
 *  NULL and issue a buffer callback to reenable the queue @q when the allocation could succeed.
 *  When NULL is returned, the caller should simply place the incoming message on the queue (i.e.
 *  with putq() from a put procedure or putbq() from a service procedure) and return.  The queue
 *  will be rescheduled with qenable() when the allocation could succeed.
 */
static __inline__ streamscall mblk_t *
ss7_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;

	if (likely((mp = allocb(size, prior)) != NULL))
		return (mp);
	rare();
	ss7_bufcall(q, size, prior);
	return (mp);
}

/**
 *  ss7_esballoc: - reliable esballoc()
 *  @q: the queue to enable when allocation can succeed
 *  @base: the base of the external data buffer
 *  @size: the size of the external data buffer
 *  @prior: the priority of the allocation
 *  @frtn: a pointer to a free routine structure (containing callback and client data)
 *
 *  This helper function can be used by most STREAMS message handling procedures that allocate a
 *  message block with an external buffer in response to an incoming message.  If the allocation
 *  fails, this routine will return NULL and issue a buffer callback to reenable the queue @q when
 *  the allocation could succeed.  When NULL is returned, the caller should simply place the
 *  incoming message on the queue (i.e.  with putq() from a put procedure or putbq() from a service
 *  procedure) and return.  The queue will be rescheduled with qenable() when the allocation could
 *  succeed.
 */
static __inline__ streamscall mblk_t *
ss7_esballoc(queue_t *q, unsigned char *base, size_t size, int prior, frtn_t *frtn)
{
	mblk_t *mp;

	if (likely((mp = esballoc(base, size, prior, frtn)) != NULL))
		return (mp);
	rare();
	ss7_esbbcall(q, prior);
	return (mp);
}

/**
 *  ss7_pullupmsg: - reliable pullupmsg()
 *  @q: queue to enable when pullup can succeed
 *  @mp: message to pullup
 *  @size: number of bytes to pull
 *
 *  This helper function can be used by most STREAMS message handling procedures that pullup an
 *  incoming message.  If the allocation fails, this routine will return -ENOBUFS and issue a buffer
 *  callback to reenable the queue @q when the pullup could succeed.  When -ENOBUFS is returned, the
 *  caller should simply place the incoming message on the queue (i.e.  with putq() from a put
 *  procedure or putbq() from a service procedure) and return.  The queue will be rescheduled with
 *  qenable() when the duplication could succeed.
 */
static __inline__ streamscall int
ss7_pullupmsg(queue_t *q, mblk_t *mp, int size)
{
	if (pullupmsg(mp, size) != 0)
		return (QR_DONE);
	ss7_bufcall(q, size > 0 ? size : -size, BPRI_MED);
	return (-ENOBUFS);
}

/**
 *  ss7_dupb: - reliable dupb()
 *  @q: queue to enable when duplication can succeed
 *  @bp: message block to duplicate
 *
 *  This helper function can be used by most STREAMS message handling procedures that duplicate an
 *  incoming message block.  If the allocation fails, this routine will return NULL and issue a
 *  buffer callback to reenable the queue @q when the duplication could succeed.  When NULL is
 *  returned, the caller should simply place the incoming message on the queue (i.e.  with putq()
 *  from a put procedure or putbq() from a service procedure) and return.  The queue will be
 *  rescheduled with qenable() when the duplication could succeed.
 */
static __inline__ streamscall mblk_t *
ss7_dupb(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (likely((mp = dupb(bp)) != NULL))
		return (mp);
	rare();
	ss7_bufcall(q, bp->b_wptr > bp->b_rptr ? bp->b_wptr - bp->b_rptr : 0, BPRI_MED);
	return (mp);
}

/**
 *  ss7_dupmsg: - reliable dupmsg()
 *  @q: queue to enable when duplication can succeed
 *  @bp: message to duplicate
 *
 *  This helper function can be used by most STREAMS message handling procedures that duplicate an
 *  incoming message.  If the allocation fails, this routine will return NULL and issue a buffer
 *  callback to reenable the queue @q when the duplication could succeed.  When NULL is returned,
 *  the caller should simply place the incoming message on the queue (i.e.  with putq() from a put
 *  procedure or putbq() from a service procedure) and return.  The queue will be rescheduled with
 *  qenable() when the duplication could succeed.
 */
static __inline__ streamscall mblk_t *
ss7_dupmsg(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (likely((mp = dupmsg(bp)) != NULL))
		return (mp);
	rare();
	ss7_bufcall(q, msgsize(bp), BPRI_MED);
	return (mp);
}

/**
 *  ss7_copyb: - reliable copyb()
 *  @q: queue to enable when copy can succeed
 *  @bp: message block to copy
 *
 *  This helper function can be used by most STREAMS message handling procedures that copy an
 *  incoming message block.  If the allocation fails, this routine will return NULL and issue a
 *  buffer callback to reenable the queue @q when the copy could succeed.  When NULL is returned,
 *  the caller should simply place the incoming message on the queue (i.e.  with putq() from a put
 *  procedure or putbq() from a service procedure) and return.  The queue will be rescheduled with
 *  qenable() when the duplication could succeed.
 */
static __inline__ streamscall mblk_t *
ss7_copyb(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (likely((mp = copyb(bp)) != NULL))
		return (mp);
	rare();
	ss7_bufcall(q, bp->b_wptr > bp->b_rptr ? bp->b_wptr - bp->b_rptr : 0, BPRI_MED);
	return (mp);
}

/**
 *  ss7_copymsg: - reliable copymsg()
 *  @q: queue to enable when copy can succeed
 *  @bp: message to copy
 *
 *  This helper function can be used by most STREAMS message handling procedures that copy an
 *  incoming message.  If the allocation fails, this routine will return NULL and issue a buffer
 *  callback to reenable the queue @q when the copy could succeed.  When NULL is returned, the
 *  caller should simply place the incoming message on the queue (i.e.  with putq() from a put
 *  procedure or putbq() from a service procedure) and return.  The queue will be rescheduled with
 *  qenable() when the duplication could succeed.
 */
static __inline__ streamscall mblk_t *
ss7_copymsg(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (likely((mp = copymsg(bp)) != NULL))
		return (mp);
	ss7_bufcall(q, msgsize(bp), BPRI_MED);
	return (mp);
}

#endif				/* __OS7_ALLOCB_H__ */
