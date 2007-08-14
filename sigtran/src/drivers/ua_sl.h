/*****************************************************************************

 @(#) $Id: ua_sl.h,v 0.9.2.2 2007/08/14 08:34:01 brian Exp $

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

 $Log: ua_sl.h,v $
 Revision 0.9.2.2  2007/08/14 08:34:01  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/11/30 13:17:55  brian
 - added files from strss7 package

 *****************************************************************************/

#ifndef __LOCAL_UA_SL_H__
#define __LOCAL_UA_SL_H__

#ident "@(#) $RCSfile: ua_sl.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/*
 *  This file contains inline functions for use with the Signaling Link Interface.
 */

/*
 *  SL User Primitives (to SL User)
 */

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
sl_info_ack(struct ss *ss, queue_t *q, uchar *ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_info_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = ss_get_i_state(ss);
		p->lmi_max_sdu = 272 + 1 + ((ss->proto.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((ss->proto.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: <- LMI_INFO_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
sl_ok_ack(struct ss *ss, queue_t *q, sl_long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (ss_get_i_state(ss)) {
		case LMI_ATTACH_PENDING:
			ss_set_i_state(ss, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			ss_set_i_state(ss, LMI_UNATTACHED);
			break;
		default:
			break;
		}
		p->lmi_state = ss_get_i_state(ss);
		printd(("%s: %p: <- LMI_OK_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
sl_error_ack(struct ss *ss, queue_t *q, sl_long prim, sl_long error)
{
	mblk_t *mp;
	lmi_error_ack_t *p;

	/* filter out queue returns */
	switch (error) {
	case QR_ABSORBED:
	case QR_TRIMMED:
	case QR_LOOP:
	case QR_PASSALONG:
	case QR_PASSFLOW:
	case QR_DISABLE:
	case QR_STRIP:
	case QR_RETRY:
	case -EBUSY:
	case -ENOBUFS:
	case -ENOMEM:
	case -EAGAIN:
		return (error);
	}
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_reason = error > 0 ? error : LMI_SYSERR;
		p->lmi_errno = error > 0 ? 0 : -error;
		p->lmi_error_primitive = prim;
		switch (ss_get_i_state(ss)) {
		case LMI_ATTACH_PENDING:
			ss_set_i_state(ss, LMI_UNATTACHED);
			break;
		case LMI_DETACH_PENDING:
			ss_set_i_state(ss, LMI_DISABLED);
			break;
		case LMI_ENABLE_PENDING:
			ss_set_i_state(ss, LMI_DISABLED);
			break;
		case LMI_DISABLE_PENDING:
			ss_set_i_state(ss, LMI_ENABLED);
			break;
		default:
			break;
		}
		p->lmi_state = ss_get_i_state(ss);
		printd(("%s: %p: <- LMI_ERROR_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
sl_enable_con(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	lmi_enable_con_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_CON;
		assure(ss_get_i_state(ss) == LMI_ENABLE_PENDING);
		ss_set_i_state(ss, LMI_ENABLED);
		p->lmi_state = ss_get_i_state(ss);
		printd(("%s: %p: <- LMI_ENABLE_CON\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
sl_disable_con(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	lmi_disable_con_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_CON;
		assure(ss_get_i_state(ss) == LMI_DISABLE_PENDING);
		ss_set_i_state(ss, LMI_DISABLED);
		p->lmi_state = ss_get_i_state(ss);
		printd(("%s: %p: <- LMI_DISABLE_CON\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
sl_optmgmt_ack(struct ss *ss, queue_t *q, uint32_t flags, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_error_ind(struct ss *ss, queue_t *q, uint32_t errno, uint32_t reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;	/* FIXME */
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = ss_get_i_state(ss);
		printd(("%s: %p: <- LMI_ERROR_IND\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_stats_ind(struct ss *ss, queue_t *q, uint32_t interval, mblk_t *dp)
{
	mblk_t *mp;
	lmi_stats_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = drv_hztomsec(jiffies);
			mp->b_cont = dp;
			printd(("%s: %p: <- LMI_STATS_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_event_ind(struct ss *ss, queue_t *q, uint32_t oid, uint32_t level)
{
	mblk_t *mp;
	lmi_event_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = drv_hztomsec(jiffies);
			p->lmi_severity = level;
			printd(("%s: %p: <- LMI_EVENT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_PDU_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_pdu_ind(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	mblk_t *dp;

	assure(!m->mp->b_cont);
	if ((dp = ss7_dupb(q, m->mp))) {
		uint32_t mpri;
		mblk_t *mp;
		sl_pdu_ind_t *p;

		if (m->data1.ptr.c) {
			mpri = 0;
			dp->b_rptr = dp->b_wptr = m->data1.ptr.c;
			dp->b_wptr += m->data1.len;
		} else if (m->data2.ptr.c) {
			mpri = m->data2.ptr.c[0] & 0x3;
			dp->b_rptr = dp->b_wptr = m->data2.ptr.c + 1;
			dp->b_wptr += m->data2.len - 1;
		} else {
			freeb(dp);
			swerr();
			return (-EFAULT);
		}
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			if (canputnext(ss->oq)) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->sl_primitive = SL_PDU_IND;
				p->sl_mp = mpri;
				mp->b_cont = dp;
				printd(("%s: %p: <- SL_PDU_IND\n", DRV_NAME, ss));
				putnext(ss->oq, mp);
				return (QR_DONE);
			}
			freemsg(mp);
			return (-EBUSY);
		}
		freeb(dp);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_link_congested_ind(struct ss *ss, queue_t *q, sl_ulong cong, sl_ulong disc)
{
	mblk_t *mp;
	sl_link_cong_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;	/* FIXME */
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		printd(("%s: %p: <- SL_LINK_CONGESTED_IND\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_link_congestion_ceased_ind(struct ss *ss, queue_t *q, sl_ulong cong, sl_ulong disc)
{
	mblk_t *mp;
	sl_link_cong_ceased_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;	/* FIXME */
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		printd(("%s: %p: <- SL_LINK_CONGESTION_CEASED_IND\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieved_message_ind(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	mblk_t *dp;

	assure(!m->mp->b_cont);
	if ((dp = ss7_dupb(q, m->mp))) {
		uint32_t mpri;
		mblk_t *mp;
		sl_retrieved_msg_ind_t *p;

		if (m->data1.ptr.c) {
			mpri = 0;
			dp->b_rptr = dp->b_wptr = m->data1.ptr.c;
			dp->b_wptr += m->data1.len;
		} else if (m->data2.ptr.c) {
			mpri = m->data2.ptr.c[0] & 0x3;
			dp->b_rptr = dp->b_wptr = m->data2.ptr.c + 1;
			dp->b_wptr += m->data2.len - 1;
		} else {
			freeb(dp);
			swerr();
			return (-EFAULT);
		}
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			if (canputnext(ss->oq)) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
				p->sl_mp = mpri;
				mp->b_cont = dp;
				printd(("%s: %p: <- SL_RETRIEVED_MESSAGE_IND\n", DRV_NAME, ss));
				putnext(ss->oq, mp);
				return (QR_DONE);
			}
			freemsg(mp);
			return (-EBUSY);
		}
		freeb(dp);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieval_complete_ind(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	mblk_t *dp = NULL;

	if ((!m->data1.ptr.c && !m->data2.ptr.c) || (dp = ss7_dupb(q, m->mp))) {
		uint32_t mpri = 0;
		mblk_t *mp;
		sl_retrieval_comp_ind_t *p;

		if (m->data1.ptr.c) {
			mpri = 0;
			dp->b_rptr = dp->b_wptr = m->data1.ptr.c;
			dp->b_wptr += m->data1.len;
		}
		if (m->data2.ptr.c) {
			mpri = m->data2.ptr.c[0] & 0x3;
			dp->b_rptr = dp->b_wptr = m->data2.ptr.c + 1;
			dp->b_wptr += m->data2.len - 1;
		}
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			if (canputnext(ss->oq)) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
				p->sl_mp = mpri;
				mp->b_cont = dp;
				printd(("%s: %p: <- SL_RETRIEVAL_COMPLETE_IND\n", DRV_NAME, ss));
				putnext(ss->oq, mp);
				return (QR_DONE);
			}
			freemsg(mp);
			return (-EBUSY);
		}
		if (dp)
			freeb(dp);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RB_CLEARED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_rb_cleared_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_rb_cleared_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RB_CLEARED_IND;
			printd(("%s: %p: <- SL_RB_CLEARED_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_bsnt_ind(struct ss *ss, queue_t *q, sl_ulong bsnt)
{
	mblk_t *mp;
	sl_bsnt_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_BSNT_IND;
			p->sl_bsnt = bsnt;
			printd(("%s: %p: <- SL_BSNT_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_IN_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_in_service_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_in_service_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_IN_SERVICE_IND;
			printd(("%s: %p: <- SL_IN_SERVICE_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_OUT_OF_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_out_of_service_ind(struct ss *ss, queue_t *q, sl_ulong reason)
{
	mblk_t *mp;
	sl_out_of_service_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;	/* FIXME */
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_reason = reason;
		printd(("%s: %p: <- SL_OUT_OF_SERVICE_IND\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_local_processor_outage_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_loc_proc_out_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;	/* FIXME */
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		printd(("%s: %p: <- SL_LOCAL_PROCESSOR_OUTAGE_IND\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LOCAL_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_local_processor_recovered_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_loc_proc_recovered_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;	/* FIXME */
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LOCAL_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		printd(("%s: %p: <- SL_LOCAL_PROCESSOR_RECOVERED_IND\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_remote_processor_outage_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_rem_proc_out_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;	/* FIXME */
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_OUTAGE_IND\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_remote_processor_recovered_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_rem_proc_recovered_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;	/* FIXME */
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_RECOVERED_IND\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RTB_CLEARED_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_rtb_cleared_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_rtb_cleared_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RTB_CLEARED_IND;
			printd(("%s: %p: <- SL_RTB_CLEARED_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieval_not_possible_ind(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_retrieval_not_poss_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
			printd(("%s: %p: <- SL_RETRIEVAL_NOT_POSSIBLE_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_bsnt_not_retrievable_ind(struct ss *ss, queue_t *q, sl_ulong bsnt)
{
	mblk_t *mp;
	sl_bsnt_not_retr_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
			p->sl_bsnt = bsnt;
			printd(("%s: %p: <- SL_BSNT_NOT_RETRIEVABLE_IND\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  SL_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
sl_optmgmt_ack(struct ss *ss, queue_t *q, uchar *opt_ptr, size_t opt_len, uint32_t flags)
{
	mblk_t *mp;
	sl_optmgmt_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_OPTMGMT_ACK;
		p->opt_length = opt_len;
		p->opt_offset = opt_len ? sizeof(*p) : 0;
		p->mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- SL_OPTMGMT_ACK\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  SL_NOTIFY_IND
 *  -----------------------------------
 */
STATIC INLINE int
sl_notify_ind(struct ss *ss, queue_t *q, uint32_t oid, uint32_t level)
{
	mblk_t *mp;
	sl_notify_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_NOTIFY_IND;
		p->sl_objectid = oid;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_severity = level;
		printd(("%s: %p: <- SL_NOTIFY_IND\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  SLI Primitives sent downstream to SL Provider
 */
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_info_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	lmi_info_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_REQ;
		printd(("%s: %p: LMI_INFO_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_attach_req(struct ss *ss, queue_t *q, uchar *ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_attach_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ATTACH_REQ;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: LMI_ATTACH_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_detach_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	lmi_detach_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DETACH_REQ;
		printd(("%s: %p: LMI_DETACH_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_enable_req(struct ss *ss, queue_t *q, uchar *rem_ptr, size_t rem_len)
{
	mblk_t *mp;
	lmi_enable_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_REQ;
		bcopy(rem_ptr, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
		printd(("%s: %p: LMI_ENABLE_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_disable_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	lmi_disable_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_REQ;
		printd(("%s: %p: LMI_DISABLE_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_optmgmt_req(struct ss *ss, queue_t *q, uchar *opt_ptr, size_t opt_len, uint32_t flags)
{
	mblk_t *mp;
	lmi_optmgmt_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_OPTMGMT_REQ;
			p->lmi_opt_length = opt_len;
			p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
			p->lmi_mgmt_flags = flags;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			printd(("%s: %p: LMI_OPTMGMT_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
STATIC int
sl_pdu_req(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	mblk_t *dp;

	assure(!m->mp->b_cont);
	if ((dp = ss7_dupb(q, m->mp))) {
		uint32_t mpri;
		mblk_t *mp;
		sl_pdu_req_t *p;

		if (m->data1.ptr.c) {
			mpri = 0;
			dp->b_rptr = dp->b_wptr = m->data1.ptr.c;
			dp->b_wptr += m->data1.len;
		} else if (m->data2.ptr.c) {
			mpri = m->data2.ptr.c[0] & 0x3;
			dp->b_rptr = dp->b_wptr = m->data2.ptr.c + 1;
			dp->b_wptr += m->data2.len - 1;
		} else {
			freeb(dp);
			swerr();
			return (-EFAULT);
		}
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			if (canputnext(ss->oq)) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->sl_primitive = SL_PDU_REQ;
				p->sl_mp = mpri;
				mp->b_cont = dp;
				printd(("%s: %p: SL_PDU_REQ ->\n", DRV_NAME, ss));
				putnext(ss->oq, mp);
				return (QR_DONE);
			}
			freemsg(mp);
			return (-EBUSY);
		}
		freeb(dp);
	}
	return (-ENOBUFS);
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_emergency_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_emergency_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_EMERGENCY_REQ;
		printd(("%s: %p: SL_EMERGENCY_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_emergency_ceases_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_emergency_ceases_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
		printd(("%s: %p: SL_EMERGENCY_CEASES_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_start_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_start_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_START_REQ;
			printd(("%s: %p: SL_START_REQ ->\n", DRV_NAME, ss));
			puntext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_stop_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_stop_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_STOP_REQ;
			printd(("%s: %p: SL_STOP_REQ ->\n", DRV_NAME, ss));
			puntext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieve_bsnt_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_retrieve_bsnt_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RETRIEVE_BSNT_REQ;
			printd(("%s: %p: SL_RETRIEVE_BSNT_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_retrieval_request_and_fsnc_req(struct ss *ss, queue_t *q, sl_ulong fsnc)
{
	mblk_t *mp;
	sl_retrieval_req_and_fsnc_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RETRIEVAL_REQUEST_AND_FSNC_REQ;
			p->sl_fsnc = fsnc;
			printd(("%s: %p: SL_RETRIEVAL_REQUEST_AND_FSNC_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_clear_buffers_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_clear_buffers_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
			printd(("%s: %p: SL_CLEAR_BUFFERS_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_clear_rtb_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_clear_rtb_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_CLEAR_RTB_REQ;
			printd(("%s: %p: SL_CLEAR_RTB_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CONTINUE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_continue_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_continue_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_CONTINUE_REQ;
			printd(("%s: %p: SL_CONTINUE_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_local_processor_outage_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_local_proc_outage_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		printd(("%s: %p: SL_LOCAL_PROCESSOR_OUTAGE_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_resume_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_resume_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(ss->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RESUME_REQ;
			printd(("%s: %p: SL_RESUME_REQ ->\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_congestion_discard_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_cong_discard_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
		printd(("%s: %p: SL_CONGESTION_DISCARD_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_congestion_accept_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_cong_accept_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		printd(("%s: %p: SL_CONGESTION_ACCEPT_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_no_congestion_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_no_cong_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_NO_CONGESTION_REQ;
		printd(("%s: %p: SL_NO_CONGESTION_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_power_on_req(struct ss *ss, queue_t *q)
{
	mblk_t *mp;
	sl_power_on_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_POWER_ON_REQ;
		printd(("%s: %p: SL_POWER_ON_REQ ->\n", DRV_NAME, ss));
		putnext(ss->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

#endif				/* __LOCAL_UA_SL_H__ */
