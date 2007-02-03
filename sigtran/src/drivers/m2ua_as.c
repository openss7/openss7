/*****************************************************************************

 @(#) $RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/02/03 03:07:44 $

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

 Last Modified $Date: 2007/02/03 03:07:44 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m2ua_as.c,v $
 Revision 0.9.2.7  2007/02/03 03:07:44  brian
 - working up drivers

 Revision 0.9.2.6  2007/01/28 13:04:31  brian
 - reworked locking

 Revision 0.9.2.5  2007/01/28 01:09:40  brian
 - updated test programs and working up m2ua-as driver

 Revision 0.9.2.4  2007/01/26 21:54:33  brian
 - working up AS drivers and docs

 Revision 0.9.2.3  2007/01/23 10:00:45  brian
 - added test program and m2ua-as updates

 Revision 0.9.2.2  2007/01/21 20:22:34  brian
 - working up drivers

 Revision 0.9.2.1  2007/01/15 11:58:42  brian
 - added new m2ua-as mux files

 *****************************************************************************/

#ident "@(#) $RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/02/03 03:07:44 $"

static char const ident[] =
    "$RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/02/03 03:07:44 $";

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
 *
 *  Notes:
 *  Originally I thought that the LMI state would track the AS state; however, with that approach
 *  there were too many LMI_ERROR_IND conditions with state changes that could confuse an simple
 *  SL-User (such as that of MTP that expects that once signalling links are enabled, they stay
 *  enabled).  Now I am taking the approach that if the AS state cannot support MAUP message
 *  communications, that the signlling link be put in the local processor outage state.  This also
 *  means that when an AS state improves, that the local processor outage state be lifted or other
 *  actions taken, like link failure, after an audit is performed on the signalling link.  This
 *  makes automatic procedures more diverse and complicated, but it is better for transparency to
 *  the SL-User.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _MAC_SOURCE	1

#define _DEBUG	1
//#undef    _DEBUG

#include <stdbool.h>
#include <sys/os7/compat.h>
#include <sys/strsun.h>

#undef DB_TYPE
#define DB_TYPE(mp) mp->b_datap->db_type

#include <linux/socket.h>
#include <net/ip.h>

#include <sys/tihdr.h>
#include <sys/xti.h>
#include <sys/xti_inet.h>
#include <sys/xti_sctp.h>	/* For talking to SCTP */

#include <sys/dlpi.h>		/* For data link states. */

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

//#include <sys/ua_ioctl.h>
#include <sys/m2ua_lm.h>	/* M2UA Layer Management interface */
#include <sys/m2ua_as_ioctl.h>	/* M2UA-AS input-output controls */

#include "ua_msg.h"		/* UA message definitions */

#define UALOGST	    1		/* loc UA state transitions */
#define UALOGTO	    2		/* loc UA timeouts */
#define UALOGRX	    3		/* loc UA primitives received */
#define UALOGTX	    4		/* loc UA primitives issued */
#define UALOGTE	    5		/* loc UA timer events */
#define UALOGDA	    6		/* loc UA data */

/* ============================== */

#define M2UA_AS_DESCRIP		"M2UA/SCTP SIGNALLING LINK (SL) STREAMS MULTIPLEXING DRIVER."
#define M2UA_AS_REVISION	"OpenSS7 $RCSfile: m2ua_as.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/02/03 03:07:44 $"
#define M2UA_AS_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define M2UA_AS_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define M2UA_AS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define M2UA_AS_LICENSE		"GPL"
#define UA_AS_BANNER		M2UA_AS_DESCRIP		"\n" \
				M2UA_AS_REVISION	"\n" \
				M2UA_AS_COPYRIGHT	"\n" \
				M2UA_AS_DEVICE		"\n" \
				M2UA_AS_CONTACT		"\n"
#define UA_AS_SPLASH		M2UA_AS_DESCRIP		" - " \
				M2UA_AS_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M2UA_AS_CONTACT);
