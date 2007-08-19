/*****************************************************************************

 @(#) $RCSfile: sl_mux.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2007/08/19 11:55:54 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

 Last Modified $Date: 2007/08/19 11:55:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sl_mux.c,v $
 Revision 0.9.2.26  2007/08/19 11:55:54  brian
 - move stdbool.h, obviate need for YFLAGS, general workup

 Revision 0.9.2.25  2007/08/15 05:20:18  brian
 - GPLv3 updates

 Revision 0.9.2.24  2007/08/12 15:10:05  brian
 - moved files around

 Revision 0.9.2.3  2007/08/06 04:43:57  brian
 - rework of pipe-based emulation modules

 Revision 0.9.2.2  2007/08/03 13:35:41  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.1  2007/07/21 20:22:02  brian
 - added channel modules

 *****************************************************************************/

#ident "@(#) $RCSfile: sl_mux.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2007/08/19 11:55:54 $"

static char const ident[] =
    "$RCSfile: sl_mux.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2007/08/19 11:55:54 $";

/*
 *  This is a signalling link multiplexing driver for signalling link management.  The purpose of
 *  the SL-MUX driver is to provide a common and consistent way of treating signalling links within
 *  a system.  The upper module presents several stream types:
 *
 *  1) a fixed control stream that can alter characteristics of the driver and the mapping of
 *  signalling links under the control of the multiplexing driver.  The control stream does not have
 *  a service interface but presents an input-output control management interface.
 *
 *  The control stream can be used to link SLI streams under the multiplexing driver and to assign
 *  information to linked signalling link streams.
 *
 *  2) a clone-based monitoring stream that can be used to monitor signalling links under the
 *  control of the multiplexing driver.  The monitoring stream uses a modified SLI service interface
 *  that is used for monitoring.
 *
 *  3) a clone-base user stream that can be attached to any signalling link under the control of the
 *  multiplexing driver.  The user stream uses a standard SLI service interface for interacting with
 *  the specific signalling link stream.
 */

#define _LFS_SOURCE 1
#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1
#define _SUN_SOURCE 1

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>

#include <ss7/sl_mux.h>

#define SL_MUX_DESCRIP		"SL-MUX: SS7/SL (Signalling Link) STREAMS MULTIPLEXING DRIVER."
#define SL_MUX_REVISION		"OpenSS7 $RCSfile: sl_mux.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2007/08/19 11:55:54 $"
#define SL_MUX_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corportation.  All Rights Reserved."
#define SL_MUX_DEVICE		"Supports the OpenSS7 MTP2 and INET transport drivers."
#define SL_MUX_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SL_MUX_LICENSE		"GPL"
#define SL_MUX_BANNER		SL_MUX_DESCRIP		"\n" \
				SL_MUX_REVISION		"\n" \
				SL_MUX_COPYRIGHT	"\n" \
				SL_MUX_DEVICE		"\n" \
				SL_MUX_CONTACT		"\n"
#define SL_MUX_SPLASH		SL_MUX_DEVICE		" - " \
				SL_MUX_REVISION

#ifdef LINUX
MODULE_AUTHOR(SL_MUX_CONTACT);
MODULE_DESCRIPTION(SL_MUX_DESCRIP);
MODULE_SUPPORTED_DEVICE(SL_MUX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SL_MUX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streasm-sl-mux");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define SL_MUX_DRV_ID		CONFIG_STREAMS_SL_MUX_MODID
#define SL_MUX_DRV_NAME		CONFIG_STREAMS_SL_MUX_NAME
#define SL_MUX_CMAJORS		CONFIG_STREAMS_SL_MUX_NMAJORS
#define SL_MUX_CMAJOR_0		CONFIG_STREAMS_SL_MUX_MAJOR
#define SL_MUX_UNITS		CONFIG_STREAMS_SL_MUX_NMINORS
#endif				/* LFS */

/*
 *  ===========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  ===========================================================================
 */

#define DRV_ID		SL_MUX_DRV_ID
#define DRV_NAME	SL_MUX_DRV_NAME
#define CMAJORS		SL_MUX_CMAJORS
#define CMAJOR_0	SL_MUX_CMAJOR_0
#define UNITS		SL_MUX_UNITS
#ifdef MODULE
#define DRV_BANNER	SL_MUX_BANNER
#else				/* MODULE */
#define DRV_BANNER	SL_MUX_SPLASH
#endif				/* MODULE */

static struct module_info sl_minfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = STRMINPSZ,	/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,	/* Max packet size accepted */
	.mi_hiwat = STRHIGH,	/* Hi water mark */
	.mi_lowat = STRLOW,	/* Lo water mark */
};

static struct module_stat sl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sl_muxrstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sl_muxwstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static caddr_t sl_opens = NULL;
static caddr_t sl_links = NULL;

static size_t sl_opens_count = 0;
static size_t sl_links_count = 0;

/*
 *  ===========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  ===========================================================================
 */

/**
 * sl: - signalling link structure
 * @cred: credentials of opener or linker
 * @oq: other queue (lower write queue for upper stream, upper read queue for lower stream)
 */
struct sl {
	queue_t *rq;			/* read queue of this stream */
	queue_t *wq;			/* write queue of this stream */
	queue_t *oq;			/* other queue for linkage */
	int state;			/* management state of stream */
	uint oldstate;			/* checkpoint state of stream */
	struct {
		struct sl *sl;		/* attached upper or lower structure */
	} sl;
	struct {
		struct sl *lm;		/* layer management stream for linked streams */
		struct sl *next;	/* next stream for same lm */
		struct sl **prev;	/* prev stream for same lm */
	} lm;
	struct {
		struct sl *mon;		/* monitor stream for this stream */
		struct sl *next;	/* next stream for same mon */
		struct sl **prev;	/* prev stream for same mon */
	} mon;
	struct {
		uint id;		/* ioctl id for management stream */
		struct sl *sl;		/* lower stream being controlled */
	} ioc;
	cred_t cred;			/* credentials of opener or linker */
	struct slmux_ppa ppa;		/* lower multiplex index, ppa and clei */
};

#define SL_PRIV(q) ((struct sl *)q->q_ptr)

static struct sl *sl_ctrl = NULL;	/* layer management control stream */

static kmem_cachep_t sl_priv_cachep = NULL;

/* LOGGING */

#define STRLOGNO    0		/* Log errors */
#define STRLOGIO    1		/* Log intput-output controls */
#define STRLOGST    2		/* Log state transitions */
#define STRLOGTO    3		/* Log timeouts */
#define STRLOGRX    4		/* Log received primitives */
#define STRLOGTX    5		/* Log issued primitives */
#define STRLOGTE    6		/* Log timer events */
#define STRLOGDA    7		/* Log data */

/*
 *  ===========================================================================
 *
 *  FINDING STUFF
 *
 *  ===========================================================================
 */

static noinline fastcall struct sl *
sl_find_index(struct sl *lm, int index)
{
	struct sl *sl;

	for (sl = lm->lm.lm; sl && sl->ppa.slm_index != index; sl = sl->lm.next) ;
	if (sl == NULL)
		sl = lm->sl.sl;
	return (sl);
}
static noinline fastcall struct sl *
sl_find_ppa(struct sl *lm, uint ppa)
{
	struct sl *sl;

	for (sl = lm->lm.lm; sl && sl->ppa.slm_ppa != ppa; sl = sl->lm.next) ;
	return (sl);
}
static noinline fastcall struct sl *
sl_find_clei(struct sl *lm, const char *clei)
{
	struct sl *sl;

	for (sl = lm->lm.lm; sl && strncmp(sl->ppa.slm_clei, clei, SLMUX_CLEI_MAX) != 0;
	     sl = sl->lm.next) ;
	return (sl);
}
static noinline fastcall struct sl *
sl_find_lower(struct sl *lm, struct slmux_ppa *ppa)
{
	if (ppa->slm_index != 0)
		return sl_find_index(lm, ppa->slm_index);
	if (ppa->slm_ppa != 0)
		return sl_find_ppa(lm, ppa->slm_ppa);
	if (ppa->slm_clei[0] != '\0')
		return sl_find_clei(lm, ppa->slm_clei);
	return (lm->sl.sl);
}

/*
 *  ===========================================================================
 *
 *  STATE HANDLING
 *
 *  ===========================================================================
 */
static const char *
sl_iocname(int cmd)
{
	switch (cmd) {
	case SLM_IOCSPPA:
		return ("SLM_IOCSPPA");
	case SLM_IOCGPPA:
		return ("SLM_IOCGPPA");
	case SLM_IOCLPPA:
		return ("SLM_IOCLPPA");
	case SLM_IOCSMON:
		return ("SLM_IOCSMON");
	case SLM_IOCCMON:
		return ("SLM_IOCCMON");
	case SL_IOCGOPTIONS:
		return ("SL_IOCGOPTIONS");
	case SL_IOCSOPTIONS:
		return ("SL_IOCSOPTIONS");
	case SL_IOCGCONFIG:
		return ("SL_IOCGCONFIG");
	case SL_IOCSCONFIG:
		return ("SL_IOCSCONFIG");
	case SL_IOCTCONFIG:
		return ("SL_IOCTCONFIG");
	case SL_IOCCCONFIG:
		return ("SL_IOCCCONFIG");
	case SL_IOCGSTATEM:
		return ("SL_IOCGSTATEM");
	case SL_IOCCMRESET:
		return ("SL_IOCCMRESET");
	case SL_IOCGSTATSP:
		return ("SL_IOCGSTATSP");
	case SL_IOCSSTATSP:
		return ("SL_IOCSSTATSP");
	case SL_IOCGSTATS:
		return ("SL_IOCGSTATS");
	case SL_IOCCSTATS:
		return ("SL_IOCCSTATS");
	case SL_IOCGNOTIFY:
		return ("SL_IOCGNOTIFY");
	case SL_IOCSNOTIFY:
		return ("SL_IOCSNOTIFY");
	case SL_IOCCNOTIFY:
		return ("SL_IOCCNOTIFY");
	case SL_IOCCPASS:
		return ("SL_IOCCPASS");
	default:
		return ("SL_IOC????");
	}
}

static const char *
sl_primname(sl_long prim)
{
	switch (prim) {
	case LMI_INFO_REQ:
		return ("LMI_INFO_REQ");
	case LMI_ATTACH_REQ:
		return ("LMI_ATTACH_REQ");
	case LMI_DETACH_REQ:
		return ("LMI_DETACH_REQ");
	case LMI_ENABLE_REQ:
		return ("LMI_ENABLE_REQ");
	case LMI_DISABLE_REQ:
		return ("LMI_DISABLE_REQ");
	case LMI_OPTMGMT_REQ:
		return ("LMI_OPTMGMT_REQ");
	case LMI_INFO_ACK:
		return ("LMI_INFO_ACK");
	case LMI_OK_ACK:
		return ("LMI_OK_ACK");
	case LMI_ERROR_ACK:
		return ("LMI_ERROR_ACK");
	case LMI_ENABLE_CON:
		return ("LMI_ENABLE_CON");
	case LMI_DISABLE_CON:
		return ("LMI_DISABLE_CON");
	case LMI_OPTMGMT_ACK:
		return ("LMI_OPTMGMT_ACK");
	case LMI_ERROR_IND:
		return ("LMI_ERROR_IND");
	case LMI_STATS_IND:
		return ("LMI_STATS_IND");
	case LMI_EVENT_IND:
		return ("LMI_EVENT_IND");
	case SL_PDU_REQ:
		return ("SL_PDU_REQ");
	case SL_EMERGENCY_REQ:
		return ("SL_EMERGENCY_REQ");
	case SL_EMERGENCY_CEASES_REQ:
		return ("SL_EMERGENCY_CEASES_REQ");
	case SL_START_REQ:
		return ("SL_START_REQ");
	case SL_STOP_REQ:
		return ("SL_STOP_REQ");
	case SL_RETRIEVE_BSNT_REQ:
		return ("SL_RETRIEVE_BSNT_REQ");
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		return ("SL_RETRIEVAL_REQUEST_AND_FSNC_REQ");
	case SL_CLEAR_BUFFERS_REQ:
		return ("SL_CLEAR_BUFFERS_REQ");
	case SL_CLEAR_RTB_REQ:
		return ("SL_CLEAR_RTB_REQ");
	case SL_CONTINUE_REQ:
		return ("SL_CONTINUE_REQ");
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		return ("SL_LOCAL_PROCESSOR_OUTAGE_REQ");
	case SL_RESUME_REQ:
		return ("SL_RESUME_REQ");
	case SL_CONGESTION_DISCARD_REQ:
		return ("SL_CONGESTION_DISCARD_REQ");
	case SL_CONGESTION_ACCEPT_REQ:
		return ("SL_CONGESTION_ACCEPT_REQ");
	case SL_NO_CONGESTION_REQ:
		return ("SL_NO_CONGESTION_REQ");
	case SL_POWER_ON_REQ:
		return ("SL_POWER_ON_REQ");
	case SL_OPTMGMT_REQ:
		return ("SL_OPTMGMT_REQ");
	case SL_NOTIFY_REQ:
		return ("SL_NOTIFY_REQ");
	case SL_PDU_IND:
		return ("SL_PDU_IND");
	case SL_LINK_CONGESTED_IND:
		return ("SL_LINK_CONGESTED_IND");
	case SL_LINK_CONGESTION_CEASED_IND:
		return ("SL_LINK_CONGESTION_CEASED_IND");
	case SL_RETRIEVED_MESSAGE_IND:
		return ("SL_RETRIEVED_MESSAGE_IND");
	case SL_RETRIEVAL_COMPLETE_IND:
		return ("SL_RETRIEVAL_COMPLETE_IND");
	case SL_RB_CLEARED_IND:
		return ("SL_RB_CLEARED_IND");
	case SL_BSNT_IND:
		return ("SL_BSNT_IND");
	case SL_IN_SERVICE_IND:
		return ("SL_IN_SERVICE_IND");
	case SL_OUT_OF_SERVICE_IND:
		return ("SL_OUT_OF_SERVICE_IND");
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		return ("SL_REMOTE_PROCESSOR_OUTAGE_IND");
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		return ("SL_REMOTE_PROCESSOR_RECOVERED_IND");
	case SL_RTB_CLEARED_IND:
		return ("SL_RTB_CLEARED_IND");
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		return ("SL_RETRIEVAL_NOT_POSSIBLE_IND");
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		return ("SL_BSNT_NOT_RETRIEVABLE_IND");
	case SL_OPTMGMT_ACK:
		return ("SL_OPTMGMT_ACK");
	case SL_NOTIFY_IND:
		return ("SL_NOTIFY_IND");
	case SL_LOCAL_PROCESSOR_OUTAGE_IND:
		return ("SL_LOCAL_PROCESSOR_OUTAGE_IND");
	case SL_LOCAL_PROCESSOR_RECOVERED_IND:
		return ("SL_LOCAL_PROCESSOR_RECOVERED_IND");
	default:
		return ("Unknown Primitive");
	}
}
static const char *
sl_m_statename(int state)
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
		return ("???");
	}
}
static int
sl_set_m_state(struct sl *sl, int newstate)
{
	int oldstate = sl->state;

	if (oldstate != newstate) {
		mi_strlog(sl->rq, STRLOGST, SL_TRACE, "%s <- %s", sl_m_statename(newstate),
			  sl_m_statename(oldstate));
		sl->state = newstate;
	}
	return (newstate);
}
static int
sl_get_m_state(struct sl *sl)
{
	return (sl->state);
}
static int
sl_save_m_state(struct sl *sl)
{
	return ((sl->oldstate = sl_get_m_state(sl)));
}
static int
sl_restore_m_state(struct sl *sl)
{
	return sl_set_m_state(sl, sl->oldstate);
}

