/*****************************************************************************

 @(#) $RCSfile: ua_mtp.h,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-29 01:52:22 $

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

 $Log: ua_mtp.h,v $
 Revision 0.9.2.3  2008-04-29 01:52:22  brian
 - updated headers for release

 Revision 0.9.2.2  2007/08/14 08:34:01  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/11/30 13:17:55  brian
 - added files from strss7 package

 *****************************************************************************/

#ifndef __LOCAL_UA_MTP_H__
#define __LOCAL_UA_MTP_H__

#ident "@(#) $RCSfile: ua_mtp.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 *  MTPI User Primitives (to SS user)
 */
/**
 * mtp_ok_ack: - issue an MTP_OK_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: correct primitive
 */
STATIC INLINE int
mtp_ok_ack(struct ss *ss, queue_t *q, mtp_ulong prim)
{
	mblk_t *mp;
	struct MTP_ok_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OK_ACK;
		p->mtp_correct_prim = prim;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- MTP_OK_ACK\n", DRV_NAME ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * mtp_error_ack: - issue an MTP_ERROR_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @prim: primitive in error
 * @error: positive MTPI error negative UNIX error
 */
STATIC INLINE int
mtp_error_ack(struct ss *ss, queue_t *q, mtp_long prim, mtp_long error)
{
	mblk_t *mp;
	struct MTP_error_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ERROR_ACK;
		p->mtp_error_primitive = prim;
		p->mtp_mtpi_error = error < 0 ? MSYSERR : 0;
		p->mtp_unix_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- MTP_ERROR_ACK\n", DRV_NAME ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * mtp_bind_ack: - issue an MTP_BIND_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 */
STATIC INLINE int
mtp_bind_ack(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len)
{
	mblk_t *mp;
	struct MTP_bind_ack *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_BIND_ACK;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		printd(("%s: %p: <- MTP_BIND_ACK\n", DRV_NAME ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * mtp_addr_ack: - issue an MTP_ADDR_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @loc_ptr: local address pointer
 * @loc_len: local address length
 * @rem_ptr: remote address pointer
 * @rem_len: remote address length
 */
STATIC INLINE int
mtp_addr_ack(struct ss *ss, queue_t *q, caddr_t loc_ptr, size_t loc_len, caddr_t rem_ptr,
	     size_t rem_len)
{
	mblk_t *mp;
	struct MTP_addr_ack *p;
	size_t mlen = sizeof(*p) + loc_len + rem_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ADDR_ACK;
		p->mtp_loc_length = loc_len;
		p->mtp_loc_offset = loc_len ? sizeof(*p) : 0;
		p->mtp_rem_length = rem_len;
		p->mtp_rem_offset = rem_len ? sizeof(*p) + loc_len : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(loc_ptr, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
		bcopy(rem_ptr, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
		printd(("%s: %p: <- MTP_ADDR_ACK\n", DRV_NAME ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * mtp_info_ack: - issue an MTP_INFO_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 */
STATIC INLINE int
mtp_info_ack(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len)
{
	mblk_t *mp;
	struct MTP_info_ack *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_msu_size = XXX;
		p->mtp_addr_size = XXX;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		p->mtp_current_state = XXX;
		p->mtp_serv_type = XXX;
		p->mtp_version = MTP_CURRENT_VERSION;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		printd(("%s: %p: <- MTP_INFO_ACK\n", DRV_NAME ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * mtp_optmgmt_ack: - issue an MTP_OPTMGMT_ACK primitive upstream
 * @ss: private structure
 * @q: active queue
 * @opt_ptr: option pointer
 * @opt_len: option length
 * @flags: management flags
 */
STATIC INLINE int
mtp_optmgmt_ack(struct ss *ss, queue_t *q, caddr_t opt_ptr, size_t opt_len, mtp_long flags)
{
	mblk_t *mp;
	struct MTP_optmgmt_ack *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OPTMGMT_ACK;
		p->mtp_opt_length = opt_len;
		p->mtp_opt_offset = opt_len ? sizeof(*p) : 0;
		p->mtp_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- MTP_OPTMGMT_ACK\n", DRV_NAME ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * mtp_transfer_ind: - issue an MTP_TRANSFER_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @src_ptr: source address pointer
 * @src_len: source address length
 * @mp: message priority
 * @sls: message sequence
 * @dp: user data
 */
STATIC INLINE int
mtp_transfer_ind(struct ss *ss, queue_t *q, caddr_t src_ptr, size_t src_len, mtp_ulong mp,
		 mtp_ulong sls, mblk_t *dp)
{
	mblk_t *mp;
	struct MTP_transfer_ind *p;
	size_t mlen = sizeof(*p) + src_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_TRANSFER_IND;
			p->mtp_srce_length = src_len;
			p->mtp_srce_offset = src_len ? sizeof(*p) : 0;
			p->mtp_mp = mp;
			p->mtp_sls = sls;
			mp->b_wptr += sizeof(*p);
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			printd(("%s: %p: <- MTP_TRANSFER_IND\n", DRV_NAME ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_pause_ind: - issue an MTP_PAUSE_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 *
 * The mtpi.h header file marks this as M_PCPROTO, but it cannot be.  Rather than a high priorty
 * message that could be lost by the Stream head, we expedite the message in message band 1, which
 * is the same message band that is used for pause, resume, status and restart indications.
 */
STATIC INLINE int
mtp_pause_ind(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len)
{
	mblk_t *mp;
	struct MTP_pause_ind *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_PAUSE_IND;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			printd(("%s: %p: <- MTP_PAUSE_IND\n", DRV_NAME ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_resume_ind: - issue an MTP_RESUME_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 *
 * The mtpi.h header file marks this as M_PCPROTO, but it cannot be.  Rather than a high priorty
 * message that could be lost by the Stream head, we expedite the message in message band 1, which
 * is the same message band that is used for pause, resume, status and restart indications.
 */
STATIC INLINE int
mtp_resume_ind(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len)
{
	mblk_t *mp;
	struct MTP_resume_ind *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESUME_IND;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			printd(("%s: %p: <- MTP_RESUME_IND\n", DRV_NAME ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_status_ind: - issue an MTP_STATUS_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 * @type: type
 * @status: status
 *
 * The mtpi.h header file marks this as M_PCPROTO, but it cannot be.  Rather than a high priorty
 * message that could be lost by the Stream head, we expedite the message in message band 1, which
 * is the same message band that is used for pause, resume, status and restart indications.
 */
STATIC INLINE int
mtp_status_ind(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len, mtp_ulong type,
	       mtp_ulong status)
{
	mblk_t *mp;
	struct MTP_status_ind *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PCPROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_STATUS_IND;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
			p->mtp_type = type;
			p->mtp_status = status;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			printd(("%s: %p: <- MTP_STATUS_IND\n", DRV_NAME ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_begins_ind: - issue an MTP_RESTART_BEGINS_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 *
 * The mtpi.h header file marks this a M_PCPROTO, but it cannot be.  Rather than a high priority
 * message that could be lost by the Stream head, we expedite the message in message band 1, which
 * is the same message band that is used for pause, resume, status and restart indications.
 */
STATIC INLINE int
mtp_restart_begins_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct MTP_restart_begins_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESTART_BEGINS_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- MTP_RESTART_BEGINS_IND\n", DRV_NAME ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_complete_ind: - issue an MTP_RESTART_COMPLETE_IND primitive upstream
 * @ss: private structure
 * @q: active queue
 *
 * The mtpi.h header file marks this a M_PCPROTO, but it cannot be.  Rather than a high priority
 * message that could be lost by the Stream head, we expedite the message in message band 1, which
 * is the same message band that is used for pause, resume and status indications.
 */
STATIC INLINE int
mtp_restart_complete_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct MTP_restart_complete_ind *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (bcanputnext(ss->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESTART_COMPLETE_IND;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: <- MTP_RESTART_COMPLETE_IND\n", DRV_NAME ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  MTPI Primitives sent downstream (to MTP Provider)
 *  -------------------------------------------------------------------------
 *  Can be used for MTP Provider.
 */
/**
 * mtp_bind_req: - issue an MTP_BIND_REQ primitive downstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 * @flags: bind flags
 */
STATIC INLINE int
mtp_bind_req(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len, mtp_ulong flags)
{
	mblk_t *mp;
	struct MTP_bind_req *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_BIND_REQ;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
			p->mtp_bind_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			printd(("%s: %p: MTP_BIND_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_unbind_req: - issue an MTP_UNBIND_REQ primitive downstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
mtp_unbind_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct MTP_unbind_req *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: MTP_UNBIND_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_conn_req: - issue an MTP_CONN_REQ primitive downstream
 * @ss: private structure
 * @q: active queue
 * @add_ptr: address pointer
 * @add_len: address length
 * @flags: connection flags
 */
STATIC INLINE int
mtp_conn_req(struct ss *ss, queue_t *q, caddr_t add_ptr, size_t add_len, mtp_ulong flags)
{
	mblk_t *mp;
	struct MTP_conn_req *p;
	size_t mlen = sizeof(*p) + add_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_CONN_REQ;
			p->mtp_addr_length = add_len;
			p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
			p->mtp_conn_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			printd(("%s: %p: MTP_CONN_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_discon_req: - issue an MTP_DISCON_REQ primitive downstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
mtp_discon_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct MTP_discon_req *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_DISCON_REQ;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: MTP_DISCON_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_addr_req: - issue an MTP_ADDR_REQ primitive downstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
mtp_addr_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct MTP_addr_req *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_ADDR_REQ;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: MTP_ADDR_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_info_req: - issue an MTP_INFO_REQ primitive downstream
 * @ss: private structure
 * @q: active queue
 */
STATIC INLINE int
mtp_info_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	struct MTP_info_req *p;
	size_t mlen = sizeof(*p);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_INFO_REQ;
			mp->b_wptr += sizeof(*p);
			printd(("%s: %p: MTP_INFO_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_optmgmt_req: - issue an MTP_OPTMGMT_REQ primitive downstream
 * @ss: private structure
 * @q: active queue
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @flags: management flags
 */
STATIC INLINE int
mtp_optmgmt_req(struct ss *ss, queue_t *q, caddr_t opt_ptr, size_t opt_len, mtp_ulong flags)
{
	mblk_t *mp;
	struct MTP_optmgmt_req *p;
	size_t mlen = sizeof(*p) + opt_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_OPTMGMT_REQ;
			p->mtp_opt_length = opt_len;
			p->mtp_opt_offset = opt_len ? sizeof(*p) : 0;
			p->mtp_mgmt_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			printd(("%s: %p: MTP_OPTMGMT_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_transfer_req: - issue an MTP_TRANSFER_REQ primitive downstream
 * @ss: private structure
 * @q: active queue
 * @dst_ptr: destination pointer
 * @dst_len: destination length
 * @mp: message priority
 * @sls: sequence selection
 * @dp: user data
 */
STATIC INLINE int
mtp_transfer_req(struct ss *ss, queue_t *q, caddr_t dst_ptr, size_t dst_len, dl_ulong mp,
		 dl_ulong sls, mblk_t *dp)
{
	mblk_t *mp;
	struct MTP_transfer_req *p;
	size_t mlen = sizeof(*p) + dst_len;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED)) != NULL) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_TRANSFER_REQ;
			p->mtp_dest_length = dst_len;
			p->mtp_dest_offset = dst_len ? sizeof(*p) : 0;
			p->mtp_mp = mp;
			p->mtp_sls = sls;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			printd(("%s: %p: MTP_TRANSFER_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_ABSORBED);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#endif				/* __LOCAL_UA_MTP_H__ */