MODULE_DESCRIPTION(M2UA_AS_DESCRIP);
MODULE_SUPPORTED_DEVICE(M2UA_AS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M2UA_AS_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-m2ua_as");
MODULE_ALIAS("streams-m2ua-as");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define M2UA_AS_DRV_ID		CONFIG_STREAMS_M2UA_AS_MODID
#define M2UA_AS_DRV_NAME	CONFIG_STREAMS_M2UA_AS_NAME
#define M2UA_AS_MAJORS		CONFIG_STREAMS_M2UA_AS_NMAJORS
#define M2UA_AS_CMAJOR_0	CONFIG_STREAMS_M2UA_AS_MAJOR
#define M2UA_AS_UNITS		CONFIG_STREAMS_M2UA_AS_NMINORS
#endif				/* LFS */

#define DRV_ID		M2UA_AS_DRV_ID
#define DRV_NAME	M2UA_AS_DRV_NAME
#define DRV_CMAJOR	M2UA_AS_CMAJOR_0
#ifdef MODULE
#define DRV_SPLASH	UA_AS_BANNER
#else				/* MODULE */
#define DRV_SPLASH	UA_AS_SPLASH
#endif				/* MODULE */

/*
 *  M2UA-AS Private Structures
 *  ---------------------------
 *  Several structures are necessary.  We need one private structure for SL streams on the upper
 *  multiplex.  We need one private structure for the TP streams on the lower multiplex.  We also
 *  need one private structure for the ASP which is associated with the minor device number of the
 *  clone minor that is used to open an AS stream.  Then we need some sort of mapping structure that
 *  maps SCTP associations to ASPs (TP Streams to ASP structures), and some sore of mapping
 *  structure that maps AS Streams to ASPs (SL Streams to ASP structures).  So we need three primary
 *  data structures and two mapping structures for connecting the three into a directed graph.
 */

struct up;				/* upper multiplex structure (UP Stream) */
struct as;				/* AS Structure */
struct sg;				/* SG Structure */
struct tp;				/* lower multiplex structure (TP Stream) */

struct up_timer {
	int timer;			/* timer number */
	struct as *as;			/* application server */
};

struct bc {
	bcid_t rbid;			/* read bufcall id */
	bcid_t wbid;			/* write bufcall id */
};

struct up {
	struct bc bc;			/* must be first */
	struct up *next;		/* list linkage */
	struct up **prev;		/* list linkage */
	struct {
		struct up *next;	/* UP attached to same SG */
		struct up **prev;	/* UP attached to same SG */
		struct tp *tp;		/* the SG to which we belong */
	} tp;

	queue_t *rq;			/* RD queue */
	queue_t *wq;			/* WR queue */
	dev_t dev;
	cred_t cred;

	uint mid;			/* module id */
	uint sid;			/* stream id */
	uint unit;			/* unit opened */

	uint state;			/* AS state */
	uint l_state;			/* link state */

	struct {
		uint pending;
		uint operate;
	} status;

	uint32_t cong;			/* congestion onset */
	uint32_t disc;			/* congestion discard */

	mblk_t *wack_aspac;		/* Timer waiting for ASPAC Ack */
	mblk_t *wack_aspia;		/* Timer waiting for ASPIA Ack */
	mblk_t *wack_hbeat;		/* Timer waiting for BEAT Ack */

	lmi_info_ack_t info;

	struct {
		uint id;
		uint flags;
		uint state;
		struct m2ua_opt_conf_lm options;
		struct m2ua_conf_lm config;
		struct m2ua_statem_lm statem;
		struct m2ua_stats_lm statsp;
		struct m2ua_stats_lm stats;
		struct lmi_notify events;
	} lm;
	struct {
		uint id;
		uint flags;
		uint state;
		struct lmi_option option;	/* SL protocol options */
		struct sl_config config;	/* SL configuration options */
		struct sl_statem statem;	/* SL state machine variables */
		struct sl_notify notify;	/* SL notification options */
		struct sl_stats stats;	/* SL statistics */
		struct sl_stats stamp;	/* SL statistics timestamps */
		struct sl_stats statsp;	/* SL statistics periods */
	} up;
	struct {
		struct sdt_timers timers;
		struct sdt_config config;
	} sdt;
	struct {
		struct sdl_timers timers;
		struct sdl_config config;
	} sdl;

	struct {
		uint id;
		uint flags;
		uint state;
		uint16_t streamid;	/* SCTP stream identifier for this AS */
		uint32_t request_id;	/* registration request id */
		struct ua_opt_conf_as options;
		struct ua_conf_as config;
		struct ua_statem_as statem;
		struct ua_stats_as statsp;
		struct ua_stats_as stats;
		struct lmi_notify events;
	} as;
};

struct tp {
	struct bc bc;			/* must be first */
	struct tp *next;		/* list linkage */
	struct tp **prev;		/* list linkage */
	struct {
		struct up *list;	/* list of UP structures associated with this SG */
	} up;
	struct up *lm;

	spinlock_t lock;		/* lock: structure lock */
	uint users;			/* lock: number of users */
	queue_t *waitq;			/* lock: queue waiting for lock */

	queue_t *rq;			/* RD queue */
	queue_t *wq;			/* WR queue */
	cred_t cred;			/* credentials */

	uint mid;			/* module id */
	uint sid;			/* stream id */

	uint state;			/* ASP state */
	uint flags;			/* SG options flags */

	uint16_t nextstream;		/* next stream to be allocated to an interface */
	uint32_t acceptorid;
	mblk_t *wack_aspup;		/* Timer waiting for ASPUP Ack */
	mblk_t *wack_aspdn;		/* Timer waiting for ASPDN Ack */
	mblk_t *wack_aspac;		/* Timer waiting for ASPAC Ack */
	mblk_t *wack_aspia;		/* Timer waiting for ASPIA Ack */
	mblk_t *wack_hbeat;		/* Timer waiting for BEAT Ack */
	struct T_info_ack info;

	struct lmi_option options;
	struct {
		uint id;
		uint flags;
		uint state;
		struct ua_opt_conf_sp options;
		struct ua_conf_sp config;
		struct ua_statem_sp statem;
		struct ua_stats_sp statsp;
		struct ua_stats_sp stats;
		struct lmi_notify events;
	} sp;
	struct {
		uint id;		/* SG identifier */
		uint flags;
		uint state;
		struct ua_opt_conf_sg options;
		struct ua_conf_sg config;
		struct ua_statem_sg statem;
		struct ua_stats_sg statsp;
		struct ua_stats_sg stats;
		struct lmi_notify events;
	} sg;
	struct {
		uint id;		/* GP identifier */
		uint flags;
		uint state;
		struct ua_opt_conf_gp options;
		struct ua_conf_gp config;
		struct ua_statem_gp statem;
		struct ua_stats_gp statsp;
		struct ua_stats_gp stats;
		struct lmi_notify events;
	} gp;
	struct {
		uint id;		/* XP identifier */
		uint flags;
		uint state;
		struct ua_opt_conf_xp options;
		struct ua_conf_xp config;
		struct ua_statem_xp statem;
		struct ua_stats_xp statsp;
		struct ua_stats_xp stats;
		struct lmi_notify events;
	} xp;
};

/* status */
#define M2UA_SG_STATUS_LPO_SET		(1<<M2UA_STATUS_LPO_SET)
#define M2UA_SG_STATUS_LPO_CLEAR	(1<<M2UA_STATUS_LPO_CLEAR)
#define M2UA_SG_STATUS_EMER_SET		(1<<M2UA_STATUS_EMER_SET)
#define M2UA_SG_STATUS_EMER_CLEAR	(1<<M2UA_STATUS_EMER_CLEAR)
#define M2UA_SG_STATUS_FLUSH_BUFFERS	(1<<M2UA_STATUS_FLUSH_BUFFERS)
#define M2UA_SG_STATUS_CONTINUE		(1<<M2UA_STATUS_CONTINUE)
#define M2UA_SG_STATUS_CLEAR_RTB	(1<<M2UA_STATUS_CLEAR_RTB)
#define M2UA_SG_STATUS_AUDIT		(1<<M2UA_STATUS_AUDIT)
#define M2UA_SG_STATUS_CONG_CLEAR	(1<<M2UA_STATUS_CONG_CLEAR)
#define M2UA_SG_STATUS_CONG_ACCEPT	(1<<M2UA_STATUS_CONG_ACCEPT)
#define M2UA_SG_STATUS_CONG_DISCARD	(1<<M2UA_STATUS_CONG_DISCARD)

/* flags */
#define UA_SG_CONFIGURED	0x01	/* SG has been configured by ioctl */
#define UA_SG_LOCADDR		0x02	/* SG has local address determined */
#define UA_SG_REMADDR		0x04	/* SG has remote address determined */

#ifndef M2UA_PPI
#define M2UA_PPI    5
#endif

struct df {
	struct ua_opt_conf_df options;
	struct ua_conf_df config;
	struct ua_statem_df statem;
	struct ua_stats_df statsp;
	struct ua_stats_df stats;
	struct lmi_notify events;
} ua_defaults = {
	/* *INDENT-OFF* */
	{
		.sl_proto = { .pvar = 0,.popt = 0}, /* FIXME */
		.sp_proto = { .pvar = M2UA_VERSION_NONE,.popt = 0},
		.sg_proto = { .pvar = M2UA_VERSION_NONE,.popt = 0},
		.tack = 2000,
		.tbeat = 2000,
		.tidle = 2000,
		.testab = 2000,
		.ppi = M2UA_PPI,
		.istreams = 257,
		.ostreams = 257,
	},
	/* *INDENT-ON* */
};

static struct up *lm_ctrl = NULL;	/* master control stream */
static caddr_t ua_opens = NULL;		/* open list */
static caddr_t ua_links = NULL;		/* link list */

/* this lock protects lm_ctrl, ua_opens, ua_links, and lower mux q->q_ptr */
static rwlock_t ua_mux_lock = RW_LOCK_UNLOCKED;
static DECLARE_WAIT_QUEUE_HEAD(ua_waitq);

/* a request id for RKMM messages */
static atomic_t ua_request_id = ATOMIC_INIT(0);

#define UP_PRIV(q) ((struct up *)q->q_ptr)
#define TP_PRIV(q) ((struct tp *)q->q_ptr)
#define BC_PRIV(q) ((struct bc *)q->q_ptr)

/* Timer values */
enum {
	wack_aspup = 0,
	wack_aspdn,
	wack_aspac,
	wack_aspia,
	wack_hbeat,
};

/*
 *  State transitions.
 *  =========================================================================
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
		strlog(tp->mid, tp->sid, UALOGST, SL_TRACE, "%s <- %s", tp_i_state_name(newstate),
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
	return (tp_get_i_statef(tp) & mask);
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
		strlog(tp->mid, tp->sid, UALOGST, SL_TRACE, "%s <- %s", tp_u_state_name(newstate),
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
up_i_state_name(uint state)
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
up_set_i_state(struct up *up, uint newstate)
{
	uint oldstate = up->lm.state;

	if (oldstate != newstate)
		strlog(up->mid, up->sid, UALOGST, SL_TRACE, "%s <- %s", up_i_state_name(newstate),
		       up_i_state_name(oldstate));
	return ((up->lm.state = newstate));
}
static inline uint
up_get_i_state(struct up *up)
{
	return up->lm.state;
}
static inline uint
up_get_i_statef(struct up *up)
{
	return ((1 << up_get_i_state(up)));
}
static inline uint
up_chk_i_state(struct up *up, uint mask)
{
	return (up_get_i_statef(up) & mask);
}
static inline uint
up_not_i_state(struct up *up, uint mask)
{
	return (up_chk_i_state(up, ~mask));
}

const char *
up_u_state_name(uint state)
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
up_set_u_state(struct up *up, uint newstate)
{
	uint oldstate = up->as.state;

	if (newstate != oldstate)
		strlog(up->mid, up->sid, UALOGST, SL_TRACE, "%s <- %s", up_u_state_name(newstate),
		       up_u_state_name(oldstate));
	return (up->as.state = newstate);
}
static inline uint
up_get_u_state(struct up *up)
{
	return (up->as.state);
}
static inline uint
up_get_u_statef(struct up *up)
{
	return ((1 << up_get_u_state(up)));
}
static inline uint
up_chk_u_state(struct up *up, uint mask)
{
	return (up_get_u_statef(up) & mask);
}
static inline uint
up_not_u_state(struct up *up, uint mask)
{
	return (up_chk_u_state(up, ~mask));
}

const char *
up_l_state_name(uint state)
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
up_set_l_state(struct up *up, uint newstate)
{
	uint oldstate = up->l_state;

	if (newstate != oldstate)
		strlog(up->mid, up->sid, UALOGST, SL_TRACE, "%s <- %s", up_l_state_name(newstate),
		       up_l_state_name(oldstate));
	return (up->l_state = newstate);
}
static inline uint
up_get_l_state(struct up *up)
{
	return (up->l_state);
}
static inline uint
up_get_l_statef(struct up *up)
{
	return ((1 << up_get_l_state(up)));
}
static inline uint
up_chk_l_state(struct up *up, uint mask)
{
	return (up_get_l_statef(up) & mask);
}
static inline uint
up_not_l_state(struct up *up, uint mask)
{
	return (up_chk_l_state(up, ~mask));
}

#define LS_UNINIT	-1U
#define LS_IDLE		0
#define LS_FAILED	1
#define LS_STARTING	2
#define LS_IN_SERVICE	3
#define LS_LOC_PO	4
#define LS_REM_PO	5
#define LS_BOTH_PO	6
#define LS_COLLECTING	7

const char *
up_s_state_name(uint state)
{
	switch (state) {
	case (uint) - 1:
		return ("LS_UNINIT");
	default:
		return ("LS_???");
	}
}
static inline uint
up_set_s_state(struct up *up, uint newstate)
{
	uint oldstate = up->up.state;

	if (newstate != oldstate)
		strlog(up->mid, up->sid, UALOGST, SL_TRACE, "%s <- %s", up_s_state_name(newstate),
		       up_s_state_name(oldstate));
	return (up->up.state = newstate);
}
static inline uint
up_get_s_state(struct up *up)
{
	return (up->up.state);
}
static inline uint
up_get_s_statef(struct up *up)
{
	return ((1 << up_get_s_state(up)));
}
static inline uint
up_chk_s_state(struct up *up, uint mask)
{
	return (up_get_s_statef(up) & mask);
}
static inline uint
up_not_s_state(struct up *up, uint mask)
{
	return (up_chk_s_state(up, ~mask));
}

/*
 *  Structure Initialization and Termination.
 *  =========================================================================
 */

static void
up_free_priv(struct up *up)
{
	if (up->bc.rbid)
		unbufcall(xchg(&up->bc.rbid, 0));
	if (up->bc.wbid)
		unbufcall(xchg(&up->bc.wbid, 0));
	if (up->wack_aspac)
		mi_timer_free(XCHG(&up->wack_aspac, NULL));
	if (up->wack_aspia)
		mi_timer_free(XCHG(&up->wack_aspia, NULL));
	if (up->wack_hbeat)
		mi_timer_free(XCHG(&up->wack_hbeat, NULL));
	mi_close_free((caddr_t) up);
	return;
}
static struct up *
up_alloc_priv(queue_t *q, dev_t *devp, cred_t *crp)
{
	struct up *up = NULL;
	struct up_timer *t;

	if ((up = (struct up *) mi_open_alloc_sleep(sizeof(*up))) == NULL)
		return (NULL);
	bzero(up, sizeof(*up));
	if (!(up->wack_aspac = mi_timer_alloc(WR(q), sizeof(struct up_timer)))) {
		up_free_priv(up);
		return (NULL);
	}
	t = (typeof(t)) up->wack_aspac->b_rptr;
	t->timer = wack_aspac;
	t->as = NULL;
	if (!(up->wack_aspia = mi_timer_alloc(WR(q), sizeof(struct up_timer)))) {
		up_free_priv(up);
		return (NULL);
	}
	t = (typeof(t)) up->wack_aspia->b_rptr;
	t->timer = wack_aspia;
	t->as = NULL;
	if (!(up->wack_hbeat = mi_timer_alloc(WR(q), sizeof(struct up_timer)))) {
		up_free_priv(up);
		return (NULL);
	}
	t = (typeof(t)) up->wack_hbeat->b_rptr;
	t->timer = wack_hbeat;
	t->as = NULL;

	up->tp.next = NULL;
	up->tp.prev = &up->tp.next;
	up->tp.tp = NULL;

	up->rq = RD(q);
	up->wq = WR(q);

	up->dev = *devp;
	up->cred = *crp;
	up->unit = getminor(*devp);	/* this is the opened rather than assigned minor number */

	up->mid = getmajor(*devp);
	up->sid = getminor(*devp);

	up->l_state = DL_UNBOUND;

	up->as.id = (uint) (*devp);
	up->as.flags = 0;
	up->as.state = ASP_DOWN;

	up->as.streamid = 0;
	up->as.request_id = 0;

	up->as.config.ppa.sgid = getminor(*devp);
	up->as.config.ppa.sdti = 0;
	up->as.config.ppa.sdli = 0;
	up->as.config.ppa.iid = 0;
	up->as.config.ppa.iid_text[0] = '\0';
	up->as.config.ppa.tmode = 0;

	up->as.options.tack = ua_defaults.options.tack;
	up->as.options.testab = ua_defaults.options.testab;

	up->sl.flags = 0;
	up->sl.state = -1U;

	/* by default, assume it is a plain-jane ITU-T link */
	up->sl.option.pvar = SS7_PVAR_ITUT_00;
	up->sl.option.popt = 0;

	up->info.lmi_primitive = LMI_INFO_ACK;
	up->info.lmi_version = 1;	/* LMI in fact does not have a version. */
	up->info.lmi_state = LMI_UNUSABLE;
	up->info.lmi_max_sdu = 272;
	/* TS 102 141 says that this is retricted to the narrowband maximum.  That is way too
	   restrictive, but we will start with it anyway.  draft-bidulock-sigtran-sginfo provides a
	   mechanism for the SG to update this value and we recognize the SGINFO parameter. */
	up->info.lmi_min_sdu = 6;
	up->info.lmi_header_len = 48;
	/* lmi_header_len is a recommendation to the module above on how much write offset to apply
	   to data messages to permit adding protocol headers.  For M2UA, the protocol header length
	   for SUs depends on whether the a text or integer based IID is used, also whether DATA1 or
	   DATA2 (TTC) formats are used.  To begin with, assume that a maximum sized M2UA header is
	   necessary. In fact, we don't care.  We always append a new message block for the header. */
	up->info.lmi_ppa_style = LMI_STYLE2;

#if 0
	up->sl.config.t1 = FIXME;	/* Timer t1 duration (milliseconds) */
	up->sl.config.t2 = FIXME;	/* Timer t2 duration (milliseconds) */
	up->sl.config.t2l = FIXME;	/* Timer t2l duration (milliseconds) */
	up->sl.config.t2h = FIXME;	/* Timer t2h duration (milliseconds) */
	up->sl.config.t3 = FIXME;	/* Timer t3 duration (milliseconds) */
	up->sl.config.t4n = FIXME;	/* Timer t4n duration (milliseconds) */
	up->sl.config.t4e = FIXME;	/* Timer t4e duration (milliseconds) */
	up->sl.config.t5 = FIXME;	/* Timer t5 duration (milliseconds) */
	up->sl.config.t6 = FIXME;	/* Timer t6 duration (milliseconds) */
	up->sl.config.t7 = FIXME;	/* Timer t7 duration (milliseconds) */
	up->sl.config.rb_abate = FIXME;	/* RB cong abatement (#msgs) */
	up->sl.config.rb_accept = FIXME;	/* RB cong onset accept (#msgs) */
	up->sl.config.rb_discard = FIXME;	/* RB cong discard (#msgs) */
	up->sl.config.tb_abate_1 = FIXME;	/* lev 1 cong abate (#bytes) */
	up->sl.config.tb_onset_1 = FIXME;	/* lev 1 cong onset (#bytes) */
	up->sl.config.tb_discd_1 = FIXME;	/* lev 1 cong discard (#bytes) */
	up->sl.config.tb_abate_2 = FIXME;	/* lev 2 cong abate (#bytes) */
	up->sl.config.tb_onset_2 = FIXME;	/* lev 2 cong onset (#bytes) */
	up->sl.config.tb_discd_2 = FIXME;	/* lev 2 cong discard (#bytes) */
	up->sl.config.tb_abate_3 = FIXME;	/* lev 3 cong abate (#bytes) */
	up->sl.config.tb_onset_3 = FIXME;	/* lev 3 cong onset (#bytes) */
	up->sl.config.tb_discd_3 = FIXME;	/* lev 3 cong discard (#bytes) */
	up->sl.config.N1 = FIXME;	/* PCR/RTBmax messages (#msg) */
	up->sl.config.N2 = FIXME;	/* PCR/RTBmax octets (#bytes) */
	up->sl.config.M = FIXME;	/* IAC normal proving periods */

	up->sdt.config.t8 = FIXME;	/* T8 timeout (milliseconds) */
	up->sdt.config.Tin = FIXME;	/* AERM normal proving threshold */
	up->sdt.config.Tie = FIXME;	/* AERM emergency proving threshold */
	up->sdt.config.T = FIXME;	/* SUERM error threshold */
	up->sdt.config.D = FIXME;	/* SUERM error rate parameter */
	up->sdt.config.Te = FIXME;	/* EIM error threshold */
	up->sdt.config.De = FIXME;	/* EIM correct decrement */
	up->sdt.config.Ue = FIXME;	/* EIM error increment */
	up->sdt.config.N = FIXME;	/* octets per su in octet-counting mode */
	up->sdt.config.m = FIXME;	/* maximum SIF size */
	up->sdt.config.b = FIXME;	/* transmit block size */
	up->sdt.config.f = FIXME;	/* number of flags between frames */
#endif
	return (up);
}

static void
up_tp_link(struct up *up, struct tp *tp)
{
	if ((up->tp.next = tp->up.list))
		up->tp.next->tp.prev = &up->tp.next;
	up->tp.prev = &(up->tp.tp = tp)->up.list;
}

static void
up_tp_unlink(struct up *up)
{
	if (up->tp.tp) {
		if ((*up->tp.prev = up->tp.next))
			up->tp.next->tp.prev = up->tp.prev;
		up->tp.next = NULL;
		up->tp.prev = &up->tp.next;
		up->tp.tp = NULL;
	}
}

static void
tp_free_priv(struct tp *tp)
{
	if (tp->bc.rbid)
		unbufcall(xchg(&tp->bc.rbid, 0));
	if (tp->bc.wbid)
		unbufcall(xchg(&tp->bc.wbid, 0));
	if (tp->wack_aspup)
		mi_timer_free(XCHG(&tp->wack_aspup, NULL));
	if (tp->wack_aspdn)
		mi_timer_free(XCHG(&tp->wack_aspdn, NULL));
	if (tp->wack_aspac)
		mi_timer_free(XCHG(&tp->wack_aspac, NULL));
	if (tp->wack_aspia)
		mi_timer_free(XCHG(&tp->wack_aspia, NULL));
	if (tp->wack_hbeat)
		mi_timer_free(XCHG(&tp->wack_hbeat, NULL));
	mi_close_free((caddr_t) tp);
}
static struct tp *
tp_alloc_priv(queue_t *q, int index, cred_t *crp, minor_t unit)
{
	struct tp *tp = NULL;

	if ((tp = (struct tp *) mi_open_alloc(sizeof(*tp))) == NULL)
		return (NULL);
	bzero(tp, sizeof(*tp));
	if (!(tp->wack_aspup = mi_timer_alloc(RD(q), sizeof(int)))) {
		tp_free_priv(tp);
		return (NULL);
	}
	*(int *) tp->wack_aspup->b_rptr = wack_aspup;
	if (!(tp->wack_aspdn = mi_timer_alloc(RD(q), sizeof(int)))) {
		tp_free_priv(tp);
		return (NULL);
	}
	*(int *) tp->wack_aspdn->b_rptr = wack_aspdn;
	if (!(tp->wack_aspac = mi_timer_alloc(RD(q), sizeof(int)))) {
		tp_free_priv(tp);
		return (NULL);
	}
	*(int *) tp->wack_aspac->b_rptr = wack_aspac;
	if (!(tp->wack_aspia = mi_timer_alloc(RD(q), sizeof(int)))) {
		tp_free_priv(tp);
		return (NULL);
	}
	*(int *) tp->wack_aspia->b_rptr = wack_aspia;
	if (!(tp->wack_hbeat = mi_timer_alloc(RD(q), sizeof(int)))) {
		tp_free_priv(tp);
		return (NULL);
	}
	*(int *) tp->wack_hbeat->b_rptr = wack_hbeat;

	spin_lock_init(&tp->lock);
	tp->users = 0;
	tp->waitq = NULL;

	tp->rq = q ? RD(q) : NULL;
	tp->wq = q ? WR(q) : NULL;
	tp->cred = *crp;

	tp->mid = q->q_qinfo->qi_minfo->mi_idnum;
	tp->sid = unit;

	tp_set_u_state(tp, ASP_DOWN);
	tp_set_i_state(tp, -1);	/* until info ack comes */

	tp->flags = 0;

	/* ASP configuration options */
	tp->sp.options.options.popt = ua_defaults.options.sp_proto.popt;
	tp->sp.options.options.pvar = ua_defaults.options.sp_proto.pvar;
	tp->sp.options.aspid = 0;
	tp->sp.options.tack = ua_defaults.options.tack;
	tp->sp.options.tbeat = ua_defaults.options.tbeat;
	tp->sp.options.tidle = ua_defaults.options.tidle;

	/* ASP configuration (none) */

	tp->sg.id = unit;
	tp->sg.flags = 0;
	tp->sg.state = ASP_DOWN;

	/* SG configuration options */
	tp->sg.options.options.popt = ua_defaults.options.sg_proto.popt;
	tp->sg.options.options.pvar = ua_defaults.options.sg_proto.pvar;

	/* SG configuration (none) */

	tp->xp.id = index;
	tp->xp.flags = 0;
	tp->xp.state = -1U;

	/* XP configuation options */
	tp->xp.options.ppi = ua_defaults.options.ppi;
	tp->xp.options.istreams = ua_defaults.options.istreams;
	tp->xp.options.ostreams = ua_defaults.options.ostreams;
	tp->xp.options.loc_len = 0;
	tp->xp.options.rem_len = 0;
	tp->xp.options.loc_add.sa_family = AF_UNSPEC;
	tp->xp.options.rem_add.sa_family = AF_UNSPEC;

	/* XP configuration */
	tp->xp.config.sgid = unit;

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
	tp->info.PROVIDER_flag = T_XPG4_1;

	return (tp);
}

/*
 *  Lookups
 *  =========================================================================
 */
static struct df *
df_find(struct up *up, uint id)
{
	if (id != 0)
		return (NULL);
	return (&ua_defaults);
}
static struct up *
lm_find(struct up *up, uint id)
{
	if (id != 0)
		for (up = (struct up *) mi_first_ptr(&ua_opens); up && up->lm.id != id;
		     up = (struct up *) mi_next_ptr((caddr_t) up)) ;
	return (up);
}
static struct up *
up_find(struct up *up, uint id)
{
	if (id != 0)
		for (up = (struct up *) mi_first_ptr(&ua_opens); up && up->up.id != id;
		     up = (struct up *) mi_next_ptr((caddr_t) up)) ;
	return (up);
}
static struct up *
as_find(struct up *up, uint id)
{
	if (id != 0)
		for (up = (struct up *) mi_first_ptr(&ua_opens); up && up->as.id != id;
		     up = (struct up *) mi_next_ptr((caddr_t) up)) ;
	return (up);
}
static struct tp *
sp_find(struct up *up, uint id)
{
	struct tp *tp = up->tp.tp;

	if (id != 0)
		for (tp = (struct tp *) mi_first_ptr(&ua_links); tp && tp->sp.id != id;
		     tp = (struct tp *) mi_next_ptr((caddr_t) tp)) ;
	return (tp);
}
static struct tp *
sg_find(struct up *up, uint id)
{
	struct tp *tp = up->tp.tp;

	if (id != 0)
		for (tp = (struct tp *) mi_first_ptr(&ua_links); tp && tp->sg.id != id;
		     tp = (struct tp *) mi_next_ptr((caddr_t) tp)) ;
	return (tp);
}
static struct tp *
gp_find(struct up *up, uint id)
{
	struct tp *tp = up->tp.tp;

	if (id != 0)
		for (tp = (struct tp *) mi_first_ptr(&ua_links); tp && tp->gp.id != id;
		     tp = (struct tp *) mi_next_ptr((caddr_t) tp)) ;
	return (tp);
}
static struct tp *
xp_find(struct up *up, uint id)
{
	struct tp *tp = up->tp.tp;

	if (id != 0)
		for (tp = (struct tp *) mi_first_ptr(&ua_links); tp && tp->xp.id != id;
		     tp = (struct tp *) mi_next_ptr((caddr_t) tp)) ;
	return (tp);
}

/*
 *  Locking
 *  =========================================================================
 */

static inline bool
tp_trylock(struct tp *tp, queue_t *q)
{
	unsigned long flags;
	bool rtn = false;

	spin_lock_irqsave(&tp->lock, flags);
	if (tp->users == 0) {
		tp->users = 1;
		rtn = true;
	} else if (tp->waitq == NULL) {
		tp->waitq = q;
	} else {
		qenable(q);
	}
	spin_unlock_irqrestore(&tp->lock, flags);
	return (rtn);
}

static inline void
tp_unlock(struct tp *tp)
{
	unsigned long flags;

	spin_lock_irqsave(&tp->lock, flags);
	tp->users = 0;
	if (tp->waitq)
		qenable(XCHG(&tp->waitq, NULL));
	if (waitqueue_active(&ua_waitq))
		wake_up_all(&ua_waitq);
	spin_unlock_irqrestore(&tp->lock, flags);
	return;
}

static inline struct tp *
tp_acquire(queue_t *q)
{
	struct tp *tp;

	read_lock(&ua_mux_lock);
	if ((tp = TP_PRIV(q)) == NULL) {
		/* Lower multiplex private structures can disappear between I_UNLINK/I_PUNLINK
		   M_IOCTL and setq(9) back to the Stream head. */
		read_unlock(&ua_mux_lock);
		return (NULL);
	}
	if (!tp_trylock(tp, q)) {
		read_unlock(&ua_mux_lock);
		return (NULL);
	}
	read_unlock(&ua_mux_lock);
	return (tp);
}
static inline void
tp_release(struct tp *tp)
{
	tp_unlock(tp);
}

static inline struct tp *
up_tp_acquire(struct up *up, queue_t *q)
{
	struct tp *tp;

	read_lock(&ua_mux_lock);
	if ((tp = up->tp.tp) == NULL) {
		read_unlock(&ua_mux_lock);
		return (NULL);
	}
	if (!tp_trylock(tp, q)) {
		read_unlock(&ua_mux_lock);
		return (NULL);
	}
	read_unlock(&ua_mux_lock);
	return (tp);
}
static inline void
up_tp_release(struct tp *tp)
{
	tp_unlock(tp);
}

static inline bool
up_acquire(struct up *up, queue_t *q)
{
	struct tp *tp;

	read_lock(&ua_mux_lock);
	if ((tp = up->tp.tp) == NULL) {
		read_unlock(&ua_mux_lock);
		return (false);
	}
	if (!tp_trylock(tp, q)) {
		read_unlock(&ua_mux_lock);
		return (false);
	}
	read_unlock(&ua_mux_lock);
	return (true);
}
static inline void
up_release(struct up *up)
{
	struct tp *tp;

	if ((tp = up->tp.tp))
		tp_unlock(tp);
}

/*
 *  Buffer allocation
 *  =========================================================================
 */

static mblk_t *
ua_allocb(queue_t *q, size_t len, int priority)
{
	mblk_t *mp;

	if (unlikely((mp = allocb(len, priority)) == NULL)) {
		struct bc *bc = BC_PRIV(q);
		bcid_t bid, *bidp = (q->q_flag & QREADR) ? &bc->rbid : &bc->wbid;

		if ((bid = bufcall(len, priority, (void (*)(long)) &qenable, (long) q)))
			unbufcall(xchg(bidp, bid));
		else
			qenable(q);
	}
	return (mp);
}

noinline fastcall int
m_hangup(struct up *up, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;

	if ((mp = ua_allocb(q, 0, BPRI_MED))) {
		DB_TYPE(mp) = M_HANGUP;
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- M_HANGUP");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * UA to LM primitives.
 * ==========================================================================
 */

/**
 * ua_t_establish_con: - issue an UA_T_ESTABLISH_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sgid: SG identifier
 * @rptr: responding address pointer
 * @rlen: responding address length
 * @optr: options pointer
 * @olen: options length
 *
 * An establish confirmation is sent in response to a successful establish request.
 */
static inline fastcall int
ua_t_establish_con(struct lm *lm, queue_t *q, mblk_t *msg, uint sgid, caddr_t rptr, size_t rlen,
		   caddr_t optr, size_t olen)
{
	struct UA_t_establish_con *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + rlen + olen, BPRI_MED))) {
		if (canputnext(lm->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->SG_id = sgid;
			p->PRIM_type = UA_T_ESTABLISH_CON;
			p->RES_length = rlen;
			p->RES_offset = sizeof(*p);
			p->OPT_length = olen;
			p->OPT_offset = sizeof(*p) + rlen;
			mp->b_wptr += sizeof(*p);
			bcopy(rptr, mp->b_wptr, rlen);
			mp->b_wptr += rlen;
			bcopy(optr, mp->b_wptr, olen);
			mp->b_wptr += olen;
			freemsg(msg);
			putnext(lm->rq, mp);
			return (0);
		}
		freemsg(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ua_t_establish_ind: - issue an UA_T_ESTABLISH_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_t_establish_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_t_establish_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_t_release_con: - issue an UA_T_RELEASE_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_t_release_con(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_t_release_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_t_release_ind: - issue an UA_T_RELEASE_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_t_release_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_t_release_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_t_restart_ind: - issue an UA_T_RESTART_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_t_restart_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_t_restart_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_t_status_ind: - issue an UA_T_STATUS_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_t_status_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_t_status_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_t_status_con: - issue an UA_T_STATUS_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_t_status_con(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_t_status_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_status_ind: - issue an UA_ASP_STATUS_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_status_ind(struct lm *lm, struct sg *sg, queue_t *q, mblk_t *msg)
{
	struct UA_asp_status_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_status_con: - issue an UA_ASP_STATUS_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_status_con(struct lm *lm, struct sg *sg, queue_t *q, mblk_t *msg)
{
	struct UA_asp_status_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_as_status_ind: - issue an UA_AS_STATUS_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_as_status_ind(struct lm *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_as_status_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_as_status_con: - issue an UA_AS_STATUS_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_as_status_con(struct lm *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_as_status_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_notify_ind: - issue an UA_NOTIFY_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_notify_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_notify_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_error_ind: - issue an UA_ERROR_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_error_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_error_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_up_ind: - issue an UA_ASP_UP_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_up_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_asp_up_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_up_con: - issue an UA_ASP_UP_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_up_con(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_asp_up_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_down_ind: - issue an UA_ASP_DOWN_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_down_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_asp_down_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_down_con: - issue an UA_ASP_DOWN_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_down_con(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_asp_down_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_active_ind: - issue an UA_ASP_ACTIVE_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_active_ind(struct lm *lm, struct as *as, queue_t *q, mblk_t *msg)
{
	struct UA_asp_active_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_active_con: - issue an UA_ASP_ACTIVE_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_active_con(struct lm *lm, struct as *as, queue_t *q, mblk_t *msg)
{
	struct UA_asp_active_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_inactive_ind: - issue an UA_ASP_INACTIVE_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_inactive_ind(struct lm *lm, struct as *as, queue_t *q, mblk_t *msg)
{
	struct UA_asp_inactive_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asp_inactive_con: - issue an UA_ASP_INACTIVE_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asp_inactive_con(struct lm *lm, struct as *as, queue_t *q, mblk_t *msg)
{
	struct UA_asp_inactive_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_as_active_ind: - issue an UA_AS_ACTIVE_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_as_active_ind(struct lm *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_as_active_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_as_inactive_ind: - issue an UA_AS_INACTIVE_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_as_inactive_ind(struct lm *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_as_inactive_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_as_down_ind: - issue an UA_AS_DOWN_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_as_down_ind(struct lm *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_as_down_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_reg_con: - issue an UA_REG_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_reg_con(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_reg_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_reg_ind: - issue an UA_REG_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_reg_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_reg_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_dereg_con: - issue an UA_DEREG_CON primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_dereg_con(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_dereg_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_dereg_ind: - issue an UA_DEREG_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_dereg_ind(struct lm *lm, struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct UA_dereg_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_error_occ_ind: - issue an UA_ERROR_OCC_IND primitive
 * @lm: LM private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_error_occ_ind(struct lm *lm, queue_t *q, mblk_t *msg)
{
	struct UA_error_occ_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/*
 * SL-Provider to SL-User primitives.
 * ==================================
 */

/**
 * lmi_info_ack - issued an LMI_INFO_ACK primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_info_ack(struct up *up, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + sizeof(uint32_t), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = up->info.lmi_version;
		p->lmi_state = up_get_i_state(up);
		p->lmi_max_sdu = up->info.lmi_max_sdu;
		p->lmi_min_sdu = up->info.lmi_min_sdu;
		p->lmi_header_len = up->info.lmi_header_len;
		p->lmi_ppa_style = up->info.lmi_ppa_style;
		mp->b_wptr += sizeof(*p);
		switch (up_get_i_state(up)) {
		case LMI_UNATTACHED:
		case LMI_ATTACH_PENDING:
		case LMI_UNUSABLE:
		default:
			break;
		case LMI_DISABLED:
		case LMI_ENABLE_PENDING:
		case LMI_ENABLED:
		case LMI_DISABLE_PENDING:
		case LMI_DETACH_PENDING:
			bcopy(&up->as.config.ppa, mp->b_wptr, sizeof(up->as.config.ppa));
			mp->b_wptr += sizeof(up->as.config.ppa);
			break;
		}
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- LMI_INFO_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_ok_ack - issued an LMI_OK_ACK primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primivite
 */
static noinline fastcall int
lmi_ok_ack(struct up *up, queue_t *q, mblk_t *msg, lmi_long prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (up_get_i_state(up)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_DISABLED;
			up_set_i_state(up, LMI_DISABLED);
			up_set_u_state(up, ASP_INACTIVE);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			up_set_i_state(up, LMI_UNATTACHED);
			up_set_u_state(up, ASP_DOWN);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- LMI_OK_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ack - issued an LMI_ERROR_ACK primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @err: positive LMI error, negative UNIX error
 */
static noinline fastcall int
lmi_error_ack(struct up *up, queue_t *q, mblk_t *msg, lmi_long prim, lmi_long err)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if (err == 0)
		return lmi_ok_ack(up, q, msg, prim);
	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_error_primitive = prim;
		switch (up_get_i_state(up)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			up_set_i_state(up, LMI_UNATTACHED);
			up_set_u_state(up, ASP_DOWN);
			break;
		case LMI_DETACH_PENDING:
		case LMI_ENABLE_PENDING:
			p->lmi_state = LMI_DISABLED;
			up_set_i_state(up, LMI_DISABLED);
			up_set_u_state(up, ASP_INACTIVE);
			break;
		case LMI_DISABLE_PENDING:
			p->lmi_state = LMI_ENABLED;
			up_set_i_state(up, LMI_ENABLED);
			up_set_u_state(up, ASP_ACTIVE);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- LMI_ERROR_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_enable_con - issued an LMI_ENABLE_CON primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_enable_con(struct up *up, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = LMI_ENABLED;
		mp->b_wptr += sizeof(*p);
		up_set_i_state(up, LMI_ENABLED);
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- LMI_ENABLE_CON");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_con - issued an LMI_DISABLE_CON primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_disable_con(struct up *up, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = LMI_DISABLED;
		mp->b_wptr += sizeof(*p);
		up_set_i_state(up, LMI_DISABLED);
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- LMI_DISABLE_CON");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * lmi_optmgmt_ack - issued an LMI_OPTMGMT_ACK primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_optmgmt_ack(struct up *up, queue_t *q, mblk_t *msg)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- LMI_OPTMGMT_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/**
 * lmi_error_ind - issued an LMI_ERROR_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: postivie LMI or negative UNIX error
 * @state: resulting state
 */
static noinline fastcall int
lmi_error_ind(struct up *up, queue_t *q, mblk_t *msg, lmi_long err, lmi_long state)
{
	lmi_error_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = err < 0 ? -err : 0;
		p->lmi_reason = err < 0 ? LMI_SYSERR : err;
		p->lmi_state = up_set_i_state(up, state);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- LMI_ERROR_IND");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * lmi_stats_ind - issued an LMI_STATS_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_stats_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_STATS_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- LMI_STATS_IND");
			putnext(up->rq, mp);
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
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
lmi_event_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_EVENT_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- LMI_EVENT_IND");
			putnext(up->rq, mp);
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
 * @up: UP private structure
 * @q: active queue
 * @msg: user data to indicate
 * @pri: message priority
 */
static noinline fastcall int
sl_pdu_ind(struct up *up, queue_t *q, mblk_t *msg, sl_ulong pri)
{
	sl_pdu_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_PDU_IND;
			p->sl_mp = pri;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = msg;
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_PDU_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congested_ind - issued an SL_LINK_CONGESTED_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cong: congestion status
 * @disc: discard status
 */
static noinline fastcall int
sl_link_congested_ind(struct up *up, queue_t *q, mblk_t *msg, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			up->cong = cong;
			up->disc = disc;
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_LINK_CONGESTED_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congestion_ceased_ind - issued an SL_LINK_CONGESTED_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cong: congestion status
 * @disc: discard status
 */
static noinline fastcall int
sl_link_congestion_ceased_ind(struct up *up, queue_t *q, mblk_t *msg, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ceased_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			up->cong = cong;
			up->disc = disc;
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE,
			       "<- SL_LINK_CONGESTION_CEASED_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieved_message_ind - issued an SL_RETRIEVED_MESSAGE_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: retrieved user data
 * @pri: message priority
 */
static noinline fastcall int
sl_retrieved_message_ind(struct up *up, queue_t *q, mblk_t *msg, sl_ulong pri)
{
	sl_retrieved_msg_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
			p->sl_mp = pri;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = msg;
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_RETRIEVED_MESSAGE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_complete_ind - issued an SL_RETRIEVAL_COMPLETE_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @pri: message priority
 * @dp: retrieved user data
 */
static noinline fastcall int
sl_retrieval_complete_ind(struct up *up, queue_t *q, mblk_t *msg, sl_ulong pri, mblk_t *dp)
{
	sl_retrieval_comp_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
			p->sl_mp = pri;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_RETRIEVAL_COMPLETE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_rb_cleared_ind - issued an SL_RB_CLEARED_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_rb_cleared_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_rb_cleared_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RB_CLEARED_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			up->status.pending &= ~(1 << M2UA_STATUS_FLUSH_BUFFERS);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_RB_CLEARED_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_bsnt_ind - issued an SL_BSNT_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @bsnt: value of BSNT to indicate
 */
static noinline fastcall int
sl_bsnt_ind(struct up *up, queue_t *q, mblk_t *msg, sl_ulong bsnt)
{
	sl_bsnt_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_BSNT_IND;
			p->sl_bsnt = bsnt;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_BSNT_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_in_service_ind - issued an SL_IN_SERVICE_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_in_service_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_in_service_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_IN_SERVICE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_IN_SERVICE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_out_of_service_ind - issued an SL_OUT_OF_SERVICE_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: reason for failure
 */
static noinline fastcall int
sl_out_of_service_ind(struct up *up, queue_t *q, mblk_t *msg, sl_ulong reason)
{
	sl_out_of_service_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_OUT_OF_SERVICE_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_OUT_OF_SERVICE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_outage_ind - issued an SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_remote_processor_outage_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_out_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE,
			       "<- SL_REMOTE_PROCESSOR_OUTAGE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_recovered_ind - issued an SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_remote_processor_recovered_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_recovered_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE,
			       "<- SL_REMOTE_PROCESSOR_RECOVERED_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_rtb_cleared_ind - issued an SL_RTB_CLEARED_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_rtb_cleared_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_rtb_cleared_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RTB_CLEARED_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			up->status.pending &= ~(1 << M2UA_STATUS_CLEAR_RTB);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_RTB_CLEARED_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * sl_retrieval_not_possible_ind - issued an SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_retrieval_not_possible_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_retrieval_not_poss_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE,
			       "<- SL_RETRIEVAL_NOT_POSSIBLE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

/**
 * sl_bsnt_not_retrievable_ind - issued an SL_BSNT_NOT_RETRIEVABLE_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_bsnt_not_retrievable_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_bsnt_not_retr_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE,
			       "<- SL_BSNT_NOT_RETRIEVABLE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * sl_optmgmt_ack - issued an SL_OPTMGMT_ACK primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_optmgmt_ack(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_optmgmt_ack_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OPTMGMT_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_OPTMGMT_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * sl_notify_ind - issued an SL_NOTIFY_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_notify_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_notify_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_NOTIFY_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- SL_NOTIFY_IND");
			putnext(up->rq, mp);
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
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_local_processor_outage_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_out_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE,
			       "<- SL_LOCAL_PROCESSOR_OUTAGE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_local_processor_recovered_ind - issued an SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
sl_local_processor_recovered_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_recovered_ind_t *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LOCAL_PROCESSOR_RECOVERED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE,
			       "<- SL_LOCAL_PROCESSOR_RECOVERED_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * TS-User to TS-Provider primitives.
 * ==========================================================================
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

	if (likely(!!(mp = ua_allocb(q, sizeof(*p) + dlen + olen, BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 2))) {
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
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_CONN_REQ ->");
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

	if (likely(!!(mp = ua_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
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
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_CONN_RES ->");
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
static inline fastcall int
t_discon_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t sequence, mblk_t *dp)
{
	struct T_discon_req *p;
	mblk_t *mp;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p), BPRI_MED)))) {
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
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_DISCON_REQ ->");
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
static inline fastcall int
t_data_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_data_req *p;
	mblk_t *mp;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 0))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DATA_REQ;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_DATA_REQ ->");
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
static inline fastcall int
t_exdata_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_exdata_req *p;
	mblk_t *mp;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 1))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_EXDATA_REQ;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_EXDATA_REQ ->");
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
static inline fastcall int
t_info_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_info_req *p;
	mblk_t *mp;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_INFO_REQ ->");
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

	if (likely(!!(mp = ua_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_BIND_REQ;
			p->ADDR_length = alen;
			p->ADDR_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, alen);
			mp->b_wptr += alen;
			if (tp_get_i_state(tp) == TS_IDLE)
				tp_set_i_state(tp, TS_WACK_BREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_BIND_REQ ->");
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
static inline fastcall int
t_unbind_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
	struct T_unbind_req *p;
	mblk_t *mp;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			tp_set_i_state(tp, TS_WACK_UREQ);
			freemsg(msg);
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_UNBIND_REQ ->");
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
static inline fastcall int
t_unitdata_req(struct tp *tp, queue_t *q, mblk_t *msg, size_t dlen, caddr_t dptr, size_t olen,
	       caddr_t optr, mblk_t *dp)
{
	struct T_unitdata_req *p;
	mblk_t *mp;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p) + dlen + olen, BPRI_MED)))) {
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
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_UNITDATA_REQ ->");
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
static inline fastcall int
t_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *msg, size_t olen, caddr_t optr, t_scalar_t flags)
{
	struct T_optmgmt_req *p;
	mblk_t *mp;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
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
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_OPTMGMT_REQ ->");
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
t_ordrel_req(struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct T_ordrel_req *p;
	mblk_t *mp;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ORDREL_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			switch (tp_get_i_state(tp)) {
			case TS_DATA_XFER:
				tp_set_i_state(tp, TS_WIND_ORDREL);
				break;
			case TS_WREQ_ORDREL:
				tp_set_i_state(tp, TS_IDLE);
				break;
			}
			freemsg(msg);
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_ORDREL_REQ ->");
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

	if (likely
	    (!!(mp = ua_allocb(q, sizeof(*p) + 2 * (sizeof(*oh) + sizeof(t_uscalar_t)), BPRI_MED))))
	{
		if (likely(bcanputnext(tp->wq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = dp->b_band;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flag;
			p->OPT_length = 2 * (sizeof(*oh) + sizeof(t_uscalar_t));
			p->OPT_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			oh = (typeof(oh)) mp->b_wptr;
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_PPI;
			oh->len = sizeof(t_uscalar_t);
			*(t_uscalar_t *) mp->b_wptr = tp->xp.options.ppi;
			mp->b_wptr += sizeof(t_uscalar_t);
			oh = (typeof(oh)) mp->b_wptr;
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_SID;
			oh->len = sizeof(t_uscalar_t);
			mp->b_wptr += sizeof(*oh);
			*(t_uscalar_t *) mp->b_wptr = sid;
			mp->b_wptr += sizeof(t_uscalar_t);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_OPTDATA_REQ ->");
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

	if (likely(!!(mp = ua_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_ADDR_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_ADDR_REQ ->");
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
static inline fastcall int
t_capability_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t bits)
{
	struct T_capability_req *p;
	mblk_t *mp;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_REQ;
		p->CAP_bits1 = bits;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "T_CAPABILITY_REQ ->");
		putnext(tp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  ASP to SGP UA protocol messages
 *  =========================================================================
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

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
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

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "MGMT ERR ->");
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
	uint popt = tp->sp.options.options.popt;
	size_t olen = 0;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + ilen ? UA_PHDR_SIZE +
	    UA_PAD4(ilen) : 0;

	if ((popt & UA_SG_ASPEXT)) {
		uint opt, none = true;

		mlen += UA_PHDR_SIZE;
		for (opt = UA_ASPEXT_NONE; opt <= UA_ASPEXT_TEXTIID; opt++) {
			if (popt & (1 << opt)) {
				none = false;
				olen += sizeof(uint32_t);
			}
		}
		if (none)
			olen += sizeof(uint32_t);
		mlen += olen;
	}

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPUP_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if ((popt & UA_SG_ASPEXT)) {
			uint opt, none = true;

			/* when the SG supports ASP Extensions per draft-bidulock-sigtran-aspext,
			   the supported extensions are added to the ASPUP Request in the ASPEXT
			   parameter. */
			*p++ = UA_PHDR(UA_PARM_ASPEXT, olen);
			for (opt = UA_ASPEXT_NONE; opt <= UA_ASPEXT_TEXTIID; opt++) {
				if (popt & (1 << opt)) {
					none = false;
					*p++ = opt;
				}
			}
			if (none)
				*p++ = 0;
		}

		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(ilen);
		tp_set_u_state(tp, ASP_WACK_ASPUP);
		if (tp->sp.options.tack) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
			       "-> WACK ASPUP START <- (%u msec)", tp->sp.options.tack);
			mi_timer(tp->wack_aspup, tp->sp.options.tack);
		}

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPS ASPUP Req ->");
		/* send unordered and expedited on stream 0 */
		if (unlikely((err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_asps_aspdn_req: - send ASP Down
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

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
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
		if (tp->sp.options.tack) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
			       "-> WACK ASPDN START <- (%u msec)", tp->sp.options.tack);
			if (tp->wack_aspdn)
				mi_timer(tp->wack_aspdn, tp->sp.options.tack);
		}

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPS ASPDN Req ->");
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
fastcall int
tp_send_asps_hbeat_req(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hlen);
		if (tp->sp.options.tbeat) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
			       "-> WACK BEAT START <- (%u msec)", tp->sp.options.tbeat);
			if (tp->wack_hbeat)
				mi_timer(tp->wack_hbeat, tp->sp.options.tbeat);
		}
		/* send ordered on management stream 0 */

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_asps_hbeat_req: - send a BEAT message
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat info pointer
 * @hlen: heartbeat info length
 *
 * This one is used for AS-SG heartbeat associated with a specific signalling link and, therefore,
 * sent on the same SCTP stream identifier as is data for the signalling link.
 */
fastcall int
as_send_asps_hbeat_req(struct up *up, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0) +
	    (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(hlen));
		}
		mp->b_wptr = (unsigned char *) p;
		if (up->as.options.tbeat) {
			strlog(up->mid, up->sid, UALOGTE, SL_TRACE,
			       "-> WACK BEAT START <- (%u msec)", up->as.options.tbeat);
			if (up->wack_hbeat)
				mi_timer(up->wack_hbeat, up->as.options.tbeat);
		}
		/* send ordered on specified stream */

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_asps_hbeat_ack: - send a BEAT Ack message
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat data pointer
 * @hlen: heartbeat data length
 *
 * This one is used for ASP-SG heartbeat not associatedw with a specific signalling link and,
 * therefore, sent on SCTP stream identifier zero (0).
 */
static fastcall int
tp_send_asps_hbeat_ack(struct tp *tp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hlen);
		/* send ordered on management stream 0 */

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_asps_hbeat_ack: - send a BEAT Ack message
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat data pointer
 * @hlen: heartbeat data length
 *
 * This one is used for AS-SG hearbeat associated with a specific signalling link and, therefore,
 * sent on the same SCTP stream identifier as is used for data (MAUP) messages.
 */
static fastcall int
as_send_asps_hbeat_ack(struct up *up, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hlen);
		/* send ordered on specified stream */

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_aspt_aspac_req: - send an ASP Active Request
 * @up: UP private structure
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
as_send_aspt_aspac_req(struct up *up, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.tmode ? UA_SIZE(UA_PARM_TMODE) : 0) +
	    (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPAC_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.config.ppa.tmode) {
			p[0] = UA_PARM_TMODE;
			p[1] = htonl(up->as.config.ppa.tmode);
			p += 2;
		}
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;
		up_set_u_state(up, ASP_WACK_ASPAC);
		if (up->as.options.tack) {
			strlog(up->mid, up->sid, UALOGTE, SL_TRACE,
			       "-> WACK ASPAC START <- (%u msec)", up->as.options.tack);
			if (up->wack_aspac)
				mi_timer(up->wack_aspac, up->as.options.tack);
		}

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "ASPT ASPAC Req ->");
		/* always sent on same stream as data */
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * tp_send_aspt_apsia_req: - send an ASP Inactive Request
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @iid: pointer to IID or NULL
 * @tptr: pointer to text IID or NULL
 * @tlen: length of text IID or zero
 * @iptr: info pointer
 * @ilen: info length
 *
 * This version is disassociated with a given signalling link (AS) and is used to request
 * deactivation of signalling links (AS) for which messages have been received but for which there
 * is no corresponding upper stream.   This might happen when the upper stream closed non-gracefully
 * and a message buffer could not be allocated during close to deactivate the AS.  Also, it might
 * happen if the SG is foolish enough to send data (MAUP) messages to an inactive (non-existent) AS.
 */
static int
tp_send_aspt_aspia_req(struct tp *tp, queue_t *q, mblk_t *msg, uint32_t *iid, caddr_t tptr,
		       size_t tlen, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + (iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) + (tptr ? UA_PHDR_SIZE +
									  UA_PAD4(tlen) : 0) +
	    (iptr ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(*iid);
			p += 2;
		}
		if (tptr) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(tptr, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPT ASPIA Req ->");
		/* always sent on stream 0 when no AS exists */
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_aspt_aspia_req: - send an ASP Inactive Request
 * @up: UP private structure
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
as_send_aspt_aspia_req(struct up *up, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;
		up_set_u_state(up, ASP_WACK_ASPIA);
		if (up->as.options.tack) {
			strlog(up->mid, up->sid, UALOGTE, SL_TRACE,
			       "-> WACK ASPIA START <- (%u msec)", up->as.options.tack);
			if (up->wack_aspia)
				mi_timer(up->wack_aspia, up->as.options.tack);
		}

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "ASPT ASPIA Req ->");
		/* always sent on same stream as data */
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_rkmm_reg_req: - send REG REQ
 * @up: UP private structure
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
as_send_rkmm_reg_req(struct up *up, queue_t *q, mblk_t *msg, uint32_t id,
		     uint16_t sdti, uint16_t sdli)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_LINK_KEY);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_REG_REQ;
		p[1] = htonl(mlen);
		p[2] = M2UA_PARM_LINK_KEY;
		p[3] = M2UA_PARM_LOC_KEY_ID;
		p[4] = htonl(id);
		p[5] = M2UA_PARM_SDTI;
		p[6] = htonl(sdti);
		p[7] = M2UA_PARM_SDLI;
		p[8] = htonl(sdli);
		p += 9;
		mp->b_wptr = (unsigned char *) p;

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "RKMM REG Req ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_rkmm_dereg_req: - send DEREG REQ
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
as_send_rkmm_dereg_req(struct up *up, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_DEREG_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "RKMM DEREG Req ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, T_ODF_EX, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_maup_data1: - send DATA1
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static inline fastcall __hot_write int
as_send_maup_data1(struct up *up, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) + UA_PHDR_SIZE;

	if (unlikely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		*p++ = UA_PHDR(M2UA_PARM_DATA1, dlen);
		mp->b_wptr = (unsigned char *) p;

		mp->b_cont = dp->b_cont;
		strlog(up->mid, up->sid, UALOGDA, SL_TRACE, "MAUP DATA1 ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_maup_data2: - send DATA2
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 * @pri: message priority (placed in first byte of message)
 */
static inline fastcall __hot_write int
as_send_maup_data2(struct up *up, queue_t *q, mblk_t *msg, mblk_t *dp, uint8_t pri)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) + UA_PHDR_SIZE + 1;

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		*p++ = UA_PHDR(M2UA_PARM_DATA2, dlen + 1);
		*(unsigned char *) p = pri;
		mp->b_wptr = (unsigned char *) p + 1;

		mp->b_cont = dp->b_cont;
		strlog(up->mid, up->sid, UALOGDA, SL_TRACE, "MAUP DATA2 ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_maup_estab_req: - send MAUP Establish Request
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
as_send_maup_estab_req(struct up *up, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_ESTAB_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		mp->b_wptr = (unsigned char *) p;

		up_set_l_state(up, DL_OUTCON_PENDING);

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "MAUP ESTAB Req ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_maup_rel_req: - send MAUP Release Request
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
as_send_maup_rel_req(struct up *up, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		mp->b_wptr = (unsigned char *) p;

		switch (up_get_l_state(up)) {
		case DL_OUTCON_PENDING:
			up_set_l_state(up, DL_DISCON8_PENDING);
			break;
		case DL_INCON_PENDING:
			up_set_l_state(up, DL_DISCON9_PENDING);
			break;
		case DL_DATAXFER:
			up_set_l_state(up, DL_DISCON11_PENDING);
			break;
		case DL_USER_RESET_PENDING:
			up_set_l_state(up, DL_DISCON12_PENDING);
			break;
		case DL_PROV_RESET_PENDING:
			up_set_l_state(up, DL_DISCON13_PENDING);
			break;
		default:
			/* software error */
			up_set_l_state(up, -1);
			break;
		}

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "MAUP REL Req ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_maup_state_req: - send MAUP State Request
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @request: state request
 */
static int
as_send_maup_state_req(struct up *up, queue_t *q, mblk_t *msg, const uint32_t request)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
	    UA_SIZE(M2UA_PARM_STATE_REQUEST);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_STATE_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		p[0] = M2UA_PARM_STATE_REQUEST;
		p[1] = htonl(request);
		p += 2;
		mp->b_wptr = (unsigned char *) p;

		up->status.pending |= (1 << request);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "MAUP STATE Req ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp)))) {
			freeb(mp);
			up->status.pending &= ~(1 << request);
		}
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_maup_retr_req: - send MAUP Retrieval Request
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @fsnc: optional FSNC value
 */
static int
as_send_maup_retr_req(struct up *up, queue_t *q, mblk_t *msg, uint32_t *fsnc)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
	    UA_SIZE(M2UA_PARM_ACTION) + (fsnc ? UA_SIZE(M2UA_PARM_SEQNO) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_RETR_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		*p++ = M2UA_PARM_ACTION;
		if (fsnc) {
			p[0] = htonl(M2UA_ACTION_RTRV_MSGS);
			p[1] = M2UA_PARM_SEQNO;
			p[2] = htonl(*fsnc);
			p += 3;
		} else {
			*p++ = htonl(M2UA_ACTION_RTRV_BSN);
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "MAUP RETR Req ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);

}

/**
 * as_send_maup_data_ack: - send MAUP Data Ack
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @corid: correlation id
 */
static inline fastcall __hot_in int
as_send_maup_data_ack(struct up *up, queue_t *q, mblk_t *msg, uint32_t corid)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.config.ppa.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.config.ppa.iid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.config.ppa.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
	    UA_SIZE(M2UA_PARM_CORR_ID_ACK);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.config.ppa.iid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.config.ppa.iid);
			p += 2;
		}
		if (up->as.config.ppa.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.config.ppa.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		p[0] = M2UA_PARM_CORR_ID_ACK;
		p[1] = htonl(corid);
		p += 2;
		mp->b_wptr = (unsigned char *) p;

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "MAUP DATA Ack ->");
		if (unlikely((err = t_optdata_req(up->tp.tp, q, msg, 0, up->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  SGP to ASP UA protocol messages
 *  =========================================================================
 */

/**
 * as_lookup: - lookup up AS from message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static struct up *
as_lookup(struct tp *tp, queue_t *q, mblk_t *mp, int *errp)
{
	struct ua_parm iid = { {NULL,}, };
	struct up *up = NULL;

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &iid, UA_PARM_IID)) {
		for (up = tp->up.list; up; up = up->tp.next)
			if (up->as.config.ppa.iid == iid.val)
				goto found;
	}
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &iid, UA_PARM_IID_TEXT)) {
		for (up = tp->up.list; up; up = up->tp.next)
			if (strncmp(iid.cp, (caddr_t) &up->as.config.ppa.iid, min(iid.len, 32)) ==
			    0)
				goto found;
	}
	/* if there is precisely one up, we do not need IID to identify the AS */
	if ((up = tp->up.list) && !up->tp.next)
		goto found;

	if (errp)
		*errp = -ESRCH;	/* could not find AS */
	return (NULL);
      found:
	if (errp)
		*errp = 0;
	return (up);
}

/**
 * as_recv_mgmt_err: - receive MGMT ERR message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Error messages pertaining to an AS or AS state must contain an IID.
 */
static int
as_recv_mgmt_err(struct up *up, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
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
	struct up *up;

	if ((up = as_lookup(tp, q, mp, NULL)))
		return as_recv_mgmt_err(up, q, mp);

	/* FIXME: process as without IID */
	freemsg(mp);
	return (0);
}

/**
 * as_recv_mgmt_ntfy: - receive MGMT NTFY message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Notification messages pertaning to an AS or AS state change must contain an IID.
 */
static int
as_recv_mgmt_ntfy(struct up *up, queue_t *q, mblk_t *mp)
{
	struct ua_parm status;
	int ecode;

	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &status, UA_PARM_STATUS))
		goto missing_parm;
	switch (status.val) {
	case UA_STATUS_AS_DOWN:
	case UA_STATUS_AS_INACTIVE:
	case UA_STATUS_AS_ACTIVE:
	case UA_STATUS_AS_PENDING:
	case UA_STATUS_AS_INSUFFICIENT_ASPS:
	case UA_STATUS_ASP_FAILURE:
	case UA_STATUS_AS_MINIMUM_ASPS:
	default:
		/* All of these are just advisory.  For now we discard them.  Later we should
		   generate and SL_EVENT_IND with an appropriate event. */
		goto discard;
	case UA_STATUS_ALTERNATE_ASP_ACTIVE:
		/* In the case of an Override mode AS, reception of an ASP Active message at an SGP
		   causes the (re)direction of all traffic for the AS to the ASP that sent the ASP
		   Active message.  Any previously active ASP in the AS is now considered to be in
		   the state ASP Inactive and SHOULD no longer receive traffic from the SGP within
		   the AS.  The SGP MUST send a Notify message ("Alternate ASP Active") to the
		   previously active ASP in the AS, and SHOULD stop traffic to/from that ASP.  The
		   ASP receiving this Notify MUST consider itself now in the ASP-INACTIVE state, it
		   it is not already aware of this via inter-ASP communication with the Overriding
		   ASP. */
		switch (up_get_u_state(up)) {
		case ASP_INACTIVE:
		case ASP_WACK_ASPUP:
		case ASP_WACK_ASPDN:
		case ASP_DOWN:
			goto discard;
		case ASP_WACK_ASPIA:
			up_set_u_state(up, ASP_INACTIVE);
			if (up_get_i_state(up) == LMI_DISABLE_PENDING)
				return lmi_disable_con(up, q, mp);
			goto outstate;
		case ASP_WACK_ASPAC:
			up_set_u_state(up, ASP_INACTIVE);
			if (up_get_i_state(up) == LMI_ENABLE_PENDING)
				return lmi_error_ind(up, q, mp, LMI_INITFAILED, LMI_DISABLED);
			goto outstate;
		case ASP_ACTIVE:
			up_set_u_state(up, ASP_INACTIVE);
			if (up_get_i_state(up) == LMI_ENABLED)
				return lmi_error_ind(up, q, mp, LMI_DISC, LMI_DISABLED);
			goto outstate;
		default:
		case ASP_UP:
			goto outstate;
		}
	      outstate:
		strlog(up->mid, up->sid, 0, SL_TRACE,
		       "Alternate ASP Active in invalid state %u:%u%u", up_get_i_state(up),
		       up_get_u_state(up), up_get_l_state(up));
		up_set_u_state(up, ASP_DOWN);
		up_set_l_state(up, DL_UNBOUND);
		return lmi_error_ind(up, q, mp, LMI_FATALERR, LMI_UNUSABLE);
	}
	/* FIXME */
      discard:
	freemsg(mp);
	return (0);
      missing_parm:
	ecode = UA_ECODE_MISSING_PARAMETER;
	goto error;
      error:
	return tp_send_mgmt_err(up->tp.tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
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
	struct ua_parm status;
	struct up *up;
	int ecode;

	if ((up = as_lookup(tp, q, mp, NULL)))
		return as_recv_mgmt_ntfy(up, q, mp);

	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &status, UA_PARM_STATUS))
		goto missing_parm;
	switch (status.val) {
	case UA_STATUS_AS_DOWN:
	case UA_STATUS_AS_INACTIVE:
	case UA_STATUS_AS_ACTIVE:
	case UA_STATUS_AS_PENDING:
	case UA_STATUS_AS_INSUFFICIENT_ASPS:
	case UA_STATUS_AS_MINIMUM_ASPS:
	case UA_STATUS_ALTERNATE_ASP_ACTIVE:
		/* all of the above must have an IID unless there is precisely one AS */
		goto missing_parm;
	default:
	case UA_STATUS_ASP_FAILURE:
		/* TODO: eventually we should provide a signalling link notification event that the
		   ASP with the associated ASP Id has failed.  For now, just discard. */
		goto discard;
	}
	/* FIXME: process as without IID */
      discard:
	freemsg(mp);
	return (0);
      missing_parm:
	ecode = UA_ECODE_MISSING_PARAMETER;
	goto error;
      error:
	return tp_send_mgmt_err(tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * tp_recv_asps_hbeat_req: - process BEAT Request
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * When processing a heartbeat, if there is an IID in the message or there is precisely one AS for
 * the ASP, respond to the heartbeat on the stream associated with the specified AS.  If there is no
 * IID in the message, respond to the heartbeat on stream 0 as a mangement heartbeat.
 *
 * This is not specifically according to the spec, but is consistent with the correlation id and
 * heartbeat procedures for UAs.  When an OpenSS7 AS or SG generates an AS-related heartbeat, it
 * includes the IID in the message.
 */
static int
tp_recv_asps_hbeat_req(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct ua_parm hbeat;
	struct up *up;

	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &hbeat, UA_PARM_HBDATA))
		return (-EINVAL);

	if ((up = as_lookup(tp, q, mp, NULL)))
		return as_send_asps_hbeat_ack(up, q, mp, hbeat.cp, hbeat.len);

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
	struct ua_parm parm = { {NULL,}, };
	int ecode;

	switch (tp_get_u_state(tp)) {
	case ASP_UP:
		/* Unsolicited: about the only time that an unsolicited ASP Up message is expected
		   is when the SG updates options */
	case ASP_WACK_ASPUP:
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &parm, UA_PARM_ASPID))
			tp->sp.options.aspid = parm.val;
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &parm, UA_PARM_ASPEXT)) {
			uint32_t *p = parm.wp;
			uint popt = 0, opt;

			if (parm.len < sizeof(uint32_t))
				goto field_error;

			while (p < ((uint32_t *) parm.cp))
				if ((opt = *p++) >= UA_ASPEXT_NONE || UA_ASPEXT_ASPCONG >= opt)
					popt |= (1 << opt);
			tp->sg.options.options.popt &= ~((1 << (UA_ASPEXT_ASPCONG + 1)) - 1);
			tp->sg.options.options.popt |= popt;
		}
		break;
	default:
		goto outstate;
	}
	if (tp_get_u_state(tp) == ASP_WACK_ASPUP) {
		/* Solicited */
		mi_timer_stop(tp->wack_aspup);
		tp_set_u_state(tp, ASP_UP);
	}
	freemsg(mp);
	return (0);
      field_error:
	ecode = UA_ECODE_PARAMETER_FIELD_ERROR;
	goto error;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_TRACE, "ASPUP Ack in unexpected state %u:%u",
	       tp_get_i_state(tp), tp_get_u_state(tp));
	ecode = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	return tp_send_mgmt_err(tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * tp_recv_asps_aspdn_ack: - process ASP Down Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * ASPDN Ack can be solicited or unsolicited indicating NIF isolation.  When it was solicited, we
 * are in the ASP_WACK_ASPDN state and we complete the ASP_DOWN procedure. ASPDN Ack is only
 * solicited when we are shutting down an SCTP transport, therefore, the last step in the procedure
 * is an orderly disconnect.
 */
static int
tp_recv_asps_aspdn_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct up *up;
	int err = 0;

	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	switch (tp_get_u_state(tp)) {
	case ASP_UP:
	case ASP_WACK_ASPUP:
	case ASP_WACK_ASPDN:
		tp_set_u_state(tp, ASP_DOWN);
		mi_timer_stop(tp->wack_aspup);
		mi_timer_stop(tp->wack_aspdn);
		for (up = tp->up.list; up; up = up->tp.next) {
			up_set_u_state(up, ASP_DOWN);
			mi_timer_cancel(up->wack_aspac);
			mi_timer_cancel(up->wack_aspia);
			if (tp->sg.options.options.pvar != UA_VERSION_TS102141
			    && (tp->sg.options.options.popt & UA_SG_DYNAMIC)) {
				/* ETSI TS 102 141 does not permit dynamic configuration. */
				if ((err = lmi_error_ind(up, q, mp, LMI_DISC, LMI_UNATTACHED)))
					break;
			} else {
				switch (up_get_i_state(up)) {
				case LMI_DETACH_PENDING:
				case LMI_ATTACH_PENDING:
				case LMI_UNATTACHED:
				default:
					strlog(up->mid, up->sid, 0, SL_ERROR,
					       "%s: unexpected in up state %u:%u", __FUNCTION__,
					       up_get_i_state(up), up_get_u_state(up));
					/* fall through */
				case LMI_ENABLED:
				case LMI_ENABLE_PENDING:
				case LMI_DISABLE_PENDING:
					if ((err =
					     lmi_error_ind(up, q, mp, LMI_DISC, LMI_DISABLED)))
						break;
					continue;
				case LMI_DISABLED:
					/* probably already disabled on a previous pass */
					continue;
				}
				break;
			}
		}
		if (err)
			return (err);
		switch (tp_get_u_state(tp)) {
		case ASP_UP:
		case ASP_WACK_ASPUP:
			tp_set_u_state(tp, ASP_DOWN);
			/* try to bring ASP back up */
			return tp_send_asps_aspup_req(tp, q, mp,
						      tp->sp.options.aspid ? &tp->sp.options.
						      aspid : NULL, NULL, 0);
		case ASP_WACK_ASPDN:
			tp_set_u_state(tp, ASP_DOWN);
			/* continue disconnection */
			return t_ordrel_req(tp, q, mp, NULL);
		}
	case ASP_DOWN:
		goto discard;
	default:
		goto outstate;
	}
      discard:
	err = 0;
	goto error;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_TRACE, "ASPDN Ack in unexpected state %u:%u",
	       tp_get_i_state(tp), tp_get_u_state(tp));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * as_recv_asps_hbeat_ack: - receive BEAT Ack message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Note that BEAT Ack messages in reply to a BEAT message that contains an IID should also contain
 * an IID.  This is not strictly to spec, but it is what the OpenSS7 stack does.
 */
static int
as_recv_asps_hbeat_ack(struct up *up, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
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
	struct up *up;

	if ((up = as_lookup(tp, q, mp, NULL)))
		return as_recv_asps_hbeat_ack(up, q, mp);

	/* If the message does not include an IID, then perhaps the IID is contained in the
	   heartbeat data.  The OpenSS7 stack formats heatbeat data with TLVs in the same manner as
	   normal messages. */
	/* FIXME: process as without IID */
	freemsg(mp);
	return (0);
}

/**
 * as_recv_aspt_aspac_ack: - receive ASPAC Ack message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_aspt_aspac_ack(struct up *up, queue_t *q, mblk_t *mp)
{
	struct ua_parm parm = { {NULL,}, };
	int ecode;

	switch (up_get_u_state(up)) {
	case ASP_ACTIVE:
		/* Unsolicited: about the only time that an unsolicited ASP Active message is
		   expected is when the SG supports draft-bidulock-sigtran-sginfo */
	case ASP_WACK_ASPAC:
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &parm, UA_PARM_TMODE))
			up->as.config.ppa.tmode = parm.val;
		if (up->tp.tp->sp.options.options.popt & UA_SG_SGINFO) {
			if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &parm, UA_PARM_PROTO_LIMITS)) {
				/* update the protocol limits information per
				   draft-bidulock-sigtran-sginfo */
				if (parm.len != 2 * sizeof(uint32_t))
					goto field_error;
				up->info.lmi_max_sdu = ntohl(parm.wp[0]);
				up->info.lmi_min_sdu = ntohl(parm.wp[1]);
			}
		}
		break;
	default:
		goto outstate;
	}
	if (up_get_u_state(up) == ASP_WACK_ASPAC) {
		/* Solicited. */
		mi_timer_stop(up->wack_aspac);
		up_set_u_state(up, ASP_ACTIVE);
		if (up_get_i_state(up) == LMI_ENABLE_PENDING)
			return lmi_enable_con(up, q, mp);
	}
	freemsg(mp);
	return (0);
      field_error:
	ecode = UA_ECODE_PARAMETER_FIELD_ERROR;
	goto error;
      outstate:
	ecode = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	return tp_send_mgmt_err(up->tp.tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * tp_recv_apsac_ack: - receive ASPAC Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_aspt_aspac_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct up *up;
	int ecode;

	if ((up = as_lookup(tp, q, mp, NULL)))
		return as_recv_aspt_aspac_ack(up, q, mp);

	mi_timer_stop(tp->wack_aspac);
	goto protocol_error;
	/* An ASPAC Ack with no IID is supposed to pertain to all AS configurated for the ASP.
	   However, we never send an ASPAC Req without an IID and so it is an error to send an
	   ASPAC Ack without an IID. */
      protocol_error:
	ecode = UA_ECODE_PROTOCOL_ERROR;
	goto error;
      error:
	return tp_send_mgmt_err(tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * as_recv_aspt_aspia_ack: - receive ASPIA Ack message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * ASPIA Acks with an IID can be either solicited or unsolicited.  Unsolicited ASPIA Acks are
 * detected when they are received in the incorrect state.  Solicited ASPIA Acks are received in the
 * ASP_WACK_ASPIA state.   Nevertheless, we may have sent multiple ASPIA Requests if there was an
 * ack timeout.  Multiple ASPIA Acks may be returned.  Therefore, if we are already in an
 * ASP_INACTIVE state, the messge is ignored.
 */
static int
as_recv_aspt_aspia_ack(struct up *up, queue_t *q, mblk_t *mp)
{
	switch (up_get_u_state(up)) {
	case ASP_INACTIVE:
	case ASP_WACK_ASPUP:
	case ASP_WACK_ASPDN:
	case ASP_DOWN:
	case ASP_WACK_ASPAC:
		goto ignore;
	case ASP_ACTIVE:
		/* unsolicited */
		up_set_u_state(up, ASP_INACTIVE);
		if (up_get_i_state(up) == LMI_ENABLED)
			/* Note that another possibility here would be to issue a local processor
			   outage indication, but the SG should be smart enough to do that when it
			   is applicable. */
			return lmi_error_ind(up, q, mp, LMI_DISC, LMI_DISABLED);
		goto outstate;
	case ASP_WACK_ASPIA:
		/* solicited */
		up_set_u_state(up, ASP_INACTIVE);
		mi_timer_stop(up->wack_aspia);
		if (up_get_i_state(up) == LMI_DISABLE_PENDING)
			return lmi_disable_con(up, q, mp);
		goto outstate;
	default:
	case ASP_UP:
		goto outstate;
	}
      outstate:
	strlog(up->mid, up->sid, 0, SL_TRACE, "APSIA Ack received in invalid state %u:%u:%u",
	       up_get_i_state(up), up_get_u_state(up), up_get_l_state(up));
	up_set_u_state(up, ASP_DOWN);
	up_set_l_state(up, DL_UNBOUND);
	return lmi_error_ind(up, q, mp, LMI_FATALERR, LMI_UNUSABLE);
      ignore:
	freemsg(mp);
	return (0);
}

/**
 * tp_recv_aspt_apia_ack: - receive ASPIA Ack message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * There are two types of ASPIA Acks: solicited and unsolicited.  An unsolicited ASPIA Ack indicates
 * some form of isolation between the M2UA and NIF at the SG.  Solicited ASPIA Acks have a
 * corresponding ASPIA Req.  Because we never send ASPIA Req without an IID, receiving a solicited
 * ASPIA Ack without an IID is an error.  ASPIA Acks without an IID can therefore always be
 * considered to be unsolicited.
 */
static int
tp_recv_aspt_aspia_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct up *up;
	int err = 0;

	if ((up = as_lookup(tp, q, mp, &err)))
		return as_recv_aspt_aspia_ack(up, q, mp);

	/* ASPIA Acks without IIDs are always unsolicited.  Also, ASPIA Acks without IIDS apply to
	   all AS served. */
	for (up = tp->up.list; up; up = up->tp.next) {
		switch (up_get_u_state(up)) {
		case ASP_DOWN:
		case ASP_WACK_ASPDN:
		case ASP_WACK_ASPUP:
		case ASP_UP:
		case ASP_INACTIVE:
			continue;
		case ASP_WACK_ASPIA:
		case ASP_WACK_ASPAC:
		case ASP_ACTIVE:
			up_set_u_state(up, ASP_INACTIVE);
			mi_timer_stop(up->wack_aspia);
			if ((err = lmi_error_ind(up, q, NULL, LMI_DISC, LMI_DISABLED)))
				break;
			continue;
		default:
			up_set_u_state(up, ASP_DOWN);
			if ((err = lmi_error_ind(up, q, NULL, LMI_FATALERR, LMI_UNUSABLE)))
				break;
			continue;
		}
	}
	if (err == 0)
		freemsg(mp);
	return (err);
}

/**
 * as_recv_maup_estab_con: - receive ESTAB Con message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * The Estab Con message is simply an SL_IN_SERVICE_IND.  Note that instead of an error indication
 * for outstate, we should try to restore synchronization of link state.
 */
static int
as_recv_maup_estab_con(struct up *up, queue_t *q, mblk_t *mp)
{
	int err;

	if (up_get_l_state(up) != DL_OUTCON_PENDING)
		goto outstate;
	up_set_l_state(up, DL_DATAXFER);
	return sl_in_service_ind(up, q, mp);
      outstate:
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT))) {
		err = 0;
		goto error;
	}
	strlog(up->mid, up->sid, 0, SL_TRACE, "ESTAB Con in unexpected state %u:%u:%u",
	       up_get_i_state(up), up_get_u_state(up), up_get_l_state(up));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * as_recv_maup_rel_con: - receive REL Con message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * SS7 links do no require release confirmation.  However, we can use this indication to check
 * state.
 */
static int
as_recv_maup_rel_con(struct up *up, queue_t *q, mblk_t *mp)
{
	int err;

	if (up_get_l_state(up) != DL_IDLE)
		goto outstate;
	/* discarded */
	freemsg(mp);
	return (0);
      outstate:
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT))) {
		err = 0;
		goto error;
	}
	strlog(up->mid, up->sid, 0, SL_TRACE, "REL Con in unexpected state %u:%u:%u",
	       up_get_i_state(up), up_get_u_state(up), up_get_l_state(up));
	err = -EINVAL;
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * as_recv_maup_rel_ind: - receive REL Ind message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * The Rel Con message is simply an SL_OUT_OF_SERVICE_IND.  Unfortunately, the MAUP release
 * indication does not provide the reason for failure (release).
 */
static int
as_recv_maup_rel_ind(struct up *up, queue_t *q, mblk_t *mp)
{
	int err;

	switch (up_get_l_state(up)) {
	case DL_OUTCON_PENDING:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	up_set_l_state(up, DL_IDLE);
	return sl_out_of_service_ind(up, q, mp, SL_FAIL_UNSPECIFIED);
      outstate:
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT))) {
		err = 0;
		goto error;
	}
	strlog(up->mid, up->sid, 0, SL_TRACE, "REL Ind in unexpecteds state %u:%u:%u",
	       up_get_i_state(up), up_get_u_state(up), up_get_l_state(up));
	err = -EINVAL;		/* unexpected message */
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * as_recv_status_lpo_set: - receive Status LPO Set message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Check if we have a pending state request.  If we do, then fine.  Otherwise, attempt to restore
 * the appropriate state if necessary.
 */
static int
as_recv_status_lpo_set(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_LPO_SET)) {
		up->status.pending &= ~(1 << M2UA_STATUS_LPO_SET);
		goto discard;
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status LPO Set confirmation unexpected");
	if (up->status.operate & (1 << M2UA_STATUS_LPO_SET))
		goto discard;
	if (!(up->status.pending & (1 << M2UA_STATUS_LPO_CLEAR)))
		return as_send_maup_state_req(up, q, mp, M2UA_STATUS_LPO_CLEAR);
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * as_recv_status_lpo_clear: - receive Status LPO Clear message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Check if we have a pending state request.  If we do, then fine.  Otherwise, attempt to restore
 * the appropriate state if necessary.
 */
static int
as_recv_status_lpo_clear(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_LPO_CLEAR)) {
		up->status.pending &= ~(1 << M2UA_STATUS_LPO_CLEAR);
		goto discard;
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status LPO Clear confirmation unexpected");
	if (up->status.operate & (1 << M2UA_STATUS_LPO_CLEAR))
		goto discard;
	if (!(up->status.pending & (1 << M2UA_STATUS_LPO_SET)))
		return as_send_maup_state_req(up, q, mp, M2UA_STATUS_LPO_SET);
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * as_recv_status_emer_set: - receive Status EMER Set message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Check if we have a pending state request.  If we do, then fine.  Otherwise, attempt to restore
 * the appropriate state if necessary.
 */
static int
as_recv_status_emer_set(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_EMER_SET)) {
		up->status.pending &= ~(1 << M2UA_STATUS_EMER_SET);
		goto discard;
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status EMER Set confirmation unexpected");
	if (up->status.operate & (1 << M2UA_STATUS_EMER_SET))
		goto discard;
	if (!(up->status.pending & (1 << M2UA_STATUS_EMER_CLEAR)))
		return as_send_maup_state_req(up, q, mp, M2UA_STATUS_EMER_CLEAR);
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * as_recv_status_emer_clear: - receive Status EMER Clear message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_status_emer_clear(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_EMER_CLEAR)) {
		up->status.pending &= ~(1 << M2UA_STATUS_EMER_CLEAR);
		goto discard;
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status EMER Clear confirmation unexpected");
	if (!(up->status.pending & (1 << M2UA_STATUS_EMER_SET)))
		return as_send_maup_state_req(up, q, mp, M2UA_STATUS_EMER_SET);
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * as_recv_status_flush_buffers: - receive Status Flush Buffers message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_status_flush_buffers(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_FLUSH_BUFFERS)) {
		// up->status.pending &= ~(1<<M2UA_STATUS_FLUSH_BUFFERS);
		return sl_rb_cleared_ind(up, q, mp);
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE,
		       "Status Flush Buffers confirmation unexpected");
	freemsg(mp);
	return (0);
}

/**
 * as_recv_status_continue: - receive Status Continue message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_status_continue(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_CONTINUE)) {
		up->status.pending &= ~(1 << M2UA_STATUS_CONTINUE);
		goto discard;
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status Continue confirmation unexpected");
	goto discard;
      discard:
	freemsg(mp);
	return (0);

}

/**
 * as_recv_status_clear_rtb: - receive Status Clear RTB message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_status_clear_rtb(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_CLEAR_RTB)) {
		// up->status.pending &= ~(1<<M2UA_STATUS_CLEAR_RTB);
		return sl_rtb_cleared_ind(up, q, mp);
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status Clear RTB confirmation unexpected");
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * as_recv_status_audit: - receive Status Audit message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_status_audit(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_AUDIT)) {
		up->status.pending &= ~(1 << M2UA_STATUS_AUDIT);
		goto discard;
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status Audit confirmation unexpected");
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * as_recv_status_cong_clear: - receive Status CONG Clear message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_status_cong_clear(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_CONG_CLEAR)) {
		up->status.pending &= ~(1 << M2UA_STATUS_CONG_CLEAR);
		goto discard;
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status CONG Clear confirmation unexpected");
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * as_recv_status_cong_accept: - receive Status CONG Accept message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_status_cong_accept(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_CONG_ACCEPT)) {
		up->status.pending &= ~(1 << M2UA_STATUS_CONG_ACCEPT);
		goto discard;
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status CONG Accept confirmation unexpected");
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/**
 * as_recv_status_cong_discard: - receive Status CONG Discard message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_status_cong_discard(struct up *up, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_CONG_DISCARD)) {
		up->status.pending &= ~(1 << M2UA_STATUS_CONG_DISCARD);
		goto discard;
	}
	strlog(up->mid, up->sid, 0, SL_TRACE, "Status CONG Discard confirmation unexpected");
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

#if 0
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
#endif

/**
 * as_recv_maup_state_con: - receive State Con message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_maup_state_con(struct up *up, queue_t *q, mblk_t *mp)
{
	struct ua_parm status = { {NULL,}, };

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &status, M2UA_PARM_STATE_REQUEST)) {
		switch (status.val) {
		case M2UA_STATUS_LPO_SET:
			return as_recv_status_lpo_set(up, q, mp);
		case M2UA_STATUS_LPO_CLEAR:
			return as_recv_status_lpo_clear(up, q, mp);
		case M2UA_STATUS_EMER_SET:
			return as_recv_status_emer_set(up, q, mp);
		case M2UA_STATUS_EMER_CLEAR:
			return as_recv_status_emer_clear(up, q, mp);
		case M2UA_STATUS_FLUSH_BUFFERS:
			return as_recv_status_flush_buffers(up, q, mp);
		case M2UA_STATUS_CONTINUE:
			return as_recv_status_continue(up, q, mp);
		case M2UA_STATUS_CLEAR_RTB:
			return as_recv_status_clear_rtb(up, q, mp);
		case M2UA_STATUS_AUDIT:
			return as_recv_status_audit(up, q, mp);
		case M2UA_STATUS_CONG_CLEAR:
			return as_recv_status_cong_clear(up, q, mp);
		case M2UA_STATUS_CONG_ACCEPT:
			return as_recv_status_cong_accept(up, q, mp);
		case M2UA_STATUS_CONG_DISCARD:
			return as_recv_status_cong_discard(up, q, mp);
		default:
			return (-EOPNOTSUPP);
		}
	}
	return (-EINVAL);
}

/**
 * as_recv_event_rpo_enter: - receive RPO Enter message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_event_rpo_enter(struct up *up, queue_t *q, mblk_t *mp)
{
	return sl_remote_processor_outage_ind(up, q, mp);
}

/**
 * as_recv_event_rpo_exit: - receive RPO Exit message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_event_rpo_exit(struct up *up, queue_t *q, mblk_t *mp)
{
	return sl_remote_processor_recovered_ind(up, q, mp);
}

/**
 * as_recv_event_lpo_enter: - receive LPO Enter message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_event_lpo_enter(struct up *up, queue_t *q, mblk_t *mp)
{
	return sl_local_processor_outage_ind(up, q, mp);
}

/**
 * as_recv_event_lpo_exit: - receive LPO Exit message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_event_lpo_exit(struct up *up, queue_t *q, mblk_t *mp)
{
	return sl_local_processor_recovered_ind(up, q, mp);
}

#if 0
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
#endif

/**
 * as_recv_maup_state_ind: - receive STATE Ind message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_maup_state_ind(struct up *up, queue_t *q, mblk_t *mp)
{
	struct ua_parm event = { {NULL,}, };

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &event, M2UA_PARM_STATE_EVENT)) {
		switch (event.val) {
		case M2UA_EVENT_RPO_ENTER:
			return as_recv_event_rpo_enter(up, q, mp);
		case M2UA_EVENT_RPO_EXIT:
			return as_recv_event_rpo_exit(up, q, mp);
		case M2UA_EVENT_LPO_ENTER:
			return as_recv_event_lpo_enter(up, q, mp);
		case M2UA_EVENT_LPO_EXIT:
			return as_recv_event_lpo_exit(up, q, mp);
		default:
			return (-EOPNOTSUPP);
		}
	}
	return (-EINVAL);
}

/**
 * as_recv_maup_retr_con: - receive RETR Con message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_maup_retr_con(struct up *up, queue_t *q, mblk_t *mp)
{
	struct ua_parm action, bsnt;

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &action, M2UA_PARM_ACTION)) {
		switch (action.val) {
		case M2UA_ACTION_RTRV_BSN:
			if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &bsnt, M2UA_PARM_SEQNO))
				return sl_bsnt_ind(up, q, mp, bsnt.val);
			return (-EINVAL);
		case M2UA_ACTION_RTRV_MSGS:
			/* ignore */
			freemsg(mp);
			return (0);
		default:
			return (-EINVAL);
		}
	}
	return (-EINVAL);
}

/**
 * as_recv_maup_retr_ind: - receive RETR Ind message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_maup_retr_ind(struct up *up, queue_t *q, mblk_t *mp)
{
	struct ua_parm data;
	unsigned char *b_rptr = mp->b_rptr;
	int err;

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &data, M2UA_PARM_DATA1)) {
		mp->b_rptr = data.cp;
		if ((err = sl_retrieved_message_ind(up, q, mp, 0)) == 0)
			return (0);
		mp->b_rptr = b_rptr;
		return (err);
	}
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &data, M2UA_PARM_DATA2)) {
		mp->b_rptr = data.cp + 1;
		if ((err = sl_retrieved_message_ind(up, q, mp, data.cp[0] & 0x3)) == 0)
			return (0);
		mp->b_rptr = b_rptr;
		return (err);
	}
	return (-EINVAL);
}

/**
 * as_recv_maup_retr_comp_ind: - receive RETR COMP Ind message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_maup_retr_comp_ind(struct up *up, queue_t *q, mblk_t *mp)
{
	struct ua_parm data;
	unsigned char *b_rptr = mp->b_rptr;
	int err;

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &data, M2UA_PARM_DATA1)) {
		mp->b_rptr = data.cp;
		if ((err = sl_retrieval_complete_ind(up, q, NULL, 0, mp)) == 0)
			return (0);
		mp->b_rptr = b_rptr;
		return (err);
	}
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &data, M2UA_PARM_DATA2)) {
		mp->b_rptr = data.cp + 1;
		if ((err = sl_retrieval_complete_ind(up, q, NULL, data.cp[0] & 0x3, mp)) == 0)
			return (0);
		mp->b_rptr = b_rptr;
		return (err);
	}
	return sl_retrieval_complete_ind(up, q, mp, 0, NULL);
}

/**
 * as_recv_maup_cong_ind: - receive CONG Ind message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_maup_cong_ind(struct up *up, queue_t *q, mblk_t *mp)
{
	struct ua_parm cong, disc;
	int ecode;

	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &cong, M2UA_PARM_CONG_STATUS))
		goto missing_parm;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &disc, M2UA_PARM_DISC_STATUS))
		goto missing_parm;
	if (cong.val >= up->cong || disc.val >= up->disc)
		return sl_link_congested_ind(up, q, mp, cong.val, disc.val);
	return sl_link_congestion_ceased_ind(up, q, mp, cong.val, disc.val);
      missing_parm:
	ecode = UA_ECODE_MISSING_PARAMETER;
	goto error;
      error:
	return tp_send_mgmt_err(up->tp.tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * as_recv_maup_data: - receive DATA message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall __hot_in int
as_recv_maup_data(struct up *up, queue_t *q, mblk_t *mp)
{
	unsigned char *b_rptr = mp->b_rptr;
	struct ua_parm data;
	int ecode, err;

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &data, M2UA_PARM_DATA1)) {
		mp->b_rptr = data.cp;
		if ((err = sl_pdu_ind(up, q, mp, 0)) == 0)
			return (0);
		mp->b_rptr = b_rptr;
		return (err);
	}
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &data, M2UA_PARM_DATA2)) {
		mp->b_rptr = data.cp + 1;
		if ((err = sl_pdu_ind(up, q, mp, data.cp[0] & 0x3)) == 0)
			return (0);
		mp->b_rptr = b_rptr;
		return (err);
	}
	goto missing_parm;
      missing_parm:
	ecode = UA_ECODE_MISSING_PARAMETER;
	goto error;
      error:
	return tp_send_mgmt_err(up->tp.tp, q, mp, ecode, mp->b_rptr, mp->b_wptr - mp->b_rptr);
}

/**
 * tp_recv_maup_data: - receive DATA message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static inline fastcall __hot_in int
tp_recv_maup_data(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct up *up;
	int err;

	if (!(up = as_lookup(tp, q, mp, &err)) && err)
		return (err);
	return as_recv_maup_data(up, q, mp);
}

/**
 * as_recv_maup_data_ack: - receive DATA Ack message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_maup_data_ack(struct up *up, queue_t *q, mblk_t *mp)
{
	/* ignore for now */
	freemsg(mp);
	return (0);
}

/**
 * as_recv_rkmm_reg_rsp: - receive REG Rsp message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_rkmm_reg_rsp(struct up *up, queue_t *q, mblk_t *mp, uint32_t status, uint32_t iid)
{
	int err, error = 0;

	up->as.config.ppa.iid = iid;

	if (up_get_u_state(up) != ASP_WACK_ASPUP)
		goto outstate;
	if (up_get_i_state(up) != LMI_ATTACH_PENDING)
		goto outstate;
	switch (status) {
	case 0:		/* Successfully Registered */
		error = 0;
		break;
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
	if ((err = lmi_error_ack(up, q, mp, LMI_ATTACH_REQ, error)) == 0)
		up->as.request_id = 0;
	return (err);
      outstate:
	strlog(up->mid, up->sid, 0, SL_TRACE, "REG RSP in unexpected state %u:%u",
	       up_get_i_state(up), up_get_u_state(up));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * as_recv_rkmm_dereg_rsp: - receive DEREG Rsp message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_rkmm_dereg_rsp(struct up *up, queue_t *q, mblk_t *mp, uint32_t status, uint32_t iid)
{
	int err, error = 0;

	if (up_get_u_state(up) != ASP_WACK_ASPDN)
		goto outstate;
	if (up_get_i_state(up) != LMI_DETACH_PENDING)
		goto outstate;
	switch (status) {
	case 0:		/* Successfully De-registered */
		error = 0;
		break;
	default:		/* protocol error */
	case 1:		/* Error - Unknown */
		error = LMI_UNSPEC;
		break;
	case 2:		/* Error - Invalid Interface Identifier */
		error = LMI_BADPPA;
		break;
	case 3:		/* Error - Permission Denied */
		error = -EPERM;
		break;
	case 4:		/* Error - Not Registered */
		error = 0;
		break;
	}
	if ((err = lmi_ok_ack(up, q, mp, LMI_DETACH_REQ)) == 0)
		up->as.config.ppa.iid = 0;
	return (err);
      outstate:
	strlog(up->mid, up->sid, 0, SL_TRACE, "DEREG RSP in unexpected state %u:%u",
	       up_get_i_state(up), up_get_u_state(up));
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * tp_recv_err: - process receive error
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 * @err: error
 *
 * The purpose of this function is to collect common return errors together and provide treatment in
 * a single rather unlikely non-inline function.
 */
static noinline fastcall int
tp_recv_err(struct tp *tp, queue_t *q, mblk_t *mp, int err)
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
	case -ESRCH:
	{
		struct ua_parm iid = { {NULL,}, };

		/* Return codes that result from the failure to locate an AS for a given MAUP
		   message (ESRCH) should also respond by attempting to disable the corresponding
		   application server.  In this way, we can simply close upper SL streams in
		   up_qclose() and the arrival of any message for the AS will result in the AS
		   being deactivated if the AS could not be deactivated during close (perhaps from
		   failure to allocate a buffer). */
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &iid, UA_PARM_IID))
			return tp_send_aspt_aspia_req(tp, q, mp, &iid.val, NULL, 0, NULL, 0);
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &iid, UA_PARM_IID_TEXT))
			return tp_send_aspt_aspia_req(tp, q, mp, NULL, iid.cp, iid.len, NULL, 0);
		if (tp->up.list == NULL)
			return tp_send_aspt_aspia_req(tp, q, mp, NULL, NULL, 0, NULL, 0);
		err = UA_ECODE_UNEXPECTED_MESSAGE;
		goto error;
	}
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
	case -ENXIO:
		err = UA_ECODE_MISSING_PARAMETER;
		goto error;
	default:
		if (err < 0)
			err = UA_ECODE_PROTOCOL_ERROR;
	      error:
		return tp_send_mgmt_err(tp, q, mp, err, mp->b_rptr, mp->b_wptr - mp->b_rptr);
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
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ERR <-");
		err = tp_recv_mgmt_err(tp, q, mp);
		break;
	case UA_MGMT_NTFY:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "NTFY <-");
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
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "BEAT <-");
		err = tp_recv_asps_hbeat_req(tp, q, mp);
		break;
	case UA_ASPS_ASPUP_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ASPUP Ack <-");
		err = tp_recv_asps_aspup_ack(tp, q, mp);
		break;
	case UA_ASPS_ASPDN_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ASPDN Ack <-");
		err = tp_recv_asps_aspdn_ack(tp, q, mp);
		break;
	case UA_ASPS_HBEAT_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "BEAT Ack <-");
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
	int err;

	switch (UA_MSG_TYPE(p[0])) {
	case UA_ASPT_ASPAC_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ASPAC Ack <-");
		err = tp_recv_aspt_aspac_ack(tp, q, mp);
		break;
	case UA_ASPT_ASPIA_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ASPIA Ack <-");
		err = tp_recv_aspt_aspia_ack(tp, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
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
	struct up *up;
	int err;

	if (!(up = as_lookup(tp, q, mp, &err)) && err)
		return (err);

	switch (UA_MSG_TYPE(p[0])) {
	case M2UA_MAUP_ESTAB_CON:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "ESTAB Con <-");
		err = as_recv_maup_estab_con(up, q, mp);
		break;
	case M2UA_MAUP_REL_CON:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "REL Con <-");
		err = as_recv_maup_rel_con(up, q, mp);
		break;
	case M2UA_MAUP_REL_IND:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "REL Ind <-");
		err = as_recv_maup_rel_ind(up, q, mp);
		break;
	case M2UA_MAUP_STATE_CON:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "STATE Con <-");
		err = as_recv_maup_state_con(up, q, mp);
		break;
	case M2UA_MAUP_STATE_IND:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "STATE Ind <-");
		err = as_recv_maup_state_ind(up, q, mp);
		break;
	case M2UA_MAUP_RETR_CON:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "RETR Con <-");
		err = as_recv_maup_retr_con(up, q, mp);
		break;
	case M2UA_MAUP_RETR_IND:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "RETR Ind <-");
		err = as_recv_maup_retr_ind(up, q, mp);
		break;
	case M2UA_MAUP_RETR_COMP_IND:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "RETR COMP Ind <-");
		err = as_recv_maup_retr_comp_ind(up, q, mp);
		break;
	case M2UA_MAUP_CONG_IND:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "CONG Ind <-");
		err = as_recv_maup_cong_ind(up, q, mp);
		break;
	case M2UA_MAUP_DATA:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "DATA <-");
		err = as_recv_maup_data(up, q, mp);
		break;
	case M2UA_MAUP_DATA_ACK:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "DATA Ack <-");
		err = as_recv_maup_data_ack(up, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
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
	struct ua_parm request_id;
	struct up *up;
	int err;

	/* FIXME: mesasge can contain multiple registration results (but probably won't) */
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &request_id, M2UA_PARM_REG_RESULT)) {
		uint32_t status = 0;
		uint32_t iid = 0;

		/* FIXME also need M2UA_PARM_DEREG_RESULT, which does not have link key id */
		for (up = tp->up.list; up; up = up->tp.next)
			if (up->as.request_id == request_id.val)
				break;
		if (up == NULL)
			/* FIXME: process error for each registration result instead of returning
			   it */
			return (-EPROTO);

		switch (UA_MSG_TYPE(p[0])) {
		case UA_RKMM_REG_RSP:
			/* FIXME: dig registration status and interface identifier out of the
			   registration result */
			strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "REG Rsp <-");
			err = as_recv_rkmm_reg_rsp(up, q, mp, status, iid);
			break;
		case UA_RKMM_DEREG_RSP:
			/* FIXME: dig registration status out of the deregistration result */
			strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "DEREG Rsp <-");
			err = as_recv_rkmm_dereg_rsp(up, q, mp, status, iid);
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
	if (mp->b_wptr < mp->b_rptr + ntohl(p[1]))
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
		/* TS 102 141 says that if a link key management message is received to return an
		   unregcognized message class error. */
		if (tp->sp.options.options.pvar == UA_VERSION_TS102141)
			goto enoprotoopt;
		err = tp_recv_rkmm(tp, q, mp);
		break;
	default:
	      enoprotoopt:
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
			if (UA_MSG_CLAS(p[0]) == UA_CLASS_MAUP)
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
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_info_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	return lmi_info_ack(up, q, mp);
}

