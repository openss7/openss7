/*****************************************************************************

 @(#) $RCSfile: dl_sl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/23 13:03:58 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/12/23 13:03:58 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl_sl.c,v $
 Revision 0.9.2.1  2006/12/23 13:03:58  brian
 - added new conversion module

 *****************************************************************************/

#ident "@(#) $RCSfile: dl_sl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/23 13:03:58 $"

static char const ident[] = "$RCSfile: dl_sl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/23 13:03:58 $";

/*
 *  This is a simple conversion module that converts between the OpenSS7 Signalling Link Interface
 *  (SLI) and the Data Link Provider Interface (DLPI) for SS7 links.
 */

#include <sys/os7/compat.h>

#define DL_SL_DESCRIP	"DLPI/SLI CONVERSION STREAMS MODULE."
#define DL_SL_REVISION	"OpenSS7 $RCSfile: dl_sl.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/23 13:03:58 $"
#define DL_SL_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define DL_SL_DEVICE	"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define DL_SL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DL_SL_LICENSE	"GPL"
#define DL_SL_BANNER	DL_SL_DESCRIP	"\n" \
			DL_SL_REVISION	"\n" \
			DL_SL_COPYRIGHT	"\n" \
			DL_SL_DEVICE	"\n" \
			DL_SL_CONTACT	"\n"
#define DL_SL_SPLASH	DL_SL_DEVICE	" - " \
			DL_SL_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(DL_SL_CONTACT);
