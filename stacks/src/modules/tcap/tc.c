/*****************************************************************************

 @(#) $RCSfile: tc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/03/08 19:31:01 $

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

 Last Modified $Date: 2005/03/08 19:31:01 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/03/08 19:31:01 $"

static char const ident[] =
    "$RCSfile: tc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/03/08 19:31:01 $";

/*
 *  This is a TC (Transaction Capabilities) mulitplexing driver for SS7 TCAP.
 *  It is opened and SCCP streams are I_LINKed under the driver by the TC User
 *  or I_PLINKed under the driver by the TC control stream for use by TC
 *  Users.  TC Users are associated with lower SCCP streams via the TC bind
 *  commands.
 *
 *  This includes a management stream for module reporting and configuration
 *  management.
 */

#include "os7/compat.h"

#define TC_DESCRIP	"TC STREAMS MULTIPLEXING DRIVER."
#define TC_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define TC_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define TC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TC_LICENSE	"GPL"
#define TC_BANNER	TC_DESCRIP	"\n" \
			TC_COPYRIGHT	"\n" \
			TC_DEVICE	"\n" \
			TC_CONTACT	"\n"

#ifdef LINUX
MODULE_AUTHOR(TC_CONTACT);
MODULE_DESCRIPTION(TC_DESCRIP);
MODULE_SUPPORTED_DEVICE(TC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TC_LICENSE);
#endif
#endif				/* LINUX */

#ifdef TC_DEBUG
static int tc_debug = TC_DEBUG;
#else
static int tc_debug = 2;
#endif

#ifdef LFS
#define TC_DRV_ID		CONFIG_STREAMS_TC_MODID
#define TC_DRV_NAME		CONFIG_STREAMS_TC_NAME
#define TC_CMAJORS		CONFIG_STREAMS_TC_NMAJORS
#define TC_CMAJOR_0		CONFIG_STREAMS_TC_MAJOR
#endif

#ifndef TC_CMAJOR_0
#define TC_CMAJOR_0 210
#endif
#ifndef TC_NMINOR
#define TC_NMINOR 255
#endif

#ifndef TC_DRV_ID
#define TC_DRV_ID ('s'<<24|'s'<<16|'7'<<8|TC_IOC_MAGIC)

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

static struct module_info tc_minfo = {
	TC_DRV_ID,			/* Module ID number */
	TC_DRV_NAME,			/* Module name */
	1,				/* Min packet size accepted *//* XXX */
	254,				/* Max packet size accepted *//* XXX */
	8 * 512,			/* Hi water mark *//* XXX */
	1 * 512				/* Lo water mark *//* XXX */
};

static int tc_open(queue_t *, dev_t *, int, int, cred_t *);
static int tc_close(queue_t *, int, cred_t *);

static int tc_u_rsrv(queue_t *);
static int tc_u_wput(queue_t *, mblk_t *);
static int tc_u_wsrv(queue_t *);

static struct qinit tc_u_rinit = {
	NULL,				/* Read put (msg from below) */
	tc_u_rsrv,			/* Read queue service */
	tc_open,			/* Each open */
	tc_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&tc_minfo,			/* Information */
	NULL				/* Statistics */
};
static struct qinit tc_u_wrinit = {
	tc_u_wput,			/* Write put (msg from above) */
	tc_u_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&tc_info,			/* Information */
	NULL				/* Statistics */
};

static int tc_l_wsrv(queue_t *);
static int tc_l_rput(queue_t *, mblk_t *);
static int tc_l_rsrv(queue_t *);

static struct qinit tc_l_winit = {
	NULL,				/* Write put (msg from above) */
	tc_l_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&tc_info,			/* Information */
	NULL				/* Statistics */
};
static struct qinit tc_l_rinit = {
	tc_l_rput,			/* Read put (msg from above) */
	tc_l_rsrv,			/* Read queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&tc_info,			/* Information */
	NULL				/* Statistics */
};

MODULE_STATIC struct streamtab tc_info = {
	&tc_u_rinit,			/* Upper read queue */
	&tc_u_winit,			/* Upper write queue */
	&tc_l_rinit,			/* Lower read queue */
	&tc_l_winit			/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  TC Private Structures
 *
 *  =========================================================================
 */

struct tc;
struct sccp;

typedef struct tc {
	struct tc *next;		/* list of all TC-Users */
	struct tc **prev;		/* list of all TC-Users */
	queue_t *q;			/* associated read queue */
	dev_t devid;			/* device id at open */
	uint state;			/* interface state */
} sccp_t;

static uint tc_next_transaction_id = 1;

/*
 *  =========================================================================
 *
 *  TC Message Structures
 *
 *  =========================================================================
 */
// #include "tc_msg.h"

/*
 *  =========================================================================
 *
 *  Module --> Module (Control Messages)
 *
 *  =========================================================================
 */

static inline mblk_t *m_error(int r_error, int w_error)
{
	mblk_t *mp;
	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = r_error;
		*(mp->b_wptr)++ = w_error;
	}
	return (mp);
}
static inline mblk_t *m_hangup(void)
{
	mblk_t *mp;
	if ((mp = allogb(0, BPRI_HI))) {
		mp->b_datap->db_type = M_HANGUP;
	}
	return (mp);
}
static inline mblk_t *m_flush(int flags, int band)
{
	mblk_t *mp;
	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_FLUSH;
		*(mp->b_wptr)++ = flags;
		*(mp->b_wptr)++ = band;
	}
	return (mp);
}

/*
 *  =========================================================================
 *
 *  TC --> TC-User (Upstream Primitives sent upstream)
 *
 *  =========================================================================
 */

