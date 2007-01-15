/*****************************************************************************

 @(#) $RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:58:42 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/01/15 11:58:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m2ua_as.c,v $
 Revision 0.9.2.1  2007/01/15 11:58:42  brian
 - added new m2ua-as mux files

 *****************************************************************************/

#ident "@(#) $RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:58:42 $"

static char const ident[] = "$RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:58:42 $";

/*
 *  This is an M2UA multiplexing driver.  It is necessary to use a multiplexing driver because most
 *  SIGTRAN UAs require that there only be one association between an ASP and an SGP.  Therefore,
 *  one SCTP association between the ASP and the SG needs to be used to support all of the
 *  Signalling Links provided by that SG to the ASP regardless of how many independent AS there are.
 *
 *  It works like this: the lower N minor device numbers represent N ASPs.  Opening a one of these N
 *  minor devices numbers will clone a new Stream.  That Stream is associated with the same ASP for
 *  the entire time that it is open.  Each Stream opened on an ASP in this fashion represents an AS
 *  and provides the OpenSS7 Signalling Link Interface (SLI).  The Stream, when freshly opened, is
 *  merely associated with the ASP and specific M2UA input-output controls can be used to configure
 *  and control the ASP.
 *
 *  The application or a configuration daemon is responsible for opening ASP Streams and linking
 *  SCTP Streams beneath the ASP Stream.  These could be private associations or public ones.
 *  Private associations are I_LINK'ed; public ones I_PLINK'ed.  Input-output controls are used to
 *  describe the configuration elements associated with the SCTP association.  SCTP associations do
 *  not necessarily even need to be in an bound or connected state when linked.  M2UA input-output
 *  control commands can be used to define the local and remote addresses and the driver can attempt
 *  to form the association on demand.  If the SCTP association is in the connected state, it can be
 *  used immediately and configuring local and remote addresses is perhaps not necessary.
 *
 *  It would be best to use a specialized configuration program or daemon to perform the opening and
 *  linking of SCTP Streams beneath the multiplexer because this is not a normal local management
 *  function that can be performed by the SLI interface.
 *
 *  ASPs can either define the PPA to IID mapping that will be used by AS Streams, or PPAs can be
 *  registered with the SG on demand (if the SG supports dynamic registration).  When an AS Stream
 *  is opened on the ASP an attached to a PPA, the attachment either invokes the registration
 *  process, or the attachment is performed internal to the driver (using the pre-defined PPA to IID
 *  mapping);  detaching disassociates the PPA with the IID or deregisters the PPA.  Enabling an SLI
 *  Stream results in activating the AS; disabling results in deactivation.  Attaching, detaching,
 *  enabling and disabling interfaces are a normal part of the SLI function and the M2UA actions are
 *  performed completely transparent to the SLI interface.  The SLI user does not need to be aware
 *  that the service is provided by M2UA instead of a direct local attachment to the device driver.
 *
 *  Mapping of SLI service primitives from AS to SCTP association is performed internal to the ASP
 *  data structures.
 *
 *  Note that this is just the AS side of M2UA.  The SG side is more complicated and will be handled
 *  by a separate multiplexing driver.  Perhaps at some point we can merge the M2UA-AS mux and the
 *  M2UA-SG mux into a single multiplexing driver, which was the original design approach; however,
 *  it might not be necessary until we want an AS to also act as an SG.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#define _DEBUG	1
//#undef        _DEBUG

#include <ss7/os7/compat.h>
#include <sys/strsun.h>

#undef mi_timer
#define mi_timer mi_timer_MAC

#ifdef DB_TYPE
#define DB_TYPE(mp) mp->b_datap->db_type
#endif				/* DB_TYPE */

#include <linux/socket.h>
#include <net/ip.h>

#include <sys/tpi.h>
#include <sys/tpi_sctp.h>	/* For talking to SCTP. */

#include <sys/dlpi.h>		/* For data link states. */

#include <sys/lmi.h>
#include <sys/lmi_ioctl.h>

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

/* ========================== */

#define M2UA_MUX_DESCRIP	"M2UA/SCTP SIGNALLING LINK (SL) STREAMS MULTIPLEXING DRIVER."
#define M2UA_MUX_REVISION	"OpenSS7 $RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/15 11:58:42 $"
#define M2UA_MUX_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define M2UA_MUX_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define M2UA_MUX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define M2UA_MUX_LICENSE	"GPL"
#define M2UA_MUX_BANNER		M2UA_MUX_DESCRIP	"\n" \
				M2UA_MUX_REVISION	"\n" \
				M2UA_MUX_COPYRIGHT	"\n" \
				M2UA_MUX_DEVICE		"\n" \
				M2UA_MUX_CONTACT	"\n"
#define M2UA_MUX_SPLASH		M2UA_MUX_DESCRIP	" - " \
				M2UA_MUX_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M2UA_MUX_CONTACT);
MODULE_DESCRIPTION(M2UA_MUX_DESCRIP);
MODULE_SUPPORTED_DEVICE(M2UA_MUX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M2UA_MUX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-m2ua_mux");
MODULE_ALIAS("streams-m2ua-mux");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define M2UA_MUX_MOD_ID		CONFIG_STREAMS_M2UA_MUX_MODID
#define M2UA_MUX_MOD_NAME	CONFIG_STREAMS_M2UA_MUX_NAME
#endif				/* LFS */

#define MOD_ID	    M2UA_MUX_MOD_ID
#define MOD_NAME    M2UA_MUX_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH  M2UA_MUX_BANNER
#else				/* MODULE */
#define MOD_SPLASH  M2UA_MUX_SPLASH
#endif				/* MODULE */

/*
 *  M2UA-MUX Private Structures
 *  ---------------------------
 *  Several structures are necessary.  We need one private structure for SL streams on the upper
 *  multiplex.  We need one private structure for the TP streams on the lower multiplex.  We also
 *  need one private structure for the ASP which is associated with the minor device number of the
 *  clone minor that is used to open an AS stream.  Then we need some sort of mapping structure that
 *  maps SCTP associations to ASPs (TP Streams to ASP structures), and some sore of mapping
 *  structure that maps AS Streams to ASPs (SL Streams to ASP structures).  So we need three primary
 *  data structures and two mapping structures for connecting the three into a directed graph.
 */

struct sl;				/* upper multiplex structure (SL Stream) */
struct tp;				/* lower multiplex structure (TP Stream) */

static rwlock_t mux_lock = RW_LOCK_UNLOCKED;

struct sl {
	struct sl *next;		/* list linkage */
	struct sl **prev;		/* list linkage */
	struct tp *tp;			/* the SG to which we belong */

	queue_t *rq;			/* RD queue */
	queue_t *wq;			/* WR queue */
	dev_t dev;
	cred_t cred;

	uint mid;			/* module id */
	uint sid;			/* stream id */

	uint state;			/* AS state */
	uint l_state;			/* link state */

	uint32_t ppa;
	uint32_t iid;
	uint16_t streamid;		/* SCTP stream identifier for this AS */

	minor_t anum;			/* ASP number, minor device number that was opened. */

	mblk_t *wack_aspac;		/* Timer waiting for ASPAC Ack */
	mblk_t *wack_aspia;		/* Timer waiting for ASPIA Ack */

	lmi_info_ack_t info;

	struct lmi_option option;	/* protocol options */
	struct {
		struct sl_timers timers;	/* SL protocol timers */
		struct sl_config config;	/* SL configuration options */
		struct sl_statem statem;	/* SL state machine variables */
		struct sl_notify notify;	/* SL notification options */
		struct sl_stats stats;	/* SL statistics */
		struct sl_stats stamp;	/* SL statistics timestamps */
		struct sl_stats statsp;	/* SL statistics periods */
	} sl;
	struct {
		struct sdt_timers timers;
		struct sdt_config config;
	} sdt;
	struct {
		struct sdl_timers timers;
		struct sdl_config config;
	} sdl;
};

struct tp {
	struct tp *next;		/* list linkage */
	struct tp **prev;		/* list linkage */
	struct sl *list;		/* list of SL structures associated with this SG */

	spinlock_t lock;		/* lock: structure lock */
	uint users;			/* lock: number of users */
	queue_t *waitq;			/* lock: queue waiting for lock */
	task_struct *owner;		/* lock: task owning lock */

	queue_t *rq;			/* RD queue */
	queue_t *wq;			/* WR queue */
	int index;			/* multiplex index */
	cred_t cred;			/* credentials */

	uint mid;			/* module id */
	uint sid;			/* stream id */

	uint state;			/* ASP state */
	struct sockaddr loc;		/* local (bind) address */
	struct sockaddr rem;		/* remove (conenct) address */
	uint32_t ppi;			/* SCTP PPI to use */
	uint32_t aspid;
	mblk_t *wack_aspup;		/* Timer waiting for ASPUP Ack */
	mblk_t *wack_aspdn;		/* Timer waiting for ASPDN Ack */
	mblk_t *wack_aspac;		/* Timer waiting for ASPAC Ack */
	mblk_t *wack_aspia;		/* Timer waiting for ASPIA Ack */
	struct T_info_ack info;
};

static struct module_info sl_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

/* Timer values */
enum {
	wack_aspup = 0,
	wack_aspdn,
	wack_aspac,
	wack_aspia,
};

/*
 *  State transitions.
 *  ==================
 */

enum {
	ASP_DOWN = 0,
#define ASP_DOWN	    ASP_DOWN
	ASP_WACK_ASPUP,
#define ASP_WACK_ASPUP	    ASP_WACK_ASPUP
	ASP_WACK_ASPDN,
#define ASP_WACK_ASPDN	    ASP_WACK_ASPDN
	ASP_UP,
#define ASP_UP		    ASP_UP
	ASP_INACTIVE,
#define ASP_INACTIVE	    ASP_INACTIVE
	ASP_WACK_ASPAC,
#define ASP_WACK_ASPAC	    ASP_WACK_ASPAC
	ASP_WACK_ASPIA,
#define ASP_WACK_ASPIA	    ASP_WACK_ASPIA
	ASP_ACTIVE,
#define ASP_ACTIVE	    ASP_ACTIVE
};

const char *
tp_i_state_name(uint state)
{
	switch (state) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WREQ_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}

static uint
tp_set_i_state(struct tp *tp, uint newstate)
{
	uint oldstate = tp->info.CURRENT_state;

	if (newstate != oldstate)
		strlog(tp->mid, tp->sid, SLLOGST, SL_TRACE, "%s <- %s", tp_i_state_name(newstate),
		       tp_i_state_name(oldstate));
	return ((tp->info.CURRENT_state = newstate));
}
static inline uint
tp_get_i_state(struct tp *tp)
{
	return (tp->info.CURRENT_state);
}
static inline uint
tp_get_i_statef(struct tp *tp)
{
	return ((1 << tp_get_i_state(tp)));
}
static inline uint
tp_chk_i_state(struct tp *tp, uint mask)
{
	return (tp_get_n_statef(tp) & mask);
}
static inline uint
tp_not_i_state(struct tp *tp, uint mask)
{
	return (tp_chk_i_state(tp, ~mask));
}

const char *
tp_u_state_name(uint state)
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
	default:
		return ("(unknown)");
	}
}
static uint
tp_set_u_state(struct tp *tp, uint newstate)
{
	uint oldstate = tp->state;

	if (newstate != oldstate)
		strlog(tp->mid, tp->sid, SLLOGST, SL_TRACE, "%s <- %s", tp_u_state_name(newstate),
		       tp_u_state_name(oldstate));
	return ((tp->state = newstate));
}
static inline uint
tp_get_u_state(struct tp *tp)
{
	return (tp->state);
}
static inline uint
tp_get_u_statef(struct tp *tp)
{
	return ((1 << tp_get_u_state(tp)));
}
static inline uint
tp_chk_u_state(struct tp *tp, uint mask)
{
	return (tp_get_u_statef(tp) & mask);
}
static inline uint
tp_not_u_state(struct tp *tp, uint mask)
{
	return (tp_chk_u_state(tp, ~mask));
}

#define LMF_UNATTACHED		(1<<LMI_UNATTACHED)
#define LMF_ATTACH_PENDING	(1<<LMI_ATTACH_PENDING)
#define LMF_UNUSABLE		(1<<LMI_UNUSABLE)
#define LMF_DISABLED		(1<<LMI_DISABLED)
#define LMF_ENABLE_PENDING	(1<<LMI_ENABLE_PENDING)
#define LMF_ENABLED		(1<<LMI_ENABLED)
#define LMF_DISABLE_PENDING	(1<<LMI_DISABLE_PENDING)
#define LMF_DETACH_PENDING	(1<<LMI_DETACH_PENDING)

const char *
sl_i_state_name(uint state)
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
static uint
sl_set_i_state(struct sl *sl, uint newstate)
{
	uint oldstate = sl->info.lmi_state;

	if (oldstate != newstate)
		strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "%s <- %s", sl_i_state_name(newstate),
		       sl_i_state_name(oldstate));
	return ((sl->info.lmi_state = newstate));
}
static inline uint
sl_get_i_state(struct sl *sl)
{
	return sl->info.lmi_state;
}
static inline uint
sl_get_i_statef(struct sl *sl)
{
	return ((1 << sl_get_i_state(sl)));
}
static inline uint
sl_chk_i_state(struct sl *sl, uint mask)
{
	return (sl_get_i_statef(sl) & mask);
}
static inline uint
sl_not_i_state(struct sl *sl, uint mask)
{
	return (sl_chk_i_state(sl, ~mask));
}

const char *
sl_u_state_name(uint state)
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
static inline uint
sl_set_u_state(struct sl *sl, uint newstate)
{
	uint oldstate = sl->state;

	if (newstate != oldstate)
		strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "%s <- %s", sl_u_state_name(newstate),
		       sl_u_state_name(oldstate));
	return (sl->state = newstate);
}
static inline uint
sl_get_u_state(struct sl *sl)
{
	return (sl->state);
}
static inline uint
sl_get_u_statef(struct sl *sl)
{
	return ((1 << sl_get_u_state(sl)));
}
static inline uint
sl_chk_u_state(struct sl *sl, uint mask)
{
	return (sl_get_u_statef(sl) & mask);
}
static inline uint
sl_not_u_state(struct sl *sl, uint mask)
{
	return (sl_chk_u_state(sl, ~mask));
}

