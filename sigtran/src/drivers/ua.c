/*****************************************************************************

 @(#) $RCSfile: ua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/11/16 20:45:41 $

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

 Last Modified $Date: 2006/11/16 20:45:41 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ua.c,v $
 Revision 0.9.2.2  2006/11/16 20:45:41  brian
 - working up UA driver release

 Revision 0.9.2.1  2006/11/04 11:31:15  brian
 - new generic ua files

 *****************************************************************************/

#ident "@(#) $RCSfile: ua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/11/16 20:45:41 $"

static char const ident[] =
    "$RCSfile: ua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/11/16 20:45:41 $";

#define UA_DESCRIP	"SIGTRAN USER ADAPTATION (UA) STREAMS MULTIPLEXING DRIVER."
#define UA_REVISION	"OpenSS7 $RCSfile: ua.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/11/16 20:45:41 $"
#define UA_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define UA_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define UA_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define UA_LICENSE	"GPL"
#define UA_BANNER	UA_DESCRIP	"\n" \
			UA_REVISION	"\n" \
			UA_COPYRIGHT	"\n" \
			UA_DEVICE	"\n" \
			UA_CONTACT

#ifdef LINUX
MODULE_AUTHOR(UA_CONTACT);
MODULE_DESCRIPTION(UA_DESCRIP);
MODULE_SUPPORTED_DEVICE(UA_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(UA_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-ua");
#endif
#endif				/* LINUX */

#ifdef LFS
#define UA_DRV_ID		CONFIG_STREAMS_UA_MODID
#define UA_DRV_NAME		CONFIG_STREAMS_UA_NAME
#define UA_CMAJORS		CONFIG_STREAMS_UA_NMAJORS
#define UA_CMAJOR_0		CONFIG_STREAMS_UA_MAJOR
#define UA_UNITS		CONFIG_STREAMS_UA_NMINORS
#endif

/* Lock debugging. */

#ifdef _DEBUG
#define spin_lock_ua(lock) (void)lock
#define spin_unlock_ua(lock) (void)lock
#else
#define spin_lock_ua(lock) spin_lock_bh(lock)
#define spin_unlock_ua(lock) spin_unlock_bh(lock)
#endif

/*
 * STREAMS Definitions
 */

#define DRV_ID		UA_DRV_ID
#define DRV_NAME	UA_DRV_NAME
#define CMAJORS		UA_CMAJORS
#define CMAJOR_0	UA_CMAJOR_0
#define UNITS		UA_UNITS
#ifdef MODULE
#define DRV_BANNER	UA_BANNER
#else				/* MODULE */
#define DRV_BANNER	UA_SPLASH
#endif				/* MODULE */

STATIC struct module_info ua_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-wr",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size expected */
	mi_maxpsz:272 + 1,		/* Max packet size expected */
	mi_hiwat:STRHIGH,		/* Hi water mark */
	mi_lowat:STRLOW,		/* Lo water mark */
};
STATIC struct module_info ua_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-rd",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size expected */
	mi_maxpsz:272 + 1,		/* Max packet size expected */
	mi_hiwat:STRHIGH,		/* Hi water mark */
	mi_lowat:STRLOW,		/* Lo water mark */
};
STATIC struct module_info mux_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-mxw",
	/* Module ID name */
	mi_minpsz:1,			/* Min packet size expected */
	mi_maxpsz:272 + 1,		/* Max packet size expected */
	mi_hiwat:STRHIGH,		/* Hi water mark */
	mi_lowat:STRLOW,		/* Lo water mark */
};
STATIC struct module_info mux_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-mxr",
	/* Module ID name */
	mi_minpsz:1,			/* Min packet size expected */
	mi_maxpsz:272 + 1,		/* Max packet size expected */
	mi_hiwat:STRHIGH,		/* Hi water mark */
	mi_lowat:STRLOW,		/* Lo water mark */
};

STATIC streamscall int ua_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int ua_close(queue_t *, int, cred_t *);

STATIC streamscall int ua_oput(queue_t *, mblk_t *);
STATIC streamscall int ua_osrv(queue_t *);

STATIC struct qinit ua_rinit = {
	qi_putp:ua_oput,		/* Read put (message from below) */
	qi_srvp:ua_osrv,		/* Read queue service */
	qi_qopen:ua_open,		/* Each open */
	qi_qclose:ua_close,		/* Last close */
	qi_minfo:&ua_rinfo,		/* Information */
};

STATIC struct qinit ua_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&ua_winfo,		/* Information */
};

STATIC struct qinit mux_rinit = {
	qi_putp:ss7_iput,		/* Read put (message from below) */
	qi_srvp:ss7_isrv,		/* Read queue service */
	qi_minfo:&mux_rinfo,		/* Information */
};

STATIC struct qinit mux_winit = {
	qi_putp:ua_oput,		/* Write put (message from above) */
	qi_srvp:ua_osrv,		/* Write queue service */
	qi_minfo:&mux_winfo,		/* Information */
};

MODULE_STATIC struct streamtab uainfo = {
	st_rdinit:&ua_rinit,		/* Upper read queue */
	st_wrinit:&ua_winit,		/* Upper write queue */
	st_muxrinit:&mux_rinit,		/* Lower read queue */
	st_muxwinit:&mux_winit,		/* Lower write queue */
};

/*
 * Private Structures
 */

#define AS_DOWN		0	/* the AS is down */
#define AS_INACTIVE	1	/* the AS is inactive */
#define AS_WACK_ASPAC	2	/* the AS is waiting to activate */
#define AS_WACK_ASPIA	3	/* the AS is waiting to deactivate */
#define AS_ACTIVE	4	/* the AS is active */

/*
 * generic linkage 
 */
#define SLIST_COUNT(__s) \
	struct { \
		struct __s *list; \
		uint numb;				/* number of elements in list */ \
		union { \
			struct { \
				uint down;		/* number of elements down in list */ \
				uint inactive;		/* number of elements inactive in list */ \
				uint wack_aspac;	/* number of elements activating in list */ \
				uint wack_aspia;	/* number of elements deactivating in list */ \
				uint active;		/* number of elements active in list */ \
			} c; \
			uint counts[AS_ACTIVE + 1]; \
		}; \
	} __s \

/*
 * forward declarations
 */
struct df;				/* DF structure */
struct ss;				/* SS structure */
struct as;				/* AS structure */
struct ap;				/* AP (AS-U to AS-P mapping) structure */
struct gp;				/* GP (AS to SPP mapping) structure */
struct spp;				/* SPP proxy */
struct sp;				/* SP structure */
struct np;				/* NP (SP-U to SP-P (SG) mapping) structure */
struct pp;				/* PP (SPP to XP mapping) structure */
struct rp;				/* RP (GP (AS/SPP) to PP (SPP/XP) mapping) structure */
struct xp;				/* XP structure */

struct lg;				/* LG structure */
struct lp;				/* LP (LG to SPP mapping) structure */

/*
 *  One more kick at the can for these complex data structures.  An sp structure represent either an
 *  ASP pool or an SG (SGP pool).  Each ASP pool or SG has a set of AS that it serves or provides.
 *  SG pools are rather fixed.  SGP are assigned to one and only one SG.  ASP pools are a loose
 *  concept.  An ASP pool is a set of ASPs serving AS.  An ASP pool is associated with a set of SG
 *  that provide it with service.
 */

/*
 *  LG Structure
 *  -----------------------------------
 *  The LG structure represents a load group within an AS and provides a mapping
 *  of load selections to serving SSPs via the LP mapping.  For an override AS,
 *  the selected load group, if any, is the first load group on the list.  For a
 *  broadcast AS, each load group is iterated across.  For a loadshare AS, the
 *  load group is selected via the load selection map.
 */

struct lg {
	uint32_t state;			/* LG state */
	uint32_t flags;			/* LG flags */
	uint32_t ldist;			/* load distribution method */
	SLIST_LINKAGE (as, lg, as);	/* AS linkage */
	SLIST_COUNT (lp);		/* GP (SPP graph) */
	bufq_t bufq;			/* Buffer queue */
	ua_ldkey_t key;			/* Load Key */
	ua_timers_lg_t timers;		/* Load group timers */
	ua_opt_conf_lg_t config;	/* Load group configuration */
	ua_stats_lg_t stats;		/* Load group statistics */
	ua_stats_lg_t statsp;		/* Load group statistics periods */
};

/*
 *  LP (LG to SPP mapping) structure
 *  -----------------------------------
 *  The LP structure maps LG to remote ASP/SGP/SPPs that are members of the LG.
 *  It also indicates the LS (load selection) that is used by the remote
 *  ASP/SGP/SPP for this LG and the state of the remote ASP/SGP/SPP for this LG.
 */
struct lp {
	uint32_t state;			/* LG state for this SPP */
	uint32_t flags;			/* LG flags fro this SPP */
	uint32_t ls;			/* LG LS for this SPP */
	SLIST_LINKAGE (lg, lp, lg);	/* LG linkage */
	SLIST_LINKAGE (spp, lp, spp);	/* SPP linkage */
	bufq_t bufq;			/* Buffer queue */
	ua_ldkey_t key;			/* Load Key */
	ua_timers_lp_t timers;		/* Load selection timers */
};

/*
 *  AS Structure
 *  -----------------------------------
 *  The AS-U structure represents each AS which is currently being served to
 *  remote UA ASPs via the LG list, AP map or to local SS users via the SS-U.
 *  An AS-U belongs to a local SP (local SGP/remote ASP relation) and has
 *  service provided by a map of AS-P.
 *
 *  The AS-P structure represents each AS which is currently being provided by
 *  remote UA SGPs via the LG list, GP map or by local SS providers via the
 *  SS-P.  An AS-P belongs to a local SG (local ASP/remote SG relation) and
 *  provides service to a map of AS-U.
 *
 *  For an AS-U or AS-P that supports load selection (LOADSEL) or load grouping
 *  (LOADGRP), the LG list provides access to the list of load selections for
 *  the AS.  Even when the AS does not support LOADSEL/LOADGRP, but is a load
 *  sharing AS, the LG list provides the load sharing distribution.  An empty LG
 *  list indicates that load sharing and load selection is not in effect.  For
 *  simple override and broadcast AS, the GP map provides access to the SPP
 *  serving the AS.  The GP map is always provided.
 *
 *  Selection procedures:
 *
 *  When sending messages that are not subject to load distribution and are
 *  broadcast to all ASPs (such as SNMM regardless of traffic mode) each SPP in
 *  the GP map that is in the appropriate state is selected and receives a copy
 *  of the message.  If the SPP is pending the message is neither sent nor
 *  buffered.
 *
 *  When sending distributed messages, and the LG list is not empty, a load key
 *  derived from the message is compared against the load key present in each LG
 *  structure until a match is found.  If the LG is buffering, the message is
 *  simply buffered, otherwise an SPP in the LP map for the LG is selected
 *  according to the load distribution algorithm and the state of the SPP in the
 *  LG.  This may, again, be according to load key.  Again the SPP may be
 *  buffering.
 *
 *  When sending distributed messages, and the LG list is empty, the AS is used
 *  in the LG's stead.  If the AS is buffering, the message is simply buffered,
 *  otherwise an SPP in the GP map for the AS is selected according to the
 *  traffic mode and the state of the SPP in the AS.  This may be according to
 *  load key (load sharing AS).  The SPP may be buffering in which case the
 *  message is simply buffered, otherwise it is sent to the SPP.
 *
 *  Note that the LG list is empty unless the AS is an AS-U and the ASP serving
 *  the AS-U support LOADSEL or LOADGRP.  When the AS is an AS-P, the LG list is
 *  always empty.  If the AS-P is a load sharing AS-P, load sharing is performed
 *  using the GP map only.
 *
 *  At an SGP, the AS-U represents a remote SS7 User and the AS-P a local SS7 Provider.
 *  At an ASP, the AS-U represents a local SS7 User and the AS-P a remote SS7 Provider.
 *  At an SPP, the AS-U represents a local SS7 User and the AS-P a remote SS7 User.
 */
struct as {
	HEAD_DECLARATION (struct as);	/* head delcaration */
	uint level;			/* UA level */
	uint32_t tmode;			/* traffice mode (for AS-U) */
	uint ktype;			/* load key type (for AS-U) */
	union {
		uint32_t iid;		/* Interface Identifier */
		uint32_t rc;		/* Routing Context */
	};
	size_t minasp;			/* minimum number of ASPs */
	SLIST_HEAD (ss, ss);		/* SS for this AS */
	SLIST_COUNT (ap);		/* AP (AS graph) list */
	SLIST_LINKAGE (sp, as, sp);	/* SP linkage */
	SLIST_COUNT (lg);		/* LG (Load group) list */
	SLIST_COUNT (gp);		/* GP (SPP graph) list */
	bufq_t bufq;			/* Buffer queue */
	ua_addr_t add;			/* SS address */
	ua_timers_as_t timers;		/* Application server timers */
	ua_opt_conf_as_t config;	/* Application server configuration */
	ua_stats_as_t stats;		/* Application server statistics */
	ua_stats_as_t statsp;		/* Application server statistics periods */
};

#define AS_LEVEL_IUA		1
#define AS_LEVEL_M2UA		2
#define AS_LEVEL_M3UA		3
#define AS_LEVEL_SUA		4
#define AS_LEVEL_TUA		5

#define ASF_INSUFFICIENT_ASPS	(1<< 0)	/* AS has insufficient asps */
#define ASF_MINIMUM_ASPS	(1<< 1)	/* AS has minmum number of asps */
#define ASF_PENDING		(1<< 2)	/* AS pending */
#define ASF_ACTIVE		(1<< 3)	/* AS active */
#define ASF_MGMT_BLOCKED	(1<< 4)	/* AS management blocked */
#define ASF_OPERATION_PENDING	(1<< 5)	/* operation pending */
#define ASF_REGISTRATION	(1<< 6)	/* registration required */

#define ASF_BSNT_REQUEST	(1<<16)	/* bsnt requested */
#define ASF_CLEAR_RTB		(1<<17)	/* clear rtb performed */
#define ASF_EMERGENCY		(1<<18)	/* emergency set */
#define ASF_FLUSH_BUFFERS	(1<<19)	/* flush buffers performed */
#define ASF_LOC_PROC_OUTAGE	(1<<20)	/* local processor outage */
#define ASF_REM_PROC_OUTAGE	(1<<21)	/* remote processor outage */
#define ASF_RECOVERY		(1<<22)	/* waiting for recovery from remote processor outage */
#define ASF_RETRIEVAL		(1<<23)	/* in the process or retrieval */
#define ASF_USR_PROC_OUTAGE	(1<<24)	/* user process outage */
#define ASF_PRV_PROC_OUTAGE	(1<<25)	/* provider process outage */
#define ASF_CONG_ACCEPT		(1<<26)	/* congestion accept */
#define ASF_CONG_DISCARD	(1<<27)	/* congestion discard */
#define ASF_WANTW		(1<<28)	/* wanted to write across multiplex */

/*
 *  AP (AS-U to AS-P mapping) structure
 *  -----------------------------------
 *  The AP structure maps AS Users (AS-U) to AS Providers (AS-P) and indicates
 *  the state of the AS-U with each AS-P.
 */
struct ap {
	uint32_t state;			/* state of this AS-U for this AS-P */
	SLIST_LINKAGE (as, ap, u);	/* AS-U linkage */
	SLIST_LINKAGE (as, ap, p);	/* AS-P linkage */
};

/*
 *  SP structure
 *  -----------------------------------
 *  The SP structure represents a local SGP (SP-U) or a remote SG (SP-P).
 *
 *  At an SGP, the SP-U represents a remote ASP and the SP-P represents a local SG.
 *  At an ASP, the SP-U represents a local ASP and the SP-P represents a remote SG.
 *  At an IPSP, the SP-U represents a local IPSP and the SP-P represents a remote IPSP pool.
 */
struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	uint32_t cost;			/* cost */
	uint32_t tmode;			/* traffic mode (for SG) */
	uint32_t ktype;			/* load key type (for SG) */
	SLIST_COUNT (np);		/* NP (SP graph) */
	SLIST_HEAD (as, as);		/* AS list */
	SLIST_COUNT (spp);		/* ASP list */
	ua_timers_sp_t timers;		/* Signalling point timers */
	ua_opt_conf_sp_t config;	/* Signalling point configuration */
	ua_stats_sp_t stats;		/* Signalling point statistics */
	ua_stats_sp_t statsp;		/* Signalling point statistics periods */
};

/*
 *  NP (SP to SG mapping) structure
 *  -----------------------------------
 *  The NP struct maps SP Users (SP) to SP Providers (SG) and indicates the
 *  state of each SP with each SG.
 */
struct np {
	uint32_t state;			/* state of this SP relation */
	SLIST_LINKAGE (sp, np, u);	/* SP-U linkage */
	SLIST_LINKAGE (sp, np, p);	/* SP-P linkage */
};

/*
 *  SPP structure
 *  -----------------------------------
 *  The SPP structure represents an instance of a connection to an SG or ASP.  The primary reason
 *  for having multiple connections to an SG or ASP is because there may be one primary connection
 *  to an SG or ASP and many secondary connections (via other ASPs or via other SGP).  Primarily
 *  this permits an SGP to be able to transfer data destined to an ASP to which it is not directly
 *  connected via other SGP, and permits transfer of data received from an ASP to the SGP providing
 *  the SS-P for that traffic.  When switching traffic between connections because the primary
 *  connection has failed, divertable traffic must be queued for a time in accordance with the CORID
 *  draft and Heartbeat procedures effected on the old path before traffic is restarted.
 *
 *  Selection process: when traffic destined for an AS-U or AS-P has selected an ASP/SGP/SSP to
 *  receive the traffic, the SPP structures are walked and the first active, pending or queueing SPP
 *  is selected.  SPP structtures are sorted according to cost, with the lowest cost paths first in
 *  the list.  We do not effect traffic modes over SPP, but simply select the first active, pending
 *  or queueing SPP.
 *
 *  At an SGP, the SPP associated with the SP-U represents an association with a remote ASP.
 *  At an SGP, the SPP associated with the SP-P (SG) represents a peer SGP.
 *
 *  At an ASP, the SPP associated with the SP-U represents a peer ASP.
 *  At an ASP, the SPP associated with the SP-P (SG) represents an association with a remote SGP.
 *
 *  At an IPSP, the SPP associated with the SP-U represents a peer IPSP.
 *  At an IPSP, the SPP associated with the SP-P represents an association with a remote IPSP.
 *
 *  The GP list represents the state of this SPP within each AS that it serves.
 *  The SP list represents the peer SPP associated with the same SP.
 *
 *  Selection:
 *
 *  At an SGP, messages are received from the local SS7 Provider or from a peer SGP.  Messages are
 *  transfered by selecting an SP-U (ASP) associated with this SP-P (SG) active for the appropriate
 *  AS-U.  The SP-U (ASP) for that AS-U is selected according to the traffic mode of the AS-U.  For
 *  selected SP-U (ASPs), an SPP is selected that is the active ASP association for the ASP.
 *
 *  At an SGP, messages are received from a remote ASP.  Messages are transferred using the same
 *  procedures as the ASP, below, but there is only one SG to choose from and a local SS7 Provider
 *  is given precedence over peer SGP.
 *
 *  At an ASP, messages are received from the local SS7 User or from a peer ASP.  Messages are
 *  transfered by selecting an SP-P (SG) associated with this SP-U (ASP) active for the appropriate
 *  AS-P.  Considerations that go into SP-P (SG) selection are load sharing and network management
 *  (i.e. SNMM).  For the selected SG an SPP (SGP) that is active for the AS-P is selected.
 *  Selection of an active SPP (SGP) normally involves load sharing over the available SGP by can be
 *  controlled with the traffic mode (unmanaged) of the SP-P (SG).
 *
 *  At an ASP, messages are received from a remote SGP.  Messages are transferred using the same
 *  procedures as the SGP, above, but there is only one SP to choose from and a local SS7 User is
 *  given precedence over a peer ASP.  Also, message distribution can be more complex in that
 *  LOADSEL or LOADGRP or other schemes can be used depending on the exact nature of the message
 *  transferred.
 *
 *  At an IPSP, the SP-U (local IPSP node) takes on the role of the ASP and the SP-P (remote IPSP
 *  pool) the role of an SG.  Transfers are more SG-SG in nature than ASP-ASP.  However, when
 *  selecting a remote IPSP node, the traffic mode of the AS-P is used instead of an unmanaged SG
 *  method.
 *
 *  Proxy arrangements are possible where an proxy node proxies between ASPs above and SGP below,
 *  translating RC (IID) and traffic modes in the process.
 *
 *  In general, when failing over or restoring traffic between transports (NP structures), it is a
 *  good idea to buffer messages for a period or execute heartbeat procedures before restarting
 *  traffic on the new transport.
 */
struct spp {
	HEAD_DECLARATION (struct spp);	/* head declaration */
	uint32_t aspid;			/* ASP Id for this SPP */
	uint32_t cost;			/* cost */
	struct spp *xsp;		/* cross-link structure for this SPP */
	SLIST_LINKAGE (xp, spp, xp);	/* XP linkage */
	SLIST_LINKAGE (sp, spp, sp);	/* SP linkage */
	SLIST_COUNT (gp);		/* GP (AS graph) */
	SLIST_COUNT (pp);		/* PP (XP graph) */
	ua_timers_spp_t timers;		/* Signalling peer process timers */
	ua_opt_conf_spp_t config;	/* Signalling peer process configuration */
	ua_stats_spp_t stats;		/* Signalling peer process statistics */
	ua_stats_spp_t statsp;		/* Signalling peer process statistics periods */
};

/*
 *  GP (AS to SPP mapping) structure
 *  -----------------------------------
 *  The GP structure maps AS Users (AS-U) to remote ASPs that are using the AS.
 *  It also indicates the RC (base IID) that is used by the remote ASP for this
 *  AS-U and the state of the remote ASP for this AS-U.
 *
 *  The GP structure maps AS Users (AS-P) to remote SGPs that are providing the
 *  AS.  It also indicates the RC (base IID) that is provided by the remote SGP
 *  for this AS-P and the state of the remote SGP for this AS-P.
 */
struct gp {
	uint32_t state;			/* AS state for this SPP */
	uint32_t flags;			/* AS flags for this SPP */
	uint32_t s_state;		/* signalling state */
	SLIST_LINKAGE (as, gp, as);	/* AS linkage */
	SLIST_LINKAGE (spp, gp, spp);	/* SPP linkage */
	SLIST_COUNT (rp);		/* RP list */
	bufq_t bufq;			/* Buffer queue */
	ua_ldkey_t key;			/* Load key */
	ua_timers_gp_t timers;		/* Load selection timers */
};

/*
 *  PP (SPP to XP mapping) structure
 *  -----------------------------------
 *  An ASP/SGP/SPP can be available via multiple transports: one direct transport and many
 *  cross-link transports.  This mapping structure describes the state of the ASP/SGP/SPP for each
 *  transport that provides access to it.  The SPP state is the overall state considering the state
 *  for each transport.
 *
 *  This complication is necessary due to the fact that with ASP-ASP, SGP-SGP or SPP-SPP
 *  cross-links, an ASP/SGP/SPP can be contacted both via direct connection as well as via multiple
 *  cross-links.  The state of the SPP (ASP_DOWN, ASP_WACK_ASPUP, ASP_WACK_ASPDN, ASP_UP) must be
 *  maintained separately for each transport in this PP structure, and coordinated into an overall
 *  SPP state maintained in SPP structure.
 */
struct pp {
	uint32_t state;			/* SPP state for this XP */
	uint32_t flags;			/* SPP flags for this XP */
	SLIST_LINKAGE (spp, pp, spp);	/* SPP linkage */
	SLIST_LINKAGE (xp, pp, xp);	/* XP linkage */
	SLIST_HEAD (rp);		/* RP list */
};

/*
 *  RP (GP to PP mapping) structure
 *  -----------------------------------
 *  An ASP/SGP/SPP can be in a separate state for a given AS for each transport by way of which the
 *  ASP/SGP/SPP may be contacted.  This structure maps the overall state of the SPP for the AS (GP
 *  structure) to the specific transport for the SPP (PP structure) and maintains the state of the
 *  SPP in the AS via a specific transport (XP).  The GP state is the overall state of the SPP
 *  within the AS, and the RP state is the state of the SPP within the AS for a specific transport (XP).
 *
 *  This complication is necessary dues to the fact that with ASP-ASP, SGP-SGP or SPP-SPP
 *  corss-links, an ASP/SGP/SPP can be separately active for an AS for each transport.  The state of
 *  the SPP for the AS (GP) (AS_DOWN, AS_WACK_RREQ/ASPUP, AS_WACK_DREQ/ASPDN, AS_INACTIVE,
 *  AS_WACK_ASPAC, AS_WACK_ASPIA, AS_ACTIVE) must be maintained separately for each AS, SPP and
 *  transport XP in this RP structure, and coordinated into an overall SPP state within the AS
 *  maintained in the GP structure (and ultimately the overall AS state maintained in the AS
 *  structure).
 */
struct rp {
	uint32_t state;			/* AS state for this SPP (GP), the SPP having this XP (PP) */
	uint32_t flags;			/* AS flags for this SPP (GP), the SPP having this XP (PP) */
	SLIST_LINKAGE (gp, rp, gp);	/* GP linkage */
	SLIST_LINKAGE (pp, rp, pp);	/* RP linkage */
};

/*
 *  SS structure
 *  -----------------------------------
 */
struct ss {
	STR_DECLARATION (struct ss);	/* stream declaration */
	uint32_t s_state;		/* signalling service state */
	union {
		uint32_t iid;		/* interface id for this service */
		uint32_t rc;		/* routing context for this service */
	};
	struct ss *ioc;			/* */
	SLIST_LINKAGE (as, ss, as);	/* AS-U linkage */
	ua_addr_t add;			/* Signalling service PPA */
	lmi_info_ack_t info;		/* signalling service information */
	lmi_option_t proto;		/* protocol options and variant */
	ua_timers_ss_t timers;		/* Signalling service timers */
	ua_opt_conf_ss_t config;	/* Signalling service configuration */
	ua_stats_ss_t stats;		/* Signalling service statistics */
	ua_stats_ss_t statsp;		/* Signalling service statistics periods */
};

#define SS_PRIV(__q) ((struct ss *)(__q)->q_ptr)

#define SLS_IDLE		0	/* Link idle */
#define SLS_WCON_EREQ		1	/* Link being established */
#define SLS_WCON_RELREQ		2	/* Link being released */
#define SLS_ESTABLISHED		3	/* Link established */
#define SLS_UNKNOWN		4	/* Link state unknown */

#define SLSF_IDLE		(1<<SLS_IDLE)
#define SLSF_WCON_EREQ		(1<<SLS_WCON_EREQ)
#define SLSF_WCON_RELREQ	(1<<SLS_WCON_RELREQ)
#define SLSF_ESTABLISHED	(1<<SLS_ESTABLISHED)
#define SLSF_UNKNOWN		(1<<SLS_UNKNOWN)

/*
 *  XP structure
 *  -----------------------------------
 */
#define MAX_TPI_ADDR_SIZE sizeof(struct sockaddr)
struct xp {
	STR_DECLARATION (struct xp);	/* stream declaration */
	struct sp *sp;			/* associated SP/SG */
	SLIST_COUNT (spp);		/* Associated ASP/SGP/SPP */
	SLIST_HEAD (pp);		/* PP (SPP graph) */
	mblk_t *nm_reassem;		/* normal reassembly buffer */
	mblk_t *ex_reassem;		/* expedited reassembly buffer */
	uchar loc[MAX_TPI_ADDR_SIZE];	/* loc opaque address */
	uchar rem[MAX_TPI_ADDR_SIZE];	/* rem opaque address */
	struct T_info_ack info;		/* protocol information */
	ua_timers_xp_t timers;		/* Transport timers */
	ua_opt_conf_xp_t config;	/* Transport configuration */
	ua_stats_xp_t stats;		/* Transport statistics */
	ua_stats_xp_t statsp;		/* Transport statistics periods */
};

#define XP_PRIV(__q) ((struct xp *)(__q)->q_ptr)

#define ASP_DOWN	0	/* ASP down */
#define ASP_WACK_ASPUP	1	/* ASP waiting to come up */
#define ASP_WACK_ASPDN	2	/* ASP waiting to go down */
#define ASP_UP		3	/* ASP up */
#define ASP_INACTIVE	6	/* ASP inactive */
#define ASP_WACK_ASPAC	7	/* ASP waiting to go active */
#define ASP_WACK_ASPIA	8	/* ASP waiting for inactive */
#define ASP_ACTIVE	9	/* ASP active */

/*
 *  LM Structure
 *  -----------------------------------
 *  This is a layer management structure.  It it used for UA management streams.
 */
struct lm {
	STR_DECLARATION (struct lm);	/* stream declaration */
};

#define MGM_PRIV(__q) ((struct lm *)(__q)->q_ptr)

/* M2UA Link Key */
struct lk {
	ushort sdti;			/* signalling data terminal identifier */
	ushort sdli;			/* signalling data link identifier */
};

/* FIXME: also need M3UA and SUA routing keys */

union priv {
	struct str str;
	struct ss ss;
	struct lm lm;
};

#define PRIV(__q) ((union priv *)(__q)->q_ptr)

union link {
	struct str str;
	struct ss ss;
	struct xp xp;
};

#define LINK(__q) ((union link *)(__q)->q_ptr)

/*
 *  DF structure
 *  -----------------------------------
 */
struct df {
	spinlock_t lock;		/* structure lock */
	struct lm *lm;			/* management stream */
	SLIST_HEAD (as, as);		/* master list of */
	SLIST_HEAD (sp, sp);		/* master list of */
	SLIST_HEAD (spp, spp);		/* master list of */
	struct {
		union priv *list;
		size_t numb;
	} priv;
	struct {
		union link *list;
		size_t numb;
	} link;
	lmi_option_t proto;		/* default protocol options */
	lmi_notify_t notify;		/* default notify options */
	ua_timers_df_t timers;		/* default timers */
	ua_opt_conf_df_t config;	/* default configuration */
	ua_stats_df_t stats;		/* default statistics */
	ua_stats_df_t statsp;		/* default statistics periods */
};

STATIC struct df master;

/*
 *  Forward declarations.
 */
STATIC union priv *ua_alloc_priv(queue_t *, union priv **, dev_t *, cred_t *, minor_t);
STATIC void ua_free_priv(queue_t *);
STATIC union priv *priv_get(union priv *);
STATIC void priv_put(union priv *);

STATIC struct as *ua_alloc_as(uint32_t, uint32_t, struct sp *, uint32_t, ua_addr_t *);
STATIC void ua_free_as(struct as *);
STATIC struct as *as_get(struct as *);
STATIC void as_put(struct as *);
STATIC uint32_t as_get_id(uint32_t);
STATIC struct as *as_lookup(uint32_t);

STATIC struct sp *ua_alloc_sp(uint32_t, uint32_t, struct sp *, uint32_t, uint32_t);
STATIC void ua_free_sp(struct sp *);
STATIC struct sp *sp_get(struct sp *);
STATIC void sp_put(struct sp *);
STATIC uint32_t sp_get_id(uint32_t);
STATIC struct sp *sp_lookup(uint32_t);

STATIC struct spp *ua_alloc_spp(uint32_t, uint32_t, struct sp *, uint32_t, uint32_t);
STATIC void ua_free_spp(struct spp *);
STATIC struct spp *spp_get(struct spp *);
STATIC void spp_put(struct spp *);
STATIC uint32_t spp_get_id(uint32_t);
STATIC struct spp *spp_lookup(uint32_t);

STATIC struct ap *ua_alloc_ap(struct as *, struct as *);
STATIC void ua_free_ap(struct ap *);

STATIC struct np *ua_alloc_np(struct sp *, struct sp *);
STATIC void ua_free_np(struct np *);

STATIC struct gp *ua_alloc_gp(struct as *, struct spp *);
STATIC void ua_free_gp(struct gp *);

STATIC void ua_alloc_ss(struct ss *, uint32_t, uint32_t, struct as *, uint32_t, ua_addr_t *);
STATIC void ua_free_ss(struct ss *);
STATIC struct ss *ss_get(struct ss *);
STATIC void ss_put(struct ss *);
STATIC uint32_t ss_get_id(uint32_t);
STATIC struct ss *ss_lookup(uint32_t);

STATIC int ua_alloc_xp(struct xp *, uint32_t, uint32_t, struct spp *, struct sp *);
STATIC void ua_free_xp(struct xp *);
STATIC struct xp *xp_get(struct xp *);
STATIC void xp_put(struct xp *);
STATIC uint32_t xp_get_id(uint32_t);
STATIC struct xp *xp_lookup(uint32_t);

STATIC union link *ua_alloc_link(queue_t *, union link **, uint32_t, cred_t *);
STATIC void ua_free_link(queue_t *);
STATIC union link *link_get(union link *);
STATIC void link_put(union link *);
STATIC union link *link_lookup(uint32_t);

STATIC INLINE uint32_t
ss_get_i_state(struct ss *ss)
{
	return ss->i_state;
}
STATIC INLINE void
ss_set_i_state(struct ss *ss, uint32_t state)
{
	ss->i_state = state;
}

/*
 *  UA MESSAGES
 */

struct ua_parm {
	union {
		uchar *cp;		/* pointer to parameter field */
		uint32_t *wp;		/* pointer to parameter field */
	};
	size_t len;			/* length of paramter field */
	uint32_t val;			/* value of first 4 bytes */
};

struct ua_msg {
	mblk_t *mp;			/* the original message block */
	uint32_t version;		/* message version */
	uint32_t class;			/* message class */
	uint32_t type;			/* message type */
	uint32_t len;			/* message length */
	struct ua_parm rc;		/* routing context */
	struct ua_parm iid;		/* interface id */
	struct ua_parm iid_text;	/* interface id (text) */
	struct ua_parm iid_range;	/* interface id (range) */
	struct ua_parm info;		/* information */
	struct ua_parm diag;		/* diagnostic */
	struct ua_parm hinfo;		/* heartbeat information */
	struct ua_parm reason;
	struct ua_parm tmode;		/* traffic mode */
	struct ua_parm ecode;
	struct ua_parm aspid;		/* asp identifier */
	struct ua_parm ssn;
	struct ua_parm smi;
	struct ua_parm cong;		/* congestion status */
	struct ua_parm disc;		/* discard status */
	struct ua_parm cause;
	struct ua_parm status;		/* status */
	struct ua_parm result;		/* result */
	struct ua_parm corid;		/* correlation id */
	struct ua_parm keyid;		/* link or routing key id */
	union {
		struct {
		} iua;
		struct {
			struct ua_parm data1;	/* data type 1 */
			struct ua_parm data2;	/* data type 2 */
			struct ua_parm event;	/* event */
			struct ua_parm action;	/* action */
			struct ua_parm seqno;	/* sequence number */
			struct ua_parm linkkey;
			struct ua_parm sdti;
			struct ua_parm sdli;
		} m2ua;
		struct {
			struct ua_parm na;
			struct ua_parm cdest;
			struct ua_parm rk;
			struct ua_parm dpc;
			struct ua_parm si;
			struct ua_parm opc_list;
			struct ua_parm prot_data;
		} m3ua;
		struct {
		} sua;
		struct {
		} tua;
	};
};

#define M2UA_PPI    5
/* FIXME: need other PPIs */

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
#define UA_CLASS_QPTM	0x05	/* Q.931 User Part Messages */
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
#define UA_PARM_IID_RANGE_O	UA_CONST_PHDR(0x0002,0)	/* m2ua-10 */
#define UA_PARM_DATA		UA_CONST_PHDR(0x0003,0)	/* sua-07 */
#define UA_PARM_IID_TEXT	UA_CONST_PHDR(0x0003,0)
#define UA_PARM_INFO		UA_CONST_PHDR(0x0004,0)
//#define UA_PARM_APC           UA_CONST_PHDR(0x0005,sizeof(uint32_t))  /* sua-07 */
#define UA_PARM_DLCI		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_RC		UA_CONST_PHDR(0x0006,sizeof(uint32_t))
#define UA_PARM_DIAG		UA_CONST_PHDR(0x0007,0)
#define UA_PARM_IID_RANGE	UA_CONST_PHDR(0x0008,0)	/* rfc3057, rfc3331 */
#define UA_PARM_HBDATA		UA_CONST_PHDR(0x0009,0)
#define UA_PARM_REASON		UA_CONST_PHDR(0x000a,sizeof(uint32_t))
#define UA_PARM_TMODE		UA_CONST_PHDR(0x000b,sizeof(uint32_t))
#define UA_PARM_ECODE		UA_CONST_PHDR(0x000c,sizeof(uint32_t))
#define UA_PARM_STATUS		UA_CONST_PHDR(0x000d,sizeof(uint32_t))
//#define UA_PARM_ASPID         UA_CONST_PHDR(0x000e,sizeof(uint32_t))
#define UA_PARM_PROT_DATA	UA_CONST_PHDR(0x000e,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_CONG_LEVEL	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_REL_REASON	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_TEI_STATUS	UA_CONST_PHDR(0x0010,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_ASPID		UA_CONST_PHDR(0x0011,sizeof(uint32_t))
#define UA_PARM_APC		UA_CONST_PHDR(0x0012,sizeof(uint32_t))
#define UA_PARM_CORID		UA_CONST_PHDR(0x0013,sizeof(uint32_t))
#define UA_PARM_REG_RESULT	UA_CONST_PHDR(0x0014,sizeof(uint32_t))	/* rfc3868, tua-04 */
#define UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x0015,sizeof(uint32_t)*4)	/* rfc3868, tua-04 */
#define UA_PARM_REG_STATUS	UA_CONST_PHDR(0x0016,sizeof(uint32_t))	/* rfc3868, tua-04 */
#define UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0017,sizeof(uint32_t))	/* rfc3868, tua-04 */
#define UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x0018,sizeof(uint32_t))	/* rfc3868, tua-04 */

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

#define UA_DEREG_STATUS_SUCCESS			(0x0)
#define UA_DEREG_STATUS_UNKNOWN			(0x1)
#define UA_DEREG_STATUS_INVALID_ID		(0x2)
#define UA_DEREG_STATUS_PERMISSION_DENIED	(0x3)
#define UA_DEREG_STATUS_NOT_REGISTERED		(0x4)
#define UA_DEREG_STATUS_ID_ACTIVE_FOR_AS	(0x5)	/* but not m2ua */

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

/* M3UA Specific parameters. */

#define M3UA_PARM_NTWK_APP	UA_CONST_PHDR(0x0200,sizeof(uint32_t))
#define M3UA_PARM_PROT_DATA1	UA_CONST_PHDR(0x0201,0)
#define M3UA_PARM_PROT_DATA2	UA_CONST_PHDR(0x0202,0)
#define M3UA_PARM_AFFECT_DEST	UA_CONST_PHDR(0x0203,sizeof(uint32_t))
#define M3UA_PARM_USER_CAUSE	UA_CONST_PHDR(0x0204,sizeof(uint32_t))
#define M3UA_PARM_CONG_IND	UA_CONST_PHDR(0x0205,sizeof(uint32_t))
#define M3UA_PARM_CONCERN_DEST	UA_CONST_PHDR(0x0206,sizeof(uint32_t))
#define M3UA_PARM_ROUTING_KEY	UA_CONST_PHDR(0x0207,0)
#define M3UA_PARM_REG_RESULT	UA_CONST_PHDR(0x0208,sizeof(uint32_t))
#define M3UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x0209,sizeof(uint32_t)*4)
#define M3UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x020a,sizeof(uint32_t))
#define M3UA_PARM_DPC		UA_CONST_PHDR(0x020b,0)
#define M3UA_PARM_SI		UA_CONST_PHDR(0x020c,0)
#define M3UA_PARM_SSN		UA_CONST_PHDR(0x020d,0)
#define M3UA_PARM_OPC		UA_CONST_PHDR(0x020e,0)
#define M3UA_PARM_CIC		UA_CONST_PHDR(0x020f,0)
#define M3UA_PARM_PROT_DATA3	UA_CONST_PHDR(0x0210,0)	/* proposed */
#define M3UA_PARM_REG_STATUS	UA_CONST_PHDR(0x0212,sizeof(uint32_t))
#define M3UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0213,sizeof(uint32_t))

/* SUA Specific parameters. */

#define SUA_PARM_FLAGS		UA_CONST_PHDR(0x0101,sizeof(uint32_t))
#define SUA_PARM_SRCE_ADDR	UA_CONST_PHDR(0x0102,sizeof(uint32_t)*5)	/* XXX */
#define SUA_PARM_DEST_ADDR	UA_CONST_PHDR(0x0103,0)
#define SUA_PARM_SRN		UA_CONST_PHDR(0x0104,sizeof(uint32_t))
#define SUA_PARM_DRN		UA_CONST_PHDR(0x0105,sizeof(uint32_t))
#define SUA_PARM_CAUSE		UA_CONST_PHDR(0x0106,sizeof(uint32_t))
#define SUA_PARM_SEQ_NUM	UA_CONST_PHDR(0x0107,sizeof(uint32_t))
#define SUA_PARM_RX_SEQ_NUM	UA_CONST_PHDR(0x0108,sizeof(uint32_t))
#define SUA_PARM_ASP_CAPS	UA_CONST_PHDR(0x0109,sizeof(uint32_t))
#define SUA_PARM_CREDIT		UA_CONST_PHDR(0x010a,sizeof(uint32_t))
#define SUA_PARM_RESERVED1	UA_CONST_PHDR(0x010b,0)
#define SUA_PARM_SMI_SUBSYS	UA_CONST_PHDR(0x010c,sizeof(uint32_t))
#define SUA_PARM_CAUSE		UA_CONST_PHDR(0x010c,sizeof(uint32_t))
#define SUA_PARM_SMI		UA_CONST_PHDR(0x0112,sizeof(uint32_t))
#define SUA_PARM_CONG		UA_CONST_PHDR(0x0118,sizeof(uint32_t))

#define SUA_SPARM_GT		UA_CONST_PHDR(0x8001,0)
#define SUA_SPARM_PC		UA_CONST_PHDR(0x8002,sizeof(uint32_t))
#define SUA_SPARM_SSN		UA_CONST_PHDR(0x8003,sizeof(uint32_t))
#define SUA_SPARM_IPV4_ADDR	UA_CONST_PHDR(0x8004,sizeof(uint32_t))
#define SUA_SPARM_HOSTNAME	UA_CONST_PHDR(0x8005,0)
#define SUA_SPARM_IPV6_ADDR	UA_CONST_PHDR(0x8006,sizeof(uint32_t)*4)

/* TUA Specific parameters */

#define TUA_PARM_QOS		UA_CONST_PHDR(0x0101,sizeof(uint32_t))
#define TUA_PARM_DEST_ADDR	UA_CONST_PHDR(0x0102,0)
#define TUA_PARM_ORIG_ADDR	UA_CONST_PHDR(0x0103,0)
#define TUA_PARM_APPL_CTXT	UA_CONST_PHDR(0x0104,0)
#define TUA_PARM_USER_INFO	UA_CONST_PHDR(0x0105,0)
#define TUA_PARM_COMP_PRES	UA_CONST_PHDR(0x0106,sizeof(uint32_t))
#define TUA_PARM_TERM		UA_CONST_PHDR(0x0107,sizeof(uint32_t))
#define TUA_PARM_P_ABORT	UA_CONST_PHDR(0x0108,sizeof(uint32_t))
#define TUA_PARM_REPT_CAUS	UA_CONST_PHDR(0x0109,sizeof(uint32_t))

#define TUA_PARM_INVK_ID	UA_CONST_PHDR(0x0201,sizeof(uint32_t))
#define TUA_PARM_LAST_COMP	UA_CONST_PHDR(0x0202,sizeof(uint32_t))
#define TUA_PARM_TIMEOUT	UA_CONST_PHDR(0x0203,sizeof(uint32_t))
#define TUA_PARM_OPCODE		UA_CONST_PHDR(0x0204,sizeof(uint32_t)*2)
#define TUA_PARM_PARMS		UA_CONST_PHDR(0x0205,0)
#define TUA_PARM_ERROR		UA_CONST_PHDR(0x0206,sizeof(uint32_t))
#define TUA_PARM_PBCODE		UA_CONST_PHDR(0x0207,sizeof(uint32_t))
#define TUA_PARM_CORR_ID	UA_CONST_PHDR(0x0208,sizeof(uint32_t))

#define TUA_PARM_SECU_CTXT	UA_CONST_PHDR(0x0301,0)
#define TUA_PARM_CONFIDENT	UA_CONST_PHDR(0x0302,0)
#define TUA_PARM_UABT_INFO	UA_CONST_PHDR(0x0303,0)

#define TUA_PARM_ABCODE		UA_CONST_PHDR(0x0401,sizeof(uint32_t))
#define TUA_PARM_CLASS		UA_CONST_PHDR(0x0402,sizeof(uint32_t))

#define TUA_PARM_SSN		UA_CONST_PHDR(0x0419,sizeof(uint32_t))
#define TUA_PARM_SMI		UA_CONST_PHDR(0x041a,sizeof(uint32_t))
#define TUA_PARM_CONG		UA_CONST_PHDR(0x041b,sizeof(uint32_t))
#define TUA_PARM_CAUSE		UA_CONST_PHDR(0x041c,sizeof(uint32_t))

#define TUA_PARM_CNV_TYPE	UA_CONST_PHDR(0x0503,sizeof(uint32_t))
#define TUA_PARM_INV_TYPE	UA_CONST_PHDR(0x0504,sizeof(uint32_t))
#define TUA_PARM_REJ_TYPE	UA_CONST_PHDR(0x0505,sizeof(uint32_t))
#define TUA_PARM_QRY_TYPE	UA_CONST_PHDR(0x0506,sizeof(uint32_t))

/* FIXME: need other UA specific message definitions */

/*
 * PRIMITIVES
 */
/*
 *  Primitives sent upstream (to SS User)
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(struct ss *ss, queue_t *q, int error)
{
	mblk_t *mp;
	int hangup = 0;

	if (error < 0)
		error = -error;
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case ENOMEM:
	case EAGAIN:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
		hangup = 1;
	}
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		if (hangup) {
			mp->b_datap->db_type = M_HANGUP;
			printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (-error);
		} else {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = error;
			*(mp->b_wptr)++ = error;
			printd(("%s: %p: <- M_ERROR\n", DRV_NAME, ss));
			putnext(ss->oq, mp);
			return (QR_DONE);
		}
	}
	return (-ENOBUFS);
}

#include "ua_sl.h"
#include "ua_mtp.h"
#include "ua_tri.h"
#include "ua_tci.h"
#include "ua_dlpi.h"
#include "ua_npi.h"
#include "ua_tpi.h"

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream (to SS Provider)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  =========================================================================
 *
 *  UA Messages
 *
 *  =========================================================================
 */

STATIC int
ua_dec_msg(struct xp *xp, queue_t *q, mblk_t *mp, struct ua_msg *m)
{
	uint32_t hdr;
	size_t mlen = msgsize(mp);
	uint32_t *wp = (typeof(wp)) mp->b_rptr;
	if (mlen < 2 * sizeof(uint32_t))
		return (-EMSGSIZE);
	bzero(m, sizeof(*m));
	hdr = *wp++;
	m->mp = mp;
	m->version = UA_MSG_VERS(hdr);
	m->class = UA_MSG_CLAS(hdr);
	m->type = UA_MSG_TYPE(hdr);
	m->len = *wp++;
	if (mlen < m->len)
		return (-EMSGSIZE);
	for (; (uchar *) (wp + 1) <= mp->b_wptr; wp += (UA_PLEN(*wp) + 3) >> 2) {
		struct ua_parm *parm = NULL;

		switch (UA_PTAG(*wp)) {
		case UA_TAG(UA_PARM_IID):	/* UA_CONST_PHDR(0x0001,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->iid;
			break;
		case UA_TAG(UA_PARM_IID_RANGE_O):	/* UA_CONST_PHDR(0x0002,0) */
			parm = &m->iid_range;
			break;
		case UA_TAG(UA_PARM_IID_TEXT):	/* UA_CONST_PHDR(0x0003,0) */
			parm = &m->iid_text;
			break;
		case UA_TAG(UA_PARM_INFO):	/* UA_CONST_PHDR(0x0004,0) */
			parm = &m->info;
			break;
		case UA_TAG(UA_PARM_DIAG):	/* UA_CONST_PHDR(0x0007,0) */
			parm = &m->diag;
			break;
		case UA_TAG(UA_PARM_IID_RANGE):	/* UA_CONST_PHDR(0x0008,0) */
			parm = &m->iid_range;
			break;
		case UA_TAG(UA_PARM_HBDATA):	/* UA_CONST_PHDR(0x0009,0) */
			parm = &m->hinfo;
			break;
		case UA_TAG(UA_PARM_REASON):	/* UA_CONST_PHDR(0x000a,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->reason;
			break;
		case UA_TAG(UA_PARM_TMODE):	/* UA_CONST_PHDR(0x000b,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->tmode;
			break;
		case UA_TAG(UA_PARM_ECODE):	/* UA_CONST_PHDR(0x000c,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->ecode;
			break;
		case UA_TAG(UA_PARM_STATUS):	/* UA_CONST_PHDR(0x000d,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->status;
			break;
		case UA_TAG(UA_PARM_ASPID):	/* UA_CONST_PHDR(0x000e,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->aspid;
			break;
		}
		switch (UA_PTAG(*wp)) {
		case UA_TAG(M2UA_PARM_DATA1):	/* UA_CONST_PHDR(0x0300,0) */
			parm = &m->m2ua.data1;
			break;
		case UA_TAG(M2UA_PARM_DATA2):	/* UA_CONST_PHDR(0x0301,0) */
			parm = &m->m2ua.data2;
			break;
		case UA_TAG(M2UA_PARM_STATE_REQUEST):	/* UA_CONST_PHDR(0x0302,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->status;
			break;
		case UA_TAG(M2UA_PARM_STATE_EVENT):	/* UA_CONST_PHDR(0x0303,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m2ua.event;
			break;
		case UA_TAG(M2UA_PARM_CONG_STATUS):	/* UA_CONST_PHDR(0x0304,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->cong;
			break;
		case UA_TAG(M2UA_PARM_DISC_STATUS):	/* UA_CONST_PHDR(0x0305,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->disc;
			break;
		case UA_TAG(M2UA_PARM_ACTION):	/* UA_CONST_PHDR(0x0306,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m2ua.action;
			break;
		case UA_TAG(M2UA_PARM_SEQNO):	/* UA_CONST_PHDR(0x0307,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m2ua.seqno;
			break;
		case UA_TAG(M2UA_PARM_RETR_RESULT):	/* UA_CONST_PHDR(0x0308,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->result;
			break;
		case UA_TAG(M2UA_PARM_LINK_KEY):	/* UA_CONST_PHDR(0x0309,sizeof(uint32_t)*6) 
							 */
			if (UA_PLEN(*wp) >= sizeof(uint32_t) * 6)
				parm = &m->m2ua.linkkey;
			break;
		case UA_TAG(M2UA_PARM_LOC_KEY_ID):	/* UA_CONST_PHDR(0x030a,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->keyid;
			break;
		case UA_TAG(M2UA_PARM_SDTI):	/* UA_CONST_PHDR(0x030b,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m2ua.sdti;
			break;
		case UA_TAG(M2UA_PARM_SDLI):	/* UA_CONST_PHDR(0x030c,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m2ua.sdli;
			break;
		case UA_TAG(M2UA_PARM_REG_RESULT):	/* UA_CONST_PHDR(0x030d,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->result;
			break;
		case UA_TAG(M2UA_PARM_REG_STATUS):	/* UA_CONST_PHDR(0x030e,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->status;
			break;
		case UA_TAG(M2UA_PARM_DEREG_RESULT):	/* UA_CONST_PHDR(0x030f,sizeof(uint32_t)*4) 
							 */
			if (UA_PLEN(*wp) >= sizeof(uint32_t) * 4)
				parm = &m->result;
			break;
		case UA_TAG(M2UA_PARM_DEREG_STATUS):	/* UA_CONST_PHDR(0x0310,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->status;
			break;
		case UA_TAG(M2UA_PARM_CORR_ID):	/* UA_CONST_PHDR(0x0311,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m2ua.corid;
			break;
		case UA_TAG(M2UA_PARM_CORR_ID_ACK):	/* UA_CONST_PHDR(0x0312,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m2ua.corid;
			break;
		}
		switch (UA_PTAG(*wp)) {
		case UA_TAG(M3UA_PARM_NTWK_APP):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m3ua.na;
			break;
		case UA_TAG(M3UA_PARM_USER_CAUSE):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->cause;
			break;
		case UA_TAG(M3UA_PARM_CONG_IND):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->cong;
			break;
		case UA_TAG(M3UA_PARM_CONCERN_DEST):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m3ua.cdest;
			break;
		case UA_TAG(M3UA_PARM_ROUTING_KEY):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m3ua.rk;
			break;
		case UA_TAG(M3UA_PARM_REG_RESULT):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->result;
			break;
		case UA_TAG(M3UA_PARM_DEREG_RESULT):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->result;
			break;
		case UA_TAG(M3UA_PARM_LOC_KEY_ID):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->keyid;
			break;
		case UA_TAG(M3UA_PARM_DPC):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m3ua.dpc;
			break;
		case UA_TAG(M3UA_PARM_SI):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m3ua.si;
			break;
		case UA_TAG(M3UA_PARM_OPC):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->m3ua.opc_list;
			break;
		case UA_TAG(M3UA_PARM_PROT_DATA3):
			parm = &m->m3ua.prot_data;
			break;
		case UA_TAG(M3UA_PARM_REG_STATUS):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->status;
			break;
		case UA_TAG(M3UA_PARM_DEREG_STATUS):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->status;
			break;
		}
		switch (UA_PTAG(*wp)) {
		case UA_TAG(SUA_SPARM_SSN):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->ssn;
			break;
		case UA_TAG(SUA_PARM_SMI):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->smi;
			break;
		case UA_TAG(SUA_PARM_CONG):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->cong;
			break;
		case UA_TAG(SUA_PARM_CAUSE):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->cause;
			break;
		}
		switch (UA_PTAG(*wp)) {
		case UA_TAG(TUA_PARM_SSN):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->ssn;
			break;
		case UA_TAG(TUA_PARM_SMI):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->smi;
			break;
		case UA_TAG(TUA_PARM_CONG):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->cong;
			break;
		case UA_TAG(TUA_PARM_CAUSE):
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->cause;
			break;
		}
		if (parm) {
			/* We accept the first of repeated parameters.  There are really no
			   instances where parameters are repeated. */
			if (parm->wp == NULL) {
				parm->wp = wp;
				parm->len = UA_PLEN(*wp);
				parm->val = ntohl(wp[1]);
			} else
				swerr();
		}
	}
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  UA Send Message Functions
 *
 *  -------------------------------------------------------------------------
 */
STATIC mblk_t *
ua_build_optdata_req(queue_t *q, t_uscalar_t flags, t_uscalar_t ppi, t_uscalar_t sid)
{
	mblk_t *mp;
	struct T_optdata_req *p;
	struct t_opthdr *oh;
	static const size_t opt_len = sizeof(*oh) + sizeof(ppi) + sizeof(*oh) + sizeof(sid);

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTDATA_REQ;
		p->DATA_flag = flags;
		p->OPT_length = opt_len;
		p->OPT_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);

		oh = (typeof(oh)) mp->b_wptr;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->len = sizeof(t_uscalar_t);
		mp->b_wptr += sizeof(*oh);
		*(t_uscalar_t *) mp->b_wptr = ppi;
		mp->b_wptr += sizeof(t_uscalar_t);

		oh = (typeof(oh)) mp->b_wptr;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->len = sizeof(t_uscalar_t);
		mp->b_wptr += sizeof(*oh);
		*(t_uscalar_t *) mp->b_wptr = sid;
		mp->b_wptr += sizeof(t_uscalar_t);
	}
	return (mp);
}

STATIC int
ua_send_optdata_req(queue_t *q, struct xp *xp, mblk_t *mp, mblk_t *dp)
{
	if (mp) {
		if (canputnext(q)) {
			if (dp) {
				linkb(mp, dp);
				putnext(q, mp);
				return (QR_ABSORBED);
			}
			putnext(q, mp);
			return (QR_DONE);
		}
		freemsg(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * ua_build_mgmt_err: - build a UA_MGMT_ERR message
 * @q: active queue
 * @ppi: protocol payload identifier
 * @ecode: error code
 * @dia_ptr: DIAGNOSTIC pointer
 * @dia_len: DIAGNOSTIC length
 */
STATIC INLINE mblk_t *
ua_build_mgmt_err(queue_t *q, uint32_t ppi, uint32_t ecode, uchar *dia_ptr, size_t dia_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_ECODE) + dia_len ? UA_SIZE(UA_PARM_DIAG) +
	    UA_PAD4(dia_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_MGMT_ERR;
			p[1] = htonl(mlen);;
			p[2] = UA_PARM_ECODE;
			p[3] = htonl(ecode);
			p += 4;
			if (dia_len) {
				*p++ = UA_PHDR(UA_PARM_DIAG, dia_len);
				bcopy(dia_ptr, p, dia_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(dia_len);

			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_mgmt_err: - send a UA_MGMT_ERR message
 * @xp: transport on which to send
 * @q: active queue
 * @ecode: error code
 * @dia_ptr: DIAGNOSTIC pointer
 * @dia_len: DIAGNOSTIC length
 */
STATIC INLINE int
ua_send_mgmt_err(struct xp *xp, queue_t *q, uint32_t ecode, uchar *dia_ptr, size_t dia_len)
{
	mblk_t *mp = ua_build_mgmt_err(q, xp->ppi, ecode, dia_ptr, dia_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_mgmt_ntfy: - build a UA_MGMT_NTFY message
 * @q: active queue
 * @ppi: protocol payload identifier
 * @status: status code
 * @aspid: ASP Id to include (if non-NULL)
 * @iid: Interface Ids to include if non-NULL
 * @num_iid: Number of Interface Ids
 * @rc: Routing Contexts to include if non-NULL
 * @num_rc: Number of Routing Contexts
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 *
 * FIXME: we do not need iids and rcs as arguments: just provide one and use
 * the type of the xp to determine whether to interpret the list of 32-bit
 * identifiers as interface ids or as routing contexts.
 */
STATIC INLINE mblk_t *
ua_build_mgmt_ntfy(queue_t *q, uint32_t ppi, uint32_t status, uint32_t *aspid, uint32_t *iid,
		   size_t num_iid, uint32_t *rc, size_t num_rc, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_STATUS) + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 +
	    num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_MGMT_NTFY;
			p[1] = htonl(mlen);
			p[2] = UA_PARM_STATUS;
			p[3] = htonl(status);
			p += 4;
			if (aspid) {
				p[0] = UA_PARM_ASPID;
				p[1] = *aspid;	/* already network byte order */
				p += 2;
			}
			if (num_iid) {
				uint32_t *ip = iid;

				*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
				while (num_iid--)
					*p++ = *ip++;	/* already network byte order */
			}
			if (num_rc) {
				uint32_t *rcp = rc;

				*p++ = UA_PHDR(UA_PARM_RC, num_rc * sizeof(uint32_t));
				while (num_rc--)
					*p++ = *rcp++;	/* already network byte order */

			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_mgmt_ntfy_asp: - send a UA_MGMT_NTFY message notifying for an ASP failure
 * @spp: ASP or SPP for which to send
 * @q: active queue
 * @asp: ASP or SPP failed
 * @inf_ptr: INFO pointer
 * @inf_len: INFO len
 *
 * There are only two notification messages, one that pertains to an AS and one that pertains to an
 * ASP/SPP.  This is the ASP/SPP version.  The only status for this version is ASP Failure.  The
 * message will include the ASP Id of the failed ASP (or it is a rather useless message).
 */
STATIC int
ua_send_mgmt_ntfy_asp(struct spp *spp, queue_t *q, struct spp *asp, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct xp *xp = spp->xp.xp;
	uint32_t *aspid = asp ? (asp->aspid ? &asp->aspid : NULL) : NULL;
	uint32_t ppi = 0;

	/* TODO: we could include the RC (IID) for which the failed ASP was configured by waling
	   the asp->gp.list */
	mp = ua_build_mgmt_ntfy(q, xp->ppi, UA_STATUS_ASP_FAILURE, aspid, NULL, 0, NULL, 0, inf_ptr,
				inf_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_send_mgmt_ntfy_as: - send a UA_MGMT_NTFY message notifying for an AS state change
 * @gp: ASP/AS or SPP/AS relation for which to send
 * @q: active queue
 * @status: status
 * @asp: ASP or SPP causing state transition
 * @inf_ptr: INFO pointer
 * @inf_len: INFO len
 *
 * There are two notification messages, one that pertains to an AS and one that pertains to an
 * ASP/SPP.  This is the AS version.  The status for this version is the resulting state of an AS
 * state change.  The message will include the RC (IID) for the AS for which the state has changed
 * and the ASP Id (if any) of the ASP that caused the state change to occur.
 */
STATIC int
ua_send_mgmt_ntfy_as(struct gp *gp, queue_t *q, uint32_t status, struct spp *asp, uchar *inf_ptr,
		     size_t inf_len)
{
	mblk_t *mp;
	struct spp *spp = gp->spp.spp;
	struct xp *xp = spp->xp.xp;
	struct as *as = gp->as.as;
	uint32_t tmode = as->tmode;
	uint32_t *aspid = asp ? (asp->aspid ? &asp->aspid : NULL) : NULL;
	uint32_t *iid, *rc, iid_val, rc_val;
	size_t num_iid, num_rc;
	uint32_t ppi;

	switch (as->level) {
	case AS_LEVEL_IUA:
		ppi = IUA_PPI;
		goto do_iid;
	case AS_LEVEL_M2UA:
		ppi = M2UA_PPI;
		goto do_iid;
	      do_iid:
		iid_val = htonl(as->iid);
		iid = &iid_val;
		num_iid = 1;
		rc_val = 0;
		rc = NULL;
		num_rc = 0;
		break;
	default:
		swerr();
		pp = 0;
		goto do_rc;
	case AS_LEVEL_M3UA:
		ppi = M3UA_PPI;
		goto do_rc;
	case AS_LEVEL_SUA:
		ppi = SUA_PPI;
		goto do_rc;
	case AS_LEVEL_TUA:
		ppi = TUA_PPI;
		goto do_rc;
	      do_rc:
		iid_val = 0;
		iid = NULL;
		num_iid = 0;
		rc_val = htonl(as->rc);
		rc = &rc_val;
		num_rc = 1;
		break;
	}

	mp = ua_build_mgmt_ntfy(q, ppi, status, aspid, iid, num_iid, rc, num_rc, inf_ptr, inf_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

static int
ua_send_mgmt_ntfy(struct gp *gp, queue_t *q, uint oldflags, uint newflags, struct spp *asp,
		  uchar *inf_ptr, size_t inf_len)
{
	int err;

	if (newflags & ASF_ACTIVE) {
		if (newflags & ASF_INSUFFICIENT_ASPS) {
			if (!(oldflags & ASF_INSUFFICIENT_ASPS))
				if ((err =
				     ua_send_mgmt_ntfy_as(gp, q, UA_STATUS_AS_INSUFFICIENT_ASPS,
							  asp, inf_ptr, inf_len)))
					return (err);
		} else {
			if (!(oldflags & ASF_ACTIVE))
				if ((err =
				     ua_send_mgmt_ntfy_as(gp, q, UA_STATUS_AS_ACTIVE, asp, inf_ptr,
							  inf_len)))
					return (err);
		}
		if (newflags & ASF_MINIMUM_ASPS) {
			if (!(oldflags & ASF_MINIMUM_ASPS))
				if ((err =
				     ua_send_mgmt_ntfy_as(gp, q, UA_STATUS_AS_MINIMUM_ASPS, asp,
							  inf_ptr, inf_len)))
					return (err);
		}
	} else if (newflags & ASF_PENDING) {
		if (!(oldflags & ASF_PENDING))
			if ((err =
			     ua_send_mgmt_ntfy_as(gp, q, UA_STATUS_AS_PENDING, asp, inf_ptr,
						  inf_len)))
				return (err);
	} else {
		if (oldflags & (ASF_ACTIVE | ASF_PENDING))
			if ((err =
			     ua_send_mgmt_ntfy_as(gp, q, UA_STATUS_AS_INACTIVE, asp, inf_ptr,
						  inf_len)))
				return (err);
	}
}

/**
 * ua_build_asps_aspup_req: - build ASP Up message (UA_ASPS_ASPUP_REQ)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @aspid: ASP Identifier
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE mblk_t *
ua_build_asps_aspup_req(queue_t *q, uint32_t ppi, uint32_t *aspid, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_ASPS_ASPUP_REQ;
			p[1] = htonl(mlen);
			p += 2;
			if (aspid) {
				p[0] = UA_PARM_ASPID;
				p[1] = *aspid;	/* already network byte order */
				p += 2;
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_asps_aspup_req: - send ASP Up message (UA_ASPS_ASPUP_REQ)
 * @spp: ASP or SPP from which to send message
 * @q: active queue
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_asps_aspup_req(struct spp *spp, queue_t *q, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct xp *xp = spp->xp.xp;
	uint32_t *aspid = spp->aspid ? &spp->aspid : NULL;

	mp = ua_build_asps_aspup_req(q, xp->ppi, aspid, inf_ptr, inf_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_asps_aspdn_req: - build ASP Down message (UA_ASPS_ASPDN_REQ)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @aspid: ASP Identifier
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE mblk_t *
ua_build_asps_aspdn_req(queue_t *q, uint32_t ppi, uint32_t *aspid, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_ASPS_ASPDN_REQ;
			p[1] = htonl(mlen);
			p += 2;
			if (aspid) {
				p[0] = UA_PARM_ASPID;
				p[1] = *aspid;	/* already network byte order */
				p += 2;
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_asps_aspdn_req: - send ASP Down message (UA_ASPS_ASPDN_REQ)
 * @spp: ASP or SPP from which to send message
 * @q: active queue
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_asps_aspdn_req(struct spp *spp, queue_t *q, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct xp *xp = spp->xp.xp;
	uint32_t *aspid = spp->aspid ? &spp->aspid : NULL;

	mp = ua_build_asps_aspdn_req(q, xp->ppi, aspid, inf_ptr, inf_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_asps_hbeat_req: - build BEAT message (UA_ASPS_HBEAT_REQ)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @hbt_ptr: Heartbeat Data pointer
 * @hbt_len: Heartbeat Data length
 */
STATIC INLINE int
ua_build_asps_hbeat_req(queue_t *q, uint32_t ppi, uchar *hbt_ptr, size_t hbt_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_ASPS_HBEAT_REQ;
			p[1] = htonl(mlen);
			p += 2;
			if (hbt_len) {
				*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
				bcopy(hbt_ptr, p, hbt_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_asps_hbeat_req: - send BEAT message (UA_ASPS_HBEAT_REQ)
 * @xp: transport on which to send
 * @q: active queue
 * @hbt_ptr: Heartbeat Data pointer
 * @hbt_len: Heartbeat Data length
 */
STATIC INLINE int
ua_send_asps_hbeat_req(struct xp *xp, queue_t *q, uchar *hbt_ptr, size_t hbt_len)
{
	mblk_t *mp = ua_build_asps_hbead_req(q, xp->ppi, hbt_ptr, hbt_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_asps_aspup_ack: - build ASP Up Ack message (UA_ASPS_ASPUP_ACK)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @aspid: ASP Identifier
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE mblk_t *
ua_build_asps_aspup_ack(queue_t *q, uint32_t ppi, uint32_t *aspid, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_ASPS_ASPUP_ACK;
			p[1] = htonl(mlen);
			p += 2;
			if (aspid) {
				p[0] = UA_PARM_ASPID;
				p[1] = *aspid;	/* already network byte order */
				p += 2;
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_asps_aspup_ack: - send ASP Up Ack message (UA_ASPS_ASPUP_ACK)
 * @spp: ASP or SPP for which to send message
 * @q: active queue
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_asps_aspup_ack(struct spp *spp, queue_t *q, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct xp *xp = spp->xp.xp;
	uint32_t *aspid = spp->aspid ? &spp->aspid : NULL;

	mp = ua_build_asps_aspup_ack(q, xp->ppi, aspid, inf_ptr, inf_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_asps_aspdn_ack: - build ASP Down Ack message (UA_ASPS_ASPDN_ACK)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @aspid: ASP Identifier
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE mblk_t *
ua_build_asps_aspdn_ack(queue_t *q, uint32_t ppi, uint32_t *aspid, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_ASPS_ASPDN_ACK;
			p[1] = htonl(mlen);
			p += 2;
			if (aspid) {
				p[0] = UA_PARM_ASPID;
				p[1] = *aspid;
				p += 2;
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_asps_aspdn_ack: - send ASP Down Ack message (UA_ASPS_ASPDN_ACK)
 * @spp: ASP or SPP for which to send message
 * @q: active queue
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_asps_aspdn_ack(struct spp *spp, queue_t *q, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct xp *xp = spp->xp.xp;
	uint32_t *aspid = spp->aspid ? &spp->aspid : NULL;

	mp = ua_build_asps_aspdn_ack(q, xp->ppi, aspid, inf_ptr, inf_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_asps_hbeat_ack: - build BEAT Ack message (UA_ASPS_HBEAT_ACK)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @hbt_ptr: Heartbeat Data pointer
 * @hbt_len: Heartbeat Data length
 */
STATIC INLINE mblk_t *
ua_build_asps_hbeat_ack(queue_t *q, uint32_t ppi, uchar *hbt_ptr, size_t hbt_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_ASPS_HBEAT_ACK;
			p[1] = htonl(mlen);
			p += 2;
			if (hbt_len) {
				*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
				bcopy(hbt_ptr, p, hbt_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_asps_hbeat_ack: - send BEAT Ack message (UA_ASPS_HBEAT_ACK)
 * @xp: transport on which to send
 * @q: active queue
 * @hbt_ptr: Heartbeat Data pointer
 * @hbt_len: Heartbeat Data length
 */
STATIC INLINE int
ua_send_asps_hbeat_ack(struct xp *xp, queue_t *q, uchar *hbt_ptr, size_t hbt_len)
{
	mblk_t *mp = ua_build_asps_hbeat_ack(q, xp->ppi, hbt_ptr, hbt_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_aspt_aspac_req: - build ASP Active message (UA_ASPT_ASPAC_REQ)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @tmode: traffic mode
 * @iid: interface ids
 * @num_iid: number of interface ids
 * @rc: routing contexts
 * @num_rc: number of routing contexts
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 *
 * FIXME: we do not need iids and rcs as arguments: just provide one and use
 * the type of the xp to determine whether to interpret the list of 32-bit
 * identifiers as interface ids or as routing contexts.
 */
STATIC INLINE mblk_t *
ua_build_aspt_aspac_req(queue_t *q, uint32_t ppi, uint32_t tmode, uint32_t *iid, size_t num_iid,
			uint32_t *rc, size_t num_rc, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_TMODE) + num_iid ? UA_PHDR_SIZE +
	    num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, iid ? *iid : rc ? *rc : 0))) {
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
					*p++ = *ip++;	/* already network byte order */
			}
			if (num_rc) {
				uint32_t *rcp = rc;
				*p++ = UA_PHDR(UA_PARM_RC, num_rc * sizeof(uint32_t));
				while (num_rc--)
					*p++ = *rcp++;	/* already network byte order */
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_aspt_aspac_req: - send ASP Active message (UA_ASPT_ASPAC_REQ)
 * @gp: ASP/AS or SPP/AS relation for which to send
 * @q: active queue
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_aspt_aspac_req(struct gp *gp, queue_t *q, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct spp *spp = gp->spp.spp;
	struct xp *xp = spp->xp.xp;
	struct as *as = gp->as.as;
	uint32_t tmode = as->tmode;
	uint32_t *iid, *rc, iid_val, rc_val;
	size_t num_iid, num_rc;
	uint32_t ppi;

	switch (as->level) {
	case AS_LEVEL_IUA:
		ppi = IUA_PPI;
		goto do_iid;
	case AS_LEVEL_M2UA:
		ppi = M2UA_PPI;
		goto do_iid;
	      do_iid:
		iid_val = htonl(as->iid);
		iid = &iid_val;
		num_iid = 1;
		rc_val = 0;
		rc = NULL;
		num_rc = 0;
		break;
	default:
		swerr();
		pp = 0;
		goto do_rc;
	case AS_LEVEL_M3UA:
		ppi = M3UA_PPI;
		goto do_rc;
	case AS_LEVEL_SUA:
		ppi = SUA_PPI;
		goto do_rc;
	case AS_LEVEL_TUA:
		ppi = TUA_PPI;
		goto do_rc;
	      do_rc:
		iid_val = 0;
		iid = NULL;
		num_iid = 0;
		rc_val = htonl(as->rc);
		rc = &rc_val;
		num_rc = 1;
		break;
	}

	mp = ua_build_aspt_aspac_req(q, ppi, tmode, iid, num_iid, rc, num_rc, inf_ptr, inf_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_aspt_aspia_req: - build ASP Inactive message (UA_ASPT_ASPIA_REQ)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @iid: interface identifiers
 * @num_iid: number of interface identifiers
 * @rc: routing contexts
 * @num_rc: number of routing contexts
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE mblk_t *
ua_build_aspt_aspia_req(queue_t *q, uint32_t ppi, uint32_t *iid, size_t num_iid,
			uint32_t *rc, size_t num_rc, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, iid ? *iid : rc ? *rc : 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_ASPT_ASPIA_REQ;
			p[1] = htonl(mlen);
			p += 2;
			if (num_iid) {
				uint32_t *ip = iid;

				*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
				while (num_iid--)
					*p++ = *ip++;	/* already network byte order */
			}
			if (num_rc) {
				uint32_t *rcp = rc;

				*p++ = UA_PHDR(UA_PARM_RC, num_rc * sizeof(uint32_t));
				while (num_rc--)
					*p++ = *rcp++;	/* already network byte order */
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_aspt_aspia_req: - send ASP Inactive message (UA_ASPT_ASPIA_REQ)
 * @gp: ASP/AS or SPP/AS relation for which to send message
 * @q: active queue
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_aspt_aspia_req(struct gp *gp, queue_t *q, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct spp *spp = gp->spp.spp;
	struct xp *xp = spp->xp.xp;
	struct as *as = gp->as.as;
	uint32_t *iid, *rc, iid_val, rc_val;
	size_t num_iid, num_rc;
	uint32_t ppi;

	switch (as->level) {
	case AS_LEVEL_IUA:
		ppi = IUA_PPI;
		goto do_iid;
	case AS_LEVEL_M2UA:
		ppi = M2UA_PPI;
		goto do_iid;
	      do_iid:
		iid_val = htonl(as->iid);
		iid = &iid_val;
		num_iid = 1;
		rc_val = 0;
		rc = NULL;
		num_rc = 0;
		break;
	default:
		swerr();
		pp = 0;
		goto do_rc;
	case AS_LEVEL_M3UA:
		ppi = M3UA_PPI;
		goto do_rc;
	case AS_LEVEL_SUA:
		ppi = SUA_PPI;
		goto do_rc;
	case AS_LEVEL_TUA:
		ppi = TUA_PPI;
		goto do_rc;
	      do_rc:
		iid_val = 0;
		iid = NULL;
		num_iid = 0;
		rc_val = htonl(as->rc);
		rc = &rc_val;
		num_rc = 1;
		break;
	}

	mp = ua_build_aspt_aspia_req(q, ppi, iid, num_iid, rc, num_rc, inf_ptr, inf_len);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_aspt_aspac_ack: - build ASP Inactive Ack message (UA_ASPT_ASPAC_ACK)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @iid: interface identifiers
 * @num_iid: number of interface identifiers
 * @rc: routing contexts
 * @num_rc: number of routing contexts
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE mblk_t *
ua_build_aspt_aspac_ack(queue_t *q, uint32_t ppi, uint tmode, uint32_t *iid, size_t num_iid,
			uint32_t *rc, size_t num_rc, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_TMODE) + num_iid ? UA_PHDR_SIZE +
	    num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, iid ? *iid : rc ? *rc : 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_ASPT_ASPAC_ACK;
			p[1] = htonl(mlen);
			p[2] = UA_PARM_TMODE;
			p[3] = htonl(tmode);
			p += 4;
			if (num_iid) {
				uint32_t *ip = iid;

				*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
				while (num_iid--)
					*p++ = *ip++;	/* already network byte order */
			}
			if (num_rc) {
				uint32_t *rcp = rc;
				*p++ = UA_PHDR(UA_PARM_RC, num_rc * sizeof(uint32_t));
				while (num_rc--)
					*p++ = *rcp++;	/* already network byte order */
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_aspt_aspac_ack: - send ASP Inactive Ack message (UA_ASPT_ASPAC_ACK)
 * @gp: SGP/AS or SPP/AS relation for which to send message
 * @q: active queue
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_aspt_aspac_ack(struct gp *gp, queue_t *q, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct spp *spp = gp->spp.spp;
	struct xp *xp = spp->xp.xp;
	struct as *as = gp->as.as;
	uint32_t tmode = as->tmode;
	uint32_t *iid, *rc, iid_val, rc_val;
	size_t num_iid, num_rc;
	uint32_t ppi;

	switch (as->level) {
	case AS_LEVEL_IUA:
		ppi = IUA_PPI;
		goto do_iid;
	case AS_LEVEL_M2UA:
		ppi = M2UA_PPI;
		goto do_iid;
	      do_iid:
		iid_val = htonl(as->iid);
		iid = &iid_val;
		num_iid = 1;
		rc_val = 0;
		rc = NULL;
		num_rc = 0;
		break;
	default:
		swerr();
		pp = 0;
		goto do_rc;
	case AS_LEVEL_M3UA:
		ppi = M3UA_PPI;
		goto do_rc;
	case AS_LEVEL_SUA:
		ppi = SUA_PPI;
		goto do_rc;
	case AS_LEVEL_TUA:
		ppi = TUA_PPI;
		goto do_rc;
	      do_rc:
		iid_val = 0;
		iid = NULL;
		num_iid = 0;
		rc_val = htonl(as->rc);
		rc = &rc_val;
		num_rc = 1;
		break;
	}

	mp = ua_build_aspt_aspac_ack(q, ppi, tmode, iid, num_iid, rc, num_rc, inf_ptr, inf_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_aspt_apia_ack: - build ASP Inactive Ack message (UA_ASPT_ASPIA_ACK)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @iid: interface identifiers
 * @num_iid: number of interface identifiers
 * @rc: routing contexts
 * @num_rc: number of routing contexts
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE mblk_t *
ua_build_aspt_aspia_ack(queue_t *q, uint32_t ppi, uint32_t *iid, size_t num_iid, uint32_t *rc,
			size_t num_rc, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, iid ? *iid : rc ? *rc : 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_ASPT_ASPIA_ACK;
			p[1] = htonl(mlen);
			p += 2;
			if (num_iid) {
				uint32_t *ip = iid;

				*p++ = UA_PHDR(UA_PARM_IID, num_iid * sizeof(uint32_t));
				while (num_iid--)
					*p++ = *ip++;	/* already network byte order */
			}
			if (num_rc) {
				uint32_t *rcp = rc;
				*p++ = UA_PHDR(UA_PARM_RC, num_rc * sizeof(uint32_t));
				while (num_rc--)
					*p++ = *rcp++;	/* already network byte order */
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_aspt_apia_ack: - send ASP Inactive Ack message (UA_ASPT_ASPIA_ACK)
 * @gp: SGP/AS or SPP/AS relation for which to send message
 * @q: active queue
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_aspt_aspia_ack(struct gp *gp, queue_t *q, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct spp *spp = gp->spp.spp;
	struct xp *xp = spp->xp.xp;
	struct as *as = gp->as.as;
	uint32_t *iid, *rc, iid_val, rc_val;
	size_t num_iid, num_rc;
	uint32_t ppi;

	switch (as->level) {
	case AS_LEVEL_IUA:
		ppi = IUA_PPI;
		goto do_iid;
	case AS_LEVEL_M2UA:
		ppi = M2UA_PPI;
		goto do_iid;
	      do_iid:
		iid_val = htonl(as->iid);
		iid = &iid_val;
		num_iid = 1;
		rc_val = 0;
		rc = NULL;
		num_rc = 0;
		break;
	default:
		swerr();
		pp = 0;
		goto do_rc;
	case AS_LEVEL_M3UA:
		ppi = M3UA_PPI;
		goto do_rc;
	case AS_LEVEL_SUA:
		ppi = SUA_PPI;
		goto do_rc;
	case AS_LEVEL_TUA:
		ppi = TUA_PPI;
		goto do_rc;
	      do_rc:
		iid_val = 0;
		iid = NULL;
		num_iid = 0;
		rc_val = htonl(as->rc);
		rc = &rc_val;
		num_rc = 1;
		break;
	}

	mp = ua_build_aspt_aspia_ack(q, ppi, iid, num_iid, rc, num_rc, inf_ptr, inf_len);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_send_rkmm_reg_req: - send REG REQ message (UA_RKMM_REG_REQ)
 * @ppi: protocol payload identifier
 * @q: active queue
 * @id: request identifier
 * @sdti: signalling data terminal identifier
 * @sdli: signalling data link identifier
 *
 * FIXME: This is specific to M2UA.
 */
STATIC INLINE int
m2ua_build_rkmm_reg_req(queue_t *q, uint32_t ppi, struct ua_key *key, size_t num_keys)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_LINK_KEY);

	if (num_keys != 1) {
		swerr();
		return (-EFAULT);
	}

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_RKMM_REG_REQ;
			p[1] = __constant_htonl(mlen);
			p[2] = M2UA_PARM_LINK_KEY;
			p[3] = M2UA_PARM_LOC_KEY_ID;
			p[4] = htonl(key->id);
			p[5] = M2UA_PARM_SDTI;
			p[6] = htonl(key->m2ua.sdti);
			p[7] = M2UA_PARM_SDLI;
			p[8] = htonl(key->m2ua.sdli);
			mp->b_wptr = (unsigned char *) &p[9];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE mblk_t *
m3ua_build_rkmm_reg_req(queue_t *q, uint32_t ppi, struct ua_key *key, size_t num_keys)
{
	mblk_t *mp;
	int i, j;
	size_t klen[num_keys] = { 0, };
	size_t mlen = UA_HDR_SIZE;

	for (i = 0; i < num_keys; i++) {
		klen[i] = UA_PHDR_SIZE + (key[i].m3ua.rc == -1) ? 0 : UA_SIZE(UA_PARM_RC)
		    + (key[i].m3ua.tmode == -1) ? 0 : UA_SIZE(UA_PARM_TMODE)
		    + (key[i].m3ua.na == -1) ? 0 : UA_SIZE(M3UA_PARM_NTWK_APP)
		    + (key[i].m3ua.si_num) ? UA_PHDR_SIZE + UA_PAD4(key[i].m3ua.si_num) : 0
		    + (key[i].m3ua.opc_num) ? UA_PHDR_SIZE + (key[i].m3ua.opc_num << 2) : 0
		    + UA_SIZE(M3UA_PARM_DPC);
		mlen += klen[i];
	}

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_RKMM_REG_REQ;
			p[1] = htonl(mlen);
			p += 2;

			for (i = 0; i < num_keys; i++) {
				p[0] = UA_PHDR(M3UA_PARM_ROUTING_KEY, klen[i]);
				p[1] = M3UA_PARM_LOC_KEY_ID;
				p[2] = htonl(key->id);
				p += 3;
				if (key[i].m3ua.rc != -1) {
					p[0] = UA_PARM_RC;
					p[1] = htonl(key[i].m3ua.rc);
					p += 2;
				}
				if (key[i].m3ua.tmode != -1) {
					p[0] = UA_PARM_TMODE;
					p[1] = htonl(key[i].m3ua.tmod);
					p += 2;
				}
				if (key[i].m3ua.na != -1) {
					p[0] = M3UA_PARM_NTWK_APP;
					p[1] = htonl(key[i].m3ua.na);
					p += 2;
				}
				p[0] = M3UA_PARM_DPC;
				p[1] = htonl(key[i].m3ua.dpc);
				p += 2;
				if (key[i].m3ua.si_num) {
					*p++ = UA_PHDR(M3UA_PARM_SI, key[i].m3ua.si_num);
					for (j = 0; j < (key[i].m3ua.si_num + 3) >> 2; j++)
						*p++ = htonl(key[i].m3ua.si[j]);
				}
				if (key[i].m3ua.opc_num) {
					*p++ =
					    UA_PHDR(M3UA_PARM_OPC_LIST, key[i].m3ua.opc_num << 2);
					for (j = 0; j < key[i].m3ua.opc_num; j++)
						*p++ = htonl(key[i].m3ua.opc[j]);
				}
			}
			mp->b_wptr = (unsigned char *) p;
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE mblk_t *
sua_build_rkmm_reg_req(queue_t *q, uint32_t ppi, struct ua_key *key, size_t num_keys)
{
	mblk_t *mp;
	int i, j;
	size_t klen[num_keys] = { 0, };
	size_t mlen = UA_HDR_SIZE;

	for (i = 0; i < num_keys; i++) {
		klen[i] = UA_PHDR_SIZE + 0;
		mlen += klen[i];
	}
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_RKMM_REG_REQ;
			p[1] = htonl(mlen);
			p += 2;

			/* FIXME: build up routing key */

			mp->b_wptr = (unsigned char *) p;
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE mblk_t *
tua_build_rkmm_reg_req(queue_t *q, uint32_t ppi, struct ua_key *key, size_t num_keys)
{
	mblk_t *mp;
	int i, j;
	size_t klen[num_keys] = { 0, };
	size_t mlen = UA_HDR_SIZE;

	for (i = 0; i < num_keys; i++) {
		klen[i] = UA_PHDR_SIZE + 0;
		mlen += klen[i];
	}
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_RKMM_REG_REQ;
			p[1] = htonl(mlen);
			p += 2;

			/* FIXME: build up routing key */

			mp->b_wptr = (unsigned char *) p;
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

STATIC INLINE mblk_t *
ua_build_rkmm_reg_req(queue_t *q, uint32_t ppi, struct ua_key *key, size_t num_keys)
{
	switch (ppi) {
	default:
		never();
		return (-EFAULT);
	case M2UA_PPI:
		return m2ua_build_rkmm_reg_req(q, ppi, key, num_keys);
	case M3UA_PPI:
		return m3ua_build_rkmm_reg_req(q, ppi, key, num_keys);
	case SUA_PPI:
		return sua_build_rkmm_reg_req(q, ppi, key, num_keys);
	case TUA_PPI:
		return tua_build_rkmm_reg_req(q, ppi, key, num_keys);
	}
}

STATIC INLINE int
ua_send_rkmm_reg_req(struct xp *xp, queue_t *q, struct ua_key *key, size_t num_keys)
{
	mblk_t *mp;

	mp = ua_build_rkmm_reg_req(q, xp->ppi, key, num_keys);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_rkmm_reg_rsp: - build REG RESP message (UA_RKMM_REG_RSP)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @id: request identifier
 * @status: request status
 * @iid: interface id
 * @rc: routing context
 *
 * FIXME: This is still M2UA specific.
 */
STATIC INLINE mblk_t *
ua_build_rkmm_reg_rsp(queue_t *q, uint32_t ppi, uint32_t id, uint32_t status, uint32_t *iid,
		      uint32_t *rc)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_REG_RESULT);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_RKMM_REG_RSP;
			p[1] = __constant_htonl(mlen);
			p[2] = M2UA_PARM_REG_RESULT;
			p[3] = M2UA_PARM_LOC_KEY_ID;
			p[4] = htonl(id);
			p[5] = M2UA_PARM_REG_STATUS;
			p[6] = htonl(status);
			p += 7;
			if (iid) {
				p[0] = UA_PARM_IID;
				p[1] = *iid;	/* already network byte order */
				p += 2;
			}
			if (rc) {
				p[0] = UA_PARM_RC;
				p[1] = *rc;	/* already network byte order */
				p += 2;
			}
			mp->b_wptr = (unsigned char *) p;
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_rkmm_reg_rsp: - send REG RESP message (UA_RKMM_REG_RSP)
 * @xp: transport on which to send
 * @q: active queue
 * @id: request identifier
 * @status: request status
 * @iid: interface id
 * @rc: routing context
 *
 * FIXME: This is still M2UA specific.
 */
STATIC INLINE int
ua_send_rkmm_reg_rsp(struct xp *xp, queue_t *q, uint32_t id, uint32_t status, uint32_t *iid,
		     uint32_t *rc)
{
	mblk_t *mp;

	mp = ua_build_rkmm_reg_rsp(q, xp->ppi, id, status, iid, rc);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_rkmm_dereg_req: - build DEREG REQ message (UA_RKMM_DEREG_REQ)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @iid: interface identifier
 * @rc: routing context
 */
STATIC INLINE mblk_t *
ua_build_rkmm_dereg_req(queue_t *q, uint32_t ppi, uint32_t *iid, uint32_t *rc)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_RKMM_DEREG_REQ;
			p[1] = __constant_htonl(mlen);
			p += 2;
			if (iid) {
				p[0] = UA_PARM_IID;
				p[1] = *iid;	/* already network byte order */
				p += 2;
			}
			if (rc) {
				p[0] = UA_PARM_RC;
				p[1] = *rc;	/* already network byte order */
				p += 2;
			}
			mp->b_wptr = (unsigned char *) p;
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_rkmm_dereg_req: - send DEREG REQ message (UA_RKMM_DEREG_REQ)
 * @xp: transport on which to send
 * @q: active queue
 * @iid: interface identifier
 * @rc: routing context
 */
STATIC INLINE int
ua_send_rkmm_dereg_req(struct xp *xp, queue_t *q, uint32_t *iid, uint32_t *rc)
{
	mblk_t *mp;

	mp = ua_build_rkmm_dereg_req(q, xp->ppi, iid, rc);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_rkmm_dereg_rsp: - build DEREG RESP message (UA_RKMM_DEREG_RSP)
 * @q: active queue
 * @ppi: protocol payload identifier
 * @num_rciid: number of routing contexts or interface identifiers and results
 * @rciid: routing context or interface identifiers
 * @status: deregistration status
 */
STATIC INLINE mblk_t *
ua_build_rkmm_dereg_rsp(queue_t *q, uint32_t ppi, size_t num_rciid, uint32_t *rciid,
			uint32_t *status)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + num_rciid * UA_SIZE(UA_PARM_DEREG_RESULT);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, 0))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;
			int i;

			p[0] = UA_RKMM_DEREG_RSP;
			p[1] = __constant_htonl(mlen);
			p += 2;
			for (i = 0; i < num_rciid; i++) {
				switch (ppi) {
				case M2UA_PPI:
					p[0] = M2UA_PARM_DEREG_RESULT;
					p[1] = UA_PARM_IID;
					p[2] = rciid[i];	/* already network byte order */
					p[3] = M2UA_PARM_DEREG_STATUS;
					p[4] = status[i];	/* already network byte order */
					p += 5;
					break;
				case M3UA_PPI:
					p[0] = M3UA_PARM_DEREG_RESULT;
					p[1] = UA_PARM_RC;
					p[2] = rciid[i];	/* already network byte order */
					p[3] = M3UA_PARM_DEREG_STATUS;
					p[4] = status[i];	/* already network byte order */
					p += 5;
					break;
				default:
					swerr();
				case SUA_PPI:
				case TUA_PPI:
					p[0] = UA_PARM_DEREG_RESULT;
					p[1] = UA_PARM_RC;
					p[2] = rciid[i];	/* already network byte order */
					p[3] = UA_PARM_DEREG_STATUS;
					p[4] = status[i];	/* already network byte order */
					p += 5;
					break;
				}
			}
			mp->b_wptr = (unsigned char *) p;
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_rkmm_dereg_rsp: - send DEREG RESP message (UA_RKMM_DEREG_RSP)
 * @spp: SPP for which to send
 * @q: active queue
 * @num_rciid: number of rc or iid and results
 * @rciid: list of rc or iid
 * @status: deregistration statuses
 */
STATIC INLINE int
ua_send_rkmm_dereg_rsp(struct spp *spp, queue_t *q, size_t num_rciid, uint32_t *rciid,
		       uint32_t *status)
{
	mblk_t *mp;
	struct xp *xp = spp->xp;

	mp = ua_build_rkmm_dereg_rsp(q, xp->ppi, num_rciid, rcidd, status);

	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  Send SNMM Messages for M3UA, SUA or TUA.
 *
 *  --------------------------------------------------------------------------
 */
/**
 * ua_build_snmm_duna: - build UA_SNMM_DUNA message
 * @q: active queue
 * @ppi: protocol payload identifier
 * @rc: routing context
 * @apc: affected point codes (if non-NULL) (network byte order)
 * @num_apc: number of affected point codes
 * @ssn: subsystem number (network byte order)
 * @smi: subsystem multiplicity indicator (network byte order)
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_build_snmm_duna(queue_t *q, uint32_t ppi, uint32_t rc, uint32_t *apc, size_t num_apc,
		   uint32_t *ssn, uint32_t *smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + (ssn ? UA_PHDR_SIZE + sizeof(*ssn) : 0)
	    + (smi ? UA_PHDR_SIZE + sizeof(*smi) : 0)
	    + (num_apc ? UA_PHDR_SIZE + num_apc * sizeof(*apc) : 0)
	    + (inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, rc))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_SNMM_DUNA;
			p[1] = htonl(mlen);
			p[2] = UA_PARM_RC;
			p[3] = htonl(rc);
			p += 4;

			if (num_apc) {
				uint32_t *apcp = apc;

				*p++ = UA_PHDR(UA_PARM_APC, num_apc * sizeof(*apc));
				while (num_apc--)
					*p++ = *apcp++;	/* already network byte order */
			}
			if (ssn) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					p[0] = SUA_SPARM_SSN;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_SSN;
					break;
				}
				p[1] = *ssn;	/* already network byte order */
				p += 2;
			}
			if (smi) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					p[0] = SUA_PARM_SMI;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_SMI;
					break;
				}
				p[1] = *smi;	/* already network byte order */
				p += 2;
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_snmm_duna: - send UA_SNMM_DUNA message
 * @rp: AS/SPP/XP relation for which to send
 * @q: active queue
 * @apc: affected point codes (if non-NULL) (network byte order)
 * @num_apc: number of affected point codes
 * @ssn: subsystem number (network byte order)
 * @smi: subsystem multiplicity indicator (network byte order)
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_snmm_duna(struct rp *rp, queue_t *q, uint32_t *apc, size_t num_apc,
		  uint32_t *ssn, uint32_t *smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct gp *gp = rp->gp.gp;
	struct xp *xp = rp->pp.pp->xp.xp;

	mp = ua_build_snmm_duna(q, xp->ppi, gp->as.as->rc, apc, num_apc, ssn, smi, inf_ptr,
				inf_len);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_snmm_dava: - build UA_SNMM_DAVA message
 * @q: active queue
 * @ppi: protocol payload identifier
 * @rc: routing context
 * @apc: affected point code (if non-NULL) (network byte order)
 * @num_apc: number of affected point codes
 * @ssn: subsystem number (network byte order)
 * @smi: subsystem multiplicity indicator (network byte order)
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_build_snmm_dava(queue_t *q, uint32_t ppi, uint32_t rc, uint32_t *apc, size_t num_apc,
		   uint32_t *ssn, uint32_t *smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + (ssn ? UA_PHDR_SIZE + sizeof(*ssn) : 0)
	    + (smi ? UA_PHDR_SIZE + sizeof(*smi) : 0)
	    + (num_apc ? UA_PHDR_SIZE + num_apc * sizeof(*apc) : 0)
	    + (inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, rc))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_SNMM_DAVA;
			p[1] = htonl(mlen);
			p[2] = UA_PARM_RC;
			p[3] = htonl(rc);
			p += 4;

			if (num_apc) {
				uint32_t *apcp = apc;

				*p++ = UA_PHDR(UA_PARM_APC, num_apc * sizeof(*apc));
				while (num_apc--)
					*p++ = *apcp++;	/* already network byte order */
			}
			if (ssn) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					p[0] = SUA_SPARM_SSN;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_SSN;
					break;
				}
				p[1] = *ssn;	/* already network byte order */
				p += 2;
			}
			if (smi) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					p[0] = SUA_PARM_SMI;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_SMI;
					break;
				}
				p[1] = *smi;	/* already network byte order */
				p += 2;
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_snmm_dava: - send UA_SNMM_DAVA message
 * @rp: AS/SPP/XP relation for which to send
 * @q: active queue
 * @apc: affected point code (if non-NULL) (network byte order)
 * @num_apc: number of affected point codes
 * @ssn: subsystem number (network byte order)
 * @smi: subsystem multiplicity indicator (network byte order)
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
STATIC INLINE int
ua_send_snmm_dava(struct rp *rp, queue_t *q, uint32_t *apc, size_t num_apc, uint32_t *ssn,
		  uint32_t *smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct gp *gp = rp->gp.gp;
	struct xp *xp = rp->pp.pp->xp.xp;

	mp = ua_build_snmm_dava(q, xp->ppi, gp->as.as->rc, apc, num_apc, ssn, smi, inf_ptr,
				inf_len);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

STATIC INLINE int
ua_build_snmm_daud(queue_t *q, uint32_t ppi, uint32_t rc, uint32_t *apc, size_t num_apc,
		   uint32_t *ssn, uint32_t *cause, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + (ssn ? UA_PHDR_SIZE + sizeof(*ssn) : 0)
	    + (cause ? UA_PHDR_SIZE + sizeof(*cause) : 0)
	    + (num_apc ? UA_PHDR_SIZE + num_apc * sizeof(*apc) : 0)
	    + (inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, rc))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_SNMM_DAUD;
			p[1] = htonl(mlen);
			p[2] = UA_PARM_RC;
			p[3] = htonl(rc);
			p += 4;

			if (num_apc) {
				uint32_t *apcp = apc;

				*p++ = UA_PHDR(UA_PARM_APC, num_apc * sizeof(*apc));
				while (num_apc--)
					*p++ = *apcp++;	/* already network byte order */
			}
			if (ssn) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					p[0] = SUA_SPARM_SSN;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_SSN;
					break;
				}
				p[1] = *ssn;	/* already network byte order */
				p += 2;
			}
			if (cause) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					/* I really don't know why this parameter was included as
					   optional in RFC 3868. */
					p[0] = SUA_PARM_CAUSE;
					break;
				}
				p[1] = *cause;	/* already network byte order */
				p += 2;
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

STATIC INLINE int
ua_send_snmm_daud(struct rp *rp, queue_t *q, uint32_t *apc, size_t num_apc, uint32_t *ssn,
		  uint32_t *cause, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct gp *gp = rp->gp.gp;
	struct xp *xp = rp->pp.pp->xp.xp;

	mp = ua_build_snmm_daud(q, xp->ppi, gp->as.as->rc, apc, num_apc, ssn, cause, inf_ptr,
				inf_len);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/**
 * ua_build_snmm_scon: - build UA_SNMM_SCON message
 * @q: active queue
 * @ppi: protocol payload identifier
 * @rc: routing context
 * @cdest: concerned destination (if non-NULL) (network byte order)
 * @cong: congestion status (if non-NULL) (network byte order)
 * @apc: affected point codes (if non-NULL) (network byte order)
 * @num_apc: number of affected point codes
 * @ssn: subsystem number (if non-NULL) (network byte order)
 * @smi: subsystem multiplicity indicator (if non-NULL) (network byte order)
 * @inf_ptr: INFO pointer
 * @inf_len : INFO length
 */
STATIC INLINE int
ua_build_snmm_scon(queue_t *q, uint32_t ppi, uint32_t rc, uint32_t *cdest, uint32_t *cong,
		   uint32_t *apc, size_t num_apc, uint32_t *ssn, uint32_t *smi, caddr_t inf_ptr,
		   size_t inf_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + (cdest ? UA_SIZE(M3UA_PARM_CONCERN_DEST) : 0)
	    + (cong ? UA_PDHR_SIZE + sizeof(*cong) : 0)
	    + (ssn ? UA_PHDR_SIZE + sizeof(*ssn) : 0)
	    + (smi ? UA_PHDR_SIZE + sizeof(*smi) : 0)
	    + (num_apc ? UA_PHDR_SIZE + num_apc * sizeof(*apc) : 0)
	    + (inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, rc))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_SNMM_SCON;
			p[1] = htonl(mlen);
			p[2] = UA_PARM_RC;
			p[3] = htonl(rc);
			p += 4;

			if (num_apc) {
				uint32_t *apcp = apc;

				*p++ = UA_PHDR(UA_PARM_APC, num_apc * sizeof(*apc));
				while (num_apc--)
					*p++ = *apcp++;	/* already network byte order */
			}
			if (cdest) {
				p[0] = M3UA_PARM_CONCERN_DEST;
				p[1] = *cdest;	/* already network byte order */
				p += 2;
			}
			if (ssn) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					p[0] = SUA_SPARM_SSN;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_SSN;
					break;
				}
				p[1] = *ssn;	/* already network byte order */
				p += 2;
			}
			if (cong) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case M3UA_PPI:
					p[0] = M3UA_PARM_CONG_IND;
					break;
				case SUA_PPI:
					p[0] = SUA_PARM_CONG;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_CONG;
					break;
				}
				p[1] = *cong;	/* already network byte order */
				p += 2;
			}
			if (smi) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					p[0] = SUA_PARM_SMI;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_SMI;
					break;
				}
				p[1] = *smi;	/* already network byte order */
				p += 2;
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * ua_send_snmm_scon: - send UA_SNMM_SCON message
 * @rp: AS/SPP/XP relation for which to send
 * @q: active queue
 * @cdest: concerned destination (if non-NULL) (network byte order)
 * @cong: congestion status (if non-NULL) (network byte order)
 * @apc: affected point codes (if non-NULL) (network byte order)
 * @num_apc: number of affected point codes
 * @ssn: subsystem number (if non-NULL) (network byte order)
 * @smi: subsystem multiplicity indicator (if non-NULL) (network byte order)
 * @inf_ptr: INFO pointer
 * @inf_len : INFO length
 */
STATIC INLINE int
ua_send_snmm_scon(struct rp *rp, queue_t *q, uint32_t *cdest, uint32_t *cong,
		  uint32_t *apc, size_t num_apc, uint32_t *ssn, uint32_t *smi, caddr_t inf_ptr,
		  size_t inf_len)
{
	mblk_t *mp;
	struct gp *gp = rp->gp.gp;
	struct xp *xp = rp->pp.pp->xp.xp;

	mp = ua_build_snmm_scon(q, xp->ppi, gp->as.as->rc, cdest, cong, apc, num_apc, ssn, smi,
				inf_ptr, inf_len);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

STATIC INLINE mblk_t *
ua_build_snmm_dupu(queue_t *q, uint32_t ppi, uint32_t rc, uint16_t cause, uint16_t user,
		   uint32_t *apc, size_t num_apc, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + UA_PHDR_SIZE + sizeof(cause) + sizeof(user)
	    + (num_apc ? UA_PHDR_SIZE + num_apc * sizeof(*apc) : 0)
	    + (inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, rc))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_SNMM_DUPU;
			p[1] = htonl(mlen);
			p[2] = UA_PARM_RC;
			p[3] = htonl(rc);
			p += 4;

			if (num_apc) {
				uint32_t *apcp = apc;

				*p++ = UA_PHDR(UA_PARM_APC, num_apc * sizeof(*apc));
				while (num_apc--)
					*p++ = *apcp++;	/* already network byte order */
			}

			/* Thank RFC authors that cannot get together for this switch statement. */
			switch (ppi) {
			default:
				swerr();
			case M3UA_PPI:
				p[0] = M3UA_PARM_USER_CAUSE;
				break;
			case SUA_PPI:
				p[0] = SUA_PARM_CAUSE;
				break;
			case TUA_PPI:
				p[0] = TUA_PARM_CAUSE;
				break;
			}
			p[1] = htonl(((uint32_t) cause << 16) | (uint32_t) user);
			p += 2;

			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
ua_send_snmm_dupu(struct rp *rp, queue_t *q, uint16_t cause, uint16_t user, uint32_t *apc,
		  size_t num_apc, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct gp *gp = rp->gp.gp;
	struct xp *xp = rp->pp.pp->xp.xp;

	mp = ua_build_snmm_scon(q, xp->ppi, gp->as.as->rc, cause, user, apc, num_apc, inf_ptr,
				inf_len);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

STATIC INLINE mblk_t *
ua_build_snmm_drst(queue_t *q, uint32_t ppi, uint32_t rc, uint32_t *apc, size_t num_apc,
		   uint32_t *ssn, uint32_t *smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC)
	    + (ssn ? UA_PHDR_SIZE + sizeof(*ssn) : 0)
	    + (smi ? UA_PHDR_SIZE + sizeof(*smi) : 0)
	    + (num_apc ? UA_PHDR_SIZE + num_apc * sizeof(*apc) : 0)
	    + (inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, T_ODF_EX, ppi, rc))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_SNMM_DRST;
			p[1] = htonl(mlen);
			p[2] = UA_PARM_RC;
			p[3] = htonl(rc);
			p += 4;

			if (num_apc) {
				uint32_t *apcp = apc;

				*p++ = UA_PHDR(UA_PARM_APC, num_apc * sizeof(*apc));
				while (num_apc--)
					*p++ = *apcp++;	/* already network byte order */
			}
			if (ssn) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					p[0] = SUA_SPARM_SSN;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_SSN;
					break;
				}
				p[1] = *ssn;	/* already network byte order */
				p += 2;
			}
			if (smi) {
				/* Thank RFC authors that cannot get together for this switch
				   statement. */
				switch (ppi) {
				default:
					swerr();
				case SUA_PPI:
					p[0] = SUA_PARM_SMI;
					break;
				case TUA_PPI:
					p[0] = TUA_PARM_SMI;
					break;
				}
				p[1] = *smi;	/* already network byte order */
				p += 2;
			}
			if (inf_len) {
				*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
				bcopy(inf_ptr, p, inf_len);
			}
			mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

STATIC INLINE int
ua_send_snmm_drst(struct rp *rp, queue_t *q, uint32_t *apc, size_t num_apc, uint32_t *ssn,
		  uint32_t *smi, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	struct gp *gp = rp->gp.gp;
	struct xp *xp = rp->pp.pp->xp.xp;

	mp = ua_build_snmm_drst(q, xp->ppi, gp->as.as->rc, apc, num_apc, ssn, smi, inf_ptr,
				inf_len);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_DATA
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_data1(queue_t *q, uint32_t iid, mblk_t *bp)
{
	mblk_t *mp;
	size_t dlen = msgdsize(bp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_DATA;
			p[1] = htonl(mlen + dlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = UA_PHDR(M2UA_PARM_DATA1, dlen);
			mp->b_wptr = (unsigned char *) &p[5];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_data1(struct gp *gp, queue_t *q, mblk_t *bp)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_data1(q, gp->as.as->iid, bp);
	return ua_send_optdata_req(xp->oq, mp, bp);
}
STATIC INLINE mblk_t *
m2ua_build_maup_data2(queue_t *q, uint32_t iid, mblk_t *bp)
{
	mblk_t *mp;
	size_t dlen = msgdsize(bp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_DATA;
			p[1] = htonl(mlen + dlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = UA_PHDR(M2UA_PARM_DATA2, dlen);
			mp->b_wptr = (unsigned char *) &p[5];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_data2(struct gp *gp, queue_t *q, mblk_t *bp)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_data2(q, gp->as.as->iid, bp);
	return ua_send_optdata_req(xp->oq, mp, bp);
}

/*
 *  M2UA_MAUP_ESTAB_REQ
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_estab_req(queue_t *q, uint32_t iid)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_ESTAB_REQ;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			mp->b_wptr = (unsigned char *) &p[4];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_estab_req(struct gp *gp, queue_t *q)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_estab_req(q, gp->as.as->iid);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_ESTAB_CON
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_build_maup_estab_con(queue_t *q, uint32_t iid)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_ESTAB_CON;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			mp->b_wptr = (unsigned char *) &p[4];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_estab_con(struct gp *gp, queue_t *q)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_estab_con(q, gp->as.as->iid);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_REL_REQ
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_rel_req(queue_t *q, uint32_t iid)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_REL_REQ;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			mp->b_wptr = (unsigned char *) &p[4];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_rel_req(struct gp *gp, queue_t *q)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_rel_req(q, gp->as.as->iid);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_REL_CON
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_rel_con(queue_t *q, uint32_t iid)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_REL_CON;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			mp->b_wptr = (unsigned char *) &p[4];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_rel_con(struct gp *gp, queue_t *q)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_rel_con(q, gp->as.as->iid);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_REL_IND
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_rel_ind(queue_t *q, uint32_t iid)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_REL_IND;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			mp->b_wptr = (unsigned char *) &p[4];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_rel_ind(struct gp *gp, queue_t *q)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_rel_ind(q, gp->as.as->iid);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_STATE_REQ
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_state_req(queue_t *q, uint32_t iid, const uint32_t request)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_STATE_REQUEST);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_STATE_REQ;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = M2UA_PARM_STATE_REQUEST;
			p[5] = request;	/* already network byte order */
			mp->b_wptr = (unsigned char *) &p[6];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_state_req(struct gp *gp, queue_t *q, const uint32_t request)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_state_req(q, gp->as.as->iid, request);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_STATE_CON
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_state_con(queue_t *q, uint32_t iid, const uint32_t request)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_STATE_REQUEST);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_STATE_CON;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = M2UA_PARM_STATE_REQUEST;
			p[5] = request;	/* already network byte order */
			mp->b_wptr = (unsigned char *) &p[6];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_state_con(struct gp *gp, queue_t *q, const uint32_t request)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_state_con(q, gp->as.as->iid, request);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}
STATIC INLINE mblk_t *
m2ua_build_maup_con(queue_t *q, uint32_t iid, struct ua_msg *m)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_STATE_REQUEST);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_STATE_CON;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = M2UA_PARM_STATE_REQUEST;
			p[5] = htonl(m->status.val);
			mp->b_wptr = (unsigned char *) &p[6];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_con(struct gp *gp, queue_t *q, struct ua_msg *m)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_con(q, gp->as.as->iid, m);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_STATE_IND
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_state_ind(queue_t *q, uint32_t iid, const uint32_t event)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_STATE_EVENT);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_STATE_IND;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = M2UA_PARM_STATE_EVENT;
			p[5] = event;	/* already network byte order */
			mp->b_wptr = (unsigned char *) &p[6];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_state_ind(struct gp *gp, queue_t *q, const uint32_t event)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_state_ind(q, gp->as.as->iid, event);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_RETR_REQ
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_retr_req(queue_t *q, uint32_t iid, uint32_t *fsnc)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID)
	    + UA_SIZE(M2UA_PARM_ACTION)
	    + fsnc ? UA_SIZE(M2UA_PARM_SEQNO) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[1] = M2UA_MAUP_RETR_REQ;
			p[2] = __constant_htonl(mlen);
			p[3] = UA_PARM_IID;
			p[4] = htonl(iid);
			p[5] = M2UA_PARM_ACTION;

			if (fsnc) {
				p[6] = __constant_htonl(M2UA_ACTION_RTRV_MSGS);
				p[7] = M2UA_PARM_SEQNO;
				p[8] = *fsnc;	/* already network byte order */
				mp->b_wptr = (unsigned char *) &p[9];
			} else {
				p[6] = __constant_htonl(M2UA_ACTION_RTRV_BSN);
				mp->b_wptr = (unsigned char *) &p[7];
			}
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_retr_req(struct gp *gp, queue_t *q, uint32_t *fsnc)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_retr_req(q, gp->as.as->iid, fsnc);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_RETR_CON
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_retr_con(queue_t *q, uint32_t iid, uint32_t action, uint32_t result, uint32_t *bsnt)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID)
	    + UA_SIZE(M2UA_PARM_ACTION)
	    + UA_SIZE(M2UA_PARM_RETR_RESULT)
	    + bsnt ? UA_SIZE(M2UA_PARM_SEQNO) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_RETR_CON;
			p[1] = htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = M2UA_PARM_ACTION;
			p[5] = action;	/* already network byte order */
			p[6] = M2UA_PARM_RETR_RESULT;
			p[7] = result;	/* already newtork byte order */
			p += 8;
			if (bsnt) {
				p[0] = M2UA_PARM_SEQNO;
				p[1] = *bsnt;	/* already network byte order */
				p += 2;
			}
			mp->b_wptr = (unsigned char *) p;
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_retr_con(struct gp *gp, queue_t *q, uint32_t action, uint32_t result, uint32_t *bsnt)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_retr_con(q, gp->as.as->iid, action, result, bsnt);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_RETR_IND
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_retr_ind1(queue_t *q, uint32_t iid, mblk_t *bp)
{
	mblk_t *mp;
	size_t dlen = msgdsize(bp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_RETR_IND;
			p[1] = htonl(mlen + dlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = UA_PHDR(M2UA_PARM_DATA1, dlen);
			mp->b_wptr = (unsigned char *) &p[5];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_retr_ind1(struct gp *gp, queue_t *q, mblk_t *bp)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_retr_ind(q, gp->as.as->iid, bp);
	return ua_send_optdata_req(xp->oq, mp, bp);
}
STATIC INLINE mblk_t *
m2ua_build_maup_retr_ind2(queue_t *q, uint32_t iid, mblk_t *bp)
{
	mblk_t *mp;
	size_t dlen = msgdsize(bp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_RETR_IND;
			p[1] = htonl(mlen + dlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = UA_PHDR(M2UA_PARM_DATA2, dlen);
			mp->b_wptr = (unsigned char *) &p[5];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_retr_ind2(struct gp *gp, queue_t *q, mblk_t *bp)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_retr_ind2(q, gp->as.as->iid, bp);
	return ua_send_optdata_req(xp->oq, mp, bp);
}

/*
 *  M2UA_MAUP_RETR_COMP_IND
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_retr_comp_ind(queue_t *q, uint32_t iid)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_ACTION) +
	    UA_SIZE(M2UA_PARM_RETR_RESULT);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_RETR_COMP_IND;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = M2UA_PARM_ACTION;
			p[5] = __constant_htonl(M2UA_ACTION_RTRV_MSGS);
			p[6] = M2UA_PARM_RETR_RESULT;
			p[7] = __constant_htonl(UA_RESULT_SUCCESS);
			mp->b_wptr = (unsigned char *) &p[8];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_retr_comp_ind(struct gp *gp, queue_t *q)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_retr_comp_ind(q, gp->as.as->iid);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_CONG_IND
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m2ua_build_maup_cong_ind(queue_t *q, uint32_t iid, uint32_t cong, uint32_t disc)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_CONG_STATUS) +
	    UA_SIZE(M2UA_PARM_DISC_STATUS);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_CONG_IND;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = M2UA_PARM_CONG_STATUS;
			p[5] = htonl(cong);
			p[6] = M2UA_PARM_DISC_STATUS;
			p[7] = htonl(disc);
			mp->b_wptr = (unsigned char *) &p[8];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_cong_ind(struct gp *gp, queue_t *q, uint32_t cong, uint32_t disc)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_build_maup_cong_ind(q, gp->as.as->iid, cong, disc);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M2UA_MAUP_DATA_ACK
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_build_maup_data_ack(queue_t *q, uint32_t iid, struct ua_msg *m)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_CORR_ID_ACK);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M2UA_PPI, iid))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = M2UA_MAUP_DATA_ACK;
			p[1] = __constant_htonl(mlen);
			p[2] = UA_PARM_IID;
			p[3] = htonl(iid);
			p[4] = M2UA_PARM_CORR_ID_ACK;
			p[5] = htonl(m->corid.val);
			mp->b_wptr = (unsigned char *) &p[6];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}
STATIC INLINE int
m2ua_send_maup_data_ack(struct gp *gp, queue_t *q, struct ua_msg *m)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m2ua_send_maup_data_ack(q, gp->as.as->iid, m);
	return ua_send_optdata_req(xp->oq, mp, NULL);
}

/*
 *  M3UA_XFER_DATA
 *  -----------------------------------
 */
STATIC INLINE mblk_t *
m3ua_build_xfer_data(queue_t *q, uint32_t rc, uint32_t opc, uint32_t dpc, uint8_t si, uint8_t ni,
		     uint8_t mp, uint8_t sls, mblk_t *bp)
{
	mblk_t *mp;
	size_t dlen = msgdsize(bp->b_cont) + 3 * sizeof(uint32);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC) + UA_PHDR_SIZE;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		mblk_t *mb;

		if ((mb = ua_build_optdata_req(q, 0, M3UA_PPI, rc))) {
			register uint32_t *p = (typeof(p)) mp->b_wptr;

			p[0] = UA_XFER_DATA;
			p[1] = htonl(mlen + dlen);
			p[2] = UA_PARM_RC;
			p[3] = htonl(rc);
			p[4] = UA_PHDR(M3UA_PARM_PROT_DATA3, dlen);
			p[5] = htonl(opc);
			p[6] = htonl(dpc);
			p[7] = htonl(((uint32_t) si << 24) | ((uint32_t) ni << 16) |
				     ((uint32_t) mp << 8) | ((uint32_t) sls << 0));
			mp->b_wptr = (unsigned char *) &p[8];
			mb->b_cont = mp;
			return (mb);
		}
		freeb(mp);
		return (NULL);
	}
	return (mp);
}

/**
 * m3ua_send_xfer_data: - send UA_XFER_DATA message
 * @gp: SPP/AS relation for which to send
 * @q: active queue
 * @opc: originating point code
 * @dpc: destination point code
 * @si: service indicator
 * @ni: network indicator
 * @mp: message priority
 * @sls: signalling link selection
 * @bp: MTP user data
 */
STATIC INLINE int
m3ua_send_xfer_data(struct gp *gp, queue_t *q, uint32_t opc, uint32_t dpc, uint8_t si, uint8_t ni,
		    uint8_t mp, uint8_t sls, mblk_t *bp)
{
	mblk_t *mp;
	struct xp *xp = gp->spp.spp->xp.xp;

	mp = m3ua_build_xfer_data(q, gp->as.as->rc, opc, dpc, si, ni, mp, sls, bp);
	return ua_send_optdata_req(xp->oq, mp, bp);
}

/*
 *  =========================================================================
 *
 *  STATE MACHINES
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Timers
 *
 *  -------------------------------------------------------------------------
 */
enum { tall, tack, tbeat, tidle, tpend
};

#define UA_DECLARE_TIMER(__o, __t) \
STATIC INLINE int __o ## _ ## __t ## _timeout(__o ## _t *); \
STATIC streamscall void __o ## _ ## __t ## _expiry(caddr_t data) \
{ \
	__o ## _do_timeout(data, # __t, &((__o ## _t *)data)->timers. __t, &__o ## _ ## __t ## _timeout, & __o ## _ ## __t ## _expiry); \
} \
STATIC INLINE void __o ## _stop_timer_ ## __t (__o ## _t *__o) \
{ \
	__o ## _stop_timer(__o, # __t, &__o->timers. __t); \
} \
STATIC INLINE void __o ## _start_timer_ ## __t (__o ## _t *__o, unsigned long val) \
{ \
	__o ## _start_timer(__o, # __t, &__o->timers. __t, &__o ## _ ## __t ## _expiry, val); \
} \

/*
 *  -------------------------------------------------------------------------
 *
 *  PP timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
pp_do_timeout(caddr_t data, const char *timer, toid_t *timeo, int (to_fnc) (struct pp *),
	      streamscall void (*exp_fnc) (caddr_t))
{
	struct pp *pp = (struct pp *) data;

	if (xchg(timeo, 0)) {
		if (spin_trylock(&pp->xp.xp->lock)) {
			printd(("%s: %p: %s: timeout at %lu\n", DRV_NAME, pp, timer, jiffies));
			switch (to_fnc(pp)) {
			default:
			case QR_DONE:
				spin_unlock(&pp->xp.xp->lock);
				xp_put(pp->xp.xp);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&pp->xp.xp->lock);
		} else
			printd(("%s: %p: %s: timeout collision at %lu\n", DRV_NAME, pp, timer,
				jiffies));
		/* back off timer 100 ticks */
		*timeo = timeout(exp_fnc, data, 100);
	}
}
STATIC INLINE void
pp_stop_timer(struct pp *pp, const char *timer, toid_t *timeo)
{
	toid_t to;

	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", DRV_NAME, pp, timer, jiffies));
		xp_put(pp->xp.xp);
	}
	return;
}
STATIC INLINE void
pp_start_timer(struct pp *pp, const char *timer, toid_t *timeo,
	       streamscall void (*exp_fnc) (caddr_t), unsigned long val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", DRV_NAME, pp, timer, val * 1000 / HZ,
		jiffies));
	*timeo = timeout(exp_fnc, (caddr_t) xp_get(pp->xp.xp), val);
}

UA_DECLARE_TIMER(pp, tack);
UA_DECLARE_TIMER(pp, tbeat);
UA_DECLARE_TIMER(pp, tidle);

STATIC INLINE void
__pp_timer_stop(struct pp *pp, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case tack:
		pp_stop_timer_tack(pp);
		if (single)
			break;
		/* fall through */
	case tbeat:
		pp_stop_timer_tbeat(pp);
		if (single)
			break;
		/* fall through */
	case tidle:
		pp_stop_timer_tidle(pp);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
pp_timer_stop(struct pp *pp, const uint t)
{
	spin_lock_ua(&pp->xp.xp->lock);
	{
		__pp_timer_stop(pp, t);
	}
	spin_unlock_ua(&pp->xp.xp->lock);
}
STATIC INLINE void
pp_timer_start(struct pp *pp, const uint t)
{
	spin_lock_ua(&pp->xp.xp->lock);
	{
		__pp_timer_stop(pp, t);
		switch (t) {
		case tack:
			pp_start_timer_tack(pp, pp->spp.spp->sp.sp->config.tack);
			break;
		case tbeat:
			pp_start_timer_tbeat(pp, pp->spp.spp->sp.sp->config.tbeat);
			break;
		case tidle:
			pp_start_timer_tidle(pp, pp->spp.spp->sp.sp->config.tidle);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_ua(&pp->xp.xp->lock);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  AS timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
as_do_timeout(caddr_t data, const char *timer, toid_t *timeo, int (to_fnc) (struct as *),
	      streamscall void (*exp_fnc) (caddr_t))
{
	struct as *as = (struct as *) data;

	if (xchg(timeo, 0)) {
		if (spin_trylock(&as->lock)) {
			printd(("%s: %p: %s: timeout at %lu\n", DRV_NAME, as, timer, jiffies));
			switch (to_fnc(as)) {
			default:
			case QR_DONE:
				spin_unlock(&as->lock);
				as_put(as);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&as->lock);
		} else
			printd(("%s: %p: %s: timeout collision at %lu\n", DRV_NAME, as, timer,
				jiffies));
		/* back off timer 100 ticks */
		*timeo = timeout(exp_fnc, data, 100);
	}
}
STATIC INLINE void
as_stop_timer(struct as *as, const char *timer, toid_t *timeo)
{
	toid_t to;

	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", DRV_NAME, as, timer, jiffies));
		as_put(as);
	}
	return;
}
STATIC INLINE void
as_start_timer(struct as *as, const char *timer, toid_t *timeo,
	       streamscall void (*exp_fnc) (caddr_t), unsigned long val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", DRV_NAME, as, timer, val * 1000 / HZ,
		jiffies));
	*timeo = timeout(exp_fnc, (caddr_t) as_get(as), val);
}

UA_DECLARE_TIMER(as, tack);

STATIC INLINE void
__as_timer_stop(struct as *as, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case tack:
		as_stop_timer_tack(as);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
as_timer_stop(struct as *as, const uint t)
{
	spin_lock_ua(&as->lock);
	{
		__as_timer_stop(as, t);
	}
	spin_unlock_ua(&as->lock);
}
STATIC INLINE void
as_timer_start(struct as *as, const uint t)
{
	spin_lock_ua(&as->lock);
	{
		__as_timer_stop(as, t);
		switch (t) {
		case tack:
			as_start_timer_tack(as, as->sp.sp->config.tack);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_ua(&as->lock);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SP timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
sp_do_timeout(caddr_t data, const char *timer, toid_t *timeo, int (to_fnc) (struct sp *),
	      streamscall void (*exp_fnc) (caddr_t))
{
	struct sp *sp = (struct sp *) data;

	if (xchg(timeo, 0)) {
		if (spin_trylock(&sp->lock)) {
			printd(("%s: %p: %s: timeout at %lu\n", DRV_NAME, sp, timer, jiffies));
			switch (to_fnc(sp)) {
			default:
			case QR_DONE:
				spin_unlock(&sp->lock);
				sp_put(sp);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&sp->lock);
		} else
			printd(("%s: %p: %s: timeout collision at %lu\n", DRV_NAME, sp, timer,
				jiffies));
		/* back off timer 100 ticks */
		*timeo = timeout(exp_fnc, data, 100);
	}
}
STATIC INLINE void
sp_stop_timer(struct sp *sp, const char *timer, toid_t *timeo)
{
	toid_t to;

	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", DRV_NAME, sp, timer, jiffies));
		sp_put(sp);
	}
	return;
}
STATIC INLINE void
sp_start_timer(struct sp *sp, const char *timer, toid_t *timeo,
	       streamscall void (*exp_fnc) (caddr_t), unsigned long val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", DRV_NAME, sp, timer, val * 1000 / HZ,
		jiffies));
	*timeo = timeout(exp_fnc, (caddr_t) sp_get(sp), val);
}

UA_DECLARE_TIMER(sp, tack);
STATIC INLINE void
__sp_timer_stop(struct sp *sp, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case tack:
		sp_stop_timer_tack(sp);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sp_timer_stop(struct sp *sp, const uint t)
{
	spin_lock_ua(&sp->lock);
	{
		__sp_timer_stop(sp, t);
	}
	spin_unlock_ua(&sp->lock);
}
STATIC INLINE void
sp_timer_start(struct sp *sp, const uint t)
{
	spin_lock_ua(&sp->lock);
	{
		__sp_timer_stop(sp, t);
		switch (t) {
		case tack:
			sp_start_timer_tack(sp, sp->config.tack);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_ua(&sp->lock);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
sl_do_timeout(caddr_t data, const char *timer, toid_t *timeo, int (to_fnc) (struct ss *),
	      streamscall void (*exp_fnc) (caddr_t))
{
	struct ss *ss = (struct ss *) data;

	if (xchg(timeo, 0)) {
		if (spin_trylock(&ss->lock)) {
			printd(("%s: %p: %s: timeout at %lu\n", DRV_NAME, ss, timer, jiffies));
			switch (to_fnc(ss)) {
			default:
			case QR_DONE:
				spin_unlock(&ss->lock);
				ss_put(ss);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&ss->lock);
		} else
			printd(("%s: %p: %s: timeout collision at %lu\n", DRV_NAME, ss, timer,
				jiffies));
		/* back off timer 100 ticks */
		*timeo = timeout(exp_fnc, data, 100);
	}
}
STATIC INLINE void
sl_stop_timer(struct ss *ss, const char *timer, toid_t *timeo)
{
	toid_t to;

	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", DRV_NAME, ss, timer, jiffies));
		ss_put(ss);
	}
	return;
}
STATIC INLINE void
sl_start_timer(struct ss *ss, const char *timer, toid_t *timeo,
	       streamscall void (*exp_fnc) (caddr_t), unsigned long val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", DRV_NAME, ss, timer, val * 1000 / HZ,
		jiffies));
	*timeo = timeout(exp_fnc, (caddr_t) ss_get(ss), val);
}

UA_DECLARE_TIMER(ss, tack);
STATIC INLINE void
__sl_timer_stop(struct ss *ss, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case tack:
		sl_stop_timer_tack(ss);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sl_timer_stop(struct ss *ss, const uint t)
{
	spin_lock_ua(&ss->lock);
	{
		__sl_timer_stop(ss, t);
	}
	spin_unlock_ua(&ss->lock);
}
STATIC INLINE void
sl_timer_start(struct ss *ss, const uint t)
{
	spin_lock_ua(&ss->lock);
	{
		__sl_timer_stop(ss, t);
		switch (t) {
		case tack:
			sl_start_timer_tack(ss, ss->config.tack);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_ua(&ss->lock);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  XP timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
xp_do_timeout(caddr_t data, const char *timer, toid_t *timeo, int (to_fnc) (struct xp *),
	      streamscall void (*exp_fnc) (caddr_t))
{
	struct xp *xp = (struct xp *) data;

	if (xchg(timeo, 0)) {
		if (spin_trylock(&xp->lock)) {
			printd(("%s: %p: %s: timeout at %lu\n", DRV_NAME, xp, timer, jiffies));
			switch (to_fnc(xp)) {
			default:
			case QR_DONE:
				spin_unlock(&xp->lock);
				xp_put(xp);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&xp->lock);
		} else
			printd(("%s: %p: %s: timeout collision at %lu\n", DRV_NAME, xp, timer,
				jiffies));
		/* back off timer 100 ticks */
		*timeo = timeout(exp_fnc, data, 100);
	}
}
STATIC INLINE void
xp_stop_timer(struct xp *xp, const char *timer, toid_t *timeo)
{
	toid_t to;

	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", DRV_NAME, xp, timer, jiffies));
		xp_put(xp);
	}
	return;
}
STATIC INLINE void
xp_start_timer(struct xp *xp, const char *timer, toid_t *timeo,
	       streamscall void (*exp_fnc) (caddr_t), unsigned long val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", DRV_NAME, xp, timer, val * 1000 / HZ,
		jiffies));
	*timeo = timeout(exp_fnc, (caddr_t) xp_get(xp), val);
}

UA_DECLARE_TIMER(xp, tack);
STATIC INLINE void
__xp_timer_stop(struct xp *xp, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case tack:
		xp_stop_timer_tack(xp);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
xp_timer_stop(struct xp *xp, const uint t)
{
	spin_lock_ua(&xp->lock);
	{
		__xp_timer_stop(xp, t);
	}
	spin_unlock_ua(&xp->lock);
}
STATIC INLINE void
xp_timer_start(struct xp *xp, const uint t)
{
	spin_lock_ua(&xp->lock);
	{
		__xp_timer_stop(xp, t);
		switch (t) {
		case tack:
			xp_start_timer_tack(xp, xp->config.tack);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_ua(&xp->lock);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  State Changes
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE t_uscalar_t
as_get_state(struct as *as)
{
	return (as->state);
}
STATIC INLINE t_uscalar_t
sp_get_state(struct sp *sp)
{
	return (sp->state);
}
STATIC INLINE t_uscalar_t
spp_get_state(struct spp *spp)
{
	return (spp->state);
}
STATIC INLINE t_uscalar_t
ss_get_state(struct ss *ss)
{
	return (ss->state);
}
STATIC INLINE t_uscalar_t
xp_get_state(struct xp *xp)
{
	return (xp->state);
}
STATIC INLINE t_uscalar_t
ap_get_state(struct ap *ap)
{
	return (ap->state);
}
STATIC INLINE t_uscalar_t
np_get_state(struct np *np)
{
	return (np->state);
}
STATIC INLINE t_uscalar_t
gp_get_state(struct gp *gp)
{
	return (gp->state);
}
STATIC INLINE t_uscalar_t
ss_get_s_state(struct ss *ss)
{
	return (ss->s_state);
}
STATIC INLINE t_uscalar_t
gp_get_s_state(struct gp *gp)
{
	return (gp->s_state);
}

STATIC INLINE void
as_set_state(struct as *as, t_uscalar_t newstate)
{
	as->state = newstate;
}
STATIC INLINE void
sp_set_state(struct sp *sp, t_uscalar_t newstate)
{
	sp->state = newstate;
}
STATIC INLINE void
spp_set_state(struct spp *spp, t_uscalar_t newstate)
{
	spp->state = newstate;
}
STATIC INLINE void
ss_set_state(struct ss *ss, t_uscalar_t newstate)
{
	ss->state = newstate;
}
STATIC INLINE void
xp_set_state(struct xp *xp, t_uscalar_t newstate)
{
	xp->state = newstate;
}
STATIC INLINE void
gp_set_state(struct gp *gp, t_uscalar_t newstate)
{
	gp->state = newstate;
}
STATIC INLINE void
ap_set_state(struct ap *ap, t_uscalar_t newstate)
{
	ap->state = newstate;
}
STATIC INLINE void
np_set_state(struct np *np, t_uscalar_t newstate)
{
	np->state = newstate;
}
STATIC INLINE void
ss_set_s_state(struct ss *ss, t_uscalar_t newstate)
{
	ss->s_state = newstate;
}
STATIC INLINE void
gp_set_s_state(struct gp *gp, t_uscalar_t newstate)
{
	gp->s_state = newstate;
}

STATIC INLINE t_uscalar_t
as_get_flags(struct as *as)
{
	return (as->flags);
}
STATIC INLINE t_uscalar_t
sp_get_flags(struct sp *sp)
{
	return (sp->flags);
}
STATIC INLINE t_uscalar_t
spp_get_flags(struct spp *spp)
{
	return (spp->flags);
}
STATIC INLINE t_uscalar_t
ss_get_flags(struct ss *ss)
{
	return (ss->flags);
}
STATIC INLINE t_uscalar_t
xp_get_flags(struct xp *xp)
{
	return (xp->flags);
}
STATIC INLINE t_uscalar_t
gp_get_flags(struct gp *gp)
{
	return (gp->flags);
}

STATIC INLINE t_uscalar_t
as_tst_flags(struct as *as, t_uscalar_t flags)
{
	return (as->flags & flags);
}
STATIC INLINE t_uscalar_t
sp_tst_flags(struct sp *sp, t_uscalar_t flags)
{
	return (sp->flags & flags);
}
STATIC INLINE t_uscalar_t
spp_tst_flags(struct spp *spp, t_uscalar_t flags)
{
	return (spp->flags & flags);
}
STATIC INLINE t_uscalar_t
ss_tst_flags(struct ss *ss, t_uscalar_t flags)
{
	return (ss->flags & flags);
}
STATIC INLINE t_uscalar_t
xp_tst_flags(struct xp *xp, t_uscalar_t flags)
{
	return (xp->flags & flags);
}
STATIC INLINE t_uscalar_t
gp_tst_flags(struct gp *gp, t_uscalar_t flags)
{
	return (gp->flags & flags);
}

STATIC INLINE void
as_set_flags(struct as *as, t_uscalar_t flags)
{
	as->flags |= flags;
}
STATIC INLINE void
sp_set_flags(struct sp *sp, t_uscalar_t flags)
{
	sp->flags |= flags;
}
STATIC INLINE void
spp_set_flags(struct spp *spp, t_uscalar_t flags)
{
	spp->flags |= flags;
}
STATIC INLINE void
ss_set_flags(struct ss *ss, t_uscalar_t flags)
{
	ss->flags |= flags;
}
STATIC INLINE void
xp_set_flags(struct xp *xp, t_uscalar_t flags)
{
	xp->flags |= flags;
}
STATIC INLINE void
gp_set_flags(struct gp *gp, t_uscalar_t flags)
{
	gp->flags |= flags;
}

STATIC INLINE void
as_clr_flags(struct as *as, t_uscalar_t flags)
{
	as->flags &= ~flags;
}
STATIC INLINE void
sp_clr_flags(struct sp *sp, t_uscalar_t flags)
{
	sp->flags &= ~flags;
}
STATIC INLINE void
spp_clr_flags(struct spp *spp, t_uscalar_t flags)
{
	spp->flags &= ~flags;
}
STATIC INLINE void
ss_clr_flags(struct ss *ss, t_uscalar_t flags)
{
	ss->flags &= ~flags;
}
STATIC INLINE void
xp_clr_flags(struct xp *xp, t_uscalar_t flags)
{
	xp->flags &= ~flags;
}
STATIC INLINE void
gp_clr_flags(struct gp *gp, t_uscalar_t flags)
{
	gp->flags &= ~flags;
}

STATIC INLINE int gp_p_set_state(struct gp *gp, queue_t *q, t_uscalar_t newstate);
STATIC INLINE int gp_u_set_state(struct gp *gp, queue_t *q, t_uscalar_t newstate);

/**
 * asp_as_p_recalc_state: - push AS-P state as a result of AS-U state changes.
 * @as: the AS-P
 * @q: active queue
 *
 * Calculate what actions are to be performed with regard to an AS-P as a result of state changes to
 * an AS-U.  This is accomplished by determining what the overall AS-U state is an then determining
 * if any actions are necessary to alter the AS-P state as follows:
 *
 *  Overall AS-U State	    Current AS-P State		Actions			Next State
 *  ------------------	    ------------------		-------------------	--------------------
 *  AS_DOWN or AS_INACTIVE  AS_DOWN or AS_INACTIVE	No action.		Unchanged.
 *  AS_WACK_ASPAC	    AS_DOWN			No action.		Unchanged.
 *  AS_WACK_ASPAC	    AS_INACTIVE			Activate provider.	AS_WACK_ASPAC
 *  AS_WACK_ASPAC	    AS_WACK_ASPAC		No action.		Unchanged.
 *  AS_WACK_ASPAC	    AS_WACK_ASPIA		Activate provider.	AS_WACK_ASPAC
 *  AS_WACK_ASPAC	    AS_ACTIVE			(Not possible.)
 *
 */
STATIC int
asp_as_p_recalc_state(struct as *as, queue_t *q)
{
	int err;
	struct gp *gp;
	t_uscalar_t state;

	/* Determine the new overall state of the AS-U. */

	for (state = AS_ACTIVE; state && as->ap.counts[state] == 0; state--) ;
	switch (state) {
	case AS_DOWN:
		switch (as_get_state(as)) {
		case AS_DOWN:
		case AS_INACTIVE:
			return (QR_DONE);	/* ignore */
		case AS_WACK_ASPAC:
		case AS_WACK_ASPIA:
		case AS_ACTIVE:
		}
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	case AS_INACTIVE:
		switch (as_get_state(as)) {
		case AS_DOWN:
		case AS_INACTIVE:
			return (QR_DONE);	/* ignore */
		case AS_WACK_ASPAC:
		case AS_WACK_ASPIA:
		case AS_ACTIVE:
		}
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	case AS_WACK_ASPAC:
		switch (as_get_state(as)) {
		case AS_DOWN:
		case AS_INACTIVE:
			return (QR_DONE);	/* ignore */
		case AS_WACK_ASPAC:
		case AS_WACK_ASPIA:
		case AS_ACTIVE:
		}
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	case AS_WACK_ASPIA:
		switch (as_get_state(as)) {
		case AS_DOWN:
		case AS_INACTIVE:
			return (QR_DONE);	/* ignore */
		case AS_WACK_ASPAC:
		case AS_WACK_ASPIA:
		case AS_ACTIVE:
		}
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	case AS_ACTIVE:
		switch (as_get_state(as)) {
		case AS_DOWN:
		case AS_INACTIVE:
			as_set_flags(as, ASF_PENDING);
			return (QR_DONE);	/* ignore */
		case AS_WACK_ASPAC:
		case AS_WACK_ASPIA:
		case AS_ACTIVE:
		}
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	}
	return (QR_DONE);
}
STATIC INLINE int ss_u_set_state(struct ss *, queue_t *, t_uscalar_t);

#if 0
STATIC int
sgp_as_u_recalc_state(struct as *as, queue_t *q)
{
	int err;
	struct gp *gp;
	struct ss *ss;
	t_uscalar_t newstate;

	for (newstate = AS_ACTIVE; newstate && as->ap.counts[newstate] != 0; newstate--) ;
	switch (newstate) {
	case AS_DOWN:
	case AS_INACTIVE:
	case AS_WACK_ASPAC:
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			struct xp *xp;

			switch (gp_get_state(gp)) {
			case AS_WACK_ASPAC:
				if (newstate == AS_WACK_ASPAC)
					continue;
			case AS_WACK_ASPIA:
			case AS_ACTIVE:
				if ((err = ua_send_aspt_aspia_ack(gp, q, NULL, 0)))
					return (err);
				if ((err = gp_u_set_state(gp, q, AS_INACTIVE)))
					return (err);
			}
		}
		for (ss = as->ss.list; ss; ss = ss->as.next) {
			switch (ss_get_state(ss)) {
			case AS_WACK_ASPAC:
				if (newstate == AS_WACK_ASPAC)
					continue;
			case AS_WACK_ASPIA:
				if ((err = sl_disable_con(ss, q)))
					return (err);
				if ((err = ss_u_set_state(ss, q, AS_INACTIVE)))
					return (err);
				break;
			case AS_ACTIVE:
				if ((err = sl_error_ind(ss, q, 0, 0)))
					return (err);
				if ((err = ss_u_set_state(ss, q, AS_INACTIVE)))
					return (err);
			}
		}
		break;
	case AS_ACTIVE:
	case AS_WACK_ASPIA:
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			struct xp *xp;

			switch (gp_get_state(gp)) {
			case AS_WACK_ASPAC:
				if ((err = ua_send_aspt_aspac_ack(gp, q, NULL, 0)))
					return (err);
				if ((err = gp_u_set_state(gp, q, AS_ACTIVE)))
					return (err);
			}
		}
		for (ss = as->ss.list; ss; ss = ss->as.next) {
			switch (ss_get_state(ss)) {
			case AS_WACK_ASPAC:
				if ((err = sl_enable_con(ss, q)))
					return (err);
				if ((err = ss_u_set_state(ss, q, AS_ACTIVE)))
					return (err);
			}
		}
		break;
	}
	return (QR_DONE);
}
#endif

/*
   AS-U propagating ASP state changes to AS-P and ASPs 
 */
STATIC int
asp_as_u_recalc_state(struct as *as, queue_t *q, struct spp *asp)
{
	int err, n = 1;
	struct ap *ap;
	struct gp *gp;
	t_uscalar_t oldstate = as_get_state(as), newstate;
	t_uscalar_t oldflags = as_get_flags(as), newflags = oldflags;

	dassert(as->ap.numb > 0);
	dassert(oldstate <= AS_ACTIVE);

	/* Find new state.  The new state is the maximum state at or above which at least 'n'
	   processes exist (where 'n' is the number required to push a state transition), and then
	   only if we follow this ridiculousness from RFC 4666 -- which we don't and 'n' is always
	   set to one. */
	for (newstate = AS_ACTIVE; newstate && (n -= as->gp.counts[newstate]) > 0; newstate--) ;

	/* propagate state changes to ASP */
	if ((newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA) && as->gp.c.active < as->minasp
	    && as->minasp > 1)
		newflags |= ASF_INSUFFICIENT_ASPS;
	else
		newflags &= ~ASF_INSUFFICIENT_ASPS;
	if ((newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA) && as->gp.c.active == as->minasp
	    && as->minasp > 1)
		newflags |= ASF_MINIMUM_ASPS;
	else
		newflags &= ~ASF_MINIMUM_ASPS;
	if (newstate == AS_WACK_ASPAC || newstate == AS_INACTIVE)
		if (oldstate == AS_WACK_ASPIA || oldstate == AS_ACTIVE)
			newflags |= ASF_PENDING;
	if (newstate == AS_WACK_ASPIA || newstate == AS_ACTIVE)
		newflags &= ~ASF_PENDING;
	for (gp = as->gp.list; gp; gp = gp->as.next) {
		struct rp *rp;

		if (gp_get_state(gp) < AS_INACTIVE)
			continue;

		for (rp = gp->rp.list; rp; rp = rp->gp.next) {
			struct xp *xp = rp->pp.pp->xp.xp;

			if (rp_get_state(rp) < AS_INACTIVE)
				continue;

			/* notify of state change */
			if ((err = ua_send_mgmt_ntfy(rp, q, oldflags, newflags, asp, NULL, 0)))
				return (err);
			break;	/* Only send state notification to primary transport for each ASP. */
		}
	}
	/* No need to propagate state when it has not changed. */
	if (oldstate == newstate)
		return (QR_DONE);
	/* propagate state changes to AS-P */
	for (ap = as->ap.list; ap; ap = ap->u.next) {
		ap->p.as->ap.counts[oldstate]--;
		ap->p.as->ap.counts[newstate]++;
		if ((err = asp_as_p_recalc_state(ap->p.as, q))) {
			ap->p.as->ap.counts[oldstate]++;
			ap->p.as->ap.counts[newstate]--;
			return (err);
		}
	}
	as_set_state(as, newstate);
	as->flags = newflags;
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA)
		as_set_flags(as, ASF_ACTIVE);
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA)
		as_clr_flags(as, ASF_ACTIVE);
	if ((newflags & ASF_PENDING) && !(oldflags & ASF_PENDING))
		as_timer_start(as, tpend);
	if ((oldflags & ASF_PENDING) && !(newflags & ASF_PENDING))
		as_timer_stop(as, tpend);
	return (QR_DONE);
}

STATIC int
sgp_as_p_recalc_state(struct as *as, queue_t *q)
{
	int err;
	struct ap *ap;
	t_uscalar_t newstate, oldstate = as_get_state(as);

	assure(as->ap.numb > 0);
	ensure(oldstate <= AS_ACTIVE, return (-EFAULT));
	for (newstate = AS_ACTIVE; newstate && as->gp.counts[newstate] != 0; newstate--) ;
	if (oldstate != newstate)
		for (ap = as->ap.list; ap; ap = ap->p.next) {
			ap->u.as->ap.counts[oldstate]--;
			ap->u.as->ap.counts[newstate]++;
			if ((err = asp_as_p_recalc_state(ap->u.as, q))) {
				ap->u.as->ap.counts[oldstate]++;
				ap->u.as->ap.counts[newstate]--;
				return (err);
			}
		}
	as_set_state(as, newstate);
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA)
		as_set_flags(as, ASF_ACTIVE);
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA)
		as_clr_flags(as, ASF_ACTIVE);
	return (QR_DONE);
}

static inline int
asp_gp_u_recalc_state(struct gp *gp, queue_t *q, struct spp *asp)
{
	int err;
	struct as *as = gp->as.as;
	t_uscalar_t newstate, oldstate = gp_get_state(gp);

	/* calculate new state */
	for (newstate = AS_ACTIVE; newstate > 0 && gp->rp.counts[newstate] == 0; newstate--) ;
	if (newstate == oldstate)
		return (QR_DONE);
	if (oldstate == AS_DOWN && newstate == AS_INACTIVE)
		/* FEATURE: ASPs which have just come up should be immediately notified of AS
		   state, they can then use the notifications to determine their next best action.
		   This is not required nor recommended by the specification (it is now by RFC
		   4666), but is in general good practice.  By setting oldflags to zero in the
		   following call, the ASP will be notified of the current state of the AS with the 
		   sole exception of when the AS is in the AS-INACTIVE state.  Note also that these 
		   notifications are sent before any associated ASP Up Ack is sent, but not before
		   a REG RESP (because the ASP cannot possibly known the approprirate RC (IID)
		   value until the REG RESP is received). */
		if ((err = ua_send_mgmt_ntfy(gp, q, 0, as_get_flags(as), asp, NULL, 0)))
			return (err);
	as->gp.counts[oldstate]--;
	as->gp.counts[newstate]++;
	if ((err = asp_as_u_recalc_state(as, q, asp))) {
		as->gp.counts[oldstate]++;
		as->gp.counts[newstate]--;
		return (err);
	}
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA) {
		gp_clr_flags(gp, ASF_PENDING);
		gp_set_flags(gp, ASF_ACTIVE);
	}
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA) {
		if (oldstate == AS_ACTIVE || oldstate == AS_WACK_ASPIA)
			gp_set_flags(gp, ASF_PENDING);
		gp_clr_flags(gp, ASF_ACTIVE);
	}
	gp_set_state(gp, newstate);
	return (QR_DONE);
}

static inline int
rp_u_set_state(struct rp *rp, queue_t *q, const t_uscalar_t newstate)
{
	int err;
	struct gp *gp = rp->gp.gp;
	t_uscalar_t oldstate = rp_get_state(rp);

	if (newstate == oldstate)
		return (QR_DONE);
	gp->rp.counts[oldstate]--;
	gp->rp.counts[newstate]++;
	if ((err = asp_gp_u_recalc_state(gp, q, gp->spp.spp))) {
		gp->rp.counts[oldstate]++;
		gp->rp.counts[newstate]--;
		return (err);
	}
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA) {
		rp_clr_flags(rp, ASF_PENDING);
		rp_set_flags(rp, ASF_ACTIVE);
	}
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA) {
		if (oldstate == AS_ACTIVE || oldstate == AS_WACK_ASPIA)
			rp_set_flags(rp, ASF_PENDING);
		rp_clr_flags(rp, ASF_ACTIVE);
	}
	rp_set_state(rp, newstate);
	return (QR_DONE);
}

/*
 *  Set the state of an ASP in an AS-U.
 */
STATIC INLINE int
gp_u_set_state(struct gp *gp, queue_t *q, const t_uscalar_t newstate)
{
	int err;
	struct as *as = gp->as.as;
	t_uscalar_t oldstate = gp_get_state(gp);

	if (newstate == oldstate)
		return (QR_DONE);
	if (oldstate == AS_DOWN && newstate == AS_INACTIVE)
		/* FEATURE: ASPs which have just come up should be immediately notified of AS
		   state, they can then use the notifications to determine their next best action.
		   This is not required nor recommended by the specification (it is now by RFC
		   4666), but is in general good practice.  By setting oldflags to zero in the
		   following call, the ASP will be notified of the current state of the AS with the 
		   sole exception of when the AS is in the AS-INACTIVE state.  Note also that these 
		   notifications are sent before any associated ASP Up Ack is sent, but not before
		   a REG RESP (because the ASP cannot possibly known the approprirate RC (IID)
		   value until the REG RESP is received). */
		if ((err = ua_send_mgmt_ntfy(gp, q, 0, as_get_flags(as), gp->spp.spp, NULL, 0)))
			return (err);

	as->gp.counts[oldstate]--;
	as->gp.counts[newstate]++;
	if ((err = asp_as_u_recalc_state(as, q, gp->spp.spp))) {
		as->gp.counts[oldstate]++;
		as->gp.counts[newstate]--;
		return (err);
	}
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA) {
		gp_clr_flags(gp, ASF_PENDING);
		gp_set_flags(gp, ASF_ACTIVE);
	}
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA) {
		if (oldstate == AS_ACTIVE || oldstate == AS_WACK_ASPIA)
			gp_set_flags(gp, ASF_PENDING);
		gp_clr_flags(gp, ASF_ACTIVE);
	}
	gp_set_state(gp, newstate);
	return (QR_DONE);
}

/*
 *  Set the state of an SL-U in an AS-U.
 */
STATIC INLINE int
ss_u_set_state(struct ss *ss, queue_t *q, const t_uscalar_t newstate)
{
	int err;
	struct as *as = ss->as.as;
	t_uscalar_t oldstate = ss_get_state(ss);

	if (newstate == oldstate)
		return (QR_DONE);
	as->gp.counts[oldstate]--;
	as->gp.counts[newstate]++;
	if ((err = asp_as_u_recalc_state(as, q, NULL))) {
		as->gp.counts[oldstate]++;
		as->gp.counts[newstate]--;
		return (err);
	}
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA)
		ss_set_flags(ss, ASF_ACTIVE);
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA)
		ss_clr_flags(ss, ASF_ACTIVE);
	ss_set_state(ss, newstate);
	return (QR_DONE);
}

/*
 *  Set the state of the SGP in the AS-P
 */
STATIC INLINE int
gp_p_set_state(struct gp *gp, queue_t *q, const t_uscalar_t newstate)
{
	int err;
	struct as *as = gp->as.as;
	t_uscalar_t oldstate = gp_get_state(gp);
	t_uscalar_t oldwack = as->gp.c.wack_aspac + as->gp.c.wack_aspia, newwack;

	if (oldstate == newstate)
		return (QR_DONE);
	as->gp.counts[oldstate]--;
	as->gp.counts[newstate]++;
	newwack = as->gp.c.wack_aspac + as->gp.c.wack_aspia;
	if ((err = sgp_as_p_recalc_state(as, q))) {
		as->gp.counts[oldstate]++;
		as->gp.counts[newstate]--;
		return (err);
	}
	if (oldwack && !newwack)
		as_timer_stop(as, tack);
	if (!oldwack && newwack)
		as_timer_start(as, tack);
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA)
		gp_set_flags(gp, ASF_ACTIVE);
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA)
		gp_clr_flags(gp, ASF_ACTIVE);
	gp_set_state(gp, newstate);
	return (QR_DONE);
}

/*
 *  Set the state of the SL-P in the AS-P
 */
STATIC INLINE int
ss_p_set_state(struct ss *ss, queue_t *q, const t_uscalar_t newstate)
{
	int err;
	struct as *as = ss->as.as;
	t_uscalar_t oldstate = ss_get_state(ss);

	if (oldstate == newstate)
		return (QR_DONE);
	as->gp.counts[oldstate]--;
	as->gp.counts[newstate]++;
	if ((err = sgp_as_p_recalc_state(as, q))) {
		as->gp.counts[oldstate]++;
		as->gp.counts[newstate]--;
		return (err);
	}
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA)
		ss_set_flags(ss, ASF_ACTIVE);
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA)
		ss_clr_flags(ss, ASF_ACTIVE);
	ss_set_state(ss, newstate);
	return (QR_DONE);
}

STATIC INLINE int
asp_pp_set_state(struct pp *pp, queue_t *q, const t_uscalar_t newstate)
{
	int err, state;
	struct rp *rp;
	struct spp *spp = pp->spp.spp;
	t_uscalar_t oldstate = pp_get_state(pp);

	switch (newstate) {
	case ASP_WACK_ASPUP:
	case ASP_UP:
		switch (oldstate) {
		default:
			swerr();
			/* fall through */
		case ASP_DOWN:
		case ASP_WACK_ASPDN:
			spp->pp.counts[oldstate]--;
			spp->pp.counts[ASP_WACK_ASPUP]++;
			if (spp_get_state(spp) < ASP_WACK_ASPUP)
				spp_set_state(spp, ASP_WACK_ASPUP);
			pp_set_state(pp, ASP_WACK_ASPUP);
		case ASP_WACK_ASPUP:
			/* move to the inactive state in all AS, if AS are dynamic and down,
			   attempt to register them. */
			for (rp = pp->rp.list; rp; rp = rp->pp.next)
				if ((err = rp_u_set_state(rp, q, AS_INACTIVE)))
					return (err);
			/* fall through */
		case ASP_UP:
			if ((err = ua_send_asps_aspup_ack(pp, q, NULL, 0)))
				return (err);
			spp->pp.counts[oldstate]--;
			spp->pp.counts[ASP_UP]++;
			if (spp_get_state(spp) < ASP_UP)
				spp_set_state(spp, ASP_UP);
			pp_set_state(pp, ASP_UP);
			return (QR_DONE);
		}
		break;
	case ASP_WACK_ASPDN:
	case ASP_DOWN:
		switch (oldstate) {
		default:
			swerr();
			/* fall through */
		case ASP_UP:
		case ASP_WACK_ASPUP:
			spp->pp.counts[oldstate]--;
			spp->pp.counts[ASP_WACK_ASPDN]++;
			if (spp_get_state(spp) > ASP_WACK_ASPDN)
				for (state = ASP_UP; state > 0 && spp->pp.counts[state] == 0;
				     state--) ;
			pp_set_state(pp, ASP_WACK_ASPDN);
		case ASP_WACK_ASPDN:
			/* move to the down state in all AS */
			for (rp = pp->rp.list; rp; rp = rp->pp.next)
				if ((err = rp_u_set_state(rp, q, AS_DOWN)))
					return (err);
			/* fall through */
		case ASP_DOWN:
			if ((err = ua_send_asps_aspdn_ack(pp, q, NULL, 0)))
				return (err);
			spp->pp.counts[oldstate]--;
			spp->pp.counts[ASP_DOWN]++;
			if (spp_get_state(spp) > ASP_DOWN)
				for (state = ASP_UP; state > 0 && spp->pp.counts[state] == 0;
				     state--) ;
			pp_set_state(pp, ASP_DOWN);
			return (QR_DONE);
		}
		break;
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SGP changing state.
 */
STATIC INLINE int
sgp_spp_set_state(struct spp *spp, queue_t *q, const t_uscalar_t newstate)
{
	int err;
	struct gp *gp;
	t_uscalar_t oldstate = spp_get_state(spp);

	switch (newstate) {
	case ASP_DOWN:
		switch (oldstate) {
		case ASP_WACK_ASPUP:
			pp_timer_stop(pp, tack);
			spp_set_state(spp, newstate);
			/* fall through */
		case ASP_DOWN:
			return (QR_DONE);
		default:
		case ASP_WACK_ASPDN:
			pp_timer_stop(pp, tack);
			/* move to the down state in all AS */
			for (gp = spp->gp.list; gp; gp = gp->spp.next)
				if ((err = gp_p_set_state(gp, q, ASP_DOWN)))
					return (err);
			spp_set_state(spp, newstate);
			return (QR_DONE);
		case ASP_UP:
			/* unsolicited ASPDN Ack */
			/* move to the down state in all AS */
			for (gp = spp->gp.list; gp; gp = gp->spp.next)
				if ((err = gp_p_set_state(gp, q, ASP_DOWN)))
					return (err);
			/* try to bring it back up */
			if ((err = ua_send_asps_aspup_req(spp, q, NULL, 0)))
				return (err);
			pp_timer_start(pp, tack);
			spp_set_state(spp, ASP_WACK_ASPUP);
			return (QR_DONE);
		}
	case ASP_WACK_ASPUP:
		if ((err = ua_send_asps_aspup_req(spp, q, NULL, 0)))
			return (err);
		pp_timer_start(pp, tack);
		spp_set_state(spp, newstate);
		return (QR_DONE);
	case ASP_WACK_ASPDN:
		if ((err = ua_send_asps_aspdn_req(spp, q, NULL, 0)))
			return (err);
		pp_timer_start(pp, tack);
		spp_set_state(spp, newstate);
		return (QR_DONE);
	case ASP_UP:
		switch (oldstate) {
		case ASP_WACK_ASPDN:
			pp_timer_stop(pp, tack);
			spp_set_state(spp, newstate);
			/* fall through */
		case ASP_UP:
			return (QR_DONE);
		default:
		case ASP_WACK_ASPUP:
			pp_timer_stop(pp, tack);
			/* fall through */
		case ASP_DOWN:
			/* move to the inactive state in all AS */
			for (gp = spp->gp.list; gp; gp = gp->spp.next)
				if (gp_tst_flags(gp, ASF_REGSITERED)) {
					if ((err = gp_p_set_state(gp, q, AS_DOWN)))
						return (err);
				} else {
					if ((err = gp_p_set_state(gp, q, AS_INACTIVE)))
						return (err);
				}
			spp_set_state(spp, newstate);
			return (QR_DONE);
		}
	}
	swerr();
	return (-EFAULT);
}

/*
 *  SPP changing state.
 */
static int
spp_spp_set_state(struct spp *spp, queue_t *q, const t_uscalar_t newstate)
{
	/* Do what SGP does. */
	return sgp_spp_set_state(spp, q, newstate);
}

/*
 *  =========================================================================
 *
 *  Events from below
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Timeout Functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  TIMER AS TACK
 *  -----------------------------------
 */
STATIC int
as_tack_timeout(struct as *as)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER SP TACK
 *  -----------------------------------
 */
STATIC int
sp_tack_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER SPP TACK
 *  -----------------------------------
 */
STATIC int
spp_tack_timeout(struct spp *spp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER SPP TBEAT
 *  -----------------------------------
 */
STATIC int
spp_tbeat_timeout(struct spp *spp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER SPP TIDLE
 *  -----------------------------------
 */
STATIC int
spp_tidle_timeout(struct spp *spp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER SL TACK
 *  -----------------------------------
 */
STATIC int
ss_tack_timeout(struct ss *ss)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER XP TACK
 *  -----------------------------------
 */
STATIC int
xp_tack_timeout(struct xp *xp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Functions for finding active ASPs.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * spp_find_gp: - find a SPP-AS graph for an interface id or routing context
 * @spp: SPP for which to find
 * @iids: list of one or more interface ids or routing contexts
 * @iid_len: the length of the interface ids or routing contexts in bytes
 *
 * Finds the SPP-AS relation that corresponds to an interface identifier or
 * routing context.  When multiplex interface identifiers or routing contexts
 * are provided, it returns the SSP-AS relation for the first interface
 * identifier or routing context found.
 *
 * The gp structure represents an SPP-AS relation.  This associates an
 * Application Server (AS) with an Signalling Point Process (SPP).  This
 * function can used appropriately with SGP, ASP and SPP nodes.
 */
STATIC INLINE struct gp *
spp_find_gp(struct spp *spp, uint32_t *iids, size_t iid_len)
{
	int i;
	struct gp *gp;

	iid_len /= sizeof(*iids);
	for (gp = spp->gp.list; gp; gp = gp->spp.next)
		for (i = 0; i < iid_len; i++)
			if (gp->as.as->iid == iids[i])
				return (gp);
	return (gp);
}

/**
 * ap_find_ssu: - find an SS-U node from an AS-U/AS-P relation.
 * @ap: AS-U/AS-P relation
 *
 * Finds and returns the first available local user signalling server for an
 * AS-U/AS-P relation.
 */
STATIC INLINE struct ss *
ap_find_ssu(struct ap *ap)
{
	struct ss *ss;

	/* return first available link */
	for (ss = ap->u.as->ss.list; ss && ss_tst_flags(ss, ASF_ACTIVE | ASF_PENDING);
	     ss = ss->as.next) ;
	return (ss);
}

/**
 * ap_find_ssu_next: - find next SS-U node from an AS-U/AS-P relation.
 * @ap: AS-U/AS-P relation
 * @ss: previousl signalling server node
 * @command: whether command or data is being transferred
 *
 * Finds the next signalling server node for a given AS-U/AS-P relation and the
 * previous node found.
 */
STATIC INLINE struct ss *
ap_find_ssu_next(struct ap *ap, struct ss *ss, const int command)
{
	struct as *as = ap->u.as;

	switch (as->sp.sp->tmode) {
	case UA_TMODE_OVERRIDE:
		return (NULL);
	default:
	case UA_TMODE_LOADSHARE:
		/* Commands are only delivered to one ASP in a loadshared AS. */
		if (!command)
			return (NULL);
		/* fall through */
	case UA_TMODE_BROADCAST:
		for (; ss && ss_tst_flags(ss, ASF_ACTIVE | ASF_PENDING); ss = ss->as.next) ;
		return (ss);
	}
}
STATIC INLINE struct gp *
ap_find_asp(struct ap *ap)
{
	struct gp *gp;

	/* return first available link */
	for (gp = ap->u.as->gp.list; gp && gp_tst_flags(gp, ASF_ACTIVE | ASF_PENDING);
	     gp = gp->as.next) ;
	return gp;
}
STATIC INLINE struct gp *
ap_find_asp_next(struct ap *ap, struct gp *gp, const int command)
{
	struct as *as = ap->u.as;

	switch (as->sp.sp->tmode) {
	case UA_TMODE_OVERRIDE:
		return (NULL);
	default:
	case UA_TMODE_LOADSHARE:
		if (!command)
			return (NULL);
		/* fall through */
	case UA_TMODE_BROADCAST:
		for (; gp && gp_tst_flags(gp, ASF_ACTIVE | ASF_PENDING); gp = gp->as.next) ;
		return gp;
	}
}
STATIC INLINE struct ap *
as_p_find_ap(struct as *as)
{
	struct ap *ap;

	/* return first available sp/sg */
	for (ap = as->ap.list; ap && as_tst_flags(ap->u.as, ASF_ACTIVE | ASF_PENDING);
	     ap = ap->p.next) ;
	return ap;
}
STATIC INLINE struct ap *
as_p_find_ap_next(struct as *as, struct ap *ap, const int command)
{
	struct sp *sp = as->sp.sp;

	switch (sp->tmode) {
	case UA_TMODE_OVERRIDE:
		return (NULL);
	default:
	case UA_TMODE_LOADSHARE:
		if (!command)
			return (NULL);
		/* fall through */
	case UA_TMODE_BROADCAST:
		/* return first available sp/sg */
		for (; ap && as_tst_flags(ap->u.as, ASF_ACTIVE | ASF_PENDING); ap = ap->p.next) ;
		return ap;
	}
}

/*
 *  Selecting an SS-P.
 */
STATIC INLINE struct ss *
ap_find_ssp(struct ap *ap)
{
	struct ss *ss;

	/* return first available link */
	for (ss = ap->p.as->ss.list; ss && ss_tst_flags(ss, ASF_ACTIVE | ASF_PENDING);
	     ss = ss->as.next) ;
	return (ss);
}
STATIC INLINE struct ss *
ap_find_ssp_next(struct ap *ap, struct ss *ss, const int command)
{
	return (NULL);
}

/*
 *  Selecting an SGP.
 */
STATIC INLINE struct gp *
ap_find_sgp(struct ap *ap)
{
	struct gp *gp;

	/* return first available link */
	for (gp = ap->p.as->gp.list; gp && gp_tst_flags(gp, ASF_ACTIVE | ASF_PENDING);
	     gp = gp->as.next) ;
	return gp;
}
STATIC INLINE struct gp *
ap_find_sgp_next(struct ap *ap, struct gp *gp, const int command)
{
	return (NULL);
}

/*
 *  Selecting an SG.
 */
STATIC INLINE struct ap *
as_u_find_ap(struct as *as)
{
	struct ap *ap;

	/* return first available sg */
	for (ap = as->ap.list; ap && as_tst_flags(ap->p.as, ASF_ACTIVE | ASF_PENDING);
	     ap = ap->u.next) ;
	return ap;
}
STATIC INLINE struct ap *
as_u_find_ap_next(struct as *as, struct ap *ap, const int command)
{
	return (NULL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Receive UA Messages
 *
 *  -------------------------------------------------------------------------
 */
/**
 * sgp_recv_mgmt_err: - process received UA_MGMT_ERR message from an SGP
 * @pp: SGP-XP associated with the message
 * @q: active queue (lower read queue)
 * @m: the message
 *
 * MGMT ERR messages are often handled within the local SGP state machine, but may be applicable to
 * an AS-P if the message contains an RC value.  Also, the message class and type (and any other
 * information) included in the diagnostic can be helpful.
 */
STATIC int
sgp_recv_mgmt_err(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->ecode.val) {
	case UA_ECODE_INVALID_VERSION:	/* (0x01) */
	case UA_ECODE_INVALID_IID:	/* (0x02) */
	case UA_ECODE_UNSUPPORTED_MESSAGE_CLASS:	/* (0x03) */
	case UA_ECODE_UNSUPPORTED_MESSAGE_TYPE:	/* (0x04) */
	case UA_ECODE_UNSUPPORTED_TRAFFIC_MODE:	/* (0x05) */
	case UA_ECODE_UNEXPECTED_MESSAGE:	/* (0x06) */
	case UA_ECODE_PROTOCOL_ERROR:	/* (0x07) */
	case UA_ECODE_UNSUPPORTED_IID_TYPE:	/* (0x08) */
	case UA_ECODE_INVALID_STREAM_IDENTIFIER:	/* (0x09) */
	case IUA_ECODE_UNASSIGNED_TEI:	/* (0x0a) */
	case IUA_ECODE_UNRECOGNIZED_SAPI:	/* (0x0b) */
	case IUA_ECODE_INVALID_TEI_SAPI_COMBINATION:	/* (0x0c) */
	case UA_ECODE_REFUSED_MANAGEMENT_BLOCKING:	/* (0x0d) */
	case UA_ECODE_ASPID_REQUIRED:	/* (0x0e) */
	case UA_ECODE_INVALID_ASPID:	/* (0x0f) */
	case M2UA_ECODE_ASP_ACTIVE_FOR_IIDS:	/* (0x10) */
	case UA_ECODE_INVALID_PARAMETER_VALUE:	/* (0x11) */
	case UA_ECODE_PARAMETER_FIELD_ERROR:	/* (0x12) */
	case UA_ECODE_UNEXPECTED_PARAMETER:	/* (0x13) */
	case UA_ECODE_DESTINATION_STATUS_UNKNOWN:	/* (0x14) */
	case UA_ECODE_INVALID_NETWORK_APPEARANCE:	/* (0x15) */
	case UA_ECODE_MISSING_PARAMETER:	/* (0x16) */
	case UA_ECODE_ROUTING_KEY_CHANGE_REFUSED:	/* (0x17) */
	case UA_ECODE_INVALID_ROUTING_CONTEXT:	/* (0x19) */
	case UA_ECODE_NO_CONFIGURED_AS_FOR_ASP:	/* (0x1a) */
	case UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN:	/* (0x1b) */
	default:
		break;
	}
	goto disable;
      disable:
	return (QR_DISABLE);
}
STATIC int
asp_recv_mgmt_err(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->ecode.val) {
	case UA_ECODE_INVALID_VERSION:	/* (0x01) */
	case UA_ECODE_INVALID_IID:	/* (0x02) */
	case UA_ECODE_UNSUPPORTED_MESSAGE_CLASS:	/* (0x03) */
	case UA_ECODE_UNSUPPORTED_MESSAGE_TYPE:	/* (0x04) */
	case UA_ECODE_UNSUPPORTED_TRAFFIC_MODE:	/* (0x05) */
	case UA_ECODE_UNEXPECTED_MESSAGE:	/* (0x06) */
	case UA_ECODE_PROTOCOL_ERROR:	/* (0x07) */
	case UA_ECODE_UNSUPPORTED_IID_TYPE:	/* (0x08) */
	case UA_ECODE_INVALID_STREAM_IDENTIFIER:	/* (0x09) */
	case IUA_ECODE_UNASSIGNED_TEI:	/* (0x0a) */
	case IUA_ECODE_UNRECOGNIZED_SAPI:	/* (0x0b) */
	case IUA_ECODE_INVALID_TEI_SAPI_COMBINATION:	/* (0x0c) */
	case UA_ECODE_REFUSED_MANAGEMENT_BLOCKING:	/* (0x0d) */
	case UA_ECODE_ASPID_REQUIRED:	/* (0x0e) */
	case UA_ECODE_INVALID_ASPID:	/* (0x0f) */
	case M2UA_ECODE_ASP_ACTIVE_FOR_IIDS:	/* (0x10) */
	case UA_ECODE_INVALID_PARAMETER_VALUE:	/* (0x11) */
	case UA_ECODE_PARAMETER_FIELD_ERROR:	/* (0x12) */
	case UA_ECODE_UNEXPECTED_PARAMETER:	/* (0x13) */
	case UA_ECODE_DESTINATION_STATUS_UNKNOWN:	/* (0x14) */
	case UA_ECODE_INVALID_NETWORK_APPEARANCE:	/* (0x15) */
	case UA_ECODE_MISSING_PARAMETER:	/* (0x16) */
	case UA_ECODE_ROUTING_KEY_CHANGE_REFUSED:	/* (0x17) */
	case UA_ECODE_INVALID_ROUTING_CONTEXT:	/* (0x19) */
	case UA_ECODE_NO_CONFIGURED_AS_FOR_ASP:	/* (0x1a) */
	case UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN:	/* (0x1b) */
	default:
		break;
	}
	goto disable;
      disable:
	return (QR_DISABLE);
}
STATIC int
spp_recv_mgmt_err(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->ecode.val) {
	case UA_ECODE_INVALID_VERSION:	/* (0x01) */
	case UA_ECODE_INVALID_IID:	/* (0x02) */
	case UA_ECODE_UNSUPPORTED_MESSAGE_CLASS:	/* (0x03) */
	case UA_ECODE_UNSUPPORTED_MESSAGE_TYPE:	/* (0x04) */
	case UA_ECODE_UNSUPPORTED_TRAFFIC_MODE:	/* (0x05) */
	case UA_ECODE_UNEXPECTED_MESSAGE:	/* (0x06) */
	case UA_ECODE_PROTOCOL_ERROR:	/* (0x07) */
	case UA_ECODE_UNSUPPORTED_IID_TYPE:	/* (0x08) */
	case UA_ECODE_INVALID_STREAM_IDENTIFIER:	/* (0x09) */
	case IUA_ECODE_UNASSIGNED_TEI:	/* (0x0a) */
	case IUA_ECODE_UNRECOGNIZED_SAPI:	/* (0x0b) */
	case IUA_ECODE_INVALID_TEI_SAPI_COMBINATION:	/* (0x0c) */
	case UA_ECODE_REFUSED_MANAGEMENT_BLOCKING:	/* (0x0d) */
	case UA_ECODE_ASPID_REQUIRED:	/* (0x0e) */
	case UA_ECODE_INVALID_ASPID:	/* (0x0f) */
	case M2UA_ECODE_ASP_ACTIVE_FOR_IIDS:	/* (0x10) */
	case UA_ECODE_INVALID_PARAMETER_VALUE:	/* (0x11) */
	case UA_ECODE_PARAMETER_FIELD_ERROR:	/* (0x12) */
	case UA_ECODE_UNEXPECTED_PARAMETER:	/* (0x13) */
	case UA_ECODE_DESTINATION_STATUS_UNKNOWN:	/* (0x14) */
	case UA_ECODE_INVALID_NETWORK_APPEARANCE:	/* (0x15) */
	case UA_ECODE_MISSING_PARAMETER:	/* (0x16) */
	case UA_ECODE_ROUTING_KEY_CHANGE_REFUSED:	/* (0x17) */
	case UA_ECODE_INVALID_ROUTING_CONTEXT:	/* (0x19) */
	case UA_ECODE_NO_CONFIGURED_AS_FOR_ASP:	/* (0x1a) */
	case UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN:	/* (0x1b) */
	default:
		break;
	}
	goto disable;
      disable:
	return (QR_DISABLE);
}

/*
 *  UA_MGMT_NTFY
 *  -----------------------------------
 */
static int
rp_recv_mgmt_ntfy(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	switch (m->status.val) {
	case UA_STATUS_AS_INACTIVE:
		if (rp_get_state(rp) != AS_INACTIVE) {
			if (rp_tst_flags(rp, (ASF_ACTIVE | ASF_PENDING)))
				rp_clr_flags(rp, (ASF_ACTIVE | ASF_PENDING));
			rp_set_state(rp, AS_INACTIVE);
		}
		break;
	case UA_STATUS_AS_ACTIVE:
		if (rp_get_state(rp) != AS_ACTIVE) {
			if (rp_tst_flags(rp, (ASF_ACTIVE | ASF_PENDING))) {
				rp_clr_flags(rp, ASF_PENDING);
				rp_set_flags(rp, ASF_ACTIVE);
			}
			rp_set_state(rp, AS_ACTIVE);
		}
		break;
	case UA_STATUS_AS_PENDING:
		if (rp_get_state(rp) != AS_PENDING) {
			rp_clr_flags(rp, ASF_ACTIVE);
			rp_set_flags(rp, ASF_PENDING);
			rp_set_state(rp, AS_PENDING);
		}
		break;
	case UA_STATUS_AS_INSUFFICIENT_ASPS:
		if (!rp_tst_flags(rp, ASF_INSUFFICIENT_ASPS))
			rp_set_flags(rp, ASF_INSUFFICIENT_ASPS);
		break;
	case UA_STATUS_ALTERNATE_ASP_ACTIVE:
		if (rp_get_state(rp) != ASP_INACTIVE) {
			if (rp_tst_flags(rp, ASF_ACTIVE))
				rp_clr_flags(rp, (ASF_ACTIVE | ASF_PENDING));
			rp_set_state(rp, ASP_INACTIVE);
		}
		break;
	case UA_STATUS_ASP_FAILURE:
		break;
	case UA_STATUS_AS_MINIMUM_ASPS:
		rp_set_flags(rp, ASF_MINIMUM_ASPS);
		break;
	default:
	case UA_STATUS_AS_DOWN:
		return (-EINVAL);
	}
	return (QR_DONE);
}

/**
 * sgp_recv_mgmt_ntfy: - process a UA_MGMT_NTFY message received from an SGP
 * @pp: SGP-XP associated with the message
 * @q: active queue (lower read queue)
 * @m: (partially) decoded message
 *
 * MGMT NTFY messages are specific to an AS with the possible exception of ASP
 * Failure.  If one or more RC (IID) are contained in the message, the message
 * is specific to those GP mappings corresponding to the RC (IID) contained in
 * the message.  If there is no RC (IID) contained in the message, the message
 * applies to all GP mappings associated with the receiving SPP.  Note that AS
 * state is SGP specific and only specifies the state in the GP mapping.  The
 * overall state of the AS-P is determined by considering the combined state of
 * all SGP, and the state of the AS-U is the combined state of all AS-P.  State
 * changes to the AS-U can cause further propagation of notification messages to
 * peer ASPs.
 *
 * Error handling.  Notification messages can be processed repeatedly without
 * affect on the state machine.  If we receive an allocation or flow control
 * error when processing the message for any gp, simply return the error.  This
 * means that preceding operations will be repeated on the next pass; however,
 * as no state change should result.  If there is an invalid RC (IID) in the
 * bunch, it is simply not processed.  Once processing of all valid RC (IID) is
 * complete, an error message is retruned indicating invalid RC (IID).
 *
 * Of course this is unecessarily complicated.  One mandatory RC/IID value per
 * message (except for ASP Failure) would have been better.
 */
STATIC int
sgp_recv_mgmt_ntfy(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	int err, error = 0;
	struct rp *rp;

	if (m->iid.wp) {
		int i, iid_num;

		if (m->iid.len < sizeof(uint32_t))
			return (-EMSGSIZE);	/* parameter with wrong length field */
		if (m->iid.len % sizeof(uint32_t))
			return (-EMSGSIZE);	/* parameter with wrong length field */

		iid_num = m->iid.len / sizeof(uint32_t);
		for (i = 0; i < iid_num; i++) {
			uint32_t iid = ntohl(m->iid.wp[i]);

			/* find rp associated with iid for sgp */
			for (rp = pp->rp.list; rp && rp->gp.gp->as.as->iid != iid;
			     rp = rp->pp.next) ;
			/* process notification for rp */
			if (rp) {
				if ((err = rp_recv_mgmt_ntfy(rp, q, m)) < 0)
					error = error ? : err;
				continue;
			}
			error = error ? : -EIO;	/* Invalid RC (IID) */
		}
		if (error)
			return (error);
	} else if (m->iid_text.cp) {
		/* find rp associated with text iid for sgp */
		for (rp = pp->rp.list;
		     rp && strncmp(m->iid_text.cp, rp->gp.gp->as.as->iid_text, m->iid_text.len);
		     rp = rp->pp.next) ;
		/* process notification for rp */
		if (rp) {
			if ((err = rp_recv_mgmt_ntfy(rp, q, m)) < 0)
				error = err;
		} else
			err = -EIO;	/* Invalid IID */
	} else if (m->iid_range.wp) {
		int i, iid_range_num;

		if (m->iid_range.len < (sizeof(uint32_t) << 1))
			return (-EMSGSIZE);	/* parameter with wrong length field */
		if (m->iid_range.len % (sizeof(uint32_t) << 1))
			return (-EMSGSIZE);	/* parameter with wrong length field */

		iid_range_num = m->iid.len / sizeof(uint32_t);
		for (i = 0; i < iid_range_num; i += 2) {
			uint32_t iid;
			uint32_t iid_start = ntohl(m->iid_range.wp[i]);
			uint32_t iid_stop = ntohl(m->iid_range.wp[i + 1]);

			if (iid_start > iid_stop)
				error = error ? : -EIO;
			for (iid = iid_start; iid <= iid_stop; iid++) {
				/* find rp associated with iid for sgp */
				for (rp = pp->rp.list; rp && rp->gp.gp->as.as->iid != iid;
				     rp = rp->pp.next) ;
				/* process notification for rp */
				if (rp) {
					if ((err = rp_recv_mgmt_ntfy(rp, q, m)) < 0)
						error = error ? : err;
					continue;
				}
				error = -EIO;
			}
		}
	} else if (m->rc.wp) {
		/* message applies to specified RC (IID) */
		int i, rc_num;

		if (m->rc.len < sizeof(uint32_t))
			return (-EMSGSIZE);	/* parameter with wrong length field */
		if (m->rc.len % sizeof(uint32_t))
			return (-EMSGSIZE);	/* parameter with wrong length field */

		rc_num = m->rc.len / sizeof(uint32_t);
		for (i = 0; i < rc_num; i++) {
			uint32_t rc = ntohl(m->rc.wp[i]);

			/* find rp associated with rc for sgp */
			for (rp = pp->rp.list; rp && rp->gp.gp->as.as->rc != rc; rp = rp->pp.next) ;
			/* process notification for rp */
			if (rp) {
				if ((err = rp_recv_mgmt_ntfy(rp, q, m)) < 0)
					error = error ? : err;
				continue;
			}
			error = -EIO;	/* Invalid RC (IID) */
		}
	} else {
		/* message applies to all RC (IID) */
		struct rp *rp;
		bool processed = false;

		for (rp = pp->rp.list; rp; rp = rp->pp.next) {
			if ((err = rp_recv_mgmt_ntfy(rp, q, m)) < 0)
				error = error ? : err;
			processed = true;
		}
		if (!processed)
			error = -EXDEV;	/* No configured AS for ASP. */
	}
	if (error)
		return (error);
	return (QR_DONE);
}

/**
 * asp_recv_mgmt_ntfy: - process a UA_MGMT_NTFY message received from an ASP
 * @pp: ASP/XP to which message pertains
 * @q: active queue (lower read queue)
 * @m: (partially) decoded message
 *
 * MGMT NTFY messages are not sent by an ASP, so to receive one from an
 * ASP is an unexpected message error.
 */
STATIC int
asp_recv_mgmt_ntfy(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);	/* Unexpected in this direction. */
}

/**
 * spp_recv_mgmt_ntfy: - process a UA_MGMT_NTFY message received from an SPP
 * @pp: SPP-XP associated with the message
 * @q: active queue (lower read queue)
 * @m: (partially) decoded message
 */
STATIC int
spp_recv_mgmt_ntfy(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (QR_DISABLE);
}

/*
 *  UA_ASPS_ASPUP_REQ
 *  -----------------------------------
 */
STATIC int
sgp_recv_asps_aspup_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);	/* Unexpected in this direction. */
}
STATIC int
asp_recv_asps_aspup_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	int err;
	struct rp *rp;
	struct gp *gp;
	struct spp *spp = pp->spp.spp;

	/* Because ASP_UP messages may be repeated by the ASP cannot send other message in between
	   (they are sent ordered on Stream 0 and the ASP must wait for the ASP Up Ack). Therefore, 
	   the ASP_UP is unexpected in any state other than ASP_DOWN or ASP_WACK_ASPUP. */
	switch (pp_get_state(pp)) {
	case ASP_WACK_ASPDN:
		swerr();	/* This is a transient state when processing ASP Down only. */
		pp_set_state(pp, ASP_DOWN);
		spp->pp.counts[ASP_WACK_ASPDN]--;
		spp->pp.counts[ASP_DOWN]++;
		if (spp->pp.counts[ASP_WACK_ASPDN] == 0)
			for (spp->state = ASP_UP; spp->state > 0 && spp->pp.counts[spp->state] == 0;
			     spp->state--) ;
		/* fall through */
	case ASP_DOWN:
		if (spp_tst_flags(spp, ASF_MGMT_BLOCKED))
			return (-EPERM);	/* Management blocked. */
		pp_set_state(pp, ASP_WACK_ASPUP);
		spp->pp.counts[ASP_DOWN]--;
		spp->pp.counts[ASP_WACK_ASPUP]++;
		if (spp_get_state(spp) < ASP_WACK_ASPUP)
			spp_set_state(spp, ASP_WACK_ASPUP);
		goto aspup_expected;
		/* fall through */
	case ASP_WACK_ASPUP:
		/* first or repeated attempt */
	      aspup_unexpected:
		/* The unexpected case is complicated by the fact that we might have to transition
		   the AS from an active state.  Note that an AS is considered down even when an
		   ASP is up when the AS is dynamic and the ASP is not registered for the AS. */
		for (rp = pp->rp.list; rp; rp = rp->pp.next) {
			if (rp_get_state(rp) > AS_INACTIVE) {
				if (as_tst_flags(rp->gp.gp->as.as, ASF_REGISTERED)) {
					if ((err = rp_u_set_state(rp, AS_DOWN)))
						return (err);
				} else {
					if ((err = rp_u_set_state(rp, AS_INACTIVE)))
						return (err);
				}
			}
		}
	      aspup_expected:
		if ((err = ua_send_asps_aspup_ack(pp, q, NULL, 0)))
			return (err);
		todo(("Notify management that the ASP has come up\n"));
		pp_set_state(pp, ASP_UP);
		spp->pp.counts[ASP_WACK_ASPUP]--;
		spp->pp.counts[ASP_UP]++;
		spp_set_state(spp, ASP_UP);
		return (QR_DONE);
	case ASP_UP:
		/* Far more complicated.  Need to move all AS from any active state to AS-INACTIVE
		   if static and AS-DOWN if dynamic.  If already in AS-INACTIVE no action is taken
		   (dynamic AS remain registered).  If any AS changed state, need to send
		   unexpected message as well as ASP Up Ack.  If nothing changes, it is likely a
		   repeated ASP Up. */
		for (rp = pp->rp.list; rp; rp = rp->pp.next)
			if (rp_get_state(rp) > AS_INACTIVE)
				break;
		if (rp) {
			/* If any AS state will change from active, send an unexpected message as
			   well.  If nothing changes, it is likely a repeated ASP Up. */
			if ((err = ua_send_mgmt_err(pp->xp.xp, q, UA_ECODE_UNEXPECTED_MESSAGE,
						    m->mp->b_rptr, m->mp->b_wptr - m->mp->b_rptr)))
				return (err);
		}
		pp_set_state(pp, ASP_WACK_ASPUP);
		spp->pp.counts[ASP_UP]--;
		spp->pp.counts[ASP_WACK_ASPUP]++;
		if (spp->pp.counts[ASP_UP] == 0)
			for (spp->state = ASP_UP; spp->state > 0 && spp->pp.counts[spp->state] == 0;
			     spp->state--) ;
		if (rp)
			goto aspup_unexpected;
		goto aspup_expected;
	}
	swerr();
	return (-EFAULT);
}

/**
 * sp_recv_asps_aspup_req: - process anonymous ASP Up message
 * @xp: transport on which the message was received
 * @m: the message
 *
 * This function is only called for transports that have been associated with an SP-U (that is, the
 * connection is from an ASP), but it cannot be determined from the connecting information (IP
 * addresses and port) for which ASP the connection has been made.  This occurs when there are
 * multiple ASPs for the same IP addresses (multiple processes on the same host) and dynamic port
 * allocation is used by the host to connect.  In that case, we have not yet associated the remote
 * ASP with an SPP structure, but were waiting for an ASP Up with an ASP Id parameter to identify
 * the ASP.  Another instance is when an ASP is connecting to our well-known port number and we have
 * not been configured to recognize the ASP at all.  In this case too, we must wait for an ASP Up
 * with an ASP Id parameter to identify the ASP.  A third instance is when an ASP for which we are
 * configured connects from an unexpected source address (perhaps it has connected via a NAT or
 * something).
 */
STATIC int
sp_recv_asps_aspup_req(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	int err;
	struct pp *pp;
	struct rp *rp;
	struct gp *gp;
	struct spp *spp;

	/* If we are not yet associated with an spp but we are associated with an sp (must be one
	   or the other) then if there is an ASP Id in the message, we can discover whether this is 
	   a unique ASP and associate it with the correct (or a new) spp.  If there is no ASPID in
	   the message, refuse the ASP Up with an ERR indicating that an ASP Id is required.
	   Perhaps the ASP will reattempt with an ASP Id. */
	if (!m->aspid.cp)
		/* Because the transport endpoint is not associated with an ASP by configuration,
		   it is necessary that an ASP Id be provided in the ASP Up message. */
		return (-ESRCH);	/* ASP Id required. */
	if (!m->aspid.val)
		/* We do not permit an ASP Id value of zero, which we treat as special to indicate
		   that an ASP Id has not yet been assigned.  ASP Ids must be unique non-zero
		   numbers. */
		return (-ENOENT);	/* Bad ASP Id. */
	/* Find an ASP SPP structure that contains that ASP Id. */
	for (spp = xp->sp->spp.list; spp && spp->aspid != m->aspid.val; spp = spp->sp.next) ;
	/* FEATURE: We create an ASP if a structure does not exist. This ASP is given access to all 
	   of the AS defined for this ASP Pool (SP-U).  If the ASP should not be permitted to
	   access the ASP Pool it should not have been linked with it by the connection manager.
	   That is, the ASP must be connecting at least from a known IP address from which ASPs
	   normally connect to the ASP pool. */
	if (!spp) {
		/* Did not find an existing spp with the same aspid: allocate a new one. */
		if (!(spp = ua_alloc_spp(spp_get_id(0), UA_OBJ_TYPE_ASP, xp->sp, m->aspid.val, 0)))
			return (-ENOMEM);
		/* Note that ua_alloc_spp() must automatically create GP structures for each AS
		   configured for the SP. */
	} else {
		/* If this ASP already has a primary transport, then it is a conflict. */
		for (pp = spp->pp.list; pp && pp->xp.xp->type != UA_OBJ_TYPE_XP_ASP;
		     pp = pp->spp.next) ;
		if (pp)
			return (-ENOENT);	/* Bad ASP Id. */
	}

	if (!(pp = ua_alloc_pp(spp, xp)))
		return (-ENOMEM);
	/* Note that ua_alloc_pp() must automatically create RP structures for each GP configured
	   for the SPP. */

	/* After this point, if an error occurs, we will not return here (an SPP has already been
	   associated with the transport). */
	return asp_recv_asps_aspup_req(pp, q, m);
}
STATIC int
spp_recv_asps_aspup_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	/* do what the SGP does */
	return asp_recv_asps_aspup_req(pp, q, m);
}

static int
xs_recv_asps_aspup_req(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO); /* Unexpected in this direction. */
}
/**
 * xg_recv_asps_aspup_req: - process anonymous ASP Up message
 * @xp: transport on which the message was received
 * @m: the message
 *
 * This function is called from cross-link transports that have been associated with an SP or SG but
 * do not have an ASP, SGP or SPP structure allocated for the corresponding ASP Id.  ASP Up received
 * on these links with an ASP Id is an offer to relay messages to (but not from) that ASP, SGP, SPP
 * using the cross-link.  ASP Up received with the ASP Id of the peer at the other end of the
 * cross-link is a request to accept messages destined for local SS-U or SS-P.  This can be
 * distinguished, even in a dynamic case, by the fact that the spp->sp.sp->type matches the
 * xp->sp->type; however, we do not yet support the dynamic case (cross-links must be fully
 * specified with IP addresses and port numbers at both ends).
 */
static int
xg_recv_asps_aspup_req(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	int err;
	struct np *np;
	struct pp *pp;
	struct spp *spp = NULL;

	if (!m->aspid.cp)
		return (-EXDEV);	/* ASP Id required on cross-links. */

	/* look for an ASP with the same id */
	for (np = xp->sp->np.list; np; np = np->p.next)
		for (spp = np->u.sp->spp.list; spp && spp->aspid != m->aspid.val;
		     spp = spp->sp.next) ;
	if (spp == NULL)
		return (-ENOENT);	/* Bad ASP Id */
	pp = ua_alloc_pp(spp, xp);
	if (pp == NULL)
		return (-ENOMEM);
	/* Process as though it was there in the first place, in which case it would have been
	   processed like an ASP. */
	return asp_recv_asps_aspup_req(pp, q, m);
}
static int
xp_recv_asps_aspup_req(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	/* XPP does what XGP does. */
	return xg_recv_asps_asup_req(xp, q, m);
}

/*
 *  UA_ASPS_ASPDN_REQ
 *  -----------------------------------
 */
STATIC int
sgp_recv_asps_aspdn_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);	/* Unexpected in this direction. */
}
STATIC int
asp_recv_asps_aspdn_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	int err;
	struct rp *rp;
	struct spp *spp = pp->spp.spp;

	switch (pp_get_state(pp)) {
	case ASP_WACK_ASPUP:
		swerr();	/* this is a transient state when processing ASP Up only. */
		pp_set_state(pp, ASP_UP);
		spp->pp.counts[ASP_WACK_ASPUP]--;
		spp->pp.counts[ASP_UP]++;
		spp_set_state(spp, ASP_UP);
		/* fall through */
	case ASP_UP:
		pp_set_state(pp, ASP_WACK_ASPDN);
		spp->pp.counts[ASP_UP]--;
		spp->pp.counts[ASP_WACK_ASPDN]++;
		if (spp->pp.counts[ASP_UP] == 0)
			spp_set_state(spp, ASP_WACK_ASPDN);
		/* fall through */
	case ASP_WACK_ASPDN:
		for (rp = pp->rp.list; rp; rp = rp->pp.next)
			if ((err = rp_u_set_state(rp, q, AS_DOWN)))
				return (err);
		pp_set_state(pp, ASP_DOWN);
		spp->pp.counts[ASP_WACK_ASPDN]--;
		spp->pp.counts[ASP_DOWN]++;
		if (spp->pp.counts[ASP_WACK_ASPDN] == 0)
			for (spp->state = ASP_UP; spp->state > 0 && spp->pp.counts[spp->state];
			     spp->state--) ;
		todo(("Notify management that the ASP has gone down\n"));
		/* fall through */
	case ASP_DOWN:
		if ((err = ua_send_asps_aspdn_ack(pp, q, NULL, 0)))
			return (err);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}
STATIC int
spp_recv_asps_aspdn_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	/* Do what the SGP does. */
	return asp_recv_asps_aspdn_req(pp, q, m);
}

/*
 *  UA_ASPS_HBEAT_REQ
 *  -----------------------------------
 */
STATIC int
sgp_recv_asps_hbeat_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return ua_send_asps_hbeat_ack(pp->xp.xp, q, m->hinfo.cp, m->hinfo.len);
}
STATIC int
asp_recv_asps_hbeat_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return ua_send_asps_hbeat_ack(pp->xp.xp, q, m->hinfo.cp, m->hinfo.len);
}
STATIC int
spp_recv_asps_hbeat_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return ua_send_asps_hbeat_ack(pp->xp.xp, q, m->hinfo.cp, m->hinfo.len);
}

/*
 *  UA_ASPS_ASPUP_ACK
 *  -----------------------------------
 */
STATIC int
sgp_recv_asps_aspup_ack(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	static const char info[] = "Received ASP Up Ack after ASP Down";
	struct spp *spp = pp->spp.spp;
	struct rp *rp;

	switch (pp_get_state(pp)) {
	case ASP_DOWN:
		if ((err = ua_send_asps_aspdn_req(pp, q, info, sizeof(info))))
			return (err);
		pp_set_state(pp, ASP_WACK_ASPDN);
		spp->pp.counts[ASP_DOWN]--;
		spp->pp.counts[ASP_WACK_ASPDN]++;
		if (spp_get_state(spp) < ASP_WACK_ASPDN)
			spp_set_state(spp, ASP_WACK_ASPDN);
		return (-EPROTO);	/* Unexpected message in this state. */
	case ASP_WACK_ASPDN:
		if ((err = ua_send_asps_aspdn_req(pp, q, info, sizeof(info))))
			return (err);
		return (-EPROTO);	/* Unexpected message in this state. */
	case ASP_UP:
		/* "If the ASP receives an unexpected ASP Up Ack message, the ASP should consider
		   itself in the ASP-INACTIVE state [for all AS].  If the ASP was not in the
		   ASP-INACTIVE state [for an AS], it SHOULD send an Error message and then
		   initiate procedures to return itself to its previous state." Note that routing
		   keys do not need to be reregistered.  This is the normal case when we send
		   multiple ASP Up messages and the SGP responds to each of them. */
		for (rp = pp->rp.list; rp; rp = rp->pp.next)
			if (rp_get_state(rp) > AS_INACTIVE)
				break;
		if (rp) {
			/* If any AS state will change from active, send an unexpected message and
			   and take actions to return the AS to its former state. */
			if ((err = ua_send_mgmt_err(pp->xp.xp, q, UA_ECODE_UNEXPECTED_MESSAGE,
						    m->mp->b_rptr, m->mp->b_wptr - m->mp->b_rptr)))
				return (err);
		}
		pp_set_state(pp, ASP_WACK_ASPUP);
		spp->pp.counts[ASP_UP]--;
		spp->pp.counts[ASP_WACK_ASPUP]++;
		if (spp->pp.counts[ASP_UP] == 0)
			for (spp->state = ASP_UP; spp->state > 0 && spp->pp.counts[spp->state] == 0;
			     spp->state--) ;
		/* fall through */
	case ASP_WACK_ASPUP:
		/* first or second attempt */
		/* Whether the ASP Up Ack was expected or not, we want to move both static and
		   dynamic AS to the AS-INACTIVE state.  For dynamic AS, requesting that the AS
		   move to the AS-INACTIVE state from the AS-DOWN state will cause registration to
		   be attempted. */
		for (rp = pp->rp.list; rp; rp = rp->pp.next)
			if ((err = rp_p_set_state(rp, AS_INACTIVE)))
				return (err);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}
STATIC int
asp_recv_asps_aspup_ack(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);	/* Unexpected in this direction. */
}
STATIC int
spp_recv_asps_aspup_ack(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	/* Do what the ASP does. */
	return sgp_recv_asps_aspup_ack(pp, q, m);
}

/*
 *  UA_ASPS_ASPDN_ACK
 *  -----------------------------------
 */
STATIC int
sgp_recv_asps_aspdn_ack(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	int err;

	switch (pp_get_state(pp)) {
	case ASP_WACK_ASPDN:
	case ASP_WACK_ASPUP:
		/* solicited ASPDN Ack */
		todo(("Indicate to management that the ASP is down.\n"));
		return sgp_pp_set_state(pp, q, ASP_DOWN);
	case ASP_UP:
		/* unsolicited ASP Down: we attempt once to re-establish the ASP.  If the second
		   attempt fails (in state ASP_WACK_ASPUP) we notify management and remain in the
		   down state. */
		if ((err = ua_send_asps_aspup_req(pp, q, NULL, 0)))
			return (err);
		return sgp_pp_set_state(pp, q, ASP_DOWN);
		return (QR_DONE);
	}
	rare();
      disable:
	return (QR_DISABLE);
}
STATIC int
asp_recv_asps_aspdn_ack(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);	/* Unexpected in this direction. */
}
STATIC int
spp_recv_asps_aspdn_ack(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	/* Do what the ASP does. */
	return sgp_recv_asps_aspdn_ack(pp, q, m);

}

/*
 *  UA_ASPS_HBEAT_ACK
 *  -----------------------------------
 */
STATIC int
sgp_recv_asps_hbeat_ack(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	pp_timer_stop(pp, tbeat);
	pp_timer_start(pp, tidle);
	return (QR_DONE);
}
STATIC int
asp_recv_asps_hbeat_ack(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	pp_timer_stop(pp, tbeat);
	pp_timer_start(pp, tidle);
	return (QR_DONE);
}
STATIC int
spp_recv_asps_hbeat_ack(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	pp_timer_stop(pp, tbeat);
	pp_timer_start(pp, tidle);
	return (QR_DONE);
}

/*
 *  UA_ASPT_ASPAC_REQ
 *  -----------------------------------
 */
STATIC int
sgp_recv_aspt_aspac_req(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
asp_recv_aspt_aspac_req(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp = rp->gp.gp;
	struct as *as = gp->as.as;

	if (m->tmode.wp) {
		if (as->tmode == 0) {
			/* The AS has not been configured for a traffic mode yet but the ASP has
			   specified a traffic mode.  Use whatever the ASP specified in the
			   message. */
			switch (m->tmode.val) {
			case UA_TMODE_OVERRIDE:
			case UA_TMODE_LOADSHARE:
			case UA_TMODE_BROADCAST:
				break;
			default:
				return (-ENOSYS);	/* Bad traffic mode. */
			}
			as->tmode = m->tmode.val;
		} else {
			/* The AS has been configured for a traffic mode and the ASP must be
			   consistent with that. */
			if (as->tmode != m->tmode.val)
				return (-ENOSYS);	/* Bad traffic mode. */
		}
	} else {
		if (as->tmode == 0)
			/* Strange.  The AS has not been configured for a traffic mode (that is we
			   were prepared to accept any traffic mode the ASP requested), the ASP has
			   not specified one, but one is necessary.  The SG is free to choose one by
			   default.  We choose loadshare (because this will actually work with
			   override as well if the ASPs behave as expected. */
			as->mode = UA_TMODE_LOADSHARE;
	}
	switch (rp_get_state(rp)) {
	case AS_DOWN:
		return (-EPROTO);	/* Unexpected in this state. */
	case AS_INACTIVE:
		if (gp_tst_flags(gp, ASF_MGMT_BLOCKED))
			return (-EPERM);	/* Management blocking. */
		if ((err = rp_u_set_state(rp, q, AS_WACK_ASPAC)))
			return (err);
		/* fall through */
	case AS_WACK_ASPAC:
		switch (gp_get_state(gp)) {
		case AS_WACK_ASPAC:
			/* Already activating from some other transport or SS-U.  When activation
			   is complete, all activators will get a response. */
			return (QR_DONE);
		case AS_WACK_ASPIA:
			/* Already deactivating from some other transport or SS-U.  This is a
			   transient state from some other transport or SS-U, let it complete its
			   transition. */
			swerr();
			return (QR_RETRY);
		case AS_ACTIVE:
			/* Already active, simply allow activation. */
			if ((err = ua_send_aspt_aspac_ack(rp, q, NULL, 0)))
				return (err);
			todo(("Notify management that ASP has gone active\n"));
			if ((err = rp_u_set_state(rp, q, AS_ACTIVE)))
				return (err);
			return (QR_DONE);
		default:
			swerr();
			return (-EFAULT);
		}
	case AS_WACK_ASPIA:
		swerr();	/* This is a transient state while processing ASP Inactive
				   messages. */
		return (-EFAULT);
	case AS_ACTIVE:
		/* Already active, simply allow activation. */
		if ((err = ua_send_aspt_aspac_ack(rp, q, NULL, 0)))
			return (err);
		return (QR_DONE);
	}

#if 0
	/* This still needs to be done at the gp level within rp_u_set_state(). */
	switch (as_get_state(as)) {
	case AS_INACTIVE:
		swerr();
		return (QR_DONE);
	case AS_WACK_ASPAC:
		if (as->tmode != UA_TMODE_OVERRIDE)
			return (QR_DONE);
		/* look for activating ASP */
		for (g2 = as->gp.list; g2 && g2 != gp && gp_get_state(g2) != AS_WACK_ASPAC;
		     g2 = g2->as.next) ;
		if (g2) {
			if ((err = ua_send_aspt_aspia_ack(g2, q, NULL, 0)))
				return (err);
			if ((err = gp_u_set_state(g2, q, AS_INACTIVE)))
				return (err);
		}
		for (ss = as->ss.list; ss && ss_get_state(ss) != AS_WACK_ASPAC; ss = ss->as.next) ;
		if (ss) {
			if ((err = sl_error_ind(ss, q, 0, 0)))
				return (err);
			if ((err = ss_u_set_state(ss, q, AS_INACTIVE)))
				return (err);
		}
		return (QR_DONE);
	case AS_WACK_ASPIA:
		if (as->tmode != UA_TMODE_OVERRIDE)
			break;
		/* look for deactivating ASP */
		for (g2 = as->gp.list; g2 && g2 != gp && gp_get_state(g2) != AS_WACK_ASPIA;
		     g2 = g2->as.next) ;
		if (g2) {
			if ((err = ua_send_aspt_aspia_ack(g2, q, NULL, 0)))
				return (err);
			if ((err = gp_u_set_state(g2, q, AS_INACTIVE)))
				return (err);
		}
		for (ss = as->ss.list; ss && ss_get_state(ss) != AS_WACK_ASPIA; ss = ss->as.next) ;
		if (ss) {
			if ((err = sl_disable_con(ss, q)))
				return (err);
			if ((err = ss_u_set_state(ss, q, AS_INACTIVE)))
				return (err);
		}
		break;
	case AS_ACTIVE:
		if (as->tmode != UA_TMODE_OVERRIDE)
			break;
		/* look for active ASP */
		for (g2 = as->gp.list; g2 && g2 != gp && gp_get_state(g2) != AS_ACTIVE;
		     g2 = g2->as.next) ;
		if (g2) {
			if ((err = ua_send_mgmt_ntfy_as(g2, q,
							UA_STATUS_ALTERNATE_ASP_ACTIVE,
							spp, NULL, 0)))
				return (err);
			if ((err = gp_u_set_state(g2, q, AS_INACTIVE)))
				return (err);
		}
		for (ss = as->ss.list; ss && ss_get_state(ss) != AS_ACTIVE; ss = ss->as.next) ;
		if (ss) {
			if ((err = sl_error_ind(ss, q, 0, 0)))
				return (err);
			if ((err = ss_u_set_state(ss, q, AS_INACTIVE)))
				return (err);
		}
		break;
	}
#endif
}
STATIC int
spp_recv_aspt_aspac_req(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	/* Do what the SGP does. */
	return asp_recv_aspt_aspac_req(rp, q, m);
}

/*
 *  UA_ASPT_ASPIA_REQ
 *  -----------------------------------
 */
STATIC int
sgp_recv_aspt_aspia_req(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
asp_recv_aspt_aspia_req(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	int err;

	switch (rp_get_state(rp)) {
	case AS_DOWN:
		return (-EPROTO);	/* Unexpected in this state. */
	case AS_ACTIVE:
		if ((err = rp_u_set_state(rp, q, AS_WACK_ASPIA)))
			return (err);
		/* fall through */
	case AS_WACK_ASPIA:
	case AS_WACK_ASPAC:
	case AS_INACTIVE:
		if ((err = ua_send_aspt_aspia_ack(rp, q, NULL, 0)))
			return (err);
		rp_u_set_state(rp, q, ASP_INACTIVE);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}
STATIC int
spp_recv_aspt_aspia_req(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	/* Do what SGP does. */
	return asp_recv_aspt_aspia_req(rp, q, m);
}

/*
 *  UA_ASPT_ASPAC_ACK
 *  -----------------------------------
 */
STATIC int
sgp_recv_aspt_aspac_ack(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	int err;

	switch (rp_get_state(rp)) {
	case ASP_ACTIVE:
		return (QR_DONE);
	case ASP_WACK_ASPAC:
		/* complete activation */
		if ((err = rp_p_set_state(rp, q, ASP_ACTIVE)))
			return (err);
		return (QR_DONE);
	case ASP_WACK_ASPIA:
	case ASP_INACTIVE:
		/* deactivate again */
		if ((err = ua_send_aspt_aspia_req(rp, q, NULL, 0)))
			return (err);
		return (-EPROTO);	/* Unexpected in this state. */
	}
	swerr();
	return (-EFAULT);
}
STATIC int
asp_recv_aspt_aspac_ack(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);	/* Unexpected in this direction. */
}
STATIC int
spp_recv_aspt_aspac_ack(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	/* Do what ASP does. */
	return sgp_recv_aspt_aspac_ack(rp, q, m);
}

/*
 *  UA_ASPT_ASPIA_ACK
 *  -----------------------------------
 */
STATIC int
sgp_recv_aspt_aspia_ack(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	int err;

	switch (rp_get_state(rp)) {
	case ASP_INACTIVE:
		/* ignore, probably late ack to our ASPIA */
		return (QR_DONE);
	case ASP_WACK_ASPIA:
		if ((err = rp_p_set_state(rp, q, AS_INACTIVE)))
			return (err);
		return (QR_DONE);
	case ASP_ACTIVE:
		/* This is an unsolicited ASP Inactive Ack.  It is the SGP forcing us to the
		   inactive state.  We must placed the ASP in the inactive state and then try to
		   bring it back up again. */
		if ((err = rp_p_set_state(rp, q, AS_WACK_ASPAC)))
			return (err);
		/* fall through */
	case ASP_WACK_ASPAC:
		/* try activation again */
		if ((err = ua_send_aspt_aspac_req(rp, q, NULL, 0)))
			return (err);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}
STATIC int
asp_recv_aspt_aspia_ack(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);	/* Unexpected in this direction. */
}
STATIC int
spp_recv_aspt_aspia_ack(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	/* Does what ASP does. */
	return sgp_recv_aspt_aspia_ack(rp, q, m);
}

/*
 *  UA_RKMM_REG_REQ
 *  -----------------------------------
 */
STATIC int
sgp_recv_rkmm_reg_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
asp_recv_rkmm_reg_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}
STATIC int
spp_recv_rkmm_reg_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	/* Do what SGP does. */
	return asp_recv_rkmm_reg_req(pp, q, m);
}

/*
 *  UA_RKMM_REG_RSP
 *  -----------------------------------
 */
STATIC int
sgp_recv_rkmm_reg_rsp(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	fixme(("Write this function.\n"));
	return (-EFAULT);
}
STATIC int
asp_recv_rkmm_reg_rsp(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);	/* Unexpected in this direction. */
}
STATIC int
spp_recv_rkmm_reg_rsp(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	/* Do what ASP does. */
	return sgp_recv_rkmm_reg_rsp(pp, q, m);
}

/*
 *  UA_RKMM_DEREG_REQ
 *  -----------------------------------
 */
STATIC int
sgp_recv_rkmm_dereg_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);	/* Unexpected in this direction. */
}

/**
 * asp_recv_rkmm_deref_req: - deregister for an AS
 * @pp: ASP-XP from which the message was received
 * @q: active qeuue
 * @m: the message
 *
 * Note that we should lookup up routing key management messages by AS and enter this function with
 * a gp instead of an spp.  If this SPP needs to register for the AS (as indicated by flags in the
 * GP structure) and the GP is in the AS-INACTIVE state, move the GP to the AS-DOWN state and
 * respond with success.  If the GP is in the AS-ACTIVE (or transitional) state, respond with
 * failure Error - ASP Currently Active for Routing Context and do nothing.  If the GP is already in
 * the AS-DOWN state,
 *
 * M2UA DEREG REQ contains a list of binary or text interface identifiers as a repeated list of
 * parameters.  We do not support text interface identifiers for DEREG REQ or repeated binary
 * Interface Identifier parameters (the binary identifier was returned in response to the REG REQ
 * and * can contain multiple identifiers).  The M3UA, SUA and TUA DEREG REQ contains a routing
 * context parameter that can contain one or more routing contexts.
 */
STATIC int
asp_recv_rkmm_dereg_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	int err, num_rc = 0, processed = 0;
	uint32_t *wp = NULL;
	unsigned char *cp = NULL;
	struct rp *rp;

	if ((wp = m->rc.wp)) {
		if (m->rc.len < sizeof(uint32_t)
		    || (m->rc.len & (sizeof(uint32_t) - 1)))
			return (-EMSGSIZE);
		num_rc = m->rc.len >> 2;
	} else if ((wp = m->iid.wp)) {
		if (m->iid.len < sizeof(uint32_t)
		    || (m->iid.len & (sizeof(uint32_t) - 1)))
			return (-EMSGSIZE);
		num_rc = m->iid.len >> 2;
	} else
		return (-ENXIO);	/* missing mandatory parameter */

	{
		mblk_t *dp;
		uint32_t *results;

		if (!(dp = ss7_allocb(num_rc * sizeof(uint32_t), BPRI_MED)))
			return (-ENOBUFS);
		results = (uint32_t *) bp->b_wptr;

		for (i = 0; i < num_rc; i++)
			results[i] = __constant_htonl(UA_DEREG_STATUS_INVALID_ID);

		for (processed = 0, rp = pp->rp.list; rp; rp = rp->pp.next) {
			int i;
			struct as *as = rp->gp.gp->as.as;

			for (i = 0; i < num_rc; i++)
				if (as->rc == ntohl(wp[i]))
					break;
			if (i >= num_rc)
				continue;
			if (!as_tst_flags(as, ASF_REGISTRATION)) {
				results[i] = __constant_htonl(UA_DEREG_STATUS_PERMISSION_DENIED);
				continue;
			}
			switch (rp_get_state(rp)) {
			case AS_DOWN:
				results[i] = __constant_htonl(UA_DEREG_STATUS_NOT_REGISTERED);
				break;
			case AS_ACTIVE:
			case AS_WACK_ASPAC:
			case AS_WACK_ASPIA:
				/* FIXME: M2UA needs to send an ERR message. */
				results[i] = __constant_htonl(UA_DEREG_STATUS_ID_ACTIVE_FOR_AS);
				break;
			case AS_INACTIVE:
				results[i] = __constant_htonl(UA_DEREG_STATUS_SUCCESS);
				break;
			default:
				results[i] = __constant_htonl(UA_DEREG_STATUS_UNKNOWN);
				break;
			}
			processed++;
			if (processed >= num_rc)
				break;
		}
		if ((err = ua_send_rkmm_dereg_rsp(pp, q, num_rc, wp, results)) < 0) {
			freeb(dp);
			return (err);
		}
		freeb(dp);
		/* Once more, moving all the deregistered ones to the AS_DOWN state. */
		for (processed = 0, rp = pp->rp.list; rp; rp = rp->pp.next) {
			int i;
			struct as *as = rp->gp.gp->as.as;

			for (i = 0; i < num_rc; i++)
				if (as->rc == ntohl(wp[i]))
					break;
			if (i >= num_rc)
				continue;
			if (!as_tst_flags(as, ASF_REGISTRATION))
				continue;
			if (rp_get_state(rp) == AS_INACTIVE) {
				/* Note that moving an RP from the AS_INACTIVE state to the AS_DOWN
				   state should never change the state of the AS-U in such a way
				   that notification is required.  That is, if we are the last ASP
				   in the AS_INACTIVE state, the AS-U transition from AS-INACTIVE to 
				   AS-DOWN sends no notifications and does not affect the state of
				   the AS-P. */
				if ((err = rp_u_set_state(rp, AS_DOWN))) {
					swerr();
					/* force it to that state anyway */
					rp->gp.gp->rp.counts.inactive--;
					rp->gp.gp->rp.counts.down++;
					rp_set_state(rp, AS_DOWN);
				}
			}
			processed++;
			if (processed >= num_rc)
				break;
		}
	}
	return (QR_DONE);
}
STATIC int
spp_recv_rkmm_dereg_req(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	/* Do what SGP does. */
	return asp_recv_rkmm_dereg_req(pp, q, m);
}

/*
 *  UA_RKMM_DEREG_RSP
 *  -----------------------------------
 */
STATIC int
sgp_recv_rkmm_dereg_rsp(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
asp_recv_rkmm_dereg_rsp(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
spp_recv_rkmm_dereg_rsp(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  IUA Received Messages
 */
/* Data Request */
STATIC int
sgp_recv_qptm_data_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_data_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_data_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/* Data Indication */
STATIC int
sgp_recv_qptm_data_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_data_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_data_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/* Unitdata Request */
STATIC int
sgp_recv_qptm_udat_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_udat_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_udat_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/* Unitdata Indication */
STATIC int
sgp_recv_qptm_udat_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_udat_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_udat_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/* Establish Request */
STATIC int
sgp_recv_qptm_estb_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_estb_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_estb_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/* Establish Confirm */
STATIC int
sgp_recv_qptm_estb_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_estb_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_estb_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/* Establish Indication */
STATIC int
sgp_recv_qptm_estb_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_estb_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_estb_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/* Release Request */
STATIC int
sgp_recv_qptm_rels_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_rels_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_rels_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/* Release Confirm */
STATIC int
sgp_recv_qptm_rels_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_rels_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_rels_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/* Release Indication */
STATIC int
sgp_recv_qptm_rels_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_qptm_rels_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_qptm_rels_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/*
 *  M2UA Received Messages
 */

/* M2UA_MAUP_ESTAB_REQ */

/**
 * sgp_recv_maup_estab_req: - process received M2UA_MAUP_ESTAB_REQ
 * @as: AS for which the message was received
 * @q: active queue (read queue)
 * @m: decoded M2UA message
 *
 * The M2UA_MAUP_ESTAB_REQ message should never be received from an SGP.
 */
STATIC int
sgp_recv_maup_estab_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/**
 * asp_recv_maup_estab_req: process recieved M2UA_MAUP_ESTAB_REQ
 * @as: AS for which the message was received
 * @q: active queue (read queue)
 * @m: decoded M2UA message
 */
STATIC int
asp_recv_maup_estab_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	dassert(as != NULL);

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		struct ss *ss;
		struct gp *gp;

		for (ss = ap_find_ssp(ap); ss; ss = ap_find_ssp_next(ap, ss, 1)) {
			switch (ss_get_s_state(ss)) {
			case SLS_WCON_RELREQ:
				/* The signalling link has failed and we are restarting it. */
				rare();
			case SLS_IDLE:
				if (!ss_tst_flags(ss, ASF_RETRIEVAL)) {
					if ((err = sl_start_req(ss, q)) < 0)
						return (err);
					gp_set_s_state(asp, SLS_WCON_EREQ);
					ss_set_s_state(ss, SLS_WCON_EREQ);
					return (QR_DONE);
				} else {
					/* Someone else is retrieving messages so we need to
					   indicate that the signalling link has failed (reason
					   will be unspecified). */
					if ((err = m2ua_send_maup_rel_ind(asp, q)) < 0)
						return (err);
					gp_set_s_state(asp, SLS_IDLE);
					return (QR_DONE);
				}
			case SLS_WCON_EREQ:
				/* The signalling link is already being established by some other
				   signalling link user. Wait for establishment confirmation. */
				gp_set_s_state(asp, SLS_WCON_EREQ);
				return (QR_DONE);
			case SLS_ESTABLISHED:
				/* The signalling link is already established by some signalling
				   link user. Indicate that the link is in service. */
				if ((err = m2ua_send_maup_estab_con(asp, q)) < 0)
					return (err);
				gp_set_s_state(asp, SLS_ESTABLISHED);
				return (QR_DONE);
			default:
				swerr();
				return (-EFAULT);
			}
		}
		for (gp = ap_find_sgp(ap); gp; gp = ap_find_sgp_next(ap, gp, 1)) {
			struct rp *rp;

			for (rp = gp->rp.list; rp; rp = rp->next) {

				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;

				switch (gp_get_s_state(gp)) {
				case SLS_WCON_RELREQ:
					/* The signalling link has failed and we are restarting it. 
					 */
					rare();
				case SLS_IDLE:
					if (!gp_tst_flags(gp, ASF_RETRIEVAL)) {
						struct xp = rp->pp.pp->xp.xp;

						if (canputnext(xp->oq)) {
							gp_set_s_state(asp, SLS_WCON_EREQ);
							gp_set_s_state(gp, SLS_WCON_EREQ);
							putnext(xp->oq, m->mp);
							return (QR_ABSORBED);
						}
						return (-EBUSY);
					} else {
						/* Someone else is retrieving messages so we need
						   to indicate that the signalling link has failed
						   (reason will be unspecified). */
						if ((err = m2ua_send_maup_rel_ind(asp, q)) < 0)
							return (err);
						gp_set_s_state(asp, SLS_IDLE);
						return (QR_DONE);
					}
				case SLS_WCON_EREQ:
					/* The signalling link is already being established by some 
					   other signalling link user. Wait for establishment
					   confirmation. */
					gp_set_s_state(asp, SLS_WCON_EREQ);
					return (QR_DONE);
				case SLS_ESTABLISHED:
					/* The signalling link is already established by some
					   signalling link user. Indicate that the link is in
					   service. */
					if ((err = m2ua_send_maup_estab_con(asp, q)) < 0)
						return (err);
					gp_set_s_state(asp, SLS_ESTABLISHED);
					return (QR_DONE);
				default:
					swerr();
					return (-EFAULT);
				}
				return (QR_DONE);
			}
		}
	}
	return (-EFAULT);
}

/**
 * spp_recv_maup_estab_req: - proces received M2UA_MAUP_ESTAB_REQ
 * @as: AS for which the message was received
 * @q: active queue (read queue)
 * @m: decoded M2UA message
 *
 * No M2UA messages are expected to be received on an SPP.
 */
STATIC int
spp_recv_maup_estab_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_ESTAB_CON:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_estab_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1))
			if (ss_get_s_state(ss) == SLS_WCON_EREQ) {
				if ((err = sl_in_service_ind(ss, q)))
					return (err);
				ss_set_s_state(ss, SLS_ESTABLISHED);
			}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1))
			if (gp_get_s_state(gp) == SLS_WCON_EREQ) {
				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					struct xp *xp = rp->pp.pp->xp.xp;

					if (rp_get_state(rp) < AS_WACK_ASPIA)
						continue;
					if (!canputnext(xp->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
					gp_set_s_state(gp, SLS_ESTABLISHED);
				}
			}
	}
	gp_set_s_state(sgp, SLS_ESTABLISHED);
	return (QR_DONE);
}
STATIC int
asp_recv_maup_estab_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
spp_recv_maup_estab_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_REL_REQ:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_rel_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
asp_recv_maup_rel_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		struct ss *ss;
		struct gp *gp;

		for (ss = ap_find_ssp(ap); ss; ss = ap_find_ssp_next(ap, ss, 1)) {
			switch (ss_get_s_state(ss)) {
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if ((err = sl_stop_req(ss, q)))
					return (err);
				/* no confirmation from SL-P */
				ss_set_s_state(ss, SLS_WCON_RELREQ);
				/* fall through */
			case SLS_WCON_RELREQ:
				ss_set_s_state(ss, SLS_IDLE);
				/* fall through */
			case SLS_IDLE:
				if ((err = m2ua_send_maup_rel_con(asp, q)))
					return (err);
				/* FIXME: All local signalling link users that believe the link to
				   be active must be notified with a link failure with unspecified
				   cause, or (better) a local processor outage. */
				ss_set_s_state(ss, SLS_IDLE);
				break;
			default:
				swerr();
				return (-EFAULT);
			}
			gp_set_s_state(asp, SLS_IDLE);
			return (QR_DONE);
		}
		for (gp = ap_find_sgp(ap); gp; gp = ap_find_sgp_next(ap, gp, 1)) {
			struct rp *rp;

			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;
				switch (gp_get_s_state(gp)) {
				case SLS_WCON_EREQ:
				case SLS_ESTABLISHED:
					struct xp *xp = rp->pp.pp->xp.xp;

					if (!canputnext(xp->oq))
						return (-EBUSY);
					putnext(xp->oq, m->mp);
					gp_set_s_state(gp, SLS_IDLE);
					break;
				case SLS_IDLE:
				case SLS_WCON_RELREQ:
					/* The signalling link has already been put out of service
					   by some other signalling link user or we can't put it
					   out of service. Just silently accept the request. */
					if ((err = m2ua_send_maup_rel_con(asp, q)))
						return (err);
					break;
				default:
					swerr();
					return (-EFAULT);
				}
				gp_set_s_state(asp, SLS_IDLE);
				return (QR_DONE);
			}
		}
	}
	return (-EFAULT);
}
STATIC int
spp_recv_maup_rel_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_REL_CON:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_rel_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	assure(gp_get_s_state(sgp) == SLS_WCON_RELREQ);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1))
			switch (ss_get_s_state(ss)) {
			case SLS_WCON_RELREQ:
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if ((err = sl_out_of_service_ind(ss, q, 0)))
					return (err);
				ss_set_s_state(ss, SLS_IDLE);
			}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1))
			switch (gp_get_s_state(gp)) {
			case SLS_WCON_RELREQ:
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					struct xp *xp = rp->pp.pp->xp.xp;

					if (!canputnext(xp->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
					gp_set_s_state(gp, SLS_IDLE);
					break;
				}
			}
	}
	gp_set_s_state(sgp, SLS_IDLE);
	return (QR_DONE);
}
STATIC int
asp_recv_maup_rel_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
spp_recv_maup_rel_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_REL_IND:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_rel_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	assure(gp_get_s_state(sgp) != SLS_IDLE);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1)) {
			if (!ss_tst_flags(ss, ASF_OPERATION_PENDING)) {
				if (ss_get_s_state(ss) != SLS_IDLE) {
					if ((err =
					     sl_out_of_service_ind(ss, q, SL_FAIL_UNSPECIFIED)))
						return (err);
					ss_set_s_state(ss, SLS_IDLE);
					ss_set_flags(ss, ASF_OPERATION_PENDING);
				}
			}
		}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1)) {
			if (!gp_tst_flags(gp, ASF_OPERATION_PENDING)) {
				if (gp_get_s_state(gp) != SLS_IDLE) {
					for (rp = gp->rp.list; rp; rp = rp->gp.next) {
						struct xp *xp = rp->pp.pp->xp.xp;

						if (rp_get_state(rp) < AS_WACK_ASPIA)
							continue;
						if (!canputnext(xp->oq))
							return (-EBUSY);
						if (!(bp = ss7_dupmsg(q, m->mp)))
							return (-ENOBUFS);
						putnext(xp->oq, bp);
						gp_set_s_state(gp, SLS_IDLE);
						gp_set_flags(gp, ASF_OPERATION_PENDING);
						break;
					}
				}
			}
		}
	}
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1))
			ss_clr_flags(ss, ASF_OPERATION_PENDING);
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1))
			gp_clr_flags(gp, ASF_OPERATION_PENDING);
	}
	gp_set_s_state(sgp, SLS_IDLE);
	return (QR_DONE);
}
STATIC int
asp_recv_maup_rel_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
spp_recv_maup_rel_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_STATE_REQ:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_state_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
asp_recv_maup_state_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		struct ss *ss;
		struct gp *gp;

		for (ss = ap_find_ssp(ap); ss; ss = ap_find_ssp_next(ap, ss, 1)) {
			if (!ss_tst_flags(ss, ASF_OPERATION_PENDING)) {
				switch (m->status.val) {
				case M2UA_STATUS_LPO_SET:
					/* FIXME: We can only set local processor outage at the
					   physical link if we are the only signalling link user. */
					if (!gp_tst_flags(asp, ASF_USR_PROC_OUTAGE)) {
						if (!ss_tst_flags(ss, ASF_USR_PROC_OUTAGE)) {
							if ((err =
							     sl_local_processor_outage_req(ss, q)))
								return (err);
							ss_set_flags(ss, ASF_USR_PROC_OUTAGE);
						}
						gp_set_flags(asp, ASF_USR_PROC_OUTAGE);
					}
					break;
				case M2UA_STATUS_LPO_CLEAR:
					/* FIXME: We can only clr local processor outage at the
					   physical link if we are the only signalling link user. */
					if (gp_tst_flags(asp, ASF_USR_PROC_OUTAGE)) {
						if (ss_tst_flags(ss, ASF_USR_PROC_OUTAGE)) {
							if ((err = sl_resume_req(ss, q)))
								return (err);
							ss_clr_flags(ss, ASF_USR_PROC_OUTAGE);
						}
						gp_clr_flags(asp, ASF_USR_PROC_OUTAGE);
					}
					break;
				case M2UA_STATUS_EMER_SET:
					if (!gp_tst_flags(asp, ASF_EMERGENCY)) {
						if (!ss_tst_flags(ss, ASF_EMERGENCY)) {
							if ((err = sl_emergency_req(ss, q)))
								return (err);
							ss_set_flags(ss, ASF_EMERGENCY);
						}
						gp_set_flags(asp, ASF_EMERGENCY);
					}
					break;
				case M2UA_STATUS_EMER_CLEAR:
					if (gp_tst_flags(asp, ASF_EMERGENCY)) {
						if (ss_tst_flags(ss, ASF_EMERGENCY)) {
							if ((err = sl_emergency_ceases_req(ss, q)))
								return (err);
							ss_clr_flags(ss, ASF_EMERGENCY);
						}
						gp_clr_flags(asp, ASF_EMERGENCY);
					}
					break;
				case M2UA_STATUS_FLUSH_BUFFERS:
					/* If the commend has already been effected, we simply set
					   our flag to indicate that we are interested in the
					   response. */
					if (!gp_tst_flags(asp, ASF_FLUSH_BUFFERS)) {
						if (!ss_tst_flags(ss, ASF_FLUSH_BUFFERS)) {
							if ((err = sl_clear_buffers_req(ss, q)))
								return (err);
							ss_set_flags(ss, ASF_FLUSH_BUFFERS);
						}
						gp_set_flags(asp, ASF_FLUSH_BUFFERS);
					}
					break;
				case M2UA_STATUS_CONTINUE:
					if (gp_tst_flags(asp, ASF_RECOVERY)) {
						if (ss_tst_flags(ss, ASF_RECOVERY)) {
							if ((err = sl_continue_req(ss, q)))
								return (err);
							ss_clr_flags(ss,
								     (ASF_RECOVERY |
								      ASF_FLUSH_BUFFERS |
								      ASF_CLEAR_RTB));
						}
						gp_clr_flags(asp,
							     (ASF_RECOVERY | ASF_FLUSH_BUFFERS |
							      ASF_CLEAR_RTB));
					}
					break;
				case M2UA_STATUS_CLEAR_RTB:
					/* If the commend has already been effected, we simply set
					   our flag to indicate that we are interested in the
					   response. */
					if (!gp_tst_flags(asp, ASF_CLEAR_RTB)) {
						if (!ss_tst_flags(ss, ASF_CLEAR_RTB)) {
							if ((err = sl_clear_rtb_req(ss, q)))
								return (err);
							ss_set_flags(ss, ASF_CLEAR_RTB);
						}
						gp_set_flags(asp, ASF_CLEAR_RTB);
					}
					break;
				case M2UA_STATUS_AUDIT:
					/* ignore for now */
					break;
				case M2UA_STATUS_CONG_CLEAR:
					if (gp_tst_flags(asp, (ASF_CONG_ACCEPT | ASF_CONG_DISCARD))) {
						if (ss_tst_flags
						    (ss, (ASF_CONG_ACCEPT | ASF_CONG_DISCARD))) {
							if ((err = sl_no_congestion_req(ss, q)))
								return (err);
							ss_clr_flags(ss,
								     (ASF_CONG_ACCEPT |
								      ASF_CONG_DISCARD));
						}
						gp_clr_flags(asp,
							     (ASF_CONG_ACCEPT | ASF_CONG_DISCARD));
					}
					break;
				case M2UA_STATUS_CONG_ACCEPT:
					if (!gp_tst_flags(asp, ASF_CONG_ACCEPT)) {
						if (!ss_tst_flags(ss, ASF_CONG_ACCEPT)) {
							if ((err = sl_congestion_accept_req(ss q)))
								return (err);
							ss_set_flags(ss, ASF_CONG_ACCEPT);
							ss_clr_flags(ss, ASF_CONG_DISCARD);
						}
						gp_set_flags(asp, ASF_CONG_ACCEPT);
						gp_clr_flags(asp, ASF_CONG_DISCARD);
					}
					break;
				case M2UA_STATUS_CONG_DISCARD:
					if (!gp_tst_flags(asp, ASF_CONG_DISCARD)) {
						if (!ss_tst_flags(ss, ASF_CONG_DISCARD)) {
							if ((err =
							     sl_congestion_discard_req(ss, q)))
								return (err);
							ss_set_flags(ss, ASF_CONG_DISCARD);
							ss_clr_flags(ss, ASF_CONG_ACCEPT);
						}
						gp_set_flags(asp, ASF_CONG_DISCARD);
						gp_clr_flags(asp, ASF_CONG_ACCEPT);
					}
					break;
				default:
					return (-EINVAL);
				}
				ss_set_flags(ss, ASF_OPERATION_PENDING);
			}
			switch (m->status.val) {
			case M2UA_STATUS_FLUSH_BUFFERS:
			case M2UA_STATUS_CLEAR_RTB:
				break;
			default:
			case M2UA_STATUS_LPO_SET:
			case M2UA_STATUS_LPO_CLEAR:
			case M2UA_STATUS_EMER_SET:
			case M2UA_STATUS_EMER_CLEAR:
			case M2UA_STATUS_CONTINUE:
			case M2UA_STATUS_AUDIT:
			case M2UA_STATUS_CONG_CLEAR:
			case M2UA_STATUS_CONG_ACCEPT:
			case M2UA_STATUS_CONG_DISCARD:
				if ((err = m2ua_send_maup_con(asp, q, m)))
					return (err);
				break;
			}
			ss_clr_flags(ss, ASF_OPERATION_PENDING);
			return (QR_DONE);
		}
		for (gp = ap_find_sgp(ap); gp; gp = ap_find_sgp_next(ap, gp, 1)) {
			struct rp *rp;

			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				struct xp *xp = rp->pp.pp->xp.xp;

				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;
				if (!canputnext(xp->oq))
					return (-EBUSY);
				putnext(xp->oq, m->mp);
				return (QR_DONE);
			}
		}
	}
	swerr();
	return (-EFAULT);
}
STATIC int
spp_recv_maup_state_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_STATE_CON:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_state_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1)) {
			switch (m->status.val) {
			case M2UA_STATUS_LPO_SET:
			case M2UA_STATUS_LPO_CLEAR:
				break;
			case M2UA_STATUS_FLUSH_BUFFERS:
				if (ss_tst_flags(ss, ASF_FLUSH_BUFFERS))
					if ((err = sl_rb_cleared_ind(ss, q)))
						return (err);
				break;
			case M2UA_STATUS_CLEAR_RTB:
				if (ss_tst_flags(ss, ASF_CLEAR_RTB))
					if ((err = sl_rtb_cleared_ind(ss, q)))
						return (err);
				break;
			case M2UA_STATUS_EMER_SET:
			case M2UA_STATUS_EMER_CLEAR:
				break;
			case M2UA_STATUS_CONTINUE:
				break;
			case M2UA_STATUS_AUDIT:
				break;
			case M2UA_STATUS_CONG_CLEAR:
			case M2UA_STATUS_CONG_ACCEPT:
			case M2UA_STATUS_CONG_DISCARD:
				break;
			default:
				return (-EINVAL);
			}
		}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1)) {
			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				struct xp *xp = rp->pp.pp->xp.xp;

				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;

				switch (m->status.val) {
				case M2UA_STATUS_LPO_SET:
				case M2UA_STATUS_LPO_CLEAR:
				case M2UA_STATUS_FLUSH_BUFFERS:
				case M2UA_STATUS_CLEAR_RTB:
				case M2UA_STATUS_EMER_SET:
				case M2UA_STATUS_EMER_CLEAR:
				case M2UA_STATUS_CONTINUE:
				case M2UA_STATUS_AUDIT:
				case M2UA_STATUS_CONG_CLEAR:
				case M2UA_STATUS_CONG_ACCEPT:
				case M2UA_STATUS_CONG_DISCARD:
					if (!canputnext(xp->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
					break;
				default:
					return (-EINVAL);
				}
				break;
			}
		}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_maup_state_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
spp_recv_maup_state_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_STATE_IND:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_state_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1)) {
			switch (m->m2ua.event.val) {
			case M2UA_EVENT_RPO_ENTER:
				if (ss_tst_flags(ss, ASF_REM_PROC_OUTAGE))
					continue;
				if ((err = sl_remote_processor_outage_ind(ss, q)))
					return (err);
				ss_set_flags(ss, ASF_REM_PROC_OUTAGE);
				continue;
			case M2UA_EVENT_RPO_EXIT:
				if (!ss_tst_flags(ss, ASF_REM_PROC_OUTAGE))
					continue;
				if ((err = sl_remote_processor_recovered_ind(ss, q)))
					return (err);
				ss_clr_flags(ss, ASF_REM_PROC_OUTAGE);
				continue;
			case M2UA_EVENT_LPO_ENTER:
				if (ss_tst_flags(ss, ASF_LOC_PROC_OUTAGE))
					continue;
				if ((err = sl_local_processor_outage_ind(ss, q)))
					return (err);
				ss_set_flags(ss, ASF_LOC_PROC_OUTAGE);
				continue;
			case M2UA_EVENT_LPO_EXIT:
				if (!ss_tst_flags(ss, ASF_LOC_PROC_OUTAGE))
					continue;
				if ((err = sl_local_processor_recovered_ind(ss, q)))
					return (err);
				ss_clr_flags(ss, ASF_LOC_PROC_OUTAGE);
				continue;
			default:
				return (-EINVAL);
			}
		}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1)) {
			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				struct xp *xp = rp->pp.pp->xp.xp;

				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;

				switch (m->m2ua.event.val) {
				case M2UA_EVENT_RPO_ENTER:
					if (ss_tst_flags(ss, ASF_REM_PROC_OUTAGE))
						continue;
					break;
				case M2UA_EVENT_RPO_EXIT:
					if (!ss_tst_flags(ss, ASF_REM_PROC_OUTAGE))
						continue;
					break;
				case M2UA_EVENT_LPO_ENTER:
					if (ss_tst_flags(ss, ASF_LOC_PROC_OUTAGE))
						continue;
					break;
				case M2UA_EVENT_LPO_EXIT:
					if (!ss_tst_flags(ss, ASF_LOC_PROC_OUTAGE))
						continue;
					break;
				default:
					return (-EINVAL);
				}
				if (!canputnext(xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(xp->oq, bp);
				break;
			}
		}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_maup_state_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
spp_recv_maup_state_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_RETR_REQ:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_retr_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
asp_recv_maup_retr_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	static const uint32_t action = __constant_htonl(M2UA_ACTION_RTRV_BSN);
	static const uint32_t result = __constant_htonl(UA_RESULT_FAILURE);

	if (!(as = asp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		struct ss *ss;
		struct gp *gp;

		for (ss = ap_find_ssp(ap); ss; ss = ap_find_ssp_next(ap, ss, 1)) {
			switch (m->m2ua.action.val) {
			case M2UA_ACTION_RTRV_BSN:
				/* If the commend has already been effected, we simply set our flag 
				   to indicate that we are interested in the response. */
				if (!ss_tst_flags(ss, ASF_BSNT_REQUEST)) {
					if ((err = sl_retrieve_bsnt_req(ss, q)))
						return (err);
					ss_set_flags(ss, ASF_BSNT_REQUEST);
				}
				gp_set_flags(asp, ASF_BSNT_REQUEST);
				break;
			case M2UA_ACTION_RTRV_MSGS:
				if (!m->m2ua.seqno.wp)
					return (-ENXIO);
				if (ss_get_s_state(ss) == SLS_IDLE
				    && !ss_tst_flags(ss, ASF_RETRIEVAL)) {
					if ((err =
					     sl_retrieval_request_and_fsnc_req(ss, q,
									       m->m2ua.seqno.val)))
						return (err);
					ss_set_flags(ss, ASF_RETRIEVAL);
					gp_set_flags(asp, ASF_RETRIEVAL);
				} else {
					if ((err =
					     m2ua_send_maup_retr_con(asp, q, action, result, NULL)))
						return (err);
				}
				break;
			default:
				return (-EINVAL);
			}
			return (QR_DONE);
		}
		for (gp = ap_find_sgp(ap); gp; gp = ap_find_sgp_next(ap, gp, 1)) {
			struct rp *rp;

			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				struct xp *xp = rp->pp.pp->xp.xp;

				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;
				if (!canputnext(xp->oq))
					return (-EBUSY);
				putnext(xp->oq, m->mp);
				return (QR_DONE);
			}
		}
	}
	swerr();
	return (-EFAULT);
}
STATIC int
spp_recv_maup_retr_req(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_RETR_CON:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_retr_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1)) {
			switch (m->m2ua.action.val) {
			case M2UA_ACTION_RTRV_BSN:
				if (!ss_tst_flags(ss, ASF_BSNT_REQUEST))
					continue;
				switch (m->result.val) {
				case UA_RESULT_SUCCESS:
					if (!m->m2ua.seqno.wp)
						return (-ENXIO);
					if ((err = sl_bsnt_ind(ss, q, m->m2ua.seqno.val)))
						return (err);
					break;
				case UA_RESULT_FAILURE:
					if ((err =
					     sl_bsnt_not_retrievable_ind(ss, q,
									 m->m2ua.seqno.cp ? m->m2ua.
									 seqno.val : 0)))
						return (err);
					break;
				default:
					return (-EINVAL);
				}
				ss_clr_flags(ss, ASF_BSNT_REQUEST);
				break;
			case M2UA_ACTION_RTRV_MSGS:
				if (!ss_tst_flags(ss, ASF_RETRIEVAL))
					continue;
				switch (m->result.val) {
				case UA_RESULT_SUCCESS:
					continue;
				case UA_RESULT_FAILURE:
					if ((err = sl_retrieval_not_possible_ind(ss, q)))
						return (err);
					break;
				}
				ss_clr_flags(ss, ASF_RETRIEVAL);
				break;
			default:
				return (-EINVAL);
			}
		}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1)) {
			struct rp *rp;

			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				struct xp *xp = rp->pp.pp->xp.xp;

				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;

				switch (m->m2ua.action.val) {
				case M2UA_ACTION_RTRV_BSN:
					if (!gp_tst_flags(gp, ASF_BSNT_REQUEST))
						continue;
					switch (m->result.val) {
					case UA_RESULT_SUCCESS:
						if (!m->m2ua.seqno.wp)
							return (-ENXIO);
						break;
					case UA_RESULT_FAILURE:
						break;
					default:
						return (-EINVAL);
					}
					if (!canputnext(xp->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
					gp_clr_flags(gp, ASF_BSNT_REQUEST);
					break;
				case M2UA_ACTION_RTRV_MSGS:
					if (!gp_tst_flags(gp, ASF_RETRIEVAL))
						continue;
					switch (m->result.val) {
					case UA_RESULT_SUCCESS:
						continue;
					case UA_RESULT_FAILURE:
						break;
					default:
						return (-EINVAL);
					}
					if (!canputnext(xp->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
					gp_clr_flags(gp, ASF_RETRIEVAL);
					break;
				default:
					return (-EINVAL);
				}
				break;
			}
		}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_maup_retr_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
spp_recv_maup_retr_con(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_RETR_IND:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_retr_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1))
			if (ss_tst_flags(ss, ASF_RETRIEVAL))
				if ((err = sl_retrieved_message_ind(ss, q, m)))
					return (err);
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1))
			if (gp_tst_flags(gp, ASF_RETRIEVAL)) {
				struct rp *rp;

				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					struct xp *xp = rp->pp.pp->xp.xp;

					if (!canputnext(xp->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
					break;
				}
			}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_maup_retr_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
spp_recv_maup_retr_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_RETR_COMP_IND:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_retr_comp_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1))
			if (ss_tst_flags(ss, ASF_RETRIEVAL)) {
				if ((err = sl_retrieval_complete_ind(ss, q, m)))
					return (err);
				ss_clr_flags(ss, ASF_RETRIEVAL);
			}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1))
			if (gp_tst_flags(sgp, ASF_RETRIEVAL)) {
				struct rp *rp;

				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					struct xp *xp = rp->pp.pp->xp.xp;

					if (rp_get_state(rp) < AS_WACK_ASPIA)
						continue;

					if (!canputnext(xp->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
					gp_clr_flags(sgp, ASF_RETRIEVAL);
					break;
				}
			}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_maup_retr_comp_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
spp_recv_maup_retr_comp_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_CONG_IND:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_cong_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	if (m->cong.val > 4 || m->disc.val > 4)
		return (-EINVAL);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1)) {
			if (ss_get_s_state(ss) == SLS_ESTABLISHED) {
				if (!m->cong.val && !m->disc.val) {
					if ((err =
					     sl_link_congestion_ceased_ind(ss, q, m->cong.val,
									   m->disc.val)))
						return (err);
				} else {
					if ((err =
					     sl_link_congested_ind(ss, q, m->cong.val,
								   m->disc.val)))
						return (err);
				}
			}
		}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1)) {
			if (gp_get_s_state(gp) == SLS_ESTABLISHED) {
				struct rp *rp;

				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					struct xp *xp = rp->pp.pp->xp.xp;

					if (rp_get_state(rp) < AS_WACK_ASPIA)
						continue;

					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
					break;
				}
			}
		}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_maup_cong_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
spp_recv_maup_cong_ind(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_DATA:
 *  -----------------------------------
 */
STATIC int
sgp_recv_maup_data(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	if ((m->m2ua.data1.wp && m->m2ua.data1.len < 4)
	    || (m->m2ua.data2.wp && m->m2ua.data2.len < 4))
		return (-EINVAL);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1)) {
			if (ss_get_s_state(ss) == SLS_ESTABLISHED) {
				if (!ss_tst_flags(ss, ASF_OPERATION_PENDING)) {
					if ((err = sl_pdu_ind(ss, q, m)))
						return (err);
					ss_set_flags(ss, ASF_OPERATION_PENDING);
				}
				if (m->corid.wp)
					if ((err = m2ua_send_maup_data_ack(sgp, q, m)))
						return (err);
				ss_clr_flags(ss, ASF_OPERATION_PENDING);
				return (QR_DONE);
			}
		}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1)) {
			if (gp_get_s_state(gp) == SLS_ESTABLISHED) {
				struct rp *rp;

				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					struct xp *xp = rp->pp.pp->xp.xp;

					if (rp_get_state(rp) < AS_WACK_ASPIA)
						continue;

					if (!canputnext(xp->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
					return (QR_DONE);
				}
			}
		}
	}
	return (-EFAULT);
}
STATIC int
asp_recv_maup_data(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	if (!(as = asp->as.as))
		return (-EFAULT);
	if ((m->m2ua.data1.wp && m->m2ua.data2.wp)
	    || ((m->m2ua.data1.wp && m->m2ua.data1.len < 4)
		|| (m->m2ua.data2.wp && m->m2ua.data2.len < 4)))
		return (-EINVAL);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		struct ss *ss;
		struct gp *gp;

		for (ss = ap_find_ssp(ap); ss; ss = ap_find_ssp_next(ap, ss, 1)) {
			if (ss_get_s_state(ss) == SLS_ESTABLISHED) {
				if (!ss_tst_flags(ss, ASF_OPERATION_PENDING)) {
					if ((err = sl_pdu_req(ss, q, m)))
						return (err);
					ss_set_flags(ss, ASF_OPERATION_PENDING);
				}
				if (m->corid.wp)
					if ((err = m2ua_send_maup_data_ack(asp, q, m)))
						return (err);
				ss_clr_flags(ss, ASF_OPERATION_PENDING);
				return (QR_DONE);
			}
		}
		for (gp = ap_find_sgp(ap); gp; gp = ap_find_sgp_next(ap, gp, 1)) {
			if (gp_get_s_state(gp) == SLS_ESTABLISHED) {
				struct rp *rp;

				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					struct xp *xp = rp->pp.pp->xp.xp;

					if (rp_get_state(rp) < AS_WACK_ASPIA)
						continue;

					if (!canputnext(xp->oq))
						return (-EBUSY);
					putnext(xp->oq, m->mp);
					return (QR_DONE);
				}
			}
		}
	}
	return (-EFAULT);
}
STATIC int
spp_recv_maup_data(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_DATA_ACK
 *  -----------------------------------
 */
STATIC int
slu_recv_maup_data_ack(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	/* 
	 *  TODO: What we could do here is walk an internal transmit buffer at the SL-U and strike the acknowledged
	 *  MSUs.  The purpose here is not retransmission but transmit buffer management.  This would give us some
	 *  back-pressure when SCTP congestion occurs and we can inform MTP of the congestion at Level 3.  This
	 *  means we would also have to track congestion indications from the link and determine when to send what
	 *  congestion indications upwards.
	 *
	 *  For now all we do is ignore the ack.
	 */
	return (QR_DONE);
}
STATIC int
slp_recv_maup_data_ack(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	/* 
	 *  Again what we could do is walk an internal receive buffer at the SL-P and strike the acknowledged MSUs.
	 *  The purpose here is not retransmission but receive buffer management.  This would give us some
	 *  back-pressure when SCTP congestion occurs and we can inform the link of the receive congestion.  This
	 *  means we would also have to track congestion requests to the link and determine when to send what
	 *  congestion requests downwards.
	 *
	 *  For now all we do is ignore the ack.
	 */
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_data_ack(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct gp *gp;
	struct ap *ap;
	struct ss *ss;
	mblk_t *bp;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ss = ap_find_ssu(ap); ss; ss = ap_find_ssu_next(ap, ss, 1)) {
			if ((err = slu_recv_maup_data_ack(ss, q, m)))
				return (err);
			return (QR_DONE);
		}
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 1)) {
			struct rp *rp;

			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				struct xp *xp = rp->pp.pp->xp.xp;

				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;

				if (!canputnext(xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(xp->oq, bp);
				return (QR_DONE);
			}
		}
	}
	return (-EFAULT);
}
STATIC int
asp_recv_maup_data_ack(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct as *as;
	struct ss *ssp;
	struct ap *ap;
	struct gp *sgp;

	if (!(as = asp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if ((err = slp_recv_maup_data_ack(ssp, q, m)))
				return (err);
			return (QR_DONE);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			struct xp *xp;

			if (!(xp = sgp->spp.spp->xp.xp))
				return (-EFAULT);
			if (!canputnext(xp->oq))
				return (-EBUSY);
			putnext(xp->oq, m->mp);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}
STATIC int
spp_recv_maup_data_ack(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 * ---------------------------------------------------------------------------
 *
 * SNMM messages.
 *
 * ---------------------------------------------------------------------------
 */

/**
 * sgp_recv_snmm_duna: - process received UA_SNMM_DUNA message
 * @as: AS for which the message was received.
 * @q: active queue (lower read queue)
 * @m: decoded M3UA message
 */
STATIC int
sgp_recv_snmm_duna(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	for (ap = as->ap.list; ap ap = ap->u.next) {
		if (!as_tst_flags(ap->u.as, ASF_ACTIVE))
			continue;
		{
			struct ss *ssu;
			struct mtp_addr addr;

			if (!m->apc.wp)
				goto skip_ssu;
			addr.pc = m->apc.val;
			addr.si = 0;
			addr.ni = 0;	/* FIXME */

			for (ssu = ap->u.as->ss.list; ssu; ssu = ssu->as.next) {
				if (!ss_tst_flags(ssu, ASF_ACTIVE))
					continue;
				if ((err = mtp_pause_ind(ss, q, (caddr_t) &addr, sizeof(addr))))
					return (err);
			}
		}
	      skip_ssu:
		{
			static const char info[] = "SGP propagated DUNA";
			struct gp *gp;
			struct rp *rp;

			for (gp = ap->u.as->gp.list; gp; gp = gp->as.next) {
				if (!gp_tst_flags(gp, ASF_ACTIVE))
					continue;
				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					if (!rp_tst_flags(rp, ASF_ACTIVE))
						continue;
					if ((err = ua_send_snmm_duna(rp, q,
								     m->asp.wp, m->apc.len >> 2,
								     NULL, NULL info,
								     sizeof(info))))
						return (err);
					break;	/* only send to one transport */
				}
			}
		}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_snmm_duna(struct as *as, queue_t *q, struct ua_msg *m)
{
	/* Unexpected in this direction. */
	return (-EPROTO);
}
STATIC int
spp_recv_snmm_duna(struct as *as, queue_t *q, struct ua_msg *m)
{
	/* Unexpected from IPSP. */
	return (-EPROTO);
}

/*
 * UA_SNMM_DAVA
 * ---------------------------------------------------------------------------
 */
STATIC int
sgp_recv_snmm_dava(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	dassert(as != NULL);

	for (ap = as->ap.list; ap; ap = ap->u.next) {
		if (!as_tst_flags(ap->u.as, ASF_ACTIVE))
			continue;
		{
			struct ss *ssu;
			struct mtp_addr addr;

			if (!m->apc.wp)
				goto skip_ssu;
			addr.pc = m->apc.val;
			addr.si = 0;
			addr.ni = 0;	/* FIXME */

			for (ssu = ap->u.as->ss.list; ssu; ssu = ssu->as.next) {
				if (!ss_tst_flags(ssu, ASF_ACTIVE))
					continue;
				if ((err = mtp_resume_ind(ss, q, (caddr_t) &addr, sizeof(addr))))
					return (err);
			}
		}
	      skip_ssu:
		{
			static const char info[] = "SGP propagated DAVA";
			struct gp *gp;
			struct rp *rp;

			for (gp = ap->u.as->gp.list; gp; gp = gp->as.next) {
				if (!gp_tst_flags(gp, ASF_ACTIVE))
					continue;
				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					if (!rp_tst_flasg(rp, ASF_ACTIVE))
						continue;
					if ((err = ua_send_snmm_dava(rp, q,
								     m->apc.wp, m->apc.len >> 2,
								     NULL, NULL, info,
								     sizeof(info))))
						return (err);
					break;	/* only send to one transport per SPP */
				}
			}
		}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_snmm_dava(struct as *as, queue_t *q, struct ua_msg *m)
{
	/* Unexpected in this direction. */
	return (-EPROTO);
}
STATIC int
spp_recv_snmm_dava(struct as *as, queue_t *q, struct ua_msg *m)
{
	/* Unexpected from IPSP. */
	return (-EPROTO);
}

/*
 * UA_SNMM_DAUD
 * ---------------------------------------------------------------------------
 */
STATIC int
sgp_recv_snmm_daud(struct as *as, queue_t *q, struct ua_msg *m)
{
	/* Unexpected in this direction. */
	return (-EPROTO);
}
STATIC int
asp_recv_snmm_daud(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_snmm_daud(struct as *as, queue_t *q, struct ua_msg *m)
{
	/* Unexpected for M3UA in this direction (but not SUA). */
	return (-EFAULT);
}

/*
 * UA_SNMM_SCON
 * ---------------------------------------------------------------------------
 */
STATIC int
sgp_recv_snmm_scon(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	if (!m->apc.wp)
		return (-ENXIO);	/* missing mandatory parameter */

	for (ap = as->ap.list; ap; ap = ap->u.next) {
		if (!as_tst_flags(ap->u.as, ASF_ACTIVE))
			continue;
		{
			struct ss *ssu;
			struct mtp_addr addr;

			addr.pc = m->apc.val;
			addr.si = 0;
			addr.ni = 0;	/* FIXME */

			for (ssu = ap->u.as->ss.list; ssu; ssu = ssu->as.next) {
				if (!ss_tst_flags(ssu, ASF_ACTIVE))
					continue;
				if ((err = mtp_status_ind(ss, q, (caddr_t) &addr, sizeof(addr),
							  MTP_STATUS_TYPE_CONG,
							  m->cong.wp ? m->cong.
							  val : MTP_STATUS_CONGESTION)))
					return (err);
			}
		}
		{
			static const char info[] = "SGP propagated SCON";
			struct gp *gp;
			struct rp *rp;

			for (gp = ap->u.as->gp.list; gp; gp = gp->as.next) {
				if (!gp_tst_flags(gp, ASF_ACTIVE))
					continue;
				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					if (!rp_tst_flags(rp, ASF_ACTIVE))
						continue;
					if ((err = ua_send_snmm_scon(rp, q,
								     m->cdest.wp, m->cong.wp,
								     m->apc.wp, m->apc.len >> 2,
								     m->ssn.wp, m->smi.wp, info,
								     sizeof(info))))
						return (err);
					break;	/* only send via one transport */
				}
			}
		}
	}
	return (-EFAULT);
}
STATIC int
asp_recv_snmm_scon(struct as *as, queue_t *q, struct ua_msg *m)
{
	/* Yes, SCON can be received from an ASP. */
	return (-EFAULT);
}
STATIC int
spp_recv_snmm_scon(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/*
 * UA_SNMM_DUPU
 * ---------------------------------------------------------------------------
 */
STATIC int
sgp_recv_snmm_dupu(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	if (!m->apc.wp)
		return (-ENXIO);	/* Missing mandatory parameter. */

	/* Normally there is only one AS-U (ASP Pool/IPSP) associated with any given AS-P
	   (SG/IPSP). */
	for (ap = as->ap.list; ap; ap = ap->u.next) {
		if (!as_tst_flags(ap->u.as, ASF_ACTIVE))
			continue;
		/* FIXME: this is M3UA specific. */
		{
			struct ss *ssu;
			struct mtp_addr addr;

			addr.pc = m->apc.val;
			addr.si = 0;
			addr.ni = 0;	/* FIXME */

			for (ssu = ap->u.as->ss.list; ssu; ssu = ssu->as.next) {
				if (!ss_tst_flags(ssu, ASF_ACTIVE))
					continue;
				if ((err = mtp_status_ind(ss, q, (caddr_t) &addr, sizeof(addr),
							  MTP_STATUS_TYPE_UPU, m->cause.val)))
					return (err);
			}
		}
		{
			static const char info[] = "SGP propagated DUPU";
			struct gp *gp;
			struct rp *rp;

			/* Note that each GP for an AS indicates the state in the AS of a different
			   SPP. */
			for (gp = ap->u.as->gp.list; gp; gp = gp->as.next) {
				if (!gp_tst_flags(gp, ASF_ACTIVE))
					continue;
				/* Note that each RP indicates the state in the AS of an SPP for a
				   transport (XP).  Only send to the first active transport
				   associated with the SPP to avoid delivering multiple messages
				   for the same indication. */
				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					if (!rp_tst_flags(rp, ASF_ACTIVE))
						continue;
					if ((err = ua_send_snmm_dupu(rp, q,
								     m->apc.wp, m->apc.len >> 2,
								     m->ssn.wp, m->smi.wp, info,
								     sizeof(info))))
						return (err);
					break;	/* only send via one transport */
				}
			}
		}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_snmm_dupu(struct as *as, queue_t *q, struct ua_msg *m)
{
	/* Unexpected in this direction.  If the ASP wishes to indicate UPU because the RK is
	   larger than an SI value, the ASP is welcome to sent UPU in a DATA message. */
	return (-EPROTO);
}
STATIC int
spp_recv_snmm_dupu(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/*
 * UA_SNMM_DRST
 * ---------------------------------------------------------------------------
 */
STATIC int
sgp_recv_snmm_drst(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	if (!m->apc.wp)
		return (-ENXIO);	/* Missing mandatory parameter. */

	/* Normally there is only one AS-U (ASP Pool/IPSP) associated with any given AS-P
	   (SG/IPSP). */
	for (ap = as->ap.list; ap; ap = ap->u.next) {
		if (!as_tst_flags(ap->u.as, ASF_ACTIVE))
			continue;
		/* FIXME: this is M3UA specific. */
		{
			struct ss *ssu;
			struct mtp_addr addr;

			addr.pc = m->apc.val;
			addr.si = 0;
			addr.ni = 0;	/* FIXME */

			for (ssu = ap->u.as->ss.list; ssu; ssu = ssu->as.next) {
				if (!ss_tst_flags(ssu, ASF_ACTIVE))
					continue;
				if ((err = mtp_status_ind(ss, q, (caddr_t) &addr, sizeof(addr),
							  MTP_STATUS_TYPE_RESTR, 0)))
					return (err);
			}
		}
		{
			static const char info[] = "SGP propagated DUPU";
			struct gp *gp;
			struct rp *rp;

			/* Note that each GP for an AS indicates the state in the AS of a different
			   SPP. */
			for (gp = ap->u.as->gp.list; gp; gp = gp->as.next) {
				if (!gp_tst_flags(gp, ASF_ACTIVE))
					continue;
				/* Note that each RP indicates the state in the AS of an SPP for a
				   transport (XP).  Only send to the first active transport
				   associated with the SPP to avoid delivering multiple messages
				   for the same indication. */
				for (rp = gp->rp.list; rp; rp = rp->gp.next) {
					if (!rp_tst_flags(rp, ASF_ACTIVE))
						continue;
					if ((err = ua_send_snmm_drst(rp, q,
								     m->apc.wp, m->apc.len >> 2,
								     m->ssn.wp, m->smi.wp, info,
								     sizeof(info))))
						return (err);
					break;	/* only send via one transport */
				}
			}
		}
	}
	return (QR_DONE);
}
STATIC int
asp_recv_snmm_drst(struct as *as, queue_t *q, struct ua_msg *m)
{
	/* Unexpected in this direction. */
	return (-EPROTO);
}
STATIC int
spp_recv_snmm_drst(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/*
 * ---------------------------------------------------------------------------
 *
 * M3UA Messages.
 *
 * ---------------------------------------------------------------------------
 */

/*
 * UA_XFER_DATA
 * ---------------------------------------------------------------------------
 */
/**
 * sgp_recv_xfer_data: - process received UA_XFER_DATA
 * @as: AS to which the message pertains
 * @q: active queue
 * @m: decoded M3UA message
 *
 * The message is to be passed to the SS-U or ASP.
 *
 */
STATIC int
sgp_recv_xfer_data(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	dassert(as != NULL);

	/* Find the first active AS-U serving this AS-P. */
	for (ap = as->ap.list; ap; ap = ap->p.next) {
		struct ss *ss;
		struct gp *gp;
		struct as *as_u = ap->u.as;

		if (!as_tst_flags(ap->u.as, (ASF_ACTIVE | ASF_PENDING)))
			continue;

		dassert(as_u != NULL);

		/* Find the first active SS-U (if any). */
		for (ss = as_u->ss.list; ss && !ss_tst_flags(ss, (ASF_ACTIVE | ASF_PENDING));
		     ss = ss->as.next) ;
		if (ss != NULL) {
			/* If there is a local SS-U, deliver the message there. */
			if ((err = mtp_transfer_ind(ss, q, m)))
				return (err);
			return (QR_DONE);
		}
		/* If there is no local SS-U, distribute the message to available ASPs according to
		   the traffic mode of the AS-U. ASPs are associated with SP-U structures.  We must
		   select ASPs (SP-U) that are active for the AS-U. */
		for (gp = ap_find_asp(ap); gp; gp = ap_find_asp_next(ap, gp, 0)) {
			struct rp *rp;

			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				struct xp *xp = rp->pp.pp->xp.xp;

				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;

				/* Well, no, we want to copy the message information and adjust it
				   accordingly and duplicate the data.  Use the m3ua_send_xfer_data
				   function instead. */

				if (!canputnext(xp->oq))
					return (-EBUSY);
				{
					mblk_t *bp;

					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(xp->oq, bp);
				}
				return (QR_DONE);
			}
		}
	}
	return (-EFAULT);
}

/**
 * asp_recv_xfer_data: - process received UA_XFER_DATA
 * @as: AS for which the message pertains
 * @q: active queue
 * @m: decoded M3UA message
 */
STATIC int
asp_recv_xfer_data(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/**
 * spp_recv_xfer_data: - process received UA_XFER_DATA
 * @as: AS for which the message pertains
 * @q: active queue
 * @m: decoded M3UA message
 */
STATIC int
spp_recv_xfer_data(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/*
 *  SUA Messages.
 */
STATIC int
sgp_recv_clns_cldt(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_clns_cldt(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_clns_cldt(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_clns_cldr(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_clns_cldr(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_clns_cldr(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

STATIC int
sgp_recv_cons_core(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_core(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_core(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_coak(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_coak(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_coak(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_coref(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_coref(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_coref(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_relre(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_relre(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_relre(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_relco(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_relco(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_relco(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_resco(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_resco(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_resco(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_resre(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_resre(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_resre(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_codt(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_codt(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_codt(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_coda(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_coda(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_coda(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_coerr(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_coerr(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_coerr(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_cons_coit(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_cons_coit(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_cons_coit(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/*
 *  TUA Messages.
 */

STATIC int
sgp_recv_tdhm_tuni(struct as *as, queue_t *q, struct ua_msg *m)
{
	int err;
	struct ap *ap;

	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 0)) {
		struct ss *ss;
		struct gp *gp;

		for (ss = ap_find_ssp(ap); ss; ss = ap_find_ssp_next(ap, ss, 0)) {
			if ((err = tc_uni_req(ss, q, m)))
				return (err);
			return (QR_DONE);
		}
		for (gp = ap_find_sgp(ap); gp; gp = ap_find_sgp_next(ap, gp, 0)) {
			struct rp *rp;

			for (rp = gp->rp.list; rp; rp = rp->gp.next) {
				struct xp *xp = rp->pp.pp->xp.xp;

				if (rp_get_state(rp) < AS_WACK_ASPIA)
					continue;
				if ((err = tua_send_tdhm_tuni(xp, q, m)))
					return (err);
				return (QR_DONE);
			}
		}
	}
	return (-EFAULT);
}
STATIC int
asp_recv_tdhm_tuni(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tdhm_tuni(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tdhm_tqry(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tdhm_tqry(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tdhm_tqry(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tdhm_tcnv(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tdhm_tcnv(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tdhm_tcnv(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tdhm_trsp(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tdhm_trsp(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tdhm_trsp(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tdhm_tuab(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tdhm_tuab(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tdhm_tuab(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tdhm_tpab(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tdhm_tpab(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tdhm_tpab(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tdhm_tnot(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tdhm_tnot(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tdhm_tnot(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

STATIC int
sgp_recv_tchm_cinv(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tchm_cinv(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tchm_cinv(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tchm_cres(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tchm_cres(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tchm_cres(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tchm_cerr(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tchm_cerr(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tchm_cerr(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tchm_crej(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tchm_crej(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tchm_crej(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
sgp_recv_tchm_ccan(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
asp_recv_tchm_ccan(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}
STATIC int
spp_recv_tchm_ccan(struct as *as, queue_t *q, struct ua_msg *m)
{
	return (-EFAULT);
}

/**
 * sgp_recv_teis: - process received UA_MGMT_TEIS message
 * @pp: SGP-XP associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded MGMT TEIS message
 */
static int
sgp_recv_mgmt_teis(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case IUA_MGMT_TEIS_REQ:
		return sgp_recv_mgmt_teis_req(pp, q, m);
	case IUA_MGMT_TEIS_CON:
		return sgp_recv_mgmt_teis_con(pp, q, m);
	case IUA_MGMT_TEIS_IND:
		return sgp_recv_mgmt_teis_ind(pp, q, m);
	case IUA_MGMT_TEIQ_REQ:
		return sgp_recv_mgmt_teiq_req(pp, q, m);
	}
	return (-EOPNOTSUPP);
}

/**
 * sgp_recv_mgmt: - process received UA_CLASS_MGMT message
 * @pp: SGP-XP assocated with the message
 * @q: active queue (lower read queue)
 * @m: decoded MGMT message
 */
static int
sgp_recv_mgmt(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_MGMT_ERR:
		return sgp_recv_mgmt_err(pp, q, m);
	case UA_MGMT_NTFY:
		return sgp_recv_mgmt_ntfy(pp, q, m);
	case IUA_MGMT_TEIS_REQ:
	case IUA_MGMT_TEIS_CON:
	case IUA_MGMT_TEIS_IND:
	case IUA_MGMT_TEIQ_REQ:
		return sgp_recv_mgmt_teis(pp, q, m);
	}
	return (-EOPNOTSUPP);
}

/**
 * sgp_recv_xfer: - process received UA_CLASS_XFER message
 * @as: AS associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded XFER message
 *
 * Transfer messages either require an RC value or there is only one AS.
 */
static streams_fastcall streams_inline __hot_in int
sgp_recv_xfer(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_XFER_DATA:
		return sgp_recv_xfer_data(as, q, m);
	default:
		return (-ENOPROTOOPT);
	}
}

STATIC int
ssu_proc_snmm_m3ua(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	struct mtp_addr addr;

	addr.pc = m->apc.val;
	addr.si = m->user.val;
	addr.ni = 0;		/* FIXME */

	switch (m->type) {
	case UA_SNMM_DUNA:
		/* depends on protocol level */
		return mtp_pause_ind(ssu, q, (caddr_t) &addr, sizeof(addr));
	case UA_SNMM_DAVA:
		/* depends on protocol level */
		return mtp_resume_ind(ssu, q, (caddr_t) &addr, sizeof(addr));
	case UA_SNMM_DAUD:
		/* Wrong direction. */
		return (-EPROTO);
	case UA_SNMM_SCON:
		/* depends on protocol level */
		return mtp_status_ind(ssu, q, (caddr_t) &addr, sizeof(addr),
				      MTP_STATUS_TYPE_CONG,
				      m->cong.wp ? m->cong.val : MTP_STATUS_CONGESTION);
	case UA_SNMM_DUPU:
		/* depends on protocol level */
		return mtp_status_ind(ssu, q, (caddr_t) &addr, sizeof(addr),
				      MTP_STATUS_TYPE_UPU, m->cause.val);
	case UA_SNMM_DRST:
		/* depends on protocol level */
		return mtp_status_ind(ssu, q, (caddr_t) &addr, sizeof(addr),
				      MTP_STATUS_TYPE_RESTR, 0);
	}
	return (-ENOPROTOOPT);
}

STATIC int
ssu_proc_snmm_sua(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	struct sccp_addr addr;

	return (-ENOPROTOOPT);
}

STATIC int
ssu_proc_snmm_tua(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	struct sccp_addr addr;

	return (-ENOPROTOOPT);
}

STATIC int
ssu_proc_snmm(struct ss *ss, queue_t *q, struct ua_msg *m)
{
	switch (ss->as.as->level) {
	case AS_LEVEL_M3UA:
		return ssu_proc_snmm_m3ua(ss, q, m);
	case AS_LEVEL_SUA:
		return ssu_proc_snmm_sua(ss, q, m);
	case AS_LEVEL_TUA:
		return ssu_proc_snmm_tua(ss, q, m);
	}
	return (-ENOPROTOOPT);
}

STATIC int
asp_proc_snmm(struct gp *asp, queue_t *q, struct ua_msg *m)
{
	static const char info[] = "SGP propagated SNMM";
	uint32_t *conp = m->cong.wp ? &m->cong.val : NULL;

	switch (m->type) {
	case UA_SNMM_DUNA:
		return ua_send_snmm_duna(rp, q,
					 m->apc.wp, m->apc.len >> 2,
					 m->ssn.wp, m->smi.wp, info, sizeof(info));
	case UA_SNMM_DAVA:
		return ua_send_snmm_dava(rp, q,
					 m->apc.wp, m->apc.len >> 2,
					 m->ssn.wp, m->smi.wp, info, sizeof(info));
	case UA_SNMM_DAUD:
		/* Wrong direction. */
		return (-EPROTO);
	case UA_SNMM_SCON:
		/* Never concerned destination for SGP generated SCON. */
		return ua_send_snmm_scon(rp, q, NULL, m->cong.wp,
					 m->apc.wp, m->apc.len >> 2,
					 m->ssn.wp, m->smi.wp, info, sizeof(info));
	case UA_SNMM_DUPU:
		return ua_send_snmm_dupu(rp, q,
					 m->apc.wp, m->apc.len >> 2,
					 m->ssn.wp, m->smi.wp, info, sizeof(info));
	case UA_SNMM_DRST:
		return ua_send_snmm_drst(rp, q,
					 m->apc.wp, m->apc.len >> 2,
					 m->ssn.wp, m->smi.wp, info, sizeof(info));
	default:
		return (-ENOPROTOOPT);
	}
}

/**
 * sgp_recv_snmm: - process received UA_CLASS_SNMM message
 * @as: AS associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded SNMM message
 *
 * Note that if the message is proxied to an ASP that we are serving, the new SNMM message will
 * contain the RC value corresponding to that AS.
 */
static int
sgp_recv_snmm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_SNMM_DUNA:
		return sgp_recv_snmm_duna(as, q, m);
	case UA_SNMM_DAVA:
		return sgp_recv_snmm_dava(as, q, m);
	case UA_SNMM_DAUD:
		return sgp_recv_snmm_daud(as, q, m);
	case UA_SNMM_SCON:
		return sgp_recv_snmm_scon(as, q, m);
	case UA_SNMM_DUPU:
		return sgp_recv_snmm_dupu(as, q, m);
	case UA_SNMM_DRST:
		return sgp_recv_snmm_drst(as, q, m);
	}
	return (-ENOPROTOOPT);
}

/**
 * sgp_recv_asps: - process received UA_CLASS_ASPS message
 * @pp: SGP-XP associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded ASPS message
 */
static int
sgp_recv_asps(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_ASPS_ASPUP_REQ:
		return sgp_recv_asps_aspup_req(pp, q, m);
	case UA_ASPS_ASPDN_REQ:
		return sgp_recv_asps_aspdn_req(pp, q, m);
	case UA_ASPS_HBEAT_REQ:
		return sgp_recv_asps_hbeat_req(pp, q, m);
	case UA_ASPS_ASPUP_ACK:
		return sgp_recv_asps_aspup_ack(pp, q, m);
	case UA_ASPS_ASPDN_ACK:
		return sgp_recv_asps_aspdn_ack(pp, q, m);
	case UA_ASPS_HBEAT_ACK:
		return sgp_recv_asps_hbeat_ack(pp, q, m);
	}
	return (-EOPNOTSUPP);
}

/**
 * sgp_recv_aspt: - process received UA_CLASS_ASPT message
 * @rp: AS/SGP/XP relation associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded ASPT message
 */
static int
sgp_recv_aspt(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_ASPT_ASPAC_REQ:
		return sgp_recv_aspt_aspac_req(rp, q, m);
	case UA_ASPT_ASPIA_REQ:
		return sgp_recv_aspt_aspia_req(rp, q, m);
	case UA_ASPT_ASPAC_ACK:
		return sgp_recv_aspt_aspac_ack(rp, q, m);
	case UA_ASPT_ASPIA_ACK:
		return sgp_recv_aspt_aspia_ack(rp, q, m);
	}
	return (-EOPNOTSUPP);
}

/**
 * sgp_recv_qptm: - process received UA_CLASS_QPTM message
 * @as: AS associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded QPTM message
 */
static int
sgp_recv_qptm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case IUA_QPTM_DATA_REQ:
		return sgp_recv_qptm_data_req(as, q, m);
	case IUA_QPTM_DATA_IND:
		return sgp_recv_qptm_data_ind(as, q, m);
	case IUA_QPTM_UDAT_REQ:
		return sgp_recv_qptm_udat_req(as, q, m);
	case IUA_QPTM_UDAT_IND:
		return sgp_recv_qptm_udat_ind(as, q, m);
	case IUA_QPTM_ESTB_REQ:
		return sgp_recv_qptm_estb_req(as, q, m);
	case IUA_QPTM_ESTB_CON:
		return sgp_recv_qptm_estb_con(as, q, m);
	case IUA_QPTM_ESTB_IND:
		return sgp_recv_qptm_estb_ind(as, q, m);
	case IUA_QPTM_RELS_REQ:
		return sgp_recv_qptm_rels_req(as, q, m);
	case IUA_QPTM_RELS_CON:
		return sgp_recv_qptm_rels_con(as, q, m);
	case IUA_QPTM_RESL_IND:
		return sgp_recv_qptm_rels_ind(as, q, m);
	}
	return (-ENOPROTOOPT);
}

/**
 * sgp_recv_maup: - process received UA_CLASS_MAUP message
 * @as: AS associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded MAUP message
 */
static int
sgp_recv_maup(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case M2UA_MAUP_ESTAB_REQ:
		return sgp_recv_maup_estab_req(as, q, m);
	case M2UA_MAUP_ESTAB_CON:
		return sgp_recv_maup_estab_con(as, q, m);
	case M2UA_MAUP_REL_REQ:
		return sgp_recv_maup_rel_req(as, q, m);
	case M2UA_MAUP_REL_CON:
		return sgp_recv_maup_rel_con(as, q, m);
	case M2UA_MAUP_REL_IND:
		return sgp_recv_maup_rel_ind(as, q, m);
	case M2UA_MAUP_STATE_REQ:
		return sgp_recv_maup_state_req(as, q, m);
	case M2UA_MAUP_STATE_CON:
		return sgp_recv_maup_state_con(as, q, m);
	case M2UA_MAUP_STATE_IND:
		return sgp_recv_maup_state_ind(as, q, m);
	case M2UA_MAUP_RETR_REQ:
		return sgp_recv_maup_retr_req(as, q, m);
	case M2UA_MAUP_RETR_CON:
		return sgp_recv_maup_retr_con(as, q, m);
	case M2UA_MAUP_RETR_IND:
		return sgp_recv_maup_retr_ind(as, q, m);
	case M2UA_MAUP_RETR_COMP_IND:
		return sgp_recv_maup_retr_comp_ind(as, q, m);
	case M2UA_MAUP_CONG_IND:
		return sgp_recv_maup_cong_ind(as, q, m);
	case M2UA_MAUP_DATA:
		return sgp_recv_maup_data(as, q, m);
	case M2UA_MAUP_DATA_ACK:
		return sgp_recv_maup_data_ack(as, q, m);
	}
	return (-EOPNOTSUPP);
}

/**
 * sgp_recv_cnls: - process received UA_CLASS_CNLS message
 * @as: AS associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded CNLS message
 */
static int
sgp_recv_cnls(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case SUA_CLNS_CLDT:
		return sgp_recv_clns_cldt(as, q, m);
	case SUA_CLNS_CLDR:
		return sgp_recv_clns_cldr(as, q, m);
	}
	return (-ENOPROTOOPT);
}

/**
 * sgp_recv_cons: - process received UA_CLASS_CONS message
 * @as: AS associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded CONS message
 */
static int
sgp_recv_cons(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case SUA_CONS_CORE:
		return sgp_recv_cons_core(as, q, m);
	case SUA_CONS_COAK:
		return sgp_recv_cons_coak(as, q, m);
	case SUA_CONS_COREF:
		return sgp_recv_cons_coref(as, q, m);
	case SUA_CONS_RELRE:
		return sgp_recv_cons_relre(as, q, m);
	case SUA_CONS_RELCO:
		return sgp_recv_cons_relco(as, q, m);
	case SUA_CONS_RESCO:
		return sgp_recv_cons_resco(as, q, m);
	case SUA_CONS_RESRE:
		return sgp_recv_cons_resre(as, q, m);
	case SUA_CONS_CODT:
		return sgp_recv_cons_codt(as, q, m);
	case SUA_CONS_CODA:
		return sgp_recv_cons_coda(as, q, m);
	case SUA_CONS_COERR:
		return sgp_recv_cons_coerr(as, q, m);
	case SUA_CONS_COIT:
		return sgp_recv_cons_coit(as, q, m);
	}
	return (-ENOPROTOOPT);
}

/**
 * sgp_recv_rkmm: - process received UA_CLASS_RKMM message
 * @pp: SGP-XP associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded RKMM message
 */
static int
sgp_recv_rkmm(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_RKMM_REG_REQ:
		return sgp_recv_rkmm_reg_req(pp, q, m);
	case UA_RKMM_REG_RSP:
		return sgp_recv_rkmm_reg_rsp(pp, q, m);
	case UA_RKMM_DEREG_REQ:
		return sgp_recv_rkmm_dereg_req(pp, q, m);
	case UA_RKMM_DEREG_RSP:
		return sgp_recv_rkmm_dereg_rsp(pp, q, m);
	}
	return (-EOPNOTSUPP);
}

/**
 * sgp_recv_tdhm: - process received UA_CLASS_TDHM message
 * @as: AS associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded TDHM message
 */
static int
sgp_recv_tdhm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case TUA_TDHM_TUNI:
		return sgp_recv_tdhm_tuni(as, q, m);
	case TUA_TDHM_TQRY:
		return sgp_recv_tdhm_tqry(as, q, m);
	case TUA_TDHM_TCNV:
		return sgp_recv_tdhm_tcnv(as, q, m);
	case TUA_TDHM_TRSP:
		return sgp_recv_tdhm_trsp(as, q, m);
	case TUA_TDHM_TUAB:
		return sgp_recv_tdhm_tuab(as, q, m);
	case TUA_TDHM_TPAB:
		return sgp_recv_tdhm_tpab(as, q, m);
	case TUA_TDHM_TNOT:
		return sgp_recv_tdhm_tnot(as, q, m);
	}
	return (-ENOPROTOOPT);
}

/**
 * sgp_recv_tchm: - process received UA_CLASS_TCHM message
 * @as: AS associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded TCHM message
 */
static int
sgp_recv_tchm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case TUA_TCHM_CINV:
		return sgp_recv_tchm_cinv(as, q, m);
	case TUA_TCHM_CRES:
		return sgp_recv_tchm_cres(as, q, m);
	case TUA_TCHM_CERR:
		return sgp_recv_tchm_cerr(as, q, m);
	case TUA_TCHM_CREJ:
		return sgp_recv_tchm_crej(as, q, m);
	case TUA_TCHM_CCAN:
		return sgp_recv_tchm_ccan(as, q, m);
	}
	return (-ENOPROTOOPT);
}

/**
 * sgp_recv_msg: - process UA message received from an SGP
 * @pp: SGP-XP associated with the message
 * @q: active queue (lower read queue)
 * @m: decoded UA message
 *
 * Selection procedures for receiving messages from an SGP:
 *
 * When a message is received at an ASP from an SGP association (XP-P) they are processed by message
 * class.  Messages that are associated with an Application Server are passed up the datastructures
 * in a manner depending on the mesage type and the traffic modes of the related application server.
 * This association (XP-P) is associated with a single local SGP instance (SPP-P).  The local SGP
 * instance (SPP-P) belongs to one SG (SP-P).  The ASP is active for one or more AS-P associated with
 * the SG, one to which the message must belong (by RC value).  For this AS-P there is a
 * corresponding AS-U (representing the AS at the local ASP) to which corresponds an SS-U (SS7
 * User) and any peer ASPs (SPP-U) that are also active for the AS-U.  If a local SS-U is active,
 * the message is converted in to SS7 Provider primitives and passed upstream from the upper multiplex
 * to the SS7 User.  If no local SS-U is active, the message is passed to the a peer ASP.  If no
 * peer ASP is active, the message is received in error (the AS-U is really inactive and the message
 * is discarded (or buffered)).  This entire selection process can be accomplised by passing the message to a
 * single function (after it has been determined that the message class and type qualitfies it for
 * this process).  Initial decoding of the message need only identify the message class and type,
 * the RC (IID) value(s), if any, and the load selection parameters (if any).  In fact demand
 * decoding can be performed past message class and type (except for data messages which need single
 * pass decoding for the fast path).
 */
static int
sgp_recv_msg(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->class) {
	case UA_CLASS_MGMT:	/* UA Management (MGMT) Message */
		return sgp_recv_mgmt(pp, q, mp);
	case UA_CLASS_SNMM:	/* Signalling Network Mgmt (SNM) Messages */
		return ua_recv_snmm(pp, q, m);
	case UA_CLASS_ASPS:	/* ASP State Maintenance (ASPSM) Messages */
		return sgp_recv_asps(pp, q, m);
	case UA_CLASS_ASPT:	/* ASP Traffic Maintenance (ASPTM) Messages */
		return ua_recv_aspt(pp, q, m);
	case UA_CLASS_RKMM:	/* Routing Key Management Messages */
		return sgp_recv_rkmm(pp, q, m);
	case UA_CLASS_XFER:	/* M3UA Data transfer message */
	case UA_CLASS_QPTM:	/* Q.931 User Part Messages */
	case UA_CLASS_MAUP:	/* M2UA Messages */
	case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
	case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
	case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
	case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
	{
		int err;
		struct rp *rp = NULL;
		struct as *as;

		if ((err = find_rp_pp_data_msg(&rp, pp, m)))
			return (err);
		if (rp_get_state(rp) < AS_WACK_ASPIA)
			return (QR_DONE);
		as = rp->gp.gp->as.as;

		switch (m->class) {
		case UA_CLASS_XFER:	/* M3UA Data transfer message */
			return sgp_recv_xfer(as, q, mp);
		case UA_CLASS_QPTM:	/* Q.921 User Part Messages */
			return sgp_recv_qptm(as, q, m);
		case UA_CLASS_MAUP:	/* M2UA Messages */
			return sgp_recv_maup(as, q, m);
		case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
			return sgp_recv_cnls(as, q, m);
		case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
			return sgp_recv_cons(as, q, m);
		case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
			return sgp_recv_tdhm(as, q, m);
		case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
			return sgp_recv_tchm(as, q, m);
		}
		swerr();
	}
	}
	return (-ENOPROTOOPT);
}

static
asp_recv_mgmt(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_MGMT_ERR:
		return asp_recv_mgmt_err(pp, q, m);
	case UA_MGMT_NTFY:
		return asp_recv_mgmt_ntfy(pp, q, m);
	case IUA_MGMT_TEIS_REQ:
		return asp_recv_mgmt_teis_req(pp, q, m);
	case IUA_MGMT_TEIS_CON:
		return asp_recv_mgmt_teis_con(pp, q, m);
	case IUA_MGMT_TEIS_IND:
		return asp_recv_mgmt_teis_ind(pp, q, m);
	case IUA_MGMT_TEIQ_REQ:
		return asp_recv_mgmt_teiq_req(pp, q, m);
	}
	return (-EOPNOTSUPP);
}

static
asp_recv_xfer(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_XFER_DATA:
		return asp_recv_xfer_data(as, q, m);
	}
	return (-ENOPROTOOPT);
}

static
asp_recv_snmm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_SNMM_DUNA:
		return asp_recv_snmm_duna(as, q, m);
	case UA_SNMM_DAVA:
		return asp_recv_snmm_dava(as, q, m);
	case UA_SNMM_DAUD:
		return asp_recv_snmm_daud(as, q, m);
	case UA_SNMM_SCON:
		return asp_recv_snmm_scon(as, q, m);
	case UA_SNMM_DUPU:
		return asp_recv_snmm_dupu(as, q, m);
	case UA_SNMM_DRST:
		return asp_recv_snmm_drst(as, q, m);
	}
	return (-ENOPROTOOPT);
}

static
asp_recv_asps(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_ASPS_ASPUP_REQ:
		return asp_recv_asps_aspup_req(pp, q, m);
	case UA_ASPS_ASPDN_REQ:
		return asp_recv_asps_aspdn_req(pp, q, m);
	case UA_ASPS_HBEAT_REQ:
		return asp_recv_asps_hbeat_req(pp, q, m);
	case UA_ASPS_ASPUP_ACK:
		return asp_recv_asps_aspup_ack(pp, q, m);
	case UA_ASPS_ASPDN_ACK:
		return asp_recv_asps_aspdn_ack(pp, q, m);
	case UA_ASPS_HBEAT_ACK:
		return asp_recv_asps_hbeat_ack(pp, q, m);
	}
	return (-EOPNOTSUPP);
}

static
asp_recv_aspt(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_ASPT_ASPAC_REQ:
		return asp_recv_aspt_aspac_req(rp, q, m);
	case UA_ASPT_ASPIA_REQ:
		return asp_recv_aspt_aspia_req(rp, q, m);
	case UA_ASPT_ASPAC_ACK:
		return asp_recv_aspt_aspac_ack(rp, q, m);
	case UA_ASPT_ASPIA_ACK:
		return asp_recv_aspt_aspia_ack(rp, q, m);
	}
	return (-EOPNOTSUPP);
}

static
asp_recv_qptm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case IUA_QPTM_DATA_REQ:
		return asp_recv_qptm_data_req(as, q, m);
	case IUA_QPTM_DATA_IND:
		return asp_recv_qptm_data_ind(as, q, m);
	case IUA_QPTM_UDAT_REQ:
		return asp_recv_qptm_udat_req(as, q, m);
	case IUA_QPTM_UDAT_IND:
		return asp_recv_qptm_udat_ind(as, q, m);
	case IUA_QPTM_ESTB_REQ:
		return asp_recv_qptm_estb_req(as, q, m);
	case IUA_QPTM_ESTB_CON:
		return asp_recv_qptm_estb_con(as, q, m);
	case IUA_QPTM_ESTB_IND:
		return asp_recv_qptm_estb_ind(as, q, m);
	case IUA_QPTM_RELS_REQ:
		return asp_recv_qptm_rels_req(as, q, m);
	case IUA_QPTM_RELS_CON:
		return asp_recv_qptm_rels_con(as, q, m);
	case IUA_QPTM_RESL_IND:
		return asp_recv_qptm_rels_ind(as, q, m);
	}
	return (-ENOPROTOOPT);
}

static
asp_recv_maup(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case M2UA_MAUP_ESTAB_REQ:
		return asp_recv_maup_estab_req(as, q, m);
	case M2UA_MAUP_ESTAB_CON:
		return asp_recv_maup_estab_con(as, q, m);
	case M2UA_MAUP_REL_REQ:
		return asp_recv_maup_rel_req(as, q, m);
	case M2UA_MAUP_REL_CON:
		return asp_recv_maup_rel_con(as, q, m);
	case M2UA_MAUP_REL_IND:
		return asp_recv_maup_rel_ind(as, q, m);
	case M2UA_MAUP_STATE_REQ:
		return asp_recv_maup_state_req(as, q, m);
	case M2UA_MAUP_STATE_CON:
		return asp_recv_maup_state_con(as, q, m);
	case M2UA_MAUP_STATE_IND:
		return asp_recv_maup_state_ind(as, q, m);
	case M2UA_MAUP_RETR_REQ:
		return asp_recv_maup_retr_req(as, q, m);
	case M2UA_MAUP_RETR_CON:
		return asp_recv_maup_retr_con(as, q, m);
	case M2UA_MAUP_RETR_IND:
		return asp_recv_maup_retr_ind(as, q, m);
	case M2UA_MAUP_RETR_COMP_IND:
		return asp_recv_maup_retr_comp_ind(as, q, m);
	case M2UA_MAUP_CONG_IND:
		return asp_recv_maup_cong_ind(as, q, m);
	case M2UA_MAUP_DATA_ACK:
		return asp_recv_maup_data_ack(as, q, m);
	case M2UA_MAUP_DATA:
		return asp_recv_maup_data(as, q, m);
	}
	return (-EOPNOTSUPP);
}

static
asp_recv_cnls(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case SUA_CLNS_CLDT:
		return asp_recv_clns_cldt(as, q, m);
	case SUA_CLNS_CLDR:
		return asp_recv_clns_cldr(as, q, m);
	}
	return (-ENOPROTOOPT);
}

static
asp_recv_cons(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case SUA_CONS_CORE:
		return asp_recv_cons_core(as, q, m);
	case SUA_CONS_COAK:
		return asp_recv_cons_coak(as, q, m);
	case SUA_CONS_COREF:
		return asp_recv_cons_coref(as, q, m);
	case SUA_CONS_RELRE:
		return asp_recv_cons_relre(as, q, m);
	case SUA_CONS_RELCO:
		return asp_recv_cons_relco(as, q, m);
	case SUA_CONS_RESCO:
		return asp_recv_cons_resco(as, q, m);
	case SUA_CONS_RESRE:
		return asp_recv_cons_resre(as, q, m);
	case SUA_CONS_CODT:
		return asp_recv_cons_codt(as, q, m);
	case SUA_CONS_CODA:
		return asp_recv_cons_coda(as, q, m);
	case SUA_CONS_COERR:
		return asp_recv_cons_coerr(as, q, m);
	case SUA_CONS_COIT:
		return asp_recv_cons_coit(as, q, m);
	}
	return (-ENOPROTOOPT);
}

static
asp_recv_rkmm(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_RKMM_REG_REQ:
		return asp_recv_rkmm_reg_req(pp, q, m);
	case UA_RKMM_REG_RSP:
		return asp_recv_rkmm_reg_rsp(pp, q, m);
	case UA_RKMM_DEREG_REQ:
		return asp_recv_rkmm_dereg_req(pp, q, m);
	case UA_RKMM_DEREG_RSP:
		return asp_recv_rkmm_dereg_rsp(pp, q, m);
	}
	return (-EOPNOTSUPP);
}

static
asp_recv_tdhm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case TUA_TDHM_TUNI:
		return asp_recv_tdhm_tuni(as, q, m);
	case TUA_TDHM_TQRY:
		return asp_recv_tdhm_tqry(as, q, m);
	case TUA_TDHM_TCNV:
		return asp_recv_tdhm_tcnv(as, q, m);
	case TUA_TDHM_TRSP:
		return asp_recv_tdhm_trsp(as, q, m);
	case TUA_TDHM_TUAB:
		return asp_recv_tdhm_tuab(as, q, m);
	case TUA_TDHM_TPAB:
		return asp_recv_tdhm_tpab(as, q, m);
	case TUA_TDHM_TNOT:
		return asp_recv_tdhm_tnot(as, q, m);
	}
	return (-ENOPROTOOPT);
}

static
asp_recv_tchm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case TUA_TCHM_CINV:
		return asp_recv_tchm_cinv(as, q, m);
	case TUA_TCHM_CRES:
		return asp_recv_tchm_cres(as, q, m);
	case TUA_TCHM_CERR:
		return asp_recv_tchm_cerr(as, q, m);
	case TUA_TCHM_CREJ:
		return asp_recv_tchm_crej(as, q, m);
	case TUA_TCHM_CCAN:
		return asp_recv_tchm_ccan(as, q, m);
	}
	return (-ENOPROTOOPT);
}

/**
 * asp_recv_msg: - process UA message received from an ASP
 * @pp: remote ASP/XP to which message pertains
 * @q: active queue (lower read queue)
 * @m: (partially) decoded UA message
 *
 * Selection procedures when receiving messagers from an ASP:
 *
 * WHen a message is received at an SGP from an ASP association (XP-U) they are processed by message
 * class.  Messages that are associated with an Application Server are passed down the datastructure
 * in a manner depending on the message type.  The association (XP-U) is associated with a single
 * local ASP association (SPP-U).  The local ASP association (SPP-U) belongs to one ASP (SP-U).  The
 * ASP is active for one or more AS-U associated with the ASP, one to which the message must belong
 * (by RC value).  For this AS-U there is one corresponding AS-P (representing the local SG at the
 * local SGP) to which corresponds an SS-P (SS7 Provider) and any peer SGP (SPP-P) that are also
 * active for the AS-P.  if a local SS-P is active, the message is converted into SS7 User
 * primitives and passed downstream to the peer from the lower multiplex to the SS7 Provider.  If no
 * local SS-P is active, the message is passed to a peer SGP (SSP-P).  If no peer SSP is active, the
 * message is received in error (the AS-P is really inactive and the message is discarded (or
 * buffered)).  The peer SPP can be selected by the traffic mode of the SG (SP-P).
 */
static int
asp_recv_msg(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->class) {
	case UA_CLASS_MGMT:	/* UA Management (MGMT) Message */
		return asp_recv_mgmt(pp, q, m);
	case UA_CLASS_SNMM:	/* Signalling Network Mgmt (SNM) Messages */
		return ua_recv_snmm(pp, q, m);
	case UA_CLASS_ASPS:	/* ASP State Maintenance (ASPSM) Messages */
		return asp_recv_asps(pp, q, m);
	case UA_CLASS_ASPT:	/* ASP Traffic Maintenance (ASPTM) Messages */
		return ua_recv_aspt(pp, q, m);
	case UA_CLASS_RKMM:	/* Routing Key Management Messages */
		return asp_recv_rkmm(pp, q, m);
	case UA_CLASS_XFER:	/* M3UA Data transfer message */
	case UA_CLASS_QPTM:	/* Q.931 User Part Messages */
	case UA_CLASS_MAUP:	/* M2UA Messages */
	case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
	case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
	case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
	case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
	{
		int err;
		struct rp *rp = NULL;
		struct as *as;

		if ((err = find_rp_pp_data_msg(&rp, pp, m)))
			return (err);
		if (rp_get_state(rp) < AS_WACK_ASPIA)
			return (QR_DONE);
		as = rp->gp.gp->as.as;

		switch (m->class) {
		case UA_CLASS_XFER:	/* M3UA Data transfer message */
			return asp_recv_xfer(as, q, m);
		case UA_CLASS_QPTM:	/* Q.931 User Part Messages */
			return asp_recv_qptm(as, q, m);
		case UA_CLASS_MAUP:	/* M2UA Messages */
			return asp_recv_maup(as, q, m);
		case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
			return asp_recv_cnls(as, q, m);
		case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
			return asp_recv_cons(as, q, m);
		case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
			return asp_recv_tdhm(as, q, m);
		case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
			return asp_recv_tchm(as, q, m);
		}
		swerr();
	}
	}
	return (-ENOPROTOOPT);
}

static int
spp_recv_mgmt(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_MGMT_ERR:
		return spp_recv_mgmt_err(pp, q, m);
	case UA_MGMT_NTFY:
		return spp_recv_mgmt_ntfy(pp, q, m);
	case IUA_MGMT_TEIS_REQ:
		return spp_recv_mgmt_teis_req(pp, q, m);
	case IUA_MGMT_TEIS_CON:
		return spp_recv_mgmt_teis_con(pp, q, m);
	case IUA_MGMT_TEIS_IND:
		return spp_recv_mgmt_teis_ind(pp, q, m);
	case IUA_MGMT_TEIQ_REQ:
		return spp_recv_mgmt_teiq_req(pp, q, m);
	}
	return (-EOPNOTSUPP);
}
static int
spp_recv_xfer(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_XFER_DATA:
		return spp_recv_xfer_data(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
spp_recv_snmm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_SNMM_DUNA:
		return spp_recv_snmm_duna(as, q, m);
	case UA_SNMM_DAVA:
		return spp_recv_snmm_dava(as, q, m);
	case UA_SNMM_DAUD:
		return spp_recv_snmm_daud(as, q, m);
	case UA_SNMM_SCON:
		return spp_recv_snmm_scon(as, q, m);
	case UA_SNMM_DUPU:
		return spp_recv_snmm_dupu(as, q, m);
	case UA_SNMM_DRST:
		return spp_recv_snmm_drst(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
spp_recv_asps(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_ASPS_ASPUP_REQ:
		return spp_recv_asps_aspup_req(pp, q, m);
	case UA_ASPS_ASPDN_REQ:
		return spp_recv_asps_aspdn_req(pp, q, m);
	case UA_ASPS_HBEAT_REQ:
		return spp_recv_asps_hbeat_req(pp, q, m);
	case UA_ASPS_ASPUP_ACK:
		return spp_recv_asps_aspup_ack(pp, q, m);
	case UA_ASPS_ASPDN_ACK:
		return spp_recv_asps_aspdn_ack(pp, q, m);
	case UA_ASPS_HBEAT_ACK:
		return spp_recv_asps_hbeat_ack(pp, q, m);
	}
	return (-EOPNOTSUPP);
}
static int
spp_recv_aspt(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_ASPT_ASPAC_REQ:
		return spp_recv_aspt_aspac_req(rp, q, m);
	case UA_ASPT_ASPIA_REQ:
		return spp_recv_aspt_aspia_req(rp, q, m);
	case UA_ASPT_ASPAC_ACK:
		return spp_recv_aspt_aspac_ack(rp, q, m);
	case UA_ASPT_ASPIA_ACK:
		return spp_recv_aspt_aspia_ack(rp, q, m);
	}
	return (-EOPNOTSUPP);
}
static int
spp_recv_qptm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case IUA_QPTM_DATA_REQ:
		return spp_recv_qptm_data_req(as, q, m);
	case IUA_QPTM_DATA_IND:
		return spp_recv_qptm_data_ind(as, q, m);
	case IUA_QPTM_UDAT_REQ:
		return spp_recv_qptm_udat_req(as, q, m);
	case IUA_QPTM_UDAT_IND:
		return spp_recv_qptm_udat_ind(as, q, m);
	case IUA_QPTM_ESTB_REQ:
		return spp_recv_qptm_estb_req(as, q, m);
	case IUA_QPTM_ESTB_CON:
		return spp_recv_qptm_estb_con(as, q, m);
	case IUA_QPTM_ESTB_IND:
		return spp_recv_qptm_estb_ind(as, q, m);
	case IUA_QPTM_RELS_REQ:
		return spp_recv_qptm_rels_req(as, q, m);
	case IUA_QPTM_RELS_CON:
		return spp_recv_qptm_rels_con(as, q, m);
	case IUA_QPTM_RESL_IND:
		return spp_recv_qptm_rels_ind(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
spp_recv_maup(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case M2UA_MAUP_ESTAB_REQ:
		return spp_recv_maup_estab_req(as, q, m);
	case M2UA_MAUP_ESTAB_CON:
		return spp_recv_maup_estab_con(as, q, m);
	case M2UA_MAUP_REL_REQ:
		return spp_recv_maup_rel_req(as, q, m);
	case M2UA_MAUP_REL_CON:
		return spp_recv_maup_rel_con(as, q, m);
	case M2UA_MAUP_REL_IND:
		return spp_recv_maup_rel_ind(as, q, m);
	case M2UA_MAUP_STATE_REQ:
		return spp_recv_maup_state_req(as, q, m);
	case M2UA_MAUP_STATE_CON:
		return spp_recv_maup_state_con(as, q, m);
	case M2UA_MAUP_STATE_IND:
		return spp_recv_maup_state_ind(as, q, m);
	case M2UA_MAUP_RETR_REQ:
		return spp_recv_maup_retr_req(as, q, m);
	case M2UA_MAUP_RETR_CON:
		return spp_recv_maup_retr_con(as, q, m);
	case M2UA_MAUP_RETR_IND:
		return spp_recv_maup_retr_ind(as, q, m);
	case M2UA_MAUP_RETR_COMP_IND:
		return spp_recv_maup_retr_comp_ind(as, q, m);
	case M2UA_MAUP_CONG_IND:
		return spp_recv_maup_cong_ind(as, q, m);
	case M2UA_MAUP_DATA_ACK:
		return spp_recv_maup_data_ack(as, q, m);
	case M2UA_MAUP_DATA:
		return spp_recv_maup_data(as, q, m);
	}
	return (-EOPNOTSUPP);
}
static int
spp_recv_cnls(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case SUA_CLNS_CLDT:
		return spp_recv_clns_cldt(as, q, m);
	case SUA_CLNS_CLDR:
		return spp_recv_clns_cldr(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
spp_recv_cons(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case SUA_CONS_CORE:
		return spp_recv_cons_core(as, q, m);
	case SUA_CONS_COAK:
		return spp_recv_cons_coak(as, q, m);
	case SUA_CONS_COREF:
		return spp_recv_cons_coref(as, q, m);
	case SUA_CONS_RELRE:
		return spp_recv_cons_relre(as, q, m);
	case SUA_CONS_RELCO:
		return spp_recv_cons_relco(as, q, m);
	case SUA_CONS_RESCO:
		return spp_recv_cons_resco(as, q, m);
	case SUA_CONS_RESRE:
		return spp_recv_cons_resre(as, q, m);
	case SUA_CONS_CODT:
		return spp_recv_cons_codt(as, q, m);
	case SUA_CONS_CODA:
		return spp_recv_cons_coda(as, q, m);
	case SUA_CONS_COERR:
		return spp_recv_cons_coerr(as, q, m);
	case SUA_CONS_COIT:
		return spp_recv_cons_coit(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
spp_recv_rkmm(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_RKMM_REG_REQ:
		return spp_recv_rkmm_reg_req(pp, q, m);
	case UA_RKMM_REG_RSP:
		return spp_recv_rkmm_reg_rsp(pp, q, m);
	case UA_RKMM_DEREG_REQ:
		return spp_recv_rkmm_dereg_req(pp, q, m);
	case UA_RKMM_DEREG_RSP:
		return spp_recv_rkmm_dereg_rsp(pp, q, m);
	}
	return (-EOPNOTSUPP);
}
static int
spp_recv_tdhm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case TUA_TDHM_TUNI:
		return spp_recv_tdhm_tuni(as, q, m);
	case TUA_TDHM_TQRY:
		return spp_recv_tdhm_tqry(as, q, m);
	case TUA_TDHM_TCNV:
		return spp_recv_tdhm_tcnv(as, q, m);
	case TUA_TDHM_TRSP:
		return spp_recv_tdhm_trsp(as, q, m);
	case TUA_TDHM_TUAB:
		return spp_recv_tdhm_tuab(as, q, m);
	case TUA_TDHM_TPAB:
		return spp_recv_tdhm_tpab(as, q, m);
	case TUA_TDHM_TNOT:
		return spp_recv_tdhm_tnot(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
spp_recv_tchm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case TUA_TCHM_CINV:
		return spp_recv_tchm_cinv(as, q, m);
	case TUA_TCHM_CRES:
		return spp_recv_tchm_cres(as, q, m);
	case TUA_TCHM_CERR:
		return spp_recv_tchm_cerr(as, q, m);
	case TUA_TCHM_CREJ:
		return spp_recv_tchm_crej(as, q, m);
	case TUA_TCHM_CCAN:
		return spp_recv_tchm_ccan(as, q, m);
	}
	return (-ENOPROTOOPT);
}

static int
spp_recv_msg(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->class) {
	case UA_CLASS_MGMT:	/* UA Management (MGMT) Message */
		return spp_recv_mgmt(pp, q, mp);
	case UA_CLASS_SNMM:	/* Signalling Network Mgmt (SNM) Messages */
		return ua_recv_snmm(pp, q, m);
	case UA_CLASS_ASPS:	/* ASP State Maintenance (ASPSM) Messages */
		return spp_recv_asps(pp, q, mp);
	case UA_CLASS_ASPT:	/* ASP Traffic Maintenance (ASPTM) Messages */
		return ua_recv_aspt(pp, q, mp);
	case UA_CLASS_RKMM:	/* Routing Key Management Messages */
		return spp_recv_rkmm(pp, q, mp);
	case UA_CLASS_XFER:	/* M3UA Data transfer message */
	case UA_CLASS_QPTM:	/* Q.931 User Part Messages */
	case UA_CLASS_MAUP:	/* M2UA Messages */
	case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
	case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
	case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
	case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
	{
		int err;
		struct rp *rp = NULL;
		struct as *as;

		if ((err = find_rp_pp_data_msg(&rp, pp, m)))
			return (err);
		if (rp_get_state(rp) < AS_WACK_ASPIA)
			return (QR_DONE);
		as = rp->gp.gp->as.as;

		switch (m->class) {
		case UA_CLASS_XFER:	/* M3UA Data transfer message */
			return spp_recv_xfer(as, q, mp);
		case UA_CLASS_QPTM:	/* Q.931 User Part Messages */
			return spp_recv_qptm(as, q, mp);
		case UA_CLASS_MAUP:	/* M2UA Messages */
			return spp_recv_maup(as, q, mp);
		case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
			return spp_recv_cnls(as, q, mp);
		case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
			return spp_recv_cons(as, q, mp);
		case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
			return spp_recv_tdhm(as, q, mp);
		case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
			return spp_recv_tchm(as, q, mp);
		}
		swerr();
	}
	}
	return (-ENOPROTOOPT);
}

static int
xgp_recv_mgmt(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_MGMT_ERR:
		return xgp_recv_mgmt_err(pp, q, m);
	case UA_MGMT_NTFY:
		return xgp_recv_mgmt_ntfy(pp, q, m);
	case IUA_MGMT_TEIS_REQ:
		return xgp_recv_mgmt_teis_req(pp, q, m);
	case IUA_MGMT_TEIS_CON:
		return xgp_recv_mgmt_teis_con(pp, q, m);
	case IUA_MGMT_TEIS_IND:
		return xgp_recv_mgmt_teis_ind(pp, q, m);
	case IUA_MGMT_TEIQ_REQ:
		return xgp_recv_mgmt_teiq_req(pp, q, m);
	}
	return (-EOPNOTSUPP);
}
static int
xgp_recv_xfer(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_XFER_DATA:
		return xgp_recv_xfer_data(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
xgp_recv_snmm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_SNMM_DUNA:
		return xgp_recv_snmm_duna(as, q, m);
	case UA_SNMM_DAVA:
		return xgp_recv_snmm_dava(as, q, m);
	case UA_SNMM_DAUD:
		return xgp_recv_snmm_daud(as, q, m);
	case UA_SNMM_SCON:
		return xgp_recv_snmm_scon(as, q, m);
	case UA_SNMM_DUPU:
		return xgp_recv_snmm_dupu(as, q, m);
	case UA_SNMM_DRST:
		return xgp_recv_snmm_drst(as, q, m);
	}
	return (-ENOPROTOOPT);
}

/**
 * xgp_recv_asps: - process ASPS message on XGP
 *
 * When an XGP receives an ASP Up or ASP Up Ack message they are treated the same, ASP Down and ASP
 * Down Ack are the same as well.  When receiving an ASP Up or ASP Up Ack, all AS configured are
 * moved to the ASP Inactive state for the the XGP, which (if the associated local providers are not
 * present) may result in one or more AS-P moving to the ASP Inactive state, resulting (possibly) in
 * one or more AS-U moving to the ASP Inactive state which may subsequently cause notifications and
 * unsolicited ASP Inactive Acks to be sent to the ASPs served by the SGP.
 */
static int
xgp_recv_asps(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_ASPS_ASPUP_REQ:
		return xgp_recv_asps_aspup_req(pp, q, m);
	case UA_ASPS_ASPDN_REQ:
		return xgp_recv_asps_aspdn_req(pp, q, m);
	case UA_ASPS_HBEAT_REQ:
		return xgp_recv_asps_hbeat_req(pp, q, m);
	case UA_ASPS_ASPUP_ACK:
		return xgp_recv_asps_aspup_ack(pp, q, m);
	case UA_ASPS_ASPDN_ACK:
		return xgp_recv_asps_aspdn_ack(pp, q, m);
	case UA_ASPS_HBEAT_ACK:
		return xgp_recv_asps_hbeat_ack(pp, q, m);
	}
	return (-EOPNOTSUPP);
}

/**
 * xgp_recv_aspt: - process ASPT message on XGP
 *
 * When an XGP receives an ASP Active or ASP Active Ack message they are treated the differently,
 * ASP Inactive and ASP Inactive Ack are different as well.  When receiving an ASP Active, the XGP
 * is moved to the active state for the associated AS-P which may result in further state changes.
 * When receiving an ASP Active Ack, if the local provider is unavailable, an ASP Inactive is sent,
 * otherwise the ASP Active Ack is ignored.
 */
static int
xgp_recv_aspt(struct rp *rp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_ASPT_ASPAC_REQ:
		return xgp_recv_aspt_aspac_req(rp, q, m);
	case UA_ASPT_ASPIA_REQ:
		return xgp_recv_aspt_aspia_req(rp, q, m);
	case UA_ASPT_ASPAC_ACK:
		return xgp_recv_aspt_aspac_ack(rp, q, m);
	case UA_ASPT_ASPIA_ACK:
		return xgp_recv_aspt_aspia_ack(rp, q, m);
	}
	return (-EOPNOTSUPP);
}
static int
xgp_recv_qptm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case IUA_QPTM_DATA_REQ:
		return xgp_recv_qptm_data_req(as, q, m);
	case IUA_QPTM_DATA_IND:
		return xgp_recv_qptm_data_ind(as, q, m);
	case IUA_QPTM_UDAT_REQ:
		return xgp_recv_qptm_udat_req(as, q, m);
	case IUA_QPTM_UDAT_IND:
		return xgp_recv_qptm_udat_ind(as, q, m);
	case IUA_QPTM_ESTB_REQ:
		return xgp_recv_qptm_estb_req(as, q, m);
	case IUA_QPTM_ESTB_CON:
		return xgp_recv_qptm_estb_con(as, q, m);
	case IUA_QPTM_ESTB_IND:
		return xgp_recv_qptm_estb_ind(as, q, m);
	case IUA_QPTM_RELS_REQ:
		return xgp_recv_qptm_rels_req(as, q, m);
	case IUA_QPTM_RELS_CON:
		return xgp_recv_qptm_rels_con(as, q, m);
	case IUA_QPTM_RESL_IND:
		return xgp_recv_qptm_rels_ind(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
xgp_recv_maup(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case M2UA_MAUP_ESTAB_REQ:
		return xgp_recv_maup_estab_req(as, q, m);
	case M2UA_MAUP_ESTAB_CON:
		return xgp_recv_maup_estab_con(as, q, m);
	case M2UA_MAUP_REL_REQ:
		return xgp_recv_maup_rel_req(as, q, m);
	case M2UA_MAUP_REL_CON:
		return xgp_recv_maup_rel_con(as, q, m);
	case M2UA_MAUP_REL_IND:
		return xgp_recv_maup_rel_ind(as, q, m);
	case M2UA_MAUP_STATE_REQ:
		return xgp_recv_maup_state_req(as, q, m);
	case M2UA_MAUP_STATE_CON:
		return xgp_recv_maup_state_con(as, q, m);
	case M2UA_MAUP_STATE_IND:
		return xgp_recv_maup_state_ind(as, q, m);
	case M2UA_MAUP_RETR_REQ:
		return xgp_recv_maup_retr_req(as, q, m);
	case M2UA_MAUP_RETR_CON:
		return xgp_recv_maup_retr_con(as, q, m);
	case M2UA_MAUP_RETR_IND:
		return xgp_recv_maup_retr_ind(as, q, m);
	case M2UA_MAUP_RETR_COMP_IND:
		return xgp_recv_maup_retr_comp_ind(as, q, m);
	case M2UA_MAUP_CONG_IND:
		return xgp_recv_maup_cong_ind(as, q, m);
	case M2UA_MAUP_DATA_ACK:
		return xgp_recv_maup_data_ack(as, q, m);
	case M2UA_MAUP_DATA:
		return xgp_recv_maup_data(as, q, m);
	}
	return (-EOPNOTSUPP);
}
static int
xgp_recv_cnls(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case SUA_CLNS_CLDT:
		return xgp_recv_clns_cldt(as, q, m);
	case SUA_CLNS_CLDR:
		return xgp_recv_clns_cldr(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
xgp_recv_cons(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case SUA_CONS_CORE:
		return xgp_recv_cons_core(as, q, m);
	case SUA_CONS_COAK:
		return xgp_recv_cons_coak(as, q, m);
	case SUA_CONS_COREF:
		return xgp_recv_cons_coref(as, q, m);
	case SUA_CONS_RELRE:
		return xgp_recv_cons_relre(as, q, m);
	case SUA_CONS_RELCO:
		return xgp_recv_cons_relco(as, q, m);
	case SUA_CONS_RESCO:
		return xgp_recv_cons_resco(as, q, m);
	case SUA_CONS_RESRE:
		return xgp_recv_cons_resre(as, q, m);
	case SUA_CONS_CODT:
		return xgp_recv_cons_codt(as, q, m);
	case SUA_CONS_CODA:
		return xgp_recv_cons_coda(as, q, m);
	case SUA_CONS_COERR:
		return xgp_recv_cons_coerr(as, q, m);
	case SUA_CONS_COIT:
		return xgp_recv_cons_coit(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
xgp_recv_rkmm(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_RKMM_REG_REQ:
		return xgp_recv_rkmm_reg_req(pp, q, m);
	case UA_RKMM_REG_RSP:
		return xgp_recv_rkmm_reg_rsp(pp, q, m);
	case UA_RKMM_DEREG_REQ:
		return xgp_recv_rkmm_dereg_req(pp, q, m);
	case UA_RKMM_DEREG_RSP:
		return xgp_recv_rkmm_dereg_rsp(pp, q, m);
	}
	return (-EOPNOTSUPP);
}
static int
xgp_recv_tdhm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case TUA_TDHM_TUNI:
		return xgp_recv_tdhm_tuni(as, q, m);
	case TUA_TDHM_TQRY:
		return xgp_recv_tdhm_tqry(as, q, m);
	case TUA_TDHM_TCNV:
		return xgp_recv_tdhm_tcnv(as, q, m);
	case TUA_TDHM_TRSP:
		return xgp_recv_tdhm_trsp(as, q, m);
	case TUA_TDHM_TUAB:
		return xgp_recv_tdhm_tuab(as, q, m);
	case TUA_TDHM_TPAB:
		return xgp_recv_tdhm_tpab(as, q, m);
	case TUA_TDHM_TNOT:
		return xgp_recv_tdhm_tnot(as, q, m);
	}
	return (-ENOPROTOOPT);
}
static int
xgp_recv_tchm(struct as *as, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case TUA_TCHM_CINV:
		return xgp_recv_tchm_cinv(as, q, m);
	case TUA_TCHM_CRES:
		return xgp_recv_tchm_cres(as, q, m);
	case TUA_TCHM_CERR:
		return xgp_recv_tchm_cerr(as, q, m);
	case TUA_TCHM_CREJ:
		return xgp_recv_tchm_crej(as, q, m);
	case TUA_TCHM_CCAN:
		return xgp_recv_tchm_ccan(as, q, m);
	}
	return (-ENOPROTOOPT);
}

static int
find_rp_pp_data_msg(struct rp **rpp, struct pp *pp, struct ua_msg *m)
{
	struct rp *rp = NULL;

	/* For data messages, either the message contains an IID or RC, or there must be only one
	   AS (the default AS) configured for the SPP-XP. */
	if (m->rc.wp) {
		if (m->rc.len != sizeof(uint32_t))
			return (-EMSGSIZE);	/* Need one and only one RC. */
		/* Find rp associated with RC for SPP-XP. */
		for (rp = pp->rp.list; rp && rp->gp.gp->as.as->rc != m->rc.val; rp = rp->pp.next) ;
	} else if (m->iid.wp) {
		if (m->iid.len != sizeof(uint32_t))
			return (-EMSGSIZE);	/* Need one and only one IID. */
		/* Find rp associated with IID for SPP-XP. */
		for (rp = pp->rp.list; rp && rp->gp.gp->as.as->iid != m->iid.val;
		     rp = rp->pp.next) ;
	} else if (m->iid_text.cp) {
		for (rp = pp->rp.list;
		     rp && strncmp(m->iid_text.cp, rp->gp.gp->as.as->iid_text, m->iid_text.len);
		     rp = rp->pp.next) ;
	} else {
		/* Message applies to default AS, must have one and only one default AS configured. 
		 */
		if (!(rp = pp->rp.list) || rp->pp.next)
			return (-EXDEV);
	}
	if (!rp)
		return (-EIO);
	*rpp = rp;
	return (0);
}

/*
 * XGP receive is a little different than the others.  This is used for provider cross-links between
 * SGP belonging to the same SG.  When we receive data transfer messages on an XGP we perform a
 * local provider selection only and send the message out (to avoid loop).  XGP are like inverted
 * SPPs (which always send data messages to the user) in that they always send data messages to the
 * provider.  Asside from that they are like SSP with regards to management messages.
 */
static int
xgp_recv_msg(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	switch (m->class) {
	case UA_CLASS_MGMT:	/* UA Management (MGMT) Message */
		return xgp_recv_mgmt(pp, q, mp);
	case UA_CLASS_SNMM:	/* Signalling Network Mgmt (SNM) Messages */
		/* Handles RC a little bit different from data messages. */
		return ua_recv_snmm(pp, q, m);
	case UA_CLASS_ASPS:	/* ASP State Maintenance (ASPSM) Messages */
		return xgp_recv_asps(pp, q, mp);
	case UA_CLASS_ASPT:	/* ASP Traffic Maintenance (ASPTM) Messages */
		/* Handles RC (IID) a little bit different from data messages. */
		return ua_recv_aspt(pp, q, mp);
	case UA_CLASS_RKMM:	/* Routing Key Management Messages */
		return xgp_recv_rkmm(pp, q, mp);
	case UA_CLASS_XFER:	/* M3UA Data transfer message */
	case UA_CLASS_QPTM:	/* Q.931 User Part Messages */
	case UA_CLASS_MAUP:	/* M2UA Messages */
	case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
	case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
	case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
	case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
	{
		int err;
		struct rp *rp = NULL;
		struct as *as;

		if ((err = find_rp_pp_data_msg(&rp, pp, m)))
			return (err);
		if (rp_get_state(rp) < AS_WACK_ASPIA)
			return (QR_DONE);
		as = rp->gp.gp->as.as;

		switch (m->class) {
		case UA_CLASS_XFER:	/* M3UA Data transfer message */
			return xgp_recv_xfer(as, q, mp);
		case UA_CLASS_QPTM:	/* Q.931 User Part Messages */
			return xgp_recv_qptm(as, q, mp);
		case UA_CLASS_MAUP:	/* M2UA Messages */
			return xgp_recv_maup(as, q, mp);
		case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
			return xgp_recv_cnls(as, q, mp);
		case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
			return xgp_recv_cons(as, q, mp);
		case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
			return xgp_recv_tdhm(as, q, mp);
		case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
			return xgp_recv_tchm(as, q, mp);
		}
		swerr();
	}
	}
	return (-ENOPROTOOPT);
}

STATIC int
ua_recv_aspt(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	int err, num_rc = 0, processed = 0;
	uint32_t *wp = NULL;
	unsigned char *cp = NULL;
	bool ranging = false;
	struct rp *rp;

	/* ASPT messages are processed individually for each RC contained in the message.  If no RC
	   is contained in the message and there is only one AS for the SGP, the message is
	   processed for that AS.  If there is more than one AS for the SGP, the error no default AS
	   for ASP is returned.  The RC (IID) value zero has no special meaning here. */
	if ((wp = m->rc.wp)) {
		if (m->rc.len < sizeof(uint32_t)
		    || (m->rc.len & (sizeof(uint32_t) - 1)))
			return (-EMSGSIZE);
		num_rc = m->rc.len >> 2;
	} else if ((wp = m->iid.wp)) {
		if (m->iid.len < sizeof(uint32_t)
		    || (m->iid.len & (sizeof(uint32_t) - 1)))
			return (-EMSGSIZE);
		num_rc = m->iid.len >> 2;
	} else if ((wp = m->iid_range.wp)) {
		if (m->iid_range.len < (sizeof(uint32_t) << 1)
		    || (m->iid_range & ((sizeof(uint32_t) << 1) - 1)))
			return (-EMSGSIZE);
		num_rc = m->iid.len >> 2;
		ranging = true;
	} else if ((cp = m->iid_text.cp)) {
		num_rc = 1;
	}
	for (rp = pp->rp.list; rp; rp = rp->pp.next) {
		int i;
		struct as *as = rp->gp.gp->as.as;

		for (i = 0; i < num_rc; i++) {
			if (wp) {
				if (ranging) {
					if (as->rc >= ntohl(wp[i]) && as->rc <= htohl(wp[i + 1]))
						break;
					i++;
				} else {
					if (as->rc == ntohl(wp[i]))
						break;
				}
			} else if (cp) {
				if (as->iid_text && !strncmp(as->iid_text, cp, m->iid_text.len))
					break;
			} else {
				swerr();
				return (-EFAULT);
			}
		}
		if (i && i >= num_rc)
			continue;
		if (!num_rc && rp->pp.next)
			return (-EXDEV);	/* No default AS for ASP. */
		switch (pp->spp.spp->type) {
		case UA_OBJ_TYPE_ASP:
			err = asp_recv_aspt(rp, q, m);
			break;
		case UA_OBJ_TYPE_SGP:
			err = sgp_recv_aspt(rp, q, m);
			break;
		case UA_OBJ_TYPE_SPP:
			err = spp_recv_aspt(rp, q, m);
			break;
		case UA_OBJ_TYPE_XGP:
			err = xgp_recv_aspt(rp, q, m);
			break;
		default:
			swerr();
			err = -EFAULT;
		}
		if (err < 0)
			return (err);
		processed++;
		if (!ranging && processed >= num_rc)
			break;
	}
	if (!ranging && processed < num_rc)
		return (-EIO);	/* Invalid RC value in list. */
	return (QR_DONE);
}

STATIC int
ua_recv_snmm(struct pp *pp, queue_t *q, struct ua_msg *m)
{
	int err, num_rc = 0, processed = 0;
	uint32_t *wp = NULL;
	struct rp *rp;

	/* SNMM messages are processed individually for each RC contained in the message. If no RC
	   is contained in the message, and there is only one AS for the SGP, the message is
	   processed for that AS.  If there is more than one AS for the SGP, it is possible to send 
	   an ERROR message indicating that an RC is required (no default AS), however, we assume
	   here that it corresponds to all AS.  We also consider the special RC value of zero to
	   indicate all AS (wildcarded RC value) as well. */
	if ((wp = m->rc.wp)) {
		if (m->rc.len < sizeof(uint32_t)
		    || (m->rc.len & (sizeof(uint32_t) - 1)))
			return (-EMSGSIZE);
		num_rc = m->rc.len >> 2;
		/* Treat special value of zero to mean all RC (for SUA). */
		if (num_rc == 1 && m->rc.val == 0)
			num_rc = 0;
	}
	/* TODO: Currently we sequentially search all AS and match on RC value.  This linear search 
	   does not scale.  A hash mechanism for AS will be necessary if the number of AS gets
	   large (as it could for SUA or TUA). */
	for (rp = pp->rp.list; rp; rp = rp->pp.next) {
		int i;
		struct as *as = NULL;

		for (i = 0; i < num_rc; i++)
			if ((as = rp->gp.gp->as.as)->rc == ntohl(wp[i]))
				break;
		if (i && i >= num_rc)
			continue;
		switch (pp->spp.spp->type) {
		case UA_OBJ_TYPE_ASP:
			err = asp_recv_snmm(as, q, mp);
			break;
		case UA_OBJ_TYPE_SGP:
			err = sgp_recv_snmm(as, q, mp);
			break;
		case UA_OBJ_TYPE_SPP:
			err = spp_recv_snmm(as, q, mp);
			break;
		case UA_OBJ_TYPE_XGP:
			err = xgp_recv_snmm(as, q, mp);
			break;
		default:
			swerr();
			err = -EFAULT;
		}
		if (err < 0)
			return (err);
		processed++;
	}
	if (processed < num_rc)
		return (-EIO);	/* Invalid RC value in list. */
	return (QR_DONE);
}

STATIC int
sp_recv_mgmt(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_MGMT_ERR:
		return sp_recv_mgmt_err(xp, q, m);
	case UA_MGMT_NTFY:
		return (-EPROTO);	/* Unexpected in this state. */
	}
	return (-EOPNOTSUPP);	/* Unsupported message type. */
}

STATIC int
sp_recv_asps(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_ASPS_ASPUP_REQ:
		return sp_recv_asps_aspup_req(xp, q, m);
	case UA_ASPS_ASPDN_REQ:
		return sp_recv_asps_aspdn_req(xp, q, m);
	case UA_ASPS_HBEAT_REQ:
	case UA_ASPS_ASPUP_ACK:
	case UA_ASPS_ASPDN_ACK:
	case UA_ASPS_HBEAT_ACK:
		return (-EPROTO);	/* Unexpected in this state. */
	}
	return (-EOPNOTSUPP);	/* Unsupported message type. */
}

STATIC int
sp_recv_msg(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	switch (m->class) {
	case UA_CLASS_MGMT:
		return sp_recv_mgmt(xp, q, m);
	case UA_CLASS_ASPS:
		return sp_recv_asps(xp, q, m);
	case UA_CLASS_SNMM:
	case UA_CLASS_ASPT:
	case UA_CLASS_RKMM:
	case UA_CLASS_XFER:
	case UA_CLASS_QPTM:
	case UA_CLASS_MAUP:
	case UA_CLASS_CNLS:
	case UA_CLASS_CONS:
	case UA_CLASS_TDHM:
	case UA_CLASS_TCHM:
		return (-EPROTO);	/* Unexpected in this state. */
	}
	return (-ENOPROTOOPT);	/* Bad message class. */
}

STATIC int
sg_recv_mgmt(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	switch (m->type) {
	case UA_MGMT_ERR:
		return sg_recv_mgmt_err(xp, q, m);
	case UA_MGMT_NTFY:
		return (-EPROTO);	/* Unexpected in this state. */
	}
	return (-EOPNOTSUPP);	/* Unsupported message type. */
}

STATIC int
sg_recv_msg(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	switch (m->class) {
	case UA_CLASS_MGMT:
		return sg_recv_mgmt(xp, q, m);
	case UA_CLASS_ASPS:
	case UA_CLASS_SNMM:
	case UA_CLASS_ASPT:
	case UA_CLASS_RKMM:
	case UA_CLASS_XFER:
	case UA_CLASS_QPTM:
	case UA_CLASS_MAUP:
	case UA_CLASS_CNLS:
	case UA_CLASS_CONS:
	case UA_CLASS_TDHM:
	case UA_CLASS_TCHM:
		return (-EPROTO);	/* Unexpected in this state. */
	}
	return (-ENOPROTOOPT);	/* Bad message class. */
}

static int
xs_recv_msg(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	if (m->aspid.cp == NULL)
		return (-EXDEV);

	switch (m->class) {
	case UA_CLASS_MGMT:
	case UA_CLASS_ASPS:
		switch (m->type) {
		case UA_ASPS_ASPUP_REQ:
			return xs_recv_asps_aspup_req(xp, q, m);
		case UA_ASPS_ASPDN_REQ:
		case UA_ASPS_HBEAT_REQ:
		case UA_ASPS_ASPUP_ACK:
		case UA_ASPS_ASPDN_ACK:
		case UA_ASPS_HBEAT_ACK:
			return (-EPROTO);	/* Unexpected in this state. */
		}
		return (-EOPNOTSUPP);	/* Unsupported message type. */
	case UA_CLASS_SNMM:
	case UA_CLASS_ASPT:
	case UA_CLASS_RKMM:
	case UA_CLASS_XFER:
	case UA_CLASS_QPTM:
	case UA_CLASS_MAUP:
	case UA_CLASS_CNLS:
	case UA_CLASS_CONS:
	case UA_CLASS_TDHM:
	case UA_CLASS_TCHM:
		return (-EPROTO);	/* Unexpected in this state. */
	}
	return (-ENOPROTOOPT);	/* Bad message class. */
}

static int
xg_recv_msg(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	if (m->aspid.cp == NULL)
		return (-EXDEV);

	switch (m->class) {
	case UA_CLASS_MGMT:
	case UA_CLASS_ASPS:
		switch (m->type) {
		case UA_ASPS_ASPUP_REQ:
			return xg_recv_asps_aspup_req(xp, q, m);
		case UA_ASPS_ASPDN_REQ:
		case UA_ASPS_HBEAT_REQ:
		case UA_ASPS_ASPUP_ACK:
		case UA_ASPS_ASPDN_ACK:
		case UA_ASPS_HBEAT_ACK:
			return (-EPROTO);	/* Unexpected in this state. */
		}
		return (-EOPNOTSUPP);	/* Unsupported message type. */
	case UA_CLASS_SNMM:
	case UA_CLASS_ASPT:
	case UA_CLASS_RKMM:
	case UA_CLASS_XFER:
	case UA_CLASS_QPTM:
	case UA_CLASS_MAUP:
	case UA_CLASS_CNLS:
	case UA_CLASS_CONS:
	case UA_CLASS_TDHM:
	case UA_CLASS_TCHM:
		return (-EPROTO);	/* Unexpected in this state. */
	}
	return (-ENOPROTOOPT);	/* Bad message class. */
}

static int
xp_recv_msg(struct xp *xp, queue_t *q, struct ua_msg *m)
{
	if (m->aspid.cp == NULL)
		return (-EXDEV);

	switch (m->class) {
	case UA_CLASS_MGMT:
	case UA_CLASS_ASPS:
		switch (m->type) {
		case UA_ASPS_ASPUP_REQ:
			return xp_recv_asps_aspup_req(xp, q, m);
		case UA_ASPS_ASPDN_REQ:
		case UA_ASPS_HBEAT_REQ:
		case UA_ASPS_ASPUP_ACK:
		case UA_ASPS_ASPDN_ACK:
		case UA_ASPS_HBEAT_ACK:
			return (-EPROTO);	/* Unexpected in this state. */
		}
		return (-EOPNOTSUPP);	/* Unsupported message type. */
	case UA_CLASS_SNMM:
	case UA_CLASS_ASPT:
	case UA_CLASS_RKMM:
	case UA_CLASS_XFER:
	case UA_CLASS_QPTM:
	case UA_CLASS_MAUP:
	case UA_CLASS_CNLS:
	case UA_CLASS_CONS:
	case UA_CLASS_TDHM:
	case UA_CLASS_TCHM:
		return (-EPROTO);	/* Unexpected in this state. */
	}
	return (-ENOPROTOOPT);	/* Bad message class. */
}

STATIC int
ua_recv_msg(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err, uaerr = 0;
	struct sp *sp;
	struct pp *pp;
	struct ua_msg msg;

	/* SCTP always delivers messages to us as single data blocks so this should never be
	   needed. If some module in between fragments data blocks, this will handle the problem
	   once, here. */

	if ((err = ss7_pullupmsg(q, mp, -1)))
		return (err);

	if ((err = ua_dec_msg(xp, q, mp, &msg)) >= 0) {
		/* XP nodes can be associated with multiple SPP nodes.  If an ASP Id is contained
		   in the message, we look for an SPP that matches the ASP Id.  If there is no ASP
		   Id in the message, the list must contain one and only one SPP structure. */
		if (msp.aspid.cp)
			for (pp = xp->pp.list; pp && pp->spp.spp->aspid != msg.aspid.val;
			     pp = pp->xp.next) ;
		else
			if ((pp = xp->pp.list) && pp->xp.next)
			/* No ASP Id in the message, the SPP list must contain exactly one or zero
			   SPP or it is an error. */
			return (-EXDEV);	/* Invalid ASP Id. */
		if (pp) {
			switch (xp->type) {
			case UA_OBJ_TYPE_XP_ASP:
				err = asp_recv_msg(pp, q, &msg);
				break;
			case UA_OBJ_TYPE_XP_SGP:
				err = sgp_recv_msg(pp, q, &msg);
				break;
			case UA_OBJ_TYPE_XP_SPP:
				err = spp_recv_msg(pp, q, &msg);
				break;
			case UA_OBJ_TYPE_XP_XSP:
				/* SGP and SPP messages from a specific SGP or SPP on an ASP X-link
				 * are treated normally.  ASP messages from the peer ASP on the
				 * X-link are treated specially. */
				switch (pp->spp.spp->type) {
				case UA_OBJ_TYPE_ASP:
					err = xsp_recv_msg(pp, q, &msg);
					break;
				case UA_OBJ_TYPE_SGP:
					err = sgp_recv_msg(pp, q, &msg);
					break;
				case UA_OBJ_TYPE_SPP:
					err = spp_recv_msg(pp, q, &msg);
					break;
				}
				break;
			case UA_OBJ_TYPE_XP_XGP:
				/* ASP and SPP messages from a specific ASP or SPP on an SGP X-link
				 * are treated normally.  SGP messages from the peer SGP on the
				 * X-link are treated specially. */
				switch (pp->spp.spp->type) {
				case UA_OBJ_TYPE_ASP:
					err = asp_recv_msg(pp, q, &msg);
					break;
				case UA_OBJ_TYPE_SGP:
					err = xgp_recv_msg(pp, q, &msg);
					break;
				case UA_OBJ_TYPE_SPP:
					err = spp_recv_msg(pp, q, &msg);
					break;
				}
				break;
			case UA_OBJ_TYPE_XP_XPP:
				/* ASP and SGP messages are not permitted on an SPP X-link.  Whether
				 * the message pertains to a peer SPP or not can be determined from
				 * the SP type. */
				switch (pp->spp.spp->sp.sp->type) {
				case UA_OBJ_TYPE_SP:
					err = xpp_recv_msg(pp, q, &msg);
					break;
				case UA_OBJ_TYPE_SG:
					err = spp_recv_msg(pp, q, &msg);
					break;
				}
				break;
			default:
				swerr();
				return (-EFAULT);
			}
		} else if ((sp = xp->sp)) {
			/* Direct links are associated with either an SP-U or an SP-P structure.
			   Direct links associated with an SP-U structure represent connecting
			   transports to remote ASPs that have not yet identified themselves.
			   Because an ASP or SPP always knows which remote SGP or SPP to which it
			   is connecting, directly links associated with only an SP-P structure
			   represent an error. */
			/* Cross-links are associated with either an SP-U or an SP-P structure.
			   Cross-links associated with an SP-U structure represent peer ASPs or
			   SPPs that have not yet identified themselves.  Cross-links associated
			   with an SP-P structure represent peer SGPs that have not yet identified
			   themselves. */
			/* The only case in which a remote ASP or SPP, or in which a peer ASP, SGP
			   or SPP is when dynamic port allocation is used and the local process
			   must wait for the ASP Up containing an ASP Id to fully identify the
			   process at the other end of the transport. */
			switch (xp->type) {
			case UA_OBJ_TYPE_XP_ASP:
				/* Unknown ASP or SPP connecting. */
				err = sp_recv_msg(xp, q, &msg);
				break;
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
				/* Unknown SGP or SPP connecting. */
				err = sg_recv_msg(xp, q, &msg);
				break;
			case UA_OBJ_TYPE_XP_XSP:
				/* New ASP, SGP or SPP connecting. */
				err = xs_recv_msg(xp, q, &msg);
				break;
			case UA_OBJ_TYPE_XP_XGP:
				/* New ASP, SGP or SPP connecting. */
				err = xg_recv_msg(xp, q, &msg);
				break;
			case UA_OBJ_TYPE_XP_XPP:
				/* New SPP connecting. */
				err = xp_recv_msg(xp, q, &msg);
				break;
			default:
				swerr();
				return (-EFAULT);
			}
		} else
			goto disable;
	}
	if (err == QR_DONE)
		return (QR_TRIMMED);
	switch (err) {
	case -EPROTO:
		uaerr = UA_ECODE_UNEXPECTED_MESSAGE;
		break;
	case -EOPNOTSUPP:
		uaerr = UA_ECODE_UNSUPPORTED_MESSAGE_TYPE;
		break;
	case -ENOPROTOOPT:
		uaerr = UA_ECODE_UNSUPPORTED_MESSAGE_CLASS;
		break;
	case -EINVAL:
		uaerr = UA_ECODE_INVALID_PARAMETER_VALUE;
		break;
	case -EMSGSIZE:
		uaerr = UA_ECODE_PARAMETER_FIELD_ERROR;
		break;
	case -ENXIO:
		uaerr = UA_ECODE_MISSING_PARAMETER;
		break;
	case -EIO:
		uaerr = UA_ECODE_INVALID_IID;
		uaerr = UA_ECODE_INVALID_ROUTING_CONTEXT;
		break;
	case -EXDEV:
		uaerr = UA_ECODE_NO_CONFIGURED_AS_FOR_ASP;
		break;
	case -EPERM:
		uaerr = UA_ECODE_REFUSED_MANAGEMENT_BLOCKING;
		break;
	case -ENOENT:
		uaerr = UA_ECODE_INVALID_ASPID;
		break;
	case -ESRCH:
		uaerr = UA_ECODE_ASPID_REQUIRED;
		break;
	case -ENOSYS:
		uaerr = UA_ECODE_UNSUPPORTED_TRAFFIC_MODE;
		break;
	case -EBADMSG:
		uaerr = UA_ECODE_UNEXPECTED_PARAMETER;
		break;
	default:
		return (err);
	}
	return ua_send_mgmt_err(xp, q, uaerr, mp->b_rptr, mp->b_wptr - mp->b_rptr);
      disable:
	swerr();
	return (QR_DISABLE);
}

/*
 *  =========================================================================
 *
 *  Messages from above:
 *
 *  =========================================================================
 *
 *  SL-U to M2UA primitives.
 *
 *  -------------------------------------------------------------------------
 */

/**
 * slu_info_req: - process LMI_INFO_REQ primitive
 * @ss: private structure
 * @q: active queue (upper write queue)
 * @mp: the LMI_INFO_REQ message
 *
 * The LMI_INFO_REQ primitive is processed locally and does not cause the provider to be
 * interrogated.
 */
STATIC int
slu_info_req(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (ss_get_i_state(ss)) {
	case LMI_ENABLED:
	case LMI_DISABLED:
	case LMI_ENABLE_PENDING:
	case LMI_DISABLE_PENDING:
		return sl_info_ack(ss, q, (unsigned char *) &ss->add, sizeof(ss->add));
	default:
		return sl_info_ack(ss, q, NULL, 0);
	}
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, ss));
	goto error;
      error:
	return sl_error_ack(ss, q, p->lmi_primitive, err);
}

/**
 * slu_attach_req: - process LMI_ATTACH_REQ primitive
 * @ss: private structure
 * @q: active queue (upper write queue)
 * @mp: the LMI_INFO_REQ message
 *
 * The LMI_ATTACH_REQ is a request to associate the upper multiplex Stream with an interface
 * identified by the PPA.
 */
STATIC int
slu_attach_req(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct as *as;
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct ua_addr add;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sizeof(add))
		goto badprim;
	if (ss_get_i_state(ss) != LMI_UNATTACHED)
		goto outstate;
	if (ss_get_state(ss) != AS_DOWN)
		goto outstate;
	ss_set_i_state(ss, LMI_ATTACH_PENDING);
	bcopy((p + 1), &add, sizeof(add));
	for (as = master.as.list; as; as = as->next) {
		if (as->type != UA_OBJ_TYPE_AS_U)
			continue;
		if (as->sp.sp->id == add.spid && as->add.sdli == add.sdli)
			break;
	}
	if (!as) {
		/* FIXME: Instead of generating an error here, we need to perform dynamic
		   allocation and configuration of signalling link providers.  We do that by
		   passing the LMI_ATTACH_REQ primitive upstream on the management Stream, but
		   prefixed with an upper multiplex identifier (major and minor device number). The 
		   management Stream uses input/output controls to effect the attachment and either 
		   returns an LMI_OK_ACK or LMI_ERROR_ACK prefixed with the device number of the
		   requesting Stream.  The requesting Stream then researches the list (as above)
		   and either completes the attachment or fails. */
		goto badppa;
	}
	/* link SL-U to AS-U */
	if ((ss->as.next = as->ss.list))
		ss->as.next->as.prev = &ss->as.next;
	ss->as.prev = &as->ss.list;
	ss->as.as = as_get(as);
	as->ss.list = ss_get(ss);
	as->ss.numb++;
	if ((err = ss_u_set_state(ss, q, AS_INACTIVE)))
		goto error;
	return sl_ok_ack(ss, q, p->lmi_primitive);
      badppa:
	err = LMI_BADPPA;
	ptrace(("%s: %p: PROTO: bad PPA\n", DRV_NAME, ss));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: PROTO: would place i/f out of state\n", DRV_NAME, ss));
	goto error;
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, ss));
	goto error;
      error:
	return sl_error_ack(ss, q, p->lmi_primitive, err);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC int
slu_detach_req(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct as *as;
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ss_get_i_state(ss) != LMI_DISABLED)
		goto outstate;
	if (ss_get_state(ss) != AS_INACTIVE)
		goto efault;
	ss_set_i_state(ss, LMI_DETACH_PENDING);
	if ((err = ss_u_set_state(ss, q, AS_DOWN)))
		goto error;
	/* unlink SL-U from AS-U */
	if ((as = ss->as.as)) {
		if ((*ss->as.prev = ss->as.next))
			ss->as.next->as.prev = ss->as.prev;
		ss->as.next = NULL;
		ss->as.prev = &ss->as.next;
		ss_put(ss);
		as->ss.numb--;
		as_put(xchg(&ss->as.as, NULL));
	}
	return sl_ok_ack(ss, q, p->lmi_primitive);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: software error\n", DRV_NAME, ss));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: PROTO: would place i/f out of state\n", DRV_NAME, ss));
	goto error;
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, ss));
	goto error;
      error:
	return sl_error_ack(ss, q, p->lmi_primitive, err);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC int
slu_enable_req(struct ss *ss, queue_t *q, mblk_t *mp)
{
	struct gp *asp;
	struct xp *xp;
	struct as *as;
	int err;
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (ss_get_i_state(ss)) {
	case LMI_ENABLED:
		goto ignore;
	case LMI_DISABLED:
	case LMI_ENABLE_PENDING:
		break;
	case LMI_UNUSABLE:
		goto unusable;
	default:
		goto outstate;
	}
	if (!(as = ss->as.as))
		goto efault;
	switch (ss_get_state(ss)) {
	case AS_DOWN:
		swerr();
		goto outstate;
	case AS_INACTIVE:
		if (as_tst_flags(ss->as.as, ASF_MGMT_BLOCKED))
			goto blocked;
		if ((err = ss_u_set_state(ss, q, AS_WACK_ASPAC)))
			goto error;
		ss_set_i_state(ss, LMI_ENABLE_PENDING);
		/* fall through */
	case AS_WACK_ASPAC:
		switch (as_get_state(as)) {
		case AS_INACTIVE:
			swerr();
			goto outstate;
		case AS_WACK_ASPAC:
			if (as->sp.sp->tmode != UA_TMODE_OVERRIDE)
				return (QR_DONE);	/* wait for activation to confirm */
			/* look for activating ASP */
			for (asp = as->gp.list; asp && gp_get_state(asp) != AS_WACK_ASPAC;
			     asp = asp->as.next) ;
			if (asp) {
				if ((err = ua_send_aspt_aspia_ack(asp, q, NULL, 0)))
					goto error;
				if ((err = gp_u_set_state(asp, q, AS_INACTIVE)))
					goto error;
			}
			return (QR_DONE);	/* wait for activation to confirm */
		case AS_WACK_ASPIA:
			if (as->sp.sp->tmode != UA_TMODE_OVERRIDE)
				break;
			/* look for deactivating ASP */
			for (asp = as->gp.list; asp && gp_get_state(asp) != AS_WACK_ASPIA;
			     asp = asp->as.next) ;
			if (asp) {
				if ((err = ua_send_aspt_aspia_ack(asp, q, NULL, 0)))
					goto error;
				if ((err = gp_u_set_state(asp, q, AS_INACTIVE)))
					goto error;
			}
			break;
		case AS_ACTIVE:
			if (as->sp.sp->tmode != UA_TMODE_OVERRIDE)
				break;
			/* look for active ASP */
			for (asp = as->gp.list; asp && gp_get_state(asp) != AS_ACTIVE;
			     asp = asp->as.next) ;
			if (asp) {
				if ((err = ua_send_mgmt_ntfy_as(asp, q,
								UA_STATUS_ALTERNATE_ASP_ACTIVE,
								NULL, NULL, 0)))
					goto error;
				if ((err = gp_u_set_state(asp, q, AS_INACTIVE)))
					goto error;
			}
			break;
		}
		/* fall through */
	case AS_WACK_ASPIA:
		todo(("Notify management that SL-U has gone active\n"));
		if ((err = ss_u_set_state(ss, q, AS_ACTIVE)))
			goto error;
	case AS_ACTIVE:
	      ignore:
		if ((err = sl_enable_con(ss, q)))
			goto error;
		ss_set_i_state(ss, LMI_ENABLED);
		return (QR_DONE);
	}
      efault:
	pswerr(("%s: %p: SWERR: software error\n", DRV_NAME, ss));
	return (-EFAULT);
      blocked:
	err = -EBUSY;
	ptrace(("%s: %p: PROTO: management blocked\n", DRV_NAME, ss));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: PROTO: out of state\n", DRV_NAME, ss));
	goto error;
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, ss));
	goto error;
      unusable:
	err = -EAGAIN;
	ptrace(("%s: %p: ERROR: waiting for stream to become usable\n", DRV_NAME, ss));
	goto error;
      error:
	return sl_error_ack(ss, q, p->lmi_primitive, err);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC int
slu_disable_req(struct ss *ss, queue_t *q, mblk_t *mp)
{
	struct as *as;
	int err;
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (ss_get_i_state(ss)) {
	case LMI_DISABLED:
		goto ignore;
	case LMI_ENABLED:
	case LMI_ENABLE_PENDING:
		break;
	case LMI_UNUSABLE:
		goto unusable;
	default:
		goto outstate;
	}
	if (!(as = ss->as.as))
		goto efault;
	switch (ss_get_state(ss)) {
	case AS_DOWN:
		swerr();
		goto outstate;
	case AS_ACTIVE:
		if ((err = ss_u_set_state(ss, q, AS_WACK_ASPIA)))
			goto error;
		ss_set_i_state(ss, LMI_DISABLE_PENDING);
		/* fall through */
	case AS_WACK_ASPIA:
		if (as_tst_flags(as, ASF_ACTIVE))
			/* wait for deactivation to confirm */
			return (QR_DONE);
		/* fall through */
	case AS_WACK_ASPAC:
		todo(("Notify management that SL-U has gone inactive\n"));
		if ((err = ss_u_set_state(ss, q, AS_INACTIVE)))
			goto error;
	case AS_INACTIVE:
		if ((err = sl_disable_con(ss, q)))
			goto error;
		return (QR_DONE);
	}
      ignore:
	rare();
	return (QR_DONE);
      efault:
	pswerr(("%s: %p: SWERR: software error\n", DRV_NAME, ss));
	return (-EFAULT);
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: PROTO: out of state\n", DRV_NAME, ss));
	goto error;
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, ss));
	goto error;
      unusable:
	err = -EAGAIN;
	ptrace(("%s: %p: ERROR: waiting for stream to become usable\n", DRV_NAME, ss));
	goto error;
      error:
	return sl_error_ack(ss, q, p->lmi_primitive, err);
}

#if 0
/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
slu_optmgmt_req(struct ss *ss, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;

	return sl_error_ack(ss, q, p->sl_primitive, LMI_NOTSUPP);
}
#endif

#if 0
/*
 *  SL Send Message
 *  -----------------------------------
 *  This function distributes SL-U request and response primitives to the SL-P or converts them to the appropriate
 *  M2UA messages and passes them to SGPs active for the AS according to the traffic mode and the message type.
 */
STATIC int
slu_send_msg(struct ss *ssu, queue_t *q, mblk_t *mp, int type,
	     int (*pass_f) (queue_t *, struct ss *, mblk_t *),
	     int (*send_f) (queue_t *, struct gp *, mblk_t *))
{
	struct ss *ssp;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			if ((err = pass_f(q, ssp, bp)) < 0) {
				freemsg(bp);
				return (err);
			}
			return (QR_DONE);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			if ((err = send_f(q, sgp, mp)) < 0) {
				freemsg(bp);
				return (err);
			}
			freeb(bp);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}
#endif

/*
 *  SL_DATA_REQ
 *  -----------------------------------
 */
STATIC int
slu_write(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	size_t dlen = mp ? msgdsize(mp) : 0;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (dlen < ssu->info.lmi_min_sdu || dlen > ssu->info.lmi_max_sdu)
		goto badprim;
	if (!(as = ssu->as.as))
		goto efault;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (!canputnext(ssp->oq))
				goto ebusy;
			putnext(ssp->oq, mp);
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((ssu->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
				if ((err = m2ua_send_maup_data1(sgp, q, mp)) < 0)
					goto error;
				return (QR_ABSORBED);
			} else {
				if ((err = m2ua_send_maup_data2(sgp, q, mp)) < 0)
					goto error;
				return (QR_ABSORBED);
			}
		}
	}
	goto efault;
      discard:
	rare();
	return (QR_DONE);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      badprim:
	err = LMI_BADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive\n", DRV_NAME, ssu));
	goto error;
      error:
	return m_error(ssu, q, EPROTO);
}

/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
STATIC int
slu_pdu_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp) : 0;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (dlen < ssu->info.lmi_min_sdu || dlen > ssu->info.lmi_max_sdu)
		goto badprim;
	if (!(as = ssu->as.as))
		goto efault;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (canputnext(ssp->oq)) {
				putnext(ssp->oq, mp);
				return (QR_ABSORBED);
			}
			goto ebusy;
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((ssu->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
				if ((err = m2ua_send_maup_data1(sgp, q, mp->b_cont)) < 0)
					goto error;
				return (QR_TRIMMED);
			} else {
				if ((err = m2ua_send_maup_data2(sgp, q, mp->b_cont)) < 0)
					goto error;
				return (QR_TRIMMED);
			}
			goto error;
		}
	}
	goto efault;
      discard:
	rare();
	return (QR_DONE);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      badprim:
	err = LMI_BADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
STATIC int
slu_emergency_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_EMER_SET);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if (ss_tst_flags(ssu, ASF_EMERGENCY))
		goto discard;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (ss_tst_flags(ssp, ASF_EMERGENCY)) {
				ss_set_flags(ssu, ASF_EMERGENCY);
				return (QR_DONE);
			}
			switch (ss_get_s_state(ssp)) {
			case SLS_IDLE:
			case SLS_WCON_EREQ:
			case SLS_WCON_RELREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if (canputnext(ssp->oq)) {
					ss_set_flags(ssu, ASF_EMERGENCY);
					putnext(ssp->oq, mp);
					return (QR_ABSORBED);
				}
				goto ebusy;
			default:
				swerr();
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (gp_tst_flags(sgp, ASF_EMERGENCY)) {
				ss_set_flags(ssu, ASF_EMERGENCY);
				return (QR_DONE);
			}
			switch (gp_get_s_state(sgp)) {
			case SLS_IDLE:
			case SLS_WCON_EREQ:
			case SLS_WCON_RELREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
					goto error;
				ss_set_flags(ssu, ASF_EMERGENCY);
				return (err);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = sl_local_processor_outage_ind(ssu, q)) < 0)
		goto error;
	ss_set_flags(ssu, ASF_PRV_PROC_OUTAGE);
	ss_set_flags(ssu, ASF_EMERGENCY);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
STATIC int
slu_emergency_ceases_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_EMER_CLEAR);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if (!ss_tst_flags(ssu, ASF_EMERGENCY))
		goto discard;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (!ss_tst_flags(ssp, ASF_EMERGENCY)) {
				ss_clr_flags(ssu, ASF_EMERGENCY);
				return (QR_DONE);
			}
			switch (ss_get_s_state(ssp)) {
			case SLS_IDLE:
			case SLS_WCON_EREQ:
			case SLS_WCON_RELREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if (canputnext(ssp->oq)) {
					ss_clr_flags(ssu, ASF_EMERGENCY);
					putnext(ssp->oq, mp);
					return (QR_ABSORBED);
				}
				goto ebusy;
			default:
				swerr();
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (!gp_tst_flags(sgp, ASF_EMERGENCY)) {
				ss_clr_flags(ssu, ASF_EMERGENCY);
				return (QR_DONE);
			}
			switch (gp_get_s_state(sgp)) {
			case SLS_IDLE:
			case SLS_WCON_EREQ:
			case SLS_WCON_RELREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
					goto error;
				ss_clr_flags(ssu, ASF_EMERGENCY);
				return (err);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = sl_local_processor_outage_ind(ssu, q)) < 0)
		goto error;
	ss_set_flags(ssu, ASF_PRV_PROC_OUTAGE);
	ss_clr_flags(ssu, ASF_EMERGENCY);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 *  Now, actually if the link has already been started, we can simply respond to this with an SL_IN_SERVICE_IND,
 *  otherwise we start the link.
 */
STATIC int
slu_start_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if (ss_get_s_state(ssu) != SLS_IDLE)
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			switch (ss_get_s_state(ssp)) {
			case SLS_WCON_RELREQ:
				/* The signalling link has failed and we are restarting it */
				rare();
			case SLS_IDLE:
				if (!ss_tst_flags(ssp, ASF_RETRIEVAL)) {
					if (canputnext(ssp->oq)) {
						ss_set_s_state(ssu, SLS_WCON_EREQ);
						ss_set_s_state(ssp, SLS_WCON_EREQ);
						putnext(ssp->oq, mp);
						return (QR_ABSORBED);
					}
					goto ebusy;
				} else {
					/* Someone else is retrieving messages, so we need to
					   indicate that the signalling link has failed (reason
					   will be unspecified). */
					if ((err =
					     sl_out_of_service_ind(ssu, q,
								   SL_FAIL_UNSPECIFIED)) < 0)
						goto error;
					ss_set_s_state(ssu, SLS_IDLE);
					return (QR_DONE);
				}
			case SLS_WCON_EREQ:
				/* The signalling link is already being established by some other
				   signalling link user. Wait for establishment confirmation. */
				ss_set_s_state(ssu, SLS_WCON_EREQ);
				return (QR_DONE);
			case SLS_ESTABLISHED:
				/* The signalling link is already established by some other
				   signalling link user. Indicate that the link is in service */
				if ((err = sl_in_service_ind(ssu, q)) < 0)
					goto error;
				ss_set_s_state(ssu, SLS_ESTABLISHED);
				return (QR_DONE);
			default:
				swerr();
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			switch (gp_get_s_state(sgp)) {
			case SLS_WCON_RELREQ:
				/* The signalling link failed and we are restarting it */
				rare();
			case SLS_IDLE:
				if (!gp_tst_flags(sgp, ASF_RETRIEVAL)) {
					if ((err = m2ua_send_maup_estab_req(sgp, q)) < 0)
						goto error;
					ss_set_s_state(ssu, SLS_WCON_EREQ);
					gp_set_s_state(sgp, SLS_WCON_EREQ);
					return (err);
				} else {
					/* Someone else is retrieving messages, so we need to
					   indicate that the signalling link has failed (reason
					   will be unspecified). */
					if ((err =
					     sl_out_of_service_ind(ssu, q,
								   SL_FAIL_UNSPECIFIED)) < 0)
						goto error;
					ss_set_s_state(ssu, SLS_IDLE);
					return (err);
				}
			case SLS_WCON_EREQ:
				/* The signalling link is already being established by some other
				   signalling link user. Wait for establishment confirmation */
				ss_set_s_state(ssu, SLS_WCON_EREQ);
				return (QR_DONE);
			case SLS_ESTABLISHED:
				/* The signalling link is already established by some other
				   signalling link user. Indicate that the link is in service */
				if ((err = sl_in_service_ind(ssu, q)) < 0)
					goto error;
				ss_set_s_state(ssu, SLS_ESTABLISHED);
				return (QR_DONE);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = sl_local_processor_outage_ind(ssu, q)) < 0)
		goto error;
	ss_set_flags(ssu, ASF_PRV_PROC_OUTAGE);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 *  If there are other users of this signalling link, we want to send local or remote processor outage (rather than
 *  link failure) to those links so that changeover procedure will not occur there.
 */
STATIC int
slu_stop_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if ((1 << ss_get_s_state(ssu)) & (SLSF_IDLE | SLSF_WCON_RELREQ))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			switch (ss_get_s_state(ssp)) {
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if (canputnext(ssp->oq)) {
					ss_set_s_state(ssu, SLS_IDLE);
					ss_set_s_state(ssp, SLS_IDLE);
					putnext(ssp->oq, mp);
					return (QR_ABSORBED);
				}
				goto ebusy;
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				/* The signalling link has already been put out of service by some
				   other signalling link user or we can't put it out of service.
				   Just silently accept the request. */
				ss_set_s_state(ssu, SLS_IDLE);
				return (QR_DONE);
			default:
				swerr();
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			switch (gp_get_s_state(sgp)) {
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if ((err = m2ua_send_maup_rel_req(sgp, q)) < 0)
					goto error;
				ss_set_s_state(ssu, SLS_IDLE);
				gp_set_s_state(sgp, SLS_IDLE);
				return (err);
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				/* The signalling link has already been put out of service by some
				   other signalling link user or we can't put it out of service.
				   Just silently accept the request. */
				ss_set_s_state(ssu, SLS_IDLE);
				return (QR_DONE);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	ss_set_s_state(ssu, SLS_IDLE);
	return (QR_DONE);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
STATIC int
slu_retrieve_bsnt_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if ((1 << ss_get_s_state(ssu)) & (SLSF_ESTABLISHED | SLSF_WCON_RELREQ))
		goto outstate;
	if (ss_tst_flags(ssu, ASF_BSNT_REQUEST))
		goto outstate;	/* alreday requested */
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (ss_tst_flags(ssp, ASF_BSNT_REQUEST)) {
				ss_set_flags(ssu, ASF_BSNT_REQUEST);
				return (QR_DONE);
			}
			switch (ss_get_s_state(ssp)) {
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				if (canputnext(ssp->oq)) {
					ss_set_flags(ssu, ASF_BSNT_REQUEST);
					ss_set_flags(ssp, ASF_BSNT_REQUEST);
					putnext(ssp->oq, mp);
					return (QR_ABSORBED);
				}
				goto ebusy;
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if ((err = sl_bsnt_not_retrievable_ind(ssu, q, 0)) < 0)
					goto error;
				return (err);
			default:
				swerr();
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (gp_tst_flags(sgp, ASF_BSNT_REQUEST)) {
				ss_set_flags(ssu, ASF_BSNT_REQUEST);
				return (QR_DONE);
			}
			switch (gp_get_s_state(sgp)) {
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				if ((err = m2ua_send_maup_retr_req(sgp, q, NULL)) < 0)
					goto error;
				ss_set_flags(ssu, ASF_BSNT_REQUEST);
				gp_set_flags(sgp, ASF_BSNT_REQUEST);
				return (err);
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if ((err = sl_bsnt_not_retrievable_ind(ssu, q, 0)) < 0)
					goto error;
				return (err);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = sl_bsnt_not_retrievable_ind(ssu, q, 0)) < 0)
		goto error;
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      outstate:
	fixme(("This should actually return BSNT not retrievable\n"));
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
STATIC int
slu_retrieval_request_and_fsnc_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	uint32_t fsnc = htohl(p->sl_fsnc);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if (ss_get_s_state(ssu) != SLS_IDLE)
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			switch (ss_get_s_state(ssp)) {
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				if (!ss_tst_flags(ssp, ASF_RETRIEVAL)) {
					if (canputnext(ssp->oq)) {
						ss_set_flags(ssu, ASF_RETRIEVAL);
						ss_set_flags(ssp, ASF_RETRIEVAL);
						putnext(ssp->oq, mp);
						return (QR_ABSORBED);
					}
					goto ebusy;
				}
				/* fall through */
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				/* Retrieval is not possible or someone else is already retrieving. 
				   Only one signalling link user can be allowed to retrieve.  We
				   must refuse the retrieval request. */
				if ((err = sl_retrieval_not_possible_ind(ssu, q)) < 0)
					goto error;
				ss_set_s_state(ssu, SLS_IDLE);
				return (err);
			default:
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			switch (gp_get_s_state(sgp)) {
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				if (!gp_tst_flags(sgp, ASF_RETRIEVAL)) {
					if ((err = m2ua_send_maup_retr_req(sgp, q, &fsnc)) < 0)
						goto error;
					gp_set_flags(sgp, ASF_RETRIEVAL);
					ss_set_flags(ssu, ASF_RETRIEVAL);
					return (err);
				}
				/* fall through */
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				/* Retrieval is not possible or someone else is already retrieving. 
				   Only one signalling link user can be allowed to retrieve.  We
				   must refuse the retrieval request. */
				if ((err = sl_retrieval_not_possible_ind(ssu, q)) < 0)
					goto error;
				ss_set_s_state(ssu, SLS_IDLE);
				return (err);
			default:
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = sl_retrieval_not_possible_ind(ssu, q)) < 0)
		goto error;
	ss_set_s_state(ssu, SLS_IDLE);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      outstate:
	fixme(("This should actually return BSNT not retrievable\n"));
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
STATIC int
slu_clear_buffers_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_FLUSH_BUFFERS);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if (ss_tst_flags(ssu, ASF_FLUSH_BUFFERS))
		goto discard;	/* already done */
	if (!ss_tst_flags(ssu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE)))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			/* active or pending local ssp */
			if (!ss_tst_flags(ssp, ASF_FLUSH_BUFFERS)) {
				if (ss_tst_flags(ssp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if (!canputnext(ssp->oq))
						goto ebusy;
					putnext(ssp->oq, mp);
					ss_clr_flags(ssp, ASF_USR_PROC_OUTAGE);
				}
				ss_clr_flags(ssu, ASF_USR_PROC_OUTAGE);
				ss_set_flags(ssu, ASF_FLUSH_BUFFERS);
			}
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			/* active or pending sgp */
			if (!gp_tst_flags(sgp, ASF_FLUSH_BUFFERS)) {
				if (gp_tst_flags(sgp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
						goto error;
					gp_clr_flags(sgp, ASF_USR_PROC_OUTAGE);
					ss_clr_flags(ssu, ASF_USR_PROC_OUTAGE);
				}
				ss_set_flags(ssu, ASF_FLUSH_BUFFERS);
			}
			return (QR_DONE);
		}
	}
	/* No active or pending provider */
	if ((err = sl_local_processor_outage_ind(ssu, q)) < 0)
		goto error;
	ss_set_flags(ssu, ASF_PRV_PROC_OUTAGE);
	ss_clr_flags(ssu, ASF_USR_PROC_OUTAGE);
	ss_set_flags(ssu, ASF_FLUSH_BUFFERS);
	return (QR_DONE);
      outstate:
	fixme(("This should actually return BSNT not retrievable\n"));
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, ssu));
	goto error;
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
STATIC int
slu_clear_rtb_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err = QR_DONE;
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_CLEAR_RTB);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if (ss_tst_flags(ssu, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS)))
		goto discard;	/* already done */
	if (!ss_tst_flags(ssu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE)))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			/* active or pending local ssp */
			if (!ss_tst_flags(ssp, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS))) {
				if (ss_tst_flags(ssp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if (!canputnext(ssp->oq))
						goto ebusy;
					putnext(ssp->oq, mp);
					err = QR_ABSORBED;
					ss_clr_flags(ssp, ASF_USR_PROC_OUTAGE);
				}
			}
			ss_clr_flags(ssu, ASF_USR_PROC_OUTAGE);
			ss_set_flags(ssu, ASF_CLEAR_RTB);
			return (err);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			/* active or pending sgp */
			if (!gp_tst_flags(sgp, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS))) {
				if (gp_tst_flags(sgp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
						goto error;
					gp_clr_flags(sgp, ASF_USR_PROC_OUTAGE);
				}
			}
			ss_clr_flags(ssu, ASF_USR_PROC_OUTAGE);
			ss_set_flags(ssu, ASF_CLEAR_RTB);
			return (err);
		}
	}
	/* No active or pending provider */
	if ((err = sl_local_processor_outage_ind(ssu, q)) < 0)
		goto error;
	ss_set_flags(ssu, ASF_PRV_PROC_OUTAGE);
	ss_clr_flags(ssu, ASF_USR_PROC_OUTAGE);
	ss_set_flags(ssu, ASF_CLEAR_RTB);
	return (err);
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, ssu));
	goto error;
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_CONTINUE_REQ
 *  -----------------------------------
 */
STATIC int
slu_continue_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err = QR_DONE;
	sl_continue_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_CONTINUE);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if (ss_tst_flags(ssu, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS)))
		goto discard;	/* already done */
	if (!ss_tst_flags(ssu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE)))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			/* active or pending local ssp */
			if (!ss_tst_flags(ssp, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS))) {
				if (ss_tst_flags(ssp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if (!canputnext(ssp->oq))
						goto ebusy;
					putnext(ssp->oq, mp);
					err = QR_ABSORBED;
					ss_clr_flags(ssp, ASF_USR_PROC_OUTAGE);
				}
			}
			ss_clr_flags(ssu, ASF_USR_PROC_OUTAGE);
			ss_set_flags(ssu, ASF_CLEAR_RTB);
			return (err);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			/* active or pending sgp */
			if (!gp_tst_flags(sgp, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS))) {
				if (gp_tst_flags(sgp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
						goto error;
					gp_clr_flags(sgp, ASF_USR_PROC_OUTAGE);
				}
			}
			ss_clr_flags(ssu, ASF_USR_PROC_OUTAGE);
			ss_set_flags(ssu, ASF_CLEAR_RTB);
			return (err);
		}
	}
	/* No active or pending provider */
	if ((err = sl_local_processor_outage_ind(ssu, q)) < 0)
		goto error;
	ss_set_flags(ssu, ASF_PRV_PROC_OUTAGE);
	ss_clr_flags(ssu, ASF_USR_PROC_OUTAGE);
	ss_set_flags(ssu, ASF_CLEAR_RTB);
	return (err);
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, ssu));
	goto error;
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
STATIC int
slu_local_processor_outage_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err = QR_DONE;
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_LPO_SET);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if (ss_tst_flags(ssu, ASF_USR_PROC_OUTAGE))
		goto discard;	/* already done */
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			/* Active or pending ssp */
			if (!ss_tst_flags(ssp, ASF_USR_PROC_OUTAGE)) {
				if (!canputnext(ssp->oq))
					goto ebusy;
				putnext(ssp->oq, mp);
				err = QR_ABSORBED;
				ss_set_flags(ssp, ASF_USR_PROC_OUTAGE);
			}
			ss_set_flags(ssu, ASF_USR_PROC_OUTAGE);
			return (err);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			/* Active or pending sgp */
			if (!gp_tst_flags(sgp, ASF_USR_PROC_OUTAGE)) {
				if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
					goto error;
				gp_set_flags(sgp, ASF_USR_PROC_OUTAGE);
			}
			ss_set_flags(ssu, ASF_USR_PROC_OUTAGE);
			return (err);
		}
	}
	/* No active or pending provider */
	/* just wait */
	ss_set_flags(ssu, ASF_USR_PROC_OUTAGE);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
STATIC int
slu_resume_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err = QR_DONE;
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_LPO_CLEAR);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = ssu->as.as))
		goto efault;
	if (!ss_tst_flags(ssu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))
	    || ss_tst_flags(ssu, (ASF_PRV_PROC_OUTAGE | ASF_LOC_PROC_OUTAGE | ASF_REM_PROC_OUTAGE)))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			/* active or pending local ssp */
			if (!ss_tst_flags
			    (ssp,
			     (ASF_PRV_PROC_OUTAGE | ASF_LOC_PROC_OUTAGE | ASF_REM_PROC_OUTAGE))) {
				if (ss_tst_flags(ssp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if (!canputnext(ssp->oq))
						goto ebusy;
					putnext(ssp->oq, mp);
					err = QR_ABSORBED;
					ss_clr_flags(ssp,
						     (ASF_RECOVERY | ASF_USR_PROC_OUTAGE |
						      ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS));
				}
			}
			ss_clr_flags(ssu,
				     (ASF_RECOVERY | ASF_USR_PROC_OUTAGE | ASF_CLEAR_RTB |
				      ASF_FLUSH_BUFFERS));
			return (err);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			/* active or pending sgp */
			if (!gp_tst_flags
			    (sgp,
			     (ASF_PRV_PROC_OUTAGE | ASF_LOC_PROC_OUTAGE | ASF_REM_PROC_OUTAGE))) {
				if (!gp_tst_flags(sgp, ASF_RECOVERY)) {
					if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
						goto error;
					gp_clr_flags(sgp,
						     (ASF_RECOVERY | ASF_USR_PROC_OUTAGE |
						      ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS));
				}
			}
			ss_clr_flags(ssu,
				     (ASF_RECOVERY | ASF_USR_PROC_OUTAGE | ASF_CLEAR_RTB |
				      ASF_FLUSH_BUFFERS));
			return (err);
		}
	}
	/* no active or pending provider */
	if ((err = sl_local_processor_outage_ind(ssu, q)) < 0)
		goto error;
	ss_set_flags(ssu, ASF_PRV_PROC_OUTAGE);
	ss_clr_flags(ssu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE | ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS));
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, ssu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, ssu));
	goto error;
      outstate:
	fixme(("This should actually return BSNT not retrievable\n"));
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, ssu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, ssu));
	goto error;
      error:
	return sl_error_ack(ssu, q, p->sl_primitive, err);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
STATIC int
slu_congestion_discard_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_CONG_DISCARD);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (!canputnext(ssp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			putnext(ssp->oq, mp);
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
				return (err);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
STATIC int
slu_congestion_accept_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_CONG_ACCEPT);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (!canputnext(ssp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			putnext(ssp->oq, mp);
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
				return (err);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
STATIC int
slu_no_congestion_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_CONG_CLEAR);

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (!canputnext(ssp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			putnext(ssp->oq, mp);
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((err = m2ua_send_maup_state_req(sgp, q, request)) < 0)
				return (err);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
STATIC int
slu_power_on_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	struct ss *ssp;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;

	if (ss_get_i_state(ssu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (ssp = ap_find_ssp(ap); ssp; ssp = ap_find_ssp_next(ap, ssp, 1)) {
			if (!canputnext(ssp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			putnext(ssp->oq, mp);
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}

#if 0
/*
 *  SL_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
slu_optmgmt_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	int err;

	if ((err = sl_error_ack(ssu, q, SL_OPTMGMT_REQ, LMI_NOTSUPP, EOPNOTSUPP)))
		return (err);
	return (-EPROTO);
}
#endif

#if 0
/*
 *  SL_NOTIFY_REQ
 *  -----------------------------------
 */
STATIC int
slu_notify_req(struct ss *ssu, queue_t *q, mblk_t *mp)
{
	int err;

	if ((err = sl_error_ack(ssu, q, SL_NOTIFY_REQ, LMI_NOTSUPP, EOPNOTSUPP)))
		return (err);
	return (-EPROTO);
}
#endif

/*
 *  =========================================================================
 *
 *  Messages from below:
 *
 *  =========================================================================
 *
 *  SL-P to M2UA primitives.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC int
slp_info_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;
	ua_addr_t *add;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sizeof(*add))
		goto efault;
	bcopy(p, &ss->info, sizeof(*p) + sizeof(*add));
	return (QR_DONE);
      efault:
	fixme(("Should\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC int
slp_ok_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_ok_ack_t *p = (typeof(p)) mp->b_wptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (ss_get_i_state(ss)) {
	case LMI_ATTACH_PENDING:
		assure(p->lmi_correct_primitive == LMI_ATTACH_REQ);
		assure(ss_get_state(ss) == AS_DOWN);
		ss_set_i_state(ss, LMI_DISABLED);
		todo(("Notify management that SL-P is up\n"));
		if ((err = ss_p_set_state(ss, q, AS_INACTIVE)))
			return (err);
		return (QR_DONE);
	case LMI_DETACH_PENDING:
		assure(p->lmi_correct_primitive == LMI_DETACH_REQ);
		assure(ss_get_state(ss) == AS_INACTIVE);
		ss_set_i_state(ss, LMI_UNATTACHED);
		todo(("Notify management that SL-P is down\n"));
		if ((err = ss_p_set_state(ss, q, AS_DOWN)))
			return (err);
		return (QR_DONE);
	}
      efault:
	fixme(("Should\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC int
slp_error_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_error_ack_t *p = (typeof(p)) mp->b_wptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (ss_get_i_state(ss)) {
	case LMI_ATTACH_PENDING:
		assure(p->lmi_error_primitive == LMI_ATTACH_REQ);
		assure(ss_get_state(ss) == AS_DOWN);
		ss_set_i_state(ss, LMI_UNATTACHED);
		todo(("Notify management that the SL-P is down\n"));
		return (QR_DONE);
	case LMI_DETACH_PENDING:
		assure(p->lmi_error_primitive == LMI_DETACH_REQ);
		assure(ss_get_state(ss) == AS_INACTIVE);
		ss_set_i_state(ss, LMI_DISABLED);
		todo(("Notify management that the SL-P is up\n"));
		return (QR_DONE);
	case LMI_ENABLE_PENDING:
		assure(p->lmi_error_primitive == LMI_ENABLE_REQ);
		assure(ss_get_state(ss) == AS_WACK_ASPAC);
		ss_set_i_state(ss, LMI_DISABLED);
		todo(("Notify management that the SL-P is inactive\n"));
		if ((err = ss_p_set_state(ss, q, AS_INACTIVE)))
			return (err);
		return (QR_DONE);
	case LMI_DISABLE_PENDING:
		assure(p->lmi_error_primitive == LMI_DISABLE_REQ);
		assure(ss_get_state(ss) == AS_WACK_ASPIA);
		ss_set_i_state(ss, LMI_ENABLED);
		todo(("Notify management that the SL-P is active\n"));
		if ((err = ss_p_set_state(ss, q, AS_ACTIVE)))
			return (err);
		return (QR_DONE);
	}
      efault:
	fixme(("Should\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC int
slp_enable_con(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (ss_get_i_state(ss)) {
	case LMI_ENABLE_PENDING:
		assure(ss_get_state(ss) == AS_WACK_ASPAC);
		ss_set_i_state(ss, LMI_ENABLED);
		todo(("Notify management that the SL-P is active\n"));
		if ((err = ss_p_set_state(ss, q, AS_ACTIVE)))
			return (err);
		return (QR_DONE);
	}
      efault:
	fixme(("Should\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC int
slp_disable_con(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (ss_get_i_state(ss)) {
	case LMI_DISABLE_PENDING:
		assure(ss_get_state(ss) == AS_WACK_ASPIA);
		ss_set_i_state(ss, LMI_DISABLED);
		todo(("Notify management that the SL-P is inactive\n"));
		if ((err = ss_p_set_state(ss, q, AS_INACTIVE)))
			return (err);
		return (QR_DONE);
	}
      efault:
	fixme(("Should\n"));
	swerr();
	return (-EFAULT);
}

#if 0
/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC int
slp_optmgmt_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	/* unexpected */
	swerr();
	return (-EFAULT);
}
#endif

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC int
slp_error_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (ss_get_i_state(ss)) {
	case LMI_ENABLED:
	case LMI_ENABLE_PENDING:
		assure(p->lmi_state == LMI_DISABLED);
		assure(ss_get_state(ss) == AS_ACTIVE);
		ss_set_i_state(ss, LMI_DISABLED);
		todo(("Notify management that SL-P is inactive\n"));
		if ((err = ss_p_set_state(ss, q, AS_INACTIVE)))
			return (err);
		return (QR_DONE);
	case LMI_DISABLED:
		assure(p->lmi_state == LMI_DISABLED);
		return (QR_DONE);
	case LMI_DISABLE_PENDING:
		assure(p->lmi_state == LMI_DISABLED);
		assure(ss_get_state(ss) == AS_WACK_ASPIA);
		ss_set_i_state(ss, LMI_DISABLED);
		todo(("Notify management that SL-P is inactive\n"));
		if ((err = ss_p_set_state(ss, q, AS_INACTIVE)))
			return (err);
		return (QR_DONE);
	}
      efault:
	fixme(("Should\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 *  We could deliver these to a local SL-User, but unfortunately there is no way to deliver stats using M2UA.  For
 *  now we'll just drop 'em.
 */
STATIC int
slp_stats_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	/* unexpected */
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 *  We could deliver these to a local SL-User, but unfortunately there is no way to deliver stats using M2UA.  For
 *  now we'll just drop 'em.
 */
STATIC int
slp_event_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	/* unexpected */
	swerr();
	return (-EFAULT);
}

/*
 *  M_ERROR, M_HANGUP IND
 *  -----------------------------------
 */
STATIC int
slp_hangup(queue_t *q, mblk_t *mp)
{
	/* unexpected */
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  M_DATA IND
 *  -----------------------------------
 */
STATIC int
slp_read(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	mblk_t *bp;

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (!ss_tst_flags(ssu, ASF_OPERATION_PENDING)) {
				if ((1 << ss_get_s_state(ssu)) &
				    (SLSF_WCON_RELREQ | SLSF_ESTABLISHED)) {
					if (!canputnext(ssu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(ssu->oq, bp);
					if (as->sp.sp->tmode != UA_TMODE_BROADCAST)
						return (QR_DONE);
					ss_set_flags(ssu, ASF_OPERATION_PENDING);
				}
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if ((1 << gp_get_s_state(asp)) &
				    (SLSF_WCON_RELREQ | SLSF_ESTABLISHED)) {
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					if ((ssp->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
						if ((err = m2ua_send_maup_data1(asp, q, bp)) < 0)
							return (err);
					} else {
						if ((err = m2ua_send_maup_data2(asp, q, bp)) < 0)
							return (err);
					}
					freeb(bp);
					if (as->sp.sp->tmode != UA_TMODE_BROADCAST)
						return (QR_DONE);
					gp_set_flags(asp, ASF_OPERATION_PENDING);
				}
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			ss_clr_flags(ssu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			xp_clr_flags(asp->spp.spp->xp.xp, ASF_OPERATION_PENDING);
	}
	return (QR_DONE);
}

/*
 *  SL_PDU_IND
 *  -----------------------------------
 *  We only send pdu indications to one signalling link user unless we are a broadcast AS.  If we are a loadshare
 *  AS, we will always send pdu indications to the first lowest cost signalling link user.  Local SL-Us are
 *  considered of lower cost that remote ASPs.
 */
STATIC int
slp_pdu_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	mblk_t *bp;

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (!ss_tst_flags(ssu, ASF_OPERATION_PENDING)) {
				if ((1 << ss_get_s_state(ssu)) &
				    (SLSF_WCON_RELREQ | SLSF_ESTABLISHED)) {
					if (!canputnext(ssu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(ssu->oq, bp);
					if (as->sp.sp->tmode != UA_TMODE_BROADCAST)
						return (QR_DONE);
					ss_set_flags(ssu, ASF_OPERATION_PENDING);
				}
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if ((1 << gp_get_s_state(asp)) &
				    (SLSF_WCON_RELREQ | SLSF_ESTABLISHED)) {
					if (!(bp = ss7_dupmsg(q, mp->b_cont)))
						return (-ENOBUFS);
					if ((ssp->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
						if ((err = m2ua_send_maup_data1(asp, q, bp)) < 0)
							return (err);
					} else {
						if ((err = m2ua_send_maup_data2(asp, q, bp)) < 0)
							return (err);
					}
					freeb(bp);
					if (as->sp.sp->tmode != UA_TMODE_BROADCAST)
						return (QR_DONE);
					gp_set_flags(asp, ASF_OPERATION_PENDING);
				}
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			ss_clr_flags(ssu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			xp_clr_flags(asp->spp.spp->xp.xp, ASF_OPERATION_PENDING);
	}
	return (QR_DONE);
}

/*
 *  SL_LINK_CONGESTED_IND
 *  -----------------------------------
 *  Pass this one to all signalling link users in the established state.
 */
STATIC int
slp_link_congested_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ind_t *p = (typeof(p)) mp->b_rptr;
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	uint32_t cong, disc;

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure(ss_get_s_state(ssp) == SLS_ESTABLISHED);

	cong = p->sl_cong_status;
	disc = p->sl_disc_status;

	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (!ss_tst_flags(ssu, ASF_OPERATION_PENDING)) {
				if (ss_get_s_state(ssu) == SLS_ESTABLISHED) {
					mblk_t *bp;

					if (!canputnext(ssu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(ssu->oq, bp);
					if (as->sp.sp->tmode == UA_TMODE_BROADCAST)
						return (QR_DONE);
					ss_set_flags(ssu, ASF_OPERATION_PENDING);
				}
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if (gp_get_s_state(asp) == SLS_ESTABLISHED) {
					if ((err = m2ua_send_maup_cong_ind(asp, q, cong, disc)))
						return (err);
					if (as->sp.sp->tmode == UA_TMODE_BROADCAST)
						return (QR_DONE);
					gp_set_flags(asp, ASF_OPERATION_PENDING);
				}
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			ss_clr_flags(ssu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			xp_clr_flags(asp->spp.spp->xp.xp, ASF_OPERATION_PENDING);
	}
	return (QR_DONE);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  -----------------------------------
 *  Pass this one to all signalling link users in the established state
 */
STATIC int
slp_congestion_ceased_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ceased_ind_t *p = (typeof(p)) mp->b_rptr;
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	uint32_t cong, disc;

	cong = p->sl_cong_status;
	disc = p->sl_disc_status;

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure(ss_get_s_state(ssp) == SLS_ESTABLISHED);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (!ss_tst_flags(ssu, ASF_OPERATION_PENDING)) {
				if (ss_get_s_state(ssu) == SLS_ESTABLISHED) {
					mblk_t *bp;

					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					if (!canputnext(ssu->oq))
						return (-EBUSY);
					putnext(ssu->oq, bp);
					if (as->sp.sp->tmode == UA_TMODE_BROADCAST)
						return (QR_DONE);
				}
				ss_set_flags(ssu, ASF_OPERATION_PENDING);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if (gp_get_s_state(asp) == SLS_ESTABLISHED) {
					if ((err = m2ua_send_maup_cong_ind(asp, q, cong, disc)))
						return (err);
					if (as->sp.sp->tmode == UA_TMODE_BROADCAST)
						return (QR_DONE);
				}
				gp_set_flags(asp, ASF_OPERATION_PENDING);
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			ss_clr_flags(ssu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			xp_clr_flags(asp->spp.spp->xp.xp, ASF_OPERATION_PENDING);
	}
	return (QR_DONE);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND
 *  -----------------------------------
 *  Regardless of the traffic mode we only send retrieved messages to one signalling link user.  This first user to
 *  set flag ASF_RETRIEVAL succeeds and the others fail.
 */
STATIC int
slp_retrieved_message_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure(ss_get_s_state(ssp) == SLS_IDLE);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (ss_tst_flags(ssu, ASF_RETRIEVAL)) {
				if (!canputnext(ssu->oq))
					return (-EBUSY);
				putnext(ssu->oq, mp);
				return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_RETRIEVAL)) {
				if ((ssp->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
					if ((err = m2ua_send_maup_retr_ind1(asp, q, mp)))
						return (err);
				} else {
					if ((err = m2ua_send_maup_retr_ind2(asp, q, mp)))
						return (err);
				}
				return (QR_TRIMMED);
			}
	}
	return (QR_DONE);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND
 *  -----------------------------------
 *  Regardless of traffic mode we only send retrieval complete messages to one signalling link user.  The first user
 *  to set flag ASF_RETRIEVAL succeeds and the others fail.
 */
STATIC int
slp_retrieval_complete_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure(ss_get_s_state(ssp) == SLS_IDLE);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (ss_tst_flags(ssu, ASF_RETRIEVAL)) {
				if (!canputnext(ssu->oq))
					return (-EBUSY);
				putnext(ssu->oq, mp);
				ss_set_s_state(ssu, SLS_IDLE);
				ss_set_s_state(ssp, SLS_IDLE);
				return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_RETRIEVAL)) {
				if ((err = m2ua_send_maup_retr_comp_ind(asp, q)))
					return (err);
				gp_set_s_state(asp, SLS_IDLE);
				ss_set_s_state(ssp, SLS_IDLE);
				return (QR_DONE);
			}
	}
	return (-EFAULT);
}

/*
 *  SL_RB_CLEARED_IND
 *  -----------------------------------
 *  We only respond to those signalling link users that have issued a flush buffers command.
 */
STATIC int
slp_rb_cleared_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_FLUSH_BUFFERS);

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure(ss_tst_flags(ssp, ASF_FLUSH_BUFFERS));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (ss_tst_flags(ssu, ASF_FLUSH_BUFFERS)) {
				mblk_t *bp;

				if (!canputnext(ssu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(ssu->oq, bp);
				ss_clr_flags(ssu, ASF_FLUSH_BUFFERS);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_FLUSH_BUFFERS)) {
				if ((err = m2ua_send_maup_state_con(asp, q, request)))
					return (err);
				gp_clr_flags(asp, ASF_FLUSH_BUFFERS);
			}
	}
	ss_clr_flags(ssp, ASF_FLUSH_BUFFERS);
	return (QR_DONE);
}

/*
 *  SL_BSNT_IND
 *  -----------------------------------
 *  We only respond to those signalling link users that have requested bsnt.
 */
STATIC int
slp_bsnt_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	sl_bsnt_ind_t *p = (typeof(p)) mp->b_rptr;
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	static const uint32_t action = __constant_htonl(M2UA_ACTION_RTRV_BSN);
	static const uint32_t result = __constant_htonl(UA_RESULT_SUCCESS);
	uint32_t bsnt = htohl(p->sl_bsnt);

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure(ss_tst_flags(ssp, ASF_BSNT_REQUEST));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (ss_tst_flags(ssu, ASF_BSNT_REQUEST)) {
				mblk_t *bp;

				if (!canputnext(ssu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(ssu->oq, bp);
				ss_clr_flags(ssu, ASF_BSNT_REQUEST);
				if (as->sp.sp->tmode == UA_TMODE_OVERRIDE)
					return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_BSNT_REQUEST)) {
				if ((err = m2ua_send_maup_retr_con(asp, q, action, result, &bsnt)))
					return (err);
				gp_clr_flags(asp, ASF_BSNT_REQUEST);
				if (as->sp.sp->tmode == UA_TMODE_OVERRIDE)
					return (QR_DONE);
			}
	}
	ss_clr_flags(ssp, ASF_BSNT_REQUEST);
	return (QR_DONE);
}

/*
 *  SL_IN_SERVICE_IND
 *  -----------------------------------
 *  We only respond to those signalling link users that have requested a start.
 */
STATIC int
slp_in_service_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		goto discard;
	if (!(as = ssp->as.as))
		goto efault;
	assure(ss_get_s_state(ssp) == SLS_WCON_EREQ);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (ss_get_s_state(ssu) == SLS_WCON_EREQ) {
				mblk_t *bp;

				if (!canputnext(ssu->oq))
					goto ebusy;
				if (!(bp = ss7_dupmsg(q, mp)))
					goto enobufs;
				putnext(ssu->oq, bp);
				ss_set_s_state(ssu, SLS_ESTABLISHED);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_get_s_state(asp) == SLS_WCON_EREQ) {
				if ((err = m2ua_send_maup_estab_con(asp, q)))
					return (err);
				gp_set_s_state(asp, SLS_ESTABLISHED);
			}
	}
	ss_set_s_state(ssp, SLS_ESTABLISHED);
	return (QR_DONE);
      discard:
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return (err);
}

/*
 *  SL_OUT_OF_SERVICE_IND
 *  -----------------------------------
 *  Unfortunately there is no way in M2UA to indicate the reason.
 */
STATIC int
slp_out_of_service_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure((1 << ss_get_s_state(ssp)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if ((1 << ss_get_s_state(ssu)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
				mblk_t *bp;

				if (!canputnext(ssu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(ssu->oq, bp);
				ss_set_s_state(ssu, SLS_IDLE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if ((1 << gp_get_s_state(asp)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
				if ((err = m2ua_send_maup_rel_ind(asp, q)))
					return (err);
				gp_set_s_state(asp, SLS_IDLE);
			}
	}
	ss_set_s_state(ssp, SLS_IDLE);
	return (QR_DONE);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC int
slp_remote_processor_outage_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	static const uint32_t event = __constant_htonl(M2UA_EVENT_RPO_ENTER);

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure((1 << ss_get_s_state(ssp)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (!ss_tst_flags(ssu, ASF_OPERATION_PENDING)) {
				if ((1 << ss_get_s_state(ssu)) &
				    (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
					mblk_t *bp;

					if (!canputnext(ssu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(ssu->oq, bp);
				}
				ss_set_flags(ssu, ASF_OPERATION_PENDING);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if ((1 << gp_get_s_state(asp)) &
				    (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
					if ((err = m2ua_send_maup_state_ind(asp, q, event)))
						return (err);
				}
				gp_set_flags(asp, ASF_OPERATION_PENDING);
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			ss_clr_flags(ssu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			gp_clr_flags(asp, ASF_OPERATION_PENDING);
	}
	return (QR_DONE);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
STATIC int
slp_remote_processor_recovered_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	static const uint32_t event = __constant_htonl(M2UA_EVENT_RPO_EXIT);

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure((1 << ss_get_s_state(ssp)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (!ss_tst_flags(ssu, ASF_OPERATION_PENDING)) {
				if ((1 << ss_get_s_state(ssu)) &
				    (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
					mblk_t *bp;

					if (!canputnext(ssu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(ssu->oq, bp);
				}
				ss_set_flags(ssu, ASF_OPERATION_PENDING);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if ((1 << gp_get_s_state(asp)) &
				    (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
					if ((err = m2ua_send_maup_state_ind(asp, q, event)))
						return (err);
				}
				gp_set_flags(asp, ASF_OPERATION_PENDING);
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			ss_clr_flags(ssu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			gp_clr_flags(asp, ASF_OPERATION_PENDING);
	}
	return (QR_DONE);
}

/*
 *  SL_RTB_CLEARED_IND
 *  -----------------------------------
 */
STATIC int
slp_rtb_cleared_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	static const uint32_t request = __constant_htonl(M2UA_STATUS_CLEAR_RTB);

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure(ss_tst_flags(ssp, ASF_CLEAR_RTB));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (ss_tst_flags(ssu, ASF_CLEAR_RTB)) {
				mblk_t *bp;

				if (!canputnext(ssu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(ssu->oq, bp);
				ss_clr_flags(ssu, ASF_CLEAR_RTB);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_CLEAR_RTB)) {
				if ((err = m2ua_send_maup_state_con(asp, q, request)))
					return (err);
				gp_clr_flags(asp, ASF_CLEAR_RTB);
			}
	}
	ss_clr_flags(ssp, ASF_CLEAR_RTB);
	return (QR_DONE);
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  -----------------------------------
 *  Regardless of traffic mode we only send retrieval complete messages to one signalling link user.  The first user
 *  to set flag ASF_RETRIEVAL succeeds and the others fail.
 */
STATIC int
slp_retrieval_not_possible_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	sl_retrieval_no_poss_ind_t *p = (typeof(p)) mp->b_rptr;
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	static const uint32_t action = __constant_htonl(M2UA_ACTION_RTRV_MSGS);
	static const uint32_t result = __constant_htonl(UA_RESULT_FAILURE);

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure(ss_get_s_state(ssp) == SLS_IDLE);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (ss_tst_flags(ssu, ASF_RETRIEVAL)) {
				if (!canputnext(ssu->oq))
					return (-EBUSY);
				putnext(ssu->oq, mp);
				ss_set_s_state(ssu, SLS_IDLE);
				ss_set_s_state(ssp, SLS_IDLE);
				return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_RETRIEVAL)) {
				if ((err = m2ua_send_maup_retr_con(asp, q, action, result, NULL)))
					return (err);
				gp_set_s_state(asp, SLS_IDLE);
				ss_set_s_state(ssp, SLS_IDLE);
				return (QR_DONE);
			}
	}
	return (-EFAULT);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND
 *  -----------------------------------
 */
STATIC int
slp_bsnt_not_retrievable_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	sl_bsnt_not_retr_ind_t *p = (typeof(p)) mp->b_rptr;
	struct ss *ssu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	static const uint32_t action = __constant_htonl(M2UA_ACTION_RTRV_BSN);
	static const uint32_t result = __constant_htonl(UA_RESULT_FAILURE);

	if (ss_get_i_state(ssp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = ssp->as.as))
		return (-EFAULT);
	assure(ss_tst_flags(ssp, ASF_BSNT_REQUEST));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (ss_tst_flags(ssu, ASF_BSNT_REQUEST)) {
				mblk_t *bp;

				if (!canputnext(ssu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(ssu->oq, bp);
				ss_clr_flags(ssu, ASF_BSNT_REQUEST);
				if (as->sp.sp->tmode == UA_TMODE_OVERRIDE)
					return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_BSNT_REQUEST)) {
				if ((err = m2ua_send_maup_retr_con(asp, q, action, result, NULL)))
					return (err);
				gp_clr_flags(asp, ASF_BSNT_REQUEST);
				if (as->sp.sp->tmode == UA_TMODE_OVERRIDE)
					return (QR_DONE);
			}
	}
	ss_clr_flags(ssp, ASF_BSNT_REQUEST);
	return (QR_DONE);
}

#if 0
/*
 *  SL_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC int
slp_optmgmt_ack(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	/* unexpected */
	swerr();
	return (-EFAULT);
}
#endif

#if 0
/*
 *  SL_NOTIFY_IND
 *  -----------------------------------
 */
STATIC int
slp_notify_ind(struct ss *ssp, queue_t *q, mblk_t *mp)
{
	/* unexpected */
	swerr();
	return (-EFAULT);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  SS-P to M3UA primitives.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * mtpp_ok_ack: - process MTP_OK_ACK from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_OK_ACK message
 */
STATIC int
mtpp_ok_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	struct MTP_ok_ack *p;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	swerr();
	return (QR_DISABLE);
}

/**
 * mtpp_error_ack: - process MTP_ERROR_ACK from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_ERROR_ACK message
 */
STATIC int
mtpp_error_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	swerr();
	return (QR_DISABLE);
}

/**
 * mtpp_bind_ack: - process MTP_BIND_ACK from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_BIND_ACK message
 */
STATIC int
mtpp_bind_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	swerr();
	return (QR_DISABLE);
}

/**
 * mtpp_addr_ack: - process MTP_ADDR_ACK from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_ADDR_ACK message
 */
STATIC int
mtpp_addr_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	swerr();
	return (QR_DISABLE);
}

/**
 * mtpp_info_ack: - process MTP_INFO_ACK from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_INFO_ACK message
 */
STATIC int
mtpp_info_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	swerr();
	return (QR_DISABLE);
}

/**
 * mtpp_optmgmt_ack: - process MTP_OPTMGMT_ACK from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_OPTMGMT_ACK message
 */
STATIC int
mtpp_optmgmt_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	swerr();
	return (QR_DISABLE);
}

/**
 * mtpp_transfer_ind: - process MTP_TRANSFER_IND from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_TRANSFER_IND message
 */
STATIC int
mtpp_transfer_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	struct as *asu;
	struct ap *ap;
	bool delivered = false;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	dassert(mp->b_cont);
	dassert(ss->as.as != NULL);
	if (ss_get_i_state(ss) != MTPS_IDLE && ss_get_i_state(ss) != MTPS_CONNECTED)
		goto discard;
	for (ap = ss->as.as->ap.list; ap; ap = ap->u.next) {
		if (!as_tst_flags((asu = ap->u.as), ASF_ACTIVE | ASF_PENDING))
			continue;
#if 0
		if (asu->lg.list != NULL) {
			struct lg *lgp;
			uint32_t askey = 0;

			if (asu->tmode == UA_TMODE_LOADSHARE) {
				/* Derive a load key for the message according to the load key type 
				   for the AS. For M3UA there are two load key types: SLS and CIC.
				   CIC can have 15 significant bits, so be careful. */
				switch (asu->ktype) {
				default:
					swerr();
				case AS_KTYPE_SLS:
					askey = p->mtp_sls;
					break;
				case AS_KTYPE_CIC:
					/* ITU/Old ANSI - 4096 circuits, New ANSI 32768 circuits,
					   do not mask spare bits */
					askey |= mp->b_cont->b_rptr[1];
					askey <<= 8;
					askey |= mp->b_cont->b_rptr[0];
					break;
				}
			}
			/* Note that tmode is always loadshare for LOADSEL, but may be different
			   for LOADGRP. */
			for (lgp = asu->lg.list; lgp; lgp = lgp->as.next) {
				struct lp *asp;
				uint32_t lskey = 0;

				if (lgp->key.min > askey || askey > lgp->key.max)
					continue;
				if (!(lgp->flags & ASF_ACTIVE))
					continue;
				if (lgp->ldist == UA_TMODE_LOADSHARE) {
					switch (lgp->ktype) {
					default:
						swerr();
					case AS_KTYPE_SLS:
						lskey = p->mtp_sls;
						break;
					case AS_KTYPE_CIC:
						lskey |= mp->b_cont->b_rptr[1];
						lskey <<= 8;
						lskey |= mp->b_cont->b_rptr[0];
						break;
					}
				}
				for (asp = lgp->lp.list; asp; asp = asp->lg.next) {
					if (asp->key.min > lskey || lskey > asp->key.max)
						continue;
					if (!(asp->flags & ASF_ACTIVE))
						continue;
					/* DELIVER THE MESSAGE */
					delivered = true;
					if (lgp->ldist != UA_TMODE_BROADCAST)
						break;
				}
				if (asu->tmode != UA_TMODE_BROADCAST)
					break;
			}
		} else
#endif
		if (asu->gp.list != NULL) {
			struct gp *asp;
			uint32_t askey = 0;

			if (asu->tmode == UA_TMODE_LOADSHARE) {
				/* Derive a load key for the message according to the load key type 
				   for the AS. For M3UA there are two load key types: SLS and CIC.
				   CIC can have 15 significant bits, so be careful. */
				switch (asu->ktype) {
				default:
					swerr();
				case AS_KTYPE_SLS:
					askey = p->mtp_sls;
					break;
				case AS_KTYPE_CIC:
					/* ITU/Old ANSI - 4096 circuits, New ANSI 32768 circuits,
					   do not mask spare bits */
					askey |= mp->b_cont->b_rptr[1];
					askey <<= 8;
					askey |= mp->b_cont->b_rptr[0];
					break;
				}
			}
			for (asp = asu->gp.list; asp; asp = asp->as.next) {
				if (!(asp->flags & ASF_ACTIVE))
					continue;
				if (asp->key.min > askey || askey > asp->key.max)
					continue;
				/* DELIVER THE MESSAGE */
				delivered = true;
				if (asu->tmode != UA_TMODE_BROADCAST)
					break;
			}

		}
		if (!delivered && asu->ss.list != NULL) {
			struct ss *ssu;

			/* Things wind up here if we are not proxying (this is the ASP), or if this 
			   is the SGP, there is no suitable ASP, and there exists an internal
			   "default ASP". */
			for (ssu = asu->ss.list; ssu; ssu = ssu->as.next) {
				mblk_t *bp;

				if (!(ssu->flags & ASF_ACTIVE))
					continue;
				/* DELIVER THE MESSAGE */
				if (unlikely((bp = ss7_copymsg(q, mp)) == NULL))
					return (-ENOBUFS);
				if (unlikely(!canputnext(ssu->oq))) {
					freemsg(bp);
					return (-EBUSY);
				}
				delivered = true;
				putnext(ssu->oq, bp);
				break;
			}
		}
		if (delivered)
			break;
	}
	if (!delivered)
		swerr();	/* SS-Provider should have been shut down. */
	return (QR_DONE);
}

/**
 * mtpp_pause_ind: - process MTP_PAUSE_IND from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_PAUSE_IND message
 * 
 * Note that, if the procedure encounters flow control on one Stream or if it fails to allocate a
 * buffer, the original indication primitive will be placed back on q and this function will be
 * called again when flow control subsides or when a buffer becomes available.  When this procedure
 * is invoked for the second time, it again generates indications to all local and remote MTP users.
 * This means that some local or remote MTP users may receive an additional copy of the indication.
 * Because the nature of the indication is an absolute status, this should not cause a problem with
 * the MTP user.
 *
 * This actually supports a little bit more than OpenSS7 MTP currently indicates:  If there are
 * multiple addresses in the address field of the primitive, we will issue a DUNA with multiple
 * affected point codes.  If there is no address indicated in the address field of the
 * primitive, we assume that it corresponds to all addresses (isolation, or MTP Restart Begins)
 * and issue a DUNA with a wildcard affected point code.
 */
STATIC streams_noinline int
mtpp_pause_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct MTP_pause_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *a;
	struct as *as = ss->as.as;
	struct ap *ap;
	int num_apc;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	dassert(mp->b_cont == NULL);
	dassert(as != NULL);

	if (ss_get_i_state(ss) != MTPS_IDLE && ss_get_i_state(ss) != MTPS_CONNECTED)
		return (-EPROTO);
	if ((p->mtp_addr_length < sizeof(*a)) || (p->mtp_addr_length % sizeof(*a)))
		return (-EPROTO);
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		return (-EPROTO);

	num_apc = p->mtp_addr_length / sizeof(*a);

	for (ap = as->ap.list; ap && as_tst_flags(ap->u.as, ASF_ACTIVE | ASF_PENDING);
	     ap = ap->u.next) {
		{
			struct ss *ssu;

			/* pass a copy to every active local MTP user */
			for (ssu = ap->u.as->ss.list; ssu && ss_tst_flags(ssu, ASF_ACTIVE);
			     ssu = ssu->as.next) {
				mblk_t *bp;

				if (!(bp = ss7_copymsg(q, mp)))
					return (-ENOBUFS);

				if (!bcanputnext(ssu->oq, mp->b_band)) {
					freemsg(bp);
					return (-EBUSY);
				}
				putnext(ssu->oq, bp);
			}
		}
		{
			struct gp *asp;
			uint32_t apc[num_apc ? : 1];
			int i;

			for (i = num_apc, a = (typeof(a)) (mp->b_rptr + p->mtp_addr_offset); i;
			     i--, a++)
				apc[num_apc - i] = htonl(a->pc);
			/* treat missing addresses as wildcarded */
			if (!num_apc)
				apc[0] = __constant_htonl(0xff000000);

			/* pass an indication to every ASP that is active for this AS */
			for (asp = ap->u.as->gp.list; asp && gp_tst_flags(asp, ASF_ACTIVE);
			     asp = asp->as.next) {
				static const char info[] = "MTP provider generated DUNA";

				if ((err = ua_send_snmm_duna(rp, q,
							     &apc, num_apc ? : 1, NULL, NULL,
							     info, sizeof(info))))
					return (err);
			}
		}
	}
	return (QR_DONE);
}

/**
 * mtpp_resume_ind: - process MTP_RESUME_IND from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_RESUME_IND message
 * 
 * Note that, if the procedure encounters flow control on one Stream or if it fails to allocate a
 * buffer, the original indication primitive will be placed back on q and this function will be
 * called again when flow control subsides or when a buffer becomes available.  When this procedure
 * is invoked for the second time, it again generates indications to all local and remote MTP users.
 * This means that some local or remote MTP users may receive an additional copy of the indication.
 * Because the nature of the indication is an absolute status, this should not cause a problem with
 * the MTP user.
 *
 * This actually supports a little bit more than OpenSS7 MTP currently indicates:  If there are
 * multiple addresses in the address field of the primitive, we will issue a DAVA with multiple
 * affected point codes.  If there is no address indicated in the address field of the
 * primitive, we assume that it corresponds to all addresses (all available, or MTP Restart
 * Ends) and issue a DUNA with a wildcard affected point code.
 */
STATIC streams_noinline int
mtpp_resume_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct MTP_resume_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *a;
	struct as *as = ss->as.as;
	struct ap *ap;
	int num_apc;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	dassert(mp->b_cont == NULL);
	dassert(as != NULL);

	if (ss_get_i_state(ss) != MTPS_IDLE && ss_get_i_state(ss) != MTPS_CONNECTED)
		return (-EPROTO);
	if ((p->mtp_addr_length < sizeof(*a)) || (p->mtp_addr_length % sizeof(*a)))
		return (-EPROTO);
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		return (-EPROTO);

	num_apc = p->mtp_addr_length / sizeof(*a);

	for (ap = as->ap.list; ap && as_tst_flags(ap->u.as, ASF_ACTIVE | ASF_PENDING);
	     ap = ap->u.next) {
		{
			struct ss *ssu;

			/* pass a copy to every active local MTP user */
			for (ssu = ap->u.as->ss.list; ssu && ss_tst_flags(ssu, ASF_ACTIVE);
			     ssu = ssu->as.next) {
				mblk_t *bp;

				if (!(bp = ss7_copymsg(q, mp)))
					return (-ENOBUFS);

				if (!bcanputnext(ssu->oq, mp->b_band)) {
					freemsg(bp);
					return (-EBUSY);
				}
				putnext(ssu->oq, bp);
			}
		}
		{
			struct gp *asp;
			uint32_t apc[num_apc ? : 1];
			int i;

			for (i = num_apc, a = (typeof(a)) (mp->b_rptr + p->mtp_addr_offset); i;
			     i--, a++)
				apc[num_apc - i] = htonl(a->pc);
			/* treat missing addresses as wildcarded */
			if (!num_apc)
				apc[0] = __constant_htonl(0xff000000);

			/* pass an indication to every ASP that is active for this AS */
			for (asp = ap->u.as->gp.list; asp && gp_tst_flags(asp, ASF_ACTIVE);
			     asp = asp->as.next) {
				static const char info[] = "MTP provider generated DAVA";

				if ((err = ua_send_snmm_dava(rp, q,
							     &apc, num_apc ? : 1, NULL, NULL,
							     info, sizeof(info))))
					return (err);
			}
		}
	}
	return (QR_DONE);
}

/**
 * mtpp_status_ind: - process MTP_STATUS_IND from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_STATUS_IND message
 * 
 * Note that, if the procedure encounters flow control on one Stream or if it fails to allocate a
 * buffer, the original indication primitive will be placed back on q and this function will be
 * called again when flow control subsides or when a buffer becomes available.  When this procedure
 * is invoked for the second time, it again generates indications to all local and remote MTP users.
 * This means that some local or remote MTP users may receive an additional copy of the indication.
 * Because the nature of the indication is an absolute status, this should not cause a problem with
 * the MTP user.
 *
 * Needed to add one more status type here (MTP_STATUS_TYPE_RESTR) for the purpose of
 * indicating DRST.  The OpenSS7 MTP stack does not currently indicate this status type, so
 * DRST will not be generated.  At some point in the future it might for virutal point codes
 * associated with STP.
 */
STATIC int
mtpp_status_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct MTP_status_ind *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *a;
	struct as *as = ss->as.as;
	struct ap *ap;
	uint32_t apc, status = 0, *congp = NULL;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	dassert(mp->b_cont == NULL);
	dassert(as != NULL);

	if (ss_get_i_state(ss) != MTPS_IDLE && ss_get_i_state(ss) != MTPS_CONNECTED)
		return (-EPROTO);
	if (p->mtp_addr_length < sizeof(*a))
		return (-EPROTO);
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		return (-EPROTO);

	a = (typeof(a)) (mp->b_rptr + p->mtp_addr_offset);
	apc = htonl(a->pc);
	user = a->si;
	status = htonl(p->mtp_status);

	switch (p->mtp_type) {
	case MTP_STATUS_TYPE_CONG:
		switch (p->mtp_status) {
		case MTP_STATUS_CONGESTION_LEVEL0:
		case MTP_STATUS_CONGESTION_LEVEL1:
		case MTP_STATUS_CONGESTION_LEVEL2:
		case MTP_STATUS_CONGESTION_LEVEL3:
			congp = &status;
			break;
		case MTP_STATUS_CONGESTION:
			congp = NULL;
			break;
		default:
			return (-EPROTO);
		}
		break;
	case MTP_STATUS_TYPE_UPU:
		switch (p->mtp_status) {
		case MTP_STATUS_UPU_UNKNOWN:
			break;
		case MTP_STATUS_UPU_UNEQUIPPED:
			break;
		case MTP_STATUS_UPU_INACCESSIBLE:
			break;
		default:
			return (-EPROTO);
		}
		break;
	case MTP_STATUS_TYPE_RESTR:
		break;
	default:
		return (-EPROTO);
	}

	for (ap = as->ap.list; ap && as_tst_flags(ap->u.as, ASF_ACTIVE | ASF_PENDING);
	     ap = ap->u.next) {
		struct ss *ssu;
		struct gp *asp;

		/* pass a copy to every active local MTP user */
		for (ssu = ap->u.as->ss.list; ssu && ss_tst_flags(ssu, ASF_ACTIVE);
		     ssu = ssu->as.next) {
			mblk_t *bp;

			if (!(bp = ss7_copymsg(q, mp)))
				return (-ENOBUFS);

			if (!bcanputnext(ssu->oq, mp->b_band)) {
				freemsg(bp);
				return (-EBUSY);
			}
			putnext(ssu->oq, bp);
		}
		/* pass an indication to every ASP that is active for this AS */
		for (asp = ap->u.as->gp.list; asp && gp_tst_flags(asp, ASF_ACTIVE);
		     asp = asp->as.next) {

			switch (p->mtp_type) {
			case MTP_STATUS_TYPE_UPU:
			{
				static const char info[] = "MTP provider generated DUPU";

				if ((err = ua_send_snmm_dupu(rp, q, p->mtp_status, a->si,
							     &apc, 1, info, sizeof(info))))
					return (err);
				continue;
			}
			case MTP_STATUS_TYPE_CONG:
			{
				static const char info[] = "MTP provider generated SCON";

				if ((err = ua_send_snmm_scon(rp, q, NULL, congp,
							     &apc, 1, NULL, NULL, info,
							     sizeof(info))))
					return (err);
				continue;
			}
			case MTP_STATUS_TYPE_RESTR:
			{
				static const char info[] = "MTP provider generated DRST";

				if ((err = ua_send_snmm_drst(rp, q,
							     &apc, 1, NULL, NULL,
							     info, sizeof(info))))
					return (err);
				continue;
			}
			}
		}
	}
	return (QR_DONE);
}

/**
 * mtpp_restart_begins_ind: - process MTP_RESTART_BEGINS_IND from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_RESTART_BEGINS_IND message
 * 
 * Note that, if the procedure encounters flow control on one Stream or if it fails to allocate a
 * buffer, the original indication primitive will be placed back on q and this function will be
 * called again when flow control subsides or when a buffer becomes available.  When this procedure
 * is invoked for the second time, it again generates indications to all local and remote MTP users.
 * This means that some local or remote MTP users may receive an additional copy of the indication.
 * Because the nature of the indication is an absolute status, this should not cause a problem with
 * the MTP user.
 *
 * Discussion: MTP User expectations are in accordance with Q.701 as follows: When the MTP restart
 * procedure is terminated, the MTP indicates the end of MTP restart to all local MTP Users showing
 * each signalling point's accessibility or inaccessibility.  The means of doing this is
 * implementation dependent (see 9/Q.704).
 *
 * Note that when MTP restart begins on a running signalling point, all destinations have already
 * been marked prohibited and DUNA messages have already been delivered for all of them.  When a
 * node initially starts up, if a user assuming that it can send to a destination sends a message,
 * it will receive an MTP_PAUSE_IND in response.  This primitive is unnecessary and is not issued by
 * the OpenSS7 MTP stack.
 *
 * If, perchance, this primitive is indicated by the MTP provider, it is translated into a DUNA(*)
 * message.
 */
STATIC int
mtpp_restart_begins_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct MTP_restart_begins_ind *p = (typeof(p)) mp->b_rptr;
	struct as *as = ss->as.as;
	struct ap *ap;
	static const uint32_t apc = __constant_htonl(0xff000000);

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	dassert(mp->b_cont == NULL);
	dassert(as != NULL);

	if (ss_get_i_state(ss) != MTPS_IDLE && ss_get_i_state(ss) != MTPS_CONNECTED)
		return (-EPROTO);

	for (ap = as->ap.list; ap && as_tst_flags(ap->u.as, ASF_ACTIVE | ASF_PENDING);
	     ap = ap->u.next) {
		struct ss *ssu;
		struct gp *asp;

		/* pass a copy to every active local MTP user */
		for (ssu = ap->u.as->ss.list; ssu && ss_tst_flags(ssu, ASF_ACTIVE);
		     ssu = ssu->as.next) {
			mblk_t *bp;

			if (!(bp = ss7_copymsg(q, mp)))
				return (-ENOBUFS);

			if (!bcanputnext(ssu->oq, mp->b_band)) {
				freemsg(bp);
				return (-EBUSY);
			}
			putnext(ssu->oq, bp);
		}
		/* pass an indication to every ASP that is active for this AS */
		for (asp = ap->u.as->gp.list; asp && gp_tst_flags(asp, ASF_ACTIVE);
		     asp = asp->as.next) {
			static const char info[] = "MTP provider generated DUNA(*)";

			if ((err = ua_send_snmm_duna(rp, q,
						     &apc, 1, NULL, NULL, info, sizeof(info))))
				return (err);
		}
	}
	return (QR_DONE);
}

/**
 * mtpp_restart_complete_ind: - process MTP_RESTART_COMPLETE_IND from MTP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: MTP_RESTART_COMPLETE_IND message
 * 
 * Note that, if the procedure encounters flow control on one Stream or if it fails to allocate a
 * buffer, the original indication primitive will be placed back on q and this function will be
 * called again when flow control subsides or when a buffer becomes available.  When this procedure
 * is invoked for the second time, it again generates indications to all local and remote MTP users.
 * This means that some local or remote MTP users may receive an additional copy of the indication.
 * Because the nature of the indication is an absolute status, this should not cause a problem with
 * the MTP user.
 *
 * Discussion: MTP User expectations are in accordance with Q.701 as follows: When the MTP restart
 * procedure is terminated, the MTP indicates the end of MTP restart to all local MTP Users showing
 * each signalling point's accessibility or inaccessibility.  The means of doing this is
 * implementation dependent (see 9/Q.704).
 *
 * Note that when isolation began, MTP-PAUSE indications were issued for all accessible
 * destinations for each MTP User.  Therefore, once MTP Restart completes, MTP-RESUME indications
 * can be sent for each available/restricted destination.  Therefore, this primitive is unnecessary
 * and is not issued by the OpenSS7 MTP stack.
 *
 * If, perchance, this pimritive is indicated by the MTP provider, it is translated to SCON[0](*)
 * followed by DAVA(*), that is, all destinations are uncongested and available.  When the MTP user
 * attempts to send to any given destination, an appropriate SCON or DUNA or DRST message will be
 * issued by the MTP provider.
 */
STATIC int
mtpp_restart_complete_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct MTP_restart_complete_ind *p = (typeof(p)) mp->b_rptr;
	struct as *as = ss->as.as;
	struct ap *ap;
	uint32_t cong = 0;
	static const uint32_t apc = __constant_htonl(0xff000000);

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	dassert(mp->b_cont == NULL);
	dassert(as != NULL);

	if (ss_get_i_state(ss) != MTPS_IDLE && ss_get_i_state(ss) != MTPS_CONNECTED)
		return (-EPROTO);

	for (ap = as->ap.list; ap && as_tst_flags(ap->u.as, ASF_ACTIVE | ASF_PENDING);
	     ap = ap->u.next) {
		struct ss *ssu;
		struct gp *asp;

		/* pass a copy to every active local MTP user */
		for (ssu = ap->u.as->ss.list; ssu && ss_tst_flags(ssu, ASF_ACTIVE);
		     ssu = ssu->as.next) {
			mblk_t *bp;

			if (!(bp = ss7_copymsg(q, mp)))
				return (-ENOBUFS);

			if (!bcanputnext(ssu->oq, mp->b_band)) {
				freemsg(bp);
				return (-EBUSY);
			}
			putnext(ssu->oq, bp);
		}
		/* pass an indication to every ASP that is active for this AS */
		for (asp = ap->u.as->gp.list; asp && gp_tst_flags(asp, ASF_ACTIVE);
		     asp = asp->as.next) {
			static const char info1[] = "MTP provider generated SCON[0](*)";
			static const char info2[] = "MTP provider generated DAVA(*)";

			if ((err = ua_send_snmm_scon(rp, q, NULL, &cong,
						     &apc, 1, NULL, NULL, info1, sizeof(info1))))
				return (err);
			if ((err = ua_send_snmm_dava(rp, q,
						     &apc, 1, NULL, NULL, info2, sizeof(info2))))
				return (err);
		}
	}
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SS-P to IUA primitives.
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
dlp_info_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct ss *ssu;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (!(as = ssp->as.as))
		return (-EFAULT);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap_find_ssu(ap); ssu; ssu = ap_find_ssu_next(ap, ssu, 1))
			if (ss_tst_flags(ssu, ASF_INFO_REQUEST)) {
				ss_clr_flags(ssu, ASF_INFO_REQUEST);
				putnext(ssu->oq, mp);
				return (QR_ABSORBED);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_INFO_REQUEST)) {
				if ((err = iua_send_iua_status_ind(asp, q, mp)))
					return (err);
				gp_clr_flags(asp, ASF_INFO_REQUEST);
				return (QR_ABSORBED);
			}
	}
	return (QR_DONE);
}
STATIC int
dlp_bind_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
	int err;
	struct ss *ssu;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (!(as = ssp->as.as))
		return (-EFAULT);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (ssu = ap->u.as->ss.list; ssu; ssu = ssu->as.next)
			if (ss_get_state(ssu) == DL_BIND_PENDING) {
				ss_set_state(ssu, DL_IDLE);
				putnext(ssu->oq, mp);
				return (QR_ABSORBED);
			}
		/* FIXME: no no no no */
		for (asp = ap->u.as->gp.list; asp; asp = asp->as.next)
			if (gp_get_state(asp) == AS_WACK_ASPAC) {
				if ((err = ua_send_aspt_aspac_ack(asp, q, NULL, 0)))
					return (err);
				ss_set_state(ssu, AS_ACTIVE);
				return (QR_ABSORBED);
			}
	}
	return (QR_DONE);
}
STATIC int
dlp_ok_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_error_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_subs_bind_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_unitdata_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_uderror_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_connect_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_connect_con(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_token_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_disconnect_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_reset_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_reset_con(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_data_ack_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_data_ack_status_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_reply_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_reply_status_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_reply_update_status_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_xid_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_xid_con(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_test_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_test_con(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_phys_addr_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}
STATIC int
dlp_get_statistics_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SS-P to SUA primitives.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * np_conn_ind: process N_CONN_IND from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_CONN_IND message
 */
STATIC int
np_conn_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	struct as *as = ss->as.as;
	struct ap *ap;
	int err;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	dassert(mp->b_cont);
	dassert(as != NULL);

	if (ss_get_i_state(ss) != NS_IDLE && ss_get_i_state(ss) != NS_WRES_CIND)
		goto discard;
	for (ap = as->ap.list; ap && as_tst_flags(ap->u.as, ASF_ACTIVE); ap = ap->u.next) {
		/* Here's an idea, instead of walking through the SSU and ASPs, for
		   UA_TMODE_OVERRIDE, set the current controlling SSU or ASP.  For loadshare, use
		   the load selection to find the load group and the distribute over the load group 
		   by load group traffic mode. */
		{
			struct ss *ssu, *ssu_selection = NULL;

			for (ssu = ap->u.as->ss.list; ssu && ss_tst_flags(ssu, ASF_ACTIVE);
			     ssu = ssu->as.next) {
				switch (ap->u.as->tmode) {
				case UA_TMODE_OVERRIDE:
					ssu_selection = ssu;
					break;
				case UA_TMODE_LOADSHARE:
					/* FIXME: need to select SSU by load selection. */
					ssu_selection = ssu;
					continue;
				case UA_TMODE_BROADCAST:
					if ((err = n_conn_ind(ssu, q,
							      seq, flags,
							      &daddr, sizeof(daddr),
							      &saddr, sizeof(saddr),
							      &qos, sizeof(qos), dp)))
						return (err);
					continue;
				}
				break;
			}
			if (ssu_selection)
				if ((err = n_conn_ind(ssu, q,
						      seq, flags,
						      &daddr, sizeof(daddr),
						      &saddr, sizeof(saddr),
						      &qos, sizeof(qos), dp)))
					return (err);
			if (ssu_selection || ssu->u.as->tmode == UA_TMODE_BROADCAST) {
				ss_set_i_state(ss, NS_WRES_CIND);
				return (QR_DONE);
			}
		}
		{
			struct gp *asp, *asp_selection = NULL;

			for (asp = ap->u.as->gp.list; asp && gp_tst_flags(asp, ASF_ACTIVE);
			     asp = asp->as.next) {
				switch (asp->u.as->tmode) {
				case UA_TMODE_OVERRIDE:
					asp_selection = asp;
					break;
				case UA_TMODE_LOADSHARE:
					/* FIXME: need to select ASP by load selection. */
					asp_selection = asp;
					continue;
				case UA_TMODE_BROADCAST:
					issue_it;
					continue;
				}
			}
			if (asp_selection)
				issue_it;
			if (asp_selection || asp->u.as->tmode == UA_TMODE_BROADCAST) {
				ss_set_i_state(ss, NS_WRES_CIND);
				return (QR_DONE);
			}
		}
	}
	/* Ignore it. */
	return (QR_DONE);
}

/**
 * np_conn_con: process N_CONN_CON from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_CONN_CON message
 */
STATIC int
np_conn_con(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_discon_ind: process N_DISCON_IND from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_DISCON_IND message
 */
STATIC int
np_discon_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_data_ind: process N_DATA_IND from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_DATA_IND message
 */
STATIC int
np_data_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_exdata_ind: process N_EXDATA_IND from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_EXDATA_IND message
 */
STATIC int
np_exdata_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_info_ack: process N_INFO_ACK from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_INFO_ACK message
 */
STATIC int
np_info_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_bind_ack: process N_BIND_ACK from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_BIND_ACK message
 */
STATIC int
np_bind_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_error_ack: process N_ERROR_ACK from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_ERROR_ACK message
 */
STATIC int
np_error_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_ok_ack: process N_OK_ACK from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_OK_ACK message
 */
STATIC int
np_ok_ack(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_unitdata_ind: process N_UNITDATA_IND from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_UNITDATA_IND message
 */
STATIC int
np_unitdata_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_uderror_ind: process N_UDERROR_IND from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_UDERROR_IND message
 */
STATIC int
np_uderror_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_datack_ind: process N_DATACK_IND from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_DATACK_IND message
 */
STATIC int
np_datack_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_reset_ind: process N_RESET_IND from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_RESET_IND message
 */
STATIC int
np_reset_ind(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/**
 * np_reset_con: process N_RESET_CON from SCCP Provider
 * @ss: private structure
 * @q: active queue (lower read queue)
 * @mp: N_RESET_CON message
 */
STATIC int
np_reset_con(struct ss *ss, queue_t *q, mblk_t *mp)
{
}

/*
 *  -------------------------------------------------------------------------
 *
 *  XP to UA primitives.
 *
 *  -------------------------------------------------------------------------
 */

/**
 * xp_conn_ind: - process T_CONN_IND message
 * @xp: transport
 * @q: active queue (lower read queue)
 * @mp: the T_CONN_IND message
 *
 * If the UA receives a connection indication it is because the transport is listening on specific
 * port number for incoming connection indications.   The connection indication can be accepted on
 * the same Stream on which it arrived (otherwise, the Stream should not have been linked).  Send a
 * connection response in reply provided that the characteristics of the indicated connection are
 * suitable.  If they are unsuitable, send a disconnect request in response.
 */
STATIC int
xp_conn_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err;
	struct gp *asp, *sgp;
	struct spp *spp, *sp2;

	if (!(spp = xp->spp.list)) {
		if (!(xp->sp))
			goto disable;
		return (-EPROTO);
	}
	switch (spp->type) {
	case UA_OBJ_TYPE_ASP:
		/* FIXME: take some action. */
	case UA_OBJ_TYPE_SGP:
		/* FIXME: take some action. */
	case UA_OBJ_TYPE_SPP:
		/* FIXME: take some action. */
	}
	/* can use this, not expecting it */
	return (-EPROTO);
}

/*
 *  T_CONN_CON
 *  -----------------------------------
 */
STATIC int
xp_conn_con(struct xp *xp, queue_t *q, mblk_t *mp)
{
	/* can use this, not expecting it */
	return (-EPROTO);
}

/*
 *  T_DISCON_IND
 *  -----------------------------------
 */
STATIC int
xp_discon_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err;
	struct gp *asp, *sgp;
	struct spp *spp, *sp2;

	if (!(spp = xp->spp.list)) {
		if (!(xp->sp))
			goto disable;
		return (-EPROTO);
	}
	switch (spp->type) {
	case UA_OBJ_TYPE_ASP:
		switch (spp_get_state(spp)) {
		case ASP_UP:
			asp_pp_set_state(pp, q, ASP_WACK_ASPDN);
			/* fall through */
		case ASP_WACK_ASPDN:
			for (asp = spp->gp.list; asp; asp = asp->spp.next)
				if (gp_get_state(asp) != AS_DOWN)
					if ((err = gp_u_set_state(asp, q, AS_DOWN)))
						return (err);
			asp_pp_set_state(pp, q, ASP_WACK_ASPUP);
			/* fall through */
		case ASP_WACK_ASPUP:
			todo(("Notify management that the SPP has failed\n"));
			asp_pp_set_state(pp, q, ASP_DOWN);
			/* fall through */
		case ASP_DOWN:
			for (sp2 = spp->sp.sp->spp.list; sp2; sp2 = sp2->sp.next) {
				if (sp2 == spp)
					continue;
				if (spp_get_state(sp2) != ASP_DOWN
				    && spp_get_state(sp2) != ASP_WACK_ASPUP) {
					if ((err = ua_send_mgmt_ntfy_asp(sp2, q, spp, NULL, 0)))
						return (err);
					break;
				}
			}
			return (QR_DONE);
		}
		break;
	case UA_OBJ_TYPE_SGP:
		switch (spp_get_state(spp)) {
		case ASP_UP:
			sgp_spp_set_state(spp, q, ASP_WACK_ASPDN);
			/* fall through */
		case ASP_WACK_ASPDN:
			for (sgp = spp->gp.list; sgp; sgp = sgp->spp.next)
				if (gp_get_state(sgp) != AS_DOWN)
					if ((err = gp_p_set_state(sgp, q, AS_DOWN)))
						return (err);
			sgp_spp_set_state(spp, q, ASP_WACK_ASPUP);
			/* fall through */
		case ASP_WACK_ASPUP:
			todo(("Notify management that the SPP has failed\n"));
			sgp_spp_set_state(spp, q, ASP_DOWN);
			/* fall through */
		case ASP_DOWN:
			for (sp2 = spp->sp.sp->spp.list; sp2; sp2 = sp2->sp.next) {
				if (sp2 == spp)
					continue;
				if (spp_get_state(sp2) != ASP_DOWN
				    && spp_get_state(sp2) != ASP_WACK_ASPUP) {
					if ((err = ua_send_mgmt_ntfy_asp(sp2, q, spp, NULL, 0)))
						return (err);
				}
			}
			return (QR_DONE);
		}
		break;
	case UA_OBJ_TYPE_SPP:
		switch (spp_get_state(spp)) {
		}
	}
	swerr();
	return (-EFAULT);
      disable:
	swerr();
	return (QR_DISABLE);
}

/*
 *  M_ERROR, M_HANGUP IND
 *  -----------------------------------
 */
STATIC int
xp_hangup(struct xp *xp, queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  M_DATA IND
 *  -----------------------------------
 */
STATIC int
xp_read(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err;

	return ua_recv_msg(xp, q, mp);
}

/*
 *  T_DATA_IND
 *  -----------------------------------
 */
STATIC int
xp_data_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_data_ind *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	if (p->MORE_flag) {
		mblk_t *rp;

		for (rp = xp->nm_reassem; rp && *(t_uscalar_t *) rp->b_rptr != 0; rp = rp->b_next) ;
		if (rp) {
			linkb(rp, mp->b_cont);
			return (QR_TRIMMED);
		} else {
			mp->b_next = xp->nm_reassem;
			xp->nm_reassem = mp;
			*(t_uscalar_t *) mp->b_rptr = 0;
			return (QR_ABSORBED);
		}
	} else {
		mblk_t **rpp;

		for (rpp = &xp->nm_reassem; *rpp && *(t_uscalar_t *) (*rpp)->b_rptr != 0;
		     rpp = &(*rpp)->b_next) ;
		if (*rpp) {
			linkb(*rpp, mp->b_cont);
			mp->b_cont = (*rpp)->b_cont;
			freeb(xchg(rpp, (*rpp)->b_next));
			return (-EAGAIN);
		}
		return ua_recv_msg(xp, q, mp->b_cont);
	}
}

/*
 *  T_EXDATA_IND
 *  -----------------------------------
 */
STATIC int
xp_exdata_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	if (p->MORE_flag) {
		mblk_t *rp;

		for (rp = xp->ex_reassem; rp && *(t_uscalar_t *) rp->b_rptr != 0; rp = rp->b_next) ;
		if (rp) {
			linkb(rp, mp->b_cont);
			return (QR_TRIMMED);
		} else {
			mp->b_next = xp->ex_reassem;
			xp->ex_reassem = mp;
			*(t_uscalar_t *) mp->b_rptr = 0;
			return (QR_ABSORBED);
		}
	} else {
		mblk_t **rpp;

		for (rpp = &xp->ex_reassem; *rpp && *(t_uscalar_t *) (*rpp)->b_rptr != 0;
		     rpp = &(*rpp)->b_next) ;
		if (*rpp) {
			linkb(*rpp, mp->b_cont);
			mp->b_cont = (*rpp)->b_cont;
			freeb(xchg(rpp, (*rpp)->b_next));
			return (-EAGAIN);
		}
		return ua_recv_msg(xp, q, mp->b_cont);
	}
}

/*
 *  T_INFO_ACK
 *  -----------------------------------
 */
STATIC int
xp_info_ack(struct xp *xp, queue_t *q, mblk_t *mp)
{
	struct T_info_ack *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	xp->info = *p;
	return (QR_DONE);
}

/*
 *  T_BIND_ACK
 *  -----------------------------------
 */
STATIC int
xp_bind_ack(struct xp *xp, queue_t *q, mblk_t *mp)
{
	/* can use this, not expecting it */
	return (-EPROTO);
}

/*
 *  T_ERROR_ACK
 *  -----------------------------------
 */
STATIC int
xp_error_ack(struct xp *xp, queue_t *q, mblk_t *mp)
{
	/* can use this, not expecting it */
	return (-EPROTO);
}

/*
 *  T_OK_ACK
 *  -----------------------------------
 */
STATIC int
xp_ok_ack(struct xp *xp, queue_t *q, mblk_t *mp)
{
	/* can use this, not expecting it */
	return (-EPROTO);
}

/*
 *  T_UNITDATA_IND
 *  -----------------------------------
 */
STATIC int
xp_unitdata_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	/* can use this, not expecting it */
	return (-EPROTO);
}

/*
 *  T_UDERROR_IND
 *  -----------------------------------
 */
STATIC int
xp_uderror_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	/* can use this, not expecting it */
	return (-EPROTO);
}

/*
 *  T_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC int
xp_optmgmt_ack(struct xp *xp, queue_t *q, mblk_t *mp)
{
	/* can use this, not expecting it */
	return (-EPROTO);
}

/*
 *  T_ORDREL_IND
 *  -----------------------------------
 */
STATIC int
xp_ordrel_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err;
	struct gp *asp, *sgp;
	struct spp *spp, *sp2;

	if (!(spp = xp->spp.list)) {
		if (!(xp->sp))
			goto disable;
		return (-EPROTO);
	}
	switch (spp->type) {
	case UA_OBJ_TYPE_ASP:
		switch (spp_get_state(spp)) {
		case ASP_UP:
			asp_pp_set_state(pp, q, ASP_WACK_ASPDN);
			/* fall through */
		case ASP_WACK_ASPDN:
			for (asp = spp->gp.list; asp; asp = asp->spp.next)
				if (gp_get_state(asp) != AS_DOWN)
					if ((err = gp_u_set_state(asp, q, AS_DOWN)))
						return (err);
			asp_pp_set_state(pp, q, ASP_WACK_ASPUP);
			/* fall through */
		case ASP_WACK_ASPUP:
			todo(("Notify management that the SPP has failed\n"));
			asp_pp_set_state(pp, q, ASP_DOWN);
			/* fall through */
		case ASP_DOWN:
			for (sp2 = spp->sp.sp->spp.list; sp2 && sp2 != spp; sp2 = sp2->sp.next)
				if (spp_get_state(sp2) != ASP_DOWN
				    && spp_get_state(sp2) != ASP_WACK_ASPUP)
					if ((err = ua_send_mgmt_ntfy_asp(sp2, q, spp, NULL, 0)))
						return (err);
			return (QR_DONE);
		}
		break;
	case UA_OBJ_TYPE_SGP:
		switch (spp_get_state(spp)) {
		case ASP_UP:
			sgp_spp_set_state(spp, q, ASP_WACK_ASPDN);
			/* fall through */
		case ASP_WACK_ASPDN:
			for (sgp = spp->gp.list; sgp; sgp = sgp->spp.next)
				if (gp_get_state(sgp) != AS_DOWN)
					if ((err = gp_p_set_state(sgp, q, AS_DOWN)))
						return (err);
			sgp_spp_set_state(spp, q, ASP_WACK_ASPUP);
			/* fall through */
		case ASP_WACK_ASPUP:
			todo(("Notify management that the SPP has failed\n"));
			sgp_spp_set_state(spp, q, ASP_DOWN);
			/* fall through */
		case ASP_DOWN:
			for (sp2 = spp->sp.sp->spp.list; sp2 && sp2 != spp; sp2 = sp2->sp.next)
				if (spp_get_state(sp2) != ASP_DOWN
				    && spp_get_state(sp2) != ASP_WACK_ASPUP)
					if ((err = ua_send_mgmt_ntfy_asp(sp2, q, spp, NULL, 0)))
						return (err);
			return (QR_DONE);
		}
		break;
	}
	swerr();
	return (-EFAULT);
      disable:
	swerr();
	return (QR_DISABLE);
}

/*
 *  T_OPTDATA_IND
 *  -----------------------------------
 */
STATIC int
xp_optdata_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err;
	struct T_optdata_ind *p = (typeof(p)) mp->b_rptr;
	t_uscalar_t sid = 0;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	if (p->OPT_length) {
		uchar *op = mp->b_rptr + p->OPT_offset;
		uchar *oe = op + p->OPT_length;
		struct t_opthdr *oh = (struct t_opthdr *) op;

		dassert(mp->b_wptr >= oe);

		/* parse the options */
		for (; op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh) && op + oh->len <= oe;
		     op += oh->len, oh = (typeof(oh)) op)
			if (oh->level == T_INET_SCTP && oh->name == T_SCTP_SID)
				sid = *((t_scalar_t *) (oh + 1)) & 0xffff;
	}
	if (p->DATA_flag & T_ODF_EX) {
		if (p->DATA_flag & T_ODF_MORE) {
			mblk_t *rp;

			for (rp = xp->ex_reassem; rp && *(t_uscalar_t *) rp->b_rptr != sid;
			     rp = rp->b_next) ;
			if (rp) {
				linkb(rp, mp->b_cont);
				return (QR_TRIMMED);
			} else {
				mp->b_next = xp->ex_reassem;
				xp->ex_reassem = mp;
				*(t_uscalar_t *) mp->b_rptr = sid;
				return (QR_ABSORBED);
			}
		} else {
			mblk_t **rpp;

			for (rpp = &xp->ex_reassem; *rpp && *(t_uscalar_t *) (*rpp)->b_rptr != sid;
			     rpp = &(*rpp)->b_next) ;
			if (*rpp) {
				linkb(*rpp, mp->b_cont);
				mp->b_cont = (*rpp)->b_cont;
				freeb(xchg(rpp, (*rpp)->b_next));
				return (-EAGAIN);
			}
			return ua_recv_msg(xp, q, mp->b_cont);
		}
	} else {
		if (p->DATA_flag & T_ODF_MORE) {
			mblk_t *rp;

			for (rp = xp->nm_reassem; rp && *(t_uscalar_t *) rp->b_rptr != sid;
			     rp = rp->b_next) ;
			if (rp) {
				linkb(rp, mp->b_cont);
				return (QR_TRIMMED);
			} else {
				mp->b_next = xp->nm_reassem;
				xp->nm_reassem = mp;
				*(t_uscalar_t *) mp->b_rptr = sid;
				return (QR_ABSORBED);
			}
		} else {
			mblk_t **rpp;

			for (rpp = &xp->nm_reassem; *rpp && *(t_uscalar_t *) (*rpp)->b_rptr != sid;
			     rpp = &(*rpp)->b_next) ;
			if (*rpp) {
				linkb(*rpp, mp->b_cont);
				mp->b_cont = (*rpp)->b_cont;
				freeb(xchg(rpp, (*rpp)->b_next));
				return (-EAGAIN);
			}
			return ua_recv_msg(xp, q, mp->b_cont);
		}
	}
}

#ifdef T_ADDR_ACK
/*
 *  T_ADDR_ACK
 *  -----------------------------------
 *  This might not be necessary.
 */
STATIC int
xp_addr_ack(struct xp *xp, queue_t *q, mblk_t *mp)
{
	struct T_addr_ack *p = (typeof(p)) mp->b_rptr;
	uchar *loc_ptr = NULL;
	size_t loc_len = 0;
	uchar *rem_ptr = NULL;
	size_t rem_len = 0;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	if ((loc_len = p->LOCADDR_length)) {
		loc_ptr = mp->b_rptr + p->LOCADDR_offset;
		dassert(mp->b_wptr >= loc_ptr + loc_len);
		if (loc_len <= sizeof(xp->loc))
			bcopy(loc_ptr, &xp->loc, sizeof(xp->loc));
	}
	if ((rem_len = p->REMADDR_length)) {
		rem_ptr = mp->b_rptr + p->REMADDR_offset;
		dassert(mp->b_wptr >= rem_ptr + rem_len);
		if (rem_len <= sizeof(xp->rem))
			bcopy(rem_ptr, &xp->rem, sizeof(xp->rem));
	}
	return (QR_DONE);
}
#endif
#ifdef T_CAPABILITY_ACK
/*
 *  T_CAPABILITY_ACK
 *  -----------------------------------
 */
STATIC int
xp_capability_ack(struct xp *xp, queue_t *q, mblk_t *mp)
{
	/* can use this, not expecting it */
	return (-EPROTO);
}
#endif

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 *
 *  SL IO Controls
 *
 *  -------------------------------------------------------------------------
 */

STATIC int
sl_iocpass(queue_t *q, mblk_t *mp)
{
	int ret = 0;
	struct ss *slu = SS_PRIV(q), *slp = NULL;
	struct as *as_u, *as_p;
	struct ap *ap;

	spin_lock_ua(&master.lock);
	{
		if ((as_u = slu->as.as))
			for (ap = as_u->ap.list; ap; ap = ap->p.next)
				if ((as_p = ap->p.as))
					if ((slp = as_p->ss.list))
						break;
		if (slp && slp->oq) {
			if (slp->ioc)
				ret = -EBUSY;
			else {
				slp->ioc = ss_get(slu);
				putnext(slp->oq, mp);
				ret = QR_ABSORBED;
			}
		} else {
			ret = -EOPNOTSUPP;
		}
	}
	spin_unlock_ua(&master.lock);
	return (ret);
}
STATIC int
sl_ackpass(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct ss *slp = SS_PRIV(q), *slu;

		if (slp->ioc) {
			ss_put((slu = xchg(&slp->ioc, NULL)));
			if (slu->oq) {
				putnext(slu->oq, mp);
				return (QR_ABSORBED);
			}
		}
	}
	rare();
	return (-EFAULT);
}

/*
 *  SL_IOCGOPTIONS
 *  -----------------------------------
 */
STATIC int
sl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		struct ss *slu = SS_PRIV(q), *slp = NULL;
		struct as *as_u, *as_p;
		struct ap *ap;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_ua(&master.lock);
		{
			if ((as_u = slu->as.as))
				for (ap = as_u->ap.list; ap; ap = ap->p.next)
					if ((as_p = ap->p.as))
						if ((slp = as_p->ss.list))
							break;
			if (slp && slp->oq) {
				if (slp->ioc)
					ret = -EBUSY;
				else {
					slp->ioc = ss_get(slu);
					putnext(slp->oq, mp);
					ret = QR_ABSORBED;
				}
			} else {
				*arg = slu->proto;
			}
		}
		spin_unlock_ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ackgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct ss *slp = SS_PRIV(q), *slu;

		if (slp->ioc) {
			lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

			ss_put((slu = xchg(&slp->ioc, NULL)));
			slu->proto = *arg;
			if (slu->oq) {
				putnext(slu->oq, mp);
				return (QR_ABSORBED);
			}
		}
	}
	rare();
	return (-EFAULT);
}

/*
 *  SL_IOCSOPTIONS
 *  -----------------------------------
 */
STATIC int
sl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = 0;
		struct ss *slu = SS_PRIV(q), *slp = NULL;
		struct as *as_u, *as_p;
		struct ap *ap;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_ua(&master.lock);
		{
			if ((as_u = slu->as.as))
				for (ap = as_u->ap.list; ap; ap = ap->p.next)
					if ((as_p = ap->p.as))
						if ((slp = as_p->ss.list))
							break;
			if (slp && slp->oq) {
				if (slp->ioc)
					ret = -EBUSY;
				else {
					slp->ioc = ss_get(slu);
					putnext(slp->oq, mp);
					ret = QR_ABSORBED;
				}
			} else {
				slu->proto = *arg;
			}
		}
		spin_unlock_ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_acksoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct ss *slp = SS_PRIV(q), *slu;

		if (slp->ioc) {
			lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

			ss_put((slu = xchg(&slp->ioc, NULL)));
			slu->proto = *arg;
			if (slu->oq) {
				putnext(slu->oq, mp);
				return (QR_ABSORBED);
			}
		}
	}
	rare();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M2UA IO Controls
 *
 *  -------------------------------------------------------------------------
 *
 *  Utility Functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  GET Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_opt_get_as(ua_option_t * arg, struct as *as, int size)
{
	ua_opt_conf_as_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!as)
		return (-EINVAL);
	*opt = as->config;
	return (QR_DONE);
}
STATIC int
ua_opt_get_sp(ua_option_t * arg, struct sp *sp, int size)
{
	ua_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sp)
		return (-EINVAL);
	*opt = sp->config;
	return (QR_DONE);
}
STATIC int
ua_opt_get_spp(ua_option_t * arg, struct spp *spp, int size)
{
	ua_opt_conf_spp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!spp)
		return (-EINVAL);
	*opt = spp->config;
	return (QR_DONE);
}
STATIC int
ua_opt_get_ss(ua_option_t * arg, struct ss *ss, int size)
{
	ua_opt_conf_ss_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!ss)
		return (-EINVAL);
	*opt = ss->config;
	return (QR_DONE);
}
STATIC int
ua_opt_get_xp(ua_option_t * arg, struct xp *xp, int size)
{
	ua_opt_conf_xp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!xp)
		return (-EINVAL);
	*opt = xp->config;
	return (QR_DONE);
}
STATIC int
ua_opt_get_df(ua_option_t * arg, struct df *df, int size)
{
	ua_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!df)
		return (-EINVAL);
	*opt = df->config;
	return (QR_DONE);
}

/*
 *  SET Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_opt_set_as(ua_option_t * arg, struct as *as, int size)
{
	ua_opt_conf_as_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!as)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	as->config = *opt;
	return (QR_DONE);
}
STATIC int
ua_opt_set_sp(ua_option_t * arg, struct sp *sp, int size)
{
	ua_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sp)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	sp->config = *opt;
	return (QR_DONE);
}
STATIC int
ua_opt_set_spp(ua_option_t * arg, struct spp *spp, int size)
{
	ua_opt_conf_spp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!spp)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	spp->config = *opt;
	return (QR_DONE);
}
STATIC int
ua_opt_set_ss(ua_option_t * arg, struct ss *ss, int size)
{
	ua_opt_conf_ss_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!ss)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	ss->config = *opt;
	return (QR_DONE);
}
STATIC int
ua_opt_set_xp(ua_option_t * arg, struct xp *xp, int size)
{
	ua_opt_conf_xp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!xp)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	xp->config = *opt;
	return (QR_DONE);
}
STATIC int
ua_opt_set_df(ua_option_t * arg, struct df *df, int size)
{
	ua_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!df)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	df->config = *opt;
	return (QR_DONE);
}

/*
 *  GET Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_get_as(ua_config_t * arg, struct as *as, int size)
{
	ua_conf_as_t *cnf = (typeof(cnf)) (arg + 1);
	struct ap *ap;
	ua_conf_as_t *cha;
	struct ss *ss;
	ua_conf_ss_t *chd;

	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!as)
		return (-EINVAL);
	/* write out queried object */
	cnf->spid = as->sp.sp ? as->sp.sp->id : 0;
	cnf->iid = as->iid;
	cnf->add = as->add;
	arg = (typeof(arg)) (cnf + 1);
	/* write out the list of associated AS */
	cha = (typeof(cha)) (arg + 1);
	for (ap = as->ap.list; ap && size >= sizeof(*arg) + sizeof(*cha) + sizeof(*arg);
	     ap = (ap->u.as == as) ? ap->p.next : ap->u.next, size -=
	     sizeof(*arg) + sizeof(*cha), arg = (typeof(arg)) (cha + 1), cha =
	     (typeof(cha)) (arg + 1)) {
		struct as *oas = (as == ap->u.as) ? ap->p.as : ap->u.as;

		arg->type = oas->type;
		arg->id = oas->id;
		cha->spid = oas->sp.sp ? oas->sp.sp->id : 0;
		cha->iid = oas->iid;
		cha->add = oas->add;
	}
	/* write out the list of SL */
	chd = (typeof(chd)) (arg + 1);
	for (ss = as->ss.list; ss && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     ss = ss->as.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = ss->type;
		arg->id = ss->id;
		chd->asid = as->id;
		chd->muxid = ss->u.mux.index;
		chd->iid = ss->iid;
		chd->add = ss->add;
		chd->proto = ss->proto;
	}
	/* terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
ua_get_sp(ua_config_t * arg, struct sp *sp, int size)
{
	ua_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);
	struct np *np;
	ua_conf_sp_t *chp;
	struct as *as;
	ua_conf_as_t *cha;
	struct spp *spp;
	ua_conf_spp_t *chs;

	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!sp)
		return (-EINVAL);
	/* write out queried object */
	cnf->spid = 0;
	cnf->cost = sp->cost;
	cnf->tmode = sp->tmode;
	arg = (typeof(arg)) (cnf + 1);
	chp = (typeof(chp)) (arg + 1);
	/* write out the list of associated SPs */
	for (np = sp->np.list; np && size >= sizeof(*arg) + sizeof(*chp) + sizeof(*arg);
	     np = (sp == np->u.sp) ? np->p.next : np->u.next, size -=
	     sizeof(*arg) + sizeof(*chp), arg = (typeof(arg)) (chp + 1), chp =
	     (typeof(chp)) (arg + 1)) {
		struct sp *osp = (sp == np->u.sp) ? np->p.sp : np->u.sp;

		arg->type = osp->type;
		arg->id = osp->id;
		chp->spid = sp->id;
		chp->cost = osp->cost;
		chp->tmode = osp->tmode;
	}
	/* write out the list of AS */
	cha = (typeof(cha)) (arg + 1);
	for (as = sp->as.list; as && size >= sizeof(*arg) + sizeof(*cha) + sizeof(*arg);
	     as = as->sp.next, size -= sizeof(*arg) + sizeof(*cha), arg =
	     (typeof(arg)) (cha + 1), cha = (typeof(cha)) (arg + 1)) {
		arg->type = as->type;
		arg->id = as->id;
		cha->spid = sp->id;
		cha->iid = as->iid;
		cha->add = as->add;
	}
	/* write out the list of SPP */
	chs = (typeof(chs)) (arg + 1);
	for (spp = sp->spp.list; spp && size >= sizeof(*arg) + sizeof(*chs) + sizeof(*arg);
	     spp = spp->sp.next, size -= sizeof(*arg) + sizeof(*chs), arg =
	     (typeof(arg)) (chs + 1), chs = (typeof(chs)) (arg + 1)) {
		arg->type = spp->type;
		arg->id = spp->id;
		chs->spid = sp->id;
		chs->aspid = spp->aspid;
		chs->cost = spp->cost;
	}
	/* terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
ua_get_spp(ua_config_t * arg, struct spp *spp, int size)
{
	ua_conf_spp_t *cnf = (typeof(cnf)) (arg + 1);
	struct xp *xp;
	ua_conf_xp_t *chd;

	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!spp)
		return (-EINVAL);
	/* write out queried object */
	cnf->spid = spp->sp.sp ? spp->sp.sp->id : 0;
	cnf->aspid = spp->aspid;
	cnf->cost = spp->cost;
	arg = (typeof(arg)) (cnf + 1);
	/* write out list of XP */
	chd = (typeof(chd)) (arg + 1);
	if ((xp = spp->xp.xp) && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg)) {
		arg->type = xp->type;
		arg->id = xp->id;
		chd->sppid = spp->id;
		chd->spid = xp->sp ? xp->sp->id : (spp->sp.sp ? spp->sp.sp->id : 0);
		chd->muxid = xp->u.mux.index;
		arg = (typeof(arg)) (chd + 1);
	}
	/* terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
ua_get_ss(ua_config_t * arg, struct ss *ss, int size)
{
	ua_conf_ss_t *cnf = (typeof(cnf)) (arg + 1);

	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!ss)
		return (-EINVAL);
	/* write out queried object */
	cnf->asid = ss->as.as ? ss->as.as->id : 0;
	cnf->muxid = ss->u.mux.index;
	cnf->iid = ss->iid;
	cnf->add = ss->add;
	cnf->proto = ss->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
ua_get_xp(ua_config_t * arg, struct xp *xp, int size)
{
	ua_conf_xp_t *cnf = (typeof(cnf)) (arg + 1);

	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!xp)
		return (-EINVAL);
	/* write out queried object */
	cnf->sppid = xp->spp.list ? xp->spp.list->id : 0;
	cnf->spid =
	    xp->sp ? xp->sp->id : ((xp->spp.list && xp->spp.list->sp.sp) ? xp->spp.list->sp.sp->
				   id : 0);
	cnf->muxid = xp->u.mux.index;
	arg = (typeof(arg)) (cnf + 1);
	/* terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
ua_get_df(ua_config_t * arg, struct df *df, int size)
{
	ua_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	struct sp *sp;
	ua_conf_sp_t *chd;

	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!df)
		return (-EINVAL);
	/* write out queried object */
	cnf->proto = df->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* write out list of SP */
	chd = (typeof(chd)) (arg + 1);
	for (sp = df->sp.list; sp && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sp = sp->next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = sp->type;
		arg->id = sp->id;
		chd->spid = 0;
		chd->cost = sp->cost;
		chd->tmode = sp->tmode;
	}
	/* terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}

/*
 *  ADD Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_add_as(ua_config_t * arg, struct as *as, int size, int force, int test)
{
	struct sp *sp = NULL;
	ua_conf_as_t *cnf = (typeof(cnf)) (arg + 1);

	if (as || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (!sp)
		return (-EINVAL);
	for (as = sp->as.list; as; as = as->sp.next)
		if (as->iid == cnf->iid)
			return (-EINVAL);
	/* make sure user has specified correct types */
	switch (arg->type) {
	case UA_OBJ_TYPE_AS_U:
		if (sp->type != UA_OBJ_TYPE_SP)
			return (-EINVAL);
		break;
	case UA_OBJ_TYPE_AS_P:
		if (sp->type != UA_OBJ_TYPE_SG)
			return (-EINVAL);
		break;
	default:
		swerr();
		return (-EFAULT);
	}
	if (!test) {
		if (!(as = ua_alloc_as(as_get_id(arg->id), arg->type, sp, cnf->iid, &cnf->add)))
			return (-ENOMEM);
		arg->id = as->id;
	}
	return (QR_DONE);
}
STATIC int
ua_add_sp(ua_config_t * arg, struct sp *sp, int size, int force, int test)
{
	struct sp *osp = NULL;
	ua_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);

	if (sp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		osp = sp_lookup(cnf->spid);
	/* make sure user has specified correct types */
	switch (arg->type) {
	case UA_OBJ_TYPE_SP:
		if (osp)
			return (-EINVAL);
		break;
	case UA_OBJ_TYPE_SG:
		if (!osp)
			return (-EINVAL);
		if (osp->type != UA_OBJ_TYPE_SP)
			return (-EINVAL);
		break;
	default:
		swerr();
		goto efault;
	}
	if (!test) {
		if (!(sp = ua_alloc_sp(sp_get_id(arg->id), arg->type, osp, cnf->cost, cnf->tmode)))
			goto enomem;
		arg->id = sp->id;
	}
	return (QR_DONE);
      einval:
	return (-EINVAL);
      enomem:
	return (-ENOMEM);
      efault:
	swerr();
	return (-EFAULT);
}
STATIC int
ua_add_spp(ua_config_t * arg, struct spp *spp, int size, int force, int test)
{
	struct sp *sp = NULL;
	ua_conf_spp_t *cnf = (typeof(cnf)) (arg + 1);

	if (spp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (!sp)
		return (-EINVAL);
	if (cnf->aspid)
		for (spp = sp->spp.list; spp; spp = spp->sp.next)
			if (spp->aspid == cnf->aspid)
				return (-EINVAL);
	/* make sure user has specified correct types */
	switch (arg->type) {
	case UA_OBJ_TYPE_ASP:
		if (sp->type != UA_OBJ_TYPE_SP)
			return (-EINVAL);
		break;
	case UA_OBJ_TYPE_SGP:
		if (sp->type != UA_OBJ_TYPE_SG)
			return (-EINVAL);
		break;
	case UA_OBJ_TYPE_SPP:
		if ((sp->type != UA_OBJ_TYPE_SP) && (sp->type != UA_OBJ_TYPE_SG))
			return (-EINVAL);
		break;
	default:
		swerr();
		goto efault;
	}
	if (!test) {
		if (!(spp = ua_alloc_spp(spp_get_id(arg->id), arg->type, sp, cnf->aspid,
					 cnf->cost)))
			goto enomem;
		arg->id = spp->id;
	}
	return (QR_DONE);
      einval:
	return (-EINVAL);
      enomem:
	return (-ENOMEM);
      efault:
	swerr();
	return (-EFAULT);
}
STATIC int
ua_add_ss(ua_config_t * arg, struct ss *ss, int size, int force, int test)
{
	struct as *as = NULL;
	ua_conf_ss_t *cnf = (typeof(cnf)) (arg + 1);

	if (ss || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->asid)
		as = as_lookup(cnf->asid);
	if (!as)
		return (-EINVAL);
	if (!(ss = (struct ss *) link_lookup(cnf->muxid)))
		return (-EINVAL);
	/* already typed */
	if (ss->type)
		return (-EINVAL);
	/* no, sorry, we can't link SL-Us */
	if (arg->type != UA_OBJ_TYPE_SL_P)
		return (-EINVAL);
	if (!test) {
		ua_alloc_ss(ss, ss_get_id(arg->id), arg->type, as, cnf->iid, &cnf->add);
		arg->id = ss->id;
	}
	return (QR_DONE);
      einval:
	return (-EINVAL);
}
STATIC int
ua_add_xp(ua_config_t * arg, struct xp *xp, int size, int force, int test)
{
	struct sp *sp = NULL;
	struct spp *spp = NULL;
	ua_conf_xp_t *cnf = (typeof(cnf)) (arg + 1);

	if (xp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (cnf->sppid)
		spp = spp_lookup(cnf->sppid);
	/* must specify one or the other */
	if ((!spp && !sp) || (spp && sp))
		return (-EINVAL);
	if (!(xp = (struct xp *) link_lookup(cnf->muxid)))
		return (-EINVAL);
	/* already typed */
	if (xp->type)
		return (-EINVAL);
	if (!test) {
		int err;

		if ((err = ua_alloc_xp(xp, xp_get_id(arg->id), arg->type, spp, sp)))
			return (err);
		arg->id = xp->id;
	}
	return (QR_DONE);
      einval:
	return (-EINVAL);
}
STATIC int
ua_add_df(ua_config_t * arg, struct df *df, int size, int force, int test)
{
	ua_conf_df_t *cnf = (typeof(cnf)) (arg + 1);

	if (df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	return (-EINVAL);
      einval:
	return (-EINVAL);
}

/*
 *  CHA Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_cha_as(ua_config_t * arg, struct as *as, int size, int force, int test)
{
	struct as *a;
	ua_conf_as_t *cnf = (typeof(cnf)) (arg + 1);

	if (!as || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid && cnf->spid != as->sp.sp->id)
		return (-EINVAL);
	/* can't change to existing iid */
	if (cnf->iid && cnf->iid != as->iid)
		for (a = as->sp.sp->as.list; a; a = a->sp.next)
			if (a->iid == cnf->iid)
				return (-EINVAL);
	if (!force) {
		/* involved with peer */
		if (as_get_state(as) != AS_INACTIVE)
			return (-EBUSY);
	}
	if (!test) {
		as->iid = cnf->iid;
		as->add = cnf->add;
	}
	return (QR_DONE);
}
STATIC int
ua_cha_sp(ua_config_t * arg, struct sp *sp, int size, int force, int test)
{
	ua_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);

	if (!sp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		return (-EINVAL);
	if (!force) {
		/* has active AS */
		if (sp_get_state(sp) != AS_INACTIVE)
			return (-EBUSY);
	}
	if (!test) {
		sp->cost = cnf->cost;
		sp->tmode = cnf->tmode;
	}
	return (QR_DONE);
}
STATIC int
ua_cha_spp(ua_config_t * arg, struct spp *spp, int size, int force, int test)
{
	ua_conf_spp_t *cnf = (typeof(cnf)) (arg + 1);

	if (!spp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid && cnf->spid != spp->sp.sp->id)
		return (-EINVAL);
	if (!force) {
		/* is active with peer */
		if (spp_get_state(spp) != ASP_DOWN)
			return (-EBUSY);
	}
	if (!test) {
		spp->aspid = cnf->aspid;
		spp->cost = cnf->cost;
	}
	return (QR_DONE);
}
STATIC int
ua_cha_ss(ua_config_t * arg, struct ss *ss, int size, int force, int test)
{
	struct as *a;
	struct ss *s;
	ua_conf_ss_t *cnf = (typeof(cnf)) (arg + 1);

	if (!ss || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->asid && cnf->asid != ss->as.as->id)
		return (-EINVAL);
	if (cnf->muxid && cnf->muxid != ss->u.mux.index)
		return (-EINVAL);
	/* can't change to existing iid */
	if (cnf->iid && cnf->iid != ss->iid)
		for (a = ss->as.as->sp.sp->as.list; a; a = a->sp.next)
			for (s = a->ss.list; s; s = s->as.next)
				if (s->iid == cnf->iid)
					return (-EINVAL);
	if (!force) {
		/* involved providing service */
		if (as_get_state(ss->as.as) != AS_INACTIVE)
			return (-EBUSY);
	}
	if (!test) {
		ss->iid = cnf->iid;
		ss->add = cnf->add;
		ss->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
ua_cha_xp(ua_config_t * arg, struct xp *xp, int size, int force, int test)
{
	ua_conf_xp_t *cnf = (typeof(cnf)) (arg + 1);

	if (!xp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->sppid && (!xp->spp.list || cnf->sppid != xp->spp.list->id))
		return (-EINVAL);
	if (cnf->spid && (!xp->sp || cnf->spid != xp->sp->id))
		return (-EINVAL);
	if (cnf->muxid && cnf->muxid != xp->u.mux.index)
		return (-EINVAL);
	if (!force) {
		/* nothing to change */
	}
	if (!test) {
		/* nothing to change */
	}
	return (QR_DONE);
}
STATIC int
ua_cha_df(ua_config_t * arg, struct df *df, int size, int force, int test)
{
	ua_conf_df_t *cnf = (typeof(cnf)) (arg + 1);

	if (!df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		df->proto = cnf->proto;
	}
	return (QR_DONE);
}

/*
 *  DEL Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_del_as(ua_config_t * arg, struct as *as, int size, int force, int test)
{
	if (!as)
		return (-EINVAL);
	if (!force) {
		/* attached to signalling link */
		if (as->ss.list)
			return (-EBUSY);
		/* attached to SPP */
		if (as->gp.list)
			return (-EBUSY);
		/* not in idle state */
		if (as_get_state(as) != 0)
			return (-EBUSY);
#if 0
		/* attached to other AS */
		if (as->ap.list)
			return (-EBUSY);
#endif
	}
	if (!test) {
		ua_free_as(as);
	}
	return (QR_DONE);
}
STATIC int
ua_del_sp(ua_config_t * arg, struct sp *sp, int size, int force, int test)
{
	if (!sp)
		return (-EINVAL);
	if (!force) {
		/* attached AS */
		if (sp->as.list)
			return (-EBUSY);
		/* attached SPP */
		if (sp->spp.list)
			return (-EBUSY);
		/* not in idle state */
		if (sp_get_state(sp) != 0)
			return (-EBUSY);
#if 0
		/* attached to other SP */
		if (sp->np.list)
			return (-EBUSY);
#endif
	}
	if (!test) {
		ua_free_sp(sp);
	}
	return (QR_DONE);
}
STATIC int
ua_del_spp(ua_config_t * arg, struct spp *spp, int size, int force, int test)
{
	if (!spp)
		return (-EINVAL);
	if (!force) {
		/* attached to XP */
		if (spp->xp.xp)
			return (-EBUSY);
		/* attached to AS */
		if (spp->gp.list)
			return (-EBUSY);
		/* not in idle state */
		if (spp_get_state(spp) != 0)
			return (-EBUSY);
#if 0
		/* attached to SP */
		if (spp->sp.sp)
			return (-EBUSY);
#endif
	}
	if (!test) {
		ua_free_spp(spp);
	}
	return (QR_DONE);
}
STATIC int
ua_del_ss(ua_config_t * arg, struct ss *ss, int size, int force, int test)
{
	if (!ss)
		return (-EINVAL);
	if (!force) {
		/* bound to internal datastructures */
		if (ss->as.as)
			return (-EBUSY);
	}
	if (!test) {
		noenable(ss->oq);
		noenable(ss->iq);
		if (ss->as.as) {
			fixme(("Notify any active AS that we have gone away.\n"));
			if ((*ss->as.prev = ss->as.next))
				ss->as.next->as.prev = ss->as.prev;
			ss->as.next = NULL;
			ss->as.prev = &ss->as.next;
			ensure(atomic_read(&ss->refcnt) > 1, ss_get(ss));
			ss_put(ss);
			assure(ss->as.as->ss.numb > 0);
			ss->as.as->ss.numb--;
			as_put(xchg(&ss->as.as, NULL));
		}
		/* we are now a typeless link waiting for I_UNLINK */
		ss->id = 0;
		ss->type = 0;
	}
	return (QR_DONE);
}
STATIC int
ua_del_xp(ua_config_t * arg, struct xp *xp, int size, int force, int test)
{
	if (!xp)
		return (-EINVAL);
	if (!force) {
		/* bound to internal datastructures */
		if (xp->spp.list || xp->sp)
			return (-EBUSY);
	}
	if (!test) {
		noenable(xp->iq);
		noenable(xp->oq);
		/* unlink from spp */
		if (xp->spp.list) {
			fixme(("Check deactivation of all AS\n"));
			xp_put(xchg(&xp->spp.list->xp.xp, NULL));
			spp_put(xchg(&xp->spp.list, NULL));
		}
		/* unlink from sp */
		if (xp->sp) {
			sp_put(xchg(&xp->sp, NULL));
		}
		/* we are now a typeless link waiting for I_UNLINK */
		xp->id = 0;
		xp->type = 0;
	}
	return (QR_DONE);
}
STATIC int
ua_del_df(ua_config_t * arg, struct df *df, int size, int force, int test)
{
	if (!df)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
	}
	return (QR_DONE);
}

/*
 *  GET State
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_sta_as(ua_statem_t * arg, struct as *as, int size)
{
	ua_statem_as_t *sta = (typeof(sta)) (arg + 1);
	lmi_ulong *p;
	struct ap *ap;
	struct gp *gp;

	if (!as || (size -= sizeof(*sta)) < sizeof(*p))
		return (-EINVAL);
	arg->flags = as_get_flags(as);
	arg->state = as_get_state(as);
	sta->timers = as->timers;
	sta->as_numb = as->ap.numb;
	sta->spp_numb = as->gp.numb;
	p = (typeof(p)) (sta + 1);
	/* list out id/state pairs for associated AS */
	for (ap = as->ap.list; ap && size >= 3 * sizeof(*p);
	     ap = (as == ap->u.as) ? ap->p.next : ap->u.next, *p++ =
	     (as == ap->u.as) ? ap->p.as->id : ap->u.as->id, *p++ = ap_get_state(ap)) ;
	/* list out id/state pairs for associated SPP */
	for (gp = as->gp.list; gp && size >= 3 * sizeof(*p);
	     gp = gp->as.next, *p++ = gp->spp.spp->id, *p++ = gp_get_state(gp)) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
ua_sta_sp(ua_statem_t * arg, struct sp *sp, int size)
{
	ua_statem_sp_t *sta = (typeof(sta)) (arg + 1);
	lmi_ulong *p;
	struct np *np;

	if (!sp || (size -= sizeof(*sta)) < sizeof(*p))
		return (-EINVAL);
	arg->flags = sp_get_flags(sp);
	arg->state = sp_get_state(sp);
	sta->timers = sp->timers;
	sta->sp_numb = sp->np.numb;
	p = (typeof(p)) (sta + 1);
	/* list out id/state pairs for associated SP */
	for (np = sp->np.list; np && size >= 3 * sizeof(*p);
	     np = (sp == np->u.sp) ? np->p.next : np->u.next, *p++ =
	     (sp == np->u.sp) ? np->p.sp->id : np->u.sp->id, *p++ = np_get_state(np)) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
ua_sta_spp(ua_statem_t * arg, struct spp *spp, int size)
{
	ua_statem_spp_t *sta = (typeof(sta)) (arg + 1);
	lmi_ulong *p;
	struct gp *gp;

	if (!spp || (size -= sizeof(*sta)) < sizeof(*p))
		return (-EINVAL);
	arg->flags = spp_get_flags(spp);
	arg->state = spp_get_state(spp);
	sta->timers = spp->timers;
	sta->as_numb = spp->gp.numb;
	p = (typeof(p)) (sta + 1);
	/* list out id/state pairs for associated AS */
	for (gp = spp->gp.list; gp && size >= 3 * sizeof(*p);
	     gp = gp->spp.next, *p++ = gp->as.as->id, *p++ = gp_get_state(gp)) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
ua_sta_ss(ua_statem_t * arg, struct ss *ss, int size)
{
	ua_statem_ss_t *sta = (typeof(sta)) (arg + 1);

	if (!ss || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	arg->flags = ss_get_flags(ss);
	arg->state = ss_get_state(ss);
	sta->timers = ss->timers;
	return (QR_DONE);
}
STATIC int
ua_sta_xp(ua_statem_t * arg, struct xp *xp, int size)
{
	ua_statem_xp_t *sta = (typeof(sta)) (arg + 1);

	if (!xp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	arg->flags = xp_get_flags(xp);
	arg->state = xp_get_state(xp);
	sta->timers = xp->timers;
	return (QR_DONE);
}
STATIC int
ua_sta_df(ua_statem_t * arg, struct df *df, int size)
{
	ua_statem_df_t *sta = (typeof(sta)) (arg + 1);

	if (!df || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	arg->flags = 0;
	arg->state = 0;
	sta->timers = df->timers;
	return (QR_DONE);
}

/*
 *  GET Statistics Periods
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_statp_get_as(ua_stats_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_get_sp(ua_stats_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_get_spp(ua_stats_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_get_ss(ua_stats_t * arg, struct ss *ss, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_get_xp(ua_stats_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_get_df(ua_stats_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  SET Statistics Periods
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_statp_set_as(ua_stats_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_set_sp(ua_stats_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_set_spp(ua_stats_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_set_ss(ua_stats_t * arg, struct ss *ss, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_set_xp(ua_stats_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_statp_set_df(ua_stats_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  GET Statistics
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_stat_get_as(ua_stats_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_get_sp(ua_stats_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_get_spp(ua_stats_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_get_ss(ua_stats_t * arg, struct ss *ss, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_get_xp(ua_stats_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_get_df(ua_stats_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  CLR Statistics
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_stat_clr_as(ua_stats_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_clr_sp(ua_stats_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_clr_spp(ua_stats_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_clr_ss(ua_stats_t * arg, struct ss *ss, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_clr_xp(ua_stats_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_stat_clr_df(ua_stats_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  GET Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_not_get_as(ua_notify_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_get_sp(ua_notify_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_get_spp(ua_notify_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_get_ss(ua_notify_t * arg, struct ss *ss, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_get_xp(ua_notify_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_get_df(ua_notify_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  SET Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_not_set_as(ua_notify_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_set_sp(ua_notify_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_set_spp(ua_notify_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_set_ss(ua_notify_t * arg, struct ss *ss, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_set_xp(ua_notify_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_set_df(ua_notify_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  CLR Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_not_clr_as(ua_notify_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_clr_sp(ua_notify_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_clr_spp(ua_notify_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_clr_ss(ua_notify_t * arg, struct ss *ss, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_clr_xp(ua_notify_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
ua_not_clr_df(ua_notify_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  UP Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_up_as(ua_mgmt_t * arg, struct as *as)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_up_sp(ua_mgmt_t * arg, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_up_spp(ua_mgmt_t * arg, struct spp *spp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_up_ss(ua_mgmt_t * arg, struct ss *ss)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_up_xp(ua_mgmt_t * arg, struct xp *xp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_up_df(ua_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}

/*
 *  DOWN Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_down_as(ua_mgmt_t * arg, struct as *as)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_down_sp(ua_mgmt_t * arg, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_down_spp(ua_mgmt_t * arg, struct spp *spp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_down_ss(ua_mgmt_t * arg, struct ss *ss)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_down_xp(ua_mgmt_t * arg, struct xp *xp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_down_df(ua_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}

/*
 *  ACTIVATE Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_act_as(ua_mgmt_t * arg, struct as *as)
{
	if (!as || as->type != UA_OBJ_TYPE_AS_P)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_act_sp(ua_mgmt_t * arg, struct sp *sp)
{
	if (!sp || sp->type != UA_OBJ_TYPE_SG)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_act_spp(ua_mgmt_t * arg, struct spp *spp)
{
	if (!spp || spp->type != UA_OBJ_TYPE_SGP)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_act_ss(ua_mgmt_t * arg, struct ss *ss)
{
	if (!ss || ss->type != UA_OBJ_TYPE_SL_P)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_act_xp(ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp || !xp->spp.list || xp->spp.list->type != UA_OBJ_TYPE_SGP)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_act_df(ua_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}

/*
 *  DEACTIVATE Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_deact_as(ua_mgmt_t * arg, struct as *as)
{
	if (!as || as->type != UA_OBJ_TYPE_AS_P)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_deact_sp(ua_mgmt_t * arg, struct sp *sp)
{
	if (!sp || sp->type != UA_OBJ_TYPE_SG)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_deact_spp(ua_mgmt_t * arg, struct spp *spp)
{
	if (!spp || spp->type != UA_OBJ_TYPE_SGP)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_deact_ss(ua_mgmt_t * arg, struct ss *ss)
{
	if (!ss || ss->type != UA_OBJ_TYPE_SL_P)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_deact_xp(ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp || !xp->spp.list || xp->spp.list->type != UA_OBJ_TYPE_SGP)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
ua_deact_df(ua_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}

/*
 *  UP BLOCK Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_up_blo_as(ua_mgmt_t * arg, struct as *as)
{
	struct gp *gp;

	if (!as)
		return (-EINVAL);
	for (gp = as->gp.list; gp; gp = gp->as.next)
		spp_set_flags(gp->spp.spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_blo_sp(ua_mgmt_t * arg, struct sp *sp)
{
	struct spp *spp;

	if (!sp)
		return (-EINVAL);
	for (spp = sp->spp.list; spp; spp = spp->sp.next)
		spp_set_flags(spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_blo_spp(ua_mgmt_t * arg, struct spp *spp)
{
	if (!spp)
		return (-EINVAL);
	spp_set_flags(spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_blo_ss(ua_mgmt_t * arg, struct ss *ss)
{
	if (!ss)
		return (-EINVAL);
	ss_set_flags(ss, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_blo_xp(ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp || !xp->spp.list)
		return (-EINVAL);
	spp_set_flags(xp->spp.list, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_blo_df(ua_mgmt_t * arg, struct df *df)
{
	struct ss *ss;
	struct spp *spp;

	for (spp = master.spp.list; spp; spp = spp->next)
		spp_set_flags(spp, ASF_MGMT_BLOCKED);
	for (ss = (struct ss *) master.priv.list; ss; ss = ss->next)
		if (ss->type == UA_OBJ_TYPE_SL_U)
			ss_set_flags(ss, ASF_MGMT_BLOCKED);
	for (ss = (struct ss *) master.link.list; ss; ss = ss->next)
		if (ss->type == UA_OBJ_TYPE_SL_P)
			ss_set_flags(ss, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}

/*
 *  UP UNBLOCK Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_up_ubl_as(ua_mgmt_t * arg, struct as *as)
{
	struct gp *gp;

	if (!as)
		return (-EINVAL);
	for (gp = as->gp.list; gp; gp = gp->as.next)
		spp_clr_flags(gp->spp.spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_ubl_sp(ua_mgmt_t * arg, struct sp *sp)
{
	struct spp *spp;

	if (!sp)
		return (-EINVAL);
	for (spp = sp->spp.list; spp; spp = spp->sp.next)
		spp_clr_flags(spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_ubl_spp(ua_mgmt_t * arg, struct spp *spp)
{
	if (!spp)
		return (-EINVAL);
	spp_clr_flags(spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_ubl_ss(ua_mgmt_t * arg, struct ss *ss)
{
	if (!ss)
		return (-EINVAL);
	ss_clr_flags(ss, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_ubl_xp(ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp || !xp->spp.list)
		return (-EINVAL);
	spp_clr_flags(xp->spp.list, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_up_ubl_df(ua_mgmt_t * arg, struct df *df)
{
	struct ss *ss;
	struct spp *spp;

	for (spp = master.spp.list; spp; spp = spp->next)
		spp_clr_flags(spp, ASF_MGMT_BLOCKED);
	for (ss = (struct ss *) master.priv.list; ss; ss = ss->next)
		if (ss->type == UA_OBJ_TYPE_SL_U)
			ss_clr_flags(ss, ASF_MGMT_BLOCKED);
	for (ss = (struct ss *) master.link.list; ss; ss = ss->next)
		if (ss->type == UA_OBJ_TYPE_SL_P)
			ss_clr_flags(ss, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}

/*
 *  ACT BLOCK Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_act_blo_as(ua_mgmt_t * arg, struct as *as)
{
	if (!as)
		return (-EINVAL);
	as_set_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_act_blo_sp(ua_mgmt_t * arg, struct sp *sp)
{
	struct as *as;

	if (!sp)
		return (-EINVAL);
	for (as = sp->as.list; as; as = as->sp.next)
		as_set_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_act_blo_spp(ua_mgmt_t * arg, struct spp *spp)
{
	struct gp *gp;

	if (!spp)
		return (-EINVAL);
	for (gp = spp->gp.list; gp; gp = gp->spp.next)
		as_set_flags(gp->as.as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_act_blo_ss(ua_mgmt_t * arg, struct ss *ss)
{
	if (!ss)
		return (-EINVAL);
	return ua_act_blo_as(arg, ss->as.as);
}
STATIC int
ua_act_blo_xp(ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp)
		return (-EINVAL);
	return ua_act_blo_spp(arg, xp->spp.list);
}
STATIC int
ua_act_blo_df(ua_mgmt_t * arg, struct df *df)
{
	struct as *as;

	for (as = master.as.list; as; as = as->next)
		as_set_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}

/*
 *  ACT UNBLOCK Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
ua_act_ubl_as(ua_mgmt_t * arg, struct as *as)
{
	if (!as)
		return (-EINVAL);
	as_clr_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_act_ubl_sp(ua_mgmt_t * arg, struct sp *sp)
{
	struct as *as;

	if (!sp)
		return (-EINVAL);
	for (as = sp->as.list; as; as = as->sp.next)
		as_clr_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_act_ubl_spp(ua_mgmt_t * arg, struct spp *spp)
{
	struct gp *gp;

	if (!spp)
		return (-EINVAL);
	for (gp = spp->gp.list; gp; gp = gp->spp.next)
		as_clr_flags(gp->as.as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
ua_act_ubl_ss(ua_mgmt_t * arg, struct ss *ss)
{
	if (!ss)
		return (-EINVAL);
	return ua_act_ubl_as(arg, ss->as.as);
}
STATIC int
ua_act_ubl_xp(ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp)
		return (-EINVAL);
	return ua_act_ubl_spp(arg, xp->spp.list);
}
STATIC int
ua_act_ubl_df(ua_mgmt_t * arg, struct df *df)
{
	struct as *as;

	for (as = master.as.list; as; as = as->next)
		as_clr_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IOCTL Interface
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M2UA_IOCGOPTIONS
 *  -----------------------------------
 *  Get configuration options by object type and id.
 */
STATIC int
ua_iocgoptions(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_opt_get_as(arg, as_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_opt_get_sp(arg, sp_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_opt_get_spp(arg, spp_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_opt_get_ss(arg, ss_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_opt_get_xp(arg, xp_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_opt_get_df(arg, &master, size);
				break;
			}
	}
	return (ret);
}

/*
 *  M2UA_IOCsOPTIONS
 *  -----------------------------------
 */
STATIC int
ua_iocsoptions(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_opt_set_as(arg, as_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_opt_set_sp(arg, sp_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_opt_set_spp(arg, spp_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_opt_set_ss(arg, ss_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_opt_set_xp(arg, xp_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_opt_set_df(arg, &master, size);
				break;
			}
	}
	return (ret);
}

/*
 *  M2UA_IOCGCONFIG
 *  -----------------------------------
 */
STATIC int
ua_iocgconfig(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_get_as(arg, as_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_get_sp(arg, sp_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_get_spp(arg, spp_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_get_ss(arg, ss_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_get_xp(arg, xp_lookup(arg->id), size);
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_get_df(arg, &master, size);
				break;
			}
	}
	return (ret);
}

/*
 *  M2UA_IOCSCONFIG
 *  -----------------------------------
 */
STATIC int
ua_iocsconfig(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case M2UA_ADD:
				switch (arg->type) {
				case UA_OBJ_TYPE_AS_U:
				case UA_OBJ_TYPE_AS_P:
					ret = ua_add_as(arg, as_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_SP:
				case UA_OBJ_TYPE_SG:
					ret = ua_add_sp(arg, sp_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_ASP:
				case UA_OBJ_TYPE_SGP:
				case UA_OBJ_TYPE_SPP:
					ret = ua_add_spp(arg, spp_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_SL_U:
				case UA_OBJ_TYPE_SL_P:
					ret = ua_add_ss(arg, ss_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_XP_ASP:
				case UA_OBJ_TYPE_XP_SGP:
				case UA_OBJ_TYPE_XP_SPP:
				case UA_OBJ_TYPE_XP_XSP:
				case UA_OBJ_TYPE_XP_XGP:
				case UA_OBJ_TYPE_XP_XPP:
					ret = ua_add_xp(arg, xp_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_DF:
					ret = ua_add_df(arg, &master, size, 0, 0);
					break;
				}
				break;
			case M2UA_CHA:
				switch (arg->type) {
				case UA_OBJ_TYPE_AS_U:
				case UA_OBJ_TYPE_AS_P:
					ret = ua_cha_as(arg, as_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_SP:
				case UA_OBJ_TYPE_SG:
					ret = ua_cha_sp(arg, sp_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_ASP:
				case UA_OBJ_TYPE_SGP:
				case UA_OBJ_TYPE_SPP:
					ret = ua_cha_spp(arg, spp_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_SL_U:
				case UA_OBJ_TYPE_SL_P:
					ret = ua_cha_ss(arg, ss_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_XP_ASP:
				case UA_OBJ_TYPE_XP_SGP:
				case UA_OBJ_TYPE_XP_SPP:
				case UA_OBJ_TYPE_XP_XSP:
				case UA_OBJ_TYPE_XP_XGP:
				case UA_OBJ_TYPE_XP_XPP:
					ret = ua_cha_xp(arg, xp_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_DF:
					ret = ua_cha_df(arg, &master, size, 0, 0);
					break;
				}
				break;
			case M2UA_DEL:
				switch (arg->type) {
				case UA_OBJ_TYPE_AS_U:
				case UA_OBJ_TYPE_AS_P:
					ret = ua_del_as(arg, as_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_SP:
				case UA_OBJ_TYPE_SG:
					ret = ua_del_sp(arg, sp_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_ASP:
				case UA_OBJ_TYPE_SGP:
				case UA_OBJ_TYPE_SPP:
					ret = ua_del_spp(arg, spp_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_SL_U:
				case UA_OBJ_TYPE_SL_P:
					ret = ua_del_ss(arg, ss_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_XP_ASP:
				case UA_OBJ_TYPE_XP_SGP:
				case UA_OBJ_TYPE_XP_SPP:
				case UA_OBJ_TYPE_XP_XSP:
				case UA_OBJ_TYPE_XP_XGP:
				case UA_OBJ_TYPE_XP_XPP:
					ret = ua_del_xp(arg, xp_lookup(arg->id), size, 0, 0);
					break;
				case UA_OBJ_TYPE_DF:
					ret = ua_del_df(arg, &master, size, 0, 0);
					break;
				}
				break;
			}
	}
	return (ret);
}

/*
 *  M2UA_IOCTCONFIG
 *  -----------------------------------
 */
STATIC int
ua_ioctconfig(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case M2UA_ADD:
				switch (arg->type) {
				case UA_OBJ_TYPE_AS_U:
				case UA_OBJ_TYPE_AS_P:
					ret = ua_add_as(arg, as_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_SP:
				case UA_OBJ_TYPE_SG:
					ret = ua_add_sp(arg, sp_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_ASP:
				case UA_OBJ_TYPE_SGP:
				case UA_OBJ_TYPE_SPP:
					ret = ua_add_spp(arg, spp_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_SL_U:
				case UA_OBJ_TYPE_SL_P:
					ret = ua_add_ss(arg, ss_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_XP_ASP:
				case UA_OBJ_TYPE_XP_SGP:
				case UA_OBJ_TYPE_XP_SPP:
				case UA_OBJ_TYPE_XP_XSP:
				case UA_OBJ_TYPE_XP_XGP:
				case UA_OBJ_TYPE_XP_XPP:
					ret = ua_add_xp(arg, xp_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_DF:
					ret = ua_add_df(arg, &master, size, 0, 1);
					break;
				}
				break;
			case M2UA_CHA:
				switch (arg->type) {
				case UA_OBJ_TYPE_AS_U:
				case UA_OBJ_TYPE_AS_P:
					ret = ua_cha_as(arg, as_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_SP:
				case UA_OBJ_TYPE_SG:
					ret = ua_cha_sp(arg, sp_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_ASP:
				case UA_OBJ_TYPE_SGP:
				case UA_OBJ_TYPE_SPP:
					ret = ua_cha_spp(arg, spp_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_SL_U:
				case UA_OBJ_TYPE_SL_P:
					ret = ua_cha_ss(arg, ss_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_XP_ASP:
				case UA_OBJ_TYPE_XP_SGP:
				case UA_OBJ_TYPE_XP_SPP:
				case UA_OBJ_TYPE_XP_XSP:
				case UA_OBJ_TYPE_XP_XGP:
				case UA_OBJ_TYPE_XP_XPP:
					ret = ua_cha_xp(arg, xp_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_DF:
					ret = ua_cha_df(arg, &master, size, 0, 1);
					break;
				}
				break;
			case M2UA_DEL:
				switch (arg->type) {
				case UA_OBJ_TYPE_AS_U:
				case UA_OBJ_TYPE_AS_P:
					ret = ua_del_as(arg, as_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_SP:
				case UA_OBJ_TYPE_SG:
					ret = ua_del_sp(arg, sp_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_ASP:
				case UA_OBJ_TYPE_SGP:
				case UA_OBJ_TYPE_SPP:
					ret = ua_del_spp(arg, spp_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_SL_U:
				case UA_OBJ_TYPE_SL_P:
					ret = ua_del_ss(arg, ss_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_XP_ASP:
				case UA_OBJ_TYPE_XP_SGP:
				case UA_OBJ_TYPE_XP_SPP:
				case UA_OBJ_TYPE_XP_XSP:
				case UA_OBJ_TYPE_XP_XGP:
				case UA_OBJ_TYPE_XP_XPP:
					ret = ua_del_xp(arg, xp_lookup(arg->id), size, 0, 1);
					break;
				case UA_OBJ_TYPE_DF:
					ret = ua_del_df(arg, &master, size, 0, 1);
					break;
				}
				break;
			}
	}
	return (ret);
}

/*
 *  M2UA_IOCCCONFIG
 *  -----------------------------------
 */
STATIC int
ua_ioccconfig(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case M2UA_ADD:
				switch (arg->type) {
				case UA_OBJ_TYPE_AS_U:
				case UA_OBJ_TYPE_AS_P:
					ret = ua_add_as(arg, as_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_SP:
				case UA_OBJ_TYPE_SG:
					ret = ua_add_sp(arg, sp_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_ASP:
				case UA_OBJ_TYPE_SGP:
				case UA_OBJ_TYPE_SPP:
					ret = ua_add_spp(arg, spp_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_SL_U:
				case UA_OBJ_TYPE_SL_P:
					ret = ua_add_ss(arg, ss_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_XP_ASP:
				case UA_OBJ_TYPE_XP_SGP:
				case UA_OBJ_TYPE_XP_SPP:
				case UA_OBJ_TYPE_XP_XSP:
				case UA_OBJ_TYPE_XP_XGP:
				case UA_OBJ_TYPE_XP_XPP:
					ret = ua_add_xp(arg, xp_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_DF:
					ret = ua_add_df(arg, &master, size, 1, 0);
					break;
				}
				break;
			case M2UA_CHA:
				switch (arg->type) {
				case UA_OBJ_TYPE_AS_U:
				case UA_OBJ_TYPE_AS_P:
					ret = ua_cha_as(arg, as_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_SP:
				case UA_OBJ_TYPE_SG:
					ret = ua_cha_sp(arg, sp_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_ASP:
				case UA_OBJ_TYPE_SGP:
				case UA_OBJ_TYPE_SPP:
					ret = ua_cha_spp(arg, spp_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_SL_U:
				case UA_OBJ_TYPE_SL_P:
					ret = ua_cha_ss(arg, ss_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_XP_ASP:
				case UA_OBJ_TYPE_XP_SGP:
				case UA_OBJ_TYPE_XP_SPP:
				case UA_OBJ_TYPE_XP_XSP:
				case UA_OBJ_TYPE_XP_XGP:
				case UA_OBJ_TYPE_XP_XPP:
					ret = ua_cha_xp(arg, xp_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_DF:
					ret = ua_cha_df(arg, &master, size, 1, 0);
					break;
				}
				break;
			case M2UA_DEL:
				switch (arg->type) {
				case UA_OBJ_TYPE_AS_U:
				case UA_OBJ_TYPE_AS_P:
					ret = ua_del_as(arg, as_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_SP:
				case UA_OBJ_TYPE_SG:
					ret = ua_del_sp(arg, sp_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_ASP:
				case UA_OBJ_TYPE_SGP:
				case UA_OBJ_TYPE_SPP:
					ret = ua_del_spp(arg, spp_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_SL_U:
				case UA_OBJ_TYPE_SL_P:
					ret = ua_del_ss(arg, ss_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_XP_ASP:
				case UA_OBJ_TYPE_XP_SGP:
				case UA_OBJ_TYPE_XP_SPP:
				case UA_OBJ_TYPE_XP_XSP:
				case UA_OBJ_TYPE_XP_XGP:
				case UA_OBJ_TYPE_XP_XPP:
					ret = ua_del_xp(arg, xp_lookup(arg->id), size, 1, 0);
					break;
				case UA_OBJ_TYPE_DF:
					ret = ua_del_df(arg, &master, size, 1, 0);
					break;
				}
				break;
			}
	}
	return (ret);
}

/*
 *  M2UA_IOCGSTATEM
 *  -----------------------------------
 *  Get state variables by object type.
 */
STATIC int
ua_iocgstatem(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_ua(&master.lock);
		switch (arg->type) {
		case UA_OBJ_TYPE_AS_U:
		case UA_OBJ_TYPE_AS_P:
			ret = ua_sta_as(arg, as_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SP:
		case UA_OBJ_TYPE_SG:
			ret = ua_sta_sp(arg, sp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_ASP:
		case UA_OBJ_TYPE_SGP:
		case UA_OBJ_TYPE_SPP:
			ret = ua_sta_spp(arg, spp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SL_U:
		case UA_OBJ_TYPE_SL_P:
			ret = ua_sta_ss(arg, ss_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_XP_ASP:
		case UA_OBJ_TYPE_XP_SGP:
		case UA_OBJ_TYPE_XP_SPP:
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			ret = ua_sta_xp(arg, xp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_DF:
			ret = ua_sta_df(arg, &master, size);
			break;
		}
		spin_unlock_ua(&master.lock);
	}
	return (ret);
}

/*
 *  M2UA_IOCCMRESET
 *  -----------------------------------
 *  Perform master reset.
 */
STATIC int
ua_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		ua_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		(void) arg;
		return (-EOPNOTSUPP);
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCGSTATSP
 *  -----------------------------------
 *  Get statistics periods.
 */
STATIC int
ua_iocgstatsp(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_ua(&master.lock);
		switch (arg->type) {
		case UA_OBJ_TYPE_AS_U:
		case UA_OBJ_TYPE_AS_P:
			ret = ua_statp_get_as(arg, as_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SP:
		case UA_OBJ_TYPE_SG:
			ret = ua_statp_get_sp(arg, sp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_ASP:
		case UA_OBJ_TYPE_SGP:
		case UA_OBJ_TYPE_SPP:
			ret = ua_statp_get_spp(arg, spp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SL_U:
		case UA_OBJ_TYPE_SL_P:
			ret = ua_statp_get_ss(arg, ss_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_XP_ASP:
		case UA_OBJ_TYPE_XP_SGP:
		case UA_OBJ_TYPE_XP_SPP:
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			ret = ua_statp_get_xp(arg, xp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_DF:
			ret = ua_statp_get_df(arg, &master, size);
			break;
		}
		spin_unlock_ua(&master.lock);
	}
	return (ret);
}

/*
 *  M2UA_IOCSSTATSP
 *  -----------------------------------
 *  Set statistics periods.
 */
STATIC int
ua_iocsstatsp(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_ua(&master.lock);
		switch (arg->type) {
		case UA_OBJ_TYPE_AS_U:
		case UA_OBJ_TYPE_AS_P:
			ret = ua_statp_set_as(arg, as_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SP:
		case UA_OBJ_TYPE_SG:
			ret = ua_statp_set_sp(arg, sp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_ASP:
		case UA_OBJ_TYPE_SGP:
		case UA_OBJ_TYPE_SPP:
			ret = ua_statp_set_spp(arg, spp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SL_U:
		case UA_OBJ_TYPE_SL_P:
			ret = ua_statp_set_ss(arg, ss_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_XP_ASP:
		case UA_OBJ_TYPE_XP_SGP:
		case UA_OBJ_TYPE_XP_SPP:
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			ret = ua_statp_set_xp(arg, xp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_DF:
			ret = ua_statp_set_df(arg, &master, size);
			break;
		}
		spin_unlock_ua(&master.lock);
	}
	return (ret);
}

/*
 *  M2UA_IOCGSTATS
 *  -----------------------------------
 */
STATIC int
ua_iocgstats(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_ua(&master.lock);
		switch (arg->type) {
		case UA_OBJ_TYPE_AS_U:
		case UA_OBJ_TYPE_AS_P:
			ret = ua_stat_get_as(arg, as_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SP:
		case UA_OBJ_TYPE_SG:
			ret = ua_stat_get_sp(arg, sp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_ASP:
		case UA_OBJ_TYPE_SGP:
		case UA_OBJ_TYPE_SPP:
			ret = ua_stat_get_spp(arg, spp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SL_U:
		case UA_OBJ_TYPE_SL_P:
			ret = ua_stat_get_ss(arg, ss_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_XP_ASP:
		case UA_OBJ_TYPE_XP_SGP:
		case UA_OBJ_TYPE_XP_SPP:
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			ret = ua_stat_get_xp(arg, xp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_DF:
			ret = ua_stat_get_df(arg, &master, size);
			break;
		}
		spin_unlock_ua(&master.lock);
	}
	return (ret);
}

/*
 *  M2UA_IOCCSTATS
 *  -----------------------------------
 */
STATIC int
ua_ioccstats(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_ua(&master.lock);
		switch (arg->type) {
		case UA_OBJ_TYPE_AS_U:
		case UA_OBJ_TYPE_AS_P:
			ret = ua_stat_clr_as(arg, as_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SP:
		case UA_OBJ_TYPE_SG:
			ret = ua_stat_clr_sp(arg, sp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_ASP:
		case UA_OBJ_TYPE_SGP:
		case UA_OBJ_TYPE_SPP:
			ret = ua_stat_clr_spp(arg, spp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SL_U:
		case UA_OBJ_TYPE_SL_P:
			ret = ua_stat_clr_ss(arg, ss_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_XP_ASP:
		case UA_OBJ_TYPE_XP_SGP:
		case UA_OBJ_TYPE_XP_SPP:
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			ret = ua_stat_clr_xp(arg, xp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_DF:
			ret = ua_stat_clr_df(arg, &master, size);
			break;
		}
		spin_unlock_ua(&master.lock);
	}
	return (ret);
}

/*
 *  M2UA_IOCGNOTIFY
 *  -----------------------------------
 */
STATIC int
ua_iocgnotify(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_ua(&master.lock);
		switch (arg->type) {
		case UA_OBJ_TYPE_AS_U:
		case UA_OBJ_TYPE_AS_P:
			ret = ua_not_get_as(arg, as_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SP:
		case UA_OBJ_TYPE_SG:
			ret = ua_not_get_sp(arg, sp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_ASP:
		case UA_OBJ_TYPE_SGP:
		case UA_OBJ_TYPE_SPP:
			ret = ua_not_get_spp(arg, spp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SL_U:
		case UA_OBJ_TYPE_SL_P:
			ret = ua_not_get_ss(arg, ss_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_XP_ASP:
		case UA_OBJ_TYPE_XP_SGP:
		case UA_OBJ_TYPE_XP_SPP:
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			ret = ua_not_get_xp(arg, xp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_DF:
			ret = ua_not_get_df(arg, &master, size);
			break;
		}
		spin_unlock_ua(&master.lock);
	}
	return (ret);
}

/*
 *  M2UA_IOCSNOTIFY
 *  -----------------------------------
 */
STATIC int
ua_iocsnotify(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_ua(&master.lock);
		switch (arg->type) {
		case UA_OBJ_TYPE_AS_U:
		case UA_OBJ_TYPE_AS_P:
			ret = ua_not_set_as(arg, as_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SP:
		case UA_OBJ_TYPE_SG:
			ret = ua_not_set_sp(arg, sp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_ASP:
		case UA_OBJ_TYPE_SGP:
		case UA_OBJ_TYPE_SPP:
			ret = ua_not_set_spp(arg, spp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SL_U:
		case UA_OBJ_TYPE_SL_P:
			ret = ua_not_set_ss(arg, ss_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_XP_ASP:
		case UA_OBJ_TYPE_XP_SGP:
		case UA_OBJ_TYPE_XP_SPP:
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			ret = ua_not_set_xp(arg, xp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_DF:
			ret = ua_not_set_df(arg, &master, size);
			break;
		}
		spin_unlock_ua(&master.lock);
	}
	return (ret);
}

/*
 *  M2UA_IOCCNOTIFY
 *  -----------------------------------
 */
STATIC int
ua_ioccnotify(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		int size = msgdsize(mp);
		ua_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_ua(&master.lock);
		switch (arg->type) {
		case UA_OBJ_TYPE_AS_U:
		case UA_OBJ_TYPE_AS_P:
			ret = ua_not_clr_as(arg, as_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SP:
		case UA_OBJ_TYPE_SG:
			ret = ua_not_clr_sp(arg, sp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_ASP:
		case UA_OBJ_TYPE_SGP:
		case UA_OBJ_TYPE_SPP:
			ret = ua_not_clr_spp(arg, spp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_SL_U:
		case UA_OBJ_TYPE_SL_P:
			ret = ua_not_clr_ss(arg, ss_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_XP_ASP:
		case UA_OBJ_TYPE_XP_SGP:
		case UA_OBJ_TYPE_XP_SPP:
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			ret = ua_not_clr_xp(arg, xp_lookup(arg->id), size);
			break;
		case UA_OBJ_TYPE_DF:
			ret = ua_not_clr_df(arg, &master, size);
			break;
		}
		spin_unlock_ua(&master.lock);
	}
	return (ret);
}

/*
 *  M2UA_IOCCMGMT
 *  -----------------------------------
 */
STATIC int
ua_ioccmgmt(queue_t *q, mblk_t *mp)
{
	int ret = -EINVAL;

	if (mp->b_cont) {
		ua_mgmt_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		switch (arg->cmd) {
		case M2UA_MGMT_UP:
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_up_as(arg, as_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_up_sp(arg, sp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_up_spp(arg, spp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_up_ss(arg, ss_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_up_xp(arg, xp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_up_df(arg, &master);
				break;
			}
			break;
		case M2UA_MGMT_ACTIVATE:
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_act_as(arg, as_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_act_sp(arg, sp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_act_spp(arg, spp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_act_ss(arg, ss_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_act_xp(arg, xp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_act_df(arg, &master);
				break;
			}
			break;
		case M2UA_MGMT_DEACTIVATE:
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_deact_as(arg, as_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_deact_sp(arg, sp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_deact_spp(arg, spp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_deact_ss(arg, ss_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_deact_xp(arg, xp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_deact_df(arg, &master);
				break;
			}
			break;
		case M2UA_MGMT_DOWN:
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_down_as(arg, as_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_down_sp(arg, sp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_down_spp(arg, spp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_down_ss(arg, ss_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_down_xp(arg, xp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_down_df(arg, &master);
				break;
			}
			break;
		case M2UA_MGMT_UP_BLOCK:
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_up_blo_as(arg, as_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_up_blo_sp(arg, sp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_up_blo_spp(arg, spp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_up_blo_ss(arg, ss_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_up_blo_xp(arg, xp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_up_blo_df(arg, &master);
				break;
			}
			break;
		case M2UA_MGMT_UP_UNBLOCK:
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_up_ubl_as(arg, as_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_up_ubl_sp(arg, sp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_up_ubl_spp(arg, spp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_up_ubl_ss(arg, ss_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_up_ubl_xp(arg, xp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_up_ubl_df(arg, &master);
				break;
			}
			break;
		case M2UA_MGMT_ACT_BLOCK:
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_act_blo_as(arg, as_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_act_blo_sp(arg, sp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_act_blo_spp(arg, spp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_act_blo_ss(arg, ss_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_act_blo_xp(arg, xp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_act_blo_df(arg, &master);
				break;
			}
			break;
		case M2UA_MGMT_ACT_UNBLOCK:
			switch (arg->type) {
			case UA_OBJ_TYPE_AS_U:
			case UA_OBJ_TYPE_AS_P:
				ret = ua_act_ubl_as(arg, as_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SP:
			case UA_OBJ_TYPE_SG:
				ret = ua_act_ubl_sp(arg, sp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_ASP:
			case UA_OBJ_TYPE_SGP:
			case UA_OBJ_TYPE_SPP:
				ret = ua_act_ubl_spp(arg, spp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_SL_U:
			case UA_OBJ_TYPE_SL_P:
				ret = ua_act_ubl_ss(arg, ss_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_XP_ASP:
			case UA_OBJ_TYPE_XP_SGP:
			case UA_OBJ_TYPE_XP_SPP:
			case UA_OBJ_TYPE_XP_XSP:
			case UA_OBJ_TYPE_XP_XGP:
			case UA_OBJ_TYPE_XP_XPP:
				ret = ua_act_ubl_xp(arg, xp_lookup(arg->id));
				break;
			case UA_OBJ_TYPE_DF:
				ret = ua_act_ubl_df(arg, &master);
				break;
			}
			break;
		}
	}
	return (ret);
}

/*
 *  M2UA_IOCCPASS
 *  -----------------------------------
 */
STATIC int
ua_ioccpass(queue_t *q, mblk_t *mp)
{
	mblk_t *bp, *dp;
	union link *lk;
	ua_pass_t *arg;

	if (unlikely(mp->b_cont == NULL))
		return (-EINVAL);

	arg = (typeof(arg)) mp->b_cont->b_rptr;

	if (unlikely(!(lk = link_lookup(arg->muxid)) || !lk->str.oq))
		return (-EINVAL);

	if (unlikely((bp = ss7_dupb(q, mp)) == NULL))
		return (-ENOBUFS);

	if (unlikely((dp = ss7_dupb(q, mp)) == NULL)) {
		freeb(bp);
		return (-ENOBUFS);
	}

	if (unlikely(arg->type < QPCTL && !bcanputnext(lk->str.oq, mp->b_band))) {
		freeb(bp);
		freeb(dp);
		return (-EBUSY);
	}

	bp->b_datap->db_type = arg->type;
	bp->b_band = arg->band;
	bp->b_cont = dp;
	bp->b_rptr += sizeof(*arg);
	bp->b_wptr = bp->b_rptr + arg->ctl_length;
	dp->b_datap->db_type = M_DATA;
	dp->b_rptr += sizeof(*arg) + arg->ctl_length;
	dp->b_wptr = dp->b_rptr + arg->dat_length;
	putnext(lk->str.oq, bp);
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
ua_w_ioctl(queue_t *q, mblk_t *mp)
{
	str_t *s = STR_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;

	(void) s;
	switch (type) {
	case _IOC_TYPE(__SID):
	{
		union link *lk, **lkp;
		struct linkblk *lb;

		if (!(lb = arg)) {
			swerr();
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			ptrace(("%s: %p: I_PLINK\n", DRV_NAME, s));
			if (s != (str_t *) master.lm) {
				ptrace(("%s: %p: ERROR: non-management attempt to I_PLINK\n",
					DRV_NAME, s));
				ret = -EOPNOTSUPP;
				break;
			}
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR; Non-root attempt to I_PLINK\n",
					DRV_NAME, s));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			ptrace(("%s: %p: I_LINK\n", DRV_NAME, s));
			spin_lock_ua(&master.lock);
			/* place in list in ascending index order */
			for (lkp = &master.link.list; *lkp && (*lkp)->str.u.mux.index < lb->l_index;
			     lkp = (union link **) &(*lkp)->str.next) ;
			if ((lk = ua_alloc_link(lb->l_qbot, lkp, lb->l_index, iocp->ioc_cr))) {
				spin_unlock_ua(&master.lock);
				break;
			}
			ret = -ENOMEM;
			spin_unlock_ua(&master.lock);
			break;
		case _IOC_NR(I_PUNLINK):
			ptrace(("%s: %p: I_PUNLINK\n", DRV_NAME, s));
			if (s != (str_t *) master.lm) {
				ptrace(("%s: %p: ERROR: non-management attempt to I_PUNLINK\n",
					DRV_NAME, s));
				ret = -EOPNOTSUPP;
				break;
			}
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n",
					DRV_NAME, s));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			ptrace(("%s: %p: I_UNLINK\n", DRV_NAME, s));
			spin_lock_ua(&master.lock);
			if ((lk = link_lookup(lb->l_index))) {
				ua_free_link(lk->str.iq);
				spin_unlock_ua(&master.lock);
				break;
			}
			ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n", DRV_NAME, s));
			ret = -EINVAL;
			spin_unlock_ua(&master.lock);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %c, %d\n", DRV_NAME,
				s, (char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SL_IOC_MAGIC:
	{
		if (count < size) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SL_IOCGOPTIONS):
			ret = sl_iocgoptions(q, mp);
			break;
		case _IOC_NR(SL_IOCSOPTIONS):
			ret = sl_iocsoptions(q, mp);
			break;
		case _IOC_NR(SL_IOCGCONFIG):
		case _IOC_NR(SL_IOCSCONFIG):
		case _IOC_NR(SL_IOCTCONFIG):
		case _IOC_NR(SL_IOCCCONFIG):
		case _IOC_NR(SL_IOCGSTATEM):
		case _IOC_NR(SL_IOCCMRESET):
		case _IOC_NR(SL_IOCGSTATSP):
		case _IOC_NR(SL_IOCSSTATSP):
		case _IOC_NR(SL_IOCGSTATS):
		case _IOC_NR(SL_IOCCSTATS):
		case _IOC_NR(SL_IOCGNOTIFY):
		case _IOC_NR(SL_IOCSNOTIFY):
		case _IOC_NR(SL_IOCCNOTIFY):
			ret = sl_iocpass(q, mp);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported SL ioctl %c, %d\n", DRV_NAME, s,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case M2UA_IOC_MAGIC:
	{
		/* Note: only the layer management stream is permitted to perform M2UA IOCTLs. This 
		   is because the SL User stream should be unaware that these IOCTLs exist so that
		   an M2UA SL-U stream is completely transparent to an normal SL stream. */
		if (s != (str_t *) master.lm) {
			ptrace(("%s: %p: ERROR: non-management attempt at M2UA IOCTL\n",
				DRV_NAME, s));
			ret = -EOPNOTSUPP;
			break;
		}
		if (count < size) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(M2UA_IOCGOPTIONS):
			printd(("%s: %p: -> M2UA_IOCGOPTIONS\n", DRV_NAME, s));
			ret = ua_iocgoptions(q, mp);
			break;
		case _IOC_NR(M2UA_IOCSOPTIONS):
			printd(("%s: %p: -> M2UA_IOCSOPTIONS\n", DRV_NAME, s));
			ret = ua_iocsoptions(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGCONFIG):
			printd(("%s: %p: -> M2UA_IOCGCONFIG\n", DRV_NAME, s));
			ret = ua_iocgconfig(q, mp);
			break;
		case _IOC_NR(M2UA_IOCSCONFIG):
			printd(("%s: %p: -> M2UA_IOCSCONFIG\n", DRV_NAME, s));
			ret = ua_iocsconfig(q, mp);
			break;
		case _IOC_NR(M2UA_IOCTCONFIG):
			printd(("%s: %p: -> M2UA_IOCTCONFIG\n", DRV_NAME, s));
			ret = ua_ioctconfig(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCCONFIG):
			printd(("%s: %p: -> M2UA_IOCCCONFIG\n", DRV_NAME, s));
			ret = ua_ioccconfig(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGSTATEM):
			printd(("%s: %p: -> M2UA_IOCGSTATEM\n", DRV_NAME, s));
			ret = ua_iocgstatem(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCMRESET):
			printd(("%s: %p: -> M2UA_IOCCMRESET\n", DRV_NAME, s));
			ret = ua_ioccmreset(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGSTATSP):
			printd(("%s: %p: -> M2UA_IOCGSTATSP\n", DRV_NAME, s));
			ret = ua_iocgstatsp(q, mp);
			break;
		case _IOC_NR(M2UA_IOCSSTATSP):
			printd(("%s: %p: -> M2UA_IOCSSTATSP\n", DRV_NAME, s));
			ret = ua_iocsstatsp(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGSTATS):
			printd(("%s: %p: -> M2UA_IOCGSTATS\n", DRV_NAME, s));
			ret = ua_iocgstats(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCSTATS):
			printd(("%s: %p: -> M2UA_IOCCSTATS\n", DRV_NAME, s));
			ret = ua_ioccstats(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGNOTIFY):
			printd(("%s: %p: -> M2UA_IOCGNOTIFY\n", DRV_NAME, s));
			ret = ua_iocgnotify(q, mp);
			break;
		case _IOC_NR(M2UA_IOCSNOTIFY):
			printd(("%s: %p: -> M2UA_IOCSNOTIFY\n", DRV_NAME, s));
			ret = ua_iocsnotify(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCNOTIFY):
			printd(("%s: %p: -> M2UA_IOCCNOTIFY\n", DRV_NAME, s));
			ret = ua_ioccnotify(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCMGMT):
			printd(("%s: %p: -> M2UA_IOCCMGMT\n", DRV_NAME, s));
			ret = ua_ioccmgmt(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCPASS):
			printd(("%s: %p: -> M2UA_IOCCPASS\n", DRV_NAME, s));
			ret = ua_ioccpass(q, mp);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unspported M2UA ioctl %c, %d\n", DRV_NAME, s,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ptrace(("%s: %p: ERROR: Unsupported ioctl %c, %d\n", DRV_NAME, s, (char) type, nr));
		ret = -EOPNOTSUPP;
		break;
	}
	if (ret > 0) {
		return (ret);
	} else if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCACK Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
slp_r_iocack(queue_t *q, mblk_t *mp)
{
	str_t *s = STR_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;

	(void) s;
	(void) nr;
	(void) size;
	(void) count;
	(void) arg;
	switch (type) {
	case SL_IOC_MAGIC:
		switch (nr) {
		case _IOC_NR(SL_IOCGOPTIONS):
			ret = sl_ackgoptions(q, mp);
			break;
		case _IOC_NR(SL_IOCSOPTIONS):
			ret = sl_acksoptions(q, mp);
			break;
		case _IOC_NR(SL_IOCGCONFIG):
		case _IOC_NR(SL_IOCSCONFIG):
		case _IOC_NR(SL_IOCTCONFIG):
		case _IOC_NR(SL_IOCCCONFIG):
		case _IOC_NR(SL_IOCGSTATEM):
		case _IOC_NR(SL_IOCCMRESET):
		case _IOC_NR(SL_IOCGSTATSP):
		case _IOC_NR(SL_IOCSSTATSP):
		case _IOC_NR(SL_IOCGSTATS):
		case _IOC_NR(SL_IOCCSTATS):
		case _IOC_NR(SL_IOCGNOTIFY):
		case _IOC_NR(SL_IOCSNOTIFY):
		case _IOC_NR(SL_IOCCNOTIFY):
			ret = sl_ackpass(q, mp);
			break;
		}
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCNAK Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
slp_r_iocnak(queue_t *q, mblk_t *mp)
{
	struct ss *slp = SS_PRIV(q), *slu;

	if ((slp->ioc)) {
		ss_put((slu = xchg(&slp->ioc, NULL)));
		if (slu->oq) {
			putnext(slu->oq, mp);
			return (QR_ABSORBED);
		}
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Primitives from SL-U to M2UA.
 *  -----------------------------------
 */
STATIC int
slu_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct ss *ss = SS_PRIV(q);

	ss->i_oldstate = ss->i_state;

	switch (*(sl_ulong *) mp->b_rptr) {
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", DRV_NAME, ss));
		rtn = slu_info_req(ss, q, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", DRV_NAME, ss));
		rtn = slu_attach_req(ss, q, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", DRV_NAME, ss));
		rtn = slu_detach_req(ss, q, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", DRV_NAME, ss));
		rtn = slu_enable_req(ss, q, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", DRV_NAME, ss));
		rtn = slu_disable_req(ss, q, mp);
		break;
#if 0
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", DRV_NAME, ss));
		rtn = slu_optmgmt_req(ss, q, mp);
		break;
#endif
	case SL_PDU_REQ:
		printd(("%s: %p: -> SL_PDU_REQ\n", DRV_NAME, ss));
		rtn = slu_pdu_req(ss, q, mp);
		break;
	case SL_EMERGENCY_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_REQ\n", DRV_NAME, ss));
		rtn = slu_emergency_req(ss, q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_CEASES_REQ\n", DRV_NAME, ss));
		rtn = slu_emergency_ceases_req(ss, q, mp);
		break;
	case SL_START_REQ:
		printd(("%s: %p: -> SL_START_REQ\n", DRV_NAME, ss));
		rtn = slu_start_req(ss, q, mp);
		break;
	case SL_STOP_REQ:
		printd(("%s: %p: -> SL_STOP_REQ\n", DRV_NAME, ss));
		rtn = slu_stop_req(ss, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		printd(("%s: %p: -> SL_RETRIEVE_BSNT_REQ\n", DRV_NAME, ss));
		rtn = slu_retrieve_bsnt_req(ss, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		printd(("%s: %p: -> SL_RETRIEVAL_REQUEST_AND_FSNC_REQ\n", DRV_NAME, ss));
		rtn = slu_retrieval_request_and_fsnc_req(ss, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		printd(("%s: %p: -> SL_CLEAR_BUFFERS_REQ\n", DRV_NAME, ss));
		rtn = slu_clear_buffers_req(ss, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		printd(("%s: %p: -> SL_CLEAR_RTB_REQ\n", DRV_NAME, ss));
		rtn = slu_clear_rtb_req(ss, q, mp);
		break;
	case SL_CONTINUE_REQ:
		printd(("%s: %p: -> SL_CONTINUE_REQ\n", DRV_NAME, ss));
		rtn = slu_continue_req(ss, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		printd(("%s: %p: -> SL_LOCAL_PROCESSOR_OUTAGE_REQ\n", DRV_NAME, ss));
		rtn = slu_local_processor_outage_req(ss, q, mp);
		break;
	case SL_RESUME_REQ:
		printd(("%s: %p: -> SL_RESUME_REQ\n", DRV_NAME, ss));
		rtn = slu_resume_req(ss, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		printd(("%s: %p: -> SL_CONGESTION_DISCARD_REQ\n", DRV_NAME, ss));
		rtn = slu_congestion_discard_req(ss, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		printd(("%s: %p: -> SL_CONGESTION_ACCEPT_REQ\n", DRV_NAME, ss));
		rtn = slu_congestion_accept_req(ss, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		printd(("%s: %p: -> SL_NO_CONGESTION_REQ\n", DRV_NAME, ss));
		rtn = slu_no_congestion_req(ss, q, mp);
		break;
	case SL_POWER_ON_REQ:
		printd(("%s: %p: -> SL_POWER_ON_REQ\n", DRV_NAME, ss));
		rtn = slu_power_on_req(ss, q, mp);
		break;
#if 0
	case SL_OPTMGMT_REQ:
		printd(("%s: %p: -> SL_OPTMGMT_REQ\n", DRV_NAME, ss));
		rtn = slu_optmgmt_req(ss, q, mp);
		break;
#endif
#if 0
	case SL_NOTIFY_REQ:
		printd(("%s: %p: -> SL_NOTIFY_REQ\n", DRV_NAME, ss));
		rtn = slu_notify_req(ss, q, mp);
		break;
#endif
	default:
		printd(("%s: %p: -> SL_????\n", DRV_NAME, ss));
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		ss->i_state = ss->i_oldstate;
	return (rtn);
}

/*
 *  Primitives from SL-P to M2UA.
 *  -----------------------------------
 */
STATIC int
slp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct ss *ss = SS_PRIV(q);

	ss->i_oldstate = ss->i_state;

	switch (*(sl_ulong *) mp->b_rptr) {
	case LMI_INFO_ACK:
		printd(("%s: %p: -> LMI_INFO_ACK\n", DRV_NAME, ss));
		rtn = slp_info_ack(ss, q, mp);
		break;
	case LMI_OK_ACK:
		printd(("%s: %p: -> LMI_OK_ACK\n", DRV_NAME, ss));
		rtn = slp_ok_ack(ss, q, mp);
		break;
	case LMI_ERROR_ACK:
		printd(("%s: %p: -> LMI_ERROR_ACK\n", DRV_NAME, ss));
		rtn = slp_error_ack(ss, q, mp);
		break;
	case LMI_ENABLE_CON:
		printd(("%s: %p: -> LMI_ENABLE_CON\n", DRV_NAME, ss));
		rtn = slp_enable_con(ss, q, mp);
		break;
	case LMI_DISABLE_CON:
		printd(("%s: %p: -> LMI_DISABLE_CON\n", DRV_NAME, ss));
		rtn = slp_disable_con(ss, q, mp);
		break;
#if 0
	case LMI_OPTMGMT_ACK:
		printd(("%s: %p: -> LMI_OPTNGNT_ACK\n", DRV_NAME, ss));
		rtn = slp_optmgmt_ack(ss, q, mp);
		break;
#endif
	case LMI_ERROR_IND:
		printd(("%s: %p: -> LMI_ERROR_IND\n", DRV_NAME, ss));
		rtn = slp_error_ind(ss, q, mp);
		break;
	case LMI_STATS_IND:
		printd(("%s: %p: -> LMI_STATS_IND\n", DRV_NAME, ss));
		rtn = slp_stats_ind(ss, q, mp);
		break;
	case LMI_EVENT_IND:
		printd(("%s: %p: -> LMI_EVENT_IND\n", DRV_NAME, ss));
		rtn = slp_event_ind(ss, q, mp);
		break;
	case SL_PDU_IND:
		printd(("%s: %p: -> SL_PDU_IND\n", DRV_NAME, ss));
		rtn = slp_pdu_ind(ss, q, mp);
		break;
	case SL_LINK_CONGESTED_IND:
		printd(("%s: %p: -> SL_LINK_CONGESTED_IND\n", DRV_NAME, ss));
		rtn = slp_link_congested_ind(ss, q, mp);
		break;
	case SL_LINK_CONGESTION_CEASED_IND:
		printd(("%s: %p: -> SL_LINK_CONGESTION_CEASED_IND\n", DRV_NAME, ss));
		rtn = slp_congestion_ceased_ind(ss, q, mp);
		break;
	case SL_RETRIEVED_MESSAGE_IND:
		printd(("%s: %p: -> SL_RETRIEVED_MESSAGE_IND\n", DRV_NAME, ss));
		rtn = slp_retrieved_message_ind(ss, q, mp);
		break;
	case SL_RETRIEVAL_COMPLETE_IND:
		printd(("%s: %p: -> SL_RETRIEVAL_COMPLETE_IND\n", DRV_NAME, ss));
		rtn = slp_retrieval_complete_ind(ss, q, mp);
		break;
	case SL_RB_CLEARED_IND:
		printd(("%s: %p: -> SL_RB_CLEARED_IND\n", DRV_NAME, ss));
		rtn = slp_rb_cleared_ind(ss, q, mp);
		break;
	case SL_BSNT_IND:
		printd(("%s: %p: -> SL_BSNT_IND\n", DRV_NAME, ss));
		rtn = slp_bsnt_ind(ss, q, mp);
		break;
	case SL_IN_SERVICE_IND:
		printd(("%s: %p: -> SL_IN_SERVICE_IND\n", DRV_NAME, ss));
		rtn = slp_in_service_ind(ss, q, mp);
		break;
	case SL_OUT_OF_SERVICE_IND:
		printd(("%s: %p: -> SL_OUT_OF_SERVICE_IND\n", DRV_NAME, ss));
		rtn = slp_out_of_service_ind(ss, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		printd(("%s: %p: -> SL_REMOTE_PROCESSOR_OUTAGE_IND\n", DRV_NAME, ss));
		rtn = slp_remote_processor_outage_ind(ss, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		printd(("%s: %p: -> SL_REMOTE_PROCESSOR_RECOVERED_IND\n", DRV_NAME, ss));
		rtn = slp_remote_processor_recovered_ind(ss, q, mp);
		break;
	case SL_RTB_CLEARED_IND:
		printd(("%s: %p: -> SL_RTB_CLEARED_IND\n", DRV_NAME, ss));
		rtn = slp_rtb_cleared_ind(ss, q, mp);
		break;
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		printd(("%s: %p: -> SL_RETRIEVAL_NOT_POSSIBLE_IND\n", DRV_NAME, ss));
		rtn = slp_retrieval_not_possible_ind(ss, q, mp);
		break;
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		printd(("%s: %p: -> SL_BSNT_NOT_RETRIEVABLE_IND\n", DRV_NAME, ss));
		rtn = slp_bsnt_not_retrievable_ind(ss, q, mp);
		break;
#if 0
	case SL_OPTMGMT_ACK:
		printd(("%s: %p: -> SL_OPTMGMT_ACK\n", DRV_NAME, ss));
		rtn = slp_optmgmt_ack(ss, q, mp);
		break;
#endif
#if 0
	case SL_NOTIFY_IND:
		printd(("%s: %p: -> SL_NOTIFY_IND\n", DRV_NAME, ss));
		rtn = slp_notify_ind(ss, q, mp);
		break;
#endif
	default:
		printd(("%s: %p: -> SL_????\n", DRV_NAME, ss));
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		ss->i_state = ss->i_oldstate;
	return (rtn);
}

/*
 *  Primitives from MTP-P to M3UA.
 *  -----------------------------------
 */
STATIC int
mtpp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct ss *ss = SS_PRIV(q);

	ss->i_oldstate = ss->i_state;

	switch (*(mtp_ulong *) mp->b_rptr) {
	case MTP_OK_ACK:
		printd(("%s: %p: MTP_OK_ACK <-\n", DRV_NAME, ss));
		rtn = mtpp_ok_ack(ss, q, mp);
		break;
	case MTP_ERROR_ACK:
		printd(("%s: %p: MTP_ERROR_ACK <-\n", DRV_NAME, ss));
		rtn = mtpp_error_ack(ss, q, mp);
		break;
	case MTP_BIND_ACK:
		printd(("%s: %p: MTP_BIND_ACK <-\n", DRV_NAME, ss));
		rtn = mtpp_bind_ack(ss, q, mp);
		break;
	case MTP_ADDR_ACK:
		printd(("%s: %p: MTP_ADDR_ACK <-\n", DRV_NAME, ss));
		rtn = mtpp_addr_ack(ss, q, mp);
		break;
	case MTP_INFO_ACK:
		printd(("%s: %p: MTP_INFO_ACK <-\n", DRV_NAME, ss));
		rtn = mtpp_info_ack(ss, q, mp);
		break;
	case MTP_OPTMGMT_ACK:
		printd(("%s: %p: MTP_OPTMGMT_ACK <-\n", DRV_NAME, ss));
		rtn = mtpp_optmgmt_ack(ss, q, mp);
		break;
	case MTP_TRANSFER_IND:
		printd(("%s: %p: MTP_TRANSFER_IND <-\n", DRV_NAME, ss));
		rtn = mtpp_trasnfer_ind(ss, q, mp);
		break;
	case MTP_PAUSE_IND:
		printd(("%s: %p: MTP_PAUSE_IND <-\n", DRV_NAME, ss));
		rtn = mtpp_pause_ind(ss, q, mp);
		break;
	case MTP_RESUME_IND:
		printd(("%s: %p: MTP_RESUME_IND <-\n", DRV_NAME, ss));
		rtn = mtpp_resume_ind(ss, q, mp);
		break;
	case MTP_STATUS_IND:
		printd(("%s: %p: MTP_STATUS_IND <-\n", DRV_NAME, ss));
		rtn = mtpp_status_ind(ss, q, mp);
		break;
	case MTP_RESTART_BEGINS_IND:
		printd(("%s: %p: MTP_RESTART_BEGINS_IND <-\n", DRV_NAME, ss));
		rtn = mtpp_restart_begins_ind(ss, q, mp);
		break;
	case MTP_RESTART_COMPLETE_IND:
		printd(("%s: %p: MTP_RESTART_COMPLETE_IND <-\n", DRV_NAME, ss));
		rtn = mtpp_restart_complete_ind(ss, q, mp);
		break;
	default:
		printd(("%s: %p: MTP_???? <-\n", DRV_NAME, xp));
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		ss->i_state = ss->i_oldstate;
	return (rtn);
}

/*
 *  Primitives from DL-P to IUA.
 *  -----------------------------------
 */
STATIC int
dlp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct ss *ss = SS_PRIV(q);

	ss->i_oldstate = ss->i_state;

	switch (*(dl_ulong *) mp->b_rptr) {
	case DL_INFO_ACK:
		printd(("%s: %p: DL_INFO_ACK <-\n", DRV_NAME, ss));
		rtn = dlp_info_ack(ss, q, mp);
		break;
	case DL_BIND_ACK:
		printd(("%s: %p: DL_BIND_ACK <-\n", DRV_NAME, ss));
		rtn = dlp_bind_ack(ss, q, mp);
		break;
	case DL_OK_ACK:
		printd(("%s: %p: DL_OK_ACK <-\n", DRV_NAME, ss));
		rtn = dlp_ok_ack(ss, q, mp);
		break;
	case DL_ERROR_ACK:
		printd(("%s: %p: DL_ERROR_ACK <-\n", DRV_NAME, ss));
		rtn = dlp_error_ack(ss, q, mp);
		break;
	case DL_SUBS_BIND_ACK:
		printd(("%s: %p: DL_SUBS_BIND_ACK <-\n", DRV_NAME, ss));
		rtn = dlp_subs_bind_ack(ss, q, mp);
		break;
	case DL_UNITDATA_IND:
		printd(("%s: %p: DL_UNITDATA_IND <-\n", DRV_NAME, ss));
		rtn = dlp_unitdata_ind(ss, q, mp);
		break;
	case DL_UDERROR_IND:
		printd(("%s: %p: DL_UDERROR_IND <-\n", DRV_NAME, ss));
		rtn = dlp_uderror_ind(ss, q, mp);
		break;
	case DL_CONNECT_IND:
		printd(("%s: %p: DL_CONNECT_IND <-\n", DRV_NAME, ss));
		rtn = dlp_connect_ind(ss, q, mp);
		break;
	case DL_CONNECT_CON:
		printd(("%s: %p: DL_CONNECT_CON <-\n", DRV_NAME, ss));
		rtn = dlp_connect_con(ss, q, mp);
		break;
	case DL_TOKEN_ACK:
		printd(("%s: %p: DL_TOKEN_ACK <-\n", DRV_NAME, ss));
		rtn = dlp_token_ack(ss, q, mp);
		break;
	case DL_DISCONNECT_IND:
		printd(("%s: %p: DL_DISCONNECT_IND <-\n", DRV_NAME, ss));
		rtn = dlp_disconnect_ind(ss, q, mp);
		break;
	case DL_RESET_IND:
		printd(("%s: %p: DL_RESET_IND <-\n", DRV_NAME, ss));
		rtn = dlp_reset_ind(ss, q, mp);
		break;
	case DL_RESET_CON:
		printd(("%s: %p: DL_RESET_CON <-\n", DRV_NAME, ss));
		rtn = dlp_reset_con(ss, q, mp);
		break;
	case DL_DATA_ACK_IND:
		printd(("%s: %p: DL_DATA_ACK_IND <-\n", DRV_NAME, ss));
		rtn = dlp_data_ack_ind(ss, q, mp);
		break;
	case DL_DATA_ACK_STATUS_IND:
		printd(("%s: %p: DL_DATA_ACK_STATUS_IND <-\n", DRV_NAME, ss));
		rtn = dlp_data_ack_status_ind(ss, q, mp);
		break;
	case DL_REPLY_IND:
		printd(("%s: %p: DL_REPLY_IND <-\n", DRV_NAME, ss));
		rtn = dlp_reply_ind(ss, q, mp);
		break;
	case DL_REPLY_STATUS_IND:
		printd(("%s: %p: DL_REPLY_STATUS_IND <-\n", DRV_NAME, ss));
		rtn = dlp_reply_status_ind(ss, q, mp);
		break;
	case DL_REPLY_UPDATE_STATUS_IND:
		printd(("%s: %p: DL_REPLY_UPDATE_STATUS_IND <-\n", DRV_NAME, ss));
		rtn = dlp_reply_update_status_ind(ss, q, mp);
		break;
	case DL_XID_IND:
		printd(("%s: %p: DL_XID_IND <-\n", DRV_NAME, ss));
		rtn = dlp_xid_ind(ss, q, mp);
		break;
	case DL_XID_CON:
		printd(("%s: %p: DL_XID_CON <-\n", DRV_NAME, ss));
		rtn = dlp_xid_con(ss, q, mp);
		break;
	case DL_TEST_IND:
		printd(("%s: %p: DL_TEST_IND <-\n", DRV_NAME, ss));
		rtn = dlp_test_ind(ss, q, mp);
		break;
	case DL_TEST_CON:
		printd(("%s: %p: DL_TEST_CON <-\n", DRV_NAME, ss));
		rtn = dlp_test_con(ss, q, mp);
		break;
	case DL_PHYS_ADDR_ACK:
		printd(("%s: %p: DL_PHYS_ADDR_ACK <-\n", DRV_NAME, ss));
		rtn = dlp_phys_addr_ack(ss, q, mp);
		break;
	case DL_GET_STATISTICS_ACK:
		printd(("%s: %p: DL_GET_STATISTICS_ACK <-\n", DRV_NAME, ss));
		rtn = dlp_get_statistics_ack(ss, q, mp);
		break;
	default:
	}
	if (rtn < 0)
		ss->i_state = ss->i_oldstate;
	return (rtn);
}

/*
 *  Primitives from XP to UA.
 *  -----------------------------------
 */
STATIC int
xp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct xp *xp = XP_PRIV(q);

	xp->i_oldstate = xp->i_state;

	switch (*(t_uscalar_t *) mp->b_rptr) {
	case T_CONN_IND:
		printd(("%s: %p: -> T_CONN_IND\n", DRV_NAME, xp));
		rtn = xp_conn_ind(xp, q, mp);
		break;
	case T_CONN_CON:
		printd(("%s: %p: -> T_CONN_CON\n", DRV_NAME, xp));
		rtn = xp_conn_con(xp, q, mp);
		break;
	case T_DISCON_IND:
		printd(("%s: %p: -> T_DISCON_IND\n", DRV_NAME, xp));
		rtn = xp_discon_ind(xp, q, mp);
		break;
	case T_DATA_IND:
		printd(("%s: %p: -> T_DATA_IND\n", DRV_NAME, xp));
		rtn = xp_data_ind(xp, q, mp);
		break;
	case T_EXDATA_IND:
		printd(("%s: %p: -> T_EXDATA_IND\n", DRV_NAME, xp));
		rtn = xp_exdata_ind(xp, q, mp);
		break;
	case T_INFO_ACK:
		printd(("%s: %p: -> T_INFO_ACK\n", DRV_NAME, xp));
		rtn = xp_info_ack(xp, q, mp);
		break;
	case T_BIND_ACK:
		printd(("%s: %p: -> T_BIND_ACK\n", DRV_NAME, xp));
		rtn = xp_bind_ack(xp, q, mp);
		break;
	case T_ERROR_ACK:
		printd(("%s: %p: -> T_ERROR_ACK\n", DRV_NAME, xp));
		rtn = xp_error_ack(xp, q, mp);
		break;
	case T_OK_ACK:
		printd(("%s: %p: -> T_OK_ACK\n", DRV_NAME, xp));
		rtn = xp_ok_ack(xp, q, mp);
		break;
	case T_UNITDATA_IND:
		printd(("%s: %p: -> T_UNITDATA_IND\n", DRV_NAME, xp));
		rtn = xp_unitdata_ind(xp, q, mp);
		break;
	case T_UDERROR_IND:
		printd(("%s: %p: -> T_UDERROR_IND\n", DRV_NAME, xp));
		rtn = xp_uderror_ind(xp, q, mp);
		break;
	case T_OPTMGMT_ACK:
		printd(("%s: %p: -> T_OPTMGMT_ACK\n", DRV_NAME, xp));
		rtn = xp_optmgmt_ack(xp, q, mp);
		break;
	case T_ORDREL_IND:
		printd(("%s: %p: -> T_ORDREL_IND\n", DRV_NAME, xp));
		rtn = xp_ordrel_ind(xp, q, mp);
		break;
	case T_OPTDATA_IND:
		printd(("%s: %p: -> T_OPTDATA_IND\n", DRV_NAME, xp));
		rtn = xp_optdata_ind(xp, q, mp);
		break;
#ifdef T_ADDR_ACK
	case T_ADDR_ACK:
		printd(("%s: %p: -> T_ADDR_ACK\n", DRV_NAME, xp));
		rtn = xp_addr_ack(xp, q, mp);
		break;
#endif
#ifdef T_CAPABILITY_ACK
	case T_CAPABILITY_ACK:
		printd(("%s: %p: -> T_CAPABILITY_ACK\n", DRV_NAME, xp));
		rtn = xp_capability_ack(xp, q, mp);
		break;
#endif
	default:
		printd(("%s: %p: -> T_????\n", DRV_NAME, xp));
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		xp->i_state = xp->i_oldstate;
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
slu_w_data(queue_t *q, mblk_t *mp)
{
	struct ss *ss = SS_PRIV(q);

	/* data from above */
	return slu_write(ss, q, mp);
}
STATIC INLINE int
slp_r_data(queue_t *q, mblk_t *mp)
{
	struct ss *ss = SS_PRIV(q);

	/* data from below */
	return slp_read(ss, q, mp);
}
STATIC INLINE int
xp_r_data(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);

	/* data from below */
	return xp_read(xp, q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR, M_HANDUP Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
slp_r_error(queue_t *q, mblk_t *mp)
{
	return slp_hangup(q, mp);
}
STATIC INLINE int
slp_r_hangup(queue_t *q, mblk_t *mp)
{
	return slp_hangup(q, mp);
}
STATIC INLINE int
xp_r_error(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);

	return xp_hangup(xp, q, mp);
}
STATIC INLINE int
xp_r_hangup(queue_t *q, mblk_t *mp)
{
	struct xp *xp = XP_PRIV(q);

	return xp_hangup(xp, q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
slu_w_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return slu_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return slu_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return slu_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return ua_w_ioctl(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC INLINE int
slp_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return slp_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return slp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return slp_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return slp_r_error(q, mp);
	case M_HANGUP:
		return slp_r_hangup(q, mp);
	case M_IOCACK:
		return slp_r_iocack(q, mp);
	case M_IOCNAK:
		return slp_r_iocnak(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC INLINE int
xp_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return xp_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return xp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return xp_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return xp_r_error(q, mp);
	case M_HANGUP:
		return xp_r_hangup(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}

/**
 * ua_oput: - upper and lower multiplex output put procedure
 * @q: active queue (upper read queue, lower write queue)
 * @mp: message to put
 *
 * The upper read queue and lower write queue are queues that can only have
 * their put procedures called from within the multiplex.  These put procedures,
 * although defined must never be called.  Instead, always check for outbound
 * flow control with bcanputnext() and call putnext().  If bcanputnext() fails,
 * place messages back on the originating queue and set the ASF_WANTW flag in
 * the private structure.  When the outbound queue becomes backenabled,
 * ua_osrv() will run and explicitly qenable() all queues feeding the
 * backenabled queue that have the ASF_WANTW flag set in their private
 * structures.
 */
STATIC streamscall int
ua_oput(queue_t *q, mblk_t *mp)
{
	/* should never be called */
	never();
	freemsg(mp);
	return (0);
}

/**
 * slu_o_wakeup: - wake up (backenable) across multiplexing driver
 * @q: active queue (upper read queue)
 * 
 * Only called when backenabled from upstream: need to explicitly qenable() each
 * feeding Stream that wanted to write across the multiplex but found itself
 * blocked (ASF_WANTW flag set).  The feeding Streams are either SL-P Streams or
 * SGP Streams, both on the lower multiplex.
 */
STATIC streamscall void
slu_o_wakeup(queue_t *q)
{
	struct *ssu = SS_PRIV(q);
	struct as *as;

	if ((as = ssu->as.as) != NULL) {
		struct ap *ap;

		for (ap = as->ap.list; ap; ap = ap->u.next) {
			struct ss *ssp;
			struct gp *sgp;

			for (ssp = ap->p.as->ss.list; ssp; ssp = ssp->as.next)
				if (ss_tst_flags(ssp, ASF_WANTW)) {
					ss_clr_flags(ssp, ASF_WANTW);
					qenable(ssp->iq);
				}
			for (sgp = ap->p.as->gp.list; sgp; sgp = sgp->as.next) {
				struct xp *xp;

				if ((xp = sgp->spp.spp->xp.xp)) {
					if (xp_tst_flags(xp, ASF_WANTW)) {
						xp_clr_flags(xp, ASF_WANTW);
						qenable(xp->iq);
					}
				}
			}
		}
	} else
		swerr();
	return;
}

/**
 * slp_o_wakeup: - wake up (backenable) across multiplexing driver
 * @q: active queue (lower write queue)
 *
 * Only called when backenabled from downstream: need to explicitly qenable()
 * each feeding Stream that wanted to write across the multiplex but found
 * itself blocked (ASF_WANTW flag set).  The feeding Streams are either SL-U
 * Streams on the upper multiplex, or ASP Streams on the lower multiplex.
 */
STATIC streamscall void
slp_o_wakeup(queue_t *q)
{
	struct ss *ssp = SS_PRIV(q);
	struct as *as;

	if ((as = ssp->as.as) != NULL) {
		struct ap *ap;

		for (ap = as->ap.list; ap; ap = ap->p.next) {
			struct ss *ssu;
			struct gp *asp;

			for (ssu = ap->u.as->ss.list; ssu; ssu = ssu->as.next)
				if (ss_tst_flags(ssu, ASF_WANTW)) {
					ss_clr_flags(ssu, ASF_WANTW);
					qenable(ssu->iq);
				}
			for (asp = ap->u.as->gp.list; asp; asp = asp->as.next) {
				struct xp *xp;

				if ((xp = asp->spp.spp->xp.xp)) {
					if (xp_tst_flags(xp, ASF_WANTW)) {
						xp_clr_flags(xp, ASF_WANTW);
						qenable(xp->iq);
					}
				}
			}
		}
	} else
		swerr();
	return;
}

/**
 * asp_o_wakeup: - wake up (backenable) across multiplexing driver
 * @asp: asp to backenable
 *
 * Only called when backenabled from downstream: need to explicitly qenable()
 * each feeding Stream that wanted to write across the multiplex but found
 * itself blocked (ASF_WANTW flag set).  The feeding Streams are either SL-P
 * Streams or SGP Streams, both on the lower multiplex.
 */
STATIC void
asp_o_wakeup(struct spp *asp)
{
	struct gp *gp;

	for (gp = asp->gp.list; gp; gp = gp->spp.next) {
		struct as *as;

		if ((as = gp->as.as)) {
			struct ap *ap;

			for (ap = as->ap.list; ap; ap = ap->u.next) {
				struct ss *ssp;
				struct gp *sgp;

				for (ssp = ap->p.as->ss.list; ssp; ssp = ssp->as.next)
					if (ss_tst_flags(ssp, ASF_WANTW)) {
						ss_clr_flags(ssp, ASF_WANTW);
						qenable(ssp->iq);
					}
				for (sgp = ap->p.as->gp.list; sgp; sgp = sgp->as.next) {
					struct xp *xp;

					if ((xp = sgp->spp.spp->xp.xp))
						if (xp_tst_flags(xp, ASF_WANTW)) {
							xp_clr_flags(xp, ASF_WANTW);
							qenable(xp->iq);
						}
				}
			}
		}
	}
}

/**
 * sgp_o_wakeup: - wake up (backenable) across multiplexing driver
 * @sgp: sgp to backenable
 *
 * Only called when backenabled from downstream: need to explicitly qenable()
 * each feeding Strem that wanted to write across the multiplex but found itself
 * blocked (ASF_WANTW flag set).  The feeding Streams are either SL-U Streams on
 * the upper multiplex, or ASP Streams on the lower multiplex.
 */
STATIC void
sgp_o_wakeup(struct spp *sgp)
{
	struct gp *gp;

	for (gp = sgp->gp.list; gp; gp = gp->spp.next) {
		struct as *as;

		if ((as = gp->as.as)) {
			struct ap *ap;

			for (ap = as->ap.list; ap; ap = ap->p.next) {
				struct ss *ssu;
				struct gp *asp;

				for (ssu = ap->u.as->ss.list; ssu; ssu = ssu->as.next)
					if (ss_tst_flags(ssu, ASF_WANTW)) {
						ss_clr_flags(ssu, ASF_WANTW);
						qenable(ssu->iq);
					}
				for (asp = ap->u.as->gp.list; asp; asp = asp->as.next) {
					struct xp *xp;

					if ((xp = asp->spp.spp->xp.xp))
						if (xp_tst_flags(xp, ASF_WANTW)) {
							xp_clr_flags(xp, ASF_WANTW);
							qenable(xp->iq);
						}
				}
			}
		}
	}
}

/**
 * sp_o_wakeup: - wake up (backenable) across multiplexing driver
 * @sp: sp to backenable
 *
 * Only called when backenabled from downstream: need to explicitly qenable()
 * each feeding Stream that wanted to write across the multiplex but found
 * itslef blocked (ASF_WANTW flag set).  The feeding Streams are either SL-P
 * Streams or SGP Streams, both on the lower mutliplex.
 */
STATIC void
sp_o_wakeup(struct sp *sp)
{
	struct as *as;

	for (as = sp->as.list; as; as = as->sp.next) {
		struct ap *ap;

		for (ap = as->as.list; ap; ap = ap->u.next) {
			struct ss *ssp;
			struct gp *sgp;

			for (ssp = ap->p.as->ss.list; ssp; ssp = ssp->as.next)
				if (ss_tst_flags(ssp, ASF_WANTW)) {
					ss_clr_flags(ssp, ASF_WANTW);
					qenable(ssp->iq);
				}
			for (sgp = ap->p.as->gp.list; sgp; sgp = sgp->as.next) {
				struct xp *xp;

				if ((xp = sgp->spp.spp->xp.xp))
					if (xp_tst_flags(xp, ASF_WANTW)) {
						xp_clr_flags(xp, ASF_WANTW);
						qenable(xp->iq);
					}
			}
		}
	}
}

/**
 * xp_o_wakeup: - wake up (backenable) across multiplexing driver
 * @q: active queue (lower write queue)
 *
 * Only called when backenabled from downstream: need to explicitly qenable()
 * each feeding upper or lower multiplex.
 */
STATIC streamscall void
xp_o_wakeup(queue_t *q)
{
	struct xp *xp = XP_PRIV(q);
	struct spp *spp;

	if ((spp = xp->spp.list)) {
		switch (spp->type) {
		case UA_OBJ_TYPE_ASP:
			asp_o_wakeup(spp);
			break;
		case UA_OBJ_TYPE_SGP:
			sgp_o_wakeup(spp);
			break;
		case UA_OBJ_TYPE_SPP:
		default:
			swerr();
		}
	} else if ((sp = xp->sp)) {
		switch (sp->type) {
		case UA_OBJ_TYPE_SP:
			sp_o_wakeup(sp);
			break;
		case UA_OBJ_TYPE_SG:
		default:
			swerr();
		}
	} else
		swerr();
	return;
}
STATIC streamscall int
ua_osrv(queue_t *q)
{
	str_t *s = STR_PRIV(q);

	if (s->o_wakeup)
		s->o_wakeup(q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC major_t ua_majors[M2UA_CMAJORS] = { M2UA_CMAJOR_0, };

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 *  There are several specialized Streams defined on the upper multiplex: a
 *  Stream for linking signalling link provider Streams for use by SGs, a Stream
 *  for linking SCTP TPI Streams representing ASPs connecting to an SGs, a
 *  Stream for linking SCTP TPI Streams representing SGs connected to an ASP, a
 *  Stream for listening for and responding to unsatisfied Attach requests.  In
 *  the current implementation, these are all represented by one management
 *  Stream.
 */
STATIC streamscall int
ua_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	minor_t bminor = cminor;
	union priv *p, **pp = &master.priv.list;

	if (q->q_ptr != NULL)
		return (0);	/* already open */

	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		return (EIO);
	}
	if (cmajor != M2UA_CMAJOR_0 || cminor > 1)
		return (ENXIO);

	if (cminor != 1)
		/* allocate a new device */
		cminor = 2;

	spin_lock_ua(&master.lock);
	for (; *pp; pp = (union priv **) &(*pp)->str.next) {
		major_t dmajor = (*pp)->str.u.dev.cmajor;

		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*pp)->str.u.dev.cminor;

			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= M2UA_CMAJORS
					    || !(cmajor = ua_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= M2UA_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_ua(&master.lock);
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(p = ua_alloc_priv(q, pp, devp, crp, bminor))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_ua(&master.lock);
		return (ENOMEM);
	}
	spin_unlock_ua(&master.lock);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
ua_close(queue_t *q, int flag, cred_t *crp)
{
	str_t *s = STR_PRIV(q);

	(void) s;
	printd(("%s: %p: closing character device %d:%d\n", DRV_NAME, s, s->u.dev.cmajor,
		s->u.dev.cminor));
	spin_lock_ua(&master.lock);
	{
		ua_free_priv(q);
	}
	spin_unlock_ua(&master.lock);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *ua_priv_cachep = NULL;
STATIC kmem_cache_t *ua_link_cachep = NULL;
STATIC kmem_cache_t *ua_as_cachep = NULL;
STATIC kmem_cache_t *ua_ap_cachep = NULL;
STATIC kmem_cache_t *ua_spp_cachep = NULL;
STATIC kmem_cache_t *ua_sp_cachep = NULL;
STATIC kmem_cache_t *ua_np_cachep = NULL;
STATIC kmem_cache_t *ua_gp_cachep = NULL;

STATIC int
ua_term_caches(void)
{
	int err = 0;

	if (ua_priv_cachep) {
		if (kmem_cache_destroy(ua_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ua_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ua_priv_cachep\n", DRV_NAME));
	}
	if (ua_link_cachep) {
		if (kmem_cache_destroy(ua_link_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ua_link_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ua_link_cachep\n", DRV_NAME));
	}
	if (ua_as_cachep) {
		if (kmem_cache_destroy(ua_as_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ua_as_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ua_as_cachep\n", DRV_NAME));
	}
	if (ua_ap_cachep) {
		if (kmem_cache_destroy(ua_ap_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ua_ap_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ua_ap_cachep\n", DRV_NAME));
	}
	if (ua_gp_cachep) {
		if (kmem_cache_destroy(ua_gp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ua_gp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ua_gp_cachep\n", DRV_NAME));
	}
	if (ua_sp_cachep) {
		if (kmem_cache_destroy(ua_sp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ua_sp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ua_sp_cachep\n", DRV_NAME));
	}
	if (ua_np_cachep) {
		if (kmem_cache_destroy(ua_np_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ua_np_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ua_np_cachep\n", DRV_NAME));
	}
	if (ua_spp_cachep) {
		if (kmem_cache_destroy(ua_spp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ua_spp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ua_spp_cachep\n", DRV_NAME));
	}
	return (err);
}
static int
ua_init_caches(void)
{
	if (!ua_priv_cachep
	    && !(ua_priv_cachep = kmem_cache_create("ua_priv_cachep", sizeof(union priv), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ua_priv_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized ua link structure cache\n", DRV_NAME));
	if (!ua_link_cachep
	    && !(ua_link_cachep = kmem_cache_create("ua_link_cachep", sizeof(union link), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ua_link_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized ua link structure cache\n", DRV_NAME));
	if (!ua_as_cachep
	    && !(ua_as_cachep = kmem_cache_create("ua_as_cachep", sizeof(struct as), 0,
						  SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ua_as_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized ua as structure cache\n", DRV_NAME));
	if (!ua_ap_cachep
	    && !(ua_ap_cachep = kmem_cache_create("ua_ap_cachep", sizeof(struct ap), 0,
						  SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ua_ap_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized ua ap structure cache\n", DRV_NAME));
	if (!ua_gp_cachep
	    && !(ua_gp_cachep = kmem_cache_create("ua_gp_cachep", sizeof(struct gp), 0,
						  SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ua_gp_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized ua gp structure cache\n", DRV_NAME));
	if (!ua_sp_cachep
	    && !(ua_sp_cachep = kmem_cache_create("ua_sp_cachep", sizeof(struct sp), 0,
						  SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ua_sp_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized ua sp structure cache\n", DRV_NAME));
	if (!ua_np_cachep
	    && !(ua_np_cachep = kmem_cache_create("ua_np_cachep", sizeof(struct np), 0,
						  SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ua_np_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized ua np structure cache\n", DRV_NAME));
	if (!ua_spp_cachep
	    && !(ua_spp_cachep = kmem_cache_create("ua_spp_cachep", sizeof(struct spp), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ua_spp_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized ua spp structure cache\n", DRV_NAME));
	return (0);
      enomem:
	ua_term_caches();
	return (-ENOMEM);
}

/*
 *  AP - AS-U/AS-P Graph Node
 *  -----------------------------------
 *  AS-U/AS-P graph node allocation, deallocation.
 */
STATIC struct ap *
ua_alloc_ap(struct as *as_u, struct as *as_p)
{
	struct ap *ap;

	printd(("%s: %s: ap graph as %ld:%ld\n", DRV_NAME, __FUNCTION__, as_u->id, as_p->id));
	if ((ap = kmem_cache_alloc(ua_ap_cachep, SLAB_ATOMIC))) {
		bzero(ap, sizeof(*ap));
		ap_set_state(ap, 0);
		/* link to AS-U */
		if ((ap->u.next = as_u->ap.list))
			ap->u.next->u.prev = &ap->u.next;
		ap->u.prev = &as_u->ap.list;
		ap->u.as = as_get(as_u);
		as_u->ap.list = ap;
		as_u->ap.numb++;
		/* link to AS-P */
		if ((ap->p.next = as_p->ap.list))
			ap->p.next->p.prev = &ap->p.next;
		ap->p.prev = &as_p->ap.list;
		ap->p.as = as_get(as_p);
		as_p->ap.list = ap;
		as_p->ap.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocated ap structure for as %lu:%lu\n",
			DRV_NAME, __FUNCTION__, as_u->id, as_p->id));
	return (ap);
}
STATIC void
ua_free_ap(struct ap *ap)
{
	if (ap) {
		/* unlink AS-U */
		if ((*ap->u.prev = ap->u.next))
			ap->u.next->u.prev = ap->u.prev;
		ap->u.next = NULL;
		ap->u.prev = &ap->u.next;
		ap->u.as->ap.numb--;
		as_put(xchg(&ap->u.as, NULL));
		/* unlink AS-P */
		if ((*ap->p.prev = ap->p.next))
			ap->p.next->p.prev = ap->p.prev;
		ap->p.next = NULL;
		ap->p.prev = &ap->p.next;
		ap->p.as->ap.numb--;
		as_put(xchg(&ap->p.as, NULL));
		kmem_cache_free(ua_ap_cachep, ap);
		printd(("%s: %s: %p: deallocated ap structure\n", DRV_NAME, __FUNCTION__, ap));
		return;
	}
	swerr();
}

/*
 *  NP - SP/SG Graph Node
 *  -----------------------------------
 *  SP/SG graph node allocation, deallocation.
 */
STATIC struct np *
ua_alloc_np(struct sp *sp, struct sp *sg)
{
	struct np *np;

	printd(("%s: %s: np graph sp %ld:%ld\n", DRV_NAME, __FUNCTION__, sp->id, sg->id));
	if ((np = kmem_cache_alloc(ua_np_cachep, SLAB_ATOMIC))) {
		bzero(np, sizeof(*np));
		np_set_state(np, 0);
		/* link to AS-U */
		if ((np->u.next = sp->np.list))
			np->u.next->u.prev = &np->u.next;
		np->u.prev = &sp->np.list;
		np->u.sp = sp_get(sp);
		sp->np.list = np;
		sp->np.numb++;
		/* link to AS-P */
		if ((np->p.next = sg->np.list))
			np->p.next->p.prev = &np->p.next;
		np->p.prev = &sg->np.list;
		np->p.sp = sp_get(sg);
		sg->np.list = np;
		sg->np.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocated np structure for %lu:%lu\n",
			DRV_NAME, __FUNCTION__, sp->id, sg->id));
	return (np);
}
STATIC void
ua_free_np(struct np *np)
{
	if (np) {
		/* unlink AS-U */
		if ((*np->u.prev = np->u.next))
			np->u.next->u.prev = np->u.prev;
		np->u.next = NULL;
		np->u.prev = &np->u.next;
		np->u.sp->np.numb--;
		sp_put(xchg(&np->u.sp, NULL));
		/* unlink AS-P */
		if ((*np->p.prev = np->p.next))
			np->p.next->p.prev = np->p.prev;
		np->p.next = NULL;
		np->p.prev = &np->p.next;
		np->p.sp->np.numb--;
		sp_put(xchg(&np->p.sp, NULL));
		kmem_cache_free(ua_np_cachep, np);
		printd(("%s: %s: %p: deallocated np structure\n", DRV_NAME, __FUNCTION__, np));
		return;
	}
	swerr();
}

/*
 *  GP - AS/SPP Graph Node
 *  -----------------------------------
 *  AS/SPP graph node allocation, deallocation.
 */
STATIC struct gp *
ua_alloc_gp(struct as *as, struct spp *spp)
{
	struct gp *gp;

	printd(("%s: %s: gp graph as %ld spp %lu\n", DRV_NAME, __FUNCTION__, as->id, spp->id));
	if ((gp = kmem_cache_alloc(ua_gp_cachep, SLAB_ATOMIC))) {
		bzero(gp, sizeof(gp));
		gp_set_state(gp, 0);
		/* link to AS */
		if ((gp->as.next = as->gp.list))
			gp->as.next->as.prev = &gp->as.next;
		gp->as.prev = &as->gp.list;
		gp->as.as = as_get(as);
		as->gp.list = gp;
		as->gp.numb++;
		/* link to SPP */
		if ((gp->spp.next = spp->gp.list))
			gp->spp.next->spp.prev = &gp->spp.next;
		gp->spp.prev = &spp->gp.list;
		gp->spp.spp = spp_get(spp);
		spp->gp.list = gp;
		spp->gp.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate gp structure for as %ld spp %ld\n",
			DRV_NAME, __FUNCTION__, as->id, spp->id));
	return (gp);
}
STATIC void
ua_free_gp(struct gp *gp)
{
	if (gp) {
		struct rp *rp;

		/* remove RP graph */
		while ((rp = gp->rp.list))
			ua_free_rp(rp);
		/* unlink from AS */
		if ((*gp->as.prev = gp->as.next))
			gp->as.next->as.prev = gp->as.prev;
		gp->as.next = NULL;
		gp->as.prev = &gp->as.next;
		gp->as.as->gp.numb--;
		as_put(xchg(&gp->as.as, NULL));
		/* unlink from SPP */
		if ((*gp->spp.prev = gp->spp.next))
			gp->spp.next->spp.prev = gp->spp.prev;
		gp->spp.next = NULL;
		gp->spp.prev = &gp->spp.next;
		gp->spp.spp->gp.numb--;
		spp_put(xchg(&gp->spp.spp, NULL));
		kmem_cache_free(ua_gp_cachep, gp);
		printd(("%s: %s: %p: deallocated gp structure\n", DRV_NAME, __FUNCTION__, gp));
		return;
	}
	swerr();
}

/*
 *  PRIV - Private Structure
 *  -----------------------------------
 *  Private structure allocation, deallocation and reference counting.
 */
STATIC union priv *
ua_alloc_priv(queue_t *q, union priv **pp, dev_t *devp, cred_t *crp, minor_t bminor)
{
	union priv *p;

	printd(("%s: %s: create priv dev = %d:%d\n", DRV_NAME, __FUNCTION__, getmajor(*devp),
		getminor(*devp)));
	if ((p = kmem_cache_alloc(ua_priv_cachep, SLAB_ATOMIC))) {
		str_t *s = (str_t *) p, **sp = (str_t **) pp;

		bzero(p, sizeof(*p));
		priv_get(p);	/* first get */
		s->u.dev.cmajor = getmajor(*devp);
		s->u.dev.cminor = getminor(*devp);
		s->cred = *crp;
		spin_lock_init(&s->qlock);	/* "priv-queue-lock" */
		(s->oq = RD(q))->q_ptr = priv_get(p);
		(s->iq = WR(q))->q_ptr = priv_get(p);
		switch (bminor) {
		default:
		case 0:	/* UA_OBJ_TYPE_SL_U */
			s->type = UA_OBJ_TYPE_SS_U;
			break;
		case 1:	/* UA_OBJ_TYPE_LM */
			s->type = UA_OBJ_TYPE_LM;
			master.lm = &p->lm;
			break;
		}
		s->o_prim = NULL;
		s->i_prim = slu_w_prim;
		s->i_state = LMI_UNUSABLE;
		s->i_style = LMI_STYLE2;
		s->i_version = 1;
		spin_lock_init(&s->lock);	/* "priv-lock" */
		/* place in master list */
		if ((s->next = *sp))
			s->next->prev = &s->next;
		s->prev = sp;
		*pp = priv_get(p);
		master.priv.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocated priv structure\n", DRV_NAME,
			__FUNCTION__));
	return (p);
}
STATIC void
ua_free_priv(queue_t *q)
{
	union priv *p = PRIV(q);
	struct str *s = &p->str;

	ensure(p, return);
	printd(("%s: %s: %p: free priv %d:%d\n", DRV_NAME, __FUNCTION__, s, s->u.dev.cmajor,
		s->u.dev.cminor));
	spin_lock_ua(&s->lock);
	{
		noenable(s->oq);
		noenable(s->iq);
		/* stopping bufcalls */
		ss7_unbufcall(s);
		/* flushing buffsers */
		flushq(s->oq, FLUSHALL);
		flushq(s->iq, FLUSHALL);
		/* remove from master list */
		if ((*s->prev = s->next))
			s->next->prev = s->prev;
		s->next = NULL;
		s->prev = &s->next;
		ensure(atomic_read(&s->refcnt) > 1, priv_get(p));
		priv_put(p);
		assure(master.priv.numb > 0);
		master.priv.numb--;
		if (s == (str_t *) master.lm)
			master.lm = NULL;
		/* remove from queues */
		ensure(atomic_read(&s->refcnt) > 1, priv_get(p));
		priv_put(xchg(&s->oq->q_ptr, NULL));
		ensure(atomic_read(&s->refcnt) > 1, priv_get(p));
		priv_put(xchg(&s->iq->q_ptr, NULL));
		/* done check final count */
		if (atomic_read(&s->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: priv lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, s, atomic_read(&s->refcnt)));
			atomic_set(&s->refcnt, 1);
		}
	}
	spin_unlock_ua(&s->lock);
	priv_put(p);		/* final put */
	return;
}
STATIC union priv *
priv_get(union priv *priv)
{
	if (priv) {
		atomic_inc(&priv->str.refcnt);
		return (priv);
	}
	swerr();
	return (priv);
}
STATIC void
priv_put(union priv *priv)
{
	if (priv) {
		assure(atomic_read(&priv->str.refcnt) > 1);
		if (atomic_dec_and_test(&priv->str.refcnt)) {
			kmem_cache_free(ua_priv_cachep, priv);
			printd(("%s: %s: %p: deallocated priv structure\n", DRV_NAME,
				__FUNCTION__, priv));
		}
		return;
	}
	swerr();
}

/*
 *  AS - Application Server
 *  -----------------------------------
 *  Application server allocation, deallocation and reference counting.
 */
STATIC struct as *
ua_alloc_as(int id, int type, struct sp *sp, uint32_t iid, ua_addr_t * add)
{
	struct as *as, **p;

	printd(("%s: %s: create as->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((as = kmem_cache_alloc(ua_as_cachep, SLAB_ATOMIC))) {
		struct spp *spp;
		struct np *np;

		bzero(as, sizeof(*as));
		as_get(as);	/* first get */
		spin_lock_init(&as->lock);	/* "as-lock" */
		as->id = id;
		as->type = type;
		as->iid = iid;
		as->add = *add;
		/* insert into master list in ascending id order */
		for (p = &master.as.list; *p && (*p)->id < id; p = &(*p)->next) ;
		if ((as->next = *p))
			as->next->prev = &as->next;
		as->prev = p;
		*p = as_get(as);
		master.as.numb++;
		/* link to sp */
		if ((as->sp.next = sp->as.list))
			as->sp.next->sp.prev = &as->sp.next;
		as->sp.prev = &sp->as.list;
		as->sp.sp = sp_get(sp);
		sp->as.list = as_get(as);
		sp->as.numb++;
		/* generate graph nodes for all of the SP's SPPs */
		for (spp = sp->spp.list; spp; spp = spp->sp.next)
			if (!ua_alloc_gp(as, spp)) {
				ua_free_as(as);
				return (NULL);
			}
		/* generate graph nodes to all matching AS for other SPs */
		if ((np = sp->np.list)) {
			struct as *a2;

			if (sp == np->u.sp)
				for (; np; np = np->u.next)
					for (a2 = np->p.sp->as.list; a2; a2 = a2->sp.next)
						if ((!iid && a2->iid == iid)
						    || (add->spid == a2->add.spid
							&& add->sdti == a2->add.sdti
							&& add->sdli == a2->add.sdli))
							if (!ua_alloc_ap(as, a2)) {
								ua_free_as(as);
								return (NULL);
							}
			if (sp == np->p.sp)
				for (; np; np = np->p.next)
					for (a2 = np->u.sp->as.list; a2; a2 = a2->sp.next)
						if ((!iid && a2->iid == iid)
						    || (add->spid == a2->add.spid
							&& add->sdti == a2->add.sdti
							&& add->sdli == a2->add.sdli))
							if (!ua_alloc_ap(a2, as)) {
								ua_free_as(as);
								return (NULL);
							}
		}
		/* as structures are created without any linked ss structures */
	} else
		printd(("%s: %s: ERROR: failed to allocate as structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (as);
}
STATIC void
ua_free_as(struct as *as)
{

	ensure(as, return);
	printd(("%s: %s: %p free as->id = %ld\n", DRV_NAME, __FUNCTION__, as, as->id));
	spin_lock_ua(&as->lock);
	{
		struct ss *ss;
		struct ap *ap;
		struct gp *gp;

		/* unlink from ss */
		while ((ss = as->ss.list)) {
			fixme(("Disable and hangup ss\n"));
			ua_free_ss(ss);
		}
		/* unlink from other as */
		while ((ap = as->ap.list))
			ua_free_ap(ap);
		/* unlink from spp */
		while ((gp = as->gp.list))
			ua_free_gp(gp);
		/* unlink from sp */
		if ((*as->sp.prev = as->sp.next))
			as->sp.next->sp.prev = as->sp.prev;
		as->sp.next = NULL;
		as->sp.prev = &as->sp.next;
		ensure(atomic_read(&as->refcnt) > 1, as_get(as));
		as_put(as);
		assure(as->sp.sp->as.numb > 0);
		as->sp.sp->as.numb--;
		sp_put(xchg(&as->sp.sp, NULL));
		/* remove from master list */
		if ((*as->prev = as->next))
			as->next->prev = as->prev;
		as->next = NULL;
		as->prev = &as->next;
		ensure(atomic_read(&as->refcnt) > 1, as_get(as));
		as_put(as);
		assure(master.as.numb > 0);
		master.as.numb++;
		/* done, check final count */
		if (atomic_read(&as->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: as lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, as, atomic_read(&as->refcnt)));
			atomic_set(&as->refcnt, 1);
		}
	}
	spin_unlock_ua(&as->lock);
	as_put(as);		/* final put */
}
STATIC struct as *
as_get(struct as *as)
{
	if (as) {
		atomic_inc(&as->refcnt);
		return (as);
	}
	swerr();
	return (as);
}
STATIC void
as_put(struct as *as)
{
	if (as) {
		assure(atomic_read(&as->refcnt) > 1);
		if (atomic_dec_and_test(&as->refcnt)) {
			kmem_cache_free(ua_as_cachep, as);
			printd(("%s: %s: %p: deallocated as structure\n", DRV_NAME,
				__FUNCTION__, as));
		}
		return;
	}
	swerr();
}
STATIC int
as_get_id(int id)
{
	struct as *as;

	if (!id) {
		id = 1;
		for (as = master.as.list; as; as = as->next)
			if (as->id == id)
				id++;
			else if (as->id > id)
				break;
	}
	return (id);
}
STATIC struct as *
as_lookup(int id)
{
	struct as *as;

	for (as = master.as.list; as && as->id != id; as = as->next) ;
	return (as);
}

/*
 *  SP - Signalling Process
 *  -----------------------------------
 *  Signalling process allocation, deallocation and reference counting.
 */
STATIC struct sp *
ua_alloc_sp(int id, int type, struct sp *osp, int cost, int tmode)
{
	struct sp *sp, **p;

	printd(("%s: %s: create sp->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((sp = kmem_cache_alloc(ua_sp_cachep, SLAB_ATOMIC))) {
		bzero(sp, sizeof(*sp));
		sp_get(sp);	/* first get */
		spin_lock_init(&sp->lock);	/* "sp-lock" */
		sp->id = id;
		sp->type = type;
		sp->cost = cost;
		sp->tmode = tmode;
		/* insert into master list in ascending id order */
		for (p = &master.sp.list; *p && (*p)->id < id; p = &(*p)->next) ;
		if ((sp->next = *p))
			sp->next->prev = &sp->next;
		sp->prev = p;
		*p = sp_get(sp);
		master.sp.numb++;
		if (osp) {
			/* link to other sp */
			if (!ua_alloc_np(sp, osp)) {
				ua_free_sp(sp);
				return (NULL);
			}
		}
		/* when sp are created they have no as and no spp */
	} else
		printd(("%s: %s: ERROR: failed to allocate sp structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (sp);
}
STATIC void
ua_free_sp(struct sp *sp)
{

	ensure(sp, return);
	printd(("%s: %s: %p free sp->id = %ld\n", DRV_NAME, __FUNCTION__, sp, sp->id));
	spin_lock_ua(&sp->lock);
	{
		/* unlink from as */
		while (sp->as.list)
			ua_free_as(sp->as.list);
		/* unlink from spp */
		while (sp->spp.list)
			ua_free_spp(sp->spp.list);
		/* unlink from other sp */
		while (sp->np.list)
			ua_free_np(sp->np.list);
		/* remove from master list */
		if ((*sp->prev = sp->next))
			sp->next->prev = sp->prev;
		sp->next = NULL;
		sp->prev = &sp->next;
		ensure(atomic_read(&sp->refcnt) > 1, sp_get(sp));
		sp_put(sp);
		assure(master.sp.numb > 0);
		master.sp.numb--;
		/* done, check final count */
		if (atomic_read(&sp->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: sp lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, sp, atomic_read(&sp->refcnt)));
			atomic_set(&sp->refcnt, 1);
		}
	}
	spin_unlock_ua(&sp->lock);
	sp_put(sp);		/* final put */
}
STATIC struct sp *
sp_get(struct sp *sp)
{
	if (sp) {
		atomic_inc(&sp->refcnt);
		return (sp);
	}
	swerr();
	return (sp);
}
STATIC void
sp_put(struct sp *sp)
{
	if (sp) {
		assure(atomic_read(&sp->refcnt) > 1);
		if (atomic_dec_and_test(&sp->refcnt)) {
			kmem_cache_free(ua_sp_cachep, sp);
			printd(("%s: %s: %p: deallocated sp structure\n", DRV_NAME,
				__FUNCTION__, sp));
		}
		return;
	}
	swerr();
}
STATIC int
sp_get_id(int id)
{
	struct sp *sp;

	if (!id) {
		id = 1;
		for (sp = master.sp.list; sp; sp = sp->next)
			if (sp->id == id)
				id++;
			else if (sp->id > id)
				break;
	}
	return (id);
}
STATIC struct sp *
sp_lookup(int id)
{
	struct sp *sp;

	for (sp = master.sp.list; sp && sp->id != id; sp = sp->next) ;
	return (sp);
}

/*
 *  SPP - Signalling Process Proxy
 *  -----------------------------------
 *  Signalling process proxy allocation, deallocation and reference counting.
 */
STATIC struct spp *
ua_alloc_spp(int id, int type, struct sp *sp, int aspid, int cost)
{
	struct spp *spp, **p;

	printd(("%s: %s: create spp->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((spp = kmem_cache_alloc(ua_spp_cachep, SLAB_ATOMIC))) {
		struct as *as;

		bzero(spp, sizeof(*spp));
		spp_get(spp);	/* first get */
		spin_lock_init(&spp->lock);	/* "spp-lock" */
		spp->id = id;
		spp->type = type;
		spp->aspid = aspid;
		spp->cost = cost;
		/* insert into master list in ascending id order */
		for (p = &master.spp.list; *p && (*p)->id < id; p = &(*p)->next) ;
		if ((spp->next = *p))
			spp->next->prev = &spp->next;
		spp->prev = p;
		*p = spp_get(spp);
		master.spp.numb++;
		/* link to sp */
		if ((spp->sp.next = sp->spp.list))
			spp->sp.next->sp.prev = &spp->sp.next;
		spp->sp.prev = &sp->spp.list;
		spp->sp.sp = sp_get(sp);
		sp->spp.list = spp_get(spp);
		sp->spp.numb++;
		/* generate graph nodes for all other SP's AS's */
		for (as = sp->as.list; as; as = as->sp.next)
			if (!ua_alloc_gp(as, spp)) {
				ua_free_spp(spp);
				return (NULL);
			}
		/* spp structures are created without any linked xp structures */
	} else
		printd(("%s: %s: ERROR: failed to allocate spp structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (spp);
}
STATIC void
ua_free_spp(struct spp *spp)
{

	ensure(spp, return);
	printd(("%s: %s: %p free spp->id = %ld\n", DRV_NAME, __FUNCTION__, spp, spp->id));
	spin_lock_ua(&spp->lock);
	{
		struct pp *pp;
		struct gp *gp;

		/* unlink from xp */
		while ((pp = spp->pp.list))
			ua_free_pp(pp);
		/* unlink from as */
		while ((gp = spp->gp.list))
			ua_free_gp(gp);
		/* unlink from sp */
		if ((*spp->sp.prev = spp->sp.next))
			spp->sp.next->sp.prev = spp->sp.prev;
		spp->sp.next = NULL;
		spp->sp.prev = &spp->sp.next;
		ensure(atomic_read(&spp->refcnt) > 1, spp_get(spp));
		spp_put(spp);
		assure(spp->sp.sp->spp.numb > 0);
		spp->sp.sp->spp.numb--;
		sp_put(xchg(&spp->sp.sp, NULL));
		/* remove from master list */
		if ((*spp->prev = spp->next))
			spp->next->prev = spp->prev;
		spp->next = NULL;
		spp->prev = &spp->next;
		ensure(atomic_read(&spp->refcnt) > 1, spp_get(spp));
		spp_put(spp);
		assure(master.spp.numb > 0);
		master.spp.numb++;
		/* done, check final count */
		if (atomic_read(&spp->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: spp lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, spp, atomic_read(&spp->refcnt)));
			atomic_set(&spp->refcnt, 1);
		}
	}
	spin_unlock_ua(&spp->lock);
	spp_put(spp);		/* final put */
}
STATIC struct spp *
spp_get(struct spp *spp)
{
	if (spp) {
		atomic_inc(&spp->refcnt);
		return (spp);
	}
	swerr();
	return (spp);
}
STATIC void
spp_put(struct spp *spp)
{
	if (spp) {
		assure(atomic_read(&spp->refcnt) > 1);
		if (atomic_dec_and_test(&spp->refcnt)) {
			kmem_cache_free(ua_spp_cachep, spp);
			printd(("%s: %s: %p: deallocated spp structure\n", DRV_NAME,
				__FUNCTION__, spp));
		}
		return;
	}
	swerr();
}
STATIC int
spp_get_id(int id)
{
	struct spp *spp;

	if (!id) {
		id = 1;
		for (spp = master.spp.list; spp; spp = spp->next)
			if (spp->id == id)
				id++;
			else if (spp->id > id)
				break;
	}
	return (id);
}
STATIC struct spp *
spp_lookup(int id)
{
	struct spp *spp;

	for (spp = master.spp.list; spp && spp->id != id; spp = spp->next) ;
	return (spp);
}

/*
 *  SS - Signalling Service
 *  -----------------------------------
 *  Signalling service allocation, deallocation and reference counting.
 */
STATIC void
ua_alloc_ss(struct ss *ss, int id, int type, struct as *as, int iid, ua_addr_t * add)
{
	ss->id = id;
	ss->type = type;
	ss->iid = iid;
	ss->add = *add;
	/* link to AS */
	if ((ss->as.next = as->ss.list))
		ss->as.prev = &as->ss.list;
	ss->as.as = as_get(as);
	as->ss.list = ss_get(ss);
	as->ss.numb++;
	ss->o_prim = NULL;
	ss->i_prim = &slp_r_prim;
	/* link is characterized, it can now be enabled */
	enableok(ss->oq);
	enableok(ss->iq);
	return;
}
STATIC void
ua_free_ss(struct ss *ss)
{

	ensure(ss, return);
	printd(("%s: %s: %p free ss index = %lu\n", DRV_NAME, __FUNCTION__, ss, ss->u.mux.index));
	spin_lock_ua(&ss->lock);
	{
		noenable(ss->oq);
		noenable(ss->iq);
		/* remove from AS */
		if (ss->as.as) {
			fixme(("Notify any active AS that we have gone away.\n"));
			if ((*ss->as.prev = ss->as.next))
				ss->as.next->as.prev = ss->as.prev;
			ss->as.next = NULL;
			ss->as.prev = &ss->as.next;
			ensure(atomic_read(&ss->refcnt) > 1, ss_get(ss));
			ss_put(ss);
			assure(ss->as.as->ss.numb > 0);
			ss->as.as->ss.numb--;
			as_put(xchg(&ss->as.as, NULL));
		}
		ss->id = 0;
		ss->type = 0;
		/* flushing buffers */
		ss7_unbufcall((str_t *) ss);
		flushq(ss->oq, FLUSHALL);
		flushq(ss->iq, FLUSHALL);
		/* remove from master list */
		if ((*ss->prev = ss->next))
			ss->next->prev = ss->prev;
		ss->next = NULL;
		ss->prev = &ss->next;
		ensure(atomic_read(&ss->refcnt) > 1, ss_get(ss));
		ss_put(ss);
		assure(master.link.numb > 0);
		master.link.numb--;
		/* remove from queues */
		ensure(atomic_read(&ss->refcnt) > 1, ss_get(ss));
		ss_put(xchg(&ss->oq->q_ptr, NULL));
		ensure(atomic_read(&ss->refcnt) > 1, ss_get(ss));
		ss_put(xchg(&ss->iq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&ss->refcnt) != 1) {
			__printd(("%s; %s: %p: ERROR: ss lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, ss, atomic_read(&ss->refcnt)));
			atomic_set(&ss->refcnt, 1);
		}
	}
	spin_unlock_ua(&ss->lock);
	ss_put(ss);		/* final put */
}
STATIC struct ss *
ss_get(struct ss *ss)
{
	if (ss) {
		atomic_inc(&ss->refcnt);
		return (ss);
	}
	swerr();
	return (ss);
}
STATIC void
ss_put(struct ss *ss)
{
	if (ss) {
		assure(atomic_read(&ss->refcnt) > 1);
		if (atomic_dec_and_test(&ss->refcnt)) {
			kmem_cache_free(ua_link_cachep, ss);
			printd(("%s: %s: %p: deallocated ss structure\n", DRV_NAME,
				__FUNCTION__, ss));
		}
		return;
	}
}
STATIC int
ss_get_id(int id)
{
	static int identifier = 0;

	if (!id)
		id = ++identifier;
	return (id);
}
STATIC struct ss *
ss_lookup(int id)
{
	struct ss *ss;

	for (ss = (struct ss *) master.link.list; ss; ss = ss->next) {
		if (ss->type != UA_OBJ_TYPE_SS_U && ss->type != UA_OBJ_TYPE_SS_P)
			continue;
		if (ss->id != id)
			continue;
		break;
	}
	return (ss);
}

/*
 *  RP - GP to PP Mapping
 *  -----------------------------------
 *  GP to PP mapping allocation, deallocation and reference counting.
 */
static void
ua_free_rp(struct rp *rp)
{
	if (rp) {
		struct gp *gp = rp->gp.gp;

		/* Freeing an RP structure should not result in a state change. */
		if (rp_get_state(rp) != AS_DOWN) {
			swerr();
			gp->rp.counts[rp->state]--;
			gp->rp.counts[AS_DOWN]++;
			rp->state = AS_DOWN;
			rp->flags = 0;
		}
		gp->rp.counts[AS_DOWN]--;
		/* unlink from GP */
		if ((*rp->gp.prev = rp->gp.next))
			rp->gp.next->gp.prev = rp->gp.prev;
		rp->gp.next = NULL;
		rp->gp.prev = &rp->gp.next;
		rp->gp.gp->rp.numb--;
		rp->gp.gp = NULL;

		/* unlink from PP */
		if ((*rp->pp.prev = rp->pp.next))
			rp->pp.next->pp.prev = rp->pp.prev;
		rp->pp.next = NULL;
		rp->pp.prev = &rp->pp.next;
		rp->pp.pp->rp.numb--;
		rp->pp.pp = NULL;

		kmem_cache_free(ua_rp_cachep, rp);
		printd(("%s: %s: %p: deallocated rp structure\n", DRV_NAME, __FUNCTION__, rp));
		return;
	}
	swerr();
}
static void
ua_alloc_rp(struct gp *gp, struct pp *pp)
{
	struct rp *rp;

	printd(("%s: %s: rp graph gp %ld pp %lu\n", DRV_NAME, __FUNCTION__, gp->id, pp->id));
	if ((rp = kmem_cache_alloc(ua_rp_cachep, SLAB_ATOMIC))) {
		bzero(rp, sizeof(rp));
		rp_set_state(rp, AS_DOWN);

		/* link to GP */
		if ((rp->gp.next = gp->rp.list))
			rp->gp.next->gp.prev = &rp->gp.next;
		rp->gp.prev = &gp->rp.list;
		rp->gp.gp = gp;
		gp->rp.list = rp;
		gp->rp.numb++;
		gp->rp.counts[AS_DOWN]++; /* will not change state of GP */

		/* link to PP */
		if ((rp->pp.next = pp->rp.list))
			rp->pp.next->pp.prev = &rp->pp.next;
		rp->pp.prev = &pp->rp.list;
		rp->pp.pp = pp;
		pp->rp.list = rp;
		pp->rp.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate rp structure for gp %ld pp %ld\n",
			DRV_NAME, __FUNCTION__, gp->id, pp->id));
	return (rp);
}

/*
 *  PP - SPP to XP Mapping
 *  -----------------------------------
 *  SPP to XP mapping allocation, deallocation and reference counting.
 */
static void
ua_free_pp(struct pp *pp)
{
	if (pp) {
		struct rp *rp;
		struct spp *spp = pp->spp.spp;

		/* remove RP graphs */
		while ((rp = pp->rp.list))
			ua_free_rp(rp);
		/* Freeing a PP structure should not result in a state change. */
		if (pp_get_state(pp) != ASP_DOWN) {
			swerr();
			spp->pp.counts[pp->state]--;
			spp->pp.counts[ASP_DOWN]++;
			pp->state = ASP_DOWN;
			pp->flags = 0;
		}
		spp->rp.counts[ASP_DOWN]--;
		/* unlink from SPP */
		if ((*pp->spp.prev = pp->spp.next))
			pp->spp.next->spp.prev = pp->spp.prev;
		pp->spp.next = NULL;
		pp->spp.prev = &pp->spp.next;
		pp->spp.spp->pp.numb--;
		spp_put(xchg(&pp->spp.spp, NULL));

		/* unlink from XP */
		if ((*pp->xp.prev = pp->xp.next))
			pp->xp.next->xp.prev = pp->xp.prev;
		pp->xp.next = NULL;
		pp->xp.prev = &pp->xp.next;
		pp->xp.xp->pp.numb--;
		xp_put(xchg(&pp->xp.xp, NULL));

		kmem_cache_free(ua_pp_cachep, pp);
		printd(("%s: %s: %p: deallocated pp structure\n", DRV_NAME, __FUNCTION__, pp));
		return;
	}
	swerr();
}
/**
 *  ua_alloc_pp: allocate SPP to XP mapping structure
 *  @spp: SPP to map
 *  @xp: XP tp map
 *
 *  PP structure start life in the ASP_DOWN state.  If the structure is allocated to link an SGP
 *  or SPP to a transport, it is desirable to initiate the ASP Up procedure (i.e. send an ASP Up
 *  message with the ASP Id of the SGP or SPP).  Once the ASP Up procedure completes, we will also
 *  initiate a Regsitration procedure for each dynamic AS whose RP structure is in the AS_DOWN
 *  state.
 */
static void
ua_alloc_pp(struct spp *spp, struct xp *xp)
{
	struct pp *pp;

	printd(("%s: %s: pp graph spp %ld xp %lu\n", DRV_NAME, __FUNCTION__, spp->id, xp->id));
	if ((pp = kmem_cache_alloc(ua_pp_cachep, SLAB_ATOMIC))) {
		bzero(pp, sizeof(pp));
		pp_set_state(pp, ASP_DOWN);

		/* link to SPP */
		if ((pp->spp.next = spp->pp.list))
			pp->spp.next->spp.prev = &pp->spp.next;
		pp->spp.prev = &spp->pp.list;
		pp->spp.spp = spp_get(spp);
		spp->pp.list = pp;
		spp->pp.numb++;
		spp->pp.counts[ASP_DOWN]++;	/* will not change state of SPP */

		/* link to XP */
		if ((pp->xp.next = xp->pp.list))
			pp->xp.next->xp.prev = &pp->xp.next;
		pp->xp.prev = &xp->pp.list;
		pp->xp.xp = xp_get(xp);
		xp->pp.list = pp;
		xp->pp.numb++;
		{
			struct gp *gp;

			/* allocate one RP structure for each GP to PP pair */
			for (gp = spp->gp.list; gp; gp = gp->spp.next) {
				if (ua_alloc_rp(gp, pp) == NULL) {
					ua_free_pp(pp);
					goto failed;
				}
			}
		}
		switch (spp->type) {
		case UA_OBJ_TYPE_ASP:
			break;
		case UA_OBJ_TYPE_SGP:
		case UA_OBJ_TYPE_SPP:
		{
			static const char info[] = "Automatic ASP Up procedure.";
			uint32_t aspid_val = spp->sp.sp->np.list->u.sp->aspid;
			uint32_t *aspid = aspid_val ? &aspid_val : NULL;
			int err;

			spp->pp.counts[ASP_DOWN]--;
			spp->pp.counts[ASP_WACK_ASPUP]++;
			if (spp_get_state(spp) < ASP_WACK_ASPUP)
				spp_set_state(spp, ASP_WACK_ASPUP);
			/* Note, never propagates a state change */
			pp_timer_start(pp, tack);

			mp = ua_build_asps_aspup_req(NULL, xp->ppi, aspid, info, sizeof(info));
			ua_send_optdata_req(xp->oq, mp, NULL);
		}
		}
		return (pp);
	}
      failed:
	printd(("%s: %s: ERROR: failed to allocate pp structure for spp %ld xp %ld\n",
		DRV_NAME, __FUNCTION__, spp->id, xp->id));
	return (NULL);
}

/*
 *  XP - Transport Provider
 *  -----------------------------------
 *  Transport provider allocation, deallocation and reference counting.
 */
STATIC void
ua_free_xp(struct xp *xp)
{

	ensure(xp, return);
	printd(("%s: %s: %p free xp index = %lu\n", DRV_NAME, __FUNCTION__, xp, xp->u.mux.index));
	spin_lock_ua(&xp->lock);
	{
		mblk_t *b_next, *bp;
		struct pp *pp;

		noenable(xp->iq);
		noenable(xp->oq);
		/* trash normal reassembly buffer */
		if ((b_next = xp->nm_reassem)) {
			while ((bp = b_next)) {
				b_next = bp->b_next;
				freemsg(bp);
			}
			xp->nm_reassem = NULL;
		}
		/* trash expedited reassembly buffer */
		if ((b_next = xp->ex_reassem)) {
			while ((bp = b_next)) {
				b_next = bp->b_next;
				freemsg(bp);
			}
			xp->ex_reassem = NULL;
		}
		/* unlink from spp */
		while ((pp = xp->pp.list))
			ua_free_pp(pp);

		/* unlink from sp */
		if (xp->sp) {
			sp_put(xchg(&xp->sp, NULL));
		}
		xp->id = 0;
		xp->type = 0;
		/* flushing buffers */
		ss7_unbufcall((str_t *) xp);
		flushq(xp->iq, FLUSHALL);
		flushq(xp->oq, FLUSHALL);
		/* remove from master list */
		if ((*xp->prev = xp->next))
			xp->next->prev = xp->prev;
		xp->next = NULL;
		xp->prev = &xp->next;
		ensure(atomic_read(&xp->refcnt) > 1, xp_get(xp));
		xp_put(xp);
		assure(master.link.numb > 0);
		master.link.numb--;
		/* remove from queues */
		ensure(atomic_read(&xp->refcnt) > 1, xp_get(xp));
		xp_put(xchg(&xp->iq->q_ptr, NULL));
		ensure(atomic_read(&xp->refcnt) > 1, xp_get(xp));
		xp_put(xchg(&xp->oq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&xp->refcnt) != 1) {
			__printd(("%s; %s: %p: ERROR: xp lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, xp, atomic_read(&xp->refcnt)));
			atomic_set(&xp->refcnt, 1);
		}
	}
	spin_unlock_ua(&xp->lock);
	xp_put(xp);		/* final put */
}
STATIC int
ua_alloc_xp(struct xp *xp, int id, int type, struct spp *spp, struct sp *sp)
{
	if (spp) {
		switch (type) {
		case UA_OBJ_TYPE_XP_XSP:
			if (spp->type != UA_OBJ_TYPE_ASP) {
				swerr();
				return (-EINVAL);
			}
			break;
		case UA_OBJ_TYPE_XP_XGP:
			if (spp->type != UA_OBJ_TYPE_SGP) {
				swerr();
				return (-EINVAL);
			}
			break;
		case UA_OBJ_TYPE_XP_XPP:
			if (spp->type != UA_OBJ_TYPE_SPP) {
				swerr();
				return (-EINVAL);
			}
			break;
		}
		/* link to SPP: this is done when we know to which ASP, SGP or SPP, the stream
		   corresponds.  If the aspid of the corresponding SPP is zero, we will not require 
		   an ASPID on ASPUP. If the aspid is non-zero, any provided ASPID must match the
		   ASPID set.  Note that this will also automatically allocate RP structures for
		   each GP structure belonging to the SPP. */
		if (ua_alloc_pp(spp, xp) == NULL)
			return (-ENOMEM);

		/* For cross-links we also need to allocate PP structures representing ASP or SGP
		   accessible via the peer.  That is, SGP for XSP, ASP for XGP or XSP. */
		switch (type) {
			struct np *np;

		case UA_OBJ_TYPE_XP_XSP:
			for (np = spp->sp.sp->np.list; np; np = np->u.next) {
				struct spp *sgp;

				for (sgp = np->p.sp->spp.list; sgp; sgp = sgp->sp.next) {
					if (ua_alloc_pp(sgp, xp) == NULL) {
						ua_free_xp(xp);
						return (-ENOMEM);
					}
				}
			}
			break;
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			for (np = spp->sp.sp->np.list; np; np = np->p.next) {
				struct ssp *asp;

				for (asp = np->u.sp->spp.list; asp; asp = asp->sp.next) {
					if (ua_alloc_pp(asp, xp) == NULL) {
						ua_free_xp(xp);
						return (-ENOMEM);
					}
				}
			}
			break;
		}

	} else if (sp) {
		/* link to SP: this is done when we don't know the ASP, SGP or SPP to which the
		   transport stream corresponds, and we will wait for the ASPUP with an ASPID to
		   determine to which ASP the stream cooresponds.  This is also used for XGP
		   connections to indicated the type but demand allocate cross-link structures. */
		switch (type) {
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			swerr();
			return (-EINVAL);
		}
		xp->sp = sp_get(sp);
	} else {
		swerr();
		return (-EINVAL);
	}
	xp->id = id;
	xp->type = type;
	switch (type) {
	case UA_OBJ_TYPE_XP_ASP:
	case UA_OBJ_TYPE_XP_SGP:
	case UA_OBJ_TYPE_XP_SPP:
	case UA_OBJ_TYPE_XP_XSP:
	case UA_OBJ_TYPE_XP_XGP:
	case UA_OBJ_TYPE_XP_XPP:
		xp->o_prim = NULL;
		xp->i_prim = &xp_r_prim;
		break;
	}
	/* link is characterized, it can now be enabled (and scheduled) */
	enableok(xp->iq);
	enableok(xp->oq);
	qenable(xp->iq);
	qenable(xp->oq);
	return (0);
}
STATIC struct xp *
xp_get(struct xp *xp)
{
	if (xp) {
		atomic_inc(&xp->refcnt);
		return (xp);
	}
	swerr();
	return (xp);
}
STATIC void
xp_put(struct xp *xp)
{
	if (xp) {
		assure(atomic_read(&xp->refcnt) > 1);
		if (atomic_dec_and_test(&xp->refcnt)) {
			kmem_cache_free(ua_link_cachep, xp);
			printd(("%s: %s: %p: deallocated xp structure\n", DRV_NAME,
				__FUNCTION__, xp));
		}
		return;
	}
}
STATIC int
xp_get_id(int id)
{
	static int identifier = 0;

	if (!id)
		id = ++identifier;
	return (id);
}
STATIC struct xp *
xp_lookup(int id)
{
	struct xp *xp;

	for (xp = (struct xp *) master.link.list; xp; xp = xp->next) {
		switch (xp->type) {
		case UA_OBJ_TYPE_XP_ASP:
		case UA_OBJ_TYPE_XP_SGP:
		case UA_OBJ_TYPE_XP_SPP:
		case UA_OBJ_TYPE_XP_XSP:
		case UA_OBJ_TYPE_XP_XGP:
		case UA_OBJ_TYPE_XP_XPP:
			break;
		default:
			continue;
		}
		if (xp->id != id)
			continue;
	}
	return (xp);
}

/*
 *  LINK - Untyped streams linked under the mux.
 *  -----------------------------------
 *  Untyped stream link allocation, deallocation and reference counting.
 */
STATIC union link *
ua_alloc_link(queue_t *q, union link **lkp, int index, cred_t *crp)
{
	union link *lk;

	printd(("%s: %s: create link index = %lu\n", DRV_NAME, __FUNCTION__, index));
	if ((lk = kmem_cache_alloc(ua_link_cachep, SLAB_ATOMIC))) {
		bzero(lk, sizeof(*lk));
		link_get(lk);	/* first get */
		lk->str.u.mux.index = index;
		lk->str.cred = *crp;
		spin_lock_init(&lk->str.qlock);	/* "link-queue_lock" */
		(lk->str.iq = RD(q))->q_ptr = link_get(lk);
		(lk->str.oq = WR(q))->q_ptr = link_get(lk);
		lk->str.i_state = LMI_UNUSABLE;
		lk->str.i_style = LMI_STYLE2;
		lk->str.i_version = 1;
		spin_lock_init(&lk->str.lock);	/* "link-lock" */
		/* place in master list */
		if ((lk->str.next = (str_t *) * lkp))
			lk->str.next->prev = &lk->str.next;
		lk->str.prev = (str_t **) lkp;
		*lkp = link_get(lk);
		master.link.numb++;
		/* link must remain disabled until it is characterized */
		noenable(lk->str.iq);
		noenable(lk->str.oq);
	} else
		printd(("%s: %s: ERROR: failed to allocate link structure %lu\n", DRV_NAME,
			__FUNCTION__, index));
	return (lk);
}
STATIC void
ua_free_link(queue_t *q)
{
	union link *lk = LINK(q);

	ensure(lk, return);
	switch (lk->str.type) {
	case UA_OBJ_TYPE_SS_U:
	case UA_OBJ_TYPE_SS_P:
		return ua_free_ss(&lk->ss);
	case UA_OBJ_TYPE_XP_ASP:
	case UA_OBJ_TYPE_XP_SGP:
	case UA_OBJ_TYPE_XP_SPP:
	case UA_OBJ_TYPE_XP_XSP:
	case UA_OBJ_TYPE_XP_XGP:
	case UA_OBJ_TYPE_XP_XPP:
		return ua_free_xp(&lk->xp.xp);
	case 0:
		break;
	default:
		swerr();
		break;
	}
	/* untyped link not assigned to anything */
	printd(("%s: %s: %p free link index = %lu\n", DRV_NAME, __FUNCTION__, lk,
		lk->str.u.mux.index));
	spin_lock_ua(&lk->str.lock);
	{
		noenable(lk->str.iq);
		noenable(lk->str.oq);
		/* flushing buffers */
		ss7_unbufcall(&lk->str);
		flushq(lk->str.iq, FLUSHALL);
		flushq(lk->str.oq, FLUSHALL);
		/* remove from master list */
		if ((*lk->str.prev = lk->str.next))
			lk->str.next->prev = lk->str.prev;
		lk->str.next = NULL;
		lk->str.prev = &lk->str.next;
		ensure(atomic_read(&lk->str.refcnt) > 1, link_get(lk));
		link_put(lk);
		assure(master.link.numb > 0);
		master.link.numb--;
		/* remove from queues */
		ensure(atomic_read(&lk->str.refcnt) > 1, link_get(lk));
		link_put(xchg(&lk->str.iq->q_ptr, NULL));
		ensure(atomic_read(&lk->str.refcnt) > 1, link_get(lk));
		link_put(xchg(&lk->str.oq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&lk->str.refcnt) != 1) {
			__printd(("%s; %s: %p: ERROR: link lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, lk, atomic_read(&lk->str.refcnt)));
			atomic_set(&lk->str.refcnt, 1);
		}
	}
	spin_unlock_ua(&lk->str.lock);
	link_put(lk);		/* final put */
}
STATIC union link *
link_get(union link *lk)
{
	if (lk) {
		atomic_inc(&lk->str.refcnt);
		return (lk);
	}
	swerr();
	return (lk);
}
STATIC void
link_put(union link *lk)
{
	if (lk) {
		assure(atomic_read(&lk->str.refcnt) > 1);
		if (atomic_dec_and_test(&lk->str.refcnt)) {
			kmem_cache_free(ua_link_cachep, lk);
			printd(("%s: %s: %p: deallocated link structure\n", DRV_NAME,
				__FUNCTION__, lk));
		}
		return;
	}
}
STATIC union link *
link_lookup(int index)
{
	union link *lk;

	for (lk = master.link.list; lk && lk->str.u.mux.index != index;
	     lk = (union link *) lk->str.next) ;
	return (lk);
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the INET driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the INET driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw ua_cdev = {
	.d_name = DRV_NAME,
	.d_str = &uainfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
ua_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&ua_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
ua_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&ua_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
ua_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &uainfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
ua_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
uaterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (ua_majors[mindex]) {
			if ((err = ua_unregister_strdev(ua_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					ua_majors[mindex]);
			if (mindex)
				ua_majors[mindex] = 0;
		}
	}
	if ((err = ua_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
uainit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = ua_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		uaterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = ua_register_strdev(ua_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					ua_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				uaterminate();
				return (err);
			}
		}
		if (ua_majors[mindex] == 0)
			ua_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(ua_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = ua_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(uainit);
module_exit(uaterminate);

#endif				/* LINUX */
