/*****************************************************************************

 @(#) $Id: queue.h,v 0.9 2004/06/22 08:41:40 brian Exp $

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

 Last Modified $Date: 2004/06/22 08:41:40 $ by $Author: brian $

 *****************************************************************************/

#ifndef __QUEUE_H__
#define __QUEUE_H__

#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
#define QR_STRIP	7
#define QR_RETRY	8

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
ss7_w_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(WR(q), FLUSHALL);
		if (q->q_next)
			return (QR_PASSALONG);
		*mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(RD(q), FLUSHALL);
		if (!q->q_next)
			return (QR_LOOP);
		*mp->b_rptr &= ~FLUSHR;
	}
	return (QR_DONE);
}
STATIC INLINE int
ss7_r_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(RD(q), FLUSHALL);
		if (q->q_next)
			return (QR_PASSALONG);
		*mp->b_rptr &= ~FLUSHR;
	}
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(WR(q), FLUSHALL);
		if (!q->q_next)
			return (QR_LOOP);
		*mp->b_rptr &= ~FLUSHW;
	}
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */

/*
 *  PUTQ Put Routine
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
ss7_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *), void (*wakeup) (queue_t *))
{
	int rtn = 0;
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
#if 0
	if (q->q_count || mp->b_datap->db_type < QPCTL) {
		putq(q, mp);
		return (0);
	}
#else
	if (q->q_count) {
		putq(q, mp);
		return (0);
	}
#endif
	if (ss7_trylockq(q)) {
		do {
			/*
			   Fast Path 
			 */
			if ((rtn = proc(q, mp)) == QR_DONE) {
				freemsg(mp);
				break;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				break;
			case QR_STRIP:
				if (mp->b_cont)
					putq(q, mp->b_cont);
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
				printd(("%s: %p: ERROR: (q dropping) %d\n",
					q->q_qinfo->qi_minfo->mi_idname, q->q_ptr, rtn));
				freemsg(mp);
				break;
			case QR_DISABLE:
				putq(q, mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					break;
				}
			case -ENOBUFS:
			case -EBUSY:
			case -ENOMEM:
			case -EAGAIN:
				putq(q, mp);
				break;
			case QR_RETRY:
				putq(q, mp);
				break;
			}
		} while (0);
		if (wakeup)
			wakeup(q);
		ss7_unlockq(q);
	} else {
		rare();
		putq(q, mp);
	}
	return (rtn);
}

/*
 *  SRVQ Service Routine
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
ss7_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *), void (*wakeup) (queue_t *))
{
	int rtn = 0;
	ensure(q, return (-EFAULT));
	if (ss7_trylockq(q)) {
		mblk_t *mp;
		while ((mp = getq(q))) {
			/*
			   Fast Path 
			 */
			if ((rtn = proc(q, mp)) == QR_DONE) {
				freemsg(mp);
				continue;
			}
			switch (rtn) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				continue;
			case QR_STRIP:
				if (mp->b_cont)
					putbq(q, mp->b_cont);
			case QR_TRIMMED:
				freeb(mp);
				continue;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					continue;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					continue;
				}
				rtn = -EOPNOTSUPP;
			default:
				printd(("%s: %p: ERROR: (q dropping) %d\n",
					q->q_qinfo->qi_minfo->mi_idname, q->q_ptr, rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				printd(("%s: %p: ERROR: (q disabling) %d\n",
					q->q_qinfo->qi_minfo->mi_idname, q->q_ptr, rtn));
				noenable(q);
				putbq(q, mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* proc must have scheduled bufcall */
			case -EBUSY:	/* proc must have failed canput */
			case -ENOMEM:	/* proc must have scheduled bufcall */
			case -EAGAIN:	/* proc must re-enable on some event */
				if (mp->b_datap->db_type < QPCTL) {
					printd(("%s: %p: ERROR: (q stalled) %d\n",
						q->q_qinfo->qi_minfo->mi_idname, q->q_ptr, rtn));
					putbq(q, mp);
					break;
				}
				/*
				 *  Be careful not to put a priority
				 *  message back on the queue.
				 */
				if (mp->b_datap->db_type == M_PCPROTO) {
					mp->b_datap->db_type = M_PROTO;
					mp->b_band = 255;
					putq(q, mp);
					break;
				}
				printd(("%s: %p: ERROR: (q dropping) %d\n",
					q->q_qinfo->qi_minfo->mi_idname, q->q_ptr, rtn));
				freemsg(mp);
				continue;
			case QR_RETRY:
				putbq(q, mp);
				continue;
			}
			break;
		}
		/*
		   perform wakeups 
		 */
		if (wakeup)
			wakeup(q);
		ss7_unlockq(q);
	} else {
		rare();
	}
	return (rtn);
}

STATIC INLINE int
ss7_oput(queue_t *q, mblk_t *mp)
{
	str_t *s = STR_PRIV(q);
	if (s->oq) {
		if (s->o_prim)
			ss7_putq(s->oq, mp, s->o_prim, s->o_wakeup);
		else
			putq(s->oq, mp);
	} else
		putq(q, mp);
	return (0);
}

STATIC int
ss7_osrv(queue_t *q)
{
	str_t *s = STR_PRIV(q);
	if (s->oq) {
		if (s->o_prim) {
			ss7_srvq(s->oq, s->o_prim, s->o_wakeup);
			return (0);
		} else if (s->o_wakeup) {
			s->o_wakeup(s->oq);
			return (0);
		}
	}
	swerr();
	noenable(q);
	return (-EFAULT);
}

STATIC INLINE int
ss7_iput(queue_t *q, mblk_t *mp)
{
	str_t *s = STR_PRIV(q);
	if (s->iq) {
		if (s->i_prim)
			ss7_putq(s->iq, mp, s->i_prim, s->i_wakeup);
		else
			putq(s->iq, mp);
	} else
		putq(q, mp);
	return (0);
}

STATIC int
ss7_isrv(queue_t *q)
{
	str_t *s = STR_PRIV(q);
	if (s->iq) {
		if (s->i_prim) {
			ss7_srvq(s->iq, s->i_prim, s->i_wakeup);
			return (0);
		} else if (s->i_wakeup) {
			s->i_wakeup(s->iq);
			return (0);
		}
	}
	swerr();
	noenable(q);
	return (-EFAULT);
}

#endif				/* __QUEUE_H__ */
