/*****************************************************************************

 @(#) $RCSfile: m3ua_as.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/02/03 03:07:46 $

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

 Last Modified $Date: 2007/02/03 03:07:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m3ua_as.c,v $
 Revision 0.9.2.3  2007/02/03 03:07:46  brian
 - working up drivers

 Revision 0.9.2.2  2007/01/23 10:00:46  brian
 - added test program and m2ua-as updates

 Revision 0.9.2.1  2006/10/17 11:55:42  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.9  2005/07/13 12:01:32  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 0.9.2.8  2005/07/05 22:45:32  brian
 - change for strcompat package

 Revision 0.9.2.7  2005/05/10 18:05:52  brian
 - do not set clone flag cause symbol no longer exported

 Revision 0.9.2.6  2005/04/09 09:42:20  brian
 - addition of module alias for ko modules

 Revision 0.9.2.5  2005/03/31 06:53:08  brian
 - changes for EL$ (CentOS 4.0) compatibility

 Revision 0.9.2.4  2005/03/08 19:30:03  brian
 - Changes for new build compile.

 Revision 0.9.2.3  2004/08/29 20:25:21  brian
 - Updates to driver registration for Linux Fast-STREAMS.

 Revision 0.9.2.2  2004/08/26 23:37:57  brian
 - Converted for use with Linux Fast-STREAMS.

 Revision 0.9.2.1  2004/08/21 10:14:44  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:20:21  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.3  2003/04/14 12:13:00  brian
 Updated module license defines.

 Revision 0.8.2.2  2003/04/03 19:50:31  brian
 Updates preparing for release.

 Revision 0.8.2.1  2002/10/18 03:27:42  brian
 Indentation changes only.

 Revision 0.8  2002/04/02 08:20:42  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/06/04 03:48:36  brian
 Added files for M3UA.

 *****************************************************************************/

#ident "@(#) $RCSfile: m3ua_as.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/02/03 03:07:46 $"

static char const ident[] =
    "$RCSfile: m3ua_as.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/02/03 03:07:46 $";

/*
 *  This is an M3UA multiplexing driver for the AS side of the ASP-SGP communications.  It works like
 *  this: the lower N minor device numbers represent N ASPs.  Opening any one of these N minor
 *  device numbers will clone a new Stream.  That Stream is associated with the same ASP for the
 *  entire time that it is open.  Each Stream opened on an ASP in this fashion represents an AS and
 *  provides the OpenSS7 Message Transfer Part Interface (MTPI) (which is based on
 *  NPI).  The Stream, when freshly opened, is merely associated with the ASP and specific M3UA-AS
 *  input-output controls can be used to configure and control the ASP.
 *
 *  The application or a configuration daemon is responsible for openeing ASP Streams and linking
 *  SCTP Streams beneath the ASP Stream.  These could be privde associations or public ones.
 *  Private associations are I_LINK'ed; public ones are I_PLINK'ed.  Input-output controls are used
 *  to describe the configuration elements associatedw with the SCTP association.  M3UA-AS
 *  input-output control commands cna be used to define the local and remote addresses and the
 *  driver can attempt to form an association on demand or under control stream invocation.  if the
 *  SCTP association is in the connected state, it can be used immediately and configuring local and
 *  remote addresses is perhaps not necesssary as the SCTP stream will be interrogated when it is
 *  linked.
 *
 *  It would be best to use a specialized configuraiton program (suaasconf(8)) or daemon to perform
 *  the opening and linking of SCTP Streams beneath the multiplexing driver because it is not a
 *  normal local management function that can be performed by the MTPI interface.
 *
 *  AS are defined with the Bind and Connect process.
 *
 *  Mapping of MTPI service primitives from the AS to the SCTP association is peformed internal to
 *  the ASP data structures.
 *
 *
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

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

#define M2UA	0
#define M3UA	1
#define SUA	0
#define TUA	0
#define ISUA	0

#if M2UA
#include <sys/dlpi.h>		/* For data link states. */
#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>
#endif

#if M3UA
#include <ss7/mtpi.h>
#include <sys/m3ua_lm.h>
#include <sys/m3ua_as_ioctl.h>
#endif

#if SUA
#include <sys/npi.h>
#include <sys/npi_sccp.h>
#include <sys/sua_lm.h>
#include <sys/sua_as_ioctl.h>
#endif

//#include <sys/ua_ioctl.h>
#include <sys/ua_as_lm.h>	/* UA-AS Layer Management interface */
#include <sys/ua_as_ioctl.h>	/* UA-AS input-output controls */

#include "ua_msg.h"		/* UA message definitions */

#define UALOGST	    1		/* loc UA state transitions */
#define UALOGTO	    2		/* loc UA timeouts */
#define UALOGRX	    3		/* loc UA primitives received */
#define UALOGTX	    4		/* loc UA primitives issued */
#define UALOGTE	    5		/* loc UA timer events */
#define UALOGDA	    6		/* loc UA data */

/* ============================== */

#define M3UA_AS_DESCRIP		"M3UA/SCTP AS MTP STREAMS MULTIPLEXING DRIVER."
#define M3UA_AS_REVISION	"OpenSS7 $RCSfile: m3ua_as.c,v $ $Name:  $ ($Revision: 0.9.2.3 $) $Date: 2007/02/03 03:07:46 $"
#define M3UA_AS_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define M3UA_AS_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define M3UA_AS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define M3UA_AS_LICENSE		"GPL"
#define UA_AS_BANNER		M3UA_AS_DESCRIP		"\n" \
				M3UA_AS_REVISION	"\n" \
				M3UA_AS_COPYRIGHT	"\n" \
				M3UA_AS_DEVICE		"\n" \
				M3UA_AS_CONTACT		"\n"
#define UA_AS_SPLASH		M3UA_AS_DESCRIP		" - " \
				M3UA_AS_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M3UA_CONTACT);
