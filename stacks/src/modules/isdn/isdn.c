/*****************************************************************************

 @(#) $RCSfile: isdn.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/08/29 20:25:12 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/29 20:25:12 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: isdn.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/08/29 20:25:12 $"

static char const ident[] =
    "$RCSfile: isdn.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/08/29 20:25:12 $";

/*
 *  This is an ISDN (DSS1) Layer 3 (Q.931) modules which can be pushed over a
 *  Data Link Layer (Q.921) module to effect an OpenSS7 ISDN D-Channel.
 *  Having the ISDN state machines separate permits live upgrade and allows
 *  this state machine to be rigorously conformance tested once for all
 *  D-channel drivers.
 */
#include "compat.h"

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <sys/dlpi.h>
#include <sys/dlpi_isdn.h>
#include <ss7/cci.h>
#include <ss7/isdni.h>
#include <ss7/isdni_ioctl.h>

#define ISDN_DESCRIP	"INTEGRATED SERVICES DIGITAL NETWORK (ISDN/Q.931) STREAMS DRIVER."
#define ISDN_REVISION	"LfS $RCSfile: isdn.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/08/29 20:25:12 $"
#define ISDN_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define ISDN_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define ISDN_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ISDN_LICENSE	"GPL"
#define ISDN_BANNER	ISDN_DESCRIP	"\n" \
			ISDN_REVISION	"\n" \
			ISDN_COPYRIGHT	"\n" \
			ISDN_DEVICE	"\n" \
			ISDN_CONTACT
#define ISDN_SPLASH	ISDN_DESCRIP	"\n" \
			ISDN_REVISION

#ifdef LINUX
MODULE_AUTHOR(ISDN_CONTACT);
MODULE_DESCRIPTION(ISDN_DESCRIP);
MODULE_SUPPORTED_DEVICE(ISDN_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ISDN_LICENSE);
#endif				/* MODULE_LICENSE */
#endif				/* LINUX */

#ifdef LFS
#define ISDN_DRV_ID		CONFIG_STREAMS_ISDN_MODID
#define ISDN_DRV_NAME		CONFIG_STREAMS_ISDN_NAME
#define ISDN_CMAJORS		CONFIG_STREAMS_ISDN_NMAJORS
#define ISDN_CMAJOR_0		CONFIG_STREAMS_ISDN_MAJOR
#define ISDN_UNITS		CONFIG_STREAMS_ISDN_NMINORS
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		ISDN_DRV_ID
#define DRV_NAME	ISDN_DRV_NAME
#define CMAJORS		ISDN_CMAJORS
#define CMAJOR_0	ISDN_CMAJOR_0
#define UNITS		ISDN_UNITS
#ifdef MODULE
#define DRV_BANNER	ISDN_BANNER
#else				/* MODULE */
#define DRV_BANNER	ISDN_SPLASH
#endif				/* MODULE */

STATIC struct module_info isdn_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC struct module_info isdn_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC struct module_info dl_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC struct module_info dl_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC int isdn_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int isdn_close(queue_t *, int, cred_t *);

STATIC struct qinit isdn_rinit = {
	qi_putp:ss7_oput,		/* Read put (msg from below) */
	qi_srvp:ss7_osrv,		/* Read queue service */
	qi_qopen:isdn_open,		/* Each open */
	qi_qclose:isdn_close,		/* Last close */
	qi_minfo:&isdn_rinfo,		/* Information */
};

STATIC struct qinit isdn_winit = {
	qi_putp:ss7_iput,		/* Write put (msg from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&isdn_winfo,		/* Information */
};

STATIC struct qinit dl_rinit = {
	qi_putp:ss7_iput,		/* Read put (msg from below) */
	qi_srvp:ss7_isrv,		/* Read queue service */
	qi_minfo:&dl_rinfo,		/* Information */
};

STATIC struct qinit dl_winit = {
	qi_putp:ss7_oput,		/* Write put (msg from above) */
	qi_srvp:ss7_osrv,		/* Write queue service */
	qi_minfo:&dl_winfo,		/* Information */
};

STATIC struct streamtab isdn_info = {
	st_rdinit:&isdn_rinit,		/* Upper read queue */
	st_wrinit:&isdn_winit,		/* Upper write queue */
	st_muxrinit:&dl_rinit,		/* Lower read queue */
	st_muxwinit:&dl_winit,		/* Lower write queue */
};

#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
#define QR_STRIP	7
#define QR_RETRY	8

/*
 *  =========================================================================
 *
 *  ISDN Private Structure
 *
 *  =========================================================================
 */

struct cc;				/* isdn user structure */
struct cr;				/* isdn call reference */
struct ch;				/* isdn channel structure */
struct tg;				/* isdn transmission group structure */
struct fg;				/* isdn facility group structure */
struct eg;				/* isdn equipment group structure */
struct xg;				/* isdn exchange group structure */
struct dl;				/* isdn data link structure */

typedef struct cc_bind {
	struct cc *mgm;			/* mgm user bound */
	struct cc *mnt;			/* mnt user bound */
	struct cc *xry;			/* xry user bound */
	struct cc *icc;			/* icc user bound */
	struct cc *ogc;			/* ogc user bound */
} cc_bind_t;

#define ISDN_BIND_NONE	0
#define ISDN_BIND_CH	1
#define ISDN_BIND_TG	2
#define ISDN_BIND_FG	3
#define ISDN_BIND_EG	4
#define ISDN_BIND_XG	5
#define ISDN_BIND_DL	6
#define ISDN_BIND_DF	7

#define cc_t isdn_t
/*
   isdn user (call control) 
 */
typedef struct cc {
	STR_DECLARATION (struct cc);	/* streams declaration */
	struct {
		int type;		/* type of bind */
		union {
			struct df *df;	/* this CC bound to this default */
			struct ch *ch;	/* this CC bound to this channel */
			struct tg *tg;	/* this CC bound to this transmission group */
			struct fg *fg;	/* this CC bound to this facility group */
			struct eg *eg;	/* this CC bound to this equipment group */
			struct xg *xg;	/* this CC bound to this exchange group */
			struct dc *dc;	/* this CC bound to this d channel */
			struct dl *dl;	/* this CC bound to this data link */
		} u;
		struct cc *next;	/* next CC bound to this entity */
		struct cc **prev;	/* prev CC bound to this entity */
	} bind;
	struct {
		struct cr *cpc;		/* call references engaged in call */
		struct {
			struct cr *cr;	/* call reference engaged in management */
			struct ch *ch;	/* channel engaged in management */
			struct tg *tg;	/* transmission group engaged in management */
			struct fg *fg;	/* facility group engaged in management */
			struct eg *eg;	/* equipment group engaged in management */
			struct xg *xg;	/* exchange group engaged in management */
		} mgm;
	} conn;
	uint maxind;			/* max number of setup indications */
	uint setind;			/* cur number of setup indications */
	uint maxreq;			/* max number of setup requests */
	uint setreq;			/* cur number of setup requests */
} cc_t;
#define CC_PRIV(__q) ((struct cc *)(__q)->q_ptr)

STATIC struct cc *isdn_alloc_cc(queue_t *, struct cc **, dev_t *, cred_t *);
STATIC void isdn_free_cc(queue_t *);
STATIC struct cc *cc_get(struct cc *);
STATIC void cc_put(struct cc *);

/*
 *  Call Reference
 *  -----------------------------------
 *  A call reference is a reference to a call independent of the B-channel that the call corresponds to.  This is to
 *  support network suspend/resume where the call is detached from the B-channel and then associated with a new
 *  B-channel when it is resumed.
 */
typedef struct cr {
	HEAD_DECLARATION (struct cr);	/* head declaration */
	mblk_t *retry;			/* retry message */
	ulong cref;			/* call reference (provider) */
	ulong uref;			/* call reference (user) */
	SLIST_HEAD (ch, ch);		/* channels used by the call */
	struct dl *dl;			/* current data link for call */
	ulong c_state;			/* call state */
	ulong i_state;			/* cpc interface state */
	ulong m_state;			/* mgm interface state */
	SLIST_LINKAGE (cc, cr, cpc);	/* call control engaged in call */
	SLIST_LINKAGE (cc, cr, mgm);	/* call control engaged in management */
	SLIST_LINKAGE (fg, cr, fg);	/* active call references for facility group */
	lmi_notify_t notify;		/* call notifications */
	isdn_statem_cr_t statem;	/* call state */
	isdn_timers_cr_t timers;	/* call timers */
	isdn_opt_conf_cr_t config;	/* call configuration */
	isdn_stats_cr_t stats;		/* call statistics */
	isdn_stats_cr_t statsp;		/* call statistics periods */
} cr_t;

STATIC struct cr *isdn_alloc_cr(ulong, struct cc *, struct fg *, struct ch **, size_t);
STATIC void isdn_free_cr(struct cr *);
STATIC struct cr *cr_get(struct cr *);
STATIC void cr_put(struct cr *);
STATIC struct cr *cr_lookup(ulong, struct fg *);
STATIC ulong cr_get_id(ulong, struct fg *);

/*
 *  Channel (B-Channel)
 *  -----------------------------------
 *  Each channel structure corresponds to a single B-Channel for which call processing is being performed.
 *
 *  Each B-Channel can be engaged in a call with a Call Control user, can be engaged in b-channel management with a
 *  Management user.  Each channel belongs to a Channel (B-Channel) Group and a Signalling (D-Channel) Group.
 */
typedef struct ch {
	HEAD_DECLARATION (struct ch);	/* head declaration */
	struct cc_bind bind;		/* bindings */
	mblk_t *retry;			/* retry message */
	ulong ts;			/* timeslot */
	ulong ci;			/* channel identification */
	ulong c_state;			/* call processing state */
	SLIST_LINKAGE (cr, ch, cr);	/* call reference list linkage */
	SLIST_LINKAGE (tg, ch, tg);	/* transmission group list linkage */
	SLIST_LINKAGE (fg, ch, fg);	/* facility group list linkage */
	SLIST_LINKAGE (fg, ch, idle);	/* facility group idle list linkage */
	lmi_notify_t notify;		/* channel notifications */
	isdn_statem_ch_t statem;	/* channel state */
	isdn_timers_ch_t timers;	/* channel timers */
	isdn_opt_conf_ch_t config;	/* channel configuration */
	isdn_stats_ch_t stats;		/* channel statistics */
	isdn_stats_ch_t statsp;		/* channel statistics periods */
} ch_t;

STATIC struct ch *isdn_alloc_ch(ulong, struct fg *, struct tg *, ulong);
STATIC void isdn_free_ch(struct ch *);
STATIC struct ch *ch_get(struct ch *);
STATIC void ch_put(struct ch *);
STATIC struct ch *ch_lookup(ulong);
STATIC ulong ch_get_id(ulong);

/*
 *  Transmission Group
 *  -----------------------------------
 *  A transmission group is a group of B-channels and D-channels that share the same transmission facility (i.e. are
 *  time-slots in the same PRI interface).
 *
 *  Each transmission group consists of a list of channels and data links that use the transmission facility.
 *
 *  Each Transmission Group can be engaged in transmission group management with a Management user.  Each
 *  transmission group belongs to a remote equipment group.
 */
typedef struct tg {
	HEAD_DECLARATION (struct tg);	/* head declaration */
	struct cc_bind bind;		/* bindings */
	ulong g_state;			/* mgm interface state */
	SLIST_HEAD (dc, dc);		/* D-channel list */
	SLIST_HEAD (ch, ch);		/* B-channel list */
	SLIST_LINKAGE (cc, tg, mgm);	/* call control list linkage (CC engaged in management) */
	SLIST_LINKAGE (eg, tg, eg);	/* equipment group list linkage */
	lmi_option_t proto;		/* transmission group options */
	lmi_notify_t notify;		/* transmission group notifications */
	isdn_statem_tg_t statem;	/* transmission group state */
	isdn_timers_tg_t timers;	/* transmission group timers */
	isdn_opt_conf_tg_t config;	/* transmission group configuration */
	isdn_stats_tg_t stats;		/* transmission group statistics */
	isdn_stats_tg_t statsp;		/* transmission group statistics periods */
} tg_t;

STATIC struct tg *isdn_alloc_tg(ulong, struct eg *);
STATIC void isdn_free_tg(struct tg *);
STATIC struct tg *tg_get(struct tg *);
STATIC void tg_put(struct tg *);
STATIC struct tg *tg_lookup(ulong);
STATIC ulong tg_get_id(ulong);

/*
 *  Facility Group
 *  -----------------------------------
 *  A facility group is a group of B-channels and a group of D-channels which provide signalling for the B-channels.
 *  Typically there is only one D-channel in a facility group.  Where multiple D-channels are used as primary-backup
 *  or using some other redundancy scheme, the facility group contains more than one D-channel.
 *
 *  Each facility group consists of a list of data links (D-channels) and a list of B-channels that are controlled
 *  by D-channels in the facility group.
 *
 *  Each facility group can be engaged in facility group management with a Management user.  Each facility group
 *  belongs to an equipment group.
 */
typedef struct fg {
	HEAD_DECLARATION (struct fg);	/* head declaration */
	struct cc_bind bind;		/* bindings */
	SLIST_HEAD (dc, dc);		/* D-channel list */
	SLIST_HEAD (ch, ch);		/* B-channel list */
	SLIST_HEAD (ch, idle);		/* B-channel idle list */
	SLIST_HEAD (cr, cr);		/* call reference list */
	ulong g_state;			/* mgm interface state */
	SLIST_LINKAGE (cc, fg, mgm);	/* call control list linkage (CC engaged in management) */
	SLIST_LINKAGE (cc, fg, xry);	/* call control list linkage (CC engaged in monitoring) */
	SLIST_LINKAGE (eg, fg, eg);	/* network/terminal equipment list linkage */
	lmi_option_t proto;		/* facility group protocol options */
	lmi_notify_t notify;		/* facility group notifications */
	isdn_statem_fg_t statem;	/* facility group state */
	isdn_timers_fg_t timers;	/* facility group timers */
	isdn_opt_conf_fg_t config;	/* facility group configuration */
	isdn_stats_fg_t stats;		/* facility group statistics */
	isdn_stats_fg_t statsp;		/* facility group statistics periods */
} fg_t;

STATIC struct fg *isdn_alloc_fg(ulong, struct eg *);
STATIC void isdn_free_fg(struct fg *);
STATIC struct fg *fg_get(struct fg *);
STATIC void fg_put(struct fg *);
STATIC struct fg *fg_lookup(ulong);
STATIC ulong fg_get_id(ulong);

/*
 *  Equipment Group
 *  -------------------------------------
 *  An equipment group is a group of transmission groups and facility groups attached to the same network switching
 *  equipment (for User mode interfaces) or attached to the same customer premises equipment (Network mode
 *  ineterfaces).
 *
 *  Each equipment group is a group of transmission and facility grups all attached to the same remote equipment.
 */
typedef struct eg {
	HEAD_DECLARATION (struct eg);	/* head declaration */
	struct cc_bind bind;		/* bindings */
	ulong g_state;			/* group state */
	SLIST_HEAD (fg, fg);		/* facility group list */
	SLIST_HEAD (tg, tg);		/* transmission group list */
	SLIST_LINKAGE (cc, eg, mgm);	/* call control list linkage (CC engaged in management) */
	SLIST_LINKAGE (xg, eg, xg);	/* exchange group list linkage */
	lmi_option_t proto;		/* equipment group options */
	lmi_notify_t notify;		/* equipment group notifications */
	isdn_statem_eg_t statem;	/* equipment group state */
	isdn_timers_eg_t timers;	/* equipment group timers */
	isdn_opt_conf_eg_t config;	/* equipment group configuration */
	isdn_stats_eg_t stats;		/* equipment group statistics */
	isdn_stats_eg_t statsp;		/* equipment group statistics periods */
} eg_t;

STATIC struct eg *isdn_alloc_eg(ulong, struct xg *);
STATIC void isdn_free_eg(struct eg *);
STATIC struct eg *eg_get(struct eg *);
STATIC void eg_put(struct eg *);
STATIC struct eg *eg_lookup(ulong);
STATIC ulong eg_get_id(ulong);

/*
 *  Exchange Group
 *  -----------------------------------
 *  A exchange group is a group of equipment groups attached to the same provider (for User mode
 *  interfaces) or customer (for Network mode interfaces).
 *
 *  Each exchange group is a group of equipment groups.
 */
typedef struct xg {
	HEAD_DECLARATION (struct xg);	/* head declaration */
	struct cc_bind bind;		/* bindings */
	ulong g_state;			/* grp state */
	SLIST_HEAD (eg, eg);		/* network/terminal equipment list */
	SLIST_LINKAGE (cc, xg, mgm);	/* call control list linkage (CC engaged in management) */
	lmi_option_t proto;		/* exchange group options */
	lmi_notify_t notify;		/* exchange group notifications */
	isdn_statem_xg_t statem;	/* exchange group state */
	isdn_timers_xg_t timers;	/* exchange group timers */
	isdn_opt_conf_xg_t config;	/* exchange group configuration */
	isdn_stats_xg_t stats;		/* exchange group statistics */
	isdn_stats_xg_t statsp;		/* exchange group statistics periods */
} xg_t;

STATIC struct xg *isdn_alloc_xg(ulong);
STATIC void isdn_free_xg(struct xg *);
STATIC struct xg *xg_get(struct xg *);
STATIC void xg_put(struct xg *);
STATIC struct xg *xg_lookup(ulong);
STATIC ulong xg_get_id(ulong);

/*
 *  D-Channel
 *  -----------------------------------
 *  A D-Channel represents a physical interface.  Multiple Data Link connection can be formed over a single physical
 *  interface.  This is particularly true for exchange side equipment.  The D-channel is characterized by the PPA
 *  (Physical Point of Attachment) and the SAPI (Service Access Point Identifier).  This interface can support
 *  multiple data links (DL) each established to a remote TEI (Terminal Equipment Identifier).
 *
 *  Each D-channel can be engaged in D-channel management with a Mangement user and can be monitored by a Monitor
 *  user.  Each D-channel belongs to a facility group and a transmission group.
 */
typedef struct dc {
	HEAD_DECLARATION (struct dc);	/* head declaration */
	struct cc_bind bind;		/* bindings */
	ulong ts;			/* timeslot */
	struct dl_addr sap;		/* data link address (PPA and SAPI) */
	SLIST_LINKAGE (cc, dc, mgm);	/* call control list linkage (CC engaged in management) */
	SLIST_LINKAGE (cc, dc, xry);	/* call control list linkage (CC engaged in monitoring) */
	SLIST_LINKAGE (tg, dc, tg);	/* transmission group */
	SLIST_LINKAGE (fg, dc, fg);	/* facility group */
	SLIST_HEAD (dl, dl);		/* data links */
	lmi_option_t proto;		/* D channel options */
	lmi_notify_t notify;		/* D channel notifications */
	isdn_statem_dc_t statem;	/* D channel state */
	isdn_timers_dc_t timers;	/* D channel timers */
	isdn_opt_conf_dc_t config;	/* D channel configuration */
	isdn_stats_dc_t stats;		/* D channel statistics */
	isdn_stats_dc_t statsp;		/* D channel statistics periods */
} dc_t;

STATIC struct dc *isdn_alloc_dc(ulong, struct fg *, struct tg *);
STATIC void isdn_free_dc(struct dc *);
STATIC struct dc *dc_get(struct dc *);
STATIC void dc_put(struct dc *);
STATIC struct dc *dc_lookup(ulong);
STATIC ulong dc_get_id(ulong);

/*
 *  Data Link (DL)
 *  -----------------------------------
 *  A Data Link (DL) represents a virtual connection over a physical D-Channel.  Each data link corresponds to one
 *  data link connection for signalling.  Multiple data links may control signalling for a facility group.
 *
 *  Each data link can be engaged in data link management with a Management user and can be monitored by a Monitor
 *  user.  Each Data Link (DL) belongs to a D-channel.
 */
typedef struct dl {
	STR_DECLARATION (struct dl);	/* stream declaration */
	struct cc_bind bind;		/* bindings */
	ulong tok;			/* token for stream */
	ulong ppa;			/* PPA */
	struct dl_addr dlc;		/* data link address (SAPI and TEI) */
	ulong conind;			/* connection indications */
	SLIST_LINKAGE (cc, dl, mgm);	/* call control list linkage (CC engaged in management) */
	SLIST_LINKAGE (cc, dl, xry);	/* call control list linkage (CC engaged in monitoring) */
	SLIST_LINKAGE (dc, dl, dc);	/* D channel linkage */
	dl_info_ack_t info;		/* data link information */
	lmi_option_t proto;		/* data link options */
	lmi_notify_t notify;		/* data link notifications */
	isdn_statem_dl_t statem;	/* data linke state */
	isdn_timers_dl_t timers;	/* data link timers */
	isdn_opt_conf_dl_t config;	/* data link configuration */
	isdn_stats_dl_t stats;		/* data link statistics */
	isdn_stats_dl_t statsp;		/* data link statistics periods */
} dl_t;
#define DL_PRIV(__q) ((struct dl *)(__q)->q_ptr)

STATIC struct dl *isdn_alloc_dl(queue_t *, struct dl **, ulong, cred_t *);
STATIC void isdn_free_dl(queue_t *);
STATIC void isdn_link_dl(ulong, struct dl *, struct dc *);
STATIC void isdn_unlink_dl(struct dl *);
STATIC struct dl *dl_get(struct dl *);
STATIC void dl_put(struct dl *);
STATIC struct dl *dl_lookup(ulong);
STATIC ulong dl_get_id(ulong);

/*
   default 
 */
typedef struct df {
	spinlock_t lock;		/* structure lock */
	struct cc_bind bind;		/* bindings */
	SLIST_HEAD (cc, cc);		/* master list of call control users */
	SLIST_HEAD (cr, cr);		/* master list of call references */
	SLIST_HEAD (ch, ch);		/* master list of channels */
	SLIST_HEAD (tg, tg);		/* master list of transmission groups */
	SLIST_HEAD (fg, fg);		/* master list of facility groups */
	SLIST_HEAD (eg, eg);		/* master list of equipment groups */
	SLIST_HEAD (xg, xg);		/* master list of exchange groups */
	SLIST_HEAD (dc, dc);		/* master list of D channels */
	SLIST_HEAD (dl, dl);		/* master list of data links */
	lmi_option_t proto;		/* default protocol options */
	lmi_notify_t notify;		/* default notifications */
	isdn_opt_conf_df_t config;	/* default configuration */
	isdn_stats_df_t stats;		/* default statistics */
	isdn_stats_df_t statsp;		/* default statistics periods */
} df_t;
STATIC struct df master;

/*
 *  ========================================================================
 *
 *  STATE Changes
 *
 *  ========================================================================
 */
STATIC INLINE ulong
cc_set(struct cc *cc, const ulong flags)
{
	cc->flags |= flags;
	return (cc->flags);
}
STATIC INLINE ulong
cc_clr(struct cc *cc, const ulong flags)
{
	cc->flags &= ~flags;
	return (cc->flags);
}
STATIC INLINE ulong
cc_tst(struct cc *cc, const ulong flags)
{
	return (cc->flags & flags);
}

#ifdef _DEBUG
STATIC const char *
cc_state_name(long state)
{
	switch (state) {
	case CCS_IDLE:
		return ("CCS_IDLE");
	case CCS_SUSPENDED:
		return ("CCS_SUSPENDED");
	case CCS_UNBND:
		return ("CCS_UNBND");
	case CCS_UNUSABLE:
		return ("CCS_UNUSABLE");
	case CCS_WCON_RELREQ:
		return ("CCS_WCON_RELREQ");
	case CCS_WCON_SREQ:
		return ("CCS_WCON_SREQ");
	case CCS_WIND_ALERTING:
		return ("CCS_WIND_ALERTING");
	case CCS_WIND_CCREP:
		return ("CCS_WIND_CCREP");
	case CCS_WIND_CONNECT:
		return ("CCS_WIND_CONNECT");
	case CCS_WIND_IBI:
		return ("CCS_WIND_IBI");
	case CCS_WIND_INFO:
		return ("CCS_WIND_INFO");
	case CCS_WIND_MORE:
		return ("CCS_WIND_MORE");
	case CCS_WIND_PROCEED:
		return ("CCS_WIND_PROCEED");
	case CCS_WIND_PROGRESS:
		return ("CCS_WIND_PROGRESS");
	case CCS_WIND_SETUP:
		return ("CCS_WIND_SETUP");
	case CCS_WREQ_ALERTING:
		return ("CCS_WREQ_ALERTING");
	case CCS_WREQ_CCREP:
		return ("CCS_WREQ_CCREP");
	case CCS_WREQ_CONNECT:
		return ("CCS_WREQ_CONNECT");
	case CCS_WREQ_IBI:
		return ("CCS_WREQ_IBI");
	case CCS_WREQ_MORE:
		return ("CCS_WREQ_MORE");
	case CCS_WREQ_PROCEED:
		return ("CCS_WREQ_PROCEED");
	case CCS_WREQ_PROGRESS:
		return ("CCS_WREQ_PROGRESS");
	case CCS_WREQ_SETUP:
		return ("CCS_WREQ_SETUP");
	case CCS_WRES_RELIND:
		return ("CCS_WRES_RELIND");
	case CCS_WRES_SIND:
		return ("CCS_WRES_SIND");
	default:
		return ("(unknown)");
	}
}
STATIC const char *
cp_state_name(long state)
{
	switch (state) {
	case U0_NULL:
		return ("U0_NULL");
	case U10_ACTIVE:
		return ("U10_ACTIVE");
	case U11_DISCONNECT_REQUEST:
		return ("U11_DISCONNECT_REQUEST");
	case U12_DISCONNECT_INDICATION:
		return ("U12_DISCONNECT_INDICATION");
	case U15_SUSPEND_REQUEST:
		return ("U15_SUSPEND_REQUEST");
	case U17_RESUME_REQUEST:
		return ("U17_RESUME_REQUEST");
	case U19_RELEASE_REQUEST:
		return ("U19_RELEASE_REQUEST");
	case U1_CALL_INITIATED:
		return ("U1_CALL_INITIATED");
	case U25_OVERLAP_RECEIVING:
		return ("U25_OVERLAP_RECEIVING");
	case U2_OVERLAP_SENDING:
		return ("U2_OVERLAP_SENDING");
	case U3_OUTGOING_CALL_PROCEEDING:
		return ("U3_OUTGOING_CALL_PROCEEDING");
	case U4_CALL_DELIVERED:
		return ("U4_CALL_DELIVERED");
	case U6_CALL_PRESENT:
		return ("U6_CALL_PRESENT");
	case U7_CALL_RECEIVED:
		return ("U7_CALL_RECEIVED");
	case U8_CONNECT_REQUEST:
		return ("U8_CONNECT_REQUEST");
	case U9_INCOMING_CALL_PROCEEDING:
		return ("U9_INCOMING_CALL_PROCEEDING");
	case N0_NULL:
		return ("N0_NULL");
	case N10_ACTIVE:
		return ("N10_ACTIVE");
	case N11_DISCONNECT_REQUEST:
		return ("N11_DISCONNECT_REQUEST");
	case N12_DISCONNECT_INDICATION:
		return ("N12_DISCONNECT_INDICATION");
	case N15_SUSPEND_REQUEST:
		return ("N15_SUSPEND_REQUEST");
	case N17_RESUME_REQUEST:
		return ("N17_RESUME_REQUEST");
	case N19_RELEASE_REQUEST:
		return ("N19_RELEASE_REQUEST");
	case N1_CALL_INITIATED:
		return ("N1_CALL_INITIATED");
	case N22_CALL_ABORT:
		return ("N22_CALL_ABORT");
	case N25_OVERLAP_RECEIVING:
		return ("N25_OVERLAP_RECEIVING");
	case N2_OVERLAP_SENDING:
		return ("N2_OVERLAP_SENDING");
	case N3_OUTGOING_CALL_PROCEEDING:
		return ("N3_OUTGOING_CALL_PROCEEDING");
	case N4_CALL_DELIVERED:
		return ("N4_CALL_DELIVERED");
	case N6_CALL_PRESENT:
		return ("N6_CALL_PRESENT");
	case N7_CALL_RECEIVED:
		return ("N7_CALL_RECEIVED");
	case N8_CONNECT_REQUEST:
		return ("N8_CONNECT_REQUEST");
	case N9_INCOMING_CALL_PROCEEDING:
		return ("N9_INCOMING_CALL_PROCEEDING");
	default:
		return ("(unknown)");
	}
}
STATIC const char *
cm_state_name(long state)
{
	switch (state) {
	case CMS_IDLE:
		return ("CMS_IDLE");
	default:
		return ("(unknown)");
	}
}
STATIC const char *
cr_flag_name(ulong flag)
{
	switch (flag) {
	default:
		return ("(unknown)");
	}
}
STATIC const char *
tg_flag_name(ulong flag)
{
	switch (flag) {
	default:
		return ("(unknown)");
	}
}
STATIC const char *
fg_flag_name(ulong flag)
{
	switch (flag) {
	default:
		return ("(unknown)");
	}
}
STATIC const char *
eg_flag_name(ulong flag)
{
	switch (flag) {
	default:
		return ("(unknown)");
	}
}
STATIC const char *
xg_flag_name(ulong flag)
{
	switch (flag) {
	default:
		return ("(unknown)");
	}
}
#endif

/*
 *  State Handling
 *  -----------------------------------
 *  State handling for call control, channels, transmission groups, facility groups, equipment groups, exchange
 *  groups and data links.
 */
STATIC INLINE long
cc_get_state(struct cc *cc)
{
	return (cc->state);
}
STATIC INLINE long
cr_get_c_state(struct cr *cr)
{
	return (cr->c_state);
}
STATIC INLINE long
cr_get_i_state(struct cr *cr)
{
	return (cr->i_state);
}
STATIC INLINE long
cr_get_m_state(struct cr *cr)
{
	return (cr->m_state);
}
STATIC INLINE long
fg_get_state(struct fg *fg)
{
	return (fg->state);
}
STATIC INLINE long
fg_get_m_state(struct fg *fg)
{
	return (fg->g_state);
}
STATIC INLINE long
tg_get_state(struct tg *tg)
{
	return (tg->state);
}
STATIC INLINE long
tg_get_m_state(struct tg *tg)
{
	return (tg->g_state);
}
STATIC INLINE long
eg_get_state(struct eg *eg)
{
	return (eg->state);
}
STATIC INLINE long
eg_get_m_state(struct eg *eg)
{
	return (eg->g_state);
}
STATIC INLINE long
xg_get_state(struct xg *xg)
{
	return (xg->state);
}
STATIC INLINE long
xg_get_m_state(struct xg *xg)
{
	return (xg->g_state);
}
STATIC INLINE long
dl_get_state(struct dl *dl)
{
	return (dl->i_state);
}

STATIC INLINE void
cc_set_state(struct cc *cc, const long newstate)
{
	long oldstate = cc_get_state(cc);
	(void) oldstate;
	printd(("%s: %p: %d:%d cc-state %s <- %s\n", DRV_NAME, cc, cc->u.dev.cmajor,
		cc->u.dev.cminor, cc_state_name(newstate), cc_state_name(oldstate)));
	cc->state = newstate;
}
STATIC INLINE void
tg_set_state(struct tg *tg, const long newstate)
{
	long oldstate = tg_get_state(tg);
	(void) oldstate;
	printd(("%s: %p: tg %2ld tg-state %s <- %s\n", DRV_NAME, tg, tg->id,
		cp_state_name(newstate), cp_state_name(oldstate)));
	tg->state = newstate;
}
STATIC INLINE void
fg_set_state(struct fg *fg, const long newstate)
{
	long oldstate = fg_get_state(fg);
	(void) oldstate;
	printd(("%s: %p: fg %2ld fg-state %s <- %s\n", DRV_NAME, fg, fg->id,
		cp_state_name(newstate), cp_state_name(oldstate)));
	fg->state = newstate;
}
STATIC INLINE void
eg_set_state(struct eg *eg, const long newstate)
{
	long oldstate = eg_get_state(eg);
	(void) oldstate;
	printd(("%s: %p: eg %2ld eg-state %s <- %s\n", DRV_NAME, eg, eg->id,
		cp_state_name(newstate), cp_state_name(oldstate)));
	eg->state = newstate;
}
STATIC INLINE void
xg_set_state(struct xg *xg, const long newstate)
{
	long oldstate = xg_get_state(xg);
	(void) oldstate;
	printd(("%s: %p: xg %2ld xg-state %s <- %s\n", DRV_NAME, xg, xg->id,
		cp_state_name(newstate), cp_state_name(oldstate)));
	xg->state = newstate;
}
STATIC INLINE void
dl_set_state(struct dl *dl, const long newstate)
{
	dl->i_state = newstate;
}

/*
 *  Call Reference and Token Handling
 *  -----------------------------------
 *  Call reference and token handling for call control.
 */
STATIC INLINE ulong
cc_get_uref(struct cr *cr, struct ch *ch, struct cc *cc, ulong uref)
{
	if (!cr->uref) {
		struct cr *c2;
		for (c2 = cc->conn.cpc; c2 && c2->uref != uref; c2 = c2->cpc.next) ;
		if (c2)
			return (0);
		cr->uref = uref;
		cc->setreq++;
		return (cr->uref);
	} else
		swerr();
	return (0);
}
STATIC INLINE void
cc_put_uref(struct cr *cr, struct cc *cc)
{
	if (cr_get_i_state(cr) != CCS_WCON_SREQ) {
		assure(cr->uref);
		cr->uref = 0;
		assure(cc->setreq > 0);
		--cc->setreq;
	} else
		swerr();
}
STATIC INLINE struct cr *
cc_find_cpc_uref(struct cc *cc, ulong uref)
{
	struct cr *cr = NULL;
	if (uref)
		for (cr = cc->conn.cpc; cr && cr->uref != uref; cr = cr->cpc.next) ;
	if (cr && cr->cpc.cc != cc) {
		swerr();
		return NULL;
	}
	return (cr);
}

STATIC INLINE ulong
cc_get_cref(struct cr *cr, struct cc *cc)
{
	static ulong reference = 0;
	if (cr_get_m_state(cr) == CMS_IDLE && cr_get_i_state(cr) == CCS_IDLE) {
		assure(!cr->cref);
		if (!cr->cref) {
			if (++reference == 0L)
				reference = 1L;
			cr->cref = reference;
		}
	}
	if (!(cc->setind++))
		cc_set_state(cc, CCS_WRES_SIND);
	return (cr->cref);
}
STATIC INLINE struct cr *
cc_find_cpc_cref(struct cc *cc, ulong cref)
{
	struct cr *cr = NULL;
	if (cref)
		for (cr = cc->conn.cpc; cr && cr->cref != cref; cr = cr->cpc.next) ;
	if (cr && cr->cpc.cc != cc) {
		swerr();
		return NULL;
	}
	return (cr);
}
STATIC INLINE struct cc *
cc_find_tok(struct cc *cc, ulong tok)
{
	struct cc *ac = cc;
	if (tok)
		for (ac = master.cc.list; ac && (ulong) ac->oq != tok; ac = ac->next) ;
	return (ac);
}
STATIC INLINE void
cc_put_cref(struct cr *cr, struct cc *cc)
{
	if (cr_get_m_state(cr) == CMS_IDLE && cr_get_i_state(cr) == CCS_IDLE) {
		assure(cr->cref);
		cr->cref = 0;
	}
	assure(cc->setind > 0);
	if (!(--cc->setind))
		cc_set_state(cc, CCS_IDLE);
}

/*
 *  Management State Handling
 *  -----------------------------------
 *  Management state handling for channels, transmission groups, facility groups, equipment groups and exchange
 *  groups.
 */
STATIC INLINE void
cr_set_m_state(struct cr *cr, struct cc *cc, const long newstate)
{
	if (cr && cc) {
		long oldstate = cr_get_m_state(cr);
		printd(("%s: %p: cr %2ld cc %d:%d m-state %s <- %s\n", DRV_NAME, cr, cr->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, cm_state_name(newstate),
			cm_state_name(oldstate)));
		if (oldstate == CMS_IDLE && newstate != CMS_IDLE) {
			/* 
			   make cc current */
			cr->mgm.cc = cc_get(cc);
			if ((cr->mgm.next = cc->conn.mgm.cr))
				cr->mgm.next->mgm.prev = &cr->mgm.next;
			cr->mgm.prev = &cc->conn.mgm.cr;
			cc->conn.mgm.cr = cr_get(cr);
		} else if (oldstate != CMS_IDLE && newstate == CMS_IDLE) {
			/* 
			   detach from current */
			cr_put(xchg(&cc->conn.mgm.cr, NULL));
			if ((*cr->mgm.prev = cr->mgm.next))
				cr->mgm.next->mgm.prev = cr->mgm.prev;
			cr->mgm.next = NULL;
			cr->mgm.prev = &cr->mgm.next;
			cc_put(xchg(&cr->mgm.cc, NULL));
		}
		cr->m_state = newstate;
		return;
	}
	swerr();
}
STATIC INLINE void
tg_set_m_state(struct tg *tg, struct cc *cc, const long newstate)
{
	if (tg && cc) {
		long oldstate = tg_get_m_state(tg);
		printd(("%s: %p: tg %2ld cc %d:%d m-state %s <- %s\n", DRV_NAME, tg, tg->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, cm_state_name(newstate),
			cm_state_name(oldstate)));
		if (oldstate == CMS_IDLE && newstate != CMS_IDLE) {
			/* 
			   make cc current */
			tg->mgm.cc = cc_get(cc);
			if ((tg->mgm.next = cc->conn.mgm.tg))
				tg->mgm.next->mgm.prev = &tg->mgm.next;
			tg->mgm.prev = &cc->conn.mgm.tg;
			cc->conn.mgm.tg = tg_get(tg);
		} else if (oldstate != CMS_IDLE && newstate == CMS_IDLE) {
			/* 
			   detach from current */
			tg_put(xchg(&cc->conn.mgm.tg, NULL));
			if ((*tg->mgm.prev = tg->mgm.next))
				tg->mgm.next->mgm.prev = tg->mgm.prev;
			tg->mgm.next = NULL;
			tg->mgm.prev = &tg->mgm.next;
			cc_put(xchg(&tg->mgm.cc, NULL));
		}
		tg->g_state = newstate;
		return;
	}
	swerr();
}
STATIC INLINE void
fg_set_m_state(struct fg *fg, struct cc *cc, const long newstate)
{
	if (fg && cc) {
		long oldstate = fg_get_m_state(fg);
		printd(("%s: %p: fg %2ld cc %d:%d m-state %s <- %s\n", DRV_NAME, fg, fg->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, cm_state_name(newstate),
			cm_state_name(oldstate)));
		if (oldstate == CMS_IDLE && newstate != CMS_IDLE) {
			/* 
			   make cc current */
			fg->mgm.cc = cc_get(cc);
			if ((fg->mgm.next = cc->conn.mgm.fg))
				fg->mgm.next->mgm.prev = &fg->mgm.next;
			fg->mgm.prev = &cc->conn.mgm.fg;
			cc->conn.mgm.fg = fg_get(fg);
		} else if (oldstate != CMS_IDLE && newstate == CMS_IDLE) {
			/* 
			   detafg from current */
			fg_put(xchg(&cc->conn.mgm.fg, NULL));
			if ((*fg->mgm.prev = fg->mgm.next))
				fg->mgm.next->mgm.prev = fg->mgm.prev;
			fg->mgm.next = NULL;
			fg->mgm.prev = &fg->mgm.next;
			cc_put(xchg(&fg->mgm.cc, NULL));
		}
		fg->g_state = newstate;
		return;
	}
	swerr();
}
STATIC INLINE void
eg_set_m_state(struct eg *eg, struct cc *cc, const long newstate)
{
	if (eg && cc) {
		long oldstate = eg_get_m_state(eg);
		printd(("%s: %p: eg %2ld cc %d:%d m-state %s <- %s\n", DRV_NAME, eg, eg->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, cm_state_name(newstate),
			cm_state_name(oldstate)));
		if (oldstate == CMS_IDLE && newstate != CMS_IDLE) {
			/* 
			   make cc current */
			eg->mgm.cc = cc_get(cc);
			if ((eg->mgm.next = cc->conn.mgm.eg))
				eg->mgm.next->mgm.prev = &eg->mgm.next;
			eg->mgm.prev = &cc->conn.mgm.eg;
			cc->conn.mgm.eg = eg_get(eg);
		} else if (oldstate != CMS_IDLE && newstate == CMS_IDLE) {
			/* 
			   detaeg from current */
			eg_put(xchg(&cc->conn.mgm.eg, NULL));
			if ((*eg->mgm.prev = eg->mgm.next))
				eg->mgm.next->mgm.prev = eg->mgm.prev;
			eg->mgm.next = NULL;
			eg->mgm.prev = &eg->mgm.next;
			cc_put(xchg(&eg->mgm.cc, NULL));
		}
		eg->g_state = newstate;
		return;
	}
	swerr();
}
STATIC INLINE void
xg_set_m_state(struct xg *xg, struct cc *cc, const long newstate)
{
	if (xg && cc) {
		long oldstate = xg_get_m_state(xg);
		printd(("%s: %p: xg %2ld cc %d:%d m-state %s <- %s\n", DRV_NAME, xg, xg->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, cm_state_name(newstate),
			cm_state_name(oldstate)));
		if (oldstate == CMS_IDLE && newstate != CMS_IDLE) {
			/* 
			   make cc current */
			xg->mgm.cc = cc_get(cc);
			if ((xg->mgm.next = cc->conn.mgm.xg))
				xg->mgm.next->mgm.prev = &xg->mgm.next;
			xg->mgm.prev = &cc->conn.mgm.xg;
			cc->conn.mgm.xg = xg_get(xg);
		} else if (oldstate != CMS_IDLE && newstate == CMS_IDLE) {
			/* 
			   detach from current */
			xg_put(xchg(&cc->conn.mgm.xg, NULL));
			if ((*xg->mgm.prev = xg->mgm.next))
				xg->mgm.next->mgm.prev = xg->mgm.prev;
			xg->mgm.next = NULL;
			xg->mgm.prev = &xg->mgm.next;
			cc_put(xchg(&xg->mgm.cc, NULL));
		}
		xg->g_state = newstate;
		return;
	}
	swerr();
}

STATIC INLINE void
cr_set_i_state(struct cr *cr, struct cc *cc, const long newstate)
{
	if (cr && cc) {
		long oldstate = cr_get_i_state(cr);
		printd(("%s: %p: cr %2ld cc %d:%d i-state %s <- %s\n", DRV_NAME, cr, cr->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, cp_state_name(newstate),
			cp_state_name(oldstate)));
		if (oldstate == CCS_IDLE && newstate != CCS_IDLE) {
			/* 
			   make cc current */
			cc_get_cref(cr, cc);
			cr->cpc.cc = cc_get(cc);
			if ((cr->cpc.next = cc->conn.cpc))
				cr->cpc.next->cpc.prev = &cr->cpc.next;
			cr->cpc.prev = &cc->conn.cpc;
			cc->conn.cpc = cr_get(cr);
		} else if (oldstate != CCS_IDLE && newstate == CCS_IDLE) {
			/* 
			   detach from current */
			cr_put(xchg(&cc->conn.cpc, NULL));
			if ((*cr->cpc.prev = cr->cpc.next))
				cr->cpc.next->cpc.prev = cr->cpc.prev;
			cr->cpc.next = NULL;
			cr->cpc.prev = &cr->cpc.next;
			cc_put(xchg(&cr->cpc.cc, NULL));
			cr->i_state = newstate;
			cc_put_cref(cr, cc);
			if (cr->uref)
				cc_put_uref(cr, cc);
		}
		cr->i_state = newstate;
		return;
	}
	swerr();
}

STATIC INLINE void
cr_swap_cref(struct cr *cr, struct cc *cc)
{
	if (cr && cc) {
		/* 
		   remove from old stream */
		if ((*cr->cpc.prev = cr->cpc.next))
			cr->cpc.next->cpc.prev = cr->cpc.prev;
		cr->cpc.next = NULL;
		cr->cpc.prev = &cr->cpc.next;
		cc_put(xchg(&cr->cpc.cc, NULL));
		/* 
		   add to new stream */
		cr->cpc.cc = cc_get(cc);
		if ((cr->cpc.next = cc->conn.cpc))
			cr->cpc.next->cpc.prev = &cr->cpc.next;
		cr->cpc.prev = &cc->conn.cpc;
		cc->conn.cpc = cr;
		return;
	}
	swerr();
}

STATIC INLINE void
cr_set_c_state(struct cr *cr, const long newstate)
{
	long oldstate = cr_get_c_state(cr);
	printd(("%s: %p: cr %2ld c-state %s <- %s\n", DRV_NAME, cr, cr->id, cp_state_name(newstate),
		cp_state_name(oldstate)));
	if (oldstate == U0_NULL || oldstate == N0_NULL) {
		/* 
		   possiblu remove circuit(s) from idle list */
		/* 
		   has no effect if not on list because prev points to next */
		struct ch *ch;
		for (ch = cr->ch.list; ch; ch = ch->cr.next) {
			/* 
			   remove from idle list */
			if ((*ch->idle.prev = ch->idle.next))
				ch->idle.next->idle.prev = ch->idle.prev;
			ch->idle.next = NULL;
			ch->idle.prev = &ch->idle.next;
		}
	}
	if (newstate == U0_NULL || newstate == N0_NULL) {
		struct ch *ch = NULL, **chp;	/* FIXME */
		/* 
		   add circuit(s) to idle list */
		/* 
		   just insert at head of facility group for now */
		chp = &ch->fg.fg->idle.list;
		for (ch = cr->ch.list; ch; ch = ch->cr.next) {
			/* 
			   insert into idle list */
			if ((ch->idle.next = *chp))
				ch->idle.next->idle.prev = ch->idle.prev;
			ch->idle.prev = chp;
			*chp = ch;
		}
	}
	cr->c_state = newstate;
	return;
}

/*
 *  Flag Handling
 *  -----------------------------------
 *  Flag handling for channels, transmission groups, facility groups, equipment groups and exchange groups.
 */
STATIC INLINE ulong
cr_set(struct cr *cr, const ulong flags)
{
	int i;
	ulong newflags = (cr->flags & flags) ^ flags;
	cr->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: cr %2ld SET %s\n", DRV_NAME, cr, cr->id,
				cr_flag_name(newflags & (0x1UL << i))));
	cr_set_c_state(cr, cr_get_c_state(cr));
	return (cr->flags);
}
STATIC INLINE ulong
cr_clr(struct cr *cr, const ulong flags)
{
	int i;
	ulong newflags = (~cr->flags & flags) ^ flags;
	cr->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: cr %2ld CLR %s\n", DRV_NAME, cr, cr->id,
				cr_flag_name(newflags & (0x1UL << i))));
	cr_set_c_state(cr, cr_get_c_state(cr));
	return (cr->flags);
}
STATIC INLINE ulong
cr_tst(struct cr *cr, const ulong flags)
{
	return (cr->flags & flags);
}

STATIC INLINE ulong
tg_set(struct tg *tg, const ulong flags)
{
	int i;
	ulong newflags = (tg->flags & flags) ^ flags;
	tg->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: tg %2ld SET %s\n", DRV_NAME, tg, tg->id,
				tg_flag_name(newflags & (0x1UL << i))));
	return (tg->flags);
}
STATIC INLINE ulong
tg_clr(struct tg *tg, const ulong flags)
{
	int i;
	ulong newflags = (~tg->flags & flags) ^ flags;
	tg->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: tg %2ld CLR %s\n", DRV_NAME, tg, tg->id,
				tg_flag_name(newflags & (0x1UL << i))));
	return (tg->flags);
}
STATIC INLINE ulong
tg_tst(struct tg *tg, const ulong flags)
{
	return (tg->flags & flags);
}

STATIC INLINE ulong
fg_set(struct fg *fg, const ulong flags)
{
	int i;
	ulong newflags = (fg->flags & flags) ^ flags;
	fg->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: fg %2ld SET %s\n", DRV_NAME, fg, fg->id,
				fg_flag_name(newflags & (0x1UL << i))));
	return (fg->flags);
}
STATIC INLINE ulong
fg_clr(struct fg *fg, const ulong flags)
{
	int i;
	ulong newflags = (~fg->flags & flags) ^ flags;
	fg->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: fg %2ld CLR %s\n", DRV_NAME, fg, fg->id,
				fg_flag_name(newflags & (0x1UL << i))));
	return (fg->flags);
}
STATIC INLINE ulong
fg_tst(struct fg *fg, const ulong flags)
{
	return (fg->flags & flags);
}

STATIC INLINE ulong
eg_set(struct eg *eg, const ulong flags)
{
	int i;
	ulong newflags = (eg->flags & flags) ^ flags;
	eg->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: eg %2ld SET %s\n", DRV_NAME, eg, eg->id,
				eg_flag_name(newflags & (0x1UL << i))));
	return (eg->flags);
}
STATIC INLINE ulong
eg_clr(struct eg *eg, const ulong flags)
{
	int i;
	ulong newflags = (~eg->flags & flags) ^ flags;
	eg->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: eg %2ld CLR %s\n", DRV_NAME, eg, eg->id,
				eg_flag_name(newflags & (0x1UL << i))));
	return (eg->flags);
}
STATIC INLINE ulong
eg_tst(struct eg *eg, const ulong flags)
{
	return (eg->flags & flags);
}

STATIC INLINE ulong
xg_set(struct xg *xg, const ulong flags)
{
	int i;
	ulong newflags = (xg->flags & flags) ^ flags;
	xg->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: xg %2ld SET %s\n", DRV_NAME, xg, xg->id,
				xg_flag_name(newflags & (0x1UL << i))));
	return (xg->flags);
}
STATIC INLINE ulong
xg_clr(struct xg *xg, const ulong flags)
{
	int i;
	ulong newflags = (~xg->flags & flags) ^ flags;
	xg->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: xg %2ld CLR %s\n", DRV_NAME, xg, xg->id,
				xg_flag_name(newflags & (0x1UL << i))));
	return (xg->flags);
}
STATIC INLINE ulong
xg_tst(struct xg *xg, const ulong flags)
{
	return (xg->flags & flags);
}

/*
 *  Lookup Functions
 *  -----------------------------------
 *  Lookup functions for channels, transmission groups and facility groups.
 */
#if 0
STATIC struct ch *
isdn_find_ch(struct cc *cc, unsigned char *add_ptr, size_t add_len)
{
	struct ch *ch = NULL;
	if (add_len) {
		struct isdn_addr add = { 0, };
		if (add_len >= sizeof(add)) {
			/* 
			   can't trust alignment or size */
			bcopy(add_ptr, &add, sizeof(add));
			switch (add.scope) {
			case ISDN_SCOPE_CH:
				if (add.id)
					ch = ch_lookup(add.id);
				else if (cc->bind.type == ISDN_BIND_CH)
					ch = cc->bind.u.ch;
				break;
			case ISDN_SCOPE_TG:
			{
				struct tg *tg = NULL;
				if (add.id)
					tg = tg_lookup(add.id);
				else if (cc->bind.type == ISDN_BIND_TG)
					tg = cc->bind.u.tg;
				if (tg)
					for (ch = tg->ch.list; ch && ch->ci != add.ci;
					     ch = ch->tg.next) ;
				break;
			}
			case ISDN_SCOPE_FG:
			{
				struct fg *fg = NULL;
				if (add.id)
					fg = fg_lookup(add.id);
				else if (cc->bind.type == ISDN_BIND_TG)
					fg = cc->bind.u.fg;
				if (fg)
					for (ch = fg->ch.list; ch && ch->ci != add.ci;
					     ch = ch->fg.next) ;
				break;
			}
			}
		}
	} else if (cc->bind.type == ISDN_BIND_CH)
		ch = cc->bind.u.ch;
	return (ch);
}
#endif

#if 0
STATIC struct tg *
isdn_find_tg(struct cc *cc, unsigned char *add_ptr, size_t add_len)
{
	struct tg *tg = NULL;
	if (add_len) {
		struct isdn_addr add = { 0, };
		if (add_len >= sizeof(add)) {
			/* 
			   can't trust alignment or size */
			bcopy(add_ptr, &add, sizeof(add));
			switch (add.scope) {
			case ISDN_SCOPE_CH:
			{
				struct ch *ch = NULL;
				if (add.id)
					ch = ch_lookup(add.id);
				else if (cc->bind.type == ISDN_BIND_CH)
					ch = cc->bind.u.ch;
				if (ch)
					tg = ch->tg.tg;
				break;
			}
			case ISDN_SCOPE_TG:
				if (add.id)
					tg = tg_lookup(add.id);
				else if (cc->bind.type == ISDN_BIND_TG)
					tg = cc->bind.u.tg;
				break;
			}
		}
	} else
		switch (cc->bind.type) {
		case ISDN_BIND_CH:
			if (cc->bind.u.ch)
				tg = cc->bind.u.ch->tg.tg;
			break;
		case ISDN_BIND_TG:
			tg = cc->bind.u.tg;
			break;
		}
	return (tg);
}
#endif

#if 0
STATIC struct fg *
isdn_find_fg(struct cc *cc, unsigned char *add_ptr, size_t add_len)
{
	struct fg *fg = NULL;
	if (add_len) {
		struct isdn_addr add = { 0, };
		if (add_len >= sizeof(add)) {
			/* 
			   can't trust alignment or size */
			bcopy(add_ptr, &add, sizeof(add));
			switch (add.scope) {
			case ISDN_SCOPE_CH:
			{
				struct ch *ch = NULL;
				if (add.id)
					ch = ch_lookup(add.id);
				else if (cc->bind.type == ISDN_BIND_CH)
					ch = cc->bind.u.ch;
				if (ch)
					fg = ch->fg.fg;
				break;
			}
			case ISDN_SCOPE_FG:
				if (add.id)
					fg = fg_lookup(add.id);
				else if (cc->bind.type == ISDN_BIND_FG)
					fg = cc->bind.u.fg;
				break;
			}
		}
	} else
		switch (cc->bind.type) {
		case ISDN_BIND_CH:
			if (cc->bind.u.ch)
				fg = cc->bind.u.ch->fg.fg;
			break;
		case ISDN_BIND_FG:
			fg = cc->bind.u.fg;
			break;
		}
	return (fg);
}
#endif

/*
 *  =========================================================================
 *
 *  ISDN MESSAGES
 *
 *  =========================================================================
 */
#define ISDN_MT_ESCAPE			0x00
#define ISDN_MT_ALERTING		0x01
#define ISDN_MT_CALL_PROCEEDING		0x02
#define ISDN_MT_PROGRESS		0x03
#define ISDN_MT_SETUP			0x05
#define ISDN_MT_CONNECT			0x07
#define ISDN_MT_SETUP_ACKNOWLEDGE	0x0d
#define ISDN_MT_CONNECT_ACKNOWLEDGE	0x0f
#define ISDN_MT_USER_INFORMATION	0x20
#define ISDN_MT_SUSPEND_REJECT		0x21
#define ISDN_MT_RESUME_REJECT		0x22
#define ISDN_MT_SUSPEND			0x25
#define ISDN_MT_RESUME			0x26
#define ISDN_MT_SUSPEND_ACKNOWLEDGE	0x2d
#define ISDN_MT_RESUME_ACKNOWLEDGE	0x2e
#define ISDN_MT_DISCONNECT		0x45
#define ISDN_MT_RESTART			0x46
#define ISDN_MT_RELEASE			0x4d
#define ISDN_MT_RESTART_ACKNOWLEDGE	0x4e
#define ISDN_MT_RELEASE_COMPLETE	0x5a
#define ISDN_MT_SEGMENT			0x60
#define ISDN_MT_NOTIFY			0x6e
#define ISDN_MT_STATUS_ENQUIRY		0x75
#define ISDN_MT_CONGESTION_CONTROL	0x79
#define ISDN_MT_INFORMATION		0x7b
#define ISDN_MT_STATUS			0x7d

#define ISDN_IE_RESERVED		0x80
#define ISDN_IE_SHIFT			0x90
#define ISDN_IE_MORE_DATA		0xa0
#define ISDN_IE_SENDING_COMPLETE	0xa1
#define ISDN_IE_CONGESTION_LEVEL	0xb0
#define ISDN_IE_REPEAT_INDICATOR	0xd0

#define ISDN_IE_MASK			0xf0

#define ISDN_IE_SEGMENTED_MESSAGE	0x00
#define ISDN_IE_BEARER_CAPABILITY	0x04
#define ISDN_IE_CAUSE			0x08
#define ISDN_IE_CALL_IDENTITY		0x10
#define ISDN_IE_CALL_STATE		0x14
#define ISDN_IE_CHANNEL_IDENTIFICATION	0x18
#define ISDN_IE_PROGRESS_INDICATOR	0x1e
#define ISDN_IE_NTWK_SPEC_FACILITIES	0x20
#define ISDN_IE_DISPLAY			0x28
#define ISDN_IE_DATE_TIME		0x29
#define ISDN_IE_KEYPAD_FACILITY		0x2c
#define ISDN_IE_SIGNAL			0x34
#define ISDN_IE_INFORMATION_RATE	0x40
#define ISDN_IE_E2E_TRANSIT_DELAY	0x42
#define ISDN_IE_TRANS_DEL_SEL_AND_IND	0x43
#define ISDN_IE_PL_BINARY_PARMS		0x44
#define ISDN_IE_PL_WINDOW_SIZE		0x45
#define ISDN_IE_PACKET_SIZE		0x46
#define ISDN_IE_CLOSED_USER_GROUP	0x47
#define ISDN_IE_REVERSE_CHARGE_IND	0x4a
#define ISDN_IE_CALLING_PARTY_NUMBER	0x6c
#define ISDN_IE_CALLING_PARTY_SUBADD	0x6d
#define ISDN_IE_CALLED_PARTY_NUMBER	0x70
#define ISDN_IE_CALLED_PARTY_SUBADD	0x71
#define ISDN_IE_REDIRECTING_NUMBER	0x74
#define ISDN_IE_TRANSIT_NTWK_SEL	0x78
#define ISDN_IE_RESTART_INDICATOR	0x79
#define ISDN_IE_LL_COMPATIBILITY	0x7c
#define ISDN_IE_HL_COMPATIBILITY	0x7d
#define ISDN_IE_USER_TO_USER_INFO	0x7e
#define ISDN_IE_ESCAPE			0x7f

/*
   circuit mode 
 */
#define ISDN_IEF_MD	(1 << 0)
#define ISDN_IEF_SC	(1 << 1)
#define ISDN_IEF_CL	(1 << 2)
#define ISDN_IEF_RP	(1 << 3)
#define ISDN_IEF_SEG	(1 << 4)
#define ISDN_IEF_BC	(1 << 5)
#define ISDN_IEF_CAUSE	(1 << 6)
#define ISDN_IEF_CID	(1 << 7)
#define ISDN_IEF_CS	(1 << 8)
#define ISDN_IEF_CI	(1 << 9)
#define ISDN_IEF_PI	(1 << 10)
#define ISDN_IEF_NSF	(1 << 11)
#define ISDN_IEF_DISP	(1 << 12)
#define ISDN_IEF_DT	(1 << 13)
#define ISDN_IEF_KPF	(1 << 14)
#define ISDN_IEF_SIG	(1 << 15)
#define ISDN_IEF_CGPN	(1 << 16)
#define ISDN_IEF_CGSA	(1 << 17)
#define ISDN_IEF_CDPN	(1 << 18)
#define ISDN_IEF_CDSA	(1 << 19)
#define ISDN_IEF_TNS	(1 << 20)
#define ISDN_IEF_LLC	(1 << 21)
#define ISDN_IEF_HLC	(1 << 22)
#define ISDN_IEF_UUI	(1 << 23)
#define ISDN_IEF_NI	(1 << 24)
#define ISDN_IEF_RI	(1 << 25)

/*
   packet mode 
 */
#define ISDN_IEF_IR	(1 << 0)
#define ISDN_IEF_EETD	(1 << 1)
#define ISDN_IEF_TDSI	(1 << 2)
#define ISDN_IEF_PLBP	(1 << 3)
#define ISDN_IEF_PLWS	(1 << 4)
#define ISDN_IEF_PS	(1 << 5)
#define ISDN_IEF_CUG	(1 << 6)
#define ISDN_IEF_RCI	(1 << 7)
#define ISDN_IEF_RDN	(1 << 8)
#define ISDN_IEF_TI	(1 << 9)

#define CI_MAX_SLOTS	32	/* XXX */

typedef struct isdn_ie {
	unsigned char *ptr;
	size_t len;
	uint32_t val;
} isdn_ie_t;

typedef struct isdn_msg {
	uint32_t cflags;		/* circuit-mode flags - codeset 0 */
	uint32_t pflags;		/* packet-mode flags - codeset 0 */
	uint32_t mt;			/* message type */
	uint32_t cr;			/* call reference */
	uint32_t crf;			/* call reference flags */
	uint32_t codeset;		/* code set */
	uint32_t codeset_lock;		/* code set locks */
	uint32_t codeset_locked;	/* locked codeset */
	struct {
		unsigned char *ptr;	/* pointer */
		size_t len;		/* length */
	} cid;				/* call identitiy */
	struct {
		uint8_t coding_standard;
		uint8_t call_state;
	} cs;				/* call state */
	struct {
		uint8_t nplan;
		uint8_t ntype;
		unsigned char *ptr;
		size_t len;
	} cdpn;				/* called party number */
	struct {
		uint8_t oe;
		uint8_t type;
		unsigned char *ptr;
		size_t len;
	} cdsa;				/* called party subaddress */
	struct {
		uint8_t nplan;
		uint8_t ntype;
		uint8_t si;
		uint8_t pi;
		unsigned char *ptr;
		size_t len;
	} cgpn;				/* calling party number */
	struct {
		uint8_t oe;
		uint8_t type;
		unsigned char *ptr;
		size_t len;
	} cgsa;				/* calling party subaddress */
	uint32_t cause;			/* cause */
	struct {
		uint8_t ichan;
		uint8_t dchan;
		uint8_t pref;
		uint8_t type;
		uint8_t idpres;
		uint32_t iid;
		struct {
			uint8_t type;
			uint8_t nmap;
			uint8_t stand;
			uint32_t slots;
		} cs;
		uint8_t chan[CI_MAX_SLOTS];
	} ci;				/* channel identifier */
	uint32_t cl;			/* congestion level */
	struct {
		uint8_t yr;		/* year */
		uint8_t mo;		/* month */
		uint8_t dy;		/* day */
		uint8_t hr;		/* hour */
		uint8_t mi;		/* minute */
		uint8_t se;		/* second */
	} dt;				/* date */
	struct {
		unsigned char *ptr;	/* pointer */
		size_t len;		/* length */
	} disp;				/* display */
	struct {
		uint8_t pres;
		uint8_t intr;
		uint8_t stand;
		uint8_t hlci;
		uint8_t ehlci;
	} hlc;				/* high layer compatibility */
	struct {
		unsigned char *ptr;	/* pointer */
		size_t len;		/* length */
	} kpf;				/* key pad facility */
	size_t nsf_numb;
	struct {
		uint8_t niplan;
		uint8_t nitype;
		struct {
			unsigned char *ptr;
			size_t len;
		} ni;
		uint8_t spec;
	} nsf[4];			/* network specific facilities */
	uint32_t ni;			/* notification indicator */
	struct {
		uint8_t loc;		/* location */
		uint8_t stand;		/* standard */
		uint8_t pd;		/* progress description */
	} pi;				/* progress indicator */
	uint32_t rp;			/* repeat indicator */
	struct {
		uint8_t clas;		/* class */
	} ri;				/* restart indicator */
	struct {
		uint8_t rem;
		uint8_t first;
		uint8_t mt;
	} seg;				/* segmented message */
	uint32_t sc;			/* sending complete */
	uint32_t sig;			/* signal */
	struct {
		uint8_t niplan;
		uint8_t nitype;
		struct {
			unsigned char *ptr;
			size_t len;
		} ni;
	} tns;				/* transit network selector */
	struct {
		uint8_t pd;
		struct {
			unsigned char *ptr;
			size_t len;
		} ui;
	} uui;				/* user-user information */
	struct {
		uint8_t cugi;
		struct {
			unsigned char *ptr;
			size_t len;
		} code;
	} cug;				/* closed user group */
	struct {
	} eetd;				/* end to end transit delay */
	struct {
	} ir;				/* information rate */
	struct {
	} plbp;				/* packet layer binary parameters */
	struct {
	} plws;				/* packet layer window size */
	struct {
	} ps;				/* packet size */
	struct {
		uint8_t nplan;
		uint8_t ntype;
		uint8_t scrn;
		uint8_t pres;
		uint8_t reason;
		struct {
			unsigned char *ptr;
			size_t len;
		} dig;
	} rdn;				/* redirecting number */
	struct {
	} rci;				/* reverse charging indication */
	struct {
	} tdsi;				/* transit delay selection and indication */
	struct {
		unsigned char *ptr;
		size_t len;
	} opt;				/* optional information elements */
} isdn_msg_t;

/*
 *  ========================================================================
 *
 *  PRIMITIVES
 *
 *  ========================================================================
 */
/*
 *  ------------------------------------------------------------------------
 *
 *  Primitive sent upstream
 *
 *  ------------------------------------------------------------------------
 */
/*
 *  M_FLUSH
 *  -----------------------------------
 */
STATIC INLINE int
m_flush(queue_t *q, queue_t *pq, int band, int flags, int what)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_FLUSH;
		*(mp->b_wptr)++ = flags | band ? FLUSHBAND : 0;
		*(mp->b_wptr)++ = band;
		printd(("%s: %p: <- M_FLUSH\n", DRV_NAME, pq));
		putq(pq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC INLINE int
m_error(queue_t *q, struct cc *cc, int error)
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
			cc_set_state(cc, CCS_UNUSABLE);
			printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (-error);
		} else {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = error < 0 ? -error : error;
			*(mp->b_wptr)++ = error < 0 ? -error : error;
			cc_set_state(cc, CCS_UNUSABLE);
			printd(("%s: %p: <- M_ERROR\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_INFO_ACK
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_info_ack(queue_t *q, struct cc *cc)
{
	mblk_t *mp;
	struct CC_info_ack *p;
	ensure(cc, return (-EFAULT));
	ensure(cc->oq, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_INFO_ACK;
		fixme(("Fill out more of the message\n"));
		printd(("%s: %p: <- CC_INFO_ACK\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_BIND_ACK
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_bind_ack(queue_t *q, struct cc *cc, unsigned char *add_ptr, size_t add_len, ulong setup_ind,
	    ulong token_value)
{
	mblk_t *mp;
	struct CC_bind_ack *p;
	ensure(cc, return (-EFAULT));
	ensure(cc->oq, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_BIND_ACK;
		p->cc_addr_length = add_len;
		p->cc_addr_offset = add_len ? sizeof(*p) : 0;
		p->cc_setup_ind = setup_ind;
		p->cc_token_value = token_value;
		if (add_len) {
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: <- CC_BIND_ACK\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_OPTMGMT_ACK
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_optmgmt_ack(queue_t *q, struct cc *cc, unsigned char *opt_ptr, size_t opt_len, ulong flags,
	       ulong cref)
{
	mblk_t *mp;
	struct CC_optmgmt_ack *p;
	ensure(cc, return (-EFAULT));
	ensure(cc->oq, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_call_ref = cref;
		p->cc_opt_length = opt_len;
		p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
		p->cc_opt_flags = flags;
		if (opt_len) {
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		printd(("%s: %p: <- CC_OPTMGMT_ACK\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_ADDR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
cc_addr_ack(queue_t *q, struct cc *cc, unsigned char *bind_ptr, size_t bind_len, ulong cref,
	    unsigned char *conn_ptr, size_t conn_len)
{
	mblk_t *mp;
	struct CC_addr_ack *p;
	ensure(cc, return (-EFAULT));
	ensure(cc->oq, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p) + bind_len + conn_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_ADDR_ACK;
		p->cc_bind_length = bind_len;
		p->cc_bind_offset = bind_len ? sizeof(*p) : 0;
		p->cc_call_ref = cref;
		p->cc_conn_length = conn_len;
		p->cc_conn_offset = conn_len ? sizeof(*p) + bind_len : 0;
		if (bind_len) {
			bcopy(bind_ptr, mp->b_wptr, bind_len);
			mp->b_wptr += bind_len;
		}
		if (conn_len) {
			bcopy(conn_ptr, mp->b_wptr, conn_len);
			mp->b_wptr += conn_len;
		}
		printd(("%s: %p: <- CC_ADDR_ACK\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_OK_ACK
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_ok_ack(queue_t *q, struct cc *cc, long prim)
{
	mblk_t *mp;
	struct CC_ok_ack *p;
	ensure(cc, return (-EFAULT));
	ensure(cc->oq, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_OK_ACK;
		p->cc_correct_prim = prim;
		p->cc_state = cc->state;
		printd(("%s: %p: <- CC_OK_ACK\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_ERROR_ACK
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_error_ack(queue_t *q, struct cc *cc, ulong prim, long error)
{
	mblk_t *mp;
	struct CC_error_ack *p;
	ensure(cc, return (-EFAULT));
	ensure(cc->oq, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_ERROR_ACK;
		p->cc_error_primitive = prim;
		p->cc_error_type = error < 0 ? CCSYSERR : error;
		p->cc_unix_error = error < 0 ? -error : 0;
		printd(("%s: %p: <- CC_ERROR_ACK\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_SETUP_IND
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_setup_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_setup_ind *p;
		ulong type = 0;		/* FIXME: get from message later */
		ulong flags = 0;	/* FIXME: get from message later */
		uchar *num_ptr = m->cdpn.ptr;	/* FIXME: get from message later */
		size_t num_len = m->cdpn.len;	/* FIXME: get from message later */
		uchar *opt_ptr = m->opt.ptr;	/* FIXME: get from message later */
		size_t opt_len = m->opt.len;	/* FIXME: get from message later */
		uchar *add_ptr = NULL;	/* FIXME: get from message later */
		size_t add_len = 0;	/* FIXME: get from message later */
		if ((mp = ss7_allocb(q, sizeof(*p) + num_len + opt_len + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SETUP_IND;
			p->cc_call_ref = cr->cref;
			p->cc_call_type = type;
			p->cc_call_flags = flags;
			p->cc_cdpn_length = num_len;
			p->cc_cdpn_offset = num_len ? sizeof(*p) : 0;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) + num_len : 0;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) + num_len + opt_len : 0;
			if (num_len) {
				bcopy(num_ptr, mp->b_wptr, num_len);
				mp->b_wptr += num_len;
			}
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			if (add_len) {
				bcopy(add_ptr, mp->b_wptr, add_len);
				mp->b_wptr += add_len;
			}
			printd(("%s: %p: <- CC_SETUP_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_SETUP_CON
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_setup_con(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (-EFAULT));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_setup_con *p;
		struct isdn_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SETUP_CON;
			p->cc_user_ref = cr->uref;
			p->cc_call_ref = cr->cref;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISDN_SCOPE_CH;
			a->id = cr->ch.list->id;
			a->ci = cr->ch.list->ci;
			printd(("%s: %p: <- CC_SETUP_CON\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			cc_put_uref(cr, cc);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_MORE_INFO_IND
 *  ------------------------------------------------------------------------
 *  The more information indication is derived directly from the SETUP ACKNOWLEDGE message.
 */
STATIC INLINE int
cc_more_info_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		struct CC_more_info_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_MORE_INFO_IND;
			p->cc_user_ref = cr->uref;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_MORE_INFO_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_INFORMATION_IND
 *  ------------------------------------------------------------------------
 *  The information indication is derived directly from the INFORMATION message.
 *
 *  Either the Called party number or the Keypad facility information element is included by the user to transfer
 *  called party number information to the network during overlap sending.  The Keypad facility information element
 *  may also be included if the user wants to convey other call establishment information to the network, or to
 *  convey supplementatry service information.
 *
 *  Either the Called party number of the Keypad facility information element is included by the user to convey
 *  called party number information to the network during overlap sending.  The Called party number information
 *  element is included by the network to convey called party number information to the user during overlap
 *  receiving.
 */
STATIC INLINE int
cc_information_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_information_ind *p;
		uchar *sub_ptr = m->cdpn.ptr ? m->cdpn.ptr + 1 : (m->kpf.ptr ? m->kpf.ptr : NULL);
		size_t sub_len = m->cdpn.ptr ? m->cdpn.len - 1 : (m->kpf.ptr ? m->kpf.len : 0);
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + sub_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_INFORMATION_IND;
			p->cc_call_ref = cr->cref;
			p->cc_subn_length = sub_len;
			p->cc_subn_offset = sub_len ? sizeof(*p) : 0;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) + sub_len : 0;
			if (sub_len) {
				bcopy(sub_ptr, mp->b_wptr, sub_len);
				mp->b_wptr += sub_len;
			}
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_INFORMATION_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_INFO_TIMEOUT_IND
 *  ------------------------------------------------------------------------
 *  The timeout indication is derived direclty from a timeout.
 */
STATIC INLINE int
cc_info_timeout_ind(queue_t *q, struct cr *cr)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_info_timeout_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_INFO_TIMEOUT_IND;
			p->cc_call_ref = cr->cref;
			printd(("%s: %p: <- CC_INFO_TIMEOUT_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_ERROR_IND
 *  ------------------------------------------------------------------------
 *  The error indication is derived directly from a data link connection.
 */
STATIC INLINE int
cc_error_ind(queue_t *q, struct cr *cr)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_error_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_ERROR_IND;
			p->cc_call_ref = cr->cref;
			printd(("%s: %p: <- CC_ERROR_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_PROCEEDING_IND
 *  ------------------------------------------------------------------------
 *  The proceeding indication is derived directly from the CALL PROCEEDING message.
 */
STATIC INLINE int
cc_proceeding_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_proceeding_ind *p;
		ulong flags = 0;	/* FIXME: get from message later */
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_PROCEEDING_IND;
			p->cc_call_ref = cr->cref;
			p->cc_flags = flags;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_PROCEEDING_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_ALERTING_IND
 *  ------------------------------------------------------------------------
 *  The alerting indication is derived directly from the ALERTING message.
 */
STATIC INLINE int
cc_alerting_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_alerting_ind *p;
		ulong flags = 0;	/* FIXME: get from message later */
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_ALERTING_IND;
			p->cc_call_ref = cr->cref;
			p->cc_flags = flags;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_ALERTING_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_PROGRESS_IND
 *  ------------------------------------------------------------------------
 *  The progress indication is derived directly from the PROGRESS message.
 */
STATIC INLINE int
cc_progress_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_progress_ind *p;
		ulong event = 0;	/* FIXME: get from message later */
		ulong flags = 0;	/* FIXME: get from message later */
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_PROGRESS_IND;
			p->cc_call_ref = cr->cref;
			p->cc_event = event;
			p->cc_flags = flags;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_PROGRESS_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_DISCONNECT_IND
 *  ------------------------------------------------------------------------
 *  The disconnect indication is derived directly from the DISCONNECT message.
 */
STATIC INLINE int
cc_disconnect_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_disconnect_ind *p;
		ulong cause = m ? m->cause : 0;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_DISCONNECT_IND;
			p->cc_call_ref = cr->cref;
			p->cc_cause = cause;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_DISCONNECT_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_CONNECT_IND
 *  ------------------------------------------------------------------------
 *  The connect indication is derived directly from the CONNECT message.
 */
STATIC INLINE int
cc_connect_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_connect_ind *p;
		ulong flags = m ? ((m->pi.loc << 16) | (m->pi.stand << 8) | (m->pi.pd)) : 0;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_CONNECT_IND;
			p->cc_call_ref = cr->cref;
			p->cc_flags = flags;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_CONNECT_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_SETUP_COMPLETE_IND
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_setup_complete_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_setup_complete_ind *p;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SETUP_COMPLETE_IND;
			p->cc_call_ref = cr->cref;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_SETUP_COMPLETE_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_NOTIFY_IND
 *  ------------------------------------------------------------------------
 *  Derived directly from NOTIFY message.
 */
STATIC INLINE int
cc_notify_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_notify_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_NOTIFY_IND;
			printd(("%s: %p: <- CC_NOTIFY_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_SUSPEND_IND
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_suspend_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_suspend_ind *p;
		ulong flags = 0;	/* FIXME: get from message later */
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SUSPEND_IND;
			p->cc_call_ref = cr->cref;
			p->cc_flags = flags;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_SUSPEND_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_SUSPEND_CON
 *  ------------------------------------------------------------------------
 *  Derived directly from SUSPEND-ACKNOWLEDGE, SUSPEND-REJECT or timeout.
 */
STATIC INLINE int
cc_suspend_con(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_suspend_con *p;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SUSPEND_CON;
			p->cc_call_ref = cr->cref;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_SUSPEND_CON\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_SUSPEND_REJECT_IND
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_suspend_reject_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_suspend_reject_ind *p;
		ulong cause = m ? m->cause : 0;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SUSPEND_REJECT_IND;
			p->cc_call_ref = cr->cref;
			p->cc_cause = cause;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_SUSPEND_REJECT_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_RESUME_IND
 *  ------------------------------------------------------------------------
 *  Derived directly from RESUME message.
 */
STATIC INLINE int
cc_resume_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_resume_ind *p;
		ulong flags = 0;	/* FIXME: get from message later */
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RESUME_IND;
			p->cc_call_ref = cr->cref;
			p->cc_flags = flags;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_RESUME_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_RESUME_CON
 *  ------------------------------------------------------------------------
 *  Derived directly from RESUME-ACKNOWLEDGE, RESUME-REJECT or timeout.
 */
STATIC INLINE int
cc_resume_con(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_resume_con *p;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RESUME_CON;
			p->cc_call_ref = cr->cref;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_RESUME_CON\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_RESUME_REJECT_IND
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_resume_reject_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_resume_reject_ind *p;
		ulong cause = m ? m->cause : 0;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RESUME_REJECT_IND;
			p->cc_call_ref = cr->cref;
			p->cc_cause = cause;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_RESUME_REJECT_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_REJECT_IND
 *  ------------------------------------------------------------------------
 *  Derived directly from RELEASE-COMPLETE message.
 */
STATIC INLINE int
cc_reject_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_reject_ind *p;
		ulong cause = m ? m->cause : 0;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_REJECT_IND;
			p->cc_user_ref = cr->uref;
			p->cc_cause = cause;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_REJECT_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_RELEASE_IND
 *  ------------------------------------------------------------------------
 *  Derived directly from RELEASE, RELEASE-COMPLETE, DL-DISCONNECT-IND, or
 *  CC_RESTART_REQ.
 */
STATIC INLINE int
cc_release_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_release_ind *p;
		ulong cause = m ? m->cause : 0;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RELEASE_IND;
			p->cc_user_ref = cr->uref;
			p->cc_call_ref = cr->cref;
			p->cc_cause = cause;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_RELEASE_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_RELEASE_CON
 *  ------------------------------------------------------------------------
 *  Derived directly from RELEASE-COMPLETE or timeout.
 */
STATIC INLINE int
cc_release_con(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_release_con *p;
		uchar *opt_ptr = m ? m->opt.ptr : NULL;
		size_t opt_len = m ? m->opt.len : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RELEASE_CON;
			p->cc_user_ref = cr->uref;
			p->cc_call_ref = cr->cref;
			p->cc_opt_length = opt_len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_RELEASE_CON\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_RESTART_CON
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_restart_con(queue_t *q, struct cr *cr)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_restart_con *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RESTART_CON;
			printd(("%s: %p: <- CC_RESTART_CON\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_STATUS_IND
 *  ------------------------------------------------------------------------
 *  Derived directly from STATUS message.
 */
STATIC INLINE int
cc_status_ind(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_status_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_STATUS_IND;
			printd(("%s: %p: <- CC_STATUS_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_DATALINK_FAILURE_IND
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
cc_datalink_failure_ind(queue_t *q, struct cr *cr)
{
	struct cc *cc = cr->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (-EFAULT));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_datalink_failure_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_DATALINK_FAILURE_IND;
			p->cc_user_ref = cr->uref;
			p->cc_call_ref = cr->cref;
			printd(("%s: %p: <- CC_DATALINK_FAILURE_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  ------------------------------------------------------------------------
 *  These are primitives which are sent downstream at the lower stream to the
 *  ISDN DL layer from the driver.
 */
/*
 *  DL_DATA_REQ
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_data_req(queue_t *q, struct dl *dl, mblk_t *dp)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		ss7_oput(dl->oq, dp);
		return (QR_DONE);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_INFO_REQ            0x00
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_info_req(queue_t *q, struct dl *dl)
{
	mblk_t *mp;
	dl_info_req_t *p;
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->dl_primitive = DL_INFO_REQ;
		printd(("%s: %p: DL_INFO_REQ ->\n", DRV_NAME, dl));
		ss7_oput(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_ATTACH_REQ          0x0b
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_attach_req(queue_t *q, struct dl *dl, ulong ppa)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_attach_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_ATTACH_REQ;
			p->dl_ppa = ppa;
			printd(("%s: %p: DL_ATTACH_REQ ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_ATTACH_PENDING);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_DETACH_REQ          0x0c
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_detach_req(queue_t *q, struct dl *dl)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_detach_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_DETACH_REQ;
			printd(("%s: %p: DL_DETACH_REQ ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_DETACH_PENDING);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_BIND_REQ            0x01
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_bind_req(queue_t *q, struct dl *dl, ulong sap)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_bind_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_BIND_REQ;
			p->dl_sap = sap;
			p->dl_max_conind = 0;
			p->dl_service_mode = DL_CLDLS;
			p->dl_conn_mgmt = 0;
			p->dl_xidtest_flg = 0;
			printd(("%s: %p: DL_BIND_REQ ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_BIND_PENDING);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_UNBIND_REQ          0x02
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_unbind_req(queue_t *q, struct dl *dl)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_unbind_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_UNBIND_REQ;
			printd(("%s: %p: DL_UNBIND_REQ ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_UNBIND_PENDING);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_SUBS_BIND_REQ       0x1b
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_subs_bind_req(queue_t *q, struct dl *dl, caddr_t sap_ptr, size_t sap_len)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_subs_bind_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + sap_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_SUBS_BIND_REQ;
			p->dl_subs_sap_offset = sap_len ? sizeof(*p) : 0;
			p->dl_subs_sap_length = sap_len;
			p->dl_subs_bind_class = DL_HIERARCHICAL_BIND;
			if (sap_len) {
				bcopy(sap_ptr, mp->b_wptr, sap_len);
				mp->b_wptr += sap_len;
			}
			printd(("%s: %p: DL_SUBS_BIND_REQ ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_SUBS_BIND_PND);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_SUBS_UNBIND_REQ     0x15
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_subs_unbind_req(queue_t *q, struct dl *dl, caddr_t sap_ptr, size_t sap_len)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_subs_unbind_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + sap_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_SUBS_UNBIND_REQ;
			p->dl_subs_sap_offset = sap_len ? sizeof(*p) : 0;
			p->dl_subs_sap_length = sap_len;
			if (sap_len) {
				bcopy(sap_ptr, mp->b_wptr, sap_len);
				mp->b_wptr += sap_len;
			}
			printd(("%s: %p: DL_SUBS_UNBIND_REQ ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_SUBS_UNBIND_PND);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_ENABMULTI_REQ       0x1d
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_enabmulti_req(queue_t *q, struct dl *dl, caddr_t add_ptr, size_t add_len)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_enabmulti_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_ENABMULTI_REQ;
			p->dl_addr_length = add_len;
			p->dl_addr_offset = add_len ? sizeof(*p) : 0;
			if (add_len) {
				bcopy(add_ptr, mp->b_wptr, add_len);
				mp->b_wptr += add_len;
			}
			printd(("%s: %p: DL_ENABMULTI_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_DISABMULTI_REQ      0x1e
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_disabmulti_req(queue_t *q, struct dl *dl, caddr_t add_ptr, size_t add_len)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_disabmulti_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_DISABMULTI_REQ;
			p->dl_addr_length = add_len;
			p->dl_addr_offset = add_len ? sizeof(*p) : 0;
			if (add_len) {
				bcopy(add_ptr, mp->b_wptr, add_len);
				mp->b_wptr += add_len;
			}
			printd(("%s: %p: DL_DISABMULTI_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_PROMISCON_REQ       0x1f
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_promiscon_req(queue_t *q, struct dl *dl, ulong level)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_promiscon_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_PROMISCON_REQ;
			p->dl_level = level;
			printd(("%s: %p: DL_PROMISCON_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_PROMISCOFF_REQ      0x20
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_promiscoff_req(queue_t *q, struct dl *dl, ulong level)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_promiscoff_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_PROMISCOFF_REQ;
			p->dl_level = level;
			printd(("%s: %p: DL_PROMISCOFF_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_CONNECT_REQ         0x0d
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_connect_req(queue_t *q, struct dl *dl)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_connect_req_t *p;
		uchar *dst_ptr = (uchar *) (&dl->dlc);
		size_t dst_len = sizeof(dl->dlc);
		uchar *qos_ptr = NULL;
		size_t qos_len = 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + qos_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_CONNECT_REQ;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = qos_len ? sizeof(*p) + dst_len : 0;
			p->dl_growth = 0;
			if (dst_len) {
				bcopy(dst_ptr, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			if (qos_len) {
				bcopy(qos_ptr, mp->b_wptr, qos_len);
				mp->b_wptr += qos_len;
			}
			printd(("%s: %p: DL_CONNECT_REQ ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_OUTCON_PENDING);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_CONNECT_RES         0x0f
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_connect_res(queue_t *q, struct dl *dl, ulong cor, ulong tok, caddr_t qos_ptr, size_t qos_len)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_connect_res_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + qos_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_CONNECT_RES;
			p->dl_correlation = cor;
			p->dl_resp_token = tok;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = qos_len ? sizeof(*p) : 0;
			p->dl_growth = 0;
			if (qos_len) {
				bcopy(qos_ptr, mp->b_wptr, qos_len);
				mp->b_wptr += qos_len;
			}
			printd(("%s: %p: DL_CONNECT_RES ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_CONN_RES_PENDING);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_TOKEN_REQ           0x11
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_token_req(queue_t *q, struct dl *dl)
{
	mblk_t *mp;
	dl_token_req_t *p;
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->dl_primitive = DL_TOKEN_REQ;
		printd(("%s: %p: DL_TOKEN_REQ ->\n", DRV_NAME, dl));
		ss7_oput(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_DISCONNECT_REQ      0x13
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_disconnect_req(queue_t *q, struct dl *dl, ulong reason, ulong cor)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_disconnect_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_DISCONNECT_REQ;
			p->dl_reason = reason;
			p->dl_correlation = cor;
			printd(("%s: %p: DL_DISCONNECT_REQ ->\n", DRV_NAME, dl));
			switch (dl_get_state(dl)) {
			case DL_OUTCON_PENDING:
				dl_set_state(dl, DL_DISCON8_PENDING);
				break;
			case DL_INCON_PENDING:
				dl_set_state(dl, DL_DISCON9_PENDING);
				break;
			case DL_DATAXFER:
				dl_set_state(dl, DL_DISCON11_PENDING);
				break;
			case DL_USER_RESET_PENDING:
				dl_set_state(dl, DL_DISCON12_PENDING);
				break;
			case DL_PROV_RESET_PENDING:
				dl_set_state(dl, DL_DISCON13_PENDING);
				break;
			}
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_RESET_REQ           0x17
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_reset_req(queue_t *q, struct dl *dl)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_reset_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_RESET_REQ;
			printd(("%s: %p: DL_RESET_REQ ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_USER_RESET_PENDING);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_RESET_RES           0x19
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_reset_res(queue_t *q, struct dl *dl)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_reset_res_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_RESET_RES;
			printd(("%s: %p: DL_RESET_RES ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_RESET_RES_PENDING);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_UNITDATA_REQ        0x07
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_unitdata_req(queue_t *q, struct dl *dl, caddr_t dst_ptr, size_t dst_len, ulong prior_min,
		ulong prior_max, mblk_t *dp)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_unitdata_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_UNITDATA_REQ;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_priority.dl_min = prior_min;
			p->dl_priority.dl_max = prior_max;
			if (dst_len) {
				bcopy(dst_ptr, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: DL_UNITDATA_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_UDQOS_REQ           0x0a
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_udqos_req(queue_t *q, struct dl *dl, caddr_t qos_ptr, size_t qos_len)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_udqos_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + qos_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_UDQOS_REQ;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = qos_len ? sizeof(*p) : 0;
			if (qos_len) {
				bcopy(qos_ptr, mp->b_wptr, qos_len);
				mp->b_wptr += qos_len;
			}
			printd(("%s: %p: DL_UDQOS_REQ ->\n", DRV_NAME, dl));
			dl_set_state(dl, DL_UDQOS_PENDING);
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_TEST_REQ            0x2d
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_test_req(queue_t *q, struct dl *dl, ulong flag, caddr_t dst_ptr, size_t dst_len, mblk_t *dp)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_test_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_TEST_REQ;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			if (dst_len) {
				bcopy(dst_ptr, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: DL_TEST_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_TEST_RES            0x2f
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_test_res(queue_t *q, struct dl *dl, ulong flag, caddr_t dst_ptr, size_t dst_len, mblk_t *dp)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_test_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_TEST_RES;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			if (dst_len) {
				bcopy(dst_ptr, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: DL_TEST_RES ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_XID_REQ             0x29
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_xid_req(queue_t *q, struct dl *dl, ulong flag, caddr_t dst_ptr, size_t dst_len, mblk_t *dp)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_xid_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_XID_REQ;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			if (dst_len) {
				bcopy(dst_ptr, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: DL_XID_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_XID_RES             0x2b
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_xid_res(queue_t *q, struct dl *dl, ulong flag, caddr_t dst_ptr, size_t dst_len, mblk_t *dp)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_xid_res_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_XID_RES;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			if (dst_len) {
				bcopy(dst_ptr, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: DL_XID_RES ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_DATA_ACK_REQ        0x21
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_data_ack_req(queue_t *q, struct dl *dl, ulong cor, caddr_t dst_ptr, size_t dst_len,
		caddr_t src_ptr, size_t src_len, ulong prior, mblk_t *dp)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_data_ack_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_DATA_ACK_REQ;
			p->dl_correlation = cor;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			p->dl_priority = prior;
			p->dl_service_class = 0;
			if (dst_len) {
				bcopy(dst_ptr, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			if (src_len) {
				bcopy(src_ptr, mp->b_wptr, src_len);
				mp->b_wptr += src_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: DL_DATA_ACK_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_REPLY_REQ           0x24
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_reply_req(queue_t *q, struct dl *dl, ulong cor, caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr,
	     size_t src_len, ulong prior, mblk_t *dp)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_reply_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_REPLY_REQ;
			p->dl_correlation = cor;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
			p->dl_priority = prior;
			p->dl_service_class = 0;
			if (dst_len) {
				bcopy(dst_ptr, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			if (src_len) {
				bcopy(src_ptr, mp->b_wptr, src_len);
				mp->b_wptr += src_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: DL_REPLY_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_REPLY_UPDATE_REQ    0x27
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_reply_update_req(queue_t *q, struct dl *dl, ulong cor, caddr_t src_ptr, size_t src_len,
		    mblk_t *dp)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_reply_update_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + src_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_REPLY_UPDATE_REQ;
			p->dl_correlation = cor;
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = src_len ? sizeof(*p) : 0;
			if (src_len) {
				bcopy(src_ptr, mp->b_wptr, src_len);
				mp->b_wptr += src_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: DL_REPLY_UPDATE_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_PHYS_ADDR_REQ       0x31
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_phys_addr_req(queue_t *q, struct dl *dl, ulong type)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_phys_addr_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_PHYS_ADDR_REQ;
			p->dl_addr_type = type;
			printd(("%s: %p: DL_PHYS_ADDR_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_SET_PHYS_ADDR_REQ   0x33
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_set_phys_addr_req(queue_t *q, struct dl *dl, caddr_t add_ptr, size_t add_len)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_set_phys_addr_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_SET_PHYS_ADDR_REQ;
			p->dl_addr_length = add_len;
			p->dl_addr_offset = add_len ? sizeof(*p) : 0;
			printd(("%s: %p: DL_SET_PHYS_ADDR_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  DL_GET_STATISTICS_REQ  0x34
 *  ------------------------------------------------------------------------
 */
STATIC INLINE int
dl_get_statistics_req(queue_t *q, struct dl *dl)
{
	ensure(dl, return (-EFAULT));
	ensure(dl->oq, return (-EFAULT));
	if (canput(dl->oq)) {
		mblk_t *mp;
		dl_get_statistics_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->dl_primitive = DL_GET_STATISTICS_REQ;
			printd(("%s: %p: DL_GET_STATISTICS_REQ ->\n", DRV_NAME, dl));
			ss7_oput(dl->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  ISDN MESSAGE ENCODING
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Sizing functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  PROTOCOL DISCRIMINATOR
 *  -----------------------------------
 */
STATIC INLINE int
size_pd(uint pvar, isdn_msg_t * m)
{
	return (1);
}

/*
 *  CALL REFERENCE
 *  -----------------------------------
 */
STATIC INLINE int
size_cr(uint pvar, isdn_msg_t * m)
{
	int len = 1;
	uint32_t cr = m->cr;
	if (cr) {
		len++;
		cr >>= 7;
	}
	while (cr) {
		len++;
		cr >>= 8;
	}
	return (len);
}

/*
 *  MESSAGE TYPE
 *  -----------------------------------
 */
STATIC INLINE int
size_mt(uint pvar, isdn_msg_t * m)
{
	int len = 0;
	uint32_t mt = m->mt;
	do {
		len++;
		mt >>= 7;
	} while (mt);
	return (len);
}

/*
 *  SHIFT
 *  -----------------------------------
 */
STATIC INLINE int
size_shift(uint pvar, isdn_msg_t * m)
{
	return (1);
}

/*
 *  BEARER CAPABILITY
 *  -----------------------------------
 */
STATIC INLINE int
size_bc(uint pvar, isdn_msg_t * m)
{
	fixme(("Write this horrible function\n"));
	return (-EFAULT);
}

/*
 *  CALL IDENTITY
 *  -----------------------------------
 */
STATIC INLINE int
size_cid(uint pvar, isdn_msg_t * m)
{
	return (m->cid.len + 2);
}

/*
 *  CALL STATE
 *  -----------------------------------
 */
STATIC INLINE int
size_cs(uint pvar, isdn_msg_t * m)
{
	return (3);
}

/*
 *  CALLED PARTY NUMBER
 *  -----------------------------------
 */
STATIC INLINE int
size_cdpn(uint pvar, isdn_msg_t * m)
{
	return (m->cdpn.len + 3);
}

/*
 *  CALLED PARTY SUBADDRESS
 *  -----------------------------------
 */
STATIC INLINE int
size_cdsa(uint pvar, isdn_msg_t * m)
{
	return (m->cdsa.len + 3);
}

/*
 *  CALLING PARTY NUMBER
 *  -----------------------------------
 */
STATIC INLINE int
size_cgpn(uint pvar, isdn_msg_t * m)
{
	return (m->cgpn.len + 4);
}

/*
 *  CALLING PARTY SUBADDRESS
 *  -----------------------------------
 */
STATIC INLINE int
size_cgsa(uint pvar, isdn_msg_t * m)
{
	return (m->cgsa.len + 3);
}

/*
 *  CAUSE
 *  -----------------------------------
 */
STATIC INLINE int
size_cause(uint pvar, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Checking parameter values
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  CALLED PARTY NUMBER
 *  -----------------------------------
 */
STATIC int
isdn_check_cdpn(uchar *ptr, size_t len)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  OPTIONAL PARAMETERS
 *  -----------------------------------
 */
STATIC int
isdn_check_opt(uchar *ptr, size_t len)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  ADDRESS
 *  -----------------------------------
 */
STATIC int
isdn_check_addr(uchar *ptr, size_t len)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  SUBSEQUENT NUMBER
 *  -----------------------------------
 */
STATIC int
isdn_check_subn(uchar *ptr, size_t len)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  BEARER CAPABILITY
 *  -----------------------------------
 */
STATIC int
isdn_check_bc(queue_t *q, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  CAUSE
 *  -----------------------------------
 */
STATIC int
isdn_check_cause(queue_t *q, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  CHANNEL IDENTIFICATION
 *  -----------------------------------
 */
STATIC int
isdn_check_ci(queue_t *q, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  CALL STATE
 *  -----------------------------------
 */
STATIC int
isdn_check_cs(queue_t *q, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  NOTIFICATION INDICATOR
 *  -----------------------------------
 */
STATIC int
isdn_check_ni(queue_t *q, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  PROGRESS INDICATOR
 *  -----------------------------------
 */
STATIC int
isdn_check_pi(queue_t *q, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (0);		/* let pass */
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Packing functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  PROTOCOL DISCRIMINATOR
 *  -----------------------------------
 */
STATIC int
pack_pd(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	ensure(p < e, return (-EFAULT));
	*p++ = 0x08;
	return (1);
}

/*
 *  CALL REFERENCE
 *  -----------------------------------
 */
STATIC int
pack_cr(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint32_t cr = m->cr;
	uint len = 0;
	unsigned char *lp = p++;
	if (cr) {
		ensure(p < e, return (-EFAULT));
		*p++ = (m->crf << 7) | (cr & 0x7f);
		len++;
		cr >>= 7;
	}
	while (cr) {
		ensure(p < e, return (-EFAULT));
		*p++ = cr;
		len++;
		cr >>= 8;
	}
	*lp = len;
	return (len + 1);
}

/*
 *  MESSAGE TYPE
 *  -----------------------------------
 */
STATIC int
pack_mt(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint32_t mt = m->mt;
	uint len = 0;
	do {
		*p++ = mt & 0x7f;
		len++;
		mt >>= 7;
	} while (mt);
	p[-1] |= 0x80;
	return (len);
}

#if 0
/*
 *  SHIFT
 *  -----------------------------------
 */
STATIC int
pack_shift(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	*p++ = ISDN_IE_SHIFT | ((m->codeset_lock & 0x1) << 3) | (m->codeset & 0x7);
	return (1);
}

/*
 *  BEARER CAPABILITY
 *  -----------------------------------
 */
STATIC int
pack_bc(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	fixme(("Write this horrible function\n"));
	return (-EFAULT);
}

/*
 *  CALL IDENTITY
 *  -----------------------------------
 */
STATIC int
pack_cid(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len = m->cid.len;
	*p++ = ISDN_IE_CALL_IDENTITY;
	*p++ = len;
	bcopy(m->cid.ptr, p, len);
	return (len + 2);
}

/*
 *  CALL STATE
 *  -----------------------------------
 */
STATIC int
pack_cs(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	*p++ = ISDN_IE_CALL_STATE;
	*p++ = 1;
	*p++ = ((m->cs.coding_standard & 0x3) << 6) | ((m->cs.call_state & 0x3f) << 0);
	return (3);
}

/*
 *  CALLED PARTY NUMBER
 *  -----------------------------------
 */
STATIC int
pack_cdpn(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	int len = m->cdpn.len;
	*p++ = ISDN_IE_CALLED_PARTY_NUMBER;
	*p++ = len + 1;
	*p++ = 0x80 | ((m->cdpn.ntype & 0x07) << 4) | ((m->cdpn.nplan & 0xf) << 0);
	bcopy(m->cdpn.ptr, p, len);
	return (len + 3);
}

/*
 *  CALLED PARTY SUBADDRESS
 *  -----------------------------------
 */
STATIC int
pack_cdsa(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	int len = m->cdsa.len;
	*p++ = ISDN_IE_CALLED_PARTY_SUBADD;
	*p++ = len + 1;
	*p++ = 0x80 | ((m->cdsa.type & 0x7) << 4) | ((m->cdsa.oe & 0x1) << 3);
	bcopy(m->cdsa.ptr, p, len);
	return (len + 3);
}

/*
 *  CALLING PARTY NUMBER
 *  -----------------------------------
 */
STATIC int
pack_cgpn(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	int len = m->cgpn.len;
	*p++ = ISDN_IE_CALLING_PARTY_NUMBER;
	*p++ = len + 2;
	*p++ = ((m->cgpn.ntype & 0x07) << 4) | ((m->cgpn.nplan & 0xf) << 0);
	*p++ = 0x80 | ((m->cgpn.pi & 0x3) << 5) | ((m->cgpn.si & 0x3) << 0);
	bcopy(m->cgpn.ptr, p, len);
	return (len + 4);
}

/*
 *  CALLING PARTY SUBADDRESS
 *  -----------------------------------
 */
STATIC int
pack_cgsa(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	int len = m->cgsa.len;
	*p++ = ISDN_IE_CALLING_PARTY_SUBADD;
	*p++ = len + 1;
	*p++ = 0x80 | ((m->cgsa.type & 0x7) << 4) | ((m->cgsa.oe & 0x1) << 3);
	bcopy(m->cgsa.ptr, p, len);
	return (len + 3);
}

/*
 *  CAUSE
 *  -----------------------------------
 */
STATIC int
pack_cause(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CHANNEL IDENTIFICATION
 *  -----------------------------------
 */
STATIC int
pack_chid(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
#endif

/*
 *  INFORMATION ELEMENTS
 *  -----------------------------------
 */
STATIC int
pack_ies(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	int ret, len = 0;
	while (p < e) {
		(void) ret;
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (len);
}

STATIC INLINE int
isdn_send_msg(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct dl *dl;
	int err;
	ensure(cr, return (-EFAULT));
	if ((dl = cr->dl) && (dl->i_state != DL_DATAXFER)) {
		struct dc *dc;
		ushort sap = dl->dlc.dl_sap;
		ushort tei = dl->dlc.dl_tei;
		for (dc = cr->fg.fg->dc.list; dc; dc = dc->fg.next)
			for (dl = dc->dl.list;
			     dl && (dl->dlc.dl_tei != tei || dl->dlc.dl_sap != sap
				    || dl->i_state != DL_DATAXFER); dl = dl->dc.next) ;
	}
	if (!dl) {
		/* 
		   no viable data link */
		rare();
		return (-EFAULT);
	}
	if (canput(dl->oq)) {
		mblk_t *mp;
		size_t size = 0;	/* FIXME */
		if ((mp = ss7_allocb(q, size, BPRI_MED))) {
			unsigned char *p;
			unsigned char *e;
			ulong pvar = cr->fg.fg->proto.pvar;
			mp->b_datap->db_type = M_DATA;
			/* 
			   fill out message */
			p = mp->b_rptr;
			mp->b_wptr += size;
			e = mp->b_wptr;
			if ((err = pack_pd(pvar, p, e, m)) < 0)
				goto free_error;
			p += err;
			if ((err = pack_cr(pvar, p, e, m)) < 0)
				goto free_error;
			p += err;
			if ((err = pack_mt(pvar, p, e, m)) < 0)
				goto free_error;
			if ((err = pack_ies(pvar, p, e, m)) < 0)
				goto free_error;
			p += err;
			ss7_oput(dl->oq, mp);
			todo(("Peg statistics\n"));
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	      free_error:
		freemsg(mp);
		return (err);
	}
	rare();
	return (-EBUSY);
}

/*
 *  SEND ALERTING
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_alerting(queue_t *q, struct cr *cr)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_ALERTING;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND CALL PROCEEDING
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_call_proceeding(queue_t *q, struct cr *cr)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_CALL_PROCEEDING;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND CONNECT ACKNOWLEDGE
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_connect_acknowledge(queue_t *q, struct cr *cr, uchar *opt_ptr, size_t opt_len)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_CONNECT_ACKNOWLEDGE;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND CONNECT
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_connect(queue_t *q, struct cr *cr)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_CONNECT;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND DISCONNECT
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_disconnect(queue_t *q, struct cr *cr, ulong cause)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_DISCONNECT;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND INFORMATION
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_information(queue_t *q, struct cr *cr)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_INFORMATION;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND NOTIFY
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_notify(queue_t *q, struct cr *cr, uchar *opt_ptr, size_t opt_len)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_NOTIFY;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND PROGRESS
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_progress(queue_t *q, struct cr *cr, ulong indicator)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_PROGRESS;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND RELEASE
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_release(queue_t *q, struct cr *cr, ulong cause)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_RELEASE;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND RELEASE_COMPLETE
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_release_complete(queue_t *q, struct cr *cr, ulong cause)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_RELEASE_COMPLETE;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND RESUME
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_resume(queue_t *q, struct cr *cr)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_RESUME;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND SETUP
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_setup(queue_t *q, struct cr *cr)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_SETUP;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND SETUP ACKNOWLEDGE
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_setup_acknowledge(queue_t *q, struct cr *cr)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_SETUP_ACKNOWLEDGE;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND STATUS
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_status(queue_t *q, struct cr *cr, ulong cause)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_STATUS;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND STATUS ENQUIRY
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_status_enquiry(queue_t *q, struct cr *cr)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_STATUS_ENQUIRY;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  SEND SUSPEND
 *  -----------------------------------
 */
STATIC INLINE int
isdn_send_suspend(queue_t *q, struct cr *cr)
{
	isdn_msg_t msg = { 0, };
	ensure(cr && cr->fg.fg, return (-EFAULT));
	msg.cr = cr->cref;
	msg.mt = ISDN_MT_SUSPEND;
	return isdn_send_msg(q, cr, &msg);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE FUNCTIONS
 *
 *  =========================================================================
 */
#define ISDN_US_NULL				0
#define ISDN_US_CALL_INITIATED			1
#define ISDU_US_OVERLAP_SENDING			2
#define ISDN_US_OUTGOING_CALL_PROCEEDING	3
#define ISDN_US_CALL_DELIVERED			4
#define ISDN_US_CALL_PRESENT			6
#define ISDN_US_CALL_RECEIVED			7
#define ISDN_US_CONNECT_REQUEST			8
#define ISDN_US_INCOMING_CALL_PROCEEDING	9
#define ISDN_US_ACTIVE				10
#define ISDN_US_DISCONNECT_REQUEST		11
#define ISDN_US_DISCONNECT_INDICATION		12
#define ISDN_US_SUSPEND_REQUEST			15
#define ISDN_US_RESUME_REQUEST			17
#define ISDN_US_RELEASE_REQUEST			19
#define ISDN_US_OVERLAP_RECEIVING		25

#define ISDN_NS_NULL				0
#define ISDN_NS_CALL_INITIATED			1
#define ISDN_NS_OVERLAP_SENDING			2
#define ISDN_NS_OUTGOING_CALL_PROCEEDING	3
#define ISDN_NS_CALL_DELIVERED			4
#define ISDN_NS_CALL_PRESENT			6
#define ISDN_NS_INCOMING_CALL_PROCEEDING	9
#define ISDN_NS_ACTIVE				10
#define ISDN_NS_DISCONNECT_REQUEST		11
#define ISDN_NS_DISCONNECT_INDICATION		12
#define ISDN_NS_SUSPEND_REQUEST			15
#define ISDN_NS_RESUME_REQUEST			17
#define ISDN_NS_RELEASE_REQUEST			19
#define ISDN_NS_CALL_ABORT			22
#define ISDN_NS_OVERLAP_RECEIVING		25

#define ISDN_GS_NULL				0
#define ISDN_GS_RESTART_REQUEST			1
#define ISDN_GS_RESTART				2

/*
 *  ========================================================================
 *
 *  PROTOCOL STATE MACHINE FUNCTIONS
 *
 *  ========================================================================
 */
STATIC isdn_opt_conf_tg_t itu_tg_u_config_defaults = {
};
STATIC isdn_opt_conf_fg_t itu_fg_u_config_defaults = {
	t301:(180 * HZ),		/* T301 */
	t302:(15 * HZ),			/* T302 */
	t303:(4 * HZ),			/* T303 */
	t304:(30 * HZ),			/* T304 */
	t305:(30 * HZ),			/* T305 */
	t306:(0 * HZ),			/* T306 */
	t307:(0 * HZ),			/* T307 */
	t308:(4 * HZ),			/* T308 */
	t309:(90 * HZ),			/* T309 */
	t310:(120 * HZ),		/* T310 */
	t312:(0 * HZ),			/* T312 */
	t313:(4 * HZ),			/* T313 */
	t314:(4 * HZ),			/* T314 */
	t316:(120 * HZ),		/* T316 */
	t317:(60 * HZ),			/* T317 */
	t318:(4 * HZ),			/* T318 */
	t319:(4 * HZ),			/* T319 */
	t320:(0 * HZ),			/* T320 */
	t321:(30 * HZ),			/* T321 */
	t322:(4 * HZ),			/* T322 */
};
STATIC isdn_opt_conf_eg_t itu_eg_u_config_defaults = {
};
STATIC isdn_opt_conf_xg_t itu_xg_u_config_defaults = {
};
STATIC isdn_opt_conf_tg_t itu_tg_n_config_defaults = {
};
STATIC isdn_opt_conf_fg_t itu_fg_n_config_defaults = {
	t301:(180 * HZ),		/* T301 */
	t302:(12 * HZ),			/* T302 */
	t303:(4 * HZ),			/* T303 */
	t304:(20 * HZ),			/* T304 */
	t305:(30 * HZ),			/* T305 */
	t306:(30 * HZ),			/* T306 */
	t307:(180 * HZ),		/* T307 */
	t308:(4 * HZ),			/* T308 */
	t309:(90 * HZ),			/* T309 */
	t310:(10 * HZ),			/* T310 */
	t312:(6 * HZ),			/* T312 */
	t313:(6 * HZ),			/* T313 */
	t314:(4 * HZ),			/* T314 */
	t316:(120 * HZ),		/* T316 */
	t317:(60 * HZ),			/* T317 */
	t318:(0 * HZ),			/* T318 */
	t319:(0 * HZ),			/* T319 */
	t320:(30 * HZ),			/* T320 */
	t321:(30 * HZ),			/* T321 */
	t322:(4 * HZ),			/* T322 */
};
STATIC isdn_opt_conf_eg_t itu_eg_n_config_defaults = {
};
STATIC isdn_opt_conf_xg_t itu_xg_n_config_defaults = {
};

STATIC isdn_opt_conf_tg_t ansi_tg_u_config_defaults = {
};
STATIC isdn_opt_conf_fg_t ansi_fg_u_config_defaults = {
	t301:(180 * HZ),		/* T301 */
	t302:(15 * HZ),			/* T302 */
	t303:(4 * HZ),			/* T303 */
	t304:(30 * HZ),			/* T304 */
	t305:(30 * HZ),			/* T305 */
	t306:(0 * HZ),			/* T306 */
	t307:(0 * HZ),			/* T307 */
	t308:(4 * HZ),			/* T308 */
	t309:(90 * HZ),			/* T309 */
	t310:(120 * HZ),		/* T310 */
	t312:(0 * HZ),			/* T312 */
	t313:(4 * HZ),			/* T313 */
	t314:(4 * HZ),			/* T314 */
	t316:(120 * HZ),		/* T316 */
	t317:(60 * HZ),			/* T317 */
	t318:(4 * HZ),			/* T318 */
	t319:(4 * HZ),			/* T319 */
	t320:(0 * HZ),			/* T320 */
	t321:(30 * HZ),			/* T321 */
	t322:(4 * HZ),			/* T322 */
};
STATIC isdn_opt_conf_eg_t ansi_eg_u_config_defaults = {
};
STATIC isdn_opt_conf_xg_t ansi_xg_u_config_defaults = {
};
STATIC isdn_opt_conf_tg_t ansi_tg_n_config_defaults = {
};
STATIC isdn_opt_conf_fg_t ansi_fg_n_config_defaults = {
	t301:(180 * HZ),		/* T301 */
	t302:(12 * HZ),			/* T302 */
	t303:(4 * HZ),			/* T303 */
	t304:(20 * HZ),			/* T304 */
	t305:(30 * HZ),			/* T305 */
	t306:(30 * HZ),			/* T306 */
	t307:(180 * HZ),		/* T307 */
	t308:(4 * HZ),			/* T308 */
	t309:(90 * HZ),			/* T309 */
	t310:(10 * HZ),			/* T310 */
	t312:(6 * HZ),			/* T312 */
	t313:(6 * HZ),			/* T313 */
	t314:(4 * HZ),			/* T314 */
	t316:(120 * HZ),		/* T316 */
	t317:(60 * HZ),			/* T317 */
	t318:(0 * HZ),			/* T318 */
	t319:(0 * HZ),			/* T319 */
	t320:(30 * HZ),			/* T320 */
	t321:(30 * HZ),			/* T321 */
	t322:(4 * HZ),			/* T322 */
};
STATIC isdn_opt_conf_eg_t ansi_eg_n_config_defaults = {
};
STATIC isdn_opt_conf_xg_t ansi_xg_n_config_defaults = {
};

/*
 *  ------------------------------------------------------------------------
 *
 *  Timers
 *
 *  ------------------------------------------------------------------------
 */

enum { tall = 0, t301, t302, t303, t304, t305, t306, t307, t308, t309, t310, t312, t313, t314, t316,
	t317,
	t318, t319, t320, t321, t322
};

#define ISDN_DECLARE_TIMER(__o,__t) \
STATIC int __o ## _ ## __t ## _timeout(struct __o *); \
STATIC void __o ## _ ## __t ## _expiry(caddr_t data) \
{ \
	__o ## _do_timeout(data, # __t, &((struct __o *)data)->timers. __t, &__o ## _ ## __t ## _timeout, & __o ## _ ## __t ## _expiry); \
} \
STATIC void __o ## _stop_timer_ ## __t (struct __o * __o) \
{ \
	__o ## _stop_timer(__o, # __t, &__o->timers. __t); \
} \
STATIC void __o ## _start_timer_ ## __t (struct __o * __o) \
{ \
	__o ## _start_timer(__o, # __t, &__o->timers. __t, &__o ## _ ## __t ## _expiry, __o->fg.fg->config. __t); \
} \

/*
 *  -------------------------------------------------------------------------
 *
 *  Channel Timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
cr_do_timeout(caddr_t data, const char *timer, ulong *timeo, int (to_fnc) (struct cr *),
	      void (*exp_fnc) (caddr_t))
{
	struct cr *cr = (struct cr *) data;
	if (xchg(timeo, 0)) {
		if (spin_trylock(&cr->lock)) {
			printd(("%s: %p: %s timeout at %lu\n", DRV_NAME, cr, timer, jiffies));
			switch (to_fnc(cr)) {
			default:
			case QR_DONE:
				cr_put(cr);
				spin_unlock(&cr->lock);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&cr->lock);
		} else
			printd(("%s: %p: %s timeout collision at %lu\n", DRV_NAME, cr, timer,
				jiffies));
		/* 
		   back off timer one tick */
		*timeo = timeout(exp_fnc, data, 100);
	}
}
STATIC INLINE void
cr_stop_timer(struct cr *cr, const char *timer, ulong *timeo)
{
	ulong to;
	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", DRV_NAME, cr, timer, jiffies));
		cr_put(cr);
	}
	return;
}
STATIC INLINE void
cr_start_timer(struct cr *cr, const char *timer, ulong *timeo, void (*exp_fnc) (caddr_t), ulong val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", DRV_NAME, cr, timer, val * 1000 / HZ,
		jiffies));
	*timeo = timeout(exp_fnc, (caddr_t) cr, val);
}

ISDN_DECLARE_TIMER(cr, t301);
ISDN_DECLARE_TIMER(cr, t302);
ISDN_DECLARE_TIMER(cr, t303);
ISDN_DECLARE_TIMER(cr, t304);
ISDN_DECLARE_TIMER(cr, t305);
ISDN_DECLARE_TIMER(cr, t306);
ISDN_DECLARE_TIMER(cr, t307);
ISDN_DECLARE_TIMER(cr, t308);
ISDN_DECLARE_TIMER(cr, t309);
ISDN_DECLARE_TIMER(cr, t310);
ISDN_DECLARE_TIMER(cr, t312);
ISDN_DECLARE_TIMER(cr, t313);
ISDN_DECLARE_TIMER(cr, t314);
ISDN_DECLARE_TIMER(cr, t316);
ISDN_DECLARE_TIMER(cr, t317);
ISDN_DECLARE_TIMER(cr, t318);
ISDN_DECLARE_TIMER(cr, t319);
ISDN_DECLARE_TIMER(cr, t320);
ISDN_DECLARE_TIMER(cr, t321);
ISDN_DECLARE_TIMER(cr, t322);

STATIC INLINE void
__cr_timer_stop(struct cr *cr, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t301:
		cr_stop_timer_t301(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t302:
		cr_stop_timer_t302(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t303:
		cr_stop_timer_t303(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t304:
		cr_stop_timer_t304(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t305:
		cr_stop_timer_t305(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t306:
		cr_stop_timer_t306(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t307:
		cr_stop_timer_t307(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t308:
		cr_stop_timer_t308(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t309:
		cr_stop_timer_t309(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t310:
		cr_stop_timer_t310(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t312:
		cr_stop_timer_t312(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t313:
		cr_stop_timer_t313(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t314:
		cr_stop_timer_t314(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t316:
		cr_stop_timer_t316(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t317:
		cr_stop_timer_t317(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t318:
		cr_stop_timer_t318(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t319:
		cr_stop_timer_t319(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t320:
		cr_stop_timer_t320(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t321:
		cr_stop_timer_t321(cr);
		if (single)
			break;
		/* 
		   fall through */
	case t322:
		cr_stop_timer_t322(cr);
		if (single)
			break;
		/* 
		   fall through */
	}
}

STATIC INLINE void
cr_timer_stop(struct cr *cr, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&cr->lock, flags);
	{
		__cr_timer_stop(cr, t);
	}
	spin_unlock_irqrestore(&cr->lock, flags);
}

STATIC INLINE void
cr_timer_start(struct cr *cr, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&cr->lock, flags);
	{
		__cr_timer_stop(cr, t);
		switch (t) {
		case t301:
			cr_start_timer_t301(cr);
			break;
		case t302:
			cr_start_timer_t302(cr);
			break;
		case t303:
			cr_start_timer_t303(cr);
			break;
		case t304:
			cr_start_timer_t304(cr);
			break;
		case t305:
			cr_start_timer_t305(cr);
			break;
		case t306:
			cr_start_timer_t306(cr);
			break;
		case t307:
			cr_start_timer_t307(cr);
			break;
		case t308:
			cr_start_timer_t308(cr);
			break;
		case t309:
			cr_start_timer_t309(cr);
			break;
		case t310:
			cr_start_timer_t310(cr);
			break;
		case t312:
			cr_start_timer_t312(cr);
			break;
		case t313:
			cr_start_timer_t313(cr);
			break;
		case t314:
			cr_start_timer_t314(cr);
			break;
		case t316:
			cr_start_timer_t316(cr);
			break;
		case t317:
			cr_start_timer_t317(cr);
			break;
		case t318:
			cr_start_timer_t318(cr);
			break;
		case t319:
			cr_start_timer_t319(cr);
			break;
		case t320:
			cr_start_timer_t320(cr);
			break;
		case t321:
			cr_start_timer_t321(cr);
			break;
		case t322:
			cr_start_timer_t322(cr);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&cr->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ISDN State Machines
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  ISDN Receive Message Functions
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  ALERTING
 *  -----------------------------------
 *  Channel identification information element is mandatory if this is the first message in response to a SETUP,
 *  unless the user accepts the B-channel indicated in the SETUP message.
 *
 *  If this is the first message in response to a SETUP message, we will issue a CC_SETUP_CON to indicated the
 *  B-channel before the CC_ALERTING_IND is issued.
 */
STATIC int
isdn_recv_alerting(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	if (!cr)
		return (-EPROTO);	/* unexpected message */
	switch (cr_get_c_state(cr)) {
	case N0_NULL:
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	case U1_CALL_INITIATED:
		cr_timer_stop(cr, t303);
		break;
	case U2_OVERLAP_SENDING:
		cr_timer_stop(cr, t304);
		break;
	case U3_OUTGOING_CALL_PROCEEDING:
		cr_timer_stop(cr, t310);
		break;
	default:
		seldom();
		return (QR_DONE);	/* ignore */
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WCON_SREQ:
		if ((err = cc_setup_con(q, cr, m)))
			return (err);
		cr_set_i_state(cr, cr->cpc.cc, CCS_WREQ_MORE);
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WIND_PROCEED:
	case CCS_WIND_ALERTING:
		if ((err = cc_alerting_ind(q, cr, m)) < 0)
			return (err);
		cr_set_i_state(cr, cr->cpc.cc, CCS_WIND_PROGRESS);
	case CCS_WIND_PROGRESS:
		break;
	case CCS_IDLE:
		break;
	default:
		swerr();
		return (-EFAULT);
	}
	switch (cr_get_c_state(cr)) {
	case U1_CALL_INITIATED:
	case U2_OVERLAP_SENDING:
	case U3_OUTGOING_CALL_PROCEEDING:
		cr_set_c_state(cr, U4_CALL_DELIVERED);
		break;
	default:
		swerr();
		return (-EFAULT);
	}
	return (QR_DONE);
}

/*
 *  CALL PROCEEDING
 *  -----------------------------------
 *  Channel identification information element is mandatory in the network to user direction if this message is the
 *  first message in response to a SETUP.  Channel identification information element is mandatory in the user to
 *  network direction if this message is the first message in response to a SETUP, unless the user accepts the
 *  B-channel indicated in the SETUP message.
 *
 *  If this message is the first message in response to a SETUP message, we have to issue a CC_SETUP_CON before
 *  issuing the CC_PROCEEDING_IND.  The CC_SETUP_CON will include the necessary B-channel channel identification.
 */
STATIC int
isdn_recv_call_proceeding(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	case U1_CALL_INITIATED:
		cr_timer_stop(cr, t303);
		break;
	case U2_OVERLAP_SENDING:
		cr_timer_stop(cr, t304);
		break;
	default:
		rare();
		return (QR_DONE);	/* ignore */
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WCON_SREQ:
		if ((err = cc_setup_con(q, cr, m)) < 0)
			return (err);
		cr_set_i_state(cr, cr->cpc.cc, CCS_WIND_PROCEED);
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WIND_PROCEED:
		if ((err = cc_proceeding_ind(q, cr, m)) < 0)
			return (err);
		cr_set_i_state(cr, cr->cpc.cc, CCS_WIND_ALERTING);
	case CCS_WIND_ALERTING:
		break;
	case CCS_IDLE:
		break;
	default:
		swerr();
		return (-EFAULT);
	}
	switch (cr_get_c_state(cr)) {
	case U1_CALL_INITIATED:
	case U2_OVERLAP_SENDING:
		cr_timer_start(cr, t310);
		cr_set_c_state(cr, U3_OUTGOING_CALL_PROCEEDING);
		break;
	default:
		swerr();
		return (-EFAULT);
	}
	return (QR_DONE);
}

/*
 *  CONNECT
 *  -----------------------------------
 */
STATIC int
isdn_recv_connect(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	case U1_CALL_INITIATED:
		cr_timer_stop(cr, t303);
		break;
	case U2_OVERLAP_SENDING:
		cr_timer_stop(cr, t304);
		break;
	case U3_OUTGOING_CALL_PROCEEDING:
		cr_timer_stop(cr, t310);
		break;
	default:
		rare();
		return (QR_DONE);	/* ignore */
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WCON_SREQ:
		if ((err = cc_setup_con(q, cr, m)))
			goto error;
		cr_set_i_state(cr, cr->cpc.cc, CCS_WIND_CONNECT);
		/* 
		   fall through */
	case CCS_WIND_MORE:
	case CCS_WREQ_INFO:
	case CCS_WIND_PROCEED:
	case CCS_WIND_ALERTING:
	case CCS_WIND_PROGRESS:
	case CCS_WIND_CONNECT:
		if ((err = cc_connect_ind(q, cr, m)) < 0)
			goto error;
		cr_set_i_state(cr, cr->cpc.cc, CCS_CONNECTED);
		/* 
		   fall through */
	case CCS_CONNECTED:
		break;
	case CCS_IDLE:
		break;
	default:
		goto efault;
	}
	switch (cr_get_c_state(cr)) {
	case U1_CALL_INITIATED:
	case U2_OVERLAP_SENDING:
	case U3_OUTGOING_CALL_PROCEEDING:
		if (cr->fg.fg->proto.popt & ISDN_POPT_ACK)
			if ((err = isdn_send_connect_acknowledge(q, cr, NULL, 0)) < 0)
				goto error;
		cr_set_c_state(cr, U10_ACTIVE);
		return (QR_DONE);
	}
      efault:
	swerr();
	return (-EFAULT);
      error:
	return (err);
}

/*
 *  CONNECT ACKNOWLEDGEMENT
 *  -----------------------------------
 */
STATIC int
isdn_recv_connect_acknowledge(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);
	case U8_CONNECT_REQUEST:
		cr_timer_stop(cr, t313);
		if ((err = cc_setup_complete_ind(q, cr, m)) < 0)
			goto error;
		cr_set_c_state(cr, U10_ACTIVE);
		break;
	}
	return (QR_DONE);
      error:
	return (err);
}

/*
 *  DISCONNECT
 *  -----------------------------------
 */
STATIC int
isdn_recv_disconnect(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	case U6_CALL_PRESENT:
		break;
	case U11_DISCONNECT_REQUEST:
		cr_timer_stop(cr, t305);
		break;
	case U15_SUSPEND_REQUEST:
		cr_timer_stop(cr, t319);
		break;
	case U1_CALL_INITIATED:
	case U12_DISCONNECT_INDICATION:
	case U17_RESUME_REQUEST:
	case U19_RELEASE_REQUEST:
		goto confused;
	default:
	case U2_OVERLAP_SENDING:
	case U3_OUTGOING_CALL_PROCEEDING:
	case U4_CALL_DELIVERED:
	case U7_CALL_RECEIVED:
	case U8_CONNECT_REQUEST:
	case U9_INCOMING_CALL_PROCEEDING:
	case U10_ACTIVE:
	case U25_OVERLAP_RECEIVING:
		/* 
		   any state except 0, 1, 6, 11, 12, 15, 17, 19 */
		cr_timer_stop(cr, tall);
		break;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_IDLE:
		break;
	default:
	case CCS_WRES_SIND:
	case CCS_WREQ_INFO:
	case CCS_WIND_ALERTING:
	case CCS_WIND_PROGRESS:
	case CCS_WIND_CONNECT:
	case CCS_WCON_SREQ:
	case CCS_WIND_INFO:
	case CCS_WREQ_ALERTING:
	case CCS_WREQ_PROGRESS:
	case CCS_WREQ_CONNECT:
	case CCS_CONNECTED:
	case CCS_WCON_SUSREQ:
		if ((err = cc_disconnect_ind(q, cr, m)) < 0)
			goto error;
		cr_set_i_state(cr, cr->cpc.cc, CCS_WIND_RELEASE);
		break;
	case CCS_WIND_RELEASE:
		assure(cr_get_c_state(cr) == U11_DISCONNECT_REQUEST);
		cr_set_i_state(cr, cr->cpc.cc, CCS_WCON_RELREQ);
		break;
	}
	switch (cr_get_c_state(cr)) {
	case U11_DISCONNECT_REQUEST:
		if ((err = isdn_send_release(q, cr, m->cause)) < 0)
			goto error;
		cr_timer_start(cr, t308);
		cr_set_c_state(cr, U19_RELEASE_REQUEST);
		return (QR_DONE);
	case U2_OVERLAP_SENDING:
	case U3_OUTGOING_CALL_PROCEEDING:
	case U4_CALL_DELIVERED:
	case U6_CALL_PRESENT:
	case U25_OVERLAP_RECEIVING:
	case U9_INCOMING_CALL_PROCEEDING:
	case U7_CALL_RECEIVED:
	case U8_CONNECT_REQUEST:
	case U10_ACTIVE:
	case U15_SUSPEND_REQUEST:
		cr_set_c_state(cr, U12_DISCONNECT_INDICATION);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
      confused:
	/* 
	   unexpected message */
	if (cr->fg.fg->proto.popt & ISDN_POPT_ENQ) {
		if ((err = isdn_send_status_enquiry(q, cr)) < 0)
			goto error;
	} else {
		if (cr->fg.fg->proto.popt & ISDN_POPT_CAUSE) {
			if ((err = isdn_send_status(q, cr, 101)) < 0)	/* cause No. 101 */
				goto error;
		} else {
			if ((err = isdn_send_status(q, cr, 98)) < 0)	/* cause No. 98 */
				goto error;
		}
	}
	/* 
	   remain in current state */
	return (-ENOPROTOOPT);
      error:
	return (err);
}

/*
 *  INFORMATION
 *  -----------------------------------
 */
STATIC int
isdn_recv_information(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	case U25_OVERLAP_RECEIVING:
		break;
	case U1_CALL_INITIATED:
	case U6_CALL_PRESENT:
	case U17_RESUME_REQUEST:
	case U19_RELEASE_REQUEST:
		goto confused;
	default:
		break;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WCON_SREQ:
		if ((err = cc_setup_con(q, cr, m)))
			goto error;
		cr_set_i_state(cr, cr->cpc.cc, CCS_WREQ_MORE);
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
		if ((err = cc_information_ind(q, cr, m)))
			goto error;
		if (m->sc)
			cr_set_i_state(cr, cr->cpc.cc, CCS_WIND_PROCEED);
		else
			cr_set_i_state(cr, cr->cpc.cc, CCS_WIND_INFO);
	case CCS_WIND_PROCEED:
		break;
	case CCS_IDLE:
		break;
	default:
		rare();
		return (QR_DONE);	/* ignore */
	}
	switch (cr_get_c_state(cr)) {
	case U25_OVERLAP_RECEIVING:
		cr_timer_start(cr, t302);
	default:
		/* 
		   any state except 0, 1, 6, 17, 19, 25 */
		/* 
		   remain in current state */
		rare();
		return (QR_DONE);	/* ignore */
	case U0_NULL:
	case U1_CALL_INITIATED:
	case U6_CALL_PRESENT:
	case U17_RESUME_REQUEST:
	case U19_RELEASE_REQUEST:
		break;
	}
	swerr();
	return (-EFAULT);
      confused:
	/* 
	   unexpected message */
	if (cr->fg.fg->proto.popt & ISDN_POPT_ENQ) {
		if ((err = isdn_send_status_enquiry(q, cr)) < 0)
			goto error;
	} else {
		if (cr->fg.fg->proto.popt & ISDN_POPT_CAUSE) {
			if ((err = isdn_send_status(q, cr, 101)) < 0)	/* cause No. 101 */
				goto error;
		} else {
			if ((err = isdn_send_status(q, cr, 98)) < 0)	/* cause No. 98 */
				goto error;
		}
	}
	/* 
	   remain in current state */
	return (-ENOPROTOOPT);
      error:
	return (err);
}

/*
 *  NOTIFY
 *  -----------------------------------
 */
STATIC int
isdn_recv_notify(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	if (!(m->cflags & ISDN_IEF_NI))
		goto enxio;
	if (!isdn_check_ni(q, m))
		goto einval;
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);
	case U10_ACTIVE:
		if ((err = cc_notify_ind(q, cr, m)) < 0)
			goto error;
		cr_set_c_state(cr, U10_ACTIVE);
		break;
	case U11_DISCONNECT_REQUEST:
		/* 
		   for further study */
		break;
	}
	return (QR_DONE);
      enxio:			/* missing mandatory ie */
	return (-ENXIO);
      einval:			/* mandatory ie in error */
	return (-EINVAL);
      error:
	return (err);

}

/*
 *  PROGRESS
 *  -----------------------------------
 */
STATIC int
isdn_recv_progress(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	if (!(m->cflags & ISDN_IEF_PI))
		goto enxio;
	if (!isdn_check_pi(q, m))
		goto einval;
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	case U2_OVERLAP_SENDING:
		if (m->pi.pd == ISDN_PI_INTERWORKING)
			cr_timer_stop(cr, t304);
		break;
	case U3_OUTGOING_CALL_PROCEEDING:
		cr_timer_stop(cr, t310);
		break;
	default:
		rare();
		return (QR_DONE);	/* ignore */
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WIND_PROGRESS:
		if ((err = cc_progress_ind(q, cr, m)) < 0)
			goto error;
	case CCS_IDLE:
		break;
	default:
		goto efault;
	}
	switch (cr_get_c_state(cr)) {
	case U2_OVERLAP_SENDING:
	case U3_OUTGOING_CALL_PROCEEDING:
		return (QR_DONE);
	}
      efault:
	swerr();
	return (-EFAULT);
      enxio:			/* mandatory ie missing */
	return (-ENXIO);
      einval:			/* mandatory ie in error */
	return (-EINVAL);
      error:
	return (err);
}

/*
 *  RELEASE
 *  -----------------------------------
 */
STATIC int
isdn_recv_release(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	if (!(m->cflags & ISDN_IEF_CAUSE))
		goto enxio;
	if (!isdn_check_cause(q, m))
		goto einval;
	switch (cr_get_c_state(cr)) {
	case N0_NULL:
	case U0_NULL:
		if ((err = isdn_send_release_complete(q, cr, 0)))
			return (err);
		return (QR_DONE);
	case U6_CALL_PRESENT:
		if ((err = cc_release_ind(q, cr, m)) < 0)
			goto error;
		if ((err = isdn_send_release_complete(q, cr, 0)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	case U11_DISCONNECT_REQUEST:
		cr_timer_stop(cr, t305);
		if ((err = cc_release_ind(q, cr, m)) < 0)
			goto error;
		if ((err = isdn_send_release_complete(q, cr, 0)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	case U12_DISCONNECT_INDICATION:
		if ((err = cc_release_ind(q, cr, m)) < 0)
			goto error;
		if ((err = isdn_send_release_complete(q, cr, 0)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	case U15_SUSPEND_REQUEST:
		cr_timer_stop(cr, t319);
		if ((err = cc_release_ind(q, cr, m)) < 0)
			goto error;
		if ((err = isdn_send_release_complete(q, cr, 0)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	case U1_CALL_INITIATED:
	case U17_RESUME_REQUEST:
	case U19_RELEASE_REQUEST:
		goto outstate;
	default:
		/* 
		   any state except 0, 1, 6, 11, 12, 15, 17, 19 */
		cr_timer_stop(cr, tall);
		if ((err = cc_release_ind(q, cr, m)) < 0)
			goto error;
		if ((err = isdn_send_release_complete(q, cr, 0)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	}
	swerr();
	return (-EFAULT);
      enxio:
	/* 
	   release without cause */
	if ((err = isdn_send_release_complete(q, cr, CC_CAUS_MISSING_MANDATORY_PARAMETER)) < 0)	/* cause 
												   No. 
												   96 
												 */
		goto error;
	if ((err = cc_release_ind(q, cr, m)) < 0)
		goto error;
	cr_set_c_state(cr, U0_NULL);
	isdn_free_cr(cr);
	return (QR_DONE);
      einval:
	/* 
	   release without cause in error */
	if ((err = isdn_send_release_complete(q, cr, CC_CAUS_INVALID_MANDATORY_PARAMETER)) < 0)	/* cause 
												   No. 
												   100 
												 */
		goto error;
	if ((err = cc_release_ind(q, cr, m)) < 0)
		goto error;
	cr_set_c_state(cr, U0_NULL);
	isdn_free_cr(cr);
	return (QR_DONE);
      outstate:
	fixme(("Are you sure?\n"));
	return (-EFAULT);
      error:
	return (err);
}

/*
 *  RELEASE COMPLETE
 *  -----------------------------------
 */
STATIC int
isdn_recv_release_complete(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	if (!cr)
		return (QR_DONE);	/* ignore */
	switch (cr_get_c_state(cr)) {
	case N0_NULL:
	case U0_NULL:
		return (QR_DONE);	/* ignore */
	case U1_CALL_INITIATED:
		cr_timer_stop(cr, t303);
		if ((err = cc_reject_ind(q, cr, m)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	case U19_RELEASE_REQUEST:
		cr_timer_stop(cr, t308);
		if ((err = cc_release_con(q, cr, m)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	default:
		/* 
		   any state except 0, 1, 19 */
		cr_timer_stop(cr, tall);
		if ((err = cc_release_ind(q, cr, m)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	}
	swerr();
	return (-EFAULT);
      error:
	return (err);
}

/*
 *  RESUME
 *  -----------------------------------
 */
STATIC int
isdn_recv_resume(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	switch (cr_get_c_state(cr)) {
	case N0_NULL:
		if ((err = cc_resume_ind(q, cr, m)) < 0)
			goto error;
		cr_set_c_state(cr, N17_RESUME_REQUEST);
		break;
	case U0_NULL:
		return (-EPROTO);
	}
	swerr();
	return (-EFAULT);
      error:
	return (err);
}

/*
 *  RESUME ACKNOWLEDGE
 *  -----------------------------------
 */
STATIC int
isdn_recv_resume_acknowledge(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	if (!(m->cflags & ISDN_IEF_CI))
		goto enxio;
	if (!isdn_check_ci(q, m))
		goto einval;
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);
	case U17_RESUME_REQUEST:
		cr_timer_stop(cr, t318);
		if ((err = cc_resume_con(q, cr, m)) < 0)	/* B chan */
			goto error;
		cr_set_c_state(cr, U10_ACTIVE);
		break;
	}
	return (QR_DONE);
      enxio:			/* mandatory ie missing */
	return (-ENXIO);
      einval:			/* mandatory ie in error */
	return (-EINVAL);
      error:
	return (err);
}

/*
 *  RESUME REJECT
 *  -----------------------------------
 */
STATIC int
isdn_recv_resume_reject(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	if (!(m->cflags & ISDN_IEF_CAUSE))
		goto enxio;
	if (!isdn_check_cause(q, m))
		goto einval;
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);
	case U17_RESUME_REQUEST:
		cr_timer_stop(cr, t318);
		if ((err = cc_resume_con(q, cr, m)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	}
	return (QR_DONE);
      enxio:			/* mandatory ie missing */
	return (-ENXIO);
      einval:			/* mandatory ie in error */
	return (-EINVAL);
      error:
	return (err);
}

/*
 *  SETUP
 *  -----------------------------------
 */
STATIC int
isdn_recv_setup(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	switch (cr_get_c_state(cr)) {
	case N0_NULL:
		if ((err = cc_setup_ind(q, cr, m)) < 0)	/* with user selected B channel */
			goto error;
		cr_set_c_state(cr, N1_CALL_INITIATED);
		break;
	case U0_NULL:
		if (m->cflags & ISDN_IEF_BC) {
			if (isdn_check_bc(q, m)) {
				if ((err = cc_setup_ind(q, cr, m)) < 0)	/* with network selected B
									   channel */
					goto error;
				cr_set_c_state(cr, U6_CALL_PRESENT);
			} else {
				/* 
				   mandtory ie error */
				if ((err = isdn_send_release_complete(q, cr, CC_CAUS_INVALID_MANDATORY_PARAMETER)) < 0)	/* cause 
															   No. 
															   100 
															 */
					goto error;
				cr_set_c_state(cr, U0_NULL);
			}
		} else {
			/* 
			   missing mandatory ie */
			if ((err = isdn_send_release_complete(q, cr, CC_CAUS_MISSING_MANDATORY_PARAMETER)) < 0)	/* cause 
														   No. 
														   96 
														 */
				goto error;
			cr_set_c_state(cr, U0_NULL);
		}
		break;
	default:
		/* 
		   discard */
		/* 
		   remain in current state */
		break;
	}
	swerr();
	return (-EFAULT);
      error:
	return (err);
}

/*
 *  SETUP ACKNOWLEDGE
 *  -----------------------------------
 */
STATIC int
isdn_recv_setup_acknowledge(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	case U1_CALL_INITIATED:
		cr_timer_stop(cr, t303);
		cc_more_info_ind(q, cr, m);
		cr_timer_start(cr, t304);
		cr_set_c_state(cr, U2_OVERLAP_SENDING);
		break;
	}
	return (QR_DONE);
}

/*
 *  STATUS
 *  -----------------------------------
 */
STATIC int
isdn_recv_status(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	if (!(m->cflags & ISDN_IEF_CAUSE) || !(m->cflags & ISDN_IEF_CS))
		goto enxio;
	if (!isdn_check_cause(q, m) || !isdn_check_cs(q, m))
		goto einval;
	switch (cr_get_c_state(cr)) {
	case N0_NULL:
		if (m->cs.call_state != 0) {
			if (cr->fg.fg->proto.popt & ISDN_POPT_REL) {
				isdn_send_release_complete(q, cr, 101);	/* cause 101 */
				cr_set_c_state(cr, N0_NULL);
			} else {
				isdn_send_release(q, cr, 101);	/* cause 101 */
				cr_timer_start(cr, t308);
				cr_set_c_state(cr, N19_RELEASE_REQUEST);
			}
		}
		break;
	case U0_NULL:
		if (m->cs.call_state != 0) {
			if (cr->fg.fg->proto.popt & ISDN_POPT_REL) {
				isdn_send_release_complete(q, cr, 101);	/* cause 101 */
				cr_set_c_state(cr, U0_NULL);
			} else {
				isdn_send_release(q, cr, 101);	/* cause 101 */
				cr_timer_start(cr, t308);
				cr_set_c_state(cr, U19_RELEASE_REQUEST);
			}
		}
		break;
	case U19_RELEASE_REQUEST:
		if (m->cs.call_state != 0) {
			cr_set_c_state(cr, U19_RELEASE_REQUEST);
		} else {
			if ((err = cc_status_ind(q, cr, m)) < 0)
				goto error;
			cr_set_c_state(cr, U0_NULL);
			isdn_free_cr(cr);
		}
		break;
	default:
		if (m->cs.call_state != 0) {
			if (0) {
				/* 
				   compatible state: TODO: need to fix up timers and call state */
			} else {
				if ((err = cc_status_ind(q, cr, m)) < 0)
					goto error;
				if ((err = isdn_send_disconnect(q, cr, m->cause)) < 0)
					goto error;
				cr_timer_start(cr, t305);
				cr_set_c_state(cr, U11_DISCONNECT_REQUEST);
			}
		} else {
			if ((err = cc_status_ind(q, cr, m)) < 0)
				goto error;
			cr_set_c_state(cr, U0_NULL);
			isdn_free_cr(cr);
		}
		break;
	}
	swerr();
	return (-EFAULT);
      enxio:			/* mandatory ie missing */
	return (-ENXIO);
      einval:			/* mandatory ie in error */
	return (-EINVAL);
      error:
	return (err);
}

/*
 *  STATUS ENQUIRY
 *  -----------------------------------
 */
STATIC int
isdn_recv_status_enquiry(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	ulong cause = 0;		/* FIXME */
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	default:
		if ((err = isdn_send_status(q, cr, cause)) < 0)
			goto error;
		/* 
		   remain in current state */
		break;
	}
	return (QR_DONE);
      error:
	return (err);
}

/*
 *  SUSPEND
 *  -----------------------------------
 */
STATIC int
isdn_recv_suspend(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	}
	return (QR_DONE);
}

/*
 *  SUSPEND ACKNOWLEDGE
 *  -----------------------------------
 */
STATIC int
isdn_recv_suspend_acknowledge(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	case U15_SUSPEND_REQUEST:
		cr_timer_stop(cr, t319);
		if ((err = cc_suspend_con(q, cr, m)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
		break;
	}
	return (QR_DONE);
      error:
	return (err);
}

/*
 *  SUSPEND REJECT
 *  -----------------------------------
 */
STATIC int
isdn_recv_suspend_reject(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	case U15_SUSPEND_REQUEST:
		cr_timer_stop(cr, t319);
		if ((err = cc_suspend_con(q, cr, m)) < 0)
			goto error;
		cr_set_c_state(cr, U10_ACTIVE);
		break;
	}
	return (QR_DONE);
      error:
	return (err);
}

/*
 *  CONGESTION CONTROL
 *  -----------------------------------
 */
STATIC int
isdn_recv_congestion_control(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	}
	return (QR_DONE);
}

/*
 *  USER INFORMATION
 *  -----------------------------------
 */
STATIC int
isdn_recv_user_information(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	}
	return (QR_DONE);
}

/*
 *  RESTART
 *  -----------------------------------
 */
STATIC int
isdn_recv_restart(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	}
	return (QR_DONE);
}

/*
 *  RESTART ACKNOWLEDGE
 *  -----------------------------------
 */
STATIC int
isdn_recv_restart_acknowledge(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	fixme(("Process message\n"));
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		return (-EPROTO);	/* unexpected message */
	}
	return (QR_DONE);
}

/*
 *  UNRECOGNIZED MESSAGE
 *  -----------------------------------
 */
STATIC int
isdn_recv_unrecognized_message(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	int err;
	fixme(("Process message\n"));
	if (cr) {
		switch (cr_get_c_state(cr)) {
		case U0_NULL:
			return (-EPROTO);	/* unexpected message */
		default:
			/* 
			   any state except 0 */
			if (cr->fg.fg->proto.popt & ISDN_POPT_ENQ) {
				if ((err = isdn_send_status_enquiry(q, cr)) < 0)
					goto error;
			} else {
				if (cr->fg.fg->proto.popt & ISDN_POPT_CAUSE) {
					if ((err = isdn_send_status(q, cr, CC_CAUS_MESSAGE_TYPE_NOT_IMPLEMENTED)) < 0)	/* cause 
															   No. 
															   97 
															 */
						goto error;
				} else {
					if ((err = isdn_send_status(q, cr, 98)) < 0)	/* cuase
											   No. 98 */
						goto error;
				}
			}
			/* 
			   remain in current state */
			break;
		}
	} else {
		if (m->crf != 0x02) {
			/* 
			   global call reference */
		} else {
			/* 
			   dummy call reference */
		}
		/* 
		   ignore for now */
	}
	return (QR_DONE);
      error:
	return (err);
}

/*
 *  UNEXPECTED MESSAGE
 *  -----------------------------------
 */
STATIC int
isdn_recv_unexpected_message(queue_t *q, struct cr *cr, isdn_msg_t * m)
{
	struct dl *dl = DL_PRIV(q);
	int err;
	if (!cr && !(cr = isdn_alloc_cr(m->cr, NULL, dl->dc.dc->fg.fg, NULL, 0)))
		return (-ENOMEM);
	if (dl->dc.dc->fg.fg->proto.popt & ISDN_POPT_REL) {
		if ((err = isdn_send_release_complete(q, cr, CC_CAUS_UNEXPECTED_MESSAGE)))
			return (err);
	} else {
		if ((err = isdn_send_release(q, cr, CC_CAUS_UNEXPECTED_MESSAGE)))
			return (err);
		cr_timer_start(cr, t308);
		if (dl->dc.dc->fg.fg->proto.popt & ISDN_POPT_USER)
			cr_set_c_state(cr, U19_RELEASE_REQUEST);
		else
			cr_set_c_state(cr, N19_RELEASE_REQUEST);
	}
	return (QR_DONE);
}

/*
 *  ========================================================================
 *
 *  ISDN MESSAGE DECODING
 *
 *  ========================================================================
 */
/*
 *  PROTOCOL DISCRIMINATOR
 *  -----------------------------------
 */
STATIC int
unpack_pd(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	if (p < e + 1) {
		if (*p != 0x08)
			return (-EPROTO);
		return (1);
	}
	return (-EMSGSIZE);
}

/*
 *  CALL REFERENCE
 *  -----------------------------------
 */
STATIC int
unpack_cr(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 1) && (p < e + (len = *p++))) {
		if (len) {
			m->crf = *p >> 7;
			m->cr = *p++ & 0x7f;
			if (len > 1)
				m->cr |= *p++ << 7;
			if (len > 2)
				m->cr |= *p++ << 15;
			if (len > 3)
				m->cr |= *p++ << 23;
			if (len > 4)
				m->cr |= *p++ << 31;
		} else {
			/* 
			   dummy call reference */
			m->crf = 0x2;
			m->cr = 0;
		}
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  MESSAGE TYPE
 *  -----------------------------------
 */
STATIC int
unpack_mt(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	if (p < e + 1) {
		m->mt = *p & 0x7f;
		if (!(*p++ & 0x80))
			return (1);
		m->mt |= (*p & 0x7f) << 7;
		if (!(*p++ & 0x80))
			return (2);
		m->mt |= (*p & 0x7f) << 14;
		if (!(*p++ & 0x80))
			return (3);
		m->mt |= (*p & 0x7f) << 21;
		if (!(*p++ & 0x80))
			return (4);
		return (-EMSGSIZE);
	}
	return (-EMSGSIZE);
}

/*
 *  SHIFT
 *  -----------------------------------
 */
STATIC int
unpack_shift(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint codeset;
	p--;
	if ((codeset = (*p >> 0) & 0x7) < 4)
		return (-EPROTO);
	if ((m->codeset_lock = (*p >> 3) & 0x1)) {
		if (codeset <= m->codeset)
			return (-EPROTO);
		m->codeset_locked = codeset;
	}
	m->codeset = codeset;
	return (0);
}

/*
 *  BEARER CAPABILITY
 *  -----------------------------------
 */
STATIC int
unpack_bc(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CALL IDENTITY
 *  -----------------------------------
 */
STATIC int
unpack_cid(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 1) && (p <= e + (len = *p++))) {
		m->cid.ptr = p;
		m->cid.len = len;
		m->cflags |= ISDN_IEF_CID;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  CALL STATE
 *  -----------------------------------
 */
STATIC int
unpack_cs(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 1) && (p <= e + (len = *p++)) && (len == 1)) {
		m->cs.coding_standard = *p >> 6;
		m->cs.call_state = *p & 0x3f;
		m->cflags |= ISDN_IEF_CS;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  CALLED PARTY NUMBER
 *  -----------------------------------
 */
STATIC int
unpack_cdpn(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 2) && (p <= e + (len = *p++)) && (len > 1)) {
		m->cdpn.nplan = *p & 0x0f;
		m->cdpn.ntype = (*p++ >> 4) & 0x7;
		m->cdpn.ptr = p;
		m->cdpn.len = len - 1;
		m->cflags |= ISDN_IEF_CDPN;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  CALLED PARY SUBADDRESS
 *  -----------------------------------
 */
STATIC int
unpack_cdsa(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 2) && (p <= e + (len = *p++)) && (len > 1)) {
		m->cdsa.oe = (*p & 0x08) >> 3;
		m->cdsa.type = (*p++ & 0x70) >> 4;
		m->cdsa.ptr = p;
		m->cdsa.len = len - 1;
		m->cflags |= ISDN_IEF_CDSA;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  CALLING PARTY NUMBER
 *  -----------------------------------
 */
STATIC int
unpack_cgpn(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 2) && (p <= e + (len = *p++)) && (len > 1)) {
		uint hlen = 1;
		m->cgpn.nplan = *p & 0x0f;
		m->cgpn.ntype = (*p >> 4) & 0x7;
		if (!(*p++ & 0x80)) {
			m->cgpn.si = *p & 0x3;
			m->cgpn.pi = (*p++ & 0x60) >> 5;
			hlen++;
		} else {
			m->cgpn.si = 0x0;
			m->cgpn.pi = 0x2;
		}
		m->cgpn.ptr = p;
		m->cgpn.len = len - hlen;
		m->cflags |= ISDN_IEF_CGPN;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  CALLING PARY SUBADDRESS
 *  -----------------------------------
 */
STATIC int
unpack_cgsa(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 2) && (p <= e + (len = *p++)) && (len > 1)) {
		m->cgsa.oe = (*p & 0x08) >> 3;
		m->cgsa.type = (*p++ & 0x79) >> 4;
		m->cgsa.ptr = p;
		m->cgsa.len = len - 1;
		m->cflags |= ISDN_IEF_CGSA;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  CAUSE
 *  -----------------------------------
 */
STATIC int
unpack_cause(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CHANNEL IDENTIFICATION
 *  -----------------------------------
 */
STATIC int
unpack_ci(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len, pos = 0;
	if ((p < e + 2) && (p <= e + (len = *p++)) && (len > 0)) {
		pos++;
		m->ci.ichan = (*p & 0x3) >> 0;
		m->ci.dchan = (*p & 0x4) >> 2;
		m->ci.pref = (*p & 0x8) >> 3;
		m->ci.type = (*p & 0x2) >> 5;
		m->ci.idpres = (*p & 0x4) >> 6;
		pos++;
		p++;
		if (m->ci.idpres)
			do {
				if (len < pos)
					goto emsgsize;
				m->ci.iid = *p & 0x7f;
				pos++;
				if (!(*p++ & 0x80))
					break;
				if (len < pos)
					goto emsgsize;
				m->ci.iid |= (*p & 0x7f) << 7;
				pos++;
				if (!(*p++ & 0x80))
					break;
				if (len < pos)
					goto emsgsize;
				m->ci.iid |= (*p & 0x7f) << 14;
				pos++;
				if (!(*p++ & 0x80))
					break;
				if (len < pos)
					goto emsgsize;
				m->ci.iid |= (*p & 0x7f) << 21;
				pos++;
				if (!(*p++ & 0x80))
					goto emsgsize;
			} while (0);
		if (m->ci.type != 0 && len > pos) {
			uint slot = 0;
			if (len < pos)
				goto emsgsize;
			m->ci.cs.type = (*p & 0x0f) >> 0;
			m->ci.cs.nmap = (*p & 0x10) >> 4;
			m->ci.cs.stand = (*p & 0x60) >> 5;
			pos++;
			p++;
			if (m->ci.cs.nmap == 0) {
				do {
					if (len < pos)
						goto emsgsize;
					m->ci.chan[slot++] = *p & 0x7f;
				} while (pos++ && (*p++ & 0x80));
			} else {
				switch (m->ci.cs.type) {
				case 0x3:	/* B-channel units */
					m->ci.cs.slots = 0;
					m->ci.cs.slots <<= 8;
					pos++;
					m->ci.cs.slots |= *p++;
					m->ci.cs.slots <<= 8;
					pos++;
					m->ci.cs.slots |= *p++;
					m->ci.cs.slots <<= 8;
					pos++;
					m->ci.cs.slots |= *p++;
					if (len > pos) {
						m->ci.cs.slots <<= 8;
						pos++;
						m->ci.cs.slots |= *p++;
					}
					break;
				case 0x6:	/* H0-channel units */
					pos++;
					m->ci.cs.slots = *p++ & 0x1f;
					break;
				case 0x8:	/* H11-channel units */
				case 0x9:	/* H12-channel units */
					pos++;
					m->ci.cs.slots = *p++ & 0x01;
					break;
				default:
					return (-EPROTO);
				}
			}
		}
		m->cflags |= ISDN_IEF_CI;
		return (len + 1);
	}
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  CONGESTION LEVEL
 *  -----------------------------------
 */
STATIC int
unpack_cl(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	m->cl = *--p & 0x0f;
	m->cflags |= ISDN_IEF_CL;
	return (0);
}

/*
 *  DATA AND TIME
 *  -----------------------------------
 */
STATIC int
unpack_dt(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++)) && (len == 6)) {
		m->dt.yr = *p++;
		m->dt.mo = *p++;
		m->dt.dy = *p++;
		m->dt.hr = *p++;
		m->dt.mi = *p++;
		m->dt.se = *p++;
		m->cflags |= ISDN_IEF_DT;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  DISPLAY
 *  -----------------------------------
 */
STATIC int
unpack_disp(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		m->disp.ptr = p;
		m->disp.len = len;
		m->cflags |= ISDN_IEF_DISP;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  HIGH LAYER COMPATIBILITY
 *  -----------------------------------
 */
STATIC int
unpack_hlc(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len, pos = 0;
	if ((p < e) && (p <= e + (len = *p++))) {
		pos++;
		if (len < pos)
			goto emsgsize;
		m->hlc.pres = (*p >> 0) & 0x3;
		m->hlc.intr = (*p >> 2) & 0x7;
		m->hlc.stand = (*p >> 5) & 0x3;
		pos++;
		p++;
		if (len < pos)
			goto emsgsize;
		m->hlc.hlci = (*p >> 0) & 0x7f;
		if (!(*p++ & 0x80)) {
			pos++;
			if (len < pos)
				goto emsgsize;
			m->hlc.ehlci = (*p >> 0) & 0x7f;
		}
		m->cflags |= ISDN_IEF_HLC;
		return (len + 1);
	}
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  KEYPAD FACILITY
 *  -----------------------------------
 */
STATIC int
unpack_kpf(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		m->kpf.ptr = p;
		m->kpf.len = len;
		m->cflags |= ISDN_IEF_KPF;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  LOW LAYER COMPATIBILITY
 *  -----------------------------------
 */
STATIC int
unpack_llc(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	fixme(("Unpack this horrendous parameter\n"));
	return (-EFAULT);
}

/*
 *  MORE DATA
 *  -----------------------------------
 */
STATIC int
unpack_md(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	m->cflags |= ISDN_IEF_MD;
	return (0);
}

/*
 *  NETWORK SPECIFIC FACILITIES
 *  -----------------------------------
 */
STATIC int
unpack_nsf(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	/* 
	   there can be 4 of these in a message */
	uint len, i = m->nsf_numb;
	if (i > 3)
		goto eproto;
	if ((p < e) && (p <= e + (len = *p++))) {
		uint ilen;
		if (p <= e + (ilen = *p++) && ilen) {
			m->nsf[i].niplan = (*p >> 0) & 0x0f;
			m->nsf[i].nitype = (*p >> 4) & 0x07;
			p++;
			if (ilen > 1) {
				m->nsf[i].ni.ptr = p;
				m->nsf[i].ni.len = ilen - 1;
			}
		}
		if (p < e + ilen) {
			m->nsf[i].spec = *p++;
		}
		m->nsf_numb++;
		m->cflags |= ISDN_IEF_NSF;
		return (len + 1);
	}
	return (-EMSGSIZE);
      eproto:
	return (-EPROTO);
}

/*
 *  NOTIFICATION INDICATOR
 *  -----------------------------------
 */
#if 0
STATIC int
unpack_ni(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++)) && (len == 1)) {
		m->ni = *p++;
		m->cflags |= ISDN_IEF_NI;
		return (len + 1);
	}
	return (-EMSGSIZE);
}
#endif

/*
 *  PROGRESS INDICATOR
 *  -----------------------------------
 */
STATIC int
unpack_pi(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++)) && (len == 2)) {
		m->pi.loc = (*p >> 0) & 0x0f;
		m->pi.stand = (*p >> 5) & 0x03;
		p++;
		m->pi.pd = (*p >> 0) & 0x7f;
		m->cflags |= ISDN_IEF_PI;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  REPEAT INDICATOR
 *  -----------------------------------
 */
STATIC int
unpack_rp(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	m->rp = *--p & 0x0f;
	m->cflags |= ISDN_IEF_RP;
	return (0);
}

/*
 *  RESTART INDICATOR
 *  -----------------------------------
 */
STATIC int
unpack_ri(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 1) && (p <= e + (len = *p++)) && (len == 1)) {
		m->ri.clas = (*p >> 0) & 0x07;
		m->cflags |= ISDN_IEF_RI;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  SEGMENTED MESSAGE
 *  -----------------------------------
 */
STATIC int
unpack_seg(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 1) && (p <= e + (len = *p++)) && (len > 1)) {
		m->seg.rem = (*p >> 0) & 0x7f;
		m->seg.first = (*p >> 7) & 0x01;
		p++;
		m->seg.mt = (*p >> 0) & 0x7f;
		m->cflags |= ISDN_IEF_SEG;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  SENDING COMPLETE
 *  -----------------------------------
 */
STATIC int
unpack_sc(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	m->cflags |= ISDN_IEF_SC;
	return (0);
}

/*
 *  SIGNAL
 *  -----------------------------------
 */
STATIC int
unpack_sig(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 1) && (p <= e + (len = *p++)) && (len == 2)) {
		m->sig = *p;
		m->cflags |= ISDN_IEF_SIG;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  TRANSIT NETWORK SELECTOR
 *  -----------------------------------
 */
STATIC int
unpack_tns(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 1) && (p <= e + (len = *p++)) && (len > 0)) {
		m->tns.niplan = (*p >> 0) & 0x0f;
		m->tns.nitype = (*p >> 4) & 0x07;
		p++;
		if (len > 1) {
			m->tns.ni.ptr = p;
			m->tns.ni.len = len - 1;
		}
		m->cflags |= ISDN_IEF_TNS;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  USER-USER INFORMATION
 *  -----------------------------------
 */
STATIC int
unpack_uui(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 1) && (p <= e + (len = *p++)) && (len > 1)) {
		m->uui.pd = *p++;
		m->uui.ui.ptr = p;
		m->uui.ui.len = len - 1;
		m->cflags |= ISDN_IEF_UUI;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  Packet mode only
 */
/*
 *  CLOSED USER GROUP
 *  -----------------------------------
 */
STATIC int
unpack_cug(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e + 1) && (p <= e + (len = *p++)) && (len > 1)) {
		m->cug.cugi = (*p++ >> 0) & 0x07;
		m->cug.code.ptr = p;
		m->cug.code.len = len - 1;
		m->pflags |= ISDN_IEF_CUG;
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  END TO END TRANSIT DELAY
 *  -----------------------------------
 */
STATIC int
unpack_eetd(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Packet-mode only IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  INFORMATION RATE
 *  -----------------------------------
 */
STATIC int
unpack_ir(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Packet-mode only IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  PACKET LAYER BINARY PARAMETERS
 *  -----------------------------------
 */
STATIC int
unpack_plbp(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Packet-mode only IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  PACKET LAYER WINDOW SIZE
 *  -----------------------------------
 */
STATIC int
unpack_plws(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Packet-mode only IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  PACKET SIZE
 *  -----------------------------------
 */
STATIC int
unpack_ps(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Packet-mode only IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  REDIRECTING NUMBER
 *  -----------------------------------
 */
STATIC int
unpack_rdn(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Packet-mode only IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
	if ((p < e + 1) && (p <= e + (len = *p++)) && (len > 1)) {
		uint pos = 0;
		m->rdn.nplan = (*p >> 0) & 0x0f;
		m->rdn.ntype = (*p >> 4) & 0x07;
		pos++;
		if (p < e)
			goto emsgsize;
		if (!(*p++ & 0x80)) {
			m->rdn.scrn = (*p >> 0) & 0x03;
			m->rdn.pres = (*p >> 5) & 0x03;
			pos++;
			if (p < e)
				goto emsgsize;
			if (!(*p++ & 0x80)) {
				m->rdn.reason = (*p >> 0) & 0x0f;
				p++;
				pos++;
			}
		}
		if (len > pos) {
			m->rdn.dig.ptr = p;
			m->rdn.dig.len = len - pos;
		}
		m->pflags |= ISDN_IEF_RDN;
		return (len + 1);
	}
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  REVERSE CHARGING INDICATION
 *  -----------------------------------
 */
STATIC int
unpack_rci(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Packet-mode only IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  TRANSIT DELAY SELECTION AND INDICATION
 *  -----------------------------------
 */
STATIC int
unpack_tdsi(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Packet-mode only IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  RESERVED
 *  -----------------------------------
 */
STATIC int
unpack_res(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if (p[-1] & 0x80)
		return (0);
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Reserved IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  UNKNOWN IE
 *  -----------------------------------
 */
STATIC int
unpack_unknown(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if (p[-1] & 0x80)
		return (0);
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Unknown IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

/*
 *  ESCAPE
 *  -----------------------------------
 */
STATIC int
unpack_esc(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	uint len;
	if (p[-1] & 0x80)
		return (0);
	if ((p < e) && (p <= e + (len = *p++))) {
		ptrace(("%s: PROTO: Escape IE encountered\n", DRV_NAME));
		/* 
		   ignore */
		return (len + 1);
	}
	return (-EMSGSIZE);
}

STATIC int
unpack_ies(uint pvar, unsigned char *p, unsigned char *e, isdn_msg_t * m)
{
	int ret = 0, len = 0;
	while (p < e) {
		uint ie;
		len++;
		ie = *p++;
		switch (m->codeset) {
		case 0:	/* codeset 0 */
			switch (ie & 0xf0) {
			case ISDN_IE_CONGESTION_LEVEL:
				ret = unpack_cl(pvar, p, e, m);
				break;
			case ISDN_IE_REPEAT_INDICATOR:
				ret = unpack_rp(pvar, p, e, m);
				break;
			case ISDN_IE_SHIFT:
				ret = unpack_shift(pvar, p, e, m);
				break;
			default:
				switch (ie) {
				case ISDN_IE_RESERVED:
					ret = unpack_res(pvar, p, e, m);
					break;
				case ISDN_IE_MORE_DATA:
					ret = unpack_md(pvar, p, e, m);
					break;
				case ISDN_IE_SENDING_COMPLETE:
					ret = unpack_sc(pvar, p, e, m);
					break;
				case ISDN_IE_SEGMENTED_MESSAGE:
					ret = unpack_seg(pvar, p, e, m);
					break;
				case ISDN_IE_BEARER_CAPABILITY:
					ret = unpack_bc(pvar, p, e, m);
					break;
				case ISDN_IE_CAUSE:
					ret = unpack_cause(pvar, p, e, m);
					break;
				case ISDN_IE_CALL_IDENTITY:
					ret = unpack_cid(pvar, p, e, m);
					break;
				case ISDN_IE_CALL_STATE:
					ret = unpack_cs(pvar, p, e, m);
					break;
				case ISDN_IE_CHANNEL_IDENTIFICATION:
					ret = unpack_ci(pvar, p, e, m);
					break;
				case ISDN_IE_PROGRESS_INDICATOR:
					ret = unpack_pi(pvar, p, e, m);
					break;
				case ISDN_IE_NTWK_SPEC_FACILITIES:
					ret = unpack_nsf(pvar, p, e, m);
					break;
				case ISDN_IE_DISPLAY:
					ret = unpack_disp(pvar, p, e, m);
					break;
				case ISDN_IE_DATE_TIME:
					ret = unpack_dt(pvar, p, e, m);
					break;
				case ISDN_IE_KEYPAD_FACILITY:
					ret = unpack_kpf(pvar, p, e, m);
					break;
				case ISDN_IE_SIGNAL:
					ret = unpack_sig(pvar, p, e, m);
					break;
				case ISDN_IE_INFORMATION_RATE:
					ret = unpack_ir(pvar, p, e, m);
					break;
				case ISDN_IE_E2E_TRANSIT_DELAY:
					ret = unpack_eetd(pvar, p, e, m);
					break;
				case ISDN_IE_TRANS_DEL_SEL_AND_IND:
					ret = unpack_tdsi(pvar, p, e, m);
					break;
				case ISDN_IE_PL_BINARY_PARMS:
					ret = unpack_plbp(pvar, p, e, m);
					break;
				case ISDN_IE_PL_WINDOW_SIZE:
					ret = unpack_plws(pvar, p, e, m);
					break;
				case ISDN_IE_PACKET_SIZE:
					ret = unpack_ps(pvar, p, e, m);
					break;
				case ISDN_IE_CLOSED_USER_GROUP:
					ret = unpack_cug(pvar, p, e, m);
					break;
				case ISDN_IE_REVERSE_CHARGE_IND:
					ret = unpack_rci(pvar, p, e, m);
					break;
				case ISDN_IE_CALLING_PARTY_NUMBER:
					ret = unpack_cgpn(pvar, p, e, m);
					break;
				case ISDN_IE_CALLING_PARTY_SUBADD:
					ret = unpack_cgsa(pvar, p, e, m);
					break;
				case ISDN_IE_CALLED_PARTY_NUMBER:
					ret = unpack_cdpn(pvar, p, e, m);
					break;
				case ISDN_IE_CALLED_PARTY_SUBADD:
					ret = unpack_cdsa(pvar, p, e, m);
					break;
				case ISDN_IE_REDIRECTING_NUMBER:
					ret = unpack_rdn(pvar, p, e, m);
					break;
				case ISDN_IE_TRANSIT_NTWK_SEL:
					ret = unpack_tns(pvar, p, e, m);
					break;
				case ISDN_IE_RESTART_INDICATOR:
					ret = unpack_ri(pvar, p, e, m);
					break;
				case ISDN_IE_LL_COMPATIBILITY:
					ret = unpack_llc(pvar, p, e, m);
					break;
				case ISDN_IE_HL_COMPATIBILITY:
					ret = unpack_hlc(pvar, p, e, m);
					break;
				case ISDN_IE_USER_TO_USER_INFO:
					ret = unpack_uui(pvar, p, e, m);
					break;
				case ISDN_IE_ESCAPE:
					ret = unpack_esc(pvar, p, e, m);
					break;
				default:
					ret = unpack_unknown(pvar, p, e, m);
					break;
				}
			}
			break;
		case 4:	/* codeset 4 */
			switch (ie) {
			default:
				ret = unpack_unknown(pvar, p, e, m);
				break;
			}
			break;
		case 5:	/* codeset 5 */
			switch (ie) {
			default:
				ret = unpack_unknown(pvar, p, e, m);
				break;
			}
			break;
		case 6:	/* codeset 6 */
			switch (ie) {
			default:
				ret = unpack_unknown(pvar, p, e, m);
				break;
			}
			break;
		case 7:	/* codeset 7 */
			switch (ie) {
			default:
				ret = unpack_unknown(pvar, p, e, m);
				break;
			}
			break;
		}
		if (!m->codeset_lock)
			m->codeset = m->codeset_locked;
		if (ret < 0)
			return (ret);
		if ((p += ret) > e)
			return (-EMSGSIZE);
		len += ret;
	}
	return (len);
}

STATIC int
isdn_dec_msg(queue_t *q, mblk_t *mp, isdn_msg_t * m)
{
	struct dl *dl = DL_PRIV(q);
	struct fg *fg = dl->dc.dc->fg.fg;
	unsigned char *p;
	unsigned char *e;
	int ret;
	if (!fg)
		goto efault;
	if (!pullupmsg(mp, -1))
		goto enobufs;
	bzero(m, sizeof(*m));
	p = mp->b_rptr;
	e = mp->b_wptr;
	if (p < e + 3)
		goto emsgsize;
	if ((ret = unpack_pd(fg->proto.pvar, p, e, m)) < 0)	/* protocol discriminator */
		goto error;
	p += ret;
	if ((ret = unpack_cr(fg->proto.pvar, p, e, m)) < 0)	/* call reference */
		goto error;
	p += ret;
	if ((ret = unpack_mt(fg->proto.pvar, p, e, m)) < 0)	/* message type */
		goto error;
	p += ret;
	if ((ret = unpack_ies(fg->proto.pvar, p, e, m)) < 0)	/* information elements */
		goto error;
	p += ret;
	assure(p == e);
	return (QR_DONE);
      enobufs:
	ptrace(("Couldn't pull up message\n"));
	return (-ENOBUFS);
      efault:
	swerr();
	return (-EFAULT);
      emsgsize:
	ptrace(("Bad message size\n"));
	return (-EMSGSIZE);
      error:
	ptrace(("Decode error\n"));
	return (ret);
}

/*
 *  ========================================================================
 *
 *  EVENTS
 *
 *  ========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Timer Events
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T301 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t301_timeout(struct cr *cr)
{
	fixme(("Implement this timer\n"));
	return (-EOPNOTSUPP);
}

/*
 *  T302 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t302_timeout(struct cr *cr)
{
	int err;
	if (cr_get_c_state(cr) == U25_OVERLAP_RECEIVING) {
		if ((err = cc_info_timeout_ind(NULL, cr)) < 0)
			goto error;
		/* 
		   it is assumed that the decision whether complete information has been received
		   or not, at the expiry of T302, will be made by the call control. */
		cr_set_c_state(cr, U25_OVERLAP_RECEIVING);
	      error:
		return (err);
	}
	/* 
	   stagnant timer */
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T303 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t303_timeout(struct cr *cr)
{
	int err;
	if (cr_get_c_state(cr) == U1_CALL_INITIATED) {
		if ((cr->statem.t303_count ^= 1)) {
			/* 
			   first timeout */
			if ((err = isdn_send_setup(NULL, cr)) < 0)	/* use user B chan */
				goto error;
			cr_timer_start(cr, t303);
			cr_set_c_state(cr, U1_CALL_INITIATED);
		} else {
			/* 
			   subsequent timeout */
			if ((err = cc_connect_ind(NULL, cr, NULL)))
				goto error;
			cr_set_c_state(cr, U0_NULL);
			isdn_free_cr(cr);
		}
	      error:
		return (err);
	}
	/* 
	   stagnant timer */
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T304 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t304_timeout(struct cr *cr)
{
	int err;
	ulong cause = 0;		/* FIXME */
	if (cr_get_c_state(cr) == U2_OVERLAP_SENDING) {
		if ((err = isdn_send_disconnect(NULL, cr, cause)) < 0)
			goto error;
		cr_timer_start(cr, t305);
		if ((err = cc_connect_ind(NULL, cr, NULL)))
			goto error;
		cr_set_c_state(cr, U11_DISCONNECT_REQUEST);
	      error:
		return (err);
	}
	/* 
	   stagnant timer */
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T305 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t305_timeout(struct cr *cr)
{
	int err;
	ulong cause = 0;		/* FIXME */
	if (cr_get_c_state(cr) == U11_DISCONNECT_REQUEST) {
		if ((err = isdn_send_release(NULL, cr, cause)) < 0)
			goto error;
		cr_timer_start(cr, t308);
		cr_set_c_state(cr, U19_RELEASE_REQUEST);
	      error:
		return (err);
	}
	/* 
	   stagnant timer */
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T306 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t306_timeout(struct cr *cr)
{
	fixme(("Implement this timer\n"));
	return (-EOPNOTSUPP);
}

/*
 *  T307 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t307_timeout(struct cr *cr)
{
	int err = QR_DONE;
	switch (cr_get_c_state(cr)) {
	case N0_NULL:
		// isdn_bchan_release(q); /* Huh? */
		cr_set_c_state(cr, N0_NULL);
		isdn_free_cr(cr);
	      error:
		return (err);
	}
	/* 
	   stagnant timer */
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T308 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t308_timeout(struct cr *cr)
{
	int err;
	ulong cause = 0;		/* FIXME */
	if (cr_get_c_state(cr) == U19_RELEASE_REQUEST) {
		if ((cr->statem.t308_count ^= 1)) {
			/* 
			   first timeout */
			if ((err = isdn_send_release(NULL, cr, cause)) < 0)
				goto error;
			cr_timer_start(cr, t308);
			cr_set_c_state(cr, U19_RELEASE_REQUEST);
		} else {
			/* 
			   subsequent timeout */
			/* 
			   TODO: place B channel in maintenance */
			if ((err = cc_release_con(NULL, cr, NULL)))
				goto error;
			cr_set_c_state(cr, U0_NULL);
			isdn_free_cr(cr);
		}
	      error:
		return (err);
	}
	/* 
	   stagnant timer */
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T309 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t309_timeout(struct cr *cr)
{
	int err;
	if (cr_get_c_state(cr) != U0_NULL) {
		if ((err = cc_datalink_failure_ind(NULL, cr)) < 0)
			goto error;
		cr_set_c_state(cr, U0_NULL);
		isdn_free_cr(cr);
	      error:
		return (err);
	}
	/* 
	   stagnant timer */
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T310 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t310_timeout(struct cr *cr)
{
	int err;
	ulong cause = 0;		/* FIXME */
	if (cr_get_c_state(cr) == U3_OUTGOING_CALL_PROCEEDING) {
		if ((err = isdn_send_disconnect(NULL, cr, cause)) < 0)
			goto error;
		cr_timer_start(cr, t305);
		if ((err = cc_connect_ind(NULL, cr, NULL)))
			goto error;
		cr_set_c_state(cr, U11_DISCONNECT_REQUEST);
	      error:
		return (err);
	}
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T312 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t312_timeout(struct cr *cr)
{
	fixme(("Implement this timer\n"));
	return (-EOPNOTSUPP);
}

/*
 *  T313 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t313_timeout(struct cr *cr)
{
	int err;
	ulong cause = 0;		/* FIXME */
	if (cr_get_c_state(cr) == U8_CONNECT_REQUEST) {
		if ((err = isdn_send_disconnect(NULL, cr, cause)) < 0)
			goto error;
		cr_timer_start(cr, t305);
		if ((err = cc_setup_complete_ind(NULL, cr, NULL)) < 0)
			goto error;
		cr_set_c_state(cr, U11_DISCONNECT_REQUEST);
	      error:
		return (err);
	}
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T314 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t314_timeout(struct cr *cr)
{
	fixme(("Implement this timer\n"));
	return (-EOPNOTSUPP);
}

/*
 *  T316 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t316_timeout(struct cr *cr)
{
	fixme(("Implement this timer\n"));
	return (-EOPNOTSUPP);
}

/*
 *  T317 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t317_timeout(struct cr *cr)
{
	fixme(("Implement this timer\n"));
	return (-EOPNOTSUPP);
}

/*
 *  T318 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t318_timeout(struct cr *cr)
{
	int err;
	ulong cause = 0;		/* FIXME */
	if (cr_get_c_state(cr) == U17_RESUME_REQUEST) {
		if ((err = cc_resume_con(NULL, cr, NULL)) < 0)	/* to error */
			goto error;
		if ((err = isdn_send_release(NULL, cr, cause)) < 0)
			goto error;
		cr_timer_start(cr, t308);
		cr_set_c_state(cr, U19_RELEASE_REQUEST);
	      error:
		return (err);
	}
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T319 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t319_timeout(struct cr *cr)
{
	int err;
	if (cr_get_c_state(cr) == U15_SUSPEND_REQUEST) {
		if ((err = cc_suspend_con(NULL, cr, NULL)) < 0)	/* to error */
			goto error;
		cr_set_c_state(cr, U10_ACTIVE);
	      error:
		return (err);
	}
	rare();
	err = -ETIMEDOUT;
	goto error;
}

/*
 *  T320 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t320_timeout(struct cr *cr)
{
	fixme(("Implement this timer\n"));
	return (-EOPNOTSUPP);
}

/*
 *  T321 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t321_timeout(struct cr *cr)
{
	fixme(("Implement this timer\n"));
	return (-EOPNOTSUPP);
}

/*
 *  T322 EXPIRY
 *  -----------------------------------
 */
STATIC int
cr_t322_timeout(struct cr *cr)
{
	fixme(("Implement this timer\n"));
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from below
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA: (DL_DATA_IND)
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_data_ind(queue_t *q, mblk_t *dp)
{
	struct dl *dl = DL_PRIV(q);
	struct fg *fg = dl->dc.dc->fg.fg;
	struct cr *cr = NULL;
	int err;
	isdn_msg_t msg = { 0, };
	if (!fg)
		goto disable;
	if ((err = isdn_dec_msg(q, dp, &msg)))
		return (err);
	if (msg.cr == 0) {
		if (msg.crf != 0x2)
			/* 
			   no call reference (global call reference) */
			goto global_cr;
		else
			/* 
			   dummy call reference */
			goto dummy_cr;
	}
	for (cr = fg->cr.list; cr && cr->cref != msg.cr; cr = cr->fg.next) ;
	if (!cr)
		goto no_cr;
	else
		goto have_cr;
      have_cr:
	switch (msg.mt) {
	case ISDN_MT_ALERTING:
		printd(("%s: :%s :%p <- [%#08lx] ALERTING\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_alerting(q, cr, &msg);
		break;
	case ISDN_MT_CALL_PROCEEDING:
		printd(("%s: :%s :%p <- [%#08lx] PROCEEDING\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_call_proceeding(q, cr, &msg);
		break;
	case ISDN_MT_CONNECT:
		printd(("%s: :%s :%p <- [%#08lx] CONNECT\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_connect(q, cr, &msg);
		break;
	case ISDN_MT_CONNECT_ACKNOWLEDGE:
		printd(("%s: :%s :%p <- [%#08lx] CONNECT-ACKNOWLEDGE\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_connect_acknowledge(q, cr, &msg);
		break;
	case ISDN_MT_DISCONNECT:
		printd(("%s: :%s :%p <- [%#08lx] DISCONNECT\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_disconnect(q, cr, &msg);
		break;
	case ISDN_MT_INFORMATION:
		printd(("%s: :%s :%p <- [%#08lx] INFORMATION\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_information(q, cr, &msg);
		break;
	case ISDN_MT_NOTIFY:
		printd(("%s: :%s :%p <- [%#08lx] NOTIFY\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_notify(q, cr, &msg);
		break;
	case ISDN_MT_PROGRESS:
		printd(("%s: :%s :%p <- [%#08lx] PROGRESS\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_progress(q, cr, &msg);
		break;
	case ISDN_MT_RELEASE:
		printd(("%s: :%s :%p <- [%#08lx] RELEASE\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_release(q, cr, &msg);
		break;
	case ISDN_MT_RELEASE_COMPLETE:
		printd(("%s: :%s :%p <- [%#08lx] RELEASE-COMPLETE\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_release_complete(q, cr, &msg);
		break;
	case ISDN_MT_RESUME:
		printd(("%s: :%s :%p <- [%#08lx] RESUME\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_resume(q, cr, &msg);
		break;
	case ISDN_MT_RESUME_ACKNOWLEDGE:
		printd(("%s: :%s :%p <- [%#08lx] RESUME-ACKNOWLEDGE\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_resume_acknowledge(q, cr, &msg);
		break;
	case ISDN_MT_RESUME_REJECT:
		printd(("%s: :%s :%p <- [%#08lx] RESUME-REJECT\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_resume_reject(q, cr, &msg);
		break;
	case ISDN_MT_SETUP:
		printd(("%s: :%s :%p <- [%#08lx] SETUP\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_setup(q, cr, &msg);
		break;
	case ISDN_MT_SETUP_ACKNOWLEDGE:
		printd(("%s: :%s :%p <- [%#08lx] SETUP-ACKNOWLEDGE\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_setup_acknowledge(q, cr, &msg);
		break;
	case ISDN_MT_STATUS:
		printd(("%s: :%s :%p <- [%#08lx] STATUS\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_status(q, cr, &msg);
		break;
	case ISDN_MT_STATUS_ENQUIRY:
		printd(("%s: :%s :%p <- [%#08lx] ENQUIRY\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_status_enquiry(q, cr, &msg);
		break;
	case ISDN_MT_SUSPEND:
		printd(("%s: :%s :%p <- [%#08lx] SUSPEND\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_suspend(q, cr, &msg);
		break;
	case ISDN_MT_SUSPEND_ACKNOWLEDGE:
		printd(("%s: :%s :%p <- [%#08lx] SUSPEND-ACKNOWLEDGE\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_suspend_acknowledge(q, cr, &msg);
		break;
	case ISDN_MT_SUSPEND_REJECT:
		printd(("%s: :%s :%p <- [%#08lx] SUSPEND-REJECT\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_suspend_reject(q, cr, &msg);
		break;
	case ISDN_MT_CONGESTION_CONTROL:
		printd(("%s: :%s :%p <- [%#08lx] CONGESTION-CONTROL\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_congestion_control(q, cr, &msg);
		break;
	case ISDN_MT_USER_INFORMATION:
		printd(("%s: :%s :%p <- [%#08lx] USER-INFORMATION\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_user_information(q, cr, &msg);
		break;
	case ISDN_MT_RESTART:
		printd(("%s: :%s :%p <- [%#08lx] RESTART\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_restart(q, cr, &msg);
		break;
	case ISDN_MT_RESTART_ACKNOWLEDGE:
		printd(("%s: :%s :%p <- [%#08lx] RESTART-ACKNOWLEDGE\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_restart_acknowledge(q, cr, &msg);
		break;
	default:
		printd(("%s: :%s :%p <- [%#08lx] ????\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_unrecognized_message(q, cr, &msg);
		break;
	}
	if (err < 0)
		switch (err) {
		case -ENXIO:	/* missing mandatory parameter */
			if ((err =
			     isdn_send_status(q, cr, CC_CAUS_MISSING_MANDATORY_PARAMETER)) < 0)
				return (err);
			return (QR_DONE);
		case -EINVAL:	/* invalid mandatory parameter */
			if ((err =
			     isdn_send_status(q, cr, CC_CAUS_INVALID_MANDATORY_PARAMETER)) < 0)
				return (err);
			return (QR_DONE);
		case -EOPNOTSUPP:	/* unrecognized message */
			if (cr && cr_get_c_state(cr) != U0_NULL && cr_get_c_state(cr) != N0_NULL) {
				err = isdn_recv_unrecognized_message(q, cr, &msg);
				break;
			}
			/* 
			   fall through */
		case -EPROTO:	/* unexpected message */
			err = isdn_recv_unexpected_message(q, cr, &msg);
			break;
		case -EFAULT:	/* software error */
			swerr();
			return (-EFAULT);
		}
	return (err);
      no_cr:
	switch (msg.mt) {
	case ISDN_MT_SETUP:
		printd(("%s: :%s :%p <- [%#08lx] SETUP\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_setup(q, cr, &msg);
		break;
	default:
		printd(("%s: :%s :%p <- [%#08lx] ????\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_unexpected_message(q, cr, &msg);
		break;
	}
	return (QR_DONE);
      global_cr:
	switch (msg.mt) {
	case ISDN_MT_STATUS:
		printd(("%s: :%s :%p <- [%#08lx] STATUS\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_status(q, cr, &msg);
		break;
	case ISDN_MT_STATUS_ENQUIRY:
		printd(("%s: :%s :%p <- [%#08lx] ENQUIRY\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_status_enquiry(q, cr, &msg);
		break;
	case ISDN_MT_RESTART:
		printd(("%s: :%s :%p <- [%#08lx] RESTART\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_restart(q, cr, &msg);
		break;
	case ISDN_MT_RESTART_ACKNOWLEDGE:
		printd(("%s: :%s :%p <- [%#08lx] RESTART-ACKNOWLEDGE\n", DRV_NAME, __FUNCTION__, cr,
			cr->id));
		err = isdn_recv_restart_acknowledge(q, cr, &msg);
		break;
	default:
		printd(("%s: :%s :%p <- [%#08lx] ????\n", DRV_NAME, __FUNCTION__, cr, cr->id));
		err = isdn_recv_unexpected_message(q, cr, &msg);
		break;
	}
	return (QR_DONE);
      dummy_cr:
	goto discard;
      discard:
	/* 
	   discard */
	return (QR_DONE);
      disable:
	rare();
	return (QR_DISABLE);
}

/*
 *  DL_INFO_ACK                 0x03
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_info_ack(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_info_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->dl_qos_offset)
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->dl_qos_range_offset)
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->dl_addr_offset)
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->dl_brdcst_addr_offset)
		goto emsgsize;
	dl->info = *p;
	bcopy(mp->b_rptr + p->dl_addr_offset, &dl->dlc, p->dl_addr_length);
	dl_set_state(dl, p->dl_current_state);
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_BIND_ACK                 0x04
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_bind_ack(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->dl_addr_offset)
		goto emsgsize;
	if (dl_get_state(dl) != DL_BIND_PENDING)
		goto eproto;
	dl->dlc.dl_sap = p->dl_sap;
	dl->conind = p->dl_max_conind;
	/* 
	   ignore XID and test? */
	dl_set_state(dl, DL_IDLE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      eproto:
	swerr();
	return (-EPROTO);
}

/*
 *  DL_SUBS_BIND_ACK            0x1c
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_subs_bind_ack(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_subs_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (dl_get_state(dl) != DL_SUBS_BIND_PND)
		goto eproto;
	if (mp->b_wptr < mp->b_rptr + p->dl_subs_sap_offset)
		goto emsgsize;
	swerr();
	return (-EOPNOTSUPP);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      eproto:
	swerr();
	return (-EPROTO);
}

/*
 *  DL_OK_ACK                   0x06
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_ok_ack(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (p->dl_correct_primitive) {
	case DL_DETACH_REQ:
		dl_set_state(dl, DL_UNATTACHED);
		break;
	case DL_ATTACH_REQ:
	case DL_UNBIND_REQ:
		dl_set_state(dl, DL_UNBOUND);
		break;
	case DL_CONNECT_RES:
	case DL_DISCONNECT_REQ:
	case DL_SUBS_UNBIND_REQ:
		dl_set_state(dl, DL_IDLE);
		break;
	case DL_RESET_RES:
		dl_set_state(dl, DL_DATAXFER);
		break;
	case DL_PROMISCON_REQ:
	case DL_ENABMULTI_REQ:
	case DL_DISABMULTI_REQ:
	case DL_PROMISCOFF_REQ:
		break;
	default:
		swerr();
		goto eproto;
	}
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      eproto:
	swerr();
	return (-EPROTO);
}

/*
 *  DL_ERROR_ACK                0x05
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_error_ack(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_error_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (p->dl_error_primitive) {
	case DL_ATTACH_REQ:
		dl_set_state(dl, DL_UNATTACHED);
		break;
	case DL_DETACH_REQ:
	case DL_BIND_REQ:
		dl_set_state(dl, DL_UNBOUND);
		break;
	case DL_UNBIND_REQ:
	case DL_SUBS_BIND_REQ:
	case DL_SUBS_UNBIND_REQ:
	case DL_CONNECT_REQ:
	case DL_UDQOS_REQ:
		dl_set_state(dl, DL_IDLE);
		break;
	case DL_CONNECT_RES:
		dl_set_state(dl, DL_INCON_PENDING);
		break;
	case DL_DISCONNECT_REQ:
		switch (dl_get_state(dl)) {
		case DL_DISCON8_PENDING:
			dl_set_state(dl, DL_OUTCON_PENDING);
			break;
		case DL_DISCON9_PENDING:
			dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_DISCON11_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON12_PENDING:
			dl_set_state(dl, DL_USER_RESET_PENDING);
			break;
		case DL_DISCON13_PENDING:
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		default:
			swerr();
			goto eproto;
		}
		break;
	case DL_RESET_REQ:
		dl_set_state(dl, DL_DATAXFER);
		break;
	case DL_RESET_RES:
		dl_set_state(dl, DL_PROV_RESET_PENDING);
		break;
	case DL_INFO_REQ:
	case DL_ENABMULTI_REQ:
	case DL_DISABMULTI_REQ:
	case DL_PROMISCON_REQ:
	case DL_PROMISCOFF_REQ:
	case DL_TOKEN_REQ:
	case DL_TEST_REQ:
	case DL_TEST_RES:
	case DL_XID_REQ:
	case DL_XID_RES:
	case DL_DATA_ACK_REQ:
	case DL_REPLY_REQ:
	case DL_REPLY_UPDATE_REQ:
	case DL_PHYS_ADDR_REQ:
	case DL_SET_PHYS_ADDR_REQ:
	case DL_GET_STATISTICS_REQ:
	case DL_UNITDATA_REQ:
		break;
	default:
		swerr();
		goto eproto;
	}
	fixme(("Now we have problems.  Error out the stream.\n"));
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      eproto:
	swerr();
	return (-EPROTO);
}

/*
 *  DL_CONNECT_IND              0x0e
 *  -------------------------------------------------------------------------
 *  Whenever we get a connection indication we accept it on the same stream.
 */
STATIC int
dl_connect_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct cr *cr;
	int err;
	dl_connect_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	for (cr = dl->dc.dc->fg.fg->cr.list; cr; cr = cr->fg.next) {
		switch (cr_get_c_state(cr)) {
		case U2_OVERLAP_SENDING:
		case U25_OVERLAP_RECEIVING:
			if ((err = cc_error_ind(q, cr)) < 0)
				goto error;
		}
	}
	return dl_connect_res(q, dl, p->dl_correlation, dl->tok, ((caddr_t) p) + p->dl_qos_offset,
			      p->dl_qos_length);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      error:
	return (err);
}

/*
 *  DL_CONNECT_CON              0x10
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_connect_con(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct cr *cr;
	int err;
	ulong cause = 0;		/* FIXME */
	dl_connect_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	for (cr = dl->dc.dc->fg.fg->cr.list; cr; cr = cr->fg.next) {
		cr_timer_stop(cr, t309);
		if ((cr->fg.fg->proto.popt & ISDN_POPT_ENQ)) {
			if ((err = isdn_send_status_enquiry(q, cr)) < 0)
				goto error;
		} else {
			if ((err = isdn_send_status(q, cr, cause)) < 0)
				goto error;
		}
		/* 
		   remain in same state */
	}
	dl_set_state(dl, DL_DATAXFER);
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      error:
	rare();
	return (err);
}

/*
 *  DL_TOKEN_ACK                0x12
 *  -------------------------------------------------------------------------
 *  We ask for our token when we are linked.  This permits us to accept
 *  connection indications later.
 */
STATIC int
dl_token_ack(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_token_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	dl->tok = p->dl_token;
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_DISCONNECT_IND           0x14
 *  -------------------------------------------------------------------------
 *  See A.3/Q.931 (Sheet 24 of 25)  Overlap sending calls and overlap
 *  receiving state calls are released.  Otherwise if there is no timer
 *  running, we start T309 and send a DL_CONNECT_REQ.
 */
STATIC int
dl_disconnect_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct cr *cr;
	int err;
	dl_disconnect_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	for (cr = dl->dc.dc->fg.fg->cr.list; cr; cr = cr->fg.next) {
		switch (cr_get_c_state(cr)) {
		case U0_NULL:
			/* 
			   do nothing */
			break;
		case U2_OVERLAP_SENDING:
			if ((err = cc_connect_ind(q, cr, NULL)) < 0)
				goto error;
			cr_timer_stop(cr, t304);
			cr_set_c_state(cr, U0_NULL);
			isdn_free_cr(cr);
			break;
		case U25_OVERLAP_RECEIVING:
			if ((err = cc_release_ind(q, cr, NULL)) < 0)
				goto error;
			cr_timer_stop(cr, t302);
			cr_set_c_state(cr, U0_NULL);
			isdn_free_cr(cr);
			break;
		default:
			if (0) {
				/* 
				   calls in inactive state */
				cr_timer_stop(cr, tall);
				cr_set_c_state(cr, U0_NULL);
				isdn_free_cr(cr);
			} else {
				if (!(cr->timers.t309)) {
					if (dl_get_state(dl) == DL_IDLE)
						if ((err = dl_connect_req(q, dl)) < 0)
							goto error;
					cr_timer_start(cr, t309);
				}
				/* 
				   remain in current state */
			}
			break;
		}
	}
	dl_set_state(dl, DL_IDLE);
	fixme(("Notify L3\n"));
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      error:
	return (err);
}

/*
 *  DL_RESET_IND                0x18
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_reset_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	fixme(("Notify L3?\n"));
	return dl_reset_res(q, dl);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_RESET_CON                0x1a
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_reset_con(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reset_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	dl_set_state(dl, DL_DATAXFER);
	fixme(("Notify L3?\n"));
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_UNITDATA_IND             0x08
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_unitdata_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_UDERROR_IND              0x09
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_uderror_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_TEST_IND                 0x2e
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_test_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_test_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_TEST_CON                 0x30
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_test_con(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_test_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_XID_IND                  0x2a
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_xid_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_xid_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_XID_CON                  0x2c
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_xid_con(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_xid_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_DATA_ACK_IND             0x22
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_data_ack_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_data_ack_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_DATA_ACK_STATUS_IND      0x23
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_data_ack_status_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_data_ack_status_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_REPLY_IND                0x25
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_reply_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reply_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_REPLY_STATUS_IND         0x26
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_reply_status_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reply_status_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_REPLY_UPDATE_STATUS_IND  0x28
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_reply_update_status_ind(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reply_update_status_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_PHYS_ADDR_ACK            0x32
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_phys_addr_ack(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_phys_addr_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  DL_GET_STATISTICS_ACK       0x35
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_get_statistics_ack(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_get_statistics_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	(void) dl;
	fixme(("Write this function\n"));
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from above
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  M_DATA: (CC_DATA_REQ)
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_data_req(queue_t *q, mblk_t *mp)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CC_INFO_REQ
 *  -------------------------------------------------------------------------
 *  Requests that the provider respond with a CC_INFO_ACK providing
 *  information about the ISDN provider.
 */
STATIC int
cc_info_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	int err;
	struct CC_info_ack *p = (typeof(p)) mp->b_rptr;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cc_get_state(cc) == CCS_UNUSABLE)
		goto outstate;
	switch (cc_get_state(cc)) {
	default:
		goto notsupp;
	}
      notsupp:
	return cc_error_ack(q, cc, p->cc_primitive, CCNOTSUPP);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
}

/*
 *  CC_OPTMGMT_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	int err;
	struct CC_optmgmt_req *p = (typeof(p)) mp->b_wptr;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (cc_get_state(cc)) {
	case CCS_UNBND:
	case CCS_UNUSABLE:
		goto outstate;
	default:
		goto notsupp;
	}
      notsupp:
	return cc_error_ack(q, cc, p->cc_primitive, CCNOTSUPP);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
}

/*
 *  CC_BIND_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_bind_req(queue_t *q, mblk_t *mp)
{
	struct cc **ccp, *cc = CC_PRIV(q);
	int err;
	unsigned char *add_ptr;
	size_t add_len;
	struct CC_bind_req *p = (typeof(p)) mp->b_rptr;
	struct isdn_addr add = { ISDN_SCOPE_DF, 0, 0 };
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto badprim;
	switch (cc_get_state(cc)) {
	case CCS_UNBND:
		break;
	default:
		goto outstate;
	}
	if (add_len) {
		if (add_len < sizeof(add))
			goto badaddr;
		bcopy(add_ptr, &add, sizeof(add));
	}
	switch (add.scope) {
	case ISDN_SCOPE_DF:
	{
		struct df *df;
		df = &master;
		/* 
		   bind to default */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &df->bind.mgm;
			else if (p->cc_bind_flags & CC_MONITOR)
				ccp = &df->bind.xry;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &df->bind.mnt;
			else
				ccp = &df->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &df->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (unsigned char *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.df = df;
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISDN_BIND_DF;
		break;
	}
	case ISDN_SCOPE_XG:
	{
		struct xg *xg;
		if (add.id) {
			for (xg = master.xg.list; xg && xg->id != add.id; xg = xg->next) ;
			if (!xg)
				goto badaddr;
		} else {
			if (!(xg = master.xg.list))
				goto noaddr;
			add.id = xg->id;
		}
		/* 
		   bind to exchange group */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &xg->bind.mgm;
			else if (p->cc_bind_flags & CC_MONITOR)
				ccp = &xg->bind.xry;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &xg->bind.mnt;
			else
				ccp = &xg->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &xg->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (unsigned char *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.xg = xg_get(xg);
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISDN_BIND_XG;
		break;
	}
	case ISDN_SCOPE_EG:
	{
		struct eg *eg;
		if (add.id) {
			for (eg = master.eg.list; eg && eg->id != add.id; eg = eg->next) ;
			if (!eg)
				goto badaddr;
		} else {
			if (!(eg = master.eg.list))
				goto noaddr;
			add.id = eg->id;
		}
		/* 
		   bind to equipment group */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &eg->bind.mgm;
			else if (p->cc_bind_flags & CC_MONITOR)
				ccp = &eg->bind.xry;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &eg->bind.mnt;
			else
				ccp = &eg->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &eg->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (unsigned char *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.eg = eg_get(eg);
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISDN_BIND_EG;
		break;
	}
	case ISDN_SCOPE_FG:
	{
		struct fg *fg;
		if (add.id) {
			for (fg = master.fg.list; fg && fg->id != add.id; fg = fg->next) ;
			if (!fg)
				goto badaddr;
		} else {
			if (!(fg = master.fg.list))
				goto noaddr;
			add.id = fg->id;
		}
		/* 
		   bind to facility group */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &fg->bind.mgm;
			else if (p->cc_bind_flags & CC_MONITOR)
				ccp = &fg->bind.xry;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &fg->bind.mnt;
			else
				ccp = &fg->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &fg->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (unsigned char *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.fg = fg_get(fg);
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISDN_BIND_FG;
		break;
	}
	case ISDN_SCOPE_TG:
	{
		struct tg *tg;
		if (add.id) {
			for (tg = master.tg.list; tg && tg->id != add.id; tg = tg->next) ;
			if (!tg)
				goto badaddr;
		} else {
			if (!(tg = master.tg.list))
				goto noaddr;
			add.id = tg->id;
		}
		/* 
		   bind to transmission group */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &tg->bind.mgm;
			else if (p->cc_bind_flags & CC_MONITOR)
				ccp = &tg->bind.xry;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &tg->bind.mnt;
			else
				ccp = &tg->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &tg->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (unsigned char *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.tg = tg_get(tg);
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISDN_BIND_TG;
		break;
	}
	case ISDN_SCOPE_CH:
	{
		struct ch *ch;
		if (add.id) {
			for (ch = master.ch.list; ch && ch->id != add.id; ch = ch->next) ;
			if (!ch)
				goto badaddr;
		} else {
			if (!(ch = master.ch.list))
				goto noaddr;
			add.id = ch->id;
		}
		/* 
		   bind to channel */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &ch->bind.mgm;
			else if (p->cc_bind_flags & CC_MONITOR)
				ccp = &ch->bind.xry;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &ch->bind.mnt;
			else
				ccp = &ch->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &ch->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (unsigned char *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.ch = ch_get(ch);
		cc->maxind = p->cc_setup_ind;
		if (cc->maxind > 1)
			cc->maxind = 1;
		cc->bind.type = ISDN_BIND_CH;
		break;
	}
	default:
		goto badaddr;
	}
	if ((cc->bind.next = *ccp))
		cc->bind.next->bind.prev = &cc->bind.next;
	*ccp = cc_get(cc);
	cc->bind.prev = ccp;
	cc_set_state(cc, CCS_IDLE);
	return (QR_DONE);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
      addrbusy:
	return cc_error_ack(q, cc, p->cc_primitive, CCADDRBUSY);
      noaddr:
	return cc_error_ack(q, cc, p->cc_primitive, CCNOADDR);
      badaddr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADADDR);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
}

/*
 *  CC_UNBIND_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_unbind_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	int err;
	struct CC_unbind_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cc_get_state(cc) != CCS_IDLE)
		goto outstate;
	switch (cc->bind.type) {
	case ISDN_BIND_DF:
	case ISDN_BIND_XG:
	case ISDN_BIND_EG:
	case ISDN_BIND_FG:
	case ISDN_BIND_TG:
	case ISDN_BIND_CH:
		if ((err = cc_ok_ack(q, cc, p->cc_primitive)))
			return (err);
		break;
	default:
		swerr();
		goto efault;
	}
	switch (cc->bind.type) {
	case ISDN_BIND_DF:
		xchg(&cc->bind.u.df, NULL);
		break;
	case ISDN_BIND_XG:
		xg_put(xchg(&cc->bind.u.xg, NULL));
		break;
	case ISDN_BIND_EG:
		eg_put(xchg(&cc->bind.u.eg, NULL));
		break;
	case ISDN_BIND_FG:
		fg_put(xchg(&cc->bind.u.fg, NULL));
		break;
	case ISDN_BIND_TG:
		tg_put(xchg(&cc->bind.u.tg, NULL));
		break;
	case ISDN_BIND_CH:
		ch_put(xchg(&cc->bind.u.ch, NULL));
		break;
	}
	cc->bind.type = ISDN_BIND_NONE;
	if ((*cc->bind.prev = cc->bind.next))
		cc->bind.next->bind.prev = cc->bind.prev;
	cc->bind.next = NULL;
	cc->bind.prev = &cc->bind.next;
	cc_put(cc);
	cc_set_state(cc, CCS_UNBND);
	return (QR_DONE);
      efault:
	return m_error(q, cc, EFAULT);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_ADDR_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_addr_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	int err;
	struct CC_addr_req *p = (typeof(p)) mp->b_rptr;
	struct isdn_addr bind, conn;
	unsigned char *bind_ptr = (unsigned char *) &bind, *conn_ptr = (unsigned char *) &conn;
	size_t bind_len = sizeof(bind), conn_len = sizeof(conn);
	ulong cref;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (cc_get_state(cc)) {
	case CCS_UNBND:
	case CCS_UNUSABLE:
		goto noaddr;
	}
	switch (cc->bind.type) {
	case ISDN_BIND_CH:
		bind.scope = ISDN_SCOPE_CH;
		bind.id = cc->bind.u.ch->id;
		bind.ci = cc->bind.u.ch->ci;
		break;
	case ISDN_BIND_TG:
		bind.scope = ISDN_SCOPE_TG;
		bind.id = cc->bind.u.tg->id;
		bind.ci = 0;
		break;
	case ISDN_BIND_FG:
		bind.scope = ISDN_SCOPE_FG;
		bind.id = cc->bind.u.fg->id;
		bind.ci = 0;
		break;
	case ISDN_BIND_EG:
		bind.scope = ISDN_SCOPE_EG;
		bind.id = cc->bind.u.eg->id;
		bind.ci = 0;
		break;
	case ISDN_BIND_XG:
		bind.scope = ISDN_SCOPE_XG;
		bind.id = cc->bind.u.xg->id;
		bind.ci = 0;
		break;
	case ISDN_BIND_DF:
		bind.scope = ISDN_SCOPE_DF;
		bind.id = 0;
		bind.ci = 0;
		break;
	default:
		goto noaddr;
	}
	if (cc->conn.cpc) {
		conn.scope = ISDN_SCOPE_CH;
		conn.id = cc->conn.cpc->id;
		conn.ci = 0;
		cref = cc->conn.cpc->cref;
	} else if (cc->conn.mgm.ch) {
		conn.scope = ISDN_SCOPE_CH;
		conn.id = cc->conn.mgm.ch->id;
		conn.ci = cc->conn.mgm.ch->ci;
		cref = 0;
	} else if (cc->conn.mgm.tg) {
		conn.scope = ISDN_SCOPE_TG;
		conn.id = cc->conn.mgm.tg->id;
		conn.ci = 0;
		cref = 0;
	} else if (cc->conn.mgm.fg) {
		conn.scope = ISDN_SCOPE_FG;
		conn.id = cc->conn.mgm.fg->id;
		conn.ci = 0;
		cref = 0;
	} else if (cc->conn.mgm.eg) {
		conn.scope = ISDN_SCOPE_EG;
		conn.id = cc->conn.mgm.eg->id;
		conn.ci = 0;
		cref = 0;
	} else if (cc->conn.mgm.xg) {
		conn.scope = ISDN_SCOPE_XG;
		conn.id = cc->conn.mgm.xg->id;
		conn.ci = 0;
		cref = 0;
	} else {
		conn_ptr = NULL;
		conn_len = 0;
		cref = 0;
	}
	return cc_addr_ack(q, cc, bind_ptr, bind_len, cref, conn_ptr, conn_len);
      noaddr:
	return cc_addr_ack(q, cc, NULL, 0, 0, NULL, 0);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_SETUP_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_setup_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr = NULL;		/* FIXME */
	struct ch *ch = NULL;		/* FIXME: Just to shut up compiler */
	unsigned char *cdpn_ptr, *opt_ptr, *add_ptr;
	size_t cdpn_len, opt_len, add_len;
	struct CC_setup_req *p = (typeof(p)) mp->b_rptr;
	struct isdn_addr add = { ISDN_SCOPE_DF, 0, 0 };
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	cdpn_ptr = mp->b_rptr + p->cc_cdpn_offset;
	cdpn_len = p->cc_cdpn_length;
	if (mp->b_wptr < cdpn_ptr + cdpn_len)
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto badprim;
	if (isdn_check_cdpn(cdpn_ptr, cdpn_len) < 0)
		goto baddigs;
	if (isdn_check_opt(opt_ptr, opt_len))
		goto badopt;
	if (isdn_check_addr(add_ptr, add_len))
		goto badaddr;
	/* 
	   cannot trust alignment of address */
	bcopy(add_ptr, &add, add_len);
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	ensure(cc->bind.u.ch, goto efault);
	if (!add_len) {
		switch (cc->bind.type) {
		case ISDN_BIND_XG:
			/* 
			   select the first circuit */
			if (!cc->bind.u.xg || !cc->bind.u.xg->eg.list
			    || !cc->bind.u.xg->eg.list->fg.list
			    || !cc->bind.u.xg->eg.list->fg.list->ch.list)
				goto addrbusy;
			ch = cc->bind.u.xg->eg.list->fg.list->ch.list;
			break;
		case ISDN_BIND_EG:
			/* 
			   select the first circuit */
			if (!cc->bind.u.eg || !cc->bind.u.eg->fg.list
			    || !cc->bind.u.eg->fg.list->ch.list)
				goto addrbusy;
			ch = cc->bind.u.eg->fg.list->ch.list;
			break;
		case ISDN_BIND_FG:
			/* 
			   select the first circuit */
			if (!cc->bind.u.fg || !cc->bind.u.fg->ch.list)
				goto addrbusy;
			ch = cc->bind.u.fg->ch.list;
			break;
		case ISDN_BIND_TG:
			/* 
			   select the first circuit */
			if (!cc->bind.u.tg || !cc->bind.u.tg->ch.list)
				goto addrbusy;
			ch = cc->bind.u.tg->ch.list;
			break;
		case ISDN_BIND_CH:
			/* 
			   select the first circuit */
			if (!cc->bind.u.ch)
				goto addrbusy;
			ch = cc->bind.u.ch;
			break;
		case ISDN_BIND_DF:
			goto noaddr;
		default:
			swerr();
			goto efault;
		}
	} else {
		switch (add.scope) {
		case ISDN_SCOPE_FG:
			/* 
			   Note: if we are bound to a facility group we can specify an address with 
			   scope facility group and a ci within the bound facility group to specify 
			   a channel.  If the ci == 0, the next idle channel within the facility
			   group will be selected.  ci == 0 is the same as not specifying an
			   address at all, so this option is useful only for specifying a ci within 
			   a bound facility group. When facilty scope is specified and no channel
			   is specified, no channel selection wil be provided in user setup
			   messages and any channel will be permitted.  When facility scope is
			   specified and a channel is specified, the channel will be preferred and
			   any channel will be permitted. */
			if (cc->bind.type != ISDN_BIND_FG)
				goto noaddr;
			if (add.id && add.id != cc->bind.u.fg->id)
				goto badaddr;
			if (add.ci) {
				for (ch = cc->bind.u.fg->ch.list; ch && ch->ci != add.ci;
				     ch = ch->fg.next) ;
				if (!ch)
					goto badaddr;
			} else {
				if (!(ch = cc->bind.u.fg->idle.list))
					goto addrbusy;
				break;
			}
			break;
		case ISDN_SCOPE_TG:
			/* 
			   Note: if we are bound to a transmission group we can specify an address
			   with scope transmission group and a ci witin the bound transmission
			   group to specify a channel. If the ci == 0, the first channel in the
			   transmission group will be selected. When transmission group scope is
			   specified and no channel is specified, all channels within the
			   transmission group will be specifie das preferred, and any channel
			   within the transmission group will be accepted.  When transmission group 
			   scope is specified and a channel is specified, the channel will be
			   selected and no other channel will be permitted. */
			if (cc->bind.type != ISDN_BIND_TG)
				goto noaddr;
			if (add.id && add.id != cc->bind.u.tg->id)
				goto badaddr;
			if (add.ci) {
				for (ch = cc->bind.u.tg->ch.list; ch && ch->ci != add.ci;
				     ch = ch->tg.next) ;
				if (!ch)
					goto badaddr;
			} else {
				if (!(ch = cc->bind.u.tg->ch.list))
					goto noaddr;
				break;
			}
		case ISDN_SCOPE_CH:
			/* 
			   Note: we can always specify a channel no matter how we are bound by
			   specifying an address with scope channel and a channel identifier and
			   optional ci. When we specify addresses this way, the setup will not
			   permit negotiation of another channel. */
			if (!add.id)
				goto noaddr;
			switch (cc->bind.type) {
			case ISDN_BIND_CH:
				if (add.id != cc->bind.u.ch->id)
					goto badaddr;
				ch = cc->bind.u.ch;
				break;
			case ISDN_BIND_TG:
				for (ch = cc->bind.u.tg->ch.list; ch && ch->id != add.id;
				     ch = ch->tg.next) ;
				if (!ch)
					goto badaddr;
				break;
			case ISDN_BIND_FG:
				for (ch = cc->bind.u.fg->ch.list; ch && ch->id != add.id;
				     ch = ch->fg.next) ;
				if (!ch)
					goto badaddr;
				break;
			case ISDN_BIND_EG:
			{
				struct fg *fg;
				for (ch = NULL, fg = cc->bind.u.eg->fg.list; fg; fg = fg->eg.next)
					for (ch = fg->ch.list; ch && ch->id != add.id;
					     ch = ch->fg.next) ;
				if (!ch)
					goto badaddr;
				break;
			}
			case ISDN_BIND_XG:
			{
				struct eg *eg;
				struct fg *fg;
				for (ch = NULL, eg = cc->bind.u.xg->eg.list; eg; eg = eg->xg.next)
					for (fg = eg->fg.list; fg; fg = fg->eg.next)
						for (ch = fg->ch.list; ch && ch->id != add.id;
						     ch = ch->fg.next) ;
				if (!ch)
					goto badaddr;
				break;
			}
			case ISDN_BIND_DF:
				for (ch = cc->bind.u.df->ch.list; ch && ch->id != add.id;
				     ch = ch->next) ;
				if (!ch)
					goto badaddr;
				break;
			default:
				goto efault;
			}
			break;
		case ISDN_SCOPE_DF:
			if (cc->bind.type != ISDN_BIND_DF)
				goto badaddr;
			goto noaddr;
		case ISDN_SCOPE_XG:
			if (cc->bind.type != ISDN_BIND_XG)
				goto badaddr;
			goto noaddr;
		case ISDN_SCOPE_EG:
			if (cc->bind.type != ISDN_BIND_EG)
				goto badaddr;
			goto noaddr;
		}
	}
	if (!cc_get_uref(cr, ch, cc, p->cc_user_ref))
		goto badclr;
#if 0
	if (ch_tst(ch, BCHM_OUT_OF_SERVICE))
		goto addrbusy;
#endif
	switch (cr_get_i_state(cr)) {
	case CCS_IDLE:
	case CCS_WREQ_SETUP:
		break;
	default:
		goto addrbusy;
	}
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		cr_set_i_state(cr, cc, CCS_WREQ_SETUP);	/* selects a call reference */
		fixme(("Determine selected, preferred and alternate B-channels\n"));
		isdn_send_setup(q, cr);	/* with user selected B channel */
		cr_timer_start(cr, t303);
		cr_set_c_state(cr, U1_CALL_INITIATED);
		cr_set_i_state(cr, cc, CCS_WIND_MORE);
		return (QR_DONE);
	case N0_NULL:
		cr_set_i_state(cr, cc, CCS_WREQ_SETUP);	/* selects a call reference */
		fixme(("Determine selected, preferred and alternate B-channels\n"));
		isdn_send_setup(q, cr);	/* with network selected B channel */
		cr_timer_start(cr, t303);
		cr_set_c_state(cr, N1_CALL_INITIATED);
		cr_set_i_state(cr, cc, CCS_WIND_MORE);
		return (QR_DONE);
	default:
		goto addrbusy;
	}
      efault:
	return m_error(q, cc, EFAULT);
      addrbusy:
	return cc_error_ack(q, cc, p->cc_primitive, CCADDRBUSY);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      noaddr:
	return cc_error_ack(q, cc, p->cc_primitive, CCNOADDR);
      badaddr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADADDR);
      baddigs:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADDIGS);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_MORE_INFO_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_more_info_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	int err;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_more_info_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_opt(opt_ptr, opt_len) < 0)
		goto badopt;
	if (cc_get_state(cc) != CCS_WRES_SIND)
		goto outstate;
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_i_state(cr)) {
	case CCS_WREQ_MORE:
		break;
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U6_CALL_PRESENT:
		fixme(("Negotiate B-channel\n"));
		if ((err = isdn_send_setup_acknowledge(q, cr)) < 0)	/* negotiated B chan */
			goto error;
		cr_timer_start(cr, t302);
		cr_set_c_state(cr, U25_OVERLAP_RECEIVING);
		break;
	default:
		swerr();
		goto outstate;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WREQ_MORE:
		cr_set_i_state(cr, cc, CCS_WIND_INFO);
		break;
	default:
		swerr();
		goto outstate;
	}
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      error:
	return (err);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
}

/*
 *  CC_INFORMATION_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_information_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	int err;
	unsigned char *opt_ptr, *sub_ptr;
	size_t opt_len, sub_len;
	struct CC_information_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	sub_ptr = mp->b_rptr + p->cc_subn_offset;
	sub_len = p->cc_subn_length;
	if (mp->b_wptr < sub_ptr + sub_len)
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_subn(sub_ptr, sub_len) < 0)
		goto baddigs;
	if (isdn_check_opt(opt_ptr, opt_len) < 0)
		goto badopt;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_uref(cc, p->cc_user_ref)))
		goto badclr;
	switch (cr_get_i_state(cr)) {
	case CCS_WREQ_INFO:
	case CCS_WREQ_PROCEED:
		break;
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U1_CALL_INITIATED:
		/* 
		   save */
		break;
	case U2_OVERLAP_SENDING:
		if ((err = isdn_send_information(q, cr)) < 0)
			goto error;
		cr_timer_start(cr, t304);	/* our timers always restart */
		cr_set_c_state(cr, U2_OVERLAP_SENDING);
		break;
	default:
		/* 
		   any state except 0, 1, 6, 17, 19 */
		if ((err = isdn_send_information(q, cr)) < 0)
			goto error;
		/* 
		   remain in same state */
		break;
	case U0_NULL:
	case U6_CALL_PRESENT:
	case U17_RESUME_REQUEST:
	case U19_RELEASE_REQUEST:
		goto outstate;
	}
	cr_set_i_state(cr, cc, CCS_WREQ_PROCEED);
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      error:
	return (err);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      baddigs:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADDIGS);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
}

/*
 *  CC_SETUP_RES
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_setup_res(queue_t *q, mblk_t *mp)
{
	struct cc *ac, *cc = CC_PRIV(q);
	struct cr *cr;
	struct CC_setup_res *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (cc_get_state(cc) != CCS_WRES_SIND)
		goto outstate;
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_i_state(cr)) {
	case CCS_WRES_SIND:
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WREQ_PROCEED:
		if (p->cc_token_value) {
			if (!(ac = cc_find_tok(cc, p->cc_token_value)))
				goto badtok;
			cr_swap_cref(cr, ac);
		}
		break;
	default:
		goto outstate;
	}
	if ((err = cc_ok_ack(q, cc, p->cc_primitive)))
		return (err);
	return (QR_DONE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badtok:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADTOK);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_PROCEEDING_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_proceeding_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	int err;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_proceeding_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_rptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U6_CALL_PRESENT:
	case U25_OVERLAP_RECEIVING:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WREQ_MORE:
	case CCS_WREQ_PROCEED:
		if ((err = cc_ok_ack(q, cc, p->cc_primitive)))
			goto error;
		cr_set_i_state(cr, cc, CCS_WACK_PROCEED);
		break;
	case CCS_WACK_PROCEED:
		/* 
		   second time through after error */
		break;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U6_CALL_PRESENT:
		/* 
		   neg B channel */
		if ((err = isdn_send_call_proceeding(q, cr)) < 0)
			goto error;
		cr_set_c_state(cr, U9_INCOMING_CALL_PROCEEDING);
		break;
	case U25_OVERLAP_RECEIVING:
		cr_timer_stop(cr, t302);
		if ((err = isdn_send_call_proceeding(q, cr)) < 0)
			goto error;
		cr_set_c_state(cr, U9_INCOMING_CALL_PROCEEDING);
		break;
	default:
		swerr();
		goto efault;
	}
	cr_set_i_state(cr, cc, CCS_WREQ_ALERTING);
	return (QR_DONE);
      error:
	return (err);
      efault:
	return m_error(q, cc, EFAULT);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_ALERTING_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_alerting_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	int err;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_alerting_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U6_CALL_PRESENT:
	case U9_INCOMING_CALL_PROCEEDING:
	case U25_OVERLAP_RECEIVING:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WREQ_PROCEED:
	case CCS_WREQ_ALERTING:
		if ((err = cc_ok_ack(q, cc, p->cc_primitive)))
			return (err);
		cr_set_i_state(cr, cc, CCS_WACK_ALERTING);
		break;
	case CCS_WACK_ALERTING:
		/* 
		   second time through after error */
		break;
	case CCS_WRES_RELIND:
		goto ignore;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U6_CALL_PRESENT:	/* with negotiated B chan */
		if ((err = isdn_send_alerting(q, cr)) < 0)	/* with neg B chan */
			goto error;
		cr_set_c_state(cr, U7_CALL_RECEIVED);
		break;
	case U9_INCOMING_CALL_PROCEEDING:
		if ((err = isdn_send_alerting(q, cr)) < 0)
			goto error;
		cr_set_c_state(cr, U7_CALL_RECEIVED);
		break;
	case U25_OVERLAP_RECEIVING:
		cr_timer_stop(cr, t302);
		if ((err = isdn_send_alerting(q, cr)) < 0)
			goto error;
		cr_set_c_state(cr, U7_CALL_RECEIVED);
		break;
	default:
		swerr();
		goto efault;
	}
	cr_set_i_state(cr, cc, CCS_WREQ_PROGRESS);
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      error:
	return (err);
      efault:
	return m_error(q, cc, EFAULT);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_PROGRESS_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_progress_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	int err;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_progress_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U9_INCOMING_CALL_PROCEEDING:
	case U25_OVERLAP_RECEIVING:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WREQ_PROCEED:
	case CCS_WREQ_ALERTING:
	case CCS_WREQ_PROGRESS:
	case CCS_WREQ_CONNECT:
		if ((err = cc_ok_ack(q, cc, p->cc_primitive)))
			return (err);
		cr_set_i_state(cr, cc, CCS_WACK_PROGRESS);
	case CCS_WACK_PROGRESS:
		break;
	case CCS_WRES_RELIND:
		goto ignore;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U9_INCOMING_CALL_PROCEEDING:
		if ((err = isdn_send_progress(q, cr, p->cc_event)) < 0)
			goto error;
		cr_set_c_state(cr, U9_INCOMING_CALL_PROCEEDING);
		break;
	case U25_OVERLAP_RECEIVING:
		if ((err = isdn_send_progress(q, cr, p->cc_event)) < 0)
			goto error;
		cr_set_c_state(cr, U25_OVERLAP_RECEIVING);
		break;
	default:
		swerr();
		goto efault;
	}
	cr_set_i_state(cr, cc, CCS_WREQ_CONNECT);
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      error:
	return (err);
      efault:
	return m_error(q, cc, EFAULT);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_DISCONNECT_REQ
 *  -------------------------------------------------------------------------
 *  This is known as CC_IBI_REQ for the NNI.
 */
STATIC int
cc_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	int err;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_disconnect_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
	case U6_CALL_PRESENT:
	case U11_DISCONNECT_REQUEST:
	case U12_DISCONNECT_INDICATION:
	case U15_SUSPEND_REQUEST:
	case U19_RELEASE_REQUEST:
		goto outstate;
	default:
		break;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WREQ_PROCEED:
	case CCS_WREQ_ALERTING:
	case CCS_WREQ_PROGRESS:
	case CCS_WREQ_CONNECT:
		if ((err = cc_ok_ack(q, cc, p->cc_primitive)))
			return (err);
		cr_set_i_state(cr, cc, CCS_WIND_CONNECT);
		break;
	case CCS_WRES_RELIND:
		goto ignore;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U1_CALL_INITIATED:
		cr_timer_stop(cr, t303);
		isdn_send_disconnect(q, cr, p->cc_cause);
		cr_timer_start(cr, t305);
		cr_set_c_state(cr, U11_DISCONNECT_REQUEST);
		break;
	case U17_RESUME_REQUEST:
		/* 
		   open issue: handling of disconnect request primitive */
		break;
	default:
		/* 
		   any state except 0, 1, 6, 11, 12, 15, 17, 19 */
		if ((err = isdn_send_disconnect(q, cr, p->cc_cause)) < 0)
			goto error;
		cr_timer_start(cr, t305);
		cr_set_c_state(cr, U11_DISCONNECT_REQUEST);
		break;
	case U0_NULL:
	case U6_CALL_PRESENT:
	case U11_DISCONNECT_REQUEST:
	case U12_DISCONNECT_INDICATION:
	case U15_SUSPEND_REQUEST:
	case U19_RELEASE_REQUEST:
		swerr();
		goto efault;
	}
	cr_set_i_state(cr, cc, CCS_WIND_RELEASE);
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      error:
	return (err);
      efault:
	return m_error(q, cc, EFAULT);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_CONNECT_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_connect_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	int err;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_connect_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U6_CALL_PRESENT:
	case U7_CALL_RECEIVED:
	case U9_INCOMING_CALL_PROCEEDING:
	case U25_OVERLAP_RECEIVING:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WREQ_MORE:
	case CCS_WREQ_PROCEED:
	case CCS_WREQ_ALERTING:
	case CCS_WREQ_PROGRESS:
	case CCS_WREQ_CONNECT:
		cr_set_i_state(cr, cc, CCS_WCON_CREQ);
		break;
	case CCS_WRES_RELIND:
		goto ignore;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U6_CALL_PRESENT:
		/* 
		   with neg B chan */
		if ((err = isdn_send_connect(q, cr)) < 0)
			goto error;
		cr_timer_start(cr, t313);
		cr_set_c_state(cr, U8_CONNECT_REQUEST);
		break;
	case U7_CALL_RECEIVED:
		/* 
		   with neg B chan */
		if ((err = isdn_send_connect(q, cr)) < 0)
			goto error;
		cr_timer_start(cr, t313);
		cr_set_c_state(cr, U8_CONNECT_REQUEST);
		break;
	case U9_INCOMING_CALL_PROCEEDING:
		if ((err = isdn_send_connect(q, cr)) < 0)
			goto error;
		cr_timer_start(cr, t313);
		cr_set_c_state(cr, U8_CONNECT_REQUEST);
		break;
	case U25_OVERLAP_RECEIVING:
		cr_timer_stop(cr, t302);
		if ((err = isdn_send_connect(q, cr)) < 0)
			goto error;
		cr_timer_start(cr, t313);
		cr_set_c_state(cr, U8_CONNECT_REQUEST);
		break;
	default:
		swerr();
		goto efault;
	}
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      error:
	return (err);
      efault:
	return m_error(q, cc, EFAULT);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_SETUP_COMPLETE_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_setup_complete_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_setup_complete_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U10_ACTIVE:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WREQ_CONNECT:
		if ((err = cc_ok_ack(q, cc, p->cc_primitive)))
			goto error;
		cr_set_i_state(cr, cc, CCS_WCON_CREQ);
		break;
	case CCS_WCON_CREQ:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U10_ACTIVE:
		if (cr->fg.fg->proto.popt & ISDN_POPT_ACK)
			if ((err = isdn_send_connect_acknowledge(q, cr, opt_ptr, opt_len)))
				goto error;
		break;
	default:
		swerr();
		goto efault;
	}
	cr_set_i_state(cr, cc, CCS_CONNECTED);
	return (QR_DONE);
      error:
	return (err);
      efault:
	return m_error(q, cc, EFAULT);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_NOTIFY_REQ
 *  -------------------------------------------------------------------------
 *  This is the same as a user suspend request in the NNI.
 */
STATIC int
cc_notify_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	int err;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_notify_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U10_ACTIVE:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_CONNECTED:
	case CCS_SUSPENDED:
		if ((err = cc_ok_ack(q, cc, p->cc_primitive)))
			goto error;
		break;
	case CCS_WRES_RELIND:
		goto ignore;
	default:
		goto outstate;
	}
	fixme(("This function needs more work\n"));
	switch (cr_get_c_state(cr)) {
	case U10_ACTIVE:
		if ((err = isdn_send_notify(q, cr, opt_ptr, opt_len)) < 0)
			goto error;
		cr_set_c_state(cr, U10_ACTIVE);
		break;
	default:
		goto efault;
	}
	cr_set_i_state(cr, cc, CCS_SUSPENDED);
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      error:
	return (err);
      efault:
	return m_error(q, cc, EFAULT);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_SUSPEND_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_suspend_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	int err;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_suspend_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_opt(opt_ptr, opt_len) < 0)
		goto badopt;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U10_ACTIVE:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_CONNECTED:
		break;
	case CCS_WRES_RELIND:
		goto ignore;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U10_ACTIVE:
		if ((err = isdn_send_suspend(q, cr)) < 0)
			goto error;
		cr_timer_start(cr, t319);
		cr_set_c_state(cr, U15_SUSPEND_REQUEST);
		break;
	default:
		goto outstate;
	}
	cr_set_i_state(cr, cc, CCS_WCON_SUSREQ);
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      error:
	return (err);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_SUSPEND_RES
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_suspend_res(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct cr *cr;
	unsigned char *opt_ptr;
	size_t opt_len;
	struct CC_suspend_res *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto badprim;
	if (isdn_check_opt(opt_ptr, opt_len) < 0)
		goto badopt;
	switch (cc_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U15_SUSPEND_REQUEST:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_i_state(cr)) {
	case CCS_WCON_SUSREQ:
		break;
	default:
		goto outstate;
	}
	switch (cr_get_c_state(cr)) {
	case U15_SUSPEND_REQUEST:
		break;
	default:
		goto outstate;
	}
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badopt:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADOPT);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_SUSPEND_REJECT_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_suspend_reject_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct CC_suspend_reject_req *p = (typeof(p)) mp->b_rptr;
	struct cr *cr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Write this function\n"));
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	default:
		goto outstate;
	}
	return (QR_DONE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_RESUME_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_resume_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct CC_resume_req *p = (typeof(p)) mp->b_rptr;
	struct cr *cr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Write this function\n"));
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U0_NULL:
		/* 
		   select call reference */
		isdn_send_resume(q, cr);
		cr_timer_start(cr, t318);
		cr_set_c_state(cr, U17_RESUME_REQUEST);
		break;
	default:
		goto outstate;
	}
	return (QR_DONE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_RESUME_RES
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_resume_res(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct CC_resume_res *p = (typeof(p)) mp->b_rptr;
	struct cr *cr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Write this function\n"));
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	default:
		goto outstate;
	}
	return (QR_DONE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_RESUME_REJECT_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_resume_reject_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct CC_resume_reject_req *p = (typeof(p)) mp->b_rptr;
	struct cr *cr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Write this function\n"));
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	default:
		goto outstate;
	}
	return (QR_DONE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_REJECT_REQ
 *  -------------------------------------------------------------------------
 *  This is just a CC_RELEASE_REQ in the NNI.
 */
STATIC int
cc_reject_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct CC_reject_req *p = (typeof(p)) mp->b_rptr;
	struct cr *cr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Write this function\n"));
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U6_CALL_PRESENT:
		if (cr->fg.fg->proto.popt & ISDN_POPT_REJ)
			if ((err = isdn_send_release_complete(q, cr, p->cc_cause)) < 0)
				goto error;
		cr_set_c_state(cr, U0_NULL);
		break;
	default:
		goto outstate;
	}
	return (QR_DONE);
      error:
	return (err);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_RELEASE_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_release_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct CC_release_req *p = (typeof(p)) mp->b_rptr;
	struct cr *cr;
	int err;
	ulong cause = 0;		/* FIXME */
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Write this function\n"));
	if (!(cr = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (cr_get_c_state(cr)) {
	case U2_OVERLAP_SENDING:
		cr_timer_stop(cr, t304);
		if ((err = isdn_send_release(q, cr, CC_CAUS_CALL_ABANDONNED)) < 0)	/* cause
											   No. 6 */
			goto error;
		cr_timer_start(cr, t308);
		cr_set_c_state(cr, U19_RELEASE_REQUEST);
		break;
	case U3_OUTGOING_CALL_PROCEEDING:
		cr_timer_stop(cr, t310);
		if ((err = isdn_send_release(q, cr, CC_CAUS_CALL_ABANDONNED)) < 0)	/* cause
											   No. 6 */
			goto error;
		cr_timer_start(cr, t308);
		cr_set_c_state(cr, U19_RELEASE_REQUEST);
		break;
	case U12_DISCONNECT_INDICATION:
		if ((err = isdn_send_release(q, cr, cause)) < 0)
			goto error;
		cr_timer_start(cr, t308);
		cr_set_c_state(cr, U19_RELEASE_REQUEST);
		break;
	default:
		goto outstate;
	}
	return (QR_DONE);
      error:
	return (err);
      outstate:
	return cc_error_ack(q, cc, p->cc_primitive, CCOUTSTATE);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_RELEASE_RES
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_release_res(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct CC_release_res *p = (typeof(p)) mp->b_rptr;
	struct cr *cr;
	int err;
	(void) cc;
	(void) p;
	(void) cr;
	(void) err;
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CC_RESTART_REQ
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_restart_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct CC_restart_req *p = (typeof(p)) mp->b_rptr;
	struct cr *cr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	fixme(("Write this function\n"));
	if (!(cr = cc_find_cpc_cref(cc, 0)))	/* global call ref */
		goto badclr;
	/* 
	   any state */
	/* 
	   from global call reference */
	cr_timer_stop(cr, tall);
	if ((err = cc_release_ind(q, cr, NULL)) < 0)
		goto error;
	if ((err = cc_restart_con(q, cr)) < 0)	/* to global call reference */
		goto error;
	cr_set_c_state(cr, U0_NULL);
	isdn_free_cr(cr);
	return (QR_DONE);
      error:
	return (err);
      badclr:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADCLR);
      badprim:
	return cc_error_ack(q, cc, p->cc_primitive, CCBADPRIM);
}

/*
 *  CC_STOP_REQ
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC int
cc_stop_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct CC_stop_req *p = (typeof(p)) mp->b_rptr;
	struct cr *cr;
	int err;
	(void) cc;
	(void) p;
	(void) cr;
	(void) err;
	fixme(("Write this function\n"));
	return (-EFAULT);
}
#endif

/*
 *  CC_ Unsupported
 *  -------------------------------------------------------------------------
 *  This is for known but unsupported primitives
 */
STATIC int
cc_unsupported_prim(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	ulong prim = *((ulong *) mp->b_rptr);
	return cc_error_ack(q, cc, prim, CCNOTSUPP);
}

/*
 *  CC_ Unknown
 *  -------------------------------------------------------------------------
 *  This is for completely unknown primitives
 */
STATIC int
cc_unknown_prim(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	ulong prim = *((ulong *) mp->b_rptr);
	return cc_error_ack(q, cc, prim, CCBADPRIM);
}

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 *
 *  ISDN IO Controls
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Utility Functions
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  GET OPTIONS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_opt_get_ch(isdn_option_t * arg, struct ch *ch, int size)
{
	isdn_opt_conf_ch_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!ch)
		return (-EINVAL);
	*opt = ch->config;
	return (QR_DONE);
}
STATIC int
isdn_opt_get_tg(isdn_option_t * arg, struct tg *tg, int size)
{
	isdn_opt_conf_tg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!tg)
		return (-EINVAL);
	*opt = tg->config;
	return (QR_DONE);
}
STATIC int
isdn_opt_get_fg(isdn_option_t * arg, struct fg *fg, int size)
{
	isdn_opt_conf_fg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!fg)
		return (-EINVAL);
	*opt = fg->config;
	return (QR_DONE);
}
STATIC int
isdn_opt_get_eg(isdn_option_t * arg, struct eg *eg, int size)
{
	isdn_opt_conf_eg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!eg)
		return (-EINVAL);
	*opt = eg->config;
	return (QR_DONE);
}
STATIC int
isdn_opt_get_xg(isdn_option_t * arg, struct xg *xg, int size)
{
	isdn_opt_conf_xg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!xg)
		return (-EINVAL);
	*opt = xg->config;
	return (QR_DONE);
}
STATIC int
isdn_opt_get_dc(isdn_option_t * arg, struct dc *dc, int size)
{
	isdn_opt_conf_dc_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!dc)
		return (-EINVAL);
	*opt = dc->config;
	return (QR_DONE);
}
STATIC int
isdn_opt_get_dl(isdn_option_t * arg, struct dl *dl, int size)
{
	isdn_opt_conf_dl_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!dl)
		return (-EINVAL);
	*opt = dl->config;
	return (QR_DONE);
}
STATIC int
isdn_opt_get_df(isdn_option_t * arg, df_t * df, int size)
{
	isdn_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!df)
		return (-EINVAL);
	*opt = df->config;
	return (QR_DONE);
}

/*
 *  SET OPTIONS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_opt_set_ch(isdn_option_t * arg, struct ch *ch, int size)
{
	isdn_opt_conf_ch_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!ch)
		return (-EINVAL);
	todo(("Check channel options before setting them\n"));
	ch->config = *opt;
	return (QR_DONE);
}
STATIC int
isdn_opt_set_tg(isdn_option_t * arg, struct tg *tg, int size)
{
	isdn_opt_conf_tg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!tg)
		return (-EINVAL);
	todo(("Check transmission group options before setting them\n"));
	tg->config = *opt;
	return (QR_DONE);
}
STATIC int
isdn_opt_set_fg(isdn_option_t * arg, struct fg *fg, int size)
{
	isdn_opt_conf_fg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!fg)
		return (-EINVAL);
	todo(("Check facility group options before setting them\n"));
	fg->config = *opt;
	return (QR_DONE);
}
STATIC int
isdn_opt_set_eg(isdn_option_t * arg, struct eg *eg, int size)
{
	isdn_opt_conf_eg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!eg)
		return (-EINVAL);
	todo(("Check equipment group options before setting them\n"));
	eg->config = *opt;
	return (QR_DONE);
}
STATIC int
isdn_opt_set_xg(isdn_option_t * arg, struct xg *xg, int size)
{
	isdn_opt_conf_xg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!xg)
		return (-EINVAL);
	todo(("Check exchange group options before setting them\n"));
	xg->config = *opt;
	return (QR_DONE);
}
STATIC int
isdn_opt_set_dc(isdn_option_t * arg, struct dc *dc, int size)
{
	isdn_opt_conf_dc_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!dc)
		return (-EINVAL);
	todo(("Check d channel options before setting them\n"));
	dc->config = *opt;
	return (QR_DONE);
}
STATIC int
isdn_opt_set_dl(isdn_option_t * arg, struct dl *dl, int size)
{
	isdn_opt_conf_dl_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!dl)
		return (-EINVAL);
	todo(("Check data link options before setting them\n"));
	dl->config = *opt;
	return (QR_DONE);
}
STATIC int
isdn_opt_set_df(isdn_option_t * arg, df_t * df, int size)
{
	isdn_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!df)
		return (-EINVAL);
	todo(("Check default options before setting them\n"));
	df->config = *opt;
	return (QR_DONE);
}

/*
 *  GET Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_get_ch(isdn_config_t * arg, struct ch *ch, int size)
{
	isdn_conf_ch_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!ch || size < sizeof(*arg))
		return (-EINVAL);
	cnf->tgid = ch->tg.tg ? ch->tg.tg->id : 0;
	cnf->fgid = ch->fg.fg ? ch->fg.fg->id : 0;
	cnf->ts = ch->ts;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isdn_get_tg(isdn_config_t * arg, struct tg *tg, int size)
{
	struct dc *dc;
	struct ch *ch;
	isdn_conf_dc_t *dcc;
	isdn_conf_ch_t *chc;
	isdn_conf_tg_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!tg)
		return (-EINVAL);
	cnf->egid = tg->eg.eg ? tg->eg.eg->id : 0;
	cnf->proto = tg->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write out list of data link configurations */
	dcc = (typeof(dcc)) (arg + 1);
	for (dc = tg->dc.list; dc && size >= sizeof(*arg) + sizeof(*dcc) + sizeof(*arg);
	     dc = dc->tg.next, size -= sizeof(*arg) + sizeof(*dcc), arg =
	     (typeof(arg)) (dcc + 1), dcc = (typeof(dcc)) (arg + 1)) {
		arg->type = ISDN_OBJ_TYPE_DC;
		arg->id = dc->id;
		dcc->tgid = tg->id;	/* transmission group id */
		dcc->fgid = dc->fg.fg ? dc->fg.fg->id : 0;	/* facility group id */
		dcc->ts = dc->ts;	/* timeslot */
		dcc->sap = dc->sap;	/* address */
		dcc->proto = dc->proto;	/* protocol options */
	}
	/* 
	   write out list of channel configurations */
	chc = (typeof(chc)) (arg + 1);
	for (ch = tg->ch.list; ch && size >= sizeof(*arg) + sizeof(*chc) + sizeof(*arg);
	     ch = ch->tg.next, size -= sizeof(*arg) + sizeof(*chc), arg =
	     (typeof(arg)) (chc + 1), chc = (typeof(chc)) (arg + 1)) {
		arg->type = ISDN_OBJ_TYPE_CH;
		arg->id = ch->id;
		chc->tgid = tg->id;	/* transmission group id */
		chc->ts = ch->ts;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isdn_get_fg(isdn_config_t * arg, struct fg *fg, int size)
{
	struct dc *dc;
	struct ch *ch;
	isdn_conf_dc_t *dcc;
	isdn_conf_ch_t *chc;
	isdn_conf_fg_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!fg)
		return (-EINVAL);
	cnf->egid = fg->eg.eg ? fg->eg.eg->id : 0;
	cnf->proto = fg->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write out list of d-channel configurations */
	dcc = (typeof(dcc)) (arg + 1);
	for (dc = fg->dc.list; dc && size >= sizeof(*arg) + sizeof(*dcc) + sizeof(*arg);
	     dc = dc->fg.next, size -= sizeof(*arg) + sizeof(*dcc), arg =
	     (typeof(arg)) (dcc + 1), dcc = (typeof(dcc)) (arg + 1)) {
		arg->type = ISDN_OBJ_TYPE_DL;
		arg->id = dc->id;
		dcc->fgid = fg->id;	/* facility group id */
		dcc->tgid = dc->tg.tg ? dc->tg.tg->id : 0;	/* transmission group id */
		dcc->ts = dc->ts;
		dcc->sap = dc->sap;	/* data link address */
		dcc->proto = dc->proto;	/* data link protocol options */
	}
	/* 
	   write out list of b-channel configurations */
	chc = (typeof(chc)) (arg + 1);
	for (ch = fg->ch.list; ch && size >= sizeof(*arg) + sizeof(*chc) + sizeof(*arg);
	     ch = ch->fg.next, size -= sizeof(*arg) + sizeof(*chc), arg =
	     (typeof(arg)) (chc + 1), chc = (typeof(chc)) (arg + 1)) {
		arg->type = ISDN_OBJ_TYPE_CH;
		arg->id = ch->id;
		chc->fgid = fg->id;	/* facility group id */
		chc->tgid = ch->tg.tg ? ch->tg.tg->id : 0;	/* transmission group */
		chc->ts = ch->ts;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isdn_get_eg(isdn_config_t * arg, struct eg *eg, int size)
{
	struct fg *fg;
	struct tg *tg;
	isdn_conf_fg_t *fgc;
	isdn_conf_tg_t *tgc;
	isdn_conf_eg_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!eg)
		return (-EINVAL);
	cnf->xgid = eg->xg.xg ? eg->xg.xg->id : 0;
	cnf->proto = eg->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write out list of facility groups */
	fgc = (typeof(fgc)) (arg + 1);
	for (fg = eg->fg.list; fg && size >= sizeof(*arg) + sizeof(*fgc) + sizeof(*arg);
	     fg = fg->eg.next, size -= sizeof(*arg) + sizeof(*fgc), arg =
	     (typeof(arg)) (fgc + 1), fgc = (typeof(fgc)) (arg + 1)) {
		arg->type = ISDN_OBJ_TYPE_FG;
		arg->id = fg->id;
		fgc->egid = eg->id;	/* equipment group id */
		fgc->proto = fg->proto;
	}
	/* 
	   write out list of transmission groups */
	tgc = (typeof(tgc)) (arg + 1);
	for (tg = eg->tg.list; tg && size >= sizeof(*arg) + sizeof(*tgc) + sizeof(*arg);
	     tg = tg->eg.next, size -= sizeof(*arg) + sizeof(*tgc), arg =
	     (typeof(arg)) (tgc + 1), tgc = (typeof(tgc)) (arg + 1)) {
		arg->type = ISDN_OBJ_TYPE_FG;
		arg->id = tg->id;
		tgc->egid = eg->id;	/* equipment group id */
		tgc->proto = tg->proto;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isdn_get_xg(isdn_config_t * arg, struct xg *xg, int size)
{
	struct eg *eg;
	isdn_conf_eg_t *egc;
	isdn_conf_xg_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!xg)
		return (-EINVAL);
	cnf->proto = xg->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write out the list of equipment group configurations */
	egc = (typeof(egc)) (arg + 1);
	for (eg = xg->eg.list; eg && size >= sizeof(*arg) + sizeof(*egc) + sizeof(*arg);
	     eg = eg->xg.next, size -= sizeof(*arg) + sizeof(*egc), arg =
	     (typeof(arg)) (egc + 1), egc = (typeof(egc)) (arg + 1)) {
		arg->type = ISDN_OBJ_TYPE_XG;
		arg->id = eg->id;
		egc->xgid = xg->id;	/* exchange group id */
		egc->proto = eg->proto;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isdn_get_dc(isdn_config_t * arg, struct dc *dc, int size)
{
	struct dl *dl;
	isdn_conf_dl_t *dlc;
	isdn_conf_dc_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!dc)
		return (-EINVAL);
	cnf->fgid = dc->fg.fg ? dc->fg.fg->id : 0;
	cnf->tgid = dc->tg.tg ? dc->tg.tg->id : 0;
	cnf->ts = dc->ts;
	cnf->sap = dc->sap;
	cnf->proto = dc->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write out list of data link configurations */
	dlc = (typeof(dlc)) (arg + 1);
	for (dl = dc->dl.list; dl && size >= sizeof(*arg) + sizeof(*dlc) + sizeof(*arg);
	     dl = dl->dc.next, size -= sizeof(*arg) + sizeof(*dlc), arg =
	     (typeof(arg)) (dlc + 1), dlc = (typeof(dlc)) (arg + 1)) {
		arg->type = ISDN_OBJ_TYPE_DL;
		arg->id = dl->id;
		dlc->muxid = dl->u.mux.index;
		dlc->dcid = dc->id;
		dlc->dlc = dl->dlc;
		dlc->proto = dl->proto;
	}
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isdn_get_dl(isdn_config_t * arg, struct dl *dl, int size)
{
	isdn_conf_dl_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!dl)
		return (-EINVAL);
	cnf->muxid = dl->u.mux.index;
	cnf->dcid = dl->dc.dc ? dl->dc.dc->id : 0;
	cnf->dlc = dl->dlc;
	cnf->proto = dl->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isdn_get_df(isdn_config_t * arg, df_t * df, int size)
{
	struct xg *xg;
	isdn_conf_xg_t *xgc;
	isdn_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!df)
		return (-EINVAL);
	cnf->proto = df->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write out list of exchange groups */
	xgc = (typeof(xgc)) (arg + 1);
	for (xg = df->xg.list; xg && size >= sizeof(*arg) + sizeof(*xgc) + sizeof(*arg);
	     xg = xg->next, size -= sizeof(*arg) + sizeof(*xgc), arg =
	     (typeof(arg)) (xgc + 1), xgc = (typeof(xgc)) (arg + 1)) {
		arg->type = ISDN_OBJ_TYPE_XG;
		arg->id = xg->id;
		xgc->proto = xg->proto;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}

/*
 *  ADD Object Configuration
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_add_ch(isdn_config_t * arg, struct ch *ch, int size, int force, int test)
{
	struct dc *dc;
	struct tg *tg = NULL;
	struct fg *fg = NULL;
	isdn_conf_ch_t *cnf = (typeof(cnf)) (arg + 1);
	if (ch || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->tgid)
		tg = tg_lookup(cnf->tgid);
	if (cnf->fgid)
		fg = fg_lookup(cnf->fgid);
	if (!tg || !fg)
		return (-EINVAL);
	/* 
	   timeslot must be unique within the transmission group */
	for (ch = tg->ch.list; ch; ch = ch->tg.next)
		if (cnf->ts == ch->ts)
			return (-EINVAL);
	for (dc = tg->dc.list; dc; dc = dc->tg.next)
		if (cnf->ts == dc->ts)
			return (-EINVAL);
	if (!test) {
		if (!(ch = isdn_alloc_ch(ch_get_id(arg->id), fg, tg, cnf->ts)))
			return (-ENOMEM);
		arg->id = ch->id;
	}
	return (QR_DONE);
}
STATIC int
isdn_add_tg(isdn_config_t * arg, struct tg *tg, int size, int force, int test)
{
	struct eg *eg = NULL;
	isdn_conf_tg_t *cnf = (typeof(cnf)) (arg + 1);
	if (tg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->egid)
		eg = eg_lookup(cnf->egid);
	if (!eg)
		return (-EINVAL);
	if (!test) {
		if (!(tg = isdn_alloc_tg(tg_get_id(arg->id), eg)))
			return (-ENOMEM);
		arg->id = tg->id;
	}
	return (QR_DONE);
}
STATIC int
isdn_add_fg(isdn_config_t * arg, struct fg *fg, int size, int force, int test)
{
	struct eg *eg = NULL;
	isdn_conf_fg_t *cnf = (typeof(cnf)) (arg + 1);
	if (fg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->egid)
		eg = eg_lookup(cnf->egid);
	if (!eg)
		return (-EINVAL);
	if (!test) {
		if (!(fg = isdn_alloc_fg(fg_get_id(arg->id), eg)))
			return (-ENOMEM);
		arg->id = fg->id;
		fg->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isdn_add_eg(isdn_config_t * arg, struct eg *eg, int size, int force, int test)
{
	struct xg *xg = NULL;
	isdn_conf_eg_t *cnf = (typeof(cnf)) (arg + 1);
	if (eg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->xgid)
		xg = xg_lookup(cnf->xgid);
	if (!xg)
		return (-EINVAL);
	if (!test) {
		if (!(eg = isdn_alloc_eg(eg_get_id(arg->id), xg)))
			return (-ENOMEM);
		eg->proto = cnf->proto;
		arg->id = eg->id;
	}
	return (QR_DONE);
}
STATIC int
isdn_add_xg(isdn_config_t * arg, struct xg *xg, int size, int force, int test)
{
	isdn_conf_xg_t *cnf = (typeof(cnf)) (arg + 1);
	if (xg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!test) {
		if (!(xg = isdn_alloc_xg(xg_get_id(arg->id))))
			return (-ENOMEM);
		xg->proto = cnf->proto;
		arg->id = xg->id;
	}
	return (QR_DONE);
}
STATIC int
isdn_add_dc(isdn_config_t * arg, struct dc *dc, int size, int force, int test)
{
	struct ch *ch;
	struct tg *tg = NULL;
	struct fg *fg = NULL;
	isdn_conf_dc_t *cnf = (typeof(cnf)) (arg + 1);
	if (dc || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->fgid)
		fg = fg_lookup(cnf->fgid);
	if (cnf->tgid)
		tg = tg_lookup(cnf->tgid);
	if (!tg || !fg)
		return (-EINVAL);
	for (ch = tg->ch.list; ch; ch = ch->tg.next)
		if (cnf->ts == ch->ts)
			return (-EINVAL);
	for (dc = tg->dc.list; dc; dc = dc->tg.next)
		if (cnf->ts == dc->ts)
			return (-EINVAL);
	if (!test) {
		if (!(dc = isdn_alloc_dc(dc_get_id(arg->id), fg, tg)))
			return (-ENOMEM);
		dc->ts = cnf->ts;
		dc->sap = cnf->sap;
		dc->proto = cnf->proto;
		arg->id = dc->id;
	}
	return (QR_DONE);
}
STATIC int
isdn_add_dl(isdn_config_t * arg, struct dl *dl, int size, int force, int test)
{
	struct dc *dc = NULL;
	isdn_conf_dl_t *cnf = (typeof(cnf)) (arg + 1);
	if (dl || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->dcid)
		dc = dc_lookup(cnf->dcid);
	if (!dc)
		return (-EINVAL);
	for (dl = dc->dl.list; dl; dl = dl->dc.next)
		if (cnf->dlc.dl_tei == dl->dlc.dl_tei)
			return (-EINVAL);
	for (dl = master.dl.list; dl; dl = dl->next)
		if (dl->u.mux.index == cnf->muxid) {
			if (dl->id)
				return (-EINVAL);
			break;
		}
	if (!dl)
		return (-EINVAL);
	if (!test) {
		/* 
		   fill out structure */
		dl->id = dl_get_id(arg->id);
		dl->dlc = cnf->dlc;
		dl->proto = cnf->proto;
		isdn_link_dl(dl->id, dl, dc);
		arg->id = dl->id;
	}
	return (QR_DONE);
}
STATIC int
isdn_add_df(isdn_config_t * arg, df_t * df, int size, int force, int test)
{
	isdn_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if (df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	return (-EINVAL);
}

/*
 *  CHA Object Configuration
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_cha_ch(isdn_config_t * arg, struct ch *ch, int size, int force, int test)
{
	struct ch *c;
	isdn_conf_ch_t *cnf = (typeof(cnf)) (arg + 1);
	if (!ch || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->tgid && cnf->tgid != ch->tg.tg->id)
		return (-EINVAL);
	/* 
	   can't change to existing ts */
	for (c = ch->tg.tg->ch.list; c; c = c->tg.next)
		if (c->ts == cnf->ts)
			return (-EINVAL);
	if (!force) {
		/* 
		   bound for management or call control */
		if (ch->bind.mgm || ch->bind.icc || ch->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (ch->bind.mnt || ch->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in call */
		if (ch->cr.next)
			return (-EBUSY);
	}
	if (!test) {
		ch->ts = cnf->ts;
	}
	return (QR_DONE);
}
STATIC int
isdn_cha_tg(isdn_config_t * arg, struct tg *tg, int size, int force, int test)
{
	isdn_conf_tg_t *cnf = (typeof(cnf)) (arg + 1);
	if (!tg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->egid && cnf->egid != tg->eg.eg->id)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have channels */
		if (tg->ch.list)
			return (-EBUSY);
		/* 
		   we have data links */
		if (tg->dc.list)
			return (-EBUSY);
		/* 
		   bound for management or call control */
		if (tg->bind.mgm || tg->bind.icc || tg->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (tg->bind.mnt || tg->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in management */
		if (tg->mgm.next)
			return (-EBUSY);
	}
	if (!test) {
	}
	return (QR_DONE);
}
STATIC int
isdn_cha_fg(isdn_config_t * arg, struct fg *fg, int size, int force, int test)
{
	isdn_conf_fg_t *cnf = (typeof(cnf)) (arg + 1);
	if (!fg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->egid && cnf->egid != fg->eg.eg->id)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have channels */
		if (fg->ch.list)
			return (-EBUSY);
		/* 
		   we have data links */
		if (fg->dc.list)
			return (-EBUSY);
		/* 
		   bound for management or call control */
		if (fg->bind.mgm || fg->bind.icc || fg->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (fg->bind.mnt || fg->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in management or monitoring */
		if (fg->mgm.next || fg->xry.next)
			return (-EBUSY);
	}
	if (!test) {
		fg->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isdn_cha_eg(isdn_config_t * arg, struct eg *eg, int size, int force, int test)
{
	isdn_conf_eg_t *cnf = (typeof(cnf)) (arg + 1);
	if (!eg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->xgid && cnf->xgid != eg->xg.xg->id)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have transmission groups */
		if (eg->tg.list)
			return (-EBUSY);
		/* 
		   we have facility groups */
		if (eg->fg.list)
			return (-EBUSY);
		/* 
		   bound for management or call control */
		if (eg->bind.mgm || eg->bind.icc || eg->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (eg->bind.mnt || eg->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in management */
		if (eg->mgm.next)
			return (-EBUSY);
	}
	if (!test) {
		eg->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isdn_cha_xg(isdn_config_t * arg, struct xg *xg, int size, int force, int test)
{
	isdn_conf_xg_t *cnf = (typeof(cnf)) (arg + 1);
	if (!xg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have equipment groups */
		if (xg->eg.list)
			return (-EBUSY);
		/* 
		   bound for management or call control */
		if (xg->bind.mgm || xg->bind.icc || xg->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (xg->bind.mnt || xg->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in management */
		if (xg->mgm.next)
			return (-EBUSY);
	}
	if (!test) {
		xg->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isdn_cha_dc(isdn_config_t * arg, struct dc *dc, int size, int force, int test)
{
	isdn_conf_dc_t *cnf = (typeof(cnf)) (arg + 1);
	if (!dc || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->tgid && (!dc->tg.tg || cnf->tgid != dc->tg.tg->id))
		return (-EINVAL);
	if (cnf->fgid && (!dc->fg.fg || cnf->fgid != dc->fg.fg->id))
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal datastructures */
		if (dc->fg.fg || dc->tg.tg)
			return (-EBUSY);
		/* 
		   engaged in management or monitoring */
		if (dc->mgm.next || dc->xry.next)
			return (-EBUSY);
	}
	if (!test) {
		dc->ts = cnf->ts;
		dc->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isdn_cha_dl(isdn_config_t * arg, struct dl *dl, int size, int force, int test)
{
	isdn_conf_dl_t *cnf = (typeof(cnf)) (arg + 1);
	if (!dl || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->muxid && cnf->muxid != dl->u.mux.index)
		return (-EINVAL);
	if (cnf->dcid && (!dl->dc.dc || cnf->dcid != dl->dc.dc->id))
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal datastructures */
		if (dl->dc.dc)
			return (-EBUSY);
		/* 
		   engaged in management or monitoring */
		if (dl->mgm.next || dl->xry.next)
			return (-EBUSY);
	}
	if (!test) {
		dl->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isdn_cha_df(isdn_config_t * arg, df_t * df, int size, int force, int test)
{
	isdn_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
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
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_del_ch(isdn_config_t * arg, struct ch *ch, int size, int force, int test)
{
	if (!ch)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound for management or call control */
		if (ch->bind.mgm || ch->bind.icc || ch->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (ch->bind.mnt || ch->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in call */
		if (ch->cr.next)
			return (-EBUSY);
	}
	if (!test) {
		isdn_free_ch(ch);
	}
	return (QR_DONE);
}
STATIC int
isdn_del_tg(isdn_config_t * arg, struct tg *tg, int size, int force, int test)
{
	if (!tg)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have channels */
		if (tg->ch.list)
			return (-EBUSY);
		/* 
		   we have data links */
		if (tg->dc.list)
			return (-EBUSY);
		/* 
		   bound for management or call control */
		if (tg->bind.mgm || tg->bind.icc || tg->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (tg->bind.mnt || tg->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in management */
		if (tg->mgm.next)
			return (-EBUSY);
	}
	if (!test) {
		isdn_free_tg(tg);
	}
	return (QR_DONE);
}
STATIC int
isdn_del_fg(isdn_config_t * arg, struct fg *fg, int size, int force, int test)
{
	if (!fg)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have channels */
		if (fg->ch.list)
			return (-EBUSY);
		/* 
		   we have data links */
		if (fg->dc.list)
			return (-EBUSY);
		/* 
		   bound for management or call control */
		if (fg->bind.mgm || fg->bind.icc || fg->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (fg->bind.mnt || fg->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in management or monitoring */
		if (fg->mgm.next || fg->xry.next)
			return (-EBUSY);
	}
	if (!test) {
		isdn_free_fg(fg);
	}
	return (QR_DONE);
}
STATIC int
isdn_del_eg(isdn_config_t * arg, struct eg *eg, int size, int force, int test)
{
	if (!eg)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have transmission groups */
		if (eg->tg.list)
			return (-EBUSY);
		/* 
		   we have facility groups */
		if (eg->fg.list)
			return (-EBUSY);
		/* 
		   bound for management or call control */
		if (eg->bind.mgm || eg->bind.icc || eg->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (eg->bind.mnt || eg->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in management */
		if (eg->mgm.next)
			return (-EBUSY);
	}
	if (!test) {
		isdn_free_eg(eg);
	}
	return (QR_DONE);
}
STATIC int
isdn_del_xg(isdn_config_t * arg, struct xg *xg, int size, int force, int test)
{
	if (!xg)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have equipment groups */
		if (xg->eg.list)
			return (-EBUSY);
		/* 
		   bound for management or call control */
		if (xg->bind.mgm || xg->bind.icc || xg->bind.ogc)
			return (-EBUSY);
		/* 
		   bound for maintenance or monitoring */
		if (xg->bind.mnt || xg->bind.xry)
			return (-EBUSY);
		/* 
		   engaged in management */
		if (xg->mgm.next)
			return (-EBUSY);
	}
	if (!test) {
		isdn_free_xg(xg);
	}
	return (QR_DONE);
}
STATIC int
isdn_del_dc(isdn_config_t * arg, struct dc *dc, int size, int force, int test)
{
	if (!dc)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal data structures */
		if (dc->fg.fg || dc->tg.tg)
			return (-EBUSY);
		/* 
		   engaged in management or monitoring */
		if (dc->mgm.next || dc->xry.next)
			return (-EBUSY);
	}
	if (!test) {
		isdn_free_dc(dc);
	}
	return (QR_DONE);
}
STATIC int
isdn_del_dl(isdn_config_t * arg, struct dl *dl, int size, int force, int test)
{
	if (!dl)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal datastructures */
		if (dl->dc.dc)
			return (-EBUSY);
		/* 
		   engaged in management or monitoring */
		if (dl->mgm.next || dl->xry.next)
			return (-EBUSY);
	}
	if (!test) {
		isdn_unlink_dl(dl);
	}
	return (QR_DONE);
}
STATIC int
isdn_del_df(isdn_config_t * arg, df_t * df, int size, int force, int test)
{
	isdn_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if (!df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		// isdn_free_df(df);
	}
	return (QR_DONE);
}

/*
 *  GET State
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_sta_ch(isdn_statem_t * arg, struct ch *ch, int size)
{
	struct cc *cc;
	ulong *p;
	isdn_statem_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = ch->flags;
	arg->state = ch->state;
	sta->timers = ch->timers;
	sta->mgm_bind = ch->bind.mgm ? ch->bind.mgm->id : 0;
	sta->mnt_bind = ch->bind.mnt ? ch->bind.mnt->id : 0;
	sta->xry_bind = ch->bind.xry ? ch->bind.xry->id : 0;
	sta->icc_bind = ch->bind.icc ? ch->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = ch->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isdn_sta_tg(isdn_statem_t * arg, struct tg *tg, int size)
{
	struct cc *cc;
	ulong *p;
	isdn_statem_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = tg->flags;
	arg->state = tg->state;
	sta->timers = tg->timers;
	sta->mgm_bind = tg->bind.mgm ? tg->bind.mgm->id : 0;
	sta->mnt_bind = tg->bind.mnt ? tg->bind.mnt->id : 0;
	sta->xry_bind = tg->bind.xry ? tg->bind.xry->id : 0;
	sta->icc_bind = tg->bind.icc ? tg->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = tg->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isdn_sta_fg(isdn_statem_t * arg, struct fg *fg, int size)
{
	struct cc *cc;
	ulong *p;
	isdn_statem_fg_t *sta = (typeof(sta)) (arg + 1);
	if (!fg || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = fg->flags;
	arg->state = fg->state;
	sta->timers = fg->timers;
	sta->mgm_bind = fg->bind.mgm ? fg->bind.mgm->id : 0;
	sta->mnt_bind = fg->bind.mnt ? fg->bind.mnt->id : 0;
	sta->xry_bind = fg->bind.xry ? fg->bind.xry->id : 0;
	sta->icc_bind = fg->bind.icc ? fg->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = fg->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isdn_sta_eg(isdn_statem_t * arg, struct eg *eg, int size)
{
	struct cc *cc;
	ulong *p;
	isdn_statem_eg_t *sta = (typeof(sta)) (arg + 1);
	if (!eg || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = eg->flags;
	arg->state = eg->state;
	sta->timers = eg->timers;
	sta->mgm_bind = eg->bind.mgm ? eg->bind.mgm->id : 0;
	sta->mnt_bind = eg->bind.mnt ? eg->bind.mnt->id : 0;
	sta->xry_bind = eg->bind.xry ? eg->bind.xry->id : 0;
	sta->icc_bind = eg->bind.icc ? eg->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = eg->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isdn_sta_xg(isdn_statem_t * arg, struct xg *xg, int size)
{
	struct cc *cc;
	ulong *p;
	isdn_statem_xg_t *sta = (typeof(sta)) (arg + 1);
	if (!xg || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = xg->flags;
	arg->state = xg->state;
	sta->timers = xg->timers;
	sta->mgm_bind = xg->bind.mgm ? xg->bind.mgm->id : 0;
	sta->mnt_bind = xg->bind.mnt ? xg->bind.mnt->id : 0;
	sta->xry_bind = xg->bind.xry ? xg->bind.xry->id : 0;
	sta->icc_bind = xg->bind.icc ? xg->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = xg->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isdn_sta_dc(isdn_statem_t * arg, struct dc *dc, int size)
{
	isdn_statem_dc_t *sta = (typeof(sta)) (arg + 1);
	if (!dc || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	arg->flags = dc->flags;
	arg->state = dc->state;
	sta->timers = dc->timers;
	sta->mgm_bind = dc->bind.mgm ? dc->bind.mgm->id : 0;
	sta->xry_bind = dc->bind.xry ? dc->bind.xry->id : 0;
	return (QR_DONE);
}
STATIC int
isdn_sta_dl(isdn_statem_t * arg, struct dl *dl, int size)
{
	isdn_statem_dl_t *sta = (typeof(sta)) (arg + 1);
	if (!dl || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	arg->flags = dl->flags;
	arg->state = dl->state;
	sta->timers = dl->timers;
	sta->mgm_bind = dl->bind.mgm ? dl->bind.mgm->id : 0;
	sta->xry_bind = dl->bind.xry ? dl->bind.xry->id : 0;
	return (QR_DONE);
}
STATIC int
isdn_sta_df(isdn_statem_t * arg, df_t * df, int size)
{
	struct cc *cc;
	ulong *p;
	isdn_statem_df_t *sta = (typeof(sta)) (arg + 1);
	if (!df || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = 0;
	arg->state = 0;
	sta->mgm_bind = df->bind.mgm ? df->bind.mgm->id : 0;
	sta->mnt_bind = df->bind.mnt ? df->bind.mnt->id : 0;
	sta->xry_bind = df->bind.xry ? df->bind.xry->id : 0;
	sta->icc_bind = df->bind.icc ? df->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = df->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}

/*
 *  GET Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_not_get_ch(isdn_notify_t * arg, struct ch *ch)
{
	if (!ch)
		return (-EINVAL);
	arg->notify.events = ch->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_get_tg(isdn_notify_t * arg, struct tg *tg)
{
	if (!tg)
		return (-EINVAL);
	arg->notify.events = tg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_get_fg(isdn_notify_t * arg, struct fg *fg)
{
	if (!fg)
		return (-EINVAL);
	arg->notify.events = fg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_get_eg(isdn_notify_t * arg, struct eg *eg)
{
	if (!eg)
		return (-EINVAL);
	arg->notify.events = eg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_get_xg(isdn_notify_t * arg, struct xg *xg)
{
	if (!xg)
		return (-EINVAL);
	arg->notify.events = xg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_get_dc(isdn_notify_t * arg, struct dc *dc)
{
	if (!dc)
		return (-EINVAL);
	arg->notify.events = dc->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_get_dl(isdn_notify_t * arg, struct dl *dl)
{
	if (!dl)
		return (-EINVAL);
	arg->notify.events = dl->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_get_df(isdn_notify_t * arg, df_t * df)
{
	if (!df)
		return (-EINVAL);
	arg->notify.events = df->notify.events;
	return (QR_DONE);
}

/*
 *  SET Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_not_set_ch(isdn_notify_t * arg, struct ch *ch)
{
	if (!ch)
		return (-EINVAL);
	ch->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_set_tg(isdn_notify_t * arg, struct tg *tg)
{
	if (!tg)
		return (-EINVAL);
	tg->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_set_fg(isdn_notify_t * arg, struct fg *fg)
{
	if (!fg)
		return (-EINVAL);
	fg->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_set_eg(isdn_notify_t * arg, struct eg *eg)
{
	if (!eg)
		return (-EINVAL);
	eg->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_set_xg(isdn_notify_t * arg, struct xg *xg)
{
	if (!xg)
		return (-EINVAL);
	xg->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_set_dc(isdn_notify_t * arg, struct dc *dc)
{
	if (!dc)
		return (-EINVAL);
	dc->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_set_dl(isdn_notify_t * arg, struct dl *dl)
{
	if (!dl)
		return (-EINVAL);
	dl->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_set_df(isdn_notify_t * arg, df_t * df)
{
	if (!df)
		return (-EINVAL);
	df->notify.events |= arg->notify.events;
	return (QR_DONE);
}

/*
 *  CLR Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_not_clr_ch(isdn_notify_t * arg, struct ch *ch)
{
	if (!ch)
		return (-EINVAL);
	ch->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_clr_tg(isdn_notify_t * arg, struct tg *tg)
{
	if (!tg)
		return (-EINVAL);
	tg->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_clr_fg(isdn_notify_t * arg, struct fg *fg)
{
	if (!fg)
		return (-EINVAL);
	fg->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_clr_eg(isdn_notify_t * arg, struct eg *eg)
{
	if (!eg)
		return (-EINVAL);
	eg->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_clr_xg(isdn_notify_t * arg, struct xg *xg)
{
	if (!xg)
		return (-EINVAL);
	xg->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_clr_dc(isdn_notify_t * arg, struct dc *dc)
{
	if (!dc)
		return (-EINVAL);
	dc->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_clr_dl(isdn_notify_t * arg, struct dl *dl)
{
	if (!dl)
		return (-EINVAL);
	dl->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isdn_not_clr_df(isdn_notify_t * arg, df_t * df)
{
	if (!df)
		return (-EINVAL);
	df->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}

/*
 *  BLO Managment
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_mgmt_blo_ch(isdn_mgmt_t * arg, struct ch *ch)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_blo_tg(isdn_mgmt_t * arg, struct tg *tg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_blo_fg(isdn_mgmt_t * arg, struct fg *fg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_blo_eg(isdn_mgmt_t * arg, struct eg *eg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_blo_xg(isdn_mgmt_t * arg, struct xg *xg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_blo_dc(isdn_mgmt_t * arg, struct dc *dc)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_blo_dl(isdn_mgmt_t * arg, struct dl *dl)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_blo_df(isdn_mgmt_t * arg, df_t * df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  UBL Managment
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_mgmt_ubl_ch(isdn_mgmt_t * arg, struct ch *ch)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_ubl_tg(isdn_mgmt_t * arg, struct tg *tg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_ubl_fg(isdn_mgmt_t * arg, struct fg *fg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_ubl_eg(isdn_mgmt_t * arg, struct eg *eg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_ubl_xg(isdn_mgmt_t * arg, struct xg *xg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_ubl_dc(isdn_mgmt_t * arg, struct dc *dc)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_ubl_dl(isdn_mgmt_t * arg, struct dl *dl)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_ubl_df(isdn_mgmt_t * arg, df_t * df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  RES Managment
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_mgmt_res_ch(isdn_mgmt_t * arg, struct ch *ch)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_res_tg(isdn_mgmt_t * arg, struct tg *tg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_res_fg(isdn_mgmt_t * arg, struct fg *fg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_res_eg(isdn_mgmt_t * arg, struct eg *eg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_res_xg(isdn_mgmt_t * arg, struct xg *xg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_res_dc(isdn_mgmt_t * arg, struct dc *dc)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_res_dl(isdn_mgmt_t * arg, struct dl *dl)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_res_df(isdn_mgmt_t * arg, df_t * df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  QRY Managment
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_mgmt_qry_ch(isdn_mgmt_t * arg, struct ch *ch)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_qry_tg(isdn_mgmt_t * arg, struct tg *tg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_qry_fg(isdn_mgmt_t * arg, struct fg *fg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_qry_eg(isdn_mgmt_t * arg, struct eg *eg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_qry_xg(isdn_mgmt_t * arg, struct xg *xg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_qry_dc(isdn_mgmt_t * arg, struct dc *dc)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_qry_dl(isdn_mgmt_t * arg, struct dl *dl)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isdn_mgmt_qry_df(isdn_mgmt_t * arg, df_t * df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  GET Statistics Period
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_statp_get_ch(isdn_stats_t * arg, struct ch *ch, int size)
{
	isdn_stats_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = ch->statsp;
	return (QR_DONE);
}
STATIC int
isdn_statp_get_tg(isdn_stats_t * arg, struct tg *tg, int size)
{
	isdn_stats_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = tg->statsp;
	return (QR_DONE);
}
STATIC int
isdn_statp_get_fg(isdn_stats_t * arg, struct fg *fg, int size)
{
	isdn_stats_fg_t *sta = (typeof(sta)) (arg + 1);
	if (!fg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = fg->statsp;
	return (QR_DONE);
}
STATIC int
isdn_statp_get_eg(isdn_stats_t * arg, struct eg *eg, int size)
{
	isdn_stats_eg_t *sta = (typeof(sta)) (arg + 1);
	if (!eg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = eg->statsp;
	return (QR_DONE);
}
STATIC int
isdn_statp_get_xg(isdn_stats_t * arg, struct xg *xg, int size)
{
	isdn_stats_xg_t *sta = (typeof(sta)) (arg + 1);
	if (!xg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = xg->statsp;
	return (QR_DONE);
}
STATIC int
isdn_statp_get_dc(isdn_stats_t * arg, struct dc *dc, int size)
{
	isdn_stats_dc_t *sta = (typeof(sta)) (arg + 1);
	if (!dc || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = dc->statsp;
	return (QR_DONE);
}
STATIC int
isdn_statp_get_dl(isdn_stats_t * arg, struct dl *dl, int size)
{
	isdn_stats_dl_t *sta = (typeof(sta)) (arg + 1);
	if (!dl || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = dl->statsp;
	return (QR_DONE);
}
STATIC int
isdn_statp_get_df(isdn_stats_t * arg, df_t * df, int size)
{
	isdn_stats_df_t *sta = (typeof(sta)) (arg + 1);
	if (!df || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = df->statsp;
	return (QR_DONE);
}

/*
 *  SET Statistics Period
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_statp_set_ch(isdn_stats_t * arg, struct ch *ch, int size)
{
	isdn_stats_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	ch->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isdn_statp_set_tg(isdn_stats_t * arg, struct tg *tg, int size)
{
	isdn_stats_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	tg->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isdn_statp_set_fg(isdn_stats_t * arg, struct fg *fg, int size)
{
	isdn_stats_fg_t *sta = (typeof(sta)) (arg + 1);
	if (!fg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	fg->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isdn_statp_set_eg(isdn_stats_t * arg, struct eg *eg, int size)
{
	isdn_stats_eg_t *sta = (typeof(sta)) (arg + 1);
	if (!eg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	eg->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isdn_statp_set_xg(isdn_stats_t * arg, struct xg *xg, int size)
{
	isdn_stats_xg_t *sta = (typeof(sta)) (arg + 1);
	if (!xg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	xg->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isdn_statp_set_dc(isdn_stats_t * arg, struct dc *dc, int size)
{
	isdn_stats_dc_t *sta = (typeof(sta)) (arg + 1);
	if (!dc || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	dc->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isdn_statp_set_dl(isdn_stats_t * arg, struct dl *dl, int size)
{
	isdn_stats_dl_t *sta = (typeof(sta)) (arg + 1);
	if (!dl || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	dl->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isdn_statp_set_df(isdn_stats_t * arg, df_t * df, int size)
{
	isdn_stats_df_t *sta = (typeof(sta)) (arg + 1);
	if (!df || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	df->statsp = *sta;
	return (QR_DONE);
}

/*
 *  GET Statistics
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_stat_get_ch(isdn_stats_t * arg, struct ch *ch, int size)
{
	isdn_stats_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = ch->stats;
	return (QR_DONE);
}
STATIC int
isdn_stat_get_tg(isdn_stats_t * arg, struct tg *tg, int size)
{
	isdn_stats_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = tg->stats;
	return (QR_DONE);
}
STATIC int
isdn_stat_get_fg(isdn_stats_t * arg, struct fg *fg, int size)
{
	isdn_stats_fg_t *sta = (typeof(sta)) (arg + 1);
	if (!fg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = fg->stats;
	return (QR_DONE);
}
STATIC int
isdn_stat_get_eg(isdn_stats_t * arg, struct eg *eg, int size)
{
	isdn_stats_eg_t *sta = (typeof(sta)) (arg + 1);
	if (!eg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = eg->stats;
	return (QR_DONE);
}
STATIC int
isdn_stat_get_xg(isdn_stats_t * arg, struct xg *xg, int size)
{
	isdn_stats_xg_t *sta = (typeof(sta)) (arg + 1);
	if (!xg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = xg->stats;
	return (QR_DONE);
}
STATIC int
isdn_stat_get_dc(isdn_stats_t * arg, struct dc *dc, int size)
{
	isdn_stats_dc_t *sta = (typeof(sta)) (arg + 1);
	if (!dc || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = dc->stats;
	return (QR_DONE);
}
STATIC int
isdn_stat_get_dl(isdn_stats_t * arg, struct dl *dl, int size)
{
	isdn_stats_dl_t *sta = (typeof(sta)) (arg + 1);
	if (!dl || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = dl->stats;
	return (QR_DONE);
}
STATIC int
isdn_stat_get_df(isdn_stats_t * arg, df_t * df, int size)
{
	isdn_stats_df_t *sta = (typeof(sta)) (arg + 1);
	if (!df || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = df->stats;
	return (QR_DONE);
}

/*
 *  CLR Statistics
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isdn_stat_clr_ch(isdn_stats_t * arg, struct ch *ch, int size)
{
	uchar *s, *d;
	isdn_stats_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & ch->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isdn_stat_clr_tg(isdn_stats_t * arg, struct tg *tg, int size)
{
	uchar *s, *d;
	isdn_stats_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & tg->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isdn_stat_clr_fg(isdn_stats_t * arg, struct fg *fg, int size)
{
	uchar *s, *d;
	isdn_stats_fg_t *sta = (typeof(sta)) (arg + 1);
	if (!fg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & fg->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isdn_stat_clr_eg(isdn_stats_t * arg, struct eg *eg, int size)
{
	uchar *s, *d;
	isdn_stats_eg_t *sta = (typeof(sta)) (arg + 1);
	if (!eg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & eg->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isdn_stat_clr_xg(isdn_stats_t * arg, struct xg *xg, int size)
{
	uchar *s, *d;
	isdn_stats_xg_t *sta = (typeof(sta)) (arg + 1);
	if (!xg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & xg->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isdn_stat_clr_dc(isdn_stats_t * arg, struct dc *dc, int size)
{
	uchar *s, *d;
	isdn_stats_dc_t *sta = (typeof(sta)) (arg + 1);
	if (!dc || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & dc->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isdn_stat_clr_dl(isdn_stats_t * arg, struct dl *dl, int size)
{
	uchar *s, *d;
	isdn_stats_dl_t *sta = (typeof(sta)) (arg + 1);
	if (!dl || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & dl->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isdn_stat_clr_df(isdn_stats_t * arg, df_t * df, int size)
{
	uchar *s, *d;
	isdn_stats_df_t *sta = (typeof(sta)) (arg + 1);
	if (!df || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & df->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
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
 *  ISDN_IOCGOPTIONS
 *  ------------------------------------
 *  Get configuration options by object type and id.
 */
STATIC int
isdn_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isdn_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case ISDN_OBJ_TYPE_CH:
				return isdn_opt_get_ch(arg, ch_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_TG:
				return isdn_opt_get_tg(arg, tg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_FG:
				return isdn_opt_get_fg(arg, fg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_EG:
				return isdn_opt_get_eg(arg, eg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_XG:
				return isdn_opt_get_xg(arg, xg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_DC:
				return isdn_opt_get_dc(arg, dc_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_DL:
				return isdn_opt_get_dl(arg, dl_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_DF:
				return isdn_opt_get_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCSOPTIONS
 *  ------------------------------------
 *  Set configuration options by object type and id.
 */
STATIC int
isdn_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isdn_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case ISDN_OBJ_TYPE_CH:
				return isdn_opt_set_ch(arg, ch_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_TG:
				return isdn_opt_set_tg(arg, tg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_FG:
				return isdn_opt_set_fg(arg, fg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_EG:
				return isdn_opt_set_eg(arg, eg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_XG:
				return isdn_opt_set_xg(arg, xg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_DC:
				return isdn_opt_set_dc(arg, dc_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_DL:
				return isdn_opt_set_dl(arg, dl_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_DF:
				return isdn_opt_set_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCGCONFIG
 *  ------------------------------------
 *  Get configuration by object type and id.  Also gets the configuration of as
 *  many children of the object as will fit in the provided buffer.
 */
STATIC int
isdn_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isdn_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			switch (arg->type) {
			case ISDN_OBJ_TYPE_CH:
				return isdn_get_ch(arg, ch_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_TG:
				return isdn_get_tg(arg, tg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_FG:
				return isdn_get_fg(arg, fg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_EG:
				return isdn_get_eg(arg, eg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_XG:
				return isdn_get_xg(arg, xg_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_DC:
				return isdn_get_dc(arg, dc_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_DL:
				return isdn_get_dl(arg, dl_lookup(arg->id), size);
			case ISDN_OBJ_TYPE_DF:
				return isdn_get_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCSCONFIG
 *  ------------------------------------
 *  Set configuration by object type, object id, and command.
 */
STATIC int
isdn_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isdn_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case ISDN_ADD:
				switch (arg->type) {
				case ISDN_OBJ_TYPE_CH:
					return isdn_add_ch(arg, ch_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_TG:
					return isdn_add_tg(arg, tg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_FG:
					return isdn_add_fg(arg, fg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_EG:
					return isdn_add_eg(arg, eg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_XG:
					return isdn_add_xg(arg, xg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_DC:
					return isdn_add_dc(arg, dc_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_DL:
					return isdn_add_dl(arg, dl_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_DF:
					return isdn_add_df(arg, &master, size, 0, 0);
				}
				break;
			case ISDN_CHA:
				switch (arg->type) {
				case ISDN_OBJ_TYPE_CH:
					return isdn_cha_ch(arg, ch_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_TG:
					return isdn_cha_tg(arg, tg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_FG:
					return isdn_cha_fg(arg, fg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_EG:
					return isdn_cha_eg(arg, eg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_XG:
					return isdn_cha_xg(arg, xg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_DC:
					return isdn_cha_dc(arg, dc_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_DL:
					return isdn_cha_dl(arg, dl_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_DF:
					return isdn_cha_df(arg, &master, size, 0, 0);
				}
				break;
			case ISDN_DEL:
				switch (arg->type) {
				case ISDN_OBJ_TYPE_CH:
					return isdn_del_ch(arg, ch_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_TG:
					return isdn_del_tg(arg, tg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_FG:
					return isdn_del_fg(arg, fg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_EG:
					return isdn_del_eg(arg, eg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_XG:
					return isdn_del_xg(arg, xg_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_DC:
					return isdn_del_dc(arg, dc_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_DL:
					return isdn_del_dl(arg, dl_lookup(arg->id), size, 0, 0);
				case ISDN_OBJ_TYPE_DF:
					return isdn_del_df(arg, &master, size, 0, 0);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCTCONFIG
 *  ------------------------------------
 *  Test configuraiton by object type and id.
 */
STATIC int
isdn_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isdn_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case ISDN_ADD:
				switch (arg->type) {
				case ISDN_OBJ_TYPE_CH:
					return isdn_add_ch(arg, ch_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_TG:
					return isdn_add_tg(arg, tg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_FG:
					return isdn_add_fg(arg, fg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_EG:
					return isdn_add_eg(arg, eg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_XG:
					return isdn_add_xg(arg, xg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_DC:
					return isdn_add_dc(arg, dc_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_DL:
					return isdn_add_dl(arg, dl_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_DF:
					return isdn_add_df(arg, &master, size, 0, 1);
				}
				break;
			case ISDN_CHA:
				switch (arg->type) {
				case ISDN_OBJ_TYPE_CH:
					return isdn_cha_ch(arg, ch_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_TG:
					return isdn_cha_tg(arg, tg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_FG:
					return isdn_cha_fg(arg, fg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_EG:
					return isdn_cha_eg(arg, eg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_XG:
					return isdn_cha_xg(arg, xg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_DC:
					return isdn_cha_dc(arg, dc_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_DL:
					return isdn_cha_dl(arg, dl_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_DF:
					return isdn_cha_df(arg, &master, size, 0, 1);
				}
				break;
			case ISDN_DEL:
				switch (arg->type) {
				case ISDN_OBJ_TYPE_CH:
					return isdn_del_ch(arg, ch_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_TG:
					return isdn_del_tg(arg, tg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_FG:
					return isdn_del_fg(arg, fg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_EG:
					return isdn_del_eg(arg, eg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_XG:
					return isdn_del_xg(arg, xg_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_DC:
					return isdn_del_dc(arg, dc_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_DL:
					return isdn_del_dl(arg, dl_lookup(arg->id), size, 0, 1);
				case ISDN_OBJ_TYPE_DF:
					return isdn_del_df(arg, &master, size, 0, 1);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCCCONFIG
 *  ------------------------------------
 *  Commit configuraiton by object type and id.
 */
STATIC int
isdn_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isdn_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case ISDN_ADD:
				switch (arg->type) {
				case ISDN_OBJ_TYPE_CH:
					return isdn_add_ch(arg, ch_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_TG:
					return isdn_add_tg(arg, tg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_FG:
					return isdn_add_fg(arg, fg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_EG:
					return isdn_add_eg(arg, eg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_XG:
					return isdn_add_xg(arg, xg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_DC:
					return isdn_add_dc(arg, dc_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_DL:
					return isdn_add_dl(arg, dl_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_DF:
					return isdn_add_df(arg, &master, size, 1, 0);
				}
				break;
			case ISDN_CHA:
				switch (arg->type) {
				case ISDN_OBJ_TYPE_CH:
					return isdn_cha_ch(arg, ch_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_TG:
					return isdn_cha_tg(arg, tg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_FG:
					return isdn_cha_fg(arg, fg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_EG:
					return isdn_cha_eg(arg, eg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_XG:
					return isdn_cha_xg(arg, xg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_DC:
					return isdn_cha_dc(arg, dc_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_DL:
					return isdn_cha_dl(arg, dl_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_DF:
					return isdn_cha_df(arg, &master, size, 1, 0);
				}
				break;
			case ISDN_DEL:
				switch (arg->type) {
				case ISDN_OBJ_TYPE_CH:
					return isdn_del_ch(arg, ch_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_TG:
					return isdn_del_tg(arg, tg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_FG:
					return isdn_del_fg(arg, fg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_EG:
					return isdn_del_eg(arg, eg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_XG:
					return isdn_del_xg(arg, xg_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_DC:
					return isdn_del_dc(arg, dc_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_DL:
					return isdn_del_dl(arg, dl_lookup(arg->id), size, 1, 0);
				case ISDN_OBJ_TYPE_DF:
					return isdn_del_df(arg, &master, size, 1, 0);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCGSTATEM
 *  ------------------------------------
 *  Get state variables by object type and id.
 */
STATIC int
isdn_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isdn_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISDN_OBJ_TYPE_CH:
			return isdn_sta_ch(arg, ch_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_TG:
			return isdn_sta_tg(arg, tg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_FG:
			return isdn_sta_fg(arg, fg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_EG:
			return isdn_sta_eg(arg, eg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_XG:
			return isdn_sta_xg(arg, xg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DC:
			return isdn_sta_dc(arg, dc_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DL:
			return isdn_sta_dl(arg, dl_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DF:
			return isdn_sta_df(arg, &master, size);
		default:
			goto einval;
		}
		goto exit;
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCCMRESET
 *  ------------------------------------
 *  Perform master reset.
 */
STATIC int
isdn_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		isdn_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		(void) arg;
		return (-EOPNOTSUPP);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCGSTATSP
 *  ------------------------------------
 *  Get statistics periods.
 */
STATIC int
isdn_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isdn_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISDN_OBJ_TYPE_CH:
			return isdn_statp_get_ch(arg, ch_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_TG:
			return isdn_statp_get_tg(arg, tg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_FG:
			return isdn_statp_get_fg(arg, fg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_EG:
			return isdn_statp_get_eg(arg, eg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_XG:
			return isdn_statp_get_xg(arg, xg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DC:
			return isdn_statp_get_dc(arg, dc_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DL:
			return isdn_statp_get_dl(arg, dl_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DF:
			return isdn_statp_get_df(arg, &master, size);
		default:
			goto einval;
		}
		goto exit;
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCSSTATSP
 *  ------------------------------------
 *  Set statistics periods.
 */
STATIC int
isdn_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isdn_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISDN_OBJ_TYPE_CH:
			return isdn_statp_set_ch(arg, ch_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_TG:
			return isdn_statp_set_tg(arg, tg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_FG:
			return isdn_statp_set_fg(arg, fg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_EG:
			return isdn_statp_set_eg(arg, eg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_XG:
			return isdn_statp_set_xg(arg, xg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DC:
			return isdn_statp_set_dc(arg, dc_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DL:
			return isdn_statp_set_dl(arg, dl_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DF:
			return isdn_statp_set_df(arg, &master, size);
		default:
			goto einval;
		}
		goto exit;
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCGSTATS
 *  ------------------------------------
 *  Get statistics.
 */
STATIC int
isdn_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isdn_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISDN_OBJ_TYPE_CH:
			return isdn_stat_get_ch(arg, ch_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_TG:
			return isdn_stat_get_tg(arg, tg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_FG:
			return isdn_stat_get_fg(arg, fg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_EG:
			return isdn_stat_get_eg(arg, eg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_XG:
			return isdn_stat_get_xg(arg, xg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DC:
			return isdn_stat_get_dc(arg, dc_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DL:
			return isdn_stat_get_dl(arg, dl_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DF:
			return isdn_stat_get_df(arg, &master, size);
		default:
			goto einval;
		}
		goto exit;
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCCSTATS
 *  ------------------------------------
 *  Clear statistics.
 */
STATIC int
isdn_ioccstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isdn_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISDN_OBJ_TYPE_CH:
			return isdn_stat_clr_ch(arg, ch_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_TG:
			return isdn_stat_clr_tg(arg, tg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_FG:
			return isdn_stat_clr_fg(arg, fg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_EG:
			return isdn_stat_clr_eg(arg, eg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_XG:
			return isdn_stat_clr_xg(arg, xg_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DC:
			return isdn_stat_clr_dc(arg, dc_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DL:
			return isdn_stat_clr_dl(arg, dl_lookup(arg->id), size);
		case ISDN_OBJ_TYPE_DF:
			return isdn_stat_clr_df(arg, &master, size);
		default:
			goto einval;
		}
		goto exit;
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCGNOTIFY
 *  ------------------------------------
 *  Get event notifications associated with an object.
 */
STATIC int
isdn_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		isdn_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISDN_OBJ_TYPE_CH:
			ret = isdn_not_get_ch(arg, ch_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_TG:
			ret = isdn_not_get_tg(arg, tg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_FG:
			ret = isdn_not_get_fg(arg, fg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_EG:
			ret = isdn_not_get_eg(arg, eg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_XG:
			ret = isdn_not_get_xg(arg, xg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_DC:
			ret = isdn_not_get_dc(arg, dc_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_DL:
			ret = isdn_not_get_dl(arg, dl_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_DF:
			ret = isdn_not_get_df(arg, &master);
			break;
		default:
			ret = -EINVAL;
			break;
		}
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCSNOTIFY
 *  ------------------------------------
 *  Set event notifications associated with an object.
 */
STATIC int
isdn_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		isdn_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISDN_OBJ_TYPE_CH:
			ret = isdn_not_set_ch(arg, ch_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_TG:
			ret = isdn_not_set_tg(arg, tg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_FG:
			ret = isdn_not_set_fg(arg, fg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_EG:
			ret = isdn_not_set_eg(arg, eg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_XG:
			ret = isdn_not_set_xg(arg, xg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_DC:
			ret = isdn_not_set_dc(arg, dc_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_DL:
			ret = isdn_not_set_dl(arg, dl_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_DF:
			ret = isdn_not_set_df(arg, &master);
			break;
		default:
			ret = -EINVAL;
			break;
		}
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCCNOTIFY
 *  ------------------------------------
 *  Clear event notifications associated with an object.
 */
STATIC int
isdn_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		isdn_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISDN_OBJ_TYPE_CH:
			ret = isdn_not_clr_ch(arg, ch_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_TG:
			ret = isdn_not_clr_tg(arg, tg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_FG:
			ret = isdn_not_clr_fg(arg, fg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_EG:
			ret = isdn_not_clr_eg(arg, eg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_XG:
			ret = isdn_not_clr_xg(arg, xg_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_DC:
			ret = isdn_not_clr_dc(arg, dc_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_DL:
			ret = isdn_not_clr_dl(arg, dl_lookup(arg->id));
			break;
		case ISDN_OBJ_TYPE_DF:
			ret = isdn_not_clr_df(arg, &master);
			break;
		default:
			ret = -EINVAL;
			break;
		}
		spin_unlock_irqrestore(&master.lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCCMGMT
 *  ------------------------------------
 *  Manage objects.
 */
STATIC int
isdn_ioccmgmt(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		isdn_mgmt_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		switch (arg->cmd) {
		case ISDN_MGMT_BLOCK:
			switch (arg->type) {
			case ISDN_OBJ_TYPE_CH:
				return isdn_mgmt_blo_ch(arg, ch_lookup(arg->id));
			case ISDN_OBJ_TYPE_TG:
				return isdn_mgmt_blo_tg(arg, tg_lookup(arg->id));
			case ISDN_OBJ_TYPE_FG:
				return isdn_mgmt_blo_fg(arg, fg_lookup(arg->id));
			case ISDN_OBJ_TYPE_EG:
				return isdn_mgmt_blo_eg(arg, eg_lookup(arg->id));
			case ISDN_OBJ_TYPE_XG:
				return isdn_mgmt_blo_xg(arg, xg_lookup(arg->id));
			case ISDN_OBJ_TYPE_DC:
				return isdn_mgmt_blo_dc(arg, dc_lookup(arg->id));
			case ISDN_OBJ_TYPE_DL:
				return isdn_mgmt_blo_dl(arg, dl_lookup(arg->id));
			case ISDN_OBJ_TYPE_DF:
				return isdn_mgmt_blo_df(arg, &master);
			}
			break;
		case ISDN_MGMT_UNBLOCK:
			switch (arg->type) {
			case ISDN_OBJ_TYPE_CH:
				return isdn_mgmt_ubl_ch(arg, ch_lookup(arg->id));
			case ISDN_OBJ_TYPE_TG:
				return isdn_mgmt_ubl_tg(arg, tg_lookup(arg->id));
			case ISDN_OBJ_TYPE_FG:
				return isdn_mgmt_ubl_fg(arg, fg_lookup(arg->id));
			case ISDN_OBJ_TYPE_EG:
				return isdn_mgmt_ubl_eg(arg, eg_lookup(arg->id));
			case ISDN_OBJ_TYPE_XG:
				return isdn_mgmt_ubl_xg(arg, xg_lookup(arg->id));
			case ISDN_OBJ_TYPE_DC:
				return isdn_mgmt_ubl_dc(arg, dc_lookup(arg->id));
			case ISDN_OBJ_TYPE_DL:
				return isdn_mgmt_ubl_dl(arg, dl_lookup(arg->id));
			case ISDN_OBJ_TYPE_DF:
				return isdn_mgmt_ubl_df(arg, &master);
			}
			break;
		case ISDN_MGMT_RESET:
			switch (arg->type) {
			case ISDN_OBJ_TYPE_CH:
				return isdn_mgmt_res_ch(arg, ch_lookup(arg->id));
			case ISDN_OBJ_TYPE_TG:
				return isdn_mgmt_res_tg(arg, tg_lookup(arg->id));
			case ISDN_OBJ_TYPE_FG:
				return isdn_mgmt_res_fg(arg, fg_lookup(arg->id));
			case ISDN_OBJ_TYPE_EG:
				return isdn_mgmt_res_eg(arg, eg_lookup(arg->id));
			case ISDN_OBJ_TYPE_XG:
				return isdn_mgmt_res_xg(arg, xg_lookup(arg->id));
			case ISDN_OBJ_TYPE_DC:
				return isdn_mgmt_res_dc(arg, dc_lookup(arg->id));
			case ISDN_OBJ_TYPE_DL:
				return isdn_mgmt_res_dl(arg, dl_lookup(arg->id));
			case ISDN_OBJ_TYPE_DF:
				return isdn_mgmt_res_df(arg, &master);
			}
			break;
		case ISDN_MGMT_QUERY:
			switch (arg->type) {
			case ISDN_OBJ_TYPE_CH:
				return isdn_mgmt_qry_ch(arg, ch_lookup(arg->id));
			case ISDN_OBJ_TYPE_TG:
				return isdn_mgmt_qry_tg(arg, tg_lookup(arg->id));
			case ISDN_OBJ_TYPE_FG:
				return isdn_mgmt_qry_fg(arg, fg_lookup(arg->id));
			case ISDN_OBJ_TYPE_EG:
				return isdn_mgmt_qry_eg(arg, eg_lookup(arg->id));
			case ISDN_OBJ_TYPE_XG:
				return isdn_mgmt_qry_xg(arg, xg_lookup(arg->id));
			case ISDN_OBJ_TYPE_DC:
				return isdn_mgmt_qry_dc(arg, dc_lookup(arg->id));
			case ISDN_OBJ_TYPE_DL:
				return isdn_mgmt_qry_dl(arg, dl_lookup(arg->id));
			case ISDN_OBJ_TYPE_DF:
				return isdn_mgmt_qry_df(arg, &master);
			}
			break;
		}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISDN_IOCCPASS
 *  ------------------------------------
 *  Pass a message to lower layer.
 */
STATIC int
isdn_ioccpass(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		isdn_pass_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		mblk_t *bp, *dp;
		struct dl *dl;
		for (dl = master.dl.list; dl && dl->u.mux.index != arg->muxid; dl = dl->next) ;
		if (!dl || !dl->oq)
			return (-EINVAL);
		if (arg->type < QPCTL && !canput(dl->oq))
			return (-EBUSY);
		if (!(bp = dupb(mp)))
			return (-ENOBUFS);
		if (!(dp = dupb(mp))) {
			freeb(bp);
			return (-ENOBUFS);
		}
		bp->b_datap->db_type = arg->type;
		bp->b_band = arg->band;
		bp->b_cont = dp;
		bp->b_rptr += sizeof(*arg);
		bp->b_wptr = bp->b_rptr + arg->ctl_length;
		dp->b_datap->db_type = M_DATA;
		dp->b_rptr += sizeof(*arg) + arg->ctl_length;
		dp->b_wptr = dp->b_rptr + arg->dat_length;
		ss7_oput(dl->oq, bp);
		return (QR_DONE);
	}
	rare();
	return (-EINVAL);
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
cc_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;
	(void) cc;
	switch (type) {
	case __SID:
	{
		struct dl *dl;
		struct linkblk *lb;
		if (!(lb = arg)) {
			swerr();
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n", DRV_NAME,
					cc));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			if ((dl =
			     isdn_alloc_dl(lb->l_qbot, &master.dl.list, lb->l_index, iocp->ioc_cr)))
				break;
			ret = -ENOMEM;
			break;
		case _IOC_NR(I_PUNLINK):
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n", DRV_NAME,
					cc));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			for (dl = master.dl.list; dl; dl = dl->next)
				if (dl->u.mux.index == lb->l_index)
					break;
			if (!dl) {
				ret = -EINVAL;
				ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n", DRV_NAME,
					cc));
				break;
			}
			isdn_free_dl(dl->iq);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", DRV_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case ISDN_IOC_MAGIC:
	{
		if (count < size) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(ISDN_IOCGOPTIONS):
			printd(("%s; %p: -> ISDN_IOCGOPTIONS\n", DRV_NAME, cc));
			ret = isdn_iocgoptions(q, mp);
			break;
		case _IOC_NR(ISDN_IOCSOPTIONS):
			printd(("%s; %p: -> ISDN_IOCSOPTIONS\n", DRV_NAME, cc));
			ret = isdn_iocsoptions(q, mp);
			break;
		case _IOC_NR(ISDN_IOCGCONFIG):
			printd(("%s; %p: -> ISDN_IOCGCONFIG\n", DRV_NAME, cc));
			ret = isdn_iocgconfig(q, mp);
			break;
		case _IOC_NR(ISDN_IOCSCONFIG):
			printd(("%s; %p: -> ISDN_IOCSCONFIG\n", DRV_NAME, cc));
			ret = isdn_iocsconfig(q, mp);
			break;
		case _IOC_NR(ISDN_IOCTCONFIG):
			printd(("%s; %p: -> ISDN_IOCTCONFIG\n", DRV_NAME, cc));
			ret = isdn_ioctconfig(q, mp);
			break;
		case _IOC_NR(ISDN_IOCCCONFIG):
			printd(("%s; %p: -> ISDN_IOCCCONFIG\n", DRV_NAME, cc));
			ret = isdn_ioccconfig(q, mp);
			break;
		case _IOC_NR(ISDN_IOCGSTATEM):
			printd(("%s; %p: -> ISDN_IOCGSTATEM\n", DRV_NAME, cc));
			ret = isdn_iocgstatem(q, mp);
			break;
		case _IOC_NR(ISDN_IOCCMRESET):
			printd(("%s; %p: -> ISDN_IOCCMRESET\n", DRV_NAME, cc));
			ret = isdn_ioccmreset(q, mp);
			break;
		case _IOC_NR(ISDN_IOCGSTATSP):
			printd(("%s; %p: -> ISDN_IOCGSTATSP\n", DRV_NAME, cc));
			ret = isdn_iocgstatsp(q, mp);
			break;
		case _IOC_NR(ISDN_IOCSSTATSP):
			printd(("%s; %p: -> ISDN_IOCSSTATSP\n", DRV_NAME, cc));
			ret = isdn_iocsstatsp(q, mp);
			break;
		case _IOC_NR(ISDN_IOCGSTATS):
			printd(("%s; %p: -> ISDN_IOCGSTATS\n", DRV_NAME, cc));
			ret = isdn_iocgstats(q, mp);
			break;
		case _IOC_NR(ISDN_IOCCSTATS):
			printd(("%s; %p: -> ISDN_IOCCSTATS\n", DRV_NAME, cc));
			ret = isdn_ioccstats(q, mp);
			break;
		case _IOC_NR(ISDN_IOCGNOTIFY):
			printd(("%s; %p: -> ISDN_IOCGNOTIFY\n", DRV_NAME, cc));
			ret = isdn_iocgnotify(q, mp);
			break;
		case _IOC_NR(ISDN_IOCSNOTIFY):
			printd(("%s; %p: -> ISDN_IOCSNOTIFY\n", DRV_NAME, cc));
			ret = isdn_iocsnotify(q, mp);
			break;
		case _IOC_NR(ISDN_IOCCNOTIFY):
			printd(("%s; %p: -> ISDN_IOCCNOTIFY\n", DRV_NAME, cc));
			ret = isdn_ioccnotify(q, mp);
			break;
		case _IOC_NR(ISDN_IOCCMGMT):
			printd(("%s; %p: -> ISDN_IOCCMGMT\n", DRV_NAME, cc));
			ret = isdn_ioccmgmt(q, mp);
			break;
		case _IOC_NR(ISDN_IOCCPASS):
			printd(("%s; %p: -> ISDN_IOCCPASS\n", DRV_NAME, cc));
			ret = isdn_ioccpass(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported ISDN ioctl %d\n", DRV_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
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
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Primitives from User to ISDN.
 *  -----------------------------------
 */
STATIC int
cc_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct cc *cc = CC_PRIV(q);
	ulong oldstate = cc_get_state(cc);
	switch (*(ulong *) mp->b_rptr) {
	case CC_INFO_REQ:
		printd(("%s: %p: -> CC_INFO_REQ\n", DRV_NAME, cc));
		rtn = cc_info_req(q, mp);
		break;
	case CC_OPTMGMT_REQ:
		printd(("%s: %p: -> CC_OPTMGMT_REQ\n", DRV_NAME, cc));
		rtn = cc_optmgmt_req(q, mp);
		break;
	case CC_BIND_REQ:
		printd(("%s: %p: -> CC_BIND_REQ\n", DRV_NAME, cc));
		rtn = cc_bind_req(q, mp);
		break;
	case CC_UNBIND_REQ:
		printd(("%s: %p: -> CC_UNBIND_REQ\n", DRV_NAME, cc));
		rtn = cc_unbind_req(q, mp);
		break;
	case CC_ADDR_REQ:
		printd(("%s: %p: -> CC_ADDR_REQ\n", DRV_NAME, cc));
		rtn = cc_addr_req(q, mp);
		break;
	case CC_SETUP_REQ:
		printd(("%s: %p: -> CC_SETUP_REQ\n", DRV_NAME, cc));
		rtn = cc_setup_req(q, mp);
		break;
	case CC_MORE_INFO_REQ:
		printd(("%s: %p: -> CC_MORE_INFO_REQ\n", DRV_NAME, cc));
		rtn = cc_more_info_req(q, mp);
		break;
	case CC_INFORMATION_REQ:
		printd(("%s: %p: -> CC_INFORMATION_REQ\n", DRV_NAME, cc));
		rtn = cc_information_req(q, mp);
		break;
	case CC_CONT_CHECK_REQ:
		printd(("%s: %p: -> CC_CONT_CHECK_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_CONT_TEST_REQ:
		printd(("%s: %p: -> CC_CONT_TEST_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_CONT_REPORT_REQ:
		printd(("%s: %p: -> CC_CONT_REPORT_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_SETUP_RES:
		printd(("%s: %p: -> CC_SETUP_RES\n", DRV_NAME, cc));
		rtn = cc_setup_res(q, mp);
		break;
	case CC_PROCEEDING_REQ:
		printd(("%s: %p: -> CC_PROCEEDING_REQ\n", DRV_NAME, cc));
		rtn = cc_proceeding_req(q, mp);
		break;
	case CC_ALERTING_REQ:
		printd(("%s: %p: -> CC_ALERTING_REQ\n", DRV_NAME, cc));
		rtn = cc_alerting_req(q, mp);
		break;
	case CC_PROGRESS_REQ:
		printd(("%s: %p: -> CC_PROGRESS_REQ\n", DRV_NAME, cc));
		rtn = cc_progress_req(q, mp);
		break;
#if 0
	case CC_IBI_REQ:
		printd(("%s: %p: -> CC_IBI_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
#endif
	case CC_DISCONNECT_REQ:
		printd(("%s: %p: -> CC_DISCONNECT_REQ\n", DRV_NAME, cc));
		rtn = cc_disconnect_req(q, mp);
		break;
	case CC_CONNECT_REQ:
		printd(("%s: %p: -> CC_CONNECT_REQ\n", DRV_NAME, cc));
		rtn = cc_connect_req(q, mp);
		break;
	case CC_SETUP_COMPLETE_REQ:
		printd(("%s: %p: -> CC_SETUP_COMPLETE_REQ\n", DRV_NAME, cc));
		rtn = cc_setup_complete_req(q, mp);
		break;
	case CC_NOTIFY_REQ:
		printd(("%s: %p: -> CC_NOTIFY_REQ\n", DRV_NAME, cc));
		rtn = cc_notify_req(q, mp);
		break;
	case CC_SUSPEND_REQ:
		printd(("%s: %p: -> CC_SUSPEND_REQ\n", DRV_NAME, cc));
		rtn = cc_suspend_req(q, mp);
		break;
	case CC_SUSPEND_RES:
		printd(("%s: %p: -> CC_SUSPEND_RES\n", DRV_NAME, cc));
		rtn = cc_suspend_res(q, mp);
		break;
	case CC_SUSPEND_REJECT_REQ:
		printd(("%s: %p: -> CC_SUSPEND_REJECT_REQ\n", DRV_NAME, cc));
		rtn = cc_suspend_reject_req(q, mp);
		break;
	case CC_RESUME_REQ:
		printd(("%s: %p: -> CC_RESUME_REQ\n", DRV_NAME, cc));
		rtn = cc_resume_req(q, mp);
		break;
	case CC_RESUME_RES:
		printd(("%s: %p: -> CC_RESUME_RES\n", DRV_NAME, cc));
		rtn = cc_resume_res(q, mp);
		break;
	case CC_RESUME_REJECT_REQ:
		printd(("%s: %p: -> CC_RESUME_REJECT_REQ\n", DRV_NAME, cc));
		rtn = cc_resume_reject_req(q, mp);
		break;
	case CC_REJECT_REQ:
		printd(("%s: %p: -> CC_REJECT_REQ\n", DRV_NAME, cc));
		rtn = cc_reject_req(q, mp);
		break;
	case CC_RELEASE_REQ:
		printd(("%s: %p: -> CC_RELEASE_REQ\n", DRV_NAME, cc));
		rtn = cc_release_req(q, mp);
		break;
	case CC_RELEASE_RES:
		printd(("%s: %p: -> CC_RELEASE_RES\n", DRV_NAME, cc));
		rtn = cc_release_res(q, mp);
		break;
	case CC_RESET_REQ:
		printd(("%s: %p: -> CC_RESET_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_RESET_RES:
		printd(("%s: %p: -> CC_RESET_RES\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_BLOCKING_REQ:
		printd(("%s: %p: -> CC_BLOCKING_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_BLOCKING_RES:
		printd(("%s: %p: -> CC_BLOCKING_RES\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_UNBLOCKING_REQ:
		printd(("%s: %p: -> CC_UNBLOCKING_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_UNBLOCKING_RES:
		printd(("%s: %p: -> CC_UNBLOCKING_RES\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_QUERY_REQ:
		printd(("%s: %p: -> CC_QUERY_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_QUERY_RES:
		printd(("%s: %p: -> CC_QUERY_RES\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_RESTART_REQ:
		printd(("%s: %p: -> CC_RESTART_REQ\n", DRV_NAME, cc));
		rtn = cc_restart_req(q, mp);
		break;
	case CC_STOP_REQ:
		printd(("%s: %p: -> CC_STOP_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	default:
		printd(("%s: %p: -> CC_????\n", DRV_NAME, cc));
		rtn = cc_unknown_prim(q, mp);
		break;
	}
	if (rtn < 0)
		cc_set_state(cc, oldstate);
	return (rtn);
}

/*
 *  Primitives from Q921 to ISDN.
 *  -----------------------------------
 */
STATIC int
dl_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct dl *dl = DL_PRIV(q);
	ulong oldstate = dl_get_state(dl);
	switch (*((ulong *) mp->b_rptr)) {
	case DL_INFO_ACK:
		printd(("%s: %p: DL_INFO_ACK <-\n", DRV_NAME, dl));
		rtn = dl_info_ack(q, mp);
		break;
	case DL_BIND_ACK:
		printd(("%s: %p: DL_BIND_ACK <-\n", DRV_NAME, dl));
		rtn = dl_bind_ack(q, mp);
		break;
	case DL_SUBS_BIND_ACK:
		printd(("%s: %p: DL_SUBS_BIND_ACK <-\n", DRV_NAME, dl));
		rtn = dl_subs_bind_ack(q, mp);
		break;
	case DL_OK_ACK:
		printd(("%s: %p: DL_OK_ACK <-\n", DRV_NAME, dl));
		rtn = dl_ok_ack(q, mp);
		break;
	case DL_ERROR_ACK:
		printd(("%s: %p: DL_ERROR_ACK <-\n", DRV_NAME, dl));
		rtn = dl_error_ack(q, mp);
		break;
	case DL_CONNECT_IND:
		printd(("%s: %p: DL_CONNECT_IND <-\n", DRV_NAME, dl));
		rtn = dl_connect_ind(q, mp);
		break;
	case DL_CONNECT_CON:
		printd(("%s: %p: DL_CONNECT_CON <-\n", DRV_NAME, dl));
		rtn = dl_connect_con(q, mp);
		break;
	case DL_TOKEN_ACK:
		printd(("%s: %p: DL_TOKEN_ACK <-\n", DRV_NAME, dl));
		rtn = dl_token_ack(q, mp);
		break;
	case DL_DISCONNECT_IND:
		printd(("%s: %p: DL_DISCONNECT_IND <-\n", DRV_NAME, dl));
		rtn = dl_disconnect_ind(q, mp);
		break;
	case DL_RESET_IND:
		printd(("%s: %p: DL_RESET_IND <-\n", DRV_NAME, dl));
		rtn = dl_reset_ind(q, mp);
		break;
	case DL_RESET_CON:
		printd(("%s: %p: DL_RESET_CON <-\n", DRV_NAME, dl));
		rtn = dl_reset_con(q, mp);
		break;
	case DL_UNITDATA_IND:
		printd(("%s: %p: DL_UNITDATA_IND [%d] <-\n", DRV_NAME, dl, msgdsize(mp->b_cont)));
		rtn = dl_unitdata_ind(q, mp);
		break;
	case DL_UDERROR_IND:
		printd(("%s: %p: DL_UDERROR_IND <-\n", DRV_NAME, dl));
		rtn = dl_uderror_ind(q, mp);
		break;
	case DL_TEST_IND:
		printd(("%s: %p: DL_TEST_IND <-\n", DRV_NAME, dl));
		rtn = dl_test_ind(q, mp);
		break;
	case DL_TEST_CON:
		printd(("%s: %p: DL_TEST_CON <-\n", DRV_NAME, dl));
		rtn = dl_test_con(q, mp);
		break;
	case DL_XID_IND:
		printd(("%s: %p: DL_XID_IND <-\n", DRV_NAME, dl));
		rtn = dl_xid_ind(q, mp);
		break;
	case DL_XID_CON:
		printd(("%s: %p: DL_XID_CON <-\n", DRV_NAME, dl));
		rtn = dl_xid_con(q, mp);
		break;
	case DL_DATA_ACK_IND:
		printd(("%s: %p: DL_DATA_ACK_IND <-\n", DRV_NAME, dl));
		rtn = dl_data_ack_ind(q, mp);
		break;
	case DL_DATA_ACK_STATUS_IND:
		printd(("%s: %p: DL_DATA_ACK_STATUS_IND <-\n", DRV_NAME, dl));
		rtn = dl_data_ack_status_ind(q, mp);
		break;
	case DL_REPLY_IND:
		printd(("%s: %p: DL_REPLY_IND <-\n", DRV_NAME, dl));
		rtn = dl_reply_ind(q, mp);
		break;
	case DL_REPLY_STATUS_IND:
		printd(("%s: %p: DL_REPLY_STATUS_IND <-\n", DRV_NAME, dl));
		rtn = dl_reply_status_ind(q, mp);
		break;
	case DL_REPLY_UPDATE_STATUS_IND:
		printd(("%s: %p: DL_REPLY_UPDATE_STATUS_IND <-\n", DRV_NAME, dl));
		rtn = dl_reply_update_status_ind(q, mp);
		break;
	case DL_PHYS_ADDR_ACK:
		printd(("%s: %p: DL_PHYS_ADDR_ACK <-\n", DRV_NAME, dl));
		rtn = dl_phys_addr_ack(q, mp);
		break;
	case DL_GET_STATISTICS_ACK:
		printd(("%s: %p: DL_GET_STATISTICS_ACK <-\n", DRV_NAME, dl));
		rtn = dl_get_statistics_ack(q, mp);
		break;
	default:
		/* 
		   reject what we don't recognize */
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		dl_set_state(dl, oldstate);
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
cc_w_data(queue_t *q, mblk_t *mp)
{
	/* 
	   data from above */
	return cc_data_req(q, mp);
}
STATIC int
dl_r_data(queue_t *q, mblk_t *mp)
{
	/* 
	   data from below */
	return dl_data_ind(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR, M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_r_error(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct cr *cr;
	int err;
	/* 
	   treat the same as a disconnect */
	for (cr = dl->dc.dc->fg.fg->cr.list; cr; cr = cr->fg.next) {
		switch (cr_get_c_state(cr)) {
		case U0_NULL:
			/* 
			   do nothing */
			break;
		case U2_OVERLAP_SENDING:
			if ((err = cc_connect_ind(q, cr, NULL)) < 0)
				goto error;
			cr_timer_stop(cr, t304);
			cr_set_c_state(cr, U0_NULL);
			isdn_free_cr(cr);
			break;
		case U25_OVERLAP_RECEIVING:
			if ((err = cc_release_ind(q, cr, NULL)) < 0)
				goto error;
			cr_timer_stop(cr, t302);
			cr_set_c_state(cr, U0_NULL);
			isdn_free_cr(cr);
			break;
		default:
			if (0) {
				/* 
				   calls in inactive state */
				cr_timer_stop(cr, tall);
				cr_set_c_state(cr, U0_NULL);
				isdn_free_cr(cr);
			} else {
				if (!(cr->timers.t309)) {
					if (dl_get_state(dl) == DL_IDLE)
						if ((err = dl_connect_req(q, dl)) < 0)
							goto error;
					cr_timer_start(cr, t309);
				}
				/* 
				   remain in current state */
			}
			break;
		}
	}
	dl_set_state(dl, DL_IDLE);
	fixme(("Notify L3\n"));
	return (QR_DONE);
      error:
	return (err);
}
STATIC int
dl_r_hangup(queue_t *q, mblk_t *mp)
{
	return dl_r_error(q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
cc_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
cc_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return cc_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return cc_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cc_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return cc_w_ioctl(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC INLINE int
dl_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return dl_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return dl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_HANGUP:
		return dl_r_hangup(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC INLINE int
dl_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
// STATIC isdn_t *isdn_list = NULL;
STATIC spinlock_t isdn_lock;
STATIC ushort isdn_majors[ISDN_CMAJORS] = { ISDN_CMAJOR_0, };

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int
isdn_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	ushort cmajor = getmajor(*devp);
	ushort cminor = getminor(*devp);
	struct cc *cc, **ip = &master.cc.list;
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (cmajor != ISDN_CMAJOR_0 || cminor >= 2) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	spin_lock_irqsave(&isdn_lock, flags);
	for (; *ip; ip = (typeof(ip)) & (*ip)->next) {
		ushort dmajor = (*ip)->u.dev.cmajor;
		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			ushort dminor = (*ip)->u.dev.cminor;
			if (cminor < dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= ISDN_CMAJORS
					    || !(cmajor = isdn_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= ISDN_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&isdn_lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(cc = isdn_alloc_cc(q, ip, devp, crp))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&isdn_lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&isdn_lock, flags);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
isdn_close(queue_t *q, int flag, cred_t *crp)
{
	struct cc *cc = CC_PRIV(q);
	head_t *h = (head_t *) cc;
	str_t *s = (str_t *) cc;
	psw_t flags;
	(void) flag;
	(void) crp;
	(void) h;
	(void) s;
	printd(("%s: %p: closing character device %d:%d\n", DRV_NAME, cc, cc->u.dev.cmajor,
		cc->u.dev.cminor));
	spin_lock_irqsave(&isdn_lock, flags);
	isdn_free_cc(q);
	spin_unlock_irqrestore(&isdn_lock, flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *isdn_cc_cachep = NULL;
STATIC kmem_cache_t *isdn_cr_cachep = NULL;
STATIC kmem_cache_t *isdn_ch_cachep = NULL;
STATIC kmem_cache_t *isdn_tg_cachep = NULL;
STATIC kmem_cache_t *isdn_fg_cachep = NULL;
STATIC kmem_cache_t *isdn_eg_cachep = NULL;
STATIC kmem_cache_t *isdn_xg_cachep = NULL;
STATIC kmem_cache_t *isdn_dc_cachep = NULL;
STATIC kmem_cache_t *isdn_dl_cachep = NULL;

STATIC int
isdn_init_caches(void)
{
	if (!isdn_cc_cachep
	    && !(isdn_cc_cachep =
		 kmem_cache_create("isdn_cc_cachep", sizeof(struct cc), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isdn_cc_cachep", DRV_NAME);
		goto no_cc;
		return (-ENOMEM);
	} else
		printd(("%s: initialized isdn cc structure cache\n", DRV_NAME));
	if (!isdn_cr_cachep
	    && !(isdn_cr_cachep =
		 kmem_cache_create("isdn_cr_cachep", sizeof(struct cr), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isdn_cr_cachep", DRV_NAME);
		goto no_cr;
	} else
		printd(("%s: initialized isdn cr structure cache\n", DRV_NAME));
	if (!isdn_ch_cachep
	    && !(isdn_ch_cachep =
		 kmem_cache_create("isdn_ch_cachep", sizeof(struct ch), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isdn_ch_cachep", DRV_NAME);
		goto no_ch;
	} else
		printd(("%s: initialized isdn ch structure cache\n", DRV_NAME));
	if (!isdn_tg_cachep
	    && !(isdn_tg_cachep =
		 kmem_cache_create("isdn_tg_cachep", sizeof(struct tg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isdn_tg_cachep", DRV_NAME);
		goto no_tg;
	} else
		printd(("%s: initialized isdn fg structure cache\n", DRV_NAME));
	if (!isdn_fg_cachep
	    && !(isdn_fg_cachep =
		 kmem_cache_create("isdn_fg_cachep", sizeof(struct fg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isdn_fg_cachep", DRV_NAME);
		goto no_fg;
	} else
		printd(("%s: initialized isdn fg structure cache\n", DRV_NAME));
	if (!isdn_eg_cachep
	    && !(isdn_eg_cachep =
		 kmem_cache_create("isdn_eg_cachep", sizeof(struct eg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isdn_eg_cachep", DRV_NAME);
		goto no_eg;
	} else
		printd(("%s: initialized isdn eg structure cache\n", DRV_NAME));
	if (!isdn_xg_cachep
	    && !(isdn_xg_cachep =
		 kmem_cache_create("isdn_xg_cachep", sizeof(struct xg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isdn_xg_cachep", DRV_NAME);
		goto no_xg;
	} else
		printd(("%s: initialized isdn xg structure cache\n", DRV_NAME));
	if (!isdn_dc_cachep
	    && !(isdn_dc_cachep =
		 kmem_cache_create("isdn_dc_cachep", sizeof(struct dc), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isdn_dc_cachep", DRV_NAME);
		goto no_dc;
	} else
		printd(("%s: initialized dc structure cache\n", DRV_NAME));
	if (!isdn_dl_cachep
	    && !(isdn_dl_cachep =
		 kmem_cache_create("isdn_dl_cachep", sizeof(struct dl), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isdn_dl_cachep", DRV_NAME);
		goto no_dl;
	} else
		printd(("%s: initialized dl structure cache\n", DRV_NAME));
	return (0);
	kmem_cache_destroy(isdn_dl_cachep);
      no_dl:
	kmem_cache_destroy(isdn_dc_cachep);
      no_dc:
	kmem_cache_destroy(isdn_xg_cachep);
      no_xg:
	kmem_cache_destroy(isdn_eg_cachep);
      no_eg:
	kmem_cache_destroy(isdn_fg_cachep);
      no_fg:
	kmem_cache_destroy(isdn_tg_cachep);
      no_tg:
	kmem_cache_destroy(isdn_ch_cachep);
      no_ch:
	kmem_cache_destroy(isdn_cr_cachep);
      no_cr:
	kmem_cache_destroy(isdn_cc_cachep);
      no_cc:
	return (-ENOMEM);
}
STATIC int
isdn_term_caches(void)
{
	int err = 0;
	if (isdn_cc_cachep) {
		if (kmem_cache_destroy(isdn_cc_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isdn_cc_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isdn_cc_cachep\n", DRV_NAME));
	}
	if (isdn_cr_cachep) {
		if (kmem_cache_destroy(isdn_cr_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isdn_cr_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isdn_cr_cachep\n", DRV_NAME));
	}
	if (isdn_ch_cachep) {
		if (kmem_cache_destroy(isdn_ch_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isdn_ch_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isdn_ch_cachep\n", DRV_NAME));
	}
	if (isdn_tg_cachep) {
		if (kmem_cache_destroy(isdn_tg_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isdn_tg_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isdn_tg_cachep\n", DRV_NAME));
	}
	if (isdn_fg_cachep) {
		if (kmem_cache_destroy(isdn_fg_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isdn_fg_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isdn_fg_cachep\n", DRV_NAME));
	}
	if (isdn_eg_cachep) {
		if (kmem_cache_destroy(isdn_eg_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isdn_eg_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isdn_eg_cachep\n", DRV_NAME));
	}
	if (isdn_xg_cachep) {
		if (kmem_cache_destroy(isdn_xg_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isdn_xg_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isdn_xg_cachep\n", DRV_NAME));
	}
	if (isdn_dc_cachep) {
		if (kmem_cache_destroy(isdn_dc_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isdn_dc_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isdn_dc_cachep\n", DRV_NAME));
	}
	if (isdn_dl_cachep) {
		if (kmem_cache_destroy(isdn_dl_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isdn_dl_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isdn_dl_cachep\n", DRV_NAME));
	}
	return (err);
}

/*
 *  CC - Call Control
 *  -----------------------------------
 *  Upper stream private structure allocation, deallocation and reference counting
 */
STATIC struct cc *
isdn_alloc_cc(queue_t *q, struct cc **ccp, dev_t *devp, cred_t *crp)
{
	struct cc *cc;
	printd(("%s: %s: create cc dev = %d:%d\n", DRV_NAME, __FUNCTION__, getmajor(*devp),
		getminor(*devp)));
	if ((cc = kmem_cache_alloc(isdn_cc_cachep, SLAB_ATOMIC))) {
		bzero(cc, sizeof(*cc));
		cc_get(cc);	/* first get */
		cc->u.dev.cmajor = getmajor(*devp);
		cc->u.dev.cminor = getminor(*devp);
		cc->cred = *crp;
		spin_lock_init(&cc->qlock);	/* "cc-queue-lock" */
		(cc->oq = RD(q))->q_ptr = cc_get(cc);
		(cc->iq = WR(q))->q_ptr = cc_get(cc);
		cc->o_prim = &cc_r_prim;
		cc->i_prim = &cc_w_prim;
		cc->o_wakeup = NULL;
		cc->i_wakeup = NULL;
		cc->i_state = LMI_UNUSABLE;
		cc->i_style = LMI_STYLE1;
		cc->i_version = 1;
		spin_lock_init(&cc->lock);	/* "cc-lock" */
		/* 
		   initialize bind list */
		cc->bind.next = NULL;
		cc->bind.prev = &cc->bind.next;
		/* 
		   initialize conn list */
		cc->conn.cpc = NULL;
		cc->conn.mgm.cr = NULL;
		/* 
		   place in master list */
		if ((cc->next = *ccp))
			cc->next->prev = &cc->next;
		cc->prev = ccp;
		*ccp = cc_get(cc);
		master.cc.numb++;
	} else
		ptrace(("%s: %s: ERROR: failed to allocate cc structure\n", DRV_NAME,
			__FUNCTION__));
	return (cc);
}
STATIC void
isdn_free_cc(queue_t *q)
{
	struct cc *cc = CC_PRIV(q);
	psw_t flags;
	ensure(cc, return);
	printd(("%s: %s: %p: free cc %d:%d\n", DRV_NAME, __FUNCTION__, cc, cc->u.dev.cmajor,
		cc->u.dev.cminor));
	spin_lock_irqsave(&cc->lock, flags);
	{
		struct cr *cr;
		struct tg *tg;
		struct fg *fg;
		struct eg *eg;
		struct xg *xg;
		/* 
		   stopping bufcalls */
		ss7_unbufcall((str_t *) cc);
		/* 
		   flushing buffser */
		flushq(cc->oq, FLUSHALL);
		flushq(cc->iq, FLUSHALL);
		/* 
		   detach from call processing */
		for (cr = cc->conn.cpc; cr; cr = cr->cpc.next)
			cr_set_i_state(cr, cc, CCS_IDLE);
		/* 
		   detach from management */
		for (cr = cc->conn.mgm.cr; cr; cr = cr->mgm.next)
			cr_set_m_state(cr, cc, CMS_IDLE);
		for (tg = cc->conn.mgm.tg; tg; tg = tg->mgm.next)
			tg_set_m_state(tg, cc, CMS_IDLE);
		for (fg = cc->conn.mgm.fg; fg; fg = fg->mgm.next)
			fg_set_m_state(fg, cc, CMS_IDLE);
		for (eg = cc->conn.mgm.eg; eg; eg = eg->mgm.next)
			eg_set_m_state(eg, cc, CMS_IDLE);
		for (xg = cc->conn.mgm.xg; xg; xg = xg->mgm.next)
			xg_set_m_state(xg, cc, CMS_IDLE);
		/* 
		   unbind */
		if (cc_get_state(cc) != CCS_UNBND) {
			switch (cc->bind.type) {
			case ISDN_BIND_CH:
				ch_put(xchg(&cc->bind.u.ch, NULL));
				break;
			case ISDN_BIND_TG:
				tg_put(xchg(&cc->bind.u.tg, NULL));
				break;
			case ISDN_BIND_FG:
				fg_put(xchg(&cc->bind.u.fg, NULL));
				break;
			case ISDN_BIND_EG:
				eg_put(xchg(&cc->bind.u.eg, NULL));
				break;
			case ISDN_BIND_XG:
				xg_put(xchg(&cc->bind.u.xg, NULL));
				break;
			case ISDN_BIND_DF:
				xchg(&cc->bind.u.df, NULL);
				break;
			default:
				swerr();
				goto not_bound;
			}
			cc->bind.type = ISDN_BIND_NONE;
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&cc->refcnt) > 1, cc_get(cc));
			cc_put(cc);
		      not_bound:
			cc_set_state(cc, CCS_UNBND);
		}
		/* 
		   remove from master list */
		if ((*cc->prev = cc->next))
			cc->next->prev = cc->prev;
		cc->next = NULL;
		cc->prev = &cc->next;
		ensure(atomic_read(&cc->refcnt) > 1, cc_get(cc));
		cc_put(cc);
		assure(master.cc.numb > 0);
		master.cc.numb--;
		/* 
		   remove from queues */
		ensure(atomic_read(&cc->refcnt) > 1, cc_get(cc));
		cc_put(xchg(&cc->oq->q_ptr, NULL));
		ensure(atomic_read(&cc->refcnt) > 1, cc_get(cc));
		cc_put(xchg(&cc->iq->q_ptr, NULL));
		/* 
		   done, check final count */
		if (atomic_read(&cc->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: cc lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, cc, atomic_read(&cc->refcnt)));
			atomic_set(&cc->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&cc->lock, flags);
	cc_put(cc);		/* final put */
	return;
}
STATIC struct cc *
cc_get(struct cc *cc)
{
	assure(cc);
	if (cc)
		atomic_inc(&cc->refcnt);
	return (cc);
}
STATIC void
cc_put(struct cc *cc)
{
	if (cc && atomic_dec_and_test(&cc->refcnt)) {
		kmem_cache_free(isdn_cc_cachep, cc);
		printd(("%s: %s: %p: deallocated cc structure", DRV_NAME, __FUNCTION__, cc));
	}
}

/*
 *  CR - Call Reference
 *  -----------------------------------
 */
STATIC struct cr *
isdn_alloc_cr(ulong id, struct cc *cc, struct fg *fg, struct ch **chp, size_t nch)
{
	/* 
	   create a call reference with a list of channels and user reference */
	struct cr *cr, **crp;
	printd(("%s: %s: create cr->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((cr = kmem_cache_alloc(isdn_cr_cachep, SLAB_ATOMIC))) {
		bzero(cr, sizeof(*cr));
		cr_get(cr);	/* first get */
		spin_lock_init(&cr->lock);	/* "cr-lock" */
		cr->id = cr_get_id(id, fg);
		cr->cref = cr->id;
		cr->uref = id;
		/* 
		   place in master list (ascending id) */
		for (crp = &master.cr.list; *crp && (*crp)->id < cr->id; crp = &(*crp)->next) ;
		if ((cr->next = *crp))
			cr->next->prev = &cr->next;
		cr->prev = crp;
		*crp = cr_get(cr);
		master.cr.numb++;
		/* 
		   place in call control list (ascending user reference) */
		if (cc) {
			for (crp = &cc->conn.cpc; *crp && (*crp)->uref < cr->uref;
			     crp = &(*crp)->cpc.next) ;
			if ((cr->cpc.next = *crp))
				cr->cpc.next->cpc.prev = &cr->cpc.next;
			cr->cpc.prev = crp;
			*crp = cr_get(cr);
			cc->conn.cpc = cr_get(cr);
		}
		/* 
		   add channels to call */
		while ((--nch) >= 0) {
			struct ch *ch;
			if ((ch = chp[nch])) {
				if (ch->idle.fg) {
					/* 
					   remove channel from facility group idle list */
					if ((*ch->idle.prev = ch->idle.next))
						ch->idle.next->idle.prev = ch->idle.prev;
					ch->idle.next = NULL;
					ch->idle.prev = &ch->idle.next;
					ch_put(ch);
					ch->idle.fg->idle.numb--;
					fg_put(xchg(&ch->idle.fg, NULL));
				} else
					swerr();
				/* 
				   add channel to call */
				if ((ch->cr.next = cr->ch.list))
					ch->cr.next->cr.prev = &ch->cr.next;
				ch->cr.prev = &cr->ch.list;
				ch->cr.cr = cr_get(cr);
				cr->ch.list = ch_get(ch);
				cr->ch.numb++;
			} else
				swerr();
		}
		/* 
		   add call reference to facility group active call reference list (descending id) */
		for (crp = &fg->cr.list; *crp && (*crp)->id < cr->id; crp = &(*crp)->next) ;
		if ((cr->fg.next = *crp))
			cr->fg.next->fg.prev = &cr->fg.next;
		cr->fg.prev = crp;
		cr->fg.fg = fg_get(fg);
		*crp = cr_get(cr);
		fg->cr.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate cr structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (cr);
}
STATIC void
isdn_free_cr(struct cr *cr)
{
	/* 
	   free a call reference, this is done when call are released as well as when channels are
	   being removed from under active calls */
	psw_t flags;
	ensure(cr, return);
	printd(("%s: %s: %p free cr->id = %ld\n", DRV_NAME, __FUNCTION__, cr, cr->id));
	spin_lock_irqsave(&cr->lock, flags);
	{
		struct ch *ch;
		struct fg *fg;
		/* 
		   stop all timers */
		__cr_timer_stop(cr, tall);
		/* 
		   free retry buffer */
		if (cr->retry)
			freemsg(xchg(&cr->retry, NULL));
		/* 
		   release call reference from call control */
		if (cr->cpc.cc) {
			switch (cr_get_i_state(cr)) {
			case U0_NULL:
			case N0_NULL:
				break;
			default:
				fixme(("Release call to call control user\n"));
				break;
			}
			cr->i_state = 0;
			if ((*cr->cpc.prev = cr->cpc.next))
				cr->cpc.next->cpc.prev = cr->cpc.prev;
			cr->cpc.next = NULL;
			cr->cpc.prev = &cr->cpc.next;
			cr_put(cr);
			cc_put(xchg(&cr->cpc.cc, NULL));
		}
		/* 
		   release call reference from management */
		if (cr->mgm.cc) {
			switch (cr_get_m_state(cr)) {
			case CMS_IDLE:
				break;
			default:
				fixme(("Release call to management user\n"));
				break;
			}
			cr->m_state = 0;
			if ((*cr->mgm.prev = cr->mgm.next))
				cr->mgm.next->mgm.prev = cr->mgm.prev;
			cr->mgm.next = NULL;
			cr->mgm.prev = &cr->mgm.next;
			cr_put(cr);
			cc_put(xchg(&cr->mgm.cc, NULL));
		}
		/* 
		   remove call reference from facility group */
		if ((fg = cr->fg.fg)) {
			if ((*cr->fg.prev = cr->fg.next))
				cr->fg.next->fg.prev = cr->fg.prev;
			cr->fg.next = NULL;
			cr->fg.prev = &cr->fg.next;
			cr_put(cr);
			fg->cr.numb--;
			fg_put(xchg(&cr->fg.fg, NULL));
			/* 
			   remove circuits from call reference and return to fg idle list */
			while ((ch = cr->ch.list)) {
				if (ch->cr.cr) {
					if ((*ch->cr.prev = ch->cr.next))
						ch->cr.next->cr.prev = ch->cr.prev;
					ch->cr.next = NULL;
					ch->cr.prev = &ch->cr.next;
					ch_put(ch);
					cr->ch.numb--;
					cr_put(xchg(&ch->cr.cr, NULL));
				} else
					swerr();
				if ((fg = ch->fg.fg)) {
					if ((ch->idle.next = fg->idle.list))
						ch->idle.next->idle.prev = &ch->idle.next;
					ch->idle.prev = &fg->idle.list;
					fg->idle.list = ch_get(ch);
					fg->idle.numb++;
					ch->idle.fg = fg_get(fg);
				} else
					swerr();
			}
		} else
			swerr();
		/* 
		   remove from master list */
		cr->id = 0;
		cr->uref = 0;
		cr->cref = 0;
		if ((*cr->prev = cr->next))
			cr->next->prev = cr->prev;
		cr->next = NULL;
		cr->prev = &cr->next;
		cr_put(cr);
		master.cr.numb--;
	}
	spin_unlock_irqrestore(&cr->lock, flags);
	cr_put(cr);		/* final put */
	return;
}
STATIC struct cr *
cr_get(struct cr *cr)
{
	assure(cr);
	if (cr)
		atomic_inc(&cr->refcnt);
	return (cr);
}
STATIC void
cr_put(struct cr *cr)
{
	assure(cr);
	if (cr) {
		assure(atomic_read(&cr->refcnt) > 1);
		if (atomic_dec_and_test(&cr->refcnt)) {
			kmem_cache_free(isdn_cr_cachep, cr);
			printd(("%s: %s: %p: deallocated cr structure\n", DRV_NAME, __FUNCTION__,
				cr));
		}
	}
}
STATIC INLINE struct cr *
cr_lookup(ulong id, struct fg *fg)
{
	struct cr *cr;
	for (cr = fg->cr.list; cr && cr->id != id; cr = cr->fg.next) ;
	return (cr);
}
STATIC ulong
cr_get_id(ulong id, struct fg *fg)
{
	static ulong sequence = 0;
	struct cr *cr;
	if (!id) {
		if (!(id = ++sequence))
			id = ++sequence;
		for (cr = fg->cr.list; cr; cr = cr->fg.next)
			if (cr->id == id) {
				id = ++sequence;
				break;
			} else if (cr->id < id)
				break;
	}
	return (id);
}

/*
 *  CH - Channel
 *  -----------------------------------
 *  Channel structure allocation, deallocation and reference counting
 */
STATIC struct ch *
isdn_alloc_ch(ulong id, struct fg *fg, struct tg *tg, ulong ts)
{
	struct ch *ch, **chp;
#ifndef _NONE
	struct eg *eg = fg->eg.eg;
	assure(eg == tg->eg.eg);
#endif
	printd(("%s: %s: create ch->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((ch = kmem_cache_alloc(isdn_ch_cachep, SLAB_ATOMIC))) {
		bzero(ch, sizeof(*ch));
		ch_get(ch);	/* first get */
		spin_lock_init(&ch->lock);	/* "ch-lock" */
		ch->id = id;
		ch->ts = ts;
		/* 
		   place in master list (ascending id) */
		for (chp = &master.ch.list; *chp && (*chp)->id < id; chp = &(*chp)->next) ;
		if ((ch->next = *chp))
			ch->next->prev = &ch->next;
		ch->prev = chp;
		*chp = ch_get(ch);
		master.ch.numb++;
		/* 
		   place in transmission group list (ascending ts) */
		for (chp = &tg->ch.list; *chp && (*chp)->ts < ts; chp = &(*chp)->tg.next) ;
		if ((ch->tg.next = *chp))
			ch->tg.next->tg.prev = &ch->tg.next;
		ch->tg.prev = chp;
		*chp = ch_get(ch);
		tg->ch.numb++;
		ch->tg.tg = tg_get(tg);
		/* 
		   place in facility group list (ascending id) */
		for (chp = &fg->ch.list; *chp && (*chp)->id < id; chp = &(*chp)->fg.next) ;
		if ((ch->fg.next = *chp))
			ch->fg.next->fg.prev = &ch->fg.next;
		ch->fg.prev = chp;
		*chp = ch_get(ch);
		fg->ch.numb++;
		ch->fg.fg = fg_get(fg);
		/* 
		   place in facility group idle list */
		if ((ch->idle.next = fg->idle.list))
			ch->idle.next->idle.prev = &ch->idle.next;
		ch->idle.prev = &fg->idle.list;
		ch->idle.fg = fg_get(fg);
		fg->idle.list = ch_get(ch);
		fg->idle.numb++;
		/* 
		   initialize call reference list */
		ch->cr.cr = NULL;
		ch->cr.next = NULL;
		ch->cr.prev = &ch->cr.next;
	} else
		printd(("%s: %s: ERROR: failed to allocate ch structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (ch);
}
STATIC void
isdn_free_ch(struct ch *ch)
{
	struct cc *cc;
	psw_t flags;
	ensure(ch, return);
	printd(("%s: %s: %p free ch->id = %ld\n", DRV_NAME, __FUNCTION__, ch, ch->id));
	spin_lock_irqsave(&ch->lock, flags);
	{
		assure(!ch->cr.cr);
		/* 
		   detach from call reference */
		if (ch->cr.cr) {
			/* 
			   we are currently engaged in a call on the circuit. This only occurs when 
			   we are removing with force.  Free the call reference. */
			isdn_free_cr(ch->cr.cr);
		}
		assure(!ch->bind.mgm);
		/* 
		   force unbind from listening management stream */
		if ((cc = xchg(&ch->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		assure(!ch->bind.mnt);
		/* 
		   force unbind from listening maintenance stream */
		if ((cc = xchg(&ch->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		assure(!ch->bind.xry);
		/* 
		   force unbind from listening xray stream */
		if ((cc = xchg(&ch->bind.xry, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		assure(!ch->bind.icc);
		/* 
		   force unbind from listening call control stream */
		if ((cc = xchg(&ch->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		assure(!ch->bind.ogc);
		/* 
		   force unbind from outgoing call control streams */
		while ((cc = ch->bind.ogc)) {
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&ch->refcnt) > 1, ch_get(ch));
			ch_put(xchg(&cc->bind.u.ch, NULL));
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   remove from facility group idle list */
		if (ch->idle.fg) {
			if ((*ch->idle.prev = ch->idle.next))
				ch->idle.next->idle.prev = ch->idle.prev;
			ch->idle.next = NULL;
			ch->idle.prev = &ch->idle.next;
			fg_put(xchg(&ch->idle.fg, NULL));
			ch_put(ch);
		}
		/* 
		   remove from facility group list */
		if (ch->fg.fg) {
			if ((*ch->fg.prev = ch->fg.next))
				ch->fg.next->fg.prev = ch->fg.prev;
			ch->fg.next = NULL;
			ch->fg.prev = &ch->fg.next;
			fg_put(xchg(&ch->fg.fg, NULL));
			ensure(atomic_read(&ch->refcnt) > 1, ch_get(ch));
			ch_put(ch);
		}
		/* 
		   remove from transmission group list */
		if (ch->tg.tg) {
			if ((*ch->tg.prev = ch->tg.next))
				ch->tg.next->tg.prev = ch->tg.prev;
			ch->tg.next = NULL;
			ch->tg.prev = &ch->tg.next;
			tg_put(xchg(&ch->tg.tg, NULL));
			ensure(atomic_read(&ch->refcnt) > 1, ch_get(ch));
			ch_put(ch);
		}
		/* 
		   remove from master list */
		if ((*ch->prev = ch->next))
			ch->next->prev = ch->prev;
		ch->next = NULL;
		ch->prev = &ch->next;
		ensure(atomic_read(&ch->refcnt) > 1, ch_get(ch));
		ch_put(ch);
		assure(master.ch.numb > 0);
		master.ch.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&ch->refcnt) != 1) {
			__printd(("%s: %p: ERROR: ch lingering reference count = %d\n", DRV_NAME,
				  ch, atomic_read(&ch->refcnt)));
			atomic_set(&ch->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&ch->lock, flags);
	ch_put(ch);		/* final put */
	return;
}
STATIC struct ch *
ch_get(struct ch *ch)
{
	assure(ch);
	if (ch)
		atomic_inc(&ch->refcnt);
	return (ch);
}
STATIC void
ch_put(struct ch *ch)
{
	assure(ch);
	if (ch) {
		assure(atomic_read(&ch->refcnt) > 1);
		if (atomic_dec_and_test(&ch->refcnt)) {
			kmem_cache_free(isdn_ch_cachep, ch);
			printd(("%s: %s: %p: deallocated ch structure\n", DRV_NAME, __FUNCTION__,
				ch));
		}
	}
}
STATIC struct ch *
ch_lookup(ulong id)
{
	struct ch *ch;
	for (ch = master.ch.list; ch && ch->id != id; ch = ch->next) ;
	return (ch);
}
STATIC ulong
ch_get_id(ulong id)
{
	struct ch *ch;
	if (!id) {
		id = 1;
		for (ch = master.ch.list; ch; ch = ch->next)
			if (ch->id == id)
				id++;
			else if (ch->id > id)
				break;
	}
	return (id);
}

/*
 *  TG - Transmission Group
 *  -----------------------------------
 *  Transmission group structure allocation, deallocation and reference counting
 */
STATIC struct tg *
isdn_alloc_tg(ulong id, struct eg *eg)
{
	struct tg *tg, **tgp;
	printd(("%s: %s: create tg->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((tg = kmem_cache_alloc(isdn_tg_cachep, SLAB_ATOMIC))) {
		bzero(tg, sizeof(*tg));
		tg_get(tg);	/* first get */
		spin_lock_init(&tg->lock);	/* "tg-lock" */
		if (tg->proto.popt & ISDN_POPT_USER) {
			switch (eg->proto.pvar & SS7_PVAR_MASK) {
			default:
			case SS7_PVAR_ITUT:
				tg->config = itu_tg_u_config_defaults;
				break;
			case SS7_PVAR_ANSI:
				tg->config = ansi_tg_u_config_defaults;
				break;
			}
		} else {
			switch (eg->proto.pvar & SS7_PVAR_MASK) {
			default:
			case SS7_PVAR_ITUT:
				tg->config = itu_tg_n_config_defaults;
				break;
			case SS7_PVAR_ANSI:
				tg->config = ansi_tg_n_config_defaults;
				break;
			}
		}
		/* 
		   place in master list (ascending id) */
		tg->id = tg_get_id(id);
		for (tgp = &master.tg.list; *tgp && (*tgp)->id < id; tgp = &(*tgp)->next) ;
		if ((tg->next = *tgp))
			tg->next->prev = &tg->next;
		tg->prev = tgp;
		*tgp = tg_get(tg);
		master.tg.numb++;
		/* 
		   place in equipment group list (any order) */
		tg->eg.eg = eg_get(eg);
		if ((tg->eg.next = eg->tg.list))
			tg->eg.next->eg.prev = &tg->eg.next;
		tg->eg.prev = &eg->tg.list;
		eg->tg.list = tg_get(tg);
		eg->tg.numb++;
		/* 
		   reset states */
		tg->g_state = CMS_IDLE;
	} else
		printd(("%s: %s: ERROR: failed to allocate tg structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (tg);
}
STATIC void
isdn_free_tg(struct tg *tg)
{
	struct cc *cc;
	psw_t flags;
	ensure(tg, return);
	printd(("%s: %s: %p free tg->id = %ld\n", DRV_NAME, __FUNCTION__, tg, tg->id));
	spin_lock_irqsave(&tg->lock, flags);
	{
		/* 
		   free all circuits */
		assure(!tg->ch.list);	/* only happens under force */
		while (tg->ch.list)
			isdn_free_ch(tg->ch.list);
		/* 
		   free all d channels */
		assure(!tg->dc.list);	/* only happens under force */
		while (tg->dc.list)
			isdn_free_dc(tg->dc.list);
		/* 
		   remove from active group management action */
		if ((cc = tg->mgm.cc)) {
			/* 
			   We are currently engaged in management on the circuit group.  This only
			   occurs when we are removing with force.  We must M_HANGUP the stream so
			   that it knows not to expect anything more from us. */
			m_error(NULL, cc, EPIPE);
			tg_set_m_state(tg, cc, CMS_IDLE);
			cc_set_state(cc, CCS_UNUSABLE);
		}
		/* 
		   force unbind listening managment streams */
		if ((cc = xchg(&tg->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening maintenance streams */
		if ((cc = xchg(&tg->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening xray streams */
		if ((cc = xchg(&tg->bind.xry, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening call control streams */
		if ((cc = xchg(&tg->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind all outgoing call control streams */
		while ((cc = tg->bind.ogc)) {
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&tg->refcnt) > 1, tg_get(tg));
			tg_put(xchg(&cc->bind.u.tg, NULL));
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		if (tg->eg.eg) {
			/* 
			   remove from equipment group list */
			if ((*tg->eg.prev = tg->eg.next))
				tg->eg.next->eg.prev = tg->eg.prev;
			tg->eg.next = NULL;
			tg->eg.prev = &tg->eg.next;
			eg_put(xchg(&tg->eg.eg, NULL));
			ensure(atomic_read(&tg->refcnt) > 1, tg_get(tg));
			tg_put(tg);
		}
		/* 
		   remove from master list */
		if ((*tg->prev = tg->next))
			tg->next->prev = tg->prev;
		tg->next = NULL;
		tg->prev = &tg->next;
		ensure(atomic_read(&tg->refcnt) > 1, tg_get(tg));
		tg_put(tg);
		assure(master.tg.numb > 0);
		master.tg.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&tg->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: tg lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, tg, atomic_read(&tg->refcnt)));
			atomic_set(&tg->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&tg->lock, flags);
	tg_put(tg);		/* final put */
	return;
}
STATIC struct tg *
tg_get(struct tg *tg)
{
	assure(tg);
	if (tg)
		atomic_inc(&tg->refcnt);
	return (tg);
}
STATIC void
tg_put(struct tg *tg)
{
	assure(tg);
	if (tg) {
		assure(atomic_read(&tg->refcnt) > 1);
		if (atomic_dec_and_test(&tg->refcnt)) {
			kmem_cache_free(isdn_tg_cachep, tg);
			printd(("%s: %s: %p: deallocated tg structure\n", DRV_NAME, __FUNCTION__,
				tg));
		}
	}
}
STATIC struct tg *
tg_lookup(ulong id)
{
	struct tg *tg;
	for (tg = master.tg.list; tg && tg->id != id; tg = tg->next) ;
	return (tg);
}
STATIC ulong
tg_get_id(ulong id)
{
	struct tg *tg;
	if (!id) {
		id = 1;
		for (tg = master.tg.list; tg; tg = tg->next)
			if (tg->id == id)
				id++;
			else if (tg->id > id)
				break;
	}
	return (id);
}

/*
 *  FG - Facility Group
 *  -----------------------------------
 *  Facility group structure allocation, deallocation and reference counting
 */
STATIC struct fg *
isdn_alloc_fg(ulong id, struct eg *eg)
{
	struct fg *fg, **fgp;
	printd(("%s: %s: create fg->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((fg = kmem_cache_alloc(isdn_fg_cachep, SLAB_ATOMIC))) {
		bzero(fg, sizeof(*fg));
		fg_get(fg);	/* first get */
		spin_lock_init(&fg->lock);	/* "fg-lock" */
		fg->proto = eg->proto;	/* as a default */
		if (fg->proto.popt & ISDN_POPT_USER) {
			switch (fg->proto.pvar & SS7_PVAR_MASK) {
			default:
			case SS7_PVAR_ITUT:
				fg->config = itu_fg_u_config_defaults;
				break;
			case SS7_PVAR_ANSI:
				fg->config = ansi_fg_u_config_defaults;
				break;
			}
		} else {
			switch (fg->proto.pvar & SS7_PVAR_MASK) {
			default:
			case SS7_PVAR_ITUT:
				fg->config = itu_fg_n_config_defaults;
				break;
			case SS7_PVAR_ANSI:
				fg->config = ansi_fg_n_config_defaults;
				break;
			}
		}
		/* 
		   add to master list (ascending id order) */
		fg->id = fg_get_id(id);
		for (fgp = &master.fg.list; *fgp && (*fgp)->id < id; fgp = &(*fgp)->next) ;
		if ((fg->next = *fgp))
			fg->next->prev = &fg->next;
		fg->prev = fgp;
		*fgp = fg_get(fg);
		master.fg.numb++;
		/* 
		   add to equipment group list (any order) */
		fg->eg.eg = eg_get(eg);
		if ((fg->eg.next = eg->fg.list))
			fg->eg.next->eg.prev = &fg->eg.next;
		fg->eg.prev = &eg->fg.list;
		eg->fg.list = fg_get(fg);
		eg->fg.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate fg structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (fg);
}
STATIC void
isdn_free_fg(struct fg *fg)
{
	struct cc *cc;
	psw_t flags;
	ensure(fg, return);
	printd(("%s: %s: %p free fg->id = %ld\n", DRV_NAME, __FUNCTION__, fg, fg->id));
	spin_lock_irqsave(&fg->lock, flags);
	{
		/* 
		   free all call references */
		assure(!fg->cr.list);	/* only happens under force */
		while (fg->cr.list)
			isdn_free_cr(fg->cr.list);
		/* 
		   free all circuits */
		assure(!fg->ch.list);	/* only happens under force */
		while (fg->ch.list)
			isdn_free_ch(fg->ch.list);
		assure(fg->idle.list == NULL);
		/* 
		   free all d channels */
		assure(!fg->dc.list);	/* only happens under force */
		while (fg->dc.list)
			isdn_free_dc(fg->dc.list);
		/* 
		   force unbind listening managment streams */
		if ((cc = xchg(&fg->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening maintenance streams */
		if ((cc = xchg(&fg->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening xray streams */
		if ((cc = xchg(&fg->bind.xry, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening call control streams */
		if ((cc = xchg(&fg->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind all outgoing call control streams */
		while ((cc = fg->bind.ogc)) {
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&fg->refcnt) > 1, fg_get(fg));
			fg_put(xchg(&cc->bind.u.fg, NULL));
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		if (fg->eg.eg) {
			/* 
			   remove from equipment group list */
			if ((*fg->eg.prev = fg->eg.next))
				fg->eg.next->eg.prev = fg->eg.prev;
			fg->eg.next = NULL;
			fg->eg.prev = &fg->eg.next;
			eg_put(xchg(&fg->eg.eg, NULL));
			ensure(atomic_read(&fg->refcnt) > 1, fg_get(fg));
			fg_put(fg);
		}
		/* 
		   remove from master list */
		if ((*fg->prev = fg->next))
			fg->next->prev = fg->prev;
		fg->next = NULL;
		fg->prev = &fg->next;
		ensure(atomic_read(&fg->refcnt) > 1, fg_get(fg));
		fg_put(fg);
		assure(master.fg.numb > 0);
		master.fg.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&fg->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: fg lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, fg, atomic_read(&fg->refcnt)));
			atomic_set(&fg->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&fg->lock, flags);
	fg_put(fg);		/* final put */
	return;
}
STATIC struct fg *
fg_get(struct fg *fg)
{
	assure(fg);
	if (fg)
		atomic_inc(&fg->refcnt);
	return (fg);
}
STATIC void
fg_put(struct fg *fg)
{
	assure(fg);
	if (fg) {
		assure(atomic_read(&fg->refcnt) > 1);
		if (atomic_dec_and_test(&fg->refcnt)) {
			kmem_cache_free(isdn_fg_cachep, fg);
			printd(("%s: %s: %p: deallocated fg structure\n", DRV_NAME, __FUNCTION__,
				fg));
		}
	}
}
STATIC struct fg *
fg_lookup(ulong id)
{
	struct fg *fg;
	for (fg = master.fg.list; fg && fg->id != id; fg = fg->next) ;
	return (fg);
}
STATIC ulong
fg_get_id(ulong id)
{
	struct fg *fg;
	if (!id) {
		id = 1;
		for (fg = master.fg.list; fg; fg = fg->next)
			if (fg->id == id)
				id++;
			else if (fg->id > id)
				break;
	}
	return (id);
}

/*
 *  EG - Equipment Group
 *  -----------------------------------
 *  Equipment group structure allocation, deallocation and reference counting
 */
STATIC struct eg *
isdn_alloc_eg(ulong id, struct xg *xg)
{
	struct eg *eg, **egp;
	printd(("%s: %s: create eg->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((eg = kmem_cache_alloc(isdn_eg_cachep, SLAB_ATOMIC))) {
		bzero(eg, sizeof(*eg));
		eg_get(eg);	/* first get */
		spin_lock_init(&eg->lock);	/* "eg-lock" */
		eg->proto = xg->proto;	/* as a default */
		if (eg->proto.popt & ISDN_POPT_USER) {
			switch (eg->proto.pvar & SS7_PVAR_MASK) {
			default:
			case SS7_PVAR_ITUT:
				eg->config = itu_eg_u_config_defaults;
				break;
			case SS7_PVAR_ANSI:
				eg->config = ansi_eg_u_config_defaults;
				break;
			}
		} else {
			switch (eg->proto.pvar & SS7_PVAR_MASK) {
			default:
			case SS7_PVAR_ITUT:
				eg->config = itu_eg_n_config_defaults;
				break;
			case SS7_PVAR_ANSI:
				eg->config = ansi_eg_n_config_defaults;
				break;
			}
		}
		/* 
		   add to master list (ascending id order) */
		eg->id = eg_get_id(id);
		for (egp = &master.eg.list; *egp && (*egp)->id < id; egp = &(*egp)->next) ;
		if ((eg->next = *egp))
			eg->next->prev = &eg->next;
		eg->prev = egp;
		*egp = eg_get(eg);
		master.eg.numb++;
		/* 
		   add to exchange group list */
		eg->xg.xg = xg_get(xg);
		if ((eg->xg.next = xg->eg.list))
			eg->xg.next->xg.prev = &eg->xg.next;
		eg->xg.prev = &xg->eg.list;
		xg->eg.list = eg_get(eg);
		xg->eg.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate eg structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (eg);
}
STATIC void
isdn_free_eg(struct eg *eg)
{
	struct cc *cc;
	psw_t flags;
	ensure(eg, return);
	printd(("%s: %s: %p free eg->id = %ld\n", DRV_NAME, __FUNCTION__, eg, eg->id));
	spin_lock_irqsave(&eg->lock, flags);
	{
		/* 
		   free all transmission groups */
		assure(!eg->tg.list);	/* only under force */
		while (eg->tg.list)
			isdn_free_tg(eg->tg.list);
		/* 
		   free all facility groups */
		assure(!eg->fg.list);	/* only under force */
		while (eg->fg.list)
			isdn_free_fg(eg->fg.list);
		/* 
		   force unbind from listening management stream */
		if ((cc = xchg(&eg->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening maintennce stream */
		if ((cc = xchg(&eg->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening xray stream */
		if ((cc = xchg(&eg->bind.xry, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening call control stream */
		if ((cc = xchg(&eg->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from outgoing call control streams */
		while ((cc = eg->bind.ogc)) {
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&eg->refcnt) > 1, eg_get(eg));
			eg_put(xchg(&cc->bind.u.eg, NULL));
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   remove from exchange group list */
		if (eg->xg.xg) {
			if ((*eg->xg.prev = eg->xg.next))
				eg->xg.next->xg.prev = eg->xg.prev;
			eg->xg.next = NULL;
			eg->xg.prev = &eg->xg.next;
			xg_put(xchg(&eg->xg.xg, NULL));
			ensure(atomic_read(&eg->refcnt) > 1, eg_get(eg));
			eg_put(eg);
		}
		/* 
		   remove from master list */
		if ((*eg->prev = eg->next))
			eg->next->prev = eg->prev;
		eg->next = NULL;
		eg->prev = &eg->next;
		ensure(atomic_read(&eg->refcnt) > 1, eg_get(eg));
		eg_put(eg);
		assure(master.eg.numb > 0);
		master.eg.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&eg->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: eg lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, eg, atomic_read(&eg->refcnt)));
			atomic_set(&eg->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&eg->lock, flags);
	eg_put(eg);		/* final put */
}
STATIC struct eg *
eg_get(struct eg *eg)
{
	assure(eg);
	if (eg)
		atomic_inc(&eg->refcnt);
	return (eg);
}
STATIC void
eg_put(struct eg *eg)
{
	assure(eg);
	if (eg) {
		assure(atomic_read(&eg->refcnt) > 1);
		if (atomic_dec_and_test(&eg->refcnt)) {
			kmem_cache_free(isdn_eg_cachep, eg);
			printd(("%s: %s: %p: deallocated eg structure\n", DRV_NAME, __FUNCTION__,
				eg));
		}
	}
}
STATIC struct eg *
eg_lookup(ulong id)
{
	struct eg *eg;
	for (eg = master.eg.list; eg && eg->id != id; eg = eg->next) ;
	return (eg);
}
STATIC ulong
eg_get_id(ulong id)
{
	struct eg *eg;
	if (!id) {
		id = 1;
		for (eg = master.eg.list; eg; eg = eg->next)
			if (eg->id == id)
				id++;
			else if (eg->id > id)
				break;
	}
	return (id);
}

/*
 *  XG - Exchange group
 *  -----------------------------------
 *  Exchange group structure allocation, deallocation and reference counting
 */
STATIC struct xg *
isdn_alloc_xg(ulong id)
{
	struct xg *xg, **xgp;
	struct df *df = &master;
	printd(("%s: %s: create xg->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((xg = kmem_cache_alloc(isdn_xg_cachep, SLAB_ATOMIC))) {
		bzero(xg, sizeof(*xg));
		xg_get(xg);	/* first get */
		spin_lock_init(&xg->lock);	/* "xg-lock" */
		xg->proto = df->proto;	/* as a default */
		if (xg->proto.popt & ISDN_POPT_USER) {
			switch (xg->proto.pvar & SS7_PVAR_MASK) {
			default:
			case SS7_PVAR_ITUT:
				xg->config = itu_xg_u_config_defaults;
				break;
			case SS7_PVAR_ANSI:
				xg->config = ansi_xg_u_config_defaults;
				break;
			}
		} else {
			switch (xg->proto.pvar & SS7_PVAR_MASK) {
			default:
			case SS7_PVAR_ITUT:
				xg->config = itu_xg_n_config_defaults;
				break;
			case SS7_PVAR_ANSI:
				xg->config = ansi_xg_n_config_defaults;
				break;
			}
		}
		/* 
		   add to master list */
		xg->id = xg_get_id(id);
		for (xgp = &master.xg.list; *xgp && (*xgp)->id < id; xgp = &(*xgp)->next) ;
		if ((xg->next = *xgp))
			xg->next->prev = &xg->next;
		xg->prev = xgp;
		*xgp = xg_get(xg);
		master.xg.numb++;
		todo(("Search I_LINKed DL for match\n"));
		/* 
		   for now, make management establish XG before I_LINKing DL streams */
	} else
		printd(("%s: %s: ERROR: failed to allocate xg structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (xg);
}
STATIC void
isdn_free_xg(struct xg *xg)
{
	struct cc *cc;
	psw_t flags;
	ensure(xg, return);
	printd(("%s: %s: %p free xg->id = %ld\n", DRV_NAME, __FUNCTION__, xg, xg->id));
	spin_lock_irqsave(&xg->lock, flags);
	{
		/* 
		   freeing all equipment groups */
		assure(!xg->eg.list);	/* only under force */
		while (xg->eg.list)
			isdn_free_eg(xg->eg.list);
		/* 
		   force unbind from listening management stream */
		if ((cc = xchg(&xg->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening maintenance stream */
		if ((cc = xchg(&xg->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening xray stream */
		if ((cc = xchg(&xg->bind.xry, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening call control stream */
		if ((cc = xchg(&xg->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from outgoing call control streams */
		while ((cc = xg->bind.ogc)) {
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&xg->refcnt) > 1, xg_get(xg));
			xg_put(xchg(&cc->bind.u.xg, NULL));
			m_error(NULL, cc, EPIPE);
			cc_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   remove from master list */
		if ((*xg->prev = xg->next))
			xg->next->prev = xg->prev;
		xg->next = NULL;
		xg->prev = &xg->next;
		ensure(atomic_read(&xg->refcnt) > 1, xg_get(xg));
		xg_put(xg);
		assure(master.xg.numb > 0);
		master.xg.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&xg->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: xg lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, xg, atomic_read(&xg->refcnt)));
			atomic_set(&xg->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&xg->lock, flags);
	xg_put(xg);		/* final put */
}
STATIC struct xg *
xg_get(struct xg *xg)
{
	assure(xg);
	if (xg)
		atomic_inc(&xg->refcnt);
	return (xg);
}
STATIC void
xg_put(struct xg *xg)
{
	assure(xg);
	if (xg) {
		assure(atomic_read(&xg->refcnt) > 1);
		if (atomic_dec_and_test(&xg->refcnt)) {
			kmem_cache_free(isdn_xg_cachep, xg);
			printd(("%s: %s: %p: deallocated xg structure\n", DRV_NAME, __FUNCTION__,
				xg));
		}
	}
}
STATIC struct xg *
xg_lookup(ulong id)
{
	struct xg *xg;
	for (xg = master.xg.list; xg && xg->id != id; xg = xg->next) ;
	return (xg);
}
STATIC ulong
xg_get_id(ulong id)
{
	struct xg *xg;
	if (!id) {
		id = 1;
		for (xg = master.xg.list; xg; xg = xg->next)
			if (xg->id == id)
				id++;
			else if (xg->id > id)
				break;
	}
	return (id);
}

/*
 *  DC - D-Channel
 *  -----------------------------------
 *  D-channel structure allocation, deallocation and reference counting.
 */
STATIC struct dc *
isdn_alloc_dc(ulong id, struct fg *fg, struct tg *tg)
{
	struct dc *dc;
	printd(("%s: %s: create dc->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((dc = kmem_cache_alloc(isdn_dc_cachep, SLAB_ATOMIC))) {
		struct dc **dcp;
		bzero(dc, sizeof(*dc));
		dc_get(dc);	/* first get */
		spin_lock_init(&dc->lock);	/* "dc-lock" */
		/* 
		   add to master list (ascending id order) */
		dc->id = dc_get_id(id);
		for (dcp = &master.dc.list; *dcp && (*dcp)->id < id; dcp = &(*dcp)->next) ;
		if ((dc->next = *dcp))
			dc->next->prev = &dc->next;
		dc->prev = dcp;
		*dcp = dc_get(dc);
		master.dc.numb++;
		/* 
		   add to facility group list */
		dc->fg.fg = fg_get(fg);
		if ((dc->fg.next = fg->dc.list))
			dc->fg.next->fg.prev = &dc->fg.next;
		dc->fg.prev = &fg->dc.list;
		fg->dc.list = dc_get(dc);
		fg->dc.numb++;
		/* 
		   add to transmission group list */
		dc->tg.tg = tg_get(tg);
		if ((dc->tg.next = tg->dc.list))
			dc->tg.next->tg.prev = &dc->tg.next;
		dc->tg.prev = &tg->dc.list;
		tg->dc.list = dc_get(dc);
		tg->dc.numb++;
	} else
		printd(("%s: %s: ERROR: failed to alllocated dc structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (dc);
}
STATIC void
isdn_free_dc(struct dc *dc)
{
	psw_t flags;
	ensure(dc, return);
	printd(("%s: %s: %p free dc->id = %ld\n", DRV_NAME, __FUNCTION__, dc, dc->id));
	spin_lock_irqsave(&dc->lock, flags);
	{
#if 0
		/* 
		   free all data links */
		assure(!dc->dl.list);	/* only happens under force */
		while (dc->dl.list)
			isdn_free_dl(dc->dl.list);
#endif
		if (dc->tg.tg) {
			/* 
			   remove from transmission group list */
			if ((*dc->tg.prev = dc->tg.next))
				dc->tg.next->tg.prev = dc->tg.prev;
			dc->tg.next = NULL;
			dc->tg.prev = &dc->tg.next;
			tg_put(xchg(&dc->tg.tg, NULL));
			ensure(atomic_read(&dc->refcnt) > 1, dc_get(dc));
			dc_put(dc);
		}
		if (dc->fg.fg) {
			/* 
			   remove from facility group list */
			if ((*dc->fg.prev = dc->fg.next))
				dc->fg.next->fg.prev = dc->fg.prev;
			dc->fg.next = NULL;
			dc->fg.prev = &dc->fg.next;
			fg_put(xchg(&dc->fg.fg, NULL));
			ensure(atomic_read(&dc->refcnt) > 1, dc_get(dc));
			dc_put(dc);
		}
		/* 
		   remove from master list */
		if ((*dc->prev = dc->next))
			dc->next->prev = dc->prev;
		dc->next = NULL;
		dc->prev = &dc->next;
		ensure(atomic_read(&dc->refcnt) > 1, dc_get(dc));
		dc_put(dc);
		assure(master.dc.numb > 0);
		master.dc.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&dc->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: dc lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, dc, atomic_read(&dc->refcnt)));
			atomic_set(&dc->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&dc->lock, flags);
	dc_put(dc);		/* final put */
	return;
}
STATIC struct dc *
dc_get(struct dc *dc)
{
	assure(dc);
	if (dc)
		atomic_inc(&dc->refcnt);
	return (dc);
}
STATIC void
dc_put(struct dc *dc)
{
	assure(dc);
	if (dc) {
		assure(atomic_read(&dc->refcnt) > 1);
		if (atomic_dec_and_test(&dc->refcnt)) {
			kmem_cache_free(isdn_dc_cachep, dc);
			printd(("%s: %s: %p: deallocated dc structure\n", DRV_NAME, __FUNCTION__,
				dc));
		}
	}
}
STATIC struct dc *
dc_lookup(ulong id)
{
	struct dc *dc;
	for (dc = master.dc.list; dc && dc->id != id; dc = dc->next) ;
	return (dc);
}
STATIC ulong
dc_get_id(ulong id)
{
	struct dc *dc;
	if (!id) {
		id = 1;
		for (dc = master.dc.list; dc; dc = dc->next)
			if (dc->id == id)
				id++;
			else if (dc->id > id)
				break;
	}
	return (id);
}

/*
 *  DL - Data Link
 *  -----------------------------------
 *  Lower stream private structure allocation, deallocation and reference counting
 */
STATIC struct dl *
isdn_alloc_dl(queue_t *q, struct dl **mpp, ulong index, cred_t *crp)
{
	struct dl *dl;
	printd(("%s: %s: create dl index = %lu\n", DRV_NAME, __FUNCTION__, index));
	if ((dl = kmem_cache_alloc(isdn_dl_cachep, SLAB_ATOMIC))) {
		bzero(dl, sizeof(*dl));
		dl_get(dl);	/* first get */
		dl->u.mux.index = index;
		dl->cred = *crp;
		spin_lock_init(&dl->qlock);	/* "dl-queue-lock" */
		(dl->iq = RD(q))->q_ptr = dl_get(dl);
		(dl->oq = WR(q))->q_ptr = dl_get(dl);
		dl->o_prim = dl_w_prim;
		dl->i_prim = dl_r_prim;
		dl->o_wakeup = NULL;
		dl->i_wakeup = NULL;
		dl->i_state = LMI_UNUSABLE;
		dl->i_style = LMI_STYLE1;
		dl->i_version = 1;
		spin_lock_init(&dl->lock);	/* "dl-lock" */
		/* 
		   place in master list */
		if ((dl->next = *mpp))
			dl->next->prev = &dl->next;
		dl->prev = mpp;
		*mpp = dl_get(dl);
		master.dl.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate dl structure %lu\n", DRV_NAME,
			__FUNCTION__, index));
	return (dl);
}
STATIC void
isdn_link_dl(ulong id, struct dl *dl, struct dc *dc)
{
	dl->id = dl_get_id(id);
	/* 
	   link to facility group */
	if ((dl->dc.next = dc->dl.list))
		dl->dc.next->dc.prev = &dl->dc.next;
	dl->dc.prev = &dc->dl.list;
	dl->dc.dc = dc_get(dc);
	dc->dl.list = dl_get(dl);
	dc->dl.numb++;
}
STATIC void
isdn_unlink_dl(struct dl *dl)
{
	struct df *df = &master;
	dl->id = 0;
	/* 
	   unlink from d channel */
	if (dl->dc.dc) {
		if ((*dl->dc.prev = dl->dc.next))
			dl->dc.next->dc.prev = dl->dc.prev;
		dl->dc.next = NULL;
		dl->dc.prev = &dl->dc.next;
		dl_put(dl);
		df->dc.numb--;
		dc_put(xchg(&dl->dc.dc, NULL));
	}
}
STATIC void
isdn_free_dl(queue_t *q)
{
	struct dl *dl = DL_PRIV(q);
	psw_t flags;
	ensure(dl, return);
	printd(("%s: %s: %p free dl index = %lu\n", DRV_NAME, __FUNCTION__, dl, dl->u.mux.index));
	spin_lock_irqsave(&dl->lock, flags);
	{
		/* 
		   flushing buffers */
		ss7_unbufcall((str_t *) dl);
		flushq(dl->iq, FLUSHALL);
		flushq(dl->oq, FLUSHALL);
		/* 
		   unlink */
		isdn_unlink_dl(dl);
		/* 
		   remote from master list */
		if ((*dl->prev = dl->next))
			dl->next->prev = dl->prev;
		dl->next = NULL;
		dl->prev = &dl->next;
		ensure(atomic_read(&dl->refcnt) > 1, dl_get(dl));
		dl_put(dl);
		assure(master.dl.numb > 0);
		master.dl.numb--;
		/* 
		   remove from queues */
		ensure(atomic_read(&dl->refcnt) > 1, dl_get(dl));
		dl_put(xchg(&dl->iq->q_ptr, NULL));
		ensure(atomic_read(&dl->refcnt) > 1, dl_get(dl));
		dl_put(xchg(&dl->oq->q_ptr, NULL));
		/* 
		   done, check final count */
		if (atomic_read(&dl->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: dl lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, dl, atomic_read(&dl->refcnt)));
			atomic_set(&dl->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&dl->lock, flags);
	dl_put(dl);		/* final put */
}
STATIC struct dl *
dl_get(struct dl *dl)
{
	assure(dl);
	if (dl)
		atomic_inc(&dl->refcnt);
	return (dl);
}
STATIC void
dl_put(struct dl *dl)
{
	assure(dl);
	if (dl) {
		assure(atomic_read(&dl->refcnt) > 1);
		if (atomic_dec_and_test(&dl->refcnt)) {
			kmem_cache_free(isdn_dl_cachep, dl);
			printd(("%s: %s: %p: deallocated dl structure\n", DRV_NAME, __FUNCTION__,
				dl));
		}
	}
}
STATIC struct dl *
dl_lookup(ulong id)
{
	struct dl *dl;
	for (dl = master.dl.list; dl && dl->id != id; dl = dl->next) ;
	return (dl);
}
STATIC ulong
dl_get_id(ulong id)
{
	static ulong identifier = 0;
	if (!id) {
		id = ++identifier;
	}
	return (id);
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
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module ID for the ISDN driver. (0 for allocation.)");

unsigned short major = CMAJOR_0;
MODULE_PARM(major, "h");
MODULE_PARM_DESC(major, "Device number for the INET driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw isdn_cdev = {
	.d_name = DRV_NAME,
	.d_str = &isdn_info,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
isdn_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&isdn_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
isdn_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&isdn_cdev, major)) < 0)
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
isdn_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &isdn_info, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
isdn_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
isdnterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (isdn_majors[mindex]) {
			if ((err = isdn_unregister_strdev(isdn_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					isdn_majors[mindex]);
			if (mindex)
				isdn_majors[mindex] = 0;
		}
	}
	if ((err = isdn_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
isdninit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = isdn_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		isdnterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = isdn_register_strdev(isdn_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					isdn_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				isdnterminate();
				return (err);
			}
		}
		if (isdn_majors[mindex] == 0)
			isdn_majors[mindex] = err;
#ifdef LIS
		LIS_DEVFLAGS(isdn_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = isdn_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(isdninit);
module_exit(isdnterminate);

#endif				/* LINUX */
