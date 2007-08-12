/*****************************************************************************

 @(#) $RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/08/12 16:15:36 $

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

 Last Modified $Date: 2007/08/12 16:15:36 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m2ua_as.c,v $
 Revision 0.9.2.11  2007/08/12 16:15:36  brian
 -

 Revision 0.9.2.10  2007/08/03 13:34:45  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.9  2007/07/14 01:33:44  brian
 - make license explicit, add documentation

 Revision 0.9.2.8  2007/05/17 22:55:37  brian
 - use mi_timer requeue to requeue mi timers

 Revision 0.9.2.7  2007/03/25 18:59:07  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.6  2007/03/05 23:01:41  brian
 - checking in release changes

 Revision 0.9.2.5  2007/01/15 12:16:28  brian
 - updated archive sizes, new development work

 Revision 0.9.2.4  2006/12/28 05:19:34  brian
 - minor changes

 Revision 0.9.2.3  2006/12/23 19:54:57  brian
 - void return

 Revision 0.9.2.2  2006/12/23 13:06:56  brian
 - manual page and other package updates for release

 Revision 0.9.2.1  2006/11/30 13:17:57  brian
 - added files from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/08/12 16:15:36 $"

static char const ident[] =
    "$RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/08/12 16:15:36 $";

/*
 *  This is the AS side of M2UA implemented as a pushable module that pushes over an SCTP NPI
 *  stream.
 *
 *  The SCTP NPI stream can be a freshly opened stream (i.e. in the NS_UNBND state), or can be bound
 *  by the opener before pushing the M2UA-AS module, or can be connected by the opener before
 *  pushing the M2UA-AS module.  When the M2UA-AS module is pushed, it interrogates the state of the
 *  SCTP NPI stream beneath it and flushes the read queue (so that no messages get past the module).
 *
 *  When an attach is performed, the SCTP stream will be bound (if it is not already bound).  If the
 *  SCTP stream is already connected, the attach operation initiates the ASP Up procedure with the
 *  attached ASPID.
 *
 *  When an enable is performed, the SCTP stream will be connected (if it is not already connected).
 *  If the SCTP stream is already connected, and the ASP Up operation has completed, an ASP Active
 *  for the enabled IID is initiated.
 *
 *  Once the interface is enabled, the M2UA-AS module interrogates the state of the singalling link
 *  for the attached IID by using the MAUP state query.
 *
 *  At this point, SL-primitives can be issued by the SL User that will be translated into M2UA
 *  messages for the attached IID.  M2UA messages received for the attached IID will be translated
 *  into SL-primitives and set upstream.
 */

#ifndef HAVE_KTYPE_BOOL
#include <stdbool.h>
#endif

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#define _DEBUG 1
//#undef _DEBUG

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#undef mi_timer
#define mi_timer mi_timer_MAC

#include <linux/socket.h>
#include <net/ip.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#include <sys/ua_ioctl.h>
#include <sys/m2ua_ioctl.h>

#define SLLOGST	    1		/* log SL state transitions */
#define SLLOGTO	    2		/* log SL timeouts */
#define SLLOGRX	    3		/* log SL primitives received */
#define SLLOGTX	    4		/* log SL primitives issued */
#define SLLOGTE	    5		/* log SL timer events */
#define SLLOGDA	    6		/* log SL data */

/* ======================= */

#define M2UA_AS_DESCRIP		"M2UA/SCTP SIGNALLING LINK (SL) STREAMS MODULE."
#define M2UA_AS_REVISION	"OpenSS7 $RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2007/08/12 16:15:36 $"
#define M2UA_AS_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define M2UA_AS_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define M2UA_AS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define M2UA_AS_LICENSE		"GPL v2"
#define M2UA_AS_BANNER		M2UA_AS_DESCRIP		"\n" \
				M2UA_AS_REVISION	"\n" \
				M2UA_AS_COPYRIGHT	"\n" \
				M2UA_AS_DEVICE		"\n" \
				M2UA_AS_CONTACT		"\n"
#define M2UA_AS_SPLASH		M2UA_AS_DEVICE		" - " \
				M2UA_AS_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M2UA_AS_CONTACT);
MODULE_DESCRIPTION(M2UA_AS_DESCRIP);
MODULE_SUPPORTED_DEVICE(M2UA_AS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M2UA_AS_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-m2ua_as");
#endif
#endif				/* LINUX */

#ifdef LFS
#define M2UA_AS_MOD_ID		CONFIG_STREAMS_M2UA_AS_MODID
#define M2UA_AS_MOD_NAME	CONFIG_STREAMS_M2UA_AS_NAME
#endif

#define MOD_ID	    M2UA_AS_MOD_ID
#define MOD_NAME    M2UA_AS_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH  M2UA_AS_BANNER
#else				/* MODULE */
#define MOD_SPLASH  M2UA_AS_SPLASH
#endif				/* MODULE */

static struct module_info sl_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat sl_mstat __attribute__((aligned(SMP_CACHE_BYTES)));

/*
 *  M2UA-AS Private Structure
 */
struct sl {
	STR_DECLARATION (struct sl);	/* stream declaration */
	int mid;			/* module id */
	int sid;			/* stream id */
	struct task_struct *owner;
	queue_t *waitq;
	int u_state;
	int n_state;
	uint32_t cong;
	uint32_t disc;
	mblk_t *aspup_tack;
	mblk_t *aspac_tack;
	mblk_t *aspdn_tack;
	mblk_t *aspia_tack;
	mblk_t *hbeat_tack;
	int audit;
	uint32_t tm;			/* traffic mode */
	uint32_t iid;			/* interface identifier */
	uint32_t aspid;			/* ASP identifier */
	int coninds;			/* npi connection indications */
	lmi_option_t option;		/* protocol and variant options */
	struct {
		uint flags;		/* overall provider flags */
		uint state;		/* overall provider state */
		sl_timers_t timers;	/* SL protocol timers */
		sl_config_t config;	/* SL configuration options */
		sl_statem_t statem;	/* SL state machine variables */
		sl_notify_t notify;	/* SL notification options */
		sl_stats_t stats;	/* SL statistics */
		sl_stats_t stamp;	/* SL statistics timestamps */
		sl_stats_t statsp;	/* SL statistics periods */
	} sl;
	struct {
		sdt_timers_t timers;	/* SDT timers */
		sdt_config_t config;	/* SDT configuration options */
	} sdt;
	struct {
		sdl_timers_t timers;	/* SDL timers */
		sdl_config_t config;	/* SDL configuration options */
	} sdl;
	struct {
		N_info_ack_t n;		/* NPI information */
		lmi_info_ack_t lm;	/* LMI information */
	} info;
	size_t loc_len;			/* local address length */
	size_t rem_len;			/* remote address length */
	struct sockaddr_in loc[8];	/* up to 8 local addresses */
	struct sockaddr_in rem[8];	/* up to 8 remote addresses */
};

#define SL_PRIV(__q) ((struct sl *)(__q)->q_ptr)

#if 0
#define NSF_UNBND	(1<<NS_UNBND)
#define NSF_WACK_BREQ	(1<<NS_WACK_BREQ)
#define NSF_WACK_UREQ	(1<<NS_WACK_UREQ)
#define NSF_IDLE	(1<<NS_IDLE)
#define NSF_WACK_OPTREQ	(1<<NS_WACK_OPTREQ)
#define NSF_WACK_RRES	(1<<NS_WACK_RRES)
#define NSF_WCON_CREQ	(1<<NS_WCON_CREQ)
#define NSF_WRES_CIND	(1<<NS_WRES_CIND)
#define NSF_WACK_CRES	(1<<NS_WACK_CRES)
#define NSF_DATA_XFER	(1<<NS_DATA_XFER)
#define NSF_WCON_RREQ	(1<<NS_WCON_RREQ)
#define NSF_WRES_RIND	(1<<NS_WRES_RIND)
#define NSF_WACK_DREQ6	(1<<NS_WACK_DREQ6)
#define NSF_WACK_DREQ7	(1<<NS_WACK_DREQ7)
#define NSF_WACK_DREQ9	(1<<NS_WACK_DREQ9)
#define NSF_WACK_DREQ10	(1<<NS_WACK_DREQ10)
#define NSF_WACK_DREQ11	(1<<NS_WACK_DREQ11)
#endif

const char *
sl_n_state_name(int state)
{
	switch (state) {
	case NS_UNBND:		/* NS user not bound to network address */
		return ("NS_UNBND");
	case NS_WACK_BREQ:	/* Awaiting acknowledgement of N_BIND_REQ */
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:	/* Pending acknowledgement for N_UNBIND_REQ */
		return ("NS_WACK_UREQ");
	case NS_IDLE:		/* Idle, no connection */
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:	/* Pending acknowledgement of N_OPTMGMT_REQ */
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:	/* Pending acknowledgement of N_RESET_RES */
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:	/* Pending confirmation of N_CONN_REQ */
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:	/* Pending response of N_CONN_REQ */
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:	/* Pending acknowledgement of N_CONN_RES */
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:	/* Connection-mode data transfer */
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:	/* Pending confirmation of N_RESET_REQ */
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:	/* Pending response of N_RESET_IND */
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:	/* Waiting ack of N_DISCON_REQ */
		return ("NS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}
static int
sl_set_n_state(struct sl *sl, int newstate)
{
	int oldstate = sl->info.n.CURRENT_state;

	strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "%s <- %s", sl_n_state_name(newstate),
	       sl_n_state_name(oldstate));
	return ((sl->info.n.CURRENT_state = newstate));
}
static inline int
sl_get_n_state(struct sl *sl)
{
	return (sl->info.n.CURRENT_state);
}
static inline int
sl_get_n_statef(struct sl *sl)
{
	return ((1 << sl_get_n_state(sl)));
}
static inline int
sl_chk_n_state(struct sl *sl, int mask)
{
	return (sl_get_n_statef(sl) & mask);
}
static inline int
sl_not_n_state(struct sl *sl, int mask)
{
	return (sl_chk_n_state(sl, ~mask));
}

#define LMF_UNATTACHED	    (1<<LMI_UNATTACHED)
#define LMF_ATTACH_PENDING  (1<<LMI_ATTACH_PENDING)
#define LMF_UNUSABLE	    (1<<LMI_UNUSABLE)
#define LMF_DISABLED	    (1<<LMI_DISABLED)
#define LMF_ENABLE_PENDING  (1<<LMI_ENABLE_PENDING)
#define LMF_ENABLED	    (1<<LMI_ENABLED)
#define LMF_DISABLE_PENDING (1<<LMI_DISABLE_PENDING)
#define LMF_DETACH_PENDING  (1<<LMI_DETACH_PENDING)

const char *
sl_i_state_name(int state)
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
		return ("(unknown)");
	}
}
static int
sl_set_i_state(struct sl *sl, int newstate)
{
	int oldstate = sl->info.lm.lmi_state;

	strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "%s <- %s", sl_i_state_name(newstate),
	       sl_i_state_name(oldstate));
	return ((sl->info.lm.lmi_state = sl->info.lm.lmi_state = newstate));
}
static inline int
sl_get_i_state(struct sl *sl)
{
	return sl->info.lm.lmi_state;
}
static inline int
sl_get_i_statef(struct sl *sl)
{
	return (1 << sl_get_i_state(sl));
}
static inline int
sl_chk_i_state(struct sl *sl, int mask)
{
	return (sl_get_i_statef(sl) & mask);
}
static inline int
sl_not_i_state(struct sl *sl, int mask)
{
	return (sl_chk_i_state(sl, ~mask));
}

#define ASP_DOWN	0
#define ASP_WACK_ASPUP	1
#define ASP_WACK_ASPDN	2
#define ASP_UP		3
#define ASP_INACTIVE	4
#define ASP_WACK_ASPAC	5
#define ASP_WACK_ASPIA	6
#define ASP_ACTIVE	7

const char *
sl_u_state_name(int state)
{
	switch (state) {
	case ASP_DOWN:
		return ("ASP_DOWN");
	case ASP_WACK_ASPUP:
		return ("ASP_WACK_ASPUP");
	case ASP_WACK_ASPDN:
		return ("ASP_WACK_ASPDN");
	case ASP_UP:
		return ("ASP_UP");
	case ASP_INACTIVE:
		return ("ASP_INACTIVE");
	case ASP_WACK_ASPAC:
		return ("ASP_WACK_ASPAC");
	case ASP_WACK_ASPIA:
		return ("ASP_WACK_ASPIA");
	case ASP_ACTIVE:
		return ("ASP_ACTIVE");
	default:
		return ("ASP_????");
	}
}
static inline int
sl_set_u_state(struct sl *sl, int newstate)
{
	int oldstate = sl->u_state;

	strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "%s <- %s", sl_u_state_name(newstate),
	       sl_u_state_name(oldstate));
	return (sl->u_state = newstate);
}
static inline int
sl_get_u_state(struct sl *sl)
{
	return (sl->u_state);
}
static inline int
sl_get_u_statef(struct sl *sl)
{
	return ((1 << sl_get_u_state(sl)));
}
static inline int
sl_chk_u_state(struct sl *sl, int mask)
{
	return (sl_get_u_statef(sl) & mask);
}
static inline int
sl_not_u_state(struct sl *sl, int mask)
{
	return (sl_chk_u_state(sl, ~mask));
}

/*
 *  Buffer allocation
 */
/**
 * sl_allocb: - allocate a buffer reliably
 * @q: active queue
 * @size: size of allocation
 * @priority: priority of allocation
 *
 * If allocation of a message block fails and this procedure returns NULL, the caller should place
 * the invoking message back on queue, q, and await q being enabled when a buffer becomes available.
 */
