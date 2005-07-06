/*****************************************************************************

 @(#) $RCSfile: sscop_n.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/07/05 22:45:55 $

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/07/05 22:45:55 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop_n.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/07/05 22:45:55 $"

static char const ident[] =
    "$RCSfile: sscop_n.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/07/05 22:45:55 $";

#include <os7/compat.h>

#define SSCOP_NPI_DESCRIP	"SSCOP/IP STREAMS DRIVER."
#define SSCOP_NPI_REVISION	"OpenSS7 $RCSfile: sscop_n.c,v $ $Name:  $ ($Revision: 0.9.2.9 $) $Date: 2005/07/05 22:45:55 $"
#define SSCOP_NPI_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SSCOP_NPI_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define SSCOP_NPI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SSCOP_NPI_LICENSE	"GPL"
#define SSCOP_NPI_BANNER	SSCOP_NPI_DESCRIP	"\n" \
				SSCOP_NPI_REIVISION	"\n" \
				SSCOP_NPI_COPYRIGHT	"\n" \
				SSCOP_NPI_DEVICE	"\n" \
				SSCOP_NPI_CONTACT
#define SSCOP_NPI_SPLASH	SSCOP_NPI_DESCRIP	"\n" \
				SSCOP_NPI_REVISION

#ifdef LINUX
MODULE_AUTHOR(SSCOP_NPI_CONTACT);
MODULE_DESCRIPTION(SSCOP_NPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(SSCOP_NPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SSCOP_NPI_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sscop_n");
#endif
#endif				/* LINUX */

#ifdef SSCOP_NPI_DEBUG
STATIC int sscop_debug = SSCOP_NPI_DEBUG;
#else
STATIC int sscop_debug = 2;
#endif

#ifdef LFS
#define SSCOP_NPI_DRV_ID	CONFIG_STREAMS_SSCOP_NPI_MODID
#define SSCOP_NPI_DRV_NAME	CONFIG_STREAMS_SSCOP_NPI_NAME
#define SSCOP_NPI_CMAJORS	CONFIG_STREAMS_SSCOP_NPI_NMAJORS
#define SSCOP_NPI_CMAJOR_0	CONFIG_STREAMS_SSCOP_NPI_MAJOR
#define SSCOP_NPI_NMINOR	CONFIG_STREAMS_SSCOP_NPI_NMINORS
#endif				/* LFS */

#ifndef SSCOP_NPI_DRV_ID
#define SSCOP_NPI_DRV_ID	"sscop"
#endif				/* SSCOP_NPI_DRV_ID */

#ifndef SSCOP_NPI_CMAJOR_0
#define SSCOP_NPI_CMAJOR_0	240
#endif				/* SSCOP_NPI_CMAJOR_0 */

#ifndef SSCOP_NPI_NMINOR
#define SSCOP_NPI_NMINOR	255
#endif				/* SSCOP_NPI_NMINOR */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		SSCOP_NPI_DRV_ID
#define DRV_NAME	SSCOP_NPI_DRV_NAME
#ifdef MODULE
#define DRV_BANNER	SSCOP_NPI_BANNER
#else				/* MODULE */
#define DRV_BANNER	SSCOP_NPI_SPLASH
#endif				/* MODULE */