/*
 *  ===========================================================================
 *
 *  LOCKING
 *
 *  ===========================================================================
 */

/**
 * sl_mux_lock: - multplexing driver lock
 *
 * This lock protects the association of upper and lower streams as well as protecting the
 * dereference of q->q_ptr for the lower queue pairs.  NOte that while this lock is head for read,
 * no upper stream can close, no lower stream can be unlinked, and the association of upper streams
 * to lower streams is frozen, also the sl_opens and sl_links lists are frozen.  This lock is held
 * for write whenever an upper stream opens and closes and affects the np_opens list, a lower stream
 * is altered during the bind and unbind operations.  This drivers is protected from interrupts by
 * its put procedures and interrupt suppression is not necessary when taking these locks (making
 * them disappear on UP).
 */
static rwlock_t sl_mux_lock = RW_LOCK_UNLOCKED;

/*
 * On SMP for the lower multiplex Streams, the put or service procedure could be invoked between the
 * point that the driver receives the I_UNLINK or I_PUNLINK and removes the private structure, and
 * the point that the Stream head is placed back against the queue pair.  We NULL q->q_ptr, for the
 * queue pair under sl_mux_lock write lock during the unlinking process and the put or service
 * procedure will encounter a NULL q->q_ptr when invoked between the unlink and Stream head
 * replacement.  This lock protects dereference of the q->q_ptr on SMP.  Note that the caller can
 * check SL_PRIV(q) after this function returns NULL to determine whether the failure was due to
 * failure to acquire the lock or whether NP_PRIV(q) was simply NULL.
 */
static inline fastcall struct sl *
sl_mux_acquire(queue_t *q)
{
	struct sl *sl;

	read_lock(&sl_mux_lock);
	sl = (struct sl *) mi_trylock(q);
	read_unlock(&sl_mux_lock);
	return (sl);
}
static inline fastcall void
sl_mux_release(struct sl *sl)
{
	mi_unlock((caddr_t) sl);
}

static inline fastcall struct sl *
sl_acquire(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);

	read_lock(&sl_mux_lock);
	if (likely(!!sl->sl.sl)) {
		/* lock lower if it exists */
		sl = (struct sl *) mi_acquire((caddr_t) sl->sl.sl, q);
	} else {
		/* lock upper if no lower */
		sl = (struct sl *) mi_trylock(q);
	}
	read_unlock(&sl_mux_lock);
	return (sl);
}

static inline fastcall void
sl_release(struct sl *sl)
{
	/* notice that the lower cannot go away until it is unlocked */
	if (likely(!!sl->sl.sl))
		return mi_unlock((caddr_t) sl->sl.sl);
	return mi_unlock((caddr_t) sl);
}

/*
 *  ===========================================================================
 *
 *  SL-MUX -> SL Provider Primitives
 *
 *  ===========================================================================
 *  These are primitives that are sent downstrem to the SL provider that sits
 *  beneath the lower multiplex.
 */

#if 0
/**
 * lmi_tx_info_req: - issue an LMI_INFO_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_info_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_info_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "LMI_INFO_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_attach_req: - issue an LMI_ATTACH_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_attach_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_attach_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ATTACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "LMI_ATTACH_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_detach_req: - issue an LMI_DETACH_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_detach_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_detach_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "LMI_DETACH_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_enable_req: - issue an LMI_ENABLE_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_enable_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_enable_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "LMI_ENABLE_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_disable_req: - issue an LMI_DISABLE_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_disable_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_disable_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "LMI_DISABLE_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_optmgmt_req: - issue an LMI_OPTMGMT_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_optmgmt_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "LMI_OPTMGMT_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_pdu_req: - issue an SL_PDU_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 * @mpri: message priority
 * @dp: user data
 */
