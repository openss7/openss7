/*****************************************************************************

 @(#) $RCSfile: slmux.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:11 $

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

 Last Modified $Date: 2008-04-29 07:11:11 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slmux.c,v $
 Revision 0.9.2.4  2008-04-29 07:11:11  brian
 - updating headers for release

 Revision 0.9.2.3  2007/08/12 16:20:29  brian
 - new PPA handling

 Revision 0.9.2.2  2007/03/25 19:00:15  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.1  2007/01/21 20:20:11  brian
 - added documetation

 *****************************************************************************/

#ident "@(#) $RCSfile: slmux.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:11 $"

static char const ident[] = "$RCSfile: slmux.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:11 $";

#define SL_SL_MINOR	0	/* /dev/streams/sl/sl */
#define SL_ADM_MINOR	1	/* /dev/streams/sl/sl-admin */
#define SL_STA_MINOR	2	/* /dev/streams/sl/sl-stats */
#define SL_EVT_MINOR	3	/* /dev/streams/sl/sl-events */
#define SL_MON_MINOR	4	/* /dev/streams/sl/sl-mon */

struct sl_streams {
	struct sl *other;		/* other (upper or lower) stream */
	struct sl *admin;		/* admin stream */
	struct sl *stats;		/* stats stream */
	struct sl *events;		/* events stream */
	struct sl *monitor;		/* monitor stream */
};

struct sl {
	int mid;			/* module identifier */
	int sid;			/* stream identifier */
	int unit;			/* clone minor device number opened */
	queue_t *rq;			/* RD queue in queue pair */
	queue_t *wq;			/* WR queue in queue pair */
	union {
		dev_t dev;		/* device number for upper stream */
		int index;		/* multiplexer index for lower stream */
	};
	struct sl_streams sl;
	uint32_t ppa;			/* Physical Point of Appearance address. */
	char clei[SLMUX_CLEI_MAX];	/* Common language location identifier */
};

struct {
	struct sl_streams sl;
} defaults;

static caddr_t sl_opens = NULL;
static caddr_t sl_links = NULL;
static size_t sl_links_num = 0;

static rwlock_t sl_mux_lock = RW_LOCK_UNLOCKED;

#define SL_PRIV(q) ((struct sl *)(q)->q_ptr)

static void
slu_init_priv(int unit, queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
}
static void
slu_term_priv(queue_t *q)
{
}
static void
slp_init_priv(int unit, queue_t *q, int index, cred_t *crp)
{
}
static void
slp_term_priv(queue_t *q)
{
}

#define SLF_USER_LPO	(1<<0)		/* User issued local processor outage. */
#define SLF_PROV_LPO	(1<<1)		/* Provider issued local processor outage. */
#define SLF_RPO		(1<<2)		/* Remote processor outage. */
#define SLF_EMERG	(1<<3)		/* Emergency */
#define SLF_RETRIEVING	(1<<4)		/* Retrieving messages */
#define SLF_GET_BSNT	(1<<5)		/* Retrieving BSNT */
#define SLF_CLEARING	(1<<6)		/* Clearing buffers */
#define SLF_DISCARD	(1<<7)		/* Congestion discard */
#define SLF_ACCEPT	(1<<8)		/* Congestion accept */

#define SLS_POWER_OFF	0		/* Power off */
#define SLS_IDLE	1		/* Idle, failed, stopped. */
#define SLS_STARTING	2		/* Start request issued. */
#define SLS_IN_SERVICE	3		/* In service, not processor outage */
#define SLS_BLOCKED	4		/* In service but blocked */


/*
 *  SL Monitoring.
 *  ==============
 */
/**
 * sl_monitor: - monitor signalling link
 * @sl: SL private structure for active queue
 * @q: active queue (upper write queue or lower read queue)
 * @msg: the message
 * @slmon: SL private structure for upper monitoring stream
 *
 * Monitoring message primitives passed from user to provider and provider to user consists of
 * encapsulating a copy of each message and passing it to the monitoring stream.   The monitoring
 * stream has been determined before this call.  Messages are simply encapsulated and passed to the
 * monitoring stream if possible.  If not possible, an error number is returned.  This function can
 * also be called multiple times for the same message and it will only be duplicated, encapsulated
 * and delivered to the monitoring stream once (it uses a special b_flag value).
 */
static noinline fastcall int
sl_monitor(struct sl *sl, queue_t *q, mblk_t *msg, struct sl *slmon)
{
	struct slmux_mon *mon;
	mblk_t *mp;

	if ((msg->b_flag & 0x8000) == 0) {
		if (!(mp = sl_allocb(q, sizeof(*mon), BPRI_MED)))
			return (-ENOBUFS);
		mp->b_datap->db_type = DB_TYPE(msg);
		mon = (typeof(mon)) mp->b_rptr;
		mon->mon_ppa.slm_index = sl->sl.other->index;
		mon->mon_ppa.slm_ppa = sl->ppa;
		strncpy(mon->mon_ppa.slm_clei, sl->clei, SLMUX_CLEI_MAX);
		mon->mon_dir = (q->q_flag & QREADR) ? 0 : 1;
		mon->mon_msg_type = DB_TYPE(msg);
		mon->mon_msg_band = msg->b_band;
		mon->mon_msg_flags = msg->b_flag;
		mp->b_wptr += sizeof(*mon);
		if ((mp->b_cont = dupmsg(msg)) == NULL || !pullupmsg(mp, -1)) {
			freemsg(mp);
			return (-ENOBUFS);
		}
		if (!canputnext(slmon->rq)) {
			freemsg(mp);
			return (-EBUSY);
		}
		mp->b_datap->db_type = M_PROTO;
		putnext(slmon->rq, mp);
		msg->b_flag |= 0x8000;	/* mark original message as already copied */
	}
	return (0);
}

/*
 *  SL-User to SL-Provider primitives.
 *  ==================================
 */

static fastcall int
sl_pass_down(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(sl->wq, mp->b_band))
		return (-EBUSY);
	putnext(sl->wq, mp);
	return (0);
}

/**
 * lmi_info_req: - process LMI_INFO_REQ primitives
 * @sl: SL private structure (lower)
 * @q: active queue (upper write queue)
 * @msg: the message
 *
 * When we get an information request, simply return the current information from the lower device
 * structure.
 */
