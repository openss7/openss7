/*****************************************************************************

 @(#) $Id: sscop_n_prov.h,v 0.9.2.5 2008-04-29 07:11:13 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 Last Modified $Date: 2008-04-29 07:11:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sscop_n_prov.h,v $
 Revision 0.9.2.5  2008-04-29 07:11:13  brian
 - updating headers for release

 Revision 0.9.2.4  2007/08/14 12:18:51  brian
 - GPLv3 header updates

 Revision 0.9.2.3  2007/06/17 01:56:28  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SSCOP_N_PROV_H__
#define __SSCOP_N_PROV_H__

#ident "@(#) $RCSfile: sscop_n_prov.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

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

/*
 *  N_INFO_ACK
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_info_ack(q)
	const queue_t *q;
{
	mblk_t *mp;
	N_info_ack_t *p;
	N_qos_co_opt_sel_t *qos;
	N_qos_co_opt_range_t *rng;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	static const size_t add_len = 0;	/* TODO */
	static const size_t qos_len = sizeof(*qos);
	static const size_t rng_len = sizeof(*rng);

	if ((mp = allocb(sizeof(*p) + add_len + qos_len + rng_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_info_ack_t *) mp->b_wptr;
		p->PRIM_type = N_INFO_ACK;
		p->NSDU_size = -1;
		p->ENSDU_size = -1;
		p->CDATA_size = -1;
		p->DDATA_size = 0;
		p->ADDR_size = -1;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + add_len : 0;
		p->QOS_range_length = rng_len;
		p->QOS_range_offset = rng_len ? sizeof(*p) + add_len + qos_len : 0;
		p->OPTIONS_flags = FIXME;
		p->NIDU_size = -1;
		p->SERV_type = N_CONS;
		p->CURRENT_state = sp->n_state;
		p->PROVIDER_type = N_SNICFP;
		mp->b_wptr += sizeof(*p);

		qos = (N_qos_co_opt_sel_t *) mp->b_wptr;
		qos->n_qos_type = N_QOS_CO_OPT_SEL;
		qos->src_throughput.thru_targ_value = FIXME;
		qos->src_throughput.thru_min_value = FIXME;
		qos->dest_throughput.thru_targ_value = FIXME;
		qos->dest_throughput.thru_min_value = FIXME;
		qos->transit_delay_t.td_targ_value = FIXME;
		qos->transit_delay_t.td_max_value = FIXME;
		qos->nc_estab_delay = FIXME;
		qos->nc_establ_fail_prob = FIXME;
		qos->residual_error_rate = FIXME;
		qos->xfer_fail_prob = FIXME;
		qos->nc_resilience = FIXME;
		qos->nc_relay = FIXME;
		qos->nc_rel_fail_prob = FIXME;
		qos->protection_sel = FIXME;
		qos->priority_sel = FIXME;
		qos->max_accept_cost = FIXME;
		mp->b_wptr += sizeof(*qos);

		rng = (N_qos_co_opt_range_t *) mp->b_wptr;
		rng->n_qos_type = N_QOS_CO_OPT_RANGE;
		rng->src_throughput.thru_targ_value = FIXME;
		rng->src_throughput.thru_min_value = FIXME;
		rng->dest_throughput.thru_targ_value = FIXME;
		rng->dest_throughput.thru_min_value = FIXME;
		rng->transit_delay_t.td_targ_value = FIXME;
		rng->transit_delay_t.td_max_value = FIXME;
		rng->nc_estab_delay = FIXME;
		rng->nc_establ_fail_prob = FIXME;
		rng->residual_error_rate = FIXME;
		rng->xfer_fail_prob = FIXME;
		rng->nc_resilience = FIXME;
		rng->nc_rel_delay = FIXME;
		rng->nc_rel_fail_prob = FIXME;
		rng->protection_range.protect_targ_value = FIXME;
		rng->protection_range.protect_min_value = FIXME;
		rng->priority_range.priority_targ_value = FIXME;
		rng->priority_range.priority_min_value = FIXME;
		rng->max_accept_cost = FIXME;
		mp->b_wptr += sizeof(*rng);
	}
	return (mp);
}