static fastcall int
sl_tx_pdu_req(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong mpri, mblk_t *dp)
{
	sl_pdu_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_PDU_REQ;
		p->sl_mp = mpri;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_PDU_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_emergency_req: - issue an SL_EMERGENCY_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_emergency_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_emergency_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_EMERGENCY_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_EMERGENCY_REQ ->");
		sl_or_l_flags(sl, SLF_EMERGENCY);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_emergency_ceases_req: - issue an SL_CEASES_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_emergency_ceases_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_emergency_ceases_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_EMERGENCY_CEASES_REQ ->");
		sl_nand_l_flags(sl, SLF_EMERGENCY);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_start_req: - issue an SL_START_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_start_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_start_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_START_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_START_REQ ->");
#if 0
		if (sl_get_l_state(sl) == SLS_OUT_OF_SERV)
			sl_set_l_state(sl) = SLS_ALIGNMENT;
#endif
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_stop_req: - issue an SL_STOP_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_stop_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_stop_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_STOP_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_STOP_REQ ->");
#if 0
		if (sl_get_l_state(sl) != SLS_POWER_OFF)
			sl_set_l_state(sl, SLS_OUT_OF_SERV);
#endif
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_retrieve_bsnt_req: - issue an SL_BSNT_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_retrieve_bsnt_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieve_bsnt_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVE_BSNT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_RETRIEVE_BSNT_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_retrieval_request_and_fsnc_req: - issue an SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_retrieval_request_and_fsnc_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieval_req_and_fsnc_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_REQUEST_AND_FSNC_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_RETRIEVAL_REQUEST_AND_FSNC_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_clear_buffers_req: - issue an SL_CLEAR_BUFFERS_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_clear_buffers_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_clear_buffers_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_CLEAR_BUFFERS_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_clear_rtb_req: - issue an SL_CLEAR_RTB_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_clear_rtb_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_clear_rtb_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CLEAR_RTB_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_CLEAR_RTB_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_continue_req: - issue an SL_CONTINUE_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_continue_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_continue_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CONTINUE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_CONTINUE_REQ ->");
		if (sl_get_l_state(sl) == SLS_IN_SERV)
			if (sl_get_l_flags(sl) & SLF_REM_PROC_OUT)
				sl_or_l_flags(sl, SLF_LEVEL_3_IND);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_local_processor_outage_req: - issue an SL_LOCAL_PROCESSOR_OUTAGE_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_local_processor_outage_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_local_proc_outage_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_LOCAL_PROCESSOR_OUTAGE_REQ ->");
		sl_or_l_flags(sl, SLF_LOC_PROC_OUT);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_resume_req: - issue an SL_RESUME_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_resume_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_resume_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RESUME_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_RESUME_REQ ->");
		sl_nand_l_flags(sl, SLF_LOC_PROC_OUT);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_congestion_discard_req: - issue an SL_CONGESTION_DISCARD_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_congestion_discard_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_cong_discard_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_CONGESTION_DISCARD_REQ ->");
		sl_or_l_flags(sl, SLF_CONG_DISCARD);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_congestion_accept_req: - issue an SL_CONGESTION_ACCEPT_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_congestion_accept_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_cong_accept_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_CONGESTION_ACCEPT_REQ ->");
		sl_or_l_flags(sl, SLF_CONG_ACCEPT);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_no_congestion_req: - issue an SL_CONGESTION_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_no_congestion_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_no_cong_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_NO_CONGESTION_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_NO_CONGESTION_REQ ->");
		sl_nand_l_flags(sl, (SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_power_on_req: - issue an SL_POWER_ON_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_power_on_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_power_on_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_POWER_ON_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_POWER_ON_REQ ->");
#if 0
		if (sl_get_l_state(sl) == SLS_POWER_OFF) {
			sl_set_l_state(sl, SLS_OUT_OF_SERV);
			sl_nand_l_flags(sl,
					(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_EMERGENCY |
					 SLF_LEVEL_3_IND | SLF_LINK_CONGESTED | SLF_CONG_DISCARD |
					 SLF_CONG_ACCEPT));
		}
#endif
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_optmgmt_req: - issue an SL_OPTMGMT_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_optmgmt_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_OPTMGMT_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_notify_req: - issue an SL_NOTIFY_REQ
 * @sl: lower private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_notify_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_notify_req_t *p;
	mblk_t *mp;

	if ((mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_NOTIFY_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->wq, STRLOGTX, SL_TRACE, "SL_NOTIFY_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  ===========================================================================
 *
 *  SL-MUX -> SL User Primitives
 *
 *  ===========================================================================
 *  These are primitives that are sent upstream to the SL user that sits above
 *  the upper multiplex.
 */

/**
 * m_error: - issue an M_ERROR message
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: error to indicate
 */
static fastcall int
m_error(struct sl *sl, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = err < 0 ? -err : err;
		*mp->b_wptr++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_info_ack: - issue an LMI_INFO_ACK primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_info_ack(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = LMI_CURRENT_VERSION;
		p->lmi_state = sl_get_m_state(sl);
		p->lmi_max_sdu = 272;
		p->lmi_min_sdu = 3;
		p->lmi_header_len = 6;
		p->lmi_ppa_style = LMI_STYLE2;
		p->lmi_ppa_length = 0;
		p->lmi_ppa_offset = sizeof(*p);
		p->lmi_prov_flags = 0;
		p->lmi_prov_state = SLS_POWER_OFF;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_INFO_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_ok_ack: - issue an LMI_OK_ACK primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_ok_ack(struct sl *sl, queue_t *q, mblk_t *msg, lmi_long prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (prim) {
		case LMI_ATTACH_REQ:
			p->lmi_state = sl_set_m_state(sl, LMI_DISABLED);
			break;
		case LMI_DETACH_REQ:
			p->lmi_state = sl_set_m_state(sl, LMI_UNATTACHED);
			break;
		default:
			p->lmi_state = sl_get_m_state(sl);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_OK_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_error_ack: - issue an LMI_ERROR_ACK primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_error_ack(struct sl *sl, queue_t *q, mblk_t *msg, lmi_long prim, lmi_long error)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_error_primitive = prim;
		p->lmi_errno = error < 0 ? LMI_SYSERR : error;
		p->lmi_reason = error < 0 ? -error : 0;
		p->lmi_state = sl_restore_m_state(sl);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_ERROR_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_error_reply: - issue an LMI_ERROR_ACK primitive if required
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_error_reply(struct sl *sl, queue_t *q, mblk_t *msg, lmi_long prim, lmi_long error)
{
	if (error <= 0)
		return (error);
	return lmi_tx_error_ack(sl, q, msg, prim, error);
}

#if 0
/**
 * lmi_tx_enable_con: - issue an LMI_ENABLE_CON primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_enable_con(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_ENABLE_CON");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_disable_con: - issue an LMI_DISABLE_CON primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_disable_con(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_DISABLE_CON");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_optmgmt_ack: - issue an LMI_OPTMGMT_ACK primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_OPTMGMT_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_error_ind: - issue an LMI_ERROR_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_error_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_error_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_ERROR_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_stats_ind: - issue an LMI_STATS_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_stats_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_STATS_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_STATS_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_tx_event_ind: - issue an LMI_EVENT_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_tx_event_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_EVENT_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_EVENT_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_pdu_ind: - issue an SL_PDU_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_pdu_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_pdu_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_PDU_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_PDU_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_link_congested_ind: - issue an SL_LINK_CONGESTED_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_link_congested_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_link_cong_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_LINK_CONGESTED_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_link_congestion_ceased_ind: - issue an SL_LINK_CONGESTION_CEASED_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_link_congestion_ceased_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_link_cong_ceased_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_LINK_CONGESTION_CEASED_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_retrieved_message_ind: - issue an SL_RETRIEVED_MESSAGE_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieved_msg_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_RETRIEVED_MESSAGE_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_retrieval_complete_ind: - issue an SL_RETRIEVAL_COMPLETE_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_retrieval_complete_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieval_comp_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_RETRIEVAL_COMPLETE_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_rb_cleared_ind: - issue an SL_RB_CLEARED_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_rb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rb_cleared_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_RB_CLEARED_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_bsnt_ind: - issue an SL_BSNT_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_bsnt_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_bsnt_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_BSNT_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_in_service_ind: - issue an SL_IN_SERVICE_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_in_service_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_in_service_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_IN_SERVICE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_IN_SERVICE_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_out_of_service_ind: - issue an SL_OUT_OF_SERVICE_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_out_of_service_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_out_of_service_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_OUT_OF_SERVICE_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_remote_processor_outage_ind: - issue an SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_remote_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_out_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_OUTAGE_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_remote_processor_recovered_ind: - issue an SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_remote_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_recovered_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_RECOVERED_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_rtb_cleared_ind: - issue an SL_RTB_CLEARED_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_rtb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rtb_cleared_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RTB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_RTB_CLEARED_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_retrieval_not_possible_ind: - issue an SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_retrieval_not_possible_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieval_not_poss_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_RETRIEVAL_NOT_POSSIBLE_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_bsnt_not_retrievable_ind: - issue an SL_BSNT_NOT_RETRIEVABLE_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_bsnt_not_retr_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_BSNT_NOT_RETRIEVABLE_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_optmgmt_ack: - issue an SL_OPTMGMT_ACK primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = SL_OPTMGMT_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_OPTMGMT_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_notify_ind: - issue an SL_NOTIFY_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_notify_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_EVENT_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- LMI_EVENT_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_local_processor_outage_ind: - issue an SL_LOCAL_PROCESSOR_OUTAGE_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_local_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_out_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_LOCAL_PROCESSOR_OUTAGE_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_tx_local_processor_recovered_ind: - issue an SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 * @sl: upper private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sl_tx_local_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_recovered_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_RECOVERED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->rq, STRLOGTX, SL_TRACE, "<- SL_LOCAL_PROCESSOR_RECOVERED_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  ===========================================================================
 *
 *  SL User -> SL-MUX Primitives
 *
 *  ===========================================================================
 *  These are primitives that are received from the upstream SL user that sits
 *  above the upper multiplex.
 */

static void
sl_link_sl(struct sl *slu, struct sl *sll)
{
	slu->sl.sl = sll;
	sll->sl.sl = slu;
	slu->oq = sll->wq;
	sll->oq = slu->rq;
}

static void
sl_unlink_sl(struct sl *sl)
{
	struct sl *s2;

	if ((s2 = sl->sl.sl)) {
		sl->oq = NULL;
		sl->sl.sl = NULL;
		s2->oq = NULL;
		s2->sl.sl = NULL;
	}
}

/**
 * lmi_rx_info_req: - process an LMI_INFO_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the LMI_INFO_REQ primitive
 */
static noinline fastcall int
lmi_rx_info_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;

	switch (sl_get_m_state(sl)) {
	case LMI_UNATTACHED:
	case LMI_UNUSABLE:
	case LMI_ATTACH_PENDING:
	case LMI_DETACH_PENDING:
		return lmi_tx_info_ack(sl, q, mp);
	case LMI_DISABLED:
	case LMI_ENABLE_PENDING:
	case LMI_ENABLED:
	case LMI_DISABLE_PENDING:
	default:
		break;
	}
	return (1);
}

/**
 * lmi_rx_attach_req: - process an LMI_ATTACH_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the LMI_ATTACH_REQ primitive
 */
static noinline fastcall int
lmi_rx_attach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct slmux_ppa ppa;
	int err;
	struct sl *lower;
	unsigned long flags;
	size_t ppa_len;
	caddr_t ppa_ptr;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (!MBLKIN(mp, p->lmi_ppa_offset, p->lmi_ppa_length))
		goto badppa;
	if (sl_get_m_state(sl) != LMI_UNATTACHED)
		goto outstate;
	ppa_len = p->lmi_ppa_length;
	if (ppa_len == 0 || ppa_len > SLMUX_CLEI_MAX)
		goto badppa;
	ppa_ptr = (caddr_t) (mp->b_rptr + p->lmi_ppa_offset);
	switch (ppa_len) {
	case 1:
		ppa.slm_index = 0;
		ppa.slm_ppa = *(uint8_t *) ppa_ptr;
		ppa.slm_clei[0] = '\0';
		break;
	case 2:
		ppa.slm_index = 0;
		ppa.slm_ppa = *(uint16_t *) ppa_ptr;
		ppa.slm_clei[0] = '\0';
		break;
	case 4:
		ppa.slm_index = 0;
		ppa.slm_ppa = *(uint32_t *) ppa_ptr;
		ppa.slm_clei[0] = '\0';
		break;
	default:
		ppa.slm_index = 0;
		ppa.slm_ppa = 0;
		if (ppa_len < SLMUX_CLEI_MAX)
			ppa.slm_clei[ppa_len] = '\0';
		strncpy(ppa.slm_clei, ppa_ptr, ppa_len);
		break;
	}
	sl_set_m_state(sl, LMI_ATTACH_PENDING);
	write_lock_irqsave(&sl_mux_lock, flags);
	if (!(lower = sl_find_lower(sl, &ppa))) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		goto badppa;
	}
	if (sl_get_m_state(lower) == LMI_UNUSABLE) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		goto fatalerr;
	}
	if (sl_get_m_state(lower) != LMI_DISABLED) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		goto initfailed;
	}
	sl_link_sl(sl, lower);
	write_unlock_irqrestore(&sl_mux_lock, flags);
	if ((err = lmi_tx_ok_ack(sl, q, mp, LMI_ATTACH_REQ))) {
		write_lock_irqsave(&sl_mux_lock, flags);
		sl_unlink_sl(sl);
		write_unlock_irqrestore(&sl_mux_lock, flags);
		return (err);
	}
	return (0);
      initfailed:
	err = LMI_INITFAILED;
	goto error;
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      badprim:
	err = LMI_BADPRIM;
	goto error;
      error:
	return lmi_error_reply(sl, q, mp, LMI_ATTACH_REQ, err);
}

/**
 * lmi_rx_detach_req: - process an LMI_DETACH_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the LMI_DETACH_REQ primitive
 */
static noinline fastcall int
lmi_rx_detach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	unsigned long flags;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (sl_get_m_state(sl) != LMI_DISABLED)
		goto outstate;
	sl_set_m_state(sl, LMI_DETACH_PENDING);
	write_lock_irqsave(&sl_mux_lock, flags);
	sl_unlink_sl(sl);
	write_unlock_irqrestore(&sl_mux_lock, flags);
	return lmi_tx_ok_ack(sl, q, mp, LMI_DETACH_REQ);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      badprim:
	err = LMI_BADPRIM;
	goto error;
      error:
	return lmi_error_reply(sl, q, mp, LMI_DETACH_REQ, err);
}

int lmi_passalong(struct sl *, queue_t *, mblk_t *);

/**
 * lmi_rx_enable_req: - process an LMI_ENABLE_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the LMI_ENABLE_REQ primitive
 */
static fastcall int
lmi_rx_enable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (sl_get_m_state(sl) != LMI_DISABLED)
		goto outstate;
	if (!MBLKIN(mp, p->lmi_rem_offset, p->lmi_rem_length))
		goto badaddr;
	if (!sl->oq)
		goto initfailed;
	return lmi_passalong(sl, q, mp);
      initfailed:
	err = LMI_INITFAILED;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      badprim:
	err = LMI_BADPRIM;
	goto error;
      error:
	return lmi_error_reply(sl, q, mp, LMI_ENABLE_REQ, err);
}

/**
 * lmi_rx_disable_req: - process an LMI_DISABLE_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the LMI_DISABLE_REQ primitive
 */
static fastcall int
lmi_rx_disable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (sl_get_m_state(sl) != LMI_ENABLED)
		goto outstate;
	if (!sl->oq)
		goto fatalerror;
	return lmi_passalong(sl, q, mp);
      fatalerror:
	err = LMI_FATALERR;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      badprim:
	err = LMI_BADPRIM;
	goto error;
      error:
	return lmi_error_reply(sl, q, mp, LMI_DISABLE_REQ, err);

	(void) p;
	return (1);
}

/**
 * lmi_rx_optmgmt_req: - process an LMI_OPTMGMT_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the LMI_OPTMGMT_REQ primitive
 */
static fastcall int
lmi_rx_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_pdu_req: - process an SL_PDU_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_PDU_REQ primitive
 */
static fastcall int
sl_rx_pdu_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_emergency_req: - process an SL_EMERGENCY_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_EMERGENCY_REQ primitive
 */
static fastcall int
sl_rx_emergency_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_emergency_ceases_req: - process an SL_EMERGENCY_CEASES_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_EMERGENCY_CEASES_REQ primitive
 */
static fastcall int
sl_rx_emergency_ceases_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_start_req: - process an SL_START_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_START_REQ primitive
 */
static fastcall int
sl_rx_start_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_stop_req: - process an SL_STOP_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_STOP_REQ primitive
 */
static fastcall int
sl_rx_stop_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_retrieve_bsnt_req: - process an SL_BSNT_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_BSNT_REQ primitive
 */
static fastcall int
sl_rx_retrieve_bsnt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_retrieval_request_and_fsnc_req: - process an SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primitive
 */
static fastcall int
sl_rx_retrieval_request_and_fsnc_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_clear_buffers_req: - process an SL_CLEAR_BUFFERS_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_CLEAR_BUFFERS_REQ primitive
 */
static fastcall int
sl_rx_clear_buffers_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_clear_rtb_req: - process an SL_CLEAR_RTB_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_CLEAR_RTB_REQ primitive
 */
static fastcall int
sl_rx_clear_rtb_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_continue_req: - process an SL_CONTINUE_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_CONTINUE_REQ primitive
 */
static fastcall int
sl_rx_continue_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_continue_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_local_processor_outage_req: - process an SL_PROCESSOR_OUTAGE_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_PROCESSOR_OUTAGE_REQ primitive
 */
static fastcall int
sl_rx_local_processor_outage_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_resume_req: - process an SL_RESUME_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_RESUME_REQ primitive
 */
static fastcall int
sl_rx_resume_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_congestion_discard_req: - process an SL_CONGESTION_DISCARD_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_CONGESTION_DISCARD_REQ primitive
 */
static fastcall int
sl_rx_congestion_discard_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_cong_discard_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_congestion_accept_req: - process an SL_CONGESTION_ACCEPT_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_CONGESTION_ACCEPT_REQ primitive
 */
static fastcall int
sl_rx_congestion_accept_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_cong_accept_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_no_congestion_req: - process an SL_NO_CONGESTION_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_NO_CONGESTION_REQ primitive
 */
static fastcall int
sl_rx_no_congestion_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_no_cong_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_power_on_req: - process an SL_POWER_ON_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_POWER_ON_REQ primitive
 */
static fastcall int
sl_rx_power_on_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_power_on_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_optmgmt_req: - process an SL_OPTMGMT_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_OPTMGMT_REQ primitive
 */
static fastcall int
sl_rx_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

#if 0
/**
 * sl_rx_notify_req: - process an SL_NOTIFY_REQ primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the SL_NOTIFY_REQ primitive
 */
static fastcall int
sl_rx_notify_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_notify_req_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}
#endif
/**
 * sl_rx_other_req: - process an unknown primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the unknown primitive
 */
static fastcall int
sl_rx_other_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_long *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_data_req: - process an M_DATA primitive
 * @sl: upper private structure
 * @q: upper write queue
 * @mp: the M_DATA primitive
 */
static fastcall int
sl_rx_data_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return (1);
}

/*
 *  ===========================================================================
 *
 *  SL Provider -> SL-MUX Primitives
 *
 *  ===========================================================================
 *  These are primitives that are received from the downstream SL provider that sits beneath the
 *  lower multiplex.
 */

/**
 * lmi_rx_info_ack: - process an LMI_INFO_ACK primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_INFO_ACK primitive
 */
static fastcall int
lmi_rx_info_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	sl_set_m_state(sl, p->lmi_state);
	return (1);
}

/**
 * lmi_rx_ok_ack: - process an LMI_OK_ACK primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_OK_ACK primitive
 */
