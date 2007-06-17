/*****************************************************************************

 @(#) $Id: sua_nsp.c,v 0.9.2.1 2006/10/17 11:55:43 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2006/10/17 11:55:43 $ by $Author: brian $

 $Log: sua_nsp.c,v $
 Revision 0.9.2.1  2006/10/17 11:55:43  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.1  2004/08/21 10:14:59  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:23:12  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.1  2002/10/18 02:33:28  brian
 Indentation changes.

 Revision 0.8  2002/04/02 08:21:09  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/07/29 10:52:19  brian
 Split up SUA files.

 *****************************************************************************/

static char const ident[] = "$Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:43 $";

/*
 *  =========================================================================
 *
 *  N-Provider --> SUA  (Upstream) Primitives
 *
 *  =========================================================================
 */
/*
 *  N_DISCON_IND
 *  -------------------------------------------------------------------------
 *  Inform management and force the asp down immediately; we don't have time
 *  to wait for management to decide to do this for us.
 */
static int n_discon_ind(queue_t * q, mblk_t * pdu)
{
	int err;
	/* give it to management */
	if ((err = m_link_ind(q, pdu)))
		return (err);
	if ((err = ua_sp_down(q)))
		return (err);
	return (0);
}

/*
 *  N_DATA_IND
 *  -------------------------------------------------------------------------
 *  This is translated into SUA messages and fed to the state machines.
 */
static int n_data_ind(queue_t * q, mblk_t * pdu)
{
	int err;
	mblk_t *mp;
	N_data_ind_t *p;
	p = (N_data_ind_t *) pdu->b_rptr;
	if (p->DATA_xfer_flags & N_MORE_DATA_FLAG) {
		/* aaargh! */
		/* give it to management */
		if ((err = m_link_ind(q, pdu)))
			return (err);
		if ((err = ua_sp_down(q)))
			return (err);
		qdisable(q);
		return (0);
	}
	if (p->DATA_xfer_flags & N_RC_FLAG) {
		/* need to send receipt confirmation */
		if (!(mp = n_datack_ind()))
			return (-ENOBUFS);
		qreply(q, mp);
	}
	/* only need the M_DATA block */
	mp = pdu->b_cont;
	mp->b_band = 0;
	freeb(pdu);
	if ((err = ua_recv_msg(q, mp)))
		return (err);
	return (0);
}

/*
 *  N_EXDATA_IND
 *  -------------------------------------------------------------------------
 *  This is translated into SUA messages and fed to the state machines.
 */
static int n_exdata_ind(queue_t * q, mblk_t * pdu)
{
	int err;
	mblk_t *mp;
	N_exdata_ind_t *p;
	p = (N_exdata_ind_t *) pdu->b_rptr;
	if (p->DATA_xfer_flags & N_MORE_DATA_FLAG) {
		int err;
		if ((err = m_link_ind(q, pdu)))
			return (err);
		if ((err = ua_sp_down(q)))
			return (err);
		qdisable(q);
		return (0);
	}
	mp = pdu->b_cont;
	mp->b_band = 1;
	freeb(pdu);
	if ((err = ua_recv_msg(q, mp)))
		return (err);
	return (0);
}

/*
 *  N_INFO_ACK
 *  -------------------------------------------------------------------------
 */
static int n_info_ack(queue_t * q, mblk_t * pdu)
{
	/* 
	 *  NOTE:- We might later want to copy out some of the pertinent
	 *  information for ourselves.
	 */
	/* give it to management */
	return m_link_ind(q, pdu);
}

/*
 *  N_UNITDATA_IND
 *  -------------------------------------------------------------------------
 *  I don't know why we would get these, but maybe this is a connection-less
 *  transport which is sequenced...
 */
static int n_unitdata_ind(queue_t * q, mblk_t * pdu)
{
	int err;
	mblk_t *mp;
	mp = pdu->b_cont;
	mp->b_band = 0;
	freeb(pdu);
	if ((err = ua_recv_msg(q, mp)))
		return (err);
	return (0);
}

/*
 *  N_UDERROR_IND
 *  -------------------------------------------------------------------------
 *  I don't know why we would get these but they are bad...
 */
static int n_uderror_ind(queue_t * q, mblk_t * pdu)
{
	int err;
	if ((err = m_link_ind(q, mp)))
		return (err);
	if ((err = ua_sp_down(q)))
		return (err);
	qdisable(q);
	return (0);
}

/*
 *  N_DATACK_IND
 *  -------------------------------------------------------------------------
 *  If we've got receipt confirmation enabled we need to strike a message from
 *  our fail-over buffer.  It is better if the underlying layer supports
 *  retrieval instead (that way we can distinguish between sent and unsent
 *  unacknolwedged messages to avoid duplication).
 *
 *  We don't suport this yet...
 */
