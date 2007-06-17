/*****************************************************************************

 @(#) $Id: sua_sccp.h,v 0.9.2.2 2007/06/17 01:56:31 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/06/17 01:56:31 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sua_sccp.h,v $
 Revision 0.9.2.2  2007/06/17 01:56:31  brian
 - updates for release, remove any later language

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

#ifndef __SUA_SCCP_H__
#define __SUA_SCCP_H__

#ident "@(#) $RCSfile: sua_sccp.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 *  =========================================================================
 *
 *  SUA --> SCCP Provider (Downstream Primitives to SCCP Provider)
 *
 *  =========================================================================
 *
 *  These are basic inlines for formulating NPI downstream primitives.  They
 *  are completely general and are called by the message translators.
 */
/*
 *  N_CONN_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_conn_req(flags, dst_ptr, dst_len, qos_ptr, qos_len)
	uint flags;
	caddr_t dst_ptr;
	size_t dst_len;
	caddr_t qos_ptr;
	size_t qos_len;
{
	mblk_t *mp;
	N_conn_req_t *p;
	if ((mp = allocb(sizeof(*p) + dst_len + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_conn_req_t *) mp->b_wptr;
		p->primitive = N_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->CONN_flags = flags;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + dst_len : 0;
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
static __inline__ mblk_t *n_conn_res(tok, seq, flags, res_ptr, res_len, qos_ptr, qos_len)
	uint tok;
	uint seq;
	uint flags;
	caddr_t res_ptr;
	size_t res_len;
	caddr_t qos_ptr;
	size_t qos_len;
{
	mblk_t *mp;
	N_conn_res_t *p;
	if ((mp = allocb(sizeof(*p) + res_len + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_conn_res_t *) mp->b_wptr;
		p->primitive = N_CONN_RES;
		p->TOKEN_value = tok;
		p->RES_length = res_len;
		p->RES_offset = resl_len ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		p->CONN_flags = flags;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(res_ptr, mp->b_wptr, res_len);
		mp->b_wptr += res_len;
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
	}
	return (mp);
}

/*
 *  N_DISCON_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_discon_req(reason, seq, res_ptr, res_len)
	uint reason;
	uint seq;
	caddr_t res_ptr;
	size_t res_len;
{
	mblk_t *mp;
	N_discon_req_t *p;
	if ((mp = allocb(sizeof(*p) + res_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_discon_req_t *) mp->b_wptr;
		p->primitive = N_DISCON_REQ;
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
 *  N_DATA_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_data_req(uint flags, mblk_t * db)
{
	mblk_t *mp;
	N_data_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_data_req_t *) mp->b_wptr;
		p->primitive = N_DATA_REQ;
		p->DATA_xfer_flags = flags;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = db;
	}
	return (mp);
}

/*
 *  N_EXDATA_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_exdata_req(mblk_t * db)
{
	mblk_t *mp;
	N_exdata_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (N_exdata_req_t *) mp->b_wptr;
		p->primitive = N_EXDATA_REQ;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = db;
	}
	return (mp);
}

/*
 *  N_INFO_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_info_req(void)
{
	mblk_t *mp;
	N_info_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_info_req_t *) mp->b_wptr;
		p->primitive = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_BIND_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_bind_req(flags, conind, add_ptr, add_len, pro_ptr, pro_len)
	uint flags;
	uint conind;
	caddr_t addr_ptr;
	size_t add_len;
	caddr_t pro_ptr;
	size_t pro_len;
{
	mblk_t *mp;
	N_bind_req_t *p;
	if ((mp = allocb(sizeof(*p) + add_len + pro_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_bind_req_t *) mp->b_wptr;
		p->primitive = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = conind;
		p->BIND_flags = flags;
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

/*
 *  N_UNBIND_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_unbind_req(void)
{
	mblk_t *mp;
	N_unbind_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_unbind_req_t *) mp->b_wptr;
		p->primitive = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_UNITDATA_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_unitdata_req(dst_ptr, dst_len, db)
	caddr_t dst_ptr;
	size_t dst_len;
	mblk_t *db;
{
	mblk_t *mp;
	N_unitdata_req_t *p;
	if ((mp = allocb(sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_unitdata_req_t *) mp->b_wptr;
		p->primitive = N_UNITDATA_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->RESERVED_field[0] = 0;
		p->RESERVED_field[1] = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		mp->b_cont = db;
	}
	return (mp);
}

/*
 *  N_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_optmgmt_req(flags, qos_ptr, qos_len)
	uint flags;
	caddr_t qos_ptr;
	size_t qos_len;
{
	mblk_t *mp;
	N_optmgmt_req_t *p;
	if ((mp = allocb(sizeof(*p) + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_optmgmt_req_t *) mp->b_wptr;
		p->primitive = N_OPTMGMT_REQ;
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
 *  N_DATACK_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_datack_req(void)
{
	mblk_t *mp;
	N_datack_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_datack_req_t *) mp->b_wptr;
		p->primitive = N_DATACK_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_RESET_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_reset_req(uint reason)
{
	mblk_t *mp;
	N_reset_req_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_reset_req_t *) mp->b_wptr;
		p->primitive = N_RESET_REQ;
		p->RESET_reason = reason;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  N_RESET_RES
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *n_reset_res(void)
{
	mblk_t *mp;
	N_reset_res_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_reset_res_t *) mp->b_wptr;
		p->primitive = N_RESET_RES;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

#endif				/* __SUA_SCCP_H__ */
