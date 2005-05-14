/*****************************************************************************

 @(#) $RCSfile: isup.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/05/14 08:30:56 $

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/05/14 08:30:56 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: isup.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/05/14 08:30:56 $"

static char const ident[] =
    "$RCSfile: isup.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/05/14 08:30:56 $";

/*
 *  ISUP STUB MULTIPLEXOR
 *  -------------------------------------------------------------------------
 *  This is a multiplexing driver which acts as an ISUP stub over MTP.  When ISUP is opened by its control stream
 *  (typically by the ss7 configuration daemon), it provides a control channel for configuration and routing.
 *  When opened normally, by a user process, it provides an OpenSS7 ISUP Provider.  OpenSS7 MTP Provider streams
 *  are I_LINKed under the mulitplexor by the ISUP-User and I_PLINKed by the SS7 Configuration Daemon.  ISUP
 *  streams differ from MTP streams in that they can be bound to a point-code pair and a range of CIC codes.
 *  Subsequent binds are permitted to bind additional CIC ranges to the stream.  After binding, the connectionless
 *  data transfer mechanisms of the MTP are used.
 */
#include "os7/compat.h"

// #undef INLINE
// #define INLINE

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <ss7/cci.h>
#include <ss7/isupi.h>
#include <ss7/isupi_ioctl.h>

#define ISUP_DESCRIP	"ISUP STREAMS MULTIPLEXING DRIVER."
#define ISUP_REVISION	"LfS $RCSfile: isup.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/05/14 08:30:56 $"
#define ISUP_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define ISUP_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define ISUP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ISUP_LICENSE	"GPL"
#define ISUP_BANNER	ISUP_DESCRIP	"\n" \
			ISUP_REVISION	"\n" \
			ISUP_COPYRIGHT	"\n" \
			ISUP_DEVICE	"\n" \
			ISUP_CONTACT	"\n"

#ifdef LINUX
MODULE_AUTHOR(ISUP_CONTACT);
MODULE_DESCRIPTION(ISUP_DESCRIP);
MODULE_SUPPORTED_DEVICE(ISUP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ISUP_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-isup");
#endif
#endif				/* LINUX */

#ifdef LFS
#define ISUP_DRV_ID		CONFIG_STREAMS_ISUP_MODID
#define ISUP_DRV_NAME		CONFIG_STREAMS_ISUP_NAME
#define ISUP_CMAJORS		CONFIG_STREAMS_ISUP_NMAJORS
#define ISUP_CMAJOR_0		CONFIG_STREAMS_ISUP_MAJOR
#define ISUP_UNITS		CONFIG_STREAMS_ISUP_NMINORS
#endif				/* LFS */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		ISUP_DRV_ID
#define DRV_NAME	ISUP_DRV_NAME
#define CMAJORS		ISUP_CMAJORS
#define CMAJOR_0	ISUP_CMAJOR_0
#define UNITS		ISUP_UNITS
#ifdef MODULE
#define DRV_BANNER	ISUP_BANNER
#else				/* MODULE */
#define DRV_BANNER	ISUP_SPLASH
#endif				/* MODULE */

STATIC struct module_info isup_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-wr",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 12,		/* Lo water mark */
};
STATIC struct module_info isup_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-rd",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 12,		/* Lo water mark */
};
STATIC struct module_info mtp_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-mxw",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 12,		/* Lo water mark */
};
STATIC struct module_info mtp_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-mxr",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 12,		/* Lo water mark */
};

STATIC int isup_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int isup_close(queue_t *, int, cred_t *);

STATIC struct qinit isup_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from above) */
	qi_srvp:ss7_osrv,		/* Read queue service */
	qi_qopen:isup_open,		/* Each open */
	qi_qclose:isup_close,		/* Last close */
	qi_minfo:&isup_rinfo,		/* Information */
};
STATIC struct qinit isup_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&isup_winfo,		/* Information */
};
STATIC struct qinit mtp_rinit = {
	qi_putp:ss7_iput,		/* Read put (message from below) */
	qi_srvp:ss7_isrv,		/* Read queue service */
	qi_minfo:&mtp_rinfo,		/* Information */
};
STATIC struct qinit mtp_winit = {
	qi_putp:ss7_oput,		/* Write put (message from above) */
	qi_srvp:ss7_osrv,		/* Write queue service */
	qi_minfo:&mtp_winfo,		/* Information */
};

STATIC struct streamtab isupinfo = {
	st_rdinit:&isup_rinit,		/* Upper read queue */
	st_wrinit:&isup_winit,		/* Upper write queue */
	st_muxrinit:&mtp_rinit,		/* Lower read queue */
	st_muxwinit:&mtp_winit,		/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private Data Structures
 *
 *  =========================================================================
 */

struct cc;				/* isup user structure */
struct tg;				/* isup trunk group structure */
struct cg;				/* isup circuit group structure */
struct ct;				/* isup circuit structure */
struct sr;				/* isup signalling relation structure */
struct sp;				/* isup signalling point structure */
struct mtp;				/* mtp structure */

typedef struct cc_bind {
	struct cc *tst;			/* tst ISUP bound */
	struct cc *mgm;			/* mgm ISUP bound */
	struct cc *mnt;			/* mnt ISUP bound */
	struct cc *icc;			/* icc ISUP bound */
	struct cc *ogc;			/* ogc ISUP bound */
} cc_bind_t;

#define ISUP_BIND_NONE	0
#define ISUP_BIND_CT	1
#define ISUP_BIND_CG	2
#define ISUP_BIND_TG	3
#define ISUP_BIND_SR	4
#define ISUP_BIND_SP	5
#define ISUP_BIND_DF	6

#define cc_t isup_t
/*
   isup user (call control) 
 */
typedef struct cc {
	STR_DECLARATION (struct cc);	/* stream declaration */
	struct {
		int type;		/* type of bind */
		union {
			struct df *df;	/* this ISUP bound to this DF */
			struct sp *sp;	/* this ISUP bound to this SP */
			struct sr *sr;	/* this ISUP bound to this SR */
			struct tg *tg;	/* this ISUP bound to this TG */
			struct cg *cg;	/* this ISUP bound to this CG */
			struct ct *ct;	/* this ISUP bound to this CT */
		} u;
		struct cc *next;	/* next ISUP bound to this entity */
		struct cc **prev;	/* prev ISUP bound to this entity */
	} bind;
	struct {
		struct ct *cpc;		/* circuit engaged in call */
		struct ct *mgm;		/* circuit engaged in management */
		struct ct *tst;		/* circuit engaged in continuity test */
		struct cg *gmg;		/* circuit group engaged in management */
	} conn;
	uint maxind;			/* max number of setup indications */
	uint setind;			/* cur number of setup indications */
	uint maxreq;			/* max number of setup requests */
	uint setreq;			/* cur number of setup requests */
	ulong mci;			/* message compatabilty */
	ulong pci;			/* parameter compatabilty */
} cc_t;
#define CC_PRIV(__q) ((struct cc *)(__q)->q_ptr)

/*
   circuit 
 */
typedef struct ct {
	HEAD_DECLARATION (struct ct);	/* head declaration */
	struct cc_bind bind;		/* bindings */
	mblk_t *sgm;			/* segmented message */
	mblk_t *rel;			/* retry message */
	ulong cic;			/* circuit identification code */
	ulong sls;			/* signalling link selection */
	ulong cref;			/* call reference (provider) */
	ulong uref;			/* call reference (user) */
	ulong c_state;			/* call processing state */
	ulong i_state;			/* call control interface state */
	ulong m_state;			/* mgm interface state */
	ulong t_state;			/* tst interface state */
	SLIST_LINKAGE (cc, ct, cpc);	/* CC list linkage (ISUP engaged in call) */
	SLIST_LINKAGE (cc, ct, mgm);	/* CC list linkage (ISUP engaged in management) */
	SLIST_LINKAGE (cc, ct, tst);	/* CC list linkage (ISUP engaged in cont teset) */
	SLIST_LINKAGE (sp, ct, sp);	/* SP list linkage */
	SLIST_LINKAGE (sr, ct, sr);	/* SR list linkage */
	SLIST_LINKAGE (tg, ct, tg);	/* TG list linkage */
	SLIST_LINKAGE (cg, ct, cg);	/* CG list linkage */
	struct {
		struct ct *next;	/* next CT in idle list */
		struct ct **prev;	/* prev CT in idle list */
	} idle;
	lmi_notify_t notify;		/* circuit notifications */
	isup_timers_ct_t timers;	/* circuit timers */
	isup_opt_conf_ct_t config;	/* circuit configuration */
	isup_stats_ct_t stats;		/* circuit statistics */
	isup_stats_ct_t statsp;		/* circuit statistics periods */
} ct_t;

#define RS_MAX_RANGE 33

/*
   circuit group 
 */
typedef struct cg {
	HEAD_DECLARATION (struct cg);	/* head declaration */
	struct cc_bind bind;		/* bindings */
	SLIST_HEAD (ct, ct);		/* CT list */
	ulong g_state;			/* gmg interface state */
	SLIST_LINKAGE (cc, cg, gmg);	/* CC list linkage (ISUP engaged in management) */
	SLIST_LINKAGE (sp, cg, sp);	/* SP list linkage */
	SLIST_LINKAGE (sr, cg, sr);	/* SR list linkage */
	ulong cic;			/* base circuit identification code */
	uchar rs_ptr[RS_MAX_RANGE];	/* range and status */
	size_t rs_len;			/* range and status length */
	uchar csi_ptr[RS_MAX_RANGE];	/* call stati information */
	size_t csi_len;			/* call state information length */
	lmi_option_t proto;		/* circuit group options */
	lmi_notify_t notify;		/* circuit group notifications */
	isup_timers_cg_t timers;	/* circuit group timers */
	isup_opt_conf_cg_t config;	/* circuit group configuration */
	isup_stats_cg_t stats;		/* circuit group statistics */
	isup_stats_cg_t statsp;		/* circuit group statistics periods */
} cg_t;

/*
   trunk group 
 */
typedef struct tg {
	HEAD_DECLARATION (struct tg);	/* head declaration */
	struct cc_bind bind;		/* bindings */
	SLIST_HEAD (ct, ct);		/* CT list */
	struct ct *idle;		/* CT idle list */
	SLIST_LINKAGE (sr, tg, sr);	/* SR list linkage */
	lmi_option_t proto;		/* trunk group protocol options */
	lmi_notify_t notify;		/* trunk group notifications */
	isup_timers_tg_t timers;	/* trunk group timers */
	isup_opt_conf_tg_t config;	/* trunk group configuration */
	isup_stats_tg_t stats;		/* trunk group statistics */
	isup_stats_tg_t statsp;		/* trunk group statistics periods */
} tg_t;

/*
   signalling relation 
 */
typedef struct sr {
	HEAD_DECLARATION (struct sr);	/* head declaration */
	struct mtp_addr add;		/* SS7 address associated with remote signalling point */
	struct cc_bind bind;		/* bindings */
	struct mtp *mtp;		/* MTP if linked at SR */
	SLIST_LINKAGE (sp, sr, sp);	/* list linkage */
	SLIST_HEAD (tg, tg);		/* trunk group list */
	SLIST_HEAD (cg, cg);		/* circuit grouplist */
	SLIST_HEAD (ct, ct);		/* circuit list */
	ulong cong_level;		/* congestion status of signalling relation */
	lmi_option_t proto;		/* signalling relation options */
	lmi_notify_t notify;		/* signalling relation notifications */
	isup_timers_sr_t timers;	/* signalling relation timers */
	isup_opt_conf_sr_t config;	/* signalling relation configuration */
	isup_stats_sr_t stats;		/* signalling relation statistics */
	isup_stats_sr_t statsp;		/* signalling relation statistics periods */
} sr_t;

/*
   signalling point 
 */
typedef struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	struct mtp_addr add;		/* SS7 address associated with local signalling point */
	struct cc_bind bind;		/* bindings */
	struct mtp *mtp;		/* MTP if linked at SP */
	SLIST_HEAD (sr, sr);		/* signalling relation list */
	SLIST_HEAD (tg, tg);		/* trunk gourp list */
	SLIST_HEAD (cg, cg);		/* circuit group list */
	SLIST_HEAD (ct, ct);		/* circuit list */
	lmi_option_t proto;		/* signalling point options */
	lmi_notify_t notify;		/* signalling point notifications */
	isup_timers_sp_t timers;	/* signalling point timers */
	isup_opt_conf_sp_t config;	/* signalling point configuration */
	isup_stats_sp_t stats;		/* signalling point statistics */
	isup_stats_sp_t statsp;		/* signalling point statistics periods */
} sp_t;

/*
   mtp user 
 */
typedef struct mtp {
	STR_DECLARATION (struct mtp);	/* stream declaration */
	struct sp *sp;			/* signalling point if linked at SP */
	struct sr *sr;			/* signalling relation if linked at SR */
	struct mtp_addr loc;		/* local signalling point */
	struct mtp_addr rem;		/* remote signalling point */
	lmi_option_t proto;		/* message transfer part options */
	lmi_notify_t notify;		/* message transfer part notifications */
	isup_timers_mtp_t timers;	/* message transfer part timers */
	isup_opt_conf_mtp_t config;	/* message transfer part configuration */
	isup_stats_mtp_t stats;		/* message transfer part statistics */
	isup_stats_mtp_t statsp;	/* message transfer part statistics periods */
} mtp_t;
#define MTP_PRIV(__q) ((struct mtp *)(__q)->q_ptr)

typedef struct df {
	spinlock_t lock;		/* structure lock */
	struct cc_bind bind;		/* bindings */
	SLIST_HEAD (cc, cc);		/* master list of isup */
	SLIST_HEAD (ct, ct);		/* master list of circuits */
	SLIST_HEAD (cg, cg);		/* master list of circuit groups */
	SLIST_HEAD (tg, tg);		/* master list of trunk groups */
	SLIST_HEAD (sr, sr);		/* master list of signalling relations */
	SLIST_HEAD (sp, sp);		/* master list of signalling points */
	SLIST_HEAD (mtp, mtp);		/* master list of message transfer parts */
	lmi_option_t proto;		/* default protocol options */
	lmi_notify_t notify;		/* default notifications */
	isup_opt_conf_df_t config;	/* default configuration */
	isup_stats_df_t stats;		/* default statistics */
	isup_stats_df_t statsp;		/* default statistics periods */
} df_t;
STATIC struct df master;

/*
 *  Forward declarations.
 */
STATIC struct cc *isup_alloc_cc(queue_t *, struct cc **, dev_t *, cred_t *);
STATIC void isup_free_cc(queue_t *);
STATIC struct cc *cc_get(struct cc *);
STATIC void cc_put(struct cc *);

STATIC struct ct *isup_alloc_ct(ulong, struct tg *, struct cg *, ulong);
STATIC void isup_free_ct(struct ct *);
STATIC struct ct *ct_get(struct ct *);
STATIC void ct_put(struct ct *);
STATIC struct ct *ct_lookup(ulong);
STATIC ulong ct_get_id(ulong);

STATIC struct cg *isup_alloc_cg(ulong, struct sr *);
STATIC void isup_free_cg(struct cg *);
STATIC struct cg *cg_get(struct cg *);
STATIC void cg_put(struct cg *);
STATIC struct cg *cg_lookup(ulong);
STATIC ulong cg_get_id(ulong);

STATIC struct tg *isup_alloc_tg(ulong, struct sr *sr);
STATIC void isup_free_tg(struct tg *);
STATIC struct tg *tg_get(struct tg *);
STATIC void tg_put(struct tg *);
STATIC struct tg *tg_lookup(ulong);
STATIC ulong tg_get_id(ulong);

STATIC struct sr *isup_alloc_sr(ulong, struct sp *, mtp_addr_t *);
STATIC void isup_free_sr(struct sr *);
STATIC struct sr *sr_get(struct sr *);
STATIC void sr_put(struct sr *);
STATIC struct sr *sr_lookup(ulong);
STATIC ulong sr_get_id(ulong);

STATIC struct sp *isup_alloc_sp(ulong, mtp_addr_t *);
STATIC void isup_free_sp(struct sp *);
STATIC struct sp *sp_get(struct sp *);
STATIC void sp_put(struct sp *);
STATIC struct sp *sp_lookup(ulong);
STATIC ulong sp_get_id(ulong);

STATIC struct mtp *isup_alloc_mtp(queue_t *, struct mtp **, ulong, cred_t *);
STATIC void isup_free_mtp(queue_t *);
STATIC struct mtp *mtp_get(struct mtp *);
STATIC void mtp_put(struct mtp *);
STATIC struct mtp *mtp_lookup(ulong);
STATIC ulong mtp_get_id(ulong);

/*
 *  =========================================================================
 *
 *  STATE Changes
 *
 *  =========================================================================
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
	case CCS_UNBND:
		return ("CCS_UNBND");
	case CCS_IDLE:
		return ("CCS_IDLE");
	case CCS_WIND_SETUP:
		return ("CCS_WIND_SETUP");
	case CCS_WREQ_SETUP:
		return ("CCS_WREQ_SETUP");
	case CCS_WREQ_MORE:
		return ("CCS_WREQ_MORE");
	case CCS_WIND_MORE:
		return ("CCS_WIND_MORE");
	case CCS_WREQ_INFO:
		return ("CCS_WREQ_INFO");
	case CCS_WIND_INFO:
		return ("CCS_WIND_INFO");
	case CCS_WACK_INFO:
		return ("CCS_WACK_INFO");
	case CCS_WCON_SREQ:
		return ("CCS_WCON_SREQ");
	case CCS_WRES_SIND:
		return ("CCS_WRES_SIND");
	case CCS_WREQ_CCREP:
		return ("CCS_WREQ_CCREP");
	case CCS_WIND_CCREP:
		return ("CCS_WIND_CCREP");
	case CCS_WREQ_PROCEED:
		return ("CCS_WREQ_PROCEED");
	case CCS_WIND_PROCEED:
		return ("CCS_WIND_PROCEED");
	case CCS_WACK_PROCEED:
		return ("CCS_WACK_PROCEED");
	case CCS_WREQ_ALERTING:
		return ("CCS_WREQ_ALERTING");
	case CCS_WIND_ALERTING:
		return ("CCS_WIND_ALERTING");
	case CCS_WACK_ALERTING:
		return ("CCS_WACK_ALERTING");
	case CCS_WREQ_PROGRESS:
		return ("CCS_WREQ_PROGRESS");
	case CCS_WIND_PROGRESS:
		return ("CCS_WIND_PROGRESS");
	case CCS_WACK_PROGRESS:
		return ("CCS_WACK_PROGRESS");
	case CCS_WREQ_IBI:
		return ("CCS_WREQ_IBI");
	case CCS_WIND_IBI:
		return ("CCS_WIND_IBI");
	case CCS_WACK_IBI:
		return ("CCS_WACK_IBI");
	case CCS_WREQ_CONNECT:
		return ("CCS_WREQ_CONNECT");
	case CCS_WIND_CONNECT:
		return ("CCS_WIND_CONNECT");
	case CCS_WACK_FORWXFER:
		return ("CCS_WACK_FORWXFER");
	case CCS_CONNECTED:
		return ("CCS_CONNECTED");
	case CCS_SUSPENDED:
		return ("CCS_SUSPENDED");
	case CCS_WCON_RELREQ:
		return ("CCS_WCON_RELREQ");
	case CCS_WRES_RELIND:
		return ("CCS_WRES_RELIND");
	case CCS_UNUSABLE:
		return ("CCS_UNUSABLE");
	default:
		return ("(unknown)");
	}
}
STATIC const char *
ck_state_name(long state)
{
	switch (state) {
	case CKS_IDLE:
		return ("CKS_IDLE");
	case CKS_WIND_CONT:
		return ("CKS_WIND_CONT");
	case CKS_WRES_CONT:
		return ("CKS_WRES_CONT");
	case CKS_WIND_CTEST:
		return ("CKS_WIND_CTEST");
	case CKS_WREQ_CTEST:
		return ("CKS_WREQ_CTEST");
	case CKS_WIND_CCREP:
		return ("CKS_WIND_CCREP");
	case CKS_WREQ_CCREP:
		return ("CKS_WREQ_CCREP");
	case CKS_WCON_RELREQ:
		return ("CKS_WCON_RELREQ");
	case CKS_WRES_RELIND:
		return ("CKS_WRES_RELIND");
	default:
		return ("(unknown)");
	}
}
STATIC const char *
cp_state_name(long state)
{
	switch (state) {
	case CTS_IDLE:
		return ("CTS_IDLE");
	case CTS_ICC_WAIT_CCR:
		return ("CTS_ICC_WAIT_CCR");
	case CTS_ICC_WAIT_SAM:
		return ("CTS_ICC_WAIT_SAM");
	case CTS_ICC_WAIT_ACM:
		return ("CTS_ICC_WAIT_ACM");
	case CTS_ICC_WAIT_ANM:
		return ("CTS_ICC_WAIT_ANM");
	case CTS_ICC_ANSWERED:
		return ("CTS_ICC_ANSWERED");
	case CTS_ICC_SUSPENDED:
		return ("CTS_ICC_SUSPENDED");
	case CTS_ICC_WAIT_RLC:
		return ("CTS_ICC_WAIT_RLC");
	case CTS_ICC_SEND_RLC:
		return ("CTS_ICC_SEND_RLC");
	case CTS_OGC_WAIT_CCR:
		return ("CTS_OGC_WAIT_CCR");
	case CTS_OGC_WAIT_SAM:
		return ("CTS_OGC_WAIT_SAM");
	case CTS_OGC_WAIT_ACM:
		return ("CTS_OGC_WAIT_ACM");
	case CTS_OGC_WAIT_ANM:
		return ("CTS_OGC_WAIT_ANM");
	case CTS_OGC_ANSWERED:
		return ("CTS_OGC_ANSWERED");
	case CTS_OGC_SUSPENDED:
		return ("CTS_OGC_SUSPENDED");
	case CTS_OGC_WAIT_RLC:
		return ("CTS_OGC_WAIT_RLC");
	case CTS_OGC_SEND_RLC:
		return ("CTS_OGC_SEND_RLC");
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
	case CMS_WCON_BLREQ:
		return ("CMS_WCON_BLREQ");
	case CMS_WRES_BLIND:
		return ("CMS_WRES_BLIND");
	case CMS_WACK_BLRES:
		return ("CMS_WACK_BLRES");
	case CMS_WCON_UBREQ:
		return ("CMS_WCON_UBREQ");
	case CMS_WRES_UBIND:
		return ("CMS_WRES_UBIND");
	case CMS_WACK_UBRES:
		return ("CMS_WACK_UBRES");
	case CMS_WCON_RESREQ:
		return ("CMS_WCON_RESREQ");
	case CMS_WRES_RESIND:
		return ("CMS_WRES_RESIND");
	case CMS_WACK_RESRES:
		return ("CMS_WACK_RESRES");
	case CMS_WCON_QRYREQ:
		return ("CMS_WCON_QRYREQ");
	case CMS_WRES_QRYIND:
		return ("CMS_WRES_QRYIND");
	case CMS_WACK_QRYRES:
		return ("CMS_WACK_QRYRES");
	default:
		return ("(unknown)");
	}
}
STATIC const char *
ct_flag_name(ulong flag)
{
	switch (flag) {
	case CCTF_COR_PENDING:
		return ("CCTF_COR_PENDING");
	case CCTF_COT_PENDING:
		return ("CCTF_COT_PENDING");
	case CCTF_LOC_G_BLOCK_PENDING:
		return ("CCTF_LOC_G_BLOCK_PENDING");
	case CCTF_LOC_G_UNBLOCK_PENDING:
		return ("CCTF_LOC_G_UNBLOCK_PENDING");
	case CCTF_LOC_H_BLOCKED:
		return ("CCTF_LOC_H_BLOCKED");
	case CCTF_LOC_H_BLOCK_PENDING:
		return ("CCTF_LOC_H_BLOCK_PENDING");
	case CCTF_LOC_H_UNBLOCK_PENDING:
		return ("CCTF_LOC_H_UNBLOCK_PENDING");
	case CCTF_LOC_M_BLOCKED:
		return ("CCTF_LOC_M_BLOCKED");
	case CCTF_LOC_M_BLOCK_PENDING:
		return ("CCTF_LOC_M_BLOCK_PENDING");
	case CCTF_LOC_M_UNBLOCK_PENDING:
		return ("CCTF_LOC_M_UNBLOCK_PENDING");
	case CCTF_LOC_QUERY_PENDING:
		return ("CCTF_LOC_QUERY_PENDING");
	case CCTF_LOC_RESET_PENDING:
		return ("CCTF_LOC_RESET_PENDING");
	case CCTF_LOC_S_BLOCKED:
		return ("CCTF_LOC_S_BLOCKED");
	case CCTF_LPA_PENDING:
		return ("CCTF_LPA_PENDING");
	case CCTF_ORIG_SUSPENDED:
		return ("CCTF_ORIG_SUSPENDED");
	case CCTF_REM_G_BLOCK_PENDING:
		return ("CCTF_REM_G_BLOCK_PENDING");
	case CCTF_REM_G_UNBLOCK_PENDING:
		return ("CCTF_REM_G_UNBLOCK_PENDING");
	case CCTF_REM_H_BLOCKED:
		return ("CCTF_REM_H_BLOCKED");
	case CCTF_REM_H_BLOCK_PENDING:
		return ("CCTF_REM_H_BLOCK_PENDING");
	case CCTF_REM_H_UNBLOCK_PENDING:
		return ("CCTF_REM_H_UNBLOCK_PENDING");
	case CCTF_REM_M_BLOCKED:
		return ("CCTF_REM_M_BLOCKED");
	case CCTF_REM_M_BLOCK_PENDING:
		return ("CCTF_REM_M_BLOCK_PENDING");
	case CCTF_REM_M_UNBLOCK_PENDING:
		return ("CCTF_REM_M_UNBLOCK_PENDING");
	case CCTF_REM_QUERY_PENDING:
		return ("CCTF_REM_QUERY_PENDING");
	case CCTF_REM_RESET_PENDING:
		return ("CCTF_REM_RESET_PENDING");
	case CCTF_TERM_SUSPENDED:
		return ("CCTF_TERM_SUSPENDED");
	case CCTF_UPT_PENDING:
		return ("CCTF_UPT_PENDING");
	default:
		return ("(unknown)");
	}
}
#endif

STATIC INLINE long
cs_get_state(struct cc *cc)
{
	return (cc->state);
}
STATIC INLINE long
ct_get_state(struct ct *ct)
{
	return (ct->state);
}
STATIC INLINE long
cg_get_state(struct cg *cg)
{
	return cg->state;
}
STATIC INLINE long
cg_get_m_state(struct cg *cg)
{
	return cg->g_state;
}
STATIC INLINE long
ct_get_m_state(struct ct *ct)
{
	return ct->m_state;
}
STATIC INLINE long
ct_get_t_state(struct ct *ct)
{
	return ct->t_state;
}
STATIC INLINE long
ct_get_i_state(struct ct *ct)
{
	return ct->i_state;
}

STATIC INLINE void
cs_set_state(struct cc *cc, const long newstate)
{
	long oldstate = cs_get_state(cc);
	(void) oldstate;
	printd(("%s: %p: %d:%d cc-state %s <- %s\n", DRV_NAME, cc, cc->u.dev.cmajor,
		cc->u.dev.cminor, cc_state_name(newstate), cc_state_name(oldstate)));
	cc->state = newstate;
}
STATIC INLINE void
ct_set_state(struct ct *ct, const long newstate)
{
	long oldstate = ct_get_state(ct);
	(void) oldstate;
	printd(("%s: %p: ct %2ld ct-state %s <- %s\n", DRV_NAME, ct, ct->id,
		cp_state_name(newstate), cp_state_name(oldstate)));
	ct->state = newstate;
}
STATIC INLINE void
cg_set_state(struct cg *cg, const long newstate)
{
	long oldstate = cg_get_state(cg);
	(void) oldstate;
	printd(("%s: %p: cg %2ld cg-state %s <- %s\n", DRV_NAME, cg, cg->id,
		cp_state_name(newstate), cp_state_name(oldstate)));
	cg->state = newstate;
}

STATIC INLINE ulong
cc_get_uref(struct ct *ct, struct cc *cc, ulong uref)
{
	if (!ct->uref) {
		struct ct *c2;
		for (c2 = cc->conn.cpc; c2 && c2->uref != uref; c2 = c2->cpc.next) ;
		if (c2)
			return (0);
		ct->uref = uref;
		cc->setreq++;
		return (ct->uref);
	} else
		swerr();
	return (0);
}
STATIC INLINE void
cc_put_uref(struct ct *ct, struct cc *cc)
{
	if (ct_get_i_state(ct) != CCS_WCON_SREQ) {
		assure(ct->uref);
		ct->uref = 0;
		assure(cc->setreq > 0);
		--cc->setreq;
	} else
		swerr();
}
STATIC INLINE struct ct *
cc_find_cpc_uref(struct cc *cc, ulong uref)
{
	struct ct *ct = NULL;
	if (uref)
		for (ct = cc->conn.cpc; ct && ct->uref != uref; ct = ct->cpc.next) ;
	if (ct && ct->cpc.cc != cc) {
		swerr();
		return NULL;
	}
	return (ct);
}

STATIC INLINE ulong
cc_get_cref(struct ct *ct, struct cc *cc)
{
	static ulong reference = 0;
	if (ct_get_m_state(ct) == CMS_IDLE && ct_get_i_state(ct) == CCS_IDLE
	    && ct_get_t_state(ct) == CKS_IDLE) {
		assure(!ct->cref);
		if (!ct->cref) {
			if (++reference == 0L)
				reference = 1L;
			ct->cref = reference;
		}
	}
	if (!(cc->setind++))
		cs_set_state(cc, CCS_WRES_SIND);
	return (ct->cref);
}
STATIC INLINE struct ct *
cc_find_cpc_cref(struct cc *cc, ulong cref)
{
	struct ct *ct = NULL;
	if (cref)
		for (ct = cc->conn.cpc; ct && ct->cref != cref; ct = ct->cpc.next) ;
	if (ct && ct->cpc.cc != cc) {
		swerr();
		return NULL;
	}
	return (ct);
}
STATIC INLINE struct ct *
cc_find_tst_cref(struct cc *cc, ulong cref)
{
	struct ct *ct = NULL;
	if (cref)
		for (ct = cc->conn.tst; ct && ct->cref != cref; ct = ct->tst.next) ;
	if (ct && ct->tst.cc != cc) {
		swerr();
		return NULL;
	}
	return (ct);
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
cc_put_cref(struct ct *ct, struct cc *cc)
{
	if (ct_get_m_state(ct) == CMS_IDLE && ct_get_i_state(ct) == CCS_IDLE
	    && ct_get_t_state(ct) == CKS_IDLE) {
		assure(ct->cref);
		ct->cref = 0;
	}
	assure(cc->setind > 0);
	if (!(--cc->setind))
		cs_set_state(cc, CCS_IDLE);
}

STATIC INLINE void
cg_set_m_state(struct cg *cg, struct cc *cc, const long newstate)
{
	if (cg && cc) {
		long oldstate = cg_get_m_state(cg);
		printd(("%s: %p: cg %2ld cc %d:%d g-state %s <- %s\n", DRV_NAME, cg, cg->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, cm_state_name(newstate),
			cm_state_name(oldstate)));
		if (oldstate == CMS_IDLE && newstate != CMS_IDLE) {
			/* 
			   make cc current */
			cg->gmg.cc = cc_get(cc);
			if ((cg->gmg.next = cc->conn.gmg))
				cg->gmg.next->gmg.prev = &cg->gmg.next;
			cg->gmg.prev = &cc->conn.gmg;
			cc->conn.gmg = cg_get(cg);
		} else if (oldstate != CMS_IDLE && newstate == CMS_IDLE) {
			/* 
			   detach from current */
			cg_put(xchg(&cc->conn.gmg, NULL));
			if ((*cg->gmg.prev = cg->gmg.next))
				cg->gmg.next->gmg.prev = cg->gmg.prev;
			cg->gmg.next = NULL;
			cg->gmg.prev = &cg->gmg.next;
			cc_put(xchg(&cg->gmg.cc, NULL));
		}
		cg->g_state = newstate;
		return;
	}
	swerr();
}
STATIC INLINE void
ct_set_m_state(struct ct *ct, struct cc *cc, const long newstate)
{
	if (ct && cc) {
		long oldstate = ct_get_m_state(ct);
		printd(("%s: %p: ct %2ld cc %d:%d m-state %s <- %s\n", DRV_NAME, ct, ct->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, cm_state_name(newstate),
			cm_state_name(oldstate)));
		if (oldstate == CMS_IDLE && newstate != CMS_IDLE) {
			/* 
			   make cc current */
			cc_get_cref(ct, cc);
			ct->mgm.cc = cc_get(cc);
			if ((ct->mgm.next = cc->conn.mgm))
				ct->mgm.next->mgm.prev = &ct->mgm.next;
			ct->mgm.prev = &cc->conn.mgm;
			cc->conn.mgm = ct_get(ct);
		} else if (oldstate != CMS_IDLE && newstate == CMS_IDLE) {
			/* 
			   detach from current */
			ct_put(xchg(&cc->conn.mgm, NULL));
			if ((*ct->mgm.prev = ct->mgm.next))
				ct->mgm.next->mgm.prev = ct->mgm.prev;
			ct->mgm.next = NULL;
			ct->mgm.prev = &ct->mgm.next;
			cc_put(xchg(&ct->mgm.cc, NULL));
			ct->m_state = newstate;
			cc_put_cref(ct, cc);
			return;
		}
		ct->m_state = newstate;
		return;
	}
	swerr();
}
STATIC INLINE void
ct_set_t_state(struct ct *ct, struct cc *cc, const long newstate)
{
	if (ct && cc) {
		long oldstate = ct_get_t_state(ct);
		printd(("%s: %p: ct %2ld cc %d:%d t-state %s <- %s\n", DRV_NAME, ct, ct->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, ck_state_name(newstate),
			ck_state_name(oldstate)));
		if (oldstate == CKS_IDLE && newstate != CKS_IDLE) {
			/* 
			   make cc current */
			cc_get_cref(ct, cc);
			ct->tst.cc = cc_get(cc);
			if ((ct->tst.next = cc->conn.tst))
				ct->tst.next->tst.prev = &ct->tst.next;
			ct->tst.prev = &cc->conn.tst;
			cc->conn.tst = ct_get(ct);
		} else if (oldstate != CKS_IDLE && newstate == CKS_IDLE) {
			/* 
			   detach from current */
			ct_put(xchg(&cc->conn.tst, NULL));
			if ((*ct->tst.prev = ct->tst.next))
				ct->tst.next->tst.prev = ct->tst.prev;
			ct->tst.next = NULL;
			ct->tst.prev = &ct->tst.next;
			cc_put(xchg(&ct->tst.cc, NULL));
			ct->t_state = newstate;
			cc_put_cref(ct, cc);
			return;
		}
		ct->t_state = newstate;
		return;
	}
	swerr();
}
STATIC INLINE void
ct_set_i_state(struct ct *ct, struct cc *cc, const long newstate)
{
	if (ct && cc) {
		long oldstate = ct_get_i_state(ct);
		printd(("%s: %p: ct %2ld cc %d:%d i-state %s <- %s\n", DRV_NAME, ct, ct->id,
			cc->u.dev.cmajor, cc->u.dev.cminor, cc_state_name(newstate),
			cc_state_name(oldstate)));
		if (oldstate == CCS_IDLE && newstate != CCS_IDLE) {
			/* 
			   make cc current */
			cc_get_cref(ct, cc);
			ct->cpc.cc = cc_get(cc);
			if ((ct->cpc.next = cc->conn.cpc))
				ct->cpc.next->cpc.prev = &ct->cpc.next;
			ct->cpc.prev = &cc->conn.cpc;
			cc->conn.cpc = ct_get(ct);
		} else if (oldstate != CCS_IDLE && newstate == CCS_IDLE) {
			/* 
			   detach from current */
			ct_put(xchg(&cc->conn.cpc, NULL));
			if ((*ct->cpc.prev = ct->cpc.next))
				ct->cpc.next->cpc.prev = ct->cpc.prev;
			ct->cpc.next = NULL;
			ct->cpc.prev = &ct->cpc.next;
			cc_put(xchg(&ct->cpc.cc, NULL));
			ct->i_state = newstate;
			cc_put_cref(ct, cc);
			if (ct->uref)
				cc_put_uref(ct, cc);
		}
		ct->i_state = newstate;
		return;
	}
	swerr();
}

STATIC INLINE void
ct_swap_cref(struct ct *ct, struct cc *cc)
{
	if (ct && cc) {
		/* 
		   remove from old stream */
		if ((*ct->cpc.prev = ct->cpc.next))
			ct->cpc.next->cpc.prev = ct->cpc.prev;
		ct->cpc.next = NULL;
		ct->cpc.prev = &ct->cpc.next;
		cc_put(xchg(&ct->cpc.cc, NULL));
		/* 
		   add to new stream */
		ct->cpc.cc = cc_get(cc);
		if ((ct->cpc.next = cc->conn.cpc))
			ct->cpc.next->cpc.prev = &ct->cpc.next;
		ct->cpc.prev = &cc->conn.cpc;
		cc->conn.cpc = ct;
		return;
	}
	swerr();
}

STATIC INLINE long
ct_get_c_state(struct ct *ct)
{
	return ct->c_state;
}
STATIC INLINE void
__ct_set_c_state(struct ct *ct, const long newstate)
{
	struct ct **ctp;
	long oldstate = ct_get_c_state(ct);
	printd(("%s: %p: ct %2ld c-state %s <- %s\n", DRV_NAME, ct, ct->id,
		cp_state_name(newstate), cp_state_name(oldstate)));
	if (oldstate == CTS_IDLE) {
		/* 
		   possibly remove circuit from idle list */
		/* 
		   has no effect if not on list cause prev points to next */
		if ((*ct->idle.prev = ct->idle.next))
			ct->idle.next->idle.prev = ct->idle.prev;
		ct->idle.next = NULL;
		ct->idle.prev = &ct->idle.next;
	}
	if (newstate == CTS_IDLE && !(ct->flags & (CCTM_OUT_OF_SERVICE))) {
		/* 
		   add circuit to idle list */
		switch (ct->tg.tg->config.select_type) {
		default:
		case ISUP_SELECTION_TYPE_MRU:	/* head is most recently used - insert at head */
			ctp = &ct->tg.tg->idle;
			break;
		case ISUP_SELECTION_TYPE_LRU:	/* head is least recently used - insert at tail */
			for (ctp = &ct->tg.tg->idle; *ctp; ctp = &((*ctp)->idle.next)) ;
			break;
		case ISUP_SELECTION_TYPE_ASEQ:	/* head is lowest cic - find insertion point */
			for (ctp = &ct->tg.tg->idle; *ctp && (*ctp)->cic < ct->cic;
			     ctp = &((*ctp)->idle.next)) ;
			break;
		case ISUP_SELECTION_TYPE_DSEQ:	/* head is highest cic - find insertion point */
			for (ctp = &ct->tg.tg->idle; *ctp && (*ctp)->cic > ct->cic;
			     ctp = &((*ctp)->idle.next)) ;
			break;
		}
		/* 
		   insert in tg idle list */
		if ((ct->idle.next = *ctp))
			ct->idle.next->idle.prev = &ct->idle.next;
		ct->idle.prev = ctp;
		*ctp = ct;
	}
	ct->c_state = newstate;
}
STATIC INLINE void
ct_set_c_state(struct ct *ct, const long newstate)
{
	__ct_set_c_state(ct, newstate);
	switch (newstate) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
	case CTS_ICC_WAIT_CCR:
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
	case CTS_OGC_WAIT_CCR:
		break;
	default:
		ct->flags &= ~(CCTM_CONT_CHECK);
		break;
	}
}

STATIC INLINE ulong
ct_set(struct ct *ct, const ulong flags)
{
	int i;
	ulong newflags = (ct->flags & flags) ^ flags;
	ct->flags |= flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: ct %2ld SET %s\n", DRV_NAME, ct, ct->id,
				ct_flag_name(newflags & (0x1UL << i))));
	__ct_set_c_state(ct, ct_get_c_state(ct));
	return (ct->flags);
}
STATIC INLINE ulong
ct_clr(struct ct *ct, const ulong flags)
{
	int i;
	ulong newflags = (~ct->flags & flags) ^ flags;
	ct->flags &= ~flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: ct %2ld CLR %s\n", DRV_NAME, ct, ct->id,
				ct_flag_name(newflags & (0x1UL << i))));
	__ct_set_c_state(ct, ct_get_c_state(ct));
	return (ct->flags);
}
STATIC INLINE ulong
ct_tst(struct ct *ct, const ulong flags)
{
	return (ct->flags & flags);
}

STATIC INLINE ulong
cg_set(struct cg *cg, const ulong flags)
{
	int i;
	ulong newflags = (cg->flags & flags) ^ flags;
	cg->flags |= flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: cg %2ld CLR %s\n", DRV_NAME, cg, cg->id,
				ct_flag_name(newflags & (0x1UL << i))));
	return (cg->flags);
}
STATIC INLINE ulong
cg_clr(struct cg *cg, const ulong flags)
{
	int i;
	ulong newflags = (~cg->flags & flags) ^ flags;
	cg->flags &= ~flags;
	newflags = cg->flags ^ flags;
	for (i = 0; i < sizeof(newflags) << 3; i++)
		if (newflags & (0x1UL << i))
			printd(("%s: %p: cg %2ld CLR %s\n", DRV_NAME, cg, cg->id,
				ct_flag_name(newflags & (0x1UL << i))));
	return (cg->flags);
}
STATIC INLINE ulong
cg_tst(struct cg *cg, const ulong flags)
{
	return (cg->flags & flags);
}

STATIC INLINE ulong
tg_set(struct tg *tg, const ulong flags)
{
	tg->flags |= flags;
	return (tg->flags);
}
STATIC INLINE ulong
tg_clr(struct tg *tg, const ulong flags)
{
	tg->flags &= ~flags;
	return (tg->flags);
}
STATIC INLINE ulong
tg_tst(struct tg *tg, const ulong flags)
{
	return (tg->flags & flags);
}

STATIC struct ct *
isup_find_cct(struct cc *cc, uchar *add_ptr, size_t add_len)
{
	struct ct *ct = NULL;
	if (add_len) {
		struct isup_addr *a;
		if (add_len >= sizeof(*a)) {
			a = (typeof(a)) (add_ptr);
			switch (a->scope) {
			case ISUP_SCOPE_CT:
				if (a->id)
					ct = ct_lookup(a->id);
				else if (cc->bind.type == ISUP_BIND_CT)
					if ((ct = cc->bind.u.ct)) ;
				break;
			case ISUP_SCOPE_CG:
			{
				struct cg *cg = NULL;
				if (a->id)
					cg = cg_lookup(a->id);
				else if (cc->bind.type == ISUP_BIND_CG)
					cg = cc->bind.u.cg;
				if (cg)
					for (ct = cg->ct.list; ct && ct->cic != a->cic;
					     ct = ct->cg.next) ;
				break;
			}
			case ISUP_SCOPE_TG:
			{
				struct tg *tg = NULL;
				if (a->id)
					tg = tg_lookup(a->id);
				else if (cc->bind.type == ISUP_BIND_TG)
					tg = cc->bind.u.tg;
				if (tg)
					for (ct = tg->ct.list; ct && ct->cic != a->cic;
					     ct = ct->tg.next) ;
				break;
			}
			case ISUP_SCOPE_SR:
			{
				struct sr *sr = NULL;
				if (a->id)
					sr = sr_lookup(a->id);
				else if (cc->bind.type == ISUP_BIND_SR)
					sr = cc->bind.u.sr;
				if (sr)
					for (ct = sr->ct.list; ct && ct->cic != a->cic;
					     ct = ct->sr.next) ;
				break;
			}
			}
		}
	} else
		switch (cc->bind.type) {
		case ISUP_BIND_CT:
			if ((ct = cc->bind.u.ct)) ;
			break;
		}
	return (ct);
}

STATIC struct cg *
isup_find_grp(struct cc *cc, uchar *add_ptr, size_t add_len)
{
	struct cg *cg = NULL;
	if (add_len) {
		struct isup_addr *a;
		if (add_len >= sizeof(*a)) {
			a = (typeof(a)) (add_ptr);
			switch (a->scope) {
			case ISUP_SCOPE_CT:
			{
				struct ct *ct = NULL;
				if (a->id)
					ct = ct_lookup(a->id);
				else if (cc->bind.type == ISUP_BIND_CT)
					ct = cc->bind.u.ct;
				if (ct)
					cg = ct->cg.cg;
				break;
			}
			case ISUP_SCOPE_CG:
				if (a->id)
					cg = cg_lookup(a->id);
				else if (cc->bind.type == ISUP_BIND_CG)
					cg = cc->bind.u.cg;
				break;
			case ISUP_SCOPE_SR:
			{
				struct sr *sr = NULL;
				if (a->id)
					sr = sr_lookup(a->id);
				else if (cc->bind.type == ISUP_BIND_SR)
					sr = cc->bind.u.sr;
				if (sr)
					for (cg = sr->cg.list; cg && cg->cic != a->cic;
					     cg = cg->sr.next) ;
				break;
			}
			}
		}
	} else
		switch (cc->bind.type) {
		case ISUP_BIND_CT:
			if (cc->bind.u.ct)
				cg = cc->bind.u.ct->cg.cg;
			break;
		case ISUP_BIND_CG:
			if ((cg = cc->bind.u.cg)) ;
			break;
		}
	return (cg);
}

/*
 *  =========================================================================
 *
 *  ISUP Functions
 *
 *  =========================================================================
 */

#if 0
/*
 *  CHECK SRC (BIND) ADDRESS
 *  -----------------------------------
 *  Check the source (bind) address.  This must be an address local to the
 *  stack.
 */
STATIC int
isup_check_src(struct cc *cc, isup_addr_t * src)
{
	return (-EFAULT);
}

/*
 *  CHECK DST (CONN) ADDRESS
 *  -----------------------------------
 *  Check the destination (connect) address.  This may be a local, adjacent or
 *  remote address and the connection must be for a destination point code and
 *  CIC range which is not already connected.
 */
STATIC int
isup_check_dst(struct cc *cc, isup_addr_t * dst)
{
	return (-EFAULT);
}

/*
 *  ISUP BIND
 *  -----------------------------------
 *  Basically just check the source address for validity.
 */
STATIC int
isup_bind(struct cc *cc, isup_addr_t * src)
{
	return (-EFAULT);
}

/*
 *  ISUP CONNECT
 *  -----------------------------------
 *  Add the ISUP user to trunk connection hashes.
 */
STATIC int
isup_connect(struct cc *cc, isup_addr_t * dst)
{
	return (-EFAULT);
}

/*
 *  ISUP DISCONNECT
 *  -----------------------------------
 *  Remove the ISUP user from the trunk connection hashes.
 */
STATIC int
isup_disconnect(struct cc *cc)
{
	return (-EFAULT);
}

/*
 *  ISUP SENDMSG
 *  -----------------------------------
 *  Send a message on a connection.
 */
STATIC int
isup_sendmsg(struct cc *cc, cc_opts_t * opt, isup_addr_t * dst, mblk_t *dp)
{
	return (-EFAULT);
}
#endif

/*
 *  =========================================================================
 *
 *  ISUP MESSAGE ENCODING/DECODING
 *
 *  =========================================================================
 */
typedef struct isup_var {
	uchar *ptr;			/* pointer to parameter contents */
	size_t len;			/* lengtho of parameter contents */
	uint32_t val;			/* up to first 4 bytes of parameter contents */
} isup_var_t;

typedef struct isup_msg {
	mblk_t *bp;			/* identd */
	queue_t *eq;			/* queue to write errors to */
	queue_t *xq;			/* queue to write results to */
	struct cc *cc;			/* ISUP-User to which this message belongs */
	struct mtp *mtp;		/* MTP-User to which this message belongs */
	ulong timestamp;		/* jiffie clock timestamp */
	ulong mp;			/* message priority */
	ulong sls;			/* signalling link selection code */
	ulong cic;			/* circuit identification code */
	ulong mt;			/* message type */
	union {
		struct {
			ulong nci;	/* nature of connection indicators */
			ulong fci;	/* forward connection indicators */
			ulong cpc;	/* called party category */
			ulong tmr;	/* transmission medium requirement */
			isup_var_t usi;	/* user service information */
			isup_var_t cdpn;	/* called party number */
		} iam;
		struct {
			isup_var_t subn;	/* subsequent number */
		} sam;
		struct {
			ulong inri;	/* information request indicators */
		} inr;
		struct {
			ulong infi;	/* information indicators */
		} inf;
		struct {
			ulong coti;	/* continuity test indicators */
		} cot;
		struct {
			ulong bci;	/* backwards call indicators */
		} acm;
		struct {
			ulong bci;	/* backward call indicators */
		} con;
		struct {
		} fot, anm;
		struct {
			isup_var_t caus;	/* cause value */
		} rel;
		struct {
			ulong sris;	/* suspend resume indicators */
		} sus, res;
		struct {
		} rlc, ccr, rsc, blo, ubl, bla, uba;
		struct {
			isup_var_t rs;	/* range and status */
		} grs, gra;
		struct {
			ulong cgi;	/* circuit state group indicators */
			isup_var_t rs;	/* range and status */
		} cgb, cgba, cgu, cgua;
		struct {
			ulong cmi;	/* call modification indicators */
		} cmr, cmc, cmrj;
		struct {
			ulong faci;	/* facility indicators */
		} far, faa;
		struct {
			ulong faci;	/* facility indicators */
			isup_var_t caus;	/* cause value */
		} frj;
		struct {
			ulong faci;	/* facility indicators */
		} fad;
		struct {
			ulong faci;	/* facility indicators */
			ulong faii;	/* facility information indicators */
		} fai;
		struct {
		} lpa, drs;
		struct {
			isup_var_t msg;	/* encapsulated message */
		} pam;
		struct {
			isup_var_t rs;	/* range and status */
		} cqm;
		struct {
			isup_var_t rs;	/* range and status */
			isup_var_t csi;	/* circuit state indicator */
		} cqr;
		struct {
			ulong evnt;	/* event */
		} cpg;
		struct {
			isup_var_t uui;	/* user to user information */
		} usr;
		struct {
		} ucic;
		struct {
			isup_var_t caus;	/* cause value */
		} cfn;
		struct {
		} olm;
		struct {
			isup_var_t icci;	/* charge indicators */
		} crg;
		struct {
		} nrm, fac, upt, upa, idr, irs, seg, cra;
		struct {
			ulong nci;	/* nature of connection indicators */
		} crm;
		struct {
			ulong cvri;
			ulong cvci;
			ulong cgci;
		} cvr;
		struct {
		} cvt, exm;
		struct {
			ulong ton;
		} non;
		struct {
		} llm, cak;
		struct {
			ulong cri;
		} tcm;
		struct {
		} mcp;
	} msg;
	isup_var_t opt;			/* optional parameters */
	struct {
		isup_var_t eop;
		isup_var_t clrf;
		ulong tmr;
		isup_var_t atp;
		isup_var_t cdpn;
		isup_var_t subn;
		ulong nci;
		ulong fci;
		isup_var_t ofci;
		ulong cpc;
		isup_var_t cgpn;
		isup_var_t rdgn;
		isup_var_t rdnn;
		isup_var_t conr;
		ulong inri;
		ulong infi;
		ulong coti;
		ulong bci;
		isup_var_t caus;
		ulong rdi;
		ulong cgi;
		isup_var_t rs;
		ulong cmi;
		ulong faci;
		ulong faii;
		isup_var_t cugi;
		ulong index;
		isup_var_t usi;
		ulong spc;
		isup_var_t uui;
		isup_var_t conn;
		ulong sris;
		isup_var_t tns;
		ulong evnt;
		isup_var_t csi;
		ulong acl;
		isup_var_t ocdn;
		isup_var_t obci;
		ulong uuind;
		ulong ispc;
		ulong gnot;
		ulong chi;
		ulong adi;
		isup_var_t nsf;
		isup_var_t usip;
		ulong prop;
		isup_var_t rops;
		isup_var_t sa;
		isup_var_t uti;
		ulong tmu;
		ulong cdi;
		ulong eci;
		ulong mci;
		ulong pci;
		ulong mlpp;
		ulong mciq;
		ulong mcir;
		ulong hopc;
		ulong tmrp;
		isup_var_t ln;
		ulong rdnr;
		ulong freep;
		isup_var_t gref;
		isup_var_t gnum;
		isup_var_t gdig;
		ulong egress;
		isup_var_t jur;
		isup_var_t cidc;
		isup_var_t bgroup;
		ulong noti;
		isup_var_t svact;
		isup_var_t trnsrq;
		isup_var_t spr;
		ulong cgci;
		ulong cvri;
		isup_var_t otgn;
		isup_var_t cin;
		isup_var_t clli;
		isup_var_t oli;
		isup_var_t chgn;
		isup_var_t svcd;
		isup_var_t csel;
		isup_var_t ori;
		isup_var_t iri;
		isup_var_t rate;
		isup_var_t iic;
		isup_var_t toi;
		ulong ton;
		ulong cri;
		isup_var_t icci;
	} oparm;
	uchar *data;			/* user data */
	size_t dlen;			/* user data length */
} isup_msg_t;

#define ISUP_MP_IAM	0UL	/* Initial address */
#define ISUP_MP_SAM	0UL	/* Subsequent address */
#define ISUP_MP_INR	1UL	/* Information request */
#define ISUP_MP_INF	1UL	/* Information */
#define ISUP_MP_COT	1UL	/* Continuity */
#define ISUP_MP_ACM	1UL	/* Address complete */
#define ISUP_MP_CON	0UL	/* Connect (not old ANSI) */
#define ISUP_MP_FOT	1UL	/* Forward transfer */
#define ISUP_MP_ANM	2UL	/* Answer */
#define ISUP_MP_REL	1UL	/* Release */
#define ISUP_MP_SUS	1UL	/* Suspend */
#define ISUP_MP_RES	1UL	/* Resume */
#define ISUP_MP_RLC	2UL	/* Release complete */
#define ISUP_MP_CCR	1UL	/* Continuity check request */
#define ISUP_MP_RSC	0UL	/* Reset circuit */
#define ISUP_MP_BLO	0UL	/* Blocking */
#define ISUP_MP_UBL	0UL	/* Unblcoking */
#define ISUP_MP_BLA	0UL	/* Blocking acknowledgement */
#define ISUP_MP_UBA	0UL	/* Unblocking acknowledgement */
#define ISUP_MP_GRS	0UL	/* Circuit group reset */
#define ISUP_MP_CGB	0UL	/* Circuit group blocking */
#define ISUP_MP_CGU	0UL	/* Circuit group unblocking */
#define ISUP_MP_CGBA	0UL	/* Circuit group blocking acknowledgement */
#define ISUP_MP_CGUA	0UL	/* Circuit group unblocking acknowledgement */
#define ISUP_MP_CMR	0UL	/* Call Modification Request (not old ANSI) */
#define ISUP_MP_CMC	0UL	/* Call Modification Completed (not old ANSI) */
#define ISUP_MP_CMRJ	0UL	/* Call Modification Reject (not old ANSI) */
#define ISUP_MP_FAR	0UL	/* Facility request */
#define ISUP_MP_FAA	0UL	/* Facility accepted */
#define ISUP_MP_FRJ	0UL	/* Facility reject */
#define ISUP_MP_FAD	0UL	/* Facility Deactivated (old Bellcore only) */
#define ISUP_MP_FAI	0UL	/* Facility Information (old Bellcore only) */
#define ISUP_MP_LPA	1UL	/* Loop back acknowledgement */
#define ISUP_MP_DRS	0UL	/* Delayed release (not old ANSI) */
#define ISUP_MP_PAM	0UL	/* Pass along */
#define ISUP_MP_GRA	0UL	/* Circuit group reset acknowledgement */
#define ISUP_MP_CQM	0UL	/* Circuit group query */
#define ISUP_MP_CQR	0UL	/* Circuit group query response */
#define ISUP_MP_CPG	1UL	/* Call progress */
#define ISUP_MP_USR	0UL	/* User-to-user information */
#define ISUP_MP_UCIC	1UL	/* Unequipped circuit identification code */
#define ISUP_MP_CFN	0UL	/* Confusion */
#define ISUP_MP_OLM	0UL	/* Overload */
#define ISUP_MP_CRG	0UL	/* Charge information */
#define ISUP_MP_NRM	0UL	/* Network resource management */
#define ISUP_MP_FAC	1UL	/* Facility */
#define ISUP_MP_UPT	0UL	/* User part test */
#define ISUP_MP_UPA	0UL	/* User part available */
#define ISUP_MP_IDR	0UL	/* Identification request */
#define ISUP_MP_IRS	0UL	/* Identification response */
#define ISUP_MP_SGM	0UL	/* Segmentation */
#define ISUP_MP_CRA	0UL	/* Circuit Reservation Ack (old Bellcore/ANSI 2000) */
#define ISUP_MP_CRM	0UL	/* Circuit Reservation (old Bellcore/ANSI 2000) */
#define ISUP_MP_CVR	0UL	/* Circuit Validation Response (old Bellcore/ANSI 2000) */
#define ISUP_MP_CVT	0UL	/* Circuit Validation Test (old Bellcore/ANSI 2000) */
#define ISUP_MP_EXM	0UL	/* Exit (old Bellcore/ANSI 2000) */
#define ISUP_MP_NON	0UL	/* National Notification (Spain) */
#define ISUP_MP_LLM	0UL	/* National Malicious Call (Spain) */
#define ISUP_MP_CAK	0UL	/* Charge Acknowledgement (Singapore) */
#define ISUP_MP_TCM	0UL	/* Tariff Charge (Singapore) */
#define ISUP_MP_MCP	0UL	/* Malicious Call Print (Singapore) */

#define ISUP_MT_IAM	  1UL	/* 0x01 - 0b00000001 - Initial address */
#define ISUP_MT_SAM	  2UL	/* 0x02 - 0b00000010 - Subsequent address */
#define ISUP_MT_INR	  3UL	/* 0x03 - 0b00000011 - Information request */
#define ISUP_MT_INF	  4UL	/* 0x04 - 0b00000100 - Information */
#define ISUP_MT_COT	  5UL	/* 0x05 - 0b00000101 - Continuity */
#define ISUP_MT_ACM	  6UL	/* 0x06 - 0b00000110 - Address complete */
#define ISUP_MT_CON	  7UL	/* 0x07 - 0b00000111 - Connect (not old ANSI) */
#define ISUP_MT_FOT	  8UL	/* 0x08 - 0b00001000 - Forward transfer */
#define ISUP_MT_ANM	  9UL	/* 0x09 - 0b00001001 - Answer */
#define ISUP_MT_REL	 12UL	/* 0x0c - 0b00001100 - Release */
#define ISUP_MT_SUS	 13UL	/* 0x0d - 0b00001101 - Suspend */
#define ISUP_MT_RES	 14UL	/* 0x0e - 0b00001110 - Resume */
#define ISUP_MT_RLC	 16UL	/* 0x10 - 0b00010000 - Release complete */
#define ISUP_MT_CCR	 17UL	/* 0x11 - 0b00010001 - Continuity check request */
#define ISUP_MT_RSC	 18UL	/* 0x12 - 0b00010010 - Reset circuit */
#define ISUP_MT_BLO	 19UL	/* 0x13 - 0b00010011 - Blocking */
#define ISUP_MT_UBL	 20UL	/* 0x14 - 0b00010100 - Unblcoking */
#define ISUP_MT_BLA	 21UL	/* 0x15 - 0b00010101 - Blocking acknowledgement */
#define ISUP_MT_UBA	 22UL	/* 0x16 - 0b00010110 - Unblocking acknowledgement */
#define ISUP_MT_GRS	 23UL	/* 0x17 - 0b00010111 - Circuit group reset */
#define ISUP_MT_CGB	 24UL	/* 0x18 - 0b00011000 - Circuit group blocking */
#define ISUP_MT_CGU	 25UL	/* 0x19 - 0b00011001 - Circuit group unblocking */
#define ISUP_MT_CGBA	 26UL	/* 0x1a - 0b00011010 - Circuit group blocking acknowledgement */
#define ISUP_MT_CGUA	 27UL	/* 0x1b - 0b00011011 - Circuit group unblocking acknowledgement */
#define ISUP_MT_CMR	 28UL	/* 0x1c - 0b00011100 - Call Modification Request (not old ANSI) */
#define ISUP_MT_CMC	 29UL	/* 0x1d - 0b00011101 - Call Modification Completed (not old ANSI) */
#define ISUP_MT_CMRJ	 30UL	/* 0x1e - 0b00011110 - Call Modification Reject (not old ANSI) */
#define ISUP_MT_FAR	 31UL	/* 0x1f - 0b00011111 - Facility request */
#define ISUP_MT_FAA	 32UL	/* 0x20 - 0b00100000 - Facility accepted */
#define ISUP_MT_FRJ	 33UL	/* 0x21 - 0b00100001 - Facility reject */
#define ISUP_MT_FAD	 34UL	/* 0x22 - 0b00100010 - Facility Deactivated (old Bellcore only) */
#define ISUP_MT_FAI	 35UL	/* 0x23 - 0b00100011 - Facility Information (old Bellcore only) */
#define ISUP_MT_LPA	 36UL	/* 0x24 - 0b00100100 - Loop back acknowledgement */
#define ISUP_MT_DRS	 39UL	/* 0x27 - 0b00100111 - Delayed release (not old ANSI) */
#define ISUP_MT_PAM	 40UL	/* 0x28 - 0b00101000 - Pass along */
#define ISUP_MT_GRA	 41UL	/* 0x29 - 0b00101001 - Circuit group reset acknowledgement */
#define ISUP_MT_CQM	 42UL	/* 0x2a - 0b00101010 - Circuit group query */
#define ISUP_MT_CQR	 43UL	/* 0x2b - 0b00101011 - Circuit group query response */
#define ISUP_MT_CPG	 44UL	/* 0x2c - 0b00101100 - Call progress */
#define ISUP_MT_USR	 45UL	/* 0x2d - 0b00101101 - User-to-user information */
#define ISUP_MT_UCIC	 46UL	/* 0x2e - 0b00101110 - Unequipped circuit identification code */
#define ISUP_MT_CFN	 47UL	/* 0x2f - 0b00101111 - Confusion */
#define ISUP_MT_OLM	 48UL	/* 0x30 - 0b00110000 - Overload */
#define ISUP_MT_CRG	 49UL	/* 0x31 - 0b00110001 - Charge information */
#define ISUP_MT_NRM	 50UL	/* 0x32 - 0b00110010 - Network resource management */
#define ISUP_MT_FAC	 51UL	/* 0x33 - 0b00110011 - Facility */
#define ISUP_MT_UPT	 52UL	/* 0x34 - 0b00110100 - User part test */
#define ISUP_MT_UPA	 53UL	/* 0x35 - 0b00110101 - User part available */
#define ISUP_MT_IDR	 54UL	/* 0x36 - 0b00110110 - Identification request */
#define ISUP_MT_IRS	 55UL	/* 0x37 - 0b00110111 - Identification response */
#define ISUP_MT_SGM	 56UL	/* 0x38 - 0b00111000 - Segmentation */
#define ISUP_MT_CRA	233UL	/* 0xe9 - 0b11101001 - Circuit Reservation Ack (old Bellcore/ANSI
				   2000) */
#define ISUP_MT_CRM	234UL	/* 0xea - 0b11101010 - Circuit Reservation (old Bellcore/ANSI 2000) 
				 */
#define ISUP_MT_CVR	235UL	/* 0xeb - 0b11101011 - Circuit Validation Response (old
				   Bellcore/ANSI 2000) */
#define ISUP_MT_CVT	236UL	/* 0xec - 0b11101100 - Circuit Validation Test (old Bellcore/ANSI
				   2000) */
#define ISUP_MT_EXM	237UL	/* 0xed - 0b11101101 - Exit (old Bellcore/ANSI 2000) */
#define ISUP_MT_NON	248UL	/* 0xf8 - 0b11111000 - National Notification (Spain) */
#define ISUP_MT_LLM	252UL	/* 0xfc - 0b11111100 - National Malicious Call (Spain) */
#define ISUP_MT_CAK	253UL	/* 0xfd - 0b11111101 - Charge Acknowledgement (Singapore) */
#define ISUP_MT_TCM	254UL	/* 0xfe - 0b11111110 - Tariff Charge (Singapore) */
#define ISUP_MT_MCP	255UL	/* 0xff - 0b11111111 - Malicious Call Print (Singapore) */

#define ISUP_PT_EOP	  0UL	/* 0x00 - 0b00000000 - End of optional parameters */
#define ISUP_PT_CLRF	  1UL	/* 0x01 - 0b00000001 - Call reference @ */
#define ISUP_PT_TMR	  2UL	/* 0x02 - 0b00000010 - Transmission medium requirement */
#define ISUP_PT_ATP	  3UL	/* 0x03 - 0b00000011 - Access transport */
#define ISUP_PT_CDPN	  4UL	/* 0x04 - 0b00000100 - Called party number */
#define ISUP_PT_SUBN	  5UL	/* 0x05 - 0b00000101 - Subsequent number */
#define ISUP_PT_NCI	  6UL	/* 0x06 - 0b00000110 - Nature of connection indicators */
#define ISUP_PT_FCI	  7UL	/* 0x07 - 0b00000111 - Forward call indicators */
#define ISUP_PT_OFCI	  8UL	/* 0x08 - 0b00001000 - Optional forward call indicators */
#define ISUP_PT_CPC	  9UL	/* 0x09 - 0b00001001 - Calling party's category */
#define ISUP_PT_CGPN	 10UL	/* 0x0a - 0b00001010 - Calling party number */
#define ISUP_PT_RDGN	 11UL	/* 0x0b - 0b00001011 - Redirecting number */
#define ISUP_PT_RDNN	 12UL	/* 0x0c - 0b00001100 - Redirection number */
#define ISUP_PT_CONR	 13UL	/* 0x0d - 0b00001101 - Connection request */
#define ISUP_PT_INRI	 14UL	/* 0x0e - 0b00001110 - Information request indicators */
#define ISUP_PT_INFI	 15UL	/* 0x0f - 0b00001111 - Information indicators @ */
#define ISUP_PT_COTI	 16UL	/* 0x10 - 0b00010000 - Continuity indicators */
#define ISUP_PT_BCI	 17UL	/* 0x11 - 0b00010001 - Backward call indicators */
#define ISUP_PT_CAUS	 18UL	/* 0x12 - 0b00010010 - Cause indicators */
#define ISUP_PT_RDI	 19UL	/* 0x13 - 0b00010011 - Redirection information */
#define ISUP_PT_CGI	 21UL	/* 0x15 - 0b00010101 - Circuit group supervision msg type ind */
#define ISUP_PT_RS	 22UL	/* 0x16 - 0b00010110 - Range and status */
#define ISUP_PT_CMI	 23UL	/* 0x17 - 0b00010111 - Call modification indicators (Blue Book) */
#define ISUP_PT_FACI	 24UL	/* 0x18 - 0b00011000 - Facility indicator */
#define ISUP_PT_FAII	 25UL	/* 0x19 - 0b00011001 - Facility information indicators (Bellcore) */
#define ISUP_PT_CUGI	 26UL	/* 0x1a - 0b00011010 - Closed user group interlock code */
#define ISUP_PT_INDEX	 27UL	/* 0x1b - 0b00011011 - Index (Bellcore) */
#define ISUP_PT_USI	 29UL	/* 0x1d - 0b00011101 - User service information */
#define ISUP_PT_SPC	 30UL	/* 0x1e - 0b00011110 - Signalling point code @ (Bellcore) */
#define ISUP_PT_UUI	 32UL	/* 0x20 - 0b00100000 - User to user information */
#define ISUP_PT_CONN	 33UL	/* 0x21 - 0b00100001 - Connected number */
#define ISUP_PT_SRIS	 34UL	/* 0x22 - 0b00100010 - Suspend/resume indicators */
#define ISUP_PT_TNS	 35UL	/* 0x23 - 0b00100011 - Transit network selection @ */
#define ISUP_PT_EVNT	 36UL	/* 0x24 - 0b00100100 - Event information */
#define ISUP_PT_CSI	 38UL	/* 0x26 - 0b00100110 - Circuit state indicator @ */
#define ISUP_PT_ACL	 39UL	/* 0x27 - 0b00100111 - Automatic congestion level */
#define ISUP_PT_OCDN	 40UL	/* 0x28 - 0b00101000 - Original called number */
#define ISUP_PT_OBCI	 41UL	/* 0x29 - 0b00101001 - Optional backward call indicators */
#define ISUP_PT_UUIND	 42UL	/* 0x2a - 0b00101010 - User to user indicators */
#define ISUP_PT_ISPC	 43UL	/* 0x2b - 0b00101011 - Origination ISC point code */
#define ISUP_PT_GNOT	 44UL	/* 0x2c - 0b00101100 - Generic notification */
#define ISUP_PT_CHI	 45UL	/* 0x2d - 0b00101101 - Call history information */
#define ISUP_PT_ADI	 46UL	/* 0x2e - 0b00101110 - Access delivery information */
#define ISUP_PT_NSF	 47UL	/* 0x2f - 0b00101111 - Network specific facilities @ */
#define ISUP_PT_USIP	 48UL	/* 0x30 - 0b00110000 - User service information prime */
#define ISUP_PT_PROP	 49UL	/* 0x31 - 0b00110001 - Propagation delay counter */
#define ISUP_PT_ROPS	 50UL	/* 0x32 - 0b00110010 - Remote operations @ */
#define ISUP_PT_SA	 51UL	/* 0x33 - 0b00110011 - Service activation @ */
#define ISUP_PT_UTI	 52UL	/* 0x34 - 0b00110100 - User teleservice information */
#define ISUP_PT_TMU	 53UL	/* 0x35 - 0b00110101 - Transmission medium used */
#define ISUP_PT_CDI	 54UL	/* 0x36 - 0b00110110 - Call diversion information */
#define ISUP_PT_ECI	 55UL	/* 0x37 - 0b00110111 - Echo control information */
#define ISUP_PT_MCI	 56UL	/* 0x38 - 0b00111000 - Message compatibility information */
#define ISUP_PT_PCI	 57UL	/* 0x39 - 0b00111001 - Parameter compatibility information */
#define ISUP_PT_MLPP	 58UL	/* 0x3a - 0b00111010 - MLPP preference */
#define ISUP_PT_MCIQ	 59UL	/* 0x3b - 0b00111011 - MCID request indicator */
#define ISUP_PT_MCIR	 60UL	/* 0x3c - 0b00111100 - MCID response indicator */
#define ISUP_PT_HOPC	 61UL	/* 0x3d - 0b00111101 - Hop counter (reserved) */
#define ISUP_PT_TMRP	 62UL	/* 0x3e - 0b00111110 - Transmission medium requirement prime */
#define ISUP_PT_LN	 63UL	/* 0x3f - 0b00111111 - Location number */
#define ISUP_PT_RDNR	 64UL	/* 0x40 - 0b01000000 - Redirection number restriction */
#define ISUP_PT_FREEP	 65UL	/* 0x41 - 0b01000001 - Freephone indicators (reserved) */
#define ISUP_PT_GREF	 66UL	/* 0x42 - 0b01000010 - Generic reference (reserved) */
#define ISUP_PT_GNUM	192UL	/* 0xc0 - 0b11000000 - Generic number (address Bellcore) */
#define ISUP_PT_GDIG	193UL	/* 0xc1 - 0b11000001 - Generic digits @ */
#define ISUP_PT_EGRESS	195UL	/* 0xc3 - 0b11000011 - Egress (ANSI) */
#define ISUP_PT_JUR	196UL	/* 0xc4 - 0b11000100 - Jurisdiction (ANSI) */
#define ISUP_PT_CIDC	197UL	/* 0xc5 - 0b11000101 - Carrier identification code (ANSI) */
#define ISUP_PT_BGROUP	198UL	/* 0xc6 - 0b11000110 - Business group (ANSI) */
#define ISUP_PT_NOTI	225UL	/* 0xe1 - 0b11100001 - Notification indicator (ANSI) */
#define ISUP_PT_SVACT	226UL	/* 0xe2 - 0b11100010 - Service activation (ANSI) */
#define ISUP_PT_TRNSRQ	227UL	/* 0xe3 - 0b11100011 - Transaction request (ANSI, Bellcore) */
#define ISUP_PT_SPR	228UL	/* 0xe4 - 0b11100100 - Special processing request (Bellcore) */
#define ISUP_PT_CGCI	229UL	/* 0xe5 - 0b11100101 - Cc't group char ind (ANSI, Bellcore) */
#define ISUP_PT_CVRI	230UL	/* 0xe6 - 0b11100110 - Cc't validation resp ind (ANSI, Bellcore) */
#define ISUP_PT_OTGN	231UL	/* 0xe7 - 0b11100111 - Outgoing trunk group numb (ANSI, Bellcore) */
#define ISUP_PT_CIN	232UL	/* 0xe8 - 0b11101000 - Circuit ident name (ANSI, Bellcore) */
#define ISUP_PT_CLLI	233UL	/* 0xe9 - 0b11101001 - Common language loc id (ANSI, Bellcore) */
#define ISUP_PT_OLI	234UL	/* 0xea - 0b11101010 - Originating line info (ANSI, Bellcore) */
#define ISUP_PT_CHGN	235UL	/* 0xeb - 0b11101011 - Charge number (ANSI, Bellcore) */
#define ISUP_PT_SVCD	236UL	/* 0xec - 0b11101100 - Service code indicator (ANSI, Bellcore) */
#define ISUP_PT_CSEL	238UL	/* 0xee - 0b11101110 - Carrier selection info (ANSI, Bellcore) */
#define ISUP_PT_ORI	243UL	/* 0xf3 - 0b11110011 - Outgoing route identification (Spain) */
#define ISUP_PT_IRI	244UL	/* 0xf4 - 0b11110100 - Incoming route identification (Spain) */
#define ISUP_PT_RATE	248UL	/* 0xf8 - 0b11111000 - Rate (Spain) */
#define ISUP_PT_IIC	249UL	/* 0xf9 - 0b11111001 - Identifier of incoming circuit (Spain) */
#define ISUP_PT_TOI	253UL	/* 0xfd - 0b11111101 - Trunk offering information (Singapore) */
#define ISUP_PT_TON	253UL	/* 0xfd - 0b11111101 - Type of notification (Spain) */
#define ISUP_PT_CRI	254UL	/* 0xfe - 0b11111110 - Charge rate information (Singapore) */
#define ISUP_PT_ICCI	255UL	/* 0xff - 0b11111111 - Call charge information (Singapore) */

#define ISUP_MCI_A_BIT		0x00000001UL
#define ISUP_MCI_B_BIT		0x00000002UL
#define ISUP_MCI_C_BIT		0x00000004UL
#define ISUP_MCI_D_BIT		0x00000008UL
#define ISUP_MCI_E_BIT		0x00000010UL
#define ISUP_MCI_F_BIT		0x00000020UL
#define ISUP_MCI_G_BIT		0x00000040UL
#define ISUP_MCI_H_BIT		0x00000080UL

#define ISUP_PCI_A_BIT		0x00000001UL
#define ISUP_PCI_B_BIT		0x00000002UL
#define ISUP_PCI_C_BIT		0x00000004UL
#define ISUP_PCI_D_BIT		0x00000008UL
#define ISUP_PCI_E_BIT		0x00000010UL
#define ISUP_PCI_F_BIT		0x00000020UL
#define ISUP_PCI_G_BIT		0x00000040UL
#define ISUP_PCI_H_BIT		0x00000080UL

/*
 *  =========================================================================
 *
 *  PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent upstream
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_FLUSH
 *  -----------------------------------
 */
STATIC int
m_flush(queue_t *q, queue_t *pq, int band, int flags, int what)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_FLUSH;
		*(mp->b_wptr)++ = flags | (band ? FLUSHBAND : 0);
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
STATIC int
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
			cs_set_state(cc, CCS_UNUSABLE);
			printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (-error);
		} else {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = error < 0 ? -error : error;
			*(mp->b_wptr)++ = error < 0 ? -error : error;
			cs_set_state(cc, CCS_UNUSABLE);
			printd(("%s: %p: <- M_ERROR\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
cc_ok_ack(queue_t *q, struct cc *cc, struct ct *ct, long prim)
{
	mblk_t *mp;
	struct CC_ok_ack *p;
	ensure(cc->oq, return (QR_DONE));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_OK_ACK;
		p->cc_correct_prim = prim;
		p->cc_state = cc->state;
		p->cc_call_ref = ct ? (ct->cref ? : ct->uref) : 0;
		printd(("%s: %p: <- CC_OK_ACK\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
cc_error_ack(queue_t *q, struct cc *cc, struct ct *ct, long prim, long error)
{
	mblk_t *mp;
	struct CC_error_ack *p;
	ensure(cc->oq, return (QR_DONE));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_ERROR_ACK;
		p->cc_error_primitive = prim;
		p->cc_error_type = error < 0 ? CCSYSERR : error;
		p->cc_unix_error = error < 0 ? -error : 0;
		p->cc_state = cc->state;
		p->cc_call_ref = ct ? (ct->cref ? : ct->uref) : 0;
		printd(("%s: %p: <- CC_ERROR_ACK\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
cc_info_ack(queue_t *q, struct cc *cc)
{
	mblk_t *mp;
	struct CC_info_ack *p;
	ensure(cc->oq, return (QR_DONE));
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_INFO_ACK;
		fixme(("Fill out more the message\n"));
		printd(("%s: %p: <- CC_INFO_ACK\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_BIND_ACK
 *  -----------------------------------
 */
STATIC INLINE int
cc_bind_ack(queue_t *q, struct cc *cc, uchar *add_ptr, size_t add_len, ulong setup_ind,
	    ulong token_value)
{
	mblk_t *mp;
	struct CC_bind_ack *p;
	ensure(cc->oq, return (QR_DONE));
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
 *  -----------------------------------
 */
STATIC INLINE int
cc_optmgmt_ack(queue_t *q, struct cc *cc, uchar *opt_ptr, size_t opt_len, ulong flags, ulong cref)
{
	mblk_t *mp;
	struct CC_optmgmt_ack *p;
	ensure(cc->oq, return (QR_DONE));
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_OPTMGMT_ACK;
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
cc_addr_ack(queue_t *q, struct cc *cc, uchar *bind_ptr, size_t bind_len, ulong cref,
	    uchar *conn_ptr, size_t conn_len)
{
	mblk_t *mp;
	struct CC_addr_ack *p;
	ensure(cc->oq, return (QR_DONE));
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
 *  CC_CALL_REATTEMPT_IND
 *  -----------------------------------
 *  This is an indication to call control to reattempt on another circuit.  Current reasons for reattempt are as
 *  follows:
 *      
 *      ISUP_REATTEMPT_DUAL_SIEZURE
 *      ISUP_REATTEMPT_RESET
 *      ISUP_REATTEMPT_BLOCKING
 *      ISUP_REATTEMPT_T24_TIMEOUT      (waiting to detect tone)
 *      ISUP_REATTEMPT_UNEXPECTED       (unexpected message)
 *      ISUP_REATTEMPT_COT_FAILURE
 */
STATIC INLINE int
cc_call_reattempt_ind(queue_t *q, struct cc *cc, struct ct *ct, ulong reason)
{
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_call_reattempt_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_CALL_REATTEMPT_IND;
			p->cc_user_ref = ct->uref;
			p->cc_reason = reason;
			printd(("%s: %p: <- CC_CALL_REATTEMPT_IND\n", DRV_NAME, cc));
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
 *  CC_SETUP_IND
 *  -----------------------------------
 *  Derived directly from an IAM.
 */
STATIC INLINE int
cc_setup_ind(queue_t *q, struct cc *cc, struct ct *ct, isup_msg_t * m)
{
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_setup_ind *p;
		struct isup_addr *a;
		size_t cdpn_len = (m->msg.iam.cdpn.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1);
		size_t opt_len = (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1);
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + cdpn_len + opt_len + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SETUP_IND;
			p->cc_call_ref = ct->cref;
			p->cc_call_flags =
			    (((ulong) m->msg.iam.nci & 0xff) << 0) | (((ulong) m->msg.iam.
								       fci & 0xffff) << 8) |
			    (((ulong) m->msg.iam.cpc & 0xff) << 24);
			p->cc_call_type = (ulong) m->msg.iam.tmr & 0xff;
			p->cc_cdpn_length = m->msg.iam.cdpn.len;
			p->cc_cdpn_offset = cdpn_len ? sizeof(*p) : 0;
			if (cdpn_len) {
				bcopy(m->msg.iam.cdpn.ptr, mp->b_wptr, m->msg.iam.cdpn.len);
				mp->b_wptr += cdpn_len;
			}
			p->cc_opt_length = m->opt.len;
			p->cc_opt_offset = opt_len ? sizeof(*p) + cdpn_len : 0;
			if (opt_len) {
				bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
				mp->b_wptr += opt_len;
			}
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) + cdpn_len + opt_len : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_CT;
			a->id = ct->id;
			a->cic = ct->cic;
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
 *  CC_MORE_INFO_IND
 *  -----------------------------------
 *  Derived directly from a timeout.
 */
STATIC INLINE int
cc_more_info_ind(queue_t *q, struct cc *cc, struct ct *ct)
{
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_more_info_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_MORE_INFO_IND;
			p->cc_user_ref = ct->uref;
			p->cc_opt_length = 0;
			p->cc_opt_offset = 0;
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
 *  -----------------------------------
 *  Derived directly from a SAM.
 */
STATIC INLINE int
cc_information_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_information_ind *p;
		size_t subn_len = (m->msg.sam.subn.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1);
		size_t opt_len = (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1);
		if ((mp = ss7_allocb(q, sizeof(*p) + subn_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_INFORMATION_IND;
			p->cc_call_ref = ct->cref;
			p->cc_subn_length = m->msg.sam.subn.len;
			p->cc_subn_offset = subn_len ? sizeof(*p) : 0;
			if (subn_len) {
				bcopy(m->msg.sam.subn.ptr, mp->b_wptr, m->msg.sam.subn.len);
				mp->b_wptr += subn_len;
			}
			p->cc_opt_length = m->opt.len;
			p->cc_opt_offset = opt_len ? sizeof(*p) + subn_len : 0;
			if (opt_len) {
				bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
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
 *  -----------------------------------
 *  Derived directly from timeout.
 */
STATIC INLINE int
cc_info_timeout_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_info_timeout_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_INFO_TIMEOUT_IND;
			p->cc_call_ref = ct->cref;
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
 *  CC_CONT_CHECK_IND
 *  -----------------------------------
 *  Derived directly from a CCR.
 */
STATIC INLINE int
cc_cont_check_ind(queue_t *q, struct cc *cc, struct ct *ct, isup_msg_t * m)
{
	ensure(cc->oq, return (QR_DONE));
	printd(("%s: MAINT: continuity check request on circuit id=%ld, cic=%ld\n", DRV_NAME,
		ct->id, ct->cic));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_cont_check_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_CONT_CHECK_IND;
			p->cc_call_ref = ct->cref;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_CT;
			a->id = ct->id;
			a->cic = ct->cic;
			printd(("%s: %p: <- CC_CONT_CHECK_IND\n", DRV_NAME, cc));
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
 *  CC_CONT_TEST_IND
 *  -----------------------------------
 *  Derived directly from LPA or synthesized after CC_CONT_CHECK_REQ received.
 */
STATIC INLINE int
cc_cont_test_ind(queue_t *q, struct cc *cc, struct ct *ct)
{
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_cont_test_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_CONT_TEST_IND;
			p->cc_call_ref = ct->cref;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_CT;
			a->id = ct->id;
			a->cic = ct->cic;
			printd(("%s: %p: <- CC_CONT_TEST_IND\n", DRV_NAME, cc));
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
 *  CC_CONT_REPORT_IND
 *  -----------------------------------
 *  Derived directly from receipt of COT or REL or T24 timeout.  Reports success or failure of continuity tests.
 *  Flags is a logical or of zero or more flags: CC_OGC_COT and CC_ICC_COT.
 */
STATIC INLINE int
cc_cont_report_ind(queue_t *q, struct cc *cc, struct ct *ct, isup_msg_t * m)
{
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_cont_report_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_CONT_REPORT_IND;
			p->cc_call_ref = ct->cref;
			switch (m ? m->mt : 0) {
			case ISUP_MT_COT:
				p->cc_result = m->msg.cot.coti;
				break;
			case ISUP_MT_REL:
				p->cc_result = ISUP_COT_SUCCESS;
				break;
			default:
				swerr();
			case 0:
				p->cc_result = ISUP_COT_FAILURE;
				break;
			}
			printd(("%s: %p: <- CC_CONT_REPORT_IND\n", DRV_NAME, cc));
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
 *  -----------------------------------
 *  Synthesized on first received backwards message.
 */
STATIC INLINE int
cc_setup_con(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_setup_con *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SETUP_CON;
			p->cc_user_ref = ct->uref;
			p->cc_call_ref = ct->cref;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_CT;
			a->id = ct->id;
			a->cic = ct->cic;
			printd(("%s: %p: <- CC_SETUP_CON\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			cc_put_uref(ct, cc);
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
 *  -----------------------------------
 *  Directly from ACM or CPG or T11 timeout.
 */
STATIC INLINE int
cc_proceeding_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_proceeding_ind *p;
		size_t opt_len = m ? (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1) : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_PROCEEDING_IND;
			p->cc_call_ref = ct->cref;
			if (m) {
				switch (m->mt) {
				case ISUP_MT_ACM:
					p->cc_flags = m->msg.acm.bci;
					break;
				case ISUP_MT_CPG:
					p->cc_flags = 0;
					break;
				default:
					swerr();
					p->cc_flags = 0;
					break;
				}
				p->cc_opt_length = m->opt.len;
				p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
				if (opt_len) {
					bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
					mp->b_wptr += opt_len;
				}
			} else {
				p->cc_flags = 0;
				p->cc_opt_length = 0;
				p->cc_opt_offset = 0;
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
 *  -----------------------------------
 *  From ACM or CPG.
 */
STATIC INLINE int
cc_alerting_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_alerting_ind *p;
		size_t opt_len = m ? (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1) : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_ALERTING_IND;
			p->cc_call_ref = ct->cref;
			if (m) {
				switch (m->mt) {
				case ISUP_MT_ACM:
					p->cc_flags = m->msg.acm.bci;
					break;
				case ISUP_MT_CPG:
					p->cc_flags = 0;
					break;
				default:
					swerr();
					p->cc_flags = 0;
					break;
				}
				p->cc_opt_length = m->opt.len;
				p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
				if (opt_len) {
					bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
					mp->b_wptr += opt_len;
				}
			} else {
				p->cc_flags = 0;
				p->cc_opt_length = 0;
				p->cc_opt_offset = 0;
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
 *  -----------------------------------
 *  From ACM or CPG.
 */
STATIC INLINE int
cc_progress_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_progress_ind *p;
		size_t opt_len = m ? (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1) : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_PROGRESS_IND;
			p->cc_call_ref = ct->cref;
			if (m) {
				switch (m->mt) {
				case ISUP_MT_ACM:
					p->cc_flags = m->msg.acm.bci;
					p->cc_event = ISUP_EVNT_PRES_RESTRICT | ISUP_EVNT_PROGRESS;
					break;
				case ISUP_MT_CPG:
					p->cc_flags = 0;
					p->cc_event = m->msg.cpg.evnt;
					break;
				default:
					swerr();
					p->cc_flags = 0;
					break;
				}
				p->cc_opt_length = m->opt.len;
				p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
				if (opt_len) {
					bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
					mp->b_wptr += opt_len;
				}
			} else {
				p->cc_flags = 0;
				p->cc_opt_length = 0;
				p->cc_opt_offset = 0;
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
 *  CC_IBI_IND
 *  -----------------------------------
 *  From ACM or CPG.
 */
STATIC INLINE int
cc_ibi_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_ibi_ind *p;
		size_t opt_len = m ? (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1) : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_IBI_IND;
			p->cc_call_ref = ct->cref;
			if (m) {
				switch (m->mt) {
				case ISUP_MT_ACM:
					p->cc_flags = m->msg.acm.bci;
					break;
				case ISUP_MT_CPG:
					p->cc_flags = 0;
					break;
				default:
					swerr();
					p->cc_flags = 0;
					break;
				}
				p->cc_opt_length = m->opt.len;
				p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
				if (opt_len) {
					bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
					mp->b_wptr += opt_len;
				}
			} else {
				p->cc_flags = 0;
				p->cc_opt_length = 0;
				p->cc_opt_offset = 0;
			}
			printd(("%s: %p: <- CC_IBI_IND\n", DRV_NAME, cc));
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
 *  -----------------------------------
 *  From ANM or CON.
 */
STATIC INLINE int
cc_connect_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_connect_ind *p;
		size_t opt_len = m ? (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1) : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_CONNECT_IND;
			p->cc_call_ref = ct->cref;
			if (m) {
				switch (m->mt) {
				case ISUP_MT_ANM:
					p->cc_flags = 0;
					break;
				case ISUP_MT_CON:
					p->cc_flags = m->msg.con.bci;
					break;
				default:
					swerr();
					p->cc_flags = 0;
					break;
				}
				p->cc_opt_length = m->opt.len;
				p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
				if (opt_len) {
					bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
					mp->b_wptr += opt_len;
				}
			} else {
				p->cc_flags = 0;
				p->cc_opt_length = 0;
				p->cc_opt_offset = 0;
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
 *  -----------------------------------
 *  Synthesized for ISDN compatibility.
 */
STATIC INLINE int
cc_setup_complete_ind(queue_t *q, struct ct *ct)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_setup_complete_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SETUP_COMPLETE_IND;
			p->cc_call_ref = ct->cref;
			p->cc_opt_length = 0;
			p->cc_opt_offset = 0;
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
 *  CC_FORWXFER_IND
 *  -----------------------------------
 *  From FOT.
 */
STATIC INLINE int
cc_forwxfer_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_forwxfer_ind *p;
		size_t opt_len = (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1);
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_FORWXFER_IND;
			p->cc_call_ref = ct->cref;
			p->cc_opt_length = m->opt.len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_FORWXFER_IND\n", DRV_NAME, cc));
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
 *  -----------------------------------
 *  From SUS.
 */
STATIC INLINE int
cc_suspend_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_suspend_ind *p;
		size_t opt_len = (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1);
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_SUSPEND_IND;
			p->cc_flags = m->msg.sus.sris;
			p->cc_call_ref = ct->cref;
			p->cc_opt_length = m->opt.len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
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
 *  CC_RESUME_IND
 *  -----------------------------------
 *  Directly from RES.
 */
STATIC INLINE int
cc_resume_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cpc.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_resume_ind *p;
		size_t opt_len = (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1);
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RESUME_IND;
			p->cc_call_ref = ct->cref;
			p->cc_flags = m->msg.res.sris;
			p->cc_opt_length = m->opt.len;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
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
 *  CC_CALL_FAILURE_IND
 *  -----------------------------------
 */
STATIC INLINE int
cc_call_failure_ind(queue_t *q, struct cc *cc, struct ct *ct, ulong reason, ulong cause)
{
	int err;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (!(err = m_flush(q, cc->oq, 0, FLUSHR | FLUSHW, FLUSHDATA))) {
		mblk_t *mp;
		struct CC_call_failure_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_CALL_FAILURE_IND;
			p->cc_call_ref = ct->cref;
			p->cc_reason = reason;
			p->cc_cause = cause;
			printd(("%s: %p: <- CC_CALL_FAILURE_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	return (err);
}

/*
 *  CC_RELEASE_IND
 *  -----------------------------------
 *  Directly from REL.
 */
STATIC INLINE int
cc_release_ind(queue_t *q, struct cc *cc, struct ct *ct, isup_msg_t * m)
{
	int err;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (!(err = m_flush(q, cc->oq, 0, FLUSHR, FLUSHDATA))) {
		mblk_t *mp;
		struct CC_release_ind *p;
		size_t opt_len = m ? (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1) : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RELEASE_IND;
			p->cc_user_ref = ct->uref;
			p->cc_call_ref = ct->cref;
			p->cc_cause = m ? m->msg.rel.caus.val : 0;
			p->cc_opt_length = m ? m->opt.len : 0;
			p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
			if (m && opt_len) {
				bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- CC_RELEASE_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	return (err);
}

/*
 *  CC_RELEASE_CON
 *  -----------------------------------
 *  Directly from RLC, RSC, timeout or request.
 */
STATIC INLINE int
cc_release_con(queue_t *q, struct cc *cc, struct ct *ct, isup_msg_t * m)
{
	mblk_t *mp;
	struct CC_release_con *p;
	size_t opt_len = m ? (m->opt.len + (sizeof(ulong) - 1)) & (sizeof(ulong) - 1) : 0;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->cc_primitive = CC_RELEASE_CON;
		p->cc_user_ref = ct->uref;
		p->cc_call_ref = ct->cref;
		p->cc_opt_length = m ? m->opt.len : 0;
		p->cc_opt_offset = opt_len ? sizeof(*p) : 0;
		if (m && opt_len) {
			bcopy(m->opt.ptr, mp->b_wptr, m->opt.len);
			mp->b_wptr += opt_len;
		}
		printd(("%s: %p: <- CC_RELEASE_CON\n", DRV_NAME, cc));
		ss7_oput(cc->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CC_RESET_IND
 *  -----------------------------------
 *  Derived from RSC or GRS.
 */
STATIC INLINE int
cc_reset_ind(queue_t *q, struct cc *cc, struct ct *ct, isup_msg_t * m)
{
	ensure(cc, return QR_DONE);
	ensure(cc->oq, return (QR_DONE));
	printd(("%s: MAINT: reset indication on circuit id=%ld, cic=%ld\n", DRV_NAME, ct->id,
		ct->cic));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_reset_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RESET_IND;
			p->cc_flags = (m->mt == ISUP_MT_GRS) ? ISUP_GROUP : 0;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			if (p->cc_flags & ISUP_GROUP) {
				a->scope = ISUP_SCOPE_CG;
				a->id = ct->cg.cg->id;
				a->cic = ct->cg.cg->cic;
			} else {
				a->scope = ISUP_SCOPE_CT;
				a->id = ct->id;
				a->cic = ct->cic;
			}
			printd(("%s: %p: <- CC_RESET_IND\n", DRV_NAME, cc));
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
 *  CC_RESET_CON
 *  -----------------------------------
 *  From received RLC or BLO or GRA.
 */
STATIC INLINE int
cc_reset_con(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = (m->mt == ISUP_MT_GRA) ? ct->cg.cg->gmg.cc : ct->mgm.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_reset_con *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_RESET_CON;
			p->cc_flags = (m->mt == ISUP_MT_GRA) ? ISUP_GROUP : 0;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			if (p->cc_flags & ISUP_GROUP) {
				a->scope = ISUP_SCOPE_CG;
				a->id = ct->cg.cg->id;
				a->cic = ct->cg.cg->cic;
			} else {
				a->scope = ISUP_SCOPE_CT;
				a->id = ct->id;
				a->cic = ct->cic;
			}
			printd(("%s: %p: <- CC_RESET_CON\n", DRV_NAME, cc));
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
 *  CC_BLOCKING_IND
 *  -----------------------------------
 *  From received BLO or CGB.
 */
STATIC INLINE int
cc_blocking_ind(queue_t *q, struct cc *cc, struct ct *ct, isup_msg_t * m)
{
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	printd(("%s: MAINT: blocking indication on circuit id=%ld, cic=%ld\n", DRV_NAME,
		ct->id, ct->cic));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_blocking_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_BLOCKING_IND;
			p->cc_flags =
			    (m->mt ==
			     ISUP_MT_CGB) ? ISUP_GROUP | m->msg.cgb.cgi : ISUP_MAINTENANCE_ORIENTED;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			if (p->cc_flags & ISUP_GROUP) {
				a->scope = ISUP_SCOPE_CG;
				a->id = ct->cg.cg->id;
				a->cic = ct->cg.cg->cic;
			} else {
				a->scope = ISUP_SCOPE_CT;
				a->id = ct->id;
				a->cic = ct->cic;
			}
			printd(("%s: %p: <- CC_BLOCKING_IND\n", DRV_NAME, cc));
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
 *  CC_BLOCKING_CON
 *  -----------------------------------
 *  From received BLA or CGBA.
 */
STATIC INLINE int
cc_blocking_con(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = (m->mt == ISUP_MT_CGBA) ? ct->cg.cg->gmg.cc : ct->mgm.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_blocking_con *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_BLOCKING_CON;
			p->cc_flags =
			    (m->mt ==
			     ISUP_MT_CGBA) ? ISUP_GROUP | m->msg.cgba.
			    cgi : ISUP_MAINTENANCE_ORIENTED;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			if (p->cc_flags & ISUP_GROUP) {
				a->scope = ISUP_SCOPE_CG;
				a->id = ct->cg.cg->id;
				a->cic = ct->cg.cg->cic;
			} else {
				a->scope = ISUP_SCOPE_CT;
				a->id = ct->id;
				a->cic = ct->cic;
			}
			printd(("%s: %p: <- CC_BLOCKING_CON\n", DRV_NAME, cc));
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
 *  CC_UNBLOCKING_IND
 *  -----------------------------------
 *  From received UBL or CGU or IAM.
 */
STATIC INLINE int
cc_unblocking_ind(queue_t *q, struct cc *cc, struct ct *ct, isup_msg_t * m)
{
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	printd(("%s: MAINT: unblocking indication on circuit id=%ld, cic=%ld\n", DRV_NAME,
		ct->id, ct->cic));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_unblocking_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_UNBLOCKING_IND;
			switch (m->mt) {
			case ISUP_MT_UBL:
				p->cc_flags = ISUP_MAINTENANCE_ORIENTED;
				break;
			case ISUP_MT_CGU:
				p->cc_flags = ISUP_GROUP | m->msg.cgu.cgi;
				break;
			default:
			case ISUP_MT_IAM:
				if (ct_tst(ct, CCTF_REM_M_BLOCKED))
					p->cc_flags = ISUP_MAINTENANCE_ORIENTED;
				else if (ct_tst(ct, CCTF_REM_H_BLOCKED))
					p->cc_flags = ISUP_HARDWARE_FAILURE_ORIENTED;
				else
					goto efault;
				break;
			}
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			if (p->cc_flags & ISUP_GROUP) {
				a->scope = ISUP_SCOPE_CG;
				a->id = ct->cg.cg->id;
				a->cic = ct->cg.cg->cic;
			} else {
				a->scope = ISUP_SCOPE_CT;
				a->id = ct->id;
				a->cic = ct->cic;
			}
			printd(("%s: %p: <- CC_UNBLOCKING_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	      efault:
		swerr();
		freemsg(mp);
		return (-EFAULT);
	}
	rare();
	return (-EBUSY);
}

/*
 *  CC_UNBLOCKING_CON
 *  -----------------------------------
 *  From received UBA or CGUA.
 */
STATIC INLINE int
cc_unblocking_con(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = (m->mt == ISUP_MT_CGUA) ? ct->cg.cg->gmg.cc : ct->mgm.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_unblocking_con *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_UNBLOCKING_CON;
			p->cc_flags =
			    (m->mt ==
			     ISUP_MT_CGUA) ? ISUP_GROUP | m->msg.cgua.
			    cgi : ISUP_MAINTENANCE_ORIENTED;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			if (p->cc_flags & ISUP_GROUP) {
				a->scope = ISUP_SCOPE_CG;
				a->id = ct->cg.cg->id;
				a->cic = ct->cg.cg->cic;
			} else {
				a->scope = ISUP_SCOPE_CT;
				a->id = ct->id;
				a->cic = ct->cic;
			}
			printd(("%s: %p: <- CC_UNBLOCKING_CON\n", DRV_NAME, cc));
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
 *  CC_QUERY_IND
 *  -----------------------------------
 *  From received CQM.
 */
STATIC INLINE int
cc_query_ind(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc;
	if (!(cc = ct->bind.mgm) && !(cc = ct->cg.cg->bind.mgm) && !(cc = ct->tg.tg->bind.mgm)
	    && !(cc = ct->sr.sr->bind.mgm) && !(cc = ct->sp.sp->bind.mgm)
	    && !(cc = master.bind.mgm)) {
		/* 
		   Note: if nobody is listening, these indications should be logged. */
		__printd(("%s: MAINT: unhandled query indication on circuit id=%ld, cic=%ld\n",
			  DRV_NAME, ct->id, ct->cic));
		return (QR_DONE);
	}
	ensure(cc->oq, return (QR_DONE));
	printd(("%s: MAINT: query indication on circuit id=%ld, cic=%ld\n", DRV_NAME, ct->id,
		ct->cic));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_query_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_QUERY_IND;
			p->cc_flags = ISUP_GROUP;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_CG;
			a->id = ct->cg.cg->id;
			a->cic = ct->cg.cg->cic;
			printd(("%s: %p: <- CC_QUERY_IND\n", DRV_NAME, cc));
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
 *  CC_QUERY_CON
 *  -----------------------------------
 *  From received CQR.
 */
STATIC INLINE int
cc_query_con(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	struct cc *cc = ct->cg.cg->gmg.cc;
	ensure(cc, return (QR_DONE));
	ensure(cc->oq, return (QR_DONE));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_query_con *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_QUERY_CON;
			p->cc_flags = ISUP_GROUP;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_CG;
			a->id = ct->cg.cg->id;
			a->cic = ct->cg.cg->cic;
			printd(("%s: %p: <- CC_QUERY_CON\n", DRV_NAME, cc));
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
 *  CC_MAINT_IND
 *  -----------------------------------
 */
STATIC INLINE int
cc_maint_ind(queue_t *q, struct ct *ct, ulong reason)
{
	struct cc *cc;
	if (!(cc = ct->bind.mnt) && !(cc = ct->cg.cg->bind.mnt) && !(cc = ct->tg.tg->bind.mnt)
	    && !(cc = ct->sr.sr->bind.mnt) && !(cc = ct->sp.sp->bind.mnt)
	    && !(cc = master.bind.mnt)) {
		/* 
		   Note: if nobody is listening, these indications should be logged. */
		__printd(("%s: MAINT: unhandled maintenance indication on circuit id=%ld, cic=%ld, reason=%lu\n", DRV_NAME, ct->id, ct->cic, reason));
		return (QR_DONE);
	}
	ensure(cc->oq, return (QR_DONE));
	printd(("%s: MAINT: maintenance indication on circuit id=%ld, cic=%ld, reason=%lu\n",
		DRV_NAME, ct->id, ct->cic, reason));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_maint_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_MAINT_IND;
			p->cc_call_ref = ct->cref;
			p->cc_reason = reason;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_CT;
			a->id = ct->id;
			a->cic = ct->cic;
			printd(("%s: %p: <- CC_MAINT_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}
STATIC INLINE int
cc_cg_maint_ind(queue_t *q, struct cg *cg, ulong cic, ulong reason)
{
	struct cc *cc;
	ensure(cg, return (QR_DONE));
	ensure(cg->sr.sr, return (QR_DONE));
	ensure(cg->sp.sp, return (QR_DONE));
	if (!(cc = cg->bind.mnt) && !(cc = cg->sr.sr->bind.mnt) && !(cc = cg->sp.sp->bind.mnt)
	    && !(cc = master.bind.mnt)) {
		/* 
		   Note: if nobody is listening, these indications should be logged. */
		__printd(("%s: MAINT: unhandled maintenance indication on circuit group id=%ld, cic=%ld, reason=%lu\n", DRV_NAME, cg->id, cic, reason));
		return (QR_DONE);
	}
	ensure(cc->oq, return (QR_DONE));
	printd(("%s: MAINT: maintenance indication on circuit group id=%ld, cic=%ld, reason=%lu\n",
		DRV_NAME, cg->id, cic, reason));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_maint_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_MAINT_IND;
			p->cc_call_ref = 0;
			p->cc_reason = reason;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_CG;
			a->id = cg->id;
			a->cic = cic;
			printd(("%s: %p: <- CC_MAINT_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}
STATIC INLINE int
cc_if_maint_ind(queue_t *q, struct sr *sr, ulong cic, ulong reason)
{
	struct cc *cc;
	if (!(cc = sr->bind.mnt) && !(cc = sr->sp.sp->bind.mnt) && !(cc = master.bind.mnt)) {
		/* 
		   Note: if nobody is listening, these indications should be logged. */
		__printd(("%s: MAINT: unhandled maintenance indication on signalling relation id=%ld, cic=%ld, reason=%lu\n", DRV_NAME, sr->id, cic, reason));
		return (QR_DONE);
	}
	ensure(cc->oq, return (QR_DONE));
	printd(("%s: MAINT: maintenance indication on signalling relation id=%ld, cic=%ld, reason=%lu\n", DRV_NAME, sr->id, cic, reason));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_maint_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_MAINT_IND;
			p->cc_call_ref = 0;
			p->cc_reason = reason;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_SR;
			a->id = sr->id;
			a->cic = cic;
			printd(("%s: %p: <- CC_MAINT_IND\n", DRV_NAME, cc));
			ss7_oput(cc->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}
STATIC INLINE int
cc_sr_maint_ind(queue_t *q, struct sr *sr, ulong reason)
{
	struct cc *cc;
	if (!(cc = sr->bind.mnt) && !(cc = sr->sp.sp->bind.mnt) && !(cc = master.bind.mnt)) {
		/* 
		   Note: if nobody is listening, these indications should be logged. */
		__printd(("%s: MAINT: unhandled maintenance indication on signalling relation id=%ld, reason=%lu\n", DRV_NAME, sr->id, reason));
		return (QR_DONE);
	}
	ensure(cc->oq, return (QR_DONE));
	printd(("%s: MAINT: maintenance indication on signalling relation id=%ld, reason=%lu\n",
		DRV_NAME, sr->id, reason));
	if (canput(cc->oq)) {
		mblk_t *mp;
		struct CC_maint_ind *p;
		struct isup_addr *a;
		size_t add_len = sizeof(*a);
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->cc_primitive = CC_MAINT_IND;
			p->cc_call_ref = 0;
			p->cc_reason = reason;
			p->cc_addr_length = add_len;
			p->cc_addr_offset = add_len ? sizeof(*p) : 0;
			a = ((typeof(a)) mp->b_wptr)++;
			a->scope = ISUP_SCOPE_SR;
			a->id = sr->id;
			a->cic = 0;
			printd(("%s: %p: <- CC_MAINT_IND\n", DRV_NAME, cc));
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
 *  -------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_BIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_bind_req(queue_t *q, struct mtp *mtp, ulong flags, mtp_addr_t * add)
{
	mblk_t *mp;
	struct MTP_bind_req *p;
	size_t add_len = add ? sizeof(*add) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_BIND_REQ;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		p->mtp_bind_flags = flags;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: MTP_BIND_REQ ->\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MTP_UNBIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_unbind_req(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	struct MTP_unbind_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_UNBIND_REQ;
		printd(("%s: %p: MTP_UNBIND_REQ ->\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MTP_CONN_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_conn_req(queue_t *q, struct mtp *mtp, ulong flags, mtp_addr_t * add)
{
	mblk_t *mp;
	struct MTP_conn_req *p;
	size_t add_len = add ? sizeof(*add) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_CONN_REQ;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		p->mtp_conn_flags = flags;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: MTP_CONN_REQ ->\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MTP_DISCON_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_discon_req(queue_t *q, struct mtp *mtp)
{
	if (canput(mtp->oq)) {
		mblk_t *mp;
		struct MTP_discon_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->mtp_primitive = MTP_DISCON_REQ;
			printd(("%s: %p: MTP_DISCON_REQ ->\n", DRV_NAME, mtp));
			ss7_oput(mtp->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  MTP_ADDR_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_addr_req(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	struct MTP_addr_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_ADDR_REQ;
		printd(("%s: %p: MTP_ADDR_REQ ->\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MTP_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_info_req(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	struct MTP_info_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_INFO_REQ;
		printd(("%s: %p: MTP_INFO_REQ ->\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MTP_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_optmgmt_req(queue_t *q, struct mtp *mtp, ulong flags, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	struct MTP_optmgmt_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_OPTMGMT_REQ;
		p->mtp_opt_length = opt_len;
		p->mtp_opt_offset = opt_len ? sizeof(*p) : 0;
		p->mtp_mgmt_flags = flags;
		if (opt_len) {
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		printd(("%s: %p: MTP_OPTMGMT_REQ ->\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MTP_TRANSFER_REQ
 *  -----------------------------------
 */
STATIC INLINE int
mtp_transfer_req(queue_t *q, struct mtp *mtp, mtp_addr_t * add, ulong prior, ulong sls, mblk_t *dp)
{
	if (canput(mtp->oq)) {
		mblk_t *mp;
		struct MTP_transfer_req *p;
		size_t add_len = add ? sizeof(*add) : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->mtp_primitive = MTP_TRANSFER_REQ;
			p->mtp_dest_length = add_len;
			p->mtp_dest_offset = add_len ? sizeof(*p) : 0;
			p->mtp_mp = prior;
			p->mtp_sls = sls;
			if (add_len) {
				bcopy(add, mp->b_wptr, add_len);
				mp->b_wptr += add_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: MTP_TRANSFER_REQ ->\n", DRV_NAME, mtp));
			ss7_oput(mtp->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Encode message functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Size, pack and unpack parameter functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  CIC
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cic(uint pvar, ulong cic)
{
	return (2);
}
STATIC INLINE int
pack_cic(uint pvar, uchar **p, ulong cic)
{
	*(*p)++ = cic;
	*(*p)++ = (cic >> 8) & 0x3f;	/* watch out for ANSI 32k CICs */
	return (2);
}

/*
 *  MESSAGE TYPE
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_mt(uint pvar, ulong mt)
{
	return (1);
}
STATIC INLINE int
pack_mt(uint pvar, uchar **p, ulong mt)
{
	*(*p)++ = mt;
	return (1);
}

/*
 *  ISUP_PT_EOP: 0 - End of optional parameters
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_eop(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_eop(uint pvar, uchar **p)
{
	*(*p)++ = 0;
	return (1);
}
STATIC INLINE int
unpack_eop(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = 0;
		v->ptr = p;
		v->val = 0;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CLRF: 1 - Call reference @
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_clrf(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_clrf(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_clrf(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_TMR: 2 - Transmission medium requirement
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_tmr(uint pvar, ulong tmr)
{
	return (1);
}
STATIC INLINE int
pack_tmr(uint pvar, uchar **p, ulong tmr)
{
	*(*p)++ = tmr;
	return (1);
}
STATIC INLINE int
unpack_tmr(uint pvar, uchar *p, uchar *e, ulong *tmr)
{
	if (p < e) {
		*tmr = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_ATP: 3 - Access transport
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_atp(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_atp(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_atp(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CDPN: 4 - Called party number
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cdpn(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_cdpn(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_cdpn(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		switch (v->len) {
		case 0:
			v->val = 0;
			break;
		case 1:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}
STATIC INLINE int
isup_st_in_cdpn(uchar *p, size_t l)
{
	uint digs, i;
	uchar c, d;
	digs = ((l - 2) << 1) - ((*p & 0x80) >> 7);
	for (i = 0; i < digs; i++) {
		c = p[(i >> 1) + 2];
		if (i & 0x1) {
			d = (c >> 0) & 0x0f;
		} else {
			d = (c >> 4) & 0x0f;
		}
		if (d == 0x0f)
			return (-1);	/* we found an ST */
	}
	return (0);		/* didn't find an ST */
}
STATIC INLINE int
isup_check_cdpn(char *p, size_t l)
{
	if (l < 3) {
		ptrace(("%s: PROTO: bad cdpn size\n", DRV_NAME));
		return (-EMSGSIZE);	/* bad msg size */
	}
	if ((p[0] & 0x80) && ((p[l - 1] & 0xf0) != 0x00)) {
		ptrace(("%s: PROTO: bad cdpn address filler\n", DRV_NAME));
		return (-EINVAL);	/* bad filler */
	}
	/* 
	   can't check much else */
	return (0);
}

/*
 *  ISUP_PT_SUBN: 5 - Subsequent number
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_subn(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_subn(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_subn(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		switch (v->len) {
		case 0:
			v->val = 0;
			break;
		case 1:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}
STATIC INLINE int
isup_st_in_subn(uchar *p, size_t l)
{
	uint digs, i;
	uchar c, d;
	digs = ((l - 1) << 1) - ((*p & 0x80) >> 7);
	for (i = 0; i < digs; i++) {
		c = p[(i >> 1) + 1];
		if (i & 0x1) {
			d = (c >> 0) & 0x0f;
		} else {
			d = (c >> 4) & 0x0f;
		}
		if (d == 0x0f)
			return (-1);	/* we found an ST */
	}
	return (0);		/* didn't find an ST */
}
STATIC INLINE int
isup_check_subn(uchar *p, size_t l)
{
	if (l < 2) {
		ptrace(("%s: PROTO: bad subn size\n", DRV_NAME));
		return (-EMSGSIZE);	/* bad msg size */
	}
	if ((p[0] & 0x80) && ((p[l - 1] & 0xf0) != 0x00)) {
		ptrace(("%s: PROTO: bad subn address filler\n", DRV_NAME));
		return (-EINVAL);	/* bad filler */
	}
	/* 
	   can't check much else */
	return (0);
}

/*
 *  ISUP_PT_NCI: 6 - Nature of connection indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_nci(uint pvar, ulong nci)
{
	return (1);
}
STATIC INLINE int
pack_nci(uint pvar, uchar **p, ulong nci)
{
	*(*p)++ = nci;
	return (1);
}
STATIC INLINE int
unpack_nci(uint pvar, uchar *p, uchar *e, ulong *nci)
{
	if (p < e) {
		*nci = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_FCI: 7 - Forward call indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_fci(uint pvar, ulong fci)
{
	return (2);
}
STATIC INLINE int
pack_fci(uint pvar, uchar **p, ulong fci)
{
	*(*p)++ = fci;
	*(*p)++ = (fci >> 8);
	return (2);
}
STATIC INLINE int
unpack_fci(uint pvar, uchar *p, uchar *e, ulong *fci)
{
	if (p + 1 < e) {
		*fci = 0;
		*fci |= (ulong) (*(p)++);
		*fci |= ((ulong) (*(p)++) << 8);
		return (2);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_OFCI: 8 - Optional forward call indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_ofci(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_ofci(uint pvar, uchar **p, ulong ofci)
{
	*(*p)++ = ofci;
	return (1);
}
STATIC INLINE int
unpack_ofci(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CPC: 9 - Calling party's category
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cpc(uint pvar, ulong cpc)
{
	return (1);
}
STATIC INLINE int
pack_cpc(uint pvar, uchar **p, ulong cpc)
{
	*(*p)++ = cpc;
	return (1);
}
STATIC INLINE int
unpack_cpc(uint pvar, uchar *p, uchar *e, ulong *cpc)
{
	if (p < e) {
		*cpc = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CGPN: 10 - Calling party number
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cgpn(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_cgpn(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_cgpn(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_RDGN: 11 - Redirecting number
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_rdgn(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_rdgn(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_rdgn(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		v->val |= ((ulong) (*p++) << 0);
		v->val |= ((ulong) (*p++) << 8);
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_RDNN: 12 - Redirection number
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_rdnn(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_rdnn(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_rdnn(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		v->val |= ((ulong) (*p++) << 0);
		v->val |= ((ulong) (*p++) << 8);
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CONR: 13 - Connection request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_conr(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_conr(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_conr(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_INRI: 14 - Information request indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_inri(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_inri(uint pvar, uchar **p, ulong inri)
{
	*(*p)++ = inri;
	return (1);
}
STATIC INLINE int
unpack_inri(uint pvar, uchar *p, uchar *e, ulong *inri)
{
	if (p < e) {
		*inri = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_INFI: 15 - Information indicators @
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_infi(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_infi(uint pvar, uchar **p, ulong infi)
{
	*(*p)++ = infi;
	return (1);
}
STATIC INLINE int
unpack_infi(uint pvar, uchar *p, uchar *e, ulong *infi)
{
	if (p < e) {
		*infi = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_COTI: 16 - Continuity indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_coti(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_coti(uint pvar, uchar **p, ulong coti)
{
	*(*p)++ = coti;
	return (1);
}
STATIC INLINE int
unpack_coti(uint pvar, uchar *p, uchar *e, ulong *coti)
{
	if (p < e) {
		*coti = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_BCI: 17 - Backward call indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_bci(uint pvar, ulong bci)
{
	return (2);
}
STATIC INLINE int
pack_bci(uint pvar, uchar **p, ulong bci)
{
	*(*p)++ = bci;
	*(*p)++ = (bci >> 8);
	return (2);
}
STATIC INLINE int
unpack_bci(uint pvar, uchar *p, uchar *e, ulong *bci)
{
	if (p < e) {
		ulong x = (*(p)++ & 0xff);
		ulong y = (*(p)++ & 0xff);
		*bci = x | (y << 8);
		return (2);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CAUS: 18 - Cause indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_caus(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_caus(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_caus(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		v->val |= ((ulong) (*(p)++) << 8);	/* XXX */
		v->val |= ((ulong) (*(p)++) << 0);
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_RDI: 19 - Redirection information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_rdi(uint pvar)
{
	return (2);
}
STATIC INLINE int
pack_rdi(uint pvar, uchar **p, ulong rdi)
{
	*(*p)++ = rdi;
	*(*p)++ = rdi >> 8;
	return (2);
}
STATIC INLINE int
unpack_rdi(uint pvar, uchar *p, uchar *e, ulong *rdi)
{
	if (p + 1 < e) {
		*rdi = 0;
		*rdi |= ((ulong) (*p++) << 0);
		*rdi |= ((ulong) (*p++) << 8);
		return (2);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CGI: 21 - Circuit group supervision msg type indicator
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cgi(uint pvar, ulong cgi)
{
	return (1);
}
STATIC INLINE int
pack_cgi(uint pvar, uchar **p, ulong cgi)
{
	*(*p)++ = cgi;
	return (1);
}
STATIC INLINE int
unpack_cgi(uint pvar, uchar *p, uchar *e, ulong *cgi)
{
	if (p < e) {
		*cgi = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_RS: 22 - Range and status
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_rs(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_rs(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_rs(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		switch (v->len) {
		case 0:
			v->val = 0;
			break;
		case 1:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	ptrace(("p = %p, e = %p\n", p, e));
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CMI: 23 - Call modification indicators (Blue Book)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cmi(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_cmi(uint pvar, uchar **p, ulong cmi)
{
	*(*p)++ = cmi;
	return (1);
}
STATIC INLINE int
unpack_cmi(uint pvar, uchar *p, uchar *e, ulong *cmi)
{
	if (p < e) {
		*cmi = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_FACI: 24 - Facility indicator
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_faci(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_faci(uint pvar, uchar **p, ulong faci)
{
	*(*p)++ = faci;
	return (1);
}
STATIC INLINE int
unpack_faci(uint pvar, uchar *p, uchar *e, ulong *faci)
{
	if (p < e) {
		*faci = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_FAII: 25 - Facility information indicators (Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_faii(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_faii(uint pvar, uchar **p, ulong faii)
{
	*(*p)++ = faii;
	return (1);
}
STATIC INLINE int
unpack_faii(uint pvar, uchar *p, uchar *e, ulong *faii)
{
	if (p < e) {
		*faii = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CUGI: 26 - Closed user group interlock code
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cugi(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_cugi(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_cugi(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_INDEX: 27 - Index (Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_index(uint pvar)
{
	fixme(("Correct format for this paramter\n"));
	return (4);
}
STATIC INLINE int
pack_index(uint pvar, uchar **p, ulong index)
{
	*(*p)++ = index >> 0;
	*(*p)++ = index >> 8;
	*(*p)++ = index >> 16;
	*(*p)++ = index >> 24;
	fixme(("Correct format for this paramter\n"));
	return (4);
}
STATIC INLINE int
unpack_index(uint pvar, uchar *p, uchar *e, ulong *index)
{
	fixme(("Correct format for this paramter\n"));
	if (p + 3 < e) {
		*index = 0;
		*index |= ((ulong) (*p++) << 0);
		*index |= ((ulong) (*p++) << 8);
		*index |= ((ulong) (*p++) << 16);
		*index |= ((ulong) (*p++) << 24);
		return (4);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_USI: 29 - User service information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_usi(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_usi(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_usi(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		v->val |= ((ulong) (*(p)++) << 0);
		v->val |= ((ulong) (*(p)++) << 8);
		v->val |= ((ulong) (*(p)++) << 16);
		v->val |= ((ulong) (*(p)++) << 24);
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_SPC: 30 - Signalling point code @ (Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_spc(uint pvar)
{
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		return (3);
	default:
		return (2);
	}
}
STATIC INLINE int
pack_spc(uint pvar, uchar **p, ulong spc)
{
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		*(*p)++ = spc >> 0;
		*(*p)++ = spc >> 8;
		*(*p)++ = spc >> 16;
		return (3);
	default:
		*(*p)++ = spc >> 0;
		*(*p)++ = spc >> 8;
		return (2);
	}
}
STATIC INLINE int
unpack_spc(uint pvar, uchar *p, uchar *e, ulong *spc)
{
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		if (p + 2 < e) {
			*spc = 0;
			*spc |= ((ulong) (*p++) >> 0);
			*spc |= ((ulong) (*p++) >> 8);
			*spc |= ((ulong) (*p++) >> 16);
			return (3);
		}
		break;
	default:
		if (p + 1 < e) {
			*spc = 0;
			*spc |= ((ulong) (*p++) >> 0);
			*spc |= ((ulong) (*p++) >> 8);
			return (2);
		}
		break;
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_UUI: 32 - User to user information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_uui(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_uui(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_uui(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		switch (v->len) {
		case 0:
			v->val = 0;
			break;
		case 1:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CONN: 33 - Connected number
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_conn(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_conn(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_conn(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_SRIS: 34 - Suspend/resume indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_sris(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_sris(uint pvar, uchar **p, ulong sris)
{
	*(*p)++ = sris;
	return (1);
}
STATIC INLINE int
unpack_sris(uint pvar, uchar *p, uchar *e, ulong *sris)
{
	if (p < e) {
		*sris = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_TNS: 35 - Transit network selection @
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_tns(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_tns(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	return (len + 1);
}
STATIC INLINE int
unpack_tns(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_EVNT: 36 - Event information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_evnt(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_evnt(uint pvar, uchar **p, ulong evnt)
{
	*(*p)++ = evnt;
	return (1);
}
STATIC INLINE int
unpack_evnt(uint pvar, uchar *p, uchar *e, ulong *evnt)
{
	if (p < e) {
		*evnt = *(p)++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CSI: 38 - Circuit state indicator @
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_csi(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_csi(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_csi(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		switch (v->len) {
		case 0:
			v->val = 0;
			break;
		case 1:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_ACL: 39 - Automatic congestion level
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_acl(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_acl(uint pvar, uchar **p, ulong acl)
{
	*(*p)++ = acl;
	return (1);
}
STATIC INLINE int
unpack_acl(uint pvar, uchar *p, uchar *e, ulong *acl)
{
	if (p < e) {
		*acl = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_OCDN: 40 - Original called number
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_ocdn(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_ocdn(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_ocdn(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_OBCI: 41 - Optional backward call indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_obci(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_obci(uint pvar, uchar **p, ulong obci)
{
	*(*p)++ = 1;
	*(*p)++ = obci;
	return (2);
}
STATIC INLINE int
unpack_obci(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_UUIND: 42 - User to user indicators
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_uuind(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_uuind(uint pvar, uchar **p, ulong uuind)
{
	*(*p)++ = uuind;
	return (1);
}
STATIC INLINE int
unpack_uuind(uint pvar, uchar *p, uchar *e, ulong *uuind)
{
	if (p < e) {
		*uuind = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_ISPC: 43 - Origination ISC point code
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_ispc(uint pvar)
{
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		return (3);
	default:
		return (2);
	}
}
STATIC INLINE int
pack_ispc(uint pvar, uchar **p, ulong ispc)
{
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		*(*p)++ = ispc >> 0;
		*(*p)++ = ispc >> 8;
		*(*p)++ = ispc >> 16;
		return (3);
	default:
		*(*p)++ = ispc >> 0;
		*(*p)++ = ispc >> 8;
		return (2);
	}
}
STATIC INLINE int
unpack_ispc(uint pvar, uchar *p, uchar *e, ulong *ispc)
{
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		if (p + 2 < e) {
			*ispc = 0;
			*ispc |= ((ulong) (*p++) << 0);
			*ispc |= ((ulong) (*p++) << 8);
			*ispc |= ((ulong) (*p++) << 16);
			return (3);
		}
		break;
	default:
		if (p + 1 < e) {
			*ispc = 0;
			*ispc |= ((ulong) (*p++) << 0);
			*ispc |= ((ulong) (*p++) << 8);
			return (2);
		}
		break;
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_GNOT: 44 - Generic notification
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_gnot(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_gnot(uint pvar, uchar **p, ulong gnot)
{
	*(*p)++ = gnot;
	return (1);
}
STATIC INLINE int
unpack_gnot(uint pvar, uchar *p, uchar *e, ulong *gnot)
{
	if (p < e) {
		*gnot = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CHI: 45 - Call history information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_chi(uint pvar)
{
	return (2);
}
STATIC INLINE int
pack_chi(uint pvar, uchar **p, ulong chi)
{
	*(*p)++ = chi >> 0;
	*(*p)++ = chi >> 8;
	return (2);
}
STATIC INLINE int
unpack_chi(uint pvar, uchar *p, uchar *e, ulong *chi)
{
	if (p < e) {
		*chi = 0;
		*chi |= ((ulong) (*p++) << 0);
		*chi |= ((ulong) (*p++) << 8);
		return (2);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_ADI: 46 - Access delivery information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_adi(uint pvar, ulong adi)
{
	return (1);
}
STATIC INLINE int
pack_adi(uint pvar, uchar **p, ulong adi)
{
	*(*p)++ = adi;
	return (1);
}
STATIC INLINE int
unpack_adi(uint pvar, uchar *p, uchar *e, ulong *adi)
{
	if (p < e) {
		*adi = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_NSF: 47 - Network specific facilities @
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_nsf(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_nsf(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_nsf(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		v->val |= ((ulong) (*p++) << 0);
		v->val |= ((ulong) (*p++) << 8);
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_USIP: 48 - User service information prime
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_usip(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_usip(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_usip(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_PROP: 49 - Propagation delay counter
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_prop(uint pvar)
{
	return (2);
}
STATIC INLINE int
pack_prop(uint pvar, uchar **p, ulong prop)
{
	*(*p)++ = prop >> 0;
	*(*p)++ = prop >> 8;
	return (2);
}
STATIC INLINE int
unpack_prop(uint pvar, uchar *p, uchar *e, ulong *prop)
{
	if (p + 1 < e) {
		*prop = 0;
		*prop |= ((ulong) (*p++) << 0);
		*prop |= ((ulong) (*p++) << 8);
		return (2);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_ROPS: 50 - Remote operations @
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_rops(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_rops(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_rops(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_SA: 51 - Service activation @
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_sa(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_sa(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_sa(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = 0;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_UTI: 52 - User teleservice information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_uti(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_uti(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	return (len + 1);
}
STATIC INLINE int
unpack_uti(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_TMU: 53 - Transmission medium used
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_tmu(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_tmu(uint pvar, uchar **p, ulong tmu)
{
	*(*p)++ = tmu;
	return (1);
}
STATIC INLINE int
unpack_tmu(uint pvar, uchar *p, uchar *e, ulong *tmu)
{
	if (p < e) {
		*tmu = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CDI: 54 - Call diversion information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cdi(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_cdi(uint pvar, uchar **p, ulong cdi)
{
	*(*p)++ = cdi;
	return (1);
}
STATIC INLINE int
unpack_cdi(uint pvar, uchar *p, uchar *e, ulong *cdi)
{
	if (p < e) {
		*cdi = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_ECI: 55 - Echo control information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_eci(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_eci(uint pvar, uchar **p, ulong eci)
{
	*(*p)++ = eci;
	return (1);
}
STATIC INLINE int
unpack_eci(uint pvar, uchar *p, uchar *e, ulong *eci)
{
	if (p < e) {
		*eci = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_MCI: 56 - Message compatibility information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_mci(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_mci(uint pvar, uchar **p, ulong mci)
{
	*(*p)++ = mci;
	return (1);
}
STATIC INLINE int
unpack_mci(uint pvar, uchar *p, uchar *e, ulong *mci)
{
	if (p < e) {
		*mci = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_PCI: 57 - Parameter compatibility information
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_pci(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_pci(uint pvar, uchar **p, ulong pci)
{
	*(*p)++ = pci;
	return (1);
}
STATIC INLINE int
unpack_pci(uint pvar, uchar *p, uchar *e, ulong *pci)
{
	if (p < e) {
		*pci = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_MLPP: 58 - MLPP preference
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_mlpp(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_mlpp(uint pvar, uchar **p, ulong mlpp)
{
	*(*p)++ = mlpp;
	return (1);
}
STATIC INLINE int
unpack_mlpp(uint pvar, uchar *p, uchar *e, ulong *mlpp)
{
	if (p < e) {
		*mlpp = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_MCIQ: 59 - MCID request indicator
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_mciq(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_mciq(uint pvar, uchar **p, ulong mciq)
{
	*(*p)++ = mciq;
	return (1);
}
STATIC INLINE int
unpack_mciq(uint pvar, uchar *p, uchar *e, ulong *mciq)
{
	if (p < e) {
		*mciq = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_MCIR: 60 - MCID response indicator
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_mcir(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_mcir(uint pvar, uchar **p, ulong mcir)
{
	*(*p)++ = mcir;
	return (1);
}
STATIC INLINE int
unpack_mcir(uint pvar, uchar *p, uchar *e, ulong *mcir)
{
	if (p < e) {
		*mcir = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_HOPC: 61 - Hop counter (reserved)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_hopc(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_hopc(uint pvar, uchar **p, ulong hopc)
{
	*(*p)++ = hopc;
	return (1);
}
STATIC INLINE int
unpack_hopc(uint pvar, uchar *p, uchar *e, ulong *hopc)
{
	if (p < e) {
		*hopc = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_TMRP: 62 - Transmission medium requirement prime
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_tmrp(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_tmrp(uint pvar, uchar **p, ulong tmrp)
{
	*(*p)++ = tmrp;
	return (1);
}
STATIC INLINE int
unpack_tmrp(uint pvar, uchar *p, uchar *e, ulong *tmrp)
{
	if (p < e) {
		*tmrp = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_LN: 63 - Location number
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_ln(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_ln(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_ln(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = ((ulong) (*p++) >> 0);
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_RDNR: 64 - Redirection number restriction
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_rdnr(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_rdnr(uint pvar, uchar **p, ulong rdnr)
{
	*(*p)++ = rdnr;
	return (1);
}
STATIC INLINE int
unpack_rdnr(uint pvar, uchar *p, uchar *e, ulong *rdnr)
{
	if (p < e) {
		*rdnr = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_FREEP: 65 - Freephone indicators (reserved)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_freep(uint pvar)
{
	return (1);
}
STATIC INLINE int
pack_freep(uint pvar, uchar **p, ulong freep)
{
	*(*p)++ = freep;
	return (1);
}
STATIC INLINE int
unpack_freep(uint pvar, uchar *p, uchar *e, ulong *freep)
{
	if (p < e) {
		*freep = ((ulong) (*p++) >> 0);
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_GREF: 66 - Generic reference (reserved)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_gref(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_gref(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
unpack_gref(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = ((ulong) (*p++) >> 0);
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_GNUM: 192 - Generic number (address Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_gnum(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_gnum(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_gnum(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = ((ulong) (*p++) >> 0);
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_GDIG: 193 - Generic digits @
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_gdig(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_gdig(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_gdig(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = ((ulong) (*p++) >> 0);
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_EGRESS: 195 - Egress (ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_egress(uint pvar)
{
	fixme(("Correct format for this paramter\n"));
	return (1);
}
STATIC INLINE int
pack_egress(uint pvar, uchar **p, ulong egress)
{
	fixme(("Correct format for this paramter\n"));
	*(*p)++ = egress;
	return (1);
}
STATIC INLINE int
unpack_egress(uint pvar, uchar *p, uchar *e, ulong *egress)
{
	fixme(("Correct format for this paramter\n"));
	if (p < e) {
		*egress = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_JUR: 196 - Jurisdiction (ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_jur(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_jur(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_jur(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CIDC: 197 - Carrier identification code (ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cidc(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_cidc(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_cidc(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_BGROUP: 198 - Business group (ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_bgroup(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_bgroup(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_bgroup(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_NOTI: 225 - Notification indicator (ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_noti(uint pvar)
{
	fixme(("Correct format for this paramter\n"));
	return (1);
}
STATIC INLINE int
pack_noti(uint pvar, uchar **p, ulong noti)
{
	fixme(("Correct format for this paramter\n"));
	*(*p)++ = noti;
	return (1);
}
STATIC INLINE int
unpack_noti(uint pvar, uchar *p, uchar *e, ulong *noti)
{
	fixme(("Correct format for this paramter\n"));
	if (p < e) {
		*noti = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_SVACT: 226 - Service activation (ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_svact(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_svact(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_svact(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_TRNSRQ: 227 - Transaction request (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_trnsrq(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_trnsrq(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_trnsrq(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_SPR: 228 - Special processing request (Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_spr(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_spr(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_spr(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CGCI: 229 - Circuit group characteristic indicators (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cgci(uint pvar)
{
	fixme(("Correct format for this paramter\n"));
	return (1);
}
STATIC INLINE int
pack_cgci(uint pvar, uchar **p, ulong cgci)
{
	fixme(("Correct format for this paramter\n"));
	*(*p)++ = cgci;
	return (1);
}
STATIC INLINE int
unpack_cgci(uint pvar, uchar *p, uchar *e, ulong *cgci)
{
	fixme(("Correct format for this paramter\n"));
	if (p < e) {
		*cgci = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CVRI: 230 - Circuit validation response indicator (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cvri(uint pvar)
{
	fixme(("Correct format for this paramter\n"));
	return (1);
}
STATIC INLINE int
pack_cvri(uint pvar, uchar **p, ulong cvri)
{
	fixme(("Correct format for this paramter\n"));
	*(*p)++ = cvri;
	return (1);
}
STATIC INLINE int
unpack_cvri(uint pvar, uchar *p, uchar *e, ulong *cvri)
{
	fixme(("Correct format for this paramter\n"));
	if (p < e) {
		*cvri = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_OTGN: 231 - Outgoing trunk group number (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_otgn(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_otgn(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_otgn(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CIN: 232 - Circuit identification name (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cin(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_cin(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_cin(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CLLI: 233 - Common language location identification (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_clli(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_clli(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_clli(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p++;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_OLI: 234 - Originating line information (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_oli(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_oli(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_oli(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		fixme(("Correct format for this paramter\n"));
		switch (v->len) {
		case 0:
			v->val = 0;
			break;
		case 1:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CHGN: 235 - Charge number (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_chgn(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_chgn(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_chgn(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		fixme(("Correct format for this paramter\n"));
		switch (v->len) {
		case 0:
			v->val = 0;
			break;
		case 1:
			v->val = 0;
			v->val = ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_SVCD: 236 - Service code indicator (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_svcd(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_svcd(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_svcd(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		fixme(("Correct format for this paramter\n"));
		switch (v->len) {
		case 0:
			v->val = 0;
			break;
		case 1:
			v->val = 0;
			v->val = ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CSEL: 238 - Carrier selection information (ANSI, Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_csel(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_csel(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_csel(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		fixme(("Correct format for this paramter\n"));
		switch (v->len) {
		case 0:
			v->val = 0;
			break;
		case 1:
			v->val = 0;
			v->val = ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val = 0;
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_ORI: 243 - Outgoing route identification (Spain)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_ori(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_ori(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_ori(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		fixme(("Correct format for this paramter\n"));
		v->val = 0;
		switch (v->len) {
		case 0:
			break;
		case 1:
			v->val |= ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_IRI: 244 - Incoming route identification (Spain)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_iri(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_iri(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_iri(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		fixme(("Correct format for this paramter\n"));
		v->val = 0;
		switch (v->len) {
		case 0:
			break;
		case 1:
			v->val = ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_RATE: 248 - Rate (Spain)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_rate(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_rate(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_rate(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		fixme(("Correct format for this paramter\n"));
		v->val = 0;
		switch (v->len) {
		case 0:
			break;
		case 1:
			v->val = ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_IIC: 249 - Identifier of incoming circuit (Spain)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_iic(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_iic(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_iic(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		fixme(("Correct format for this paramter\n"));
		v->val = 0;
		switch (v->len) {
		case 0:
			break;
		case 1:
			v->val = ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_TOI: 253 - Trunk offering information (Singapore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_toi(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_toi(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_toi(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		fixme(("Correct format for this paramter\n"));
		v->val = 0;
		switch (v->len) {
		case 0:
			break;
		case 1:
			v->val = ((ulong) (*(p)++) << 0);
			break;
		case 2:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			break;
		case 3:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			break;
		default:
		case 4:
			v->val |= ((ulong) (*(p)++) << 0);
			v->val |= ((ulong) (*(p)++) << 8);
			v->val |= ((ulong) (*(p)++) << 16);
			v->val |= ((ulong) (*(p)++) << 24);
			break;
		}
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_TON: 253 - Type of notification (Spain)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_ton(uint pvar, ulong ton)
{
	return (1);
}
STATIC INLINE int
pack_ton(uint pvar, uchar **p, ulong ton)
{
	*(*p)++ = ton;
	return (1);
}
STATIC INLINE int
unpack_ton(uint pvar, uchar *p, uchar *e, ulong *ton)
{
	if (p < e) {
		*ton = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_CRI: 254 - Charge rate information (Singapore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_cri(uint pvar, ulong cri)
{
	return (1);
}
STATIC INLINE int
pack_cri(uint pvar, uchar **p, ulong cri)
{
	*(*p)++ = cri;
	return (1);
}
STATIC INLINE int
unpack_cri(uint pvar, uchar *p, uchar *e, ulong *cri)
{
	if (p < e) {
		*cri = *p++;
		return (1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  ISUP_PT_ICCI: 255 - Call charge information (Singapore)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_icci(uint pvar, size_t len)
{
	return (len + 1);
}
STATIC INLINE int
pack_icci(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	*(*p)++ = len;
	bcopy(ptr, *p, len);
	*p += len;
	return (len + 1);
}
STATIC INLINE int
unpack_icci(uint pvar, uchar *p, uchar *e, isup_var_t * v)
{
	if (p + *p - 1 < e) {
		v->len = e - p;
		v->ptr = p;
		v->val = *p;
		return (v->len + 1);
	}
	trace();
	return (-EMSGSIZE);
}

/*
 *  Optional Parameters
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
size_opt(uint pvar, size_t len)
{
	return (len);
}
STATIC INLINE int
pack_opt(uint pvar, uchar **p, uchar *ptr, size_t len)
{
	bcopy(ptr, *p, len);
	*p += len;
	return (len);
}
STATIC INLINE int
unpack_opt(uint pvar, uchar *p, uchar *e, isup_var_t * v, size_t len)
{
	if (p + len - 1 < e) {
		v->len = len - 1;
		v->ptr = p;
		v->val = 0;
		return (len);
	}
	trace();
	return (-EMSGSIZE);
}

STATIC INLINE int
isup_check_opt(uchar *p, size_t len)
{
	uchar ptype = 0;
	uchar *pp = p, *ep, *e = p + len;
	if (!len)
		return (0);
	for (pp = p, ep = pp + pp[1] + 2; pp < e && (ptype = *pp); pp = ep, ep = pp + pp[1] + 2) ;
	return ((((pp != (e - 1) || ptype)) && pp != e) ? -EINVAL : 0);
}

STATIC int
isup_dec_opt(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int err = 0;
	uchar ptype;
	uchar *pp = p, *ep = e;
	if (e - p < 3) {
		trace();
		return (-EMSGSIZE);
	}
	for (pp = p, ep = pp + pp[1] + 2; pp < e && (ptype = *pp); pp = ep, ep = pp + pp[1] + 2) {
		switch (ptype) {
		case ISUP_PT_EOP:	/* 0 - End of optional parameters */
			err = unpack_eop(pvar, pp + 2, ep, &m->oparm.eop);
			break;
		case ISUP_PT_CLRF:	/* 1 - Call reference @ */
			err = unpack_clrf(pvar, pp + 2, ep, &m->oparm.clrf);
			break;
		case ISUP_PT_TMR:	/* 2 - Transmission medium requirement */
			err = unpack_tmr(pvar, pp + 2, ep, &m->oparm.tmr);
			break;
		case ISUP_PT_ATP:	/* 3 - Access transport */
			err = unpack_atp(pvar, pp + 2, ep, &m->oparm.atp);
			break;
		case ISUP_PT_CDPN:	/* 4 - Called party number */
			err = unpack_cdpn(pvar, pp + 2, ep, &m->oparm.cdpn);
			break;
		case ISUP_PT_SUBN:	/* 5 - Subsequent number */
			err = unpack_subn(pvar, pp + 2, ep, &m->oparm.subn);
			break;
		case ISUP_PT_NCI:	/* 6 - Nature of connection indicators */
			err = unpack_nci(pvar, pp + 2, ep, &m->oparm.nci);
			break;
		case ISUP_PT_FCI:	/* 7 - Forward call indicators */
			err = unpack_fci(pvar, pp + 2, ep, &m->oparm.fci);
			break;
		case ISUP_PT_OFCI:	/* 8 - Optional forward call indicators */
			err = unpack_ofci(pvar, pp + 2, ep, &m->oparm.ofci);
			break;
		case ISUP_PT_CPC:	/* 9 - Calling party's category */
			err = unpack_cpc(pvar, pp + 2, ep, &m->oparm.cpc);
			break;
		case ISUP_PT_CGPN:	/* 10 - Calling party number */
			err = unpack_cgpn(pvar, pp + 2, ep, &m->oparm.cgpn);
			break;
		case ISUP_PT_RDGN:	/* 11 - Redirecting number */
			err = unpack_rdgn(pvar, pp + 2, ep, &m->oparm.rdgn);
			break;
		case ISUP_PT_RDNN:	/* 12 - Redirection number */
			err = unpack_rdnn(pvar, pp + 2, ep, &m->oparm.rdnn);
			break;
		case ISUP_PT_CONR:	/* 13 - Connection request */
			err = unpack_conr(pvar, pp + 2, ep, &m->oparm.conr);
			break;
		case ISUP_PT_INRI:	/* 14 - Information request indicators */
			err = unpack_inri(pvar, pp + 2, ep, &m->oparm.inri);
			break;
		case ISUP_PT_INFI:	/* 15 - Information indicators @ */
			err = unpack_infi(pvar, pp + 2, ep, &m->oparm.infi);
			break;
		case ISUP_PT_COTI:	/* 16 - Continuity indicators */
			err = unpack_coti(pvar, pp + 2, ep, &m->oparm.coti);
			break;
		case ISUP_PT_BCI:	/* 17 - Backward call indicators */
			err = unpack_bci(pvar, pp + 2, ep, &m->oparm.bci);
			break;
		case ISUP_PT_CAUS:	/* 18 - Cause indicators */
			err = unpack_caus(pvar, pp + 2, ep, &m->oparm.caus);
			break;
		case ISUP_PT_RDI:	/* 19 - Redirection information */
			err = unpack_rdi(pvar, pp + 2, ep, &m->oparm.rdi);
			break;
		case ISUP_PT_CGI:	/* 21 - Circuit group supervision msg type indicator */
			err = unpack_cgi(pvar, pp + 2, ep, &m->oparm.cgi);
			break;
		case ISUP_PT_RS:	/* 22 - Range and status */
			err = unpack_rs(pvar, pp + 2, ep, &m->oparm.rs);
			break;
		case ISUP_PT_CMI:	/* 23 - Call modification indicators (Blue Book) */
			err = unpack_cmi(pvar, pp + 2, ep, &m->oparm.cmi);
			break;
		case ISUP_PT_FACI:	/* 24 - Facility indicator */
			err = unpack_faci(pvar, pp + 2, ep, &m->oparm.faci);
			break;
		case ISUP_PT_FAII:	/* 25 - Facility information indicators (Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_faii(pvar, pp + 2, ep, &m->oparm.faii);
			break;
		case ISUP_PT_CUGI:	/* 26 - Closed user group interlock code */
			err = unpack_cugi(pvar, pp + 2, ep, &m->oparm.cugi);
			break;
		case ISUP_PT_INDEX:	/* 27 - Index (Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_index(pvar, pp + 2, ep, &m->oparm.index);
			break;
		case ISUP_PT_USI:	/* 29 - User service information */
			err = unpack_usi(pvar, pp + 2, ep, &m->oparm.usi);
			break;
		case ISUP_PT_SPC:	/* 30 - Signalling point code @ (Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_spc(pvar, pp + 2, ep, &m->oparm.spc);
			break;
		case ISUP_PT_UUI:	/* 32 - User to user information */
			err = unpack_uui(pvar, pp + 2, ep, &m->oparm.uui);
			break;
		case ISUP_PT_CONN:	/* 33 - Connected number */
			err = unpack_conn(pvar, pp + 2, ep, &m->oparm.conn);
			break;
		case ISUP_PT_SRIS:	/* 34 - Suspend/resume indicators */
			err = unpack_sris(pvar, pp + 2, ep, &m->oparm.sris);
			break;
		case ISUP_PT_TNS:	/* 35 - Transit network selection @ */
			err = unpack_tns(pvar, pp + 2, ep, &m->oparm.tns);
			break;
		case ISUP_PT_EVNT:	/* 36 - Event information */
			err = unpack_evnt(pvar, pp + 2, ep, &m->oparm.evnt);
			break;
		case ISUP_PT_CSI:	/* 38 - Circuit state indicator @ */
			err = unpack_csi(pvar, pp + 2, ep, &m->oparm.csi);
			break;
		case ISUP_PT_ACL:	/* 39 - Automatic congestion level */
			err = unpack_acl(pvar, pp + 2, ep, &m->oparm.acl);
			break;
		case ISUP_PT_OCDN:	/* 40 - Original called number */
			err = unpack_ocdn(pvar, pp + 2, ep, &m->oparm.ocdn);
			break;
		case ISUP_PT_OBCI:	/* 41 - Optional backward call indicators */
			err = unpack_obci(pvar, pp + 2, ep, &m->oparm.obci);
			break;
		case ISUP_PT_UUIND:	/* 42 - User to user indicators */
			err = unpack_uuind(pvar, pp + 2, ep, &m->oparm.uuind);
			break;
		case ISUP_PT_ISPC:	/* 43 - Origination ISC point code */
			err = unpack_ispc(pvar, pp + 2, ep, &m->oparm.ispc);
			break;
		case ISUP_PT_GNOT:	/* 44 - Generic notification */
			err = unpack_gnot(pvar, pp + 2, ep, &m->oparm.gnot);
			break;
		case ISUP_PT_CHI:	/* 45 - Call history information */
			err = unpack_chi(pvar, pp + 2, ep, &m->oparm.chi);
			break;
		case ISUP_PT_ADI:	/* 46 - Access delivery information */
			err = unpack_adi(pvar, pp + 2, ep, &m->oparm.adi);
			break;
		case ISUP_PT_NSF:	/* 47 - Network specific facilities @ */
			err = unpack_nsf(pvar, pp + 2, ep, &m->oparm.nsf);
			break;
		case ISUP_PT_USIP:	/* 48 - User service information prime */
			err = unpack_usip(pvar, pp + 2, ep, &m->oparm.usip);
			break;
		case ISUP_PT_PROP:	/* 49 - Propagation delay counter */
			err = unpack_prop(pvar, pp + 2, ep, &m->oparm.prop);
			break;
		case ISUP_PT_ROPS:	/* 50 - Remote operations @ */
			err = unpack_rops(pvar, pp + 2, ep, &m->oparm.rops);
			break;
		case ISUP_PT_SA:	/* 51 - Service activation @ */
			err = unpack_sa(pvar, pp + 2, ep, &m->oparm.sa);
			break;
		case ISUP_PT_UTI:	/* 52 - User teleservice information */
			err = unpack_uti(pvar, pp + 2, ep, &m->oparm.uti);
			break;
		case ISUP_PT_TMU:	/* 53 - Transmission medium used */
			err = unpack_tmu(pvar, pp + 2, ep, &m->oparm.tmu);
			break;
		case ISUP_PT_CDI:	/* 54 - Call diversion information */
			err = unpack_cdi(pvar, pp + 2, ep, &m->oparm.cdi);
			break;
		case ISUP_PT_ECI:	/* 55 - Echo control information */
			err = unpack_eci(pvar, pp + 2, ep, &m->oparm.eci);
			break;
		case ISUP_PT_MCI:	/* 56 - Message compatibility information */
			err = unpack_mci(pvar, pp + 2, ep, &m->oparm.mci);
			break;
		case ISUP_PT_PCI:	/* 57 - Parameter compatibility information */
			err = unpack_pci(pvar, pp + 2, ep, &m->oparm.pci);
			break;
		case ISUP_PT_MLPP:	/* 58 - MLPP preference */
			err = unpack_mlpp(pvar, pp + 2, ep, &m->oparm.mlpp);
			break;
		case ISUP_PT_MCIQ:	/* 59 - MCID request indicator */
			err = unpack_mciq(pvar, pp + 2, ep, &m->oparm.mciq);
			break;
		case ISUP_PT_MCIR:	/* 60 - MCID response indicator */
			err = unpack_mcir(pvar, pp + 2, ep, &m->oparm.mcir);
			break;
		case ISUP_PT_HOPC:	/* 61 - Hop counter (reserved) */
			err = unpack_hopc(pvar, pp + 2, ep, &m->oparm.hopc);
			break;
		case ISUP_PT_TMRP:	/* 62 - Transmission medium requirement prime */
			err = unpack_tmrp(pvar, pp + 2, ep, &m->oparm.tmrp);
			break;
		case ISUP_PT_LN:	/* 63 - Location number */
			err = unpack_ln(pvar, pp + 2, ep, &m->oparm.ln);
			break;
		case ISUP_PT_RDNR:	/* 64 - Redirection number restriction */
			err = unpack_rdnr(pvar, pp + 2, ep, &m->oparm.rdnr);
			break;
		case ISUP_PT_FREEP:	/* 65 - Freephone indicators (reserved) */
			err = unpack_freep(pvar, pp + 2, ep, &m->oparm.freep);
			break;
		case ISUP_PT_GREF:	/* 66 - Generic reference (reserved) */
			err = unpack_gref(pvar, pp + 2, ep, &m->oparm.gref);
			break;
		case ISUP_PT_GNUM:	/* 192 - Generic number (address Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_gnum(pvar, pp + 2, ep, &m->oparm.gnum);
			break;
		case ISUP_PT_GDIG:	/* 193 - Generic digits @ */
			err = unpack_gdig(pvar, pp + 2, ep, &m->oparm.gdig);
			break;
		case ISUP_PT_EGRESS:	/* 195 - Egress (ANSI) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_egress(pvar, pp + 2, ep, &m->oparm.egress);
			break;
		case ISUP_PT_JUR:	/* 196 - Jurisdiction (ANSI) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_jur(pvar, pp + 2, ep, &m->oparm.jur);
			break;
		case ISUP_PT_CIDC:	/* 197 - Carrier identification code (ANSI) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_cidc(pvar, pp + 2, ep, &m->oparm.cidc);
			break;
		case ISUP_PT_BGROUP:	/* 198 - Business group (ANSI) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_bgroup(pvar, pp + 2, ep, &m->oparm.bgroup);
			break;
		case ISUP_PT_NOTI:	/* 225 - Notification indicator (ANSI) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_noti(pvar, pp + 2, ep, &m->oparm.noti);
			break;
		case ISUP_PT_SVACT:	/* 226 - Service activation (ANSI) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_svact(pvar, pp + 2, ep, &m->oparm.svact);
			break;
		case ISUP_PT_TRNSRQ:	/* 227 - Transaction request (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_trnsrq(pvar, pp + 2, ep, &m->oparm.trnsrq);
			break;
		case ISUP_PT_SPR:	/* 228 - Special processing request (Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_spr(pvar, pp + 2, ep, &m->oparm.spr);
			break;
		case ISUP_PT_CGCI:	/* 229 - Circuit group char inds (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_cgci(pvar, pp + 2, ep, &m->oparm.cgci);
			break;
		case ISUP_PT_CVRI:	/* 230 - Circuit validation resp ind (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_cvri(pvar, pp + 2, ep, &m->oparm.cvri);
			break;
		case ISUP_PT_OTGN:	/* 231 - Outgoing trunk group number (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_otgn(pvar, pp + 2, ep, &m->oparm.otgn);
			break;
		case ISUP_PT_CIN:	/* 232 - Circuit identification name (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_cin(pvar, pp + 2, ep, &m->oparm.cin);
			break;
		case ISUP_PT_CLLI:	/* 233 - Common language loc id (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_clli(pvar, pp + 2, ep, &m->oparm.clli);
			break;
		case ISUP_PT_OLI:	/* 234 - Originating line information (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_oli(pvar, pp + 2, ep, &m->oparm.oli);
			break;
		case ISUP_PT_CHGN:	/* 235 - Charge number (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_chgn(pvar, pp + 2, ep, &m->oparm.chgn);
			break;
		case ISUP_PT_SVCD:	/* 236 - Service code indicator (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_svcd(pvar, pp + 2, ep, &m->oparm.svcd);
			break;
		case ISUP_PT_CSEL:	/* 238 - Carrier selection information (ANSI, Bellcore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
				err = unpack_csel(pvar, pp + 2, ep, &m->oparm.csel);
			break;
		case ISUP_PT_ORI:	/* 243 - Outgoing route identification (Spain) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_SPAN)
				err = unpack_ori(pvar, pp + 2, ep, &m->oparm.ori);
			break;
		case ISUP_PT_IRI:	/* 244 - Incoming route identification (Spain) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_SPAN)
				err = unpack_iri(pvar, pp + 2, ep, &m->oparm.iri);
			break;
		case ISUP_PT_RATE:	/* 248 - Rate (Spain) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_SPAN)
				err = unpack_rate(pvar, pp + 2, ep, &m->oparm.rate);
			break;
		case ISUP_PT_IIC:	/* 249 - Identifier of incoming circuit (Spain) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_SPAN)
				err = unpack_iic(pvar, pp + 2, ep, &m->oparm.iic);
			break;
		case 253:
			// case ISUP_PT_TOI: /* 253 - Trunk offering information (Singapore) */
			// case ISUP_PT_TON: /* 253 - Type of notification (Spain) */
			switch (pvar & SS7_PVAR_MASK) {
			case SS7_PVAR_SING:
				err = unpack_toi(pvar, pp + 2, ep, &m->oparm.toi);
				break;
			case SS7_PVAR_SPAN:
				err = unpack_ton(pvar, pp + 2, ep, &m->oparm.ton);
				break;
			default:
				err = 0;
				break;
			}
			break;
		case ISUP_PT_CRI:	/* 254 - Charge rate information (Singapore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_SING)
				err = unpack_cri(pvar, pp + 2, ep, &m->oparm.cri);
			break;
		case ISUP_PT_ICCI:	/* 255 - Call charge information (Singapore) */
			if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_SING)
				err = unpack_icci(pvar, pp + 2, ep, &m->oparm.icci);
			break;
		default:
			err = 0;	/* ignore unrecognized optional parameters */
		}
		if (err < 0)
			return (err);
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Encode message functions
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_send_msg(queue_t *q, struct sr *sr, uchar mt, ulong prio, ulong sls, mblk_t *dp)
{
	struct mtp *mtp;
	if ((mtp = sr->mtp) || (mtp = sr->sp.sp->mtp)) {
		if (canput(mtp->oq)) {
			mblk_t *mp;
			struct MTP_transfer_req *p;
			size_t hlen = sizeof(*p) + sizeof(sr->add);
			if ((mp = ss7_allocb(q, hlen, BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = ((typeof(p)) mp->b_wptr)++;
				p->mtp_primitive = MTP_TRANSFER_REQ;
				p->mtp_dest_length = sizeof(sr->add);
				p->mtp_dest_offset = sizeof(*p);
				p->mtp_mp = prio;
				p->mtp_sls = sls;
				bcopy(&sr->add, mp->b_wptr, sizeof(sr->add));
				mp->b_wptr += sizeof(sr->add);
				mp->b_cont = dp;
				ss7_oput(mtp->oq, mp);
				/* 
				   Q.752 11.1 */
				sr->stats.msgs_sent++;
				sr->stats.msgs_sent_by_type[mt]++;
				sr->sp.sp->stats.msgs_sent++;
				sr->sp.sp->stats.msgs_sent_by_type[mt]++;
				master.stats.msgs_sent++;
				master.stats.msgs_sent_by_type[mt]++;
				return (QR_DONE);
			}
			rare();
			freemsg(dp);
			return (-ENOBUFS);
		}
		rare();
		return (-EBUSY);
	}
	swerr();
	return (-EFAULT);
}
STATIC int
isup_send_cpy(queue_t *q, struct ct *ct, uchar mt, ulong prio, ulong sls, mblk_t *dp)
{
	struct mtp *mtp;
	struct sr *sr = ct->sr.sr;
	struct sp *sp = ct->sp.sp;
	if ((mtp = sr->mtp) || (mtp = sp->mtp)) {
		if (canput(mtp->oq)) {
			mblk_t *mp;
			struct MTP_transfer_req *p;
			size_t hlen = sizeof(*p) + sizeof(sr->add);
			if ((mp = ss7_allocb(q, hlen, BPRI_MED))) {
				mblk_t *bp;
				mp->b_datap->db_type = M_PROTO;
				p = ((typeof(p)) mp->b_wptr)++;
				p->mtp_primitive = MTP_TRANSFER_REQ;
				p->mtp_dest_length = sizeof(sr->add);
				p->mtp_dest_offset = sizeof(*p);
				p->mtp_mp = prio;
				p->mtp_sls = sls;
				bcopy(&sr->add, mp->b_wptr, sizeof(sr->add));
				mp->b_wptr += sizeof(sr->add);
				mp->b_cont = dp;
				if ((bp = dupmsg(mp))) {
					bp = xchg(&ct->rel, bp);
					if (bp)
						freemsg(bp);
					ss7_oput(mtp->oq, mp);
					/* 
					   Q.752 11.1 */
					sr->stats.msgs_sent++;
					sr->stats.msgs_sent_by_type[mt]++;
					sr->sp.sp->stats.msgs_sent++;
					sr->sp.sp->stats.msgs_sent_by_type[mt]++;
					master.stats.msgs_sent++;
					master.stats.msgs_sent_by_type[mt]++;
					return (QR_DONE);
				}
				rare();
				freemsg(mp);
				return (-ENOBUFS);
			}
			rare();
			freemsg(dp);
			return (-ENOBUFS);
		}
		rare();
		return (-EBUSY);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  ISUP_MT_IAM   0x01 - Initial address
 *  -------------------------------------------------------------------------
 *  ANSI: F(NCI FCI CPC) V(USI CDPN) O()
 *  ITUT: F(NCI FCI CPC TMR) V(CDPN) O()
 */
STATIC INLINE int
isup_send_iam(queue_t *q, struct ct *ct, ulong type, ulong flags, uchar *usi_ptr, size_t usi_len,
	      uchar *cdpn_ptr, size_t cdpn_len, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	int ansi = ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI);
	ulong nci = (flags >> 0) & 0xff;
	ulong fci = (flags >> 8) & 0xffff;
	ulong cpc = (flags >> 24) & 0xff;
	ulong tmr = type & 0xff;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_IAM) + size_nci(pvar, nci)
	    + size_fci(pvar, fci) + size_cpc(pvar, cpc) + size_tmr(pvar, tmr)
	    + (ansi ? size_usi(pvar, usi_len) + 1 : 0) + size_cdpn(pvar,
								   cdpn_len) + 1 + size_opt(pvar,
											    opt_len)
	    + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_IAM);
		pack_nci(pvar, pp, nci);
		pack_fci(pvar, pp, fci);
		pack_cpc(pvar, pp, cpc);
		pack_tmr(pvar, pp, tmr);
		p = *pp;
		if (!ansi) {
			*pp += 2;
		} else {
			/* 
			   ansi has extra mandatory variable parameter USI */
			*pp += 3;
			*p = *pp - p;
			p++;
			pack_usi(pvar, pp, usi_ptr, usi_len);
		}
		*p = *pp - p;
		p++;
		pack_cdpn(pvar, pp, cdpn_ptr, cdpn_len);
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_IAM, ISUP_MP_IAM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_SAM   0x02 - Subsequent address
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: V(SUBN) O()
 */
STATIC INLINE int
isup_send_sam(queue_t *q, struct ct *ct, uchar *subn_ptr, size_t subn_len, uchar *opt_ptr,
	      size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_SAM) + size_subn(pvar, subn_len)
	    + 1 + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_SAM);
		p = mp->b_wptr;
		mp->b_wptr += 2;
		*p = mp->b_wptr - p;
		p++;
		pack_subn(pvar, pp, subn_ptr, subn_len);
		if (opt_len) {
			*p = mp->b_wptr - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_SAM, ISUP_MP_SAM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_INR   0x03 - Information request
 *  -------------------------------------------------------------------------
 *  ANSI: F(INRI) O()
 *  ITUT: F(INRI) O()
 */
STATIC INLINE int
isup_send_inr(queue_t *q, struct ct *ct, ulong inri, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_INR) + size_inri(pvar)
	    + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_INR);
		pack_inri(pvar, pp, inri);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_INR, ISUP_MP_INR, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_INF   0x04 - Information
 *  -------------------------------------------------------------------------
 *  ANSI: F(INFI) O()
 *  ITUT: F(INFI) O()
 */
STATIC INLINE int
isup_send_inf(queue_t *q, struct ct *ct, ulong infi, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_INF) + size_infi(pvar)
	    + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_INF);
		pack_infi(pvar, pp, infi);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_INF, ISUP_MP_INF, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_COT   0x05 - Continuity
 *  -------------------------------------------------------------------------
 *  ANSI: F(COTI)
 *  ITUT: F(COTI)
 */
STATIC INLINE int
isup_send_cot(queue_t *q, struct ct *ct, ulong coti)
{
	mblk_t *mp;
	uint cic = ct->cic;
	uint pvar = ct->tg.tg->proto.pvar;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_COT) + size_coti(pvar);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_COT);
		pack_coti(pvar, pp, coti);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_COT, ISUP_MP_COT, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_ACM   0x06 - Address complete
 *  -------------------------------------------------------------------------
 *  ANSI: F(BCI) O()
 *  ITUT: F(BCI) O()
 */
STATIC INLINE int
isup_send_acm(queue_t *q, struct ct *ct, ulong bci, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_ACM) + size_bci(pvar, bci)
	    + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_ACM);
		pack_bci(pvar, pp, bci);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_ACM, ISUP_MP_ACM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CON   0x07 - Connect (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: F(BCI) O()
 */
STATIC INLINE int
isup_send_con(queue_t *q, struct ct *ct, ulong bci, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CON) + size_bci(pvar, bci)
	    + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CON);
		pack_bci(pvar, pp, bci);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CON, ISUP_MP_CON, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_FOT   0x08 - Forward transfer
 *  -------------------------------------------------------------------------
 *  ANSI: O()
 *  ITUT: O()
 */
STATIC INLINE int
isup_send_fot(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_FOT) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_FOT);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_FOT, ISUP_MP_FOT, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_ANM   0x09 - Answer
 *  -------------------------------------------------------------------------
 *  ANSI: O()
 *  ITUT: O()
 */
STATIC INLINE int
isup_send_anm(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_ANM) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_ANM);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_ANM, ISUP_MP_ANM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_REL   0x0c - Release
 *  -------------------------------------------------------------------------
 *  ANSI: V(CAUS) O()
 *  ITUT: V(CAUS) O()
 */
STATIC INLINE int
isup_send_rel(queue_t *q, struct ct *ct, ulong causv, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp, *dp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	char caus_ptr[2] = { 0x80, 0x80 };
	size_t caus_len = 2;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_REL)
	    + size_caus(pvar, caus_len) + 1 + size_opt(pvar, opt_len) + 1;
	caus_ptr[1] |= causv;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		if ((dp = ss7_dupmsg(q, mp))) {
			pack_cic(pvar, pp, cic);
			pack_mt(pvar, pp, ISUP_MT_REL);
			p = *pp;
			*pp += 2;
			*p = *pp - p;
			p++;
			pack_caus(pvar, pp, caus_ptr, caus_len);
			if (opt_len) {
				*p = *pp - p;
				p++;
				pack_opt(pvar, pp, opt_ptr, opt_len);
			} else
				*p++ = 0;
			dp->b_rptr = mp->b_rptr;
			dp->b_wptr = mp->b_wptr;
			return ctrace(isup_send_cpy(q, ct, ISUP_MT_REL, ISUP_MP_REL, ct->sls, mp));
		}
		rare();
		freeb(mp);
		return (-ENOBUFS);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_SUS   0x0d - Suspend
 *  -------------------------------------------------------------------------
 *  ANSI: F(SRIS) O()
 *  ITUT: F(SRIS) O()
 */
STATIC INLINE int
isup_send_sus(queue_t *q, struct ct *ct, ulong sris, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_SUS) + size_sris(pvar)
	    + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_SUS);
		pack_sris(pvar, pp, sris);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_SUS, ISUP_MP_SUS, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_RES   0x0e - Resume
 *  -------------------------------------------------------------------------
 *  ANSI: F(SRIS) O()
 *  ITUT: F(SRIS) O()
 */
STATIC INLINE int
isup_send_res(queue_t *q, struct ct *ct, ulong sris, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_RES) + size_sris(pvar)
	    + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_RES);
		pack_sris(pvar, pp, sris);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_RES, ISUP_MP_RES, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_RLC   0x10 - Release complete
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: O()
 */
STATIC INLINE int
isup_send_rlc(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	int ansi = ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI);
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar,
						    ISUP_MT_RLC) + (ansi ? size_opt(pvar, opt_len) +
								    1 : 0);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_RLC);
		if (!ansi) {
			p = *pp;
			*pp += 1;
			if (opt_len) {
				*p = *pp - p;
				p++;
				pack_opt(pvar, pp, opt_ptr, opt_len);
			} else
				*p++ = 0;
		}
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_RLC, ISUP_MP_RLC, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CCR   0x11 - Continuity check request
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_send_ccr(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CCR);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CCR);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CCR, ISUP_MP_CCR, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_RSC   0x12 - Reset circuit
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_send_rsc(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_RSC);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_RSC);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_RSC, ISUP_MP_RSC, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_BLO   0x13 - Blocking
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_send_blo(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_BLO);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_BLO);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_BLO, ISUP_MP_BLO, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_UBL   0x14 - Unblcoking
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_send_ubl(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_UBL);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_UBL);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_UBL, ISUP_MP_UBL, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_BLA   0x15 - Blocking acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_send_bla(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_BLA);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_BLA);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_BLA, ISUP_MP_BLA, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_UBA   0x16 - Unblocking acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_send_uba(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_UBA);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_UBA);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_UBA, ISUP_MP_UBA, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_GRS   0x17 - Circuit group reset
 *  -------------------------------------------------------------------------
 *  ANSI: V(RS)
 *  ITUT: V(RS)
 */
STATIC INLINE int
isup_send_grs(queue_t *q, struct ct *ct, uchar *rs_ptr, size_t rs_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_GRS) + size_rs(pvar, rs_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_GRS);
		p = *pp;
		*pp += 1;
		*p = *pp - p;
		p++;
		pack_rs(pvar, pp, rs_ptr, rs_len);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_GRS, ISUP_MP_GRS, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CGB   0x18 - Circuit group blocking
 *  -------------------------------------------------------------------------
 *  ANSI: F(CGI) V(RS)
 *  ITUT: F(CGI) V(RS)
 */
STATIC INLINE int
isup_send_cgb(queue_t *q, struct ct *ct, ulong cgi, uchar *rs_ptr, size_t rs_len)
{
	mblk_t *mp;
	uint pvar;
	uint cic;
	size_t mlen;
	ensure(ct, return (-EFAULT));
	pvar = ct->tg.tg->proto.pvar;
	cic = ct->cic;
	mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CGB)
	    + size_cgi(pvar, cgi) + size_rs(pvar, rs_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CGB);
		pack_cgi(pvar, pp, cgi);
		p = *pp;
		*pp += 1;
		*p = *pp - p;
		p++;
		pack_rs(pvar, pp, rs_ptr, rs_len);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CGB, ISUP_MP_CGB, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CGU   0x19 - Circuit group unblocking
 *  -------------------------------------------------------------------------
 *  ANSI: F(CGI) V(RS)
 *  ITUT: F(CGI) V(RS)
 */
STATIC INLINE int
isup_send_cgu(queue_t *q, struct ct *ct, ulong cgi, uchar *rs_ptr, size_t rs_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CGU)
	    + size_cgi(pvar, cgi) + size_rs(pvar, rs_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CGU);
		pack_cgi(pvar, pp, cgi);
		p = *pp;
		*pp += 1;
		*p = *pp - p;
		p++;
		pack_rs(pvar, pp, rs_ptr, rs_len);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CGU, ISUP_MP_CGU, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CGBA   0x1a - Circuit group blocking acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: F(CGI) V(RS)
 *  ITUT: F(CGI) V(RS)
 */
STATIC INLINE int
isup_send_cgba(queue_t *q, struct ct *ct, ulong cgi, uchar *rs_ptr, size_t rs_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CGBA)
	    + size_cgi(pvar, cgi) + size_rs(pvar, rs_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CGBA);
		pack_cgi(pvar, pp, cgi);
		p = *pp;
		*pp += 1;
		*p = *pp - p;
		p++;
		pack_rs(pvar, pp, rs_ptr, rs_len);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CGBA, ISUP_MP_CGBA, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CGUA   0x1b - Circuit group unblocking acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: F(CGI) V(RS)
 *  ITUT: F(CGI) V(RS)
 */
STATIC INLINE int
isup_send_cgua(queue_t *q, struct ct *ct, ulong cgi, uchar *rs_ptr, size_t rs_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CGUA)
	    + size_cgi(pvar, cgi) + size_rs(pvar, rs_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CGUA);
		pack_cgi(pvar, pp, cgi);
		p = *pp;
		*pp += 1;
		*p = *pp - p;
		p++;
		pack_rs(pvar, pp, rs_ptr, rs_len);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CGUA, ISUP_MP_CGUA, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CMR   0x1c - Call Modification Request (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: CMI O()
 */
STATIC INLINE int
isup_send_cmr(queue_t *q, struct ct *ct, ulong cmi, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CMR)
	    + size_cmi(pvar) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CMR);
		pack_cmi(pvar, pp, cmi);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CMR, ISUP_MP_CMR, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CMC   0x1d - Call Modification Completed (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: CMI O()
 */
STATIC INLINE int
isup_send_cmc(queue_t *q, struct ct *ct, ulong cmi, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CMC)
	    + size_cmi(pvar) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CMC);
		pack_cmi(pvar, pp, cmi);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CMC, ISUP_MP_CMC, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CMRJ   0x1e - Call Modification Reject (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: CMI O()
 */
STATIC INLINE int
isup_send_cmrj(queue_t *q, struct ct *ct, ulong cmi, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CMRJ)
	    + size_cmi(pvar) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CMRJ);
		pack_cmi(pvar, pp, cmi);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CMRJ, ISUP_MP_CMRJ, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_FAR   0x1f - Facility request
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) O()
 *  ITUT: F(FACI) O()
 */
STATIC INLINE int
isup_send_far(queue_t *q, struct ct *ct, ulong faci, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_FAR)
	    + size_faci(pvar) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_FAR);
		pack_faci(pvar, pp, faci);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_FAR, ISUP_MP_FAR, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_FAA   0x20 - Facility accepted
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) O()
 *  ITUT: F(FACI) O()
 */
STATIC INLINE int
isup_send_faa(queue_t *q, struct ct *ct, ulong faci, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_FAA)
	    + size_faci(pvar) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_FAA);
		pack_faci(pvar, pp, faci);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_FAA, ISUP_MP_FAA, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_FRJ   0x21 - Facility reject
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) V(CAUS) O()
 *  ITUT: F(FACI) V(CAUS) O()
 */
STATIC INLINE int
isup_send_frj(queue_t *q, struct ct *ct, ulong faci, uchar *caus_ptr, size_t caus_len,
	      uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_FRJ) + size_faci(pvar)
	    + size_caus(pvar, opt_len) + 1 + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_FRJ);
		pack_faci(pvar, pp, faci);
		p = *pp;
		*pp += 2;
		*p = *pp - p;
		p++;
		pack_caus(pvar, pp, caus_ptr, caus_len);
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_FRJ, ISUP_MP_FRJ, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_FAD   0x22 - Facility Deactivated (Bellcore only)
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) O()
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_send_fad(queue_t *q, struct ct *ct, ulong faci, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_FAD)
	    + size_faci(pvar) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_FAD);
		pack_faci(pvar, pp, faci);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_FAD, ISUP_MP_FAD, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_FAI   0x23 - Facility Information (Bellcore only)
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) FAII O()
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_send_fai(queue_t *q, struct ct *ct, ulong faci, ulong faii, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_FAI) + size_faci(pvar)
	    + size_faii(pvar) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_FAI);
		pack_faci(pvar, pp, faci);
		pack_faii(pvar, pp, faii);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_FAI, ISUP_MP_FAI, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_LPA   0x24 - Loop back acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_send_lpa(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_LPA);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_LPA);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_LPA, ISUP_MP_LPA, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_DRS   0x27 - Delayed release (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O()
 */
STATIC INLINE int
isup_send_drs(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_DRS) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_DRS);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_DRS, ISUP_MP_DRS, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_PAM   0x28 - Pass along
 *  -------------------------------------------------------------------------
 *  ANSI: (encapsultated ISUP message)
 *  ITUT: (encapsultated ISUP message)
 */
STATIC INLINE int
isup_send_pam(queue_t *q, struct ct *ct, uchar *msg_ptr, size_t msg_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_DRS) + msg_len;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_DRS);
		bcopy(msg_ptr, *pp, msg_len);
		*pp += msg_len;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_PAM, ISUP_MP_PAM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_GRA   0x29 - Circuit group reset acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: V(RS)
 *  ITUT: V(RS)
 */
STATIC INLINE int
isup_send_gra(queue_t *q, struct ct *ct, uchar *rs_ptr, size_t rs_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_GRA) + size_rs(pvar, rs_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_GRA);
		p = *pp;
		*pp += 1;
		*p = *pp - p;
		p++;
		pack_rs(pvar, pp, rs_ptr, rs_len);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_GRA, ISUP_MP_GRA, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CQM   0x2a - Circuit group query
 *  -------------------------------------------------------------------------
 *  ANSI: V(RS)
 *  ITUT: V(RS)
 */
STATIC INLINE int
isup_send_cqm(queue_t *q, struct ct *ct, uchar *rs_ptr, size_t rs_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CQM) + size_rs(pvar, rs_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CQM);
		p = *pp;
		*pp += 1;
		*p = *pp - p;
		p++;
		pack_rs(pvar, pp, rs_ptr, rs_len);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CQM, ISUP_MP_CQM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CQR   0x2b - Circuit group query response
 *  -------------------------------------------------------------------------
 *  ANSI: V(RS) V(CSI)
 *  ITUT: V(RS) V(CSI)
 */
STATIC INLINE int
isup_send_cqr(queue_t *q, struct ct *ct, uchar *rs_ptr, size_t rs_len, uchar *csi_ptr,
	      size_t csi_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CQR) + size_rs(pvar,
										 rs_len) + 1 +
	    size_csi(pvar,
		     csi_len)
	    + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CQR);
		p = *pp;
		*pp += 2;
		*p = *pp - p;
		p++;
		/* 
		   RS never has a status field */
		pack_rs(pvar, pp, rs_ptr, rs_len);
		*p = *pp - p;
		p++;
		pack_csi(pvar, pp, csi_ptr, csi_len);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CQR, ISUP_MP_CQR, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CPG   0x2c - Call progress
 *  -------------------------------------------------------------------------
 *  ANSI: F(EVNT) O()
 *  ITUT: F(EVNT) O()
 */
STATIC INLINE int
isup_send_cpg(queue_t *q, struct ct *ct, ulong evnt, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CPG) + size_evnt(pvar)
	    + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CPG);
		pack_evnt(pvar, pp, evnt);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CPG, ISUP_MP_CPG, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_USR   0x2d - User-to-user information
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: V(UUI) O()
 */
STATIC INLINE int
isup_send_usr(queue_t *q, struct ct *ct, uchar *uui_ptr, size_t uui_len, uchar *opt_ptr,
	      size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_USR)
	    + size_uui(pvar, uui_len) + 1 + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_USR);
		p = *pp;
		*pp += 2;
		*p = *pp - p;
		p++;
		pack_uui(pvar, pp, uui_ptr, uui_len);
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_USR, ISUP_MP_USR, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_UCIC   0x2e - Unequipped circuit identification code
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_send_ucic(queue_t *q, struct sr *sr, ulong cic)
{
	mblk_t *mp;
	uint pvar = sr->proto.pvar;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_UCIC);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_UCIC);
		return ctrace(isup_send_msg(q, sr, ISUP_MT_UCIC, ISUP_MP_UCIC, cic, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CFN   0x2f - Confusion
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: V(CAUS) O()
 */
STATIC INLINE int
isup_send_cfn(queue_t *q, struct ct *ct, uchar *caus_ptr, size_t caus_len, uchar *opt_ptr,
	      size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CFN)
	    + size_caus(pvar, caus_len) + 1 + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CFN);
		p = *pp;
		*pp += 2;
		*p = *pp - p;
		p++;
		pack_caus(pvar, pp, caus_ptr, caus_len);
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CFN, ISUP_MP_CFN, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_OLM   0x30 - Overload
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_send_olm(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_OLM);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_OLM);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_OLM, ISUP_MP_OLM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CRG   0x31 - Charge information
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (none)
 *  SING: ICCI O()
 *  SPAN: O()
 */
STATIC INLINE int
isup_send_crg(queue_t *q, struct ct *ct, uchar *icci_ptr, size_t icci_len, uchar *opt_ptr,
	      size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CRG);
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_SING:
		mlen += size_icci(pvar, icci_len);
		/* 
		   fall thru */
	case SS7_PVAR_SPAN:
		mlen += size_opt(pvar, opt_len) + 1;
		/* 
		   fall thru */
	default:
	case SS7_PVAR_ITUT:
		break;
	}
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CRG);
		switch (pvar & SS7_PVAR_MASK) {
		case SS7_PVAR_SING:
			pack_icci(pvar, pp, icci_ptr, icci_len);
			/* 
			   fall thru */
		case SS7_PVAR_SPAN:
			p = *pp;
			*pp += 1;
			if (opt_len) {
				*p = *pp - p;
				p++;
				pack_opt(pvar, pp, opt_ptr, opt_len);
			} else
				*p++ = 0;
			/* 
			   fall thru */
		default:
		case SS7_PVAR_ITUT:
			break;
		}
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CRG, ISUP_MP_CRG, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_NRM   0x32 - Network resource management
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(MCI PCI ECI TMU)
 */
STATIC INLINE int
isup_send_nrm(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_NRM) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_NRM);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_NRM, ISUP_MP_NRM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_FAC   0x33 - Facility
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(MCI PCI ROPS SA)
 */
STATIC INLINE int
isup_send_fac(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_FAC) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_FAC);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_FAC, ISUP_MP_FAC, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_UPT   0x34 - User part test
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(PCI)
 */
STATIC INLINE int
isup_send_upt(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_UPT) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_UPT);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_UPT, ISUP_MP_UPT, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_UPA   0x35 - User part available
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(PCI)
 */
STATIC INLINE int
isup_send_upa(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_UPA) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_UPA);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_UPA, ISUP_MP_UPA, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_IDR   0x36 - Identification request
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(MCIQ MCI PCI)
 */
STATIC INLINE int
isup_send_idr(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_IDR) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_IDR);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_IDR, ISUP_MP_IDR, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_IRS   0x37 - Identification response
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(MCIR MCI PCI CGPN ATP GNUM*)
 */
STATIC INLINE int
isup_send_irs(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_IRS) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_IRS);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_IRS, ISUP_MP_IRS, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_SGM   0x38 - Segmentation
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(ATP UUI MCI GDIG* GNOT* GNUM*)
 */
STATIC INLINE int
isup_send_sgm(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_SGM) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_SGM);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_SGM, ISUP_MP_SGM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CRA   0xe9 - Circuit Reservation Acknowledgement (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_send_cra(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CRA);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CRA);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CRA, ISUP_MP_CRA, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CRM   0xea - Circuit Reservation (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: NCI
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_send_crm(queue_t *q, struct ct *ct, ulong nci)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CRM) + size_nci(pvar, nci);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CRM);
		pack_nci(pvar, pp, nci);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CRM, ISUP_MP_CRM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CVR   0xeb - Circuit Validation Response (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: CVRI CGCI O()
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_send_cvr(queue_t *q, struct ct *ct, ulong cvri, ulong cgvi, ulong cgci, uchar *opt_ptr,
	      size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CVR) + size_cvri(pvar)
	    + size_cgci(pvar) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CVR);
		pack_cvri(pvar, pp, cvri);
		pack_cgci(pvar, pp, cgci);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CVR, ISUP_MP_CVR, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CVT   0xec - Circuit Validation Test (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_send_cvt(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CVT);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CVT);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CVT, ISUP_MP_CVT, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_EXM   0xed - Exit (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: O()
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_send_exm(queue_t *q, struct ct *ct, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen =
	    size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_EXM) + size_opt(pvar, opt_len) + 1;
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar *p, **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_EXM);
		p = *pp;
		*pp += 1;
		if (opt_len) {
			*p = *pp - p;
			p++;
			pack_opt(pvar, pp, opt_ptr, opt_len);
		} else
			*p++ = 0;
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_EXM, ISUP_MP_EXM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_NON   0xf8 - National Notification (Spain)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SPAN: TON
 */
STATIC INLINE int
isup_send_non(queue_t *q, struct ct *ct, ulong ton)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_NON)
	    + size_ton(pvar, ton);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_NON);
		pack_ton(pvar, pp, ton);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_NON, ISUP_MP_NON, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_LLM   0xfc - National Malicious Call (Spain)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SPAN: (none)
 */
STATIC INLINE int
isup_send_llm(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_LLM);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_LLM);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_LLM, ISUP_MP_LLM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_CAK   0xfd - Charge Acknowledgement (Singapore)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SING: (none)
 */
STATIC INLINE int
isup_send_cak(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_CAK);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_CAK);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_CAK, ISUP_MP_CAK, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_TCM   0xfe - Tariff Charge (Singapore)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SING: CRI
 */
STATIC INLINE int
isup_send_tcm(queue_t *q, struct ct *ct, ulong cri)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_TCM)
	    + size_cri(pvar, cri);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_TCM);
		pack_cri(pvar, pp, cri);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_TCM, ISUP_MP_TCM, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  ISUP_MT_MCP   0xff - Malicious Call Print (Singapore)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SING: (none)
 */
STATIC INLINE int
isup_send_mcp(queue_t *q, struct ct *ct)
{
	mblk_t *mp;
	uint pvar = ct->tg.tg->proto.pvar;
	uint cic = ct->cic;
	size_t mlen = size_cic(pvar, cic) + size_mt(pvar, ISUP_MT_MCP);
	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		uchar **pp = &mp->b_wptr;
		mp->b_datap->db_type = M_DATA;
		pack_cic(pvar, pp, cic);
		pack_mt(pvar, pp, ISUP_MT_MCP);
		return ctrace(isup_send_msg(q, ct->sr.sr, ISUP_MT_MCP, ISUP_MP_MCP, ct->sls, mp));
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE FUNCTIONS
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
enum { tall =
	    0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19,
	t20, t21,
	t22, t23, t24, t25, t26, t27, t28, t29, t30, t31, t32, t33, t34, t35, t36, t37, t38, tacc_r,
	tccr,
	tccr_r, tcgb, tcra, tcrm, tcvt, texm_d, tgrs, thga, tscga, tscga_d
};

#define ISUP_DECLARE_TIMER(__o,__t) \
STATIC INLINE int __o ## _ ## __t ## _timeout(__o ## _t *); \
STATIC void __o ## _ ## __t ## _expiry(caddr_t data) \
{ \
	__o ## _do_timeout(data, # __t, &((__o ## _t *)data)->timers. __t, &__o ## _ ## __t ## _timeout, & __o ## _ ## __t ## _expiry); \
} \

STATIC isup_opt_conf_cg_t itu_cg_config_defaults = {
};
STATIC isup_opt_conf_tg_t itu_tg_config_defaults = {
	t1:(15 * HZ),			/* T1 15-60 seconds */
	t2:(3 * 60 * HZ),		/* T2 3 minutes */
	t3:(2 * 60 * HZ),		/* T3 2 minutes */
	t5:(5 * 60 * HZ),		/* T5 5-15 minutes */
	t6:(10 * HZ),			/* T6 10-32 seconds (specified in Q.118) */
	t7:(20 * HZ),			/* T7 20-30 seconds */
	t8:(10 * HZ),			/* T8 10-15 seconds */
	t9:(2 * 60 * HZ),		/* T9 2-4 minutes (specified in Q.118) */
	t10:(4 * HZ),			/* T10 4-6 seconds */
	t11:(15 * HZ),			/* T11 15-20 seconds */
	t12:(15 * HZ),			/* T12 15-60 seconds */
	t13:(5 * 60 * HZ),		/* T13 5-15 minutes */
	t14:(15 * HZ),			/* T14 15-60 seconds */
	t15:(5 * 60 * HZ),		/* T15 5-15 minutes */
	t16:(15 * HZ),			/* T16 15-60 seconds */
	t17:(5 * 60 * HZ),		/* T17 5-15 minutes */
	t24:(1 * HZ),			/* T24 < 2 seconds */
	t25:(1 * HZ),			/* T25 1-10 seconds */
	t26:(1 * 60 * HZ),		/* T26 1-3 minutes */
	t27:(4 * 60 * HZ),		/* T27 4 minutes */
	t31:(7 * 60 * HZ),		/* T31 > 6 minutes */
	t32:(3 * HZ),			/* T32 3-5 seconds */
	t33:(12 * HZ),			/* T33 12-15 seconds */
	t34:(12 * HZ),			/* T34 12-15 seconds */
	t35:(15 * HZ),			/* T35 15-20 seconds */
	t36:(15 * HZ),			/* T36 15-20 seconds */
	t37:(2 * HZ),			/* T37 2-4 seconds */
	t38:(15 * HZ),			/* T38 15-20 seconds */
	tacc_r:(0 * HZ),		/* TACC,r (not specified for ITU networks) */
	tccr:(0 * HZ),			/* TCCR (not specified for ITU networks) */
	tccr_r:(0 * HZ),		/* TCCR,r (not specified for ITU networks) */
	tcra:(0 * HZ),			/* TCRA (not specified for ITU networks) */
	tcrm:(0 * HZ),			/* TCRM (not specified for ITU networks) */
	tcvt:(0 * HZ),			/* TCVT (not specified for ITU networks) */
	texm_d:(0 * HZ),		/* TEXM,d (not specified for ITU networks) */
};
STATIC isup_opt_conf_sr_t itu_sr_config_defaults = {
	t4:(5 * 60 * HZ),		/* T4 5-15 minutes */
	t18:(15 * HZ),			/* T18 15-60 seconds */
	t19:(5 * 60 * HZ),		/* T19 5-15 minutes */
	t20:(15 * HZ),			/* T20 15-60 seconds */
	t21:(5 * 60 * HZ),		/* T21 5-15 minutes */
	t22:(15 * HZ),			/* T22 15-60 seconds */
	t23:(5 * 60 * HZ),		/* T23 5-15 minutes */
	t28:(10 * HZ),			/* T28 10 seconds */
	t29:(300 * HZ / 1000),		/* T29 300-600 milliseconds */
	t30:(5 * HZ),			/* T30 5-10 seconds */
	tcgb:(0 * HZ),			/* TCGB (not specified for ITU networks) */
	tgrs:(0 * HZ),			/* TGRS (not specified for ITU networks) */
	thga:(0 * HZ),			/* THGA (not specified for ITU networks) */
	tscga:(0 * HZ),			/* TSCGA (not specified for ITU networks) */
	tscga_d:(0 * HZ),		/* TSCGA,d (not specified for ITU networks) */
};

STATIC isup_opt_conf_cg_t ansi_cg_config_defaults = {
};
STATIC isup_opt_conf_tg_t ansi_tg_config_defaults = {
	t1:(4 * HZ),			/* T1 4-15 seconds */
	t2:(0 * HZ),			/* T2 (not specified for US networks) */
	t3:(0 * HZ),			/* T3 (not specified for US networks) */
	t5:(1 * 60 * HZ),		/* T5 1 minute */
	t6:(10 * HZ),			/* T6 10-32 seconds */
	t7:(20 * HZ),			/* T7 20-30 seconds */
	t8:(10 * HZ),			/* T8 10-15 seconds */
	t9:(2 * 60 * HZ),		/* T9 2-4 minutes (specified in Q.118) */
	t10:(0 * HZ),			/* T10 (not specified for US networks) */
	t11:(15 * HZ),			/* T11 15-20 seconds */
	t12:(4 * HZ),			/* T12 4-15 seconds */
	t13:(1 * 60 * HZ),		/* T13 1 minute */
	t14:(4 * HZ),			/* T14 4-15 seconds */
	t15:(1 * 60 * HZ),		/* T15 1 minute */
	t16:(4 * HZ),			/* T16 4-15 seconds */
	t17:(1 * 60 * HZ),		/* T17 1 minutes */
	t24:(1 * HZ),			/* T24 < 2 seconds */
	t25:(1 * HZ),			/* T25 1-10 seconds */
	t26:(1 * 60 * HZ),		/* T26 1-3 minutes */
	t27:(4 * 60 * HZ),		/* T27 > 3 minutes */
	t31:(7 * 60 * HZ),		/* T31 > 6 minutes */
	t32:(3 * HZ),			/* T32 3-5 seconds */
	t33:(12 * HZ),			/* T33 12-15 seconds */
	t34:(10 * HZ),			/* T34 10-15 seconds */
	t35:(0 * HZ),			/* T35 (not specified for US networks) */
	t36:(2 * HZ),			/* T36 2-4 seconds */
	t37:(2 * HZ),			/* T37 2-4 seconds */
	t38:(0 * HZ),			/* T38 (not specified for US networks) */
	tacc_r:(5 * HZ),		/* TACC,r 5 seconds */
	tccr:(2 * HZ),			/* TCCR 2 seconds */
	tccr_r:(20 * HZ),		/* TCCR,r 20 seconds */
	tcra:(20 * HZ),			/* TCRA 20 seconds */
	tcrm:(3 * HZ),			/* TCRM 3-4 seconds */
	tcvt:(10 * HZ),			/* TCVT 10 seconds */
	texm_d:(30 * HZ),		/* TEXM,d (network dependent) */
};
STATIC isup_opt_conf_sr_t ansi_sr_config_defaults = {
	t4:(0 * HZ),			/* T4 (not specified for US networks) */
	t18:(4 * HZ),			/* T18 4-15 seconds */
	t19:(1 * 60 * HZ),		/* T19 1 minute */
	t20:(4 * HZ),			/* T20 4-15 seconds */
	t21:(1 * 60 * HZ),		/* T21 1 minute */
	t22:(4 * HZ),			/* T22 4-15 seconds */
	t23:(1 * 60 * HZ),		/* T23 1 minute */
	t28:(10 * HZ),			/* T28 10 seconds */
	t29:(0 * HZ),			/* T29 (not specified for US networks) */
	t30:(0 * HZ),			/* T30 (not specified for US networks) */
	tcgb:(5 * HZ),			/* TCGB 5 seconds */
	tgrs:(5 * HZ),			/* TGRS 5 seconds */
	thga:(1 * 60 * HZ),		/* THGA 0-5 minutes */
	tscga:(1 * 60 * HZ),		/* TSCGA 0-2 minutes */
	tscga_d:(5 * HZ),		/* TSCGA,d 5-120 seconds */
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Circuit timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
ct_do_timeout(caddr_t data, const char *timer, ulong *timeo, int (to_fnc) (struct ct *),
	      void (*exp_fnc) (caddr_t))
{
	struct ct *ct = (struct ct *) data;
	if (xchg(timeo, 0)) {
		if (spin_trylock(&ct->lock)) {
			printd(("%s: %p: %s timeout at %lu\n", DRV_NAME, ct, timer, jiffies));
			switch (to_fnc(ct)) {
			default:
			case QR_DONE:
				ct_put(ct);
				spin_unlock(&ct->lock);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&ct->lock);
		} else
			printd(("%s: %p: %s timeout collision at %lu\n", DRV_NAME, ct, timer,
				jiffies));
		/* 
		   back off timer one tick */
		*timeo = timeout(exp_fnc, data, 100);
	}
}

STATIC int ct_t1_timeout(struct ct *);
STATIC void
ct_t1_expiry(caddr_t data)
{
	ct_do_timeout(data, "t1", &((struct ct *) data)->timers.t1, &ct_t1_timeout, &ct_t1_expiry);
};

STATIC int ct_t2_timeout(struct ct *);
STATIC void
ct_t2_expiry(caddr_t data)
{
	ct_do_timeout(data, "t2", &((struct ct *) data)->timers.t2, &ct_t2_timeout, &ct_t2_expiry);
};

STATIC int ct_t3_timeout(struct ct *);
STATIC void
ct_t3_expiry(caddr_t data)
{
	ct_do_timeout(data, "t3", &((struct ct *) data)->timers.t3, &ct_t3_timeout, &ct_t3_expiry);
};

STATIC int ct_t5_timeout(struct ct *);
STATIC void
ct_t5_expiry(caddr_t data)
{
	ct_do_timeout(data, "t5", &((struct ct *) data)->timers.t5, &ct_t5_timeout, &ct_t5_expiry);
};

STATIC int ct_t6_timeout(struct ct *);
STATIC void
ct_t6_expiry(caddr_t data)
{
	ct_do_timeout(data, "t6", &((struct ct *) data)->timers.t6, &ct_t6_timeout, &ct_t6_expiry);
};

STATIC int ct_t7_timeout(struct ct *);
STATIC void
ct_t7_expiry(caddr_t data)
{
	ct_do_timeout(data, "t7", &((struct ct *) data)->timers.t7, &ct_t7_timeout, &ct_t7_expiry);
};

STATIC int ct_t8_timeout(struct ct *);
STATIC void
ct_t8_expiry(caddr_t data)
{
	ct_do_timeout(data, "t8", &((struct ct *) data)->timers.t8, &ct_t8_timeout, &ct_t8_expiry);
};

STATIC int ct_t9_timeout(struct ct *);
STATIC void
ct_t9_expiry(caddr_t data)
{
	ct_do_timeout(data, "t9", &((struct ct *) data)->timers.t9, &ct_t9_timeout, &ct_t9_expiry);
};

STATIC int ct_t10_timeout(struct ct *);
STATIC void
ct_t10_expiry(caddr_t data)
{
	ct_do_timeout(data, "t10", &((struct ct *) data)->timers.t10, &ct_t10_timeout,
		      &ct_t10_expiry);
};

STATIC int ct_t11_timeout(struct ct *);
STATIC void
ct_t11_expiry(caddr_t data)
{
	ct_do_timeout(data, "t11", &((struct ct *) data)->timers.t11, &ct_t11_timeout,
		      &ct_t11_expiry);
};

STATIC int ct_t12_timeout(struct ct *);
STATIC void
ct_t12_expiry(caddr_t data)
{
	ct_do_timeout(data, "t12", &((struct ct *) data)->timers.t12, &ct_t12_timeout,
		      &ct_t12_expiry);
};

STATIC int ct_t13_timeout(struct ct *);
STATIC void
ct_t13_expiry(caddr_t data)
{
	ct_do_timeout(data, "t13", &((struct ct *) data)->timers.t13, &ct_t13_timeout,
		      &ct_t13_expiry);
};

STATIC int ct_t14_timeout(struct ct *);
STATIC void
ct_t14_expiry(caddr_t data)
{
	ct_do_timeout(data, "t14", &((struct ct *) data)->timers.t14, &ct_t14_timeout,
		      &ct_t14_expiry);
};

STATIC int ct_t15_timeout(struct ct *);
STATIC void
ct_t15_expiry(caddr_t data)
{
	ct_do_timeout(data, "t15", &((struct ct *) data)->timers.t15, &ct_t15_timeout,
		      &ct_t15_expiry);
};

STATIC int ct_t16_timeout(struct ct *);
STATIC void
ct_t16_expiry(caddr_t data)
{
	ct_do_timeout(data, "t16", &((struct ct *) data)->timers.t16, &ct_t16_timeout,
		      &ct_t16_expiry);
};

STATIC int ct_t17_timeout(struct ct *);
STATIC void
ct_t17_expiry(caddr_t data)
{
	ct_do_timeout(data, "t17", &((struct ct *) data)->timers.t17, &ct_t17_timeout,
		      &ct_t17_expiry);
};

STATIC int ct_t24_timeout(struct ct *);
STATIC void
ct_t24_expiry(caddr_t data)
{
	ct_do_timeout(data, "t24", &((struct ct *) data)->timers.t24, &ct_t24_timeout,
		      &ct_t24_expiry);
};

STATIC int ct_t25_timeout(struct ct *);
STATIC void
ct_t25_expiry(caddr_t data)
{
	ct_do_timeout(data, "t25", &((struct ct *) data)->timers.t25, &ct_t25_timeout,
		      &ct_t25_expiry);
};

STATIC int ct_t26_timeout(struct ct *);
STATIC void
ct_t26_expiry(caddr_t data)
{
	ct_do_timeout(data, "t26", &((struct ct *) data)->timers.t26, &ct_t26_timeout,
		      &ct_t26_expiry);
};

STATIC int ct_t27_timeout(struct ct *);
STATIC void
ct_t27_expiry(caddr_t data)
{
	ct_do_timeout(data, "t27", &((struct ct *) data)->timers.t27, &ct_t27_timeout,
		      &ct_t27_expiry);
};

STATIC int ct_t31_timeout(struct ct *);
STATIC void
ct_t31_expiry(caddr_t data)
{
	ct_do_timeout(data, "t31", &((struct ct *) data)->timers.t31, &ct_t31_timeout,
		      &ct_t31_expiry);
};

STATIC int ct_t32_timeout(struct ct *);
STATIC void
ct_t32_expiry(caddr_t data)
{
	ct_do_timeout(data, "t32", &((struct ct *) data)->timers.t32, &ct_t32_timeout,
		      &ct_t32_expiry);
};

STATIC int ct_t33_timeout(struct ct *);
STATIC void
ct_t33_expiry(caddr_t data)
{
	ct_do_timeout(data, "t33", &((struct ct *) data)->timers.t33, &ct_t33_timeout,
		      &ct_t33_expiry);
};

STATIC int ct_t34_timeout(struct ct *);
STATIC void
ct_t34_expiry(caddr_t data)
{
	ct_do_timeout(data, "t34", &((struct ct *) data)->timers.t34, &ct_t34_timeout,
		      &ct_t34_expiry);
};

STATIC int ct_t35_timeout(struct ct *);
STATIC void
ct_t35_expiry(caddr_t data)
{
	ct_do_timeout(data, "t35", &((struct ct *) data)->timers.t35, &ct_t35_timeout,
		      &ct_t35_expiry);
};

STATIC int ct_t36_timeout(struct ct *);
STATIC void
ct_t36_expiry(caddr_t data)
{
	ct_do_timeout(data, "t36", &((struct ct *) data)->timers.t36, &ct_t36_timeout,
		      &ct_t36_expiry);
};

STATIC int ct_t37_timeout(struct ct *);
STATIC void
ct_t37_expiry(caddr_t data)
{
	ct_do_timeout(data, "t37", &((struct ct *) data)->timers.t37, &ct_t37_timeout,
		      &ct_t37_expiry);
};

STATIC int ct_t38_timeout(struct ct *);
STATIC void
ct_t38_expiry(caddr_t data)
{
	ct_do_timeout(data, "t38", &((struct ct *) data)->timers.t38, &ct_t38_timeout,
		      &ct_t38_expiry);
};

STATIC int ct_tacc_r_timeout(struct ct *);
STATIC void
ct_tacc_r_expiry(caddr_t data)
{
	ct_do_timeout(data, "tacc_r", &((struct ct *) data)->timers.tacc_r, &ct_tacc_r_timeout,
		      &ct_tacc_r_expiry);
};

STATIC int ct_tccr_r_timeout(struct ct *);
STATIC void
ct_tccr_r_expiry(caddr_t data)
{
	ct_do_timeout(data, "tccr_r", &((struct ct *) data)->timers.tccr_r, &ct_tccr_r_timeout,
		      &ct_tccr_r_expiry);
};

STATIC int ct_tccr_timeout(struct ct *);
STATIC void
ct_tccr_expiry(caddr_t data)
{
	ct_do_timeout(data, "tccr", &((struct ct *) data)->timers.tccr, &ct_tccr_timeout,
		      &ct_tccr_expiry);
};

STATIC int ct_tcra_timeout(struct ct *);
STATIC void
ct_tcra_expiry(caddr_t data)
{
	ct_do_timeout(data, "tcra", &((struct ct *) data)->timers.tcra, &ct_tcra_timeout,
		      &ct_tcra_expiry);
};

STATIC int ct_tcrm_timeout(struct ct *);
STATIC void
ct_tcrm_expiry(caddr_t data)
{
	ct_do_timeout(data, "tcrm", &((struct ct *) data)->timers.tcrm, &ct_tcrm_timeout,
		      &ct_tcrm_expiry);
};

STATIC int ct_tcvt_timeout(struct ct *);
STATIC void
ct_tcvt_expiry(caddr_t data)
{
	ct_do_timeout(data, "tcvt", &((struct ct *) data)->timers.tcvt, &ct_tcvt_timeout,
		      &ct_tcvt_expiry);
};

STATIC int ct_texm_d_timeout(struct ct *);
STATIC void
ct_texm_d_expiry(caddr_t data)
{
	ct_do_timeout(data, "texm_d", &((struct ct *) data)->timers.texm_d, &ct_texm_d_timeout,
		      &ct_texm_d_expiry);
};

STATIC INLINE void
__ct_timer_stop(struct ct *ct, const uint t)
{
	int single = 1;
	ulong to;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t1:
		if ((to = xchg(&ct->timers.t1, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t1 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t2:
		if ((to = xchg(&ct->timers.t2, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t2 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t3:
		if ((to = xchg(&ct->timers.t3, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t3 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t5:
		if ((to = xchg(&ct->timers.t5, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t5 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t6:
		if ((to = xchg(&ct->timers.t6, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t6 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t7:
		if ((to = xchg(&ct->timers.t7, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t7 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t8:
		if ((to = xchg(&ct->timers.t8, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t8 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t9:
		if ((to = xchg(&ct->timers.t9, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t9 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t10:
		if ((to = xchg(&ct->timers.t10, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t10 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t11:
		if ((to = xchg(&ct->timers.t11, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t11 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t24:
		if ((to = xchg(&ct->timers.t24, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t24 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t31:
		if ((to = xchg(&ct->timers.t31, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t31 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t32:
		if ((to = xchg(&ct->timers.t32, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t32 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t33:
		if ((to = xchg(&ct->timers.t33, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t33 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t34:
		if ((to = xchg(&ct->timers.t34, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t34 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t35:
		if ((to = xchg(&ct->timers.t35, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t35 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t38:
		if ((to = xchg(&ct->timers.t38, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t38 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tacc_r:
		if ((to = xchg(&ct->timers.tacc_r, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tacc_r at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tcra:
		if ((to = xchg(&ct->timers.tcra, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tcra at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tcrm:
		if ((to = xchg(&ct->timers.tcrm, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tcrm at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case texm_d:
		if ((to = xchg(&ct->timers.texm_d, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping texm_d at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t12:
		if ((to = xchg(&ct->timers.t12, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t12 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t13:
		if ((to = xchg(&ct->timers.t13, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t13 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t14:
		if ((to = xchg(&ct->timers.t14, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t14 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t15:
		if ((to = xchg(&ct->timers.t15, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t15 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t16:
		if ((to = xchg(&ct->timers.t16, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t16 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t17:
		if ((to = xchg(&ct->timers.t17, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t17 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t25:
		if ((to = xchg(&ct->timers.t25, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t25 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t26:
		if ((to = xchg(&ct->timers.t26, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t26 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t27:
		if ((to = xchg(&ct->timers.t27, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t27 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t36:
		if ((to = xchg(&ct->timers.t36, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t36 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t37:
		if ((to = xchg(&ct->timers.t37, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t37 at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tccr:
		if ((to = xchg(&ct->timers.tccr, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tccr at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tccr_r:
		if ((to = xchg(&ct->timers.tccr_r, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tccr_r at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tcvt:
		if ((to = xchg(&ct->timers.tcvt, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tcvt at %lu\n", DRV_NAME, ct, jiffies));
			ct_put(ct);
		}
		if (single)
			break;
		/* 
		   fall through */
	}
}

STATIC INLINE void
ct_timer_stop(struct ct *ct, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&ct->lock, flags);
	{
		__ct_timer_stop(ct, t);
	}
	spin_unlock_irqrestore(&ct->lock, flags);
}

STATIC INLINE void
ct_timer_start(struct ct *ct, const uint t)
{
	psw_t flags;
	struct tg *tg = ct->tg.tg;
	ensure(tg, return);
	spin_lock_irqsave(&ct->lock, flags);
	{
		__ct_timer_stop(ct, t);
		switch (t) {
		case t1:
			printd(("%s: %p: starting t1 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t1 * 1000 / HZ, jiffies));
			ct->timers.t1 = timeout(&ct_t1_expiry, (caddr_t) ct_get(ct), tg->config.t1);
			break;
		case t2:
			printd(("%s: %p: starting t2 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t2 * 1000 / HZ, jiffies));
			ct->timers.t2 = timeout(&ct_t2_expiry, (caddr_t) ct_get(ct), tg->config.t2);
			break;
		case t3:
			printd(("%s: %p: starting t3 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t3 * 1000 / HZ, jiffies));
			ct->timers.t3 = timeout(&ct_t3_expiry, (caddr_t) ct_get(ct), tg->config.t3);
			break;
		case t5:
			printd(("%s: %p: starting t5 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t5 * 1000 / HZ, jiffies));
			ct->timers.t5 = timeout(&ct_t5_expiry, (caddr_t) ct_get(ct), tg->config.t5);
			break;
		case t6:
			printd(("%s: %p: starting t6 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t6 * 1000 / HZ, jiffies));
			ct->timers.t6 = timeout(&ct_t6_expiry, (caddr_t) ct_get(ct), tg->config.t6);
			break;
		case t7:
			printd(("%s: %p: starting t7 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t7 * 1000 / HZ, jiffies));
			ct->timers.t7 = timeout(&ct_t7_expiry, (caddr_t) ct_get(ct), tg->config.t7);
			break;
		case t8:
			printd(("%s: %p: starting t8 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t8 * 1000 / HZ, jiffies));
			ct->timers.t8 = timeout(&ct_t8_expiry, (caddr_t) ct_get(ct), tg->config.t8);
			break;
		case t9:
			printd(("%s: %p: starting t9 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t9 * 1000 / HZ, jiffies));
			ct->timers.t9 = timeout(&ct_t9_expiry, (caddr_t) ct_get(ct), tg->config.t9);
			break;
		case t10:
			printd(("%s: %p: starting t10 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t10 * 1000 / HZ, jiffies));
			ct->timers.t10 =
			    timeout(&ct_t10_expiry, (caddr_t) ct_get(ct), tg->config.t10);
			break;
		case t11:
			printd(("%s: %p: starting t11 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t11 * 1000 / HZ, jiffies));
			ct->timers.t11 =
			    timeout(&ct_t11_expiry, (caddr_t) ct_get(ct), tg->config.t11);
			break;
		case t24:
			printd(("%s: %p: starting t24 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t24 * 1000 / HZ, jiffies));
			ct->timers.t24 =
			    timeout(&ct_t24_expiry, (caddr_t) ct_get(ct), tg->config.t24);
			break;
		case t31:
			printd(("%s: %p: starting t31 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t31 * 1000 / HZ, jiffies));
			ct->timers.t31 =
			    timeout(&ct_t31_expiry, (caddr_t) ct_get(ct), tg->config.t31);
			break;
		case t32:
			printd(("%s: %p: starting t32 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t32 * 1000 / HZ, jiffies));
			ct->timers.t32 =
			    timeout(&ct_t32_expiry, (caddr_t) ct_get(ct), tg->config.t32);
			break;
		case t33:
			printd(("%s: %p: starting t33 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t33 * 1000 / HZ, jiffies));
			ct->timers.t33 =
			    timeout(&ct_t33_expiry, (caddr_t) ct_get(ct), tg->config.t33);
			break;
		case t34:
			printd(("%s: %p: starting t34 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t34 * 1000 / HZ, jiffies));
			ct->timers.t34 =
			    timeout(&ct_t34_expiry, (caddr_t) ct_get(ct), tg->config.t34);
			break;
		case t35:
			printd(("%s: %p: starting t35 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t35 * 1000 / HZ, jiffies));
			ct->timers.t35 =
			    timeout(&ct_t35_expiry, (caddr_t) ct_get(ct), tg->config.t35);
			break;
		case t38:
			printd(("%s: %p: starting t38 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t38 * 1000 / HZ, jiffies));
			ct->timers.t38 =
			    timeout(&ct_t38_expiry, (caddr_t) ct_get(ct), tg->config.t38);
			break;
		case tacc_r:
			printd(("%s: %p: starting tacc_r %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.tacc_r * 1000 / HZ, jiffies));
			ct->timers.tacc_r =
			    timeout(&ct_tacc_r_expiry, (caddr_t) ct_get(ct), tg->config.tacc_r);
			break;
		case tcra:
			printd(("%s: %p: starting tcra %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.tcra * 1000 / HZ, jiffies));
			ct->timers.tcra =
			    timeout(&ct_tcra_expiry, (caddr_t) ct_get(ct), tg->config.tcra);
			break;
		case tcrm:
			printd(("%s: %p: starting tcrm %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.tcrm * 1000 / HZ, jiffies));
			ct->timers.tcrm =
			    timeout(&ct_tcrm_expiry, (caddr_t) ct_get(ct), tg->config.tcrm);
			break;
		case texm_d:
			printd(("%s: %p: starting texm_d %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.texm_d * 1000 / HZ, jiffies));
			ct->timers.texm_d =
			    timeout(&ct_texm_d_expiry, (caddr_t) ct_get(ct), tg->config.texm_d);
			break;
		case t12:
			printd(("%s: %p: starting t12 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t12 * 1000 / HZ, jiffies));
			ct->timers.t12 =
			    timeout(&ct_t12_expiry, (caddr_t) ct_get(ct), tg->config.t12);
			break;
		case t13:
			printd(("%s: %p: starting t13 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t13 * 1000 / HZ, jiffies));
			ct->timers.t13 =
			    timeout(&ct_t13_expiry, (caddr_t) ct_get(ct), tg->config.t13);
			break;
		case t14:
			printd(("%s: %p: starting t14 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t14 * 1000 / HZ, jiffies));
			ct->timers.t14 =
			    timeout(&ct_t14_expiry, (caddr_t) ct_get(ct), tg->config.t14);
			break;
		case t15:
			printd(("%s: %p: starting t15 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t15 * 1000 / HZ, jiffies));
			ct->timers.t15 =
			    timeout(&ct_t15_expiry, (caddr_t) ct_get(ct), tg->config.t15);
			break;
		case t16:
			printd(("%s: %p: starting t16 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t16 * 1000 / HZ, jiffies));
			ct->timers.t16 =
			    timeout(&ct_t16_expiry, (caddr_t) ct_get(ct), tg->config.t16);
			break;
		case t17:
			printd(("%s: %p: starting t17 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t17 * 1000 / HZ, jiffies));
			ct->timers.t17 =
			    timeout(&ct_t17_expiry, (caddr_t) ct_get(ct), tg->config.t17);
			break;
		case t25:
			printd(("%s: %p: starting t25 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t25 * 1000 / HZ, jiffies));
			ct->timers.t25 =
			    timeout(&ct_t25_expiry, (caddr_t) ct_get(ct), tg->config.t25);
			break;
		case t26:
			printd(("%s: %p: starting t26 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t26 * 1000 / HZ, jiffies));
			ct->timers.t26 =
			    timeout(&ct_t26_expiry, (caddr_t) ct_get(ct), tg->config.t26);
			break;
		case t27:
			printd(("%s: %p: starting t27 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t27 * 1000 / HZ, jiffies));
			ct->timers.t27 =
			    timeout(&ct_t27_expiry, (caddr_t) ct_get(ct), tg->config.t27);
			break;
		case t36:
			printd(("%s: %p: starting t36 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t36 * 1000 / HZ, jiffies));
			ct->timers.t36 =
			    timeout(&ct_t36_expiry, (caddr_t) ct_get(ct), tg->config.t36);
			break;
		case t37:
			printd(("%s: %p: starting t37 %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.t37 * 1000 / HZ, jiffies));
			ct->timers.t37 =
			    timeout(&ct_t37_expiry, (caddr_t) ct_get(ct), tg->config.t37);
			break;
		case tccr:
			printd(("%s: %p: starting tccr %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.tccr * 1000 / HZ, jiffies));
			ct->timers.tccr =
			    timeout(&ct_tccr_expiry, (caddr_t) ct_get(ct), tg->config.tccr);
			break;
		case tccr_r:
			printd(("%s: %p: starting tccr_r %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.tccr_r * 1000 / HZ, jiffies));
			ct->timers.tccr_r =
			    timeout(&ct_tccr_r_expiry, (caddr_t) ct_get(ct), tg->config.tccr_r);
			break;
		case tcvt:
			printd(("%s: %p: starting tcvt %lu ms at %lu\n", DRV_NAME, ct,
				tg->config.tcvt * 1000 / HZ, jiffies));
			ct->timers.tcvt =
			    timeout(&ct_tcvt_expiry, (caddr_t) ct_get(ct), tg->config.tcvt);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&ct->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CG timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
cg_do_timeout(caddr_t data, const char *timer, ulong *timeo, int (to_fnc) (struct cg *),
	      void (*exp_fnc) (caddr_t))
{
	struct cg *cg = (struct cg *) data;
	if (xchg(timeo, 0)) {
		if (spin_trylock(&cg->lock)) {
			printd(("%s: %p: %s timeout at %lu\n", DRV_NAME, cg, timer, jiffies));
			switch (to_fnc(cg)) {
			default:
			case QR_DONE:
				cg_put(cg);
				spin_unlock(&cg->lock);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&cg->lock);
		} else
			printd(("%s: %p: %s timeout collision at %lu\n", DRV_NAME, cg, timer,
				jiffies));
		/* 
		   back off timer one tick */
		*timeo = timeout(exp_fnc, data, 100);
	}
}

STATIC int cg_t18_timeout(struct cg *);
STATIC void
cg_t18_expiry(caddr_t data)
{
	cg_do_timeout(data, "t18", &((struct cg *) data)->timers.t18, &cg_t18_timeout,
		      &cg_t18_expiry);
};

STATIC int cg_t19_timeout(struct cg *);
STATIC void
cg_t19_expiry(caddr_t data)
{
	cg_do_timeout(data, "t19", &((struct cg *) data)->timers.t19, &cg_t19_timeout,
		      &cg_t19_expiry);
};

STATIC int cg_t20_timeout(struct cg *);
STATIC void
cg_t20_expiry(caddr_t data)
{
	cg_do_timeout(data, "t20", &((struct cg *) data)->timers.t20, &cg_t20_timeout,
		      &cg_t20_expiry);
};

STATIC int cg_t21_timeout(struct cg *);
STATIC void
cg_t21_expiry(caddr_t data)
{
	cg_do_timeout(data, "t21", &((struct cg *) data)->timers.t21, &cg_t21_timeout,
		      &cg_t21_expiry);
};

STATIC int cg_t22_timeout(struct cg *);
STATIC void
cg_t22_expiry(caddr_t data)
{
	cg_do_timeout(data, "t22", &((struct cg *) data)->timers.t22, &cg_t22_timeout,
		      &cg_t22_expiry);
};

STATIC int cg_t23_timeout(struct cg *);
STATIC void
cg_t23_expiry(caddr_t data)
{
	cg_do_timeout(data, "t23", &((struct cg *) data)->timers.t23, &cg_t23_timeout,
		      &cg_t23_expiry);
};

STATIC int cg_t28_timeout(struct cg *);
STATIC void
cg_t28_expiry(caddr_t data)
{
	cg_do_timeout(data, "t28", &((struct cg *) data)->timers.t28, &cg_t28_timeout,
		      &cg_t28_expiry);
};

STATIC int cg_tcgb_timeout(struct cg *);
STATIC void
cg_tcgb_expiry(caddr_t data)
{
	cg_do_timeout(data, "tcgb", &((struct cg *) data)->timers.tcgb, &cg_tcgb_timeout,
		      &cg_tcgb_expiry);
};

STATIC int cg_tgrs_timeout(struct cg *);
STATIC void
cg_tgrs_expiry(caddr_t data)
{
	cg_do_timeout(data, "tgrs", &((struct cg *) data)->timers.tgrs, &cg_tgrs_timeout,
		      &cg_tgrs_expiry);
};

STATIC int cg_thga_timeout(struct cg *);
STATIC void
cg_thga_expiry(caddr_t data)
{
	cg_do_timeout(data, "thga", &((struct cg *) data)->timers.thga, &cg_thga_timeout,
		      &cg_thga_expiry);
};

STATIC int cg_tscga_timeout(struct cg *);
STATIC void
cg_tscga_expiry(caddr_t data)
{
	cg_do_timeout(data, "tscga", &((struct cg *) data)->timers.tscga, &cg_tscga_timeout,
		      &cg_tscga_expiry);
};

STATIC int cg_tscga_d_timeout(struct cg *);
STATIC void
cg_tscga_d_expiry(caddr_t data)
{
	cg_do_timeout(data, "tscga_d", &((struct cg *) data)->timers.tscga_d, &cg_tscga_d_timeout,
		      &cg_tscga_d_expiry);
};

STATIC INLINE void
__cg_timer_stop(struct cg *cg, const uint t)
{
	int single = 1;
	ulong to;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t18:
		if ((to = xchg(&cg->timers.t18, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t18 at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t19:
		if ((to = xchg(&cg->timers.t19, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t19 at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t20:
		if ((to = xchg(&cg->timers.t20, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t20 at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t21:
		if ((to = xchg(&cg->timers.t21, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t21 at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t22:
		if ((to = xchg(&cg->timers.t22, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t22 at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t23:
		if ((to = xchg(&cg->timers.t23, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t23 at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case t28:
		if ((to = xchg(&cg->timers.t28, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t28 at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tcgb:
		if ((to = xchg(&cg->timers.tcgb, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tcgb at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tgrs:
		if ((to = xchg(&cg->timers.tgrs, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tgrs at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case thga:
		if ((to = xchg(&cg->timers.thga, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping thga at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tscga:
		if ((to = xchg(&cg->timers.tscga, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tscga at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	case tscga_d:
		if ((to = xchg(&cg->timers.tscga_d, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping tscga_d at %lu\n", DRV_NAME, cg, jiffies));
			cg_put(cg);
		}
		if (single)
			break;
		/* 
		   fall through */
	}
}

STATIC INLINE void
cg_timer_stop(struct cg *cg, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&cg->lock, flags);
	{
		__cg_timer_stop(cg, t);
	}
	spin_unlock_irqrestore(&cg->lock, flags);
}

STATIC INLINE void
cg_timer_start(struct cg *cg, const uint t)
{
	psw_t flags;
	struct sr *sr = cg->sr.sr;
	spin_lock_irqsave(&cg->lock, flags);
	{
		__cg_timer_stop(cg, t);
		switch (t) {
		case t18:
			printd(("%s: %p: starting t18 %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.t18 * 1000 / HZ, jiffies));
			cg->timers.t18 =
			    timeout(&cg_t18_expiry, (caddr_t) cg_get(cg), sr->config.t18);
			break;
		case t19:
			printd(("%s: %p: starting t19 %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.t19 * 1000 / HZ, jiffies));
			cg->timers.t19 =
			    timeout(&cg_t19_expiry, (caddr_t) cg_get(cg), sr->config.t19);
			break;
		case t20:
			printd(("%s: %p: starting t20 %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.t20 * 1000 / HZ, jiffies));
			cg->timers.t20 =
			    timeout(&cg_t20_expiry, (caddr_t) cg_get(cg), sr->config.t20);
			break;
		case t21:
			printd(("%s: %p: starting t21 %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.t21 * 1000 / HZ, jiffies));
			cg->timers.t21 =
			    timeout(&cg_t21_expiry, (caddr_t) cg_get(cg), sr->config.t21);
			break;
		case t22:
			printd(("%s: %p: starting t22 %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.t22 * 1000 / HZ, jiffies));
			cg->timers.t22 =
			    timeout(&cg_t22_expiry, (caddr_t) cg_get(cg), sr->config.t22);
			break;
		case t23:
			printd(("%s: %p: starting t23 %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.t23 * 1000 / HZ, jiffies));
			cg->timers.t23 =
			    timeout(&cg_t23_expiry, (caddr_t) cg_get(cg), sr->config.t23);
			break;
		case t28:
			printd(("%s: %p: starting t28 %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.t28 * 1000 / HZ, jiffies));
			cg->timers.t28 =
			    timeout(&cg_t28_expiry, (caddr_t) cg_get(cg), sr->config.t28);
			break;
		case tcgb:
			printd(("%s: %p: starting tcgb %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.tcgb * 1000 / HZ, jiffies));
			cg->timers.tcgb =
			    timeout(&cg_tcgb_expiry, (caddr_t) cg_get(cg), sr->config.tcgb);
			break;
		case tgrs:
			printd(("%s: %p: starting tgrs %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.tgrs * 1000 / HZ, jiffies));
			cg->timers.tgrs =
			    timeout(&cg_tgrs_expiry, (caddr_t) cg_get(cg), sr->config.tgrs);
			break;
		case thga:
			printd(("%s: %p: starting thga %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.thga * 1000 / HZ, jiffies));
			cg->timers.thga =
			    timeout(&cg_thga_expiry, (caddr_t) cg_get(cg), sr->config.thga);
			break;
		case tscga:
			printd(("%s: %p: starting tscga %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.tscga * 1000 / HZ, jiffies));
			cg->timers.tscga =
			    timeout(&cg_tscga_expiry, (caddr_t) cg_get(cg), sr->config.tscga);
			break;
		case tscga_d:
			printd(("%s: %p: starting tscga_d %lu ms at %lu\n", DRV_NAME, cg,
				sr->config.tscga_d * 1000 / HZ, jiffies));
			cg->timers.tscga_d =
			    timeout(&cg_tscga_d_expiry, (caddr_t) cg_get(cg), sr->config.tscga_d);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&cg->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SR timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
sr_do_timeout(caddr_t data, const char *timer, ulong *timeo, int (to_fnc) (struct sr *),
	      void (*exp_fnc) (caddr_t))
{
	struct sr *sr = (struct sr *) data;
	if (xchg(timeo, 0)) {
		if (spin_trylock(&sr->lock)) {
			printd(("%s: %p: %s timeout at %lu\n", DRV_NAME, sr, timer, jiffies));
			switch (to_fnc(sr)) {
			default:
			case QR_DONE:
				sr_put(sr);
				spin_unlock(&sr->lock);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&sr->lock);
		} else
			printd(("%s: %p: %s timeout collision at %lu\n", DRV_NAME, sr, timer,
				jiffies));
		/* 
		   back off timer one tick */
		*timeo = timeout(exp_fnc, data, 100);
	}
}

STATIC int sr_t4_timeout(struct sr *);
STATIC void
sr_t4_expiry(caddr_t data)
{
	sr_do_timeout(data, "t4", &((struct sr *) data)->timers.t4, &sr_t4_timeout, &sr_t4_expiry);
};

STATIC int sr_t29_timeout(struct sr *);
STATIC void
sr_t29_expiry(caddr_t data)
{
	sr_do_timeout(data, "t29", &((struct sr *) data)->timers.t29, &sr_t29_timeout,
		      &sr_t29_expiry);
};

STATIC int sr_t30_timeout(struct sr *);
STATIC void
sr_t30_expiry(caddr_t data)
{
	sr_do_timeout(data, "t30", &((struct sr *) data)->timers.t30, &sr_t30_timeout,
		      &sr_t30_expiry);
};

STATIC INLINE void
__sr_timer_stop(struct sr *sr, const uint t)
{
	int single = 1;
	ulong to;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t4:
		if ((to = xchg(&sr->timers.t4, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t4 at %lu\n", DRV_NAME, sr, jiffies));
			sr_put(sr);
		}
		/* 
		   fall through */
	case t29:
		if ((to = xchg(&sr->timers.t29, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t29 at %lu\n", DRV_NAME, sr, jiffies));
			sr_put(sr);
		}
		/* 
		   fall through */
	case t30:
		if ((to = xchg(&sr->timers.t30, 0))) {
			untimeout(to);
			printd(("%s: %p: stopping t30 at %lu\n", DRV_NAME, sr, jiffies));
			sr_put(sr);
		}
		/* 
		   fall through */
	}
}

STATIC INLINE void
sr_timer_stop(struct sr *sr, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&sr->lock, flags);
	{
		__sr_timer_stop(sr, t);
	}
	spin_unlock_irqrestore(&sr->lock, flags);
}

STATIC INLINE void
sr_timer_start(struct sr *sr, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&sr->lock, flags);
	{
		__sr_timer_stop(sr, t);
		switch (t) {
		case t4:
			printd(("%s: %p: starting t4 %lu ms at %lu\n", DRV_NAME, sr,
				sr->config.t4 * 1000 / HZ, jiffies));
			sr->timers.t4 = timeout(&sr_t4_expiry, (caddr_t) sr_get(sr), sr->config.t4);
			break;
		case t29:
			printd(("%s: %p: starting t29 %lu ms at %lu\n", DRV_NAME, sr,
				sr->config.t29 * 1000 / HZ, jiffies));
			sr->timers.t29 =
			    timeout(&sr_t29_expiry, (caddr_t) sr_get(sr), sr->config.t29);
			break;
		case t30:
			printd(("%s: %p: starting t30 %lu ms at %lu\n", DRV_NAME, sr,
				sr->config.t30 * 1000 / HZ, jiffies));
			sr->timers.t30 =
			    timeout(&sr_t30_expiry, (caddr_t) sr_get(sr), sr->config.t30);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&sr->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ISUP State Machines
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  ISUP Circuit Management Functions
 *
 *  -------------------------------------------------------------------------
 */
static int
ct_block(queue_t *q, struct ct *ct)
{
	int err;
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
		ct_timer_stop(ct, t35);
	case CTS_ICC_WAIT_ACM:
		if (ct_tst(ct, CCTF_COR_PENDING))
			ct_timer_stop(ct, t8);
		break;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WRES_SIND:
	case CCS_WREQ_PROCEED:
		/* 
		   XXX These should probably be repeat attempt */
		if ((err =
		     cc_call_failure_ind(q, ct->cpc.cc, ct, ISUP_CALL_FAILURE_BLOCKING,
					 CC_CAUS_TEMPORARY_FAILURE)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	}
	ct_set(ct, CCTF_LOC_M_BLOCKED);
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
		ct_set_c_state(ct, CTS_IDLE);
		break;
	}
	ct_set(ct, CCTF_LOC_M_BLOCK_PENDING);
	ct_timer_start(ct, t12);
	ct_timer_start(ct, t13);
	if ((err = isup_send_blo(q, ct)))
		return (err);
	return (QR_DONE);
}

static int
ct_unblock(queue_t *q, struct ct *ct)
{
	int err;
	ct_set(ct, CCTF_LOC_M_UNBLOCK_PENDING);
	ct_timer_start(ct, t14);
	ct_timer_start(ct, t15);
	if ((err = isup_send_ubl(q, ct)))
		return (err);
	return (QR_DONE);
}

static int
ct_reset(queue_t *q, struct ct *ct)
{
	int err;
	switch (ct_get_c_state(ct)) {
	case CTS_IDLE:
		break;
	case CTS_ICC_WAIT_SAM:
		ct_timer_stop(ct, t35);
	case CTS_ICC_WAIT_ACM:
		if (ct_tst(ct, CCTF_COR_PENDING))
			ct_timer_stop(ct, t8);
		break;
	case CTS_ICC_WAIT_ANM:
	case CTS_ICC_SUSPENDED:
	case CTS_ICC_ANSWERED:
		break;
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		if (ct_tst(ct, CCTF_COT_PENDING))
			ct_timer_stop(ct, t24);
		ct_timer_stop(ct, t7);
		break;
	case CTS_OGC_SUSPENDED:
		ct_timer_stop(ct, t6);
		ct_timer_stop(ct, t38);
	case CTS_OGC_ANSWERED:
		break;
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
		ct_timer_stop(ct, t5);
		ct_timer_stop(ct, t1);
		if (ct->rel)
			freemsg(xchg(&ct->rel, NULL));
		break;
	default:
		break;
	}
	switch (ct_get_t_state(ct)) {
	case CKS_WIND_CTEST:
		ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
		break;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_IDLE:
		break;
	case CCS_WIND_MORE:
	case CCS_WREQ_INFO:
	case CCS_WCON_SREQ:
	case CCS_WIND_PROCEED:
		if ((err = cc_call_reattempt_ind(q, ct->cpc.cc, ct, ISUP_REATTEMPT_RESET)))
			return (err);
		/* 
		   responsibility of CC to reattempt */
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	default:
		if ((err =
		     cc_call_failure_ind(q, ct->cpc.cc, ct, ISUP_CALL_FAILURE_RESET,
					 CC_CAUS_NORMAL_UNSPECIFIED)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_IDLE:
		break;
	default:
		ct_set_c_state(ct, CTS_IDLE);
		break;
	}
	ct_set(ct, CCTF_LOC_RESET_PENDING);
	ct_timer_start(ct, t16);
	ct_timer_start(ct, t17);
	if ((err = isup_send_rsc(q, ct)))
		return (err);
	return (QR_DONE);
}

static int
cg_m_block(queue_t *q, struct cg *cg)
{
	int err;
	struct ct *ct;
	int o, i, j;
	cg->cic = cg->ct.list->cic;	/* reset base cic */
	for (i = 0; i < RS_MAX_RANGE; i++)
		cg->rs_ptr[i] = 0;
	for (ct = cg->ct.list; ct; ct = ct->cg.next) {
		if (!ct_tst(ct, CCTF_LOC_G_BLOCK_PENDING)) {
			o = ct->cic - cg->cic;
			j = (o >> 3) + 1;
			i = o & 0x7;
			if (cg->rs_ptr[0] < o)
				cg->rs_ptr[0] = o;
			cg->rs_ptr[j] |= (0x1 << i);
			ct_set(ct, CCTF_LOC_G_BLOCK_PENDING);
			if (ct_get_c_state(ct) == CTS_IDLE)
				continue;
			switch (ct_get_c_state(ct)) {
			case CTS_ICC_WAIT_SAM:
				ct_timer_stop(ct, t35);
			case CTS_ICC_WAIT_ACM:
				if (ct_tst(ct, CCTF_COR_PENDING))
					ct_timer_stop(ct, t8);
				break;
			}
			switch (ct_get_i_state(ct)) {
			case CCS_IDLE:
			case CCS_WREQ_MORE:
			case CCS_WIND_INFO:
			case CCS_WRES_SIND:
			case CCS_WREQ_PROCEED:
				if ((err =
				     cc_call_failure_ind(q, ct->cpc.cc, ct,
							 ISUP_CALL_FAILURE_BLOCKING,
							 CC_CAUS_TEMPORARY_FAILURE)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
				break;
			}
			switch (ct_get_c_state(ct)) {
			case CTS_ICC_WAIT_SAM:
			case CTS_ICC_WAIT_ACM:
				ct_set_c_state(ct, CTS_IDLE);
				break;
			}
		}
	}
	cg->rs_len = ((cg->rs_ptr[0] + 7) >> 3) + 1;
	cg_set(cg, CCTF_LOC_M_BLOCK_PENDING);
	cg_timer_start(cg, t18);
	cg_timer_start(cg, t19);
	if ((err =
	     isup_send_cgb(q, cg->ct.list, ISUP_MAINTENANCE_ORIENTED, cg->rs_ptr, cg->rs_len)))
		return (err);
	return (QR_DONE);
}

static int
cg_h_block(queue_t *q, struct cg *cg)
{
	int err;
	struct ct *ct;
	int o, i, j;
	cg->cic = cg->ct.list->cic;	/* reset base cic */
	for (i = 0; i < RS_MAX_RANGE; i++)
		cg->rs_ptr[i] = 0;
	for (ct = cg->ct.list; ct; ct = ct->cg.next) {
		if (!ct_tst(ct, CCTF_LOC_G_BLOCK_PENDING)) {
			o = ct->cic - cg->cic;
			j = (o >> 3) + 1;
			i = o & 0x7;
			if (cg->rs_ptr[0] < o)
				cg->rs_ptr[0] = o;
			cg->rs_ptr[j] |= (0x1 << i);
			ct_set(ct, CCTF_LOC_G_BLOCK_PENDING);
			if (ct_get_c_state(ct) == CTS_IDLE)
				continue;
			switch (ct_get_c_state(ct)) {
			case CTS_OGC_WAIT_SAM:
				ct_timer_stop(ct, t35);
			case CTS_OGC_WAIT_ACM:
				ct_timer_stop(ct, t7);
				break;
			case CTS_OGC_WAIT_ANM:
				ct_timer_stop(ct, t9);
				break;
			case CTS_ICC_WAIT_SAM:
				ct_timer_stop(ct, t35);
			case CTS_ICC_WAIT_ACM:
				if (ct_tst(ct, CCTF_COR_PENDING))
					ct_timer_stop(ct, t8);
				break;
			case CTS_ICC_WAIT_ANM:
				ct_timer_stop(ct, t9);
				break;
			case CTS_ICC_WAIT_RLC:
			case CTS_OGC_WAIT_RLC:
				ct_timer_stop(ct, t5);
				ct_timer_stop(ct, t1);
				if (ct->rel)
					freemsg(xchg(&ct->rel, NULL));
				break;
			}
			switch (ct_get_t_state(ct)) {
			case CKS_WCON_RELREQ:
				/* 
				   confirm release */
				if ((err = cc_release_con(q, ct->tst.cc, ct, NULL)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
				break;
			}
			switch (ct_get_i_state(ct)) {
			case CCS_WCON_RELREQ:
				/* 
				   confirm release */
				if ((err = cc_release_con(q, ct->cpc.cc, ct, NULL)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
				break;
			case CCS_WRES_RELIND:
				/* 
				   await response */
				break;
			default:
				/* 
				   CC responsibility to abort COT */
				if ((err =
				     cc_call_failure_ind(q, ct->cpc.cc, ct,
							 ISUP_CALL_FAILURE_BLOCKING,
							 CC_CAUS_TEMPORARY_FAILURE)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
				break;
			}
			/* 
			   keep from being added to idle list */
			ct_set_c_state(ct, CTS_IDLE);
		}
	}
	cg->rs_len = ((cg->rs_ptr[0] + 7) >> 3) + 1;
	cg_set(cg, CCTF_LOC_H_BLOCK_PENDING);
	cg_timer_start(cg, t18);
	cg_timer_start(cg, t19);
	if ((err =
	     isup_send_cgb(q, cg->ct.list, ISUP_HARDWARE_FAILURE_ORIENTED, cg->rs_ptr, cg->rs_len)))
		return (err);
	return (QR_DONE);
}

static int
cg_unblock(queue_t *q, struct cg *cg, ulong cgi)
{
	int err;
	struct ct *ct;
	int o, i, j;
	cg->cic = cg->ct.list->cic;	/* reset base cic */
	for (i = 0; i < RS_MAX_RANGE; i++)
		cg->rs_ptr[i] = 0;
	for (ct = cg->ct.list; ct; ct = ct->cg.next) {
		if (!ct_tst(ct, CCTF_LOC_G_UNBLOCK_PENDING)) {
			o = ct->cic - cg->cic;
			j = (o >> 3) + 1;
			i = o & 0x7;
			if (cg->rs_ptr[0] < o)
				cg->rs_ptr[0] = o;
			cg->rs_ptr[j] |= (0x1 << i);
			ct_set(ct, CCTF_LOC_G_UNBLOCK_PENDING);
		}
	}
	cg->rs_len = ((cg->rs_ptr[0] + 7) >> 3) + 1;
	if (cgi)
		cg_set(cg, CCTF_LOC_H_UNBLOCK_PENDING);
	else
		cg_set(cg, CCTF_LOC_M_UNBLOCK_PENDING);
	cg_timer_start(cg, t20);
	cg_timer_start(cg, t21);
	if ((err = isup_send_cgu(q, cg->ct.list, cgi, cg->rs_ptr, cg->rs_len)))
		return (err);
	return (QR_DONE);
}

static int
cg_reset(queue_t *q, struct cg *cg)
{
	int err;
	struct ct *ct;
	cg->cic = cg->ct.list->cic;	/* reset base cic */
	cg->rs_ptr[0] = 0;
	for (ct = cg->ct.list; ct; ct = ct->cg.next) {
		if (!(ct_tst(ct, CCTF_LOC_RESET_PENDING))) {
			if (cg->rs_ptr[0] < (ct->cic - cg->cic))
				cg->rs_ptr[0] = ct->cic - cg->cic;
			switch (ct_get_c_state(ct)) {
			case CTS_ICC_WAIT_SAM:
			case CTS_ICC_WAIT_ACM:
				if (ct_tst(ct, CCTF_COR_PENDING))
					ct_timer_stop(ct, t8);
				break;
			case CTS_OGC_WAIT_SAM:
			case CTS_OGC_WAIT_ACM:
				if (ct_tst(ct, CCTF_COT_PENDING))
					ct_timer_stop(ct, t24);
				ct_timer_stop(ct, t7);
				break;
			case CTS_OGC_SUSPENDED:
				ct_timer_stop(ct, t6);
				ct_timer_stop(ct, t38);
				break;
			case CTS_ICC_WAIT_RLC:
			case CTS_OGC_WAIT_RLC:
				ct_timer_stop(ct, t5);
				ct_timer_stop(ct, t1);
				if (ct->rel)
					freemsg(xchg(&ct->rel, NULL));
				break;
			}
			switch (ct_get_t_state(ct)) {
			case CKS_WIND_CTEST:
				ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
				break;
			}
			switch (ct_get_i_state(ct)) {
			case CCS_WIND_MORE:
			case CCS_WREQ_INFO:
			case CCS_WCON_SREQ:
			case CCS_WIND_PROCEED:
				if ((err =
				     cc_call_reattempt_ind(q, ct->cpc.cc, ct,
							   ISUP_REATTEMPT_RESET)))
					return (err);
				/* 
				   responsibility of CC to reattempt */
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
				break;
			default:
				if ((err =
				     cc_call_failure_ind(q, ct->cpc.cc, ct, ISUP_CALL_FAILURE_RESET,
							 CC_CAUS_NORMAL_UNSPECIFIED)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
				break;
			}
			ct_set_c_state(ct, CTS_IDLE);
			ct_set(ct, CCTF_LOC_RESET_PENDING);
		}
	}
	cg->rs_len = 1;
	cg_set(cg, CCTF_LOC_RESET_PENDING);
	cg_timer_start(cg, t22);
	cg_timer_start(cg, t23);
	if ((err = isup_send_grs(q, cg->ct.list, cg->rs_ptr, cg->rs_len)))
		return (err);
	return (QR_DONE);
}

static int
cg_query(queue_t *q, struct cg *cg)
{
	int err;
	struct ct *ct;
	int o;
	cg->cic = cg->ct.list->cic;	/* reset base cic */
	cg->rs_ptr[0] = 0;
	for (ct = cg->ct.list; ct; ct = ct->cg.next) {
		if (!ct_tst(ct, CCTF_LOC_QUERY_PENDING)) {
			o = ct->cic - cg->cic;
			if (cg->rs_ptr[0] < o)
				cg->rs_ptr[0] = o;
			ct_set(ct, CCTF_LOC_QUERY_PENDING);
		}
	}
	cg->rs_len = 1;
	cg_set(cg, CCTF_LOC_QUERY_PENDING);
	cg_timer_start(cg, t28);
	if ((err = isup_send_cqm(q, cg->ct.list, cg->rs_ptr, cg->rs_len)))
		return (err);
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ISUP Receive Message Functions
 *
 *  -------------------------------------------------------------------------
 *  These messages are received at the upper queue to which they belong: they have already been routed to the
 *  appropriate upper queue by the lower queue discrimitation functions and have been decoded and checked for the
 *  existence of mandatory parameters.
 */
/*
 *  ISUP_MT_IAM:        -  0x01 - Initial address
 *  -------------------------------------------------------------------------
 *  2.8.2.3 xiv) If a non-test initial address message is received on a remotely blocked circuit, the remotely
 *  blocked state of the circuit is removed and the initial address message is processed normally unless the
 *  circuit is also locally blocked in which case the initial address message is discarded.  This applies to the
 *  blocking state whether maintenance, hardware or both.  However, it should not be the preferred method of
 *  unblocking a circuit.
 *
 *  2.3.4 Each exchange of the connection shall store during the call setup the initial address message
 *  information sent or received.  The information to be stored includes all parameters in the initial address
 *  messge and, if the intial address message has been segmented, in the subsequent segmentation message.  The
 *  contents of the initial address message information shall be updated, if the values of parameters change
 *  during the call setup.
 */
STATIC int
isup_recv_iam(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	struct cc *cc, *ck;
	printd(("%s; %p: IAM <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_IDLE:
		break;
	case CTS_OGC_WAIT_CCR:
		/* 
		   glare scenario with CCR */
		break;
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		/* 
		   glare scenario */
		ct->tg.tg->stats.tg_dual_siezures++;
		ct->sr.sr->stats.tg_dual_siezures++;
		ct->sp.sp->stats.tg_dual_siezures++;
		master.stats.tg_dual_siezures++;
		if (ct->tg.tg->config.flags & ISUP_TGF_GLARE_PRIORITY)
			return (QR_DONE);
		ct_timer_stop(ct, t7);
		break;
	default:
		/* 
		   unexpected message */
		return (-EOPNOTSUPP);
	}
	switch (ct_get_t_state(ct)) {
	case CKS_IDLE:
		break;
	case CKS_WREQ_CTEST:
		/* 
		   second time through after error */
		break;
	default:
		swerr();
		ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
		break;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_IDLE:
	case CCS_WIND_SETUP:
		break;
	case CCS_WIND_MORE:
	case CCS_WREQ_INFO:
	case CCS_WCON_SREQ:
	case CCS_WIND_PROCEED:
		/* 
		   responsibility of CC to reattempt */
		/* 
		   CC responsibility to abort COT when it receives cc_reattempt_ind */
		if ((err = cc_call_reattempt_ind(q, ct->cpc.cc, ct, ISUP_REATTEMPT_DUAL_SIEZURE)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WRES_SIND:
	case CCS_WIND_CCREP:
		/* 
		   second time through after error */
		break;
	default:
		swerr();
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		ct_set_c_state(ct, CTS_IDLE);
		/* 
		   fall through */
	case CTS_IDLE:
		if (m->msg.iam.cpc != ((ISUP_CPC_TEST_CALL >> 24) & 0xff)) {
			/* 
			   not a test call - consider remote blocking */
			if (ct_tst(ct, CCTF_REM_M_BLOCKED)
			    && !(ct_tst(ct, CCTF_REM_M_UNBLOCK_PENDING))) {
				struct cc *cm;
				if ((cm = ct->bind.mgm) || (cm = ct->cg.cg->bind.mgm)
				    || (cm = ct->tg.tg->bind.mgm) || (cm = ct->sr.sr->bind.mgm)
				    || (cm = ct->sp.sp->bind.mgm) || (cm = master.bind.mgm)) {
					if (ct_get_m_state(ct) == CMS_IDLE)
						ct_set_m_state(ct, cm, CMS_WRES_UBIND);
					if ((err = cc_unblocking_ind(q, cm, ct, m)))
						return (err);
					// ct_set(ct, CCTF_REM_M_UNBLOCK_PENDING);
					ct_clr(ct, CCTF_REM_M_BLOCKED);
				} else {
					__printd(("%s: MGMT: unhandled unblocking indication on circuit id=%ld, cic=%ld\n", DRV_NAME, ct->id, ct->cic));
					/* 
					   complete procedure automatically */
					ct_clr(ct, CCTF_REM_M_BLOCKED);
				}
			} else if (ct_tst(ct, CCTF_REM_H_BLOCKED)
				   && !(ct_tst(ct, CCTF_REM_H_UNBLOCK_PENDING))) {
				struct cc *cm;
				if ((cm = ct->bind.mgm) || (cm = ct->cg.cg->bind.mgm)
				    || (cm = ct->tg.tg->bind.mgm) || (cm = ct->sr.sr->bind.mgm)
				    || (cm = ct->sp.sp->bind.mgm) || (cm = master.bind.mgm)) {
					if (ct_get_m_state(ct) == CMS_IDLE)
						ct_set_m_state(ct, cm, CMS_WRES_UBIND);
					if ((err = cc_unblocking_ind(q, cm, ct, m)))
						return (err);
					// ct_set(ct, CCTF_REM_H_UNBLOCK_PENDING);
					ct_clr(ct, CCTF_REM_H_BLOCKED);
				} else {
					__printd(("%s: MGMT: unhandled unblocking indication on circuit id=%ld, cic=%ld\n", DRV_NAME, ct->id, ct->cic));
					/* 
					   complete procedure automatically */
					ct_clr(ct, CCTF_REM_H_BLOCKED);
				}
			}
			if (ct_tst(ct, CCTF_LOC_M_BLOCKED)) {
				if ((err = ct_block(q, ct)))
					return (err);
				return (QR_DONE);
			} else if (ct_tst(ct, CCTF_LOC_H_BLOCKED)) {
				if ((err = cg_h_block(q, ct->cg.cg)))
					return (err);
				return (QR_DONE);
			}
			if (ct->tg.tg->config.type == ISUP_TG_TYPE_OG)
				goto outgoing_only;
		}
		/* 
		   look for listeners */
		if ((!(cc = ct->bind.icc) || cc->setind >= cc->maxind)
		    && (!(cc = ct->cg.cg->bind.icc) || cc->setind >= cc->maxind)
		    && (!(cc = ct->tg.tg->bind.icc) || cc->setind >= cc->maxind)
		    && (!(cc = ct->sr.sr->bind.icc) || cc->setind >= cc->maxind)
		    && (!(cc = ct->sp.sp->bind.icc) || cc->setind >= cc->maxind)
		    && (!(cc = master.bind.icc) || cc->setind >= cc->maxind)
		    ) ;
		/* 
		   look for test */
		if ((!(ck = ct->bind.tst) || ck->setind >= ck->maxind)
		    && (!(ck = ct->cg.cg->bind.tst) || ck->setind >= ck->maxind)
		    && (!(ck = ct->tg.tg->bind.tst) || ck->setind >= ck->maxind)
		    && (!(ck = ct->sr.sr->bind.tst) || ck->setind >= ck->maxind)
		    && (!(ck = ct->sp.sp->bind.tst) || ck->setind >= ck->maxind)
		    && (!(ck = master.bind.tst) || ck->setind >= ck->maxind)
		    ) ;
		if ((m->msg.iam.cpc == ((ISUP_CPC_TEST_CALL >> 24) & 0xff) && !(cc = ck)) || !cc)
			goto temporary_failure;
		if ((m->msg.iam.nci & ISUP_NCI_CONT_CHECK_MASK) == ISUP_NCI_CONT_CHECK_REQUIRED
		    && !ck)
			goto temporary_failure;
		ct_set_i_state(ct, cc, CCS_WIND_SETUP);
		if ((err = cc_setup_ind(q, cc, ct, m)))
			return (err);
		/* 
		   store information from iam */
		switch (m->msg.iam.nci & ISUP_NCI_CONT_CHECK_MASK) {
		case ISUP_NCI_CONT_CHECK_PREVIOUS:
			ct_timer_start(ct, t8);
			ct_set(ct, CCTF_COR_PENDING);
			ct_set_i_state(ct, cc, CCS_WIND_CCREP);
			break;
		case ISUP_NCI_CONT_CHECK_REQUIRED:
			if (ct_get_t_state(ct) != CKS_WREQ_CTEST) {
				if ((err = cc_cont_check_ind(q, ck, ct, m)))
					return (err);
				ct_set_t_state(ct, ck, CKS_WREQ_CTEST);
			}
			/* 
			   responsibility of CM to perform loopback */
			ct_timer_start(ct, t8);
			ct_set(ct, CCTM_CONT_CHECK);
			ct_set_i_state(ct, cc, CCS_WIND_CCREP);
			break;
		default:
			if (!isup_st_in_cdpn(m->msg.iam.cdpn.ptr, m->msg.iam.cdpn.len)) {
				ct_set_i_state(ct, cc, CCS_WREQ_MORE);
			} else {
				ct_set_i_state(ct, cc, CCS_WRES_SIND);
			}
			break;
		}
		if (!isup_st_in_cdpn(m->msg.iam.cdpn.ptr, m->msg.iam.cdpn.len)) {
			ct_timer_start(ct, t35);
			ct_set_c_state(ct, CTS_ICC_WAIT_SAM);
		} else {
			ct_set_c_state(ct, CTS_ICC_WAIT_ACM);
		}
		return (QR_DONE);
	}
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
      temporary_failure:
	/* 
	   no listening stream when there should be */
	if ((err = isup_send_rel(q, ct, CC_CAUS_TEMPORARY_FAILURE, NULL, 0)))
		return (err);
	ct_timer_start(ct, t5);
	ct_timer_start(ct, t1);
	ct_set_c_state(ct, CTS_ICC_WAIT_RLC);
	return (QR_DONE);
      outgoing_only:
	/* 
	   call received on outgoing trunk */
	if ((err = isup_send_rel(q, ct, CC_CAUS_NO_ROUTE_TO_DESTINATION, NULL, 0)))
		return (err);
	ct_timer_start(ct, t5);
	ct_timer_start(ct, t1);
	ct_set_c_state(ct, CTS_ICC_WAIT_RLC);
	return (QR_DONE);
}

/*
 *  ISUP_MT_SAM:        -  0x02 - Subsequent address (not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_sam(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: SAM <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_MORE:
		/* 
		   skip optional CC_MORE_INFO_REQ */
		ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_INFO);
		/* 
		   fall through */
	case CCS_WIND_INFO:
		if ((err = cc_information_ind(q, ct, m)))
			return (err);
		if (isup_st_in_subn(m->msg.sam.subn.ptr, m->msg.sam.subn.len)) {
			ct_set_i_state(ct, ct->cpc.cc, CCS_WREQ_PROCEED);
		} else {
			ct_set_i_state(ct, ct->cpc.cc, CCS_WREQ_MORE);
		}
		break;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
		if (isup_st_in_subn(m->msg.sam.subn.ptr, m->msg.sam.subn.len)) {
			ct_timer_stop(ct, t35);
			ct_set_c_state(ct, CTS_ICC_WAIT_ACM);
		} else {
			ct_timer_start(ct, t35);
		}
		return (QR_DONE);
	}
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_INR:        -  0x03 - Information request
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_inr(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: INR <-\n", DRV_NAME, ct));
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_INF:        -  0x04 - Information
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_inf(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: INF <-\n", DRV_NAME, ct));
	ct_timer_stop(ct, t33);
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_COT:        -  0x05 - Continuity
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cot(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: COT <-\n", DRV_NAME, ct));
	/* 
	   does not interrupt segmentation */
	if (!ct_tst(ct, CCTF_COR_PENDING))
		goto unexpected;
	switch (m->msg.cot.coti & (ISUP_COT_FAILURE | ISUP_COT_SUCCESS)) {
	case ISUP_COT_SUCCESS:
		/* 
		   responsibility of CC to remove loopback */
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
			ct_timer_stop(ct, t8);
			if (ct_get_i_state(ct) == CCS_WIND_CCREP) {
				if ((err = cc_cont_report_ind(q, ct->cpc.cc, ct, m)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_WREQ_MORE);
			}
			if (ct_get_t_state(ct) == CKS_WIND_CCREP) {
				if ((err = cc_cont_report_ind(q, ct->tst.cc, ct, m)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
			}
			ct_clr(ct, CCTM_CONT_CHECK);
			ct_set_c_state(ct, CTS_ICC_WAIT_SAM);
			break;
		case CTS_ICC_WAIT_ACM:
			ct_timer_stop(ct, t8);
			if (ct_get_i_state(ct) == CCS_WIND_CCREP) {
				if ((err = cc_cont_report_ind(q, ct->cpc.cc, ct, m)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_WRES_SIND);
			}
			if (ct_get_t_state(ct) == CKS_WIND_CCREP) {
				if ((err = cc_cont_report_ind(q, ct->tst.cc, ct, m)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
			}
			ct_clr(ct, CCTM_CONT_CHECK);
			ct_set_c_state(ct, CTS_ICC_WAIT_ACM);
			break;
		case CTS_ICC_WAIT_CCR:
			ct_timer_stop(ct, t36);
			if (ct_get_t_state(ct) == CKS_WIND_CCREP) {
				if ((err = cc_cont_report_ind(q, ct->tst.cc, ct, m)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
			}
			ct_clr(ct, CCTM_CONT_CHECK);
			ct_set_c_state(ct, CTS_IDLE);
			break;
		default:
			goto unexpected;
		}
		break;
	case ISUP_COT_FAILURE:
		/* 
		   responsibility of CC to remove loopback */
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
			ct_timer_stop(ct, t35);
		case CTS_ICC_WAIT_ACM:
			ct_timer_stop(ct, t8);
			if (ct_get_i_state(ct) != CCS_IDLE) {
				if ((err =
				     cc_call_failure_ind(q, ct->cpc.cc, ct,
							 ISUP_CALL_FAILURE_COT_FAILURE,
							 CC_CAUS_TEMPORARY_FAILURE)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			}
			/* 
			   fall through */
		case CTS_ICC_WAIT_CCR:
			ct_timer_stop(ct, t36);
			if (ct_get_t_state(ct) == CKS_WIND_CCREP) {
				/* 
				   responsibiltiy of CM to remove loopback */
				if ((err = cc_cont_report_ind(q, ct->tst.cc, ct, m)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
			}
			break;
		default:
			goto unexpected;
		}
		if (ct_tst(ct, CCTF_COT_PENDING)) {
			ct_timer_start(ct, t27);
			ct_set_c_state(ct, CTS_ICC_WAIT_CCR);	/* prepare for recheck */
		} else {
			ct_set_c_state(ct, CTS_IDLE);	/* failed on previous circuit */
		}
		ct_clr(ct, CCTM_CONT_CHECK);
		break;
	default:
		/* 
		   bad parameter */
		return (-EPROTONOSUPPORT);
	}
	return (QR_DONE);
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_ACM:        -  0x06 - Address complete
 *  -------------------------------------------------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: a) in the originating exchange
 *  when the address complete message or connect message has been received and the calling party deos not
 *  subscribe to a supplementary service which would cause a new call setup.  ... b) in the intermediate exchange
 *  when the address complete message or the connect message has been received;
 *
 *  2.2.4 ... If call setup failes and an inband tone or announcement has to be returned to the calling party from
 *  an exchange or called party, the exchange or user concerned connects the inband tone or announcement to the
 *  transmission path.  If a time-out occurs at the exchange providing the inband tone or announcement, the
 *  exchange sends a release message to the preceding exchange with cause value #31 (normal unspecified).
 *
 *  If an address complete message has been returned to the preceding exchange a call progress message indicating
 *  that inband tone information is available along with the cause parameter, is returned to the preceding
 *  exchange (see 2.1.5).  The cause value should reflect the reason of call failure in the same way as the inband
 *  tone or announcement to be applied.
 *
 *  If an address complete message has not been returned to the preceding exchange already, an address complete
 *  message with the cause parameter and the "in-band information" indicator set in the optional backward call
 *  indicator, will be returned to the originating exchange.  The cause value should reflect the reason of call
 *  failure in the same way as the in-band tone or announcement to be applied.
 *
 *  In case a special tone or announcement has to be applied due to an event only known by a certain exchange and
 *  not covered by a cause value, not cause parameter is included in either the address complete or call progress
 *  messages.  The answer message must not be sent in this case.
 *
 *  For the preceding exchanges the inclusion of the cause parameter in the address complete or call progress
 *  message implies an unsuccessful call setup.  The cause parameter will not be included for unsuccessful call
 *  stups when interworking has occured and the in-band tone or announcement is returned from the exchange beyond
 *  the interworking point.
 *
 */
STATIC int
isup_recv_acm(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: ACM <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ACM:
	case CTS_OGC_WAIT_SAM:
		ct_timer_stop(ct, t7);
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_MORE:
	case CCS_WREQ_INFO:
	case CCS_WCON_SREQ:
		if ((err = cc_setup_con(q, ct, m)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_PROCEED);
		/* 
		   fall through */
	case CCS_WIND_PROCEED:
		if (!(m->msg.acm.bci & ISUP_BCI_SUBSCRIBER_FREE)) {
			if ((err = cc_proceeding_ind(q, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_ALERTING);
			break;
		}
		/* 
		   fall through */
	case CCS_WIND_ALERTING:
		if (m->msg.acm.bci & ISUP_BCI_SUBSCRIBER_FREE) {
			if ((err = cc_alerting_ind(q, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_PROGRESS);
			break;
		}
		/* 
		   fall through */
	case CCS_WIND_PROGRESS:
		if (!(m->msg.acm.bci & ISUP_BCI_SUBSCRIBER_FREE)) {
			if ((err = cc_progress_ind(q, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_PROGRESS);
			break;
		}
		/* 
		   fall through */
	case CCS_WIND_IBI:
		if (m->oparm.obci.ptr
		    && ((m->oparm.obci.val << 16) & ISUP_OBCI_INBAND_INFORMATION_AVAILABLE)) {
			if ((err = cc_ibi_ind(q, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_CONNECT);
			break;
		}
		/* 
		   fall through */
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ACM:
	case CTS_OGC_WAIT_SAM:
		ct_timer_start(ct, t9);
		/* 
		   release iam information */
		ct_set_c_state(ct, CTS_OGC_WAIT_ANM);
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
      unexpected:
	printd(("%s: %p: PROTO: unexpected message\n", DRV_NAME, ct));
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_CON:        -  0x07 - Connect (not ANSI)
 *  -------------------------------------------------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: a) in the originating exchange
 *  when the address complete message or connect message has been received and the calling party deos not
 *  subscribe to a supplementary service which would cause a new call setup.  ... b) in the intermediate exchange
 *  when the address complete message or the connect message has been received;
 */
STATIC int
isup_recv_con(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: CON <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ACM:
		ct_timer_stop(ct, t7);
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_MORE:
	case CCS_WREQ_INFO:
	case CCS_WCON_SREQ:
		if ((err = cc_setup_con(q, ct, m)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_CONNECT);
		/* 
		   fall through */
	case CCS_WIND_PROCEED:
	case CCS_WIND_ALERTING:
	case CCS_WIND_PROGRESS:
	case CCS_WIND_CONNECT:
		/* 
		   responsiblity of CC to start charging after receiving CC_CONNECT_IND */
		if ((err = cc_connect_ind(q, ct, m)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_CONNECTED);
		break;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ACM:
		/* 
		   release iam information */
		ct_set_c_state(ct, CTS_OGC_ANSWERED);
		return (QR_DONE);
	}
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_FOT:        -  0x08 - Forward transfer
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_fot(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: FOT <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_ANM:
	case CTS_ICC_ANSWERED:
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_CONNECT:
	case CCS_CONNECTED:
		if ((err = cc_forwxfer_ind(q, ct, m)))
			return (err);
		/* 
		   don't change state */
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_ANM:
	case CTS_ICC_ANSWERED:
		return (QR_DONE);
	}
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_ANM:        -  0x09 - Answer
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_anm(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: ANM <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ANM:
		ct_timer_stop(ct, t9);
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_ALERTING:
	case CCS_WIND_PROGRESS:
	case CCS_WIND_CONNECT:
		/* 
		   CC responsibility to start charging */
		if ((err = cc_connect_ind(q, ct, m)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_CONNECTED);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ANM:
		ct_set_c_state(ct, CTS_OGC_ANSWERED);
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_REL:        -  0x0c - Release
 *  -------------------------------------------------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: ... wehn the call is released
 *  earlier and no automatic repeat attempt is to be attempted.
 * 
 *  2.3.1 e) In the case when two points in the connection both initiate the release of a call, a release message
 *  may be received at an exchange from a succeeding or preceeding exchange after the release of the switched path
 *  is initiated and after sending a release message to the adjacent exchange.  In this case, the exchange will
 *  return a release complete message to the exchange from which the concerned message was received.  The release
 *  complete message will be sent only after the switch path has been released.  The exchange will make the
 *  circuit available for new calls when both a release complete message is received (corresponding to the sent
 *  release message) and a release complete message is sent (corresponding to the received release message).
 *
 *  Note that the proper sequences of events here is to send CC_RELEASE_IND to Call Control.  Call Control should
 *  release the circuit send CC_RELEASE_REQ to any succeeding paths and then reply with CC_RELEASE_RES only after
 *  the circuit has been verified disconnected and the circuit is reselectable.  See Q.764 Section 2.3.1.
 *
 *
 *  2.9.8.1  Inability to release in response to a release message
 *
 *  If an exchange is unable to return the circuit to the idle condition in response to a release message, it
 *  should immediately remove the circuit from service, alert the maintenance system and send the blocking
 *  message.
 *
 *  Upon receipt of the blocking acknowledgement message, the release complete message is sent in acknowledgement
 *  of the release message.
 */
STATIC int
isup_recv_rel(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: REL <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_CCR:
		if (ct_tst(ct, CCTF_COT_PENDING))
			ct_timer_stop(ct, t36);
		break;
	case CTS_ICC_WAIT_SAM:
		ct_timer_stop(ct, t35);
	case CTS_ICC_WAIT_ACM:
		if (ct_tst(ct, CCTF_COR_PENDING))
			ct_timer_stop(ct, t8);
		break;
	case CTS_ICC_WAIT_ANM:
	case CTS_ICC_SUSPENDED:
	case CTS_ICC_ANSWERED:
		break;
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
	case CTS_OGC_WAIT_CCR:
		if (ct_tst(ct, CCTF_COT_PENDING))
			ct_timer_stop(ct, t24);
		ct_timer_stop(ct, t7);
		break;
	case CTS_OGC_WAIT_ANM:
		ct_timer_stop(ct, t9);
		break;
	case CTS_OGC_SUSPENDED:
		ct_timer_stop(ct, t6);
		ct_timer_stop(ct, t38);
		/* 
		   fall through */
	case CTS_OGC_ANSWERED:
		break;
	case CTS_ICC_SEND_RLC:
		goto release_icc;
	case CTS_OGC_SEND_RLC:
		goto release_ogc;
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_t_state(ct)) {
	case CKS_WIND_CCREP:
		if ((err = cc_release_ind(q, ct->tst.cc, ct, m)))
			return (err);
		ct_set_t_state(ct, ct->tst.cc, CKS_WRES_RELIND);
		break;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_IDLE:
		break;
	case CCS_WCON_RELREQ:
		if ((err = cc_release_con(q, ct->cpc.cc, ct, m)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	default:
		if ((err = cc_release_ind(q, ct->cpc.cc, ct, m)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_WRES_RELIND);
		break;
	case CCS_WRES_RELIND:
		break;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_CCR:
		ct_set_c_state(ct, CTS_ICC_SEND_RLC);
		return (QR_DONE);
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
		/* 
		   responsibility of CC to remove loopback */
		/* 
		   release iam information */
		goto release_icc;
	case CTS_ICC_WAIT_ANM:
	case CTS_ICC_SUSPENDED:
	case CTS_ICC_ANSWERED:
		goto release_icc;
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		/* 
		   responsibility of CC to remove tone/detector */
		goto release_ogc;
	case CTS_OGC_WAIT_ANM:
		goto release_ogc;
	case CTS_OGC_SUSPENDED:
	case CTS_OGC_ANSWERED:
		/* 
		   responsibility of CC to stop charging */
		goto release_ogc;
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
		if ((err = isup_send_rlc(q, ct, NULL, 0)))
			return (err);
		// ct_set_c_state(ct, CTS_IDLE); /* still waiting for my RLC */
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
      release_icc:
	if (!ct->cpc.cc)
		goto release_problem;
	ct_set_c_state(ct, CTS_ICC_SEND_RLC);
	return (QR_DONE);
      release_ogc:
	if (!ct->cpc.cc)
		goto release_problem;
	ct_set_c_state(ct, CTS_OGC_SEND_RLC);
	return (QR_DONE);
      release_problem:
	if (!ct_tst(ct, CCTF_LOC_M_BLOCK_PENDING)) {
		/* 
		   call control disappeared for this circuit */
		if ((err = cc_maint_ind(q, ct, ISUP_MAINT_RELEASE_FAILURE)))
			return (err);
		/* 
		   Q.752 12.23 1st and delta */
		ct->stats.ct_rel_problem++;
		ct->tg.tg->stats.ct_rel_problem++;
		ct->sr.sr->stats.ct_rel_problem++;
		ct->sp.sp->stats.ct_rel_problem++;
		master.stats.ct_rel_problem++;
		return ct_block(q, ct);
	}
	return (QR_DONE);
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_SUS:        -  0x0d - Suspend
 *  -------------------------------------------------------------------------
 *  2.4.1  Suspend.  The suspend message indicates a temporary cessation of communication without releasing the
 *  call.  It can only be accepted during the conversation/data phase.
 *
 *  A suspend message can be generated by the network in response to a clearback indication from an interworking
 *  node or an on-hook condition from an analogue called party.
 *
 *  a)  Action at the destination exchange or an interworking exchange
 *
 *      On receipt of an on-hook condition in the destination exchange or a clearback signal at the interworking
 *      exchange, the exchange may send a suspend (network) message to the preceding exchange.
 *
 *  b)  Action at the intermediate exchange
 *
 *      On receipt of a suspend message the exchange will send a suspend message to the preceding exchange.
 *
 *  c)  Action at the controlling exchange (i.e, exchange controlling the call)
 *
 *      On receipt of the on-hook condition or clearback indication or suspend message, the controlling exchange
 *      starts a timer (T6) to ensure that an off-hook condition, a re-answer indication, a resume (network)
 *      message or a release message is received and, if applicable, sends a suspend (network) message to the
 *      preceding exchange.  The value of this timer (T6) is covered in Recommendation Q.118.  If the timer (T6)
 *      expires, the procedures in 2.4.3 apply.
 *
 *  d)  Actions at the incoming international exchange
 *
 *      On receipt of the on-hook condition or clearback indication or suspend (network) message, the incoming
 *      international exchange sends to the preceding exchange a suspend (network) message and starts a timer
 *      (T38), to ensure that a release message is received.  The value of this timer is covered in Recommendation
 *      Q.118.  If the timer (T38) expires, the procedure in 2.4.3 applies.  The procedure described above may not
 *      be applied in the incomining international exchange if a similar arrangement is already made in the
 *      network of the incoming country.
 */
STATIC int
isup_recv_sus(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: SUS <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_ANSWERED:
	case CTS_ICC_SUSPENDED:
	case CTS_OGC_ANSWERED:
	case CTS_OGC_SUSPENDED:
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_CONNECTED:
		if ((err = cc_suspend_ind(q, ct, m)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_SUSPENDED);
		/* 
		   fall through */
	case CCS_SUSPENDED:
		/* 
		   fall through */
		break;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_ANSWERED:
	case CTS_ICC_SUSPENDED:
		ct_set(ct, CCTF_TERM_SUSPENDED);
		ct_set_c_state(ct, CTS_ICC_SUSPENDED);
		return (QR_DONE);
	case CTS_OGC_ANSWERED:
	{
		struct tg *tg = ct->tg.tg;
		if (tg->config.flags & ISUP_TGF_CONTROLLING_EXCHANGE) {
			switch (m->msg.sus.sris & ISUP_SRIS_MASK) {
			case ISUP_SRIS_NETWORK_INITIATED:
				ct_timer_start(ct, t6);
				break;
			case ISUP_SRIS_USER_INITIATED:
				ct_timer_start(ct, t2);
				break;
			}
		}
		if ((tg->config.flags & ISUP_TGF_INCOMING_INTERNATIONAL_EXCHANGE)
		    && !(tg->config.flags & ISUP_TGF_SUSPEND_NATIONALLY_PERFORMED))
			ct_timer_start(ct, t38);
		/* 
		   fall through */
	}
	case CTS_OGC_SUSPENDED:
		ct_set(ct, CCTF_TERM_SUSPENDED);
		ct_set_c_state(ct, CTS_OGC_SUSPENDED);
		return (QR_DONE);
	}
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_RES:        -  0x0e - Resume
 *  -------------------------------------------------------------------------
 *  2.4.2  Resume.  A resume message indicates a request to recommence communication.  A request to release the
 *  call received from the calling party will override the suspend/resume sequence and the procedures given in 2.3
 *  will be followed.
 *
 *  A resume message is initiated by the network, if a suspend message had previously been sent, in response to a
 *  re-answer indication from an interworking node or an off-hook condition from an analogue called party.
 *
 *  a)  Action at the destination exchange or interworking exchange
 *
 *      On receipt of a re-answer indication at the interworking exchange or an off-hook condition in the
 *      destination exchange, the exchange may send a resume (network) message to the preceding exchange if a
 *      suspend (network) message had previously been sent.
 *
 *  b)  Action of the intermediate exchange
 *
 *      On receipt of a resume message the exchange will send a resume message to the preceding exchange.
 *
 *  c)  Action of the controlling exchange (i.e, exchange controlling the call)
 *
 *      On receipt of the off-hook condition, re-answer signal, or resume (network) message, the controlling
 *      exchange stops the timer (T6) and, if applicable, sends a resume (network) message to the preceding
 *      exchange.  On receipt of a release message the controlling exchange stops the timer T6 and releases the
 *      call according to the procedures in 2.3.
 *
 *  d)  Actions ar the incoming international exchange
 *
 *      On receipt of the off-hook condition, re-answer signal, or resume (network) message from the succeeding
 *      exchange, the incoming international exchange stops the timer (T38) and sends resume (network) message to
 *      the preceding exchange.  On receipt of a release message the incoming international exchange stops the
 *      timer T38 and releases the call according to the procedure in 2.3.
 */
STATIC int
isup_recv_res(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: RES <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_SUSPENDED:
		break;
	case CTS_OGC_SUSPENDED:
		switch (m->msg.res.sris & ISUP_SRIS_MASK) {
		case ISUP_SRIS_NETWORK_INITIATED:
			ct_timer_stop(ct, t6);
			break;
		case ISUP_SRIS_USER_INITIATED:
			ct_timer_stop(ct, t2);
			break;
		}
		ct_timer_stop(ct, t38);
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_SUSPENDED:
		if (!ct_tst(ct, CCTF_ORIG_SUSPENDED)) {
			if ((err = cc_resume_ind(q, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_CONNECTED);
		}
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_SUSPENDED:
		ct_clr(ct, CCTF_TERM_SUSPENDED);
		if (!ct_tst(ct, CCTF_ORIG_SUSPENDED))
			ct_set_c_state(ct, CTS_ICC_ANSWERED);
		return (QR_DONE);
	case CTS_OGC_SUSPENDED:
		ct_clr(ct, CCTF_TERM_SUSPENDED);
		if (!ct_tst(ct, CCTF_ORIG_SUSPENDED))
			ct_set_c_state(ct, CTS_OGC_ANSWERED);
		return (QR_DONE);
	}
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_RLC:        -  0x10 - Release complete
 *  -------------------------------------------------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: ... wehn the call is released
 *  earlier and no automatic repeat attempt is to be attempted.
 *
 *  2.9.5.1 c) if a release complete message is received relating to a busy circuit for which a release message
 *  has not been sent, the circuit will be released and a release message will be sent.
 *
 *  2.9.5.1 e) if a release complete message is received identifying one of the busy circuits being used by a
 *  multirate connection type or N x 64 kbits/s connection type call for which a release message has not been
 *  sent, the call will be cleared, all circuits made idle and a release message sent indicating the lowest
 *  circuit identification code of the multiple 64 kbit/s circuits used by the call;
 */
STATIC int
isup_recv_rlc(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: RLC <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_IDLE:
		break;
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
		if (ct_tst(ct, CCTF_COR_PENDING))
			ct_timer_stop(ct, t8);
		break;
	case CTS_ICC_WAIT_ANM:
	case CTS_ICC_ANSWERED:
	case CTS_ICC_SUSPENDED:
		break;
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		if (ct_tst(ct, CCTF_COT_PENDING))
			ct_timer_stop(ct, t24);
		ct_timer_stop(ct, t7);
		break;
	case CTS_OGC_WAIT_ANM:
		break;
	case CTS_OGC_ANSWERED:
	case CTS_OGC_SUSPENDED:
		/* 
		   stop charging */
		break;
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
		ct_timer_stop(ct, t5);
		ct_timer_stop(ct, t1);
		if (ct->rel)
			freemsg(xchg(&ct->rel, NULL));
		break;
	}
	switch (ct_get_t_state(ct)) {
	case CKS_WCON_RELREQ:
		if ((err = cc_release_con(q, ct->tst.cc, ct, m)))
			return (err);
		ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WCON_RELREQ:
		if ((err = cc_release_con(q, ct->cpc.cc, ct, m)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	case CCS_WRES_RELIND:
	case CCS_UNUSABLE:
	case CCS_IDLE:
		break;
	default:
		if ((err =
		     cc_call_failure_ind(q, ct->cpc.cc, ct, ISUP_CALL_FAILURE_RECV_RLC,
					 CC_CAUS_NORMAL_UNSPECIFIED)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	}
	switch (ct_get_m_state(ct)) {
	case CMS_IDLE:
		break;
	case CMS_WCON_RESREQ:
		if ((err = cc_reset_con(q, ct, m)))
			return (err);
		ct_set_m_state(ct, ct->mgm.cc, CMS_IDLE);
		break;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_IDLE:
		break;
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
		/* 
		   responsibility of CC to remove loopback */
		goto release_icc;
	case CTS_ICC_WAIT_ANM:
	case CTS_ICC_ANSWERED:
	case CTS_ICC_SUSPENDED:
		goto release_icc;
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		/* 
		   responsibility of CC to remove tone/detector */
		goto release_ogc;
	case CTS_OGC_WAIT_ANM:
		goto release_ogc;
	case CTS_OGC_ANSWERED:
	case CTS_OGC_SUSPENDED:
		/* 
		   CC responsibility to stop charging */
		goto release_ogc;
	case CTS_ICC_WAIT_RLC:	/* call failure or reattempt */
	case CTS_OGC_WAIT_RLC:	/* call failure or reattempt */
		/* 
		   places circuit back in trunk group idle list */
		ct_set_c_state(ct, CTS_IDLE);
		if (ct_tst(ct, CCTF_LOC_RESET_PENDING)) {
			ct_timer_stop(ct, t16);
			ct_timer_stop(ct, t17);
			ct_clr(ct, CCTF_LOC_RESET_PENDING);
		}
		return (QR_DONE);
	default:
		goto unexpected;
	}
	if (ct_tst(ct, CCTF_LOC_RESET_PENDING)) {
		ct_timer_stop(ct, t16);
		ct_timer_stop(ct, t17);
		ct_clr(ct, CCTF_LOC_RESET_PENDING);
		return (QR_DONE);
	}
      unexpected:
	/* 
	   unexpected mesage */
	return (-EOPNOTSUPP);
      release_icc:
	/* 
	   release iam information */
	if ((err = isup_send_rel(q, ct, CC_CAUS_NORMAL_UNSPECIFIED, NULL, 0)))
		return (err);
	ct_timer_start(ct, t5);
	ct_timer_start(ct, t1);
	ct_set_c_state(ct, CTS_ICC_WAIT_RLC);
	return (QR_DONE);
      release_ogc:
	/* 
	   release iam information */
	if ((err = isup_send_rel(q, ct, CC_CAUS_NORMAL_UNSPECIFIED, NULL, 0)))
		return (err);
	ct_timer_start(ct, t5);
	ct_timer_start(ct, t1);
	ct_set_c_state(ct, CTS_OGC_WAIT_RLC);
	return (QR_DONE);
}

/*
 *  ISUP_MT_CCR:        -  0x11 - Continuity check request
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_ccr(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	struct cc *ck;
	printd(("%s; %p: CCR <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_IDLE:
		/* 
		   this is an initial CCR */
		break;
	case CTS_ICC_WAIT_CCR:
		/* 
		   this is a recheck CCR */
		ct_timer_stop(ct, t27);
		ct_timer_start(ct, t36);
		break;
	default:
		goto unexpected;
	}
	if (ct_get_i_state(ct) != CCS_IDLE) {
		swerr();
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
	}
	if (!(ck = ct->tst.cc)) {
		/* 
		   look for test */
		if (((ck = ct->bind.tst) && (ck->setind < ck->maxind))
		    || ((ck = ct->cg.cg->bind.tst) && (ck->setind < ck->maxind))
		    || ((ck = ct->tg.tg->bind.tst) && (ck->setind < ck->maxind))
		    || ((ck = ct->sr.sr->bind.tst) && (ck->setind < ck->maxind))
		    || ((ck = ct->sp.sp->bind.tst) && (ck->setind < ck->maxind))
		    || ((ck = master.bind.tst) && (ck->setind < ck->maxind))
		    )
			ct_set_t_state(ct, ck, CKS_WIND_CONT);
	}
	if (ck) {
		if ((err = cc_cont_check_ind(q, ck, ct, m)))
			return (err);
		/* 
		   CC responsibility to apply loopback and respond */
		ct_set_t_state(ct, ck, CKS_WREQ_CTEST);
	} else
		__printd(("%s: MAINT: unhandled continuity check request on circuit id=%ld, cic=%ld\n", DRV_NAME, ct->id, ct->cic));
	ct_set(ct, CCTM_CONT_CHECK);
	ct_set_c_state(ct, CTS_ICC_WAIT_CCR);
	return (QR_DONE);
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_RSC:        -  0x12 - Reset circuit
 *  -------------------------------------------------------------------------
 *  2.9.3.1  Reset circuit message
 *
 *  If only a few circuits are concerned, a reset circuit message should be sent for each affected circuit.
 *
 *  On receipt of a reset circuit message, the receiving (unaffected) exchange will:
 *
 *  a)  if it is the incoming or outgoing exchange on a connection in any state of call setup or during a call,
 *      accept the message as a release message and respond by sending a release complete message, after the
 *      circuit has been made idle.
 *
 *  b)  if the circuit is in the idle condition, accept the message as a release message and respond by sending a
 *      release complete mesage.
 *
 *  c)  if it has previously sent a blocking message, or if it is unable to release the circuit as described
 *      above, respond by the blocking message.  If an incoming or outgoing call is in progress, this call should
 *      be released and the circuit returned to the "idle, blocked" state.  A release complete message is sent
 *      following the blocking message.  The blocking message should be acknowledged by the affected exchange.  If
 *      the acknowledgement is not received, the repetition procedure specified in 2.9.4 should be followed;
 *
 *  d)  if it has previously received a blocking message, respond by releasing a possible outgoing call or call
 *      attempt on the circuit, remove the blocked condition, restore the circuit to the idle state, and respond
 *      with a release complete message.
 *
 *  e)  if the message is received after the sending of an initial address message but before the receipt of a
 *      backward message relating to that call, clear the circuit and make an automatic repeat attempt on another
 *      circuit if appropriate;
 *
 *  f)  if the message is received after having sent a reset circuit message, respond by a release complete
 *      message.  After receipt of the appropriate acknowledgement message, the circuit should be made available
 *      for service;
 *
 *  g)  clear any interconnected circuits by the appropriate method (e.g., release),
 *
 *  h)  when the reset circuit message identifies a circuit being used by a multiple connection type or N x 64
 *      kbit/s connection type call, in addition, in order to make all circuits user for the call but not
 *      indicated in the reset circuit message, send reset circuit messages (or circuit group reset messages) for
 *      those circuits to the affected exchange.  Alternatively, the exchange receiving the reset message may;
 *      before completing the reset procedure, clear those circuits used for the call but not indicated in the
 *      reset message, using the normal release procedure.
 *
 *  The affected exchange will then reconstruct its memory according to the received response(s) to the reset
 *  circuit and respond to the message(s) in the normal way, i.e., blocking acknolwedgement message in response to
 *  a blocking message.
 *
 *  If no release complete message is received in acknolwedgement to the reset circuit message before 15-60
 *  seconds, (T16) the reset circuit message should be repeated.  If an aknowledgement for the message is not
 *  received withing 5-15 minutes (T17) after the initial reset circuit message, the maintenance system should be
 *  notified.  However, the sending of the reset circuit message should continue at 5-15 minutes (T17) intervals
 *  until maintenance intervention occurs.
 *
 *  2.9.3.1 b) if the circuit is in the idle condition, accept the message as a release message and respond by
 *  sending a release complete mesage.
 *
 */
STATIC int
isup_recv_rsc(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	struct cc *cm;
	printd(("%s; %p: RSC <-\n", DRV_NAME, ct));
	if (ct_get_c_state(ct) != CTS_IDLE) {
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
		case CTS_ICC_WAIT_ACM:
			if (ct_tst(ct, CCTF_COR_PENDING))
				ct_timer_stop(ct, t8);
			break;
		case CTS_OGC_WAIT_SAM:
		case CTS_OGC_WAIT_ACM:
			if (ct_tst(ct, CCTF_COT_PENDING))
				ct_timer_stop(ct, t24);
			ct_timer_stop(ct, t7);
			break;
		case CTS_OGC_SUSPENDED:
			ct_timer_stop(ct, t6);
			ct_timer_stop(ct, t38);
			break;
		case CTS_ICC_WAIT_RLC:
		case CTS_OGC_WAIT_RLC:
			ct_timer_stop(ct, t5);
			ct_timer_stop(ct, t1);
			if (ct->rel)
				freemsg(xchg(&ct->rel, NULL));
			break;
		}
		switch (ct_get_t_state(ct)) {
		case CKS_WIND_CTEST:
			ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
			break;
		}
		switch (ct_get_t_state(ct)) {
		case CKS_WCON_RELREQ:
			if ((err = cc_release_con(q, ct->tst.cc, ct, m)))
				return (err);
			ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
		}
		switch (ct_get_i_state(ct)) {
		case CCS_WCON_RELREQ:
			if ((err = cc_release_con(q, ct->cpc.cc, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			break;
		case CCS_WIND_MORE:
		case CCS_WREQ_INFO:
		case CCS_WCON_SREQ:
		case CCS_WIND_PROCEED:
			/* 
			   responsibility of CC to reattempt */
			if ((err = cc_call_reattempt_ind(q, ct->cpc.cc, ct, ISUP_REATTEMPT_RESET)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			break;
		case CCS_WRES_RELIND:
		case CCS_UNUSABLE:
		case CCS_IDLE:
			break;
		default:
			if ((err =
			     cc_call_failure_ind(q, ct->cpc.cc, ct, ISUP_CALL_FAILURE_RESET,
						 CC_CAUS_NORMAL_UNSPECIFIED)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			break;
		}
		/* 
		   responsibility of CC to remove loopback */
		/* 
		   release iam information */
		/* 
		   responsibility of CC to disconnect tone/detector */
		/* 
		   CC responsibility to stop charging */
		ct_set_c_state(ct, CTS_IDLE);
	}
	if (ct_tst(ct, CCTF_LOC_M_BLOCKED | CCTF_LOC_M_BLOCK_PENDING))
		if ((err = ct_block(q, ct)))
			return (err);
	if (ct_tst(ct, CCTF_LOC_H_BLOCKED | CCTF_LOC_H_BLOCK_PENDING))
		if ((err = cg_h_block(q, ct->cg.cg)))
			return (err);
	if (!ct_tst(ct, CCTF_REM_RESET_PENDING)) {
		/* 
		   find management to give inddication */
		if ((cm = ct->bind.mgm) || (cm = ct->cg.cg->bind.mgm) || (cm = ct->tg.tg->bind.mgm)
		    || (cm = ct->sr.sr->bind.mgm) || (cm = ct->sp.sp->bind.mgm)
		    || (cm = master.bind.mgm)) {
			if (ct_get_m_state(ct) == CMS_IDLE)
				ct_set_m_state(ct, cm, CMS_WRES_RESIND);
			if ((err = cc_reset_ind(q, cm, ct, m)))
				return (err);
			ct_set(ct, CCTF_REM_RESET_PENDING);
			// return (QR_DONE);
		} else {
			/* 
			   Note: if nobody is listening, these indications should be logged. */
			__printd(("%s: MGMT: unhandled reset indication on circuit id=%ld, cic=%ld\n", DRV_NAME, ct->id, ct->cic));
			ct_set(ct, CCTF_REM_RESET_PENDING);
		}
	}
	/* 
	   complete procedure automatically */
	if (ct_tst(ct, CCTF_REM_RESET_PENDING)) {
		if ((err = isup_send_rlc(q, ct, NULL, 0)))
			return (err);
		ct_clr(ct, (CCTF_REM_M_BLOCKED | CCTF_REM_H_BLOCKED | CCTF_REM_RESET_PENDING));
	}
	return (QR_DONE);
}

/*
 *  ISUP_MT_BLO:        -  0x13 - Blocking
 *  -------------------------------------------------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: ... wehn the call is released
 *  earlier and no automatic repeat attempt is to be attempted.
 * 
 *  IMPLEMENTATION NOTE: Even on reattempts, it is the responsibility of call control to maintain the call setup
 *  information between reattempts.
 *
 *  2.8.2.1 ... In the event of a blocking message being received, after an intial address message has been sent
 *  in the opposite direction on that circuit, and before a backward message relating to that call has been
 *  received, an automatic repeate attempt will be made on another circuit.  The exchange receiving the blocking
 *  message releases the oritinal call attempt in the normal manner after sending the blocking acknowledgement
 *  message and will not seize that circuit for subsequent calls.
 */
STATIC int
isup_recv_blo(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: BLO <-\n", DRV_NAME, ct));
	/* 
	   does not interrupt segmentation */
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		if (ct_tst(ct, CCTF_COT_PENDING))
			ct_timer_stop(ct, t24);
		ct_timer_stop(ct, t7);
		break;
	}
	switch (ct_get_t_state(ct)) {
	case CKS_WIND_CTEST:
		ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
		break;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_MORE:
	case CCS_WREQ_INFO:
	case CCS_WCON_SREQ:
	case CCS_WIND_PROCEED:
		if ((err = cc_call_reattempt_ind(q, ct->cpc.cc, ct, ISUP_REATTEMPT_BLOCKING)))
			return (err);
		/* 
		   responsibility of CC to reattempt */
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	}
	if (!ct_tst(ct, CCTF_REM_M_BLOCK_PENDING)) {
		struct cc *cm;
		if ((cm = ct->bind.mgm) || (cm = ct->cg.cg->bind.mgm) || (cm = ct->tg.tg->bind.mgm)
		    || (cm = ct->sr.sr->bind.mgm) || (cm = ct->sp.sp->bind.mgm)
		    || (cm = master.bind.mgm)) {
			if (ct_get_m_state(ct) == CMS_IDLE)
				ct_set_m_state(ct, cm, CMS_WRES_BLIND);
			if ((err = cc_blocking_ind(q, cm, ct, m)))
				return (err);
			ct_set(ct, CCTF_REM_M_BLOCK_PENDING);
			// return (QR_DONE);
		} else {
			/* 
			   Note: if nobody is listening, these indications should be logged. */
			__printd(("%s: MGMT: unhandled blocking indication on circuit id=%ld, cic=%ld\n", DRV_NAME, ct->id, ct->cic));
			ct_set(ct, CCTF_REM_M_BLOCK_PENDING);
		}
	}
	/* 
	   complete procedure automatically */
	if (ct_tst(ct, CCTF_REM_M_BLOCK_PENDING)) {
		if ((err = isup_send_bla(q, ct)))
			return (err);
		ct_set(ct, CCTF_REM_M_BLOCKED);
		ct_clr(ct, CCTF_REM_M_BLOCK_PENDING);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		/* 
		   responsibility of CC to disconnect tone/detector */
		/* 
		   CC responsibility to release iam information */
		/* 
		   does not put on idle list because remote block pending */
		ct_timer_start(ct, t5);
		ct_timer_start(ct, t1);
		if ((err = isup_send_rel(q, ct, CC_CAUS_NORMAL_UNSPECIFIED, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_OGC_WAIT_RLC);
		break;
	}
	return (QR_DONE);
}

/*
 *  ISUP_MT_UBL:        -  0x14 - Unblcoking
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_ubl(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: UBL <-\n", DRV_NAME, ct));
	if (!ct_tst(ct, CCTF_REM_M_UNBLOCK_PENDING)) {
		struct cc *cm;
		/* 
		   does not interrupt segmentation */
		/* 
		   does not affect calls in progress */
		if ((cm = ct->bind.mgm) || (cm = ct->cg.cg->bind.mgm) || (cm = ct->tg.tg->bind.mgm)
		    || (cm = ct->sr.sr->bind.mgm) || (cm = ct->sp.sp->bind.mgm)
		    || (cm = master.bind.mgm)) {
			if (ct_get_m_state(ct) == CMS_IDLE)
				ct_set_m_state(ct, cm, CMS_WRES_UBIND);
			if ((err = cc_unblocking_ind(q, cm, ct, m)))
				return (err);
			ct_set(ct, CCTF_REM_M_UNBLOCK_PENDING);
			// return (QR_DONE);
		} else {
			__printd(("%s: MGMT: unhandled unblocking indication on circuit id=%ld, cic=%ld\n", DRV_NAME, ct->id, ct->cic));
			ct_set(ct, CCTF_REM_M_UNBLOCK_PENDING);
		}
	}
	/* 
	   complete procedure automatically */
	if (ct_tst(ct, CCTF_REM_M_UNBLOCK_PENDING)) {
		if ((err = isup_send_uba(q, ct)))
			return (err);
		ct_clr(ct, CCTF_REM_M_BLOCKED);
		ct_clr(ct, CCTF_REM_M_UNBLOCK_PENDING);
	}
	return (QR_DONE);
}

/*
 *  ISUP_MT_BLA:        -  0x15 - Blocking acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_bla(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: BLA <-\n", DRV_NAME, ct));
	if (ct_tst(ct, CCTF_LOC_M_BLOCK_PENDING)) {
		if (ct_get_m_state(ct) == CMS_WCON_BLREQ) {
			if ((err = cc_blocking_con(q, ct, m)))
				return (err);
			ct_set_m_state(ct, ct->mgm.cc, CMS_IDLE);
		}
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_SEND_RLC:
		case CTS_OGC_SEND_RLC:
			/* 
			   release problem, send rlc now */
			if ((err = isup_send_rlc(q, ct, NULL, 0)))
				return (err);
			ct_set_c_state(ct, CTS_IDLE);
		}
		/* 
		   does not interrupt segmentation */
		/* 
		   does not affect calls in progress */
		/* 
		   complete procedure automatically */
		ct_timer_stop(ct, t12);
		ct_timer_stop(ct, t13);
		ct_set(ct, CCTF_LOC_M_BLOCKED);
		ct_clr(ct, CCTF_LOC_M_BLOCK_PENDING);
		return (QR_DONE);
	}
	/* 
	   unexpected message */
	if ((err = cc_maint_ind(q, ct, ISUP_MAINT_UNEXPECTED_BLA)))
		return (err);
	/* 
	   Q.752 12.14 1st and delta */
	ct->stats.ct_unexpected_bla++;
	ct->tg.tg->stats.ct_unexpected_bla++;
	ct->sr.sr->stats.ct_unexpected_bla++;
	ct->sp.sp->stats.ct_unexpected_bla++;
	master.stats.ct_unexpected_bla++;
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_UBA:        -  0x16 - Unblocking acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_uba(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: UBA <-\n", DRV_NAME, ct));
	if (ct_tst(ct, CCTF_LOC_M_UNBLOCK_PENDING)) {
		if (ct_get_m_state(ct) == CMS_WCON_UBREQ) {
			if ((err = cc_unblocking_con(q, ct, m)))
				return (err);
			ct_set_m_state(ct, ct->mgm.cc, CMS_IDLE);
		}
		/* 
		   does not interrupt segmentation */
		/* 
		   does not affect calls in progress */
		ct_timer_stop(ct, t14);
		ct_timer_stop(ct, t15);
		ct_clr(ct, CCTF_LOC_M_BLOCKED);
		ct_clr(ct, CCTF_LOC_M_UNBLOCK_PENDING);
		return (QR_DONE);
	}
	/* 
	   unexpected message */
	if ((err = cc_maint_ind(q, ct, ISUP_MAINT_UNEXPECTED_UBA)))
		return (err);
	/* 
	   Q.752 12.14 1st and delta */
	ct->stats.ct_unexpected_uba++;
	ct->tg.tg->stats.ct_unexpected_uba++;
	ct->sr.sr->stats.ct_unexpected_uba++;
	ct->sp.sp->stats.ct_unexpected_uba++;
	master.stats.ct_unexpected_uba++;
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_GRS:        -  0x17 - Circuit group reset
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_grs(queue_t *q, struct ct *bc, isup_msg_t * m)
{
	int err;
	struct cc *cm;
	int o, i, j;
	struct ct *ct;
	struct cg *cg = bc->cg.cg;
	uchar rs[RS_MAX_RANGE] = { 0, };
	printd(("%s; %p: GRS <-\n", DRV_NAME, cg));
	/* 
	   TODO: handle prearranged circuits when rs[0] == 0 */
	rs[0] = m->msg.grs.rs.ptr[0];
	for (ct = bc; ct; ct = ct->sr.next) {
		o = ct->cic - bc->cic;
		j = (o >> 3) + 1;
		i = o & 0x7;
		if (rs[0] <= o)
			break;
		if (ct_tst(ct, CCTF_LOC_M_BLOCKED))
			rs[j] |= (0x1 << i);
		ct_set(ct, CCTF_REM_RESET_PENDING);
		if (ct_get_c_state(ct) == CTS_IDLE)
			continue;
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
		case CTS_ICC_WAIT_ACM:
			if (ct_tst(ct, CCTF_COR_PENDING))
				ct_timer_stop(ct, t8);
			break;
		case CTS_OGC_WAIT_SAM:
		case CTS_OGC_WAIT_ACM:
			if (ct_tst(ct, CCTF_COT_PENDING))
				ct_timer_stop(ct, t24);
			ct_timer_stop(ct, t7);
			break;
		case CTS_OGC_SUSPENDED:
			ct_timer_stop(ct, t6);
			ct_timer_stop(ct, t38);
			break;
		case CTS_ICC_WAIT_RLC:
		case CTS_OGC_WAIT_RLC:
			ct_timer_stop(ct, t5);
			ct_timer_stop(ct, t1);
			if (ct->rel)
				freemsg(xchg(&ct->rel, NULL));
			break;
		}
		switch (ct_get_t_state(ct)) {
		case CKS_WIND_CTEST:
			ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
			break;
		case CKS_WCON_RELREQ:
			if ((err = cc_release_con(q, ct->tst.cc, ct, m)))
				return (err);
			ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
			break;
		}
		switch (ct_get_i_state(ct)) {
		case CCS_WCON_RELREQ:
			if ((err = cc_release_con(q, ct->cpc.cc, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			break;
		case CCS_WIND_MORE:
		case CCS_WREQ_INFO:
		case CCS_WCON_SREQ:
		case CCS_WIND_PROCEED:
			/* 
			   responsibility of CC to reattempt */
			if ((err = cc_call_reattempt_ind(q, ct->cpc.cc, ct, ISUP_REATTEMPT_RESET)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			break;
		case CCS_WRES_RELIND:
		case CCS_UNUSABLE:
		case CCS_IDLE:
			break;
		default:
			if ((err =
			     cc_call_failure_ind(q, ct->cpc.cc, ct, ISUP_CALL_FAILURE_RESET,
						 CC_CAUS_NORMAL_UNSPECIFIED)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			break;
		}
		/* 
		   responsibility of CC to remove loopback */
		/* 
		   release iam information */
		/* 
		   responsibility of CC to disconnect tone/detector */
		/* 
		   CC responsibility to stop charging */
		ct_set_c_state(ct, CTS_IDLE);
	}
	if (!cg_tst(cg, CCTF_LOC_H_BLOCK_PENDING))
		if (cg_tst(cg, CCTF_LOC_H_BLOCKED))
			if ((err = cg_h_block(q, cg)))
				return (err);
	if (!cg_tst(cg, CCTF_REM_RESET_PENDING)) {
		/* 
		   find management to give inddication */
		if ((cm = cg->bind.mgm) || (cm = cg->sr.sr->bind.mgm) || (cm = cg->sp.sp->bind.mgm)
		    || (cm = master.bind.mgm)) {
			if (cg_get_m_state(cg) == CMS_IDLE)
				cg_set_m_state(cg, cm, CMS_WRES_RESIND);
			if ((err = cc_reset_ind(q, cm, bc, m)))
				return (err);
			cg_set(cg, CCTF_REM_RESET_PENDING);
			// return (QR_DONE);
		} else {
			/* 
			   Note: if nobody is listening, these indications should be logged. */
			__printd(("%s: MGMT: unhandled reset indication on circuit group id=%ld, cic=%ld\n", DRV_NAME, cg->id, ct->cic));
			cg_set(cg, CCTF_REM_RESET_PENDING);
		}
	}
	/* 
	   complete procedure automatically */
	if (cg_tst(cg, CCTF_REM_RESET_PENDING)) {
		if ((err = isup_send_gra(q, bc, rs, ((rs[0] + 7) >> 3) + 1)))
			return (err);
		for (ct = bc; ct; ct = ct->sr.next)
			if (ct_tst(ct, CCTF_REM_RESET_PENDING))
				ct_clr(ct,
				       (CCTF_REM_M_BLOCKED | CCTF_REM_H_BLOCKED |
					CCTF_REM_RESET_PENDING));
		cg_clr(cg, (CCTF_REM_M_BLOCKED | CCTF_REM_H_BLOCKED | CCTF_REM_RESET_PENDING));
	}
	return (QR_DONE);
}

/*
 *  ISUP_MT_CGB:        -  0x18 - Circuit group blocking
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cgb(queue_t *q, struct ct *bc, isup_msg_t * m)
{
	int err;
	struct ct *ct;
	struct cg *cg = bc->cg.cg;
	int o, i, j;
	uchar rs[RS_MAX_RANGE] = { 0, };
	printd(("%s; %p: CGB <-\n", DRV_NAME, cg));
	rs[0] = m->msg.cgb.rs.ptr[0];	/* TODO: handle prearranged */
	switch (m->msg.cgb.cgi & 0x03) {
	case ISUP_MAINTENANCE_ORIENTED:
		for (ct = bc; ct; ct = ct->sr.next) {	/* walk sr list */
			o = ct->cic - bc->cic;
			j = (o >> 3) + 1;
			i = o & 0x7;
			if (rs[0] <= o)
				break;
			if (!(m->msg.cgb.rs.ptr[j] & (0x1 << i)))
				continue;
			rs[j] |= (0x1 << i);
			ct_set(ct, CCTF_REM_G_BLOCK_PENDING);
			if (ct_get_c_state(ct) == CTS_IDLE)
				continue;
			/* 
			   does not interrupt segmentation */
			switch (ct_get_c_state(ct)) {
			case CTS_OGC_WAIT_SAM:
			case CTS_OGC_WAIT_ACM:
				if (ct_tst(ct, CCTF_COT_PENDING))
					ct_timer_stop(ct, t24);
				ct_timer_stop(ct, t7);
				break;
			}
			switch (ct_get_t_state(ct)) {
			case CKS_WIND_CTEST:
				ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
				break;
			}
			switch (ct_get_i_state(ct)) {
			case CCS_WIND_MORE:
			case CCS_WREQ_INFO:
			case CCS_WCON_SREQ:
			case CCS_WIND_PROCEED:
				if ((err =
				     cc_call_reattempt_ind(q, ct->cpc.cc, ct,
							   ISUP_REATTEMPT_BLOCKING)))
					return (err);
				/* 
				   responsibility of CC to reattempt */
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
				break;
			}
			switch (ct_get_c_state(ct)) {
			case CTS_OGC_WAIT_SAM:
			case CTS_OGC_WAIT_ACM:
				/* 
				   responsibility of CC to disconnect tone/detector */
				/* 
				   CC responsibility to release iam information */
				/* 
				   does not put on idle list because remote blocked */
				ct_set_c_state(ct, CTS_IDLE);
				break;
			}
		}
		if (!cg_tst(cg, CCTF_REM_M_BLOCK_PENDING)) {
			struct cc *cm;
			if ((cm = cg->bind.mgm) || (cm = cg->sr.sr->bind.mgm)
			    || (cm = cg->sp.sp->bind.mgm)
			    || (cm = master.bind.mgm)) {
				if (cg_get_m_state(cg) == CMS_IDLE)
					cg_set_m_state(cg, cm, CMS_WRES_BLIND);
				if ((err = cc_blocking_ind(q, cm, bc, m)))
					return (err);
				cg_set(cg, CCTF_REM_M_BLOCK_PENDING);
				// return (QR_DONE);
			} else {
				/* 
				   Note: if nobody is listening, these indications should be
				   logged. */
				__printd(("%s: MGMT: unhandled blocking indication on circuit group id=%ld, cic=%ld\n", DRV_NAME, cg->id, cg->cic));
				cg_set(cg, CCTF_REM_M_BLOCK_PENDING);
			}
		}
		/* 
		   complete procedure automatically */
		if (cg_tst(cg, CCTF_REM_M_BLOCK_PENDING)) {
			if ((err = isup_send_cgba(q, bc, m->msg.cgb.cgi, rs, m->msg.cgb.rs.len)))
				return (err);
			for (ct = bc; ct; ct = ct->sr.next)
				if (ct_tst(ct, CCTF_REM_G_BLOCK_PENDING)) {
					ct_set(ct, CCTF_REM_M_BLOCKED);
					ct_clr(ct, CCTF_REM_G_BLOCK_PENDING);
				}
			cg_set(cg, CCTF_REM_M_BLOCKED);
			cg_clr(cg, CCTF_REM_M_BLOCK_PENDING);
		}
		return (QR_DONE);
	case ISUP_HARDWARE_FAILURE_ORIENTED:
		for (ct = bc; ct; ct = ct->sr.next) {	/* walk sr list */
			o = ct->cic - bc->cic;
			j = (o >> 3) + 1;
			i = o & 0x7;
			if (rs[0] <= o)
				break;
			if (!(m->msg.cgb.rs.ptr[j] & (0x1 << i)))
				continue;
			rs[j] |= (0x1 << i);
			ct_set(ct, CCTF_REM_G_BLOCK_PENDING);
			if (ct_get_c_state(ct) == CTS_IDLE)
				continue;
			switch (ct_get_c_state(ct)) {
			case CTS_ICC_WAIT_SAM:
				ct_timer_stop(ct, t35);
			case CTS_ICC_WAIT_ACM:
				if (ct_tst(ct, CCTF_COR_PENDING))
					ct_timer_stop(ct, t8);
				break;
			case CTS_ICC_WAIT_ANM:
				ct_timer_stop(ct, t9);
				break;
			case CTS_OGC_WAIT_SAM:
				ct_timer_stop(ct, t35);
			case CTS_OGC_WAIT_ACM:
				ct_timer_stop(ct, t7);
				break;
			case CTS_OGC_WAIT_ANM:
				ct_timer_stop(ct, t9);
				break;
			case CTS_ICC_WAIT_RLC:
			case CTS_OGC_WAIT_RLC:
				ct_timer_stop(ct, t5);
				ct_timer_stop(ct, t1);
				if (ct->rel)
					freemsg(xchg(&ct->rel, NULL));
				break;
			}
			switch (ct_get_t_state(ct)) {
			case CKS_WCON_RELREQ:
				/* 
				   confirm release */
				if ((err = cc_release_con(q, ct->tst.cc, ct, NULL)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
				break;
			}
			switch (ct_get_i_state(ct)) {
			case CCS_WCON_RELREQ:
				/* 
				   confirm release */
				if ((err = cc_release_con(q, ct->cpc.cc, ct, NULL)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
				break;
			case CCS_WRES_RELIND:
				/* 
				   await response */
				break;
			case CCS_IDLE:
				break;
			default:
				/* 
				   CC responsibility to abort COT */
				if ((err =
				     cc_call_failure_ind(q, ct->cpc.cc, ct,
							 ISUP_CALL_FAILURE_BLOCKING,
							 CC_CAUS_TEMPORARY_FAILURE)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
				break;
			}
			/* 
			   keep from being added to idle list */
			ct_set_c_state(ct, CTS_IDLE);
		}
		if (!cg_tst(cg, CCTF_REM_H_BLOCK_PENDING)) {
			struct cc *cm;
			if ((cm = cg->bind.mgm) || (cm = cg->sr.sr->bind.mgm)
			    || (cm = cg->sp.sp->bind.mgm)
			    || (cm = master.bind.mgm)) {
				if (cg_get_m_state(cg) == CMS_IDLE)
					cg_set_m_state(cg, cm, CMS_WRES_BLIND);
				if ((err = cc_blocking_ind(q, cm, bc, m)))
					return (err);
				cg_set(cg, CCTF_REM_H_BLOCK_PENDING);
				// return (QR_DONE);
			} else {
				/* 
				   Note: if nobody is listening, these indications should be
				   logged. */
				__printd(("%s: MGMT: unhandled blocking indication on circuit group id=%ld, cic=%ld\n", DRV_NAME, cg->id, cg->cic));
				cg_set(cg, CCTF_REM_H_BLOCK_PENDING);
			}
		}
		/* 
		   complete procedure automatically */
		if (cg_tst(cg, CCTF_REM_H_BLOCK_PENDING)) {
			if ((err = isup_send_cgba(q, bc, m->msg.cgb.cgi, rs, m->msg.cgb.rs.len)))
				return (err);
			for (ct = bc; ct; ct = ct->sr.next)
				if (ct_tst(ct, CCTF_REM_G_BLOCK_PENDING)) {
					ct_set(ct, CCTF_REM_H_BLOCKED);
					ct_clr(ct, CCTF_REM_G_BLOCK_PENDING);
				}
			cg_set(cg, CCTF_REM_H_BLOCKED);
			cg_clr(cg, CCTF_REM_H_BLOCK_PENDING);
		}
		return (QR_DONE);
	}
	/* 
	   bad mandatory parameter value */
	return (-EINVAL);
}

/*
 *  ISUP_MT_CGU:        -  0x19 - Circuit group unblocking
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cgu(queue_t *q, struct ct *bc, isup_msg_t * m)
{
	int err;
	struct ct *ct;
	struct cg *cg = bc->cg.cg;
	int o, i, j;
	uchar rs[RS_MAX_RANGE] = { 0, };
	printd(("%s; %p: CGU <-\n", DRV_NAME, cg));
	rs[0] = m->msg.cgu.rs.ptr[0];	/* TODO: handle prearranged */
	switch (m->msg.cgu.cgi & 0x03) {
	case ISUP_MAINTENANCE_ORIENTED:
		for (ct = bc; ct; ct = ct->sr.next) {
			o = ct->cic - bc->cic;
			j = (o >> 3) + 1;
			i = o & 0x7;
			if (rs[0] <= o)
				break;
			if (!(m->msg.cgu.rs.ptr[j] & (0x1 << i)))
				continue;
			rs[j] |= (0x1 << i);
			ct_set(ct, CCTF_REM_G_UNBLOCK_PENDING);
		}
		if (!cg_tst(cg, CCTF_REM_M_UNBLOCK_PENDING)) {
			struct cc *cm;
			if ((cm = cg->bind.mgm) || (cm = cg->sr.sr->bind.mgm)
			    || (cm = cg->sp.sp->bind.mgm)
			    || (cm = master.bind.mgm)) {
				if (cg_get_m_state(cg) == CMS_IDLE)
					cg_set_m_state(cg, cm, CMS_WRES_UBIND);
				if ((err = cc_unblocking_ind(q, cm, bc, m)))
					return (err);
				cg_set(cg, CCTF_REM_M_UNBLOCK_PENDING);
				// return (QR_DONE);
			} else {
				__printd(("%s: MGMT: unhandled unblocking indication on circuit group id=%ld, cic=%ld\n", DRV_NAME, cg->id, cg->cic));
				cg_set(cg, CCTF_REM_M_UNBLOCK_PENDING);
			}
		}
		/* 
		   complete procedure automatically */
		if (cg_tst(cg, CCTF_REM_M_UNBLOCK_PENDING)) {
			if ((err = isup_send_cgua(q, bc, m->msg.cgu.cgi, rs, m->msg.cgu.rs.len)))
				return (err);
			for (ct = bc; ct; ct = ct->sr.next)
				if (ct_tst(ct, CCTF_REM_G_UNBLOCK_PENDING)) {
					ct_clr(ct, CCTF_REM_M_BLOCKED);
					ct_clr(ct, CCTF_REM_G_UNBLOCK_PENDING);
				}
			cg_clr(cg, CCTF_REM_M_BLOCKED);
			cg_clr(cg, CCTF_REM_M_UNBLOCK_PENDING);
		}
		return (QR_DONE);
	case ISUP_HARDWARE_FAILURE_ORIENTED:
		for (ct = bc; ct; ct = ct->sr.next) {
			o = ct->cic - bc->cic;
			j = (o >> 3) + 1;
			i = o & 0x7;
			if (rs[0] <= o)
				break;
			if (!(m->msg.cgu.rs.ptr[j] & (0x1 << i)))
				continue;
			rs[j] |= (0x1 << i);
			ct_set(ct, CCTF_REM_G_UNBLOCK_PENDING);
		}
		if (!cg_tst(cg, CCTF_REM_H_UNBLOCK_PENDING)) {
			struct cc *cm;
			if ((cm = cg->bind.mgm) || (cm = cg->sr.sr->bind.mgm)
			    || (cm = cg->sp.sp->bind.mgm)
			    || (cm = master.bind.mgm)) {
				if (cg_get_m_state(cg) == CMS_IDLE)
					cg_set_m_state(cg, cm, CMS_WRES_UBIND);
				if ((err = cc_unblocking_ind(q, cm, bc, m)))
					return (err);
				cg_set(cg, CCTF_REM_H_UNBLOCK_PENDING);
				// return (QR_DONE);
			} else {
				__printd(("%s: MGMT: unhandled unblocking indication on circuit group id=%ld, cic=%ld\n", DRV_NAME, cg->id, cg->cic));
				cg_set(cg, CCTF_REM_H_UNBLOCK_PENDING);
			}
		}
		/* 
		   complete procedure automatically */
		if (cg_tst(cg, CCTF_REM_H_UNBLOCK_PENDING)) {
			if ((err = isup_send_cgua(q, bc, m->msg.cgu.cgi, rs, m->msg.cgu.rs.len)))
				return (err);
			for (ct = bc; ct; ct = ct->sr.next)
				if (ct_tst(ct, CCTF_REM_G_UNBLOCK_PENDING)) {
					ct_clr(ct, CCTF_REM_H_BLOCKED);
					ct_clr(ct, CCTF_REM_G_UNBLOCK_PENDING);
				}
			cg_clr(cg, CCTF_REM_H_BLOCKED);
			cg_clr(cg, CCTF_REM_H_UNBLOCK_PENDING);
		}
		return (QR_DONE);
	}
	/* 
	   bad mandatory parameter value */
	return (-EINVAL);
}

/*
 *  ISUP_MT_CGBA:       -  0x1a - Circuit group blocking acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cgba(queue_t *q, struct ct *bc, isup_msg_t * m)
{
	int err;
	int o, i, j, ask, ack;
	struct ct *ct;
	struct cg *cg = bc->cg.cg;
	ulong CCTF_LOC_X_BLOCKED;
	ulong CCTF_LOC_X_BLOCK_PENDING;
	printd(("%s; %p: CGBA <-\n", DRV_NAME, cg));
	if (cg->cic != bc->cic || cg->rs_len != m->msg.cgba.rs.len
	    || cg->rs_ptr[0] != m->msg.cgba.rs.ptr[0])
		goto unexpected;
	switch (m->msg.cgba.cgi & 0x03) {
	case ISUP_MAINTENANCE_ORIENTED:
		CCTF_LOC_X_BLOCKED = CCTF_LOC_M_BLOCKED;
		CCTF_LOC_X_BLOCK_PENDING = CCTF_LOC_M_BLOCK_PENDING;
		break;
	case ISUP_HARDWARE_FAILURE_ORIENTED:
		CCTF_LOC_X_BLOCKED = CCTF_LOC_H_BLOCKED;
		CCTF_LOC_X_BLOCK_PENDING = CCTF_LOC_H_BLOCK_PENDING;
		break;
	default:
		/* 
		   bad mandatory parameter value */
		return (-EINVAL);
	}
	if (!cg_tst(cg, CCTF_LOC_X_BLOCK_PENDING))
		goto unexpected;
	if (cg_get_m_state(cg) == CMS_WCON_BLREQ) {
		if ((err = cc_blocking_con(q, bc, m)))
			return (err);
		cg_set_m_state(cg, cg->gmg.cc, CMS_IDLE);
	}
	cg_timer_stop(cg, t18);
	cg_timer_stop(cg, t19);
	for (ct = bc; ct; ct = ct->cg.next) {
		o = ct->cic - bc->cic;
		i = (o >> 3) + 1;
		j = o & 0x7;
		ask = cg->rs_ptr[i] & (0x1 << j);
		ack = m->msg.cgba.rs.ptr[i] & (0x1 << j);
		if (ask && ack) {
			ct_set(ct, CCTF_LOC_X_BLOCKED);
			ct_clr(ct, CCTF_LOC_S_BLOCKED);
			ct_clr(ct, CCTF_LOC_G_BLOCK_PENDING);
			continue;
		}
		if (ask && !ack) {
			/* 
			   missing acknowledgement */
			if ((err = cc_maint_ind(q, ct, ISUP_MAINT_MISSING_ACK_IN_CGBA)))
				return (err);
			/* 
			   Q.752 12.8 1st and delta */
			ct->stats.ct_missing_ack_cgba++;
			cg->stats.ct_missing_ack_cgba++;
			ct->tg.tg->stats.ct_missing_ack_cgba++;
			ct->sr.sr->stats.ct_missing_ack_cgba++;
			ct->sp.sp->stats.ct_missing_ack_cgba++;
			master.stats.ct_missing_ack_cgba++;
			cg->rs_ptr[i] &= ~(0x1 << j);	/* cancel ask */
			ct_set(ct, CCTF_LOC_S_BLOCKED);
			ct_clr(ct, CCTF_LOC_G_BLOCK_PENDING);
			continue;
		}
		if (!ask && ack) {
			/* 
			   abnormal acknowledgement */
			if ((err = cc_maint_ind(q, ct, ISUP_MAINT_ABNORMAL_ACK_IN_CGBA)))
				return (err);
			/* 
			   Q.752 12.10 1st and delta */
			ct->stats.ct_abnormal_ack_cgba++;
			cg->stats.ct_abnormal_ack_cgba++;
			ct->tg.tg->stats.ct_abnormal_ack_cgba++;
			ct->sr.sr->stats.ct_abnormal_ack_cgba++;
			ct->sp.sp->stats.ct_abnormal_ack_cgba++;
			master.stats.ct_abnormal_ack_cgba++;
			cg->rs_ptr[i] &= (0x1 << j);	/* set ask */
			ct_set(ct, CCTF_LOC_X_BLOCKED);
			ct_clr(ct, CCTF_LOC_G_BLOCK_PENDING);
		}
	}
	for (i = 1; i < cg->rs_len; i++) {
		for (j = 0; j < 8; j++) {
			ulong cic = bc->cic + (j << 3) + i;
			ask = cg->rs_ptr[i] & (0x1 << j);
			ack = m->msg.cgba.rs.ptr[i] & (0x1 << j);
			if (!ask & ack) {
				/* 
				   abnormal acknowledgement */
				if ((err =
				     cc_cg_maint_ind(q, cg, cic, ISUP_MAINT_ABNORMAL_ACK_IN_CGBA)))
					return (err);
				/* 
				   Q.752 12.10 1st and delta */
				ct->stats.ct_abnormal_ack_cgba++;
				cg->stats.ct_abnormal_ack_cgba++;
				ct->tg.tg->stats.ct_abnormal_ack_cgba++;
				ct->sr.sr->stats.ct_abnormal_ack_cgba++;
				ct->sp.sp->stats.ct_abnormal_ack_cgba++;
				master.stats.ct_abnormal_ack_cgba++;
			}
		}
	}
	cg_set(cg, CCTF_LOC_X_BLOCKED);
	cg_clr(cg, CCTF_LOC_X_BLOCK_PENDING);
	cg->cic = cg->ct.list->cic;	/* adjust base cic */
	return (QR_DONE);
      unexpected:
	if ((err = cc_maint_ind(q, bc, ISUP_MAINT_UNEXPECTED_CGBA)))
		return (err);
	/* 
	   Q.752 12.12 1st and delta */
	cg->stats.cg_unexpected_cgba++;
	cg->sr.sr->stats.cg_unexpected_cgba++;
	cg->sp.sp->stats.cg_unexpected_cgba++;
	master.stats.cg_unexpected_cgba++;
	return (QR_DONE);
}

/*
 *  ISUP_MT_CGUA:       -  0x1b - Circuit group unblocking acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cgua(queue_t *q, struct ct *bc, isup_msg_t * m)
{
	int err;
	struct ct *ct;
	struct cg *cg = bc->cg.cg;
	int o, i, j, ask, ack;
	ulong CCTF_LOC_X_BLOCKED;
	ulong CCTF_LOC_X_UNBLOCK_PENDING;
	printd(("%s; %p: CGUA <-\n", DRV_NAME, cg));
	if (cg->cic != bc->cic || cg->rs_len != m->msg.cgua.rs.len
	    || cg->rs_ptr[0] != m->msg.cgua.rs.ptr[0])
		goto unexpected;
	switch (m->msg.cgua.cgi & 0x03) {
	case ISUP_MAINTENANCE_ORIENTED:
		CCTF_LOC_X_BLOCKED = CCTF_LOC_M_BLOCKED;
		CCTF_LOC_X_UNBLOCK_PENDING = CCTF_LOC_M_UNBLOCK_PENDING;
		break;
	case ISUP_HARDWARE_FAILURE_ORIENTED:
		CCTF_LOC_X_BLOCKED = CCTF_LOC_H_BLOCKED;
		CCTF_LOC_X_UNBLOCK_PENDING = CCTF_LOC_H_UNBLOCK_PENDING;
		break;
	default:
		/* 
		   bad mandatory parameter value */
		return (-EINVAL);
	}
	if (!cg_tst(cg, CCTF_LOC_X_UNBLOCK_PENDING))
		goto unexpected;
	if (cg_get_m_state(cg) == CMS_WCON_UBREQ) {
		if ((err = cc_unblocking_con(q, bc, m)))
			return (err);
		cg_set_m_state(cg, cg->gmg.cc, CMS_IDLE);
	}
	cg_timer_stop(cg, t20);
	cg_timer_stop(cg, t21);
	for (ct = bc; ct; ct = ct->cg.next) {
		o = ct->cic - bc->cic;
		i = (o >> 3) + 1;
		j = o & 0x7;
		ask = cg->rs_ptr[i] & (0x1 << j);
		ack = m->msg.cgua.rs.ptr[i] & (0x1 << j);
		if (ask && ack) {
			ct_clr(ct, CCTF_LOC_X_BLOCKED);
			ct_clr(ct, CCTF_LOC_S_BLOCKED);
			ct_clr(ct, CCTF_LOC_G_UNBLOCK_PENDING);
			continue;
		}
		if (ask && !ack) {
			/* 
			   missing acknowledgement */
			if ((err = cc_maint_ind(q, ct, ISUP_MAINT_MISSING_ACK_IN_CGUA)))
				return (err);
			/* 
			   Q.752 12.9 1st and delta */
			ct->stats.ct_missing_ack_cgua++;
			cg->stats.ct_missing_ack_cgua++;
			ct->tg.tg->stats.ct_missing_ack_cgua++;
			ct->sr.sr->stats.ct_missing_ack_cgua++;
			ct->sp.sp->stats.ct_missing_ack_cgua++;
			master.stats.ct_missing_ack_cgua++;
			cg->rs_ptr[i] &= ~(0x1 << j);	/* cancel ask */
			ct_set(ct, CCTF_LOC_S_BLOCKED);
			ct_clr(ct, CCTF_LOC_G_UNBLOCK_PENDING);
			continue;
		}
		if (!ask && ack) {
			/* 
			   abnormal acknowledgement */
			if ((err = cc_maint_ind(q, ct, ISUP_MAINT_ABNORMAL_ACK_IN_CGUA)))
				return (err);
			/* 
			   Q.752 12.11 1st and delta */
			ct->stats.ct_abnormal_ack_cgua++;
			cg->stats.ct_abnormal_ack_cgua++;
			ct->tg.tg->stats.ct_abnormal_ack_cgua++;
			ct->sr.sr->stats.ct_abnormal_ack_cgua++;
			ct->sp.sp->stats.ct_abnormal_ack_cgua++;
			master.stats.ct_abnormal_ack_cgua++;
			cg->rs_ptr[i] &= (0x1 << j);	/* set ask */
			ct_clr(ct, CCTF_LOC_X_BLOCKED);
			ct_clr(ct, CCTF_LOC_G_UNBLOCK_PENDING);
		}
	}
	for (i = 1; i < cg->rs_len; i++) {
		for (j = 0; j < 8; j++) {
			ulong cic = bc->cic + (j << 3) + i;
			ask = cg->rs_ptr[i] & (0x1 << j);
			ack = m->msg.cgua.rs.ptr[i] & (0x1 << j);
			if (!ask & ack) {
				/* 
				   abnormal acknowledgement */
				if ((err =
				     cc_cg_maint_ind(q, cg, cic, ISUP_MAINT_ABNORMAL_ACK_IN_CGUA)))
					return (err);
				/* 
				   Q.752 12.10 1st and delta */
				ct->stats.ct_abnormal_ack_cgua++;
				cg->stats.ct_abnormal_ack_cgua++;
				ct->tg.tg->stats.ct_abnormal_ack_cgua++;
				ct->sr.sr->stats.ct_abnormal_ack_cgua++;
				ct->sp.sp->stats.ct_abnormal_ack_cgua++;
				master.stats.ct_abnormal_ack_cgua++;
			}
		}
	}
	cg_clr(cg, CCTF_LOC_X_BLOCKED);
	cg_clr(cg, CCTF_LOC_X_UNBLOCK_PENDING);
	cg->cic = cg->ct.list->cic;	/* adjust base cic */
	return (QR_DONE);
      unexpected:
	if ((err = cc_maint_ind(q, bc, ISUP_MAINT_UNEXPECTED_CGUA)))
		return (err);
	/* 
	   Q.752 12.13 1st and delta */
	cg->stats.cg_unexpected_cgua++;
	cg->sr.sr->stats.cg_unexpected_cgua++;
	cg->sp.sp->stats.cg_unexpected_cgua++;
	master.stats.cg_unexpected_cgua++;
	return (QR_DONE);
}

/*
 *  ISUP_MT_CMR:        -  0x1c - Call Modification Request (not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cmr(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CMR <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_CMC:        -  0x1d - Call Modification Completed (not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cmc(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CMC <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_CMRJ:       -  0x1e - Call Modification Reject (not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cmrj(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CMRJ <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_FAR:        -  0x1f - Facility request (not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_far(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: FAR <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_FAA:        -  0x20 - Facility accepted (Not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_faa(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: FAA <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_FRJ:        -  0x21 - Facility reject (not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_frj(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: FRJ <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_FAD:        -  0x22 - Facility Deactivated (Bellcore only, not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_fad(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: FAD <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_FAI:        -  0x23 - Facility Information (Bellcore only)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_fai(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: FAI <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_LPA:        -  0x24 - Loop back acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_lpa(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: LPA <-\n", DRV_NAME, ct));
	if (ct->tg.tg->proto.popt & SS7_POPT_LPA) {
		if (!ct_tst(ct, CCTF_LPA_PENDING))
			goto unexpected;
		switch (ct_get_t_state(ct)) {
		case CKS_IDLE:
			break;
		case CKS_WIND_CTEST:
			if ((err = cc_cont_test_ind(q, ct->tst.cc, ct)))
				return (err);
			ct_set_t_state(ct, ct->tst.cc, CKS_WREQ_CCREP);
			break;
		default:
			goto unexpected;
		}
		ct_clr(ct, CCTF_LPA_PENDING);
		ct_timer_start(ct, t24);
		return (QR_DONE);
	}
      unexpected:
	/* 
	   unexpected */
	/* 
	   not supported */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_DRS:        -  0x27 - Delayed release (not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_drs(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: DRS <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_PAM:        -  0x28 - Pass along
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_pam(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: PAM <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_GRA:        -  0x29 - Circuit group reset acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_gra(queue_t *q, struct ct *bc, isup_msg_t * m)
{
	int err;
	int o, i, j, mblock;
	struct ct *ct;
	struct cg *cg = bc->cg.cg;
	printd(("%s; %p: GRA <-\n", DRV_NAME, cg));
	if (cg->cic != bc->cic || cg->rs_ptr[0] != m->msg.gra.rs.ptr[0])
		goto unexpected;
	if (!cg_tst(cg, CCTF_LOC_RESET_PENDING))
		goto unexpected;
	if (cg_get_m_state(cg) == CMS_WCON_RESREQ) {
		if ((err = cc_reset_con(q, bc, m)))
			return (err);
		cg_set_m_state(cg, cg->gmg.cc, CMS_IDLE);
	}
	cg_timer_stop(cg, t22);
	cg_timer_stop(cg, t23);
	for (ct = bc; ct; ct = ct->cg.next) {
		o = ct->cic - bc->cic;
		i = (o >> 3) + 1;
		j = o & 0x7;
		mblock = m->msg.gra.rs.ptr[i] & (0x1 << j);
		if (mblock)
			ct_set(ct, CCTF_REM_M_BLOCKED);
		else
			ct_clr(ct, CCTF_REM_M_BLOCKED);
		ct_clr(ct, CCTF_LOC_RESET_PENDING);
	}
	cg_clr(cg, CCTF_LOC_RESET_PENDING);
	cg->cic = cg->ct.list->cic;	/* adjust base cic */
	return (QR_DONE);
      unexpected:
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_CQM:        -  0x2a - Circuit group query
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cqm(queue_t *q, struct ct *bc, isup_msg_t * m)
{
	int err;
	struct ct *ct;
	struct cg *cg = bc->cg.cg;
	int j;
	uchar csi[RS_MAX_RANGE] = { 0, };
	printd(("%s; %p: CQM <-\n", DRV_NAME, cg));
	if (!cg_tst(cg, CCTF_REM_QUERY_PENDING)) {
		struct cc *cm;
		for (ct = bc; ct; ct = ct->sr.next) {
			j = ct->cic - bc->cic;
			if (m->msg.cqm.rs.ptr[0] <= j)
				break;
			ct_set(ct, CCTF_REM_QUERY_PENDING);
		}
		if ((cm = cg->bind.mgm) || (cm = cg->sr.sr->bind.mgm) || (cm = cg->sp.sp->bind.mgm)
		    || (cm = master.bind.mgm)) {
			if (cg_get_m_state(cg) == CMS_IDLE)
				cg_set_m_state(cg, cm, CMS_WRES_QRYIND);
			if ((err = cc_query_ind(q, bc, m)))
				return (err);
			cg_set(cg, CCTF_REM_QUERY_PENDING);
			// return (QR_DONE);
		} else {
			/* 
			   Note: if nobody is listening, these indications should be logged. */
			__printd(("%s: MGMT: unhandled query indication on circuit group id=%ld, cic=%ld\n", DRV_NAME, cg->id, cg->cic));
			cg_set(cg, CCTF_REM_QUERY_PENDING);
		}
	}
	/* 
	   complete procedure automatically */
	for (j = 0; j < RS_MAX_RANGE; j++)
		csi[j] = 0x3;	/* unequipped */
	for (ct = bc; ct; ct = ct->sr.next) {
		j = ct->cic - bc->cic;
		if (j >= m->msg.cqm.rs.ptr[0])
			break;
		csi[j] = 0;
		csi[j] |= 0x0;	/* transient */
		switch (ct_tst(ct, (CCTF_LOC_M_BLOCKED | CCTF_REM_M_BLOCKED))) {
		case 0:
			csi[j] |= 0x0;
			break;
		case CCTF_LOC_M_BLOCKED:
			csi[j] |= 0x1;
			break;
		case CCTF_REM_M_BLOCKED:
			csi[j] |= 0x2;
			break;
		case CCTF_LOC_M_BLOCKED | CCTF_REM_M_BLOCKED:
			csi[j] |= 0x3;
			break;
		}
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
		case CTS_ICC_WAIT_ACM:
		case CTS_ICC_WAIT_ANM:
		case CTS_ICC_ANSWERED:
		case CTS_ICC_SUSPENDED:
		case CTS_ICC_WAIT_RLC:
		case CTS_ICC_SEND_RLC:
			csi[j] |= 0x04;
			break;
		case CTS_OGC_WAIT_SAM:
		case CTS_OGC_WAIT_ACM:
		case CTS_OGC_WAIT_ANM:
		case CTS_OGC_ANSWERED:
		case CTS_OGC_SUSPENDED:
		case CTS_OGC_WAIT_RLC:
		case CTS_OGC_SEND_RLC:
			csi[j] |= 0x08;
			break;
		case CTS_IDLE:
			csi[j] |= 0x0c;
			break;
		}
		switch (ct_tst(ct, (CCTF_LOC_H_BLOCKED | CCTF_REM_H_BLOCKED))) {
		case 0:
			csi[j] |= 0x00;
			break;
		case CCTF_LOC_H_BLOCKED:
			csi[j] |= 0x10;
			break;
		case CCTF_REM_H_BLOCKED:
			csi[j] |= 0x20;
			break;
		case CCTF_LOC_H_BLOCKED | CCTF_REM_H_BLOCKED:
			csi[j] |= 0x30;
			break;
		}
	}
	if ((err = isup_send_cqr(q, bc, m->msg.cqm.rs.ptr, 1, csi, m->msg.cqm.rs.ptr[0])))
		return (err);
	for (ct = bc; ct; ct = ct->sr.next) {
		j = ct->cic - bc->cic;
		if (j >= m->msg.cqm.rs.ptr[0])
			break;
		ct_clr(ct, CCTF_REM_QUERY_PENDING);
	}
	cg_clr(cg, CCTF_REM_QUERY_PENDING);
	return (QR_DONE);
}

/*
 *  ISUP_MT_CQR:        -  0x2b - Circuit group query response
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cqr(queue_t *q, struct ct *bc, isup_msg_t * m)
{
	int err;
	int j;
	struct ct *ct;
	struct cg *cg = bc->cg.cg;
	printd(("%s; %p: CQR <-\n", DRV_NAME, cg));
	if (cg->cic != bc->cic || cg->rs_ptr[0] != m->msg.cqr.rs.ptr[0])
		goto unexpected;
	if (!(cg_tst(cg, CCTF_LOC_QUERY_PENDING)))
		goto unexpected;
	if (cg_get_m_state(cg) == CMS_WCON_QRYREQ) {
		if ((err = cc_query_con(q, bc, m)))
			return (err);
		cg_set_m_state(cg, cg->gmg.cc, CMS_IDLE);
	}
	cg_timer_stop(cg, t28);
	for (ct = bc; ct; ct = ct->cg.next) {
		j = ct->cic - bc->cic;
		if (j < m->msg.cqr.csi.len) {
			if (!(m->msg.cqr.csi.ptr[j] & 0x0c)) {
				switch (m->msg.cqr.csi.ptr[j] & 0x03) {
				case 0x00:	/* transient */
					break;
				case 0x01:	/* spare */
				case 0x02:	/* spare */
					ct_set(ct, CCTF_LOC_S_BLOCKED);
					ct_clr(ct, CCTF_LOC_QUERY_PENDING);
					break;
				case 0x03:	/* unequipped */
					ct_set(ct, CCTF_LOC_S_BLOCKED);
					ct_clr(ct, CCTF_LOC_QUERY_PENDING);
					/* 
					   notify maintenance */
					break;
				}
			} else {
				switch (m->msg.cqr.csi.ptr[j] & 0x03) {
				case 0x00:	/* maintenance blocked none */
					if (ct_tst(ct, CCTF_REM_M_BLOCKED))
						ct_clr(ct, CCTF_REM_M_BLOCKED);
					if (ct_tst(ct, CCTF_LOC_M_BLOCKED))
						if ((err = ct_block(q, ct)))
							return (err);
					break;
				case 0x01:	/* maintenance blocked local */
					if (ct_tst(ct, CCTF_REM_M_BLOCKED))
						ct_clr(ct, CCTF_REM_M_BLOCKED);
					if (!ct_tst(ct, CCTF_LOC_M_BLOCKED))
						if ((err = ct_unblock(q, ct)))
							return (err);
					break;
				case 0x02:	/* maintenance blocked remote */
					if (!ct_tst(ct, CCTF_REM_M_BLOCKED))
						ct_set(ct, CCTF_REM_M_BLOCKED);
					if (ct_tst(ct, CCTF_LOC_M_BLOCKED))
						if ((err = ct_block(q, ct)))
							return (err);
					break;
				case 0x03:	/* maintenance blocked both */
					if (!ct_tst(ct, CCTF_REM_M_BLOCKED))
						ct_set(ct, CCTF_REM_M_BLOCKED);
					if (!ct_tst(ct, CCTF_LOC_M_BLOCKED))
						if ((err = ct_unblock(q, ct)))
							return (err);
					break;
				}
				switch (m->msg.cqr.csi.ptr[j] & 0x0c) {
				case 0x00:
					swerr();
					break;
				case 0x04:	/* icc busy */
					switch (ct_get_c_state(ct)) {
					case CTS_OGC_WAIT_SAM:
					case CTS_OGC_WAIT_ACM:
					case CTS_OGC_WAIT_ANM:
					case CTS_OGC_ANSWERED:
					case CTS_OGC_SUSPENDED:
					case CTS_OGC_WAIT_RLC:
					case CTS_OGC_SEND_RLC:
						if ((err = ct_reset(q, ct)))
							return (err);
						break;
					case CTS_ICC_WAIT_SAM:
					case CTS_ICC_WAIT_ACM:
					case CTS_ICC_WAIT_ANM:
					case CTS_ICC_ANSWERED:
					case CTS_ICC_SUSPENDED:
					case CTS_ICC_WAIT_RLC:
					case CTS_ICC_SEND_RLC:
						break;
					case CTS_IDLE:
						if ((err = ct_reset(q, ct)))
							return (err);
						break;
					}
					break;
				case 0x08:	/* ogc busy */
					switch (ct_get_c_state(ct)) {
					case CTS_OGC_WAIT_SAM:
					case CTS_OGC_WAIT_ACM:
					case CTS_OGC_WAIT_ANM:
					case CTS_OGC_ANSWERED:
					case CTS_OGC_SUSPENDED:
					case CTS_OGC_WAIT_RLC:
					case CTS_OGC_SEND_RLC:
						break;
					case CTS_ICC_WAIT_SAM:
					case CTS_ICC_WAIT_ACM:
					case CTS_ICC_WAIT_ANM:
					case CTS_ICC_ANSWERED:
					case CTS_ICC_SUSPENDED:
					case CTS_ICC_WAIT_RLC:
					case CTS_ICC_SEND_RLC:
						if ((err = ct_reset(q, ct)))
							return (err);
						break;
					case CTS_IDLE:
						if ((err = ct_reset(q, ct)))
							return (err);
						break;
					}
					break;
				case 0x0c:	/* idle */
					switch (ct_get_c_state(ct)) {
					case CTS_IDLE:
						break;
					default:
						if ((err = ct_reset(q, ct)))
							return (err);
						break;
					}
					break;
				}
				switch (m->msg.cqr.csi.ptr[j] & 0x30) {
				case 0x00:	/* hardware blocked none */
					if (ct_tst(ct, CCTF_REM_H_BLOCKED))
						ct_clr(ct, CCTF_REM_H_BLOCKED);
					if (ct_tst(ct, CCTF_LOC_H_BLOCKED))
						if ((err = cg_h_block(q, ct->cg.cg)))
							return (err);
					break;
				case 0x10:	/* hardware blocked local */
					if (ct_tst(ct, CCTF_REM_H_BLOCKED))
						ct_clr(ct, CCTF_REM_H_BLOCKED);
					if (!ct_tst(ct, CCTF_LOC_H_BLOCKED))
						if ((err =
						     cg_unblock(q, ct->cg.cg,
								ISUP_HARDWARE_FAILURE_ORIENTED)))
							return (err);
					break;
				case 0x20:	/* hardware blocked remote */
					if (!ct_tst(ct, CCTF_REM_H_BLOCKED))
						ct_set(ct, CCTF_REM_H_BLOCKED);
					if (ct_tst(ct, CCTF_LOC_H_BLOCKED))
						if ((err = cg_h_block(q, ct->cg.cg)))
							return (err);
					break;
				case 0x30:	/* hardware blocked both */
					if (!ct_tst(ct, CCTF_REM_H_BLOCKED))
						ct_set(ct, CCTF_REM_H_BLOCKED);
					if (!ct_tst(ct, CCTF_LOC_H_BLOCKED))
						if ((err =
						     cg_unblock(q, ct->cg.cg,
								ISUP_HARDWARE_FAILURE_ORIENTED)))
							return (err);
					break;
				}
			}
		} else {
			if (j < m->msg.cqr.rs.ptr[0])
				ct_set(ct, CCTF_LOC_S_BLOCKED);
		}
	}
	cg->cic = cg->ct.list->cic;	/* adjust base cic */
	return (QR_DONE);
      unexpected:
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_CPG:        -  0x2c - Call progress
 *  -------------------------------------------------------------------------
 *  2.2.4 ... If call setup failes and an inband tone or announcement has to be returned to the calling party from
 *  an exchange or called party, the exchange or user concerned connects the inband tone or announcement to the
 *  transmission path.  If a time-out occurs at the exchange providing the inband tone or announcement, the
 *  exchange sends a release message to the preceding exchange with cause value #31 (normal unspecified).  
 *
 *  If an address complete message has been returned to the preceding exchange a call progress message indicating
 *  that inband tone information is available along with the cause parameter, is returned to the preceding
 *  exchange (see 2.1.5).  The cause value should reflect the reason of call failure in the same way as the inband
 *  tone or announcement to be applied.  
 *
 *  If an address complete message has not been returned to the preceding exchange already, an address complete
 *  message with the cause parameter and the "in-band information" indicator set in the optional backward call
 *  indicator, will be returned to the originating exchange.  The cause value should reflect the reason of call
 *  failure in the same way as the in-band tone or announcement to be applied.  
 *
 *  In case a special tone or announcement has to be applied due to an event only known by a certain exchange and
 *  not covered by a cause value, not cause parameter is included in either the address complete or call progress
 *  messages.  The answer message must not be sent in this case.  
 *
 *  For the preceding exchanges the inclusion of the cause parameter in the address complete or call progress
 *  message implies an unsuccessful call setup.  The cause parameter will not be included for unsuccessful call
 *  stups when interworking has occured and the in-band tone or announcement is returned from the exchange beyond
 *  the interworking point.
 */
STATIC int
isup_recv_cpg(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: CPG <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ACM:
		ct_timer_stop(ct, t7);
		switch (m->msg.cpg.evnt & ISUP_EVNT_MASK) {
		case ISUP_EVNT_ALERTING:
		case ISUP_EVNT_IBI:
			/* 
			   discard */
			return (QR_DONE);
		case ISUP_EVNT_PROGRESS:
		default:
			/* 
			   see Q.730 */
			break;
		}
		break;
	case CTS_OGC_WAIT_ANM:
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_MORE:
	case CCS_WREQ_INFO:
	case CCS_WCON_SREQ:
		if ((err = cc_setup_con(q, ct, m)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_PROCEED);
		/* 
		   fall through */
	case CCS_WIND_PROCEED:
		/* 
		   need ACM (not CPG) in these states */
		if ((m->msg.cpg.evnt & ISUP_EVNT_MASK) == ISUP_EVNT_PROGRESS) {
			if ((err = cc_proceeding_ind(q, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_ALERTING);
			break;
		}
		/* 
		   fall through */
	case CCS_WIND_ALERTING:
		if ((m->msg.cpg.evnt & ISUP_EVNT_MASK) == ISUP_EVNT_ALERTING) {
			if ((err = cc_alerting_ind(q, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_PROGRESS);
			break;
		}
		/* 
		   fall through */
	case CCS_WIND_PROGRESS:
		if ((m->msg.cpg.evnt & ISUP_EVNT_MASK) == ISUP_EVNT_PROGRESS) {
			if ((err = cc_progress_ind(q, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_PROGRESS);
			break;
		}
		/* 
		   fall through */
	case CCS_WIND_IBI:
		if ((m->msg.cpg.evnt & ISUP_EVNT_MASK) == ISUP_EVNT_IBI) {
			if ((err = cc_ibi_ind(q, ct, m)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_CONNECT);
			break;
		}
		/* 
		   fall through */
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ACM:
	case CTS_OGC_WAIT_ANM:
		break;
	default:
		swerr();
		return (-EFAULT);
	}
      unexpected:
	/* 
	   unexpected message */
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_USR:        -  0x2d - User-to-user information
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_usr(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: USR <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_UCIC:       -  0x2e - Unequipped circuit identification code
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_ucic(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: UCIC <-\n", DRV_NAME, ct));
	/* 
	   unexpected message */
	if ((err = cc_maint_ind(q, ct, ISUP_MAINT_UNEQUIPPED_CIC)))
		return (err);
	/* 
	   Custom 1s and delta */
	ct->sr.sr->stats.sr_unequipped_cic++;
	fixme(("Write this function\n"));
	swerr();
	return (-EFAULT);
}

/*
 *  ISUP_MT_CFN:        -  0x2f - Confusion
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cfn(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CFN <-\n", DRV_NAME, ct));
	/* 
	   discard */
	return (QR_DONE);
}

/*
 *  ISUP_MT_OLM:        -  0x30 - Overload
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_olm(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: OLM <-\n", DRV_NAME, ct));
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		if (ct_tst(ct, CCTF_COT_PENDING))
			ct_timer_stop(ct, t24);
		ct_timer_stop(ct, t7);
		break;
	default:
		goto unexpected;
	}
	switch (ct_get_t_state(ct)) {
	case CKS_WIND_CTEST:
		ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
		break;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_MORE:
	case CCS_WREQ_INFO:
	case CCS_WCON_SREQ:
	case CCS_WREQ_PROCEED:
		/* 
		   CC responsibility to release call */
		/* 
		   CC should not reattempt on this trunk group */
		if (ct->cpc.cc)
			if ((err =
			     cc_call_failure_ind(q, ct->cpc.cc, ct, ISUP_CALL_FAILURE_T3_TIMEOUT,
						 CC_CAUS_SWITCHING_EQUIP_CONGESTION)))
				return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		break;
	default:
		swerr();
		break;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		ct_timer_start(ct, t3);
		break;
	}
	return (QR_DONE);
      unexpected:
	return (-EOPNOTSUPP);
}

/*
 *  ISUP_MT_CRG:        -  0x31 - Charge information (Not ANSI)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_crg(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CRG <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_NRM:        -  0x32 - Network resource management
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_nrm(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: NRM <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_FAC:        -  0x33 - Facility
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_fac(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: FAC <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_UPT:        -  0x34 - User part test
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_upt(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	printd(("%s; %p: UPT <-\n", DRV_NAME, ct));
	if ((err = isup_send_upa(q, ct, NULL, 0)))
		return (err);
	return (QR_DONE);
}

/*
 *  ISUP_MT_UPA:        -  0x35 - User part available
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_upa(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	int err;
	struct sr *sr = ct->sr.sr;
	printd(("%s; %p: UPA <-\n", DRV_NAME, ct));
	if (!(sr->proto.popt & SS7_POPT_UPT))
		goto unexpected;
	if (sr->flags & CCTF_UPT_PENDING) {
		sr_timer_stop(sr, t4);
		if ((err = cc_sr_maint_ind(q, sr, ISUP_MAINT_USER_PART_AVAILABLE)))
			return (err);
		sr->flags &= ~CCTF_UPT_PENDING;
		/* 
		   Q.752 10.9 duration */
		sr->stats.sr_up_unavailable = jiffies - sr->stats.sr_up_unavailable;
	}
	return (QR_DONE);
      unexpected:
	/* 
	   unexpected */
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_IDR:        -  0x36 - Identification request
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_idr(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: IDR <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_IRS:        -  0x37 - Identification response
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_irs(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: IRS <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_SGM:        -  0x38 - Segmentation
 *  -------------------------------------------------------------------------
 *  Note: to handle simple segmentation, when the segmentation bit is set in optional forward call indicators or
 *  optional backwards call indicators we save the initial message.  When the segmenetation message is received,
 *  we process the messages concatentated, or when T34 expires, we process the initial message.
 */
STATIC int
isup_recv_sgm(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: SGM <-\n", DRV_NAME, ct));
	if (ct->timers.t34) {
		ct_timer_stop(ct, 34);
		if (ct->sgm) {
			fixme(("Handle segmentation\n"));
			/* 
			   Here we should just add the optional parameters in the SEG message to
			   the optional parameters in the stored message.  We could do this by
			   trimming the CIC, MT and optional parameters pointer and then
			   concatenating the parameters with the original message.  First the EOP
			   parameter must be truncated from the first message. */
			return (-EFAULT);
		} else {
			/* 
			   2.9.5.1 d) if a segmentation message is received and if the circuit is
			   seized by the call, in case the segmentation has not been announced in
			   the simple segmentation indicator, the segmenetation, message shall be
			   discarded; */
			int err;
			if ((err = cc_maint_ind(q, ct, ISUP_MAINT_SEGMENTATION_DISCARDED)))
				return (err);
			/* 
			   Custom 1st and delta */
			ct->stats.ct_segm_discarded++;
			ct->tg.tg->stats.ct_segm_discarded++;
			ct->sr.sr->stats.ct_segm_discarded++;
			ct->sp.sp->stats.ct_segm_discarded++;
			master.stats.ct_segm_discarded++;
			return (QR_DONE);
		}
	}
	/* 
	   unexpected */
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_CRA:        -  0xe9 - Circuit Reservation Acknowledgement (Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cra(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CRA <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_CRM:        -  0xea - Circuit Reservation (Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_crm(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CRM <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_CVR:        -  0xeb - Circuit Validation Response (Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cvr(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CVR <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_CVT:        -  0xec - Circuit Validation Test (Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cvt(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CVT <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_EXM:        -  0xed - Exit (Bellcore)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_exm(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: EXM <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_NON:        -  0xf8 - National Notification (Spain)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_non(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: NON <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_LLM:        -  0xfc - National Malicious Call (Spain)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_llm(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: LLM <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_CAK:        -  0xfd - Charge Acknowledgement (Singapore)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_cak(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: CAK <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_TCM:        -  0xfe - Tariff Charge (Singapore)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_tcm(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: TCM <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_MCP:        -  0xff - Malicious Call Print (Singapore)
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_recv_mcp(queue_t *q, struct ct *ct, isup_msg_t * m)
{
	printd(("%s; %p: MCP <-\n", DRV_NAME, ct));
	return (-ENOPROTOOPT);
}

/*
 *  =========================================================================
 *
 *  ISUP MESSAGE DECODING
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Decode message functions
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  ISUP_MT_???? - Unrecognized Message
 *  -------------------------------------------------------------------------
 *  ANSI: O(MCI PCI ...)
 *  ITUT: O(MCI PCI ...)
 *  SING: O(MCI PCI ...)
 *  SPAN: O(MCI PCI ...)
 */
STATIC INLINE int
isup_dec_unrecognized(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	/* 
	   unrecognized message */
	return (-ENOPROTOOPT);
}

/*
 *  ISUP_MT_IAM   0x01 - Initial address
 *  -------------------------------------------------------------------------
 *  ANSI: F(NCI FCI CPC) V(USI CDPN) O()
 *  ITUT: F(NCI FCI CPC TMR) V(CDPN) O()
 */
STATIC INLINE int
isup_dec_iam(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	if ((rtn = unpack_nci(pvar, p, e, &m->msg.iam.nci)) < 0)
		return (rtn);	/* NCI F */
	p += rtn;
	if ((rtn = unpack_fci(pvar, p, e, &m->msg.iam.fci)) < 0)
		return (rtn);	/* FCI F */
	p += rtn;
	if ((rtn = unpack_cpc(pvar, p, e, &m->msg.iam.cpc)) < 0)
		return (rtn);	/* CPC F */
	p += rtn;
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
			return (-EMSGSIZE);
		if ((rtn = unpack_usi(pvar, pp, ep, &m->msg.iam.usi)) < 0)
			return (rtn);	/* USI V */
		break;
	default:
		if ((rtn = unpack_tmr(pvar, p, e, &m->msg.iam.tmr)) < 0)
			return (rtn);
		p += rtn;
		break;
	}
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_cdpn(pvar, pp, ep, &m->msg.iam.cdpn)) < 0)
		return (rtn);	/* CDPN V */
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_SAM   0x02 - Subsequent address
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: V(SUBN) O()
 */
STATIC INLINE int
isup_dec_sam(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_subn(pvar, pp, ep, &m->msg.sam.subn)) < 0)
		return (rtn);	/* SUBN V */
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_INR   0x03 - Information request
 *  -------------------------------------------------------------------------
 *  ANSI: F(INRI) O()
 *  ITUT: F(INRI) O()
 */
STATIC INLINE int
isup_dec_inr(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((rtn = unpack_inri(pvar, p, e, &m->msg.inr.inri)) < 0)
		return (rtn);	/* INRI F */
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_INF   0x04 - Information
 *  -------------------------------------------------------------------------
 *  ANSI: F(INFI) O()
 *  ITUT: F(INFI) O()
 */
STATIC INLINE int
isup_dec_inf(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((rtn = unpack_infi(pvar, p, e, &m->msg.inf.infi)) < 0)
		return (rtn);	/* INFI F */
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_COT   0x05 - Continuity
 *  -------------------------------------------------------------------------
 *  ANSI: F(COTI)
 *  ITUT: F(COTI)
 */
STATIC INLINE int
isup_dec_cot(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((rtn = unpack_coti(pvar, p, e, &m->msg.cot.coti)) < 0)
		return (rtn);	/* COTI F */
	p += rtn;
	return (0);
}

/*
 *  ISUP_MT_ACM   0x06 - Address complete
 *  -------------------------------------------------------------------------
 *  ANSI: F(BCI) O()
 *  ITUT: F(BCI) O()
 */
STATIC INLINE int
isup_dec_acm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((rtn = unpack_bci(pvar, p, e, &m->msg.acm.bci)) < 0)
		return (rtn);	/* BCI F */
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_CON   0x07 - Connect (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: F(BCI) O()
 */
STATIC INLINE int
isup_dec_con(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_bci(pvar, p, e, &m->msg.con.bci)) < 0)
		return (rtn);	/* BCI F */
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_FOT   0x08 - Forward transfer
 *  -------------------------------------------------------------------------
 *  ANSI: O()
 *  ITUT: O()
 */
STATIC INLINE int
isup_dec_fot(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_ANM   0x09 - Answer
 *  -------------------------------------------------------------------------
 *  ANSI: O()
 *  ITUT: O()
 */
STATIC INLINE int
isup_dec_anm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_REL   0x0c - Release
 *  -------------------------------------------------------------------------
 *  ANSI: V(CAUS) O()
 *  ITUT: V(CAUS) O()
 */
STATIC INLINE int
isup_dec_rel(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_caus(pvar, pp, ep, &m->msg.rel.caus)) < 0)
		return (rtn);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_SUS   0x0d - Suspend
 *  -------------------------------------------------------------------------
 *  ANSI: F(SRIS) O()
 *  ITUT: F(SRIS) O()
 */
STATIC INLINE int
isup_dec_sus(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((rtn = unpack_sris(pvar, p, e, &m->msg.sus.sris)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_RES   0x0e - Resume
 *  -------------------------------------------------------------------------
 *  ANSI: F(SRIS) O()
 *  ITUT: F(SRIS) O()
 */
STATIC INLINE int
isup_dec_res(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((rtn = unpack_sris(pvar, p, e, &m->msg.res.sris)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_RLC   0x10 - Release complete
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: O()
 */
STATIC INLINE int
isup_dec_rlc(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_CCR   0x11 - Continuity check request
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_dec_ccr(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	return (0);
}

/*
 *  ISUP_MT_RSC   0x12 - Reset circuit
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_dec_rsc(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	return (0);
}

/*
 *  ISUP_MT_BLO   0x13 - Blocking
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_dec_blo(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	return (0);
}

/*
 *  ISUP_MT_UBL   0x14 - Unblcoking
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_dec_ubl(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	return (0);
}

/*
 *  ISUP_MT_BLA   0x15 - Blocking acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_dec_bla(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	return (0);
}

/*
 *  ISUP_MT_UBA   0x16 - Unblocking acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_dec_uba(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	return (0);
}

/*
 *  ISUP_MT_GRS   0x17 - Circuit group reset
 *  -------------------------------------------------------------------------
 *  ANSI: V(RS)
 *  ITUT: V(RS)
 */
STATIC INLINE int
isup_dec_grs(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	isup_var_t *v = &m->msg.grs.rs;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_rs(pvar, pp, ep, v)) < 0)
		return (rtn);
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		/* 
		   ANSI allows range value of zero for pre-arranged circuit groups */
		if (v->len == 1 && v->ptr[0] == 0)
			break;
		/* 
		   fall through */
	default:
	case SS7_PVAR_ITUT:
		if (v->len != 1 || v->ptr[0] < 1 || v->ptr[0] > 31)
			return (-EINVAL);
	}
	return (0);
}

/*
 *  ISUP_MT_CGB   0x18 - Circuit group blocking
 *  -------------------------------------------------------------------------
 *  ANSI: F(CGI) V(RS)
 *  ITUT: F(CGI) V(RS)
 */
STATIC INLINE int
isup_dec_cgb(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	isup_var_t *v = &m->msg.cgb.rs;
	if ((rtn = unpack_cgi(pvar, p, e, &m->msg.cgb.cgi)) < 0)
		return (rtn);
	p += rtn;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_rs(pvar, pp, ep, v)) < 0)
		return (rtn);
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		/* 
		   ANSI allows range value of zero for pre-arranged circuit groups */
		if (v->len == 1) {
			if (v->ptr[0] == 0)
				break;
			return (-EINVAL);
		}
		/* 
		   fall through */
	default:
	case SS7_PVAR_ITUT:
	{
		int i, j, set;
		if (v->len < 2 || v->len > 33 || v->ptr[0] < 1 || v->ptr[0] > 31)
			return (-EINVAL);
		for (set = 0, i = 1; i < v->len; i++)
			for (j = 0; j < 7; j++)
				if (v->ptr[i] & (0x1 << j))
					set++;
		if (set > 32)
			return (-EINVAL);
	}
	}
	return (0);
}

/*
 *  ISUP_MT_CGU   0x19 - Circuit group unblocking
 *  -------------------------------------------------------------------------
 *  ANSI: F(CGI) V(RS)
 *  ITUT: F(CGI) V(RS)
 */
STATIC INLINE int
isup_dec_cgu(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	isup_var_t *v = &m->msg.cgu.rs;
	if ((rtn = unpack_cgi(pvar, p, e, &m->msg.cgu.cgi)) < 0)
		return (rtn);
	p += rtn;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_rs(pvar, pp, ep, v)) < 0)
		return (rtn);
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		/* 
		   ANSI allows range value of zero for pre-arranged circuit groups */
		if (v->len == 1) {
			if (v->ptr[0] == 0)
				break;
			return (-EINVAL);
		}
		/* 
		   fall through */
	default:
	{
		int i, j, set;
		if (v->len < 2 || v->len > 33 || v->ptr[0] < 1 || v->ptr[0] > 31)
			return (-EINVAL);
		for (set = 0, i = 1; i < v->len; i++)
			for (j = 0; j < 7; j++)
				if (v->ptr[i] & (0x1 << j))
					set++;
		if (set > 32)
			return (-EINVAL);
	}
	}
	return (0);
}

/*
 *  ISUP_MT_CGBA   0x1a - Circuit group blocking acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: F(CGI) V(RS)
 *  ITUT: F(CGI) V(RS)
 */
STATIC INLINE int
isup_dec_cgba(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	isup_var_t *v = &m->msg.cgba.rs;
	if ((rtn = unpack_cgi(pvar, p, e, &m->msg.cgba.cgi)) < 0)
		return (rtn);
	p += rtn;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_rs(pvar, pp, ep, v)) < 0)
		return (rtn);
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		/* 
		   ANSI allows range value of zero for pre-arranged circuit groups */
		if (v->len == 1) {
			if (v->ptr[0] == 0)
				break;
			return (-EINVAL);
		}
		/* 
		   fall through */
	default:
	case SS7_PVAR_ITUT:
	{
		int i, j, set;
		if (v->len < 2 || v->len > 33 || v->ptr[0] < 1 || v->ptr[0] > 31)
			return (-EINVAL);
		for (set = 0, i = 1; i < v->len; i++)
			for (j = 0; j < 7; j++)
				if (v->ptr[i] & (0x1 << j))
					set++;
		if (set > 32)
			return (-EINVAL);
	}
	}
	return (0);
}

/*
 *  ISUP_MT_CGUA   0x1b - Circuit group unblocking acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: F(CGI) V(RS)
 *  ITUT: F(CGI) V(RS)
 */
STATIC INLINE int
isup_dec_cgua(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	isup_var_t *v = &m->msg.cgua.rs;
	if ((rtn = unpack_cgi(pvar, p, e, &m->msg.cgua.cgi)) < 0)
		return (rtn);
	p += rtn;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_rs(pvar, pp, ep, v)) < 0)
		return (rtn);
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		/* 
		   ANSI allows range value of zero for pre-arranged circuit groups */
		if (v->len == 1) {
			if (v->ptr[0] == 0)
				break;
			return (-EINVAL);
		}
		/* 
		   fall through */
	default:
	case SS7_PVAR_ITUT:
	{
		int i, j, set;
		if (v->len < 2 || v->len > 33 || v->ptr[0] < 1 || v->ptr[0] > 31)
			return (-EINVAL);
		for (set = 0, i = 1; i < v->len; i++)
			for (j = 0; j < 7; j++)
				if (v->ptr[i] & (0x1 << j))
					set++;
		if (set > 32)
			return (-EINVAL);
	}
	}
	return (0);
}

/*
 *  ISUP_MT_CMR   0x1c - Call Modification Request (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: CMI O()
 */
STATIC INLINE int
isup_dec_cmr(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_cmi(pvar, p, e, &m->msg.cmr.cmi)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_CMC   0x1d - Call Modification Completed (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: CMI O()
 */
STATIC INLINE int
isup_dec_cmc(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_cmi(pvar, p, e, &m->msg.cmr.cmi)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_CMRJ   0x1e - Call Modification Reject (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: CMI O()
 */
STATIC INLINE int
isup_dec_cmrj(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_cmi(pvar, p, e, &m->msg.cmr.cmi)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_FAR   0x1f - Facility request
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) O()
 *  ITUT: F(FACI) O()
 */
STATIC INLINE int
isup_dec_far(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((rtn = unpack_faci(pvar, p, e, &m->msg.far.faci)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_FAA   0x20 - Facility accepted
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) O()
 *  ITUT: F(FACI) O()
 */
STATIC INLINE int
isup_dec_faa(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((rtn = unpack_faci(pvar, p, e, &m->msg.faa.faci)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_FRJ   0x21 - Facility reject
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) V(CAUS) O()
 *  ITUT: F(FACI) V(CAUS) O()
 */
STATIC INLINE int
isup_dec_frj(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	if ((rtn = unpack_faci(pvar, p, e, &m->msg.frj.faci)) < 0)
		return (rtn);
	p += rtn;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_caus(pvar, pp, ep, &m->msg.frj.caus)) < 0)
		return (rtn);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_FAD   0x22 - Facility Deactivated (Bellcore only)
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) O()
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_dec_fad(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_faci(pvar, p, e, &m->msg.fad.faci)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_FAI   0x23 - Facility Information (Bellcore only)
 *  -------------------------------------------------------------------------
 *  ANSI: F(FACI) FAII O()
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_dec_fai(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_faci(pvar, p, e, &m->msg.fai.faci)) < 0)
		return (rtn);
	p += rtn;
	if ((rtn = unpack_faii(pvar, p, e, &m->msg.fai.faii)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_LPA   0x24 - Loop back acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_dec_lpa(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	return (0);
}

/*
 *  ISUP_MT_DRS   0x27 - Delayed release (not ANSI)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O()
 */
STATIC INLINE int
isup_dec_drs(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_PAM   0x28 - Pass along
 *  -------------------------------------------------------------------------
 *  ANSI: (encapsultated ISUP message)
 *  ITUT: (encapsultated ISUP message)
 */
STATIC INLINE int
isup_dec_pam(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	swerr();
	return (-EFAULT);
}

/*
 *  ISUP_MT_GRA   0x29 - Circuit group reset acknowledgement
 *  -------------------------------------------------------------------------
 *  ANSI: V(RS)
 *  ITUT: V(RS)
 */
STATIC INLINE int
isup_dec_gra(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_rs(pvar, pp, ep, &m->msg.gra.rs)) < 0)
		return (rtn);
	return (0);
}

/*
 *  ISUP_MT_CQM   0x2a - Circuit group query
 *  -------------------------------------------------------------------------
 *  ANSI: V(RS)
 *  ITUT: V(RS)
 */
STATIC INLINE int
isup_dec_cqm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	isup_var_t *v = &m->msg.cqm.rs;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_rs(pvar, pp, ep, v)) < 0)
		return (rtn);
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		/* 
		   ANSI allows range value of zero for pre-arranged circuit groups */
		if (v->len == 1 && v->ptr[0] == 0)
			break;
		/* 
		   fall through */
	default:
	case SS7_PVAR_ITUT:
		if (v->len != 1 || v->ptr[0] < 1 || v->ptr[0] > 31)
			return (-EINVAL);
	}
	return (0);
}

/*
 *  ISUP_MT_CQR   0x2b - Circuit group query response
 *  -------------------------------------------------------------------------
 *  ANSI: V(RS) V(CSI)
 *  ITUT: V(RS) V(CSI)
 */
STATIC INLINE int
isup_dec_cqr(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_rs(pvar, pp, ep, &m->msg.cqr.rs)) < 0)
		return (rtn);
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_csi(pvar, pp, ep, &m->msg.cqr.csi)) < 0)
		return (rtn);
	return (0);
}

/*
 *  ISUP_MT_CPG   0x2c - Call progress
 *  -------------------------------------------------------------------------
 *  ANSI: F(EVNT) O()
 *  ITUT: F(EVNT) O()
 */
STATIC INLINE int
isup_dec_cpg(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((rtn = unpack_evnt(pvar, p, e, &m->msg.cpg.evnt)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_USR   0x2d - User-to-user information
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: V(UUI) O()
 */
STATIC INLINE int
isup_dec_usr(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_uui(pvar, pp, ep, &m->msg.usr.uui)) < 0)
		return (rtn);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_UCIC   0x2e - Unequipped circuit identification code
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_dec_ucic(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	return (0);
}

/*
 *  ISUP_MT_CFN   0x2f - Confusion
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: V(CAUS) O()
 */
STATIC INLINE int
isup_dec_cfn(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	uchar *pp, *ep;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((pp = p + *p++) > e || (ep = pp + *pp++ + 1) > e)
		return (-EMSGSIZE);
	if ((rtn = unpack_caus(pvar, pp, ep, &m->msg.cfn.caus)) < 0)
		return (rtn);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_OLM   0x30 - Overload
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (none)
 */
STATIC INLINE int
isup_dec_olm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	return (0);
}

/*
 *  ISUP_MT_CRG   0x31 - Charge information
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (none)
 *  SING: ICCI O()
 *  SPAN: O()
 */
STATIC INLINE int
isup_dec_crg(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	switch (pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_ANSI:
		return isup_dec_unrecognized(pvar, p, e, m);
	default:
	case SS7_PVAR_ITUT:
		return (0);
	case SS7_PVAR_SPAN:
		break;
	case SS7_PVAR_SING:
		if ((rtn = unpack_icci(pvar, p, e, &m->msg.crg.icci)) < 0)
			return (rtn);
		p += rtn;
		break;
	}
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_NRM   0x32 - Network resource management
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(MCI PCI ECI TMU)
 */
STATIC INLINE int
isup_dec_nrm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_FAC   0x33 - Facility
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(MCI PCI ROPS SA)
 */
STATIC INLINE int
isup_dec_fac(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_UPT   0x34 - User part test
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(PCI)
 */
STATIC INLINE int
isup_dec_upt(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_UPA   0x35 - User part available
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(PCI)
 */
STATIC INLINE int
isup_dec_upa(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_IDR   0x36 - Identification request
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(MCIQ MCI PCI)
 */
STATIC INLINE int
isup_dec_idr(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_IRS   0x37 - Identification response
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(MCIR MCI PCI CGPN ATP GNUM*)
 */
STATIC INLINE int
isup_dec_irs(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_SGM   0x38 - Segmentation
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: O(ATP UUI MCI GDIG* GNOT* GNUM*)
 */
STATIC INLINE int
isup_dec_sgm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	if ((pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_CRA   0xe9 - Circuit Reservation Acknowledgement (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_dec_cra(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	(void) rtn;
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	return (0);
}

/*
 *  ISUP_MT_CRM   0xea - Circuit Reservation (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: NCI
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_dec_crm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_nci(pvar, p, e, &m->msg.crm.nci)) < 0)
		return (rtn);
	p += rtn;
	return (0);
}

/*
 *  ISUP_MT_CVR   0xeb - Circuit Validation Response (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: CVRI CGCI O()
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_dec_cvr(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_cvri(pvar, p, e, &m->msg.cvr.cvri)) < 0)
		return (rtn);
	p += rtn;
	if ((rtn = unpack_cgci(pvar, p, e, &m->msg.cvr.cgci)) < 0)
		return (rtn);
	p += rtn;
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_CVT   0xec - Circuit Validation Test (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: (none)
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_dec_cvt(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	return (0);
}

/*
 *  ISUP_MT_EXM   0xed - Exit (Bellcore)
 *  -------------------------------------------------------------------------
 *  ANSI: O()
 *  ITUT: (no such message)
 */
STATIC INLINE int
isup_dec_exm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		return isup_dec_unrecognized(pvar, p, e, m);
	if (*p)
		isup_dec_opt(pvar, p + *p, e, m);
	return (0);
}

/*
 *  ISUP_MT_NON   0xf8 - National Notification (Spain)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SPAN: TON
 */
STATIC INLINE int
isup_dec_non(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_SPAN)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_ton(pvar, p, e, &m->msg.non.ton)) < 0)
		return (rtn);
	p += rtn;
	return (0);
}

/*
 *  ISUP_MT_LLM   0xfc - National Malicious Call (Spain)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SPAN: (none)
 */
STATIC INLINE int
isup_dec_llm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_SPAN)
		return isup_dec_unrecognized(pvar, p, e, m);
	return (0);
}

/*
 *  ISUP_MT_CAK   0xfd - Charge Acknowledgement (Singapore)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SING: (none)
 */
STATIC INLINE int
isup_dec_cak(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_SING)
		return isup_dec_unrecognized(pvar, p, e, m);
	return (0);
}

/*
 *  ISUP_MT_TCM   0xfe - Tariff Charge (Singapore)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SING: CRI
 */
STATIC INLINE int
isup_dec_tcm(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	int rtn;
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_SING)
		return isup_dec_unrecognized(pvar, p, e, m);
	if ((rtn = unpack_cri(pvar, p, e, &m->msg.tcm.cri)) < 0)
		return (rtn);
	p += rtn;
	return (0);
}

/*
 *  ISUP_MT_MCP   0xff - Malicious Call Print (Singapore)
 *  -------------------------------------------------------------------------
 *  ANSI: (no such message)
 *  ITUT: (no such message)
 *  SING: (none)
 */
STATIC INLINE int
isup_dec_mcp(uint pvar, uchar *p, uchar *e, isup_msg_t * m)
{
	if ((pvar & SS7_PVAR_MASK) != SS7_PVAR_SING)
		return isup_dec_unrecognized(pvar, p, e, m);
	return (0);
}

/*
 *  =========================================================================
 *
 *  EVENTS
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Timer Events
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  T1 TIMEOUT
 *  -----------------------------------
 *  2.9.6  If a release complete message is not received in response to a release message before expiry of timer
 *  (T1), the exchange will retransmit the release message.
 *
 *  Start T1 whenever REL sent.  Stop T1 when RLC received.  When T1 expires, retransmit REL and restart T1.
 */
STATIC int
ct_t1_timeout(struct ct *ct)
{
	int err;
	mblk_t *mp;
#if 0
	if (ct_get_t_state(ct) == CKS_WCON_RELREQ) {
		if ((err = cc_release_con(NULL, ct->tst.cc, ct, NULL)))
			return (err);
		ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
	}
#endif
	/* 
	   release call control immediately and let maintenance take over circuit */
	if (ct_get_i_state(ct) == CCS_WCON_RELREQ) {
		if ((err = cc_release_con(NULL, ct->cpc.cc, ct, NULL)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
	{
		struct mtp *mtp;
		struct sr *sr = ct->sr.sr;
		ct_timer_start(ct, t1);
		/* 
		   resend contents of retrans buffer */
		if ((mtp = sr->mtp) || (mtp = sr->sp.sp->mtp))
			if ((mp = dupmsg(ct->rel)))
				ss7_oput(mtp->oq, mp);
	      done:
		return (QR_DONE);
	}
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T2 TIMEOUT
 *  -----------------------------------
 *  Start T2 when controlling exchange receives suspend (user) message.  Stop T2 ar the receipt of resume (user)
 *  message at controlling exchange.  When T2 expires, initiate release procedure.
 */
STATIC int
ct_t2_timeout(struct ct *ct)
{
	int err;
	switch (ct_get_i_state(ct)) {
	case CCS_SUSPENDED:
		/* 
		   CC responsibility to stop charging */
		if (ct->cpc.cc)
			if ((err =
			     cc_call_failure_ind(NULL, ct->cpc.cc, ct, ISUP_CALL_FAILURE_T2_TIMEOUT,
						 CC_CAUS_NORMAL_CALL_CLEARING)))
				return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_SUSPENDED:
		ct_timer_start(ct, t5);
		ct_timer_start(ct, t1);
		if ((err = isup_send_rel(NULL, ct, CC_CAUS_NORMAL_CALL_CLEARING, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_OGC_WAIT_RLC);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T3 TIMEOUT
 *  -----------------------------------
 *  Start T3 at the receipt of overload message.  Stop T3 on expiry.  When T3 expires, initiate release procedure.
 */
STATIC int
ct_t3_timeout(struct ct *ct)
{
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
		/* 
		   returned to idle pool after delay */
		ct_set_c_state(ct, CTS_IDLE);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T4 TIMEOUT
 *  -----------------------------------
 *  2.13.2 ... When an MTP-STATUS primitive with the cause "user part unavailability - inaccessible remote user"
 *  is received, the network management/overload control function should be informed.
 *
 *  The user part shall send a user part test message (using a circuit identification code that is equipped in
 *  both switches) to the remote user and starts timer T4 which supervises the receipt of an answer to the user
 *  part test message.
 *
 *  As a reaction to the user part test message, the remote ISDN User Part sends the user part available message
 *  if it is available again.
 *
 *  If the user part test message is not recognized by the rmoete user part, a confusion message with cause 97
 *  (message type non-existent or not implemented - discarded) is sent as a response.
 *
 *  All MTP-STATUS primitives with the cause "user part availability - inaccessible remote user" received while
 *  timer T4 is running are ignored.
 *
 *  On receipt of a user part available message or any other message timer T4 is stopped, the user part is marked
 *  available again and traffic is restarted.
 *
 *  On expiry of timer T4, the procedure is restarted.
 *
 *  When an MTP-STATUS primitive with the cause "user part unavailability - unequipped remote user" is received,
 *  the management system should be informed to restrict further traffic.  The ISDN User Part should not initiate
 *  the available test procedure for this case.
 *
 *  Start UPU detected; stop UPA/other recv; sent UPT/other
 *
 *  Start T4 at receipt of MTP_STATUS primitive with cause "remote user unavailable".  Stop T4 on expiry, or at
 *  receipt of UPA message (or any other).  On expiry of T4, send UPT message and start T4.
 *
 *  This is a signalling relation timer because MTP-STATUS is only applicable to a signalling relation.
 */
STATIC int
sr_t4_timeout(struct sr *sr)
{
	int err;
	if (sr->flags & CCTF_UPT_PENDING) {
		if ((sr->proto.popt & SS7_POPT_UPT) && sr->ct.list) {
			if ((err = isup_send_upt(NULL, sr->ct.list, NULL, 0)))
				return (err);
			sr_timer_start(sr, t4);
		}
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T5 TIMEOUT
 *  -----------------------------------
 *  Start T5 when first REL message sent.  Stop T5 when RLC received.  When T5 expires, send RSC, alert
 *  maintenance, remote the circuit from service, start T17 and stop T1.
 */
STATIC int
ct_t5_timeout(struct ct *ct)
{
	int err;
	if (ct_get_t_state(ct) == CKS_WCON_RELREQ) {
		if ((err = cc_release_con(NULL, ct->tst.cc, ct, NULL)))
			return (err);
		ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
	}
	if (ct_get_i_state(ct) == CCS_WCON_RELREQ) {
		if ((err = cc_release_con(NULL, ct->cpc.cc, ct, NULL)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
		if (!ct_tst(ct, CCTF_LOC_RESET_PENDING)) {
			ct_timer_start(ct, t17);
			ct_timer_stop(ct, t1);
			if (ct->rel)
				freemsg(xchg(&ct->rel, NULL));
			/* 
			   Q.752 12.5 on occurence */
			if ((err = cc_maint_ind(NULL, ct, ISUP_MAINT_T5_TIMEOUT)))
				return (err);
			ct_set(ct, CCTF_LOC_RESET_PENDING);
		}
		if ((err = isup_send_rsc(NULL, ct)))
			return (err);
		ct_set_c_state(ct, CTS_IDLE);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T6 TIMEOUT
 *  -----------------------------------
 *  2.4.3 Expiration of timer (T6) or timer (T38).  If a request for reconnection or a resume (network) message is
 *  not received within timer (T6) or timer (T38), both covered in Recommnedation Q.118, then the exchange where
 *  the timer has been started will initiate the release procedure on both sides.  Cause value #16 (normal call
 *  clearing) is used in the release message on expiry of T6; cause value #102 (recovery on timer expiry) is used
 *  in the release message on expiry of T38.
 *
 *  Start T6 when controlling exhcnage receives SUS.  Stop when controlling exchange receives RES.  When T6
 *  expires, initiate release.
 */
STATIC int
ct_t6_timeout(struct ct *ct)
{
	int err;
	switch (ct_get_i_state(ct)) {
	case CCS_SUSPENDED:
		/* 
		   CC responsibility to stop charging */
		if (ct->cpc.cc)
			if ((err =
			     cc_call_failure_ind(NULL, ct->cpc.cc, ct, ISUP_CALL_FAILURE_T6_TIMEOUT,
						 CC_CAUS_NORMAL_CALL_CLEARING)))
				return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_SUSPENDED:
		ct_timer_start(ct, t5);
		ct_timer_start(ct, t1);
		if ((err = isup_send_rel(NULL, ct, CC_CAUS_NORMAL_CALL_CLEARING, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_OGC_WAIT_RLC);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T7 TIMEOUT
 *  -----------------------------------
 *  2.1.2.1 e) Network protection timer.   Each time when the originating exchange has sent an address message the
 *  awaiting address complete timer (T7) is started.  If timer (T7) expires, the connection is released and an
 *  indication is sent to the calling subscriber.
 *
 *  Start T7 when IAM sent.  Stop T7 when ACM/CON received.  When T7 expires, release equipment and sent REL.
 */
STATIC int
ct_t7_timeout(struct ct *ct)
{
	int err;
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_MORE:
	case CCS_WREQ_INFO:
	case CCS_WCON_SREQ:
	case CCS_WIND_PROCEED:
		if ((err =
		     cc_call_failure_ind(NULL, ct->cpc.cc, ct, ISUP_CALL_FAILURE_T7_TIMEOUT,
					 CC_CAUS_ADDRESS_INCOMPLETE)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ACM:
		ct_timer_start(ct, t5);
		ct_timer_start(ct, t1);
		if ((err = isup_send_rel(NULL, ct, CC_CAUS_ADDRESS_INCOMPLETE, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_OGC_WAIT_RLC);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T8 TIMEOUT
 *  -----------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: ... when the call is released
 *  earlier and no automatic repeat attempt is to be attempted.
 *
 *  2.1.8 ... When an initial address message is received with a request for continuity check (either on this
 *  circuit or on a previous circuit), timer t8 is started.  On receipt of a successful indication of continuity
 *  check in a continuity message, timer T8 is stopped.  However, if timer T8 expires, the connection is cleared
 *  (cause 41).
 *
 *  Start T8 when IAN received indicating succeeding or incoming COT.  Stop T8 when COT received.  When T8
 *  expires, release equipement and send REL.
 */
STATIC int
ct_t8_timeout(struct ct *ct)
{
	int err;
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
		ct_timer_stop(ct, t35);
	case CTS_ICC_WAIT_ACM:
		/* 
		   responsibility of CC to remove COT */
		if (ct_get_i_state(ct) != CCS_IDLE) {
			if ((err =
			     cc_call_failure_ind(NULL, ct->cpc.cc, ct, ISUP_CALL_FAILURE_T8_TIMEOUT,
						 CC_CAUS_TEMPORARY_FAILURE)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		}
		if (ct_get_t_state(ct) == CKS_WIND_CCREP) {
			if ((err = cc_cont_report_ind(NULL, ct->tst.cc, ct, NULL)))
				return (err);
			ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
		}
		/* 
		   release iam information */
		ct_timer_start(ct, t5);
		ct_timer_start(ct, t1);
		if ((err = isup_send_rel(NULL, ct, CC_CAUS_TEMPORARY_FAILURE, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_ICC_WAIT_RLC);	/* wait for RLC */
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T9 TIMEOUT
 *  -----------------------------------
 *  Upon receipt of an address complete message, the awaiting address complete timer (T7) is stopped and the
 *  awaiting answer timer (T9) is started.  If timer (T9) expires, the connection is released and an indication is
 *  sent to the calling subscriber.
 *
 *  Start T9 when Outgoing International receives an IAM.  Stop T9 when ANM/CON received.  When T9 expires,
 *  release requipment and sent release.
 */
STATIC int
ct_t9_timeout(struct ct *ct)
{
	int err;
	/* 
	   this should probably be only in the backward direction */
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_PROCEED:
	case CCS_WIND_ALERTING:
	case CCS_WIND_PROGRESS:
	case CCS_WIND_CONNECT:
		if ((err =
		     cc_call_failure_ind(NULL, ct->cpc.cc, ct, ISUP_CALL_FAILURE_T9_TIMEOUT,
					 CC_CAUS_NO_ANSWER)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_ANM:
		ct_timer_start(ct, t5);
		ct_timer_start(ct, t1);
		if ((err = isup_send_rel(NULL, ct, CC_CAUS_NO_ANSWER, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_OGC_WAIT_RLC);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T10 TIMEOUT
 *  -----------------------------------
 *  2.1.4.8  An address complete message will not be sent until the cross-office check is made, if applicable (see
 *  Recommendation Q.543 [12]).  If the succeeding network does not provide electrical
 *  called-party's-line-condition indications the last Signalling System No. 7 exchange shall originate and send
 *  an address complete message when the end of address signalling has been determined:
 *
 *  a)  by receipt of an end-of-pulsing (ST) signal; or
 *
 *  b)  by receipt of the maximum number of digits used in the national numbering plan; or
 *
 *  c)  by analysis of the national (significant) number to indicate that a sufficient number of digits has been
 *  received to route the call to the called party; or
 *
 *  d)  by receipt of an end-of-selection signal from the succeeding network (e.g, number received signal in
 *  Signalling System No. 5); or
 *
 *  e)  exceptionally, if the succeeding newtork uses overlap signalling and number analysis is not possible, by
 *  observing that timer (T10) has elasped since the last digit was received, and that no fresh information has
 *  been received; in such circumstances, transmission to the national network of the last digit received must be
 *  prevented until the end of the waiting period which causes an address complete message to be sent backward.
 *  In this way, it is ensured that no national answer signal can arrive before an address complete message has
 *  been sent.
 *
 *  Start T10 when last digit is received in interworking situations.  Stop T10 at the receipt of fresh
 *  information.  When T10 expires, send address complete message.
 */
STATIC int
ct_t10_timeout(struct ct *ct)
{
	int err;
	ulong bci = 0;
	fixme(("Set bci value properly\n"));
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WRES_SIND:
	case CCS_WREQ_PROCEED:
		if ((err = cc_proceeding_ind(NULL, ct, NULL)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_WIND_ALERTING);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
		if ((err = isup_send_acm(NULL, ct, bci, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_ICC_WAIT_ANM);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T11 TIMEOUT
 *  -----------------------------------
 *  2.1.4.8 ... If in normal operation, a delay in the receipt of an address complete signal from the succeeding
 *  network is expected, the last common channel signalling exchange will originate and send an address complete
 *  message 15 to 20 seconds (timer T11) after receiving the latest address message.  The time-out condition is an
 *  upper limit consdiering the subclauses of 2.9.8.3 (20 to 30 seconds waiting for address complete message timer
 *  (T7) for outgoing international exchanges in abormal release conditions).
 *
 *  When the T11 expires, an ACM is sent to the preceding exchange.  Exchanges that run T11 do not run T7.  In
 *  addition, the wait for answer timer shall be started at the sending of ACM.
 *
 *  Start T11 when an IAM indicating interworking is received.  Stop T11 when an ACM has been sent.  When T11
 *  expires, send an ACM.
 */
STATIC int
ct_t11_timeout(struct ct *ct)
{
	int err;
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WRES_SIND:
		if ((err = cc_info_timeout_ind(NULL, ct, NULL)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_WRES_SIND);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
		if ((err = isup_send_acm(NULL, ct, ISUP_BCI_SUBSCRIBER_FREE, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_ICC_WAIT_ANM);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T12 TIMEOUT
 *  -----------------------------------
 *  Start T12 when BLO sent.  Stop T12 when BLA received.  When T12 expires, resend BLO and restart T12.
 */
STATIC int
ct_t12_timeout(struct ct *ct)
{
	int err;
	if (ct_tst(ct, CCTF_LOC_M_BLOCK_PENDING)) {
		if ((err = isup_send_blo(NULL, ct)))
			return (err);
		ct_timer_start(ct, t12);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T13 TIMEOUT
 *  -----------------------------------
 *  Start 1st BLO sent; stop BLA recv; send BLO alert maint start T13 stop T12
 */
STATIC int
ct_t13_timeout(struct ct *ct)
{
	int err;
	if (ct_tst(ct, CCTF_LOC_M_BLOCK_PENDING)) {
		ct_timer_stop(ct, t12);
		if ((err = cc_maint_ind(NULL, ct, ISUP_MAINT_T13_TIMEOUT)))
			return (err);
		/* 
		   Q.752 12.16 1st and delta */
		ct->stats.ct_t13_expiry++;
		ct->tg.tg->stats.ct_t13_expiry++;
		ct->sr.sr->stats.ct_t13_expiry++;
		ct->sp.sp->stats.ct_t13_expiry++;
		master.stats.ct_t13_expiry++;
		isup_send_blo(NULL, ct);
		ct_timer_start(ct, t13);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T14 TIMEOUT
 *  -----------------------------------
 *  Start UBL sent; stop UBA recv; send UBL and restart T14
 */
STATIC int
ct_t14_timeout(struct ct *ct)
{
	int err;
	if (ct_tst(ct, CCTF_LOC_M_UNBLOCK_PENDING)) {
		if ((err = isup_send_ubl(NULL, ct)))
			return (err);
		ct_timer_start(ct, t14);
	} else
		rare();		/* stagnant timer */
	return (QR_DONE);
}

/*
 *  T15 TIMEOUT
 *  -----------------------------------
 *  Start 1st UBL sent; stop UBA recv; send UBL alert maint start T15 stop T14
 */
STATIC int
ct_t15_timeout(struct ct *ct)
{
	int err;
	if (ct_tst(ct, CCTF_LOC_M_UNBLOCK_PENDING)) {
		ct_timer_stop(ct, t14);
		if ((err = cc_maint_ind(NULL, ct, ISUP_MAINT_T15_TIMEOUT)))
			return (err);
		/* 
		   Q.752 12.17 1st and delta */
		ct->stats.ct_t15_expiry++;
		ct->tg.tg->stats.ct_t15_expiry++;
		ct->sr.sr->stats.ct_t15_expiry++;
		ct->sp.sp->stats.ct_t15_expiry++;
		master.stats.ct_t15_expiry++;
		isup_send_ubl(NULL, ct);
		ct_timer_start(ct, t15);
	} else
		rare();		/* stagnant timer */
	return (QR_DONE);
}

/*
 *  T16 TIMEOUT
 *  -----------------------------------
 *  Start RSC sent; stop RLC recv; send RSC and restart T16
 */
STATIC int
ct_t16_timeout(struct ct *ct)
{
	int err;
	if (ct_tst(ct, CCTF_LOC_RESET_PENDING)) {
		if ((err = isup_send_rsc(NULL, ct)))
			return (err);
		ct_timer_start(ct, t16);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T17 TIMEOUT
 *  -----------------------------------
 *  Start 1st RSC sent; stop RLC recv; send RSC alert maint start T17 stop T16
 */
STATIC int
ct_t17_timeout(struct ct *ct)
{
	int err;
	if (ct_tst(ct, CCTF_LOC_RESET_PENDING)) {
		ct_timer_stop(ct, t16);
		if ((err = cc_maint_ind(NULL, ct, ISUP_MAINT_T17_TIMEOUT)))
			return (err);
		/* 
		   Q.752 12.1 1st and delat */
		ct->stats.ct_t17_expiry++;
		ct->tg.tg->stats.ct_t17_expiry++;
		ct->sr.sr->stats.ct_t17_expiry++;
		ct->sp.sp->stats.ct_t17_expiry++;
		master.stats.ct_t17_expiry++;
		isup_send_rsc(NULL, ct);
		ct_timer_start(ct, t17);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T18 TIMEOUT
 *  -----------------------------------
 *  Start CGB sent; stop CGBA recv; send CGB and restart T18
 */
STATIC int
cg_t18_timeout(struct cg *cg)
{
	int err;
	if (cg_tst(cg, CCTF_LOC_H_BLOCK_PENDING | CCTF_LOC_M_BLOCK_PENDING)) {
		ulong cgi = 0;
		if (cg_tst(cg, CCTF_LOC_M_BLOCK_PENDING))
			cgi = ISUP_MAINTENANCE_ORIENTED;
		if (cg_tst(cg, CCTF_LOC_H_BLOCK_PENDING))
			cgi = ISUP_HARDWARE_FAILURE_ORIENTED;
		if ((err = isup_send_cgb(NULL, cg->ct.list, cgi, cg->rs_ptr, cg->rs_len)))
			return (err);
		cg_timer_start(cg, t18);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T19 TIMEOUT
 *  -----------------------------------
 *  Start 1st CGB sent; stop CGBA recv; send CGB alert maint start T19 stop T18
 */
STATIC int
cg_t19_timeout(struct cg *cg)
{
	int err;
	ensure(cg, return (QR_DONE));
	if (cg_tst(cg, CCTF_LOC_H_BLOCK_PENDING | CCTF_LOC_M_BLOCK_PENDING)) {
		ulong cgi = 0;
		cg_timer_stop(cg, t18);
		if ((err = cc_cg_maint_ind(NULL, cg, cg->cic, ISUP_MAINT_T19_TIMEOUT)))
			return (err);
		/* 
		   Q.752 12.18 1st and delta */
		cg->stats.cg_t19_expiry++;
		ensure(cg->sr.sr, return (QR_DONE));
		cg->sr.sr->stats.cg_t19_expiry++;
		ensure(cg->sp.sp, return (QR_DONE));
		cg->sp.sp->stats.cg_t19_expiry++;
		master.stats.cg_t19_expiry++;
		if (cg_tst(cg, CCTF_LOC_M_BLOCK_PENDING))
			cgi = ISUP_MAINTENANCE_ORIENTED;
		if (cg_tst(cg, CCTF_LOC_H_BLOCK_PENDING))
			cgi = ISUP_HARDWARE_FAILURE_ORIENTED;
		isup_send_cgb(NULL, cg->ct.list, cgi, cg->rs_ptr, cg->rs_len);
		cg_timer_start(cg, t19);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T20 TIMEOUT
 *  -----------------------------------
 *  Start CGU sent; stop CGUA recv; send CGU and restart T20
 */
STATIC int
cg_t20_timeout(struct cg *cg)
{
	int err;
	if (cg_tst(cg, CCTF_LOC_H_UNBLOCK_PENDING | CCTF_LOC_M_UNBLOCK_PENDING)) {
		ulong cgi = 0;
		if (cg_tst(cg, CCTF_LOC_M_UNBLOCK_PENDING))
			cgi = ISUP_MAINTENANCE_ORIENTED;
		if (cg_tst(cg, CCTF_LOC_H_UNBLOCK_PENDING))
			cgi = ISUP_HARDWARE_FAILURE_ORIENTED;
		if ((err = isup_send_cgu(NULL, cg->ct.list, cgi, cg->rs_ptr, cg->rs_len)))
			return (err);
		cg_timer_start(cg, t20);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T21 TIMEOUT
 *  -----------------------------------
 *  Start 1st CGU sent; stop CGUA recv; send CGU alert maint start T21 stop T20
 */
STATIC int
cg_t21_timeout(struct cg *cg)
{
	int err;
	if (cg_tst(cg, CCTF_LOC_H_UNBLOCK_PENDING | CCTF_LOC_M_UNBLOCK_PENDING)) {
		ulong cgi = 0;
		cg_timer_stop(cg, t20);
		if ((err = cc_cg_maint_ind(NULL, cg, cg->cic, ISUP_MAINT_T21_TIMEOUT)))
			return (err);
		/* 
		   Q.752 12.19 1st and delta */
		cg->stats.cg_t21_expiry++;
		cg->sr.sr->stats.cg_t21_expiry++;
		cg->sp.sp->stats.cg_t21_expiry++;
		master.stats.cg_t21_expiry++;
		if (cg_tst(cg, CCTF_LOC_M_UNBLOCK_PENDING))
			cgi = ISUP_MAINTENANCE_ORIENTED;
		if (cg_tst(cg, CCTF_LOC_H_UNBLOCK_PENDING))
			cgi = ISUP_HARDWARE_FAILURE_ORIENTED;
		isup_send_cgu(NULL, cg->ct.list, cgi, cg->rs_ptr, cg->rs_len);
		cg_timer_start(cg, t21);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T22 TIMEOUT
 *  -----------------------------------
 *  Start GRS sent; stop GRA recv; send GRS and restart T22
 */
STATIC int
cg_t22_timeout(struct cg *cg)
{
	int err;
	if (cg_tst(cg, CCTF_LOC_RESET_PENDING)) {
		if ((err = isup_send_grs(NULL, cg->ct.list, cg->rs_ptr, cg->rs_len)))
			return (err);
		cg_timer_start(cg, t22);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T23 TIMEOUT
 *  -----------------------------------
 *  Start 1st GRS sent; stop GRA recv; send GRS alert maint start T23 stop T22
 */
STATIC int
cg_t23_timeout(struct cg *cg)
{
	int err;
	if (cg_tst(cg, CCTF_LOC_RESET_PENDING)) {
		cg_timer_stop(cg, t22);
		if ((err = cc_cg_maint_ind(NULL, cg, cg->cic, ISUP_MAINT_T23_TIMEOUT)))
			return (err);
		/* 
		   Q.752 1st and delta */
		cg->stats.cg_t23_expiry++;
		cg->sr.sr->stats.cg_t23_expiry++;
		cg->sp.sp->stats.cg_t23_expiry++;
		master.stats.cg_t23_expiry++;
		isup_send_grs(NULL, cg->ct.list, cg->rs_ptr, cg->rs_len);
		cg_timer_start(cg, t23);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T24 TIMEOUT
 *  -----------------------------------
 *  Start tone sent; stop recv tone; send COT failure start T25 make repeat attempt (from IAM) or start T24 (from
 *  CCR).
 */
STATIC int
ct_t24_timeout(struct ct *ct)
{
	int err;
	if (ct_tst(ct, CCTF_COT_PENDING)) {
		switch (ct_get_c_state(ct)) {
		case CTS_OGC_WAIT_SAM:
		case CTS_OGC_WAIT_ACM:
			if (ct_get_i_state(ct) != CCS_IDLE) {
				/* 
				   responsibility of CC to reattempt */
				if ((err =
				     cc_call_reattempt_ind(NULL, ct->cpc.cc, ct,
							   ISUP_REATTEMPT_T24_TIMEOUT)))
					return (err);
				ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			}
			if (ct_get_t_state(ct) == CKS_WREQ_CCREP) {
				/* 
				   responsibility of CM to remove COT */
				if ((err = cc_cont_report_ind(NULL, ct->tst.cc, ct, NULL)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_WIND_CTEST);
			}
			ct_timer_start(ct, t25);
			break;
		case CTS_OGC_WAIT_CCR:
			if (ct_get_t_state(ct) == CKS_WREQ_CCREP) {
				/* 
				   responsibility of CM to remove COT */
				if ((err = cc_cont_report_ind(NULL, ct->tst.cc, ct, NULL)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_WIND_CTEST);
			}
			ct_timer_start(ct, t26);
			break;
		}
		if ((err = isup_send_cot(NULL, ct, ISUP_COT_FAILURE)))
			return (err);
		ct_clr(ct, CCTM_CONT_CHECK);
		ct_set_c_state(ct, CTS_OGC_WAIT_CCR);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T25 TIMEOUT
 *  -----------------------------------
 *  Start T25 when initial continuity check failure is detected.  Stop T25 on expiry.  When T25 expires, send CCR
 *  message and repeat continuity check.
 */
STATIC int
ct_t25_timeout(struct ct *ct)
{
	int err;
	if (ct_get_c_state(ct) == CTS_OGC_WAIT_CCR) {
		if (!(ct->tg.tg->proto.popt & SS7_POPT_LPA)) {
			struct cc *ck;
			switch (ct_get_t_state(ct)) {
			case CKS_IDLE:
				/* 
				   find test */
				if (((ck = ct->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = ct->cg.cg->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = ct->tg.tg->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = ct->sr.sr->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = ct->sp.sp->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = master.bind.tst) && ck->setind < ck->maxind)
				    ) {
					ct_set_t_state(ct, ck, CKS_WIND_CONT);
					if ((err = cc_cont_test_ind(NULL, ck, ct)))
						return (err);
					ct_set_t_state(ct, ck, CKS_WREQ_CCREP);
				}
				break;
			case CKS_WIND_CONT:
			case CKS_WIND_CTEST:
				if ((err = cc_cont_test_ind(NULL, ct->tst.cc, ct)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_WREQ_CCREP);
			}
		}
		if (!ct_tst(ct, CCTF_COT_PENDING)) {
			/* 
			   repeat COT test */
			if ((err = isup_send_ccr(NULL, ct)))
				return (err);
			if (!(ct->tg.tg->proto.popt & SS7_POPT_LPA))
				ct_timer_start(ct, t24);
			ct_set(ct, CCTM_CONT_CHECK);
		}
		if ((err = cc_maint_ind(NULL, ct, ISUP_MAINT_T25_TIMEOUT)))
			return (err);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T26 TIMEOUT
 *  -----------------------------------
 *  Start T26 when second or subsequent continuity check failure is detected.  Stop T26 on expiry.  When T26
 *  expires, send CCR message and repeat continuity check.
 */
STATIC int
ct_t26_timeout(struct ct *ct)
{
	int err;
	if (ct_get_c_state(ct) == CTS_OGC_WAIT_CCR) {
		if (!(ct->tg.tg->proto.popt & SS7_POPT_LPA)) {
			struct cc *ck;
			switch (ct_get_t_state(ct)) {
			case CKS_IDLE:
				/* 
				   find test */
				if (((ck = ct->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = ct->cg.cg->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = ct->tg.tg->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = ct->sr.sr->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = ct->sp.sp->bind.tst) && ck->setind < ck->maxind)
				    || ((ck = master.bind.tst) && ck->setind < ck->maxind)
				    ) {
					ct_set_t_state(ct, ck, CKS_WIND_CONT);
					if ((err = cc_cont_test_ind(NULL, ck, ct)))
						return (err);
					ct_set_t_state(ct, ck, CKS_WREQ_CCREP);
				}
				break;
			case CKS_WIND_CONT:
			case CKS_WIND_CTEST:
				if ((err = cc_cont_test_ind(NULL, ct->tst.cc, ct)))
					return (err);
				ct_set_t_state(ct, ct->tst.cc, CKS_WREQ_CCREP);
			}
		}
		if (!ct_tst(ct, CCTF_COT_PENDING)) {
			/* 
			   repeat COT test */
			if ((err = isup_send_ccr(NULL, ct)))
				return (err);
			if (!(ct->tg.tg->proto.popt & SS7_POPT_LPA))
				ct_timer_start(ct, t24);
			ct_set(ct, CCTM_CONT_CHECK);
		}
		if ((err = cc_maint_ind(NULL, ct, ISUP_MAINT_T26_TIMEOUT)))
			return (err);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T27 TIMEOUT
 *  -----------------------------------
 *  2.1.8 ... If an indication of continuity check failure is received in a continuity message, timer T27 is
 *  started awaiting a continuity re-check request.  Also, the connection to the succeeding exchange is cleared.
 *  Timer T27 is stopped when the continuity-check request message is received and timer T36 is started awaiting a
 *  continuity or release message.
 *
 *  If either timer T27 or timer T36 expires, a reset circuit message is sent to the preceding exchange.  On
 *  receiption of the release complete message, the circuit is set to idle.
 *
 *  Start COT failure recv on repeat; stop CCR recv; send RSC start T16 and T17
 */
STATIC int
ct_t27_timeout(struct ct *ct)
{
	int err;
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_CCR:
		if (!ct_tst(ct, CCTF_LOC_RESET_PENDING)) {
			if ((err = isup_send_rsc(NULL, ct)))
				return (err);
			ct_timer_start(ct, t16);
			ct_timer_start(ct, t17);
			ct_set(ct, CCTF_LOC_RESET_PENDING);
			ct_set_c_state(ct, CTS_ICC_WAIT_RLC);
		}
		/* 
		   fall through */
	case CTS_ICC_WAIT_RLC:
		if ((err = cc_maint_ind(NULL, ct, ISUP_MAINT_T27_TIMEOUT)))
			return (err);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T28 TIMEOUT
 *  -----------------------------------
 *  Start CQM sent; stop CQR recv; alert maint
 */
STATIC int
cg_t28_timeout(struct cg *cg)
{
	int err;
	if (cg_tst(cg, CCTF_LOC_QUERY_PENDING)) {
		struct ct *ct;
		if ((err = cc_cg_maint_ind(NULL, cg, cg->cic, ISUP_MAINT_T28_TIMEOUT)))
			return (err);
		cg_clr(cg, CCTF_LOC_QUERY_PENDING);
		for (ct = cg->ct.list; ct; ct = ct->cg.next) {
			ct_clr(ct, CCTF_LOC_QUERY_PENDING);
		}
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T29 TIMEOUT
 *  -----------------------------------
 *  Start MTP-STATUS congestion received; stop on expiry; do nothing
 *
 *  2.10.2 ... During T29 all received congestion indications for the same destination point code are ignored in
 *  order not to reduce traffic too rapidly.
 *
 *  This is a signalling relation timer beceause MTP-STATUS is only applicable to a signalling relation.
 */
STATIC int
sr_t29_timeout(struct sr *sr)
{
	(void) sr;
	return (QR_DONE);
}

/*
 *  T30 TIMEOUT
 *  -----------------------------------
 *  Start MTP-STATUS congestion received; stop on expiry; decrement and restart
 *
 *  2.10.2 ... If T30 expires (i.e. no congestion indications have been receive during the T30 period) traffic
 *  will be increased by one step and T30 will be restarted unless full traffic load has been resumed.
 *
 *  This is a signalling relation timer because MTP-STATUS is only applicable to a signalling relation.
 */
STATIC int
sr_t30_timeout(struct sr *sr)
{
	int err;
	if (sr->cong_level) {
		if (sr->cong_level == 1) {
			/* 
			   congestion will cease with next operation */
			if ((err = cc_sr_maint_ind(NULL, sr, ISUP_MAINT_USER_PART_UNCONGESTED)))
				return (err);
			/* 
			   Q.752 10.13 duration */
			sr->stats.sr_up_congested = jiffies - sr->stats.sr_up_congested;
		} else
			sr_timer_start(sr, t30);
		sr->cong_level--;
	}
	return (QR_DONE);
}

/*
 *  T31 TIMEOUT
 *  -----------------------------------
 *  Start release of SCCP connection; stop on expiry; expires call reference reusable
 */
STATIC int
ct_t31_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  T32 TIMEOUT
 *  -----------------------------------
 *  Start request for SCCP connection; stop E2E msg recv; expires E2E msg allowed to be sent
 */
STATIC int
ct_t32_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  T33 TIMEOUT
 *  -----------------------------------
 *  Start INR sent; stop INF recv; expires release call alert maint
 */
STATIC int
ct_t33_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  T34 TIMEOUT
 *  -----------------------------------
 *  Note: to handle simple segmentation, when the segmentation bit is set in optional forward call indicators or
 *  optional backwards call indicators we save the initial message.  When the segmenetation message is received,
 *  we process the messages concatentated, or when T34 expires, we process the initial message.
 *  
 *  Start OBCI/OFCI SEG recv; stop SGM recv; proceed with call processing
 */
STATIC int mtp_read(queue_t *q, mblk_t *mp, struct sr *sr);
STATIC int
ct_t34_timeout(struct ct *ct)
{
	int err;
	if (ct->sgm) {
		fixme(("This will NOT work passing NULL here...\n"));
		switch ((err = mtp_read(NULL, ct->sgm, ct->sr.sr))) {
		case QR_DONE:
			freemsg(ct->sgm);
			/* 
			   fall through */
		case QR_ABSORBED:
			ct->sgm = NULL;
			return (QR_DONE);
		}
		return (err);
	}
	/* 
	   stagnant timer */
	rare();
	return (QR_DONE);
}

/*
 *  T35 TIMEOUT
 *  -----------------------------------
 *  The determination that the proper number of digits has not been received can be made at once if the end of
 *  pulsing (ST) signal is received.
 *
 *  When overlap working is used and the end of pulsing has not been received, the release message with cause 28
 *  (address incomplete) will be sent 15-20 seconds (T35) after receipt of the latest address message and before
 *  receipt of the minimum or fixed number of digits for forward routing of the call.  
 *
 *  2.3.4 ... The initial address message information can be released from memory: ... wehn the call is released
 *  earlier and no automatic repeat attempt is to be attempted.
 */
STATIC int
ct_t35_timeout(struct ct *ct)
{
	int err;
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WRES_SIND:
	case CCS_WREQ_PROCEED:
		if (ct->cpc.cc)
			if ((err =
			     cc_call_failure_ind(NULL, ct->cpc.cc, ct,
						 ISUP_CALL_FAILURE_T35_TIMEOUT,
						 CC_CAUS_ADDRESS_INCOMPLETE)))
				return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
		/* 
		   release iam information */
		ct_timer_start(ct, t5);
		ct_timer_start(ct, t1);
		if ((err = isup_send_rel(NULL, ct, CC_CAUS_ADDRESS_INCOMPLETE, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_ICC_WAIT_RLC);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T36 TIMEOUT
 *  -----------------------------------
 *  2.1.8 ... If an indication of continuity check failure is received in a continuity message, timer T27 is
 *  started awaiting a continuity re-check request.  Also, the connection to the succeeding exchange is cleared.
 *  Timer T27 is stopped when the continuity-check request message is received and timer T36 is started awaiting a
 *  continuity or release message.
 *
 *  If either timer T27 or timer T36 expires, a reset circuit message is sent to the preceding exchange.  On
 *  receiption of the release complete message, the circuit is set to idle.
 *
 *  Start LBA sent; stop COT or REL recv; release equip send RSC start T16, T17
 */
STATIC int
ct_t36_timeout(struct ct *ct)
{
	int err;
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_CCR:
		if (!ct_tst(ct, CCTF_LOC_RESET_PENDING)) {
			if ((err = isup_send_rsc(NULL, ct)))
				return (err);
			ct_timer_start(ct, t16);
			ct_timer_start(ct, t17);
			ct_set(ct, CCTF_LOC_RESET_PENDING);
		}
		ct_set_c_state(ct, CTS_ICC_WAIT_RLC);
		/* 
		   fall through */
	case CTS_ICC_WAIT_RLC:
		if ((err = cc_maint_ind(NULL, ct, ISUP_MAINT_T36_TIMEOUT)))
			return (err);
	      done:
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	goto done;
}

/*
 *  T37 TIMEOUT
 *  -----------------------------------
 *  2.7.3.1.1.1 ... If the originating exchange has incoming half echo control devices available, the following
 *  actions occur:
 *
 *  -- The exchange notes in memeory that there may be need to include/enable half echo control device in the
 *  circuit for the call and timer T37 is strated provided the exchange does not know that thesucceeding exchanges
 *  will include an incoming half echo control device.
 *
 *  -- The echo control information parameter in the network resource management message sent forward will
 *  indicate that an incoming half echo control device is included and that an incoming half echo control device
 *  is requested.
 *
 *  If the originating exchange receives from the succeeding network the echo control ifnormation parameter in a
 *  network resource management message indicating that an incoming half echo control device has been included,
 *  the actions at the originating exchange depend on if the exchange has a note in memory that there may be a
 *  need to include/enable an incoming half echo control device in the circuit for the call.
 *
 *  If the originating exchange has a note in memory that there may be a need to include/enable an incoming half
 *  echo control device in the circuit for the call, it will release the incoming half echo control device and
 *  stop timer T37.
 *
 *  If timer T37 expires, the incoming half echo control device is included.
 *
 *  If the echange receivs a backward network resource management message with the echo control information
 *  paraemter set to indicate that an incoming half echo control device is not included, the actions depend on
 *  whether the exchange has a not in memeory that there may be a need to include/enable an incoming half echo
 *  control device in the circuit for the call.
 *
 *  If the exchange has a note in memory that there may be a need to include/enable an incoming half echo control
 *  device in the circuit for the call, the device is included/enbled and timer T37 is stopped.
 *
 *  If the exchange has no note in memory that there may be a need to include/enable an incoming half echo control
 *  device in the circuit for the call, no special action is taken.
 *
 *  ...
 *
 *  c) If the intermediate exchange receives in the echo control information parameter in the network resource
 *  management message from the preceding exchange an indication that an incoming half echo control device is
 *  required, the following actions occur depdending on if incoming half echo control devices are available or
 *  not.
 *
 *  If the incoming half echo control devices are available, the following actions can occur:
 *
 *  -- The exchange notes in memory that there may be a need to include/enable an incoming half echo control
 *  device in the circuit for the call and timer T37 is started provided the exchange does not know that the
 *  succeeding exchanges will include an incoming half echo control device.
 *
 *  -- The echo control information parameter in the network management message sent forward will indicate that an
 *  incoming half echo control device is included and that an incoming half echo control device is requested.
 *
 *  If the echo control information parameter indicates that an incoming half echo control device has been
 *  included at a preceding exchange, a network resource management message is sent in the backward direction
 *  indicating in the echo control information parameter that an incoming half echo control device included.
 *
 *  If no incoming half echo control devices are available, the following action occurs:
 *
 *  -- The echo control information parameter in the network management message sent forward will be unchanged.
 *
 *  d)  If the intermediate exchange receives from the succeeding exchange the echo control information parameter
 *  in a network resource management message indicating that an incoming half echo control device has been
 *  included, the actions at the intermediate exchange depend on if the exchange has a note in memory that there
 *  may be a need to include/enable an incoming half echo control device in the circuit for the call or not.
 *
 *  If the exchange has a note in memory that there may be a need to include/enable an incoming half echo control
 *  device in the circuit for the call, it will release the incoming half echo control deivce and timer T37 is
 *  stopped.
 *
 *  If the exchange has no note in memory that there may be a need to include/enable *  an incoming half echo
 *  control device in the circuit for the call, the network management message will be sent backward to the
 *  preceding exchange.
 *
 *  e) If the exhcnage receives a backward network resource management message with the echo control information
 *  parameter set to indicate that an incoming half echo control deivce is not included, the actions depden on
 *  whether this exchange has a note in memory that there may be a need to include/enable an incoming half echo
 *  control device in the circuit for the call.
 *
 *  If the exchange has a note in memory that there may be a need to include/enable an incoming half echo control
 *  device in the circuit for the call, the device is included/enabled an timer T37 is stopped.  The network
 *  resource management message is not sent to the preceding exchange.
 *
 *  If the exchange has no note in memory that there may be a need to include/enable an incoming half echo control
 *  device in the circuit for the call, the network management message is passed to the preceding exchange.
 *
 *  f) If timer T37 expires, the incoming half echo control device is included/enabled.
 *
 *  ...
 *
 *  (More of the same for outgoing echo control device)
 *  
 *  ...
 *
 *  Start ECD request; ECD included; include/enable ECD
 */
STATIC int
ct_t37_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  T38 TIMEOUT
 *  -----------------------------------
 *  2.4.3 Expiration of timer (T6) or timer (T38).  If a request for reconnection or a resume (network) message is
 *  not received within timer (T6) or timer (T38), both covered in Recommnedation Q.118, then the exchange where
 *  the timer has been started will initiate the release procedure on both sides.  Cause value #16 (normal call
 *  clearing) is used in the release message on expiry of T6; cause value #102 (recovery on timer expiry) is used
 *  in the release message on expiry of T38.
 */
STATIC int
ct_t38_timeout(struct ct *ct)
{
	int err;
	switch (ct_get_i_state(ct)) {
	case CCS_SUSPENDED:
		if (ct->cpc.cc)
			if ((err =
			     cc_call_failure_ind(NULL, ct->cpc.cc, ct,
						 ISUP_CALL_FAILURE_T38_TIMEOUT,
						 CC_CAUS_RECOVERY_ON_TIMER_EXPIRY)))
				return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_WRES_RELIND);
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_SUSPENDED:
		ct_timer_start(ct, t5);
		ct_timer_start(ct, t1);
		if ((err = isup_send_rel(NULL, ct, CC_CAUS_RECOVERY_ON_TIMER_EXPIRY, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_ICC_WAIT_RLC);
		return (QR_DONE);
	case CTS_OGC_SUSPENDED:
		ct_timer_start(ct, t5);
		ct_timer_start(ct, t1);
		if ((err = isup_send_rel(NULL, ct, CC_CAUS_RECOVERY_ON_TIMER_EXPIRY, NULL, 0)))
			return (err);
		ct_set_c_state(ct, CTS_OGC_WAIT_RLC);
		return (QR_DONE);
	}
	/* 
	   stagnant timer */
	rare();
	return (QR_DONE);
}

/*
 *  Tacc-r TIMEOUT
 *  -----------------------------------
 */
STATIC int
ct_tacc_r_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Tccr TIMEOUT
 *  -----------------------------------
 */
STATIC int
ct_tccr_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Tccr-r TIMEOUT
 *  -----------------------------------
 */
STATIC int
ct_tccr_r_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Tcgb TIMEOUT
 *  -----------------------------------
 */
STATIC int
cg_tcgb_timeout(struct cg *cg)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Tcra TIMEOUT
 *  -----------------------------------
 */
STATIC int
ct_tcra_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Tcrm TIMEOUT
 *  -----------------------------------
 */
STATIC int
ct_tcrm_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Tcvt TIMEOUT
 *  -----------------------------------
 */
STATIC int
ct_tcvt_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Texm-d TIMEOUT
 *  -----------------------------------
 */
STATIC int
ct_texm_d_timeout(struct ct *ct)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Tgrs TIMEOUT
 *  -----------------------------------
 */
STATIC int
cg_tgrs_timeout(struct cg *cg)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Thga TIMEOUT
 *  -----------------------------------
 */
STATIC int
cg_thga_timeout(struct cg *cg)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Tscga TIMEOUT
 *  -----------------------------------
 */
STATIC int
cg_tscga_timeout(struct cg *cg)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  Tscga-d TIMEOUT
 *  -----------------------------------
 */
STATIC int
cg_tscga_d_timeout(struct cg *cg)
{
	swerr();
	fixme(("Unimplemented timer\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from below
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  M_ERROR, M_HANGUP:
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_hangup(struct mtp *mtp)
{
	if (mtp->sr) {
		struct sr *sr = mtp->sr;
		/* 
		   perform equivalent of MTP-PAUSE procedure for remote signalling point */
		if (!(sr->flags & CCTF_LOC_S_BLOCKED)) {
			struct ct *ct;
			for (ct = sr->ct.list; ct; ct = ct->sr.next)
				/* 
				   this will idle circuits from the idle list */
				ct_set(ct, CCTF_LOC_S_BLOCKED);
			sr->flags |= CCTF_LOC_S_BLOCKED;
		}
	} else if (mtp->sp) {
		struct sr *sr;
		struct sp *sp = mtp->sp;
		for (sr = sp->sr.list; sr; sr = sr->sp.next) {
			/* 
			   perform equivalent of MTP-PAUSE procedure for remote signalling point */
			if (!(sr->flags & CCTF_LOC_S_BLOCKED)) {
				struct ct *ct;
				for (ct = sr->ct.list; ct; ct = ct->sr.next)
					/* 
					   this will idle circuits from the idle list */
					ct_set(ct, CCTF_LOC_S_BLOCKED);
				sr->flags |= CCTF_LOC_S_BLOCKED;
			}
		}
	} else {
		/* 
		   mtp lower stream is not attached to a signalling relation */
		rare();
	}
	fixme(("Notify management of failed lower stream\n"));
	return (QR_DONE);
}

/*
 *  M_DATA:
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_read(queue_t *q, mblk_t *mp, struct sr *sr)
{
	struct ct *ct;
	struct cc *cc;
	uint pvar;
	int err, segmentation = 0;
	isup_msg_t msg = { NULL, };
	uchar *p, *e;
	ulong x, y;
	if (!mp)
		goto efault;
	if (mp->b_cont && !pullupmsg(mp, -1))
		goto enomem;
	p = mp->b_rptr;
	e = mp->b_wptr;
	if (p + 2 > e)
		goto emsgsize;
	x = *p++;
	y = *p++;
	msg.cic = (y << 8 | x) & 0x0fff;
#if 0
	{
		int i;
		printd(("%s: MSG:", DRV_NAME));
		for (i = 0; i < mp->b_wptr - mp->b_rptr; i++)
			printd((" %02X", (unsigned char) mp->b_rptr[i]));
		printd(("\n"));
	}
#endif
	if (p + 1 > e)
		goto emsgsize;
	msg.mt = *p++;
	/* 
	   Q.752 11.2 */
	sr->stats.msgs_recv++;
	sr->stats.msgs_recv_by_type[msg.mt]++;
	sr->sp.sp->stats.msgs_recv++;
	sr->sp.sp->stats.msgs_recv_by_type[msg.mt]++;
	master.stats.msgs_recv++;
	master.stats.msgs_recv_by_type[msg.mt]++;
	todo(("Change to hash lookup later (cic = %lu, mt = %lu)\n", msg.cic, msg.mt));
	for (ct = sr->ct.list; ct && ct->cic != msg.cic; ct = ct->sr.next) ;
	if (!ct)
		goto unequipped;
	if (!ct->tg.tg)
		goto unequipped;
	pvar = ct->tg.tg->proto.pvar;	/* use trunk group protocol variant */
	/* 
	   2.1.12 f) In case any other message except the ones listed below is received before the
	   segmentation message containing the second segment the exchange should react as if the
	   second segment is lost, i.e., the timer T34 is stopped and the call continues */
	if ((cc = ct->cpc.cc) && ct->sgm) {
		switch (msg.mt) {
		case ISUP_MT_COT:
		case ISUP_MT_BLO:
		case ISUP_MT_UBL:
		case ISUP_MT_BLA:
		case ISUP_MT_UBA:
		case ISUP_MT_CGB:
		case ISUP_MT_CGU:
		case ISUP_MT_CGBA:
		case ISUP_MT_CGUA:
		case ISUP_MT_CQM:
		case ISUP_MT_CQR:
			break;
		case ISUP_MT_SGM:
			break;
		default:
			segmentation = 1;
			ct_timer_stop(ct, t34);
			msg.mt = ct->sgm->b_rptr[2];
		}
	}
	switch (msg.mt) {
	case ISUP_MT_IAM:	/* 0x01 - Initial address */
		if (!(err = isup_dec_iam(pvar, p, e, &msg)))
			err = isup_recv_iam(q, ct, &msg);
		break;
	case ISUP_MT_SAM:	/* 0x02 - Subsequent address (not ANSI) */
		if (!(err = isup_dec_sam(pvar, p, e, &msg)))
			err = isup_recv_sam(q, ct, &msg);
		break;
	case ISUP_MT_INR:	/* 0x03 - Information request */
		if (!(err = isup_dec_inr(pvar, p, e, &msg)))
			err = isup_recv_inr(q, ct, &msg);
		break;
	case ISUP_MT_INF:	/* 0x04 - Information */
		if (!(err = isup_dec_inf(pvar, p, e, &msg)))
			err = isup_recv_inf(q, ct, &msg);
		break;
	case ISUP_MT_COT:	/* 0x05 - Continuity */
		if (!(err = isup_dec_cot(pvar, p, e, &msg)))
			err = isup_recv_cot(q, ct, &msg);
		break;
	case ISUP_MT_ACM:	/* 0x06 - Address complete */
		if (!(err = isup_dec_acm(pvar, p, e, &msg)))
			err = isup_recv_acm(q, ct, &msg);
		break;
	case ISUP_MT_CON:	/* 0x07 - Connect (not ANSI) */
		if (!(err = isup_dec_con(pvar, p, e, &msg)))
			err = isup_recv_con(q, ct, &msg);
		break;
	case ISUP_MT_FOT:	/* 0x08 - Forward transfer */
		if (!(err = isup_dec_fot(pvar, p, e, &msg)))
			err = isup_recv_fot(q, ct, &msg);
		break;
	case ISUP_MT_ANM:	/* 0x09 - Answer */
		if (!(err = isup_dec_anm(pvar, p, e, &msg)))
			err = isup_recv_anm(q, ct, &msg);
		break;
	case ISUP_MT_REL:	/* 0x0c - Release */
		if (!(err = isup_dec_rel(pvar, p, e, &msg)))
			err = isup_recv_rel(q, ct, &msg);
		break;
	case ISUP_MT_SUS:	/* 0x0d - Suspend */
		if (!(err = isup_dec_sus(pvar, p, e, &msg)))
			err = isup_recv_sus(q, ct, &msg);
		break;
	case ISUP_MT_RES:	/* 0x0e - Resume */
		if (!(err = isup_dec_res(pvar, p, e, &msg)))
			err = isup_recv_res(q, ct, &msg);
		break;
	case ISUP_MT_RLC:	/* 0x10 - Release complete */
		if (!(err = isup_dec_rlc(pvar, p, e, &msg)))
			err = isup_recv_rlc(q, ct, &msg);
		break;
	case ISUP_MT_CCR:	/* 0x11 - Continuity check request */
		if (!(err = isup_dec_ccr(pvar, p, e, &msg)))
			err = isup_recv_ccr(q, ct, &msg);
		break;
	case ISUP_MT_RSC:	/* 0x12 - Reset circuit */
		if (!(err = isup_dec_rsc(pvar, p, e, &msg)))
			err = isup_recv_rsc(q, ct, &msg);
		break;
	case ISUP_MT_BLO:	/* 0x13 - Blocking */
		if (!(err = isup_dec_blo(pvar, p, e, &msg)))
			err = isup_recv_blo(q, ct, &msg);
		break;
	case ISUP_MT_UBL:	/* 0x14 - Unblcoking */
		if (!(err = isup_dec_ubl(pvar, p, e, &msg)))
			err = isup_recv_ubl(q, ct, &msg);
		break;
	case ISUP_MT_BLA:	/* 0x15 - Blocking acknowledgement */
		if (!(err = isup_dec_bla(pvar, p, e, &msg)))
			err = isup_recv_bla(q, ct, &msg);
		break;
	case ISUP_MT_UBA:	/* 0x16 - Unblocking acknowledgement */
		if (!(err = isup_dec_uba(pvar, p, e, &msg)))
			err = isup_recv_uba(q, ct, &msg);
		break;
	case ISUP_MT_GRS:	/* 0x17 - 0b00010111 - Circuit group reset */
		if (!(err = isup_dec_grs(pvar, p, e, &msg)))
			err = isup_recv_grs(q, ct, &msg);
		break;
	case ISUP_MT_CGB:	/* 0x18 - 0b00011000 - Circuit group blocking */
		if (!(err = isup_dec_cgb(pvar, p, e, &msg)))
			err = isup_recv_cgb(q, ct, &msg);
		break;
	case ISUP_MT_CGU:	/* 0x19 - 0b00011001 - Circuit group unblocking */
		if (!(err = isup_dec_cgu(pvar, p, e, &msg)))
			err = isup_recv_cgu(q, ct, &msg);
		break;
	case ISUP_MT_CGBA:	/* 0x1a - 0b00011010 - Circuit group blocking acknowledgement */
		if (!(err = isup_dec_cgba(pvar, p, e, &msg)))
			err = isup_recv_cgba(q, ct, &msg);
		break;
	case ISUP_MT_CGUA:	/* 0x1b - 0b00011011 - Circuit group unblocking acknowledgement */
		if (!(err = isup_dec_cgua(pvar, p, e, &msg)))
			err = isup_recv_cgua(q, ct, &msg);
		break;
	case ISUP_MT_CMR:	/* 0x1c - Call Modification Request (not ANSI) */
		if (!(err = isup_dec_cmr(pvar, p, e, &msg)))
			err = isup_recv_cmr(q, ct, &msg);
		break;
	case ISUP_MT_CMC:	/* 0x1d - Call Modification Completed (not ANSI) */
		if (!(err = isup_dec_cmc(pvar, p, e, &msg)))
			err = isup_recv_cmc(q, ct, &msg);
		break;
	case ISUP_MT_CMRJ:	/* 0x1e - Call Modification Reject (not ANSI) */
		if (!(err = isup_dec_cmrj(pvar, p, e, &msg)))
			err = isup_recv_cmrj(q, ct, &msg);
		break;
	case ISUP_MT_FAR:	/* 0x1f - Facility request (not ANSI) */
		if (!(err = isup_dec_far(pvar, p, e, &msg)))
			err = isup_recv_far(q, ct, &msg);
		break;
	case ISUP_MT_FAA:	/* 0x20 - Facility accepted (not ANSI) */
		if (!(err = isup_dec_faa(pvar, p, e, &msg)))
			err = isup_recv_faa(q, ct, &msg);
		break;
	case ISUP_MT_FRJ:	/* 0x21 - Facility reject (not ANSI) */
		if (!(err = isup_dec_frj(pvar, p, e, &msg)))
			err = isup_recv_frj(q, ct, &msg);
		break;
	case ISUP_MT_FAD:	/* 0x22 - Facility Deactivated (Bellcore only, not ANSI) */
		if (!(err = isup_dec_fad(pvar, p, e, &msg)))
			err = isup_recv_fad(q, ct, &msg);
		break;
	case ISUP_MT_FAI:	/* 0x23 - Facility Information (Bellcore only) */
		if (!(err = isup_dec_fai(pvar, p, e, &msg)))
			err = isup_recv_fai(q, ct, &msg);
		break;
	case ISUP_MT_LPA:	/* 0x24 - Loop back acknowledgement */
		if (!(err = isup_dec_lpa(pvar, p, e, &msg)))
			err = isup_recv_lpa(q, ct, &msg);
		break;
	case ISUP_MT_DRS:	/* 0x27 - Delayed release (not ANSI) */
		if (!(err = isup_dec_drs(pvar, p, e, &msg)))
			err = isup_recv_drs(q, ct, &msg);
		break;
	case ISUP_MT_PAM:	/* 0x28 - Pass along */
		if (!(err = isup_dec_pam(pvar, p, e, &msg)))
			err = isup_recv_pam(q, ct, &msg);
		break;
	case ISUP_MT_GRA:	/* 0x29 - 0b00101001 - Circuit group reset acknowledgement */
		if (!(err = isup_dec_gra(pvar, p, e, &msg)))
			err = isup_recv_gra(q, ct, &msg);
		break;
	case ISUP_MT_CQM:	/* 0x2a - 0b00101010 - Circuit group query */
		if (!(err = isup_dec_cqm(pvar, p, e, &msg)))
			err = isup_recv_cqm(q, ct, &msg);
		break;
	case ISUP_MT_CQR:	/* 0x2b - 0b00101011 - Circuit group query response */
		if (!(err = isup_dec_cqr(pvar, p, e, &msg)))
			err = isup_recv_cqr(q, ct, &msg);
		break;
	case ISUP_MT_CPG:	/* 0x2c - Call progress */
		if (!(err = isup_dec_cpg(pvar, p, e, &msg)))
			err = isup_recv_cpg(q, ct, &msg);
		break;
	case ISUP_MT_USR:	/* 0x2d - User-to-user information */
		if (!(err = isup_dec_usr(pvar, p, e, &msg)))
			err = isup_recv_usr(q, ct, &msg);
		break;
	case ISUP_MT_UCIC:	/* 0x2e - Unequipped circuit identification code */
		if (!(err = isup_dec_ucic(pvar, p, e, &msg)))
			err = isup_recv_ucic(q, ct, &msg);
		break;
	case ISUP_MT_CFN:	/* 0x2f - Confusion */
		if (!(err = isup_dec_cfn(pvar, p, e, &msg)))
			err = isup_recv_cfn(q, ct, &msg);
		break;
	case ISUP_MT_OLM:	/* 0x30 - Overload */
		if (!(err = isup_dec_olm(pvar, p, e, &msg)))
			err = isup_recv_olm(q, ct, &msg);
		break;
	case ISUP_MT_CRG:	/* 0x31 - Charge information (Not ANSI) */
		if (!(err = isup_dec_crg(pvar, p, e, &msg)))
			err = isup_recv_crg(q, ct, &msg);
		break;
	case ISUP_MT_NRM:	/* 0x32 - Network resource management */
		if (!(err = isup_dec_nrm(pvar, p, e, &msg)))
			err = isup_recv_nrm(q, ct, &msg);
		break;
	case ISUP_MT_FAC:	/* 0x33 - Facility */
		if (!(err = isup_dec_fac(pvar, p, e, &msg)))
			err = isup_recv_fac(q, ct, &msg);
		break;
	case ISUP_MT_UPT:	/* 0x34 - 0b00110100 - User part test */
		if (!(err = isup_dec_upt(pvar, p, e, &msg)))
			err = isup_recv_upt(q, ct, &msg);
		break;
	case ISUP_MT_UPA:	/* 0x35 - 0b00110101 - User part available */
		if (!(err = isup_dec_upa(pvar, p, e, &msg)))
			err = isup_recv_upa(q, ct, &msg);
		break;
	case ISUP_MT_IDR:	/* 0x36 - Identification request */
		if (!(err = isup_dec_idr(pvar, p, e, &msg)))
			err = isup_recv_idr(q, ct, &msg);
		break;
	case ISUP_MT_IRS:	/* 0x37 - Identification response */
		if (!(err = isup_dec_irs(pvar, p, e, &msg)))
			err = isup_recv_irs(q, ct, &msg);
		break;
	case ISUP_MT_SGM:	/* 0x38 - Segmentation */
		if (!(err = isup_dec_sgm(pvar, p, e, &msg)))
			err = isup_recv_sgm(q, ct, &msg);
		break;
	case ISUP_MT_CRA:	/* 0xe9 - Circuit Reservation Acknowledgement (Bellcore) */
		if (!(err = isup_dec_cra(pvar, p, e, &msg)))
			err = isup_recv_cra(q, ct, &msg);
		break;
	case ISUP_MT_CRM:	/* 0xea - Circuit Reservation (Bellcore) */
		if (!(err = isup_dec_crm(pvar, p, e, &msg)))
			err = isup_recv_crm(q, ct, &msg);
		break;
	case ISUP_MT_CVR:	/* 0xeb - Circuit Validation Response (Bellcore) */
		if (!(err = isup_dec_cvr(pvar, p, e, &msg)))
			err = isup_recv_cvr(q, ct, &msg);
		break;
	case ISUP_MT_CVT:	/* 0xec - Circuit Validation Test (Bellcore) */
		if (!(err = isup_dec_cvt(pvar, p, e, &msg)))
			err = isup_recv_cvt(q, ct, &msg);
		break;
	case ISUP_MT_EXM:	/* 0xed - Exit (Bellcore) */
		if (!(err = isup_dec_exm(pvar, p, e, &msg)))
			err = isup_recv_exm(q, ct, &msg);
		break;
	case ISUP_MT_CAK:	/* 0xfd - Charge Acknowledgement (Singapore) */
		if (!(err = isup_dec_cak(pvar, p, e, &msg)))
			err = isup_recv_cak(q, ct, &msg);
		break;
	case ISUP_MT_TCM:	/* 0xfe - Tariff Charge (Singapore) */
		if (!(err = isup_dec_tcm(pvar, p, e, &msg)))
			err = isup_recv_tcm(q, ct, &msg);
		break;
	case ISUP_MT_MCP:	/* 0xff - Malicious Call Print (Singapore) */
		if (!(err = isup_dec_mcp(pvar, p, e, &msg)))
			err = isup_recv_mcp(q, ct, &msg);
		break;
	case ISUP_MT_NON:	/* 0xf8 - National Notification (Spain) */
		if (!(err = isup_dec_non(pvar, p, e, &msg)))
			err = isup_recv_non(q, ct, &msg);
		break;
	case ISUP_MT_LLM:	/* 0xfc - National Malicious Call (Spain) */
		if (!(err = isup_dec_llm(pvar, p, e, &msg)))
			err = isup_recv_llm(q, ct, &msg);
		break;
	default:
		if (!(err = isup_dec_unrecognized(pvar, p, e, &msg)))
			err = -ENOPROTOOPT;
		break;
	}
	if (ct->sgm) {
		switch (err) {
		case QR_DONE:
			freemsg(ct->sgm);
			/* 
			   fall through */
		case QR_ABSORBED:
			ct->sgm = NULL;
			return (QR_RETRY);
		}
	}
	switch (err) {
	case -EOPNOTSUPP:
		/* 
		   2.5.9.1 f) if other unexpected signalling messages are received, the following
		   actions will be undertaken: */
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
		case CTS_ICC_WAIT_ACM:
			/* 
			   responsibility of CC to remove loopback */
			if (ct_tst(ct, CCTF_COR_PENDING))
				ct_timer_stop(ct, t8);
			break;
		case CTS_OGC_WAIT_SAM:
		case CTS_OGC_WAIT_ACM:
			/* 
			   responsibility of CC to disconnect tone/detector */
			if (ct_tst(ct, CCTF_COT_PENDING))
				ct_timer_stop(ct, t24);
			ct_timer_stop(ct, t7);
			break;
		}
		switch (ct_get_t_state(ct)) {
		case CKS_WIND_CTEST:
		case CKS_WREQ_CTEST:
		case CKS_WREQ_CCREP:
			ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
			break;
		}
		switch (ct_get_i_state(ct)) {
		case CCS_WREQ_MORE:
		case CCS_WIND_INFO:
		case CCS_WRES_SIND:
		case CCS_WREQ_PROCEED:
			/* 
			   If the circuit is seized by an incoming call, any interconnected
			   circuits will be released. */
			if ((err =
			     cc_call_failure_ind(q, ct->cpc.cc, ct, ISUP_CALL_FAILURE_RESET,
						 CC_CAUS_INVALID_MESSAGE)))
				return (err);
			ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			break;
		case CCS_WIND_MORE:
		case CCS_WREQ_INFO:
		case CCS_WCON_SREQ:
		case CCS_WIND_PROCEED:
			/* 
			   If the circuit is seized by an outgoing call, an automatic repeat
			   attempt is provided on another circuit. */
			if ((err =
			     cc_call_reattempt_ind(q, ct->cpc.cc, ct, ISUP_REATTEMPT_UNEXPECTED)))
				return (err);
			/* 
			   responsibility of CC to reattempt */
			ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
			break;
		}
		switch (ct_get_c_state(ct)) {
			/* 
			   -- if the circuit is seized by a call, before receipt of a backward
			   message required for the call setup, the Reset Circuit Message is sent
			   (or, in the case of a multirate connection type or N x 64 kbit/s
			   connection type call, a circuit group reset message or multiple reset
			   circuit messages are sent). */
		case CTS_ICC_WAIT_SAM:
		case CTS_ICC_WAIT_ACM:
			/* 
			   If the circuit is seized by an incoming call, any interconnected
			   circuits will be released. */
		case CTS_OGC_WAIT_SAM:
		case CTS_OGC_WAIT_ACM:
			/* 
			   If the circuit is seized by an outgoing call, an automatic repeat
			   attempt is provided on another circuit. */
		case CTS_IDLE:
			/* 
			   -- if the circuit is idle, the reset circuit message is sent; */
			if (!ct_tst(ct, CCTF_LOC_RESET_PENDING)) {
				if ((err = cc_maint_ind(q, ct, ISUP_MAINT_UNEXPECTED_MESSAGE)))
					return (err);
				/* 
				   Q.752 12.21 1st and delta */
				ct->stats.ct_unexpected_msg++;
				ct->tg.tg->stats.ct_unexpected_msg++;
				ct->sr.sr->stats.ct_unexpected_msg++;
				ct->sp.sp->stats.ct_unexpected_msg++;
				master.stats.ct_unexpected_msg++;
				if ((err = isup_send_rsc(q, ct)))
					return (err);
				ct_timer_start(ct, t16);
				ct_timer_start(ct, t17);
				ct_set(ct, CCTF_LOC_RESET_PENDING);
			}
			ct_set_c_state(ct, CTS_IDLE);
			break;
		case CTS_ICC_WAIT_ANM:
		case CTS_ICC_ANSWERED:
		case CTS_ICC_SUSPENDED:
		case CTS_OGC_WAIT_ANM:
		case CTS_OGC_ANSWERED:
		case CTS_OGC_SUSPENDED:
		case CTS_ICC_WAIT_RLC:
		case CTS_OGC_WAIT_RLC:
		case CTS_ICC_SEND_RLC:
		case CTS_OGC_SEND_RLC:
			/* 
			   -- if the circuit is seized by a call after receipt of a backward
			   message required for the call setup, the unexpected signalling message
			   is discarded ... */
			err = QR_DONE;
			break;
		}
		break;
	case -ENOPROTOOPT:	/* message type error */
		switch (ct->tg.tg->config.exchange_type) {
		default:
			if (cc->mci & ISUP_MCI_E_BIT) {
				/* 
				   discard */
				return (QR_DONE);
			}
			/* 
			   release call */
			return (QR_DONE);
		case ISUP_XCHG_TYPE_B:
			if (cc->mci & ISUP_MCI_B_BIT) {
				/* 
				   release call */
				return (QR_DONE);
			}
			if (cc->mci & ISUP_MCI_D_BIT) {
				/* 
				   discard message */
				if (cc->mci & ISUP_MCI_C_BIT) {
					/* 
					   send notification */
				}
				return (QR_DONE);
			}
			if (1 /* pass_along_possible */ ) {
				/* 
				   pass along */
				return (QR_ABSORBED);
			}
			if (cc->mci & ISUP_MCI_E_BIT) {
				/* 
				   discard message */
				if (cc->mci & ISUP_MCI_C_BIT) {
					/* 
					   send notification */
				}
				return (QR_DONE);
			}
			/* 
			   release call */
			if (cc->mci & ISUP_MCI_C_BIT) {
				/* 
				   send notification */
			}
			return (QR_DONE);
		}
		break;
	case -EPROTONOSUPPORT:	/* parameter type error */
		fixme(("Move this to optional parameter decoder\n"));
		switch (ct->tg.tg->config.exchange_type) {
		default:
			switch (cc->pci & (ISUP_PCI_G_BIT | ISUP_PCI_F_BIT)) {
			case 0:
			case ISUP_PCI_G_BIT | ISUP_PCI_F_BIT:
				/* 
				   release call */
				return (QR_DONE);
			case ISUP_PCI_G_BIT:
				/* 
				   discard parameter */
				return (QR_DONE);
			case ISUP_PCI_F_BIT:
				/* 
				   discard message */
				return (-EPROTO);
			}
			swerr();
			return (-EFAULT);
		case ISUP_XCHG_TYPE_B:
			switch (cc->
				pci & (ISUP_PCI_B_BIT | ISUP_PCI_C_BIT | ISUP_PCI_D_BIT |
				       ISUP_PCI_E_BIT)) {
			case 0:
			case ISUP_PCI_C_BIT:
				/* 
				   pass on parameter */
				return (QR_DONE);
			case ISUP_PCI_E_BIT:
				/* 
				   discard parameter */
				return (QR_DONE);
			case ISUP_PCI_D_BIT:
			case ISUP_PCI_D_BIT | ISUP_PCI_E_BIT:
				/* 
				   discard message */
				return (-EPROTO);
			case ISUP_PCI_C_BIT | ISUP_PCI_E_BIT:
			case ISUP_PCI_C_BIT | ISUP_PCI_D_BIT:
			case ISUP_PCI_C_BIT | ISUP_PCI_D_BIT | ISUP_PCI_E_BIT:
				/* 
				   discard message */
				/* 
				   send notification */
				return (-EPROTO);
			default:
				/* 
				   release call */
				return (-EPROTO);
			}
			swerr();
			return (-EFAULT);
		}
		break;
	}
	return (err);
      unequipped:
	/* 
	   the circuit does is not equipped, respond with unequipped cic */
	if ((err = cc_if_maint_ind(q, sr, msg.cic, ISUP_MAINT_UNEQUIPPED_CIC)))
		return (err);
	/* 
	   Custom 1st and delta */
	sr->sp.sp->stats.sr_unequipped_cic++;
	if (sr->proto.popt & SS7_POPT_UCIC)
		isup_send_ucic(q, sr, msg.cic);
	return (QR_DONE);
      enomem:
	swerr();
	return (-ENOMEM);
      efault:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EPROTO);
}

/*
 *  MTP_OK_ACK
 *  ------------------------------------
 *  We shouldn't get these.  All requests which required an MTP_OK_ACK must be performed before the stream is
 *  linked.
 */
STATIC int
mtp_ok_ack(queue_t *q, mblk_t *mp)
{
	struct MTP_ok_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto efault;
      efault:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MTP_ERROR_ACK
 *  ------------------------------------
 *  We shouldn't get these; however, we attempt a MTP_ADDR_REQ when we link to get information about the MTP we
 *  have linked.  This might be an error response to our request.
 */
STATIC int
mtp_error_ack(queue_t *q, mblk_t *mp)
{
	struct MTP_error_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto efault;
      efault:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MTP_BIND_ACK
 *  ------------------------------------
 *  We shouldn't get these.  MTP streams must be bound before they are linked under the ISUP multiplexing driver.
 */
STATIC int
mtp_bind_ack(queue_t *q, mblk_t *mp)
{
	struct MTP_bind_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto efault;
      efault:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MTP_ADDR_ACK
 *  ------------------------------------
 *  This is a response to our request for the addresses bound and connected to the MTP stream after linking.  What
 *  we want to do is find the associated Signalling Point structure or associated Signalling Relation structure
 *  for the remote signalling point.  MTP streams can be bound in two fashions: with locat signalling point code
 *  and wildcard remote signalling point, or with both local and remote signalling points specified.
 */
STATIC int
mtp_addr_ack(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct sp *sp;
	struct sr *sr = NULL;
	struct MTP_addr_ack *p = (typeof(p)) mp->b_rptr;
	uchar *loc_ptr, *rem_ptr;
	size_t loc_len, rem_len;
	struct mtp_addr *loc, *rem;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	loc_ptr = mp->b_rptr + p->mtp_loc_offset;
	if ((loc_len = p->mtp_loc_length) < sizeof(*loc))
		goto emsgsize;
	if (mp->b_wptr < loc_ptr + loc_len)
		goto emsgsize;
	rem_ptr = mp->b_rptr + p->mtp_rem_offset;
	if ((rem_len = p->mtp_rem_length) < sizeof(*rem))
		goto emsgsize;
	if (mp->b_wptr < rem_ptr + rem_len)
		goto emsgsize;
	if (loc_len && loc_len < sizeof(*loc))
		goto efault;
	if (rem_len && rem_len < sizeof(*rem))
		goto efault;
	loc = (typeof(loc)) loc_ptr;
	rem = (typeof(rem)) loc_ptr;
	if (!loc_len)
		goto efault;
	for (sp = master.sp.list;
	     sp && (sp->add.ni != loc->ni || sp->add.pc != loc->pc || sp->add.si != loc->si);
	     sp = sp->next) ;
	if (!sp)
		goto notfound;
	if (!rem_len)
		goto noremote;
	for (sr = master.sr.list;
	     sr && (sr->add.ni != rem->ni || sr->add.pc != rem->pc || sr->add.si != rem->si);
	     sr = sr->next) ;
	if (!sr)
		goto notfound;
	unless(sr->mtp || sp->mtp, goto ebusy);
	sr->mtp = mtp_get(mtp);
	unless(mtp->sr, goto efault);
	mtp->sr = sr_get(sr);
	fixme(("Set proper state for SR and SP\n"));
	return (QR_DONE);
      noremote:
	unless(sp->mtp, goto ebusy);
	sp->mtp = mtp_get(mtp);
	unless(mtp->sp, goto efault);
	mtp->sp = sp_get(sp);
	fixme(("Set proper state for SR and SP\n"));
	return (QR_DONE);
      notfound:
	noenable(mtp->iq);
	noenable(mtp->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mtp));
	fixme(("Need to inform layer management to unlink stream\n"));
	return (-EINVAL);
#if defined(_SAFE)||defined(_DEBUG)
      ebusy:
	noenable(mtp->iq);
	noenable(mtp->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mtp));
	fixme(("Need to inform layer management to unlink stream\n"));
	return (-EBUSY);
#endif
      efault:
	noenable(mtp->iq);
	noenable(mtp->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mtp));
	fixme(("Need to inform layer management to unlink stream\n"));
	return (-EFAULT);
      emsgsize:
	noenable(mtp->iq);
	noenable(mtp->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: Bad message size\n", DRV_NAME, mtp));
	fixme(("Need to inform layer management to unlink stream\n"));
	return (-EMSGSIZE);
}

/*
 *  MTP_INFO_ACK
 *  ------------------------------------
 *  We can use this instead of MTP_ADDR_ACK which has less information in it.
 */
STATIC int
mtp_info_ack(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct sp *sp;
	struct sr *sr = NULL;
	struct MTP_info_ack *p = (typeof(p)) mp->b_rptr;
	uchar *add_ptr, *loc_ptr = NULL, *rem_ptr = NULL;
	size_t add_len, loc_len = 0, rem_len = 0;
	struct mtp_addr *loc, *rem;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->mtp_addr_offset;
	add_len = p->mtp_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	if (add_len) {
		if (add_len >= sizeof(*loc)) {
			loc_ptr = add_ptr;
			loc_len = sizeof(*loc);
			add_ptr += sizeof(*loc);
			add_len -= sizeof(*loc);
		} else if (add_len)
			goto efault;
	}
	if (add_len) {
		if (add_len >= sizeof(*rem)) {
			rem_ptr = add_ptr;
			rem_len = sizeof(*rem);
			add_ptr += sizeof(*rem);
			add_len -= sizeof(*rem);
		} else if (add_len)
			goto efault;
	}
	loc = (typeof(loc)) loc_ptr;
	rem = (typeof(rem)) rem_ptr;
	if (!loc)
		goto efault;
	for (sp = master.sp.list;
	     sp && (sp->add.ni != loc->ni || sp->add.pc != loc->pc || sp->add.si != loc->si);
	     sp = sp->next) ;
	if (!sp)
		goto notfound;
	if (!rem)
		goto noremote;
	for (sr = master.sr.list;
	     sr && (sr->add.ni != rem->ni || sr->add.pc != rem->pc || sr->add.si != rem->si);
	     sr = sr->next) ;
	if (!sr)
		goto notfound;
	unless(sr->mtp || sp->mtp, goto ebusy);
	unless(mtp->sr, goto efault);
	sr->mtp = mtp_get(mtp);
	mtp->sr = sr_get(sr);
	fixme(("Set proper state for SR and SP\n"));
	return (QR_DONE);
      noremote:
	unless(sp->mtp, goto ebusy);
	unless(mtp->sp, goto efault);
	sp->mtp = mtp_get(mtp);
	mtp->sp = sp_get(sp);
	fixme(("Set proper state for SR and SP\n"));
	return (QR_DONE);
      notfound:
	noenable(mtp->iq);
	noenable(mtp->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mtp));
	fixme(("Need to inform layer management to unlink stream\n"));
	return (-EINVAL);
#if defined(_SAFE)||defined(_DEBUG)
      ebusy:
	noenable(mtp->iq);
	noenable(mtp->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mtp));
	fixme(("Need to inform layer management to unlink stream\n"));
	return (-EBUSY);
#endif
      efault:
	noenable(mtp->iq);
	noenable(mtp->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: MTP provider stream already linked\n", DRV_NAME, mtp));
	fixme(("Need to inform layer management to unlink stream\n"));
	return (-EFAULT);
      emsgsize:
	noenable(mtp->iq);
	noenable(mtp->oq);
	swerr();
	__ptrace(("%s: %p: ERROR: Bad message size\n", DRV_NAME, mtp));
	fixme(("Need to inform layer management to unlink stream\n"));
	return (-EMSGSIZE);
}

/*
 *  MTP_OPTMGMT_ACK
 *  ------------------------------------
 *  We shouldn't get these.  MTP streams must have options set before they are linked under the ISUP multiplexing
 *  driver.
 */
STATIC int
mtp_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	struct MTP_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto efault;
      efault:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MTP_TRANSFER_IND
 *  ------------------------------------
 *  This is data.  It should normally come to us, however, as M_DATA blocks rather than MTP_TRANSFER_IND because
 *  ISUP is loosely connection oriented (local and remote point codes are fixed).
 */
STATIC int
mtp_transfer_ind(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	struct sr *sr;
	struct mtp_addr *a;
	struct MTP_transfer_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mtp_srce_offset + p->mtp_srce_length)
		goto emsgsize;
	if (!mp->b_cont)
		goto efault;
	a = (typeof(a)) (mp->b_rptr + p->mtp_srce_offset);
	if (!mtp->sp)
		sr = mtp->sr;
	else
		for (sr = mtp->sp->sr.list; sr && sr->id != a->pc; sr = sr->sp.next) ;
	if (!sr)
		goto eproto;
	if ((err = mtp_read(q, mp->b_cont, sr)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      efault:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      eproto:
	fixme(("Handle message from SP not known to ISUP\n"));
	return (-EPROTO);
}

/*
 *  MTP_PAUSE_IND
 *  ------------------------------------
 *  This is an indication that the remote switch is now inaccessible.
 *
 *  2.14 ... On the reception of an MTP pause primitive, the ISDN User Part takes the following action:
 *
 *  If the affected destination is not a destination (Signalling Point) known by the ISDN User Part (not connected
 *  by circuits to the exchange), not action takes place.
 *
 *  If the affected destination is a destination (Signalling Point) known by the ISDN User Part, all circuits are
 *  blocked for new calls.
 *
 *  Calls in progress need not be released even though signalling messages cannot be sent to the affected
 *  exchange.  (While it may not be technically necessary to release calls in progress, network providers may
 *  choose to release such calls, perhaps after some interval, if there is a concern about over charging due to
 *  the exchange's inability to completely clear the call when either the calling or called party disconnects.)
 */
STATIC int
mtp_pause_ind(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct sr *sr;
	struct mtp_addr *a;
	struct MTP_pause_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto emsgsize;
	a = (typeof(a)) (mp->b_rptr + p->mtp_addr_offset);
	if (!mtp->sp)
		sr = mtp->sr;
	else
		for (sr = mtp->sp->sr.list; sr && sr->add.pc != a->pc; sr = sr->sp.next) ;
	if (!sr)
		goto ignore;
	if (!(sr->flags & CCTF_LOC_S_BLOCKED)) {
		struct ct *ct;
		for (ct = sr->ct.list; ct; ct = ct->sr.next) {
			/* 
			   this will remove idle circuits from the idle list */
			ct_set(ct, CCTF_LOC_S_BLOCKED);
		}
		sr->flags |= CCTF_LOC_S_BLOCKED;
	}
      ignore:
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MTP_RESUME_IND
 *  ------------------------------------
 *  This is an indication that the remote switch is now accessible.
 *  
 *  2.14 ... On the receiption of a MTP resume primitive, the ISDN User Part takes the following action:
 *
 *  If the affected destination is not a destination (Signalling Point) known by the ISDUP User Part (not
 *  connected by circuits to the exchange), no action takes place.
 *
 *  If the affected destination (Signalling Point) is known by the ISDN User Part, the circuits in the idle state
 *  can be used for calls immediately.  Normal call release procedures that may have started during the period of
 *  signalling isolation continue and as such will ensure that affected circuits are returned to idle state.
 */
STATIC int
mtp_resume_ind(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct sr *sr;
	struct mtp_addr *a;
	struct MTP_resume_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto emsgsize;
	a = (typeof(a)) (mp->b_rptr + p->mtp_addr_offset);
	if (!mtp->sp)
		sr = mtp->sr;
	else
		for (sr = mtp->sp->sr.list; sr && sr->add.pc != a->pc; sr = sr->sp.next) ;
	if (!sr)
		goto ignore;	/* not for us */
	if (sr->flags & CCTF_LOC_S_BLOCKED) {
		struct ct *ct;
		for (ct = sr->ct.list; ct; ct = ct->sr.next) {
			/* 
			   this will add idle circuits to the idle list */
			ct_clr(ct, CCTF_LOC_S_BLOCKED);
		}
		sr->flags &= ~CCTF_LOC_S_BLOCKED;
	}
      ignore:
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MTP_STATUS_IND
 *  ------------------------------------
 *  This can be either an indication of MTP congestion, or an indication of User Part (remote ISUP) availability.
 *
 *  2.10.2 ... When the first congestion indication is received by the ISDN User Part, the traffic load into the
 *  affected destination point code is reduced by one step.  At the same time, two timers T29 and T30 are started.
 *  During T29 all received congestion indications for the same destination point code are ignored in order not to
 *  reduce traffic too rapidly.  Reception of a congestion indication after the expiry of T29, but still during
 *  T30, will decrease the traffic load by one more step and restart T29 and T30.  This step wise reduction of the
 *  ISDN User Part signalling traffic is continued util maximum reduction is obtained at the last step.  If T30
 *  expires (i.e.  no cnogestion indications have been received during the T30 period) traffic will be increased
 *  by one step and T30 will be restarted unless full traffic load has been resumed.
 *
 *  Timers T29 and T30 have the following values:
 *
 *      T29 = 300-600 ms;
 *      T30 = 5-10 s.
 *
 *  The number of steps of traffic reduction and type and/or amount of increase/decrease of traffic load at the
 *  various steps are considered to be an implementation matter.
 *
 *  2.13.2 ... When an MTP-STATUS primitive with the cause "user part unavailability - inaccessible remote user"
 *  is received, the network management/overload control function should be informed.
 *
 *  The user part shall send a user part test message (using a circuit identification code that is equipped in
 *  both switches) to the remote user and starts timer T4 which supervises the receipt of an answer to the user
 *  part test message.
 *
 *  As a reaction to the user part test message, the remote ISDN User Part sends the user part available message
 *  if it is available again.
 *
 *  If the user part test message is not recognized by the rmoete user part, a confusion message with cause 97
 *  (message type non-existent or not implemented - discarded) is sent as a response.
 *
 *  All MTP-STATUS primitives with the cause "user part availability - inaccessible remote user" received while
 *  timer T4 is running are ignored.
 *
 *  On receipt of a user part available message or any other message timer T4 is stopped, the user part is marked
 *  available again and traffic is restarted.
 *
 *  On expiry of timer T4, the procedure is restarted.
 *
 *  When an MTP-STATUS primitive with the cause "user part unavailability - unequipped remote user" is received,
 *  the management system should be informed to restrict further traffic.  The ISDN User Part should not initiate
 *  the available test procedure for this case.
 */
STATIC int
mtp_status_ind(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	struct sr *sr;
	struct mtp_addr *a;
	struct MTP_status_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto emsgsize;
	a = (typeof(a)) (mp->b_rptr + p->mtp_addr_offset);
	if (!mtp->sp)
		sr = mtp->sr;
	else
		for (sr = mtp->sp->sr.list; sr && sr->add.pc != a->pc; sr = sr->sp.next) ;
	if (!sr)
		goto ignore;	/* not for us */
	switch (p->mtp_type) {
	case MTP_STATUS_TYPE_CONG:
		if (sr->proto.popt & SS7_POPT_MPLEV) {
			int level;
			/* 
			   multiple congestion levels ala ANSI */
			switch (p->mtp_status) {
			case MTP_STATUS_CONGESTION_LEVEL0:
				level = 0;
				break;
			case MTP_STATUS_CONGESTION_LEVEL1:
				level = 1;
				break;
			case MTP_STATUS_CONGESTION_LEVEL2:
				level = 2;
				break;
			case MTP_STATUS_CONGESTION_LEVEL3:
				level = 3;
				break;
			default:
				goto efault;
			}
			if (sr->cong_level < level) {
				if (!sr->cong_level) {
					/* 
					   congestion begins */
					if ((err =
					     cc_sr_maint_ind(q, sr,
							     ISUP_MAINT_USER_PART_CONGESTED)))
						return (err);
					/* 
					   Q.752 10.11 duration */
					sr->stats.sr_up_congested =
					    jiffies - sr->stats.sr_up_congested;
				}
				sr->cong_level = level;
				sr_timer_start(sr, t30);
			}
		} else {
			/* 
			   single congestion level ala ITU */
			if (!sr->timers.t29) {
				if (!sr->cong_level) {
					/* 
					   congestion begins */
					if ((err =
					     cc_sr_maint_ind(q, sr,
							     ISUP_MAINT_USER_PART_CONGESTED)))
						return (err);
					/* 
					   Q.752 10.11 duration */
					sr->stats.sr_up_congested =
					    jiffies - sr->stats.sr_up_congested;
				}
				sr->cong_level++;
				sr_timer_start(sr, t29);
				sr_timer_start(sr, t30);
			}
		}
		return (QR_DONE);
	case MTP_STATUS_TYPE_UPU:
		switch (p->mtp_status) {
		case MTP_STATUS_UPU_UNEQUIPPED:
			/* 
			   inform management */
			if ((err = cc_sr_maint_ind(q, sr, ISUP_MAINT_USER_PART_UNEQUIPPED)))
				return (err);
			/* 
			   2.13.2 ... When an MTP-STATUS primitive with the cause "user part
			   unavailability - unequipped remote user" is received, the management
			   system should be informed to restrict further traffic. The ISDN User
			   Part should not initiate the available test procedure for this case. */
			return (QR_DONE);
		case MTP_STATUS_UPU_UNKNOWN:
		case MTP_STATUS_UPU_INACCESSIBLE:
			if ((err = cc_sr_maint_ind(q, sr, ISUP_MAINT_USER_PART_UNAVAILABLE)))
				return (err);
			/* 
			   Q.752 10.8 on occurence, 10.10 duration */
			sr->stats.sr_up_unavailable = jiffies - sr->stats.sr_up_unavailable;
			/* 
			   start UPU procedures */
			if (!(sr->flags & CCTF_UPT_PENDING)) {
				if ((sr->proto.popt & SS7_POPT_UPT) && sr->ct.list) {
					if ((err = isup_send_upt(q, sr->ct.list, NULL, 0)))
						return (err);
					sr_timer_start(sr, t4);
				} else {
					/* 
					   send some other message */
				}
				sr->flags |= CCTF_UPT_PENDING;
			}
			return (QR_DONE);
		default:
			goto efault;
		}
		break;
	default:
		goto efault;
	}
	goto efault;
      ignore:
	return (QR_DONE);
      efault:
	return (-EFAULT);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  MTP_RESTART_COMPLETE_IND
 *  ------------------------------------
 *  This is an indication that MTP has completed a restart.  It is of little use to us.  We will treat it the same
 *  as an MTP-RESUME.
 */
STATIC int
mtp_restart_complete_ind(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct sr *sr;
	struct MTP_restart_complete_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!mtp->sp && (!mtp->sr || !mtp->sr->sp.sp))
		goto efault;	/* shouldn't happen */
	for (sr = mtp->sp->sr.list; sr; sr = sr->sp.next) {
		if (sr->flags & CCTF_LOC_S_BLOCKED) {
			struct ct *ct;
			for (ct = sr->ct.list; ct; ct = ct->sr.next) {
				/* 
				   this will add idle circuits to the idle list */
				ct_clr(ct, CCTF_LOC_S_BLOCKED);
			}
			sr->flags &= ~CCTF_LOC_S_BLOCKED;
		}
	}
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  UNRECOGNIZED PRIMITIVE
 *  ------------------------------------
 */
STATIC int
mtp_unrecognized_prim(queue_t *q, mblk_t *mp)
{
	swerr();
	fixme(("Notify management of unrecognized primitive\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from above
 *
 *  -------------------------------------------------------------------------
 */

STATIC int
isup_check_addr(uchar *p, size_t l)
{
	struct isup_addr *a;
	if (l < sizeof(*a)) {
		trace();
		return (-EMSGSIZE);
	}
	a = (typeof(a)) p;
	switch (a->scope) {
	case ISUP_SCOPE_DF:
		if (a->id || a->cic) {
			trace();
			return (-EINVAL);
		}
		break;
	case ISUP_SCOPE_SP:
		if (!a->id || a->cic) {
			trace();
			return (-EINVAL);
		}
		break;
	case ISUP_SCOPE_SR:
	case ISUP_SCOPE_TG:
	case ISUP_SCOPE_CG:
	case ISUP_SCOPE_CT:
		if (!a->id && !a->cic) {
			trace();
			return (-EINVAL);
		}
		break;
	case ISUP_SCOPE_CIC:
		if (a->id || !a->cic) {
			trace();
			return (-EINVAL);
		}
		break;
	default:
		trace();
		return (-EINVAL);
	}
	return (0);
}

/*
 *  M_DATA:
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_write(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	int err;
	(void) cc;
	(void) err;
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  CC_INFO_REQ:
 *  ------------------------------------
 */
STATIC int
cc_info_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	int err;
	struct CC_info_req *p = (typeof(p)) mp->b_rptr;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cc->state == CCS_UNUSABLE)
		goto outstate;
	switch (cc->state) {
	default:
		swerr();
		return (-EFAULT);
	}
      emsgsize:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, -EMSGSIZE);
      outstate:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, CCOUTSTATE);
}

/*
 *  CC_OPTMGMT_REQ:
 *  ------------------------------------
 */
STATIC int
cc_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	int err;
	struct CC_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cc->state == CCS_UNBND || cc->state == CCS_UNUSABLE)
		goto outstate;
	switch (cc->state) {
	default:
		swerr();
		return (-EFAULT);
	}
      emsgsize:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, -EMSGSIZE);
      outstate:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, CCOUTSTATE);
}

/*
 *  CC_BIND_REQ:
 *  ------------------------------------
 */
STATIC int
cc_bind_req(queue_t *q, mblk_t *mp)
{
	struct cc **ccp, *cc = CC_PRIV(q);
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_bind_req *p = (typeof(p)) mp->b_rptr;
	struct isup_addr add = { ISUP_SCOPE_DF, 0 };
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
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
	case ISUP_SCOPE_DF:
	{
		struct df *df;
		df = &master;
		/* 
		   bind to default */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_TEST)
				ccp = &df->bind.tst;
			else if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &df->bind.mgm;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &df->bind.mnt;
			else
				ccp = &df->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &df->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (uchar *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.df = df;
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISUP_BIND_DF;
		break;
	}
	case ISUP_SCOPE_SP:
	{
		struct sp *sp;
		if (add.id) {
			for (sp = master.sp.list; sp && sp->id != add.id; sp = sp->next) ;
			if (!sp)
				goto badaddr;
		} else {
			if (!(sp = master.sp.list))
				goto noaddr;
			add.id = sp->id;
		}
		/* 
		   bind to local signalling point */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_TEST)
				ccp = &sp->bind.tst;
			else if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &sp->bind.mgm;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &sp->bind.mnt;
			else
				ccp = &sp->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &sp->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (uchar *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.sp = sp_get(sp);
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISUP_BIND_SP;
		break;
	}
	case ISUP_SCOPE_SR:
	{
		struct sr *sr;
		if (add.id) {
			for (sr = master.sr.list; sr && sr->id != add.id; sr = sr->next) ;
			if (!sr)
				goto badaddr;
		} else {
			if (!(sr = master.sr.list))
				goto noaddr;
			add.id = sr->id;
		}
		/* 
		   bind to remote signalling point */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_TEST)
				ccp = &sr->bind.tst;
			else if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &sr->bind.mgm;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &sr->bind.mnt;
			else
				ccp = &sr->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &sr->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (uchar *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.sr = sr_get(sr);
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISUP_BIND_SR;
		break;
	}
	case ISUP_SCOPE_TG:
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
		   bind to trunk group */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_TEST)
				ccp = &tg->bind.tst;
			else if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &tg->bind.mgm;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &tg->bind.mnt;
			else
				ccp = &tg->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &tg->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (uchar *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.tg = tg_get(tg);
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISUP_BIND_TG;
		break;
	}
	case ISUP_SCOPE_CG:
	{
		struct cg *cg;
		if (add.id) {
			for (cg = master.cg.list; cg && cg->id != add.id; cg = cg->next) ;
			if (!cg)
				goto badaddr;
		} else {
			if (!(cg = master.cg.list))
				goto noaddr;
			add.id = cg->id;
		}
		/* 
		   bind to circuit group */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_TEST)
				ccp = &cg->bind.tst;
			else if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &cg->bind.mgm;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &cg->bind.mnt;
			else
				ccp = &cg->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &cg->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (uchar *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.cg = cg_get(cg);
		cc->maxind = p->cc_setup_ind;
		cc->bind.type = ISUP_BIND_CG;
		break;
	}
	case ISUP_SCOPE_CT:
	{
		struct ct *ct;
		if (add.id) {
			for (ct = master.ct.list; ct && ct->id != add.id; ct = ct->next) ;
			if (!ct)
				goto badaddr;
		} else {
			if (!(ct = master.ct.list))
				goto noaddr;
			add.id = ct->id;
		}
		/* 
		   bind to circuit */
		if (p->cc_setup_ind) {
			if (p->cc_bind_flags & CC_TEST)
				ccp = &ct->bind.tst;
			else if (p->cc_bind_flags & CC_MANAGEMENT)
				ccp = &ct->bind.mgm;
			else if (p->cc_bind_flags & CC_MAINTENANCE)
				ccp = &ct->bind.mnt;
			else
				ccp = &ct->bind.icc;
			if (*ccp)
				goto addrbusy;
		} else
			ccp = &ct->bind.ogc;
		if ((err =
		     cc_bind_ack(q, cc, (uchar *) &add, sizeof(add), p->cc_setup_ind,
				 (ulong) (cc->oq))))
			return (err);
		cc->bind.u.ct = ct_get(ct);
		cc->maxind = p->cc_setup_ind;
		if (cc->maxind > 1)
			cc->maxind = 1;
		cc->bind.type = ISUP_BIND_CT;
		break;
	}
	default:
		goto badaddr;
	}
	if ((cc->bind.next = *ccp))
		cc->bind.next->bind.prev = &cc->bind.next;
	*ccp = cc_get(cc);
	cc->bind.prev = ccp;
	cs_set_state(cc, CCS_IDLE);
	return (QR_DONE);
      emsgsize:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, -EMSGSIZE);
      addrbusy:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, CCADDRBUSY);
      noaddr:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, CCNOADDR);
      badaddr:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, CCBADADDR);
      outstate:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, CCOUTSTATE);
}

/*
 *  CC_UNBIND_REQ:
 *  ------------------------------------
 */
STATIC int
cc_unbind_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	int err;
	struct CC_unbind_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cs_get_state(cc) != CCS_IDLE)
		goto outstate;
	switch (cc->bind.type) {
	case ISUP_BIND_CT:
	case ISUP_BIND_CG:
	case ISUP_BIND_TG:
	case ISUP_BIND_SR:
	case ISUP_BIND_SP:
	case ISUP_BIND_DF:
		if ((err = cc_ok_ack(q, cc, NULL, p->cc_primitive)))
			return (err);
		break;
	default:
		swerr();
		goto efault;
	}
	switch (cc->bind.type) {
	case ISUP_BIND_CT:
		ct_put(xchg(&cc->bind.u.ct, NULL));
		break;
	case ISUP_BIND_CG:
		cg_put(xchg(&cc->bind.u.cg, NULL));
		break;
	case ISUP_BIND_TG:
		tg_put(xchg(&cc->bind.u.tg, NULL));
		break;
	case ISUP_BIND_SR:
		sr_put(xchg(&cc->bind.u.sr, NULL));
		break;
	case ISUP_BIND_SP:
		sp_put(xchg(&cc->bind.u.sp, NULL));
		break;
	case ISUP_BIND_DF:
		xchg(&cc->bind.u.df, NULL);
		break;
	}
	cc->bind.type = ISUP_BIND_NONE;
	if ((*cc->bind.prev = cc->bind.next))
		cc->bind.next->bind.prev = cc->bind.prev;
	cc->bind.next = NULL;
	cc->bind.prev = &cc->bind.next;
	cc_put(cc);
	cs_set_state(cc, CCS_UNBND);
	return (QR_DONE);
      efault:
	swerr();
	return m_error(q, cc, EFAULT);
      outstate:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_ADDR_REQ:
 *  ------------------------------------
 */
STATIC int
cc_addr_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	int err;
	struct CC_addr_req *p = (typeof(p)) mp->b_rptr;
	struct isup_addr bind, conn;
	uchar *bind_ptr = (uchar *) &bind, *conn_ptr = (uchar *) &conn;
	size_t bind_len = sizeof(bind), conn_len = sizeof(conn);
	ulong cref;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cc->state == CCS_UNBND || cc->state == CCS_UNUSABLE)
		goto noaddr;
	switch (cc->bind.type) {
	case ISUP_BIND_CT:
		bind.scope = ISUP_SCOPE_CT;
		bind.id = cc->bind.u.ct->id;
		bind.cic = cc->bind.u.ct->cic;
		break;
	case ISUP_BIND_CG:
		bind.scope = ISUP_SCOPE_CG;
		bind.id = cc->bind.u.cg->id;
		bind.cic = cc->bind.u.cg->cic;
		break;
	case ISUP_BIND_TG:
		bind.scope = ISUP_SCOPE_TG;
		bind.id = cc->bind.u.tg->id;
		bind.cic = 0;
		break;
	case ISUP_BIND_SR:
		bind.scope = ISUP_SCOPE_SR;
		bind.id = cc->bind.u.sr->id;
		bind.cic = 0;
		break;
	case ISUP_BIND_SP:
		bind.scope = ISUP_SCOPE_SP;
		bind.id = cc->bind.u.sp->id;
		bind.cic = 0;
		break;
	case ISUP_BIND_DF:
		bind.scope = ISUP_SCOPE_DF;
		bind.id = 0;
		bind.cic = 0;
		break;
	default:
		goto noaddr;
	}
	if (cc->conn.cpc) {
		conn.scope = ISUP_SCOPE_CT;
		conn.id = cc->conn.cpc->id;
		conn.cic = cc->conn.cpc->cic;
		cref = cc->conn.cpc->cref;
	} else if (cc->conn.tst) {
		conn.scope = ISUP_SCOPE_CT;
		conn.id = cc->conn.tst->id;
		conn.cic = cc->conn.tst->cic;
		cref = cc->conn.cpc->cref;
	} else if (cc->conn.mgm) {
		conn.scope = ISUP_SCOPE_CT;
		conn.id = cc->conn.mgm->id;
		conn.cic = cc->conn.mgm->cic;
		cref = cc->conn.cpc->cref;
	} else if (cc->conn.gmg) {
		conn.scope = ISUP_SCOPE_CG;
		conn.id = cc->conn.gmg->id;
		conn.cic = cc->conn.gmg->cic;
		cref = cc->conn.cpc->cref;
	} else {
		conn_ptr = NULL;
		conn_len = 0;
		cref = 0;
	}
	return cc_addr_ack(q, cc, bind_ptr, bind_len, cref, conn_ptr, conn_len);
      noaddr:
	return cc_addr_ack(q, cc, NULL, 0, 0, NULL, 0);
      emsgsize:
	return cc_error_ack(q, cc, NULL, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_SETUP_REQ:
 *  ------------------------------------
 */
STATIC int
cc_setup_req(queue_t *q, mblk_t *mp)
{
	int err;
	struct cc *cc = CC_PRIV(q), *ck;
	struct ct *ct = NULL;
	uchar *cdpn_ptr, *opt_ptr, *add_ptr;
	size_t cdpn_len, opt_len, add_len;
	struct CC_setup_req *p = (typeof(p)) mp->b_rptr;
	struct isup_addr add = { ISUP_SCOPE_DF, 0, 0 };
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)) {
		ptrace(("ERROR: message too small for primitive\n"));
		goto emsgsize;
	}
	cdpn_ptr = mp->b_rptr + p->cc_cdpn_offset;
	cdpn_len = p->cc_cdpn_length;
	if (mp->b_wptr < cdpn_ptr + cdpn_len) {
		ptrace(("ERROR: cdpn too large\n"));
		goto emsgsize;
	}
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len) {
		ptrace(("ERROR: opt too large\n"));
		goto emsgsize;
	}
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len) {
		ptrace(("ERROR: addr too large\n"));
		goto emsgsize;
	}
	if (isup_check_cdpn(cdpn_ptr, cdpn_len) < 0)
		goto baddigs;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	if (isup_check_addr(add_ptr, add_len) < 0)
		goto badaddr;
	/* 
	   cannot trust alignment of address */
	bcopy(add_ptr, &add, add_len);
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	ensure(cc->bind.u.ct, goto null_bind);
	if (!add_len) {
		switch (cc->bind.type) {
		case ISUP_BIND_SR:
			/* 
			   select the first circuit */
			if (!(ct = cc->bind.u.sr->ct.list))
				goto failbusy;
			break;
		case ISUP_BIND_TG:
			/* 
			   select next idle circuit */
			if (!(ct = cc->bind.u.tg->idle))
				goto failbusy;
			break;
		case ISUP_BIND_CG:
			/* 
			   select the first circuit */
			if (!(ct = cc->bind.u.cg->ct.list))
				goto failbusy;
			break;
		case ISUP_BIND_CT:
			/* 
			   select bound circuit */
			ct = cc->bind.u.ct;
			break;
		case ISUP_BIND_DF:
		case ISUP_BIND_SP:
			goto noaddr;
		default:
			pswerr(("%s: %p: SWERR: invalid cc bind type %d in bound state\n",
				DRV_NAME, cc, cc->bind.type));
			goto efault;
		}
	} else {
		switch (add.scope) {
		case ISUP_SCOPE_SR:
			/* 
			   Note: if we are bound to a remote signalling point we can specify an
			   address with scope signalling relation and a cic within the bound
			   signalling relation to specify a circuit.  If the cic == 0, the first
			   circuit in the signalling relation will be selected.  cic == 0 is the
			   same as not specifying an address at all, so this option is useful only
			   for specifying a cic within a signalling relation. */
			if (cc->bind.type != ISUP_BIND_SR)
				goto noaddr;
			if (add.id && add.id != cc->bind.u.sr->id)
				goto badaddr;
			if (add.cic) {
				for (ct = cc->bind.u.sr->ct.list; ct && ct->cic != add.cic;
				     ct = ct->sr.next) ;
				if (!ct)
					goto badaddr;
			} else {
				goto noaddr;
			}
			break;
		case ISUP_SCOPE_TG:
			/* 
			   Note: if we are bound to a trunk group we can specify an address with
			   scope trunk group and a cic within the bound trunk group to specify a
			   circuit.  If the cic == 0, the next idle circuit in the bound trunk
			   group will be selected.  cic == 0 is the same as not specifying an
			   address at all, so this option is useful only for specifying a cic
			   within a bound trunk group. */
			if (cc->bind.type != ISUP_BIND_TG)
				goto badaddr;
			if (add.id && add.id != cc->bind.u.tg->id)
				goto badaddr;
			if (add.cic) {
				for (ct = cc->bind.u.tg->ct.list; ct && ct->cic != add.cic;
				     ct = ct->tg.next) ;
				if (!ct)
					goto badaddr;
			} else {
				if (!(ct = cc->bind.u.tg->idle))
					goto failbusy;
				break;
			}
			break;
		case ISUP_SCOPE_CG:
			/* 
			   Note: if we are bound to a circuit group we can specify an address with
			   scope circuit group and a cic within the bound circuit group to sepcify
			   a circuit. If the cic == 0, the first circuit in the bound circuit group 
			   will be selected.  cic == 0 is the same as not specifying an address at
			   all, so this option is useful only for specifying a cic within a bound
			   circuit group. */
			if (cc->bind.type != ISUP_BIND_CG)
				goto badaddr;
			if (add.id && add.id != cc->bind.u.cg->id)
				goto badaddr;
			if (add.cic) {
				for (ct = cc->bind.u.cg->ct.list; ct && ct->cic != add.cic;
				     ct = ct->cg.next) ;
				if (!ct)
					goto badaddr;
			} else {
				if (!(ct = cc->bind.u.cg->ct.list))
					goto noaddr;
				break;
			}
			break;
		case ISUP_SCOPE_CT:
			/* 
			   Note: we can always specify a circuit no matter how we are bound by
			   sepcifying an address with scope circuit and a circuit id and optional
			   cic. */
			if (!add.id)
				goto noaddr;
			switch (cc->bind.type) {
			case ISUP_BIND_CT:
				if (add.id != cc->bind.u.ct->id)
					goto badaddr;
				ct = cc->bind.u.ct;
				break;
			case ISUP_BIND_CG:
				for (ct = cc->bind.u.cg->ct.list; ct && ct->id != add.id;
				     ct = ct->cg.next) ;
				if (!ct)
					goto badaddr;
				break;
			case ISUP_BIND_TG:
				for (ct = cc->bind.u.tg->ct.list; ct && ct->id != add.id;
				     ct = ct->tg.next) ;
				if (!ct)
					goto badaddr;
				break;
			case ISUP_BIND_SR:
				for (ct = cc->bind.u.sr->ct.list; ct && ct->id != add.id;
				     ct = ct->sr.next) ;
				if (!ct)
					goto badaddr;
				break;
			case ISUP_BIND_SP:
				for (ct = cc->bind.u.sp->ct.list; ct && ct->id != add.id;
				     ct = ct->sp.next) ;
				if (!ct)
					goto badaddr;
				break;
			case ISUP_BIND_DF:
				for (ct = cc->bind.u.df->ct.list; ct && ct->id != add.id;
				     ct = ct->next) ;
				if (!ct)
					goto badaddr;
				break;
			default:
				goto efault;
			}
			break;
		case ISUP_SCOPE_CIC:
			goto badaddr;
		case ISUP_SCOPE_DF:
			if (cc->bind.type != ISUP_BIND_DF)
				goto badaddr;
			goto noaddr;
		case ISUP_SCOPE_SP:
			if (cc->bind.type != ISUP_BIND_SP)
				goto badaddr;
			goto noaddr;
		default:
			goto badaddr;
		}
	}
	if (!cc_get_uref(ct, cc, p->cc_user_ref))
		goto badclr;
	if (ct_tst(ct, CCTM_OUT_OF_SERVICE))
		goto reattempt_blocking;
	switch (ct_get_t_state(ct)) {
	case CKS_IDLE:
	case CKS_WIND_CONT:
		break;
	default:
		goto reattempt_busy;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_IDLE:
	case CCS_WREQ_SETUP:
		break;
	default:
		goto reattempt_busy;
	}
	if (ct_get_c_state(ct) != CTS_IDLE)
		goto reattempt_busy;
	todo(("Check if circuit requires continuity check\n"));
	/* 
	   look for test */
	if ((!(ck = ct->bind.tst) || ck->setind >= ck->maxind)
	    && (!(ck = ct->cg.cg->bind.tst) || ck->setind >= ck->maxind)
	    && (!(ck = ct->tg.tg->bind.tst) || ck->setind >= ck->maxind)
	    && (!(ck = ct->sr.sr->bind.tst) || ck->setind >= ck->maxind)
	    && (!(ck = ct->sp.sp->bind.tst) || ck->setind >= ck->maxind) && (!(ck = master.bind.tst)
									     || ck->setind >=
									     ck->maxind)
	    )
		p->cc_call_flags &= ~ISUP_NCI_CONT_CHECK_REQUIRED;	/* go without cot */
	ct_timer_start(ct, t7);
	if ((err =
	     isup_send_iam(q, ct, p->cc_call_flags, p->cc_call_type, NULL, 0, cdpn_ptr, cdpn_len,
			   opt_ptr, opt_len)))
		return (err);
	switch (p->cc_call_flags & ISUP_NCI_CONT_CHECK_MASK) {
	case ISUP_NCI_CONT_CHECK_REQUIRED:
		ct_timer_start(ct, t24);
		switch (ct_get_t_state(ct)) {
		case CKS_IDLE:
		case CKS_WIND_CONT:
			if (!(ct->tg.tg->proto.popt & SS7_POPT_LPA)) {
				/* 
				   if we are not going to receive confirmation from the other side
				   (LPA message) we should issue the confirmation now */
				ct_set_t_state(ct, ck, CKS_WIND_CONT);
				if ((err = cc_cont_test_ind(q, ck, ct)))
					return (err);
				ct_set_t_state(ct, ck, CKS_WREQ_CCREP);
			} else
				ct_set_t_state(ct, ck, CKS_WIND_CTEST);
		}
		ct_set(ct, CCTM_CONT_CHECK);
		ct_set_i_state(ct, cc, CCS_WCON_SREQ);
		break;
	case ISUP_NCI_CONT_CHECK_PREVIOUS:
		/* 
		   responsibility of CC to report cot */
		ct_set(ct, CCTF_COR_PENDING);
		ct_set_i_state(ct, cc, CCS_WREQ_CCREP);
		break;
	default:
		ct_set_i_state(ct, cc, CCS_WREQ_SETUP);
		if (!isup_st_in_cdpn(cdpn_ptr, cdpn_len)) {
			if ((err = cc_more_info_ind(q, cc, ct)))
				return (err);
			ct_set_i_state(ct, cc, CCS_WREQ_INFO);
		} else {
			ct_set_i_state(ct, cc, CCS_WCON_SREQ);
		}
		break;
	}
	if (!isup_st_in_cdpn(cdpn_ptr, cdpn_len)) {
		ct_set_c_state(ct, CTS_OGC_WAIT_SAM);
	} else {
		ct_set_c_state(ct, CTS_OGC_WAIT_ACM);
	}
	return (QR_DONE);
      reattempt_busy:
	/* 
	   responsibility of CC to reattempt */
	if ((err = cc_call_reattempt_ind(q, cc, ct, ISUP_REATTEMPT_CIRCUIT_BUSY)))
		return (err);
	return (QR_DONE);
      reattempt_blocking:
	printd(("%s: %p: circuit id=%ld, i_state = %ld, c_state = %ld, m_tate = %ld, flags = %08lx\n", DRV_NAME, cc, ct->id, ct->i_state, ct->c_state, ct->m_state, ct->flags));
	/* 
	   responsibility of CC to reattempt */
	if ((err = cc_call_reattempt_ind(q, cc, ct, ISUP_REATTEMPT_BLOCKING)))
		return (err);
	return (QR_DONE);
      failbusy:
	return cc_call_failure_ind(q, ct->cpc.cc, ct, ISUP_CALL_FAILURE_CIRCUIT_BUSY,
				   CC_CAUS_NO_CCT_AVAILABLE);
#if defined(_SAFE)||defined(_DEBUG)
      null_bind:
	pswerr(("%s: %p: SWERR: null bind pointer in bound state\n", DRV_NAME, cc));
	goto efault;
#endif
      efault:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EFAULT);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      noaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCNOADDR);
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      baddigs:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADDIGS);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_MORE_INFO_REQ:
 *  ------------------------------------
 *  Accepting CC_MORE_INFO_REQ is merely for compatibility with Q.931.
 *  Well, we caould always solicit information with an INF.
 */
STATIC int
cc_more_info_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_more_info_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	if (cs_get_state(cc) != CCS_WRES_SIND)
		goto outstate;
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:	/* be forgiving */
		break;
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
		ct_timer_start(ct, t35);
		break;
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
	case CTS_ICC_SEND_RLC:
	case CTS_OGC_SEND_RLC:
		goto ignore;
	default:
		swerr();
		goto outstate;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_MORE:
		ct_set_i_state(ct, cc, CCS_WIND_INFO);
		break;
	default:
		swerr();
		goto outstate;
	}
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_INFORMATION_REQ:
 *  ------------------------------------
 */
STATIC int
cc_information_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr, *subn_ptr;
	size_t opt_len, subn_len;
	struct CC_information_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	subn_ptr = mp->b_rptr + p->cc_subn_offset;
	subn_len = p->cc_subn_length;
	if (mp->b_wptr < subn_ptr + subn_len)
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_subn(subn_ptr, subn_len))
		goto baddigs;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_uref(cc, p->cc_user_ref)))
		goto badclr;
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_INFO:
		switch (ct_get_c_state(ct)) {
		case CTS_OGC_WAIT_SAM:
			/* 
			   restart if running */
			if (ct->timers.t7)
				ct_timer_start(ct, t7);
			if ((err = isup_send_sam(q, ct, subn_ptr, subn_len, opt_ptr, opt_len)))
				return (err);
			/* 
			   if there is an ST code in the subn then make this state change */
			if (isup_st_in_subn(subn_ptr, subn_len))
				ct_set_c_state(ct, CTS_OGC_WAIT_ACM);
			break;
		case CTS_ICC_SEND_RLC:
		case CTS_OGC_SEND_RLC:
			goto ignore;
		default:
			swerr();
			goto outstate;
		}
		ct_set_i_state(ct, cc, CCS_WACK_INFO);
		/* 
		   fall through */
	case CCS_WACK_INFO:
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		if (!isup_st_in_subn(subn_ptr, subn_len))
			ct_set_i_state(ct, cc, CCS_WREQ_INFO);
		else
			ct_set_i_state(ct, cc, CCS_WCON_SREQ);
		return (QR_DONE);
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	rare();
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      baddigs:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADDIGS);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_CONT_CHECK_REQ:
 *  ------------------------------------
 */
STATIC int
cc_cont_check_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_cont_check_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_UNBND:
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = isup_find_cct(cc, add_ptr, add_len)) && !(ct = cc->conn.tst))
		goto badaddr;
	switch (ct_get_t_state(ct)) {
	case CKS_IDLE:
	case CKS_WIND_CONT:
		if (!(ct->tg.tg->proto.popt & SS7_POPT_LPA)) {
			/* 
			   if we are not going to receive confirmation from the other side (LPA
			   message) we should issue the confirmation now */
			ct_set_t_state(ct, cc, CKS_WIND_CONT);
			if ((err = cc_cont_test_ind(q, cc, ct)))
				return (err);
			ct_set_t_state(ct, cc, CKS_WREQ_CCREP);
		} else
			ct_set_t_state(ct, cc, CKS_WIND_CTEST);
		break;
	case CKS_WREQ_CCREP:
	case CKS_WIND_CTEST:
		/* 
		   second time through after error */
		break;
	default:
		goto outstate;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_IDLE:
		break;
	case CTS_OGC_WAIT_CCR:
		ct_timer_stop(ct, t25);
		ct_timer_stop(ct, t26);
		break;
	default:
		goto outstate;
	}
	if ((err = isup_send_ccr(q, ct)))
		return (err);
	ct_set(ct, CCTM_CONT_CHECK);
	ct_timer_start(ct, t24);
	ct_set_c_state(ct, CTS_OGC_WAIT_CCR);
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_CONT_TEST_REQ:
 *  ------------------------------------
 */
STATIC int
cc_cont_test_req(queue_t *q, mblk_t *mp)
{
	struct cc *ac, *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	struct CC_cont_test_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cs_get_state(cc) != CCS_WRES_SIND)
		goto outstate;
	if (!(ct = cc_find_tst_cref(cc, p->cc_call_ref)))
		goto badclr;
	if (!ct_tst(ct, CCTF_COT_PENDING))
		goto outstate;
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
	case CTS_ICC_WAIT_CCR:
		break;
	default:
		goto outstate;
	}
	switch (ct_get_t_state(ct)) {
	case CKS_WREQ_CTEST:
		if (p->cc_token_value) {
			if (!(ac = cc_find_tok(cc, p->cc_token_value)))
				goto badtok;
			ct_swap_cref(ct, ac);
		}
		ct_set_t_state(ct, ct->tst.cc, CKS_WIND_CCREP);
		/* 
		   fall through */
	case CKS_WIND_CCREP:
		break;
	default:
		goto outstate;
	}
	if (ct_tst(ct, CCTF_LPA_PENDING)) {
		if (ct->tg.tg->proto.popt & SS7_POPT_LPA)
			if ((err = isup_send_lpa(q, ct)))
				return (err);
		ct_clr(ct, CCTF_LPA_PENDING);
	}
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badtok:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADTOK);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_CONT_REPORT_REQ:
 *  ------------------------------------
 */
STATIC int
cc_cont_report_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	struct CC_cont_report_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		trace();
		goto outstate;
	}
	if (p->cc_result != ISUP_COT_SUCCESS && p->cc_result != ISUP_COT_FAILURE)
		goto badflag;
	if (p->cc_user_ref && p->cc_call_ref)
		goto badclr;
	if ((cc->conn.tst && !(ct = cc_find_tst_cref(cc, p->cc_call_ref)))
	    || (cc->conn.cpc && !(ct = cc_find_cpc_uref(cc, p->cc_user_ref))) || !ct)
		goto badclr;
	switch (ct_get_c_state(ct)) {
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
	case CTS_OGC_WAIT_CCR:
		if (ct_tst(ct, CCTF_COR_PENDING))
			break;
	default:
		trace();
		goto outstate;
	}
	if (cc == ct->cpc.cc) {
		switch (ct_get_i_state(ct)) {
		case CCS_WREQ_CCREP:
			switch (p->cc_result) {
			case ISUP_COT_SUCCESS:
				switch (ct_get_c_state(ct)) {
				case CTS_OGC_WAIT_SAM:
					if ((err = cc_more_info_ind(q, ct->cpc.cc, ct)))
						return (err);
					ct_set_i_state(ct, ct->cpc.cc, CCS_WREQ_INFO);
					break;
				case CTS_OGC_WAIT_ACM:
					ct_set_i_state(ct, ct->cpc.cc, CCS_WCON_SREQ);
					break;
				case CTS_OGC_WAIT_CCR:
					swerr();
					ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
					break;
				}
				break;
			case ISUP_COT_FAILURE:
				switch (ct_get_c_state(ct)) {
				case CTS_OGC_WAIT_SAM:
				case CTS_OGC_WAIT_ACM:
					/* 
					   responsibility of CC to reattempt */
					if ((err =
					     cc_call_reattempt_ind(NULL, ct->cpc.cc, ct,
								   ISUP_REATTEMPT_COT_FAILURE)))
						return (err);
					ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
				}
				break;
			}
		case CCS_IDLE:
		case CCS_WIND_MORE:
		case CCS_WREQ_INFO:
		case CCS_WIND_PROCEED:
			/* 
			   second time through after error */
			break;
		default:
			trace();
			goto outstate;
		}
	}
	if (cc == ct->tst.cc) {
		switch (ct_get_t_state(ct)) {
		case CKS_WREQ_CCREP:
			switch (p->cc_result) {
			case ISUP_COT_SUCCESS:
				switch (ct_get_c_state(ct)) {
				case CTS_OGC_WAIT_SAM:
					if (ct_get_i_state(ct) != CCS_IDLE) {
						if ((err = cc_more_info_ind(q, ct->cpc.cc, ct)))
							return (err);
						ct_set_i_state(ct, ct->cpc.cc, CCS_WREQ_INFO);
					}
					ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
					break;
				case CTS_OGC_WAIT_ACM:
					if (ct_get_i_state(ct) != CCS_IDLE) {
						ct_set_i_state(ct, ct->cpc.cc, CCS_WCON_SREQ);
					}
					ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
					break;
				case CTS_OGC_WAIT_CCR:
					if (ct_get_i_state(ct) != CCS_IDLE) {
						ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
					}
					ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
					break;
				}
				break;
			case ISUP_COT_FAILURE:
				switch (ct_get_c_state(ct)) {
				case CTS_OGC_WAIT_SAM:
				case CTS_OGC_WAIT_ACM:
					if (ct_get_i_state(ct) != CCS_IDLE) {
						/* 
						   responsibility of CC to reattempt */
						if ((err =
						     cc_call_reattempt_ind(NULL, ct->cpc.cc, ct,
									   ISUP_REATTEMPT_COT_FAILURE)))
							return (err);
						ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
					}
					ct_set_t_state(ct, ct->tst.cc, CKS_WIND_CTEST);
					break;
				case CTS_OGC_WAIT_CCR:
					ct_set_t_state(ct, ct->tst.cc, CKS_WIND_CTEST);
					break;
				}
				break;
			}
			break;
		case CKS_IDLE:
		case CKS_WIND_CTEST:
			/* 
			   second time through after error */
			break;
		default:
			trace();
			goto outstate;
		}
	}
	/* 
	   responsibility of CM to remove COT */
	if (ct_tst(ct, CCTF_COT_PENDING))
		ct_timer_stop(ct, t24);
	if ((err = isup_send_cot(q, ct, p->cc_result)))
		return (err);
	switch (p->cc_result) {
	case ISUP_COT_SUCCESS:
		if (ct_get_c_state(ct) == CTS_OGC_WAIT_CCR)
			ct_set_c_state(ct, CTS_IDLE);
		break;
	case ISUP_COT_FAILURE:
		if (ct_get_c_state(ct) == CTS_OGC_WAIT_CCR) {
			ct_timer_start(ct, t26);
			ct_set_c_state(ct, CTS_OGC_WAIT_CCR);	/* prepare for cont recheck */
		} else if (ct_tst(ct, CCTF_COT_PENDING)) {
			ct_timer_start(ct, t25);
			ct_set_c_state(ct, CTS_OGC_WAIT_CCR);	/* prepare for cont recheck */
		} else {
			ct_set_c_state(ct, CTS_IDLE);
		}
		break;
	}
	ct_clr(ct, CCTM_CONT_CHECK);
	return (QR_DONE);
      badflag:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADFLAG);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_SETUP_RES:
 *  ------------------------------------
 */
STATIC int
cc_setup_res(queue_t *q, mblk_t *mp)
{
	struct cc *ac, *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	struct CC_setup_res *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cs_get_state(cc) != CCS_WRES_SIND)
		goto outstate;
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
		if (ct_tst(ct, CCTM_CONT_CHECK))
			goto outstate;
		break;
	default:
		goto outstate;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WRES_SIND:
		if (p->cc_token_value) {
			if (!(ac = cc_find_tok(cc, p->cc_token_value)))
				goto badtok;
			ct_swap_cref(ct, ac);
		}
		break;
	default:
		goto outstate;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
		ct_set_i_state(ct, ct->cpc.cc, CCS_WREQ_MORE);
		break;
	case CTS_ICC_WAIT_ACM:
		ct_set_i_state(ct, ct->cpc.cc, CCS_WREQ_PROCEED);
		break;
	}
	return cc_ok_ack(q, cc, ct, p->cc_primitive);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badtok:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADTOK);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_PROCEEDING_REQ:
 *  ------------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: ... ct) in the destination
 *  exchange when the address complete messge or connect message has been sent and the called party does not
 *  subscribe to a supplementary ervice which would cause a new call setup
 */
STATIC int
cc_proceeding_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_proceeding_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_MORE:
	case CCS_WIND_INFO:
	case CCS_WRES_SIND:
	case CCS_WREQ_PROCEED:
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
		case CTS_ICC_WAIT_ACM:
			/* 
			   release iam information */
			if ((err = isup_send_acm(q, ct, p->cc_flags, opt_ptr, opt_len)))
				return (err);
			ct_set_c_state(ct, CTS_ICC_WAIT_ANM);
			break;
		case CTS_ICC_WAIT_RLC:
		case CTS_OGC_WAIT_RLC:
		case CTS_ICC_SEND_RLC:
		case CTS_OGC_SEND_RLC:
			goto ignore;
		default:
			goto outstate;
		}
		ct_set_i_state(ct, cc, CCS_WACK_PROCEED);
		/* 
		   fall through */
	case CCS_WACK_PROCEED:
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		ct_set_i_state(ct, cc, CCS_WREQ_ALERTING);
		return (QR_DONE);
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	rare();
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_ALERTING_REQ:
 *  ------------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: ... c) in the destination
 *  exchange when the address complete messge or connect message has been sent and the called party does not
 *  subscribe to a supplementary ervice which would cause a new call setup
 */
STATIC int
cc_alerting_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_alerting_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_PROCEED:
	case CCS_WREQ_ALERTING:
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
		case CTS_ICC_WAIT_ACM:
			/* 
			   release iam information */
			if ((err = isup_send_acm(q, ct, p->cc_flags, opt_ptr, opt_len)))
				return (err);
			ct_set_c_state(ct, CTS_ICC_WAIT_ANM);
			break;
		case CTS_ICC_WAIT_ANM:
			if ((err = isup_send_cpg(q, ct, ISUP_EVNT_ALERTING, opt_ptr, opt_len)))
				return (err);
			ct_set_c_state(ct, CTS_ICC_WAIT_ANM);
			break;
		case CTS_ICC_WAIT_RLC:
		case CTS_OGC_WAIT_RLC:
		case CTS_ICC_SEND_RLC:
		case CTS_OGC_SEND_RLC:
			goto ignore;
		default:
			goto outstate;
		}
		ct_set_i_state(ct, cc, CCS_WACK_ALERTING);
		/* 
		   fall through */
	case CCS_WACK_ALERTING:
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		ct_set_i_state(ct, cc, CCS_WREQ_PROGRESS);
		return (QR_DONE);
	case CCS_WRES_RELIND:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	rare();
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_PROGRESS_REQ:
 *  ------------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: ... c) in the destination
 *  exchange when the address complete messge or connect message has been sent and the called party does not
 *  subscribe to a supplementary ervice which would cause a new call setup
 */
STATIC int
cc_progress_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_progress_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_PROCEED:
	case CCS_WREQ_ALERTING:
	case CCS_WREQ_PROGRESS:
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
		case CTS_ICC_WAIT_ACM:
			/* 
			   can we not send CPG before ACM? */
			/* 
			   release iam information */
			if ((err = isup_send_acm(q, ct, p->cc_flags, opt_ptr, opt_len)))
				return (err);
			ct_set_c_state(ct, CTS_ICC_WAIT_ANM);
			break;
		case CTS_ICC_WAIT_ANM:
			if ((err = isup_send_cpg(q, ct, p->cc_event, opt_ptr, opt_len)))
				return (err);
			ct_set_c_state(ct, CTS_ICC_WAIT_ANM);
			break;
		case CTS_ICC_WAIT_RLC:
		case CTS_OGC_WAIT_RLC:
		case CTS_ICC_SEND_RLC:
		case CTS_OGC_SEND_RLC:
			goto ignore;
		default:
			goto outstate;
		}
		ct_set_i_state(ct, cc, CCS_WACK_PROGRESS);
		/* 
		   fall through */
	case CCS_WACK_PROGRESS:
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		ct_set_i_state(ct, cc, CCS_WREQ_PROGRESS);
		return (QR_DONE);
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_IBI_REQ:
 *  ------------------------------------
 *  2.2.4 ... If call setup fails and an inband tone or announcement has to be returned to the calling party from
 *  an exchange or called party, the exchange or user concerned connects the inband tone or announcement to the
 *  transmission path.  If a time-out occurs at the exchange providing the inband tone or announcement, the
 *  exchange sends a release message to the preceding exchange with cause value #31 (normal unspecified).
 *
 *  If an address complete message has been returned to the preceding exchange a call progress message indicating
 *  that inband tone information is available along with the cause parameter, is returned to the preceding
 *  exchange (see 2.1.5).  The cause value should reflect the reason of call failure in the same way as the inband
 *  tone or announcement to be applied.
 *
 *  If an address complete message has not been returned to the preceding exchange already, an address complete
 *  message with the cause parameter and the "in-band information" indicator set in the optional backward call
 *  indicator, will be returned to the originating exchange.  The cause value should reflect the reason of call
 *  failure in the same way as the in-band tone or announcement to be applied.
 *
 *  In case a special tone or announcement has to be applied due to an event only known by a certain exchange and
 *  not covered by a cause value, not cause parameter is included in either the address complete or call progress
 *  messages.  The answer message must not be sent in this case.
 *
 *  For the preceding exchanges the inclusion of the cause parameter in the address complete or call progress
 *  message implies an unsuccessful call setup.  The cause parameter will not be included for unsuccessful call
 *  stups when interworking has occured and the in-band tone or announcement is returned from the exchange beyond
 *  the interworking point.
 *
 *  2.3.4 ... The initial address message information can be released from memory: ... c) in the destination
 *  exchange when the address complete messge or connect message has been sent and the called party does not
 *  subscribe to a supplementary ervice which would cause a new call setup
 */
STATIC int
cc_ibi_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_ibi_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_PROCEED:
	case CCS_WREQ_ALERTING:
	case CCS_WREQ_PROGRESS:
		switch (ct_get_c_state(ct)) {
		case CTS_ICC_WAIT_SAM:
		case CTS_ICC_WAIT_ACM:
			/* 
			   release iam information */
			/* 
			   need to add ibi (obci) and cause value to opt parms */
			if ((err = isup_send_acm(q, ct, p->cc_flags, opt_ptr, opt_len)))
				return (err);
			ct_set_c_state(ct, CTS_ICC_WAIT_ANM);
			break;
		case CTS_ICC_WAIT_ANM:
			/* 
			   need to add ibi (obci) and cause value to opt parms */
			if ((err = isup_send_cpg(q, ct, ISUP_EVNT_IBI, opt_ptr, opt_len)))
				return (err);
			ct_set_c_state(ct, CTS_ICC_WAIT_ANM);
			break;
		case CTS_ICC_WAIT_RLC:
		case CTS_OGC_WAIT_RLC:
		case CTS_ICC_SEND_RLC:
		case CTS_OGC_SEND_RLC:
			goto ignore;
		default:
			goto outstate;
		}
		ct_set_i_state(ct, cc, CCS_WACK_IBI);
		/* 
		   fall through */
	case CCS_WACK_IBI:
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		ct_set_i_state(ct, cc, CCS_WREQ_CONNECT);
		return (QR_DONE);
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_CONNECT_REQ
 *  ------------------------------------
 *  2.3.4 ... The initial address message information can be released from memory: ... c) in the destination
 *  exchange when the address complete messge or connect message has been sent and the called party does not
 *  subscribe to a supplementary ervice which would cause a new call setup
 */
STATIC int
cc_connect_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_connect_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->cc_opt_offset + p->cc_opt_length)
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
	case CTS_ICC_WAIT_ANM:
		break;
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
	case CTS_ICC_SEND_RLC:
	case CTS_OGC_SEND_RLC:
		goto ignore;
	default:
		goto outstate;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_WREQ_PROCEED:
	case CCS_WREQ_ALERTING:
	case CCS_WREQ_PROGRESS:
	case CCS_WREQ_CONNECT:
		if ((err = cc_setup_complete_ind(q, ct)))
			return (err);
		ct_set_i_state(ct, cc, CCS_CONNECTED);
		break;
	case CCS_CONNECTED:
		/* 
		   second time through */
		break;
	case CCS_WRES_RELIND:
		goto ignore;
	default:
	case CCS_WCON_RELREQ:
		goto outstate;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
		/* 
		   release iam information */
		if ((err = isup_send_con(q, ct, p->cc_flags, opt_ptr, opt_len)))
			return (err);
		ct_set_c_state(ct, CTS_ICC_ANSWERED);
		break;
	case CTS_ICC_WAIT_ANM:
		if ((err = isup_send_anm(q, ct, opt_ptr, opt_len)))
			return (err);
		ct_set_c_state(ct, CTS_ICC_ANSWERED);
		break;
	default:
		swerr();
		goto outstate;
	}
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_SETUP_COMPLETE_REQ
 *  ------------------------------------
 *  Accepting CC_SETUP_COMPLETE_REQ is merely for compatiblity with Q.931.
 */
STATIC int
cc_setup_complete_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_setup_complete_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_i_state(ct)) {
	case CCS_WRES_SIND:
	case CCS_CONNECTED:	/* be forgiving */
		break;
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_ANSWERED:
	case CTS_OGC_ANSWERED:
		ct_set_i_state(ct, cc, CCS_CONNECTED);
		break;
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
	case CTS_ICC_SEND_RLC:
	case CTS_OGC_SEND_RLC:
		goto ignore;
	default:
		swerr();
		goto outstate;
	}
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_FORWXFER_REQ:
 *  ------------------------------------
 */
STATIC int
cc_forwxfer_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_forwxfer_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_i_state(ct)) {
	case CCS_WIND_PROGRESS:
	case CCS_WIND_CONNECT:
	case CCS_CONNECTED:
		switch (ct_get_c_state(ct)) {
		case CTS_OGC_WAIT_ANM:
		case CTS_OGC_ANSWERED:
			if ((err = isup_send_fot(q, ct, opt_ptr, opt_len)))
				return (err);
			break;
		case CTS_ICC_WAIT_RLC:
		case CTS_OGC_WAIT_RLC:
		case CTS_ICC_SEND_RLC:
		case CTS_OGC_SEND_RLC:
			goto ignore;
		default:
			goto outstate;
		}
		ct_set_i_state(ct, cc, CCS_WACK_FORWXFER);
		/* 
		   fall through */
	case CCS_WACK_FORWXFER:
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		switch (ct_get_c_state(ct)) {
		case CTS_OGC_WAIT_ANM:
			ct_set_i_state(ct, cc, CCS_WIND_CONNECT);
			break;
		case CTS_OGC_ANSWERED:
			ct_set_i_state(ct, cc, CCS_CONNECTED);
			break;
		}
		return (QR_DONE);
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	rare();
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_SUSPEND_REQ:
 *  ------------------------------------
 *  2.4.1  Suspend.  The suspend message indicates a temporary cessation of communication without releasing the
 *  call.  It can only be accepted during the conversation/data phase.
 *
 *  A suspend message can be generated by the network in response to a clearback indication from an interworking
 *  node or an on-hook condition from an analogue called party.
 *
 *  a)  Action at the destination exchange or an interworking exchange
 *
 *      On receipt of an on-hook condition in the destination exchange or a clearback signal at the interworking
 *      exchange, the exchange may send a suspend (network) message to the preceding exchange.
 *
 *  b)  Action at the intermediate exchange
 *
 *      On receipt of a suspend message the exchange will send a suspend message to the preceding exchange.
 *
 *  c)  Action at the controlling exchange (i.e, exchange controlling the call)
 *
 *      On receipt of the on-hook condition or clearback indication or suspend message, the controlling exchange
 *      starts a timer (T6) to ensure that an off-hook condition, a re-answer indication, a resume (network)
 *      message or a release message is received and, if applicable, sends a suspend (network) message to the
 *      preceding exchange.  The value of this timer (T6) is covered in Recommendation Q.118.  If the timer (T6)
 *      expires, the procedures in 2.4.3 apply.
 *
 *  d)  Actions at the incoming international exchange
 *
 *      On receipt of the on-hook condition or clearback indication or suspend (network) message, the incoming
 *      international exchange sends to the preceding exchange a suspend (network) message and starts a timer
 *      (T38), to ensure that a release message is received.  The value of this timer is covered in Recommendation
 *      Q.118.  If the timer (T38) expires, the procedure in 2.4.3 applies.  The procedure described above may not
 *      be applied in the incomining international exchange if a similar arrangement is already made in the
 *      network of the incoming country.
 */
STATIC int
cc_suspend_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_suspend_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_i_state(ct)) {
	case CCS_CONNECTED:
	case CCS_SUSPENDED:
		break;
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_ANSWERED:
	case CTS_ICC_SUSPENDED:
	{
		ulong sris = p->cc_flags;
		ct_set(ct, CCTF_ORIG_SUSPENDED);
		if ((err = isup_send_sus(q, ct, sris, opt_ptr, opt_len)))
			return (err);
		ct_set_i_state(ct, cc, CCS_SUSPENDED);
		ct_set_c_state(ct, CTS_ICC_SUSPENDED);
		break;
	}
	case CTS_OGC_ANSWERED:
	{
		struct tg *tg = ct->tg.tg;
		if ((tg->config.flags & ISUP_TGF_CONTROLLING_EXCHANGE)) {
			switch (p->cc_flags & ISUP_SRIS_MASK) {
			case ISUP_SRIS_NETWORK_INITIATED:
				ct_timer_start(ct, t6);
				break;
			case ISUP_SRIS_USER_INITIATED:
				ct_timer_start(ct, t2);
				break;
			}
		}
		if ((tg->config.flags & ISUP_TGF_INCOMING_INTERNATIONAL_EXCHANGE)
		    && !(tg->config.flags & ISUP_TGF_SUSPEND_NATIONALLY_PERFORMED))
			ct_timer_start(ct, t38);
		/* 
		   fall through */
	}
	case CTS_OGC_SUSPENDED:
	{
		ulong sris = p->cc_flags;

		ct_set(ct, CCTF_ORIG_SUSPENDED);
		if ((err = isup_send_sus(q, ct, sris, opt_ptr, opt_len)))
			return (err);
		ct_set_i_state(ct, cc, CCS_SUSPENDED);
		ct_set_c_state(ct, CTS_OGC_SUSPENDED);
		break;
	}
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
	case CTS_ICC_SEND_RLC:
	case CTS_OGC_SEND_RLC:
		goto ignore;
	default:
		goto outstate;
	}
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_RESUME_REQ:
 *  ------------------------------------
 *  2.4.2  Resume.  A resume message indicates a request to recommence communication.  A request to release the
 *  call received from the calling party will override the suspend/resume sequence and the procedures given in 2.3
 *  will be followed.
 *
 *  A resume message is initiated by the network, if a suspend message had previously been sent, in response to a
 *  re-answer indication from an interworking node or an off-hook condition from an analogue called party.
 *
 *  a)  Action at the destination exchange or interworking exchange
 *
 *      On receipt of a re-answer indication at the interworking exchange or an off-hook condition in the
 *      destination exchange, the exchange may send a resume (network) message to the preceding exchange if a
 *      suspend (network) message had previously been sent.
 *
 *  b)  Action of the intermediate exchange
 *
 *      On receipt of a resume message the exchange will send a resume message to the preceding exchange.
 *
 *  c)  Action of the controlling exchange (i.e, exchange controlling the call)
 *
 *      On receipt of the off-hook condition, re-answer signal, or resume (network) message, the controlling
 *      exchange stops the timer (T6) and, if applicable, sends a resume (network) message to the preceding
 *      exchange.  On receipt of a release message the controlling exchange stops the timer T6 and releases the
 *      call according to the procedures in 2.3.
 *
 *  d)  Actions ar the incoming international exchange
 *
 *      On receipt of the off-hook condition, re-answer signal, or resume (network) message from the succeeding
 *      exchange, the incoming international exchange stops the timer (T38) and sends resume (network) message to
 *      the preceding exchange.  On receipt of a release message the incoming international exchange stops the
 *      timer T38 and releases the call according to the procedure in 2.3.
 */
STATIC int
cc_resume_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_resume_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(ct = cc_find_cpc_cref(cc, p->cc_call_ref)))
		goto badclr;
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_SUSPENDED:
		break;
	case CTS_OGC_SUSPENDED:
		switch (p->cc_flags & ISUP_SRIS_MASK) {
		case ISUP_SRIS_NETWORK_INITIATED:
			ct_timer_stop(ct, t6);
			break;
		case ISUP_SRIS_USER_INITIATED:
			ct_timer_stop(ct, t2);
			break;
		}
		ct_timer_stop(ct, t38);
		break;
	}
	switch (ct_get_i_state(ct)) {
	case CCS_SUSPENDED:
		break;
	case CCS_WRES_RELIND:
	case CCS_WCON_RELREQ:
		goto ignore;
	default:
		goto outstate;
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_SUSPENDED:
	{
		ulong sris = p->cc_flags;
		if ((err = isup_send_res(q, ct, sris, opt_ptr, opt_len)))
			return (err);
		ct_clr(ct, CCTF_ORIG_SUSPENDED);
		if (ct_tst(ct, CCTF_TERM_SUSPENDED)) {
			ct_set_i_state(ct, cc, CCS_CONNECTED);
			ct_set_c_state(ct, CTS_ICC_ANSWERED);
		}
		break;
	}
	case CTS_OGC_SUSPENDED:
	{
		ulong sris = p->cc_flags;
		if ((err = isup_send_res(q, ct, sris, opt_ptr, opt_len)))
			return (err);
		ct_clr(ct, CCTF_ORIG_SUSPENDED);
		if (ct_tst(ct, CCTF_TERM_SUSPENDED)) {
			ct_set_i_state(ct, cc, CCS_CONNECTED);
			ct_set_c_state(ct, CTS_OGC_ANSWERED);
		}
		break;
	}
	case CTS_ICC_WAIT_RLC:
	case CTS_OGC_WAIT_RLC:
	case CTS_ICC_SEND_RLC:
	case CTS_OGC_SEND_RLC:
		goto ignore;
	default:
		swerr();
		goto outstate;
	}
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_RELEASE_REQ:
 *  ------------------------------------
 *  On receipt of a request to release the call from the calling party, the originating exchagne immediately start
 *  the release of the switched path.  A release message is sent to the succeeding exchange and timers T1 and T5
 *  are started to ensure that a release complete message is received from the succeeding echange.
 *
 *  2.3.4 ... The initial address message information can be released from memory: ... when the call is released
 *  earlier and no automatic repeat attempt is to be attempted.
 *
 *  2.3.1 d) Charging is stopped upon receipt of the release message at the charging exchange or on receipt of a
 *  receipt request to release the call from the calling party when the charging exchange is the originating
 *  exchange.
 */
STATIC int
cc_release_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_release_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (p->cc_user_ref && p->cc_call_ref)
		goto badclr;
	if ((cc->conn.cpc && !(ct = cc_find_cpc_cref(cc, p->cc_call_ref))
	     && !(ct = cc_find_cpc_uref(cc, p->cc_user_ref))) || (cc->conn.tst
								  && !(ct =
								       cc_find_tst_cref(cc,
											p->
											cc_call_ref)))
	    || !ct)
		goto badclr;
	if (cc == ct->tst.cc) {
		switch (ct_get_t_state(ct)) {
		case CKS_WREQ_CCREP:
			ct_set_t_state(ct, ct->tst.cc, CKS_WCON_RELREQ);
			break;
		case CKS_WRES_RELIND:
			/* 
			   dual release case, treat the request as a response */
			/* 
			   user should treat indication as confirmation */
			ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
			break;
		case CKS_IDLE:
			goto outstate;
		}
	}
	if (cc == ct->cpc.cc) {
		switch (ct_get_t_state(ct)) {
		case CKS_WREQ_CCREP:
			if (ct_tst(ct, CCTF_COT_PENDING))
				ct_timer_stop(ct, t24);
			if ((err = cc_cont_report_ind(q, ct->tst.cc, ct, NULL)))
				return (err);
			ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
		}
		switch (ct_get_i_state(ct)) {
		case CCS_WRES_RELIND:
			/* 
			   dual release case, treat the request as a response */
			/* 
			   user should treat indication as confirmation */
			ct_set_i_state(ct, cc, CCS_IDLE);
			break;
		default:
			ct_set_i_state(ct, cc, CCS_WCON_RELREQ);
			break;
		case CCS_IDLE:
			goto outstate;
		}
	}
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_WAIT_SAM:
	case CTS_ICC_WAIT_ACM:
	case CTS_ICC_WAIT_CCR:
		/* 
		   CC responsibility to abort COT and remove loopback */
		if (ct_tst(ct, CCTF_COR_PENDING))
			ct_timer_stop(ct, t8);
		/* 
		   responsibility of CC to remove loopback */
		/* 
		   release iam information */
		goto release_icc;
	case CTS_ICC_WAIT_ANM:
	case CTS_ICC_ANSWERED:
	case CTS_ICC_SUSPENDED:
		goto release_icc;
	case CTS_OGC_WAIT_SAM:
	case CTS_OGC_WAIT_ACM:
	case CTS_OGC_WAIT_CCR:
		if (ct_tst(ct, CCTF_COT_PENDING))
			ct_timer_stop(ct, t24);
		ct_timer_stop(ct, t7);
		/* 
		   responsibility of CC to abort COT */
		goto release_ogc;
	case CTS_OGC_WAIT_ANM:
		goto release_ogc;
	case CTS_OGC_SUSPENDED:
		ct_timer_stop(ct, t6);
		ct_timer_stop(ct, t38);
		/* 
		   fall through */
	case CTS_OGC_ANSWERED:
		/* 
		   stop charging */
		goto release_ogc;
	case CTS_ICC_SEND_RLC:
	case CTS_OGC_SEND_RLC:
		/* 
		   waiting to send release complete -- dual release */
		if ((err = isup_send_rlc(q, ct, opt_ptr, opt_len)))
			return (err);
		ct_set_c_state(ct, CTS_IDLE);
	case CTS_IDLE:
		/* 
		   ignore it */
		return (QR_DONE);
	default:
		swerr();
		goto outstate;
	}
      release_icc:
	/* 
	   release iam information */
	ct_timer_start(ct, t5);
	ct_timer_start(ct, t1);
	if ((err = isup_send_rel(q, ct, p->cc_cause, opt_ptr, opt_len)))
		return (err);
	ct_set_c_state(ct, CTS_ICC_WAIT_RLC);
	return (QR_DONE);
      release_ogc:
	/* 
	   release iam information */
	ct_timer_start(ct, t5);
	ct_timer_start(ct, t1);
	if ((err = isup_send_rel(q, ct, p->cc_cause, opt_ptr, opt_len)))
		return (err);
	ct_set_c_state(ct, CTS_OGC_WAIT_RLC);
	return (QR_DONE);
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_RELEASE_RES:
 *  ------------------------------------
 */
STATIC int
cc_release_res(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	int err;
	uchar *opt_ptr;
	size_t opt_len;
	struct CC_release_res *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	opt_ptr = mp->b_rptr + p->cc_opt_offset;
	opt_len = p->cc_opt_length;
	if (mp->b_wptr < opt_ptr + opt_len)
		goto emsgsize;
	if (isup_check_opt(opt_ptr, opt_len))
		goto badopt;
	switch (cs_get_state(cc)) {
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (p->cc_user_ref && p->cc_call_ref)
		goto badclr;
	if ((cc->conn.cpc && !(ct = cc_find_cpc_cref(cc, p->cc_call_ref))
	     && !(ct = cc_find_cpc_uref(cc, p->cc_user_ref))) || (cc->conn.tst
								  && !(ct =
								       cc_find_tst_cref(cc,
											p->
											cc_call_ref)))
	    || !ct)
		goto badclr;
	if (cc == ct->tst.cc && ct_get_t_state(ct) != CKS_WRES_RELIND)
		goto outstate;
	if (cc == ct->cpc.cc && ct_get_i_state(ct) != CCS_WRES_RELIND)
		goto outstate;
	switch (ct_get_c_state(ct)) {
	case CTS_ICC_SEND_RLC:
	case CTS_OGC_SEND_RLC:
		if ((err = isup_send_rlc(q, ct, opt_ptr, opt_len)))
			return (err);
		ct_set_c_state(ct, CTS_IDLE);
	case CTS_IDLE:
		/* 
		   ignore it */
		break;
	default:
		swerr();
		goto outstate;
	}
	if (cc == ct->tst.cc && ct_get_t_state(ct) == CKS_WRES_RELIND) {
		if ((err = cc_ok_ack(q, ct->tst.cc, ct, p->cc_primitive)))
			return (err);
		ct_set_t_state(ct, ct->tst.cc, CKS_IDLE);
		return (QR_DONE);
	}
	if (cc == ct->cpc.cc && ct_get_i_state(ct) == CCS_WRES_RELIND) {
		if ((err = cc_ok_ack(q, ct->cpc.cc, ct, p->cc_primitive)))
			return (err);
		ct_set_i_state(ct, ct->cpc.cc, CCS_IDLE);
		return (QR_DONE);
	}
	swerr();
	goto outstate;
      badclr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADCLR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      badopt:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADOPT);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_RESET_REQ:
 *  ------------------------------------
 */
STATIC int
cc_reset_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	struct cg *cg = NULL;
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_reset_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_UNBND:
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (p->cc_flags & ISUP_GROUP) {
		if (!(cg = isup_find_grp(cc, add_ptr, add_len)) && (!(cg = cc->conn.gmg)))
			goto badaddr;
		/* 
		   need at least two circuits for a group action */
		if (!cg->ct.list || !cg->ct.list->cg.next)
			goto badaddr;
		if (cg_get_m_state(cg) != CMS_IDLE)
			goto outstate;
		if ((err = cg_reset(q, cg)))
			return (err);
		cg_set_m_state(cg, cc, CMS_WCON_RESREQ);
		return (QR_DONE);
	} else {
		if (!(ct = isup_find_cct(cc, add_ptr, add_len)) && !(ct = cc->conn.mgm))
			goto badaddr;
		if (ct_get_m_state(ct) != CMS_IDLE)
			goto outstate;
		if ((err = ct_reset(q, ct)))
			return (err);
		ct_set_m_state(ct, cc, CMS_WCON_RESREQ);
		return (QR_DONE);
	}
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_RESET_RES:
 *  ------------------------------------
 */
STATIC int
cc_reset_res(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	struct cg *cg;
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_reset_res *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_UNBND:
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (p->cc_flags & ISUP_GROUP) {
		if (!(cg = isup_find_grp(cc, add_ptr, add_len)) && !(cg = cc->conn.gmg))
			goto badaddr;
		/* 
		   complete procedure */
		if (cg_tst(cg, CCTF_REM_RESET_PENDING)) {
			struct ct *bc = NULL;
			int o, i, j;
			uchar rs[RS_MAX_RANGE] = { 0, };
			for (ct = cg->ct.list; ct; ct = ct->sr.next) {
				if (ct_tst(ct, CCTF_REM_RESET_PENDING)) {
					bc = bc ? bc : ct;
					o = ct->cic - bc->cic;
					j = (o >> 3) + 1;
					i = o & 0x7;
					if (rs[0] < o)
						rs[0] = o;
					if (ct_tst(ct, CCTF_LOC_M_BLOCKED))
						rs[j] |= (0x1 << i);
				}
			}
			if ((err = isup_send_gra(q, bc, rs, ((rs[0] + 7) >> 3) + 1)))
				return (err);
			for (ct = cg->ct.list; ct; ct = ct->sr.next)
				if (ct_tst(ct, CCTF_REM_RESET_PENDING))
					ct_clr(ct,
					       (CCTF_REM_M_BLOCKED | CCTF_REM_H_BLOCKED |
						CCTF_REM_RESET_PENDING));
			cg_clr(cg,
			       (CCTF_REM_M_BLOCKED | CCTF_REM_H_BLOCKED | CCTF_REM_RESET_PENDING));
		}
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		if (cg_get_m_state(cg) == CMS_WRES_RESIND)
			cg_set_m_state(cg, cc, CMS_IDLE);
		return (QR_DONE);
	} else {
		if (!(ct = isup_find_cct(cc, add_ptr, add_len)) && !(ct = cc->conn.mgm))
			goto badaddr;
		/* 
		   complete procedure */
		if (ct_tst(ct, CCTF_REM_RESET_PENDING)) {
			if ((err = isup_send_rlc(q, ct, NULL, 0)))
				return (err);
			ct_clr(ct,
			       (CCTF_REM_M_BLOCKED | CCTF_REM_H_BLOCKED | CCTF_REM_RESET_PENDING));
		}
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		if (ct_get_m_state(ct) == CMS_WRES_RESIND)
			ct_set_m_state(ct, cc, CMS_IDLE);
		return (QR_DONE);
	}
	goto outstate;
	return (QR_DONE);
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_BLOCKING_REQ:
 *  ------------------------------------
 */
STATIC int
cc_blocking_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	struct cg *cg;
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_blocking_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_UNBND:
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (p->cc_flags & ~(ISUP_GROUP | ISUP_HARDWARE_FAILURE_ORIENTED))
		goto badflag;
	if (p->cc_flags & ISUP_GROUP) {
		ulong cgi = (p->cc_flags & ISUP_HARDWARE_FAILURE_ORIENTED);
		if (!(cg = isup_find_grp(cc, add_ptr, add_len)) && (!(cg = cc->conn.gmg)))
			goto noaddr;
		/* 
		   need at least two circuits for a group action */
		if (!cg->ct.list || !cg->ct.list->cg.next)
			goto badaddr;
		if (cg_get_m_state(cg) != CMS_IDLE)
			goto outstate;
		if (cgi) {
			if ((err = cg_h_block(q, cg)))
				return (err);
		} else {
			if ((err = cg_m_block(q, cg)))
				return (err);
		}
		cg_set_m_state(cg, cc, CMS_WCON_BLREQ);
		return (QR_DONE);
	} else {
		if (p->cc_flags & ISUP_HARDWARE_FAILURE_ORIENTED)
			goto badflag;
		if (!(ct = isup_find_cct(cc, add_ptr, add_len)) && !(ct = cc->conn.mgm))
			goto noaddr;
		if (ct_get_m_state(ct) != CMS_IDLE)
			goto outstate;
		if ((err = ct_block(q, ct)))
			return (err);
		ct_set_m_state(ct, cc, CMS_WCON_BLREQ);
		return (QR_DONE);
	}
	return (QR_DONE);
      badflag:
	printd(("%s: %s: %p: ERROR bad flags = 0x%08lx\n", DRV_NAME, __FUNCTION__, cc,
		p->cc_flags));
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADFLAG);
      noaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCNOADDR);
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_BLOCKING_RES:
 *  ------------------------------------
 */
STATIC int
cc_blocking_res(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	struct cg *cg;
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_blocking_res *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_UNBND:
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (p->cc_flags & ISUP_GROUP) {
		ulong cgi = (p->cc_flags & ISUP_HARDWARE_FAILURE_ORIENTED);
		ulong CCTF_REM_X_BLOCK_PENDING =
		    cgi ? CCTF_REM_H_BLOCK_PENDING : CCTF_REM_M_BLOCK_PENDING;
		ulong CCTF_REM_X_BLOCKED = cgi ? CCTF_REM_H_BLOCKED : CCTF_REM_M_BLOCKED;
		if (!(cg = isup_find_grp(cc, add_ptr, add_len)) && !(cg = cc->conn.gmg))
			goto badaddr;
		/* 
		   complete procedure */
		if (cg_tst(cg, CCTF_REM_X_BLOCK_PENDING)) {
			struct ct *bc = NULL;
			int o, i, j;
			uchar rs[RS_MAX_RANGE] = { 0, };
			for (ct = cg->ct.list; ct; ct = ct->sr.next) {
				if (ct_tst(ct, CCTF_REM_G_BLOCK_PENDING)) {
					bc = bc ? bc : ct;
					o = ct->cic - bc->cic;
					j = (o >> 3) + 1;
					i = o & 0x7;
					if (rs[0] < o)
						rs[0] = o;
					rs[j] |= (0x1 << i);
				}
			}
			if ((err = isup_send_cgba(q, bc, cgi, rs, ((rs[0] + 7) >> 3) + 1)))
				return (err);
			for (ct = bc; ct; ct = ct->sr.next)
				if (ct_tst(ct, CCTF_REM_G_BLOCK_PENDING)) {
					ct_set(ct, CCTF_REM_X_BLOCKED);
					ct_clr(ct, CCTF_REM_G_BLOCK_PENDING);
				}
			cg_set(cg, CCTF_REM_X_BLOCKED);
			cg_clr(cg, CCTF_REM_X_BLOCK_PENDING);
		}
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		if (cg_get_m_state(cg) == CMS_WRES_BLIND)
			cg_set_m_state(cg, cc, CMS_IDLE);
		return (QR_DONE);
	} else {
		if (p->cc_flags & ISUP_HARDWARE_FAILURE_ORIENTED)
			goto badflag;
		if (!(ct = isup_find_cct(cc, add_ptr, add_len)) && !(ct = cc->conn.cpc))
			goto badaddr;
		/* 
		   complete procedure */
		if (ct_tst(ct, CCTF_REM_M_BLOCK_PENDING)) {
			if ((err = isup_send_bla(q, ct)))
				return (err);
			ct_set(ct, CCTF_REM_M_BLOCKED);
			ct_clr(ct, CCTF_REM_M_BLOCK_PENDING);
		}
		switch (ct_get_c_state(ct)) {
		case CTS_OGC_WAIT_SAM:
		case CTS_OGC_WAIT_ACM:
			/* 
			   responsibility of CC to disconnect tone/detector */
			/* 
			   CC responsibility to release iam information */
			/* 
			   does not put on idle list because remote block pending */
			ct_timer_start(ct, t5);
			ct_timer_start(ct, t1);
			if ((err = isup_send_rel(q, ct, CC_CAUS_NORMAL_UNSPECIFIED, NULL, 0)))
				return (err);
			ct_set_c_state(ct, CTS_OGC_WAIT_RLC);
			break;
		}
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		if (ct_get_m_state(ct) == CMS_WRES_BLIND)
			ct_set_m_state(ct, cc, CMS_IDLE);
		return (QR_DONE);
	}
	return (QR_DONE);
      badflag:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADFLAG);
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_UNBLOCKING_REQ:
 *  ------------------------------------
 */
STATIC int
cc_unblocking_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	struct cg *cg;
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_unblocking_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_UNBND:
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (p->cc_flags & ~(ISUP_GROUP | ISUP_HARDWARE_FAILURE_ORIENTED))
		goto badflag;
	if (p->cc_flags & ISUP_GROUP) {
		ulong cgi = (p->cc_flags & ISUP_HARDWARE_FAILURE_ORIENTED);
		if (!(cg = isup_find_grp(cc, add_ptr, add_len)) && (!(cg = cc->conn.gmg)))
			goto badaddr;
		/* 
		   need at least two circuits for a group action */
		if (!cg->ct.list || !cg->ct.list->cg.next)
			goto badaddr;
		if (cg_get_m_state(cg) != CMS_IDLE)
			goto outstate;
		if ((err = cg_unblock(q, cg, cgi)))
			return (err);
		cg_set_m_state(cg, cc, CMS_WCON_UBREQ);
		return (QR_DONE);
	} else {
		if (p->cc_flags & ISUP_HARDWARE_FAILURE_ORIENTED)
			goto badflag;
		if (!(ct = isup_find_cct(cc, add_ptr, add_len)) && !(ct = cc->conn.mgm))
			goto badaddr;
		if (ct_get_m_state(ct) != CMS_IDLE)
			goto outstate;
		if ((err = ct_unblock(q, ct)))
			return (err);
		ct_set_m_state(ct, cc, CMS_WCON_UBREQ);
		return (QR_DONE);
	}
	return (QR_DONE);
      badflag:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADFLAG);
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_UNBLOCKING_RES:
 *  ------------------------------------
 */
STATIC int
cc_unblocking_res(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	struct cg *cg;
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_unblocking_res *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_UNBND:
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (p->cc_flags & ISUP_GROUP) {
		ulong cgi = (p->cc_flags & ISUP_HARDWARE_FAILURE_ORIENTED);
		ulong CCTF_REM_X_UNBLOCK_PENDING =
		    cgi ? CCTF_REM_H_UNBLOCK_PENDING : CCTF_REM_M_UNBLOCK_PENDING;
		ulong CCTF_REM_X_BLOCKED = cgi ? CCTF_REM_H_BLOCKED : CCTF_REM_M_BLOCKED;
		if (!(cg = isup_find_grp(cc, add_ptr, add_len))
		    && (!(ct = cc->conn.cpc) || !(cg = ct->cg.cg)))
			goto badaddr;
		/* 
		   complete procedure */
		if (cg_tst(cg, CCTF_REM_X_UNBLOCK_PENDING)) {
			struct ct *bc = NULL;
			int o, i, j;
			uchar rs[RS_MAX_RANGE] = { 0, };
			for (ct = cg->ct.list; ct; ct = ct->sr.next) {
				if (ct_tst(ct, CCTF_REM_G_UNBLOCK_PENDING)) {
					bc = bc ? bc : ct;
					o = ct->cic - bc->cic;
					j = (o >> 3) + 1;
					i = o & 0x7;
					if (rs[0] < o)
						rs[0] = o;
					rs[j] |= (0x1 << i);
				}
			}
			if ((err = isup_send_cgua(q, bc, cgi, rs, ((rs[0] + 7) >> 3) + 1)))
				return (err);
			for (ct = bc; ct; ct = ct->sr.next)
				if (ct_tst(ct, CCTF_REM_G_UNBLOCK_PENDING)) {
					ct_clr(ct, CCTF_REM_X_BLOCKED);
					ct_clr(ct, CCTF_REM_G_UNBLOCK_PENDING);
				}
			cg_clr(cg, CCTF_REM_X_BLOCKED);
			cg_clr(cg, CCTF_REM_X_UNBLOCK_PENDING);
		}
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		if (cg_get_m_state(cg) == CMS_WRES_UBIND)
			cg_set_m_state(cg, cc, CMS_IDLE);
		return (QR_DONE);
	} else {
		if (p->cc_flags & ISUP_HARDWARE_FAILURE_ORIENTED)
			goto badflag;
		if (!(ct = isup_find_cct(cc, add_ptr, add_len)) && !(ct = cc->conn.cpc))
			goto badaddr;
		/* 
		   complete procedure */
		if (ct_tst(ct, CCTF_REM_M_UNBLOCK_PENDING)) {
			if ((err = isup_send_uba(q, ct)))
				return (err);
			ct_clr(ct, CCTF_REM_M_BLOCKED);
			ct_clr(ct, CCTF_REM_M_UNBLOCK_PENDING);
		}
		if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
			return (err);
		if (ct_get_m_state(ct) == CMS_WRES_UBIND)
			ct_set_m_state(ct, cc, CMS_IDLE);
		return (QR_DONE);
	}
	return (QR_DONE);
      badflag:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADFLAG);
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_QUERY_REQ:
 *  ------------------------------------
 *  To initiate the circuit group query procedure, the sending exchange sends a circuit group query message
 *  indicating in the routing label and range field those circuits to tbe audited.  If no response to the circuit
 *  group query message is received before timer T28 expires, maintenance systems should be informed.
 */
STATIC int
cc_query_req(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	struct cg *cg;
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_query_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_UNBND:
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (p->cc_flags & ISUP_GROUP) {
		if (!(cg = isup_find_grp(cc, add_ptr, add_len)) && (!(cg = cc->conn.gmg)))
			goto badaddr;
		/* 
		   need at least two circuits for a group action */
		if (!cg->ct.list || !cg->ct.list->cg.next)
			goto badaddr;
		if (cg_get_m_state(cg) != CMS_IDLE)
			goto outstate;
		if ((err = cg_query(q, cg)))
			return (err);
		cg_set_m_state(cg, cc, CMS_WCON_QRYREQ);
		return (QR_DONE);
	} else {
		goto badflag;
	}
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      badflag:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADFLAG);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_QUERY_RES:
 *  ------------------------------------
 *  The receiving exchange will process the circuit group query message, and return a circuit group query response
 *  message setting the circuit state indicators to the state of the circuits being audited.
 *
 *  If this circuit group procedure uncovers discrepancies in the state of a circuit as perceived at the two ends,
 *  the action to be taken in order to align the two views is for further study.
 */
STATIC int
cc_query_res(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct ct *ct = NULL;
	struct cg *cg;
	int err;
	uchar *add_ptr;
	size_t add_len;
	struct CC_query_res *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	add_ptr = mp->b_rptr + p->cc_addr_offset;
	add_len = p->cc_addr_length;
	if (mp->b_wptr < add_ptr + add_len)
		goto emsgsize;
	switch (cs_get_state(cc)) {
	case CCS_UNBND:
	case CCS_IDLE:
	case CCS_WRES_SIND:
		break;
	default:
		goto outstate;
	}
	if (!(p->cc_flags & ISUP_GROUP))
		goto badflag;
	if (!(cg = isup_find_grp(cc, add_ptr, add_len)) && (!(cg = cc->conn.gmg)))
		goto badaddr;
	/* 
	   complete procedure */
	if (cg_tst(cg, CCTF_REM_QUERY_PENDING)) {
		struct ct *bc = NULL;
		int j;
		uchar rs[RS_MAX_RANGE] = { 0, };
		uchar csi[RS_MAX_RANGE] = { 0, };
		for (j = 0; j < RS_MAX_RANGE; j++)
			csi[j] = 0x3;	/* unequipped */
		for (ct = cg->ct.list; ct; ct = ct->cg.next) {
			if (ct_tst(ct, CCTF_REM_QUERY_PENDING)) {
				bc = bc ? bc : ct;
				j = ct->cic - bc->cic;
				if (rs[0] < j)
					rs[0] = j;
				csi[j] = 0;
				csi[j] |= 0x0;	/* transient */
				switch (ct_tst(ct, (CCTF_LOC_M_BLOCKED | CCTF_REM_M_BLOCKED))) {
				case 0:
					csi[j] |= 0x0;
					break;
				case CCTF_LOC_M_BLOCKED:
					csi[j] |= 0x1;
					break;
				case CCTF_REM_M_BLOCKED:
					csi[j] |= 0x2;
					break;
				case CCTF_LOC_M_BLOCKED | CCTF_REM_M_BLOCKED:
					csi[j] |= 0x3;
					break;
				}
				switch (ct_get_c_state(ct)) {
				case CTS_ICC_WAIT_SAM:
				case CTS_ICC_WAIT_ACM:
				case CTS_ICC_WAIT_ANM:
				case CTS_ICC_ANSWERED:
				case CTS_ICC_SUSPENDED:
				case CTS_ICC_WAIT_RLC:
				case CTS_ICC_SEND_RLC:
					csi[j] |= 0x04;
					break;
				case CTS_OGC_WAIT_SAM:
				case CTS_OGC_WAIT_ACM:
				case CTS_OGC_WAIT_ANM:
				case CTS_OGC_ANSWERED:
				case CTS_OGC_SUSPENDED:
				case CTS_OGC_WAIT_RLC:
				case CTS_OGC_SEND_RLC:
					csi[j] |= 0x08;
					break;
				case CTS_IDLE:
					csi[j] |= 0x0c;
					break;
				}
				switch (ct_tst(ct, (CCTF_LOC_H_BLOCKED | CCTF_REM_H_BLOCKED))) {
				case 0:
					csi[j] |= 0x00;
					break;
				case CCTF_LOC_H_BLOCKED:
					csi[j] |= 0x10;
					break;
				case CCTF_REM_H_BLOCKED:
					csi[j] |= 0x20;
					break;
				case CCTF_LOC_H_BLOCKED | CCTF_REM_H_BLOCKED:
					csi[j] |= 0x30;
					break;
				}
			}
		}
		if ((err = isup_send_cqr(q, bc, rs, 1, csi, rs[0])))
			return (err);
		for (ct = bc; ct; ct = ct->cg.next)
			ct_clr(ct, CCTF_REM_QUERY_PENDING);
		cg_clr(cg, CCTF_REM_QUERY_PENDING);
	}
	if ((err = cc_ok_ack(q, cc, ct, p->cc_primitive)))
		return (err);
	if (cg_get_m_state(cg) == CMS_WRES_QRYIND)
		cg_set_m_state(cg, cc, CMS_IDLE);
	return (QR_DONE);
      badflag:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADFLAG);
      badaddr:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCBADADDR);
      outstate:
	return cc_error_ack(q, cc, ct, p->cc_primitive, CCOUTSTATE);
      emsgsize:
	return cc_error_ack(q, cc, ct, p->cc_primitive, -EMSGSIZE);
}

/*
 *  CC_ Unsupported
 *  -----------------------------------
 *  This is for known but unsupported primitives.
 */
STATIC int
cc_unsupported_prim(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	ulong prim = *((ulong *) mp->b_rptr);
	return cc_error_ack(q, cc, NULL, prim, CCNOTSUPP);
}

/*
 *  CC_ Unknown
 *  -----------------------------------
 *  This is for completely unknown primitives.
 */
STATIC int
cc_unknown_prim(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	ulong prim = *((ulong *) mp->b_rptr);
	return cc_error_ack(q, cc, NULL, prim, CCBADPRIM);
}

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 *
 *  ISUP IO Controls
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
isup_opt_get_ct(isup_option_t * arg, struct ct *ct, int size)
{
	isup_opt_conf_ct_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!ct)
		return (-EINVAL);
	*opt = ct->config;
	return (QR_DONE);
}
STATIC int
isup_opt_get_cg(isup_option_t * arg, struct cg *cg, int size)
{
	isup_opt_conf_cg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!cg)
		return (-EINVAL);
	*opt = cg->config;
	return (QR_DONE);
}
STATIC int
isup_opt_get_tg(isup_option_t * arg, struct tg *tg, int size)
{
	isup_opt_conf_tg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!tg)
		return (-EINVAL);
	*opt = tg->config;
	return (QR_DONE);
}
STATIC int
isup_opt_get_sr(isup_option_t * arg, struct sr *sr, int size)
{
	isup_opt_conf_sr_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sr)
		return (-EINVAL);
	*opt = sr->config;
	return (QR_DONE);
}
STATIC int
isup_opt_get_sp(isup_option_t * arg, struct sp *sp, int size)
{
	isup_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sp)
		return (-EINVAL);
	*opt = sp->config;
	return (QR_DONE);
}
STATIC int
isup_opt_get_mtp(isup_option_t * arg, struct mtp *mtp, int size)
{
	isup_opt_conf_mtp_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!mtp)
		return (-EINVAL);
	*opt = mtp->config;
	return (QR_DONE);
}
STATIC int
isup_opt_get_df(isup_option_t * arg, struct df *df, int size)
{
	isup_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
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
isup_opt_set_ct(isup_option_t * arg, struct ct *ct, int size)
{
	isup_opt_conf_ct_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!ct)
		return (-EINVAL);
	todo(("Check circuit options before setting them\n"));
	ct->config = *opt;
	return (QR_DONE);
}
STATIC int
isup_opt_set_cg(isup_option_t * arg, struct cg *cg, int size)
{
	isup_opt_conf_cg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!cg)
		return (-EINVAL);
	todo(("Check circuit options before setting them\n"));
	cg->config = *opt;
	return (QR_DONE);
}
STATIC int
isup_opt_set_tg(isup_option_t * arg, struct tg *tg, int size)
{
	isup_opt_conf_tg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!tg)
		return (-EINVAL);
	todo(("Check circuit options before setting them\n"));
	tg->config = *opt;
	return (QR_DONE);
}
STATIC int
isup_opt_set_sr(isup_option_t * arg, struct sr *sr, int size)
{
	isup_opt_conf_sr_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sr)
		return (-EINVAL);
	todo(("Check circuit options before setting them\n"));
	sr->config = *opt;
	return (QR_DONE);
}
STATIC int
isup_opt_set_sp(isup_option_t * arg, struct sp *sp, int size)
{
	isup_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sp)
		return (-EINVAL);
	todo(("Check circuit options before setting them\n"));
	sp->config = *opt;
	return (QR_DONE);
}
STATIC int
isup_opt_set_mtp(isup_option_t * arg, struct mtp *mtp, int size)
{
	isup_opt_conf_mtp_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!mtp)
		return (-EINVAL);
	todo(("Check circuit options before setting them\n"));
	mtp->config = *opt;
	return (QR_DONE);
}
STATIC int
isup_opt_set_df(isup_option_t * arg, struct df *df, int size)
{
	isup_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!df)
		return (-EINVAL);
	todo(("Check circuit options before setting them\n"));
	df->config = *opt;
	return (QR_DONE);
}

/*
 *  GET Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isup_get_ct(isup_config_t * arg, struct ct *ct, int size)
{
	isup_conf_ct_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!ct || size < sizeof(*arg))
		return (-EINVAL);
	cnf->cgid = ct->cg.cg ? ct->cg.cg->id : 0;
	cnf->tgid = ct->tg.tg ? ct->tg.tg->id : 0;
	cnf->cic = ct->cic;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isup_get_cg(isup_config_t * arg, struct cg *cg, int size)
{
	struct ct *ct;
	isup_conf_ct_t *chd;
	isup_conf_cg_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!cg)
		return (-EINVAL);
	cnf->srid = cg->sr.sr ? cg->sr.sr->id : 0;
	/* 
	   write out list of circuit configurations following trunk group configurations */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (ct = cg->ct.list; ct && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     ct = ct->cg.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = ISUP_OBJ_TYPE_CT;
		arg->id = ct->id;
		chd->cgid = ct->cg.cg ? ct->cg.cg->id : 0;
		chd->tgid = ct->tg.tg ? ct->tg.tg->id : 0;
		chd->cic = ct->cic;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isup_get_tg(isup_config_t * arg, struct tg *tg, int size)
{
	struct ct *ct;
	isup_conf_ct_t *chd;
	isup_conf_tg_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!tg)
		return (-EINVAL);
	cnf->srid = tg->sr.sr ? tg->sr.sr->id : 0;
	cnf->proto = tg->proto;
	/* 
	   write out list of circuit configurations following trunk group configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (ct = tg->ct.list; ct && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     ct = ct->tg.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = ISUP_OBJ_TYPE_CT;
		arg->id = ct->id;
		chd->cgid = ct->cg.cg ? ct->cg.cg->id : 0;
		chd->tgid = ct->tg.tg ? ct->tg.tg->id : 0;
		chd->cic = ct->cic;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isup_get_sr(isup_config_t * arg, struct sr *sr, int size)
{
	struct cg *cg;
	struct tg *tg;
	isup_conf_cg_t *chc;
	isup_conf_tg_t *cht;
	isup_conf_sr_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!sr)
		return (-EINVAL);
	cnf->spid = sr->sp.sp ? sr->sp.sp->id : 0;
	cnf->proto = sr->proto;
	cnf->add = sr->add;
	/* 
	   write out the list of circuit group and trunk group configurations following remote
	   signalling point configuration */
	arg = (typeof(arg)) (cnf + 1);
	chc = (typeof(chc)) (arg + 1);
	/* 
	   circuit groups */
	for (cg = sr->cg.list; cg && size >= sizeof(*arg) + sizeof(*chc) + sizeof(*arg);
	     cg = cg->sr.next, size -= sizeof(*arg) + sizeof(*chc), arg =
	     (typeof(arg)) (chc + 1), chc = (typeof(chc)) (arg + 1)) {
		arg->type = ISUP_OBJ_TYPE_CG;
		arg->id = cg->id;
		chc->srid = cg->sr.sr ? cg->sr.sr->id : 0;
	}
	cht = (typeof(cht)) chc;
	/* 
	   trunk groups */
	for (tg = sr->tg.list; tg && size >= sizeof(*arg) + sizeof(*cht) + sizeof(*arg);
	     tg = tg->sr.next, size -= sizeof(*arg) + sizeof(*cht), arg =
	     (typeof(arg)) (cht + 1), cht = (typeof(cht)) (arg + 1)) {
		arg->type = ISUP_OBJ_TYPE_SP;
		arg->id = tg->id;
		cht->srid = tg->sr.sr ? tg->sr.sr->id : 0;
		cht->proto = tg->proto;
	}
	todo(("Should also print associated message transfer parts\n"));
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isup_get_sp(isup_config_t * arg, struct sp *sp, int size)
{
	struct sr *sr;
	isup_conf_sr_t *chd;
	isup_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!sp)
		return (-EINVAL);
	cnf->proto = sp->proto;
	cnf->add = sp->add;
	/* 
	   write out the list of remote signalling point configurations following local signalling
	   point configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (sr = sp->sr.list; sr && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sr = sr->sp.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = ISUP_OBJ_TYPE_SR;
		arg->id = sr->id;
		chd->spid = sr->sp.sp ? sr->sp.sp->id : 0;
		chd->proto = sr->proto;
		chd->add = sr->add;
	}
	todo(("Should also print associated message transfer parts\n"));
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isup_get_mtp(isup_config_t * arg, struct mtp *mtp, int size)
{
	struct sr *sr;
	struct sp *sp;
	isup_conf_sr_t *chr;
	isup_conf_sp_t *chp;
	isup_conf_mtp_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!mtp)
		return (-EINVAL);
	cnf->proto = mtp->proto;
	/* 
	   write out remote signalling point or local signalling point configuration following
	   message transfer part configuration */
	arg = (typeof(arg)) (cnf + 1);
	chr = (typeof(chr)) (arg + 1);
	/* 
	   remote signalling point */
	for (sr = mtp->sr; sr && size >= sizeof(*arg) + sizeof(*chr) + sizeof(*arg);
	     sr = NULL, size -= sizeof(*arg) + sizeof(*chr), arg = (typeof(arg)) (chr + 1), chr =
	     (typeof(chr)) (arg + 1)) {
		arg->type = ISUP_OBJ_TYPE_SR;
		arg->id = sr->id;
		chr->spid = sr->sp.sp ? sr->sp.sp->id : 0;
		chr->proto = sr->proto;
		chr->add = sr->add;
	}
	chp = (typeof(chp)) chr;
	/* 
	   local signalling point */
	for (sp = mtp->sp; sp && size >= sizeof(*arg) + sizeof(*chp) + sizeof(*arg);
	     sp = NULL, size -= sizeof(*arg) + sizeof(*chp), arg = (typeof(arg)) (chp + 1), chp =
	     (typeof(chp)) (arg + 1)) {
		arg->type = ISUP_OBJ_TYPE_SP;
		arg->id = sp->id;
		chp->proto = sp->proto;
		chp->add = sp->add;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
isup_get_df(isup_config_t * arg, struct df *df, int size)
{
	struct sp *sp;
	isup_conf_sp_t *chd;
	isup_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!df)
		return (-EINVAL);
	cnf->proto = df->proto;
	/* 
	   write out the list of SP configurations following default configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (sp = df->sp.list; sp && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sp = sp->next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = ISUP_OBJ_TYPE_SP;
		arg->id = sp->id;
		chd->proto = sp->proto;
		chd->add = sp->add;
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
isup_add_ct(isup_config_t * arg, struct ct *ct, int size, int force, int test)
{
	struct tg *tg = NULL;
	struct cg *cg = NULL;
	isup_conf_ct_t *cnf = (typeof(cnf)) (arg + 1);
	if (ct || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->tgid)
		tg = tg_lookup(cnf->tgid);
	if (cnf->cgid)
		cg = cg_lookup(cnf->cgid);
	if (!tg || !cg)
		return (-EINVAL);
	for (ct = tg->ct.list; ct; ct = ct->tg.next)
		if (ct->cic == cnf->cic)
			return (-EINVAL);
	for (ct = cg->ct.list; ct; ct = ct->cg.next)
		if (ct->cic == cnf->cic)
			return (-EINVAL);
	/* 
	   TODO:- if the circuit group selected is not a pre-determined circuit group, we will have 
	   to check to make sure that the CIC of the circuit being added does not exceed the range
	   of the group.  We should also add the circuit to the new range and status parameter. */
	if (!test) {
		if (!(ct = isup_alloc_ct(ct_get_id(arg->id), tg, cg, cnf->cic)))
			return (-ENOMEM);
		arg->id = ct->id;
	}
	return (QR_DONE);
}
STATIC int
isup_add_cg(isup_config_t * arg, struct cg *cg, int size, int force, int test)
{
	struct sr *sr = NULL;
	isup_conf_cg_t *cnf = (typeof(cnf)) (arg + 1);
	if (cg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->srid)
		sr = sr_lookup(cnf->srid);
	if (!sr)
		return (-EINVAL);
	if (!test) {
		if (!(cg = isup_alloc_cg(cg_get_id(arg->id), sr)))
			return (-ENOMEM);
		cg->proto = cnf->proto;
		arg->id = cg->id;
	}
	return (QR_DONE);
}
STATIC int
isup_add_tg(isup_config_t * arg, struct tg *tg, int size, int force, int test)
{
	struct sr *sr = NULL;
	isup_conf_tg_t *cnf = (typeof(cnf)) (arg + 1);
	if (tg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->srid)
		sr = sr_lookup(cnf->srid);
	if (!sr)
		return (-EINVAL);
	if (!test) {
		if (!(tg = isup_alloc_tg(tg_get_id(arg->id), sr)))
			return (-ENOMEM);
		tg->proto = cnf->proto;
		arg->id = tg->id;
	}
	return (QR_DONE);
}
STATIC int
isup_add_sr(isup_config_t * arg, struct sr *sr, int size, int force, int test)
{
	struct sp *sp = NULL;
	isup_conf_sr_t *cnf = (typeof(cnf)) (arg + 1);
	if (sr || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (!sp)
		return (-EINVAL);
	/* 
	   network appearance of remote must match network appearance of local */
	if (sp->add.ni != cnf->add.ni)
		return (-EINVAL);
	for (sr = sp->sr.list; sr; sr = sr->sp.next)
		if (sr->add.pc == cnf->add.pc && sr->add.si == cnf->add.si)
			return (-EINVAL);
	if (!force) {
		todo(("Check if remote switch is active first\n"));
	}
	if (!test) {
		if (!(sr = isup_alloc_sr(sr_get_id(arg->id), sp, &cnf->add)))
			return (-ENOMEM);
		sr->proto = cnf->proto;
		arg->id = sr->id;
	}
	return (QR_DONE);
}
STATIC int
isup_add_sp(isup_config_t * arg, struct sp *sp, int size, int force, int test)
{
	isup_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);
	if (sp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	for (sp = master.sp.list; sp; sp = sp->next)
		if (sp->add.ni == cnf->add.ni && sp->add.pc == cnf->add.pc
		    && sp->add.si == cnf->add.si)
			return (-EBUSY);
	if (!force) {
		todo(("Check if local switch is active first\n"));
	}
	if (!test) {
		if (!(sp = isup_alloc_sp(sp_get_id(arg->id), &cnf->add)))
			return (-ENOMEM);
		sp->proto = cnf->proto;
		arg->id = sp->id;
	}
	return (QR_DONE);
}
STATIC int
isup_add_mtp(isup_config_t * arg, struct mtp *mtp, int size, int force, int test)
{
	struct sp *sp = NULL;
	struct sr *sr = NULL;
	isup_conf_mtp_t *cnf = (typeof(cnf)) (arg + 1);
	if (mtp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (cnf->srid)
		sr = sr_lookup(cnf->srid);
	if (!sp || (!sr && cnf->srid))
		return (-EINVAL);
	if (sp->mtp || (sr && sr->mtp))
		return (-EINVAL);
	for (mtp = master.mtp.list; mtp; mtp = mtp->next)
		if (mtp->u.mux.index == cnf->muxid) {
			if (mtp->id)
				return (-EINVAL);
			break;
		}
	if (!mtp)
		return (-EINVAL);
	if (!force) {
		todo(("Check if mtp is active first\n"));
	}
	if (!test) {
		mtp->id = mtp_get_id(arg->id);
		if (sr) {
			mtp->sr = sr_get(sr);
			sr->mtp = mtp_get(mtp);
		} else {
			mtp->sp = sp_get(sp);
			sp->mtp = mtp_get(mtp);
		}
		mtp->proto = cnf->proto;
		arg->id = mtp->id;
	}
	return (QR_DONE);
}
STATIC int
isup_add_df(isup_config_t * arg, struct df *df, int size, int force, int test)
{
	isup_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if (df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	return (-EINVAL);
}

/*
 *  CHA Object Configuration
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isup_cha_ct(isup_config_t * arg, struct ct *ct, int size, int force, int test)
{
	struct ct *c;
	isup_conf_ct_t *cnf = (typeof(cnf)) (arg + 1);
	if (!ct || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->cgid && cnf->cgid != ct->cg.cg->id)
		return (-EINVAL);
	if (cnf->tgid && cnf->tgid != ct->tg.tg->id)
		return (-EINVAL);
	/* 
	   can't change to existing cic */
	for (c = ct->sr.sr->ct.list; c; c = c->sr.next)
		if (c->cic == cnf->cic)
			return (-EINVAL);
	if (!force) {
		/* 
		   bound for maintenance or call control */
		if (ct->bind.tst || ct->bind.mgm || ct->bind.icc || ct->bind.ogc)
			return (-EBUSY);
		/* 
		   involved in call or local maintenance */
		if (ct->cpc.cc || ct->tst.cc || ct->mgm.cc)
			return (-EBUSY);
		/* 
		   involved in processing with remote switch */
		if (ct_get_c_state(ct) != CTS_IDLE)
			return (-EBUSY);
		/* 
		   involved in processing with call control interface */
		if (ct_get_i_state(ct) != CCS_IDLE)
			return (-EBUSY);
		/* 
		   involved in processing with local maintenance */
		if (ct_get_m_state(ct) != CMS_IDLE)
			return (-EBUSY);
	}
	/* 
	   TODO:- if the circuit group selected is not a pre-determined circuit group, we will have 
	   to check to make sure that the CIC of the circuit being changed does not exceed the
	   range of the group.  We should also change the circuit in the range and status
	   parameter. */
	if (!test) {
		ct->cic = cnf->cic;
	}
	return (QR_DONE);
}
STATIC int
isup_cha_cg(isup_config_t * arg, struct cg *cg, int size, int force, int test)
{
	isup_conf_cg_t *cnf = (typeof(cnf)) (arg + 1);
	if (!cg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->srid && cnf->srid != cg->sr.sr->id)
		return (-EINVAL);
	/* 
	   TODO:- if we are changing the options from preassigned to non-preassigned, then we
	   should check to make sure that the resulting range and status is not outside of the
	   permitted range. */
	if (!force) {
		/* 
		   we have circuits */
		if (cg->ct.list)
			return (-EBUSY);
		/* 
		   bound for maintenance or call control */
		if (cg->bind.tst || cg->bind.mgm || cg->bind.icc || cg->bind.ogc)
			return (-EBUSY);
		/* 
		   involved in local maintenance */
		if (cg->gmg.cc)
			return (-EBUSY);
		/* 
		   involved in processing with local maintenance */
		if (cg_get_m_state(cg) != CMS_IDLE)
			return (-EBUSY);
	}
	if (!test) {
		cg->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isup_cha_tg(isup_config_t * arg, struct tg *tg, int size, int force, int test)
{
	isup_conf_tg_t *cnf = (typeof(cnf)) (arg + 1);
	if (!tg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->srid && cnf->srid != tg->sr.sr->id)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have circuits */
		if (tg->ct.list)
			return (-EBUSY);
		/* 
		   bound to a stream for call control or management */
		if (tg->bind.tst || tg->bind.mgm || tg->bind.icc || tg->bind.ogc)
			return (-EBUSY);
	}
	if (!test) {
		tg->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isup_cha_sr(isup_config_t * arg, struct sr *sr, int size, int force, int test)
{
	struct sr *s;
	isup_conf_sr_t *cnf = (typeof(cnf)) (arg + 1);
	if (!sr || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid && cnf->spid != sr->sp.sp->id)
		return (-EINVAL);
	/* 
	   can't change network appearance */
	if (cnf->add.ni != sr->add.ni)
		return (-EINVAL);
	/* 
	   cannot change address to an existing remote address */
	for (s = master.sr.list; s; s = s->next)
		if (s->add.ni == cnf->add.ni && s->add.pc == cnf->add.pc
		    && s->add.si == cnf->add.si)
			return (-EINVAL);
	if (!force) {
		/* 
		   bound to a call control or maangement stream */
		if (sr->bind.tst || sr->bind.mgm || sr->bind.icc || sr->bind.ogc)
			return (-EBUSY);
		/* 
		   we are attached to a live MTP lower stream */
		if (sr->mtp)
			return (-EBUSY);
		/* 
		   we have circuit groups and trunk groups */
		if (sr->cg.list || sr->tg.list)
			return (-EBUSY);
	}
	if (!test) {
		sr->add = cnf->add;
		sr->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isup_cha_sp(isup_config_t * arg, struct sp *sp, int size, int force, int test)
{
	struct sp *s;
	isup_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);
	if (!sp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   can't change network appearance */
	if (cnf->add.ni != sp->add.ni)
		return (-EINVAL);
	/* 
	   can't change to an existing local */
	for (s = master.sp.list; s; s = s->next)
		if (s->add.ni == cnf->add.ni && s->add.pc == cnf->add.pc
		    && s->add.si == cnf->add.si)
			return (-EINVAL);
	if (!force) {
		/* 
		   bound to call control or management */
		if (sp->bind.tst || sp->bind.mgm || sp->bind.icc || sp->bind.ogc)
			return (-EBUSY);
		/* 
		   attached to live MTP interface */
		if (sp->mtp)
			return (-EBUSY);
		/* 
		   we have remote signalling points associated */
		if (sp->sr.list)
			return (-EBUSY);
	}
	if (!test) {
		sp->add = cnf->add;
		sp->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isup_cha_mtp(isup_config_t * arg, struct mtp *mtp, int size, int force, int test)
{
	isup_conf_mtp_t *cnf = (typeof(cnf)) (arg + 1);
	if (!mtp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid && (!mtp->sp || cnf->spid != mtp->sp->id))
		return (-EINVAL);
	if (cnf->srid && (!mtp->sr || cnf->srid != mtp->sr->id))
		return (-EINVAL);
	if (cnf->muxid && cnf->muxid != mtp->u.mux.index)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal datastructures */
		if (mtp->sp || mtp->sr)
			return (-EBUSY);
	}
	if (!test) {
		mtp->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
isup_cha_df(isup_config_t * arg, struct df *df, int size, int force, int test)
{
	isup_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
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
isup_del_ct(isup_config_t * arg, struct ct *ct, int size, int force, int test)
{
	if (!ct)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound for maintenance or call control */
		if (ct->bind.tst || ct->bind.mgm || ct->bind.icc || ct->bind.ogc)
			return (-EBUSY);
		/* 
		   involved in call or local maintenance */
		if (ct->cpc.cc || ct->tst.cc || ct->mgm.cc)
			return (-EBUSY);
		/* 
		   involved in processing with remote switch */
		if (ct_get_c_state(ct) != CTS_IDLE)
			return (-EBUSY);
		/* 
		   involved in processing with call control interface */
		if (ct_get_i_state(ct) != CCS_IDLE)
			return (-EBUSY);
		/* 
		   involved in processing with local maintenance */
		if (ct_get_m_state(ct) != CMS_IDLE)
			return (-EBUSY);
	}
	if (!test) {
		isup_free_ct(ct);
	}
	return (QR_DONE);
}
STATIC int
isup_del_cg(isup_config_t * arg, struct cg *cg, int size, int force, int test)
{
	if (!cg)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have circuits */
		if (cg->ct.list)
			return (-EBUSY);
		/* 
		   bound for maintenance or call control */
		if (cg->bind.tst || cg->bind.mgm || cg->bind.icc || cg->bind.ogc)
			return (-EBUSY);
		/* 
		   involved in local maintenance */
		if (cg->gmg.cc)
			return (-EBUSY);
		/* 
		   involved in processing with local maintenance */
		if (cg_get_m_state(cg) != CMS_IDLE)
			return (-EBUSY);
	}
	if (!test) {
		isup_free_cg(cg);
	}
	return (QR_DONE);
}
STATIC int
isup_del_tg(isup_config_t * arg, struct tg *tg, int size, int force, int test)
{
	if (!tg)
		return (-EINVAL);
	if (!force) {
		/* 
		   we have circuits */
		if (tg->ct.list)
			return (-EBUSY);
		/* 
		   bound to a stream for call control or management */
		if (tg->bind.tst || tg->bind.mgm || tg->bind.icc || tg->bind.ogc)
			return (-EBUSY);
	}
	if (!test) {
		isup_free_tg(tg);
	}
	return (QR_DONE);
}
STATIC int
isup_del_sr(isup_config_t * arg, struct sr *sr, int size, int force, int test)
{
	if (!sr)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to a call control or maangement stream */
		if (sr->bind.tst || sr->bind.mgm || sr->bind.icc || sr->bind.ogc)
			return (-EBUSY);
		/* 
		   we are attached to a live MTP lower stream */
		if (sr->mtp)
			return (-EBUSY);
		/* 
		   we have circuit groups and trunk groups */
		if (sr->cg.list || sr->tg.list)
			return (-EBUSY);
	}
	if (!test) {
		isup_free_sr(sr);
	}
	return (QR_DONE);
}
STATIC int
isup_del_sp(isup_config_t * arg, struct sp *sp, int size, int force, int test)
{
	if (!sp)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to call control or management */
		if (sp->bind.tst || sp->bind.mgm || sp->bind.icc || sp->bind.ogc)
			return (-EBUSY);
		/* 
		   attached to live MTP interface */
		if (sp->mtp)
			return (-EBUSY);
		/* 
		   we have remote signalling points associated */
		if (sp->sr.list)
			return (-EBUSY);
	}
	if (!test) {
		isup_free_sp(sp);
	}
	return (QR_DONE);
}
STATIC int
isup_del_mtp(isup_config_t * arg, struct mtp *mtp, int size, int force, int test)
{
	if (!mtp)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal datastructures */
		if (mtp->sp || mtp->sr)
			return (-EBUSY);
	}
	if (!test) {
		// isup_free_mtp(mtp);
	}
	return (QR_DONE);
}
STATIC int
isup_del_df(isup_config_t * arg, struct df *df, int size, int force, int test)
{
	isup_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if (!df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		// isup_free_df(df);
	}
	return (QR_DONE);
}

/*
 *  GET State
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isup_sta_ct(isup_statem_t * arg, struct ct *ct, int size)
{
	struct cc *cc;
	ulong *p;
	isup_statem_ct_t *sta = (typeof(sta)) (arg + 1);
	if (!ct || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = ct->flags;
	arg->state = ct->state;
	sta->timers = ct->timers;
	sta->tst_actv = ct->tst.cc ? ct->tst.cc->id : 0;
	sta->mgm_actv = ct->mgm.cc ? ct->mgm.cc->id : 0;
	sta->cpc_actv = ct->cpc.cc ? ct->cpc.cc->id : 0;
	sta->tst_bind = ct->bind.tst ? ct->bind.tst->id : 0;
	sta->mgm_bind = ct->bind.mgm ? ct->bind.mgm->id : 0;
	sta->mnt_bind = ct->bind.mnt ? ct->bind.mnt->id : 0;
	sta->icc_bind = ct->bind.icc ? ct->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = ct->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isup_sta_cg(isup_statem_t * arg, struct cg *cg, int size)
{
	struct cc *cc;
	ulong *p;
	isup_statem_cg_t *sta = (typeof(sta)) (arg + 1);
	if (!cg || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = cg->flags;
	arg->state = cg->state;
	sta->timers = cg->timers;
	sta->gmg_actv = cg->gmg.cc ? cg->gmg.cc->id : 0;
	sta->tst_bind = cg->bind.tst ? cg->bind.tst->id : 0;
	sta->mgm_bind = cg->bind.mgm ? cg->bind.mgm->id : 0;
	sta->mnt_bind = cg->bind.mnt ? cg->bind.mnt->id : 0;
	sta->icc_bind = cg->bind.icc ? cg->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = cg->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isup_sta_tg(isup_statem_t * arg, struct tg *tg, int size)
{
	struct cc *cc;
	ulong *p;
	isup_statem_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = tg->flags;
	arg->state = tg->state;
	sta->timers = tg->timers;
	sta->tst_bind = tg->bind.tst ? tg->bind.tst->id : 0;
	sta->mgm_bind = tg->bind.mgm ? tg->bind.mgm->id : 0;
	sta->mnt_bind = tg->bind.mnt ? tg->bind.mnt->id : 0;
	sta->icc_bind = tg->bind.icc ? tg->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = tg->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isup_sta_sr(isup_statem_t * arg, struct sr *sr, int size)
{
	struct cc *cc;
	ulong *p;
	isup_statem_sr_t *sta = (typeof(sta)) (arg + 1);
	if (!sr || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = sr->flags;
	arg->state = sr->state;
	sta->timers = sr->timers;
	sta->cong_level = sr->cong_level;
	sta->tst_bind = sr->bind.tst ? sr->bind.tst->id : 0;
	sta->mgm_bind = sr->bind.mgm ? sr->bind.mgm->id : 0;
	sta->mnt_bind = sr->bind.mnt ? sr->bind.mnt->id : 0;
	sta->icc_bind = sr->bind.icc ? sr->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = sr->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isup_sta_sp(isup_statem_t * arg, struct sp *sp, int size)
{
	struct cc *cc;
	ulong *p;
	isup_statem_sp_t *sta = (typeof(sta)) (arg + 1);
	if (!sp || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = sp->flags;
	arg->state = sp->state;
	sta->timers = sp->timers;
	sta->tst_bind = sp->bind.tst ? sp->bind.tst->id : 0;
	sta->mgm_bind = sp->bind.mgm ? sp->bind.mgm->id : 0;
	sta->mnt_bind = sp->bind.mnt ? sp->bind.mnt->id : 0;
	sta->icc_bind = sp->bind.icc ? sp->bind.icc->id : 0;
	/* 
	   fit in a many as we can */
	for (cc = sp->bind.ogc, p = sta->ogc_bind; cc && size >= 2 * sizeof(ulong);
	     cc = cc->bind.next, size -= sizeof(ulong), *p++ = cc->id) ;
	*p++ = 0;
	return (QR_DONE);
}
STATIC int
isup_sta_mtp(isup_statem_t * arg, struct mtp *mtp, int size)
{
	isup_statem_mtp_t *sta = (typeof(sta)) (arg + 1);
	if (!mtp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	arg->flags = mtp->flags;
	arg->state = mtp->state;
	sta->timers = mtp->timers;
	return (QR_DONE);
}
STATIC int
isup_sta_df(isup_statem_t * arg, struct df *df, int size)
{
	struct cc *cc;
	ulong *p;
	isup_statem_df_t *sta = (typeof(sta)) (arg + 1);
	if (!df || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = 0;
	arg->state = 0;
	sta->tst_bind = df->bind.tst ? df->bind.tst->id : 0;
	sta->mgm_bind = df->bind.mgm ? df->bind.mgm->id : 0;
	sta->mnt_bind = df->bind.mnt ? df->bind.mnt->id : 0;
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
isup_not_get_ct(isup_notify_t * arg, struct ct *ct)
{
	if (!ct)
		return (-EINVAL);
	arg->notify.events = ct->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_get_cg(isup_notify_t * arg, struct cg *cg)
{
	if (!cg)
		return (-EINVAL);
	arg->notify.events = cg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_get_tg(isup_notify_t * arg, struct tg *tg)
{
	if (!tg)
		return (-EINVAL);
	arg->notify.events = tg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_get_sr(isup_notify_t * arg, struct sr *sr)
{
	if (!sr)
		return (-EINVAL);
	arg->notify.events = sr->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_get_sp(isup_notify_t * arg, struct sp *sp)
{
	if (!sp)
		return (-EINVAL);
	arg->notify.events = sp->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_get_mtp(isup_notify_t * arg, struct mtp *mtp)
{
	if (!mtp)
		return (-EINVAL);
	arg->notify.events = mtp->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_get_df(isup_notify_t * arg, struct df *df)
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
isup_not_set_ct(isup_notify_t * arg, struct ct *ct)
{
	if (!ct)
		return (-EINVAL);
	ct->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_set_cg(isup_notify_t * arg, struct cg *cg)
{
	if (!cg)
		return (-EINVAL);
	cg->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_set_tg(isup_notify_t * arg, struct tg *tg)
{
	if (!tg)
		return (-EINVAL);
	tg->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_set_sr(isup_notify_t * arg, struct sr *sr)
{
	if (!sr)
		return (-EINVAL);
	sr->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_set_sp(isup_notify_t * arg, struct sp *sp)
{
	if (!sp)
		return (-EINVAL);
	sp->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_set_mtp(isup_notify_t * arg, struct mtp *mtp)
{
	if (!mtp)
		return (-EINVAL);
	mtp->notify.events |= arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_set_df(isup_notify_t * arg, struct df *df)
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
isup_not_clr_ct(isup_notify_t * arg, struct ct *ct)
{
	if (!ct)
		return (-EINVAL);
	ct->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_clr_cg(isup_notify_t * arg, struct cg *cg)
{
	if (!cg)
		return (-EINVAL);
	cg->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_clr_tg(isup_notify_t * arg, struct tg *tg)
{
	if (!tg)
		return (-EINVAL);
	tg->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_clr_sr(isup_notify_t * arg, struct sr *sr)
{
	if (!sr)
		return (-EINVAL);
	sr->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_clr_sp(isup_notify_t * arg, struct sp *sp)
{
	if (!sp)
		return (-EINVAL);
	sp->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_clr_mtp(isup_notify_t * arg, struct mtp *mtp)
{
	if (!mtp)
		return (-EINVAL);
	mtp->notify.events &= ~arg->notify.events;
	return (QR_DONE);
}
STATIC int
isup_not_clr_df(isup_notify_t * arg, struct df *df)
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
isup_mgmt_blo_ct(isup_mgmt_t * arg, struct ct *ct)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_blo_cg(isup_mgmt_t * arg, struct cg *cg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_blo_tg(isup_mgmt_t * arg, struct tg *tg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_blo_sr(isup_mgmt_t * arg, struct sr *sr)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_blo_sp(isup_mgmt_t * arg, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_blo_mtp(isup_mgmt_t * arg, struct mtp *mtp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_blo_df(isup_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  UBL Managment
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isup_mgmt_ubl_ct(isup_mgmt_t * arg, struct ct *ct)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_ubl_cg(isup_mgmt_t * arg, struct cg *cg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_ubl_tg(isup_mgmt_t * arg, struct tg *tg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_ubl_sr(isup_mgmt_t * arg, struct sr *sr)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_ubl_sp(isup_mgmt_t * arg, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_ubl_mtp(isup_mgmt_t * arg, struct mtp *mtp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_ubl_df(isup_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  RES Managment
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isup_mgmt_res_ct(isup_mgmt_t * arg, struct ct *ct)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_res_cg(isup_mgmt_t * arg, struct cg *cg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_res_tg(isup_mgmt_t * arg, struct tg *tg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_res_sr(isup_mgmt_t * arg, struct sr *sr)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_res_sp(isup_mgmt_t * arg, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_res_mtp(isup_mgmt_t * arg, struct mtp *mtp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_res_df(isup_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  QRY Managment
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isup_mgmt_qry_ct(isup_mgmt_t * arg, struct ct *ct)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_qry_cg(isup_mgmt_t * arg, struct cg *cg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_qry_tg(isup_mgmt_t * arg, struct tg *tg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_qry_sr(isup_mgmt_t * arg, struct sr *sr)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_qry_sp(isup_mgmt_t * arg, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_qry_mtp(isup_mgmt_t * arg, struct mtp *mtp)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
isup_mgmt_qry_df(isup_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  GET Statistics Period
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
isup_statp_get_ct(isup_stats_t * arg, struct ct *ct, int size)
{
	isup_stats_ct_t *sta = (typeof(sta)) (arg + 1);
	if (!ct || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = ct->statsp;
	return (QR_DONE);
}
STATIC int
isup_statp_get_cg(isup_stats_t * arg, struct cg *cg, int size)
{
	isup_stats_cg_t *sta = (typeof(sta)) (arg + 1);
	if (!cg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = cg->statsp;
	return (QR_DONE);
}
STATIC int
isup_statp_get_tg(isup_stats_t * arg, struct tg *tg, int size)
{
	isup_stats_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = tg->statsp;
	return (QR_DONE);
}
STATIC int
isup_statp_get_sr(isup_stats_t * arg, struct sr *sr, int size)
{
	isup_stats_sr_t *sta = (typeof(sta)) (arg + 1);
	if (!sr || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = sr->statsp;
	return (QR_DONE);
}
STATIC int
isup_statp_get_sp(isup_stats_t * arg, struct sp *sp, int size)
{
	isup_stats_sp_t *sta = (typeof(sta)) (arg + 1);
	if (!sp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = sp->statsp;
	return (QR_DONE);
}
STATIC int
isup_statp_get_mtp(isup_stats_t * arg, struct mtp *mtp, int size)
{
	isup_stats_mtp_t *sta = (typeof(sta)) (arg + 1);
	if (!mtp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = mtp->statsp;
	return (QR_DONE);
}
STATIC int
isup_statp_get_df(isup_stats_t * arg, struct df *df, int size)
{
	isup_stats_df_t *sta = (typeof(sta)) (arg + 1);
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
isup_statp_set_ct(isup_stats_t * arg, struct ct *ct, int size)
{
	isup_stats_ct_t *sta = (typeof(sta)) (arg + 1);
	if (!ct || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	ct->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isup_statp_set_cg(isup_stats_t * arg, struct cg *cg, int size)
{
	isup_stats_cg_t *sta = (typeof(sta)) (arg + 1);
	if (!cg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	cg->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isup_statp_set_tg(isup_stats_t * arg, struct tg *tg, int size)
{
	isup_stats_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	tg->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isup_statp_set_sr(isup_stats_t * arg, struct sr *sr, int size)
{
	isup_stats_sr_t *sta = (typeof(sta)) (arg + 1);
	if (!sr || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	sr->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isup_statp_set_sp(isup_stats_t * arg, struct sp *sp, int size)
{
	isup_stats_sp_t *sta = (typeof(sta)) (arg + 1);
	if (!sp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	sp->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isup_statp_set_mtp(isup_stats_t * arg, struct mtp *mtp, int size)
{
	isup_stats_mtp_t *sta = (typeof(sta)) (arg + 1);
	if (!mtp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	mtp->statsp = *sta;
	return (QR_DONE);
}
STATIC int
isup_statp_set_df(isup_stats_t * arg, struct df *df, int size)
{
	isup_stats_df_t *sta = (typeof(sta)) (arg + 1);
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
isup_stat_get_ct(isup_stats_t * arg, struct ct *ct, int size)
{
	isup_stats_ct_t *sta = (typeof(sta)) (arg + 1);
	if (!ct || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = ct->stats;
	return (QR_DONE);
}
STATIC int
isup_stat_get_cg(isup_stats_t * arg, struct cg *cg, int size)
{
	isup_stats_cg_t *sta = (typeof(sta)) (arg + 1);
	if (!cg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = cg->stats;
	return (QR_DONE);
}
STATIC int
isup_stat_get_tg(isup_stats_t * arg, struct tg *tg, int size)
{
	isup_stats_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = tg->stats;
	return (QR_DONE);
}
STATIC int
isup_stat_get_sr(isup_stats_t * arg, struct sr *sr, int size)
{
	isup_stats_sr_t *sta = (typeof(sta)) (arg + 1);
	if (!sr || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = sr->stats;
	return (QR_DONE);
}
STATIC int
isup_stat_get_sp(isup_stats_t * arg, struct sp *sp, int size)
{
	isup_stats_sp_t *sta = (typeof(sta)) (arg + 1);
	if (!sp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = sp->stats;
	return (QR_DONE);
}
STATIC int
isup_stat_get_mtp(isup_stats_t * arg, struct mtp *mtp, int size)
{
	isup_stats_mtp_t *sta = (typeof(sta)) (arg + 1);
	if (!mtp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = mtp->stats;
	return (QR_DONE);
}
STATIC int
isup_stat_get_df(isup_stats_t * arg, struct df *df, int size)
{
	isup_stats_df_t *sta = (typeof(sta)) (arg + 1);
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
isup_stat_clr_ct(isup_stats_t * arg, struct ct *ct, int size)
{
	uchar *s, *d;
	isup_stats_ct_t *sta = (typeof(sta)) (arg + 1);
	if (!ct || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & ct->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isup_stat_clr_cg(isup_stats_t * arg, struct cg *cg, int size)
{
	uchar *s, *d;
	isup_stats_cg_t *sta = (typeof(sta)) (arg + 1);
	if (!cg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & cg->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isup_stat_clr_tg(isup_stats_t * arg, struct tg *tg, int size)
{
	uchar *s, *d;
	isup_stats_tg_t *sta = (typeof(sta)) (arg + 1);
	if (!tg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & tg->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isup_stat_clr_sr(isup_stats_t * arg, struct sr *sr, int size)
{
	uchar *s, *d;
	isup_stats_sr_t *sta = (typeof(sta)) (arg + 1);
	if (!sr || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & sr->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isup_stat_clr_sp(isup_stats_t * arg, struct sp *sp, int size)
{
	uchar *s, *d;
	isup_stats_sp_t *sta = (typeof(sta)) (arg + 1);
	if (!sp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & sp->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isup_stat_clr_mtp(isup_stats_t * arg, struct mtp *mtp, int size)
{
	uchar *s, *d;
	isup_stats_mtp_t *sta = (typeof(sta)) (arg + 1);
	if (!mtp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & mtp->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
isup_stat_clr_df(isup_stats_t * arg, struct df *df, int size)
{
	uchar *s, *d;
	isup_stats_df_t *sta = (typeof(sta)) (arg + 1);
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
 *  ISUP_IOCGOPTIONS
 *  ------------------------------------
 *  Get configuration options by object type and id.
 */
STATIC int
isup_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isup_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case ISUP_OBJ_TYPE_CT:
				return isup_opt_get_ct(arg, ct_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_CG:
				return isup_opt_get_cg(arg, cg_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_TG:
				return isup_opt_get_tg(arg, tg_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_SR:
				return isup_opt_get_sr(arg, sr_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_SP:
				return isup_opt_get_sp(arg, sp_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_MT:
				return isup_opt_get_mtp(arg, mtp_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_DF:
				return isup_opt_get_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISUP_IOCSOPTIONS
 *  ------------------------------------
 *  Set configuration options by object type and id.
 */
STATIC int
isup_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isup_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case ISUP_OBJ_TYPE_CT:
				return isup_opt_set_ct(arg, ct_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_CG:
				return isup_opt_set_cg(arg, cg_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_TG:
				return isup_opt_set_tg(arg, tg_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_SR:
				return isup_opt_set_sr(arg, sr_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_SP:
				return isup_opt_set_sp(arg, sp_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_MT:
				return isup_opt_set_mtp(arg, mtp_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_DF:
				return isup_opt_set_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISUP_IOCGCONFIG
 *  ------------------------------------
 *  Get configuration by object type and id.  Also gets the configuration of as
 *  many children of the object as will fit in the provided buffer.
 */
STATIC int
isup_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isup_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case ISUP_OBJ_TYPE_CT:
				return isup_get_ct(arg, ct_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_CG:
				return isup_get_cg(arg, cg_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_TG:
				return isup_get_tg(arg, tg_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_SR:
				return isup_get_sr(arg, sr_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_SP:
				return isup_get_sp(arg, sp_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_MT:
				return isup_get_mtp(arg, mtp_lookup(arg->id), size);
			case ISUP_OBJ_TYPE_DF:
				return isup_get_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISUP_IOCSCONFIG
 *  ------------------------------------
 *  Set configuration by object type, object id, and command.
 */
STATIC int
isup_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isup_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case ISUP_ADD:
				switch (arg->type) {
				case ISUP_OBJ_TYPE_CT:
					return isup_add_ct(arg, ct_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_CG:
					return isup_add_cg(arg, cg_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_TG:
					return isup_add_tg(arg, tg_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_SR:
					return isup_add_sr(arg, sr_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_SP:
					return isup_add_sp(arg, sp_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_MT:
					return isup_add_mtp(arg, mtp_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_DF:
					return isup_add_df(arg, &master, size, 0, 0);
				}
				break;
			case ISUP_CHA:
				switch (arg->type) {
				case ISUP_OBJ_TYPE_CT:
					return isup_cha_ct(arg, ct_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_CG:
					return isup_cha_cg(arg, cg_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_TG:
					return isup_cha_tg(arg, tg_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_SR:
					return isup_cha_sr(arg, sr_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_SP:
					return isup_cha_sp(arg, sp_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_MT:
					return isup_cha_mtp(arg, mtp_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_DF:
					return isup_cha_df(arg, &master, size, 0, 0);
				}
				break;
			case ISUP_DEL:
				switch (arg->type) {
				case ISUP_OBJ_TYPE_CT:
					return isup_del_ct(arg, ct_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_CG:
					return isup_del_cg(arg, cg_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_TG:
					return isup_del_tg(arg, tg_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_SR:
					return isup_del_sr(arg, sr_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_SP:
					return isup_del_sp(arg, sp_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_MT:
					return isup_del_mtp(arg, mtp_lookup(arg->id), size, 0, 0);
				case ISUP_OBJ_TYPE_DF:
					return isup_del_df(arg, &master, size, 0, 0);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISUP_IOCTCONFIG
 *  ------------------------------------
 *  Test configuraiton by object type and id.
 */
STATIC int
isup_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isup_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case ISUP_ADD:
				switch (arg->type) {
				case ISUP_OBJ_TYPE_CT:
					return isup_add_ct(arg, ct_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_CG:
					return isup_add_cg(arg, cg_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_TG:
					return isup_add_tg(arg, tg_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_SR:
					return isup_add_sr(arg, sr_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_SP:
					return isup_add_sp(arg, sp_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_MT:
					return isup_add_mtp(arg, mtp_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_DF:
					return isup_add_df(arg, &master, size, 0, 1);
				}
				break;
			case ISUP_CHA:
				switch (arg->type) {
				case ISUP_OBJ_TYPE_CT:
					return isup_cha_ct(arg, ct_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_CG:
					return isup_cha_cg(arg, cg_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_TG:
					return isup_cha_tg(arg, tg_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_SR:
					return isup_cha_sr(arg, sr_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_SP:
					return isup_cha_sp(arg, sp_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_MT:
					return isup_cha_mtp(arg, mtp_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_DF:
					return isup_cha_df(arg, &master, size, 0, 1);
				}
				break;
			case ISUP_DEL:
				switch (arg->type) {
				case ISUP_OBJ_TYPE_CT:
					return isup_del_ct(arg, ct_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_CG:
					return isup_del_cg(arg, cg_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_TG:
					return isup_del_tg(arg, tg_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_SR:
					return isup_del_sr(arg, sr_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_SP:
					return isup_del_sp(arg, sp_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_MT:
					return isup_del_mtp(arg, mtp_lookup(arg->id), size, 0, 1);
				case ISUP_OBJ_TYPE_DF:
					return isup_del_df(arg, &master, size, 0, 1);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISUP_IOCCCONFIG
 *  ------------------------------------
 *  Commit configuraiton by object type and id.
 */
STATIC int
isup_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		isup_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case ISUP_ADD:
				switch (arg->type) {
				case ISUP_OBJ_TYPE_CT:
					return isup_add_ct(arg, ct_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_CG:
					return isup_add_cg(arg, cg_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_TG:
					return isup_add_tg(arg, tg_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_SR:
					return isup_add_sr(arg, sr_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_SP:
					return isup_add_sp(arg, sp_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_MT:
					return isup_add_mtp(arg, mtp_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_DF:
					return isup_add_df(arg, &master, size, 1, 0);
				}
				break;
			case ISUP_CHA:
				switch (arg->type) {
				case ISUP_OBJ_TYPE_CT:
					return isup_cha_ct(arg, ct_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_CG:
					return isup_cha_cg(arg, cg_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_TG:
					return isup_cha_tg(arg, tg_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_SR:
					return isup_cha_sr(arg, sr_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_SP:
					return isup_cha_sp(arg, sp_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_MT:
					return isup_cha_mtp(arg, mtp_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_DF:
					return isup_cha_df(arg, &master, size, 1, 0);
				}
				break;
			case ISUP_DEL:
				switch (arg->type) {
				case ISUP_OBJ_TYPE_CT:
					return isup_del_ct(arg, ct_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_CG:
					return isup_del_cg(arg, cg_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_TG:
					return isup_del_tg(arg, tg_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_SR:
					return isup_del_sr(arg, sr_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_SP:
					return isup_del_sp(arg, sp_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_MT:
					return isup_del_mtp(arg, mtp_lookup(arg->id), size, 1, 0);
				case ISUP_OBJ_TYPE_DF:
					return isup_del_df(arg, &master, size, 1, 0);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISUP_IOCGSTATEM
 *  ------------------------------------
 *  Get state variables by object type and id.
 */
STATIC int
isup_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isup_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISUP_OBJ_TYPE_CT:
			return isup_sta_ct(arg, ct_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_CG:
			return isup_sta_cg(arg, cg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_TG:
			return isup_sta_tg(arg, tg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SR:
			return isup_sta_sr(arg, sr_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SP:
			return isup_sta_sp(arg, sp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_MT:
			return isup_sta_mtp(arg, mtp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_DF:
			return isup_sta_df(arg, &master, size);
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
 *  ISUP_IOCCMRESET
 *  ------------------------------------
 *  Perform master reset.
 */
STATIC int
isup_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		isup_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		(void) arg;
		return (-EOPNOTSUPP);
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISUP_IOCGSTATSP
 *  ------------------------------------
 *  Get statistics periods.
 */
STATIC int
isup_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isup_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISUP_OBJ_TYPE_CT:
			return isup_statp_get_ct(arg, ct_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_CG:
			return isup_statp_get_cg(arg, cg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_TG:
			return isup_statp_get_tg(arg, tg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SR:
			return isup_statp_get_sr(arg, sr_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SP:
			return isup_statp_get_sp(arg, sp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_MT:
			return isup_statp_get_mtp(arg, mtp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_DF:
			return isup_statp_get_df(arg, &master, size);
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
 *  ISUP_IOCSSTATSP
 *  ------------------------------------
 *  Set statistics periods.
 */
STATIC int
isup_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isup_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISUP_OBJ_TYPE_CT:
			return isup_statp_set_ct(arg, ct_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_CG:
			return isup_statp_set_cg(arg, cg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_TG:
			return isup_statp_set_tg(arg, tg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SR:
			return isup_statp_set_sr(arg, sr_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SP:
			return isup_statp_set_sp(arg, sp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_MT:
			return isup_statp_set_mtp(arg, mtp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_DF:
			return isup_statp_set_df(arg, &master, size);
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
 *  ISUP_IOCGSTATS
 *  ------------------------------------
 *  Get statistics.
 */
STATIC int
isup_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isup_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISUP_OBJ_TYPE_CT:
			return isup_stat_get_ct(arg, ct_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_CG:
			return isup_stat_get_cg(arg, cg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_TG:
			return isup_stat_get_tg(arg, tg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SR:
			return isup_stat_get_sr(arg, sr_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SP:
			return isup_stat_get_sp(arg, sp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_MT:
			return isup_stat_get_mtp(arg, mtp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_DF:
			return isup_stat_get_df(arg, &master, size);
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
 *  ISUP_IOCCSTATS
 *  ------------------------------------
 *  Clear statistics.
 */
STATIC int
isup_ioccstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		isup_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISUP_OBJ_TYPE_CT:
			return isup_stat_clr_ct(arg, ct_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_CG:
			return isup_stat_clr_cg(arg, cg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_TG:
			return isup_stat_clr_tg(arg, tg_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SR:
			return isup_stat_clr_sr(arg, sr_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_SP:
			return isup_stat_clr_sp(arg, sp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_MT:
			return isup_stat_clr_mtp(arg, mtp_lookup(arg->id), size);
		case ISUP_OBJ_TYPE_DF:
			return isup_stat_clr_df(arg, &master, size);
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
 *  ISUP_IOCGNOTIFY
 *  ------------------------------------
 *  Get event notifications associated with an object.
 */
STATIC int
isup_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		isup_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISUP_OBJ_TYPE_CT:
			ret = isup_not_get_ct(arg, ct_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_CG:
			ret = isup_not_get_cg(arg, cg_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_TG:
			ret = isup_not_get_tg(arg, tg_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_SR:
			ret = isup_not_get_sr(arg, sr_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_SP:
			ret = isup_not_get_sp(arg, sp_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_MT:
			ret = isup_not_get_mtp(arg, mtp_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_DF:
			ret = isup_not_get_df(arg, &master);
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
 *  ISUP_IOCSNOTIFY
 *  ------------------------------------
 *  Set event notifications associated with an object.
 */
STATIC int
isup_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		isup_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISUP_OBJ_TYPE_CT:
			ret = isup_not_set_ct(arg, ct_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_CG:
			ret = isup_not_set_cg(arg, cg_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_TG:
			ret = isup_not_set_tg(arg, tg_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_SR:
			ret = isup_not_set_sr(arg, sr_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_SP:
			ret = isup_not_set_sp(arg, sp_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_MT:
			ret = isup_not_set_mtp(arg, mtp_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_DF:
			ret = isup_not_set_df(arg, &master);
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
 *  ISUP_IOCCNOTIFY
 *  ------------------------------------
 *  Clear event notifications associated with an object.
 */
STATIC int
isup_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		isup_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case ISUP_OBJ_TYPE_CT:
			ret = isup_not_clr_ct(arg, ct_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_CG:
			ret = isup_not_clr_cg(arg, cg_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_TG:
			ret = isup_not_clr_tg(arg, tg_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_SR:
			ret = isup_not_clr_sr(arg, sr_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_SP:
			ret = isup_not_clr_sp(arg, sp_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_MT:
			ret = isup_not_clr_mtp(arg, mtp_lookup(arg->id));
			break;
		case ISUP_OBJ_TYPE_DF:
			ret = isup_not_clr_df(arg, &master);
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
 *  ISUP_IOCCMGMT
 *  ------------------------------------
 *  Manage objects.
 */
STATIC int
isup_ioccmgmt(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		isup_mgmt_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		switch (arg->cmd) {
		case ISUP_MGMT_BLOCK:
			switch (arg->type) {
			case ISUP_OBJ_TYPE_CT:
				return isup_mgmt_blo_ct(arg, ct_lookup(arg->id));
			case ISUP_OBJ_TYPE_CG:
				return isup_mgmt_blo_cg(arg, cg_lookup(arg->id));
			case ISUP_OBJ_TYPE_TG:
				return isup_mgmt_blo_tg(arg, tg_lookup(arg->id));
			case ISUP_OBJ_TYPE_SR:
				return isup_mgmt_blo_sr(arg, sr_lookup(arg->id));
			case ISUP_OBJ_TYPE_SP:
				return isup_mgmt_blo_sp(arg, sp_lookup(arg->id));
			case ISUP_OBJ_TYPE_MT:
				return isup_mgmt_blo_mtp(arg, mtp_lookup(arg->id));
			case ISUP_OBJ_TYPE_DF:
				return isup_mgmt_blo_df(arg, &master);
			}
			break;
		case ISUP_MGMT_UNBLOCK:
			switch (arg->type) {
			case ISUP_OBJ_TYPE_CT:
				return isup_mgmt_ubl_ct(arg, ct_lookup(arg->id));
			case ISUP_OBJ_TYPE_CG:
				return isup_mgmt_ubl_cg(arg, cg_lookup(arg->id));
			case ISUP_OBJ_TYPE_TG:
				return isup_mgmt_ubl_tg(arg, tg_lookup(arg->id));
			case ISUP_OBJ_TYPE_SR:
				return isup_mgmt_ubl_sr(arg, sr_lookup(arg->id));
			case ISUP_OBJ_TYPE_SP:
				return isup_mgmt_ubl_sp(arg, sp_lookup(arg->id));
			case ISUP_OBJ_TYPE_MT:
				return isup_mgmt_ubl_mtp(arg, mtp_lookup(arg->id));
			case ISUP_OBJ_TYPE_DF:
				return isup_mgmt_ubl_df(arg, &master);
			}
			break;
		case ISUP_MGMT_RESET:
			switch (arg->type) {
			case ISUP_OBJ_TYPE_CT:
				return isup_mgmt_res_ct(arg, ct_lookup(arg->id));
			case ISUP_OBJ_TYPE_CG:
				return isup_mgmt_res_cg(arg, cg_lookup(arg->id));
			case ISUP_OBJ_TYPE_TG:
				return isup_mgmt_res_tg(arg, tg_lookup(arg->id));
			case ISUP_OBJ_TYPE_SR:
				return isup_mgmt_res_sr(arg, sr_lookup(arg->id));
			case ISUP_OBJ_TYPE_SP:
				return isup_mgmt_res_sp(arg, sp_lookup(arg->id));
			case ISUP_OBJ_TYPE_MT:
				return isup_mgmt_res_mtp(arg, mtp_lookup(arg->id));
			case ISUP_OBJ_TYPE_DF:
				return isup_mgmt_res_df(arg, &master);
			}
			break;
		case ISUP_MGMT_QUERY:
			switch (arg->type) {
			case ISUP_OBJ_TYPE_CT:
				return isup_mgmt_qry_ct(arg, ct_lookup(arg->id));
			case ISUP_OBJ_TYPE_CG:
				return isup_mgmt_qry_cg(arg, cg_lookup(arg->id));
			case ISUP_OBJ_TYPE_TG:
				return isup_mgmt_qry_tg(arg, tg_lookup(arg->id));
			case ISUP_OBJ_TYPE_SR:
				return isup_mgmt_qry_sr(arg, sr_lookup(arg->id));
			case ISUP_OBJ_TYPE_SP:
				return isup_mgmt_qry_sp(arg, sp_lookup(arg->id));
			case ISUP_OBJ_TYPE_MT:
				return isup_mgmt_qry_mtp(arg, mtp_lookup(arg->id));
			case ISUP_OBJ_TYPE_DF:
				return isup_mgmt_qry_df(arg, &master);
			}
			break;
		}
	}
	rare();
	return (-EINVAL);
}

/*
 *  ISUP_IOCCPASS
 *  ------------------------------------
 *  Pass a message to lower layer.
 */
STATIC int
isup_ioccpass(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		isup_pass_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		mblk_t *bp, *dp;
		struct mtp *mtp;
		for (mtp = master.mtp.list; mtp && mtp->u.mux.index != arg->muxid;
		     mtp = mtp->next) ;
		if (!mtp || !mtp->oq)
			return (-EINVAL);
		if (arg->type < QPCTL && !canput(mtp->oq))
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
		ss7_oput(mtp->oq, bp);
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
isup_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct cc *cc = CC_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	// struct linkblk *lp = (struct linkblk *) arg;
	int ret = 0;
	(void) cc;
	switch (type) {
	case _IOC_TYPE(__SID):
	{
		psw_t flags;
		struct mtp *mtp, **mtpp;
		struct linkblk *lb;
		if (!(lb = arg)) {
			swerr();
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			ptrace(("%s: %p: I_PLINK\n", DRV_NAME, cc));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n",
					DRV_NAME, cc));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			ptrace(("%s: %p: I_LINK\n", DRV_NAME, cc));
			MOD_INC_USE_COUNT;	/* keep module from unloading */
			spin_lock_irqsave(&master.lock, flags);
			{
				/* 
				   place in list in ascending index order */
				for (mtpp = &master.mtp.list;
				     *mtpp && (*mtpp)->u.mux.index < lb->l_index;
				     mtpp = &(*mtpp)->next) ;
				if ((mtp =
				     isup_alloc_mtp(lb->l_qbot, mtpp, lb->l_index, iocp->ioc_cr))) {
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				MOD_DEC_USE_COUNT;
				ret = -ENOMEM;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		case _IOC_NR(I_PUNLINK):
			ptrace(("%s: %p: I_PUNLINK\n", DRV_NAME, cc));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n",
					DRV_NAME, cc));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			ptrace(("%s: %p: I_UNLINK\n", DRV_NAME, cc));
			spin_lock_irqsave(&master.lock, flags);
			{
				for (mtp = master.mtp.list; mtp; mtp = mtp->next)
					if (mtp->u.mux.index == lb->l_index)
						break;
				if (!mtp) {
					ret = -EINVAL;
					ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n",
						DRV_NAME, cc));
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				isup_free_mtp(mtp->iq);
				MOD_DEC_USE_COUNT;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %c, %d\n", DRV_NAME,
				cc, (char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case ISUP_IOC_MAGIC:
	{
		if (count < size) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(ISUP_IOCGOPTIONS):
			printd(("%s; %p: -> ISUP_IOCGOPTIONS\n", DRV_NAME, cc));
			ret = isup_iocgoptions(q, mp);
			break;
		case _IOC_NR(ISUP_IOCSOPTIONS):
			printd(("%s; %p: -> ISUP_IOCSOPTIONS\n", DRV_NAME, cc));
			ret = isup_iocsoptions(q, mp);
			break;
		case _IOC_NR(ISUP_IOCGCONFIG):
			printd(("%s; %p: -> ISUP_IOCGCONFIG\n", DRV_NAME, cc));
			ret = isup_iocgconfig(q, mp);
			break;
		case _IOC_NR(ISUP_IOCSCONFIG):
			printd(("%s; %p: -> ISUP_IOCSCONFIG\n", DRV_NAME, cc));
			ret = isup_iocsconfig(q, mp);
			break;
		case _IOC_NR(ISUP_IOCTCONFIG):
			printd(("%s; %p: -> ISUP_IOCTCONFIG\n", DRV_NAME, cc));
			ret = isup_ioctconfig(q, mp);
			break;
		case _IOC_NR(ISUP_IOCCCONFIG):
			printd(("%s; %p: -> ISUP_IOCCCONFIG\n", DRV_NAME, cc));
			ret = isup_ioccconfig(q, mp);
			break;
		case _IOC_NR(ISUP_IOCGSTATEM):
			printd(("%s; %p: -> ISUP_IOCGSTATEM\n", DRV_NAME, cc));
			ret = isup_iocgstatem(q, mp);
			break;
		case _IOC_NR(ISUP_IOCCMRESET):
			printd(("%s; %p: -> ISUP_IOCCMRESET\n", DRV_NAME, cc));
			ret = isup_ioccmreset(q, mp);
			break;
		case _IOC_NR(ISUP_IOCGSTATSP):
			printd(("%s; %p: -> ISUP_IOCGSTATSP\n", DRV_NAME, cc));
			ret = isup_iocgstatsp(q, mp);
			break;
		case _IOC_NR(ISUP_IOCSSTATSP):
			printd(("%s; %p: -> ISUP_IOCSSTATSP\n", DRV_NAME, cc));
			ret = isup_iocsstatsp(q, mp);
			break;
		case _IOC_NR(ISUP_IOCGSTATS):
			printd(("%s; %p: -> ISUP_IOCGSTATS\n", DRV_NAME, cc));
			ret = isup_iocgstats(q, mp);
			break;
		case _IOC_NR(ISUP_IOCCSTATS):
			printd(("%s; %p: -> ISUP_IOCCSTATS\n", DRV_NAME, cc));
			ret = isup_ioccstats(q, mp);
			break;
		case _IOC_NR(ISUP_IOCGNOTIFY):
			printd(("%s; %p: -> ISUP_IOCGNOTIFY\n", DRV_NAME, cc));
			ret = isup_iocgnotify(q, mp);
			break;
		case _IOC_NR(ISUP_IOCSNOTIFY):
			printd(("%s; %p: -> ISUP_IOCSNOTIFY\n", DRV_NAME, cc));
			ret = isup_iocsnotify(q, mp);
			break;
		case _IOC_NR(ISUP_IOCCNOTIFY):
			printd(("%s; %p: -> ISUP_IOCCNOTIFY\n", DRV_NAME, cc));
			ret = isup_ioccnotify(q, mp);
			break;
		case _IOC_NR(ISUP_IOCCMGMT):
			printd(("%s; %p: -> ISUP_IOCCMGMT\n", DRV_NAME, cc));
			ret = isup_ioccmgmt(q, mp);
			break;
		case _IOC_NR(ISUP_IOCCPASS):
			printd(("%s; %p: -> ISUP_IOCCPASS\n", DRV_NAME, cc));
			ret = isup_ioccpass(q, mp);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported ISUP ioctl %c, %d\n", DRV_NAME, cc,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ptrace(("%s: %p: ERROR: Unsupported ioctl %c, %d\n", DRV_NAME, cc, (char) type,
			nr));
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
 *  Primitives from CC to ISUP.
 *  -----------------------------------
 */
STATIC int
isup_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct cc *cc = CC_PRIV(q);
	ulong oldstate = cs_get_state(cc);
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
		rtn = cc_cont_check_req(q, mp);
		break;
	case CC_CONT_TEST_REQ:
		printd(("%s: %p: -> CC_CONT_TEST_REQ\n", DRV_NAME, cc));
		rtn = cc_cont_test_req(q, mp);
		break;
	case CC_CONT_REPORT_REQ:
		printd(("%s: %p: -> CC_CONT_REPORT_REQ\n", DRV_NAME, cc));
		rtn = cc_cont_report_req(q, mp);
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
	case CC_IBI_REQ:
		printd(("%s: %p: -> CC_IBI_REQ\n", DRV_NAME, cc));
		rtn = cc_ibi_req(q, mp);
		break;
	case CC_CONNECT_REQ:
		printd(("%s: %p: -> CC_CONNECT_REQ\n", DRV_NAME, cc));
		rtn = cc_connect_req(q, mp);
		break;
	case CC_SETUP_COMPLETE_REQ:
		printd(("%s: %p: -> CC_SETUP_COMPLETE_REQ\n", DRV_NAME, cc));
		rtn = cc_setup_complete_req(q, mp);
		break;
	case CC_FORWXFER_REQ:
		printd(("%s: %p: -> CC_FORWXFER_REQ\n", DRV_NAME, cc));
		rtn = cc_forwxfer_req(q, mp);
		break;
	case CC_SUSPEND_REQ:
		printd(("%s: %p: -> CC_SUSPEND_REQ\n", DRV_NAME, cc));
		rtn = cc_suspend_req(q, mp);
		break;
	case CC_SUSPEND_RES:
		printd(("%s: %p: -> CC_SUSPEND_RES\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_SUSPEND_REJECT_REQ:
		printd(("%s: %p: -> CC_SUSPEND_REJECT_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_RESUME_REQ:
		printd(("%s: %p: -> CC_RESUME_REQ\n", DRV_NAME, cc));
		rtn = cc_resume_req(q, mp);
		break;
	case CC_RESUME_RES:
		printd(("%s: %p: -> CC_RESUME_RES\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_RESUME_REJECT_REQ:
		printd(("%s: %p: -> CC_RESUME_REJECT_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
		break;
	case CC_REJECT_REQ:
		printd(("%s: %p: -> CC_REJECT_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
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
		rtn = cc_reset_req(q, mp);
		break;
	case CC_RESET_RES:
		printd(("%s: %p: -> CC_RESET_RES\n", DRV_NAME, cc));
		rtn = cc_reset_res(q, mp);
		break;
	case CC_BLOCKING_REQ:
		printd(("%s: %p: -> CC_BLOCKING_REQ\n", DRV_NAME, cc));
		rtn = cc_blocking_req(q, mp);
		break;
	case CC_BLOCKING_RES:
		printd(("%s: %p: -> CC_BLOCKING_RES\n", DRV_NAME, cc));
		rtn = cc_blocking_res(q, mp);
		break;
	case CC_UNBLOCKING_REQ:
		printd(("%s: %p: -> CC_UNBLOCKING_REQ\n", DRV_NAME, cc));
		rtn = cc_unblocking_req(q, mp);
		break;
	case CC_UNBLOCKING_RES:
		printd(("%s: %p: -> CC_UNBLOCKING_RES\n", DRV_NAME, cc));
		rtn = cc_unblocking_res(q, mp);
		break;
	case CC_QUERY_REQ:
		printd(("%s: %p: -> CC_QUERY_REQ\n", DRV_NAME, cc));
		rtn = cc_query_req(q, mp);
		break;
	case CC_QUERY_RES:
		printd(("%s: %p: -> CC_QUERY_RES\n", DRV_NAME, cc));
		rtn = cc_query_res(q, mp);
		break;
	case CC_RESTART_REQ:
		printd(("%s: %p: -> CC_RESTART_REQ\n", DRV_NAME, cc));
		rtn = cc_unsupported_prim(q, mp);
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
		cs_set_state(cc, oldstate);
	return (rtn);
}

/*
 *  Primitives from MTP to ISUP.
 *  -----------------------------------
 */
STATIC int
mtp_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct mtp *mtp = MTP_PRIV(q);
	ulong oldstate = mtp->i_state;
	switch (*((ulong *) mp->b_rptr)) {
	case MTP_OK_ACK:
		printd(("%s: %p: MTP_OK_ACK <-\n", DRV_NAME, mtp));
		rtn = mtp_ok_ack(q, mp);
		break;
	case MTP_ERROR_ACK:
		printd(("%s: %p: MTP_ERROR_ACK <-\n", DRV_NAME, mtp));
		rtn = mtp_error_ack(q, mp);
		break;
	case MTP_BIND_ACK:
		printd(("%s: %p: MTP_BIND_ACK <-\n", DRV_NAME, mtp));
		rtn = mtp_bind_ack(q, mp);
		break;
	case MTP_ADDR_ACK:
		printd(("%s: %p: MTP_ADDR_ACK <-\n", DRV_NAME, mtp));
		rtn = mtp_addr_ack(q, mp);
		break;
	case MTP_INFO_ACK:
		printd(("%s: %p: MTP_INFO_ACK <-\n", DRV_NAME, mtp));
		rtn = mtp_info_ack(q, mp);
		break;
	case MTP_OPTMGMT_ACK:
		printd(("%s: %p: MTP_OPTMGMT_ACK <-\n", DRV_NAME, mtp));
		rtn = mtp_optmgmt_ack(q, mp);
		break;
	case MTP_TRANSFER_IND:
		printd(("%s: %p: MTP_TRANSFER_IND <-\n", DRV_NAME, mtp));
		rtn = mtp_transfer_ind(q, mp);
		break;
	case MTP_PAUSE_IND:
		printd(("%s: %p: MTP_PAUSE_IND <-\n", DRV_NAME, mtp));
		rtn = mtp_pause_ind(q, mp);
		break;
	case MTP_RESUME_IND:
		printd(("%s: %p: MTP_RESUME_IND <-\n", DRV_NAME, mtp));
		rtn = mtp_resume_ind(q, mp);
		break;
	case MTP_STATUS_IND:
		printd(("%s: %p: MTP_STATUS_IND <-\n", DRV_NAME, mtp));
		rtn = mtp_status_ind(q, mp);
		break;
	case MTP_RESTART_COMPLETE_IND:
		printd(("%s: %p: MTP_RESART_COMPLETE_IND <-\n", DRV_NAME, mtp));
		rtn = mtp_restart_complete_ind(q, mp);
		break;
	default:
		printd(("%s: %p: ???? %lu <-\n", DRV_NAME, mtp, *(ulong *) mp->b_rptr));
		rtn = mtp_unrecognized_prim(q, mp);
		break;
	}
	if (rtn < 0)
		mtp->i_state = oldstate;
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
isup_w_data(queue_t *q, mblk_t *mp)
{
	/* 
	   data from above */
	return isup_write(q, mp);
}
STATIC int
mtp_r_data(queue_t *q, mblk_t *mp)
{
	/* 
	   data from below */
	swerr();
	/* 
	   we need the MTP-TRANSFER-IND because we cannot tell which switch the message came from
	   without it. */
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR, M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
mtp_r_error(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	return mtp_hangup(mtp);
}
STATIC INLINE int
mtp_r_hangup(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	return mtp_hangup(mtp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
isup_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE int
isup_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return isup_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return isup_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return isup_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return isup_w_ioctl(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC INLINE int
mtp_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mtp_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mtp_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return mtp_r_error(q, mp);
	case M_HANGUP:
		return mtp_r_hangup(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC INLINE int
mtp_w_prim(queue_t *q, mblk_t *mp)
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
STATIC major_t isup_majors[ISUP_CMAJORS] = { ISUP_CMAJOR_0, };

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct cc *cc, **ccp = &master.cc.list;
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
	if (cmajor != ISUP_CMAJOR_0 || cminor > 0) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/* 
	   allocate a new device */
	cminor = 1;
	spin_lock_irqsave(&master.lock, flags);
	for (; *ccp; ccp = &(*ccp)->next) {
		major_t dmajor = (*ccp)->u.dev.cmajor;
		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*ccp)->u.dev.cminor;
			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= ISUP_CMAJORS
					    || !(cmajor = isup_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= ISUP_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(cc = isup_alloc_cc(q, ccp, devp, crp))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&master.lock, flags);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
isup_close(queue_t *q, int flag, cred_t *crp)
{
	struct cc *cc = CC_PRIV(q);
	head_t *h = (head_t *) cc;
	str_t *s = (str_t *) cc;
	psw_t flags;
	(void) flag;
	(void) crp;
	(void) s;
	(void) h;
	printd(("%s: %p: closing character device %d:%d\n", DRV_NAME, cc, cc->u.dev.cmajor,
		cc->u.dev.cminor));
	spin_lock_irqsave(&master.lock, flags);
	{
		isup_free_cc(q);
	}
	spin_unlock_irqrestore(&master.lock, flags);
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
STATIC kmem_cache_t *isup_cc_cachep = NULL;
STATIC kmem_cache_t *isup_tg_cachep = NULL;
STATIC kmem_cache_t *isup_cg_cachep = NULL;
STATIC kmem_cache_t *isup_ct_cachep = NULL;
STATIC kmem_cache_t *isup_sr_cachep = NULL;
STATIC kmem_cache_t *isup_sp_cachep = NULL;
STATIC kmem_cache_t *isup_mtp_cachep = NULL;

STATIC int
isup_init_caches(void)
{
	if (!isup_cc_cachep
	    && !(isup_cc_cachep =
		 kmem_cache_create("isup_cc_cachep", sizeof(struct cc), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isup_cc_cachep", DRV_NAME);
		goto no_cc;
		return (-ENOMEM);
	} else
		printd(("%s: initialized isup cc structure cache\n", DRV_NAME));
	if (!isup_tg_cachep
	    && !(isup_tg_cachep =
		 kmem_cache_create("isup_tg_cachep", sizeof(struct tg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isup_tg_cachep", DRV_NAME);
		goto no_tg;
	} else
		printd(("%s: initialized isup tg structure cache\n", DRV_NAME));
	if (!isup_cg_cachep
	    && !(isup_cg_cachep =
		 kmem_cache_create("isup_cg_cachep", sizeof(struct cg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isup_cg_cachep", DRV_NAME);
		goto no_cg;
	} else
		printd(("%s: initialized isup tg structure cache\n", DRV_NAME));
	if (!isup_ct_cachep
	    && !(isup_ct_cachep =
		 kmem_cache_create("isup_ct_cachep", sizeof(struct ct), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isup_ct_cachep", DRV_NAME);
		goto no_ct;
	} else
		printd(("%s: initialized isup ct structure cache\n", DRV_NAME));
	if (!isup_sr_cachep
	    && !(isup_sr_cachep =
		 kmem_cache_create("isup_sr_cachep", sizeof(struct sr), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isup_sr_cachep", DRV_NAME);
		goto no_sr;
	} else
		printd(("%s: initialized isup sr structure cache\n", DRV_NAME));
	if (!isup_sp_cachep
	    && !(isup_sp_cachep =
		 kmem_cache_create("isup_sp_cachep", sizeof(struct sp), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isup_sp_cachep", DRV_NAME);
		goto no_sp;
	} else
		printd(("%s: initialized isup sp structure cache\n", DRV_NAME));
	if (!isup_mtp_cachep
	    && !(isup_mtp_cachep =
		 kmem_cache_create("isup_mtp_cachep", sizeof(struct mtp), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate isup_mtp_cachep", DRV_NAME);
		goto no_mtp;
	} else
		printd(("%s: initialized mtp structure cache\n", DRV_NAME));
	return (0);
      no_mtp:
	kmem_cache_destroy(isup_sp_cachep);
      no_sp:
	kmem_cache_destroy(isup_sr_cachep);
      no_sr:
	kmem_cache_destroy(isup_ct_cachep);
      no_ct:
	kmem_cache_destroy(isup_cg_cachep);
      no_cg:
	kmem_cache_destroy(isup_tg_cachep);
      no_tg:
	kmem_cache_destroy(isup_cc_cachep);
      no_cc:
	return (-ENOMEM);
}
STATIC int
isup_term_caches(void)
{
	int err = 0;
	if (isup_cc_cachep) {
		if (kmem_cache_destroy(isup_cc_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isup_cc_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isup_cc_cachep\n", DRV_NAME));
	}
	if (isup_tg_cachep) {
		if (kmem_cache_destroy(isup_tg_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isup_tg_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isup_tg_cachep\n", DRV_NAME));
	}
	if (isup_cg_cachep) {
		if (kmem_cache_destroy(isup_cg_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isup_cg_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isup_cg_cachep\n", DRV_NAME));
	}
	if (isup_ct_cachep) {
		if (kmem_cache_destroy(isup_ct_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isup_ct_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isup_ct_cachep\n", DRV_NAME));
	}
	if (isup_sr_cachep) {
		if (kmem_cache_destroy(isup_sr_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isup_sr_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isup_sr_cachep\n", DRV_NAME));
	}
	if (isup_sp_cachep) {
		if (kmem_cache_destroy(isup_sp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isup_sp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isup_sp_cachep\n", DRV_NAME));
	}
	if (isup_mtp_cachep) {
		if (kmem_cache_destroy(isup_mtp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy isup_mtp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed isup_mtp_cachep\n", DRV_NAME));
	}
	return (err);
}

/*
 *  CC - Call Control
 *  -----------------------------------
 *  Upper stream private structure allocation, deallocation and reference counting
 */
STATIC struct cc *
isup_alloc_cc(queue_t *q, struct cc **ccp, dev_t *devp, cred_t *crp)
{
	struct cc *cc;
	printd(("%s: %s: create cc dev = %d:%d\n", DRV_NAME, __FUNCTION__, getmajor(*devp),
		getminor(*devp)));
	if ((cc = kmem_cache_alloc(isup_cc_cachep, SLAB_ATOMIC))) {
		bzero(cc, sizeof(*cc));
		cc_get(cc);	/* first get */
		cc->u.dev.cmajor = getmajor(*devp);
		cc->u.dev.cminor = getminor(*devp);
		cc->cred = *crp;
		spin_lock_init(&cc->qlock);	/* "cc-queue-lock" */
		(cc->oq = RD(q))->q_ptr = cc_get(cc);
		(cc->iq = WR(q))->q_ptr = cc_get(cc);
		cc->o_prim = &isup_r_prim;
		cc->i_prim = &isup_w_prim;
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
isup_free_cc(queue_t *q)
{
	struct cc *cc = CC_PRIV(q);
	psw_t flags;
	ensure(cc, return);
	printd(("%s: %s: %p: free cc %d:%d\n", DRV_NAME, __FUNCTION__, cc, cc->u.dev.cmajor,
		cc->u.dev.cminor));
	spin_lock_irqsave(&cc->lock, flags);
	{
		/* 
		   stopping bufcalls */
		ss7_unbufcall((str_t *) cc);
		/* 
		   flushing buffser */
		flushq(cc->oq, FLUSHALL);
		flushq(cc->iq, FLUSHALL);
		/* 
		   detach from call processing */
		if (cc->conn.cpc)
			ct_set_i_state(cc->conn.cpc, cc, CCS_IDLE);
		/* 
		   detach from circuit test */
		if (cc->conn.tst)
			ct_set_t_state(cc->conn.tst, cc, CKS_IDLE);
		/* 
		   detach from circuit management */
		if (cc->conn.mgm)
			ct_set_m_state(cc->conn.mgm, cc, CMS_IDLE);
		/* 
		   detach from circuit group management */
		if (cc->conn.gmg)
			cg_set_m_state(cc->conn.gmg, cc, CMS_IDLE);
		/* 
		   unbind */
		if (cs_get_state(cc) != CCS_UNBND) {
			switch (cc->bind.type) {
			case ISUP_BIND_CT:
				ct_put(xchg(&cc->bind.u.ct, NULL));
				break;
			case ISUP_BIND_CG:
				cg_put(xchg(&cc->bind.u.cg, NULL));
				break;
			case ISUP_BIND_TG:
				tg_put(xchg(&cc->bind.u.tg, NULL));
				break;
			case ISUP_BIND_SR:
				sr_put(xchg(&cc->bind.u.sr, NULL));
				break;
			case ISUP_BIND_SP:
				sp_put(xchg(&cc->bind.u.sp, NULL));
				break;
			case ISUP_BIND_DF:
				xchg(&cc->bind.u.df, NULL);
				break;
			default:
				swerr();
				goto not_bound;
			}
			cc->bind.type = ISUP_BIND_NONE;
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&cc->refcnt) > 1, cc_get(cc));
			cc_put(cc);
		      not_bound:
			cs_set_state(cc, CCS_UNBND);
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
		kmem_cache_free(isup_cc_cachep, cc);
		printd(("%s: %s: %p: deallocated cc structure", DRV_NAME, __FUNCTION__, cc));
	}
}

/*
 *  CT - Circuit
 *  -----------------------------------
 *  Circuit structure allocation, deallocation and reference counting
 */
STATIC struct ct *
isup_alloc_ct(ulong id, struct tg *tg, struct cg *cg, ulong cic)
{
	struct ct *ct, **ctp;
	struct sr *sr = tg->sr.sr;
	struct sp *sp = sr->sp.sp;
	assure(sr == cg->sr.sr);
	printd(("%s: %s: create ct->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((ct = kmem_cache_alloc(isup_ct_cachep, SLAB_ATOMIC))) {
		bzero(ct, sizeof(*ct));
		ct_get(ct);	/* first get */
		spin_lock_init(&ct->lock);	/* "ct-lock" */
		ct->id = id;
		ct->cic = cic;
		/* 
		   place in master list (ascending id) */
		for (ctp = &master.ct.list; *ctp && (*ctp)->id < id; ctp = &(*ctp)->next) ;
		if ((ct->next = *ctp))
			ct->next->prev = &ct->next;
		ct->prev = ctp;
		*ctp = ct_get(ct);
		master.ct.numb++;
		/* 
		   place in circuit group list (ascending cic) */
		for (ctp = &cg->ct.list; *ctp && (*ctp)->cic < cic; ctp = &(*ctp)->cg.next) ;
		if ((ct->cg.next = *ctp))
			ct->cg.next->cg.prev = &ct->cg.next;
		ct->cg.prev = ctp;
		*ctp = ct_get(ct);
		cg->ct.numb++;
		ct->cg.cg = cg_get(cg);
		/* 
		   adjust base cic if no blocking pending */
		if (!cg_tst(cg, CCTF_LOC_M_BLOCK_PENDING | CCTF_LOC_H_BLOCK_PENDING))
			cg->cic = cg->ct.list->cic;
		/* 
		   place in trunk group list (ascending cic) */
		for (ctp = &tg->ct.list; *ctp && (*ctp)->cic < cic; ctp = &(*ctp)->tg.next) ;
		if ((ct->tg.next = *ctp))
			ct->tg.next->tg.prev = &ct->tg.next;
		ct->tg.prev = ctp;
		*ctp = ct_get(ct);
		tg->ct.numb++;
		ct->tg.tg = tg_get(tg);
		/* 
		   place in signalling relation list (ascending cic) */
		for (ctp = &sr->ct.list; *ctp && (*ctp)->cic < cic; ctp = &(*ctp)->sr.next) ;
		if ((ct->sr.next = *ctp))
			ct->sr.next->sr.prev = &ct->sr.next;
		ct->sr.prev = ctp;
		*ctp = ct_get(ct);
		sr->ct.numb++;
		ct->sr.sr = sr_get(sr);
		/* 
		   place in signalling point list (ascending cic) */
		for (ctp = &sp->ct.list; *ctp && (*ctp)->cic < cic; ctp = &(*ctp)->sp.next) ;
		if ((ct->sp.next = *ctp))
			ct->sp.next->sp.prev = &ct->sp.next;
		ct->sp.prev = ctp;
		*ctp = ct_get(ct);
		sp->ct.numb++;
		ct->sp.sp = sp_get(sp);
		/* 
		   reset idle (this will place us in the trunk group idle list) */
		ct->idle.next = NULL;
		ct->idle.prev = &ct->idle.next;
		ct_set_c_state(ct, CTS_IDLE);
		ct->i_state = CCS_IDLE;
		ct->cpc.next = NULL;
		ct->cpc.prev = &ct->cpc.next;
		ct->m_state = CMS_IDLE;
		ct->mgm.next = NULL;
		ct->mgm.prev = &ct->mgm.next;
		ct->t_state = CKS_IDLE;
		ct->tst.next = NULL;
		ct->tst.prev = &ct->tst.next;
	} else
		printd(("%s: %s: ERROR: failed to allocate ct structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (ct);
}
STATIC void
isup_free_ct(struct ct *ct)
{
	struct cc *cc;
	psw_t flags;
	ensure(ct, return);
	printd(("%s: %s: %p free ct->id = %ld\n", DRV_NAME, __FUNCTION__, ct, ct->id));
	spin_lock_irqsave(&ct->lock, flags);
	{
		/* 
		   stop all timers */
		__ct_timer_stop(ct, tall);
		/* 
		   free messages */
		if (ct->sgm)
			freemsg(xchg(&ct->sgm, NULL));
		if (ct->rel)
			freemsg(xchg(&ct->rel, NULL));
		assure(!ct->cpc.cc);
		/* 
		   detach from active call processing */
		if ((cc = ct->cpc.cc)) {
			/* 
			   We are currently engaged in a call on the circuit.  This only occurs
			   when we are removing with force.  We must M_HANGUP the stream so that it 
			   knows not to expect anything more from us. */
			m_error(NULL, cc, EPIPE);
			ct_set_i_state(ct, cc, CCS_IDLE);
			cs_set_state(cc, CCS_UNUSABLE);
		}
		assure(!ct->tst.cc);
		/* 
		   detach from active call testing */
		if ((cc = ct->tst.cc)) {
			/* 
			   We are currently engaged in a test call on the circuit.  This only
			   occures when we are removing with force.  We must M_HANGUP the stream so 
			   that it knows not to expecte anything more from us. */
			m_error(NULL, cc, EPIPE);
			ct_set_t_state(ct, cc, CKS_IDLE);
			cs_set_state(cc, CCS_UNUSABLE);
		}
		assure(!ct->mgm.cc);
		/* 
		   detach from active management action */
		if ((cc = ct->mgm.cc)) {
			/* 
			   We are currently engaged in management on the circuit. This only occurs
			   when we are removing with force.  we must M_HANGUP the stream so that it 
			   knows not to expect anything more from us. */
			m_error(NULL, cc, EPIPE);
			ct_set_m_state(ct, cc, CMS_IDLE);
			cs_set_state(cc, CCS_UNUSABLE);
		}
		assure(!ct->bind.tst);
		/* 
		   force unbind from listening call test stream */
		if ((cc = xchg(&ct->bind.tst, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		assure(!ct->bind.mgm);
		/* 
		   force unbind from listening management stream */
		if ((cc = xchg(&ct->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		assure(!ct->bind.mnt);
		/* 
		   force unbind from listening maintenance stream */
		if ((cc = xchg(&ct->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		assure(!ct->bind.icc);
		/* 
		   force unbind from listening call control stream */
		if ((cc = xchg(&ct->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		assure(!ct->bind.ogc);
		/* 
		   force unbind from outgoing call control streams */
		while ((cc = ct->bind.ogc)) {
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&ct->refcnt) > 1, ct_get(ct));
			ct_put(xchg(&cc->bind.u.ct, NULL));
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		assure(!ct->idle.next);
		/* 
		   remove from trunk group idle list */
		if ((*ct->idle.prev = ct->idle.next))
			ct->idle.next->idle.prev = ct->idle.prev;
		ct->idle.next = NULL;
		ct->idle.prev = &ct->idle.next;
		/* 
		   (don't do reference counts on idle list) */
		/* 
		   remove from trunk group list */
		if (ct->tg.tg) {
			if ((*ct->tg.prev = ct->tg.next))
				ct->tg.next->tg.prev = ct->tg.prev;
			ct->tg.next = NULL;
			ct->tg.prev = &ct->tg.next;
			tg_put(xchg(&ct->tg.tg, NULL));
			ensure(atomic_read(&ct->refcnt) > 1, ct_get(ct));
			ct_put(ct);
		}
		/* 
		   remove from circuit group list */
		if (ct->cg.cg) {
			if ((*ct->cg.prev = ct->cg.next))
				ct->cg.next->cg.prev = ct->cg.prev;
			ct->cg.next = NULL;
			ct->cg.prev = &ct->cg.next;
			cg_put(xchg(&ct->cg.cg, NULL));
			ensure(atomic_read(&ct->refcnt) > 1, ct_get(ct));
			ct_put(ct);
		}
		/* 
		   remove from signalling relation list */
		if (ct->sr.sr) {
			if ((*ct->sr.prev = ct->sr.next))
				ct->sr.next->sr.prev = ct->sr.prev;
			ct->sr.next = NULL;
			ct->sr.prev = &ct->sr.next;
			sr_put(xchg(&ct->sr.sr, NULL));
			ensure(atomic_read(&ct->refcnt) > 1, ct_get(ct));
			ct_put(ct);
		}
		/* 
		   remove from signalling point list */
		if (ct->sp.sp) {
			if ((*ct->sp.prev = ct->sp.next))
				ct->sp.next->sp.prev = ct->sp.prev;
			ct->sp.next = NULL;
			ct->sp.prev = &ct->sp.next;
			sp_put(xchg(&ct->sp.sp, NULL));
			ensure(atomic_read(&ct->refcnt) > 1, ct_get(ct));
			ct_put(ct);
		}
		/* 
		   remove from master list */
		if ((*ct->prev = ct->next))
			ct->next->prev = ct->prev;
		ct->next = NULL;
		ct->prev = &ct->next;
		ensure(atomic_read(&ct->refcnt) > 1, ct_get(ct));
		ct_put(ct);
		assure(master.ct.numb > 0);
		master.ct.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&ct->refcnt) != 1) {
			__printd(("%s: %p: ERROR: ct lingering reference count = %d\n",
				  DRV_NAME, ct, atomic_read(&ct->refcnt)));
			atomic_set(&ct->refcnt, 1);
		}

	}
	spin_unlock_irqrestore(&ct->lock, flags);
	ct_put(ct);		/* final put */
	return;
}
STATIC struct ct *
ct_get(struct ct *ct)
{
	assure(ct);
	if (ct)
		atomic_inc(&ct->refcnt);
	return (ct);
}
STATIC void
ct_put(struct ct *ct)
{
	assure(ct);
	if (ct) {
		assure(atomic_read(&ct->refcnt) > 1);
		if (atomic_dec_and_test(&ct->refcnt)) {
			kmem_cache_free(isup_ct_cachep, ct);
			printd(("%s: %s: %p: deallocated ct structure\n", DRV_NAME,
				__FUNCTION__, ct));
		}
	}
}
STATIC struct ct *
ct_lookup(ulong id)
{
	struct ct *ct;
	for (ct = master.ct.list; ct && ct->id != id; ct = ct->next) ;
	return (ct);
}
STATIC ulong
ct_get_id(ulong id)
{
	struct ct *ct;
	if (!id) {
		id = 1;
		for (ct = master.ct.list; ct; ct = ct->next)
			if (ct->id == id)
				id++;
			else if (ct->id > id)
				break;
	}
	return (id);
}

/*
 *  CG - Circuit Group
 *  -----------------------------------
 *  Circuit group structure allocation, deallocation and reference counting
 */
STATIC struct cg *
isup_alloc_cg(ulong id, struct sr *sr)
{
	struct cg *cg, **cgp;
	printd(("%s: %s: create cg->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((cg = kmem_cache_alloc(isup_cg_cachep, SLAB_ATOMIC))) {
		bzero(cg, sizeof(*cg));
		cg_get(cg);	/* first get */
		spin_lock_init(&cg->lock);	/* "cg-lock" */
		cg->id = id;
		switch (sr->proto.pvar & SS7_PVAR_MASK) {
		default:
		case SS7_PVAR_ITUT:
			cg->config = itu_cg_config_defaults;
			break;
		case SS7_PVAR_ANSI:
			cg->config = ansi_cg_config_defaults;
			break;
		}
		/* 
		   place in master list (ascending id) */
		for (cgp = &master.cg.list; *cgp && (*cgp)->id < id; cgp = &(*cgp)->next) ;
		if ((cg->next = *cgp))
			cg->next->prev = &cg->next;
		cg->prev = cgp;
		*cgp = cg_get(cg);
		master.cg.numb++;
		/* 
		   place in signalling relation list (any order) */
		cg->sr.sr = sr_get(sr);
		if ((cg->sr.next = sr->cg.list))
			cg->sr.next->sr.prev = &cg->sr.next;
		cg->sr.prev = &sr->cg.list;
		sr->cg.list = cg_get(cg);
		sr->cg.numb++;
		/* 
		   place in signalling point list (any order) */
		cg->sp.sp = sp_get(sr->sp.sp);
		if ((cg->sp.next = sr->sp.sp->cg.list))
			cg->sp.next->sr.prev = &cg->sp.next;
		cg->sp.prev = &sr->sp.sp->cg.list;
		sr->sp.sp->cg.list = cg_get(cg);
		sr->sp.sp->cg.numb++;
		/* 
		   reset states */
		cg->g_state = CMS_IDLE;
		cg->gmg.next = NULL;
		cg->gmg.prev = &cg->gmg.next;
	} else
		printd(("%s: %s: ERROR: failed to allocate cg structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (cg);
}
STATIC void
isup_free_cg(struct cg *cg)
{
	struct cc *cc;
	psw_t flags;
	ensure(cg, return);
	printd(("%s: %s: %p free cg->id = %ld\n", DRV_NAME, __FUNCTION__, cg, cg->id));
	spin_lock_irqsave(&cg->lock, flags);
	{
		/* 
		   stop all timers */
		__cg_timer_stop(cg, tall);
		/* 
		   free all circuits */
		while (cg->ct.list)
			isup_free_ct(cg->ct.list);
		/* 
		   remove from active group management action */
		if ((cc = cg->gmg.cc)) {
			/* 
			   We are currently engaged in management on the circuit group.  This only
			   occurs when we are removing with force.  We must M_HANGUP the stream so
			   that it knows not to expect anything more from us. */
			m_error(NULL, cc, EPIPE);
			cg_set_m_state(cg, cc, CMS_IDLE);
			cs_set_state(cc, CCS_UNUSABLE);
		}
		/* 
		   force unbind listening call test streams */
		if ((cc = xchg(&cg->bind.tst, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening managment streams */
		if ((cc = xchg(&cg->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening maintenance streams */
		if ((cc = xchg(&cg->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening call control streams */
		if ((cc = xchg(&cg->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind all outgoing call control streams */
		while ((cc = cg->bind.ogc)) {
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&cg->refcnt) > 1, cg_get(cg));
			cg_put(xchg(&cc->bind.u.cg, NULL));
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		if (cg->sp.sp) {
			/* 
			   remove from signalling point list */
			if ((*cg->sp.prev = cg->sp.next))
				cg->sp.next->sp.prev = cg->sp.prev;
			cg->sp.next = NULL;
			cg->sp.prev = &cg->sp.next;
			sp_put(xchg(&cg->sp.sp, NULL));
			ensure(atomic_read(&cg->refcnt) > 1, cg_get(cg));
			cg_put(cg);
		}
		if (cg->sr.sr) {
			/* 
			   remove from signalling relation list */
			if ((*cg->sr.prev = cg->sr.next))
				cg->sr.next->sr.prev = cg->sr.prev;
			cg->sr.next = NULL;
			cg->sr.prev = &cg->sr.next;
			sr_put(xchg(&cg->sr.sr, NULL));
			ensure(atomic_read(&cg->refcnt) > 1, cg_get(cg));
			cg_put(cg);
		}
		/* 
		   remove from master list */
		if ((*cg->prev = cg->next))
			cg->next->prev = cg->prev;
		cg->next = NULL;
		cg->prev = &cg->next;
		ensure(atomic_read(&cg->refcnt) > 1, cg_get(cg));
		cg_put(cg);
		assure(master.cg.numb > 0);
		master.cg.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&cg->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: cg lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, cg, atomic_read(&cg->refcnt)));
			atomic_set(&cg->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&cg->lock, flags);
	cg_put(cg);		/* final put */
	return;
}
STATIC struct cg *
cg_get(struct cg *cg)
{
	assure(cg);
	if (cg)
		atomic_inc(&cg->refcnt);
	return (cg);
}
STATIC void
cg_put(struct cg *cg)
{
	assure(cg);
	if (cg) {
		assure(atomic_read(&cg->refcnt) > 1);
		if (atomic_dec_and_test(&cg->refcnt)) {
			kmem_cache_free(isup_cg_cachep, cg);
			printd(("%s: %s: %p: deallocated cg structure\n", DRV_NAME,
				__FUNCTION__, cg));
		}
	}
}
STATIC struct cg *
cg_lookup(ulong id)
{
	struct cg *cg;
	for (cg = master.cg.list; cg && cg->id != id; cg = cg->next) ;
	return (cg);
}
STATIC ulong
cg_get_id(ulong id)
{
	struct cg *cg;
	if (!id) {
		id = 1;
		for (cg = master.cg.list; cg; cg = cg->next)
			if (cg->id == id)
				id++;
			else if (cg->id > id)
				break;
	}
	return (id);
}

/*
 *  TG - Trunk Group
 *  -----------------------------------
 *  Trunk group structure allocation, deallocation and reference counting
 */
STATIC struct tg *
isup_alloc_tg(ulong id, struct sr *sr)
{
	struct tg *tg, **tgp;
	printd(("%s: %s: create tg->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((tg = kmem_cache_alloc(isup_tg_cachep, SLAB_ATOMIC))) {
		bzero(tg, sizeof(*tg));
		tg_get(tg);	/* first get */
		spin_lock_init(&tg->lock);	/* "tg-lock" */
		tg->id = id;
		tg->proto.pvar = SS7_PVAR_ITUT_96;
		tg->proto.popt = 0;
		switch (sr->proto.pvar & SS7_PVAR_MASK) {
		default:
		case SS7_PVAR_ITUT:
			tg->config = itu_tg_config_defaults;
			break;
		case SS7_PVAR_ANSI:
			tg->config = ansi_tg_config_defaults;
			break;
		}
		for (tgp = &master.tg.list; *tgp && (*tgp)->id < id; tgp = &(*tgp)->next) ;
		if ((tg->next = *tgp))
			tg->next->prev = &tg->next;
		tg->prev = tgp;
		*tgp = tg_get(tg);
		master.tg.numb++;
		tg->sr.sr = sr_get(sr);
		if ((tg->sr.next = sr->tg.list))
			tg->sr.next->sr.prev = &tg->sr.next;
		tg->sr.prev = &sr->tg.list;
		sr->tg.list = tg_get(tg);
		sr->tg.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate tg structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (tg);
}
STATIC void
isup_free_tg(struct tg *tg)
{
	struct cc *cc;
	psw_t flags;
	ensure(tg, return);
	printd(("%s: %s: %p free tg->id = %ld\n", DRV_NAME, __FUNCTION__, tg, tg->id));
	spin_lock_irqsave(&tg->lock, flags);
	{
		/* 
		   free all circuits */
		while (tg->ct.list)
			isup_free_ct(tg->ct.list);
		assure(tg->idle == NULL);
		/* 
		   force unbind listening call test streams */
		if ((cc = xchg(&tg->bind.tst, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening managment streams */
		if ((cc = xchg(&tg->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening maintenance streams */
		if ((cc = xchg(&tg->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind listening call control streams */
		if ((cc = xchg(&tg->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
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
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		if (tg->sr.sr) {
			/* 
			   remove from signalling reference list */
			if ((*tg->sr.prev = tg->sr.next))
				tg->sr.next->sr.prev = tg->sr.prev;
			tg->sr.next = NULL;
			tg->sr.prev = &tg->sr.next;
			sr_put(xchg(&tg->sr.sr, NULL));
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
			kmem_cache_free(isup_tg_cachep, tg);
			printd(("%s: %s: %p: deallocated tg structure\n", DRV_NAME,
				__FUNCTION__, tg));
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
 *  SR - Signalling Relation
 *  -----------------------------------
 *  Signalling relation structure allocation, deallocation and reference counting
 */
STATIC struct sr *
isup_alloc_sr(ulong id, struct sp *sp, mtp_addr_t * add)
{
	struct sr *sr, **srp;
	printd(("%s: %s: create sr->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((sr = kmem_cache_alloc(isup_sr_cachep, SLAB_ATOMIC))) {
		bzero(sr, sizeof(*sr));
		sr_get(sr);	/* first get */
		spin_lock_init(&sr->lock);	/* "sr-lock" */
		sr->id = id;
		/* 
		   add to master list */
		for (srp = &master.sr.list; *srp && (*srp)->id < id; srp = &(*srp)->next) ;
		if ((sr->next = *srp))
			sr->next->prev = &sr->next;
		sr->prev = srp;
		*srp = sr_get(sr);
		master.sr.numb++;
		/* 
		   add to signalling point list of signalling relations */
		sr->sp.sp = sp_get(sp);
		if ((sr->sp.next = sp->sr.list))
			sr->sp.next->sp.prev = &sr->sp.next;
		sr->sp.prev = &sp->sr.list;
		sp->sr.list = sr_get(sr);
		sp->sr.numb++;
		sr->add = *add;
		sr->proto = sp->proto;	/* as a default */
		switch (sr->proto.pvar & SS7_PVAR_MASK) {
		default:
		case SS7_PVAR_ITUT:
			sr->config = itu_sr_config_defaults;
			break;
		case SS7_PVAR_ANSI:
			sr->config = ansi_sr_config_defaults;
			break;
		}
	} else
		printd(("%s: %s: ERROR: failed to allocate sr structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (sr);
}
STATIC void
isup_free_sr(struct sr *sr)
{
	struct cc *cc;
	psw_t flags;
	ensure(sr, return);
	printd(("%s: %s: %p free sr->id = %ld\n", DRV_NAME, __FUNCTION__, sr, sr->id));
	spin_lock_irqsave(&sr->lock, flags);
	{
		/* 
		   stop all timers */
		__sr_timer_stop(sr, tall);
		/* 
		   free all trunk groups */
		while (sr->tg.list)
			isup_free_tg(sr->tg.list);
		/* 
		   free all circuit groups */
		while (sr->cg.list)
			isup_free_cg(sr->cg.list);
		assure(sr->ct.list == NULL);
		/* 
		   unlink from message transfer part */
		if (sr->mtp) {
			sr_put(xchg(&sr->mtp->sr, NULL));
			mtp_put(xchg(&sr->mtp, NULL));
		}
		/* 
		   force unbind from listening call test stream */
		if ((cc = xchg(&sr->bind.tst, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening management stream */
		if ((cc = xchg(&sr->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening maintennce stream */
		if ((cc = xchg(&sr->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening call control stream */
		if ((cc = xchg(&sr->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from outgoing call control streams */
		while ((cc = sr->bind.ogc)) {
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&sr->refcnt) > 1, sr_get(sr));
			sr_put(xchg(&cc->bind.u.sr, NULL));
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   remove from signalling point list */
		if (sr->sp.sp) {
			if ((*sr->sp.prev = sr->sp.next))
				sr->sp.next->sp.prev = sr->sp.prev;
			sr->sp.next = NULL;
			sr->sp.prev = &sr->sp.next;
			sp_put(xchg(&sr->sp.sp, NULL));
			ensure(atomic_read(&sr->refcnt) > 1, sr_get(sr));
			sr_put(sr);
		}
		/* 
		   remove from master list */
		if ((*sr->prev = sr->next))
			sr->next->prev = sr->prev;
		sr->next = NULL;
		sr->prev = &sr->next;
		ensure(atomic_read(&sr->refcnt) > 1, sr_get(sr));
		sr_put(sr);
		assure(master.sr.numb > 0);
		master.sr.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&sr->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: sr lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, sr, atomic_read(&sr->refcnt)));
			atomic_set(&sr->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&sr->lock, flags);
	sr_put(sr);		/* final put */
}
STATIC struct sr *
sr_get(struct sr *sr)
{
	assure(sr);
	if (sr)
		atomic_inc(&sr->refcnt);
	return (sr);
}
STATIC void
sr_put(struct sr *sr)
{
	assure(sr);
	if (sr) {
		assure(atomic_read(&sr->refcnt) > 1);
		if (atomic_dec_and_test(&sr->refcnt)) {
			kmem_cache_free(isup_sr_cachep, sr);
			printd(("%s: %s: %p: deallocated sr structure\n", DRV_NAME,
				__FUNCTION__, sr));
		}
	}
}
STATIC struct sr *
sr_lookup(ulong id)
{
	struct sr *sr;
	for (sr = master.sr.list; sr && sr->id != id; sr = sr->next) ;
	return (sr);
}
STATIC ulong
sr_get_id(ulong id)
{
	struct sr *sr;
	if (!id) {
		id = 1;
		for (sr = master.sr.list; sr; sr = sr->next)
			if (sr->id == id)
				id++;
			else if (sr->id > id)
				break;
	}
	return (id);
}

/*
 *  SP - Signalling Point
 *  -----------------------------------
 *  Signalling point structure allocation, deallocation and reference counting
 */
STATIC struct sp *
isup_alloc_sp(ulong id, mtp_addr_t * add)
{
	struct sp *sp, **spp;
	printd(("%s: %s: create sp->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((sp = kmem_cache_alloc(isup_sp_cachep, SLAB_ATOMIC))) {
		bzero(sp, sizeof(*sp));
		sp_get(sp);	/* first get */
		spin_lock_init(&sp->lock);	/* "sp-lock" */
		sp->id = id;
		sp->add = *add;
		for (spp = &master.sp.list; *spp && (*spp)->id < id; spp = &(*spp)->next) ;
		if ((sp->next = *spp))
			sp->next->prev = &sp->next;
		sp->prev = spp;
		*spp = sp_get(sp);
		master.sp.numb++;
		todo(("Search I_LINKed MTP for match\n"));
		/* 
		   for now, make management establish SP before I_LINKing MTP streams */
	} else
		printd(("%s: %s: ERROR: failed to allocate sp structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (sp);
}
STATIC void
isup_free_sp(struct sp *sp)
{
	struct cc *cc;
	psw_t flags;
	ensure(sp, return);
	printd(("%s: %s: %p free sp->id = %ld\n", DRV_NAME, __FUNCTION__, sp, sp->id));
	spin_lock_irqsave(&sp->lock, flags);
	{
		/* 
		   freeing all signalling relations */
		while (sp->sr.list)
			isup_free_sr(sp->sr.list);
		assure(sp->tg.list == NULL);
		assure(sp->cg.list == NULL);
		assure(sp->ct.list == NULL);
		/* 
		   unlink from message transfer part */
		if (sp->mtp) {
			sp_put(xchg(&sp->mtp->sp, NULL));
			mtp_put(xchg(&sp->mtp, NULL));
		}
		/* 
		   force unbind from listening call test stream */
		if ((cc = xchg(&sp->bind.tst, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening management stream */
		if ((cc = xchg(&sp->bind.mgm, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening maintenance stream */
		if ((cc = xchg(&sp->bind.mnt, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from listening call control stream */
		if ((cc = xchg(&sp->bind.icc, NULL))) {
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   force unbind from outgoing call control streams */
		while ((cc = sp->bind.ogc)) {
			if ((*cc->bind.prev = cc->bind.next))
				cc->bind.next->bind.prev = cc->bind.prev;
			cc->bind.next = NULL;
			cc->bind.prev = &cc->bind.next;
			ensure(atomic_read(&sp->refcnt) > 1, sp_get(sp));
			sp_put(xchg(&cc->bind.u.sp, NULL));
			m_error(NULL, cc, EPIPE);
			cs_set_state(cc, CCS_UNUSABLE);
			cc_put(cc);
		}
		/* 
		   remove from master list */
		if ((*sp->prev = sp->next))
			sp->next->prev = sp->prev;
		sp->next = NULL;
		sp->prev = &sp->next;
		ensure(atomic_read(&sp->refcnt) > 1, sp_get(sp));
		sp_put(sp);
		assure(master.sp.numb > 0);
		master.sp.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&sp->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: sp lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, sp, atomic_read(&sp->refcnt)));
			atomic_set(&sp->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&sp->lock, flags);
	sp_put(sp);		/* final put */
}
STATIC struct sp *
sp_get(struct sp *sp)
{
	assure(sp);
	if (sp)
		atomic_inc(&sp->refcnt);
	return (sp);
}
STATIC void
sp_put(struct sp *sp)
{
	assure(sp);
	if (sp) {
		assure(atomic_read(&sp->refcnt) > 1);
		if (atomic_dec_and_test(&sp->refcnt)) {
			kmem_cache_free(isup_sp_cachep, sp);
			printd(("%s: %s: %p: deallocated sp structure\n", DRV_NAME,
				__FUNCTION__, sp));
		}
	}
}
STATIC struct sp *
sp_lookup(ulong id)
{
	struct sp *sp;
	for (sp = master.sp.list; sp && sp->id != id; sp = sp->next) ;
	return (sp);
}
STATIC ulong
sp_get_id(ulong id)
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

/*
 *  MTP = Message Transfer Part
 *  -----------------------------------
 *  Lower stream private structure allocation, deallocation and reference counting
 */
STATIC struct mtp *
isup_alloc_mtp(queue_t *q, struct mtp **mpp, ulong index, cred_t *crp)
{
	struct mtp *mtp;
	printd(("%s: %s: create mtp index = %lu\n", DRV_NAME, __FUNCTION__, index));
	if ((mtp = kmem_cache_alloc(isup_mtp_cachep, SLAB_ATOMIC))) {
		bzero(mtp, sizeof(*mtp));
		mtp_get(mtp);	/* first get */
		mtp->u.mux.index = index;
		mtp->cred = *crp;
		spin_lock_init(&mtp->qlock);	/* "mtp-queue-lock" */
		(mtp->iq = RD(q))->q_ptr = mtp_get(mtp);
		(mtp->oq = WR(q))->q_ptr = mtp_get(mtp);
		mtp->o_prim = mtp_w_prim;
		mtp->i_prim = mtp_r_prim;
		mtp->o_wakeup = NULL;
		mtp->i_wakeup = NULL;
		mtp->i_state = LMI_UNUSABLE;
		mtp->i_style = LMI_STYLE1;
		mtp->i_version = 1;
		spin_lock_init(&mtp->lock);	/* "mtp-lock" */
		/* 
		   place in master list */
		if ((mtp->next = *mpp))
			mtp->next->prev = &mtp->next;
		mtp->prev = mpp;
		*mpp = mtp_get(mtp);
		master.mtp.numb++;
	} else
		printd(("%s: %s: ERROR: failed to allocate mtp structure %lu\n", DRV_NAME,
			__FUNCTION__, index));
	return (mtp);
}
STATIC void
isup_free_mtp(queue_t *q)
{
	struct mtp *mtp = MTP_PRIV(q);
	psw_t flags;
	ensure(mtp, return);
	printd(("%s: %s: %p free mtp index = %lu\n", DRV_NAME, __FUNCTION__, mtp,
		mtp->u.mux.index));
	spin_lock_irqsave(&mtp->lock, flags);
	{
		/* 
		   flushing buffers */
		ss7_unbufcall((str_t *) mtp);
		flushq(mtp->iq, FLUSHALL);
		flushq(mtp->oq, FLUSHALL);
		/* 
		   unlink from signalling relation */
		if (mtp->sr) {
			struct sr *sr = mtp->sr;
			if (!(sr->flags & CCTF_LOC_S_BLOCKED)) {
				/* 
				   software block all circuits */
				struct ct *ct;
				for (ct = sr->ct.list; ct; ct = ct->sr.next) {
					ct_set(ct, CCTF_LOC_S_BLOCKED);
				}
				sr->flags |= CCTF_LOC_S_BLOCKED;
			}
			mtp_put(xchg(&mtp->sr->mtp, NULL));
			sr_put(xchg(&mtp->sr, NULL));
		}
		/* 
		   unlink from signalling point */
		if (mtp->sp) {
			struct sr *sr;
			for (sr = mtp->sp->sr.list; sr; sr = sr->sp.next) {
				if (!(sr->flags & CCTF_LOC_S_BLOCKED)) {
					/* 
					   software block all circuits */
					struct ct *ct;
					for (ct = sr->ct.list; ct; ct = ct->sr.next) {
						ct_set(ct, CCTF_LOC_S_BLOCKED);
					}
					sr->flags |= CCTF_LOC_S_BLOCKED;
				}
			}
			mtp_put(xchg(&mtp->sp->mtp, NULL));
			sp_put(xchg(&mtp->sp, NULL));
		}
		/* 
		   remote from master list */
		if ((*mtp->prev = mtp->next))
			mtp->next->prev = mtp->prev;
		mtp->next = NULL;
		mtp->prev = &mtp->next;
		ensure(atomic_read(&mtp->refcnt) > 1, mtp_get(mtp));
		mtp_put(mtp);
		assure(master.mtp.numb > 0);
		master.mtp.numb--;
		/* 
		   remove from queues */
		ensure(atomic_read(&mtp->refcnt) > 1, mtp_get(mtp));
		mtp_put(xchg(&mtp->iq->q_ptr, NULL));
		ensure(atomic_read(&mtp->refcnt) > 1, mtp_get(mtp));
		mtp_put(xchg(&mtp->oq->q_ptr, NULL));
		/* 
		   done, check final count */
		if (atomic_read(&mtp->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: mtp lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, mtp, atomic_read(&mtp->refcnt)));
			atomic_set(&mtp->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&mtp->lock, flags);
	mtp_put(mtp);		/* final put */
}
STATIC struct mtp *
mtp_get(struct mtp *mtp)
{
	assure(mtp);
	if (mtp)
		atomic_inc(&mtp->refcnt);
	return (mtp);
}
STATIC void
mtp_put(struct mtp *mtp)
{
	assure(mtp);
	if (mtp) {
		assure(atomic_read(&mtp->refcnt) > 1);
		if (atomic_dec_and_test(&mtp->refcnt)) {
			kmem_cache_free(isup_mtp_cachep, mtp);
			printd(("%s: %s: %p: deallocated mtp structure\n", DRV_NAME,
				__FUNCTION__, mtp));
		}
	}
}
STATIC struct mtp *
mtp_lookup(ulong id)
{
	struct mtp *mtp;
	for (mtp = master.mtp.list; mtp && mtp->id != id; mtp = mtp->next) ;
	return (mtp);
}
STATIC ulong
mtp_get_id(ulong id)
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
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the ISUP driver. (0 for allocation.)");

major_t major = CMAJOR_0;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the ISUP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw isup_cdev = {
	.d_name = DRV_NAME,
	.d_str = &isupinfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
isup_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&isup_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
isup_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&isup_cdev, major)) < 0)
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
isup_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &isupinfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
isup_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
isupterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (isup_majors[mindex]) {
			if ((err = isup_unregister_strdev(isup_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					isup_majors[mindex]);
			if (mindex)
				isup_majors[mindex] = 0;
		}
	}
	if ((err = isup_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
isupinit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = isup_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		isupterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = isup_register_strdev(isup_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					isup_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				isupterminate();
				return (err);
			}
		}
		if (isup_majors[mindex] == 0)
			isup_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(isup_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = isup_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(isupinit);
module_exit(isupterminate);

#endif				/* LINUX */