MODULE_DESCRIPTION(M3UA_DESCRIP);
MODULE_SUPPORTED_DEVICE(M3UA_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M3UA_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-m3ua_as");
MODULE_ALIAS("streams-m3ua-as");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define M3UA_AS_DRV_ID		CONFIG_STREAMS_M3UA_MODID
#define M3UA_AS_DRV_NAME	CONFIG_STREAMS_M3UA_NAME
#define M3UA_AS_CMAJORS		CONFIG_STREAMS_M3UA_NMAJORS
#define M3UA_AS_CMAJOR_0	CONFIG_STREAMS_M3UA_MAJOR
#define M3UA_AS_UNITS		CONFIG_STREAMS_M3UA_NMINORS
#endif				/* LFS */

#define DRV_ID		M3UA_AS_DRV_ID
#define DRV_NAME	M3UA_AS_DRV_NAME
#define DRV_CMAJOR	M3UA_AS_CMAJOR_0
#ifdef MODULE
#define DRV_SPLASH	UA_AS_BANNER
#else				/* MODULE */
#define DRV_SPLASH	UA_AS_SPLASH
#endif				/* MODULE */

struct df;				/* DF structure */
struct up;				/* upper multiplex structure (UP Stream) */
struct as;				/* AS Structure */
struct rp;				/* RP Structure */
struct sp;				/* SP Structure */
struct sg;				/* SG Structure */
struct gp;				/* GP Structure */
struct tp;				/* lower multiplex structure (TP Stream) */

struct ua_timer {
	int timer;			/* timer number */
	void *obj;			/* object */
};

struct ua_syncq {
	spinlock_t lock;		/* lock: stream lock */
	uint users;			/* lock: number of users */
	queue_t *waitq;			/* lock: queue waiting for lock */
};

struct bc {
	bcid_t rbid;			/* read bufcall id */
	bcid_t wbid;			/* write bufcall id */
};

struct up {
	struct bc bc;			/* must be first */
	struct ua_syncq sq;		/* streams synchronization queue */

	queue_t *rq;			/* RD queue */
	queue_t *wq;			/* WR queue */
	cred_t cred;
	union {
		dev_t dev;		/* device number */
		uint index;		/* multiplex index */
	};

	uint mid;			/* module id */
	uint sid;			/* stream id */
	uint spid;			/* unit opened */

	uint state;			/* AS state */
	uint l_state;			/* link state */

	struct {
		uint pending;
		uint operate;
	} status;

	uint32_t cong;			/* congestion onset */
	uint32_t disc;			/* congestion discard */

	struct {
		struct as *as;		/* AS that we are bound/attached to */
	} as;
	struct {
		struct up *next;	/* next UP associated with same SP */
		struct up **prev;	/* prev UP associated with same SP */
		struct sp *sp;		/* this UP controls or is served by this SP */
	} sp;
	struct {
		struct tp *list;	/* list of controlled (linked) XP objects */
	} xp;
	struct {
		uint id;
		uint flags;
		uint state;

		struct ua_opt_conf_lm options;
		struct ua_conf_lm config;
		struct ua_statem_lm statem;
		struct ua_stats_lm statsp;
		struct ua_stats_lm stats;
		struct lmi_notify events;
	} lm;
	struct {
		struct up *next;	/* list linkage */
		struct up **prev;	/* list linkage */
		uint id;
		uint flags;
		uint state;

#if M2UA
		lmi_info_ack_t info;

		struct lmi_option option;	/* SL protocol options */
		struct sl_config config;	/* SL configuration options */
		struct sl_statem statem;	/* SL state machine variables */
		struct sl_notify notify;	/* SL notification options */
		struct sl_stats stats;	/* SL statistics */
		struct sl_stats stamp;	/* SL statistics timestamps */
		struct sl_stats statsp;	/* SL statistics periods */
#else
		struct ua_opt_conf_up options;
		struct ua_conf_up config;
		struct ua_statem_up statem;
		struct ua_stats_up statsp;
		struct ua_stats_up stats;
		struct lmi_notify events;
#endif
	} up;
#if M2UA
	struct {
		struct sdt_timers timers;
		struct sdt_config config;
	} sdt;
	struct {
		struct sdl_timers timers;
		struct sdl_config config;
	} sdl;
#endif
};

struct as {
	struct {
		struct up *up;
	} up;
	struct {
		struct as *next;	/* next AS served by the same SP */
		struct as **prev;	/* prev AS served by the same SP */
		struct sp *sp;		/* this AS is served by this SP */
	} sp;
	struct {
		struct rp *list;	/* list of states for each GP */
		uchar count[8];		/* count of RP in each state */
	} rp;
	struct {
		struct as *next;	/* object linkage */
		struct as **prev;	/* object linkage */
		uint id;		/* object identifier */
		uint flags;		/* object flags */
		uint state;		/* object state */

		uint16_t streamid;	/* SCTP stream identifier for this AS */
		uint32_t request_id;	/* registration request id */

		struct ua_opt_conf_as options;
		struct ua_conf_as config;
#if M2UA
		struct m2ua_ppa addr;
#endif
#if M3UA
		struct m3ua_addr addr;
#endif
#if SUA
		struct sua_addr addr;
#endif
#if TUA
		struct tua_addr addr;
#endif
#if ISUA
		struct isua_addr addr;
#endif
		struct ua_statem_as statem;
		struct ua_stats_as statsp;
		struct ua_stats_as stats;
		struct lmi_notify events;
	} as;
};

/*
 * RP Structure.
 * --------------------------------------------------------------------------
 * The RP structure is not a managed object (and therefore has no object identifier or type).  It is
 * used for maintaining state mappings between AS and GP structures: that is, it maintains the state
 * of a specific AS for a specific SGP.  When an AS structure is created, one of these RP structures
 * is created for each existing GP.  When a GP structure is created, one of these structures is
 * created for each existing AS.
 */
struct rp {
	struct {
		struct rp *next;
		struct rp **prev;
		struct as *as;
	} as;
	struct {
		struct rp *next;
		struct rp **prev;
		struct gp *gp;
	} gp;
	struct {
		uint flags;		/* object flags */
		uint state;		/* object state */

		uint16_t streamid;	/* SCTP stream id to send data on */
		mblk_t *wack_aspac;	/* Timer waiting for ASPAC Ack */
		mblk_t *wack_aspia;	/* Timer waiting for ASPIA Ack */
		mblk_t *wack_hbeat;	/* Timer waiting for BEAT Ack */
	} rp;
};

/*
 * SP Structure.
 * --------------------------------------------------------------------------
 * An SP structure represents the local ASP.  I has a nubmer of AS that are served locally as well
 * as a number of SG structure representing remote SG.  This is the root structure for a
 * configuration and therefore has a lock associated with it.  Whenever a lock is acquired, the
 * corresponding SP structure is located with the MUX lock held and then the SP lock is acquired
 * (the MUX lock can then be released if the structure is not to be changed).
 */
struct sp {
	struct ua_syncq sq;		/* streams synchronization queue */

	struct {
		struct up *lm;		/* local management stream */
	} lm;
	struct {
		struct up *list;	/* list of UP loosely associated with this SP */
	} up;
	struct {
		struct as *list;	/* list of AS for this SP */
		ushort count[8];	/* count of AS in each state */
	} as;
	struct {
		struct sg *list;	/* list of SG for this SP (normally 0, 1, or 2) */
		uchar count[4];		/* count of SG in each state */

	} sg;
	struct {
		struct tp *list;	/* list of TP loosely associated with this SP */
	} xp;
	struct {
		struct sp *next;	/* object linkage */
		struct sp **prev;	/* object linkage */

		uint id;		/* object identifier */
		uint flags;		/* object flags */
		uint state;		/* object state */

		struct ua_opt_conf_sp options;
		struct ua_conf_sp config;
		struct ua_statem_sp statem;
		struct ua_stats_sp statsp;
		struct ua_stats_sp stats;
		struct lmi_notify events;
	} sp;
};

/*
 * SG Structure.
 * --------------------------------------------------------------------------
 * An SG structure represents a remote SG.  It is associated with a position serving an SP.
 * Normally there is only 1 or 2 SG structures per SP.  In the driver this is a separate structure
 * because there can be more than one SG per SP.
 */
struct sg {
	struct {
		struct sg *next;	/* next SG serving the same SP */
		struct sg **prev;	/* prev SG serving the same SP */
		struct sp *sp;		/* this SG's served SP */
		uint position;		/* this SG's (cardinal) position serving the SP */
	} sp;
	struct {
		struct gp *list;	/* list of SGP belonging to this SG */
		uchar count[4];		/* count of SGP in each state */
	} gp;
	struct {
		struct sg *next;	/* object linkage */
		struct sg **prev;	/* object linkage */

		uint id;		/* object identifier */
		uint flags;		/* object flags */
		uint state;		/* object state */

		struct ua_opt_conf_sg options;
		struct ua_conf_sg config;
		struct ua_statem_sg statem;
		struct ua_stats_sg statsp;
		struct ua_stats_sg stats;
		struct lmi_notify events;
	} sg;
};

/*
 * GP Structure.
 * --------------------------------------------------------------------------
 * A GP structure represents the SGP at the remote end of an ASP-SGP connection (SCTP association).
 * Each GP is associate with an SG structure that represents the SG to which the SGP at the remote
 * end of an SCTP association belongs.
 */
struct gp {
	struct {
		struct rp *list;	/* list of states for each AS */
		uchar count[8];		/* count of AS in each state */
	} rp;
	struct {
		struct gp *next;	/* next GP belonging to the same SG */
		struct gp **prev;	/* prev GP belonging to the same SG */
		struct sg *sg;		/* this GP's owning SG */
		uint position;		/* this GP's (cardinal) position within the SG */
	} sg;
	struct {
		struct tp *xp;		/* XP associated with this GP */
	} xp;
	struct {
		struct gp *next;	/* object linkage */
		struct gp **prev;	/* object linkage */
		uint id;		/* object identifier */
		uint flags;		/* object flags */
		uint state;		/* object state */

		uint16_t nextstream;	/* next stream to be allocated to an interface */
		mblk_t *wack_aspup;	/* Timer waiting for ASPUP Ack */
		mblk_t *wack_aspdn;	/* Timer waiting for ASPDN Ack */
		mblk_t *wack_hbeat;	/* Timer waiting for BEAT Ack */

		struct ua_opt_conf_gp options;
		struct ua_conf_gp config;
		struct ua_statem_gp statem;
		struct ua_stats_gp statsp;
		struct ua_stats_gp stats;
		struct lmi_notify events;
	} gp;
};

/*
 * XP Structure.
 * --------------------------------------------------------------------------
 * An XP structure represents an SCTP association.  It is either directly associated with a GP
 * structure.  XP structures can be anonymous (not assigned to either SG or GP), awaiting assignment
 * (assigned to SG but not specific GP), or assigned (assigned to a GP).
 */
struct tp {
	struct bc bc;			/* must be first */
	struct ua_syncq sq;		/* streams synchronization queue */

	queue_t *rq;			/* RD queue */
	queue_t *wq;			/* WR queue */
	cred_t cred;			/* credentials */
	union {
		dev_t dev;		/* device number */
		uint index;		/* multiplex index */
	};

	uint mid;			/* module id */
	uint sid;			/* stream id */
	uint spid;			/* default SP id */

	struct {
		struct tp *next;	/* next XP assigned to the same LM */
		struct tp **prev;	/* prev XP assigned to the same LM */
		struct up *lm;		/* this XP is assigned to this LM (null for master) */
	} lm;
	struct {
		struct tp *next;	/* next XP associated with same SP */
		struct tp **prev;	/* prev XP associated with same SP */
		struct sp *sp;		/* this XP associated with this SP */
	} sp;
	struct {
		struct gp *gp;		/* GP associated with this XP */
	} gp;

	struct {
		struct tp *next;	/* object linkage */
		struct tp **prev;	/* object linkage */
		uint id;		/* object identifier */
		uint flags;		/* object flags */
		uint state;		/* object state */

		uint32_t acceptorid;
		struct T_info_ack info;

		struct ua_opt_conf_xp options;
		struct ua_conf_xp config;
		struct ua_statem_xp statem;
		struct ua_stats_xp statsp;
		struct ua_stats_xp stats;
		struct lmi_notify events;
	} xp;
};

/* flags */
#define UA_CONFIGURED	0x01	/* SG has been configured by ioctl */
#define UA_LOCADDR	0x02	/* SG has local address determined */
#define UA_REMADDR	0x04	/* SG has remote address determined */

#ifndef M2UA_PPI
#define M2UA_PPI    5
#endif
#ifndef M3UA_PPI
#define M3UA_PPI    3
#endif
#ifndef SUA_PPI
#define SUA_PPI    4
#endif
#ifndef TUA_PPI
#define TUA_PPI    6
#endif

struct df {
	struct ua_opt_conf_df options;
	struct ua_conf_df config;
	struct ua_statem_df statem;
	struct ua_stats_df statsp;
	struct ua_stats_df stats;
	struct lmi_notify events;
	struct {
		struct up *list;
	} up;
	struct {
		struct as *list;
	} as;
	struct {
		struct sp *list;
	} sp;
	struct {
		struct sg *list;
	} sg;
	struct {
		struct gp *list;
	} gp;
	struct {
		struct xp *list;
	} xp;
} ua_defaults = {
	/* *INDENT-OFF* */
	{
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
	gp_wack_aspup = 0,
	gp_wack_aspdn,
	gp_wack_hbeat,
	rp_wack_aspac,
	rp_wack_aspia,
	rp_wack_hbeat,
};

/*
 *  State transitions.
 *  =========================================================================
 */

#define LMF_UNATTACHED		(1<<LMI_UNATTACHED)
#define LMF_ATTACH_PENDING	(1<<LMI_ATTACH_PENDING)
#define LMF_UNUSABLE		(1<<LMI_UNUSABLE)
#define LMF_DISABLED		(1<<LMI_DISABLED)
#define LMF_ENABLE_PENDING	(1<<LMI_ENABLE_PENDING)
#define LMF_ENABLED		(1<<LMI_ENABLED)
#define LMF_DISABLE_PENDING	(1<<LMI_DISABLE_PENDING)
#define LMF_DETACH_PENDING	(1<<LMI_DETACH_PENDING)

const char *
up_state_name(uint state)
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
up_set_state(struct up *up, uint newstate)
{
	uint oldstate = up->lm.state;

	if (oldstate != newstate)
		strlog(up->mid, up->sid, UALOGST, SL_TRACE, "%s <- %s", up_state_name(newstate),
		       up_state_name(oldstate));
	return ((up->lm.state = newstate));
}
static inline uint
up_get_state(struct up *up)
{
	return up->lm.state;
}
static inline uint
up_get_statef(struct up *up)
{
	return ((1 << up_get_state(up)));
}
static inline uint
up_chk_state(struct up *up, uint mask)
{
	return (up_get_statef(up) & mask);
}
static inline uint
up_not_state(struct up *up, uint mask)
{
	return (up_chk_state(up, ~mask));
}

const char *
as_state_name(uint state)
{
	switch (state) {
	case AS_DOWN:
		return ("AS_DOWN");
	case ASP_WACK_ASPUP:
		return ("AS_WRSP_RREQ");
	case ASP_WACK_ASPDN:
		return ("AS_WRSP_DREQ");
	case AS_INACTIVE:
		return ("AS_INACTIVE");
	case AS_WACK_ASPAC:
		return ("AS_WACK_ASPAC");
	case AS_WACK_ASPIA:
		return ("AS_WACK_ASPIA");
	case AS_ACTIVE:
		return ("AS_ACTIVE");
	default:
		return ("ASP_????");
	}
}
static int
as_set_state(struct as *as, queue_t *q, uint newstate)
{
	uint oldstate = as->as.state;
	int err = 0;

	if (newstate == oldstate)
		return (0);

	strlog(DRV_ID, as->sp.sp->sp.id, UALOGST, SL_TRACE, "%s <- %s",
	       as_state_name(newstate), as_state_name(oldstate));

	if (!(up = as->up.up)) {
		as->as.state = newstate;
		return (0);
	}

	switch (newstate) {
	case AS_ACTIVE:
		if ((err = m_unhangup(up, q, NULL)))
			return (err);
		switch (up_get_state(up)) {
		case LMI_DISABLED:
			break;
		case LMI_ENABLE_PENDING:
			if ((err = lmi_enable_con(up, q, NULL)))
				return (err);
			break;
		case LMI_DISABLE_PENDING:
			if ((err = lmi_error_ack(up, q, NULL, LMI_DISABLE_REQ, LMI_FAILURE)))
				return (err);
			break;
		case LMI_ENABLED:
			break;
		}
		break;
	case AS_WACK_ASPIA:
	case AS_WACK_ASPAC:
		/* wait for result */
		break;
	case AS_INACTIVE:
		switch (up_get_state(up)) {
		case LMI_DISABLED:
			break;
		case LMI_ENABLE_PENDING:
			if ((err = lmi_error_ack(up, q, NULL, LMI_ENABLE_REQ, LMI_FAILURE)))
				return (err);
			break;
		case LMI_DISABLE_PENDING:
			if ((err = lmi_disable_con(up, q, NULL)))
				return (err);
			break;
		case LMI_ENABLED:
			if ((err = m_hangup(up, q, NULL)))
				return (err);
			break;
		}
		break;
	case ASP_WACK_ASPDN:
	case ASP_WACK_ASPUP:
		/* wait for result */
		break;
	case AS_DOWN:
		switch (up_get_state(up)) {
		case LMI_DISABLED:
			break;
		case LMI_ENABLE_PENDING:
			if ((err = lmi_error_ack(up, q, NULL, LMI_ENABLE_REQ, LMI_FAILURE)))
				return (err);
			break;
		case LMI_DISABLE_PENDING:
			if ((err = lmi_disable_con(up, q, NULL)))
				return (err);
			break;
		case LMI_ENABLED:
			if ((err = m_hangup(up, q, NULL)))
				return (err);
			break;
		}
		break;
	}
	as->as.state = newstate;
	return (err);
}
static inline uint
as_get_state(struct as *as)
{
	return (as->as.state);
}
static inline uint
as_get_statef(struct as *as)
{
	return ((1 << as_get_state(as)));
}
static inline uint
as_chk_state(struct as *as, uint mask)
{
	return (as_get_statef(as) & mask);
}
static inline uint
as_not_state(struct as *as, uint mask)
{
	return (as_chk_state(as, ~mask));
}
static int
rp_set_state(struct rp *rp, queue_t *q, uint newstate)
{
	uint oldstate = rp->rp.state;
	int err = 0;

	if (newstate != oldstate) {
		struct as *as = rp->as.as;

		strlog(DRV_ID, as->sp.sp->sp.id, UALOGST, SL_TRACE, "%s <- %s",
		       as_state_name(newstate), as_state_name(oldstate));
		if (newstate >= as_get_state(as)) {
			if ((err = as_set_state(as, q, newstate)))
				return (err);
		} else {
			if (oldstate == as_get_state(as) && as->rp.count[oldstate] == 1) {
				switch (oldstate) {
				case AS_ACTIVE:
					if (as->rp.count[AS_WACK_ASPIA]
					    || newstate == AS_WACK_ASPIA) {
						if ((err = as_set_state(as, q, AS_WACK_ASPIA)))
							return (err);
						break;
					}
					/* fall through */
				case AS_WACK_ASPIA:
					if (as->rp.count[AS_WACK_ASPAC]
					    || newstate == AS_WACK_ASPAC) {
						if ((err = as_set_state(as, q, AS_WACK_ASPAC)))
							return (err);
						break;
					}
					/* fall through */
				case AS_WACK_ASPAC:
					if (as->rp.count[AS_INACTIVE]
					    || newstate == AS_INACTIVE) {
						if ((err = as_set_state(as, q, AS_INACTIVE)))
							return (err);
						break;
					}
					/* fall through */
				case AS_INACTIVE:
					if (as->rp.count[ASP_WACK_ASPDN]
					    || newstate == ASP_WACK_ASPDN) {
						if ((err = as_set_state(as, q, ASP_WACK_ASPDN)))
							return (err);
						break;
					}
					/* fall through */
				case ASP_WACK_ASPDN:
					if (as->rp.count[ASP_WACK_ASPUP]
					    || newstate == ASP_WACK_ASPUP) {
						if ((err = as_set_state(as, q, ASP_WACK_ASPUP)))
							return (err);
						break;
					}
					/* fall through */
				case ASP_WACK_ASPUP:
					if ((err = as_set_state(as, q, AS_DOWN)))
						return (err);
				case AS_DOWN:
					break;
				}
			}
		}
		rp->rp.state = newstate;
		as->rp.count[oldstate]--;
		as->rp.count[newstate]++;
	}
	return (err);
}
static inline uint
rp_get_state(struct rp *rp)
{
	return (rp->rp.state);
}
static inline uint
rp_get_statef(struct rp *rp)
{
	return ((1 << rp_get_state(rp)));
}
static inline uint
rp_chk_state(struct rp *rp, uint mask)
{
	return (rp_get_statef(rp) & mask);
}
static inline uint
rp_not_state(struct rp *rp, uint mask)
{
	return (rp_chk_state(rp, ~mask));
}

const char *
asp_state_name(uint state)
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
static int
sp_set_state(struct sp *sp, queue_t *q, uint newstate)
{
	uint oldstate = sp->sp.state;
	int err = 0;

	if (newstate != oldstate) {
		struct as *as;
		struct up *up;

		strlog(DRV_ID, sp->sp.id, UALOGST, SL_TRACE, "ASP: %s <- %s",
		       asp_state_name(newstate), asp_state_name(oldstate));
		switch (newstate) {
		case ASP_UP:
		case ASP_WACK_ASPUP:
		case ASP_WACK_ASPDN:
		case ASP_DOWN:
		}
		sp->sp.state = newstate;
	}
	return (err);
}
static inline uint
sp_get_state(struct sp *sp)
{
	return (sp->sp.state);
}
static inline uint
sp_get_statef(struct sp *sp)
{
	return ((1 << sp_get_state(sp)));
}
static inline uint
sp_chk_state(struct sp *sp, uint mask)
{
	return (sp_get_statef(sp) & mask);
}
static inline uint
sp_not_state(struct sp *sp, uint mask)
{
	return (sp_chk_state(sp, ~mask));
}

static int
sg_set_state(struct sg *sg, queue_t *q, uint newstate)
{
	uint oldstate = sg->sg.state;
	int err = 0;

	if (newstate != oldstate) {
		struct sp *sp = sg->sp.sp;

		strlog(DRV_ID, sg->sp.sp->sp.id, UALOGST, SL_TRACE, "SG: %s <- %s",
		       asp_state_name(newstate), asp_state_name(oldstate));
		if (newstate >= sp_get_state(sp)) {
			if ((err = sp_set_state(sp, q, newstate)))
				return (err);
		} else {
			if (oldstate == sp_get_state(sp) && sp->sg.count[oldstate] == 1) {
				switch (oldstate) {
				case ASP_UP:
					if (sp->sg.count[ASP_WACK_ASPDN]
					    || newstate == ASP_WACK_ASPDN) {
						if ((err = sp_set_state(sp, q, ASP_WACK_ASPDN)))
							return (err);
						break;
					}
					/* fall through */
				case ASP_WACK_ASPDN:
					if (sp->sg.count[ASP_WACK_ASPUP]
					    || newstate == ASP_WACK_ASPUP) {
						if ((err = sp_set_state(sp, q, ASP_WACK_ASPUP)))
							return (err);
						break;
					}
					/* fall through */
				case ASP_WACK_ASPUP:
					if ((err = sp_set_state(sp, q, ASP_DOWN)))
						return (err);
					break;
				case ASP_DOWN:
					break;
				}
			}
		}
		sg->sg.state = newstate;
		sp->sg.count[oldstate]--;
		sp->sg.count[newstate]++;
	}
	return (newstate);
}
static inline uint
sg_get_state(struct sg *sg)
{
	return (sg->sg.state);
}
static inline uint
sg_get_statef(struct sg *sg)
{
	return ((1 << sg_get_state(sg)));
}
static inline uint
sg_chk_state(struct sg *sg, uint mask)
{
	return (sg_get_statef(sg) & mask);
}
static inline uint
sg_not_state(struct sg *sg, uint mask)
{
	return (sg_chk_state(sg, ~mask));
}

static int
gp_set_state(struct gp *gp, queue_t *q, uint newstate)
{
	uint oldstate = gp->gp.state;
	int err = 0;

	if (newstate != oldstate) {
		struct sg *sg = gp->sg.sg;
		struct sp *sp = sg->sp.sp;
		bool dynamic =
		    ((sg->gp.options.options.popt & UA_DYNAMIC) &&
		     (sp->sp.options.options.pvar != UA_VERSION_TS102141));

		strlog(gp->tp.tp->mid, gp->tp.tp->sid, UALOGST, SL_TRACE, "SGP: %s <- %s",
		       asp_state_name(newstate), asp_state_name(oldstate));
		switch (newstate) {
		case ASP_DOWN:
			/* When the SGP goes ASP_DOWN, all AS down for this SGP. */
			for (rp = gp->rp.list; rp; rp = rp->gp.next)
				if ((err = rp_set_state(rp, q, dynamic ? AS_DOWN : AS_INACTIVE)))
					return (err);
			if (oldstate == ASP_UP)	/* try to bring ASP back up */
				if (gp->xp.xp && tp_get_state(gp->xp.xp) == TS_DATA_XFER)
					if ((err = gp_send_asps_aspup_req(gp, q, mp, NULL, 0)))
						return (err);
			break;
		case ASP_WACK_ASPUP:
		case ASP_WACK_ASPDN:
			break;
		case ASP_UP:
			/* When the SGP comes ASP_UP, check if there are keys that need to be
			   registered. */
			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				struct as *as = rp->as.as;
				struct up *up = as->up.up;

				if (dynamic) {
					if (rp_get_state(rp) == AS_DOWN)
						if ((err = rp_send_rkmm_reg_req(rp, q, NULL)))
							return (err);
				} else if (up && up_get_state(up) != LMI_DISABLED) {
					if (rp_get_state(rp) == AS_INACTIVE)
						if ((err = rp_send_aspt_aspac_req(rp, q, NULL)))
							return (err);
				}
			}
			break;
		}
		if (newstate >= sg_get_state(sg)) {
			if ((err = sg_set_state(sg, q, newstate)))
				return (err);
		} else {
			if (oldstate == sg_get_state(sg) && sg->gp.count[oldstate] == 1) {
				switch (oldstate) {
				case ASP_UP:
					if (sg->gp.count[ASP_WACK_ASPDN]
					    || newstate == ASP_WACK_ASPDN) {
						if ((err = sg_set_state(sg, q, ASP_WACK_ASPDN)))
							return (err);
						break;
					}
					/* fall through */
				case ASP_WACK_ASPDN:
					if (sg->gp.count[ASP_WACK_ASPUP]
					    || newstate == ASP_WACK_ASPUP) {
						if ((err = sg_set_state(sg, q, ASP_WACK_ASPUP)))
							return (err);
						break;
					}
					/* fall through */
				case ASP_WACK_ASPUP:
					if ((err = sg_set_state(sg, q, ASP_DOWN)))
						return (err);
					break;
				case ASP_DOWN:
					break;
				}
			}
		}
		gp->gp.state = newstate;
		sg->gp.count[oldstate]--;
		sg->gp.count[newstate]++;
	}
	return (err);
}
static inline uint
gp_get_state(struct gp *gp)
{
	return (gp->gp.state);
}
static inline uint
gp_get_statef(struct gp *gp)
{
	return ((1 << gp_get_state(gp)));
}
static inline uint
gp_chk_state(struct gp *gp, uint mask)
{
	return (gp_get_statef(gp) & mask);
}
static inline uint
gp_not_state(struct gp *gp, uint mask)
{
	return (gp_chk_state(gp, ~mask));
}

const char *
tp_state_name(uint state)
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
tp_set_state(struct tp *tp, uint newstate)
{
	uint oldstate = tp->xp.info.CURRENT_state;

	if (newstate != oldstate)
		strlog(tp->mid, tp->sid, UALOGST, SL_TRACE, "%s <- %s", tp_state_name(newstate),
		       tp_state_name(oldstate));
	return ((tp->xp.info.CURRENT_state = newstate));
}
static inline uint
tp_get_state(struct tp *tp)
{
	return (tp->xp.info.CURRENT_state);
}
static inline uint
tp_get_statef(struct tp *tp)
{
	return ((1 << tp_get_state(tp)));
}
static inline uint
tp_chk_state(struct tp *tp, uint mask)
{
	return (tp_get_statef(tp) & mask);
}
static inline uint
tp_not_state(struct tp *tp, uint mask)
{
	return (tp_chk_state(tp, ~mask));
}

#if M2UA
const char *
dl_state_name(uint state)
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
dl_set_state(struct up *up, uint newstate)
{
	uint oldstate = up->l_state;

	if (newstate != oldstate)
		strlog(up->mid, up->sid, UALOGST, SL_TRACE, "%s <- %s", dl_state_name(newstate),
		       dl_state_name(oldstate));
	return (up->l_state = newstate);
}
static inline uint
dl_get_state(struct up *up)
{
	return (up->l_state);
}
static inline uint
dl_get_statef(struct up *up)
{
	return ((1 << dl_get_state(up)));
}
static inline uint
dl_chk_state(struct up *up, uint mask)
{
	return (dl_get_statef(up) & mask);
}
static inline uint
dl_not_state(struct up *up, uint mask)
{
	return (dl_chk_state(up, ~mask));
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
sl_state_name(uint state)
{
	switch (state) {
	case (uint) - 1:
		return ("LS_UNINIT");
	default:
		return ("LS_???");
	}
}
static inline uint
sl_set_state(struct up *up, uint newstate)
{
	uint oldstate = up->up.state;

	if (newstate != oldstate)
		strlog(up->mid, up->sid, UALOGST, SL_TRACE, "%s <- %s", sl_state_name(newstate),
		       sl_state_name(oldstate));
	return (up->up.state = newstate);
}
static inline uint
sl_get_state(struct up *up)
{
	return (up->up.state);
}
static inline uint
sl_get_statef(struct up *up)
{
	return ((1 << sl_get_state(up)));
}
static inline uint
sl_chk_state(struct up *up, uint mask)
{
	return (sl_get_statef(up) & mask);
}
static inline uint
sl_not_state(struct up *up, uint mask)
{
	return (sl_chk_state(up, ~mask));
}
#endif

/*
 *  Locking
 *  =========================================================================
 */

static inline bool
ua_trylock(struct ua_syncq *sq, queue_t *q)
{
	unsigned long flags;
	queue_t *oldq == NULL;
	bool rtn;

	spin_lock_irqsave(&sq->lock, flags);
	if (sq->users == 0) {
		sq->users = 1;
		rtn = true;
	} else {
		oldq = XCHG(&sq->waitq, q);
		rtn = false;
	}
	spin_unlock_irqrestore(&sq->lock, flags);
	if (oldq)
		qenable(oldq);
	return (rtn);
}

static inline void
ua_unlock(struct ua_syncq *sq)
{
	unsigned long flags;
	queue_t *newq;

	spin_lock_irqsave(&sq->lock, flags);
	sq->users = 0;
	newq = XCHG(&sq->waitq, NULL);
	spin_unlock_irqrestore(&sq->lock, flags);
	if (newq)
		qenable(newq);
	if (waitqueue_active(&ua_waitq))
		wake_up_all(&ua_waitq);
	return;
}

static inline bool
up_trylock(struct up *up, queue_t *q)
{
	return ua_trylock(&up->sq, q);
}
static inline void
up_unlock(struct up *up)
{
	return ua_unlock(&up->sq);
}

/**
 * sp_trylock: - try to lock an SP object
 * @sp: the SP object to lock
 * @q: active queue
 */
static inline bool
sp_trylock(struct sp *sp, queue_t *q)
{
	return ua_trylock(&sp->sq, q);
}

/**
 * sp_unlock: - unlock an SP object
 * @sp: the SP object to unlock
 *
 * Two kinds of processes can wait on a lock.  A STREAMS put or service procedure can wait in which
 * case it adds its queue pointer to the waitq member.  A STREAMS open or close procedure can wait
 * in which case it adds its task to ua_waitq.  Waiting procedures or routines are expected to be
 * rare.
 */
static inline void
sp_unlock(struct sp *sp)
{
	return ua_unlock(&sp->sq);
}

static inline bool
tp_trylock(struct tp *tp, queue_t *q)
{
	return ua_trylock(&tp->sq, q);
}
static inline void
tp_unlock(struct tp *tp)
{
	return ua_unlock(&tp->sq);
}

/**
 * tp_acquire: - try to acquire a pointer to the TP structure from a TP queue procedure
 * @q: the lower queue
 *
 * Check the TP queue private pointer for NULL under mux read lock.  This is because unlinking lower
 * Streams can cause their private structures to be deallocated before their put and service
 * procedrues are replaced.  Then walk the structures to the SP.
 *
 * Lower multiplex private structures can disappear between I_UNLINK/I_PUNLINK M_IOCTL and setq(9)
 * back to the Stream head.
 */
static inline struct tp *
tp_acquire(queue_t *q)
{
	struct tp *tp;
	struct sp *sp;

	read_lock(&ua_mux_lock);
	if (!(tp = TP_PRIV(q)) || !tp_trylock(tp, q)) {
		tp = NULL;
	} else if ((sp = tp->sp.sp) && !sp_trylock(sp, q)) {
		tp_unlock(tp);
		tp = NULL;
	}
	read_unlock(&ua_mux_lock);
	return (tp);
}
static inline void
tp_release(struct tp *tp)
{
	struct sp *sp;

	if ((sp = tp->sp.sp))
		sp_unlock(sp);
	tp_unlock(tp);
}

static inline struct up *
up_acquire(queue_t *q)
{
	struct sp *sp;

	read_lock(&ua_mux_lock);
	if (!(up = UP_PRIV(q)) || !up_trylock(up, q)) {
		up = NULL;
	} else if ((sp = up->sp.sp) && !sp_trylock(sp, q)) {
		up_unlock(up);
		up = NULL;
	}
	read_unlock(&ua_mux_lock);
	return (up);
}
static inline void
up_release(struct up *up)
{
	struct sp *sp;

	if ((sp = up->sp.sp))
		sp_unlock(sp);
	up_unlock(up);
}

static inline struct up *
lm_acquire(queue_t *q)
{
	struct up *lm = UP_PRIV(q);

	if (!up_trylock(lm, q))
		return (NULL);
	return (lm);
}
static inline void
lm_release(struct up *lm)
{
	up_unlock(lm);
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

	if ((up->up.flags & UA_HUNGUP)) {
		freemsg(msg);
		return (0);
	}
	if ((mp = ua_allocb(q, 0, BPRI_MED))) {
		DB_TYPE(mp) = M_HANGUP;
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- M_HANGUP");
		up->up.flags |= UA_HUNGUP;
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

noinline fastcall int
m_unhangup(struct up *up, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;

	if (!(up->up.flags & UA_HUNGUP)) {
		freemsg(msg);
		return (0);
	}
	if ((mp = ua_allocb(q, 0, BPRI_MED))) {
		DB_TYPE(mp) = M_UNHANGUP;
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- M_HANGUP");
		up->up.flags &= ~UA_HUNGUP;
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

noinline fastcall int
m_error(struct up *up, queue_t *q, mblk_t *msg, uchar rerr, uchar werr)
{
	mblk_t *mp;

	if ((mp = ua_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = rerr;
		*mp->b_wptr++ = werr;
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- M_HANGUP");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  Structure Initialization and Termination.
 *  =========================================================================
 */

static struct up *
up_alloc_priv(queue_t *q, dev_t *devp, cred_t *crp, minor_t unit)
{
	struct up *up;
	int err = 0;

	if ((up = alloc_object_up(&err, *devp))) {
		up->rq = RD(q);
		up->wq = WR(q);
		up->cred = *crp;
		up->dev = *devp;
		up->mid = getmajor(*devp);
		up->sid = getminor(*devp);
		up->spid = unit;
	}
	return (up);
}

static void
up_sp_link(struct up *up, struct sp *sp)
{
	if ((up->sp.next = sp->up.list))
		up->sp.next->sp.prev = &up->sp.next;
	up->sp.prev = &(up->sp.sp = sp)->up.list;
}

static void
up_sp_unlink(struct up *up)
{
	if (up->sp.sp) {
		if ((*up->sp.prev = up->sp.next))
			up->sp.next->sp.prev = up->sp.prev;
		up->sp.next = NULL;
		up->sp.prev = &up->sp.next;
		up->sp.sp = NULL;
	}
}

static void
tp_sp_link(struct tp *tp, struct sp *sp)
{
	if ((tp->sp.next = sp->xp.list))
		tp->sp.next->sp.prev = &tp->sp.next;
	tp->sp.prev = &(tp->sp.sp = sp)->xp.list;
}
static void
tp_sp_unlink(struct tp *tp)
{
	if (tp->sp.sp) {
		if ((*tp->sp.prev = tp->sp.next))
			tp->sp.next->sp.prev = tp->sp.prev;
		tp->sp.next = NULL;
		tp->sp.prev = &tp->sp.next;
		tp->sp.sp = NULL;
	}
}

static void
tp_free_priv(struct tp *tp)
{
	free_object if (tp->bc.rbid)
		 unbufcall(xchg(&tp->bc.rbid, 0));

	if (tp->bc.wbid)
		unbufcall(xchg(&tp->bc.wbid, 0));
	mi_close_free((caddr_t) tp);
}
static struct tp *
tp_alloc_priv(queue_t *q, int index, cred_t *crp, minor_t unit)
{
	struct tp *tp;
	int err = 0;

	if ((tp = alloc_object_xp(&err, index))) {
		tp->rq = RD(q);
		tp->wq = WR(q);
		tp->cred = *crp;
		tp->index = index;
		tp->mid = q->q_qinfo->qi_minfo->mi_idnum;
		tp->sid = index;
		tp->spid = unit;
	}
	return (tp);
}

/*
 *  Lookups
 *  =========================================================================
 */
static struct df *
find_object_df(int *errp, struct up *lm, uint id, struct ua_conf_df *c)
{
	if (id != 0) {
		*errp = ENOENT;
		return (NULL);
	}
	return (&ua_defaults);
}
static struct up *
find_object_lm(int *errp, struct up *lm, uint id, struct ua_conf_lm *c)
{
	struct up *up = lm_ctrl;

	if (id != 0)
		for (up = (struct up *) mi_first_ptr(&ua_opens); up && up->lm.id != id;
		     up = (struct up *) mi_next_ptr((caddr_t) up)) ;
	if (!up)
		*errp = ENOENT;
	return (up);
}
static struct up *
find_object_up(int *errp, struct up *lm, uint id, struct ua_conf_up *c)
{
	if (id != 0)
		for (lm = (struct up *) mi_first_ptr(&ua_opens); lm && lm->lm.id != id;
		     lm = (struct up *) mi_next_ptr((caddr_t) lm)) ;
	if (!lm)
		*errp = ENOENT;
	return (lm);
}
static struct as *
find_object_as(int *errp, struct up *lm, uint id, struct ua_conf_as *c)
{
	struct as *as = lm->as.as;

	if (id != 0)
		for (as = ua_defaults.as.list; as && as->as.id != id; as = as->as.next) ;
	if (!as)
		*errp = ENOENT;
	return (as);
}
static struct sp *
find_object_sp(int *errp, struct up *lm, uint id, struct ua_conf_sp *c)
{
	struct sp *sp = lm->sp.sp;

	if (id != 0)
		for (sp = ua_defaults.sp.list; sp && sp->sp.id != id; sp = sp->sp.next) ;
	if (!sp)
		*errp = ENOENT;
	return (sp);
}
static struct sg *
find_object_sg(int *errp, struct up *lm, uint id, struct ua_conf_sg *c)
{
	struct sg *sg = (lm->sp.sp) ? lm->sp.sp->sg.list : NULL;

	if (id != 0)
		for (sg = ua_defaults.sg.list; sg && sg->sg.id != id; sg = sg->sg.next) ;
	if (!sg)
		*errp = ENOENT;
	return (sg);
}
static struct gp *
find_object_gp(int *errp, struct up *lm, uint id, struct ua_conf_gp *c)
{
	struct gp *gp = (lm->sp.sp && lm->sp.sp->sg.list) ? lm->sp.sp->sg.list->gp.list : NULL;

	if (id != 0)
		for (gp = ua_defaults.gp.list; gp && gp->gp.id != id; gp = gp->gp.next) ;
	if (!gp)
		*errp = ENOENT;
	return (gp);
}
static struct tp *
find_object_xp(int *errp, struct up *lm, uint id, struct ua_conf_xp *c)
{
	struct tp *tp = NULL;

	if (lm == lm_ctrl) {
		/* master control Stream searches permanent links only */
		for (tp = (struct tp *) mi_first_ptr(&ua_links); tp && tp->xp.id != id && id;
		     tp = (struct tp *) mi_next_ptr((caddr_t) tp)) ;
	} else {
		struct sp *sp;

		/* local control Stream searches temporary links only */
		if (((sp = lm->sp.sp) && sp->lm.lm == lm))
			for (tp = lm->xp.list; tp && tp->xp.id != id && id; tp = tp->lm.next) ;
	}
	if (!tp)
		*errp = ENOENT;
	return (tp);
}

static void *
find_object(int *errp, struct up *lm, struct ua_config *p)
{
	switch (p->type) {
	case UA_OBJ_TYPE_DF:
		return (void *) find_object_df(errp, lm, p->id, (struct ua_conf_df *) (p + 1));
	case UA_OBJ_TYPE_LM:
		return (void *) find_object_lm(errp, lm, p->id, (struct ua_conf_lm *) (p + 1));
	case UA_OBJ_TYPE_UP:
		return (void *) find_object_up(errp, lm, p->id, (struct ua_conf_up *) (p + 1));
	case UA_OBJ_TYPE_AS:
		return (void *) find_object_as(errp, lm, p->id, (struct ua_conf_as *) (p + 1));
	case UA_OBJ_TYPE_SP:
		return (void *) find_object_sp(errp, lm, p->id, (struct ua_conf_sp *) (p + 1));
	case UA_OBJ_TYPE_SG:
		return (void *) find_object_sg(errp, lm, p->id, (struct ua_conf_sg *) (p + 1));
	case UA_OBJ_TYPE_GP:
		return (void *) find_object_gp(errp, lm, p->id, (struct ua_conf_gp *) (p + 1));
	case UA_OBJ_TYPE_XP:
		return (void *) find_object_xp(errp, lm, p->id, (struct ua_conf_xp *) (p + 1));
	default:
		return (NULL);
	}
}

#define UA_OBJ_TYPE_RP	(UA_OBJ_TYPE_XP + 1)

static void __unlikely
free_object_df(struct df *df)
{
}
static void __unlikely
free_object_lm(struct up *lm)
{
}
static void __unlikely
free_object_up(struct up *up)
{
	bcid_t b;

	if ((b = XCHG(&up->bc.rbid, 0)))
		unbufcall(b);
	if ((b = XCHG(&up->bc.wbid, 0)))
		unbufcall(b);
	if (up->sq.users)
		up_unlock(up);

	mi_close_free((caddr_t) up);
}
static void __unlikely
free_object_as(struct as *as)
{
	return kmem_free(obj, sizeof(struct as));
}
static void __unlikely
free_object_sp(struct sp *sp)
{
	return kmem_free(obj, sizeof(struct sp));
}
static void __unlikely
free_object_sg(struct sg *sg)
{
	return kmem_free(obj, sizeof(struct sg));
}
static void __unlikely
free_object_gp(struct gp *gp)
{
	return kmem_free(obj, sizeof(struct gp));
}
static void __unlikely
free_object_xp(struct tp *xp)
{
	int b;

	if ((b = XCHG(&tp->bc.rbid, 0)))
		unbufcall(b);
	if ((b = XCHG(&tp->bc.wbid, 0)))
		unbufcall(b);
	if (tp->sq.users)
		tp_unlock(tp);

	mi_close_free((caddr_t) tp);
}
static void __unlikely
free_object_rp(struct rp *rp)
{
	return kmem_free(obj, sizeof(struct rp));
}

static void __unlikely
free_object(uint type, void *obj)
{
	switch (type) {
	case UA_OBJ_TYPE_DF:
		return free_object_df(obj);
	case UA_OBJ_TYPE_LM:
		return free_object_lm(obj);
	case UA_OBJ_TYPE_UP:
		return free_object_up(obj);
	case UA_OBJ_TYPE_AS:
		return free_object_as(obj);
	case UA_OBJ_TYPE_SP:
		return free_object_sp(obj);
	case UA_OBJ_TYPE_SG:
		return free_object_sg(obj);
	case UA_OBJ_TYPE_GP:
		return free_object_gp(obj);
	case UA_OBJ_TYPE_XP:
		return free_object_xp(obj);
	case UA_OBJ_TYPE_RP:
		return free_object_rp(obj);
	default:
		return;
	}
}

static struct df *__unlikely
alloc_object_df(int *errp, uint id)
{
	return &ua_defaults;
}
static struct lm *__unlikely
alloc_object_lm(int *errp, uint id)
{
}
static struct up *__unlikely
alloc_object_up(int *errp, uint id)
{
	struct up *up;

	if ((up = (typeof(up)) mi_open_alloc_sleep(sizeof(*up)))) {
		bzero(up, sizeof(*up));
		init_ua_lock(&up->sq);
		up->l_state = DL_UNBOUND;
		up->up.flags = 0;
		up->up.state = -1U;
		up->up.options = ua_defaults.options.up;
		up->up.info.lmi_primitive = LMI_INFO_ACK;
		up->up.info.lmi_version = 1;	/* LMI in fact does not have a version. */
		up->up.info.lmi_state = LMI_UNUSABLE;
		up->up.info.lmi_max_sdu = 272;
		/* TS 102 141 says that this is retricted to the narrowband maximum.  That is way
		   too restrictive, but we will start with it anyway. draft-bidulock-sigtran-sginfo 
		   provides a mechanism for the SG to update this value and we recognize the SGINFO 
		   parameter. */
		up->up.info.lmi_min_sdu = 6;
		up->up.info.lmi_header_len = 48;
		/* lmi_header_len is a recommendation to the module above on how much write offset
		   to apply to data messages to permit adding protocol headers.  For M2UA, the
		   protocol header length for SUs depends on whether the a text or integer based
		   IID is used, also whether DATA1 or DATA2 (TTC) formats are used.  To begin with, 
		   assume that a maximum sized M2UA header is necessary. In fact, we don't care. We 
		   always append a new message block for the header. */
		up->up.info.lmi_ppa_style = LMI_STYLE2;
	} else
		*errp = ENOMEM;
	return (up);
}
static struct as *__unlikely
alloc_object_as(int *errp, uint id)
{
	struct as *as;

	if ((as = kmem_alloc(sizeof(*as), KM_NOSLEEP))) {
		bzero(as, sizeof(*as));
		as->as.id = id;	/* FIXME: assign if zero */
		as->as.flags = 0;
		as->as.state = AS_DOWN;
		as->as.streamid = 1;
		as->as.request_id = 0;
		as->as.options = ua_defaults.options.as;
	} else
		*errp = ENOMEM;
	return (as);
}
static struct sp *__unlikely
alloc_object_sp(int *errp, uint id)
{
	struct sp *sp;

	if ((sp = kmem_alloc(sizeof(*sp), KM_NOSLEEP))) {
		bzero(sp, sizeof(*sp));
		/* start life locked */
		spinlock_init(&sp->lock);
		sp->users = 1;
		sp->waitq = NULL;
		sp->sp.id = id;	/* FIXME: assign if zero */
		sp->sp.flags = 0;
		sp->sp.state = ASP_DOWN;
		sp->sp.options = ua_defaults.options.sp;
	} else
		*errp = ENOMEM;
	return (sp);
}
static struct sg *__unlikely
alloc_object_sg(int *errp, uint id)
{
	struct sg *sg;

	if ((sg = kmem_alloc(sizeof(*sg), KM_NOSLEEP))) {
		bzero(sg, sizeof(*sg));
		sg->sg.id = id;	/* FIXME: assign if zero */
		sg->sg.flags = 0;
		sg->sg.state = ASP_DOWN;
		sg->sg.options = ua_defaults.options.sg;
	} else
		*errp = ENOMEM;
	return (sg);
}
static struct gp *__unlikely
alloc_object_gp(int *errp, uint id)
{
	struct gp *gp;
	struct ua_timer *t;

	if ((gp = kmem_alloc(sizeof(*gp), KM_NOSLEEP))) {
		bzero(gp, sizeof(*gp));
		gp->gp.id = id;	/* FIXME: assign if zero */
		gp->gp.flags = 0;
		gp->gp.state = ASP_DOWN;
		gp->gp.nextstream = 0;
		gp->gp.options = ua_defaults.options.gp;

		if (!gp->gp.wack_aspup = mi_timer_alloc(sizeof(*t)))
			goto no_wack_aspup;
		t = (typeof(t)) gp->rp.wack_aspup->b_rptr;
		t->timer = gp_wack_aspup;
		t->obj = gp;
		if (!gp->gp.wack_aspdn = mi_timer_alloc(sizeof(*t)))
			goto no_wack_aspdn;
		t = (typeof(t)) gp->rp.wack_aspdn->b_rptr;
		t->timer = gp_wack_aspdn;
		t->obj = gp;
		if (!gp->gp.wack_hbeat = mi_timer_alloc(sizeof(*t)))
			goto no_wack_hbeat;
		t = (typeof(t)) gp->rp.wack_hbeat->b_rptr;
		t->timer = gp_wack_hbeat;
		t->obj = gp;
	} else
		*errp = ENOMEM;
	return (gp);
      no_wack_hbeat:
	mi_timer_free(gp->gp.wack_aspdn);
      no_wack_aspdn:
	mi_timer_free(gp->gp.wack_aspup);
      no_wack_aspup:
	kmem_free(XCHG(&gp, NULL), sizeof(*gp));
	*errp = ENOBUFS;
	return (gp);
}
static struct xp *__unlikely
alloc_object_xp(int *errp, uint id)
{
	struct tp *tp;

	if ((tp = (typeof(tp)) mi_open_alloc(sizeof(*tp)))) {
		bzero(tp, sizeof(*tp));
		init_ua_lock(&tp->sq);
		tp->xp.id = id;
		tp->xp.flags = 0;
		tp->xp.state = -1U;
		tp->xp.options = ua_defaults.options.xp;
		tp->xp.info.PRIM_type = T_INFO_ACK;
		tp->xp.info.TSDU_size = -1;
		tp->xp.info.ETSDU_size = -1;
		tp->xp.info.CDATA_size = -1;
		tp->xp.info.DDATA_size = -1;
		tp->xp.info.ADDR_size = sizeof(struct sockaddr);
		tp->xp.info.OPT_size = -1;
		tp->xp.info.TIDU_size = -1;
		tp->xp.info.SERV_type = T_COTS_ORD;
		tp->xp.info.CURRENT_state = -1;
		tp->xp.info.PROVIDER_flag = T_XPG4_1;
	} else
		*errp = ENOMEM;
	return (tp);
}
static struct rp *__unlikely
alloc_object_rp(int *errp, uint id)
{
	struct rp *rp;
	struct ua_timer *t;

	if ((rp = kmem_alloc(sizeof(*rp), KM_NOSLEEP))) {
		bzero(rp, sizeof(*rp));
		rp->rp.flags = 0;
		rp->rp.state = AS_DOWN;
		if (!(rp->rp.wack_aspac = mi_timer_alloc(sizeof(*t))))
			goto no_wack_aspac;
		t = (typeof(t)) rp->rp.wack_aspac->b_rptr;
		t->timer = rp_wack_aspac;
		t->obj = rp;
		if (!(rp->rp.wack_aspia = mi_timer_alloc(sizeof(*t))))
			goto no_wack_aspia;
		t = (typeof(t)) rp->rp.wack_aspia->b_rptr;
		t->timer = rp_wack_aspia;
		t->obj = rp;
		if (!(rp->rp.wack_hbeat = mi_timer_alloc(sizeof(*t))))
			goto no_wack_hbeat;
		t = (typeof(t)) rp->rp.wack_hbeat->b_rptr;
		t->timer = rp_wack_hbeat;
		t->obj = rp;
	} else
		*errp = ENOMEM;
	return (rp);
      no_wack_aspac:
	mi_timer_free(rp->rp.wack_aspia);
      no_wack_aspia:
	mi_timer_free(rp->rp.wack_aspac);
      no_wack_hbeat:
	kmem_free(XCHG(&rp, NULL), sizeof(*rp));
	*errp = ENOBUFS;
	return (rp);
}
static void *__unlikely
alloc_object(int *errp, uint type, uint id)
{
	switch (type) {
	case UA_OBJ_TYPE_DF:
		return (void *) alloc_object_df(errp, id);
	case UA_OBJ_TYPE_LM:
		return (void *) alloc_object_lm(errp, id);
	case UA_OBJ_TYPE_UP:
		return (void *) alloc_object_up(errp, id);
	case UA_OBJ_TYPE_AS:
		return (void *) alloc_object_as(errp, id);
	case UA_OBJ_TYPE_SP:
		return (void *) alloc_object_sp(errp, id);
	case UA_OBJ_TYPE_SG:
		return (void *) alloc_object_sg(errp, id);
	case UA_OBJ_TYPE_GP:
		return (void *) alloc_object_gp(errp, id);
	case UA_OBJ_TYPE_XP:
		return (void *) alloc_object_xp(errp, id);
	case UA_OBJ_TYPE_RP:
		return (void *) alloc_object_rp(errp, id);
	}
	*errp = EFAULT;
	return (NULL);
}

/**
 * del_object_df: - remove a DF object
 * @lm: layer management stream
 * @id: object identifier
 * @c: configuration structure pointer
 */
static int __unlikely
del_object_df(struct up *lm, queue_t *q, uint id, struct ua_conf_df *c)
{
	return (EINVAL);
}

/* forward declarations */
static void remove_object_df(struct df *df);
static void remove_object_lm(struct up *lm);
static void remove_object_up(struct up *up);
static void remove_object_as(struct as *as);
static void remove_object_sp(struct sp *sp);
static void remove_object_sg(struct sg *sg);
static void remove_object_gp(struct gp *gp);
static void remove_object_xp(struct tp *tp);

static void __unlikely
remove_object_df(struct df *df)
{
	struct as *as;
	struct sp *sp;
	struct sg *sg;
	struct gp *gp;

	while ((as = df->as.list))
		remove_object_as(as);
	while ((sp = df->sp.list))
		remove_object_sp(sp);
	while ((sg = df->sg.list))
		remove_object_sg(sg);
	while ((gp = df->gp.list))
		remove_object_gp(gp);

	free_object(UA_OBJ_TYPE_DF, df);
}
static void __unlikely
remove_object_lm(struct up *lm)
{
	unsigned long flags;
	struct as *as;
	struct sp *sp;

	write_lock_irqsave(&ua_mux_lock, flags);
	if ((as = lm->as.as)) {
		lm->as.as = NULL;
		as->up.up = NULL;
	}
	if ((sp = lm->sp.sp)) {
		lm->sp.sp = NULL;
		sp->lm.lm = NULL;
	}
	write_unlock_irqrestore(&ua_mux_lock, flags);

	free_object(UA_OBJ_TYPE_LM, lm);
}
static void __unlikely
remove_object_up(struct up *up)
{
	unsigned long flags;
	struct as *as;

	write_lock_irqsave(&ua_mux_lock, flags);
	if ((as = up->as.as)) {
		up->as.as = NULL;
		as->up.up = NULL;
	}
	if ((*up->sp.prev = up->sp.next))
		up->sp.next->sp.prev = up->sp.prev;
	up->sp.next = NULL;
	up->sp.prev = &up->sp.next;
	up->sp.sp = NULL;
	write_unlock_irqrestore(&ua_mux_lock, flags);

	free_object(UA_OBJ_TYPE_UP, up);
}
static void __unlikely
remove_object_rp(struct rp *rp)
{
	unsigned long flags;
	mblk_t *t;

	if (t = XCHG(&rp->rp.wack_aspac, NULL))
		mi_timer_free(t);
	if (t = XCHG(&rp->rp.wack_aspia, NULL))
		mi_timer_free(t);
	if (t = XCHG(&rp->rp.wack_hbeat, NULL))
		mi_timer_free(t);

	write_lock_irqsave(&ua_mux_lock, flags);
	if ((*rp->as.prev = rp->as.next))
		rp->as.next->as.prev = rp->as.prev;
	rp->as.next = NULL;
	rp->as.prev = &rp->as.next;
	if (rp->as.as) {
		rp->as.as->rp.count[rp->rp.state]--;
		rp->as.as = NULL;
	}
	if ((*rp->gp.prev = rp->gp.next))
		rp->gp.next->gp.prev = rp->gp.prev;
	rp->gp.next = NULL;
	rp->gp.prev = &rp->gp.next;
	if (rp->gp.gp) {
		rp->gp.gp->rp.count[rp->rp.state]--;
		rp->gp.gp = NULL;
	}
	write_unlock_irqrestore(&ua_mux_lock, flags);

	free_object(UA_OBJ_TYPE_RP, rp);
}
static void __unlikely
remove_object_as(struct as *as)
{
	unsigned long flags;
	struct up *up;
	struct rp *rp;

	while ((rp = as->rp.list))
		remove_object_rp(rp);

	write_lock_irqsave(&ua_mux_lock, flags);
	if ((up = as->up.up)) {
		up->as.as = NULL;
		as->up.up = NULL;
	}
	if ((*as->sp.prev = as->sp.next))
		as->sp.next->sp.prev = as->sp.prev;
	as->sp.next = NULL;
	as->sp.prev = &as->sp.next;
	if ((*as->as.prev = as->as.next))
		as->as.next->as.prev = as->as.prev;
	as->as.next = NULL;
	as->as.prev = &as->as.next;
	write_unlock_irqrestore(&ua_mux_lock, flags);

	free_object(UA_OBJ_TYPE_AS, as);
}
static void __unlikely
remove_object_sp(struct sp *sp)
{
	unsigned long flags;
	struct up *lm;
	struct up *up;
	struct as *as;
	struct sg *sg;
	struct tp *tp;

	if ((lm = sp->lm.lm))
		remove_object_lm(lm);
	while ((up = sp->up.list))
		remove_object_up(up);
	while ((as = sp->as.list))
		remove_object_as(as);
	while ((sg = sp->sg.list))
		remove_object_sg(sg);
	while ((tp = sp->xp.list))
		remove_object_xp(tp);

	write_lock_irqsave(&ua_mux_lock, flags);
	if ((*sp->sp.prev = sp->sp.next))
		sp->sp.next->sp.prev = sp->sp.prev;
	sp->sp.next = NULL;
	sp->sp.prev = &sp->sp.next;
	write_unlock_irqrestore(&ua_mux_lock, flags);

	if (sp->users)
		sp_unlock(sp);

	free_object(UA_OBJ_TYPE_SP, sp);
}
static void __unlikely
remove_object_sg(struct sg *sg)
{
	unsigned long flags;
	struct gp *gp;
	struct sp *sp;

	while ((gp = sg->gp.list))
		remove_object_gp(gp);

	write_lock_irqsave(&ua_mux_lock, flags);
	if ((*sg->sp.prev = sg->sp.next))
		sg->sp.next->sp.prev = sg->sp.prev;
	sg->sp.next = NULL;
	sg->sp.prev = &sg->sp.next;
	if ((sp = sg->sp.sp)) {
		sg->sp.sp = NULL;
		sp->sg.count[sg->sg.state]--;
	}
	if ((*sg->sg.prev = sg->sg.next))
		sg->sg.next->sg.prev = sg->sg.prev;
	sg->sg.next = NULL;
	sg->sg.prev = &sg->sg.next;
	write_unlock_irqrestore(&ua_mux_lock, flags);

	free_object(UA_OBJ_TYPE_SG, sg);
}
static void __unlikely
remove_object_gp(struct gp *gp)
{
	unsigned long flags;

	if (gp->xp.xp)
		remove_object_xp(gp->xp.xp);
	while (gp->rp.list)
		remove_object_rp(gp->rp.list);

	write_lock_irqsave(&ua_mux_lock, flags);
	if ((*gp->sg.prev = gp->sg.next))
		gp->sg.next->sg.prev = gp->sg.prev;
	gp->sg.next = NULL;
	gp->sg.prev = &gp->sg.next;
	if (gp->sg.sg) {
		gp->sg.sg->gp.count[gp->gp.state]--;
		gp->sg.sg = NULL;
	}
	if ((*gp->gp.prev = gp->gp.next))
		gp->gp.next->gp.prev = gp->gp.prev;
	gp->gp.next = NULL;
	gp->gp.prev = &gp->gp.next;
	write_unlock_irqrestore(&ua_mux_lock, flags);

	free_object(UA_OBJ_TYPE_GP, gp);
}
static void __unlikely
remove_object_xp(struct tp *tp)
{
	unsigned long flags;
	struct gp *gp;

	write_lock_irqsave(&ua_mux_lock, flags);
	if ((gp = tp->gp.gp)) {
		gp->xp.xp = NULL;
		tp->gp.gp = NULL;
	}
	if ((*tp->sp.prev = tp->sp.next))
		tp->sp.next->sp.prev = tp->sp.prev;
	tp->sp.next = NULL;
	tp->sp.prev = &tp->sp.next;
	tp->sp.sp = NULL;
	write_unlock_irqrestore(&ua_mux_lock, flags);

	free_object(UA_OBJ_TYPE_XP, tp);
}

static void __unlikely
remove_object(uint type, void *obj)
{
	switch (type) {
	case UA_OBJ_TYPE_DF:
		return remove_object_df(obj);
	case UA_OBJ_TYPE_LM:
		return remove_object_lm(obj);
	case UA_OBJ_TYPE_UP:
		return remove_object_up(obj);
	case UA_OBJ_TYPE_AS:
		return remove_object_as(obj);
	case UA_OBJ_TYPE_SP:
		return remove_object_sp(obj);
	case UA_OBJ_TYPE_SG:
		return remove_object_sg(obj);
	case UA_OBJ_TYPE_GP:
		return remove_object_gp(obj);
	case UA_OBJ_TYPE_XP:
		return remove_object_xp(obj);
	default:
		return;
	}
}

/**
 * del_object_lm: - delete an LM object
 * @lm: layer management stream
 * @id: object identifier
 * @c: configuration structure pointer
 *
 * Deleting a layer management object consists of removing it as a controlling Stream for an SP
 * object.
 */
static int __unlikely
del_object_lm(struct up *up, queue_t *q, uint id, struct ua_conf_lm *c)
{
	struct up *lm;
	struct sp *sp;
	int err = 0;

	if (!(lm = find_object_lm(&err, up, id, c)))
		return (err);
	if (!(sp = lm->sp.sp))
		return (ENOENT);
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != up && up != lm_ctrl) {
		sp_unlock(sp);
		return (EACCES);
	}
	if (lm->xp.list) {
		sp_unlock(sp);
		return (EBUSY);
	}
	remove_object_lm(lm);
	sp_unlock(sp);
	return (0);
}

/**
 * del_object_up: - delete a UP object
 * @lm: layer management stream
 * @id: object identifier
 * @c: configuration structure pointer
 */
static int __unlikely
del_object_up(struct up *lm, queue_t *q, uint id, struct ua_conf_up *c)
{
	struct up *up;
	struct sp *sp;
	int err = 0;

	if (!(up = find_object_up(&err, lm, id, c)))
		return (err);
	if (!(sp = up->sp.sp))
		return (ENOENT);
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (up != lm && lm != lm_ctrl) {
		sp_unlock(sp);
		return (EACCES);
	}
	remove_object_up(up);
	sp_unlock(sp);
	return (EINVAL);
}

/**
 * del_object_as: - delete an AS object
 * @lm: layer management stream
 * @id: object identifier
 * @c: configuration structure pointer
 */
static int __unlikely
del_object_as(struct up *lm, queue_t *q, uint id, struct ua_conf_as *c)
{
	struct as *as;
	struct sp *sp;
	int err = 0;

	if (!(as = find_object_as(&err, lm, id, c)))
		return (err);
	sp = as->sp.sp;
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != lm_ctrl && lm != sp->lm.lm) {
		sp_unlock(sp);
		return (EACCES);
	}
	remove_object_as(as);
	sp_unlock(sp);
	return (0);
}

/**
 * del_object_sp: - delete an SP object
 * @lm: layer management stream
 * @id: object identifier
 * @c: configuration structure pointer
 */
static int __unlikely
del_object_sp(struct up *lm, queue_t *q, uint id, struct ua_conf_sp *c)
{
	struct sp *sp;
	int err = 0;

	if (!(sp = find_object_sp(&err, lm, id, c)))
		return (err);
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != lm_ctrl && lm != sp->lm.lm) {
		sp_unlock(sp);
		return (EACCES);
	}
	remove_object_sp(sp);
	return (0);
}

/**
 * del_object_sg: - delete an SG object
 * @lm: layer management stream
 * @id: object identifier
 * @c: configuration structure pointer
 */
static int __unlikely
del_object_sg(struct up *lm, queue_t *q, uint id, struct ua_conf_sg *c)
{
	struct sg *sg;
	struct sp *sp;
	int err = 0;

	if (!(sg = find_object_sg(&err, lm, id, c)))
		return (err);
	sp = sg->sp.sp;
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != lm_ctrl && lm != sp->lm.lm) {
		sp_unlock(sp);
		return (EACCES);
	}
	remove_object_sg(sg);
	sp_unlock(sp);
	return (0);
}

/**
 * del_object_gp: - delete a GP object
 * @lm: layer management stream
 * @id: object identifier
 * @c: configuration structure pointer
 */
static int __unlikely
del_object_gp(struct up *lm, queue_t *q, uint id, struct ua_conf_gp *c)
{
	struct gp *gp;
	struct sp *sp;
	int err = 0;

	if (!(gp = find_object_gp(&err, lm, id, c)))
		return (err);
	sp = gp->sg.sg->sp.sp;
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != lm_ctrl && lm != sp->lm.lm) {
		sp_unlock(sp);
		return (EACCES);
	}
	remove_object_gp(gp);
	sp_unlock(sp);
	return (0);
}

/**
 * del_object_xp: - delete an XP object
 * @lm: layer management stream
 * @id: object identifier
 * @c: configuration structure pointer
 */
static int __unlikely
del_object_xp(struct up *lm, queue_t *q, uint id, struct ua_conf_xp *c)
{
	struct tp *tp;
	struct sp *sp;
	int err = 0;

	if (!(tp = find_object_xp(&err, lm, id, c)))
		return (err);
	if (!tp->gp.gp)
		return (ENOENT);
	sp = tp->gp.gp->sg.sg->sp.sp;
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != lm_ctrl && lm != sp->lm.lm) {
		sp_unlock(sp);
		return (EACCES);
	}
	remove_object_xp(tp);
	sp_unlock(sp);
	return (0);
}

/**
 * del_object: - delete an object
 * @lm: layer management stream
 * @p: configuration structure pointer
 */
static int __unlikely
del_object(struct up *lm, queue_t *q, struct ua_config *p)
{
	switch (p->type) {
	case UA_OBJ_TYPE_DF:
		return del_object_df(lm, q, p->id, (struct ua_conf_df *) (p + 1));
	case UA_OBJ_TYPE_LM:
		return del_object_lm(lm, q, p->id, (struct ua_conf_lm *) (p + 1));
	case UA_OBJ_TYPE_UP:
		return del_object_up(lm, q, p->id, (struct ua_conf_up *) (p + 1));
	case UA_OBJ_TYPE_AS:
		return del_object_as(lm, q, p->id, (struct ua_conf_as *) (p + 1));
	case UA_OBJ_TYPE_SP:
		return del_object_sp(lm, q, p->id, (struct ua_conf_sp *) (p + 1));
	case UA_OBJ_TYPE_SG:
		return del_object_sg(lm, q, p->id, (struct ua_conf_sg *) (p + 1));
	case UA_OBJ_TYPE_GP:
		return del_object_gp(lm, q, p->id, (struct ua_conf_gp *) (p + 1));
	case UA_OBJ_TYPE_XP:
		return del_object_xp(lm, q, p->id, (struct ua_conf_xp *) (p + 1));
	default:
		return (EINVAL);
	}
}

static int __unlikely
cha_object_df(struct up *lm, queue_t *q, uint id, struct ua_conf_df *c)
{
	struct df *df;
	int err = 0;

	if (!(df = find_object_df(&err, lm, id, c)))
		return (err);
	if (lm != lm_ctrl)
		return (EACCES);
	return (0);
}
static int __unlikely
cha_object_lm(struct up *up, queue_t *q, uint id, struct ua_conf_lm *c)
{
	struct up *lm;
	struct sp *sp;
	int err = 0;

	if (!(lm = find_object_lm(&err, up, id, c)))
		return (err);
	if (up != lm_ctrl && (!(sp = up->sp.sp) || sp->lm.lm != up))
		return (EACCES);
	lm->lm.config = *c;
	return (0);
}
static int __unlikely
cha_object_up(struct up *lm, queue_t *q, uint id, struct ua_conf_up *c)
{
	struct up *up;
	struct sp *sp;
	int err = 0;

	if (!(up = find_object_up(&err, lm, id, c)))
		return (err);
	if (lm != lm_ctrl && (!(sp = up->sp.sp) || sp->lm.lm != lm))
		return (EACCES);
	// up->up.config = *c;
	return (0);
}
static int __unlikely
cha_object_as(struct up *lm, queue_t *q, uint id, struct ua_conf_as *c)
{
	struct as *as;
	struct sp *sp;
	int err = 0;

	if (!(as = find_object_as(&err, lm, id, c)))
		return (err);
	sp = as->sp.sp;
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != lm_ctrl && sp->lm.lm != lm) {
		sp_unlock(sp);
		return (EACCES);
	}
	as->as.config = *c;
	sp_unlock(sp);
	return (0);
}
static int __unlikely
cha_object_sp(struct up *lm, queue_t *q, uint id, struct ua_conf_sp *c)
{
	struct sp *sp;
	int err = 0;

	if (!(sp = find_object_sp(&err, lm, id, c)))
		return (err);
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != lm_ctrl && sp->lm.lm != lm) {
		sp_unlock(sp);
		return (EACCES);
	}
	sp->sp.config = *c;
	sp_unlock(sp);
	return (0);
}
static int __unlikely
cha_object_sg(struct up *lm, queue_t *q, uint id, struct ua_conf_sg *c)
{
	struct sg *sg;
	struct sp *sp;
	int err = 0;

	if (!(sg = find_object_sg(&err, lm, id, c)))
		return (err);
	sp = sg->sp.sp;
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != lm_ctrl && sp->lm.lm != lm) {
		sp_unlock(sp);
		return (EACCES);
	}
	sg->sg.config = *c;
	sp_unlock(sp);
	return (0);
}
static int __unlikely
cha_object_gp(struct up *lm, queue_t *q, uint id, struct ua_conf_gp *c)
{
	struct gp *gp;
	struct sp *sp;
	int err = 0;

	if (!(gp = find_object_gp(&err, lm, id, c)))
		return (err);
	sp = gp->sg.sg->sp.sp;
	if (!sp_trylock(sp, q))
		return (-EDEADLK)
		    if (lm != lm_ctrl && sp->lm.lm != lm) {
			sp_unlock(sp);
			return (EACCES);
		}
	gp->gp.config = *c;
	sp_unlock(sp);
	return (0);
}
static int __unlikely
cha_object_xp(struct up *lm, queue_t *q, uint id, struct ua_conf_xp *c)
{
	struct tp *tp;
	struct sp *sp;
	int err = 0;

	if (!(tp = find_object_xp(&err, lm, id, c)))
		return (err);
	if (!tp->gp.gp)
		return (ENOENT);
	sp = tp->gp.gp->sg.sg->sp.sp;
	if (!sp_trylock(sp, q))
		return (-EDEADLK);
	if (lm != lm_ctrl && sp->lm.lm != lm) {
		sp_unlock(sp);
		return (EACCES);
	}
	tp->xp.config = *c;
	sp_unlock(sp);
	return (0);
}

