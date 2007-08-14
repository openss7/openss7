/*****************************************************************************

 @(#) $Id: ua_tri.h,v 0.9.2.2 2007/08/14 08:34:01 brian Exp $

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

 Last Modified $Date: 2007/08/14 08:34:01 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ua_tri.h,v $
 Revision 0.9.2.2  2007/08/14 08:34:01  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/11/30 13:17:55  brian
 - added files from strss7 package

 *****************************************************************************/

#ifndef __LOCAL_UA_TRI_H__
#define __LOCAL_UA_TRI_H__

#ident "@(#) $RCSfile: ua_tri.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/*
 *  TRI User Primitives (to SS User)
 *  --------------------------------------------------------------------------
 *  These are Transaction Interface primitives to a TCAP TR-User.
 */
/**
 * tr_info_ack: - issue a TR_INFO_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
tr_info_ack(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct TR_info_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_INFO_ACK;
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
		p->TRPI_version = XXX;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %d: <- TR_INFO_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tr_bind_ack: - issue a TR_BIND_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 * @xacts: maximum outstanding transactions
 * @flags: bind flags
 */
STATIC INLINE int
tr_bind_ack(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len, t_uscalar_t xacts,
	    t_scalar_t flags)
{
	mblk_t *mp;
	struct TR_bind_ack *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->XACT_number = xacts;
		p->BIND_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		printd(("%s: %d: <- TR_BIND_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tr_ok_ack: - issue a TR_OK_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: correct primitive
 */
STATIC INLINE int
tr_ok_ack(struct ss *ss, queue_t *q, t_scalar_t prim)
{
	mblk_t *mp;
	struct TR_ok_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %d: <- TR_OK_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tr_error_ack: - issue a TR_ERROR_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: primitive in error
 * @error: positive TRPI error negative UNIX error
 */
STATIC INLINE int
tr_error_ack(struct ss *ss, queue_t *q, t_scalar_t prim, t_scalar_t error)
{
	mblk_t *mp;
	struct TR_error_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TRPI_error = error < 0 ? TSYSERR : 0;
		p->UNIX_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %d: <- TR_ERROR_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tr_optmgmt_ack: - issue a TR_OPTMGMT_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @flags: management flags
 */
STATIC INLINE int
tr_optmgmt_ack(struct ss *ss, queue_t *q, caddr_t opt_ptr, size_t opt_len, t_uscalar_t flags)
{
	mblk_t *mp;
	struct TR_optmgmt_ack *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TR_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %d: <- TR_OPTMGMT_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tr_uni_ind: - issue a TR_UNI_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dst_ptr: destination address pointer
 * @dst_len: destination address length
 * @ori_ptr: origination address pointer
 * @ori_len: origination address length
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dp: user data
 */
STATIC INLINE int
tr_uni_ind(struct ss *ss, queue_t *q, caddr_t dst_ptr, size_t dst_len, caddr_t ori_ptr,
	   size_t ori_len, caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct TR_uni_ind *p;
	size_t mlen = sizeof(*p) + dst_len + ori_len + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_UNI_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->ORIG_length = ori_len;
			p->ORIG_offset = ori_len ? sizeof(*p) + dst_len : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len + ori_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(ori_ptr, mp->b_wptr, ori_len);
			mp->b_wptr += ori_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %d: <- TR_UNI_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tr_begin_ind: - issue a TR_BEGIN_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @transid: transaction id
 * @flags: association flags
 * @dst_ptr: destination address pointer
 * @dst_len: destination address length
 * @ori_ptr: origination address pointer
 * @ori_len: origination address length
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dp: user data
 */
STATIC INLINE int
tr_begin_ind(struct ss *ss, queue_t *q, t_uscalar_t transid, t_uscalar_t flags, caddr_t dst_ptr,
	     size_t dst_len, caddr_t ori_ptr, size_t ori_len, caddr_t opt_ptr, size_t opt_len,
	     mblk_t *dp)
{
	mblk_t *mp;
	struct TR_begin_ind *p;
	size_t mlen = sizeof(*p) + dst_len + ori_len + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_BEGIN_IND;
			p->TRANS_id = transid;
			p->ASSOC_flags = flags;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->ORIG_length = ori_len;
			p->ORIG_offset = ori_len ? sizeof(*p) + dst_len : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len + ori_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(ori_ptr, mp->b_wptr, ori_len);
			mp->b_wptr += ori_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %d: <- TR_BEGIN_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tr_begin_con: - issue a TR_BEGIN_CON primitive upstream
 * @ss: private structure
 * @q: active queue
 * @corid: correlation id
 * @flags: association flags
 * @transid: transaction id
 * @ori_ptr: origination pointer
 * @ori_len: origination length
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dp: user data
 */
STATIC INLINE int
tr_begin_con(struct ss *ss, queue_t *q, t_uscalar_t corid, t_uscalar_t flags, t_uscalar_t transid,
	     caddr_t ori_ptr, size_t ori_len, caddr_t opt_ptr, size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct TR_begin_con *p;
	size_t mlen = sizeof(*p) + ori_len + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_BEGIN_CON;
			p->CORR_id = corid;
			p->ASSOC_flags = flags;
			p->TRANS_id = transid;
			p->ORIG_length = ori_len;
			p->ORIG_offset = ori_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + ori_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(ori_ptr, mp->b_wptr, ori_len);
			mp->b_wptr += ori_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %d: <- TR_BEGIN_CON\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tr_cont_ind: - issue a TR_CONT_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @transid: transaction id
 * @flags: association flags
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dp: user data
 */
STATIC INLINE int
tr_cont_ind(struct ss *ss, queue_t *q, t_uscalar_t transid, t_uscalar_t flags, caddr_t opt_ptr,
	    size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct TR_cont_ind *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_CONT_IND;
			p->TRANS_id = transid;
			p->ASSOC_flags = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %d: <- TR_CONT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tr_end_ind: - issue a TR_END_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @corid: correlation id
 * @transid: transaction id
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dp: user data
 */
STATIC INLINE int
tr_end_ind(struct ss *ss, queue_t *q, t_uscalar_t corid, t_uscalar_t transid, caddr_t opt_ptr,
	   size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct TR_end_ind *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_END_IND;
			p->CORR_id = corrid;
			p->TRANS_id = transid;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %d: <- TR_END_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tr_abort_ind: - issue a TR_ABORT_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @transid: transaction id
 * @cause: abort cause
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dp: user data
 */
STATIC INLINE int
tr_abort_ind(struct ss *ss, queue_t *q, t_uscalar_t transid, t_scalar_t cause, caddr_t opt_ptr,
	     size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct TR_abort_ind *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_ABORT_IND;
			p->TRANS_id = transid;
			p->ABORT_cause = cause;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %d: <- TR_ABORT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tr_notice_ind: - issue a TR_NOTICE_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @corid: correlation id
 * @transid: transaction id
 * @cause: report cause
 * @dp: user data
 */
STATIC INLINE int
tr_notice_ind(struct ss *ss, queue_t *q, t_uscalar_t corid, t_uscalar_t transid, t_scalar_t cause,
	      mblk_t *dp)
{
	mblk_t *mp;
	struct TR_notice_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TR_NOTICE_IND;
			p->CORR_id = corid;
			p->TRANS_id = transid;
			p->REPORT_cause = cause;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %d: <- TR_NOTICE_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#endif				/* __LOCAL_UA_TRI_H__ */
