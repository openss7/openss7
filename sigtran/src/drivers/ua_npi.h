/*****************************************************************************

 @(#) $Id: ua_npi.h,v 0.9.2.1 2006/11/30 13:17:55 brian Exp $

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

 $Log: ua_npi.h,v $
 Revision 0.9.2.1  2006/11/30 13:17:55  brian
 - added files from strss7 package

 *****************************************************************************/

#ifndef __LOCAL_UA_NPI_H__
#define __LOCAL_UA_NPI_H__

#ident "@(#) $RCSfile: ua_npi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/*
 *  NPI User Primitives (to SS User)
 *  --------------------------------------------------------------------------
 *  Network Provider Interface primitives can be used for MTP, SCCP and even SCTP.  The ss pointer
 *  to these functions could be a linked transport structure.
 */
/**
 * n_conn_ind: - N_CONN_IND primitive
 * @ss: private structure
 * @q: active queue
 * @seq: sequence number
 * @flags: connection flags
 * @dst_ptr: destination pointer
 * @dst_len: destination length
 * @src_ptr: source pointer
 * @src_len: source length
 * @qos_ptr: qos pointer
 * @qos_len: qos length
 * @dp: user data
 */
STATIC INLINE int
n_conn_ind(struct ss *ss, queue_t *q, np_ulong seq, np_ulong flags, caddr_t dst_ptr,
	   size_t dst_len, caddr_t src_ptr, size_t src_len, caddr_t qos_ptr, size_t qos_len,
	   mblk_t *dp)
{
	mblk_t *mp;
	N_conn_ind_t *p;
	size_t mlen = sizeof(*p) + dst_len + src_len + qos_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->SRC_length = src_len;
			p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
			p->SEQ_number = seq;
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- N_CONN_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_con: - N_CONN_CON primitive
 * @ss: private structure
 * @q: active queue
 * @flags: connect flags
 * @res_ptr: responding address pointer
 * @res_len: responding address length
 * @qos_ptr: quality of service pointer
 * @qos_len: quality of service length
 * @dp: user data
 */
STATIC INLINE int
n_conn_con(struct ss *ss, queue_t *q, np_ulong flags, caddr_t res_ptr, size_t res_len,
	   caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_conn_con_t *p;
	size_t mlen = sizeof(*p) + res_len + qos_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
			mp->b_wptr += sizeof(*p);
			if (res_len) {
				bcopy(res_ptr, mp->b_wptr, res_len);
				mp->b_wptr += res_len;
			}
			if (qos_len) {
				bcopy(qos_ptr, mp->b_wptr, qos_len);
				mp->b_wptr += qos_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: <- N_CONN_CON\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_ind: - N_DISCON_IND primitive
 * @ss: private structure
 * @q: active queue
 * @origin: disconnect origin
 * @reason: disconnect reason
 * @res_ptr: responding address pointer
 * @res_len: responding address length
 * @seq: sequence number
 * @dp: user data
 */
STATIC INLINE int
n_discon_ind(struct ss *ss, queue_t *q, np_ulong origin, np_ulong reason, caddr_t res_ptr,
	     size_t res_len, np_ulong seq, mblk_t *dp)
{
	mblk_t *mp;
	N_discon_ind_t *p;
	size_t mlen = sizeof(*p) + res_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_IND;
			p->DISCON_orig = origin;
			p->DISCON_reason = reason;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			if (res_len) {
				bcopy(res_ptr, mp->b_wptr, res_len);
				mp->b_wptr += res_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: <- N_DISCON_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_data_ind: - N_DATA_IND primitive
 * @ss: private structure
 * @q: active queue
 * @flags: data transfer flags
 * @qos_ptr: quality of service pointer
 * @qos_len: quality of service length
 * @dp: user data
 */
STATIC streams_inline streams_fastcall __hot_read int
n_data_ind(struct ss *ss, queue_t *q, np_ulong flags, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;
	size_t mlen = sizeof(*p) + qos_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_IND;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			if (qos_len) {
				bcopy(qos_ptr, mp->b_wptr, qos_len);
				mp->b_wptr += qos_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: <- N_DATA_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_exdata_ind: - N_EXDATA_IND primitive
 * @ss: private structure
 * @q: active queue
 * @qos_ptr: quality of service pointer
 * @qos_len: quality of service length
 * @dp: user data
 */
STATIC streams_inline streams_fastcall __hot_read int
n_exdata_ind(struct ss *ss, queue_t *q, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_ind_t *p;
	size_t mlen = sizeof(*p) + qos_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_IND;
			mp->b_wptr += sizeof(*p);
			if (qos_len) {
				bcopy(qos_ptr, mp->b_wptr, qos_len);
				mp->b_wptr += qos_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: <- N_EXDATA_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_info_ack: - N_INFO_ACK primitive
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 * @qos_ptr: quality of service pointer
 * @qos_len: quality of service length
 * @qor_ptr: quality of service range pointer
 * @qor_len: quality of service range length
 * @pro_ptr: protocol ids pointer
 * @pro_len: protocol ids length
 */
STATIC INLINE int
n_info_ack(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len, caddr_t qos_ptr,
	   size_t qos_len, caddr_t qor_ptr, size_t qor_len, caddr_t pro_ptr, size_t pro_len)
{
	mblk_t *mp;
	N_info_ack_t *p;
	size_t mlen = sizeof(*p) + add_len + qos_len + qor_len + pro_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_ACK;
		p->NSDU_size = XXX;
		p->CDATA_size = XXX;
		p->DDATA_size = XXX;
		p->ADDR_size = XXX;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + add_len : 0;
		p->QOS_range_length = qor_len;
		p->QOS_range_offset = qor_len ? sizeof(*p) + add_len + qos_len : 0;
		p->OPTIONS_flags = XXX;
		p->NIDU_size = XXX;
		p->SERV_type = XXX;
		p->CURRENT_state = XXX;
		p->PROVIDER_type = XXX;
		p->NODU_size = XXX;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len + qos_len + pro_len : 0;
		p->NPI_version = N_CURRENT_VERSION;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_qptr, add_len);
		mp->b_wptr += add_len;
		bcopy(qos_ptr, mp->b_qptr, qos_len);
		mp->b_wptr += qos_len;
		bcopy(qor_ptr, mp->b_qptr, qor_len);
		mp->b_wptr += qor_len;
		bcopy(pro_ptr, mp->b_qptr, pro_len);
		mp->b_wptr += pro_len;
		printd(("%s: %p: <- N_INFO_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * n_bind_ack: - N_BIND_ACK primitive
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 * @coninds: maximum number of oustanding connection indications
 * @pro_ptr: protocol ids pointer
 * @pro_len: protocol ids length
 */
STATIC INLINE int
n_bind_ack(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len, np_ulong coninds,
	   caddr_t pro_ptr, size_t pro_len)
{
	mblk_t *mp;
	N_bind_ack_t *p;
	size_t mlen = sizeof(*p) + add_len + pro_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = coninds;
		p->TOKEN_value = (np_ulong) (long) ss->iq;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(pro_ptr, mp->b_wptr, pro_len);
		mp->b_wptr += pro_len;
		printd(("%s: %p: <- N_BIND_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * n_error_ack: - N_ERROR_ACK primitive
 * @ss: private structure
 * @q: active queue
 * @prim: primitive in error
 * @error: positive NPI error or negative UNIX error
 */
STATIC INLINE int
n_error_ack(struct ss *ss, queue_t *q, np_ulong prim, np_long error)
{
	mblk_t *mp;
	N_error_ack_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = error < 0 ? NSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- N_ERROR_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * n_unitdata_ind: - N_UNITDATA_IND primitive
 * @ss: private structure
 * @q: active queue
 * @src_ptr: source address pointer
 * @src_len: source address length
 * @dst_ptr: destination address pointer
 * @dst_len: destination address length
 * @qos_ptr: quality of service pointer
 * @qos_len: quality of service length
 * @dp: user data
 */
STATIC streams_inline streams_fastcall __hot_read int
n_unitdata_ind(struct ss *ss, queue_t *q, caddr_t src_ptr, size_t src_len, caddr_t dst_ptr,
	       size_t dst_len, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_unitdata_ind_t *p;
	size_t mlen = sizeof(*p) + src_len + dst_len + qos_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_IND;
			p->SRC_length = src_len;
			p->SRC_offset = src_len ? sizeof(*p) : 0;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
			p->ERROR_type = 0;	/* reserved field */
			mp->b_wptr += sizeof(*p);
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
		      mp->b_cont = dp:
			printd(("%s: %p: <- N_UNITDATA_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_uderror_ind: - N_UDERROR_IND primitive
 * @ss: private structure
 * @q: active queue
 * @dst_ptr: destination address pointer
 * @dst_len: destination address length
 * @error: error type
 * @qos_ptr: quality of service pointer
 * @qos_len: quality of service length
 * @dp: user data
 */
STATIC INLINE int
n_uderror_ind(struct ss *ss, queue_t *q, caddr_t dst_ptr, size_t dst_len,
	      np_ulong error, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	N_uderror_ind_t *p;
	size_t mlen = sizeof(*p) + dst_len + qos_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->RESERVED_field = 0;
			p->ERROR_type = error;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- N_UDERROR_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_datack_ind: - N_DATACK_IND primitive
 * @ss: private structure
 * @q: active queue
 * @qos_ptr; quality of service pointer
 * @qos_len: quality of service length
 *
 * Data acknowledgements are sent in band 1 so that they are expedited ahead of data available for
 * reading.  Also this is the same band, band 1, in which unitdata error messages are expedited.
 * Therefore, both unitdata errors and unitdata acknowledgements are both expedited.
 */
STATIC INLINE int
n_datack_ind(struct ss *ss, queue_t *q, caddr_t qos_ptr, size_t qos_len)
{
	mblk_t *mp;
	N_datack_ind_t *p;
	size_t mlen = sizeof(*p) + qos_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATACK_IND;
			mp->b_wptr += sizeof(*p);
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			printd(("%s: %p: <- N_DATACK_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_reset_ind: - N_RESET_IND primitive
 * @ss: private structure
 * @q: active queue
 * @origin: reset origin
 * @reason: reset reason
 */
STATIC INLINE int
n_reset_ind(struct ss *ss, queue_t *q, np_ulong origin, np_ulong reason)
{
	mblk_t *mp;
	N_reset_ind_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = origin;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- N_RESET_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_reset_con: - N_RESET_CON primitive
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
n_reset_con(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	N_reset_con_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != null) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_CON;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- N_RESET_CON\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#endif				/* __LOCAL_UA_NPI_H__ */
