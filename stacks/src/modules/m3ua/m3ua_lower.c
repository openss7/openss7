/*****************************************************************************

 @(#) $RCSfile: m3ua_lower.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/03/08 19:30:05 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

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

 Last Modified $Date: 2005/03/08 19:30:05 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: m3ua_lower.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/03/08 19:30:05 $"

static char const ident[] =
    "$RCSfile: m3ua_lower.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/03/08 19:30:05 $";

#define __NO_VERSION__

#include "os7/compat.h"

#include "m3ua.h"
#include "m3ua_data.h"

/*
 *  =========================================================================
 *
 *  Lower --> SUA (Upstream Primitives received from downstream)
 *
 *  =========================================================================
 */

/* from m3ua_uap.c */
extern int uap_r_data(queue_t *, mblk_t *);
extern int uap_r_proto(queue_t *, mblk_t *);

/* from m3ua_ss7.c */
extern int ss7_r_data(queue_t *, mblk_t *);
extern int ss7_r_proto(queue_t *, mblk_t *);

/*
 *  M_ERROR, M_HANGUP Processing
 *  -------------------------------------------------------------------------
 */
static int uap_r_error(queue_t * q, mblk_t * mp)
{
	int err;
	if ((err = lm_link_ind(q, mp)))
		return (err);
	if ((err = uap_blocked(q)))
		return (err);
	return (0);
}
static int ipc_r_error(queue_t * q, mblk_t * mp)
{
	int err;
	if ((err = lm_link_ind(q, mp)))
		return (err);
	if ((err = ipc_blocked(q)))
		return (err);
	return (0);
}
static int ss7_r_error(queue_t * q, mblk_t * mp)
{
	int err;
	if ((err = lm_link_ind(q, mp)))
		return (err);
	if ((err = ss7_blocked(q)))
		return (err);
	return (0);
}

/*
 *  M_CTL Processing
 *  -------------------------------------------------------------------------
 */
static int uap_r_ctl(queue_t * q, mblk_t * mp)
{
	int err;
	if ((err = lm_link_ind(q, mp)))
		return (err);
	return (0);
}
static int ipc_r_ctl(queue_t * q, mblk_t * mp)
{
	int err;
	if ((err = lm_link_ind(q, mp)))
		return (err);
	return (0);
}
static int ss7_r_ctl(queue_t * q, mblk_t * mp)
{
	int err;
	if ((err = lm_link_ind(q, mp)))
		return (err);
	return (0);
}

/*
 *  M_FLUSH Processing
 *  -------------------------------------------------------------------------
 */
static int xxx_r_flush(queue_t * q, mblk_t * mp)
{
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushall(q, FLUSHALL);
		*mp->b_rptr &= ~FLUSHR;
	}
	if ((*mp->b_rptr & FLUSHW) && !(mp->b_flags & MSGNOLOOP)) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushall(q, FLUSHALL);
		mp->b_flag |= MSGNOLOOP;
		qreply(q, mp);	/* flush all the way back down */
	}
	freemsg(mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  QUEUE PUT and SERVICE routines
 *
 *  =========================================================================
 */
static inline int m3ua_recover(queue_t * q, mblk_t * mp, int err)
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
static inline int m3ua_reservice(queue_t * q, mblk_t * mp, int err)
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
 *  READ PUT and SERVICE (Message from below to SUA)
 *  -------------------------------------------------------------------------
 */
static inline int m3ua_rd(queue_t * q, mblk_t * mp)
{
	switch (Q_TYPE(q)) {
	case UA_TYPE_UAP:
		switch (mp->b_datap->db_type) {
		case M_DATA:
			return uap_r_data(q, mp);
		case M_PROTO:
		case M_PCPROTO:
			return uap_r_proto(q, mp);
		case M_ERROR:
		case M_HANGUP:
			return uap_r_error(q, mp);
		case M_CTL:
			return uap_r_ctl(q, mp);
		case M_FLUSH:
			return xxx_r_flush(q, mp);
		}
		return (-EOPNOTSUPP);
	case UA_TYPE_IPC:
		switch (mp->b_datap->db_type) {
		case M_DATA:
			return ipc_r_data(q, mp);
		case M_PROTO:
		case M_PCPROTO:
			return ipc_r_proto(q, mp);
		case M_ERROR:
		case M_HANGUP:
			return ipc_r_error(q, mp);
		case M_CTL:
			return ipc_r_ctl(q, mp);
		case M_FLUSH:
			return xxx_r_flush(q, mp);
		}
		return (-EOPNOTSUPP);
	case UA_TYPE_SS7:
		switch (mp->b_datap->db_type) {
		case M_DATA:
			return ss7_r_data(q, mp);
		case M_PROTO:
		case M_PCPROTO:
			return ss7_r_proto(q, mp);
		case M_ERROR:
		case M_HANGUP:
			return ss7_r_error(q, mp);
		case M_CTL:
			return ss7_r_ctl(q, mp);
		case M_FLUSH:
			return xxx_r_flush(q, mp);
		}
		return (-EOPNOTSUPP);
	}
	return (-EFAULT);
}

int m3ua_l_rput(queue_t * q, mblk_t * mp)
{
	int err;
	if (mp->b_datap->db_type < QPCTL && (q->q_count || !canputnext(q))) {
		putq(q, mp);
		return (0);
	}
	if ((err = m3ua_rd(q, mp)))
		return (m3ua_recover(q, mp, err));
	return (0);
}

int m3ua_l_rsrv(queue_t * q)
{
	mblk_t *mp;
	while ((mp = getq(q))) {
		int err;
		if (!(err = m3ua_rd(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (m3ua_reservice(q, mp, err));
		freemsg(mp);
		return (err);
	}
	return (0);
}
