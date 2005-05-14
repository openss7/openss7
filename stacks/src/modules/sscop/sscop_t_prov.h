/*****************************************************************************

 @(#) $RCSfile: sscop_t_prov.h,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/05/14 08:31:10 $

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

 Last Modified $Date: 2005/05/14 08:31:10 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SSCOP_T_PROV_H__
#define __SSCOP_T_PROV_H__

#ident "@(#) $RCSfile: sscop_t_prov.h,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/05/14 08:31:10 $"

/*
 *  =========================================================================
 *
 *  SSCOP T-Provider --> T-User Primitives (M_CTL, M_PROTO, M_PCPROTO)
 *
 *  =========================================================================
 *
 *  These functions package up the necessary primitive format given the
 *  input parameters necessary to create them.  They are all inlined and
 *  return NULL if a message block could not be allocated.  The reason for not
 *  passing the mblk up the queue yet is because the caller may wish to
 *  allocate the response before committing to processing the action which
 *  causes these indications and confirmations to the T-User.
 */

/*
 *  T_INFO_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_info_ack(q)
	const queue_t *q;
{
	mblk_t *mp;
	struct T_info_ack *p;
	sscop_t *sp = SSCOP_PRIV(q);
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_info_ack * p) mp->b_wptr;
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

/*
 *  T_OK_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_ok_ack(prim)
	const int prim;
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

/*
 *  T_ERROR_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_error_ack(prim, err)
	const int prim;
	const int err;
{
	mblk_t *mp;
	struct T_error_ack *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = err > 0 ? err : TSYSERR;
		p->UNIX_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_OPTMGMT_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_optmgmt_ack(opt_ptr, opt_len, flags)
	const caddr_t opt_ptr;
	const size_t opt_len;
	const uint flags;
{
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	if ((mp = allocb(sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
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

/*
 *  T_ADDR_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_addr_ack(q)
	const queue_t *q;
{
	mblk_t *mp;
	struct T_addr_ack *p;
	sscop_t *sp = SSCOP_PRIV(q);

	size_t bnd_len = sizeof(u16) + sp->banum * sizeof(u32);
	size_t loc_len = sizeof(u16) + sp->sanum * sizeof(u32);
	size_t rem_len = sizeof(u16) + sp->danum * sizeof(u32);

	struct sscop_baddr *sb = sp->baddr;
	struct sscop_saddr *ss = sp->saddr;
	struct sscop_daddr *ss = sp->daddr;

	u16 loc_port = sp->bport;
	u16 rem_port = sp->dport;

	if (ss) {
		bnd_len = 0;
		sb = NULL;
		loc_port = sp->sport;
	}

	loc_len += bnd_len;

	if ((mp = allocb(sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		struct sscop_saddr *ss;
		struct sscop_daddr *sd;

		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
		mp->b_wptr += sizeof(*p);

		*((u16 *) mp->b_wptr)++ = loc_port;
		for (; sb; sb = sb->next)
			*((u32 *) mp->b_wptr)++ = sb->baddr;
		for (; ss; ss = ss->next)
			*((u32 *) mp->b_wptr)++ = ss->saddr;

		*((u16 *) mp->b_wptr)++ = rem_port;
		for (; sd; sd = sd->next)
			*((u32 *) mp->b_wptr)++ = sd->daddr;
	}
	return (mp);
}

/*
 *  T_BIND_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_bind_ack(q)
	const queue_t *q;
{
	mblk_t *mp;
	struct T_bind_ack *p;
	sscop_t *sp = SSCOP_PRIV(q);
	size_t add_len = sizeof(u16) + sp->sanum * sizeof(u32);

	if ((mp = allocb(sizeof(*p) + add_len, BPRI_MED))) {
		struct sscop_baddr *sb = sp->baddr;

		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = sp->cons;
		mp->b_wptr += sizeof(*p);

		*((u16 *) mp->b_wptr)++ = sp->bport;
		for (; sb; sb = sb->next)
			*((u32 *) mp->b_wptr)++ = sb->baddr;
	}
	return (mp);
}

/*
 *  T_CONN_IND
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_conn_ind(ck)
	const struct sscop_cookie *ck;
{
	mblk_t *mp;
	struct T_conn_ind *p;
	struct t_opthdr *oh;

	size_t dta_num = ck->dta_num;
	u32 *dta_ptr = (u32 *) (((caddr_t) (ck + 1)) + ck->opt_len);

	const size_t dst_len = sizeof(u16) + dta_num * sizeof(u32);
	static const size_t opt_len = 2 * PADC(sizeof(*oh) + sizeof(u16));

	if ((mp = allocb(sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_CONN_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);

		*((u16 *) mp->b_wptr)++ = ck->dport;
		while (dta_num--)
			*((u32 *) mp->b_wptr)++ = *dta_ptr++;

		oh = (struct t_opthdr *) mp->b_wptr;
		oh->level = T_INET_SSCOP;
		oh->name = SSCOP_I_STREAMS;
		oh->len = sizeof(*oh) + sizeof(u16);
		mp->b_wptr += sizeof(*oh);
		*((u16 *) mp->b_wptr)++ = ck->i_strs;
		*((u16 *) mp->b_wptr)++ = 0;

		oh = (struct t_opthdr *) mp->b_wptr;
		oh->level = T_INET_SSCOP;
		oh->name = SSCOP_O_STREAMS;
		oh->len = sizeof(*oh) + sizeof(u16);
		mp->b_wptr += sizeof(*oh);
		*((u16 *) mp->b_wptr)++ = ck->o_strs;
		*((u16 *) mp->b_wptr)++ = 0;

	}
	return (mp);
}

/*
 *  T_CONN_CON
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_conn_con(q)
	const queue_t *q;
{
	mblk_t *mp;
	struct T_conn_con *p;
	struct t_opthdr *oh;
	sscop_t *sp = SSCOP_PRIV(q);

	const size_t res_len = sizeof(u16) + sp->danum * sizeof(u32);
	const size_t opt_len = 2 * PADC(sizeof(*oh) + sizeof(u16));

	if ((mp = allocb(sizeof(*p) + res_len + opt_len, BPRI_MED))) {
		struct sscop_daddr *sd;

		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_CONN_CON;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + res_len : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);

		*((u16 *) mp->b_wptr)++ = sp->dport;
		for (sd = sp->daddr; sd; sd = sd->next)
			*((u32 *) mp->b_wptr)++ = sd->daddr;

		oh = (struct t_opthdr *) mp->b_wptr;
		oh->level = T_INET_SSCOP;
		oh->name = SSCOP_I_STREAMS;
		oh->len = sizeof(*oh) + sizeof(u16);
		mp->b_wptr += sizeof(*oh);
		*((u16 *) mp->b_wptr)++ = sp->p_istr;
		*((u16 *) mp->b_wptr)++ = 0;

		oh = (struct t_opthdr *) mp->b_wptr;
		oh->level = T_INET_SSCOP;
		oh->name = SSCOP_O_STREAMS;
		oh->len = sizeof(*oh) + sizeof(u16);
		mp->b_wptr += sizeof(*oh);
		*((u16 *) mp->b_wptr)++ = sp->p_ostr;
		*((u16 *) mp->b_wptr)++ = 0;
	}
	return (mp);
}

/*
 *  T_DATA_IND
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_data_ind(flag, dp)
	const uint flag;
	const mblk_t *dp;
{
	mblk_t *mp;
	struct T_data_ind *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_DATA_IND;
		p->MORE_flag = flag;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
	}
	return (mp);
}

/*
 *  T_EXDATA_IND
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_exdata_ind(flag, dp)
	const uint flag;
	const mblk_t *dp;
{
	mblk_t *mp;
	struct T_exdata_ind *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_EXDATA_IND;
		p->MORE_flag = flag;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_UNITDATA_IND
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_unitdata_ind(caddr_t src_ptr, size_t src_len, caddr_t opt_ptr,
					 size_t opt_len)
{
	mblk_t *mp;
	struct T_unitdata_ind *p;
	if ((mp = allocb(sizeof(*p) + src_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
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

/*
 *  T_UDERROR_IND
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_uderror_ind(caddr_t dst_ptr, size_t dst_len,
					caddr_t opt_ptr, size_t opt_len, int type)
{
	mblk_t *mp;
	struct T_uderror_ind *p;
	if ((mp = allocb(sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
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

/*
 *  T_DISCON_IND
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_discon_ind(reason, seq)
	const int reason;
	const uint seq;
{
	mblk_t *mp;
	struct T_discon_ind *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_DISCON_IND;
		p->DISCON_reason = reason;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  T_ORDREL_IND
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *t_ordrel_ind(void)
{
	mblk_t *mp;
	struct T_ordrel_ind *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (struct T_ok_ack *) mp->b_wptr;
		p->PRIM_type = T_ORDREL_IND;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

#endif				/* __SSCOP_T_PROV_H__ */