static fastcall int
lmi_info_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p) + SLMUX_CLEI_MAX, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		*p = sl->info;
		mp->b_wptr += sizeof(*p);
		if (sl->clei[0]) {
			strncpy(p->lmi_ppa_addr, sl->clei, SLMUX_CLEI_MAX);
			mp->b_wptr += SLMUX_CLEI_MAX;
		} else if (sl->ppa) {
			*(uint32_t *) p->lmi_ppa_addr = sl->ppa;
			mp->b_wptr += sizeof(uint32_t);
		}
		qreply(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_attach_req: - process LMI_ATTACH_REQ primitives
 * @sl: SL private structure (upper)
 * @q: active queue (upper write queue)
 * @mp: the message
 *
 * The LMI_ATTACH_REQ can attach to a lower multiplex stream using either the global PPA or the
 * CLEI.  The stream records the PPA used when it attaches and a subsequent LMI_ATTACH_REQ can be
 * issued to reattach the stream without a global PPA or CLEI, that is, with a zero length PPA.
 * The SL-MUX differentiates between a global PPA and an CLEI by the length of the PPA.  If the
 * length of the PPA is zero (0), it is interpreted as being a reattach request and the previous PPA
 * (if any) will be used; when four (4), it is interpreted as being an integer value global-PPA; and
 * any other length, it is interpreted as being an ASCII string CLEI.
 */
static fastcall int
lmi_attach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct sl *sl2, **slp, **slp2, **slpd;
	caddr_t ppa_ptr;
	size_t ppa_len;
	uint32_t ppa;
	int err, ecode;

	ppa_ptr = (caddr_t) p->lmi_ppa;
	ppa_len = mp->b_wptr - mp->b_rptr - sizeof(*p);

	if (0 > sl->unit || sl->unit > SL_MON_MINOR)
		goto notsupp;

	slp = &sl->sl.other + sl->unit;
	slpd = &defaults.sl.other + sl->unit;

	if (*slp != NULL)
		goto outstate;

	write_lock(&sl_links);

	switch (ppa_len) {
	case 0:
		if (sl->unit != SL_SL_MINOR) {
			for (sl2 = (struct sl *) mi_first_ptr(&sl_links); sl2;
			     sl2 = (struct sl *) mi_next_ptr((caddr_t) sl2)) {
				slp2 = &sl2->sl.other + sl->unit;
				if (*slp2 == NULL) {
					*slp2 = sl;
					*slp = sl2;
				}
			}
			*slpd = sl;
			write_unlock(&sl_links);
			if ((err = lmi_ok_ack(sl, q, mp, LMI_ATTACH_REQ)) != 0) {
				write_lock(&sl_links);
				for (sl2 = (struct sl *) mi_first_ptr(&sl_links); sl2;
				     sl2 = (struct sl *) mi_next_ptr((caddr_t) sl2)) {
					slp2 = &sl2->sl.other + sl->unit;
					if (*slp2 == sl)
						*slp2 = NULL;
				}
				*slp = NULL;
				*slpd = NULL;
				write_unlock(&sl_links);
			}
			return (err);
		}

		/* This is a reattach request.  The stored PPA (if any) should be used. */
		for (sl2 = (struct sl *) mi_first_ptr(&sl_links); sl2;
		     sl2 = (struct sl *) mi_next_ptr((caddr_t) sl2)) {
			if (sl2->ppa && sl2->ppa == sl->ppa)
				break;
			if (sl2->clei[0] && strncmp(sl2->clei, sl->clei, SLMUX_CLEI_MAX) == 0)
				break;
		}
		break;
	case 4:
		/* This is a global-PPA attach request. */
		ppa = *(uint32_t *) p->lmi_ppa;
		for (sl2 = (struct sl *) mi_first_ptr(&sl_links); sl2;
		     sl2 = (struct sl *) mi_next_ptr((caddr_t) sl2)) {
			if (sl2->ppa && sl2->ppa == ppa)
				break;
		}
		break;
	default:
		/* This is an ASCII string CLEI attach request. */
		for (sl2 = (struct sl *) mi_first_ptr(&sl_links); sl2;
		     sl2 = (struct sl *) mi_next_ptr((caddr_t) sl2)) {
			if (sl2->clei[0] && strncmp(sl2->clei, ppa_ptr, SLMUX_CLEI_MAX) == 0)
				break;
		}
		break;
	}
	if (sl2 == NULL) {
		write_unlock(&sl_links);
		goto badppa;
	}

	slp2 = &sl2->sl.other + sl->unit;

	if (*slp2 != NULL) {
		write_unlock(&sl_links);
		goto busy;
	}
	*slp2 = sl;
	*slp = sl2;
	write_unlock(&sl_links);
	if ((err = lmi_ok_ack(sl, q, mp, LMI_ATTACH_REQ)) != 0) {
		write_lock(&sl_links);
		*slp2 = NULL;
		*slp = NULL;
		write_unlock(&sl_links);
	}
	return (err);

      busy:
	ecode = LMI_BUSY;
	goto error;
      badppa:
	ecode = LMI_BADPPA;
	goto error;
      outstate:
	ecode = LMI_OUTSTATE;
	goto error;
      notsupp:
	ecode = LMI_NOTSUPP;
	goto error;
      error:
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, ecode);
}

/**
 * lmi_detach_req: - process LMI_DETACH_REQ primitives
 * @sl: SL private structure (lower)
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
lmi_detach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * lmi_enable_req: - process LMI_ENABLE_REQ primitives
 * @sl: SL private structure (lower)
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
lmi_enable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_i_state(sl, LMI_ENABLE_PENDING);
	return sl_pass_down(sl, q, mp);
}

/**
 * lmi_disable_req: - process LMI_DISABLE_REQ primitives
 * @sl: SL private structure (lower)
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
lmi_disable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_i_state(sl, LMI_DISABLE_PENDING);
	return sl_pass_down(sl, q, mp);
}

/**
 * lmi_optmgmt_req: - process LMI_OPTMGMT_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
lmi_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_pdu_req: - process SL_PDU_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_pdu_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_emergency_req: - process SL_EMERGENCY_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_emergency_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_LOC_EMERG);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_emergency_ceases: - process SL_EMERGENCY_CEASES primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_emergency_ceases(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, SLF_LOC_EMERG);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_start_req: - process SL_START_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_start_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_l_state(sl, SLS_STARTING);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_stop_req: - process SL_STOP_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_stop_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_l_state(sl, SLS_IDLE);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_retrieve_bsnt_req: - process SL_RETRIEVE_BSNT_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_retrieve_bsnt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_GET_BSNT);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_retreival_request_and_fsnc_req: - process SL_RETRIEVAL_REQUEST_AND_FNSC_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_retrieval_request_and_fsnc_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_RETRIEVING);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_clear_buffers_req: - process SL_CLEAR_BUFFERS_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_clear_buffers_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_CLEARING);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_clear_rtb_req: - process SL_CLEAR_RTB_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_clear_rtb_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_CLEARING);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_continue_req: - process SL_CONTINUE_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_continue_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl, SLS_BLOCKED))
		sl_set_l_state(sl, SLS_IN_SERVICE);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_local_processor_outage_req: - process SL_LOCAL_PROCESSOR_OUTAGE_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_local_processor_outage_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_USER_LPO);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_resume_req: - process SL_RESUME_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_resume_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, SLF_USER_LPO);
	if (!(sl_get_flags(sl) & (SLF_USER_LPO|SLF_PROV_LPO|SLF_RPO)))
		if (sl_get_l_state(sl) == SLS_BLOCKED)
			sl_set_l_state(sl, SL_IN_SERVICE);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_congestion_discard_req: - process SL_CONGESTION_DISCARD_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_congestion_discard_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_DISCARD);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_congestion_accept_req: - process SL_CONGESTION_ACCEPT_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_congestion_accept_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_ACCEPT);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_no_congestion_req: - process SL_NO_CONGESTION_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_no_congestion_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, (SLF_DISCARD|SLF_ACCEPT));
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_power_on_req: - process SL_POWER_ON_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_power_on_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) == SLS_POWER_OFF)
		sl_set_l_state(sl, SLS_IDLE);
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_optmgmt_req: - process SL_OPTMGMT_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_pass_down(sl, q, mp);
}

/**
 * sl_notify_req: - process SL_NOTIFY_REQ primitives
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_notify_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_pass_down(sl, q, mp);
}

/*
 *  SL-Provider to SL-User primitives.
 *  ==================================
 */

