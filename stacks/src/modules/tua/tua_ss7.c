/*****************************************************************************

 @(#) $RCSfile: tua_ss7.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:15:02 $

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

 Last Modified $Date: 2004/08/21 10:15:02 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tua_ss7.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:15:02 $"

static char const ident[] =
    "$RCSfile: tua_ss7.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/08/21 10:15:02 $";

/*
 *  =========================================================================
 *
 *  TCAP-User --> TUA (ASP) Upstream Primitives
 *
 *  =========================================================================
 */

/*
 *  TC_INFO_REQ		 0
 */
static int ss7_info_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_BIND_REQ		 1
 */
static int ss7_bind_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_UNBIND_REQ	 2
 */
static int ss7_unbind_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_SUBS_BIND_REQ	 3
 */
static int ss7_subs_bind_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_SUBS_UNBIND_REQ	 4
 */
static int ss7_subs_unbind_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_OPTMGMT_REQ	 5
 */
static int ss7_optmgmt_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_UNI_REQ		 6
 */
static int ss7_uni_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_BEGIN_REQ	 7
 */
static int ss7_begin_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_BEGIN_RES	 8
 */
static int ss7_begin_res(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_CONT_REQ		 9
 */
static int ss7_cont_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_END_REQ		10
 */
static int ss7_end_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_ABORT_REQ	11
 */
static int ss7_abort_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_INVOKE_REQ	26
 */
static int ss7_invoke_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_RESULT_REQ	27
 */
static int ss7_result_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_ERROR_REQ	28
 */
static int ss7_error_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_CANCEL_REQ	29
 */
static int ss7_cancel_req(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_REJECT_REQ	30
 */
static int ss7_reject_req(queue_t * q, mblk_t * pdu)
{
}

static int (*ss7_dstr_prim[]) (queue_t *, mblk_t *) = {
#define TCAP_DSTR_FIRST		   TC_INFO_REQ
	ss7_info_req,		/* TC_INFO_REQ 0 */
	    ss7_bind_req,	/* TC_BIND_REQ 1 */
	    ss7_unbind_req,	/* TC_UNBIND_REQ 2 */
	    ss7_subs_bind_req,	/* TC_SUBS_BIND_REQ 3 */
	    ss7_subs_unbind_req,	/* TC_SUBS_UNBIND_REQ 4 */
	    ss7_optmgmt_req,	/* TC_OPTMGMT_REQ 5 */
	    ss7_uni_req,	/* TC_UNI_REQ 6 */
	    ss7_begin_req,	/* TC_BEGIN_REQ 7 */
	    ss7_begin_res,	/* TC_BEGIN_RES 8 */
	    ss7_cont_req,	/* TC_CONT_REQ 9 */
	    ss7_end_req,	/* TC_END_REQ 10 */
	    ss7_abort_req,	/* TC_ABORT_REQ 11 */
	    NULL,		/* TC_INFO_ACK 12 */
	    NULL,		/* TC_BIND_ACK 13 */
	    NULL,		/* TC_SUBS_BIND_ACK 14 */
	    NULL,		/* TC_OK_ACK 15 */
	    NULL,		/* TC_ERROR_ACK 16 */
	    NULL,		/* TC_OPTMGMT_ACK 17 */
	    NULL,		/* TC_UNI_IND 18 */
	    NULL,		/* TC_BEGIN_IND 19 */
	    NULL,		/* TC_BEGIN_CON 20 */
	    NULL,		/* TC_CONT_IND 21 */
	    NULL,		/* TC_END_IND 22 */
	    NULL,		/* TC_ABORT_IND 23 */
	    NULL,		/* TC_NOTICE_IND 24 */
	    NULL,		/* (not used) 25 */
	    ss7_invoke_req,	/* TC_INVOKE_REQ 26 */
	    ss7_result_req,	/* TC_RESULT_REQ 27 */
	    ss7_error_req,	/* TC_ERROR_REQ 28 */
	    ss7_cancel_req,	/* TC_CANCEL_REQ 29 */
	    ss7_reject_req,	/* TC_REJECT_REQ 30 */
#define TCAP_USTR_LAST 		   TC_REJECT_REQ
	    NULL,		/* (not used) 31 */
	    NULL,		/* TC_INVOKE_IND 32 */
	    NULL,		/* TC_RESULT_IND 33 */
	    NULL,		/* TC_ERROR_IND 34 */
	    NULL,		/* TC_CANCEL_IND 35 */
	    NULL		/* TC_REJECT_IND 36 */
};

static int ss7_w_proto(queue_t * q, mblk_t * pdu)
{
	int prim = *((long *) mp->b_rptr);
	if (TCAP_DSTR_FIRST <= prim && prim <= TCAP_DSTR_LAST && ss7_dstr_prim[prim])
		return ((*ss7_dstr_prim[prim]) (q, mp));
	return (-EOPNOTSUPP);
}
static int ss7_w_flush(queue_t * q, mblk_t * pdu)
{
	return ua_w_flush(q, pdu);
}

static int (*ss7_w_ops[]) (queue_t *, mblk_t *) = {
	ss7_w_data,		/* M_DATA */
	    ss7_w_proto,	/* M_PROTO */
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
	    ss7_w_flush,	/* M_FLUSH */
	    NULL,		/* M_HANGUP */
	    NULL,		/* M_IOCACK */
	    NULL,		/* M_IOCNAK */
	    NULL,		/* M_IOCDATA */
	    ss7_w_proto,	/* M_PCPROTO */
	    NULL,		/* M_PCRSE */
	    NULL,		/* M_PCSIG */
	    NULL,		/* M_READ */
	    NULL,		/* M_STOP */
	    NULL,		/* M_START */
	    NULL,		/* M_STARTI */
	    NULL		/* M_STOPI */
};

static void ss7_u_create(lp_t * lp)
{
}
static void ss7_u_delete(lp_t * lp)
{
}

struct ops tua_ss7_u_ops = {
	&ss7_u_create,			/* create priv struct */
	&ss7_u_delete,			/* delete priv struct */
	NULL,				/* read operations */
	&ss7_w_ops			/* write operations */
};

/*
 *  =========================================================================
 *
 *  TCAP-Provider --> TUA (SGP) Upstream Primitives
 *
 *  =========================================================================
 */

/*
 *  TC_INFO_ACK		12
 */
static int ss7_info_ack(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_BIND_ACK		13
 */
static int ss7_bind_ack(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_SUBS_BIND_ACK	14
 */
static int ss7_subs_bind_ack(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_OK_ACK		15
 */
static int ss7_ok_ack(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_ERROR_ACK	16
 */
static int ss7_error_ack(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_OPTMGMT_ACK	17
 */
static int ss7_optmgmt_ack(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_UNI_IND		18
 */
static int ss7_uni_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_BEGIN_IND	19
 */
static int ss7_begin_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_BEGIN_CON	20
 */
static int ss7_begin_con(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_CONT_IND		21
 */
static int ss7_cont_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_END_IND		22
 */
static int ss7_end_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_ABORT_IND	23
 */
static int ss7_abort_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_NOTICE_IND	24
 */
static int ss7_notice_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_INVOKE_IND	32
 */
static int ss7_invoke_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_RESULT_IND	33
 */
static int ss7_result_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_ERROR_IND	34
 */
static int ss7_error_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_CANCEL_IND	35
 */
static int ss7_cancel_ind(queue_t * q, mblk_t * pdu)
{
}

/*
 *  TC_REJECT_IND	36
 */
static int ss7_reject_ind(queue_t * q, mblk_t * pdu)
{
}

static int (*ss7_ustr_prim[]) (queue_t *, mblk_t *) = {
	NULL,			/* TC_INFO_REQ 0 */
	    NULL,		/* TC_BIND_REQ 1 */
	    NULL,		/* TC_UNBIND_REQ 2 */
	    NULL,		/* TC_SUBS_BIND_REQ 3 */
	    NULL,		/* TC_SUBS_UNBIND_REQ 4 */
	    NULL,		/* TC_OPTMGMT_REQ 5 */
	    NULL,		/* TC_UNI_REQ 6 */
	    NULL,		/* TC_BEGIN_REQ 7 */
	    NULL,		/* TC_BEGIN_RES 8 */
	    NULL,		/* TC_CONT_REQ 9 */
	    NULL,		/* TC_END_REQ 10 */
	    NULL,		/* TC_ABORT_REQ 11 */
#define TCAP_USTR_FIRST		   TC_INFO_ACK
	    ss7_info_ack,	/* TC_INFO_ACK 12 */
	    ss7_bind_ack,	/* TC_BIND_ACK 13 */
	    ss7_subs_bind_ack,	/* TC_SUBS_BIND_ACK 14 */
	    ss7_ok_ack,		/* TC_OK_ACK 15 */
	    ss7_error_ack,	/* TC_ERROR_ACK 16 */
	    ss7_optmgmt_ack,	/* TC_OPTMGMT_ACK 17 */
	    ss7_uni_ind,	/* TC_UNI_IND 18 */
	    ss7_begin_ind,	/* TC_BEGIN_IND 19 */
	    ss7_begin_con,	/* TC_BEGIN_CON 20 */
	    ss7_cont_ind,	/* TC_CONT_IND 21 */
	    ss7_end_ind,	/* TC_END_IND 22 */
	    ss7_abort_ind,	/* TC_ABORT_IND 23 */
	    ss7_notice_ind,	/* TC_NOTICE_IND 24 */
	    NULL,		/* (not used) 25 */
	    NULL,		/* TC_INVOKE_REQ 26 */
	    NULL,		/* TC_RESULT_REQ 27 */
	    NULL,		/* TC_ERROR_REQ 28 */
	    NULL,		/* TC_CANCEL_REQ 29 */
	    NULL,		/* TC_REJECT_REQ 30 */
	    NULL,		/* (not used) 31 */
	    ss7_invoke_ind,	/* TC_INVOKE_IND 32 */
	    ss7_result_ind,	/* TC_RESULT_IND 33 */
	    ss7_error_ind,	/* TC_ERROR_IND 34 */
	    ss7_cancel_ind,	/* TC_CANCEL_IND 35 */
	    ss7_reject_ind	/* TC_REJECT_IND 36 */
#define TCAP_USTR_LAST 		   TC_REJECT_IND
};

static int ss7_r_proto(queue_t * q, mblk_t * pdu)
{
	int prim = *((long *) mp->b_rptr);
	if (TCAP_USTR_FIRST <= prim && prim <= TCAP_USTR_LAST && ss7_ustr_prim[prim])
		return ((*ss7_ustr_prim[prim]) (q, mp));
	return (-EOPNOTSUPP);
}
static int ss7_r_error(queue_t * q, mblk_t * pdu)
{
}
static int ss7_r_hangup(queue_t * q, mblk_t * pdu)
{
}
static int ss7_r_flush(queue_t * q, mblk_t * pdu)
{
	return ua_r_flush(q, pdu);
}

static int (*ss7_r_ops[]) (queue_t *, mblk_t *) = {
	ss7_r_data,		/* M_DATA */
	    ss7_r_proto,	/* M_PROTO */
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
	    ss7_r_error,	/* M_ERROR */
	    ss7_r_flush,	/* M_FLUSH */
	    ss7_r_hangup,	/* M_HANGUP */
	    NULL,		/* M_IOCACK */
	    NULL,		/* M_IOCNAK */
	    NULL,		/* M_IOCDATA */
	    ss7_r_proto,	/* M_PCPROTO */
	    NULL,		/* M_PCRSE */
	    NULL,		/* M_PCSIG */
	    NULL,		/* M_READ */
	    NULL,		/* M_STOP */
	    NULL,		/* M_START */
	    NULL,		/* M_STARTI */
	    NULL		/* M_STOPI */
};

static void ss7_l_create(lp_t * lp)
{
}
static void ss7_l_delete(lp_t * lp)
{
}

struct ops tua_ss7_l_ops = {
	&ss7_l_create,			/* create priv struct */
	&ss7_l_delete,			/* delete priv struct */
	&ss7_r_ops,			/* read operations */
	NULL				/* write operations */
};
