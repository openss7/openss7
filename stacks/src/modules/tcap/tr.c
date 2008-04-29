/*****************************************************************************

 @(#) $RCSfile: tr.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2008-04-29 07:11:16 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-29 07:11:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tr.c,v $
 Revision 0.9.2.15  2008-04-29 07:11:16  brian
 - updating headers for release

 Revision 0.9.2.14  2007/08/15 05:20:41  brian
 - GPLv3 updates

 Revision 0.9.2.13  2007/08/14 12:18:55  brian
 - GPLv3 header updates

 *****************************************************************************/

#ident "@(#) $RCSfile: tr.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2008-04-29 07:11:16 $"

static char const ident[] =
    "$RCSfile: tr.c,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2008-04-29 07:11:16 $";

/*
 *  This is a TR (Transaction Sub-Layer) mulitplexing driver for SS7 TCAP.
 *  It is opened and SCCP streams are I_LINKed under the driver by the TR User
 *  or I_PLINKed under the driver by the TR control stream for use by TR
 *  Users.  TR Users are associated with lower SCCP streams via the TR bind
 *  commands.
 *
 *  This includes a management stream for module reporting and configuration
 *  management.
 */

#include <sys/os7/compat.h>

#define TR_DESCRIP	"TCAP TR STREAMS MULTIPLEXING DRIVER."
#define TR_REVISION	"LfS $RCSfile: tr.c,v $ $Name:  $ ($Revision: 0.9.2.15 $) $Date"
#define TR_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define TR_DEVICE	"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define TR_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TR_LICENSE	"GPL"
#define TR_BANNER	TR_DESCRIP	"\n" \
			TR_REVISION	"\n" \
			TR_COPYRIGHT	"\n" \
			TR_DEVICE	"\n" \
			TR_CONTACT	"\n"
#define TR_SPLASH	TR_DEVICE	" - " \
			TR_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(TR_CONTACT);