static fastcall int
sl_pass_up(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl->sl.other) {
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(sl->sl.other->rq, mp->b_band))
			return (-EBUSY);
		putnext(sl->sl.other->rq, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * lmi_info_ack: - process LMI_INFO_ACK primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * When an LMI_INFO_ACK is received it is because the SL-MUX driver is initializing the information
 * from the lower stream.  LMI_INFO_ACK is always returned by the upper stream directly.
 */
static fastcall int
lmi_info_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	sl->info.lmi_primitive = LMI_INFO_ACK;
	sl->info.lmi_version = p->lmi_version;
	sl->info.lmi_state = p->lmi_state;
	sl->info.lmi_max_sdu = p->lmi_max_sdu;
	sl->info.lmi_min_sdu = p->lmi_min_sdu;
	sl->info.lmi_header_len = p->lmi_header_len;
	sl->info.lmi_ppa_style = LMI_STYLE2;

	freemsg(mp);
	return (0);
}

/**
 * lmi_ok_ack: - process LMI_OK_ACK primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * We use this primitive simply to track state.
 */
static fastcall int
lmi_ok_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	sl_set_i_state(sl, p->lmi_state);
	return sl_pass_up(sl, q, mp);
}

/**
 * lmi_error_ack: - process LMI_ERROR_ACK primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * We use this primitive simply to track state.
 */
static fastcall int
lmi_error_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;

	sl_set_i_state(sl, p->lmi_state);
	return sl_pass_up(sl, q, mp);
}

/**
 * lmi_enable_con: - process LMI_ENABLE_CON primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * We use this primitive simply to track state.
 */
static fastcall int
lmi_enable_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;

	sl_set_i_state(sl, p->lmi_state);
	return sl_pass_up(sl, q, mp);
}

/**
 * lmi_disable_con: - process LMI_DISABLE_CON primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * We use this primitive simply to track state.
 */
static fastcall int
lmi_disable_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;

	sl_set_i_state(sl, p->lmi_state);
	return sl_pass_up(sl, q, mp);
}

/**
 * lmi_optmgmt_ack: - process LMI_OPGMGMT_ACK primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
lmi_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_pass_up(sl, q, mp);
}

/**
 * lmi_error_ind: - process LMI_ERROR_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
lmi_error_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;

	sl_set_i_state(sl, p->lmi_state);
	return sl_pass_up(sl, q, mp);
}

/**
 * lmi_stats_ind: - process LMI_STATS_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
lmi_stats_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl->sl.stats)
		return sl_monitor(sl, q, mp, sl->sl.stats);
	return sl_pass_up(sl, q, mp);
}

/**
 * lmi_event_ind: - process LMI_EVENT_INND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
lmi_event_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl->sl.events)
		return sl_monitor(sl, q, mp, sl->sl.events);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_pdu_ind: - process SL_PDU_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_pdu_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl->sl.other)
		return sl_pass_up(sl, q, mp);
	return (-EAGAIN);
}

/**
 * sl_link_congested_ind: - process SL_LINK_CONGESTED_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_link_congested_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_link_congestion_ceased_ind: - process SL_LINK_CONGESTION_CEASED_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_link_congestion_ceased_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_retreived_message_ind: - process SL_RETRIEVED_MESSAGE_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_RETRIEVING);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_retrieval_complete_ind: - process SL_RETRIEVAL_COMPLETE_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_retrieval_complete_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, SLF_RETRIEVING);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_rb_cleared_ind: - process SL_RB_CLEARED_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_rb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, SLF_CLEARING);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_bsnt_ind: - process SL_BSNT_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_bsnt_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, SLF_GET_BSNT);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_in_service_ind: - process SL_IN_SERVICE_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_in_service_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, (SLF_PROV_LPO|SLF_RPO));
	sl_set_l_state(sl, SLS_IN_SERVICE);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_out_of_service_ind: - process SL_OUT_OF_SERVICE_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_out_of_service_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_l_state(sl, SLS_IDLE);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_remote_processor_outage_ind: - process SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_remote_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_RPO);
	sl_set_l_state(sl, SLS_BLOCKED);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_remote_processor_recovered_ind: - process SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_remote_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_flags(sl) & (SLF_USER_LPO|SLF_PROV_LPO|SLF_RPO))
		sl_set_l_state(sl, SLS_BLOCKED);
	sl_clr_flags(sl, SLF_PRO);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_rtb_cleared_ind: - process SL_RTB_CLEARED_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_rtb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, SLF_CLEARING);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_retrieval_not_possible_ind: - process SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_retrieval_not_possible_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, SLF_RETRIEVING);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_bsnt_not_retrievable_ind: - process SL_BSNT_NOT_RETRIEVABLE_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, SLF_GET_BSNT);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_optmgmt_ack: - process SL_OPTMGMT_ACK primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_notify_ind: - process SL_NOTIFY_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_notify_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl->sl.events)
		return sl_monitor(sl, q, mp, sl->sl.events);
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_local_processor_outage_ind: - process SL_LOCAL_PROCESSOR_OUTAGE_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_local_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_set_flags(sl, SLF_PROV_LPO);
	switch (sl_get_l_state(sl)) {
	case SLS_IN_SERVICE:
	case SLS_STARTING:
		sl_set_l_state(sl, SLS_BLOCKED);
		break;
	}
	return sl_pass_up(sl, q, mp);
}

/**
 * sl_local_processor_recovered_ind: - process SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_local_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clr_flags(sl, SLF_USER_LPO);
	return sl_pass_up(sl, q, mp);
}

/*
 *  SL-MUX Input-Output Controls.
 *  =============================
 */

/**
 * sl_i_link: - perform an I_LINK operation
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * I_LINK operations can be permitted so long as the linked stream remains private to the linking
 * stream.  This means, that when an upper Style II SL stream links a lower SL stream, that the SL
 * Stream is immediately attached to the lower SL stream and essentially becomes a Style I stream
 * from that point on.
 *
 */