static fastcall int
lmi_rx_ok_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	sl_set_m_state(sl, p->lmi_state);
	return (1);
}

/**
 * lmi_rx_error_ack: - process an LMI_ERROR_ACK primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_ERROR_ACK primitive
 */
static fastcall int
lmi_rx_error_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	sl_set_m_state(sl, p->lmi_state);
	return (1);
}

/**
 * lmi_rx_enable_con: - process an LMI_ENABLE_CON primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_ENABLE_CON primitive
 */
static fastcall int
lmi_rx_enable_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	sl_set_m_state(sl, p->lmi_state);
	return (1);
}

/**
 * lmi_rx_disable_con: - process an LMI_DISABLE_CON primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_DISABLE_CON primitive
 */
static fastcall int
lmi_rx_disable_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	sl_set_m_state(sl, p->lmi_state);
	return (1);
}

/**
 * lmi_rx_optmgmt_ack: - process an LMI_OPTMGMT_ACK primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_OPTMGMT_ACK primitive
 */
static fastcall int
lmi_rx_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * lmi_rx_error_ind: - process an LMI_ERROR_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_ERROR_IND primitive
 */
static fastcall int
lmi_rx_error_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	sl_set_m_state(sl, p->lmi_state);
	return (1);
}

/**
 * lmi_rx_stats_ind: - process an LMI_STATS_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_STATS_IND primitive
 */
static fastcall int
lmi_rx_stats_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * lmi_rx_event_ind: - process an LMI_EVENT_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_EVENT_IND primitive
 */
static fastcall int
lmi_rx_event_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

/**
 * sl_rx_pdu_ind: - process an LMI_PDU_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the LMI_PDU_IND primitive
 */
static fastcall int
sl_rx_pdu_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_pdu_ind_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return (1);
}

int sl_passalong(struct sl *, queue_t *, mblk_t *);

/**
 * sl_rx_link_congested_ind: - process an SL_LINK_CONGESTED_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_LINK_CONGESTED_IND primitive
 */
static fastcall int
sl_rx_link_congested_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	(void) p;
	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
#if 0
	sl_set_l_state(sl, SLS_IN_SERV);
	if (p->sl_cong_status || p->sl_disc_status)
		sl_or_l_flags(sl, SLF_LINK_CONGESTED);
#endif
	return sl_passalong(sl, q, mp);
      badprim:
	err = LMI_BADPRIM;
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * sl_rx_link_congestion_ceased_ind: - process an SL_LINK_CONGESTION_CEASED_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_LINK_CONGESTION_CEASED_IND primitive
 */
static fastcall int
sl_rx_link_congestion_ceased_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ceased_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_IN_SERV);
	if (!p->sl_cong_status && !p->sl_disc_status)
		sl_nand_l_flags(sl, SLF_LINK_CONGESTED);
#endif
	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_retrieved_message_ind: - process an SL_RETRIEVED_MESSAGE_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_RETRIEVED_MESSAGE_IND primitive
 */
static fastcall int
sl_rx_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieved_msg_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_OUT_OF_SERV);
	sl_nand_l_flags(sl,
			(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_LEVEL_3_IND | SLF_LINK_CONGESTED
			 | SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
#endif
	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_retrieval_complete_ind: - process an SL_RETRIEVAL_COMPLETE_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_RETRIEVAL_COMPLETE_IND primitive
 */
static fastcall int
sl_rx_retrieval_complete_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_comp_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_OUT_OF_SERV);
	sl_nand_l_flags(sl,
			(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_LEVEL_3_IND | SLF_LINK_CONGESTED
			 | SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
#endif
	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_rb_cleared_ind: - process an SL_RB_CLEARED_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_RB_CLEARED_IND primitive
 */
static fastcall int
sl_rx_rb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_rb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_OUT_OF_SERV);
	sl_nand_l_flags(sl,
			(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_LEVEL_3_IND | SLF_LINK_CONGESTED
			 | SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
#endif
	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_bsnt_ind: - process an SL_BSNT_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_BSNT_IND primitive
 */
static fastcall int
sl_rx_bsnt_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_bsnt_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_OUT_OF_SERV);
	sl_nand_l_flags(sl,
			(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_LEVEL_3_IND | SLF_LINK_CONGESTED
			 | SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
#endif
	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_in_service_ind: - process an SL_IN_SERVICE_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_IN_SERVICE_IND primitive
 */
static fastcall int
sl_rx_in_service_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_in_service_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_IN_SERV);
	sl_nand_l_flags(sl,
			(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_LEVEL_3_IND | SLF_LINK_CONGESTED
			 | SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
#endif
	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_out_of_service_ind: - process an SL_OUT_OF_SERVICE_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_OUT_OF_SERVICE_IND primitive
 */
static fastcall int
sl_rx_out_of_service_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_out_of_service_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_OUT_OF_SERV);
	sl_nand_l_flags(sl,
			(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_LEVEL_3_IND | SLF_LINK_CONGESTED
			 | SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
#endif

	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_remote_processor_outage_ind: - process an SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 */
static fastcall int
sl_rx_remote_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_rem_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_IN_SERV);
	sl_or_l_flags(sl, SLF_REM_PROC_OUT);
	sl_nand_l_flags(sl, SLF_LEVEL_3_IND);
#endif

	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_remote_processor_recovered_ind: - process an SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 */
static fastcall int
sl_rx_remote_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_rem_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_IN_SERV);
	sl_nand_l_flags(sl, SLF_REM_PROC_OUT);
#endif

	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_rtb_cleared_ind: - process an SL_RTB_CLEARED_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_RTB_CLEARED_IND primitive
 */
static fastcall int
sl_rx_rtb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_rtb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_OUT_OF_SERV);
	sl_nand_l_flags(sl,
			(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_LEVEL_3_IND | SLF_LINK_CONGESTED
			 | SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
#endif

	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_retrieval_not_possible_ind: - process an SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 */
static fastcall int
sl_rx_retrieval_not_possible_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_not_poss_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_OUT_OF_SERV);
	sl_nand_l_flags(sl,
			(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_LEVEL_3_IND | SLF_LINK_CONGESTED
			 | SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
#endif

	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_bsnt_not_retrievable_ind: - process an SL_BSNT_NOT_RETRIEVABLE_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_BSNT_NOT_RETRIEVABLE_IND primitive
 */
static fastcall int
sl_rx_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_bsnt_not_retr_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_set_l_state(sl, SLS_OUT_OF_SERV);
	sl_nand_l_flags(sl,
			(SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_LEVEL_3_IND | SLF_LINK_CONGESTED
			 | SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
#endif

	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_optmgmt_ack: - process an SL_OPTMGMT_ACK primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_OPTMGMT_ACK primitive
 */
static fastcall int
sl_rx_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_notify_ind: - process an SL_NOTIFY_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_NOTIFY_IND primitive
 */
static fastcall int
sl_rx_notify_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_processor_outage_ind: - process an SL_PROCESSOR_OUTAGE_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_PROCESSOR_OUTAGE_IND primitive
 */
static fastcall int
sl_rx_local_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_loc_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_or_l_flags(sl, (SLF_LOC_PROC_OUT));
#endif

	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_local_processor_recovered_ind: - process an SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 */
static fastcall int
sl_rx_local_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_loc_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;

#if 0
	sl_nand_l_flags(sl, (SLF_LOC_PROC_OUT));
#endif

	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_other_ind: - process an unknown primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the unknown primitive
 */
static fastcall int
sl_rx_other_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_long *p = (typeof(p)) mp->b_rptr;

	(void) p;
	return sl_passalong(sl, q, mp);
}

/**
 * sl_rx_data_ind: - process an M_DATA primitive
 * @sl: lower private structure
 * @q: lower read queue
 * @mp: the M_DATA primitive
 */
static fastcall int
sl_rx_data_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_passalong(sl, q, mp);
}

/*
 *  ===========================================================================
 *
 *  INPUT-OUTPUT CONTROLS
 *
 *  ===========================================================================
 */

static void
sl_init(struct sl *sl, queue_t *q, int index, cred_t *crp)
{
	/* Initialize lower stream private structure. */
	bzero(sl, sizeof(*sl));
	sl->rq = RD(q);
	sl->wq = WR(q);
	sl->oq = NULL;
	sl->sl.sl = NULL;
	sl->state = LMI_UNUSABLE;
	sl->oldstate = LMI_UNUSABLE;
	sl->lm.lm = NULL;
	sl->lm.next = NULL;
	sl->lm.prev = &sl->lm.next;
	sl->mon.mon = NULL;
	sl->mon.next = NULL;
	sl->mon.prev = &sl->mon.next;
	sl->ioc.id = 0;
	sl->ioc.sl = NULL;
	sl->cred = *crp;
	sl->ppa.slm_index = index;
	sl->ppa.slm_ppa = 0;
	sl->ppa.slm_clei[0] = '\0';
}

static void
sl_link_lm(struct sl *lm, struct sl *sl)
{
	sl->lm.lm = lm;
	if ((sl->lm.next = lm->lm.lm))
		sl->lm.next->lm.prev = &sl->lm.next;
	sl->lm.prev = &lm->lm.lm;
	lm->lm.lm = sl;
}

static void
sl_unlink_lm(struct sl *sl)
{
	if ((*(sl->lm.prev) = sl->lm.next))
		sl->lm.next->lm.prev = sl->lm.prev;
	sl->lm.next = NULL;
	sl->lm.prev = &sl->lm.next;
	sl->lm.lm = NULL;
}

static void
sl_link_mon(struct sl *mon, struct sl *sl)
{
	sl->mon.mon = mon;
	if ((sl->mon.next = mon->mon.mon))
		sl->mon.next->mon.prev = &sl->mon.next;
	sl->mon.prev = &mon->mon.mon;
	mon->mon.mon = sl;
}

static void
sl_unlink_mon(struct sl *sl)
{
	if ((*(sl->mon.prev) = sl->mon.next))
		sl->mon.next->mon.prev = sl->mon.prev;
	sl->mon.next = NULL;
	sl->mon.prev = &sl->mon.next;
	sl->mon.mon = NULL;
}

/**
 * sl_i_link: - perform I_LINK operation
 * @lm: private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * The driver supports I_LINK operations; however, any SL stream that is linked with an I_LINK
 * operation can only be managed by the control stream linking the lower Stream and cannot be shared
 * across other upper Streams unless configured against a layer management id.
 *
 * Note that if this is not the first SL linked and there are running users, this SL will not be
 * available to them until it is configured.  If this is the first SL, there cannot be running
 * users.
 *
 * Several steps.  Allocate a private structure (using mi_open_alloc()) and associate it with the
 * lower stream.
 */
static noinline fastcall __unlikely int
sl_i_link(struct sl *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	unsigned long flags;
	lmi_info_req_t *p;
	struct sl *sl;
	mblk_t *rp;
	int err;

	if ((rp = mi_allocb(q, sizeof(*p), BPRI_MED)) == NULL) {
		mi_copy_done(q, mp, ENOBUFS);
		return (0);
	}
	if ((sl = (typeof(sl)) mi_open_alloc_cache(sl_priv_cachep, GFP_ATOMIC)) == NULL) {
		mi_copy_done(q, mp, ENOMEM);
		freeb(rp);
		return (0);
	}
	sl_init(sl, l->l_qtop, l->l_index, ioc->ioc_cr);
	write_lock_irqsave(&sl_mux_lock, flags);
	if ((err = mi_open_link(&sl_links, (caddr_t) sl, NULL, 0, MODOPEN, ioc->ioc_cr))) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		mi_close_free((caddr_t) sl);
		mi_copy_done(q, mp, err);
		freeb(rp);
		return (0);
	}
	sl_link_lm(lm, sl);
	mi_attach(l->l_qtop, (caddr_t) sl);
	sl_links_count++;
	write_unlock_irqrestore(&sl_mux_lock, flags);
	mi_copy_done(q, mp, 0);
	/* send immediate information request */
	rp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->lmi_primitive = LMI_INFO_REQ;
	rp->b_wptr += sizeof(*p);
	putnext(sl->wq, rp);
	qenable(sl->rq);
	return (0);
}

/**
 * sl_i_plink: - perform I_PLINK operation
 * @lm: private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 */
static noinline fastcall __unlikely int
sl_i_plink(struct sl *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	unsigned long flags;
	lmi_info_req_t *p;
	struct sl *sl;
	mblk_t *rp;
	int err;

	if ((rp = mi_allocb(q, sizeof(*p), BPRI_MED)) == NULL) {
		mi_copy_done(q, mp, ENOBUFS);
		return (0);
	}
	if ((sl = (typeof(sl)) mi_open_alloc_cache(sl_priv_cachep, GFP_ATOMIC)) == NULL) {
		mi_copy_done(q, mp, ENOMEM);
		freemsg(rp);
		return (0);
	}
	sl_init(sl, l->l_qtop, l->l_index, ioc->ioc_cr);
	write_lock_irqsave(&sl_mux_lock, flags);
	/* Only allow master control Stream to do permanent links. */
	if (sl_ctrl != lm) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		mi_close_free((caddr_t) sl);
		mi_copy_done(q, mp, EPERM);
		freemsg(rp);
		return (0);
	}
	if ((err = mi_open_link(&sl_links, (caddr_t) sl, NULL, 0, MODOPEN, ioc->ioc_cr))) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		mi_close_free((caddr_t) sl);
		mi_copy_done(q, mp, err);
		freemsg(rp);
		return (0);
	}
	sl_link_lm(lm, sl);
	mi_attach(l->l_qtop, (caddr_t) sl);
	sl_links_count++;
	write_unlock_irqrestore(&sl_mux_lock, flags);
	mi_copy_done(q, mp, 0);
	/* send immediate information request */
	rp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->lmi_primitive = LMI_INFO_REQ;
	rp->b_wptr += sizeof(*p);
	putnext(sl->wq, rp);
	qenable(sl->rq);
	return (0);
}

/**
 * sl_i_unlink: - perform I_UNLINK operation
 * @lm: private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 */
static noinline fastcall __unlikely int
sl_i_unlink(struct sl *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	unsigned long flags;
	struct sl *sl;

	write_lock_irqsave(&sl_mux_lock, flags);
	if (unlikely((sl = sl_acquire(l->l_qtop)) == NULL)) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		return (-EDEADLK);
	}
	/* if issued by user, check credentials */
	if (unlikely((ioc->ioc_flag & IOC_MASK) == IOC_NONE
		     && drv_priv(ioc->ioc_cr) != 0 && ioc->ioc_cr->cr_uid != sl->cred.cr_uid)) {
		sl_release(sl);
		write_unlock_irqrestore(&sl_mux_lock, flags);
		mi_copy_done(q, mp, EPERM);
		return (0);
	}
	sl_unlink_lm(sl);	/* remove from layer manager stream */
	sl_unlink_mon(sl);	/* remove from monitor stream */
	mi_detach(l->l_qtop, (caddr_t) sl);
	sl_links_count--;
	mi_close_unlink(&sl_links, (caddr_t) sl);
	sl_release(sl);
	write_unlock_irqrestore(&sl_mux_lock, flags);
	mi_close_free((caddr_t) sl);
	/* Should probably flush queues in case a Stream head is reattached.  */
	flushq(RD(l->l_qtop), FLUSHDATA);
	mi_copy_done(q, mp, 0);
	return (0);
}

/**
 * sl_i_punlink: - perform I_PUNLINK operation
 * @lm: private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 */
static noinline fastcall __unlikely int
sl_i_punlink(struct sl *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	unsigned long flags;
	struct sl *sl = NULL;

	write_lock_irqsave(&sl_mux_lock, flags);
	if (unlikely((sl = sl_acquire(l->l_qtop)) == NULL)) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		return (-EDEADLK);
	}
	/* Always issued by user, check credentials.  Only the master control stream is allows to
	   create permanent links, however, to avoid difficulties with hanging permanent links,
	   permit the owner of the link or the super user to always undo permanent links. */
	if (lm != sl_ctrl && drv_priv(ioc->ioc_cr) != 0 && ioc->ioc_cr->cr_uid != sl->cred.cr_uid) {
		sl_release(sl);
		write_unlock_irqrestore(&sl_mux_lock, flags);
		mi_copy_done(q, mp, EPERM);
		return (0);
	}
	sl_unlink_lm(sl);	/* remove from layer manager stream */
	sl_unlink_mon(sl);	/* remove from monitor stream */
	mi_detach(l->l_qtop, (caddr_t) sl);
	sl_links_count--;
	mi_close_unlink(&sl_links, (caddr_t) sl);
	sl_release(sl);
	write_unlock_irqrestore(&sl_mux_lock, flags);
	mi_close_free((caddr_t) sl);
	/* Should probably flush queues in case a Stream head is reattached.  */
	flushq(RD(l->l_qtop), FLUSHDATA);
	mi_copy_done(q, mp, 0);
	return (0);
}

/**
 * sl_ioctl_ioccpass: - process M_IOCTL message for XX_IOCCPASS operation
 * @lm: layer management stream
 * @q: upper write queue
 * @mp: the M_IOCTL message
 *
 * These are pass-along style M_IOCTL messages.  They have a lower multiplex index number in the
 * first integer of the message argument.  When the index number is zero (0), the message is
 * intended for the current stream.  When the index number is non-zero, the message is intended for
 * the lower multiplex stream to which the index corresponds.
 */
static noinline fastcall __unlikely int
sl_ioctl_ioccpass(struct sl *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct lmi_pass *arg = (typeof(arg)) mp->b_cont->b_rptr;
	unsigned long flags;
	struct sl *sl;

	if (ioc->ioc_count == TRANSPARENT) {
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
	write_lock_irqsave(&sl_mux_lock, flags);
	/* Find the lower stream */
	if ((sl = sl_find_index(lm, arg->index)) == NULL) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
	/* We must be the management stream for the lower stream, or the lower stream must be
	   associated with this upper stream. */
	if (sl->lm.lm != lm && lm->oq != sl->wq) {
		write_unlock_irqrestore(&sl_mux_lock, flags);
		mi_copy_done(q, mp, EPERM);
		return (0);
	}
	lm->ioc.id = ioc->ioc_id;
	lm->ioc.sl = sl;
	write_unlock_irqrestore(&sl_mux_lock, flags);
	read_lock(&sl_mux_lock);
	arg->index = 0;		/* for next level */
	/* recheck because we released locks ane retook them as read locks */
	if (lm->ioc.id != ioc->ioc_id || lm->ioc.sl == NULL || sl->ioc.sl->wq == NULL) {
		read_unlock(&sl_mux_lock);
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
	putnext(lm->ioc.sl->wq, mp);
	read_unlock(&sl_mux_lock);
	return (0);
}

/**
 * sl_iocdata_ioccpas: - process M_IOCDATA for XX_IOCCPASS input-output control
 * @lm: layer management stream
 * @q: upper write queue
 * @mp: the M_IOCDATA message
 */
static noinline fastcall __unlikely int
sl_iocdata_ioccpass(struct sl *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	read_lock(&sl_mux_lock);
	if (lm->ioc.id != ioc->ioc_id || lm->ioc.sl == NULL || lm->ioc.sl->wq == NULL) {
		read_unlock(&sl_mux_lock);
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
	putnext(lm->ioc.sl->wq, mp);
	read_unlock(&sl_mux_lock);
	return (0);
}

/**
 * slm_ioctl: - process SLM_IOC_MAGIC M_IOCTL message
 * @lm: layer manager stream
 * @q: upper write queue
 * @mp: the M_IOCTL message
 *
 * This is the first step of the common IOCTL operation for SL-MUX input-output controls.  The first
 * step is normally a simply copyin operation.
 */
static noinline fastcall __unlikely int
slm_ioctl(struct sl *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int err = 0, size = -1;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SLM_IOCSPPA):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SLM_IOCSPPA)");
		size = sizeof(struct slmux_ppa);
		break;
	case _IOC_NR(SLM_IOCGPPA):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SLM_IOCGPPA)");
		size = sizeof(struct slmux_ppa);
		break;
	case _IOC_NR(SLM_IOCLPPA):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SLM_IOCLPPA)");
		/* situation where passed pointer is NULL or copyin length is zero */
		if ((ioc->ioc_count == TRANSPARENT && *(long *) mp->b_cont->b_rptr == 0) ||
		    ioc->ioc_count == 0) {
			/* just return length */
			mi_copy_set_rval(mp, sl_links_count);
			mi_copy_done(q, mp, 0);
			return (0);
		}
		size = sizeof(struct slmux_ppa_list);
		break;
	case _IOC_NR(SLM_IOCSMON):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SLM_IOCSMON)");
		size = sizeof(struct slmux_ppa);
		break;
	case _IOC_NR(SLM_IOCCMON):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SLM_IOCCMON)");
		size = sizeof(struct slmux_ppa);
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(SLM_IOC????)");
		err = EINVAL;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0) {
		mi_copy_done(q, mp, err);
		return (0);
	}
	mi_copyin(q, mp, NULL, size);
	return (0);
}