/**
 * lmi_attach_req - process LMI_ATTACH_REQ primtive
 * @up: UP private structure
 * @tp: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * All M2UA-AS upper multiplex Streams (SL-AS) are Style II links.  That is, they must be attached
 * to a PPA before they are enabled.  For M2UA-AS the form of the PPA depends upon whether the SG
 * requires dynamic registration or not.  When the SG requires dynamic registration, the PPA takes
 * the form of an SGID an SDLI and an SDTI.  When the SG requires static configuration, the PPA
 * takes the form of an SGID and an IID (either 32-bit integer or text string).
 *
 * It is possible to use the minor device number opened to specify the SGID; however, because the
 * PPA is variable length for the SLI, it is quite possible to provide 64-bits of information.
 * This saves creating so many device nodes or using /dev/streams/m2ua-as/NN minor device nodes.
 * Therefore, when the device opened was the minor device 0, an SGID must be specified in the PPA.
 * When the device opened was the minor device NNN, where NNN is a number from 1 to 16367, the SGID
 * does not need to be present in the PPA and the opened minor device number is used as the SGID
 * instead.
 *
 * For OpenSS7 SGs, the SDLI/SDTI combination and the IID are the same value.  When an ASP
 * registers a link key composed of an SDLI and SDTI, the IID returned to the registration request
 * is simply the 16-bit SDLI and SDTI concatenated into a 32-bit IID.  Therefore, regardless of
 * whether the SG requires dynamic registration or not, the PPA is formatted as a 32-bit SGID
 * followed by a 32-bit IID that can be viewed as composed of a 16-bit SDLI concatenated with a
 * 16-bit SDTI when the SG requires dynamic registration.
 *
 * The driver also accepts text IIDs.  Text IIDs are of any length other than 4 or 8 bytes.  4 byte
 * long PPAs are interpreted as an integer IID (or SDLI/SDTI) if the opened device was a non-zero
 * minor device number.  8 byte long PPAs are interpreted as an integer SGID followed by an integer
 * IID (or SDLI/SDTI).  When the opened device was a non-zero minor device, PPAs of length other
 * than 4 are interpreted as a text IID.  The text IID must include the terminating null character
 * if the text IID is shorter than the PPA.  (This way the PPA can be made of arbitrary length,
 * regardless of the length of the text identifier.)  When the opened device was the zero minor
 * device, PPAs of length other than 8 are interpreted as a 4 byte integer SGID followed by a text
 * IID (including the null terminating character if the text IID is shorter than the PPA).  The
 * driver allocates storage for 32 bytes of text IID.  Text IIDs longer than 32 bytes will be
 * truncated to 32 bytes.
 * 
 * The SGID can be used directly to locate the lower multiplex Stream.  If there does not exist a
 * lower multiplex stream with the requested SGID, the request wil fail.  When the lower multiplex
 * stream exists, the SL streams attached to the stream can be searched to determine if another
 * stream is already attached to the SDLI/SDTI or IID.  Only one stream can be attached to a given
 * SDLI/SDTI or IID at a time.  The second request to attach to the same SDLI/SDTI or IID will fail.
 * When successful, if the lower SCTP stream is active (ASP Up), and the SG requires dynamic
 * registration, dynamic registration will be performed on the SDLI/SDTI used as a Link Key.  If the
 * lower stream is not ASP Up, or the registration fails, the attach request will fail.  If the SG
 * does not require registration, the IID will be assigned to the attached stream and the attach
 * operation will succeed.  The SCTP association does not need to be in the ASP Up state at this
 * point (it will when the LMI_ENABLE_REQ appears).
 *
 * As another wrinkle, the SL-User should be permitted to reattach the signalling link without
 * having to know the PPA or other particulars about the signalling link.  To accomplish this, once
 * the Signalling Link has been attached, we remember the PPA.  Then to re-attach the signalling
 * link, the SL-User issues an LMI_ATTACH_REQ with a zero-length PPA.
 */