static fastcall int
sl_i_link(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct sl *slp, *slu = SL_PRIV(q);

	if ((slp = (struct sl *) mi_copy_alloc(sizeof(*slp))) == NULL) {
		mi_copy_done(q, mp, ENOMEM);
		return (0);
	}

	sl_lock(slu);
	if (slu->sl.other != NULL) {
		sl_unlock(slu);
		mi_close_free((caddr_t) slp);
		mi_copy_done(q, mp, EALREADY);
		return (0);
	}

	slp_init_priv(0, l->l_qtop, l->l_index, ioc->ioc_cr);	/* XXX: unit number? */
	mi_attach(l->l_qtop, (caddr_t) slp);

	slp->sl.other = slu;
	slu->sl.other = slp;
	sl_unlock(slu);

	mi_copy_done(q, mp, 0);
	return (0);
}

/**
 * sl_i_unlink: - perform an I_UNLINK operation
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * I_UNLINK operations can occur because the upper Stream is being closed, or an I_UNLINK operation
 * was issued on the upper stream.  In either case, the upper Stream is sufficiently locked so as to
 * avoid problems in locking.  One problem could occur if one process/thread is unlinking the stream
 * while another is attempting to use it.  To avoid problems with this, we lock the control stream.
 *
 * Note that STREAMS ensures that I_UNLINK only arrives here for Streams that were I_LINKED.
 */
static fastcall int
sl_i_unlink(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct sl *slp, *slu = SL_PRIV(q);

	sl_lock(slu);
	if ((slp = slu->sl.other) == NULL) {
		sl_unlock(slu);
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
	if (slp->wq != l->l_qtop || slp->index != l->l_index) {
		sl_unlock(slu);
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
	slu->sl.other = NULL;
	sl_unlock(slu);

	slp_term_priv(l->l_qtop);
	mi_detach(l->l_qtop, (caddr_t) slp);
	mi_close_free((caddr_t) slp);
	mi_copy_done(q, mp, 0);
	return (0);
}

/**
 * sl_i_plink: - perform an I_PLINK operation
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * Credentials of the linking process are saved.  Note that the credentials can be used for three
 * purposes: (1) to check that an attaching process has sufficient privilege to attach to the linked
 * stream; (2) to check that an unlinking process has sufficient privilege to unlink the linked
 * stream; (3) to check that a configuring process has sufficient privilege to configure the linked
 * stream.  Also, any other detached action taken on a lower stream by an upper stream can be
 * subjected to a credentials check.
 */
static fastcall int
sl_i_plink(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (struct iocblk *) mp->b_rptr;
	struct linkblk *l = mp->b_cont ? (struct linkblk *) mp->b_cont->b - rptr : NULL;
	struct sl *sl;
	dev_t dev;
	int err;

	if (l == NULL) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}

	if ((sl = (struct sl *) mi_open_alloc(sizeof(*sl))) == NULL) {
		mi_copy_done(q, mp, ENOMEM);
		return (0);
	}

	dev = l->l_index;

	mi_attach(l->l_qtop, (caddr_t) sl);

	slp_init_priv(0, l->l_qtop, l->l_index, ioc->ioc_cr);	/* XXX: unit number? */

	write_lock(&sl_mux_lock);
	if ((err = mi_open_link(&sl_links, (caddr_t) sl, &dev, 0, MODOPEN, ioc->ioc_cr))) {
		write_unlock(&sl_mux_lock);
		mi_detach(l->l_qtop, (caddr_t) sl);
		mi_copy_done(q, mp, err);
		return (0);
	}
	sl_links_num++;

	write_unlock(&sl_mux_lock);
	mi_copy_done(q, mp, 0);
	return (0);
}

/**
 * sl_i_punlink: - perform an I_PUNLINK operation
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * Note that STREAMS ensures that I_PUNLINK only arrives here for Streams that were I_PLINKED.
 *
 * To handle difficulties with locking, consider the following characteristic of STREAMS: if the
 * I_UNLINK/I_PUNLINK operation is unsucessful if can be refused, provided that the unlinking
 * opertion is not a result of tearing down the multiplexer.  The multiplexer only gets torn down
 * automatically in one situation: the link was a temporary link and the control stream is being
 * closed.  All other unlinking operations are explicit and the driver can return failure.
 *
 * Therefore, if we only allow permanent links, we can avoid the issue altogether.  Explicit
 * unlinking can then always be refused on the basis that a signalling link is attached to the lower
 * stream.  Then if the a signalling link is in the attached state, we can ensure that the attached
 * lower stream will not be going away until the signalling link is detached.
 */
static fastcall int
sl_i_punlink(queue_t *q, mblk_t *mp)
{
	struct linkblk *l = mp->b_cont ? (struct linkblk *) mp->b_cont->b_rptr : NULL;

	if (l == NULL) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}

	/* TODO: we should probably check the credentials of the unlinking process against the
	   credentials of the linking process and refuse EPERM if the unlinking process does not
	   have sufficient privilege. */

	write_lock(&sl_mux_lock);

	if (SL_PRIV(l->l_qtop)->sl.other) {
		write_unlock(&sl_mux_lock);
		mi_copy_done(q, mp, EBUSY);
		return (0);
	}

	slp_term_priv(l->l_qtop);
	mi_close_comm(&sl_links, l->l_qtop);
	sl_links_num--;
	write_unlock(&sl_mux_lock);
	mi_copy_done(q, mp, 0);
	return (0);
}

/**
 * slmux_iocsppa: - set PPA input-output control operation
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * This input-output control is used to set the other two of multiplexer identifier, global PPA or
 * CLEI, given one of the three.  The input-output control takes a slmux_ppa structure pointer as an
 * argument.  The driver finds the first lower multiplex stream that meets the specification and
 * copies the assigned multiplex identifier, global PPA and CLEI to the stream.
 *
 * This input-output control is useful for assigning configuration information to a specific
 * signalling link.
 */
static noinline fastcall int
slmux_iocsppa(struct sl *sl, queue_t *q, mblk_t *mp)
{
	mblk_t *dp;

	switch (DB_TYPE(mp)) {
	case M_IOCTL:
		mi_copyin(q, mp, NULL, sizeof(struct slmux_ppa));
		break;
	case M_IOCDATA:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		case MI_COPY_CASE(MI_COPY_IN, 1):
		{
			struct slmux_ppa *pp;
			struct sl *sl;

			pp = (typeof(pp)) dp->b_rtpr;
			write_lock(&sl_mux_lock);
			for (sl = (struct sl *) mi_first_ptr(&sl_links); sl;
			     sl = (struct sl *) mi_next_ptr((caddr_t) sl)) {
				if (pp->slm_index && pp->slm_index == sl->index)
					break;
				if (pp->slm_ppa && pp->slm_ppa == sl->ppa)
					break;
				if (pp->slm_clei[0]
				    && strncmp(pp->slm_clei, sl->clei, SLMUX_CLEI_MAX) == 0)
					break;
			}
			if (sl == NULL) {
				write_unlock(&sl_mux_lock);
				mi_copy_done(q, mp, ESRCH);
				break;
			}
			if (pp->slm_index && pp->slm_inndex != sl->index) {
				write_unlock(&sl_mux_lock);
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			sl->ppa = pp->slm_ppa;
			strncpy(sl->clei, pp->slm_clei, SLMUX_CLEI_MAX);
			write_unlock(&sl_mux_lock);
			mi_copy_done(q, mp, 0);
			break;
		}
		}
		break;
	default:
		freemsg(mp);
		break;
	}
	return (0);
}

