/*****************************************************************************

 @(#) $RCSfile: slpmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/03/25 19:00:15 $

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

 Last Modified $Date: 2007/03/25 19:00:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slpmod.c,v $
 Revision 0.9.2.3  2007/03/25 19:00:15  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.2  2006/12/29 12:18:14  brian
 - old rpms hate nested ifs, release updates

 Revision 0.9.2.1  2006/12/27 16:35:55  brian
 - added slpmod module and fixups for make check target

 *****************************************************************************/

#ident "@(#) $RCSfile: slpmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/03/25 19:00:15 $"

static char const ident[] = "$RCSfile: slpmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/03/25 19:00:15 $";

#ifndef HAVE_KTYPE_BOOL
#include <stdbool.h>
#endif

/*
 *  This is SLPMOD, an SL module that is pushed over a pipe end to form an internal
 *  pseudo-signalling link.  Service primitives on the SL interface at one end of the pipe are
 *  translated into Service primitives on the SL interface at the other end of the pipe.  Although
 *  the primaly purpose of such as link is testing, it also has the abilty to provide internal
 *  signalling links for building virtual SS7 networks within a host.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>

#ifndef mi_timer
#define mi_timer mi_timer_MAC
#endif
#ifndef mi_timer_alloc
#define mi_timer_alloc mi_timer_alloc_MAC
#endif

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#define SLPMOD_DESCRIP		"Signalling Link (SL) Pipe Module (SLPMOD) STREAMS MODULE."
#define SLPMOD_REVISION		"OpenSS7 $RCSfile: slpmod.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/03/25 19:00:15 $"
#define SLPMOD_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define SLPMOD_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SLPMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SLPMOD_LICENSE		"GPL"
#define SLPMOD_BANNER		SLPMOD_DESCRIP		"\n" \
				SLPMOD_REVISION		"\n" \
				SLPMOD_COPYRIGHT	"\n" \
				SLPMOD_DEVICE		"\n" \
				SLPMOD_CONTACT		"\n"
#define SLPMOD_SPLASH		SLPMOD_DESCRIP		" - " \
				SLPMOD_REVISION		"\n"

#ifdef LINUX
MODULE_AUTHOR(SLPMOD_CONTACT);
MODULE_DESCRIPTION(SLPMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(SLPMOD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SLPMOD_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-slpmod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define SLPMOD_MOD_ID		CONFIG_STREAMS_SLPMOD_MODID
#define SLPMOD_MOD_NAME		CONFIG_STREAMS_SLPMOD_NAME
#endif				/* LFS */

#define MOD_ID		SLPMOD_MOD_ID
#define MOD_NAME	SLPMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SLPMOD_BANNER
#else				/* MODULE */
#define MOD_BANNER	SLPMOD_SPLASH
#endif				/* MODULE */

#define SLLOGST		1
#define SLLOGTO		2
#define SLLOGRX		3
#define SLLOGTX		4
#define SLLOGTE		5
#define SLLOGDA		6

struct st {
	int l_state;			/* local management state */
	int i_state;			/* interface state */
	int i_flags;			/* interface flags */
};

struct lk {
	struct lk *other;		/* other link */
	struct st state, oldstate;	/* state */
	mblk_t *t1;			/* alignment timer */
	queue_t *oq;			/* output queue */
};

struct sl {
	int mid;			/* module id */
	int sid;			/* stream id */
	lock_t lock;
	int users;
	queue_t *waitq;
	struct lk rd, wr;
};

#define SL_PRIV(q)	    ((struct sl *)(q)->q_ptr)
#define SL_LOC_PRIV(sl, q)  ((q->q_flag & QREADR) ? &sl->rd : &sl->wr)
#define SL_REM_PRIV(sl, q)  ((q->q_flag & QREADR) ? &sl->wr : &sl->rd)

static const char *
sl_l_state_name(int state)
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
		return ("LMI_????");
	}
}

#define SL_POWER_OFF		0
#define SL_OUT_OF_SERVICE	1
#define SL_ALIGNMENT		2
#define SL_ALIGNED_READY	3
#define SL_ALIGNED_NOT_READY	4
#define SL_IN_SERVICE		5

