/*****************************************************************************

 @(#) $RCSfile: sscop_n_user.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/06/17 01:56:28 $

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

 Last Modified $Date: 2007/06/17 01:56:28 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sscop_n_user.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/06/17 01:56:28 $"

static char const ident[] =
    "$RCSfile: sscop_n_user.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/06/17 01:56:28 $";

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

/*
 *  N_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 *  There isn't much point in this for IP except to set the TOS bits in the IP
 *  header and to indicate whether to set the DF bit to frag or not.  Also,
 *  default IP options could be set here.
 */
static inline mblk_t *n_optmgmt_req(caddr_t qos_ptr, size_t qos_len, uint flags)
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
static inline mblk_t *n_bind_req(void)
	static inline mblk_t *n_bind_req(caddr_t add_ptr, size_t add_len, int cons,
					 uint flags, caddr_t pro_ptr, size_t pro_len)
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

/*
 *  N_CONN_REQ
 *  -------------------------------------------------------------------------
 */
static inline mblk_t *n_conn_req(caddr_t dst_ptr, size_t dst_len, uint flags, caddr_t qos_ptr,
				 size_t qos_len)
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
static inline mblk_t *n_conn_res(queue_t * q, caddr_t res_ptr, size_t res_len,
				 uint flags, caddr_t qos_ptr, size_t qos_len)
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
static inline mblk_t *n_data_req(uint flags)
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
static inline mblk_t *n_datack_req(void)
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
static inline mblk_t *n_exdata_req(void)
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
static inline mblk_t *n_reset_req(int reason)
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
static inline mblk_t *n_reset_con(void)
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
static inline mblk_t *n_discon_req(int reason, caddr_t res_ptr, size_t res_len, uint seq)
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
static inline mblk_t *n_unitdata_req(caddr_t dst_ptr, size_t dst_len)
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
