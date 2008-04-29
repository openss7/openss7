/*****************************************************************************

 @(#) $RCSfile: ua_dlpi.h,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-29 01:52:22 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2008-04-29 01:52:22 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ua_dlpi.h,v $
 Revision 0.9.2.3  2008-04-29 01:52:22  brian
 - updated headers for release

 Revision 0.9.2.2  2007/08/14 08:34:00  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/11/30 13:17:55  brian
 - added files from strss7 package

 *****************************************************************************/

#ifndef __LOCAL_UA_DLPI_H__
#define __LOCAL_UA_DLPI_H__

#ident "@(#) $RCSfile: ua_dlpi.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 *  DLPI User Primitives (to SS User)
 *  --------------------------------------------------------------------------
 *  Data Link Provider Interface primitive can be used for MTP2, ISDN, V5.2, DASS-1.
 */
/**
 * dl_info_ack: - issue a DL_INFO_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @qos_ptr: quality of service pointer
 * @qos_len: quality of service length
 * @qor_ptr: quality of service range pointer
 * @qor_len: quality of service range length
 * @brd_ptr: broadcast address pointer
 * @brd_len: broadcast address length
 * @add_ptr: address pointer
 * @add_len: address length
 */
STATIC INLINE int
dl_info_ack(struct ss *ss, queue_t *q, caddr_t qos_ptr, size_t qos_len, caddr_t qor_ptr,
	    size_t qor_len, caddr_t brd_ptr, size_t brd_len, caddr_t add_ptr, size_t add_len)
{
	mblk_t *mp;
	dl_info_ack_t *p;
	size_t mlen = sizeof(*p) + qos_len + qor_len + brd_len + (add_ptr ? add_len : 0);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_max_sdu = XXX;
		p->dl_min_sdu = XXX;
		p->dl_addr_length = XXX;
		p->dl_mac_type = XXX;
		p->dl_reserved = XXX;
		p->dl_current_state = XXX;
		p->dl_sap_length = XXX;
		p->dl_service_mode = XXX;
		p->dl_qos_length = qos_len;
		p->dl_qos_offset = qos_len ? sizeof(*p) : 0;
		p->dl_qos_range_length = qor_len;
		p->dl_qos_range_offset = qor_len ? sizeof(*p) + qos_len : 0;
		p->dl_provider_style = XXX;
		p->dl_addr_offset = add_ptr ? sizeof(*p) + qos_len + qor_len + brd_len : 0;
		p->dl_version = XXX;
		p->dl_brdcst_addr_length = brd_len;
		p->dl_brdcst_addr_offset = brd_len ? sizeof(*p) + qos_len + qor_len : 0;
		p->dl_growth = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(qos_ptr, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		bcopy(qor_ptr, mp->b_wptr, qor_len);
		mp->b_wptr += qor_len;
		bcopy(brd_ptr, mp->b_wptr, brd_len);
		mp->b_wptr += brd_len;
		if (add_ptr) {
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: <- DL_INFO_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * dl_bind_ack: - issue a DL_BIND_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dlsap: dlsap
 * @add_ptr: address pointer
 * @add_len: address length
 * @coninds: maximum number of outstanding connection indications
 * @flags: XID and TEST flags
 */
STATIC INLINE int
dl_bind_ack(struct ss *ss, queue_t *q, dl_ulong dlsap, caddr_t add_ptr, size_t add_len,
	    dl_ulong coninds, dl_ulong flags)
{
	mblk_t *mp;
	dl_bind_ack_t *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = dlsap;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = add_len ? sizeof(*p) : 0;
		p->dl_max_conind = coninds;
		p->dl_xidtest_flg = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		printd(("%s: %p: <- DL_BIND_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * dl_ok_ack: - issue a DL_OK_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: correct primitive
 */
STATIC INLINE int
dl_ok_ack(struct ss *ss, queue_t *q, dl_long prim)
{
	mblk_t *mp;
	dl_ok_ack_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- DL_OK_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * dl_error_ack: - issue a DL_ERROR_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: primitive in error
 * @error: positive DLPI error negative UNIX error
 */
STATIC INLINE int
dl_error_ack(struct ss *ss, queue_t *q, dl_long prim, dl_long error)
{
	mblk_t *mp;
	dl_error_ack_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = error < 0 ? DL_SYSERR : 0;
		p->dl_unix_errno = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- DL_ERROR_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * dl_subs_bind_ack: - issue a DL_SUBS_BIND_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @sap_ptr: subsequent SAP pointer
 * @sap_len: subsequent SAP length
 */
STATIC INLINE int
dl_subs_bind_ack(struct ss *ss, queue_t *q, caddr_t sap_ptr, size_t sap_len)
{
	mblk_t *mp;
	dl_subs_bind_ack_t *p;
	size_t mlen = sizeof(*p) + sap_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_SUBS_BIND_ACK;
			p->dl_subs_sap_length = sap_len;
			p->dl_subs_sap_offset = sap_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(sap_ptr, mp->b_wptr, sap_len);
			mp->b_wptr += sap_len;
			printd(("%s: %p: <- DL_SUBS_BIND_ACK\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_unitdata_ind: - issue a DL_UNITDATA_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dst_ptr: destination address pointer
 * @dst_len: destination address length
 * @src_ptr: source address pointer
 * @src_len: source address length
 * @group: group address
 * @dp: user data
 */
STATIC INLINE int
dl_unitdata_ind(struct ss *ss, queue_t *q, caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr,
		size_t src_len, dl_ulong group, mblk_t *dp)
{
	mblk_t *mp;
	dl_unitdata_ind_t *p;
	size_t mlen = sizeof(*p) + dst_len + src_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UNITDATA_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			p->dl_group_address = group;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- DL_UNITDATA_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_uderror_ind: - issue a DL_UDERROR_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @dst_ptr: destination address pointer
 * @dst_len: destination address length
 * @error: positive DLPI error negative UNIX error
 * @dp: user data
 */
STATIC INLINE int
dl_uderror_ind(struct ss *ss, queue_t *q, caddr_t dst_ptr, size_t dst_len, dl_long error,
	       mblk_t *dp)
{
	mblk_t *mp;
	dl_uderror_ind_t *p;
	size_t mlen = sizeof(*p) + dst_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UDERROR_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_unix_errno = error < 0 ? -error : 0;
			p->dl_errno = error < 0 ? DL_SYSERR : error;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- DL_UDERROR_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_connect_ind: - issue a DL_CONNECT_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @corid: correlation
 * @cld_ptr: called address pointer
 * @cld_len: called address length
 * @clg_ptr: calling address pointer
 * @clg_len: calling address length
 * @qos_ptr: quality of service pointer
 * @qos_len: quality of service length
 * @dp: user data
 */
STATIC INLINE int
dl_connect_ind(struct ss *ss, queue_t *q, dl_ulong corid, caddr_t cld_ptr, size_t cld_len,
	       caddr_t clg_ptr, size_t clg_len, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	dl_connect_ind_t *p;
	size_t mlen = sizeof(*p) + cld_len + clg_len + qos_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_IND;
			p->dl_correlation = corid;
			p->dl_called_addr_length = cld_len;
			p->dl_called_addr_offset = cld_len ? sizeof(*p) : 0;
			p->dl_calling_addr_length = clg_len;
			p->dl_calling_addr_offset = clg_len ? sizeof(*p) + cld_len : 0;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = qos_len ? sizeof(*p) + cld_len + clg_len : 0;
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(cld_ptr, mp->b_wptr, cld_len);
			mp->b_wptr += cld_len;
			bcopy(clg_ptr, mp->b_wptr, clg_len);
			mp->b_wptr += clg_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- DL_CONNECT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_connect_con: - issue a DL_CONNECT_CON primitive upstream
 * @ss: private structure
 * @q: active queue
 * @res_ptr: responding address pointer
 * @res_len: responding address length
 * @qos_ptr: quality of service pointer
 * @qos_len: quality of service length
 *
 */
STATIC INLINE int
dl_connect_con(struct ss *ss, queue_t *q, caddr_t res_ptr, size_t res_len, caddr_t qos_ptr,
	       size_t qos_len, mblk_t *dp)
{
	mblk_t *mp;
	dl_connect_con_t *p;
	size_t mlen = sizeof(*p) + res_len + qos_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_CON;
			p->dl_resp_addr_length = res_len;
			p->dl_resp_addr_offset = res_len ? sizeof(*p) : 0;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = qos_len ? sizeof(*p) + res_len : 0;
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(res_ptr, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- DL_CONNECT_CON\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_token_ack: - issue a DL_TOKEN_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_token_ack(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	dl_token_ack_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TOKEN_ACK;
		p->dl_token = (dl_ulong) (long) ss->iq;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- DL_TOKEN_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * dl_disconnect_ind: - issue a DL_DISCONNECT_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @origin: originator
 * @reason: reason
 * @corid: correlation
 */
STATIC INLINE int
dl_disconnect_ind(struct ss *ss, queue_t *q, dl_long origin, dl_long reason, dl_ulong corid)
{
	mblk_t *mp;
	dl_disconnect_ind_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DISCONNECT_IND;
			p->dl_originator = origin;
			p->dl_reason = reason;
			p->dl_correlation = corid;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_DISCONNECT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_reset_ind: - issue a DL_RESET_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @origin: originator
 * @reason: reason
 */
STATIC INLINE int
dl_reset_ind(struct ss *ss, queue_t *q, dl_long origin, dl_long reason)
{
	mblk_t *mp;
	dl_reset_ind_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_IND;
			p->dl_originator = origin;
			p->dl_reason = reason;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_RESET_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_reset_con: - issue a DL_RESET_CON primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_reset_con(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	dl_reset_con_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_CON;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_RESET_CON\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * dl_data_ack_ind: - issue a DL_DATA_ACK_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_data_ack_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	dl_data_ack_ind_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DATA_ACK_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_DATA_ACK_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_data_ack_status_ind: - issue a DL_DATA_ACK_STATUS_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_data_ack_status_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	dl_data_ack_status_ind_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DATA_ACK_STATUS_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_DATA_ACK_STATUS_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_reply_ind: - issue a DL_REPLY_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_reply_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	dl_reply_ind_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_REPLY_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_reply_status_ind: - issue a DL_REPLY_STATUS_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_reply_status_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	dl_reply_status_ind_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_STATUS_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_REPLY_STATUS_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_reply_update_status_ind: - issue a DL_REPLY_UPDATE_STATUS_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_reply_update_status_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	dl_reply_update_status_ind_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_REPLY_UPDATE_STATUS_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

/**
 * dl_xid_ind: - issue a DL_XID_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @flag: flags
 * @dst_ptr: destination address pointer
 * @dst_len: destination address length
 * @src_ptr: source address pointer
 * @src_len: source address length
 */
STATIC INLINE int
dl_xid_ind(struct ss *ss, queue_t *q, dl_ulong flags, caddr_t dst_ptr, size_t dst_len,
	   caddr_t src_ptr, size_t src_len)
{
	mblk_t *mp;
	dl_xid_ind_t *p;
	size_t mlen = sizeof(*p) + dst_len + src_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_IND;
			p->dl_flag = flags;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			printd(("%s: %p: <- DL_XID_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_xid_con: - issue a DL_XID_CON primitive upstream
 * @ss: private structure
 * @q: active queue
 * @flag: flags
 * @dst_ptr: destination address pointer
 * @dst_len: destination address length
 * @src_ptr: source address pointer
 * @src_len: source address length
 */
STATIC INLINE int
dl_xid_con(struct ss *ss, queue_t *q, dl_ulong flags, caddr_t dst_ptr, size_t dst_len,
	   caddr_t src_ptr, size_t src_len)
{
	mblk_t *mp;
	dl_xid_con_t *p;
	size_t mlen = sizeof(*p) + dst_len + src_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_CON;
			p->dl_flag = flags;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			printd(("%s: %p: <- DL_XID_CON\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_test_ind: - issue a DL_TEST_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_test_ind(struct ss *ss, queue_t *q, dl_ulong flags, caddr_t dst_ptr, size_t dst_len,
	    caddr_t src_ptr, size_t src_len)
{
	mblk_t *mp;
	dl_test_ind_t *p;
	size_t mlen = sizeof(*p) + dst_len + src_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_IND;
			p->dl_flag = flags;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			printd(("%s: %p: <- DL_TEST_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_test_con: - issue a DL_TEST_CON primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_test_con(struct ss *ss, queue_t *q, dl_ulong flags, caddr_t dst_ptr, size_t dst_len,
	    caddr_t src_ptr, size_t src_len)
{
	mblk_t *mp;
	dl_test_con_t *p;
	size_t mlen = sizeof(*p) + dst_len + src_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_CON;
			p->dl_flag = flags;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			printd(("%s: %p: <- DL_TEST_CON\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * dl_phys_addr_ack: - issue a DL_PHYS_ADDR_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_phys_addr_ack(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	dl_phys_addr_ack_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_PHYS_ADDR_ACK;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_PHYS_ADDR_ACK\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_get_statistics_ack: - issue a DL_GET_STATISTICS_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
dl_get_statistics_ack(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	dl_get_statistics_ack_t *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_GET_STATISTICS_ACK;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- DL_GET_STATISTICS_ACK\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

#endif				/* __LOCAL_UA_DLPI_H__ */