static const char *
sl_i_state_name(int state)
{
	switch (state) {
	case SL_POWER_OFF:
		return ("SL_POWER_OFF");
	case SL_OUT_OF_SERVICE:
		return ("SL_OUT_OF_SERVICE");
	case SL_ALIGNMENT:
		return ("SL_ALIGNMENT");
	case SL_ALIGNED_READY:
		return ("SL_ALIGNED_READY");
	case SL_ALIGNED_NOT_READY:
		return ("SL_ALIGNED_NOT_READY");
	case SL_IN_SERVICE:
		return ("SL_IN_SERVICE");
	default:
		return ("SL_????");
	}
}

#define SL_LOC_PROC_OUT		0x1
#define SL_REM_PROC_OUT		0x2
#define SL_EMERGENCY		0x4
#define SL_CONG_DISCARD		0x8
#define SL_CONG_ACCEPT		0x10

static const char *
sl_flags_set(int mask)
{
	switch (mask & (SL_LOC_PROC_OUT | SL_REM_PROC_OUT | SL_EMERGENCY)) {
	case 0:
		return ("");
	case SL_LOC_PROC_OUT:
		return ("(LOC)");
	case SL_REM_PROC_OUT:
		return ("(REM)");
	case SL_EMERGENCY:
		return ("(EMERG)");
	case SL_LOC_PROC_OUT | SL_REM_PROC_OUT:
		return ("(LOC|REM)");
	case SL_LOC_PROC_OUT | SL_EMERGENCY:
		return ("(LOC|EMERG)");
	case SL_LOC_PROC_OUT | SL_REM_PROC_OUT | SL_EMERGENCY:
		return ("(LOC|REM|EMERG)");
	default:
		return ("(error)");
	}
}

static inline int
sl_get_l_state(struct lk *lk)
{
	return (lk->state.l_state);
}
static inline int
sl_set_l_state(struct sl *sl, struct lk *lk, int newstate)
{
	int oldstate = sl_get_l_state(lk);

	if (oldstate != newstate)
		strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "%s <- %s", sl_l_state_name(newstate),
		       sl_l_state_name(oldstate));
	return (lk->state.l_state = newstate);
}

static inline int
sl_get_i_state(struct lk *lk)
{
	return (lk->state.i_state);
}
static inline int
sl_set_i_state(struct sl *sl, struct lk *lk, int newstate)
{
	int oldstate = sl_get_i_state(lk);

	if (oldstate != newstate)
		strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "%s <- %s", sl_i_state_name(newstate),
		       sl_i_state_name(oldstate));
	return (lk->state.i_state = newstate);
}

static inline int
sl_get_flags(struct lk *lk)
{
	return (lk->state.i_flags);
}
static inline int
sl_set_flags(struct sl *sl, struct lk *lk, int mask)
{
	int oldflags = sl_get_flags(lk), newflags = oldflags |= mask;

	if (oldflags != newflags)
		strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "%s", sl_flags_set((newflags ^ oldflags) & mask));
	return (lk->state.i_flags = newflags);
}
static inline int
sl_clr_flags(struct sl *sl, struct lk *lk, int mask)
{
	int oldflags = sl_get_flags(lk), newflags = oldflags &= ~mask;

	if (oldflags != newflags)
		strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "~%s", sl_flags_set((newflags ^ oldflags) & mask));
	return (lk->state.i_flags = newflags);
}

static inline void
sl_timer_start(struct lk *lk, int timer, unsigned long value)
{
	switch (timer) {
	case 1:
		mi_timer(lk->t1, value);
		break;
	default:
		break;
	}
}
static void
sl_timer_stop(struct lk *lk, int timer)
{
	switch (timer) {
	case 1:
		mi_timer_cancel(lk->t1);
		break;
	default:
		break;
	}
}