/**
 * slm_copyin: - process SLM_IOC_MAGIC M_IOCDATA (copyin) message
 * @lm: layer manager stream
 * @q: upper write queue
 * @mp: the M_IOCDATA message
 *
 * This is the second step of the common IOCTL operation for SL-MUX input-output controls.  The
 * second step consists of processing an M_IOCDATA message that contains an implicit or explicit
 * result of an M_IOCTL/M_COPYIN operation.  This is where most of the work is done on these
 * input-output controls.
 */
static noinline fastcall __unlikely int
slm_copyin(struct sl *lm, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	void *arg = (typeof(arg)) mp->b_cont->b_rptr;
	struct slmux_ppa *p = arg;
	struct slmux_ppa_list *l = arg;
	unsigned long flags;
	int i, num, err = 0;
	struct sl *sl, *s;
	mblk_t *dp;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SLM_IOCSPPA):
		/* SLM_IOCSPPA: Set the PPA for a given lower multiplex stream.  The lower
		   multiplex stream can be found by index, existing PPA or existing CLEI.  Only
		   when the PPA or CLEI is non-zero is it set.  Both the PPA and CLEI to set must
		   be unique and can only identify the specified stream. */
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SLM_IOCSPPA)");
		if ((dp = mi_copyout_alloc(q, mp, NULL, sizeof(*p), 0))) {
			write_lock_irqsave(&sl_mux_lock, flags);
			if ((sl = sl_find_lower(lm, p)) &&
			    ((!p->slm_ppa || !(s = sl_find_ppa(lm, p->slm_ppa)) || s == sl) ||
			     (!p->slm_clei[0] || !(s = sl_find_clei(lm, p->slm_clei))
			      || s == sl))) {
				if (p->slm_ppa != 0)
					sl->ppa.slm_ppa = p->slm_ppa;
				if (p->slm_clei[0] != '\0')
					strncpy(sl->ppa.slm_clei, p->slm_clei, SLMUX_CLEI_MAX);
				*(struct slmux_ppa *) dp->b_rptr = sl->ppa;
			} else
				err = EINVAL;
			write_unlock_irqrestore(&sl_mux_lock, flags);
		} else
			err = ENOBUFS;
		break;
	case _IOC_NR(SLM_IOCGPPA):
		/* SLM_IOCGPPA: Get the PPA for a given lower multiplex stream.  The lower
		   multiplex stream can eb found by index, PPA or CLEI.  The entire triplet is
		   returned. */
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SLM_IOCGPPA)");
		if ((dp = mi_copyout_alloc(q, mp, NULL, sizeof(*p), 0))) {
			write_lock_irqsave(&sl_mux_lock, flags);
			if ((sl = sl_find_lower(lm, p))) {
				*(struct slmux_ppa *) dp->b_rptr = sl->ppa;
			} else
				err = EINVAL;
			write_unlock_irqrestore(&sl_mux_lock, flags);
		} else
			err = ENOBUFS;
		break;
	case _IOC_NR(SLM_IOCLPPA):
		/* SLM_IOCLPPA: List the PPAs for all lower multiplex streams.  The maximum number
		   of elements to list is in the passed in slmux_ppa_list structure.  The return
		   value of the ioctl is the number of elements available for listing. */
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SLM_IOCLPPA)");
		if (l->slm_list_num >= 0) {
			num = l->slm_list_num;
			i = sizeof(*l) + num * sizeof(struct slmux_ppa);
			if ((dp = mi_copyout_alloc(q, mp, NULL, i, 0))) {
				l = (typeof(l)) dp->b_rptr;
				p = l->slm_list_array;
				read_lock(&sl_mux_lock);
				l->slm_list_num = num;
				for (i = 0, sl = (typeof(sl)) mi_first_ptr(&sl_links);
				     i < sl_links_count && i < num
				     && (sl = (typeof(sl)) mi_next_ptr((caddr_t) sl)); i++, p++) {
					*p = sl->ppa;
				}
				if (i < l->slm_list_num)
					l->slm_list_num = i;
				mi_copy_set_rval(mp, sl_links_count);
				read_unlock(&sl_mux_lock);
			} else
				err = ENOBUFS;
		} else
			err = EINVAL;
		break;
	case _IOC_NR(SLM_IOCSMON):
		/* SLM_IOCSMON: Set a stream for monitoring.  The stream to monitor on the current
		   stream is identified by its index, PPA or CLEI.  A stream can only be monitored
		   by one monitoring stream at a time. */
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SLM_IOCSMON)");
		if ((dp = mi_copyout_alloc(q, mp, NULL, sizeof(*p), 0))) {
			write_lock_irqsave(&sl_mux_lock, flags);
			if ((sl = sl_find_lower(lm, p)) && !sl->mon.mon) {
				sl_link_mon(lm, sl);
				*(struct slmux_ppa *) dp->b_rptr = sl->ppa;
			} else
				err = EINVAL;
			write_unlock_irqrestore(&sl_mux_lock, flags);
		} else
			err = ENOBUFS;
		break;
	case _IOC_NR(SLM_IOCCMON):
		/* SLM_IOCCMON: Clear a stream from monitoring.  The stream to clear from
		   monitoring is identified by its index, PPA or CLEI. */
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SLM_IOCCMON)");
		if ((dp = mi_copyout_alloc(q, mp, NULL, sizeof(*p), 0))) {
			write_lock_irqsave(&sl_mux_lock, flags);
			if ((sl = sl_find_lower(lm, p)) && sl->mon.mon == lm) {
				sl_unlink_mon(sl);
				*(struct slmux_ppa *) dp->b_rptr = sl->ppa;
			} else
				err = EINVAL;
			write_unlock_irqrestore(&sl_mux_lock, flags);
		} else
			err = ENOBUFS;
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SLM_IOC????)");
		err = EINVAL;
		break;
	}
	if (err >= 0) {
		if (err == 0)
			mi_copyout(q, mp);
		else {
			mi_copy_done(q, mp, err);
			err = 0;
		}
	}
	return (err);
}