static noinline fastcall int
lmi_attach_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct up *sl2;
	uint32_t sgid, iid;
	caddr_t ppa_ptr, iid_ptr;
	size_t ppa_len, iid_len;
	unsigned long flags;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_UNATTACHED)
		goto outstate;
	ppa_ptr = (caddr_t) p->lmi_ppa;
	ppa_len = mp->b_wptr - mp->b_rptr - sizeof(*p);
	if (ppa_len == 0) {
		/* the user can specific a zero-length PPA when reattaching */
		if (up->as.config.ppa.sdti == 0 && up->as.config.ppa.sdli == 0
		    && up->as.config.ppa.iid == 0 && up->as.config.ppa.iid_text[0] == '\0')
			goto badppa;
		if ((sgid = up->as.config.ppa.sgid) == 0)
			goto badppa;
		iid =
		    ((up->as.config.ppa.sdti << 16) | up->as.config.ppa.sdli) ? : up->as.config.ppa.
		    iid;
		iid_ptr =
		    (up->as.config.ppa.iid_text[0] == '\0') ? NULL : up->as.config.ppa.iid_text;
		iid_len = iid_ptr ? strnlen(iid_ptr, 32) : 0;
	} else {
		size_t sgid_len = ((sgid = up->unit) == 0) ? sizeof(uint32_t) : 0;

		if (ppa_len < sgid_len + 1)
			goto badppa;
		sgid = sgid ? : *(uint32_t *) p->lmi_ppa;
		if (ppa_len == sgid_len + sizeof(uint32_t)) {
			iid = *(uint32_t *) ((caddr_t) p->lmi_ppa + sgid_len);
			iid_ptr = NULL;
			iid_len = 0;
		} else {
			iid = 0;
			iid_ptr = (caddr_t) p->lmi_ppa + sgid_len;
			iid_len = ppa_len - sgid_len;
			iid_len = strnlen(iid_ptr, iid_len);
			if (iid_len == 0)
				goto badppa;
			if (iid_len > 32)
				iid_len = 32;
		}
	}
	write_lock_irqsave(&ua_mux_lock, flags);
	for (tp = (struct tp *) mi_first_ptr(&ua_links); tp && tp->sg.id != sgid;
	     tp = (struct tp *) mi_next_ptr((caddr_t) tp)) ;
	if (tp == NULL) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		goto badppa;
	}
	if (!tp_trylock(tp, q)) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		return (-EDEADLK);
	}
	if (tp->sp.options.options.pvar != UA_VERSION_TS102141
	    && (tp->sg.options.options.popt & UA_SG_DYNAMIC)) {
		/* ETSI TS 102 141 does not permit dynamic configuration. */
		if (iid_ptr != NULL) {
			tp_unlock(tp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			goto badppa;
		}
		if (tp_get_u_state(tp) != ASP_UP) {
			tp_unlock(tp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			goto enxio;
		}
		/* interpret iid as link key */
		for (sl2 = tp->up.list; sl2; sl2 = sl2->tp.next)
			if (((up->as.config.ppa.sdti << 16) | up->as.config.ppa.sdli) == iid)
				break;
	} else if (iid_ptr) {
		/* iid is text */
		if (tp->sp.options.options.pvar == UA_VERSION_TS102141
		    || !(tp->sg.options.options.popt & UA_SG_TEXTIID)) {
			/* ETSI TS 102 141 does not permit the use of text interface identifiers. */
			tp_unlock(tp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			goto badppa;
		}
		for (sl2 = tp->up.list; sl2; sl2 = sl2->tp.next)
			if (strncmp(sl2->as.config.ppa.iid_text, iid_ptr, iid_len) == 0)
				break;
	} else {
		/* iid is integer */
		for (sl2 = tp->up.list; sl2; sl2 = sl2->tp.next)
			if (sl2->as.config.ppa.iid == iid)
				break;
	}
	if (sl2) {
		if (sl2 != up) {
			tp_unlock(tp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			goto ebusy;
		}
		/* already attached, probably from previous run that required buffers */
	} else {
		up_tp_link(up, tp);
		up->as.config.ppa.sdti = (iid >> 16) & 0xffff;
		up->as.config.ppa.sdli = (iid >> 0) & 0xffff;
		up->as.config.ppa.iid = 0;
		up->as.config.ppa.iid_text[0] = '\0';
		if (iid_len)
			strncpy(iid_ptr, up->as.config.ppa.iid_text, iid_len);
		/* When a signalling link interface is attached to an SCTP transport, it is
		   assigned one of the available DATA (non-zero) stream ids for use for MAUP
		   messages.  The precise stream id number is unimportant: we just want to spread
		   the interfaces over as many streams as possible. Therefore, we assign streams in 
		   a round-robin fashion over the available streams. */
		up->as.streamid = tp->nextstream;
		if (++tp->nextstream >= tp->xp.options.ostreams)
			tp->nextstream = 1;
	}
	if (tp->sp.options.options.pvar != UA_VERSION_TS102141
	    && (tp->sg.options.options.popt & UA_SG_DYNAMIC)) {
		int rtn;

		/* ETSI TS 102 141 does not permit dynamic configuration. */
		write_unlock_irqrestore(&ua_mux_lock, flags);
		up_set_i_state(up, LMI_ATTACH_PENDING);
		up_set_u_state(up, ASP_WACK_ASPUP);
		/* issue registation request */
		up->as.request_id = atomic_inc_return(&ua_request_id);
		rtn = as_send_rkmm_reg_req(up, q, mp, up->as.request_id, up->as.config.ppa.sdti,
					   up->as.config.ppa.sdli);
		tp_unlock(tp);
		return (rtn);
	} else {
		tp_unlock(tp);
		write_unlock_irqrestore(&ua_mux_lock, flags);
		up_set_i_state(up, LMI_ATTACH_PENDING);
		up_set_u_state(up, ASP_INACTIVE);
		return lmi_ok_ack(up, q, mp, LMI_ATTACH_REQ);
	}
      ebusy:
	return lmi_error_ack(up, q, mp, LMI_ATTACH_REQ, -EBUSY);
      enxio:
	return lmi_error_ack(up, q, mp, LMI_ATTACH_REQ, -ENXIO);
      badppa:
	return lmi_error_ack(up, q, mp, LMI_ATTACH_REQ, LMI_BADPPA);
      tooshort:
	return lmi_error_ack(up, q, mp, LMI_ATTACH_REQ, LMI_TOOSHORT);
      outstate:
	return lmi_error_ack(up, q, mp, LMI_ATTACH_REQ, LMI_OUTSTATE);
}

/**
 * lmi_detach_req - process LMI_DETACH_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_detach_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_DISABLED)
		goto outstate;
	up_set_i_state(up, LMI_DISABLE_PENDING);
	switch (tp_get_u_state(tp)) {
	case ASP_DOWN:
	case ASP_WACK_ASPDN:
	case ASP_WACK_ASPUP:
		up_set_u_state(up, ASP_DOWN);
		break;
	case ASP_UP:
		break;
	default:
		strlog(up->mid, up->sid, 0, SL_ERROR, "%s: unexpected tp u state %d",
		       __FUNCTION__, tp_get_u_state(tp));
		goto unspec;
	}
	switch (up_get_u_state(up)) {
	case ASP_INACTIVE:
		if (tp->sp.options.options.pvar != UA_VERSION_TS102141
		    && (tp->sg.options.options.popt & UA_SG_DYNAMIC)) {
			/* ETSI TS 102 141 does not permit dynamic configuration. */
			/* need to deregister */
			up_set_u_state(up, ASP_WACK_ASPDN);
			return as_send_rkmm_dereg_req(up, q, mp);
		}
		break;
	case ASP_WACK_ASPDN:
		/* wait for deregistration to complete */
		freemsg(mp);
		return (0);
	case ASP_DOWN:
		break;
	case ASP_WACK_ASPUP:
	case ASP_WACK_ASPIA:
	case ASP_WACK_ASPAC:
	case ASP_ACTIVE:
	default:
		strlog(up->mid, up->sid, 0, SL_TRACE, "%s: unexpected up u state %d",
		       __FUNCTION__, up_get_u_state(up));
		goto unspec;
	}
	if ((err = lmi_ok_ack(up, q, mp, LMI_DETACH_REQ)) == 0) {
		unsigned long flags;

		/* Note that this sequence means that we alway have to take the tp lock before
		   taking the up mux lock. */
		write_lock_irqsave(&ua_mux_lock, flags);
		/* remove from tp list */
		up_tp_unlink(up);
		write_unlock_irqrestore(&ua_mux_lock, flags);
	}
	return (err);
      unspec:
	return lmi_error_ack(up, q, mp, LMI_DETACH_REQ, LMI_UNSPEC);
      outstate:
	return lmi_error_ack(up, q, mp, LMI_DETACH_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, LMI_DETACH_REQ, LMI_TOOSHORT);

}

/**
 * lmi_enable_req - process LMI_ENABLE_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * All M2UA-AS links must be enabled before use.  Enabling consists of activating the AS
 * corresponding to the signalling link.
 *
 * All M2UA-AS links must be enabled before use.  Enabling can consist of two steps.  If the
 * underlying SCTP association is not connected, it must be connected and the ASP brought up.  If
 * the underlying SCTP association is connected and the ASP is up, then the ASP Active procedure for
 * the IIDs associated with the SL link are invoked.  All invoked procedures must complete before
 * the LMI_ENABLE_CON is issued, but an LMI_OK_ACK can be issued immediately in response to the
 * LMI_ENABLE_REQ.
 */
static noinline fastcall int
lmi_enable_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_DISABLED)
		goto outstate;
	up_set_i_state(up, LMI_ENABLE_PENDING);
	if (tp_get_u_state(tp) != ASP_UP)
		goto initfailed;
	if (up_get_u_state(up) != ASP_INACTIVE)
		goto unspec;
	up_set_u_state(up, ASP_WACK_ASPAC);
	return as_send_aspt_aspac_req(up, q, mp, NULL, 0);
      unspec:
	return lmi_error_ack(up, q, mp, LMI_ENABLE_REQ, LMI_UNSPEC);
      initfailed:
	return lmi_error_ack(up, q, mp, LMI_ENABLE_REQ, LMI_INITFAILED);
      outstate:
	return lmi_error_ack(up, q, mp, LMI_ENABLE_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, LMI_ENABLE_REQ, LMI_TOOSHORT);
}