/**
 * slmux_iocgppa: - get PPA input-output control operation
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * This input-output control is used to obtain the other two of multiplexer identifier, global PPA
 * or CLEI, given one of the three.  The input-output control takes a slmux_ppa structure pointer as
 * an argument and populates one (or more) of the three fields.  The driver finds the first lower
 * multiplex stream that meets the specification and copies the assigned multiplex identifier,
 * gloabl PPA and CLEI back to the original user slmux_ppa structure.
 *
 * This input-output control is useful for discovering configuration information about a specific
 * signalling link.
 */
static noinline fastcall int
slmux_iocgppa(struct sl *sl, queue_t *q, mblk_t *mp)
{
	mblk_t *dp, *bp;

	switch (DB_TYPE(mp)) {
	case M_IOCTL:
		mi_copyin(q, mp, NULL, sizeof(struct slmux_ppa));
		break;
	case M_IOCDATA:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		case MI_COPY_CASE(MI_COPY_IN, 1):
		{
			struct slmux_ppa *pp;
			struct sl *sl;

			if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct slmux_ppa)))) {
				mi_copy_done(q, mp, ENOBUFS);
				break;
			}
			bcopy(dp->b_rptr, bp->b_rptr, sizeof(struct slmux_ppa));
			pp = (typeof(pp)) bp->b_rptr;
			read_lock(&sl_mux_lock);
			for (sl = (struct sl *) mi_first_ptr(&sl_links); sl;
			     sl = (struct sl *) mi_next_ptr((caddr_t) sl)) {
				if (pp->slm_index && pp->slm_index == sl->index)
					break;
				if (pp->slm_ppa && pp->slm_ppa == sl->ppa)
					break;
				if (pp->slm_clei[0]
				    && strncmp(pp->slm_clei, sl->clei, SLMUX_CLEI_MAX) == 0)
					break;
			}
			if (sl == NULL) {
				read_unlock(&sl_mux_lock);
				mi_copy_done(q, mp, ESRCH);
				break;
			}
			pp->slm_index = sl->index;
			pp->slm_ppa = sl->ppa;
			strncpy(pp->slm_clei, sl->clei, SLMUX_CLEI_MAX);
			read_unlock(&sl_mux_lock);
			mi_copyout(q, mp);
			break;
		}
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	default:
		freemsg(mp);
		break;
	}
	return (0);
}

/**
 * slmux_ioclppa: - list PPA(s) input-output control operation
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * This input-output control is used to list a number of multiplexer identifiers, global PPAs and
 * CLEIs assocaited with lower multiplex signalling links.  The return value on success is always
 * the number of lower signalling links that were available at the time that the input-output
 * control was called.  (This does not mean that the number will not change between successive
 * input-output control calls.)  (If the caller is the only process performing I_PLINK/I_PUNLINK
 * operations, it can be guaranteed that the returned number of lower signalling links is the same
 * from call to call.)
 *
 * When the number of elements in the slm_list_array (slm_list_num) is a positive (non-zero) number,
 * up to slm_list_num (or fewer) elements will be populated with the multiplex identifiers, global
 * PPAs, and CLEIs of lower multiplex streams.
 */
static noinline fastcall int
slmux_ioclppa(struct sl *sl, queue_t *q, mblk_t *mp)
{
	mblk_t *dp, *bp;

	switch (DB_TYPE(mp)) {
	case M_IOCTL:
		mi_copyin(q, mp, NULL, sizeof(struct slmux_ppa_list));
		break;
	case M_IOCDATA:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		case MI_COPY_CASE(MI_COPY_IN, 1):
		{
			struct slmux_ppa_list *lp;
			struct slmux_ppa *pp;
			struct sl *sl;
			int numlinks;

			read_lock(&sl_mux_lock);
			lp = (typeof(lp)) dp->b_rptr;
			numlinks = min(sl_opens_num, lp->slm_list_num);
			if (!(bp = mi_copyout_alloc(q, mp, NULL,
						    sizeof(struct slmux_ppa_list) +
						    numlinks * sizeof(struct slmux_ppa)))) {
				read_unlock(&sl_mux_lock);
				mi_copy_done(q, mp, ENOBUFS);
				break;
			}
			mi_copy_set_rval(mp, numlinks);
			bcopy(dp->b_rptr, bp->b_rptr, sizeof(struct slmux_ppa_list));
			lp = (typeof(lp)) bp->b_rptr;
			lp->slm_list_num = numlinks;
			for (sl = (struct sl *) mi_first_ptr(&sl_links), pp = lp->slm_list_array;
			     sl && numlinks > 0;
			     sl = (struct sl *) mi_next_ptr((caddr_t) sl), numlinks--, pp++) {
				pp->slm_index = sl->index;
				pp->slm_ppa = sl->ppa;
				strncpy(pp->slm_clei, sl->clei, SLMUX_CLEI_MAX);
			}
			read_unlock(&sl_mux_lock);
			mi_copyout(q, mp);
			break;
		}
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			/* return value will be what it was set to above */
			mi_copy_done(q, mp, 0);
			break;
		}
	default:
		freemsg(mp);
		break;
	}
	return (0);
}

