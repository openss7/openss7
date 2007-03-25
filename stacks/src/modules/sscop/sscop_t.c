/*****************************************************************************

 @(#) $RCSfile: sscop_t.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2007/03/25 05:59:46 $

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

 Last Modified $Date: 2007/03/25 05:59:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sscop_t.c,v $
 Revision 0.9.2.14  2007/03/25 05:59:46  brian
 - flush corrections

 Revision 0.9.2.13  2007/03/25 02:23:03  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.12  2007/03/25 00:52:12  brian
 - synchronization updates

 Revision 0.9.2.11  2006/03/07 01:13:47  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop_t.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2007/03/25 05:59:46 $"

static char const ident[] =
    "$RCSfile: sscop_t.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2007/03/25 05:59:46 $";

#include <sys/os7/compat.h>

#define SSCOP_TPI_DESCRIP	"SSCOP-MCE/IP STREAMS DRIVER."
#define SSCOP_TPI_REVISION	"OpenSS7 $RCSfile: sscop_t.c,v $ $Name:  $ ($Revision: 0.9.2.14 $) $Date: 2007/03/25 05:59:46 $"
#define SSCOP_TPI_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SSCOP_TPI_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SSCOP_TPI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SSCOP_TPI_LICENSE	"GPL"
#define SSCOP_TPI_BANNER	SSCOP_TPI_DESCRIP	"\n" \
				SSCOP_TPI_REVISION	"\n" \
				SSCOP_TPI_COPYRIGHT	"\n" \
				SSCOP_TPI_DEVICE	"\n" \
				SSCOP_TPI_CONTACT
#define SSCOP_TPI_BANNER	SSCOP_TPI_DESCRIP	"\n" \
				SSCOP_TPI_REVISION

#ifdef LINUX
MODULE_AUTHOR(SSCOP_TPI_CONTACT);
MODULE_DESCRIPTION(SSCOP_TPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(SSCOP_TPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SSCOP_TPI_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sscop_t");
#endif
#endif				/* LINUX */

#ifdef SSCOP_TPI_DEBUG
STATIC int sscop_debug = SSCOP_TPI_DEBUG;
#else
STATIC int sscop_debug = 2;
#endif

#ifdef LFS
#define SSCOP_TPI_DRV_ID	CONFIG_STREAMS_SSCOP_TPI_MODID
#define SSCOP_TPI_DRV_NAME	CONFIG_STREAMS_SSCOP_TPI_NAME
#define SSCOP_TPI_CMAJORS	CONFIG_STREAMS_SSCOP_TPI_NMAJORS
#define SSCOP_TPI_CMAJOR_0	CONFIG_STREAMS_SSCOP_TPI_MAJOR
#endif				/* LFS */

#ifndef SSCOP_TPI_DRV_NAME
#define SSCOP_TPI_DRV_NAME	"sscop"
#endif				/* SSCOP_TPI_DRV_NAME */

#ifndef SSCOP_TPI_CMAJOR_0
#define SSCOP_TPI_CMAJOR_0	240
#endif				/* SSCOP_TPI_CMAJOR_0 */

#ifndef SSCOP_TPI_NMINOR
#define SSCOP_TPI_NMINOR	255
#endif				/* SSCOP_TPI_NMINOR */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		SSCOP_TPI_DRV_ID
#define DRV_NAME	SSCOP_TPI_DRV_NAME
#ifdef MODULE
#define DRV_BANNER	SSCOP_TPI_BANNER
#else				/* MODULE */
#define DRV_BANNER	SSCOP_TPI_SPLASH
#endif				/* MODULE */

MODULE_STATIC struct module_info sscop_tpiinfo = {
	DRV_ID,				/* Module ID number *//* FIXME */
	DRV_NAME,			/* Module name */
	1,				/* Min packet size accepted */
	512,				/* Max packet size accepted */
	8 * 512,			/* Hi water mark */
	1 * 512				/* Lo water mark */
};

STATIC int sscop_rput(queue_t *, mblk_t *);
STATIC int sscop_rsrv(queue_t *);

STATIC int sscop_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sscop_close(queue_t *, int, cred_t *);

