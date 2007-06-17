/*****************************************************************************

 @(#) $RCSfile: m3ua_mtpp.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/06/17 01:56:18 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

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

 Last Modified $Date: 2007/06/17 01:56:18 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: m3ua_mtpp.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/06/17 01:56:18 $"

static char const ident[] =
    "$RCSfile: m3ua_mtpp.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/06/17 01:56:18 $";

#define __NO_VERSION__

#include <sys/os7/compat.h>

#include "m3ua.h"
#include "m3ua_data.h"
#include "m3ua_ctrl.h"
#include "m3ua_msg.h"

/*
 *  =========================================================================
 *
 *  (SG) MTPP --> M2UA (Upstream Primitives)
 *
 *  =========================================================================
 *  These primitives are sent from the lower mux MTP Provider in SG operation
 *  to a Signalling Gateway Process (SGP).
 */

/*
 *  MTP_CONN_IND     11 - Incoming connection indication
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_CONN_CON     12 - Connection established
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_DISCON_IND   13 - NC disconnected
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_DATA_IND     14 - Incoming connection-mode data indication
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_EXDATA_IND   15 - Incoming expedited data indication
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_INFO_ACK     16 - Information Acknowledgement
 *  -------------------------------------------------------------------------
 */
static int mtpp_info_ack(queue_t * q, mblk_t * mp)
{
	/* 
	 *  FIXME: might want to look at this one to get some pertinent
	 *  information and then pass it on.  We might have generated the
	 *  infor request ourselves.
	 */
	mtpp_t *mtp = Q_MTP(q);
	N_info_ack_t *p = (N_info_ack_t *) mp->b_rptr;
	if (mp->b_wptr - mp->b_rptr < sizeof(*p))
		return -EPROTO;
	mtp->max_sdu = p->NSDU_size;	/* maximum NSDU size */
	mtp->header_len = p->ADDR_size;	/* address size */
	/* 
	 *  FIXME: need to dig out bound ADDR. (Do I?)
	 *  FIXME: need to dig out QOS values, these would be size of the SLS.
	 */
	mtp->options = p->OPTIONS_flags;
	mtp->service = p->SERV_type;
	mtp->state = p->CURRENT_state;
	mtp->provider = p->PROVIDER_type;
	mtp->opt_sdu = p->NODU_size;
	mtp->version = p->NPI_version;
	return (lm_link_ind(q, mp));
}

/*
 *  MTP_BIND_ACK     17 - NS User bound to network address
 *  -------------------------------------------------------------------------
 */
static int mtpp_bind_ack(queue_t * q, mblk_t * mp)
{
	/* 
	 *  FIXME: might want to look at this one to echo the state of the
	 *  interface and then pass it on.
	 */
	return (lm_link_ind(q, mp));
}

/*
 *  MTP_ERROR_ACK    18 - Error Acknowledgement
 *  -------------------------------------------------------------------------
 */
static int mtpp_error_ack(queue_t * q, mblk_t * mp)
{
	/* 
	 *  FIXME: might want to look at this one to echo the state of the
	 *  interface and then pass it on.
	 */
	return (lm_link_ind(q, mp));
}

/*
 *  MTP_OK_ACK       19 - Success Acknowledgement
 *  -------------------------------------------------------------------------
 */
static int mtpp_ok_ack(queue_t * q, mblk_t * mp)
{
	/* 
	 *  FIXME: might want to look at this one to echo the state of the
	 *  interface and then pass it on.
	 */
	return (lm_link_ind(q, mp));
}

/*
 *  MTP_UNITDATA_IND 20 - Connection-less data receive indication
 *  -------------------------------------------------------------------------
 *  N_UNITDATA_IND (MTP_TRANSFER_IND)
 *  -------------------------------------------------------------------------
 *  This covers only the MTP-TRANSFER-Indication primitive.
 *
 *  Let me ask a question here: why dont we just pass the unitdata on to the
 *  ASP and let the ASP translate it into an M3UA message.  That way, if there
 *  are different ASPs supporting different versions, that can be handled at
 *  the ASP instead of here.  Also, we don't know the transport type.  If it
 *  is an SCTP transport, it can do other things with the message, like select
 *  stream.
 */