/**
 * slm_copyout: - process SLM_IOC_MAGIC M_IOCDATA (copyout) message
 * @lm: layer manager stream
 * @q: upper write queue
 * @mp: the M_IOCDATA message
 */
static noinline fastcall __unlikely int
slm_copyout(struct sl *lm, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(%s)", sl_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SLM_IOCSPPA):
		goto copyout;
	case _IOC_NR(SLM_IOCGPPA):
		goto copyout;
	case _IOC_NR(SLM_IOCLPPA):
		goto copyout;
	case _IOC_NR(SLM_IOCSMON):
		goto copyout;
	case _IOC_NR(SLM_IOCCMON):
		goto copyout;
	      copyout:
		mi_copyout(q, mp);
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	return (0);
}

/*
 *  ===========================================================================
 *
 *  UPPER MULTIPLEX MESSAGE HANDLING
 *
 *  ===========================================================================
 *  These are STREAMS messages received from above the upper multipex on the
 *  upper write queue.
 */

static noinline fastcall int
sl_passalong_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *mon, *lower;
	int err = 0;

	read_lock(&sl_mux_lock);
	if (sl->oq) {
		if (DB_TYPE(mp) < QPCTL && !bcanputnext(sl->oq, mp->b_band)) {
			read_unlock(&sl_mux_lock);
			return (-EBUSY);
		}
		if ((lower = sl->sl.sl) && (mon = lower->mon.mon)) {
			mblk_t *bp, *dp = NULL;
			struct slmux_mon *p;

			if (!canputnext(mon->rq)) {
				read_unlock(&sl_mux_lock);
				return (-EBUSY);
			}
			if ((bp = mi_allocb(q, sizeof(*p) + mp->b_wptr - mp->b_rptr,
					    BPRI_MED)) == NULL) {
				read_unlock(&sl_mux_lock);
				return (-ENOBUFS);
			}
			if (mp->b_cont && (dp = dupmsg(mp->b_cont)) == NULL) {
				read_unlock(&sl_mux_lock);
				freeb(bp);
				mi_bufcall(q, 0, BPRI_MED);
				return (-ENOBUFS);
			}
			bp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) bp->b_rptr;
			p->mon_ppa = lower->ppa;
			p->mon_dir = 1;	/* write */
			p->mon_msg_type = DB_TYPE(mp);
			p->mon_msg_band = mp->b_band;
			p->mon_msg_flags = mp->b_flag;
			bp->b_wptr += sizeof(*p);
			bcopy(mp->b_rptr, bp->b_wptr, mp->b_wptr - mp->b_rptr);
			bp->b_wptr += mp->b_wptr - mp->b_rptr;
			bp->b_cont = dp;
			putnext(mon->rq, mp);
		}
		putnext(sl->oq, mp);
	} else
		freemsg(mp);
	read_unlock(&sl_mux_lock);
	return (err);
}

/**
 * sl_w_data: - process M_DATA message
 * @q: upper write queue
 * @mp: the M_DATA message
 */
static fastcall int
sl_w_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	int err = 0;

	if (likely(!!(sl = sl_acquire(q)))) {
		switch (sl_save_m_state(sl)) {
		case LMI_ENABLED:
			break;
		case LMI_UNUSABLE:
		default:
			err = m_error(sl, q, mp, EPROTO);
			goto done;
		}
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> M_DATA");
		err = sl_rx_data_req(sl, q, mp);
		if (err > 0)
			err = sl_passalong_req(sl, q, mp);
	      done:
		if (err)
			sl_restore_m_state(sl);
		sl_release(sl);
	} else
		err = -EAGAIN;
	return (err);
}

/**
 * sl_w_proto: - process M_(PC)PROTO message
 * @q: upper write queue
 * @mp: the M_(PC)PROTO message
 */
static noinline fastcall int
sl_w_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	int err = 0;

	if (likely(!!(sl = sl_acquire(q)))) {
		sl_long prim;

		sl_save_m_state(sl);

		if (!MBLKIN(mp, 0, sizeof(prim))) {
			err = lmi_error_reply(sl, q, mp, 0, LMI_BADPRIM);
			goto done;
		}
		if ((prim = *(typeof(prim) *) mp->b_rptr) == SL_PDU_REQ)
			mi_strlog(q, STRLOGDA, SL_TRACE, "-> SL_PDU_REQ");
		else
			mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", sl_primname(prim));

		switch (sl_get_m_state(sl)) {
		case LMI_UNUSABLE:
			err = lmi_error_reply(sl, q, mp, prim, LMI_FATALERR);
			goto done;
		case LMI_ATTACH_PENDING:
		case LMI_ENABLE_PENDING:
		case LMI_DISABLE_PENDING:
		case LMI_DETACH_PENDING:
		default:
			err = lmi_error_reply(sl, q, mp, prim, LMI_OUTSTATE);
			goto done;
		case LMI_UNATTACHED:
		case LMI_DISABLED:
		case LMI_ENABLED:
			break;
		}

		switch (prim) {
		case LMI_INFO_REQ:
			err = lmi_rx_info_req(sl, q, mp);
			break;
		case LMI_ATTACH_REQ:
			err = lmi_rx_attach_req(sl, q, mp);
			break;
		case LMI_DETACH_REQ:
			err = lmi_rx_detach_req(sl, q, mp);
			break;
		case LMI_ENABLE_REQ:
			err = lmi_rx_enable_req(sl, q, mp);
			break;
		case LMI_DISABLE_REQ:
			err = lmi_rx_disable_req(sl, q, mp);
			break;
		case LMI_OPTMGMT_REQ:
			err = lmi_rx_optmgmt_req(sl, q, mp);
			break;
		default:
			if (sl_get_m_state(sl) != LMI_ENABLED) {
				err = lmi_error_reply(sl, q, mp, prim, LMI_OUTSTATE);
				goto done;
			}
			switch (prim) {
			case SL_PDU_REQ:
				err = sl_rx_pdu_req(sl, q, mp);
				break;
			case SL_EMERGENCY_REQ:
				err = sl_rx_emergency_req(sl, q, mp);
				break;
			case SL_EMERGENCY_CEASES_REQ:
				err = sl_rx_emergency_ceases_req(sl, q, mp);
				break;
			case SL_START_REQ:
				err = sl_rx_start_req(sl, q, mp);
				break;
			case SL_STOP_REQ:
				err = sl_rx_stop_req(sl, q, mp);
				break;
			case SL_RETRIEVE_BSNT_REQ:
				err = sl_rx_retrieve_bsnt_req(sl, q, mp);
				break;
			case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
				err = sl_rx_retrieval_request_and_fsnc_req(sl, q, mp);
				break;
			case SL_CLEAR_BUFFERS_REQ:
				err = sl_rx_clear_buffers_req(sl, q, mp);
				break;
			case SL_CLEAR_RTB_REQ:
				err = sl_rx_clear_rtb_req(sl, q, mp);
				break;
			case SL_CONTINUE_REQ:
				err = sl_rx_continue_req(sl, q, mp);
				break;
			case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
				err = sl_rx_local_processor_outage_req(sl, q, mp);
				break;
			case SL_RESUME_REQ:
				err = sl_rx_resume_req(sl, q, mp);
				break;
			case SL_CONGESTION_DISCARD_REQ:
				err = sl_rx_congestion_discard_req(sl, q, mp);
				break;
			case SL_CONGESTION_ACCEPT_REQ:
				err = sl_rx_congestion_accept_req(sl, q, mp);
				break;
			case SL_NO_CONGESTION_REQ:
				err = sl_rx_no_congestion_req(sl, q, mp);
				break;
			case SL_POWER_ON_REQ:
				err = sl_rx_power_on_req(sl, q, mp);
				break;
			case SL_OPTMGMT_REQ:
				err = sl_rx_optmgmt_req(sl, q, mp);
				break;
#if 0
			case SL_NOTIFY_REQ:
				err = sl_rx_notify_req(sl, q, mp);
				break;
#endif
			default:
				err = sl_rx_other_req(sl, q, mp);
				break;
			}
		}
	      done:
		if (err > 0)
			err = sl_passalong_req(sl, q, mp);
		if (err)
			sl_restore_m_state(sl);
		sl_release(sl);
	} else
		err = -EAGAIN;
	return (err);
}

/**
 * sl_w_flush: - process M_FLUSH message
 * @q: upper write queue
 * @mp: the M_FLUSH message
 *
 * FIXME: This function is not yet mp safe: need some mechanism to stop the
 * lower stream from being unlinked while the upper stream is flushing.
 *
 * Note that the upper read queue does not need to be flushed becaue we never
 * place messages on the upper read queue.
 */
static noinline fastcall int
sl_w_flush(queue_t *q, mblk_t *mp)
{
	struct sl *sl;

	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	read_lock(&sl_mux_lock);
	sl = SL_PRIV(q);
	if (sl->oq) {
		putnext(sl->oq, mp);
	} else {
		mp->b_rptr[0] &= ~FLUSHW;
		qreply(q, mp);
	}
	read_unlock(&sl_mux_lock);
	return (0);
}

static noinline fastcall int
sl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct sl *sl;
	int err = 0;

	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}

	if (likely(!!(sl = sl_acquire(q)))) {
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(%s)", sl_iocname(ioc->ioc_cmd));
		switch (_IOC_TYPE(ioc->ioc_cmd)) {
		case __SID:
			switch (_IOC_NR(ioc->ioc_cmd)) {
			case _IOC_NR(I_PLINK):
				err = sl_i_plink(sl, q, mp);
				break;
			case _IOC_NR(I_LINK):
				err = sl_i_link(sl, q, mp);
				break;
			case _IOC_NR(I_PUNLINK):
				err = sl_i_punlink(sl, q, mp);
				break;
			case _IOC_NR(I_UNLINK):
				err = sl_i_unlink(sl, q, mp);
				break;
			default:
				goto passalong;
			}
			if (err > 0) {
				mi_copy_done(q, mp, err);
				err = 0;
			}
			break;
		case LMI_IOC_MAGIC:
			if (_IOC_NR(ioc->ioc_cmd) != _IOC_NR(LMI_IOCCPASS))
				goto passalong;
			err = sl_ioctl_ioccpass(sl, q, mp);
			break;
		case SL_IOC_MAGIC:
			if (_IOC_NR(ioc->ioc_cmd) != _IOC_NR(SL_IOCCPASS))
				goto passalong;
			err = sl_ioctl_ioccpass(sl, q, mp);
			break;
		case SDT_IOC_MAGIC:
			if (_IOC_NR(ioc->ioc_cmd) != _IOC_NR(SDT_IOCCPASS))
				goto passalong;
			err = sl_ioctl_ioccpass(sl, q, mp);
			break;
		case SDL_IOC_MAGIC:
			if (_IOC_NR(ioc->ioc_cmd) != _IOC_NR(SDL_IOCCPASS))
				goto passalong;
			err = sl_ioctl_ioccpass(sl, q, mp);
			break;
		case SLM_IOC_MAGIC:
			err = slm_ioctl(sl, q, mp);
			break;
		default:
		      passalong:
			read_lock(&sl_mux_lock);
			if (sl->oq)
				putnext(sl->oq, mp);
			else
				err = EINVAL;
			read_unlock(&sl_mux_lock);
			if (err) {
				mi_copy_done(q, mp, err);
				err = 0;
			}
			break;
		}
		sl_release(sl);
	} else
		err = -EAGAIN;
	return (err);
}
static noinline fastcall int
sl_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct sl *lm;
	mblk_t *dp;
	int err = 0;

	if (likely(!!(lm = sl_acquire(q)))) {
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(%s)", sl_iocname(cp->cp_cmd));
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case SLM_IOC_MAGIC:
			switch (mi_copy_state(q, mp, &dp)) {
			case -1:
				break;
			case MI_COPY_CASE(MI_COPY_IN, 1):
				err = slm_copyin(lm, q, mp);
				break;
			case MI_COPY_CASE(MI_COPY_OUT, 1):
				err = slm_copyout(lm, q, mp);
				break;
			default:
				mi_copy_done(q, mp, EPROTO);
				break;
			}
			break;
		case LMI_IOC_MAGIC:
			if (_IOC_NR(cp->cp_cmd) != _IOC_NR(LMI_IOCCPASS))
				goto passalong;
			err = sl_iocdata_ioccpass(lm, q, mp);
			break;
		case SL_IOC_MAGIC:
			if (_IOC_NR(cp->cp_cmd) != _IOC_NR(SL_IOCCPASS))
				goto passalong;
			err = sl_iocdata_ioccpass(lm, q, mp);
			break;
		case SDT_IOC_MAGIC:
			if (_IOC_NR(cp->cp_cmd) != _IOC_NR(SDT_IOCCPASS))
				goto passalong;
			err = sl_iocdata_ioccpass(lm, q, mp);
			break;
		case SDL_IOC_MAGIC:
			if (_IOC_NR(cp->cp_cmd) != _IOC_NR(SDL_IOCCPASS))
				goto passalong;
			err = sl_iocdata_ioccpass(lm, q, mp);
			break;
		default:
		      passalong:
			read_lock(&sl_mux_lock);
			if (lm->oq)
				putnext(lm->oq, mp);
			else
				err = EINVAL;
			read_unlock(&sl_mux_lock);
			if (err) {
				mi_copy_done(q, mp, err);
				err = 0;
			}
			break;
		}
		sl_release(lm);
	} else
		err = -EAGAIN;
	return (err);
}