static int __unlikely
cha_object(struct up *lm, queue_t *q, struct ua_config *p)
{
	switch (p->type) {
	case UA_OBJ_TYPE_DF:
		return cha_object_df(lm, q, p->id, (struct ua_conf_df *) (p + 1));
	case UA_OBJ_TYPE_LM:
		return cha_object_lm(lm, q, p->id, (struct ua_conf_lm *) (p + 1));
	case UA_OBJ_TYPE_UP:
		return cha_object_up(lm, q, p->id, (struct ua_conf_up *) (p + 1));
	case UA_OBJ_TYPE_AS:
		return cha_object_as(lm, q, p->id, (struct ua_conf_as *) (p + 1));
	case UA_OBJ_TYPE_SP:
		return cha_object_sp(lm, q, p->id, (struct ua_conf_sp *) (p + 1));
	case UA_OBJ_TYPE_SG:
		return cha_object_sg(lm, q, p->id, (struct ua_conf_sg *) (p + 1));
	case UA_OBJ_TYPE_GP:
		return cha_object_gp(lm, q, p->id, (struct ua_conf_gp *) (p + 1));
	case UA_OBJ_TYPE_XP:
		return cha_object_xp(lm, q, p->id, (struct ua_conf_xp *) (p + 1));
	default:
		return (EINVAL);
	}
}

/* forward declarations */
static struct lm *create_object_lm(int *errp, struct up *lm, uint id);
static struct up *create_object_up(int *errp, struct up *lm, uint id);
static struct as *create_object_as(int *errp, struct up *lm, uint id, struct sp *sp);
static struct sp *create_object_sp(int *errp, struct up *lm, uint id);
static struct sg *create_object_sg(int *errp, struct up *lm, uint id, struct sp *sp, int pos);
static struct gp *create_object_gp(int *errp, struct up *lm, uint id, struct sg *sg, int pos);
static struct tp *create_object_xp(int *errp, struct up *lm, uint id, struct gp *gp);

static struct lm *__unlikely
create_object_lm(int *errp, struct up *lm, uint id, struct sp *sp)
{
	unsigned long flags;

	write_lock_irqsave(&ua_mux_lock);
	sp->lm.lm = lm;
	lm->sp.sp = sp;
	write_unlock_irqrestore(&ua_mux_lock);

	return (lm);
}
static struct up *__unlikely
create_object_up(int *errp, struct up *lm, uint id)
{
}
static struct as *__unlikely
create_object_as(int *errp, struct up *lm, uint id, struct sp *sp)
{
	unsigned long flags;
	struct as *as;
	struct sg *sg;
	struct gp *gp;
	struct rp *rp;
	struct ua_timer *t;

	if ((as = alloc_object(errp, UA_OBJ_TYPE_AS, id))) {
		as->as.config.spid = sp->sp.id;
		as->up.up = NULL;

		write_lock_irqsave(&ua_mux_lock, flags);
		if ((as->as.next = ua_defaults.as.list))
			as->as.next->as.prev = &as->as.next;
		as->as.prev = &ua_defaults.as.list;
		if ((as->sp.next = sp->as.list))
			as->sp.next->sp.prev = &as->sp.next;
		as->sp.prev = &sp->as.list;
		as->sp.sp = sp;
		sp->as.count[AS_DOWN]++;
		write_unlock_irqrestore(&ua_mux_lock, flags);

		for (sg = sp->sg.list; sg; sg = sg->sp.next) {
			for (gp = sg->gp.list; gp; gp = gp->sg.next) {
				if (!(rp = alloc_object(errp, UA_OBJ_TYPE_RP, 0))) {
					remove_object_as(as);
					return (NULL);
				}
				write_lock_irqsave(&ua_mux_lock, flags);
				rp->rp.streamid = gp->gp.nextstream;
				if (++gp->gp.nextstream > gp->gp.options.ostreams)
					gp->gp.nextstream = 1;
				if ((rp->as.next = as->rp.list))
					rp->as.next->as.prev = &rp->as.next;
				rp->as.prev = &as->rp.list;
				rp->as.as = as;
				as->rp.count[AS_DOWN]++;
				if ((rp->gp.next = gp->rp.list))
					rp->gp.next->gp.prev = &rp->gp.next;
				rp->gp.prev = &gp->rp.list;
				rp->gp.gp = gp;
				gp->rp.count[AS_DOWN]++;
				write_unlock_irqrestore(&ua_mux_lock, flags);
			}
		}
	}
	return (as);
}
static struct sp *__unlikely
create_object_sp(int *errp, struct up *lm, uint id)
{
	unsigned long flags;
	struct sp *sp;

	if ((sp = alloc_object(errp, UA_OBJ_TYPE_SP, id))) {
		write_lock_irqsave(&ua_mux_lock, flags);
		if ((sp->sp.next = ua_defaults.sp.list))
			sp->sp.next->sp.prev = &sp->sp.next;
		sp->sp.prev = &ua_defaults.sp.list;
		ua_defaults.sp.list = sp;
		lm->sp.next = NULL;
		lm->sp.prev = &sp->up.list;
		lm->sp.sp = sp;
		sp->up.list = lm;
		sp->lm.lm = lm;
		write_unlock_irqrestore(&ua_mux_lock, flags);
	}
	return (sp);
}
static struct sg *__unlikely
create_object_sg(int *errp, struct up *lm, uint id, struct sp *sp, int pos)
{
	unsigned long flags;
	struct sg *sg;

	/* assign a new unique position ordinal */
	if (pos == -1)
		for (sg = sp->sg.list; sg; sg = sg->sp.next)
			if (pos <= (int) sg->sp.position)
				pos = (int) sg->sp.position + 1;

	if ((sg = alloc_object(errp, UA_OBJ_TYPE_SG, id))) {
		sg->sg.config.spid = sp->sp.id;
		sg->sg.config.position = pos;
		sg->gp.list = NULL;

		write_lock_irqsave(&ua_mux_lock, flags);
		if ((sg->sg.next = ua_defaults.sg.list))
			sg->sg.next->sg.prev = &sg->sg.next;
		sg->sg.prev = &ua_defaults.sg.list;
		ua_defaults.sg.list = sg;
		if ((sg->sp.next = sp->sg.list))
			sg->sp.next->sp.prev = &sg->sp.next;
		sg->sp.prev = &sp->sg.list;
		sg->sp.sp = sp;
		sp->sg.count[ASP_DOWN]++;
		sp->sg.list = sg;
		write_unlock_irqrestore(&ua_mux_lock, flags);
	}
	return (sg);
}
static struct gp *__unlikely
create_object_gp(int *errp, struct up *lm, uint id, struct sg *sg, int pos)
{
	unsigned long flags;
	struct ua_timer *t;
	struct gp *gp;
	struct sp *sp;
	struct as *as;
	struct rp *rp;

	/* assign a new unique position oridinal */
	if (pos == -1)
		for (gp = sg->gp.list; gp; gp = gp->sg.next)
			if (pos <= (int) gp->sg.position)
				pos = (int) gp->sg.position + 1;

	if ((gp = alloc_object(errp, UA_OBJ_TYPE_GP, id))) {
		gp->gp.config.sgid = sg->sg.id;
		gp->gp.config.position = pos;
		gp->xp.xp = NULL;

		write_lock_irqsave(&ua_mux_lock, flags);
		if ((gp->gp.next = ua_defaults.gp.list))
			gp->gp.next->gp.prev = &gp->gp.next;
		gp->gp.prev = &ua_defaults.gp.list;
		ua_defaults.gp.list = gp;
		if ((gp->sg.next = sg->gp.list))
			gp->sg.next->sg.prev = &gp->sg.next;
		gp->sg.prev = &sg->gp.list;
		gp->sg.sg = sg;
		gp->sg.position = pos;
		sg->gp.count[ASP_DOWN]++;
		sg->gp.list = gp;
		write_unlock_irqrestore(&ua_mux_lock, flags);

		for (as = sg->sp.sp->as.list; as; as = as->sg.next) {
			if (!(rp = alloc_object(errp, UA_OBJ_TYPE_RP, 0))) {
				remove_object_gp(gp);
				return (NULL);
			}
			write_lock_irqsave(&ua_mux_lock, flags);
			rp->rp.streamid = gp->gp.nextstream;
			if (++gp->gp.nextstream > gp->gp.options.ostreams)
				gp->gp.nextstream = 1;
			if ((rp->as.next = as->rp.list))
				rp->as.next->as.prev = &rp->as.next;
			rp->as.prev = &as->rp.list;
			rp->as.as = as;
			as->rp.count[AS_DOWN]++;
			as->rp.list = rp;
			if ((rp->gp.next = gp->rp.list))
				rp->gp.next->gp.prev = &rp->gp.next;
			rp->gp.prev = &gp->rp.list;
			rp->gp.gp = gp;
			gp->rp.count[AS_DOWN]++;
			gp->rp.list = gp;
			write_unlock_irqrestore(&ua_mux_lock, flags);
		}
	}
	return (gp);
}
static struct tp *__unlikely
create_object_xp(int *errp, struct up *lm, uint id, struct gp *gp)
{
	if ((tp = find_object_xp(errp, lm, id, NULL))) {
		unsigned long flags;

		write_lock_irqsave(&ua_mux_lock, flags);
		tp->gp.gp = gp;
		gp->xp.xp = tp;
		write_unlock_irqrestore(&ua_mux_lock, flags);
	}
	return (tp);
}

static int __unlikely
add_object_df(struct up *lm, queue_t *q, uint id, struct ua_conf_df *c)
{
	struct df *df;
	int err = 0;

	if (!(df = find_object_df(&err, lm, id, c)))
		return (EINVAL);
	return (EEXIST);
}
static int __unlikely
add_object_lm(struct up *up, queue_t *q, uint id, struct ua_conf_lm *c)
{
	struct lm *lm;
	struct sp *sp;
	int err = 0;

	if (!(lm = find_object_lm(&err, up, id, c)))
		return (err);
	if (up != lm_ctrl && up != lm)
		return (EACCES);
	if (lm->sp.sp)
		return (EEXIST);
	if ((sp = find_object_sp(&err, up, c->spid, NULL))) {
		if (!sp_trylock(sp, q))
			return (-EDEADLK);
		if (up != lm_ctrl && up != sp->lm.lm) {
			sp_unlock(sp);
			return (EACCES);
		}
		if (!(lm = create_object_lm(&err, lm, id, sp))) {
			sp_unlock(sp);
			return (err);
		}
		sp_unlock(sp);
		return (0);
	}
	if (!(sp = create_object_sp(&err, up, c->spid)))
		return (err);
	if (!(lm = create_object_lm(&err, lm, id, sp))) {
		remove_object_sp(sp);
		return (err);
	}
	sp_unlock(sp);
	return (0);
}
static int __unlikely
add_object_up(struct up *lm, queue_t *q, uint id, struct ua_conf_up *c)
{
	struct up *up;
	int err = 0;

	if ((up = find_object_up(&err, lm, id, c)))
		return (EEXIST);
	if (!(up = create_object_up(&err, lm, id)))
		return (err);
	return (0);
}
static int __unlikely
add_object_as(struct up *lm, queue_t *q, uint id, struct ua_conf_as *c)
{
	struct as *as;
	struct sp *sp;
	int err = 0;

	if ((as = find_object_as(&err, lm, id, c)))
		return (EEXIST);
	if ((sp = find_object_sp(&err, lm, c->spid, NULL))) {
		if (!sp_trylock(sp, q))
			return (-EDEADLK);
		if (lm != lm_ctrl && lm != sp->lm.lm) {
			sp_unlock(sp);
			return (EACCES);
		}
		if (!(as = create_object_as(&err, lm, id, sp))) {
			sp_unlock(sp);
			return (err);
		}
		sp_unlock(sp);
		return (0);
	}
	if (!(sp = create_object_sp(&err, lm, c->spid))) {
		return (err);
	}
	if (!(as = create_object_as(&err, lm, id, sp))) {
		remove_object_sp(sp);
		return (err);
	}
	sp_unlock(sp);
	return (0);
}
static int __unlikely
add_object_sp(struct up *lm, queue_t *q, uint id, struct ua_conf_sp *c)
{
	struct sp *sp;
	int err = 0;

	if ((sp = find_object_sp(&err, lm, id, c)))
		return (EEXIST);
	if (!(sp = create_object_sp(&err, lm, id)))
		return (err);
	sp_unlock(sp);
	return (0);
}
static int __unlikely
add_object_sg(struct up *lm, queue_t *q, uint id, struct ua_conf_sg *c)
{
	struct sg *sg;
	struct sp *sp;
	int err = 0;

	if ((sg = find_object_sg(&err, lm, id, c)))
		return (EEXIST);
	if ((sp = find_object_sp(&err, lm, c->spid, NULL))) {
		if (!sp_trylock(sp, q))
			return (-EDEADLK);
		if (lm != lm_ctrl && lm != sp->lm.lm) {
			sp_unlock(sp);
			return (EACCES);
		}
		if (!(sg = create_object_sg(&err, lm, sp, c->position))) {
			sp_unlock(sp);
			return (err);
		}
		sp_unlock(sp);
		return (0);
	}
	if (!(sp = create_object_sp(&err, lm, c->spid)))
		return (err);
	if (!(sg = create_object_sg(&err, lm, id, sp, c->position))) {
		remove_object_sp(sp);
		return (err);
	}
	sp_unlock(sp);
	return (0);
}
static int __unlikely
add_object_gp(struct up *lm, queue_t *q, uint id, struct ua_conf_gp *c)
{
	struct gp *gp;
	struct sg *sg;
	struct sp *sp;
	int err = 0;

	if ((gp = find_object_gp(&err, lm, id, c)))
		return (EEXIST);
	if ((sg = find_object_sg(&err, lm, c->sgid, NULL))) {
		sp = sg->sp.sp;
		if (!sp_trylock(sp, q))
			return (-EDEADLK);
		if (lm != lm_ctrl && lm != sg->sp.sp->lm.lm) {
			sp_unlock(sp);
			return (EACCES);
		}
		if (!(gp = create_object_gp(&err, lm, id, sg, c->position))) {
			sp_unlock(sp);
			return (err);
		}
		sp_unlock(sp);
		return (0);
	}
	if ((sp = find_object_sp(&err, lm, 0, NULL))) {
		if (!sp_trylock(sp, q))
			return (-EDEADLK);
		if (lm != lm_ctrl && lm != sp->lm.lm) {
			sp_unlock(sp);
			return (EACCES);
		}
		if (!(sg = create_object_sg(&err, lm, c->sgid, sp, -1))) {
			sp_unlock(sp);
			return (err);
		}
		if (!(gp = create_object_gp(&err, lm, id, sg, c->position))) {
			remove_object_sg(sg);
			sp_unlock(sp);
			return (err);
		}
		sp_unlock(sp);
		return (0);
	}
	if (!(sp = create_object_sp(&err, lm, 0)))
		return (err);
	if (!(sg = create_object_sg(&err, lm, c->sgid, sp, -1))) {
		remove_object_sp(sp);
		return (err);
	}
	if (!(gp = create_object_gp(&err, lm, id, sg, c->position))) {
		remove_object_sg(sg);
		remove_object_sp(sp);
		return (err);
	}
	sp_unlock(sp);
	return (0);
}
static int __unlikely
add_object_xp(struct up *lm, queue_t *q, uint id, struct ua_conf_xp *c)
{
	struct tp *tp;
	struct gp *gp;
	struct sg *sg;
	struct sp *sp;
	int err = 0;

	if (!(tp = find_object_xp(&err, lm, id, c)))
		return (err);
	if ((gp = find_object_gp(&err, lm, c->gpid, NULL))) {
		sp = gp->sg.sg->sp.sp;
		if (!sp_trylock(sp, q))
			return (-EDEADLK);
		if (lm != lm_ctrl && lm != gp->sg.sg->sp.sp->lm.lm) {
			sp_unlock(sp);
			return (EACCES);
		}
		if (!(tp = create_object_xp(&err, lm, id, gp))) {
			sp_unlock(sp);
			return (err);
		}
		sp_unlock(sp);
		return (0);
	}
	if ((sg = find_object_sg(&err, lm, 0, NULL))) {
		sp = sg->sp.sp;
		if (!sp_trylock(sp, q))
			return (-EDEADLK);
		if (lm != lm_ctrl && lm != sg->sp.sp->lm.lm) {
			sp_unlock(sp);
			return (EACCES);
		}
		if (!(gp = create_object_gp(&err, lm, c->gpid, sg, -1))) {
			sp_unlock(sp);
			return (err);
		}
		if (!(tp = create_object_xp(&err, lm, id, gp))) {
			remove_object_gp(gp);
			sp_unlock(sp);
			return (err);
		}
		sp_unlock(sp);
		return (0);
	}
	if ((sp = find_object_sp(&err, lm, 0, NULL))) {
		if (!sp_trylock(sp, q))
			return (-EDEADLK);
		if (lm != lm_ctrl && lm != sp->lm.lm) {
			sp_unlock(sp);
			return (EACCES);
		}
		if (!(sg = create_object_sg(&err, lm, 0, sp, -1))) {
			sp_unlock(sp);
			return (err);
		}
		if (!(gp = create_object_gp(&err, lm, c->gpid, sg, -1))) {
			remove_object_sg(sg);
			sp_unlock(sp);
			return (err);
		}
		if (!(tp = create_object_xp(&err, lm, id, gp))) {
			remove_object_gp(gp);
			remove_object_sg(sg);
			sp_unlock(sp);
			return (err);
		}
		sp_unlock(sp);
		return (0);
	}
	if (!(sp = create_object_sp(&err, lm, 0))) {
		return (err);
	}
	if (!(sg = create_object_sg(&err, lm, 0, sp, -1))) {
		remove_object_sp(sp);
		return (err);
	}
	if (!(gp = create_object_gp(&err, lm, c->gpid, sg, -1))) {
		remove_object_sg(sg);
		remove_object_sp(sp);
		return (err);
	}
	if (!(tp = create_object_xp(&err, lm, id, gp))) {
		remove_object_gp(gp);
		remove_object_sg(sg);
		remove_object_sp(sp);
		return (err);
	}
	sp_unlock(sp);
	return (0);
}
static int __unlikely
add_object(struct up *lm, queue_t *q, struct ua_config *p)
{
	switch (p->type) {
	case UA_OBJ_TYPE_DF:
		return add_object_df(lm, q, p->id, (struct ua_conf_df *) (p + 1));
	case UA_OBJ_TYPE_LM:
		return add_object_lm(lm, q, p->id, (struct ua_conf_lm *) (p + 1));
	case UA_OBJ_TYPE_UP:
		return add_object_up(lm, q, p->id, (struct ua_conf_up *) (p + 1));
	case UA_OBJ_TYPE_AS:
		return add_object_as(lm, q, p->id, (struct ua_conf_as *) (p + 1));
	case UA_OBJ_TYPE_SP:
		return add_object_sp(lm, q, p->id, (struct ua_conf_sp *) (p + 1));
	case UA_OBJ_TYPE_SG:
		return add_object_sg(lm, q, p->id, (struct ua_conf_sg *) (p + 1));
	case UA_OBJ_TYPE_GP:
		return add_object_gp(lm, q, p->id, (struct ua_conf_gp *) (p + 1));
	case UA_OBJ_TYPE_XP:
		return add_object_xp(lm, q, p->id, (struct ua_conf_xp *) (p + 1));
	default:
		return (EINVAL);
	}
}

/*
 * UA to LM primitives.
 * ==========================================================================
 */

/**
 * ua_user_ind: - issue a UA_USER_IND primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_user_ind(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_user_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_attach_ack: - issue a UA_ATTACH_ACK primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_attach_ack(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_attach_ack *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_sctp_ind: - issue a UA_SCTP_IND primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_sctp_ind(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_sctp_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_link_ack: - issue a UA_LINK_ACK primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_link_ack(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_link_ack *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_ok_ack: - issue a UA_OK_ACK primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_ok_ack(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_ok_ack *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_error_ack: - issue a UA_ERROR_ACK primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_error_ack(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_error_ack *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_aspup_con: - issue a UA_ASPUP_CON primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_aspup_con(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_aspup_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_aspup_ind: - issue a UA_ASPUP_IND primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_aspup_ind(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_aspup_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_aspdn_con: - issue a UA_ASPDN_CON primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_aspdn_con(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_aspdn_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_aspdn_ind: - issue a UA_ASPDN_IND primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_aspdn_ind(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_aspdn_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_hbeat_con: - issue a UA_HBEAT_CON primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_hbeat_con(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_hbeat_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_hbeat_ind: - issue a UA_HBEAT_IND primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_hbeat_ind(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_hbeat_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_info_ack: - issue a UA_INFO_ACK primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_info_ack(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_info_ack *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_aspac_con: - issue a UA_ASPAC_CON primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_aspac_con(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_aspac_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_aspac_ind: - issue a UA_ASPAC_IND primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_aspac_ind(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_aspac_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_aspia_con: - issue a UA_ASPIA_CON primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_aspia_con(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_aspia_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_aspia_ind: - issue a UA_ASPIA_IND primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_aspia_ind(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_aspia_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_asinfo_ack: - issue a UA_ASINFO_ACK primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_asinfo_ack(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_asinfo_ack *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_reg_rsp: - issue a UA_REG_RSP primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_reg_rsp(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_reg_rsp *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_dereg_rsp: - issue a UA_DEREG_RSP primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_dereg_rsp(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_dereg_rsp *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_error_con: - issue a UA_ERROR_CON primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_error_con(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_error_con *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

/**
 * ua_error_ind: - issue a UA_ERROR_IND primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_error_ind(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
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
 * ua_notify_ind: - issue a UA_NOTIFY_IND primitive to layer management
 * @lm: layer management stream
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
ua_notify_ind(struct up *lm, struct up *up, queue_t *q, mblk_t *msg)
{
	struct UA_notify_ind *p;
	mblk_t *mp;

	(void) p;
	(void) mp;
	/* FIXME: write this function */
	freemsg(msg);
	return (0);
}