/**
 * lmi_disable_req - process LMI_DISABLE_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_disable_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_ENABLED)
		goto outstate;
	up_set_i_state(up, LMI_DISABLE_PENDING);
	switch (tp_get_u_state(tp)) {
	case ASP_DOWN:
	case ASP_WACK_ASPDN:
	case ASP_WACK_ASPUP:
		up_set_u_state(up, ASP_INACTIVE);
		return lmi_disable_con(up, q, mp);
	case ASP_UP:
		break;
	}
	switch (up_get_u_state(up)) {
	case ASP_ACTIVE:
		up_set_u_state(up, ASP_WACK_ASPIA);
		return as_send_aspt_aspia_req(up, q, mp, NULL, 0);
	case ASP_WACK_ASPIA:
		freemsg(mp);
		return (0);
	case ASP_INACTIVE:
		return lmi_disable_con(up, q, mp);
	case ASP_WACK_ASPAC:
	default:
		goto unspec;
	}
      unspec:
	return lmi_error_ack(up, q, mp, LMI_DISABLE_REQ, LMI_UNSPEC);
      outstate:
	return lmi_error_ack(up, q, mp, LMI_DISABLE_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, LMI_DISABLE_REQ, LMI_TOOSHORT);
}

/**
 * lmi_optmgmt_req - process LMI_OPTMGMT_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_optmgmt_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * sl_pdu_req - process SL_PDU_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static inline fastcall __hot_write int
sl_pdu_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_ENABLED)
		goto outstate;
	if (up_get_u_state(up) != ASP_ACTIVE)
		goto discard;
	mp->b_cont = NULL;
	if ((up->up.option.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
		if ((err = sl_send_maup_data1(up, tp, q, mp, dp)) != 0)
			mp->b_cont = dp;
	} else {
		if ((err = sl_send_maup_data2(up, tp, q, mp, dp, p->sl_mp)) != 0)
			mp->b_cont = dp;
	}
	return (err);
#if 0
      store:
	bufq_queue(&up->tb, mp);
	return (0);
#endif
      discard:
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(up, q, mp, SL_PDU_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_PDU_REQ, LMI_TOOSHORT);
}

/**
 * sl_emergency_req - process SL_EMERGENCY_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_emergency_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_ENABLED)
		goto outstate;
	if (up_get_u_state(up) != ASP_ACTIVE)
		goto outstate;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_EMER_SET);
      outstate:
	return lmi_error_ack(up, q, mp, SL_EMERGENCY_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_EMERGENCY_REQ, LMI_TOOSHORT);
}

/**
 * sl_emergency_ceases_req - process SL_EMERGENCY_CEASES_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_emergency_ceases_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_ENABLED)
		goto outstate;
	if (up_get_u_state(up) != ASP_ACTIVE)
		goto outstate;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_EMER_CLEAR);
      outstate:
	return lmi_error_ack(up, q, mp, SL_EMERGENCY_CEASES_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_EMERGENCY_CEASES_REQ, LMI_TOOSHORT);
}

/**
 * sl_start_req - process SL_START_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * This primitive is only valid in state LS_INIT or LS_FAILED.  It is ignored in any other state.
 */
static noinline fastcall int
sl_start_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_ENABLED)
		goto outstate;
	if (up_get_u_state(up) != ASP_ACTIVE)
		return sl_local_processor_outage_ind(up, q, mp);
	return sl_send_maup_estab_req(up, tp, q, mp);
      outstate:
	return lmi_error_ack(up, q, mp, SL_START_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_START_REQ, LMI_TOOSHORT);
}

/**
 * sl_stop_req - process SL_STOP_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * This primitive is only valid in state LS_STARTING, LS_IN_SERVICE, LS_REM_PO, and is ignored in
 * any other state.
 */
static noinline fastcall int
sl_stop_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_ENABLED)
		goto outstate;
	switch (up_get_s_state(up)) {
	case LS_STARTING:
	case LS_IN_SERVICE:
	case LS_REM_PO:
		break;
	default:
		goto ignore;
	}
	return sl_send_maup_rel_req(up, tp, q, mp);
      ignore:
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(up, q, mp, SL_STOP_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_STOP_REQ, LMI_TOOSHORT);
}

/**
 * sl_retrieve_bsnt_req - process SL_RETRIEVE_BSNT_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * This primitive is only really valid in state LS_FAILED, but is responded to in any state.
 */
static noinline fastcall int
sl_retrieve_bsnt_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_i_state(up) != LMI_ENABLED)
		goto outstate;
	if (up_get_u_state(up) != ASP_ACTIVE)
		return sl_bsnt_not_retrievable_ind(up, q, mp);
	return sl_send_maup_retr_req(up, tp, q, mp, NULL);
      outstate:
	return lmi_error_ack(up, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_TOOSHORT);
}

/**
 * sl_retrieval_request_and_fsnc_req - process SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_retreival_request_and_fsnc_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;
	uint32_t fsnc;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_IDLE)
		goto outstate;
	fsnc = p->sl_fsnc;
	return sl_send_maup_retr_req(up, tp, q, mp, &fsnc);
      outstate:
	return lmi_error_ack(up, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_TOOSHORT);
}

/**
 * sl_clear_buffers_req - process SL_CLEAR_BUFFERS_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_clear_buffers_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_FLUSH_BUFFERS);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_TOOSHORT);
}

/**
 * sl_clear_rtb_req - process SL_CLEAR_RTB_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_clear_rtb_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CLEAR_RTB);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CLEAR_RTB_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CLEAR_RTB_REQ, LMI_TOOSHORT);
}

/**
 * sl_continue_req - process SL_CONTINUE_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_continue_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_continue_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CONTINUE);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CONTINUE_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CONTINUE_REQ, LMI_TOOSHORT);
}

/**
 * sl_local_processor_outage_req - process SL_LOCAL_PROCESSOR_OUTAGE_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_local_processor_outage_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_DATAXFER)
		goto outstate;
	/* ETSI TS 102 141 does not permit use of this primitive. */
	if (tp->sp.options.options.pvar == UA_VERSION_TS102141)
		goto notsupp;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_LPO_SET);
      notsupp:
	return lmi_error_ack(up, q, mp, SL_LOCAL_PROCESSOR_OUTAGE_REQ, LMI_NOTSUPP);
      outstate:
	return lmi_error_ack(up, q, mp, SL_LOCAL_PROCESSOR_OUTAGE_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_LOCAL_PROCESSOR_OUTAGE_REQ, LMI_TOOSHORT);
}

/**
 * sl_resume_req - process SL_RESUME_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_resume_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_DATAXFER)
		goto outstate;
	/* ETSI TS 102 141 does not permit use of this primitive. */
	if (tp->sp.options.options.pvar == UA_VERSION_TS102141)
		goto notsupp;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_LPO_CLEAR);
      notsupp:
	return lmi_error_ack(up, q, mp, SL_RESUME_REQ, LMI_NOTSUPP);
      outstate:
	return lmi_error_ack(up, q, mp, SL_RESUME_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_RESUME_REQ, LMI_TOOSHORT);
}

/**
 * sl_congestion_discard_req - process SL_CONGESTION_DISCARD_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_congestion_discard_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_cong_discard_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CONG_DISCARD);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_TOOSHORT);
}

/**
 * sl_congestion_accept_req - process SL_CONGESTION_ACCEPT_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_congestion_accept_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_cong_accept_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CONG_ACCEPT);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_TOOSHORT);
}

/**
 * sl_no_congestion_req - process SL_NO_CONGESTION_REQ primtive
 * @up: UP private structure
 * @tp: SL private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_no_congestion_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_no_cong_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_DATAXFER)
		goto outstate;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CONG_CLEAR);
      outstate:
	return lmi_error_ack(up, q, mp, SL_NO_CONGESTION_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_NO_CONGESTION_REQ, LMI_TOOSHORT);
}

/**
 * sl_power_on_req - process SL_POWER_ON_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_power_on_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	sl_power_on_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_l_state(up) != DL_UNBOUND)
		goto outstate;
	return sl_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_AUDIT);
      outstate:
	return lmi_error_ack(up, q, mp, SL_POWER_ON_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_POWER_ON_REQ, LMI_TOOSHORT);
}

#if 0
/**
 * sl_optmgmt_req - process SL_OPTMGMT_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_optmgmt_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
}
#endif

#if 0
/**
 * sl_notify_req - process SL_NOTIFY_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_notify_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
}
#endif

/**
 * sl_other_req - process SL_OTHER_REQ primtive
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_other_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	uint32_t *p = (typeof(p)) mp->b_rptr;

	return lmi_error_ack(up, q, mp, *p, LMI_NOTSUPPORT);
}

/*
 *  UA-User to UA-Management Primitives
 *  -------------------------------------------------------------------------
 */

/**
 * ua_t_establish_req: - process UA_T_ESTABLISH_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_t_establish_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_t_establish_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_t_release_req: - process UA_T_RELEASE_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_t_release_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_t_release_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_t_status_req: - process UA_T_STATUS_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_t_status_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_t_status_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_asp_status_req: - process UA_ASP_STATUS_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_asp_status_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_asp_status_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_as_status_req: - process UA_AS_STATUS_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_as_status_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_as_status_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_asp_up_req: - process UA_ASP_UP_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_asp_up_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_asp_up_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_asp_down_req: - process UA_ASP_DOWN_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_asp_down_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_asp_down_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_asp_active_req: - process UA_ASP_ACTIVE_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_asp_active_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_asp_active_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_asp_inactive_req: - process UA_ASP_INACTIVE_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_asp_inactive_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_asp_inactive_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_reg_req: - process UA_REG_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_reg_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_reg_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_dereg_req: - process UA_DEREG_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_dereg_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_dereg_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(up->mid, up->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * ua_t_primitive_req: - process UA_T_PRIMITIVE_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * The UA_T_PRIMITIVE_REQ primitive provides a way for control Streams to send messages directly to
 * a lower controlled Stream and receive the responsees.  The messages are encapsulated in the
 * UA_T_PRIMITIVE_REQ primitive.  This simply finds the lower Stream from the encapulating SGP
 * identifier, and then passes the encapsulated message to that Stream.  Some state control is
 * performed.
 */
static noinline fastcall int
ua_t_primitive_req(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct UA_t_primitive_req *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sizeof(t_scalar_t))
		goto tooshort;

	if (!(tp = tp_acquire(lm, p->SGP_id, q)))
		return up_error_ack(lm, q, mp, UA_T_PRIMITIVE_REQ, ESRCH);

	if (tp->lm == NULL) {
		read_lock(&ua_mux_lock);
		if (lm != lm_ctrl) {
			read_unlock(&ua_mux_lock);
			goto eperm;
		}
	} else if (tp->lm != lm) {
	      eperm:
		tp_release(tp);
		return up_error_ack(lm, q, mp, UA_T_PRIMITIVE_REQ, EPERM);
	}
	/* ok, yes, you can send to it */
	mp->b_rptr += sizeof(*p);
	switch (*(t_uscalar_t *) mp->b_rptr) {
	case T_CONN_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		if (tp_get_i_state(tp) == TS_IDLE)
			tp_set_i_state(tp, TS_WACK_CREQ);
		break;
	case T_CONN_RES:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		if (tp_get_i_state(tp) == TS_WRES_CIND)
			tp_set_i_state(tp, TS_WACK_CRES);
		break;
	case T_DISCON_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
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
		break;
	case T_DATA_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		break;
	case T_EXDATA_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		break;
	case T_INFO_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		break;
	case T_BIND_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		if (tp_get_i_state(tp) == TS_UNBND)
			tp_set_i_state(tp, TS_WACK_BREQ);
		break;
	case T_UNBIND_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		if (tp_get_i_state(tp) == TS_IDLE)
			tp_set_i_state(tp, TS_WACK_UREQ);
		break;
	case T_UNITDATA_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		break;
	case T_OPTMGMT_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		if (tp_get_i_state(tp) == TS_IDLE)
			tp_set_i_state(tp, TS_WACK_OPTREQ);
		break;
	case T_ORDREL_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		switch (tp_get_i_state(tp)) {
		case TS_DATA_XFER:
			tp_set_i_state(tp, TS_WIND_ORDREL);
			break;
		case TS_WIND_ORDREL:
			tp_set_i_state(tp, TS_IDLE);
			break;
		}
		break;
	case T_OPTDATA_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		break;
	case T_ADDR_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		break;
	case T_CAPABILITY_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", p->SGP_id);
		break;
	default:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_????_???", p->SGP_id);
		break;
	}
	putnext(tp->wq, mp);
	tp_release(tp);
	return (0);
}

/*
 *  Application Server (AS) Timeouts
 *  -------------------------------------------------------------------------
 */

/**
 * up_wack_aspac_timeout: - process timeout waiting for ASP Active Ack
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 */
static noinline fastcall int
up_wack_aspac_timeout(struct up *up, struct tp *tp, queue_t *q)
{
	int err = 0;

	if (up_get_u_state(up) == ASP_WACK_ASPIA)
		if ((err = as_send_aspt_aspac_req(up, q, NULL, NULL, 0)) == 0)
			if (up->as.options.tack) {
				strlog(up->mid, up->sid, UALOGTE, SL_TRACE,
				       "-> WACK ASPAC START <- (%u msec)", up->as.options.tack);
				if (up->wack_aspac)
					mi_timer(up->wack_aspac, up->as.options.tack);
			}
	return (err);
}

/**
 * up_wack_aspia_timeout: - process timeout waiting for ASP Inactive Ack
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 */
static noinline fastcall int
up_wack_aspia_timeout(struct up *up, struct tp *tp, queue_t *q)
{
	int err = 0;

	if (up_get_u_state(up) == ASP_WACK_ASPIA)
		if ((err = as_send_aspt_aspia_req(up, q, NULL, NULL, 0)) == 0)
			if (up->as.options.tack) {
				strlog(up->mid, up->sid, UALOGTE, SL_TRACE,
				       "-> WACK ASPAC START <- (%u msec)", up->as.options.tack);
				if (up->wack_aspia)
					mi_timer(up->wack_aspia, up->as.options.tack);
			}
	return (err);
}

/**
 * up_wack_hbeat_timeout: - process timeout waiting for BEAT Ack
 * @up: UP private structure
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 */
static noinline fastcall int
up_wack_hbeat_timeout(struct up *up, struct tp *tp, queue_t *q)
{
	int err = 0;

#if 0
	if (up_get_u_state(up) == ASP_WACK_HBEAT)
		if ((err = as_send_asps_hbeat_req(up, q, NULL, NULL, 0)) == 0)
			if (up->as.options.tack) {
				strlog(up->mid, up->sid, UALOGTE, SL_TRACE,
				       "-> WACK ASPAC START <- (%u msec)", up->as.options.tack);
				if (up->wack_hbeat)
					mi_timer(up->wack_hbeat, up->as.options.tack);
			}
#endif
	return (err);
}

/*
 *  I_(P)LINK/I_(P)UNLINK Linking of lower streams.
 *  -------------------------------------------------------------------------
 */

/**
 * lm_i_link: - perform I_LINK operation
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * The driver supports I_LINK operations; however, any SGP stream that is linked with an I_LINK
 * operation can only be managed by the control stream linking the lower Stream and cannot be shared
 * across other upper Streams unless configured against an SGID.
 *
 * Note that if this is not the first SGP linked and there are running Users, this SGP will not
 * be available to them until it is configured and brought to the ASP Up state.  If this is the
 * first SGP, there cannot be running users.
 */
static noinline fastcall __unlikely int
lm_i_link(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct T_capability_req *p;
	unsigned long flags;
	struct tp *tp = NULL;
	mblk_t *rp = NULL;
	int err;

	if (!(rp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		err = -ENOBUFS;
		goto error;
	}
	if (!(tp = tp_alloc_priv(l->l_qtop, l->l_index, ioc->ioc_cr, 0))) {
		err = ENOMEM;
		goto error;
	}
	write_lock_irqsave(&ua_mux_lock, flags);

	if (!(lm->tp.tp)) {

		tp->users = 1;
		tp->lm = lm;
		up_tp_link(lm, tp);
	} else {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		err = EALREADY;
		goto error;
	}

	mi_attach(l->l_qtop, (caddr_t) tp);
	tp_unlock(tp);
	write_unlock_irqrestore(&ua_mux_lock, flags);

	mi_copy_done(q, mp, 0);

	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->PRIM_type = T_CAPABILITY_REQ;
	p->CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
	rp->b_wptr = rp->b_rptr + sizeof(*p);
	putnext(tp->wq, rp);	/* immediate capability request */

	return (0);
      error:
	if (rp)
		freeb(rp);
	if (tp)
		tp_free_priv(tp);
	if (err >= 0) {
		mi_copy_done(q, mp, err);
		return (0);
	}
	return (err);
}

/**
 * lm_i_unlink: - perform I_UNLINK operation
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * STREAMS ensures that I_UNLINK will only arrive for temporary links that were formed using this
 * control Stream.  Not all I_UNLINK operations can be refused.  I_UNLINK operations that result
 * from tearing down of the multiplex due to closing of the control Stream cannot be refused.  In
 * general, there is no way to distinguish between an explicit unlink on the control Stream and an
 * implicit unlink resulting from the closing of the control Stream.  For Linux Fast-STREAMS and
 * possibly Solaris there is: the ioc_flag member will have IOC_NONE for forced unlinks not
 * originating from a user and IOC_NATIVE or IOC_ILP32 for I_UNLINK operations originating from the
 * user.
 *
 * Note that the lower multiplex driver put and service procedures must be prepared to be invoked
 * even after the M_IOCACK for the I_UNLINK or I_PUNLINK ioctl has been returned.  This is because
 * the setq(9) back to the Stream head is not performed until after the acknowledgement has been
 * received.  The easiest way to accomplish this is to set q->q_ptr to NULL and have the put and
 * service procedures for the lower multiplex check the private structure pointer for NULL (under
 * suitable lock).
 */
static noinline fastcall __unlikely int
lm_i_unlink(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	unsigned long flags;
	struct tp *tp;
	struct up *up;

	write_lock_irqsave(&ua_mux_lock, flags);

	if (!(tp = TP_PRIV(l->l_qtop))) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
	if ((ioc->ioc_flag & IOC_MASK) == IOC_NONE) {
		/* if issued by user, check credentials */
		if (drv_priv(ioc->ioc_cr) != 0 && ioc->ioc_cr->cr_uid != tp->cred.cr_uid) {
			write_unlock_irqrestore(&ua_mux_lock, flags);
			mi_copy_done(q, mp, EPERM);
			return (0);
		}
	}

	if (!tp_trylock(tp, q)) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		return (-EDEADLK);
	}

	for (up = tp->up.list; up; up = up->tp.next) {
		/* TODO: issue upstream primitives as necessary.  We can release the ua_mux_lock
		   here and issue primitives, reacquiring it before unlinking the stream. */
		up_set_i_state(up, LMI_UNUSABLE);
		up_set_u_state(up, ASP_DOWN);
		up_tp_unlink(up);
	}

	tp_unlink(tp);		/* this also sets state appropriately */

	mi_detach(l->l_qtop, (caddr_t) tp);

	tp_unlock(tp);
	write_unlock_irqrestore(&ua_mux_lock, flags);

	tp_free_priv(tp);

	/* Should probably flush queues in case a Stream head is reattached. */
	flushq(RD(l->l_qtop), FLUSHALL);
	mi_copy_done(q, mp, 0);
	return (0);
}

/**
 * lm_i_plink: - perform I_PLINK operation
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * With I_PLINK operations, the lower SGP stream can be shared across other upper streams.  The
 * credentials of the linker are used to control access to the lower stream.  The lower stream
 * cannot be configured or unlinked without the privileges associated with the linking process.  Any
 * upper stream; however, can attach/bind to a lower stream that was permanently linked.
 *
 * Note that if the linking Stream is associated with as an SG, the linked Stream will also be
 * associated with that SG.  Otherwise, the linked Stream will simply be added to the list of lower
 * permanent links and it will have to be configured by MUX id.
 */
static noinline fastcall __unlikely int
lm_i_plink(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct T_capability_req *p;
	unsigned long flags;
	struct tp *tp = NULL;
	mblk_t *rp = NULL;
	int err;

	if (!(rp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		err = -ENOBUFS;
		goto error;
	}
	/* Sneaky trick.  If a non-zero minor device number was opened and on which the linking was 
	   performed, then the SGID is implied by the minor device number that was opened. */
	if (!(tp = tp_alloc_priv(l->l_qtop, l->l_index, ioc->ioc_cr, lm->as.config.ppa.sgid))) {
		err = ENOMEM;
		goto error;
	}

	write_lock_irqsave(&ua_mux_lock, flags);

	if (lm_ctrl != lm) {
		/* Only allow the master control Stream to do permanent links. */
		write_unlock_irqrestore(&ua_mux_lock, flags);
		err = EPERM;
		goto error;
	}

	if ((err = mi_open_link(&ua_links, (caddr_t) tp, NULL, 0, MODOPEN, ioc->ioc_cr))) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		goto error;
	}
	if (lm->tp.tp) {
		if (lm->tp.tp->lm != NULL) {
			/* Only one layer management Stream is permitted per SG.  If the SG has a
			   temporary layer manager, we cannot do permanent links against it. */
			mi_close_unlink(&ua_links, (caddr_t) tp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			err = EPERM;
			goto error;
		}
	}

	mi_attach(l->l_qtop, (caddr_t) tp);

	write_unlock_irqrestore(&ua_mux_lock, flags);

	mi_copy_done(q, mp, 0);

	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->PRIM_type = T_CAPABILITY_REQ;
	p->CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
	rp->b_wptr = rp->b_rptr + sizeof(*p);
	putnext(tp->wq, rp);	/* immediate capability request */

	return (0);
      error:
	if (rp)
		freeb(rp);
	if (tp)
		tp_free_priv(tp);
	if (err >= 0) {
		mi_copy_done(q, mp, err);
		return (0);
	}
	return (err);
}

/**
 * lm_i_punlink: - perform I_PUNLINK operation
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * STREAMS ensures that I_PUNLINK will only arrive for permanent links that were formed using this
 * driver.  All I_PUNLINK operations can be refused.  Only refuse to unlink when the unlinking
 * process does not have sufficient privilege.  The unlinking process must either be the super user
 * or the owner of the link.  All associated Users are addressed before the final unlinking.
 */
static noinline fastcall __unlikely int
lm_i_punlink(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	unsigned long flags;
	struct tp *tp;
	struct up *up;

	write_lock_irqsave(&ua_mux_lock, flags);

	if (!(tp = TP_PRIV(l->l_qtop))) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		mi_copy_done(q, mp, EINVAL);
		return (0);
	}
	/* Always issued by user, check credentials. Only the master control Stream is allowed to
	   create permanent links, however, to avoid difficults with hanging permanent links,
	   however, permit the owner of the link or the super user to undo permanent links. */
	if (lm != lm_ctrl && drv_priv(ioc->ioc_cr) != 0 && ioc->ioc_cr->cr_uid != tp->cred.cr_uid) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		mi_copy_done(q, mp, EPERM);
		return (0);
	}

	if (!tp_trylock(tp, q)) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		return (-EDEADLK);
	}

	for (up = tp->up.list; up; up = up->tp.next) {
		/* TODO: issue upstream primitives as necessary.  We can release the ua_mux_lock
		   here and issue primitives, reacquiring it before unlinking the stream. */
		up_set_i_state(up, LMI_UNUSABLE);
		up_set_u_state(up, ASP_DOWN);
		up_tp_unlink(up);
	}

	mi_detach(l->l_qtop, (caddr_t) tp);
	mi_close_unlink(&ua_links, (caddr_t) tp);

	tp_unlock(tp);
	write_unlock_irqrestore(&ua_mux_lock, flags);

	tp_free_priv(tp);

	/* Should probably flush queues in case a Stream head is reattached. */
	flushq(RD(l->l_qtop), FLUSHALL);
	mi_copy_done(q, mp, 0);
	return (0);
}

/*
 *  SCCP User INPUT-OUTPUT Control processing
 *  -------------------------------------------------------------------------
 */
static noinline fastcall __unlikely int
lm_i_ioctl(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int rtn;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_LINK):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> I_LINK");
		rtn = lm_i_link(lm, q, mp);
		break;
	case _IOC_NR(I_UNLINK):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> I_UNLINK");
		rtn = lm_i_unlink(lm, q, mp);
		break;
	case _IOC_NR(I_PLINK):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> I_PLINK");
		rtn = lm_i_plink(lm, q, mp);
		break;
	case _IOC_NR(I_PUNLINK):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> I_PUNLINK");
		rtn = lm_i_punlink(lm, q, mp);
		break;
	default:
		mi_copy_done(q, mp, EOPNOTSUPP);
		rtn = 0;
		break;
	}
	return (rtn);
}

static int
ua_size_opts(uint type, size_t header)
{
	switch (type) {
	case UA_AS_OBJ_TYPE_DF:
		return (header + sizeof(struct ua_opt_conf_df));
	case UA_AS_OBJ_TYPE_LM:
		return (header + sizeof(struct ua_opt_conf_lm));
	case UA_AS_OBJ_TYPE_UP:
		return (header + sizeof(struct ua_opt_conf_up));
	case UA_AS_OBJ_TYPE_AS:
		return (header + sizeof(struct ua_opt_conf_as));
	case UA_AS_OBJ_TYPE_SP:
		return (header + sizeof(struct ua_opt_conf_sp));
	case UA_AS_OBJ_TYPE_SG:
		return (header + sizeof(struct ua_opt_conf_sg));
	case UA_AS_OBJ_TYPE_GP:
		return (header + sizeof(struct ua_opt_conf_gp));
	case UA_AS_OBJ_TYPE_XP:
		return (header + sizeof(struct ua_opt_conf_xp));
	default:
		return (-1);
	}
}

static int
ua_size_conf(uint type, size_t header)
{
	switch (type) {
	case UA_AS_OBJ_TYPE_DF:
		return (header + sizeof(struct ua_conf_df));
	case UA_AS_OBJ_TYPE_LM:
		return (header + sizeof(struct ua_conf_lm));
	case UA_AS_OBJ_TYPE_UP:
		return (header + sizeof(struct ua_conf_up));
	case UA_AS_OBJ_TYPE_AS:
		return (header + sizeof(struct ua_conf_as));
	case UA_AS_OBJ_TYPE_SP:
		return (header + sizeof(struct ua_conf_sp));
	case UA_AS_OBJ_TYPE_SG:
		return (header + sizeof(struct ua_conf_sg));
	case UA_AS_OBJ_TYPE_GP:
		return (header + sizeof(struct ua_conf_gp));
	case UA_AS_OBJ_TYPE_XP:
		return (header + sizeof(struct ua_conf_xp));
	default:
		return (-1);
	}
}

static int
ua_size_statem(uint type, size_t header)
{
	switch (type) {
	case UA_AS_OBJ_TYPE_DF:
		return (header + sizeof(struct ua_statem_df));
	case UA_AS_OBJ_TYPE_LM:
		return (header + sizeof(struct ua_statem_lm));
	case UA_AS_OBJ_TYPE_UP:
		return (header + sizeof(struct ua_statem_up));
	case UA_AS_OBJ_TYPE_AS:
		return (header + sizeof(struct ua_statem_as));
	case UA_AS_OBJ_TYPE_SP:
		return (header + sizeof(struct ua_statem_sp));
	case UA_AS_OBJ_TYPE_SG:
		return (header + sizeof(struct ua_statem_sg));
	case UA_AS_OBJ_TYPE_GP:
		return (header + sizeof(struct ua_statem_gp));
	case UA_AS_OBJ_TYPE_XP:
		return (header + sizeof(struct ua_statem_xp));
	default:
		return (-1);
	}
}

static int
ua_size_stats(uint type, size_t header)
{
	switch (type) {
	case UA_AS_OBJ_TYPE_DF:
		return (header + sizeof(struct ua_stats_df));
	case UA_AS_OBJ_TYPE_LM:
		return (header + sizeof(struct ua_stats_lm));
	case UA_AS_OBJ_TYPE_UP:
		return (header + sizeof(struct ua_stats_up));
	case UA_AS_OBJ_TYPE_AS:
		return (header + sizeof(struct ua_stats_as));
	case UA_AS_OBJ_TYPE_SP:
		return (header + sizeof(struct ua_stats_sp));
	case UA_AS_OBJ_TYPE_SG:
		return (header + sizeof(struct ua_stats_sg));
	case UA_AS_OBJ_TYPE_GP:
		return (header + sizeof(struct ua_stats_gp));
	case UA_AS_OBJ_TYPE_XP:
		return (header + sizeof(struct ua_stats_xp));
	default:
		return (-1);
	}
}

static void *
find_object(struct lm *lm, uint type, uint id)
{
	switch (type) {
	case UA_AS_OBJ_TYPE_DF:
		return (void *) df_find(lm, id);
	case UA_AS_OBJ_TYPE_LM:
		return (void *) lm_find(lm, id);
	case UA_AS_OBJ_TYPE_UP:
		return (void *) up_find(lm, id);
	case UA_AS_OBJ_TYPE_AS:
		return (void *) as_find(lm, id);
	case UA_AS_OBJ_TYPE_SP:
		return (void *) sp_find(lm, id);
	case UA_AS_OBJ_TYPE_SG:
		return (void *) sg_find(lm, id);
	case UA_AS_OBJ_TYPE_GP:
		return (void *) gp_find(lm, id);
	case UA_AS_OBJ_TYPE_XP:
		return (void *) xp_find(lm, id);
	default:
		return (NULL);
	}
}

/**
 * ua_as_ioctl: - process M_IOCTL for UA-AS input-output control
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the input-output control operation.  Step 1 consists of copying in the
 * necessary prefix structure that identifies the object type and id being managed.
 */