const char *
sl_l_state_name(uint state)
{
	switch (state) {
	case DL_UNBOUND:	/* 0 */
		return ("DL_UNBOUND");
	case DL_BIND_PENDING:	/* 1 */
		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:	/* 2 */
		return ("DL_UNBIND_PENDING");
	case DL_IDLE:		/* 3 */
		return ("DL_IDLE");
	case DL_UNATTACHED:	/* 4 */
		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:	/* 5 */
		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:	/* 6 */
		return ("DL_DETACH_PENDING");
	case DL_UDQOS_PENDING:	/* 7 */
		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:	/* 8 */
		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:	/* 9 */
		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:	/* 10 */
		return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:	/* 11 */
		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:	/* 12 */
		return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:	/* 13 */
		return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:	/* 14 */
		return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:	/* 15 *//* from DL_OUTCON_PENDING */
		return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:	/* 16 *//* from DL_INCON_PENDING */
		return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:	/* 17 *//* from DL_DATAXFER */
		return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:	/* 18 *//* from DL_USER_RESET_PENDING */
		return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:	/* 19 *//* from DL_PROV_RESET_PENDING */
		return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:	/* 20 */
		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:	/* 21 */
		return ("DL_SUBS_UNBIND_PND");
	default:
		return ("DL_????");
	}
}
static inline uint
sl_set_l_state(struct sl *sl, uint newstate)
{
	uint oldstate = sl->l_state;

	if (newstate != oldstate)
		strlog(sl->mid, sl->sid, SLLOGST, SL_TRACE, "%s <- %s", sl_l_state_name(newstate),
		       sl_l_state_name(oldstate));
	return (sl->l_state = newstate);
}
static inline uint
sl_get_l_state(struct sl *sl)
{
	return (sl->l_state);
}
static inline uint
sl_get_l_statef(struct sl *sl)
{
	return ((1 << sl_get_l_state(sl)));
}
static inline uint
sl_chk_l_state(struct sl *sl, uint mask)
{
	return (sl_get_l_statef(sl) & mask);
}
static inline uint
sl_not_l_state(struct sl *sl, uint mask)
{
	return (sl_chk_l_state(sl, ~mask));
}

/*
 *  Structure Initialization and Termination.
 *  =========================================
 */
static void
sl_init_priv(struct sl *sl, struct tp *tp, queue_t *q, dev_t *devp, int
	     oflags, int sflag, cred_t *crp, mblk_t *t1, mblk_t *t2, minor_t unit)
{
	/* initialize sl structure */
	bzero(sl, sizeof(*sl));
	if ((sl->sl_next = tp->list))
		sp->next->prev = &sl->next;
	sl->prev = &tp->list;
	tp->list = sl;
	sl->tp = tp;

	sl->rq = RD(q);
	sl->wq = WR(q);
	sl->dev = *devp;
	sl->cred = *crp;

	sl->mid = q->q_info->qi_minfo->mi_idnum;
	sl->sid = getminor(*devp);

	sl->state = ASP_DOWN;
	sl->l_state = DL_UNBOUND;

	sl->ppa = 0;
	sl->iid = 0;
	sl->streamid = 0;
	sl->anum = unit;

	*(int *) t1->b_rptr = wack_aspac;
	*(int *) t2->b_rptr = wack_aspia;
	sl->wack_aspac = t1;
	sl->wack_aspia = t2;

	/* by default, assume it is a plain-jane ITU-T link */
	sl->option.pvar = SS7_PVAR_ITUT_00;
	sl->option.popt = 0;

	sl->info.lmi_primitive = LMI_INFO_ACK;
	sl->info.lmi_state = LMI_UNUSABLE;
	sl->info.lmi_max_sdu = 272;
	sl->info.lmi_min_sdu = 6;
	sl->info.lmi_header_len = 12;
	sl->info.lmi_ppa_style = LMI_STYLE2;

	sl->sl.config.t1 = FIXME;	/* Timer t1 duration (milliseconds) */
	sl->sl.config.t2 = FIXME;	/* Timer t2 duration (milliseconds) */
	sl->sl.config.t2l = FIXME;	/* Timer t2l duration (milliseconds) */
	sl->sl.config.t2h = FIXME;	/* Timer t2h duration (milliseconds) */
	sl->sl.config.t3 = FIXME;	/* Timer t3 duration (milliseconds) */
	sl->sl.config.t4n = FIXME;	/* Timer t4n duration (milliseconds) */
	sl->sl.config.t4e = FIXME;	/* Timer t4e duration (milliseconds) */
	sl->sl.config.t5 = FIXME;	/* Timer t5 duration (milliseconds) */
	sl->sl.config.t6 = FIXME;	/* Timer t6 duration (milliseconds) */
	sl->sl.config.t7 = FIXME;	/* Timer t7 duration (milliseconds) */
	sl->sl.config.rb_abate = FIXME;	/* RB cong abatement (#msgs) */
	sl->sl.config.rb_accept = FIXME;	/* RB cong onset accept (#msgs) */
	sl->sl.config.rb_discard = FIXME;	/* RB cong discard (#msgs) */
	sl->sl.config.tb_abate_1 = FIXME;	/* lev 1 cong abate (#bytes) */
	sl->sl.config.tb_onset_1 = FIXME;	/* lev 1 cong onset (#bytes) */
	sl->sl.config.tb_discd_1 = FIXME;	/* lev 1 cong discard (#bytes) */
	sl->sl.config.tb_abate_2 = FIXME;	/* lev 2 cong abate (#bytes) */
	sl->sl.config.tb_onset_2 = FIXME;	/* lev 2 cong onset (#bytes) */
	sl->sl.config.tb_discd_2 = FIXME;	/* lev 2 cong discard (#bytes) */
	sl->sl.config.tb_abate_3 = FIXME;	/* lev 3 cong abate (#bytes) */
	sl->sl.config.tb_onset_3 = FIXME;	/* lev 3 cong onset (#bytes) */
	sl->sl.config.tb_discd_3 = FIXME;	/* lev 3 cong discard (#bytes) */
	sl->sl.config.N1 = FIXME;	/* PCR/RTBmax messages (#msg) */
	sl->sl.config.N2 = FIXME;	/* PCR/RTBmax octets (#bytes) */
	sl->sl.config.M = FIXME;	/* IAC normal proving periods */

	sl->sdt.config.t8 = FIXME;	/* T8 timeout (milliseconds) */
	sl->sdt.config.Tin = FIXME;	/* AERM normal proving threshold */
	sl->sdt.config.Tie = FIXME;	/* AERM emergency proving threshold */
	sl->sdt.config.T = FIXME;	/* SUERM error threshold */
	sl->sdt.config.D = FIXME;	/* SUERM error rate parameter */
	sl->sdt.config.Te = FIXME;	/* EIM error threshold */
	sl->sdt.config.De = FIXME;	/* EIM correct decrement */
	sl->sdt.config.Ue = FIXME;	/* EIM error increment */
	sl->sdt.config.N = FIXME;	/* octets per su in octet-counting mode */
	sl->sdt.config.m = FIXME;	/* maximum SIF size */
	sl->sdt.config.b = FIXME;	/* transmit block size */
	sl->sdt.config.f = FIXME;	/* number of flags between frames */

	return;
}
static void
sl_term_priv(struct sl *sl)
{
	if ((*sl->prev = sl->next))
		sp->next->prev = sl->prev;
	sl->next = NULL;
	sl->prev = &sl->next;
	m2_asp_put(XCHG(&sl->tp, NULL));
	mi_timer_free(XCHG(&sl->wack_aspac, NULL));
	mi_timer_free(XCHG(&sl->wack_aspia, NULL));
	return;
}
static void
tp_init_priv(struct tp *tp, queue_t *q, int unit, int index, cred_t *crp, mblk_t *t1, mblk_t *t2,
	     mblk_t *t3, mblk_t *t4)
{
	bzero(tp, sizeof(*tp));

	spin_lock_init(tp->lock);
	tp->users = 0;
	tp->waitq = NULL;
	tp->owner = NULL;

	tp->rq = q ? RD(q) : NULL;
	tp->wq = q ? WR(q) : NULL;
	tp->index = index;
	tp->cred = *crp;

	tp->mid = q->q_info->qi_minfo->mi_idnum;
	tp->sid = unit;

	tp->state = TS_UNBND;

	bzero(&tp->loc, sizeof(tp->loc));
	bzero(&tp->rem, sizeof(tp->rem));

	tp->ppi = M2UA_PPI;
	tp->aspid = 0;

	*(int *) t1->b_rptr = wack_aspup;
	*(int *) t2->b_rptr = wack_aspdn;
	*(int *) t3->b_rptr = wack_aspac;
	*(int *) t4->b_rptr = wack_aspia;

	tp->wack_aspup = t1;
	tp->wack_aspdn = t2;
	tp->wack_aspac = t3;
	tp->wack_aspia = t4;

	tp->info.PRIM_type = T_INFO_ACK;
	tp->info.TSDU_size = -1;
	tp->info.ETSDU_size = -1;
	tp->info.CDATA_size = -1;
	tp->info.DDATA_size = -1;
	tp->info.ADDR_size = sizeof(struct sockaddr);
	tp->info.OPT_size = -1;
	tp->info.TIDU_size = -1;
	tp->info.SERV_type = T_COTS_ORD;
	tp->info.CURRENT_state = -1;
	tp->info.PROVIDER_flag = XPG4;

	return;
}
static void
tp_term_priv(struct tp *tp)
{
}

/*
 *  Locking
 *  =======
 */

/**
 * tp_trylock: - try to lock an TP queue pair
 * @tp: TP private structure
 * @q: active queue (read or write queue)
 *
 * Note that if we always run (at least initially) from a service procedure, there is no need to
 * suppress interrupts while holding the lock.  This simple lock will do because the service
 * procedure is guaranteed single thread on both UP and SMP machines.  Also, because interrupts do
 * not need to be suppressed while holding the lock, the current task pointer identifies the thread
 * and the thread can be allowed to recurse on the lock.  When a queue procedure fails to acquire
 * the lock, it is marked to have its service procedure scheduled later, but we only remember one
 * queue, so if there are two waiting, the second one is reenabled immediately.
 */
static inline bool
tp_trylock(struct tp *tp, queue_t *q)
{
	bool rtn = false;
	pl_t pl;

	pl = LOCK(&tp->lock, plhigh);
	if (tp->users == 0 && (q->q_flag & QSVCBUSY)) {
		rtn = true;
		tp->users = 1;
		tp->owner = current;
	} else if (tp->users != 0 && tp->owner == current) {
		rtn = true;
		tp->users++;
	} else if (!tp->waitq) {
		tp->waitq = q;
	} else if (tp->waitq != q) {
		qenable(q);
	}
	UNLOCK(&tp->lock, pl);
	return (rtn);
}

static inline void
tp_lock(struct tp *tp)
{
	while (!tp_trylock(tp, NULL)) ;
}

/**
 * tp_unlock: - unlock an TP queue pair
 * @tp: TP private structure
 */
static inline void
tp_unlock(struct tp *tp)
{
	pl_t pl;

	pl = LOCK(&tp->lock, plhigh);
	if (--tp->users == 0 && tp->waitq) {
		qenable(tp->waitq);
		tp->waitq = NULL;
	}
	UNLOCK(&tp->lock, pl);
}

/*
 *  Buffer allocation
 *  =================
 */
static mblk_t *
sl_allocb(queue_t *q, size_t len, int priority)
{
	mblk_t *mp;

	if (unlikely((mp = allocb(len, priority)) == NULL)) {
		struct sl *sl = SL_PRIV(q);
		bcid_t bid, *bidp = (q->q_flag & QREADR) ? &sl->rbid : &sl->wbid;

		if ((bid = bufcall(len, priority, (void (*)(long)) &qenable, (long) q)))
			unbufcall(xchg(bidp, bid));
		else
			qenable(q);
	}
	return (mp);
}

/*
 *  M2UA Message Definitions
 *  ========================
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

struct ua_parm {
	union {
		uchar *cp;		/* pointer to parameter field */
		uint32_t *wp;		/* pointer to parameter field */
	};
	size_t len;			/* length of parameter field */
	uint32_t val;			/* value of first 4 bytes (host order) */
};

/**
 * ua_dec_parm: - extract parameter from message or field.
 * @beg: beginning of field or message
 * @eng: end of field or message
 * @parm: structure to hold result
 * @tag: parameter tag value
 *
 * If the parameter does not exist in the field or message it returns false; otherwise true, and
 * sets the parameter values if parm is non-NULL.
 */
static bool
ua_dec_parm(uchar *beg, uchar *end, struct ua_parm *parm, uint32_t tag)
{
	uint32_t *wp;
	bool rtn = false;

	for (wp = (uint32_t *) beg; (uchar *) (wp + 1) <= end; wp += (UA_LEN(*wp) + 3) >> 2) {
		if (UA_TAG(wp[0]) == UA_TAG(tag)) {
			rtn = true;
			if (parm) {
				parm->wp = (wp + 1);
				parm->len = UA_PLEN(wp[0]);
				pamr->val = ntohl(wp[1]);
			}
			break;
		}
	}
	return (rtn);

}

/*
 * SL-Provider to SL-User primitives.
 * ==================================
 */

/**
 * lmi_info_ack - issued an LMI_INFO_ACK primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_info_ack(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = sl->info.lmi_version;
		p->lmi_max_sdu = sl->info.lmi_max_sdu;
		p->lmi_min_sdu = sl->info.lmi_min_sdu;
		p->lmi_header_len = sl->info.lmi_header_len;
		p->lmi_ppa_style = sl->info.lmi_ppa_style;
		mp->b_wptr += sizeof(*p);
		bcopy(&sl->ppa, mp->b_wptr, sizeof(sl->ppa));
		mp->b_wptr += sizeof(sl->ppa);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_INFO_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_ok_ack - issued an LMI_OK_ACK primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primivite
 */
