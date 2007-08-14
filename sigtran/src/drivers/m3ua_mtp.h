/*****************************************************************************

 @(#) $Id: m3ua_mtp.h,v 0.9.2.3 2007/08/14 08:33:54 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 08:33:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m3ua_mtp.h,v $
 Revision 0.9.2.3  2007/08/14 08:33:54  brian
 - GPLv3 header update

 Revision 0.9.2.2  2007/06/17 02:00:50  brian
 - updates for release, remove any later language

 Revision 0.9.2.1  2006/10/17 11:55:42  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.1  2004/08/21 10:14:44  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:20:21  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.1  2002/10/18 03:27:43  brian
 Indentation changes only.

 Revision 0.8  2002/04/02 08:20:42  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/07/29 10:46:23  brian
 Split up M3UA files.

 *****************************************************************************/

#ifndef __M3UA_MTP_H__
#define __M3UA_MTP_H__

#ident "@(#) $RCSfile: m3ua_mtp.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 *  =========================================================================
 *
 *  M3UA --> MTPP (Downstream Primtiives to MTP Provider)
 *
 *  =========================================================================
 */
/*
 *  MTP_INFO_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_info_req(void)
{
	mblk_t *mp;
	m3_info_req_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (m3_info_req_t *) mp->b_wptr;
		p->primitive = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  MTP_BIND_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_bind_req(src_ptr, src_len)
	const caddr_t src_ptr;
	const size_t src_len;
{
	mblk_t *mp;
	mtp_bind_req_t *p;

	if ((mp = allocb(sizeof(*p) + src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (mtp_bind_req_t *) mp->b_wptr;
		p->primitive = N_BIND_REQ;
		mp->b_wptr += sizeof(*p);
		/* FIXME: this is probably not correct */
		bcopy(src_ptr, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
	}
	return (mp);
}

/*
 *  MTP_UNBIND_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_unbind_req(void)
{
	mblk_t *mp;
	m3_unbind_req_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (m3u_unbind_req_t *) mp->b_wptr;
		p->primitive = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  MTP_CONN_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_conn_req(dst_ptr, dst_len)
	const caddr_t dst_ptr;
	const size_t dst_len;
{
	mblk_t *mp;
	mtp_conn_req_t *p;

	if ((mp = allocb(sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (mtp_conn_req_t *) mp->b_wptr;
		p->primitive = N_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		/* FIXME: this is probably not correct */
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
	}
	return (mp);
}

/*
 *  MTP_DISCON_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mpt_discon_req(void)
{
	mblk_t *mp;
	mtp_discon_req_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (mtp_discon_req_t *) mp->b_wptr;
		p->primitive = N_DISCON_REQ;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  MTP_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_optmgmt_req(opt_ptr, opt_len, flags)
	const caddr_t opt_ptr;
	const size_t opt_len;
	const uint flags;
{
	mblk_t *mp;
	mtp_optmgmt_req_t *p;

	if ((mp = allocb(sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (mtp_optmgmt_req_t *) mp->b_wptr;
		p->primitive = N_OPTMGMT_REQ;
		p->opt_length = opt_len;
		p->opt_offset = opt_len ? sizeof(*p) : 0;
		p->mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
	}
	return (mp);
}

/*
 *  MTP_NOTIFY_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_notify_req(not_ptr, not_len, flags)
	const caddr_t not_ptr;
	const size_t not_len;
	const uint flags;
{
	mblk_t *mp;
	mtp_notify_ind_t *p;

	if ((mp = allocb(sizeof(*p) + not_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (mtp_notify_req_t *) mp->b_wptr;
		p->primtiive = N_NOTIFY_IND;
		p->not_length = not_len;
		p->not_offset = not_len ? sizeof(*p) : 0;
		p->mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(not_ptr, mp->b_wptr, not_len);
		mp->b_wptr += not_len;
	}
	return (mp);
}

/*
 *  MTP_TRANSFER_REQ
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_transfer_req(opc, dpc, si, sls, mp, dp)
	const uint32_t opc;
	const uint32_t dpc;
	const uint si;
	const uint sls;
	const uint mp;
	const mblk_t *dp;
{
	mblk_t *mp;
	mtp_transfer_req_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (mtp_transfer_req_t *) mp->b_wptr;
		p->primitive = N_UNITDATA_REQ;
		p->opc = opc;
		p->dpc = dpc;
		p->si = si;
		p->sls = sls;
		p->mp = mp;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
	}
	return (mp);
}

/*
 *  =========================================================================
 *
 *  M3UA --> MTPU (Upstream Primitives to MTP User) (Not handled by translator)
 *
 *  =========================================================================
 */
/*
 *  MTP_INFO_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_info_ack(q)
	const queue_t *q;
{
	mblk_t *mp;
	mtpu_t *mu = (mtpu_t *) q->q_ptr;
	mtp_info_ack_t *p;

	if ((mp = allocb(sizeof(*p) + mu->src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (mtp_info_ack_t *) mp->b_wptr;
		p->primitive = N_INFO_ACK;
		/* FIXME: all this is probably not correct */
		p->version = mu->version;
		p->state = mu->state;
		p->max_sdu = mu->max_sdu;
		p->min_sdu = mu->min_sdu;
		p->header_len = mu->header_len;
		p->style = mu->style;
		mp->b_wptr += sizeof(*p);
		bcopy(mu->src, mp->b_wptr, mu->src_len);
		mp->b_wptr += src_len;
	}
	return (mp);
}

/*
 *  MTP_OK_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_ok_ack(prim)
	const ulong prim;
{
	mblk_t *mp;
	mtp_ok_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (mtp_ok_ack_t *) mp->b_wptr;
		p->primitive = N_OK_ACK;
		p->correct_prim = prim;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  MTP_ERROR_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_error_ack(prim, err)
	const ulong prim;
	const long err;
{
	mblk_t *mp;
	mtp_error_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (mtp_error_ack_t *) mp->b_wptr;
		p->primitive = N_ERROR_ACK;
		p->errno = err > 0 ? err : MTP_SYSERR;
		p->reason = err < 0 ? -err : 0;
		p->error_prim = prim;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  MTP_BIND_ACK
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_bind_ack(void)
{
	mblk_t *mp;
	N_bind_ack_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_bind_ack_t *) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = 0;
		p->TOKEN_value = token;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
		mp->b_wptr += sizeof(*p);
	}
	return (mp);
}

/*
 *  MTP_CONN_CON
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_DISCON_IND
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_OPTMGMT_ACK
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_NOTIFY_IND
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_UDERROR_IND
 *  -------------------------------------------------------------------------
 */
static __inline__ mblk_t *
mtp_uderror_ind(uint ecode, uint32_t apc, uint si, uint mp)
{
	mblk_t *mp;
	N_uderror_ind_t *p;

	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_uderror_ind_t *) mp->b_wptr;
		p->PRIM_type = N_UDERROR_IND;
		p->DEST_length = sizeof(uint32_t) * 2;
		p->DEST_offset = sizeof(*p);
		p->RESERVED_field = 0;
		p->ERROR_type = ecode;
		mp->b_wptr += sizeof(*p);
		*((uint32_t *) mp->b_wptr)++ = apc;	/* affected point code */
		*((uint32_t *) mp->b_wptr)++ = si;	/* MTP-User */
		*((uint32_t *) mp->b_wptr)++ = mp;	/* Congestion Level */
	}
	return (mp);
}

/*
 *  FIXME: Maybe some more...
 */

#endif				/* __M3UA_MTP_H__ */
