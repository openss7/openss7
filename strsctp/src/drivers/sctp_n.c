/*****************************************************************************

 @(#) $RCSfile: sctp_n.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 06:39:01 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/06/22 06:39:01 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp_n.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 06:39:01 $"

static char const ident[] = "$RCSfile: sctp_n.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 06:39:01 $";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include "debug.h"
#include "bufq.h"

#include "sctp.h"
#include "sctp_defs.h"
#include "sctp_hash.h"
#include "sctp_cache.h"
#include "sctp_msg.h"

#include "sctp_n.h"

#define SCTP_N_CMINORS 255

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 *  This driver defines two user interfaces: one NPI, the other TPI.
 */

STATIC struct module_info sctp_n_minfo = {
	mi_idnum:0,			/* Module ID number */
	mi_idname:"sctp",		/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10		/* Lo water mark */
};

STATIC int sctp_n_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sctp_n_close(queue_t *, int, cred_t *);

STATIC int sctp_n_rput(queue_t *, mblk_t *);
STATIC int sctp_n_rsrv(queue_t *);

STATIC struct qinit sctp_n_rinit = {
	qi_putp:sctp_n_rput,		/* Read put (msg from below) */
	qi_srvp:sctp_n_rsrv,		/* Read queue service */
	qi_qopen:sctp_n_open,		/* Each open */
	qi_qclose:sctp_n_close,		/* Last close */
	qi_minfo:&sctp_n_minfo,		/* Information */
};

STATIC int sctp_n_wput(queue_t *, mblk_t *);
STATIC int sctp_n_wsrv(queue_t *);

STATIC struct qinit sctp_n_winit = {
	qi_putp:sctp_n_wput,		/* Write put (msg from above) */
	qi_srvp:sctp_n_wsrv,		/* Write queue service */
	qi_minfo:&sctp_n_minfo,		/* Information */
};

STATIC struct streamtab sctp_n_info = {
	st_rdinit:&sctp_n_rinit,	/* Upper read queue */
	st_wrinit:&sctp_n_winit,	/* Upper write queue */
};

#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6

/*
 *  =========================================================================
 *
 *  NPI Provider (SCTP) -> NPI User Primitives
 *
 *  =========================================================================
 */

/*
 *  N_CONN_IND      11 - Incoming connection indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_conn_ind(sctp_t * sp, mblk_t *cp)
{
	mblk_t *mp;
	N_conn_ind_t *p;
	N_qos_sel_conn_sctp_t *q;
	struct sctp_cookie_echo *m = (struct sctp_cookie_echo *) cp->b_rptr;
	struct sctp_cookie *ck = (struct sctp_cookie *) m->cookie;
	size_t danum = ck->danum + 1;
	uint32_t *daptr = (uint32_t *) (((caddr_t) (ck + 1) + ck->opt_len));
	size_t dst_len = sizeof(uint16_t) + sizeof(uint32_t);
	size_t src_len = danum ? sizeof(uint16_t) + danum * sizeof(uint32_t) : 0;
	size_t qos_len = sizeof(*q);

	ensure(((1 << sp->i_state) & (NSF_IDLE | NSF_WRES_CIND)), return (-EFAULT));
	if (bufq_length(&sp->conq) < sp->conind) {
		if (canputnext(sp->rq)) {
			if ((mp = allocb(sizeof(*p) + dst_len + src_len + qos_len, BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = (N_conn_ind_t *) mp->b_wptr;
				p->PRIM_type = N_CONN_IND;
				p->DEST_length = dst_len;
				p->DEST_offset = dst_len ? sizeof(*p) : 0;
				p->SRC_length = src_len;
				p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
				p->SEQ_number = (ulong) cp;
				p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
				p->QOS_length = qos_len;
				p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
				mp->b_wptr += sizeof(*p);

				*((uint16_t *) mp->b_wptr)++ = ck->sport;
				*((uint32_t *) mp->b_wptr)++ = ck->saddr;

				if (danum)
					*((uint16_t *) mp->b_wptr)++ = ck->dport;
				if (danum--)
					*((uint32_t *) mp->b_wptr)++ = ck->daddr;
				while (danum--)
					*((uint32_t *) mp->b_wptr)++ = *daptr++;

				q = (N_qos_sel_conn_sctp_t *) mp->b_wptr;
				q->n_qos_type = N_QOS_SEL_CONN_SCTP;
				q->i_streams = ck->n_istr;
				q->o_streams = ck->n_ostr;
				mp->b_wptr += sizeof(*q);

				bufq_queue(&sp->conq, cp);

				sp->i_state = NS_WRES_CIND;
				putnext(sp->rq, mp);
				return (0);
			}
			seldom();
			return (-ENOBUFS);
		}
		seldom();
		return (-EBUSY);
	}
	seldom();
	return (-ERESTART);
}

/*
 *  N_CONN_CON      12 - Connection established
 *  ---------------------------------------------------------------
 */