static noinline fastcall int
lmi_ok_ack(struct sl *sl, queue_t *q, mblk_t *msg, lmi_long prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_DISABLED;
			sl_set_i_state(sl, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			sl_set_i_state(sl, LMI_UNATTACHED);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_OK_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ack - issued an LMI_ERROR_ACK primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @err: positive LMI error, negative UNIX error
 */
static noinline fastcall int
lmi_error_ack(struct sl *sl, queue_t *q, mblk_t *msg, lmi_long prim, lmi_long err)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_error_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			sl_set_i_state(sl, LMI_UNATTACHED);
			break;
		case LMI_DETACH_PENDING:
		case LMI_ENABLE_PENDING:
			p->lmi_state = LMI_DISABLED;
			sl_set_i_state(sl, LMI_DISABLED);
			break;
		case LMI_DISABLE_PENDING:
			p->lmi_state = LMI_ENABLED;
			sl_set_i_state(sl, LMI_ENABLED);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_ERROR_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_enable_con - issued an LMI_ENABLE_CON primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_enable_con(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = LMI_ENABLED;
		mp->b_wptr += sizeof(*p);
		sl_set_i_state(sl, LMI_ENABLED);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_ENABLE_CON");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_con - issued an LMI_DISABLE_CON primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_disable_con(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = LMI_DISABLED;
		mp->b_wptr += sizeof(*p);
		sl_set_i_state(sl, LMI_DISABLED);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_DISABLE_CON");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * lmi_optmgmt_ack - issued an LMI_OPTMGMT_ACK primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_OPTMGMT_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/**
 * lmi_error_ind - issued an LMI_ERROR_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: postivie LMI or negative UNIX error
 * @fatal: whether error is fatal or not
 */
static noinline fastcall int
lmi_error_ind(struct sl *sl, queue_t *q, mblk_t *msg, lmi_long err, bool fatal)
{
	lmi_error_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		if (fatal)
			sl_set_i_state(sl, LMI_UNUSABLE);
		p->lmi_state = sl_get_i_state(sl);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_ERROR_IND");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * lmi_stats_ind - issued an LMI_STATS_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_stats_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_STATS_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_STATS_IND");
			putnext(sl->rq, mp);
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
 * lmi_event_ind - issued an LMI_EVENT_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_event_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_EVENT_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- LMI_EVENT_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

/**
 * sl_pdu_ind - issued an SL_PDU_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: user data to indicate
 */
static noinline fastcall int
sl_pdu_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_pdu_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_PDU_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = msg;
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_PDU_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congested_ind - issued an SL_LINK_CONGESTED_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cong: congestion status
 * @disc: discard status
 */
static noinline fastcall int
sl_link_congested_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			sl->cong = cong;
			sl->disc = disc;
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_LINK_CONGESTED_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congestion_ceased_ind - issued an SL_LINK_CONGESTED_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cong: congestion status
 * @disc: discard status
 */
static noinline fastcall int
sl_link_congestion_ceased_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ceased_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			sl->cong = cong;
			sl->disc = disc;
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE,
			       "<- SL_LINK_CONGESTION_CEASED_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieved_message_ind - issued an SL_RETRIEVED_MESSAGE_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: retrieved user data
 */
static noinline fastcall int
sl_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieved_msg_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = msg;
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_RETRIEVED_MESSAGE_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_complete_ind - issued an SL_RETRIEVAL_COMPLETE_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: retrieved user data
 */
static noinline fastcall int
sl_retrieval_complete_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieval_comp_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = msg;
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_RETRIEVAL_COMPLETE_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_rb_cleared_ind - issued an SL_RB_CLEARED_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_rb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rb_cleared_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RB_CLEARED_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_RB_CLEARED_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_bsnt_ind - issued an SL_BSNT_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @bsnt: value of BSNT to indicate
 */
static noinline fastcall int
sl_bsnt_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong bsnt)
{
	sl_bsnt_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_BSNT_IND;
			p->sl_bsnt = bsnt;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_BSNT_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_in_service_ind - issued an SL_IN_SERVICE_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_in_service_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_in_service_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_IN_SERVICE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_IN_SERVICE_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_out_of_service_ind - issued an SL_OUT_OF_SERVICE_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: reason for failure
 */
static noinline fastcall int
sl_out_of_service_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong reason)
{
	sl_out_of_service_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_OUT_OF_SERVICE_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_OUT_OF_SERVICE_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_outage_ind - issued an SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_remote_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_out_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE,
			       "<- SL_REMOTE_PROCESSOR_OUTAGE_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_recovered_ind - issued an SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_remote_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_recovered_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE,
			       "<- SL_REMOTE_PROCESSOR_RECOVERED_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_rtb_cleared_ind - issued an SL_RTB_CLEARED_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_rtb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rtb_cleared_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RTB_CLEARED_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_RTB_CLEARED_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		reutrn(-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * sl_retrieval_not_possible_ind - issued an SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_retrieval_not_possible_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieval_not_poss_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE,
			       "<- SL_RETRIEVAL_NOT_POSSIBLE_IND");
			putnext(sl->rq, mp);
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
 * sl_bsnt_not_retrievable_ind - issued an SL_BSNT_NOT_RETRIEVABLE_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_bsnt_not_retr_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE,
			       "<- SL_BSNT_NOT_RETRIEVABLE_IND");
			putnext(sl->rq, mp);
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
 * sl_optmgmt_ack - issued an SL_OPTMGMT_ACK primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_optmgmt_ack_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OPTMGMT_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_OPTMGMT_ACK");
		putnext(sl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * sl_notify_ind - issued an SL_NOTIFY_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_notify_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_notify_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_NOTIFY_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE, "<- SL_NOTIFY_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

/**
 * sl_local_processor_outage_ind - issued an SL_LOCAL_PROCESSOR_OUTAGE_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_local_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_out_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE,
			       "<- SL_LOCAL_PROCESSOR_OUTAGE_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_local_processor_recovered_ind - issued an SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_local_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_recovered_ind_t *p;
	mblk_t *mp;

	if ((mp = sl_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canpuntext(sl->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LOCAL_PROCESSOR_RECOVERED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(sl->mid, sl->sid, SLLOGTX, SL_TRACE,
			       "<- SL_LOCAL_PROCESSOR_RECOVERED_IND");
			putnext(sl->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * TS-User to TS-Provider primitives.
 * ==================================
 */
/**
 * t_conn_req: - issue a T_CONN_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dlen: destination address length
 * @dptr: destination address pointer
 * @olen: options length
 * @optr: options pointer
 * @dp: user data
 */
static noinline fastcall int
t_conn_req(struct tp *tp, queue_t *q, mblk_t *msg, size_t dlen, caddr_t dptr, size_t olen,
	   caddr_t optr, mblk_t *dp)
{
	struct T_conn_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + dlen + olen, BPRI_MED)))) {
		if (likely(bcanpuntext(tp->wq, 2))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_REQ;
			p->DEST_length = dlen;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p) + dlen;
			mp->b_wptr += sizeof(*p);
			bcopy(dptr, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			tp_set_i_state(tp, TS_WACK_CREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_CONN_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_conn_res: - issue a T_CONN_RES primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @token: stream upon which to accept the connection
 * @olen: options length
 * @optr: options pointer
 * @sequence: sequence number of connect indication
 * @dp: user data
 */
static noinline fastcall int
t_conn_res(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t token, size_t olen, caddr_t optr,
	   t_scalar_t sequence, mblk_t *dp)
{
	struct T_conn_res *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 2))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_RES;
			p->ACCEPTOR_id = token;
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p);
			p->SEQ_number = sequence;
			mp->b_wptr += sizeof(*p);
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			tp_set_i_state(tp, TS_WACK_CRES);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_CONN_RES ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_discon_req: - issue a T_DISCON_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sequence: sequence number of connect indication (or zero)
 * @dp: user data
 */
static noinline fastcall int
t_discon_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t sequence, mblk_t *dp)
{
	struct T_discon_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 2))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DISCON_REQ;
			p->SEQ_number = sequence;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			switch (tp_get_i_state(tp)) {
			case TS_WCON_CREQ:
				tp_set_i_state(tp, TS_WACK_DREQ6);
				break;
			case TS_WRES_CIND:
				tp_set_i_state(tp, TS_WACK_DREQ7);
				break;
			case TS_DATA_XFER:
				tp_set_i_state(tp, TS_WACK_DREQ9);
				break;
			case TS_WIND_ORDREL:
				tp_set_i_state(tp, TS_WACK_DREQ10);
				break;
			case TS_WREQ_ORDREL:
				tp_set_i_state(tp, TS_WACK_DREQ11);
				break;
			}
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_DISCON_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_data_req: - issue a T_DATA_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @more: more flag
 * @dp: user data
 */
static noinline fastcall int
t_data_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_data_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DATA_REQ;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_DATA_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_exdata_req: - issue a T_EXDATA_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @more: more flag
 * @dp: user data
 */