/**
 * sl_w_other: other STREAMS message on write queue
 * @q: upper write queue
 * @mp: the STREAMS message
 *
 * Simply pass unrecognized STREAMS messages to the lower Stream if one
 * exists, otherwise, discard the message.
 */
static noinline fastcall int
sl_w_other(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	int err = 0;

	if (likely(!!(sl = sl_acquire(q)))) {
		read_lock(&sl_mux_lock);
		if (sl->oq) {
			if (DB_TYPE(mp) >= QPCTL || bcanputnext(sl->oq, mp->b_band))
				putnext(sl->oq, mp);
			else
				err = -EBUSY;
		} else
			freemsg(mp);
		read_unlock(&sl_mux_lock);
		sl_release(sl);
	} else
		err = -EAGAIN;
	return (err);
}
static noinline fastcall int
sl_w_data_slow(queue_t *q, mblk_t *mp)
{
	return sl_w_data(q, mp);
}
static fastcall int
sl_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sl_w_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_w_proto(q, mp);
	case M_FLUSH:
		return sl_w_flush(q, mp);
	case M_IOCTL:
		return sl_w_ioctl(q, mp);
	case M_IOCDATA:
		return sl_w_iocdata(q, mp);
	default:
		return sl_w_other(q, mp);
	}
}

/*
 *  ===========================================================================
 *
 *  UPPER MULTIPLEX MESSAGE HANDLING
 *
 *  ===========================================================================
 *  These are STREAMS messages received from below the lower multipex on the
 *  lower read queue.
 */

static noinline fastcall int
sl_passalong_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *mon;
	int err = 0;

	read_lock(&sl_mux_lock);
	if (sl->oq && DB_TYPE(mp) < QPCTL && !bcanputnext(sl->oq, mp->b_band)) {
		read_unlock(&sl_mux_lock);
		return (-EBUSY);
	}
	if ((mon = sl->mon.mon)) {
		mblk_t *bp, *dp = NULL;
		struct slmux_mon *p;

		if (!canputnext(mon->rq)) {
			read_unlock(&sl_mux_lock);
			return (-EBUSY);
		}
		if ((bp = mi_allocb(q, sizeof(*p) + mp->b_wptr - mp->b_rptr, BPRI_MED)) == NULL) {
			read_unlock(&sl_mux_lock);
			return (-ENOBUFS);
		}
		if (mp->b_cont && (dp = dupmsg(mp->b_cont)) == NULL) {
			read_unlock(&sl_mux_lock);
			freeb(bp);
			mi_bufcall(q, 0, BPRI_MED);
			return (-ENOBUFS);
		}
		bp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) bp->b_rptr;
		p->mon_ppa = sl->ppa;
		p->mon_dir = 0;	/* read */
		p->mon_msg_type = DB_TYPE(mp);
		p->mon_msg_band = mp->b_band;
		p->mon_msg_flags = mp->b_flag;
		bp->b_wptr += sizeof(*p);
		bcopy(mp->b_rptr, bp->b_wptr, mp->b_wptr - mp->b_rptr);
		bp->b_wptr += mp->b_wptr - mp->b_rptr;
		bp->b_cont = dp;
		putnext(mon->rq, mp);
	}
	if (sl->oq)
		putnext(sl->oq, mp);
	else
		freemsg(mp);
	read_unlock(&sl_mux_lock);
	return (err);
}

/**
 * sl_r_data: - process M_DATA message
 * @q: lower read queue
 * @mp: the M_DATA message
 */
static fastcall int
sl_r_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	int err = 0;

	read_lock(&sl_mux_lock);
	sl = sl_acquire(q);
	read_unlock(&sl_mux_lock);

	if (likely(!!sl)) {
		sl_save_m_state(sl);

		err = sl_rx_data_ind(sl, q, mp);
		if (err > 0)
			err = sl_passalong_ind(sl, q, mp);
		if (err)
			sl_restore_m_state(sl);
		sl_release(sl);
	} else
		err = -EAGAIN;
	return (err);
}

/**
 * sl_r_proto: - process M_(PC)PROTO message
 * @q: lower read queue
 * @mp: the M_(PC)PROTO message
 *
 * These messages are simply passed to the upper stream with flow control or
 * discarded.  No messages from the lower stream are currently intercepted.
 */
static noinline fastcall int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	int err = 0;

	read_lock(&sl_mux_lock);
	sl = sl_acquire(q);
	read_unlock(&sl_mux_lock);

	if (likely(!!sl)) {
		sl_long prim;

		sl_save_m_state(sl);

		if (!MBLKIN(mp, 0, sizeof(prim))) {
			freemsg(mp);
			goto done;
		}
		if ((prim = *(typeof(prim) *) mp->b_rptr) == SL_PDU_IND)
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- SL_PDU_IND");
		else
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", sl_primname(prim));

		switch (prim) {
		case LMI_INFO_ACK:
			err = lmi_rx_info_ack(sl, q, mp);
			break;
		case LMI_OK_ACK:
			err = lmi_rx_ok_ack(sl, q, mp);
			break;
		case LMI_ERROR_ACK:
			err = lmi_rx_error_ack(sl, q, mp);
			break;
		case LMI_ENABLE_CON:
			err = lmi_rx_enable_con(sl, q, mp);
			break;
		case LMI_DISABLE_CON:
			err = lmi_rx_disable_con(sl, q, mp);
			break;
		case LMI_OPTMGMT_ACK:
			err = lmi_rx_optmgmt_ack(sl, q, mp);
			break;
		case LMI_ERROR_IND:
			err = lmi_rx_error_ind(sl, q, mp);
			break;
		case LMI_STATS_IND:
			err = lmi_rx_stats_ind(sl, q, mp);
			break;
		case LMI_EVENT_IND:
			err = lmi_rx_event_ind(sl, q, mp);
			break;
		case SL_PDU_IND:
			err = sl_rx_pdu_ind(sl, q, mp);
			break;
		case SL_LINK_CONGESTED_IND:
			err = sl_rx_link_congested_ind(sl, q, mp);
			break;
		case SL_LINK_CONGESTION_CEASED_IND:
			err = sl_rx_link_congestion_ceased_ind(sl, q, mp);
			break;
		case SL_RETRIEVED_MESSAGE_IND:
			err = sl_rx_retrieved_message_ind(sl, q, mp);
			break;
		case SL_RETRIEVAL_COMPLETE_IND:
			err = sl_rx_retrieval_complete_ind(sl, q, mp);
			break;
		case SL_RB_CLEARED_IND:
			err = sl_rx_rb_cleared_ind(sl, q, mp);
			break;
		case SL_BSNT_IND:
			err = sl_rx_bsnt_ind(sl, q, mp);
			break;
		case SL_IN_SERVICE_IND:
			err = sl_rx_in_service_ind(sl, q, mp);
			break;
		case SL_OUT_OF_SERVICE_IND:
			err = sl_rx_out_of_service_ind(sl, q, mp);
			break;
		case SL_REMOTE_PROCESSOR_OUTAGE_IND:
			err = sl_rx_remote_processor_outage_ind(sl, q, mp);
			break;
		case SL_REMOTE_PROCESSOR_RECOVERED_IND:
			err = sl_rx_remote_processor_recovered_ind(sl, q, mp);
			break;
		case SL_RTB_CLEARED_IND:
			err = sl_rx_rtb_cleared_ind(sl, q, mp);
			break;
		case SL_RETRIEVAL_NOT_POSSIBLE_IND:
			err = sl_rx_retrieval_not_possible_ind(sl, q, mp);
			break;
		case SL_BSNT_NOT_RETRIEVABLE_IND:
			err = sl_rx_bsnt_not_retrievable_ind(sl, q, mp);
			break;
		case SL_OPTMGMT_ACK:
			err = sl_rx_optmgmt_ack(sl, q, mp);
			break;
		case SL_NOTIFY_IND:
			err = sl_rx_notify_ind(sl, q, mp);
			break;
		case SL_LOCAL_PROCESSOR_OUTAGE_IND:
			err = sl_rx_local_processor_outage_ind(sl, q, mp);
			break;
		case SL_LOCAL_PROCESSOR_RECOVERED_IND:
			err = sl_rx_local_processor_recovered_ind(sl, q, mp);
			break;
		default:
			err = sl_rx_other_ind(sl, q, mp);
			break;
		}
		if (err > 0)
			err = sl_passalong_ind(sl, q, mp);
	      done:
		if (err)
			sl_restore_m_state(sl);
		read_unlock(&sl_mux_lock);
		sl_release(sl);
	} else
		err = -EAGAIN;
	return (err);
}

/**
 * sl_r_iocack: - process M_IOC(ACK|NAK) or M_COPY(IN|OUT) message
 * @q: lower read queue
 * @mp: the M_IOC(ACK|NAK) or M_COPY(IN|OUT) message
 *
 * These are typically passed on to the associated upper stream or discarded.  The only exception is
 * when layer managemet stream for this lower stream is processing a pass-along input-output
 * control.  Then, provided that the ioctl id matches that of the pass-along operation, the message
 * is passed to the layer management stream instead of the upper stream.  This can be checked by
 * checking the ioc_id of the message.
 */
static noinline fastcall int
sl_r_iocack(queue_t *q, mblk_t *mp)
{
	struct sl *sl, *lm;
	int err = 0;

	switch (DB_TYPE(mp)) {
	case M_IOCACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- M_IOCACK");
		break;
	case M_IOCNAK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- M_IOCNAK");
		break;
	case M_COPYIN:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- M_COPYIN");
		break;
	case M_COPYOUT:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- M_COPYOUT");
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- M_????(%d)", (int) DB_TYPE(mp));
		break;
	}

	read_lock(&sl_mux_lock);
	if ((sl = SL_PRIV(q)) != NULL) {
		struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

		if ((lm = sl->lm.lm) && lm->ioc.id == ioc->ioc_id)
			putnext(lm->rq, mp);
		else if (sl->oq)
			putnext(sl->oq, mp);
		else
			freemsg(mp);
	} else
		freemsg(mp);
	read_unlock(&sl_mux_lock);
	return (err);
}

/**
 * sl_r_flush: - process M_FLUSH message
 * @q: lower read queue
 * @mp: the M_FLUSH message
 *
 * FIXME: This function is not yet mp safe: need some mechanism to stop the
 * upper stream from being closed while the lower stream is flushing.
 *
 * Note that the lower write queue does not need to be flushed because we
 * never place messages on the lower write queue.
 */
static noinline fastcall int
sl_r_flush(queue_t *q, mblk_t *mp)
{
	struct sl *sl;

	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	read_lock(&sl_mux_lock);
	sl = SL_PRIV(q);
	if (sl->oq) {
		putnext(sl->oq, mp);
	} else {
		mp->b_rptr[0] &= ~FLUSHR;
		qreply(q, mp);
	}
	read_unlock(&sl_mux_lock);
	return (0);
}

/**
 * sl_r_other: other sTREAMS message on read queue
 * @q: lower read queue
 * @mp: the STREAMS message
 *
 * Simply pass unrecognized STREAMS messages to the upper Stream if one
 * exists, otherwise, discard the message.
 */
static noinline fastcall int
sl_r_other(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	int err = 0;

	read_lock(&sl_mux_lock);
	sl = sl_acquire(q);
	read_unlock(&sl_mux_lock);

	if (likely(!!sl)) {
		read_lock(&sl_mux_lock);
		if (sl->oq) {
			if (DB_TYPE(mp) >= QPCTL || bcanputnext(sl->oq, mp->b_band))
				putnext(sl->oq, mp);
			else
				err = -EBUSY;
		} else
			freemsg(mp);
		read_unlock(&sl_mux_lock);
		sl_release(sl);
	} else
		err = -EAGAIN;
	return (err);
}
static noinline fastcall int
sl_r_data_slow(queue_t *q, mblk_t *mp)
{
	return sl_r_data(q, mp);
}
static fastcall int
sl_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sl_r_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return sl_r_iocack(q, mp);
	case M_FLUSH:
		return sl_r_flush(q, mp);
	default:
		return sl_r_other(q, mp);
	}
}

/*
 *  ===========================================================================
 *
 *  PUT AND SERVICE PROCEDURES
 *
 *  ===========================================================================
 */

/**
 * sl_w_msg: - process upper write message
 * @q: upper write queue
 * @mp: the message
 */
static fastcall int
sl_w_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return sl_w_data(q, mp);
	return sl_w_msg_slow(q, mp);
}

