/*****************************************************************************

 @(#) $RCSfile: m2ua.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2008-09-10 03:49:16 $

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

 Last Modified $Date: 2008-09-10 03:49:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m2ua.c,v $
 Revision 0.9.2.11  2008-09-10 03:49:16  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.10  2008-05-05 15:34:49  brian
 - be strict with MORE_data and DATA_flag

 Revision 0.9.2.9  2008-04-29 01:52:20  brian
 - updated headers for release

 Revision 0.9.2.8  2007/08/15 05:13:38  brian
 - GPLv3 updates

 Revision 0.9.2.7  2007/08/14 08:33:50  brian
 - GPLv3 header update

 Revision 0.9.2.6  2007/07/14 01:33:26  brian
 - make license explicit, add documentation

 Revision 0.9.2.5  2007/03/25 18:58:34  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.4  2007/03/25 02:22:24  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.3  2007/03/25 00:51:04  brian
 - synchronization updates

 Revision 0.9.2.2  2006/11/04 11:35:25  brian
 - open source release of commercial package

 Revision 0.9.2.1  2006/10/17 11:55:41  brian
 - copied files into new packages from strss7 package

 Revision 0.9.2.15  2006/05/08 11:00:53  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.14  2006/03/07 01:10:04  brian
 - binary compatible callouts

 Revision 0.9.2.13  2006/03/04 13:00:10  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: m2ua.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2008-09-10 03:49:16 $"

static char const ident[] =
    "$RCSfile: m2ua.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2008-09-10 03:49:16 $";

#define _LFS_SOURCE 1

#include <sys/os7/compat.h>
#include <linux/socket.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>
#include <ss7/m2ua_ioctl.h>
#include <sys/tihdr.h>
#include <sys/xti.h>
#include <sys/xti_sctp.h>

#define M2UA_DESCRIP	"SS7 MTP2 USER ADAPTATION (M2UA) STREAMS MULTIPLEXING DRIVER."
#define M2UA_REVISION	"LfS $RCSfile: m2ua.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2008-09-10 03:49:16 $"
#define M2UA_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define M2UA_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define M2UA_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define M2UA_LICENSE	"GPL"
#define M2UA_BANNER	M2UA_DESCRIP	"\n" \
			M2UA_REVISION	"\n" \
			M2UA_COPYRIGHT	"\n" \
			M2UA_DEVICE	"\n" \
			M2UA_CONTACT

#ifdef LINUX
MODULE_AUTHOR(M2UA_CONTACT);
MODULE_DESCRIPTION(M2UA_DESCRIP);
MODULE_SUPPORTED_DEVICE(M2UA_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M2UA_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-m2ua");
#endif
#endif				/* LINUX */

#ifdef LFS
#define M2UA_DRV_ID		CONFIG_STREAMS_M2UA_MODID
#define M2UA_DRV_NAME		CONFIG_STREAMS_M2UA_NAME
#define M2UA_CMAJORS		CONFIG_STREAMS_M2UA_NMAJORS
#define M2UA_CMAJOR_0		CONFIG_STREAMS_M2UA_MAJOR
#define M2UA_UNITS		CONFIG_STREAMS_M2UA_NMINORS
#endif

/* Lock debugging. */

#ifdef _DEBUG
#define spin_lock_m2ua(lock) (void)lock
#define spin_unlock_m2ua(lock) (void)lock
#else
#define spin_lock_m2ua(lock) spin_lock_bh(lock)
#define spin_unlock_m2ua(lock) spin_unlock_bh(lock)
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		M2UA_DRV_ID
#define DRV_NAME	M2UA_DRV_NAME
#define CMAJORS		M2UA_CMAJORS
#define CMAJOR_0	M2UA_CMAJOR_0
#define UNITS		M2UA_UNITS
#ifdef MODULE
#define DRV_BANNER	M2UA_BANNER
#else				/* MODULE */
#define DRV_BANNER	M2UA_SPLASH
#endif				/* MODULE */

STATIC struct module_info m2ua_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-wr",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size expected */
	mi_maxpsz:272 + 1,		/* Max packet size expected */
	mi_hiwat:STRHIGH,		/* Hi water mark */
	mi_lowat:STRLOW,		/* Lo water mark */
};
STATIC struct module_info m2ua_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-rd",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size expected */
	mi_maxpsz:272 + 1,		/* Max packet size expected */
	mi_hiwat:STRHIGH,		/* Hi water mark */
	mi_lowat:STRLOW,		/* Lo water mark */
};
STATIC struct module_info mux_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-mxw",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size expected */
	mi_maxpsz:272 + 1,		/* Max packet size expected */
	mi_hiwat:STRHIGH,		/* Hi water mark */
	mi_lowat:STRLOW,		/* Lo water mark */
};
STATIC struct module_info mux_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-mxr",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size expected */
	mi_maxpsz:272 + 1,		/* Max packet size expected */
	mi_hiwat:STRHIGH,		/* Hi water mark */
	mi_lowat:STRLOW,		/* Lo water mark */
};

STATIC streamscall int m2ua_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int m2ua_close(queue_t *, int, cred_t *);

STATIC streamscall int m2ua_oput(queue_t *, mblk_t *);
STATIC streamscall int m2ua_osrv(queue_t *);

STATIC struct qinit m2ua_rinit = {
	qi_putp:m2ua_oput,		/* Read put (message from below) */
	qi_srvp:m2ua_osrv,		/* Read queue service */
	qi_qopen:m2ua_open,		/* Each open */
	qi_qclose:m2ua_close,		/* Last close */
	qi_minfo:&m2ua_rinfo,		/* Information */
};

STATIC struct qinit m2ua_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&m2ua_winfo,		/* Information */
};

STATIC struct qinit mux_rinit = {
	qi_putp:ss7_iput,		/* Read put (message from below) */
	qi_srvp:ss7_isrv,		/* Read queue service */
	qi_minfo:&mux_rinfo,		/* Information */
};

STATIC struct qinit mux_winit = {
	qi_putp:m2ua_oput,		/* Write put (message from above) */
	qi_srvp:m2ua_osrv,		/* Write queue service */
	qi_minfo:&mux_winfo,		/* Information */
};