/**
 * sl_w_other: - process other message on upper write queue
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_w_other(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q), *slp = sl->sl.other;

	if (slp) {
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(slp->wq, mp->b_band))
			return (-EBUSY);
		putnext(slp->wq, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_w_data: - process M_DATA message on upper write queue
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_w_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q), *slp = sl->sl.other;

	if (slp) {
		if (!bcanputnext(slp->wq, mp->b_band))
			return (-EBUSY);
		putnext(slp->wq, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_w_proto: - process M_PROTO/M_PCPROTO message on upper write queue
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_w_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q), *slp = sl->sl.other;
	uint32_t prim;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t))
		goto tooshort;

	prim = *(uint32_t *)mp->b_rptr;

	if (!(slp = sl->sl.other)) {
		lmi_error_ack_t *p;
		mblk_t *rp;

		if (prim == LMI_ATTACH_REQ)
			return lmi_attach_req(sl, q, mp);
		if ((rp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
			DB_TYPE(rp) = M_PCPROTO;
			p = (typeof(p)) rp->b_wptr;
			p->lmi_primitive = LMI_ERROR_ACK;
			p->lmi_errno = 0;
			p->lmi_reason = sl->unit ? LMI_NOTSUPP : LMI_OUTSTATE;
			p->lmi_error_primitive = prim;
			p->lmi_state = LMI_UNATTACHED;
			rp->b_wptr += sizeof(*p);
			freemsg(mp);
			qreply(q, rp);
			return (0);
		}
		return (-ENOBUFS);
	}

	/* this is the entire monitoring hook */
	if (slp->sl.monitor && (err = sl_monitor(sl, q, mp, slp->sl.monitor)) != 0)
		return (err);

	if (!sl_trylock(slp, q))
		return (-EDEADLK);

	switch (prim) {
	case LMI_INFO_REQ:
		err = lmi_info_req(slp, q, mp);
		break;
	case LMI_ATTACH_REQ:
		err = lmi_attach_req(slp, q, mp);
		break;
	case LMI_DETACH_REQ:
		err = lmi_detach_req(slp, q, mp);
		break;
	case LMI_ENABLE_REQ:
		err = lmi_enable_req(slp, q, mp);
		break;
	case LMI_DISABLE_REQ:
		err = lmi_disable_req(slp, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		err = lmi_optmgmt_req(slp, q, mp);
		break;
	case SL_PDU_REQ:
		err = sl_pdu_req(slp, q, mp);
		break;
	case SL_EMERGENCY_REQ:
		err = sl_emergency_req(slp, q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		err = sl_emergency_ceases(slp, q, mp);
		break;
	case SL_START_REQ:
		err = sl_start_req(slp, q, mp);
		break;
	case SL_STOP_REQ:
		err = sl_stop_req(slp, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		err = sl_retrieve_bsnt_req(slp, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		err = sl_retrieval_request_and_fsnc_req(slp, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		err = sl_clear_buffers_req(slp, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		err = sl_clear_rtb_req(slp, q, mp);
		break;
	case SL_CONTINUE_REQ:
		err = sl_continue_req(slp, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		err = sl_local_processor_outage_req(slp, q, mp);
		break;
	case SL_RESUME_REQ:
		err = sl_resume_req(slp, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		err = sl_congestion_discard_req(slp, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		err = sl_congestion_accept_req(slp, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		err = sl_no_congestion_req(slp, q, mp);
		break;
	case SL_POWER_ON_REQ:
		err = sl_power_on_req(slp, q, mp);
		break;
	case SL_OPTMGMT_REQ:
		err = sl_optmgmt_req(slp, q, mp);
		break;
	case SL_NOTIFY_REQ:
		err = sl_notify_req(slp, q, mp);
		break;
	default:
		/* pass the rest along if possible */
		err = sl_w_other(q, mp);
	}
	if (err) {
		sl_set_i_state(slp, old_i_state);
		sl_set_l_state(slp, old_l_state);
	}
	sl_unlock(slp);
	return (err);
}

/**
 * sl_w_flush: - process M_FLUSH message on upper write queue
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_w_flush(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (sl->sl.other) {
			putnext(sl->sl.other->wq, mp);
			return (0);
		}
		mp->b_rptr[0] &= ~FLUSHW;
	}
	/* the read queue never holds anything */
	if (mp->b_rptr[0] & FLUSHR) {
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_w_ioctl: - process M_IOCTL/M_IOCDATA message on upper write queue
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct sl *sl = SL_PRIV(q);

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		if (DB_TYPE(mp) != M_IOCTL) {
			mi_copy_done(q, mp, EINVAL);
			return (0);
		}
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(I_LINK):
			return sl_i_link(sl, q, mp);
		case _IOC_NR(I_UNLINK):
			return sl_i_unlink(sl, q, mp);
		case _IOC_NR(I_PLINK):
			return sl_i_plink(sl, q, mp);
		case _IOC_NR(I_PUNLINK):
			return sl_i_punlink(sl, q, mp);
		}
		break;
	case SLMUX_IOC_MAGIC:
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(SLMUX_IOCSPPA):
			return slmux_iocsppa(sl, q, mp);
		case _IOC_NR(SLMUX_IOCGPPA):
			return slmux_iocgppa(sl, q, mp);
		case _IOC_NR(SLMUX_IOCLPPA):
			return slmux_ioclppa(sl, q, mp);
		}
		break;
	}
	{
		struct sl **slp = &sl->sl.other + sl->unit;
		struct sl **slpd = &defaults.sl.other + sl->unit;

		/* cannot issue input-output controls unless attached */
		if (*slp) {
			/* cannot issue input-output controls unless explicitly attached */
			if (*slpd && *slpd == sl) {
				mi_copy_done(q, mp, EINVAL);
				return (0);
			}
			if (DB_TYPE(mp) == M_IOCTL)
				(*slp)->ioc_id = ioc->ioc_id;
			putnext((*slp)->wq, mp);
			return (0);
		}
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
}

/**
 * sl_r_other: - process other message on lower read queue
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_r_other(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q), *slp;

	slp = &sl->sl.other;
	if (*slp && (q = (*slp)->rq)) {
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
			return (-EBUSY);
		putnext(q, mp);
		return (0);
	}
	return (-EAGAIN);
}

/**
 * sl_r_data: - process M_DATA message on lower read queue
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_r_data(queue_t *q, mblk_t *mp)
{
	return sl_r_other(q, mp);
}

/**
 * sl_r_proto: - process M_PROTO/M_PCPROTO message on lower read queue
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	uint old_i_state = sl_get_i_state(sl);
	uint old_l_state = sl_get_l_state(sl);

	/* this is the entire monitoring hook */
	if (sl->sl.monitor && (err = sl_monitor(sl, q, mp, sl->sl.monitor)) != 0)
		return (err);

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t))
		goto tooshort;

	if (!sl_trylock(sl, q))
		return (-EDEADLK);

	switch (*(uint32_t *) mp->b_rptr) {
	case LMI_INFO_ACK:
		err = lmi_info_ack(sl, q, mp);
		break;
	case LMI_OK_ACK:
		err = lmi_ok_ack(sl, q, mp);
		break;
	case LMI_ERROR_ACK:
		err = lmi_error_ack(sl, q, mp);
		break;
	case LMI_ENABLE_CON:
		err = lmi_enable_con(sl, q, mp);
		break;
	case LMI_DISABLE_CON:
		err = lmi_disable_con(sl, q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		err = lmi_optmgmt_ack(sl, q, mp);
		break;
	case LMI_ERROR_IND:
		err = lmi_error_ind(sl, q, mp);
		break;
	case LMI_STATS_IND:
		err = lmi_stats_ind(sl, q, mp);
		break;
	case LMI_EVENT_IND:
		err = lmi_event_ind(sl, q, mp);
		break;
	case SL_PDU_IND:
		err = sl_pdu_ind(sl, q, mp);
		break;
	case SL_LINK_CONGESTED_IND:
		err = sl_link_congested_ind(sl, q, mp);
		break;
	case SL_LINK_CONGESTION_CEASED_IND:
		err = sl_link_congestion_ceased_ind(sl, q, mp);
		break;
	case SL_RETRIEVED_MESSAGE_IND:
		err = sl_retrieved_message_ind(sl, q, mp);
		break;
	case SL_RETRIEVAL_COMPLETE_IND:
		err = sl_retrieval_complete_ind(sl, q, mp);
		break;
	case SL_RB_CLEARED_IND:
		err = sl_rb_cleared_ind(sl, q, mp);
		break;
	case SL_BSNT_IND:
		err = sl_bsnt_ind(sl, q, mp);
		break;
	case SL_IN_SERVICE_IND:
		err = sl_in_service_ind(sl, q, mp);
		break;
	case SL_OUT_OF_SERVICE_IND:
		err = sl_out_of_service_ind(sl, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		err = sl_remote_processor_outage_ind(sl, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		err = sl_remote_processor_recovered_ind(sl, q, mp);
		break;
	case SL_RTB_CLEARED_IND:
		err = sl_rtb_cleared_ind(sl, q, mp);
		break;
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		err = sl_retrieval_not_possible_ind(sl, q, mp);
		break;
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		err = sl_bsnt_not_retrievable_ind(sl, q, mp);
		break;
	case SL_OPTMGMT_ACK:
		err = sl_optmgmt_ack(sl, q, mp);
		break;
	case SL_NOTIFY_IND:
		err = sl_notify_ind(sl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_IND:
		err = sl_local_processor_outage_ind(sl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_RECOVERED_IND:
		err = sl_local_processor_recovered_ind(sl, q, mp);
		break;
	default:
		/* pass the rest along if possible */
		err = sl_r_other(q, mp);
		break;
	}

	if (err) {
		sl_set_i_state(sl, old_i_state);
		sl_set_l_state(sl, old_l_state);
	}

	sl_unlock(sl);
	return (err);
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * sl_r_ioctl: - process M_COPYIN/M_COPYOUT/M_IOCACK/M_IOCNAK message on lower read queue
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * When passing up a input output control there are several upper stream from which the input-output
 * control could have originated.  We search the 5 possible upper streams for the correct ioctl
 * identifier, and pass these response messages to that stream.
 */
static fastcall int
sl_r_ioctl(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q), *slp = &sl->sl.other;
	struct copyresp *cp;
	int i;

	cp = (typeof(cp)) mp->b_rptr;
	for (i = SL_SL_MINOR; i < SL_MON_MINOR; i++, slp++) {
		if (*slp && (*slp)->ioc_id && (*slp)->ioc_id == cp->cp_id) {
			switch (DB_TYPE(mp)) {
			case M_IOCACK:
			case M_IOCNAK:
				/* these complete an ioctl operation */
				(*slp)->ioc_id = 0;
				break;
			}
			putnext((*slp)->rq, mp);
			return (0);
		}
	}
	/* if M_COPYIN or M_COPYOUT respond with error */
	switch (DB_TYPE(mp)) {
	case M_COPYIN:
	case M_COPYOUT:
		cp->cp_rval = 1;
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_r_flush: - process M_FLUSH message on lower read queue
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_r_flush(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (sl->sl.other) {
			putnext(sl->sl.other->rq, mp);
			return (0);
		}
		mp->b_rptr[0] &= ~FLUSHR;
	}
	/* the write queue never holds anything */
	if (mp->b_rptr[0] & FLUSHW) {
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_w_msg: - process a STREAMS message on the upper write queue
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static fastcall int
sl_w_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_w_proto(q, mp);
	case M_FLUSH:
		return sl_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return sl_w_ioctl(q, mp);
	default:
		return sl_w_other(q, mp);
	}
}

/**
 * sl_r_msg: - process a STREAMS message on the lower read queue
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall int
sl_r_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
	case M_IOCACK:
	case M_IOCNAK:
		return sl_r_ioctl(q, mp);
	case M_FLUSH:
		return sl_r_flush(q, mp);
	default:
		return sl_r_other(q, mp);
	}
}

/**
 * slu_wput: - upper write put procedure
 * @q: active queue (upper write queue)
 * @mp: the message
 *
 * Canonical MP put procedure.
 */
static streamscall int
slu_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sl_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * slu_wsrv: - upper write service procedure
 * @q: active queue (upper write queue)
 *
 * Canonical draining service procedure.
 */
static streamscall int
slu_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sl_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/**
 * slu_rput: - upper read put procedure
 * @q: active queue (upper read queue)
 * @mp: the message
 *
 * The upper read put procedure is never used and is only provided for completeness (SVR4 says that
 * every queue must have a put procedure).  All messages are passed beyond the upper read queue with
 * putnext.
 */
static streamscall int
slu_rput(queue_t *q, mblk_t *mp)
{
	/* error */
	putnext(q, mp);
	return (0);
}

/**
 * slu_rsrv: - upper read service procedure
 * @q: active queue (upper read queue)
 *
 * The upper read service procedure is only used for backenabling and flow control across the
 * multiplexing driver.  When upper stream read side flow control subsides, this procedure is called
 * and the feeding lower read side queue(s) are enabled.
 */
static streamscall int
slu_rsrv(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);

	if (sl->sl.other && sl->sl.other->rq)
		qenable(sl->sl.other->rq);
	return (0);
}

/**
 * slp_wsrv: - lower write service procedure
 * @q: active queue (lower write queue);
 *
 * The lower write service procedure is only used for backenabling and flow control across the
 * multiplexing driver.  When lower stream write side flow control subsides, this proceudre is
 * called and feeding upper write side queue(s) are enabled.
 */
static streamscall int
slp_wsrv(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);

	if (sl->sl.other && sl->sl.other->wq)
		qenable(sl->sl.other->wq);
	return (0);
}

/**
 * slp_wput: - lower write put procedure
 * @q: active queue (lower write queue);
 * @mp: the message
 *
 * The lower write put procedure is never used and is only provided for completeness (SVR4 says that
 * every queue must have a put procedure).  All messages are passed beyond the lower write queue
 * with putnext.
 */
static streamscall int
slp_wput(queue_t *q, mblk_t *mp)
{
	/* error */
	putnext(q, mp);
	return (0);
}

/**
 * slp_rsrv: - lower read service procedure
 * @q: active queue (lower read queue);
 *
 * Canonical draining service procedure.
 */
static streamscall int
slp_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sl_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/**
 * slp_rput: - lower read put procedure
 * @q: active queue (lower read queue);
 * @mp: the message
 *
 * Canonical MP put procedure.
 */
static streamscall int
slp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sl_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * sl_qopen: - open a Stream on the SLMUX driver
 * @q: active queue pair (read queue)
 * @devp: pointer to device number
 * @oflags: open flags
 * @sflag: STREAMS flag (MODOPEN or CLONEOPEN or DRVOPEN)
 * @crp: caller's credentials
 *
 * There are several clone minor device numbers:
 *
 * 0 - /dev/sl, basic SL stream clone minor device
 * 1 - /dev/sl-admin, basic SL admin stream minor device
 * 2 - /dev/sl-stats, based SL stream statistics device
 * 3 - /dev/sl-events, based SL stream events device
 * 4 - /dev/sl-mon, based SL stream monitoring device
 */
static streamscall int
sl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	int err;

	/* already open */
	if (q->q_ptr != NULL)
		return (0);

	/* cannot be opened as module */
	if (sflag == MODOPEN || q->q_next != NULL)
		return (ENXIO);

	switch (cminor) {
	case SL_SL_MINOR:
	case SL_ADM_MINOR:
	case SL_STA_MINOR:
	case SL_EVT_MINOR:
	case SL_MON_MINOR:
		sflag = CLONEOPEN;
		break;
	default:
		return (ENXIO);
	}
	write_lock(&sl_mux_lock);
	if ((err = mi_open_comm(&sl_opens, sizeof(*sl), q, devp, oflags, sflag, crp))) {
		write_unlock(&sl_mux_lock);
		return (err);
	}
	slu_init_priv(q, devp, oflags, sflag, crp, cminor);
	write_unlock(&sl_mux_lock);
	qprocson(q);
	return (0);
}

/**
 * sl_qclose: - close a Stream on the SLMUX driver
 * @q: active queue pair (read queue)
 * @oflags: open flags
 * @crp: caller's credentials
 */
static streamscall int
sl_qclose(queue_t *q, int oflags, cred_t crp)
{
	qprocsoff(q);
	write_lock(&sl_mux_lock);
	slu_term_priv(q);
	mi_close_comm(&sl_opens, q);
	write_unlock(&sl_mux_lock);
	return (0);
}

/*
 *  STREAMS Definitions.
 */

static struct module_info slu_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat slu_mstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct module_info slp_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME "-mux",
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat slp_mstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct qinit slu_rinit = {
	.qi_putp = slu_rput,
	.qi_srvp = slu_rsrv,
	.qi_qopen = sl_qopen,
	.qi_qclose = sl_qclose,
	.qi_minfo = &slu_minfo,
	.qi_mstat = &slu_mstat,
};
static struct qinit slu_winit = {
	.qi_putp = slu_wput,
	.qi_srvp = slu_wsrv,
	.qi_minfo = &slu_minfo,
	.qi_mstat = &slu_mstat,
};
static struct qinit slp_rinit = {
	.qi_putp = slp_rput,
	.qi_srvp = slp_rsrv,
	.qi_minfo = &slp_minfo,
	.qi_mstat = &slp_mstat,
};
static struct qinit slp_winit = {
	.qi_putp = slp_wput,
	.qi_srvp = slp_wsrv,
	.qi_minfo = &slp_minfo,
	.qi_mstat = &slp_mstat,
};

static struct streamtab slmuxinfo = {
	.st_rdinit = &slu_rinit,
	.st_wrinit = &slu_winit,
	.st_muxrinit = &slp_rinit,
	.st_muxwinit = &slp_winit,
};

static struct cdevsw sl_cdev = {
	.d_name = DRV_NAME,
	.d_str = &slmuxinfo,
	.d_flag = D_MP,
	.d_kmod = THIS_MODULE,
};

static struct devnode sl_node = {
	.n_name = "sl",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode sl_adm_node = {
	.n_name = "sl-admin",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode sl_sta_node = {
	.n_name = "sl-stats",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode sl_evt_node = {
	.n_name = "sl-events",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode sl_mon_node = {
	.n_name = "sl-mon",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for SLMUX driver. (0 for allocation.)");

static __init int
slmuxinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_SPLASH);	/* console splash */
	if ((err = register_strdev(&sl_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register major %d, err = %d\n", DRV_NAME,
			(int) major, err);
		return (err);
	}
	if (major == 0)
		major = err;
	if ((err = register_strnod(&sl_cdev, &sl_node, SL_SL_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_SL_MINOR, err);
		unregister_strdev(&sl_cdev, major);
		return (err);
	}
	if ((err = register_strnod(&sl_cdev, &sl_adm_node, SL_ADM_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_ADM_MINOR, err);
		unregister_strnod(&sl_cdev, SL_SL_MINOR);
		unregister_strdev(&sl_cdev, major);
		return (err);
	}
	if ((err = register_strnod(&sl_cdev, &sl_sta_node, SL_STA_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_STA_MINOR, err);
		unregister_strnod(&sl_cdev, SL_ADM_MINOR);
		unregister_strnod(&sl_cdev, SL_SL_MINOR);
		unregister_strdev(&sl_cdev, major);
		return (err);
	}
	if ((err = register_strnod(&sl_cdev, &sl_evt_node, SL_EVT_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_EVT_MINOR, err);
		unregister_strnod(&sl_cdev, SL_STA_MINOR);
		unregister_strnod(&sl_cdev, SL_ADM_MINOR);
		unregister_strnod(&sl_cdev, SL_SL_MINOR);
		unregister_strdev(&sl_cdev, major);
		return (err);
	}
	if ((err = register_strnod(&sl_cdev, &sl_mon_node, SL_MON_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_MON_MINOR, err);
		unregister_strnod(&sl_cdev, SL_EVT_MINOR);
		unregister_strnod(&sl_cdev, SL_STA_MINOR);
		unregister_strnod(&sl_cdev, SL_ADM_MINOR);
		unregister_strnod(&sl_cdev, SL_SL_MINOR);
		unregister_strdev(&sl_cdev, major);
		return (err);
	}
	return (0);
}

static __exit void
slmuxexit(void)
{
	int err;

	if ((err = unregister_strnod(&sl_cdev, SL_MON_MINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			(int) SL_MON_MINOR, err);
	if ((err = unregister_strnod(&sl_cdev, SL_EVT_MINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			(int) SL_EVT_MINOR, err);
	if ((err = unregister_strnod(&sl_cdev, SL_STA_MINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			(int) SL_STA_MINOR, err);
	if ((err = unregister_strnod(&sl_cdev, SL_ADM_MINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			(int) SL_ADM_MINOR, err);
	if ((err = unregister_strnod(&sl_cdev, SL_SL_MINOR)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister minor %d, err = %d\n", DRV_NAME,
			(int) SL_SL_MINOR, err);
	if ((err = unregister_strdev(&sl_cdev, major)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister major %d, err = %d\n", DRV_NAME,
			(int) major, err);
	major = DRV_CMAJOR;
	return;
}

module_init(slmuxinit);
module_exit(slmuxexit);