/*
 *  TC_INFO_ACK		 Information acknowledgement
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_info_ack(void)
{
	mblk_t *mp;
	TC_info_ack_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_info_ack_t *) mp->b_wptr;
		p->PRIM_type = TC_INFO_ACK;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_BIND_ACK		 Bound to global title
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_bind_ack(void)
{
	mblk_t *mp;
	TC_bind_ack_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_bind_ack_t *) mp->b_wptr;
		p->PRIM_type = TC_BIND_ACK;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_OK_ACK		 Success acknowledgement
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_ok_ack(void)
{
	mblk_t *mp;
	TC_ok_ack_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_ok_ack_t *) mp->b_wptr;
		p->PRIM_type = TC_OK_ACK;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_ERROR_ACK	 Error acknowledgement
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_error_ack(void)
{
	mblk_t *mp;
	TC_error_ack_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_error_ack_t *) mp->b_wptr;
		p->PRIM_type = TC_ERROR_ACK;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_UNIDIR_IND	 Unidirectional Indication
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_unidir_ind(void)
{
	mblk_t *mp;
	TC_unidir_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_unidir_ind_t *) mp->b_wptr;
		p->PRIM_type = TC_UNIDIR_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_QUERY_W_PERM_IND	 Incoming Query with permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_query_w_perm_ind(void)
{
	mblk_t *mp;
	TC_query_w_perm_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_query_w_perm_ind_t *) mp->b_wptr;
		p->PRIM_type = TC_QUERY_W_PERM_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_QUERY_WO_PERM_IND Incoming Query without permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_query_wo_perm_ind(void)
{
	mblk_t *mp;
	TC_query_wo_perm_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_query_wo_perm_ind_t *) mp->b_wptr;
		p->PRIM_type = TC_QUERY_WO_PERM_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_RESP_IND		 Incoming Response
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_resp_ind(void)
{
	mblk_t *mp;
	TC_resp_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_resp_ind_t *) mp->b_wptr;
		p->PRIM_type = TC_RESP_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_CONV_W_PERM_IND	 Incoming Conversation with permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_conv_w_perm_ind(void)
{
	mblk_t *mp;
	TC_conv_w_perm_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_conv_w_perm_ind_t *) mp->b_wptr;
		p->PRIM_type = TC_CONV_W_PERM_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_CONV_WO_PERM_IND	 Incoming Conversation without permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_conv_wo_perm_ind(void)
{
	mblk_t *mp;
	TC_conv_wo_perm_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_conv_wo_perm_ind_t *) mp->b_wptr;
		p->PRIM_type = TC_CONV_WO_PERM_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_CONV_W_PERM_CON	 Incoming Conversation with permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_conv_w_perm_con(void)
{
	mblk_t *mp;
	TC_conv_w_perm_con_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_conv_w_perm_con_t *) mp->b_wptr;
		p->PRIM_type = TC_CONV_W_PERM_CON;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_CONV_WO_PERM_CON	 Incoming Conversation without permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_conv_wo_perm_con(void)
{
	mblk_t *mp;
	TC_conv_wo_perm_con_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_conv_wo_perm_con_t *) mp->b_wptr;
		p->PRIM_type = TC_CONV_WO_PERM_CON;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TC_ABORT_IND	 Abort indication
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *tc_abort_ind(void)
{
	mblk_t *mp;
	TC_abort_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_abort_ind_t *) mp->b_wptr;
		p->PRIM_type = TC_ABORT_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  =========================================================================
 *
 *  TC --> SCCP (Downstream Primitives sent downstream)
 *
 *  =========================================================================
 */
#include "../npi/npi_user.h"

/*
 *  N_INFO_REQ	     5 - Information Request
 *  -------------------------------------------------------------------------
 */
static int sccp_info_req(void)
{
	return n_info_req();
}

/*
 *  N_BIND_REQ	     6 - Bind a NS user to network address
 *  -------------------------------------------------------------------------
 */
static int sccp_bind_req(uint flags, sccp_addr_t * bnd, uint32_t ssn)
{
	return n_bind_req(flags, 0, (caddr_t) bnd, bnd ? sizeof(*bind) : 0, (caddr_t) & ssn,
			  sizeof(ssn));
}

/*
 *  N_UNBIND_REQ     7 - Unbind NS user from network address
 *  -------------------------------------------------------------------------
 */
static int sccp_unbind_req(void)
{
	return n_unbind_req();
}

/*
 *  N_UNITDATA_REQ   8 - Connection-less data send request
 *  -------------------------------------------------------------------------
 */
static int sccp_unitdata_req(sccp_addr_t * dst, mblk_t * dp)
{
	return n_unitdata_req((caddr_t) dst, dst ? sizeof(*dst) : 0, dp);
}

/*
 *  N_OPTMGMT_REQ    9 - Options Management request
 *  -------------------------------------------------------------------------
 */
static int sccp_optmgmt_req(uint flags, caddr_t qos_ptr, size_t qos_len)
{
	return n_optmgmt_req(flags, qos_ptr, qos_len);
}

/*
 *  =========================================================================
 *
 *  TC SEND Peer Messages
 *
 *  =========================================================================
 */