MODULE_DESCRIPTION(DL_SL_DESCRIP);
MODULE_SUPPORTED_DEVICE(DL_SL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DL_SL_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-dl_sl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define DL_SL_MOD_ID		CONFIG_STREAMS_DL_SL_MODID
#define DL_SL_MOD_NAME		CONFIG_STREAMS_DL_SL_NAME
#endif				/* LFS */

#define MOD_ID		DL_SL_MOD_ID
#define MOD_NAME	DL_SL_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH	DL_SL_BANNER
#else				/* MODULE */
#define MOD_SPLASH	DL_SL_SPLASH
#endif				/* MODULE */

static struct module_info dl_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat dl_mstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

const char *
dl_d_state_name(int state)
{
	switch (state) {
	case DL_UNATTACHED:
		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:
		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:
		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:
		return ("DL_UNBOUND");
	case DL_BIND_PENDING:
		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:
		return ("DL_UNBIND_PENDING");
	case DL_IDLE:
		return ("DL_IDLE");
	case DL_UDQOS_PENDING:
		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:
		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:
		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:
		return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:
		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:
		return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:
		return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:
		return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:
		return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:
		return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:
		return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:
		return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:
		return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:
		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:
		return ("DL_SUBS_UNBIND_PND");
	default:
		return ("DL_????_???");
	}
}
static inline int
dl_get_d_state(struct dl *dl)
{
	return (dl->info.d.dl_current_state);
}
static inline int
dl_get_d_statef(struct dl *dl)
{
	return ((1 << dl_get_d_state(dl)));
}
static inline int
dl_chk_d_state(struct dl *dl, int mask)
{
	return (dl_get_d_statef(dl) & mask);
}
static inline int
dl_not_d_state(struct dl *dl, int mask)
{
	return (dl_chk_d_state(dl, ~mask));
}
static int
dl_set_d_state(struct dl *dl, int newstate)
{
	int oldstate = dl_get_d_state(dl);

	strlog(dl->mid, dl->sid, DLLOGST, SL_TRACE, "%s <- %s", dl_d_state_name(newstate),
	       dl_d_state_name(oldstate));
	return (dl->info.d.dl_current_state = newstate);
}

const char *
dl_s_state_name(int state)
{
	switch (state) {
	case LMI_UNATTACHED:
		return ("LMI_UNATTACHED");
	case LMI_ATTACH_PENDING:
		return ("LMI_ATTACH_PENDING");
	case LMI_UNUSABLE:
		return ("LMI_UNUSABLE");
	case LMI_DISABLED:
		return ("LMI_DISABLED");
	case LMI_ENABLE_PENDING:
		return ("LMI_ENABLE_PENDING");
	case LMI_ENABLED:
		return ("LMI_ENABLED");
	case LMI_DISABLE_PENDING:
		return ("LMI_DISABLE_PENDING");
	case LMI_DETACH_PENDING:
		return ("LMI_DETACH_PENDING");
	default:
		return ("LMI_????_???");
	}
}
static inline int
dl_get_s_state(struct dl *dl)
{
	return (dl->info.s.lmi_state);
}
static inline int
dl_get_s_statef(struct dl *dl)
{
	return ((1 << dl_get_s_state(dl)));
}
static inline int
dl_chk_s_state(struct dl *dl, int mask)
{
	return (dl_get_s_statef(dl) & mask);
}
static inline int
dl_not_s_state(struct dl *dl, int mask)
{
	return (dl_chk_s_state(dl, ~mask));
}
static int
dl_set_s_state(struct dl *dl, int newstate)
{
	int oldstate = dl_get_s_state(dl);

	strlog(dl->mid, dl->sid, DLLOGST, SL_TRACE, "%s <- %s", dl_s_state_name(newstate),
	       dl_s_state_name(oldstate));
	return (dl->info.s.lmi_state = newstate);
}

/*
 *  Buffer allocation
 */
/**
 * dl_allocb: - allocate a buffer reliably
 * @q: active queue
 * @size: size of allocation
 * @priority: priority of allocation
 */
static inline fastcall mblk_t *
dl_allocb(queue_t *q, size_t size, int priority)
{
	mblk_t *mp;

	if (unlikely(!(mp = allocb(size, priority))))
		mi_bufcall(q, size, priority);
	return (mp);
}

/**
 * dl_trylock: - try to lock a DL queue pair
 * @dl: private structure
 * @q: active queue
 *
 * Note that if we always run (at least initially) from a service procedure, there is not need to
 * suppres interrupts while holding the lock.  This simple lock will do because the service
 * procedure is guaranteed to run single threaded.  Also, because interrupts do not need to be
 * suppressed while holding the lock, the current task pointer identifies the thread and the thread
 * can be allowed to recurse on the lock.  When a queue procedure fails to acquire the lock, it is
 * marked to have its service procedure scheduled later, but we only remember one queue, so if there
 * are two waiting, the second one is reenabled.
 */
static inline bool
dl_trylock(struct dl *dl, queue_t *q)
{
	bool rtn = false;
	unsigned long flags;

	spin_lock_irqsave(&dl->lock, flags);
	if (dl->users == 0 && (q->q_flag & QSVCBUSY)) {
		rtn = true;
		dl->users = 1;
		dl->owner = current;
	} else if (dl->users != 0 && dl->owner == current) {
		rtn = true;
		dl->users++;
	} else if (!dl->waitq) {
		dl->waitq = q;
	} else if (dl->waitq != q) {
		qenable(q);
	}
	spin_unlock_irqrestore(&dl->lock, flags);
	return (rtn);
}

/**
 * dl_unlock: - unlock a DL queue pair
 * @dl: private structure
 *
 * If the a queue wanted service, enable it.
 */
static inline void
dl_unlock(struct dl *dl)
{
	unsigned long flags;

	spin_lock_irqsave(&dl->lock, flags);
	if (--dl->users == 0 && dl->waitq) {
		qenable(dl->waitq);
		dl->waitq = NULL;
	}
	spin_unlock_irqrestore(&dl->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LMI User -> LMI Provider primitives.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * lmi_info_req: - issue LMI_INFO_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_info_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	lmi_info_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "LMI_INFO_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_attach_req: - issue LMI_ATTACH_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_attach_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	lmi_attach_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ATTACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "LMI_ATTACH_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_detach_req: - issue LMI_DETACH_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_detach_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	lmi_detach_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "LMI_DETACH_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_enable_req: - issue LMI_ENABLE_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_enable_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	lmi_enable_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "LMI_ENABLE_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_req: - issue LMI_DISABLE_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_disable_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	lmi_disable_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "LMI_DISABLE_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_optmgmt_req: - issue LMI_OPTMGMT_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_optmgmt_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	lmi_optmgmt_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "LMI_OPTMGMT_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL User -> SL Provider primitives.
 *
 *  -------------------------------------------------------------------------
 */

/**
 * sl_pdu_req: - issue SL_PDU_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_pdu_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_pdu_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_PDU_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_PDU_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_emergency_req: - issue SL_EMERGENCY_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_emergency_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_emergency_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_EMERGENCY_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_EMERGENCY_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_emergency_ceases_req: - issue SL_EMERGENCY_CEASES_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_emergency_ceases_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_emergency_ceases_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_EMERGENCY_CEASES_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_start_req: - issue SL_START_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_start_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_start_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_START_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_START_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_stop_req: - issue SL_STOP_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_stop_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_stop_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_STOP_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_STOP_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieve_bsnt_req: - issue SL_RETRIEVE_BSNT_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_retrieve_bsnt_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_retrieve_bsnt_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVE_BSNT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_RETRIEVE_BSNT_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_request_and_fsnc_req: - issue SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_retrieval_request_and_fsnc_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_retrieval_req_and_fsnc_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_REQUEST_AND_FSNC_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_RETRIEVAL_REQUEST_AND_FSNC_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_clear_buffers_req: - issue SL_CLEAR_BUFFERS_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_clear_buffers_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_clear_buffers_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_CLEAR_BUFFERS_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_clear_rtb_req: - issue SL_CLEAR_RTB_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_clear_rtb_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_clear_rtb_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CLEAR_RTB_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_CLEAR_RTB_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_continue_req: - issue SL_CONTINUE_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_continue_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_continue_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CONTINUE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_CONTINUE_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_local_processor_outage_req: - issue SL_LOCAL_PROCESSOR_OUTAGE_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_local_processor_outage_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_local_proc_outage_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_LOCAL_PROCESSOR_OUTAGE_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_resume_req: - issue SL_RESUME_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_resume_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_resume_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RESUME_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_RESUME_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_congestion_discard_req: - issue SL_CONGESTION_DISCARD_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_congestion_discard_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_cong_discard_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_CONGESTION_DISCARD_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_congestion_accept_req: - issue SL_CONGESTION_ACCEPT_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_congestion_accept_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_cong_accept_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_CONGESTION_ACCEPT_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_no_congestion_req: - issue SL_NO_CONGESTION_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_no_congestion_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_no_cong_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_NO_CONGESTION_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_NO_CONGESTION_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_power_on_req: - issue SL_POWER_ON_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_power_on_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_power_on_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_POWER_ON_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_POWER_ON_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_optmgmt_req: - issue SL_OPTMGMT_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_optmgmt_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_optmgmt_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OPTMGMT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_OPTMGMT_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_notify_req: - issue SL_NOTIFY_REQ primitive downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_notify_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	sl_notify_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_NOTIFY_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "SL_NOTIFY_REQ ->");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  DL Provider -> DL User primitives.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * dl_info_ack: - issue DL_INFO_ACK primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_INFO_ACK");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_bind_ack: - issue DL_BIND_ACK primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_bind_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_BIND_ACK");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_ok_ack: - issue DL_OK_ACK primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_OK_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_OK_ACK");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_error_ack: - issue DL_ERROR_ACK primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_ERROR_ACK");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_subs_bind_ack: - issue DL_SUBS_BIND_ACK primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_subs_bind_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_SUBS_BIND_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_SUBS_BIND_ACK");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_unitdata_ind: - issue DL_UNITDATA_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_UNITDATA_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_UNITDATA_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_uderror_ind: - issue DL_UDERROR_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_uderror_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_UDERROR_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_UDERROR_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_connect_ind: - issue DL_CONNECT_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_connect_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_CONNECT_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_CONNECT_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_connect_con: - issue DL_CONNECT_CON primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_CONNECT_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_CONNECT_CON");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_token_ack: - issue DL_TOKEN_ACK primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_token_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TOKEN_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_TOKEN_ACK");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_disconnect_ind: - issue DL_DISCONNECT_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DISCONNECT_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_DISCONNECT_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reset_ind: - issue DL_RESET_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_RESET_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_RESET_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reset_con: - issue DL_RESET_CON primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_RESET_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_RESET_CON");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_data_ack_ind: - issue DL_DATA_ACK_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_data_ack_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_data_ack_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DATA_ACK_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_DATA_ACK_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_data_ack_status_ind: - issue DL_DATA_ACK_STATUS_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_data_ack_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_data_ack_status_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_DATA_ACK_STATUS_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_DATA_ACK_STATUS_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reply_ind: - issue DL_REPLY_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_reply_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_REPLY_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_REPLY_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reply_status_ind: - issue DL_REPLY_STATUS_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_reply_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_status_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_REPLY_STATUS_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_REPLY_STATUS_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reply_update_status_ind: - issue DL_REPLY_UPDATE_STATUS_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_reply_update_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_update_status_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_REPLY_UPDATE_STATUS_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_xid_ind: - issue DL_XID_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_xid_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_XID_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_XID_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_xid_con: - issue DL_XID_CON primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_xid_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_XID_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_XID_CON");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_test_ind: - issue DL_TEST_IND primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_test_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TEST_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_TEST_IND");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_test_con: - issue DL_TEST_CON primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_test_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TEST_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_TEST_CON");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_phys_addr_ack: - issue DL_PHYS_ADDR_ACK primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_phys_addr_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_phys_addr_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_PHYS_ADDR_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_PHYS_ADDR_ACK");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_get_statistics_ack: - issue DL_STATISTICS_ACK primitive upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = dl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_GET_STATISTICS_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(dl->mid, dl->sid, DLLOGTX, SL_TRACE, "<- DL_GET_STATISTICS_ACK");
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LMI Provider -> LMI User primitives.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * lmi_info_ack: - process LMI_INFO_ACK primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
lmi_info_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * lmi_ok_ack: - process LMI_OK_ACK primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
lmi_ok_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	lmi_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * lmi_error_ack: - process LMI_ERROR_ACK primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
lmi_error_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * lmi_enable_con: - process LMI_ENABLE_CON primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
lmi_enable_con(struct dl *dl, queue_t *q, mblk_t *mp)
{
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * lmi_disable_con: - process LMI_DISABL_CON primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
lmi_disable_con(struct dl *dl, queue_t *q, mblk_t *mp)
{
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * lmi_optmgmt_ack: - process LMI_OPTMGMT_ACK primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
lmi_optmgmt_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * lmi_error_ind: - process LMI_ERROR_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
lmi_error_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * lmi_stats_ind: - process LMI_STATS_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
lmi_stats_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * lmi_event_ind: - process LMI_EVENT_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
lmi_event_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL Provider -> SL User primitives.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * sl_pdu_ind: - process SL_PDU_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_pdu_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_pdu_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_link_congested_ind: - process SL_LINK_CONGESTED_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_link_congested_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_link_congestion_ceased_ind: - process SL_LINK_CONGESTION_CEASED_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_link_congestion_ceased_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ceased_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_retrieved_message_ind: - process SL_RETRIEVED_MESSAGE_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_retrieved_message_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_retrieved_msg_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_retrieval_complete_ind: - process SL_RETRIEVAL_COMPLETE_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_retrieval_complete_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_comp_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_rb_cleared_ind: - process SL_RB_CLEARED_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_rb_cleared_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_rb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_bsnt_ind: - process SL_BSNT_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_bsnt_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_bsnt_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_in_service_ind: - process SL_IN_SERVICE_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_in_service_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_in_service_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_out_of_service_ind: - process SL_OUT_OF_SERVICE_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_out_of_service_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_out_of_service_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_remote_processor_outage_ind: - process SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_remote_processor_outage_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_rem_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_remote_processor_recovered_ind: - process SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_remote_processor_recovered_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_rem_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_rtb_cleared_ind: - process SL_RTB_CLEARED_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_rtb_cleared_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_rtb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_retrieval_not_possible_ind: - process SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_retrieval_not_possible_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_not_poss_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_bsnt_not_retrievable_ind: - process SL_BSNT_NOT_RETRIEVABLE_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_bsnt_not_retrievable_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_bsnt_not_retr_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_optmgmt_ack: - process SL_OPTMGMT_ACK primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_optmgmt_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_notify_ind: - process SL_NOTIFY_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_notify_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_notify_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_local_processor_outage_ind: - process SL_LOCAL_PROCESSOR_OUTAGE_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_local_processor_outage_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_loc_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_local_processor_recovered_ind: - process SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_local_processor_recovered_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_loc_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_other_ind: - process unknown primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
sl_other_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	sl_ulong *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  DL User -> DL Provider primitives.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * dl_info_req: - process DL_INFO_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_info_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_attach_req: - process DL_ATTACH_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_detach_req: - process DL_DETACH_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_detach_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_bind_req: - process DL_BIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_unbind_req: - process DL_UNBIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unbind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_subs_bind_req: - process DL_SUBS_BIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_subs_unbind_req: - process DL_SUBS_UNBIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_unbind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_enabmulti_req: - process DL_ENABMULTI_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_enabmulti_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_disabmulti_req: - process DL_DISABMULTI_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disabmulti_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_promiscon_req: - process DL_PROMISCON_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscon_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_promiscoff_req: - process DL_PROMISCOFF_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscoff_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_unitdata_req: - process DL_UNITDATA_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_udqos_req: - process DL_UDQOS_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_udqos_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_connect_req: - process DL_CONNECT_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_connect_res: - process DL_CONNECT_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_token_req: - process DL_TOKEN_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_token_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_disconnect_req: - process DL_DISCONNECT_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_reset_req: - process DL_RESET_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_reset_res: - process DL_RESET_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_data_ack_req: - process DL_DATA_ACK_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_data_ack_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_reply_req: - process DL_REPLY_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_reply_update_req: - process DL_REPLY_UDPATE_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_update_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_xid_req: - process DL_XID_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_xid_res: - process DL_XID_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_test_req: - process DL_TEST_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_test_res: - process DL_TEST_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_phys_addr_req: - process DL_PHYS_ADDR_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_set_phys_addr_req: - process DL_SET_PHYS_ADDR_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_set_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_get_statistics_req: - process DL_GET_STATISTICS_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_get_statistics_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

/**
 * dl_other_req: - process unknown primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
dl_other_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, -EMSGSIZE);
}

static fastcall int
dl_r_data_slow(queue_t *q, mblk_t *mp)
{
	if (bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
dl_r_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	int old_d_state, old_s_state;
	int rtn;

	if (!dl_trylock(dl, q))
		return (-EDEADLK);

	old_d_state = dl_get_d_state(dl);
	old_s_state = dl_get_s_state(dl);

	switch (*(sl_ulong *) mp->b_rptr) {
	case LMI_INFO_ACK:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "LMI_INFO_ACK <-");
		rtn = lmi_info_ack(dl, q, mp);
		break;
	case LMI_OK_ACK:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "LMI_OK_ACK <-");
		rtn = lmi_ok_ack(dl, q, mp);
		break;
	case LMI_ERROR_ACK:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "LMI_ERROR_ACK <-");
		rtn = lmi_error_ack(dl, q, mp);
		break;
	case LMI_ENABLE_CON:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "LMI_ENABLE_CON <-");
		rtn = lmi_enable_con(dl, q, mp);
		break;
	case LMI_DISABLE_CON:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "LMI_DISABLE_CON <-");
		rtn = lmi_disable_con(dl, q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "LMI_OPTMGMT_ACK <-");
		rtn = lmi_optmgmt_ack(dl, q, mp);
		break;
	case LMI_ERROR_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "LMI_ERROR_IND <-");
		rtn = lmi_error_ind(dl, q, mp);
		break;
	case LMI_STATS_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "LMI_STATS_IND <-");
		rtn = lmi_stats_ind(dl, q, mp);
		break;
	case LMI_EVENT_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "LMI_EVENT_IND <-");
		rtn = lmi_event_ind(dl, q, mp);
		break;
	case SL_PDU_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_PDU_IND <-");
		rtn = sl_pdu_ind(dl, q, mp);
		break;
	case SL_LINK_CONGESTED_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_LINK_CONGESTED_IND <-");
		rtn = sl_link_congested_ind(dl, q, mp);
		break;
	case SL_LINK_CONGESTION_CEASED_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_LINK_CONGESTION_CEASED_IND <-");
		rtn = sl_link_congestion_ceased_ind(dl, q, mp);
		break;
	case SL_RETRIEVED_MESSAGE_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_RETRIEVED_MESSAGE_IND <-");
		rtn = sl_retrieved_message_ind(dl, q, mp);
		break;
	case SL_RETRIEVAL_COMPLETE_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_RETRIEVAL_COMPLETE_IND <-");
		rtn = sl_retrieval_complete_ind(dl, q, mp);
		break;
	case SL_RB_CLEARED_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_RB_CLEARED_IND <-");
		rtn = sl_rb_cleared_ind(dl, q, mp);
		break;
	case SL_BSNT_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_BSNT_IND <-");
		rtn = sl_bsnt_ind(dl, q, mp);
		break;
	case SL_IN_SERVICE_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_IN_SERVICE_IND <-");
		rtn = sl_in_service_ind(dl, q, mp);
		break;
	case SL_OUT_OF_SERVICE_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_OUT_OF_SERVICE_IND <-");
		rtn = sl_out_of_service_ind(dl, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_REMOTE_PROCESSOR_OUTAGE_IND <-");
		rtn = sl_remote_processor_outage_ind(dl, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_REMOTE_PROCESSOR_RECOVERED_IND <-");
		rtn = sl_remote_processor_recovered_ind(dl, q, mp);
		break;
	case SL_RTB_CLEARED_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_RTB_CLEARED_IND <-");
		rtn = sl_rtb_cleared_ind(dl, q, mp);
		break;
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_RETRIEVAL_NOT_POSSIBLE_IND <-");
		rtn = sl_retrieval_not_possible_ind(dl, q, mp);
		break;
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_BSNT_NOT_RETRIEVABLE_IND <-");
		rtn = sl_bsnt_not_retrievable_ind(dl, q, mp);
		break;
	case SL_OPTMGMT_ACK:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_OPTMGMT_ACK <-");
		rtn = sl_optmgmt_ack(dl, q, mp);
		break;
	case SL_NOTIFY_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_NOTIFY_IND <-");
		rtn = sl_notify_ind(dl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_LOCAL_PROCESSOR_OUTAGE_IND <-");
		rtn = sl_local_processor_outage_ind(dl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_RECOVERED_IND:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_LOCAL_PROCESSOR_RECOVERED_IND <-");
		rtn = sl_local_processor_recovered_ind(dl, q, mp);
		break;
	default:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "SL_????_??? <-");
		rtn = sl_other_ind(dl, q, mp);
		break;
	}
	if (rtn) {
		dl_set_d_state(dl, old_d_state);
		dl_set_s_state(dl, old_s_state);
	}
	dl_unlock(dl);
	return (rtn);
}
static fastcall int
dl_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static fastcall int
dl_r_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(mp->b_datap->db_type) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);

}

static noinline fastcall int
dl_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return dl_r_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(q, mp);
	case M_FLUSH:
		return dl_r_flush(q, mp);
	default:
		return dl_r_other(q, mp);
	}
}
static inline fastcall int
dl_r_msg(queue_t *q, mblk_t *mp)
{
	return dl_r_msg_slow(q, mp);
}

static fastcall int
dl_w_data_slow(queue_t *q, mblk_t *mp)
{
	if (bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
static fastcall int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	int old_d_state, old_s_state;
	int rtn;

	if (!dl_trylock(dl, q))
		return (-EDEADLK);

	old_d_state = dl_get_d_state(dl);
	old_s_state = dl_get_s_state(dl);

	switch (*(dl_ulong *) mp->b_rptr) {
	case DL_INFO_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_INFO_REQ");
		rtn = dl_info_req(dl, q, mp);
		break;
	case DL_ATTACH_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_ATTACH_REQ");
		rtn = dl_attach_req(dl, q, mp);
		break;
	case DL_DETACH_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_DETACH_REQ");
		rtn = dl_detach_req(dl, q, mp);
		break;
	case DL_BIND_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_BIND_REQ");
		rtn = dl_bind_req(dl, q, mp);
		break;
	case DL_UNBIND_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_UNBIND_REQ");
		rtn = dl_unbind_req(dl, q, mp);
		break;
	case DL_SUBS_BIND_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_SUBS_BIND_REQ");
		rtn = dl_subs_bind_req(dl, q, mp);
		break;
	case DL_SUBS_UNBIND_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_SUBS_UNBIND_REQ");
		rtn = dl_subs_unbind_req(dl, q, mp);
		break;
	case DL_ENABMULTI_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_ENABMULTI_REQ");
		rtn = dl_enabmulti_req(dl, q, mp);
		break;
	case DL_DISABMULTI_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_DISABMULTI_REQ");
		rtn = dl_disabmulti_req(dl, q, mp);
		break;
	case DL_PROMISCON_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_PROMISCON_REQ");
		rtn = dl_promiscon_req(dl, q, mp);
		break;
	case DL_PROMISCOFF_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_PROMISCOFF_REQ");
		rtn = dl_promiscoff_req(dl, q, mp);
		break;
	case DL_UNITDATA_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_UNITDATA_REQ");
		rtn = dl_unitdata_req(dl, q, mp);
		break;
	case DL_UDQOS_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_UDQOS_REQ");
		rtn = dl_udqos_req(dl, q, mp);
		break;
	case DL_CONNECT_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_CONNECT_REQ");
		rtn = dl_connect_req(dl, q, mp);
		break;
	case DL_CONNECT_RES:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_CONNECT_RES");
		rtn = dl_connect_res(dl, q, mp);
		break;
	case DL_TOKEN_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_TOKEN_REQ");
		rtn = dl_token_req(dl, q, mp);
		break;
	case DL_DISCONNECT_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_DISCONNECT_REQ");
		rtn = dl_disconnect_req(dl, q, mp);
		break;
	case DL_RESET_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_RESET_REQ");
		rtn = dl_reset_req(dl, q, mp);
		break;
	case DL_RESET_RES:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_RESET_RES");
		rtn = dl_reset_res(dl, q, mp);
		break;
	case DL_DATA_ACK_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_DATA_ACK_REQ");
		rtn = dl_data_ack_req(dl, q, mp);
		break;
	case DL_REPLY_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_REPLY_REQ");
		rtn = dl_reply_req(dl, q, mp);
		break;
	case DL_REPLY_UPDATE_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_REPLY_UPDATE_REQ");
		rtn = dl_reply_update_req(dl, q, mp);
		break;
	case DL_XID_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_XID_REQ");
		rtn = dl_xid_req(dl, q, mp);
		break;
	case DL_XID_RES:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_XID_RES");
		rtn = dl_xid_res(dl, q, mp);
		break;
	case DL_TEST_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_TEST_REQ");
		rtn = dl_test_req(dl, q, mp);
		break;
	case DL_TEST_RES:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_TEST_RES");
		rtn = dl_test_res(dl, q, mp);
		break;
	case DL_PHYS_ADDR_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_PHYS_ADDR_REQ");
		rtn = dl_phys_addr_req(dl, q, mp);
		break;
	case DL_SET_PHYS_ADDR_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_SET_PHYS_ADDR_REQ");
		rtn = dl_set_phys_addr_req(dl, q, mp);
		break;
	case DL_GET_STATISTICS_REQ:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_GET_STATISTICS_REQ");
		rtn = dl_get_statistics_req(dl, q, mp);
		break;
	default:
		strlog(dl->mid, dl->sid, DLLOGRX, SL_TRACE, "-> DL_????_??");
		rtn = dl_other_req(dl, q, mp);
		break;
	}
	if (rtn) {
		dl_set_d_state(dl, old_d_state);
		dl_set_w_state(dl, old_s_state);
	}
	dl_unlock(dl);
	return (rtn);
}
static fastcall int
dl_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static fastcall int
dl_w_ioctl(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static fastcall int
dl_w_iocdata(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static fastcall int
dl_w_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(mp->b_datap->db_type) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
dl_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return dl_w_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(q, mp);
	case M_FLUSH:
		return dl_w_flush(q, mp);
	case M_IOCTL:
		return dl_w_ioctl(q, mp);
	case M_IOCDATA:
		return dl_w_iocdata(q, mp);
	default:
		return dl_w_other(q, mp);
	}
}
static inline fastcall int
dl_w_msg(queue_t *q, mblk_t *mp)
{
	return dl_w_msg_slow(q, mp);
}

static streamscall __hot_in int
dl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || dl_r_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_read int
dl_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (dl_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_write int
dl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || dl_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
dl_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (dl_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static caddr_t dl_opens = NULL;

static streamscall __unlikely int
dl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct dl *dl;
	mblk_t *mp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */

	while (!(mp = allocb(sizeof(lmi_info_req_t), BPRI_WAITOK))) ;

	if ((err = mi_open_comm(&dl_opens, sizeof(*dl), q, devp, oflags, sflag, crp)))
		return (err);

	dl = DL_PRIV(q);
	/* initialize the structure */

	/* issue an immediate information request */
	mp->b_datap->db_type = M_PCPROTO;
	((lmi_info_req_t *) mp->b_wptr)->lmi_primitive = LMI_INFO_REQ;
	mp->b_wptr += sizeof(lmi_info_req_t);
	qprocson(q);
	putnext(q, mp);
	return (0);
}

static streamscall __unlikely int
dl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	mi_close_comm(&dl_opens, q);
	return (0);
}

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for the DL-SL module. (0 for allocation.)");

static struct qinit dl_rinit = {
	.qi_putp = dl_rput,
	.qi_srvp = dl_rsrv,
	.qi_qopen = dl_qopen,
	.qi_qclose = dl_qclose,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_mstat,
};

static struct qinit dl_winit = {
	.qi_putp = dl_wput,
	.qi_srvp = dl_wsrv,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_mstat,
};

static struct streamtab dl_slinfo = {
	.st_rdinit = &dl_rinit,
	.st_wrinit = &dl_winit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif

static struct fmodsw dl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &dl_slinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
dl_slinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_SPLASH);
	if ((err = register_strmod(&dl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module id %d\n", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
dl_slexit(void)
{
	int err;

	if ((err = unregister_strmod(&dl_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d\n", MOD_NAME, err);
	return;
}

module_init(dl_slinit);
module_exit(dl_slexit);