static noinline fastcall __unlikely int
ua_as_ioctl(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(UA_AS_IOCGOPTIONS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGOPTIONS)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_option));
		break;
	case _IOC_NR(UA_AS_IOCSOPTIONS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCSOPTIONS)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_option));
		break;
	case _IOC_NR(UA_AS_IOCSCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCSCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_config));
		break;
	case _IOC_NR(UA_AS_IOCGCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_config));
		break;
	case _IOC_NR(UA_AS_IOCTCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCTCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_config));
		break;
	case _IOC_NR(UA_AS_IOCCCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_config));
		break;
	case _IOC_NR(UA_AS_IOCLCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCLCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_config));
		break;
	case _IOC_NR(UA_AS_IOCGSTATEM):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGSTATEM)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_statem));
		break;
	case _IOC_NR(UA_AS_IOCCMRESET):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCMRESET)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_statem));
		break;
	case _IOC_NR(UA_AS_IOCGSTATSP):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGSTATSP)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_stats));
		break;
	case _IOC_NR(UA_AS_IOCSSTATSP):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCSSTATSP)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_stats));
		break;
	case _IOC_NR(UA_AS_IOCGSTATS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGSTATS)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_stats));
		break;
	case _IOC_NR(UA_AS_IOCCSTATS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCSTATS)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_stats));
		break;
	case _IOC_NR(UA_AS_IOCGNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_notify));
		break;
	case _IOC_NR(UA_AS_IOCSNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCSNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_notify));
		break;
	case _IOC_NR(UA_AS_IOCCNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_notify));
		break;
	case _IOC_NR(UA_AS_IOCCMGMT):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCMGMT)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_mgmt));
		break;
	case _IOC_NR(UA_AS_IOCCPASS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCPASS)");
		mi_copyin(q, mp, NULL, sizeof(struct ua_pass));
		break;
	default:
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	}
	return (0);
}

/**
 * ua_as_copyin: - process M_IOCDATA message from copyin
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 *
 * This is step 2 of the UA-AS input-output control operation.  Step 2 consists of copying out for
 * GET operations, and processing an additional copy in operation for SET operations.
 */
static noinline fastcall __unlikely int
ua_as_copyin(struct lm *lm, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp;
	void *obj;
	int size;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(UA_AS_IOCGOPTIONS):
	{
		struct ua_option *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGOPTIONS)");
		if ((size = ua_size_opts(p->type, sizeof(*p))) < 0)
			break;
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			return (0);
		}
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			*(struct ua_opt_conf_df *) o = ((struct df *) obj)->options;
			break;
		case UA_AS_OBJ_TYPE_LM:
			*(struct ua_opt_conf_lm *) o = ((struct lm *) obj)->lm.options;
			break;
		case UA_AS_OBJ_TYPE_UP:
			*(struct ua_opt_conf_up *) o = ((struct up *) obj)->up.options;
			break;
		case UA_AS_OBJ_TYPE_AS:
			*(struct ua_opt_conf_as *) o = ((struct up *) obj)->as.options;
			break;
		case UA_AS_OBJ_TYPE_SP:
			*(struct ua_opt_conf_sp *) o = ((struct tp *) obj)->sp.options;
			break;
		case UA_AS_OBJ_TYPE_SG:
			*(struct ua_opt_conf_sg *) o = ((struct tp *) obj)->sg.options;
			break;
		case UA_AS_OBJ_TYPE_GP:
			*(struct ua_opt_conf_gp *) o = ((struct tp *) obj)->gp.options;
			break;
		case UA_AS_OBJ_TYPE_XP:
			*(struct ua_opt_conf_xp *) o = ((struct tp *) obj)->xp.options;
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copyout(q, mp);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCSOPTIONS):
	{
		struct ua_option *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSOPTIONS)");
		if ((size = ua_size_opts(p->type, sizeof(*p))) < 0)
			break;
		mi_copyin_n(q, mp, 0, size);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCSCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSCONFIG)");
		if ((size = ua_size_conf(p->type, sizeof(*p))) < 0)
			break;
		mi_copyin_n(q, mp, 0, size);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCGCONFIG):
	{
		struct ua_config *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGCONFIG)");
		if ((size = ua_size_conf(p->type, sizeof(*p))) < 0)
			break;
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			return (0);
		}
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			*(struct ua_conf_df *) o = ((struct df *) obj)->config;
			break;
		case UA_AS_OBJ_TYPE_LM:
			*(struct ua_conf_lm *) o = ((struct lm *) obj)->lm.config;
			break;
		case UA_AS_OBJ_TYPE_UP:
			*(struct ua_conf_up *) o = ((struct up *) obj)->up.config;
			break;
		case UA_AS_OBJ_TYPE_AS:
			*(struct ua_conf_as *) o = ((struct up *) obj)->as.config;
			break;
		case UA_AS_OBJ_TYPE_SP:
			*(struct ua_conf_sp *) o = ((struct tp *) obj)->sp.config;
			break;
		case UA_AS_OBJ_TYPE_SG:
			*(struct ua_conf_sg *) o = ((struct tp *) obj)->sg.config;
			break;
		case UA_AS_OBJ_TYPE_GP:
			*(struct ua_conf_gp *) o = ((struct tp *) obj)->gp.config;
			break;
		case UA_AS_OBJ_TYPE_XP:
			*(struct ua_conf_xp *) o = ((struct tp *) obj)->xp.config;
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copyout(q, mp);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCTCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCTCONFIG)");
		if ((size = ua_size_conf(p->type, sizeof(*p))) < 0)
			break;
		mi_copyin_n(q, mp, 0, size);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCCCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCCONFIG)");
		if ((size = ua_size_conf(p->type, sizeof(*p))) < 0)
			break;
		mi_copyin_n(q, mp, 0, size);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCLCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCLCONFIG)");
		/* not supported just yet */
		mi_copy_done(q, mp, EOPNOTSUPP);
		return (0);
	case _IOC_NR(UA_AS_IOCGSTATEM):
	{
		struct ua_statem *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGSTATEM)");
		if ((size = ua_size_statem(p->type, sizeof(*p))) < 0)
			break;
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			return (0);
		}
		o = (typeof(o)) bp->b_rptr;
		*o = *p;

		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			o->state = ((struct df *) obj)->state;
			*(struct ua_statem_df *) (o + 1) = ((struct df *) obj)->statem;
			break;
		case UA_AS_OBJ_TYPE_LM:
			o->state = ((struct lm *) obj)->lm.state;
			*(struct ua_statem_lm *) (o + 1) = ((struct lm *) obj)->lm.statem;
			break;
		case UA_AS_OBJ_TYPE_UP:
			o->state = ((struct up *) obj)->up.state;
			*(struct ua_statem_up *) (o + 1) = ((struct up *) obj)->up.statem;
			break;
		case UA_AS_OBJ_TYPE_AS:
			o->state = ((struct up *) obj)->as.state;
			*(struct ua_statem_as *) (o + 1) = ((struct up *) obj)->as.statem;
			break;
		case UA_AS_OBJ_TYPE_SP:
			o->state = ((struct tp *) obj)->sp.state;
			*(struct ua_statem_sp *) (o + 1) = ((struct tp *) obj)->sp.statem;
			break;
		case UA_AS_OBJ_TYPE_SG:
			o->state = ((struct tp *) obj)->sg.state;
			*(struct ua_statem_sg *) (o + 1) = ((struct tp *) obj)->sg.statem;
			break;
		case UA_AS_OBJ_TYPE_GP:
			o->state = ((struct tp *) obj)->gp.state;
			*(struct ua_statem_gp *) (o + 1) = ((struct tp *) obj)->gp.statem;
			break;
		case UA_AS_OBJ_TYPE_XP:
			o->state = ((struct tp *) obj)->xp.state;
			*(struct ua_statem_xp *) (o + 1) = ((struct tp *) obj)->xp.statem;
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copyout(q, mp);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCCMRESET):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCMRESET)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		return (0);
	case _IOC_NR(UA_AS_IOCGSTATSP):
	{
		struct ua_stats *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGSTATSP)");
		if ((size = ua_size_stats(p->type, sizeof(*p))) < 0)
			break;
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			return (0);
		}
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			*(struct ua_stats_df *) o = ((struct df *) obj)->statsp;
			break;
		case UA_AS_OBJ_TYPE_LM:
			*(struct ua_stats_lm *) o = ((struct lm *) obj)->lm.statsp;
			break;
		case UA_AS_OBJ_TYPE_UP:
			*(struct ua_stats_up *) o = ((struct up *) obj)->up.statsp;
			break;
		case UA_AS_OBJ_TYPE_AS:
			*(struct ua_stats_as *) o = ((struct up *) obj)->as.statsp;
			break;
		case UA_AS_OBJ_TYPE_SP:
			*(struct ua_stats_sp *) o = ((struct tp *) obj)->sp.statsp;
			break;
		case UA_AS_OBJ_TYPE_SG:
			*(struct ua_stats_sg *) o = ((struct tp *) obj)->sg.statsp;
			break;
		case UA_AS_OBJ_TYPE_GP:
			*(struct ua_stats_gp *) o = ((struct tp *) obj)->gp.statsp;
			break;
		case UA_AS_OBJ_TYPE_XP:
			*(struct ua_stats_xp *) o = ((struct tp *) obj)->xp.statsp;
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copyout(q, mp);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCSSTATSP):
	{
		struct ua_stats *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSSTATSP)");
		if ((size = ua_size_stats(p->type, sizeof(*p))) < 0)
			break;
		mi_copyin_n(q, mp, 0, size);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCGSTATS):
	{
		struct ua_stats *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGSTATS)");
		if ((size = ua_size_stats(p->type, sizeof(*p))) < 0)
			break;
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			return (0);
		}
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			*(struct ua_stats_df *) o = ((struct df *) obj)->stats;
			break;
		case UA_AS_OBJ_TYPE_LM:
			*(struct ua_stats_lm *) o = ((struct lm *) obj)->lm.stats;
			break;
		case UA_AS_OBJ_TYPE_UP:
			*(struct ua_stats_up *) o = ((struct up *) obj)->up.stats;
			break;
		case UA_AS_OBJ_TYPE_AS:
			*(struct ua_stats_as *) o = ((struct up *) obj)->as.stats;
			break;
		case UA_AS_OBJ_TYPE_SP:
			*(struct ua_stats_sp *) o = ((struct tp *) obj)->sp.stats;
			break;
		case UA_AS_OBJ_TYPE_SG:
			*(struct ua_stats_sg *) o = ((struct tp *) obj)->sg.stats;
			break;
		case UA_AS_OBJ_TYPE_GP:
			*(struct ua_stats_gp *) o = ((struct tp *) obj)->gp.stats;
			break;
		case UA_AS_OBJ_TYPE_XP:
			*(struct ua_stats_xp *) o = ((struct tp *) obj)->xp.stats;
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copyout(q, mp);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCCSTATS):
	{
		struct ua_stats *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCSTATS)");
		if ((size = ua_size_stats(p->type, sizeof(*p))) < 0)
			break;
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			return (0);
		}
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			*(struct ua_stats_df *) o = ((struct df *) obj)->stats;
			bzero(&((struct df *) obj)->stats, sizeof(((struct df *) obj)->stats));
			break;
		case UA_AS_OBJ_TYPE_LM:
			*(struct ua_stats_lm *) o = ((struct lm *) obj)->lm.stats;
			bzero(&((struct lm *) obj)->lm.stats,
			      sizeof(((struct lm *) obj)->lm.stats));
			break;
		case UA_AS_OBJ_TYPE_UP:
			*(struct ua_stats_up *) o = ((struct up *) obj)->up.stats;
			bzero(&((struct up *) obj)->up.stats,
			      sizeof(((struct up *) obj)->up.stats));
			break;
		case UA_AS_OBJ_TYPE_AS:
			*(struct ua_stats_as *) o = ((struct up *) obj)->as.stats;
			bzero(&((struct up *) obj)->as.stats,
			      sizeof(((struct up *) obj)->as.stats));
			break;
		case UA_AS_OBJ_TYPE_SP:
			*(struct ua_stats_sp *) o = ((struct tp *) obj)->sp.stats;
			bzero(&((struct tp *) obj)->sp.stats,
			      sizeof(((struct tp *) obj)->sp.stats));
			break;
		case UA_AS_OBJ_TYPE_SG:
			*(struct ua_stats_sg *) o = ((struct tp *) obj)->sg.stats;
			bzero(&((struct tp *) obj)->sg.stats,
			      sizeof(((struct tp *) obj)->sg.stats));
			break;
		case UA_AS_OBJ_TYPE_GP:
			*(struct ua_stats_gp *) o = ((struct tp *) obj)->gp.stats;
			bzero(&((struct tp *) obj)->gp.stats,
			      sizeof(((struct tp *) obj)->gp.stats));
			break;
		case UA_AS_OBJ_TYPE_XP:
			*(struct ua_stats_xp *) o = ((struct tp *) obj)->xp.stats;
			bzero(&((struct tp *) obj)->xp.stats,
			      sizeof(((struct tp *) obj)->xp.stats));
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copyout(q, mp);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCGNOTIFY):
	{
		struct ua_notify *o, *p = (typeof(p)) dp->b_rptr;
		int size;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGNOTIFY)");
		if ((size = size_ntfy_by_type(p->type, sizeof(*p))) < 0)
			break;
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			return (0);
		}
		o = (typeof(o)) bp->b_rptr;
		*o = *p;

		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			o->notify = ((struct df *) obj)->events;
			break;
		case UA_AS_OBJ_TYPE_LM:
			o->notify = ((struct lm *) obj)->lm.events;
			break;
		case UA_AS_OBJ_TYPE_UP:
			o->notify = ((struct up *) obj)->up.events;
			break;
		case UA_AS_OBJ_TYPE_AS:
			o->notify = ((struct up *) obj)->as.events;
			break;
		case UA_AS_OBJ_TYPE_SP:
			o->notify = ((struct tp *) obj)->sp.events;
			break;
		case UA_AS_OBJ_TYPE_SG:
			o->notify = ((struct tp *) obj)->sg.events;
			break;
		case UA_AS_OBJ_TYPE_GP:
			o->notify = ((struct tp *) obj)->gp.events;
			break;
		case UA_AS_OBJ_TYPE_XP:
			o->notify = ((struct tp *) obj)->xp.events;
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copyout(q, mp);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCSNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSNOTIFY)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		return (0);
	case _IOC_NR(UA_AS_IOCCNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCNOTIFY)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		return (0);
	case _IOC_NR(UA_AS_IOCCMGMT):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCMGMT)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		return (0);
	case _IOC_NR(UA_AS_IOCCPASS):
	{
		struct ua_pass *p = (typeof(p)) dp->b_rptr;
		size_t mlen = sizeof(*p);

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCPASS)");
		if (p->ctl_length)
			if (sizeof(*p) + p->ctl_length > mlen)
				mlen = sizeof(*p) + p->ctl_length;
		if (p->dat_length)
			if (sizeof(*p) + p->dat_length > mlen)
				mlen = sizeof(*p) + p->ctl_length + p->dat_length;
		mi_copyin_n(q, mp, 0, mlen);
		return (0);
	}
	default:
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	}
	return (0);
}

/**
 * ua_as_copyin2: - process M_IOCDATA from second copyin
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 *
 * This is step 3, and the final step, of the UA-AS input-output control SET operation.  Step 3
 * consists of processing the copied in data for a SET operation and completing the input-output
 * control.
 */
static noinline fastcall __unlikely int
ua_as_copyin2(struct lm *lm, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tp *tp;
	void *obj;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(UA_AS_IOCSOPTIONS):
	{
		struct ua_option *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSOPTIONS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			((struct df *) obj)->options = *(struct ua_opt_conf_df *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_LM:
			((struct lm *) obj)->lm.options = *(struct ua_opt_conf_lm *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_UP:
			((struct up *) obj)->up.options = *(struct ua_opt_conf_up *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_AS:
			((struct up *) obj)->as.options = *(struct ua_opt_conf_as *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_SP:
			((struct tp *) obj)->sp.options = *(struct ua_opt_conf_sp *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_SG:
			((struct tp *) obj)->sg.options = *(struct ua_opt_conf_sg *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_GP:
			((struct tp *) obj)->gp.options = *(struct ua_opt_conf_gp *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_XP:
			((struct tp *) obj)->xp.options = *(struct ua_opt_conf_xp *) (p + 1);
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copy_done(q, mp, 0);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCSCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			((struct df *) obj)->config = *(struct ua_conf_df *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_LM:
			((struct lm *) obj)->lm.config = *(struct ua_conf_lm *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_UP:
			((struct up *) obj)->up.config = *(struct ua_conf_up *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_AS:
			((struct up *) obj)->as.config = *(struct ua_conf_as *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_SP:
			((struct tp *) obj)->sp.config = *(struct ua_conf_sp *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_SG:
			((struct tp *) obj)->sg.config = *(struct ua_conf_sg *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_GP:
			((struct tp *) obj)->gp.config = *(struct ua_conf_gp *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_XP:
			((struct tp *) obj)->xp.config = *(struct ua_conf_xp *) (p + 1);
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copy_done(q, mp, 0);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCCCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			((struct df *) obj)->config = *(struct ua_conf_df *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_LM:
			((struct lm *) obj)->lm.config = *(struct ua_conf_lm *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_UP:
			((struct up *) obj)->up.config = *(struct ua_conf_up *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_AS:
			((struct up *) obj)->as.config = *(struct ua_conf_as *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_SP:
			((struct tp *) obj)->sp.config = *(struct ua_conf_sp *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_SG:
			((struct tp *) obj)->sg.config = *(struct ua_conf_sg *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_GP:
			((struct tp *) obj)->gp.config = *(struct ua_conf_gp *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_XP:
			((struct tp *) obj)->xp.config = *(struct ua_conf_xp *) (p + 1);
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copy_done(q, mp, 0);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCTCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCTCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		up_release(lm);
		mi_copy_done(q, mp, 0);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCSSTATSP):
	{
		struct ua_stats *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSSTATSP)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(obj = find_object(lm, p->type, p->id))) {
			up_release(lm);
			mi_copy_done(q, mp, ESRCH);
			return (0);
		}
		switch (p->type) {
		case UA_AS_OBJ_TYPE_DF:
			((struct df *) obj)->statsp = *(struct ua_stats_df *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_LM:
			((struct lm *) obj)->lm.statsp = *(struct ua_stats_lm *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_UP:
			((struct up *) obj)->up.statsp = *(struct ua_stats_up *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_AS:
			((struct up *) obj)->as.statsp = *(struct ua_stats_as *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_SP:
			((struct tp *) obj)->sp.statsp = *(struct ua_stats_sp *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_SG:
			((struct tp *) obj)->sg.statsp = *(struct ua_stats_sg *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_GP:
			((struct tp *) obj)->gp.statsp = *(struct ua_stats_gp *) (p + 1);
			break;
		case UA_AS_OBJ_TYPE_XP:
			((struct tp *) obj)->xp.statsp = *(struct ua_stats_xp *) (p + 1);
			break;
		default:
			up_release(lm);
			mi_copy_done(q, mp, EFAULT);
			return (0);
		}
		up_release(lm);
		mi_copy_done(q, mp, 0);
		return (0);
	}
	case _IOC_NR(UA_AS_IOCCPASS):
	{
		struct ua_pass *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCPASS)");
		if (p->ctl_length + p->dat_length == 0) {
			mi_copy_done(q, mp, 0);
			return (0);
		}
		mi_copy_done(q, mp, EOPNOTSUPP);
		return (0);
	}
	default:
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	}
	return (0);
}

/**
 * ua_as_copyout: - process M_IOCDATA from copyout
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 *
 * This is step3, and the final step, for the TRANSPARENT UA-AS input-output control GET operation.
 * Step 3 consists of acknowledging the input-output control oepration with a return value.
 */
static noinline fastcall __unlikely int
ua_as_copyout(struct lm *lm, queue_t *q, mblk_t *mp)
{
	mi_copyout(q, mp);
	return (0);
}

static noinline fastcall __unlikely int
sl_ioctl(struct up *up, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tp *tp;
	mblk_t *bp;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SL_IOCGOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGOPTIONS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_option), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct lmi_option *) bp->b_rptr = up->up.options;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SL_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSOPTIONS)");
		mi_copyin(q, mp, NULL, sizeof(struct lmi_option));
		break;
	case _IOC_NR(SL_IOCGCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_config), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sl_config *) bp->b_rptr = up->up.config;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SL_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct sl_config));
		break;
	case _IOC_NR(SL_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCTCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct sl_config));
		break;
	case _IOC_NR(SL_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCCCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct sl_config));
		break;
	case _IOC_NR(SL_IOCGSTATEM):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGSTATEM)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_statem), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sl_statem *) bp->b_rptr = up->up.statem;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SL_IOCCMRESET):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCCMRESET)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	case _IOC_NR(SL_IOCGSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGSTATSP)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_stats), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sl_stats *) bp->b_rptr = up->up.statsp;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SL_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSSTATSP)");
		mi_copyin(q, mp, NULL, sizeof(struct sl_stats));
		break;
	case _IOC_NR(SL_IOCGSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGSTATS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_stats), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sl_stats *) bp->b_rptr = up->up.stats;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SL_IOCCSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSSTATS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_stats), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sl_stats *) bp->b_rptr = up->up.stats;
		bzero(&up->up.stats, sizeof(up->up.stats));
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SL_IOCGNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGNOTIFY)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_notify), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct lmi_notify *) bp->b_rptr = up->up.events;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SL_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(struct lmi_notify));
		break;
	case _IOC_NR(SL_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCCNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(struct lmi_notify));
		break;
	}
	return (0);
}
static noinline fastcall __unlikely int
sl_copyin(struct up *up, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tp *tp;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SL_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCSOPTIONS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->up.options = *(struct lmi_option *) dp->b_rptr;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SL_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCSCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->up.config = *(struct sl_config *) dp->b_rptr;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SL_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCTCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		/* FIXME: actually test and lock configuration */
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SL_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCCCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->up.config = *(struct sl_config *) dp->b_rptr;
		/* FIXME: unlock configuration */
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SL_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCSSTATSP)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->up.statsp = *(struct sl_stats *) dp->b_rptr;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SL_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCSNOTIFY)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->up.events.events |= ((struct lmi_notify *) dp->b_rptr)->events;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SL_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCCNOTIFY)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->up.events.events &= ~((struct lmi_notify *) dp->b_rptr)->events;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	}
	return (0);
}
static noinline fastcall __unlikely int
sl_copyout(struct up *up, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}

static noinline fastcall __unlikely int
sdt_ioctl(struct up *up, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tp *tp;
	mblk_t *bp;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDT_IOCGOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGOPTIONS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_option), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct lmi_option *) bp->b_rptr = up->sdt.options;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDT_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSOPTIONS)");
		mi_copyin(q, mp, NULL, sizeof(struct lmi_option));
		break;
	case _IOC_NR(SDT_IOCGCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_config), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdt_config *) bp->b_rptr = up->sdt.config;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDT_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct sdt_config));
		break;
	case _IOC_NR(SDT_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCTCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct sdt_config));
		break;
	case _IOC_NR(SDT_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct sdt_config));
		break;
	case _IOC_NR(SDT_IOCGSTATEM):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGSTATEM)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_statem), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdt_statem *) bp->b_rptr = up->sdt.statem;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDT_IOCCMRESET):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCMRESET)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	case _IOC_NR(SDT_IOCGSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGSTATSP)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_stats), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdt_stats *) bp->b_rptr = up->sdt.statsp;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDT_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSSTATSP)");
		mi_copyin(q, mp, NULL, sizeof(struct sdt_stats));
		break;
	case _IOC_NR(SDT_IOCGSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGSTATS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_stats), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdt_stats *) bp->b_rptr = up->sdt.stats;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDT_IOCCSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCSTATS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_stats), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdt_stats *) bp->b_rptr = up->sdt.stats;
		bzero(&up->sdt.stats, sizeof(up->sdt.stats));
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDT_IOCGNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGNOTIFY)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_notify), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct lmi_notify *) bp->b_rptr = up->sdt.events;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDT_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(struct lmi_notify));
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(struct lmi_notify));
		break;
	case _IOC_NR(SDT_IOCCABORT):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCABORT)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	default:
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	}
	return (0);
}
static noinline fastcall __unlikely int
sdt_copyin(struct up *up, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tp *tp;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDT_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSOPTIONS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdt.options = *(struct lmi_option *) dp->b_rptr;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDT_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdt.config = *(struct sdt_config *) dp->b_rptr;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDT_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCTCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		/* FIXME: actually test and lock configuration */
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDT_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCCCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdt.config = *(struct sdt_config *) dp->b_rptr;
		/* FIXME: actually unlock configuration */
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDT_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSSTATSP)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdt.statsp = *(struct sdt_stats *) dp->b_rptr;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDT_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSNOTIFY)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdt.events.events |= ((struct lmi_notify *) dp->b_rptr)->events;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCCNOTIFY)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdt.events.events &= ~((struct lmi_notify *) dp->b_rptr)->events;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		break;
	}
	return (0);
}
static noinline fastcall __unlikely int
sdt_copyout(struct up *up, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}

static noinline fastcall __unlikely int
sdl_ioctl(struct up *up, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tp *tp;
	mblk_t *bp;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDL_IOCGOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGOPTIONS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_option), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct lmi_option *) bp->b_rptr = up->sdl.options;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDL_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSOPTIONS)");
		mi_copyin(q, mp, NULL, sizeof(struct lmi_option));
		break;
	case _IOC_NR(SDL_IOCGCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_config), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdl_config *) bp->b_rptr = up->sdl.config;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct sdl_config));
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCTCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct sdl_config));
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(struct sdl_config));
		break;
	case _IOC_NR(SDL_IOCGSTATEM):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGSTATEM)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_statem), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdl_statem *) bp->b_rptr = up->sdl.statem;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDL_IOCCMRESET):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCMRESET)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	case _IOC_NR(SDL_IOCGSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGSTATSP)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_stats), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdl_stats *) bp->b_rptr = up->sdl.statsp;
		up_release(lm);
		mi_copyout(q, mp);
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSSTATSP)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	case _IOC_NR(SDL_IOCGSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGSTATS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_stats), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdl_stats *) bp->b_rptr = up->sdl.stats;
		up_release(lm);
		break;
	case _IOC_NR(SDL_IOCCSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCSTATS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_stats), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct sdl_stats *) bp->b_rptr = up->sdl.stats;
		bzero(&up->sdl.stats, sizeof(up->sdl.stats));
		up_release(lm);
		break;
	case _IOC_NR(SDL_IOCGNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGNOTIFY)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct lmi_notify), false))) {
			up_release(lm);
			mi_copy_done(q, mp, ENOBUFS);
			break;
		}
		*(struct lmi_notify *) bp->b_rptr = up->sdl.events;
		up_release(lm);
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(lmi_notify));
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(lmi_notify));
		break;
	case _IOC_NR(SDL_IOCCDISCTX):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCDISCTX)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	case _IOC_NR(SDL_IOCCCONNTX):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCCONNTX)");
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	default:
		mi_copy_done(q, mp, EOPNOTSUPP);
		break;
	}
	return (0);
}
static noinline fastcall __unlikely int
sdl_copyin(struct up *up, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tp *tp;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDL_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSOPTIONS)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdl.options = *(struct lmi_option *) dp->b_rptr;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdl.config = *(struct sdl_config *) dp->b_rptr;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCTCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		/* FIXME: actually test and lock configuration */
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCCCONFIG)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdl.config = *(struct sdl_config *) dp->b_rptr;
		/* FIXME: unlock configuration */
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSSTATSP)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdl.statsp = *(struct sdl_stats *) dp->b_rptr;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSNOTIFY)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdl.events.events |= ((struct lmi_notify *) dp->b_rptr)->events;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCCNOTIFY)");
		if (!up_acquire(lm, q))
			return (-EDEADLK);
		up->sdl.events.events &= ~((struct lmi_notify *) dp->b_rptr)->events;
		up_release(lm);
		mi_copy_done(q, mp, 0);
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		break;
	}
	return (0);
}
static noinline fastcall __unlikely int
sdl_copyout(struct up *up, queue_t *q, mblk_t *mp)
{
	mi_copyout(q, mp);
	return (0);
}

/*
 *  User STREAMS Message processing
 *  -------------------------------------------------------------------------
 */

/**
 * up_w_data: - process upper write M_DATA/M_HPDATA message
 * @q: active queue (upper write queue)
 * @mp: the M_DATA/M_HPDATA message
 *
 * M_DATA is the preferred way of passing messages whenever a primitives is not required.
 *
 * What to do about messages that are too short or too long, or sent in the wrong state.  Probably
 * the best thing to do is to issue an M_ERROR message in response: that is to treat them as a fatal
 * error.
 */