MODULE_DESCRIPTION(TR_DESCRIP);
MODULE_SUPPORTED_DEVICE(TR_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TR_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-tr");
#endif
#endif				/* LINUX */

#ifdef TR_DEBUG
static int tr_debug = TR_DEBUG;
#else
static int tr_debug = 2;
#endif

#ifdef LFS
#define TR_DRV_ID	CONFIG_STREAMS_TR_MODID
#define TR_DRV_NAME	CONFIG_STREAMS_TR_NAME
#define TR_CMAJORS	CONFIG_STREAMS_TR_NMAJORS
#define TR_CMAJOR_0	CONFIG_STREAMS_TR_MAJOR
#define TR_NMINOR	CONFIG_STREAMS_TR_NMINORS
#endif

#ifndef TR_CMAJOR_0
#define TR_CMAJOR_0	210
#endif
#ifndef TR_NMINOR
#define TR_NMINOR	255
#endif
#ifndef TR_DRV_ID
#define TR_DRV_ID	('s'<<24|'s'<<16|'7'<<8|TR_IOC_MAGIC)
#endif
#ifndef TR_DRV_NAME
#define TR_DRV_NAME	"tcap-tr"
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

static struct module_info tr_minfo = {
	TR_DRV_ID,			/* Module ID number */
	TR_DRV_NAME,			/* Module name */
	1,				/* Min packet size accepted *//* XXX */
	254,				/* Max packet size accepted *//* XXX */
	8 * 512,			/* Hi water mark *//* XXX */
	1 * 512				/* Lo water mark *//* XXX */
};

static int tr_open(queue_t *, dev_t *, int, int, cred_t *);
static int tr_close(queue_t *, int, cred_t *);

static int tr_u_rsrv(queue_t *);
static int tr_u_wput(queue_t *, mblk_t *);
static int tr_u_wsrv(queue_t *);

static struct qinit tr_u_rinit = {
	NULL,				/* Read put (msg from below) */
	tr_u_rsrv,			/* Read queue service */
	tr_open,			/* Each open */
	tr_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&tr_minfo,			/* Information */
	NULL				/* Statistics */
};
static struct qinit tr_u_wrinit = {
	tr_u_wput,			/* Write put (msg from above) */
	tr_u_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&tr_info,			/* Information */
	NULL				/* Statistics */
};

static int tr_l_wsrv(queue_t *);
static int tr_l_rput(queue_t *, mblk_t *);
static int tr_l_rsrv(queue_t *);

static struct qinit tr_l_winit = {
	NULL,				/* Write put (msg from above) */
	tr_l_wsrv,			/* Write queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&tr_info,			/* Information */
	NULL				/* Statistics */
};
static struct qinit tr_l_rinit = {
	tr_l_rput,			/* Read put (msg from above) */
	tr_l_rsrv,			/* Read queue service */
	NULL,				/* Each open */
	NULL,				/* Last close */
	NULL,				/* Admin (not used) */
	&tr_info,			/* Information */
	NULL				/* Statistics */
};

MODULE_STATIC struct streamtab tr_info = {
	&tr_u_rinit,			/* Upper read queue */
	&tr_u_winit,			/* Upper write queue */
	&tr_l_rinit,			/* Lower read queue */
	&tr_l_winit			/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  TR Private Structures
 *
 *  =========================================================================
 */

struct tr;
struct sccp;

typedef struct tr {
	struct tr *next;		/* list of all TR-Users */
	struct tr **prev;		/* list of all TR-Users */
	queue_t *q;			/* associated read queue */
	dev_t devid;			/* device id at open */
	uint state;			/* interface state */
} sccp_t;

static uint tr_next_transaction_id = 1;

/*
 *  =========================================================================
 *
 *  TR Message Structures
 *
 *  =========================================================================
 */
// #include "tr_msg.h"

/*
 *  =========================================================================
 *
 *  Module --> Module (Control Messages)
 *
 *  =========================================================================
 */

static inline mblk_t *
m_error(int r_error, int w_error)
{
	mblk_t *mp;

	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = r_error;
		*(mp->b_wptr)++ = w_error;
	}
	return (mp);
}
static inline mblk_t *
m_hangup(void)
{
	mblk_t *mp;

	if ((mp = allogb(0, BPRI_HI))) {
		mp->b_datap->db_type = M_HANGUP;
	}
	return (mp);
}
static inline mblk_t *
m_flush(int flags, int band)
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
 *  TR --> TR-User (Upstream Primitives sent upstream)
 *
 *  =========================================================================
 */

/*
 *  TR_INFO_ACK		 Information acknowledgement
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_info_ack(void)
{
	mblk_t *mp;
	TR_info_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_info_ack_t *) mp->b_wptr;
		p->PRIM_type = TR_INFO_ACK;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_BIND_ACK		 Bound to global title
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_bind_ack(void)
{
	mblk_t *mp;
	TR_bind_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_bind_ack_t *) mp->b_wptr;
		p->PRIM_type = TR_BIND_ACK;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_OK_ACK		 Success acknowledgement
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_ok_ack(void)
{
	mblk_t *mp;
	TR_ok_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_ok_ack_t *) mp->b_wptr;
		p->PRIM_type = TR_OK_ACK;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_ERROR_ACK	 Error acknowledgement
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_error_ack(void)
{
	mblk_t *mp;
	TR_error_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_error_ack_t *) mp->b_wptr;
		p->PRIM_type = TR_ERROR_ACK;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_UNIDIR_IND	 Unidirectional Indication
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_unidir_ind(void)
{
	mblk_t *mp;
	TR_unidir_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_unidir_ind_t *) mp->b_wptr;
		p->PRIM_type = TR_UNIDIR_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_QUERY_W_PERM_IND	 Incoming Query with permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_query_w_perm_ind(void)
{
	mblk_t *mp;
	TR_query_w_perm_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_query_w_perm_ind_t *) mp->b_wptr;
		p->PRIM_type = TR_QUERY_W_PERM_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_QUERY_WO_PERM_IND Incoming Query without permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_query_wo_perm_ind(void)
{
	mblk_t *mp;
	TR_query_wo_perm_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_query_wo_perm_ind_t *) mp->b_wptr;
		p->PRIM_type = TR_QUERY_WO_PERM_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_RESP_IND		 Incoming Response
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_resp_ind(void)
{
	mblk_t *mp;
	TR_resp_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_resp_ind_t *) mp->b_wptr;
		p->PRIM_type = TR_RESP_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_CONV_W_PERM_IND	 Incoming Conversation with permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_conv_w_perm_ind(void)
{
	mblk_t *mp;
	TR_conv_w_perm_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_conv_w_perm_ind_t *) mp->b_wptr;
		p->PRIM_type = TR_CONV_W_PERM_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_CONV_WO_PERM_IND	 Incoming Conversation without permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_conv_wo_perm_ind(void)
{
	mblk_t *mp;
	TR_conv_wo_perm_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_conv_wo_perm_ind_t *) mp->b_wptr;
		p->PRIM_type = TR_CONV_WO_PERM_IND;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_CONV_W_PERM_CON	 Incoming Conversation with permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_conv_w_perm_con(void)
{
	mblk_t *mp;
	TR_conv_w_perm_con_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_conv_w_perm_con_t *) mp->b_wptr;
		p->PRIM_type = TR_CONV_W_PERM_CON;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_CONV_WO_PERM_CON	 Incoming Conversation without permission to continue
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_conv_wo_perm_con(void)
{
	mblk_t *mp;
	TR_conv_wo_perm_con_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_conv_wo_perm_con_t *) mp->b_wptr;
		p->PRIM_type = TR_CONV_WO_PERM_CON;
		/* 
		 *  FIXME: more...
		 */
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  TR_ABORT_IND	 Abort indication
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
tr_abort_ind(void)
{
	mblk_t *mp;
	TR_abort_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (T_abort_ind_t *) mp->b_wptr;
		p->PRIM_type = TR_ABORT_IND;
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
 *  TR --> SCCP (Downstream Primitives sent downstream)
 *
 *  =========================================================================
 */
#include "../npi/npi_user.h"

/*
 *  N_INFO_REQ	     5 - Information Request
 *  -------------------------------------------------------------------------
 */
static int
sccp_info_req(void)
{
	return n_info_req();
}

/*
 *  N_BIND_REQ	     6 - Bind a NS user to network address
 *  -------------------------------------------------------------------------
 */
static int
sccp_bind_req(uint flags, sccp_addr_t * bnd, uint32_t ssn)
{
	return n_bind_req(flags, 0, (caddr_t) bnd, bnd ? sizeof(*bind) : 0, (caddr_t) &ssn,
			  sizeof(ssn));
}

/*
 *  N_UNBIND_REQ     7 - Unbind NS user from network address
 *  -------------------------------------------------------------------------
 */
static int
sccp_unbind_req(void)
{
	return n_unbind_req();
}

/*
 *  N_UNITDATA_REQ   8 - Connection-less data send request
 *  -------------------------------------------------------------------------
 */
static int
sccp_unitdata_req(sccp_addr_t * dst, mblk_t *dp)
{
	return n_unitdata_req((caddr_t) dst, dst ? sizeof(*dst) : 0, dp);
}

/*
 *  N_OPTMGMT_REQ    9 - Options Management request
 *  -------------------------------------------------------------------------
 */
static int
sccp_optmgmt_req(uint flags, caddr_t qos_ptr, size_t qos_len)
{
	return n_optmgmt_req(flags, qos_ptr, qos_len);
}

/*
 *  =========================================================================
 *
 *  TR SEND Peer Messages
 *
 *  =========================================================================
 */

static inline mblk_t *
tr_enc_uni(mblk_t *dp)
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
static inline mblk_t *
tr_enc_qry_invoke(mblk_t *dp)
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
static inline mblk_t *
tr_enc_qwp_invoke(mblk_t *dp)
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
static inline mblk_t *
tr_enc_res_reply(mblk_t *dp)
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
static inline mblk_t *
tr_enc_cnv_invoke(mblk_t *dp)
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
static inline mblk_t *
tr_enc_cwp_invoke(mblk_t *dp)
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
static inline mblk_t *
tr_enc_cnv_reply(mblk_t *dp)
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
static inline mblk_t *
tr_enc_cwp_reply(mblk_t *dp)
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
static inline mblk_t *
tr_enc_abort_reject(mblk_t *dp)
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
 *  TR OUTPUT EVENT Translator
 *
 *  =========================================================================
 */
static int (*tr_o_events[]) (queue_t *, mblk_t *) = {
	NULL,			/* TRE_NONE 0x00 - (reserved) */
	    tr_o_uni_inv,	/* TRE_UNI_INV 0x01 - Unidirectional Invoke */
	    tr_o_qry_inv,	/* TRE_QRY_INV 0x02 - Query w/o perm (Invoke) */
	    tr_o_qwp_inv,	/* TRE_QWP_INV 0x03 - Query w/ perm (Invoke) */
	    tr_o_rsp_rep,	/* TRE_RSP_REP 0x04 - Response (Reply) */
	    tr_o_cnv_inv,	/* TRE_CNV_INV 0x05 - Conv w/o perm (Invoke) */
	    tr_o_cwp_inv,	/* TRE_CWP_INV 0x06 - Conv w perm (Invoke) */
	    tr_o_cnv_rep,	/* TRE_CNV_REP 0x07 - Conv w/o perm (Reply) */
	    tr_o_cwp_rep,	/* TRE_CWP_REP 0x08 - Conv w perm (Reply) */
	    tr_o_abt_rej	/* TRE_ABT_REJ 0x09 - Abort (Reject) */
};
static inline int
tr_o_proto(queue_t *q, mblk_t *mp)
{
	tr_event_t *m = (tr_event_t *) mp->b_wptr;

	return (*tr_o_events[m->event]) (q, mp);
}

/*
 *  =========================================================================
 *
 *  TR INPUT EVENT State Machines
 *
 *  =========================================================================
 *  These functions take an input event at the TR stream and run the state
 *  machine associated with the TR stream.
 */
/*
 *  TRE_UNI_INV	    0x01 - Unidirectional (Invoke)	
 *  ----------------------------------------------------
 */
static int
tr_i_uni_inv(queue_t *q, mblk_t *mp)
{
	tr_t *tr = (tr_t *) q->q_ptr;
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TRE_QRY_INV	    0x02 - Query w/o perm (Invoke)	
 *  ----------------------------------------------------
 *  ANSI: TID
 *
 *  Open a new transaction id and assign it to the stream (or provide it to
 *  the TR-User on single-threaded streams).  Mark without permission and
 *  reject TR-User attempts at conversation.
 */
static int
tr_i_qry_inv(queue_t *q, mblk_t *mp)
{
	tr_t *tr = (tr_t *) q->q_ptr;
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TRE_QWP_INV	    0x03 - Query w/ perm (Invoke)	
 *  ----------------------------------------------------
 *  ANSI: TID
 *
 *  Open a new transaction id and assign it to the stream (or provide it to
 *  the TR-User on single-threaded streams).  Mark with permission and permit
 *  TR-User attempts at conversation.
 */
static int
tr_i_qwp_inv(queue_t *q, mblk_t *mp)
{
	tr_t *tr = (tr_t *) q->q_ptr;
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TRE_RSP_REP	    0x04 - Response (Reply)		
 *  ----------------------------------------------------
 *  ANSI: TID
 *
 *  Close a transaction.  Pass response to user (with transaction id on
 *  multiple-threaded streams).
 */
static int
tr_i_rsp_rep(queue_t *q, mblk_t *mp)
{
	tr_t *tr = (tr_t *) q->q_ptr;
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TRE_CNV_INV	    0x05 - Conv w/o perm (Invoke)	
 *  ----------------------------------------------------
 *  ANSI: Originating TID, Responding TID
 *
 *  Continue a transaction.  Assign a correlation id to this conversation.
 *  Pass conversation to user (with transaction id on multiple-threaded
 *  streams.)  Mark conversation without permission and reject additional
 *  conversational attempts from TR-User on this transaction.
 *
 *  If conversation is not permitted, reject the invoke and abort the
 *  transaction.  Pass an abort to the user.
 */
static int
tr_i_cnv_inv(queue_t *q, mblk_t *mp)
{
	tr_t *tr = (tr_t *) q->q_ptr;
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TRE_CWP_INV	    0x06 - Conv w perm (Invoke)		
 *  ----------------------------------------------------
 *  ANSI: Originating TID, Responding TID
 *
 *  Continue a transaction.  Assign a correlation to this conversation.
 *  Pass conversation to user (with correlation id and transaction id on
 *  multiple-threaded streams.)  Mark conversation with permission and permit
 *  additional convesration attempts from TR-user on this conversation.
 *
 *  If conversation is not permitted, reject the invoke and abort the
 *  transaction.  Pass an abort to the user.
 */
static int
tr_i_cwp_inv(queue_t *q, mblk_t *mp)
{
	tr_t *tr = (tr_t *) q->q_ptr;
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TRE_CNV_REP	    0x07 - Conv w/o perm (Reply)	
 *  ----------------------------------------------------
 *  ANSI: Originating TID, Responding TID
 *
 *  Close the conversation and deallocate the correlation id.
 */
static int
tr_i_cnv_rep(queue_t *q, mblk_t *mp)
{
	tr_t *tr = (tr_t *) q->q_ptr;
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TRE_CWP_REP	    0x08 - Conv w perm (Reply)		
 *  ----------------------------------------------------
 *  ANSI: Originating TID, Responding TID
 *
 *  Close the conversation and deallocate the correlation id.
 */
static int
tr_i_cwp_rep(queue_t *q, mblk_t *mp)
{
	tr_t *tr = (tr_t *) q->q_ptr;
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

/*
 *  TRE_ABT_REJ	    0x09 - Abort (Reject)		
 *  ----------------------------------------------------
 *  ANSI: TID
 *
 *  Abort the conversation and the transaction.  Deallocate transaction id and
 *  all correlation ids.
 */
static int
tr_i_abt_rej(queue_t *q, mblk_t *mp)
{
	tr_t *tr = (tr_t *) q->q_ptr;
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	/* 
	 *  FIXME: process event...
	 */
	return (0);
}

static int (*tr_i_events[]) (queue_t *, mblk_t *) = {
	NULL,			/* TRE_NONE 0x00 - (reserved) */
	    tr_i_uni_inv,	/* TRE_UNI_INV 0x01 - Unidirectional Invoke */
	    tr_i_qry_inv,	/* TRE_QRY_INV 0x02 - Query w/o perm (Invoke) */
	    tr_i_qwp_inv,	/* TRE_QWP_INV 0x03 - Query w/ perm (Invoke) */
	    tr_i_rsp_rep,	/* TRE_RSP_REP 0x04 - Response (Reply) */
	    tr_i_cnv_inv,	/* TRE_CNV_INV 0x05 - Conv w/o perm (Invoke) */
	    tr_i_cwp_inv,	/* TRE_CWP_INV 0x06 - Conv w perm (Invoke) */
	    tr_i_cnv_rep,	/* TRE_CNV_REP 0x07 - Conv w/o perm (Reply) */
	    tr_i_cwp_rep,	/* TRE_CWP_REP 0x08 - Conv w perm (Reply) */
	    tr_i_abt_rej	/* TRE_ABT_REJ 0x09 - Abort (Reject) */
};
static inline int
tr_i_proto(queue_t *q, mblk_t *mp)
{
	tr_event_t *e = (tr_event_t *) mp->b_rptr;

	return (*tr_i_events[e->event]) (q, mp);
}

/*
 *  =========================================================================
 *
 *  TR RECV Peer Messages
 *
 *  =========================================================================
 */

static inline int
sccp_dec_uni(caddr_t p, caddr_t e, tr_event_t * e)
{
	return (0);
}
static inline int
sccp_dec_qry(caddr_t p, caddr_t e, tr_event_t * e)
{
	return (0);
}
static inline int
sccp_dec_qwp(caddr_t p, caddr_t e, tr_event_t * e)
{
	return (0);
}
static inline int
sccp_dec_cnv(caddr_t p, caddr_t e, tr_event_t * e)
{
	return (0);
}
static inline int
sccp_dec_cwp(caddr_t p, caddr_t e, tr_event_t * e)
{
	return (0);
}
static inline int
sccp_dec_res(caddr_t p, caddr_t e, tr_event_t * e)
{
	return (0);
}
static inline int
sccp_dec_abt(caddr_t p, caddr_t e, tr_event_t * e)
{
	return (0);
}

/*
 *  =========================================================================
 *
 *  SCCP --> TR (Upstream Primitives received from downstream)
 *
 *  =========================================================================
 */
/*
 *  N_INFO_ACK	    16 - Information Acknowledgement
 *  ----------------------------------------------------------------
 */
static int
sccp_info_ack(queue_t *q, mblk_t *pdu)
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
static int
sccp_bind_ack(queue_t *q, mblk_t *pdu)
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
static int
sccp_error_ack(queue_t *q, mblk_t *pdu)
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
static int
sccp_ok_ack(queue_t *q, mblk_t *mp)
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
static int
sccp_unitdata_ind(queue_t *q, mblk_t *pdu)
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
	return tr_recv_msg(q, pdu);
}

/*
 *  N_UDERROR_IND   21 - UNITDATA Error Indication
 *  ----------------------------------------------------------------
 */
static int
sccp_uderror_ind(queue_t *q, mblk_t *pdu)
{
	tr_t *tr;
	int ecode;
	sccp_t *sccp = (sccp_t *) q->q_ptr;
	N_uderror_ind_t *p = (N_uderror_ind_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return m_error_all(q, pdu, EFAULT, EFAULT);
	if (p->DEST_length < sizeof(sccp_addr_t))
		return m_error_all(q, pdu, EFAULT, EFAULT);
	/* 
	 *  TODO: have to propagate error to TR-Users.  Most of these turn
	 *  into aborts (TR_ABORT_IND).  We still have to generate a
	 *  TRE_ABT_RES event.
	 */
	for (tr = sccp->tr; tr; tr = tr->sccp_next) {
		if (tr->outcnt > 0) {
			/* 
			 *  Send TRE_ABT_RES event to tr user....
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

static int
tr_r_data(queue_t *q, mblk_t *mp)
{
	/* 
	 *  This is an SCCP unitdata message from below.  We don't get SCCP
	 *  unitdata messages as M_DATA, so if this happens we ignore it.
	 */
	freemsg(mp);
	return (0);
}
static int tr_r_proto(queue_t *q, mblk_t *mp);

{
}

/*
 *  =========================================================================
 *
 *  TR-User --> TR (Downstream Primitives received from upstream)
 *
 *  =========================================================================
 */
/*
 *  TR Options parsing.
 */
static int
parse_options(queue_t *q, mblk_t *pdu, int prim, caddr_t opt_ptr, size_t opt_len)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	if (opt_len && opt_len < sizeof(uint32_t))
		return tr_error_ack_reply(q, pdu, prim, TRBADOPT);
	/* 
	 *  Decode the options....
	 */
	return (0);
}

/*
 *  TR_INFO_REQ           0 - Information request                     
 *  ---------------------------------------------------------------
 */
static int
user_info_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_info_req_t *p = (TR_info_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_INFO_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	freemsg(pdu);
	return (0);
}

/*
 *  TR_BIND_REQ           1 - Bind to network address                 
 *  ---------------------------------------------------------------
 */
static int
user_bind_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_bind_req_t *p = (TR_bind_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_UNBIND_REQ         2 - Unbind from network address             
 *  ---------------------------------------------------------------
 */
static int
user_unbind_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_unbind_req_t *p = (TR_unbind_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_UNIDIR_REQ         3 - Unidirectional request                  
 *  ---------------------------------------------------------------
 */
static int
user_unidir_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_unidir_req_t *p = (TR_unidir_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_BEGIN_REQ          4 - Begin transaction request               
 *  ---------------------------------------------------------------
 */
static int
user_begin_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_begin_req_t *p = (TR_begin_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_QUERY_W_PERM_REQ   5 - Begin transaction request               
 *  ---------------------------------------------------------------
 */
static int
user_query_w_perm_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_query_w_perm_req_t *p = (TR_query_w_perm_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_QUERY_WO_PERM_REQ  6 - Begin transaction request               
 *  ---------------------------------------------------------------
 */
static int
user_query_wo_perm_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_query_wo_perm_req_t *p = (TR_query_wo_perm_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_END_REQ            7 - End transaction request                 
 *  ---------------------------------------------------------------
 */
static int
user_end_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_end_req_t *p = (TR_end_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_RESP_REQ           8 - End transaction request                 
 *  ---------------------------------------------------------------
 */
static int
user_resp_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_resp_req_t *p = (TR_resp_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_CONTINUE_REQ       9 - Continue transaction request            
 *  ---------------------------------------------------------------
 */
static int
user_continue_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_continue_req_t *p = (TR_continue_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_CONTINUE_RES      10 - Continue transaction response           
 *  ---------------------------------------------------------------
 */
static int
user_continue_res(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_continue_res_t *p = (TR_continue_res_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_CONV_W_PERM_REQ   11 - Continue transaction request            
 *  ---------------------------------------------------------------
 */
static int
user_conv_w_perm_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_conv_w_perm_req_t *p = (TR_conv_w_perm_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_CONV_WO_PERM_REQ  12 - Continue transaction request            
 *  ---------------------------------------------------------------
 */
static int
user_conv_wo_perm_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_conv_wo_perm_req_t *p = (TR_conv_wo_perm_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_CONV_W_PERM_RES   13 - Continue transaction response           
 *  ---------------------------------------------------------------
 */
static int
user_conv_w_perm_res(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_conv_w_perm_res_t *p = (TR_conv_w_perm_res_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_CONV_WO_PERM_RES  14 - Continue transaction response           
 *  ---------------------------------------------------------------
 */
static int
user_conv_wo_perm_res(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_conv_wo_perm_res_t *p = (TR_conv_wo_perm_res_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

/*
 *  TR_ABORT_REQ         15 - Abort transaction request               
 *  ---------------------------------------------------------------
 */
static int
user_abort_req(queue_t *q, mblk_t *pdu)
{
	int err;
	tr_t *tr = (tr_t *) q->q_ptr;
	TR_abort_req_t *p = (TR_abort_req_t *) pdu->b_rptr;

	if (pdu->b_wptr - pdu->b_rptr < sizeof(*p))
		return tr_error_ack_reply(q, TR_BIND_REQ, -EMSGSIZE);
	/* 
	 *  TODO: process primitive...
	 */
	tr->state = NS_IDLE;
	freemsg(pdu);
	return (0);
}

int (*user_dprim[]) (queue_t *, mblk_t *) = {
#define USER_DSTR_FIRST	TR_INFO_REQ
	tr_info_req,		/* TR_INFO_REQ 0 - Information request */
	    tr_bind_req,	/* TR_BIND_REQ 1 - Bind to network address */
	    tr_unbind_req,	/* TR_UNBIND_REQ 2 - Unbind from network address */
	    tr_unidir_req,	/* TR_UNIDIR_REQ 3 - Unidirectional request */
	    tr_begin_req,	/* TR_BEGIN_REQ 4 - Begin transaction request */
	    tr_qwp_req,		/* TR_QUERY_W_PERM_REQ 5 - Begin transaction request */
	    tr_qwop_req,	/* TR_QUERY_WO_PERM_REQ 6 - Begin transaction request */
	    tr_end_req,		/* TR_END_REQ 7 - End transaction request */
	    tr_resp_req,	/* TR_RESP_REQ 8 - End transaction request */
	    tr_cont_req,	/* TR_CONTINUE_REQ 9 - Continue transaction request */
	    tr_cont_res,	/* TR_CONTINUE_RES 10 - Continue transaction response */
	    tr_cwp_req,		/* TR_CONV_W_PERM_REQ 11 - Continue transaction request */
	    tr_cwop_req,	/* TR_CONV_WO_PERM_REQ 12 - Continue transaction request */
	    tr_cwp_res,		/* TR_CONV_W_PERM_RES 13 - Continue transaction response */
	    tr_cwop_res,	/* TR_CONV_WO_PERM_RES 14 - Continue transaction response */
	    tr_abort_req	/* TR_ABORT_REQ 15 - Abort transaction request */
#define USER_DSTR_LAST  TR_ABORT_REQ
};

static int
tr_w_data(queue_t *q, mblk_t *pdu)
{
	tr_t *tr = (tr_t *) q->q_ptr;

	if (tr->state != TRS_XACT_OPEN) {
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
	 *  This is an TR data message from above.  This is only valid in
	 *  connection-oriented states.
	 */
	/* 
	 *  TODO: pass data message to TR.
	 */
	return (0);
}
static int
tr_w_proto(queue_t *q, mblk_t *mp)
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
 *  We have no defined module to module controls for either TR or SCCP.
 */
static int
tr_r_ctl(queue_t *q, mblk_t *pdu)
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
static int
tr_w_ctl(queue_t *q, mblk_t *pdu)
{
	/* 
	 *  FIXME: Remove this TR-User from the SCCP-Provider...
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
 *  Provider to every TR-User upper stream which is using the SCCP provider.
 *  We also set states here to indicate the error.  This is because if we run
 *  out of mblks to propagate the M_ERROR, we will discard the priority
 *  message (M_ERROR) if we are in the read service routine.  Setting the
 *  error ensures that we will respond with an M_ERROR later when resources
 *  are available.
 */
static int
tr_r_error(queue_t *q, mblk_t *pdu)
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
 *  Provider to every TR-User upper stream which is using the SCCP provider.
 *  We also set states here to indicate the error.  This is because if we run
 *  out of mblks to propagate the M_HANGUP, we will discard the priority
 *  message (M_HANGUP) if we are in the read service routine.  Setting the
 *  error ensures that we will respond with an M_HANGUP later when resources
 *  are available.
 */
static int
tr_r_hangup(queue_t *q, mblk_t *pdu)
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
static int
tr_r_flush(queue_t *q, mblk_t *pdu)
{
	int err;

	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		/* flush all TR-Users upwards */
		m_flush_all(q, NULL, mp->b_rptr[0], mp->b_rptr[1]);
		*mp->b_rptr &= ~FLUSHR;
	}
	if (!(mp->b_flag & MSGNOLOOP)) {
		if (*mp->b_rptr & FLUSHW) {
			if (*mp->b_rptr & FLUSHBAND)
				flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
			else
				flushq(WR(q), FLUSHDATA);
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);	/* flush all the way back down */
			return (0);
		}
	}
	freemsg(mp);
	return (0);
}

/*
 *  M_FLUSH from above (i.e. from TR User)
 *  -------------------------------------------------------------------------
 *  We want to take flushes from above and turn them back around back up to
 *  the TR User.
 */
static int
tr_w_flush(queue_t *q, mblk_t *pdu)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		*mp->b_rptr &= ~FLUSHW;
	}
	if (!(mp->b_flag & MSGNOLOOP)) {
		if (*mp->b_rptr & FLUSHR) {
			if (*mp->b_rptr & FLUSHBAND)
				flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
			else
				flushq(RD(q), FLUSHDATA);
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
static int
tr_i_flush(queue_t *q, mblk_t *pdu)
{
	if (*mp->b_rptr & FLUSHR) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
}

/*
 *  M_FLUSH internal from above.
 *  -------------------------------------------------------------------------
 */
static int
tr_o_flush(queue_t *q, mblk_t *pdu)
{
	if (*mp->b_rptr & FLUSHW) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
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
 *  SCCP Provider streams can be linked under the TR multiplexing driver.  We
 *  permit normal TR streams to perform an I_LINK of SCCP Provider streams
 *  under the SCCP driver, but we only permit the TR control stream to
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
static inline int
tr_i_link(queue_t *q, mblk_t *mp, struct linkblk *lp)
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
 *  When we unlink, if we still have referencing upper TR-User streams, we
 *  send each of the a M_HANGUP message indicating the loss of the connection.
 *  This might result in a SIG_PIPE signal being sent to the process if the
 *  TR-User is a stream head.
 */
static inline int
tr_i_unlink(queue_t *q, struct linkblk *lp)
{
	int err;
	sccp_t *sccp;
	uint muxid;

	ensure(lp, return (-EFAULT));
	muxid = lp->l_index;
	for (sccp = sccp_links; sccp && sccp->muxid != muxid; sccp = sccp->next) ;
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
static int
tr_w_ioctl(queue_t *q, mblk_t *pdu)
{
	int ret = -EINVAL;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	struct iocblk *iocp = (struct iocblk *) mp->b_wptr;
	int cmd = iocp->ioc_cmd;

	switch (_IOC_TYPE(cmd)) {
	case __SID:
		switch (cmd) {
		case I_PLINK:
			if (RD(q) != tr_lmq) {
				ret = -EPERM;
				break;
			}
		case I_LINK:
			ret = tr_i_link(q, arg);
			break;
		case I_PUNLINK:
			if (RD(q) != tr_lmq) {
				ret = -EPERM;
				break;
			}
		case I_UNLINK:
			ret = tr_i_unlink(q, arg);
			break;
		}
		break;
	case TR_IOC_MAGIC:
		if (iocp->ioc_count >= _IOC_SIZE(cmd)) {
			int nr = _IOC_NR(cmd);

			ret = -EOPNOTSUPP;
			if (0 <= nr && nr < sizeof(tr_ioctl) / sizeof(int (*)(void))
			    && tr_ioctl[nr])
				ret = (*tr_ioctl[nr]) (q, cmd, arg);
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
static int tr_u_wput(queue_t *, mblk_t *);
static int tr_u_wsrv(queue_t *);

static int tr_l_wsrv(queue_t *);
static int tr_l_rput(queue_t *, mblk_t *);

static int tr_u_rsrv(queue_t *);
static int tr_l_rsrv(queue_t *);

static inline int
tr_recover(queue_t *q, mblk_t *mp, int err)
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
 *  READ PUT and SERVICE (Message from below SCCP-Provider --> TR)
 *  -------------------------------------------------------------------------
 */
static inline int
tr_rd(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return tr_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tr_r_proto(q, mp);
	case M_CTL:
		return tr_r_ctl(q, mp);
	case M_ERROR:
		return tr_r_error(q, mp);
	case M_FLUSH:
		return tr_r_flush(q, mp);
	case M_HANGUP:
		return tr_r_hangup(q, mp);
	}
	return (-EOPNOTSUPP);
}
static int
tr_l_rput(queue_t *q, mblk_t *mp)
{
	int err;

	if (mp->b_datap->db_type < QPCTL && (q->q_count || !canputnext(q))) {
		putq(q, mp);
		return (0);
	}
	if ((err = tr_rd(q, mp)))
		return (tr_recover(q, mp, err));
	return (0);
}
static int
tr_l_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		int err;

		if (!(err = tr_rd(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (tr_recover(q, mp, err));
		freemsg(mp);
		return (err);
	}
	return (0);
}

/*
 *  WRITE PUT and SERVICE (Message from above TR-User --> TR)
 *  -------------------------------------------------------------------------
 */
static inline int
tr_wr(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db - type) {
	case M_DATA:
		return tr_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tr_w_proto(q, mp);
	case M_CTL:
		return tr_w_ctl(q, mp);
	case M_FLUSH:
		return tr_w_flush(q, mp);
	case M_IOCTL:
		return ttcioctl(q, mp);
	}
	return (-EOPNOTSUPP);
}
static int
tr_u_wput(queue_t *q, mblk_t *mp)
{
	int err;

	if (mp->b_datap->db_type < QPCTL && (q->q_count || !canputnext(q))) {
		putq(q, mp);
		return (0);
	}
	if ((err = tr_wr(q, mp)))
		return (tr_recover(q, mp, err));
	return (0);
}
static int
tr_u_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		int err;

		if (!(err = tr_wr(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (tr_recover(q, mp, err));
		freemsg(mp);
		return (err);
	}
	return (0);
}

/*
 *  READ SERVICE (Event from below as input to TR State Machines)
 *  -------------------------------------------------------------------------
 */
static inline int
tr_input(queue_t *q, mblk_t *mp)
{
	uint type = mp->b_datap->db_type;

	if (M_DATA <= type && type <= M_STOPI && tr_input_ops[type])
		return (*tr_input_ops[type]) (q, mp);
	putnext(q, mp);
	return (0);
}
static inline int
tr_input(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return (-EOPNOTSUPP);
	case M_PROTO:
	case M_PCPROTO:
		return tr_i_proto(q, mp);
	case M_FLUSH:
		return tr_i_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}
static int
tr_u_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		int err;

		if (!(err = tr_input(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (tr_recover(q, mp, err));
		freemsg(mp);
		return (err);
	}
}

/*
 *  WRITE SERVICE (Event from above as output from TR State Machines)
 *  -------------------------------------------------------------------------
 */
static inline int
tr_output(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return (-EOPNOTSUPP);
	case M_PROTO:
	case M_PCPROTO:
		return tr_o_proto(q, mp);
	case M_FLUSH:
		return tr_o_flush(q, mp);
	}
	putnext(q, mp);
	return (0);
}
static int
tr_l_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		int err;

		if (!(err = tr_output(q, mp)))
			continue;
		if (mp->b_datap->db_type < QPCTL)
			return (tr_recover(q, mp, err));
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
static int tr_open(queue_t *, dev_t *, int, int, cred_t *);
static int tr_close(queue_t *, int, cred_t *);

static int tr_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp);

{
	tr_t *tr, **trp = &tr_opens;
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);

	if (q->q_ptr != NULL)
		return (0);	/* already open */

	if (sflag == MODOPEN || WR(q)->q_next)
		return (EIO);	/* cannot push as module */
	if (!cminor && !tr_lmq) {
		if (crp->cr_ruid != 0)
			return (EPERM);	/* need r00t */
		tr_lmq = q;
	} else if (!cminor || sflag == CLONEOPEN) {
		cminor = 1;
	}
	for (; cminor <= TR_NMINOR && *trp; trp = &(*trp)->next) {
		int dminor = getminor((*trp)->devid);

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
	if (cminor > TR_NMINOR)
		return (ENXIO);
	*devp = makedevice(cmajor, cminor);
	if (!(tr = kmalloc(sizeof(*tr), GFP_KERNEL)))
		return (ENOMEM);
	bzero(tr, sizeof(*tr));
	q->q_ptr = WR(q)->q_ptr = tr;
	tr->q = q;
	tr->devid = *devp;
	tr->state = 0;
	if ((tr->next = *trp))
		tr->next->prev = &tr->next;
	tr->prev = trp;
	*trp = tr;
	return (0);
}
static int
tr_close(queue_t *q, int flag, cred_t *crp)
{
	tr_t *tr = (tr_t *) q->q_ptr;

	if (!tr)
		return (EIO);
	if (q == tr_lmq)
		tr_lmq = NULL;
	if (tr->sccp) {
		/* 
		 *  Detach this TR stream from whatever SCCP streams it is
		 *  using on the lower half.
		 */
		sccp_t *sccp = tr->sccp;

		if ((--tr->sccp->use_count) <= 0) {
			/* 
			 *  Unbind the SCCP stream if it is not being used by
			 *  any TR streams.
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
		if ((*(tr->sccp_prev) = tr->sccp_next))
			tr->sccp_next->sccp_prev = tr->sccp_prev;
	}
	/* 
	 *  TODO:  Might want to do some more things to deallocate some TR
	 *  structures associated with the module.  I.e. some TR bindings.
	 */
	if ((*(tr->prev) = tr->next))
		tr->next->prev = tr->prev;
	WR(q)->q_ptr = RD(q)->q_ptr = NULL;
	kfree(tr);
	return (0);
}

/*
 *  =========================================================================
 *
 *  LIS STREAMS INITIALIZATION
 *
 *  =========================================================================
 */
static int tr_initialized = 0;

#ifndef LIS_REGISTERED
static inline int
tr_init(void)
#else
__initfunc(int tr_init(void))
#endif
{
	int err;

	if (tr_initialized)
		return (0);
	tr_initialized = 1;
	cmn_err(CE_NOTE, TR_BANNER);	/* console splash */
#ifndef LIS_REGISTERED
	if ((err = lis_register_strmod(&tr_info, tr_minfo.ni_name)) < 0)
		cmn_err(CE_WARN "tr: couldn't register module!\n");
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0)
		lis_unregister_strmod(&tr_info);
	return (err);
#endif
}

#ifndef LIS_REGISTERED
static inline void
tr_terminate(void)
#else
__initfunc(void tr_terminate(void))
#endif
{
	if (!tr_initialized)
		return;
	tr_initialized = 0;
#ifndef LIS_REGISTERED
	if (lis_unregister_strmod(&tr_info))
		cmn_err(CE_WARN "tr: couldn't unregister module!\n");
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
int
init_module(void)
{
	return tr_init();
}

void
cleanup_module(void)
{
	tr_terminate();
}
#endif