STATIC int
n_conn_con(sctp_t * sp)
{
	mblk_t *mp;
	N_conn_con_t *p;
	N_qos_sel_conn_sctp_t *q;
	struct sctp_daddr *sd = sp->daddr;
	size_t res_len = sp->danum ? sizeof(uint16_t) + sp->danum * sizeof(sd->daddr) : 0;
	size_t qos_len = sizeof(*q);

	ensure((sp->i_state == NS_WCON_CREQ), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + res_len + qos_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
			p = (N_conn_con_t *) mp->b_wptr;
			p->PRIM_type = N_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
			p->QOS_length = qos_len;
			p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
			mp->b_wptr += sizeof(*p);

			if (sd)
				*((uint16_t *) mp->b_wptr)++ = sp->dport;
			for (; sd; sd = sd->next)
				*((uint32_t *) mp->b_wptr)++ = sd->daddr;

			q = (N_qos_sel_conn_sctp_t *) mp->b_wptr;
			q->n_qos_type = N_QOS_SEL_CONN_SCTP;
			q->i_streams = sp->n_istr;
			q->o_streams = sp->n_ostr;
			mp->b_wptr += sizeof(*q);

			sp->i_state = NS_DATA_XFER;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  N_DISCON_IND    13 - NC disconnected
 *  ---------------------------------------------------------------
 *  For SCTP the responding address in a NC connection refusal is always the
 *  destination address to which the NC connection request was sent.
 */
STATIC int
n_discon_ind(sctp_t * sp, ulong orig, long reason, mblk_t *seq)
{
	mblk_t *mp;
	N_discon_ind_t *p;

	ensure(((1 << sp->
		 i_state) & (NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_RREQ |
			     NSF_WRES_RIND)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_discon_ind_t *) mp->b_wptr;
			p->PRIM_type = N_DISCON_IND;
			p->DISCON_orig = orig;
			p->DISCON_reason = reason;
			p->RES_length = 0;
			p->RES_offset = 0;
			p->SEQ_number = (ulong) seq;
			mp->b_wptr += sizeof(*p);
			if (seq) {
				bufq_unlink(&sp->conq, seq);
				freemsg(seq);
			}
			if (!bufq_length(&sp->conq))
				sp->i_state = NS_IDLE;
			else
				sp->i_state = NS_WRES_CIND;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  N_DATA_IND      14 - Incoming connection-mode data indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_data_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint more,
	   mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;
	N_qos_sel_data_sctp_t *q;

	if (!((1 << sp->i_state) & (NSF_DATA_XFER))) {
		printd(("Interace in state %u\n", sp->i_state));
		printd(("mblk size is %d\n", msgdsize(dp)));
	}
	ensure(((1 << sp->i_state) & (NSF_DATA_XFER)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_data_ind_t *) mp->b_wptr;
			p->PRIM_type = N_DATA_IND;
			p->DATA_xfer_flags = (more ? N_MORE_DATA_FLAG : 0);
			mp->b_wptr += sizeof(*p);
			q = (N_qos_sel_data_sctp_t *) mp->b_wptr;
			q->n_qos_type = N_QOS_SEL_DATA_SCTP;
			q->ppi = ppi;
			q->sid = sid;
			q->ssn = ssn;
			q->tsn = tsn;
			q->more = more ? 1 : 0;
			mp->b_wptr += sizeof(*q);
			mp->b_cont = dp;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  N_EXDATA_IND    15 - Incoming expedited data indication
 *  ---------------------------------------------------------------
 *  The lack of a more flag presents a challenge.  Perhaps we should be
 *  reassembling expedited data.
 */
STATIC int
n_exdata_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint more,
	     mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_ind_t *p;
	N_qos_sel_data_sctp_t *q;

	ensure(((1 << sp->i_state) & (NSF_DATA_XFER)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
			p = (N_exdata_ind_t *) mp->b_wptr;
			p->PRIM_type = N_EXDATA_IND;
			mp->b_wptr += sizeof(*p);
			q = (N_qos_sel_data_sctp_t *) mp->b_wptr;
			q->n_qos_type = N_QOS_SEL_DATA_SCTP;
			q->ppi = ppi;
			q->sid = sid;
			q->ssn = ssn;
			q->tsn = tsn;
			q->more = more ? 1 : 0;
			mp->b_wptr += sizeof(*q);
			mp->b_cont = dp;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  N_INFO_ACK      16 - Information Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_info_ack(sctp_t * sp)
{
	mblk_t *mp;
	N_info_ack_t *p;
	N_qos_sel_info_sctp_t *q;
	N_qos_range_info_sctp_t *r;
	size_t add_len = sp->sanum ? sizeof(uint16_t) + sp->sanum * sizeof(uint32_t) : 0;
	size_t qos_len = sizeof(*q);
	size_t qor_len = sizeof(*r);
	size_t pro_len = sizeof(uint32_t);
	sctp_saddr_t *ss = sp->saddr;

	if ((mp = allocb(sizeof(*p) + add_len + qos_len + qor_len + pro_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_info_ack_t *) mp->b_wptr;
		p->PRIM_type = N_INFO_ACK;
		p->NSDU_size = -1;	/* no limit on NSDU size */
		p->ENSDU_size = -1;	/* no limit on ENSDU size */
		p->CDATA_size = -1;	/* no limit on CDATA size */
		p->DDATA_size = -1;	/* no limit on DDATA size */
		p->ADDR_size = -1;	/* no limit on ADDR size */
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + add_len : 0;
		p->QOS_range_length = qor_len;
		p->QOS_range_offset = qor_len ? sizeof(*p) + add_len + qos_len : 0;
		p->OPTIONS_flags =
		    REC_CONF_OPT | EX_DATA_OPT | ((sp->flags & SCTP_FLAG_DEFAULT_RC_SEL) ?
						  DEFAULT_RC_SEL : 0);
		p->NIDU_size = -1;	/* no limit on NIDU size */
		p->SERV_type = N_CONS;
		p->CURRENT_state = sp->i_state;
		p->PROVIDER_type = N_SUBNET;
		p->NODU_size = sp->pmtu >> 1;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len + qos_len + qor_len : 0;
		p->NPI_version = N_VERSION_2;
		mp->b_wptr += sizeof(*p);

		if (ss)
			*((uint16_t *) mp->b_wptr)++ = sp->sport;
		for (; ss; ss = ss->next)
			*((uint32_t *) mp->b_wptr)++ = ss->saddr;

		q = (N_qos_sel_info_sctp_t *) mp->b_wptr;
		q->n_qos_type = N_QOS_SEL_INFO_SCTP;
		if ((1 << sp->i_state) & (NSF_UNBND | NSF_IDLE | NSF_WCON_CREQ)) {
			q->i_streams = sp->max_istr;
			q->o_streams = sp->req_ostr;
		} else {
			q->i_streams = sp->n_istr;
			q->o_streams = sp->n_ostr;
		}
		q->ppi = sp->ppi;
		q->sid = sp->sid;
		q->max_inits = sp->max_inits;
		q->max_retrans = sp->max_retrans;
		q->ck_life = sp->ck_life;
		q->ck_inc = sp->ck_inc;
		q->hmac = sp->hmac;
		q->throttle = sp->throttle;
		q->max_sack = sp->max_sack;
		q->rto_ini = sp->rto_ini;
		q->rto_min = sp->rto_min;
		q->rto_max = sp->rto_max;
		q->rtx_path = sp->rtx_path;
		q->hb_itvl = sp->hb_itvl;
		mp->b_wptr += sizeof(*q);

		r = (N_qos_range_info_sctp_t *) mp->b_wptr;
		r->n_qos_type = N_QOS_RANGE_INFO_SCTP;
		mp->b_wptr += sizeof(*r);

		*((uint32_t *) mp->b_wptr)++ = sp->ppi;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/*
 *  N_BIND_ACK      17 - NS User bound to network address
 *  ---------------------------------------------------------------
 */
STATIC int
n_bind_ack(sctp_t * sp)
{
	mblk_t *mp;
	N_bind_ack_t *p;
	sctp_saddr_t *ss = sp->saddr;
	size_t add_len = sp->sanum ? sizeof(sp->sport) + sp->sanum * sizeof(ss->saddr) : 0;
	size_t pro_len = sizeof(sp->ppi);

	if ((mp = allocb(sizeof(*p) + add_len + pro_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_bind_ack_t *) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = sp->conind;
		p->TOKEN_value = (ulong) sp->rq;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
		mp->b_wptr += sizeof(*p);

		if (ss)
			*((typeof(sp->sport) *) mp->b_wptr)++ = sp->sport;
		for (; ss; ss = ss->next)
			*((typeof(ss->saddr) *) mp->b_wptr)++ = ss->saddr;

		*((typeof(sp->ppi) *) mp->b_wptr)++ = sp->ppi;

		sp->i_state = NS_IDLE;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/*
 *  N_ERROR_ACK     18 - Error Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_error_ack(sctp_t * sp, int prim, int err)
{
	mblk_t *mp;
	N_error_ack_t *p;

	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	case 0:
		never();
		return (err);
	}
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_error_ack_t *) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = err < 0 ? NSYSERR : err;
		p->UNIX_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
		switch (sp->i_state) {
		case NS_WACK_OPTREQ:
		case NS_WACK_UREQ:
		case NS_WCON_CREQ:
			sp->i_state = NS_IDLE;
			break;
		case NS_WCON_RREQ:
			sp->i_state = NS_DATA_XFER;
			break;
		case NS_WACK_BREQ:
			sp->i_state = NS_UNBND;
			break;
		case NS_WACK_CRES:
			sp->i_state = NS_WRES_CIND;
			break;
		case NS_WACK_DREQ6:
			sp->i_state = NS_WCON_CREQ;
			break;
		case NS_WACK_DREQ7:
			sp->i_state = NS_WRES_CIND;
			break;
		case NS_WACK_DREQ9:
			sp->i_state = NS_DATA_XFER;
			break;
		case NS_WACK_DREQ10:
			sp->i_state = NS_WCON_RREQ;
			break;
		case NS_WACK_DREQ11:
			sp->i_state = NS_WRES_RIND;
			break;
			/*
			 *  Note: if we are not in a WACK state we simply do
			 *  not change state.  This occurs normally when we
			 *  send NOUTSTATE or NNOTSUPPORT or are responding
			 *  to an N_OPTMGMT_REQ in other than the NS_IDLE
			 *  state.
			 */
		}
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/*
 *  N_OK_ACK        19 - Success Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_ok_ack(sctp_t * sp, ulong prim, ulong seq, ulong tok)
{
	mblk_t *mp;
	N_ok_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_ok_ack_t *) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		switch (sp->i_state) {
		case NS_WACK_OPTREQ:
			sp->i_state = NS_IDLE;
			break;
		case NS_WACK_RRES:
			sp->i_state = NS_DATA_XFER;
			break;
		case NS_WACK_UREQ:
			sp->i_state = NS_UNBND;
			break;
		case NS_WACK_CRES:
		{
			queue_t *aq = (queue_t *) tok;
			sctp_t *ap = (sctp_t *) aq->q_ptr;

			if (ap) {
				ap->i_state = NS_DATA_XFER;
				sctp_cleanup_read(sp);	/* deliver to user what is possible */
				sctp_transmit_wakeup(ap);	/* reply to peer what is necessary */
			}
			if (seq) {
				bufq_unlink(&sp->conq, (mblk_t *) seq);
				freemsg((mblk_t *) seq);
			}
			if (aq != sp->rq) {
				if (bufq_length(&sp->conq))
					sp->i_state = NS_WRES_CIND;
				else
					sp->i_state = NS_IDLE;
			}
			break;
		}
		case NS_WACK_DREQ7:
			if (seq) {
				bufq_unlink(&sp->conq, (mblk_t *) seq);
				freemsg((mblk_t *) seq);
			}
		case NS_WACK_DREQ6:
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			if (bufq_length(&sp->conq))
				sp->i_state = NS_WRES_CIND;
			else
				sp->i_state = NS_IDLE;
			break;
			/*
			 *  Note: if we are not in a WACK state we simply do
			 *  not change state.  This occurs normally when we
			 *  are responding to an N_OPTMGMT_REQ in other than
			 *  the NS_IDLE state.
			 */
		}
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/*
 *  N_DATACK_IND    24 - Data acknowledgement indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_datack_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn)
{
	mblk_t *mp;
	N_datack_ind_t *p;
	N_qos_sel_data_sctp_t *q;

	ensure(((1 << sp->i_state) & (NSF_DATA_XFER)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_datack_ind_t *) mp->b_wptr;
			p->PRIM_type = N_DATACK_IND;
			mp->b_wptr += sizeof(*p);
			q = (N_qos_sel_data_sctp_t *) mp->b_wptr;
			q->n_qos_type = N_QOS_SEL_DATA_SCTP;
			q->ppi = ppi;
			q->sid = sid;
			q->ssn = ssn;
			q->tsn = tsn;
			q->more = 0;
			mp->b_wptr += sizeof(*q);
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  N_RESET_IND     26 - Inccoming NC reset request indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_reset_ind(sctp_t * sp, ulong orig, ulong reason, mblk_t *cp)
{
	mblk_t *mp;
	N_reset_ind_t *p;

	ensure(((1 << sp->i_state) & (NSF_DATA_XFER)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_reset_ind_t *) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = orig;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);

			bufq_queue(&sp->conq, cp);

			sp->i_state = NS_WRES_RIND;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  N_RESET_CON     28 - Reset processing complete
 *  ---------------------------------------------------------------
 */
STATIC int
n_reset_con(sctp_t * sp)
{
	mblk_t *mp;
	N_reset_con_t *p;

	ensure(((1 << sp->i_state) & (NSF_WCON_RREQ)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_reset_con_t *) mp->b_wptr;
			p->PRIM_type = N_RESET_CON;
			mp->b_wptr += sizeof(*p);
			sp->i_state = NS_DATA_XFER;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

#if 0
/*
 *  N_RECOVER_IND   29 - NC Recovery indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_recover_ind(void)
{
	mblk_t *mp;
	N_recover_ind_t *p;

	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_recover_ind_t *) mp->b_wptr;
			p->PRIM_type = N_RECOVER_IND;
			mp->b_wptr += sizeof(*p);
			sp->i_state = NS_DATA_XFER;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  N_RETRIEVE_IND  32 - NC Retrieval indication
 *  ---------------------------------------------------------------
 */
STATIC mblk_t *
n_retrieve_ind(mblk_t *dp)
{
	mblk_t *mp;
	N_retrieve_ind_t *p;

	ensure(((1 << sp->i_state) & (NSF_IDLE)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_retrieve_ind_t *) mp->b_wptr;
			p->PRIM_type = N_RETRIEVE_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			sp->i_state = NS_IDLE;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/*
 *  N_RETRIEVE_CON  33 - NC Retrieval complete confirmation
 *  ---------------------------------------------------------------
 */
STATIC mblk_t *
n_retrieve_con(void)
{
	mblk_t *mp;
	N_retrieve_con_t *p;

	ensure(((1 << sp->i_state) & (NSF_IDLE)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_retrieve_con_t *) mp->b_wptr;
			p->PRIM_type = N_RETREIVE_CON;
			mp->b_wptr += sizeof(*p);
			sp->i_state = NS_IDLE;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}
#endif

STATIC int
sctp_n_conn_ind(sctp_t * sp, mblk_t *cp)
{
	// ptrace(("sp = %x, CONN_IND: seq = %x\n", (uint)sp, (uint)cp));
	return n_conn_ind(sp, cp);
}
STATIC int
sctp_n_conn_con(sctp_t * sp)
{
	// ptrace(("sp = %x, CONN_CONF\n", (uint)sp));
	return n_conn_con(sp);
}
STATIC int
sctp_n_data_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
		uint more, mblk_t *dp)
{
	// ptrace(("sp = %x, DATA_IND: ppi=%u,sid=%u,ssn=%u,tsn=%u,ord=%u,more=%u\n", (uint)sp,
	// ppi,sid,ssn,tsn,ord,more));
	if (ord)
		return n_data_ind(sp, ppi, sid, ssn, tsn, more, dp);
	else
		return n_exdata_ind(sp, ppi, sid, ssn, tsn, more, dp);
}
STATIC int
sctp_n_datack_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn)
{
	// ptrace(("sp = %x, DATACK_IND: ppi=%u,sid=%u,ssn=%u,tsn=%u\n", (uint)sp,
	// ppi,sid,ssn,tsn));
	return n_datack_ind(sp, ppi, sid, ssn, tsn);
}
STATIC int
sctp_n_reset_ind(sctp_t * sp, ulong orig, ulong reason, mblk_t *cp)
{
	// ptrace(("sp = %x, RESET_IND\n", (uint)sp));
	return n_reset_ind(sp, orig, reason, cp);
}
STATIC int
sctp_n_reset_con(sctp_t * sp)
{
	// ptrace(("sp = %x, RESET_CON\n", (uint)sp));
	return n_reset_con(sp);
}
STATIC int
sctp_n_discon_ind(sctp_t * sp, ulong orig, long reason, mblk_t *cp)
{
	// ptrace(("sp = %x, DISCON_IND\n", (uint)sp));
	return n_discon_ind(sp, orig, reason, cp);
}
STATIC int
sctp_n_ordrel_ind(sctp_t * sp)
{
	// ptrace(("sp = %x, ORDREL_IND\n", (uint)sp));
	sctp_ordrel_req(sp);	/* shut it down right now */
	return sctp_n_discon_ind(sp, 0, 0, NULL);
}

STATIC struct sctp_ifops n_ops = {
	sctp_n_conn_ind,
	sctp_n_conn_con,
	sctp_n_data_ind,
	sctp_n_datack_ind,
	sctp_n_reset_ind,
	sctp_n_reset_con,
	sctp_n_discon_ind,
	sctp_n_ordrel_ind,
	NULL,
	NULL
};

/*
 *  =========================================================================
 *
 *  NPI User --> NPI Provider (SCTP) Primitives
 *
 *  =========================================================================
 */

/*
 *  N_CONN_REQ           0 - NC request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_conn_req(sctp_t * sp, mblk_t *mp)
{
	int err = -EFAULT;
	size_t anum;
	struct sctp_addr *a;
	N_conn_req_t *p = (N_conn_req_t *) mp->b_rptr;
	N_qos_sel_conn_sctp_t *q = (N_qos_sel_conn_sctp_t *) (mp->b_rptr + p->QOS_offset);

	if (sp->i_state != NS_IDLE)
		goto outstate;
	sp->i_state = NS_WCON_CREQ;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt1;
		if (q->n_qos_type != N_QOS_SEL_CONN_SCTP)
			goto badqostype;
		if (p->QOS_length != sizeof(*q))
			goto badopt2;
	}
	a = (struct sctp_addr *) (mp->b_rptr + p->DEST_offset);
	anum = (p->DEST_length - sizeof(a->port)) / sizeof(a->addr[0]);
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length || !anum
	    || p->DEST_length != sizeof(a->port) + anum * sizeof(a->addr[0]) || !a->port)
		goto badaddr;
	if (sp->cred.cr_uid != 0 && a->port < 1024)
		goto access;
	if (p->QOS_length) {
		if (q->i_streams != -1UL)
			sp->max_istr = q->i_streams ? q->i_streams : 1;
		if (q->o_streams != -1UL)
			sp->req_ostr = q->o_streams ? q->o_streams : 1;
	}
	if (!sp->max_istr)
		sp->max_istr = 1;
	if (!sp->req_ostr)
		sp->req_ostr = 1;
	sp->flags &= ~(SCTP_FLAG_REC_CONF_OPT | SCTP_FLAG_EX_DATA_OPT);
	if (p->CONN_flags & REC_CONF_OPT)
		sp->flags |= SCTP_FLAG_REC_CONF_OPT;
	if (p->CONN_flags & EX_DATA_OPT)
		sp->flags |= SCTP_FLAG_EX_DATA_OPT;
	if ((err = sctp_conn_req(sp, a->port, a->addr, anum, mp->b_cont)))
		goto error;
	return (QR_TRIMMED);
      access:
	seldom();
	err = NACCESS;
	goto error;		/* no permission for requested address */
      badaddr:
	seldom();
	err = NBADADDR;
	goto error;		/* addresss is unusable */
      badopt2:
	seldom();
	err = NBADOPT;
	goto error;		/* options are unusable */
      badqostype:
	seldom();
	err = NBADQOSTYPE;
	goto error;		/* QOS structure type not supported */
      badopt1:
	seldom();
	err = NBADOPT;
	goto error;		/* options are unusable */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid message format */
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_CONN_REQ, err);
}

/*
 *  N_CONN_RES           1 - Accept previous connection indication
 *  -------------------------------------------------------------------------
 *  IMPLEMENTATION NOTES:- Sequence numbers are actually the address of the
 *  mblk which contains the COOKIE-ECHO chunk and contains the cookie as a
 *  connection indication.  To find if a particular sequence number is valid,
 *  we walk the connection indication queue looking for a mblk with the same
 *  address as the sequence number.  Sequence numbers are only valid on the
 *  stream for which the connection indication is queued.
 */
STATIC mblk_t *
n_seq_check(sctp_t * sp, ulong seq)
{
	mblk_t *mp = bufq_head(&sp->conq);

	for (; mp && mp != (mblk_t *) seq; mp = mp->b_next) ;
	usual(mp);
	return (mp);
}

sctp_t *sctp_n_list;
STATIC sctp_t *
n_tok_check(sctp_t * sp, ulong tok)
{
	sctp_t *ap;
	queue_t *aq = (queue_t *) tok;

	for (ap = sctp_n_list; ap && ap->rq != aq; ap = ap->next) ;
	usual(ap);
	return (ap);
}
STATIC int
n_conn_res(sctp_t * sp, mblk_t *mp)
{
	int err;
	mblk_t *cp;
	sctp_t *ap;
	N_conn_res_t *p = (N_conn_res_t *) mp->b_rptr;
	N_qos_sel_conn_sctp_t *q = (N_qos_sel_conn_sctp_t *) (mp->b_rptr + p->QOS_offset);

	if (sp->i_state != NS_WRES_CIND)
		goto outstate;
	sp->i_state = NS_WACK_CRES;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt1;
		if (q->n_qos_type != N_QOS_SEL_CONN_SCTP)
			goto badqostype;
		if (p->QOS_length != sizeof(*q))
			goto badopt2;
	}
	if (!(cp = n_seq_check(sp, p->SEQ_number)))
		goto badseq;
	if (!(ap = n_tok_check(sp, p->TOKEN_value)))
		goto badtoken1;
	if (ap != sp && !((1 << ap->i_state) & (NSF_UNBND | NSF_IDLE)))
		goto badtoken1;
	if (ap->i_state == NS_IDLE && ap->conind)
		goto badtoken2;
	/*
	   protect at least r00t streams from users 
	 */
	if (sp->cred.cr_uid != 0 && (ap->cred.cr_uid != sp->cred.cr_uid))
		goto access;
	{
		uint ap_oldstate = ap->i_state;
		uint ap_oldflags = ap->flags;

		ap->i_state = NS_DATA_XFER;
		ap->flags &= ~(SCTP_FLAG_REC_CONF_OPT | SCTP_FLAG_EX_DATA_OPT);
		if (p->CONN_flags & REC_CONF_OPT)
			ap->flags |= SCTP_FLAG_REC_CONF_OPT;
		if (p->CONN_flags & EX_DATA_OPT)
			ap->flags |= SCTP_FLAG_EX_DATA_OPT;
		if ((err = sctp_conn_res(sp, cp, ap, mp->b_cont))) {
			ap->i_state = ap_oldstate;
			ap->flags = ap_oldflags;
			goto error;
		}
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return n_ok_ack(sp, N_CONN_RES, p->SEQ_number, p->TOKEN_value);
	}
      access:
	seldom();
	err = NACCESS;
	goto error;		/* no access to accepting queue */
      badtoken2:
	seldom();
	err = NBADTOKEN;
	goto error;		/* accepting queue is listening */
      badtoken1:
	seldom();
	err = NBADTOKEN;
	goto error;		/* accepting queue id is invalid */
      badseq:
	seldom();
	err = NBADSEQ;
	goto error;		/* connection ind reference is invalid */
      badopt2:
	seldom();
	err = NBADOPT;
	goto error;		/* quality of service options are bad */
      badqostype:
	seldom();
	err = NBADQOSTYPE;
	goto error;		/* quality of service options are bad */
      badopt1:
	seldom();
	err = NBADOPT;
	goto error;		/* quality of service options are bad */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_CONN_RES, err);
}

/*
 *  N_DISCON_REQ         2 - NC disconnection request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_discon_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	N_discon_req_t *p = (N_discon_req_t *) mp->b_rptr;

	if (!
	    ((1 << sp->
	      i_state) & (NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_RREQ |
			  NSF_WRES_RIND)))
		goto outstate;
	switch (sp->i_state) {
	case NS_WCON_CREQ:
		sp->i_state = NS_WACK_DREQ6;
		break;
	case NS_WRES_CIND:
		sp->i_state = NS_WACK_DREQ7;
		break;
	case NS_DATA_XFER:
		sp->i_state = NS_WACK_DREQ9;
		break;
	case NS_WCON_RREQ:
		sp->i_state = NS_WACK_DREQ10;
		break;
	case NS_WRES_RIND:
		sp->i_state = NS_WACK_DREQ11;
		break;
	default:
		goto outstate;
	}
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->RES_length)
		goto badaddr;
	if (sp->i_state == NS_WACK_DREQ7 && !(cp = n_seq_check(sp, p->SEQ_number)))
		goto badseq;
	/*
	 *  XXX: What do we do with the disconnect reason?  Nothing?
	 */
	if ((err = sctp_discon_req(sp, cp)))
		goto error;
	return n_ok_ack(sp, N_DISCON_REQ, p->SEQ_number, 0);
      badseq:
	seldom();
	err = NBADSEQ;
	goto error;		/* connection ind reference is invalid */
      badaddr:
	seldom();
	err = NBADADDR;
	goto error;		/* responding address is inavlid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_DISCON_REQ, err);
}

/*
 *  N_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_error_reply(sctp_t * sp, int err)
{
	mblk_t *mp;

	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	case 0:
	case 1:
	case 2:
		never();
		return (err);
	}
	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}
STATIC int
n_write(sctp_t * sp, mblk_t *mp)
{
	int err;

	if (sp->i_state == NS_IDLE)
		goto discard;
	if (!((1 << sp->i_state) & (NSF_DATA_XFER | NSF_WRES_RIND)))
		goto eproto;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = 1;
		ulong more = 0;
		ulong rcpt = 0;

		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp)))
			goto error;
		return (QR_ABSORBED);	/* absorbed mp */
	}
      eproto:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      discard:
	seldom();
	return (0);
	goto error;		/* ignore in idle state */
      error:
	seldom();
	return n_error_reply(sp, err);
}
STATIC int
n_data_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_qos_sel_data_sctp_t *q = NULL;
	N_data_req_t *p = (N_data_req_t *) mp->b_rptr;

	if (sp->i_state == NS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mp->b_wptr >= mp->b_rptr + sizeof(*p) + sizeof(*q)) {
		q = (N_qos_sel_data_sctp_t *) (p + 1);
		if (q->n_qos_type != N_QOS_SEL_DATA_SCTP)
			q = NULL;
	}
	if (!((1 << sp->i_state) & (NSF_DATA_XFER | NSF_WRES_RIND)))
		goto eproto;
	{
		ulong ppi = q ? q->ppi : sp->ppi;
		ulong sid = q ? q->sid : sp->sid;
		ulong ord = 1;
		ulong more = p->DATA_xfer_flags & N_MORE_DATA_FLAG;
		ulong rcpt = p->DATA_xfer_flags & N_RC_FLAG;

		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		return (QR_TRIMMED);
	}
      eproto:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      error:
	seldom();
	return n_error_reply(sp, err);
}

/*
 *  N_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_exdata_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_qos_sel_data_sctp_t *q = NULL;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	N_exdata_req_t *p = (N_exdata_req_t *) mp->b_rptr;

	if (sp->i_state == NS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mlen >= sizeof(*p) + sizeof(*q)) {
		q = (N_qos_sel_data_sctp_t *) (p + 1);
		if (q->n_qos_type != N_QOS_SEL_DATA_SCTP)
			q = NULL;
	}
	if (!((1 << sp->i_state) & (NSF_DATA_XFER | NSF_WRES_RIND)))
		goto eproto;
	{
		ulong ppi = q ? q->ppi : sp->ppi;
		ulong sid = q ? q->sid : sp->sid;
		ulong ord = 0;
		ulong more = 0;
		ulong rcpt = 0;

		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		return (QR_TRIMMED);
	}
      eproto:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      error:
	seldom();
	return n_error_reply(sp, err);
}

/*
 *  N_INFO_REQ           5 - Information request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_info_req(sctp_t * sp, mblk_t *mp)
{
	(void) mp;
	return n_info_ack(sp);
}

/*
 *  N_BIND_REQ           6 - Bind a NS user to network address
 *  -------------------------------------------------------------------------
 */
STATIC int
n_bind_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	struct sctp_addr *a;
	size_t anum;
	N_bind_req_t *p = (N_bind_req_t *) mp->b_rptr;

	if (sp->i_state != NS_UNBND)
		goto outstate;
	sp->i_state = NS_WACK_BREQ;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	a = (struct sctp_addr *) (mp->b_rptr + p->ADDR_offset);
	anum = (p->ADDR_length - sizeof(a->port)) / sizeof(a->addr[0]);
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badaddr;
	if (p->ADDR_length != sizeof(a->port) + anum * sizeof(a->addr[0]))
		goto badaddr;
	/*
	   we don't allow wildcards just yet 
	 */
	if (!anum || (!a->port && !(a->port = sctp_get_port())))
		goto noaddr;
	if (sp->cred.cr_uid != 0 && a->port < 1024)
		goto access;
	if ((err = sctp_bind_req(sp, a->port, a->addr, anum, p->CONIND_number)))
		goto error;
	return n_bind_ack(sp);
      access:
	seldom();
	err = NACCESS;
	goto error;		/* no permission for requested address */
      noaddr:
	seldom();
	err = NNOADDR;
	goto error;		/* could not allocate address */
      badaddr:
	seldom();
	err = NBADADDR;
	goto error;		/* address is invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_BIND_REQ, err);
}

/*
 *  N_UNBIND_REQ         7 - Unbind NS user from network address
 *  -------------------------------------------------------------------------
 */
STATIC int
n_unbind_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_unbind_req_t *p = (N_unbind_req_t *) mp->b_rptr;

	(void) p;
	if (sp->i_state != NS_IDLE)
		goto outstate;
	sp->i_state = NS_WACK_UREQ;
	if ((err = sctp_unbind_req(sp)))
		goto error;
	return n_ok_ack(sp, N_UNBIND_REQ, 0, 0);
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_UNBIND_REQ, err);
}

/*
 *  N_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_optmgmt_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_optmgmt_req_t *p = (N_optmgmt_req_t *) mp->b_rptr;
	N_qos_sel_info_sctp_t *q = (N_qos_sel_info_sctp_t *) (mp->b_rptr + p->QOS_offset);

	if (sp->i_state == NS_IDLE)
		sp->i_state = NS_WACK_OPTREQ;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt;
		if (q->n_qos_type != N_QOS_SEL_INFO_SCTP)
			goto badqostype;
		if (p->QOS_length != sizeof(*q))
			goto badopt2;
	}
	if (p->QOS_length) {
		if (q->i_streams != -1UL)
			sp->max_istr = q->i_streams ? q->i_streams : 1;
		if (q->o_streams != -1UL)
			sp->req_ostr = q->o_streams ? q->o_streams : 1;
		if (q->ppi != -1UL)
			sp->ppi = q->ppi;
		if (q->sid != -1UL)
			sp->sid = q->sid;
		if (q->max_inits != -1UL)
			sp->max_inits = q->max_inits;
		if (q->max_retrans != -1UL)
			sp->max_retrans = q->max_retrans;
		if (q->ck_life != -1UL)
			sp->ck_life = q->ck_life;
		if (q->ck_inc != -1UL)
			sp->ck_inc = q->ck_inc;
		if (q->hmac != -1UL)
			sp->hmac = q->hmac;
		if (q->throttle != -1UL)
			sp->throttle = q->throttle;
		if (q->max_sack != -1UL)
			sp->max_sack = q->max_sack;
		if (q->rto_ini != -1UL)
			sp->rto_ini = q->rto_ini;
		if (q->rto_min != -1UL)
			sp->rto_min = q->rto_min;
		if (q->rto_max != -1UL)
			sp->rto_max = q->rto_max;
		if (q->rtx_path != -1UL)
			sp->rtx_path = q->rtx_path;
		if (q->hb_itvl != -1UL)
			sp->hb_itvl = q->hb_itvl;
		if (q->options != -1UL)
			sp->options = q->options;
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		sp->flags |= SCTP_FLAG_DEFAULT_RC_SEL;
	else
		sp->flags &= ~SCTP_FLAG_DEFAULT_RC_SEL;
	return n_ok_ack(sp, N_OPTMGMT_REQ, 0, 0);
      badopt2:
	seldom();
	err = NBADOPT;
	goto error;		/* QOS options were invalid */
      badqostype:
	seldom();
	err = NBADQOSTYPE;
	goto error;		/* QOS structure type not supported */
      badopt:
	seldom();
	err = NBADOPT;
	goto error;		/* QOS options were invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return n_error_ack(sp, N_OPTMGMT_REQ, err);
}

/*
 *  N_RESET_REQ         25 - NC reset request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_reset_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_reset_req_t *p = (N_reset_req_t *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state == NS_IDLE)
		goto discard;
	if (sp->i_state != NS_DATA_XFER)
		goto outstate;
	sp->i_state = NS_WCON_RREQ;
	if ((err = sctp_reset_req(sp)))
		goto error;
	return (0);
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return n_error_ack(sp, N_RESET_REQ, err);
}

/*
 *  N_RESET_RES         27 - Reset processing accepted
 *  -------------------------------------------------------------------------
 */
STATIC int
n_reset_res(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_reset_res_t *p = (N_reset_res_t *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state == NS_IDLE)
		goto discard;
	if (sp->i_state != NS_WRES_RIND)
		goto outstate;
	sp->i_state = NS_WACK_RRES;
	if ((err = sctp_reset_res(sp)))
		goto error;
	return n_ok_ack(sp, N_RESET_RES, 0, 0);
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return n_error_ack(sp, N_RESET_RES, err);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_n_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	sctp_t *sp = (sctp_t *) q->q_ptr;
	ulong oldstate = sp->i_state;

	switch ((prim = *((ulong *) mp->b_rptr))) {
	case N_CONN_REQ:
		rtn = n_conn_req(sp, mp);
		break;
	case N_CONN_RES:
		rtn = n_conn_res(sp, mp);
		break;
	case N_DISCON_REQ:
		rtn = n_discon_req(sp, mp);
		break;
	case N_DATA_REQ:
		rtn = n_data_req(sp, mp);
		break;
	case N_EXDATA_REQ:
		rtn = n_exdata_req(sp, mp);
		break;
	case N_INFO_REQ:
		rtn = n_info_req(sp, mp);
		break;
	case N_BIND_REQ:
		rtn = n_bind_req(sp, mp);
		break;
	case N_UNBIND_REQ:
		rtn = n_unbind_req(sp, mp);
		break;
	case N_OPTMGMT_REQ:
		rtn = n_optmgmt_req(sp, mp);
		break;
	case N_RESET_REQ:
		rtn = n_reset_req(sp, mp);
		break;
	case N_RESET_RES:
		rtn = n_reset_res(sp, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		seldom();
		sp->i_state = oldstate;
	}
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_n_w_data(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;

	return n_write(sp, mp);
}
STATIC int
sctp_n_r_data(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;

	return sctp_recv_msg(sp, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_n_r_error(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;

	rare();
	return sctp_recv_err(sp, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 *  This is complete flush handling in both directions.  Standard stuff.
 */
STATIC int
sctp_n_m_flush(queue_t *q, mblk_t *mp, const uint8_t mflag, const uint8_t oflag)
{
	if (*mp->b_rptr & mflag) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		if (q->q_next) {
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		*mp->b_rptr &= ~mflag;
	}
	if (*mp->b_rptr & oflag) {
		queue_t *oq = q->q_other;
		if (*mp->b_rptr & FLUSHBAND)
			flushband(oq, mp->b_rptr[1], FLUSHALL);
		else
			flushq(oq, FLUSHALL);
		if (oq->q_next) {
			putnext(oq, mp);
			return (QR_ABSORBED);
		}
		*mp->b_rptr &= ~oflag;
	}
	return (0);
}
STATIC int
sctp_n_w_flush(queue_t *q, mblk_t *mp)
{
	return sctp_n_m_flush(q, mp, FLUSHW, FLUSHR);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_n_r_other(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = SCTP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on RD(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	putnext(q, mp);
	return (QR_ABSORBED);
}
STATIC int
sctp_n_w_other(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = SCTP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	return (-EOPNOTSUPP);
}

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */
/*
 *  NPI Write Message
 */
STATIC INLINE int
sctp_n_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sctp_n_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sctp_n_w_proto(q, mp);
	case M_FLUSH:
		return sctp_n_w_flush(q, mp);
	default:
		return sctp_n_w_other(q, mp);
	}
}

/*
 *  IP Read Message
 */
STATIC INLINE int
sctp_n_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sctp_n_r_data(q, mp);
	case M_ERROR:
		return sctp_n_r_error(q, mp);
	default:
		return sctp_n_r_other(q, mp);
	}
}

/*
 *  PUTQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int
sctp_n_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (mp->b_datap->db_type < QPCTL || q->q_count) {
		putq(q, mp);
		return (0);
	}
	if (!sctp_trylock(q)) {
		int rtn;

		switch ((rtn = proc(q, mp))) {
		case QR_DONE:
			freemsg(mp);
		case QR_ABSORBED:
			break;
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
			ptrace(("ERROR: (q dropping) %d\n", rtn));
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
		}
		sctp_unlock(q);
		return (rtn);
	} else {
		seldom();
		putq(q, mp);
		return (0);
	}
}

/*
 *  SRVQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int
sctp_n_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *))
{
	ensure(q, return (-EFAULT));
	if (!sctp_waitlock(q)) {
		int rtn = 0;
		mblk_t *mp;

		while ((mp = getq(q))) {
			switch ((rtn = proc(q, mp))) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				continue;
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
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				ptrace(("ERROR: (q disabling) %d\n", rtn));
				noenable(q);
				putbq(q, mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* proc must schedule bufcall */
			case -EBUSY:	/* proc must fail canput */
			case -ENOMEM:	/* proc must schedule re-enable */
			case -EAGAIN:	/* proc must schedule re-enable */
				if (mp->b_datap->db_type < QPCTL) {
					ptrace(("ERROR: (q stalled) %d\n", rtn));
					putbq(q, mp);
					break;
				}
				if (mp->b_datap->db_type == M_PCPROTO) {
					mp->b_datap->db_type = M_PROTO;
					mp->b_band = 255;
					putq(q, mp);
					break;
				}
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				continue;
			}
			break;
		}
		sctp_unlock(q);
		return (rtn);
	}
	return (-EAGAIN);
}

STATIC int
sctp_n_rput(queue_t *q, mblk_t *mp)
{
	return (int) sctp_n_putq(q, mp, &sctp_n_r_prim);
}

STATIC int
sctp_n_rsrv(queue_t *q)
{
	return (int) sctp_n_srvq(q, &sctp_n_r_prim);
}

STATIC int
sctp_n_wput(queue_t *q, mblk_t *mp)
{
	return (int) sctp_n_putq(q, mp, &sctp_n_w_prim);
}

STATIC int
sctp_n_wsrv(queue_t *q)
{
	return (int) sctp_n_srvq(q, &sctp_n_w_prim);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
sctp_t *sctp_n_list = NULL;

STATIC int
sctp_n_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	sctp_t *sp, **spp = &sctp_n_list;

	(void) crp;
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next) {
		rare();
		return (EIO);	/* can't open as module */
	}
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN)
		cminor = 1;
	for (; *spp && (*spp)->cmajor < cmajor; spp = &(*spp)->next) ;
	for (; *spp && cminor <= SCTP_N_CMINORS; spp = &(*spp)->next) {
		ushort dminor = (*spp)->cminor;

		if (cminor < dminor)
			break;
		if (cminor == dminor) {
			if (sflag != CLONEOPEN) {
				rare();
				return (ENXIO);	/* requested device in use */
			}
			cminor++;
		}
	}
	if (cminor > SCTP_N_CMINORS) {
		rare();
		return (ENXIO);
	}
	*devp = makedevice(cmajor, cminor);
	if (!(sp = sctp_alloc_priv(q, spp, cmajor, cminor, &n_ops))) {
		rare();
		return (ENOMEM);
	}
	return (0);
}
STATIC int
sctp_n_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sctp_free_priv(q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
void
sctp_n_init(void)
{
	int cmajor;

	if ((cmajor =
	     lis_register_strdev(SCTP_N_CMAJOR_0, &sctp_n_info, SCTP_N_CMINORS,
				 sctp_n_minfo.mi_idname)) < 0) {
		sctp_n_minfo.mi_idnum = 0;
		rare();
		cmn_err(CE_NOTE, "sctp: couldn't register driver\n");
		return;
	}
	sctp_n_minfo.mi_idnum = cmajor;
}

void
sctp_n_term(void)
{
	if (sctp_n_minfo.mi_idnum) {
		if ((sctp_n_minfo.mi_idnum = lis_unregister_strdev(sctp_n_minfo.mi_idnum))) {
			sctp_n_minfo.mi_idnum = 0;
			rare();
			cmn_err(CE_WARN, "sctp: couldn't unregister driver!\n");
		}
	}
}