static inline fastcall bool
sl_trylock(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	bool rtn = false;
	pl_t pl;

	pl = LOCK(&sl->lock, plbase);
	if (sl->users == 0) {
		rtn = true;
		sl->users = 1;
	} else if (!sl->waitq) {
		sl->waitq = q;
	} else if (sl->waitq != q) {
		qenable(q);
	}
	UNLOCK(&sl->lock, pl);
	return (rtn);
}
static inline fastcall void
sl_unlock(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	pl_t pl;

	pl = LOCK(&sl->lock, plbase);
	sl->users = 0;
	if (sl->waitq)
		qenable(XCHG(&sl->waitq, NULL));
	UNLOCK(&sl->lock, pl);
}

static inline mblk_t *
sl_allocb(queue_t *q, size_t size, int priority)
{
	mblk_t *mp;

	if (unlikely(!(mp = allocb(size, priority))))
		mi_bufcall(q, size, priority);
	return (mp);
}

static inline int
lmi_info_ack(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sl_get_l_state(lk);
		p->lmi_max_sdu = 4096;
		p->lmi_min_sdu = 1;
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE1;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_INFO_ACK");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_ok_ack(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl_get_l_state(lk)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = sl_set_l_state(sl, lk, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = sl_set_l_state(sl, lk, LMI_UNATTACHED);
			break;
		default:
			/* FIXME: log error */
			p->lmi_state = sl_get_l_state(lk);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_OK_ACK");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_error_ack(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong prim, sl_long err)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_error_primitive = prim;
		p->lmi_state = sl_get_l_state(lk);
		if (p->lmi_reason != LMI_OUTSTATE) {
			switch (sl_get_l_state(lk)) {
			case LMI_ATTACH_PENDING:
				p->lmi_state = sl_set_l_state(sl, lk, LMI_UNATTACHED);
				break;
			case LMI_DETACH_PENDING:
				p->lmi_state = sl_set_l_state(sl, lk, LMI_DISABLED);
				break;
			case LMI_ENABLE_PENDING:
				p->lmi_state = sl_set_l_state(sl, lk, LMI_DISABLED);
				break;
			case LMI_DISABLE_PENDING:
				p->lmi_state = sl_set_l_state(sl, lk, LMI_ENABLED);
				break;
			default:
				/* FIXME: log error */
				p->lmi_state = sl_get_l_state(lk);
				break;
			}
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_ERROR_ACK");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_enable_con(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = sl_set_l_state(sl, lk, LMI_ENABLED);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_ENABLE_CON");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_disable_con(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = sl_set_l_state(sl, lk, LMI_DISABLED);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_DISABLE_CON");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_optmgmt_ack(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen,
		sl_ulong flags)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = olen;
		p->lmi_opt_offset = olen ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_OPTMGMT_ACK");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_error_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_long err)
{
	lmi_error_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (bcanputnext(lk->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_ERROR_IND;
			p->lmi_errno = err < 0 ? -err : 0;
			p->lmi_reason = err < 0 ? LMI_SYSERR : err;
			p->lmi_state = sl_get_l_state(lk);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_ERROR_IND");
			putnext(lk->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static inline int
lmi_stats_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong itvl)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 1;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = itvl;
		p->lmi_timestamp = drv_hztomsec(jiffies);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_STATS_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_event_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong oid,
	      sl_ulong severity)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (bcanputnext(lk->oq, 1)) {
			mp->b_datap->db_type = M_PCPROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = drv_hztomsec(jiffies);
			p->lmi_severity = severity;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_EVENT_IND");
			putnext(lk->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static inline int
sl_pdu_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong pri, mblk_t *dp)
{
	sl_pdu_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(lk->oq))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_PDU_IND;
			p->sl_mp = pri;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_PDU_IND");
			putnext(lk->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static inline int
sl_link_congested_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong cong,
		      sl_ulong disc)
{
	sl_link_cong_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (bcanputnext(lk->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_LINK_CONGESTED_IND");
			putnext(lk->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static inline int
sl_link_congestion_ceased_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_link_cong_ceased_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (bcanputnext(lk->oq, 1)) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = 0;
			p->sl_disc_status = 0;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE,
			       "<- SL_LINK_CONGESTION_CEASED_IND");
			putnext(lk->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static inline int
sl_retrieved_message_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong pri,
			 mblk_t *dp)
{
	sl_retrieved_msg_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
		p->sl_mp = pri;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_RETRIEVED_MESSAGE_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_retrieval_complete_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong pri,
			  mblk_t *dp)
{
	sl_retrieval_comp_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		p->sl_mp = pri;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_RETRIEVAL_COMPLETE_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_rb_cleared_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_rb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_RB_CLEARED_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_bsnt_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong bsnt)
{
	sl_bsnt_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_IND;
		p->sl_bsnt = bsnt;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_BSNT_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_in_service_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_in_service_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_IN_SERVICE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_IN_SERVICE_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_out_of_service_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg, sl_ulong reason)
{
	sl_out_of_service_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_reason = reason;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_OUT_OF_SERVICE_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_remote_processor_outage_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_out_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_OUTAGE_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_remote_processor_recovered_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_recovered_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_RECOVERED_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_rtb_cleared_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_rtb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RTB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_RTB_CLEARED_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_retrieval_not_possible_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_retrieval_not_poss_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_RETRIEVAL_NOT_POSSIBLE_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_bsnt_not_retrievable_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_bsnt_not_retr_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_BSNT_NOT_RETRIEVABLE_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#if 0
static inline int
sl_optmgmt_ack(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_optmgmt_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OPTMGMT_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_OPTMGMT_ACK");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_notify_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_notify_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_NOTIFY_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_NOTIFY_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
static inline int
sl_local_processor_outage_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_out_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_LOCAL_PROCESSOR_OUTAGE_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
sl_local_processor_recovered_ind(struct sl *sl, struct lk *lk, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_recovered_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_LOCAL_PROCESSOR_RECOVERED_IND");
		putnext(lk->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static int
lmi_info_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	return lmi_info_ack(sl, loc, q, mp);
}
static int
lmi_attach_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	/* always style 1 */
	return lmi_error_ack(sl, loc, q, mp, LMI_ATTACH_REQ, LMI_NOTSUPP);
}
static int
lmi_detach_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	/* always style 1 */
	return lmi_error_ack(sl, loc, q, mp, LMI_DETACH_REQ, LMI_NOTSUPP);
}
static int
lmi_enable_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_DISABLED)
		goto outstate;
	sl_set_l_state(sl, loc, LMI_ENABLE_PENDING);
	return lmi_enable_con(sl, loc, q, mp);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, LMI_ENABLE_REQ, LMI_OUTSTATE);
}
static int
lmi_disable_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	sl_set_l_state(sl, loc, LMI_DISABLE_PENDING);
	return lmi_disable_con(sl, loc, q, mp);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, LMI_DISABLE_REQ, LMI_OUTSTATE);
}
static int
lmi_optmgmt_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	return lmi_error_ack(sl, loc, q, mp, LMI_OPTMGMT_REQ, LMI_NOTSUPP);
}
static int
sl_pdu_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	struct lk *rem = loc->other;

	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	if (sl_get_i_state(loc) != SL_IN_SERVICE)
		goto putback;
	if (!bcanputnext(rem->oq, mp->b_band))
		goto busy;
	*(sl_ulong *) mp->b_rptr = SL_PDU_IND;
	putnext(rem->oq, mp);
	return (0);
      busy:
	return (-EBUSY);
      putback:
	noenable(q);
	return (-EAGAIN);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_PDU_REQ, LMI_OUTSTATE);
}
static int
sl_emergency_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	sl_set_flags(sl, loc, SL_EMERGENCY);
      outstate:
	freemsg(mp);
	return (0);
}
static int
sl_emergency_ceases_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	sl_clr_flags(sl, loc, SL_EMERGENCY);
      outstate:
	freemsg(mp);
	return (0);
}
static int
sl_start_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	struct lk *rem = loc->other;
	int err;

	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	switch (sl_get_i_state(loc)) {
	case SL_POWER_OFF:
		goto outstate;
	case SL_OUT_OF_SERVICE:
		sl_set_i_state(sl, loc, SL_ALIGNMENT);
		sl_timer_start(loc, 1, 20000);
		switch (sl_get_i_state(rem)) {
		case SL_POWER_OFF:
			break;
		case SL_ALIGNMENT:
			sl_timer_stop(loc, 1);
			if (sl_get_flags(loc) & (SL_LOC_PROC_OUT | SL_REM_PROC_OUT)) {
				sl_set_i_state(sl, loc, SL_ALIGNED_NOT_READY);
				if ((err = sl_remote_processor_outage_ind(sl, rem, q, mp)))
					return (err);
			} else {
				sl_set_i_state(sl, loc, SL_ALIGNED_READY);
				if ((err = sl_in_service_ind(sl, rem, q, mp)))
					return (err);
			}
			if (sl_get_flags(rem) & (SL_LOC_PROC_OUT | SL_REM_PROC_OUT)) {
				sl_set_i_state(sl, rem, SL_ALIGNED_NOT_READY);
				if ((err = sl_remote_processor_outage_ind(sl, loc, q, mp)))
					return (err);
			} else {
				sl_set_i_state(sl, rem, SL_ALIGNED_READY);
				if ((err = sl_in_service_ind(sl, loc, q, mp)))
					return (err);
			}
			break;
		case SL_ALIGNED_READY:
		case SL_ALIGNED_NOT_READY:
		case SL_IN_SERVICE:
		default:
			goto outstate;
		}
		break;
	case SL_ALIGNMENT:
	case SL_ALIGNED_READY:
	case SL_ALIGNED_NOT_READY:
	case SL_IN_SERVICE:
		goto discard;
	default:
		goto outstate;
	}
      discard:
      outstate:
	freemsg(mp);
	return (0);
}
static int
sl_stop_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	struct lk *rem = loc->other;
	int err;

	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	switch (sl_get_i_state(loc)) {
	case SL_POWER_OFF:
		goto discard;
	case SL_OUT_OF_SERVICE:
		goto discard;
	case SL_ALIGNMENT:
	case SL_ALIGNED_READY:
	case SL_ALIGNED_NOT_READY:
	case SL_IN_SERVICE:
	default:
		sl_timer_stop(loc, 1);
		sl_set_i_state(sl, loc, SL_OUT_OF_SERVICE);
		switch (sl_get_i_state(rem)) {
		case SL_POWER_OFF:
		case SL_OUT_OF_SERVICE:
			break;
		case SL_ALIGNMENT:
		case SL_ALIGNED_READY:
		case SL_ALIGNED_NOT_READY:
		case SL_IN_SERVICE:
		default:
			sl_timer_stop(rem, 1);
			sl_set_i_state(sl, rem, SL_OUT_OF_SERVICE);
			if ((err = sl_out_of_service_ind(sl, rem, q, mp, 0)))
				return (err);
			break;
		}
		break;
	}
      discard:
      outstate:
	freemsg(mp);
	return (0);
}
static int
sl_retrieve_bsnt_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	return sl_bsnt_not_retrievable_ind(sl, loc, q, mp);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_OUTSTATE);
}
static int
sl_retrieval_request_and_fsnc_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	return sl_retrieval_not_possible_ind(sl, loc, q, mp);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_OUTSTATE);
}
static int
sl_clear_buffers_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	return sl_rb_cleared_ind(sl, loc, q, mp);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_OUTSTATE);
}
static int
sl_clear_rtb_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	return sl_rtb_cleared_ind(sl, loc, q, mp);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_CLEAR_RTB_REQ, LMI_OUTSTATE);
}
static int
sl_continue_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	struct lk *rem = loc->other;

	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	if (!(sl_get_flags(loc) & SL_LOC_PROC_OUT))
		goto discard;
	sl_clr_flags(sl, loc, SL_LOC_PROC_OUT);
	if (sl_get_i_state(loc) == SL_ALIGNED_NOT_READY) {
		sl_set_i_state(sl, loc, SL_IN_SERVICE);
		if (sl_get_i_state(rem) == SL_ALIGNED_READY) {
			sl_set_i_state(sl, rem, SL_IN_SERVICE);
			return sl_in_service_ind(sl, rem, q, mp);
		}
	}
	return sl_remote_processor_recovered_ind(sl, rem, q, mp);
      discard:
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_CONTINUE_REQ, LMI_OUTSTATE);
}
static int
sl_local_processor_outage_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	struct lk *rem = loc->other;

	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	if ((sl_get_flags(loc) & SL_LOC_PROC_OUT))
		goto discard;
	sl_set_flags(sl, loc, SL_LOC_PROC_OUT);
	sl_set_flags(sl, rem, SL_REM_PROC_OUT);
	return sl_remote_processor_outage_ind(sl, rem, q, mp);
      discard:
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_LOCAL_PROCESSOR_OUTAGE_REQ, LMI_OUTSTATE);
}
static int
sl_resume_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	struct lk *rem = loc->other;

	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	if (!(sl_get_flags(loc) & SL_LOC_PROC_OUT))
		goto discard;
	sl_clr_flags(sl, loc, SL_LOC_PROC_OUT);
	if (sl_get_i_state(loc) == SL_ALIGNED_NOT_READY) {
		sl_set_i_state(sl, loc, SL_IN_SERVICE);
		if (sl_get_i_state(rem) == SL_ALIGNED_READY) {
			sl_set_i_state(sl, rem, SL_IN_SERVICE);
			return sl_in_service_ind(sl, rem, q, mp);
		}
	}
	return sl_remote_processor_recovered_ind(sl, rem, q, mp);
      discard:
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_RESUME_REQ, LMI_OUTSTATE);
}
static int
sl_congestion_discard_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	sl_set_flags(sl, loc, SL_CONG_DISCARD);
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_OUTSTATE);
}
static int
sl_congestion_accept_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	sl_set_flags(sl, loc, SL_CONG_ACCEPT);
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_OUTSTATE);
}
static int
sl_no_congestion_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	struct lk *rem = loc->other;

	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	sl_clr_flags(sl, loc, (SL_CONG_DISCARD | SL_CONG_ACCEPT));
	canenable(rem->oq);
	qenable(rem->oq);
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_NO_CONGESTION_REQ, LMI_OUTSTATE);
}
static int
sl_power_on_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	if (sl_get_i_state(loc) != SL_POWER_OFF)
		goto outstate;
	sl_set_i_state(sl, loc, SL_OUT_OF_SERVICE);
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, loc, q, mp, SL_POWER_ON_REQ, LMI_OUTSTATE);
}
static int
sl_optmgmt_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static int
sl_notify_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}
static int
sl_other_req(struct sl *sl, struct lk *loc, queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
static int
sl_t1_timeout(struct sl *sl, struct lk *loc, queue_t *q)
{
	if (sl_get_l_state(loc) != LMI_ENABLED)
		goto outstate;
	if (sl_get_i_state(loc) != SL_ALIGNMENT)
		goto outstate;
	sl_set_i_state(sl, loc, SL_OUT_OF_SERVICE);
	return sl_out_of_service_ind(sl, loc, q, NULL, 0);
      outstate:
	return (0);
}

static int
sl_data(queue_t *q, mblk_t *mp)
{
	if (bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
static int
sl_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *loc = SL_LOC_PRIV(sl, q);
	struct lk *rem = SL_REM_PRIV(sl, q);
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(sl_ulong)) {
		freemsg(mp);
		return (0);
	}

	if (!sl_trylock(q))
		return (-EDEADLK);

	loc->oldstate = loc->state;
	rem->oldstate = loc->state;

	switch (*(sl_ulong *) mp->b_rptr) {
	case LMI_INFO_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_INFO_REQ");
		rtn = lmi_info_req(sl, loc, q, mp);
		break;
	case LMI_ATTACH_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_ATTACH_REQ");
		rtn = lmi_attach_req(sl, loc, q, mp);
		break;
	case LMI_DETACH_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_DETACH_REQ");
		rtn = lmi_detach_req(sl, loc, q, mp);
		break;
	case LMI_ENABLE_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_ENABLE_REQ");
		rtn = lmi_enable_req(sl, loc, q, mp);
		break;
	case LMI_DISABLE_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_DISABLE_REQ");
		rtn = lmi_disable_req(sl, loc, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_OPTMGMT_REQ");
		rtn = lmi_optmgmt_req(sl, loc, q, mp);
		break;
	case SL_PDU_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_PDU_REQ");
		rtn = sl_pdu_req(sl, loc, q, mp);
		break;
	case SL_EMERGENCY_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_EMERGENCY_REQ");
		rtn = sl_emergency_req(sl, loc, q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_EMERGENCY_CEASES_REQ");
		rtn = sl_emergency_ceases_req(sl, loc, q, mp);
		break;
	case SL_START_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_START_REQ");
		rtn = sl_start_req(sl, loc, q, mp);
		break;
	case SL_STOP_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_STOP_REQ");
		rtn = sl_stop_req(sl, loc, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_RETRIEVE_BSNT_REQ");
		rtn = sl_retrieve_bsnt_req(sl, loc, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_RETRIEVAL_REQUEST_AND_FSNC_REQ");
		rtn = sl_retrieval_request_and_fsnc_req(sl, loc, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CLEAR_BUFFERS_REQ");
		rtn = sl_clear_buffers_req(sl, loc, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CLEAR_RTB_REQ");
		rtn = sl_clear_rtb_req(sl, loc, q, mp);
		break;
	case SL_CONTINUE_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CONTINUE_REQ");
		rtn = sl_continue_req(sl, loc, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_LOCAL_PROCESSOR_OUTAGE_REQ");
		rtn = sl_local_processor_outage_req(sl, loc, q, mp);
		break;
	case SL_RESUME_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_RESUME_REQ");
		rtn = sl_resume_req(sl, loc, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CONGESTION_DISCARD_REQ");
		rtn = sl_congestion_discard_req(sl, loc, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CONGESTION_ACCEPT_REQ");
		rtn = sl_congestion_accept_req(sl, loc, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_NO_CONGESTION_REQ");
		rtn = sl_no_congestion_req(sl, loc, q, mp);
		break;
	case SL_POWER_ON_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_POWER_ON_REQ");
		rtn = sl_power_on_req(sl, loc, q, mp);
		break;
	case SL_OPTMGMT_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_OPTMGMT_REQ");
		rtn = sl_optmgmt_req(sl, loc, q, mp);
		break;
	case SL_NOTIFY_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_NOTIFY_REQ");
		rtn = sl_notify_req(sl, loc, q, mp);
		break;
	default:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_????_???");
		rtn = sl_other_req(sl, loc, q, mp);
		break;
	}
	if (rtn != 0) {
		loc->state = loc->oldstate;
		rem->state = rem->oldstate;
	}
	sl_unlock(q);
	return (rtn);
}
static int
sl_sig(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *loc = SL_LOC_PRIV(sl, q);
	struct lk *rem = SL_REM_PRIV(sl, q);
	int rtn;

	if (!sl_trylock(q))
		return (-EDEADLK);

	if (!mi_timer_valid(mp))
		return (0);

	loc->oldstate = loc->state;
	rem->oldstate = rem->state;

	switch (*(int *) mp->b_rptr) {
	case 1:
		strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = sl_t1_timeout(sl, loc, q);
		break;
	default:
		strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> ?? TIMEOUT <-");
		rtn = 0;
		break;
	}
	if (rtn != 0) {
		loc->state = loc->oldstate;
		rem->state = rem->oldstate;
	}
	sl_unlock(q);
	return (rtn);
}
static int
sl_ioctl(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static int
sl_iocdata(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static int
sl_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	/* pipemod style flush bit reversal */
	switch (mp->b_rptr[0] & FLUSHRW) {
	case FLUSHW:
		mp->b_rptr[0] &= ~FLUSHW;
		mp->b_rptr[0] |= FLUSHR;
		break;
	case FLUSHR:
		mp->b_rptr[0] &= ~FLUSHR;
		mp->b_rptr[0] |= FLUSHW;
		break;
	}
	putnext(q, mp);
	return (0);
}
static int
sl_other(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
sl_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sl_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return sl_sig(q, mp);
	case M_IOCTL:
		return sl_ioctl(q, mp);
	case M_IOCDATA:
		return sl_iocdata(q, mp);
	case M_FLUSH:
		return sl_flush(q, mp);
	default:
		return sl_other(q, mp);
	}
}

static inline fastcall int
sl_msg(queue_t *q, mblk_t *mp)
{
	/* fastpath for data */
	if (mp->b_datap->db_type == M_DATA) {
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
	if (mp->b_datap->db_type == M_PROTO) {
		if (mp->b_wptr >= mp->b_rptr + sizeof(sl_ulong)) {
			if (*(sl_ulong *) mp->b_rptr == SL_PDU_REQ) {
				if (bcanputnext(q, mp->b_band)) {
					*(sl_ulong *) mp->b_rptr = SL_PDU_IND;
					putnext(q, mp);
					return (0);
				}
				return (-EBUSY);
			}
		}
	}
	return sl_msg_slow(q, mp);
}
static streamscall __hot_in int
sl_put(queue_t *q, mblk_t *mp)
{
	if ((mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || sl_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_read int
sl_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sl_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static caddr_t sl_opens = NULL;

static streamscall int
sl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct sl *sl;
	static int instance = 0;
	mblk_t *loc_tp, *rem_tp;
	int err;

	if (q->q_ptr)
		return (0);
	if (!(loc_tp = mi_timer_alloc(q, sizeof(int))))
		return (ENOBUFS);
	*(int *) loc_tp->b_rptr = 1;
	if (!(rem_tp = mi_timer_alloc(WR(q), sizeof(int)))) {
		mi_timer_free(loc_tp);
		return (ENOBUFS);
	}
	*(int *) rem_tp->b_rptr = 1;
	if ((err = mi_open_comm(&sl_opens, sizeof(*sl), q, devp, oflags, sflag, crp))) {
		mi_timer_free(loc_tp);
		mi_timer_free(rem_tp);
		return (err);
	}

	sl = SL_PRIV(q);

	/* initialize sl structure */
	sl->mid = q->q_qinfo->qi_minfo->mi_idnum;
	sl->sid = ++instance;
	sl->lock = SPIN_LOCK_UNLOCKED;
	sl->users = 0;
	sl->waitq = NULL;
	sl->rd.state.l_state = LMI_DISABLED;
	sl->rd.state.i_state = SL_POWER_OFF;
	sl->rd.state.i_flags = 0;
	sl->rd.oldstate.l_state = LMI_DISABLED;
	sl->rd.oldstate.i_state = SL_POWER_OFF;
	sl->rd.oldstate.i_flags = 0;
	sl->rd.t1 = loc_tp;
	sl->rd.other = &sl->wr;
	sl->rd.oq = RD(q);
	sl->wr.state.l_state = LMI_DISABLED;
	sl->wr.state.i_state = SL_POWER_OFF;
	sl->wr.state.i_flags = 0;
	sl->wr.oldstate.l_state = LMI_DISABLED;
	sl->wr.oldstate.i_state = SL_POWER_OFF;
	sl->wr.oldstate.i_flags = 0;
	sl->wr.t1 = rem_tp;
	sl->wr.other = &sl->rd;
	sl->wr.oq = WR(q);

	qprocson(q);
	return (0);
}

static streamscall int
sl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct sl *sl = SL_PRIV(q);

	qprocsoff(q);
	mi_timer_free(XCHG(&sl->rd.t1, NULL));
	mi_timer_free(XCHG(&sl->wr.t1, NULL));
	mi_close_comm(&sl_opens, q);
	return (0);
}

static struct module_info sl_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat sl_mstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct qinit sl_rinit = {
	.qi_putp = sl_put,
	.qi_srvp = sl_srv,
	.qi_qopen = sl_qopen,
	.qi_qclose = sl_qclose,
	.qi_minfo = &sl_minfo,
	.qi_mstat = &sl_mstat,
};

static struct qinit sl_winit = {
	.qi_putp = sl_put,
	.qi_srvp = sl_srv,
	.qi_minfo = &sl_minfo,
	.qi_mstat = &sl_mstat,
};

static struct streamtab slpmodinfo = {
	.st_rdinit = &sl_rinit,
	.st_wrinit = &sl_winit,
};

static unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module id for SLPMOD module.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif

static struct fmodsw sl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &slpmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
slpmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&sl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not regsiter module %d\n", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
slpmodexit(void)
{
	int err;

	if ((err = unregister_strmod(&sl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(slpmodinit);
module_exit(slpmodexit);