static inline fastcall __hot_write int
up_w_data(queue_t *q, mblk_t *mp)
{
	struct up *up = UP_PRIV(q);
	int err;

	if (!up_acquire(up, q))
		return (-EDEADLK);

	switch (up_get_l_state(up)) {
	case DL_DATAXFER:
		if ((up->up.option.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
			err = as_send_maup_data1(up, q, NULL, mp);
		} else {
			if ((err = as_send_maup_data2(up, q, NULL, mp, *mp->b_rptr++)) != 0)
				mp->b_rptr--;
		}
		break;
	default:
		up_release(up);
		goto outstate;
	}
	up_release(up);
	return (err);
      outstate:
	strlog(up->mid, up->sid, 0, SL_ERROR, "M_DATA/M_HPDATA out of state");
	freemsg(mp);
	return (0);
}

/**
 * up_w_proto: - process upper write M_PROTO/M_PCPROTO message
 * @q: active queue (upper write queue)
 * @mp: the M_PROTO/M_PCPROTO message
 */
static noinline fastcall __unlikely int
up_w_proto(queue_t *q, mblk_t *mp)
{
	struct up *up = UP_PRIV(q);
	struct tp *tp;
	int old_i_state, old_u_state, old_n_state, old_a_state;
	uint32_t prim;
	int rtn = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t)) {
		strlog(up->mid, up->sid, 0, SL_ERROR, "-> primitive too short");
		freemsg(mp);
		return (0);
	}
	prim = *(uint32_t *) mp->b_rptr;

	read_lock(&ua_mux_lock);
	if (!(tp = up->tp.tp)) {
		read_unlock(&ua_mux_lock);
		/* have not been associated with SG yet */
		switch (prim) {
		case LMI_INFO_REQ:
			return lmi_info_req(up, NULL, q, mp);
		case LMI_ATTACH_REQ:
			return lmi_attach_req(up, NULL, q, mp);
		case LMI_OPTMGMT_REQ:
			return lmi_optmgmt_req(up, NULL, q, mp);
		default:
			return lmi_error_ack(up, q, mp, prim, LMI_OUTSTATE);
		}
	}
	if (!tp_trylock(tp, q)) {
		read_unlock(&ua_mux_lock);
		return (-EDEADLK);
	}
	read_unlock(&ua_mux_lock);

	old_i_state = up_get_i_state(up);
	old_u_state = up_get_u_state(up);
	old_n_state = tp_get_i_state(tp);
	old_a_state = tp_get_u_state(tp);

	switch (*(uint32_t *) mp->b_rptr) {
	case LMI_INFO_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_INFO_REQ");
		rtn = lmi_info_req(up, tp, q, mp);
		break;
	case LMI_ATTACH_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_ATTACH_REQ");
		rtn = lmi_attach_req(up, tp, q, mp);
		break;
	case LMI_DETACH_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_DETACH_REQ");
		rtn = lmi_detach_req(up, tp, q, mp);
		break;
	case LMI_ENABLE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_ENABLE_REQ");
		rtn = lmi_enable_req(up, tp, q, mp);
		break;
	case LMI_DISABLE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_DISABLE_REQ");
		rtn = lmi_disable_req(up, tp, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_OPTMGMT_REQ");
		rtn = lmi_optmgmt_req(up, tp, q, mp);
		break;
	case SL_PDU_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_PDU_REQ");
		rtn = sl_pdu_req(up, tp, q, mp);
		break;
	case SL_EMERGENCY_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_EMERGENCY_REQ");
		rtn = sl_emergency_req(up, tp, q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_EMERGENCY_CEASES_REQ");
		rtn = sl_emergency_ceases_req(up, tp, q, mp);
		break;
	case SL_START_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_START_REQ");
		rtn = sl_start_req(up, tp, q, mp);
		break;
	case SL_STOP_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_STOP_REQ");
		rtn = sl_stop_req(up, tp, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_RETRIEVE_BSNT_REQ");
		rtn = sl_retrieve_bsnt_req(up, tp, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_RETRIEVAL_REQUEST_AND_FSNC_REQ");
		rtn = sl_retreival_request_and_fsnc_req(up, tp, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CLEAR_BUFFERS_REQ");
		rtn = sl_clear_buffers_req(up, tp, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CLEAR_RTB_REQ");
		rtn = sl_clear_rtb_req(up, tp, q, mp);
		break;
	case SL_CONTINUE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CONTINUE_REQ");
		rtn = sl_continue_req(up, tp, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_LOCAL_PROCESSOR_OUTAGE_REQ");
		rtn = sl_local_processor_outage_req(up, tp, q, mp);
		break;
	case SL_RESUME_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_RESUME_REQ");
		rtn = sl_resume_req(up, tp, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CONGESTION_DISCARD_REQ");
		rtn = sl_congestion_discard_req(up, tp, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CONGESTION_ACCEPT_REQ");
		rtn = sl_congestion_accept_req(up, tp, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_NO_CONGESTION_REQ");
		rtn = sl_no_congestion_req(up, tp, q, mp);
		break;
	case SL_POWER_ON_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_POWER_ON_REQ");
		rtn = sl_power_on_req(up, tp, q, mp);
		break;
#if 0
	case SL_OPTMGMT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_OPTMGMT_REQ");
		rtn = sl_optmgmt_req(up, tp, q, mp);
		break;
	case SL_NOTIFY_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_NOTIFY_REQ");
		rtn = sl_notify_req(up, tp, q, mp);
		break;
#endif
	default:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_????_???");
		rtn = sl_other_req(up, tp, q, mp);
		break;
/* UA based management interface */
	case UA_T_ESTABLISH_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_T_ESTABLISH_REQ");
		rtn = ua_t_establish_req(up, q, mp);
		break;
	case UA_T_RELEASE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_T_RELEASE_REQ");
		rtn = ua_t_release_req(up, q, mp);
		break;
	case UA_T_STATUS_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_T_STATUS_REQ");
		rtn = ua_t_status_req(up, q, mp);
		break;
	case UA_ASP_STATUS_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASP_STATUS_REQ");
		rtn = ua_asp_status_req(up, q, mp);
		break;
	case UA_AS_STATUS_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_AS_STATUS_REQ");
		rtn = ua_as_status_req(up, q, mp);
		break;
	case UA_ASP_UP_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASP_UP_REQ");
		rtn = ua_asp_up_req(up, q, mp);
		break;
	case UA_ASP_DOWN_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASP_DOWN_REQ");
		rtn = ua_asp_down_req(up, q, mp);
		break;
	case UA_ASP_ACTIVE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASP_ACTIVE_REQ");
		rtn = ua_asp_active_req(up, q, mp);
		break;
	case UA_ASP_INACTIVE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASP_INACTIVE_REQ");
		rtn = ua_asp_inactive_req(up, q, mp);
		break;
	case UA_REG_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_REG_REQ");
		rtn = ua_reg_req(up, q, mp);
		break;
	case UA_DEREG_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_DEREG_REQ");
		rtn = ua_dereg_req(up, q, mp);
		break;
	case UA_T_PRIMITIVE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_T_PRIMITIVE_REQ");
		rtn = ua_t_primitive_req(up, q, mp);
		break;
	}
	if (rtn < 0) {
		up_set_i_state(up, old_i_state);
		up_set_u_state(up, old_u_state);
		tp_set_i_state(tp, old_n_state);
		tp_set_u_state(tp, old_a_state);
	}
	tp_unlock(tp);
	return (rtn);
}

/**
 * up_w_ctl: - process M_CTL/M_PCCTL message
 * @q: active queue (upper write queue)
 * @mp: the M_CTL/M_PCCTL message
 */
static noinline fastcall int
up_w_ctl(queue_t *q, mblk_t *mp)
{
	struct up *up = UP_PRIV(q);

	strlog(up->mid, up->sid, 0, SL_ERROR, "M_CTL/M_PCCTL message on upper write queue");
	freemsg(mp);
	return (0);
}

/**
 * up_w_ioctl: - process M_IOCTL message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 */
static noinline fastcall int
up_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct up *up = UP_PRIV(q);
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		return lm_i_ioctl(up, q, mp);
	case UA_AS_IOC_MAGIC:
		return ua_as_ioctl(up, q, mp);
#if 1
	case SL_IOC_MAGIC:
		return sl_ioctl(up, q, mp);
	case SDT_IOC_MAGIC:
		return sdt_ioctl(up, q, mp);
	case SDL_IOC_MAGIC:
		return sdl_ioctl(up, q, mp);
#endif
	}
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/**
 * up_w_iocdata: - process M_IOCDATA message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 */
static noinline fastcall int
up_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct up *up = UP_PRIV(q);
	struct sg *sg;
	mblk_t *dp;
	int err;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		return (0);
	case M_COPY_CASE(MI_COPY_IN, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case UA_AS_IOC_MAGIC:
			return ua_as_copyin(up, q, mp, dp);
#if 1
		case SL_IOC_MAGIC:
			return sl_copyin(up, q, mp, dp);
		case SDT_IOC_MAGIC:
			return sdt_copyin(up, q, mp, dp);
		case SDL_IOC_MAGIC:
			return sdl_copyin(up, q, mp, dp);
#endif
		}
		break;
	case M_COPY_CASE(MI_COPY_IN, 2):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case UA_AS_IOC_MAGIC:
			return ua_as_copyin2(up, q, mp, dp);
		}
		break;
	case M_COPY_CASE(MI_COPY_OUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case UA_AS_IOC_MAGIC:
			return ua_as_copyout(up, q, mp, dp);
#if 1
		case SL_IOC_MAGIC:
			return sl_copyout(up, q, mp, dp);
		case SDT_IOC_MAGIC:
			return sdt_copyout(up, q, mp, dp);
		case SDL_IOC_MAGIC:
			return sdl_copyout(up, q, mp, dp);
#endif
		}
		break;
	}
	mi_copy_done(q, mp, EPROTO);
	return (0);
}

/**
 * up_w_sig: - process upper write M_SIG/M_PCSIG message
 * @q: active queue (upper write queue)
 * @mp: the M_SIG/M_PCSIG message
 *
 * M_PCSIG and M_SIG messages are issued by mi_timer's.
 */
static noinline fastcall __unlikely int
up_w_sig(queue_t *q, mblk_t *mp)
{
	struct up *up = UP_PRIV(q);
	struct up_timer *t = (typeof(t)) mp->b_rptr;
	struct tp *tp;
	int rtn = 0;

	if (!(tp = up_tp_acquire(up, q)))
		return (-EDEADLK);

	if (likely(mi_timer_valid(mp))) {
		switch (t->timer) {
		case wack_aspac:
			strlog(up->mid, up->sid, UALOGTO, SL_TRACE, "-> WACK ASPAC TIMEOUT <-");
			rtn = up_wack_aspac_timeout(up, tp, q);
			break;
		case wack_aspia:
			strlog(up->mid, up->sid, UALOGTO, SL_TRACE, "-> WACK ASPIA TIMEOUT <-");
			rtn = up_wack_aspia_timeout(up, tp, q);
			break;
		case wack_hbeat:
			strlog(up->mid, up->sid, UALOGTO, SL_TRACE, "-> WACK HBEAT TIMEOUT <-");
			rtn = up_wack_hbeat_timeout(up, tp, q);
			break;
		default:
			strlog(up->mid, up->sid, 0, SL_ERROR, "-> invalid timer %d <-", t->timer);
			rtn = 0;
			break;
		}
	}

	up_tp_release(tp);
	return (rtn);
}

/**
 * up_w_flush: - process upper write M_FLUSH message
 * @q: active queue (upper write queue)
 * @mp: the M_FLUSH message
 *
 * Note that messages are never queued to the upper read side queue of the queue pair.  Therefore,
 * the upper read queue does not have to be flushed.
 */
static noinline fastcall __unlikely int
up_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * up_w_other: - process upper write other message
 * @q: active queue (upper write queue)
 * @mp: the other message
 */
static noinline fastcall __unlikely int
up_w_other(queue_t *q, mblk_t *mp)
{
	struct up *up = UP_PRIV(q);

	strlog(up->mid, up->sid, 0, SL_ERROR, "unhandled STREAMS message %d on upper write queue",
	       (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

/**
 * up_w_msg_slow: - process an User message
 * @q: active queue (upper write queue)
 * @mp: message to process
 */
static noinline fastcall int
up_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return up_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return up_w_proto(q, mp);
	case M_CTL:
	case M_PCCTL:
		return up_w_ctl(q, mp);
	case M_IOCTL:
		return up_w_ioctl(q, mp);
	case M_IOCDATA:
		return up_w_iocdata(q, mp);
	case M_SIG:
	case M_PCSIG:
		return up_w_sig(q, mp);
	case M_FLUSH:
		return up_w_flush(q, mp);
	default:
		return up_w_other(q, mp);
	}
}

static inline fastcall int
up_w_msg(queue_t *q, mblk_t *mp)
{
	/* FIXME: write fast path */
	return up_w_msg_slow(q, mp);
}

/*
 *  TS-Provider to TS-User primitives.
 *  -------------------------------------------------------------------------
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
		goto tooshort;
	switch (tp_get_i_state(tp)) {
	case TS_WRES_CIND:
	case TS_IDLE:
	case (uint) - 1:
		tp_set_i_state(tp, TS_WRES_CIND);
		break;
	default:
		goto outstate;
	}
	return t_conn_res(tp, q, mp, tp->acceptorid, 0, NULL, p->SEQ_number, NULL);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_IND out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
		goto tooshort;
	switch (tp_get_i_state(tp)) {
	case TS_WCON_CREQ:
	case (uint) - 1:
		break;
	default:
		goto outstate;
	}
	tp_set_i_state(tp, TS_DATA_XFER);
	if (tp->flags & UA_SG_CONFIGURED)
		return t_addr_req(tp, q, mp);
	if (!(tp->flags & UA_SG_LOCADDR))
		return t_addr_req(tp, q, mp);
	if (!(tp->flags & UA_SG_REMADDR)) {
		if (p->RES_length) {
			bcopy((caddr_t) p + p->RES_offset, &tp->xp.options.rem_add, p->RES_length);
			tp->xp.options.rem_len = p->RES_length;
			tp->flags |= UA_SG_REMADDR;
		} else {
			return t_addr_req(tp, q, mp);
		}
	}
	if (tp_get_u_state(tp) == ASP_DOWN) {
		tp_set_u_state(tp, ASP_WACK_ASPUP);
		return tp_send_asps_aspup_req(tp, q, mp,
					      tp->sp.options.aspid ? &tp->sp.options.aspid : NULL,
					      NULL, 0);
	}
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CONN_CON out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
	struct up *up;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (tp_get_i_state(tp)) {
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
	case TS_WCON_CREQ:
		break;
	default:
		goto outstate;
	}
	write_lock(&ua_mux_lock);
	for (up = tp->up.list; up; up = up->tp.next) {
		if (tp->sp.options.options.pvar != UA_VERSION_TS102141
		    && (tp->sg.options.options.popt & UA_SG_DYNAMIC)) {
			/* ETSI TS 102 141 does not permit dynamic configuration. */
			/* When the SG requires dynamic registration, when the association is lost,
			   we also lose the registration, meaning that the User must reattach. This
			   is probably not really what user are expecting, but then registration is
			   complicated for UAs anyway. */
			if ((err = lmi_error_ind(up, q, mp, LMI_DISC, LMI_UNATTACHED)))
				break;
		} else {
			/* When the SG requires static configuration, when the association is lost,
			   the configuration is not lost, meaning that the User merely needs to
			   reenable.  This is probably what users are expecting. */
			switch (up_get_i_state(up)) {
			case LMI_DETACH_PENDING:
			case LMI_ATTACH_PENDING:
			case LMI_UNATTACHED:
			default:
				strlog(up->mid, up->sid, 0, SL_ERROR,
				       "%s: unexpected up i state %u", __FUNCTION__,
				       up_get_i_state(up));
				/* fall through */
			case LMI_ENABLED:
			case LMI_ENABLE_PENDING:
			case LMI_DISABLE_PENDING:
				if ((err = lmi_error_ind(up, q, mp, LMI_DISC, LMI_DISABLED)))
					break;
				continue;
			case LMI_DISABLED:
				/* probably already disabled on a previous pass */
				continue;
			}
			break;
		}
		up_set_u_state(up, ASP_DOWN);
	}
	write_unlock(&ua_mux_lock);
	if (err)
		return (err);
	tp_set_i_state(tp, TS_IDLE);
	/* XXX: we should attempt to reconnect the association. */
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DISCON_IND out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
 * When we receive data on a Stream, we take the UA part and pass it for processing.  Note that we
 * should check the PPI to ensure that it is either the UA PPI or PPI zero.
 */
static noinline fastcall int
t_data_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_data_ind *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_DATA_IND out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
 * When we receive data on a Stream, we take the UA part and pass it for processing.  Note that we
 * should check the PPI to ensure that it is either the UA PPI or PPI zero.
 */
static noinline fastcall int
t_exdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (tp_get_i_state(tp) != TS_DATA_XFER)
		goto outstate;
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_EXDATA_IND out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_EXDATA_IND too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

static noinline fastcall int
t_info(struct tp *tp, queue_t *q, mblk_t *mp, struct T_info_ack *p)
{
	uint oldstate = tp_get_i_state(tp);

	tp->info.PRIM_type = T_INFO_ACK;
	tp->info.TSDU_size = p->TSDU_size;
	tp->info.ETSDU_size = p->ETSDU_size;
	tp->info.CDATA_size = p->CDATA_size;
	tp->info.DDATA_size = p->DDATA_size;
	tp->info.ADDR_size = p->ADDR_size;
	tp->info.OPT_size = p->OPT_size;
	tp->info.TIDU_size = p->TIDU_size;
	tp->info.SERV_type = p->SERV_type;
	tp->info.CURRENT_state = p->CURRENT_state;
	tp->info.PROVIDER_flag = p->PROVIDER_flag;
	if (oldstate != tp->info.CURRENT_state) {
		if (oldstate == (uint) - 1) {
			switch (tp->info.CURRENT_state) {
			case TS_UNBND:
				if (tp->flags & UA_SG_CONFIGURED)
					return t_bind_req(tp, q, mp, tp->xp.options.loc_len,
							  (caddr_t) &tp->xp.options.loc_add);
				break;
			case TS_IDLE:
				if (tp->flags & UA_SG_CONFIGURED) {
					struct {
						struct t_opthdr oh;
						t_uscalar_t val;
					} opts[2];

					opts[0].oh.len = sizeof(opts[0]);
					opts[0].oh.level = T_INET_SCTP;
					opts[0].oh.name = T_SCTP_OSTREAMS;
					opts[0].oh.status = T_NEGOTIATE;
					opts[0].val = tp->xp.options.ostreams;

					opts[1].oh.len = sizeof(opts[1]);
					opts[1].oh.level = T_INET_SCTP;
					opts[1].oh.name = T_SCTP_ISTREAMS;
					opts[1].oh.status = T_NEGOTIATE;
					opts[1].val = tp->xp.options.istreams;

					return t_conn_req(tp, q, mp, tp->xp.options.rem_len,
							  (caddr_t) &tp->xp.options.rem_add,
							  sizeof(opts), (caddr_t) &opts, NULL);
				}
				if (!(tp->flags & UA_SG_LOCADDR))
					return t_addr_req(tp, q, mp);
				break;
			case TS_DATA_XFER:
				if ((tp->flags & UA_SG_CONFIGURED)
				    && tp_get_u_state(tp) == ASP_DOWN)
					return tp_send_asps_aspup_req(tp, q, mp,
								      tp->sp.options.aspid ? &tp->
								      sp.options.aspid : NULL, NULL,
								      0);
				if ((tp->flags & (UA_SG_LOCADDR | UA_SG_REMADDR)) !=
				    (UA_SG_LOCADDR | UA_SG_REMADDR))
					return t_addr_req(tp, q, mp);
				break;
			case TS_WREQ_ORDREL:
				return t_ordrel_req(tp, q, mp, NULL);
			case TS_WACK_BREQ:
			case TS_WACK_UREQ:
			case TS_WACK_CREQ:
			case TS_WACK_CRES:
			case TS_WACK_DREQ6:
			case TS_WACK_DREQ7:
			case TS_WACK_DREQ9:
			case TS_WACK_DREQ10:
			case TS_WACK_DREQ11:
			case TS_WACK_OPTREQ:
			case TS_WIND_ORDREL:
				break;
			default:
				goto outstate;
			}
		} else
			goto outstate;
	}
      outstate:
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
 *
 * Whenever an SCTP transport stream is linked under the multiplexing driver, we set the interface
 * state to -1 an issue an immediate T_INFO_ACK.  When we receive a T_INFO_ACK and the state is set
 * to -1, after processing the T_INFO_ACK we may need to perform additional actions.  One action is
 * issuing a T_ADDR_REQ to obtain addresses if the stream is in any state other than TS_UNBND.
 */
static noinline fastcall int
t_info_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_info_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	return t_info(tp, q, mp, p);
#if 0
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_INFO_ACK out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
#endif
      tooshort:
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
 *
 * Acknowledges a successful bind.  Note that the address bound is in the response; however, because
 * the trasnport provider may have assigned an address and we want to retain specification of a
 * wildcard bind, we only use the address if there was no adddress configured.
 */
static noinline fastcall int
t_bind_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_bind_ack *p = (typeof(p)) mp->b_rptr;
	struct lm *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (tp_get_i_state(tp) != TS_WACK_BREQ)
		goto outstate;
	tp_set_i_state(tp, TS_IDLE);
	if ((lm = tp->sg.lm) || (lm = lm_ctrl))
		return ua_t_primitive_ind(lm, q, mp);
	if (tp->flags & (UA_SG_CONFIGURED | UA_SG_REMADDR)) {
		struct {
			struct t_opthdr oh;
			t_uscalar_t val;
		} opts[2];

		opts[0].oh.len = sizeof(opts[0]);
		opts[0].oh.level = T_INET_SCTP;
		opts[0].oh.name = T_SCTP_OSTREAMS;
		opts[0].oh.status = T_NEGOTIATE;
		opts[0].val = tp->xp.options.ostreams;

		opts[1].oh.len = sizeof(opts[1]);
		opts[1].oh.level = T_INET_SCTP;
		opts[1].oh.name = T_SCTP_ISTREAMS;
		opts[1].oh.status = T_NEGOTIATE;
		opts[1].val = tp->xp.options.istreams;

		return t_conn_req(tp, q, mp, (size_t) tp->xp.options.rem_len,
				  (caddr_t) &tp->xp.options.rem_add, sizeof(opts), (caddr_t) &opts,
				  NULL);
	}
	if ((tp->flags & (UA_SG_LOCADDR)) != (UA_SG_LOCADDR))
		return t_addr_req(tp, q, mp);
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_BIND_ACK out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
	struct lm *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (tp_get_i_state(tp)) {
	case TS_WACK_BREQ:
		tp_set_i_state(tp, TS_UNBND);
		break;
	case TS_WACK_UREQ:
		tp_set_i_state(tp, TS_IDLE);
		break;
	case TS_WACK_CREQ:
		tp_set_i_state(tp, TS_IDLE);
		break;
	case TS_WACK_CRES:
		tp_set_i_state(tp, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ6:
		tp_set_i_state(tp, TS_WCON_CREQ);
		break;
	case TS_WACK_DREQ7:
		tp_set_i_state(tp, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ9:
		tp_set_i_state(tp, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ10:
		tp_set_i_state(tp, TS_WIND_ORDREL);
		break;
	case TS_WACK_DREQ11:
		tp_set_i_state(tp, TS_WREQ_ORDREL);
		break;
	case TS_WACK_OPTREQ:
		tp_set_i_state(tp, TS_IDLE);
		break;
	default:
		goto outstate;
	}
	if ((lm = tp->sg.lm) || (lm = lm_ctrl))
		return ua_t_primitive_ind(lm, q, mp);
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ERROR_ACK out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
	struct lm *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if ((lm = tp->sg.lm) || (lm = lm_ctrl)) {
		switch (tp_get_i_state(tp)) {
		case TS_WACK_UREQ:
			tp_set_i_state(tp, TS_UNBND);
			break;
		case TS_WACK_CREQ:
			tp_set_i_state(tp, TS_WCON_CREQ);
			break;
		case TS_WACK_CRES:
			tp_set_i_state(tp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			tp_set_i_state(tp, TS_IDLE);
			break;
		default:
		case TS_WACK_BREQ:
		case TS_UNBND:
		case TS_DATA_XFER:
		case TS_WIND_ORDREL:
		case TS_WREQ_ORDREL:
		case TS_WCON_CREQ:
		case TS_DATA_XFER:
		case TS_IDLE:
			break;
		}
		return ua_t_primitive_ind(lm, q, mp);
	}
	switch (tp_get_i_state(tp)) {
	case TS_WACK_UREQ:
		tp_set_i_state(tp, TS_UNBND);
	case TS_UNBND:
		if (tp->flags & (UA_SG_CONFIGURED | UA_SG_LOCADDR))
			return t_bind_req(tp, q, mp, tp->xp.options.loc_len,
					  (caddr_t) &tp->xp.options.loc_add);
		break;
	case TS_WACK_CREQ:
		tp_set_i_state(tp, TS_WCON_CREQ);
	case TS_WCON_CREQ:
		break;
	case TS_WACK_CRES:
		tp_set_i_state(tp, TS_DATA_XFER);
	case TS_DATA_XFER:
		if (tp->flags & UA_SG_CONFIGURED) {
			if (tp_get_u_state(tp) == ASP_DOWN)
				return tp_send_asps_aspup_req(tp, q, mp,
							      tp->sp.options.aspid ? &tp->sp.
							      options.aspid : NULL, NULL, 0);
		}
		if ((tp->flags & (UA_SG_REMADDR | UA_SG_LOCADDR)) !=
		    (UA_SG_REMADDR | UA_SG_LOCADDR)) {
			return t_addr_req(tp, q, mp);
		}
		if (tp_get_u_state(tp) == ASP_DOWN)
			return tp_send_asps_aspup_req(tp, q, mp,
						      tp->sp.options.aspid ? &tp->sp.options.
						      aspid : NULL, NULL, 0);
		break;
	case TS_WACK_BREQ:
	case TS_WACK_OPTREQ:
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		tp_set_i_state(tp, TS_IDLE);
	case TS_IDLE:
		if (tp->flags & UA_SG_CONFIGURED) {
			struct {
				struct t_opthdr oh;
				t_uscalar_t val;
			} opts[2];

			opts[0].oh.len = sizeof(opts[0]);
			opts[0].oh.level = T_INET_SCTP;
			opts[0].oh.name = T_SCTP_OSTREAMS;
			opts[0].oh.status = T_NEGOTIATE;
			opts[0].val = tp->xp.options.ostreams;

			opts[1].oh.len = sizeof(opts[1]);
			opts[1].oh.level = T_INET_SCTP;
			opts[1].oh.name = T_SCTP_ISTREAMS;
			opts[1].oh.status = T_NEGOTIATE;
			opts[1].val = tp->xp.options.istreams;

			return t_conn_req(tp, q, mp, tp->xp.options.rem_len,
					  (caddr_t) &tp->xp.options.rem_add, sizeof(opts),
					  (caddr_t) &opts, NULL);
		}
		if ((tp->flags & (UA_SG_REMADDR | UA_SG_LOCADDR)) !=
		    (UA_SG_REMADDR | UA_SG_LOCADDR)) {
			return t_addr_req(tp, q, mp);
		}
		if (tp->flags & UA_SG_REMADDR) {
			struct {
				struct t_opthdr oh;
				t_uscalar_t val;
			} opts[2];

			opts[0].oh.len = sizeof(opts[0]);
			opts[0].oh.level = T_INET_SCTP;
			opts[0].oh.name = T_SCTP_OSTREAMS;
			opts[0].oh.status = T_NEGOTIATE;
			opts[0].val = tp->xp.options.ostreams;

			opts[1].oh.len = sizeof(opts[1]);
			opts[1].oh.level = T_INET_SCTP;
			opts[1].oh.name = T_SCTP_ISTREAMS;
			opts[1].oh.status = T_NEGOTIATE;
			opts[1].val = tp->xp.options.istreams;

			return t_conn_req(tp, q, mp, tp->xp.options.rem_len,
					  (caddr_t) &tp->xp.options.rem_add, sizeof(opts),
					  (caddr_t) &opts, NULL);
		}
		break;
	default:
		goto outstate;
	}
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OK_ACK out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (tp_get_i_state(tp)) {
	case TS_IDLE:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
		break;
	default:
		goto outstate;
	}
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UNITDATA_IND out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
	struct lm *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (tp_get_i_state(tp)) {
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	default:
		goto outstate;
	}
	if ((lm = tp->sg.sg->lm) || (lm = lm_ctrl))
		return ua_t_primitive_ind(lm, q, mp);
	/* ignore for now */
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UDERROR_IND out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
	struct lm *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (tp_get_i_state(tp)) {
	case TS_WACK_OPTREQ:
		tp_set_i_state(tp, TS_IDLE);
		break;
	}
	if ((lm = tp->sg.sg->lm) || (lm = lm_ctrl))
		return ua_t_primitive_ind(lm, q, mp);
	freemsg(mp);
	return (0);
#if 0
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTMGMT_ACK out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
#endif
      tooshort:
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
 *
 * Orderly releases as just as bad as abortive disconnects.  We need to do the same thing as is done
 * for an abortive disconnect and reponse immediately with an orderly release request.
 */
static noinline fastcall int
t_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_ind *p = (typeof(p)) mp->b_rptr;
	struct lm *lm;
	struct up *up;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (tp_get_i_state(tp)) {
	case TS_DATA_XFER:
		tp_set_i_state(tp, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		tp_set_i_state(tp, TS_IDLE);
		break;
	default:
		goto outstate;
	}
	for (up = tp->up.list; up; up = up->tp.next) {
		if (tp->sp.options.options.pvar != UA_VERSION_TS102141
		    && (tp->sg.options.options.popt & UA_SG_DYNAMIC)) {
			/* ETSI TS 102 141 does not permit dynamic configuration. */
			/* When the SG requires dynamic registration, when the association is lost,
			   we also lose the registration, meaning that the AS must reregister. This
			   is probably not really what Users are expecting, but then registration is 
			   complicated for UAs anyway. */
			/* FIXME: check all of the other AS for this User and determine if this
			   disconnects the User.  If it does, issue a primitive for all
			   destinations.  We really need a better indication. */
			if ((err = lmi_error_ind(up, q, mp, LMI_DISC, LMI_UNATTACHED)))
				return (err);
		} else {
			/* When the SG requires static configuration, when the association is lost,
			   the configuration is not lost, meaning that the AS merely needs to
			   reenable.  This is probably what Users are expecting. */
			/* FIXME: check all of the other AS for this User and determine if this
			   disconnects the User.  If it does, issue a primitive for all
			   destinations.  We really need a better indication. */
			switch (up_get_i_state(up)) {
			case LMI_DETACH_PENDING:
			case LMI_ATTACH_PENDING:
			case LMI_UNATTACHED:
			default:
				strlog(up->mid, up->sid, 0, SL_ERROR,
				       "%s: unexpected up i state %u", __FUNCTION__,
				       up_get_i_state(up));
				/* fall through */
			case LMI_ENABLED:
			case LMI_ENABLE_PENDING:
			case LMI_DISABLE_PENDING:
				if ((err = lmi_error_ind(up, q, mp, LMI_DISC, LMI_DISABLED)))
					return (err);
				continue;
			case LMI_DISABLED:
				/* probably already disabled on a previous pass */
				continue;
			}
			break;
		}
		up_set_u_state(up, ASP_DOWN);
	}
	/* FIXME: determine whether the tp going down affects the state of the sg. */
	tp_set_u_state(tp, ASP_DOWN);

	if ((lm = tp->lm) || (lm = lm_ctrl)) {
		/* If we have a layer manager, pass the message to the layer manager and let it
		   respond. */
		return ua_t_primitive_ind(lm, q, mp);
	} else {
		switch (tp_get_i_state(tp)) {
		case TS_WREQ_ORDREL:
			tp_set_i_state(tp, TS_IDLE);
			return t_ordrel_req(tp, q, mp, NULL);
		case TS_IDLE:
			freemsg(mp);
			return (0);
		default:
			/* never happens */
			goto outstate;
		}
	}
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ORDREL_IND out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
	mblk_t *dp = mp->b_cont;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (tp_get_i_state(tp)) {
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
		break;
	default:
		goto outstate;
	}
	if (tp->sp.options.options.pvar == UA_VERSION_TS102141) {
		/* Note that TS 102 141 says to check the PPI and if the PPI is other than 0 or 5
		   to discard the message.  We don't do that yet.  It will impact performance. */
	}
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTDATA_IND out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
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
 *
 * We often generate a T_ADDR_REQ primitive when the SCTP transport has first been linked under the
 * multiplexing driver and there is not yet any address configuration information for the transport.
 * This is to obtain and save local and remote addresses for later use in reestablishing an
 * association.
 */
static noinline fastcall int
t_addr_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_addr_ack *p = (typeof(p)) mp->b_rptr;
	struct lm *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (p->LOCADDR_length) {
		tp->flags |= UA_SG_LOCADDR;
		bcopy((caddr_t) p + p->LOCADDR_offset, &tp->xp.options.loc_add, p->LOCADDR_length);
		tp->xp.options.loc_len = p->LOCADDR_length;
	}
	if (p->REMADDR_length) {
		tp->flags |= UA_SG_REMADDR;
		bcopy((caddr_t) p + p->REMADDR_offset, &tp->xp.options.rem_add, p->REMADDR_length);
		tp->xp.options.rem_len = p->REMADDR_length;
	}
	if ((lm = tp->sg.sg->lm) || (lm = lm_ctrl)) {
		/* If there is a layer manager, pass the message to the layer manager. */
		return ua_t_primitive_ind(lm, q, mp);
	}
	/* The T_ADDR_REQ is often initiated as part of another sequence that needs to complete.
	   Check the state and complete the sequence if possible. */
	switch (tp_get_i_state(tp)) {
	case TS_UNBND:
		/* bind if possible */
		if (tp->flags & (UA_SG_CONFIGURED | UA_SG_LOCADDR)) {
			tp_set_i_state(tp, TS_WACK_BREQ);
			return t_bind_req(tp, q, mp, (size_t) tp->xp.options.loc_len,
					  (caddr_t) &tp->xp.options.loc_add);
		}
		break;
	case TS_WACK_BREQ:
	case TS_WACK_UREQ:
		break;
	case TS_IDLE:
		/* connect if possible */
		if (tp->flags & (UA_SG_CONFIGURED | UA_SG_REMADDR)) {
			struct {
				struct t_opthdr oh;
				t_uscalar_t val;
			} opts[2];

			opts[0].oh.len = sizeof(opts[0]);
			opts[0].oh.level = T_INET_SCTP;
			opts[0].oh.name = T_SCTP_OSTREAMS;
			opts[0].oh.status = T_NEGOTIATE;
			opts[0].val = tp->xp.options.ostreams;

			opts[1].oh.len = sizeof(opts[1]);
			opts[1].oh.level = T_INET_SCTP;
			opts[1].oh.name = T_SCTP_ISTREAMS;
			opts[1].oh.status = T_NEGOTIATE;
			opts[1].val = tp->xp.options.istreams;

			return t_conn_req(tp, q, mp, (size_t) tp->xp.options.rem_len,
					  (caddr_t) &tp->xp.options.rem_add, 0, NULL, NULL);
		}
		break;
	case TS_WACK_CREQ:
	case TS_WACK_CRES:
	case TS_WCON_CREQ:
		break;
	case TS_DATA_XFER:
		/* bring up ASP if necessary */
		switch (tp_get_u_state(tp)) {
		case ASP_DOWN:
			tp_set_u_state(tp, ASP_WACK_ASPUP);
			return tp_send_asps_aspup_req(tp, q, mp,
						      tp->sp.options.aspid ? &tp->sp.options.
						      aspid : NULL, NULL, 0);
		case ASP_WACK_ASPDN:
		case ASP_WACK_ASPUP:
		case ASP_UP:
			break;
		default:
			goto tooshort;
		}
		break;
	case TS_WACK_OPTREQ:
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		break;
	case TS_WIND_ORDREL:
		break;
	case TS_WREQ_ORDREL:
		/* request orderly release */
		return t_ordrel_req(tp, q, mp, NULL);
	}
	freemsg(mp);
	return (0);
#if 0
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ADDR_ACK out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
#endif
      tooshort:
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
		goto tooshort;
	if (p->CAP_bits1 & TC1_ACCEPTOR_ID)
		tp->acceptorid = p->ACCEPTOR_id;
	if (p->CAP_bits1 & TC1_INFO)
		return t_info(tp, q, mp, &p->INFO_ack);
	freemsg(mp);
	return (0);
#if 0
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CAPABILITY_ACK out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
#endif
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CAPABILITY_ACK too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/**
 * t_other_ind: - process other TPI primitive
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: primitive to process
 */
static noinline fastcall int
t_other_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	t_scalar_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	goto unexpected;
      unexpected:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "Unexpected primitive %d", *p);
	goto error;
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "unknown primitive too short");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  Transport User (SGP) Timeouts
 *  -------------------------------------------------------------------------
 */

static noinline fastcall int
tp_wack_aspup_timeout(struct tp *tp, queue_t *q)
{
	int err = 0;

	if (tp_get_u_state(tp) == ASP_WACK_ASPUP)
		if ((err = tp_send_asps_aspup_req(tp, q, NULL,
						  tp->sp.options.aspid ? &tp->sp.options.
						  aspid : NULL, NULL, 0)) == 0)
			if (tp->sp.options.tack) {
				strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
				       "-> WACK ASPUP START <- (%u msec)", tp->sp.options.tack);
				if (tp->wack_aspup)
					mi_timer(tp->wack_aspup, tp->sp.options.tack);
			}
	return (err);
}
static noinline fastcall int
tp_wack_aspdn_timeout(struct tp *tp, queue_t *q)
{
	int err = 0;

	if (tp_get_u_state(tp) == ASP_WACK_ASPDN)
		if ((err = tp_send_asps_aspdn_req(tp, q, NULL,
						  tp->sp.options.aspid ? &tp->sp.options.
						  aspid : NULL, NULL, 0)) == 0)
			if (tp->sp.options.tack) {
				strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
				       "-> WACK ASPDN START <- (%u msec)", tp->sp.options.tack);
				if (tp->wack_aspdn)
					mi_timer(tp->wack_aspdn, tp->sp.options.tack);
			}
	return (err);
}
static noinline fastcall int
tp_wack_aspac_timeout(struct tp *tp, queue_t *q)
{
	int err = 0;

#if 0
	if (tp_get_u_state(tp) == ASP_WACK_ASPAC)
		if ((err = tp_send_aspt_aspac_req(tp, q, NULL, NULL, 0)) == 0)
			if (tp->sp.options.tack) {
				strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
				       "-> WACK ASPAC START <- (%u msec)", tp->sp.options.tack);
				if (tp->wack_aspac)
					mi_timer(tp->wack_aspac, tp->sp.options.tack);
			}
#endif
	return (err);
}
static noinline fastcall int
tp_wack_aspia_timeout(struct tp *tp, queue_t *q)
{
	int err = 0;

#if 0
	if (tp_get_u_state(tp) == ASP_WACK_ASPIA)
		if ((err = tp_send_aspt_aspia_req(tp, q, NULL, NULL, 0)) == 0)
			if (tp->sp.options.tack) {
				strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
				       "-> WACK ASPIA START <- (%u msec)", tp->sp.options.tack);
				if (tp->wack_aspia)
					mi_timer(tp->wack_aspia, tp->sp.options.tack);
			}
#endif
	return (err);
}
static noinline fastcall int
tp_wack_hbeat_timeout(struct tp *tp, queue_t *q)
{
	int err = 0;

#if 0
	if (tp_get_u_state(tp) == ASP_WACK_HBEAT)
		if ((err = tp_send_asps_hbeat_req(tp, q, NULL, NULL, 0)) == 0)
			if (tp->sp.options.tack) {
				strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
				       "-> WACK HBEAT START <- (%u msec)", tp->sp.options.tack);
				if (tp->wack_hbeat)
					mi_timer(tp->wack_hbeat, tp->sp.options.tack);
			}
#endif
	return (err);
}

/*
 *  Transport Provider STREAMS Message processing
 *  -------------------------------------------------------------------------
 */

/**
 * tp_r_data: - process lower read M_DATA/M_HPDATA message
 * @q: active queue (lower read queue)
 * @mp: the M_DATA message
 */
static inline fastcall __hot_in int
tp_r_data(queue_t *q, mblk_t *mp)
{
	struct tp *tp;

	if ((tp = tp_acquire(q))) {
		strlog(tp->mid, tp->sid, 0, SL_ERROR, "M_DATA/M_HPDATA on lower read queue");
		tp_release(tp);
		freemsg(mp);
		return (0);
	}
	return (-EAGAIN);
}

/**
 * tp_r_proto: - process lower read M_PROTO/M_PCPROTO message
 * @q: active queue (lower read queue)
 * @mp: the M_PROTO/M_PCPROT message
 */
static noinline fastcall __unlikely int
tp_r_proto(queue_t *q, mblk_t *mp)
{
	struct tp *tp;
	uint old_i_state, old_u_state;
	int rtn;

	if (!(tp = tp_acquire(q)))
		return (-EDEADLK);

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t)) {
		strlog(tp->mid, tp->sid, 0, SL_ERROR, "primitive too short <-");
		tp_release(tp);
		freemsg(mp);
		return (0);
	}

	old_i_state = tp_get_i_state(tp);
	old_u_state = tp_get_u_state(tp);

	if (!pcmsg(DB_TYPE(mp)) && old_i_state == -1) {
		/* If we have not yet received a response to our T_CAPABILITY_REQ and this is not
		   the response, wait for the response (one _is_ coming).  Of course, if this is a
		   priority message, it must be processed immediately anyway. */
		tp_release(tp);
		return (-EAGAIN);
	}

	switch (*(uint32_t *) mp->b_rptr) {
	case T_CONN_IND:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_CONN_IND <-");
		rtn = t_conn_ind(tp, q, mp);
		break;
	case T_CONN_CON:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_CONN_CON <-");
		rtn = t_conn_con(tp, q, mp);
		break;
	case T_DISCON_IND:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_DISCON_IND <-");
		rtn = t_discon_ind(tp, q, mp);
		break;
	case T_DATA_IND:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_DATA_IND <-");
		rtn = t_data_ind(tp, q, mp);
		break;
	case T_EXDATA_IND:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_EXDATA_IND <-");
		rtn = t_exdata_ind(tp, q, mp);
		break;
	case T_INFO_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_INFO_ACK <-");
		rtn = t_info_ack(tp, q, mp);
		break;
	case T_BIND_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_BIND_ACK <-");
		rtn = t_bind_ack(tp, q, mp);
		break;
	case T_ERROR_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_ERROR_ACK <-");
		rtn = t_error_ack(tp, q, mp);
		break;
	case T_OK_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_OK_ACK <-");
		rtn = t_ok_ack(tp, q, mp);
		break;
	case T_UNITDATA_IND:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_UNITDATA_IND <-");
		rtn = t_unitdata_ind(tp, q, mp);
		break;
	case T_UDERROR_IND:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_UDERROR_IND <-");
		rtn = t_uderror_ind(tp, q, mp);
		break;
	case T_OPTMGMT_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_OPTMGMT_ACK <-");
		rtn = t_optmgmt_ack(tp, q, mp);
		break;
	case T_ORDREL_IND:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_ORDREL_IND <-");
		rtn = t_ordrel_ind(tp, q, mp);
		break;
	case T_OPTDATA_IND:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_OPTDATA_IND <-");
		rtn = t_optdata_ind(tp, q, mp);
		break;
	case T_ADDR_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_ADDR_ACK <-");
		rtn = t_addr_ack(tp, q, mp);
		break;
	case T_CAPABILITY_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_CAPABILITY_ACK <-");
		rtn = t_capability_ack(tp, q, mp);
		break;
	default:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "T_????_??? <-");
		rtn = t_other_ind(tp, q, mp);
		break;
	}
	if (rtn < 0) {
		tp_set_i_state(tp, old_i_state);
		tp_set_u_state(tp, old_u_state);
	}
	tp_release(tp);
	return (rtn);
}

