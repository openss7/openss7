/*****************************************************************************

 @(#) $Id: m3ua_sg.c,v 0.9.2.3 2004/08/29 20:25:21 brian Exp $

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

 Last Modified $Date: 2004/08/29 20:25:21 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m3ua_sg.c,v $
 Revision 0.9.2.3  2004/08/29 20:25:21  brian
 - Updates to driver registration for Linux Fast-STREAMS.

 Revision 0.9.2.2  2004/08/26 23:37:57  brian
 - Converted for use with Linux Fast-STREAMS.

 Revision 0.9.2.1  2004/08/21 10:14:45  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:20:21  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.3  2003/04/14 12:13:00  brian
 Updated module license defines.

 Revision 0.8.2.2  2003/04/03 19:50:31  brian
 Updates preparing for release.

 Revision 0.8.2.1  2002/10/18 03:27:43  brian
 Indentation changes only.

 Revision 0.8  2002/04/02 08:20:42  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/06/04 03:48:36  brian
 Added files for M3UA.

 *****************************************************************************/

static char const ident[] = "$Name:  $($Revision: 0.9.2.3 $) $Date: 2004/08/29 20:25:21 $";

#include "compat.h"

#include <ss7/m3ua.h>
#include <ss7/m3ua_ioctl.h>

#define M3UA_DESCRIP	"M3UA/SCTP STREAMS MULTIPLEXOR."
#define M3UA_COPYRIGHT	"Copyright (c) 2001 OpenSS7 Corp. All Rights Reserved."
#define M3UA_DEVICES	"Supports OpenSS7 drivers."
#define M3UA_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define M3UA_LICENSE	"GPL"
#define M3UA_BANNER	M3UA_DESCRIP	"\n" \
			M3UA_COPYRIGHT	"\n" \
			M3UA_DEVICES	"\n" \
			M3UA_CONTACT	"\n"

#ifdef LINUX
MODULE_AUTHOR(M3UA_CONTACT);
MODULE_DESCRIPTION(M3UA_DESCRIP);
MODULE_SUPPORTED_DEVICE(M3UA_DEVICES);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M3UA_LICENSE);
#endif				/* MODULE_LICENSE */
#endif				/* LINUX */

#ifdef M3UA_DEBUG
static int m3ua_debug = M3UA_DEBUG;
#else
static int m3ua_debug = 2;
#endif

#define DEBUG_LEVEL m3ua_debug

#ifdef LFS
#define M3UA_DRV_ID		CONFIG_STREAMS_M3UA_MODID
#define M3UA_DRV_NAME		CONFIG_STREAMS_M3UA_NAME
#define M3UA_CMAJORS		CONFIG_STREAMS_M3UA_NMAJORS
#define M3UA_CMAJOR_0		CONFIG_STREAMS_M3UA_MAJOR
#define M3UA_UNITS		CONFIG_STREAMS_M3UA_NMINORS
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		M3UA_DRV_ID
#define DRV_NAME	M3UA_DRV_NAME
#define CMAJORS		M3UA_CMAJORS
#define CMAJOR_0	M3UA_CMAJOR_0
#define UNITS		M3UA_UNITS
#ifdef MODULE
#define DRV_BANNER	M3UA_BANNER
#else				/* MODULE */
#define DRV_BANNER	M3UA_SPLASH
#endif				/* MODULE */

static struct module_info m3ua_minfo = {
	DRV_ID,				/* Module ID number */
	DRV_NAME,			/* Module name */
	1,				/* Min packet size accepted */
	512,				/* Max packet size accepted */
	8 * 512,			/* Hi water mark */
	1 * 512				/* Lo water mark */
};

static void m3ua_rput(queue_t *, mblk_t *);
static void m3ua_rsrv(queue_t *);
static int m3ua_open(queue_t *, dev_t *, int, int, cred_t *);
static int m3ua_close(queue_t *, int, cred_t *);

static struct qinit m3ua_rinit = {
	m3ua_rput,			/* Read put (msg from below) */
	m3ua_rsrv,			/* Read queue service */
	m3ua_open,			/* Each open */
	m3ua_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&m3ua_minfo,			/* Information */
	NULL				/* Statistics */
};

