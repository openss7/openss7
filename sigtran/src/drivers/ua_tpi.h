/*****************************************************************************

 @(#) $Id: ua_tpi.h,v 0.9.2.1 2006/11/30 13:17:55 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2006/11/30 13:17:55 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ua_tpi.h,v $
 Revision 0.9.2.1  2006/11/30 13:17:55  brian
 - added files from strss7 package

 *****************************************************************************/

#ifndef __LOCAL_UA_TPI_H__
#define __LOCAL_UA_TPI_H__

#ident "@(#) $RCSfile: ua_tpi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/*
 *  TPI User Primitives (to SS User)
 *  --------------------------------------------------------------------------
 *  Transport Provider Interface primitives can be used for MTP, SCCP, TCAP-TR and TC, and even
 *  SCTP.  The ss pointer here could be a linked transport structure.
 */
/**
 * t_conn_ind: - issued T_CONN_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @src_ptr: srouce address pointer
 * @src_len: source address length
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @seq: sequence number
 * @dp: user data
 */
STATIC INLINE int
t_conn_ind(struct ss *ss, queue_t *q, caddr_t src_ptr, size_t src_len, caddr_t opt_ptr,
	   size_t opt_len, t_uscalar_t seq, mblk_t *dp)
{
	mblk_t *mp;
	struct T_conn_ind *p;
	size_t mlen = sizeof(*p) + src_len + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_IND;
			p->SRC_length = src_len;
			p->SRC_offset = src_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- T_CONN_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_conn_con: - issued T_CONN_CON primitive upstream
 * @ss: private structure
 * @q: active queue
 * @res_ptr: responding address pointer
 * @res_len: responding address length
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dp: user data
 */
STATIC INLINE int
t_conn_con(struct ss *ss, queue_t *q, caddr_t res_ptr, size_t res_len, caddr_t opt_ptr,
	   size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct T_conn_con *p;
	size_t mlen = sizeof(*p) + res_len + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + res_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(res_ptr, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- T_CONN_CON\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_discon_ind: - issued T_DISCON_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @reason: disconnect reason
 * @seq: sequence number
 * @dp: user data
 */
STATIC INLINE int
t_discon_ind(struct ss *ss, queue_t *q, t_scalar_t reason, t_scalar_t seq, mblk_t *dp)
{
	mblk_t *mp;
	struct T_discon_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DISCON_IND;
			p->DISCON_reason = reason;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- T_DISCON_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_data_ind: - issued T_DATA_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @more: more flag
 * @dp: user data
 */
STATIC INLINE int
t_data_ind(struct ss *ss, queue_t *q, t_scalar_t more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DATA_IND;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- T_DATA_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_exdata_ind: - issued T_EXDATA_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @more: more flag
 * @dp: user data
 */
STATIC INLINE int
t_exdata_ind(struct ss *ss, queue_t *q, t_scalar_t more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_exdata_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_EXDATA_IND;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- T_EXDATA_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_info_ack: - issued T_INFO_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
t_info_ack(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct T_info_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = XXX;
		p->ETSDU_size = XXX;
		p->CDATA_size = XXX;
		p->DDATA_size = XXX;
		p->ADDR_size = XXX;
		p->OPT_size = XXX;
		p->TIDU_size = XXX;
		p->SERV_type = XXX;
		p->CURRENT_state = XXX;
		p->PROVIDER_flag = XXX;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- T_INFO_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * t_bind_ack: - issued T_BIND_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 * @coninds: maximum outstanding number of connection indications
 */
STATIC INLINE int
t_bind_ack(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len, t_scalar_t coninds)
{
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = coninds;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		printd(("%s: %p: <- T_BIND_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * t_error_ack: - issued T_ERROR_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: primitive in error
 * @error: positive TPI error negative UNIX error
 */
STATIC INLINE int
t_error_ack(struct ss *ss, queue_t *q, t_scalar_t prim, t_scalar_t error)
{
	mblk_t *mp;
	struct T_error_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ERROR_ACK;
			p->ERROR_prim = prim;
			p->TLI_error = error < 0 ? TSYSERR : 0;
			p->UNIX_error = error < 0 ? -error : 0;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- T_ERROR_ACK\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_ok_ack: - issued T_OK_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: correct primitive
 */
STATIC INLINE int
t_ok_ack(struct ss *ss, queue_t *q, t_scalar_t prim)
{
	mblk_t *mp;
	struct T_ok_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OK_ACK;
			p->CORRECT_prim = prim;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- T_OK_ACK\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_unitdata_ind: - issued T_UNITDATA_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @src_ptr: source address pointer
 * @src_len: source address length
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dp: user data
 */
STATIC INLINE int
t_unitdata_ind(struct ss *ss, queue_t *q, caddr_t src_ptr, size_t src_len, caddr_t opt_ptr,
	       size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct T_unitdata_ind *p;
	size_t mlen = sizeof(*p) + src_len + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
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
			mp->b_cont = dp;
			printd(("%s: %p: <- T_UNITDATA_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_uderror_ind: - issued T_UDERROR_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dst_ptr: destination address pointer
 * @dst_len: destination address length
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @error: error type
 * @dn: user data
 */
STATIC INLINE int
t_uderror_ind(struct ss *ss, queue_t *q, caddr_t dst_ptr, size_t dst_len, caddr_t opt_ptr,
	      size_t opt_len, t_scalar_t error, mblk_t *dp)
{
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t mlen = sizeof(*p) + dst_len + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
			p->ERROR_type = error;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- T_UDERROR_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_optmgmt_ack: - issued T_OPTMGMT_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @flags: management flags
 */
STATIC INLINE int
t_optmgmt_ack(struct ss *ss, queue_t *q, caddr_t opt_ptr, size_t opt_len, t_scalar_t flags)
{
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- T_OPTMGMT_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * t_ordrel_ind: - issued T_ORDREL_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dp: user data
 */
STATIC INLINE int
t_ordrel_ind(struct ss *ss, queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	struct T_ordrel_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ORDREL_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- T_ORDREL_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_optdata_ind: - issued T_OPTDATA_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @flag: data transfer flags
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dp: user data
 */
STATIC INLINE int
t_optdata_ind(struct ss *ss, queue_t *q, t_scalar_t flags, caddr_t opt_ptr, size_t opt_len,
	      mblk_t *dp)
{
	mblk_t *mp;
	struct T_optdata_ind *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_band = (flags & T_ODF_EX) ? 1 : 0;
		if (bcanputnext(ss->oq, mp->b_band)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_IND;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- T_OPTDATA_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_addr_ack: - issued T_ADDR_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @loc_ptr: local address pointer
 * @loc_len: local address length
 * @rem_ptr: remote address pointer
 * @rem_len: remote address length
 */
STATIC INLINE int
t_addr_ack(struct ss *ss, queue_t *q, caddr_t loc_ptr, size_t loc_len, caddr_t rem_ptr,
	   size_t rem_len)
{
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t mlen = sizeof(*p) + loc_len + rem_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- T_ADDR_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * t_capability_ack: - issued T_CAPABILITY_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
t_capability_ack(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct T_capability_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = 0;
		p->INFO_ack.PRIM_type = T_INFO_ACK;
		p->INFO_ack.TSDU_size = XXX;
		p->INFO_ack.ETSDU_size = XXX;
		p->INFO_ack.CDATA_size = XXX;
		p->INFO_ack.DDATA_size = XXX;
		p->INFO_ack.ADDR_size = XXX;
		p->INFO_ack.OPT_size = XXX;
		p->INFO_ack.TIDU_size = XXX;
		p->INFO_ack.SERV_type = XXX;
		p->INFO_ack.CURRENT_state = XXX;
		p->INFO_ack.PROVIDER_flag = XXX;
		p->ACCEPTOR_id = (t_uscalar_t) (long) (ss->oq);
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- T_CAPABILITY_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  TPI Primitive sent downstream (to T Provider)
 *  -------------------------------------------------------------------------
 *  Can be used for MTP, SCCP, TCAP and SCTP.
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream (to Transport Provider)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T_CONN_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_conn_req(struct xp *xp, queue_t *q, uchar *dst_ptr, size_t dst_len, uchar *opt_ptr,
	   size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct T_conn_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_CONN_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s; %p: T_CONN_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_CONN_RES
 *  -----------------------------------
 */
STATIC INLINE int
t_conn_res(struct xp *xp, queue_t *q, uint32_t acceptor, uchar *opt_ptr, size_t opt_len,
	   uint32_t seqno, mblk_t *dp)
{
	mblk_t *mp;
	struct T_conn_res *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_CONN_RES;
			p->ACCEPTOR_id = acceptor;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->SEQ_number = seqno;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s; %p: T_CONN_RES ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_DISCON_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_discon_req(struct xp *xp, queue_t *q, uint32_t seqno, mblk_t *dp)
{
	mblk_t *mp;
	struct T_discon_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_DISCON_REQ;
			p->SEQ_number = seqno;
			mp->b_cont = dp;
			printd(("%s; %p: T_DISCON_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_DATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_data_req(struct xp *xp, queue_t *q, uint32_t more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_DATA_REQ;
			p->MORE_flag = more;
			mp->b_cont = dp;
			printd(("%s; %p: T_DATA_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_EXDATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_exdata_req(struct xp *xp, queue_t *q, uint32_t more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_exdata_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_EXDATA_REQ;
			p->MORE_flag = more;
			mp->b_cont = dp;
			printd(("%s; %p: T_EXDATA_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_info_req(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	struct T_info_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_INFO_REQ;
			printd(("%s; %p: T_INFO_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_BIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_bind_req(struct xp *xp, queue_t *q, uchar *add_ptr, size_t add_len, uint32_t conind)
{
	mblk_t *mp;
	struct T_bind_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_BIND_REQ;
			p->ADDR_length = add_len;
			p->ADDR_offset = add_len ? sizeof(*p) : 0;
			p->CONIND_number = conind;
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			printd(("%s; %p: T_BIND_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_UNBIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_unbind_req(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	struct T_unbind_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_UNBIND_REQ;
			printd(("%s; %p: T_UNBIND_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_UNITDATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_unitdata_req(struct xp *xp, queue_t *q, uchar *dst_ptr, size_t dst_len, uchar *opt_ptr,
	       size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct T_unitdata_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_UNITDATA_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
			mp->b_cont = dp;
			printd(("%s; %p: T_UNITDATA_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_optmgmt_req(struct xp *xp, queue_t *q, uchar *opt_ptr, size_t opt_len, uint32_t flags)
{
	mblk_t *mp;
	struct T_optmgmt_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_OPTMGMT_REQ;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->MGMT_flags = flags;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			printd(("%s; %p: T_OPTMGMT_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_ORDREL_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_ordrel_req(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	struct T_ordrel_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_ORDREL_REQ;
			printd(("%s; %p: T_ORDREL_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_OPTDATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_optdata_req(struct xp *xp, queue_t *q, uint32_t flags, uint32_t sid, mblk_t *dp)
{
	mblk_t *mp;
	struct T_optdata_req *p;
	uchar opt[2 * sizeof(struct t_opthdr) + 2 * sizeof(t_uscalar_t)];
	size_t opt_len = (xp->type == UA_OBJ_TYPE_XP_SCTP) ? sizeof(opt) : 0;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				struct t_opthdr *oh;

				oh = (typeof(oh)) mp->b_wptr;
				mp->b_wptr += sizeof(*oh);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->len = sizeof(t_uscalar_t);
				*(t_uscalar_t *) mp->b_wptr = xp->ppi;
				mp->b_wptr += sizeof(t_uscalar_t);
				oh = (typeof(oh)) mp->b_wptr;
				mp->b_wptr += sizeof(*oh);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SID;
				oh->len = sizeof(t_uscalar_t);
				*(t_uscalar_t *) mp->b_wptr = sid;
				mp->b_wptr += sizeof(t_uscalar_t);
			}
			mp->b_cont = dp;
			printd(("%s; %p: T_OPTDATA_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_ADDR_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_addr_req(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	struct T_addr_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_ADDR_REQ;
			printd(("%s; %p: T_ADDR_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_capability_req(struct xp *xp, queue_t *q)
{
	mblk_t *mp;
	struct T_capability_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_CAPABILITY_REQ;
			printd(("%s; %p: T_CAPABILITY_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

#endif				/* __LOCAL_UA_TPI_H__ */