static int mtpp_unitdata_ind(queue_t * q, mblk_t * msg)
{
	sls_t *sls;
	queue_t *wq;
	mtpp_t *mtp = Q_MTP(q);
	mblk_t *mp, *db = msg->b_cont;
	size_t dlen = msgdsize(db);
	N_unitdata_ind_t *p = (N_unitdata_req_t *) msg->b_rptr;
	struct mtp_rl *rl = (mtp_rt *) (((caddr_t) p) + p->SRC_offset);
	static const size_t mlen =
	    M3UA_MHDR_SIZE + M3UA_PARM_SIZE_RC + M3UA_PARM_SIZE_RL + M3UA_PHDR_SIZE;
	/* 
	 *  First let's find out where the data is going.  The AS should have
	 *  this all set up for us in the SLS tables.
	 */
	ensure(mtp, return (-EFAULT));
	ensure(mtp->rc, return (-EFAULT));
	ensure(mtp->rc->as, return (-EFAULT));

	sls = &mtp->rc->as->sls[(rl->sls & UA_SLS_MASK)];

	ensure(sls->sp, return (-EFAULT));
	ensure(sls->sp->lp, return (-EFAULT));
	ensure(sls->sp->lp->q, return (-EFAULT));

	if (!(sls->flags & UA_SLS_BUFFERING))
		if (!(canput((wq = WR(sls->sp->lp->q)))))
			return (-EBUSY);	/* apply backpressure! */

	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DATA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen + dlen);
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
		/* 
		 *  A couple of big arguments on what should be in the
		 *  messages here...
		 */
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RL;
		*((uint32_t *) mp->b_wptr)++ = hotnl(rl->opc);
		*((uint32_t *) mp->b_wptr)++ = hotnl(rl->dpc);
		*((uint8_t *) mp->b_wptr)++ = 0;
		*((uint8_t *) mp->b_wptr)++ = hotnl(rl->sls);
		*((uint8_t *) mp->b_wptr)++ = hotnl(rl->ni);
		*((uint8_t *) mp->b_wptr)++ = hotnl(rl->mp);
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_DATA;
		mp->b_cont = db;
		freeb(msg);

		if (sls->flags & UA_SLS_BUFFERING)
			/* hold back data for this sls */
			bufq_queue(&sls->buf, mp);
		else
			putq(wq, mp);
		return (0);
	}
	return (-ENOBUFS);	/* try again later */
}

/*
 *  MTP_UDERROR_IND  21 - UNITDATA Error Indication
 *  -------------------------------------------------------------------------
 *  N_UDERROR_IND (MTP_PAUSE_IND, MTP_RESUME_IND, MTP_STATUS_IND, MTP_RESTART_IND)
 *  -------------------------------------------------------------------------
 *  This covers the MTP-STATUS-Indication, MTP-PAUSE-Indication,
 *  MTP-RESUME-Indication, MTP-RESTART-BEGINS and MTP-RESTART-ENDS.
 */