STATIC struct module_info sscop_info = {
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

struct sscop {
	struct sscop *next;		/* linkage for private structure list */
	struct sscop *pprev;		/* linkage for private structure list */

	queue_t *wq;
	queue_t *rq;

};

typedef struct sscop sscop_t;

/*
 *  =========================================================================
 *
 *  SSCOP Message Structures
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  SSCOP N-Provider --> N-User Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 *
 *  These functions package up the necessary primitive format given the
 *  input parameters necessary to create them.  They are all inlined and
 *  return NULL if a message block could not be allocated.  The reason for not
 *  passing the mblk up the queue yet is because the caller may wish to
 *  allocate the response before committing to processing the action which
 *  causes these indications and confirmations to the N-User.
 */

#include "sscop_n_prov.h"

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
 *  N_INFO_REQ
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_info_req(void)
{
	mblk_t *mp;
	N_info_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_info_req_t *) mp->b_wptr;
		p->PRIM_type = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 *  There isn't much point in this for IP except to set the TOS bits in the IP
 *  header and to indicate whether to set the DF bit to frag or not.  Also,
 *  default IP options could be set here.
 */
STATIC INLINE mblk_t *
n_optmgmt_req(caddr_t qos_ptr, size_t qos_len, uint flags)
{
	mblk_t *mp;
	N_optmgmt_req_t *p;
	if ((mp = allocb(sizeof(*p) + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_optmgmt_req_t *) mp->b_wptr;
		p->PRIM_type = N_OPTMGMT_REQ;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) : 0;
		p->OPTMGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	return (mp);
}

/*
 *  N_BIND_REQ
 *  -------------------------------------------------------------------------
 *  For IP for SSCOP we use this to bind the stream to the Protocol ID which is
 *  to be used for SSCOP (132).  The network layer will then pass messages up
 *  to the SSCOP.
 */
STATIC INLINE mblk_t *mblk_t *
n_bind_req(caddr_t add_ptr, size_t add_len, int cons, uint flags, caddr_t pro_ptr, size_t pro_len)
{
	mblk_t *mp;
	N_bind_req_t *p;
	if ((mp = allocb(sizeof(*p) + add_len + pro_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_bind_req_t *) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = 0;
		p->ADDR_offset = 0;
		p->CONIND_number = 0;
		p->BIND_flags = flags;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;	/* might pad */
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(pro_ptr, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	return (mp);

}

/*
 *  N_UNBIND_REQ
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_unbind_req(void)
{
	mblk_t *mp;
	N_unbind_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_unbind_req_t *) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_CONN_REQ
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_conn_req(caddr_t dst_ptr, size_t dst_len, uint flags, caddr_t qos_ptr, size_t qos_len)
{
	mblk_t *mp;
	N_conn_req_t *p;
	if ((mp = allocb(sizeof(*p) + dst_len + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_conn_req_t *) mp->b_wptr;
		p->PRIM_type = N_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->CONN_flags = flags;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + dst_len : 0;	/* might pad */
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	return (mp);
}

/*
 *  N_CONN_RES
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_conn_res(queue_t *q, caddr_t res_ptr, size_t res_len, uint flags, caddr_t qos_ptr, size_t qos_len)
{
	mblk_t *mp;
	N_conn_res_t *p;
	if ((mp = allocb(sizeof(*p) + res_len + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_conn_res_t *) mp->b_wptr;
		p->PRIM_type = N_CONN_RES;
		p->TOKEN_value = q;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->CONN_flags = flags;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(res_ptr, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	return (mp);
}

/*
 *  N_DATA_REQ
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_data_req(uint flags)
{
	mblk_t *mp;
	N_data_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_data_req_t *) mp->b_wptr;
		p->PRIM_type = N_DATA_REQ;
		p->DATA_xfer_flags = flags;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_DATACK_REQ
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_datack_req(void)
{
	mblk_t *mp;
	N_datack_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_datack_req_t *) mp->b_wptr;
		p->PRIM_type = N_DATACK_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_EXDATA_REQ
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_exdata_req(void)
{
	mblk_t *mp;
	N_exdata_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_exdata_req_t *) mp->b_wptr;
		p->PRIM_type = N_EXDATA_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_RESET_REQ
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_reset_req(int reason)
{
	mblk_t *mp;
	N_reset_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_reset_req_t *) mp->b_wptr;
		p->PRIM_type = N_RESET_REQ;
		p->RESET_reason = reason;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_RESET_CON
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_reset_con(void)
{
	mblk_t *mp;
	N_reset_con_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_reset_con_t *) mp->b_wptr;
		p->PRIM_type = N_RESET_CON;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_DISCON_REQ
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
n_discon_req(int reason, caddr_t res_ptr, size_t res_len, uint seq)
{
	mblk_t *mp;
	N_discon_req_t *p;
	if ((mp = allocb(sizeof(*p) + res_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_discon_req_t *) mp->b_wptr;
		p->PRIM_type = N_DISCON_REQ;
		p->DISCON_reason = reason;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
		bcopy(res_ptr, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
	}
	return (mp);
}

/*
 *  N_UNITDATA_REQ
 *  -------------------------------------------------------------------------
 */
STATIC INLINE mblk_t *
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

/*
 *  =========================================================================
 *
 *  SSCOP State Machine TIMEOUTS
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  SSCOP Peer --> SSCOP Primitives (Received Messages)
 *
 *  =========================================================================
 */

/*
 *  RECV SSCOP MESSAGE
 */
STATIC int
sscop_recv_msg(q, mp)
	const queue_t *q;
	const mblk_t *mp;
{
	struct sscop *sp = SSCOP_PRIV(q);
	int err = -EMSGSIZE;

	if (!mp)
		return -EFAULT;

	if (mp->b_cont)
		do {
			uint s_state, type;

			if ((s_state = sp->s_state) >= SSCOP_MAX_STATES)
				s_state = 0;

			type = ((struct sscopchdr *) mp->b_cont->b_rptr)->type & SSCOP_CTYPE_MASK;

			if (0 < type && type <= SSCOP_SHUTDOWN_COMPLETE)
				err = (*sscop_states[s_state][type]) (q, mp);
			else
				err = sscop_recv_unrec_ctype(q, mp);
		}
		while (err > 0 && mp->b - cont);

	return (err);
}

/*
 *  =========================================================================
 *
 *  N-Provider (IP) --> N-User (SSCOP) Primitives (M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */

/*
 *  N_UNITDATA_IND
 *  N_UDERROR_IND
 */

STATIC int
ip_unitdata_ind(queue_t *q, mblk_t *pdu)
{
	return (-EFAULT);
}
STATIC int
ip_uderror_ind(queue_t *q, mblk_t *pdu)
{
	return (-EFAULT);
}

STATIC int (*ip_prim[]) (queue_t *, mblk_t *) = {
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
	    &ip_unitdata_ind,	/* N_UNITDATA_IND 20 */
	    &ip_uderror_ind,	/* N_UDERROR_IND 21 */
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
 *  N-User --> N-Provider (SSCOP) Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 */
STATIC int
n_info_req(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	(void) pdu;
	if (!(mp = n_info_ack(q)))
		return (-ENOBUFS);
	freemsg(pdu);
	qreply(q, mp);
	return (0);
}
STATIC int
t_optmgmt_req(queue_t *q, mblk_t *pdu)
{
	int err;
	mblk_t *mp;
	sscop_t *sp = (sscop_t *) q->q_ptr;

	if (sp->n_state != NS_IDLE)
		goto opmgmt_req_outstate;
	{
		struct T_optmgmt_req *p = (struct T_optmgmt_req *) pdu->b_rptr;
		const caddr_t opt_ptr = p->OPT_offset + pdu->b_rptr;
		const size_t opt_len = p->OPT_length;
		const uint flags = p->MGMT_flags;

	}

      optmgmt_req_outstate:
	err = NOUTSTATE;
	goto optmgmt_req_error;

      optmgmt_req_error:
	freemsg(pdu);
	qreply(q, t_error_ack(T_OPTMGMT_REQ, err));
	return (0);
}
STATIC int
t_bind_req(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);

	switch (sp->n_state) {
	case NS_UNBND:
	{
		struct T_bind_req *p = (struct T_bind_req *) pdu->b_rptr;
		caddr_t add_ptr;
		size_t add_len;
		u16 bport = 0;
		u32 baddr;
		int err = 0;
		struct sscop_bind *bb, **bbp;

		/* pull addresses out of bind primitive */
		if ((add_len = p->ADDR_length) >= sizeof(u16)
		    && add_len <= mp->b_wptr - p->ADDR_offset) {
			struct sscop_baddr **sbp = &sp->baddr;

			add_ptr = p->ADDR_offset + pdu->b_rptr;

			bport = *((u16 *) add_ptr)++;

			for (add_len -= sizeof(u16); add_len >= sizeof(u32); add_len -= sizeof(u32)) {
				if (!((baddr = *((u32 *) add_ptr)++) & 0xff000000))
					goto t_bind_req_badaddr;

				if (!
				    (*sbp =
				     kmem_alloc_cache(sscop_badd_cachep, SLAB_HWCACHE_ALIGN)))
					goto t_bind_req_nomem;

				(*sbp)->baddr = baddr;

				(*sbp)->next = NULL;
				(*sbp)->pprev = sbp;
				sbp = &(*sbp)->next;

				sp->banum++;
			}
		}

		cons = p->CONIND_number;
		/* See if we need to assign a port number */
		if (!(sp->bport = htons(bport)) && !(sp->cons = cons)) {
			static u16 sscop_port_rover = 0;
			/* 
			 *  This stream can only be used for outgoing connections, so
			 *  if it is assigned a zero port number we choose an unused
			 *  port number for the stream and assign it.
			 */
			int low = sysctl_local_port_range[0];
			int high = sysctl_local_port_range[1];
			int rem = (high - low) + 1;

			bport = sscop_port_rover;

			for (; rem > 0; bport++, rem--) {
				if (bport > high || bport < low)
					bport = low;
				bbp = &sscop_bind_hash[bport & 0xff];
				for (bb = *bbp; bb && bb->port != bport; bb = bb->next) ;
				if (!bb || !bb->bound)
					break;
			}
			/* want a position with an unowned bind bucket */
			if (rem <= 0 || (bb && bb->bound))
				goto t_bind_req_noaddr;

			sscop_port_rover = bport;
		} else {
			bbp = &sscop_bind_hash[bport & 0xff];
			for (bb = *bbp; bb && sp->bport != bport; bb = bb->next) ;
		}
		/* If we have an existing bind bucket, check for conflicts */
		if (bb && bb->cons && cons) {
			struct sscop *sp2;

			err = TADDRBUSY;
			for (sp2 = bb->bound; sp2; sp2 = sp2->bind_next) {
				struct sscop_baddr *sb, *sb2;

				if (!sp->baddr && !sb2->baddr)
					goto t_bind_req_addrbusy;

				for (sb = sp->baddr; sb; sb = sb->next)
					for (sb2 = sp2->baddr; sb2; sb2 = sb2->next)
						if (sb->baddr == sb2->baddr)
							goto t_bind_req_addrbusy;
			}
		}
		/* place in bind hashes */
		if (!bb) {
			/* no bind bucket, create one */
			if (!(bb = kmalloc(sizeof(*bb), GFP_ATOMIC))) {
				freemsg(mp);
				goto t_bind_req_nomem;
			}
			if ((bb->next = *bbp))
				bb->next->pprev = &bb->next;
			bb->pprev = bbp;
			*bbp = bb;
			bb->port = bport;
			bb->cons = 0;
			bb->bound = NULL;
		}
		sp->bind = bb;
		if ((sp->bind_next = bb->bound))
			sp->bind_next->bind_pprev = &sp->bind_next;
		sp->bind_pprev = &bb->bound;
		bb->bound = sp;
		bb->cons++;

		sp->n_state = NS_IDLE;
		freemsg(pdu);
		mp = t_bind_ack(q);
		qreply(q, mp);
		return (0);
	}
	}

      t_bind_req_outstate:
	err = NOUTSTATE;
	goto t_bind_req_error;

      t_bind_req_badaddr:
	err = NBADADDR;
	goto t_bind_req_error;

      t_bind_req_addrbusy:
	err = TADDRBUSY;
	goto t_bind_req_error;

      t_bind_req_noaddr:
	err = NNOADDR;
	goto t_bind_req_error;

      t_bind_req_nomem:
	err = -ENOMEM;
	goto t_bind_req_error;

      t_bind_req_nobufs:
	err = -ENOBUFS;
	goto t_bind_req_error;

      t_bind_req_error:
	sp->bport = 0;
	{
		struct sscop_baddr *sb;
		while ((sb = sp->baddr)) {
			sb->baddr = sb->next;
			kmem_free_cache(sscop_badd_cachep, sb);
		}
		sp->banum = 0;
		sp->baddr = NULL;
	}
	freemsg(pdu);
	qreply(q, t_error_ack(T_BIND_REQ, err));
	return (0);
}
STATIC int
t_unbind_req(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);

	struct sscop_bind *bb;

	if (sp->n_state != NS_IDLE)
		goto t_unbind_req_outstate;

	if ((bb = sp->bind)) {
		if (sp->cons)
			bb->cons--;
		if ((*sp->bind_pprev = sp->bind_next))
			sp->bind_next->bind_pprev = sp->bind_pprev;
		sp->bind = NULL;
		sp->bind_next = NULL;
		sp->bind_pprev = NULL;
		sp->cons = 0;
		if (!bb->bound) {
			if ((*bb->pprev = bb->next))
				bb->next->pprev = bb->pprev;
			kfree(bb);
		}
	}
	{
		struct sscop_baddr *sb;

		while ((sb = sp->baddr)) {
			sp->baddr = sb->next;
			kmem_free_cache(sscop_badd_cachep, sb);
		}
		sp->banum = 0;
		sp->bport = 0;
	}
	sp->n_state = NS_UNBND;
	freemsg(pdu);
	mp = t_ok_ack(T_UNBIND_REQ);
	qreply(q, mp);
	return (0);

      t_unbind_req_outstate:
	err = NOUTSTATE;
	goto t_unbind_req_error;

      t_unbind_req_error:
	freemsg(pdu);
	qreply(q, t_error_ack(T_UNBIND_REQ, err));
	return (0);
}
STATIC int
t_conn_req(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_conn_req *p = (struct T_conn_req *) pdu->b_rptr;
	const caddr_t dst_ptr = p->DEST_offset + pdu->b_rptr;
	const size_t dst_len = p->DEST_length;
	const caddr_t opt_ptr = p->OPT_offset + pdu->b_rptr;
	const size_t opt_len = p->OPT_length;

	if (sp->n_state != NS_IDLE)
		goto t_conn_req_outstate;

	/* pull addresses out of the destination */
	if ((dst_len = p->DEST_length) < sizeof(u16) + sizeof(u32))
		goto t_conn_req_badaddr;
	if ((dst_len > mp->b_wptr - p->DEST_offset))
		goto t_conn_req_badaddr;

	dst_ptr = p->DEST_offset + pdu->b_rptr;

	if (!(sp->dport = htons(*((u16 *) dst_ptr)++)))
		goto t_conn_req_badaddr;
	/* 
	 *  TODO: check if user is allowed to set port address.
	 */

	for (dst_len -= sizeof(u16); dst_len >= sizeof(u32); dst_len -= siseof(u32)) {
		struct sscop_daddr *sd;

		if (!((daddr = *((u32 *) dst_ptr)++) & 0xff000000))
			goto t_conn_req_badaddr;

		if (!(sd = kmalloc(sizeof(*sd), GFP_ATOMIC)))
			goto t_conn_req_nomem;
		bzero(sd, sizeof(*sd));

		if ((sd->next = sp->daddr))
			sd->next->pprev = &sd->next;
		sd->pprev = &sp->daddr;
		sp->daddr = sd;
		sp->danum++;
		sd->sp = sp;
		sd->daddr = daddr;
		/* 
		 *  TODO: initialize reset of daddr structure.
		 */
	}
	/* 
	 *  TODO: check if we can route to one of the destination addresses.
	 */
	sp->v_tag = tcp_secure_sequence(sp->saddr->saddr, sp->daddr->daddr, sp->sport, sp->dport);
	sp->a_rwnd = FIXME;
	sp->n_ostr = -1;	/* ask for as many as we can get */
	sp->n_istr = -1;	/* offer as many as the other end needs */
	sp->i_tsn = htonl(sp->v_tag);
	/* 
	 *  TODO: pull out options, this will wind up negotiating the number
	 *  if outbound streams and the accepted number of inbound streams
	 *  requested by the user.  It may also set the advertized receive
	 *  window, but I would rather gather that from the high water mark on
	 *  the upstream receive queue.  We must pick an initial verification
	 *  tag and an initial transmit sequence number.
	 */
	/* 
	 *  TODO: do some connect routing on the destination address list and
	 *  determine the best alternative addresses.
	 */
	sp->taddr = sp->daddr;
	sp->raddr = sp->daddr->next ? sp->daddr->next : sp->daddr;

	if ((err = sscop_send_init(p)))
		goto t_conn_req_error;

	sp->s_state = SSCOP_COOKIE_WAIT;
	sp->n_state = T_WCON_CREQ;

	freemsg(pdu);
	mp = t_ok_ack(T_CONN_REQ);
	qreply(q, mp);
	return (0);

	/* Try to connect to the first address */

      t_conn_req_outstate:
	err = NOUTSTATE;
	goto t_conn_req_error;

      t_conn_req_badaddr:
	err = NBADADDR;
	goto t_conn_req_error;

      t_conn_req_error:
	bzero(&sp->l, sizeof(sp->l));
	{
		struct sscop_daddr *sd, sd_next = sp->daddrs;
		while ((sd = sd_next)) {
			sd_next = sd->next;
			kfree(sd);
		}
		sp->danum = 0;
	}
	freemsg(pdu);
	qreply(q, t_error_ack(T_CONN_REQ, err));
	return (0);
}
STATIC int
t_conn_res(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_conn_res *p = (struct T_conn_res *) pdu->b_rptr;
	queue_t *aq = (queue_t *) p->ACCEPTOR_id;
	const caddr_t opt_ptr = p->OPT_offset + pdu->b_rptr;
	const size_t opt_len = p->OPT_length;
	const uint seq = p->SEQ_number;

	switch (sp->n_state) {
	case NS_WRES_CIND:
	{
		struct sscop *ap;
		struct sscop_cookie_echo *m;
		struct sscop_cookie *ck
		    /* first look for connection indication with indicated sequence number */
		for (mp = sp->connect_queue.q_head; mp; mp = mp->b_next) {
			struct sscop_rcb *cb = SSCOP_RCB(mp);

			if (cb->seq == seq)
				break;
		}
		if (!mp)
			goto conn_res_badseq;
		if (!aq || !aq->q_ptr || !aq->q_qinfo || !aq->q_qinfo->qi_minfo)
			goto conn_res_badq;
		if (aq->q_qinfo->qi_minfo.mi_idnum != q->q_qinfo->qi_minfo.mi_idnum)
			goto conn_res_provmismatch;

		ap = SSCOP_PRIV(aq);

		if (aq != q) {
			if (aq->n_state == NS_UNBND)
				goto conn_res_badaddr;
			if (aq->n_state != NS_IDLE)
				goto conn_res_badf;
			if (ap->cons)
				goto conn_res_resqlen;
			if (sp->sport && ap->sport && sp->sport != ap->sport)
				goto conn_res_resaddr;
			if (sp->saddr && ap->saddr && !sscop_same_bind(sp, ap))
				goto conn_res_resaddr;
		}
		if (opt_len) {
			u16 *sidp = NULL;
			u32 *ppip = NULL;

			struct t_opthdr *oh;
			caddr_t op;
			for (op = opt_ptr, oh = (struct t_opthdr *) op;
			     op < opt_ptr + opt_len;
			     op += PADC(oh->length), oh = (struct t_opthdr *) op) {
				if (oh->level == T_INET_SSCOP) {
					switch (oh->name) {
					case SSCOP_SID:
						if (oh->length >= sizeof(*oh) + sizeof(u16))
							sidp = ((u16 *) oh->value);
						continue;
					case SSCOP_PPI:
						if (oh->length >= sizeof(*oh) + sizeof(u32))
							ppip = ((u32 *) oh->value);
						continue;
					default:
						goto conn_res_badopt;
					}
				}
				goto conn_res_badopt;
			}
			if (sidp)
				ap->sid = *sidp;
			if (ppip)
				ap->ppi = *ppip;

			ap->ostr = sscop_find_ostr(ap, ap->sid);
			ap->ostr->ppi = ap->ppi;
		}
		m = (struct sscop_cookie_echo *) mp->b_cont->b_rptr;
		ck = m->cookie;

		ap->s_state = SSCOP_ESTABLISHED;
		ap->n_state = NS_DATA_XFER;

		ap->dport = ck->dport;
		ap->sport = ck->sport;

		ap->sackf = SSCOP_SACKF_NOD;
		ap->v_tag = ck->v_tag;
		ap->p_tag = ck->p_tag;
		ap->i_strs = ck->i_strs;
		ap->o_strs = ck->o_strs;
		ap->sscop_tsn = ck->v_tag;
		ap->a_tsn = ck->v_tag - 1;
		ap->c_tsn = ck->p_tsn - 1;
		ap->e_tsn = ck->p_tsn - 1;
		ap->p_rwnd = ck->p_rwnd;

		while ((st = ap->ostrm)) {
			ap->ostrm = st->next;
			kmem_free_cache(sscop_strm_cachep, st);
		}
		ap->osnum = 0;

		while ((st = ap->istrm)) {
			ap->istrm = st->next;
			kmem_free_cache(sscop_strm_cachep, st);
		}
		ap->isnum = 0;

		while ((sd = ap->daddr)) {
			ap->daddr = sd->next;
			kmem_free_cache(sscop_dest_cachep, sd);
		}
		ap->danum = 0;

		while ((ss = ap->saddr)) {
			ap->saddr = ss->next;
			kmem_free_cache(sscop_srce_cachep, ss);
		}
		ap->sanum = 0;

		{
			int dnum = ck->dta_num;
			int snum = ck->sta_num;
			struct sscop_daddr **sdp = &ap->daddr;
			struct sscop_saddr **ssp = &ap->saddr;

			u32 *addp = (u32 *) (((u8 *) (ck + 1) + ck->opt_len));

			for (i = 0; i < ck->dta_num; i++, sdp = &(*sdp)->next) {
				if (!
				    (*sdp =
				     kmem_alloc_cache(sscop_dest_cachep, SLAB_HWALIGN_CACHE)))
					goto conn_res_nomem;
				bzero(*sdp, sizeof(**sdp));
				(*sdp)->daddr = *addp++;
				(*sdp)->sp = ap;
				/* 
				 *  More daddr initialization.
				 */
			}
			for (i = 0; i < ck->sta_num; i++, ssp = (*ssp)->next) {
				if (!
				    (*ssp =
				     kmem_alloc_cache(sscop_srce_cachep, SLAB_HWALIGN_CACHE)))
					goto conn_res_nomem;
				bzero(*ssp, sizeof(**ssp));
				(*ssp)->saddr = *addp++;
				(*ssp)->sp = ap;
				/* 
				 *  More saddr initialization.
				 */
			}
		}

	}
	}
      conn_res_outstate:
	err = NOUTSTATE;
	goto conn_res_error;

      conn_res_nomem:
	err = -ENOMEM;
	goto conn_res_free_error;

      conn_res_badaddr:
	err = NBADADDR;
	goto conn_res_error;

      conn_res_resaddr:
	err = TRESADDR;
	goto conn_res_error;

      conn_res_badseq:
	err = NBADSEQ;
	goto conn_res_error;

      conn_res_badf:
	err = NBADTOKEN;
	goto conn_res_error;

      conn_res_badopt:
	err = NBADOPT;
	goto conn_res_error;

      conn_res_provmismatch:
	err = TPROVMISMATCH;
	goto conn_res_error;

      conn_res_resqlen:
	err = TRESQLEN;
	goto conn_res_error;

      conn_res_free_error:
	while ((sd = ap->daddr)) {
		ap->daddr = sd->next;
		kmem_free_cache(sscop_dest_cachep, sd);
	}
	ap->danum = 0;
	ap->dport = 0;
	while ((ss = ap->daddr)) {
		ap->daddr = ss->next;
		kmem_free_cache(sscop_srce_cachep, ss);
	}
	ap->sanum = 0;

      conn_res_error:
	freemsg(pdu);
	qreply(q, t_error_ack(T_CONN_RES, NOUTSTATE));
	return (0);
}
STATIC int
t_data_req(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_data_req *p = (struct T_data_req *) pdu->b_rptr;
	const uint flag = p->MORE_flag;

	uint flags = 0;

	switch (sp->n_state) {
	case NS_DATA_XFER:
	case NS_WREQ_ORDREL:
		if (!pdu->b_cont || pdu->b_cont->b_datap->db_type != M_DATA)
			goto data_req_error;
		if (sp->ostr->x_more)
			goto data_req_error;
		if (!sp->ostr->n_more)
			flags |= SSCOPCB_FLAG_FIRST_FRAG;
		if (!(sp->ostr->n_more = (flag & T_MORE)))
			flags |= SSCOPCB_FLAG_LAST_FRAG;
		if ((err = sscop_send_data(sp, NULL, sp->ostr, flags, pdu->b_cont)))
			return (err);
		freeb(pdu);
		return (0);

	case NS_IDLE:
		freemsg(pdu);
		return (0);
	}
      data_req_error:
	freemsg(pdu);
	qreply(q, t_m_error(EPROTO));
	return (0);
}
STATIC int
t_exdata_req(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_exdata_req *p = (struct T_exdata_req *) pdu->b_rptr;
	const uint flag = p->MORE_flag;

	uint flags = SSCOPCB_FLAG_URG;

	switch (sp->n_state) {
	case NS_DATA_XFER:
	case NS_WREQ_ORDREL:
		if (!pdu->b_cont || pdu->b_cont->b_datap->db_type != M_DATA)
			goto exdata_req_error;
		if (!sp->ostr->x_more)
			flags |= SSCOPCB_FLAG_FIRST_FRAG;
		if (!(sp->ostr->x_more = (flag & T_MORE)))
			flags |= SSCOPCB_FLAG_LAST_FRAG;
		if ((err = sscop_send_data(sp, NULL, sp->ostr, flags pdu->b_cont)))
			return (err);
		freeb(pdu);
		return (0);

	case NS_IDLE:
		freemsg(pdu);
		return (0);
	}
	freemsg(pdu);
	qreply(q, t_m_error(EPROTO));
	return (0);
}
STATIC int
t_optdata_req(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_optdata_req *p = (struct T_optdata_req *) pdu->b_rptr;
	const uint flag = p->DATA_flag;
	const caddr_t opt_ptr = p->OPT_offset + pdu->b_rptr;
	const size_t opt_len = p->OPT_length;

	if (!opt_len)
		return (flag & T_EXPEDITED)
		    ? t_exdata_req(q, pdu) : t_data_req(q, pdu);

	switch (sp->n_state) {
	case NS_DATA_XFER:
	case NS_WREQ_ORDREL:
	{
		uint flags = 0;
		struct sscop_strm *st = sp->ostr;
		uint *morep;

		{
			int sid = -1;
			int ppi = -1;
			caddr_t op;
			struct t_opthdr *oh;

			/* Walk through options */
			for (op = opt_ptr, oh = (struct t_opthdr *) op;
			     op < opt_ptr + opt_len;
			     op += PADC(oh->length), oh = (struct t_opthdr *) op) {
				switch (oh->level) {
				case T_INET_SSCOP:
					switch (oh->name) {
					case SSCOP_SID:
						if (oh->length >= sizeof(*oh) + sizeof(u16))
							sid = *((u16 *) oh->value);
						continue;

					case SSCOP_PPI:
						if (oh->length >= sizeof(*oh) + sizeof(u32))
							ppi = *((u32 *) oh->value);
						continue;
					}
				}
			}
			if (sid != -1 && !(st = sscop_find_ostr(sp, sid))
			    && !(st = sp->ostr)
			    && !(st = sp->ostr = sscop_find_ostr(sp, sp->sid))
			    && !(st = sp->ostr = sscop_find_ostr(sp, 0))
			    && !(st = sp->ostr = sscop_alloc_ostr(sp, 0)))
				return -ENOMEM;
			if (ppi != -1)
				st->ppi = ppi;
		}

		if (flag & T_EXPEDITED) {
			flags |= SSCOPCB_FLAG_URG;
			morep = &st->x_more;
		} else
			morep = &st->n_more;

		if (!pdu->b_cont || pdu->b_cont->b_datap->db_type != M_DATA)
			goto optdata_req_error;
		if (!*morep)
			flags |= SSCOPCB_FLAG_FIRST_FRAG;
		if (!(*morep = (flag & T_MORE)))
			flags |= SSCOPCB_FLAG_LAST_FRAG;
		if ((err = sscop_send_data(sp, NULL, st, flags, pdu->b_cont)))
			return (err);
		freeb(pdu);
		return (0);
	}
	case NS_IDLE:
		freemsg(pdu);
		return (0);
	}
	freemsg(pdu);
	qreply(q, t_m_error(EPROTO));
	return (0);
}
STATIC int
t_unitdata_req(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_unitdata_req *p = (struct T_unitdata_req *) pdu->b_rptr;
	const caddr_t dst_ptr = p->DEST_offset + pdu->b_rptr;
	const size_t dst_len = p->DEST_length;
	const caddr_t opt_ptr = p->OPT_offset + pdu->b_rptr;
	const size_t opt_len = p->OPT_length;

	freemsg(pdu);
	qreply(q, t_error_ack(T_UNIDATA_REQ, NNOTSUPPORT));
	return (0);

}
STATIC int
t_discon_req(queue_t *q, mblk_t *pdu)
{
	mblk_t *mp;
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_discon_req *p = (struct T_discon_req *) pdu->b_rptr;
	const uint seq = p->SEQ_number;

	if (sp->n_state == NS_WRES_CIND) {
		mblk_t *mp;
		for (mp = sp->connect_queue.q_head; mp; mp->next) {
			struct sscop_rcb *cb = SSCOP_RCB(mp);

			if (cb->seq == seq)
				break;
		}
		if (!mp)
			goto discon_res_badseq;

		bufq_unlink(&sp->connect_queue, mp);
		freemsg(pdu);
		qreply(q, t_ok_ack(T_DISCON_REQ));
		return (0);
	}
	if ((1 << sp->
	     n_state) & (TSF_WCON_CREQ | TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL)) {
		struct sscop_daddr *sd;
		struct sscop_saddr *ss;

		/* stop association timers */
		if (sp->timer_cookie)
			untimeout(xchg(&sp->timer_cookie, 0));
		if (sp->timer_sack)
			untimeout(xchg(&sp->timer_sack, 0));

		freemsg(xchg(&sp->retry, NULL));

		/* send abort */
		s_send_abort(sp);

		/* remove from vtag cache */
		if (*sp->vtag_pprev = sp->vtag_next)
			sp->vtag_next->vtag_pprev = sp->vtag_pprev;
		sp->vtag_next = NULL;
		sp->vtag_pprev = &sp->vtag_next;
		sp->v_tag = 0;

		/* remove from ptag cache */
		if (*sp->ptag_pprev = sp->ptag_next)
			sp->ptag_next->ptag_pprev = sp->ptag_pprev;
		sp->ptag_next = NULL;
		sp->ptag_pprev = &sp->ptag_next;
		sp->p_tag = 0;

		/* remove peer addresses */
		while ((sd = sp->daddr)) {
			sp->daddr = sd->next;

			if (sd->timer_retrans)
				untimeout(xchg(&sd->timer_retrans, 0));
			if (sd->timer_idle)
				untimeout(xchg(&sd->timer_idle, 0));
			if (sd->timer_heartbeat)
				untimeout(xchg(&sd->timer_heartbeat, 0));

			/* FIXME: free destination cache */

			kmem_free_cache(sscop_dest_cachep, sd);
		}
		sp->danum = 0;

		/* remove local addresses */
		while ((ss = sp->saddr)) {
			sp->saddr = ss->next;
		      kmem_free_cache(sscop_srce_cachep, ss):
		}
		sp->sanum = 0;

		/* purge send queues */
		bufq_purge(&sp->write_queue);
		bufq_purge(&sp->urgent_queue);
		bufq_purge(&sp->retrans_queue);

		/* purge recv queues */
		bufq_purge(&sp->out_of_order_queue);
		bufq_purge(&sp->duplicate_queue);
		sp->ngaps = 0;
		sp->ndups = 0;

		sp->s_state = SSCOP_CLOSED;
		sp->n_state = T_IDLE;

		freemsg(pdu);
		qreply(q, t_ok_ack(T_DISCON_REQ));
		return (0);
	}

      discon_req_outstate:
	err = NOUTSTATE;
	goto discon_req_error;

      discon_req_badseq:
	err = NBADSEQ;
	goto discon_req_error;

      discon_req_error:
	freemsg(pdu);
	qreply(q, t_error_ack(T_DISCON_REQ, err));
	return (0);
}
STATIC int
t_ordrel_req(queue_t *q, mblk_t *pdu)
{
	sscop_t *sp = SSCOP_PRIV(q);
	struct T_ordrel_req *p = (struct T_ordrel_req *) pdu->b_rptr;

	switch (sp->n_state) {
	case NS_DATA_XFER:
		ensure(sp->s_state == SSCOP_ESTABLISHED, break);

		freemsg(pdu);
		sp->s_state = SSCOP_SHUTDOWN_PENDING;
		if (!sp->retrans_queue.q_count) {
			sscop_send_shutdown(sp);
			sp->s_state = SSCOP_SHUTDOWN_SENT;
		}
		sp->n_state = NS_WIND_ORDREL;
		return (0);

	case NS_WREQ_ORDREL:
		ensure(sp->s_state == SSCOP_SHUTDOWN_RECEIVED, break);

		freemsg(pdu);
		if (!sp->retrans_queue.q_count) {
			sscop_send_shutdown_ack(sp);
			sp->s_state = SSCOP_SHUTDOWN_ACK_SENT2;
		}
		sp->n_state = NS_IDLE;
		return (0);
	}
	sp->n_state = NS_NOSTATES;
	freemsg(pdu);
	qreply(q, t_m_error(EPROTO));
	return (0);
}

STATIC int (*t_prim[]) (queue_t *, mblk_t *) = {
	&t_conn_req,		/* T_CONN_REQ 0 */
	    &t_conn_res,	/* T_CONN_RES 1 */
	    &t_discon_req,	/* T_DISCON_REQ 2 */
	    &t_data_req,	/* T_DATA_REQ 3 */
	    &t_exdata_req,	/* T_EXDATA_REQ 4 */
	    &t_info_req,	/* T_INFO_REQ 5 */
	    &t_bind_req,	/* T_BIND_REQ 6 */
	    &t_unbind_req,	/* T_UNBIND_REQ 7 */
	    &t_unitdata_req,	/* T_UNITDATA_REQ 8 */
	    &t_optmgmt_req,	/* T_OPTMGMT_REQ 9 */
	    &t_ordrel_req,	/* T_ORDREL_REQ 10 */
	    NULL,		/* T_CONN_IND 11 */
	    NULL,		/* T_CONN_CON 12 */
	    NULL,		/* T_DISCON_IND 13 */
	    NULL,		/* T_DATA_IND 14 */
	    NULL,		/* T_EXDATA_IND 15 */
	    NULL,		/* T_INFO_ACK 16 */
	    NULL,		/* T_BIND_ACK 17 */
	    NULL,		/* T_ERROR_ACK 18 */
	    NULL,		/* T_OK_ACK 19 */
	    NULL,		/* T_UNITDATA_IND 20 */
	    NULL,		/* T_UDERROR_IND 21 */
	    NULL,		/* T_OPTMGMT_ACK 22 */
	    NULL,		/* T_ORDREL_IND 23 */
	    &t_optdata_req,	/* T_OPTDATA_REQ 24 */
	    &t_addr_req,	/* T_ADDR_REQ 25 */
	    NULL,		/* T_OPTDATA_IND 26 */
	    NULL		/* T_ADDR_ACK 27 */
};

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
STATIC INLINE int
sscop_w_data(queue_t *q, mblk_t *mp)
{
	return sscop_write_data(q, mp);
}
STATIC INLINE int
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
STATIC INLINE int
sscop_w_proto(queue_t *q, mblk_t *mp)
{
	int prim = ((union T_primitives *) (mp->b_rptr))->type;
	if (0 <= prim && prim < sizeof(t_prim) / sizeof(void *))
		if (t_prim[prim])
			return (*t_prim[prim]) (q, mp);
	return (-EOPNOTSUPP);
}
STATIC INLINE int
sscop_r_proto(queue_t *q, mblk_t *mp)
{
	int prim = ((union N_primitives *) (mp->b_rptr))->type;
	if (0 <= prim && prim < sizeof(n_prim) / sizeof(void *))
		if (n_prim[prim])
			return (*n_prim[prim]) (q, mp);
	return (-EOPNOTSUPP);
}
STATIC INLINE int
sscop_w_pcproto(queue_t *q, mblk_t *mp)
{
	return sscop_w_proto(q, mp);
}
STATIC INLINE int
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
STATIC INLINE int
sscop_w_ctl(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	return (-EOPNOTSUPP);
}
STATIC INLINE int
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
STATIC INLINE int
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
STATIC INLINE void
sscop_w_flush(queue_t *q, mblk_t *mp)
{
	if (*mp->b_rptr & M_FLUSHW) {
		flushq(q, FLUSHALL);
		if (q - q_next) {
			putnext(q, mp);
			return;
		}
		*mp->b_rptr &= ~FLUSHW;
	}
	if (*mp->b_rptr & FLUSHR) {
		flushq(RD(q), FLUSHALL);
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
STATIC INLINE void
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
 *  supported, we pass it down-queue if possible.  If the message cannot be
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
kmem_cache_t *sscop_cachep = NULL;

STATIC sscop_t *
sscop_alloc_priv(queue_t *q)
{
	sscop_t *sscop;

	if ((sscop = kmem_cache_alloc(sscop_cachep))) {
		bzero(sscop, sizeof(*sscop));
		RD(q)->q_ptr = WR(q)->q_tpr = sscop;
		sscop->rq = RD(q);
		sscop->wq = WR(q);
	}
	return (sscop);
}

STATIC void
sscop_free_priv(queue_t *q)
{
	sscop_t *m2 = SSCOP_PRIV(q);
	kmem_cache_free(sscop_cachep, sscop);
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
		if (!(sscop = kmem_cache_alloc(sscop_priv_cachep, SLAB_HWCACHE_ALIGN)))
			return ENOMEM;
		RD(q)->q_ptr = WR(q)->q_ptr = sscop;
		sscop->rq = RD(q);
		sscop->wq = WR(q);
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
	sscop_t *sscop = SSCOP_PRIV(q);

	(void) flag;
	(void) crp;

	/* 
	 *  TODO: make sure everything is deallocated
	 */
	kmem_cache_free(sscop_priv_cachep, sscop);
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
	.f_str = &sscop_npiinfo,
	.f_flag = 0,
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
	if ((err = lis_register_strmod(&sscop_npiinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
sscop_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&sscop_npiinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sscop_npiinit(void)
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
sscop_npiterminate(void)
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
module_init(sscop_npiinit);
module_exit(sscop_npiterminate);

#endif				/* LINUX */