/**
 * sl_r_msg: - process lower read message
 * @q: lower read queue
 * @mp: the message
 */
static fastcall int
sl_r_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return sl_r_data(q, mp);
	return sl_r_msg_slow(q, mp);
}

/**
 * sl_wput: - upper write put procedure
 * @q: queue to put
 * @mp: message to put
 *
 * This is a canonical upper write put procedure.
 */
static streamscall int
sl_wput(queue_t *q, mblk_t *mp)
{
	if (((DB_TYPE(mp) < QPCTL) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || sl_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * sl_wsrv: - upper write service procedure
 * @q: queue to service
 *
 * This is a canonical service procedure.  Messages are placed on the upper
 * write queue when they have arrived from above and cannot be serviced
 * immediately.
 */
static streamscall int
sl_wsrv(queue_t *q)
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
 * sl_rsrv: - upper read service procedure
 * @q: queue to service
 *
 * The upper read service procedure is only used for back-enabling.  When this
 * queue service procedure runs, it is only because a flow controlled upper
 * queue has abated. Find the feeding lower stream and enable its read service
 * procedure. 
 */
static streamscall int
sl_rsrv(queue_t *q)
{
	struct sl *sl;

	if ((sl = sl_acquire(q)) != NULL) {
		read_lock(&sl_mux_lock);
		if (sl->oq)
			qenable(sl->oq);
		read_unlock(&sl_mux_lock);
		sl_release(sl);
	}
	return (0);
}

/**
 * sl_rput: - upper read put procedure
 * @q: queue to put
 * @mp: message to put
 *
 * The upper read put procedure is never used: messages are always placed on
 * the queue above the upper read queue.  It is an error for this function to
 * be invoked.
 */
static streamscall int
sl_rput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGNO, SL_ERROR, "%s: should not be called", __FUNCTION__);
	putq(q, mp);
	return (0);
}

/**
 * sl_muxwput: - lower write put procedure
 * @q: queue to put
 * @mp: message to put
 *
 * The lower write put procedure is never used: messages are always placed on
 * the queue below the lower write queue.  It is an error for this function to
 * be invoked.
 */
static streamscall int
sl_muxwput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGNO, SL_ERROR, "%s: should not be called", __FUNCTION__);
	putq(q, mp);
	return (0);
}

/**
 * sl_muxwsrv: - lower write service procedure
 * @q: queue to service
 *
 * The lower write service procedure is only used for back-enabling. When this
 * queue service procedure runs, it is only because a flow controlled lower
 * queue has abated.  Find the feeding upper stream and enable its write
 * service procedure.
 */
static streamscall int
sl_muxwsrv(queue_t *q)
{
	struct sl *sl;

	if ((sl = sl_acquire(q)) != NULL) {
		read_lock(&sl_mux_lock);
		if (sl->oq)
			qenable(sl->oq);
		read_unlock(&sl_mux_lock);
		sl_release(sl);
	}
	return (0);
}

/**
 * sl_muxrsrv: - lower read service procedure
 * @q: queue to service
 *
 * This is a canonical service procedure. Messages are placed on the lower
 * read queue when they have arrived from below and cannot be serviced
 * immediately.
 */
static streamscall int
sl_muxrsrv(queue_t *q)
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
 * sl_muxrput: - lower read put procedure
 * @q: queue to put
 * @mp: message to put
 *
 * This is a canonical lower read put procedure with interrupt protection.
 *
 * We check whether we are in an interrupt to keep from running sl_r_msg() in interrupt context.
 * This protects the entire driver from interrupt context processing and makes all locks simple spin
 * locks that do not need BH or IRQ protection.  This runs much faster on UP.
 */
static streamscall int
sl_muxrput(queue_t *q, mblk_t *mp)
{
	if (in_interrupt() || ((DB_TYPE(mp) < QPCTL) && (q->q_first || (q->q_flag & QSVCBUSY))) || sl_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  ===========================================================================
 *
 *  OPEN AND CLOSE ROUTINES
 *
 *  ===========================================================================
 */

#define SL_C_MINOR	0	/* "sl" device */
#define SL_A_MINOR	1	/* "sl-admin" device */
#define SL_S_MINOR	2	/* "sl-stats" device */
#define SL_E_MINOR	3	/* "sl-events" device */
#define SL_M_MINOR	4	/* "sl-mon" device */
#define SL_F_MINOR	5	/* first free minor */

/**
 * sl_qopen: - open the STREAMS driver
 * @q: read queue of freshly allocated queue pair
 * @devp: pointer to device number being opened
 * @oflags: flags to open(2s)
 * @sflag: STREAMS flag
 * @crp: credentials of opening process
 */
static int
sl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	minor_t cminor = getminor(*devp);
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (cminor >= SL_F_MINOR) {
		sflag = CLONEOPEN;
		cminor = 0;
	}
	if (sflag == CLONEOPEN)
		*devp = makedevice(getmajor(*devp), SL_F_MINOR);
	if ((err = mi_open_comm_cache(&sl_opens, sl_priv_cachep, q, devp, oflags, sflag, crp)))
		return (err);
	sl_opens_count++;
	sl_init(SL_PRIV(q), q, (int) cminor, crp);
	return (0);
}

/**
 * sl_qclose: - close the STREAMS driver
 * @q: read queue of closing queue pair
 * @oflags: flags to open(2s) call
 * @crp: credentials of closing process
 */
static int
sl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	unsigned long flags;
	struct sl *sl;
	int state;

	while ((sl = (struct sl *) mi_sleeplock(q)) == NULL) ;
	write_lock_irqsave(&sl_mux_lock, flags);
	/* We have a list of streams that we are managing: this must be the layer management
	   control stream, because temporarily linked streams would have closed already, so these
	   are only permanent links. */
	while (sl->lm.lm) {
		sl_unlink_lm(sl->lm.lm);
	}
	/* We have a list of stream that we are monitoring: this must be the layer management
	   control stream, because temporarily linked streams would have closed already, so these
	   are only permanent links. */
	while (sl->mon.mon) {
		sl_unlink_mon(sl->mon.mon);
	}
	write_unlock_irqrestore(&sl_mux_lock, flags);
	read_lock(&sl_mux_lock);
	state = sl_get_m_state(sl);
	if (sl->oq && (state == LMI_ENABLED || state == LMI_ENABLE_PENDING)) {
		lmi_disable_req_t *p;
		mblk_t *mp;

		while ((mp = allocb(sizeof(*p), BPRI_WAITOK)) == NULL) ;
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		sl_set_m_state(sl, LMI_DISABLE_PENDING);
		putnext(sl->oq, mp);
	}
	read_unlock(&sl_mux_lock);
	write_lock_irqsave(&sl_mux_lock, flags);
	sl_unlink_sl(sl);
	write_unlock_irqrestore(&sl_mux_lock, flags);
	mi_unlock((caddr_t) sl);
	qprocsoff(q);
	mi_close_comm(&sl_opens, q);
	sl_opens_count--;
	return (0);
}

/*
 *  ===========================================================================
 *
 *  INITIALIZATION
 *
 *  ===========================================================================
 */

static struct qinit sl_rinit = {
	.qi_putp = sl_rput,	/* Read put (message from below) */
	.qi_srvp = sl_rsrv,	/* Read queue service */
	.qi_qopen = sl_qopen,	/* Each open */
	.qi_qclose = sl_qclose,	/* Last close */
	.qi_minfo = &sl_minfo,	/* Information */
	.qi_mstat = &sl_rstat,	/* Statistics */
};

static struct qinit sl_winit = {
	.qi_putp = sl_wput,	/* Write put (message from above) */
	.qi_srvp = sl_wsrv,	/* Write queue service */
	.qi_minfo = &sl_minfo,	/* Information */
	.qi_mstat = &sl_wstat,	/* Statistics */
};

static struct qinit sl_muxrinit = {
	.qi_putp = sl_muxrput,	/* Read put (message from below) */
	.qi_srvp = sl_muxrsrv,	/* Read queue service */
	.qi_minfo = &sl_minfo,	/* Information */
	.qi_mstat = &sl_muxrstat,	/* Statistics */
};

static struct qinit sl_muxwinit = {
	.qi_putp = sl_muxwput,	/* Write put (message from above) */
	.qi_srvp = sl_muxwsrv,	/* Write queue service */
	.qi_minfo = &sl_minfo,	/* Information */
	.qi_mstat = &sl_muxwstat,	/* Statistics */
};

static struct streamtab sl_muxinfo = {
	.st_rdinit = &sl_rinit,	/* Upper read queue */
	.st_wrinit = &sl_winit,	/* Upper write queue */
	.st_muxrinit = &sl_muxrinit,	/* Lower read queue */
	.st_muxwinit = &sl_muxwinit,	/* Lower write queue */
};

#ifdef LINUX

unsigned short modid = DRV_ID;
unsigned short major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(modid, "h");
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
module_param(major, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for SL-MUX driver.  (0 for allocation.)");
MODULE_PARM_DESC(major, "Major Num for SL-MUX driver.  (0 for allocation.)");

static struct cdevsw sl_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sl_muxinfo,
	.d_flag = D_MP,
	.d_kmod = THIS_MODULE,
};

static struct devnode sl_c_node = {
	.n_name = "sl",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode sl_a_node = {
	.n_name = "sl-admin",
	.n_flag = 0,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode sl_s_node = {
	.n_name = "sl-stats",
	.n_flag = 0,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode sl_e_node = {
	.n_name = "sl-events",
	.n_flag = 0,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode sl_m_node = {
	.n_name = "sl-mon",
	.n_flag = 0,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
static __init int
sl_muxinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if (!(sl_priv_cachep =
	      kmem_cache_create("sl_priv_cachep", mi_open_size(sizeof(struct sl)), 0,
				SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_WARN, "%s: could not create sl_priv_cachep\n", DRV_NAME);
		err = -ENOMEM;
		goto nocache;
	}
	if ((err = register_strdrv(&sl_cdev)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver, err = %d\n", DRV_NAME, err);
		goto nostrdrv;
	}
	if ((err = register_strnod(&sl_cdev, &sl_c_node, SL_C_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_C_MINOR, err);
		goto nocnode;
	}
	if ((err = register_strnod(&sl_cdev, &sl_a_node, SL_A_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_A_MINOR, err);
		goto noanode;
	}
	if ((err = register_strnod(&sl_cdev, &sl_s_node, SL_S_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_S_MINOR, err);
		goto nosnode;
	}
	if ((err = register_strnod(&sl_cdev, &sl_e_node, SL_E_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_E_MINOR, err);
		goto noenode;
	}
	if ((err = register_strnod(&sl_cdev, &sl_m_node, SL_M_MINOR)) < 0) {
		cmn_err(CE_WARN, "%s: could not register minor %d, err = %d\n", DRV_NAME,
			(int) SL_M_MINOR, err);
		goto nomnode;
	}
	if (major == 0)
		major = err;
	return (0);
	unregister_strnod(&sl_cdev, SL_M_MINOR);
      nomnode:
	unregister_strnod(&sl_cdev, SL_E_MINOR);
      noenode:
	unregister_strnod(&sl_cdev, SL_S_MINOR);
      nosnode:
	unregister_strnod(&sl_cdev, SL_A_MINOR);
      noanode:
	unregister_strnod(&sl_cdev, SL_C_MINOR);
      nocnode:
	unregister_strdrv(&sl_cdev);
      nostrdrv:
	kmem_cache_destroy(sl_priv_cachep);
      nocache:
	return (err);
}

static __exit void
sl_muxexit(void)
{
	int err;

	if ((err = unregister_strnod(&sl_cdev, SL_M_MINOR)))
		cmn_err(CE_WARN, "%s: could not unregister node %d, err = %d", DRV_NAME,
			(int) SL_M_MINOR, err);
	if ((err = unregister_strnod(&sl_cdev, SL_E_MINOR)))
		cmn_err(CE_WARN, "%s: could not unregister node %d, err = %d", DRV_NAME,
			(int) SL_E_MINOR, err);
	if ((err = unregister_strnod(&sl_cdev, SL_S_MINOR)))
		cmn_err(CE_WARN, "%s: could not unregister node %d, err = %d", DRV_NAME,
			(int) SL_S_MINOR, err);
	if ((err = unregister_strnod(&sl_cdev, SL_A_MINOR)))
		cmn_err(CE_WARN, "%s: could not unregister node %d, err = %d", DRV_NAME,
			(int) SL_A_MINOR, err);
	if ((err = unregister_strnod(&sl_cdev, SL_C_MINOR)))
		cmn_err(CE_WARN, "%s: could not unregister node %d, err = %d", DRV_NAME,
			(int) SL_C_MINOR, err);
	if ((err = unregister_strdrv(&sl_cdev)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d", DRV_NAME, err);
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
	if (kmem_cache_destroy(sl_priv_cachep))
		cmn_err(CE_WARN, "%s: could not destroy sl_priv_cachep\n", DRV_NAME);
#else
	kmem_cache_destroy(sl_priv_cachep);
#endif
	return;
}

module_init(sl_muxinit);
module_exit(sl_muxexit);

#endif				/* LINUX */

/*
 * vim: tw=100
 */
