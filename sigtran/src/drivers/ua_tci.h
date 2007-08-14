/*****************************************************************************

 @(#) $Id: ua_tci.h,v 0.9.2.2 2007/08/14 08:34:01 brian Exp $

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

 $Log: ua_tci.h,v $
 Revision 0.9.2.2  2007/08/14 08:34:01  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/11/30 13:17:55  brian
 - added files from strss7 package

 *****************************************************************************/

#ifndef __LOCAL_UA_TCI_H__
#define __LOCAL_UA_TCI_H__

#ident "@(#) $RCSfile: ua_tci.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/*
 *  TCI User Primitives (to SS User)
 *  --------------------------------------------------------------------------
 *  Transaction Component Interface primitives can be used for TCAP TC users.
 */
/**
 * tc_info_ack: - issue a TC_INFO_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
tc_info_ack(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct TC_info_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_INFO_ACK;
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
		p->TCI_version = XXX;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- TC_INFO_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tc_bind_ack: - issue a TC_BIND_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 * @xacts: maximum number of outstanding transactions
 */
STATIC INLINE int
tc_bind_ack(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len, t_scalar_t xacts)
{
	mblk_t *mp;
	struct TC_bind_ack *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->XACT_number = xacts;
		p->TOKEN_value = (t_uscalar_t) (long) ss->oq;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		printd(("%s: %p: <- TC_BIND_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tc_subs_bind_ack: - issue a TC_SUBS_BIND_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
tc_subs_bind_ack(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct TC_subs_bind_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_SUBS_BIND_ACK;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- TC_SUBS_BIND_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tc_ok_ack: - issue a TC_OK_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: correct primitive
 */
STATIC INLINE int
tc_ok_ack(struct ss *ss, queue_t *q, t_scalar_t prim)
{
	mblk_t *mp;
	struct TC_ok_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- TC_OK_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tc_error_ack: - issue a TC_ERROR_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: primitive in error
 * @error: postitive TCPI error negative UNIX error
 * @dialog: dialog id
 * @invoke: invoke id
 */
STATIC INLINE int
tc_error_ack(struct ss *ss, queue_t *q, t_scalar_t prim, t_scalar_t error, t_uscalar_t dialog,
	     t_uscalar_t invoke)
{
	mblk_t *mp;
	struct TC_error_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TRPI_error = error < 0 ? TSYSERR : 0;
		p->UNIX_error = error < 0 ? -error : 0;
		p->DIALOG_id = dialog;
		p->INVOKE_id = invoke;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- TC_ERROR_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tc_optmgmt_ack: - issue a TC_OPTMGMT_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @flags: management flags
 */
STATIC INLINE int
tc_optmgmt_ack(struct ss *ss, queue_t *q, caddr_t opt_ptr, size_t opt_len, t_uscalar_t flags)
{
	mblk_t *mp;
	struct TC_optmgmt_ack *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = TC_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- TC_OPTMGMT_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * tc_uni_ind: - issue a TC_UNI_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @src_ptr: source pointer
 * @src_len: source length
 * @dst_ptr: destination pointer
 * @dst_len: destination length
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dialog: dialog id
 * @flags: component flags
 * @dp: user data
 */
STATIC INLINE int
tc_uni_ind(struct ss *ss, queue_t *q,
	   caddr_t src_ptr, size_t src_len, caddr_t dst_ptr, size_t dst_len, caddr_t opt_ptr,
	   size_t opt_len, t_uscalar_t dialog, t_uscalar_t flags, mblk_t *dp)
{
	mblk_t *mp;
	struct TC_uni_ind *p;
	size_t mlen = sizeof(*p) + src_len + dst_len + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_UNI_IND;
			p->SRC_length = src_len;
			p->SRC_offset = src_len ? sizeof(*p) : 0;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + src_len + dst_len : 0;
			p->DIALOG_id = dialog;
			p->COMP_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- TC_UNI_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_begin_ind: - issue a TC_BEGIN_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @src_ptr: source pointer
 * @src_len: source length
 * @dst_ptr: destination pointer
 * @dst_len: destination length
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dialog: dialog id
 * @flags: component flags
 * @dp: user data
 */
STATIC INLINE int
tc_begin_ind(struct ss *ss, queue_t *q,
	     caddr_t src_ptr, size_t src_len, caddr_t dst_ptr, size_t dst_len, caddr_t opt_ptr,
	     size_t opt_len, t_uscalar_t dialog, t_uscalar_t flags, mblk_t *dp)
{
	mblk_t *mp;
	struct TC_begin_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_BEGIN_IND;
			p->SRC_length = src_len;
			p->SRC_offset = src_len ? sizeof(*p) : 0;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + src_len + dst_len : 0;
			p->DIALOG_id = dialog;
			p->COMP_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- TC_BEGIN_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_begin_con: - issue a TC_BEGIN_CON primitive upstream
 * @ss: private structure
 * @q: active queue
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dialog: dialog id
 * @flags: component flags
 * @dp: user data
 */
STATIC INLINE int
tc_begin_con(struct ss *ss, queue_t *q, caddr_t opt_ptr, size_t opt_len, t_uscalar_t dialog,
	     t_uscalar_t flags, mblk_t *dp)
{
	mblk_t *mp;
	struct TC_begin_con *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_BEGIN_CON;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->DIALOG_id = dialog;
			p->COMP_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- TC_BEGIN_CON\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_cont_ind: - issue a TC_CONT_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dialog: dialog id
 * @flags: component flags
 * @dp: user data
 */
STATIC INLINE int
tc_cont_ind(struct ss *ss, queue_t *q, caddr_t opt_ptr, size_t opt_len, t_uscalar_t dialog,
	    t_uscalar_t flags, mblk_t *dp)
{
	mblk_t *mp;
	struct TC_cont_ind *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_CONT_IND;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->DIALOG_id = dialog;
			p->COMP_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- TC_CONT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_end_ind: - issue a TC_END_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dialog: dialog id
 * @flags: component flags
 * @dp: user data
 */
STATIC INLINE int
tc_end_ind(struct ss *ss, queue_t *q, caddr_t opt_ptr, size_t opt_len, t_uscalar_t dialog,
	   t_uscalar_t flags, mblk_t *dp)
{
	mblk_t *mp;
	struct TC_end_ind *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_END_IND;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->DIALOG_id = dialog;
			p->COMP_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- TC_END_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_abort_ind: - issue a TC_ABORT_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @dialog: dialog id
 * @reason: abort reason
 * @origin: originator
 * @dp: user data
 */
STATIC INLINE int
tc_abort_ind(struct ss *ss, queue_t *q, caddr_t opt_ptr, size_t opt_len, t_uscalar_t dialog,
	     t_scalar_t reason, t_scalar_t origin, mblk_t *dp)
{
	mblk_t *mp;
	struct TC_abort_ind *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_ABORT_IND;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->DIALOG_id = dialog;
			p->ABORT_reason = reason;
			p->ORIGINATOR = origin;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- TC_ABORT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_notice_ind: - issue a TC_NOTICE_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dialog: dialog id
 * @cause: report cause
 */
STATIC INLINE int
tc_notice_ind(struct ss *ss, queue_t *q, t_uscalar_t dialog, t_scalar_t cause)
{
	mblk_t *mp;
	struct TC_notice_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_NOTICE_IND;
			p->DIALOG_id = dialog;
			p->REPORT_cause = cause;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- TC_NOTICE_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_invoke_ind: - issue a TC_INVOKE_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dialog: dialog id
 * @class: operations class
 * @invoke: invoke id
 * @linked: linked id
 * @op: operation
 * @flags: more flags
 * @dp: user data
 */
STATIC INLINE int
tc_invoke_ind(struct ss *ss, queue_t *q, t_uscalar_t dialog, t_scalar_t class, t_uscalar_t invoke,
	      t_uscalar_t linked, t_scalar_t op, t_uscalar_t flags, mblk_t *dp)
{
	mblk_t *mp;
	struct TC_invoke_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_INVOKE_IND;
			p->DIALOG_id = dialog;
			p->OP_class = class;
			p->INVOKE_id = invoke;
			p->LINKED_id = linked;
			p->OPERATION = op;
			p->MORE_flag = flags;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- TC_INVOKE_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_result_ind: - issue a TC_RESULT_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dialog: dialog id
 * @invoke: invoke id
 * @op: operqation
 * @flags: more flag
 * @dp: user data
 */
STATIC INLINE int
tc_result_ind(struct ss *ss, queue_t *q, t_uscalar_t dialog, t_uscalar_t invoke, t_scalar_t op,
	      t_uscalar_t flags, mblk_t *dp)
{
	mblk_t *mp;
	struct TC_result_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_RESULT_IND;
			p->DIALOG_id = dialog;
			p->INVOKE_id = invoke;
			p->OPERATION = op;
			p->MORE_flag = flags;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- TC_RESULT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_error_ind: - issue a TC_ERROR_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dialog: dialog id
 * @invoke: invoke id
 * @error: error code
 * @dp: user data
 */
STATIC INLINE int
tc_error_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct TC_error_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_ERROR_IND;
			p->DIALOG_id = dialog;
			p->INVOKE_id = invoke;
			p->ERROR_code = error;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- TC_ERROR_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_cancel_ind: - issue a TC_CANCEL_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dialog: dialog id
 * @invoke: invoke id
 */
STATIC INLINE int
tc_cancel_ind(struct ss *ss, queue_t *q, t_uscalar_t dialog, t_uscalar_t invoke)
{
	mblk_t *mp;
	struct TC_cancel_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_CANCEL_IND;
			p->DIALOG_id = dialog;
			p->INVOKE_id = invoke;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- TC_CANCEL_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * tc_reject_ind: - issue a TC_REJECT_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dialog: dialog id
 * @invoke: invoke id
 * @origin: originator
 * @problem: problem code
 */
STATIC INLINE int
tc_reject_ind(struct ss *ss, queue_t *q, t_uscalar_t dialog, t_uscalar_t invoke, t_scalar_t origin,
	      t_scalar_t problem)
{
	mblk_t *mp;
	struct TC_reject_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = TC_REJECT_IND;
			p->DIALOG_id = dialog;
			p->INVOKE_id = invoke;
			p->ORIGINATOR = origin;
			p->PROBLEM_code = problem;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- TC_REJECT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#endif				/* __LOCAL_UA_TCI_H__ */