#if M2UA
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
		p->lmi_version = up->up.info.lmi_version;
		p->lmi_state = up_get_state(up);
		p->lmi_max_sdu = up->up.info.lmi_max_sdu;
		p->lmi_min_sdu = up->up.info.lmi_min_sdu;
		p->lmi_header_len = up->up.info.lmi_header_len;
		p->lmi_ppa_style = up->up.info.lmi_ppa_style;
		mp->b_wptr += sizeof(*p);
		switch (up_get_state(up)) {
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
			bcopy(&up->as.as->as.addr, mp->b_wptr, sizeof(up->as.as->as.addr));
			mp->b_wptr += sizeof(up->as.as->as.addr);
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
		switch (up_get_state(up)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_DISABLED;
			up_set_state(up, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			up_set_state(up, LMI_UNATTACHED);
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
		switch (up_get_state(up)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = LMI_UNATTACHED;
			up_set_state(up, LMI_UNATTACHED);
			break;
		case LMI_DETACH_PENDING:
		case LMI_ENABLE_PENDING:
			p->lmi_state = LMI_DISABLED;
			up_set_state(up, LMI_DISABLED);
			break;
		case LMI_DISABLE_PENDING:
			p->lmi_state = LMI_ENABLED;
			up_set_state(up, LMI_ENABLED);
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
		up_set_state(up, LMI_ENABLED);
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
		up_set_state(up, LMI_DISABLED);
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
		p->lmi_state = up_set_state(up, state);
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
#endif

#if M3UA
/*
 *  MTP-Provider to MTP-User primitives.
 *  =========================================================================
 */

/**
 * mtp_ok_ack: - issue MTP_OK_ACK primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static inline fastcall int
mtp_ok_ack(struct up *up, queue_t *q, mblk_t *msg, mtp_long prim)
{
	struct MTP_ok_ack *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OK_ACK;
		p->mtp_correct_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_OK_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_error_ack: - issue MTP_ERROR_ACK primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: error primitive
 * @error: negative UNIX, positive MTPI error
 */
static inline fastcall int
mtp_error_ack(struct up *up, queue_t *q, mblk_t *msg, mtp_long prim, mtp_long error)
{
	struct MTP_error_ack *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ERROR_ACK;
		p->mtp_error_primitive = prim;
		p->mtp_mtpi_error = error >= 0 ? error : MSYSERR;
		p->mtp_unix_error = error >= 0 ? 0 : -error;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_ERROR_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_bind_ack: - issue MTP_BIND_ACK primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 * @flags: bind flags
 */
static inline fastcall int
mtp_bind_ack(struct up *up, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, mtp_ulong flags)
{
	struct MTP_bind_ack *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_BIND_ACK;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = sizeof(*p);
		p->mtp_bind_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_BIND_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_addr_ack: - issue MTP_ADDR_ACK primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @lptr: local address pointer
 * @llen: local address length
 * @rptr: remote address pointer
 * @rlen: remote address length
 */
static inline fastcall int
mtp_addr_ack(struct up *up, queue_t *q, mblk_t *msg, caddr_t lptr, size_t llen, caddr_t rptr,
	     size_t rlen)
{
	struct MTP_addr_ack *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + llen + rlen, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ADDR_ACK;
		p->mtp_loc_length = llen;
		p->mtp_loc_offset = sizeof(*p);
		p->mtp_rem_length = rlen;
		p->mtp_rem_offset = sizeof(*p) + llen;
		mp->b_wptr += sizeof(*p);
		bcopy(lptr, mp->b_wptr, llen);
		mp->b_wptr += llen;
		bcopy(rptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_ADDR_ACK");
		putnext(up->rq, mp);
		return (0);

	}
	return (-ENOBUFS);
}

/**
 * mtp_info_ack: - issue MTP_INFO_ACK primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 */
static inline fastcall int
mtp_info_ack(struct up *up, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct MTP_info_ack *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_msu_size = up->info.mtp_msu_size;
		p->mtp_addr_size = up->info.mtp_addr_size;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = sizeof(*p);
		p->mtp_current_state = up->info.mtp_current_state;
		p->mtp_serv_type = up->info.mtp_serv_type;
		p->mtp_version = up->info.mtp_version;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_INFO_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_optmgmt_ack: - issue MTP_OPTMGMT_ACK primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @optr: options pointer
 * @olen: options length
 * @flags: management flags
 */
static inline fastcall int
mtp_optmgmt_ack(struct up *up, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, mtp_ulong flags)
{
	struct MTP_optmgmt_ack *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OPTMGMT_ACK;
		p->mtp_opt_length = olen;
		p->mtp_opt_offset = sizeof(*p);
		p->mtp_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_OPTMGMT_ACK");
		putnext(up->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_transfer_ind: - issue MTP_TRANSFER_IND primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @srce: source address
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static inline fastcall int
mtp_transfer_ind(struct up *up, queue_t *q, mblk_t *msg, struct mtp_addr *srce, mtp_ulong pri,
		 mtp_ulong sls, mblk_t *dp)
{
	struct MTP_transfer_ind *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + sizeof(*srce), BPRI_MED))) {
		if (canputnext(up->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_TRANSFER_IND;
			p->mtp_srce_length = sizeof(*srce);
			p->mtp_srce_offset = sizeof(*p);
			p->mtp_mp = pri;
			p->mtp_sls = sls;
			mp->b_wptr += sizeof(*p);
			bcopy(srce, mp->b_wptr, sizeof(*srce));
			mp->b_wptr += sizeof(*srce);
			mp->b_cont = dp;
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_TRANSFER_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_pause_ind: - issue MTP_PAUSE_IND primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @addr: address
 */
static inline fastcall int
mtp_pause_ind(struct up *up, queue_t *q, mblk_t *msg, struct mtp_addr *addr)
{
	struct MTP_pause_ind *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + sizeof(*addr), BPRI_MED))) {
		if (bcanputnext(up->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_PAUSE_IND;
			p->mtp_addr_length = sizeof(*addr);
			p->mtp_addr_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, sizeof(*addr));
			mp->b_wptr += sizeof(*addr);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_PAUSE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_resume_ind: - issue MTP_RESUME_IND primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @addr: address
 */
static inline fastcall int
mtp_resume_ind(struct up *up, queue_t *q, mblk_t *msg, struct mtp_addr *addr)
{
	struct MTP_resume_ind *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + sizeof(*addr), BPRI_MED))) {
		if (bcanputnext(up->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESUME_IND;
			p->mtp_addr_length = sizeof(*addr);
			p->mtp_addr_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, sizeof(*addr));
			mp->b_wptr += sizeof(*addr);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_RESUME_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_status_ind: - issue MTP_STATUS_IND primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @addr: address
 * @type: status type
 * @status: the status
 */
static inline fastcall int
mtp_status_ind(struct up *up, queue_t *q, mblk_t *msg, struct mtp_addr *addr, mtp_ulong type,
	       mtp_ulong status)
{
	struct MTP_status_ind *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + sizeof(*addr), BPRI_MED))) {
		if (bcanputnext(up->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_STATUS_IND;
			p->mtp_addr_length = sizeof(*addr);
			p->mtp_addr_offset = sizeof(*p);
			p->mtp_type = type;
			p->mtp_status = status;
			mp->b_wptr += sizeof(*p);
			bcopy(aptr, mp->b_wptr, sizeof(*addr));
			mp->b_wptr += sizeof(*addr);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_STATUS_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_begins_ind: - issue MTP_RESTART_BEGINS_IND primitive upstream
 * @q: active queue
 * @up: MTP private structure
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_restart_begins_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_begins_ind *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		if (bcanputnext(up->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESTART_BEGINS_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_RESTART_BEGINS");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_complete_ind: - issue MTP_RESTART_COMPLETE_IND primitive upstream
 * @up: MTP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
mtp_restart_complete_ind(struct up *up, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_complete_ind *p;
	mblk_t *mp;

	if ((mp = ua_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		if (bcanputnext(up->rq, 3)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 3;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_RESTART_COMPLETE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "<- MTP_RESTART_COMPLETE_IND");
			putnext(up->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

#if SUA
/*
 *  SCCP-Provider to SCCP-User primitives.
 *  =========================================================================
 */
#endif

#if TUA
/*
 *  TCAP-Provider to TCAP-User primitives.
 *  =========================================================================
 */
#endif

/*
 * TS-User to TS-Provider primitives.
 * ==========================================================================
 */

/**
 * t_conn_req: - issue a T_CONN_REQ primitive downstream
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
			tp_set_state(tp, TS_WACK_CREQ);
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
 * t_conn_res: - issue a T_CONN_RES primitive downstream
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
			tp_set_state(tp, TS_WACK_CRES);
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
 * t_discon_req: - issue a T_DISCON_REQ primitive downstream
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

	if (likely(!!(mp = ua_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, 2))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DISCON_REQ;
			p->SEQ_number = sequence;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			switch (tp_get_state(tp)) {
			case TS_WCON_CREQ:
				tp_set_state(tp, TS_WACK_DREQ6);
				break;
			case TS_WRES_CIND:
				tp_set_state(tp, TS_WACK_DREQ7);
				break;
			case TS_DATA_XFER:
				tp_set_state(tp, TS_WACK_DREQ9);
				break;
			case TS_WIND_ORDREL:
				tp_set_state(tp, TS_WACK_DREQ10);
				break;
			case TS_WREQ_ORDREL:
				tp_set_state(tp, TS_WACK_DREQ11);
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
 * t_data_req: - issue a T_DATA_REQ primitive downstream
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
 * t_exdata_req: - issue a T_EXDATA_REQ primitive downstream
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
 * t_info_req: - issue a T_INFO_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
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
 * t_bind_req: - issue a T_BIND_REQ primitive downstream
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
			if (tp_get_state(tp) == TS_IDLE)
				tp_set_state(tp, TS_WACK_BREQ);
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
 * t_unbind_req: - issue a T_UNBIND_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline fastcall int
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
			tp_set_state(tp, TS_WACK_UREQ);
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
 * t_unitdata_req: - issue a T_UNITDATA_REQ primitive downstream
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
 * t_optmgmt_req: - issue a T_OPTMGMT_REQ primitive downstream
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
			if (tp_get_state(tp) == TS_IDLE)
				tp_set_state(tp, TS_WACK_OPTREQ);
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
 * t_ordrel_req: - issue a T_ORDREL_REQ primitive downstream
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
			switch (tp_get_state(tp)) {
			case TS_DATA_XFER:
				tp_set_state(tp, TS_WIND_ORDREL);
				break;
			case TS_WREQ_ORDREL:
				tp_set_state(tp, TS_IDLE);
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
 * t_optdata_req: - issue a T_OPTDATA_REQ primitive downstream
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flag: data transfer flag
 * @sid: stream id
 * @dp: user data
 */
static inline fastcall int
t_optdata_req(struct tp *tp, queue_t *q, mblk_t *msg, t_scalar_t flag, t_uscalar_t sid, mblk_t *dp)
{
	struct T_optdata_req *p;
	mblk_t *mp;
	struct t_opthdr *oh;

	if (likely(!!(mp = ua_allocb(q, sizeof(*p) + 2 * (sizeof(*oh) + sizeof(sid)), BPRI_MED)))) {
		if (likely(bcanputnext(tp->wq, dp->b_band))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = dp->b_band;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flag;
			p->OPT_length = 2 * (sizeof(*oh) + sizeof(sid));
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
			oh->len = sizeof(sid);
			mp->b_wptr += sizeof(*oh);
			*(t_uscalar_t *) mp->b_wptr = sid;
			mp->b_wptr += sizeof(sid);
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
 * t_addr_req: - issue a T_ADDR_REQ primitive downstream
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
 * t_capability_req: - issue a T_CAPABILITY_REQ primitive downstream
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
 * gp_send_mgmt_err: - send MGMT ERR message
 * @gp: GP object
 * @q: active queue
 * @msg: message to free upon success
 * @ecode: error code
 * @dptr: diagnostics pointer
 * @dlen: diagnostics length
 */
static noinline fastcall __unlikely int
gp_send_mgmt_err(struct gp *gp, queue_t *q, mblk_t *msg, uint32_t ecode, caddr_t dptr, size_t dlen)
{
	struct tp *tp = gp->xp.xp;
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
 * gp_send_asps_aspup_req: - send ASP Up
 * @gp: GP object
 * @q: active queue
 * @msg: message to free upon success
 * @iptr: Info pointer
 * @ilen: Info length
 */
static fastcall int
gp_send_asps_aspup_req(struct gp *gp, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	struct tp *tp = gp->xp.xp;
	struct sp *sp = gp->sp.sp;
	int err;
	mblk_t *mp;
	uint popt = sp->sp.options.options.popt;
	size_t olen = 0;
	size_t mlen = UA_MHDR_SIZE +
	    sp->sp.options.aspid ? UA_SIZE(UA_PARM_ASPID) : 0 +
	    ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0;

	if ((popt & UA_ASPEXT)) {
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
		if (sp->sp.options.aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(sp->sp.options.aspid);
			p += 2;
		}
		if ((popt & UA_ASPEXT)) {
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
		if (sp->sp.options.tack) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
			       "-> WACK ASPUP START <- (%u msec)", sp->sp.options.tack);
			mi_timer(tp->rq, gp->gp.wack_aspup, sp->sp.options.tack);
		}

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPS ASPUP Req ->");
		/* send unordered and expedited on stream 0 */
		if ((err = gp_set_state(gp, q, ASP_WACK_ASPUP)) ||
		    (err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp)))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * gp_send_asps_aspdn_req: - send ASP Down
 * @gp: SGP to which to send message
 * @q: active queue
 * @msg: message to free upon success
 * @iptr: Info pointer
 * @ilen: Info length
 */
static fastcall int
gp_send_asps_aspdn_req(struct gp *gp, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	struct tp *tp = gp->xp.xp;
	struct sp *sp = gp->sp.sp;
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    sp->sp.options.aspid ? UA_SIZE(UA_PARM_ASPID) : 0 +
	    ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0;

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (sp->sp.options.aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(sp->sp.options.aspid);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(ilen);
		if (gp->sp.sp->sp.options.tack) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
			       "-> WACK ASPDN START <- (%u msec)", gp->sp.sp->sp.options.tack);
			if (gp->gp.wack_aspdn)
				mi_timer(tp->rq, gp->gp.wack_aspdn, gp->sp.sp->sp.options.tack);
		}

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPS ASPDN Req ->");
		/* send unordered and expedited on stream 0 */
		if ((err = gp_set_state(gp, ASP_WACK_ASPDN)) ||
		    (err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp)))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * gp_send_asps_hbeat_req: - send a BEAT message
 * @gp: SGP to which to send
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat info pointer
 * @hlen: heartbeat info length
 *
 * This one is used for MGMT hearbeat not associated with any specific signalling link and,
 * therefore, sent on SCTP stream identifier 0 (management stream).
 */
fastcall int
gp_send_asps_hbeat_req(struct gp *gp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	struct tp *tp = gp->xp.xp;
	struct sp *sp = gp->sp.sp;
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
		if (sp->sp.options.tbeat && tp->rq) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
			       "-> WACK BEAT START <- (%u msec)", sp->sp.options.tbeat);
			if (gp->gp.wack_hbeat)
				mi_timer(tp->rq, gp->gp.wack_hbeat, sp->sp.options.tbeat);
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
 * rp_send_asps_hbeat_req: - send a BEAT message
 * @rp: AS-SGP relation pointer
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat info pointer
 * @hlen: heartbeat info length
 *
 * This one is used for AS-SG heartbeat associated with a specific User and, therefore, sent on the
 * same SCTP stream identifier as is data for the User.
 */
fastcall int
rp_send_asps_hbeat_req(struct rp *rp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	struct as *as = rp->as.as;
	struct gp *gp = rp->gp.gp;
	struct tp *tp = gp->tp.tp;
	int err;
	mblk_t *mp;
#if M2UA
	size_t tlen = strnlen(as->as.addr.iid_text, 32);
#endif
	size_t mlen = UA_MHDR_SIZE +
	    (hlen ? UA_PHDR_SIZE + UA_PAD4(hlen) : 0) +
#if M2UA
	    (as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
#endif
	    (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.asid) {
#if M2UA
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
#else
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
#endif
			p[1] = htonl(as->as.asid);
			p += 2;
		}
#if M2UA
		if (as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
#endif
		if (hlen) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hlen);
			bcopy(hptr, p, hlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(hlen));
		}
		mp->b_wptr = (unsigned char *) p;
		if (as->as.options.tbeat) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
			       "-> WACK BEAT START <- (%u msec)", as->as.options.tbeat);
			if (rp->rp.wack_hbeat)
				mi_timer(tp->rq, rp->rp.wack_hbeat, rp->as.as->as.options.tbeat);
		}
		/* send ordered on specified stream */

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, as->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * gp_send_asps_hbeat_ack: - send a BEAT Ack message
 * @gp: SGP to which to send
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
gp_send_asps_hbeat_ack(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *msg, caddr_t hptr, size_t hlen)
{
	struct sp *sp = gp->sg.sg->sp.sp;
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
		if (sp->sp.options.tbeat) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
					"-> WACK BEAT START <- (%u msec)", sp->sp.options.tbeat);
			if (gp->gp.wack_hbeat)
				mi_timer(tp->rq, gp->gp.wack_hbeat, sp->sp.options.tbeat);
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
 * rp_send_asps_hbeat_ack: - send a BEAT Ack message
 * @rp: AS-SGP relation pointer
 * @tp: TP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @hptr: heartbeat data pointer
 * @hlen: heartbeat data length
 *
 * This one is used for AS-SG hearbeat associated with a specific User and, therefore,
 * sent on the same SCTP stream identifier as is used for data messages.
 */
static fastcall int
rp_send_asps_hbeat_ack(struct rp *rp, struct tp *tp, queue_t *q, mblk_t *msg, caddr_t hptr,
		       size_t hlen)
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

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPS BEAT Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, rp->streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * rp_send_aspt_aspac_req: - send an ASP Active Request
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @iptr: info pointer
 * @ilen: info length
 *
 * This is used to activate an AS.  AS Users are activated when they are enabled by the User
 * enable/connect.  They are deactivated when they are disabled by the User diable/disconnect.
 */
static int
rp_send_aspt_aspac_req(struct rp *rp, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	struct tp *tp = rp->gp.gp->xp.xp;
	struct as *as = rp->as.as;

	int err;
	mblk_t *mp;
#if M2UA
	size_t tlen = strnlen(as->as.addr.iid_text, 32);
#endif
	size_t mlen = UA_MHDR_SIZE +
	    (as->as.addr.tmode ? UA_SIZE(UA_PARM_TMODE) : 0) +
	    (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
#if M2UA
	    (as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
#endif
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPAC_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.addr.tmode) {
			p[0] = UA_PARM_TMODE;
			p[1] = htonl(as->as.addr.tmode);
			p += 2;
		}
		if (as->as.asid) {
#if M2UA
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
#else
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
#endif
			p[1] = htonl(as->as.asid);
			p += 2;
		}
#if M2UA
		if (as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
#endif
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;
		if (as->as.options.tack) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
			       "-> WACK ASPAC START <- (%u msec)", as->as.options.tack);
			if (rp->rp.wack_aspac)
				mi_timer(tp->rq, rp->rp.wack_aspac, as->as.options.tack);
		}

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPT ASPAC Req ->");
		/* always sent on same stream as data */
		if (!(err = rp_set_state(rp, q, AS_WACK_ASPAC)) &&
		    !(err = t_optdata_req(tp, q, msg, 0, as->as.streamid, mp)))
			return (0);
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
 * @asid: pointer to IID/RC or NULL
 * @tptr: pointer to text IID or NULL
 * @tlen: length of text IID or zero
 * @iptr: info pointer
 * @ilen: info length
 *
 * This version is disassociated with a given User (AS) and is used to request deactivation of
 * the User (AS) for which messages have been received but for which there is no corresponding upper
 * stream.   This might happen when the upper stream closed non-gracefully and a message buffer
 * could not be allocated during close to deactivate the AS.  Also, it might happen if the SG is
 * foolish enough to send data messages to an inactive (non-existent) AS.
 */
static int
tp_send_aspt_aspia_req(struct tp *tp, queue_t *q, mblk_t *msg, uint32_t *asid,
		       caddr_t tptr, size_t tlen, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (tptr ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) + (iptr ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (asid) {
#if M2UA
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
#else
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
#endif
			p[1] = htonl(*asid);
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
 * rp_send_aspt_aspia_req: - send an ASP Inactive Request
 * @rp: AS-SGP relation pointer
 * @q: active queue
 * @msg: message to free upon success
 * @iptr: info pointer
 * @ilen: info length
 *
 * This is used to deactivate an AS.  Users are deactivated when they are disabled by the User
 * disable/disconnect.  They are activate when they are enabled by the User enable/connect.
 */
static int
rp_send_aspt_aspia_req(struct rp *rp, queue_t *q, mblk_t *msg, caddr_t iptr, size_t ilen)
{
	struct tp *tp = rp->gp.gp->xp.xp;
	struct as *as = rp->as.as;
	int err;
	mblk_t *mp;
#if M2UA
	size_t tlen = strnlen(as->as.addr.iid_text, 32);
#endif
	size_t mlen = UA_MHDR_SIZE +
	    (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
#if M2UA
	    (as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
#endif
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.asid) {
#if M2UA
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
#else
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
#endif
			p[1] = htonl(as->as.asid);
			p += 2;
		}
#if M2UA
		if (as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
#endif
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;
		if (as->as.options.tack) {
			strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE,
			       "-> WACK ASPIA START <- (%u msec)", as->as.options.tack);
			if (rp->rp.wack_aspia)
				mi_timer(tp->rq, rp->rp.wack_aspia, as->as.options.tack);
		}

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "ASPT ASPIA Req ->");
		/* always sent on same stream as data */
		if (!(err = rp_set_state(rp, q, AS_WACK_ASPIA)) &&
		    !(err = t_optdata_req(tp, q, msg, 0, as->as.streamid, mp)))
			return (0);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * rp_send_rkmm_reg_req: - send REG REQ
 * @rp: AS-SGP relation pointer
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
rp_send_rkmm_reg_req(struct rp *rp, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_LINK_KEY);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;
		struct tp *tp = rp->gp.gp->xp.xp;
		struct as *as = rp->as.as;

		p[0] = UA_RKMM_REG_REQ;
		p[1] = htonl(mlen);
		/* FIXME: make this for other UAs too */
		p[2] = M2UA_PARM_LINK_KEY;
		p[3] = M2UA_PARM_LOC_KEY_ID;
		p[4] = htonl((rp->request_id = ++rp->gp.gp->gp.request_id));
		p[5] = M2UA_PARM_SDTI;
		p[6] = htonl(as->as.addr.sdti);
		p[7] = M2UA_PARM_SDLI;
		p[8] = htonl(as->as.addr.sdli);
		p += 9;
		mp->b_wptr = (unsigned char *) p;

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "RKMM REG Req ->");
		if (!(err = rp_set_state(rp, q, ASP_WACK_ASPUP)) &&
		    !(err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp)))
			return (0);
		freeb(mp);
		rp_set_state(rp, q, AS_DOWN);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * rp_send_rkmm_dereg_req: - send DEREG REQ
 * @rp: AS-SGP relation pointer
 * @q: active queue
 * @msg: message to free upon success
 */
static int
rp_send_rkmm_dereg_req(struct rp *rp, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
#if M2UA
	size_t tlen = strnlen(as->as.addr.iid_text, 32);
#endif
	size_t mlen = UA_MHDR_SIZE +
#if M2UA
	    (as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
#endif
	    (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;
		struct tp *tp = rp->gp.gp->xp.xp;
		struct as *as = rp->as.as;

		p[0] = UA_RKMM_DEREG_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.asid) {
#if M2UA
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
#else
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
#endif
			p[1] = htonl(as->as.asid);
			p += 2;
		}
#if M2UA
		if (as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
#endif
		mp->b_wptr = (unsigned char *) p;

		strlog(tp->mid, tp->sid, UALOGTX, SL_TRACE, "RKMM DEREG Req ->");
		if (!(err = rp_set_state(rp, q, ASP_WACK_ASPDN)) &&
		    !(err = t_optdata_req(tp, q, msg, T_ODF_EX, 0, mp)))
			return (0);
		rp_set_state(rp, q, AS_INACTIVE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

#if M2UA
/**
 * as_send_maup_data1: - send DATA1
 * @up: UP private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static inline fastcall __hot_write int
as_send_maup_data1(struct up *up, struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t tlen = strnlen(up->as.as->as.addr.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) + UA_PHDR_SIZE;

	if (unlikely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p += 2;
		if (up->as.as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.as->as.asid);
			p += 2;
		}
		if (up->as.as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		*p++ = UA_PHDR(M2UA_PARM_DATA1, dlen);
		mp->b_wptr = (unsigned char *) p;

		mp->b_cont = dp->b_cont;
		strlog(up->mid, up->sid, UALOGDA, SL_TRACE, "MAUP DATA1 ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, up->as.as->as.streamid, mp))))
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
as_send_maup_data2(struct up *up, struct tp *tp, queue_t *q, mblk_t *msg, mblk_t *dp, uint8_t pri)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp->b_cont);
	size_t tlen = strnlen(up->as.as->as.addr.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) + UA_PHDR_SIZE + 1;

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p += 2;
		if (up->as.as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.as->as.asid);
			p += 2;
		}
		if (up->as.as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		*p++ = UA_PHDR(M2UA_PARM_DATA2, dlen + 1);
		*(unsigned char *) p = pri;
		mp->b_wptr = (unsigned char *) p + 1;

		mp->b_cont = dp->b_cont;
		strlog(up->mid, up->sid, UALOGDA, SL_TRACE, "MAUP DATA2 ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, up->as.as->as.streamid, mp))))
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
as_send_maup_estab_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.as->as.addr.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_ESTAB_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.as->as.asid);
			p += 2;
		}
		if (up->as.as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		mp->b_wptr = (unsigned char *) p;

		dl_set_state(up, DL_OUTCON_PENDING);

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "MAUP ESTAB Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, up->as.as->as.streamid, mp))))
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
as_send_maup_rel_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *msg)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.as->as.addr.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.as->as.asid);
			p += 2;
		}
		if (up->as.as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		mp->b_wptr = (unsigned char *) p;

		switch (dl_get_state(up)) {
		case DL_OUTCON_PENDING:
			dl_set_state(up, DL_DISCON8_PENDING);
			break;
		case DL_INCON_PENDING:
			dl_set_state(up, DL_DISCON9_PENDING);
			break;
		case DL_DATAXFER:
			dl_set_state(up, DL_DISCON11_PENDING);
			break;
		case DL_USER_RESET_PENDING:
			dl_set_state(up, DL_DISCON12_PENDING);
			break;
		case DL_PROV_RESET_PENDING:
			dl_set_state(up, DL_DISCON13_PENDING);
			break;
		default:
			/* software error */
			dl_set_state(up, -1);
			break;
		}

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "MAUP REL Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, up->as.as->as.streamid, mp))))
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
as_send_maup_state_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *msg,
		       const uint32_t request)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.as->as.addr.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
	    UA_SIZE(M2UA_PARM_STATE_REQUEST);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_STATE_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.as->as.asid);
			p += 2;
		}
		if (up->as.as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		p[0] = M2UA_PARM_STATE_REQUEST;
		p[1] = htonl(request);
		p += 2;
		mp->b_wptr = (unsigned char *) p;

		up->status.pending |= (1 << request);
		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "MAUP STATE Req ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, up->as.as->as.streamid, mp)))) {
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
as_send_maup_retr_req(struct up *up, struct tp *tp, queue_t *q, mblk_t *msg, uint32_t *fsnc)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.as->as.addr.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
	    UA_SIZE(M2UA_PARM_ACTION) + (fsnc ? UA_SIZE(M2UA_PARM_SEQNO) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_RETR_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.as->as.asid);
			p += 2;
		}
		if (up->as.as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.as->as.addr.iid_text, p, tlen);
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
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, up->as.as->as.streamid, mp))))
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
as_send_maup_data_ack(struct up *up, struct tp *tp, queue_t *q, mblk_t *msg, uint32_t corid)
{
	int err;
	mblk_t *mp;
	size_t tlen = strnlen(up->as.as->as.addr.iid_text, 32);
	size_t mlen =
	    UA_MHDR_SIZE + (up->as.as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (up->as.as->as.addr.iid_text[0] ? UA_PHDR_SIZE + UA_PAD4(tlen) : 0) +
	    UA_SIZE(M2UA_PARM_CORR_ID_ACK);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (up->as.as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_IID, sizeof(uint32_t));
			p[1] = htonl(up->as.as->as.asid);
			p += 2;
		}
		if (up->as.as->as.addr.iid_text[0]) {
			*p++ = UA_PHDR(UA_PARM_IID_TEXT, tlen);
			bcopy(up->as.as->as.addr.iid_text, p, tlen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(tlen));
		}
		p[0] = M2UA_PARM_CORR_ID_ACK;
		p[1] = htonl(corid);
		p += 2;
		mp->b_wptr = (unsigned char *) p;

		strlog(up->mid, up->sid, UALOGTX, SL_TRACE, "MAUP DATA Ack ->");
		if (unlikely((err = t_optdata_req(tp, q, msg, 0, up->as.as->as.streamid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
#endif

#if M3UA
/**
 * as_send_xfer_data: - send XFER DATA
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @orig: originating address
 * @dest: destination address
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static int
as_send_xfer_data(struct as *as, queue_t *q, mblk_t *msg, struct tcap_addr *orig,
		  struct tcap_addr *dest, uint8_t pri, uint8_t sls, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp) + 3 * sizeof(uint32_t);
	size_t mlen = UA_MHDR_SIZE + (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) + UA_PHDR_SIZE;

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_XFER_DATA;
		p[1] = htonl(mlen + dlen);
		p += 2;
		if (as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(as->as.asid);
			p += 2;
		}
		p[0] = UA_PHDR(M3UA_PARM_PROT_DATA3, dlen);
		p[1] = htonl(orig->pc);
		p[2] = htonl(dest->pc);
		p[3] = htonl(((uint32_t) dest->si << 24) |
			     ((uint32_t) dest->ni << 16) |
			     ((uint32_t) pri << 8) | ((uint32_t) sls << 0)
		    );
		p += 4;
		mp->b_wptr = (unsigned char *) p;
		mp->b_cont = dp;

		strlog(as->tp.tp->mid, as->tp.tp->sid, UALOGDA, SL_TRACE, "XFER DATA ->");
		if (unlikely((err = t_optdata_req(as->tp.tp, q, msg, 0, as->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_snmm_daud: - send SNMM DAUD
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @apc: affected point code
 * @iptr: info pointer
 * @ilen: info length
 */
static int
as_send_snmm_duna(struct as *as, queue_t *q, mblk_t *msg, uint32_t apc, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    UA_PDHR_SIZE + sizeof(uint32_t) + (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_SNMM_DAUD;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(as->as.asid);
			p += 2;
		}
		p[0] = UA_PHDR(UA_PARM_APC, sizeof(uint32_t));
		p[1] = htonl(apc);
		p += 2;
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(as->tp.tp->mid, as->tp.tp->sid, UALOGTX, SL_TRACE, "SNMM DAUD ->");
		if (unlikely((err = t_optdata_req(as->tp.tp, q, msg, 0, as->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_snmm_scon: - send SNMM SCON
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @apc: affected point code
 * @cpc: concerned point code
 * @cong: congestion level or NULL
 * @iptr: info pointer
 * @ilen: info length
 */
static int
as_send_snmm_scon(struct as *as, queue_t *q, mblk_t *msg, uint32_t apc,
		  uint32_t cpc, uint32_t *cong, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (as->as.asid ? UA_PDHR_SIZE + sizeof(uint32_t) : 0) +
	    UA_PHDR_SIZE + sizeof(uint32_t) +
	    UA_PHDR_SIZE + sizeof(uint32_t) +
	    (cong ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely(mp = ua_allocb(q, mlen, BPRI_MED))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_SNMM_SCON;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(as->as.asid);
			p += 2;
		}
		p[0] = UA_PHDR(UA_PARM_APC, sizeof(uint32_t));
		p[1] = htonl(apc);
		p[2] = UA_PHDR(M3UA_PARM_CONCERN_DEST, sizeof(uint32_t));
		p[3] = htonl(cpc);
		p += 4;
		if (cong) {
			p[0] = UA_PHDR(M3UA_PARM_CONG_IND, sizeof(uint32_t));
			p[1] = htonl(*cong);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(as->tp.tp->mid, as->tp.tp->sid, UALOGTX, SL_TRACE, "SNMM SCON ->");
		if (unlikely((err = t_optdata_req(as->tp.tp, q, msg, 0, as->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
#endif

#if SUA
/**
 * as_send_xfer_data: - send XFER DATA
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @orig: originating address
 * @dest: destination address
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static int
as_send_xfer_data(struct as *as, queue_t *q, mblk_t *msg, struct sccp_addr *orig,
		  struct sccp_addr *dest, uint8_t pri, uint8_t sls, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp) + 3 * sizeof(uint32_t);
	size_t mlen = UA_MHDR_SIZE + (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) + UA_PHDR_SIZE;

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_XFER_DATA;
		p[1] = htonl(mlen + dlen);
		p += 2;
		if (as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(as->as.asid);
			p += 2;
		}
		p[0] = UA_PHDR(M3UA_PARM_PROT_DATA3, dlen);
		p[1] = htonl(orig->pc);
		p[2] = htonl(dest->pc);
		p[3] = htonl(((uint32_t) dest->si << 24) |
			     ((uint32_t) dest->ni << 16) |
			     ((uint32_t) pri << 8) | ((uint32_t) sls << 0)
		    );
		p += 4;
		mp->b_wptr = (unsigned char *) p;
		mp->b_cont = dp;

		strlog(as->tp.tp->mid, as->tp.tp->sid, UALOGDA, SL_TRACE, "XFER DATA ->");
		if (unlikely((err = t_optdata_req(as->tp.tp, q, msg, 0, as->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_snmm_daud: - send SNMM DAUD
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @apc: affected point code
 * @iptr: info pointer
 * @ilen: info length
 */
static int
as_send_snmm_duna(struct as *as, queue_t *q, mblk_t *msg, uint32_t apc, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    UA_PDHR_SIZE + sizeof(uint32_t) + (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_SNMM_DAUD;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(as->as.asid);
			p += 2;
		}
		p[0] = UA_PHDR(UA_PARM_APC, sizeof(uint32_t));
		p[1] = htonl(apc);
		p += 2;
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(as->tp.tp->mid, as->tp.tp->sid, UALOGTX, SL_TRACE, "SNMM DAUD ->");
		if (unlikely((err = t_optdata_req(as->tp.tp, q, msg, 0, as->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_snmm_scon: - send SNMM SCON
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @apc: affected point code
 * @cpc: concerned point code
 * @cong: congestion level or NULL
 * @iptr: info pointer
 * @ilen: info length
 */
static int
as_send_snmm_scon(struct as *as, queue_t *q, mblk_t *msg, uint32_t apc,
		  uint32_t cpc, uint32_t *cong, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (as->as.asid ? UA_PDHR_SIZE + sizeof(uint32_t) : 0) +
	    UA_PHDR_SIZE + sizeof(uint32_t) +
	    UA_PHDR_SIZE + sizeof(uint32_t) +
	    (cong ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely(mp = ua_allocb(q, mlen, BPRI_MED))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_SNMM_SCON;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(as->as.asid);
			p += 2;
		}
		p[0] = UA_PHDR(UA_PARM_APC, sizeof(uint32_t));
		p[1] = htonl(apc);
		p[2] = UA_PHDR(M3UA_PARM_CONCERN_DEST, sizeof(uint32_t));
		p[3] = htonl(cpc);
		p += 4;
		if (cong) {
			p[0] = UA_PHDR(M3UA_PARM_CONG_IND, sizeof(uint32_t));
			p[1] = htonl(*cong);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(as->tp.tp->mid, as->tp.tp->sid, UALOGTX, SL_TRACE, "SNMM SCON ->");
		if (unlikely((err = t_optdata_req(as->tp.tp, q, msg, 0, as->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
#endif

#if TUA
/**
 * as_send_xfer_data: - send XFER DATA
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @orig: originating address
 * @dest: destination address
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static int
as_send_xfer_data(struct as *as, queue_t *q, mblk_t *msg, struct tcap_addr *orig,
		  struct tcap_addr *dest, uint8_t pri, uint8_t sls, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp) + 3 * sizeof(uint32_t);
	size_t mlen = UA_MHDR_SIZE + (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) + UA_PHDR_SIZE;

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_XFER_DATA;
		p[1] = htonl(mlen + dlen);
		p += 2;
		if (as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(as->as.asid);
			p += 2;
		}
		p[0] = UA_PHDR(M3UA_PARM_PROT_DATA3, dlen);
		p[1] = htonl(orig->pc);
		p[2] = htonl(dest->pc);
		p[3] = htonl(((uint32_t) dest->si << 24) |
			     ((uint32_t) dest->ni << 16) |
			     ((uint32_t) pri << 8) | ((uint32_t) sls << 0)
		    );
		p += 4;
		mp->b_wptr = (unsigned char *) p;
		mp->b_cont = dp;

		strlog(as->tp.tp->mid, as->tp.tp->sid, UALOGDA, SL_TRACE, "XFER DATA ->");
		if (unlikely((err = t_optdata_req(as->tp.tp, q, msg, 0, as->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_snmm_daud: - send SNMM DAUD
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @apc: affected point code
 * @iptr: info pointer
 * @ilen: info length
 */
static int
as_send_snmm_duna(struct as *as, queue_t *q, mblk_t *msg, uint32_t apc, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (as->as.asid ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    UA_PDHR_SIZE + sizeof(uint32_t) + (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely((mp = ua_allocb(q, mlen, BPRI_MED)))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_SNMM_DAUD;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(as->as.asid);
			p += 2;
		}
		p[0] = UA_PHDR(UA_PARM_APC, sizeof(uint32_t));
		p[1] = htonl(apc);
		p += 2;
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(as->tp.tp->mid, as->tp.tp->sid, UALOGTX, SL_TRACE, "SNMM DAUD ->");
		if (unlikely((err = t_optdata_req(as->tp.tp, q, msg, 0, as->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * as_send_snmm_scon: - send SNMM SCON
 * @as: AS structure
 * @q: active queue
 * @msg: message to free upon success
 * @apc: affected point code
 * @cpc: concerned point code
 * @cong: congestion level or NULL
 * @iptr: info pointer
 * @ilen: info length
 */
static int
as_send_snmm_scon(struct as *as, queue_t *q, mblk_t *msg, uint32_t apc,
		  uint32_t cpc, uint32_t *cong, caddr_t iptr, size_t ilen)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE +
	    (as->as.asid ? UA_PDHR_SIZE + sizeof(uint32_t) : 0) +
	    UA_PHDR_SIZE + sizeof(uint32_t) +
	    UA_PHDR_SIZE + sizeof(uint32_t) +
	    (cong ? UA_PHDR_SIZE + sizeof(uint32_t) : 0) +
	    (ilen ? UA_PHDR_SIZE + UA_PAD4(ilen) : 0);

	if (likely(mp = ua_allocb(q, mlen, BPRI_MED))) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_SNMM_SCON;
		p[1] = htonl(mlen);
		p += 2;
		if (as->as.asid) {
			p[0] = UA_PHDR(UA_PARM_RC, sizeof(uint32_t));
			p[1] = htonl(as->as.asid);
			p += 2;
		}
		p[0] = UA_PHDR(UA_PARM_APC, sizeof(uint32_t));
		p[1] = htonl(apc);
		p[2] = UA_PHDR(M3UA_PARM_CONCERN_DEST, sizeof(uint32_t));
		p[3] = htonl(cpc);
		p += 4;
		if (cong) {
			p[0] = UA_PHDR(M3UA_PARM_CONG_IND, sizeof(uint32_t));
			p[1] = htonl(*cong);
			p += 2;
		}
		if (ilen) {
			*p++ = UA_PHDR(UA_PARM_INFO, ilen);
			bcopy(iptr, p, ilen);
			p = (uint32_t *) ((caddr_t) p + UA_PAD4(ilen));
		}
		mp->b_wptr = (unsigned char *) p;

		strlog(as->tp.tp->mid, as->tp.tp->sid, UALOGTX, SL_TRACE, "SNMM SCON ->");
		if (unlikely((err = t_optdata_req(as->tp.tp, q, msg, 0, as->streamid, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  SGP to ASP UA protocol messages
 *  =========================================================================
 */

/**
 * rp_lookup: - lookup up RP from message
 * @gp: SGP from which message arrived
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static struct rp *
rp_lookup(struct gp *gp, queue_t *q, mblk_t *mp, int *errp)
{
	struct ua_parm asid = { {NULL,}, };
	struct rp *rp = NULL;

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &asid, UA_PARM_IID)) {

		for (rp = gp->rp.list; rp; rp = rp->gp.next)
			if (rp->as.as->as.asid == asid.val)
				goto found;
	}
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &asid, UA_PARM_IID_TEXT)) {
		for (rp = gp->rplist; rp; rp = rp->sp.next)
			if (strncmp(asid.cp, (caddr_t) &rp->as.as->as.asid, min(asid.len, 32)) ==
			    0)
				goto found;
	}
	/* if there is precisely one AS, we do not need IID to identify the AS */
	if ((rp = gp->rp.list) && !(rp->gp.next))
		goto found;

	if (errp)
		*errp = -ENOENT;	/* could not find AS */
	return (NULL);
      found:
	if (errp)
		*errp = 0;
	return (rp);
}

/**
 * rp_recv_mgmt_err: - receive MGMT ERR message
 * @rp: RP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Error messages pertaining to an AS or AS state must contain an RC/IID.
 */
static int
rp_recv_mgmt_err(struct rp *rp, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * gp_recv_mgmt_err: - receive MGMT ERR message
 * @gp: SGP from which message arrived
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
gp_recv_mgmt_err(struct gp *gp, queue_t *q, mblk_t *mp)
{
	struct rp *rp;

	if ((rp = rp_lookup(gp, q, mp, NULL)))
		return rp_recv_mgmt_err(rp, q, mp);

	/* FIXME: process as without RC/IID */
	freemsg(mp);
	return (0);
}

/**
 * rp_recv_mgmt_ntfy: - receive MGMT NTFY message
 * @rp: AS-SGP relation pointer
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Notification messages pertaning to an AS or AS state change must contain an IID.
 */
static int
rp_recv_mgmt_ntfy(struct rp *rp, queue_t *q, mblk_t *mp)
{
	struct ua_parm status;
	int ecode;
	int err = 0;

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
		   generate and UP_EVENT_IND with an appropriate event. */
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
		switch (rp_get_state(rp)) {
		case AS_INACTIVE:
		case ASP_WACK_ASPUP:
		case ASP_WACK_ASPDN:
		case AS_DOWN:
			goto discard;
		case AS_WACK_ASPIA:
			if ((err = rp_set_state(rp, q, AS_INACTIVE)))
				return (err);
			goto outstate;
		case AS_WACK_ASPAC:
			if ((err = rp_set_state(rp, q, AS_INACTIVE)))
				return (err);
			goto outstate;
		case AS_ACTIVE:
			if ((err = rp_set_state(rp, q, AS_INACTIVE)))
				return (err);
			goto outstate;
		default:
			goto outstate;
		}
	      outstate:
		strlog(DRV_ID, sp->sid, 0, SL_TRACE, "Alternate ASP Active in invalid state %u",
		       rp_get_state(rp));
		if ((err = rp_set_state(rp, q, AS_DOWN)))
			return (err);
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
	return (ecode);
}

/**
 * gp_recv_mgmt_ntfy: - receive MGMT NTFY message
 * @gp: SGP from which message arrived
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
gp_recv_mgmt_ntfy(struct gp *gp, queue_t *q, mblk_t *mp)
{
	struct ua_parm status;
	struct rp *rp;
	int ecode;

	if ((rp = rp_lookup(gp, q, mp, NULL)))
		return rp_recv_mgmt_ntfy(rp, q, mp);

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
	/* FIXME: process as without RC/IID */
      discard:
	freemsg(mp);
	return (0);
      missing_parm:
	ecode = UA_ECODE_MISSING_PARAMETER;
	goto error;
      error:
	return (ecode);
}

/**
 * gp_recv_asps_hbeat_req: - process BEAT Request
 * @gp: SGP from which message was received
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
gp_recv_asps_hbeat_req(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct ua_parm hbeat;
	struct up *up;

	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &hbeat, UA_PARM_HBDATA))
		return (-EINVAL);

	if ((rp = rp_lookup(gp, q, mp, NULL)))
		return rp_send_asps_hbeat_ack(rp, tp, q, mp, hbeat.cp, hbeat.len);

	/* process as normal */
	return gp_send_asps_hbeat_ack(gp, tp, q, mp, hbeat.cp, hbeat.len);
}

/**
 * gp_recv_asps_aspup_ack: - process ASP Up Ack message
 * @gp: SGP from which message was received
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
gp_recv_asps_aspup_ack(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct ua_parm parm = { {NULL,}, };
	int ecode;
	struct sg *sg = gp->sg.sg;
	struct sp *sp = sg->sp.sp;
	int err;

	switch (gp_get_state(gp)) {
	case ASP_UP:
		/* Unsolicited: about the only time that an unsolicited ASP Up message is expected
		   is when the SG updates options */
	case ASP_WACK_ASPUP:
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &parm, UA_PARM_ASPID))
			sp->sp.options.aspid = parm.val;
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &parm, UA_PARM_ASPEXT)) {
			uint32_t *p = parm.wp;
			uint popt = 0, opt;

			if (parm.len < sizeof(uint32_t))
				goto field_error;

			while (p < ((uint32_t *) parm.cp))
				if ((opt = *p++) >= UA_ASPEXT_NONE || UA_ASPEXT_ASPCONG >= opt)
					popt |= (1 << opt);
			sg->sg.options.options.popt &= ~((1 << (UA_ASPEXT_ASPCONG + 1)) - 1);
			sg->sg.options.options.popt |= popt;
		}
		break;
	default:
		goto outstate;
	}
	if (gp_get_state(gp) == ASP_WACK_ASPUP) {
		/* Solicited */
		if ((err = gp_set_state(gp, q, ASP_UP)))
			return (err);
		mi_timer_stop(gp->gp.wack_aspup);
	}
	freemsg(mp);
	return (0);
      field_error:
	ecode = UA_ECODE_PARAMETER_FIELD_ERROR;
	goto error;
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_TRACE, "ASPUP Ack in unexpected state %u", gp_get_state(gp));
	ecode = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	return (ecode);
}

/**
 * gp_recv_asps_aspdn_ack: - process ASP Down Ack message
 * @gp: SGP from which message was received
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
gp_recv_asps_aspdn_ack(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err;

	if (gp_get_state(gp) != ASP_DOWN) {
		if ((err = gp_set_state(gp, q, ASP_DOWN)))
			return (err);
		mi_timer_stop(gp->gp.wack_aspup);
		mi_timer_stop(gp->gp.wack_aspdn);
	}
	freemsg(mp);
	return (0);
}

/**
 * rp_recv_asps_hbeat_ack: - receive BEAT Ack message
 * @rp: AS-SGP relation pointer
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * Note that BEAT Ack messages in reply to a BEAT message that contains an RC/IID should also contain
 * an RC/IID.  This is not strictly to spec, but it is what the OpenSS7 stack does.
 */
static int
rp_recv_asps_hbeat_ack(struct rp *rp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * gp_recv_asps_hbeat_ack: - receive BEAT Ack message
 * @gp: SGP from which message was received
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
gp_recv_asps_hbeat_ack(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct rp *rp;

	if ((rp = rp_lookup(gp, q, mp, NULL)))
		return rp_recv_asps_hbeat_ack(rp, tp, q, mp);

	/* If the message does not include an IID, then perhaps the IID is contained in the
	   heartbeat data.  The OpenSS7 stack formats heatbeat data with TLVs in the same manner as
	   normal messages. */
	/* FIXME: process as without IID */
	freemsg(mp);
	return (0);
}

/**
 * rp_recv_aspt_aspac_ack: - receive ASPAC Ack message
 * @rp: AS-SGP relation pointer
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
rp_recv_aspt_aspac_ack(struct rp *rp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct as *as = rp->as.as;
	struct sp *sp = as->sp.sp;
	struct up *up = as->up.up;
	struct ua_parm parm = { {NULL,}, };
	int err;

	if (rp_not_state(rp, ((1 << AS_ACTIVE) | (1 << AS_WACK_ASPAC))))
		return (UA_ECODE_UNEXPECTED_MESSAGE);
	/* There was some discussion on the SIGTRAN WG mailing list about what an ASP should do if
	   the SGP returns a traffic mode that was not specified in the ASP Active, or if the SGP
	   returns a traffic mode when none was specified in the ASP Active. */
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &parm, UA_PARM_TMODE))
		as->as.addr.tmode = parm.val;
	if ((sp->sp.options.options.popt & UA_SGINFO) &&
	    ua_dec_parm(mp->b_rptr, mp->b_wptr, &parm, UA_PARM_PROTO_LIMITS)) {
		/* update protocol limits information per draft-bidulock-sigtran-sginfo */
		(void) up;
#if M2UA
		if (parm.len != 2 * sizeof(uint32_t))
			return (UA_ECODE_PARAMETER_FIELD_ERROR);
		as->as.options.max_sdu = ntohl(parm.wp[0]);
		as->as.options.opt_sdu = ntohl(parm.wp[1]);
		if (up) {
			up->up.info.lmi_max_sdu = as->as.options.max_sdu;
			up->up.info.lmi_min_sdu = as->as.options.opt_sdu;
		}
#endif
#if M3UA
		/* FIXME: do this for M3UA */
#endif
#if SUA
		/* FIXME: do this for SUA */
#endif
#if TUA
		/* FIXME: do this for TUA */
#endif
#if ISUA
		/* FIXME: do this for ISUA */
#endif
	}
	if ((err = rp_set_state(rp, q, AS_ACTIVE)))
		return (err);
	mi_timer_stop(rp->rp.wack_aspac);
	freemsg(mp);
	return (0);
}

/**
 * gp_recv_aspt_aspac_ack: - receive ASPAC Ack message
 * @gp: SGP from which the message was received
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
gp_recv_aspt_aspac_ack(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct rp *rp;
	int ecode;

	if ((rp = rp_lookup(gp, q, mp, NULL)))
		return rp_recv_aspt_aspac_ack(rp, tp, q, mp);

	mi_timer_stop(gp->gp.wack_aspac);
	goto protocol_error;
	/* An ASPAC Ack with no RC/IID is supposed to pertain to all AS configurated for the ASP.
	   However, we never send an ASPAC Req without an RC/IID and so it is an error to send an
	   ASPAC Ack without an RC/IID. */
      protocol_error:
	ecode = UA_ECODE_PROTOCOL_ERROR;
	goto error;
      error:
	return (ecode);
}

/**
 * rp_recv_aspt_aspia_ack: - receive ASPIA Ack message
 * @rp: AS-SGP relation pointer
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * ASPIA Acks with an AS id can be either solicited or unsolicited.  Unsolicited ASPIA Acks are
 * detected when they are received in the incorrect state.  Solicited ASPIA Acks are received in the
 * AS_WACK_ASPIA state.   Nevertheless, we may have sent multiple ASPIA Requests if there was an
 * ack timeout.  Multiple ASPIA Acks may be returned.  Therefore, if we are already in an
 * AS_INACTIVE state, the messge is ignored.
 */
static int
rp_recv_aspt_aspia_ack(struct rp *rp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	int err;

	if ((err = rp_set_state(rp, q, AS_INACTIVE)))
		return (err);
	mi_timer_stop(rp->rp.wack_aspia);
	mi_timer_stop(rp->rp.wack_aspac);
	freemsg(mp);
	return (0);
}

/**
 * gp_recv_aspt_aspia_ack: - receive ASPIA Ack message
 * @gp: SGP from which message was received
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * There are two types of ASPIA Acks: solicited and unsolicited.  An unsolicited ASPIA Ack indicates
 * some form of isolation between the UA and NIF at the SG.  Solicited ASPIA Acks have a
 * corresponding ASPIA Req.  Because we never send ASPIA Req without an AS id, receiving a solicited
 * ASPIA Ack without an AS id is an error.  ASPIA Acks without an AS id can therefore always be
 * considered to be unsolicited.
 */
static int
gp_recv_aspt_aspia_ack(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct rp *rp;
	int err;

	if ((rp = rp_lookup(gp, q, mp, NULL)))
		return rp_recv_aspt_aspia_ack(rp, tp, q, mp);

	/* ASPIA Acks without AS ids are always unsolicited.  Also, ASPIA Acks without AS ids apply 
	   to all AS served. */
	for (rp = gp->rp.list; rp; rp = rp->gp.next) {
		if ((err = rp_set_state(rp, q, AS_INACTIVE)))
			return (err);
		mi_timer_stop(rp->rp.wack_aspia);
		mi_timer_stop(rp->rp.wack_aspac);
	}
	freemsg(mp);
	return (0);
}

#if M3UA
/**
 * as_recv_xfer_data: - receive XFER DATA message
 * @as: AS structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static inline __hot_in int
as_recv_xfer_data(struct as *as, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm data;
	struct mtp_addr srce;
	int pri, sls;
	unsigned char *b_rptr = mp->b_rptr;

	if (up_get_state(as->up.up) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &data, M3UA_PARM_PROT_DATA3))
		goto missing;
	srce.pc = ntohl(data.wp[0]);
	srce.ni = (ntohl(data.wp[2]) >> 16) & 0xff;
	srce.si = (htohl(data.wp[2]) >> 24) & 0xff;
	pri = (htonl(data.wp[2]) >> 8) & 0xff;
	sls = (htonl(data.wp[2]) >> 0) & 0xff;
	mp->b_rptr = (unsigned char *) &data.wp[3];
	if ((err = mtp_transfer_ind(as->up.up, q, mp, &srce, pri, sls, mp)) == 0)
		return (0);
	mp->b_rptr = b_rptr;
	return (err);
      missing:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR,
	       "XFER DATA missing PROT DATA3 parameter");
	err = UA_ECODE_MISSING_PARAMETER;
	goto error;
      outstate:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_TRACE, "XFER DATA in unexpected AS state %s",
	       as_u_state_name(as));
	err = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	freemsg(mp);
	return (err);

}

/**
 * tp_recv_xfer_data: - receive XFER DATA message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static inline __hot_in int
tp_recv_xfer_data(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct as *as;

	if ((as = rp_lookup(tp, q, mp, NULL)))
		return as_recv_xfer_data(as, tp, q, mp);
	return (-ENOENT);
}
#endif

#if TUA
/* FIXME: SNMM for TUA */
#endif

#if M3UA || SUA
/**
 * as_recv_snmm_duna: - receive SNMM DUNA message
 * @as: AS structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_snmm_duna(struct as *as, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc = { {NULL,}, };
	struct mtp_addr addr;

	if (up_get_state(as->up.up) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing;
	addr.pc = apc.val;
	addr.si = 0;
	addr.ni = 0;
	/* FIXME: sanity check value */
	return mtp_pause_ind(as->up.up, q, mp, &addr);
      missing:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM DUNA missing APC parameter");
	err = -ENXIO;		/* missing mandatory parameter */
	goto error;
      outstate:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM DUNA in unexpected AS state %s",
	       as_u_state_name(as));
	err = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * as_recv_snmm_dava: - receive SNMM DAVA message
 * @as: AS structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_snmm_dava(struct as *as, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc = { {NULL,}, };
	struct mtp_addr addr;

	if (up_get_state(as->up.up) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing;
	addr.pc = apc.val;
	addr.si = 0;
	addr.ni = 0;
	/* FIXME: sanity check value */
	return mtp_resume_ind(as->up.up, q, mp, &addr);
      missing:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM DAVA missing APC parameter");
	err = UA_ECODE_MISSING_PARAMETER;
	goto error;
      outstate:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM DAVA in unexpected AS state %s",
	       as_u_state_name(as));
	err = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * as_recv_snmm_scon: - receive SNMM SCON message
 * @as: AS structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_snmm_scon(struct as *as, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc, cong;
	struct mtp_addr addr;
	int status = MTP_STATUS_CONGESTION;

	if (up_get_state(as->up.up) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing;
	addr.pc = apc.val;
	addr.si = 0;
	addr.ni = 0;
	/* FIXME: sanity check value */
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &cong, M3UA_PARM_CONG_IND)) {
		switch (cong.val) {
		case 0:
			status = MTP_STATUS_CONGESTION_LEVEL0;
			break;
		case 1:
			status = MTP_STATUS_CONGESTION_LEVEL1;
			break;
		case 2:
			status = MTP_STATUS_CONGESTION_LEVEL2;
			break;
		case 3:
			status = MTP_STATUS_CONGESTION_LEVEL3;
			break;
		default:
			/* really an error */
			status = MTP_STATUS_CONGESTION;
			break;
		}
	}
	return mtp_status_ind(as->up.up, q, mp, &addr, MTP_STATUS_TYPE_CONG, status);
      missing:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM SCON missing APC parameter");
	err = UA_ECODE_MISSING_PARAMETER;
	goto error;
      outstate:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM SCON in unexpected AS state %s",
	       as_u_state_name(as));
	err = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * as_recv_snmm_dupu: - receive SNMM DUPU message
 * @as: AS structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_snmm_dupu(struct as *as, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc, cause;
	struct mtp_addr addr;
	int status = MTP_STATUS_UPU_UNKNOWN;

	if (up_get_state(as->up.up) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing_apc;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &cause, M3UA_PARM_USER_CAUSE))
		goto missing_user_cause;
	addr.pc = apc.val;
	addr.si = (cause.val & 0xffff);
	addr.ni = 0;
	/* FIXME: sanity check value */
	switch (cause.val >> 16) {
	default:
	case 0:
		status = MTP_STATUS_UPU_UNKNOWN;
		break;
	case 1:
		status = MTP_STATUS_UPU_UNEQUIPPED;
		break;
	case 2:
		status = MTP_STATUS_UPU_INACCESSIBLE;
		break;
	}
	return mtp_status_ind(as->up.up, q, mp, &addr, MTP_STATUS_TYPE_UPU, status);
      missing_user_cause:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR,
	       "SNMM DUPU missing USER/CAUSE parameter");
	err = -ENXIO;		/* missing mandatory parameter */
	goto error;
      missing_apc:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM DUPU missing APC parameter");
	err = UA_ECODE_MISSING_PARAMETER;
	goto error;
      outstate:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM DUPU in unexpected AS state %s",
	       as_u_state_name(as));
	err = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	freemsg(mp);
	return (err);
}

/**
 * as_recv_snmm_drst: - receive SNMM DRST message
 * @as: AS structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
as_recv_snmm_drst(struct as *as, queue_t *q, mblk_t *mp)
{
	int err;
	struct ua_parm apc = { {NULL,}, };
	struct mtp_addr addr;

	if (up_get_state(as->up.up) == MTPS_UNBND)
		goto outstate;
	if (!ua_dec_parm(mp->b_rptr, mp->b_wptr, &apc, UA_PARM_APC))
		goto missing;
	addr.pc = apc.val;
	addr.si = 0;
	addr.ni = 0;
	/* FIXME: sanity check value */
	return mtp_status_ind(as->up.up, q, mp, &addr, MTP_STATUS_TYPE_RSTR, 0);
      missing:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM DRST missing APC parameter");
	err = UA_ECODE_MISSING_PARAMETER;
	goto error;
      outstate:
	strlog(as->tp.tp->mid, as->tp.tp->sid, 0, SL_ERROR, "SNMM DRST in unexpected AS state %s",
	       as_u_state_name(as));
	err = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	freemsg(mp);
	return (err);
}
#endif

#if M2UA
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

	if (dl_get_state(up) != DL_OUTCON_PENDING)
		goto outstate;
	dl_set_state(up, DL_DATAXFER);
	return sl_in_service_ind(up, q, mp);
      outstate:
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT))) {
		err = 0;
		goto error;
	}
	strlog(up->mid, up->sid, 0, SL_TRACE, "ESTAB Con in unexpected state %u:%u:%u",
	       up_get_state(up), as_get_state(as), dl_get_state(up));
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

	if (dl_get_state(up) != DL_IDLE)
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
	       up_get_state(up), as_get_state(as), dl_get_state(up));
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

	switch (dl_get_state(up)) {
	case DL_OUTCON_PENDING:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	dl_set_state(up, DL_IDLE);
	return sl_out_of_service_ind(up, q, mp, SL_FAIL_UNSPECIFIED);
      outstate:
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT))) {
		err = 0;
		goto error;
	}
	strlog(up->mid, up->sid, 0, SL_TRACE, "REL Ind in unexpecteds state %u:%u:%u",
	       up_get_state(up), as_get_state(as), dl_get_state(up));
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
as_recv_status_lpo_set(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
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
		return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_LPO_CLEAR);
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
as_recv_status_lpo_clear(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
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
		return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_LPO_SET);
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
as_recv_status_emer_set(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
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
		return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_EMER_CLEAR);
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
as_recv_status_emer_clear(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	if (up->status.pending & (1 << M2UA_STATUS_EMER_CLEAR)) {
		up->status.pending &= ~(1 << M2UA_STATUS_EMER_CLEAR);
		goto discard;
	}
	if (!(up->status.pending & (1 << M2UA_STATUS_AUDIT)))
		strlog(up->mid, up->sid, 0, SL_TRACE, "Status EMER Clear confirmation unexpected");
	if (!(up->status.pending & (1 << M2UA_STATUS_EMER_SET)))
		return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_EMER_SET);
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
as_recv_maup_state_con(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct ua_parm status = { {NULL,}, };

	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &status, M2UA_PARM_STATE_REQUEST)) {
		switch (status.val) {
		case M2UA_STATUS_LPO_SET:
			return as_recv_status_lpo_set(up, tp, q, mp);
		case M2UA_STATUS_LPO_CLEAR:
			return as_recv_status_lpo_clear(up, tp, q, mp);
		case M2UA_STATUS_EMER_SET:
			return as_recv_status_emer_set(up, tp, q, mp);
		case M2UA_STATUS_EMER_CLEAR:
			return as_recv_status_emer_clear(up, tp, q, mp);
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
as_recv_maup_cong_ind(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
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
	return (ecode);
}

/**
 * as_recv_maup_data: - receive DATA message
 * @up: UP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static fastcall __hot_in int
as_recv_maup_data(struct up *up, struct tp *tp, queue_t *q, mblk_t *mp)
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
	return (ecode);
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

	if (!(rp = rp_lookup(gp, q, mp, &err)) && err)
		return (err);
	return as_recv_maup_data(up, tp, q, mp);
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
#endif

/**
 * rp_recv_rkmm_reg_rsp: - receive REG Rsp message
 * @tp: TP private structure
 * @rp: AS-SGP relation pointer
 * @q: active queue (lower read queue)
 * @mp: the message
 * @status: registration status
 * @asid: IID or RC value
 */
static int
rp_recv_rkmm_reg_rsp(struct tp *tp, struct rp *rp, queue_t *q, mblk_t *mp, uint32_t status, uint32_t asid)
{
	int err, error = 0;
	struct as *as = rp->as.as;

	if (rp_get_state(rp) != ASP_WACK_ASPUP)
		goto outstate;
	as->as.asid = asid;
	switch (status) {
	case 0:		/* Successfully Registered */
		break;
	default:
	case 1:		/* Error - Unknown */
//	case 2:		/* Error - Invalid SDLI */
	case 2:		/* Error - Invalid Destination Address */
//	case 3:		/* Error - Invalid SDTI */
	case 3:		/* Error - Invalid Network Appearance */
//	case 4:		/* Error - Invalid Link Key */
	case 4:		/* Error - Invalid Routing Key */
	case 5:		/* Error - Permission Denied */
//	case 6:		/* Error - Overlapping (Non-unique) Link Key */
	case 6:		/* Error - Cannot Support Unique Routing */
//	case 7:		/* Error - Link Key not Provisioned */
	case 7:		/* Error - Routing Key not Currently Provisioned */
	case 8:		/* Error - Insufficient Resources */
	case 9:		/* Error - Unsupported RK parameter field */
	case 10:		/* Error - Routing Key Change Refused */
		goto discard;	/* fall back on timer */
	}
	if ((err = rp_set_state(rp, q, AS_INACTIVE)))
		return (err);
	rp->rp.request_id = 0;
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_TRACE, "REG RSP in unexpected state %u", rp_get_state(rp));
	err = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	return (err);
}

/**
 * rp_recv_rkmm_dereg_rsp: - receive DEREG Rsp message
 * @tp: TP private structure
 * @rp: AS-SGP relation pointer
 * @q: active queue (lower read queue)
 * @mp: the message
 * @status: deregistration status
 * @asid: IID or RC value
 */
static int
rp_recv_rkmm_dereg_rsp(struct tp *tp, struct rp *rp, queue_t *q, mblk_t *mp, uint32_t status, uint32_t asid)
{
	int err;

	if (rp_get_state(rp) != ASP_WACK_ASPDN)
		goto outstate;
	switch (status) {
	case 0:		/* Successfully De-registered */
		break;
	default:		/* protocol error */
	case 1:		/* Error - Unknown */
	case 2:		/* Error - Invalid Interface Identifier or Routing Context */
	case 3:		/* Error - Permission Denied */
	case 4:		/* Error - Not Registered */
	case 5:		/* Error - Registration Change Refused */
		goto discard;
	}
	if ((err = rp_set_state(rp, q, AS_DOWN)))
		return (err);
      discard:
	/* FIXME: generate message to layer manager */
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_TRACE, "DEREG RSP in unexpected state %u", rp_get_state(rp));
	err = UA_ECODE_UNEXPECTED_MESSAGE;
	goto error;
      error:
	return (err);
}

/**
 * tp_recv_err: - process receive error by code
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 * @err: error value
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
	case -ENOENT:
	{
		struct ua_parm asid = { {NULL,}, };

		/* Return codes that result from the failure to locate an AS for a given UA message
		   (ENOENT) should always respond by attempting to disable the corresponding
		   application server.  In this way, we can simply close upper User Streams in
		   up_qclose() and the arrival of any message for the AS will result in the AS being
		   deactivated if the AS could not be deactivated during close (perhaps from failure
		   to allocate a buffer). */
#if M2UA
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &asid, UA_PARM_IID))
			return tp_send_aspt_aspia_req(tp, q, mp, &asid.val, NULL, 0, NULL, 0);
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &asid, UA_PARM_IID_TEXT))
			return tp_send_aspt_aspia_req(tp, q, mp, NULL, asid.cp, asid.len, NULL, 0);
#endif
#if M3UA
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &asid, UA_PARM_RC))
			return tp_send_aspt_aspia_req(tp, q, mp, &asid.val, NULL, 0);
#endif
#if SUA
		if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &asid, UA_PARM_RC))
			return tp_send_aspt_aspia_req(tp, q, mp, &asid.val, NULL, 0);
#endif
		if (tp->gp.gp->sg.sg->sp.sp->up.list == NULL)
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
		return gp_send_mgmt_err(gp, q, mp, err, mp->b_rptr, mp->b_wptr - mp->b_rptr);
	}
	return (err);
}

/**
 * tp_recv_mgmt: - receive MGMT message from SG
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: MGMT message
 */
static int
tp_recv_mgmt(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (UA_MSG_TYPE(p[0])) {
	case UA_MGMT_ERR:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ERR <-");
		err = gp_recv_mgmt_err(tp->gp.gp, q, mp);
		break;
	case UA_MGMT_NTFY:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "NTFY <-");
		err = gp_recv_mgmt_ntfy(tp->gp.gp, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
}

/**
 * tp_recv_asps: - receive ASPS message from SG
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: ASPS message
 */
static int
tp_recv_asps(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (UA_MSG_TYPE(p[0])) {
	case UA_ASPS_HBEAT_REQ:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "BEAT <-");
		err = gp_recv_asps_hbeat_req(tp->gp.gp, tp, q, mp);
		break;
	case UA_ASPS_ASPUP_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ASPUP Ack <-");
		err = gp_recv_asps_aspup_ack(tp->gp.gp, tp, q, mp);
		break;
	case UA_ASPS_ASPDN_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ASPDN Ack <-");
		err = gp_recv_asps_aspdn_ack(tp->gp.gp, tp, q, mp);
		break;
	case UA_ASPS_HBEAT_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "BEAT Ack <-");
		err = gp_recv_asps_hbeat_ack(tp->gp.gp, tp, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
}

/**
 * tp_recv_aspt: - receive ASPT message from SG
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: ASPT message
 *
 * A note about ASPAC Ack and ASPIA Ack: the RFC says that if there is an RC/IID in the request that
 * there MUST be an RC/IID in the Ack.  Because we always send an RC/IID in the request, we require
 * that there always be an RC/IID in the Ack.  We use the RC/IID to determine for which User and AS
 * the acknowledgement applies.
 */
static int
tp_recv_aspt(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (UA_MSG_TYPE(p[0])) {
	case UA_ASPT_ASPAC_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ASPAC Ack <-");
		err = gp_recv_aspt_aspac_ack(tp->gp.gp, tp, q, mp);
		break;
	case UA_ASPT_ASPIA_ACK:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "ASPIA Ack <-");
		err = gp_recv_aspt_aspia_ack(tp->gp.gp, tp, q, mp);
		break;
	default:
		err = (-EOPNOTSUPP);
		break;
	}
	return (err);
}

#if M2UA
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

	if (!(rp = rp_lookup(gp, q, mp, &err)) && err)
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
		err = as_recv_maup_state_con(up, tp, q, mp);
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
		err = as_recv_maup_cong_ind(up, tp, q, mp);
		break;
	case M2UA_MAUP_DATA:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "DATA <-");
		err = as_recv_maup_data(up, tp, q, mp);
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
#endif

#if M3UA
/**
 * tp_recv_xfer: - receive XFER message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_xfer(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	int err;

	if (!(as = rp_lookup(tp, q, mp, &err)) && err)
		return (err);

	switch (UA_MSG_TYPE(p[0])) {
	case UA_XFER_DATA:
		strlog(tp->mid, tp->sid, UALOGDA, SL_TRACE, "XFER DATA <-");
		err = as_recv_xfer_data(as, tp, q, mp);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return (err);
}

/**
 * tp_recv_snmm: - receive SNMM message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_snmm(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	int err;

	if (!(as = rp_lookup(tp, q, mp, &err)) && err)
		return (err);

	switch (UA_MSG_TYPE(p[0])) {
	case UA_SNMM_DUNA:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DUNA <-");
		err = as_recv_snmm_duna(as, q, mp);
		break;
	case UA_SNMM_DAVA:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DAVA <-");
		err = as_recv_snmm_dava(as, q, mp);
		break;
	case UA_SNMM_DAUD:
		err = -EINVAL;	/* unexpected in this direction */
		break;
	case UA_SNMM_SCON:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM SCON <-");
		err = as_recv_snmm_scon(as, q, mp);
		break;
	case UA_SNMM_DUPU:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DUPU <-");
		err = as_recv_snmm_dupu(as, q, mp);
		break;
	case UA_SNMM_DRST:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DRST <-");
		err = as_recv_snmm_drst(as, q, mp);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return (err);
}
#endif

#if SUA
/**
 * tp_recv_cnls: - receive CNLS message from SG
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: CNLS message
 */
static int
tp_recv_cnls(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
}

/**
 * tp_recv_cons: - receive CONS message from SG
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: CONS message
 */
static int
tp_recv_cons(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
}

/**
 * tp_recv_snmm: - receive SNMM message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_snmm(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	int err;

	if (!(as = as_lookup(tp, q, mp, &err)) && err)
		return (err);

	switch (UA_MSG_TYPE(p[0])) {
	case UA_SNMM_DUNA:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DUNA <-");
		err = as_recv_snmm_duna(as, q, mp);
		break;
	case UA_SNMM_DAVA:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DAVA <-");
		err = as_recv_snmm_dava(as, q, mp);
		break;
	case UA_SNMM_DAUD:
		err = -EINVAL;	/* unexpected in this direction */
		break;
	case UA_SNMM_SCON:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM SCON <-");
		err = as_recv_snmm_scon(as, q, mp);
		break;
	case UA_SNMM_DUPU:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DUPU <-");
		err = as_recv_snmm_dupu(as, q, mp);
		break;
	case UA_SNMM_DRST:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DRST <-");
		err = as_recv_snmm_drst(as, q, mp);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return (err);
}
#endif

#if TUA
/**
 * tp_recv_snmm: - receive SNMM message
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
tp_recv_snmm(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	int err;

	if (!(as = as_lookup(tp, q, mp, &err)) && err)
		return (err);

	switch (UA_MSG_TYPE(p[0])) {
	case UA_SNMM_DUNA:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DUNA <-");
		err = as_recv_snmm_duna(as, q, mp);
		break;
	case UA_SNMM_DAVA:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DAVA <-");
		err = as_recv_snmm_dava(as, q, mp);
		break;
	case UA_SNMM_DAUD:
		err = -EINVAL;	/* unexpected in this direction */
		break;
	case UA_SNMM_SCON:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM SCON <-");
		err = as_recv_snmm_scon(as, q, mp);
		break;
	case UA_SNMM_DUPU:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DUPU <-");
		err = as_recv_snmm_dupu(as, q, mp);
		break;
	case UA_SNMM_DRST:
		strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "SNMM DRST <-");
		err = as_recv_snmm_drst(as, q, mp);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return (err);
}
#endif

/**
 * tp_recv_rkmm: - receive RKMM message from SG
 * @tp: TP private structure
 * @q: active queue (lower read queue)
 * @mp: RKMM message
 *
 * RKMM messages are only sent for an AS-SGP relation.  They are sent as part of the bind/attach
 * process and used to translate an address to an AS id (if required).  When an AS-SGP relation
 * sends a registration or deregistration request, it saves the id for the request in the RP
 * structure.  We can find the RP structure that corresponds to a response by finding the structure
 * with the same request identifier.
 */
static int
tp_recv_rkmm(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	struct ua_parm request_id;
	struct rp *rp;
	int err;

	/* FIXME: message can contain multiple registration results (but probably won't) */
	if (ua_dec_parm(mp->b_rptr, mp->b_wptr, &request_id, M3UA_PARM_REG_RESULT)) {
		uint32_t status = 0;
		uint32_t asid = 0;

		/* FIXME: also need M3UA_PARM_DEREG_RESULT, which does not have routing key id */
		for (rp = tp->gp.gp->rp.list; rp; rp = rp->gp.next)
			if (rp->rp.request_id == request_id.val)
				break;
		if (rp == NULL)
			/* FIXME: process error for each registration result instead of returning
			   it */
			return (-EPROTO);

		switch (UA_MSG_TYPE(p[0])) {
		case UA_RKMM_REG_RSP:
			/* FIXME: dig registration status and AS identifier out of the
			   registration result. */
			strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "REG Rsp <-");
			err = rp_recv_rkmm_reg_rsp(tp, rp, q, mp, status, asid);
			break;
		case UA_RKMM_DEREG_RSP:
			/* FIXME: dig registration status out of the deregistration result */
			strlog(tp->mid, tp->sid, UALOGRX, SL_TRACE, "DEREG Rsp <-");
			err = rp_recv_rkmm_dereg_rsp(tp, rp, q, mp, status, asid);
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
	struct gp *gp;
	int err;

	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + 2 * sizeof(*p))
		goto error;
	if (mp->b_wptr < mp->b_rptr + ntohl(p[1]))
		goto error;
	if (!(gp = tp->gp.gp))
		return (-EAGAIN);
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
#if M2UA
	case UA_CLASS_MAUP:
		err = tp_recv_maup(tp, q, mp);
		break;
#endif
#if M3UA
	case UA_CLASS_XFER:
		err = tp_recv_xfer(tp, q, mp);
		break;
	case UA_CLASS_SNMM:
		err = tp_recv_snmm(tp, q, mp);
		break;
#endif
#if SUA
	case UA_CLASS_CNLS:
		err = tp_recv_cnls(tp, q, mp);
		break;
	case UA_CLASS_CONS:
		err = tp_recv_cons(tp, q, mp);
		break;
#endif
#if TUA
		/* FIXME */
#endif
	case UA_CLASS_RKMM:
#if M2UA
		/* TS 102 141 says that if a link key management message is received to return an
		   unrecognized message class error. */
		if (gp->sg.sg->sp.sp->sp.options.options.pvar == UA_VERSION_TS102141)
			goto enoprotoopt;
#endif
#if M3UA
		/* TS 102 142 says that if a routing key management message is received to return an
		   unrecognized message class error. */
		if (gp->sg.sg->sp.sp->sp.options.options.pvar == UA_VERSION_TS102142)
			goto enoprotoopt;
#endif
#if SUA
		/* TS 102 143 says that if a routing key management message is received to return
		   an unrecognized message class error. */
		if (sg->sp.options.options.pvar == UA_VERSION_TS102143)
			goto enoprotoopt;
#endif
#if TUA
		/* FIXME */
#endif
#if ISUA
		/* FIXME */
#endif
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
 * @mp: the message (just the UA part)
 */
static inline fastcall int
tp_recv_msg(struct tp *tp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	/* fast path for data */
	if (mp->b_wptr >= mp->b_rptr + 2 * sizeof(*p))
		if (mp->b_wptr >= mp->b_rptr + ntohl(p[1])) {
#if M2UA
			if (UA_MSG_CLAS(p[0]) == UA_CLASS_MAUP) {
				if (UA_MSG_TYPE(p[0]) == M2UA_MAUP_DATA) {
					if ((err = tp_recv_maup_data(tp, q, mp)) == 0)
						return (0);
					return tp_recv_err(tp, q, mp, err);
				}
			}
#endif
#if M3UA
			if (UA_MSG_CLAS(p[0]) == UA_CLASS_XFER) {
				if (UA_MSG_TYPE(p[0]) == UA_XFER_DATA) {
					if ((err = tp_recv_xfer_data(tp, q, mp)) == 0)
						return (0);
					return tp_recv_err(tp, q, mp, err);
				}
			}
#endif
#if SUA
			if (UA_MSG_CLAS(p[0]) == UA_CLASS_CNLS) {
				if (UA_MSG_TYPE(p[0]) == SUA_CNLS_CLDT) {
					if ((err = tp_recv_clns_cldt(tp, q, mp)) == 0)
						return (0);
					return tp_recv_err(tp, q, mp, err);
				}
			} else if (UA_MSG_CLAS(p[0]) == UA_CLASS_CONS) {
				if (UA_MSG_TYPE(p[0]) == SUA_CONS_CODT) {
					if ((err = tp_recv_cons_codt(tp, q, mp)) == 0)
						return (0);
					return tp_recv_err(tp, q, mp, err);
				}
			}
#endif
		}
	return tp_recv_msg_slow(tp, q, mp);
}

#if M2UA
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
lmi_info_req(struct up *up, queue_t *q, mblk_t *mp)
{
	return lmi_info_ack(up, q, mp);
}

/**
 * lmi_attach_req - process LMI_ATTACH_REQ primtive
 * @up: UP private structure
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
lmi_attach_req(struct up *up, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct up *sl2;
	struct sp *sp;
	uint32_t spid, asid;
	caddr_t ppa_ptr, iid_ptr;
	size_t ppa_len, iid_len;
	unsigned long flags;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_UNATTACHED)
		goto outstate;
	ppa_ptr = (caddr_t) p->lmi_ppa;
	ppa_len = mp->b_wptr - mp->b_rptr - sizeof(*p);
	if (ppa_len == 0) {
		/* the user can specific a zero-length PPA when reattaching */
		if (up->as.as->as.addr.sdti == 0 && up->as.as->as.addr.sdli == 0
		    && up->as.as->as.asid == 0 && up->as.as->as.addr.iid_text[0] == '\0')
			goto badppa;
		if ((spid = up->as.as->as.addr.spid) == 0)
			goto badppa;
		asid =
		    ((up->as.as->as.addr.sdti << 16) | up->as.as->as.addr.sdli) ? : up->as.as->as.addr.
		    asid;
		iid_ptr =
		    (up->as.as->as.addr.iid_text[0] == '\0') ? NULL : up->as.as->as.addr.iid_text;
		iid_len = iid_ptr ? strnlen(iid_ptr, 32) : 0;
	} else {
		size_t spid_len = ((spid = up->unit) == 0) ? sizeof(uint32_t) : 0;

		if (ppa_len < spid_len + 1)
			goto badppa;
		spid = spid ? : *(uint32_t *) p->lmi_ppa;
		if (ppa_len == spid_len + sizeof(uint32_t)) {
			asid = *(uint32_t *) ((caddr_t) p->lmi_ppa + spid_len);
			iid_ptr = NULL;
			iid_len = 0;
		} else {
			asid = 0;
			iid_ptr = (caddr_t) p->lmi_ppa + spid_len;
			iid_len = ppa_len - spid_len;
			iid_len = strnlen(iid_ptr, iid_len);
			if (iid_len == 0)
				goto badppa;
			if (iid_len > 32)
				iid_len = 32;
		}
	}
	write_lock_irqsave(&ua_mux_lock, flags);
	if (!(sp = find_object_sp(&err, up, spid, NULL))) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		goto badppa;
	}
	if (!sp_trylock(sp, q)) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		return (-EDEADLK);
	}
	if (sp->sp.options.options.pvar != UA_VERSION_TS102141
	    && (sp->sg.list->sg.options.options.popt & UA_DYNAMIC)) {
		/* ETSI TS 102 141 does not permit dynamic configuration. */
		if (iid_ptr != NULL) {
			sp_unlock(sp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			goto badppa;
		}
		/* interpret asid as link key */
		for (sl2 = sp->up.list; sl2; sl2 = sl2->sp.next)
			if (((up->as.as->as.addr.sdti << 16) | up->as.as->as.addr.sdli) == asid)
				break;
	} else if (iid_ptr) {
		/* asid is text */
		if (sp->sp.options.options.pvar == UA_VERSION_TS102141
		    || !(sp->sg.list->sg.options.options.popt & UA_TEXTIID)) {
			/* ETSI TS 102 141 does not permit the use of text interface identifiers. */
			sp_unlock(sp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			goto badppa;
		}
		for (sl2 = sp->up.list; sl2; sl2 = sl2->sp.next)
			if (strncmp(sl2->as.as->as.addr.iid_text, iid_ptr, iid_len) == 0)
				break;
	} else {
		/* asid is integer */
		for (sl2 = sp->up.list; sl2; sl2 = sl2->sp.next)
			if (sl2->as.as->as.asid == asid)
				break;
	}
	if (sl2) {
		if (sl2 != up) {
			sp_unlock(sp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			goto ebusy;
		}
		/* already attached, probably from previous run that required buffers */
	} else {
		struct tp *tp = up->as.as->sp.sp->sg.list->gp.list->xp.xp;

		up_sp_link(up, sp);
		up->as.as->as.addr.sdti = (asid >> 16) & 0xffff;
		up->as.as->as.addr.sdli = (asid >> 0) & 0xffff;
		up->as.as->as.asid = 0;
		up->as.as->as.addr.iid_text[0] = '\0';
		if (iid_len)
			strncpy(iid_ptr, up->as.as->as.addr.iid_text, iid_len);
		/* When a signalling link interface is attached to an SCTP transport, it is
		   assigned one of the available DATA (non-zero) stream ids for use for MAUP
		   messages.  The precise stream id number is unimportant: we just want to spread
		   the interfaces over as many streams as possible. Therefore, we assign streams in 
		   a round-robin fashion over the available streams. */
		up->as.as->as.streamid = tp->gp.gp->gp.nextstream;
		if (++tp->gp.gp->gp.nextstream >= tp->xp.options.ostreams)
			tp->gp.gp->gp.nextstream = 1;
	}
	if (sp->sp.options.options.pvar != UA_VERSION_TS102141
	    && (sp->sg.list->sg.options.options.popt & UA_DYNAMIC)) {
		int rtn;
		struct tp *tp = up->as.as->sp.sp->sg.list->gp.list->xp.xp;

		/* ETSI TS 102 141 does not permit dynamic configuration. */
		write_unlock_irqrestore(&ua_mux_lock, flags);
		up_set_state(up, LMI_ATTACH_PENDING);
		as_set_state(as, AS_WRSP_RREQ);
		/* issue registation request */
		up->as.as->as.request_id = atomic_inc_return(&ua_request_id);
		rtn = rp_send_rkmm_reg_req(rp, q, mp);
		sp_unlock(sp);
		return (rtn);
	} else {
		sp_unlock(sp);
		write_unlock_irqrestore(&ua_mux_lock, flags);
		up_set_state(up, LMI_ATTACH_PENDING);
		as_set_state(as, AS_INACTIVE);
		return lmi_ok_ack(up, q, mp, LMI_ATTACH_REQ);
	}
      ebusy:
	return lmi_error_ack(up, q, mp, LMI_ATTACH_REQ, -EBUSY);
#if 0
      enxio:
	return lmi_error_ack(up, q, mp, LMI_ATTACH_REQ, -ENXIO);
#endif
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
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_detach_req(struct up *up, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	struct sp *sp = up->sp.sp;
	struct tp *tp;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_DISABLED)
		goto outstate;
	up_set_state(up, LMI_DETACH_PENDING);
	switch (sp_get_state(sp) != ASP_UP)
		return lmi_ok_ack(up, q, mp, LMI_DETACH_REQ);
	switch (as_get_state(as)) {
	case AS_INACTIVE:
		if (sp->sp.options.options.pvar != UA_VERSION_TS102141
		    && (sp->sg.list->sg.options.options.popt & UA_DYNAMIC)) {
			/* ETSI TS 102 141 does not permit dynamic configuration. */
			/* need to deregister */
			as_set_state(as, AS_WRSP_DREQ);
			tp = sp->sg.list->gp.list->xp.xp;
			return rp_send_rkmm_dereg_req(rp, q, mp);
		}
		break;
	case ASP_WACK_ASPDN:
		/* wait for deregistration to complete */
		freemsg(mp);
		return (0);
	case ASP_DOWN:
		break;
	case ASP_WACK_ASPUP:
	case AS_WACK_ASPIA:
	case AS_WACK_ASPAC:
	case AS_ACTIVE:
	default:
		strlog(up->mid, up->sid, 0, SL_TRACE, "%s: unexpected up u state %d",
		       __FUNCTION__, as_get_state(as));
		goto unspec;
	}
	if ((err = lmi_ok_ack(up, q, mp, LMI_DETACH_REQ)) == 0) {
		unsigned long flags;

		/* Note that this sequence means that we alway have to take the sp lock before
		   taking the up mux lock. */
		write_lock_irqsave(&ua_mux_lock, flags);
		/* remove from sp list */
		up_sp_unlink(up);
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
lmi_enable_req(struct up *up, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;
	struct sp *sp = up->sp.sp;
	struct rp *rp;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_DISABLED)
		goto outstate;
	up_set_state(up, LMI_ENABLE_PENDING);
	if (sp_get_state(sp) != ASP_UP)
		goto initfailed;
	if (as_get_state(as) != AS_INACTIVE)
		goto unspec;
	for (rp = up->as.as->rp.list; rp; rp = rp->as.next)
		if (rp_get_state(rp) == AS_INACTIVE)
			if ((err = rp_send_aspt_aspac_req(rp, q, NULL, NULL, 0)))
				return (err);
	freemsg(mp);
	return (0);
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
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_disable_req(struct up *up, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;
	struct sp *sp = up->sp.sp;
	struct rp *rp;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_ENABLED)
		goto outstate;
	up_set_state(up, LMI_DISABLE_PENDING);
	if (sp_get_state(sp) != ASP_UP)
		return lmi_disable_con(up, q, mp);
	switch (as_get_state(as)) {
	case AS_ACTIVE:
	case AS_WACK_ASPAC:
		for (rp = up->as.as->rp.list; rp; rp = rp->as.next)
			if (rp_get_state(rp) == AS_ACTIVE || rp_get_state(rp) == AS_WACK_ASPAC)
				if ((err = rp_send_aspt_aspia_req(rp, q, NULL, NULL, 0)))
					return (err);
		break;
	case AS_WACK_ASPIA:
		break;
	case AS_INACTIVE:
	case ASP_WACK_ASPUP:
	case ASP_WACK_ASPDN:
	case ASP_DOWN:
		return lmi_disable_con(up, q, mp);
	default:
		goto unspec;
	}
	freemsg(mp);
	return (0);
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
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
lmi_optmgmt_req(struct up *up, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * sl_pdu_req - process SL_PDU_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static inline fastcall __hot_write int
sl_pdu_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	struct tp *tp;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_ENABLED)
		goto outstate;
	if (as_get_state(as) != AS_ACTIVE)
		goto discard;
	mp->b_cont = NULL;
	tp = up->as.as->sp.sp->sg.list->gp.list->xp.xp;
	if ((up->up.option.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
		if ((err = as_send_maup_data1(up, tp, q, mp, dp)) != 0)
			mp->b_cont = dp;
	} else {
		if ((err = as_send_maup_data2(up, tp, q, mp, dp, p->sl_mp)) != 0)
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
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_emergency_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_ENABLED)
		goto outstate;
	if (as_get_state(as) != AS_ACTIVE)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_EMER_SET);
      outstate:
	return lmi_error_ack(up, q, mp, SL_EMERGENCY_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_EMERGENCY_REQ, LMI_TOOSHORT);
}

/**
 * sl_emergency_ceases_req - process SL_EMERGENCY_CEASES_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_emergency_ceases_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_ENABLED)
		goto outstate;
	if (as_get_state(as) != AS_ACTIVE)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_EMER_CLEAR);
      outstate:
	return lmi_error_ack(up, q, mp, SL_EMERGENCY_CEASES_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_EMERGENCY_CEASES_REQ, LMI_TOOSHORT);
}

/**
 * sl_start_req - process SL_START_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * This primitive is only valid in state LS_INIT or LS_FAILED.  It is ignored in any other state.
 */
static noinline fastcall int
sl_start_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_ENABLED)
		goto outstate;
	if (as_get_state(as) != AS_ACTIVE)
		return sl_local_processor_outage_ind(up, q, mp);
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_estab_req(up, tp, q, mp);
      outstate:
	return lmi_error_ack(up, q, mp, SL_START_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_START_REQ, LMI_TOOSHORT);
}

/**
 * sl_stop_req - process SL_STOP_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * This primitive is only valid in state LS_STARTING, LS_IN_SERVICE, LS_REM_PO, and is ignored in
 * any other state.
 */
static noinline fastcall int
sl_stop_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_ENABLED)
		goto outstate;
	switch (sl_get_state(up)) {
	case LS_STARTING:
	case LS_IN_SERVICE:
	case LS_REM_PO:
		break;
	default:
		goto ignore;
	}
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_rel_req(up, tp, q, mp);
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
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * This primitive is only really valid in state LS_FAILED, but is responded to in any state.
 */
static noinline fastcall int
sl_retrieve_bsnt_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != LMI_ENABLED)
		goto outstate;
	if (as_get_state(as) != AS_ACTIVE)
		return sl_bsnt_not_retrievable_ind(up, q, mp);
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_retr_req(up, tp, q, mp, NULL);
      outstate:
	return lmi_error_ack(up, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_TOOSHORT);
}

/**
 * sl_retrieval_request_and_fsnc_req - process SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_retreival_request_and_fsnc_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;
	uint32_t fsnc;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_IDLE)
		goto outstate;
	fsnc = p->sl_fsnc;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_retr_req(up, tp, q, mp, &fsnc);
      outstate:
	return lmi_error_ack(up, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_TOOSHORT);
}

/**
 * sl_clear_buffers_req - process SL_CLEAR_BUFFERS_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_clear_buffers_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_DATAXFER)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_FLUSH_BUFFERS);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_TOOSHORT);
}

/**
 * sl_clear_rtb_req - process SL_CLEAR_RTB_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_clear_rtb_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_DATAXFER)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CLEAR_RTB);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CLEAR_RTB_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CLEAR_RTB_REQ, LMI_TOOSHORT);
}

/**
 * sl_continue_req - process SL_CONTINUE_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_continue_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_continue_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_DATAXFER)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CONTINUE);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CONTINUE_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CONTINUE_REQ, LMI_TOOSHORT);
}

/**
 * sl_local_processor_outage_req - process SL_LOCAL_PROCESSOR_OUTAGE_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_local_processor_outage_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_DATAXFER)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	/* ETSI TS 102 141 does not permit use of this primitive. */
	if (tp->sp.sp->sp.options.options.pvar == UA_VERSION_TS102141)
		goto notsupp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_LPO_SET);
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
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_resume_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_DATAXFER)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	/* ETSI TS 102 141 does not permit use of this primitive. */
	if (tp->sp.sp->sp.options.options.pvar == UA_VERSION_TS102141)
		goto notsupp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_LPO_CLEAR);
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
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_congestion_discard_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_cong_discard_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_DATAXFER)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CONG_DISCARD);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_TOOSHORT);
}

/**
 * sl_congestion_accept_req - process SL_CONGESTION_ACCEPT_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_congestion_accept_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_cong_accept_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_DATAXFER)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CONG_ACCEPT);
      outstate:
	return lmi_error_ack(up, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_TOOSHORT);
}

/**
 * sl_no_congestion_req - process SL_NO_CONGESTION_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_no_congestion_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_no_cong_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_DATAXFER)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_CONG_CLEAR);
      outstate:
	return lmi_error_ack(up, q, mp, SL_NO_CONGESTION_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_NO_CONGESTION_REQ, LMI_TOOSHORT);
}

/**
 * sl_power_on_req - process SL_POWER_ON_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_power_on_req(struct up *up, queue_t *q, mblk_t *mp)
{
	sl_power_on_req_t *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (dl_get_state(up) != DL_UNBOUND)
		goto outstate;
	tp = up->sp.sp->sg.list->gp.list->xp.xp;
	return as_send_maup_state_req(up, tp, q, mp, M2UA_STATUS_AUDIT);
      outstate:
	return lmi_error_ack(up, q, mp, SL_POWER_ON_REQ, LMI_OUTSTATE);
      tooshort:
	return lmi_error_ack(up, q, mp, SL_POWER_ON_REQ, LMI_TOOSHORT);
}

#if 0
/**
 * sl_optmgmt_req - process SL_OPTMGMT_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_optmgmt_req(struct up *up, queue_t *q, mblk_t *mp)
{
}
#endif

#if 0
/**
 * sl_notify_req - process SL_NOTIFY_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_notify_req(struct up *up, queue_t *q, mblk_t *mp)
{
}
#endif

/**
 * sl_other_req - process SL_OTHER_REQ primtive
 * @up: UP private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
sl_other_req(struct up *up, queue_t *q, mblk_t *mp)
{
	uint32_t *p = (typeof(p)) mp->b_rptr;

	return lmi_error_ack(up, q, mp, *p, LMI_NOTSUPPORT);
}
#endif

#if M3UA
/*
 *  MTP-User to MTP-Provider Primitives
 *  -------------------------------------------------------------------------
 */

/**
 * mtp_bind_req: - process MTP_BIND_REQ primitive
 * @up: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_bind_req(struct up *up, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_unbind_req: - process MTP_UNBIND_REQ primitive
 * @up: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_unbind_req(struct up *up, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_conn_req: - process MTP_CONN_REQ primitive
 * @up: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_conn_req(struct up *up, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_discon_req: - process MTP_DISCON_REQ primitive
 * @up: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_discon_req(struct up *up, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_addr_req: - process MTP_ADDR_REQ primitive
 * @up: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_addr_req(struct up *up, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_info_req: - process MTP_INFO_REQ primitive
 * @up: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_info_req(struct up *up, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_optmgmt_req: - process MTP_OPTMGMT_REQ primitive
 * @up: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_optmgmt_req(struct up *up, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_transfer_req: - process MTP_TRANSFER_REQ primitive
 * @up: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_transfer_req(struct up *up, queue_t *q, mblk_t *mp)
{
}

/**
 * mtp_other_req: - process other MTP primitive
 * @up: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the primitive
 */
static noinline fastcall int
mtp_other_req(struct up *up, queue_t *q, mblk_t *mp)
{
}
#endif

#if SUA
/*
 *  SCCP-User to SCCP-Provider Primitives
 *  -------------------------------------------------------------------------
 */

/**
 * n_conn_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_conn_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_conn_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_conn_res: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_conn_res(struct up *up, queue_t *q, mblk_t *mp)
{
	N_conn_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_discon_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_discon_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_discon_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_data_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_data_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_data_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_exdata_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_exdata_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_exdata_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_info_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_info_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_info_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_bind_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_bind_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_unbind_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_unbind_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_unbind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_unitdata_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_unitdata_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_optmgmt_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_optmgmt_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_datack_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_datack_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_datack_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_reset_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_reset_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_reset_res: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_reset_res(struct up *up, queue_t *q, mblk_t *mp)
{
	N_reset_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_inform_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_inform_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_inform_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_coord_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_coord_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_coord_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_coord_res: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_coord_res(struct up *up, queue_t *q, mblk_t *mp)
{
	N_coord_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_state_req: - process N_CONN_REQ primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_state_req(struct up *up, queue_t *q, mblk_t *mp)
{
	N_state_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (up_get_state(up) != NS_IDLE)
		goto outstate;
	/* FIXME: implement this function */
	freemsg(mp);
	return (0);
      outstate:
	return n_error_ack(up, q, mp, N_CONN_REQ, NOUTSTATE);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}

/**
 * n_other_req: - process other primitive
 * @up: SC private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
n_other_req(struct up *up, queue_t *q, mblk_t *mp)
{
	np_ulong *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	return n_error_ack(up, q, mp, *p, NNOTSUPPORT);
      tooshort:
	return m_error(up, q, mp, EPROTO, EPROTO);
}
#endif

#if TUA
/*
 *  TCAP-User to TCAP-Provider Primitives
 *  -------------------------------------------------------------------------
 */
#endif

/*
 *  UA-User to UA-Management Primitives
 *  -------------------------------------------------------------------------
 */

/**
 * ua_info_req: - process UA_INFO_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_info_req(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct UA_info_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(lm->mid, lm->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(lm, q, mp, EPROTO, EPROTO);
}

/**
 * ua_asinfo_req: - process UA_ASINFO_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_asinfo_req(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct UA_asinfo_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(lm->mid, lm->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(lm, q, mp, EPROTO, EPROTO);
}

/**
 * ua_aspup_req: - process UA_ASPUP_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_aspup_req(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct UA_aspup_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(lm->mid, lm->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(lm, q, mp, EPROTO, EPROTO);
}

/**
 * ua_aspdn_req: - process UA_ASPDN_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_aspdn_req(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct UA_aspdn_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(lm->mid, lm->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(lm, q, mp, EPROTO, EPROTO);
}

/**
 * ua_aspac_req: - process UA_ASPAC_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_aspac_req(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct UA_aspac_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(lm->mid, lm->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(lm, q, mp, EPROTO, EPROTO);
}

/**
 * ua_aspia_req: - process UA_ASPIA_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_aspia_req(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct UA_aspia_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(lm->mid, lm->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(lm, q, mp, EPROTO, EPROTO);
}

/**
 * ua_reg_req: - process UA_REG_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_reg_req(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct UA_reg_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(lm->mid, lm->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(lm, q, mp, EPROTO, EPROTO);
}

/**
 * ua_dereg_req: - process UA_DEREG_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 */
static noinline fastcall int
ua_dereg_req(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct UA_dereg_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	strlog(lm->mid, lm->sid, 0, SL_ERROR, "FIXME: %s: implement this function", __FUNCTION__);
	freemsg(mp);
	return (0);
      tooshort:
	return m_error(lm, q, mp, EPROTO, EPROTO);
}

/**
 * ua_sctp_req: - process UA_SCTP_REQ primitive
 * @lm: LM private structure
 * @q: active queue (upper write queue)
 * @mp: primitive to process
 *
 * The UA_SCTP_REQ primitive provides a way for control Streams to send messages directly to
 * a lower controlled Stream and receive the responsees.  The messages are encapsulated in the
 * UA_SCTP_REQ primitive.  This simply finds the lower Stream from the encapulating SGP
 * identifier, and then passes the encapsulated message to that Stream.  Some state control is
 * performed.
 */
static noinline fastcall int
ua_sctp_req(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct UA_sctp_req *p = (typeof(p)) mp->b_rptr;
	struct tp *tp;
	struct sp *sp;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sizeof(t_scalar_t))
		return lmi_error_ack(lm, q, mp, 0, LMI_TOOSHORT);

	read_lock(&ua_mux_lock);
	if (!(tp = find_object_xp(&err, lm, p->ASSOC_id, NULL))) {
		read_unlock(&ua_mux_lock);
		return lmi_error_ack(lm, q, mp, UA_SCTP_REQ, err);
	}
	if (!(sp = tp->sp.sp)) {
		read_unlock(&ua_mux_lock);
		return lmi_error_ack(lm, q, mp, UA_SCTP_REQ, ENOENT);
	}
	if (!sp_trylock(sp, q)) {
		read_unlock(&ua_mux_lock);
		return (-EDEADLK);
	}
	if (tp->lm.lm == NULL) {
		if (lm != lm_ctrl)
			goto eperm;
	} else if (tp->lm.lm != lm) {
	      eperm:
		sp_unlock(sp);
		read_unlock(&ua_mux_lock);
		return lmi_error_ack(lm, q, mp, UA_SCTP_REQ, EPERM);
	}
	read_unlock(&ua_mux_lock);

	/* ok, yes, you can send to it */
	mp->b_rptr += sizeof(*p);
	switch (*(t_uscalar_t *) mp->b_rptr) {
	case T_CONN_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		if (tp_get_state(tp) == TS_IDLE)
			tp_set_state(tp, TS_WACK_CREQ);
		break;
	case T_CONN_RES:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		if (tp_get_state(tp) == TS_WRES_CIND)
			tp_set_state(tp, TS_WACK_CRES);
		break;
	case T_DISCON_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		switch (tp_get_state(tp)) {
		case TS_WCON_CREQ:
			tp_set_state(tp, TS_WACK_DREQ6);
			break;
		case TS_WRES_CIND:
			tp_set_state(tp, TS_WACK_DREQ7);
			break;
		case TS_DATA_XFER:
			tp_set_state(tp, TS_WACK_DREQ9);
			break;
		case TS_WIND_ORDREL:
			tp_set_state(tp, TS_WACK_DREQ10);
			break;
		case TS_WREQ_ORDREL:
			tp_set_state(tp, TS_WACK_DREQ11);
			break;
		}
		break;
	case T_DATA_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		break;
	case T_EXDATA_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		break;
	case T_INFO_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		break;
	case T_BIND_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		if (tp_get_state(tp) == TS_UNBND)
			tp_set_state(tp, TS_WACK_BREQ);
		break;
	case T_UNBIND_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		if (tp_get_state(tp) == TS_IDLE)
			tp_set_state(tp, TS_WACK_UREQ);
		break;
	case T_UNITDATA_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		break;
	case T_OPTMGMT_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		if (tp_get_state(tp) == TS_IDLE)
			tp_set_state(tp, TS_WACK_OPTREQ);
		break;
	case T_ORDREL_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		switch (tp_get_state(tp)) {
		case TS_DATA_XFER:
			tp_set_state(tp, TS_WIND_ORDREL);
			break;
		case TS_WIND_ORDREL:
			tp_set_state(tp, TS_IDLE);
			break;
		}
		break;
	case T_OPTDATA_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		break;
	case T_ADDR_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		break;
	case T_CAPABILITY_REQ:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_CONN_REQ", sp->sp.id);
		break;
	default:
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> %u T_????_???", sp->sp.id);
		break;
	}
	putnext(tp->wq, mp);
	sp_unlock(sp);
	return (0);
}

/*
 *  Application Server (AS) Timeouts
 *  -------------------------------------------------------------------------
 */

/**
 * rp_wack_aspac_timeout: - process timeout waiting for ASP Active Ack
 * @rp: AS-SGP relation pointer
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: timer message
 */
static noinline fastcall int
rp_wack_aspac_timeout(struct rp *rp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	uint tack = rp->as.as->as.options.tack;
	int err;

	if (rp_get_state(rp) != AS_WACK_ASPAC ||
	    (err = rp_send_aspt_aspac_req(rp, q, NULL, NULL, 0)) || !tack)
		return (err);
	strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE, "-> WACK ASPAC START <- (%u msec)", tack);
	mi_timer(q, mp, tack);
	return (0);
}

/**
 * rp_wack_aspia_timeout: - process timeout waiting for ASP Inactive Ack
 * @rp: AS-SGP relation pointer
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: timer message
 */
static noinline fastcall int
rp_wack_aspia_timeout(struct rp *rp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	uint tack = rp->as.as->as.options.tack;
	int err = 0;

	if (rp_get_state(rp) != AS_WACK_ASPIA ||
	    (err = rp_send_aspt_aspia_req(rp, q, NULL, NULL, 0)) || !tack)
		return (err);
	strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE, "-> WACK ASPIA START <- (%u msec)", tack);
	mi_timer(q, mp, tack);
	return (0);
}

/**
 * rp_wack_hbeat_timeout: - process timeout waiting for BEAT Ack
 * @rp: AS-SGP relation pointer
 * @tp: TP private structure
 * @q: active queue (upper write queue)
 * @mp: timer message
 */
static noinline fastcall int
rp_wack_hbeat_timeout(struct rp *rp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	uint tack = rp->as.as->as.options.tack;
	int err = 0;

	if ((err = rp_send_asps_hbeat_req(rp, q, NULL, NULL, 0)) || !tack)
		return (err);
	strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE, "-> WACK HBEAT START <- (%u msec)", tack);
	mi_timer(q, mp, tack);
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
lm_i_link(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct T_capability_req *p;
	unsigned long flags;
	struct sp *sp, *sg_new = NULL;
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
	/* might need an SG structure */
	if (!(sg_new = sg_alloc(0))) {
		err = ENOMEM;
		goto error;
	}

	write_lock_irqsave(&ua_mux_lock, flags);

	if (!(sp = lm->sp.sp)) {
		sp = XCHG(&sg_new, NULL);
		sp->users = 1;
		sp->lm.lm = lm;
	} else {
		if (!sp_trylock(sp, q)) {
			err = -EDEADLK;
			write_unlock_irqrestore(&ua_mux_lock, flags);
			goto error;
		}
		/* Note that there can only be one layer management Stream per SG.  For temporary
		   links, that must be the same layer manamgement Stream used to create the SG. */
		if (sp->lm.lm != lm) {
			err = EPERM;
			sp_release(sp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			goto error;
		}
	}

	tp_sp_link(tp, sp);

	mi_attach(l->l_qtop, (caddr_t) tp);
	sp_unlock(sp);
	write_unlock_irqrestore(&ua_mux_lock, flags);

	if (sg_new)
		sg_free(sg_new);

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
	if (sg_new)
		sg_free(sg_new);
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
lm_i_unlink(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	unsigned long flags;
	struct sp *sp;
	struct tp *tp;
	struct as *as, *as_free = NULL;

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

	sp = tp->sp.sp;
	if (!sp_trylock(sp, q)) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		return (-EDEADLK);
	}

	for (as = tp->as.list; as; as = as->tp.next) {
		/* TODO: issue upstream primitives as necessary.  We can release the ua_mux_lock
		   here and issue primitives, reacquiring it before unlinking the stream. */
		as_unlink(as);	/* this also sets AS state appropriately */
		as->tp.next = as_free;
		as_free = as;
	}

	tp_unlink(tp);		/* this also sets state appropriately */

	mi_detach(l->l_qtop, (caddr_t) tp);

	sp_unlock(sp);
	write_unlock_irqrestore(&ua_mux_lock, flags);

	/* free structures outside locks */
	for (as = as_free; as; as = as->tp.next)
		as_free(as);
	sp_check_free(sp);	/* free it if it is now orphaned */
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
lm_i_plink(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct T_capability_req *p;
	unsigned long flags;
	struct sp *sp = NULL;
	struct tp *tp = NULL;
	mblk_t *rp = NULL;
	int err;

	if (!(rp = ua_allocb(q, sizeof(*p), BPRI_MED))) {
		err = -ENOBUFS;
		goto error;
	}
	/* Sneaky trick.  If a non-zero minor device number was opened and on which the linking was 
	   performed, then the SGID is implied by the minor device number that was opened. */
	if (!(tp = tp_alloc_priv(l->l_qtop, l->l_index, ioc->ioc_cr, lm->as.addr.sgid))) {
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
	if ((sp = lm->sp.sp)) {
		if (!sp_trylock(sp, q)) {
			mi_close_unlink(&ua_links, (caddr_t) tp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			err = -EDEADLK;
			goto error;
		}
		if (sp->lm.lm != NULL) {
			/* Only one layer management Stream is permitted per SG.  If the SG has a
			   temporary layer manager, we cannot do permanent links against it. */
			sp_release(sp);
			mi_close_unlink(&ua_links, (caddr_t) tp);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			err = EPERM;
			goto error;
		}
		tp_sp_link(tp, sp);
	}

	mi_attach(l->l_qtop, (caddr_t) tp);
	if (sp)
		sp_unlock(sp);
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
lm_i_punlink(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	unsigned long flags;
	struct sp *sp;
	struct tp *tp;
	struct as *as, *as_free = NULL;

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

	sp = tp->sp.sp;
	if (!sp_trylock(sp, q)) {
		write_unlock_irqrestore(&ua_mux_lock, flags);
		return (-EDEADLK);
	}

	for (as = tp->as.list; as; as = as->tp.next) {
		/* TODO: issue upstream primitives as necessary.  We can release the ua_mux_lock
		   here and issue primitives, reacquiring it before unlinking the stream. */
		as_unlink(as);	/* this also sets AS state appropriately */
		as->tp.next = as_free;
		as_free = as;
	}

	tp_unlink(tp);		/* this also sets state appropriately */

	mi_detach(l->l_qtop, (caddr_t) tp);
	mi_close_unlink(&ua_links, (caddr_t) tp);

	sp_unlock(sp);
	write_unlock_irqrestore(&ua_mux_lock, flags);

	/* free structures outside locks */
	for (as = as_free; as; as = as->tp.next)
		as_free(as);
	sp_check_free(sp);	/* free it if it is now orphaned */
	tp_free_priv(tp);

	/* Should probably flush queues in case a Stream head is reattached. */
	flushq(RD(l->l_qtop), FLUSHALL);
	mi_copy_done(q, mp, 0);
	return (0);
}

/*
 *  INPUT-OUTPUT Control processing
 *  -------------------------------------------------------------------------
 */
static noinline fastcall __unlikely int
lm_i_ioctl(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int err;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_LINK):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> I_LINK");
		err = lm_i_link(lm, q, mp);
		break;
	case _IOC_NR(I_UNLINK):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> I_UNLINK");
		err = lm_i_unlink(lm, q, mp);
		break;
	case _IOC_NR(I_PLINK):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> I_PLINK");
		err = lm_i_plink(lm, q, mp);
		break;
	case _IOC_NR(I_PUNLINK):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> I_PUNLINK");
		err = lm_i_punlink(lm, q, mp);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	return (0);
}

static int
ua_size_opts(uint type, size_t header)
{
	switch (type) {
	case UA_OBJ_TYPE_DF:
		return (header + sizeof(struct ua_opt_conf_df));
	case UA_OBJ_TYPE_LM:
		return (header + sizeof(struct ua_opt_conf_lm));
	case UA_OBJ_TYPE_UP:
		return (header + sizeof(struct ua_opt_conf_up));
	case UA_OBJ_TYPE_AS:
		return (header + sizeof(struct ua_opt_conf_as));
	case UA_OBJ_TYPE_SP:
		return (header + sizeof(struct ua_opt_conf_sp));
	case UA_OBJ_TYPE_SG:
		return (header + sizeof(struct ua_opt_conf_sg));
	case UA_OBJ_TYPE_GP:
		return (header + sizeof(struct ua_opt_conf_gp));
	case UA_OBJ_TYPE_XP:
		return (header + sizeof(struct ua_opt_conf_xp));
	default:
		return (-1);
	}
}

static int
ua_size_conf(uint type, size_t header)
{
	switch (type) {
	case UA_OBJ_TYPE_DF:
		return (header + sizeof(struct ua_conf_df));
	case UA_OBJ_TYPE_LM:
		return (header + sizeof(struct ua_conf_lm));
	case UA_OBJ_TYPE_UP:
		return (header + sizeof(struct ua_conf_up));
	case UA_OBJ_TYPE_AS:
#if M2UA
		return (header + sizeof(struct ua_conf_as) + sizeof(struct m2ua_key));
#endif
#if M3UA
		return (header + sizeof(struct ua_conf_as) + sizeof(struct m3ua_key));
#endif
#if SUA
		return (header + sizeof(struct ua_conf_as) + sizeof(struct sua_key));
#endif
#if TUA
		return (header + sizeof(struct ua_conf_as) + sizeof(struct tua_key));
#endif
#if ISUA
		return (header + sizeof(struct ua_conf_as) + sizeof(struct isua_key));
#endif
	case UA_OBJ_TYPE_SP:
		return (header + sizeof(struct ua_conf_sp));
	case UA_OBJ_TYPE_SG:
		return (header + sizeof(struct ua_conf_sg));
	case UA_OBJ_TYPE_GP:
		return (header + sizeof(struct ua_conf_gp));
	case UA_OBJ_TYPE_XP:
		return (header + sizeof(struct ua_conf_xp));
	default:
		return (-1);
	}
}

static int
ua_size_statem(uint type, size_t header)
{
	switch (type) {
	case UA_OBJ_TYPE_DF:
		return (header + sizeof(struct ua_statem_df));
	case UA_OBJ_TYPE_LM:
		return (header + sizeof(struct ua_statem_lm));
	case UA_OBJ_TYPE_UP:
		return (header + sizeof(struct ua_statem_up));
	case UA_OBJ_TYPE_AS:
		return (header + sizeof(struct ua_statem_as));
	case UA_OBJ_TYPE_SP:
		return (header + sizeof(struct ua_statem_sp));
	case UA_OBJ_TYPE_SG:
		return (header + sizeof(struct ua_statem_sg));
	case UA_OBJ_TYPE_GP:
		return (header + sizeof(struct ua_statem_gp));
	case UA_OBJ_TYPE_XP:
		return (header + sizeof(struct ua_statem_xp));
	default:
		return (-1);
	}
}

static int
ua_size_stats(uint type, size_t header)
{
	switch (type) {
	case UA_OBJ_TYPE_DF:
		return (header + sizeof(struct ua_stats_df));
	case UA_OBJ_TYPE_LM:
		return (header + sizeof(struct ua_stats_lm));
	case UA_OBJ_TYPE_UP:
		return (header + sizeof(struct ua_stats_up));
	case UA_OBJ_TYPE_AS:
		return (header + sizeof(struct ua_stats_as));
	case UA_OBJ_TYPE_SP:
		return (header + sizeof(struct ua_stats_sp));
	case UA_OBJ_TYPE_SG:
		return (header + sizeof(struct ua_stats_sg));
	case UA_OBJ_TYPE_GP:
		return (header + sizeof(struct ua_stats_gp));
	case UA_OBJ_TYPE_XP:
		return (header + sizeof(struct ua_stats_xp));
	default:
		return (-1);
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
ua_as_ioctl(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(UA_AS_IOCGOPTIONS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGOPTIONS)");
		size = sizeof(struct ua_option);
		break;
	case _IOC_NR(UA_AS_IOCSOPTIONS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCSOPTIONS)");
		size = sizeof(struct ua_option);
		break;
	case _IOC_NR(UA_AS_IOCSCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCSCONFIG)");
		size = sizeof(struct ua_config);
		break;
	case _IOC_NR(UA_AS_IOCGCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGCONFIG)");
		size = sizeof(struct ua_config);
		break;
	case _IOC_NR(UA_AS_IOCTCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCTCONFIG)");
		size = sizeof(struct ua_config);
		break;
	case _IOC_NR(UA_AS_IOCCCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCCONFIG)");
		size = sizeof(struct ua_config);
		break;
	case _IOC_NR(UA_AS_IOCLCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCLCONFIG)");
		size = sizeof(struct ua_config);
		break;
	case _IOC_NR(UA_AS_IOCGSTATEM):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGSTATEM)");
		size = sizeof(struct ua_statem);
		break;
	case _IOC_NR(UA_AS_IOCCMRESET):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCMRESET)");
		size = sizeof(struct ua_statem);
		break;
	case _IOC_NR(UA_AS_IOCGSTATSP):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGSTATSP)");
		size = sizeof(struct ua_stats);
		break;
	case _IOC_NR(UA_AS_IOCSSTATSP):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCSSTATSP)");
		size = sizeof(struct ua_stats);
		break;
	case _IOC_NR(UA_AS_IOCGSTATS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGSTATS)");
		size = sizeof(struct ua_stats);
		break;
	case _IOC_NR(UA_AS_IOCCSTATS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCSTATS)");
		size = sizeof(struct ua_stats);
		break;
	case _IOC_NR(UA_AS_IOCGNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCGNOTIFY)");
		size = sizeof(struct ua_notify);
		break;
	case _IOC_NR(UA_AS_IOCSNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCSNOTIFY)");
		size = sizeof(struct ua_notify);
		break;
	case _IOC_NR(UA_AS_IOCCNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCNOTIFY)");
		size = sizeof(struct ua_notify);
		break;
	case _IOC_NR(UA_AS_IOCCMGMT):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCMGMT)");
		size = sizeof(struct ua_mgmt);
		break;
	case _IOC_NR(UA_AS_IOCCPASS):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(UA_AS_IOCCPASS)");
		size = sizeof(struct ua_pass);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
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
ua_as_copyin(struct up *lm, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp;
	void *obj;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(UA_AS_IOCGOPTIONS):
	{
		struct ua_option *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGOPTIONS)");
		if ((size = ua_size_opts(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(&err, lm, p)))
			break;
		switch (p->type) {
		case UA_OBJ_TYPE_DF:
			*(struct ua_opt_conf_df *) o = ((struct df *) obj)->options;
			break;
		case UA_OBJ_TYPE_LM:
			*(struct ua_opt_conf_lm *) o = ((struct up *) obj)->lm.options;
			break;
		case UA_OBJ_TYPE_UP:
			*(struct ua_opt_conf_up *) o = ((struct up *) obj)->up.options;
			break;
		case UA_OBJ_TYPE_AS:
			*(struct ua_opt_conf_as *) o = ((struct up *) obj)->as.options;
			break;
		case UA_OBJ_TYPE_SP:
			*(struct ua_opt_conf_sp *) o = ((struct sp *) obj)->sp.options;
			break;
		case UA_OBJ_TYPE_SG:
			*(struct ua_opt_conf_sg *) o = ((struct sg *) obj)->sg.options;
			break;
		case UA_OBJ_TYPE_GP:
			*(struct ua_opt_conf_gp *) o = ((struct gp *) obj)->gp.options;
			break;
		case UA_OBJ_TYPE_XP:
			*(struct ua_opt_conf_xp *) o = ((struct tp *) obj)->xp.options;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCSOPTIONS):
	{
		struct ua_option *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSOPTIONS)");
		if ((size = ua_size_opts(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(UA_AS_IOCSCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSCONFIG)");
		if ((size = ua_size_conf(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(UA_AS_IOCGCONFIG):
	{
		struct ua_config *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGCONFIG)");
		if ((size = ua_size_conf(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(&err, lm, p)))
			break;
		switch (p->type) {
		case UA_OBJ_TYPE_DF:
			*(struct ua_conf_df *) o = ((struct df *) obj)->config;
			break;
		case UA_OBJ_TYPE_LM:
			*(struct ua_conf_lm *) o = ((struct up *) obj)->lm.config;
			break;
		case UA_OBJ_TYPE_UP:
			*(struct ua_conf_up *) o = ((struct up *) obj)->up.config;
			break;
		case UA_OBJ_TYPE_AS:
			*(struct ua_conf_as *) o = ((struct up *) obj)->as.config;
			break;
		case UA_OBJ_TYPE_SP:
			*(struct ua_conf_sp *) o = ((struct sp *) obj)->sp.config;
			break;
		case UA_OBJ_TYPE_SG:
			*(struct ua_conf_sg *) o = ((struct sg *) obj)->sg.config;
			break;
		case UA_OBJ_TYPE_GP:
			*(struct ua_conf_gp *) o = ((struct gp *) obj)->gp.config;
			break;
		case UA_OBJ_TYPE_XP:
			*(struct ua_conf_xp *) o = ((struct tp *) obj)->xp.config;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCTCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCTCONFIG)");
		if ((size = ua_size_conf(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(UA_AS_IOCCCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCCONFIG)");
		if ((size = ua_size_conf(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(UA_AS_IOCLCONFIG):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCLCONFIG)");
		/* not supported just yet */
		goto eopnotsupp;
	case _IOC_NR(UA_AS_IOCGSTATEM):
	{
		struct ua_statem *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGSTATEM)");
		if ((size = ua_size_statem(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o = *p;

		if (!(obj = find_object(&err, lm, p)))
			break;
		switch (p->type) {
		case UA_OBJ_TYPE_DF:
			o->state = ((struct df *) obj)->state;
			*(struct ua_statem_df *) (o + 1) = ((struct df *) obj)->statem;
			break;
		case UA_OBJ_TYPE_LM:
			o->state = ((struct up *) obj)->lm.state;
			*(struct ua_statem_lm *) (o + 1) = ((struct up *) obj)->lm.statem;
			break;
		case UA_OBJ_TYPE_UP:
			o->state = ((struct up *) obj)->up.state;
			*(struct ua_statem_up *) (o + 1) = ((struct up *) obj)->up.statem;
			break;
		case UA_OBJ_TYPE_AS:
			o->state = ((struct up *) obj)->as.state;
			*(struct ua_statem_as *) (o + 1) = ((struct up *) obj)->as.statem;
			break;
		case UA_OBJ_TYPE_SP:
			o->state = ((struct sp *) obj)->sp.state;
			*(struct ua_statem_sp *) (o + 1) = ((struct sp *) obj)->sp.statem;
			break;
		case UA_OBJ_TYPE_SG:
			o->state = ((struct sg *) obj)->sg.state;
			*(struct ua_statem_sg *) (o + 1) = ((struct sg *) obj)->sg.statem;
			break;
		case UA_OBJ_TYPE_GP:
			o->state = ((struct gp *) obj)->gp.state;
			*(struct ua_statem_gp *) (o + 1) = ((struct gp *) obj)->gp.statem;
			break;
		case UA_OBJ_TYPE_XP:
			o->state = ((struct tp *) obj)->xp.state;
			*(struct ua_statem_xp *) (o + 1) = ((struct tp *) obj)->xp.statem;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCCMRESET):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCMRESET)");
		goto eopnotsupp;
	case _IOC_NR(UA_AS_IOCGSTATSP):
	{
		struct ua_stats *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGSTATSP)");
		if ((size = ua_size_stats(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(&err, lm, p)))
			break;
		switch (p->type) {
		case UA_OBJ_TYPE_DF:
			*(struct ua_stats_df *) o = ((struct df *) obj)->statsp;
			break;
		case UA_OBJ_TYPE_LM:
			*(struct ua_stats_lm *) o = ((struct up *) obj)->lm.statsp;
			break;
		case UA_OBJ_TYPE_UP:
			*(struct ua_stats_up *) o = ((struct up *) obj)->up.statsp;
			break;
		case UA_OBJ_TYPE_AS:
			*(struct ua_stats_as *) o = ((struct up *) obj)->as.statsp;
			break;
		case UA_OBJ_TYPE_SP:
			*(struct ua_stats_sp *) o = ((struct sp *) obj)->sp.statsp;
			break;
		case UA_OBJ_TYPE_SG:
			*(struct ua_stats_sg *) o = ((struct sg *) obj)->sg.statsp;
			break;
		case UA_OBJ_TYPE_GP:
			*(struct ua_stats_gp *) o = ((struct gp *) obj)->gp.statsp;
			break;
		case UA_OBJ_TYPE_XP:
			*(struct ua_stats_xp *) o = ((struct tp *) obj)->xp.statsp;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCSSTATSP):
	{
		struct ua_stats *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSSTATSP)");
		if ((size = ua_size_stats(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(UA_AS_IOCGSTATS):
	{
		struct ua_stats *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGSTATS)");
		if ((size = ua_size_stats(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(&err, lm, p)))
			break;
		switch (p->type) {
		case UA_OBJ_TYPE_DF:
			*(struct ua_stats_df *) o = ((struct df *) obj)->stats;
			break;
		case UA_OBJ_TYPE_LM:
			*(struct ua_stats_lm *) o = ((struct up *) obj)->lm.stats;
			break;
		case UA_OBJ_TYPE_UP:
			*(struct ua_stats_up *) o = ((struct up *) obj)->up.stats;
			break;
		case UA_OBJ_TYPE_AS:
			*(struct ua_stats_as *) o = ((struct up *) obj)->as.stats;
			break;
		case UA_OBJ_TYPE_SP:
			*(struct ua_stats_sp *) o = ((struct sp *) obj)->sp.stats;
			break;
		case UA_OBJ_TYPE_SG:
			*(struct ua_stats_sg *) o = ((struct sg *) obj)->sg.stats;
			break;
		case UA_OBJ_TYPE_GP:
			*(struct ua_stats_gp *) o = ((struct gp *) obj)->gp.stats;
			break;
		case UA_OBJ_TYPE_XP:
			*(struct ua_stats_xp *) o = ((struct tp *) obj)->xp.stats;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCCSTATS):
	{
		struct ua_stats *o, *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCSTATS)");
		if ((size = ua_size_stats(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		if (!(obj = find_object(&err, lm, p)))
			break;
		switch (p->type) {
		case UA_OBJ_TYPE_DF:
			*(struct ua_stats_df *) o = ((struct df *) obj)->stats;
			bzero(&((struct df *) obj)->stats, sizeof(((struct df *) obj)->stats));
			break;
		case UA_OBJ_TYPE_LM:
			*(struct ua_stats_lm *) o = ((struct up *) obj)->lm.stats;
			bzero(&((struct up *) obj)->lm.stats,
			      sizeof(((struct up *) obj)->lm.stats));
			break;
		case UA_OBJ_TYPE_UP:
			*(struct ua_stats_up *) o = ((struct up *) obj)->up.stats;
			bzero(&((struct up *) obj)->up.stats,
			      sizeof(((struct up *) obj)->up.stats));
			break;
		case UA_OBJ_TYPE_AS:
			*(struct ua_stats_as *) o = ((struct up *) obj)->as.stats;
			bzero(&((struct up *) obj)->as.stats,
			      sizeof(((struct up *) obj)->as.stats));
			break;
		case UA_OBJ_TYPE_SP:
			*(struct ua_stats_sp *) o = ((struct sp *) obj)->sp.stats;
			bzero(&((struct sp *) obj)->sp.stats,
			      sizeof(((struct sp *) obj)->sp.stats));
			break;
		case UA_OBJ_TYPE_SG:
			*(struct ua_stats_sg *) o = ((struct sg *) obj)->sg.stats;
			bzero(&((struct sg *) obj)->sg.stats,
			      sizeof(((struct sg *) obj)->sg.stats));
			break;
		case UA_OBJ_TYPE_GP:
			*(struct ua_stats_gp *) o = ((struct gp *) obj)->gp.stats;
			bzero(&((struct gp *) obj)->gp.stats,
			      sizeof(((struct gp *) obj)->gp.stats));
			break;
		case UA_OBJ_TYPE_XP:
			*(struct ua_stats_xp *) o = ((struct tp *) obj)->xp.stats;
			bzero(&((struct tp *) obj)->xp.stats,
			      sizeof(((struct tp *) obj)->xp.stats));
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCGNOTIFY):
	{
		struct ua_notify *o, *p = (typeof(p)) dp->b_rptr;
		int size;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCGNOTIFY)");
		if ((size = size_ntfy_by_type(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o = *p;

		if (!(obj = find_object(&err, lm, p)))
			break;
		switch (p->type) {
		case UA_OBJ_TYPE_DF:
			o->notify = ((struct df *) obj)->events;
			break;
		case UA_OBJ_TYPE_LM:
			o->notify = ((struct up *) obj)->lm.events;
			break;
		case UA_OBJ_TYPE_UP:
			o->notify = ((struct up *) obj)->up.events;
			break;
		case UA_OBJ_TYPE_AS:
			o->notify = ((struct up *) obj)->as.events;
			break;
		case UA_OBJ_TYPE_SP:
			o->notify = ((struct sp *) obj)->sp.events;
			break;
		case UA_OBJ_TYPE_SG:
			o->notify = ((struct sg *) obj)->sg.events;
			break;
		case UA_OBJ_TYPE_GP:
			o->notify = ((struct gp *) obj)->gp.events;
			break;
		case UA_OBJ_TYPE_XP:
			o->notify = ((struct tp *) obj)->xp.events;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCSNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSNOTIFY)");
		goto eopnotsupp;
	case _IOC_NR(UA_AS_IOCCNOTIFY):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCNOTIFY)");
		goto eopnotsupp;
	case _IOC_NR(UA_AS_IOCCMGMT):
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCMGMT)");
		goto eopnotsupp;
	case _IOC_NR(UA_AS_IOCCPASS):
	{
		struct ua_pass *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCPASS)");
		size = sizeof(*p);
		if (p->ctl_length)
			if (sizeof(*p) + p->ctl_length > size)
				size = sizeof(*p) + p->ctl_length;
		if (p->dat_length)
			if (sizeof(*p) + p->dat_length > size)
				size = sizeof(*p) + p->ctl_length + p->dat_length;
		break;
	}
	default:
		goto eopnotsupp;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin_n(q, mp, 0, size);
	}
	return (0);
      efault:
	err = EFAULT;
	goto done;
      eopnotsupp:
	err = EOPNOTSUPP;
	goto done;
      enobufs:
	err = ENOBUFS;
	goto done;
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
ua_as_copyin2(struct up *lm, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;
	struct sp *sp;
	void *obj;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(UA_AS_IOCSOPTIONS):
	{
		struct ua_option *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSOPTIONS)");
		if (!(obj = find_object(&err, lm, p)))
			break;
		switch (p->type) {
		case UA_OBJ_TYPE_DF:
			((struct df *) obj)->options = *(struct ua_opt_conf_df *) (p + 1);
			break;
		case UA_OBJ_TYPE_LM:
			((struct up *) obj)->lm.options = *(struct ua_opt_conf_lm *) (p + 1);
			break;
		case UA_OBJ_TYPE_UP:
			((struct up *) obj)->up.options = *(struct ua_opt_conf_up *) (p + 1);
			break;
		case UA_OBJ_TYPE_AS:
			((struct up *) obj)->as.options = *(struct ua_opt_conf_as *) (p + 1);
			break;
		case UA_OBJ_TYPE_SP:
			((struct sp *) obj)->sp.options = *(struct ua_opt_conf_sp *) (p + 1);
			break;
		case UA_OBJ_TYPE_SG:
			((struct sg *) obj)->sg.options = *(struct ua_opt_conf_sg *) (p + 1);
			break;
		case UA_OBJ_TYPE_GP:
			((struct gp *) obj)->gp.options = *(struct ua_opt_conf_gp *) (p + 1);
			break;
		case UA_OBJ_TYPE_XP:
			((struct tp *) obj)->xp.options = *(struct ua_opt_conf_xp *) (p + 1);
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCSCONFIG):
	{
		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSCONFIG)");
		goto sconfig;
	}
	case _IOC_NR(UA_AS_IOCCCONFIG):
	{
		struct ua_config *p;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCCONFIG)");
	      sconfig:
		p = (typeof(p)) dp->b_rptr;
		switch (p->cmd) {
		case UA_ADD:
			err = add_object(lm, q, p);
			break;
		case UA_CHA:
			err = cha_object(lm, q, p);
			break;
		case UA_DEL:
			err = del_object(lm, q, p);
			break;
		case UA_MOD:
			err = mod_object(lm, q, p);
			break;
		default:
			err = EINVAL;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCTCONFIG):
	{
		struct ua_config *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCTCONFIG)");
		if (!(obj = find_object(&err, lm, p)))
			break;
		/* FIXME: do something */
		break;
	}
	case _IOC_NR(UA_AS_IOCSSTATSP):
	{
		struct ua_stats *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCSSTATSP)");
		if (!(obj = find_object(&err, lm, p)))
			break;
		switch (p->type) {
		case UA_OBJ_TYPE_DF:
			((struct df *) obj)->statsp = *(struct ua_stats_df *) (p + 1);
			break;
		case UA_OBJ_TYPE_LM:
			((struct up *) obj)->lm.statsp = *(struct ua_stats_lm *) (p + 1);
			break;
		case UA_OBJ_TYPE_UP:
			((struct up *) obj)->up.statsp = *(struct ua_stats_up *) (p + 1);
			break;
		case UA_OBJ_TYPE_AS:
			((struct as *) obj)->as.statsp = *(struct ua_stats_as *) (p + 1);
			break;
		case UA_OBJ_TYPE_SP:
			((struct sp *) obj)->sp.statsp = *(struct ua_stats_sp *) (p + 1);
			break;
		case UA_OBJ_TYPE_SG:
			((struct sg *) obj)->sg.statsp = *(struct ua_stats_sg *) (p + 1);
			break;
		case UA_OBJ_TYPE_GP:
			((struct gp *) obj)->gp.statsp = *(struct ua_stats_gp *) (p + 1);
			break;
		case UA_OBJ_TYPE_XP:
			((struct tp *) obj)->xp.statsp = *(struct ua_stats_xp *) (p + 1);
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(UA_AS_IOCCPASS):
	{
		struct ua_pass *p = (typeof(p)) dp->b_rptr;

		strlog(lm->mid, lm->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(UA_AS_IOCCPASS)");
		if (p->ctl_length + p->dat_length == 0)
			break;
		err = EOPNOTSUPP;
		break;
	}
	default:
		err = EOPNOTSUPP;
		break;
	}
	if (err < 0)
		return (err);
	mi_copy_done(q, mp, err);
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
ua_as_copyout(struct up *lm, queue_t *q, mblk_t *mp)
{
	mi_copyout(q, mp);
	return (0);
}

#if M2UA
static noinline fastcall __unlikely int
sl_ioctl(struct up *up, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tp *tp;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SL_IOCGOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGOPTIONS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_option), false)))
			goto enobufs;
		*(struct lmi_option *) bp->b_rptr = up->up.options;
		break;
	case _IOC_NR(SL_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSOPTIONS)");
		size = sizeof(struct lmi_option);
		break;
	case _IOC_NR(SL_IOCGCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGCONFIG)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_config), false)))
			goto enobufs;
		*(struct sl_config *) bp->b_rptr = up->up.config;
		break;
	case _IOC_NR(SL_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSCONFIG)");
		size = sizeof(struct sl_config);
		break;
	case _IOC_NR(SL_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCTCONFIG)");
		size = sizeof(struct sl_config);
		break;
	case _IOC_NR(SL_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCCCONFIG)");
	      size = sizeof(struct sl_config):
		break;
	case _IOC_NR(SL_IOCGSTATEM):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGSTATEM)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_statem), false)))
			goto enobufs;
		*(struct sl_statem *) bp->b_rptr = up->up.statem;
		break;
	case _IOC_NR(SL_IOCCMRESET):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCCMRESET)");
		goto eopnotsupp;
	case _IOC_NR(SL_IOCGSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGSTATSP)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_stats), false)))
			goto enobufs;
		*(struct sl_stats *) bp->b_rptr = up->up.statsp;
		break;
	case _IOC_NR(SL_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSSTATSP)");
	      size = sizeof(struct sl_stats):
		break;
	case _IOC_NR(SL_IOCGSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_stats), false)))
			goto enobufs;
		*(struct sl_stats *) bp->b_rptr = up->up.stats;
		break;
	case _IOC_NR(SL_IOCCSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sl_stats), false)))
			goto enobufs;
		*(struct sl_stats *) bp->b_rptr = up->up.stats;
		bzero(&up->up.stats, sizeof(up->up.stats));
		break;
	case _IOC_NR(SL_IOCGNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCGNOTIFY)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_notify), false)))
			goto enobufs;
		*(struct lmi_notify *) bp->b_rptr = up->up.events;
		break;
	case _IOC_NR(SL_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCSNOTIFY)");
		size = sizeof(struct lmi_notify);
		break;
	case _IOC_NR(SL_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SL_IOCCNOTIFY)");
		size = sizeof(struct lmi_notify);
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
      eopnotsupp:
	err = EOPNOTSUPP;
	goto done;
      enobufs:
	err = ENOBUFS;
	goto done;
}
static noinline fastcall __unlikely int
sl_copyin(struct up *up, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tp *tp;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SL_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCSOPTIONS)");
		up->up.options = *(struct lmi_option *) dp->b_rptr;
		break;
	case _IOC_NR(SL_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCSCONFIG)");
		up->up.config = *(struct sl_config *) dp->b_rptr;
		break;
	case _IOC_NR(SL_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCTCONFIG)");
		/* FIXME: actually test and lock configuration */
		break;
	case _IOC_NR(SL_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCCCONFIG)");
		up->up.config = *(struct sl_config *) dp->b_rptr;
		/* FIXME: unlock configuration */
		break;
	case _IOC_NR(SL_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCSSTATSP)");
		up->up.statsp = *(struct sl_stats *) dp->b_rptr;
		break;
	case _IOC_NR(SL_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCSNOTIFY)");
		up->up.events.events |= ((struct lmi_notify *) dp->b_rptr)->events;
		break;
	case _IOC_NR(SL_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SL_IOCCNOTIFY)");
		up->up.events.events &= ~((struct lmi_notify *) dp->b_rptr)->events;
		break;
	case default:
		err = EPROTO;
		break;
	}
	mi_copy_done(q, mp, err);
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
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDT_IOCGOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGOPTIONS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_option), false)))
			goto enobufs;
		*(struct lmi_option *) bp->b_rptr = up->sdt.options;
		break;
	case _IOC_NR(SDT_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSOPTIONS)");
		size = sizeof(struct lmi_option);
		break;
	case _IOC_NR(SDT_IOCGCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGCONFIG)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_config), false)))
			goto enobufs;
		*(struct sdt_config *) bp->b_rptr = up->sdt.config;
		break;
	case _IOC_NR(SDT_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSCONFIG)");
		size = sizeof(struct sdt_config);
		break;
	case _IOC_NR(SDT_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCTCONFIG)");
		size = sizeof(struct sdt_config);
		break;
	case _IOC_NR(SDT_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCCONFIG)");
		size = sizeof(struct sdt_config);
		break;
	case _IOC_NR(SDT_IOCGSTATEM):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGSTATEM)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_statem), false)))
			goto enobufs;
		*(struct sdt_statem *) bp->b_rptr = up->sdt.statem;
		break;
	case _IOC_NR(SDT_IOCCMRESET):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCMRESET)");
		goto eopnotsupp;
	case _IOC_NR(SDT_IOCGSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGSTATSP)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_stats), false)))
			goto enobufs;
		*(struct sdt_stats *) bp->b_rptr = up->sdt.statsp;
		break;
	case _IOC_NR(SDT_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSSTATSP)");
		size = sizeof(struct sdt_stats);
		break;
	case _IOC_NR(SDT_IOCGSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_stats), false)))
			goto enobufs;
		*(struct sdt_stats *) bp->b_rptr = up->sdt.stats;
		break;
	case _IOC_NR(SDT_IOCCSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCSTATS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct sdt_stats), false)))
			goto enobufs;
		*(struct sdt_stats *) bp->b_rptr = up->sdt.stats;
		bzero(&up->sdt.stats, sizeof(up->sdt.stats));
		break;
	case _IOC_NR(SDT_IOCGNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCGNOTIFY)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_notify), false)))
			goto enobufs;
		*(struct lmi_notify *) bp->b_rptr = up->sdt.events;
		break;
	case _IOC_NR(SDT_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCSNOTIFY)");
		size = sizeof(struct lmi_notify);
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCNOTIFY)");
		size = sizeof(struct lmi_notify);
		break;
	case _IOC_NR(SDT_IOCCABORT):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDT_IOCCABORT)");
		goto eopnotsupp;
	default:
		goto eopnotsupp;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
      eopnotsupp:
	err = EOPNOTSUPP;
	goto done;
      enobufs:
	err = ENOBUFS;
	goto done;

}
static noinline fastcall __unlikely int
sdt_copyin(struct up *up, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tp *tp;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDT_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSOPTIONS)");
		up->sdt.options = *(struct lmi_option *) dp->b_rptr;
		break;
	case _IOC_NR(SDT_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSCONFIG)");
		up->sdt.config = *(struct sdt_config *) dp->b_rptr;
		break;
	case _IOC_NR(SDT_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCTCONFIG)");
		/* FIXME: actually test and lock configuration */
		break;
	case _IOC_NR(SDT_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCCCONFIG)");
		up->sdt.config = *(struct sdt_config *) dp->b_rptr;
		/* FIXME: actually unlock configuration */
		break;
	case _IOC_NR(SDT_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSSTATSP)");
		up->sdt.statsp = *(struct sdt_stats *) dp->b_rptr;
		break;
	case _IOC_NR(SDT_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCSNOTIFY)");
		up->sdt.events.events |= ((struct lmi_notify *) dp->b_rptr)->events;
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDT_IOCCNOTIFY)");
		up->sdt.events.events &= ~((struct lmi_notify *) dp->b_rptr)->events;
		break;
	default:
		err = EPROTO;
		break;
	}
	mi_copy_done(q, mp, err);
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
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDL_IOCGOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGOPTIONS)");
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(struct lmi_option), false)))
			goto enobufs;
		*(struct lmi_option *) bp->b_rptr = up->sdl.options;
		break;
	case _IOC_NR(SDL_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSOPTIONS)");
		size = sizeof(struct lmi_option);
		break;
	case _IOC_NR(SDL_IOCGCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGCONFIG)");
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_config), false)))
			goto enobufs;
		*(struct sdl_config *) bp->b_rptr = up->sdl.config;
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSCONFIG)");
		size = sizeof(struct sdl_config);
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCTCONFIG)");
		size = sizeof(struct sdl_config);
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCCONFIG)");
		size = sizeof(struct sdl_config);
		break;
	case _IOC_NR(SDL_IOCGSTATEM):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGSTATEM)");
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_statem), false)))
			goto enobufs;
		*(struct sdl_statem *) bp->b_rptr = up->sdl.statem;
		break;
	case _IOC_NR(SDL_IOCCMRESET):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCMRESET)");
		goto eopnotsupp;
	case _IOC_NR(SDL_IOCGSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGSTATSP)");
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_stats), false)))
			goto enobufs;
		*(struct sdl_stats *) bp->b_rptr = up->sdl.statsp;
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSSTATSP)");
		goto eopnotsupp;
	case _IOC_NR(SDL_IOCGSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGSTATS)");
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_stats), false)))
			goto enobufs;
		*(struct sdl_stats *) bp->b_rptr = up->sdl.stats;
		break;
	case _IOC_NR(SDL_IOCCSTATS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCSTATS)");
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct sdl_stats), false)))
			goto enobufs;
		*(struct sdl_stats *) bp->b_rptr = up->sdl.stats;
		bzero(&up->sdl.stats, sizeof(up->sdl.stats));
		break;
	case _IOC_NR(SDL_IOCGNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCGNOTIFY)");
		if (!(bp = mi_copyout_aloc(q, mp, NULL, sizeof(struct lmi_notify), false)))
			goto enobufs;
		*(struct lmi_notify *) bp->b_rptr = up->sdl.events;
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCSNOTIFY)");
		size = sizeof(lmi_notify);
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCNOTIFY)");
		size = sizeof(lmi_notify);
		break;
	case _IOC_NR(SDL_IOCCDISCTX):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCDISCTX)");
		goto eopnotsupp;
	case _IOC_NR(SDL_IOCCCONNTX):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCTL(SDL_IOCCCONNTX)");
		goto eopnotsupp;
	default:
		goto eopnotsupp;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
      eopnotsupp:
	err = EOPNOTSUPP;
	goto done;
      enobufs:
	err = ENOBUFS;
	goto done;
}
static noinline fastcall __unlikely int
sdl_copyin(struct up *up, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tp *tp;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDL_IOCSOPTIONS):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSOPTIONS)");
		up->sdl.options = *(struct lmi_option *) dp->b_rptr;
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSCONFIG)");
		up->sdl.config = *(struct sdl_config *) dp->b_rptr;
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCTCONFIG)");
		/* FIXME: actually test and lock configuration */
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCCCONFIG)");
		up->sdl.config = *(struct sdl_config *) dp->b_rptr;
		/* FIXME: unlock configuration */
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSSTATSP)");
		up->sdl.statsp = *(struct sdl_stats *) dp->b_rptr;
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCSNOTIFY)");
		up->sdl.events.events |= ((struct lmi_notify *) dp->b_rptr)->events;
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCCNOTIFY)");
		up->sdl.events.events &= ~((struct lmi_notify *) dp->b_rptr)->events;
		break;
	default:
		err = EPROTO;
		break;
	}
	mi_copy_done(q, mp, err);
	return (0);
}
static noinline fastcall __unlikely int
sdl_copyout(struct up *up, queue_t *q, mblk_t *mp)
{
	mi_copyout(q, mp);
	return (0);
}
#endif

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
	struct up *up;
	struct tp *tp;
	int err;

	if (!(up = up_acquire(q)))
		return (-EDEADLK);