MODULE_STATIC struct streamtab m2uainfo = {
	st_rdinit:&m2ua_rinit,		/* Upper read queue */
	st_wrinit:&m2ua_winit,		/* Upper write queue */
	st_muxrinit:&mux_rinit,		/* Lower read queue */
	st_muxwinit:&mux_winit,		/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private Structures
 *
 *  =========================================================================
 */

#define AS_DOWN		0	/* the AS is down */
#define AS_INACTIVE	1	/* the AS is inactive */
#define AS_WACK_ASPAC	2	/* the AS is waiting to activate */
#define AS_WACK_ASPIA	3	/* the AS is waiting to deactivate */
#define AS_ACTIVE	4	/* the AS is active */

/*
   generic linkage 
 */
#define SLIST_COUNT(__s) \
	struct { \
		struct __s *list; \
		size_t numb;				/* number of elements in list */ \
		union { \
			struct { \
				size_t down;		/* number of elements down in list */ \
				size_t inactive;	/* number of elements inactive in list */ \
				size_t wack_aspac;	/* number of elements activating in list */ \
				size_t wack_aspia;	/* number of elements deactivating in list */ \
				size_t active;		/* number of elements active in list */ \
			} c; \
			size_t counts[AS_ACTIVE + 1]; \
		} u; \
	} __s \

/*
   forward declarations 
 */
struct df;				/* DF structure */
struct as;				/* AS structure */
struct ap;				/* AG (AS-U to AS-P mapping) structure */
struct gp;				/* GP (AS to SPP mapping) structure */
struct sp;				/* SP structure */
struct np;				/* NP (SP-U to SP-P (SG) mapping) structure */
struct spp;				/* SPP proxy */
struct sl;				/* SL structure */

/*
 *  AS Structure
 *  -----------------------------------
 *  The AS-U structure represents each AS which is currently being served to
 *  remote M2UA ASPs via the AP map or to local SL users via the SL-U.  An
 *  AS-U belongs to a local SP (local SGP/remote ASP relation) and has service
 *  provided by a map of AS-P.
 *
 *  The AS-P structure represents each AS which is currently being provided by
 *  remote M2UA SGPs via the GP map or by local SL providers via the SL-P.  An
 *  AS-P belongs to a local SG (local ASP/remote SG relation) and provides
 *  service to a map of AS-U.
 */
struct as {
	HEAD_DECLARATION (struct as);	/* head delcaration */
	uint32_t iid;			/* Interface Identifier */
	size_t minasp;			/* minimum number of ASPs */
	m2ua_addr_t add;		/* SL address */
	SLIST_HEAD (sl, sl);		/* SL for this AS */
	SLIST_COUNT (ap);		/* AP (AS graph) list */
	SLIST_LINKAGE (sp, as, sp);	/* SP linkage */
	SLIST_COUNT (gp);		/* GP (SPP graph) list */
	m2ua_timers_as_t timers;	/* Application server timers */
	m2ua_opt_conf_as_t config;	/* Application server configuration */
	m2ua_stats_as_t stats;		/* Application server statistics */
	m2ua_stats_as_t statsp;		/* Application server statistics periods */
};

#define ASF_INSUFFICIENT_ASPS	(1<< 0)	/* AS has insufficient asps */
#define ASF_MINIMUM_ASPS	(1<< 1)	/* AS has minmum number of asps */
#define ASF_PENDING		(1<< 2)	/* AS pending */
#define ASF_ACTIVE		(1<< 3)	/* AS active */
#define ASF_MGMT_BLOCKED	(1<< 4)	/* AS management blocked */
#define ASF_OPERATION_PENDING	(1<< 5)	/* operation pending */
#define ASF_BSNT_REQUEST	(1<< 6)	/* bsnt requested */
#define ASF_CLEAR_RTB		(1<< 7)	/* clear rtb performed */
#define ASF_EMERGENCY		(1<< 8)	/* emergency set */
#define ASF_FLUSH_BUFFERS	(1<< 9)	/* flush buffers performed */
#define ASF_LOC_PROC_OUTAGE	(1<<10)	/* local processor outage */
#define ASF_REM_PROC_OUTAGE	(1<<11)	/* remote processor outage */
#define ASF_RECOVERY		(1<<12)	/* waiting for recovery from remote processor outage */
#define ASF_RETRIEVAL		(1<<13)	/* in the process or retrieval */
#define ASF_USR_PROC_OUTAGE	(1<<14)	/* user process outage */
#define ASF_PRV_PROC_OUTAGE	(1<<15)	/* provider process outage */
#define ASF_CONG_ACCEPT		(1<<16)	/* congestion accept */
#define ASF_CONG_DISCARD	(1<<17)	/* congestion discard */
#define ASF_WANTW		(1<<18)	/* wanted to write across multiplex */

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
 */
struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	uint32_t cost;			/* cost */
	uint32_t tmode;			/* traffic mode */
	SLIST_COUNT (np);		/* GS (SP graph) */
	SLIST_HEAD (as, as);		/* AS list */
	SLIST_COUNT (spp);		/* ASP list */
	m2ua_timers_sp_t timers;	/* Signalling point timers */
	m2ua_opt_conf_sp_t config;	/* Signalling point configuration */
	m2ua_stats_sp_t stats;		/* Signalling point statistics */
	m2ua_stats_sp_t statsp;		/* Signalling point statistics periods */
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
 */
struct spp {
	HEAD_DECLARATION (struct spp);	/* head declaration */
	uint32_t aspid;			/* ASP Id for this SPP */
	uint32_t cost;			/* cost */
	struct xp *xp;			/* XP transport for this ASP */
	SLIST_LINKAGE (sp, spp, sp);	/* SP linkage */
	SLIST_COUNT (gp);		/* GP (AS graph) */
	m2ua_timers_spp_t timers;	/* Signalling peer process timers */
	m2ua_opt_conf_spp_t config;	/* Signalling peer process configuration */
	m2ua_stats_spp_t stats;		/* Signalling peer process statistics */
	m2ua_stats_spp_t statsp;	/* Signalling peer process statistics periods */
};

/*
 *  GP (AS to SPP mapping) structure
 *  -----------------------------------
 *  The AP structure maps AS Users (AS-U) to remote ASPs that are using the
 *  AS.  It also indicates the base IID that is used by the remote ASP for
 *  this AS-U and the state of the remote ASP for this AS-U.
 *
 *  The AP structure maps AS Users (AS-P) to remote SGPs that are providing the
 *  AS.  It also indicates the base IID that is provided by the remote SGP for
 *  this AS-P and the state of the remote SGP for this AS-P.
 */
struct gp {
	uint32_t state;			/* AS state for this SPP */
	uint32_t flags;			/* AS flags for this SPP */
	uint32_t l_state;		/* signalling link state */
	uint32_t iid;			/* AS iid for this SPP */
	SLIST_LINKAGE (as, gp, as);	/* AS linkage */
	SLIST_LINKAGE (spp, gp, spp);	/* SPP linkage */
};

/*
 *  SL structure
 *  -----------------------------------
 */
struct sl {
	STR_DECLARATION (struct sl);	/* stream declaration */
	uint32_t l_state;		/* signalling link state */
	uint32_t iid;			/* interface id for this link */
	m2ua_addr_t add;		/* Signalling Link PPA */
	struct sl *ioc;			/* */
	SLIST_LINKAGE (as, sl, as);	/* AS-U linkage */
	lmi_info_ack_t info;		/* signalling link information */
	lmi_option_t proto;		/* protocol options and variant */
	m2ua_timers_sl_t timers;	/* Signalling link timers */
	m2ua_opt_conf_sl_t config;	/* Signalling link configuration */
	m2ua_stats_sl_t stats;		/* Signalling link statistics */
	m2ua_stats_sl_t statsp;		/* Signalling link statistics periods */
};

#define SL_PRIV(__q) ((struct sl *)(__q)->q_ptr)

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
	struct spp *spp;		/* Associated ASP/SGP */
	mblk_t *nm_reassem;		/* normal reassembly buffer */
	mblk_t *ex_reassem;		/* expedited reassembly buffer */
	uchar loc[MAX_TPI_ADDR_SIZE];	/* loc opaque address */
	uchar rem[MAX_TPI_ADDR_SIZE];	/* rem opaque address */
	struct T_info_ack info;		/* protocol information */
	m2ua_timers_xp_t timers;	/* Transport timers */
	m2ua_opt_conf_xp_t config;	/* Transport configuration */
	m2ua_stats_xp_t stats;		/* Transport statistics */
	m2ua_stats_xp_t statsp;		/* Transport statistics periods */
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
 *  This is a layer management structure.  It it used for M2UA management streams.
 */
struct lm {
	STR_DECLARATION (struct lm);	/* stream declaration */
};

#define MGM_PRIV(__q) ((struct lm *)(__q)->q_ptr)

struct lk {
	ushort sdti;			/* signalling data terminal identifier */
	ushort sdli;			/* signalling data link identifier */
};

union priv {
	struct str str;
	struct sl sl;
	struct lm lm;
};

#define PRIV(__q) ((union priv *)(__q)->q_ptr)

union link {
	struct str str;
	struct sl sl;
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
	m2ua_timers_df_t timers;	/* default timers */
	m2ua_opt_conf_df_t config;	/* default configuration */
	m2ua_stats_df_t stats;		/* default statistics */
	m2ua_stats_df_t statsp;		/* default statistics periods */
};

STATIC struct df master;

/*
 *  Forward declarations.
 */
STATIC union priv *m2ua_alloc_priv(queue_t *, union priv **, dev_t *, cred_t *, minor_t);
STATIC void m2ua_free_priv(queue_t *);
STATIC union priv *priv_get(union priv *);
STATIC void priv_put(union priv *);

STATIC struct as *m2ua_alloc_as(uint32_t, uint32_t, struct sp *, uint32_t, m2ua_addr_t *);
STATIC void m2ua_free_as(struct as *);
STATIC struct as *as_get(struct as *);
STATIC void as_put(struct as *);
STATIC uint32_t as_get_id(uint32_t);
STATIC struct as *as_lookup(uint32_t);

STATIC struct sp *m2ua_alloc_sp(uint32_t, uint32_t, struct sp *, uint32_t, uint32_t);
STATIC void m2ua_free_sp(struct sp *);
STATIC struct sp *sp_get(struct sp *);
STATIC void sp_put(struct sp *);
STATIC uint32_t sp_get_id(uint32_t);
STATIC struct sp *sp_lookup(uint32_t);

STATIC struct spp *m2ua_alloc_spp(uint32_t, uint32_t, struct sp *, uint32_t, uint32_t);
STATIC void m2ua_free_spp(struct spp *);
STATIC struct spp *spp_get(struct spp *);
STATIC void spp_put(struct spp *);
STATIC uint32_t spp_get_id(uint32_t);
STATIC struct spp *spp_lookup(uint32_t);

STATIC struct ap *m2ua_alloc_ap(struct as *, struct as *);
STATIC void m2ua_free_ap(struct ap *);

STATIC struct np *m2ua_alloc_np(struct sp *, struct sp *);
STATIC void m2ua_free_np(struct np *);

STATIC struct gp *m2ua_alloc_gp(uint32_t, struct as *, struct spp *);
STATIC void m2ua_free_gp(struct gp *);

STATIC void m2ua_alloc_sl(struct sl *, uint32_t, uint32_t, struct as *, uint32_t, m2ua_addr_t *);
STATIC void m2ua_free_sl(struct sl *);
STATIC struct sl *sl_get(struct sl *);
STATIC void sl_put(struct sl *);
STATIC uint32_t sl_get_id(uint32_t);
STATIC struct sl *sl_lookup(uint32_t);

STATIC void m2ua_alloc_xp(struct xp *, uint32_t, uint32_t, struct spp *, struct sp *);
STATIC void m2ua_free_xp(struct xp *);
STATIC struct xp *xp_get(struct xp *);
STATIC void xp_put(struct xp *);
STATIC uint32_t xp_get_id(uint32_t);
STATIC struct xp *xp_lookup(uint32_t);

STATIC union link *m2ua_alloc_link(queue_t *, union link **, uint32_t, cred_t *);
STATIC void m2ua_free_link(queue_t *);
STATIC union link *link_get(union link *);
STATIC void link_put(union link *);
STATIC union link *link_lookup(uint32_t);

STATIC INLINE uint32_t
sl_get_i_state(struct sl *sl)
{
	return sl->i_state;
}
STATIC INLINE void
sl_set_i_state(struct sl *sl, uint32_t state)
{
	sl->i_state = state;
}

/*
 *  =========================================================================
 *
 *  M2UA MESSAGES
 *
 *  =========================================================================
 */

struct ua_parm {
	union {
		uchar *c;		/* pointer to parameter field */
		uint32_t *w;		/* pointer to parameter field */
	} ptr;
	size_t len;			/* length of paramter field */
	uint32_t val;			/* value of first 4 bytes */
};

struct ua_msg {
	mblk_t *mp;			/* the original message block */
	uint32_t version;		/* message version */
	uint32_t class;			/* message class */
	uint32_t type;			/* message type */
	uint32_t len;			/* message length */
	struct ua_parm iid;		/* interface id */
	struct ua_parm iid_text;	/* interface id (text) */
	struct ua_parm info;		/* information */
	struct ua_parm diag;		/* diagnostic */
	struct ua_parm hinfo;		/* heartbeat information */
	struct ua_parm reason;
	struct ua_parm tmode;		/* traffic mode */
	struct ua_parm ecode;
	struct ua_parm aspid;		/* asp identifier */
	struct ua_parm data1;		/* data type 1 */
	struct ua_parm data2;		/* data type 2 */
	struct ua_parm status;		/* status */
	struct ua_parm event;		/* event */
	struct ua_parm cong;		/* congestion status */
	struct ua_parm disc;		/* discard status */
	struct ua_parm action;		/* action */
	struct ua_parm seqno;		/* sequence number */
	struct ua_parm result;		/* result */
	struct ua_parm linkkey;
	struct ua_parm keyid;
	struct ua_parm sdti;
	struct ua_parm sdli;
	struct ua_parm corid;		/* correlation id */
};

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
 *  =========================================================================
 *
 *  PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent upstream (to SL User)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, struct sl *sl, int error)
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
			printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (-error);
		} else {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = error;
			*(mp->b_wptr)++ = error;
			printd(("%s: %p: <- M_ERROR\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
	}
	return (-ENOBUFS);
}

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
slu_info_ack(queue_t *q, struct sl *sl, uchar *ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_info_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sl_get_i_state(sl);
		p->lmi_max_sdu = 272 + 1 + ((sl->proto.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((sl->proto.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: <- LMI_INFO_ACK\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
slu_ok_ack(queue_t *q, struct sl *sl, sl_long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			sl_set_i_state(sl, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			sl_set_i_state(sl, LMI_UNATTACHED);
			break;
		default:
			break;
		}
		p->lmi_state = sl_get_i_state(sl);
		printd(("%s: %p: <- LMI_OK_ACK\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
slu_error_ack(queue_t *q, struct sl *sl, sl_long prim, sl_long error)
{
	mblk_t *mp;
	lmi_error_ack_t *p;

	/* filter out queue returns */
	switch (error) {
	case QR_ABSORBED:
	case QR_TRIMMED:
	case QR_LOOP:
	case QR_PASSALONG:
	case QR_PASSFLOW:
	case QR_DISABLE:
	case QR_STRIP:
	case QR_RETRY:
	case -EBUSY:
	case -ENOBUFS:
	case -ENOMEM:
	case -EAGAIN:
		return (error);
	}
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_reason = error > 0 ? error : LMI_SYSERR;
		p->lmi_errno = error > 0 ? 0 : -error;
		p->lmi_error_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			sl_set_i_state(sl, LMI_UNATTACHED);
			break;
		case LMI_DETACH_PENDING:
			sl_set_i_state(sl, LMI_DISABLED);
			break;
		case LMI_ENABLE_PENDING:
			sl_set_i_state(sl, LMI_DISABLED);
			break;
		case LMI_DISABLE_PENDING:
			sl_set_i_state(sl, LMI_ENABLED);
			break;
		default:
			break;
		}
		p->lmi_state = sl_get_i_state(sl);
		printd(("%s: %p: <- LMI_ERROR_ACK\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
slu_enable_con(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_enable_con_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_CON;
		assure(sl_get_i_state(sl) == LMI_ENABLE_PENDING);
		sl_set_i_state(sl, LMI_ENABLED);
		p->lmi_state = sl_get_i_state(sl);
		printd(("%s: %p: <- LMI_ENABLE_CON\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
slu_disable_con(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_disable_con_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_CON;
		assure(sl_get_i_state(sl) == LMI_DISABLE_PENDING);
		sl_set_i_state(sl, LMI_DISABLED);
		p->lmi_state = sl_get_i_state(sl);
		printd(("%s: %p: <- LMI_DISABLE_CON\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
slu_optmgmt_ack(queue_t *q, struct sl *sl, uint32_t flags, uchar *opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_error_ind(queue_t *q, struct sl *sl, uint32_t errno, uint32_t reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = sl_get_i_state(sl);
		printd(("%s: %p: <- LMI_ERROR_IND\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_stats_ind(queue_t *q, struct sl *sl, uint32_t interval, mblk_t *dp)
{
	mblk_t *mp;
	lmi_stats_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = drv_hztomsec(jiffies);
			mp->b_cont = dp;
			printd(("%s: %p: <- LMI_STATS_IND\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_event_ind(queue_t *q, struct sl *sl, uint32_t oid, uint32_t level)
{
	mblk_t *mp;
	lmi_event_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = drv_hztomsec(jiffies);
			p->lmi_severity = level;
			printd(("%s: %p: <- LMI_EVENT_IND\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_PDU_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_pdu_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *dp;

	assure(!m->mp->b_cont);
	if ((dp = ss7_dupb(q, m->mp))) {
		uint32_t mpri;
		mblk_t *mp;
		sl_pdu_ind_t *p;

		if (m->data1.ptr.c) {
			mpri = 0;
			dp->b_rptr = dp->b_wptr = m->data1.ptr.c;
			dp->b_wptr += m->data1.len;
		} else if (m->data2.ptr.c) {
			mpri = m->data2.ptr.c[0] & 0x3;
			dp->b_rptr = dp->b_wptr = m->data2.ptr.c + 1;
			dp->b_wptr += m->data2.len - 1;
		} else {
			freeb(dp);
			swerr();
			return (-EFAULT);
		}
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			if (canputnext(sl->oq)) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->sl_primitive = SL_PDU_IND;
				p->sl_mp = mpri;
				mp->b_cont = dp;
				printd(("%s: %p: <- SL_PDU_IND\n", DRV_NAME, sl));
				putnext(sl->oq, mp);
				return (QR_DONE);
			}
			freemsg(mp);
			return (-EBUSY);
		}
		freeb(dp);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTED_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_link_congested_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_link_cong_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		p->sl_cong_status = m->cong.val;
		p->sl_disc_status = m->disc.val;
		printd(("%s: %p: <- SL_LINK_CONGESTED_IND\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_link_congestion_ceased_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_link_cong_ceased_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_cong_status = m->cong.val;
		p->sl_disc_status = m->disc.val;
		printd(("%s: %p: <- SL_LINK_CONGESTION_CEASED_IND\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_retrieved_message_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *dp;

	assure(!m->mp->b_cont);
	if ((dp = ss7_dupb(q, m->mp))) {
		uint32_t mpri;
		mblk_t *mp;
		sl_retrieved_msg_ind_t *p;

		if (m->data1.ptr.c) {
			mpri = 0;
			dp->b_rptr = dp->b_wptr = m->data1.ptr.c;
			dp->b_wptr += m->data1.len;
		} else if (m->data2.ptr.c) {
			mpri = m->data2.ptr.c[0] & 0x3;
			dp->b_rptr = dp->b_wptr = m->data2.ptr.c + 1;
			dp->b_wptr += m->data2.len - 1;
		} else {
			freeb(dp);
			swerr();
			return (-EFAULT);
		}
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			if (canputnext(sl->oq)) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
				p->sl_mp = mpri;
				mp->b_cont = dp;
				printd(("%s: %p: <- SL_RETRIEVED_MESSAGE_IND\n", DRV_NAME, sl));
				putnext(sl->oq, mp);
				return (QR_DONE);
			}
			freemsg(mp);
			return (-EBUSY);
		}
		freeb(dp);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_retrieval_complete_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *dp = NULL;

	if ((!m->data1.ptr.c && !m->data2.ptr.c) || (dp = ss7_dupb(q, m->mp))) {
		uint32_t mpri = 0;
		mblk_t *mp;
		sl_retrieval_comp_ind_t *p;

		if (m->data1.ptr.c) {
			mpri = 0;
			dp->b_rptr = dp->b_wptr = m->data1.ptr.c;
			dp->b_wptr += m->data1.len;
		}
		if (m->data2.ptr.c) {
			mpri = m->data2.ptr.c[0] & 0x3;
			dp->b_rptr = dp->b_wptr = m->data2.ptr.c + 1;
			dp->b_wptr += m->data2.len - 1;
		}
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			if (canputnext(sl->oq)) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
				p->sl_mp = mpri;
				mp->b_cont = dp;
				printd(("%s: %p: <- SL_RETRIEVAL_COMPLETE_IND\n", DRV_NAME, sl));
				putnext(sl->oq, mp);
				return (QR_DONE);
			}
			freemsg(mp);
			return (-EBUSY);
		}
		if (dp)
			freeb(dp);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RB_CLEARED_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_rb_cleared_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_rb_cleared_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RB_CLEARED_IND;
			printd(("%s: %p: <- SL_RB_CLEARED_IND\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_bsnt_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_bsnt_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_BSNT_IND;
			p->sl_bsnt = m->seqno.val;
			printd(("%s: %p: <- SL_BSNT_IND\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_IN_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_in_service_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_in_service_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_IN_SERVICE_IND;
			printd(("%s: %p: <- SL_IN_SERVICE_IND\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_OUT_OF_SERVICE_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_out_of_service_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_out_of_service_ind_t *p;
	uint32_t reason = m ? (m->type == M2UA_MAUP_REL_CON ? 0 : SL_FAIL_UNSPECIFIED)
	    : SL_FAIL_UNSPECIFIED;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_reason = reason;
		printd(("%s: %p: <- SL_OUT_OF_SERVICE_IND\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_local_processor_outage_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_loc_proc_out_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		printd(("%s: %p: <- SL_LOCAL_PROCESSOR_OUTAGE_IND\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LOCAL_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_local_processor_recovered_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_loc_proc_recovered_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LOCAL_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		printd(("%s: %p: <- SL_LOCAL_PROCESSOR_RECOVERED_IND\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_remote_processor_outage_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_rem_proc_out_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_OUTAGE_IND\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_remote_processor_recovered_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_rem_proc_recovered_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		printd(("%s: %p: <- SL_REMOTE_PROCESSOR_RECOVERED_IND\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RTB_CLEARED_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_rtb_cleared_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_rtb_cleared_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RTB_CLEARED_IND;
			printd(("%s: %p: <- SL_RTB_CLEARED_IND\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_retrieval_not_possible_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_retrieval_not_poss_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
			printd(("%s: %p: <- SL_RETRIEVAL_NOT_POSSIBLE_IND\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_bsnt_not_retrievable_ind(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_bsnt_not_retr_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
			p->sl_bsnt = m->seqno.ptr.c ? m->seqno.val : 0;
			printd(("%s: %p: <- SL_BSNT_NOT_RETRIEVABLE_IND\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  SL_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
slu_optmgmt_ack(queue_t *q, struct sl *sl, uchar *opt_ptr, size_t opt_len, uint32_t flags)
{
	mblk_t *mp;
	sl_optmgmt_ack_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_OPTMGMT_ACK;
		p->opt_length = opt_len;
		p->opt_offset = opt_len ? sizeof(*p) : 0;
		p->mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- SL_OPTMGMT_ACK\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  SL_NOTIFY_IND
 *  -----------------------------------
 */
STATIC INLINE int
slu_notify_ind(queue_t *q, struct sl *sl, uint32_t oid, uint32_t level)
{
	mblk_t *mp;
	sl_notify_ind_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_NOTIFY_IND;
		p->sl_objectid = oid;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_severity = level;
		printd(("%s: %p: <- SL_NOTIFY_IND\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream (to SL Provider)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_info_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_info_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_REQ;
		printd(("%s: %p: LMI_INFO_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_attach_req(queue_t *q, struct sl *sl, uchar *ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_attach_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ATTACH_REQ;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: LMI_ATTACH_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_detach_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_detach_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DETACH_REQ;
		printd(("%s: %p: LMI_DETACH_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_enable_req(queue_t *q, struct sl *sl, uchar *rem_ptr, size_t rem_len)
{
	mblk_t *mp;
	lmi_enable_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_REQ;
		bcopy(rem_ptr, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
		printd(("%s: %p: LMI_ENABLE_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_disable_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_disable_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_REQ;
		printd(("%s: %p: LMI_DISABLE_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_optmgmt_req(queue_t *q, struct sl *sl, uchar *opt_ptr, size_t opt_len, uint32_t flags)
{
	mblk_t *mp;
	lmi_optmgmt_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_OPTMGMT_REQ;
			p->lmi_opt_length = opt_len;
			p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
			p->lmi_mgmt_flags = flags;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			printd(("%s: %p: LMI_OPTMGMT_REQ ->\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
STATIC int
slp_pdu_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *dp;

	assure(!m->mp->b_cont);
	if ((dp = ss7_dupb(q, m->mp))) {
		uint32_t mpri;
		mblk_t *mp;
		sl_pdu_req_t *p;

		if (m->data1.ptr.c) {
			mpri = 0;
			dp->b_rptr = dp->b_wptr = m->data1.ptr.c;
			dp->b_wptr += m->data1.len;
		} else if (m->data2.ptr.c) {
			mpri = m->data2.ptr.c[0] & 0x3;
			dp->b_rptr = dp->b_wptr = m->data2.ptr.c + 1;
			dp->b_wptr += m->data2.len - 1;
		} else {
			freeb(dp);
			swerr();
			return (-EFAULT);
		}
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			if (canputnext(sl->oq)) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->sl_primitive = SL_PDU_REQ;
				p->sl_mp = mpri;
				mp->b_cont = dp;
				printd(("%s: %p: SL_PDU_REQ ->\n", DRV_NAME, sl));
				putnext(sl->oq, mp);
				return (QR_DONE);
			}
			freemsg(mp);
			return (-EBUSY);
		}
		freeb(dp);
	}
	return (-ENOBUFS);
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_emergency_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_emergency_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_EMERGENCY_REQ;
		printd(("%s: %p: SL_EMERGENCY_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_emergency_ceases_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_emergency_ceases_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
		printd(("%s: %p: SL_EMERGENCY_CEASES_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_start_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_start_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_START_REQ;
			printd(("%s: %p: SL_START_REQ ->\n", DRV_NAME, sl));
			puntext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_stop_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_stop_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_STOP_REQ;
			printd(("%s: %p: SL_STOP_REQ ->\n", DRV_NAME, sl));
			puntext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_retrieve_bsnt_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_retrieve_bsnt_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RETRIEVE_BSNT_REQ;
			printd(("%s: %p: SL_RETRIEVE_BSNT_REQ ->\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_retrieval_request_and_fsnc_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_retrieval_req_and_fsnc_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RETRIEVAL_REQUEST_AND_FSNC_REQ;
			p->sl_fsnc = m->seqno.val;
			printd(("%s: %p: SL_RETRIEVAL_REQUEST_AND_FSNC_REQ ->\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_clear_buffers_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_clear_buffers_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
			printd(("%s: %p: SL_CLEAR_BUFFERS_REQ ->\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_clear_rtb_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_clear_rtb_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_CLEAR_RTB_REQ;
			printd(("%s: %p: SL_CLEAR_RTB_REQ ->\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CONTINUE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_continue_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_continue_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_CONTINUE_REQ;
			printd(("%s: %p: SL_CONTINUE_REQ ->\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_local_processor_outage_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_local_proc_outage_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		printd(("%s: %p: SL_LOCAL_PROCESSOR_OUTAGE_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_resume_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_resume_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sl_primitive = SL_RESUME_REQ;
			printd(("%s: %p: SL_RESUME_REQ ->\n", DRV_NAME, sl));
			putnext(sl->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_congestion_discard_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_cong_discard_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
		printd(("%s: %p: SL_CONGESTION_DISCARD_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_congestion_accept_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_cong_accept_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		printd(("%s: %p: SL_CONGESTION_ACCEPT_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_no_congestion_req(queue_t *q, struct sl *sl, struct ua_msg *m)
{
	mblk_t *mp;
	sl_no_cong_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_NO_CONGESTION_REQ;
		printd(("%s: %p: SL_NO_CONGESTION_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
STATIC INLINE int
slp_power_on_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_power_on_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sl_primitive = SL_POWER_ON_REQ;
		printd(("%s: %p: SL_POWER_ON_REQ ->\n", DRV_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent downstream (to Transport Provider)
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T_CONN_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_conn_req(queue_t *q, struct xp *xp, uchar *dst_ptr, size_t dst_len, uchar *opt_ptr,
	   size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct T_conn_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_CONN_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s; %p: T_CONN_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_CONN_RES
 *  -----------------------------------
 */
STATIC INLINE int
t_conn_res(queue_t *q, struct xp *xp, uint32_t acceptor, uchar *opt_ptr, size_t opt_len,
	   uint32_t seqno, mblk_t *dp)
{
	mblk_t *mp;
	struct T_conn_res *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_CONN_RES;
			p->ACCEPTOR_id = acceptor;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->SEQ_number = seqno;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			printd(("%s; %p: T_CONN_RES ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_DISCON_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_discon_req(queue_t *q, struct xp *xp, uint32_t seqno, mblk_t *dp)
{
	mblk_t *mp;
	struct T_discon_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_DISCON_REQ;
			p->SEQ_number = seqno;
			mp->b_cont = dp;
			printd(("%s; %p: T_DISCON_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_DATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_data_req(queue_t *q, struct xp *xp, uint32_t more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_DATA_REQ;
			p->MORE_flag = more;
			mp->b_cont = dp;
			printd(("%s; %p: T_DATA_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_EXDATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_exdata_req(queue_t *q, struct xp *xp, uint32_t more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_exdata_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_EXDATA_REQ;
			p->MORE_flag = more;
			mp->b_cont = dp;
			printd(("%s; %p: T_EXDATA_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_info_req(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	struct T_info_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_INFO_REQ;
			printd(("%s; %p: T_INFO_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_BIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_bind_req(queue_t *q, struct xp *xp, uchar *add_ptr, size_t add_len, uint32_t conind)
{
	mblk_t *mp;
	struct T_bind_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_BIND_REQ;
			p->ADDR_length = add_len;
			p->ADDR_offset = add_len ? sizeof(*p) : 0;
			p->CONIND_number = conind;
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			printd(("%s; %p: T_BIND_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_UNBIND_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_unbind_req(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	struct T_unbind_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_UNBIND_REQ;
			printd(("%s; %p: T_UNBIND_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_UNITDATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_unitdata_req(queue_t *q, struct xp *xp, uchar *dst_ptr, size_t dst_len, uchar *opt_ptr,
	       size_t opt_len, mblk_t *dp)
{
	mblk_t *mp;
	struct T_unitdata_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_UNITDATA_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
			mp->b_cont = dp;
			printd(("%s; %p: T_UNITDATA_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_optmgmt_req(queue_t *q, struct xp *xp, uchar *opt_ptr, size_t opt_len, uint32_t flags)
{
	mblk_t *mp;
	struct T_optmgmt_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_OPTMGMT_REQ;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->MGMT_flags = flags;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			printd(("%s; %p: T_OPTMGMT_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_ORDREL_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_ordrel_req(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	struct T_ordrel_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_ORDREL_REQ;
			printd(("%s; %p: T_ORDREL_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_OPTDATA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_optdata_req(queue_t *q, struct xp *xp, uint32_t flags, uint32_t sid, mblk_t *dp)
{
	mblk_t *mp;
	struct T_optdata_req *p;
	uchar opt[2 * sizeof(struct t_opthdr) + 2 * sizeof(t_uscalar_t)];
	size_t opt_len = (xp->type == M2UA_OBJ_TYPE_XP_SCTP) ? sizeof(opt) : 0;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			if (opt_len) {
				struct t_opthdr *oh;

				oh = (typeof(oh)) mp->b_wptr;
				mp->b_wptr += sizeof(*oh);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->len = sizeof(t_uscalar_t);
				*(t_uscalar_t *) mp->b_wptr = M2UA_PPI;
				mp->b_wptr += sizeof(t_uscalar_t);
				oh = (typeof(oh)) mp->b_wptr;
				mp->b_wptr += sizeof(*oh);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SID;
				oh->len = sizeof(t_uscalar_t);
				*(t_uscalar_t *) mp->b_wptr = sid;
				mp->b_wptr += sizeof(t_uscalar_t);
			}
			mp->b_cont = dp;
			printd(("%s; %p: T_OPTDATA_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  T_ADDR_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_addr_req(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	struct T_addr_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_ADDR_REQ;
			printd(("%s; %p: T_ADDR_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ
 *  -----------------------------------
 */
STATIC INLINE int
t_capability_req(queue_t *q, struct xp *xp)
{
	mblk_t *mp;
	struct T_capability_req *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(xp->oq)) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_CAPABILITY_REQ;
			printd(("%s; %p: T_CAPABILITY_REQ ->\n", DRV_NAME, xp));
			putnext(xp->oq, mp);
			return (QR_DONE);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  =========================================================================
 *
 *  M2UA Messages
 *
 *  =========================================================================
 */

STATIC int
m2ua_dec_msg(mblk_t *mp, struct ua_msg *m)
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
		case UA_TAG(UA_PARM_IID_RANGE):	/* UA_CONST_PHDR(0x0002,0) */
			parm = &m->iid;
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
		case UA_TAG(UA_PARM_IID_RANGE1):	/* UA_CONST_PHDR(0x0008,0) */
			parm = &m->iid;
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
		case UA_TAG(M2UA_PARM_DATA1):	/* UA_CONST_PHDR(0x0300,0) */
			parm = &m->data1;
			break;
		case UA_TAG(M2UA_PARM_DATA2):	/* UA_CONST_PHDR(0x0301,0) */
			parm = &m->data2;
			break;
		case UA_TAG(M2UA_PARM_STATE_REQUEST):	/* UA_CONST_PHDR(0x0302,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->status;
			break;
		case UA_TAG(M2UA_PARM_STATE_EVENT):	/* UA_CONST_PHDR(0x0303,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->event;
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
				parm = &m->action;
			break;
		case UA_TAG(M2UA_PARM_SEQNO):	/* UA_CONST_PHDR(0x0307,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->seqno;
			break;
		case UA_TAG(M2UA_PARM_RETR_RESULT):	/* UA_CONST_PHDR(0x0308,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->result;
			break;
		case UA_TAG(M2UA_PARM_LINK_KEY):	/* UA_CONST_PHDR(0x0309,sizeof(uint32_t)*6) 
							 */
			if (UA_PLEN(*wp) >= sizeof(uint32_t) * 6)
				parm = &m->linkkey;
			break;
		case UA_TAG(M2UA_PARM_LOC_KEY_ID):	/* UA_CONST_PHDR(0x030a,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->keyid;
			break;
		case UA_TAG(M2UA_PARM_SDTI):	/* UA_CONST_PHDR(0x030b,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->sdti;
			break;
		case UA_TAG(M2UA_PARM_SDLI):	/* UA_CONST_PHDR(0x030c,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->sdli;
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
				parm = &m->corid;
			break;
		case UA_TAG(M2UA_PARM_CORR_ID_ACK):	/* UA_CONST_PHDR(0x0312,sizeof(uint32_t)) */
			if (UA_PLEN(*wp) >= sizeof(uint32_t))
				parm = &m->corid;
			break;
		}
		if (parm) {
			parm->val = ntohl(wp[1]);
			parm->val = wp[1];
			parm->len = UA_PLEN(*wp);
		}
	}
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M2UA Send Message Functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  UA_MGMT_ERR
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_mgmt_err(queue_t *q, struct xp *xp, uint ecode, uchar *dia_ptr, size_t dia_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_ECODE) + dia_len ? UA_SIZE(UA_PARM_DIAG) +
	    UA_PAD4(dia_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
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

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_MGMT_NTFY
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_mgmt_ntfy(queue_t *q, struct xp *xp, uint status, uint32_t *aspid, uint *iid,
		    size_t num_iid, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_STATUS) + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 +
	    num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_MGMT_NTFY;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_STATUS;
		p[3] = htonl(status);
		p += 4;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
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

		if ((err = t_optdata_req(q, xp, T_ODF_EX, iid ? *iid : 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPS_ASPUP_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_asps_aspup_req(queue_t *q, struct xp *xp, uint32_t *aspid, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
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

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPS_ASPDN_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_asps_aspdn_req(queue_t *q, struct xp *xp, uint32_t *aspid, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
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

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPS_HBEAT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_asps_hbeat_req(queue_t *q, struct xp *xp, uchar *hbt_ptr, size_t hbt_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPS_ASPUP_ACK
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_asps_aspup_ack(queue_t *q, struct xp *xp, uint32_t *aspid, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPUP_ACK;
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

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPS_ASPDN_ACK
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_asps_aspdn_ack(queue_t *q, struct xp *xp, uint32_t *aspid, uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE +
	    UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_ACK;
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

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPS_HBEAT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_asps_hbeat_ack(queue_t *q, struct xp *xp, uchar *hbt_ptr, size_t hbt_len)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPT_ASPAC_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_aspt_aspac_req(queue_t *q, struct xp *xp, uint tmode, uint32_t *iid, size_t num_iid,
			 uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_TMODE) + num_iid ? UA_PHDR_SIZE +
	    num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
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

		if ((err = t_optdata_req(q, xp, T_ODF_EX, iid ? *iid : 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPT_ASPIA_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_aspt_aspia_req(queue_t *q, struct xp *xp, uint32_t *iid, size_t num_iid, uchar *inf_ptr,
			 size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
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

		if ((err = t_optdata_req(q, xp, T_ODF_EX, iid ? *iid : 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPT_ASPAC_ACK
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_aspt_aspac_ack(queue_t *q, struct xp *xp, uint tmode, uint32_t *iid, size_t num_iid,
			 uchar *inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_TMODE) + num_iid ? UA_PHDR_SIZE +
	    num_iid * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
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
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);

		if ((err = t_optdata_req(q, xp, T_ODF_EX, iid ? *iid : 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_ASPT_ASPIA_ACK
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_aspt_aspia_ack(queue_t *q, struct xp *xp, uint32_t *iid, size_t num_iid, uchar *inf_ptr,
			 size_t inf_len)
{
	mblk_t *mp;
	size_t mlen =
	    UA_MHDR_SIZE + num_iid ? UA_PHDR_SIZE + num_iid * sizeof(uint32_t) : 0 +
	    inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_ACK;
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

		if ((err = t_optdata_req(q, xp, T_ODF_EX, iid ? *iid : 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_RKMM_REG_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_rkmm_reg_req(queue_t *q, struct xp *xp, uint id, uint sdti, uint sdli)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_LINK_KEY);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
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
		mp->b_wptr = (unsigned char *) &p[9];

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_RKMM_REG_RSP
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_rkmm_reg_rsp(queue_t *q, struct xp *xp, struct as *as, uint id, uint status)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_REG_RESULT);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_REG_RSP;
		p[1] = __constant_htonl(mlen);
		p[2] = M2UA_PARM_REG_RESULT;
		p[3] = M2UA_PARM_LOC_KEY_ID;
		p[4] = htonl(id);
		p[5] = M2UA_PARM_REG_STATUS;
		p[6] = htonl(status);
		p[7] = UA_PARM_IID;
		p[8] = status ? 0 : htonl(as->iid);
		mp->b_wptr = (unsigned char *) &p[9];

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_RKMM_DEREG_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_rkmm_dereg_req(queue_t *q, struct xp *xp, struct as *as)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_DEREG_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(as->iid);
		mp->b_wptr = (unsigned char *) &p[4];

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  UA_RKMM_DEREG_RSP
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_rkmm_dereg_rsp(queue_t *q, struct xp *xp, struct as *as, uint status)
{
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_DEREG_STATUS);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_DEREG_RSP;
		p[1] = __constant_htonl(mlen);
		p[2] = M2UA_PARM_DEREG_RESULT;
		p[3] = UA_PARM_IID;
		p[4] = htonl(as->iid);
		p[5] = M2UA_PARM_DEREG_STATUS;
		p[6] = htonl(status);
		mp->b_wptr = (unsigned char *) &p[7];

		if ((err = t_optdata_req(q, xp, T_ODF_EX, 0, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_DATA
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_data1(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	size_t dlen = msgdsize(bp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA1, dlen);
		mp->b_wptr = (unsigned char *) &p[5];

		mp->b_cont = bp->b_cont;
		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_TRIMMED);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
m2ua_send_maup_data2(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	size_t dlen = msgdsize(bp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA2, dlen);
		mp->b_wptr = (unsigned char *) &p[5];

		mp->b_cont = bp->b_cont;
		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_TRIMMED);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_ESTAB_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_estab_req(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_ESTAB_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		mp->b_wptr = (unsigned char *) &p[4];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_ESTAB_CON
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_estab_con(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_ESTAB_CON;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		mp->b_wptr = (unsigned char *) &p[4];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_REL_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_rel_req(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		mp->b_wptr = (unsigned char *) &p[4];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_REL_CON
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_rel_con(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_CON;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		mp->b_wptr = (unsigned char *) &p[4];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_REL_IND
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_rel_ind(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_REL_IND;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		mp->b_wptr = (unsigned char *) &p[4];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_STATE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_state_req(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_STATE_REQUEST);
	union SL_primitives *p = (typeof(p)) bp->b_rptr;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_STATE_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = M2UA_PARM_STATE_REQUEST;

		switch (p->sl_primitive) {
		case SL_EMERGENCY_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_EMER_SET);
			break;
		case SL_EMERGENCY_CEASES_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_EMER_CLEAR);
			break;
		case SL_CONTINUE_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_CONTINUE);
			break;
		case SL_CLEAR_BUFFERS_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_FLUSH_BUFFERS);
			break;
		case SL_CLEAR_RTB_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_CLEAR_RTB);
			break;
		case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_LPO_SET);
			break;
		case SL_RESUME_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_LPO_CLEAR);
			break;
		case SL_CONGESTION_DISCARD_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_CONG_DISCARD);
			break;
		case SL_CONGESTION_ACCEPT_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_CONG_ACCEPT);
			break;
		case SL_NO_CONGESTION_REQ:
			p[5] = __constant_htonl(M2UA_STATUS_CONG_CLEAR);
			break;
		default:
			goto efault;
		}
		mp->b_wptr = (unsigned char *) &p[6];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_STATE_CON
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_state_con(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_STATE_REQUEST);
	union SL_primitives *p = (typeof(p)) bp->b_rptr;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_STATE_CON;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = M2UA_PARM_STATE_REQUEST;

		switch (p->sl_primitive) {
		case SL_RB_CLEARED_IND:
			p[5] = __constant_htonl(M2UA_STATUS_FLUSH_BUFFERS);
			break;
		case SL_RTB_CLEARED_IND:
			p[5] = __constant_htonl(M2UA_STATUS_CLEAR_RTB);
			break;
		default:
			goto efault;
		}
		mp->b_wptr = (unsigned char *) &p[6];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
      efault:
	swerr();
	freeb(mp);
	return (-EFAULT);
}
STATIC INLINE int
m2ua_send_maup_con(queue_t *q, struct gp *gp, struct ua_msg *m)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_STATE_REQUEST);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_STATE_CON;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = M2UA_PARM_STATE_REQUEST;
		p[5] = htonl(m->status.val);
		mp->b_wptr = (unsigned char *) &p[6];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_STATE_IND
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_state_ind(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_STATE_EVENT);
	union SL_primitives *p = (typeof(p)) bp->b_rptr;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_STATE_IND;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = M2UA_PARM_STATE_EVENT;
		switch (p->sl_primitive) {
		case SL_REMOTE_PROCESSOR_OUTAGE_IND:
			p[5] = __constant_htonl(M2UA_EVENT_RPO_ENTER);
			break;
		case SL_REMOTE_PROCESSOR_RECOVERED_IND:
			p[5] = __constant_htonl(M2UA_EVENT_RPO_EXIT);
			break;
		case SL_LOCAL_PROCESSOR_OUTAGE_IND:
			p[5] = __constant_htonl(M2UA_EVENT_LPO_ENTER);
			break;
		case SL_LOCAL_PROCESSOR_RECOVERED_IND:
			p[5] = __constant_htonl(M2UA_EVENT_LPO_EXIT);
			break;
		default:
			goto efault;
		}
		mp->b_wptr = (unsigned char *) &p[6];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_RETR_REQ
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_retr_req(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	union SL_primitives *p = (typeof(p)) bp->b_rptr;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_ACTION) +
	    (p->sl_primitive == SL_RETRIEVAL_REQUEST_AND_FSNC_REQ) ? UA_SIZE(M2UA_PARM_SEQNO) : 0;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[1] = M2UA_MAUP_RETR_REQ;
		p[2] = __constant_htonl(mlen);
		p[3] = UA_PARM_IID;
		p[4] = htonl(gp->as.as->iid);
		p[5] = M2UA_PARM_ACTION;
		switch (p->sl_primitive) {
		case SL_RETRIEVE_BSNT_REQ:
			p[6] = __constant_htonl(M2UA_ACTION_RTRV_BSN);
			mp->b_wptr = (unsigned char *) &p[7];
			break;
		case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
			p[6] = __constant_htonl(M2UA_ACTION_RTRV_MSGS);
			p[7] = M2UA_PARM_SEQNO;
			p[8] = htonl(p->retrieval_req_and_fsnc.sl_fsnc);
			mp->b_wptr = (unsigned char *) &p[9];
			break;
		default:
			goto efault;
		}

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_RETR_CON
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_retr_con(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	union SL_primitives *p = (typeof(p)) bp->b_rptr;
	size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_ACTION) +
	    UA_SIZE(M2UA_PARM_RETR_RESULT) + UA_SIZE(M2UA_PARM_SEQNO);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_RETR_CON;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = M2UA_PARM_ACTION;
		switch (p->sl_primitive) {
		case SL_BSNT_IND:
			p[5] = __constant_htonl(M2UA_ACTION_RTRV_BSN);
			p[6] = M2UA_PARM_RETR_RESULT;
			p[7] = __constant_htonl(UA_RESULT_SUCCESS);
			p[8] = M2UA_PARM_SEQNO;
			p[9] = htonl(p->bsnt_ind.sl_bsnt);
			break;
		case SL_BSNT_NOT_RETRIEVABLE_IND:
			p[5] = __constant_htonl(M2UA_ACTION_RTRV_BSN);
			p[6] = M2UA_PARM_RETR_RESULT;
			p[7] = __constant_htonl(UA_RESULT_FAILURE);
			p[8] = M2UA_PARM_SEQNO;
			p[9] = htonl(p->bsnt_not_retr_ind.sl_bsnt);
			break;
		case SL_RETRIEVAL_NOT_POSSIBLE_IND:
			p[5] = __constant_htonl(M2UA_ACTION_RTRV_MSGS);
			p[6] = M2UA_PARM_RETR_RESULT;
			p[7] = __constant_htonl(UA_RESULT_FAILURE);
			p[8] = M2UA_PARM_SEQNO;
			p[9] = 0;
			break;
		default:
			goto efault;
		}
		mp->b_wptr = (unsigned char *) &p[10];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_RETR_IND
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_retr_ind1(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	size_t dlen = msgdsize(bp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_RETR_IND;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA1, dlen);
		mp->b_wptr = (unsigned char *) &p[5];

		mp->b_cont = bp->b_cont;

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0) {
			freeb(bp);
			return (QR_DONE);
		}
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
m2ua_send_maup_retr_ind2(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	size_t dlen = msgdsize(bp->b_cont);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_PHDR_SIZE;

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_RETR_IND;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = UA_PHDR(M2UA_PARM_DATA2, dlen);
		mp->b_wptr = (unsigned char *) &p[5];

		mp->b_cont = bp->b_cont;

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0) {
			freeb(bp);
			return (QR_DONE);
		}
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_RETR_COMP_IND
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_retr_comp_ind(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_ACTION) +
	    UA_SIZE(M2UA_PARM_RETR_RESULT);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_RETR_COMP_IND;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = M2UA_PARM_ACTION;
		p[5] = __constant_htonl(M2UA_ACTION_RTRV_MSGS);
		p[6] = M2UA_PARM_RETR_RESULT;
		p[7] = __constant_htonl(UA_RESULT_SUCCESS);
		mp->b_wptr = (unsigned char *) &p[8];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  M2UA_MAUP_CONG_IND
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_cong_ind(queue_t *q, struct gp *gp, mblk_t *bp)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_CONG_STATUS) +
	    UA_SIZE(M2UA_PARM_DISC_STATUS);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;
		union SL_primitives *p = (typeof(p)) bp->b_rptr;

		p[0] = M2UA_MAUP_CONG_IND;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		switch (p->sl_primitive) {
		case SL_LINK_CONGESTED_IND:
			p[4] = M2UA_PARM_CONG_STATUS;
			p[5] = htonl(p->link_cong_ind.sl_cong_status);
			p[6] = M2UA_PARM_DISC_STATUS;
			p[7] = htonl(p->link_cong_ind.sl_disc_status);
			break;
		case SL_LINK_CONGESTION_CEASED_IND:
			p[4] = M2UA_PARM_CONG_STATUS;
			p[5] = mp->b_wptr += sizeof(uint32_t);
			p[6] = M2UA_PARM_DISC_STATUS;
			p[7] = htonl(p->link_cong_ceased_ind.sl_disc_status);
			break;
		default:
			goto efault;
		}
		mp->b_wptr = (unsigned char *) &p[8];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_DATA_ACK
 *  -----------------------------------
 */
STATIC INLINE int
m2ua_send_maup_data_ack(queue_t *q, struct gp *gp, struct ua_msg *m)
{
	mblk_t *mp;
	static const size_t mlen =
	    UA_MHDR_SIZE + UA_SIZE(UA_PARM_IID) + UA_SIZE(M2UA_PARM_CORR_ID_ACK);

	if ((mp = ss7_allocb(q, mlen, BPRI_MED))) {
		int err;
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = M2UA_MAUP_DATA_ACK;
		p[1] = __constant_htonl(mlen);
		p[2] = UA_PARM_IID;
		p[3] = htonl(gp->as.as->iid);
		p[4] = M2UA_PARM_CORR_ID_ACK;
		p[5] = htonl(m->corid.val);
		mp->b_wptr = (unsigned char *) &p[6];

		if ((err = t_optdata_req(q, gp->spp.spp->xp, 0, gp->as.as->iid, mp)) >= 0)
			return (QR_DONE);
		freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
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

#define M2UA_DECLARE_TIMER(__o, __t) \
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
 *  SPP timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
spp_do_timeout(caddr_t data, const char *timer, toid_t *timeo, int (to_fnc) (struct spp *),
	       streamscall void (*exp_fnc) (caddr_t))
{
	struct spp *spp = (struct spp *) data;

	if (xchg(timeo, 0)) {
		if (spin_trylock(&spp->lock)) {
			printd(("%s: %p: %s: timeout at %lu\n", DRV_NAME, spp, timer, jiffies));
			switch (to_fnc(spp)) {
			default:
			case QR_DONE:
				spin_unlock(&spp->lock);
				spp_put(spp);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&spp->lock);
		} else
			printd(("%s: %p: %s: timeout collision at %lu\n", DRV_NAME, spp, timer,
				jiffies));
		/* back off timer 100 ticks */
		*timeo = timeout(exp_fnc, data, 100);
	}
}
STATIC INLINE void
spp_stop_timer(struct spp *spp, const char *timer, toid_t *timeo)
{
	toid_t to;

	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", DRV_NAME, spp, timer, jiffies));
		spp_put(spp);
	}
	return;
}
STATIC INLINE void
spp_start_timer(struct spp *spp, const char *timer, toid_t *timeo,
		streamscall void (*exp_fnc) (caddr_t), unsigned long val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", DRV_NAME, spp, timer, val * 1000 / HZ,
		jiffies));
	*timeo = timeout(exp_fnc, (caddr_t) spp_get(spp), val);
}

M2UA_DECLARE_TIMER(spp, tack);
M2UA_DECLARE_TIMER(spp, tbeat);
M2UA_DECLARE_TIMER(spp, tidle);

STATIC INLINE void
__spp_timer_stop(struct spp *spp, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case tack:
		spp_stop_timer_tack(spp);
		if (single)
			break;
		/* fall through */
	case tbeat:
		spp_stop_timer_tbeat(spp);
		if (single)
			break;
		/* fall through */
	case tidle:
		spp_stop_timer_tidle(spp);
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
spp_timer_stop(struct spp *spp, const uint t)
{
	spin_lock_m2ua(&spp->lock);
	{
		__spp_timer_stop(spp, t);
	}
	spin_unlock_m2ua(&spp->lock);
}
STATIC INLINE void
spp_timer_start(struct spp *spp, const uint t)
{
	spin_lock_m2ua(&spp->lock);
	{
		__spp_timer_stop(spp, t);
		switch (t) {
		case tack:
			spp_start_timer_tack(spp, spp->sp.sp->config.tack);
			break;
		case tbeat:
			spp_start_timer_tbeat(spp, spp->sp.sp->config.tbeat);
			break;
		case tidle:
			spp_start_timer_tidle(spp, spp->sp.sp->config.tidle);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_m2ua(&spp->lock);
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

M2UA_DECLARE_TIMER(as, tack);

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
	spin_lock_m2ua(&as->lock);
	{
		__as_timer_stop(as, t);
	}
	spin_unlock_m2ua(&as->lock);
}
STATIC INLINE void
as_timer_start(struct as *as, const uint t)
{
	spin_lock_m2ua(&as->lock);
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
	spin_unlock_m2ua(&as->lock);
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

M2UA_DECLARE_TIMER(sp, tack);
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
	spin_lock_m2ua(&sp->lock);
	{
		__sp_timer_stop(sp, t);
	}
	spin_unlock_m2ua(&sp->lock);
}
STATIC INLINE void
sp_timer_start(struct sp *sp, const uint t)
{
	spin_lock_m2ua(&sp->lock);
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
	spin_unlock_m2ua(&sp->lock);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
sl_do_timeout(caddr_t data, const char *timer, toid_t *timeo, int (to_fnc) (struct sl *),
	      streamscall void (*exp_fnc) (caddr_t))
{
	struct sl *sl = (struct sl *) data;

	if (xchg(timeo, 0)) {
		if (spin_trylock(&sl->lock)) {
			printd(("%s: %p: %s: timeout at %lu\n", DRV_NAME, sl, timer, jiffies));
			switch (to_fnc(sl)) {
			default:
			case QR_DONE:
				spin_unlock(&sl->lock);
				sl_put(sl);
				return;
			case -ENOMEM:
			case -ENOBUFS:
			case -EBUSY:
			case -EAGAIN:
				break;
			}
			spin_unlock(&sl->lock);
		} else
			printd(("%s: %p: %s: timeout collision at %lu\n", DRV_NAME, sl, timer,
				jiffies));
		/* back off timer 100 ticks */
		*timeo = timeout(exp_fnc, data, 100);
	}
}
STATIC INLINE void
sl_stop_timer(struct sl *sl, const char *timer, toid_t *timeo)
{
	toid_t to;

	if ((to = xchg(timeo, 0))) {
		untimeout(to);
		printd(("%s: %p: stopping %s at %lu\n", DRV_NAME, sl, timer, jiffies));
		sl_put(sl);
	}
	return;
}
STATIC INLINE void
sl_start_timer(struct sl *sl, const char *timer, toid_t *timeo,
	       streamscall void (*exp_fnc) (caddr_t), unsigned long val)
{
	printd(("%s: %p: starting %s %lu ms at %lu\n", DRV_NAME, sl, timer, val * 1000 / HZ,
		jiffies));
	*timeo = timeout(exp_fnc, (caddr_t) sl_get(sl), val);
}

M2UA_DECLARE_TIMER(sl, tack);
STATIC INLINE void
__sl_timer_stop(struct sl *sl, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case tack:
		sl_stop_timer_tack(sl);
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
sl_timer_stop(struct sl *sl, const uint t)
{
	spin_lock_m2ua(&sl->lock);
	{
		__sl_timer_stop(sl, t);
	}
	spin_unlock_m2ua(&sl->lock);
}
STATIC INLINE void
sl_timer_start(struct sl *sl, const uint t)
{
	spin_lock_m2ua(&sl->lock);
	{
		__sl_timer_stop(sl, t);
		switch (t) {
		case tack:
			sl_start_timer_tack(sl, sl->config.tack);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_m2ua(&sl->lock);
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

M2UA_DECLARE_TIMER(xp, tack);
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
	spin_lock_m2ua(&xp->lock);
	{
		__xp_timer_stop(xp, t);
	}
	spin_unlock_m2ua(&xp->lock);
}
STATIC INLINE void
xp_timer_start(struct xp *xp, const uint t)
{
	spin_lock_m2ua(&xp->lock);
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
	spin_unlock_m2ua(&xp->lock);
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
sl_get_state(struct sl *sl)
{
	return (sl->state);
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
sl_get_l_state(struct sl *sl)
{
	return (sl->l_state);
}
STATIC INLINE t_uscalar_t
gp_get_l_state(struct gp *gp)
{
	return (gp->l_state);
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
sl_set_state(struct sl *sl, t_uscalar_t newstate)
{
	sl->state = newstate;
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
sl_set_l_state(struct sl *sl, t_uscalar_t newstate)
{
	sl->l_state = newstate;
}
STATIC INLINE void
gp_set_l_state(struct gp *gp, t_uscalar_t newstate)
{
	gp->l_state = newstate;
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
sl_get_flags(struct sl *sl)
{
	return (sl->flags);
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
sl_tst_flags(struct sl *sl, t_uscalar_t flags)
{
	return (sl->flags & flags);
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
sl_set_flags(struct sl *sl, t_uscalar_t flags)
{
	sl->flags |= flags;
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
sl_clr_flags(struct sl *sl, t_uscalar_t flags)
{
	sl->flags &= ~flags;
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

STATIC INLINE int gp_p_set_state(queue_t *q, struct gp *gp, t_uscalar_t newstate);
STATIC INLINE int gp_u_set_state(queue_t *q, struct gp *gp, t_uscalar_t newstate);

STATIC int
asp_as_p_recalc_state(queue_t *q, struct as *as)
{
	struct gp *gp;
	t_uscalar_t newstate;

	for (newstate = AS_ACTIVE; newstate && as->ap.u.counts[newstate] != 0; newstate--) ;
	switch (newstate) {
	case AS_DOWN:
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	case AS_INACTIVE:
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	case AS_WACK_ASPAC:
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	case AS_WACK_ASPIA:
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	case AS_ACTIVE:
		for (gp = as->gp.list; gp; gp = gp->as.next) {
			switch (gp_get_state(gp)) {
			}
		}
		break;
	}
	return (QR_DONE);
}

STATIC INLINE int sl_u_set_state(queue_t *, struct sl *, t_uscalar_t);

#if 0
STATIC int
sgp_as_u_recalc_state(queue_t *q, struct as *as)
{
	int err;
	struct gp *gp;
	struct sl *sl;
	t_uscalar_t newstate;

	for (newstate = AS_ACTIVE; newstate && as->ap.u.counts[newstate] != 0; newstate--) ;
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
				if ((xp = gp->spp.spp->xp))
					if ((err = m2ua_send_aspt_aspia_ack(q, xp,
									    &as->iid,
									    sizeof(as->iid), NULL,
									    0)))
						return (err);
				if ((err = gp_u_set_state(q, gp, AS_INACTIVE)))
					return (err);
			}
		}
		for (sl = as->sl.list; sl; sl = sl->as.next) {
			switch (sl_get_state(sl)) {
			case AS_WACK_ASPAC:
				if (newstate == AS_WACK_ASPAC)
					continue;
			case AS_WACK_ASPIA:
				if ((err = slu_disable_con(q, sl)))
					return (err);
				if ((err = sl_u_set_state(q, sl, AS_INACTIVE)))
					return (err);
				break;
			case AS_ACTIVE:
				if ((err = slu_error_ind(q, sl, 0, 0)))
					return (err);
				if ((err = sl_u_set_state(q, sl, AS_INACTIVE)))
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
				if ((xp = gp->spp.spp->xp))
					if ((err = m2ua_send_aspt_aspac_ack(q, xp,
									    as->sp.sp->tmode,
									    &as->iid,
									    sizeof(as->iid), NULL,
									    0)))
						return (err);
				if ((err = gp_u_set_state(q, gp, AS_ACTIVE)))
					return (err);
			}
		}
		for (sl = as->sl.list; sl; sl = sl->as.next) {
			switch (sl_get_state(sl)) {
			case AS_WACK_ASPAC:
				if ((err = slu_enable_con(q, sl)))
					return (err);
				if ((err = sl_u_set_state(q, sl, AS_ACTIVE)))
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
asp_as_u_recalc_state(queue_t *q, struct as *as)
{
	int err;
	struct ap *ap;
	struct gp *gp;
	t_uscalar_t oldstate = as_get_state(as), newstate;
	t_uscalar_t oldflags = as_get_flags(as), newflags = oldflags;

	assure(as->ap.numb > 0);
	ensure(oldstate <= AS_ACTIVE, return (-EFAULT));
	for (newstate = AS_ACTIVE; newstate && as->gp.u.counts[newstate] != 0; newstate--) ;
	/* propagate state changes to ASP */
	if ((newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA) && as->gp.u.c.active < as->minasp
	    && as->minasp > 1)
		newflags |= ASF_INSUFFICIENT_ASPS;
	if ((newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA) && as->gp.u.c.active == as->minasp
	    && as->minasp > 1)
		newflags |= ASF_MINIMUM_ASPS;
	if ((newstate == AS_WACK_ASPAC || newstate == AS_INACTIVE)
	    && (oldstate == AS_WACK_ASPIA || oldstate == AS_ACTIVE))
		newflags |= ASF_PENDING;
	if (newstate == AS_WACK_ASPIA || newstate == AS_ACTIVE)
		newflags &= ~ASF_PENDING;
	for (gp = as->gp.list; gp; gp = gp->as.next) {
		struct xp *xp = gp->spp.spp->xp;
		t_uscalar_t state = gp_get_state(gp);

		if (state == AS_DOWN)
			continue;
		/* notify of state change */
		switch (newstate) {
		default:
		case AS_ACTIVE:
		case AS_WACK_ASPIA:
			if ((newflags & ASF_INSUFFICIENT_ASPS)
			    && !(oldflags & ASF_INSUFFICIENT_ASPS))
				/* notify of insufficient ASPs */
				if ((err = m2ua_send_mgmt_ntfy(q, xp,
							       UA_STATUS_AS_INSUFFICIENT_ASPS,
							       NULL, NULL, 0, NULL, 0)))
					return (err);
			if (!(newflags & ASF_INSUFFICIENT_ASPS)
			    && (oldflags & ASF_INSUFFICIENT_ASPS))
				/* notify of AS active */
				if ((err = m2ua_send_mgmt_ntfy(q, xp,
							       UA_STATUS_AS_ACTIVE, NULL, NULL, 0,
							       NULL, 0)))
					return (err);
			if ((newflags & ASF_MINIMUM_ASPS) && !(oldflags & ASF_MINIMUM_ASPS))
				/* notify of minimum ASPs */
				if ((err = m2ua_send_mgmt_ntfy(q, xp,
							       UA_STATUS_AS_MINIMUM_ASPS, NULL,
							       NULL, 0, NULL, 0)))
					return (err);
			switch (oldstate) {
			default:
			case AS_DOWN:
			case AS_INACTIVE:
			case AS_WACK_ASPAC:
				/* withold AS_ACTIVE notification until sufficient ASPs */
				if (!(newflags & ASF_INSUFFICIENT_ASPS))
					/* notify of AS active */
					if ((err = m2ua_send_mgmt_ntfy(q, xp,
								       UA_STATUS_AS_ACTIVE, NULL,
								       NULL, 0, NULL, 0)))
						return (err);
				break;
			}
			break;
		case AS_DOWN:
		case AS_INACTIVE:
		case AS_WACK_ASPAC:
			switch (oldstate) {
			case AS_ACTIVE:
			case AS_WACK_ASPIA:
				/* don't notify during override transitions */
				if (newstate != AS_WACK_ASPAC
				    || as->sp.sp->tmode != UA_TMODE_OVERRIDE) {
					if ((newflags & ASF_PENDING)) {
						if (!(oldflags & ASF_PENDING))
							/* notify of AS pending */
							if ((err = m2ua_send_mgmt_ntfy(q, xp,
										       UA_STATUS_AS_PENDING,
										       NULL, NULL,
										       0, NULL, 0)))
								return (err);
					} else {
						/* notify of AS inactive */
						if ((err = m2ua_send_mgmt_ntfy(q, xp,
									       UA_STATUS_AS_INACTIVE,
									       NULL, NULL, 0, NULL,
									       0)))
							return (err);
					}
				}
				break;
			}
			break;
		}
	}
	if (oldstate == newstate)
		return (QR_DONE);
	/* propagate state changes to AS-P */
	for (ap = as->ap.list; ap; ap = ap->u.next) {
		ap->p.as->ap.u.counts[oldstate]--;
		ap->p.as->ap.u.counts[newstate]++;
		if ((err = asp_as_p_recalc_state(q, ap->p.as))) {
			ap->p.as->ap.u.counts[oldstate]++;
			ap->p.as->ap.u.counts[newstate]--;
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
sgp_as_p_recalc_state(queue_t *q, struct as *as)
{
	int err;
	struct ap *ap;
	t_uscalar_t newstate, oldstate = as_get_state(as);

	assure(as->ap.numb > 0);
	ensure(oldstate <= AS_ACTIVE, return (-EFAULT));
	for (newstate = AS_ACTIVE; newstate && as->gp.u.counts[newstate] != 0; newstate--) ;
	if (oldstate != newstate)
		for (ap = as->ap.list; ap; ap = ap->p.next) {
			ap->u.as->ap.u.counts[oldstate]--;
			ap->u.as->ap.u.counts[newstate]++;
			if ((err = asp_as_p_recalc_state(q, ap->u.as))) {
				ap->u.as->ap.u.counts[oldstate]++;
				ap->u.as->ap.u.counts[newstate]--;
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

/*
 *  Set the state of an ASP in an AS-U.
 */
STATIC INLINE int
gp_u_set_state(queue_t *q, struct gp *gp, const t_uscalar_t newstate)
{
	int err;
	struct as *as = gp->as.as;
	t_uscalar_t oldstate = gp_get_state(gp);

	if (newstate == oldstate)
		return (QR_DONE);
	as->gp.u.counts[oldstate]--;
	as->gp.u.counts[newstate]++;
	if ((err = asp_as_u_recalc_state(q, as))) {
		as->gp.u.counts[oldstate]++;
		as->gp.u.counts[newstate]--;
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
sl_u_set_state(queue_t *q, struct sl *sl, const t_uscalar_t newstate)
{
	int err;
	struct as *as = sl->as.as;
	t_uscalar_t oldstate = sl_get_state(sl);

	if (newstate == oldstate)
		return (QR_DONE);
	as->gp.u.counts[oldstate]--;
	as->gp.u.counts[newstate]++;
	if ((err = asp_as_u_recalc_state(q, as))) {
		as->gp.u.counts[oldstate]++;
		as->gp.u.counts[newstate]--;
		return (err);
	}
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA)
		sl_set_flags(sl, ASF_ACTIVE);
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA)
		sl_clr_flags(sl, ASF_ACTIVE);
	sl_set_state(sl, newstate);
	return (QR_DONE);
}

/*
 *  Set the state of the SGP in the AS-P
 */
STATIC INLINE int
gp_p_set_state(queue_t *q, struct gp *gp, const t_uscalar_t newstate)
{
	int err;
	struct as *as = gp->as.as;
	t_uscalar_t oldstate = gp_get_state(gp);
	t_uscalar_t oldwack = as->gp.u.c.wack_aspac + as->gp.u.c.wack_aspia, newwack;

	if (oldstate == newstate)
		return (QR_DONE);
	as->gp.u.counts[oldstate]--;
	as->gp.u.counts[newstate]++;
	newwack = as->gp.u.c.wack_aspac + as->gp.u.c.wack_aspia;
	if ((err = sgp_as_p_recalc_state(q, as))) {
		as->gp.u.counts[oldstate]++;
		as->gp.u.counts[newstate]--;
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
sl_p_set_state(queue_t *q, struct sl *sl, const t_uscalar_t newstate)
{
	int err;
	struct as *as = sl->as.as;
	t_uscalar_t oldstate = sl_get_state(sl);

	if (oldstate == newstate)
		return (QR_DONE);
	as->gp.u.counts[oldstate]--;
	as->gp.u.counts[newstate]++;
	if ((err = sgp_as_p_recalc_state(q, as))) {
		as->gp.u.counts[oldstate]++;
		as->gp.u.counts[newstate]--;
		return (err);
	}
	if (newstate == AS_ACTIVE || newstate == AS_WACK_ASPIA)
		sl_set_flags(sl, ASF_ACTIVE);
	if (newstate != AS_ACTIVE && newstate != AS_WACK_ASPIA)
		sl_clr_flags(sl, ASF_ACTIVE);
	sl_set_state(sl, newstate);
	return (QR_DONE);
}

STATIC INLINE int
asp_set_state(queue_t *q, struct spp *spp, const t_uscalar_t newstate)
{
	int err;
	struct gp *gp;
	t_uscalar_t oldstate = spp_get_state(spp);

	switch (newstate) {
	case ASP_WACK_ASPUP:
	case ASP_UP:
		switch (oldstate) {
		default:
			swerr();
			/* fall through */
		case ASP_DOWN:
		case ASP_WACK_ASPUP:
			/* move to the inactive state in all AS */
			for (gp = spp->gp.list; gp; gp = gp->spp.next)
				if ((err = gp_u_set_state(q, gp, ASP_INACTIVE)))
					return (err);
			/* fall through */
			spp_set_state(spp, ASP_WACK_ASPDN);
		case ASP_WACK_ASPDN:
		case ASP_UP:
			if ((err = m2ua_send_asps_aspup_ack(q, spp->xp, NULL, NULL, 0)))
				return (err);
			spp_set_state(spp, ASP_UP);
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
		case ASP_WACK_ASPDN:
			/* move to the down state in all AS */
			for (gp = spp->gp.list; gp; gp = gp->spp.next)
				if ((err = gp_u_set_state(q, gp, ASP_DOWN)))
					return (err);
			/* fall through */
		case ASP_WACK_ASPUP:
		case ASP_DOWN:
			if ((err = m2ua_send_asps_aspdn_ack(q, spp->xp, NULL, NULL, 0)))
				return (err);
			spp_set_state(spp, ASP_DOWN);
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
sgp_set_state(queue_t *q, struct spp *spp, const t_uscalar_t newstate)
{
	int err;
	struct gp *gp;
	t_uscalar_t oldstate = spp_get_state(spp);

	switch (newstate) {
	case ASP_DOWN:
		switch (oldstate) {
		case ASP_WACK_ASPUP:
			spp_timer_stop(spp, tack);
			spp_set_state(spp, newstate);
			/* fall through */
		case ASP_DOWN:
			return (QR_DONE);
		default:
		case ASP_WACK_ASPDN:
			spp_timer_stop(spp, tack);
			/* move to the down state in all AS */
			for (gp = spp->gp.list; gp; gp = gp->spp.next)
				if ((err = gp_p_set_state(q, gp, ASP_DOWN)))
					return (err);
			spp_set_state(spp, newstate);
			return (QR_DONE);
		case ASP_UP:
			/* unsolicited ASPDN Ack */
			/* move to the down state in all AS */
			for (gp = spp->gp.list; gp; gp = gp->spp.next)
				if ((err = gp_p_set_state(q, gp, ASP_DOWN)))
					return (err);
			/* try to bring it back up */
			if ((err = m2ua_send_asps_aspup_req(q, spp->xp, NULL, NULL, 0)))
				return (err);
			spp_timer_start(spp, tack);
			spp_set_state(spp, ASP_WACK_ASPUP);
			return (QR_DONE);
		}
	case ASP_WACK_ASPUP:
		if ((err = m2ua_send_asps_aspup_req(q, spp->xp, NULL, NULL, 0)))
			return (err);
		spp_timer_start(spp, tack);
		spp_set_state(spp, newstate);
		return (QR_DONE);
	case ASP_WACK_ASPDN:
		if ((err = m2ua_send_asps_aspdn_req(q, spp->xp, NULL, NULL, 0)))
			return (err);
		spp_timer_start(spp, tack);
		spp_set_state(spp, newstate);
		return (QR_DONE);
	case ASP_UP:
		switch (oldstate) {
		case ASP_WACK_ASPDN:
			spp_timer_stop(spp, tack);
			spp_set_state(spp, newstate);
			/* fall through */
		case ASP_UP:
			return (QR_DONE);
		default:
		case ASP_WACK_ASPUP:
			spp_timer_stop(spp, tack);
			/* fall through */
		case ASP_DOWN:
			/* move to the inactive state in all AS */
			for (gp = spp->gp.list; gp; gp = gp->spp.next)
				if ((err = gp_p_set_state(q, gp, ASP_INACTIVE)))
					return (err);
			spp_set_state(spp, newstate);
			return (QR_DONE);
		}
	}
	swerr();
	return (-EFAULT);
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
sl_tack_timeout(struct sl *sl)
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
STATIC INLINE struct sl *
ap_find_slu(struct ap *ap)
{
	struct sl *sl;

	/* return first available link */
	for (sl = ap->u.as->sl.list; sl && sl_tst_flags(sl, ASF_ACTIVE | ASF_PENDING);
	     sl = sl->as.next) ;
	return (sl);
}
STATIC INLINE struct sl *
ap_find_slu_next(struct ap *ap, struct sl *sl, const int command)
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
		for (; sl && sl_tst_flags(sl, ASF_ACTIVE | ASF_PENDING); sl = sl->as.next) ;
		return (sl);
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
 *  Selecting an SL-P.  Only override mode between SL-P makes sense for M2UA.
 */
STATIC INLINE struct sl *
ap_find_slp(struct ap *ap)
{
	struct sl *sl;

	/* return first available link */
	for (sl = ap->p.as->sl.list; sl && sl_tst_flags(sl, ASF_ACTIVE | ASF_PENDING);
	     sl = sl->as.next) ;
	return (sl);
}
STATIC INLINE struct sl *
ap_find_slp_next(struct ap *ap, struct sl *sl, const int command)
{
	return (NULL);
}

/*
 *  Selecting an SGP.  Only override mode between SGP makes sense for M2UA.
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
 *  Selecting an SG.  Only override mode between SG makes sense for M2UA.
 */
STATIC INLINE struct ap *
as_u_find_ap(struct as *as)
{
	struct ap *ap;

	/* return first available sg */
	for (ap = as->ap.list; ap && as_tst_flags(ap->u.as, ASF_ACTIVE | ASF_PENDING);
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
 *  Receive M2UA Messages
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  UA_MGMT_ERR
 *  -----------------------------------
 */
STATIC int
asp_recv_mgmt_err(queue_t *q, struct ua_msg *m)
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
sgp_recv_mgmt_err(queue_t *q, struct ua_msg *m)
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
STATIC int
asp_recv_mgmt_ntfy(queue_t *q, struct ua_msg *m)
{
	return (QR_DISABLE);
}
STATIC int
sgp_recv_mgmt_ntfy(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  UA_ASPS_ASPUP_REQ
 *  -----------------------------------
 */
STATIC int
asp_recv_asps_aspup_req(queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
sgp_recv_asps_aspup_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct gp *gp;
	struct sp *sp;
	struct spp *spp;

	fixme(("Need to check ASPID\n"));
	if (!(spp = xp->spp)) {
		if (!(sp = xp->sp))
			goto disable;
		/* If we are not yet associated with an spp but we are associated with an sp (must
		   be one or the other) then if there is an ASPID in the mssage then we can
		   discover whether this is a unique ASP and associate it with the correct (or a
		   new) spp.  If there is no ASPID in the message, we should refuse the ASPUP with
		   an ERR. */
		if (!m->aspid.ptr.c)
			goto needaspid;
		if (!m->aspid.val)
			goto badaspid;
		for (spp = sp->spp.list; spp && spp->aspid != m->aspid.val; spp = spp->sp.next) ;
		/* FEATURE: We create an ASP if a structure does not exist. This ASP is given
		   access to all of the AS defined for this SGP.  If one wants security, the ASP
		   should have never been allowed to connect. */
		if (!spp) {
			if (!
			    (spp = m2ua_alloc_spp(spp_get_id(0), M2UA_OBJ_TYPE_ASP, sp,
						  m->aspid.val, 0)))
				goto enomem;
		} else if (spp->xp)
			goto badaspid;
	} else {
		sp = spp->sp.sp;
		if (m->aspid.ptr.c) {
			struct spp *s;

			if (!spp->aspid) {
				if (!m->aspid.val)
					goto needaspid;
				for (s = sp->spp.list; s && s->aspid != m->aspid.val;
				     s = s->sp.next) ;
				if (s)
					goto badaspid;
				spp->aspid = m->aspid.val;
			} else if (spp->aspid != m->aspid.val)
				goto badaspid;
		} else if (!spp->aspid)
			goto needaspid;
	}
	switch (spp_get_state(spp)) {
	case ASP_DOWN:
		if (spp_tst_flags(spp, ASF_MGMT_BLOCKED))
			goto blocked;
		asp_set_state(q, spp, ASP_WACK_ASPUP);
		/* fall through */
	case ASP_WACK_ASPUP:
		for (gp = spp->gp.list; gp; gp = gp->spp.next) {
			struct as *as = gp->as.as;
			struct xp *xp = gp->spp.spp->xp;

			if (gp_get_state(gp) != AS_DOWN)
				continue;
			/* FEATURE: ASPs which have just come up should be immediately notified of
			   AS state, they can then use the notifications to determine their next
			   best action */
			if (as_tst_flags(as, ASF_ACTIVE)) {
				if ((err =
				     m2ua_send_mgmt_ntfy(q, xp,
							 UA_STATUS_AS_ACTIVE, NULL, NULL, 0,
							 NULL, 0)))
					return (err);
				if (as_tst_flags(as, ASF_INSUFFICIENT_ASPS) && as->minasp > 1)
					if ((err =
					     m2ua_send_mgmt_ntfy(q, xp,
								 UA_STATUS_AS_INSUFFICIENT_ASPS,
								 NULL, NULL, 0, NULL, 0)))
						return (err);
				if (as_tst_flags(as, ASF_MINIMUM_ASPS) && as->minasp > 1)
					if ((err =
					     m2ua_send_mgmt_ntfy(q, xp, UA_STATUS_AS_MINIMUM_ASPS,
								 NULL, NULL, 0, NULL, 0)))
						return (err);
			} else {
				if (as_tst_flags(as, ASF_PENDING)) {
					if ((err =
					     m2ua_send_mgmt_ntfy(q, xp, UA_STATUS_AS_PENDING, NULL,
								 NULL, 0, NULL, 0)))
						return (err);
				} else {
					if ((err =
					     m2ua_send_mgmt_ntfy(q, xp, UA_STATUS_AS_INACTIVE, NULL,
								 NULL, 0, NULL, 0)))
						return (err);
				}
			}
			if ((err = gp_u_set_state(q, gp, AS_INACTIVE)))
				return (err);
		}
		asp_set_state(q, spp, ASP_WACK_ASPDN);
		/* fall through */
	case ASP_WACK_ASPDN:
		todo(("Notify management that the ASP has come up\n"));
		asp_set_state(q, spp, ASP_UP);
		/* fall through */
	case ASP_UP:
		if ((err = m2ua_send_asps_aspup_ack(q, xp, NULL, NULL, 0)))
			return (err);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
      enomem:
	rare();
	return (-ENOMEM);
      blocked:
	rare();
	return (-EPERM);
      needaspid:
	rare();
	return (-ESRCH);
      badaspid:
	rare();
	return (-ENOENT);
      disable:
	swerr();
	return (QR_DISABLE);
}

/*
 *  UA_ASPS_ASPDN_REQ
 *  -----------------------------------
 */
STATIC int
asp_recv_asps_aspdn_req(queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
sgp_recv_asps_aspdn_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct gp *gp;
	struct spp *spp;

	if (!(spp = xp->spp)) {
		if (!(xp->sp))
			goto disable;
		return (-EPROTO);
	}
	if (m->aspid.ptr.c && m->aspid.val && m->aspid.val != spp->aspid) {
		/* FEATURE: As discussed on SIGTRAN list it might be advantageous to have an ASP
		   indicate ASP Down for an ASP other than itself.  To do so, it would include the
		   ASPID of the other ASP. So, if there is an ASPID in the message, we look for the 
		   other ASP. */
		for (spp = spp->sp.sp->spp.list; spp && spp->aspid != m->aspid.val;
		     spp = spp->sp.next) ;
		if (!spp || !spp->xp)
			goto badaspid;
		xp = spp->xp;
	}
	switch (spp_get_state(spp)) {
	case ASP_UP:
		asp_set_state(q, spp, ASP_WACK_ASPDN);
		/* fall through */
	case ASP_WACK_ASPDN:
		for (gp = spp->gp.list; gp; gp = gp->spp.next)
			if (gp_get_state(gp) != AS_DOWN)
				if ((err = gp_u_set_state(q, gp, AS_DOWN)))
					return (err);
		asp_set_state(q, spp, ASP_WACK_ASPUP);
		/* fall through */
	case ASP_WACK_ASPUP:
		todo(("Notify management that the ASP has gone down\n"));
		asp_set_state(q, spp, ASP_DOWN);
		/* fall through */
	case ASP_DOWN:
		if ((err = m2ua_send_asps_aspdn_ack(q, spp->xp, &spp->aspid, NULL, 0)))
			return (err);
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
      disable:
	return (QR_DISABLE);
      badaspid:
	return (-ENOENT);
}

/*
 *  UA_ASPS_HBEAT_REQ
 *  -----------------------------------
 */
STATIC int
asp_recv_asps_hbeat_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);

	return m2ua_send_asps_hbeat_ack(q, xp, m->hinfo.ptr.c, m->hinfo.len);
}
STATIC int
sgp_recv_asps_hbeat_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);

	return m2ua_send_asps_hbeat_ack(q, xp, m->hinfo.ptr.c, m->hinfo.len);
}

/*
 *  UA_ASPS_ASPUP_ACK
 *  -----------------------------------
 */
STATIC int
asp_recv_asps_aspup_ack(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	struct spp *spp;

	if (!(spp = xp->spp))
		goto disable;
	switch (spp_get_state(spp)) {
	case ASP_UP:
		/* ignore: probably a late ack to our ASPUP */
		return (QR_DONE);
	case ASP_WACK_ASPUP:
		todo(("Indicate to management that the ASP is up.\n"));
		return sgp_set_state(q, spp, ASP_UP);
	}
	return (-EPROTO);
      disable:
	return (QR_DISABLE);
}
STATIC int
sgp_recv_asps_aspup_ack(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  UA_ASPS_ASPDN_ACK
 *  -----------------------------------
 */
STATIC int
asp_recv_asps_aspdn_ack(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct spp *spp;

	if (!(spp = xp->spp))
		goto disable;
	switch (spp_get_state(spp)) {
	case ASP_WACK_ASPDN:
	case ASP_WACK_ASPUP:
		/* solicited ASPDN Ack */
		todo(("Indicate to management that the ASP is down.\n"));
		return sgp_set_state(q, spp, ASP_DOWN);
	case ASP_UP:
		/* unsolicited ASP Down: we attempt once to re-establish the ASP.  If the second
		   attempt fails (in state ASP_WACK_ASPUP) we notify management and remain in the
		   down state. */
		if ((err = m2ua_send_asps_aspup_req(q, xp, NULL, NULL, 0)))
			return (err);
		return sgp_set_state(q, spp, ASP_DOWN);
		return (QR_DONE);
	}
	rare();
      disable:
	return (QR_DISABLE);
}
STATIC int
sgp_recv_asps_aspdn_ack(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  UA_ASPS_HBEAT_ACK
 *  -----------------------------------
 */
STATIC int
asp_recv_asps_hbeat_ack(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	struct spp *spp;

	if ((spp = xp->spp)) {
		spp_timer_stop(spp, tbeat);
		spp_timer_start(spp, tidle);
	}
	return (QR_DONE);
}
STATIC int
sgp_recv_asps_hbeat_ack(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	struct spp *spp;

	if ((spp = xp->spp)) {
		spp_timer_stop(spp, tbeat);
		spp_timer_start(spp, tidle);
	}
	return (QR_DONE);
}

/*
 *  UA_ASPT_ASPAC_REQ
 *  -----------------------------------
 */
STATIC int
asp_recv_aspt_aspac_req(queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
sgp_recv_aspt_aspac_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct spp *spp;
	struct gp *gp, *g2;
	struct xp *x2;
	struct as *as;
	struct sp *sp;
	struct sl *sl;

	if (!(spp = xp->spp)) {
		if (!(xp->sp))
			goto disable;
		goto badstate;
	}
	if (!m->iid.ptr.c || !m->tmode.ptr.c)
		goto missing;
	if (m->iid.len / sizeof(uint32_t) < 1)
		goto badparm;
	if (!(gp = spp_find_gp(spp, m->iid.ptr.w, m->iid.len)))
		goto badiid;
	if (!(as = gp->as.as) || !(sp = as->sp.sp))
		goto efault;
	if (sp->tmode != m->tmode.val)
		goto badtmode;
	switch (gp_get_state(gp)) {
	case AS_DOWN:
		return (-EPROTO);
	case AS_INACTIVE:
		if (gp_tst_flags(gp, ASF_MGMT_BLOCKED))
			goto blocked;
		if ((err = gp_u_set_state(q, gp, AS_WACK_ASPAC)))
			return (err);
		/* fall through */
	case AS_WACK_ASPAC:
		switch (as_get_state(as)) {
		case AS_INACTIVE:
			swerr();
			return (QR_DONE);
		case AS_WACK_ASPAC:
			if (as->sp.sp->tmode != UA_TMODE_OVERRIDE)
				return (QR_DONE);
			/* look for activating ASP */
			for (g2 = as->gp.list; g2 && g2 != gp && gp_get_state(g2) != AS_WACK_ASPAC;
			     g2 = g2->as.next) ;
			if (g2) {
				if ((x2 = g2->spp.spp->xp)
				    && (err = m2ua_send_aspt_aspia_ack(q, x2, NULL, 0, NULL, 0)))
					return (err);
				if ((err = gp_u_set_state(q, g2, AS_INACTIVE)))
					return (err);
			}
			for (sl = as->sl.list; sl && sl_get_state(sl) != AS_WACK_ASPAC;
			     sl = sl->as.next) ;
			if (sl) {
				if ((err = slu_error_ind(q, sl, 0, 0)))
					return (err);
				if ((err = sl_u_set_state(q, sl, AS_INACTIVE)))
					return (err);
			}
			return (QR_DONE);
		case AS_WACK_ASPIA:
			if (as->sp.sp->tmode != UA_TMODE_OVERRIDE)
				break;
			/* look for deactivating ASP */
			for (g2 = as->gp.list; g2 && g2 != gp && gp_get_state(g2) != AS_WACK_ASPIA;
			     g2 = g2->as.next) ;
			if (g2) {
				if ((x2 = g2->spp.spp->xp)
				    && (err = m2ua_send_aspt_aspia_ack(q, x2, NULL, 0, NULL, 0)))
					return (err);
				if ((err = gp_u_set_state(q, g2, AS_INACTIVE)))
					return (err);
			}
			for (sl = as->sl.list; sl && sl_get_state(sl) != AS_WACK_ASPIA;
			     sl = sl->as.next) ;
			if (sl) {
				if ((err = slu_disable_con(q, sl)))
					return (err);
				if ((err = sl_u_set_state(q, sl, AS_INACTIVE)))
					return (err);
			}
			break;
		case AS_ACTIVE:
			if (as->sp.sp->tmode != UA_TMODE_OVERRIDE)
				break;
			/* look for active ASP */
			for (g2 = as->gp.list; g2 && g2 != gp && gp_get_state(g2) != AS_ACTIVE;
			     g2 = g2->as.next) ;
			if (g2) {
				if ((x2 = g2->spp.spp->xp)
				    && (err = m2ua_send_mgmt_ntfy(q, x2,
								  UA_STATUS_ALTERNATE_ASP_ACTIVE,
								  &spp->aspid, &as->iid,
								  sizeof(as->iid), NULL, 0)))
					return (err);
				if ((err = gp_u_set_state(q, g2, AS_INACTIVE)))
					return (err);
			}
			for (sl = as->sl.list; sl && sl_get_state(sl) != AS_ACTIVE;
			     sl = sl->as.next) ;
			if (sl) {
				if ((err = slu_error_ind(q, sl, 0, 0)))
					return (err);
				if ((err = sl_u_set_state(q, sl, AS_INACTIVE)))
					return (err);
			}
			break;
		}
		/* fall through */
	case AS_WACK_ASPIA:
		todo(("Notify management that ASP has gone active\n"));
		if ((err = gp_u_set_state(q, gp, AS_ACTIVE)))
			return (err);
	case AS_ACTIVE:
		if ((err = m2ua_send_aspt_aspac_ack(q, xp, as->sp.sp->tmode,
						    m->iid.ptr.w, m->iid.len / sizeof(uint32_t),
						    NULL, 0)))
			return (err);
		return (QR_DONE);
	}
      badstate:
	return (-EPROTO);
      efault:
	swerr();
	return (-EFAULT);
      blocked:
	return m2ua_send_aspt_aspia_ack(q, xp, m->iid.ptr.w, m->iid.len / sizeof(uint32_t), NULL,
					0);
      badtmode:
	return (-ENOSYS);
      badiid:
	return (-EIO);
      missing:
	return (-ENXIO);
      badparm:
	return (-EMSGSIZE);
      disable:
	return (QR_DISABLE);
}

/*
 *  UA_ASPT_ASPIA_REQ
 *  -----------------------------------
 */
STATIC int
asp_recv_aspt_aspia_req(queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
sgp_recv_aspt_aspia_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct spp *spp;
	struct gp *gp;
	struct as *as;
	struct sp *sp;

	if (!(spp = xp->spp)) {
		if (!(xp->sp))
			goto disable;
		goto badstate;
	}
	if (!m->iid.ptr.w)
		goto missing;
	if (!(gp = spp_find_gp(spp, m->iid.ptr.w, m->iid.len)))
		goto badiid;
	if (!(as = gp->as.as) || !(sp = as->sp.sp))
		goto efault;
	switch (gp_get_state(gp)) {
	case AS_DOWN:
		return (-EPROTO);
	case AS_ACTIVE:
		if ((err = gp_u_set_state(q, gp, AS_WACK_ASPIA)))
			return (err);
		/* fall through */
	case AS_WACK_ASPIA:
	case AS_WACK_ASPAC:
	case AS_INACTIVE:
		break;
	}
	gp_u_set_state(q, gp, ASP_WACK_ASPIA);
	if ((err = m2ua_send_aspt_aspia_ack(q, xp, m->iid.ptr.w, m->iid.len / sizeof(uint32_t),
					    NULL, 0)))
		return (err);
	gp_u_set_state(q, gp, ASP_INACTIVE);
      efault:
	swerr();
	return (-EFAULT);
      badstate:
	return (-EPROTO);
      badiid:
	return (-EIO);
      missing:
	return (-ENXIO);
      disable:
	return (QR_DISABLE);
}

/*
 *  UA_ASPT_ASPAC_ACK
 *  -----------------------------------
 */
STATIC int
asp_recv_aspt_aspac_ack(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	struct spp *spp;
	struct gp *gp;

	if (!(spp = xp->spp))
		goto disable;
	if (!m->iid.ptr.w)
		goto missing;
	if (!(gp = spp_find_gp(spp, m->iid.ptr.w, m->iid.len)))
		goto badiid;
	switch (gp_get_state(gp)) {
	case ASP_ACTIVE:
		return (QR_DONE);
	case ASP_WACK_ASPAC:
		/* complete activation */
		spp_set_state(gp->spp.spp, ASP_ACTIVE);
		return (QR_DONE);
	case ASP_INACTIVE:
		/* activate */
		spp_set_state(gp->spp.spp, ASP_ACTIVE);
		/* attempt deactivation */
		return (QR_DONE);
	}
	return (-EPROTO);
      badiid:
	return (-EIO);
      missing:
	return (-ENXIO);
      disable:
	return (QR_DISABLE);
}
STATIC int
sgp_recv_aspt_aspac_ack(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  UA_ASPT_ASPIA_ACK
 *  -----------------------------------
 */
STATIC int
asp_recv_aspt_aspia_ack(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct spp *spp;
	struct gp *gp;

	if (!(spp = xp->spp))
		goto disable;
	if (!m->iid.ptr.w)
		goto missing;
	if (!(gp = spp_find_gp(spp, m->iid.ptr.w, m->iid.len)))
		goto badiid;
	switch (gp_get_state(gp)) {
	case ASP_INACTIVE:
		/* ignore, probably late ack to our ASPIA */
		/* XXX: maybe we should inform management here */
		return (QR_DONE);
	case ASP_WACK_ASPIA:
		gp_p_set_state(q, gp, ASP_INACTIVE);
		return (QR_DONE);
	case ASP_WACK_ASPAC:
		/* ignore, wait for timer to fire */
		/* XXX: maybe we should inform management here */
		return (QR_DONE);
	case ASP_ACTIVE:
		/* This is an unsolicited ASP Inactive Ack.  It is the SGP forcing us to the
		   inactive state.  We must placed the ASP in the inactive state and then try to
		   bring it back up again. */
		if ((err = m2ua_send_aspt_aspac_req(q, xp, gp->as.as->sp.sp->tmode,
						    m->iid.ptr.w, m->iid.len / sizeof(uint32_t),
						    NULL, 0)))
			return (err);
		gp_p_set_state(q, gp, ASP_INACTIVE);
		gp_p_set_state(q, gp, ASP_WACK_ASPAC);
		return (QR_DONE);
	}
	return (-EOPNOTSUPP);
      badiid:
	return (-EIO);
      missing:
	return (-ENXIO);
      disable:
	return (QR_DISABLE);
}
STATIC int
sgp_recv_aspt_aspia_ack(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  UA_RKMM_REG_REQ
 *  -----------------------------------
 */
STATIC int
asp_recv_rkmm_reg_req(queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
sgp_recv_rkmm_reg_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	struct spp *spp;

	if (!(spp = xp->spp))
		goto disable;
	return (-EPROTO);
      disable:
	return (QR_DISABLE);
}

/*
 *  UA_RKMM_REG_RSP
 *  -----------------------------------
 */
STATIC int
asp_recv_rkmm_reg_rsp(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	struct spp *spp;

	if (!(spp = xp->spp))
		goto disable;
	return (-EPROTO);
      disable:
	return (QR_DISABLE);
}
STATIC int
sgp_recv_rkmm_reg_rsp(queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}

/*
 *  UA_RKMM_DEREG_REQ
 *  -----------------------------------
 */
STATIC int
asp_recv_rkmm_dereg_req(queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}
STATIC int
sgp_recv_rkmm_dereg_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	struct spp *spp;

	if (!(spp = xp->spp))
		goto disable;
	return (-EPROTO);
      disable:
	return (QR_DISABLE);
}

/*
 *  UA_RKMM_DEREG_RSP
 *  -----------------------------------
 */
STATIC int
asp_recv_rkmm_dereg_rsp(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	struct spp *spp;

	if (!(spp = xp->spp))
		goto disable;
	return (-EPROTO);
      disable:
	return (QR_DISABLE);
}
STATIC int
sgp_recv_rkmm_dereg_rsp(queue_t *q, struct ua_msg *m)
{
	return (-EOPNOTSUPP);
}

/*
 *  M2UA_MAUP_ESTAB_REQ:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_estab_req(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
sgp_recv_maup_estab_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct sl *slp;
	struct ap *ap;
	struct gp *asp, *sgp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(asp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (!gp_tst_flags(asp, ASF_ACTIVE))
		return (QR_DONE);
	if (!(as = asp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			switch (sl_get_l_state(slp)) {
			case SLS_WCON_RELREQ:
				/* The signalling link has failed and we are restarting it. */
				rare();
			case SLS_IDLE:
				if (!sl_tst_flags(slp, ASF_RETRIEVAL)) {
					if ((err = slp_start_req(q, slp, m)) < 0)
						return (err);
					gp_set_l_state(asp, SLS_WCON_EREQ);
					sl_set_l_state(slp, SLS_WCON_EREQ);
					return (QR_DONE);
				} else {
					/* Someone else is retrieving messages so we need to
					   indicate that the signalling link has failed (reason
					   will be unspecified). */
					if ((err = m2ua_send_maup_rel_ind(q, asp, NULL)) < 0)
						return (err);
					gp_set_l_state(asp, SLS_IDLE);
					return (QR_DONE);
				}
			case SLS_WCON_EREQ:
				/* The signalling link is already being established by some other
				   signalling link user. Wait for establishment confirmation. */
				gp_set_l_state(asp, SLS_WCON_EREQ);
				return (QR_DONE);
			case SLS_ESTABLISHED:
				/* The signalling link is already established by some signalling
				   link user. Indicate that the link is in service. */
				if ((err = m2ua_send_maup_estab_con(q, asp, NULL)) < 0)
					return (err);
				gp_set_l_state(asp, SLS_ESTABLISHED);
				return (QR_DONE);
			default:
				swerr();
				return (-EFAULT);
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (!(xp = sgp->spp.spp->xp))
				return (-EFAULT);
			switch (gp_get_l_state(sgp)) {
			case SLS_WCON_RELREQ:
				/* The signalling link has failed and we are restarting it. */
				rare();
			case SLS_IDLE:
				if (!gp_tst_flags(sgp, ASF_RETRIEVAL)) {
					if (canputnext(xp->oq)) {
						gp_set_l_state(asp, SLS_WCON_EREQ);
						gp_set_l_state(sgp, SLS_WCON_EREQ);
						putnext(xp->oq, m->mp);
						return (QR_ABSORBED);
					}
					return (-EBUSY);
				} else {
					/* Someone else is retrieving messages so we need to
					   indicate that the signalling link has failed (reason
					   will be unspecified). */
					if ((err = m2ua_send_maup_rel_ind(q, asp, NULL)) < 0)
						return (err);
					gp_set_l_state(asp, SLS_IDLE);
					return (QR_DONE);
				}
			case SLS_WCON_EREQ:
				/* The signalling link is already being established by some other
				   signalling link user. Wait for establishment confirmation. */
				gp_set_l_state(asp, SLS_WCON_EREQ);
				return (QR_DONE);
			case SLS_ESTABLISHED:
				/* The signalling link is already established by some signalling
				   link user. Indicate that the link is in service. */
				if ((err = m2ua_send_maup_estab_con(q, asp, NULL)) < 0)
					return (err);
				gp_set_l_state(asp, SLS_ESTABLISHED);
				return (QR_DONE);
			default:
				swerr();
				return (-EFAULT);
			}
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_ESTAB_CON:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_estab_con(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_get_l_state(slu) == SLS_WCON_EREQ) {
				if ((err = slu_in_service_ind(q, slu, m)))
					return (err);
				sl_set_l_state(slu, SLS_ESTABLISHED);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_get_l_state(asp) == SLS_WCON_EREQ) {
				if (!canputnext(asp->spp.spp->xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(asp->spp.spp->xp->oq, bp);
				gp_set_l_state(asp, SLS_ESTABLISHED);
			}
	}
	gp_set_l_state(sgp, SLS_ESTABLISHED);
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_estab_con(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_REL_REQ:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_rel_req(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
sgp_recv_maup_rel_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct sl *slp;
	struct ap *ap;
	struct gp *sgp, *asp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(asp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (!gp_tst_flags(asp, ASF_ACTIVE))
		return (QR_DONE);
	if (!(as = asp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			switch (sl_get_l_state(slp)) {
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if ((err = slp_stop_req(q, slp, m)))
					return (err);
				/* no confirmation from SL-P */
				sl_set_l_state(slp, SLS_WCON_RELREQ);
				/* fall through */
			case SLS_WCON_RELREQ:
				sl_set_l_state(slp, SLS_IDLE);
				/* fall through */
			case SLS_IDLE:
				if ((err = m2ua_send_maup_rel_con(q, asp, NULL)))
					return (err);
				/* FIXME: All local signalling link users that believe the link to
				   be active must be notified with a link failure with unspecified
				   cause, or (better) a local processor outage. */
				sl_set_l_state(slp, SLS_IDLE);
				break;
			default:
				swerr();
				return (-EFAULT);
			}
			gp_set_l_state(asp, SLS_IDLE);
			return (QR_DONE);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (!(xp = sgp->spp.spp->xp))
				return (-EFAULT);
			switch (gp_get_l_state(sgp)) {
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if (!canputnext(xp->oq))
					return (-EBUSY);
				putnext(xp->oq, m->mp);
				gp_set_l_state(sgp, SLS_IDLE);
				break;
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				/* The signalling link has already been put out of service by some
				   other signalling link user or we can't put it out of service.
				   Just silently accept the request. */
				if ((err = m2ua_send_maup_rel_con(q, asp, NULL)))
					return (err);
				break;
			default:
				swerr();
				return (-EFAULT);
			}
			gp_set_l_state(asp, SLS_IDLE);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_REL_CON:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_rel_con(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	assure(gp_get_l_state(sgp) == SLS_WCON_RELREQ);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			switch (sl_get_l_state(slu)) {
			case SLS_WCON_RELREQ:
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if ((err = slu_out_of_service_ind(q, slu, m)))
					return (err);
				sl_set_l_state(slu, SLS_IDLE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			switch (gp_get_l_state(asp)) {
			case SLS_WCON_RELREQ:
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if (!canputnext(asp->spp.spp->xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(asp->spp.spp->xp->oq, bp);
				gp_set_l_state(asp, SLS_IDLE);
			}
	}
	gp_set_l_state(sgp, SLS_IDLE);
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_rel_con(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_REL_IND:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_rel_ind(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	assure(gp_get_l_state(sgp) != SLS_IDLE);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1)) {
			if (!sl_tst_flags(slu, ASF_OPERATION_PENDING)) {
				if (sl_get_l_state(slu) != SLS_IDLE) {
					if ((err = slu_out_of_service_ind(q, slu, m)))
						return (err);
					sl_set_l_state(slu, SLS_IDLE);
					sl_set_flags(slu, ASF_OPERATION_PENDING);
				}
			}
		}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1)) {
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if (gp_get_l_state(asp) != SLS_IDLE) {
					if (!canputnext(asp->spp.spp->xp->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, m->mp)))
						return (-ENOBUFS);
					putnext(asp->spp.spp->xp->oq, bp);
					gp_set_l_state(asp, SLS_IDLE);
					gp_set_flags(asp, ASF_OPERATION_PENDING);
				}
			}
		}
	}
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			sl_clr_flags(slu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			gp_clr_flags(asp, ASF_OPERATION_PENDING);
	}
	gp_set_l_state(sgp, SLS_IDLE);
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_rel_ind(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_STATE_REQ:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_state_req(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
sgp_recv_maup_state_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct sl *slp;
	struct ap *ap;
	struct gp *sgp, *asp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w || !m->status.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(asp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (!gp_tst_flags(asp, ASF_ACTIVE))
		return (QR_DONE);
	if (!(as = asp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (!sl_tst_flags(slp, ASF_OPERATION_PENDING)) {
				switch (m->status.val) {
				case M2UA_STATUS_LPO_SET:
					/* FIXME: We can only set local processor outage at the
					   physical link if we are the only signalling link user. */
					if (!gp_tst_flags(asp, ASF_USR_PROC_OUTAGE)) {
						if (!sl_tst_flags(slp, ASF_USR_PROC_OUTAGE)) {
							if ((err = slp_local_processor_outage_req(q,
												  slp,
												  m)))
								return (err);
							sl_set_flags(slp, ASF_USR_PROC_OUTAGE);
						}
						gp_set_flags(asp, ASF_USR_PROC_OUTAGE);
					}
					break;
				case M2UA_STATUS_LPO_CLEAR:
					/* FIXME: We can only clr local processor outage at the
					   physical link if we are the only signalling link user. */
					if (gp_tst_flags(asp, ASF_USR_PROC_OUTAGE)) {
						if (sl_tst_flags(slp, ASF_USR_PROC_OUTAGE)) {
							if ((err = slp_resume_req(q, slp, m)))
								return (err);
							sl_clr_flags(slp, ASF_USR_PROC_OUTAGE);
						}
						gp_clr_flags(asp, ASF_USR_PROC_OUTAGE);
					}
					break;
				case M2UA_STATUS_EMER_SET:
					if (!gp_tst_flags(asp, ASF_EMERGENCY)) {
						if (!sl_tst_flags(slp, ASF_EMERGENCY)) {
							if ((err = slp_emergency_req(q, slp, m)))
								return (err);
							sl_set_flags(slp, ASF_EMERGENCY);
						}
						gp_set_flags(asp, ASF_EMERGENCY);
					}
					break;
				case M2UA_STATUS_EMER_CLEAR:
					if (gp_tst_flags(asp, ASF_EMERGENCY)) {
						if (sl_tst_flags(slp, ASF_EMERGENCY)) {
							if ((err = slp_emergency_ceases_req(q, slp,
											    m)))
								return (err);
							sl_clr_flags(slp, ASF_EMERGENCY);
						}
						gp_clr_flags(asp, ASF_EMERGENCY);
					}
					break;
				case M2UA_STATUS_FLUSH_BUFFERS:
					/* If the commend has already been effected, we simply set
					   our flag to indicate that we are interested in the
					   response. */
					if (!gp_tst_flags(asp, ASF_FLUSH_BUFFERS)) {
						if (!sl_tst_flags(slp, ASF_FLUSH_BUFFERS)) {
							if ((err = slp_clear_buffers_req(q, slp,
											 m)))
								return (err);
							sl_set_flags(slp, ASF_FLUSH_BUFFERS);
						}
						gp_set_flags(asp, ASF_FLUSH_BUFFERS);
					}
					break;
				case M2UA_STATUS_CONTINUE:
					if (gp_tst_flags(asp, ASF_RECOVERY)) {
						if (sl_tst_flags(slp, ASF_RECOVERY)) {
							if ((err = slp_continue_req(q, slp, m)))
								return (err);
							sl_clr_flags(slp,
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
						if (!sl_tst_flags(slp, ASF_CLEAR_RTB)) {
							if ((err = slp_clear_rtb_req(q, slp, m)))
								return (err);
							sl_set_flags(slp, ASF_CLEAR_RTB);
						}
						gp_set_flags(asp, ASF_CLEAR_RTB);
					}
					break;
				case M2UA_STATUS_AUDIT:
					/* ignore for now */
					break;
				case M2UA_STATUS_CONG_CLEAR:
					if (gp_tst_flags(asp, (ASF_CONG_ACCEPT | ASF_CONG_DISCARD))) {
						if (sl_tst_flags
						    (slp, (ASF_CONG_ACCEPT | ASF_CONG_DISCARD))) {
							if ((err = slp_no_congestion_req(q, slp,
											 m)))
								return (err);
							sl_clr_flags(slp,
								     (ASF_CONG_ACCEPT |
								      ASF_CONG_DISCARD));
						}
						gp_clr_flags(asp,
							     (ASF_CONG_ACCEPT | ASF_CONG_DISCARD));
					}
					break;
				case M2UA_STATUS_CONG_ACCEPT:
					if (!gp_tst_flags(asp, ASF_CONG_ACCEPT)) {
						if (!sl_tst_flags(slp, ASF_CONG_ACCEPT)) {
							if ((err = slp_congestion_accept_req(q, slp,
											     m)))
								return (err);
							sl_set_flags(slp, ASF_CONG_ACCEPT);
							sl_clr_flags(slp, ASF_CONG_DISCARD);
						}
						gp_set_flags(asp, ASF_CONG_ACCEPT);
						gp_clr_flags(asp, ASF_CONG_DISCARD);
					}
					break;
				case M2UA_STATUS_CONG_DISCARD:
					if (!gp_tst_flags(asp, ASF_CONG_DISCARD)) {
						if (!sl_tst_flags(slp, ASF_CONG_DISCARD)) {
							if ((err = slp_congestion_discard_req(q,
											      slp,
											      m)))
								return (err);
							sl_set_flags(slp, ASF_CONG_DISCARD);
							sl_clr_flags(slp, ASF_CONG_ACCEPT);
						}
						gp_set_flags(asp, ASF_CONG_DISCARD);
						gp_clr_flags(asp, ASF_CONG_ACCEPT);
					}
					break;
				default:
					return (-EINVAL);
				}
				sl_set_flags(slp, ASF_OPERATION_PENDING);
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
				if ((err = m2ua_send_maup_con(q, asp, m)))
					return (err);
				break;
			}
			sl_clr_flags(slp, ASF_OPERATION_PENDING);
			return (QR_DONE);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (!sgp->spp.spp->xp)
				return (-EFAULT);
			if (!canputnext(sgp->spp.spp->xp->oq))
				return (-EBUSY);
			putnext(sgp->spp.spp->xp->oq, m->mp);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_STATE_CON:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_state_con(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w || !m->status.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1)) {
			switch (m->status.val) {
			case M2UA_STATUS_LPO_SET:
			case M2UA_STATUS_LPO_CLEAR:
				break;
			case M2UA_STATUS_FLUSH_BUFFERS:
				if (sl_tst_flags(slu, ASF_FLUSH_BUFFERS))
					if ((err = slu_rb_cleared_ind(q, slu, m)))
						return (err);
				break;
			case M2UA_STATUS_CLEAR_RTB:
				if (sl_tst_flags(slu, ASF_CLEAR_RTB))
					if ((err = slu_rtb_cleared_ind(q, slu, m)))
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
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1)) {
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
				if (!canputnext(asp->spp.spp->xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(asp->spp.spp->xp->oq, bp);
				break;
			default:
				return (-EINVAL);
			}
		}
	}
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_state_con(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_STATE_IND:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_state_ind(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w || !m->event.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1)) {
			switch (m->event.val) {
			case M2UA_EVENT_RPO_ENTER:
				if (sl_tst_flags(slu, ASF_REM_PROC_OUTAGE))
					continue;
				if ((err = slu_remote_processor_outage_ind(q, slu, m)))
					return (err);
				sl_set_flags(slu, ASF_REM_PROC_OUTAGE);
				continue;
			case M2UA_EVENT_RPO_EXIT:
				if (!sl_tst_flags(slu, ASF_REM_PROC_OUTAGE))
					continue;
				if ((err = slu_remote_processor_recovered_ind(q, slu, m)))
					return (err);
				sl_clr_flags(slu, ASF_REM_PROC_OUTAGE);
				continue;
			case M2UA_EVENT_LPO_ENTER:
				if (sl_tst_flags(slu, ASF_LOC_PROC_OUTAGE))
					continue;
				if ((err = slu_local_processor_outage_ind(q, slu, m)))
					return (err);
				sl_set_flags(slu, ASF_LOC_PROC_OUTAGE);
				continue;
			case M2UA_EVENT_LPO_EXIT:
				if (!sl_tst_flags(slu, ASF_LOC_PROC_OUTAGE))
					continue;
				if ((err = slu_local_processor_recovered_ind(q, slu, m)))
					return (err);
				sl_clr_flags(slu, ASF_LOC_PROC_OUTAGE);
				continue;
			default:
				return (-EINVAL);
			}
		}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1)) {
			switch (m->event.val) {
			case M2UA_EVENT_RPO_ENTER:
				if (sl_tst_flags(slu, ASF_REM_PROC_OUTAGE))
					continue;
				break;
			case M2UA_EVENT_RPO_EXIT:
				if (!sl_tst_flags(slu, ASF_REM_PROC_OUTAGE))
					continue;
				break;
			case M2UA_EVENT_LPO_ENTER:
				if (sl_tst_flags(slu, ASF_LOC_PROC_OUTAGE))
					continue;
				break;
			case M2UA_EVENT_LPO_EXIT:
				if (!sl_tst_flags(slu, ASF_LOC_PROC_OUTAGE))
					continue;
				break;
			default:
				return (-EINVAL);
			}
			if (!canputnext(asp->spp.spp->xp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, m->mp)))
				return (-ENOBUFS);
			putnext(asp->spp.spp->xp->oq, bp);
		}
	}
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_state_ind(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_RETR_REQ:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_retr_req(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}
STATIC int
sgp_recv_maup_retr_req(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct ap *ap;
	struct sl *slp;
	struct gp *asp, *sgp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w || !m->action.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(asp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (!gp_tst_flags(asp, ASF_ACTIVE))
		return (QR_DONE);
	if (!(as = asp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			switch (m->action.val) {
			case M2UA_ACTION_RTRV_BSN:
				/* If the commend has already been effected, we simply set our flag 
				   to indicate that we are interested in the response. */
				if (!sl_tst_flags(slp, ASF_BSNT_REQUEST)) {
					if ((err = slp_retrieve_bsnt_req(q, slp, m)))
						return (err);
					sl_set_flags(slp, ASF_BSNT_REQUEST);
				}
				gp_set_flags(asp, ASF_BSNT_REQUEST);
				break;
			case M2UA_ACTION_RTRV_MSGS:
				if (!m->seqno.ptr.w)
					return (-ENXIO);
				if (sl_get_l_state(slp) == SLS_IDLE
				    && !sl_tst_flags(slp, ASF_RETRIEVAL)) {
					if ((err = slp_retrieval_request_and_fsnc_req(q, slp, m)))
						return (err);
					sl_set_flags(slp, ASF_RETRIEVAL);
					gp_set_flags(asp, ASF_RETRIEVAL);
				} else {
					if ((err = m2ua_send_maup_retr_con(q, asp, NULL)))
						return (err);
				}
				break;
			default:
				return (-EINVAL);
			}
			return (QR_DONE);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (!sgp->spp.spp->xp)
				return (-EFAULT);
			if (!canputnext(sgp->spp.spp->xp->oq))
				return (-EBUSY);
			putnext(sgp->spp.spp->xp->oq, m->mp);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_RETR_CON:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_retr_con(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w || !m->action.ptr.w || !m->result.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1)) {
			switch (m->action.val) {
			case M2UA_ACTION_RTRV_BSN:
				if (!sl_tst_flags(slu, ASF_BSNT_REQUEST))
					continue;
				switch (m->result.val) {
				case UA_RESULT_SUCCESS:
					if (!m->seqno.ptr.w)
						return (-ENXIO);
					if ((err = slu_bsnt_ind(q, slu, m)))
						return (err);
					break;
				case UA_RESULT_FAILURE:
					if ((err = slu_bsnt_not_retrievable_ind(q, slu, m)))
						return (err);
					break;
				default:
					return (-EINVAL);
				}
				sl_clr_flags(slu, ASF_BSNT_REQUEST);
				break;
			case M2UA_ACTION_RTRV_MSGS:
				if (!sl_tst_flags(slu, ASF_RETRIEVAL))
					continue;
				switch (m->result.val) {
				case UA_RESULT_SUCCESS:
					continue;
				case UA_RESULT_FAILURE:
					if ((err = slu_retrieval_not_possible_ind(q, slu, m)))
						return (err);
					break;
				}
				sl_clr_flags(slu, ASF_RETRIEVAL);
				break;
			default:
				return (-EINVAL);
			}
		}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1)) {
			switch (m->action.val) {
			case M2UA_ACTION_RTRV_BSN:
				if (!gp_tst_flags(asp, ASF_BSNT_REQUEST))
					continue;
				switch (m->result.val) {
				case UA_RESULT_SUCCESS:
					if (!m->seqno.ptr.w)
						return (-ENXIO);
					break;
				case UA_RESULT_FAILURE:
					break;
				default:
					return (-EINVAL);
				}
				if (!canputnext(asp->spp.spp->xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(asp->spp.spp->xp->oq, bp);
				gp_clr_flags(asp, ASF_BSNT_REQUEST);
				break;
			case M2UA_ACTION_RTRV_MSGS:
				if (!gp_tst_flags(asp, ASF_RETRIEVAL))
					continue;
				switch (m->result.val) {
				case UA_RESULT_SUCCESS:
					continue;
				case UA_RESULT_FAILURE:
					break;
				default:
					return (-EINVAL);
				}
				if (!canputnext(asp->spp.spp->xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(asp->spp.spp->xp->oq, bp);
				gp_clr_flags(asp, ASF_RETRIEVAL);
				break;
			default:
				return (-EINVAL);
			}
		}
	}
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_retr_con(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_RETR_IND:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_retr_ind(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w || (!m->data1.ptr.w && !m->data2.ptr.w))
		return (-ENXIO);
	if (m->data1.ptr.w && m->data2.ptr.w)
		return (-EBADMSG);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_tst_flags(slu, ASF_RETRIEVAL))
				if ((err = slu_retrieved_message_ind(q, slu, m)))
					return (err);
		for (sgp = ap_find_asp(ap); sgp; sgp = ap_find_asp_next(ap, sgp, 1))
			if (gp_tst_flags(sgp, ASF_RETRIEVAL)) {
				if (!canputnext(sgp->spp.spp->xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(sgp->spp.spp->xp->oq, bp);
			}
	}
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_retr_ind(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_RETR_COMP_IND:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_retr_comp_ind(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w)
		return (-ENXIO);
	if (m->data1.ptr.w && m->data2.ptr.w)
		return (-EBADMSG);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_tst_flags(slu, ASF_RETRIEVAL)) {
				if ((err = slu_retrieval_complete_ind(q, slu, m)))
					return (err);
				sl_clr_flags(slu, ASF_RETRIEVAL);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(sgp, ASF_RETRIEVAL)) {
				if (!canputnext(asp->spp.spp->xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(asp->spp.spp->xp->oq, bp);
				gp_clr_flags(sgp, ASF_RETRIEVAL);
			}
	}
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_retr_comp_ind(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_CONG_IND:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_cong_ind(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w || !m->cong.ptr.w || !m->disc.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	if (m->cong.val > 4 || m->disc.val > 4)
		return (-EINVAL);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1)) {
			if (sl_get_l_state(slu) == SLS_ESTABLISHED) {
				if (!m->cong.val && !m->disc.val) {
					if ((err = slu_link_congestion_ceased_ind(q, slu, m)))
						return (err);
				} else {
					if ((err = slu_link_congested_ind(q, slu, m)))
						return (err);
				}
			}
		}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1)) {
			if (sl_get_l_state(slu) == SLS_ESTABLISHED) {
				if (!canputnext(asp->spp.spp->xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(asp->spp.spp->xp->oq, bp);
			}
		}
	}
	return (QR_DONE);
}
STATIC int
sgp_recv_maup_cong_ind(queue_t *q, struct ua_msg *m)
{
	return (-EPROTO);
}

/*
 *  M2UA_MAUP_DATA:
 *  -----------------------------------
 */
STATIC int
asp_recv_maup_data(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w || (!m->data1.ptr.w && !m->data2.ptr.w))
		return (-ENXIO);
	if (m->data1.ptr.w && m->data2.ptr.w)
		return (-EBADMSG);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	if ((m->data1.ptr.w && m->data1.len < 4) || (m->data2.ptr.w && m->data2.len < 4))
		return (-EINVAL);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1)) {
			if (sl_get_l_state(slu) == SLS_ESTABLISHED) {
				if (!sl_tst_flags(slu, ASF_OPERATION_PENDING)) {
					if ((err = slu_pdu_ind(q, slu, m)))
						return (err);
					sl_set_flags(slu, ASF_OPERATION_PENDING);
				}
				if (m->corid.ptr.w)
					if ((err = m2ua_send_maup_data_ack(q, sgp, m)))
						return (err);
				sl_clr_flags(slu, ASF_OPERATION_PENDING);
				return (QR_DONE);
			}
		}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1)) {
			if (gp_get_l_state(asp) == SLS_ESTABLISHED) {
				if (!canputnext(asp->spp.spp->xp->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, m->mp)))
					return (-ENOBUFS);
				putnext(asp->spp.spp->xp->oq, bp);
				return (QR_DONE);
			}
		}
	}
	return (-EFAULT);
}
STATIC int
sgp_recv_maup_data(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct sl *slp;
	struct ap *ap;
	struct gp *asp, *sgp;

	if (!xp->spp)
		return (QR_DISABLE);
	if ((!m->iid.ptr.w) || (!m->data1.ptr.w && !m->data2.ptr.w))
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(asp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (!gp_tst_flags(asp, ASF_ACTIVE))
		return (QR_DONE);
	if (!(as = asp->as.as))
		return (-EFAULT);
	if ((m->data1.ptr.w && m->data2.ptr.w)
	    || ((m->data1.ptr.w && m->data1.len < 4) || (m->data2.ptr.w && m->data2.len < 4)))
		return (-EINVAL);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (sl_get_l_state(slp) == SLS_ESTABLISHED) {
				if (!sl_tst_flags(slp, ASF_OPERATION_PENDING)) {
					if ((err = slp_pdu_req(q, slp, m)))
						return (err);
					sl_set_flags(slp, ASF_OPERATION_PENDING);
				}
				if (m->corid.ptr.w)
					if ((err = m2ua_send_maup_data_ack(q, asp, m)))
						return (err);
				sl_clr_flags(slp, ASF_OPERATION_PENDING);
				return (QR_DONE);
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (gp_get_l_state(sgp) == SLS_ESTABLISHED) {
				if (!sgp->spp.spp->xp)
					return (-EFAULT);
				if (!canputnext(sgp->spp.spp->xp->oq))
					return (-EBUSY);
				putnext(sgp->spp.spp->xp->oq, m->mp);
				return (QR_DONE);
			}
		}
	}
	return (-EFAULT);
}

/*
 *  M2UA_MAUP_DATA_ACK
 *  -----------------------------------
 */
STATIC int
slu_recv_maup_data_ack(queue_t *q, struct sl *sl, struct ua_msg *m)
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
slp_recv_maup_data_ack(queue_t *q, struct sl *sl, struct ua_msg *m)
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
asp_recv_maup_data_ack(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct gp *asp, *sgp;
	struct ap *ap;
	struct sl *slu;
	mblk_t *bp;

	if (!xp->spp)
		return (QR_DISABLE);
	if (!m->iid.ptr.w || !m->corid.ptr.w)
		return (-ENXIO);
	if (m->data1.ptr.w && m->data2.ptr.w)
		return (-EBADMSG);
	if (m->iid.len != sizeof(uint32_t)
	    || !(sgp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (gp_get_state(sgp) != AS_ACTIVE)
		return (QR_DONE);
	if (!(as = sgp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1)) {
			if ((err = slu_recv_maup_data_ack(q, slu, m)))
				return (err);
			return (QR_DONE);
		}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1)) {
			if (!canputnext(asp->spp.spp->xp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, m->mp)))
				return (-ENOBUFS);
			putnext(asp->spp.spp->xp->oq, bp);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}
STATIC int
sgp_recv_maup_data_ack(queue_t *q, struct ua_msg *m)
{
	struct xp *xp = XP_PRIV(q);
	int err;
	struct as *as;
	struct sl *slp;
	struct ap *ap;
	struct gp *asp, *sgp;

	if (!xp->spp)
		return (QR_DISABLE);
	if ((!m->iid.ptr.w) || !m->corid.ptr.w)
		return (-ENXIO);
	if (m->iid.len != sizeof(uint32_t)
	    || !(asp = spp_find_gp(xp->spp, m->iid.ptr.w, m->iid.len)))
		return (-EIO);
	if (!gp_tst_flags(asp, ASF_ACTIVE))
		return (QR_DONE);
	if (!(as = asp->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if ((err = slp_recv_maup_data_ack(q, slp, m)))
				return (err);
			return (QR_DONE);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (!sgp->spp.spp->xp)
				return (-EFAULT);
			if (!canputnext(sgp->spp.spp->xp->oq))
				return (-EBUSY);
			putnext(sgp->spp.spp->xp->oq, m->mp);
			return (QR_DONE);
		}
	}
	return (-EFAULT);
}

static int
asp_recv_msg(queue_t *q, struct ua_msg *m)
{
	switch (m->class) {
	case UA_CLASS_MGMT:	/* UA Management (MGMT) Message */
		switch (m->type) {
		case UA_MGMT_ERR:
			return asp_recv_mgmt_err(q, m);
		case UA_MGMT_NTFY:
			return asp_recv_mgmt_ntfy(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_XFER:	/* M3UA Data transfer message */
		return (-ENOPROTOOPT);
	case UA_CLASS_SNMM:	/* Signalling Network Mgmt (SNM) Messages */
		return (-ENOPROTOOPT);
	case UA_CLASS_ASPS:	/* ASP State Maintenance (ASPSM) Messages */
		switch (m->type) {
		case UA_ASPS_ASPUP_REQ:
			return asp_recv_asps_aspup_req(q, m);
		case UA_ASPS_ASPDN_REQ:
			return asp_recv_asps_aspdn_req(q, m);
		case UA_ASPS_HBEAT_REQ:
			return asp_recv_asps_hbeat_req(q, m);
		case UA_ASPS_ASPUP_ACK:
			return asp_recv_asps_aspup_ack(q, m);
		case UA_ASPS_ASPDN_ACK:
			return asp_recv_asps_aspdn_ack(q, m);
		case UA_ASPS_HBEAT_ACK:
			return asp_recv_asps_hbeat_ack(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_ASPT:	/* ASP Traffic Maintenance (ASPTM) Messages */
		switch (m->type) {
		case UA_ASPT_ASPAC_REQ:
			return asp_recv_aspt_aspac_req(q, m);
		case UA_ASPT_ASPIA_REQ:
			return asp_recv_aspt_aspia_req(q, m);
		case UA_ASPT_ASPAC_ACK:
			return asp_recv_aspt_aspac_ack(q, m);
		case UA_ASPT_ASPIA_ACK:
			return asp_recv_aspt_aspia_ack(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_Q921:	/* Q.931 User Part Messages */
		return (-ENOPROTOOPT);
	case UA_CLASS_MAUP:	/* M2UA Messages */
		switch (m->type) {
		case M2UA_MAUP_ESTAB_REQ:
			return asp_recv_maup_estab_req(q, m);
		case M2UA_MAUP_ESTAB_CON:
			return asp_recv_maup_estab_con(q, m);
		case M2UA_MAUP_REL_REQ:
			return asp_recv_maup_rel_req(q, m);
		case M2UA_MAUP_REL_CON:
			return asp_recv_maup_rel_con(q, m);
		case M2UA_MAUP_REL_IND:
			return asp_recv_maup_rel_ind(q, m);
		case M2UA_MAUP_STATE_REQ:
			return asp_recv_maup_state_req(q, m);
		case M2UA_MAUP_STATE_CON:
			return asp_recv_maup_state_con(q, m);
		case M2UA_MAUP_STATE_IND:
			return asp_recv_maup_state_ind(q, m);
		case M2UA_MAUP_RETR_REQ:
			return asp_recv_maup_retr_req(q, m);
		case M2UA_MAUP_RETR_CON:
			return asp_recv_maup_retr_con(q, m);
		case M2UA_MAUP_RETR_IND:
			return asp_recv_maup_retr_ind(q, m);
		case M2UA_MAUP_RETR_COMP_IND:
			return asp_recv_maup_retr_comp_ind(q, m);
		case M2UA_MAUP_CONG_IND:
			return asp_recv_maup_cong_ind(q, m);
		case M2UA_MAUP_DATA:
			return asp_recv_maup_data(q, m);
		case M2UA_MAUP_DATA_ACK:
			return asp_recv_maup_data_ack(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
		return (-ENOPROTOOPT);
	case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
		return (-ENOPROTOOPT);
	case UA_CLASS_RKMM:	/* Routing Key Management Messages */
		switch (m->type) {
		case UA_RKMM_REG_REQ:
			return asp_recv_rkmm_reg_req(q, m);
		case UA_RKMM_REG_RSP:
			return asp_recv_rkmm_reg_rsp(q, m);
		case UA_RKMM_DEREG_REQ:
			return asp_recv_rkmm_dereg_req(q, m);
		case UA_RKMM_DEREG_RSP:
			return asp_recv_rkmm_dereg_rsp(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
		return (-ENOPROTOOPT);
	case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
		return (-ENOPROTOOPT);
	}
	return (-ENOPROTOOPT);
}

static int
sgp_recv_msg(queue_t *q, struct ua_msg *m)
{
	switch (m->class) {
	case UA_CLASS_MGMT:	/* UA Management (MGMT) Message */
		switch (m->type) {
		case UA_MGMT_ERR:
			return sgp_recv_mgmt_err(q, m);
		case UA_MGMT_NTFY:
			return sgp_recv_mgmt_ntfy(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_XFER:	/* M3UA Data transfer message */
		return (-ENOPROTOOPT);
	case UA_CLASS_SNMM:	/* Signalling Network Mgmt (SNM) Messages */
		return (-ENOPROTOOPT);
	case UA_CLASS_ASPS:	/* ASP State Maintenance (ASPSM) Messages */
		switch (m->type) {
		case UA_ASPS_ASPUP_REQ:
			return sgp_recv_asps_aspup_req(q, m);
		case UA_ASPS_ASPDN_REQ:
			return sgp_recv_asps_aspdn_req(q, m);
		case UA_ASPS_HBEAT_REQ:
			return sgp_recv_asps_hbeat_req(q, m);
		case UA_ASPS_ASPUP_ACK:
			return sgp_recv_asps_aspup_ack(q, m);
		case UA_ASPS_ASPDN_ACK:
			return sgp_recv_asps_aspdn_ack(q, m);
		case UA_ASPS_HBEAT_ACK:
			return sgp_recv_asps_hbeat_ack(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_ASPT:	/* ASP Traffic Maintenance (ASPTM) Messages */
		switch (m->type) {
		case UA_ASPT_ASPAC_REQ:
			return sgp_recv_aspt_aspac_req(q, m);
		case UA_ASPT_ASPIA_REQ:
			return sgp_recv_aspt_aspia_req(q, m);
		case UA_ASPT_ASPAC_ACK:
			return sgp_recv_aspt_aspac_ack(q, m);
		case UA_ASPT_ASPIA_ACK:
			return sgp_recv_aspt_aspia_ack(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_Q921:	/* Q.931 User Part Messages */
		return (-ENOPROTOOPT);
	case UA_CLASS_MAUP:	/* M2UA Messages */
		switch (m->type) {
		case M2UA_MAUP_ESTAB_REQ:
			return sgp_recv_maup_estab_req(q, m);
		case M2UA_MAUP_ESTAB_CON:
			return sgp_recv_maup_estab_con(q, m);
		case M2UA_MAUP_REL_REQ:
			return sgp_recv_maup_rel_req(q, m);
		case M2UA_MAUP_REL_CON:
			return sgp_recv_maup_rel_con(q, m);
		case M2UA_MAUP_REL_IND:
			return sgp_recv_maup_rel_ind(q, m);
		case M2UA_MAUP_STATE_REQ:
			return sgp_recv_maup_state_req(q, m);
		case M2UA_MAUP_STATE_CON:
			return sgp_recv_maup_state_con(q, m);
		case M2UA_MAUP_STATE_IND:
			return sgp_recv_maup_state_ind(q, m);
		case M2UA_MAUP_RETR_REQ:
			return sgp_recv_maup_retr_req(q, m);
		case M2UA_MAUP_RETR_CON:
			return sgp_recv_maup_retr_con(q, m);
		case M2UA_MAUP_RETR_IND:
			return sgp_recv_maup_retr_ind(q, m);
		case M2UA_MAUP_RETR_COMP_IND:
			return sgp_recv_maup_retr_comp_ind(q, m);
		case M2UA_MAUP_CONG_IND:
			return sgp_recv_maup_cong_ind(q, m);
		case M2UA_MAUP_DATA_ACK:
			return sgp_recv_maup_data_ack(q, m);
		case M2UA_MAUP_DATA:
			return sgp_recv_maup_data(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_CNLS:	/* SUA Connectionless Messages */
		return (-ENOPROTOOPT);
	case UA_CLASS_CONS:	/* SUA Connection Oriented Messages */
		return (-ENOPROTOOPT);
	case UA_CLASS_RKMM:	/* Routing Key Management Messages */
		switch (m->type) {
		case UA_RKMM_REG_REQ:
			return sgp_recv_rkmm_reg_req(q, m);
		case UA_RKMM_REG_RSP:
			return sgp_recv_rkmm_reg_rsp(q, m);
		case UA_RKMM_DEREG_REQ:
			return sgp_recv_rkmm_dereg_req(q, m);
		case UA_RKMM_DEREG_RSP:
			return sgp_recv_rkmm_dereg_rsp(q, m);
		}
		return (-EOPNOTSUPP);
	case UA_CLASS_TDHM:	/* TUA Dialog Handling Mesages */
		return (-ENOPROTOOPT);
	case UA_CLASS_TCHM:	/* TUA Component Handling Messages */
		return (-ENOPROTOOPT);
	}
	return (-ENOPROTOOPT);
}

STATIC int
m2ua_recv_msg(struct xp *xp, queue_t *q, mblk_t *mp)
{
	int err, uaerr = 0;
	struct sp *sp;
	struct spp *spp;
	struct m2ua_msg msg;

	if ((err = m2ua_dec_msg(mp, &msg)) >= 0) {
		if ((spp = xp->spp)) {
			switch (spp->type) {
			case M2UA_OBJ_TYPE_ASP:
				err = sgp_recv_msg(q, &msg);
				break;
			case M2UA_OBJ_TYPE_SGP:
				err = asp_recv_msg(q, &msg);
				break;
#if 0
			case M2UA_OBJ_TYPE_SPP:
				err = spp_recv_msg(q, &msg);
				break;
#endif
			default:
				goto efault;
			}
		} else if ((sp = xp->sp)) {
			switch (sp->type) {
			case M2UA_OBJ_TYPE_SP:
				/* Unknown ASP connecting */
				err = sgp_recv_msg(q, &msg);
				break;
			case M2UA_OBJ_TYPE_SG:
				goto disable;
			default:
				goto efault;
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
		break;
		uaerr = UA_ECODE_INVALID_IID;
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
	return m2ua_send_mgmt_err(q, xp, uaerr, mp->b_rptr, msgdsize(mp));
      disable:
	swerr();
	return (QR_DISABLE);
      efault:
	swerr();
	return (-EFAULT);
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

/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC int
slu_info_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (sl_get_i_state(sl)) {
	case LMI_ENABLED:
	case LMI_DISABLED:
	case LMI_ENABLE_PENDING:
	case LMI_DISABLE_PENDING:
		return slu_info_ack(q, sl, (unsigned char *) &sl->add, sizeof(sl->add));
	default:
		return slu_info_ack(q, sl, NULL, 0);
	}
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, sl));
	goto error;
      error:
	return slu_error_ack(q, sl, p->lmi_primitive, err);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC int
slu_attach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	struct as *as;
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct m2ua_addr add;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sizeof(add))
		goto badprim;
	if (sl_get_i_state(sl) != LMI_UNATTACHED)
		goto outstate;
	if (sl_get_state(sl) != AS_DOWN)
		goto outstate;
	sl_set_i_state(sl, LMI_ATTACH_PENDING);
	bcopy((p + 1), &add, sizeof(add));
	for (as = master.as.list; as; as = as->next) {
		if (as->type != M2UA_OBJ_TYPE_AS_U)
			continue;
		if (as->sp.sp->id == add.spid && as->add.sdli == add.sdli)
			break;
	}
	if (!as)
		/* FIXME: Instead of generating an error here, we need to perform dynamic
		   allocation and configuration of signalling link providers.  We do that by
		   passing the LMI_ATTACH_REQ primitive upstream on the management Stream, but
		   prefixed with an upper multiplex identifier (major and minor device number). The 
		   management Stream uses input/output controls to effect the attachment and either 
		   returns an LMI_OK_ACK or LMI_ERROR_ACK prefixed with the device number of the
		   requesting Stream.  The requesting Stream then researches the list (as above)
		   and either completes the attachment or fails. */
		goto badppa;
	/* link SL-U to AS-U */
	if ((sl->as.next = as->sl.list))
		sl->as.next->as.prev = &sl->as.next;
	sl->as.prev = &as->sl.list;
	sl->as.as = as_get(as);
	as->sl.list = sl_get(sl);
	as->sl.numb++;
	if ((err = sl_u_set_state(q, sl, AS_INACTIVE)))
		goto error;
	return slu_ok_ack(q, sl, p->lmi_primitive);
      badppa:
	err = LMI_BADPPA;
	ptrace(("%s: %p: PROTO: bad PPA\n", DRV_NAME, sl));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: PROTO: would place i/f out of state\n", DRV_NAME, sl));
	goto error;
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, sl));
	goto error;
      error:
	return slu_error_ack(q, sl, p->lmi_primitive, err);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC int
slu_detach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	struct as *as;
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (sl_get_i_state(sl) != LMI_DISABLED)
		goto outstate;
	if (sl_get_state(sl) != AS_INACTIVE)
		goto efault;
	sl_set_i_state(sl, LMI_DETACH_PENDING);
	if ((err = sl_u_set_state(q, sl, AS_DOWN)))
		goto error;
	/* unlink SL-U from AS-U */
	if ((as = sl->as.as)) {
		if ((*sl->as.prev = sl->as.next))
			sl->as.next->as.prev = sl->as.prev;
		sl->as.next = NULL;
		sl->as.prev = &sl->as.next;
		sl_put(sl);
		as->sl.numb--;
		as_put(xchg(&sl->as.as, NULL));
	}
	return slu_ok_ack(q, sl, p->lmi_primitive);
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: SWERR: software error\n", DRV_NAME, sl));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: PROTO: would place i/f out of state\n", DRV_NAME, sl));
	goto error;
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, sl));
	goto error;
      error:
	return slu_error_ack(q, sl, p->lmi_primitive, err);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC int
slu_enable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct gp *asp;
	struct xp *xp;
	struct as *as;
	int err;
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (sl_get_i_state(sl)) {
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
	if (!(as = sl->as.as))
		goto efault;
	switch (sl_get_state(sl)) {
	case AS_DOWN:
		swerr();
		goto outstate;
	case AS_INACTIVE:
		if (as_tst_flags(sl->as.as, ASF_MGMT_BLOCKED))
			goto blocked;
		if ((err = sl_u_set_state(q, sl, AS_WACK_ASPAC)))
			goto error;
		sl_set_i_state(sl, LMI_ENABLE_PENDING);
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
				if ((xp = asp->spp.spp->xp)
				    && (err = m2ua_send_aspt_aspia_ack(q, xp, NULL, 0, NULL, 0)))
					goto error;
				if ((err = gp_u_set_state(q, asp, AS_INACTIVE)))
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
				if ((xp = asp->spp.spp->xp)
				    && (err = m2ua_send_aspt_aspia_ack(q, xp, NULL, 0, NULL, 0)))
					goto error;
				if ((err = gp_u_set_state(q, asp, AS_INACTIVE)))
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
				if ((xp = asp->spp.spp->xp)
				    && (err = m2ua_send_mgmt_ntfy(q, xp,
								  UA_STATUS_ALTERNATE_ASP_ACTIVE,
								  NULL, &as->iid, sizeof(as->iid),
								  NULL, 0)))
					goto error;
				if ((err = gp_u_set_state(q, asp, AS_INACTIVE)))
					goto error;
			}
			break;
		}
		/* fall through */
	case AS_WACK_ASPIA:
		todo(("Notify management that SL-U has gone active\n"));
		if ((err = sl_u_set_state(q, sl, AS_ACTIVE)))
			goto error;
	case AS_ACTIVE:
	      ignore:
		if ((err = slu_enable_con(q, sl)))
			goto error;
		sl_set_i_state(sl, LMI_ENABLED);
		return (QR_DONE);
	}
      efault:
	pswerr(("%s: %p: SWERR: software error\n", DRV_NAME, sl));
	return (-EFAULT);
      blocked:
	err = -EBUSY;
	ptrace(("%s: %p: PROTO: management blocked\n", DRV_NAME, sl));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: PROTO: out of state\n", DRV_NAME, sl));
	goto error;
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, sl));
	goto error;
      unusable:
	err = -EAGAIN;
	ptrace(("%s: %p: ERROR: waiting for stream to become usable\n", DRV_NAME, sl));
	goto error;
      error:
	return slu_error_ack(q, sl, p->lmi_primitive, err);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC int
slu_disable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct as *as;
	int err;
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (sl_get_i_state(sl)) {
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
	if (!(as = sl->as.as))
		goto efault;
	switch (sl_get_state(sl)) {
	case AS_DOWN:
		swerr();
		goto outstate;
	case AS_ACTIVE:
		if ((err = sl_u_set_state(q, sl, AS_WACK_ASPIA)))
			goto error;
		sl_set_i_state(sl, LMI_DISABLE_PENDING);
		/* fall through */
	case AS_WACK_ASPIA:
		if (as_tst_flags(as, ASF_ACTIVE))
			/* wait for deactivation to confirm */
			return (QR_DONE);
		/* fall through */
	case AS_WACK_ASPAC:
		todo(("Notify management that SL-U has gone inactive\n"));
		if ((err = sl_u_set_state(q, sl, AS_INACTIVE)))
			goto error;
	case AS_INACTIVE:
		if ((err = slu_disable_con(q, sl)))
			goto error;
		return (QR_DONE);
	}
      ignore:
	rare();
	return (QR_DONE);
      efault:
	pswerr(("%s: %p: SWERR: software error\n", DRV_NAME, sl));
	return (-EFAULT);
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: PROTO: out of state\n", DRV_NAME, sl));
	goto error;
      badprim:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", DRV_NAME, sl));
	goto error;
      unusable:
	err = -EAGAIN;
	ptrace(("%s: %p: ERROR: waiting for stream to become usable\n", DRV_NAME, sl));
	goto error;
      error:
	return slu_error_ack(q, sl, p->lmi_primitive, err);
}

#if 0
/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
slu_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;

	return slu_error_ack(q, sl, p->sl_primitive, LMI_NOTSUPP);
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
slu_send_msg(struct sl *slu, queue_t *q, mblk_t *mp, int type,
	     int (*pass_f) (queue_t *, struct sl *, mblk_t *),
	     int (*send_f) (queue_t *, struct gp *, mblk_t *))
{
	struct sl *slp;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			if ((err = pass_f(q, slp, bp)) < 0) {
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
slu_write(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	size_t dlen = mp ? msgdsize(mp) : 0;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (dlen < slu->info.lmi_min_sdu || dlen > slu->info.lmi_max_sdu)
		goto badprim;
	if (!(as = slu->as.as))
		goto efault;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (!canputnext(slp->oq))
				goto ebusy;
			putnext(slp->oq, mp);
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((slu->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
				if ((err = m2ua_send_maup_data1(q, sgp, mp)) < 0)
					goto error;
				return (QR_ABSORBED);
			} else {
				if ((err = m2ua_send_maup_data2(q, sgp, mp)) < 0)
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
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      badprim:
	err = LMI_BADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive\n", DRV_NAME, slu));
	goto error;
      error:
	return m_error(q, slu, EPROTO);
}

/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
STATIC int
slu_pdu_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp) : 0;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (dlen < slu->info.lmi_min_sdu || dlen > slu->info.lmi_max_sdu)
		goto badprim;
	if (!(as = slu->as.as))
		goto efault;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (canputnext(slp->oq)) {
				putnext(slp->oq, mp);
				return (QR_ABSORBED);
			}
			goto ebusy;
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((slu->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
				if ((err = m2ua_send_maup_data1(q, sgp, mp->b_cont)) < 0)
					goto error;
				return (QR_TRIMMED);
			} else {
				if ((err = m2ua_send_maup_data2(q, sgp, mp->b_cont)) < 0)
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
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      badprim:
	err = LMI_BADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
STATIC int
slu_emergency_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if (sl_tst_flags(slu, ASF_EMERGENCY))
		goto discard;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (sl_tst_flags(slp, ASF_EMERGENCY)) {
				sl_set_flags(slu, ASF_EMERGENCY);
				return (QR_DONE);
			}
			switch (sl_get_l_state(slp)) {
			case SLS_IDLE:
			case SLS_WCON_EREQ:
			case SLS_WCON_RELREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if (canputnext(slp->oq)) {
					sl_set_flags(slu, ASF_EMERGENCY);
					putnext(slp->oq, mp);
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
				sl_set_flags(slu, ASF_EMERGENCY);
				return (QR_DONE);
			}
			switch (gp_get_l_state(sgp)) {
			case SLS_IDLE:
			case SLS_WCON_EREQ:
			case SLS_WCON_RELREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
					goto error;
				sl_set_flags(slu, ASF_EMERGENCY);
				return (err);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = slu_local_processor_outage_ind(q, slu, NULL)) < 0)
		goto error;
	sl_set_flags(slu, ASF_PRV_PROC_OUTAGE);
	sl_set_flags(slu, ASF_EMERGENCY);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
STATIC int
slu_emergency_ceases_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if (!sl_tst_flags(slu, ASF_EMERGENCY))
		goto discard;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (!sl_tst_flags(slp, ASF_EMERGENCY)) {
				sl_clr_flags(slu, ASF_EMERGENCY);
				return (QR_DONE);
			}
			switch (sl_get_l_state(slp)) {
			case SLS_IDLE:
			case SLS_WCON_EREQ:
			case SLS_WCON_RELREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if (canputnext(slp->oq)) {
					sl_clr_flags(slu, ASF_EMERGENCY);
					putnext(slp->oq, mp);
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
				sl_clr_flags(slu, ASF_EMERGENCY);
				return (QR_DONE);
			}
			switch (gp_get_l_state(sgp)) {
			case SLS_IDLE:
			case SLS_WCON_EREQ:
			case SLS_WCON_RELREQ:
			case SLS_ESTABLISHED:
			case SLS_UNKNOWN:
				if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
					goto error;
				sl_clr_flags(slu, ASF_EMERGENCY);
				return (err);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = slu_local_processor_outage_ind(q, slu, NULL)) < 0)
		goto error;
	sl_set_flags(slu, ASF_PRV_PROC_OUTAGE);
	sl_clr_flags(slu, ASF_EMERGENCY);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 *  Now, actually if the link has already been started, we can simply respond to this with an SL_IN_SERVICE_IND,
 *  otherwise we start the link.
 */
STATIC int
slu_start_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if (sl_get_l_state(slu) != SLS_IDLE)
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			switch (sl_get_l_state(slp)) {
			case SLS_WCON_RELREQ:
				/* The signalling link has failed and we are restarting it */
				rare();
			case SLS_IDLE:
				if (!sl_tst_flags(slp, ASF_RETRIEVAL)) {
					if (canputnext(slp->oq)) {
						sl_set_l_state(slu, SLS_WCON_EREQ);
						sl_set_l_state(slp, SLS_WCON_EREQ);
						putnext(slp->oq, mp);
						return (QR_ABSORBED);
					}
					goto ebusy;
				} else {
					/* Someone else is retrieving messages, so we need to
					   indicate that the signalling link has failed (reason
					   will be unspecified). */
					if ((err = slu_out_of_service_ind(q, slu, NULL)) < 0)
						goto error;
					sl_set_l_state(slu, SLS_IDLE);
					return (QR_DONE);
				}
			case SLS_WCON_EREQ:
				/* The signalling link is already being established by some other
				   signalling link user. Wait for establishment confirmation. */
				sl_set_l_state(slu, SLS_WCON_EREQ);
				return (QR_DONE);
			case SLS_ESTABLISHED:
				/* The signalling link is already established by some other
				   signalling link user. Indicate that the link is in service */
				if ((err = slu_in_service_ind(q, slu, NULL)) < 0)
					goto error;
				sl_set_l_state(slu, SLS_ESTABLISHED);
				return (QR_DONE);
			default:
				swerr();
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			switch (gp_get_l_state(sgp)) {
			case SLS_WCON_RELREQ:
				/* The signalling link failed and we are restarting it */
				rare();
			case SLS_IDLE:
				if (!gp_tst_flags(sgp, ASF_RETRIEVAL)) {
					if ((err = m2ua_send_maup_estab_req(q, sgp, mp)) < 0)
						goto error;
					sl_set_l_state(slu, SLS_WCON_EREQ);
					gp_set_l_state(sgp, SLS_WCON_EREQ);
					return (err);
				} else {
					/* Someone else is retrieving messages, so we need to
					   indicate that the signalling link has failed (reason
					   will be unspecified). */
					if ((err = slu_out_of_service_ind(q, slu, NULL)) < 0)
						goto error;
					sl_set_l_state(slu, SLS_IDLE);
					return (err);
				}
			case SLS_WCON_EREQ:
				/* The signalling link is already being established by some other
				   signalling link user. Wait for establishment confirmation */
				sl_set_l_state(slu, SLS_WCON_EREQ);
				return (QR_DONE);
			case SLS_ESTABLISHED:
				/* The signalling link is already established by some other
				   signalling link user. Indicate that the link is in service */
				if ((err = slu_in_service_ind(q, slu, NULL)) < 0)
					goto error;
				sl_set_l_state(slu, SLS_ESTABLISHED);
				return (QR_DONE);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = slu_local_processor_outage_ind(q, slu, NULL)) < 0)
		goto error;
	sl_set_flags(slu, ASF_PRV_PROC_OUTAGE);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 *  If there are other users of this signalling link, we want to send local or remote processor outage (rather than
 *  link failure) to those links so that changeover procedure will not occur there.
 */
STATIC int
slu_stop_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if ((1 << sl_get_l_state(slu)) & (SLSF_IDLE | SLSF_WCON_RELREQ))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			switch (sl_get_l_state(slp)) {
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if (canputnext(slp->oq)) {
					sl_set_l_state(slu, SLS_IDLE);
					sl_set_l_state(slp, SLS_IDLE);
					putnext(slp->oq, mp);
					return (QR_ABSORBED);
				}
				goto ebusy;
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				/* The signalling link has already been put out of service by some
				   other signalling link user or we can't put it out of service.
				   Just silently accept the request. */
				sl_set_l_state(slu, SLS_IDLE);
				return (QR_DONE);
			default:
				swerr();
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			switch (gp_get_l_state(sgp)) {
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if ((err = m2ua_send_maup_rel_req(q, sgp, mp)) < 0)
					goto error;
				sl_set_l_state(slu, SLS_IDLE);
				gp_set_l_state(sgp, SLS_IDLE);
				return (err);
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				/* The signalling link has already been put out of service by some
				   other signalling link user or we can't put it out of service.
				   Just silently accept the request. */
				sl_set_l_state(slu, SLS_IDLE);
				return (QR_DONE);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	sl_set_l_state(slu, SLS_IDLE);
	return (QR_DONE);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
STATIC int
slu_retrieve_bsnt_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if ((1 << sl_get_l_state(slu)) & (SLSF_ESTABLISHED | SLSF_WCON_RELREQ))
		goto outstate;
	if (sl_tst_flags(slu, ASF_BSNT_REQUEST))
		goto outstate;	/* alreday requested */
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (sl_tst_flags(slp, ASF_BSNT_REQUEST)) {
				sl_set_flags(slu, ASF_BSNT_REQUEST);
				return (QR_DONE);
			}
			switch (sl_get_l_state(slp)) {
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				if (canputnext(slp->oq)) {
					sl_set_flags(slu, ASF_BSNT_REQUEST);
					sl_set_flags(slp, ASF_BSNT_REQUEST);
					putnext(slp->oq, mp);
					return (QR_ABSORBED);
				}
				goto ebusy;
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if ((err = slu_bsnt_not_retrievable_ind(q, slu, NULL)) < 0)
					goto error;
				return (err);
			default:
				swerr();
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if (gp_tst_flags(sgp, ASF_BSNT_REQUEST)) {
				sl_set_flags(slu, ASF_BSNT_REQUEST);
				return (QR_DONE);
			}
			switch (gp_get_l_state(sgp)) {
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				if ((err = m2ua_send_maup_retr_req(q, sgp, mp)) < 0)
					goto error;
				sl_set_flags(slu, ASF_BSNT_REQUEST);
				gp_set_flags(sgp, ASF_BSNT_REQUEST);
				return (err);
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				if ((err = slu_bsnt_not_retrievable_ind(q, slu, NULL)) < 0)
					goto error;
				return (err);
			default:
				swerr();
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = slu_bsnt_not_retrievable_ind(q, slu, NULL)) < 0)
		goto error;
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      outstate:
	fixme(("This should actually return BSNT not retrievable\n"));
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
STATIC int
slu_retrieval_request_and_fsnc_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if (sl_get_l_state(slu) != SLS_IDLE)
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			switch (sl_get_l_state(slp)) {
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				if (!sl_tst_flags(slp, ASF_RETRIEVAL)) {
					if (canputnext(slp->oq)) {
						sl_set_flags(slu, ASF_RETRIEVAL);
						sl_set_flags(slp, ASF_RETRIEVAL);
						putnext(slp->oq, mp);
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
				if ((err = slu_retrieval_not_possible_ind(q, slu, NULL)) < 0)
					goto error;
				sl_set_l_state(slu, SLS_IDLE);
				return (err);
			default:
				goto efault;
			}
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			switch (gp_get_l_state(sgp)) {
			case SLS_IDLE:
			case SLS_WCON_RELREQ:
				if (!gp_tst_flags(sgp, ASF_RETRIEVAL)) {
					if ((err = m2ua_send_maup_retr_req(q, sgp, mp)) < 0)
						goto error;
					gp_set_flags(sgp, ASF_RETRIEVAL);
					sl_set_flags(slu, ASF_RETRIEVAL);
					return (err);
				}
				/* fall through */
			case SLS_WCON_EREQ:
			case SLS_ESTABLISHED:
				/* Retrieval is not possible or someone else is already retrieving. 
				   Only one signalling link user can be allowed to retrieve.  We
				   must refuse the retrieval request. */
				if ((err = slu_retrieval_not_possible_ind(q, slu, NULL)) < 0)
					goto error;
				sl_set_l_state(slu, SLS_IDLE);
				return (err);
			default:
				goto efault;
			}
		}
	}
	/* No active or pending provider */
	if ((err = slu_retrieval_not_possible_ind(q, slu, NULL)) < 0)
		goto error;
	sl_set_l_state(slu, SLS_IDLE);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      outstate:
	fixme(("This should actually return BSNT not retrievable\n"));
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
STATIC int
slu_clear_buffers_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if (sl_tst_flags(slu, ASF_FLUSH_BUFFERS))
		goto discard;	/* already done */
	if (!sl_tst_flags(slu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE)))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			/* active or pending local slp */
			if (!sl_tst_flags(slp, ASF_FLUSH_BUFFERS)) {
				if (sl_tst_flags(slp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if (!canputnext(slp->oq))
						goto ebusy;
					putnext(slp->oq, mp);
					sl_clr_flags(slp, ASF_USR_PROC_OUTAGE);
				}
				sl_clr_flags(slu, ASF_USR_PROC_OUTAGE);
				sl_set_flags(slu, ASF_FLUSH_BUFFERS);
			}
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			/* active or pending sgp */
			if (!gp_tst_flags(sgp, ASF_FLUSH_BUFFERS)) {
				if (gp_tst_flags(sgp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
						goto error;
					gp_clr_flags(sgp, ASF_USR_PROC_OUTAGE);
					sl_clr_flags(slu, ASF_USR_PROC_OUTAGE);
				}
				sl_set_flags(slu, ASF_FLUSH_BUFFERS);
			}
			return (QR_DONE);
		}
	}
	/* No active or pending provider */
	if ((err = slu_local_processor_outage_ind(q, slu, NULL)) < 0)
		goto error;
	sl_set_flags(slu, ASF_PRV_PROC_OUTAGE);
	sl_clr_flags(slu, ASF_USR_PROC_OUTAGE);
	sl_set_flags(slu, ASF_FLUSH_BUFFERS);
	return (QR_DONE);
      outstate:
	fixme(("This should actually return BSNT not retrievable\n"));
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, slu));
	goto error;
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
STATIC int
slu_clear_rtb_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err = QR_DONE;
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if (sl_tst_flags(slu, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS)))
		goto discard;	/* already done */
	if (!sl_tst_flags(slu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE)))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			/* active or pending local slp */
			if (!sl_tst_flags(slp, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS))) {
				if (sl_tst_flags(slp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if (!canputnext(slp->oq))
						goto ebusy;
					putnext(slp->oq, mp);
					err = QR_ABSORBED;
					sl_clr_flags(slp, ASF_USR_PROC_OUTAGE);
				}
			}
			sl_clr_flags(slu, ASF_USR_PROC_OUTAGE);
			sl_set_flags(slu, ASF_CLEAR_RTB);
			return (err);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			/* active or pending sgp */
			if (!gp_tst_flags(sgp, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS))) {
				if (gp_tst_flags(sgp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
						goto error;
					gp_clr_flags(sgp, ASF_USR_PROC_OUTAGE);
				}
			}
			sl_clr_flags(slu, ASF_USR_PROC_OUTAGE);
			sl_set_flags(slu, ASF_CLEAR_RTB);
			return (err);
		}
	}
	/* No active or pending provider */
	if ((err = slu_local_processor_outage_ind(q, slu, NULL)) < 0)
		goto error;
	sl_set_flags(slu, ASF_PRV_PROC_OUTAGE);
	sl_clr_flags(slu, ASF_USR_PROC_OUTAGE);
	sl_set_flags(slu, ASF_CLEAR_RTB);
	return (err);
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, slu));
	goto error;
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_CONTINUE_REQ
 *  -----------------------------------
 */
STATIC int
slu_continue_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err = QR_DONE;
	sl_continue_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if (sl_tst_flags(slu, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS)))
		goto discard;	/* already done */
	if (!sl_tst_flags(slu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE)))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			/* active or pending local slp */
			if (!sl_tst_flags(slp, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS))) {
				if (sl_tst_flags(slp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if (!canputnext(slp->oq))
						goto ebusy;
					putnext(slp->oq, mp);
					err = QR_ABSORBED;
					sl_clr_flags(slp, ASF_USR_PROC_OUTAGE);
				}
			}
			sl_clr_flags(slu, ASF_USR_PROC_OUTAGE);
			sl_set_flags(slu, ASF_CLEAR_RTB);
			return (err);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			/* active or pending sgp */
			if (!gp_tst_flags(sgp, (ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS))) {
				if (gp_tst_flags(sgp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
						goto error;
					gp_clr_flags(sgp, ASF_USR_PROC_OUTAGE);
				}
			}
			sl_clr_flags(slu, ASF_USR_PROC_OUTAGE);
			sl_set_flags(slu, ASF_CLEAR_RTB);
			return (err);
		}
	}
	/* No active or pending provider */
	if ((err = slu_local_processor_outage_ind(q, slu, NULL)) < 0)
		goto error;
	sl_set_flags(slu, ASF_PRV_PROC_OUTAGE);
	sl_clr_flags(slu, ASF_USR_PROC_OUTAGE);
	sl_set_flags(slu, ASF_CLEAR_RTB);
	return (err);
      outstate:
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, slu));
	goto error;
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
STATIC int
slu_local_processor_outage_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err = QR_DONE;
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if (sl_tst_flags(slu, ASF_USR_PROC_OUTAGE))
		goto discard;	/* already done */
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			/* Active or pending slp */
			if (!sl_tst_flags(slp, ASF_USR_PROC_OUTAGE)) {
				if (!canputnext(slp->oq))
					goto ebusy;
				putnext(slp->oq, mp);
				err = QR_ABSORBED;
				sl_set_flags(slp, ASF_USR_PROC_OUTAGE);
			}
			sl_set_flags(slu, ASF_USR_PROC_OUTAGE);
			return (err);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			/* Active or pending sgp */
			if (!gp_tst_flags(sgp, ASF_USR_PROC_OUTAGE)) {
				if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
					goto error;
				gp_set_flags(sgp, ASF_USR_PROC_OUTAGE);
			}
			sl_set_flags(slu, ASF_USR_PROC_OUTAGE);
			return (err);
		}
	}
	/* No active or pending provider */
	/* just wait */
	sl_set_flags(slu, ASF_USR_PROC_OUTAGE);
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
STATIC int
slu_resume_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err = QR_DONE;
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!(as = slu->as.as))
		goto efault;
	if (!sl_tst_flags(slu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))
	    || sl_tst_flags(slu, (ASF_PRV_PROC_OUTAGE | ASF_LOC_PROC_OUTAGE | ASF_REM_PROC_OUTAGE)))
		goto outstate;
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			/* active or pending local slp */
			if (!sl_tst_flags
			    (slp,
			     (ASF_PRV_PROC_OUTAGE | ASF_LOC_PROC_OUTAGE | ASF_REM_PROC_OUTAGE))) {
				if (sl_tst_flags(slp, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE))) {
					if (!canputnext(slp->oq))
						goto ebusy;
					putnext(slp->oq, mp);
					err = QR_ABSORBED;
					sl_clr_flags(slp,
						     (ASF_RECOVERY | ASF_USR_PROC_OUTAGE |
						      ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS));
				}
			}
			sl_clr_flags(slu,
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
					if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
						goto error;
					gp_clr_flags(sgp,
						     (ASF_RECOVERY | ASF_USR_PROC_OUTAGE |
						      ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS));
				}
			}
			sl_clr_flags(slu,
				     (ASF_RECOVERY | ASF_USR_PROC_OUTAGE | ASF_CLEAR_RTB |
				      ASF_FLUSH_BUFFERS));
			return (err);
		}
	}
	/* no active or pending provider */
	if ((err = slu_local_processor_outage_ind(q, slu, NULL)) < 0)
		goto error;
	sl_set_flags(slu, ASF_PRV_PROC_OUTAGE);
	sl_clr_flags(slu, (ASF_RECOVERY | ASF_USR_PROC_OUTAGE | ASF_CLEAR_RTB | ASF_FLUSH_BUFFERS));
	return (err);
      discard:
	rare();
	return (QR_DONE);
      ebusy:
	err = -EBUSY;
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, slu));
	goto error;
      efault:
	err = -EFAULT;
	pswerr(("%s: %p: ERROR: software error\n", DRV_NAME, slu));
	goto error;
      outstate:
	fixme(("This should actually return BSNT not retrievable\n"));
	err = LMI_OUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, slu));
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, slu));
	goto error;
      error:
	return slu_error_ack(q, slu, p->sl_primitive, err);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
STATIC int
slu_congestion_discard_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (!canputnext(slp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			putnext(slp->oq, mp);
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
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
slu_congestion_accept_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (!canputnext(slp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			putnext(slp->oq, mp);
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
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
slu_no_congestion_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (!canputnext(slp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			putnext(slp->oq, mp);
			return (QR_ABSORBED);
		}
		for (sgp = ap_find_sgp(ap); sgp; sgp = ap_find_sgp_next(ap, sgp, 1)) {
			if ((err = m2ua_send_maup_state_req(q, sgp, mp)) < 0)
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
slu_power_on_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	struct sl *slp;
	struct as *as;
	struct ap *ap;
	struct gp *sgp;
	mblk_t *bp;

	if (sl_get_i_state(slu) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slu->as.as))
		return (-EFAULT);
	for (ap = as_p_find_ap(as); ap; ap = as_p_find_ap_next(as, ap, 1)) {
		for (slp = ap_find_slp(ap); slp; slp = ap_find_slp_next(ap, slp, 1)) {
			if (!canputnext(slp->oq))
				return (-EBUSY);
			if (!(bp = ss7_dupmsg(q, mp)))
				return (-ENOBUFS);
			putnext(slp->oq, mp);
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
slu_optmgmt_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	int err;

	if ((err = sl_error_ack(q, slu, SL_OPTMGMT_REQ, LMI_NOTSUPP, EOPNOTSUPP)))
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
slu_notify_req(struct sl *slu, queue_t *q, mblk_t *mp)
{
	int err;

	if ((err = sl_error_ack(q, slu, SL_NOTIFY_REQ, LMI_NOTSUPP, EOPNOTSUPP)))
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
slp_info_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;
	m2ua_addr_t *add;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sizeof(*add))
		goto efault;
	bcopy(p, &sl->info, sizeof(*p) + sizeof(*add));
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
slp_ok_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_ok_ack_t *p = (typeof(p)) mp->b_wptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (sl_get_i_state(sl)) {
	case LMI_ATTACH_PENDING:
		assure(p->lmi_correct_primitive == LMI_ATTACH_REQ);
		assure(sl_get_state(sl) == AS_DOWN);
		sl_set_i_state(sl, LMI_DISABLED);
		todo(("Notify management that SL-P is up\n"));
		if ((err = sl_p_set_state(q, sl, AS_INACTIVE)))
			return (err);
		return (QR_DONE);
	case LMI_DETACH_PENDING:
		assure(p->lmi_correct_primitive == LMI_DETACH_REQ);
		assure(sl_get_state(sl) == AS_INACTIVE);
		sl_set_i_state(sl, LMI_UNATTACHED);
		todo(("Notify management that SL-P is down\n"));
		if ((err = sl_p_set_state(q, sl, AS_DOWN)))
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
slp_error_ack(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_error_ack_t *p = (typeof(p)) mp->b_wptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (sl_get_i_state(sl)) {
	case LMI_ATTACH_PENDING:
		assure(p->lmi_error_primitive == LMI_ATTACH_REQ);
		assure(sl_get_state(sl) == AS_DOWN);
		sl_set_i_state(sl, LMI_UNATTACHED);
		todo(("Notify management that the SL-P is down\n"));
		return (QR_DONE);
	case LMI_DETACH_PENDING:
		assure(p->lmi_error_primitive == LMI_DETACH_REQ);
		assure(sl_get_state(sl) == AS_INACTIVE);
		sl_set_i_state(sl, LMI_DISABLED);
		todo(("Notify management that the SL-P is up\n"));
		return (QR_DONE);
	case LMI_ENABLE_PENDING:
		assure(p->lmi_error_primitive == LMI_ENABLE_REQ);
		assure(sl_get_state(sl) == AS_WACK_ASPAC);
		sl_set_i_state(sl, LMI_DISABLED);
		todo(("Notify management that the SL-P is inactive\n"));
		if ((err = sl_p_set_state(q, sl, AS_INACTIVE)))
			return (err);
		return (QR_DONE);
	case LMI_DISABLE_PENDING:
		assure(p->lmi_error_primitive == LMI_DISABLE_REQ);
		assure(sl_get_state(sl) == AS_WACK_ASPIA);
		sl_set_i_state(sl, LMI_ENABLED);
		todo(("Notify management that the SL-P is active\n"));
		if ((err = sl_p_set_state(q, sl, AS_ACTIVE)))
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
slp_enable_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (sl_get_i_state(sl)) {
	case LMI_ENABLE_PENDING:
		assure(sl_get_state(sl) == AS_WACK_ASPAC);
		sl_set_i_state(sl, LMI_ENABLED);
		todo(("Notify management that the SL-P is active\n"));
		if ((err = sl_p_set_state(q, sl, AS_ACTIVE)))
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
slp_disable_con(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (sl_get_i_state(sl)) {
	case LMI_DISABLE_PENDING:
		assure(sl_get_state(sl) == AS_WACK_ASPIA);
		sl_set_i_state(sl, LMI_DISABLED);
		todo(("Notify management that the SL-P is inactive\n"));
		if ((err = sl_p_set_state(q, sl, AS_INACTIVE)))
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
slp_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *mp)
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
slp_error_ind(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	switch (sl_get_i_state(sl)) {
	case LMI_ENABLED:
	case LMI_ENABLE_PENDING:
		assure(p->lmi_state == LMI_DISABLED);
		assure(sl_get_state(sl) == AS_ACTIVE);
		sl_set_i_state(sl, LMI_DISABLED);
		todo(("Notify management that SL-P is inactive\n"));
		if ((err = sl_p_set_state(q, sl, AS_INACTIVE)))
			return (err);
		return (QR_DONE);
	case LMI_DISABLED:
		assure(p->lmi_state == LMI_DISABLED);
		return (QR_DONE);
	case LMI_DISABLE_PENDING:
		assure(p->lmi_state == LMI_DISABLED);
		assure(sl_get_state(sl) == AS_WACK_ASPIA);
		sl_set_i_state(sl, LMI_DISABLED);
		todo(("Notify management that SL-P is inactive\n"));
		if ((err = sl_p_set_state(q, sl, AS_INACTIVE)))
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
slp_stats_ind(struct sl *sl, queue_t *q, mblk_t *mp)
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
slp_event_ind(struct sl *sl, queue_t *q, mblk_t *mp)
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
slp_read(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	mblk_t *bp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (!sl_tst_flags(slu, ASF_OPERATION_PENDING)) {
				if ((1 << sl_get_l_state(slu)) &
				    (SLSF_WCON_RELREQ | SLSF_ESTABLISHED)) {
					if (!canputnext(slu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(slu->oq, bp);
					if (as->sp.sp->tmode != UA_TMODE_BROADCAST)
						return (QR_DONE);
					sl_set_flags(slu, ASF_OPERATION_PENDING);
				}
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if ((1 << gp_get_l_state(asp)) &
				    (SLSF_WCON_RELREQ | SLSF_ESTABLISHED)) {
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					if ((slp->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
						if ((err = m2ua_send_maup_data1(q, asp, bp)) < 0)
							return (err);
					} else {
						if ((err = m2ua_send_maup_data2(q, asp, bp)) < 0)
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
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			sl_clr_flags(slu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			xp_clr_flags(asp->spp.spp->xp, ASF_OPERATION_PENDING);
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
slp_pdu_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;
	mblk_t *bp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (!sl_tst_flags(slu, ASF_OPERATION_PENDING)) {
				if ((1 << sl_get_l_state(slu)) &
				    (SLSF_WCON_RELREQ | SLSF_ESTABLISHED)) {
					if (!canputnext(slu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(slu->oq, bp);
					if (as->sp.sp->tmode != UA_TMODE_BROADCAST)
						return (QR_DONE);
					sl_set_flags(slu, ASF_OPERATION_PENDING);
				}
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if ((1 << gp_get_l_state(asp)) &
				    (SLSF_WCON_RELREQ | SLSF_ESTABLISHED)) {
					if (!(bp = ss7_dupmsg(q, mp->b_cont)))
						return (-ENOBUFS);
					if ((slp->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
						if ((err = m2ua_send_maup_data1(q, asp, bp)) < 0)
							return (err);
					} else {
						if ((err = m2ua_send_maup_data2(q, asp, bp)) < 0)
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
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			sl_clr_flags(slu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			xp_clr_flags(asp->spp.spp->xp, ASF_OPERATION_PENDING);
	}
	return (QR_DONE);
}

/*
 *  SL_LINK_CONGESTED_IND
 *  -----------------------------------
 *  Pass this one to all signalling link users in the established state.
 */
STATIC int
slp_link_congested_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure(sl_get_l_state(slp) == SLS_ESTABLISHED);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (!sl_tst_flags(slu, ASF_OPERATION_PENDING)) {
				if (sl_get_l_state(slu) == SLS_ESTABLISHED) {
					mblk_t *bp;

					if (!canputnext(slu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(slu->oq, bp);
					if (as->sp.sp->tmode == UA_TMODE_BROADCAST)
						return (QR_DONE);
					sl_set_flags(slu, ASF_OPERATION_PENDING);
				}
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if (gp_get_l_state(asp) == SLS_ESTABLISHED) {
					if ((err = m2ua_send_maup_cong_ind(q, asp, mp)))
						return (err);
					if (as->sp.sp->tmode == UA_TMODE_BROADCAST)
						return (QR_DONE);
					gp_set_flags(asp, ASF_OPERATION_PENDING);
				}
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			sl_clr_flags(slu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			xp_clr_flags(asp->spp.spp->xp, ASF_OPERATION_PENDING);
	}
	return (QR_DONE);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  -----------------------------------
 *  Pass this one to all signalling link users in the established state
 */
STATIC int
slp_congestion_ceased_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure(sl_get_l_state(slp) == SLS_ESTABLISHED);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (!sl_tst_flags(slu, ASF_OPERATION_PENDING)) {
				if (sl_get_l_state(slu) == SLS_ESTABLISHED) {
					mblk_t *bp;

					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					if (!canputnext(slu->oq))
						return (-EBUSY);
					putnext(slu->oq, bp);
					if (as->sp.sp->tmode == UA_TMODE_BROADCAST)
						return (QR_DONE);
				}
				sl_set_flags(slu, ASF_OPERATION_PENDING);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if (gp_get_l_state(asp) == SLS_ESTABLISHED) {
					if ((err = m2ua_send_maup_cong_ind(q, asp, mp)))
						return (err);
					if (as->sp.sp->tmode == UA_TMODE_BROADCAST)
						return (QR_DONE);
				}
				gp_set_flags(asp, ASF_OPERATION_PENDING);
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			sl_clr_flags(slu, ASF_OPERATION_PENDING);
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			xp_clr_flags(asp->spp.spp->xp, ASF_OPERATION_PENDING);
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
slp_retrieved_message_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure(sl_get_l_state(slp) == SLS_IDLE);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_tst_flags(slu, ASF_RETRIEVAL)) {
				if (!canputnext(slu->oq))
					return (-EBUSY);
				putnext(slu->oq, mp);
				return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_RETRIEVAL)) {
				if ((slp->proto.pvar & SS7_PVAR_MASK) != SS7_PVAR_JTTC) {
					if ((err = m2ua_send_maup_retr_ind1(q, asp, mp)))
						return (err);
				} else {
					if ((err = m2ua_send_maup_retr_ind2(q, asp, mp)))
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
slp_retrieval_complete_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure(sl_get_l_state(slp) == SLS_IDLE);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_tst_flags(slu, ASF_RETRIEVAL)) {
				if (!canputnext(slu->oq))
					return (-EBUSY);
				putnext(slu->oq, mp);
				sl_set_l_state(slu, SLS_IDLE);
				sl_set_l_state(slp, SLS_IDLE);
				return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_RETRIEVAL)) {
				if ((err = m2ua_send_maup_retr_comp_ind(q, asp, mp)))
					return (err);
				gp_set_l_state(asp, SLS_IDLE);
				sl_set_l_state(slp, SLS_IDLE);
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
slp_rb_cleared_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure(sl_tst_flags(slp, ASF_FLUSH_BUFFERS));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_tst_flags(slu, ASF_FLUSH_BUFFERS)) {
				mblk_t *bp;

				if (!canputnext(slu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(slu->oq, bp);
				sl_clr_flags(slu, ASF_FLUSH_BUFFERS);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_FLUSH_BUFFERS)) {
				if ((err = m2ua_send_maup_state_con(q, asp, mp)))
					return (err);
				gp_clr_flags(asp, ASF_FLUSH_BUFFERS);
			}
	}
	sl_clr_flags(slp, ASF_FLUSH_BUFFERS);
	return (QR_DONE);
}

/*
 *  SL_BSNT_IND
 *  -----------------------------------
 *  We only respond to those signalling link users that have requested bsnt.
 */
STATIC int
slp_bsnt_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure(sl_tst_flags(slp, ASF_BSNT_REQUEST));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_tst_flags(slu, ASF_BSNT_REQUEST)) {
				mblk_t *bp;

				if (!canputnext(slu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(slu->oq, bp);
				sl_clr_flags(slu, ASF_BSNT_REQUEST);
				if (as->sp.sp->tmode == UA_TMODE_OVERRIDE)
					return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_BSNT_REQUEST)) {
				if ((err = m2ua_send_maup_retr_con(q, asp, mp)))
					return (err);
				gp_clr_flags(asp, ASF_BSNT_REQUEST);
				if (as->sp.sp->tmode == UA_TMODE_OVERRIDE)
					return (QR_DONE);
			}
	}
	sl_clr_flags(slp, ASF_BSNT_REQUEST);
	return (QR_DONE);
}

/*
 *  SL_IN_SERVICE_IND
 *  -----------------------------------
 *  We only respond to those signalling link users that have requested a start.
 */
STATIC int
slp_in_service_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		goto discard;
	if (!(as = slp->as.as))
		goto efault;
	assure(sl_get_l_state(slp) == SLS_WCON_EREQ);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_get_l_state(slu) == SLS_WCON_EREQ) {
				mblk_t *bp;

				if (!canputnext(slu->oq))
					goto ebusy;
				if (!(bp = ss7_dupmsg(q, mp)))
					goto enobufs;
				putnext(slu->oq, bp);
				sl_set_l_state(slu, SLS_ESTABLISHED);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_get_l_state(asp) == SLS_WCON_EREQ) {
				if ((err = m2ua_send_maup_estab_con(q, asp, mp)))
					return (err);
				gp_set_l_state(asp, SLS_ESTABLISHED);
			}
	}
	sl_set_l_state(slp, SLS_ESTABLISHED);
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
slp_out_of_service_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure((1 << sl_get_l_state(slp)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if ((1 << sl_get_l_state(slu)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
				mblk_t *bp;

				if (!canputnext(slu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(slu->oq, bp);
				sl_set_l_state(slu, SLS_IDLE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if ((1 << gp_get_l_state(asp)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
				if ((err = m2ua_send_maup_rel_ind(q, asp, mp)))
					return (err);
				gp_set_l_state(asp, SLS_IDLE);
			}
	}
	sl_set_l_state(slp, SLS_IDLE);
	return (QR_DONE);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
STATIC int
slp_remote_processor_outage_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure((1 << sl_get_l_state(slp)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (!sl_tst_flags(slu, ASF_OPERATION_PENDING)) {
				if ((1 << sl_get_l_state(slu)) &
				    (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
					mblk_t *bp;

					if (!canputnext(slu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(slu->oq, bp);
				}
				sl_set_flags(slu, ASF_OPERATION_PENDING);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if ((1 << gp_get_l_state(asp)) &
				    (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
					if ((err = m2ua_send_maup_state_ind(q, asp, mp)))
						return (err);
				}
				gp_set_flags(asp, ASF_OPERATION_PENDING);
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			sl_clr_flags(slu, ASF_OPERATION_PENDING);
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
slp_remote_processor_recovered_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure((1 << sl_get_l_state(slp)) & (SLSF_WCON_EREQ | SLSF_ESTABLISHED));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (!sl_tst_flags(slu, ASF_OPERATION_PENDING)) {
				if ((1 << sl_get_l_state(slu)) &
				    (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
					mblk_t *bp;

					if (!canputnext(slu->oq))
						return (-EBUSY);
					if (!(bp = ss7_dupmsg(q, mp)))
						return (-ENOBUFS);
					putnext(slu->oq, bp);
				}
				sl_set_flags(slu, ASF_OPERATION_PENDING);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (!gp_tst_flags(asp, ASF_OPERATION_PENDING)) {
				if ((1 << gp_get_l_state(asp)) &
				    (SLSF_WCON_EREQ | SLSF_ESTABLISHED)) {
					if ((err = m2ua_send_maup_state_ind(q, asp, mp)))
						return (err);
				}
				gp_set_flags(asp, ASF_OPERATION_PENDING);
			}
	}
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			sl_clr_flags(slu, ASF_OPERATION_PENDING);
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
slp_rtb_cleared_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure(sl_tst_flags(slp, ASF_CLEAR_RTB));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_tst_flags(slu, ASF_CLEAR_RTB)) {
				mblk_t *bp;

				if (!canputnext(slu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(slu->oq, bp);
				sl_clr_flags(slu, ASF_CLEAR_RTB);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_CLEAR_RTB)) {
				if ((err = m2ua_send_maup_state_con(q, asp, mp)))
					return (err);
				gp_clr_flags(asp, ASF_CLEAR_RTB);
			}
	}
	sl_clr_flags(slp, ASF_CLEAR_RTB);
	return (QR_DONE);
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  -----------------------------------
 *  Regardless of traffic mode we only send retrieval complete messages to one signalling link user.  The first user
 *  to set flag ASF_RETRIEVAL succeeds and the others fail.
 */
STATIC int
slp_retrieval_not_possible_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure(sl_get_l_state(slp) == SLS_IDLE);
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_tst_flags(slu, ASF_RETRIEVAL)) {
				if (!canputnext(slu->oq))
					return (-EBUSY);
				putnext(slu->oq, mp);
				sl_set_l_state(slu, SLS_IDLE);
				sl_set_l_state(slp, SLS_IDLE);
				return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_RETRIEVAL)) {
				if ((err = m2ua_send_maup_retr_con(q, asp, mp)))
					return (err);
				gp_set_l_state(asp, SLS_IDLE);
				sl_set_l_state(slp, SLS_IDLE);
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
slp_bsnt_not_retrievable_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	struct sl *slu;
	int err;
	struct as *as;
	struct ap *ap;
	struct gp *asp;

	if (sl_get_i_state(slp) != LMI_ENABLED)
		return (QR_DONE);
	if (!(as = slp->as.as))
		return (-EFAULT);
	assure(sl_tst_flags(slp, ASF_BSNT_REQUEST));
	for (ap = as_u_find_ap(as); ap; ap = as_u_find_ap_next(as, ap, 1)) {
		for (slu = ap_find_slu(ap); slu; slu = ap_find_slu_next(ap, slu, 1))
			if (sl_tst_flags(slu, ASF_BSNT_REQUEST)) {
				mblk_t *bp;

				if (!canputnext(slu->oq))
					return (-EBUSY);
				if (!(bp = ss7_dupmsg(q, mp)))
					return (-ENOBUFS);
				putnext(slu->oq, bp);
				sl_clr_flags(slu, ASF_BSNT_REQUEST);
				if (as->sp.sp->tmode == UA_TMODE_OVERRIDE)
					return (QR_DONE);
			}
		for (asp = ap_find_asp(ap); asp; asp = ap_find_asp_next(ap, asp, 1))
			if (gp_tst_flags(asp, ASF_BSNT_REQUEST)) {
				if ((err = m2ua_send_maup_retr_con(q, asp, mp)))
					return (err);
				gp_clr_flags(asp, ASF_BSNT_REQUEST);
				if (as->sp.sp->tmode == UA_TMODE_OVERRIDE)
					return (QR_DONE);
			}
	}
	sl_clr_flags(slp, ASF_BSNT_REQUEST);
	return (QR_DONE);
}

#if 0
/*
 *  SL_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC int
slp_optmgmt_ack(struct sl *slp, queue_t *q, mblk_t *mp)
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
slp_notify_ind(struct sl *slp, queue_t *q, mblk_t *mp)
{
	/* unexpected */
	swerr();
	return (-EFAULT);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  XP to M2UA primitives.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  T_CONN_IND
 *  -----------------------------------
 */
STATIC int
xp_conn_ind(struct xp *xp, queue_t *q, mblk_t *mp)
{
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

	if (!(spp = xp->spp)) {
		if (!(xp->sp))
			goto disable;
		return (-EPROTO);
	}
	switch (spp->type) {
	case M2UA_OBJ_TYPE_ASP:
		switch (spp_get_state(spp)) {
		case ASP_UP:
			asp_set_state(q, spp, ASP_WACK_ASPDN);
			/* fall through */
		case ASP_WACK_ASPDN:
			for (asp = spp->gp.list; asp; asp = asp->spp.next)
				if (gp_get_state(asp) != AS_DOWN)
					if ((err = gp_u_set_state(q, asp, AS_DOWN)))
						return (err);
			asp_set_state(q, spp, ASP_WACK_ASPUP);
			/* fall through */
		case ASP_WACK_ASPUP:
			todo(("Notify management that the SPP has failed\n"));
			asp_set_state(q, spp, ASP_DOWN);
			/* fall through */
		case ASP_DOWN:
			for (sp2 = spp->sp.sp->spp.list; sp2 && sp2 != spp; sp2 = sp2->sp.next)
				if (spp_get_state(sp2) != ASP_DOWN
				    && spp_get_state(sp2) != ASP_WACK_ASPUP)
					if ((err = m2ua_send_mgmt_ntfy(q, xp,
								       UA_STATUS_ASP_FAILURE,
								       &spp->aspid, NULL, 0, NULL,
								       0)))
						return (err);
			return (QR_DONE);
		}
		break;
	case M2UA_OBJ_TYPE_SGP:
		switch (spp_get_state(spp)) {
		case ASP_UP:
			sgp_set_state(q, spp, ASP_WACK_ASPDN);
			/* fall through */
		case ASP_WACK_ASPDN:
			for (sgp = spp->gp.list; sgp; sgp = sgp->spp.next)
				if (gp_get_state(sgp) != AS_DOWN)
					if ((err = gp_p_set_state(q, sgp, AS_DOWN)))
						return (err);
			sgp_set_state(q, spp, ASP_WACK_ASPUP);
			/* fall through */
		case ASP_WACK_ASPUP:
			todo(("Notify management that the SPP has failed\n"));
			sgp_set_state(q, spp, ASP_DOWN);
			/* fall through */
		case ASP_DOWN:
			for (sp2 = spp->sp.sp->spp.list; sp2 && sp2 != spp; sp2 = sp2->sp.next)
				if (spp_get_state(sp2) != ASP_DOWN
				    && spp_get_state(sp2) != ASP_WACK_ASPUP)
					if ((err = m2ua_send_mgmt_ntfy(q, xp,
								       UA_STATUS_ASP_FAILURE,
								       &spp->aspid, NULL, 0, NULL,
								       0)))
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

	if ((err = ss7_pullupmsg(q, mp, -1)))
		return (err);
	return m2ua_recv_msg(xp, q, mp);
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

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (p->MORE_flag & T_MORE) {
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
		if ((err = ss7_pullupmsg(q, mp->b_cont, -1)))
			return (err);
		return m2ua_recv_msg(xp, q, mp->b_cont);
	}
      efault:
	swerr();
	return (-EFAULT);
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

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (p->MORE_flag & T_MORE) {
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
		if ((err = ss7_pullupmsg(q, mp->b_cont, -1)))
			return (err);
		return m2ua_recv_msg(xp, q, mp->b_cont);
	}
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  T_INFO_ACK
 *  -----------------------------------
 */
STATIC int
xp_info_ack(struct xp *xp, queue_t *q, mblk_t *mp)
{
	struct T_info_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	xp->info = *p;
	return (QR_DONE);
      efault:
	swerr();
	return (-EFAULT);
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

	if (!(spp = xp->spp)) {
		if (!(xp->sp))
			goto disable;
		return (-EPROTO);
	}
	switch (spp->type) {
	case M2UA_OBJ_TYPE_ASP:
		switch (spp_get_state(spp)) {
		case ASP_UP:
			asp_set_state(q, spp, ASP_WACK_ASPDN);
			/* fall through */
		case ASP_WACK_ASPDN:
			for (asp = spp->gp.list; asp; asp = asp->spp.next)
				if (gp_get_state(asp) != AS_DOWN)
					if ((err = gp_u_set_state(q, asp, AS_DOWN)))
						return (err);
			asp_set_state(q, spp, ASP_WACK_ASPUP);
			/* fall through */
		case ASP_WACK_ASPUP:
			todo(("Notify management that the SPP has failed\n"));
			asp_set_state(q, spp, ASP_DOWN);
			/* fall through */
		case ASP_DOWN:
			for (sp2 = spp->sp.sp->spp.list; sp2 && sp2 != spp; sp2 = sp2->sp.next)
				if (spp_get_state(sp2) != ASP_DOWN
				    && spp_get_state(sp2) != ASP_WACK_ASPUP)
					if ((err = m2ua_send_mgmt_ntfy(q, xp,
								       UA_STATUS_ASP_FAILURE,
								       &spp->aspid, NULL, 0, NULL,
								       0)))
						return (err);
			return (QR_DONE);
		}
		break;
	case M2UA_OBJ_TYPE_SGP:
		switch (spp_get_state(spp)) {
		case ASP_UP:
			sgp_set_state(q, spp, ASP_WACK_ASPDN);
			/* fall through */
		case ASP_WACK_ASPDN:
			for (sgp = spp->gp.list; sgp; sgp = sgp->spp.next)
				if (gp_get_state(sgp) != AS_DOWN)
					if ((err = gp_p_set_state(q, sgp, AS_DOWN)))
						return (err);
			sgp_set_state(q, spp, ASP_WACK_ASPUP);
			/* fall through */
		case ASP_WACK_ASPUP:
			todo(("Notify management that the SPP has failed\n"));
			sgp_set_state(q, spp, ASP_DOWN);
			/* fall through */
		case ASP_DOWN:
			for (sp2 = spp->sp.sp->spp.list; sp2 && sp2 != spp; sp2 = sp2->sp.next)
				if (spp_get_state(sp2) != ASP_DOWN
				    && spp_get_state(sp2) != ASP_WACK_ASPUP)
					if ((err = m2ua_send_mgmt_ntfy(q, xp,
								       UA_STATUS_ASP_FAILURE,
								       &spp->aspid, NULL, 0, NULL,
								       0)))
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

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if (p->OPT_length) {
		uchar *op = mp->b_rptr + p->OPT_offset;
		uchar *oe = op + p->OPT_length;
		struct t_opthdr *oh = (struct t_opthdr *) op;

		if (mp->b_wptr < oe)
			goto efault;
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
			if ((err = ss7_pullupmsg(q, mp->b_cont, -1)))
				return (err);
			return m2ua_recv_msg(xp, q, mp->b_cont);
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
			if ((err = ss7_pullupmsg(q, mp->b_cont, -1)))
				return (err);
			return m2ua_recv_msg(xp, q, mp->b_cont);
		}
	}
      efault:
	swerr();
	return (-EFAULT);
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

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto efault;
	if ((loc_len = p->LOCADDR_length)) {
		loc_ptr = mp->b_rptr + p->LOCADDR_offset;
		if (mp->b_wptr < loc_ptr + loc_len)
			goto efault;
		if (loc_len <= sizeof(xp->loc))
			bcopy(loc_ptr, &xp->loc, sizeof(xp->loc));
	}
	if ((rem_len = p->REMADDR_length)) {
		rem_ptr = mp->b_rptr + p->REMADDR_offset;
		if (mp->b_wptr < rem_ptr + rem_len)
			goto efault;
		if (rem_len <= sizeof(xp->rem))
			bcopy(rem_ptr, &xp->rem, sizeof(xp->rem));
	}
	return (QR_DONE);
      efault:
	swerr();
	return (-EFAULT);
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
	struct sl *sl_u = SL_PRIV(q), *sl_p = NULL;
	struct as *as_u, *as_p;
	struct ap *ap;

	spin_lock_m2ua(&master.lock);
	{
		if ((as_u = sl_u->as.as))
			for (ap = as_u->ap.list; ap; ap = ap->p.next)
				if ((as_p = ap->p.as))
					if ((sl_p = as_p->sl.list))
						break;
		if (sl_p && sl_p->oq) {
			if (sl_p->ioc)
				ret = -EBUSY;
			else {
				sl_p->ioc = sl_get(sl_u);
				putnext(sl_p->oq, mp);
				ret = QR_ABSORBED;
			}
		} else {
			ret = -EOPNOTSUPP;
		}
	}
	spin_unlock_m2ua(&master.lock);
	return (ret);
}
STATIC int
sl_ackpass(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sl *sl_p = SL_PRIV(q), *sl_u;

		if (sl_p->ioc) {
			sl_put((sl_u = xchg(&sl_p->ioc, NULL)));
			if (sl_u->oq) {
				putnext(sl_u->oq, mp);
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
		struct sl *sl_u = SL_PRIV(q), *sl_p = NULL;
		struct as *as_u, *as_p;
		struct ap *ap;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_m2ua(&master.lock);
		{
			if ((as_u = sl_u->as.as))
				for (ap = as_u->ap.list; ap; ap = ap->p.next)
					if ((as_p = ap->p.as))
						if ((sl_p = as_p->sl.list))
							break;
			if (sl_p && sl_p->oq) {
				if (sl_p->ioc)
					ret = -EBUSY;
				else {
					sl_p->ioc = sl_get(sl_u);
					putnext(sl_p->oq, mp);
					ret = QR_ABSORBED;
				}
			} else {
				*arg = sl_u->proto;
			}
		}
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_ackgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sl *sl_p = SL_PRIV(q), *sl_u;

		if (sl_p->ioc) {
			lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

			sl_put((sl_u = xchg(&sl_p->ioc, NULL)));
			sl_u->proto = *arg;
			if (sl_u->oq) {
				putnext(sl_u->oq, mp);
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
		struct sl *sl_u = SL_PRIV(q), *sl_p = NULL;
		struct as *as_u, *as_p;
		struct ap *ap;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		spin_lock_m2ua(&master.lock);
		{
			if ((as_u = sl_u->as.as))
				for (ap = as_u->ap.list; ap; ap = ap->p.next)
					if ((as_p = ap->p.as))
						if ((sl_p = as_p->sl.list))
							break;
			if (sl_p && sl_p->oq) {
				if (sl_p->ioc)
					ret = -EBUSY;
				else {
					sl_p->ioc = sl_get(sl_u);
					putnext(sl_p->oq, mp);
					ret = QR_ABSORBED;
				}
			} else {
				sl_u->proto = *arg;
			}
		}
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sl_acksoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sl *sl_p = SL_PRIV(q), *sl_u;

		if (sl_p->ioc) {
			lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

			sl_put((sl_u = xchg(&sl_p->ioc, NULL)));
			sl_u->proto = *arg;
			if (sl_u->oq) {
				putnext(sl_u->oq, mp);
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
m2ua_opt_get_as(m2ua_option_t * arg, struct as *as, int size)
{
	m2ua_opt_conf_as_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!as)
		return (-EINVAL);
	*opt = as->config;
	return (QR_DONE);
}
STATIC int
m2ua_opt_get_sp(m2ua_option_t * arg, struct sp *sp, int size)
{
	m2ua_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sp)
		return (-EINVAL);
	*opt = sp->config;
	return (QR_DONE);
}
STATIC int
m2ua_opt_get_spp(m2ua_option_t * arg, struct spp *spp, int size)
{
	m2ua_opt_conf_spp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!spp)
		return (-EINVAL);
	*opt = spp->config;
	return (QR_DONE);
}
STATIC int
m2ua_opt_get_sl(m2ua_option_t * arg, struct sl *sl, int size)
{
	m2ua_opt_conf_sl_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sl)
		return (-EINVAL);
	*opt = sl->config;
	return (QR_DONE);
}
STATIC int
m2ua_opt_get_xp(m2ua_option_t * arg, struct xp *xp, int size)
{
	m2ua_opt_conf_xp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!xp)
		return (-EINVAL);
	*opt = xp->config;
	return (QR_DONE);
}
STATIC int
m2ua_opt_get_df(m2ua_option_t * arg, struct df *df, int size)
{
	m2ua_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);

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
m2ua_opt_set_as(m2ua_option_t * arg, struct as *as, int size)
{
	m2ua_opt_conf_as_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!as)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	as->config = *opt;
	return (QR_DONE);
}
STATIC int
m2ua_opt_set_sp(m2ua_option_t * arg, struct sp *sp, int size)
{
	m2ua_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sp)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	sp->config = *opt;
	return (QR_DONE);
}
STATIC int
m2ua_opt_set_spp(m2ua_option_t * arg, struct spp *spp, int size)
{
	m2ua_opt_conf_spp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!spp)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	spp->config = *opt;
	return (QR_DONE);
}
STATIC int
m2ua_opt_set_sl(m2ua_option_t * arg, struct sl *sl, int size)
{
	m2ua_opt_conf_sl_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!sl)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	sl->config = *opt;
	return (QR_DONE);
}
STATIC int
m2ua_opt_set_xp(m2ua_option_t * arg, struct xp *xp, int size)
{
	m2ua_opt_conf_xp_t *opt = (typeof(opt)) (arg + 1);

	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!xp)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	xp->config = *opt;
	return (QR_DONE);
}
STATIC int
m2ua_opt_set_df(m2ua_option_t * arg, struct df *df, int size)
{
	m2ua_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);

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
m2ua_get_as(m2ua_config_t * arg, struct as *as, int size)
{
	m2ua_conf_as_t *cnf = (typeof(cnf)) (arg + 1);
	struct ap *ap;
	m2ua_conf_as_t *cha;
	struct sl *sl;
	m2ua_conf_sl_t *chd;

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
	for (sl = as->sl.list; sl && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sl = sl->as.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = sl->type;
		arg->id = sl->id;
		chd->asid = as->id;
		chd->muxid = sl->u.mux.index;
		chd->iid = sl->iid;
		chd->add = sl->add;
		chd->proto = sl->proto;
	}
	/* terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
m2ua_get_sp(m2ua_config_t * arg, struct sp *sp, int size)
{
	m2ua_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);
	struct np *np;
	m2ua_conf_sp_t *chp;
	struct as *as;
	m2ua_conf_as_t *cha;
	struct spp *spp;
	m2ua_conf_spp_t *chs;

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
m2ua_get_spp(m2ua_config_t * arg, struct spp *spp, int size)
{
	m2ua_conf_spp_t *cnf = (typeof(cnf)) (arg + 1);
	struct xp *xp;
	m2ua_conf_xp_t *chd;

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
	if ((xp = spp->xp) && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg)) {
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
m2ua_get_sl(m2ua_config_t * arg, struct sl *sl, int size)
{
	m2ua_conf_sl_t *cnf = (typeof(cnf)) (arg + 1);

	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!sl)
		return (-EINVAL);
	/* write out queried object */
	cnf->asid = sl->as.as ? sl->as.as->id : 0;
	cnf->muxid = sl->u.mux.index;
	cnf->iid = sl->iid;
	cnf->add = sl->add;
	cnf->proto = sl->proto;
	arg = (typeof(arg)) (cnf + 1);
	/* terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
m2ua_get_xp(m2ua_config_t * arg, struct xp *xp, int size)
{
	m2ua_conf_xp_t *cnf = (typeof(cnf)) (arg + 1);

	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!xp)
		return (-EINVAL);
	/* write out queried object */
	cnf->sppid = xp->spp ? xp->spp->id : 0;
	cnf->spid = xp->sp ? xp->sp->id : ((xp->spp && xp->spp->sp.sp) ? xp->spp->sp.sp->id : 0);
	cnf->muxid = xp->u.mux.index;
	arg = (typeof(arg)) (cnf + 1);
	/* terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
m2ua_get_df(m2ua_config_t * arg, struct df *df, int size)
{
	m2ua_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	struct sp *sp;
	m2ua_conf_sp_t *chd;

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
m2ua_add_as(m2ua_config_t * arg, struct as *as, int size, int force, int test)
{
	struct sp *sp = NULL;
	m2ua_conf_as_t *cnf = (typeof(cnf)) (arg + 1);

	if (as || (size -= sizeof(*cnf)) < 0)
		goto einval;
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (!sp)
		goto einval;
	for (as = sp->as.list; as; as = as->sp.next)
		if (as->iid == cnf->iid)
			goto einval;
	/* make sure user has specified correct types */
	switch (arg->type) {
	case M2UA_OBJ_TYPE_AS_U:
		if (sp->type != M2UA_OBJ_TYPE_SP)
			goto einval;
		break;
	case M2UA_OBJ_TYPE_AS_P:
		if (sp->type != M2UA_OBJ_TYPE_SG)
			goto einval;
		break;
	default:
		swerr();
		goto efault;
	}
	if (!test) {
		if (!(as = m2ua_alloc_as(as_get_id(arg->id), arg->type, sp, cnf->iid, &cnf->add)))
			goto enomem;
		arg->id = as->id;
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
m2ua_add_sp(m2ua_config_t * arg, struct sp *sp, int size, int force, int test)
{
	struct sp *osp = NULL;
	m2ua_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);

	if (sp || (size -= sizeof(*cnf)) < 0)
		goto einval;
	if (cnf->spid)
		osp = sp_lookup(cnf->spid);
	/* make sure user has specified correct types */
	switch (arg->type) {
	case M2UA_OBJ_TYPE_SP:
		if (osp)
			goto einval;
		break;
	case M2UA_OBJ_TYPE_SG:
		if (!osp)
			goto einval;
		if (osp->type != M2UA_OBJ_TYPE_SP)
			goto einval;
		break;
	default:
		swerr();
		goto efault;
	}
	if (!test) {
		if (!
		    (sp = m2ua_alloc_sp(sp_get_id(arg->id), arg->type, osp, cnf->cost, cnf->tmode)))
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
m2ua_add_spp(m2ua_config_t * arg, struct spp *spp, int size, int force, int test)
{
	struct sp *sp = NULL;
	m2ua_conf_spp_t *cnf = (typeof(cnf)) (arg + 1);

	if (spp || (size -= sizeof(*cnf)) < 0)
		goto einval;
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (!sp)
		goto einval;
	if (cnf->aspid)
		for (spp = sp->spp.list; spp; spp = spp->sp.next)
			if (spp->aspid == cnf->aspid)
				goto einval;
	/* make sure user has specified correct types */
	switch (arg->type) {
	case M2UA_OBJ_TYPE_ASP:
		if (sp->type != M2UA_OBJ_TYPE_SP)
			goto einval;
		break;
	case M2UA_OBJ_TYPE_SGP:
		if (sp->type != M2UA_OBJ_TYPE_SG)
			goto einval;
		break;
	case M2UA_OBJ_TYPE_SPP:
		if ((sp->type != M2UA_OBJ_TYPE_SP) && (sp->type != M2UA_OBJ_TYPE_SG))
			goto einval;
		break;
	default:
		swerr();
		goto efault;
	}
	if (!test) {
		if (!
		    (spp = m2ua_alloc_spp(spp_get_id(arg->id), arg->type, sp, cnf->aspid,
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
m2ua_add_sl(m2ua_config_t * arg, struct sl *sl, int size, int force, int test)
{
	struct as *as = NULL;
	m2ua_conf_sl_t *cnf = (typeof(cnf)) (arg + 1);

	if (sl || (size -= sizeof(*cnf)) < 0)
		goto einval;
	if (cnf->asid)
		as = as_lookup(cnf->asid);
	if (!as)
		goto einval;
	if (!(sl = (struct sl *) link_lookup(cnf->muxid)))
		goto einval;
	/* already typed */
	if (sl->type)
		goto einval;
	/* no, sorry, we can't link SL-Us */
	if (arg->type != M2UA_OBJ_TYPE_SL_P)
		goto einval;
	if (!test) {
		m2ua_alloc_sl(sl, sl_get_id(arg->id), arg->type, as, cnf->iid, &cnf->add);
		arg->id = sl->id;
	}
	return (QR_DONE);
      einval:
	return (-EINVAL);
}
STATIC int
m2ua_add_xp(m2ua_config_t * arg, struct xp *xp, int size, int force, int test)
{
	struct sp *sp = NULL;
	struct spp *spp = NULL;
	m2ua_conf_xp_t *cnf = (typeof(cnf)) (arg + 1);

	if (xp || (size -= sizeof(*cnf)) < 0)
		goto einval;
	if (cnf->spid)
		sp = sp_lookup(cnf->spid);
	if (cnf->sppid)
		spp = spp_lookup(cnf->sppid);
	/* must specify one or the other */
	if ((!spp && !sp) || (spp && sp))
		goto einval;
	if (!(xp = (struct xp *) link_lookup(cnf->muxid)))
		goto einval;
	/* already typed */
	if (xp->type)
		goto einval;
	if (!test) {
		m2ua_alloc_xp(xp, xp_get_id(arg->id), arg->type, spp, sp);
		arg->id = xp->id;
	}
	return (QR_DONE);
      einval:
	return (-EINVAL);
}
STATIC int
m2ua_add_df(m2ua_config_t * arg, struct df *df, int size, int force, int test)
{
	m2ua_conf_df_t *cnf = (typeof(cnf)) (arg + 1);

	if (df || (size -= sizeof(*cnf)) < 0)
		goto einval;
	goto einval;
      einval:
	return (-EINVAL);
}

/*
 *  CHA Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_cha_as(m2ua_config_t * arg, struct as *as, int size, int force, int test)
{
	struct as *a;
	m2ua_conf_as_t *cnf = (typeof(cnf)) (arg + 1);

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
m2ua_cha_sp(m2ua_config_t * arg, struct sp *sp, int size, int force, int test)
{
	m2ua_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);

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
m2ua_cha_spp(m2ua_config_t * arg, struct spp *spp, int size, int force, int test)
{
	m2ua_conf_spp_t *cnf = (typeof(cnf)) (arg + 1);

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
m2ua_cha_sl(m2ua_config_t * arg, struct sl *sl, int size, int force, int test)
{
	struct as *a;
	struct sl *s;
	m2ua_conf_sl_t *cnf = (typeof(cnf)) (arg + 1);

	if (!sl || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->asid && cnf->asid != sl->as.as->id)
		return (-EINVAL);
	if (cnf->muxid && cnf->muxid != sl->u.mux.index)
		return (-EINVAL);
	/* can't change to existing iid */
	if (cnf->iid && cnf->iid != sl->iid)
		for (a = sl->as.as->sp.sp->as.list; a; a = a->sp.next)
			for (s = a->sl.list; s; s = s->as.next)
				if (s->iid == cnf->iid)
					return (-EINVAL);
	if (!force) {
		/* involved providing service */
		if (as_get_state(sl->as.as) != AS_INACTIVE)
			return (-EBUSY);
	}
	if (!test) {
		sl->iid = cnf->iid;
		sl->add = cnf->add;
		sl->proto = cnf->proto;
	}
	return (QR_DONE);
}
STATIC int
m2ua_cha_xp(m2ua_config_t * arg, struct xp *xp, int size, int force, int test)
{
	m2ua_conf_xp_t *cnf = (typeof(cnf)) (arg + 1);

	if (!xp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->sppid && (!xp->spp || cnf->sppid != xp->spp->id))
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
m2ua_cha_df(m2ua_config_t * arg, struct df *df, int size, int force, int test)
{
	m2ua_conf_df_t *cnf = (typeof(cnf)) (arg + 1);

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
m2ua_del_as(m2ua_config_t * arg, struct as *as, int size, int force, int test)
{
	if (!as)
		return (-EINVAL);
	if (!force) {
		/* attached to signalling link */
		if (as->sl.list)
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
		m2ua_free_as(as);
	}
	return (QR_DONE);
}
STATIC int
m2ua_del_sp(m2ua_config_t * arg, struct sp *sp, int size, int force, int test)
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
		m2ua_free_sp(sp);
	}
	return (QR_DONE);
}
STATIC int
m2ua_del_spp(m2ua_config_t * arg, struct spp *spp, int size, int force, int test)
{
	if (!spp)
		return (-EINVAL);
	if (!force) {
		/* attached to XP */
		if (spp->xp)
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
		m2ua_free_spp(spp);
	}
	return (QR_DONE);
}
STATIC int
m2ua_del_sl(m2ua_config_t * arg, struct sl *sl, int size, int force, int test)
{
	if (!sl)
		return (-EINVAL);
	if (!force) {
		/* bound to internal datastructures */
		if (sl->as.as)
			return (-EBUSY);
	}
	if (!test) {
		noenable(sl->oq);
		noenable(sl->iq);
		if (sl->as.as) {
			fixme(("Notify any active AS that we have gone away.\n"));
			if ((*sl->as.prev = sl->as.next))
				sl->as.next->as.prev = sl->as.prev;
			sl->as.next = NULL;
			sl->as.prev = &sl->as.next;
			ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
			sl_put(sl);
			assure(sl->as.as->sl.numb > 0);
			sl->as.as->sl.numb--;
			as_put(xchg(&sl->as.as, NULL));
		}
		/* we are now a typeless link waiting for I_UNLINK */
		sl->id = 0;
		sl->type = 0;
	}
	return (QR_DONE);
}
STATIC int
m2ua_del_xp(m2ua_config_t * arg, struct xp *xp, int size, int force, int test)
{
	if (!xp)
		return (-EINVAL);
	if (!force) {
		/* bound to internal datastructures */
		if (xp->spp || xp->sp)
			return (-EBUSY);
	}
	if (!test) {
		noenable(xp->iq);
		noenable(xp->oq);
		/* unlink from spp */
		if (xp->spp) {
			fixme(("Check deactivation of all AS\n"));
			xp_put(xchg(&xp->spp->xp, NULL));
			spp_put(xchg(&xp->spp, NULL));
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
m2ua_del_df(m2ua_config_t * arg, struct df *df, int size, int force, int test)
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
m2ua_sta_as(m2ua_statem_t * arg, struct as *as, int size)
{
	m2ua_statem_as_t *sta = (typeof(sta)) (arg + 1);
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
m2ua_sta_sp(m2ua_statem_t * arg, struct sp *sp, int size)
{
	m2ua_statem_sp_t *sta = (typeof(sta)) (arg + 1);
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
m2ua_sta_spp(m2ua_statem_t * arg, struct spp *spp, int size)
{
	m2ua_statem_spp_t *sta = (typeof(sta)) (arg + 1);
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
m2ua_sta_sl(m2ua_statem_t * arg, struct sl *sl, int size)
{
	m2ua_statem_sl_t *sta = (typeof(sta)) (arg + 1);

	if (!sl || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	arg->flags = sl_get_flags(sl);
	arg->state = sl_get_state(sl);
	sta->timers = sl->timers;
	return (QR_DONE);
}
STATIC int
m2ua_sta_xp(m2ua_statem_t * arg, struct xp *xp, int size)
{
	m2ua_statem_xp_t *sta = (typeof(sta)) (arg + 1);

	if (!xp || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	arg->flags = xp_get_flags(xp);
	arg->state = xp_get_state(xp);
	sta->timers = xp->timers;
	return (QR_DONE);
}
STATIC int
m2ua_sta_df(m2ua_statem_t * arg, struct df *df, int size)
{
	m2ua_statem_df_t *sta = (typeof(sta)) (arg + 1);

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
m2ua_statp_get_as(m2ua_stats_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_get_sp(m2ua_stats_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_get_spp(m2ua_stats_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_get_sl(m2ua_stats_t * arg, struct sl *sl, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_get_xp(m2ua_stats_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_get_df(m2ua_stats_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  SET Statistics Periods
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_statp_set_as(m2ua_stats_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_set_sp(m2ua_stats_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_set_spp(m2ua_stats_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_set_sl(m2ua_stats_t * arg, struct sl *sl, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_set_xp(m2ua_stats_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_statp_set_df(m2ua_stats_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  GET Statistics
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_stat_get_as(m2ua_stats_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_get_sp(m2ua_stats_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_get_spp(m2ua_stats_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_get_sl(m2ua_stats_t * arg, struct sl *sl, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_get_xp(m2ua_stats_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_get_df(m2ua_stats_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  CLR Statistics
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_stat_clr_as(m2ua_stats_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_clr_sp(m2ua_stats_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_clr_spp(m2ua_stats_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_clr_sl(m2ua_stats_t * arg, struct sl *sl, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_clr_xp(m2ua_stats_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_stat_clr_df(m2ua_stats_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  GET Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_not_get_as(m2ua_notify_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_get_sp(m2ua_notify_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_get_spp(m2ua_notify_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_get_sl(m2ua_notify_t * arg, struct sl *sl, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_get_xp(m2ua_notify_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_get_df(m2ua_notify_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  SET Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_not_set_as(m2ua_notify_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_set_sp(m2ua_notify_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_set_spp(m2ua_notify_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_set_sl(m2ua_notify_t * arg, struct sl *sl, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_set_xp(m2ua_notify_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_set_df(m2ua_notify_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  CLR Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_not_clr_as(m2ua_notify_t * arg, struct as *as, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_clr_sp(m2ua_notify_t * arg, struct sp *sp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_clr_spp(m2ua_notify_t * arg, struct spp *spp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_clr_sl(m2ua_notify_t * arg, struct sl *sl, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_clr_xp(m2ua_notify_t * arg, struct xp *xp, int size)
{
	return (-ENOSYS);
}
STATIC int
m2ua_not_clr_df(m2ua_notify_t * arg, struct df *df, int size)
{
	return (-ENOSYS);
}

/*
 *  UP Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_up_as(m2ua_mgmt_t * arg, struct as *as)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_up_sp(m2ua_mgmt_t * arg, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_up_spp(m2ua_mgmt_t * arg, struct spp *spp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_up_sl(m2ua_mgmt_t * arg, struct sl *sl)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_up_xp(m2ua_mgmt_t * arg, struct xp *xp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_up_df(m2ua_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}

/*
 *  DOWN Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_down_as(m2ua_mgmt_t * arg, struct as *as)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_down_sp(m2ua_mgmt_t * arg, struct sp *sp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_down_spp(m2ua_mgmt_t * arg, struct spp *spp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_down_sl(m2ua_mgmt_t * arg, struct sl *sl)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_down_xp(m2ua_mgmt_t * arg, struct xp *xp)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_down_df(m2ua_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}

/*
 *  ACTIVATE Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_act_as(m2ua_mgmt_t * arg, struct as *as)
{
	if (!as || as->type != M2UA_OBJ_TYPE_AS_P)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_act_sp(m2ua_mgmt_t * arg, struct sp *sp)
{
	if (!sp || sp->type != M2UA_OBJ_TYPE_SG)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_act_spp(m2ua_mgmt_t * arg, struct spp *spp)
{
	if (!spp || spp->type != M2UA_OBJ_TYPE_SGP)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_act_sl(m2ua_mgmt_t * arg, struct sl *sl)
{
	if (!sl || sl->type != M2UA_OBJ_TYPE_SL_P)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_act_xp(m2ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp || !xp->spp || xp->spp->type != M2UA_OBJ_TYPE_SGP)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_act_df(m2ua_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}

/*
 *  DEACTIVATE Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_deact_as(m2ua_mgmt_t * arg, struct as *as)
{
	if (!as || as->type != M2UA_OBJ_TYPE_AS_P)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_deact_sp(m2ua_mgmt_t * arg, struct sp *sp)
{
	if (!sp || sp->type != M2UA_OBJ_TYPE_SG)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_deact_spp(m2ua_mgmt_t * arg, struct spp *spp)
{
	if (!spp || spp->type != M2UA_OBJ_TYPE_SGP)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_deact_sl(m2ua_mgmt_t * arg, struct sl *sl)
{
	if (!sl || sl->type != M2UA_OBJ_TYPE_SL_P)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_deact_xp(m2ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp || !xp->spp || xp->spp->type != M2UA_OBJ_TYPE_SGP)
		return (-EINVAL);
	todo(("Write this function\n"));
	return (-ENOSYS);
}
STATIC int
m2ua_deact_df(m2ua_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-ENOSYS);
}

/*
 *  UP BLOCK Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_up_blo_as(m2ua_mgmt_t * arg, struct as *as)
{
	struct gp *gp;

	if (!as)
		return (-EINVAL);
	for (gp = as->gp.list; gp; gp = gp->as.next)
		spp_set_flags(gp->spp.spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_blo_sp(m2ua_mgmt_t * arg, struct sp *sp)
{
	struct spp *spp;

	if (!sp)
		return (-EINVAL);
	for (spp = sp->spp.list; spp; spp = spp->sp.next)
		spp_set_flags(spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_blo_spp(m2ua_mgmt_t * arg, struct spp *spp)
{
	if (!spp)
		return (-EINVAL);
	spp_set_flags(spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_blo_sl(m2ua_mgmt_t * arg, struct sl *sl)
{
	if (!sl)
		return (-EINVAL);
	sl_set_flags(sl, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_blo_xp(m2ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp || !xp->spp)
		return (-EINVAL);
	spp_set_flags(xp->spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_blo_df(m2ua_mgmt_t * arg, struct df *df)
{
	struct sl *sl;
	struct spp *spp;

	for (spp = master.spp.list; spp; spp = spp->next)
		spp_set_flags(spp, ASF_MGMT_BLOCKED);
	for (sl = (struct sl *) master.priv.list; sl; sl = sl->next)
		if (sl->type == M2UA_OBJ_TYPE_SL_U)
			sl_set_flags(sl, ASF_MGMT_BLOCKED);
	for (sl = (struct sl *) master.link.list; sl; sl = sl->next)
		if (sl->type == M2UA_OBJ_TYPE_SL_P)
			sl_set_flags(sl, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}

/*
 *  UP UNBLOCK Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_up_ubl_as(m2ua_mgmt_t * arg, struct as *as)
{
	struct gp *gp;

	if (!as)
		return (-EINVAL);
	for (gp = as->gp.list; gp; gp = gp->as.next)
		spp_clr_flags(gp->spp.spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_ubl_sp(m2ua_mgmt_t * arg, struct sp *sp)
{
	struct spp *spp;

	if (!sp)
		return (-EINVAL);
	for (spp = sp->spp.list; spp; spp = spp->sp.next)
		spp_clr_flags(spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_ubl_spp(m2ua_mgmt_t * arg, struct spp *spp)
{
	if (!spp)
		return (-EINVAL);
	spp_clr_flags(spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_ubl_sl(m2ua_mgmt_t * arg, struct sl *sl)
{
	if (!sl)
		return (-EINVAL);
	sl_clr_flags(sl, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_ubl_xp(m2ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp || !xp->spp)
		return (-EINVAL);
	spp_clr_flags(xp->spp, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_up_ubl_df(m2ua_mgmt_t * arg, struct df *df)
{
	struct sl *sl;
	struct spp *spp;

	for (spp = master.spp.list; spp; spp = spp->next)
		spp_clr_flags(spp, ASF_MGMT_BLOCKED);
	for (sl = (struct sl *) master.priv.list; sl; sl = sl->next)
		if (sl->type == M2UA_OBJ_TYPE_SL_U)
			sl_clr_flags(sl, ASF_MGMT_BLOCKED);
	for (sl = (struct sl *) master.link.list; sl; sl = sl->next)
		if (sl->type == M2UA_OBJ_TYPE_SL_P)
			sl_clr_flags(sl, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}

/*
 *  ACT BLOCK Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
m2ua_act_blo_as(m2ua_mgmt_t * arg, struct as *as)
{
	if (!as)
		return (-EINVAL);
	as_set_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_act_blo_sp(m2ua_mgmt_t * arg, struct sp *sp)
{
	struct as *as;

	if (!sp)
		return (-EINVAL);
	for (as = sp->as.list; as; as = as->sp.next)
		as_set_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_act_blo_spp(m2ua_mgmt_t * arg, struct spp *spp)
{
	struct gp *gp;

	if (!spp)
		return (-EINVAL);
	for (gp = spp->gp.list; gp; gp = gp->spp.next)
		as_set_flags(gp->as.as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_act_blo_sl(m2ua_mgmt_t * arg, struct sl *sl)
{
	if (!sl)
		return (-EINVAL);
	return m2ua_act_blo_as(arg, sl->as.as);
}
STATIC int
m2ua_act_blo_xp(m2ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp)
		return (-EINVAL);
	return m2ua_act_blo_spp(arg, xp->spp);
}
STATIC int
m2ua_act_blo_df(m2ua_mgmt_t * arg, struct df *df)
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
m2ua_act_ubl_as(m2ua_mgmt_t * arg, struct as *as)
{
	if (!as)
		return (-EINVAL);
	as_clr_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_act_ubl_sp(m2ua_mgmt_t * arg, struct sp *sp)
{
	struct as *as;

	if (!sp)
		return (-EINVAL);
	for (as = sp->as.list; as; as = as->sp.next)
		as_clr_flags(as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_act_ubl_spp(m2ua_mgmt_t * arg, struct spp *spp)
{
	struct gp *gp;

	if (!spp)
		return (-EINVAL);
	for (gp = spp->gp.list; gp; gp = gp->spp.next)
		as_clr_flags(gp->as.as, ASF_MGMT_BLOCKED);
	return (QR_DONE);
}
STATIC int
m2ua_act_ubl_sl(m2ua_mgmt_t * arg, struct sl *sl)
{
	if (!sl)
		return (-EINVAL);
	return m2ua_act_ubl_as(arg, sl->as.as);
}
STATIC int
m2ua_act_ubl_xp(m2ua_mgmt_t * arg, struct xp *xp)
{
	if (!xp)
		return (-EINVAL);
	return m2ua_act_ubl_spp(arg, xp->spp);
}
STATIC int
m2ua_act_ubl_df(m2ua_mgmt_t * arg, struct df *df)
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
m2ua_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		m2ua_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_opt_get_as(arg, as_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_opt_get_sp(arg, sp_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_opt_get_spp(arg, spp_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_opt_get_sl(arg, sl_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_opt_get_xp(arg, xp_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_DF:
				return m2ua_opt_get_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCsOPTIONS
 *  -----------------------------------
 */
STATIC int
m2ua_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		m2ua_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_opt_set_as(arg, as_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_opt_set_sp(arg, sp_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_opt_set_spp(arg, spp_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_opt_set_sl(arg, sl_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_opt_set_xp(arg, xp_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_DF:
				return m2ua_opt_set_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCGCONFIG
 *  -----------------------------------
 */
STATIC int
m2ua_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		m2ua_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_get_as(arg, as_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_get_sp(arg, sp_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_get_spp(arg, spp_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_get_sl(arg, sl_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_get_xp(arg, xp_lookup(arg->id), size);
			case M2UA_OBJ_TYPE_DF:
				return m2ua_get_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCSCONFIG
 *  -----------------------------------
 */
STATIC int
m2ua_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		m2ua_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case M2UA_ADD:
				switch (arg->type) {
				case M2UA_OBJ_TYPE_AS_U:
				case M2UA_OBJ_TYPE_AS_P:
					return m2ua_add_as(arg, as_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_SP:
				case M2UA_OBJ_TYPE_SG:
					return m2ua_add_sp(arg, sp_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_ASP:
				case M2UA_OBJ_TYPE_SGP:
				case M2UA_OBJ_TYPE_SPP:
					return m2ua_add_spp(arg, spp_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_SL_U:
				case M2UA_OBJ_TYPE_SL_P:
					return m2ua_add_sl(arg, sl_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_XP_SCTP:
				case M2UA_OBJ_TYPE_XP_TCP:
				case M2UA_OBJ_TYPE_XP_SSCOP:
					return m2ua_add_xp(arg, xp_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_DF:
					return m2ua_add_df(arg, &master, size, 0, 0);
				}
				break;
			case M2UA_CHA:
				switch (arg->type) {
				case M2UA_OBJ_TYPE_AS_U:
				case M2UA_OBJ_TYPE_AS_P:
					return m2ua_cha_as(arg, as_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_SP:
				case M2UA_OBJ_TYPE_SG:
					return m2ua_cha_sp(arg, sp_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_ASP:
				case M2UA_OBJ_TYPE_SGP:
				case M2UA_OBJ_TYPE_SPP:
					return m2ua_cha_spp(arg, spp_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_SL_U:
				case M2UA_OBJ_TYPE_SL_P:
					return m2ua_cha_sl(arg, sl_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_XP_SCTP:
				case M2UA_OBJ_TYPE_XP_TCP:
				case M2UA_OBJ_TYPE_XP_SSCOP:
					return m2ua_cha_xp(arg, xp_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_DF:
					return m2ua_cha_df(arg, &master, size, 0, 0);
				}
				break;
			case M2UA_DEL:
				switch (arg->type) {
				case M2UA_OBJ_TYPE_AS_U:
				case M2UA_OBJ_TYPE_AS_P:
					return m2ua_del_as(arg, as_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_SP:
				case M2UA_OBJ_TYPE_SG:
					return m2ua_del_sp(arg, sp_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_ASP:
				case M2UA_OBJ_TYPE_SGP:
				case M2UA_OBJ_TYPE_SPP:
					return m2ua_del_spp(arg, spp_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_SL_U:
				case M2UA_OBJ_TYPE_SL_P:
					return m2ua_del_sl(arg, sl_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_XP_SCTP:
				case M2UA_OBJ_TYPE_XP_TCP:
				case M2UA_OBJ_TYPE_XP_SSCOP:
					return m2ua_del_xp(arg, xp_lookup(arg->id), size, 0, 0);
				case M2UA_OBJ_TYPE_DF:
					return m2ua_del_df(arg, &master, size, 0, 0);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCTCONFIG
 *  -----------------------------------
 */
STATIC int
m2ua_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		m2ua_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case M2UA_ADD:
				switch (arg->type) {
				case M2UA_OBJ_TYPE_AS_U:
				case M2UA_OBJ_TYPE_AS_P:
					return m2ua_add_as(arg, as_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_SP:
				case M2UA_OBJ_TYPE_SG:
					return m2ua_add_sp(arg, sp_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_ASP:
				case M2UA_OBJ_TYPE_SGP:
				case M2UA_OBJ_TYPE_SPP:
					return m2ua_add_spp(arg, spp_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_SL_U:
				case M2UA_OBJ_TYPE_SL_P:
					return m2ua_add_sl(arg, sl_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_XP_SCTP:
				case M2UA_OBJ_TYPE_XP_TCP:
				case M2UA_OBJ_TYPE_XP_SSCOP:
					return m2ua_add_xp(arg, xp_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_DF:
					return m2ua_add_df(arg, &master, size, 0, 1);
				}
				break;
			case M2UA_CHA:
				switch (arg->type) {
				case M2UA_OBJ_TYPE_AS_U:
				case M2UA_OBJ_TYPE_AS_P:
					return m2ua_cha_as(arg, as_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_SP:
				case M2UA_OBJ_TYPE_SG:
					return m2ua_cha_sp(arg, sp_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_ASP:
				case M2UA_OBJ_TYPE_SGP:
				case M2UA_OBJ_TYPE_SPP:
					return m2ua_cha_spp(arg, spp_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_SL_U:
				case M2UA_OBJ_TYPE_SL_P:
					return m2ua_cha_sl(arg, sl_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_XP_SCTP:
				case M2UA_OBJ_TYPE_XP_TCP:
				case M2UA_OBJ_TYPE_XP_SSCOP:
					return m2ua_cha_xp(arg, xp_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_DF:
					return m2ua_cha_df(arg, &master, size, 0, 1);
				}
				break;
			case M2UA_DEL:
				switch (arg->type) {
				case M2UA_OBJ_TYPE_AS_U:
				case M2UA_OBJ_TYPE_AS_P:
					return m2ua_del_as(arg, as_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_SP:
				case M2UA_OBJ_TYPE_SG:
					return m2ua_del_sp(arg, sp_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_ASP:
				case M2UA_OBJ_TYPE_SGP:
				case M2UA_OBJ_TYPE_SPP:
					return m2ua_del_spp(arg, spp_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_SL_U:
				case M2UA_OBJ_TYPE_SL_P:
					return m2ua_del_sl(arg, sl_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_XP_SCTP:
				case M2UA_OBJ_TYPE_XP_TCP:
				case M2UA_OBJ_TYPE_XP_SSCOP:
					return m2ua_del_xp(arg, xp_lookup(arg->id), size, 0, 1);
				case M2UA_OBJ_TYPE_DF:
					return m2ua_del_df(arg, &master, size, 0, 1);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCCCONFIG
 *  -----------------------------------
 */
STATIC int
m2ua_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		m2ua_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case M2UA_ADD:
				switch (arg->type) {
				case M2UA_OBJ_TYPE_AS_U:
				case M2UA_OBJ_TYPE_AS_P:
					return m2ua_add_as(arg, as_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_SP:
				case M2UA_OBJ_TYPE_SG:
					return m2ua_add_sp(arg, sp_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_ASP:
				case M2UA_OBJ_TYPE_SGP:
				case M2UA_OBJ_TYPE_SPP:
					return m2ua_add_spp(arg, spp_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_SL_U:
				case M2UA_OBJ_TYPE_SL_P:
					return m2ua_add_sl(arg, sl_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_XP_SCTP:
				case M2UA_OBJ_TYPE_XP_TCP:
				case M2UA_OBJ_TYPE_XP_SSCOP:
					return m2ua_add_xp(arg, xp_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_DF:
					return m2ua_add_df(arg, &master, size, 1, 0);
				}
				break;
			case M2UA_CHA:
				switch (arg->type) {
				case M2UA_OBJ_TYPE_AS_U:
				case M2UA_OBJ_TYPE_AS_P:
					return m2ua_cha_as(arg, as_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_SP:
				case M2UA_OBJ_TYPE_SG:
					return m2ua_cha_sp(arg, sp_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_ASP:
				case M2UA_OBJ_TYPE_SGP:
				case M2UA_OBJ_TYPE_SPP:
					return m2ua_cha_spp(arg, spp_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_SL_U:
				case M2UA_OBJ_TYPE_SL_P:
					return m2ua_cha_sl(arg, sl_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_XP_SCTP:
				case M2UA_OBJ_TYPE_XP_TCP:
				case M2UA_OBJ_TYPE_XP_SSCOP:
					return m2ua_cha_xp(arg, xp_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_DF:
					return m2ua_cha_df(arg, &master, size, 1, 0);
				}
				break;
			case M2UA_DEL:
				switch (arg->type) {
				case M2UA_OBJ_TYPE_AS_U:
				case M2UA_OBJ_TYPE_AS_P:
					return m2ua_del_as(arg, as_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_SP:
				case M2UA_OBJ_TYPE_SG:
					return m2ua_del_sp(arg, sp_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_ASP:
				case M2UA_OBJ_TYPE_SGP:
				case M2UA_OBJ_TYPE_SPP:
					return m2ua_del_spp(arg, spp_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_SL_U:
				case M2UA_OBJ_TYPE_SL_P:
					return m2ua_del_sl(arg, sl_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_XP_SCTP:
				case M2UA_OBJ_TYPE_XP_TCP:
				case M2UA_OBJ_TYPE_XP_SSCOP:
					return m2ua_del_xp(arg, xp_lookup(arg->id), size, 1, 0);
				case M2UA_OBJ_TYPE_DF:
					return m2ua_del_df(arg, &master, size, 1, 0);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCGSTATEM
 *  -----------------------------------
 *  Get state variables by object type.
 */
STATIC int
m2ua_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = QR_DONE;
		int size = msgdsize(mp);
		m2ua_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_m2ua(&master.lock);
		switch (arg->type) {
		case M2UA_OBJ_TYPE_AS_U:
		case M2UA_OBJ_TYPE_AS_P:
			return m2ua_sta_as(arg, as_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SP:
		case M2UA_OBJ_TYPE_SG:
			return m2ua_sta_sp(arg, sp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_ASP:
		case M2UA_OBJ_TYPE_SGP:
		case M2UA_OBJ_TYPE_SPP:
			return m2ua_sta_spp(arg, spp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SL_U:
		case M2UA_OBJ_TYPE_SL_P:
			return m2ua_sta_sl(arg, sl_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_XP_SCTP:
		case M2UA_OBJ_TYPE_XP_TCP:
		case M2UA_OBJ_TYPE_XP_SSCOP:
			return m2ua_sta_xp(arg, xp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_DF:
			return m2ua_sta_df(arg, &master, size);
		default:
			goto einval;
		}
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCCMRESET
 *  -----------------------------------
 *  Perform master reset.
 */
STATIC int
m2ua_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		m2ua_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

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
m2ua_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = QR_DONE;
		int size = msgdsize(mp);
		m2ua_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_m2ua(&master.lock);
		switch (arg->type) {
		case M2UA_OBJ_TYPE_AS_U:
		case M2UA_OBJ_TYPE_AS_P:
			return m2ua_statp_get_as(arg, as_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SP:
		case M2UA_OBJ_TYPE_SG:
			return m2ua_statp_get_sp(arg, sp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_ASP:
		case M2UA_OBJ_TYPE_SGP:
		case M2UA_OBJ_TYPE_SPP:
			return m2ua_statp_get_spp(arg, spp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SL_U:
		case M2UA_OBJ_TYPE_SL_P:
			return m2ua_statp_get_sl(arg, sl_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_XP_SCTP:
		case M2UA_OBJ_TYPE_XP_TCP:
		case M2UA_OBJ_TYPE_XP_SSCOP:
			return m2ua_statp_get_xp(arg, xp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_DF:
			return m2ua_statp_get_df(arg, &master, size);
		default:
			goto einval;
		}
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCSSTATSP
 *  -----------------------------------
 *  Set statistics periods.
 */
STATIC int
m2ua_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = QR_DONE;
		int size = msgdsize(mp);
		m2ua_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_m2ua(&master.lock);
		switch (arg->type) {
		case M2UA_OBJ_TYPE_AS_U:
		case M2UA_OBJ_TYPE_AS_P:
			return m2ua_statp_set_as(arg, as_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SP:
		case M2UA_OBJ_TYPE_SG:
			return m2ua_statp_set_sp(arg, sp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_ASP:
		case M2UA_OBJ_TYPE_SGP:
		case M2UA_OBJ_TYPE_SPP:
			return m2ua_statp_set_spp(arg, spp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SL_U:
		case M2UA_OBJ_TYPE_SL_P:
			return m2ua_statp_set_sl(arg, sl_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_XP_SCTP:
		case M2UA_OBJ_TYPE_XP_TCP:
		case M2UA_OBJ_TYPE_XP_SSCOP:
			return m2ua_statp_set_xp(arg, xp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_DF:
			return m2ua_statp_set_df(arg, &master, size);
		default:
			goto einval;
		}
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCGSTATS
 *  -----------------------------------
 */
STATIC int
m2ua_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = QR_DONE;
		int size = msgdsize(mp);
		m2ua_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_m2ua(&master.lock);
		switch (arg->type) {
		case M2UA_OBJ_TYPE_AS_U:
		case M2UA_OBJ_TYPE_AS_P:
			return m2ua_stat_get_as(arg, as_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SP:
		case M2UA_OBJ_TYPE_SG:
			return m2ua_stat_get_sp(arg, sp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_ASP:
		case M2UA_OBJ_TYPE_SGP:
		case M2UA_OBJ_TYPE_SPP:
			return m2ua_stat_get_spp(arg, spp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SL_U:
		case M2UA_OBJ_TYPE_SL_P:
			return m2ua_stat_get_sl(arg, sl_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_XP_SCTP:
		case M2UA_OBJ_TYPE_XP_TCP:
		case M2UA_OBJ_TYPE_XP_SSCOP:
			return m2ua_stat_get_xp(arg, xp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_DF:
			return m2ua_stat_get_df(arg, &master, size);
		default:
			goto einval;
		}
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCCSTATS
 *  -----------------------------------
 */
STATIC int
m2ua_ioccstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = QR_DONE;
		int size = msgdsize(mp);
		m2ua_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_m2ua(&master.lock);
		switch (arg->type) {
		case M2UA_OBJ_TYPE_AS_U:
		case M2UA_OBJ_TYPE_AS_P:
			return m2ua_stat_clr_as(arg, as_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SP:
		case M2UA_OBJ_TYPE_SG:
			return m2ua_stat_clr_sp(arg, sp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_ASP:
		case M2UA_OBJ_TYPE_SGP:
		case M2UA_OBJ_TYPE_SPP:
			return m2ua_stat_clr_spp(arg, spp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SL_U:
		case M2UA_OBJ_TYPE_SL_P:
			return m2ua_stat_clr_sl(arg, sl_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_XP_SCTP:
		case M2UA_OBJ_TYPE_XP_TCP:
		case M2UA_OBJ_TYPE_XP_SSCOP:
			return m2ua_stat_clr_xp(arg, xp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_DF:
			return m2ua_stat_clr_df(arg, &master, size);
		default:
			goto einval;
		}
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCGNOTIFY
 *  -----------------------------------
 */
STATIC int
m2ua_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = QR_DONE;
		int size = msgdsize(mp);
		m2ua_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_m2ua(&master.lock);
		switch (arg->type) {
		case M2UA_OBJ_TYPE_AS_U:
		case M2UA_OBJ_TYPE_AS_P:
			return m2ua_not_get_as(arg, as_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SP:
		case M2UA_OBJ_TYPE_SG:
			return m2ua_not_get_sp(arg, sp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_ASP:
		case M2UA_OBJ_TYPE_SGP:
		case M2UA_OBJ_TYPE_SPP:
			return m2ua_not_get_spp(arg, spp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SL_U:
		case M2UA_OBJ_TYPE_SL_P:
			return m2ua_not_get_sl(arg, sl_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_XP_SCTP:
		case M2UA_OBJ_TYPE_XP_TCP:
		case M2UA_OBJ_TYPE_XP_SSCOP:
			return m2ua_not_get_xp(arg, xp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_DF:
			return m2ua_not_get_df(arg, &master, size);
		default:
			goto einval;
		}
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCSNOTIFY
 *  -----------------------------------
 */
STATIC int
m2ua_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = QR_DONE;
		int size = msgdsize(mp);
		m2ua_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_m2ua(&master.lock);
		switch (arg->type) {
		case M2UA_OBJ_TYPE_AS_U:
		case M2UA_OBJ_TYPE_AS_P:
			return m2ua_not_set_as(arg, as_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SP:
		case M2UA_OBJ_TYPE_SG:
			return m2ua_not_set_sp(arg, sp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_ASP:
		case M2UA_OBJ_TYPE_SGP:
		case M2UA_OBJ_TYPE_SPP:
			return m2ua_not_set_spp(arg, spp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SL_U:
		case M2UA_OBJ_TYPE_SL_P:
			return m2ua_not_set_sl(arg, sl_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_XP_SCTP:
		case M2UA_OBJ_TYPE_XP_TCP:
		case M2UA_OBJ_TYPE_XP_SSCOP:
			return m2ua_not_set_xp(arg, xp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_DF:
			return m2ua_not_set_df(arg, &master, size);
		default:
			goto einval;
		}
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCCNOTIFY
 *  -----------------------------------
 */
STATIC int
m2ua_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int ret = QR_DONE;
		int size = msgdsize(mp);
		m2ua_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_m2ua(&master.lock);
		switch (arg->type) {
		case M2UA_OBJ_TYPE_AS_U:
		case M2UA_OBJ_TYPE_AS_P:
			return m2ua_not_clr_as(arg, as_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SP:
		case M2UA_OBJ_TYPE_SG:
			return m2ua_not_clr_sp(arg, sp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_ASP:
		case M2UA_OBJ_TYPE_SGP:
		case M2UA_OBJ_TYPE_SPP:
			return m2ua_not_clr_spp(arg, spp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_SL_U:
		case M2UA_OBJ_TYPE_SL_P:
			return m2ua_not_clr_sl(arg, sl_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_XP_SCTP:
		case M2UA_OBJ_TYPE_XP_TCP:
		case M2UA_OBJ_TYPE_XP_SSCOP:
			return m2ua_not_clr_xp(arg, xp_lookup(arg->id), size);
		case M2UA_OBJ_TYPE_DF:
			return m2ua_not_clr_df(arg, &master, size);
		default:
			goto einval;
		}
	      einval:
		ret = -EINVAL;
		goto exit;
	      exit:
		spin_unlock_m2ua(&master.lock);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCCMGMT
 *  -----------------------------------
 */
STATIC int
m2ua_ioccmgmt(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		m2ua_mgmt_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		switch (arg->cmd) {
		case M2UA_MGMT_UP:
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_up_as(arg, as_lookup(arg->id));
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_up_sp(arg, sp_lookup(arg->id));
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_up_spp(arg, spp_lookup(arg->id));
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_up_sl(arg, sl_lookup(arg->id));
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_up_xp(arg, xp_lookup(arg->id));
			case M2UA_OBJ_TYPE_DF:
				return m2ua_up_df(arg, &master);
			}
			break;
		case M2UA_MGMT_ACTIVATE:
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_act_as(arg, as_lookup(arg->id));
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_act_sp(arg, sp_lookup(arg->id));
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_act_spp(arg, spp_lookup(arg->id));
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_act_sl(arg, sl_lookup(arg->id));
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_act_xp(arg, xp_lookup(arg->id));
			case M2UA_OBJ_TYPE_DF:
				return m2ua_act_df(arg, &master);
			}
			break;
		case M2UA_MGMT_DEACTIVATE:
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_deact_as(arg, as_lookup(arg->id));
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_deact_sp(arg, sp_lookup(arg->id));
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_deact_spp(arg, spp_lookup(arg->id));
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_deact_sl(arg, sl_lookup(arg->id));
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_deact_xp(arg, xp_lookup(arg->id));
			case M2UA_OBJ_TYPE_DF:
				return m2ua_deact_df(arg, &master);
			}
			break;
		case M2UA_MGMT_DOWN:
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_down_as(arg, as_lookup(arg->id));
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_down_sp(arg, sp_lookup(arg->id));
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_down_spp(arg, spp_lookup(arg->id));
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_down_sl(arg, sl_lookup(arg->id));
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_down_xp(arg, xp_lookup(arg->id));
			case M2UA_OBJ_TYPE_DF:
				return m2ua_down_df(arg, &master);
			}
			break;
		case M2UA_MGMT_UP_BLOCK:
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_up_blo_as(arg, as_lookup(arg->id));
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_up_blo_sp(arg, sp_lookup(arg->id));
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_up_blo_spp(arg, spp_lookup(arg->id));
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_up_blo_sl(arg, sl_lookup(arg->id));
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_up_blo_xp(arg, xp_lookup(arg->id));
			case M2UA_OBJ_TYPE_DF:
				return m2ua_up_blo_df(arg, &master);
			}
			break;
		case M2UA_MGMT_UP_UNBLOCK:
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_up_ubl_as(arg, as_lookup(arg->id));
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_up_ubl_sp(arg, sp_lookup(arg->id));
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_up_ubl_spp(arg, spp_lookup(arg->id));
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_up_ubl_sl(arg, sl_lookup(arg->id));
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_up_ubl_xp(arg, xp_lookup(arg->id));
			case M2UA_OBJ_TYPE_DF:
				return m2ua_up_ubl_df(arg, &master);
			}
			break;
		case M2UA_MGMT_ACT_BLOCK:
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_act_blo_as(arg, as_lookup(arg->id));
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_act_blo_sp(arg, sp_lookup(arg->id));
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_act_blo_spp(arg, spp_lookup(arg->id));
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_act_blo_sl(arg, sl_lookup(arg->id));
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_act_blo_xp(arg, xp_lookup(arg->id));
			case M2UA_OBJ_TYPE_DF:
				return m2ua_act_blo_df(arg, &master);
			}
			break;
		case M2UA_MGMT_ACT_UNBLOCK:
			switch (arg->type) {
			case M2UA_OBJ_TYPE_AS_U:
			case M2UA_OBJ_TYPE_AS_P:
				return m2ua_act_ubl_as(arg, as_lookup(arg->id));
			case M2UA_OBJ_TYPE_SP:
			case M2UA_OBJ_TYPE_SG:
				return m2ua_act_ubl_sp(arg, sp_lookup(arg->id));
			case M2UA_OBJ_TYPE_ASP:
			case M2UA_OBJ_TYPE_SGP:
			case M2UA_OBJ_TYPE_SPP:
				return m2ua_act_ubl_spp(arg, spp_lookup(arg->id));
			case M2UA_OBJ_TYPE_SL_U:
			case M2UA_OBJ_TYPE_SL_P:
				return m2ua_act_ubl_sl(arg, sl_lookup(arg->id));
			case M2UA_OBJ_TYPE_XP_SCTP:
			case M2UA_OBJ_TYPE_XP_TCP:
			case M2UA_OBJ_TYPE_XP_SSCOP:
				return m2ua_act_ubl_xp(arg, xp_lookup(arg->id));
			case M2UA_OBJ_TYPE_DF:
				return m2ua_act_ubl_df(arg, &master);
			}
			break;
		}
	}
	rare();
	return (-EINVAL);
}

/*
 *  M2UA_IOCCPASS
 *  -----------------------------------
 */
STATIC int
m2ua_ioccpass(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_cont != NULL)) {
		mblk_t *bp, *dp;
		union link *lk;
		m2ua_pass_t *arg = (typeof(arg)) mp->b_cont->b_rptr;

		if (likely((lk = link_lookup(arg->muxid)) && lk->str.oq)) {
			if (likely((bp = ss7_dupb(q, mp)) != NULL)) {
				if (likely((dp = ss7_dupb(q, mp)) != NULL)) {
					if (likely(arg->type >= QPCTL
						   || bcanputnext(lk->str.oq, mp->b_band))) {
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
					freemsg(bp);
					return (-EBUSY);
				}
				freeb(bp);
				return (-ENOBUFS);
			}
			return (-ENOBUFS);
		}
		rare();
		return (-EINVAL);
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
m2ua_w_ioctl(queue_t *q, mblk_t *mp)
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
			spin_lock_m2ua(&master.lock);
			/* place in list in ascending index order */
			for (lkp = &master.link.list; *lkp && (*lkp)->str.u.mux.index < lb->l_index;
			     lkp = (union link **) &(*lkp)->str.next) ;
			if ((lk = m2ua_alloc_link(lb->l_qbot, lkp, lb->l_index, iocp->ioc_cr))) {
				spin_unlock_m2ua(&master.lock);
				break;
			}
			ret = -ENOMEM;
			spin_unlock_m2ua(&master.lock);
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
			spin_lock_m2ua(&master.lock);
			if ((lk = link_lookup(lb->l_index))) {
				m2ua_free_link(lk->str.iq);
				spin_unlock_m2ua(&master.lock);
				break;
			}
			ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n", DRV_NAME, s));
			ret = -EINVAL;
			spin_unlock_m2ua(&master.lock);
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
			ret = m2ua_iocgoptions(q, mp);
			break;
		case _IOC_NR(M2UA_IOCSOPTIONS):
			printd(("%s: %p: -> M2UA_IOCSOPTIONS\n", DRV_NAME, s));
			ret = m2ua_iocsoptions(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGCONFIG):
			printd(("%s: %p: -> M2UA_IOCGCONFIG\n", DRV_NAME, s));
			ret = m2ua_iocgconfig(q, mp);
			break;
		case _IOC_NR(M2UA_IOCSCONFIG):
			printd(("%s: %p: -> M2UA_IOCSCONFIG\n", DRV_NAME, s));
			ret = m2ua_iocsconfig(q, mp);
			break;
		case _IOC_NR(M2UA_IOCTCONFIG):
			printd(("%s: %p: -> M2UA_IOCTCONFIG\n", DRV_NAME, s));
			ret = m2ua_ioctconfig(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCCONFIG):
			printd(("%s: %p: -> M2UA_IOCCCONFIG\n", DRV_NAME, s));
			ret = m2ua_ioccconfig(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGSTATEM):
			printd(("%s: %p: -> M2UA_IOCGSTATEM\n", DRV_NAME, s));
			ret = m2ua_iocgstatem(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCMRESET):
			printd(("%s: %p: -> M2UA_IOCCMRESET\n", DRV_NAME, s));
			ret = m2ua_ioccmreset(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGSTATSP):
			printd(("%s: %p: -> M2UA_IOCGSTATSP\n", DRV_NAME, s));
			ret = m2ua_iocgstatsp(q, mp);
			break;
		case _IOC_NR(M2UA_IOCSSTATSP):
			printd(("%s: %p: -> M2UA_IOCSSTATSP\n", DRV_NAME, s));
			ret = m2ua_iocsstatsp(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGSTATS):
			printd(("%s: %p: -> M2UA_IOCGSTATS\n", DRV_NAME, s));
			ret = m2ua_iocgstats(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCSTATS):
			printd(("%s: %p: -> M2UA_IOCCSTATS\n", DRV_NAME, s));
			ret = m2ua_ioccstats(q, mp);
			break;
		case _IOC_NR(M2UA_IOCGNOTIFY):
			printd(("%s: %p: -> M2UA_IOCGNOTIFY\n", DRV_NAME, s));
			ret = m2ua_iocgnotify(q, mp);
			break;
		case _IOC_NR(M2UA_IOCSNOTIFY):
			printd(("%s: %p: -> M2UA_IOCSNOTIFY\n", DRV_NAME, s));
			ret = m2ua_iocsnotify(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCNOTIFY):
			printd(("%s: %p: -> M2UA_IOCCNOTIFY\n", DRV_NAME, s));
			ret = m2ua_ioccnotify(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCMGMT):
			printd(("%s: %p: -> M2UA_IOCCMGMT\n", DRV_NAME, s));
			ret = m2ua_ioccmgmt(q, mp);
			break;
		case _IOC_NR(M2UA_IOCCPASS):
			printd(("%s: %p: -> M2UA_IOCCPASS\n", DRV_NAME, s));
			ret = m2ua_ioccpass(q, mp);
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
	struct sl *sl_p = SL_PRIV(q), *sl_u;

	if ((sl_p->ioc)) {
		sl_put((sl_u = xchg(&sl_p->ioc, NULL)));
		if (sl_u->oq) {
			putnext(sl_u->oq, mp);
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
	struct sl *sl = SL_PRIV(q);

	sl->i_oldstate = sl->i_state;

	switch (*(sl_ulong *) mp->b_rptr) {
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", DRV_NAME, sl));
		rtn = slu_info_req(sl, q, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", DRV_NAME, sl));
		rtn = slu_attach_req(sl, q, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", DRV_NAME, sl));
		rtn = slu_detach_req(sl, q, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", DRV_NAME, sl));
		rtn = slu_enable_req(sl, q, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", DRV_NAME, sl));
		rtn = slu_disable_req(sl, q, mp);
		break;
#if 0
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", DRV_NAME, sl));
		rtn = slu_optmgmt_req(sl, q, mp);
		break;
#endif
	case SL_PDU_REQ:
		printd(("%s: %p: -> SL_PDU_REQ\n", DRV_NAME, sl));
		rtn = slu_pdu_req(sl, q, mp);
		break;
	case SL_EMERGENCY_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_REQ\n", DRV_NAME, sl));
		rtn = slu_emergency_req(sl, q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		printd(("%s: %p: -> SL_EMERGENCY_CEASES_REQ\n", DRV_NAME, sl));
		rtn = slu_emergency_ceases_req(sl, q, mp);
		break;
	case SL_START_REQ:
		printd(("%s: %p: -> SL_START_REQ\n", DRV_NAME, sl));
		rtn = slu_start_req(sl, q, mp);
		break;
	case SL_STOP_REQ:
		printd(("%s: %p: -> SL_STOP_REQ\n", DRV_NAME, sl));
		rtn = slu_stop_req(sl, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		printd(("%s: %p: -> SL_RETRIEVE_BSNT_REQ\n", DRV_NAME, sl));
		rtn = slu_retrieve_bsnt_req(sl, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		printd(("%s: %p: -> SL_RETRIEVAL_REQUEST_AND_FSNC_REQ\n", DRV_NAME, sl));
		rtn = slu_retrieval_request_and_fsnc_req(sl, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		printd(("%s: %p: -> SL_CLEAR_BUFFERS_REQ\n", DRV_NAME, sl));
		rtn = slu_clear_buffers_req(sl, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		printd(("%s: %p: -> SL_CLEAR_RTB_REQ\n", DRV_NAME, sl));
		rtn = slu_clear_rtb_req(sl, q, mp);
		break;
	case SL_CONTINUE_REQ:
		printd(("%s: %p: -> SL_CONTINUE_REQ\n", DRV_NAME, sl));
		rtn = slu_continue_req(sl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		printd(("%s: %p: -> SL_LOCAL_PROCESSOR_OUTAGE_REQ\n", DRV_NAME, sl));
		rtn = slu_local_processor_outage_req(sl, q, mp);
		break;
	case SL_RESUME_REQ:
		printd(("%s: %p: -> SL_RESUME_REQ\n", DRV_NAME, sl));
		rtn = slu_resume_req(sl, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		printd(("%s: %p: -> SL_CONGESTION_DISCARD_REQ\n", DRV_NAME, sl));
		rtn = slu_congestion_discard_req(sl, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		printd(("%s: %p: -> SL_CONGESTION_ACCEPT_REQ\n", DRV_NAME, sl));
		rtn = slu_congestion_accept_req(sl, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		printd(("%s: %p: -> SL_NO_CONGESTION_REQ\n", DRV_NAME, sl));
		rtn = slu_no_congestion_req(sl, q, mp);
		break;
	case SL_POWER_ON_REQ:
		printd(("%s: %p: -> SL_POWER_ON_REQ\n", DRV_NAME, sl));
		rtn = slu_power_on_req(sl, q, mp);
		break;
#if 0
	case SL_OPTMGMT_REQ:
		printd(("%s: %p: -> SL_OPTMGMT_REQ\n", DRV_NAME, sl));
		rtn = slu_optmgmt_req(sl, q, mp);
		break;
#endif
#if 0
	case SL_NOTIFY_REQ:
		printd(("%s: %p: -> SL_NOTIFY_REQ\n", DRV_NAME, sl));
		rtn = slu_notify_req(sl, q, mp);
		break;
#endif
	default:
		printd(("%s: %p: -> SL_????\n", DRV_NAME, sl));
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		sl->i_state = sl->i_oldstate;
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
	struct sl *sl = SL_PRIV(q);

	sl->i_oldstate = sl->i_state;

	switch (*(sl_ulong *) mp->b_rptr) {
	case LMI_INFO_ACK:
		printd(("%s: %p: -> LMI_INFO_ACK\n", DRV_NAME, sl));
		rtn = slp_info_ack(sl, q, mp);
		break;
	case LMI_OK_ACK:
		printd(("%s: %p: -> LMI_OK_ACK\n", DRV_NAME, sl));
		rtn = slp_ok_ack(sl, q, mp);
		break;
	case LMI_ERROR_ACK:
		printd(("%s: %p: -> LMI_ERROR_ACK\n", DRV_NAME, sl));
		rtn = slp_error_ack(sl, q, mp);
		break;
	case LMI_ENABLE_CON:
		printd(("%s: %p: -> LMI_ENABLE_CON\n", DRV_NAME, sl));
		rtn = slp_enable_con(sl, q, mp);
		break;
	case LMI_DISABLE_CON:
		printd(("%s: %p: -> LMI_DISABLE_CON\n", DRV_NAME, sl));
		rtn = slp_disable_con(sl, q, mp);
		break;
#if 0
	case LMI_OPTMGMT_ACK:
		printd(("%s: %p: -> LMI_OPTNGNT_ACK\n", DRV_NAME, sl));
		rtn = slp_optmgmt_ack(sl, q, mp);
		break;
#endif
	case LMI_ERROR_IND:
		printd(("%s: %p: -> LMI_ERROR_IND\n", DRV_NAME, sl));
		rtn = slp_error_ind(sl, q, mp);
		break;
	case LMI_STATS_IND:
		printd(("%s: %p: -> LMI_STATS_IND\n", DRV_NAME, sl));
		rtn = slp_stats_ind(sl, q, mp);
		break;
	case LMI_EVENT_IND:
		printd(("%s: %p: -> LMI_EVENT_IND\n", DRV_NAME, sl));
		rtn = slp_event_ind(sl, q, mp);
		break;
	case SL_PDU_IND:
		printd(("%s: %p: -> SL_PDU_IND\n", DRV_NAME, sl));
		rtn = slp_pdu_ind(sl, q, mp);
		break;
	case SL_LINK_CONGESTED_IND:
		printd(("%s: %p: -> SL_LINK_CONGESTED_IND\n", DRV_NAME, sl));
		rtn = slp_link_congested_ind(sl, q, mp);
		break;
	case SL_LINK_CONGESTION_CEASED_IND:
		printd(("%s: %p: -> SL_LINK_CONGESTION_CEASED_IND\n", DRV_NAME, sl));
		rtn = slp_congestion_ceased_ind(sl, q, mp);
		break;
	case SL_RETRIEVED_MESSAGE_IND:
		printd(("%s: %p: -> SL_RETRIEVED_MESSAGE_IND\n", DRV_NAME, sl));
		rtn = slp_retrieved_message_ind(sl, q, mp);
		break;
	case SL_RETRIEVAL_COMPLETE_IND:
		printd(("%s: %p: -> SL_RETRIEVAL_COMPLETE_IND\n", DRV_NAME, sl));
		rtn = slp_retrieval_complete_ind(sl, q, mp);
		break;
	case SL_RB_CLEARED_IND:
		printd(("%s: %p: -> SL_RB_CLEARED_IND\n", DRV_NAME, sl));
		rtn = slp_rb_cleared_ind(sl, q, mp);
		break;
	case SL_BSNT_IND:
		printd(("%s: %p: -> SL_BSNT_IND\n", DRV_NAME, sl));
		rtn = slp_bsnt_ind(sl, q, mp);
		break;
	case SL_IN_SERVICE_IND:
		printd(("%s: %p: -> SL_IN_SERVICE_IND\n", DRV_NAME, sl));
		rtn = slp_in_service_ind(sl, q, mp);
		break;
	case SL_OUT_OF_SERVICE_IND:
		printd(("%s: %p: -> SL_OUT_OF_SERVICE_IND\n", DRV_NAME, sl));
		rtn = slp_out_of_service_ind(sl, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		printd(("%s: %p: -> SL_REMOTE_PROCESSOR_OUTAGE_IND\n", DRV_NAME, sl));
		rtn = slp_remote_processor_outage_ind(sl, q, mp);
		break;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		printd(("%s: %p: -> SL_REMOTE_PROCESSOR_RECOVERED_IND\n", DRV_NAME, sl));
		rtn = slp_remote_processor_recovered_ind(sl, q, mp);
		break;
	case SL_RTB_CLEARED_IND:
		printd(("%s: %p: -> SL_RTB_CLEARED_IND\n", DRV_NAME, sl));
		rtn = slp_rtb_cleared_ind(sl, q, mp);
		break;
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		printd(("%s: %p: -> SL_RETRIEVAL_NOT_POSSIBLE_IND\n", DRV_NAME, sl));
		rtn = slp_retrieval_not_possible_ind(sl, q, mp);
		break;
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		printd(("%s: %p: -> SL_BSNT_NOT_RETRIEVABLE_IND\n", DRV_NAME, sl));
		rtn = slp_bsnt_not_retrievable_ind(sl, q, mp);
		break;
#if 0
	case SL_OPTMGMT_ACK:
		printd(("%s: %p: -> SL_OPTMGMT_ACK\n", DRV_NAME, sl));
		rtn = slp_optmgmt_ack(sl, q, mp);
		break;
#endif
#if 0
	case SL_NOTIFY_IND:
		printd(("%s: %p: -> SL_NOTIFY_IND\n", DRV_NAME, sl));
		rtn = slp_notify_ind(sl, q, mp);
		break;
#endif
	default:
		printd(("%s: %p: -> SL_????\n", DRV_NAME, sl));
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		sl->i_state = sl->i_oldstate;
	return (rtn);
}

/*
 *  Primitives from XP to M2UA.
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
	struct sl *sl = SL_PRIV(q);

	/* data from above */
	return slu_write(sl, q, mp);
}
STATIC INLINE int
slp_r_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	/* data from below */
	return slp_read(sl, q, mp);
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
slu_r_prim(queue_t *q, mblk_t *mp)
{
	return (QR_PASSFLOW);
}
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
		return m2ua_w_ioctl(q, mp);
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
slp_w_prim(queue_t *q, mblk_t *mp)
{
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
STATIC INLINE int
xp_w_prim(queue_t *q, mblk_t *mp)
{
	return (QR_PASSFLOW);
}

/**
 * m2ua_oput: - upper and lower multiplex output put procedure
 * @q: active queue (upper read queue, lower write queue)
 * @mp: message to put
 *
 * The upper read queue and lower write queue are queues that can only have
 * their put procedures called from within the multiplex.  These put procedures,
 * although defined must never be called.  Instead, always check for outbound
 * flow control with bcanputnext() and call putnext().  If bcanputnext() fails,
 * place messages back on the originating queue and set the ASF_WANTW flag in
 * the private structure.  When the outbound queue becomes backenabled,
 * m2ua_osrv() will run and explicitly qenable() all queues feeding the
 * backenabled queue that have the ASF_WANTW flag set in their private
 * structures.
 */
STATIC streamscall int
m2ua_oput(queue_t *q, mblk_t *mp)
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
	struct *slu = SL_PRIV(q);
	struct as *as;

	if ((as = slu->as.as) != NULL) {
		struct ap *ap;

		for (ap = as->ap.list; ap; ap = ap->u.next) {
			struct sl *slp;
			struct gp *sgp;

			for (slp = ap->p.as->sl.list; slp; slp = slp->as.next)
				if (sl_tst_flags(slp, ASF_WANTW)) {
					sl_clr_flags(slp, ASF_WANTW);
					qenable(slp->iq);
				}
			for (sgp = ap->p.as->gp.list; sgp; sgp = sgp->as.next) {
				struct xp *xp;

				if ((xp = sgp->spp.spp->xp)) {
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
	struct sl *slp = SL_PRIV(q);
	struct as *as;

	if ((as = slp->as.as) != NULL) {
		struct ap *ap;

		for (ap = as->ap.list; ap; ap = ap->p.next) {
			struct sl *slu;
			struct gp *asp;

			for (slu = ap->u.as->sl.list; slu; slu = slu->as.next)
				if (sl_tst_flags(slu, ASF_WANTW)) {
					sl_clr_flags(slu, ASF_WANTW);
					qenable(slu->iq);
				}
			for (asp = ap->u.as->gp.list; asp; asp = asp->as.next) {
				struct xp *xp;

				if ((xp = asp->spp.spp->xp)) {
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
				struct sl *slp;
				struct gp *sgp;

				for (slp = ap->p.as->sl.list; slp; slp = slp->as.next)
					if (sl_tst_flags(slp, ASF_WANTW)) {
						sl_clr_flags(slp, ASF_WANTW);
						qenable(slp->iq);
					}
				for (sgp = ap->p.as->gp.list; sgp; sgp = sgp->as.next) {
					struct xp *xp;

					if ((xp = sgp->spp.spp->xp))
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
				struct sl *slu;
				struct gp *asp;

				for (slu = ap->u.as->sl.list; slu; slu = slu->as.next)
					if (sl_tst_flags(slu, ASF_WANTW)) {
						sl_clr_flags(slu, ASF_WANTW);
						qenable(slu->iq);
					}
				for (asp = ap->u.as->gp.list; asp; asp = asp->as.next) {
					struct xp *xp;

					if ((xp = asp->spp.spp->xp))
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
			struct sl *slp;
			struct gp *sgp;

			for (slp = ap->p.as->sl.list; slp; slp = slp->as.next)
				if (sl_tst_flags(slp, ASF_WANTW)) {
					sl_clr_flags(slp, ASF_WANTW);
					qenable(slp->iq);
				}
			for (sgp = ap->p.as->gp.list; sgp; sgp = sgp->as.next) {
				struct xp *xp;

				if ((xp = sgp->spp.spp->xp))
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

	if ((spp = xp->spp)) {
		switch (spp->type) {
		case M2UA_OBJ_TYPE_ASP:
			asp_o_wakeup(spp);
			break;
		case M2UA_OBJ_TYPE_SGP:
			sgp_o_wakeup(spp);
			break;
		case M2UA_OBJ_TYPE_SPP:
		default:
			swerr();
		}
	} else if ((sp = xp->sp)) {
		switch (sp->type) {
		case M2UA_OBJ_TYPE_SP:
			sp_o_wakeup(sp);
			break;
		case M2UA_OBJ_TYPE_SG:
		default:
			swerr();
		}
	} else
		swerr();
	return;
}
STATIC streamscall int
m2ua_osrv(queue_t *q)
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
STATIC major_t m2ua_majors[M2UA_CMAJORS] = { M2UA_CMAJOR_0, };

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
m2ua_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
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

	spin_lock_m2ua(&master.lock);
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
					    || !(cmajor = m2ua_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= M2UA_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_m2ua(&master.lock);
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(p = m2ua_alloc_priv(q, pp, devp, crp, bminor))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_m2ua(&master.lock);
		return (ENOMEM);
	}
	spin_unlock_m2ua(&master.lock);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
m2ua_close(queue_t *q, int flag, cred_t *crp)
{
	str_t *s = STR_PRIV(q);

	(void) s;
	printd(("%s: %p: closing character device %d:%d\n", DRV_NAME, s, s->u.dev.cmajor,
		s->u.dev.cminor));
	spin_lock_m2ua(&master.lock);
	{
		m2ua_free_priv(q);
	}
	spin_unlock_m2ua(&master.lock);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cachep_t m2ua_priv_cachep = NULL;
STATIC kmem_cachep_t m2ua_link_cachep = NULL;
STATIC kmem_cachep_t m2ua_as_cachep = NULL;
STATIC kmem_cachep_t m2ua_ap_cachep = NULL;
STATIC kmem_cachep_t m2ua_spp_cachep = NULL;
STATIC kmem_cachep_t m2ua_sp_cachep = NULL;
STATIC kmem_cachep_t m2ua_np_cachep = NULL;
STATIC kmem_cachep_t m2ua_gp_cachep = NULL;

STATIC int
m2ua_term_caches(void)
{
	int err = 0;

	if (m2ua_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(m2ua_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy m2ua_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed m2ua_priv_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(m2ua_priv_cachep);
#endif
	}
	if (m2ua_link_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(m2ua_link_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy m2ua_link_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed m2ua_link_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(m2ua_link_cachep);
#endif
	}
	if (m2ua_as_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(m2ua_as_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy m2ua_as_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed m2ua_as_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(m2ua_as_cachep);
#endif
	}
	if (m2ua_ap_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(m2ua_ap_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy m2ua_ap_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed m2ua_ap_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(m2ua_ap_cachep);
#endif
	}
	if (m2ua_gp_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(m2ua_gp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy m2ua_gp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed m2ua_gp_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(m2ua_gp_cachep);
#endif
	}
	if (m2ua_sp_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(m2ua_sp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy m2ua_sp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed m2ua_sp_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(m2ua_sp_cachep);
#endif
	}
	if (m2ua_np_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(m2ua_np_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy m2ua_np_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed m2ua_np_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(m2ua_np_cachep);
#endif
	}
	if (m2ua_spp_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(m2ua_spp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy m2ua_spp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed m2ua_spp_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(m2ua_spp_cachep);
#endif
	}
	return (err);
}
static int
m2ua_init_caches(void)
{
	if (!m2ua_priv_cachep
	    && !(m2ua_priv_cachep = kmem_create_cache("m2ua_priv_cachep", sizeof(union priv), 0,
						      SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate m2ua_priv_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized m2ua link structure cache\n", DRV_NAME));
	if (!m2ua_link_cachep
	    && !(m2ua_link_cachep = kmem_create_cache("m2ua_link_cachep", sizeof(union link), 0,
						      SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate m2ua_link_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized m2ua link structure cache\n", DRV_NAME));
	if (!m2ua_as_cachep
	    && !(m2ua_as_cachep = kmem_create_cache("m2ua_as_cachep", sizeof(struct as), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate m2ua_as_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized m2ua as structure cache\n", DRV_NAME));
	if (!m2ua_ap_cachep
	    && !(m2ua_ap_cachep = kmem_create_cache("m2ua_ap_cachep", sizeof(struct ap), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate m2ua_ap_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized m2ua ap structure cache\n", DRV_NAME));
	if (!m2ua_gp_cachep
	    && !(m2ua_gp_cachep = kmem_create_cache("m2ua_gp_cachep", sizeof(struct gp), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate m2ua_gp_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized m2ua gp structure cache\n", DRV_NAME));
	if (!m2ua_sp_cachep
	    && !(m2ua_sp_cachep = kmem_create_cache("m2ua_sp_cachep", sizeof(struct sp), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate m2ua_sp_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized m2ua sp structure cache\n", DRV_NAME));
	if (!m2ua_np_cachep
	    && !(m2ua_np_cachep = kmem_create_cache("m2ua_np_cachep", sizeof(struct np), 0,
						    SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate m2ua_np_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized m2ua np structure cache\n", DRV_NAME));
	if (!m2ua_spp_cachep
	    && !(m2ua_spp_cachep = kmem_create_cache("m2ua_spp_cachep", sizeof(struct spp), 0,
						     SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate m2ua_spp_cachep", DRV_NAME);
		goto enomem;
	}
	printd(("%s: initialized m2ua spp structure cache\n", DRV_NAME));
	return (0);
      enomem:
	m2ua_term_caches();
	return (-ENOMEM);
}

/*
 *  AP - AS-U/AS-P Graph Node
 *  -----------------------------------
 *  AS-U/AS-P graph node allocation, deallocation.
 */
STATIC struct ap *
m2ua_alloc_ap(struct as *as_u, struct as *as_p)
{
	struct ap *ap;

	printd(("%s: %s: ap graph as %ld:%ld\n", DRV_NAME, __FUNCTION__, as_u->id, as_p->id));
	if ((ap = kmem_cache_alloc(m2ua_ap_cachep, GFP_ATOMIC))) {
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
m2ua_free_ap(struct ap *ap)
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
		kmem_cache_free(m2ua_ap_cachep, ap);
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
m2ua_alloc_np(struct sp *sp, struct sp *sg)
{
	struct np *np;

	printd(("%s: %s: np graph sp %ld:%ld\n", DRV_NAME, __FUNCTION__, sp->id, sg->id));
	if ((np = kmem_cache_alloc(m2ua_np_cachep, GFP_ATOMIC))) {
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
m2ua_free_np(struct np *np)
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
		kmem_cache_free(m2ua_np_cachep, np);
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
m2ua_alloc_gp(int iid, struct as *as, struct spp *spp)
{
	struct gp *gp;

	printd(("%s: %s: gp graph as %ld spp %lu\n", DRV_NAME, __FUNCTION__, as->id, spp->id));
	if ((gp = kmem_cache_alloc(m2ua_gp_cachep, GFP_ATOMIC))) {
		bzero(gp, sizeof(gp));
		gp->iid = iid;
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
m2ua_free_gp(struct gp *gp)
{
	if (gp) {
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
		kmem_cache_free(m2ua_gp_cachep, gp);
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
m2ua_alloc_priv(queue_t *q, union priv **pp, dev_t *devp, cred_t *crp, minor_t bminor)
{
	union priv *p;

	printd(("%s: %s: create priv dev = %d:%d\n", DRV_NAME, __FUNCTION__, getmajor(*devp),
		getminor(*devp)));
	if ((p = kmem_cache_alloc(m2ua_priv_cachep, GFP_ATOMIC))) {
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
		case 0:	/* M2UA_OBJ_TYPE_SL_U */
			s->type = M2UA_OBJ_TYPE_SL_U;
			break;
		case 1:	/* M2UA_OBJ_TYPE_LM */
			s->type = M2UA_OBJ_TYPE_LM;
			master.lm = &p->lm;
			break;
		}
		s->o_prim = slu_r_prim;
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
m2ua_free_priv(queue_t *q)
{
	union priv *p = PRIV(q);
	struct str *s = &p->str;

	ensure(p, return);
	printd(("%s: %s: %p: free priv %d:%d\n", DRV_NAME, __FUNCTION__, s, s->u.dev.cmajor,
		s->u.dev.cminor));
	spin_lock_m2ua(&s->lock);
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
	spin_unlock_m2ua(&s->lock);
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
			kmem_cache_free(m2ua_priv_cachep, priv);
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
m2ua_alloc_as(int id, int type, struct sp *sp, uint32_t iid, m2ua_addr_t * add)
{
	struct as *as, **p;

	printd(("%s: %s: create as->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((as = kmem_cache_alloc(m2ua_as_cachep, GFP_ATOMIC))) {
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
			if (!m2ua_alloc_gp(iid, as, spp))
				goto enomem;
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
							if (!m2ua_alloc_ap(as, a2))
								goto enomem;
			if (sp == np->p.sp)
				for (; np; np = np->p.next)
					for (a2 = np->u.sp->as.list; a2; a2 = a2->sp.next)
						if ((!iid && a2->iid == iid)
						    || (add->spid == a2->add.spid
							&& add->sdti == a2->add.sdti
							&& add->sdli == a2->add.sdli))
							if (!m2ua_alloc_ap(a2, as))
								goto enomem;
		}
		/* as structures are created without any linked sl structures */
	} else
		printd(("%s: %s: ERROR: failed to allocate as structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (as);
      enomem:
	m2ua_free_as(as);
	return (NULL);
}
STATIC void
m2ua_free_as(struct as *as)
{

	ensure(as, return);
	printd(("%s: %s: %p free as->id = %ld\n", DRV_NAME, __FUNCTION__, as, as->id));
	spin_lock_m2ua(&as->lock);
	{
		struct sl *sl;
		struct ap *ap;
		struct gp *gp;

		/* unlink from sl */
		while ((sl = as->sl.list)) {
			fixme(("Disable and hangup sl\n"));
			m2ua_free_sl(sl);
		}
		/* unlink from other as */
		while ((ap = as->ap.list))
			m2ua_free_ap(ap);
		/* unlink from spp */
		while ((gp = as->gp.list))
			m2ua_free_gp(gp);
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
	spin_unlock_m2ua(&as->lock);
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
			kmem_cache_free(m2ua_as_cachep, as);
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
m2ua_alloc_sp(int id, int type, struct sp *osp, int cost, int tmode)
{
	struct sp *sp, **p;

	printd(("%s: %s: create sp->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((sp = kmem_cache_alloc(m2ua_sp_cachep, GFP_ATOMIC))) {
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
			if (!m2ua_alloc_np(sp, osp))
				goto enomem;
		}
		/* when sp are created they have no as and no spp */
	} else
		printd(("%s: %s: ERROR: failed to allocate sp structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (sp);
      enomem:
	m2ua_free_sp(sp);
	return (NULL);
}
STATIC void
m2ua_free_sp(struct sp *sp)
{

	ensure(sp, return);
	printd(("%s: %s: %p free sp->id = %ld\n", DRV_NAME, __FUNCTION__, sp, sp->id));
	spin_lock_m2ua(&sp->lock);
	{
		/* unlink from as */
		while (sp->as.list)
			m2ua_free_as(sp->as.list);
		/* unlink from spp */
		while (sp->spp.list)
			m2ua_free_spp(sp->spp.list);
		/* unlink from other sp */
		while (sp->np.list)
			m2ua_free_np(sp->np.list);
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
	spin_unlock_m2ua(&sp->lock);
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
			kmem_cache_free(m2ua_sp_cachep, sp);
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
m2ua_alloc_spp(int id, int type, struct sp *sp, int aspid, int cost)
{
	struct spp *spp, **p;

	printd(("%s: %s: create spp->id = %ld\n", DRV_NAME, __FUNCTION__, id));
	if ((spp = kmem_cache_alloc(m2ua_spp_cachep, GFP_ATOMIC))) {
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
			if (!m2ua_alloc_gp(as->iid, as, spp))
				goto enomem;
		/* spp structures are created without any linked xp structures */
	} else
		printd(("%s: %s: ERROR: failed to allocate spp structure %lu\n", DRV_NAME,
			__FUNCTION__, id));
	return (spp);
      enomem:
	m2ua_free_spp(spp);
	return (NULL);
}
STATIC void
m2ua_free_spp(struct spp *spp)
{

	ensure(spp, return);
	printd(("%s: %s: %p free spp->id = %ld\n", DRV_NAME, __FUNCTION__, spp, spp->id));
	spin_lock_m2ua(&spp->lock);
	{
		struct xp *xp;
		struct gp *gp;

		/* unlink from xp */
		if ((xp = spp->xp)) {
			fixme(("Disable and hangup xp\n"));
			m2ua_free_xp(xp);
		}
		/* unlink from as */
		while ((gp = spp->gp.list))
			m2ua_free_gp(gp);
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
	spin_unlock_m2ua(&spp->lock);
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
			kmem_cache_free(m2ua_spp_cachep, spp);
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
 *  SL - Signalling Link
 *  -----------------------------------
 *  Signalling link allocation, deallocation and reference counting.
 */
STATIC void
m2ua_alloc_sl(struct sl *sl, int id, int type, struct as *as, int iid, m2ua_addr_t * add)
{
	sl->id = id;
	sl->type = type;
	sl->iid = iid;
	sl->add = *add;
	/* link to AS */
	if ((sl->as.next = as->sl.list))
		sl->as.prev = &as->sl.list;
	sl->as.as = as_get(as);
	as->sl.list = sl_get(sl);
	as->sl.numb++;
	sl->o_prim = &slp_w_prim;
	sl->i_prim = &slp_r_prim;
	/* link is characterized, it can now be enabled */
	enableok(sl->oq);
	enableok(sl->iq);
	return;
}
STATIC void
m2ua_free_sl(struct sl *sl)
{

	ensure(sl, return);
	printd(("%s: %s: %p free sl index = %lu\n", DRV_NAME, __FUNCTION__, sl, sl->u.mux.index));
	spin_lock_m2ua(&sl->lock);
	{
		noenable(sl->oq);
		noenable(sl->iq);
		/* remove from AS */
		if (sl->as.as) {
			fixme(("Notify any active AS that we have gone away.\n"));
			if ((*sl->as.prev = sl->as.next))
				sl->as.next->as.prev = sl->as.prev;
			sl->as.next = NULL;
			sl->as.prev = &sl->as.next;
			ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
			sl_put(sl);
			assure(sl->as.as->sl.numb > 0);
			sl->as.as->sl.numb--;
			as_put(xchg(&sl->as.as, NULL));
		}
		sl->id = 0;
		sl->type = 0;
		/* flushing buffers */
		ss7_unbufcall((str_t *) sl);
		flushq(sl->oq, FLUSHALL);
		flushq(sl->iq, FLUSHALL);
		/* remove from master list */
		if ((*sl->prev = sl->next))
			sl->next->prev = sl->prev;
		sl->next = NULL;
		sl->prev = &sl->next;
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(sl);
		assure(master.link.numb > 0);
		master.link.numb--;
		/* remove from queues */
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(xchg(&sl->oq->q_ptr, NULL));
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(xchg(&sl->iq->q_ptr, NULL));
		/* done, check final count */
		if (atomic_read(&sl->refcnt) != 1) {
			__printd(("%s; %s: %p: ERROR: sl lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, sl, atomic_read(&sl->refcnt)));
			atomic_set(&sl->refcnt, 1);
		}
	}
	spin_unlock_m2ua(&sl->lock);
	sl_put(sl);		/* final put */
}
STATIC struct sl *
sl_get(struct sl *sl)
{
	if (sl) {
		atomic_inc(&sl->refcnt);
		return (sl);
	}
	swerr();
	return (sl);
}
STATIC void
sl_put(struct sl *sl)
{
	if (sl) {
		assure(atomic_read(&sl->refcnt) > 1);
		if (atomic_dec_and_test(&sl->refcnt)) {
			kmem_cache_free(m2ua_link_cachep, sl);
			printd(("%s: %s: %p: deallocated sl structure\n", DRV_NAME,
				__FUNCTION__, sl));
		}
		return;
	}
}
STATIC int
sl_get_id(int id)
{
	static int identifier = 0;

	if (!id)
		id = ++identifier;
	return (id);
}
STATIC struct sl *
sl_lookup(int id)
{
	struct sl *sl;

	for (sl = (struct sl *) master.link.list; sl; sl = sl->next) {
		if (sl->type != M2UA_OBJ_TYPE_SL_U && sl->type != M2UA_OBJ_TYPE_SL_P)
			continue;
		if (sl->id != id)
			continue;
		break;
	}
	return (sl);
}

/*
 *  XP - Transport Provider
 *  -----------------------------------
 *  Transport provider allocation, deallocation and reference counting.
 */
STATIC void
m2ua_alloc_xp(struct xp *xp, int id, int type, struct spp *spp, struct sp *sp)
{
	xp->id = id;
	xp->type = type;
	if (spp) {
		/* link to SPP: this is done when we know to which ASP or SGP the stream
		   corresponds.  If the aspid of the corresponding SPP is zero, we will not require 
		   an ASPID on ASPUP. If the aspid is non-zero, any provided ASPID must match the
		   ASPID set. */
		spp->xp = xp_get(xp);
		xp->spp = spp_get(spp);
	}
	if (sp) {
		/* link to SP: this is done when we don't know the ASP or SGP to which the
		   transport stream corresponds, and we will wait for the ASPUP with an ASPID to
		   determine to which SGP the stream cooresponds. */
		xp->sp = sp_get(sp);
	}
	switch (xp->type) {
	case M2UA_OBJ_TYPE_XP_SCTP:
	case M2UA_OBJ_TYPE_XP_TCP:
	case M2UA_OBJ_TYPE_XP_SSCOP:
		xp->o_prim = &xp_w_prim;
		xp->i_prim = &xp_r_prim;
		break;
	}
	/* link is characterized, it can now be enabled */
	enableok(xp->iq);
	enableok(xp->oq);
}
STATIC void
m2ua_free_xp(struct xp *xp)
{

	ensure(xp, return);
	printd(("%s: %s: %p free xp index = %lu\n", DRV_NAME, __FUNCTION__, xp, xp->u.mux.index));
	spin_lock_m2ua(&xp->lock);
	{
		mblk_t *b_next, *bp;

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
		if (xp->spp) {
			fixme(("Check deactivation of all AS\n"));
			xp_put(xchg(&xp->spp->xp, NULL));
			spp_put(xchg(&xp->spp, NULL));
		}
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
	spin_unlock_m2ua(&xp->lock);
	xp_put(xp);		/* final put */
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
			kmem_cache_free(m2ua_link_cachep, xp);
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
		if (xp->type != M2UA_OBJ_TYPE_XP_SCTP && xp->type != M2UA_OBJ_TYPE_XP_TCP
		    && xp->type != M2UA_OBJ_TYPE_XP_SSCOP)
			continue;
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
m2ua_alloc_link(queue_t *q, union link **lkp, int index, cred_t *crp)
{
	union link *lk;

	printd(("%s: %s: create link index = %lu\n", DRV_NAME, __FUNCTION__, index));
	if ((lk = kmem_cache_alloc(m2ua_link_cachep, GFP_ATOMIC))) {
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
m2ua_free_link(queue_t *q)
{
	union link *lk = LINK(q);

	ensure(lk, return);
	switch (lk->str.type) {
	case M2UA_OBJ_TYPE_SL_U:
	case M2UA_OBJ_TYPE_SL_P:
		return m2ua_free_sl(&lk->sl);
	case M2UA_OBJ_TYPE_XP_SCTP:
	case M2UA_OBJ_TYPE_XP_TCP:
	case M2UA_OBJ_TYPE_XP_SSCOP:
		return m2ua_free_xp(&lk->xp);
	case 0:
		break;
	default:
		swerr();
		break;
	}
	/* untyped link not assigned to anything */
	printd(("%s: %s: %p free link index = %lu\n", DRV_NAME, __FUNCTION__, lk,
		lk->str.u.mux.index));
	spin_lock_m2ua(&lk->str.lock);
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
	spin_unlock_m2ua(&lk->str.lock);
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
			kmem_cache_free(m2ua_link_cachep, lk);
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
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the INET driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the INET driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw m2ua_cdev = {
	.d_name = DRV_NAME,
	.d_str = &m2uainfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
m2ua_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&m2ua_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
m2ua_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&m2ua_cdev, major)) < 0)
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
m2ua_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &m2uainfo, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

STATIC int
m2ua_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
m2uaterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (m2ua_majors[mindex]) {
			if ((err = m2ua_unregister_strdev(m2ua_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					m2ua_majors[mindex]);
			if (mindex)
				m2ua_majors[mindex] = 0;
		}
	}
	if ((err = m2ua_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
m2uainit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = m2ua_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		m2uaterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = m2ua_register_strdev(m2ua_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					m2ua_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				m2uaterminate();
				return (err);
			}
		}
		if (m2ua_majors[mindex] == 0)
			m2ua_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(m2ua_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = m2ua_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(m2uainit);
module_exit(m2uaterminate);

#endif				/* LINUX */