static inline fastcall mblk_t *
sl_allocb(queue_t *q, size_t size, int priority)
{
	mblk_t *mp;

	if (unlikely(!(mp = allocb(size, priority))))
		mi_bufcall(q, size, priority);
	return (mp);
}

/*
 *  Locking
 */
/**
 * sl_trylock: - try to lock an SL queue pair
 * @sl: SL private structure
 * @q: active queue (read or write queue)
 *
 * Note that if we awlays run (at least initially) from a service procedure, there is no need to
 * suppress interrupts while holding the lock.  This simple lock will do becuase the service
 * procedure is guaranteed single threaded on UP and SMP machines.  Also, because interrupts do not
 * need to be suppressed while holding the lock, the current task pointer identifies the thread and
 * the thread can be allowed to recurse on the lock.  When a queue procedure fails to acquire the
 * lock, it is marked to have its service procedure shceduled later, but we only remember one queue,
 * so if there are two waiting, the second one is reenabled.
 */
static inline bool
sl_trylock(struct sl *sl, queue_t *q)
{
	bool rtn = false;
	pl_t pl;

	pl = LOCK(&sl->lock, plbase);
	if (sl->users == 0 && (q->q_flag & QSVCBUSY)) {
		rtn = true;
		sl->users = 1;
		sl->owner = current;
	} else if (sl->users != 0 && sl->owner == current) {
		rtn = true;
		sl->users++;
	} else if (!sl->waitq) {
		sl->waitq = q;
	} else if (sl->waitq != q) {
		qenable(q);
	}
	UNLOCK(&sl->lock, pl);
	return (rtn);
}

/**
 * sl_unlock: - unlock an SL queue pair
 * @sl: SL private structure
 */
static inline void
sl_unlock(struct sl *sl)
{
	pl_t pl;

	pl = LOCK(&sl->lock, plbase);
	if (--sl->users == 0 && sl->waitq) {
		qenable(sl->waitq);
		sl->waitq = NULL;
	}
	UNLOCK(&sl->lock, pl);
}

/*
 *  M2UA Message Definitions
 */

#define M2UA_PPI    5

#define UA_VERSION  1
#define UA_PAD4(__len) (((__len)+3)&~0x3)
#define UA_MHDR(__version, __spare, __class, __type) \
	(__constant_htonl(((__version)<<24)|((__spare)<<16)|((__class)<<8)|(__type)))

#define UA_MSG_VERS(__hdr) ((ntohl(__hdr)>>24)&0xff)
#define UA_MSG_CLAS(__hdr) ((ntohl(__hdr)>> 8)&0xff)
#define UA_MSG_TYPE(__hdr) ((ntohl(__hdr)>> 0)&0xff)

/*
 *  MESSAGE CLASSES:-
 */
#define UA_CLASS_MGMT	0x00	/* UA Management (MGMT) Message */
#define UA_CLASS_XFER	0x01	/* M3UA Data transfer message */
#define UA_CLASS_SNMM	0x02	/* Signalling Network Mgmt (SNM) Messages */
#define UA_CLASS_ASPS	0x03	/* ASP State Maintenance (ASPSM) Messages */
#define UA_CLASS_ASPT	0x04	/* ASP Traffic Maintenance (ASPTM) Messages */
#define UA_CLASS_Q921	0x05	/* Q.931 User Part Messages */
#define UA_CLASS_MAUP	0x06	/* M2UA Messages */
#define UA_CLASS_CNLS	0x07	/* SUA Connectionless Messages */
#define UA_CLASS_CONS	0x08	/* SUA Connection Oriented Messages */
#define UA_CLASS_RKMM	0x09	/* Routing Key Management Messages */
#define UA_CLASS_TDHM	0x0a	/* TUA Dialog Handling Mesages */
#define UA_CLASS_TCHM	0x0b	/* TUA Component Handling Messages */

/*
 *  MESSAGES DEFINED IN EACH CLASS:-
 */
#define UA_MGMT_ERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x00)
#define UA_MGMT_NTFY		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x01)
#define UA_MGMT_LAST		0x01

#define UA_XFER_DATA		UA_MHDR(UA_VERSION, 0, UA_CLASS_XFER, 0x01)

#define UA_SNMM_DUNA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x01)
#define UA_SNMM_DAVA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x02)
#define UA_SNMM_DAUD		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x03)
#define UA_SNMM_SCON		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x04)
#define UA_SNMM_DUPU		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x05)
#define UA_SNMM_DRST		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x06)
#define UA_SNMM_LAST		0x06

#define UA_ASPS_ASPUP_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x01)
#define UA_ASPS_ASPDN_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x02)
#define UA_ASPS_HBEAT_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x03)
#define UA_ASPS_ASPUP_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x04)
#define UA_ASPS_ASPDN_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x05)
#define UA_ASPS_HBEAT_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x06)
#define UA_ASPS_LAST		0x06

#define UA_ASPT_ASPAC_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x01)
#define UA_ASPT_ASPIA_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x02)
#define UA_ASPT_ASPAC_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x03)
#define UA_ASPT_ASPIA_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x04)
#define UA_ASPT_LAST		0x04

#define UA_RKMM_REG_REQ		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x01)
#define UA_RKMM_REG_RSP		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x02)
#define UA_RKMM_DEREG_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x03)
#define UA_RKMM_DEREG_RSP	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x04)
#define UA_RKMM_LAST		0x04

#define UA_MHDR_SIZE (sizeof(uint32_t)*2)
#define UA_PHDR_SIZE (sizeof(uint32_t))
#define UA_MAUP_SIZE (UA_MHDR_SIZE + UA_PHDR_SIZE + sizeof(uint32_t))

#define UA_TAG_MASK		(__constant_htonl(0xffff0000))
#define UA_PTAG(__phdr)		((htonl(__phdr)>>16)&0xffff)
#define UA_PLEN(__phdr)		(htonl(__phdr)&0xffff)
#define UA_SIZE(__phdr)		(__constant_htonl(__phdr)&0xffff)
#define UA_TAG(__phdr)		((__constant_htonl(__phdr)>>16)&0xffff)
#define UA_PHDR(__phdr, __length) \
	(htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))
#define UA_CONST_PHDR(__phdr, __length) \
	(__constant_htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))

/*
 *  COMMON PARAMETERS:-
 *
 *  Common parameters per draft-ietf-sigtran-m2ua-10.txt
 *  Common parameters per draft-ietf-sigtran-m3ua-08.txt
 *  Common parameters per draft-ietf-sigtran-sua-07.txt
 *  Common parameters per rfc3057.txt
 *  -------------------------------------------------------------------
 */
#define UA_PARM_RESERVED	UA_CONST_PHDR(0x0000,0)
#define UA_PARM_IID		UA_CONST_PHDR(0x0001,sizeof(uint32_t))
#define UA_PARM_IID_RANGE	UA_CONST_PHDR(0x0002,0)	/* m2ua-10 */
#define UA_PARM_DATA		UA_CONST_PHDR(0x0003,0)	/* sua-07 */
#define UA_PARM_IID_TEXT	UA_CONST_PHDR(0x0003,0)
#define UA_PARM_INFO		UA_CONST_PHDR(0x0004,0)
#define UA_PARM_APC		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_DLCI		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_RC		UA_CONST_PHDR(0x0006,sizeof(uint32_t))
#define UA_PARM_DIAG		UA_CONST_PHDR(0x0007,0)
#define UA_PARM_IID_RANGE1	UA_CONST_PHDR(0x0008,0)	/* rfc3057 */
#define UA_PARM_HBDATA		UA_CONST_PHDR(0x0009,0)
#define UA_PARM_REASON		UA_CONST_PHDR(0x000a,sizeof(uint32_t))
#define UA_PARM_TMODE		UA_CONST_PHDR(0x000b,sizeof(uint32_t))
#define UA_PARM_ECODE		UA_CONST_PHDR(0x000c,sizeof(uint32_t))
#define UA_PARM_STATUS		UA_CONST_PHDR(0x000d,sizeof(uint32_t))
#define UA_PARM_ASPID		UA_CONST_PHDR(0x000e,sizeof(uint32_t))
#define UA_PARM_PROT_DATA	UA_CONST_PHDR(0x000e,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_CONG_LEVEL	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_REL_REASON	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_TEI_STATUS	UA_CONST_PHDR(0x0010,sizeof(uint32_t))	/* rfc3057 */

/*
 *  Somewhat common field values:
 */
#define   UA_ECODE_INVALID_VERSION		(0x01)
#define   UA_ECODE_INVALID_IID			(0x02)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_CLASS	(0x03)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_TYPE	(0x04)
#define   UA_ECODE_UNSUPPORTED_TRAFFIC_MODE	(0x05)
#define   UA_ECODE_UNEXPECTED_MESSAGE		(0x06)
#define   UA_ECODE_PROTOCOL_ERROR		(0x07)
#define   UA_ECODE_UNSUPPORTED_IID_TYPE		(0x08)
#define   UA_ECODE_INVALID_STREAM_IDENTIFIER	(0x09)
#define  IUA_ECODE_UNASSIGNED_TEI		(0x0a)
#define  IUA_ECODE_UNRECOGNIZED_SAPI		(0x0b)
#define  IUA_ECODE_INVALID_TEI_SAPI_COMBINATION	(0x0c)
#define   UA_ECODE_REFUSED_MANAGEMENT_BLOCKING	(0x0d)
#define   UA_ECODE_ASPID_REQUIRED		(0x0e)
#define   UA_ECODE_INVALID_ASPID		(0x0f)
#define M2UA_ECODE_ASP_ACTIVE_FOR_IIDS		(0x10)
#define   UA_ECODE_INVALID_PARAMETER_VALUE	(0x11)
#define   UA_ECODE_PARAMETER_FIELD_ERROR	(0x12)
#define   UA_ECODE_UNEXPECTED_PARAMETER		(0x13)
#define   UA_ECODE_DESTINATION_STATUS_UNKNOWN	(0x14)
#define   UA_ECODE_INVALID_NETWORK_APPEARANCE	(0x15)
#define   UA_ECODE_MISSING_PARAMETER		(0x16)
#define   UA_ECODE_ROUTING_KEY_CHANGE_REFUSED	(0x17)
#define   UA_ECODE_INVALID_ROUTING_CONTEXT	(0x19)
#define   UA_ECODE_NO_CONFIGURED_AS_FOR_ASP	(0x1a)
#define   UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN	(0x1b)

#define UA_STATUS_AS_DOWN			(0x00010001)
#define UA_STATUS_AS_INACTIVE			(0x00010002)
#define UA_STATUS_AS_ACTIVE			(0x00010003)
#define UA_STATUS_AS_PENDING			(0x00010004)
#define UA_STATUS_AS_INSUFFICIENT_ASPS		(0x00020001)
#define UA_STATUS_ALTERNATE_ASP_ACTIVE		(0x00020002)
#define UA_STATUS_ASP_FAILURE			(0x00020003)
#define UA_STATUS_AS_MINIMUM_ASPS		(0x00020004)

#define UA_TMODE_OVERRIDE			(0x1)
#define UA_TMODE_LOADSHARE			(0x2)
#define UA_TMODE_BROADCAST			(0x3)
#define UA_TMODE_SB_OVERRIDE			(0x4)
#define UA_TMODE_SB_LOADSHARE			(0x5)
#define UA_TMODE_SB_BROADCAST			(0x6)

#define UA_RESULT_SUCCESS			(0x00)
#define UA_RESULT_FAILURE			(0x01)

/*
 *  M2UA-Specific Messages: per draft-ietf-sigtran-m2ua-10.txt
 *  -------------------------------------------------------------------
 */
#define M2UA_MAUP_DATA		UA_MHDR(1, 0, UA_CLASS_MAUP, 0x01)
#define M2UA_MAUP_ESTAB_REQ	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x02)
#define M2UA_MAUP_ESTAB_CON	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x03)
#define M2UA_MAUP_REL_REQ	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x04)
#define M2UA_MAUP_REL_CON	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x05)
#define M2UA_MAUP_REL_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x06)
#define M2UA_MAUP_STATE_REQ	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x07)
#define M2UA_MAUP_STATE_CON	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x08)
#define M2UA_MAUP_STATE_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x09)
#define M2UA_MAUP_RETR_REQ	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0a)
#define M2UA_MAUP_RETR_CON	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0b)
#define M2UA_MAUP_RETR_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0c)
#define M2UA_MAUP_RETR_COMP_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0d)
#define M2UA_MAUP_CONG_IND	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0e)
#define M2UA_MAUP_DATA_ACK	UA_MHDR(1, 0, UA_CLASS_MAUP, 0x0f)
#define M2UA_MAUP_LAST		0x0f

/*
 *  M2UA-Specific Parameters: per draft-ietf-sigtran-m2ua-10.txt
 *  -------------------------------------------------------------------
 */
