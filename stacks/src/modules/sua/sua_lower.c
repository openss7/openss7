/*****************************************************************************

 @(#) $Id: sua_lower.c,v 0.9.2.1 2004/08/21 10:14:59 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2004/08/21 10:14:59 $ by $Author: brian $

 $Log: sua_lower.c,v $
 Revision 0.9.2.1  2004/08/21 10:14:59  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:23:12  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.2  2003/04/03 19:51:34  brian
 Updates preparing for release.

 Revision 0.8.2.1  2002/10/18 02:33:28  brian
 Indentation changes.

 Revision 0.8  2002/04/02 08:21:09  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/08/20 07:14:49  brian
 Added two files consistent with SCCP.

 *****************************************************************************/

#ident "@(#) $Id: sua_lower.c,v 0.9.2.1 2004/08/21 10:14:59 brian Exp $"

static char const ident[] = "$Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:14:59 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/cmn_err.h>

#include "sua.h"
#include "sua_data.h"

/*
 *  =========================================================================
 *
 *  SCCP/NPI/TPI --> SUA (Upstream Primitives received from downstream)
 *
 *  =========================================================================
 */

/* from sua_nsp.c */
extern int nsp_r_data(queue_t *, mblk_t *);
extern int nsp_r_proto(queue_t *, mblk_t *);
extern int nsp_r_ctl(queue_t *, mblk_t *);
extern int nsp_r_error(queue_t *, mblk_t *);
extern int nsp_r_flush(queue_t *, mblk_t *);
extern int nsp_r_hangup(queue_t *, mblk_t *);

/* from sua_tsp.c */
extern int tsp_r_data(queue_t *, mblk_t *);
extern int tsp_r_proto(queue_t *, mblk_t *);
extern int tsp_r_ctl(queue_t *, mblk_t *);
extern int tsp_r_error(queue_t *, mblk_t *);
extern int tsp_r_flush(queue_t *, mblk_t *);
extern int tsp_r_hangup(queue_t *, mblk_t *);

/* from sua_ss7.c */
extern int ss7_r_data(queue_t *, mblk_t *);
extern int ss7_r_proto(queue_t *, mblk_t *);
extern int ss7_r_ctl(queue_t *, mblk_t *);
extern int ss7_r_error(queue_t *, mblk_t *);
extern int ss7_r_flush(queue_t *, mblk_t *);
extern int ss7_r_hangup(queue_t *, mblk_t *);

/*
 *  =========================================================================
 *
 *  QUEUE PUT and SERVICE routines
 *
 *  =========================================================================
 */

static inline int sua_recover(queue_t * q, mblk_t * mp, int err)
{
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		putq(q, mp);
		return (0);
	}
	freemsg(mp);
	return (err);
}
static inline int sua_reservice(queue_t * q, mblk_t * mp, int err)
{
	if (mp->b_datap->db_type < QPCTL)
		switch (err) {
		case -EBUSY:
		case -EAGAIN:
		case -ENOMEM:
		case -ENOBUFS:
			putbq(q, mp);
			return (0);
		}
	freemsg(mp);
	return (err);
}

/*
 *  READ PUT and SERVICE (Message from below SCCP/NPI/TPI --> SUA)
 *  -------------------------------------------------------------------------
 */
static inline int sua_rd(queue_t * q, mblk_t * mp)
{
	switch (Q_TYPE(q)) {
	case Q_TYPE_NPI:
		switch (mp->b_datap->db_type) {
		case M_DATA:
			return nsp_r_data(q, mp);
		case M_PROTO:
		case M_PCPROTO:
			return nsp_r_proto(q, mp);
		case M_CTL:
			return nsp_r_ctl(q, mp);
		case M_ERROR:
			return nsp_r_error(q, mp);
		case M_FLUSH:
			return nsp_r_flush(q, mp);
		case M_HANGUP:
			return nsp_r_hangup(q, mp);
		}
		return (-EOPNOTSUPP);
	case Q_TYPE_TPI:
		switch (mp->b_datap->db_type) {
		case M_DATA:
			return tsp_r_data(q, mp);
		case M_PROTO:
		case M_PCPROTO:
			return tsp_r_proto(q, mp);
		case M_CTL:
			return tsp_r_ctl(q, mp);
		case M_ERROR:
			return tsp_r_error(q, mp);
		case M_FLUSH:
			return tsp_r_flush(q, mp);
		case M_HANGUP:
			return tsp_r_hangup(q, mp);
		}
		return (-EOPNOTSUPP);
	case Q_TYPE_SS7:
		switch (mp->b_datap->db_type) {
		case M_DATA:
			return ss7_r_data(q, mp);
		case M_PROTO:
		case M_PCPROTO:
			return ss7_r_proto(q, mp);
		case M_CTL:
			return ss7_r_ctl(q, mp);
		case M_ERROR:
			return ss7_r_error(q, mp);
		case M_FLUSH:
			return ss7_r_flush(q, mp);
		case M_HANGUP:
			return ss7_r_hangup(q, mp);
		}
		return (-EOPNOTSUPP);
	}
	return (-EFAULT);
}

INT sua_l_rput(queue_t * q, mblk_t * mp)
{
	int err;
	if (mp->b_datap->db_type < QPCTL && (q->q_count || !canputnext(q))) {
		putq(q, mp);
		return (INT) (0);
	}
	if ((err = sua_rd(q, mp)))
		return (INT) (sua_recover(q, mp, err));
	return (INT) (0);
}

INT sua_l_rsrv(queue_t * q)
{
	mblk_t *mp;
	while ((mp = getq(q))) {
		int err;
		if (!(err = sua_rd(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (INT) (sua_reservice(q, mp, err));
		freemsg(mp);
		return (INT) (err);
	}
	return (INT) (0);
}