STATIC struct qinit sscop_rinit = {
	sscop_rput,			/* Read put (msg from below) */
	sscop_rsrv,			/* Read queue service */
	sscop_open,			/* Each open */
	sscop_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&sscop_info,			/* Information */
	NULL				/* Statistics */
};

STATIC int sscop_wput(queue_t *, mblk_t *);
STATIC int sscop_wsrv(queue_t *);

STATIC struct qinit sscop_winit = {
	sscop_wput,			/* Write put (msg from above) */
	sscop_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&sscop_info,			/* Information */
	NULL				/* Statistics */
};

MODULE_STATIC struct streamtab sscop_info = {
	&sscop_rinit,			/* Upper read queue */
	&sscop_winit,			/* Upper write queue */
	NULL,				/* Lower read queue */
	NULL				/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  SSCOP Private Structure
 *
 *  =========================================================================
 */

/*
 *  SSCOP Protocol Parameters and Defaults
 */
int sscop_some_parameters = 3 * HZ;

#define SSCOP_PRIV(__q) ((sscop_t *)(__q)->q_ptr)

kmem_cachep_t sscop_priv_cachep = NULL;

struct sscop {
	struct sscop *next;		/* linkage for master list */
	struct sscop *pprev;		/* linkage for master list */
};

/*
 *  =========================================================================
 *
 *  SSCOP Message Structures
 *
 *  =========================================================================
 */

#define SSCOP_BGN
#define SSCOP_BGACK
#define SSCOP_BGREJ
#define SSCOP_END
#define SSCOP_ENDAK
#define SSCOP_RS
#define SSCOP_RSAK
#define SSCOP_ER
#define SSCOP_ERAK
#define SSCOP_SD
#define SSCOP_POLL
#define SSCOP_STAT
#define SSCOP_USTAT
#define SSCOP_UD
#define SSCOP_MD

/*
 *  =========================================================================
 *
 *  SSCOP T-Provider --> T-User Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 *
 *  These functions package up the necessary primitive format given the
 *  input parameters necessary to create them.  They are all inlined and
 *  return NULL if a message block could not be allocated.  The reason for not
 *  passing the mblk up the queue yet is because the caller may wish to
 *  allocate the response before committing to processing the action which
 *  causes these indications and confirmations to the T-User.
 */

#include "sscop_t_prov.h"

/*
 *  =========================================================================
 *
 *  SSCOP T-Provider --> N-Provider (IP) Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 *
 *  These functions package up the necessary primitive format given the
 *  input parameters necessary to create them.  They are all inlined and
 *  return NULL if a message block could not be allocated.
 */

/*
 *  N_UNITDATA_REQ
 *  -------------------------------------------------------------------------
 */
STATIC inline mblk_t *
n_unitdata_req(caddr_t dst_ptr, size_t dst_len)
{
	mblk_t *mp;
	N_unitdata_req_t *p;
	if ((mp = allocb(sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_unitdata_req_t *) mp->b_wptr;
		p->PRIM_type = N_DISCON_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->RESERVED_field[0] = 0;
		p->RESERVED_field[1] = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	return (mp);
}

/*
 *  =========================================================================
 *
 *  SSCOP --> SSCOP Peer Primitives (Send Messages)
 *
 *  =========================================================================
 */

#include "sscop_send.h"

/*
 *  =========================================================================
 *
 *  SSCOP State Machine TIMEOUTS
 *
 *  =========================================================================
 */

#include "sscop_time.h"

/*
 *  =========================================================================
 *
 *  SSCOP Peer --> SSCOP Primitives (Received Messages)
 *
 *  =========================================================================
 */

extern int sscop_recv_msg(const queue_t *, const mblk_t *);

/*
 *  =========================================================================
 *
 *  N-Provider --> N-User (SSCOP) Primitives (M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */

/*
 *  N_UNITDATA_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
n_unitdata_ind(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	N_unitdata_ind_t *p = (N_unitdata_ind_t *) pdu->b_rptr;
	const caddr_t dst_ptr = p->DEST_offset + pdu->b_rptr;
	const size_t dst_len = p->DEST_length;
	const caddr_t src_ptr = p->SRC_offset + pdu->b_rptr;
	const size_t src_len = p->SRC_length;
}

/*
 *  N_UDERROR_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
n_uderror_ind(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	N_uderror_ind_t *p = (N_uderror_ind_t *) pdu->b_rptr;
	const caddr_t dst_ptr = p->DEST_offset + pdu->b_rptr;
	const size_t dst_len = p->DEST_length;
	const uint error = p->ERROR_type;
}

STATIC int (*n_prim[]) (queue_t *, mblk_t *) = {
	NULL,			/* N_CONN_REQ 0 */
	    NULL,		/* N_CONN_RES 1 */
	    NULL,		/* N_DISCON_REQ 2 */
	    NULL,		/* N_DATA_REQ 3 */
	    NULL,		/* N_EXDATA_REQ 4 */
	    NULL,		/* N_INFO_REQ 5 */
	    NULL,		/* N_BIND_REQ 6 */
	    NULL,		/* N_UNBIND_REQ 7 */
	    NULL,		/* N_UNITDATA_REQ 8 */
	    NULL,		/* N_OPTMGMT_REQ 9 */
	    NULL,		/* not used 10 */
	    NULL,		/* N_CONN_IND 11 */
	    NULL,		/* N_CONN_CON 12 */
	    NULL,		/* N_DISCON_IND 13 */
	    NULL,		/* N_DATA_IND 14 */
	    NULL,		/* N_EXDATA_IND 15 */
	    NULL,		/* N_INFO_ACK 16 */
	    NULL,		/* N_BIND_ACK 17 */
	    NULL,		/* N_ERROR_ACK 18 */
	    NULL,		/* N_OK_ACK 19 */
	    &n_unitdata_ind,	/* N_UNITDATA_IND 20 */
	    &n_uderror_ind,	/* N_UDERROR_IND 21 */
	    NULL,		/* not used 22 */
	    NULL,		/* N_DATACK_REQ 23 */
	    NULL,		/* N_DATACK_IND 24 */
	    NULL,		/* N_RESET_REQ 25 */
	    NULL,		/* N_RESET_IND 26 */
	    NULL,		/* N_RESET_RES 27 */
	    NULL		/* N_RESET_CON 28 */
};

/*
 *  =========================================================================
 *
 *  T-User --> T-Provider (SSCOP) Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */

extern int sscop_w_proto(queue_t *, mblk_t *);
extern int sscop_w_pcproto(queue_t *, mblk_t *);

/*
 *  =========================================================================
 *
 *  SSCOP IOCTLs
 *
 *  =========================================================================
 */

STATIC int (*sscop_ioctl[]) (queue_t *, uint, void *) = {
};

/*
 *  =========================================================================
 *
 *  SSCOP STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline int
sscop_w_data(queue_t *q, mblk_t *mp)
{
	return sscop_write_data(q, mp);
}
STATIC inline int
sscop_r_data(queue_t *q, mblk_t *mp)
{
	return sscop_recv_sdu(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */

STATIC inline int
sscop_r_proto(queue_t *q, mblk_t *mp)
{
	int prim = ((union N_primitives *) (mp->b_rptr))->type;
	if (0 <= prim && prim < sizeof(n_prim) / sizeof(void *))
		if (n_prim[prim])
			return (*n_prim[prim]) (q, mp);
	return (-EOPNOTSUPP);
}
STATIC inline int
sscop_r_pcproto(queue_t *q, mblk_t *mp)
{
	return sscop_r_proto(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_CTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline int
sscop_w_ctl(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (-EOPNOTSUPP);
}
STATIC inline int
sscop_r_ctl(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline int
sscop_w_ioctl(queue_t *q, mblk_t *mp)
{
	int ret;
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	int cmd = iocp->ioc_cmd;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int type = _IOC_TYPE(cmd);
	int nr = _IOC_NR(cmd);
	int size = _IOC_SIZE(cmd);

	switch (type) {
	case __SID:
		switch (cmd) {
		case I_LINK:
		case I_PLINK:
		case I_UNLINK:
		case I_PUNLINK:
		{
			struct linkblk *lp = (struct linkblk *) arg;
			(void) lp;
		}
		default:
		case I_FDINSERT:
			ret = -EINVAL;
		}
		break;

	case SSCOP_IOC_MAGIC:
		if (iocp->ioc_count >= size) {
			if (0 <= nr && nr < sizeof(sscop_ioctl) / sizeof(void *)) {
				if (sscop_ioctl[nr])
					ret = (*sscop_ioctl[nr]) (q, cmd, arg);
				else
					ret = -EOPNOTSUPP;
			}
		} else
			return = -EINVAL;
		break;

	default:
		if (q->q_next) {
			putnext(q, mp);
			return (0);
		}
		ret = -EOPNOTSUPP;
	}
	mp->b_datap->db_type = ret ? M_IOCNAK : M_IOCACK;
	iocp->ioc_error = -ret;
	iocp->ioc_rval = ret ? -1 : 0;
	qrepy(q, mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline void
sscop_w_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (q - q_next) {
			putnext(q, mp);
			return;
		}
		*mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return;
	}
	if (q->q_next) {
		putnext(q, mp);
		return;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC inline void
sscop_r_error(queue_t *q, mblk_t *mp)
{
	sscop_t *sp = SSCOP_PRIV(q);
	sp->zapped = 1;
	if (q->q_next) {
		putnext(q, mp);
		return;
	}
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE PUT and QUEUE SERVICE routines
 *
 *  =========================================================================
 *
 *  READ QUEUE PUT and SRV routines
 *
 *  -------------------------------------------------------------------------
 *
 *  SSCOP RPUT - Message from below.
 *
 *  If the message is a priority message we attempt to process it immediately.
 *  If the message is a non-priority message, but there are no messages on the
 *  queue yet, we attempt to process it immediately.  If the message is not
 *  supported, we pass it up-queue if possible.  If the message cannot be
 *  processed immediately we place it on the queue.
 */
STATIC int
sscop_rput(queue_t *q, mblk_t *mp)
{
	int err = -EOPNOTSUPP;

	if (mp->b_datap->db_type < QPCTL && q->q_count) {
		putq(q, mp);
		/* 
		 *  NOTE:- after placing messages on the queue here, I should
		 *  check if placing the message on the queue crosses a band
		 *  threshold for congestion accept and congestion discard.
		 *  When crossing congestion accept, I should sent busy to the
		 *  peer and notify MTP3.  When crossing congestion discard I
		 *  should notify MTP3.
		 */
		return (0);
	}
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if ((err = sscop_r_data(q, mp)))
			break;
		return (0);
	case M_PROTO:
		if ((err = sscop_r_proto(q, mp)))
			break;
		return (0);
	case M_PCPROTO:
		if ((err = sscop_r_pcproto(q, mp)))
			break;
		return (0);
	case M_CTL:
		if ((err = sscop_r_ctl(q, mp)))
			break;
		return (0);
	case M_ERROR:
		sscop_r_error(q, mp);
		return (0);
	}
	switch (err) {
	case -EAGAIN:
		putq(q, mp);
		return (0);
	case -EOPNOTSUPP:
		if (q->q_next) {
			putnext(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (err);
}

/*
 *  SSCOP RSRV - Queued message from below.
 *
 *  If the message is a priority message we attempt to process it immediately
 *  and without flow control.  If the message is a non-priority message and
 *  the next queue is flow controlled, we put the message back on the queue
 *  and wait.  If we cannot process a priority message immediately we cannot
 *  place it back on the queue and discard it.  We requeue non-priority
 *  messages which cannot be processed immediately.  Unrecognized messages are
 *  passed down-queue.
 */
STATIC int
sscop_rsrv(queue_t *q)
{
	mblk_t *mp;
	int err = -EOPNOTSUPP;

	while ((mp = getq(q))) {
		if (mp->b_datap->db_type < QPCTL && !canputnext(q)) {
			putbq(q, mp);
			return (0);
		}
		switch (mp->b_datap->db_type) {
		case M_DATA:
			if ((err = sscop_r_data(q, mp)))
				break;
			goto rsrv_continue;
		case M_PROTO:
			if ((err = sscop_r_proto(q, mp)))
				break;
			goto rsrv_continue;
		case M_PCPROTO:
			if ((err = sscop_r_pcproto(q, mp)))
				break;
			goto rsrv_continue;
		case M_CTL:
			if ((err = sscop_r_ctl(q, mp)))
				break;
			goto rsrv_continue;
		}
		switch (err) {
		case -EAGAIN:
			if (mp->b_datap->db_type < QPCTL) {
				putbq(q, mp);
				return (0);
			}
		case -EOPNOTSUPP:
			if (q->q_next) {
				putnext(q, mp);
				goto rsrv_continue;
			}
			break;
		}
		freemsg(mp);
	      rsrv_continue:
		/* 
		 *  NOTE:-  I have removed and processed a message from the
		 *  receive queue, so I should check for receive congestion
		 *  abatement.  If receive congestion has abated below the
		 *  accept threshold, I should signal MTP that there is no
		 *  longer any receive congestion.
		 */
		continue;
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  WRITE QUEUE PUT and SRV routines
 *
 *  -------------------------------------------------------------------------
 *
 *  SSCOP WPUT - Message from above.
 *
 *  If the message is priority message we attempt to process it immediately.
 *  If the message is non-priority message, but there are no messages on the
 *  queue yet, we attempt to process it immediately.  If the message is not
 *  supported, we pass it down-queue if possible.  If the message cannot be
 *  processed immediately, we place it on the queue.
 */
STATIC int
sscop_wput(queue_t *q, mblk_t *mp)
{
	mblk_t *mp;
	int err = -EOPNOTSUPP;
	if (q->q_count && mp->b_datap->db_type < QPCTL) {
		putq(q, mp);
		/* 
		 *  NOTE:- after placing messages on the queue here, I should
		 *  check for transmit congestion.  I should check if placing
		 *  the message on the queue crosses a band threshold for
		 *  congestion onset and abatement.  When crossing congestion
		 *  thresholds, I should notify MTP3.
		 */
		return (0);
	}
	switch (mp->b_datap->db_type) {
	case M_DATA:
		if ((err = sscop_w_data(q, mp)))
			break;
		return (0);
	case M_PROTO:
		if ((err = sscop_w_proto(q, mp)))
			break;
		return (0);
	case M_PCPROTO:
		if ((err = sscop_w_pcproto(q, mp)))
			break;
		return (0);
	case M_CTL:
		if ((err = sscop_w_ctl(q, mp)))
			break;
		return (0);
	case M_IOCTL:
		if ((err = sscop_w_ioctl(q, mp)))
			break;
		return (0);
	case M_FLUSH:
		sscop_w_flush(q, mp);
		return (0);
	}
	switch (err) {
	case -EAGAIN:
		if (mp->b_datap->db_type < QPCTL) {
			putq(q, mp);
			return (0);
		}
	case -EOPNOTSUPP:
		if (q->q_next) {
			putnext(q, mp);
			return (0);
		}
	}
	freemsg(mp);
	return (err);
}

/*
 *  SSCOP WSRV = Queued message from above.
 *
 *  If the message is a priority message we attempt to process it immediately
 *  and without flow control.  If the message is a non-priority message and
 *  the next queue is flow controlled, we put the message back on the queue
 *  and wait.  If we cannot process a priority message immediately we cannot
 *  place it back on the queue, we discard it.  We requeue non-priority
 *  messages which cannot be processed immediately.  Unrecognized messages are
 *  passed down-queue.
 */
STATIC int
sscop_wsrv(queue_t *q)
{
	int err = -EOPNOTSUPP;
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (q->q_next && mp->b_datap->db_type < QPCTL && !canputnext(q)) {
			putbq(q, mp);
			return (0);
		}
		switch (mp->b_datap->db_type) {
		case M_DATA:
			if ((err = sscop_w_data(q, mp)))
				break;
			goto wsrv_continue;
		case M_PROTO:
			if ((err = sscop_w_proto(q, mp)))
				break;
			goto wsrv_continue;
		case M_PCPROTO:
			if ((err = sscop_w_pcproto(q, mp)))
				break;
			goto wsrv_continue;
		case M_CTL:
			if ((err = sscop_w_ctl(q, mp)))
				break;
			goto wsrv_continue;
		}
		switch (err) {
		case -EAGAIN:
			if (mp->b_datap->db_type < QPCTL) {
				putbq(q, mp);
				return (0);
			}
		case -EOPNOTSUPP:
			if (q->q_next) {
				putnext(q, mp);
				goto wsrv_continue;
			}
		}
		freemsg(mp);
	      wsrv_continue:
		/* 
		 *  NOTE:-   I have removed a message from the queue, so I
		 *  should check for band congestion abatement for the data
		 *  band to see if transmit congestion has abated.  If it has,
		 *  I should notify MTP3 of transmit abatement.
		 */
		continue;
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
/*
 *  Private structure allocation and deallocation.
 *
 *  We use Linux hardware aligned cache here for speedy access to information
 *  contained in the private data structure.
 */
STATIC sscop_t *sscop_priv_list = NULL;

kmem_cache_t *sscop_cachep = NULL;

STATIC sscop_t *
sscop_alloc_priv(queue_t *q)
{
	sscop_t *sp;

	if ((sp = kmem_cache_alloc(sscop_cachep))) {
		bzero(sp, sizeof(*sp));
		RD(q)->q_ptr = WR(q)->q_tpr = sp;
		sp->rq = RD(q);
		sp->wq = WR(q);

		/* link into master list */
		if ((sp->next = sscop_priv_list))
			sp->next->pprev = &sp->next;
		sp->pprev = &sscop_priv_list;
		sscop_priv_list = sp;

		sp->t_state = TS_UNINIT;

		sp->n_ostr = sscop_default_o_streams;
		sp->n_istr = sscop_default_i_streams;
		sp->a_rwnd = sscop_default_rmem;

		bufq_init(&sp->out_of_order_queue);
		bufq_init(&sp->duplicate_queue);
		bufq_init(&sp->write_queue);
		bufq_init(&sp->urgent_queue);
		bufq_init(&sp->retrans_queue);
		bufq_init(&sp->error_queue);
		bufq_init(&sp->connect_queue);

		sp->pmtu = 576;

		sp->max_retrans = sscop_assoc_max_retrans;
		sp->max_inits = sscop_max_init_retries;
	}
	return (sp);
}

STATIC void
sscop_free_priv(queue_t *q)
{
	sscop_t *sp = SSCOP_PRIV(q);
	kmem_cache_free(sscop_cachep, sp);
	return;
}

STATIC void
sscop_init_priv(void)
{
	if (!(sscop_cachep = kmem_find_general_cachep(sizeof(sscop_t)))) {
		/* allocate a new cachep */
	}
	return;
}

STATIC int
sscop_init_caches(void)
{
	return (0);
}

STATIC int
sscop_term_caches(void)
{
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN - Each open
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sscop_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	(void) crp;

	if (q->q_ptr != NULL)
		return (0);

	/* 
	 *  FIXME: we have to open this as a driver, not a module
	 */
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		/* 
		 *  FIXME: check to make sure that the module we are being
		 *  pushed over is compatible (i.e. it is the right kind of
		 *  transport module.
		 */
		if (!(sp = kmem_cache_alloc(sscop_priv_cachep, SLAB_HWCACHE_ALIGN)))
			return ENOMEM;
		RD(q)->q_ptr = WR(q)->q_ptr = sp;
		sp->rq = RD(q);
		sp->wq = WR(q);
		/* 
		 *  TODO: more structure initialization
		 */
		return (0);
	}
	return EIO;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CLOSE - Last close
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sscop_close(queue_t *q, int flag, cred_t *crp)
{
	sscop_t *sp = SSCOP_PRIV(q);

	(void) flag;
	(void) crp;

	/* 
	 *  TODO: make sure everything is deallocated
	 */
	kmem_cache_free(sscop_priv_cachep, sp);
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

unsigned short modid = MOD_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SSCOP module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sscop_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sscop_tpiinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC int
sscop_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&sscop_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
sscop_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&sscop_fmod)) < 0)
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
sscop_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&sscop_tpiinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&sscop_tpiinfo);
		return (err);
	}
	return (0);
}

STATIC int
sscop_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&sscop_tpiinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sscop_tpiinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = sscop_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = sscop_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		sscop_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sscop_tpiterminate(void)
{
	int err;
	if ((err = sscop_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = sscop_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sscop_tpiinit);
module_exit(sscop_tpiterminate);

#endif				/* LINUX */