#define M2UA_PARM_DATA1		UA_CONST_PHDR(0x0300,0)
#define M2UA_PARM_DATA2		UA_CONST_PHDR(0x0301,0)
#define M2UA_PARM_STATE_REQUEST	UA_CONST_PHDR(0x0302,sizeof(uint32_t))
#define M2UA_PARM_STATE_EVENT	UA_CONST_PHDR(0x0303,sizeof(uint32_t))
#define M2UA_PARM_CONG_STATUS	UA_CONST_PHDR(0x0304,sizeof(uint32_t))
#define M2UA_PARM_DISC_STATUS	UA_CONST_PHDR(0x0305,sizeof(uint32_t))
#define M2UA_PARM_ACTION	UA_CONST_PHDR(0x0306,sizeof(uint32_t))
#define M2UA_PARM_SEQNO		UA_CONST_PHDR(0x0307,sizeof(uint32_t))
#define M2UA_PARM_RETR_RESULT	UA_CONST_PHDR(0x0308,sizeof(uint32_t))
#define M2UA_PARM_LINK_KEY	UA_CONST_PHDR(0x0309,sizeof(uint32_t)*6)
#define M2UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x030a,sizeof(uint32_t))
#define M2UA_PARM_SDTI		UA_CONST_PHDR(0x030b,sizeof(uint32_t))
#define M2UA_PARM_SDLI		UA_CONST_PHDR(0x030c,sizeof(uint32_t))
#define M2UA_PARM_REG_RESULT	UA_CONST_PHDR(0x030d,sizeof(uint32_t))
#define M2UA_PARM_REG_STATUS	UA_CONST_PHDR(0x030e,sizeof(uint32_t))
#define M2UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x030f,sizeof(uint32_t)*4)
#define M2UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0310,sizeof(uint32_t))
#define M2UA_PARM_CORR_ID	UA_CONST_PHDR(0x0311,sizeof(uint32_t))
#define M2UA_PARM_CORR_ID_ACK	UA_CONST_PHDR(0x0312,sizeof(uint32_t))

#define M2UA_ACTION_RTRV_BSN		(0x01)
#define M2UA_ACTION_RTRV_MSGS		(0x02)

#define M2UA_STATUS_LPO_SET		(0x00)
#define M2UA_STATUS_LPO_CLEAR		(0x01)
#define M2UA_STATUS_EMER_SET		(0x02)
#define M2UA_STATUS_EMER_CLEAR		(0x03)
#define M2UA_STATUS_FLUSH_BUFFERS	(0x04)
#define M2UA_STATUS_CONTINUE		(0x05)
#define M2UA_STATUS_CLEAR_RTB		(0x06)
#define M2UA_STATUS_AUDIT		(0x07)
#define M2UA_STATUS_CONG_CLEAR		(0x08)
#define M2UA_STATUS_CONG_ACCEPT		(0x09)
#define M2UA_STATUS_CONG_DISCARD	(0x0a)

#define M2UA_EVENT_RPO_ENTER		(0x01)
#define M2UA_EVENT_RPO_EXIT		(0x02)
#define M2UA_EVENT_LPO_ENTER		(0x03)
#define M2UA_EVENT_LPO_EXIT		(0x04)

#define M2UA_LEVEL_NONE			(0x00)
#define M2UA_LEVEL_1			(0x01)
#define M2UA_LEVEL_2			(0x02)
#define M2UA_LEVEL_3			(0x03)
#define M2UA_LEVEL_4			(0x04)	/* big argument */

/*
 *  Decode functions.
 */
struct ua_parm {
	union {
		uchar *cp;		/* pointer to parameter field */
		uint32_t *wp;		/* pointer to parameter field */
	};
	size_t len;			/* length of parameter field */
	uint32_t val;			/* value of first 4 bytes (host order) */
};

/*
 *  Extract a parameter from a message.  If the parameter does not exist in the message it returns
 *  false; otherwise true, and sets the parameter values if parm is non-NULL.
 */
static bool
ua_dec_parm(mblk_t *mp, struct ua_parm *parm, uint32_t tag)
{
	uint32_t *wp;
	bool rtn = false;

	for (wp = (uint32_t *) mp->b_rptr + 2; (uchar *) (wp + 1) <= mp->b_wptr;
	     wp += (UA_PLEN(*wp) + 3) >> 2) {
		if (UA_TAG(*wp) == UA_TAG(tag)) {
			rtn = true;
			if (parm) {
				parm->wp = (wp + 1);
				parm->len = UA_PLEN(*wp);
				parm->val = ntohl(wp[1]);
			}
			break;
		}
	}
	return (rtn);
}