static inline mblk_t *tc_enc_uni(mblk_t * dp)
{
	mblk_t *mp;
	size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* 
		 *  FIXME: build message...
		 */
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *tc_enc_qry_invoke(mblk_t * dp)
{
	mblk_t *mp;
	size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* 
		 *  FIXME: build message...
		 */
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *tc_enc_qwp_invoke(mblk_t * dp)
{
	mblk_t *mp;
	size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* 
		 *  FIXME: build message...
		 */
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *tc_enc_res_reply(mblk_t * dp)
{
	mblk_t *mp;
	size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* 
		 *  FIXME: build message...
		 */
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *tc_enc_cnv_invoke(mblk_t * dp)
{
	mblk_t *mp;
	size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* 
		 *  FIXME: build message...
		 */
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *tc_enc_cwp_invoke(mblk_t * dp)
{
	mblk_t *mp;
	size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* 
		 *  FIXME: build message...
		 */
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *tc_enc_cnv_reply(mblk_t * dp)
{
	mblk_t *mp;
	size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* 
		 *  FIXME: build message...
		 */
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *tc_enc_cwp_reply(mblk_t * dp)
{
	mblk_t *mp;
	size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* 
		 *  FIXME: build message...
		 */
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *tc_enc_abort_reject(mblk_t * dp)
{
	mblk_t *mp;
	size_t mlen = FIXME;
	if ((mp = allocb(mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* 
		 *  FIXME: build message...
		 */
		mp->b_cont = dp;
	}
	return (mp);
}

/*
 *  =========================================================================
 *
 *  TC OUTPUT EVENT Translator
 *
 *  =========================================================================
 */
static int (*tc_o_events[]) (queue_t *, mblk_t *) = {
	NULL,			/* TCE_NONE 0x00 - (reserved) */
	    tc_o_uni_inv,	/* TCE_UNI_INV 0x01 - Unidirectional Invoke */
	    tc_o_qry_inv,	/* TCE_QRY_INV 0x02 - Query w/o perm (Invoke) */
	    tc_o_qwp_inv,	/* TCE_QWP_INV 0x03 - Query w/ perm (Invoke) */
	    tc_o_rsp_rep,	/* TCE_RSP_REP 0x04 - Response (Reply) */
	    tc_o_cnv_inv,	/* TCE_CNV_INV 0x05 - Conv w/o perm (Invoke) */
	    tc_o_cwp_inv,	/* TCE_CWP_INV 0x06 - Conv w perm (Invoke) */
	    tc_o_cnv_rep,	/* TCE_CNV_REP 0x07 - Conv w/o perm (Reply) */
	    tc_o_cwp_rep,	/* TCE_CWP_REP 0x08 - Conv w perm (Reply) */
	    tc_o_abt_rej	/* TCE_ABT_REJ 0x09 - Abort (Reject) */
};
static inline int tc_o_proto(queue_t * q, mblk_t * mp)
{
	tc_event_t *m = (tc_event_t *) mp->b_wptr;
	return (*tc_o_events[m->event]) (q, mp);
}

/*
 *  =========================================================================
 *
 *  TC INPUT EVENT State Machines
 *
 *  =========================================================================
 *  These functions take an input event at the TC stream and run the state
 *  machine associated with the TC stream.
 */
/*
 *  TCE_UNI_INV	    0x01 - Unidirectional (Invoke)	
 *  ----------------------------------------------------
 */
static int tc_i_uni_inv(queue_t * q, mblk_t * mp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TCE_QRY_INV	    0x02 - Query w/o perm (Invoke)	
 *  ----------------------------------------------------
 *  ANSI: TID
 *
 *  Open a new transaction id and assign it to the stream (or provide it to
 *  the TC-User on single-threaded streams).  Mark without permission and
 *  reject TC-User attempts at conversation.
 */
static int tc_i_qry_inv(queue_t * q, mblk_t * mp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TCE_QWP_INV	    0x03 - Query w/ perm (Invoke)	
 *  ----------------------------------------------------
 *  ANSI: TID
 *
 *  Open a new transaction id and assign it to the stream (or provide it to
 *  the TC-User on single-threaded streams).  Mark with permission and permit
 *  TC-User attempts at conversation.
 */
static int tc_i_qwp_inv(queue_t * q, mblk_t * mp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TCE_RSP_REP	    0x04 - Response (Reply)		
 *  ----------------------------------------------------
 *  ANSI: TID
 *
 *  Close a transaction.  Pass response to user (with transaction id on
 *  multiple-threaded streams).
 */
static int tc_i_rsp_rep(queue_t * q, mblk_t * mp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TCE_CNV_INV	    0x05 - Conv w/o perm (Invoke)	
 *  ----------------------------------------------------
 *  ANSI: Originating TID, Responding TID
 *
 *  Continue a transaction.  Assign a correlation id to this conversation.
 *  Pass conversation to user (with transaction id on multiple-threaded
 *  streams.)  Mark conversation without permission and reject additional
 *  conversational attempts from TC-User on this transaction.
 *
 *  If conversation is not permitted, reject the invoke and abort the
 *  transaction.  Pass an abort to the user.
 */
static int tc_i_cnv_inv(queue_t * q, mblk_t * mp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TCE_CWP_INV	    0x06 - Conv w perm (Invoke)		
 *  ----------------------------------------------------
 *  ANSI: Originating TID, Responding TID
 *
 *  Continue a transaction.  Assign a correlation to this conversation.
 *  Pass conversation to user (with correlation id and transaction id on
 *  multiple-threaded streams.)  Mark conversation with permission and permit
 *  additional convesration attempts from TC-user on this conversation.
 *
 *  If conversation is not permitted, reject the invoke and abort the
 *  transaction.  Pass an abort to the user.
 */
static int tc_i_cwp_inv(queue_t * q, mblk_t * mp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TCE_CNV_REP	    0x07 - Conv w/o perm (Reply)	
 *  ----------------------------------------------------
 *  ANSI: Originating TID, Responding TID
 *
 *  Close the conversation and deallocate the correlation id.
 */
static int tc_i_cnv_rep(queue_t * q, mblk_t * mp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TCE_CWP_REP	    0x08 - Conv w perm (Reply)		
 *  ----------------------------------------------------
 *  ANSI: Originating TID, Responding TID
 *
 *  Close the conversation and deallocate the correlation id.
 */
static int tc_i_cwp_rep(queue_t * q, mblk_t * mp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TCE_ABT_REJ	    0x09 - Abort (Reject)		
 *  ----------------------------------------------------
 *  ANSI: TID
 *
 *  Abort the conversation and the transaction.  Deallocate transaction id and
 *  all correlation ids.
 */
static int tc_i_abt_rej(queue_t * q, mblk_t * mp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

static int (*tc_i_events[]) (queue_t *, mblk_t *) = {
	NULL,			/* TCE_NONE 0x00 - (reserved) */
	    tc_i_uni_inv,	/* TCE_UNI_INV 0x01 - Unidirectional Invoke */
	    tc_i_qry_inv,	/* TCE_QRY_INV 0x02 - Query w/o perm (Invoke) */
	    tc_i_qwp_inv,	/* TCE_QWP_INV 0x03 - Query w/ perm (Invoke) */
	    tc_i_rsp_rep,	/* TCE_RSP_REP 0x04 - Response (Reply) */
	    tc_i_cnv_inv,	/* TCE_CNV_INV 0x05 - Conv w/o perm (Invoke) */
	    tc_i_cwp_inv,	/* TCE_CWP_INV 0x06 - Conv w perm (Invoke) */
	    tc_i_cnv_rep,	/* TCE_CNV_REP 0x07 - Conv w/o perm (Reply) */
	    tc_i_cwp_rep,	/* TCE_CWP_REP 0x08 - Conv w perm (Reply) */
	    tc_i_abt_rej	/* TCE_ABT_REJ 0x09 - Abort (Reject) */
};
static inline int tc_i_proto(queue_t * q, mblk_t * mp)
{
	tc_event_t *e = (tc_event_t *) mp->b_rptr;
	return (*tc_i_events[e->event]) (q, mp);
}

/*
 *  =========================================================================
 *
 *  TC RECV Peer Messages
 *
 *  =========================================================================
 */

static inline int sccp_dec_uni(caddr_t p, caddr_t e, tc_event_t * e)
{
	return (0);
}
static inline int sccp_dec_qry(caddr_t p, caddr_t e, tc_event_t * e)
{
	return (0);
}
static inline int sccp_dec_qwp(caddr_t p, caddr_t e, tc_event_t * e)
{
	return (0);
}
static inline int sccp_dec_cnv(caddr_t p, caddr_t e, tc_event_t * e)
{
	return (0);
}
static inline int sccp_dec_cwp(caddr_t p, caddr_t e, tc_event_t * e)
{
	return (0);
}
static inline int sccp_dec_res(caddr_t p, caddr_t e, tc_event_t * e)
{
	return (0);
}
static inline int sccp_dec_abt(caddr_t p, caddr_t e, tc_event_t * e)
{
	return (0);
}

/*
 *  =========================================================================
 *
 *  SCCP --> TC (Upstream Primitives received from downstream)
 *
 *  =========================================================================
 */
/*
 *  N_INFO_ACK	    16 - Information Acknowledgement
 *  ----------------------------------------------------------------
 */
static int sccp_info_ack(queue_t * q, mblk_t * pdu)
{
	sccp_t *sccp = (sccp_t *) q->q_ptr;
	N_info_ack_t *p = (N_info_ack_t *) pdu->b_rptr;
	if (pdu->b_wptr = pdu->b_rptr < sizeof(*p))
		return m_error_all(q, pdu, EFAULT, EFAULT);
	sccp->flags = p->OPTIONS_flags;
	sccp->nsdu = p->NSDU_size;
	sccp->nidu = p->NIDU_size;
	sccp->nodu = p->NODU_size;
	if (p->ADDR_length >= sizeof(sccp_addr_t)) {
		sccp_addr_t *add = (sccp_addr_t *) (pdu->b_rptr + p->ADDR_offset);
		bcopy(add, sccp->bnd, sizeof(*add) + add->alen);
	}
	if (p->PROTOID_length == 1)
		sccp->ssn = *((uint8_t &) (pdu->b_rptr + p->PROTOID_offset));
	sccp->state = p->CURRENT_state;
	freemsg(pdu);
	return (0);
}

/*
 *  N_BIND_ACK	    17 - NS User bound to network address
 *  ----------------------------------------------------------------
 */
static int sccp_bind_ack(queue_t * q, mblk_t * pdu)
{
	sccp_t *sccp = (sccp_t *) q->q_ptr;
	N_bind_ack_t *p = (N_bind_ack_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return m_error_ack(q, pdu, EFAULT, EFAULT);
	if (sccp->state != NS_WACK_BREQ)
		return m_error_ack(q, pdu, EPROTO, EPROTO);
	if (p->ADDR_length >= sizeof(sccp_addr_t)) {
		sccp_addr_t *add = (sccp_addr_t *) (pdu->b_rptr + p->ADDR_offset);
		bcopy(add, sccp->bnd, sizeof(*add) + add->alen);
	}
	if (p->PROTOID_length == 1)
		sccp->ssn = *((uint8_t &) (pdu->b_rptr + p->PROTOID_offset));
	sccp->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  N_ERROR_ACK	    18 - Error Acknowledgement
 *  ----------------------------------------------------------------
 */
static int sccp_error_ack(queue_t * q, mblk_t * pdu)
{
	sccp_t *sccp = (sccp_t *) q->q_ptr;
	N_error_ack_t *p = (N_error_ack_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return m_error_ack(q, pdu, EFAULT, EFAULT);
	switch (sccp->state) {
	case NS_WACK_BREQ:
		if (p->ERROR_prim == N_BIND_REQ)
			sccp->state = NS_UNBND;
		break;
	case NS_WACK_UREQ:
		if (p->ERROR_prim == N_UNBIND_REQ)
			sccp->state = NS_IDLE;
		break;
	case NS_WACK_OPTREQ:
		if (p->ERROR_prim == N_OPTMGMT_REQ)
			sccp->state = sccp->state;
		break;
	case NS_WACK_CREQ:
		if (p->ERROR_prim == N_CONN_REQ)
			sccp->state = NS_IDLE;
		break;
	case NS_WACK_CIND:
		if (p->ERROR_prim == N_CONN_RES)
			sccp->state = sccp->state;
		break;
	case NS_WACK_CRES:
	case NS_DATA_XFER:
	case NS_WCON_RREQ:
	case NS_WRES_RIND:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
	default:
	case NS_NOSTATES:
	}
	freemsg(pdu);
	return (0)
}

/*
 *  N_OK_ACK	    19 - Success Acknowledgement
 *  ----------------------------------------------------------------
 */
static int sccp_ok_ack(queue_t * q, mblk_t * mp)
{
	sccp_t *sccp = (sccp_t *) q->q_ptr;
	N_ok_ack_t *p = (N_ok_ack_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return m_error_ack(q, pdu, EFAULT, EFAULT);
	switch (sccp->state) {
	case NS_WACK_BREQ:
	case NS_WACK_UREQ:
	case NS_WACK_OPTREQ:
	case NS_WACK_CREQ:
	case NS_WACK_CIND:
	case NS_WACK_CRES:
	case NS_DATA_XFER:
	case NS_WCON_RREQ:
	case NS_WRES_RIND:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
	default:
	case NS_NOSTATES:
	}
	/* 
	 *  TODO: more...
	 */
	freemsg(pdu);
	return (0)
}

/*
 *  N_UNITDATA_IND  20 - Connection-less data receive indication
 *  ----------------------------------------------------------------
 */
static int sccp_unitdata_ind(queue_t * q, mblk_t * pdu)
{
	sccp_t *sccp = (sccp_t *) q->q_ptr;
	N_unitdata_ind_t *p = (N_unitdata_ind_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return m_error_all(q, pdu, EFAULT, EFAULT);
	if (p->SRC_length < sizeof(sccp_addr_t))
		return m_error_all(q, pdu, EFAULT, EFAULT);
	if (p->DEST_length < sizeof(sccp_addr_t))
		return m_error_all(q, pdu, EFAULT, EFAULT);
	if (!pdu->b_cont)
		return (-EINVAL);
	return tc_recv_msg(q, pdu);
}

/*
 *  N_UDERROR_IND   21 - UNITDATA Error Indication
 *  ----------------------------------------------------------------
 */
static int sccp_uderror_ind(queue_t * q, mblk_t * pdu)
{
	tc_t *tc;
	int ecode;
	sccp_t *sccp = (sccp_t *) q->q_ptr;
	N_uderror_ind_t *p = (N_uderror_ind_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return m_error_all(q, pdu, EFAULT, EFAULT);
	if (p->DEST_length < sizeof(sccp_addr_t))
		return m_error_all(q, pdu, EFAULT, EFAULT);
	/* 
	 *  TODO: have to propagate error to TC-Users.  Most of these turn
	 *  into aborts (TC_ABORT_IND).  We still have to generate a
	 *  TCE_ABT_RES event.
	 */
	for (tc = sccp->tc; tc; tc = tc->sccp_next) {
		if (tc->outcnt > 0) {
			/* 
			 *  Send TCE_ABT_RES event to tc user....
			 */
		}
	}
	freemsg(pdu);
	return (0);
}

int (*sccp_dprim[]) (queue_t *, mblk_t *) = {
#define SCCP_DSTR_FIRST N_CONN_REQ
	NULL,			/* N_CONN_REQ 0 - NC request */
	    NULL,		/* N_CONN_RES 1 - Accept prev NC indication */
	    NULL,		/* N_DISOCN_REQ 2 - NC disconnect request */
	    NULL,		/* N_DATA_REQ 3 - CO data transfer request */
	    NULL,		/* N_EXDATA_REQ 4 - CO data transfer request */
	    NULL,		/* N_INFO_REQ 5 - Information request */
	    NULL,		/* N_BIND_REQ 6 - Bind an NS user */
	    NULL,		/* N_UNBIND_REQ 7 - Unbind an NS user */
	    NULL,		/* N_UNITDATA_REQ 8 - CL data transfer request */
	    NULL,		/* N_OPTMGMT_REQ 9 - Options mgmt request */
	    NULL,		/* (not used) 10 - (not used) */
	    NULL,		/* N_CONN_IND 11 - Incoming NC indication */
	    NULL,		/* N_CONN_CON 12 - NC established */
	    NULL,		/* N_DISCON_IND 13 - NC disconnected */
	    NULL,		/* N_DATA_IND 14 - CO data transfer indication */
	    NULL,		/* N_EXDATA_IND 15 - CO data transfer indication */
	    sccp_info_ack,	/* N_INFO_ACK 16 - Information Acknowledgement */
	    sccp_bind_ack,	/* N_BIND_ACK 17 - NS User bound Ack */
	    sccp_error_ack,	/* N_ERROR_ACK 18 - Error Acknowledgement */
	    sccp_ok_ack,	/* N_OK_ACK 19 - Success Acknowledgement */
	    sccp_unitdata_ind,	/* N_UNITDATA_IND 20 - CL data transfer indication */
	    sccp_uderror_ind,	/* N_UDERROR_IND 21 - CL data error indication */
	    NULL,		/* (not used) 22 - (not used) */
	    NULL,		/* N_DATACK_REQ 23 - Data ack request */
	    NULL,		/* N_DATACK_IND 24 - Data ack indication */
	    NULL,		/* N_RESET_REQ 25 - NC reset request */
	    NULL,		/* N_RESET_IND 26 - Incoming NC reset ind */
	    NULL,		/* N_RESET_RES 27 - Reset processing accepted */
	    NULL		/* N_RESET_CON 28 - Reset processing complete */
#define SCCP_DSTR_LAST	N_RESET_CON
};

static int tc_r_data(queue_t * q, mblk_t * mp)
{
	/* 
	 *  This is an SCCP unitdata message from below.  We don't get SCCP
	 *  unitdata messages as M_DATA, so if this happens we ignore it.
	 */
	freemsg(mp);
	return (0);
}
static int tc_r_proto(queue_t * q, mblk_t * mp);
{
}

/*
 *  =========================================================================
 *
 *  TC-User --> TC (Downstream Primitives received from upstream)
 *
 *  =========================================================================
 */
/*
 *  TC Options parsing.
 */
static int parse_options(queue_t * q, mblk_t * pdu, int prim, caddr_t opt_ptr, size_t opt_len)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	if (opt_len && opt_len < sizeof(uint32_t))
		return tc_error_ack_reply(q, pdu, prim, TCBADOPT);
	/* 
	 *  Decode the options....
	 */
	return (0);
}

/*
 *  TC_INFO_REQ           0 - Information request                     
 *  ---------------------------------------------------------------
 */
static int user_info_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_info_req_t *p = (TC_info_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_INFO_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	freemsg(pdu);
	return (0);
}

/*
 *  TC_BIND_REQ           1 - Bind to network address                 
 *  ---------------------------------------------------------------
 */
static int user_bind_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_bind_req_t *p = (TC_bind_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_UNBIND_REQ         2 - Unbind from network address             
 *  ---------------------------------------------------------------
 */
static int user_unbind_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_unbind_req_t *p = (TC_unbind_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_UNIDIR_REQ         3 - Unidirectional request                  
 *  ---------------------------------------------------------------
 */
static int user_unidir_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_unidir_req_t *p = (TC_unidir_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_BEGIN_REQ          4 - Begin transaction request               
 *  ---------------------------------------------------------------
 */
static int user_begin_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_begin_req_t *p = (TC_begin_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_QUERY_W_PERM_REQ   5 - Begin transaction request               
 *  ---------------------------------------------------------------
 */
static int user_query_w_perm_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_query_w_perm_req_t *p = (TC_query_w_perm_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_QUERY_WO_PERM_REQ  6 - Begin transaction request               
 *  ---------------------------------------------------------------
 */
static int user_query_wo_perm_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_query_wo_perm_req_t *p = (TC_query_wo_perm_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_END_REQ            7 - End transaction request                 
 *  ---------------------------------------------------------------
 */
static int user_end_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_end_req_t *p = (TC_end_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_RESP_REQ           8 - End transaction request                 
 *  ---------------------------------------------------------------
 */
static int user_resp_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_resp_req_t *p = (TC_resp_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_CONTINUE_REQ       9 - Continue transaction request            
 *  ---------------------------------------------------------------
 */
static int user_continue_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_continue_req_t *p = (TC_continue_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_CONTINUE_RES      10 - Continue transaction response           
 *  ---------------------------------------------------------------
 */
static int user_continue_res(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_continue_res_t *p = (TC_continue_res_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_CONV_W_PERM_REQ   11 - Continue transaction request            
 *  ---------------------------------------------------------------
 */
static int user_conv_w_perm_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_conv_w_perm_req_t *p = (TC_conv_w_perm_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_CONV_WO_PERM_REQ  12 - Continue transaction request            
 *  ---------------------------------------------------------------
 */
static int user_conv_wo_perm_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_conv_wo_perm_req_t *p = (TC_conv_wo_perm_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_CONV_W_PERM_RES   13 - Continue transaction response           
 *  ---------------------------------------------------------------
 */
static int user_conv_w_perm_res(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_conv_w_perm_res_t *p = (TC_conv_w_perm_res_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_CONV_WO_PERM_RES  14 - Continue transaction response           
 *  ---------------------------------------------------------------
 */
static int user_conv_wo_perm_res(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_conv_wo_perm_res_t *p = (TC_conv_wo_perm_res_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TC_ABORT_REQ         15 - Abort transaction request               
 *  ---------------------------------------------------------------
 */
static int user_abort_req(queue_t * q, mblk_t * pdu)
{
	int err;
	tc_t *tc = (tc_t *) q->q_ptr;
	TC_abort_req_t *p = (TC_abort_req_t *) pdu->b_rptr;
	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tc_error_ack_reply(q, TC_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tc->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

int (*user_dprim[]) (queue_t *, mblk_t *) = {
#define USER_DSTR_FIRST	TC_INFO_REQ
	tc_info_req,		/* TC_INFO_REQ 0 - Information request */
	    tc_bind_req,	/* TC_BIND_REQ 1 - Bind to network address */
	    tc_unbind_req,	/* TC_UNBIND_REQ 2 - Unbind from network address */
	    tc_unidir_req,	/* TC_UNIDIR_REQ 3 - Unidirectional request */
	    tc_begin_req,	/* TC_BEGIN_REQ 4 - Begin transaction request */
	    tc_qwp_req,		/* TC_QUERY_W_PERM_REQ 5 - Begin transaction request */
	    tc_qwop_req,	/* TC_QUERY_WO_PERM_REQ 6 - Begin transaction request */
	    tc_end_req,		/* TC_END_REQ 7 - End transaction request */
	    tc_resp_req,	/* TC_RESP_REQ 8 - End transaction request */
	    tc_cont_req,	/* TC_CONTINUE_REQ 9 - Continue transaction request */
	    tc_cont_res,	/* TC_CONTINUE_RES 10 - Continue transaction response */
	    tc_cwp_req,		/* TC_CONV_W_PERM_REQ 11 - Continue transaction request */
	    tc_cwop_req,	/* TC_CONV_WO_PERM_REQ 12 - Continue transaction request */
	    tc_cwp_res,		/* TC_CONV_W_PERM_RES 13 - Continue transaction response */
	    tc_cwop_res,	/* TC_CONV_WO_PERM_RES 14 - Continue transaction response */
	    tc_abort_req	/* TC_ABORT_REQ 15 - Abort transaction request */
#define USER_DSTR_LAST  TC_ABORT_REQ
};

static int tc_w_data(queue_t * q, mblk_t * pdu)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	if (tc->state != TCS_XACT_OPEN) {
		/* 
		 *  If we are not in a connection oriented state, we return an
		 *  M_ERROR on all subsequent write operations on the stream
		 *  will fail.
		 */
		mblk_t *mp;
		if (!(mp = m_error(0, EPROTO)))
			return (-ENOBUFS);
		qreply(q, mp);
		freemsg(pdu);
		return (0);
	}
	/* 
	 *  This is an TC data message from above.  This is only valid in
	 *  connection-oriented states.
	 */
	/* 
	 *  TODO: pass data message to TC.
	 */
	return (0);
}
static int tc_w_proto(queue_t * q, mblk_t * mp)
{
	uint32_t prim = *((uint32_t *) mp->b_rptr);
	if (USER_DSTR_FIRST <= prim && prim <= USER_DSTR_LAST && user_dprim[prim])
		return (*user_dprim[prim]) (q, mp);
	/* 
	 *  FIXME: We should probably send an M_ERROR here.
	 */
	return (-EOPNOTSUPP);
}

/*
 *  =========================================================================
 *
 *  M_CTL Processing
 *
 *  =========================================================================
 *  We have no defined module to module controls for either TC or SCCP.
 */
static int tc_r_ctl(queue_t * q, mblk_t * pdu)
{
	int err;
	mblk_t *mp;
	if ((err = m_error_all(q, NULL, EFAULT, EFAULT)))
		return (err);
	if (!(mp = sccp_unbind_req()))
		return (-ENOBUFS);
	qreply(q, mp);
	freemsg(pdu);
	return (0);
}
static int tc_w_ctl(queue_t * q, mblk_t * pdu)
{
	/* 
	 *  FIXME: Remove this TC-User from the SCCP-Provider...
	 */
	return m_error_reply(q, pdu, EFAULT, EFAULT);
}

/*
 *  =========================================================================
 *
 *  M_ERROR Processing
 *
 *  =========================================================================
 *  We propagate any M_ERROR(s) received on the lower stream from the SCCP
 *  Provider to every TC-User upper stream which is using the SCCP provider.
 *  We also set states here to indicate the error.  This is because if we run
 *  out of mblks to propagate the M_ERROR, we will discard the priority
 *  message (M_ERROR) if we are in the read service routine.  Setting the
 *  error ensures that we will respond with an M_ERROR later when resources
 *  are available.
 */
static int tc_r_error(queue_t * q, mblk_t * pdu)
{
	/* 
	 *  FIXME: Notify this SCCP-Provider's management stream that the
	 *  SCCP-Provider has errored out...
	 */
	return m_error_all(q, pdu, *(pdu->r_ptr + 4), *(pdu->w_ptr - 1));
}

/*
 *  =========================================================================
 *
 *  M_HANGUP Processing
 *
 *  =========================================================================
 *  We propagate any M_HANGUP(s) received on the lower stream from the SCCP
 *  Provider to every TC-User upper stream which is using the SCCP provider.
 *  We also set states here to indicate the error.  This is because if we run
 *  out of mblks to propagate the M_HANGUP, we will discard the priority
 *  message (M_HANGUP) if we are in the read service routine.  Setting the
 *  error ensures that we will respond with an M_HANGUP later when resources
 *  are available.
 */
static int tc_r_hangup(queue_t * q, mblk_t * pdu)
{
	/* 
	 *  FIXME: Notify this SCCP-Provider's management stream that the
	 *  SCCP-Provider has hung up...
	 */
	return m_hangup_all(q, pdu);
}

/*
 *  =========================================================================
 *
 *  M_FLUSH Processing
 *
 *  =========================================================================
 *  We turn flushes around at the multiplexing driver.  When we receive
 *  flushes from above we turn them back around upwards.  When we receive
 *  flushes from below we turn them back around downwards.
 */
/*
 *  M_FLUSH from below (i.e. from SCCP Provider)
 *  -------------------------------------------------------------------------
 *  We want to take flushes from below and turn them back around back down to
 *  the SCCP provider.
 */
static int tc_r_flush(queue_t * q, mblk_t * pdu)
{
	int err;
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		/* flush all TC-Users upwards */
		m_flush_all(q, NULL, mp->b_rptr[0], mp->b_rptr[1]);
		*mp->b_rptr &= ~FLUSHR;
	}
	if (!(mp->b_flag & MSGNOLOOP)) {
		if (*mp->b_rptr & FLUSHW) {
			if (*mp->b_rptr & FLUSHBAND)
				flushband(WR(q), mp->b_rptr[1], FLUSHALL);
			else
				flushq(WR(q), FLUSHALL);
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);	/* flush all the way back down */
			return (0);
		}
	}
	freemsg(mp);
	return (0);
}

/*
 *  M_FLUSH from above (i.e. from TC User)
 *  -------------------------------------------------------------------------
 *  We want to take flushes from above and turn them back around back up to
 *  the TC User.
 */
static int tc_w_flush(queue_t * q, mblk_t * pdu)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		*mp->b_rptr &= ~FLUSHW;
	}
	if (!(mp->b_flag & MSGNOLOOP)) {
		if (*mp->b_rptr & FLUSHR) {
			if (*mp->b_rptr & FLUSHBAND)
				flushband(RD(q), mp->b_rptr[1], FLUSHALL);
			else
				flushq(RD(q), FLUSHALL);
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);	/* flush all the way back up */
			return (0);
		}
	}
	freemsg(mp);
	return (0);
}

/*
 *  M_FLUSH internal from below.
 *  -------------------------------------------------------------------------
 */
static int tc_i_flush(queue_t * q, mblk_t * pdu)
{
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	putnext(q, mp);
}

/*
 *  M_FLUSH internal from above.
 *  -------------------------------------------------------------------------
 */
static int tc_o_flush(queue_t * q, mblk_t * pdu)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	putnext(q, mp);
}

/*
 *  =========================================================================
 *
 *  M_IOCTL Processing
 *
 *  =========================================================================
 *
 *  I_LINK, I_PLINK Handling
 *  -------------------------------------------------------------------------
 *  SCCP Provider streams can be linked under the TC multiplexing driver.  We
 *  permit normal TC streams to perform an I_LINK of SCCP Provider streams
 *  under the SCCP driver, but we only permit the TC control stream to
 *  perform an I_PLINK of SCCP Provider streams.  This is so that if the
 *  configuration daemon crashes it can come back an the configuration has not
 *  been destroyed.
 *
 *  When linked, we generate an N_INFO_REQ downstream to the newly linked SCCP
 *  Provider.  The purpose of this is to discover the response from the
 *  provider as to which state the provider is currently in and which
 *  addresses the SCCP Provider might be currently bound to.  This permits the
 *  caller to bind the SCCP Provider.
 */
static inline int tc_i_link(queue_t * q, mblk_t * mp, struct linkblk *lp)
{
	mblk_t *mp;
	sccp_t *sccp;
	queue_t *lq;
	ensure(lp, return (-EFAULT));
	lq = RD(lp->l_qbot);
	if (!(mp = sccp_info_req()))
		return (-ENOBUFS);
	if (!(sccp = kmalloc(sizeof(*sccp), GFP_KERNEL))) {
		freemsg(mp);
		return (-ENOMEM);
	}
	bzero(sccp, sizeof(*sccp));
	sccp->q = RD(lq);
	sccp->lmq = RD(q);
	sccp->muxid = lp->l_index;
	sccp->state = NS_UNBND;
	RD(lq)->q_ptr = WR(lq)->q_ptr = sccp;
	putnext(WR(lq), mp);	/* get info on SCCP provider */
	if (sccp->state != NS_IDLE) {
		/* sccp providers must be prebound */
		RD(lq)->q_ptr = WR(lq)->q_ptr = NULL;
		kfree(sccp);
		return (-EINVAL);
	}
	if ((sccp->next = sccp_links))
		sccp->next->prev = &sccp->next;
	sccp->prev = &sccp_links;
	return (lp->l_index);
}

/*
 *  I_UNLINK, I_PUNLINK Handling
 *  -------------------------------------------------------------------------
 *  When we unlink, if we still have referencing upper TC-User streams, we
 *  send each of the a M_HANGUP message indicating the loss of the connection.
 *  This might result in a SIG_PIPE signal being sent to the process if the
 *  TC-User is a stream head.
 */
static inline int tc_i_unlink(queue_t * q, struct linkblk *lp)
{
	int err;
	sccp_t *sccp;
	uint muxid;
	ensure(lp, return (-EFAULT));
	muxid = lp->l_index;
	for (sccp = sccp_links; sccp && sccp->muxid != muxid; sccp = sccp->next);
	if (!sccp)
		return (-EINVAL);
	if ((err = m_flush_all(q, NULL, FLUSHW, 0)))
		return (err);
	if ((err = m_hangup_all(q, NULL)))
		return (err);
	if (sccp->state == NS_IDLE) {
		mblk_t *mp;
		if (!(mp = sccp_unbind_req()))
			return (-ENOBUFS);
		sccp->state = NS_WACK_UREQ;
		putnext(WR(sccp->q), mp);
	}
	if ((*(sccp->prev) = sccp->next))
		sccp->next->prev = sccp->prev;
	sccp->q->q_ptr = WR(sccp->q)->q_ptr = NULL;
	kfree(sccp);
	return (0);
}
static int tc_w_ioctl(queue_t * q, mblk_t * pdu)
{
	int ret = -EINVAL;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	struct iocblk *iocp = (struct iocblk *) mp->b_wptr;
	int cmd = iocp->ioc_cmd;
	switch (_IOC_TYPE(cmd)) {
	case __SID:
		switch (cmd) {
		case I_PLINK:
			if (RD(q) != tc_lmq) {
				ret = -EPERM;
				break;
			}
		case I_LINK:
			ret = tc_i_link(q, arg);
			break;
		case I_PUNLINK:
			if (RD(q) != tc_lmq) {
				ret = -EPERM;
				break;
			}
		case I_UNLINK:
			ret = tc_i_unlink(q, arg);
			break;
		}
		break;
	case TC_IOC_MAGIC:
		if (iocp->ioc_count >= _IOC_SIZE(cmd)) {
			int nr = _IOC_NR(cmd);
			ret = -EOPNOTSUPP;
			if (0 <= nr && nr < sizeof(tc_ioctl) / sizeof(int (*)(void))
			    && tc_ioctl[nr])
				ret = (*tc_ioctl[nr]) (q, cmd, arg);
		}
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}
	mp->b_datap->db_type = ret < 0 ? M_IOCNAK : M_IOCACK;
	iocp->ioc_error = ret < 0 ? -ret : 0;
	iocp->ioc_rval = ret < 0 ? -1 : ret;
	qreply(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  QUEUE PUT and SERVICE routines
 *
 *  =========================================================================
 */
static int tc_u_wput(queue_t *, mblk_t *);
static int tc_u_wsrv(queue_t *);

static int tc_l_wsrv(queue_t *);
static int tc_l_rput(queue_t *, mblk_t *);

static int tc_u_rsrv(queue_t *);
static int tc_l_rsrv(queue_t *);

static inline int tc_recover(queue_t * q, mblk_t * mp, int err)
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

/*
 *  READ PUT and SERVICE (Message from below SCCP-Provider --> TC)
 *  -------------------------------------------------------------------------
 */
static inline int tc_rd(queue_t * q, mblk_t * mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return tc_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tc_r_proto(q, mp);
	case M_CTL:
		return tc_r_ctl(q, mp);
	case M_ERROR:
		return tc_r_error(q, mp);
	case M_FLUSH:
		return tc_r_flush(q, mp);
	case M_HANGUP:
		return tc_r_hangup(q, mp);
	}
	return (-EOPNOTSUPP);
}
static int tc_l_rput(queue_t * q, mblk_t * mp)
{
	int err;
	if (mp->b_datap->db_type < QPCTL && (q->q_count || !canputnext(q))) {
		putq(q, mp);
		return (0);
	}
	if ((err = tc_rd(q, mp)))
		return (tc_recover(q, mp, err));
	return (0);
}
static int tc_l_rsrv(queue_t * q)
{
	mblk_t *mp;
	while ((mp = getq(q))) {
		int err;
		if (!(err = tc_rd(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (tc_recover(q, mp, err));
		freemsg(mp);
		return (err);
	}
	return (0);
}

/*
 *  WRITE PUT and SERVICE (Message from above TC-User --> TC)
 *  -------------------------------------------------------------------------
 */
static inline int tc_wr(queue_t * q, mblk_t * mp)
{
	switch (mp->b_datap->db - type) {
	case M_DATA:
		return tc_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tc_w_proto(q, mp);
	case M_CTL:
		return tc_w_ctl(q, mp);
	case M_FLUSH:
		return tc_w_flush(q, mp);
	case M_IOCTL:
		return ttcioctl(q, mp);
	}
	return (-EOPNOTSUPP);
}
static int tc_u_wput(queue_t * q, mblk_t * mp)
{
	int err;
	if (mp->b_datap->db_type < QPCTL && (q->q_count || !canputnext(q))) {
		putq(q, mp);
		return (0);
	}
	if ((err = tc_wr(q, mp)))
		return (tc_recover(q, mp, err));
	return (0);
}
static int tc_u_wsrv(queue_t * q)
{
	mblk_t *mp;
	while ((mp = getq(q))) {
		int err;
		if (!(err = tc_wr(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (tc_recover(q, mp, err));
		freemsg(mp);
		return (err);
	}
	return (0);
}

/*
 *  READ SERVICE (Event from below as input to TC State Machines)
 *  -------------------------------------------------------------------------
 */
static inline int tc_input(queue_t * q, mblk_t * mp)
{
	uint type = mp->b_datap->db_type;
	if (M_DATA <= type && type <= M_STOPI && tc_input_ops[type])
		return (*tc_input_ops[type]) (q, mp);
	putnext(q, mp);
	return (0);
}
static inline int tc_input(queue_t * q, mblk_t * mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return (-EOPNOTSUPP);
	case M_PROTO:
	case M_PCPROTO:
		return tc_i_proto(q, mp);
	case M_FLUSH:
		return tc_i_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}
static int tc_u_rsrv(queue_t * q)
{
	mblk_t *mp;
	while ((mp = getq(q))) {
		int err;
		if (!(err = tc_input(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (tc_recover(q, mp, err));
		freemsg(mp);
		return (err);
	}
}

/*
 *  WRITE SERVICE (Event from above as output from TC State Machines)
 *  -------------------------------------------------------------------------
 */
static inline int tc_output(queue_t * q, mblk_t * mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return (-EOPNOTSUPP);
	case M_PROTO:
	case M_PCPROTO:
		return tc_o_proto(q, mp);
	case M_FLUSH:
		return tc_o_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}
static int tc_l_wsrv(queue_t * q)
{
	mblk_t *mp;
	while ((mp = getq(q))) {
		int err;
		if (!(err = tc_output(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (tc_recover(q, mp, err));
		freemsg(mp);
		return (err);
	}
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
static int tc_open(queue_t *, dev_t *, int, int, cred_t *);
static int tc_close(queue_t *, int, cred_t *);

static int tc_open(queue_t * q, dev_t * devp, int flag, int sflag, cred_t * crp);
{
	tc_t *tc, **tcp = &tc_opens;
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);

	if (q->q_ptr != NULL)
		return (0);	/* already open */

	if (sflag == MODOPEN || WR(q)->q_next)
		return (EIO);	/* cannot push as module */
	if (!cminor && !tc_lmq) {
		if (crp->cr_ruid != 0)
			return (EPERM);	/* need r00t */
		tc_lmq = q;
	} else if (!cminor || sflag == CLONEOPEN) {
		cminor = 1;
	}
	for (; cminor <= TC_NMINOR && *tcp; tcp = &(*tcp)->next) {
		int dminor = getminor((*tcp)->devid);
		if (cminor < dminor)
			break;
		if (cminor == dminor) {
			if (sflag == CLONEOPEN) {
				cminor++;
				continue;
			}
			return (EIO);
		}
	}
	if (cminor > TC_NMINOR)
		return (ENXIO);
	*devp = makedevice(cmajor, cminor);
	if (!(tc = kmalloc(sizeof(*tc), GFP_KERNEL)))
		return (ENOMEM);
	bzero(tc, sizeof(*tc));
	q->q_ptr = WR(q)->q_ptr = tc;
	tc->q = q;
	tc->devid = *devp;
	tc->state = 0;
	if ((tc->next = *tcp))
		tc->next->prev = &tc->next;
	tc->prev = tcp;
	*tcp = tc;
	return (0);
}
static int tc_close(queue_t * q, int flag, cred_t * crp)
{
	tc_t *tc = (tc_t *) q->q_ptr;
	if (!tc)
		return (EIO);
	if (q == tc_lmq)
		tc_lmq = NULL;
	if (tc->sccp) {
		/* 
		 *  Detach this TC stream from whatever SCCP streams it is
		 *  using on the lower half.
		 */
		sccp_t *sccp = tc->sccp;
		if ((--tc->sccp->use_count) <= 0) {
			/* 
			 *  Unbind the SCCP stream if it is not being used by
			 *  any TC streams.
			 */
			mblk_t *mp;
			N_unbind_req_t *p;
			if (!(mp = allocb(sizeof(*p), BPRI_MED)))
				return (ENOSR);
			mp->b_datap->db_type = M_PCPROTO;
			p = (N_unbind_req_t *) mp->b_wptr;
			p->PRIM_type = N_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			sccp->state = NS_WACK_UREQ;
			putnext(WR(sccp->q), mp);
			sccp->use_count = 0;
		}
		if ((*(tc->sccp_prev) = tc->sccp_next))
			tc->sccp_next->sccp_prev = tc->sccp_prev;
	}
	/* 
	 *  TODO:  Might want to do some more things to deallocate some TC
	 *  structures associated with the module.  I.e. some TC bindings.
	 */
	if ((*(tc->prev) = tc->next))
		tc->next->prev = tc->prev;
	WR(q)->q_ptr = RD(q)->q_ptr = NULL;
	kfree(tc);
	return (0);
}

/*
 *  =========================================================================
 *
 *  LIS STREAMS INITIALIZATION
 *
 *  =========================================================================
 */
static int tc_initialized = 0;

#ifndef LIS_REGISTERED
static inline int tc_init(void)
#else
__initfunc(int tc_init(void))
#endif
{
	int err;
	if (tc_initialized)
		return (0);
	tc_initialized = 1;
	cmn_err(CE_NOTE, TC_BANNER);	/* console splash */
#ifndef LIS_REGISTERED
	if ((err = lis_register_strmod(&tc_info, tc_minfo.ni_name)) < 0)
		cmn_err(CE_WARN "tc: couldn't register module!\n");
	return (err);
#endif
}

#ifndef LIS_REGISTERED
static inline void tc_terminate(void)
#else
__initfunc(void tc_terminate(void))
#endif
{
	if (!tc_initialized)
		return;
	tc_initialized = 0;
#ifndef LIS_REGISTERED
	if (lis_unregister_strmod(&tc_info))
		cmn_err(CE_WARN "tc: couldn't unregister module!\n");
#endif
	return;
}

/*
 *  =========================================================================
 *
 *  LINUX MODULE INITIALIZATION
 *
 *  =========================================================================
 */

#ifdef MODULE
int init_module(void)
{
	return tc_init();
}
void cleanup_module(void)
{
	tc_terminate();
}
#endif