#if M2UA
	switch (dl_get_state(up)) {
	case DL_DATAXFER:
		tp = up->as.as->sp.sp->sg.list->gp.list->xp.xp;
		if ((up->up.option.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
			err = as_send_maup_data1(up, tp, q, NULL, mp);
		} else {
			if ((err = as_send_maup_data2(up, tp, q, NULL, mp, *mp->b_rptr++)) != 0)
				mp->b_rptr--;
		}
		break;
	default:
		up_release(up);
		goto outstate;
	}
#endif
#if M3UA
	switch (up_get_state(up)) {
	case MTPS_IDLE:
		/* XXX Well.... must be connectionless. */
		err = as_send_xfer_data(up, q, &up->orig, &up->dest, 0, 0, mp);
		break;
	case MTPS_CONNECTED:
		/* XXX Well.... must be connection oriented. */
		err = as_send_xfer_data(up, q, &up->orig, &up->dest, 0, 0, mp);
		break;
	default:
		up_release(up);
		goto outstate;
	}
#endif
#if SUA
	switch (up_get_state(up)) {
	case NS_IDLE:
		/* XXX Well.... must be connectionless. */
		err = up_send_clns(up, q, &up->orig, &up->dest, 0, 0, mp);
		break;
	case NS_DATA_XFER:
		/* XXX Well.... must be connection oriented. */
		err = up_send_cons(up, q, 0, 0, mp);
		break;
	default:
		up_release(up);
		goto outstate;
	}
#endif
#if TUA
	/* FIXME */
#endif
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
	struct sp *sp;
	int oldstate;
	uint32_t prim;
	int rtn = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		strlog(up->mid, up->sid, 0, SL_ERROR, "-> primitive too short");
		/* should issue M_ERROR */
		freemsg(mp);
		return (0);
	}
	prim = *(uint32_t *) mp->b_rptr;

	read_lock(&ua_mux_lock);
	if (!(sp = up->sp.sp)) {
		read_unlock(&ua_mux_lock);
		/* have not been associated with SP yet */
#if M2UA
		switch (prim) {
		case LMI_INFO_REQ:
			return lmi_info_req(up, q, mp);
		case LMI_ATTACH_REQ:
			return lmi_attach_req(up, q, mp);
		case LMI_OPTMGMT_REQ:
			return lmi_optmgmt_req(up, q, mp);
		default:
			return lmi_error_ack(up, q, mp, prim, LMI_OUTSTATE);
		}
#endif
#if M3UA
		switch (prim) {
		case MTP_INFO_REQ:
			return mtp_info_req(up, q, mp);
		case MTP_BIND_REQ:
			return mtp_bind_req(up, q, mp);
		case MTP_OPTMGMT_REQ:
			return mtp_optmgmt_req(up, q, mp);
		default:
			return mtp_error_ack(up, q, mp, prim, MOUTSTATE);
		}
#endif
#if SUA
		switch (prim) {
		case N_INFO_REQ:
			return n_info_req(up, q, mp);
		case N_BIND_REQ:
			return n_bind_req(up, q, mp);
		case N_OPTMGMT_REQ:
			return n_optmgmt_req(up, q, mp);
		default:
			return n_error_ack(up, q, mp, prim, NOUTSTATE);
		}
#endif
#if TUA
		/* FIXME */
#endif
	}
	if (!sp_trylock(sp, q)) {
		read_unlock(&ua_mux_lock);
		return (-EDEADLK);
	}
	read_unlock(&ua_mux_lock);

	oldstate = up_get_state(up);

	switch (prim) {
#if M2UA
	case LMI_INFO_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_INFO_REQ");
		rtn = lmi_info_req(up, q, mp);
		break;
	case LMI_ATTACH_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_ATTACH_REQ");
		rtn = lmi_attach_req(up, q, mp);
		break;
	case LMI_DETACH_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_DETACH_REQ");
		rtn = lmi_detach_req(up, q, mp);
		break;
	case LMI_ENABLE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_ENABLE_REQ");
		rtn = lmi_enable_req(up, q, mp);
		break;
	case LMI_DISABLE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_DISABLE_REQ");
		rtn = lmi_disable_req(up, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> LMI_OPTMGMT_REQ");
		rtn = lmi_optmgmt_req(up, q, mp);
		break;
	case SL_PDU_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_PDU_REQ");
		rtn = sl_pdu_req(up, q, mp);
		break;
	case SL_EMERGENCY_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_EMERGENCY_REQ");
		rtn = sl_emergency_req(up, q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_EMERGENCY_CEASES_REQ");
		rtn = sl_emergency_ceases_req(up, q, mp);
		break;
	case SL_START_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_START_REQ");
		rtn = sl_start_req(up, q, mp);
		break;
	case SL_STOP_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_STOP_REQ");
		rtn = sl_stop_req(up, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_RETRIEVE_BSNT_REQ");
		rtn = sl_retrieve_bsnt_req(up, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_RETRIEVAL_REQUEST_AND_FSNC_REQ");
		rtn = sl_retreival_request_and_fsnc_req(up, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CLEAR_BUFFERS_REQ");
		rtn = sl_clear_buffers_req(up, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CLEAR_RTB_REQ");
		rtn = sl_clear_rtb_req(up, q, mp);
		break;
	case SL_CONTINUE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CONTINUE_REQ");
		rtn = sl_continue_req(up, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_LOCAL_PROCESSOR_OUTAGE_REQ");
		rtn = sl_local_processor_outage_req(up, q, mp);
		break;
	case SL_RESUME_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_RESUME_REQ");
		rtn = sl_resume_req(up, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CONGESTION_DISCARD_REQ");
		rtn = sl_congestion_discard_req(up, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_CONGESTION_ACCEPT_REQ");
		rtn = sl_congestion_accept_req(up, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_NO_CONGESTION_REQ");
		rtn = sl_no_congestion_req(up, q, mp);
		break;
	case SL_POWER_ON_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_POWER_ON_REQ");
		rtn = sl_power_on_req(up, q, mp);
		break;
#if 0
	case SL_OPTMGMT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_OPTMGMT_REQ");
		rtn = sl_optmgmt_req(up, q, mp);
		break;
	case SL_NOTIFY_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_NOTIFY_REQ");
		rtn = sl_notify_req(up, q, mp);
		break;
#endif
	default:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> SL_????_???");
		rtn = sl_other_req(up, q, mp);
		break;
#endif
#if M3UA
	case MTP_BIND_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> MTP_BIND_REQ");
		rtn = mtp_bind_req(up, q, mp);
		break;
	case MTP_UNBIND_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> MTP_UNBIND_REQ");
		rtn = mtp_unbind_req(up, q, mp);
		break;
	case MTP_CONN_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> MTP_CONN_REQ");
		rtn = mtp_conn_req(up, q, mp);
		break;
	case MTP_DISCON_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> MTP_DISCON_REQ");
		rtn = mtp_discon_req(up, q, mp);
		break;
	case MTP_ADDR_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> MTP_ADDR_REQ");
		rtn = mtp_addr_req(up, q, mp);
		break;
	case MTP_INFO_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> MTP_INFO_REQ");
		rtn = mtp_info_req(up, q, mp);
		break;
	case MTP_OPTMGMT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> MTP_OPTMGMT_REQ");
		rtn = mtp_optmgmt_req(up, q, mp);
		break;
	case MTP_TRANSFER_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> MTP_TRANSFER_REQ");
		rtn = mtp_transfer_req(up, q, mp);
		break;
	default:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> MTP_????_???");
		rtn = mtp_other_req(up, q, mp);
		break;
#endif
#if SUA
	case N_CONN_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_CONN_REQ");
		rtn = n_conn_req(up, q, mp);
		break;
	case N_CONN_RES:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_CONN_RES");
		rtn = n_conn_res(up, q, mp);
		break;
	case N_DISCON_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_DISCON_REQ");
		rtn = n_discon_req(up, q, mp);
		break;
	case N_DATA_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_DATA_REQ");
		rtn = n_data_req(up, q, mp);
		break;
	case N_EXDATA_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_EXDATA_REQ");
		rtn = n_exdata_req(up, q, mp);
		break;
	case N_INFO_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_INFO_REQ");
		rtn = n_info_req(up, q, mp);
		break;
	case N_BIND_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_BIND_REQ");
		rtn = n_bind_req(up, q, mp);
		break;
	case N_UNBIND_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_UNBIND_REQ");
		rtn = n_unbind_req(up, q, mp);
		break;
	case N_UNITDATA_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_UNITDATA_REQ");
		rtn = n_unitdata_req(up, q, mp);
		break;
	case N_OPTMGMT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_OPTGMTM_REQ");
		rtn = n_optmgmt_req(up, q, mp);
		break;
	case N_DATACK_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_DATACK_REQ");
		rtn = n_datack_req(up, q, mp);
		break;
	case N_RESET_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_RESET_REQ");
		rtn = n_reset_req(up, q, mp);
		break;
	case N_RESET_RES:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_RESET_RES");
		rtn = n_reset_res(up, q, mp);
		break;
/* Just SCCP from here on */
	case N_INFORM_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_INFORM_REQ");
		rtn = n_inform_req(up, q, mp);
		break;
	case N_COORD_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_COORD_REQ");
		rtn = n_coord_req(up, q, mp);
		break;
	case N_COORD_RES:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_COORD_RES");
		rtn = n_coord_res(up, q, mp);
		break;
	case N_STATE_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_STATE_REQ");
		rtn = n_state_req(up, q, mp);
		break;
	default:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> N_????_???");
		rtn = n_other_req(up, q, mp);
		break;
#endif
#if TUA
		/* FIXME */
#endif
/* UA based management interface */
	case UA_USER_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_USER_REQ");
		rtn = ua_user_req(up, q, mp);
		break;
	case UA_ATTACH_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ATTACH_REQ");
		rtn = ua_attach_req(up, q, mp);
		break;
	case UA_DETACH_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_DETACH_REQ");
		rtn = ua_detach_req(up, q, mp);
		break;
	case UA_SCTP_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_SCTP_REQ");
		rtn = ua_sctp_req(up, q, mp);
		break;
	case UA_LINK_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_LINK_REQ");
		rtn = ua_link_req(up, q, mp);
		break;
	case UA_UNLINK_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_UNLINK_REQ");
		rtn = ua_unlink_req(up, q, mp);
		break;
	case UA_ASPUP_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASPUP_REQ");
		rtn = ua_aspup_req(up, q, mp);
		break;
	case UA_ASPDN_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASPDN_REQ");
		rtn = ua_aspdn_req(up, q, mp);
		break;
	case UA_HBEAT_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_HBEAT_REQ");
		rtn = ua_hbeat_req(up, q, mp);
		break;
	case UA_INFO_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_INFO_REQ");
		rtn = ua_info_req(up, q, mp);
		break;
	case UA_ASPAC_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASPAC_REQ");
		rtn = ua_aspac_req(up, q, mp);
		break;
	case UA_ASPIA_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASPIA_REQ");
		rtn = ua_aspia_req(up, q, mp);
		break;
	case UA_ASINFO_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_ASINFO_REQ");
		rtn = ua_asinfo_req(up, q, mp);
		break;
	case UA_REG_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_REG_REQ");
		rtn = ua_reg_req(up, q, mp);
		break;
	case UA_DEREG_REQ:
		strlog(up->mid, up->sid, UALOGRX, SL_TRACE, "-> UA_DEREG_REQ");
		rtn = ua_dereg_req(up, q, mp);
		break;
	}
	if (rtn < 0)
		up_set_state(up, oldstate);
	sp_unlock(sp);
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
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct up *lm;
	int rtn = 0;

	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if (!(lm = lm_acquire(q)))
		return (-EDEADLK);

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		rtn = lm_i_ioctl(lm, q, mp);
		break;
	case UA_AS_IOC_MAGIC:
		rtn = ua_as_ioctl(lm, q, mp);
		break;
#if M2UA
	case SL_IOC_MAGIC:
		rtn = sl_ioctl(lm, q, mp);
		break;
	case SDT_IOC_MAGIC:
		rtn = sdt_ioctl(lm, q, mp);
		break;
	case SDL_IOC_MAGIC:
		rtn = sdl_ioctl(lm, q, mp);
		break;
#endif
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	lm_release(lm);
	return (rtn);
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
	struct up *lm;
	int rtn = 0;
	mblk_t *dp;

	if (!(lm = lm_acquire(q)))
		return (-EDEADLK);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		break;
	case M_COPY_CASE(MI_COPY_IN, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case UA_AS_IOC_MAGIC:
			rtn = ua_as_copyin(lm, q, mp, dp);
			break;
#if M2UA
		case SL_IOC_MAGIC:
			rtn = sl_copyin(lm, q, mp, dp);
			break;
		case SDT_IOC_MAGIC:
			rtn = sdt_copyin(lm, q, mp, dp);
			break;
		case SDL_IOC_MAGIC:
			rtn = sdl_copyin(lm, q, mp, dp);
			break;
#endif
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case M_COPY_CASE(MI_COPY_IN, 2):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case UA_AS_IOC_MAGIC:
			rtn = ua_as_copyin2(lm, q, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case M_COPY_CASE(MI_COPY_OUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case UA_AS_IOC_MAGIC:
			rtn = ua_as_copyout(lm, q, mp, dp);
			break;
#if M2UA
		case SL_IOC_MAGIC:
			rtn = sl_copyout(lm, q, mp, dp);
			break;
		case SDT_IOC_MAGIC:
			rtn = sdt_copyout(lm, q, mp, dp);
			break;
		case SDL_IOC_MAGIC:
			rtn = sdl_copyout(lm, q, mp, dp);
			break;
#endif
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		break;
	}
	lm_release(lm);
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
	switch (tp_get_state(tp)) {
	case TS_WRES_CIND:
	case TS_IDLE:
	case (uint) - 1:
		tp_set_state(tp, TS_WRES_CIND);
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
	switch (tp_get_state(tp)) {
	case TS_WCON_CREQ:
	case (uint) - 1:
		break;
	default:
		goto outstate;
	}
	tp_set_state(tp, TS_DATA_XFER);
	if (tp->flags & UA_CONFIGURED)
		return t_addr_req(tp, q, mp);
	if (!(tp->flags & UA_LOCADDR))
		return t_addr_req(tp, q, mp);
	if (!(tp->flags & UA_REMADDR)) {
		if (p->RES_length) {
			bcopy((caddr_t) p + p->RES_offset, &tp->xp.options.rem_add, p->RES_length);
			tp->xp.options.rem_len = p->RES_length;
			tp->flags |= UA_REMADDR;
		} else {
			return t_addr_req(tp, q, mp);
		}
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
	switch (tp_get_state(tp)) {
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
	case TS_WCON_CREQ:
		break;
	default:
		goto outstate;
	}
	if ((err = gp_set_state(tp->gp.gp, q, ASP_DOWN)))
		return (err);
	tp_set_state(tp, TS_IDLE);
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
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
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
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
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
	uint oldstate = tp_get_state(tp);

	tp->xp.info.PRIM_type = T_INFO_ACK;
	tp->xp.info.TSDU_size = p->TSDU_size;
	tp->xp.info.ETSDU_size = p->ETSDU_size;
	tp->xp.info.CDATA_size = p->CDATA_size;
	tp->xp.info.DDATA_size = p->DDATA_size;
	tp->xp.info.ADDR_size = p->ADDR_size;
	tp->xp.info.OPT_size = p->OPT_size;
	tp->xp.info.TIDU_size = p->TIDU_size;
	tp->xp.info.SERV_type = p->SERV_type;
	tp->xp.info.CURRENT_state = p->CURRENT_state;
	tp->xp.info.PROVIDER_flag = p->PROVIDER_flag;
	if (oldstate != tp->xp.info.CURRENT_state) {
		if (oldstate == (uint) - 1) {
			switch (tp->xp.info.CURRENT_state) {
			case TS_UNBND:
				if (tp->flags & UA_CONFIGURED)
					return t_bind_req(tp, q, mp, tp->xp.options.loc_len,
							  (caddr_t) &tp->xp.options.loc_add);
				break;
			case TS_IDLE:
				if (tp->flags & UA_CONFIGURED) {
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
				if (!(tp->flags & UA_LOCADDR))
					return t_addr_req(tp, q, mp);
				break;
			case TS_DATA_XFER:
				if ((tp->flags & UA_CONFIGURED)
				    && gp_get_state(gp) == ASP_DOWN)
					return gp_send_asps_aspup_req(gp, q, mp, NULL, 0);
				if ((tp->flags & (UA_LOCADDR | UA_REMADDR)) !=
				    (UA_LOCADDR | UA_REMADDR))
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
	struct up *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	tp_set_state(tp, TS_IDLE);
	if ((lm = tp->sp.sp->lm.lm) || (lm = lm_ctrl))
		return ua_sctp_ind(lm, q, mp);
	if (tp->flags & (UA_CONFIGURED | UA_REMADDR)) {
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
	if ((tp->flags & (UA_LOCADDR)) != (UA_LOCADDR))
		return t_addr_req(tp, q, mp);
      error:
	freemsg(mp);
	return (0);
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_BIND_ACK too short");
	goto error;
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
	struct up *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case TS_WACK_BREQ:
		tp_set_state(tp, TS_UNBND);
		break;
	case TS_WACK_UREQ:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_CREQ:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_CRES:
		tp_set_state(tp, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ6:
		tp_set_state(tp, TS_WCON_CREQ);
		break;
	case TS_WACK_DREQ7:
		tp_set_state(tp, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ9:
		tp_set_state(tp, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ10:
		tp_set_state(tp, TS_WIND_ORDREL);
		break;
	case TS_WACK_DREQ11:
		tp_set_state(tp, TS_WREQ_ORDREL);
		break;
	case TS_WACK_OPTREQ:
		tp_set_state(tp, TS_IDLE);
		break;
	default:
		goto outstate;
	}
	if ((lm = tp->sp.sp->lm.lm) || (lm = lm_ctrl))
		return ua_sctp_ind(lm, q, mp);
      error:
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ERROR_ACK out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ERROR_ACK too short");
	goto error;
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
	struct up *lm;
	struct sp *sp = tp->sp.sp;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if ((lm = tp->sp.sp->lm.lm) || (lm = lm_ctrl)) {
		switch (tp_get_state(tp)) {
		case TS_WACK_UREQ:
			tp_set_state(tp, TS_UNBND);
			break;
		case TS_WACK_CREQ:
			tp_set_state(tp, TS_WCON_CREQ);
			break;
		case TS_WACK_CRES:
			tp_set_state(tp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			tp_set_state(tp, TS_IDLE);
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
		return ua_sctp_ind(lm, q, mp);
	}
	switch (tp_get_state(tp)) {
	case TS_WACK_UREQ:
		tp_set_state(tp, TS_UNBND);
	case TS_UNBND:
		if (tp->flags & (UA_CONFIGURED | UA_LOCADDR))
			return t_bind_req(tp, q, mp, tp->xp.options.loc_len,
					  (caddr_t) &tp->xp.options.loc_add);
		break;
	case TS_WACK_CREQ:
		tp_set_state(tp, TS_WCON_CREQ);
	case TS_WCON_CREQ:
		break;
	case TS_WACK_CRES:
		tp_set_state(tp, TS_DATA_XFER);
	case TS_DATA_XFER:
		if (tp->flags & UA_CONFIGURED) {
			if (gp_get_state(gp) == ASP_DOWN)
				return gp_send_asps_aspup_req(gp, q, mp, NULL, 0);
		}
		if ((tp->flags & (UA_REMADDR | UA_LOCADDR)) != (UA_REMADDR | UA_LOCADDR)) {
			return t_addr_req(tp, q, mp);
		}
		if (gp_get_state(gp) == ASP_DOWN)
			return gp_send_asps_aspup_req(gp, q, mp, NULL, 0);
		break;
	case TS_WACK_BREQ:
	case TS_WACK_OPTREQ:
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		tp_set_state(tp, TS_IDLE);
	case TS_IDLE:
		if (tp->flags & UA_CONFIGURED) {
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
		if ((tp->flags & (UA_REMADDR | UA_LOCADDR)) != (UA_REMADDR | UA_LOCADDR)) {
			return t_addr_req(tp, q, mp);
		}
		if (tp->flags & UA_REMADDR) {
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
      error:
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OK_ACK out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OK_ACK too short");
	goto error;
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
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
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
	struct up *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if ((lm = tp->sp.sp->lm.lm) || (lm = lm_ctrl))
		return ua_sctp_ind(lm, q, mp);
	/* ignore for now */
      error:
	freemsg(mp);
	return (0);
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_UDERROR_IND too short");
	goto error;
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
	struct up *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (tp_get_state(tp) == TS_WACK_OPTREQ)
		tp_set_state(tp, TS_IDLE);
	if ((lm = tp->sp.sp->lm.lm) || (lm = lm_ctrl))
		return ua_sctp_ind(lm, q, mp);
      error:
	freemsg(mp);
	return (0);
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_OPTMGMT_ACK too short");
	goto error;
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
	struct up *lm;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	switch (tp_get_state(tp)) {
	case TS_DATA_XFER:
		tp_set_state(tp, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		tp_set_state(tp, TS_IDLE);
		break;
	}
	if ((err = gp_set_state(tp->gp.gp, q, ASP_DOWN)))
		return (err);

	if ((lm = tp->sp.sp->lm.lm) || (lm = lm_ctrl)) {
		/* If we have a layer manager, pass the message to the layer manager and let it
		   respond. */
		return ua_sctp_ind(lm, q, mp);
	}
	switch (tp_get_state(tp)) {
	case TS_WREQ_ORDREL:
		tp_set_state(tp, TS_IDLE);
		return t_ordrel_req(tp, q, mp, NULL);
	case TS_IDLE:
		break;
	default:
		goto outstate;
	}
      error:
	freemsg(mp);
	return (0);
      outstate:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ORDREL_IND out of state");
	/* Should really generate T_CAPABILITY_REQ to synchronize state. */
	goto error;
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ORDREL_IND too short");
	goto error;
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
#if M2UA
	if (tp->sp.sp->sp.options.options.pvar == UA_VERSION_TS102141) {
		/* Note that TS 102 141 says to check the PPI and if the PPI is other than 0 or 5
		   to discard the message.  We don't do that yet.  It will impact performance. */
	}
#endif
#if M3UA
	if (tp->sp.sp->sp.options.options.pvar == UA_VERSION_TS102142) {
		/* Note that TS 102 142 says to check the PPI and if the PPI is other than 0 or 3
		   to discard the message.  We don't do that yet.  It will impact performance. */
	}
#endif
#if SUA
	if (tp->sg.sg->sp.options.options.pvar == UA_VERSION_TS102143) {
		/* Note that TS 102 143 says to check the PPI and if the PPI is other than 0 or 4
		   to discard the message.  We don't do that yet.  It will impact performance. */
	}
#endif
	if ((err = tp_recv_msg(tp, q, dp)) == 0)
		freeb(mp);
	return (err);
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
	struct up *lm;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (p->LOCADDR_length) {
		tp->flags |= UA_LOCADDR;
		bcopy((caddr_t) p + p->LOCADDR_offset, &tp->xp.options.loc_add, p->LOCADDR_length);
		tp->xp.options.loc_len = p->LOCADDR_length;
	}
	if (p->REMADDR_length) {
		tp->flags |= UA_REMADDR;
		bcopy((caddr_t) p + p->REMADDR_offset, &tp->xp.options.rem_add, p->REMADDR_length);
		tp->xp.options.rem_len = p->REMADDR_length;
	}
	if ((lm = tp->sp.sp->lm.lm) || (lm = lm_ctrl)) {
		/* If there is a layer manager, pass the message to the layer manager. */
		return ua_sctp_ind(lm, q, mp);
	}
	/* The T_ADDR_REQ is often initiated as part of another sequence that needs to complete.
	   Check the state and complete the sequence if possible. */
	switch (tp_get_state(tp)) {
	case TS_UNBND:
		/* bind if possible */
		if (tp->flags & (UA_CONFIGURED | UA_LOCADDR)) {
			tp_set_state(tp, TS_WACK_BREQ);
			return t_bind_req(tp, q, mp, (size_t) tp->xp.options.loc_len,
					  (caddr_t) &tp->xp.options.loc_add);
		}
		break;
	case TS_WACK_BREQ:
	case TS_WACK_UREQ:
		break;
	case TS_IDLE:
		/* connect if possible */
		if (tp->flags & (UA_CONFIGURED | UA_REMADDR)) {
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
		switch (gp_get_state(gp)) {
		case ASP_DOWN:
			return gp_send_asps_aspup_req(tp->gp.gp, q, mp, NULL, 0);
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
      error:
	freemsg(mp);
	return (0);
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_ADDR_ACK too short");
	goto error;
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
      error:
	freemsg(mp);
	return (0);
      tooshort:
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "T_CAPABILITY_ACK too short");
	goto error;
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

static fastcall int
gp_wack_aspup_timeout(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	uint tack = gp->sg.sg->sp.sp->sp.options.tack;
	int err = 0;

	if (gp_get_state(gp) != ASP_WACK_ASPUP
	    (err = gp_send_asps_aspup_req(gp, q, NULL, NULL, 0)) || !tack)
		return (err);
	strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE, "-> WACK ASPUP START <- (%u msec)", tack);
	mi_timer(q, mp, tack);
	return (0);
}
static fastcall int
gp_wack_aspdn_timeout(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	uint tack = gp->sg.sg->sp.sp->sp.options.tack;
	int err = 0;

	if (gp_get_state(gp) != ASP_WACK_ASPDN ||
	    (err = gp_send_asps_aspdn_req(gp, q, NULL, NULL, 0)) || !tack)
		return (err);
	strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE, "-> WACK ASPDN START <- (%u msec)", tack);
	mi_timer(q, mp, tack);
	return (0);
}
static fastcall int
gp_wack_hbeat_timeout(struct gp *gp, struct tp *tp, queue_t *q, mblk_t *mp)
{
	uint tack = gp->sg.sg->sp.sp->sp.options.tack;
	int err = 0;

	if (gp_get_state(gp) != ASP_WACK_HBEAT
	    (err = gp_send_asps_hbeat_req(gp, q, NULL, NULL, 0)) || !tack)
		return (err);
	strlog(tp->mid, tp->sid, UALOGTE, SL_TRACE, "-> WACK HBEAT START <- (%u msec)", tack);
	mi_timer(q, mp, tack);
	return (0);
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
	struct gp *gp;
	uint oldstate;
	int rtn;

	if (!(tp = tp_acquire(q)))
		return (-EDEADLK);

	if (!(gp = tp->gp.gp)) {
		tp_release(tp);
		return (-EAGAIN);
	}

	if (mp->b_wptr < mp->b_rptr + sizeof(uint32_t)) {
		strlog(tp->mid, tp->sid, 0, SL_ERROR, "primitive too short <-");
		tp_release(tp);
		freemsg(mp);
		return (0);
	}

	oldstate = tp_get_state(tp);

	if (!pcmsg(DB_TYPE(mp)) && oldstate == -1) {
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
	if (rtn < 0)
		tp_set_state(tp, oldstate);
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
	struct ua_timer *t = (typeof(t)) mp->b_rptr;
	int rtn = 0;

	if (!(tp = tp_acquire(q)))
		return (-EDEADLK);

	if (likely(mi_timer_valid(mp))) {
		switch (t->timer) {
		case gp_wack_aspup:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK ASPUP TIMEOUT <-");
			rtn = gp_wack_aspup_timeout(t->obj, tp, q, mp);
			break;
		case gp_wack_aspdn:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK ASPDN TIMEOUT <-");
			rtn = gp_wack_aspdn_timeout(t->obj, tp, q, mp);
			break;
		case gp_wack_hbeat:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK HBEAT TIMEOUT <-");
			rtn = gp_wack_hbeat_timeout(t->obj, tp, q, mp);
			break;
		case rp_wack_aspac:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK ASPAC TIMEOUT <-");
			rtn = rp_wack_aspac_timeout(t->obj, tp, q, mp);
			break;
		case rp_wack_aspia:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK ASPIA TIMEOUT <-");
			rtn = rp_wack_aspia_timeout(t->obj, tp, q, mp);
			break;
		case rp_wack_hbeat:
			strlog(tp->mid, tp->sid, UALOGTO, SL_TRACE, "-> WACK HBEAT TIMEOUT <-");
			rtn = rp_wack_hbeat_timeout(t->obj, tp, q, mp);
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

	if (!(tp = tp_acquire(q)))
		return (-EAGAIN);
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "M_CTL/M_PCCTL on lower read queue");
	tp_release(tp);
	freemsg(mp);
	return (0);
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

	if (!(tp = tp_acquire(q)))
		return (-EAGAIN);
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "M_ERROR/M_HANGUP/M_UNHANGUP on lower read queue");
	tp_release(tp);
	freemsg(mp);
	return (0);
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

	if (!(tp = tp_acquire(q)))
		return (-EAGAIN);
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "M_IOCACK/M_IOCNAK on lower read queue");
	tp_release(tp);
	freemsg(mp);
	return (0);
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

	if (!(tp = tp_acquire(q)))
		return (-EAGAIN);
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "M_COPYIN/M_COPYOUT on lower read queue");
	tp_release(tp);
	((struct copyresp *) mp->b_rptr)->cp_rval = (caddr_t) 1;	/* indicate failure */
	qreply(q, mp);
	return (0);
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

	if (!(tp = tp_acquire(q)))
		return (-EAGAIN);
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "unknown STREAMS message %d", (int) DB_TYPE(mp));
	tp_release(tp);
	freemsg(mp);
	return (0);
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
	struct sp *sp;
	struct tp *tp;

	read_lock(&ua_mux_lock);
	if ((sp = up->sp.sp))
		for (tp = sp->xp.list; tp; tp = tp->sp.next)
			if (tp->rq)
				qenable(tp->rq);
	read_unlock(&ua_mux_lock);
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

	if (!(tp = tp_acquire(q)))
		return (-EAGAIN);
	strlog(tp->mid, tp->sid, 0, SL_ERROR, "%s called in error", __FUNCTION__);
	tp_release(tp);
	putnext(q, mp);
	return (0);
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
		if (tp->sp.sp)
			for (up = tp->sp.sp->up.list; up; up = up->sp.next)
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
 *	autocloning features of Linux Fast-STREAMS.  This corresponds to the /dev/streams/m3ua-as/lm
 *	minor device node.
 *
 * (cminor == 0) && (sflag == CLONEOPEN)
 *	This is a normal clone open using the clone(4) driver.  A disassociated user Stream is
 *	opened.  A new unique minor device number above NUM_SG is assigned.  This corresponds to the
 *	/dev/streams/clone/m3ua-as clone device node.
 *
 * (1 <= cminor && cminor <= NUM_SG)
 *	This is a normal non-clone open.  Where the minor device number is between 1 and NUM_SG, an
 *	associated user Stream is opened.  If there is no SG structure to associate, one is created
 *	with default values.  A new minor device number above NUM_SG is assigned.  This uses the
 *	autocloning features of Linux Fast-STREAMS.  This corresponds to the
 *	/dev/streams/m3ua-as/NNNN minor device node where NNNN is the minor device number.
 */
static streamscall int
up_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	unsigned long flags;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct up *up;
	struct sp *sp;
	int err = 0;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || q->q_next)
		return (ENXIO);
	if (cminor > NUM_SG)
		return (ENXIO);
	if (!(up = up_alloc_priv(q, devp, crp, cminor)))
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
				free_object_up(up);
				return (ENXIO);
			}
		}
		if ((err = mi_open_link(&ua_opens, (caddr_t) up, devp, oflags, CLONEOPEN, crp))) {
			write_unlock_irqrestore(&ua_mux_lock, flags);
			free_object_up(up);
			return (err);
		}
		if (sflag == DRVOPEN)
			lm_ctrl = up;
		/* Both master control Streams and clone user Streams are disassociated with any
		   specific SG.  Master control Streams are never associated with a specific SG.
		   User Streams are associated with an SG using the spid in the UA Address to the
		   attach/bind primitive, or when an SCTP Stream is temporarily linked under the
		   driver using the I_LINK input-output control. */
	} else {
		DECLARE_WAITQUEUE(wait, current);

		/* When a non-zero minor device number was opened, the Stream is automatically
		   associated with the SG to which the minor device number corresponds.  It cannot
		   be disassociated except when it is closed. */
		if (!(sp = find_object_sp(&err, up, cminor, NULL))) {
			write_unlock_irqrestore(&ua_mux_lock, flags);
			free_object_up(up);
			return (ENXIO);
		}
		/* Locking: need to wait until a lock on the SG structure can be acquired, or a
		   signal is received, or the SG structure is deallocated.  If the lock can be
		   acquired, associate the User Stream with the SG structure; in all other cases,
		   return an error.  Note that it is a likely event that the lock can be acquired
		   without waiting. */
		err = 0;
		add_wait_queue(&ua_waitq, &wait);
		spin_lock(&sp->lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (signal_pending(current)) {
				err = EINTR;
				spin_unlock(&sp->lock);
				break;
			}
			if (sp->users != 0) {
				spin_unlock(&sp->lock);
				write_unlock_irqrestore(&ua_mux_lock, flags);
				schedule();
				write_lock_irqsave(&ua_mux_lock, flags);
				if (!(sp = find_object_sp(&err, up, cminor, NULL))) {
					err = ENXIO;
					break;
				}
				spin_lock(&sp->lock);
				continue;
			}
			err = mi_open_link(&ua_opens, (caddr_t) up, devp, oflags, CLONEOPEN, crp);
			if (err == 0)
				up_sp_link(up, sp);
			spin_unlock(&sp->lock);
			break;
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&ua_waitq, &wait);
		if (err) {
			write_unlock_irqrestore(&ua_mux_lock, flags);
			free_object_up(up);
			return (err);
		}
	}

	/* just a few fixups after device number assigned */
	up->dev = *devp;
	up->mid = getmajor(*devp);
	up->sid = getminor(*devp);
	up->as.as->as.id = (uint) (*devp);

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
	struct sp *sp;

	qprocsoff(q);
	mi_detach(q, (caddr_t) up);

	write_lock_irqsave(&ua_mux_lock, flags);

	if ((sp = up->sp.sp)) {
		DECLARE_WAITQUEUE(wait, current);

		/* Locking: need to wait until a SG lock can be acquired, or the SG structure is
		   deallocated.  If a lock can be acquired, the closing Stream is disassociated
		   with the SG; otherwise, if the SG structure is deallocated, there is no further
		   need to disassociate.  Note that it is a likely event that the lock can be
		   acquired without waiting. */
		add_wait_queue(&ua_waitq, &wait);
		spin_lock(&sp->lock);
		for (;;) {
			set_current_state(TASK_UNINTERRUPTIBLE);
			if (sp->users == 0) {
				up_sp_unlink(up);
				spin_unlock(&sp->lock);
				break;
			}
			spin_unlock(&sp->lock);
			write_unlock_irqrestore(&ua_mux_lock, flags);
			schedule();
			write_lock_irqsave(&ua_mux_lock, flags);
			if (!(sp = up->sp.sp))
				break;
			spin_lock(&sp->lock);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&ua_waitq, &wait);
	}
	mi_close_unlink(&ua_opens, (caddr_t) up);

	write_unlock_irqrestore(&ua_mux_lock, flags);

	free_object_up(up);	/* includes mi_close_free */
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

static struct streamtab m3ua_asinfo = {
	.st_rdinit = &up_rinit,
	.st_wrinit = &up_winit,
	.st_muxrinit = &tp_rinit,
	.st_muxwinit = &tp_winit,
};

static struct cdevsw ua_cdev = {
	.d_name = DRV_NAME,
	.d_str = &m3ua_asinfo,
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
MODULE_PARM_DESC(modid, "Module ID for the M3UA-AS driver. (0 for allocation.)");

unsigned short major = DRV_CMAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, ushort, DRV_CMAJOR);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for M3UA-AS driver.  (0 for allocation.)");

static __init int
m3ua_asinit(void)
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
m3ua_asexit(void)
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
module_init(m3ua_asinit);
module_exit(m3ua_asexit);