/*
 *  =========================================================================
 *
 *  OUTPUT Events
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  SL Provider (M2PA) -> SL User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * lmi_info_ack: - issue an LMI_INFO_ACK prmitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_info_ack(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p) + sl->loc_len, BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = sl->info.lm.lmi_version;
		p->lmi_state = sl->info.lm.lmi_state;
		p->lmi_max_sdu = sl->info.lm.lmi_max_sdu;
		p->lmi_min_sdu = sl->info.lm.lmi_min_sdu;
		p->lmi_header_len = sl->info.lm.lmi_header_len;
		p->lmi_ppa_style = sl->info.lm.lmi_ppa_style;
		p->lmi_ppa_length = sl->loc_len;
		p->lmi_ppa_offset = sizeof(*p);
		p->lmi_prov_flags = sl->sl.flags; /* FIXME */
		p->lmi_prov_state = sl->sl.state; /* FIXME */
		mp->b_wptr += sizeof(*p);
		bcopy(&sl->loc, mp->b_wptr, sl->loc_len);
		mp->b_wptr += sl->loc_len;
		freemsg(msg);
		printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, sl));
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_ok_ack: - issue an LMI_OK_ACK prmitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
lmi_ok_ack(struct sl *sl, queue_t *q, mblk_t *msg, lmi_long prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_DISABLED;
			sl_set_i_state(sl, LMI_DISABLED);
			sl_set_n_state(sl, NS_IDLE);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			sl_set_i_state(sl, LMI_UNATTACHED);
			sl_set_n_state(sl, NS_UNBND);
			break;
		default:
			/* default is don't change state */
			p->lmi_state = sl_get_i_state(sl);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		printd(("%s: %p: <- LMI_OK_ACK\n", MOD_NAME, sl));
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ack: - issue an LMI_ERROR_ACK prmitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @err: positive LMI error or negative UNIX error
 */
static int
lmi_error_ack(struct sl *sl, queue_t *q, mblk_t *msg, lmi_long prim, lmi_long err)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_error_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			sl_set_i_state(sl, LMI_UNATTACHED);
			sl_set_n_state(sl, NS_UNBND);
			break;
		case LMI_DETACH_PENDING:
		case LMI_ENABLE_PENDING:
			p->lmi_state = LMI_DISABLED;
			sl_set_i_state(sl, LMI_DISABLED);
			sl_set_n_state(sl, NS_IDLE);
			break;
		case LMI_DISABLE_PENDING:
			p->lmi_state = LMI_ENABLED;
			sl_set_i_state(sl, LMI_ENABLED);
			switch (sl_get_n_state(sl)) {
			case NS_WACK_DREQ6:
				sl_set_n_state(sl, NS_WCON_CREQ);
				break;
			case NS_WACK_DREQ7:
				sl_set_n_state(sl, NS_WRES_CIND);
				break;
			case NS_WACK_DREQ9:
				sl_set_n_state(sl, NS_DATA_XFER);
				break;
			case NS_WACK_DREQ10:
				sl_set_n_state(sl, NS_WCON_RREQ);
				break;
			case NS_WACK_DREQ11:
				sl_set_n_state(sl, NS_WRES_RIND);
				break;
			default:
				/* Default is not to change state. */
				swerr();
				break;
			}
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, sl));
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_enable_con: - issue an LMI_ENABLE_CON primitive upstream
 * @sl: private strucutre
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_enable_con(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	ensure(sl_get_i_state(sl) == LMI_ENABLE_PENDING, return (-EFAULT));
	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = LMI_ENABLED;
		mp->b_wptr += sizeof(*p);
		sl_set_i_state(sl, LMI_ENABLED);
		freemsg(msg);
		printd(("%s: %p: <- LMI_ENABLE_CON\n", MOD_NAME, sl));
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_con: - issue an LMI_DISABLE_CON primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_disable_con(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	ensure(sl_get_i_state(sl) == LMI_DISABLE_PENDING, return (-EFAULT));
	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = LMI_DISABLED;
		sl_set_i_state(sl, LMI_DISABLED);
		sl_set_n_state(sl, NS_IDLE);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		printd(("%s: %p: <- LMI_DISABLE_CON\n", MOD_NAME, sl));
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  ---------------------------------------------
 */
#if 0
static int
lmi_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *msg, lmi_ulong flags, void *opt_ptr,
		size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((lmi_optmgmt_ack_t *) mp->b_wptr)++;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", MOD_NAME, sl));
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/**
 * lmi_error_ind: - issue an LMI_ERROR_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: postitive LMI or negative UNIX error
 * @fatal: whether error is fatal or not
 */
static int
lmi_error_ind(struct sl *sl, queue_t *q, mblk_t *msg, lmi_long err, int fatal)
{
	mblk_t *mp;
	lmi_error_ind_t *p;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		if (fatal)
			sl_set_i_state(sl, LMI_UNUSABLE);
		p->lmi_state = sl_get_i_state(sl);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		printd(("%s: %p: <- LMI_ERROR_IND\n", MOD_NAME, sl));
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * lmi_stats_ind: - issue an LMI_STATS_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @interval: interval reported
 * @timestamp: timestamp of report
 */
static int
lmi_stats_ind(struct sl *sl, queue_t *q, mblk_t *msg, lmi_ulong interval, lmi_ulong timestamp)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = timestamp;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * lmi_event_ind: - issue an LMI_EVENT_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @oid: event id
 * @severity: event severity
 * @inf_ptr: information pointer
 * @inf_len: information length
 */
static int
lmi_event_ind(struct sl *sl, queue_t *q, mblk_t *msg, lmi_ulong oid, lmi_ulong severity,
	      void *inf_ptr, size_t inf_len)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p) + inf_len, BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = jiffies;
			p->lmi_severity = severity;
			mp->b_wptr += sizeof(*p);
			bcopy(mp->b_wptr, inf_ptr, inf_len);
			mp->b_wptr += inf_len;
			freemsg(msg);
			printd(("%s: %p: <- LMI_EVENT_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

/**
 * sl_pdu_ind: - issued an SL_PDU_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @dp: user data to indicate
 */
static inline fastcall __hot_in int
sl_pdu_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_pdu_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_PDU_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- SL_PDU_IND [%lu]\n", MOD_NAME, sl,
				(ulong) msgdsize(mp)));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congested_ind: - issue an SL_LINK_CONGESTED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cong: congestion status
 * @disc: discard status
 */
static int
sl_link_congested_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			sl->cong = cong;
			sl->disc = disc;
			freemsg(msg);
			printd(("%s: %p: <- SL_LINK_CONGESTED_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congestion_ceased_ind: - issue an SL_LINK_CONGESTION_CEASED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cong: congestion status
 * @disc: discard status
 */
static int
sl_link_congestion_ceased_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ceased_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			sl->cong = cong;
			sl->disc = disc;
			freemsg(msg);
			printd(("%s: %p: <- SL_LINK_CONGESTION_CEASED_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieved_message_ind: - issue an SL_RETRIEVED_MESSAGE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @dp: retrieved user data
 */
static int
sl_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_retrieved_msg_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			printd(("%s: %p: <- SL_RETRIEVED_MESSAGE_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_complete_ind: - issue an SL_RETRIEVAL_COMPLETE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 */
static int
sl_retrieval_complete_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_retrieval_comp_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		printd(("%s: %p: <- SL_RETRIEVAL_COMPLETE_IND\n", MOD_NAME, sl));
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_rb_cleared_ind: - issue an SL_RB_CLEARED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_rb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RB_CLEARED_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			printd(("%s: %p: <- SL_RB_CLEARED_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_bsnt_ind: - issue an SL_BSNT_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @bsnt: value of BSNT to indicate
 */
static int
sl_bsnt_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong bsnt)
{
	sl_bsnt_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_BSNT_IND;
			p->sl_bsnt = bsnt;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			printd(("%s: %p: <- SL_BSNT_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_in_service_ind: - issue an SL_IN_SERVICE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_in_service_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_in_service_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_IN_SERVICE_IND;
		freemsg(msg);
		printd(("%s: %p: <- SL_IN_SERVICE_IND\n", MOD_NAME, sl));
		mp->b_wptr += sizeof(*p);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_out_of_service_ind: - issue an SL_OUT_OF_SERVICE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: reason for failure
 */
static int
sl_out_of_service_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong reason)
{
	sl_out_of_service_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_reason = reason;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		printd(("%s: %p: <- SL_OUT_OF_SERVICE_IND\n", MOD_NAME, sl));
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_outage_ind: - issue an SL_REMOTE_PROCESSOR_OUTAGE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_remote_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_out_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			printd(("%s: %p: <- SL_PROCESSOR_OUTAGE_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_recovered_ind: - issue an SL_REMOTE_PROCESSOR_RECOVERED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_remote_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_recovered_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			printd(("%s: %p: <- SL_REMOTE_PROCESSOR_RECOVERED_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_rtb_cleared_ind: - issue an SL_RTB_CLEARED_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_rtb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rtb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RTB_CLEARED_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			printd(("%s: %p: <- SL_RTB_CLEARED_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * sl_retrieval_not_possible_ind: - issue an SL_RETRIEVAL_NOT_POSSIBLE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_retrieval_not_possible_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieval_not_poss_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			printd(("%s: %p: <- SL_RETRIEVAL_NOT_POSSIBLE_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * sl_bsnt_not_retrievable_ind: - issue an SL_BSNT_NOT_RETRIEVABLE_IND primitive upstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @bsnt: BSNT
 */
static int
sl_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong bsnt)
{
	sl_bsnt_not_retr_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
			p->sl_bsnt = bsnt;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			printd(("%s: %p: <- SL_BSNT_NOT_RETRIEVABLE_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

static int
sl_local_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_out_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			printd(("%s: %p: <- SL_LOCAL_PROCESSOR_OUTAGE_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static int
sl_local_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_recovered_ind_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(RD(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LOCAL_PROCESSOR_RECOVERED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			printd(("%s: %p: <- SL_LOCAL_PROCESSOR_RECOVERED_IND\n", MOD_NAME, sl));
			putnext(RD(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NPI User (M2PA) -> NPI Provider (SCTP) Primitives
 *
 *  -------------------------------------------------------------------------
 */

/**
 * n_bind_req: - issue a bind request to fullfill an attach request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add_ptr: pointer to address to bind
 * @add_len: length of bind address
 */
static int
n_bind_req(struct sl *sl, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len)
{
	N_bind_req_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p) + add_len, BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = 0;
		p->BIND_flags = TOKEN_REQUEST;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		sl_set_i_state(sl, LMI_ATTACH_PENDING);
		sl_set_n_state(sl, NS_WACK_BREQ);
		freemsg(msg);
		printd(("%s: %p: N_BIND_REQ ->\n", MOD_NAME, sl));
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_unbind_req: - issue an unbind request to fullfull a detach request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
n_unbind_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		sl_set_i_state(sl, LMI_DETACH_PENDING);
		sl_set_n_state(sl, NS_WACK_UREQ);
		freemsg(msg);
		printd(("%s: %p: N_UNBIND_REQ ->\n", MOD_NAME, sl));
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_req: - issue a connection request to fulfull an enable request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dst_ptr: destination address
 * @dst_len: destination adresss length
 * @qos_ptr: quality of service parameters
 * @qos_len: quality of service parameters length
 */
static int
n_conn_req(struct sl *sl, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len)
{
	N_qos_sel_conn_sctp_t *n;
	N_conn_req_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p) + dst_len + sizeof(*n), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->QOS_length = sizeof(*n);
		p->QOS_offset = sizeof(*p) + dst_len;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		n = (typeof(n)) mp->b_wptr;
		n->n_qos_type = N_QOS_SEL_CONN_SCTP;
		n->i_streams = 2;
		n->o_streams = 2;
		mp->b_wptr += sizeof(*n);
		sl_set_i_state(sl, LMI_ENABLE_PENDING);
		sl_set_n_state(sl, NS_WCON_CREQ);
		freemsg(msg);
		printd(("%s: %p: N_CONN_REQ ->\n", MOD_NAME, sl));
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_req: - issue a disconnection request to fulfill a disable request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: disconnect reason
 */
static int
n_discon_req(struct sl *sl, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_discon_req_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(WR(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_REQ;
			p->DISCON_reason = reason;
			p->RES_length = 0;
			p->RES_offset = 0;
			p->SEQ_number = 0;
			mp->b_wptr += sizeof(*p);
			sl_set_i_state(sl, LMI_DISABLE_PENDING);
			switch (sl_get_n_state(sl)) {
			case NS_WCON_CREQ:
				sl_set_n_state(sl, NS_WACK_DREQ6);
				break;
			case NS_WRES_CIND:
				sl_set_n_state(sl, NS_WACK_DREQ7);
				break;
			case NS_DATA_XFER:
				sl_set_n_state(sl, NS_WACK_DREQ9);
				break;
			case NS_WCON_RREQ:
				sl_set_n_state(sl, NS_WACK_DREQ10);
				break;
			case NS_WRES_RIND:
				sl_set_n_state(sl, NS_WACK_DREQ11);
				break;
			default:
				sl_set_n_state(sl, NS_IDLE);
				break;
			}
			freemsg(msg);
			printd(("%s: %p: N_DISCON_REQ ->\n", MOD_NAME, sl));
			putnext(WR(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_data_req: - issue a data transfer request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flags
 * @sid: SCTP stream on which to send
 * @dp: user data to transfer
 */
static inline fastcall __hot_out int
n_data_req(struct sl *sl, queue_t *q, mblk_t *msg, np_ulong flags, np_ulong sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *n;
	N_data_req_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p) + sizeof(*n), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(WR(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			n = (typeof(n)) mp->b_wptr;
			n->n_qos_type = N_QOS_SEL_DATA_SCTP;
			n->ppi = M2UA_PPI;
			n->sid = sid;
			n->ssn = 0;
			n->tsn = 0;
			n->more = flags;
			mp->b_wptr += sizeof(*n);
			mp->b_cont = dp;
			freemsg(msg);
			printd(("%s: %p: N_DATA_REQ ->\n", MOD_NAME, sl));
			putnext(WR(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_exdata_req: - issue an expedited data transfer request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flags
 * @sid: SCTP stream on which to send
 * @dp: user data to transfer
 */
static inline fastcall int
n_exdata_req(struct sl *sl, queue_t *q, mblk_t *msg, np_ulong flags, np_ulong sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *n;
	N_exdata_req_t *p;
	mblk_t *mp;

	if (likely((mp = sl_allocb(q, sizeof(*p) + sizeof(*n), BPRI_MED)) != NULL)) {
		mp->b_band = 1;	/* expedite */
		if (likely(bcanputnext(WR(q), mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_REQ;
			mp->b_wptr += sizeof(*p);
			n = (typeof(n)) mp->b_wptr;
			n->n_qos_type = N_QOS_SEL_DATA_SCTP;
			n->ppi = M2UA_PPI;
			n->sid = sid;
			n->ssn = 0;
			n->tsn = 0;
			n->more = flags;
			mp->b_wptr += sizeof(*n);
			mp->b_cont = dp;
			freemsg(msg);
			printd(("%s: %p: N_EXDATA_REQ ->\n", MOD_NAME, sl));
			putnext(WR(q), mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  ==========================================================================
 *
 *  M2UA State Machines
 *
 *  ==========================================================================
 */

/*
 *  ==========================================================================
 *
 *  OUTPUT Events
 *
 *  ==========================================================================
 */
/*
 *  --------------------------------------------------------------------------
 *
 *  SL AS -> SL SG (M2UA) Sent Messages
 *
 *  --------------------------------------------------------------------------
 */
/**
 * sl_send_mgmt_err: - send MGMT ERR message
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @ecode: error code
 * @dia_ptr: pointer to diagnostics
 * @dia_len: length of diagnostics
 */
static inline __unlikely int
sl_send_mgmt_err(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t ecode, caddr_t dia_ptr,
		 size_t dia_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_ECODE) + dia_len ? UA_SIZE(UA_PARM_DIAG) +
	    UA_PAD4(dia_len) : 0;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_MGMT_ERR;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_ECODE;
		p[3] = htonl(ecode);
		p += 4;
		if (dia_len) {
			*p++ = UA_PHDR(UA_PARM_DIAG, dia_len);
			bcopy(dia_ptr, p, dia_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(dia_len);
		/* sent unordered and expedited on management stream */
		if (unlikely((err = n_exdata_req(sl, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);

	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_aspup_req: - send ASP Up
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @inf_ptr: Info pointer
 * @inf_len: Info length
 */
static int
sl_send_asps_aspup_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t inf_ptr,
		       size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPUP_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		sl_set_u_state(sl, ASP_WACK_ASPUP);
		mi_timer(sl->aspup_tack, 2000);
		/* send unordered and expedited on stream 0 */
		if (unlikely((err = n_exdata_req(sl, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_aspdn_req: - send ASP Down
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @inf_ptr: Info pointer
 * @inf_len: Info length
 */
static int
sl_send_asps_aspdn_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t inf_ptr,
		       size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		sl_set_u_state(sl, ASP_WACK_ASPDN);
		mi_timer(sl->aspdn_tack, 2000);
		if (unlikely((err = n_exdata_req(sl, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_hbeat_req: - send a BEAT message
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sid: SCTP stream identifier
 * @hbt_ptr: heartbeat info pointer
 * @hbt_len: heartbeat info length
 */
static inline int
sl_send_asps_hbeat_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t sid, caddr_t hbt_ptr,
		       size_t hbt_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
		mi_timer(sl->hbeat_tack, 2000);
		/* send ordered on specified stream */
		if (unlikely((err = n_data_req(sl, q, msg, 0, sid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_hbeat_ack: - send a BEAT message
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sid: SCTP stream identifier
 * @hbt_ptr: heartbeat info pointer
 * @hbt_len: heartbeat info length
 */
static int
sl_send_asps_hbeat_ack(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t sid, caddr_t hbt_ptr,
		       size_t hbt_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
		/* send ordered on specified stream */
		if (unlikely((err = n_data_req(sl, q, msg, 0, sid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_aspt_aspac_req: - send ASP Active
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tmode: traffic mode
 * @iid: Interface Id
 * @num_iid: number of interface ids
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
static int
sl_send_aspt_aspac_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t tmode, uint32_t *iid,
		       uint32_t num_iid, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_TMODE) + num_iid ? UA_PHDR_SIZE +
	    num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPAC_REQ;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_TMODE;
		p[3] = htonl(tmode);
		p += 4;
		if (num_iid) {
			uint32_t *ip = iid;

			*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
			while (num_iid--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		sl_set_u_state(sl, ASP_WACK_ASPAC);
		mi_timer(sl->aspac_tack, 2000);
		/* always sent on same stream as data */
		if (unlikely((err = n_data_req(sl, q, msg, 0, iid ? *iid : 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_aspt_aspia_req: - send ASP Inactive
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iid: Interface Id
 * @num_iid: number of interface ids
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
static int
sl_send_aspt_aspia_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t *iid, uint32_t num_iid,
		       caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (num_iid) {
			uint32_t *ip = iid;

			*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
			while (num_iid--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		sl_set_u_state(sl, ASP_WACK_ASPIA);
		mi_timer(sl->aspia_tack, 2000);
		if (unlikely((err = n_data_req(sl, q, msg, 0, iid ? *iid : 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * sl_send_rkmm_reg_req: - send REG REQ
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @id: request identifier
 * @sdti: Signalling Data Terminal Identifier
 * @sdli: Signalling Data Link Identifier
 */
static int
sl_send_rkmm_reg_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t id, uint32_t sdti,
		     uint32_t sdli)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_LINK_KEY);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_REG_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = M2UA_PARM_LINK_KEY;
		p[3] = M2UA_PARM_LOC_KEY_ID;
		p[4] = htonl(id);
		p[5] = M2UA_PARM_SDTI;
		p[6] = htonl(sdti);
		p[7] = M2UA_PARM_SDLI;
		p[8] = htonl(sdli);
		p += 9;
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = n_exdata_req(sl, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
#endif

/**
 * sl_send_maup_data1: - send DATA1
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iid: interface id
 * @dp: user data
 */
static inline fastcall __hot_write int
sl_send_maup_data1(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t iid, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA1, dlen);
		p += 5;
		mp->b_wptr = (unsigned char *) p;

		mp->b_cont = dp->b_cont;
		if (unlikely((err = n_data_req(sl, q, msg, 0, iid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_data2: - send DATA2
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iid: interface id
 * @dp: user data
 * @pri: message priority (placed in first byte of message)
 */
static inline fastcall __hot_write int
sl_send_maup_data2(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t iid, mblk_t *dp, uint8_t pri)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE + 1;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA2, dlen + 1);
		p += 5;
		*(unsigned char *)p = pri;
		mp->b_wptr = (unsigned char *) p + 1;

		mp->b_cont = dp->b_cont;
		if (unlikely((err = n_data_req(sl, q, msg, 0, iid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_estab_req: - send MAUP Establish Request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iid: interface id
 */
static int
sl_send_maup_estab_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t iid)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_ESTAB_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = n_data_req(sl, q, msg, 0, iid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_rel_req: - send MAUP Release Request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iid: interface id
 */
static int
sl_send_maup_rel_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t iid)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = n_data_req(sl, q, msg, 0, iid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_state_req: - send MAUP State Request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iid: interface id
 * @request: state request
 */
static int
sl_send_maup_state_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t iid, const uint32_t request)
{
	int err;
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_STATE_REQUEST);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_STATE_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p[4] = M2UA_PARM_STATE_REQUEST;
		p[5] = request;	/* already network byte order */
		p += 6;
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = n_data_req(sl, q, msg, 0, iid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_retr_req: - send MAUP Retrieval Request
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iid: interface id
 * @fsnc: optional FSNC value
 */
static int
sl_send_maup_retr_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t iid, uint32_t *fsnc)
{
	int err;
	mblk_t *mp;
	const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_ACTION)
	    + fsnc ? UA_SIZE(M2UA_PARM_SEQNO) : 0;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_RETR_REQ;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p[4] = M2UA_PARM_ACTION;
		if (fsnc) {
			p[5] = __constant_htonl(M2UA_ACTION_RTRV_MSGS);
			p[6] = M2UA_PARM_SEQNO;
			p[7] = *fsnc;	/* already network byte order */
			p += 8;
		} else {
			p[5] = __constant_htonl(M2UA_ACTION_RTRV_BSN);
			p += 6;
		}
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = n_data_req(sl, q, msg, 0, iid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_data_ack: - send MAUP Data Acknowledgement
 * @sl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iid: interface id
 * @corid: correlation id
 */
static inline fastcall __hot_in int
sl_send_maup_data_ack(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t iid, uint32_t corid)
{
	int err;
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_CORR_ID_ACK);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA_ACK;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(iid);
		p[4] = M2UA_PARM_CORR_ID_ACK;
		p[5] = corid;	/* already in network byte order */
		mp->b_wptr = (unsigned char *) &p[6];

		if (unlikely((err = n_data_req(sl, q, msg, 0, iid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  ==========================================================================
 *
 *  INPUT Events
 *
 *  ==========================================================================
 */
/*
 *  --------------------------------------------------------------------------
 *
 *  SL SG -> SL AS (M2UA) Received Messages
 *
 *  --------------------------------------------------------------------------
 */

/**
 * sl_recv_mgmt_err: - process received MGMT ERR message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MGMT ERR message (with header)
 */
static int
sl_recv_mgmt_err(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* FIXME: check the current state: if we are in a WACK state then we probably have to deal
	   with the error.  If we are not in a WACK state, the error might be able to be ignored,
	   but also might require bringing the ASP down. */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_mgmt_ntfy: - process received MGMT NTFY message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MGMT NTFY message (with header)
 */
static int
sl_recv_mgmt_ntfy(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* FIXME: Mostly to do with traffic modes.  If we are in override and we came up as a
	   standby, we might now be active. */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_asps_hbeat_req: - process received ASPS BEAT message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPS BEAT message (with header)
 */
static int
sl_recv_asps_hbeat_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	caddr_t hptr = NULL;		/* pointer to heartbeat data */
	size_t hlen = 0;		/* length of heartbeat data */
	int sid = 0;			/* stream id for ack message */

	/* FIXME: need to dig HBEAT DATA out of message and possibly derive the stream id from the
	   IID. */

	return sl_send_asps_hbeat_ack(sl, q, mp, sid, hptr, hlen);
}

/**
 * sl_recv_asps_aspup_ack: - process received ASPS ASP Up Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPS ASP Up Ack message (with header)
 */
static int
sl_recv_asps_aspup_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_u_state(sl) != ASP_WACK_ASPUP)
		goto outstate;
	mi_timer_cancel(sl->aspup_tack);
	sl_set_u_state(sl, ASP_INACTIVE);
	switch (sl_get_i_state(sl)) {
	case LMI_ATTACH_PENDING:
		sl_set_i_state(sl, LMI_DISABLED);
		return lmi_ok_ack(sl, q, mp, LMI_ATTACH_REQ);
	case LMI_ENABLE_PENDING:
	{
		uint32_t *iid;

		/* one more step */
		iid = (sl->iid ? &sl->iid : NULL);
		sl_set_u_state(sl, ASP_WACK_ASPAC);
		return sl_send_aspt_aspac_req(sl, q, mp, sl->tm, iid, 1, NULL, 0);
	}
	}
outstate:
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_asps_aspdn_ack: - process received ASPS ASP Down Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPS ASP Down Ack message (with header)
 */
static int
sl_recv_asps_aspdn_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	switch (sl_get_u_state(sl)) {
	case ASP_WACK_ASPUP:
		mi_timer_cancel(sl->aspup_tack);
		sl_set_u_state(sl, ASP_DOWN);
		sl_set_i_state(sl, LMI_UNATTACHED);
		return lmi_error_ind(sl, q, mp, LMI_ATTACH_REQ, LMI_UNSPEC);
	case ASP_INACTIVE:	/* unsolicited */
		sl_set_u_state(sl, ASP_DOWN);
		sl_set_i_state(sl, LMI_UNATTACHED);
		return lmi_error_ind(sl, q, mp, LMI_UNSPEC, LMI_UNSPEC);
	case ASP_WACK_ASPIA:	/* unsolicited */
		mi_timer_cancel(sl->aspia_tack);
		sl_set_u_state(sl, ASP_DOWN);
		sl_set_i_state(sl, LMI_UNATTACHED);
		return lmi_error_ind(sl, q, mp, LMI_DISABLE_REQ, LMI_UNSPEC);
	case ASP_WACK_ASPAC:	/* unsolicited */
		mi_timer_cancel(sl->aspac_tack);
		sl_set_u_state(sl, ASP_DOWN);
		sl_set_i_state(sl, LMI_UNATTACHED);
		return lmi_error_ind(sl, q, mp, LMI_ENABLE_REQ, LMI_UNSPEC);
	case ASP_ACTIVE:	/* unsolicited */
		sl_set_u_state(sl, ASP_DOWN);
		sl_set_i_state(sl, LMI_UNATTACHED);
		return lmi_error_ind(sl, q, mp, LMI_UNSPEC, LMI_UNSPEC);
	case ASP_WACK_ASPDN:
		mi_timer_cancel(sl->aspdn_tack);
		sl_set_u_state(sl, ASP_DOWN);
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			sl_set_i_state(sl, LMI_UNATTACHED);
			return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_UNSPEC);
		case LMI_DETACH_PENDING:
			sl_set_i_state(sl, LMI_UNATTACHED);
			return lmi_ok_ack(sl, q, mp, LMI_DETACH_REQ);
		}
		break;
	case ASP_DOWN:
		break;
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_asps_hbeat_ack: - process received ASPSP BEAT Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPSP BEAT Ack message (with header)
 */
static int
sl_recv_asps_hbeat_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	mi_timer_cancel(sl->hbeat_tack);
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_aspt_aspac_ack: - process received ASPT ASP Active Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPT ASP Active Ack message (with header)
 */
static int
sl_recv_aspt_aspac_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	switch (sl_get_u_state(sl)) {
	case ASP_WACK_ASPAC:
		mi_timer_cancel(sl->aspac_tack);
		sl_set_u_state(sl, ASP_ACTIVE);
		switch (sl_get_i_state(sl)) {
		case LMI_ENABLE_PENDING:
			sl_set_i_state(sl, LMI_ENABLED);
			return lmi_enable_con(sl, q, mp);
		}
		break;
	case ASP_ACTIVE:
		break;
	case ASP_INACTIVE:
	case ASP_WACK_ASPIA:
	case ASP_WACK_ASPUP:
	case ASP_WACK_ASPDN:
	case ASP_DOWN:
		break;
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_aspt_aspia_ack: - process received ASPT ASP Inactive Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the ASPT ASP Inactive Ack message (with header)
 */
static int
sl_recv_aspt_aspia_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* FIXME: check IID */
	switch (sl_get_u_state(sl)) {
	case ASP_WACK_ASPAC:
	case ASP_ACTIVE:	/* unsolicited */
		mi_timer_cancel(sl->aspac_tack);
		sl_set_u_state(sl, ASP_INACTIVE);
		switch (sl_get_i_state(sl)) {
		case LMI_ENABLE_PENDING:
		case LMI_ENABLED:	/* unsolicited */
			sl_set_i_state(sl, LMI_DISABLED);
			return lmi_error_ind(sl, q, mp, LMI_ENABLE_REQ, LMI_UNSPEC);
		}
		break;
	case ASP_WACK_ASPIA:
		mi_timer_cancel(sl->aspia_tack);
		sl_set_u_state(sl, ASP_INACTIVE);
		switch (sl_get_i_state(sl)) {
		case LMI_ENABLE_PENDING:
			sl_set_i_state(sl, LMI_DISABLED);
			return lmi_error_ind(sl, q, mp, LMI_ENABLE_REQ, LMI_UNSPEC);
		case LMI_DISABLE_PENDING:
			sl_set_i_state(sl, LMI_DISABLED);
			return lmi_disable_con(sl, q, mp);
		}
		break;
	case ASP_INACTIVE:
		break;
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_maup_estab_con: - process received MAUP Establish Confirmation message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Establish Confirmation message (with header)
 *
 * This is simply SL_IN_SERVICE_IND.
 */
static int
sl_recv_maup_estab_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_in_service_ind(sl, q, mp);
}

/**
 * sl_recv_maup_rel_con: - process received MAUP Release Confirmation message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Release Confirmation message (with header)
 *
 * This is discarded (unnecessary).
 */
static int
sl_recv_maup_rel_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_maup_rel_ind: - process received MAUP Release Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Release Indication message (with header)
 *
 * This is simply SL_OUT_OF_SERVICE_IND.
 */
static int
sl_recv_maup_rel_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int reason = 0;

	/* FIXME: get reason from MAUP message */
	return sl_out_of_service_ind(sl, q, mp, reason);
}

/**
 * sl_recv_status_lpo_set: - process STATUS_LPO_SET status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_lpo_set(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_status_lpo_clear: - process STATUS_LPO_CLEAR status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_lpo_clear(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_status_emer_set: - process STATUS_EMER_SET status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_emer_set(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_status_emer_clear: - process STATUS_EMER_CLEAR status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_emer_clear(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_status_flush_buffers: - process STATUS_FLUSH_BUFFERS status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_flush_buffers(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_rb_cleared_ind(sl, q, mp);
}

/**
 * sl_recv_status_continue: - process STATUS_CONTINUE status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_continue(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_status_clear_rtb: - process STATUS_CLEAR_RTB status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_clear_rtb(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_rtb_cleared_ind(sl, q, mp);
}

/**
 * sl_recv_status_audit: - process STATUS_AUDIT status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_audit(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl->audit = 0;
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_status_cong_clear: - process STATUS_CONG_CLEAR status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_cong_clear(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_status_cong_accept: - process STATUS_CONG_ACCEPT status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_cong_accept(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_status_cong_discard: - process STATUS_CONG_DISCARD status
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP status message (with headers)
 */
static int
sl_recv_status_cong_discard(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_maup_state_con: - process received MAUP State Confirmation message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP State Confirmation message (with header)
 *
 * These get translated into one of the other status indications.  There are
 * a couple of such commands that are expected to take some time under SS7
 * such as 
 */
static int
sl_recv_maup_state_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct ua_parm status = { { NULL, }, };

	if (ua_dec_parm(mp, &status, M2UA_PARM_STATE_REQUEST)) {
		switch (status.val) {
		case M2UA_STATUS_LPO_SET:	/* (0x00) */
			return sl_recv_status_lpo_set(sl, q, mp);
		case M2UA_STATUS_LPO_CLEAR:	/* (0x01) */
			return sl_recv_status_lpo_clear(sl, q, mp);
		case M2UA_STATUS_EMER_SET:	/* (0x02) */
			return sl_recv_status_emer_set(sl, q, mp);
		case M2UA_STATUS_EMER_CLEAR:	/* (0x03) */
			return sl_recv_status_emer_clear(sl, q, mp);
		case M2UA_STATUS_FLUSH_BUFFERS:	/* (0x04) */
			return sl_recv_status_flush_buffers(sl, q, mp);
		case M2UA_STATUS_CONTINUE:	/* (0x05) */
			return sl_recv_status_continue(sl, q, mp);
		case M2UA_STATUS_CLEAR_RTB:	/* (0x06) */
			return sl_recv_status_clear_rtb(sl, q, mp);
		case M2UA_STATUS_AUDIT:	/* (0x07) */
			return sl_recv_status_audit(sl, q, mp);
		case M2UA_STATUS_CONG_CLEAR:	/* (0x08) */
			return sl_recv_status_cong_clear(sl, q, mp);
		case M2UA_STATUS_CONG_ACCEPT:	/* (0x09) */
			return sl_recv_status_cong_accept(sl, q, mp);
		case M2UA_STATUS_CONG_DISCARD:	/* (0x0a) */
			return sl_recv_status_cong_discard(sl, q, mp);
		default:
			return (-EOPNOTSUPP);
		}
	}
	return (-EINVAL);
}

/**
 * sl_recv_event_rpo_enter: - process received EVENT_RPO_ENTER event
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message containing all headers
 */
static int
sl_recv_event_rpo_enter(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_remote_processor_outage_ind(sl, q, mp);
}

/**
 * sl_recv_event_rpo_enter: - process received EVENT_RPO_EXIT event
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message containing all headers
 */
static int
sl_recv_event_rpo_exit(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_remote_processor_recovered_ind(sl, q, mp);
}

/**
 * sl_recv_event_lpo_enter: - process received EVENT_LPO_ENTER event
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message containing all headers
 */
static int
sl_recv_event_lpo_enter(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_local_processor_outage_ind(sl, q, mp);
}

/**
 * sl_recv_event_lpo_enter: - process received EVENT_LPO_EXIT event
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message containing all headers
 */
static int
sl_recv_event_lpo_exit(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return sl_local_processor_recovered_ind(sl, q, mp);
}

/**
 * sl_recv_maup_state_ind: - process received MAUP State Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP State Indication message (with header)
 */
static int
sl_recv_maup_state_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct ua_parm event = { { NULL, }, };

	if (ua_dec_parm(mp, &event, M2UA_PARM_STATE_EVENT)) {
		switch (event.val) {
		case M2UA_EVENT_RPO_ENTER:
			return sl_recv_event_rpo_enter(sl, q, mp);
		case M2UA_EVENT_RPO_EXIT:
			return sl_recv_event_rpo_exit(sl, q, mp);
		case M2UA_EVENT_LPO_ENTER:
			return sl_recv_event_lpo_enter(sl, q, mp);
		case M2UA_EVENT_LPO_EXIT:
			return sl_recv_event_lpo_exit(sl, q, mp);
		default:
			return (-EOPNOTSUPP);
		}
	}
	return (-EINVAL);
}

/**
 * sl_recv_maup_retr_con: - process received MAUP Retrieval Confirm message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Retrieval Confirm message (with header)
 */
static int
sl_recv_maup_retr_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct ua_parm action, bsnt;

	if (ua_dec_parm(mp, &action, M2UA_PARM_ACTION))
		switch (action.val) {
		case M2UA_ACTION_RTRV_BSN:
			if (ua_dec_parm(mp, &bsnt, M2UA_PARM_SEQNO))
				return sl_bsnt_ind(sl, q, mp, bsnt.val);
			return (-EINVAL);
		case M2UA_ACTION_RTRV_MSGS:
			/* ignore */
			freemsg(mp);
			return (0);
		default:
			return (-EINVAL);
		}
	return (-EINVAL);
}

/**
 * sl_recv_maup_retr_ind: - process received MAUP Retrieval Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Retrieval Indication message (with header)
 */
static int
sl_recv_maup_retr_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	unsigned char *b_rptr = mp->b_rptr;

	mp->b_rptr += 3 * sizeof(uint32_t);
	if (likely((err = sl_retrieved_message_ind(sl, q, mp)) == 0))
		return (0);
	mp->b_rptr = b_rptr;
	return (err);
}

/**
 * sl_recv_maup_retr_comp_ind: - process received MAUP Retrieval Complete Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Retrieval Complete Indication message (with header)
 */
static int
sl_recv_maup_retr_comp_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	unsigned char *b_rptr = mp->b_rptr;

	mp->b_rptr += 3 * sizeof(uint32_t);
	if (likely((err = sl_retrieval_complete_ind(sl, q, mp)) == 0))
		return (0);
	mp->b_rptr = b_rptr;
	return (err);
}

/**
 * sl_recv_maup_cong_ind: - process received MAUP Congestion Indication message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Congestion Indication message (with header)
 */
static int
sl_recv_maup_cong_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct ua_parm cong, disc;

	if (ua_dec_parm(mp, &cong, M2UA_PARM_CONG_STATUS) &&
	    ua_dec_parm(mp, &disc, M2UA_PARM_DISC_STATUS)) {
		if (cong.val > sl->cong || disc.val > sl->disc)
			return sl_link_congested_ind(sl, q, mp, cong.val, disc.val);
		return sl_link_congestion_ceased_ind(sl, q, mp, cong.val, disc.val);
	}
	return (-EINVAL);
}

/**
 * sl_recv_maup_data: - process received MAUP Data message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Data message (with header)
 */
static inline fastcall int
sl_recv_maup_data(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;

	if (likely((err = sl_pdu_ind(sl, q, mp)) == 0))
		return (0);
	return (err);
}

/**
 * sl_recv_maup_data_ack: - process received MAUP Data Ack message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the MAUP Data Ack message (with header)
 */
static int
sl_recv_maup_data_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore for now */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_rkmm_req_rsp: - process received RKMM REG RSP message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the RKMM REG RSP message (with header)
 */
static int
sl_recv_rkmm_reg_rsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore for now */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_rkmm_dereg_rsp: - process received RKMM DEREG RSP message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the RKMM DEREG RSP message (with header)
 */
static int
sl_recv_rkmm_dereg_rsp(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* ignore for now */
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_err: - process error for received message
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message (M2UA part only)
 * @err: error number
 */
static noinline fastcall int
sl_recv_err(struct sl *sl, queue_t *q, mblk_t *mp, int err)
{
	switch (err) {
	case 0:
	case -EBUSY:
	case -ENOMEM:
	case -ENOBUFS:
	case -ENOSR:
	case -EAGAIN:
	case -EDEADLK:
		break;
	case -EINVAL:		/* unexpected message */
		err = UA_ECODE_UNEXPECTED_MESSAGE;
		goto error;
	case -EMSGSIZE:	/* syntax error */
		err = UA_ECODE_PARAMETER_FIELD_ERROR;
		goto error;
	case -EOPNOTSUPP:	/* unrecongized message type */
		err = UA_ECODE_UNSUPPORTED_MESSAGE_TYPE;
		goto error;
	case -ENOPROTOOPT:	/* unrecognized message class */
		err = UA_ECODE_UNSUPPORTED_MESSAGE_CLASS;
		goto error;
	case -EPROTO:		/* protocol error */
		err = UA_ECODE_PROTOCOL_ERROR;
		goto error;
	default:
		if (err < 0)
			err = UA_ECODE_PROTOCOL_ERROR;
	      error:
		return sl_send_mgmt_err(sl, q, mp, err, mp->b_rptr, mp->b_wptr - mp->b_rptr);
	}
	return (err);
}

/**
 * sl_recv_msg_slow: - process message received from M2UA peer (SG)
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message (just the M2UA part)
 */
static noinline fastcall int
sl_recv_msg_slow(struct sl *sl, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err = -EMSGSIZE;

	if (mp->b_wptr < mp->b_rptr + 2 * sizeof(*p))
		goto error;
	if (mp->b_wptr < mp->b_rptr + ntohl(p[1]))
		goto error;
	switch (UA_MSG_CLAS(p[0])) {
	case UA_CLASS_MGMT:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_MGMT_ERR:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "ERR <-");
			err = sl_recv_mgmt_err(sl, q, mp);
			break;
		case UA_MGMT_NTFY:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "NTFY <-");
			err = sl_recv_mgmt_ntfy(sl, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_ASPS:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_ASPS_HBEAT_REQ:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "BEAT <-");
			err = sl_recv_asps_hbeat_req(sl, q, mp);
			break;
		case UA_ASPS_ASPUP_ACK:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "ASPUP Ack <-");
			err = sl_recv_asps_aspup_ack(sl, q, mp);
			break;
		case UA_ASPS_ASPDN_ACK:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "ASPDN Ack <-");
			err = sl_recv_asps_aspdn_ack(sl, q, mp);
			break;
		case UA_ASPS_HBEAT_ACK:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "BEAT Ack <-");
			err = sl_recv_asps_hbeat_ack(sl, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_ASPT:
		/* FIXME: could probably validate the IID right here. */
		switch (UA_MSG_TYPE(p[0])) {
		case UA_ASPT_ASPAC_ACK:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "ASPAC Ack <-");
			err = sl_recv_aspt_aspac_ack(sl, q, mp);
			break;
		case UA_ASPT_ASPIA_ACK:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "ASPIA Ack <-");
			err = sl_recv_aspt_aspia_ack(sl, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_MAUP:
		/* FIXME: should probably validate the IID and the M2UA common header right here. */
		switch (UA_MSG_TYPE(p[0])) {
		case M2UA_MAUP_ESTAB_CON:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "ESTAB Con <-");
			err = sl_recv_maup_estab_con(sl, q, mp);
			break;
		case M2UA_MAUP_REL_CON:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "REL Con <-");
			err = sl_recv_maup_rel_con(sl, q, mp);
			break;
		case M2UA_MAUP_REL_IND:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "REL Ind <-");
			err = sl_recv_maup_rel_ind(sl, q, mp);
			break;
		case M2UA_MAUP_STATE_CON:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "STATE Con <-");
			err = sl_recv_maup_state_con(sl, q, mp);
			break;
		case M2UA_MAUP_STATE_IND:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "STATE Ind <-");
			err = sl_recv_maup_state_ind(sl, q, mp);
			break;
		case M2UA_MAUP_RETR_CON:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "RETR Con <-");
			err = sl_recv_maup_retr_con(sl, q, mp);
			break;
		case M2UA_MAUP_RETR_IND:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "RETR Ind <-");
			err = sl_recv_maup_retr_ind(sl, q, mp);
			break;
		case M2UA_MAUP_RETR_COMP_IND:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "RETR COMP Ind <-");
			err = sl_recv_maup_retr_comp_ind(sl, q, mp);
			break;
		case M2UA_MAUP_CONG_IND:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "CONG Ind <-");
			err = sl_recv_maup_cong_ind(sl, q, mp);
			break;
		case M2UA_MAUP_DATA:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "DATA <-");
			err = sl_recv_maup_data(sl, q, mp);
			break;
		case M2UA_MAUP_DATA_ACK:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "DATA Ack <-");
			err = sl_recv_maup_data_ack(sl, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_RKMM:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_RKMM_REG_RSP:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "REG Rsp <-");
			err = sl_recv_rkmm_reg_rsp(sl, q, mp);
			break;
		case UA_RKMM_DEREG_RSP:
			strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "DEREG Rsp <-");
			err = sl_recv_rkmm_dereg_rsp(sl, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	default:
		err = (-ENOPROTOOPT);
		break;
	}
	if (err == 0)
		return (0);
      error:
	return sl_recv_err(sl, q, mp, err);
}

/**
 * sl_recv_msg: - process message received from M2UA peer (SG)
 * @sl: private structure
 * @q: active queue (read queue)
 * @mp: the message (just the M2UA part)
 */
static inline fastcall int
sl_recv_msg(struct sl *sl, queue_t *q, mblk_t *mp)
{
	uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	/* fast path for data */
	if (mp->b_wptr >= mp->b_rptr + 2 * sizeof(*p))
		if (mp->b_wptr >= mp->b_rptr + ntohl(p[1]))
			if (UA_MSG_CLAS(p[0]) == UA_CLASS_MAUP)
				if (UA_MSG_TYPE(p[0]) == M2UA_MAUP_DATA) {
					if ((err = sl_recv_maup_data(sl, q, mp)) == 0)
						return (0);
					return sl_recv_err(sl, q, mp, err);
				}
	return sl_recv_msg_slow(sl, q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL User -> SL Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * lmi_info_req: - process LMI_INFO_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
lmi_info_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return lmi_info_ack(sl, q, mp);
}

/**
 * lmi_attach_req: - process LMI_ATTACH_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * A valid LMI_ATTACH_REQ occurs when the underlying SCTP stream is not yet bound.  The
 * LMI_ATTACH_REQ primitive must somehow specify the local address (could be simply a wildcard I
 * suppose) so it is placed in the ppa address space.  As a result of the attach operation, initiate
 * an N_BIND_REQ operation on the underlying SCTP stream and wait for the N_BIND_ACK.  When an
 * N_BIND_ACK or N_ERROR_ACK arrives, either acknowledge with an LMI_OK_ACK or generate an
 * LMI_ERROR_ACK primitive.
 */
static int
lmi_attach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_UNATTACHED)
		goto outstate;
	if (!MBLKIN(mp, p->lmi_ppa_offset, p->lmi_ppa_length))
		goto badppa;

	/* FIXME: the first 4 bytes of the PPA should be an ASPID (set to zero when ASPID not
	   required) and the remainder should be a sockaddr_storage structure, or just an ASPID, or 
	   nothing at all.  Also, if the N provider is already bound, complete the operation.  If
	   the N provider is already connected, send an ASP Up request with the ASPID from the
	   attach request. */
	switch (sl_get_n_state(sl)) {
	case NS_UNBND:
		sl_set_i_state(sl, LMI_ATTACH_PENDING);
		return n_bind_req(sl, q, mp, (caddr_t) (mp->b_rptr + p->lmi_ppa_offset),
				  p->lmi_ppa_length);
	case NS_IDLE:
		sl_set_i_state(sl, LMI_ATTACH_PENDING);
		return lmi_ok_ack(sl, q, mp, LMI_ATTACH_REQ);
	case NS_WCON_CREQ:
	case NS_WACK_CRES:
	case NS_DATA_XFER:
		/* FIXME: send an ASP Up first. */
		sl_set_i_state(sl, LMI_ATTACH_PENDING);
		return lmi_ok_ack(sl, q, mp, LMI_ATTACH_REQ);
	}
	goto outstate;
      badppa:
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_BADPPA);
      tooshort:
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_OUTSTATE);
}

/**
 * lmi_detach_req: - process LMI_DETACH_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * A valid LMI_DETACH_REQ occurs when the underlying SCTP stream is bound but idle. As a result of
 * the detach operation, initiate an N_UNBIND_REQ operation on the underlying SCTP stream and wait
 * for the N_OK_ACK.  When an N_OK_ACK or N_ERROR_ACK arrives, either acknowledge wtih an LMI_OK_ACK
 * or generate an LMI_ERROR_ACK primitive.
 *
 * When detaching, it might not be necessary to unbind the SCTP stream.  It is also not necessary to
 * disconnect the SCTP stream.  We could simply perform an ASP Down procedure if the SCTP
 * association is connected.
 */
static int
lmi_detach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_DISABLED)
		goto outstate;
	sl_set_i_state(sl, LMI_DETACH_PENDING);
	switch (sl_get_n_state(sl)) {
	case NS_UNBND:
		return lmi_ok_ack(sl, q, mp, LMI_DETACH_REQ);
	case NS_IDLE:
	default:
		return n_unbind_req(sl, q, mp);
	case NS_DATA_XFER:
		return sl_send_asps_aspdn_req(sl, q, mp, &sl->aspid, NULL, 0);
	}
	goto outstate;
      tooshort:
	return lmi_error_ack(sl, q, mp, LMI_DETACH_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_DETACH_REQ, LMI_OUTSTATE);
}

/**
 * lmi_enable_req: - process LMI_ENABLE_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * A valid LMI_ENABLE_REQ occurs when the underlying SCTP stream is idle.  As a resul of the
 * enableoperation, initiate an N_CONN_REQ operation on the underlying SCTP stream and wait fro the
 * N_ERROR_ACK or N_CONN_CON or N_DISCON_IND.  When an N_CONN_CON, N_ERROR_ACK or N_DISCON_IND
 * arrives, either akcnowledge with an LMI_OK_ACK and LMI_ENABLE_CON or generate an LMI_ERROR_ACK.
 */
static int
lmi_enable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_DISABLED)
		goto outstate;
	if (!MBLKIN(mp, p->lmi_rem_offset, p->lmi_rem_length))
		goto badaddr;
	sl_set_i_state(sl, LMI_ENABLE_PENDING);

	/* FIXME: the first 4 bytes of the remote should be an IID and the remainder should be a
	   sockaddr_storage structure, or just an IID, or nothing at all.  If the N provider state
	   is already connected, complete the procedure as though an N_CONN_CON was received.  This
	   must also consist of sending an ASP Up request with the ASPID to which the stream was
	   attached, and an ASP Active message for the rfor the IID that was enabled. */

	switch (sl_get_n_state(sl)) {
	case NS_IDLE:
	default:
	{
		caddr_t aptr;
		size_t alen;

		if (MBLKIN(mp, p->lmi_rem_offset, sizeof(uint32_t))) {
			sl->iid = *(uint32_t *) (mp->b_rptr + p->lmi_rem_offset);
			aptr = (caddr_t) (mp->b_rptr + p->lmi_rem_offset + sizeof(uint32_t));
			alen = p->lmi_rem_length - sizeof(uint32_t);
		} else {
			sl->iid = 0;
			aptr = NULL;
			alen = 0;
		}
		return n_conn_req(sl, q, mp, aptr, alen);
	}
	case NS_DATA_XFER:
		if (sl_get_u_state(sl) == ASP_DOWN) {
			uint32_t *aspid;

			sl_set_u_state(sl, ASP_WACK_ASPUP);
			aspid = (sl->aspid) ? &sl->aspid : NULL;
			return sl_send_asps_aspup_req(sl, q, mp, aspid, NULL, 0);
		}
		if (sl_get_u_state(sl) == ASP_INACTIVE) {
			uint32_t *iid;

			if (MBLKIN(mp, p->lmi_rem_offset, sizeof(uint32_t))) {
				iid = (sl->iid =
				       *(uint32_t *) (mp->b_rptr +
						      p->lmi_rem_offset)) ? &sl->iid : NULL;
			} else {
				iid = NULL;
			}
			sl_set_u_state(sl, ASP_WACK_ASPAC);
			return sl_send_aspt_aspac_req(sl, q, mp, sl->tm, iid, 1, NULL, 0);
		}
		return lmi_enable_con(sl, q, mp);
	}
	goto outstate;
      badaddr:
	return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_BADADDRESS);
      tooshort:
	return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_OUTSTATE);
}

/**
 * lmi_disable_req: - process LMI_DISABLE_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * A valid LMI_DISABLE_REQ occurs when the underlying SCTP stream is connected.  As a result of the
 * disable operation, initiate an N_DISCON_REQ operation on the underlying SCTP stream and wait for
 * the N_OK_ACK or N_ERROR_ACK primitive.  When an N_OK_ACK or N_ERROR_ACK arrives, either
 * acknowledge with an LMI_OK_ACK and LMI_DISABLE_CON or generate an LMI_ERROR_ACK.
 *
 * It might not be necessary whatsoever to disconnect the SCTP association when disabling.  When
 * disabling, we can simply perform an ASP Inactive procedure for the IID for which we are enabled.
 */
static int
lmi_disable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	sl_set_i_state(sl, LMI_DISABLE_PENDING);
	switch (sl_get_n_state(sl)) {
	case NS_UNBND:
	case NS_IDLE:
	default:
		return lmi_disable_con(sl, q, mp);
	case NS_DATA_XFER:
		return sl_send_aspt_aspia_req(sl, q, mp, &sl->iid, 1, NULL, 0);
	}

	return n_discon_req(sl, q, mp, N_UNDEFINED);
      tooshort:
	return lmi_error_ack(sl, q, mp, LMI_DISABLE_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_DISABLE_REQ, LMI_OUTSTATE);
}

/**
 * sl_pdu_req: - process SL_PDU_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static inline fastcall __hot_write int
sl_pdu_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	if ((dp = mp->b_cont) == NULL)
		goto badprim;
	if (1)
		err = sl_send_maup_data1(sl, q, mp, sl->iid, dp);
	else
		err = sl_send_maup_data2(sl, q, mp, sl->iid, dp, p->sl_mp);
	if (err == 0)
		freeb(mp);
	return (err);
      badprim:
	return lmi_error_ack(sl, q, mp, SL_PDU_REQ, LMI_BADPRIM);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_PDU_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_PDU_REQ, LMI_OUTSTATE);
}

/**
 * sl_emergency_req: - process SL_EMERGENCY_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_emergency_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_EMER_SET);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_EMERGENCY_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_EMERGENCY_REQ, LMI_OUTSTATE);
}

/**
 * sl_emergency_ceases_req: - process SL_EMERGENCY_CEASES_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_emergency_ceases_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_EMER_CLEAR);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_EMERGENCY_CEASES_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_EMERGENCY_CEASES_REQ, LMI_OUTSTATE);
}

/**
 * sl_start_req: - process SL_START_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_start_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_estab_req(sl, q, mp, sl->iid);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_START_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_START_REQ, LMI_OUTSTATE);
}

/**
 * sl_stop_req: - process SL_STOP_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_stop_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_rel_req(sl, q, mp, sl->iid);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_STOP_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_STOP_REQ, LMI_OUTSTATE);
}

/**
 * sl_retrieve_bsnt_req: - process SL_RETRIEVE_BSNT_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_retrieve_bsnt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_retr_req(sl, q, mp, sl->iid, NULL);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_OUTSTATE);
}

/**
 * sl_retrieval_request_and_fsnc_req: - process SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_retrieval_request_and_fsnc_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_retr_req(sl, q, mp, sl->iid, &p->sl_fsnc);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_OUTSTATE);
}

/**
 * sl_continue_req: - process SL_CONTINUE_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_continue_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_continue_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_CONTINUE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CONTINUE_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CONTINUE_REQ, LMI_OUTSTATE);
}

/**
 * sl_clear_buffers_req: - process SL_CLEAR_BUFFERS_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_clear_buffers_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_FLUSH_BUFFERS);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_OUTSTATE);
}

/**
 * sl_clear_rtb_req: - process SL_CLEAR_RTB_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_clear_rtb_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_CLEAR_RTB);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_RTB_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_RTB_REQ, LMI_OUTSTATE);
}

/**
 * sl_local_processor_outage_req: - process SL_LOCAL_PROCESSOR_OUTAGE_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_local_processor_outage_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_LPO_SET);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_LOCAL_PROCESSOR_OUTAGE_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_LOCAL_PROCESSOR_OUTAGE_REQ, LMI_OUTSTATE);
}

/**
 * sl_resume_req: - process SL_RESUME_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_resume_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_LPO_CLEAR);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_RESUME_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_RESUME_REQ, LMI_OUTSTATE);
}

/**
 * sl_congestion_discard_req: - process SL_CONGESTION_DISCARD_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_congestion_discard_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_cong_discard_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_CONG_DISCARD);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_OUTSTATE);
}

/**
 * sl_congestion_accept_req: - process SL_CONGESTION_ACCEPT_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_congestion_accept_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_cong_accept_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_CONG_ACCEPT);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_OUTSTATE);
}

/**
 * sl_congestion_req: - process SL_NO_CONGESTION_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_no_congestion_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_no_cong_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, sl->iid, M2UA_STATUS_CONG_CLEAR);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_NO_CONGESTION_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_NO_CONGESTION_REQ, LMI_OUTSTATE);
}

/**
 * sl_power_on_req: - process SL_POWER_ON_REQ primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_power_on_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	/* There is simply no way to do this with M2UA. */
	freemsg(mp);
	return (0);
}

/**
 * sl_other_req: - process unknown primitive
 * @sl: SL private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sl_other_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return lmi_error_ack(sl, q, mp, *(uint32_t *) mp->b_rptr, LMI_BADPRIM);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timeouts
 *
 *  -------------------------------------------------------------------------
 */
/**
 * sl_aspup_tack_timeout: - timeout awaiting ASP Up Ack
 * @sl: SL private structure
 * @q: active queue (read queue)
 */
static int
sl_aspup_tack_timeout(struct sl *sl, queue_t *q)
{
	switch (sl_get_u_state(sl)) {
	case ASP_WACK_ASPUP:
	{
		uint32_t *aspid;

		aspid = sl->aspid ? &sl->aspid : NULL;
		return sl_send_asps_aspup_req(sl, q, NULL, aspid, NULL, 0);
	}
	default:
		break;
	}
	return (0);
}

/**
 * sl_aspac_tack_timeout: - timeout awaiting ASP Active Ack
 * @sl: SL private structure
 * @q: active queue (read queue)
 */
static int
sl_aspac_tack_timeout(struct sl *sl, queue_t *q)
{
	switch (sl_get_u_state(sl)) {
	case ASP_WACK_ASPAC:
	{
		uint32_t *iid;

		iid = sl->iid ? &sl->iid : NULL;
		return sl_send_aspt_aspac_req(sl, q, NULL, sl->tm, iid, 1, NULL, 0);
	}
	default:
		break;
	}
	return (0);
}

/**
 * sl_aspdn_tack_timeout: - timeout awaiting ASP Down Ack
 * @sl: SL private structure
 * @q: active queue (read queue)
 */
static int
sl_aspdn_tack_timeout(struct sl *sl, queue_t *q)
{
	switch (sl_get_u_state(sl)) {
	case ASP_WACK_ASPDN:
	{
		uint32_t *aspid;

		aspid = sl->aspid ? &sl->aspid : NULL;
		return sl_send_asps_aspdn_req(sl, q, NULL, aspid, NULL, 0);
	}
	default:
		break;
	}
	return (0);
}

/**
 * sl_aspia_tack_timeout: - timeout awaiting ASP Inactive Ack
 * @sl: SL private structure
 * @q: active queue (read queue)
 */
static int
sl_aspia_tack_timeout(struct sl *sl, queue_t *q)
{
	switch (sl_get_u_state(sl)) {
	case ASP_WACK_ASPIA:
	{
		uint32_t *iid;

		iid = sl->iid ? &sl->iid : NULL;
		return sl_send_aspt_aspia_req(sl, q, NULL, iid, 1, NULL, 0);
	}
	default:
		break;
	}
	return (0);
}

/**
 * sl_hbeat_tack_timeout: - timeout awaiting Heartbeat Ack
 * @sl: SL private structure
 * @q: active queue (read queue)
 */
static int
sl_hbeat_tack_timeout(struct sl *sl, queue_t *q)
{
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  N Provider -> N User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * n_conn_ind: - process N_CONN_IND primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 * If we get a connection indication, we pretty much have to remember it.
 * Another possibility is issuing an LMI_ENABLE_IND upstream.
 */
static int
n_conn_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_conn_con: - process N_CONN_CON primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 * When we get a valid connection confirmation it is a confirmation of our
 * attempt to connect that was part of the LMI_ENABLE_REQ operation.  Respond
 * with an LMI_OK_ACK and LMI_ENABLE_CON primitive.
 */
static int
n_conn_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_n_state(sl) != NS_WCON_CREQ)
		goto outstate;
	sl_set_n_state(sl, NS_DATA_XFER);
	if (sl_get_i_state(sl) != LMI_ENABLE_PENDING)
		goto outstate;
	sl_set_i_state(sl, LMI_ENABLED);
	return lmi_enable_con(sl, q, mp);
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_discon_ind: - process N_DISCON_IND primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 * A disconnect indication corresponds to a disable indication.
 */
static int
n_discon_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_n_state(sl) != NS_DATA_XFER)
		goto outstate;
	switch (sl_get_i_state(sl)) {
	case LMI_ENABLED:
	case LMI_ENABLE_PENDING:
		sl_set_i_state(sl, LMI_DISABLED);
		sl_set_n_state(sl, NS_IDLE);
		return lmi_error_ind(sl, q, mp, 0, true);
	}
	goto outstate;
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_data_ind: - process N_DATA_IND primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static inline fastcall __hot_in int
n_data_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if ((err = sl_recv_msg(sl, q, dp)) == 0)
		freeb(mp);
	return (err);
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_exdata_ind: - process N_EXDATA_IND primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_exdata_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if ((err = sl_recv_msg(sl, q, dp)) == 0)
		freeb(mp);
	return (err);
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_info_ack: - process N_INFO_ACK primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_info_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_bind_ack: - process N_BIND_ACK primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 * An N_BIND_ACK is returned as a result of an LMI_ATTACH_REQ operation.
 */
static int
n_bind_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_n_state(sl) != NS_WACK_BREQ)
		goto outstate;
	sl_set_n_state(sl, NS_IDLE);
	if (sl_get_i_state(sl) != LMI_ATTACH_PENDING)
		goto outstate;
	sl_set_i_state(sl, LMI_DISABLED);
	return lmi_ok_ack(sl, q, mp, LMI_ATTACH_REQ);
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_error_ack: - process N_ERROR_ACK primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_error_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (sl_get_n_state(sl)) {
	case NS_WACK_BREQ:
		sl_set_n_state(sl, NS_UNBND);
		if (sl_get_i_state(sl) != LMI_ATTACH_PENDING)
			goto outstate;
		sl_set_i_state(sl, LMI_UNATTACHED);
		return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_BADPPA);
	case NS_WACK_UREQ:
		sl_set_n_state(sl, NS_IDLE);
		if (sl_get_i_state(sl) != LMI_DETACH_PENDING)
			goto outstate;
		sl_set_i_state(sl, LMI_DISABLED);
		return lmi_error_ack(sl, q, mp, LMI_DETACH_REQ, LMI_UNSPEC);
	case NS_WCON_CREQ:
		sl_set_n_state(sl, NS_IDLE);
		if (sl_get_i_state(sl) != LMI_ENABLE_PENDING)
			goto outstate;
		sl_set_i_state(sl, LMI_DISABLED);
		return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_UNSPEC);
	case NS_WACK_DREQ6:
		sl_set_n_state(sl, NS_WCON_CREQ);
		goto disconn;
	case NS_WACK_DREQ7:
		sl_set_n_state(sl, NS_WRES_CIND);
		goto disconn;
	case NS_WACK_DREQ9:
		sl_set_n_state(sl, NS_DATA_XFER);
		goto disconn;
	case NS_WACK_DREQ10:
		sl_set_n_state(sl, NS_WCON_RREQ);
		goto disconn;
	case NS_WACK_DREQ11:
		sl_set_n_state(sl, NS_WRES_RIND);
		goto disconn;
	      disconn:
		if (sl_get_i_state(sl) != LMI_DISABLE_PENDING)
			goto outstate;
		sl_set_i_state(sl, LMI_ENABLED);
		return lmi_error_ack(sl, q, mp, LMI_DISABLE_REQ, LMI_UNSPEC);
	}
	goto outstate;
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_ok_ack: - process N_OK_ACK primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_ok_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (sl_get_n_state(sl)) {
	case NS_WACK_UREQ:
		sl_set_n_state(sl, NS_UNBND);
		if (sl_get_i_state(sl) != LMI_DETACH_PENDING)
			goto outstate;
		sl_set_i_state(sl, LMI_UNATTACHED);
		return lmi_ok_ack(sl, q, mp, LMI_DETACH_REQ);
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		sl_set_n_state(sl, NS_IDLE);
		if (sl_get_i_state(sl) != LMI_DISABLE_PENDING)
			goto outstate;
		sl_set_i_state(sl, LMI_DISABLED);
		return lmi_ok_ack(sl, q, mp, LMI_DISABLE_REQ);
	}
	goto outstate;
      outstate:
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_unitdata_ind: - process N_UNITDATA_IND primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_unitdata_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_uderror_ind: - process N_UDERROR_IND primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_uderror_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_datack_ind: - process N_DATACK_IND primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_datack_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_reset_ind: - process N_RESET_IND primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_reset_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_reset_con: - process N_RESET_CON primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_reset_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
      tooshort:
	freemsg(mp);
	return (0);
}

/**
 * n_other_ind: - process unknown primitive
 * @sl: SL private structure
 * @q: active queue (read queue)
 * @mp: the primitive
 */
static int
n_other_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL User -> SL Provider Message Handling
 *
 *  -------------------------------------------------------------------------
 */
static inline fastcall __hot_write int
sl_w_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	return sl_send_maup_data1(sl, q, NULL, sl->iid, mp);
}
static noinline fastcall __unlikely int
sl_w_data_slow(queue_t *q, mblk_t *mp)
{
	return sl_w_data(q, mp);
}
static int
sl_w_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int old_i_state, old_n_state, old_u_state;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t))
		return lmi_error_ack(sl, q, mp, -1, LMI_TOOSHORT);

	if (!sl_trylock(sl, q))
		return (-EDEADLK);

	old_i_state = sl_get_i_state(sl);
	old_n_state = sl_get_n_state(sl);
	old_u_state = sl_get_u_state(sl);

	switch (*(uint32_t *) mp->b_rptr) {
		/* Local management primitives */
	case LMI_INFO_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_INFO_REQ");
		rtn = lmi_info_req(sl, q, mp);
		break;
	case LMI_ATTACH_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_ATTACH_REQ");
		rtn = lmi_attach_req(sl, q, mp);
		break;
	case LMI_DETACH_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_DETACH_REQ");
		rtn = lmi_detach_req(sl, q, mp);
		break;
	case LMI_ENABLE_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_ENABLE_REQ");
		rtn = lmi_enable_req(sl, q, mp);
		break;
	case LMI_DISABLE_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_DISABLE_REQ");
		rtn = lmi_disable_req(sl, q, mp);
		break;
		/* Signalling Link Primitives */
	case SL_PDU_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_PDU_REQ");
		rtn = sl_pdu_req(sl, q, mp);
		break;
	case SL_EMERGENCY_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_EMERGENCY_REQ");
		rtn = sl_emergency_req(sl, q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_EMERGENCY_CEASES_REQ");
		rtn = sl_emergency_ceases_req(sl, q, mp);
		break;
	case SL_START_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_START_REQ");
		rtn = sl_start_req(sl, q, mp);
		break;
	case SL_STOP_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_STOP_REQ");
		rtn = sl_stop_req(sl, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_RETRIEVE_BSNT_REQ");
		rtn = sl_retrieve_bsnt_req(sl, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_RETRIEVAL_REQUEST_AND_FSNC_REQ");
		rtn = sl_retrieval_request_and_fsnc_req(sl, q, mp);
		break;
	case SL_CONTINUE_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CONTINUE_REQ");
		rtn = sl_continue_req(sl, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CLEAR_BUFFERS_REQ");
		rtn = sl_clear_buffers_req(sl, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CLEAR_RTB_REQ");
		rtn = sl_clear_rtb_req(sl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_LOCAL_PROCESSOR_OUTAGE_REQ");
		rtn = sl_local_processor_outage_req(sl, q, mp);
		break;
	case SL_RESUME_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_RESUME_REQ");
		rtn = sl_resume_req(sl, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CONGESTION_DISCARD_REQ");
		rtn = sl_congestion_discard_req(sl, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CONGESTION_ACCEPT_REQ");
		rtn = sl_congestion_accept_req(sl, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_NO_CONGESTION_REQ");
		rtn = sl_no_congestion_req(sl, q, mp);
		break;
	case SL_POWER_ON_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_POWER_ON_REQ");
		rtn = sl_power_on_req(sl, q, mp);
		break;
	default:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_????_???");
		rtn = sl_other_req(sl, q, mp);
		break;
	}
	if (rtn < 0) {
		sl_set_i_state(sl, old_i_state);
		sl_set_n_state(sl, old_n_state);
		sl_set_u_state(sl, old_u_state);
	}
	sl_unlock(sl);
	return (rtn);
}
static int
sl_w_flush(queue_t *q, mblk_t *mp)
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
static int
sl_w_ioctl(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static int
sl_w_iocdata(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static int
sl_w_other(queue_t *q, mblk_t *mp)
{
	/* pass unprocessed message types along */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
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
static inline fastcall int
sl_w_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return sl_w_data(q, mp);
	if (DB_TYPE(mp) == M_PROTO)
		if (mp->b_wptr >= mp->b_rptr + sizeof(uint32_t))
			if (*(uint32_t *) mp->b_rptr == SL_PDU_REQ)
				return sl_pdu_req(SL_PRIV(q), q, mp);
	return sl_w_msg_slow(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NS Provider -> NS User Message Handling
 *
 *  -------------------------------------------------------------------------
 */
static inline fastcall __hot_in int
sl_r_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	return sl_pdu_ind(sl, q, mp);
}
static noinline fastcall __unlikely int
sl_r_data_slow(queue_t *q, mblk_t *mp)
{
	return sl_r_data(q, mp);
}
static int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int old_i_state, old_n_state, old_u_state;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t)) {
		freemsg(mp);
		return (0);
	}
	if (!sl_trylock(sl, q))
		return (-EDEADLK);

	old_i_state = sl_get_i_state(sl);
	old_n_state = sl_get_n_state(sl);
	old_u_state = sl_get_u_state(sl);

	switch (*(uint32_t *) mp->b_rptr) {
	case N_CONN_IND:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_CONN_IND <-");
		rtn = n_conn_ind(sl, q, mp);
		break;
	case N_CONN_CON:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_CONN_CON <-");
		rtn = n_conn_con(sl, q, mp);
		break;
	case N_DISCON_IND:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_DISCON_IND <-");
		rtn = n_discon_ind(sl, q, mp);
		break;
	case N_DATA_IND:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_DATA_IND <-");
		rtn = n_data_ind(sl, q, mp);
		break;
	case N_EXDATA_IND:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_EXDATA_IND <-");
		rtn = n_exdata_ind(sl, q, mp);
		break;
	case N_INFO_ACK:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_INFO_ACK <-");
		rtn = n_info_ack(sl, q, mp);
		break;
	case N_BIND_ACK:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_BIND_ACK <-");
		rtn = n_bind_ack(sl, q, mp);
		break;
	case N_ERROR_ACK:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_ERROR_ACK <-");
		rtn = n_error_ack(sl, q, mp);
		break;
	case N_OK_ACK:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_OK_ACK <-");
		rtn = n_ok_ack(sl, q, mp);
		break;
	case N_UNITDATA_IND:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_UNITDATA_IND <-");
		rtn = n_unitdata_ind(sl, q, mp);
		break;
	case N_UDERROR_IND:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_UDERROR_IND <-");
		rtn = n_uderror_ind(sl, q, mp);
		break;
	case N_DATACK_IND:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_DATACK_IND <-");
		rtn = n_datack_ind(sl, q, mp);
		break;
	case N_RESET_IND:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_RESET_IND <-");
		rtn = n_reset_ind(sl, q, mp);
		break;
	case N_RESET_CON:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_RESET_CON <-");
		rtn = n_reset_con(sl, q, mp);
		break;
	default:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "N_????_??? <-");
		rtn = n_other_ind(sl, q, mp);
		break;
	}
	if (rtn < 0) {
		sl_set_i_state(sl, old_i_state);
		sl_set_n_state(sl, old_n_state);
		sl_set_u_state(sl, old_u_state);
	}
	sl_unlock(sl);
	return (rtn);
}
static int
sl_r_flush(queue_t *q, mblk_t *mp)
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
static int
sl_r_sig(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int rtn;

	if (unlikely(!mi_timer_valid(mp)))
		return (0);

	if (unlikely(!sl_trylock(sl, q)))
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);

	switch (*(int *) mp->b_rptr) {
	case 1:
		strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> ASP Up TIMEOUT <-");
		rtn = sl_aspup_tack_timeout(sl, q);
		break;
	case 2:
		strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> ASP Active TIMEOUT <-");
		rtn = sl_aspac_tack_timeout(sl, q);
		break;
	case 3:
		strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> ASP Down TIMEOUT <-");
		rtn = sl_aspdn_tack_timeout(sl, q);
		break;
	case 4:
		strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> ASP Inactive TIMEOUT <-");
		rtn = sl_aspia_tack_timeout(sl, q);
		break;
	case 5:
		strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> Heartbeat TIMEOUT <-");
		rtn = sl_hbeat_tack_timeout(sl, q);
		break;
	default:
		strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> ??? TIMEOUT <-");
		rtn = 0;
		break;
	}
	sl_unlock(sl);
	if (rtn != 0)
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	return (rtn);
}
static int
sl_r_other(queue_t *q, mblk_t *mp)
{
	/* pass unprocessed message types along */
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
sl_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sl_r_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(q, mp);
	case M_FLUSH:
		return sl_r_flush(q, mp);
	case M_SIG:
	case M_PCSIG:
		return sl_r_sig(q, mp);
	default:
		return sl_r_other(q, mp);
	}
}
static inline fastcall int
sl_r_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return sl_r_data(q, mp);
	if (DB_TYPE(mp) == M_PROTO)
		if (mp->b_wptr >= mp->b_rptr + sizeof(uint32_t))
			if (*(uint32_t *) mp->b_rptr == N_DATA_IND)
				return n_data_ind(SL_PRIV(q), q, mp);
	return sl_r_msg_slow(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Put and Service Procedures
 *
 *  -------------------------------------------------------------------------
 */
static streamscall __hot_in int
sl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sl_r_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_read int
sl_rsrv(queue_t *q)
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
static streamscall __hot_write int
sl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sl_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
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

/*
 *  -------------------------------------------------------------------------
 *
 *  Open and Close Routines
 *
 *  -------------------------------------------------------------------------
 */
static caddr_t sl_opens = NULL;

static streamscall __unlikely int
sl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct sl *sl;
	mblk_t *mp, *tp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */

	while (!(mp = allocb(sizeof(N_info_req_t), BPRI_WAITOK))) ;

	if ((err = mi_open_comm(&sl_opens, sizeof(*sl), q, devp, oflags, sflag, crp)))
		return (err);

	sl = SL_PRIV(q);
	/* initialize the structure */

	/* allocate timers */
	if (!(tp = sl->aspup_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 1;
	if (!(tp = sl->aspac_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 2;
	if (!(tp = sl->aspdn_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 3;
	if (!(tp = sl->aspia_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 4;
	if (!(tp = sl->hbeat_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 5;

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((N_info_req_t *) mp->b_wptr)->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(N_info_req_t);
	qprocson(q);
	putnext(q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto error;
      error:
	freemsg(mp);
	/* cancel all timers */
	if ((tp = xchg(&sl->aspup_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&sl->aspdn_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&sl->aspac_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&sl->aspia_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&sl->hbeat_tack, NULL)))
		mi_timer_free(tp);
	mi_close_comm(&sl_opens, q);
	return (err);
}
static streamscall __unlikely int
sl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *tp;

	qprocsoff(q);

	/* cancel all timers */
	if ((tp = xchg(&sl->aspup_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&sl->aspdn_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&sl->aspac_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&sl->aspia_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&sl->hbeat_tack, NULL)))
		mi_timer_free(tp);

	mi_close_comm(&sl_opens, q);
	return (0);
}

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the M2UA-AS module. (0 for allocation.)");

static struct qinit sl_rinit = {
	.qi_putp = sl_rput,		/* Read put (message from below) */
	.qi_srvp = sl_rsrv,		/* Read queue service */
	.qi_qclose = sl_qclose,		/* Each open */
	.qi_qopen = sl_qopen,		/* Last close */
	.qi_minfo = &sl_minfo,		/* Information */
	.qi_mstat = &sl_mstat,		/* Statistics */
};

static struct qinit sl_winit = {
	.qi_putp = sl_wput,		/* Write put (message from above) */
	.qi_srvp = sl_wsrv,		/* Write queue service */
	.qi_minfo = &sl_minfo,		/* Information */
	.qi_mstat = &sl_mstat,		/* Statistics */
};

static struct streamtab m2ua_asinfo = {
	.st_rdinit = &sl_rinit,		/* Upper read queue */
	.st_wrinit = &sl_winit,		/* Upper write queue */
};

#ifdef LIS
#define fmodsw _fmodsw
#endif

static struct fmodsw m2_fmod = {
	.f_name = MOD_NAME,
	.f_str = &m2ua_asinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
m2ua_asinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_SPLASH);
	if ((err = register_strmod(&m2_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module id %d\n", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
m2ua_asexit(void)
{
	int err;

	if ((err = unregister_strmod(&m2_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d\n", MOD_NAME, err);
	return;
}

module_init(m2ua_asinit);
module_exit(m2ua_asexit);
