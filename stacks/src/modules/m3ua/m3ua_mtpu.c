/*****************************************************************************

 @(#) $RCSfile: m3ua_mtpu.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:20:21 $

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

 Last Modified $Date: 2004/01/17 08:20:21 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: m3ua_mtpu.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:20:21 $"

static char const ident[] =
    "$RCSfile: m3ua_mtpu.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:20:21 $";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif

#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/cmn_err.h>

#include "m3ua.h"
#include "m3ua_data.h"
#include "m3ua_ctrl.h"
#include "m3ua_msg.h"

/*
 *  =========================================================================
 *
 *  MTP-User --> M3UA (ASP) Downstream Primitives
 *
 *  =========================================================================
 */
static inline int mtp_error_reply(queue_t * q, mblk_t * pdu, int prim, int err)
{
	mblk_t *mp;
	if ((mp = mtp_error_ack(prim, err))) {
		freemsg(pdu);
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int mtp_uderror_reply(queue_t * q, mblk_t * pdu, int err)
{
	mblk_t *mp;
	N_unitdata_req_t *p = (N_unitdata_req_t *) pdu->b_rptr;
	if ((mp = mtp_uderror_ind(err,
				  p->DEST_length ? (mtp_addr_t *) (pdu->b_rptr +
								   p->DEST_offset) : 0,
				  p->SRC_length ? (mtp_addr_t *) (pdu->b_rptr + p->SRC_offset) : 0,
				  pdu->b_cont))) {
		freeb(pdu);
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static int parse_options(queue_t * q, mblk_t * pdu, int prim, caddr_t opt_ptr, size_t opt_len)
{
	mtp_t *mtp = (mtp_t *) q->q_ptr;
	if (opt_len < sizeof(uint32_t))
		return mtp_error_reply(q, pdu, prim, NBADQOSTYPE);
	switch (*((ulong *) opt_ptr)) {
	case N_QOS_SEL_MTP:
	{
		N_qos_sel_mtp_t *p = (N_qos_sel_mtp_t *) opt_ptr;
		if (prim == N_OPTMGMT_REQ)
			goto parse_options_badqostype;
		if (opt_len < sizeof(*p))
			goto parse_options_badqostype;
		if (prim != N_CONN_REQ && prim != N_CONN_RES && prim != N_UNITDATA_REQ)
			goto parse_options_badqostype;
		if (p->sls == QOS_UNKNOWN)
			mtp->sls = mtp_next_sls_value++;
		else
			mtp->sls = p->sls;
		if (p->mp == QOS_UNKNOWN)
			mtp->mp = 0;
		else
			mtp->mp = p->mp;
		break;
	}
	case N_QOS_OPT_SEL_MTP:
	{
		N_qos_opt_sel_mtp_t *p = (N_qos_sel_mtp_t *) opt_ptr;
		if (prim != N_OPTMGMT_REQ)
			goto parse_options_badqostype;
		if (opt_len < sizeof(*p))
			goto parse_options_badqostype;
		break;
	}
	default:
		goto parse_options_badqostype;
	}
	return (0);

      parse_options_badqostype:return mtp_error_reply(q, pdu, prim, NBADQOSTYPE);
}

/*
 *  N_CONN_REQ	     0 - NC request
 *  -------------------------------------------------------------------------
 *  This request is only valid for ISUP/TUP MTP-Users.  When we receive a
 *  connect request, we either have an existing AS which has the indicated
 *  Routing Key or we request that the LM queue create and return an AS.  If
 *  the AS exists and is statically provisioned, then we can perform ASPAC
 *  procedures on any associated SGP.  If no SGP are associated, we can refuse
 *  the connection request.  If SGP are associated we send an ASPAC and wait
 *  for the result before responding with an N_CONN_CON.
 */
static int mtpu_conn_req(queue_t * q, mblk_t * pdu)
{
	mtp_addr_t *a;
	mtp_t *mtp = (mtp_t *) q->q_ptr;
	N_conn_req_t *p = (N_conn_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		goto mtpu_conn_req_emsgsize;
	if (pdu->b_rptr + p->DEST_offset > pdu->b_wptr - p->DEST_length)
		goto mtpu_conn_req_badaddr;
	if (pdu->b_rptr + p->QOS_offset > pdu->b_wptr - p->QOS_length)
		goto mtpu_conn_req_badopt;
	if (p->DEST_length == 0)
		goto mtpu_conn_req_noaddr;
	if (p->DEST_length < sizeof(*a))
		goto mtpu_conn_req_badaddr;
	if (mtp->state != NS_IDLE)
		goto mtpu_conn_req_outstate;
	if ((err = parse_options(q, pdu, N_CONN_REQ, pdu->b_rptr + p->QOS_offset, p->QOS_length)))
		return (err);
	a = (mtp_addr_t *) (pdu->b_rptr + p->DEST_offset);
	bcopy(a, &mtp->dst, sizeof(*a));
	/* 
	 *  FIXME:  There is a couple of things we could do here:
	 *
	 *  1)  We could formulate a Routing Key and then check the list of
	 *      configured Routing Keys to see if one is associated with a
	 *      current AS.
	 *
	 *  2)  If we find an RK match but it is not associated with an AS,
	 *      yet it is associated with SGPs for dynamic registration, we
	 *      can create an AS and attempt to dynamically register the RK
	 *      with a REG REQ for those SGP supporting it.
	 *
	 *  3)  If we cannot find an RK match, we can ask the LM queue to map
	 *      the RK for us.
	 *
	 *  4)  If we cannot find an RK match, we can formulate a REG REQ and
	 *      send it to all available SGPs.
	 *
	 */
	mtp->state = NS_WCON_CREQ;
	freemsg(pdu);
	return (0);
      mtpu_conn_req_emsgsize:return mtp_error_reply(q, pdu, N_CONN_REQ, -EMSGSIZE);
      mtpu_conn_req_badaddr:return mtp_error_reply(q, pdu, N_CONN_REQ, NBADADDR);
      mtpu_conn_req_badopt:return mtp_error_reply(q, pdu, N_CONN_REQ, NBADOPT);
      mtpu_conn_req_noaddr:return mtp_error_reply(q, pdu, N_CONN_REQ, NNOADDR);
      mtpu_conn_req_outstate:return mtp_error_reply(q, pdu, N_CONN_REQ, NOUTSTATE);
}

/*
 *  N_DISCON_REQ     2 - NC disconnection request
 *  -------------------------------------------------------------------------
 *  This request is only valid for ISUP/TUP MTP-Users.  When we receive a
 *  disconnect request from the user, we should detach the SS7 stream from the
 *  AS and change the AS state if necessary.  If the AS state changes to
 *  INACTIVE from ACTIVE we should perorm ASPIA procedures with each
 *  associated SGP (IPC or UAP) and ASPIA Ack procedures with each associated
 *  ASP (IPC).  Reasons are not given.
 */
static int mtpu_discon_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	size_t dlen = msgdsize(pdu);
	size_t mlen = pdu->b_wptr - pdu->b_rptr;
	mtp_t *mpt = (mtp_t *) q->q_ptr;
	N_discon_req_t *p = (N_discon_req_t *) pdu->b_rptr;
	if (mlen < sizeof(*p))
		goto mtpu_discon_req_emsgsize;
	if (dlen > 0)
		goto mtpu_discon_req_baddata;
	switch (mtp->state) {
	case NS_WCON_CREQ:
		/* 
		 *  FIXME: we are trying to connect, we need to abort
		 *  the connection request.
		 */
		break;

	case NS_DATA_XFER:
	case NS_WRES_RIND:
		/* 
		 *  FIXME: initiate disconnect.  This consists of
		 *  deactivating the SS7 stream and seeing if the AS
		 *  is now deactivated.  If the AS is inactive as a
		 *  result, send ASPIA for the AS.
		 */
	default:
		goto mtp_discon_req_outstate;
	}
	freemsg(pdu);
	return (0);

      mtpu_discon_req_outstate:return mtp_error_reply(q, pdu, N_DISCON_REQ, NOUTSTATE);
      mtpu_discon_req_baddata:return mtp_error_reply(q, pdu, N_DISCON_REQ, NBADDATA);
      mtpu_discon_req_emsgsize:return mtp_error_reply(q, pdu, N_DISCON_REQ, -EMSGSIZE);
}

/*
 *  N_DATA_REQ	     3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------------
 *  This request is only valid for ISUP/TUP MTP-Users.  If the interface is in
 *  the NS_DATA_XFER state, we select an SGP and send DATA messages, otherwise
 *  we return an error or N_DISCON_IND.
 */
static int mtpu_data_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *mp;
	uint more;
	size_t dlen = msgdsize(pdu);
	size_t mlen = pdu->b_wptr - pdu->b_rptr;
	mtp_t *mtp = (mtp_t *) q->q_ptr;
	N_data_req_t *p = (N_data_req_t *) pdu->b_rptr;
	if (mlen < sizeof(*p))
		goto mtpu_data_req_eproto;
	if (!dlen || dlen > mtp->nidu)
		goto mtpu_data_req_eproto;
	if (mtp->state == NS_IDLE || sccp->state == NS_WRES_RIND)
		goto mtpu_data_req_ignore;
	if (mtp->state != NS_DATA_XFER)
		goto mtpu_data_req_eproto;
	if (!pdu->b_cont)
		goto mtpu_data_req_eproto;
	if (p->DATA_xfer_flags & N_RC_FLAG)
		goto mtpu_data_req_eproto;
	if (p->DATA_xfer_flags & N_MORE_DATA_FLAG) {
		if (!mtp->assemble)
			mtp->assemble = pdu->b_cont;
		else
			linkb(mtp->assemble, pdu->b_cont);
		freeb(pdu);
		return (0);
	}
	if (mtp->assemble) {
		linkb(mtp->assemble, pdu->b_cont);
		pdu->b_cont = mtp->assemble;
		mtp->assemble = NULL;
		if (msgdsize(pdu) > mtp->nidu)
			goto mtpu_data_req_eproto;
	}
	/* 
	 *  FIXME: select an SGP and formulate a DATA message and send
	 *  it to the SGP on a selected stream.
	 */
	putq(q, mp);
	freeb(pdu);
	return (0);

      mtpu_data_req_ignore:freemsg(pdu);
	return (0);
      mtpu_data_req_eproto:return m_error_reply(q, pdu, EPROTO, EPROTO);
}

/*
 *  N_INFO_REQ	     5 - Information Request
 *  -------------------------------------------------------------------------
 *  Valid for both CNLS and CONS.  We always return current information
 *  associated with the interface in an N_INFO_ACK.
 */
static int mtpu_info_req(queue_t * q, mblk_t * mp)
{
}

/*
 *  N_BIND_REQ
 *  -------------------------------------------------------------------------
 *  Depending on CNLS or CONS.  CNLS we check for Routing Keys associated with
 *  ASPs which match the requested bind.  If no AS is found with the requested
 *  bind, we RK or perform a REG REQ to associated SGP.  If an AS is found, we
 *  or REG REQ is successful, we perform ASPAC procedures to associated SGP.
 *  For CONS, we bind the local address and return N_BIND_ACK.
 */
static int mtpu_bind_req(queue_t * q, mblk_t * mp)
{
	mblk_t *rp;
	mtp_addr_t *a;
	mtp_t *mtp = (mtp_t *) q->q_ptr;
	N_bind_req_t *p = (N_bind_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		goto mtpu_bind_req_emsgsize;
	if (pdu->b_rptr + p->ADDR_offset > pdu->b_wptr - p->ADDR_length)
		goto mtpu_bind_req_badaddr;
	if (pdu->b_rptr + p->PROTOID_offset > pdu->b_wptr - p->PROTOID_length)
		goto mtpu_bind_req_badprot;
	if (p->ADDR_length == 0)
		goto mtpu_bind_req_noaddr;
	if (p->ADDR_length < sizeof(*a))
		goto mtpu_bind_req_badaddr;
	if (mtp->state != NS_UNBND)
		goto mtpu_conn_req_outstate;
	a = (mtp_addr_t *) (pdu->b_rptr + p->ADDR_offset);
	bcopy(a, &mtp->src, sizeof(*a));
	/* 
	 *  FIXME:  There is a couple of things we could do here:
	 *
	 *  1)  We could formulate a RK and then check the list of configured
	 *      RKs to see if one is associated with a current AS.
	 *
	 *  2)  If we find an RK match but it is not associated with an AS,
	 *      yet it is associated with SGPs for dynamic registration, we
	 *      can create an AS and attempt to dynamically register the RK
	 *      with a REG REQ for those SGP supporting it.
	 *
	 *  3)  If we cannot find an RK match, we can ask the LM queue to map
	 *      the RK for us.
	 *
	 *  4)  If we cannot find an RK match, we can formulate a REG REQ and
	 *      send it to all available SGPs.
	 */
	mtp->state = NS_IDLE;
	freemsg(pdu);
	return (0);

      mtpu_bind_req_emsgsize:return mtp_error_reply(q, pdu, N_BIND_REQ, -EMSGSIZE);
      mtpu_bind_req_badaddr:return mtp_error_reply(q, pdu, N_BIND_REQ, NBADADDR);
      mtpu_bind_req_badprot:return mtp_error_reply(q, pdu, N_BIND_REQ, NBADPROTO);
      mtpu_bind_req_noaddr:return mtp_error_reply(q, pdu, N_BIND_REQ, NNOADDR);
      mtpu_conn_req_outstate:return mtp_error_reply(q, pdu, N_BIND_REQ, NOUTSTATE);

}

/*
 *  N_UNBIND_REQ
 *  -------------------------------------------------------------------------
 *  We always just change state and return a N_OK_ACK.  For CNLS, unbinding
 *  the stream also needs to deactivate it.  If the associated AS becomes
 *  inactive due to this, the ASPIA procedures may need to be performed.
 */
static int mtpu_unbind_req(queue_t * q, mblk_t * pdu)
{
	mblk_t *rp;
	mtp_t *mtp = (mtp_t *) q->q_ptr;
	N_unbind_req_t *p = (N_unbind_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		goto mtpu_unbind_req_emsgsize;
	if (mtp->state != NS_IDLE)
		goto mtpu_unbind_req_outstate;
	if (!(rp = mtp_ok_ack(N_UNBIND_REQ)))
		return (-ENOBUFS);
	mtp->state = NS_WACK_UREQ;
	qreply(q, mp);
	mtp->state = NS_UNBND;
	freemsg(pdu);
	return (0);

      mtpu_unbind_req_emsgsize:return mtp_error_reply(q, pdu, N_UNBIND_REQ, -EMSGSIZE);
      mtpu_unbind_req_outstate:return mtp_error_reply(q, pdu, N_UNBIND_REQ, NOUTSTATE);
}

/*
 *  N_UNITDATA_REQ
 *  -------------------------------------------------------------------------
 *  This request is only valid for non-ISUP/TUP MTP-Users.  If we are in
 *  NS_IDLE state we formulate a DATA message and send it on to a selected
 *  SGP (or return error).
 */
static int mtpu_unitdata_req(queue_t * q, mblk_t * mp)
{
	mtp_addr_t *src, *dst;
	N_qos_sel_mtp_t *qos;
	mtp_t *mtp = (mtp_t *) q->q_ptr;
	size_t dlen = msgdsize(pdu);
	size_t mlen = pdu->b_wptr - pdu->b_rptr;
	N_unitdata_req_t *p = (N_unitdata_req_t *) pdu->b_rptr;
	if (mlen < sizeof(*p)
	    || mlen < p->DEST_offset + p->DEST_length
	    || mlen < p->SRC_offset + p->SRC_length || mlen < p->QOS_offset + p->QOS_length)
		goto mtpu_unitdata_req_emsgsize;
	if (mtp->state != NS_IDLE || dlen > mtp->nidu)
		goto mtpu_unitdata_req_eproto;
	if (!dlen)
		goto mtpu_unitdata_req_baddata;
	if (!p->QOS_length || p->QOS_length < sizeof(*qos))
		goto mtpu_unitdata_req_badqos;
	else {
		qos = (N_qos_sel_mtp_t *) (pdu->b_rptr + p->QOS_offset);
		if (qos->n_qos_type != N_QOS_SEL_MTP)
			goto mtpu_unitdata_req_badqos;
		/* fill in missing QOS parameters for user */
		if (qos->sls == -1)
			mtp->sls = mtp_next_sls_value++;
		if (qos->mp == -1)
			mtp->mp = 0;
	}
	if (!p->SRC_length) {
		/* implicit source address */
		src = &mtp->src;
	} else {
		/* check provided source address */
		src = (mtp_addr_t *) (pdu->b_rptr + p->SRC_offset);
		if (p->SRC_length < sizeof(*src))
			goto mtpu_unitdata_req_badaddr;
		/* fill in PC for user */
		if (!src->ni || src->ni == -1)
			src->ni = mtp->src.ni;
		if (!src->pc || src->pc == -1)
			src->pc = mtp->src.pc;
		if (!src->si || src->si == -1)
			src->si = mtp->src.si;
	}
	if (!p->DEST_length) {
		/* destination address is mandatory */
		goto mtp_unitdata_req_badaddr;
	} else {
		/* check provided destination address */
		dst = (mtp_addr_t *) (pdu->b_rptr + p->DEST_offset);
		if (p->DEST_length < sizeof(*dst))
			goto mtpu_unitdata_req_badaddr;
		if (!dst->ni || dst->ni == -1)
			dst->ni = mtp->src.ni;
		if (!dst->si || dst->si == -1)
			dst->si = mtp->src.si;
		if (!dst->ni || dst->ni == -1)
			goto mtpu_unitdata_req_badaddr;
	}
	/* 
	 *  FIXME:  Package up the data, select an SGP, stream and transmit
	 *          message.
	 */
	freeb(pdu);
	return (0);

      mtp_unitdata_req_badaddr:return mtp_uderror_reply(q, pdu, NBADADDR);
      mtp_unitdata_req_badqos:return mtp_uderror_reply(q, pdu, NBADOPT);
      mtp_unitdata_req_baddata:return mtp_uderror_reply(q, pdu, NBADDATA);
      mtp_unitdata_req_emsgsize:return mtp_error_reply(q, pdu, N_UNITDATA_REQ, -EMSGSIZE);
      mtp_unitdata_req_eproto:return m_error_reply(q, pdu, EPROTO, EPROTO);
}

/*
 *  N_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 *  This depends on a number of factors.  Some requests are local, some
 *  require interaction with the SGP.
 */
static int mtpu_optmgmt_req(queue_t * q, mblk_t * mp)
{
}

/*
 *  N_RESET_RES
 *  -------------------------------------------------------------------------
 *  This request is only valid for ISUP/TUP MTP-Users.  This indicates that
 *  the User has accepted a reset indication.  We change state back to
 *  NS_DATA_XFER.
 */
static int mtpu_reset_res(queue_t * q, mblk_t * mp)
{
	int err;
	mblk_t *rp;
	size_t mlen = pdu->b_wptr - pdu->b_rptr;
	mtp_t *mtp = (mtp_t *) q->q_ptr;
	N_reset_res_t *p = (N_reset_res_t *) pdu->b_rptr;
	if (mlen < sizeof(*p))
		goto mtpu_reset_res_emsgsize;
	if (mtp->state == NS_IDLE)
		goto mtpu_reset_req_ignore;
	if (mtp->state != NS_WRES_RIND)
		goto mtpu_reset_req_outstate;
	if (!(rp = mtp_ok_ack(N_RESET_RES)))
		return (-ENOBUFS);
	/* 
	 *  TODO: more...
	 */
	mtp->state = NS_WACK_RRES;
	qreply(q, rp);
	mtp->state = NS_DATA_XFER;	/* on success */
      mtpu_reset_res_ignore:
	freemsg(pdu);
	return (0);

      mtpu_reset_res_emsgsize:return mtp_reply_error(q, pdu, N_RESET_RES, -EMSGSIZE);
      mtpu_reset_res_outstate:return mtp_reply_error(q, pdu, N_RESET_RES, NOUTSTATE);
}

static int (*mtpu_dstr_prim[]) (queue_t *, mblk_t *) = {
#define MTP_DSTR_FIRST		   N_CONN_REQ
	mtpu_conn_req,		/* N_CONN_REQ 0 */
	    NULL,		/* N_CONN_RES 1 */
	    mtpu_discon_req,	/* N_DISCON_REQ 2 */
	    mtpu_data_req,	/* N_DATA_REQ 3 */
	    NULL,		/* N_EXDATA_REQ 4 */
	    mtpu_info_req,	/* N_INFO_REQ 5 */
	    mtpu_bind_req,	/* N_BIND_REQ 6 */
	    mtpu_unbind_req,	/* N_UNBIND_REQ 7 */
	    mtpu_unitdata_req,	/* N_UNITDATA_REQ 8 */
	    mtpu_optmgmt_req,	/* N_OPTMGMT_REQ 9 */
	    NULL,		/* 10 */
	    NULL,		/* N_CONN_IND 11 */
	    NULL,		/* N_CONN_CON 12 */
	    NULL,		/* N_DISCON_IND 13 */
	    NULL,		/* N_DATA_IND 14 */
	    NULL,		/* N_EXDATA_IND 15 */
	    NULL,		/* N_INFO_ACK 16 */
	    NULL,		/* N_BIND_ACK 17 */
	    NULL,		/* N_ERROR_ACK 18 */
	    NULL,		/* N_OK_ACK 19 */
	    NULL,		/* N_UNITDATA_IND 20 */
	    NULL,		/* N_UDERROR_IND 21 */
	    NULL,		/* 22 */
	    NULL,		/* N_DATACK_REQ 23 */
	    NULL,		/* N_DATACK_IND 24 */
	    NULL,		/* N_RESET_REQ 25 */
	    NULL,		/* N_RESET_IND 26 */
	    mtpu_reset_res,	/* N_RESET_RES 27 */
#define MTP_DSTR_LAST		   N_RESET_RES
	    NULL,		/* N_RESET_CON 28 */
};

/*
 *  M_DATA Processing
 *  -------------------------------------------------------------------------
 */
static int mtpu_w_data(queue_t * q, mblk_t * mp)
{
	int err;
	mblk_t *np;
	if (!(np = mtpu_data(q, mp)))
		return (-ENOBUFS);
//      if ( (err = m3ua_as_write(q, np)) )
//              return(err);
	return (0);
}

/*
 *  M_PROTO, M_PCPROTO Processing
 *  -------------------------------------------------------------------------
 */
static int mtpu_w_proto(queue_t * q, mblk_t * mp)
{
	int prim = *((long *) mp->b_wptr);
	if (MTP_DSTR_FIRST <= prim && prim <= MTP_DSTR_LAST && mtpu_dstr_prim[prim])
		return ((*mtpu_dstr_prim[prim]) (q, mp));
	return (-EOPNOTSUPP);
}

/*
 *  M_ERROR, M_HANGUP Processing
 *  -------------------------------------------------------------------------
 */

/*
 *  M_FLUSH Processing
 *  -------------------------------------------------------------------------
 */

static int (*mtpu_w_ops[]) (queue_t *, mblk_t *) = {
	mtpu_w_data,		/* M_DATA */
	    mtpu_w_proto,	/* M_PROTO */
	    NULL,		/* M_BREAK */
	    NULL,		/* M_CTL */
	    NULL,		/* M_DELAY */
	    NULL,		/* M_IOCTL */
	    NULL,		/* M_PASSFP */
	    NULL,		/* M_RSE */
	    NULL,		/* M_SETOPTS */
	    NULL,		/* M_SIG */
	    NULL,		/* M_COPYIN */
	    NULL,		/* M_COPYOUT */
	    NULL,		/* M_ERROR */
	    mtpu_w_flush,	/* M_FLUSH */
	    NULL,		/* M_HANGUP */
	    NULL,		/* M_IOCACK */
	    NULL,		/* M_IOCNAK */
	    NULL,		/* M_IOCDATA */
	    mtpu_w_proto,	/* M_PCPROTO */
	    NULL,		/* M_PCRSE */
	    NULL,		/* M_PCSIG */
	    NULL,		/* M_READ */
	    NULL,		/* M_STOP */
	    NULL,		/* M_START */
	    NULL,		/* M_STARTI */
	    NULL		/* M_STOPI */
};

struct ops mtpu_ops = {
	NULL,				/* read operations */
	&mtpu_w_ops			/* write operations */
};