/**
 * tp_r_sig: - process lower read M_SIG/M_PCSIG message
 * @q: active queue (lower read queue)
 * @mp: the M_SIG/M_PCSIG message
 */
static noinline fastcall __unlikely int
tp_r_sig(queue_t *q, mblk_t *mp)
{
	struct tp *tp;
	int rtn = 0;

	if (!(tp = tp_acquire(q)))
		return (-EDEADLK);

	if (likely(mi_timer_valid(mp))) {
		switch (*(int *) mp->b_rptr) {
		case wack_aspup:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK ASPUP TIMEOUT <-");
			rtn = tp_wack_aspup_timeout(tp, q);
			break;
		case wack_aspdn:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK ASPDN TIMEOUT <-");
			rtn = tp_wack_aspdn_timeout(tp, q);
			break;
		case wack_aspac:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK ASPAC TIMEOUT <-");
			rtn = tp_wack_aspac_timeout(tp, q);
			break;
		case wack_aspia:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK ASPIA TIMEOUT <-");
			rtn = tp_wack_aspia_timeout(tp, q);
			break;
		case wack_hbeat:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK HBEAT TIMEOUT <-");
			rtn = tp_wack_hbeat_timeout(tp, q);
			break;
		default:
			strlog(tp->mid, tp->sid, 0, SL_ERROR, "invalid timer %d",
			       *(int *) mp->b_rptr);
			rtn = 0;
			break;
		}
	}

	tp_release(tp);
	return (rtn);
}

/**
 * tp_r_ctl: - process lower read M_CTL/M_PCCTL message
 * @q: active queue (lower read queue)
 * @mp: the M_CTL/M_PCCTL message
 */
static noinline fastcall __unlikely int
tp_r_ctl(queue_t *q, mblk_t *mp)
{
	struct tp *tp;

	if ((tp = tp_acquire(q))) {
		strlog(tp->mid, tp->sid, 0, SL_ERROR, "M_CTL/M_PCCTL on lower read queue");
		tp_release(tp);
		freemsg(mp);
		return (0);
	}
	return (-EAGAIN);
}

/**
 * tp_r_error: - process lower read M_ERROR/M_HANGUP/M_UNHANGUP message
 * @q: active queue (lower read queue)
 * @mp: the M_ERROR/M_HANGUP/M_UNHANGUP message
 */
static noinline fastcall __unlikely int
tp_r_error(queue_t *q, mblk_t *mp)
{
	struct tp *tp;

	if ((tp = tp_acquire(q))) {
		strlog(tp->mid, tp->sid, 0, SL_ERROR,
		       "M_ERROR/M_HANGUP/M_UNHANGUP on lower read queue");
		tp_release(tp);
		freemsg(mp);
		return (0);
	}
	return (-EAGAIN);
}

/**
 * tp_r_iocack: - process lower read M_IOCACK/M_IOCNAK message
 * @q: active queue (lower read queue)
 * @mp: the M_IOCACK/M_IOCNAK message
 */
static noinline fastcall __unlikely int
tp_r_iocack(queue_t *q, mblk_t *mp)
{
	struct tp *tp;

	if ((tp = tp_acquire(q))) {
		strlog(tp->mid, tp->sid, 0, SL_ERROR, "M_IOCACK/M_IOCNAK on lower read queue");
		tp_release(tp);
		freemsg(mp);
		return (0);
	}
	return (-EAGAIN);
}

/**
 * tp_r_copy: - process M_COPYIN/M_COPYOUT message
 * @q: active queue (lower read queue)
 * @mp: the M_COPYIN/M_COPYOUT message
 */
static noinline fastcall __unlikely int
tp_r_copy(queue_t *q, mblk_t *mp)
{
	struct tp *tp;

	if ((tp = tp_acquire(q))) {
		struct copyresp *cp;

		strlog(tp->mid, tp->sid, 0, SL_ERROR, "M_COPYIN/M_COPYOUT on lower read queue");
		tp_release(tp);
		cp = (typeof(cp)) mp->b_rptr;
		cp->cp_rval = (caddr_t) 1;	/* indicate failure */
		qreply(q, mp);
		return (0);
	}
	return (-EAGAIN);
}

/**
 * tp_r_flush: - process lower read M_FLUSH message
 * @q: active queue (lower read queue)
 * @mp: the M_FLUSH message
 *
 * Note that we never queue messages on the lower write side of the lower queue pair.  Therefore, it
 * is not necessary to flush that queue.
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
	if (mp->b_rptr[0] & FLUSHW) {
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * tp_r_other: - process other STREAMS message
 * @q: active queue (lower read queue)
 * @mp: the other message
 */
static noinline fastcall __unlikely int
tp_r_other(queue_t *q, mblk_t *mp)
{
	struct tp *tp;

	if ((tp = tp_acquire(q))) {
		strlog(tp->mid, tp->sid, 0, SL_ERROR, "unknown STREAMS message %d",
		       (int) DB_TYPE(mp));
		tp_release(tp);
		freemsg(mp);
		return (0);
	}
	return (-EAGAIN);
}

/**
 * tp_r_msg_slow: - process an T-provider message
 * @q: active queue (lower read queue)
 * @mp: message to process
 */
static noinline fastcall int
tp_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return tp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tp_r_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tp_r_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
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

static inline fastcall int
tp_r_msg(queue_t *q, mblk_t *mp)
{
	/* FIXME: write fast path */
	return tp_r_msg_slow(q, mp);
}

/*
 *  PUT and SERVICE procedures.
 *  -------------------------------------------------------------------------
 */

/**
 * up_rsrv: - upper read service procedure
 * @q: queue to service (upper read queue)
 *
 * This service procedure is simply to perform backenabling across the multiplexing driver for flow
 * control.  When invoked, all lower read queues feeding this upper read queue have their service
 * procedures enabled.  Some locking is required to ensure that the lower Streams are not unlinked
 * and deallocated before the call to qenable(9).
 */
static streamscall __hot_read int
up_rsrv(queue_t *q)
{
	struct up *up = UP_PRIV(q);
	struct tp *tp;

	if ((tp = up_tp_acquire(up, q)) != NULL) {
		if (tp->rq)
			qenable(tp->rq);
		up_tp_release(tp);
	}
	return (0);
}

/**
 * up_rput: - upper read put procedure
 * @q: active queue (upper read queue)
 * @mp: message to put
 *
 * The upper read put procedure is not used.  It is a software error if this procedure is called.
 * Messages should be put with putnext(9) to the queue following this one.
 */
static streamscall int
up_rput(queue_t *q, mblk_t *mp)
{
	struct up *up = UP_PRIV(q);

	strlog(up->mid, up->sid, 0, SL_ERROR, "%s called in error", __FUNCTION__);
	putnext(q, mp);
	return (0);
}

/**
 * up_wsrv: - upper write service procedure
 * @q: queue to service (upper write queue)
 *
 * This is a canonical draining service procedure.
 */
static streamscall __hot_put int
up_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (up_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/**
 * up_wput: - upper write put procedure
 * @q: active queue (upper write queue)
 * @mp: message to put
 *
 * This is a canonical put procedure.  Messages are processed immediately if possible, otherwise,
 * they are queued.  Normal priority messages are queued if any messages are already queued.  The
 * processing procedure returns non-zero if the message is to be queued and returns zero if the
 * message has been processed.
 */
static streamscall __hot_write int
up_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || up_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * tp_rput: - lower read put procedure
 * @q: active queue (lower read queue)
 * @mp: message to put
 *
 * This is a canonical put procedure.  Messages are processed immediately if possible, otherwise,
 * they are queued.  Normal priority messages are queued if any messages are already queued.  The
 * processing procedure returns non-zero if the message is to be queued and returns zero if the
 * message has been processed.
 */
static streamscall __hot_in int
tp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tp_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * tp_rsrv: - lower read service procedure
 * @q: queue to service (lower read queue)
 *
 * This is a canonical draining service procedure.
 */
static streamscall __hot_read int
tp_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tp_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/**
 * tp_wput: - lower write put procedure
 * @q: active queue (lower write queue)
 * @mp: message to put
 *
 * The lower write put procedure is not used.  It is a software error if this procedure is called.
 * Messages should be put with putnext to the queue following this one.
 */
static streamscall int
tp_wput(queue_t *q, mblk_t *mp)
{
	struct tp *tp;

	if ((tp = tp_acquire(q))) {
		strlog(tp->mid, tp->sid, 0, SL_ERROR, "%s called in error", __FUNCTION__);
		tp_release(tp);
		putnext(q, mp);
		return (0);
	}
	return (-EAGAIN);
}

/**
 * tp_wsrv: - lower write service procedure
 * @q: queue to service (lower write queue)
 *
 * This service procedure is simply to perform backenabling across the multiplexing driver for flow
 * control.  When invoked, all upper write queues feeding this lower write queue have their service
 * procedures enabled.  Some locking is required to ensure that upper Streams are not closed and
 * deallocated before the call to qenable(9).
 */
static streamscall __hot_out int
tp_wsrv(queue_t *q)
{
	struct tp *tp;
	struct up *up;

	if ((tp = tp_acquire(q))) {
		for (up = tp->up.list; up; up = up->tp.next)
			if (up->wq)
				qenable(up->wq);
		tp_release(tp);
	}
	return (0);
}

/*
 *  OPEN and CLOSE routine.
 *  -------------------------------------------------------------------------
 */

#ifndef NUM_SG
#define NUM_SG 4096
#endif				/* NUM_SG */

/**
 * up_qopen: - STREAMS driver open routine
 * @q: newly created queue pair
 * @devp: pointer to device number associated with Stream
 * @oflags: flags to the open(2s) call
 * @sflag: STREAMS flag
 * @crp: pointer to the credentials of the opening process
 *
 * When a Stream is opened on the driver it corresponds to an AS associated with a given ASP-SG
 * relation.  The ASP-SG relation is determined from the minor device opened.  All minor devices
 * corresponding to ASP-SG relations are clone or auto-clone devices.  There may be several SCTP
 * lower Streams for each SG (one for each SGP associated with the SG).  If an SG structure has not
 * been allocated for the corresponding minor device number, we allocate one.  When an SCTP Stream
 * is I_LINK'ed under the driver, it is associated with the SG structure.  An UP and AS structure is
 * allocated and associated with each upper Stream.
 *
 * This driver cannot be pushed as a module.
 *
 * (cminor == 0) && (sflag == DRVOPEN)
 *	When minor device number 0 is opened with DRVOPEN (non-clone), a control Stream is opened.
 *	If a control Stream has already been opened, the open is refused.  The sflag is changed from
 *	DRVOPEN to CLONEOPEN and a new minor device number above NUM_SG is assigned.  This uses the
 *	autocloning features of Linux Fast-STREAMS.  This corresponds to the /dev/streams/m2ua-as/lm
 *	minor device node.
 *
 * (cminor == 0) && (sflag == CLONEOPEN)
 *	This is a normal clone open using the clone(4) driver.  A disassociated user Stream is
 *	opened.  A new unique minor device number above NUM_SG is assigned.  This corresponds to the
 *	/dev/streams/clone/m2ua-as clone device node.
 *
 * (1 <= cminor && cminor <= NUM_SG)
 *	This is a normal non-clone open.  Where the minor device number is between 1 and NUM_SG, an
 *	associated user Stream is opened.  If there is no SG structure to associate, one is created
 *	with default values.  A new minor device number above NUM_SG is assigned.  This uses the
 *	autocloning features of Linux Fast-STREAMS.  This corresponds to the
 *	/dev/streams/m2ua-as/NNNN minor device node where NNNN is the minor device number.
 */
static streamscall int
up_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	unsigned long flags;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct up *up;
	struct tp *tp;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || q->q_next)
		return (ENXIO);
	if (cminor > NUM_SG)
		return (ENXIO);
	if (!(up = up_alloc_priv(q, devp, crp)))
		return (ENOMEM);

	*devp = makedevice(cmajor, NUM_SG + 1);
	/* start assigning minors at NUM_SG + 1 */

	write_lock_irqsave(&ua_mux_lock, flags);

	if (cminor == 0) {
		/* When a zero minor device number was opened, the Stream is either a clone open or
		   an attempt to open the master control Stream.  The difference is whether the
		   sflag was DRVOPEN or CLONEOPEN. */
		if (sflag == DRVOPEN) {
			/* When the master control Stream is opened, another master control Stream
			   must not yet exist.  If this is the only master control Stream then it
			   is created. */
			if (lm_ctrl != NULL) {
				write_unlock_irqrestore(&ua_mux_lock, flags);
				up_free_priv(up);
				return (ENXIO);
			}
		}
		if ((err = mi_open_link(&ua_opens, (caddr_t) up, devp, oflags, CLONEOPEN, crp))) {
			write_unlock_irqrestore(&ua_mux_lock, flags);
			up_free_priv(up);
			return (err);
		}
		if (sflag == DRVOPEN)
			lm_ctrl = up;
		/* Both master control Streams and clone user Streams are disassociated with any
		   specific SG.  Master control Streams are never associated with a specific SG.
		   User Streams are associated with an SG using the sgid in the UA Address to the
		   attach/bind primitive, or when an SCTP Stream is temporarily linked under the
		   driver using the I_LINK input-output control. */
	} else {
		DECLARE_WAITQUEUE(wait, current);

		/* When a non-zero minor device number was opened, the Stream is automatically
		   associated with the SG to which the minor device number corresponds.  It cannot
		   be disassociated except when it is closed. */
		if (!(tp = sg_find(up, cminor))) {
			write_unlock_irqrestore(&ua_mux_lock, flags);
			up_free_priv(up);
			return (ENXIO);
		}
		/* Locking: need to wait until a lock on the SG structure can be acquired, or a
		   signal is received, or the SG structure is deallocated.  If the lock can be
		   acquired, associate the User Stream with the SG structure; in all other cases,
		   return an error.  Note that it is a likely event that the lock can be acquired
		   without waiting. */
		err = 0;
		add_wait_queue(&ua_waitq, &wait);
		spin_lock(&tp->lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (signal_pending(current)) {
				err = EINTR;
				spin_unlock(&tp->lock);
				break;
			}
			if (tp->users != 0) {
				spin_unlock(&tp->lock);
				write_unlock_irqrestore(&ua_mux_lock, flags);
				schedule();
				write_lock_irqsave(&ua_mux_lock, flags);
				if (!(tp = sg_find(up, cminor))) {
					err = ENXIO;
					break;
				}
				spin_lock(&tp->lock);
				continue;
			}
			err = mi_open_link(&ua_opens, (caddr_t) up, devp, oflags, CLONEOPEN, crp);
			if (err == 0)
				up_tp_link(up, tp);
			spin_unlock(&tp->lock);
			break;
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&ua_waitq, &wait);
		if (err) {
			write_unlock_irqrestore(&ua_mux_lock, flags);
			up_free_priv(up);
			return (err);
		}
	}

	/* just a few fixups after device number assigned */
	up->dev = *devp;
	up->mid = getmajor(*devp);
	up->sid = getminor(*devp);
	up->as.id = (uint) (*devp);

	write_unlock_irqrestore(&ua_mux_lock, flags);

	mi_attach(q, (caddr_t) up);
	qprocson(q);
	return (0);
}

/**
 * up_qclose: - STREAMS driver close routine
 * @q: queue pair
 * @oflags: flags to the open(2s) call
 * @crp: pointer to the credentials of the closing process
 */
static streamscall int
up_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct up *up = UP_PRIV(q);
	unsigned long flags;
	struct tp *tp;

	qprocsoff(q);
	mi_detach(q, (caddr_t) up);

	write_lock_irqsave(&ua_mux_lock, flags);

	if ((tp = up->tp.tp)) {
		DECLARE_WAITQUEUE(wait, current);

		/* Locking: need to wait until a SG lock can be acquired, or the SG structure is
		   deallocated.  If a lock can be acquired, the closing Stream is disassociated
		   with the SG; otherwise, if the SG structure is deallocated, there is no further
		   need to disassociate.  Note that it is a likely event that the lock can be
		   acquired without waiting. */
		add_wait_queue(&ua_waitq, &wait);
		spin_lock(&tp->lock);
		for (;;) {
			set_current_state(TASK_UNINTERRUPTIBLE);
			if (tp->users == 0) {
				up_tp_unlink(up);
				spin_unlock(&tp->lock);
				break;
			}
			spin_unlock(&tp->lock);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			schedule();
			write_lock_irqsave(&ua_mux_lock, flags);
			if ((tp = up->tp.tp) == NULL)
				break;
			spin_lock(&tp->lock);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&ua_waitq, &wait);
	}
	mi_close_unlink(&ua_opens, (caddr_t) up);

	write_unlock_irqrestore(&ua_mux_lock, flags);

	up_free_priv(up);	/* includes mi_close_free */
	return (0);
}

/*
 *  STREAMS Definitions.
 *  -------------------------------------------------------------------------
 */

static struct module_info up_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat up_rstat __attribute__ ((aligned(SMP_CACHE_BYTES)));
static struct module_stat up_wstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct module_info tp_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat tp_rstat __attribute__ ((aligned(SMP_CACHE_BYTES)));
static struct module_stat tp_wstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

static struct qinit up_rinit = {
	.qi_putp = up_rput,
	.qi_srvp = up_rsrv,
	.qi_qopen = up_qopen,
	.qi_qclose = up_qclose,
	.qi_minfo = &up_minfo,
	.qi_mstat = &up_rstat,
};

static struct qinit up_winit = {
	.qi_putp = up_wput,
	.qi_srvp = up_wsrv,
	.qi_minfo = &up_minfo,
	.qi_mstat = &up_wstat,
};

static struct qinit tp_rinit = {
	.qi_putp = tp_rput,
	.qi_srvp = tp_rsrv,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_rstat,
};

static struct qinit tp_winit = {
	.qi_putp = tp_wput,
	.qi_srvp = tp_wsrv,
	.qi_minfo = &tp_minfo,
	.qi_mstat = &tp_wstat,
};

static struct streamtab m2ua_asinfo = {
	.st_rdinit = &up_rinit,
	.st_wrinit = &up_winit,
	.st_muxrinit = &tp_rinit,
	.st_muxwinit = &tp_winit,
};

static struct cdevsw ua_cdev = {
	.d_name = DRV_NAME,
	.d_str = &m2ua_asinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the M2UA-AS driver. (0 for allocation.)");

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

	cmn_err(CE_NOTE, DRV_SPLASH);	/* console splash */
	if ((err = register_strdev(&ua_cdev, major)) < 0) {
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

	if ((err = unregister_strdev(&ua_cdev, major)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d\n", DRV_NAME, err);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(m2ua_asinit);
module_exit(m2ua_asexit);