static mblk_t *mtpp_uderror_ind(queue_t * q, mblk_t * msg)
{
	mblk_t *mp;
	mtpp_t *mtp = Q_MTP(q);
	static const size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		N_uderror_ind_t *p = (N_uderror_ind_t *) msg->b_rptr;
		struct mtp_rl *rl = (mtp_rl *) (((caddr_t) p) + p->DEST_offset - sizeof(uint32_t));
		mp->b_datap->db_type = M_DATA;
		switch (p->ERROR_type) {
		case MTP_DEST_CONGESTED:
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_SCON;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(rl->dpc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_CONG_LEVEL;
			*((uint32_t *) mp->b_wptr)++ = htonl(rl->mp);
			break;
		case MTP_DEST_PROHIBITED:
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DUNA;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(rl->dpc);
			break;
		case MTP_DEST_RESTRICTED:
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DRST;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(rl->dpc);
			break;
		case MTP_CLUSTER_PROHIBITED:
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DUNA;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(0x08000000 | rl->dpc);
			break;
		case MTP_CLUSTER_RESTRICTED:
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DRST;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(0x08000000 | rl->dpc);
			break;
		case MTP_RESTARTING:
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DUNA;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(0xff000000 | rl->dpc);
			break;
		case MTP_USER_PART_UNKNOWN:
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DUPU;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(rl->dpc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_USER_CAUSE;
			*((uint16_t *) mp->b_wptr)++ = M3UA_USER_UNKNOWN;
			*((uint16_t *) mp->b_wptr)++ = htons(rl->si);
			break;
		case MTP_USER_PART_UNEQUIPPED:
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DUPU;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(rl->dpc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_USER_CAUSE;
			*((uint16_t *) mp->b_wptr)++ = M3UA_USER_UNEQUIPPED;
			*((uint16_t *) mp->b_wptr)++ = htons(rl->si);
			break;
		case MTP_USER_PART_UNAVAILABLE:
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DUPU;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(rl->dpc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_USER_CAUSE;
			*((uint16_t *) mp->b_wptr)++ = M3UA_USER_INACCESSIBLE;
			*((uint16_t *) mp->b_wptr)++ = htons(rl->si);
			break;
		default:
			/* 
			 *  FIXME:  The default will be a zero-sized
			 *  M_DATA message if we don't fix this.
			 *  Someone downstream is going to complian!
			 */
		}
	}
	return (mp);
}

/*
 *  MTP_DATACK_IND   24 - Data acknowledgement indication
 *  -------------------------------------------------------------------------
 *  We receive one of these from MTP when we have asked for receipt
 *  confirmation, in which case, the MTP sends us one of these each time that
 *  a message has been transferred from the MTP-User write queue to a
 *  SL-Provider write queue (i.e., committed for delivery).  Before this
 *  indication is received, the corresponding message in the MTP-User write
 *  queue may yet be discarded due to an inaccessible or congested point code,
 *  or an unavailable user part.
 *
 *  When receipt confirmation is used, the MTP-User should hang onto a copy of
 *  its transmitted messages until this.
 */
static int mtpp_datack_ind(queue_t * q, mblk_t * msg)
{
}

/*
 *  MTP_RESET_IND    26 - Incoming NC reset request indication
 *  -------------------------------------------------------------------------
 *  (MTP-PAUSE, MTP-STATUS, MTP-RESTART indications)
 *  -------------------------------------------------------------------------
 *  The NSD reset facility is used to indicate to point code-to-point code
 *  connection-oriented MTP-Users the unavailability, restriction, congestion
 *  or user-part unavailability.
 *
 *  This reset is rather uni-directional in that messages are not flushed from
 *  the read queue, they will just be flushed from the write queue.  If
 *  receipt confirmation is on, the MTP-User at the other end will know which
 *  messages have not been received.
 */
static int mtpp_reset_ind(queue_t * q, mblk_t * msg)
{
	mblk_t *mp;
	mtpp_t *mtp = Q_MTP(q);
	size_t mlen = FIXME;
	N_reset_ind_t *p = (N_reset_ind_t *) msg->b_rptr;
	if (msg->b_wptr - msg->b_rptr < sizeof(*p))
		return (-EFAULT);
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		mp->b_band = 2;	/* double expedite to ASP */
		switch (p->RESET_reason) {
		case MTP_DEST_CONGESTED:
		case MTP_DEST_CONGESTED_LEVEL_1:
		case MTP_DEST_CONGESTED_LEVEL_2:
		case MTP_DEST_CONGESTED_LEVEL_3:
			mlen = sizeof(uint32_t) * 10;
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_SCON;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->dpc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_CONG_LEVEL;
			switch (p->RESET_reason) {
			case MTP_DEST_CONGESTED_LEVEL_1:
				*((uint32_t *) mp->b_wptr)++ = htonl(1);
				break;
			case MTP_DEST_CONGESTED_LEVEL_2:
				*((uint32_t *) mp->b_wptr)++ = htonl(2);
				break;
			case MTP_DEST_CONGESTED:
			case MTP_DEST_CONGESTED_LEVEL_3:
				*((uint32_t *) mp->b_wptr)++ = htonl(3);
				break;
			}
			break;
		case MTP_DEST_PROHIBITED:
			mlen = sizeof(uint32_t) * 8;
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DUNA;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->dpc);
			break;
		case MTP_DEST_RESTRICTED:
			mlen = sizeof(uint32_t) * 8;
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DRST;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->dpc);
			break;
		case MTP_RESTARTING:
			mlen = sizeof(uint32_t) * 8;
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DUNA;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(0xff000000 | mtp->dpc);
			break;
		case MTP_USER_PART_UNKNOWN:
		case MTP_USER_PART_UNEQUIPPED:
		case MTP_USER_PART_UNAVAILABLE:
			mlen = sizeof(uint32_t) * 11;
			*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DUPU;
			*((uint32_t *) mp->b_wptr)++ = __constant_htohl(mlen);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_APC;
			*((uint32_t *) mp->b_wptr)++ = htonl(mtp->dpc);
			*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_USER_CAUSE;
			switch (p->RESET_reason) {
			case MTP_USER_PART_UNKNOWN:
				*((uint16_t *) mp->b_wptr)++ = M3UA_USER_UNKNOWN;
				break;
			case MTP_USER_PART_UNEQUIPPED:
				*((uint16_t *) mp->b_wptr)++ = M3UA_USER_UNEQUIPPED;
				break;
			case MTP_USER_PART_UNAVAILABLE:
				*((uint16_t *) mp->b_wptr)++ = M3UA_USER_UNAVAILABLE;
				break;
			}
			*((uint16_t *) mp->b_wptr)++ = htons(mtp->si);
			break;
		default:
			freemsg(mp);
			return (-EFAULT);
		}
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  MTP-Provider/MTP-User --> M3UA Primitive Translator
 *
 *  -------------------------------------------------------------------------
 *  Many of the messages of M3UA map directly onto MTP primitives.  These
 *  message encoder functions take an MTP primitive and return an M3UA message
 *  which is the direct equivalent of the primitives.  These functions are
 *  called by functions which service MTP primitives.
 */
/*
 *  N_UNITDATA_REQ (MTP_TRANSFER_REQ)
 *  -------------------------------------------------------------------------
 *  This covers only the MTP-TRANSFER-Request primitive.
 */
static mblk_t *mtpp_unitdata_req(queue_t * q, mblk_t * msg)
{
	mblk_t *mp, *db = msg->b_cont;
	mtpp_t *mtp = Q_MTP(q);
	static const size_t mlen =
	    M3UA_MHDR_SIZE + M3UA_PARM_SIZE_RC + M3UA_PARM_SIZE_RL + M3UA_PHDR_SIZE;
	if ((mp = allocb(mlen, BPRI_MED))) {
		size_t dlen = msgdsize(db);
		N_unitdata_req_t *p = (N_unitdata_req_t *) msg->b_rptr;
		struct mtp_rl *rl = (mtp_rl *) (((caddr_t) p) + p->DEST_offset - sizeof(uint32_t));
		mp->b_datap->db_type = M_DATA;
		*((uint32_t *) mp->b_wptr)++ = M3UA_MAUP_DATA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mlen + dlen);
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_NA;
		*((uint32_t *) mp->b_wptr)++ = htonl(mtp->na);
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RC;
		*((uint32_t *) mp->b_wptr)++ = htonl(mtp->rc);
		/* 
		 *  A couple of big arguments on what should be in the
		 *  messages here...
		 */
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_RL;
		*((uint32_t *) mp->b_wptr)++ = hotnl(rl->opc);
		*((uint32_t *) mp->b_wptr)++ = hotnl(rl->dpc);
		*((uint8_t *) mp->b_wptr)++ = 0;
		*((uint8_t *) mp->b_wptr)++ = hotnl(rl->sls);
		*((uint8_t *) mp->b_wptr)++ = hotnl(rl->ni);
		*((uint8_t *) mp->b_wptr)++ = hotnl(rl->mp);
		*((uint32_t *) mp->b_wptr)++ = M3UA_PARM_DATA;
		mp->b_cont = db;
		freeb(msg);
	}
	return (mp);
}

static int (*mtpp_dstr_prim[]) (queue_t *, mblk_t *) {
#define MTP_DSTR_FIRST	           N_CONN_REQ
	mtpp_conn_req,		/* N_CONN_REQ 0 */
	    NULL,		/* N_CONN_RES 1 */
	    mtpp_discon_req,	/* N_DISCON_REQ 2 */
	    mtpp_data_req,	/* N_DATA_REQ 3 */
	    mtpp_exdata_req,	/* N_EXDATA_REQ 4 */
	    mtpp_info_req,	/* N_INFO_REQ 5 */
	    mtpp_bind_req,	/* N_BIND_REQ 6 */
	    mtpp_unbind_req,	/* N_UNBIND_REQ 7 */
	    mtpp_unitdata_req,	/* N_UNITDATA_REQ 8 */
	    mtpp_optmgmt_req,	/* N_OPTMGMT_REQ 9 */
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
	    mtpp_datack_req,	/* N_DATACK_REQ 23 */
	    NULL,		/* N_DATACK_IND 24 */
	    NULL,		/* N_RESET_REQ 25 */
	    NULL,		/* N_RESET_IND 26 */
	    mtpp_reset_res,	/* N_RESET_RES 27 */
#define MTP_DSTR_LAST		   N_RESET_RES
	    NULL,		/* N_RESET_CON 28 */
};
static int (*mtpp_ustr_prim[]) (queue_t *, mblk_t *) {
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
	    NULL,		/* 10 */
	    NULL,		/* N_CONN_IND 11 */
#define MTP_USTR_FIRST		   N_CONN_CON
	    mtpp_conn_con,	/* N_CONN_CON 12 */
	    mtpp_discon_ind,	/* N_DISCON_IND 13 */
	    mtpp_data_ind,	/* N_DATA_IND 14 */
	    mtpp_exdata_ind,	/* N_EXDATA_IND 15 */
	    mtpp_info_ack,	/* N_INFO_ACK 16 */
	    mtpp_bind_ack,	/* N_BIND_ACK 17 */
	    mtpp_error_ack,	/* N_ERROR_ACK 18 */
	    mtpp_ok_ack,	/* N_OK_ACK 19 */
	    mtpp_unitdata_ind,	/* N_UNITDATA_IND 20 */
	    mtpp_uderror_ind,	/* N_UDERROR_IND 21 */
	    NULL,		/* 22 */
	    NULL,		/* N_DATACK_REQ 23 */
	    mtpp_datack_ind,	/* N_DATACK_IND 24 */
	    NULL,		/* N_RESET_REQ 25 */
	    mtpp_reset_ind,	/* N_RESET_IND 26 */
#define MTP_USTR_LAST		   N_RESET_IND
	    NULL,		/* N_RESET_RES 27 */
	    NULL		/* N_RESET_CON 28 */
};

/*
 *  M_DATA Processing
 *  -------------------------------------------------------------------------
 */
static int mtpp_w_data(queue_t * q, mblk_t * mp)
{
	int err;
	mblk_t *np;
	if (!(np = mtpp_data(q, mp)))
		return (-ENOBUFS);
	if ((err = m3ua_as_write(q, np)))
		return (err);
	return (0);
}
static int mtpp_r_data(queue_t * q, mblk_t * mp)
{
	int err;
	mblk_t *np;
	if (!(np = mtpp_data(q, mp)))
		return (-ENOBUFS);
	if ((err = m3ua_as_read(q, np)))
		return (err);
	return (0);
}

/*
 *  M_PROTO, M_PCPROTO Processing
 *  -------------------------------------------------------------------------
 */
static int mtpp_w_proto(queue_t * q, mblk_t * mp)
{
	int prim = *((long *) mp->b_wptr);
	if (MTP_DSTR_FIRST <= prim && prim <= MTP_DSTR_LAST && mtpp_dstr_prim[prim])
		return ((*mtpp_dstr_prim[prim]) (q, mp));
	return (-EOPNOTSUPP);
}
static int mtpp_r_proto(queue_t * q, mblk_t * mp)
{
	int prim = *((long *) mp->b_wptr);
	if (MTP_USTR_FIRST <= prim && prim <= MTP_USTR_LAST && mtpp_ustr_prim[prim])
		return ((*mtpp_ustr_prim[prim]) (q, mp));
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

static int (*mtpp_w_ops[]) (queue_t *, mblk_t *) = {
	mtpp_w_data,		/* M_DATA */
	    mtpp_w_proto,	/* M_PROTO */
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
	    mtpp_w_flush,	/* M_FLUSH */
	    NULL,		/* M_HANGUP */
	    NULL,		/* M_IOCACK */
	    NULL,		/* M_IOCNAK */
	    NULL,		/* M_IOCDATA */
	    mtpp_w_proto,	/* M_PCPROTO */
	    NULL,		/* M_PCRSE */
	    NULL,		/* M_PCSIG */
	    NULL,		/* M_READ */
	    NULL,		/* M_STOP */
	    NULL,		/* M_START */
	    NULL,		/* M_STARTI */
	    NULL		/* M_STOPI */
};

static int (*mtpp_r_ops[]) (queue_t *, mblk_t *) = {
	mtpp_r_data,		/* M_DATA */
	    mtpp_r_proto,	/* M_PROTO */
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
	    mtpp_r_error,	/* M_ERROR */
	    mtpp_r_flush,	/* M_FLUSH */
	    mtpp_r_hangup,	/* M_HANGUP */
	    NULL,		/* M_IOCACK */
	    NULL,		/* M_IOCNAK */
	    NULL,		/* M_IOCDATA */
	    mtpp_r_proto,	/* M_PCPROTO */
	    NULL,		/* M_PCRSE */
	    NULL,		/* M_PCSIG */
	    NULL,		/* M_READ */
	    NULL,		/* M_STOP */
	    NULL,		/* M_START */
	    NULL,		/* M_STARTI */
	    NULL		/* M_STOPI */
};

struct ops mtpp_ops = {
	&mtpp_r_ops,			/* read operations */
	&mtpp_w_ops			/* write operations */
};