/*
 *  N_OK_ACK
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_ok_ack(prim)
	const int prim;
{
	mblk_t *mp;
	N_ok_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_ok_ack_t *) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_ERROR_ACK
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_error_ack(prim, err)
	const int prim;
	const int err;
{
	mblk_t *mp;
	N_error_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_error_ack_t *) mp->b_wptr;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = err > 0 ? err : NSYSERR;
		p->UNIX_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_BIND_ACK
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_bind_ack(q)
	const queue_t *q;
{
	mblk_t *mp;
	N_bind_ack_t *p;
	sscop_t *sp = (sscop_t *) q->q_ptr;
	size_t add_len = sizeof(u16) + sp->banum * sizeof(u32);

	if ((mp = allocb(sizeof(*p) + add_len, BPRI_MED))) {
		struct sscop_baddr *sb = sp->baddr;

		mp->b_datap->db_type = M_PCPROTO;
		p = (N_bind_ack_t *) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = sp->cons;
		p->TOKEN_value = sp->qid;
		mp->b_wptr += sizeof(*p);

		*((u16 *) mp->b_wptr)++ = sp->bport;
		for (; sb; sb = sb->next)
			*((u32 *) mp->b_wptr)++ = sb->baddr;
	}
	return (mp);
}

/*
 *  N_CONN_IND
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_conn_ind(ck)
	const struct sscop_cookie *ck;
{
	mblk_t *mp;
	N_conn_ind_t *p;
	N_qos_co_range_t *qos;

	size_t dta_num = ck->dta_num;
	size_t sta_num = ck->sta_num;
	u32 *dta_ptr = (u32 *) (((caddr_t) (ck + 1)) + ck->opt_len);
	u32 *sta_ptr = dta_ptr + dta_num;

	const size_t dst_len = sizeof(u16) + dta_num * sizeof(u32);
	const size_t src_len = sizeof(u16) + sta_num * sizeof(u32);
	static const size_t qos_len = sizeof(*qos);

	if ((mp = allocb(sizeof(*p) + dst_len + src_len + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_conn_ind_t *) mp->b_wptr;
		p->PRIM_type = N_CONN_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->SEQ_number = ck->seq;
		p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
		mp->b_wptr += sizeof(*p);

		*((u16 *) mp->b_wptr)++ = ck->dport;
		while (dta_num--)
			*((u32 *) mp->b_wptr)++ = *dta_ptr++;

		*((u16 *) mp->b_wptr)++ = ck->sport;
		while (sta_num--)
			*((u32 *) mp->b_wptr)++ = *sta_ptr++;

		qos = (N_qos_co_range_t *) mp->b_wptr;
		qos->n_qos_type = N_QOS_CO_RANGE;
		qos->src_throughput_range.thru_targ_value = FIXME;
		qos->src_throughput_range.thru_min_value = FIXME;
		qos->dest_throughput_range.thru_targ_value = FIXME;
		qos->dest_throughput_range.thru_min_value = FIXME;
		qos->transit_delay_range.td_targ_value = FIXME;
		qos->transit_delay_range.td_min_value = FIXME;
		qos->protection_range.protect_targ_value = FIXME;
		qos->protection_range.protect_min_value = FIXME;
		qos->priority_range.priority_targ_value = FIXME;
		qos->priority_range.priority_min_value = FIXME;
		mp->b_wptr += sizeof(*qos);

	}
	return (mp);
}

/*
 *  N_CONN_CON
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_conn_con(q)
	const queue_t *q;
{
	mblk_t *mp;
	N_conn_con_t *p;
	N_qos_co_range_t *qos;
	sscop_t *sp = (sscop_t *) q->q_ptr;

	const size_t res_len = sizeof(u16) + sp->danum * sizeof(u32);
	static const size_t qos_len = sizeof(*qos);

	if ((mp = allocb(sizeof(*p) + res_len + qos_len, BPRI_MED))) {
		struct sscop_daddr *sd;

		mp->b_datap->db_type = M_PCPROTO;
		p = (N_conn_con_t *) mp->b_wptr;
		p->PRIM_type = N_CONN_CON;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
		mp->b_wptr += sizeof(*p);

		*((u16 *) mp->b_wptr)++ = sp->dport;
		for (sd = sp->daddr; sd; sd = sd->next)
			*((u32 *) mp->b_wptr)++ = sd->daddr;

		qos = (N_qos_co_range_t *) mp->b_wptr;
		qos->n_qos_type = N_QOS_CO_RANGE;
		qos->src_throughput_range.thru_targ_value = FIXME;
		qos->src_throughput_range.thru_min_value = FIXME;
		qos->dest_throughput_range.thru_targ_value = FIXME;
		qos->dest_throughput_range.thru_min_value = FIXME;
		qos->transit_delay_range.td_targ_value = FIXME;
		qos->transit_delay_range.td_min_value = FIXME;
		qos->protection_range.protect_targ_value = FIXME;
		qos->protection_range.protect_min_value = FIXME;
		qos->priority_range.priority_targ_value = FIXME;
		qos->priority_range.priority_min_value = FIXME;
		mp->b_wptr += sizeof(*qos);
	}
	return (mp);
}

/*
 *  N_DATA_IND
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_data_ind(flags, dp)
	const uint flags;
	const mblk_t *dp;
{
	mblk_t *mp;
	N_data_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_data_ind_t *) mp->b_wptr;
		p->PRIM_type = N_DATA_IND;
		p->DATA_xfer_flags = flags;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
	}
	return (mp);
}

/*
 *  N_DATACK_IND
 */