static int n_datack_ind(queue_t * q, mblk_t * pdu)
{
	(void) q;
	(void) pdu;
	return (-EOPNOTSUPPORT);
}

/*
 *  N_RESET_IND
 *  -------------------------------------------------------------------------
 *  We have lost and resetalished communication.  This results from an SCTP
 *  Restart or from an SSCOP Reset.  The N-Provider should have flushed the
 *  read queue up to us.  We should have responded to that an flushed read
 *  queues to the SCCP-Users, however, we also need to place the state of the
 *  ASP into the down state so that everything is in a known state.
 */
static int n_reset_ind(queue_t * q, mblk_t * pdu)
{
	if ((err = m_link_ind(q, mp)))
		return (err);
	if ((err = ua_sp_down(q)))
		return (err);
	/* 
	 *  FIXME:  Perhaps we should be sending the N_RESET_CON back to the
	 *  peer to indicate that we have reset.  Also, we should flush the
	 *  write queues.
	 */
	return (0);
}

/*
 *  N_RESET_CON
 *  -------------------------------------------------------------------------
 *  Same as N_RESET_IND, but Layer Management must have provoked it.
 */
static int n_reset_con(queue_t * q, mblk_t * pdu)
{
	if ((err = m_link_ind(q, mp)))
		return (err);
	if ((err = ua_sp_down(q)))
		return (err);
	return (0);
}

static int (*n_prim[]) (queue_t * q, mblk_t * mp) = {
	NULL,			/* N_CONN_REQ */
	    NULL,		/* N_CONN_RES */
	    NULL,		/* N_DISCON_REQ */
	    NULL,		/* N_DATA_REQ */
	    NULL,		/* N_EXDATA_REQ */
	    NULL,		/* N_INFO_REQ */
	    NULL,		/* N_BIND_REQ */
	    NULL,		/* N_UNBIND_REQ */
	    NULL,		/* N_UNITDATA_REQ */
	    NULL,		/* N_OPTMGMT_REQ */
	    NULL,		/* (unused) */
	    m_link_ind,		/* N_CONN_IND */
	    m_link_ind,		/* N_CONN_CON */
	    n_discon_ind,	/* N_DISCON_IND */
	    n_data_ind,		/* N_DATA_IND */
	    n_exdata_ind,	/* N_EXDATA_IND */
	    n_info_ack,		/* N_INFO_ACK */
	    m_link_ind,		/* N_BIND_ACK */
	    m_link_ind,		/* N_ERROR_ACK */
	    m_link_ind,		/* N_OK_ACK */
	    n_unitdata_ind,	/* N_UNITDATA_IND */
	    n_uderror_ind,	/* N_UDERROR_IND */
	    NULL,		/* (unused) */
	    NULL,		/* N_DATACK_REQ */
	    n_datack_ind,	/* N_DATACK_IND */
	    NULL,		/* N_RESET_REQ */
	    n_reset_ind,	/* N_RESET_IND */
	    NULL,		/* N_RESET_RES */
	    n_reset_con		/* N_RESET_CON */
};

static __inline__ int n_l_r_data(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	int err;
	if ((err = ua_recv_msg(q, mp)))
		return (err);
	return (0);
}
static __inline__ int n_l_r_proto(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	int prim = *((n_scalar_t *) mp->b_rptr);
	if (0 <= prim && prim <= sizeof(n_prim) / sizeof(int (*)(void)) && n_prim[prim])
		return ((*n_prim[prim]) (q, mp));
	return (-EOPNOTSUPP);
}
static __inline__ int n_l_r_error(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	/* 
	 *  FIXME:  The transport stream is toast.  Do whatever we need to to
	 *  to tear it down and notify management.  We want to pass this
	 *  M_ERROR to each application server which is being serviced by this
	 *  transport stream.
	 */
	sp_t *sp = ((lp_t *) q->q_ptr)->u.sp;
	gp_t *gp;
	for (gp = sp->as_list; gp; gp = gp->as_next) {
		mblk_t *dp;
		if ((dp = allocb(1, BPRI_HI))) {
			dp->b_datap->db_type = M_HANGUP;
			ua_u_rput(as->link->q, dp);
		}
	}
}
static __inline__ int n_l_r_hangup(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	/* 
	 *  The network provider has hung up on us.  This stream would be
	 *  toast, but we can still use it.  We tell Layer Management what is
	 *  going on.  Layer Management can start with the stream again from
	 *  its reset state.
	 */
	int err;
	if ((err = m_link_ind(q, mp)))
		return (err);
	if ((err = ua_sp_down(q)))
		return (err);
	qdisable(q);
	return (0);
}