static noinline fastcall int
t_exdata_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_exdata_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 1))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_EXDATA_REQ;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_EXDATA_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_info_req: - issue a T_INFO_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
t_info_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_info_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_INFO_REQ ->");
		putnext(tp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_bind_req: - issue a T_BIND_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @alen: address length
 * @aptr: address pointer
 */
static noinline fastcall int
t_bind_req(struct tp *tp, queue_t *q, mblk_t *msg, size_t alen, caddr_t aptr)
{
	struct T_bind_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_BIND_REQ;
			p->ADDR_length = alen;
			p->ADDR_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, alen);
			mp->b_wptr += alen;
			tp_set_i_state(tp, TS_WACK_BREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_BIND_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_unbind_req: - issue a T_UNBIND_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
t_unbind_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_unbind_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			tp_set_i_state(tp, TS_WACK_UREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_UNBIND_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_unitdata_req: - issue a T_UNITDATA_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dlen: destination address length
 * @dptr: destination address pointer
 * @olen: options length
 * @optr: options pointer
 * @dp: user data
 */
static noinline fastcall int
t_unitdata_req(struct tp *tp, queue_t *q, mblk_t *msg, size_t dlen, caddr_t dptr, size_t olen,
	       caddr_t optr, mblk_t *dp)
{
	struct T_unitdata_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + dlen + olen, BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = dp->b_band;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNITDATA_REQ;
			p->DEST_length = dlen;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p) + dlen;
			mp->b_wptr += sizeof(*p);
			bcopy(dptr, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_UNITDATA_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_optmgmt_req: - issue a T_OPTMGMT_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @olen: options length
 * @optr: options pointer
 * @flags: management flags
 */
static noinline fastcall int
t_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *msg, size_t olen, caddr_t optr, t_scalar_t flags)
{
	struct T_optmgmt_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTMGMT_REQ;
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p);
			p->MGMT_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			if (tp_get_i_state(tp) == TS_IDLE)
				tp_set_i_state(tp, TS_WACK_OPTREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_OPTMGMT_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_ordrel_req: - issue a T_ORDREL_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static noinline fastcall int
t_ordrel_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_ordrel_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ORDREL_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			tp_set_i_state(tp, TS_WIND_ORDREL);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_ORDREL_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_optdata_req: - issue a T_OPTDATA_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flag: data transfer flag
 * @sid: stream id
 * @dp: user data
 */
static noinline fastcall int
t_optdata_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flag, t_uscalar_t sid, mblk_t *dp)
{
	struct T_optdata_req *p;
	mblk_t *mp;
	struct t_opthdr *oh;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p) + sizeof(*oh) + sizeof(sid), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, dl->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = dp->b_band;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flag;
			p->OPT_length = sizeof(*oh) + sizeof(sid);
			p->OPT_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			oh = (typeof(oh)) mp->b_wptr;
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_SID;
			oh->len = sizeof(sid);
			mp->b_wptr += sizeof(*oh);
			*(t_uscalar_t *) mp->b_wptr = sid;
			mp->b_wptr += sizeof(sid);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_OPTDATA_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_addr_req: - issue a T_ADDR_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
t_addr_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_addr_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ADDR_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_ADDR_REQ ->");
			putnext(tp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_capability_req: - issue a T_CAPABILITY_REQ primitive
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @bits: flags
 */
static noinline fastcall int
t_capability_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t bits)
{
	struct T_capability_req *p;
	mblk_t *mp;

	if (likely(!!(mp = sl_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_REQ;
		p->CAP_bits1 = bits;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(tp->mid, tp->sid, SLLOGTX, SL_TRACE, "T_CAPABILITY_REQ ->");
		putnext(tp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA-AS to M2UA-SG (Sent) M2UA Protocol Messages.
 *  =================================================
 */

/**
 * tp_send_mgmt_err: - send MGMT ERR message
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @ecode: error code
 * @dptr: diagnostics pointer
 * @dlen: diagnostics length
 */
static noinline fastcall __unlikely int
tp_send_mgmt_err(struct tp *tp, queue_t *q, mblk_t *msg, uint32_t ecode, caddr_t dptr, size_t dlen)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_ECODE) + dlen ? UA_SIZE(UA_PARM_DIAG) +
	    UA_PAD4(dlen) : 0;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_MGMT_ERR;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_ECODE;
		p[3] = htonl(ecode);
		p += 4;
		if (dlen) {
			*p++ = UA_PHDR(UA_PARM_DIAG, dlen);
			bcopy(dptr, p, dlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(dlen);

		/* send unordered and expedited on management stream 0 */
		if (unlikely((err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_asps_aspup_req: - send ASP Up
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @iptr: Info pointer
 * @ilen: Info length
 */
static fastcall int
tp_send_asps_aspup_req(struct tp *tp, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t iptr,
		       size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + ilen ? UA_PHDR_SIZE +
	    UA_PAD4(ilen) : 0;

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
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(ilen);
		tp_set_u_state(tp, ASP_WACK_ASPUP);
		mi_timer(tp->aspup_ack, 2000);
		/* send unordered and expedited on stream 0 */
		if (unlikely((err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_asps_aspdn_req: = send ASP Down
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @iptr: Info pointer
 * @ilen: Info length
 */
static fastcall int
tp_send_asps_aspdn_req(struct tp *tp, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t iptr,
		       size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + ilen ? UA_PHDR_SIZE +
	    UA_PAD4(ilen) : 0;

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
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(ilen);
		tp_set_u_state(tp, ASP_WACK_ASPDN);
		mi_timer(tp->aspdn_ack, 2000);
		/* send unordered and expedited on stream 0 */
		if (unlikely((err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_asps_hbeat_req: - send a BEAT message
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat info pointer
 * @hlen: heartbeat info length
 *
 * This one is used for MGMT hearbeat not associated with any specific signalling link and,
 * therefore, sent on SCTP stream identifier 0 (management stream).
 */
static fastcall int
tp_send_asps_hbeat_req(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hlen);
		mi_timer(tp->hbeat_tack, 2000);
		/* send ordered on management stream 0 */
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_asps_hbeat_req: - send a BEAT message
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat info pointer
 * @hlen: heartbeat info length
 *
 * This one is used for AS-SG heartbeat associated with a specific signalling link and, therefore,
 * sent on the same SCTP stream identifier as is data for the signalling link.
 */
static fastcall int
sl_send_asps_hbeat_req(struct sl *sl, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hlen);
		mi_timer(sl->hbeat_tack, 2000);
		/* send ordered on specified stream */
		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_aspt_aspac_req: - send an ASP Active Request
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iptr: info pointer
 * @ilen: info length
 *
 * This is used to activate an AS (SL).  Signalling Links are activated when they are enabled by the
 * SL-User (LMI_ENABLE_REQ).  They are deactivated when they are disabled by the SL-User
 * (LMI_DISABLE_REQ).
 */
static int
sl_send_aspt_aspac_req(struct sl *sl, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (sl->tmode ? UA_SIZE(UA_PARM_TMODE) : 0) +
	    (sl->iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPAC_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (sl->tmode) {
			p[0] = UA_PARM_TMODE;
			p[1] = htonl(sl->tmode);
			p += 2;
		}
		if (sl->iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(sl->iid);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(ilen);
		sl_set_u_state(sl, ASP_WACK_ASPAC);
		mi_timer(sl->aspac_tack, 2000);
		/* always sent on same stream as data */
		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_aspt_aspia_req: - send an ASP Inactive Request
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iptr: info pointer
 * @ilen: info length
 *
 * This is used to deactivate an AS (SL).  Signalling links are deactivated when they are disabled
 * by the SL-User (LMI_DISABLE_REQ).  They are activate when they are enabled by the SL-User
 * (LMI_ENABLE_REQ).
 */
static int
sl_send_aspt_aspia_req(struct sl *sl, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t meln = UA_MHDR_SIZE +
	    (sl->iid ? UA_PDHR_SIZE + sizeof(uint32_t) : 0) +
	    (ieln ? UA_PDHR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (sl->iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(sl->iid);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(ilen);
		sl_set_u_state(sl, ASP_WACK_ASPIA);
		mi_timer(sl->aspia_tack, 2000);
		/* always sent on same stream as data */
		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_rkmm_reg_req: - send REG REQ
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @id: request identifier
 * @sdti: signalling data terminal identifier
 * @sdli: signalling data link identifer
 *
 * Used to register a PPA if the PPA to IID mapping is not statically assigned (using input-output
 * controls) and the SG is a Style II SG (that is, it supports registration requests).
 */
static int
sl_send_rkmm_reg_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t id, uint16_t sdti,
		     uint16_t sdli)
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

		if (unlikely((err = t_optdata_req(sl->tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_rkmm_dereg_req: - send DEREG REQ
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_send_rkmm_dereg_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_DEREG_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(sl->iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = t_optdata_req(sl->tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		retrn(err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_data1: - send DATA1
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static inline fastcall __hot_write int
sl_send_maup_data1(struct sl *sl, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if (unlikely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(sl->iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA1, dlen);
		p += 5;
		mp->b_wptr = (unsigned char *) p;

		mp->b_cont = dp->b_cont;
		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
}

/**
 * sl_send_maup_data2: - send DATA2
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 * @pri: message priority (placed in first byte of message)
 */
static inline fastcall __hot_write int
sl_send_maup_data2(struct sl *sl, queue_t *q, mblk_t *msg, mblk_t *dp, uint8_t pri)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PDHR_SIZE + 1;

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(sl->iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA2, dlen + 1);
		p += 5;
		*(unsigned char *) p = pri;
		mp->b_wptr = (unsigned char *) p + 1;

		mp->b_cont = dp->b_cont;
		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_estab_req: - send MAUP Establish Request
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_send_maup_estab_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_ESTAB_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(sl->iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		sl_set_l_state(sl, DL_OUTCON_PENDING);

		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_rel_req: - send MAUP Release Request
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_send_maup_rel_req(struct sl *sl, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(sl->iid);
		p += 4;
		mp->b_wptr = (unsigned char *) p;

		switch (sl_get_l_state(sl)) {
		case DL_OUTCON_PENDING:
			sl_set_l_state(sl, DL_DISCON8_PENDING);
			break;
		case DL_INCON_PENDING:
			sl_set_l_state(sl, DL_DISCON9_PENDING);
			break;
		case DL_DATAXFER:
			sl_set_l_state(sl, DL_DISCON11_PENDING);
			break;
		case DL_USER_RESET_PENDING:
			sl_set_l_state(sl, DL_DISCON12_PENDING);
			break;
		case DL_PROV_RESET_PENDING:
			sl_set_l_state(sl, DL_DISCON13_PENDING);
			break;
		default:
			/* software error */
			sl_set_l_state(sl, -1);
			break;
		}

		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_state_req: - send MAUP State Request
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @request: state request
 */
static int
sl_send_maup_state_req(struct sl *sl, queue_t *q, mblk_t *msg, const uint32_t request)
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
		p[3] = htonl(sl->iid);
		p[4] = M2UA_PARM_STATE_REQUEST;
		p[5] = htonl(request);
		p += 6;
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * sl_send_maup_retr_req: - send MAUP Retrieval Request
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @fsnc: optional FSNC value
 */
static int
sl_send_maup_retr_req(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t *fsnc)
{
	int err;
	mblk_t *mp;
	const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_ACTION) +
	    (fsnc ? UA_SIZE(M2UA_PARM_SEQNO) : 0);

	if (likely((mp = sl_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_RETR_REQ;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(sl->iid);
		p[4] = M2UA_PARM_ACTION;
		if (fsnc) {
			p[5] = __constant_htonl(M2UA_ACTION_RTRV_MSGS);
			p[6] = M2UA_PARM_SEQNO;
			p[7] = htonl(*fnsc);
			p += 8;
		} else {
			p[5] = __constant_htonl(M2UA_ACTION_RTRV_BSN);
			p += 6;
		}
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);

}

/**
 * sl_send_maup_data_ack: - send MAUP Data Ack
 * @sl: SL private structure
 * @q: active queue
 * @msg: message to free upon success
 * @corid: correlation id
 */
static inline fastcall __hot_in int
sl_send_maup_data_ack(struct sl *sl, queue_t *q, mblk_t *msg, uint32_t corid)
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
		p[3] = htonl(sl->iid);
		p[4] = M2UA_PARM_CORR_ID_ACK;
		p[5] = htonl(corid);
		p += 6;
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = t_optdata_req(sl->tp, q, msg, 0, sl->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 * M2UA-SG to M2UA-AS M2UA Protocol Messages.
 * ==========================================
 */

/**
 * sl_recv_mgmt_err: - receive MGMT ERR message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_mgmt_err(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_recv_mgmt_err: - receive MGMT ERR message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_mgmt_err(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_mgmt_ntfy: - receive MGMT NTFY message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_mgmt_ntfy(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_recv_mgmt_ntfy: - receive MGMT NTFY message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_mgmt_ntfy(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_recv_asps_hbeat_req: - process BEAT Request
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_asps_hbeat_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct ua_parm iid, hbeat;

	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &hbeat, UA_PARM_HBDATA))
		return (-EINVAL);
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &iid, UA_PARM_IID)) {
		struct sl *sl;

		for (sl = tp->list; sl; sl = sl->next)
			if (sl->iid == iid.val)
				break;
		if (sl != NULL)
			return sl_send_asps_hbeat_ack(sl, q, mp, hbeat.cp, hbeat.len);
	}
	/* process as normal */
	return tp_send_asps_hbeat_ack(tp, q, mp, hbeat.cp, hbeat.len);
}

/**
 * tp_recv_asps_aspup_ack: - process ASP Up Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_asps_aspup_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_recv_asps_aspdn_ack: - process ASP Down Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_asps_aspdn_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_asps_hbeat_ack: - receive BEAT Ack message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_asps_hbeat_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_recv_asps_hbeat_ack: - receive BEAT Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_asps_hbeat_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_aspt_aspac_ack: - receive ASPAC Ack message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_aspt_aspac_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_recv_apsac_ack: - receive ASPAC Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_aspt_aspac_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_aspt_aspia_ack: - receive ASPIA Ack message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_aspt_aspia_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_recv_aspt_apia_ack: - receive ASPIA Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_aspt_aspia_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_estab_con: - receive ESTAB Con message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_estab_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_rel_con: - receive REL Con message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_rel_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_rel_ind: - receive REL Ind message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_rel_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_lpo_set: - receive Status LPO Set message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_lpo_set(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_lpo_clear: - receive Status LPO Clear message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_lpo_clear(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_emer_set: - receive Status EMER Set message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_emer_set(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_emer_clear: - receive Status EMER Clear message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_emer_clear(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_flush_buffers: - receive Status Flush Buffers message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_flush_buffers(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_continue: - receive Status Continue message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_continue(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_clear_rtb: - receive Status Clear RTB message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_clear_rtb(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_audit: - receive Status Audit message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_audit(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_cong_clear: - receive Status CONG Clear message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_cong_clear(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_cong_accept: - receive Status CONG Accept message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_cong_accept(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_status_cong_discard: - receive Status CONG Discard message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_status_cong_discard(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

enum {
	M2UA_STATUS_LPO_SET = 0,
#define M2UA_STATUS_LPO_SET		M2UA_STATUS_LPO_SET
	M2UA_STATUS_LPO_CLEAR,
#define M2UA_STATUS_LPO_CLEAR		M2UA_STATUS_LPO_CLEAR
	M2UA_STATUS_EMER_SET,
#define M2UA_STATUS_EMER_SET		M2UA_STATUS_EMER_SET
	M2UA_STATUS_EMER_CLEAR,
#define M2UA_STATUS_EMER_CLEAR		M2UA_STATUS_EMER_CLEAR
	M2UA_STATUS_FLUSH_BUFFERS,
#define M2UA_STATUS_FLUSH_BUFFERS	M2UA_STATUS_FLUSH_BUFFERS
	M2UA_STATUS_CONTINUE,
#define M2UA_STATUS_CONTINUE		M2UA_STATUS_CONTINUE
	M2UA_STATUS_CLEAR_RTB,
#define M2UA_STATUS_CLEAR_RTB		M2UA_STATUS_CLEAR_RTB
	M2UA_STATUS_AUDIT,
#define M2UA_STATUS_AUDIT		M2UA_STATUS_AUDIT
	M2UA_STATUS_CONG_CLEAR,
#define M2UA_STATUS_CONG_CLEAR		M2UA_STATUS_CONG_CLEAR
	M2UA_STATUS_CONG_ACCEPT,
#define M2UA_STATUS_CONG_ACCEPT		M2UA_STATUS_CONG_ACCEPT
	M2UA_STATUS_CONG_DISCARD,
#define M2UA_STATUS_CONG_DISCARD	M2UA_STATUS_CONG_DISCARD
};

/**
 * sl_recv_maup_state_con: - receive State Con message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_state_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct ua_parm status = { {NULL,}, };

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &status, M2UA_PARM_STATE_REQUEST)) {
		switch (status.val) {
		case M2UA_STATUS_LPO_SET:
			return sl_recv_status_lpo_set(sl, q, mp);
		case M2UA_STATUS_LPO_CLEAR:
			return sl_recv_status_lpo_clear(sl, q, mp);
		case M2UA_STATUS_EMER_SET:
			return sl_recv_status_emer_set(sl, q, mp);
		case M2UA_STATUS_EMER_CLEAR:
			return sl_recv_status_emer_clear(sl, q, mp);
		case M2UA_STATUS_FLUSH_BUFFERS:
			return sl_recv_status_flush_buffers(sl, q, mp);
		case M2UA_STATUS_CONTINUE:
			return sl_recv_status_continue(sl, q, mp);
		case M2UA_STATUS_CLEAR_RTB:
			return sl_recv_status_clear_rtb(sl, q, mp);
		case M2UA_STATUS_AUDIT:
			return sl_recv_status_audit(sl, q, mp);
		case M2UA_STATUS_CONG_CLEAR:
			return sl_recv_status_cong_clear(sl, q, mp);
		case M2UA_STATUS_CONG_ACCEPT:
			return sl_recv_status_cong_accept(sl, q, mp);
		case M2UA_STATUS_CONG_DISCARD:
			return sl_recv_status_cong_discard(sl, q, mp);
		default:
			return (-EOPNOTSUPP);
		}
	}
	return (-EINVAL);
}

/**
 * sl_recv_event_rpo_enter: - receive RPO Enter message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_event_rpo_enter(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_event_rpo_exit: - receive RPO Exit message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_event_rpo_exit(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_event_lpo_enter: - receive LPO Enter message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_event_lpo_enter(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_event_lpo_exit: - receive LPO Exit message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_event_lpo_exit(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

enum {
	M2UA_EVENT_RPO_ENTER = 1,
#define M2UA_EVENT_RPO_ENTER		M2UA_EVENT_RPO_ENTER
	M2UA_EVENT_RPO_EXIT,
#define M2UA_EVENT_RPO_EXIT		M2UA_EVENT_RPO_EXIT
	M2UA_EVENT_LPO_ENTER,
#define M2UA_EVENT_LPO_ENTER		M2UA_EVENT_LPO_ENTER
	M2UA_EVENT_LPO_EXIT,
#define M2UA_EVENT_LPO_EXIT		M2UA_EVENT_LPO_EXIT
};

/**
 * sl_recv_maup_state_ind: - receive STATE Ind message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_state_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct up_parm event = { {NULL,}, };

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &event, M2UA_PARM_STATE_EVENT)) {
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
 * sl_recv_maup_retr_con: - receive RETR Con message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_retr_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_retr_ind: - receive RETR Ind message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_retr_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_retr_comp_ind: - receive RETR COMP Ind message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_retr_comp_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_cong_ind: - receive CONG Ind message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_cong_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_data: - receive DATA message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_data(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_maup_data_ack: - receive DATA Ack message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_maup_data_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_recv_rkmm_reg_rsp: - receive REG Rsp message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_rkmm_reg_rsp(struct sl *sl, queue_t *q, mblk_t *mp, uint32_t status, uint32_t iid)
{
	int err, error = 0;

	sl->iid = iid;

	switch (status) {
	case 0:		/* Successfully Registered */
		error = 0;
	case 2:		/* Error - Invalid SDLI */
	case 3:		/* Error - Invalid SDTI */
	case 4:		/* Error - Invalid Link Key */
	case 6:		/* Error - Overlapping (Non-unique) Link Key */
	case 7:		/* Error - Link Key not Provisioned */
		error = LMI_BADPPA;
		break;
	default:
	case 1:		/* Error - Unknown */
		error = LMI_UNSPEC;
		break;
	case 5:		/* Error - Permission Denied */
		error = -EPERM;
		break;
	case 8:		/* Error - Insufficient Resources */
		error = -ENOMEM;
		break;
	}
	if (sl_get_i_state(sl) == LMI_ATTACH_PENDING) {
		if ((err = error ? lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, error)
		     : lmi_ok_ack(sl, q, mp, LMI_ATTACH_REQ)) == 0)
			sl->request_id = 0;
		return (err);
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_recv_rkmm_dereg_rsp: - receive DEREG Rsp message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_recv_rkmm_dereg_rsp(struct sl *sl, queue_t *q, mblk_t *mp, uint32_t status, uint32_t iid)
{
	switch (status) {
	case 0:		/* Successfully De-registered */
	case 1:		/* Error - Unknown */
	case 2:		/* Error - Invalid Interface Identifier */
	case 3:		/* Error - Permission Denied */
	case 4:		/* Error - Not Registered */
	default:		/* protocol error */
		sl->iid = 0;
		if (sl_get_i_state(sl) == LMI_DETACH_PENDING)
			return lmi_ok_ack(sl, q, mp, LMI_DETACH_REQ);
		freemsg(mp);
		return (0);
	}
}

/**
 * sl_recv_err: - process receive error
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 * @err: error
 */
static int
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
	case -EINVAL:
		err = UA_ECODE_UNEXPECTED_MESSAGE;
		goto error;
	case -EMSGSIZE:
		err = UA_ECODE_PARAMETER_FIELD_ERROR;
		goto error;
	case -EOPNOTSUPP:
		err = UA_ECODE_UNSUPPORTED_MESSAGE_TYPE;
		goto error;
	case -ENOPROTOOPT:
		err = UA_ECODE_UNSUPPORTED_MESSAGE_CLASS;
		goto error;
	case -EPROTO:
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
 * tp_recv_mgmt: - receive MGMT message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_mgmt(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (UA_MSG_TYPE(p[0])) {
	case UA_MGMT_ERR:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "ERR <-");
		err = tp_recv_mgmt_err(tp, q, mp);
		break;
	case UA_MGMT_NTFY:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "NTFY <-");
		err = tp_recv_mgmt_ntfy(tp, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
}

/**
 * snp_recv_asps: - receive ASPS message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_asps(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (UA_MSG_TYPE(p[0])) {
	case UA_ASPS_HBEAT_REQ:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "BEAT <-");
		err = tp_recv_asps_hbeat_req(tp, q, mp);
		break;
	case UA_ASPS_ASPUP_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "ASPUP Ack <-");
		err = tp_recv_asps_aspup_ack(tp, q, mp);
		break;
	case UA_ASPS_ASPDN_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "ASPDN Ack <-");
		err = tp_recv_asps_aspdn_ack(tp, q, mp);
		break;
	case UA_ASPS_HBEAT_ACK:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "BEAT Ack <-");
		err = tp_recv_asps_hbeat_ack(tp, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
}

/**
 * tp_recv_aspt - receive ASPT message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * A note about ASPAC Ack and ASPIA Ack: the RFC says that if there is an IID in the request that
 * ther MUST be an IID in the Ack.  Because we always send an IID in the request, we require that
 * there always be an IID in the Ack.  We use the IID to determine for which signalling link the
 * acknowledgement applies.
 */
static int
tp_recv_aspt(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct ua_parm iid = { {NULL,}, };
	int err;

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &status, UA_PARM_IID)) {
		struct sl *sl;

		for (sl = tp->list; sl; sl = sl->next)
			if (sl->iid == iid.val)
				break;
		if (sl == NULL)
			return (-EINVAL);

		switch (UA_MSG_TYPE(p[0])) {
		case UA_ASPT_ASPAC_ACK:
			strlog(tp->mid, tp->sdi, SLLOGRX, SL_TRACE, "ASPAC Ack <-");
			err = tp_recv_aspt_aspac_ack(tp, q, mp);
			break;
		case UA_ASPT_ASPIA_ACK:
			strlog(tp->mid, tp->sdi, SLLOGRX, SL_TRACE, "ASPIA Ack <-");
			err = tp_recv_aspt_aspia_ack(tp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		return (err);
	}
	return (-EINVAL);
}

/**
 * tp_recv_maup: - receive MAUP message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_maup(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct ua_parm iid = { {NULL,}, };
	int err;

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &iid, UA_PARM_IID)) {
		struct sl *sl;

		for (sl = tp->list; sl; sl = sl->next)
			if (sl->iid == iid.val)
				break;
		if (sl == NULL)
			return (-EINVAL);

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
		return (err);
	}
	return (-EINVAL);
}

/**
 * tp_recv_rkmm - receive RKMM message from SG
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: RKMM message
 *
 * RKMM messages are only sent by upper SL Streams.  They are sent as part of the LMI_ATTACH_REQ
 * process and used to translate a PPA to an IID (if required).  When an upper SL Stream sends a
 * registration or deregistration request, it save the identifier of the request in the SL private
 * structure.  We can find the SL structure that corresponds to a response by finding the structure
 * with the same request identifier.
 */
static int
tp_recv_rkmm(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct sl *sl;
	int err;

	/* FIXME: mesasge can contain multiple registration results (but probably won't) */
	for (each_registration_result) {
		for (sl = tp->list; sl; sl = sl->next)
			if (sl->id == request_id)
				break;
		if (sl == NULL)
			/* FIXME: process error for each registration result instead of returning
			   it */
			return (-EPROTO);

		switch (UA_MSG_TYPE(p[0])) {
		case UA_RKMM_REG_RSP:
			/* FIXME: dig registration status and interface identifier out of the
			   registration result */
			strlog(sl->mid, sl->sid < SLLOGRX, SL_TRACE, "REG Rsp <-");
			err = sl_recv_rkmm_reg_rsp(sl, q, mp, status, iid);
			break;
		case UA_RKMM_DEREG_RSP:
			/* FIXME: dig registration status out of the deregistration result */
			strlog(sl->mid, sl->sid < SLLOGRX, SL_TRACE, "DEREG Rsp <-");
			err = sl_recv_rkmm_dereg_rsp(sl, q, mp, status, iid);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		/* FIXME: process error for each registration result instead of returning it */
		return (err);
	}
	return (0);
}

/**
 * tp_recv_msg_slow: - receive message (slow path)
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall int
tp_recv_msg_slow(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err = -EMSGSIZE;

	if (mp->b_wptr < mp->b_rptr + 2 * sizeof(*p))
		goto error;
	if (mp->b_wptr < mp->b_rptr + ntonl(p[1]))
		goto error;
	switch (UA_MSG_CLAS(p[0])) {
	case UA_CLASS_MGMT:
		err = tp_recv_mgmt(tp, q, mp);
		break;
	case UA_CLASS_ASPS:
		err = tp_recv_asps(tp, q, mp);
		break;
	case UA_CLASS_ASPT:
		err = tp_recv_aspt(tp, q, mp);
		break;
	case UA_CLASS_MAUP:
		err = tp_recv_maup(tp, q, mp);
		break;
	case UA_CLASS_RKMM:
		err = tp_recv_rkmm(tp, q, mp);
		break;
	default:
		err = -ENOPROTOOPT;
		break;
	}
	if (err == 0)
		return (0);
      error:
	return tp_recv_err(tp, q, mp, err);
}

/**
 * tp_recv_msg: - receive message (fast path)
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message (just the M2UA part)
 */
static inline fastcall int
tp_recv_msg(struct tp *tp, queue_t *q, mblk_t *mp)
{
	uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	/* fast path for data */
	if (mp->b_wptr >= mp->b_rptr + 2 * sizeof(*p))
		if (mp->b_wptr >= mp->b_rptr + ntohl(p[1]))
			if (UA_MSG_CLAS(p[0]) = UA_CLASS_MAUP)
				if (UA_MSG_TYPE(p[0]) == M2UA_MAUP_DATA) {
					if ((err = tp_recv_maup_data(tp, q, mp)) == 0)
						return (0);
					return tp_recv_err(tp, q, mp, err);
				}
	return tp_recv_msg_slow(tp, q, mp);
}

/*
 * SL-User to SL-Provider primitives.
 * ==================================
 */

/**
 * lmi_info_req - process LMI_INFO_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_info_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return lmi_info_ack(sl, q, mp);
}

/**
 * lmi_attach_req - process LMI_ATTACH_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * All M2UA-AS upper multiplex Streams (SL-AS) are Style II links.  That is, they must be attached
 * to a PPA before they are enabled.  The PPA maps to an IID.  Either the mapping is available
 * locally (static configuration) or the PPA can be mapped to an IID using the registration
 * mechanism of M2UA (dynamic configuration).  Which is performed depends on the Style of the SG to
 * which the association is attached.
 *
 * When there is not yet an SCTP association linked to the SL Stream, the request will fail.  When
 * the SCTP association exists, but the association needs some work before it can be used (i.e. ASP
 * Up, Reg Request), that procedure is invoked and the attach request remains pending until the
 * invoked procedure completes.
 *
 * The local address can be specified as normal, which is just a 4 byte PPA, which, for M2UA
 * corresponds to a 2 byte SDTI (Signalling Data Terminal Identifier) and 2 byte SDLI (Signalling
 * Data Link Identifier), the later of which could simply by a CIC.  Also, an exended PPA can be
 * used, consisting of the 4 byte PPA just mentioned, followed by a sockaddr_storage structure
 * containing the local IP address(es) to which to bind.  When no local IP address(es) are specified
 * and the TS-Provider is unbound, the TS-Provider can be requested to assign IP address(es) to the
 * connection.
 */
static noinline fastcall int
lmi_attach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sizeof(uint32_t))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_UNATTACHED)
		goto outstate;
	/* FIXME: the first 4 bytes of the PPA should be an ASPID (set to zero when ASPID not
	   required) and the remainder should be a sockaddr_storage structure, or just an ASPID, or
	   nothing at all.  Also, if the TS-provider is already bound, complete the operation.  If
	   that TS-privder is already connected, send an ASP Up request with the ASPID from the
	   attach request. */
	if ((tp = sl->tp)) {
		switch (tp_get_i_state(tp)) {
		case TS_UNBND:
			sl_set_i_state(sl, LMI_ATTACH_PENDING);
			sl->ppa = *(uint32_t *) &p->lmi_ppa[0];
			return t_bind_req(tp, q, mp, (caddr_t) &p->lmi_ppa[0],
					  mp->b_wptr - mp->b_rptr - sizeof(*p) - sizeof(uint32_t));
		case TS_IDLE:
			sl_set_i_state(sl, LMI_ATTACH_PENDING);
			return lmi_ok_ack(sl, q, mp, LMI_ATTACH_REQ);
		case TS_WCON_CREQ:
		case TS_WACK_CRES:
		case TS_DATA_XFER:
			/* FIXME: send an ASP Up first. */
			sl_set_i_state(sl, LMI_ATTACH_PENDING);
			return lmi_ok_ack(sl, q, mp, LMI_ATTACH_REQ);
		}
		goto outstate;
	}
	/* XXX: well, this is not really out state, the PPA is not ready */
	goto outstate;
      tooshort:
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_OUTSTATE);
}

/**
 * lmi_detach_req - process LMI_DETACH_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_detach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp = sl->tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_DISABLED)
		goto outstate;
	sl_set_i_state(sl, LMI_DISABLE_PENDING);
	switch (tp_get_u_state(tp)) {
	case ASP_DOWN:
	case ASP_WACK_ASPDN:
	case ASP_WACK_ASPUP:
		sl_set_u_state(sl, ASP_DOWN);
		return lmi_ok_ack(sl, q, mp, LMI_DETACH_REQ);
	case ASP_UP:
	default:
		goto unspec;
	}
	switch (sl_get_u_state(sl)) {
	case ASP_INACTIVE:
		/* do I need to deregister? */
		if (need_to_deregister) {
			sl_set_u_state(sl, ASP_WACK_ASPDN);
			return sl_send_rkmm_dereg_req(sl, q, mp);
		}
		return lmi_ok_ack(sl, q, mp, LMI_DETACH_REQ);
	case ASP_WACK_ASPDN:
		/* wait for deregistration to complete */
		freemsg(mp);
		return (0);
	case ASP_WACK_ASPUP:
	case ASP_WACK_ASPIA:
	case ASP_WACK_ASPAC:
	case ASP_ACTIVE:
	default:
		goto unspec;
	}
      unspec:
	return lmi_error_ack(sl, q, mp, LMI_DETACH_REQ, LMI_UNSPEC);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_DETACH_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, LMI_DETACH_REQ, LMI_TOOSHORT);

}

/**
 * lmi_enable_req - process LMI_ENABLE_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * All M2UA-AS links must be enabled before use.  Enabling can consist of two steps.  If the
 * underlying SCTP association is not connected, it must be connected and the ASP brought up.  If
 * the underlying SCTP association is connected and the ASP is up, then the ASP Active procedure for
 * the IIDs associated with the SL link are invoked.  All invoked procedures must complete before
 * the LMI_ENABLE_CON is issued, but an LMI_OK_ACK can be issued immediately in response to the
 * LMI_ENABLE_REQ.
 */
static noinline fastcall int
lmi_enable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp = sl->tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_DISABLED)
		goto outstate;
	sl_set_i_state(sl, LMI_ENABLE_PENDING);
	switch (tp_get_u_state(tp)) {
	case ASP_DOWN:
		tp_set_u_state(tp, ASP_WACK_ASPUP);
		return tp_send_asps_aspup_req(tp, q, mp, &tp->aspid, NULL, 0);
	case ASP_WACK_ASPDN:
	default:
		goto unspec;
	case ASP_WACK_ASPUP:
	case ASP_UP:
		break;
	}
	switch (sl_get_u_state(sl)) {
	case ASP_INACTIVE:
		sl_set_u_state(sl, ASP_WACK_ASPAC);
		return sl_send_aspt_aspac_req(sl, q, mp, NULL, 0);
	case ASP_WACK_ASPAC:
		freemsg(mp);
		return (0);
	case ASP_ACTIVE:
		return lmi_enable_con(sl, q, msg);
	case ASP_WACK_ASPIA:
	default:
		goto unspec;
	}
      unspec:
	return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_UNSPEC);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_TOOSHORT);
}

/**
 * lmi_disable_req - process LMI_DISABLE_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_disable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_i_state(sl) != LMI_ENABLED)
		goto oustate;
	sl_set_i_state(sl, LMI_DISABLE_PENDING);
	switch (tp_get_u_state(tp)) {
	case ASP_DOWN:
	case ASP_WACK_ASPDN:
	case ASP_WACK_ASPUP:
		sl_set_u_state(sl, ASP_INACTIVE);
		return lmi_disable_con(sl, q, mp);
	case ASP_UP:
		break;
	}
	switch (sl_get_u_state(sl)) {
	case ASP_ACTIVE:
		sl_set_u_state(sl, ASP_WACK_ASPIA);
		return sl_send_aspt_aspia_req(sl, q, mp, NULL, 0);
	case ASP_WACK_ASPIA:
		freemsg(mp);
		return (0);
	case ASP_INACTIVE:
		return lmi_disable_con(sl, q, msg);
	case ASP_WACK_ASPAC:
	default:
		goto unspec;
	}
      unspec:
	return lmi_error_ack(sl, q, mp, LMI_DISABLE_REQ, LMI_UNSPEC);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_DISABLE_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, LMI_DISABLE_REQ, LMI_TOOSHORT);
}

/**
 * lmi_optmgmt_req - process LMI_OPTMGMT_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
}

/**
 * sl_pdu_req - process SL_PDU_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static inline fastcall __hot_write int
sl_pdu_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto outstate;
	mp->b_cont = NULL;
	if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
		if ((err = sl_send_maup_data1(sl, q, mp, dp)) != 0)
			mp->b_cont = dp;
	} else {
		if ((err = sl_send_maup_data2(sl, q, mp, dp, sl->sl_mp)) != 0)
			mp->b_cont = dp;
	}
	return (err);
      oustate:
	return lmi_error_ack(sl, q, mp, SL_PDU_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_PDU_REQ, LMI_TOOSHORT);
}

/**
 * sl_emergency_req - process SL_EMERGENCY_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_emergency_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) == DL_UNBOUND)
		goto oustate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_EMER_SET);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_EMERGENCY_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_EMERGENCY_REQ, LMI_TOOSHORT);
}

/**
 * sl_emergency_ceases_req - process SL_EMERGENCY_CEASES_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_emergency_ceases_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) == DL_UNBOUND)
		goto oustate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_EMER_CLEAR);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_EMERGENCY_CEASES_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_EMERGENCY_CEASES_REQ, LMI_TOOSHORT);
}

/**
 * sl_start_req - process SL_START_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_start_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_IDLE)
		goto oustate;
	sl_set_l_state(sl, DL_OUTCON_PENDING);
	return sl_send_maup_estab_req(sl, q, mp);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_START_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_START_REQ, LMI_TOOSHORT);
}

/**
 * sl_stop_req - process SL_STOP_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_stop_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (sl_get_l_state(sl)) {
	case DL_OUTCON_PENDING:
		sl_set_l_state(sl, DL_DISCON8_PENDING);
		break;
	case DL_INCON_PENDING:
		sl_set_l_state(sl, DL_DISCON9_PENDING);
		break;
	case DL_DATAXFER:
		sl_set_l_state(sl, DL_DISCON11_PENDING);
		break;
	case DL_USER_RESET_PENDING:
		sl_set_l_state(sl, DL_DISCON12_PENDING);
		break;
	case DL_PROV_RESET_PENDING:
		sl_set_l_state(sl, DL_DISCON13_PENDING);
		break;
	default:
		goto outstate;
	}
	return sl_send_maup_rel_req(sl, q, mp);
      oustate:
	return lmi_error_ack(sl, q, mp, SL_STOP_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_STOP_REQ, LMI_TOOSHORT);
}

/**
 * sl_retrieve_bsnt_req - process SL_RETRIEVE_BSNT_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_retrieve_bsnt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_IDLE)
		goto outstate;
	return sl_send_maup_retr_req(sl, q, mp, NULL);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_TOOSHORT);
}

/**
 * sl_retrieval_request_and_fsnc_req - process SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_retreival_request_and_fsnc_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;
	uint32_t fsnc;

	if (mp->b_wptr < mp->b_prtr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_IDLE)
		goto outstate;
	fnsc = sl->sl_fnsc;
	return sl_send_maup_retr_req(sl, q, mp, &fsnc);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_TOOSHORT);
}

/**
 * sl_clear_buffers_req - process SL_CLEAR_BUFFERS_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_clear_buffers_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto oustate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_FLUSH_BUFFERS);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_TOOSHORT);
}

/**
 * sl_clear_rtb_req - process SL_CLEAR_RTB_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_clear_rtb_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_CLEAR_RTB);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_RTB_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_RTB_REQ, LMI_TOOSHORT);
}

/**
 * sl_continue_req - process SL_CONTINUE_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_continue_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_continue_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_CONTINUE);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CONTINUE_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CONTINUE_REQ, LMI_TOOSHORT);
}

/**
 * sl_local_processor_outage_req - process SL_LOCAL_PROCESSOR_OUTAGE_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_local_processor_outage_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_LPO_SET);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_LOCAL_PROCESSOR_OUTAGE_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_LOCAL_PROCESSOR_OUTAGE_REQ, LMI_TOOSHORT);
}

/**
 * sl_resume_req - process SL_RESUME_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_resume_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_CONTINUE);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_RESUME_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_RESUME_REQ, LMI_TOOSHORT);
}

/**
 * sl_congestion_discard_req - process SL_CONGESTION_DISCARD_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_congestion_discard_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_cong_discard_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_CONG_DISCARD);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_TOOSHORT);
}

/**
 * sl_congestion_accept_req - process SL_CONGESTION_ACCEPT_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_congestion_accept_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_cong_accept_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_CONG_ACCEPT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_TOOSHORT);
}

/**
 * sl_no_congestion_req - process SL_NO_CONGESTION_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_no_congestion_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_no_cong_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_CONG_CLEAR);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_NO_CONGESTION_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_NO_CONGESTION_REQ, LMI_TOOSHORT);
}

/**
 * sl_power_on_req - process SL_POWER_ON_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_power_on_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_power_on_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sl_get_l_state(sl) != DL_UNBOUND)
		goto outstate;
	return sl_send_maup_state_req(sl, q, mp, M2UA_STATUS_AUDIT);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_POWER_ON_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(sl, q, mp, SL_POWER_ON_REQ, LMI_TOOSHORT);
}

#if 0
/**
 * sl_optmgmt_req - process SL_OPTMGMT_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
}
#endif

#if 0
/**
 * sl_notify_req - process SL_NOTIFY_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_notify_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
}
#endif

/**
 * sl_other_req - process SL_OTHER_REQ primtive
 * @sl: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_other_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	uint32_t *p = (typeof(p)) mp->b_rptr;

	return lmi_error_ack(sl, q, mp, *p, LMI_NOTSUPPORT);
}

/*
 *  SL-Provider (AS) Timeouts.
 *  ==========================
 */
static noinline fastcall int
sl_wack_aspac_timeout(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (sl_get_u_state(sl) == ASP_WACK_ASPIA)
		if ((err = sl_send_aspt_aspac_req(sl, q, NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
	return (err);
}
static noinline fastcall int
sl_wack_aspia_timeout(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (sl_get_u_state(sl) == ASP_WACK_ASPIA)
		if ((err = sl_send_aspt_aspia_req(sl, q, NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
	return (err);
}

/**
 * sl_w_data - process upper write M_DATA message
 * @q: active queue (upper write queue)
 * @mp: the message
 *
 * M_DATA is the preferred way of passing data messages to a signalling link.  The only time that an
 * SL_PDU_REQ primitive is required is for TTC (where the message priority must be passed in the
 * attached M_PROTO message block).  Even for TTC, the message priority can be passed in M_DATA as
 * the first by in the message block.
 *
 * What to do about messages that are too short or too long, or sent in the wrong state.  Probably
 * the best thing to do is to issue an M_ERROR message in response: that is to treat them as a fatal
 * error.
 */
static inline fastcall __hot_write int
sl_w_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	uint8_t pri;
	int err;

	if (!tp_trylock(tp, q))
		return (-EDEADLK);

	if (sl_get_l_state(sl) != DL_DATAXFER)
		goto outstate;
	if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
		err = sl_send_maup_data1(sl, q, NULL, mp);
	} else {
		pri = mp->b_rptr++;
		if ((err = sl_send_maup_data2(sl, q, NULL, mp, pri)) != 0)
			mp->b_rptr--;
	}

	tp_unlock(tp);
	return (err);
}

/**
 * sl_w_proto - process upper write M_PROTO/M_PCPROTO message
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
sl_w_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int old_i_state, old_u_state, old_n_state, old_a_state;
	int rtn = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t)) {
		strlog(sl->mid, sl->sid, 0, SL_TRACE | SL_ERROR, "-> primitive too short");
		freemsg(mp);
		return (0);
	}

	if (!tp_trylock(sl->tp, q))
		return (-EDEADLK);

	old_i_state = sl_get_i_state(sl);
	old_u_state = sl_get_u_state(sl);
	old_n_state = tp_get_i_state(sl->tp);
	old_a_state = tp_get_u_state(sl->tp);

	switch (*(uint32_t *) mp->b_rptr) {
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
	case LMI_OPTMGMT_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> LMI_OPTMGMT_REQ");
		rtn = lmi_optmgmt_req(sl, q, mp);
		break;
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
		rtn = sl_retreival_request_and_fsnc_req(sl, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CLEAR_BUFFERS_REQ");
		rtn = sl_clear_buffers_req(sl, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CLEAR_RTB_REQ");
		rtn = sl_clear_rtb_req(sl, q, mp);
		break;
	case SL_CONTINUE_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_CONTINUE_REQ");
		rtn = sl_continue_req(sl, q, mp);
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
#if 0
	case SL_OPTMGMT_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_OPTMGMT_REQ");
		rtn = sl_optmgmt_req(sl, q, mp);
		break;
	case SL_NOTIFY_REQ:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_NOTIFY_REQ");
		rtn = sl_notify_req(sl, q, mp);
		break;
#endif
	default:
		strlog(sl->mid, sl->sid, SLLOGRX, SL_TRACE, "-> SL_????_???");
		rtn = sl_other_req(sl, q, mp);
		break;
	}
	if (rtn < 0) {
		sl_set_i_state(sl, old_i_state);
		sl_set_u_state(sl, old_u_state);
		tp_set_i_state(sl->tp, old_n_state);
		tp_set_u_state(sl->tp, old_a_state);
	}
	tp_unlock(sl->tp);
	return (rtn);
}

/**
 * sl_w_ctl - process upper write M_CTL/M_PCCTL message
 * @q: active queue (upper write queue)
 * @mp: the message
 *
 * Eventually we process M_CTL messages like M_IOCTL messages, but passed from module to module.
 * For now we just discard them.
 */
static noinline fastcall __unlikely int
sl_w_ctl(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	strlog(sl->mid, sl->sid, 0, SL_ERROR | SL_TRACE, "unexpected M_CTL message on write queue");
	freemsg(mp);
	return (0);
}

/**
 * sl_w_ioctl - process upper write M_IOCTL message
 * @q: active queue (upper write queue)
 * @mp: the message
 *
 * For I_(P)LINK and I_(P)UNLINK operations, the tp structure was already allocated when the upper
 * stream was opened on the multiplexing driver.  We simply attach or detach the linked queue pair
 * from the structure.  There are some other input-output controls for configuration that we need ot
 * handle later, but we just return EINVAL for any other input-output control for now.
 */
static noinline fastcall __unlikely int
sl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *dp;

	if (!mp->b_cont)
		goto einval;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		switch (ioc->ioc_cmd) {
		case I_PLINK:
		case I_LINK:
		{
			struct linkblk *l;
			struct tp *tp = sl->tp;

			if (!mp->b_cont || tp->wq != NULL)
				break;

			l = (typeof(l)) mp->b_cont->b_rptr;

			tp_lock(tp);
			tp->index = l->l_index;
			tp->rq = RD(l->l_qtop);
			tp->wq = l->l_qtop;
			tp->rq->q_ptr = tp->wq->q_ptr = (void *) tp;
			tp_unlock(tp);
			mi_copy_done(q, mp, 0);
			return (0);
		}
		case I_PUNLINK:
		case I_UNLINK:
		{
			struct linkblk *l;
			struct tp *tp = sl->tp;

			if (!mp->b_cont)
				break;

			l = (typeof(l)) mp->b_cont->b_rptr;

			if (tp->index != l->l_index)
				break;

			tp_lock(tp);
			tp->rq->q_ptr = tp->wq->q_ptr = NULL;
			tp->rq = tp->wq = NULL;
			tp->index = 0;
			tp_unlock(tp);
			mi_copy_done(q, mp, 0);
			return (0);
		}
		}
		break;
	case SL_IOC_MAGIC:
	case SDT_IOC_MAGIC:
	case SDL_IOC_MAGIC:
		if (ioc->ioc_count < _IOC_SIZE(ioc->ioc_cmd) || sl_get_i_state(sl) == LMI_UNUSABLE) {
			strlog(sl->mid, sl->sid, 0, SL_ERROR,
			       "ioctl count = %d, size = %d, state = %d", (int) ioc->ioc_count,
			       (int) _IOC_SIZE(ioc->ioc_cmd), (int) sl_get_i_state(sl));
			break;
		}
		switch (_IOC_TYPE(ioc->ioc_cmd)) {
		case SL_IOC_MAGIC:
			switch (_IOC_NR(ioc->ioc_cmd)) {
			case _IOC_NR(SL_IOCGOPTIONS):
				if ((dp =
				     mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_option), 1))) {
					spin_lock_m2ua(sl);
					*(struct lmi_option *) dp->b_rptr = sl->option;
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SL_IOCSOPTIONS):
				mi_copyin(q, mp, NULL, sizeof(struct lmi_option));
				return (0);
			case _IOC_NR(SL_IOCGCONFIG):
				if ((dp =
				     mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_config), 1))) {
					struct sl_config *arg = (struct sl_config *) dp->b_rptr;

					spin_lock_m2ua(sl);
					*arg = sl->sl.config;
					/* convert hz to milliseconds */
					arg->t1 = drv_hztomsec(arg->t1);
					arg->t2 = drv_hztomsec(arg->t2);
					arg->t2l = drv_hztomsec(arg->t2l);
					arg->t2h = drv_hztomsec(arg->t2h);
					arg->t3 = drv_hztomsec(arg->t3);
					arg->t4n = drv_hztomsec(arg->t4n);
					arg->t4e = drv_hztomsec(arg->t4e);
					arg->t5 = drv_hztomsec(arg->t5);
					arg->t6 = drv_hztomsec(arg->t6);
					arg->t7 = drv_hztomsec(arg->t7);
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SL_IOCSCONFIG):
			case _IOC_NR(SL_IOCTCONFIG):
			case _IOC_NR(SL_IOCCCONFIG):
				mi_copyin(q, mp, NULL, sizeof(struct sl_config));
				return (0);
			case _IOC_NR(SL_IOCGSTATEM):
				if ((dp =
				     mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_statem), 1))) {
					spin_lock_m2ua(sl);
					sl_iocgstatem(sl, q, dp);
					*(struct sl_statem *) dp->b_rptr = sl->sl.statem;
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SL_IOCCMRESET):
				spin_lock_m2ua(sl);
				bzero(&sl->sl.statem, sizeof(sl->sl.statem));
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			case _IOC_NR(SL_IOCGSTATSP):
				if ((dp =
				     mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_stats), 1))) {
					spin_lock_m2ua(sl);
					*(struct sl_stats *) dp->b_rptr = sl->sl.statsp;
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SL_IOCSSTATSP):
				mi_copyin(q, mp, NULL, sizeof(struct sl_stats));
				return (0);
			case _IOC_NR(SL_IOCGSTATS):
				if ((dp =
				     mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_stats), 1))) {
					spin_lock_m2ua(sl);
					*(struct sl_stats *) dp->b_rptr = sl->sl.stats;
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SL_IOCCSTATS):
				spin_lock_m2ua(sl);
				bzero(&sl->sl.stats, sizeof(sl->sl.stats));
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			case _IOC_NR(SL_IOCGNOTIFY):
				if ((dp =
				     mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_notify), 1))) {
					spin_lock_m2ua(sl);
					*(struct sl_notify *) dp->b_rptr = sl->sl.notify;
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SL_IOCSNOTIFY):
			case _IOC_NR(SL_IOCCNOTIFY):
				mi_copyin(q, mp, NULL, sizeof(struct sl_notify));
				return (0);
			default:
				mi_copy_done(q, mp, EOPNOTSUPP);
				return (0);
			}
			break;
		case SDT_IOC_MAGIC:
			switch (_IOC_NR(ioc->ioc_cmd)) {
			case _IOC_NR(SDT_IOCGOPTIONS):
				if ((dp =
				     mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_option), 1))) {
					spin_lock_m2ua(sl);
					*(struct lmi_option *) dp->b_rptr = sl->option;
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SDT_IOCSOPTIONS):
				mi_copyin(q, mp, NULL, sizeof(struct lmi_option));
				return (0);
			case _IOC_NR(SDT_IOCGCONFIG):
				if ((dp =
				     mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_config), 1))) {
					spin_lock_m2ua(sl);
					*(struct sdt_config *) dp->b_rptr = sl->sdt.config;
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SDT_IOCSCONFIG):
			case _IOC_NR(SDT_IOCTCONFIG):
			case _IOC_NR(SDT_IOCCCONFIG):
				mi_copyin(q, mp, NULL, sizeof(sdt_option_t));
				return (0);
			case _IOC_NR(SDT_IOCGSTATEM):
			case _IOC_NR(SDT_IOCCMRESET):
			case _IOC_NR(SDT_IOCGSTATSP):
			case _IOC_NR(SDT_IOCSSTATSP):
			case _IOC_NR(SDT_IOCGSTATS):
			case _IOC_NR(SDT_IOCCSTATS):
			case _IOC_NR(SDT_IOCGNOTIFY):
			case _IOC_NR(SDT_IOCSNOTIFY):
			case _IOC_NR(SDT_IOCCNOTIFY):
			case _IOC_NR(SDT_IOCCABORT):
			default:
				mi_copy_done(q, mp, EOPNOTSUPP);
				return (0);
			}
			break;
		case SDL_IOC_MAGIC:
			switch (_IOC_NR(ioc->ioc_cmd)) {
			case _IOC_NR(SDL_IOCGOPTIONS):
				if ((dp = mi_copyout_alloc(q, mp, NULL, sizeof(lmi_option + t), 1))) {
					spin_lock_m2ua(sl);
					*(struct lmi_option *) dp->b_rptr = sl->option;
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SDL_IOCSOPTIONS):
				mi_copyin(q, mp, NULL, sizeof(struct lmi_option));
				return (0);
			case _IOC_NR(SDL_IOCGCONFIG):
				if ((dp =
				     mi_copyout_alloc(q, mp, NULL, sizeof(struct sdl_config), 1))) {
					spin_lock_m2ua(sl);
					*(struct sdl_config *) dp->b_rptr = sl->sdl.config;
					spin_unlock_m2ua(sl);
					mi_copyout(q, mp);
				}
				return (0);
			case _IOC_NR(SDL_IOCSCONFIG):
			case _IOC_NR(SDL_IOCTCONFIG):
			case _IOC_NR(SDL_IOCCCONFIG):
				mi_copyin(q, mp, NULL, sizeof(struct sdl_config));
				return (0);
			case _IOC_NR(SDL_IOCGSTATEM):
			case _IOC_NR(SDL_IOCCMRESET):
			case _IOC_NR(SDL_IOCGSTATSP):
			case _IOC_NR(SDL_IOCSSTATSP):
			case _IOC_NR(SDL_IOCGSTATS):
			case _IOC_NR(SDL_IOCCSTATS):
			case _IOC_NR(SDL_IOCGNOTIFY):
			case _IOC_NR(SDL_IOCSNOTIFY):
			case _IOC_NR(SDL_IOCCNOTIFY):
			case _IOC_NR(SDL_IOCCDISCTX):
			case _IOC_NR(SDL_IOCCCONNTX):
			default:
				miocnak(q, mp, 0, EOPNOTSUPP);
				return (0);
			}
			break;
		}
	}
      einval:
	miocnak(q, mp, 0, EINVAL);
	return (0);
}

/**
 * sl_w_iocdata - process upper write M_IOCDATA message
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
sl_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		return (0);
	default:
		mi_copy_done(q, mp, EPROTO);
		return (0);
	case MI_COPY_CASE(MI_COPY_IN, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case SL_IOC_MAGIC:
			switch (_IOC_NR(cp->cp_cmd)) {
			case SL_IOCSOPTIONS:
				spin_lock_m2ua(sl);
				sl->option = *(struct lmi_option *) dp->b_rptr;
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			case SL_IOCSCONFIG:
			{
				struct sl_config *arg = (typeof(arg)) dp->b_rptr;

				arg->t1 = drv_msectohz(arg->t1);
				arg->t2 = drv_msectohz(arg->t2);
				arg->t2l = drv_msectohz(arg->t2l);
				arg->t2h = drv_msectohz(arg->t2h);
				arg->t3 = drv_msectohz(arg->t3);
				arg->t4n = drv_msectohz(arg->t4n);
				arg->t4e = drv_msectohz(arg->t4e);
				arg->t5 = drv_msectohz(arg->t5);
				arg->t6 = drv_msectohz(arg->t6);
				arg->t7 = drv_msectohz(arg->t7);

				spin_lock_m2ua(sl);
				sl->sl.config = *arg;
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			}
			case SL_IOCTCONFIG:
			case SL_IOCCCONFIG:
			default:
				mi_copy_done(q, mp, EOPNOTSUPP);
				return (0);
			case SL_IOCSSTATSP:
				spin_lock_m2ua(sl);
				sl->sl.statsp = *(struct sl_stats *) dp->b_rptr;
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			case SL_IOCSNOTIFY:
				spin_lock_m2ua(sl);
				sl->sl.notify.events |= ((struct sl_notify *) dp->b_rptr)->events;
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			case SL_IOCCNOTIFY:
				spin_lock_m2ua(sl);
				sl->sl.notify.events &= ~((struct sl_notify *) dp->b_rptr)->events;
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			}
			break;
		case SDT_IOC_MAGIC:
			switch (_IOC_NR(cp->cp_cmd)) {
			case SDT_IOCSOPTIONS:
				spin_lock_m2ua(sl);
				sl->option = *(struct lmi_option *) dp->b_rptr;
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			case SDT_IOCSCONFIG:
				spin_lock_m2ua(sl);
				sl->sdt.config = *(struct sdt_config *) dp->b_rptr;
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			case SDT_IOCTCONFIG:
			case SDT_IOCCCONFIG:
			case SDT_IOCSSTATSP:
			case SDT_IOCSNOTIFY:
			case SDT_IOCCNOTIFY:
			default:
				mi_copy_done(q, mp, EOPNOTSUPP);
				return (0);
			}
			break;
		case SDL_IOC_MAGIC:
			switch (_IOC_NR(cp->cp_cmd)) {
			case SDL_IOCSOPTIONS:
				spin_lock_m2ua(sl);
				sl->option = *(struct lmi_option *) dp->b_rptr;
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			case SDL_IOCSCONFIG:
				spin_lock_m2ua(sl);
				sl->sdl.config = *(struct sdl_config *) dp->b_rptr;
				spin_unlock_m2ua(sl);
				mi_copy_done(q, mp, 0);
				return (0);
			case SDL_IOCTCONFIG:
			case SDL_IOCCCONFIG:
			case SDL_IOCSSTATSP:
			case SDL_IOCSNOTIFY:
			case SDL_IOCCNOTIFY:
			default:
				mi_copy_done(q, mp, EOPNOTSUPP);
				return (0);
			}
			break;
		}
		mi_copy_done(q, mp, EINVAL);
		return (0);
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		mi_copy_done(q, mp, 0);
		return (0);
	}
}

/**
 * sl_w_sig - process upper write M_SIG/M_PCSIG message
 * @q: active queue (upper write queue)
 * @mp: the message
 *
 * M_PCSIG messages are issued from mi timers.
 */
static noinline fastcall __unlikely int
sl_w_sig(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int rtn = 0;

	if (!tp_trylock(sl->tp, q))
		return (-EDEADLK);

	if (likely(mi_timer_valid(mp))) {
		switch (*(int *) mp->b_rptr) {
		case wack_aspac:
			strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> WACK ASPAC TIMEOUT <-");
			rtn = sl_wack_aspac_timeout(sl, q, mp);
			break;
		case wack_aspia:
			strlog(sl->mid, sl->sid, SLLOGTO, SL_TRACE, "-> WACK ASPIA TIMEOUT <-");
			rtn = sl_wack_aspia_timeout(sl, q, mp);
			break;
		default:
			strlog(sl->mid, sl->sid, 0, SL_ERROR | SL_TRACE "invalid timer %d",
			       *(int *) mp->b_rptr);
			rtn = 0;
			break;
		}
	}

	tp_unlock(sl->tp);
	return (rtn);
}

/**
 * sl_w_flush - process upper write M_FLUSH message
 * @q: active queue (upper write queue)
 * @mp: the message
 * 
 * What we should probably do here is send a BEAT message on the SCTP stream id that corresponds to
 * the SL and the return the flush message back upstream only after we get the BEAT ACK. (Well, no,
 * holding a priority message will accomplish nothing.
 */
static noinline fastcall __unlikely int
sl_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		qreply(q, 0);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * sl_w_other - process upper write other message
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static inline fastcall __unlikely int
sl_w_other(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	strlog(sl->mid, sl->sid, 0, SL_ERROR | SL_TRACE,
	       "unhandled STREAMS message on write queue");
	freemsg(mp);
	return (0);
}

/**
 * sl_msg - process an SL-user message
 * @q: active queue (upper write queue)
 * @mp: message to process
 */
static int
sl_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_w_proto(q, mp);
	case M_CTL:
		return sl_w_ctl(q, mp);
	case M_IOCTL:
		return sl_w_ioctl(q, mp);
	case M_IOCDATA:
		return sl_w_iocdata(q, mp);
	case M_SIG:
	case M_PCSIG:
		return sl_w_sig(q, mp);
	case M_FLUSH:
		return sl_w_flush(q, mp);
	default:
		return sl_w_other(q, mp);
	}
}

/*
 * TS-Provider to TS-User primitives.
 * ==================================
 */

/**
 * t_conn_ind: - process T_CONN_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 *
 * We basically allow the linked SCTP T-Provider Stream to be in any state whatsoever for maximum
 * compatibility with possible use cases.  The only time that we would get an T_CONN_IND on a Stream
 * is if the Stream was bound as a listening Stream, in which case, we should complete the
 * connection by responding with the listening Stream as the accepting Stream and move the Stream to
 * the established state.  Also, we should save the connecting address(es) so that we can reconnect
 * the Stream at a later point.
 *
 * Because we move to the established state (TS_DATA_XFER) at the end of this procedure, we should
 * also determine whether there are any signalling links that are in a pending state, either
 * LMI_ATTACH_PENDING or LMI_ENABLE_PENDING and complete their procedures.  If there are any
 * signalling links in the pending state going down or are in a state better than LMI_DISABLED, they
 * need to be downgraded to at least disabled.  If the SG processes registration, all signalling
 * links must be considered in the unregistered state.
 */
static noinline fastcall int
t_conn_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_conn_con: - process T_CONN_CON primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 *
 * The connection of an association is being confirmed.  This is either the original connection
 * confirmation or is a connection confirmation resutling from a reconnect.  In either case, we want
 * to issue an T_ADDR_REQ to determine the local and remote addresses.
 */
static noinline fastcall int
t_conn_con(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_con *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_CON out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_CON too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_discon_ind: - process T_DISCON_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 *
 * A disconnect indication corresponds to a disable indication for each of the enabled signalling
 * links being served by this association.  We issue an error indication for each enabled signalling
 * link and mark it as a fatal error.  Then we should attempt to reconnect the association.  What we
 * need to do as follows: when an association has been in the connected state previously, it should
 * store the remote address.  Then, when we wish to reconnect the association, we should use the
 * stored address in the connect request.
 */
static noinline fastcall int
t_discon_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_discon_ind *p = (typeof(p)) mp->b_rptr;
	struct sl *sl;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	for (sl = tp->list; sl; sl = sl->next) {
		int oldstate = sl_get_i_state(sl);

		switch (oldstate) {
		case LMI_ENABLED:
		case LMI_ENABLE_PENDING:
			sl_set_i_state(sl, LMI_DISABLED);
			if ((err = lmi_error_ind(sl, q, mp, 0, true)) != 0) {
				sl_set_i_state(sl, oldstate);
				break;
			}
			continue;
		default:
			/* this is an error */
			continue;
		}
		break;
	}
	if (err)
		return (err);
	tp_set_i_state(tp, TS_IDLE);
	/* XXX: we should attempt to reconnect the association. */
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DISCON_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DISCON_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_data_ind: - process T_DATA_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 *
 * When we receive data on a Stream, we take the M2UA part and pass it for processing.  Note that we
 * should check the PPI to ensure that it is either the M2UA PPI or PPI zero.
 */
static noinline fastcall int
t_data_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_data_ind *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DATA_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DATA_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_exdata_ind: - process T_EXDATA_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 *
 * When we receive data on a Stream, we take the M2UA part and pass it for processing.  Note that we
 * should check the PPI to ensure that it is either the M2UA PPI or PPI zero.
 */
static noinline fastcall int
t_exdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_EXDATA_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_EXDATA_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_info_ack: - process T_INFO_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 *
 * Whenever and T_INFO_ACK is returned we process and save the information from the acknowledgement.
 * Also, the state is changed to synchronize with the information acknowledgement.
 */
static noinline fastcall int
t_info_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_info_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_INFO_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_bind_ack: - process T_BIND_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_bind_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_bind_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_BIND_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_BIND_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_error_ack: - process T_ERROR_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_error_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_error_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ERROR_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ERROR_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_ok_ack: - process T_OK_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_ok_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ok_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OK_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OK_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_unitdata_ind: - process T_UNITDATA_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_unitdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UNITDATA_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UNITDATA_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_uderror_ind: - process T_UDERROR_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_uderror_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_uderror_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UDERROR_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UDERROR_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_optmgmt_ack: - process T_OPTMGMT_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTMGMT_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTMGMT_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_ordrel_ind: - process T_ORDREL_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ORDREL_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ORDREL_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_optdata_ind: - process T_OPTDATA_IND primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_optdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTDATA_IND out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTDATA_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_addr_ack: - process T_ADDR_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_addr_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_addr_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ADDR_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ADDR_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_capability_ack: - process T_CAPABILITY_ACK primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_capability_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_capability_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
	if (tp_get_i_state(tp) != TS_IDLE)
		goto outstate;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CAPABILITY_ACK out of state");
	goto error;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CAPABILITY_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_other_ind: - process unknown primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_other_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	t_scalar_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto fault;
      fault:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "Primitive too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  TS-User (APS) Timeouts.
 *  =======================
 */
static noinline fastcall int
tp_wack_aspup_timeout(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (tp_get_u_state(tp) == ASP_WACK_ASPUP)
		if ((err = tp_send_asps_aspup_req(tp, q, NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
	return (err);
}
static noinline fastcall int
tp_wack_aspdn_timeout(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (tp_get_u_state(tp) == ASP_WACK_ASPDN)
		if ((err = tp_send_asps_aspdn_req(tp, q, NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
	return (err);
}
static noinline fastcall int
tp_wack_aspia_timeout(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (tp_get_u_state(tp) == ASP_WACK_ASPIA)
		if ((err = tp_send_aspt_aspia_req(tp, q, NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
	return (err);
}
static noinline fastcall int
tp_wack_aspac_timeout(struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (tp_get_u_state(tp) == ASP_WACK_ASPAC)
		if ((err = tp_send_aspt_aspac_req(tp, q, NULL, NULL, 0)) == 0)
			mi_timer(mp, 2000);
	return (err);
}

/**
 * tp_r_data - process lower read M_DATA message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static inline fastcall __hot_in int
tp_r_data(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_proto - process lower read M_PROTO/M_PCPROTO message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
tp_r_proto(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	int old_i_state, old_u_state;
	uint32_t prim;
	int rtn = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t)) {
		strlog(tp->mid, tp->sid, 0, SL_TRACE | SL_ERROR, "primitive too short <-");
		freemsg(mp);
		return (0);
	}
	if (!pcmsg(DB_TYPE(mp)) && oldstate == -1) {
		/* If we have not yet received a response to our T_INFO_REQ and this is not the
		   response, wait for the response (one _is_ coming).  Of course, if this is a
		   priority message it must be processed immediately anyway. */
		return (-EAGAIN);
	}
	if (!tp_trylock(tp, q))
		return (-EDEADLK);

	old_i_state = tp_get_i_state(tp);
	old_u_state = tp_get_u_state(tp);

	switch (prim) {
		case T_CONN_IND strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_CONN_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_CONN_CON strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_CONN_CON <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_DISCON_IND strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_DISCON_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_DATA_IND strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_DATA_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_EXDATA_IND strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_EXDATA_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_INFO_ACK strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_INFO_ACK <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_BIND_ACK strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_BIND_ACK <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_ERROR_ACK strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_ERROR_ACK <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_OK_ACK strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_OK_ACK <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_UNITDATA_IND
		    strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_UNITDATA_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_UDERROR_IND strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_UDERROR_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_OPTMGMT_ACK strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_OPTMGMT_ACK <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_ORDREL_IND strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_ORDREL_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_OPTDATA_IND strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_OPTDATA_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_ADDR_ACK strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_ADDR_ACK <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
		case T_CAPABILITY_ACK
		    strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "T_CAPABILITY_ACK <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
	default:
		strlog(tp->mid, tp->sid, SLLOGRX, SL_TRACE, "N_????_??? <-");
		rtn = t_other_ind(tp, q, mp);
		break;
	}
	if (rtn < 0) {
		tp_set_i_state(tp, old_i_state);
		tp_set_u_state(tp, old_u_state);
	}
	tp_unlock(tp);
	return (rtn);
}

/**
 * tp_r_sig - process lower read M_SIG/M_PCSIG message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
tp_r_sig(queue_t *q, mblk_t *mp)
{
	struct tp *tp = TP_PRIV(q);
	int rtn = 0;

	if (!tp_trylock(tp, q))
		return (-EDEADLK);

	if (likely(mi_timer_valid(mp))) {
		switch (*(int *) mp->b_rptr) {
		case wack_aspup:
			strlog(tp->mid, tp->sid, SLLOGTO, SL_TRACE, "-> WACK ASPUP TIMEOUT <-");
			rtn = tp_wack_aspup_timeout(tp, q, mp);
			break;
		case wack_aspdn:
			strlog(tp->mid, tp->sid, SLLOGTO, SL_TRACE, "-> WACK ASPDN TIMEOUT <-");
			rtn = tp_wack_aspdn_timeout(tp, q, mp);
			break;
		case wack_aspia:
			strlog(tp->mid, tp->sid, SLLOGTO, SL_TRACE, "-> WACK ASPIA TIMEOUT <-");
			rtn = tp_wack_aspia_timeout(tp, q, mp);
			break;
		case wack_aspac:
			strlog(tp->mid, tp->sid, SLLOGTO, SL_TRACE, "-> WACK ASPAC TIMEOUT <-");
			rtn = tp_wack_aspac_timeout(tp, q, mp);
			break;
		default:
			strlog(tp->mid, tp->sid, 0, SL_ERROR | SL_TRACE, "invalid timer %d",
			       *(int *) mp->b_rptr);
			rtn = 0;
			break;
		}
	}
	tp_unlock(tp);
	return (rtn);
}

/**
 * tp_r_ctl - process lower read M_CTL/M_PCCTL message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
tp_r_ctl(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_error - process lower read M_ERROR/M_HANGUP/M_UNHANGUP message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
tp_r_error(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_iocack - process lower read M_IOCACK/M_IOCNAK message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
tp_r_iocack(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_copy - process lower read M_COPYIN/M_COPYOUT message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
tp_r_copy(queue_t *q, mblk_t *mp)
{
}

/**
 * tp_r_flush - process lower read M_FLUSH message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
tp_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		qreply(q, 0);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * tp_r_other - process lower read other message
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static noinline fastcall __unlikely int
tp_r_other(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * tp_msg - process an N-provider message
 * @q: active queue (lower read queue)
 * @mp: message to process
 */
static int
tp_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return tp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tp_r_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tp_r_sig(q, mp);
	case M_CTL:
		return tp_r_ctl(q, mp);
	case M_ERROR:
	case M_HANGUP:
	case M_UNHANGUP:
		return tp_r_error(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return tp_r_iocack(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return tp_r_copy(q, mp);
	case M_FLUSH:
		return tp_r_flush(q, mp);
	default:
		return tp_r_other(q, mp);
	}
}

/*
 *  PUT and SERVICE procedure.
 */

/**
 * sl_rsrv - upper read service procedure
 * @q: queue to service (upper read queue)
 *
 * This service procedure is simply to perform backenabling across the
 * multiplexing driver for flow control.  When invoked, all lower read queues
 * feeding this upper read queue have their service procedures enabled.
 */
static streamscall
sl_rsrv(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);

	if (sl->tp)
		qenable(sl->tp->rq);
	return (0);
}

/**
 * sl_rput - upper read put procedure
 * @q: active queue (upper read queue)
 * @mp: message to put
 *
 * The upper read put procedure is not used.  It is a software error if this
 * procedure is called.  Messages should be put with putnext to the queue
 * following this one.
 */
static streamscall
sl_rput(queue_t *q, mblk_t *mp)
{
	/* this is an error */
	putnext(q, mp);
	return (0);
}

/**
 * sl_wsrv - upper write service procedure
 * @q: queue to service (upper write queue)
 *
 * This is a canonical draining service procedure.
 */
static streamscall
sl_wsrv(queue_t *q)
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

/**
 * sl_wput - upper write put procedure
 * @q: active queue (upper write queue)
 * @mp: message to put
 *
 * This is a canoncical put procedure.  Messages are processed immediately if
 * possible, otherwise they are queued.  Normal priority messages are queued
 * if any messages are already queued.  The processing procedure returns
 * non-zero if the message is to be queued and returns zero if the message has
 * been processed.
 */
static streamscall
sl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || sl_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * tp_rput - lower read put procedure
 * @q: active queue (lower read queue)
 * @mp: message to put
 *
 * This is a canonical put procedure.  Messages are processed immediately if
 * possible, otherwise they are queued.  Normal priority message are queue
 * if any messages are already queued.  The processing procedure returns
 * non-zero if the message is to be queued and returns zero if the message has
 * been processed.
 */
static streamscall
tp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || tp_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * tp_rsrv - lower read service procedure
 * @q: queue to service (lower read queue)
 *
 * This is a canonical draining service procedure.
 */
static streamscall
tp_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tp_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/**
 * tp_wput - lower write put procedure
 * @q: active queue (lower write queue)
 * @mp: message to put
 *
 * The lower write put procedure is not used.  It is a software error if this
 * procedure is called.  Messages should be put with putnext to the queue
 * following this one.
 */
static streamscall
tp_wput(queue_t *q, mblk_t *mp)
{
	/* this is an error */
	putnext(q, mp);
	return (0);
}

/**
 * tp_wsrv - lower write service procedure.
 * @q: queue to service (lower write queue)
 *
 * This service procedure is simply to perform backenabling across the
 * multiplexing driver for flow control.  When invoked, all upper write queues
 * feeding this lower write queue have their service procedures enabled.
 */
static streamscall
tp_wsrv(queue_t *q)
{
	struct sl *sl;
	pl_t pl;

	pl = RW_RDLOCK(&mux_lock, plstr);
	for (sl = tp->list; sl; sl = sl->next)
		if (sl->wq)
			qenable(sl->wq);
	RW_UNLOCK(&mux_loc, pl);
	return (0);
}

/*
 *  OPEN and CLOSE.
 */
static caddr_t sl_opens = NULL;		/* open list */

/**
 * sl_qopen - driver open routine
 * @q: newly created queue pair
 * @devp: pointer to device number associated with Stream
 * @oflags: flags to the open(2) call
 * @sflag: STREAMS flag
 * @crp: pointer to the credentials of the opening process
 *
 * When a Stream is opened on the driver it corresponds to an AS assocaited with a given ASP.  The
 * ASP is determined from the minor device number opened.  All minor devices corresponding to ASPs
 * are clone devices.  There is precisely one SCTP lower Stream for each ASP (for M2UA).  If an TP
 * structure has not been allocated for the corresponding minor device number, we allocate one.
 * When an SCTP Stream is I_LINK'ed under the driver, it is associated with the TP structure.  An SL
 * structure is allocated and associated with each each upper Stream.
 *
 * This driver can also be pushed as a module.  When pushed as a module it is pushed over an SCTP
 * Stream.  When pushed as a module, there exists precisely one AS.  (It takes a multiplexing driver
 * to provide more than on AS for a given SCTP association.)  This is a simplified case.  We will
 * handle this one later, but instead of allocating an SL structure and an TP structure, they should
 * be allocated together.
 */
static streamscall int
sl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	mblk_t *t1, *t2;
	minor_t cminor;
	major_t cmajor;
	struct sl *sl;
	struct tp *tp;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || q->q_next)
		return (ENXIO);

	cmajor = getmajor(*devp);
	cminor = getminor(*devp);

	if (cminor > NUM_AS)
		return (ENXIO);

	if (!(tp = m2_get_asp(cminor)))
		return (ENXIO);

	*devp = makedevice(cmajor, NUM_AS + 1);
	sflag = CLONEOPEN;

	if (!(t1 = mi_timer_alloc(WR(q), sizeof(int))))
		return (ENOBUFS);
	if (!(t2 = mi_timer_alloc(WR(q), sizeof(int)))) {
		mi_timer_free(t1);
		return (ENOBUFS);
	}

	if ((err = mi_open_comm(&sl_opens, q, devp, oflags, sflag, crp))) {
		m2_put_asp(tp);
		mi_timer_free(t1);
		mi_timer_free(t2);
		return (err);
	}
	/* initialize sl structure */
	sl = SL_PRIV(q);
	sl_init_priv(sl, tp, q, devip, oflags, sflag, crp, t1, t2, cminor);
	return (0);
}

/**
 * sl_qclose - driver close routine
 * @q: queue pair
 * @oflags: flags to the open(2) call
 * @crp: pointer to the credentials of the closing process
 */
static streamscall int
sl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct sl *sl = SL_PRIV(q);

	qprocsoff(q);
	mi_detach(q, (caddr_t) sl);
	sl_term_priv(sl);
	mi_close_detached(&sl_opens, (caddr_t) sl);
	return (0);
}

/*
 *  STREAMS Definitions.
 */

static struct module_stat sl_mstat = __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct module_info tp_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat tp_mstat = __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct qinit sl_rinit = {
	.qi_putp = sl_rput,
	.qi_srvp = sl_rsrv,
	.qi_qopen = sl_qopen,
	.qi_qclose = sl_qclose,
	.qi_minfo = &sl_minfo,
	.qi_mstat = &sl_mstat,
};

static struct qinit sl_winit = {
	.qi_putp = sl_wput,
	.qi_srvp = sl_wsrv,
	.qi_minfo = &sl_minfo,
	.qi_mstat = &sl_mstat,
};

static struct qinit tp_rinit = {
	.qi_putp = tp_rput,
	.qi_srvp = tp_rsrv,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_mstat,
};

static struct qinit tp_winit = {
	.qi_putp = tp_wput,
	.qi_srvp = tp_wsrv,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_mstat,
};

static struct streamtab m2ua_asinfo = {
	.st_rdinit = &sl_rinit,
	.st_wrinit = &sl_winit,
	.st_muxrinit = &tp_rinit,
	.st_muxwinit = &tp_winit,
};

static struct cdevsw m2_cdev = {
	.d_name = DRV_NAME,
	.d_str = &m2ua_asinfo,
	.d_flag = D_MP,
	.d_kmod = THIS_MODULE,
};

unsigned short major = DRV_CMAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, ushort, DRV_CMAJOR);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for M2UA-AS driver.  (0 for allocation.)");

static __init int
m2ua_asinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_SPLASH);
	if ((err = register_strdev(&m2_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register major %d\n", DRV_NAME, (int) major);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

static __exit void
m2ua_asexit(void)
{
	int err;

	if ((err = unregister_strdev(&m2_cdev, major)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d\n", DRV_NAME, err);
	return;
}

module_init(m2ua_asinit);
module_exit(m2ua_asexit);