static inline mblk_t *
n_datack_ind(void)
{
	mblk_t *mp;
	N_datack_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_datack_ind_t *) mp->b_wptr;
		p->PRIM_type = N_DATACK_IND;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_EXDATA_IND
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_exdata_ind(dp)
	const mblk_t *dp;
{
	mblk_t *mp;
	N_exdata_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_exdata_ind_t *) mp->b_wptr;
		p->PRIM_type = N_EXDATA_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
	}
	return (mp);
}

/*
 *  N_UNITDATA_IND
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_unitdata_ind(caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len)
{
	mblk_t *mp;
	N_unitdata_ind_t *p;

	if ((mp = allocb(sizeof(*p) + dst_len + src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_unitdata_ind_t *) mp->b_wptr;
		p->PRIM_type = N_UNITDATA_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->RESERVED_field = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += src_len;
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	return (mp);
}

/*
 *  N_UDERROR_IND
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_uderror_ind(caddr_t dst_ptr, size_t dst_len, int type)
{
	mblk_t *mp;
	N_uderror_ind_t *p;

	if ((mp = allocb(sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_uderror_ind_t *) mp->b_wptr;
		p->PRIM_type = N_UDERROR_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->RESERVED_field = 0;
		p->ERROR_type = type;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	return (mp);
}

/*
 *  N_RESET_IND
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_reset_ind(orig, reason)
	const uint orig;
	const int reason;
{
	mblk_t *mp;
	N_reset_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_reset_ind_t *) mp->b_wptr;
		p->PRIM_type = N_RESET_IND;
		p->RESET_orig = orig;
		p->RESET_reason = reason;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_RESET_CON
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_reset_con(void)
{
	mblk_t *mp;
	N_reset_con_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_reset_con_t *) mp->b_wptr;
		p->PRIM_type = N_RESET_CON;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_DISCON_IND
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *
n_discon_ind(orig, reason, seq)
	const uint orig;
	const int reason;
	const uint seq;
{
	mblk_t *mp;
	N_discon_ind_t *p;
	static const size_t res_len = 0;	/* TODO */

	if ((mp = allocb(sizeof(*p) + res_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_discon_ind_t *) mp->b_wptr;
		p->PRIM_type = N_DISCON_IND;
		p->DISCON_orig = orig;
		p->DISCON_reason = reason;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

#endif				/* __SSCOP_N_PROV_H__ */
