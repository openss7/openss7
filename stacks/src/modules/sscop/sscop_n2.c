/*****************************************************************************

 @(#) $RCSfile: sscop_n2.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:59 $

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

 Last Modified $Date: 2004/01/17 08:22:59 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop_n2.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:59 $"

static char const ident[] =
    "$RCSfile: sscop_n2.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/01/17 08:22:59 $";

#define SSCOP_DESCRIP	"SSCOP/UDP STREAMS DRIVER."
#define SSCOP_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SSCOP_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define SSCOP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SSCOP_LICENSE	"GPL"
#define SSCOP_BANNER	SSCOP_DESCIP	"\n" \
			SSCOP_COPYTIGHT	"\n" \
			SSCOP_DEVICE	"\n" \
			SSCOP_CONTACT	"\n"

#ifdef MODULE
MODULE_AUTHOR(SSCOP_CONTACT);
MODULE_DESCRIPTION(SSCOP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCCOP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SSCOP_LICENSE);
#endif
#define MODULE_STATIC static
#else
#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT
#endif

#ifdef SCCOP_DEBUG
static int sscop_debug = SSCOP_DEBUG;
#else
static int sscop_debug = 2;
#endif

#ifndef SSCOP_CMAJOR
#define SSCOP_CMAJOR 240
#endif
#define SSCOP_NMINOR 255

#ifdef LIS_2_12
#define INT int
#else
#define INT void
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Defintions
 *
 *  =========================================================================
 */

static struct module_info sscop_info = {
	0,				/* Module ID number */
	"sscop",			/* Module name */
	1,				/* Min packet size accepted */
	512,				/* Max packet size accepted */
	8 * 512,			/* Hi water mark */
	1 * 512				/* Lo water mark */
};

static INT sscop_rput(queue_t *, mblk_t *);
static INT sscop_rsrv(queue_t *);
static int sscop_open(queue_t *, dev_t *, int, int, cred_t *);
static int sscop_close(queue_t *, int, cred_t *);

static struct qinit sscop_rinit = {
	sscop_rput,			/* Read put (msg from below) */
	sscop_rsrv,			/* Read queue service */
	sscop_open,			/* Each open */
	sscop_close,			/* Last close */
	NULL,				/* Admin (not used) */
	&sscop_info,			/* Information */
	NULL				/* Statistics */
};

static INT sscop_wput(queue_t *, mblk_t *);
static INT sscop_wsrv(queue_t *);