static void m3ua_wput(queue_t *, mblk_t *);
static void m3ua_wsrv(queue_t *);

static struct qinit m3ua_winit = {
	m3ua_wput,			/* Read put (msg from below) */
	m3ua_wsrv,			/* Read queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&m3ua_minfo,			/* Information */
	NULL				/* Statistics */
};

static struct module_info t_minfo = {
	0,				/* Module ID number */
	"m3ua-T",			/* Module name */
	1,				/* Min packet size accepted */
	512,				/* Max packet size accepted */
	8 * 512,			/* Hi water mark */
	1 * 512				/* Lo water mark */
};

static void t_rput(queue_t *, mblk_t *);
static void t_rsrv(queue_t *);

static struct qinit t_rinit = {
	t_rput,				/* Write put (msg from above) */
	t_rsrv,				/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	&t_minfo,			/* Information */
	NULL				/* Statistics */
};

static void t_wput(queue_t *, mblk_t *);
static void t_rput(queue_t *);

static struct qinit t_winit = {
	t_wput,				/* Write put (msg from above) */
	t_wsrv,				/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	&t_minfo,			/* Information */
	NULL				/* Statistics */
};

MODULE_STATIC struct streamtab m3uainfo = {
	&m3ua_rinit,			/* Upper read queue */
	&m3ua_winit,			/* Upper write queue */
	&t_rinit,			/* Lower read queue */
	&t_winit			/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  PROTOCOL CONFIGURATION IOCTLs
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  M3UA-Provider -> MTP-Provider Primitives (M_CTL, M_PROTO, M_PCPROTO
 *
 *  =========================================================================
 */

static int
mtp_transfer_ind(m3ua_t * m3)
{
}

/*
 *  =========================================================================
 *
 *  M3UA-Provider -> T-Provider Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */

static int
t_conn_req(t_t * t)
{
}
static int
t_conn_res(t_t * t)
{
}
static int
t_discon_req(t_t * t)
{
}
static int
t_data_req(t_t * t)
{
}
static int
t_exdata_req(t_t * t)
{
}
static int
t_info_req(t_t * t)
{
}
static int
t_bind_req(t_t * t)
{
}
static int
t_unbind_req(t_t * t)
{
}
static int
t_unitdata_req(t_t * t)
{
}
static int
t_optmgmt_req(t_t * t)
{
}
static int
t_ordrel_req(t_t * t)
{
}
static int
t_addr_req(t_t * t)
{
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINES
 *
 *  =========================================================================
 */

static int
m3ua_error_ind(m3ua_t * m3)
{
}
static int
m3ua_notify_ind(m3ua_t * m3)
{
}
static int
m3ua_data_ind(m3ua_t * m3)
{
}
static int
m3ua_duna_ind(m3ua_t * m3)
{
}
static int
m3ua_dava_ind(m3ua_t * m3)
{
}
static int
m3ua_duad_ind(m3ua_t * m3)
{
}
static int
m3ua_scon_ind(m3ua_t * m3)
{
}
static int
m3ua_dupu_ind(m3ua_t * m3)
{
}
static int
m3ua_aspup_ind(m3ua_t * m3)
{
}
static int
m3ua_aspdn_ind(m3ua_t * m3)
{
}
static int
m3ua_asphb_ind(m3ua_t * m3)
{
}
static int
m3ua_aspup_ack_ind(m3ua_t * m3)
{
}
static int
m3ua_aspdn_ack_ind(m3ua_t * m3)
{
}
static int
m3ua_asphb_ack_ind(m3ua_t * m3)
{
}
static int
m3ua_aspac_ind(m3ua_t * m3)
{
}
static int
m3ua_aspia_ind(m3ua_t * m3)
{
}
static int
m3ua_aspac_ack_ind(m3ua_t * m3)
{
}
static int
m3ua_aspia_ack_ind(m3ua_t * m3)
{
}

/*
 *  =========================================================================
 *
 *  MTP-Provider -> M3UA-Provider Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */

#define M3UA_DSTR_FIRST
#define M3UA_DSTR_LAST

static int
mtp_transfer_req(m3ua_t * m3, mblk_t *mp)
{
	mtp_transfer_t *m = (mtp_transfer_ind_t *) mp->b_rptr;
	/* 
	 *  Find the appropriate AS using the routing information in the
	 *  request and select an available ASP.  Package up a DATA message
	 *  and send it to the ASP on the appropriate stream.
	 */
}
static int
mtp_pause_req(m3ua_t * m3, mblk_t *mp)
{
	mtp_pause_t *m = (mtp_pause_ind_t *) mp->b_rptr;
}
static int
mtp_resume_req(m3ua_t * m3, mblk_t *mp)
{
	mtp_resume_t *m = (mtp_resume_ind_t *) mp->b_rptr;
}
static int
mtp_status_req(m3ua_t * m3, mblk_t *mp)
{
	mtp_status_t *m = (mtp_status_ind_t *) mp->b_rptr;
}
static int
mtp_restart_complete_req(m3ua_t * m3, mblk_t *mp)
{
	mtp_restart_comp_t *m = (mtp_restart_comp_t *) mp->b_rptr;
}

static void

/*
 *  =========================================================================
 *
 *  T-Provider -> M3UA-Provider Primitives (M_CTL, M_PROTO, M_PCPROTO
 *
 *  =========================================================================
 */
#define M3UA_USTR_FIRST	    T_CONN_IND
#define M3UA_USTR_LAST	    T_ADDR_ACK
    static void
t_conn_ind(t_t * t, mblk_t *mp)
{
}
static void
t_conn_con(t_t * t, mblk_t *mp)
{
}
static void
t_discon_ind(t_t * t, mblk_t *mp)
{
}
static void
t_data_ind(t_t * t, mblk_t *mp)
{
}
static void
t_exdata_ind(t_t * t, mblk_t *mp)
{
}
static void
t_info_ack(t_t * t, mblk_t *mp)
{
}
static void
t_bind_ack(t_t * t, mblk_t *mp)
{
}
static void
t_error_ack(t_t * t, mblk_t *mp)
{
}
static void
t_ok_ack(t_t * t, mblk_t *mp)
{
}
static void
t_unitdata_ind(t_t * t, mblk_t *mp)
{
}
static void
t_uderror_ind(t_t * t, mblk_t *mp)
{
}
static void
t_optmgmt_ack(t_t * t, mblk_t *mp)
{
}
static void
t_nosup(t_t * t, mblk_t *mp)
{
}
static void
t_addr_ack(t_t * t, mblk_t *mp)
{
}
static void (*m3ua_t_ops[]) (t_t *, mblk_t *) = {
	t_conn_ind,		/* T_CONN_IND */
	    t_conn_con,		/* T_CONN_CON */
	    t_discon_ind,	/* T_DISCON_IND */
	    t_data_ind,		/* T_DATA_IND */
	    t_exdata_ind,	/* T_EXDATA_IND */
	    t_info_ack,		/* T_INFO_ACK */
	    t_bind_ack,		/* T_BIND_ACK */
	    t_error_ack,	/* T_ERROR_ACK */
	    t_ok_ack,		/* T_OK_ACK */
	    t_unitdata_ind,	/* T_UNITDATA_IND */
	    t_uderror_ind,	/* T_UDERROR_IND */
	    t_optmgmt_ack,	/* T_OPTMGMT_ACK */
	    t_nosup,		/* T_ADDR_REQ */
	    t_addr_ack		/* T_ADDR_ACK */
}

/*
 *  =========================================================================
 *
 *  M_IOCTL handling
 *
 *  =========================================================================
 */

static int
m3ua_do_ioctl(m3ua_t * m3, int cmd, void *arg)
{
	int nr = _IOC_NR(cmd);

	trace();
	if (_IOC_TYPE(cmd) == M3UA_IOC_MAGIC)
		if (M3UA_IOC_FIRST <= nr && nr <= M3UA_IOC_LAST)
			if (m3ua_ioc_ops[nr])
				return m3ua_ioc_ops[nr] (m3, cmd, arg);
	return -ENXIO;
}

#ifndef abs
#define abs(x) ((x)<0 ? -(x):(x))
#endif

static inline int
m3ua_m_ioctl(queue_t *q, mblk_t *mp)
{
	t_t *t;
	m3ua_t *m3ua = (m3ua_t *) q->q_ptr;
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int ret = -EINVAL;
	struct linkblk *lp = (struct linkblk *) arg;

	trace();
	switch (cmd) {
	case I_LINK:
	case I_PLINK:
		/* 
		 *  NOTE: PLINK can be a bit different and could be rather useful.
		 *  A PLINKED SCTP stream will never be closed, even if the
		 *  configuration daemon crashes, even if the m3ua stream is
		 *  closed.  This permits more permanent M3UA configuration
		 *  structures.  The m3uad upon restart reopens the control
		 *  channel with root priviledges and can access the exisitng
		 *  configuration.
		 *
		 *  NOTE: do a MOD_INC_USE_COUNT when PLINKs are added and a
		 *  MOD_DEC_USE_COUNT when they are removed.  Otherwise the module
		 *  may autoclean.
		 */
		if (getminor(m3ua->devnum)) {
			ret = -EPERM;
			break;
		}
		if (!(t = kmalloc(sizeof(*t), GFP_KERNEL))) {
			ret = -ENOMEM;
			break;
		}
		bzero(t, sizeof(*t));

		t->next = m3ua->links;
		m3ua->links = t;
		t->linked = m3ua;
		t->muxid = lp->l_index;
		t->rq = RD(lp->l_qbot);
		t->rq->q_ptr = t;
		t->wq = WR(lp->l_qbot);
		t->wq->q_ptr = t;

		ret = 0;
		break;
	case I_UNLINK:
	case I_PUNLINK:
		if (getminor(m3ua->devnum)) {
			ret = -EPERM;
			break;
		}
		if (t->next)
			t->next->prev = t->prev;
		if (t->prev)
			t->prev->next = t->next;
		kfree(t);
		ret = 0;
		break;
	default:
		if (count >= _IOC_SIZE(cmd)) {
			ret = m3ua_do_ioctl(m3ua, cmd, arg);
		}
		if (abs(ret) == ENXIO) {
			if (q->q_next) {
				putnext(q, mp);
				return (0);
			}
		}
		break;
	}
	if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = abs(ret);
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  M_PROTO, M_PCPROTO handling
 *
 *  =========================================================================
 */

static inline int
m3ua_m_proto(queue_t *q, mblk_t *mp)
{
	int err = EOPNOTSUPP;
	m3ua_t *m3ua = (m3ua_t *) q->q_ptr;
	int prim = ntohs(((struct m3ua_hdr *) (mp->b_rptr))->type);

	trace();
	if (M3UA_DSTR_FIRST <= prim && prim <= M3UA_DSTR_LAST) {
		(*m3ua_dstr_ops[M3UA_DSTR_FIRST - prim]) (m3ua, mp);
		return (0);
	}
	if (q->q_next) {
		putnext(q, mp);
		return (0);
	}
	return err;
}

static inline int
t_m_proto(queue_t *q, mblk_t *mp)
{
	int err = EOPNOTSUPP;
	t_t *t = (t_t *) q->q_ptr;
	int prim = ((union T_primitives *) (mp->b_rptr))->PRIM_type;

	trace();
	if (M3UA_USTR_FIRST <= prim && prim <= M3UA_USTR_LAST) {
		(*m3ua_ustr_ops[M3UA_USTR_FIRST - prim]) (t, mp);
		return (0);
	}
	if (q->q_next) {
		putnext(q, mp);
		return (0);
	}
	return err;
}

/*
 *  =========================================================================
 *
 *  M_DATA handling
 *
 *  =========================================================================
 */

static inline int
m3ua_m_data(queue_t *q, mblk_t *mp)
{
	m3ua_t *m3 = (m3ua_t *) q->q_ptr;
	trace();
	m3ua_data(m3, mp);
	return (0);
}

static inline int
t_m_data(queue_t *q, mblk_t *mp)
{
	t_t *t = (t_t *) q->q_ptr;
	trace();
	t_data(t, mp);
	return (0);
}

/*
 *  =======================================================================
 *
 *  STREAMS QUEUE PUT and QUEUE SERVICE routines
 *
 *  =======================================================================
 */

/*
 *  ------------------------------------------
 *
 *  UPPER QUEUE PUT and QUEUE SERVICE routines
 *
 *  ------------------------------------------
 */

static void
m3ua_wput(queue_t *q, mblk_t *mp)
{
	int err = EOPNOTSUPP;

	trace();
	if (q->q_count && mp->b_datap->db_type < QPCTL) {
		putq(q, mp);
		return;
	}
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if ((err = m3ua_m_proto(q, mp)))
			break;
		return;
	case M_CTL:
	case M_PROTO:
	case M_PCPROTO:
		if ((err = m3ua_m_proto(q, mp)))
			break;
		return;
	case M_FLUSH:
		if (*mp->b_wptr & FLUSHW) {
			flushq(q, FLUSHALL);
			if (q->q_next) {
				putnext(q, mp);
				return;
			}
			*mp->b_rptr &= ~FLUSHW;
		}
		if (*mp->b_rptr & FLUSHR) {
			flushq(RD(q), FLUSHALL);
			qreply(q, mp);
		} else
			break;
		return;
	case M_IOCTL:
		if ((err = ls_m_ioctl(q, mp)))
			break;
		return;
	}
	switch (err) {
	case EAGAIN:
		if (mp->b_datap->db_type < QPCTL) {
			putq(q, mp);
			return;
		}
		break;
	case EOPNTOSUPP:
		if (q->q_next) {
			putnext(q, mp);
			return;
		}
	}
	trace();
	freemsg(mp);
	return;
}

static void
m3ua_rsrv(queue_t *q)
{
	mblk_t *mp;
	int err = EOPNOTSUPP;

	trace();
	while ((mp = getq(q))) {
		if (mp->b_datap->db_type < QPCTL && !canputnext(q)) {
			putbq(q, mp);
			return;
		}
		switch (mp->b_datap->db_type) {
		case M_DATA:
			if ((err = m3ua_m_data(q, mp)))
				break;
			continue;
		case M_CTL:
		case M_PROTO:
		case M_PCPROTO:
			if ((err = m3ua_m_proto(q, mp)))
				break;
			continue;
		}
		switch (err) {
		case EAGAIN:
			if (mp->b_datap->db_type < QPCTL) {
				putbq(q, mp);
				return;
			}
			break;
		case EOPNOTSUPP:
			if (q->q_next) {
				putnext(q, mp);
				return;
			}
		}
		freemsg(mp);
	}
}

/*
 *  ------------------------------------------
 *
 *  LOWER QUEUE PUT and QUEUE SERVICE routines
 *
 *  ------------------------------------------
 */

static void
t_wput(queue_t *q, mblk_t *mp)
{
	trace();
	if (mp->b_datap->db_type < QPCTL && (q->q_count || !canputnext(q)))
		putq(q, mp);
	else
		putnext(q, mp);
}

static void
t_wsrv(queue_t *q)
{
	mblk_t *mp;

	trace();
	while ((mp = getq(q))) {
		if (mp->b_datap->db_type < QPCTL && !canputnext(q)) {
			putbq(q, mp);
			return;
		}
		putnext(q, mp);
	}
}

static void
t_rput(queue_t *q, mblk_t *mp)
{
	int err = EOPNOTSUPP;

	trace();
	if (q->q_count && mp->b_datap->db_type < QPCTL) {
		putq(q, mp);
		return;
	}
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if ((err = t_m_data(q, mp)))
			break;
		return;
	case M_CTL:
	case M_PROTO:
	case M_PCPROTO:
		if ((err = t_m_proto(q, mp)))
			break;
		return;
	}
	switch (err) {
	case EAGAIN:
		putq(q, mp);
		return;
	case EOPNOTSUPP:
		if (q->q_next) {
			putnext(q, mp);
			return;
		}
	}
	freemsg(mp);
}

static void
t_rsrv(queue_t *q)
{
	mblk_t *mp;
	int err = EOPNOTSUPP;

	trace();
	while ((mp = getq(q))) {
		if (mp->b_datap->db_type < QPCTL && !canputnext(q)) {
			putbq(q, mp);
			return;
		}
		switch (mp->b_datap->db_type) {
		case M_DATA:
			if ((err = t_m_data(q, mp)))
				break;
			continue;
		case M_CTL:
		case M_PROTO:
		case M_PCPROTO:
			if ((err = t_m_proto(q, mp)))
				break;
			continue;
		}
		switch (err) {
		case EAGAIN:
			if (mp->b_datap->db_type < QPCTL) {
				putbq(q, mp);
				return;
			}
			break;
		case EOPNOTSUPP:
			putnext(q, mp);
			return;
		}
		freemsg(mp);
	}
}

/*
 *  =======================================================================
 *
 *  OPEN and CLOSE
 *
 *  =======================================================================
 */

static m3ua_t *m3ua_devices = NULL;
static queue_t *m3ua_ctrlq = NULL;

static int
m3ua_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	m3ua_t *m3ua, **m3p = &m3ua_devices;
	int cmajor = getmajor(*devp);
	int cminor = getmajor(*devp);

	trace();
	if (q->q_ptr != NULL)
		return (0);

	if (sflag == MODOPEN || WR(q)->q_next)
		return EIO;

	if (!cminor) {
		if (!m3ua_ctrlq)
			m3ua_ctrlq = q;
		else
			sflag = CLONEOPEN;
	}

	if (sflag == CLONEOPEN)
		cminor = 1;

	for (; cminor <= NMINORS && *m3p; m3p = &(*m3p)->next) {
		int dminor = getminor((*m3p)->devnum);
		if (cminor < dminor)
			break;
		if (cminor == dminor) {
			if (sflag == CLONEOPEN) {
				cminor++;
				continue;
			}
			return EIO;
		}
	}
	if (cminor > M3UA_MINOR)
		return ENXIO;

	*devp = makedevice(cmajor, cminor);

	if (!(m3ua = kmalloc(sizeof(*m3ua), GFP_KERNEL)))
		return ENOMEM;

	bzero(m3ua, sizeof(*m3ua));

	q->q_ptr = WR(q)->q_ptr;
	m3ua->rq = RD(q);
	m3ua->wq = WR(q);

	m3ua->state = FIXME;
	m3ua->devnum = *devp;

	return (0);
}

static int
m3ua_close(queue_t *q, int flag, cred_t *crp)
{
	m3ua_t *m3 = (m3ua_t *) q->q_ptr;
	kfree(m3);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = DRV_ID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module ID for the INET driver. (0 for allocation.)");

unsigned short major = CMAJOR_0;
MODULE_PARM(major, "h");
MODULE_PARM_DESC(major, "Device number for the INET driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw m3ua_cdev = {
	.d_name = DRV_NAME,
	.d_str = &m3uainfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
m3ua_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&m3ua_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
m3ua_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&m3ua_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
m3ua_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &m3uainfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
m3ua_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
m3uaterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (m3ua_majors[mindex]) {
			if ((err = m3ua_unregister_strdev(m3ua_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					m3ua_majors[mindex]);
			if (mindex)
				m3ua_majors[mindex] = 0;
		}
	}
	if ((err = m3ua_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
m3uainit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = m3ua_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		m3uaterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = m3ua_register_strdev(m3ua_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					m3ua_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				m3uaterminate();
				return (err);
			}
		}
		if (m3ua_majors[mindex] == 0)
			m3ua_majors[mindex] = err;
#ifdef LIS
		LIS_DEVFLAGS(m3ua_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = m3ua_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(m3uainit);
module_exit(m3uaterminate);

#endif				/* LINUX */
