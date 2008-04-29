/*****************************************************************************

 @(#) $RCSfile: sua_ss7.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $

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

 Last Modified $Date: 2008-04-29 01:52:21 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sua_ss7.c,v $
 Revision 0.9.2.4  2008-04-29 01:52:21  brian
 - updated headers for release

 Revision 0.9.2.3  2007/08/14 08:33:55  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: sua_ss7.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $"

static char const ident[] =
    "$RCSfile: sua_ss7.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $";

#define __NO_VERSION__

#include <sys/os7/compat.h>

#include "sua.h"
#include "sua_data.h"
#include "sua_ctrl.h"
#include "sua_msg.h"

/*
 *  =========================================================================
 *
 *  SCCP-User --> SUA (ASP) Downstream Primitives
 *
 *  =========================================================================
 */
static inline
ss7_error_reply(queue_t *q, mblk_t *pdu, int prim, int err)
{
	mblk_t *mp;

	if ((mp = sccp_error_ack(prim, err))) {
		freemsg(pdu);
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
ss7_uderror_reply(queue_t *q, mblk_t *pdu, int err)
{
	mblk_t *mp;
	N_unitdata_req_t *p = (N_unitdata_req_t *) pdu->b_rptr;

	if ((mp = sccp_uderror_ind(err,
				   p->DEST_length ? (sccp_addr_t *) (pdu->b_rptr +
								     p->DEST_offset) : 0,
				   p->SRC_length ? (sccp_addr_t *) (pdu->b_rptr +
								    p->SRC_offset) : 0,
				   pdu->b_cont))) {
		freeb(pdu);
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  N_CONN_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_conn_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_CONN_RES
 *  -------------------------------------------------------------------------
 */
static int
ss7_conn_res(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_DISCON_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_discon_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_DATA_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_data_req(queue_t *q, mblk_t *pdu)
{
}
static int
ss7_w_data(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_EXDATA_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_exdata_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_INFO_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_info_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_BIND_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_bind_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_UNBIND_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_unbind_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_UNITDATA_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_unitdata_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_optmgmt_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_DATACK_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_datack_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_RESET_REQ
 *  -------------------------------------------------------------------------
 */
static int
ss7_reset_req(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_RESET_RES
 *  -------------------------------------------------------------------------
 */
static int
ss7_reset_res(queue_t *q, mblk_t *pdu)
{
}

static int (*ss7_dstr_prim[]) (queue_t *, mblk_t *) = {
#define SS7_DSTR_FIRST		   N_CONN_REQ
	ss7_conn_req,		/* N_CONN_REQ 0 */
	    ss7_conn_res,	/* N_CONN_RES 1 */
	    ss7_discon_req,	/* N_DICON_REQ 2 */
	    ss7_data_req,	/* N_DATA_REQ 3 */
	    ss7_exdata_req,	/* N_EXDATA_REQ 4 */
	    ss7_info_req,	/* N_INFO_REQ 5 */
	    ss7_bind_req,	/* N_BIND_REQ 6 */
	    ss7_unbind_req,	/* N_UNBIND_REQ 7 */
	    ss7_unitdata_req,	/* N_UNITDATA_REQ 8 */
	    ss7_optmgmt_req,	/* N_OPTMGMT_REQ 9 */
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
	    ss7_datack_req,	/* N_DATACK_REQ 23 */
	    NULL,		/* N_DATACK_IND 24 */
	    ss7_reset_req,	/* N_RESET_REQ 25 */
	    NULL,		/* N_RESET_IND 26 */
	    ss7_reset_res,	/* N_RESET_RES 27 */
#define SS7_DSTR_LAST		   N_RESET_RES
	    NULL		/* N_RESET_CON 28 */
};

static int
ss7_w_proto(queue_t *q, mblk_t *pdu)
{
	int prim = *((long *) mp->b_rptr);

	if (SS7_DSTR_FIRST <= prim && prim <= SS7_DSTR_LAST && ss7_dstr_prim[prim])
		return ((*ss7_dstr_prim[prim]) (q, mp));
	return (-EOPNOTSUPP);
}
static int
ss7_w_flush(queue_t *q, mblk_t *pdu)
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

static void
ss7_u_create(lp_t * lp)
{
}
static void
ss7_u_delete(lp_t * lp)
{
}

struct ops sua_ss7_u_ops = {
	&ss7_u_create,			/* create priv struct */
	&ss7_u_delete,			/* delete priv struct */
	NULL,				/* read operations */
	&ss7_w_ops			/* write operations */
};

/*
 *  =========================================================================
 *
 *  SCCP-Provider --> SUA (SGP) Upstream Primitives
 *
 *  =========================================================================
 */

/*
 *  N_CONN_IND
 *  -------------------------------------------------------------------------
 */
static int
ss7_conn_ind(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_CONN_CON
 *  -------------------------------------------------------------------------
 */
static int
ss7_conn_con(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_DISCON_IND
 *  -------------------------------------------------------------------------
 */
static int
ss7_discon_ind(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_DATA_IND
 *  -------------------------------------------------------------------------
 */
static int
ss7_data_ind(queue_t *q, mblk_t *pdu)
{
}
static int
ss7_r_data(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_EXDATA_IND
 *  -------------------------------------------------------------------------
 */
static int
ss7_exdata_ind(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_INFO_ACK
 *  -------------------------------------------------------------------------
 */
static int
ss7_info_ack(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_BIND_ACK
 *  -------------------------------------------------------------------------
 */
static int
ss7_bind_ack(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_ERROR_ACK
 *  -------------------------------------------------------------------------
 */
static int
ss7_error_ack(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_OK_ACK
 *  -------------------------------------------------------------------------
 */
static int
ss7_ok_ack(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_UNITDATA_IND
 *  -------------------------------------------------------------------------
 */
static int
ss7_unitdata_ind(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_UDERROR_IND
 *  -------------------------------------------------------------------------
 */
static int
ss7_uderror_ind(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_DATACK_IND
 *  -------------------------------------------------------------------------
 */
static int
ss7_datack_ind(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_RESET_IND
 *  -------------------------------------------------------------------------
 */
static int
ss7_reset_ind(queue_t *q, mblk_t *pdu)
{
}

/*
 *  N_RESET_CON
 *  -------------------------------------------------------------------------
 */
static int
ss7_conn_ind(queue_t *q, mblk_t *pdu)
{
}

static int (*ss7_ustr_prim[]) (queue_t *, mblk_t *) = {
	NULL,			/* N_CONN_REQ 0 */
	    NULL,		/* N_CONN_RES 1 */
	    NULL,		/* N_DICON_REQ 2 */
	    NULL,		/* N_DATA_REQ 3 */
	    NULL,		/* N_EXDATA_REQ 4 */
	    NULL,		/* N_INFO_REQ 5 */
	    NULL,		/* N_BIND_REQ 6 */
	    NULL,		/* N_UNBIND_REQ 7 */
	    NULL,		/* N_UNITDATA_REQ 8 */
	    NULL,		/* N_OPTMGMT_REQ 9 */
	    NULL,		/* 10 */
#define SS7_USTR_FIRST		   N_CONN_IND
	    ss7_conn_ind,	/* N_CONN_IND 11 */
	    ss7_conn_con,	/* N_CONN_CON 12 */
	    ss7_discon_ind,	/* N_DISCON_IND 13 */
	    ss7_data_ind,	/* N_DATA_IND 14 */
	    ss7_exdata_ind,	/* N_EXDATA_IND 15 */
	    ss7_info_ack,	/* N_INFO_ACK 16 */
	    ss7_bind_ack,	/* N_BIND_ACK 17 */
	    ss7_error_ack,	/* N_ERROR_ACK 18 */
	    ss7_ok_ack,		/* N_OK_ACK 19 */
	    ss7_unitdata_ind,	/* N_UNITDATA_IND 20 */
	    ss7_uderror_ind,	/* N_UDERROR_IND 21 */
	    NULL,		/* 22 */
	    NULL,		/* N_DATACK_REQ 23 */
	    ss7_datack_ind,	/* N_DATACK_IND 24 */
	    NULL,		/* N_RESET_REQ 25 */
	    ss7_reset_ind,	/* N_RESET_IND 26 */
	    NULL,		/* N_RESET_RES 27 */
	    ss7_reset_con	/* N_RESET_CON 28 */
#define SS7_USTR_LAST		   N_RESET_CON
};

static int
ss7_r_proto(queue_t *q, mblk_t *pdu)
{
}
static int
ss7_r_error(queue_t *q, mblk_t *pdu)
{
}
static int
ss7_r_hangup(queue_t *q, mblk_t *pdu)
{
}
static int
ss7_r_flush(queue_t *q, mblk_t *pdu)
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

static void
ss7_l_create(lp_t * lp)
{
}
static void
ss7_l_delete(lp_t * lp)
{
}

struct ops sua_ss7_l_ops = {
	&ss7_l_create,			/* create priv struct */
	&ss7_l_delete,			/* delete priv struct */
	&ss7_r_ops,			/* read operations */
	NULL				/* write operations */
};