static struct qinit sscop_winit = {
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
 *  SSCOP Private Structures
 *
 *  =========================================================================
 */

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
 *  SSCOP T-Provider --> T-User Primitives
 *
 *  =========================================================================
 */

static inline mblk_t *t_info_ack(queue_t * q)
{
	mblk_t *mp;
	struct T_info_ack *p;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_info_ack *) mp->b_wptr;
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = -1;
		p->ETSDU_size = -1;
		p->CDATA_size = -1;
		p->DDATA_size = 0;
		p->ADDR_size = -1;
		p->OPT_size = -1;
		p->TIDU_size = -1;
		p->SERV_type = T_COTS_ORD;
		p->CURRENT_state = sp->t_state;
		p->PROVIDER_flag = 0;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
static inline mblk_t *t_ok_ack(int prim)
{
	mblk_t *mp;
	struct T_ok_ack *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
static inline mblk_t *t_error_ack(int prim, int err)
{
	mblk_t *mp;
	struct T_error_ack *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_error_ack *) mp->b_wptr;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = err > 0 ? err : TSYSERR;
		p->UNIX_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
static inline mblk_t *t_optmgmt_ack(caddr_t opt_ptr, size_t opt_len, uint flags)
{
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	if ((mp = allocb(sizeof(*p) + opt_len, BRPI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_optmgmt_ack) mp->b_wptr;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
	}
	return (mp);
}
static inline mblk_t *t_addr_ack(queue_t * q)
{
	mblk_t *mp;
	struct T_addr_ack *p;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const size_t loc_len = sp->sport ? (sizeof(uint16_t) + sizeof(uint32_t)) : 0;
	const size_t rem_len = sp->dport ? (sizeof(uint16_t) + sizeof(uint32_t)) : 0;
	if ((mp = allocb(sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_addr_ack *) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = rem_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		*((uint16_t *) mp->b_wptr)++ = ntohs(sp->sport);
		*((uint32_t *) mp->b_wptr)++ = ntohs(sp->saddr);
		*((uint16_t *) mp->b_wptr)++ = ntohs(sp->dport);
		*((uint32_t *) mp->b_wptr)++ = ntohs(sp->daddr);
	}
	return (mp);
}
static inline mblk_t *t_bind_ack(queue_t * q)
{
	mblk_t *mp;
	struct T_bind_ack *p;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	const size_t add_len = sp->sport ? (sizeof(uint16_t) + sizeof(uint32_t)) : 0;
	if ((mp = allocb(sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_bind_ack *) mp->b_wptr;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = sp->cons;
		*((uint16_t *) mp->b_wptr)++ = sp->bport;
		*((uint32_t *) mp->b_wptr)++ = sp->baddr;
	}
	return (mp);
}
static inline mblk_t *t_conn_ind(dst_ptr, dst_len, opt_ptr, opt_len)
	caddr_t dst_ptr;
	size_t dst_len;
	caddr_t opt_ptr;
	size_t opt_len;
{
	mblk_t *mp;
	struct T_conn_ind *p;
	if ((mp = allocb(sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_conn_ind *) mp->b_wptr;
		p->PRIM_type = T_CONN_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dsg_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, b->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(opt_ptr, b->b_wptr, opt_len);
		mp->b_wptr += opt_len;
	}
	return (mp);
}
static inline mblk_t *t_data_ind(uint flag, mblk_t * dp)
{
	mblk_t *mp;
	struct T_data_ind *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_data_ind *) mp->b_wptr;
		p->PRIM_type = T_DATA_IND;
		p->MORE_flag = flag;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *t_exdata_ind(uint flag, mblk_t * dp)
{
	mblk_t *mp;
	struct T_exdata_ind *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_exdata_ind *) mp->b_wptr;
		p->PRIM_type = T_EXDATA_IND;
		p->MORE_flag = flag;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
	}
	return (mp);
}
static inline mblk_t *t_unitdata_ind(src_ptr, src_len, opt_ptr, opt_len)
	caddr_t src_ptr;
	size_t src_len;
	caddr_t opt_ptr;
	size_t opt_len;
{
	mblk_t *mp;
	struct T_unitdata_ind *p;
	if ((mp = allocb(sizeof(*p) + src_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_unitdata_ind *) mp->b_wptr;
		p->PRIM_type = T_UNITDATA_IND;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
	}
	return (mp);
}
static inline mblk_t *t_uderror_ind(dst_ptr, dst_len, opt_ptr, opt_len, type)
	caddr_t dst_ptr;
	size_t dst_len;
	caddr_t opt_ptr;
	size_t opt_len;
	int type;
{
	mblk_t *mp;
	struct T_uderror_ind *p;
	if ((mp = allocb(sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_uderror_ind *) mp->b_wptr;
		p->PRIM_type = T_UDERROR_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
		p->ERROR_type = type;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
	}
	return (mp);
}
static inline mblk_t *t_discon_ind(int reason, uint seq)
{
	mblk_t *mp;
	struct T_discon_ind *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_discon_ind *) mp->b_wptr;
		p->PRIM_type = T_DISCON_IND;
		p->DISCON_reason = reason;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
static inline mblk_t *t_ordrel_ind(void)
{
	mblk_t *mp;
	struct T_ordrel_ind *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ordrel_ind *) mp->b_wptr;
		p->PRIM_type = T_ORDREL_IND;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  =========================================================================
 *
 *  SCTP T-Provider --> N-Provider (UDP) Primitives
 *
 *  =========================================================================
 */
static inline mblk_t *n_info_req(void)
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
static inline mblk_t *m_optmgnt_req(qos_ptr, qos_len, flags)
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
static inline mblk_t *n_bind_req(add_ptr, add_len, cons, flags, pro_ptr, pro_len)
	caddr_t add_ptr;
	size_t add_len;
	uint cons;
	uint flags;
	caddr_t pro_ptr;
	size_t pro_len;
{
	mblk_t *mp;
	N_bind_req_t *p;
	if ((mp = allocb(sizeof(*p) + add_len + pro_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_bind_req_t *) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = cons;
		p->BIND_flags = falgs;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(pro_ptr, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
	}
	return (mp);
}
static inline mblk_t *n_unbind_req(void)
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
static inline mblk_t *n_datack_req(void)
{
	mblk_t *mp;
	N_datack_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_unbind_req_t *) mp->b_wptr;
		p->PRIM_type = N_DATACK_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}
static inline mblk_t *n_unitdata_req(dst_ptr, dst_len, dp)
	caddr_t dst_ptr;
	size_t dst_len;
	mblk_t *dp;
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
		mp->b_cont = dp;
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
 *  SEND BGN
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_bgn(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND BGAK
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_bgak(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND END
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_end(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND ENDAK
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_endak(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND RS
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_rs(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND RSAK
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_rsak(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND BGREJ
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_bgrej(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND SD
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_sd(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND ER
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_er(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND POLL
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_poll(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND STAT
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_stat(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND USTAT
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_ustat(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND UD
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_ud(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND MD
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_md(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}

/*
 *  SEND ERAK
 *  -------------------------------------------------------------------------
 */
static inline int sscop_send_erak(sp)
	const struct sscop *sp;
{
	mblk_t *mp;
}
