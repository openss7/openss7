/*****************************************************************************

 @(#) $Id: allocb.h,v 0.9 2004/06/22 08:41:39 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2004/06/22 08:41:39 $ by $Author: brian $

 *****************************************************************************/

#ifndef __ALLOCB_H__
#define __ALLOCB_H__

/*
 *  =========================================================================
 *
 *  BUFFER Allocation
 *
 *  =========================================================================
 */
typedef void (*bufcall_fnc_t) (long);
/*
 *  BUFSRV call service routine
 *  -------------------------------------------------------------------------
 */
STATIC void
ss7_bufsrv(long data)
{
	queue_t *q = (queue_t *) data;
	if (q) {
		str_t *s = STR_PRIV(q);
		if (q == s->iq) {
			if (s->ibid) {
				s->ibid = 0;
				atomic_dec(&s->refcnt);
			}
			if (q->q_qinfo && q->q_qinfo->qi_srvp)
				qenable(q);
			else
				ss7_isrv(q);
		}
		if (q == s->oq) {
			if (s->obid) {
				s->obid = 0;
				atomic_dec(&s->refcnt);
			}
			if (q->q_qinfo && q->q_qinfo->qi_srvp)
				qenable(q);
			else
				ss7_osrv(q);
		}
	}
}

/*
 *  UNBUFCALL
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
ss7_unbufcall(str_t * s)
{
	if (s->ibid) {
		unbufcall(xchg(&s->ibid, 0));
		atomic_dec(&s->refcnt);
	}
	if (s->obid) {
		unbufcall(xchg(&s->obid, 0));
		atomic_dec(&s->refcnt);
	}
}

/*
 *  BUFCALL
 *  -------------------------------------------------------------------------
 */
STATIC void
ss7_bufcall(queue_t *q, size_t size, int prior)
{
	if (q) {
		str_t *s = STR_PRIV(q);
		if (q == s->iq) {
			if (!s->ibid) {
				s->ibid = bufcall(size, prior, &ss7_bufsrv, (long) q);
				atomic_inc(&s->refcnt);
			}
			return;
		}
		if (q == s->oq) {
			if (!s->obid) {
				s->obid = bufcall(size, prior, &ss7_bufsrv, (long) q);
				atomic_inc(&s->refcnt);
			}
			return;
		}
		swerr();
		return;
	}
}

/*
 *  ESBBCALL
 *  -------------------------------------------------------------------------
 */
STATIC void
ss7_esbbcall(queue_t *q, int prior)
{
	if (q) {
		str_t *s = STR_PRIV(q);
		if (q == s->iq) {
			if (!s->ibid) {
				s->ibid = esbbcall(prior, &ss7_bufsrv, (long) q);
				atomic_inc(&s->refcnt);
			}
			return;
		}
		if (q == s->oq) {
			if (!s->obid) {
				s->obid = esbbcall(prior, &ss7_bufsrv, (long) q);
				atomic_inc(&s->refcnt);
			}
			return;
		}
		swerr();
		return;
	}
}

/*
 *  ALLOCB
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
ss7_allocb(queue_t *q, size_t size, int prior)
{
	mblk_t *mp;
	if ((mp = allocb(size, prior)))
		return (mp);
	rare();
	ss7_bufcall(q, size, prior);
	return (mp);
}

/*
 *  ESBALLOC
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
ss7_esballoc(queue_t *q, unsigned char *base, size_t size, int prior, frtn_t *frtn)
{
	mblk_t *mp;
	if ((mp = esballoc(base, size, prior, frtn)))
		return (mp);
	rare();
	ss7_esbbcall(q, prior);
	return (mp);
}

/*
 *  PULLUPMSG
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
ss7_pullupmsg(queue_t *q, mblk_t *mp, int size)
{
	if (pullupmsg(mp, size) != 0)
		return (QR_DONE);
	ss7_bufcall(q, size, BPRI_MED);
	return (-ENOBUFS);
}

/*
 *  DUPB
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
ss7_dupb(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;
	if (!(mp = dupb(bp)))
		ss7_bufcall(q, bp->b_wptr - bp->b_rptr, BPRI_MED);
	return (mp);
}

/*
 *  DUPMSG
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
ss7_dupmsg(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;
	if (!(mp = dupmsg(bp)))
		ss7_bufcall(q, msgsize(bp), BPRI_MED);
	return (mp);
}

/*
 *  COPYB
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
ss7_copyb(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;
	if (!(mp = copyb(bp)))
		ss7_bufcall(q, bp->b_wptr - bp->b_rptr, BPRI_MED);
	return (mp);
}

/*
 *  COPYMSG
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
ss7_copymsg(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;
	if (!(mp = copymsg(bp)))
		ss7_bufcall(q, msgsize(bp), BPRI_MED);
	return (mp);
}

#endif				/* __ALLOCB_H__ */
