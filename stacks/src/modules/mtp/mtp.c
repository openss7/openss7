/*****************************************************************************

 @(#) $RCSfile: mtp.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/08/31 07:19:49 $

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

 Last Modified $Date: 2004/08/31 07:19:49 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: mtp.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/08/31 07:19:49 $"

static char const ident[] =
    "$RCSfile: mtp.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/08/31 07:19:49 $";

/*
 *  This an MTP (Message Transfer Part) multiplexing driver which can have SL
 *  (Signalling Link) streams I_LINK'ed or I_PLINK'ed underneath it to form a
 *  complete Message Transfer Part protocol layer for SS7.
 */
#include "compat.h"

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <sys/npi.h>
#include <sys/npi_ss7.h>
#include <sys/npi_mtp.h>
#include <sys/tihdr.h>
// #include <sys/tpi_ss7.h>
// #include <sys/tpi_mtp.h>
#include <sys/xti.h>
#include <sys/xti_mtp.h>

#define MTP_DESCRIP	"SS7 MESSAGE TRANSFER PART (MTP) STREAMS MULTIPLEXING DRIVER."
#define MTP_REVISION	"LfS $RCSfile: mtp.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/08/31 07:19:49 $"
#define MTP_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define MTP_DEVICE	"Part of the OpenSS7 Stack for LiS STREAMS."
#define MTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MTP_LICENSE	"GPL"
#define MTP_BANNER	MTP_DESCRIP	"\n" \
			MTP_REVISION	"\n" \
			MTP_COPYRIGHT	"\n" \
			MTP_DEVICE	"\n" \
			MTP_CONTACT
#define MTP_SPLASH	MTP_DESCRIP	"\n" \
			MTP_REVISION

#ifdef LINUX
MODULE_AUTHOR(MTP_CONTACT);
MODULE_DESCRIPTION(MTP_DESCRIP);
MODULE_SUPPORTED_DEVICE(MTP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MTP_LICENSE);
#endif				/* MODULE_LICENSE */
#endif				/* LINUX */

#ifdef LFS
#define MTP_DRV_ID		CONFIG_STREAMS_MTP_MODID
#define MTP_DRV_NAME		CONFIG_STREAMS_MTP_NAME
#define MTP_CMAJORS		CONFIG_STREAMS_MTP_NMAJORS
#define MTP_CMAJOR_0		CONFIG_STREAMS_MTP_MAJOR
#define MTP_UNITS		CONFIG_STREAMS_MTP_NMINORS
#endif

#define MTP_CMINOR_MTPI	0
#define MTP_CMINOR_TPI	1
#define MTP_CMINOR_NPI	2
#define MTP_CMINOR_MGMT	3
#define MTP_CMINOR_FREE	4

#define MTP_STYLE_MTPI	MTP_CMINOR_MTPI
#define MTP_STYLE_TPI	MTP_CMINOR_TPI
#define MTP_STYLE_NPI	MTP_CMINOR_NPI
#define MTP_STYLE_MGMT	MTP_CMINOR_MGMT

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		    MTP_DRV_ID
#define DRV_NAME	    MTP_DRV_NAME
#define CMAJORS		    MTP_CMAJORS
#define CMAJOR_0	    MTP_CMAJOR_0
#define UNITS		    MTP_UNITS
#ifdef MODULE
#define DRV_BANNER	    MTP_BANNER
#else				/* MODULE */
#define DRV_BANNER	    MTP_SPLASH
#endif				/* MODULE */

STATIC struct module_info mtp_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-wr",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};
STATIC struct module_info mtp_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-rd",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};
STATIC struct module_info sl_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-muxw",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};
STATIC struct module_info sl_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME "-muxr",	/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:272 + 1,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};

STATIC int mtp_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int mtp_close(queue_t *, int, cred_t *);

STATIC struct qinit mtp_rinit = {
	qi_putp:ss7_oput,		/* Write put (message from below) */
	qi_srvp:ss7_osrv,		/* Write queue service */
	qi_qopen:mtp_open,		/* Each open */
	qi_qclose:mtp_close,		/* Last close */
	qi_minfo:&mtp_rinfo,		/* Information */
};

STATIC struct qinit mtp_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&mtp_winfo,		/* Information */
};

STATIC struct qinit sl_rinit = {
	qi_putp:ss7_iput,		/* Write put (message from below) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&sl_rinfo,		/* Information */
};

STATIC struct qinit sl_winit = {
	qi_putp:ss7_oput,		/* Write put (message from above) */
	qi_srvp:ss7_osrv,		/* Write queue service */
	qi_minfo:&sl_winfo,		/* Information */
};

MODULE_STATIC struct streamtab mtpinfo = {
	st_rdinit:&mtp_rinit,		/* Upper read queue */
	st_wrinit:&mtp_winit,		/* Upper write queue */
	st_muxrinit:&sl_rinit,		/* Lower read queue */
	st_muxwinit:&sl_winit,		/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private structures
 *
 *  =========================================================================
 */

/*
   structure linkage 
 */
#define SLIST_COUNTERS(__s1,__s2) \
	struct { \
		size_t numb;		/* numb of __s1 */ \
		struct __s1 *list;	/* list of __s1 */ \
		struct __s1 *curr;	/* current __s1 */ \
		size_t sls_mask;	/* mask for selecting __s1 */ \
		size_t sls_bits;	/* numb of significant bits (1's) in the sls mask */ \
		size_t allowed;		/* numb of __s1 allowed */ \
		size_t danger;		/* numb of __s1 in danger */ \
		size_t congested;	/* numb of __s1 congested */ \
		size_t restricted;	/* numb of __s1 restricted */ \
		size_t prohibited;	/* numb of __s1 prohibited */ \
		size_t inhibited;	/* numb of __s1 inhibited */ \
		size_t blocked;		/* numb of __s1 blocked */ \
		size_t inactive;	/* numb of __s1 inactive */ \
		size_t restart;		/* numb of __s1 restart */ \
	} __s2; \

/*
   generic counters structure 
 */
typedef struct counters {
	SLIST_COUNTERS (head, c);	/* counter declaration */
} counters_t;

/*
   structure map 
 */
#define SLIST_SMAP(__s1, __s2, __n) \
	union { \
		struct __s1 *__s1;	/* pointer to normal type */ \
		struct __s2 *__s2;	/* pointer to buffer type */ \
		struct head *h;		/* pointer to generic type */ \
	} smap[__n]; \

/*
   forward declarations 
 */
struct df;				/* Default structure */
struct mtp;				/* MTP User */
struct na;				/* Network Appearance */
struct sp;				/* Signalling Point */
struct rr;				/* Route Restriction structure */
struct rs;				/* Route Set */
struct cr;				/* Controlled Rerouting Buffer */
struct rl;				/* Route List */
struct rt;				/* Route */
struct cb;				/* Changeback Buffer */
struct ls;				/* (Combined) Link Set */
struct lk;				/* Link (Set) */
struct sl;				/* Signalling Link */

/*
   default 
 */
typedef struct df {
	spinlock_t lock;		/* master list lock */
	SLIST_HEAD (mtp, mtp);		/* list of MTP structures */
	SLIST_COUNTERS (na, na);	/* list of NA structures */
	SLIST_HEAD (sp, sp);		/* list of SP structures */
	SLIST_HEAD (rs, rs);		/* list of RS structures */
	SLIST_HEAD (rl, rl);		/* list of RL structures */
	SLIST_HEAD (rt, rt);		/* list of RT structures */
	SLIST_HEAD (ls, ls);		/* list of LS structures */
	SLIST_HEAD (lk, lk);		/* list of LK structures */
	SLIST_HEAD (sl, sl);		/* list of SL structures */
	SLIST_HEAD (cb, cb);		/* list of CB buffers */
	SLIST_HEAD (cr, cr);		/* list of CR buffers */
	struct mtp_timers_df timers;	/* default timers */
	struct mtp_opt_conf_df config;	/* default configuration */
	struct mtp_stats_df statsp;	/* default statistics periods */
	struct mtp_stats_df stats;	/* default statistics */
	struct mtp_notify_df notify;	/* default notificiations */
} df_t;

STATIC struct df master;

STATIC INLINE struct df *
df_lookup(ulong id)
{
	if (id)
		return (NULL);
	return (&master);
}

/*
 *  MT - MTP User
 *  -----------------------------------
 *  The MTP structure corresponds to an MTP User stream.  This stream is bound to a source address, and possibly a
 *  destination address if the SI is a connection-oriented SI (e.g. ISUP).  The source address defines a local
 *  signalling point and the destination address defines a routeset to the remote signalling point for pseudo-
 *  connection-oriented service indicators.
 */
typedef struct mtp {
	STR_DECLARATION (struct mtp);	/* stream declaration */
	struct mtp_addr src;		/* srce address */
	struct mtp_addr dst;		/* dest address */
	struct {
		struct sp *loc;		/* this mtp user for local signalling point */
		struct rs *rem;		/* this mtp user for remote signalling point */
		struct mtp *next;	/* next mtp user for local signalling point */
		struct mtp **prev;	/* prev mtp user for local signalling point */
	} sp;
	struct {
		t_uscalar_t sls;	/* default options */
		t_uscalar_t mp;		/* default options */
		t_uscalar_t debug;	/* default options */
	} options;
	struct T_info_ack *prot;	/* Protocol parameters */
} mtp_t;
#define MTP_PRIV(__q) ((struct mtp *)(__q)->q_ptr)

STATIC struct mtp *mtp_alloc_priv(queue_t *, struct mtp **, dev_t *, cred_t *, minor_t);
STATIC void mtp_free_priv(struct mtp *);
STATIC struct mtp *mtp_lookup(ulong);
STATIC ulong mtp_get_id(ulong);
STATIC struct mtp *mtp_get(struct mtp *);
STATIC void mtp_put(struct mtp *);

/*
 *  NA - Network Apperance
 *  -----------------------------------
 *  The NA struct defines a network appearance.  A network appearance is a combination of protocol variant, protocol
 *  options and point code format.  The network appearance identifies a point code numbering space and defines the
 *  service indicators within that numbering space.  Signalling points belong to a single network apperance.
 */
typedef struct na {
	HEAD_DECLARATION (struct na);	/* head declaration */
	struct {
		uint32_t member;	/* pc member mask */
		uint32_t cluster;	/* pc cluster mask */
		uint32_t network;	/* pc network mask */
	} mask;
	SLIST_COUNTERS (sp, sp);	/* signalling points in this na */
	struct lmi_option option;	/* protocol variant and options */
	struct T_info_ack *prot[16];	/* protoocl profiles for si values */
	struct mtp_timers_na timers;	/* network appearance timers */
	struct mtp_opt_conf_na config;	/* network appearance configuration */
	struct mtp_stats_na statsp;	/* network appearance statistics periods */
	struct mtp_stats_na stats;	/* network appearance statistics */
	struct mtp_notify_na notify;	/* network appearance notifications */
} na_t;

STATIC struct na *mtp_alloc_na(ulong, uint32_t, uint32_t, uint32_t, uint, struct lmi_option *);
STATIC void mtp_free_na(struct na *);
STATIC struct na *na_lookup(ulong);
STATIC ulong na_get_id(ulong);
STATIC struct na *na_get(struct na *);
STATIC void na_put(struct na *);

/*
 *  RR - Routeset restriction
 *  -----------------------------------
 *  The routeset restriction (RR) structure...
 */
typedef struct rr {
	struct lk *lk;			/* link (set) that this route restriction corresponds to */
	SLIST_LINKAGE (rs, rr, rs);	/* route restriction list linkage */
} rr_t;

STATIC struct rr *mtp_alloc_rr(struct rs *, struct lk *);
STATIC void mtp_free_rr(struct rr *);

/*
 *  RS - Routeset
 *  -----------------------------------
 *  The routeset structure defines the SS7 routeset to a remote signalling point within the network appearance and
 *  belonging to a signalling point.
 */
typedef struct rs {
	HEAD_DECLARATION (struct rs);	/* head declaration */
	int rs_type;			/* type of routeset (cluster, member) */
	int cong_status;		/* congestion status */
	int disc_status;		/* discard status */
	uint32_t dest;			/* remote signalling point/cluster */
	size_t cong_msg_count;		/* msg counts under congestion */
	size_t cong_oct_count;		/* msg octets under congestion */
	SLIST_LINKAGE (sp, rs, sp);	/* signalling point list linkage */
	SLIST_COUNTERS (rl, rl);	/* list and counts of route lists */
	SLIST_HEAD (rr, rr);		/* routeset restriction list */
	struct mtp_timers_rs timers;	/* routeset timers */
	struct mtp_opt_conf_rs config;	/* routeset configuration */
	struct mtp_stats_rs statsp;	/* routeset statistics periods */
	struct mtp_stats_rs stats;	/* routeset statistics */
	struct mtp_notify_rs notify;	/* routeset notifications */
} rs_t;

STATIC struct rs *mtp_alloc_rs(ulong, struct sp *, ulong, ulong);
STATIC void mtp_free_rs(struct rs *);
STATIC struct rs *rs_lookup(ulong);
STATIC ulong rs_get_id(ulong);
STATIC struct rs *rs_get(struct rs *);
STATIC void rs_put(struct rs *);

/*
 *  CR - Controlled rerouting buffer
 *  -----------------------------------
 *  The controlled rerouting buffer structure is used to hold MSUs during the controlled rerouting procedure between
 *  a equal priority alternate route (from) and an normal route (onto).  MSUs are accumulated in the buffer until
 *  controlled rerouting is cancelled or the associated t6 timer expires and buffered signalling traffic is rerouted
 *  to the normal route.
 */
typedef struct cr {
	HEAD_DECLARATION (struct cr);	/* head declaration */
	ulong index;			/* sls index in route sls map */
	struct {
		struct rt *from;	/* route we are buffering from */
		struct rt *onto;	/* route we are buffering onto */
	} rt;
	SLIST_LINKAGE (rl, cr, rl);	/* route list list linkage */
	struct {
		ulong t6;		/* controlled rerouting timer */
	} timers;
	struct bufq buf;		/* message buffer */
} cr_t;

STATIC struct cr *mtp_alloc_cr(ulong, struct rl *, struct rt *, struct rt *, ulong);
STATIC void mtp_free_cr(struct cr *);
STATIC struct cr *cr_lookup(ulong);
STATIC ulong cr_get_id(ulong);
STATIC struct cr *cr_get(struct cr *);
STATIC void cr_put(struct cr *);

/*
 *  RL - Routelist
 *  -----------------------------------
 *  The route list structure contains a list of routes of identical cost.  A routelist represents a route via a
 *  combined link set.  Routelists belong to one routeset and contain several equal cost routes.  Cost is associated
 *  wtih a routelist (all contained routes have the same priority as the route list).  An SLS map is used to route
 *  messages to the appropriate route within the route list whenever the route list is actively carrying traffic.
 */
#define RT_SMAP_SIZE	32
typedef struct rl {
	HEAD_DECLARATION (struct rl);	/* head declaration */
	ulong cost;			/* priority of this route list */
	SLIST_LINKAGE (rs, rl, rs);	/* route list linkage */
	SLIST_LINKAGE (ls, rl, ls);	/* linkset list linkage */
	SLIST_COUNTERS (rt, rt);	/* list of routes and counts */
	SLIST_HEAD (cr, cr);		/* list of controlled rerouting buffers */
	SLIST_SMAP (rt, cr, RT_SMAP_SIZE);	/* routelist sls map */
	struct mtp_timers_rl timers;	/* routelist timers */
	struct mtp_opt_conf_rl config;	/* routelist configuration */
	struct mtp_stats_rl statsp;	/* routelist statistics periods */
	struct mtp_stats_rl stats;	/* routelist statistics */
	struct mtp_notify_rl notify;	/* routelist notifications */
} rl_t;

STATIC struct rl *mtp_alloc_rl(ulong, struct rs *, struct ls *, ulong);
STATIC void mtp_free_rl(struct rl *);
STATIC struct rl *rl_lookup(ulong);
STATIC ulong rl_get_id(ulong);
STATIC struct rl *rl_get(struct rl *);
STATIC void rl_put(struct rl *);

/*
 *  RT - Route
 *  -----------------------------------
 *  The route structure represents a route over a link (set) to an adjacent signalling point.  Routes belong to
 *  route lists and are associated with a link (LK) structure.  Each route occupies a slot within the parent
 *  routelist SLS map and has a load factor indicating the loading of this route within the route list.  Load
 *  factors are used to determine danger of congestion for ANSI transfer restricted procedures as well as when
 *  rebalancing load due to a destination which has become inaccessible or accessible.
 */
typedef struct rt {
	HEAD_DECLARATION (struct rt);	/* head declaration */
	uint load;			/* load in this route list */
	uint slot;			/* slot in this route list */
	SLIST_LINKAGE (rl, rt, rl);	/* route list list linkage */
	SLIST_LINKAGE (lk, rt, lk);	/* link list linkage */
	struct mtp_timers_rt timers;	/* route timers */
	struct mtp_opt_conf_rt config;	/* route configuration */
	struct mtp_stats_rt statsp;	/* route statistics periods */
	struct mtp_stats_rt stats;	/* route statistics */
	struct mtp_notify_rt notify;	/* route notifications */
} rt_t;

STATIC struct rt *mtp_alloc_rt(ulong, struct rl *, struct lk *, ulong);
STATIC void mtp_free_rt(struct rt *);
STATIC struct rt *rt_lookup(ulong);
STATIC ulong rt_get_id(ulong);
STATIC struct rt *rt_get(struct rt *);
STATIC void rt_put(struct rt *);

/*
 *  SP - Signalling point
 *  -----------------------------------
 *  The signalling point structure represents a local (real, virtual or alias) signalling point within a network
 *  appearance.
 */
typedef struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	uint32_t ni;			/* network indicator */
	uint32_t pc;			/* point code */
	uint sls;			/* sls for loadsharing of management messages */
	SLIST_LINKAGE (na, sp, na);	/* network appearance linkage */
	struct {
		ushort equipped;	/* bit mask of equipped users */
		ushort available;	/* bit mask of available users */
		struct mtp *lists[16];	/* lists of MTP users */
	} mtp;
	SLIST_COUNTERS (ls, ls);	/* list and counts of link sets */
	SLIST_COUNTERS (rs, rs);	/* list and counts of route sets */
	struct mtp_timers_sp timers;	/* signalling point timers */
	struct mtp_opt_conf_sp config;	/* signalling point configuration */
	struct mtp_stats_sp statsp;	/* signalling point statistics periods */
	struct mtp_stats_sp stats;	/* signalling point statistics */
	struct mtp_notify_sp notify;	/* signalling point notifications */
} sp_t;

STATIC struct sp *mtp_alloc_sp(ulong, struct na *, ulong, ulong, ulong);
STATIC void mtp_free_sp(struct sp *);
STATIC struct sp *sp_lookup(ulong);
STATIC ulong sp_get_id(ulong);
STATIC struct sp *sp_get(struct sp *);
STATIC void sp_put(struct sp *);

/*
 *  CB - Changeback buffer
 *  -----------------------------------
 *  The changeback buffer structure is used for the MTP changeover and changeback procedures, where MSUs are
 *  buffered and trasferred from one signalling link in the (combined) link set (from) to another signalling link
 *  the combined link set (onto).
 */
typedef struct cb {
	HEAD_DECLARATION (struct cb);	/* head declaration */
	ulong cbc;			/* changeback code */
	ulong slc;			/* signalling link code */
	ulong index;			/* sls index in signaling link sls map */
	struct {
		struct sl *from;	/* signalling link we are buffering from */
		struct sl *onto;	/* signalling link we are buffering onto */
	} sl;
	SLIST_LINKAGE (lk, cb, lk);	/* link list linkage */
	struct {
		ulong t1;		/* changeover timer */
		ulong t2;		/* changeover ack timer */
		ulong t3;		/* changeback timer */
		ulong t4;		/* changeback ack timer */
		ulong t5;		/* changeback ack second attempt timer */
	} timers;
	struct bufq buf;		/* message buffer */
} cb_t;

STATIC struct cb *mtp_alloc_cb(ulong, struct lk *, struct sl *, struct sl *, ulong);
STATIC void mtp_free_cb(struct cb *);
STATIC struct cb *cb_lookup(ulong);
STATIC ulong cb_get_id(ulong);
STATIC struct cb *cb_get(struct cb *);
STATIC void cb_put(struct cb *);

/*
 *  LS - (Combined) Link Set
 *  -----------------------------------
 *  The link set structure represents a combined link set.  Each link set belongs to a local signalling point and
 *  contains a number of link (LK) structures.  An SLS map is used to select a link within the link set.
 */
typedef struct ls {
	HEAD_DECLARATION (struct ls);	/* head declaration */
	ulong sls;			/* sls for mgmt loadsharing */
	SLIST_LINKAGE (sp, ls, sp);	/* signalling point list linkage */
	SLIST_COUNTERS (rl, rl);	/* list and counts of route lists */
	SLIST_COUNTERS (lk, lk);	/* list and counts of links */
	struct mtp_timers_ls timers;	/* link set timers */
	struct mtp_opt_conf_ls config;	/* link set configuration */
	struct mtp_stats_ls statsp;	/* link set statistics periods */
	struct mtp_stats_ls stats;	/* link set statistics */
	struct mtp_notify_ls notify;	/* link set notifications */
} ls_t;

STATIC struct ls *mtp_alloc_ls(ulong, struct sp *, ulong);
STATIC void mtp_free_ls(struct ls *);
STATIC struct ls *ls_lookup(ulong);
STATIC ulong ls_get_id(ulong);
STATIC struct ls *ls_get(struct ls *);
STATIC void ls_put(struct ls *);

/*
 *  LK - Link (set)
 *  -----------------------------------
 *  The link structure represents a link set.  Each link belongs to a combined link set and contains a number of
 *  signalling link (SL) structure.  An SLS map is ued to select a signalling link within the link.
 */
#define SL_SMAP_SIZE	256
typedef struct lk {
	HEAD_DECLARATION (struct lk);	/* head declaration */
	ulong ni;			/* network indicator for link */
	uint slot;			/* slot in this linkset */
	uint load;			/* load in this linkset */
	struct {
		struct sp *loc;		/* local signalling point for this link */
		struct rs *adj;		/* adjacent signalling point for this link */
	} sp;
	SLIST_LINKAGE (ls, lk, ls);	/* linkset list linkage */
	SLIST_COUNTERS (rt, rt);	/* list and counts of routes */
	SLIST_COUNTERS (sl, sl);	/* list and counts of signalling links */
	SLIST_HEAD (cb, cb);		/* list of changeback buffers */
	SLIST_SMAP (sl, cb, SL_SMAP_SIZE);	/* link sls map */
	struct mtp_timers_lk timers;	/* link timers */
	struct mtp_opt_conf_lk config;	/* link configuration */
	struct mtp_stats_lk statsp;	/* link statistics periods */
	struct mtp_stats_lk stats;	/* link statistics */
	struct mtp_notify_lk notify;	/* link notifications */
} lk_t;

STATIC struct lk *mtp_alloc_lk(ulong, struct ls *, struct sp *, struct rs *, ulong, ulong);
STATIC void mtp_free_lk(struct lk *);
STATIC struct lk *lk_lookup(ulong);
STATIC ulong lk_get_id(ulong);
STATIC struct lk *lk_get(struct lk *);
STATIC void lk_put(struct lk *);

/*
 *  SL - Signalling link
 *  -----------------------------------
 *  The signalling link structure represents a lower multiplexing driver stream associated with a Signalling Link
 *  (SL) driver.  Each signalling link has a unique signalling link code (SLC) within a link and has a Signalling
 *  Data Link Identifier identifying the signalling data link (channel).  Each signalling link has a load factor
 *  which indicates how many Signalling Link Selection (SLS) codes of traffic are assigned to this signalling link.
 */
typedef struct sl {
	STR_DECLARATION (struct sl);	/* stream declaration */
	long l_state;			/* signalling link state */
	uint load;			/* load in this link */
	uint slot;			/* slot in this link */
	SLIST_LINKAGE (lk, sl, lk);	/* link list linkage */
	ulong slc;			/* signalling link code */
	ulong sdli;			/* signalling data link identifier */
	int cong_status;		/* congestion status */
	int disc_status;		/* discard status */
	uint fsnc;			/* FSNC received remotely */
	uint bsnt;			/* BSNT retrieved locally */
	struct bufq rbuf;		/* retrieval buffer */
	uint reason;
	uchar tdata[16];		/* SL test data */
	uchar tlen;			/* SL test data length */
	struct mtp_timers_sl timers;	/* signalling link timers */
	struct mtp_opt_conf_sl config;	/* signalling link configuration */
	struct mtp_stats_sl statsp;	/* signalling link statistics periods */
	struct mtp_stats_sl stats;	/* signalling link statistics */
	struct mtp_notify_sl notify;	/* signalling link notifications */
} sl_t;
#define SL_PRIV(__q) ((struct sl *)(__q)->q_ptr)

STATIC struct sl *mtp_alloc_link(queue_t *, struct sl **, ulong, cred_t *);
STATIC void mtp_free_link(struct sl *);
STATIC void mtp_free_sl(struct sl *);
STATIC struct sl *sl_lookup(ulong);
STATIC ulong sl_get_id(ulong);
STATIC struct sl *sl_get(struct sl *);
STATIC void sl_put(struct sl *);

/*
 *  SL interface state flags
 */
#define SLSF_OUT_OF_SERVICE	(1<< SLS_OUT_OF_SERVICE)	/* out of service */
#define SLSF_PROC_OUTG		(1<< SLS_PROC_OUTG)	/* processor outage */
#define SLSF_IN_SERVICE		(1<< SLS_IN_SERVICE)	/* in service */
#define SLSF_WACK_COO		(1<< SLS_WACK_COO)	/* waiting COA/ECA in response to COO */
#define SLSF_WACK_ECO		(1<< SLS_WACK_ECO)	/* waiting COA/ECA in response to ECO */
#define SLSF_WCON_RET		(1<< SLS_WCON_RET)	/* waiting for retrieval confrimation */
#define SLSF_WIND_CLRB		(1<< SLS_WIND_CLRB)	/* waiting for clear buffers indication */
#define SLSF_WIND_BSNT		(1<< SLS_WIND_BSNT)	/* waiting for BSNT indication */
#define SLSF_WIND_INSI		(1<< SLS_WIND_INSI)	/* waiting for in service indication */
#define SLSF_WACK_SLTM		(1<< SLS_WACK_SLTM)	/* waiting SLTA in response to 1st SLTM */

/*
 *  TLI interface state flags
 */
#define TSF_UNBND	( 1 << TS_UNBND		)
#define TSF_WACK_BREQ	( 1 << TS_WACK_BREQ	)
#define TSF_WACK_UREQ	( 1 << TS_WACK_UREQ	)
#define TSF_IDLE	( 1 << TS_IDLE		)
#define TSF_WACK_OPTREQ	( 1 << TS_WACK_OPTREQ	)
#define TSF_WACK_CREQ	( 1 << TS_WACK_CREQ	)
#define TSF_WCON_CREQ	( 1 << TS_WCON_CREQ	)
#define TSF_WRES_CIND	( 1 << TS_WRES_CIND	)
#define TSF_WACK_CRES	( 1 << TS_WACK_CRES	)
#define TSF_DATA_XFER	( 1 << TS_DATA_XFER	)
#define TSF_WIND_ORDREL	( 1 << TS_WIND_ORDREL	)
#define TSF_WREQ_ORDREL	( 1 << TS_WREQ_ORDREL	)
#define TSF_WACK_DREQ6	( 1 << TS_WACK_DREQ6	)
#define TSF_WACK_DREQ7	( 1 << TS_WACK_DREQ7	)
#define TSF_WACK_DREQ9	( 1 << TS_WACK_DREQ9	)
#define TSF_WACK_DREQ10	( 1 << TS_WACK_DREQ10	)
#define TSF_WACK_DREQ11	( 1 << TS_WACK_DREQ11	)
#define TSF_NOSTATES	( 1 << TS_NOSTATES	)

#define TSF_WACK_DREQ	(TSF_WACK_DREQ6 \
			|TSF_WACK_DREQ7 \
			|TSF_WACK_DREQ9 \
			|TSF_WACK_DREQ10 \
			|TSF_WACK_DREQ11)

typedef struct mtp_opts {
	uint flags;			/* success flags */
	t_uscalar_t *sls;
	t_uscalar_t *mp;
	t_uscalar_t *debug;
} mtp_opts_t;

STATIC struct {
	t_uscalar_t sls;
	t_uscalar_t mp;
	t_uscalar_t debug;
} opt_defaults = {
0, 0, 0};

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */
#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))
STATIC size_t
mtp_opts_size(struct mtp *mtp, struct mtp_opts *ops)
{
	size_t len = 0;
	if (ops) {
		const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */
		if (ops->sls)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->sls));
		if (ops->mp)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->mp));
		if (ops->debug)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->debug));
	}
	return (len);
}
STATIC void
mtp_build_opts(struct mtp *mtp, struct mtp_opts *ops, unsigned char *p)
{
	if (ops) {
		struct t_opthdr *oh;
		const size_t hlen = sizeof(struct t_opthdr);
		if (ops->sls) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->sls));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_SLS;
			oh->status = (ops->flags & (1 << T_MTP_SLS)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->sls)) p) = *(ops->sls);
			p += _T_ALIGN_SIZEOF(*ops->sls);
		}
		if (ops->mp) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->mp));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_MP;
			oh->status = (ops->flags & (1 << T_MTP_MP)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->mp)) p) = *(ops->mp);
			p += _T_ALIGN_SIZEOF(*ops->mp);
		}
		if (ops->debug) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->debug));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_DEBUG;
			oh->status = (ops->flags & (1 << T_MTP_DEBUG)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->debug)) p) = *(ops->debug);
			p += _T_ALIGN_SIZEOF(*ops->debug);
		}
	}
}
STATIC int
mtp_parse_opts(struct mtp *mtp, struct mtp_opts *ops, unsigned char *op, size_t len)
{
	struct t_opthdr *oh;
	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
		switch (oh->level) {
		case T_SS7_MTP:
			switch (oh->name) {
			case T_MTP_SLS:
				ops->sls = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_MTP_SLS);
				continue;
			case T_MTP_MP:
				ops->mp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_MTP_MP);
				continue;
			case T_MTP_DEBUG:
				ops->debug = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_MTP_DEBUG);
				continue;
			}
		}
	}
	if (oh)
		return (TBADOPT);
	return (0);
}
STATIC int
mtp_parse_qos(struct mtp *mtp, struct mtp_opts *ops, unsigned char *op, size_t len)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  OPTIONS handling
 *
 *  =========================================================================
 */
STATIC int
mtp_opt_check(struct mtp *mtp, struct mtp_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->sls)
			ops->flags |= (1 << T_MTP_SLS);
		if (ops->mp)
			ops->flags |= (1 << T_MTP_MP);
		if (ops->debug)
			ops->flags |= (1 << T_MTP_DEBUG);
	}
	return (0);
}
STATIC int
mtp_opt_default(struct mtp *mtp, struct mtp_opts *ops)
{
	if (ops) {
		int flags = ops->flags;
		ops->flags = 0;
		if (!flags || ops->sls) {
			ops->sls = &opt_defaults.sls;
			ops->flags |= (1 << T_MTP_SLS);
		}
		if (!flags || ops->mp) {
			ops->mp = &opt_defaults.mp;
			ops->flags |= (1 << T_MTP_MP);
		}
		if (!flags || ops->debug) {
			ops->debug = &opt_defaults.debug;
			ops->flags |= (1 << T_MTP_DEBUG);
		}
		return (0);
	}
	swerr();
	return (-EFAULT);
}
STATIC int
mtp_opt_current(struct mtp *mtp, struct mtp_opts *ops)
{
	int flags = ops->flags;
	ops->flags = 0;
	if (!flags || ops->sls) {
		ops->sls = &mtp->options.sls;
		ops->flags |= (1 << T_MTP_SLS);
	}
	if (!flags || ops->mp) {
		ops->mp = &mtp->options.mp;
		ops->flags |= (1 << T_MTP_MP);
	}
	if (!flags || ops->debug) {
		ops->debug = &mtp->options.debug;
		ops->flags |= (1 << T_MTP_DEBUG);
	}
	return (0);
}
STATIC int
mtp_opt_negotiate(struct mtp *mtp, struct mtp_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->sls) {
			mtp->options.sls = *ops->sls;
			ops->sls = &mtp->options.sls;
			ops->flags |= (1 << T_MTP_SLS);
		}
		if (ops->mp) {
			mtp->options.mp = *ops->mp;
			ops->mp = &mtp->options.mp;
			ops->flags |= (1 << T_MTP_MP);
		}
		if (ops->debug) {
			mtp->options.debug = *ops->debug;
			ops->debug = &mtp->options.debug;
			ops->flags |= (1 << T_MTP_DEBUG);
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  STATE Changes
 *
 *  =========================================================================
 */
#ifdef _DEBUG
STATIC const char *
mtp_state_name(struct mtp *mtp, long state)
{
	switch (mtp->i_style) {
	case MTP_STYLE_MGMT:
	case MTP_STYLE_MTPI:
		switch (state) {
		case MTPS_UNBND:
			return ("MTPS_UNBND");
		case MTPS_WACK_BREQ:
			return ("MTPS_WACK_BREQ");
		case MTPS_IDLE:
			return ("MTPS_IDLE");
		case MTPS_WACK_CREQ:
			return ("MTPS_WACK_CREQ");
		case MTPS_WCON_CREQ:
			return ("MTPS_WCON_CREQ");
		case MTPS_CONNECTED:
			return ("MTPS_CONNECTED");
		case MTPS_WACK_UREQ:
			return ("MTPS_WACK_UREQ");
		case MTPS_WACK_DREQ6:
			return ("MTPS_WACK_DREQ6");
		case MTPS_WACK_DREQ9:
			return ("MTPS_WACK_DREQ9");
		case MTPS_WACK_OPTREQ:
			return ("MTPS_WACK_OPTREQ");
		case MTPS_WREQ_ORDREL:
			return ("MTPS_WREQ_ORDREL");
		case MTPS_WIND_ORDREL:
			return ("MTPS_WIND_ORDREL");
		case MTPS_WRES_CIND:
			return ("MTPS_WRES_CIND");
		case MTPS_UNUSABLE:
			return ("MTPS_UNUSABLE");
		default:
			return ("(unknown)");
		}
		break;
	case MTP_STYLE_TPI:
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
		case TS_NOSTATES:
			return ("TS_NOSTATES");
		default:
			return ("(unknown)");
		}
		break;
	case MTP_STYLE_NPI:
		switch (state) {
		case NS_UNBND:
			return ("NS_UNBND");
		case NS_WACK_BREQ:
			return ("NS_WACK_BREQ");
		case NS_WACK_UREQ:
			return ("NS_WACK_UREQ");
		case NS_IDLE:
			return ("NS_IDLE");
		case NS_WACK_OPTREQ:
			return ("NS_WACK_OPTREQ");
		case NS_WACK_RRES:
			return ("NS_WACK_RRES");
		case NS_WCON_CREQ:
			return ("NS_WCON_CREQ");
		case NS_WRES_CIND:
			return ("NS_WRES_CIND");
		case NS_WACK_CRES:
			return ("NS_WACK_CRES");
		case NS_DATA_XFER:
			return ("NS_DATA_XFER");
		case NS_WCON_RREQ:
			return ("NS_WCON_RREQ");
		case NS_WRES_RIND:
			return ("NS_WRES_RIND");
		case NS_WACK_DREQ6:
			return ("NS_WACK_DREQ6");
		case NS_WACK_DREQ7:
			return ("NS_WACK_DREQ7");
		case NS_WACK_DREQ9:
			return ("NS_WACK_DREQ9");
		case NS_WACK_DREQ10:
			return ("NS_WACK_DREQ10");
		case NS_WACK_DREQ11:
			return ("NS_WACK_DREQ11");
		case NS_NOSTATES:
			return ("NS_NOSTATES");
		default:
			return ("(unknown)");
		}
		break;
	default:
		return ("(unknown)");
	}
}
STATIC const char *
sl_state_name(struct sl *sl, long state)
{
	switch (state) {
	case SLS_OUT_OF_SERVICE:
		return ("SLS_OUT_OF_SERVICE");
	case SLS_PROC_OUTG:
		return ("SLS_PROC_OUTG");
	case SLS_IN_SERVICE:
		return ("SLS_IN_SERVICE");
	case SLS_WACK_COO:
		return ("SLS_WACK_COO");
	case SLS_WACK_ECO:
		return ("SLS_WACK_ECO");
	case SLS_WCON_RET:
		return ("SLS_WCON_RET");
	case SLS_WIND_CLRB:
		return ("SLS_WIND_CLRB");
	case SLS_WIND_BSNT:
		return ("SLS_WIND_BSNT");
	case SLS_WIND_INSI:
		return ("SLS_WIND_INSI");
	case SLS_WACK_SLTM:
		return ("SLS_WACK_SLTM");
	default:
		return ("(unknown)");
	}
}
STATIC const char *
sl_i_state_name(struct sl *sl, long state)
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
#endif
STATIC void
mtp_set_state(struct mtp *mtp, long state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, mtp, mtp_state_name(mtp, state),
		mtp_state_name(mtp, mtp->prot->CURRENT_state)));
	mtp->prot->CURRENT_state = state;
}
STATIC long
mtp_get_state(struct mtp *mtp)
{
	return (mtp->prot->CURRENT_state);
}
STATIC INLINE void
sl_set_l_state(struct sl *sl, long state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, sl, sl_state_name(sl, state),
		sl_state_name(sl, sl->l_state)));
	sl->l_state = state;
}
STATIC INLINE long
sl_get_l_state(struct sl *sl)
{
	return (sl->l_state);
}
STATIC INLINE void
sl_set_i_state(struct sl *sl, long state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, sl, sl_i_state_name(sl, state),
		sl_i_state_name(sl, sl->i_state)));
	sl->i_state = state;
}
STATIC INLINE long
sl_get_i_state(struct sl *sl)
{
	return (sl->i_state);
}

/*
 *  =========================================================================
 *
 *  MTP Functions
 *
 *  =========================================================================
 */
/*
 *  CHECK SRC (BIND) ADDRESS
 *  -----------------------------------
 *  Check the source (bind) address. This must be an address local to the
 *  stack and the bind must be for an SI value which is not alreay bound
 *  (T_CLTS).
 */
STATIC struct sp *
mtp_check_src(struct mtp *mtp, struct mtp_addr *src, int *errp)
{
	struct sp *sp;
	for (sp = master.sp.list; sp; sp = (struct sp *) sp->next)
		if (sp->ni == src->ni && sp->pc == (src->pc & sp->na.na->mask.member))
			goto check;
	goto noaddr;
      check:
	if (sp->na.na->prot[src->si]->SERV_type == T_CLTS && sp->mtp.lists[src->si])
		goto addrbusy;
	return (sp);
      noaddr:
	*errp = (TNOADDR);
	printd(("%s: %p: ERROR: Couldn't allocate source address\n", DRV_NAME, mtp));
	return (NULL);
      addrbusy:
	*errp = (TADDRBUSY);
	printd(("%s: %p: ERROR: Source address in use\n", DRV_NAME, mtp));
	return (NULL);
}

/*
 *  CHECK DST (CONN) ADDRESS
 *  -----------------------------------
 *  Check the destination (connect) address.  This may be a local, adjacent or
 *  remote address and the connection must be for a destination point code and
 *  SI value which is not already connected.
 */
STATIC int
mtp_check_dst(struct mtp *mtp, struct mtp_addr *dst)
{
	struct sp *sp;
	struct mtp *m2;
	for (sp = master.sp.list; sp; sp = (struct sp *) sp->next)
		if (sp->ni == dst->ni && sp->pc == (dst->pc & sp->na.na->mask.member))
			goto check;
	goto noaddr;
      check:
	if (mtp->prot->SERV_type == T_CLTS)
		goto efault;
	for (m2 = sp->mtp.lists[dst->si & 0x0f]; m2; m2 = m2->next)
		if (m2->src.ni == dst->ni
		    && m2->src.pc == (dst->pc & m2->sp.loc->na.na->mask.member))
			goto addrbusy;
	return (0);
      noaddr:
	printd(("%s: %p: ERROR: Couldn't allocate source address\n", DRV_NAME, mtp));
	return (TNOADDR);
      addrbusy:
	printd(("%s: %p: ERROR: Source address in use\n", DRV_NAME, mtp));
	return (TADDRBUSY);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  MTP BIND
 *  -------------------------------------------------------------------------
 *  Add the MTP user to the local signalling point hashes if T_CLTS.
 */
STATIC int
mtp_bind(struct mtp *mtp, struct mtp_addr *src)
{
	struct sp *loc;
	int err = 0;
	if (!(loc = mtp_check_src(mtp, src, &err)))
		goto error;
	if (!mtp->sp.loc)
		mtp->sp.loc = sp_get(loc);
	mtp->prot = na_get(loc->na.na)->prot[src->si];	/* point to protocol profile */
	loc->mtp.equipped |= (1 << src->si);
	if (mtp->prot->SERV_type == T_CLTS) {
		/* 
		   connectionless, add to user lists now */
		loc->mtp.available |= (1 << src->si);
		if ((mtp->sp.next = loc->mtp.lists[src->si]))
			mtp->sp.next->sp.prev = &mtp->sp.next;
		mtp->sp.prev = &loc->mtp.lists[src->si];
		loc->mtp.lists[src->si] = mtp_get(mtp);
	}
	/* 
	   connection oriented, add to user lists on connect only */
	return (0);
      error:
	return (err);
}

/*
 *  MTP UNBIND
 *  -------------------------------------------------------------------------
 *  Remove the MTP user from the local signalling point hashes if T_CLTS.
 */
STATIC int
mtp_unbind(struct mtp *mtp)
{
	struct sp *loc;
	struct na *na;
	if (!mtp || !mtp->prot || !(loc = mtp->sp.loc) || !(na = loc->na.na))
		goto efault;
	if (mtp->prot->SERV_type == T_CLTS) {
		/* 
		   connectionless, remove from user lists now */
		if ((*(mtp->sp.prev) = mtp->sp.next))
			mtp->sp.next->sp.prev = mtp->sp.prev;
		else
			loc->mtp.available &= ~(1 << mtp->src.si);
		mtp->sp.next = NULL;
		mtp_put(mtp);
	}
	/* 
	   connection oriented, already removed from user lists on disconnect */
	mtp->sp.loc = NULL;
	sp_put(loc);
	na_put(na);
	mtp->prot = NULL;
	return (0);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  MTP CONNECT
 *  -------------------------------------------------------------------------
 *  Add the MTP user to the local signalling point service hashes.
 */
STATIC int
mtp_connect(struct mtp *mtp, struct mtp_addr *dst)
{
	struct sp *loc;
	struct rs *rem;
	if ((rem = mtp->sp.rem))
		goto eisconn;
	if (!(loc = mtp->sp.loc))
		goto efault;
	if (loc->ni != dst->ni)
		goto badaddr;
	for (rem = loc->rs.list; rem && rem->dest != dst->pc; rem = rem->sp.next) ;
	if (!rem)
		goto badaddr;
	mtp->sp.rem = rs_get(rem);
	loc->mtp.available |= (1 << dst->si);
	if ((mtp->sp.next = loc->mtp.lists[dst->si]))
		mtp->sp.next->sp.prev = &mtp->sp.next;
	mtp->sp.prev = &loc->mtp.lists[dst->si];
	loc->mtp.lists[dst->si] = mtp_get(mtp);
	return (0);
      efault:
	swerr();
	return (-EFAULT);
      eisconn:
	swerr();
	return (-EISCONN);
      badaddr:
	return (-EADDRNOTAVAIL);
}

/*
 *  MTP DISCONNECT
 *  -------------------------------------------------------------------------
 *  Remove the MTP user from the local signalling point service hashes.
 */
STATIC int
mtp_disconnect(struct mtp *mtp)
{
	struct sp *loc;
	struct rs *rem;
	if (!mtp || !(loc = mtp->sp.loc) || !(rem = mtp->sp.rem))
		goto efault;
	if ((*(mtp->sp.prev) = mtp->sp.next))
		mtp->sp.next->sp.prev = mtp->sp.prev;
	mtp->sp.next = NULL;
	mtp->sp.prev = NULL;
	if (!loc->mtp.lists[mtp->src.si])
		loc->mtp.available &= ~(1 << mtp->src.si);
	mtp_put(mtp);
	mtp->sp.rem = NULL;
	rs_put(rem);
	return (0);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  MTP SEND MSG
 *  -------------------------------------------------------------------------
 *  Send a user message.
 */
STATIC int mtp_send_user(queue_t *, struct sp *, uint, uint32_t, uint32_t, uint, uint, uint,
			 mblk_t *);
STATIC int
mtp_send_msg(queue_t *q, struct mtp *mtp, struct mtp_opts *opt, struct mtp_addr *dst, mblk_t *dp)
{
	struct sp *sp = mtp->sp.loc;
	ulong mp = (opt && opt->mp) ? *opt->mp : 0;
	ulong sls = (opt && opt->sls) ? *opt->sls : (sp->sls++ & sp->ls.sls_mask);
	fixme(("Rewrite this function for multiple interface styles\n"));
	return mtp_send_user(q, sp, sp->ni, dst->pc, sp->pc, sls, mp, dst->si, dp);
}

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
 *  Primitive sent upstream
 *
 *  -------------------------------------------------------------------------
 */
#ifndef TS_NO_STATES
#define TS_NO_STATES 64
#endif
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(q, mtp, error)
	queue_t *q;			/* service queue */
	struct mtp *mtp;		/* mtp private structure */
	int error;			/* error number */
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
			printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, mtp));
			ss7_oput(mtp->oq, mp);
			return (-error);
		} else {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = error < 0 ? -error : error;
			*(mp->b_wptr)++ = error < 0 ? -error : error;
			mtp_set_state(mtp, TS_NO_STATES);
			printd(("%s; %p: <- M_ERROR\n", DRV_NAME, mtp));
			ss7_oput(mtp->oq, mp);
			return (QR_DONE);
		}
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MTP_ERROR_ACK               10 - Negative acknowledgement
 *  -----------------------------------
 */
STATIC int
m_error_ack(q, mtp, prim, err)
	queue_t *q;			/* service queue */
	struct mtp *mtp;		/* mtp private structure */
	ulong prim;			/* primitive in error */
	ulong err;			/* error type */
{
	mblk_t *mp;
	struct MTP_error_ack *p;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	case 0:
		never();
		return (err);
	}
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_ERROR_ACK;
		p->mtp_error_primitive = prim;
		p->mtp_mtpi_error = err < 0 ? MSYSERR : err;
		p->mtp_unix_error = err < 0 ? -err : 0;
		printd(("%s: %p: <- MTP_ERROR_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		/* 
		   Returning -EPROTO here will make sure that the old state is restored correctly
		   (in mtp_w_proto).  If we return QR_DONE, then the state will never be restored. */
		if (err >= 0)
			err = -EPROTO;
		return (err);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  MTP_OK_ACK                   9 - Positivie acknowledgement
 *  -----------------------------------
 */
STATIC int
m_ok_ack(queue_t *q, struct mtp *mtp, ulong prim)
{
	int err = -EFAULT;
	mblk_t *mp;
	struct MTP_ok_ack *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_OK_ACK;
		p->mtp_correct_prim = prim;
		switch (mtp_get_state(mtp)) {
		case MTPS_WACK_CREQ:
			if ((err = mtp_connect(mtp, &mtp->dst)))
				goto error;
			mtp_set_state(mtp, MTPS_CONNECTED);
			break;
		case MTPS_WACK_UREQ:
			if ((err = mtp_unbind(mtp)))
				goto error;
			mtp_set_state(mtp, MTPS_UNBND);
			break;
		case MTPS_WACK_DREQ6:
		case MTPS_WACK_DREQ9:
			if ((err = mtp_disconnect(mtp)))
				goto error;
			mtp_set_state(mtp, MTPS_IDLE);
			break;
		default:
			/* 
			   Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a MTP_OPTMGMT_REQ in other
			   than MTPS_IDLE state. */
			break;
		}
		printd(("%s: %p: <- MTP_OK_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
      error:
	freemsg(mp);
	return m_error_ack(q, mtp, prim, err);
}

/*
 *  MTP_BIND_ACK                11 - Bind acknowledgement
 *  -----------------------------------
 */
STATIC int
m_bind_ack(queue_t *q, struct mtp *mtp, struct mtp_addr *add)
{
	int err;
	mblk_t *mp;
	struct MTP_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		if ((err = mtp_bind(mtp, add)))
			goto error;
		mtp_set_state(mtp, MTPS_IDLE);
		printd(("%s: %p: <- MTP_BIND_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
      error:
	freemsg(mp);
	return m_error_ack(q, mtp, MTP_BIND_REQ, err);
}

/*
 *  MTP_ADDR_ACK                12 - Address acknowledgement
 *  -----------------------------------
 */
STATIC int
m_addr_ack(queue_t *q, struct mtp *mtp, struct mtp_addr *loc, struct mtp_addr *rem)
{
	mblk_t *mp;
	struct MTP_addr_ack *p;
	size_t loc_len = loc ? sizeof(*loc) : 0;
	size_t rem_len = rem ? sizeof(*rem) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_ADDR_ACK;
		p->mtp_loc_length = loc_len;
		p->mtp_loc_offset = loc_len ? sizeof(*p) : 0;
		p->mtp_rem_length = rem_len;
		p->mtp_rem_offset = rem_len ? sizeof(*p) : 0;
		if (loc_len) {
			bcopy(loc, mp->b_wptr, loc_len);
			mp->b_wptr += loc_len;
		}
		if (rem_len) {
			bcopy(rem, mp->b_wptr, rem_len);
			mp->b_wptr += rem_len;
		}
		printd(("%s: %p: <- MTP_ADDR_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  MTP_INFO_ACK                13 - Information acknowledgement
 *  -----------------------------------
 */
STATIC int
m_info_ack(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	struct MTP_info_ack *p;
	size_t src_len, dst_len;
	uchar *src_ptr, *dst_ptr;
	switch (mtp_get_state(mtp)) {
	default:
	case MTPS_UNBND:
	case MTPS_WACK_BREQ:
		src_len = 0;
		src_ptr = NULL;
		dst_len = 0;
		dst_ptr = NULL;
		break;
	case MTPS_IDLE:
	case MTPS_WACK_UREQ:
	case MTPS_WACK_CREQ:
		src_len = sizeof(mtp->src);
		src_ptr = (uchar *) &mtp->src;
		dst_len = 0;
		dst_ptr = NULL;
		break;
	case MTPS_CONNECTED:
	case MTPS_WACK_DREQ6:
	case MTPS_WACK_DREQ9:
		src_len = sizeof(mtp->src);
		src_ptr = (uchar *) &mtp->src;
		dst_len = sizeof(mtp->dst);
		dst_ptr = (uchar *) &mtp->dst;
		break;
	}
	if ((mp = ss7_allocb(q, sizeof(*p) + src_len + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_msu_size = 279;
		p->mtp_addr_size = sizeof(struct mtp_addr);
		p->mtp_addr_length = src_len + dst_len;
		p->mtp_addr_offset = src_len + dst_len ? sizeof(*p) : 0;
		p->mtp_current_state = mtp_get_state(mtp);
		p->mtp_serv_type = mtp->prot->SERV_type == T_CLTS ? M_CLMS : M_COMS;
		p->mtp_version = MTP_CURRENT_VERSION;
		if (src_len) {
			bcopy(&mtp->src, mp->b_wptr, sizeof(mtp->src));
			mp->b_wptr += sizeof(mtp->src);
		}
		if (dst_len) {
			bcopy(&mtp->dst, mp->b_wptr, sizeof(mtp->dst));
			mp->b_wptr += sizeof(mtp->dst);
		}
		printd(("%s: %p: <- MTP_INFO_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  MTP_OPTMGMT_ACK             14 - Options management acknowledgement
 *  -----------------------------------
 */
STATIC int
m_optmgmt_ack(queue_t *q, struct mtp *mtp, struct mtp_opts *opt, ulong flags)
{
	mblk_t *mp;
	struct MTP_optmgmt_ack *p;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_OPTMGMT_ACK;
		p->mtp_opt_length = opt_len;
		p->mtp_opt_offset = opt_len ? sizeof(*p) : 0;
		p->mtp_mgmt_flags = flags;
		if (opt_len) {
			mtp_build_opts(mtp, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
		}
		if (mtp_get_state(mtp) == MTPS_WACK_OPTREQ)
			mtp_set_state(mtp, MTPS_IDLE);
		printd(("%s: %p: <- MTP_OPTMGMT_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  MTP_TRANSFER_IND            15 - MTP data transfer indication
 *  -----------------------------------
 */
STATIC int
m_transfer_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *src, ulong sls, ulong mp, mblk_t *dp)
{
	if (canput(mtp->oq)) {
		mblk_t *bp;
		struct MTP_transfer_ind *p;
		size_t src_len = src ? sizeof(*src) : 0;
		if ((bp = ss7_allocb(q, sizeof(*p) + src_len, BPRI_MED))) {
			bp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) bp->b_wptr)++;
			p->mtp_primitive = MTP_TRANSFER_IND;
			p->mtp_srce_length = src_len;
			p->mtp_srce_offset = src_len ? sizeof(*p) : 0;
			p->mtp_mp = mp;
			p->mtp_sls = sls;
			bp->b_cont = dp;
			printd(("%s: %p: <- MTP_TRANSFER_IND\n", DRV_NAME, mtp));
			ss7_oput(mtp->oq, bp);
			return (QR_ABSORBED);
		}
		ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
		return (-ENOBUFS);
	}
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, mtp));
	return (-EBUSY);
}

/*
 *  MTP_PAUSE_IND               16 - MTP pause (stop) indication
 *  -----------------------------------
 */
STATIC int
m_pause_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *add)
{
	mblk_t *mp;
	struct MTP_pause_ind *p;
	const size_t add_len = sizeof(*add);
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_PAUSE_IND;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: <- MTP_TRANSFER_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  MTP_RESUME_IND              17 - MTP resume (start) indication
 *  -----------------------------------
 */
STATIC int
m_resume_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *add)
{
	mblk_t *mp;
	struct MTP_resume_ind *p;
	const size_t add_len = sizeof(*add);
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_RESUME_IND;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: <- MTP_RESUME_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  MTP_STATUS_IND              18 - MTP status indication
 *  -----------------------------------
 */
STATIC int
m_status_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *add, struct mtp_opts *opt, mblk_t *dp,
	     ulong etype)
{
	mblk_t *mp;
	struct MTP_status_ind *p;
	const size_t add_len = sizeof(*add);
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_STATUS_IND;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = add_len ? sizeof(*p) : 0;
		if ((etype & 0x07) == 0x03) {
			p->mtp_type = MTP_STATUS_TYPE_CONG;
			p->mtp_status = T_MTP_CONGESTION_STATUS(etype);
		} else {
			p->mtp_type = MTP_STATUS_TYPE_UPU;
			p->mtp_status = (etype & 0x07) - 4;
		}
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: <- MTP_STATUS_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  MTP_RESTART_COMPLETE_IND    19 - MTP restart complete (impl. dep.)
 *  -----------------------------------
 */
STATIC INLINE int
m_restart_complete_ind(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	struct MTP_restart_complete_ind *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->mtp_primitive = MTP_RESTART_COMPLETE_IND;
		printd(("%s: %p: <- MTP_RESTART_COMPLETE_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  N_CONN_IND          11 - Incoming connection indication
 *  -----------------------------------
 */
STATIC INLINE int
n_conn_ind(queue_t *q, struct mtp *mtp, ulong seq, ulong flags, struct mtp_addr *src,
	   struct mtp_addr *dst, N_qos_sel_conn_mtp_t *qos)
{
	mblk_t *mp;
	N_conn_ind_t *p;
	const size_t src_len = src ? sizeof(*src) : 0;
	const size_t dst_len = dst ? sizeof(*dst) : 0;
	const size_t qos_len = qos ? sizeof(*qos) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + src_len + dst_len + qos_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_CONN_IND;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) : 0;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + src_len + dst_len : 0;
		p->SEQ_number = seq;
		p->CONN_flags = flags;
		if (src_len) {
			bcopy(mp->b_wptr, src, src_len);
			mp->b_wptr += src_len;
		}
		if (dst_len) {
			bcopy(mp->b_wptr, dst, dst_len);
			mp->b_wptr += dst_len;
		}
		if (qos_len) {
			bcopy(mp->b_wptr, qos, qos_len);
			mp->b_wptr += qos_len;
		}
		printd(("%s: %p: <- N_CONN_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_CONN_CON          12 - Connection established
 *  -----------------------------------
 */
STATIC int
n_conn_con(queue_t *q, struct mtp *mtp, ulong flags, struct mtp_addr *res,
	   N_qos_sel_conn_mtp_t *qos)
{
	mblk_t *mp;
	N_conn_con_t *p;
	const size_t res_len = res ? sizeof(*res) : 0;
	const size_t qos_len = qos ? sizeof(*qos) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + res_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_CONN_CON;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->CONN_flags = flags;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
		if (res_len) {
			bcopy(mp->b_wptr, res, res_len);
			mp->b_wptr += res_len;
		}
		if (qos_len) {
			bcopy(mp->b_wptr, qos, qos_len);
			mp->b_wptr += qos_len;
		}
		if (mtp_get_state(mtp) != NS_WCON_CREQ)
			swerr();
		mtp_set_state(mtp, NS_DATA_XFER);
		printd(("%s: %p: <- N_CONN_CON\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_DISCON_IND        13 - NC disconnected
 *  -----------------------------------
 */
STATIC INLINE int
n_discon_ind(queue_t *q, struct mtp *mtp, ulong orig, ulong reason, ulong seq, struct mtp_addr *res,
	     mblk_t *dp)
{
	mblk_t *mp;
	N_discon_ind_t *p;
	const size_t res_len = res ? sizeof(*res) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + res_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_DISCON_IND;
		p->DISCON_orig = orig;
		p->DISCON_reason = reason;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		if (res_len) {
			bcopy(mp->b_wptr, res, res_len);
			mp->b_wptr += res_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- N_DISCON_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_DATA_IND          14 - Incoming connection-mode data indication
 *  -----------------------------------
 */
STATIC int
n_data_ind(queue_t *q, struct mtp *mtp, ulong flags, mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_DATA_IND;
		p->DATA_xfer_flags = flags;
		mp->b_cont = dp;
		printd(("%s: %p: <- N_DATA_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_EXDATA_IND        15 - Incoming expedited data indication
 *  -----------------------------------
 */
STATIC INLINE int
n_exdata_ind(queue_t *q, struct mtp *mtp, mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 1;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_EXDATA_IND;
		mp->b_cont = dp;
		printd(("%s: %p: <- N_EXDATA_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_INFO_ACK          16 - Information Acknowledgement
 *  -----------------------------------
 */
STATIC int
n_info_ack(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	N_info_ack_t *p;
	size_t src_len, dst_len, qos_len, qor_len;
	N_qos_sel_info_mtp_t *qos;
	N_qos_range_info_mtp_t *qor;
	struct sp *sp = mtp->sp.loc;
	struct na *na;
	ulong state;
	if (sp && sp->na.na) {
		na = sp->na.na;
		qos_len = sizeof(*qos);
		qor_len = sizeof(*qor);
	} else {
		na = NULL;
		qos_len = 0;
		qor_len = 0;
	}
	switch (mtp_get_state(mtp)) {
	default:
	case MTPS_UNBND:
	case MTPS_WACK_BREQ:
		src_len = 0;
		dst_len = 0;
		break;
	case MTPS_IDLE:
	case MTPS_WACK_UREQ:
	case MTPS_WACK_CREQ:
		src_len = sizeof(mtp->src);
		dst_len = 0;
		break;
	case MTPS_CONNECTED:
	case MTPS_WACK_DREQ6:
	case MTPS_WACK_DREQ9:
		src_len = sizeof(mtp->src);
		dst_len = sizeof(mtp->dst);
		break;
	}
	switch (mtp_get_state(mtp)) {
	case MTPS_UNBND:
		state = NS_UNBND;
		break;
	case MTPS_WACK_BREQ:
		state = NS_WACK_BREQ;
		break;
	case MTPS_IDLE:
		state = NS_IDLE;
		break;
	case MTPS_WACK_UREQ:
		state = NS_WACK_UREQ;
		break;
	case MTPS_WACK_CREQ:
		state = NS_WCON_CREQ;
		break;
	case MTPS_CONNECTED:
		state = NS_DATA_XFER;
		break;
	case MTPS_WACK_DREQ6:
		state = NS_WACK_DREQ6;
		break;
	case MTPS_WACK_DREQ9:
		state = NS_WACK_DREQ9;
		break;
	default:
		state = NS_NOSTATES;
		break;
	}
	if ((mp = ss7_allocb(q, sizeof(*p) + src_len + dst_len + qos_len + qor_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_INFO_ACK;
		p->NSDU_size = 279;
		p->ENSDU_size = 0;
		p->CDATA_size = 0;
		p->DDATA_size = 0;
		p->ADDR_size = sizeof(struct mtp_addr);
		p->ADDR_length = src_len + dst_len;
		p->ADDR_offset = src_len + dst_len ? sizeof(*p) : 0;
		p->QOS_length = qos_len;
		p->QOS_offset = qos_len ? sizeof(*p) + src_len + dst_len : 0;
		p->QOS_range_length = qor_len;
		p->QOS_range_offset = qor_len ? sizeof(*p) + src_len + dst_len + qos_len : 0;
		p->OPTIONS_flags = 0;
		p->NIDU_size = 279;
		p->SERV_type = (mtp->prot->SERV_type == T_CLTS) ? N_CLNS : N_CONS;
		p->CURRENT_state = state;
		p->PROVIDER_type = N_SNICFP;
		p->NODU_size = 279;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		p->NPI_version = N_VERSION_2;
		if (src_len) {
			bcopy(&mtp->src, mp->b_wptr, sizeof(mtp->src));
			mp->b_wptr += sizeof(mtp->src);
		}
		if (dst_len) {
			bcopy(&mtp->dst, mp->b_wptr, sizeof(mtp->dst));
			mp->b_wptr += sizeof(mtp->dst);
		}
		if (qos_len) {
			qos = ((typeof(qos)) mp->b_wptr)++;
			qos->n_qos_type = N_QOS_SEL_INFO_MTP;
			qos->pvar = na->option.pvar;
			qos->popt = na->option.popt;
		}
		if (qor_len) {
			qor = ((typeof(qor)) mp->b_wptr)++;
			qor->n_qos_type = N_QOS_RANGE_INFO_MTP;
			qor->sls_range = sp->ls.sls_mask;
			qor->mp_range = (na->option.popt & SS7_POPT_MPLEV) ? 3 : 0;
		}
		printd(("%s: %p: <- N_INFO_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_ERROR_ACK         18 - Error Acknowledgement
 *  -----------------------------------
 */
STATIC int
n_error_ack(queue_t *q, struct mtp *mtp, ulong prim, long err)
{
	mblk_t *mp;
	N_error_ack_t *p;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	case 0:
		swerr();
		return (err);
	}
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = err < 0 ? NSYSERR : err;
		p->UNIX_error = err < 0 ? -err : 0;
		printd(("%s: %p: <- N_ERROR_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		/* 
		   Retruning -EPROTO here will make sure that the old state is restored correctly.
		   If we return QR_DONE, then the state will never be restored. */
		if (err >= 0)
			err = -EPROTO;
		return (err);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_OK_ACK            19 - Success Acknowledgement
 *  -----------------------------------
 */
STATIC int
n_ok_ack(queue_t *q, struct mtp *mtp, ulong prim)
{
	mblk_t *mp;
	N_ok_ack_t *p;
	int err;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		switch (mtp_get_state(mtp)) {
		case NS_WACK_UREQ:
			if ((err = mtp_unbind(mtp)))
				return (err);
			mtp_set_state(mtp, NS_UNBND);
			break;
		case NS_WACK_OPTREQ:
			mtp_set_state(mtp, NS_IDLE);
			break;
		case NS_WACK_RRES:
			mtp_set_state(mtp, NS_DATA_XFER);
			break;
		case NS_WACK_CRES:
			mtp_set_state(mtp, NS_DATA_XFER);
			break;
		case NS_WACK_DREQ6:
		case NS_WACK_DREQ7:
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			if ((err = mtp_disconnect(mtp)))
				return (err);
			mtp_set_state(mtp, NS_IDLE);
			break;
		default:
			/* 
			   Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a T_OPTMGMT_REQ in other than
			   TS_IDLE state. */
			seldom();
			break;
		}
		printd(("%s: %p: <- N_OK_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_BIND_ACK          17 - NS User bound to network address
 *  -----------------------------------
 */
STATIC int
n_bind_ack(queue_t *q, struct mtp *mtp, struct mtp_addr *add)
{
	int err;
	mblk_t *mp;
	N_bind_ack_t *p;
	size_t add_len = add ? sizeof(*add) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = 0;
		p->TOKEN_value = (ulong) mtp->oq;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		if ((err = mtp_bind(mtp, add)))
			goto free_error;
		mtp_set_state(mtp, NS_IDLE);
		printd(("%s: %p: <- N_BIND_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return n_error_ack(q, mtp, N_BIND_REQ, err);
}

/*
 *  N_UNITDATA_IND      20 - Connection-less data receive indication
 *  -----------------------------------
 */
STATIC int
n_unitdata_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *src, struct mtp_addr *dst, mblk_t *dp)
{
	mblk_t *mp;
	N_unitdata_ind_t *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + src_len + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_UNITDATA_IND;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) : 0;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) + src_len : 0;
		if (src_len) {
			bcopy(src, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		if (dst_len) {
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- N_UNITDATA_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_UDERROR_IND       21 - UNITDATA Error Indication
 *  -----------------------------------
 */
STATIC int
n_uderror_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *dst, struct mtp_opts *opt, mblk_t *dp,
	      ulong etype)
{
	mblk_t *mp;
	N_uderror_ind_t *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_UDERROR_IND;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->ERROR_type = etype;
		if (dst_len) {
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- N_UDERROR_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_DATACK_IND        24 - Data acknowledgement indication
 *  -----------------------------------
 */
STATIC INLINE int
n_datack_ind(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	N_datack_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_DATACK_IND;
		printd(("%s: %p: <- N_DATACK_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_RESET_IND         26 - Incoming NC reset request indication
 *  -----------------------------------
 */
STATIC INLINE int
n_reset_ind(queue_t *q, struct mtp *mtp, ulong orig, ulong reason)
{
	mblk_t *mp;
	N_reset_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_RESET_IND;
		p->RESET_orig = orig;
		p->RESET_reason = reason;
		printd(("%s: %p: <- N_RESET_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  N_RESET_CON         28 - Reset processing complete
 *  -----------------------------------
 */
STATIC INLINE int
n_reset_con(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	N_reset_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = N_RESET_CON;
		printd(("%s: %p: <- N_RESET_CON\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_CONN_IND          11 - 
 *  -----------------------------------
 */
STATIC INLINE int
t_conn_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", DRV_NAME, mtp));
	return (-EFAULT);
}

/*
 *  T_CONN_CON          12 - 
 *  -----------------------------------
 */
STATIC int
t_conn_con(queue_t *q, struct mtp *mtp, struct mtp_addr *res, struct mtp_opts *opt, mblk_t *dp)
{
	mblk_t *mp;
	struct T_conn_con *p;
	size_t res_len = res ? sizeof(*res) : 0;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_CONN_CON;
		p->RES_length = res_len;
		p->RES_offset = res_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + opt_len : 0;
		if (res_len) {
			bcopy(res, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
		}
		if (opt_len) {
			mtp_build_opts(mtp, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- T_CONN_CON\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_DISCON_IND        13 - 
 *  -----------------------------------
 */
STATIC INLINE int
t_discon_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", DRV_NAME, mtp));
	return (-EFAULT);
}

/*
 *  T_DATA_IND          14 - 
 *  -----------------------------------
 */
STATIC INLINE int
t_data_ind(queue_t *q, struct mtp *mtp, ulong more, mblk_t *dp)
{
	if (canput(mtp->oq)) {
		mblk_t *mp;
		struct T_data_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_DATA_IND;
			p->MORE_flag = more;
			mp->b_cont = dp;
			printd(("%s: %p: <- T_DATA_IND\n", DRV_NAME, mtp));
			ss7_oput(mtp->oq, mp);
			return (QR_DONE);
		}
		ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
		return (-ENOBUFS);
	}
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, mtp));
	return (-EBUSY);
}

/*
 *  T_EXDATA_IND        15 - 
 *  -----------------------------------
 */
STATIC INLINE int
t_exdata_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", DRV_NAME, mtp));
	return (-EFAULT);
}

/*
 *  T_INFO_ACK          16 - 
 *  -----------------------------------
 */
STATIC int
t_info_ack(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	struct T_info_ack *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_INFO_ACK;
		*p = *mtp->prot;
		printd(("%s: %p: <- T_INFO_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  T_ERROR_ACK         18 - 
 *  -----------------------------------
 */
STATIC int
t_error_ack(queue_t *q, struct mtp *mtp, ulong prim, long error)
{
	mblk_t *mp;
	struct T_error_ack *p;
	switch (error) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (error);
	case 0:
		never();
		return (error);
	}
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = error < 0 ? TSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		printd(("%s: %p: <- T_ERROR_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		/* 
		   Retruning -EPROTO here will make sure that the old state is restored correctly.
		   If we return QR_DONE, then the state will never be restored. */
		return (-EPROTO);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  T_OK_ACK            19 - 
 *  -----------------------------------
 */
STATIC int
t_ok_ack(queue_t *q, struct mtp *mtp, ulong prim)
{
	int err = -EFAULT;
	mblk_t *mp;
	struct T_ok_ack *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		switch (mtp_get_state(mtp)) {
		case TS_WACK_UREQ:
			if ((err = mtp_unbind(mtp)))
				goto free_error;
			mtp_set_state(mtp, TS_UNBND);
			break;
		case TS_WACK_OPTREQ:
			mtp_set_state(mtp, TS_IDLE);
			break;
		case TS_WACK_CREQ:
			if ((err = mtp_connect(mtp, &mtp->dst)))
				goto free_error;
			mtp_set_state(mtp, TS_WCON_CREQ);
			break;
		case TS_WACK_CRES:
			mtp_set_state(mtp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			if ((err = mtp_disconnect(mtp)))
				goto free_error;
			mtp_set_state(mtp, TS_IDLE);
			break;
		default:
			/* 
			   Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a T_OPTMGMT_REQ in other than
			   TS_IDLE state. */
			break;
		}
		printd(("%s: %p: <- T_OK_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(q, mtp, prim, err);
}

/*
 *  T_BIND_ACK          17 - 
 *  -----------------------------------
 */
STATIC int
t_bind_ack(queue_t *q, struct mtp *mtp, struct mtp_addr *add)
{
	int err;
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = 0;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		if ((err = mtp_bind(mtp, add)))
			goto free_error;
		mtp_set_state(mtp, TS_IDLE);
		printd(("%s: %p: <- T_BIND_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(q, mtp, T_BIND_REQ, err);
}

/*
 *  T_UNITDATA_IND      20 - 
 *  -----------------------------------
 */
STATIC int
t_unitdata_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *src, struct mtp_opts *opt, mblk_t *dp)
{
	mblk_t *mp;
	struct T_unitdata_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_UNITDATA_IND;
		p->SRC_length = src_len;
		p->SRC_offset = src_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
		if (src_len) {
			bcopy(src, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		if (opt_len) {
			mtp_build_opts(mtp, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- T_UNITDATA_IND\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  T_UDERROR_IND       21 - Unitdata error indication
 *  -----------------------------------
 *  This primitive indicates to the MTP user that a message with the specified destination address and options
 *  produced an error.
 */
STATIC int
t_uderror_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *dst, struct mtp_opts *opt, mblk_t *dp,
	      ulong etype)
{
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	if (canput(mtp->oq)) {
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 2;	/* XXX move ahead of data indications */
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
			p->ERROR_type = etype;
			if (dst_len) {
				bcopy(dst, mp->b_wptr, dst_len);
				mp->b_wptr += dst_len;
			}
			if (opt_len) {
				mtp_build_opts(mtp, opt, mp->b_wptr);
				mp->b_wptr += opt_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: <- T_UDERROR_IND\n", DRV_NAME, mtp));
			ss7_oput(mtp->oq, mp);
			return (QR_DONE);
		}
		ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
		return (-ENOBUFS);
	}
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, mtp));
	return (-EBUSY);
}

/*
 *  T_OPTMGMT_ACK       22 - 
 *  -----------------------------------
 */
STATIC int
t_optmgmt_ack(queue_t *q, struct mtp *mtp, ulong flags, struct mtp_opts *opt)
{
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		if (opt_len) {
			mtp_build_opts(mtp, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
		}
#ifdef TS_WACK_OPTREQ
		if (mtp_get_state(mtp) == TS_WACK_OPTREQ)
			mtp_set_state(mtp, TS_IDLE);
#endif
		printd(("%s: %p: <- T_OPTMGMT_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}

/*
 *  T_ORDREL_IND        23 - 
 *  -----------------------------------
 */
STATIC int
t_ordrel_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", DRV_NAME, mtp));
	return (-EFAULT);
}

/*
 *  T_OPTDATA_IND       26 - 
 *  -----------------------------------
 */
STATIC INLINE int
t_optdata_ind(queue_t *q, struct mtp *mtp, ulong flags, struct mtp_opts *opt, mblk_t *dp)
{
	if (canput(mtp->oq)) {
		mblk_t *mp;
		struct T_optdata_ind *p;
		size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->PRIM_type = T_OPTDATA_IND;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->DATA_flag = flags;
			if (opt_len) {
				mtp_build_opts(mtp, opt, mp->b_wptr);
				mp->b_wptr += opt_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: <- T_OPTDATA_IND\n", DRV_NAME, mtp));
			ss7_oput(mtp->oq, mp);
			return (QR_ABSORBED);
		}
		ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
		return (-ENOBUFS);
	}
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, mtp));
	return (-EBUSY);
}

#ifdef T_ADDR_ACK
/*
 *  T_ADDR_ACK          27 - 
 *  -----------------------------------
 */
STATIC int
t_addr_ack(queue_t *q, struct mtp *mtp, struct mtp_addr *loc, struct mtp_addr *rem)
{
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = loc ? sizeof(*loc) : 0;
	size_t rem_len = rem ? sizeof(*rem) : 0;
	if ((mp = ss7_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = rem_len ? sizeof(*p) : 0;
		if (loc_len) {
			bcopy(loc, mp->b_wptr, loc_len);
			mp->b_wptr += loc_len;
		}
		if (rem_len) {
			bcopy(rem, mp->b_wptr, rem_len);
			mp->b_wptr += rem_len;
		}
		printd(("%s: %p: <- T_ADDR_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}
#endif

#ifdef T_CAPABILITY_ACK
/*
 *  T_CAPABILITY_ACK
 *  -----------------------------------
 */
STATIC int
t_capability_ack(queue_t *q, struct mtp *mtp, ulong caps)
{
	mblk_t *mp;
	struct T_capability_ack *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = TC1_INFO;
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (ulong) mtp->oq : 0;
		if (caps & TC1_INFO)
			p->INFO_ack = *(mtp->prot);
		else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		printd(("%s: %p: <- T_CAPABILITY_ACK\n", DRV_NAME, mtp));
		ss7_oput(mtp->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mtp));
	return (-ENOBUFS);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_pdu_req(queue_t *q, struct sl *sl, mblk_t *dp)
{
	if (canput(sl->oq)) {
		mblk_t *mp;
		sl_pdu_req_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->sl_primitive = SL_PDU_REQ;
			p->sl_mp = 0;
			mp->b_cont = dp;
			printd(("%s: %p: SL_PDU_REQ [%d] ->\n", DRV_NAME, sl, msgdsize(dp)));
			ss7_oput(sl->oq, mp);
			return (QR_ABSORBED);
		}
		ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
		return (-ENOBUFS);
	}
	ptrace(("%s: %p: ERROR: flow controlled\n", DRV_NAME, sl));
	return (-EBUSY);
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_emergency_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_emergency_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_EMERGENCY_REQ;
		printd(("%s: %p: SL_EMERGENCY_REQ ->\n", DRV_NAME, sl));
		/* 
		   does not change interface state */
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_emergency_ceases_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_emergency_ceases_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
		printd(("%s: %p: SL_EMERGENCY_CEASES_REQ ->\n", DRV_NAME, sl));
		/* 
		   does not change interface state */
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 */
STATIC int
sl_start_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_start_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_START_REQ;
		/* 
		   routing state of link does not change */
		printd(("%s: %p: SL_START_REQ ->\n", DRV_NAME, sl));
		sl_set_l_state(sl, SLS_WIND_INSI);
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 */
STATIC int
sl_stop_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_stop_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_STOP_REQ;
		printd(("%s: %p: SL_STOP_REQ ->\n", DRV_NAME, sl));
		sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
STATIC int
sl_retrieve_bsnt_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_retrieve_bsnt_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVE_BSNT_REQ;
		printd(("%s: %p: SL_RETRIEVE_BSNT_REQ ->\n", DRV_NAME, sl));
		sl_set_l_state(sl, SLS_WIND_BSNT);
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
STATIC int
sl_retrieval_request_and_fsnc_req(queue_t *q, struct sl *sl, ulong fsnc)
{
	mblk_t *mp;
	sl_retrieval_req_and_fsnc_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVAL_REQUEST_AND_FSNC_REQ;
		p->sl_fsnc = fsnc;
		printd(("%s: %p: SL_RETRIEVAL_REQUEST_AND_FSNC_REQ ->\n", DRV_NAME, sl));
		sl_set_l_state(sl, SLS_WCON_RET);
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_resume_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_resume_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RESUME_REQ;
		printd(("%s: %p: SL_RESUME_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
STATIC int
sl_clear_buffers_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_clear_buffers_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
		printd(("%s: %p: SL_CLEAR_BUFFERS_REQ ->\n", DRV_NAME, sl));
		sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
STATIC int
sl_clear_rtb_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_clear_rtb_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_CLEAR_RTB_REQ;
		printd(("%s: %p: SL_CLEAR_RTB_REQ ->\n", DRV_NAME, sl));
		sl_set_l_state(sl, SLS_WIND_CLRB);
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_local_processor_outage_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_local_proc_outage_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		printd(("%s: %p: SL_LOCAL_PROCESSOR_OUTAGE_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_congestion_discard_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_cong_discard_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
		printd(("%s: %p: SL_CONGESTION_DISCARD_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_congestion_accept_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_cong_accept_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		printd(("%s: %p: SL_CONGESTION_ACCEPT_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_no_congestion_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_no_cong_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_NO_CONGESTION_REQ;
		printd(("%s: %p: SL_NO_CONGESTION_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sl_power_on_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_power_on_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_POWER_ON_REQ;
		/* 
		   should maybe set routing state to active */
		printd(("%s: %p: SL_POWER_ON_REQ ->\n", DRV_NAME, sl));
		sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

#if 0
/*
 *  SL_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
sl_optmgmt_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_optmgmt_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_OPTMGMT_REQ;
		printd(("%s: %p: SL_OPTMGMT_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_NOTIFY_REQ
 *  -----------------------------------
 */
STATIC int
sl_notify_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_notify_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_NOTIFY_REQ;
		printd(("%s: %p: SL_NOTIFY_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}
#endif
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_info_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_info_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_INFO_REQ;
		printd(("%s: %p: LMI_INFO_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_attach_req(queue_t *q, struct sl *sl, caddr_t ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_attach_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ATTACH_REQ;
		if (ppa_ptr && ppa_len) {
			bcopy(ppa_ptr, mp->b_wptr, ppa_len);
			mp->b_wptr += ppa_len;
		}
		printd(("%s: %p: LMI_ATTACH_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_detach_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_detach_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_DETACH_REQ;
		printd(("%s: %p: LMI_DETACH_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_enable_req(queue_t *q, struct sl *sl, caddr_t dst_ptr, size_t dst_len)
{
	mblk_t *mp;
	lmi_enable_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ENABLE_REQ;
		if (dst_ptr && dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		printd(("%s: %p: LMI_ENABLE_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_disable_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_disable_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_DISABLE_REQ;
		printd(("%s: %p: LMI_DISABLE_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_optmgmt_req(queue_t *q, struct sl *sl, ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OPTMGMT_REQ;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		if (opt_ptr && opt_len) {
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		printd(("%s: %p: LMI_OPTMGMT_REQ ->\n", DRV_NAME, sl));
		ss7_oput(sl->oq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  MESSAGE ENCODING
 *
 *  =========================================================================
 */
typedef struct mtp_msg {
	mblk_t *bp;			/* identd */
	queue_t *eq;			/* queue to write errors to */
	queue_t *xq;			/* queue to write results to */
	struct mtp *mtp;		/* MTP-User to which this message belongs */
	struct sl *sl;			/* Signalling Link to which thes message belongs */
	unsigned long timestamp;	/* jiffie clock timestamp */
	ulong pvar;			/* protocol variant */
	ulong popt;			/* protocol options */
	uint mp;			/* message priority */
	uint ni;			/* network indicator */
	uint si;			/* service indicator */
	uint32_t opc;			/* originating point code */
	uint32_t dpc;			/* destination point code */
	uint sls;			/* signalling link selection */
	uint h0;			/* H0 value */
	uint h1;			/* H1 value */
	uint slc;			/* signalling link code */
	uint32_t dest;			/* destination point code */
	union {
		uint fsnl;		/* forward sequence number last acknowledged */
		uint cbc;		/* changeback code */
		uint stat;		/* status */
		uint sdli;		/* signalling data link identifier */
		uint upi;		/* user part identifier */
	} arg;
	caddr_t data;			/* user data */
	size_t dlen;			/* user data length */
} mtp_msg_t;
/*
 *  -------------------------------------------------------------------------
 *
 *  Encode message functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Encode Changeover messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +-v-------------+
 *  |0|    FSNL     |
 *  +-^-------------+
 *  ANSI Format:
 *  +---------v-----+-------v-------+
 *  |    0    |    FSNL     |  SLC  |
 *  +---------^-----+-------^-------+
 */
STATIC int
mtp_enc_com(mblk_t *mp, struct sp *sp, uint slc, uint fsnl)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = fsnl & 0x7f;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = (slc & 0x0f) | (fsnl << 4);
		*mp->b_wptr++ = (fsnl >> 4) & 0x7;
		break;
	}
	return (0);
}

/*
 *  Encode Changeback messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +---------------+
 *  |      CBC      |
 *  +---------------+
 *  ANSI Format:
 *  +-------v-------+-------v-------+
 *  |   0   |      CBC      |  SLC  |
 *  +-------^-------+-------^-------+
 */
STATIC int
mtp_enc_cbm(mblk_t *mp, struct sp *sp, uint slc, uint cbc)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = cbc;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = (slc & 0x0f) | (cbc << 4);
		*mp->b_wptr++ = (cbc >> 4) & 0x0f;
		break;
	}
	return (0);
}

/*
 *  Encode Link Management messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +
 *  | (SLC contained in SLS in RL)
 *  +
 *  ANSI Format:
 *  +-------v-------+
 *  |   0   |  SLC  |
 *  +-------^-------+
 */
STATIC int
mtp_enc_slm(mblk_t *mp, struct sp *sp, uint slc)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	default:
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = slc & 0x0f;
		break;
	}
	return (0);
}

/*
 *  Encode Trasfer Controlled messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +---v-----------+---------------+
 *  | S |          DEST             |
 *  +---^-----------+---------------+
 *  ANSI, JTTC, CHIN Format:
 *  +-----------v---+---------------+---------------+---------------+
 *  |     0     | S |                     DEST                      |
 *  +-----------^---+---------------+---------------+---------------+
 */
STATIC int
mtp_enc_tfc(mblk_t *mp, struct sp *sp, uint dest, uint stat)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ITUT:
	case SS7_PVAR_ETSI:
		*mp->b_wptr++ = dest;
		*mp->b_wptr++ = ((dest >> 8) & 0x3f) | (stat << 6);
		break;
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		*mp->b_wptr++ = dest;
		*mp->b_wptr++ = (dest >> 8);
		*mp->b_wptr++ = (dest >> 16);
		*mp->b_wptr++ = (stat & 0x3);
		break;
	}
	return (0);
}

/*
 *  Encode Traffic Flow Control and Test messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +---v-----------+---------------+
 *  | 0 |          DEST             |
 *  +---^-----------+---------------+
 *  ANSI, JTTC, CHIN Format:
 *  +---------------+---------------+---------------+
 *  |                     DEST                      |
 *  +---------------+---------------+---------------+
 */
STATIC int
mtp_enc_tfm(mblk_t *mp, struct sp *sp, uint dest)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ITUT:
	case SS7_PVAR_ETSI:
		*mp->b_wptr++ = dest;
		*mp->b_wptr++ = (dest >> 8) & 0x3f;
		break;
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		*mp->b_wptr++ = dest;
		*mp->b_wptr++ = (dest >> 8);
		*mp->b_wptr++ = (dest >> 16);
		break;
	}
	return (0);
}

/*
 *  Encode Data Link Connection message
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format: (12-bit SDLI)
 *  +-------v-------+---------------+
 *  |   0   |          SDLI         |
 *  +-------^-------+---------------+
 *  ANSI Format: (14-bit SDLI)
 *  +-----------v---+---------------+-------v-------+
 *  |     0     |           SDLI            |  SLC  |
 *  +-----------^---+---------------+-------^-------+
 */
STATIC int
mtp_enc_dlc(mblk_t *mp, struct sp *sp, uint slc, uint sdli)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = sdli;
		*mp->b_wptr++ = (sdli >> 8) & 0x0f;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = (slc & 0x0f) | (sdli << 4);
		*mp->b_wptr++ = sdli >> 4;
		*mp->b_wptr++ = (sdli >> 12) & 0x03;
		break;
	}
	return (0);
}

/*
 *  Encode User Part Flow Control messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +-------v-------+---v-----------+---------------+
 *  |   0   |  UPI  | 0 |          DEST             |
 *  +-------^-------+---^-----------+---------------+
 *  ANSI, JTTC, CHIN Format:
 *  +-------v-------+---------------+---------------+---------------+
 *  |   0   |  UPI  |                     DEST                      |
 *  +-------^-------+---------------+---------------+---------------+
 *
 */
STATIC int
mtp_enc_upm(mblk_t *mp, struct sp *sp, uint dest, uint upi)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = dest;
		*mp->b_wptr++ = (dest >> 8) & 0x3f;
		*mp->b_wptr++ = upi & 0x0f;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = dest;
		*mp->b_wptr++ = dest >> 8;
		*mp->b_wptr++ = dest >> 16;
		*mp->b_wptr++ = upi & 0x0f;
		break;
	}
	return (0);
}

/*
 *  Encode Signalling Link Test Messages/Acknowledgements
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  ------------------------+-------v-------+
 *   ...   Test Data        |  TLI  |   0   |
 *  ------------------------+-------^-------+
 *  ANSI Format:
 *  ------------------------+-------v-------+
 *   ...   Test Data        |  TLI  |  SLC  |
 *  ------------------------+-------^-------+
 */
STATIC int
mtp_enc_sltm(mblk_t *mp, struct sp *sp, uint slc, size_t dlen, uchar *data)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = dlen << 4;
		bcopy(data, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = (slc & 0x0f) | (dlen << 4);
		bcopy(data, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		break;
	}
	return (0);
}

/*
 *  Encode User Part
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  All Formats:
 *  ------------------------+
 *   ...   User Part Data   |
 *  ------------------------+
 */
STATIC int
mtp_enc_user(mblk_t *mp, mblk_t *dp)
{
	mp->b_cont = dp;
	return (0);
}

/*
 *  Encode Service Information Octet
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +---v---v-------+---v-----------+
 *  | NI| 0 |  SI   | 0 |           |
 *  +---^---^-------+---^-----------+
 *  ANSI Format:
 *  +---v---v-------+---v-----------+
 *  | NI| MP|  SI   | 0 |           |
 *  +---^---^-------+---^-----------+
 *  JTTC Format:
 *  +---v---v-------+---v-----------+
 *  | NI| 0 |  SI   | MP|           |
 *  +---^---^-------+---^-----------+
 */
STATIC void
mtp_enc_sio(mblk_t *bp, struct sp *sp, uint mp, uint ni, uint si)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_JTTC:
		bp->b_wptr[-1] = mp << 6;	/* put message priority in header */
		*bp->b_wptr++ = (si & 0x0f) | ((ni & 0x3) << 6);
		break;
	case SS7_PVAR_ANSI:
		*bp->b_wptr++ = (si & 0x0f) | ((mp & 0x3) << 4) | ((ni & 0x3) << 6);
		break;
	default:
		if (sp->na.na->option.popt & SS7_POPT_MPLEV)
			*bp->b_wptr++ = (si & 0x0f) | ((mp & 0x3) << 4) | ((ni & 0x3) << 6);
		else
			*bp->b_wptr++ = (si & 0x0f) | ((ni & 0x3) << 6);
		break;
	}
}

/*
 *  Encode Routing Label
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format: (14-bit PC, 4-bit SLS)
 *  +-------v-------+---------------+---v-----------+---------------+
 *  |  SLS  |            OPC            |            DPC            |
 *  +-------^-------+---------------+---^-----------+---------------+
 *  ANSI, JTTC, CHIN Format: (24-bit PC, 5-bit SLS)
 *  +-----v---------+---------------+---------------+---------------+---------------+---------------+---------------+
 *  |     |   SLS   |                      OPC                      |                     DPC                       |
 *  +-----^---------+---------------+---------------+---------------+---------------+---------------+---------------+
 *  ANSI Format: (24-bit PC, 8-bit SLS)
 *  +---------------+---------------+---------------+---------------+---------------+---------------+---------------+
 *  |      SLS      |                      OPC                      |                     DPC                       |
 *  +---------------+---------------+---------------+---------------+---------------+---------------+---------------+
 *
 */
STATIC void
mtp_enc_rl(mblk_t *mp, struct sp *sp, uint dpc, uint opc, uint sls)
{
	switch (sp->na.na->option.pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ETSI:
	case SS7_PVAR_ITUT:
		*mp->b_wptr++ = dpc;
		*mp->b_wptr++ = ((dpc >> 8) & 0x3f) | (opc << 6);
		*mp->b_wptr++ = (opc >> 2);
		*mp->b_wptr++ = ((opc >> 10) & 0x0f) | (sls << 4);
		break;
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		*mp->b_wptr++ = dpc;
		*mp->b_wptr++ = dpc >> 8;
		*mp->b_wptr++ = dpc >> 16;
		*mp->b_wptr++ = opc;
		*mp->b_wptr++ = opc >> 8;
		*mp->b_wptr++ = opc >> 16;
		*mp->b_wptr++ = sls;
		break;
	}
}

/*
 *  Encode Service Information Field
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  All Formats:
 *  --------------------+-------v-------+
 *         signal       |   H1  |  H0   |
 *  --------------------+-------^-------+
 */
STATIC INLINE void
mtp_enc_sif(mblk_t *mp, struct sp *sp, const uint h0, const uint h1)
{
	(void) sp;
	*mp->b_wptr++ = (h0 & 0x0f) | ((h1 & 0x0f) << 4);
}

#define max_msg_size (6 + 1 + 7 + 1 + 3 + 1)
STATIC mblk_t *
mtp_enc_msg(queue_t *q, struct sp *sp, uint ni, uint mp, uint si, uint dpc, uint opc, uint sls)
{
	mblk_t *bp;
	if ((bp = ss7_allocb(q, max_msg_size, BPRI_MED))) {
		bp->b_datap->db_type = M_DATA;
		bzero(bp->b_rptr, 6);	/* zero header */
		bp->b_rptr += 6;	/* reserve header room for Level 2 */
		bp->b_wptr = bp->b_rptr;
		mtp_enc_sio(bp, sp, mp, ni, si);
		mtp_enc_rl(bp, sp, dpc, opc, sls);
	}
	return (bp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timers
 *
 *  -------------------------------------------------------------------------
 */
enum { tall, t1, t1r, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14,
	t15, t16, t17, t18, t18a, t19, t19a, t20, t20a, t21, t21a, t22, t22a,
	t23, t23a, t24, t24a, t25a, t26a, t27a, t28a, t29a, t30a, t31a, t32a,
	t33a, t34a, t1t, t2t, t1s
};

STATIC mtp_opt_conf_na_t itut_na_config_default = {
	t1:((800 * HZ) / 1000),		/* (sl) T1 0.5 (0.8) 1.2 sec */
	t1r:((800 * HZ) / 1000),	/* (sp) T1R 0.5 (0.8) 1.2 sec */
	t2:((1400 * HZ) / 1000),	/* (sl) T2 0.7 (1.4) 2.0 sec */
	t3:((800 * HZ) / 1000),		/* (sl) T3 0.5 (0.8) 1.2 sec */
	t4:((800 * HZ) / 1000),		/* (sl) T4 0.5 (0.8) 1.2 sec */
	t5:((800 * HZ) / 1000),		/* (sl) T5 0.5 (0.8) 1.2 sec */
	t6:((800 * HZ) / 1000),		/* (rt) T6 0.5 (0.8) 1.2 sec */
	t7:((1500 * HZ) / 1000),	/* (lk) T7 1 to 2 sec */
	t8:((1000 * HZ) / 1000),	/* (rs) T8 0.8 to 1.2 sec */
	t10:(45 * HZ),			/* (rt) T10 30 to 60 sec */
	t11:(60 * HZ),			/* (rs) T11 30 to 90 sec */
	t12:((1150 * HZ) / 1000),	/* (sl) T12 0.8 to 1.5 sec */
	t13:((1150 * HZ) / 1000),	/* (sl) T13 0.8 to 1.5 sec */
	t14:((2500 * HZ) / 1000),	/* (sl) T14 2 to 3 sec */
	t15:((2500 * HZ) / 1000),	/* (rs) T15 2 to 3 sec */
	t16:((1700 * HZ) / 1000),	/* (rs) T16 1.4 to 2.0 sec */
	t17:((1150 * HZ) / 1000),	/* (sl) T17 0.8 to 1.5 sec */
	t18:(600 * HZ),			/* (sp) T18 network dependent */
	t18a:(12 * HZ),			/* (rs) T18A 2 to 20 sec */
	t19:(68 * HZ),			/* (sp) T19 67 to 69 sec */
	t19a:(540 * HZ),		/* (sl) T19A 480 to 600 sec */
	t20:(60 * HZ),			/* (sp) T20 59 to 61 sec */
	t20a:(105 * HZ),		/* (sl) T20A 90 to 120 sec */
	t21:(64 * HZ),			/* (sp) T21 63 to 65 sec */
	t21a:(105 * HZ),		/* (sl) T21A 90 to 120 sec */
	t22:(270 * HZ),			/* (sl) T22 3 to 6 min */
	t22a:(600 * HZ),		/* (sp) T22A network dependent */
	t23:(270 * HZ),			/* (sl) T23 3 to 6 min */
	t23a:(600 * HZ),		/* (sp) T23A network dependent */
	t24:((500 * HZ) / 1000),	/* (sl) T24 500 ms */
	t24a:(600 * HZ),		/* (sp) T24A network dependent */
	t25a:(32 * HZ),			/* (sp) T25 32 to 35 sec */
	t26a:(13 * HZ),			/* (sp) T26 12 to 15 sec */
	t27a:(3 * HZ),			/* (sp) T27 2 (3) 5 sec */
	t28a:(19 * HZ),			/* (sp) T28 3 to 35 sec */
	t29a:(63 * HZ),			/* (sp) T29 60 to 65 sec */
	t30a:(33 * HZ),			/* (sp) T30 30 to 35 sec */
	t31a:(60 * HZ),			/* (sl) T31 10 to 120 sec */
	t32a:(60 * HZ),			/* (sl) T32 5 to 120 sec */
	t33a:(360 * HZ),		/* (sl) T33 60 to 600 sec */
	t34a:(60 * HZ),			/* (sl) T34 5 to 120 sec */
	t1t:(4 * HZ),			/* (sl) T1T 4 to 12 sec */
	t2t:(60 * HZ),			/* (sl) T2T 30 to 90 sec */
	t1s:(4 * HZ),			/* (sl) T1S 4 to 12 sec */
};

STATIC mtp_opt_conf_na_t etsi_na_config_default = {
	t1:((800 * HZ) / 1000),		/* (sl) T1 0.5 (0.8) 1.2 sec */
	t1r:((800 * HZ) / 1000),	/* (sp) T1R 0.5 (0.8) 1.2 sec */
	t2:((1400 * HZ) / 1000),	/* (sl) T2 0.7 (1.4) 2.0 sec */
	t3:((800 * HZ) / 1000),		/* (sl) T3 0.5 (0.8) 1.2 sec */
	t4:((800 * HZ) / 1000),		/* (sl) T4 0.5 (0.8) 1.2 sec */
	t5:((800 * HZ) / 1000),		/* (sl) T5 0.5 (0.8) 1.2 sec */
	t6:((800 * HZ) / 1000),		/* (rt) T6 0.5 (0.8) 1.2 sec */
	t7:((1500 * HZ) / 1000),	/* (lk) T7 1 to 2 sec */
	t8:((1000 * HZ) / 1000),	/* (rs) T8 0.8 to 1.2 sec */
	t10:(45 * HZ),			/* (rt) T10 30 to 60 sec */
	t11:(60 * HZ),			/* (rs) T11 30 to 90 sec */
	t12:((1150 * HZ) / 1000),	/* (sl) T12 0.8 to 1.5 sec */
	t13:((1150 * HZ) / 1000),	/* (sl) T13 0.8 to 1.5 sec */
	t14:((2500 * HZ) / 1000),	/* (sl) T14 2 to 3 sec */
	t15:((2500 * HZ) / 1000),	/* (rs) T15 2 to 3 sec */
	t16:((1700 * HZ) / 1000),	/* (rs) T16 1.4 to 2.0 sec */
	t17:((1150 * HZ) / 1000),	/* (sl) T17 0.8 to 1.5 sec */
	t18:(600 * HZ),			/* (sp) T18 network dependent */
	t18a:(12 * HZ),			/* (rs) T18A 2 to 20 sec */
	t19:(68 * HZ),			/* (sp) T19 67 to 69 sec */
	t19a:(540 * HZ),		/* (sl) T19A 480 to 600 sec */
	t20:(60 * HZ),			/* (sp) T20 59 to 61 sec */
	t20a:(105 * HZ),		/* (sl) T20A 90 to 120 sec */
	t21:(64 * HZ),			/* (sp) T21 63 to 65 sec */
	t21a:(105 * HZ),		/* (sl) T21A 90 to 120 sec */
	t22:(270 * HZ),			/* (sl) T22 3 to 6 min */
	t22a:(600 * HZ),		/* (sp) T22A network dependent */
	t23:(270 * HZ),			/* (sl) T23 3 to 6 min */
	t23a:(600 * HZ),		/* (sp) T23A network dependent */
	t24:((500 * HZ) / 1000),	/* (sl) T24 500 ms */
	t24a:(600 * HZ),		/* (sp) T24A network dependent */
	t25a:(32 * HZ),			/* (sp) T25 32 to 35 sec */
	t26a:(13 * HZ),			/* (sp) T26 12 to 15 sec */
	t27a:(3 * HZ),			/* (sp) T27 2 (3) 5 sec */
	t28a:(19 * HZ),			/* (sp) T28 3 to 35 sec */
	t29a:(63 * HZ),			/* (sp) T29 60 to 65 sec */
	t30a:(33 * HZ),			/* (sp) T30 30 to 35 sec */
	t31a:(60 * HZ),			/* (sl) T31 10 to 120 sec */
	t32a:(60 * HZ),			/* (sl) T32 5 to 120 sec */
	t33a:(360 * HZ),		/* (sl) T33 60 to 600 sec */
	t34a:(60 * HZ),			/* (sl) T34 5 to 120 sec */
	t1t:(4 * HZ),			/* (sl) T1T 4 to 12 sec */
	t2t:(60 * HZ),			/* (sl) T2T 30 to 90 sec */
	t1s:(4 * HZ),			/* (sl) T1S 4 to 12 sec */
};

STATIC mtp_opt_conf_na_t ansi_na_config_default = {
	t1:((800 * HZ) / 1000),		/* (sl) T1 0.5 (0.8) 1.2 sec */
	t1r:((800 * HZ) / 1000),	/* (sp) T1R 0.5 (0.8) 1.2 sec */
	t2:((1400 * HZ) / 1000),	/* (sl) T2 0.7 (1.4) 2.0 sec */
	t3:((800 * HZ) / 1000),		/* (sl) T3 0.5 (0.8) 1.2 sec */
	t4:((800 * HZ) / 1000),		/* (sl) T4 0.5 (0.8) 1.2 sec */
	t5:((800 * HZ) / 1000),		/* (sl) T5 0.5 (0.8) 1.2 sec */
	t6:((800 * HZ) / 1000),		/* (rt) T6 0.5 (0.8) 1.2 sec */
	t7:((1500 * HZ) / 1000),	/* (lk) T7 1 to 2 sec */
	t8:((1000 * HZ) / 1000),	/* (rs) T8 0.8 to 1.2 sec */
	t10:(45 * HZ),			/* (rt) T10 30 to 60 sec */
	t11:(60 * HZ),			/* (rs) T11 30 to 90 sec */
	t12:((1150 * HZ) / 1000),	/* (sl) T12 0.8 to 1.5 sec */
	t13:((1150 * HZ) / 1000),	/* (sl) T13 0.8 to 1.5 sec */
	t14:((2500 * HZ) / 1000),	/* (sl) T14 2 to 3 sec */
	t15:((2500 * HZ) / 1000),	/* (rs) T15 2 to 3 sec */
	t16:((1700 * HZ) / 1000),	/* (rs) T16 1.4 to 2.0 sec */
	t17:((1150 * HZ) / 1000),	/* (sl) T17 0.8 to 1.5 sec */
	t18:(600 * HZ),			/* (sp) T18 network dependent */
	t18a:(12 * HZ),			/* (rs) T18A 2 to 20 sec */
	t19:(68 * HZ),			/* (sp) T19 67 to 69 sec */
	t19a:(540 * HZ),		/* (sl) T19A 480 to 600 sec */
	t20:(60 * HZ),			/* (sp) T20 59 to 61 sec */
	t20a:(105 * HZ),		/* (sl) T20A 90 to 120 sec */
	t21:(64 * HZ),			/* (sp) T21 63 to 65 sec */
	t21a:(105 * HZ),		/* (sl) T21A 90 to 120 sec */
	t22:(270 * HZ),			/* (sl) T22 3 to 6 min */
	t22a:(600 * HZ),		/* (sp) T22A network dependent */
	t23:(270 * HZ),			/* (sl) T23 3 to 6 min */
	t23a:(600 * HZ),		/* (sp) T23A network dependent */
	t24:((500 * HZ) / 1000),	/* (sl) T24 500 ms */
	t24a:(600 * HZ),		/* (sp) T24A network dependent */
	t25a:(32 * HZ),			/* (sp) T25 32 to 35 sec */
	t26a:(13 * HZ),			/* (sp) T26 12 to 15 sec */
	t27a:(3 * HZ),			/* (sp) T27 2 (3) 5 sec */
	t28a:(19 * HZ),			/* (sp) T28 3 to 35 sec */
	t29a:(63 * HZ),			/* (sp) T29 60 to 65 sec */
	t30a:(33 * HZ),			/* (sp) T30 30 to 35 sec */
	t31a:(60 * HZ),			/* (sl) T31 10 to 120 sec */
	t32a:(60 * HZ),			/* (sl) T32 5 to 120 sec */
	t33a:(360 * HZ),		/* (sl) T33 60 to 600 sec */
	t34a:(60 * HZ),			/* (sl) T34 5 to 120 sec */
	t1t:(4 * HZ),			/* (sl) T1T 4 to 12 sec */
	t2t:(60 * HZ),			/* (sl) T2T 30 to 90 sec */
	t1s:(4 * HZ),			/* (sl) T1S 4 to 12 sec */
};

STATIC mtp_opt_conf_na_t jttc_na_config_default = {
	t1:((800 * HZ) / 1000),		/* (sl) T1 0.5 (0.8) 1.2 sec */
	t1r:((800 * HZ) / 1000),	/* (sp) T1R 0.5 (0.8) 1.2 sec */
	t2:((1400 * HZ) / 1000),	/* (sl) T2 0.7 (1.4) 2.0 sec */
	t3:((800 * HZ) / 1000),		/* (sl) T3 0.5 (0.8) 1.2 sec */
	t4:((800 * HZ) / 1000),		/* (sl) T4 0.5 (0.8) 1.2 sec */
	t5:((800 * HZ) / 1000),		/* (sl) T5 0.5 (0.8) 1.2 sec */
	t6:((800 * HZ) / 1000),		/* (rt) T6 0.5 (0.8) 1.2 sec */
	t7:((1500 * HZ) / 1000),	/* (lk) T7 1 to 2 sec */
	t8:((1000 * HZ) / 1000),	/* (rs) T8 0.8 to 1.2 sec */
	t10:(45 * HZ),			/* (rt) T10 30 to 60 sec */
	t11:(60 * HZ),			/* (rs) T11 30 to 90 sec */
	t12:((1150 * HZ) / 1000),	/* (sl) T12 0.8 to 1.5 sec */
	t13:((1150 * HZ) / 1000),	/* (sl) T13 0.8 to 1.5 sec */
	t14:((2500 * HZ) / 1000),	/* (sl) T14 2 to 3 sec */
	t15:((2500 * HZ) / 1000),	/* (rs) T15 2 to 3 sec */
	t16:((1700 * HZ) / 1000),	/* (rs) T16 1.4 to 2.0 sec */
	t17:((1150 * HZ) / 1000),	/* (sl) T17 0.8 to 1.5 sec */
	t18:(600 * HZ),			/* (sp) T18 network dependent */
	t18a:(12 * HZ),			/* (rs) T18A 2 to 20 sec */
	t19:(68 * HZ),			/* (sp) T19 67 to 69 sec */
	t19a:(540 * HZ),		/* (sl) T19A 480 to 600 sec */
	t20:(60 * HZ),			/* (sp) T20 59 to 61 sec */
	t20a:(105 * HZ),		/* (sl) T20A 90 to 120 sec */
	t21:(64 * HZ),			/* (sp) T21 63 to 65 sec */
	t21a:(105 * HZ),		/* (sl) T21A 90 to 120 sec */
	t22:(270 * HZ),			/* (sl) T22 3 to 6 min */
	t22a:(600 * HZ),		/* (sp) T22A network dependent */
	t23:(270 * HZ),			/* (sl) T23 3 to 6 min */
	t23a:(600 * HZ),		/* (sp) T23A network dependent */
	t24:((500 * HZ) / 1000),	/* (sl) T24 500 ms */
	t24a:(600 * HZ),		/* (sp) T24A network dependent */
	t25a:(32 * HZ),			/* (sp) T25 32 to 35 sec */
	t26a:(13 * HZ),			/* (sp) T26 12 to 15 sec */
	t27a:(3 * HZ),			/* (sp) T27 2 (3) 5 sec */
	t28a:(19 * HZ),			/* (sp) T28 3 to 35 sec */
	t29a:(63 * HZ),			/* (sp) T29 60 to 65 sec */
	t30a:(33 * HZ),			/* (sp) T30 30 to 35 sec */
	t31a:(60 * HZ),			/* (sl) T31 10 to 120 sec */
	t32a:(60 * HZ),			/* (sl) T32 5 to 120 sec */
	t33a:(360 * HZ),		/* (sl) T33 60 to 600 sec */
	t34a:(60 * HZ),			/* (sl) T34 5 to 120 sec */
	t1t:(4 * HZ),			/* (sl) T1T 4 to 12 sec */
	t2t:(60 * HZ),			/* (sl) T2T 30 to 90 sec */
	t1s:(4 * HZ),			/* (sl) T1S 4 to 12 sec */
};

/*
 *  -------------------------------------------------------------------------
 *
 *  MTP timers
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
__mtp_timer_stop(struct mtp *mtp, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
mtp_timer_stop(struct mtp *mtp, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&mtp->lock, flags);
	{
		__mtp_timer_stop(mtp, t);
	}
	spin_unlock_irqrestore(&mtp->lock, flags);
}
STATIC INLINE void
mtp_timer_start(struct mtp *mtp, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&mtp->qlock, flags);
	{
		__mtp_timer_stop(mtp, t);
		switch (t) {
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&mtp->qlock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  RS timers
 *
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, rs, t8, config);	/* transfer prohibited inhibitition timer */
SS7_DECLARE_TIMER(DRV_NAME, rs, t11, config);	/* waiting to begin transfer restricted */
SS7_DECLARE_TIMER(DRV_NAME, rs, t15, config);	/* waiting to start routeset congestion test */
SS7_DECLARE_TIMER(DRV_NAME, rs, t16, config);	/* waiting for routeset congestion status update */
SS7_DECLARE_TIMER(DRV_NAME, rs, t18a, config);

STATIC INLINE void
__rs_timer_stop(struct rs *rs, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t8:
		rs_stop_timer_t8(rs);
		if (single)
			break;
		/* 
		   fall through */
	case t11:
		rs_stop_timer_t11(rs);
		if (single)
			break;
		/* 
		   fall through */
	case t15:
		rs_stop_timer_t15(rs);
		if (single)
			break;
		/* 
		   fall through */
	case t16:
		rs_stop_timer_t16(rs);
		if (single)
			break;
		/* 
		   fall through */
	case t18a:
		rs_stop_timer_t18a(rs);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
rs_timer_stop(struct rs *rs, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&rs->lock, flags);
	{
		__rs_timer_stop(rs, t);
	}
	spin_unlock_irqrestore(&rs->lock, flags);
}
STATIC INLINE void
rs_timer_start(struct rs *rs, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&rs->lock, flags);
	{
		__rs_timer_stop(rs, t);
		switch (t) {
		case t8:
			rs_start_timer_t8(rs);
			break;
		case t11:
			rs_start_timer_t11(rs);
			break;
		case t15:
			rs_start_timer_t15(rs);
			break;
		case t16:
			rs_start_timer_t16(rs);
			break;
		case t18a:
			rs_start_timer_t18a(rs);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&rs->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CR timers
 *
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, cr, t6, rt.onto->config);

STATIC INLINE void
__cr_timer_stop(struct cr *cr, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t6:
		cr_stop_timer_t6(cr);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
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
		case t6:
			cr_start_timer_t6(cr);
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
 *  RT timers
 *
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, rt, t10, config);	/* waiting to repeat RST message */

STATIC INLINE void
__rt_timer_stop(struct rt *rt, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t10:
		rt_stop_timer_t10(rt);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
rt_timer_stop(struct rt *rt, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&rt->lock, flags);
	{
		__rt_timer_stop(rt, t);
	}
	spin_unlock_irqrestore(&rt->lock, flags);
}
STATIC INLINE void
rt_timer_start(struct rt *rt, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&rt->lock, flags);
	{
		__rt_timer_stop(rt, t);
		switch (t) {
		case t10:
			rt_start_timer_t10(rt);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&rt->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SP timers
 *
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, sp, t1r, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t18, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t19, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t20, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t21, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t22a, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t23a, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t24a, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t25a, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t26a, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t27a, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t28a, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t29a, config);	/* MTP restart timer */
SS7_DECLARE_TIMER(DRV_NAME, sp, t30a, config);	/* MTP restart timer */

STATIC INLINE void
__sp_timer_stop(struct sp *sp, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t1r:
		sp_stop_timer_t1r(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t18:
		sp_stop_timer_t18(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t19:
		sp_stop_timer_t19(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t20:
		sp_stop_timer_t20(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t21:
		sp_stop_timer_t21(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t22a:
		sp_stop_timer_t22a(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t23a:
		sp_stop_timer_t23a(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t24a:
		sp_stop_timer_t24a(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t25a:
		sp_stop_timer_t25a(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t26a:
		sp_stop_timer_t26a(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t27a:
		sp_stop_timer_t27a(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t28a:
		sp_stop_timer_t28a(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t29a:
		sp_stop_timer_t29a(sp);
		if (single)
			break;
		/* 
		   fall through */
	case t30a:
		sp_stop_timer_t30a(sp);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sp_timer_stop(struct sp *sp, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&sp->lock, flags);
	{
		__sp_timer_stop(sp, t);
	}
	spin_unlock_irqrestore(&sp->lock, flags);
}
STATIC INLINE void
sp_timer_start(struct sp *sp, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&sp->lock, flags);
	{
		__sp_timer_stop(sp, t);
		switch (t) {
		case t1r:
			sp_start_timer_t1r(sp);
			break;
		case t18:
			sp_start_timer_t18(sp);
			break;
		case t19:
			sp_start_timer_t19(sp);
			break;
		case t20:
			sp_start_timer_t20(sp);
			break;
		case t21:
			sp_start_timer_t21(sp);
			break;
		case t22a:
			sp_start_timer_t22a(sp);
			break;
		case t23a:
			sp_start_timer_t23a(sp);
			break;
		case t24a:
			sp_start_timer_t24a(sp);
			break;
		case t25a:
			sp_start_timer_t25a(sp);
			break;
		case t26a:
			sp_start_timer_t26a(sp);
			break;
		case t27a:
			sp_start_timer_t27a(sp);
			break;
		case t28a:
			sp_start_timer_t28a(sp);
			break;
		case t29a:
			sp_start_timer_t29a(sp);
			break;
		case t30a:
			sp_start_timer_t30a(sp);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&sp->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CB timers
 *
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, cb, t1, sl.from->config);
SS7_DECLARE_TIMER(DRV_NAME, cb, t2, sl.from->config);
SS7_DECLARE_TIMER(DRV_NAME, cb, t3, sl.from->config);
SS7_DECLARE_TIMER(DRV_NAME, cb, t4, sl.from->config);
SS7_DECLARE_TIMER(DRV_NAME, cb, t5, sl.from->config);

STATIC INLINE void
__cb_timer_stop(struct cb *cb, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t1:
		cb_stop_timer_t1(cb);
		if (single)
			break;
		/* 
		   fall through */
	case t2:
		cb_stop_timer_t2(cb);
		if (single)
			break;
		/* 
		   fall through */
	case t3:
		cb_stop_timer_t3(cb);
		if (single)
			break;
		/* 
		   fall through */
	case t4:
		cb_stop_timer_t4(cb);
		if (single)
			break;
		/* 
		   fall through */
	case t5:
		cb_stop_timer_t5(cb);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
cb_timer_stop(struct cb *cb, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&cb->lock, flags);
	{
		__cb_timer_stop(cb, t);
	}
	spin_unlock_irqrestore(&cb->lock, flags);
}
STATIC INLINE void
cb_timer_start(struct cb *cb, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&cb->lock, flags);
	{
		__cb_timer_stop(cb, t);
		switch (t) {
		case t1:
			cb_start_timer_t1(cb);
			break;
		case t2:
			cb_start_timer_t2(cb);
			break;
		case t3:
			cb_start_timer_t3(cb);
			break;
		case t4:
			cb_start_timer_t4(cb);
			break;
		case t5:
			cb_start_timer_t5(cb);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&cb->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LS timers
 *
 *  -------------------------------------------------------------------------
 */

STATIC INLINE void
__ls_timer_stop(struct ls *ls, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
ls_timer_stop(struct ls *ls, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&ls->lock, flags);
	{
		__ls_timer_stop(ls, t);
	}
	spin_unlock_irqrestore(&ls->lock, flags);
}
STATIC INLINE void
ls_timer_start(struct ls *ls, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&ls->lock, flags);
	{
		ls_timer_stop(ls, t);
		switch (t) {
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&ls->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LK timers
 *
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, lk, t7, config);

STATIC INLINE void
__lk_timer_stop(struct lk *lk, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t7:
		lk_stop_timer_t7(lk);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
lk_timer_stop(struct lk *lk, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&lk->lock, flags);
	{
		__lk_timer_stop(lk, t);
	}
	spin_unlock_irqrestore(&lk->lock, flags);
}
STATIC INLINE void
lk_timer_start(struct lk *lk, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&lk->lock, flags);
	{
		__lk_timer_stop(lk, t);
		switch (t) {
		case t7:
			lk_start_timer_t7(lk);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&lk->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL timers
 *
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DRV_NAME, sl, t12, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t13, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t14, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t17, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t19a, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t20a, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t21a, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t22, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t23, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t24, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t31a, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t32a, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t33a, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t34a, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t1t, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t2t, config);
SS7_DECLARE_TIMER(DRV_NAME, sl, t1s, config);

STATIC INLINE void
__sl_timer_stop(struct sl *sl, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t12:
		sl_stop_timer_t12(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t13:
		sl_stop_timer_t13(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t14:
		sl_stop_timer_t14(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t17:
		sl_stop_timer_t17(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t19a:
		sl_stop_timer_t19a(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t20a:
		sl_stop_timer_t20a(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t21a:
		sl_stop_timer_t21a(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t22:
		sl_stop_timer_t22(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t23:
		sl_stop_timer_t23(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t24:
		sl_stop_timer_t24(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t31a:
		sl_stop_timer_t31a(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t32a:
		sl_stop_timer_t32a(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t33a:
		sl_stop_timer_t33a(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t34a:
		sl_stop_timer_t34a(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t1t:
		sl_stop_timer_t1t(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t2t:
		sl_stop_timer_t2t(sl);
		if (single)
			break;
		/* 
		   fall through */
	case t1s:
		sl_stop_timer_t1s(sl);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sl_timer_stop(struct sl *sl, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	{
		__sl_timer_stop(sl, t);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
}
STATIC INLINE void
sl_timer_start(struct sl *sl, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&sl->lock, flags);
	{
		__sl_timer_stop(sl, t);
		switch (t) {
		case t12:
			sl_start_timer_t12(sl);
			break;
		case t13:
			sl_start_timer_t13(sl);
			break;
		case t14:
			sl_start_timer_t14(sl);
			break;
		case t17:
			sl_start_timer_t17(sl);
			break;
		case t19a:
			sl_start_timer_t19a(sl);
			break;
		case t20a:
			sl_start_timer_t20a(sl);
			break;
		case t21a:
			sl_start_timer_t21a(sl);
			break;
		case t22:
			sl_start_timer_t22(sl);
			break;
		case t23:
			sl_start_timer_t23(sl);
			break;
		case t24:
			sl_start_timer_t24(sl);
			break;
		case t31a:
			sl_start_timer_t31a(sl);
			break;
		case t32a:
			sl_start_timer_t32a(sl);
			break;
		case t33a:
			sl_start_timer_t33a(sl);
			break;
		case t34a:
			sl_start_timer_t34a(sl);
			break;
		case t1t:
			sl_start_timer_t1t(sl);
			break;
		case t2t:
			sl_start_timer_t2t(sl);
			break;
		case t1s:
			sl_start_timer_t1s(sl);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&sl->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  MTP User procedures
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP-STATUS-IND
 *  -----------------------------------
 *  Indicate to an MTP User an MTP-STATUS-IND with respect to the concerned routesets.
 */
STATIC INLINE int
mtp_status_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *dst, int status)
{
	switch (mtp->i_style) {
	case MTP_STYLE_MTPI:
	case MTP_STYLE_MGMT:
		switch (status) {
		default:
			return m_status_ind(q, mtp, dst, NULL, NULL, status);
		case T_MTP_PROHIBITED:
			return m_pause_ind(q, mtp, dst);
		case T_MTP_AVAILABLE:
			return m_resume_ind(q, mtp, dst);
		}
		break;
	case MTP_STYLE_TPI:
		switch (mtp->prot->SERV_type) {
		case T_CLTS:
			return t_uderror_ind(q, mtp, dst, NULL, NULL, status);
		case T_COTS:
		case T_COTS_ORD:
			fixme(("We should send an optdata indication with null data instead\n"));
			return t_uderror_ind(q, mtp, dst, NULL, NULL, status);
		}
		break;
	case MTP_STYLE_NPI:
		switch (mtp->prot->SERV_type) {
		case T_CLTS:	/* N_CLNS */
			return n_uderror_ind(q, mtp, dst, NULL, NULL, status);
		case T_COTS:	/* N_CONS */
		case T_COTS_ORD:
			todo(("We should actually set the originator properly\n"));
			return n_reset_ind(q, mtp, N_UNDEFINED, status);
		}
		break;
	}
	swerr();
	return (-EFAULT);
}
STATIC int
mtp_up_status_ind(queue_t *q, struct mtp *mtp, struct rs *rs, uint user, uint status)
{
	struct mtp_addr dest = mtp->src;
	uint error;
	dest.pc = rs->dest;
	dest.si = user;
	switch (status) {
	case 0x0:
		error = T_MTP_USER_PART_UNEQUIPPED;
		break;
	case 0x1:
		error = T_MTP_USER_PART_UNAVAILABLE;
		break;
	default:
		error = T_MTP_USER_PART_UNKNOWN;
		break;
	}
	return mtp_status_ind(q, mtp, &dest, error);
}
STATIC INLINE int
mtp_up_status_ind_all_local(queue_t *q, struct rs *rs, uint si, uint status)
{
	struct mtp *mtp;
	int err;
	for (mtp = rs->sp.sp->mtp.lists[si & 0xf]; mtp; mtp = mtp->sp.next)
		if ((err = mtp_up_status_ind(q, mtp, rs, si, status)))
			return (err);
	return (0);
}
STATIC int
mtp_cong_status_ind(queue_t *q, struct mtp *mtp, struct rs *rs, uint newstatus)
{
	struct mtp_addr dest = mtp->src;
	uint error = T_MTP_CONGESTED(newstatus);
	dest.pc = rs->dest;
	switch (rs->rs_type) {
	default:
		swerr();
	case RT_TYPE_MEMBER:
		error |= T_MTP_M_MEMBER;
		break;
	case RT_TYPE_CLUSTER:
		error |= T_MTP_M_CLUSTER;
		break;
	}
	return mtp_status_ind(q, mtp, &dest, error);
}
STATIC int
mtp_cong_status_ind_all_local(queue_t *q, struct rs *rs, uint newstatus)
{
	int i, err;
	struct mtp *mtp;
	for (i = 0; i < 16; i++)
		for (mtp = rs->sp.sp->mtp.lists[i]; mtp; mtp = mtp->sp.next)
			if ((err = mtp_cong_status_ind(q, mtp, rs, newstatus)))
				return (err);
	return (0);
}

/*
 *  MTP-PAUSE-IND
 *  -----------------------------------
 */
STATIC int
mtp_pause_ind(queue_t *q, struct mtp *mtp, struct rs *rs)
{
	struct mtp_addr dest = mtp->src;
	uint error = T_MTP_PROHIBITED;
	dest.pc = rs->dest;
	switch (rs->rs_type) {
	default:
		swerr();
	case RT_TYPE_MEMBER:
		error |= T_MTP_M_MEMBER;
		break;
	case RT_TYPE_CLUSTER:
		error |= T_MTP_M_CLUSTER;
		break;
	}
	return mtp_status_ind(q, mtp, &dest, error);
}
STATIC int
mtp_pause_ind_all_local(queue_t *q, struct rs *rs)
{
	int i, err;
	struct mtp *mtp;
	for (i = 0; i < 16; i++)
		for (mtp = rs->sp.sp->mtp.lists[i]; mtp; mtp = mtp->sp.next)
			if ((err = mtp_pause_ind(q, mtp, rs)))
				return (err);
	return (0);
}

/*
 *  MTP-RESUME-IND
 *  -----------------------------------
 */
STATIC int
mtp_resume_ind(queue_t *q, struct mtp *mtp, struct rs *rs)
{
	struct mtp_addr dest = mtp->src;
	uint error = T_MTP_AVAILABLE;
	dest.pc = rs->dest;
	switch (rs->rs_type) {
	default:
		swerr();
	case RT_TYPE_MEMBER:
		error |= T_MTP_M_MEMBER;
		break;
	case RT_TYPE_CLUSTER:
		error |= T_MTP_M_CLUSTER;
		break;
	}
	return mtp_status_ind(q, mtp, &dest, error);
}
STATIC int
mtp_resume_ind_all_local(queue_t *q, struct rs *rs)
{
	int i, err;
	struct mtp *mtp;
	for (i = 0; i < 16; i++)
		for (mtp = rs->sp.sp->mtp.lists[i]; mtp; mtp = mtp->sp.next)
			if ((err = mtp_resume_ind(q, mtp, rs)))
				return (err);
	return (0);
}

/*
 *  MTP-TRANSFER-IND
 *  -----------------------------------
 */
STATIC int
mtp_transfer_ind(queue_t *q, struct mtp *mtp, struct mtp_msg *m)
{
	mblk_t *dp;
	if ((dp = ss7_dupb(q, m->bp))) {
		struct mtp_addr src = { ni:m->ni, pc:m->opc, si:m->si };
		struct mtp_addr dst = { ni:m->ni, pc:m->dpc, si:m->si };
		int err;
		dp->b_rptr = m->data;
		dp->b_wptr = dp->b_rptr + m->dlen;
		switch (mtp->i_style) {
		case MTP_STYLE_MTPI:
		case MTP_STYLE_MGMT:
			if ((err = m_transfer_ind(q, mtp, &src, m->sls, m->mp, dp)) != QR_ABSORBED)
				break;
			return (QR_DONE);
		case MTP_STYLE_TPI:
			switch (mtp->prot->SERV_type) {
			case T_CLTS:
				if ((err = t_unitdata_ind(q, mtp, &src, NULL, dp)) != QR_ABSORBED)
					break;
				return (QR_DONE);
			case T_COTS:
			case T_COTS_ORD:
				if ((err = t_data_ind(q, mtp, 0, dp)) != QR_ABSORBED)
					break;
				return (QR_DONE);
			default:
				swerr();
				err = -EFAULT;
				break;
			}
			break;
		case MTP_STYLE_NPI:
			switch (mtp->prot->SERV_type) {
			case T_CLTS:	/* N_CLNS */
				if ((err = n_unitdata_ind(q, mtp, &src, &dst, dp)) != QR_ABSORBED)
					break;
				return (QR_DONE);
			case T_COTS:	/* N_CONS */
			case T_COTS_ORD:
				if ((err = n_data_ind(q, mtp, 0, dp)) != QR_ABSORBED)
					break;
				return (QR_DONE);
			default:
				swerr();
				err = -EFAULT;
				break;
			}
			break;
		default:
			swerr();
			err = -EFAULT;
			break;
		}
		rare();
		freeb(dp);
		return (err);
	}
	rare();
	return (-ENOBUFS);
}
STATIC int
mtp_transfer_ind_local(queue_t *q, struct sp *sp, struct mtp_msg *m)
{
	struct mtp *mtp;
	for (mtp = sp->mtp.lists[m->si & 0xf]; mtp; mtp = mtp->sp.next)
		switch (mtp->prot->SERV_type) {
		default:
			swerr();
		case T_CLTS:
			if (mtp->state != TS_IDLE || mtp->src.pc != m->dpc)
				continue;
			break;
		case T_COTS:
		case T_COTS_ORD:
			if (mtp->state != TS_DATA_XFER || mtp->src.pc != m->dpc
			    || mtp->dst.pc != m->opc)
				continue;
			break;
		}
	if (mtp)
		return mtp_transfer_ind(q, mtp, m);
#if 0
	if (!(sp->mtp.known & (1 << m->si))) {
		fixme(("Reply with UPU unknown\n"));
		return (QR_DONE);
	}
#endif
	if (!(sp->mtp.equipped & (1 << m->si))) {
		fixme(("Reply with UPU unequipped\n"));
		return (QR_DONE);
	}
	if (!(sp->mtp.available & (1 << m->si))) {
		fixme(("Reply with UPU inaccessible\n"));
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Routing functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Select index on sls, mask and bits
 *  -------------------------------------------------------------------------
 */
STATIC INLINE uchar
sls_index(uchar sls, uchar mask, uchar bits)
{
	uchar index = 0;
	uchar ibit = 0x01;
	uchar obit = 0x01;
	while (mask) {
		if (mask & ibit) {
			mask &= ~ibit;
			if (sls & ibit)
				index |= obit;
			obit <<= 1;
		}
		ibit <<= 1;
	}
	index = index & ((1 << bits) - 1);
	return (index);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Send message routing functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  SEND LINK
 *  -------------------------------------------------------------------------
 *  For sending a message on a specific link.  This is used for CBD which must be sent on the
 *  altnerate link.
 */
STATIC int
mtp_send_link(queue_t *q, mblk_t *dp, struct sl *sl)
{
	(void) q;
	if (canput(sl->oq)) {
		if (sl->lk.lk->ls.ls->lk.numb > 1) {
			/* 
			   only rotate on combined linksets (i.e. not C-links) */
			struct sp *sp = sl->lk.lk->sp.loc;
			switch (sp->na.na->option.pvar) {
			case SS7_PVAR_ANSI:
			case SS7_PVAR_JTTC:
			case SS7_PVAR_CHIN:	/* ??? */
			{
				uint sls = dp->b_rptr[7] & sp->ls.sls_mask;
				/* 
				   Actually, for 5-bit and 8-bit compatability, we rotate based on
				   5 sls bits regardless of the number of bits in the SLS */
				if (sls & 0x01)
					sls = (sls & 0xe0) | ((sls & 0x1f) >> 1) | 0x10;
				else
					sls = (sls & 0xe0) | ((sls & 0x1f) >> 1) | 0x00;
				/* 
				   rewrite sls in routing label */
				dp->b_rptr[7] = sls;
				break;
			}
			}
		}
		ss7_oput(sl->oq, dp);	/* send message */
		return (QR_DONE);
	}
	rare();
	freemsg(dp);
	return (-EBUSY);
}

/*
 *  SEND ROUTE
 *  -------------------------------------------------------------------------
 *  Full routing for management messages sent from a signalling point.  This is for management only
 *  message because it does not report congestion back to the user.
 */
STATIC int
mtp_send_route(queue_t *q, struct sp *sp, mblk_t *mp, uint priority, uint32_t dpc, uint sls)
{
	struct rs *rs;
	struct rl *rl;
	struct rt *rt;
	struct ls *ls;
	struct sl *sl;
	struct na *na = sp->na.na;
	int err;
	for (rs = sp->rs.list; rs; rs = rs->sp.next)
		if (rs->rs_type == RT_TYPE_MEMBER && rs->dest == dpc)
			goto rs_found;
	for (rs = sp->rs.list; rs; rs = rs->sp.next)
		if (rs->rs_type == RT_TYPE_CLUSTER && rs->dest == (dpc & na->mask.cluster))
			goto rs_found;
	goto efault;
      rs_found:
	if (rs->state > RS_RESTRICTED)
		goto discard;
	if (!(rl = rs->rl.curr))
		goto efault;
	if (!(ls = rl->ls.ls))
		goto efault;
	/* 
	   select active route based on sls */
	if (!(rt = rl->smap[sls_index(sls, ls->rl.sls_mask, ls->rl.sls_bits)].rt))
		goto efault;
	if (rt->type == MTP_OBJ_TYPE_CR) {
		/* 
		   we are performing controlled rerouting, buffer message */
		bufq_queue(&((struct cr *) rt)->buf, mp);
		return (QR_DONE);
	}
	/* 
	   Selection signalling link in link (set): for ITU-T we need to use the bits that were not 
	   used in route selection; for ANSI we use the least significant 4 bits (but could be 7
	   for 8-bit SLSs) after route selection and sls rotation.  Again, the following algorithm
	   does the job for both ITU-T and ANSI and will accomodate quad linksets to quad STPs. */
	/* 
	   select active signalling link based on sls */
	if (!(sl = rt->lk.lk->smap[sls_index(sls, ls->lk.sls_mask, ls->lk.sls_bits)].sl))
		goto efault;
	/* 
	   Check for changeback on the signalling link index: normally the pointer is to a
	   signalling link object not a changeback buffer object. This approach is faster than the
	   previous approach. */
	if (sl->type == MTP_OBJ_TYPE_CB) {
		/* 
		   we are performing changeback or changeover, buffer message */
		bufq_queue(&((struct cb *) sl)->buf, mp);
		return (QR_DONE);
	}
	if (!(na->option.popt & SS7_POPT_MPLEV))
		priority = 0;
	if (sl->cong_status > priority) {
		/* 
		   As the message originated from us, we should treat this as though we received a
		   TFC for the RCT message.  This means that the congestion priority on the
		   routeset is increased to the congestion of the linkset. */
		rs_timer_stop(rs, t15);
		rs_timer_stop(rs, t16);
		/* 
		   signal congestion (just for RCT) */
		if ((err = mtp_cong_status_ind_all_local(q, rs, sl->cong_status)))
			return (err);
		rs->cong_status = sl->cong_status;
		rs_timer_start(rs, t15);
	}
	if (sl->disc_status > priority)
		goto discard;
	if (!canput(sl->oq))
		goto ebusy;
	/* 
	   Note: we do not do SLS bit rotation for messages sent from this signalling point, even
	   if it is an STP.  We only perform SLS bit rotation on messages transferred across a
	   Signalling Transfer Point (see mtp_xfer_route()). However, because we must unrotate all
	   ANSI messages retrieved from the signalling link on changeover, we still need to rotate. 
	 */
	if (sl->lk.lk->ls.ls->lk.numb > 1) {
		/* 
		   Only rotate when part of the sls was actually used for route selection (i.e. not 
		   on C-Links or F-Links, but only on combined link sets (A-, B-, D-, E- Links) */
		switch (na->option.pvar) {
		case SS7_PVAR_ANSI:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_CHIN:	/* ??? */
			/* 
			   XXX Actually, for 5-bit and 8-bit compatability, we rotate based on 5
			   sls bits regardless of the number of bits in the SLS */
			if (sls & 0x01)
				sls = (sls & 0xe0) | ((sls & 0x1f) >> 1) | 0x10;
			else
				sls = (sls & 0xe0) | ((sls & 0x1f) >> 1) | 0x00;
			/* 
			   rewrite sls in routing label */
			mp->b_rptr[7] = sls;
			break;
		}
	}
	sl->flags |= SLF_TRAFFIC;
	ss7_oput(sl->oq, mp);	/* send message */
	return (QR_DONE);
      discard:
	/* 
	   This is for management messages only, so we do not report congestion or prohibited
	   routes back to the user.  Oft times, we may blindly send management messages not
	   considering the state of the routeset with the anticipation that they will be discarded
	   if the routeset is not accessible. */
	freemsg(mp);
	return (QR_DONE);	/* discard message */
      ebusy:
	rare();
	freemsg(mp);
	return (-EBUSY);
      efault:
	swerr();
	freemsg(mp);
	return (-EFAULT);
}

/*
 *  SEND ROUTE (LOADSHARE)
 *  -------------------------------------------------------------------------
 *  Full routine for a message we created with loadsharing.
 */
STATIC int
mtp_send_route_loadshare(queue_t *q, struct sp *sp, mblk_t *bp, uint mp, uint32_t dpc, uint sls)
{
	fixme(("Select new sls based on loadsharing\n"));
	return mtp_send_route(q, sp, bp, mp, dpc, sls);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Send message functions
 *
 *  -------------------------------------------------------------------------
 */
STATIC int mtp_send_link(queue_t *q, mblk_t *dp, struct sl *sl);

STATIC int
mtp_send_coo(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc,
	     uint fsnl)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 1);
		mtp_enc_com(bp, sp, slc, fsnl);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_coa(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc,
	     uint fsnl)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 2);
		mtp_enc_com(bp, sp, slc, fsnl);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}

/*
 *  6.3.1   ... a changeback declaration is sent to the remote signalling point of the signalling link make
 *  available via the concerned alternative signalling link; this messge indicates that no more message signal units
 *  relating to the traffic being diverted to the link made available will be sent on the alternative signalling
 *  link.
 */
STATIC int
mtp_send_cbd(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc,
	     uint cbc, struct sl *sl)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 5);
		mtp_enc_cbm(bp, sp, slc, cbc);
		return mtp_send_link(q, bp, sl);
	}
	return (-ENOBUFS);
}

/*
 *  6.3.2   ... any available signalling route between the two signalling points can be used to carry the changeback
 *  acknowledgement.
 */
STATIC int
mtp_send_cba(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc,
	     uint cbc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 6);
		mtp_enc_cbm(bp, sp, slc, cbc);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_eco(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 2, 1);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_eca(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 2, 2);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_rct(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint mp)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, mp, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 3, 1);
		return mtp_send_route(q, sp, bp, mp, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_tfc(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest, uint stat)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 3, 2);
		mtp_enc_tfc(bp, sp, dest, stat);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}

/*
 *  13.2.1 ... Transfer-prohibited messages are always addressed to an adjacent signalling point.  They may use any
 *  available signalling route that leads to that signalling point.
 */
STATIC int
mtp_send_tfp(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 4, 1);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_tcp(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 4, 2);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}

/*
 *  13.4.1 ... Transfer-restricted messages are always address to an adjacent signalling point.  They may use any
 *  available signalling route that leads to that signalling point.
 */
STATIC int
mtp_send_tfr(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 4, 3);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_tcr(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 4, 4);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}

/*
 *  13.3.1 ... Transfer-allowed message are always address to an adjacent signalling point.  They may use any
 *  available signalling route that leads to that signalling point.
 */
STATIC int
mtp_send_tfa(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 4, 5);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_tca(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 4, 6);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}

/*
 *  13.5.3  A signalling-route-set-test message is sent to the adjacent signalling transfer point as an ordinary
 *  signalling network management message.
 */
STATIC int
mtp_send_rst(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 5, 1);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_rsr(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 5, 2);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_rcp(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 5, 3);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_rcr(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 5, 4);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_lin(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 1);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_lun(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 2);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_lia(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 3);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_lua(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 4);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_lid(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 5);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_lfu(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 6);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_llt(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 7);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_lrt(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 8);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_tra(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 7, 1);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_trw(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 7, 2);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_dlc(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc,
	     uint sdli)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 8, 1);
		mtp_enc_dlc(bp, sp, slc, sdli);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_css(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 8, 2);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_cns(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 8, 3);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_cnp(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 8, 4);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_upu(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest, uint upi)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 10, 1);
		mtp_enc_upm(bp, sp, dest, upi);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC INLINE int
mtp_send_upa(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest, uint upi)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 10, 2);
		mtp_enc_upm(bp, sp, dest, upi);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_upt(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest, uint upi)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 10, 3);
		mtp_enc_upm(bp, sp, dest, upi);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_sltm(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc,
	      unsigned char *data, size_t dlen, struct sl *sl)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 1, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 1);
		mtp_enc_sltm(bp, sp, slc, dlen, data);
		return mtp_send_link(q, bp, sl);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_slta(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc,
	      unsigned char *data, size_t dlen)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 1, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 2);
		mtp_enc_sltm(bp, sp, slc, dlen, data);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_ssltm(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc,
	       unsigned char *data, size_t dlen, struct sl *sl)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 2, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 1);
		mtp_enc_sltm(bp, sp, slc, dlen, data);
		return mtp_send_link(q, bp, sl);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_sslta(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint slc,
	       unsigned char *data, size_t dlen)
{
	mblk_t *bp;
	uint rl_sls = ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;
	if ((bp = mtp_enc_msg(q, sp, ni, 3, 2, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 2);
		mtp_enc_sltm(bp, sp, slc, dlen, data);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
STATIC int
mtp_send_user(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc, uint sls, uint mp,
	      uint si, mblk_t *dp)
{
	mblk_t *bp;
	if ((bp = mtp_enc_msg(q, sp, ni, mp, si, dpc, opc, sls))) {
		mtp_enc_user(bp, dp);
		return mtp_send_route(q, sp, bp, mp, dpc, sls);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  MTP Adjacent procedures
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  TFP/TCP BROADCAST
 *  -----------------------------------
 *  Perform local and adjacent (if STP) broadcast of routeset prohibited, except on a primary linkset which receives
 *  nothing, and except for an old non-primary linkset which has already received TFP.
 *
 *  13.2.1 ... Transfer-prohibited messages are always addressed to an adjacent signalling point.  They may use any
 *  available signalling route that leads to that signalling point.
 *
 *  13.2.2  A transfer prohibited message relating to a given destination X is sent from a signalling transfer point
 *  Y in the following cases:
 *
 *  i)      When signalling transfer point Y starts to route (at changeover, changeback, forced or controlled
 *  rerouting) signalling destined to signalling point X via a signalling transfer point Z not currently used by
 *  siganalling transfer point Y for this traffic.  In this case, the transfer prohibited message is sent to
 *  signalling transfer point Z.
 *
 *  ii)     When signalling transfer point Y recognizes the inaccessibility of signalling point X (see 5.3.3 and
 *  7.2.3).  In this case a transfer-prohibited message is sent to all accessible adjacent signalling points
 *  (Broadcast method) and timer T8 (see clause 16) is started concerning SP X.
 *
 *  iii)    When a message destined to signalling point X is received at signalling transfer point Y and Y is unable
 *  to transfer the message, and if no corresponding timer T8 is running.  In this case the transfer-prohibited
 *  message is sent to the adjacent signalling point from which the message concerned was received (Response
 *  Method).  In addition, timer T8 is started concerning SP X.
 *
 *  iv)     When an adjacent signalling point Z becomes accessible, STP Y sends to Z a transfer prohibited message
 *  concerning destination X, if X is inaccessible from Y (clause 9).
 *
 *  v)      When a signalling transfer point Y restarts, it broadcasts to all accessible adjacent signalling points
 *  transfer prohibited messages concerning destination X, if X is inaccessible from Y (see clause 9).
 */
STATIC int
mtp_tfp_broadcast(queue_t *q, struct rs *rs)
{
	struct sp *sp = rs->sp.sp;
	int err;
	if ((err = mtp_pause_ind_all_local(q, rs)))
		return (err);
	if (sp->flags & SPF_XFER_FUNC) {
		if (!(rs->flags & RSF_CLUSTER)) {
			struct ls *ls;
			for (ls = sp->ls.list; ls; ls = ls->sp.next) {
				struct lk *lk;
				for (lk = ls->lk.list; lk; lk = lk->ls.next) {
					struct rs *adj = lk->sp.adj;
					if (adj->dest == rs->dest)
						continue;
					mtp_send_tfp(q, sp, lk->ni, adj->dest, sp->pc, 0, rs->dest);
				}
			}
		} else {
			struct ls *ls;
			for (ls = sp->ls.list; ls; ls = ls->sp.next) {
				struct lk *lk;
				for (lk = ls->lk.list; lk; lk = lk->ls.next) {
					struct rs *adj = lk->sp.adj;
					mtp_send_tcp(q, sp, lk->ni, adj->dest, sp->pc, 0, rs->dest);
				}
			}
		}
	}
	return (QR_DONE);
}

/*
   TFP procedure for STPs during rerouting 
 */
STATIC void
mtp_tfp_reroute(struct rl *rl)
{
	struct rs *rs = rl->rs.rs;
	struct sp *sp = rs->sp.sp;
	if (sp->flags & SPF_XFER_FUNC) {
		if (!(rs->flags & RSF_CLUSTER)) {
			struct rt *rt;
			for (rt = rl->rt.list; rt; rt = rt->rl.next) {
				struct lk *lk = rt->lk.lk;
				struct rs *adj = lk->sp.adj;
				if (adj->dest != rs->dest)
					mtp_send_tfp(NULL, sp, lk->ni, adj->dest, sp->pc, 0,
						     rs->dest);
			}
		} else {
			struct rt *rt;
			for (rt = rl->rt.list; rt; rt = rt->rl.next) {
				struct lk *lk = rt->lk.lk;
				struct rs *adj = lk->sp.adj;
				if (adj->dest != rs->dest)
					mtp_send_tcp(NULL, sp, lk->ni, adj->dest, sp->pc, 0,
						     rs->dest);
			}
		}
	}
}

/*
 *  TFR/TCR BROADCAST
 *  -----------------------------------
 *  Perform adjacent (if STP) broadcast of routeset restricted, including an old non-primary linkset, but not a new
 *  non-primary linkset which receives a directed TFP, and not on a primary linkset which receives nothing.
 *
 *  13.4.1 ... Transfer-restricted messages are always address to an adjacent signalling point.  They may use any
 *  available signalling route that leads to that signalling point.
 *
 *  13.4.2  A transfer-restricted message relating to a given destination X is sent from a signalling transfer point
 *  Y when the normal link set (combined link set) used by signalling point Y to route to destination X experiences
 *  a long-term failure such as an equipment failure, or there is congestion on an alternate link set currently
 *  being used to destination X.  In this case, a transfer-retricted message is sent to all acessible signalling
 *  points except those that receive a TFP message according to 13.2.2 i), and except signalling point X if it is an
 *  adjacent point (Broadcast Method).
 *
 *  When and adjacent signalling point X becomes acessible, the STP Y sends to X transfer-restricted message
 *  concerning destinations that are restricted from Y (see clause 9).
 *
 *  When a signalling point Y restarts, it broadcasts to all accessible adjacent signalling points transfer
 *  retricted messages concerning destinations that are restricted from Y (see clause 9).
 */
STATIC int
mtp_tfr_broadcast(queue_t *q, struct rs *rs)
{
	struct sp *sp = rs->sp.sp;
	/* 
	   don't inform local users */
	if (sp->flags & SPF_XFER_FUNC) {
		/* 
		   current and primary linksets were addressed during rerouting */
		struct ls *lc = rs->rl.curr ? rs->rl.curr->ls.ls : NULL;	/* current link set 
										 */
		struct ls *lp = rs->rl.list ? rs->rl.list->ls.ls : NULL;	/* primary link set 
										 */
		if (!(rs->flags & RSF_CLUSTER)) {
			struct ls *ls;
			for (ls = sp->ls.list; ls; ls = ls->sp.next) {
				struct lk *lk;
				for (lk = ls->lk.list; lk; lk = lk->ls.next) {
					struct rs *adj = lk->sp.adj;
					if (ls != lc && ls != lp && adj->dest != rs->dest)
						mtp_send_tfr(q, sp, lk->ni, adj->dest, sp->pc, 0,
							     rs->dest);
				}
			}
		} else {
			struct ls *ls;
			for (ls = sp->ls.list; ls; ls = ls->sp.next) {
				struct lk *lk;
				for (lk = ls->lk.list; lk; lk = lk->ls.next) {
					struct rs *adj = lk->sp.adj;
					if (ls != lc && ls != lp && adj->dest != rs->dest)
						mtp_send_tcr(q, sp, lk->ni, adj->dest, sp->pc, 0,
							     rs->dest);
				}
			}
		}
	}
	return (QR_DONE);
}

/*
   TFA procedure for STPs during rerouting 
 */
STATIC void
mtp_tfr_reroute(struct rl *rl)
{
	struct rs *rs = rl->rs.rs;
	struct sp *sp = rs->sp.sp;
	if (sp->flags & SPF_XFER_FUNC) {
		if (!(rs->flags * RSF_CLUSTER)) {
			struct rt *rt;
			for (rt = rl->rt.list; rt; rt = rt->rl.next) {
				struct lk *lk = rt->lk.lk;
				struct rs *adj = lk->sp.adj;
				if (adj->dest != rs->dest)
					mtp_send_tfr(NULL, sp, lk->ni, adj->dest, sp->pc, 0,
						     rs->dest);
			}
		} else {
			struct rt *rt;
			for (rt = rl->rt.list; rt; rt = rt->rl.next) {
				struct lk *lk = rt->lk.lk;
				struct rs *adj = lk->sp.adj;
				if (adj->dest != rs->dest)
					mtp_send_tcr(NULL, sp, lk->ni, adj->dest, sp->pc, 0,
						     rs->dest);
			}
		}
	}
}

/*
 *  TFA/TCA BROADCAST
 *  -----------------------------------
 *  Perform local and adjacent (if STP) broadcast of routeset allowed, including an old non-primary linkset, but not
 *  a new non-primary linkset which receives a directed TFP, and not on a primary linkset which receives nothing.
 *
 *  13.3.1 ... Transfer-allowed message are always address to an adjacent signalling point.  They may use any
 *  available signalling route that leads to that signalling point.
 *
 *  13.3.2  A transfer-allowed message relating to a given destination X is sent form signalling transfer point Y in
 *  the following cases:
 *
 *  i)  When signalling transfer point Y stops routing (at changeback or controlled re-routing), signalling traffic
 *  destined to signalling point X via a signalling transfer point Z (to which the concerned traffic was previously
 *  diverted as a consequence of changeover or forced rerouting).  In this case, the transfer-allowed message is
 *  sent to signalling transfer point Z.
 *
 *  ii) When signalling transfer point Y recognizes that it is again able to transfer signalling traffic destined to
 *  signalling point X (see 6.2.3 and 8.2.3).  In this case a transfer-allowed message is sent to all accessible
 *  adjacent signalling points, except those signalling points that receive a TFP message according to 13.2.2 i) and
 *  excpet signalling point X if it is an adjacent point. (Broadcast Method).
 */
STATIC int
mtp_tfa_broadcast(queue_t *q, struct rs *rs)
{
	struct sp *sp = rs->sp.sp;
	/* 
	   inform users */
	int err;
	if ((err = mtp_resume_ind_all_local(q, rs)))
		return (err);
	if (sp->flags & SPF_XFER_FUNC) {
		/* 
		   current and primary linksets were addressed during rerouting below */
		struct ls *lc = rs->rl.curr ? rs->rl.curr->ls.ls : NULL;	/* current link set 
										 */
		struct ls *lp = rs->rl.list ? rs->rl.list->ls.ls : NULL;	/* primary link set 
										 */
		if (!(rs->flags & RSF_CLUSTER)) {
			struct ls *ls;
			for (ls = sp->ls.list; ls; ls = ls->sp.next) {
				struct lk *lk;
				for (lk = ls->lk.list; lk; lk = lk->ls.next) {
					struct rs *adj = lk->sp.adj;
					if (ls != lc && ls != lp && adj->dest != rs->dest)
						mtp_send_tfa(q, sp, lk->ni, adj->dest, sp->pc, 0,
							     rs->dest);
				}
			}
		} else {
			struct ls *ls;
			for (ls = sp->ls.list; ls; ls = ls->sp.next) {
				struct lk *lk;
				for (lk = ls->lk.list; lk; lk = lk->ls.next) {
					struct rs *adj = lk->sp.adj;
					if (ls != lc && ls != lp && adj->dest != rs->dest)
						mtp_send_tca(q, sp, lk->ni, adj->dest, sp->pc, 0,
							     rs->dest);
				}
			}
		}
	}
	return (QR_DONE);
}

/*
   TFA procedure for STPs during rerouting 
 */
STATIC void
mtp_tfa_reroute(struct rl *rl)
{
	struct rs *rs = rl->rs.rs;
	struct sp *sp = rs->sp.sp;
	if (sp->flags & SPF_XFER_FUNC) {
		if (!(rs->flags * RSF_CLUSTER)) {
			struct rt *rt;
			for (rt = rl->rt.list; rt; rt = rt->rl.next) {
				struct lk *lk = rt->lk.lk;
				struct rs *adj = lk->sp.adj;
				if (adj->dest != rs->dest)
					mtp_send_tfa(NULL, sp, lk->ni, adj->dest, sp->pc, 0,
						     rs->dest);
			}
		} else {
			struct rt *rt;
			for (rt = rl->rt.list; rt; rt = rt->rl.next) {
				struct lk *lk = rt->lk.lk;
				struct rs *adj = lk->sp.adj;
				if (adj->dest != rs->dest)
					mtp_send_tca(NULL, sp, lk->ni, adj->dest, sp->pc, 0,
						     rs->dest);
			}
		}
	}
}

/*
 *  SEND XFER
 *  ------------------------------------------------------------------------
 *  Full routing for messages transfered through a signalling transfer point.  This is for normal transfer messages
 *  (including the possibility of circular route tests).
 */
STATIC int
mtp_xfer_route(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;	/* the link (set) on which the message arrived */
	struct rs *rs;			/* the outgoing route set when transferring */
	struct rl *rl;			/* the outgoing route list when transferring */
	struct rt *rt;			/* the outgoing route when transferring */
	struct ls *ls;			/* the outgoing link set when transferring */
	struct sp *loc = lk->sp.loc;	/* the local signalling point the message arrived for */
	struct rs *adj = lk->sp.adj;	/* the route set to the adjacent of the arriving link */
	struct na *na = loc->na.na;	/* the local network appearance, protocol and options */
	int err;
	/* 
	   9.6.6 All messages to another destination received at a signalling point whose MTP is
	   restarting are discarded. */
	if (loc->state >= SP_RESTART)
		goto discard;
	/* 
	   this linear search could be changed to a hash later */
	for (rs = loc->rs.list; rs; rs = rs->sp.next)
		if (rs->rs_type == RT_TYPE_MEMBER && rs->dest == m->dpc)
			goto rs_found;
	for (rs = loc->rs.list; rs; rs = rs->sp.next)
		if (rs->rs_type == RT_TYPE_CLUSTER
		    && rs->dest == (m->dpc & loc->na.na->mask.cluster))
			goto rs_found;
	/* 
	   Somebody could be probing our route sets.  There is an additional reason for not sending 
	   responsive TFP here and it is described in Q.704: if we send TFP, we will get a lot of
	   uninvited RST messages every T10 inquiring as to the state of the route, when a
	   configuration error clearly exists.  We should screen this message and inform management 
	   instead. */
	goto screened;
      rs_found:
	todo(("This is where we could do enhanced gateway screening\n"));
	switch (rs->state) {
	case RS_RESTRICTED:
		/* 
		   Note: rr.list will be null if we do not send TFR */
		if (rs->rr.list) {
			/* 
			   if the message came in on a link set to which we have not yet sent
			   responsive TFR, then we should send responsive TFR */
			struct rr *rr;
			for (rr = rs->rr.list; rr; rr = rr->rs.next) {
				if (rr->lk == lk) {
					/* 
					   send responsive TFR on this link */
					fixme(("Send responsive TFR on this link\n"));
					mtp_free_rr(rr);
					break;
				}
			}
			/* 
			   Note: if we are using TFR responsive method and this is the very first
			   TFR sent, we need to start timer T18a. */
			if ((rs->flags & RSF_TFR_PENDING)) {
				rs->flags &= ~RSF_TFR_PENDING;
				rs_timer_start(rs, t18a);
			}
		}
		/* 
		   fall through */
	case RS_CONGESTED:
	case RS_DANGER:
	case RS_ALLOWED:
		break;
	case RS_RESTART:
		/* 
		   9.6.7 In adjacent signalling points during the restart procedure, message not
		   part of the restart procedure but which are destined to or through the
		   signalling point whose MTP is restarting, are discarded. */
		goto discard;
	case RS_PROHIBITED:
	case RS_BLOCKED:
	case RS_INHIBITED:
	case RS_INACTIVE:
		/* 
		   if the destination is currently inaccessible and we have not sent a TFP within
		   the last t8, send a responsive TFP */
		if (!rs->timers.t8) {
			if (!(rs->flags & RSF_CLUSTER))
				mtp_send_tfp(q, loc, m->ni, adj->dest, loc->pc, 0, rs->dest);
			else
				mtp_send_tcp(q, loc, m->ni, adj->dest, loc->pc, 0, rs->dest);
			rs_timer_start(rs, t8);
		}
		return (QR_DONE);
	}
	if (!(rl = rs->rl.curr))
		goto efault;
	if (!(ls = rl->ls.ls))
		goto efault;
	/* 
	   select active route based on sls */
	if (!(rt = rl->smap[sls_index(m->sls, ls->rl.sls_mask, ls->rl.sls_bits)].rt))
		goto efault;
	if (rt->type == MTP_OBJ_TYPE_CR) {
		/* 
		   we are performing controlled rerouting, buffer message */
		bufq_queue(&((struct cr *) rt)->buf, mp);
		return (QR_ABSORBED);
	}
	/* 
	   Selection signalling link in link (set): for ITU-T we need to use the bits that were not 
	   used in route selection; for ANSI we use the least significant 4 bits (but could be 7
	   for 8-bit SLSs) after route selection and sls rotation.  Again, the following algorithm
	   does the job for both ITU-T and ANSI and will accomodate quad linksets to quad STPs. */
	/* 
	   select active signalling link based on sls */
	if (!(sl = rt->lk.lk->smap[sls_index(m->sls, ls->lk.sls_mask, ls->lk.sls_bits)].sl))
		goto efault;
	/* 
	   Check for changeback on the signalling link index: normally the pointer is to a
	   signalling link object not a changeback buffer object. This approach is faster than the
	   previous approach. */
	if (sl->type == MTP_OBJ_TYPE_CB) {
		/* 
		   we are performing changeback or changeover, buffer message */
		bufq_queue(&((struct cb *) sl)->buf, mp);
		return (QR_ABSORBED);
	}
	if (!(na->option.popt & SS7_POPT_MPLEV))
		m->mp = 0;
	/* 
	   ANSI T1.111.4 (2000) 3.8.2.2.  ...  When the transmit buffer is full, all messages
	   destined for the link should be discarded.  Transfer controlled messages indicating
	   level 3 congestion should be sent to the originators of messages destined for the
	   congested link when the messages are received, if the received messages are not priority 
	   3.  When priority 3 messages destined for a congested link are received and the transmit 
	   buffer is full, transfer-controlled messages are not sent to the originators of the
	   received priority 3 messages. */
	if (sl->cong_status > m->mp && m->mp != 3)
		if ((err =
		     mtp_send_tfc(q, loc, lk->ni, m->opc, loc->pc, m->sls, m->dpc,
				  sl->cong_status)))
			goto error;
	if (sl->disc_status > m->mp)
		goto discard;
	if (!canput(sl->oq))
		goto ebusy;
	if (rl->rt.numb > 1) {
		/* 
		   Only rotate when part of the sls was actually used for route selection (i.e. not 
		   on C-Links or F-Links. */
		switch (na->option.pvar) {
		case SS7_PVAR_ANSI:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_CHIN:	/* ??? */
			/* 
			   Actually, for 5-bit and 8-bit compatability, we rotate based on 5 sls
			   bits regardless of the number of bits in the SLS */
			if (m->sls & 0x01)
				m->sls = (m->sls & 0xe0) | ((m->sls & 0x1f) >> 1) | 0x10;
			else
				m->sls = (m->sls & 0xe0) | ((m->sls & 0x1f) >> 1) | 0x00;
			/* 
			   rewrite sls in routing label */
			mp->b_rptr[7] = m->sls;
			break;
		}
	}
	sl->flags |= SLF_TRAFFIC;
	ss7_oput(sl->oq, mp);	/* send message */
	return (QR_ABSORBED);
      discard:
	return (QR_DONE);	/* discard message */
      screened:
	todo(("Deliver screened message to MGMT\n"));
	return (-EPROTO);
      ebusy:
	rare();
	return (-EBUSY);
      efault:
	swerr();
	return (-EFAULT);
      error:
	return (err);
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
 *  Buffer rerouting
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  CR REROUTE BUFFER
 *  -----------------------------------
 *  Controlled rerouting is performed with a special controlled rerouting buffer which is attached to both the route
 *  from which traffic is being rerouted and the route to which traffic is being rerouted.  If the routeset has
 *  become unavailable while the timer was running, this will result in purging the controlled rerouting buffer.  If
 *  the unavailability was transient and the routeset is now available again, these messages will not be lost.  This
 *  is not mentioned in the MTP specifications.
 *
 *  Rerouting a controlled rerouting buffer consists of diverting the buffer contents on the normal route onto which
 *  traffic was being held for diversion.
 */
STATIC int
cr_reroute_buffer(struct cr *cr)
{
	struct cr **cr_slot = &cr->rl.rl->smap[cr->index].cr;
	struct rt **rt_slot = &cr->rl.rl->smap[cr->index].rt;
	cr_timer_stop(cr, tall);
	if (cr->rt.onto) {
		if (*cr_slot == cr) {
			cr_put(xchg(cr_slot, NULL));
			*rt_slot = rt_get(cr->rt.onto);
		} else if (*rt_slot != cr->rt.onto) {
			swerr();
			goto fault;
		}
		{
			fixme(("Reroute buffer contents.\n"));
			bufq_purge(&cr->buf);
		}
	}
      fault:
	mtp_free_cr(cr);
	return (QR_DONE);
}

/*
 *  CR CANCEL BUFFER
 *  -----------------------------------
 *  Cancelling a controlled rerouting buffer consists of sending the buffer contents back on the original route from
 *  which traffic was being held for diversion.
 */
STATIC void
cr_cancel_buffer(struct cr *cr)
{
	struct cr **cr_slot = &cr->rl.rl->smap[cr->index].cr;
	struct rt **rt_slot = &cr->rl.rl->smap[cr->index].rt;
	cr_timer_stop(cr, tall);
	if (cr->rt.from) {
		cr->rt.from->load++;
		if (cr->rt.onto)
			cr->rt.onto->load--;
		if (*cr_slot == cr) {
			cr_put(xchg(cr_slot, NULL));
			*rt_slot = rt_get(cr->rt.from);
		} else if (*rt_slot != cr->rt.from) {
			swerr();
			mtp_free_cr(cr);
			return;
		}
		{
			fixme(("Restart buffer contents.\n"));
			bufq_purge(&cr->buf);
		}
		mtp_free_cr(cr);
		return;
	}
	/* 
	   can't cancel, reroute forward */
	pswerr(("Forward routing non-cancellable controller rerouting buffer.\n"));
	cr_reroute_buffer(cr);
	return;
}

/*
 *  CB REROUTE BUFFER
 *  -----------------------------------
 *  Rerouting changeback/changeover buffer consists of diverting the buffer contents on the normal signalling link
 *  onto which traffic was being held for diversion (changeover or changeback).
 */
STATIC int
cb_reroute_buffer(struct cb *cb)
{
	struct cb **cb_slot = &cb->lk.lk->smap[cb->index].cb;
	struct sl **sl_slot = &cb->lk.lk->smap[cb->index].sl;
	cb_timer_stop(cb, tall);
	if (cb->sl.onto) {
		if (*cb_slot == cb) {
			cb_put(xchg(cb_slot, NULL));
			*sl_slot = sl_get(cb->sl.onto);
		} else if (*sl_slot != cb->sl.onto) {
			swerr();
			goto fault;
		}
		{
			fixme(("Reroute buffer contents.\n"));
			bufq_purge(&cb->buf);
		}
	}
      fault:
	mtp_free_cb(cb);
	return (QR_DONE);
}

/*
 *  CB CANCEL BUFFER
 *  -----------------------------------
 *  Cancelling a changeback/changeover buffer consists of sending the buffer contents back on the original
 *  signalling link from which traffic was being held for diversion (changeover or changeback).
 */
STATIC void
cb_cancel_buffer(struct cb *cb)
{
	struct cb **cb_slot = &cb->lk.lk->smap[cb->index].cb;
	struct sl **sl_slot = &cb->lk.lk->smap[cb->index].sl;
	cb_timer_stop(cb, tall);
	if (cb->sl.from) {
		cb->sl.from->load++;
		if (cb->sl.onto)
			cb->sl.onto->load--;
		if (*cb_slot == cb) {
			cb_put(xchg(cb_slot, NULL));
			*sl_slot = sl_get(cb->sl.from);
		} else if (*sl_slot != cb->sl.from) {
			swerr();
			mtp_free_cb(cb);
			return;
		}
		{
			fixme(("Restart buffer contents.\n"));
			bufq_purge(&cb->buf);
		}
		mtp_free_cb(cb);
		return;
	}
	/* 
	   can't cancel, change forward */
	pswerr(("Forward change non-cancellable changeover/changeback buffer.\n"));
	cb_reroute_buffer(cb);
	return;
}

/*
 *  REROUTE between routing lists.
 *  -----------------------------------
 *  Reroute traffic from the current route-list (rl_from) onto the specified route-list (rl_onto).  Any controlled
 *  rerouting buffers will be reused and T6 timers left running.  Any traffic flows that do not already have
 *  controlled rerouting buffers will have buffers allocated (controlled rerouting) or not (forced rerouting).
 */
STATIC void
rl_reroute(struct rs *rs, struct rl *rl_onto, const int force)
{
	struct sp *sp = rs->sp.sp;
	struct rl *rl_from = rs->rl.curr;	/* current route list */
	struct rl *rl_prim = rs->rl.list;	/* primary route list */
	struct cr *cr;
	if (rl_onto == rl_from)
		swerr();
	else if (rl_from && !rl_onto)
		while ((cr = rl_from->cr.list))
			mtp_free_cr(cr);
	else if (rl_onto && !rl_from)
		while ((cr = rl_onto->cr.list))
			mtp_free_cr(cr);
	else {
		int index;
		while ((cr = rl_from->cr.list)) {
			/* 
			   if there are existing controlled rerouting buffers in the SLS map, move
			   them to the new route list, leaving messages and timers running */
			/* 
			   remove from old list */
			rl_from->smap[cr->index].rt = cr->rt.onto;
			if ((*cr->rl.prev = cr->rl.next))
				cr->rl.next->rl.prev = cr->rl.prev;
			rl_put(xchg(&cr->rl.rl, NULL));
			/* 
			   insert into new list */
			if ((cr->rl.next = rl_onto->cr.list))
				cr->rl.next->rl.prev = &cr->rl.next;
			cr->prev = &rl_onto->cr.list;
			rl_onto->cr.list = cr;
			cr->rl.rl = rl_get(rl_onto);
			cr->rt.onto = rt_get(rl_onto->smap[cr->index].rt);
			rl_onto->smap[cr->index].cr = cr;
		}
		for (index = 0; index < RT_SMAP_SIZE; index++) {
			if (rl_onto->smap[index].rt->type == MTP_OBJ_TYPE_RT && !force) {
				/* 
				   if there are not controlled rerouting buffers in the resulting
				   SLS map, create them if we are doing controlled rerouting */
				struct rt *rt_from = rl_from->smap[index].rt;
				struct rt *rt_onto = rl_onto->smap[index].rt;
				if ((cr = mtp_alloc_cr(0, rl_onto, rt_from, rt_onto, index))) {
					cr_timer_start(cr, t6);
					rt_put(rt_from);
					rl_onto->smap[index].cr = cr_get(cr);
				}
			}
		}
	}
	/* 
	   special transfer-allowed, -restricted, and -prohibited procedures only apply to STPs. */
	if (!sp->flags & SPF_XFER_FUNC) {
		if (rl_from && rl_from != rl_prim) {
			/* 
			   rerouting from a non-primary route list */
			switch (rs->state) {
			case RS_ALLOWED:
			case RS_DANGER:
			case RS_CONGESTED:
				/* 
				   perform STP transfer-allowed procedure */
				mtp_tfa_reroute(rl_from);
				break;
			case RS_RESTRICTED:
				/* 
				   perform STP transfer-restricted procedure */
				mtp_tfr_reroute(rl_from);
				break;
			}
		}
		if (rl_onto && rl_onto != rl_prim) {
			/* 
			   rerouting onto a non-primary route list */
			/* 
			   perform STP transfer-prohibited procedure */
			mtp_tfp_reroute(rl_onto);
		}
	}
	/* 
	   make new route list current */
	rs->rl.curr = rl_onto;
}

/*
 *  REROUTE between routes
 *  -----------------------------------
 *  Reroute traffic from the current route for the index (rt_from) onto the specified route (rl_onto).  Any
 *  controlled rerouting buffers are automatically assumed and T6 timers left running.  Any traffic flows that do
 *  not already have controlled rerouting buffers will have buffers allocated (controlled rerouting) or not (forced
 *  rerouting).
 */
STATIC void
rt_reroute(struct rl *rl, struct rt *rt_onto, ulong index, int force)
{
	struct rt *rt;
	struct cr *cr;
	if (!(rt = rl->smap[index].rt) || rt->type == MTP_OBJ_TYPE_RT) {
		if (rt != rt_onto) {
			if (rt) {
				/* 
				   unload existing route */
				rt->load--;
				rt_put(xchg(&rl->smap[index].rt, NULL));
			}
			if (rl == rl->rs.rs->rl.curr && !force
			    && (cr = mtp_alloc_cr(0, rl, rt, rt_onto, index))) {
				cr_timer_start(cr, t6);
				rl->smap[index].cr = cr_get(cr);
			} else {
				/* 
				   force reroute anyway */
				rl->smap[index].rt = rt_get(rt_onto);
			}
			rt_onto->load++;
		}
	} else if ((cr = rl->smap[index].cr)->type == MTP_OBJ_TYPE_CR) {
		rt = cr->rt.onto;
		if (rt != rt_onto) {
			/* 
			   move reroute buffer to new target and reload regardless of force */
			if (rt) {
				/* 
				   unload existing route */
				rt->load--;
				rt_put(xchg(&cr->rt.onto, NULL));
			}
			cr->rt.onto = rt_get(rt_onto);
			rt_onto->load++;
		}
	} else {
		pswerr(("Wrong object type in SMAP\n"));
	}
}

/*
 *  REROUTE ALL
 *  -----------------------------------
 *  The specified route takes over all traffic for the route-list.  This is used when the specified route is the
 *  only accessilble route.
 */
STATIC void
rt_reroute_all(struct rl *rl, struct rt *rt_onto, int force)
{
	int i;
	for (i = 0; i < RT_SMAP_SIZE; i++)
		rt_reroute(rl, rt_onto, i, force);
	return;
}

/*
 *  REROUTE ADD
 *  -----------------------------------
 *  Add the specified accessible or restricted route to hand traffic within the route-list.  Assume traffic
 *  dedicated to the specified route as well as any non-normal excess traffic from other active routes in the
 *  route-list.
 */
STATIC void
rt_reroute_add(struct rl *rl, struct rt *rt_onto, int maxload)
{
	int i, n = rl->rt.numb;
	for (i = 0; i < RT_SMAP_SIZE && rt_onto->load < maxload; i++) {
		ulong slot = i % n;
		struct rt *rt = rl->smap[i].rt;
		if (rt->type == MTP_OBJ_TYPE_CR)
			rt = ((struct cr *) rt)->rt.onto;
		if ((slot == rt_onto->slot) || (slot != rt->slot && rt->load > maxload))
			/* 
			   controlled rerouting for addition */
			rt_reroute(rl, rt_onto, i, 0);
	}
	assure(rt_onto->load >= maxload - 1);
}

/*
 *  REROUTE CAN(CEL)
 *  -----------------------------------
 *  When a route fails, this procedure is used to cancel any controlled rerouting which is being performed onto
 *  the failed route.
 */
STATIC void
rt_reroute_can(struct rl *rl, struct rt *rt)
{
	struct cr *cr, *cr_next = rl->cr.list;
	while (rt->load && (cr = cr_next)) {
		cr_next = cr->rl.next;
		if (cr->rt.onto == rt)
			cr_cancel_buffer(cr);
	}
}

/*
 *  REROUTE SUB
 *  -----------------------------------
 *  Remove the specified route from handling traffic within the route-list.  Offload the traffic from the specified
 *  route to other routes currently handling traffic.
 */
STATIC void
rt_reroute_sub(struct rl *rl, struct rt *rt_from, int maxload)
{
	int i;
	/* 
	   first cancel any route controlled rerouting to the failed route */
	rt_reroute_can(rl, rt_from);
	for (i = 0; i < RT_SMAP_SIZE && rt_from->load; i++) {
		struct rt *rt = rl->smap[i].rt;
		if (rt && rt->type == MTP_OBJ_TYPE_CR)
			rt = ((struct cr *) rt)->rt.onto;
		if (!rt || rt != rt_from)
			continue;
		for (rt = rl->rt.list; rt; rt = rt->rl.next)
			if (rt->load && rt->load < maxload)
				/* 
				   force rerouting for subtraction */
				rt_reroute(rl, rt, i, 1);
	}
	assure(rt_from->load == 0);
}

/*
 *  REROUTE RES
 *  -----------------------------------
 *  Remove the sepcified accessible route from handling traffic within the route-list.  Offload the traffic from the
 *  specified route to restricted routes currently handling traffic.
 */
STATIC void
rt_reroute_res(struct rl *rl, struct rt *rt_from, int maxload)
{
	int i;
	/* 
	   first cancel any route controlled rerouting to the failed route */
	rt_reroute_can(rl, rt_from);
	for (i = 0; i < RT_SMAP_SIZE && rt_from->load; i++) {
		struct rt *rt = rl->smap[i].rt;
		if (rt && rt->type == MTP_OBJ_TYPE_CR)
			rt = ((struct cr *) rt)->rt.onto;
		if (!rt || rt != rt_from)
			continue;
		for (rt = rl->rt.list; rt; rt = rt->rl.next)
			if (rt->state == RT_RESTRICTED && rt->load < maxload)
				/* 
				   force rerouting under failure */
				rt_reroute(rl, rt, i, 1);
	}
	assure(rt_from->load == 0);
}

/*
 *  CHANGEBACK
 *  -----------------------------------
 */
STATIC void
sl_changeback(struct lk *lk, struct sl *sl_onto, ulong index)
{
	struct cb *cb;
	struct sl *sl, **slp;
	ensure(sl_onto, return);
	if (!(sl = lk->smap[index].sl))
		return;
	if (sl->type == MTP_OBJ_TYPE_CB) {
		cb = ((struct cb *) sl);
		slp = &cb->sl.onto;
	} else {
		cb = NULL;
		slp = &lk->smap[index].sl;
	}
	if ((sl = *slp)) {
		if (sl == sl_onto) {
			/* 
			   we would be taking over our own change: just cancel the change */
			cb_cancel_buffer(cb);
			return;
		}
		if (sl->load)
			sl->load--;
		sl_put(xchg(slp, NULL));
	}
	*slp = sl_get(sl_onto);
	sl_onto->load++;
	/* 
	   if carrying traffic for the linkset */
	if (lk->load && sl) {
		struct rt *rt = NULL;
		struct rl *rl;
		struct lk *lk = sl_onto->lk.lk;
		struct sp *loc = lk->sp.loc;
		struct rs *adj = lk->sp.adj;
		if (cb)
			cb_timer_stop(cb, tall);
		else if (!(cb = mtp_alloc_cb(0, lk, sl, sl_onto, index)))
			return;
		for (rl = adj->rl.list; rl; rl = rl->rs.next)
			for (rt = rl->rt.list; rt; rt = rt->rl.next)
				if (rt->lk.lk == lk)
					break;
		if (!rt) {
			swerr();
			return;
		}
		if (rt->state < RT_RESTART) {
			/* 
			   sequence changeback */
			mtp_send_cbd(NULL, loc, lk->ni, adj->dest, loc->pc, sl_onto->slc,
				     sl_onto->slc, cb->cbc, sl);
			cb_timer_start(cb, t4);
		} else {
			/* 
			   time controlled changeback */
			cb_timer_start(cb, t3);
		}
	}
}

/*
 *  CHANGEOVER
 *  -----------------------------------
 */
STATIC void
sl_changeover(struct lk *lk, struct sl *sl_onto, ulong index)
{
	struct cb *cb;
	struct sl *sl, **slp;
	ensure(sl_onto, return);
	if (!(sl = lk->smap[index].sl))
		return;
	if (sl->type == MTP_OBJ_TYPE_CB) {
		cb = ((struct cb *) sl);
		slp = &cb->sl.onto;
	} else {
		cb = NULL;
		slp = &lk->smap[index].sl;
	}
	if ((sl = *slp)) {
		if (sl->load)
			sl->load--;
		sl_put(xchg(slp, NULL));
	}
	*slp = sl_get(sl_onto);
	sl_onto->load++;
	if (lk->load && sl) {
		struct lk *lk = sl->lk.lk;
		struct rs *adj = lk->sp.adj;
		if (cb)
			cb_timer_stop(cb, tall);
		else if (!(cb = mtp_alloc_cb(0, lk, sl, sl_onto, index)))
			return;
		if (adj->state < RS_RESTART) {
			/* 
			   sequence changeover */
			/* 
			   the changeover (COO or ECO) should be sent after BSNT retrieval succeeds 
			   or fails */
			cb_timer_start(cb, t2);
		} else {
			/* 
			   time controlled changeover */
			cb_timer_start(cb, t1);
		}
	}
}

/*
 *  CHANGEBACK ADD
 *  -----------------------------------
 */
STATIC void
sl_changeback_add(struct lk *lk, struct sl *sl_onto, int maxload)
{
	int i, n = lk->sl.numb;
	for (i = 0; i < SL_SMAP_SIZE && sl_onto->load < maxload; i++) {
		ulong slot = i % n;
		struct sl *sl = lk->smap[i].sl;
		if (sl->type == MTP_OBJ_TYPE_CB)
			sl = ((struct cb *) sl)->sl.onto;
		if ((slot == sl_onto->slot) || (slot != sl->slot && sl->load > maxload))
			/* 
			   changeback for addition */
			sl_changeback(lk, sl_onto, i);
	}
	assure(sl_onto->load >= maxload - 1);
}

/*
 *  CHANGOVER CAN(CEL)
 *  -----------------------------------
 *  When a signalling link fails, this procedure is used to cancel any changeovers or changebacks which are being
 *  performed onto the failed signalling link.
 */
STATIC void
sl_changeover_can(struct lk *lk, struct sl *sl)
{
	struct cb *cb, *cb_next = lk->cb.list;
	while (sl->load && (cb = cb_next)) {
		cb_next = cb->lk.next;
		if (cb->sl.onto == sl)
			cb_cancel_buffer(cb);
	}
}

/*
 *  CHANGOVER SUB
 *  -----------------------------------
 */
STATIC void
sl_changeover_sub(struct lk *lk, struct sl *sl_from, int maxload)
{
	int i;
	/* 
	   first cancel any signalling link changing over or back to the failed signalling link */
	sl_changeover_can(lk, sl_from);
	for (i = 0; i < SL_SMAP_SIZE && sl_from->load; i++) {
		struct sl *sl = lk->smap[i].sl;
		if (sl && sl->type == MTP_OBJ_TYPE_CB)
			sl = ((struct cb *) sl)->sl.onto;
		if (!sl || sl != sl_from)
			continue;
		for (sl = lk->sl.list; sl; sl = sl->lk.next)
			if (sl->load && sl->load < maxload)
				/* 
				   changeover for subtraction */
				sl_changeover(lk, sl, i);
	}
	assure(sl_from->load == 0);
}

/*
 *  CHANGOVER RES
 *  -----------------------------------
 */
STATIC void
sl_changeover_res(struct lk *lk, struct sl *sl_from, int maxload)
{
	int i;
	/* 
	   first cancel any signalling link changing over or back to the failed signalling link */
	sl_changeover_can(lk, sl_from);
	for (i = 0; i < RT_SMAP_SIZE && sl_from->load; i++) {
		struct sl *sl = lk->smap[i].sl;
		if (sl && sl->type == MTP_OBJ_TYPE_CB)
			sl = ((struct cb *) sl)->sl.onto;
		if (!sl || sl != sl_from)
			continue;
		for (sl = lk->sl.list; sl; sl = sl->lk.next)
			if (sl->state == SL_INHIBITED && sl->load < maxload)
				/* 
				   changeover for subtraction */
				sl_changeover(lk, sl, i);
	}
	assure(sl_from->load = 0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  State Change Functions
 *
 *  -------------------------------------------------------------------------
 */

STATIC ulong
mtp_sta_flags(const ulong state, ulong oldflags)
{
	ulong newflags = oldflags;
	/* *INDENT-OFF* */
	switch (state) {
	case MTP_INACTIVE:	newflags |=  MTPF_INACTIVE;	break;
	case MTP_ACTIVE:	newflags &= ~MTPF_INACTIVE;	break;
	case MTP_BLOCKED:	newflags |=  MTPF_BLOCKED;	break;
	case MTP_UNBLOCKED:	newflags &= ~MTPF_BLOCKED;	break;
	case MTP_INHIBITED:	newflags |=  MTPF_INHIBITED;	break;
	case MTP_UNINHIBITED:	newflags &= ~MTPF_INHIBITED;	break;
	case MTP_PROHIBITED:	newflags |=  MTPF_PROHIBITED;
				newflags &= ~MTPF_RESTRICTED;
				newflags &= ~MTPF_ALLOWED;	break;
	case MTP_RESTART:	newflags |=  MTPF_RESTART;	break;
	case MTP_RESTARTED:	newflags &= ~MTPF_RESTART;	break;
	case MTP_RESTRICTED:	newflags |=  MTPF_RESTRICTED;
				newflags &= ~MTPF_PROHIBITED;
				newflags &= ~MTPF_ALLOWED;	break;
	case MTP_CONGESTED:	newflags |=  MTPF_CONGESTED;	break;
	case MTP_UNCONGESTED:	newflags &= ~MTPF_CONGESTED;	break;
	case MTP_DANGER:	newflags |=  MTPF_DANGER;	break;
	case MTP_NODANGER:	newflags &= ~MTPF_DANGER;	break;
	case MTP_ALLOWED:	newflags |=  MTPF_ALLOWED;
				newflags &= ~MTPF_RESTRICTED;
				newflags &= ~MTPF_PROHIBITED;	break;
	default:
				swerr();
	}
	/* *INDENT-ON* */
	return (newflags);
}

STATIC ulong
mtp_flg_state(ulong newflags)
{
	/* *INDENT-OFF* */
	ulong newstate =
	    (newflags & MTPF_INACTIVE  ) ? MTP_INACTIVE   :
	    (newflags & MTPF_BLOCKED   ) ? MTP_BLOCKED    :
	    (newflags & MTPF_INHIBITED ) ? MTP_INHIBITED  :
	    (newflags & MTPF_PROHIBITED) ? MTP_PROHIBITED :
	    (newflags & MTPF_RESTART   ) ? MTP_RESTART    :
	    (newflags & MTPF_RESTRICTED) ? MTP_RESTRICTED :
	    (newflags & MTPF_CONGESTED ) ? MTP_CONGESTED  :
	    (newflags & MTPF_DANGER    ) ? MTP_DANGER     :
	    (newflags & MTPF_ALLOWED   ) ? MTP_ALLOWED    : MTP_ALLOWED;
	/* *INDENT-ON* */
	return (newstate);
}

STATIC ulong
mtp_cnt_state(struct counters *c, ulong oldstate)
{
	/* *INDENT-OFF* */
	ulong newstate =
	    c->c.allowed    ? MTP_ALLOWED    :
	    c->c.danger     ? MTP_DANGER     :
	    c->c.congested  ? MTP_CONGESTED  :
	    c->c.restricted ? MTP_RESTRICTED :
	    c->c.restart    ? MTP_RESTART    :
	    c->c.prohibited ? MTP_PROHIBITED :
	    c->c.inhibited  ? MTP_INHIBITED  :
	    c->c.blocked    ? MTP_BLOCKED    :
	    c->c.inactive   ? MTP_INACTIVE   :
	    oldstate;
	/* *INDENT-ON* */
	return (newstate);
}

/*
   Change state 
 */
STATIC int
sp_set_state(queue_t *q, struct sp *sp, const ulong state)
{
	ulong sp_oldstate = sp->state;
	ulong sp_newstate = sp->state;
	ulong sp_oldflags = sp->flags;
	ulong sp_newflags = sp->flags;
	if ((sp_newflags = mtp_sta_flags(state, sp_oldflags)) == sp_oldflags)
		goto no_flags_change;
	if ((sp_newstate = mtp_flg_state(sp_newflags)) == sp_oldstate)
		goto no_state_change;
	if (sp_oldstate == SP_INACTIVE && sp_newstate < SP_RESTART) {
		/* 
		   This is the first link in service (usable) at Level 3. */
		if (sp->timers.t1r) {
			/* 
			   If we have a T1 timer running, this is a simple recovery.  We cancel the 
			   t1 timer and move to the active state. */
			sp_timer_stop(sp, t1r);
		} else {
			/* 
			   If T1 timer is not running (has expired) then an MTP restart procedure
			   is required if the SP is an STP or is attached to an STP */
			fixme(("Write this procedure\n"));
			sp_newflags |= SPF_RESTART;
			sp_newstate = SP_RESTART;
		}
	}
	if (sp_newstate == SP_INACTIVE && sp_oldstate < SP_RESTART) {
		/* 
		   This is the last link in service (usable) at level 3. We need to start T1 for
		   the SP to determine whether MTP Restart is necessary on recovery.  We have
		   already set emergency on all signalling links and have initiated restoration of
		   any signalling links that have not been activated. */
		fixme(("Don't need t1r because last t1 has already expired\n"));
		if (!sp->timers.t1r)
			sp_timer_start(sp, t1r);
	}
	(&sp->na.na->sp.allowed)[sp_oldstate]--;
	(&sp->na.na->sp.allowed)[sp_newstate]++;
	sp->state = sp_newstate;
      no_state_change:
	sp->flags = sp_newflags;
      no_flags_change:
	return (QR_DONE);
}

STATIC int
rs_set_state(queue_t *q, struct rs *rs, const ulong state)
{
	struct sp *sp = rs->sp.sp;
	ulong rs_oldstate = rs->state;
	ulong rs_newstate = rs->state;
	ulong rs_oldflags = rs->flags;
	ulong rs_newflags = rs->flags;
	int err;
	if ((rs_newflags = mtp_sta_flags(state, rs_oldflags)) == rs_oldflags)
		goto no_flags_change;
	if ((rs_newstate = mtp_flg_state(rs_newflags)) == rs_oldstate)
		goto no_state_change;
	if (rs_oldstate > RS_RESTART && rs_newstate <= RS_RESTART) {
		if ((rs->flags & RSF_ADJACENT)
		    && ((sp->flags & SPF_XFER_FUNC) || (rs->flags & RSF_XFER_FUNC))) {
			/* 
			   perform adjacent restart procedure */
			fixme(("Write this procedure\n"));
		}
	}
	if (rs_oldstate == RS_RESTRICTED) {
		struct rr *rr, *rr_next = rs->rr.list;
		rs_timer_stop(rs, t11);
		rs_timer_stop(rs, t18a);
		rs->flags &= ~RSF_TFR_PENDING;
		/* 
		   purge responsive TFR list */
		while ((rr = rr_next)) {
			rr_next = rr->rs.next;
			mtp_free_rr(rr);
		}
	}
	if (rs_newstate < RS_RESTRICTED && rs_oldstate >= RS_RESTRICTED) {
		/* 
		   transfer allowed broadcast */
		if ((err = mtp_tfa_broadcast(q, rs)) < 0)
			return (err);
	}
	if (rs_newstate == RS_RESTRICTED && (sp->na.na->option.popt & SS7_POPT_TFR)) {
		if (rs_oldstate < RS_RESTRICTED) {
			/* 
			   prepare for transfer restricted broadcast */
			rs_timer_start(rs, t11);
		}
		if (rs_oldstate > RS_RESTRICTED) {
			/* 
			   transfer restricted broadcast */
			if ((err = mtp_tfr_broadcast(q, rs)) < 0)
				return (err);
		}
	}
	if (rs_newstate > RS_RESTART && rs_oldstate <= RS_RESTART) {
		/* 
		   transfer prohibited broadcast */
		if ((err = mtp_tfp_broadcast(q, rs)))
			return (err);
	}
	rs->state = rs_newstate;
      no_state_change:
	rs->flags = rs_newflags;
      no_flags_change:
	return (QR_DONE);
}

STATIC int
rl_set_state(queue_t *q, struct rl *rl, const ulong state)
{
	struct rs *rs = rl->rs.rs;
	ulong rl_oldstate = rl->state;
	ulong rl_newstate = rl->state;
	ulong rl_oldflags = rl->flags;
	ulong rl_newflags = rl->flags;
	int err;
	if ((rl_newflags = mtp_sta_flags(state, rl_oldflags)) == rl_oldflags)
		goto no_flags_change;
	if ((rl_newstate = mtp_flg_state(rl_newflags)) == rl_oldstate)
		goto no_state_change;
	if (rl_newstate > RL_RESTRICTED && rl_oldstate < RL_RESTART) {
		/* 
		   route list moved from accessible to inaccessible */
		if (rl == rs->rl.curr) {
			/* 
			   inaccessible route-list was current route-list */
			struct rl *ra;
			for (ra = rs->rl.list; ra && (ra == rl || ra->state > RL_RESTRICTED);
			     ra = ra->rs.next) ;
			rl_reroute(rs, ra, 1);
			goto reroute;
		}
	}
	if (rl_oldstate > RL_RESTRICTED && rl_newstate < RL_RESTART) {
		/* 
		   route list moved from inaccessible to accessible */
		if (!rs->rl.curr || rs->rl.curr->cost > rl->cost) {
			/* 
			   accessible route-list is highest priority available route-list */
			rl_reroute(rs, rl, 0);
			goto reroute;
		}
	}
	if (rl == rs->rl.curr) {
	      reroute:
		(&rs->rl.allowed)[rl_oldstate]--;
		(&rs->rl.allowed)[rl_newstate]++;
		if ((err = rs_set_state(q, rs, state))) {
			(&rs->rl.allowed)[rl_oldstate]++;
			(&rs->rl.allowed)[rl_newstate]--;
			return (err);
		}
	} else {
		(&rs->rl.allowed)[rl_oldstate]--;
		(&rs->rl.allowed)[rl_newstate]++;
	}
	/* 
	   push counts to ls */
	(&rl->ls.ls->rl.allowed)[rl_oldstate]--;
	(&rl->ls.ls->rl.allowed)[rl_newstate]++;
	/* 
	   What we are more interested in than pushing state here is determining the route loading
	   of the link, and whether the link is in danger of congestion due to route loading.  That 
	   is a difficult thing to calculate, however.  The link can only be in danger of
	   congestion if the routes assigned to it are actually active.  This might be better
	   considered at the route-list/link-set level rather than at the route/link level */
	rl->state = rl_newstate;
      no_state_change:
	rl->flags = rl_newflags;
      no_flags_change:
	return (QR_DONE);
}

STATIC int
rt_set_state(queue_t *q, struct rt *rt, const ulong state)
{
	struct rl *rl = rt->rl.rl;
	ulong rl_newstate;
	ulong rt_oldstate = rt->state;
	ulong rt_newstate = rt->state;
	ulong rt_oldflags = rt->flags;
	ulong rt_newflags = rt->flags;
	int err;
	if ((rt_newflags = mtp_sta_flags(state, rt_oldflags)) == rt_oldflags)
		goto no_flags_change;
	if ((rt_newstate = mtp_flg_state(rt_newflags)) == rt_oldstate)
		goto no_state_change;
	/* 
	   ---------------------------------------------- */
	if (rt_newflags & (RTF_PROHIBITED | RTF_RESTRICTED)) {
		/* 
		   start signalling route set test procedure */
		rt_timer_start(rt, t10);
	} else {
		/* 
		   stop signalling route set test procedure */
		rt_timer_stop(rt, t10);
	}
	if (rt_oldstate < RT_RESTART && rt_newstate > RT_RESTRICTED) {
		int accessible = rl->rt.allowed + rl->rt.danger + rl->rt.congested;
		int restricted = rl->rt.restricted;
		if (rt_newstate != RT_RESTRICTED) {
			/* 
			   route has moved from accessible to inaccessible */
			if (accessible > 1) {
				/* 
				   offload traffic to other accessible routes */
				int maxload = RT_SMAP_SIZE / (accessible - 1) + 1;
				rt_reroute_sub(rl, rt, maxload);
			} else if (restricted) {
				/* 
				   offload traffic to restricted routes */
				int maxload = RT_SMAP_SIZE / restricted + 1;
				rt_reroute_res(rl, rt, maxload);
			} else {
				/* 
				   we are the last accessible route in route list */
				/* 
				   empty route list */
				rt_reroute_all(rl, NULL, 1);
			}
		} else {
			/* 
			   route has moved from restricted to inaccessible */
			if (!accessible && restricted > 1) {
				/* 
				   route was carrying traffic */
				/* 
				   offload traffic to other restricted routes */
				int maxload = RT_SMAP_SIZE / (restricted - 1) + 1;
				rt_reroute_sub(rl, rt, maxload);
			} else if (!accessible) {
				/* 
				   we are the last accessible route in route list */
				/* 
				   empty route list */
				rt_reroute_all(rl, NULL, 1);
			}
		}
	}
	if (rt_newstate < RT_RESTART && rt_oldstate > RT_RESTRICTED) {
		int accessible = rl->rt.allowed + rl->rt.danger + rl->rt.congested;
		int restricted = rl->rt.restricted;
		if (rt_oldstate != RT_RESTRICTED) {
			/* 
			   route has moved from inaccessible to accessible */
			if (accessible == 0) {
				/* 
				   only allowed route in route list */
				/* 
				   take over all traffic */
				rt_reroute_all(rl, rt, 0);
			} else {
				/* 
				   other routes are allowed */
				/* 
				   rebalance the route load */
				int maxload = RT_SMAP_SIZE / (accessible + 1) + 1;
				rt_reroute_add(rl, rt, maxload);
			}
		} else {
			/* 
			   route has moved from inaccessible to restricted */
			if (accessible + restricted == 0) {
				/* 
				   only accessible route in route list */
				/* 
				   take over all traffic */
				rt_reroute_all(rl, rt, 0);
			} else if (!accessible) {
				/* 
				   other routes are restricted but none are allowed */
				/* 
				   rebalance the route load */
				int maxload = RT_SMAP_SIZE / (restricted + 1) + 1;
				rt_reroute_add(rl, rt, maxload);
			}
		}
	}
	/* 
	   ---------------------------------------------- */
	/* 
	   push state change to rl */
	(&rl->rt.allowed)[rt_oldstate]--;
	(&rl->rt.allowed)[rt_newstate]++;
	if ((rl_newstate = mtp_cnt_state((struct counters *) (&rl->rt), rl->state)) != rl->state)
		if ((err = rl_set_state(q, rl, rl_newstate)) < 0) {
			(&rl->rt.allowed)[rt_oldstate]++;
			(&rl->rt.allowed)[rt_newstate]--;
			return (err);
		}
	/* 
	   push counts to lk */
	(&rt->lk.lk->rt.allowed)[rt_oldstate]--;
	(&rt->lk.lk->rt.allowed)[rt_newstate]++;
	/* 
	   What we are more interested in than pushing state here is determining the route loading
	   of the link, and whether the link is in danger of congestion due to route loading.  That 
	   is a difficult thing to calculate, however.  The link can only be in danger of
	   congestion if the routes assigned to it are actually active.  This might be better
	   considered at the route-list/link-set level rather than at the route/link level */
#if 0
	if ((lk_newstate = mtp_cnt_state((struct counters *) (&lk->rt), lk->state)) != lk->state) {
		/* 
		   don't know quite what to do with this... */
	}
#endif
	rt->state = rt_newstate;
      no_state_change:
	rt->flags = rt_newflags;
      no_flags_change:
	return (QR_DONE);
}

STATIC int
ls_set_state(queue_t *q, struct ls *ls, ulong state)
{
	struct sp *sp = ls->sp.sp;
	struct rl *rl;
	ulong sp_newstate;
	ulong rl_newstate;
	ulong ls_oldstate = ls->state;
	ulong ls_newstate = ls->state;
	ulong ls_oldflags = ls->flags;
	ulong ls_newflags = ls->flags;
	int err;
	if ((ls_newflags = mtp_sta_flags(state, ls_oldflags)) == ls_oldflags)
		goto no_flags_change;
	if ((ls_newstate = mtp_flg_state(ls_newflags)) == ls_oldstate)
		goto no_state_change;
	/* 
	   --------------------------------------------- */
	/* 
	   --------------------------------------------- */
	/* 
	   push state change to rl */
	if (ls_newstate == SL_INACTIVE)
		rl_newstate = MTP_BLOCKED;
	else
		rl_newstate = MTP_UNBLOCKED;
	for (rl = ls->rl.list; rl; rl = rl->ls.next)
		if (rl_newstate != rl->state)
			if ((err = rl_set_state(q, rl, rl_newstate)) < 0)
				return (err);
	/* 
	   --------------------------------------------- */
	/* 
	   push state change to sp */
	(&sp->ls.allowed)[ls_oldstate]--;
	(&sp->ls.allowed)[ls_newstate]++;
	if ((sp_newstate = mtp_cnt_state((struct counters *) (&sp->ls), sp->state)) != sp->state)
		if ((err = sp_set_state(q, sp, sp_newstate)) < 0) {
			(&sp->ls.allowed)[ls_oldstate]++;
			(&sp->ls.allowed)[ls_newstate]--;
			return (err);
		}
	ls->state = ls_newstate;
      no_state_change:
	ls->flags = ls_newflags;
      no_flags_change:
	return (QR_DONE);
}

STATIC int
lk_set_state(queue_t *q, struct lk *lk, ulong state)
{
	struct ls *ls = lk->ls.ls;
	struct rt *rt;
	ulong ls_newstate;
	ulong rt_newstate;
	ulong lk_oldstate = lk->state;
	ulong lk_newstate = lk->state;
	ulong lk_oldflags = lk->flags;
	ulong lk_newflags = lk->flags;
	int err;
	if ((lk_newflags = mtp_sta_flags(state, lk_oldflags)) == lk_oldflags)
		goto no_flags_change;
	if ((lk_newstate = mtp_flg_state(lk_newflags)) == lk_oldstate)
		goto no_state_change;
	/* 
	   --------------------------------------------- */
	/* 
	   --------------------------------------------- */
	/* 
	   push state change to rt */
	if (lk_newstate == SL_INACTIVE)
		rt_newstate = MTP_BLOCKED;
	else
		rt_newstate = MTP_UNBLOCKED;
	for (rt = lk->rt.list; rt; rt = rt->lk.next)
		if (rt_newstate != rt->state)
			if ((err = rt_set_state(q, rt, rt_newstate)) < 0)
				return (err);
	/* 
	   --------------------------------------------- */
	/* 
	   push state change to ls */
	(&ls->lk.allowed)[lk_oldstate]--;
	(&ls->lk.allowed)[lk_newstate]++;
	if ((ls_newstate = mtp_cnt_state((struct counters *) (&ls->lk), ls->state)) != ls->state)
		if ((err = ls_set_state(q, ls, ls_newstate)) < 0) {
			(&ls->lk.allowed)[lk_oldstate]++;
			(&ls->lk.allowed)[lk_newstate]--;
			return (err);
		}
	lk->state = lk_newstate;
      no_state_change:
	lk->flags = lk_newflags;
      no_flags_change:
	return (QR_DONE);
}

STATIC int
sl_set_state(queue_t *q, struct sl *sl, ulong state)
{
	struct lk *lk = sl->lk.lk;
	ulong lk_newstate;
	ulong sl_oldstate = sl->state;
	ulong sl_newstate = sl->state;
	ulong sl_oldflags = sl->flags;
	ulong sl_newflags = sl->flags;
	int err;
	if ((sl_newflags = mtp_sta_flags(state, sl_oldflags)) == sl_oldflags)
		goto no_flags_change;
	if ((sl_newstate = mtp_flg_state(sl_newflags)) == sl_oldstate)
		goto no_state_change;
	/* 
	   --------------------------------------------- */
	if (sl_newstate < SL_UNUSABLE && sl_oldstate > SL_CONGESTED) {
		/* 
		   link has become usable from unusable */
		/* 
		   in all cases when we move to active, we could have our own changeover buffers
		   hanging around or we could have links that at the time that they initiated
		   changeover, they had no alternate (but they do now). */
		/* 
		   redistribute traffic to the new link if required */
		int active =
		    lk->sl.allowed + lk->sl.danger + lk->sl.congested + lk->sl.restricted + 1;
		int maxload = SL_SMAP_SIZE / active + 1;
		sl_changeback_add(lk, sl, maxload);
	}
	if (sl_newstate == SL_INHIBITED && sl_oldstate < SL_UNUSABLE) {
		/* 
		   inihibited from usable */
		/* 
		   we already have a changeover buffer allocated at the beginning of the inhibition 
		   process, this is just the final state change.  We should check if there are any
		   changeovers or changebacks to us and redirect them. */
		fixme(("Write this function\n"));
	}
	if (sl_oldstate < SL_UNUSABLE && sl_newstate > SL_CONGESTED) {
		/* 
		   unusable (blocked or inactive) from usable */
		int active = lk->sl.allowed + lk->sl.danger + lk->sl.congested;
		int inhibited = lk->sl.restricted;
		if (sl_oldstate != SL_INHIBITED) {
			/* 
			   link has moved from active to unusable */
			if (active > 1) {
				/* 
				   offload traffic to other active links */
				int maxload = SL_SMAP_SIZE / (active - 1) + 1;
				sl_changeover_sub(lk, sl, maxload);
			} else if (inhibited) {
				/* 
				   offload traffic to inhibited links */
				int maxload = SL_SMAP_SIZE / inhibited + 1;
				sl_changeover_res(lk, sl, maxload);
			}
		} else {
			/* 
			   link has moved from inhibited to unusable */
			/* 
			   link was not carrying traffic */
		}
	}
	/* 
	   --------------------------------------------- */
	/* 
	   push state change to lk */
	(&lk->sl.allowed)[sl_oldstate]--;
	(&lk->sl.allowed)[sl_newstate]++;
	if ((lk_newstate = mtp_cnt_state((struct counters *) (&lk->sl), lk->state)) != lk->state)
		if ((err = lk_set_state(q, lk, lk_newstate)) < 0) {
			(&lk->sl.allowed)[sl_oldstate]++;
			(&lk->sl.allowed)[sl_newstate]--;
			return (err);
		}
	sl->state = sl_newstate;
      no_state_change:
	sl->flags = sl_newflags;
      no_flags_change:
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Lookup Functions
 *
 *  -------------------------------------------------------------------------
 */
#if 0
/*
 *  Select the route set to be used for routing messages on the specified
 *  signalling point.
 */
STATIC INLINE struct rs *
rs_find(struct sp *sp, uint32_t dest, int type)
{
	struct rs *rs;
	for (rs = sp->rs.list; rs; rs = rs->sp.next)
		if (rs->dest == dest && rs->rs_type == type)
			break;
	return (rs);
}

/*
 *  Select the route list to be used for routing messages on the specified
 *  route set.
 */
STATIC INLINE struct rl *
rl_find(struct rs *rs, int state)
{
	struct rl *rl;
	for (rl = rs->rl.list; rl && rl->state != state; rl = rl->rs.next) ;
	return (rl);
}

/*
 *  Select the route to be used for routing messages in the specified route list.
 */
STATIC INLINE struct rt *
rt_find(struct rl *rl, int state)
{
	struct rt *rt;
	for (rt = rl->rt.list; rt && rt->state != state; rt = rt->rl.next) ;
	return (rt);
}
#endif

/*
 *  LOOKUP ROUTESET (LOCAL)
 *  -----------------------------------
 *  Find a local route set via the signalling link with the specified destination and type.
 */
STATIC INLINE struct rs *
mtp_lookup_rs_local(sl, dest, type)
	struct sl *sl;			/* signalling link for the local route set */
	uint32_t dest;			/* destination of the route set */
	int type;			/* type of route set */
{
	struct rt *rt;
	struct rl *rl;
	struct rs *rs = NULL;
	for (rt = sl->lk.lk->rt.list; rt; rt = rt->lk.next) {
		if ((rl = rt->rl.rl) && (rs = rl->rs.rs) && rs->dest == dest && rs->rs_type == type)
			break;
		rs = NULL;
	}
	return (rs);
}

/*
 *  LOOKUP ROUTESET
 *  -----------------------------------
 *  Lookup the route set to which a route set related message pertains with appropriate security screening.  Route
 *  set related messages include TFC, UPU, UPA, and UPT.
 */
STATIC struct rs *
mtp_lookup_rs(sl, m, type)
	struct sl *sl;			/* the signalling link the message came in on */
	struct mtp_msg *m;		/* the decoded message */
	uint type;			/* type of destination in message */
{
	struct rs *rs;
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;
	/* 
	   Route set related message which arrive on link sets upon which we have no existing route 
	   to the specified destination are suspect and should be ignored.  Several DoS attacks are 
	   possible without this screening. */
	if (sp->flags & SPF_SECURITY) {
		if (!mtp_lookup_rs_local(sl, m->dest, type))
			goto error1;
		if (!(rs = mtp_lookup_rs_local(sl, m->opc, RT_TYPE_MEMBER)))
			goto error2;
		if (m->opc != m->dest && !(rs->flags & RSF_XFER_FUNC))
			goto error3;
	}
	for (rs = sp->rs.list; rs; rs = rs->sp.next)
		if (rs->dest == m->dest && rs->rs_type == type)
			goto found;
      found:
	if (rs)
		return (rs);
	goto error7;
      error1:
	ptrace(("PROTOCOL ERROR: Route set message: no local route to destination\n"));
	goto error;
      error2:
	ptrace(("PROTOCOL ERROR: Route set message: no local route to originator\n"));
	goto error;
      error3:
	ptrace(("PROTOCOL ERROR: Route set message: originator non-STP\n"));
	goto error;
      error7:
	ptrace(("PROTOCOL ERROR: Route set message: no route to destination\n"));
	goto error;
      error:
	todo(("Deliver screened message to MTP management\n"));
	return (NULL);
}

/*
 *  LOOKUP ROUTE (LOCAL)
 *  -----------------------------------
 *  Find a local route via the signalling link with the specified destination and type.
 */
STATIC INLINE struct rt *
mtp_lookup_rt_local(struct sl *sl, uint32_t dest, int type)
{
	struct rt *rt;
	struct rl *rl;
	struct rs *rs;
	for (rt = sl->lk.lk->rt.list; rt; rt = rt->lk.next)
		if ((rl = rt->rl.rl) && (rs = rl->rs.rs) && rs->dest == dest && rs->rs_type == type)
			break;
	return (rt);
}

/*
 *  LOOKUP ROUTE (TEST)
 *  -----------------------------------
 *  Lookup the route to which a route related test message pertains with appropriate screening.  Route related test
 *  messages include RST, RSR, RCP, and RCR.
 */
STATIC INLINE struct rt *
mtp_lookup_rt_test(sl, m, type)
	struct sl *sl;			/* the signalling link the message came in on */
	struct mtp_msg *m;		/* the decoded message */
	uint type;			/* type of destination in message */
{
	struct sp *sp = sl->lk.lk->sp.loc;
	struct ls *ls;
	struct lk *lk;
	struct rs *rs;
	struct rt *rt = NULL;
	if (sp->flags & SPF_SECURITY) {
		if (!(sp->flags & SPF_XFER_FUNC))
			goto error1;
		if (type == RT_TYPE_CLUSTER && !(sp->flags & SPF_CLUSTER))
			goto error2;
		if (!(rs = mtp_lookup_rs_local(sl, m->opc, RT_TYPE_MEMBER)))
			goto error3;
		if (!(rs->flags & RSF_ADJACENT))
			goto error4;
		if (m->dest == sp->pc)
			goto error5;
		if (!mtp_lookup_rs_local(sl, m->dest, type))
			goto error6;
	}
	for (ls = sp->ls.list; ls; ls = ls->sp.next)
		for (lk = ls->lk.list; lk; lk = lk->ls.next)
			if ((rs = lk->sp.adj)->dest == m->opc
			    && ((!(rs->flags & RSF_CLUSTER) && type == RT_TYPE_MEMBER)
				|| ((rs->flags & RSF_CLUSTER) && type == RT_TYPE_CLUSTER)))
				for (rt = lk->rt.list; rt; rt = rt->lk.next)
					if ((rs = rt->rl.rl->rs.rs) && rs->dest == m->dest
					    && rs->rs_type == type)
						goto found;
      found:
	if (rt)
		return (rt);
	goto error7;
      error1:
	ptrace(("PROTOCOL ERROR: Route set test message: local is non-STP\n"));
	goto error;
      error2:
	ptrace(("PROTOCOL ERROR: Route set test message: no cluster support\n"));
	goto error;
      error3:
	ptrace(("PROTOCOL ERROR: Route set test message: no local route to originator\n"));
	goto error;
      error4:
	ptrace(("PROTOCOL ERROR: Route set test message: originator not adjacent\n"));
	goto error;
      error5:
	ptrace(("PROTOCOL ERROR: Route set test message: testing local adjacent\n"));
	goto error;
      error6:
	ptrace(("PROTOCOL ERROR: Route set test message: no local route to destination\n"));
	goto error;
      error7:
	ptrace(("PROTOCOL ERROR: Route set test message: no route to destination\n"));
	goto error;
      error:
	todo(("Deliver screened message to MTP management\n"));
	return (NULL);
}

/*
 *  LOOKUP ROUTE
 *  -----------------------------------
 *  Lookup the route to which a route related message pertains with appropriate security screening.  Route related
 *  messages include TFA, TFR, TFP, TCA, TCR, TCP.
 */
STATIC INLINE struct rt *
mtp_lookup_rt(sl, m, type)
	struct sl *sl;			/* the signalling link the message came in on */
	struct mtp_msg *m;		/* the message */
	uint type;			/* type of destination in message */
{
	struct sp *sp = sl->lk.lk->sp.loc;
	struct ls *ls;
	struct lk *lk;
	struct rs *rs;
	struct rt *rt = NULL;
	if (sp->flags & SPF_SECURITY) {
		if (!(sl->lk.lk->sp.adj->flags & RSF_XFER_FUNC))
			goto error1;
		if (type == RT_TYPE_CLUSTER && !(sp->flags & RSF_CLUSTER))
			goto error2;
		if ((type == RT_TYPE_MEMBER && m->dest == m->opc)
		    || (type == RT_TYPE_CLUSTER && m->dest == (m->opc & sp->na.na->mask.cluster)))
			goto error3;
		if (!(rs = mtp_lookup_rs_local(sl, m->opc, RT_TYPE_MEMBER)))
			goto error4;
		if (!(rs->flags & RSF_ADJACENT))
			goto error5;
		if (!mtp_lookup_rs_local(sl, m->dest, type))
			goto error6;
	}
	for (ls = sp->ls.list; ls; ls = ls->sp.next)
		for (lk = ls->lk.list; lk; lk = lk->ls.next)
			if ((rs = lk->sp.adj)->dest == m->opc
			    && ((!(rs->flags & RSF_CLUSTER) && type == RT_TYPE_MEMBER)
				|| ((rs->flags & RSF_CLUSTER) && type == RT_TYPE_CLUSTER)))
				for (rt = lk->rt.list; rt; rt = rt->lk.next)
					if ((rs = rt->rl.rl->rs.rs) && rs->dest == m->dest
					    && rs->rs_type == type)
						goto found;
      found:
	if (rt)
		return (rt);
	goto error7;
      error1:
	ptrace(("PROTOCOL ERROR: Route related message: from adjacent non-STP\n"));
	goto error;
      error2:
	ptrace(("PROTOCOL ERROR: Route related message: no cluster support\n"));
	goto error;
      error3:
	ptrace(("PROTOCOL ERROR: Route related message: concerning adjacent\n"));
	goto error;
      error4:
	ptrace(("PROTOCOL ERROR: Route related message: no local route to originator\n"));
	goto error;
      error5:
	ptrace(("PROTOCOL ERROR: Route related message: from non-adjacent\n"));
	goto error;
      error6:
	ptrace(("PROTOCOL ERROR: Route related message: no local route to destination\n"));
	goto error;
      error7:
	ptrace(("PROTOCOL ERROR: Route related message: no route to destination\n"));
	goto error;
      error:
	todo(("Deliver screened message to MTP management\n"));
	return (NULL);
}

/*
 *  LOOKUP SIGNALLING LINK
 *  -----------------------------------
 *  Lookup the signalling link to which a signalling link related message pertains.
 */
STATIC struct sl *
mtp_lookup_sl(sl, m)
	struct sl *sl;			/* the signalling link the message came in on */
	struct mtp_msg *m;		/* the message */
{
	struct sp *sp = sl->lk.lk->sp.loc;
	struct ls *ls;
	struct lk *lk;
	struct sl *s2 = NULL;
	/* 
	   Signalling link related messages which arrive on link sets upon which we have no
	   existing route to the adjacent signalling point are suspect and should be ignored.
	   Several DoS attacks are possible without this screening. */
	if (sp->flags & SPF_SECURITY)
		if (!mtp_lookup_rt_local(sl, m->opc, RT_TYPE_MEMBER)
		    && !mtp_lookup_rt_local(sl, m->opc & sp->na.na->mask.cluster, RT_TYPE_CLUSTER))
			goto screened;
	for (ls = sp->ls.list; ls; ls = ls->sp.next)
		for (lk = ls->lk.list; lk; lk = lk->ls.next)
			if (lk->sp.adj->dest == m->opc)
				for (s2 = lk->sl.list; s2; s2 = s2->lk.next)
					if (s2->slc == m->slc)
						goto found;
      found:
	return (s2);
      screened:
	todo(("Deliver screened message to MTP management\n"));
	return (NULL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Signalling Point Functions
 *
 *  -------------------------------------------------------------------------
 *  3.6 Status of signalling points.  A signalling point can be in two states: available and unavailable (see Figure
 *  6).  However, implementation congestion states may exist.
 *
 *  3.6.1 Signalling point unavailability
 *
 *  3.6.1.1 Unavailability of a signalling point itself.  A signalling point becomes unavailable when all connected
 *  signalling links are unavailable.
 *
 *  3.6.1.2 Unavailibility of an adjacent signalling point.  A signalling point considers that an adjacent
 *  signalling point becomes unavailable when: all signalling links connected to the adjacent signalling point are
 *  unavailable; and, the adjacent signalling point is inaccessible.
 *
 *  3.6.2 Signalling point availability
 *
 *  3.6.2.1 Availbility of a signalling point itself.  A signalling point becomes available when at least one
 *  signalling link connected to this signalling point becomes available.
 *
 *  3.6.2.2 Availability of an adjacent signalling point.  A signalling point considers that an adjacent signalling
 *  point Y becomes available when: 1) at least one signalling link conencted to Y becomes available at level 3 and
 *  the MTP restart procedure (see clause 9) has been completed; or, 2) the adjacent signalling point Y becomes
 *  accessible: on the receipt of a transfer allowed message or a transfer restricted message; if an alternative
 *  route becomes available again via the corresponding local linkset; or, if a traffic restart allowed message from
 *  another adjacent signalling point Z, whose MTP is restarting, is received so that a route towards Y using Z
 *  becomes available.
 *
 *  3.7 Procedures used in connection with point status changes.
 *
 *  3.7.1 Signalling point unavailable.
 *
 *  There is no specific procedures used when a signalling point becomes unavailable.  The transfer prohibited
 *  procedure is used to update the status of the recovered routes in all nodes of the signalling network (see
 *  13.2).
 *
 *  3.7.2. Signalling point available.
 *
 *  3.7.2.1 Signalling traffic management: the MTP restart procedures (see clause 9) is applied; it is used to
 *  restart the traffic between the signalling network and the signalling point which becomes available.  This
 *  restart is based on the following criteria: avoid loss of messages; limit the level 3 load due to the restart of
 *  a signalling point's MTP; restart, as much as possible, simultaneously in both directions of the signalling
 *  relations.
 *
 *  3.7.2.2 Signalling link management: The first step of the MTP restart procedure consists of updating the
 *  signalling route states before carrying traffic to the point which becomes available and in all adjacent points;
 *  the transfer prohibited and transfer restricted procedures are used (see clause 13).
 *
 *  3.7.3 Signalling point congested: (implementation-dependent option, see 11.2.6).
 *
 *  4.8 Signalling point availabilty
 *
 *  When a previously unavailable signalling point becomes available (see 3.6), signalling traffic may be
 *  transferred to the available point by means of the MTP restart procedure (see clause 9).
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Route Functions
 *
 *  -------------------------------------------------------------------------
 *
 *  3.4 Status of signalling routes.  A signalling route can be in three states for signalling traffic having the
 *  concerned destination; these are available, restricted, unavailable (see also Figure 6).
 *
 *  3.4.1 Signalling route unavailabilty.  A signalling route becomes unavailable when a transfer-prohibited
 *  message, indicating that signalling traffic towards a particular desintation cannot be transferred via the
 *  signalling transfer point sending the concerned message is received (see clause 13).
 *
 *  3.4.2 Signalling route availability.  A signalling route becomes available when a trasfer-allowed message,
 *  indicating that signalling traffic towards a particular destination can be transferred via the signalling
 *  transfer point sending the concerned message, is received (see clause 13).
 *
 *  3.4.3 Signalling route restricted.  A signalling route becomes restricted when a transfer-restricted message,
 *  indicating that the signalling traffic towards a particular destination is being transferred with some
 *  difficulty via the signalling transfer point sending the concerned message is received (see clause 13).
 *
 *  3.5 Procedures used in connection with route status changes.  In this subclause, the procedures related to each
 *  signalling management function, which in general are applied in connection with route status changes, are
 *  listed.  See also Figures 6 and 8.  Typical examples of the application of the procedures to particular newtork
 *  cases appear in Recommendation Q.705.
 *
 *  3.5.1 Signalling route unavailable
 *
 *  3.5.1.1  Signalling traffic management: the forced rerouting procedure (see clause 7) is applied; it is used to
 *  transfer signalling traffic to the concerned destination from the link set, belonging to the unavailable route,
 *  to an alternative link set which terminates in another signalling transfer point.  It includes actions to
 *  determine the alternative route.
 *
 *  3.5.1.2  Signalling route management: because of the unavailability of the signalling route, the network is
 *  reconfigured; in the case that a signalling transfer point can no longer route the concerned signalling traffic,
 *  it applies the procedures described in clause 13.
 *
 *  3.5.2 Signalling route available
 *
 *  3.5.2.1 Signalling traffic management: the controlled rerouting procedure (see clause 8) is applied; it is used
 *  to transfer signalling traffic to the concerned destination from a signalling link or link set belonging to an
 *  available route, to another link set which terminates in another signalling transfer point.  It includes the
 *  determination of which traffic should be diverted and procedure for maintaining the correct message sequence.
 *
 *  3.5.2.2 Signalling route management: because of the restored availability of the signalling route, the network
 *  is reconfigured; in the case that a signalling transfer point can once again route the concerned signalling
 *  traffic, it applies the procedures described in clause 13.
 *
 *  3.5.3 Signalling route restricted
 *
 *  3.5.3.1 Signalling traffic management: the controlled rerouting procedure (see clause 8) is applied; it is used
 *  to transfer signalling traffic to the concerned destination from the link set belonging to the restricted route,
 *  to an alternative link set if one is available to give more, if possible, efficient routing.  It includes
 *  actions to determine the alternative route.
 *
 *  3.5.3.2 Signalling route management: because of restricted availability of the signalling route, the network
 *  routine is, if possible, reconfigured; procedure described in clause 13 are used to advise adjacent signalling
 *  points.
 *
 *  4.5 Signalling route unavailability
 *
 *  When a signalling route becomes unavailable (see 3.4), signalling traffic currently carried by the unavailable
 *  route is transferred to an alternative route by means of forced re-routing procedure.  The alternative route
 *  (i.e, the alternative link set or link sets) is determined in accordance with the alternative routing defined
 *  for the concerned destination (see 4.3.3).
 *
 *  4.6 Signalling route availability
 *
 *  When a previously unavailable signalling route becomes available again (see 3.4), signalling traffic may be
 *  transferred to the available route by means of a controlled rerouting procedure.  This is applicable in the case
 *  when the available route (link set) has higher priority than the route (link set) currently used for traffic to
 *  the concerned destination (see 4.4.3).
 *
 *  The transferred traffic is distributed over the links of the new link set in accordance with the routing
 *  currently applicable for that link set.
 *
 *  4.7 Signalling route restriction
 *
 *  When a signalling route becomes restricted (see 3.4), signalling carried by the restricted route is, if
 *  possible, transferred to an alternative route by means of the controlled rerouting procedure, if an equal
 *  priority alternative route is available and not restricted.  The alternative route is determined in accordance
 *  with the alternate routing defined for the concerned destination (see 4.3.3).
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Signalling Link Functions
 *
 *  -------------------------------------------------------------------------
 *  ANSI T1.111.4-2000 ...
 *
 *  3.2 Status of Signalling Links
 * 
 *  3.2.1 A signalling link is always considered by level 3 in one of two possible major states: available
 *  and unavailable.  Depending on the cause of unavailability, the unavailable state can be subdivided into seven
 *  possible cases as follows (see also Figure 6/T1.111.4):
 *
 *  (1) Unavailable, failed or inactive
 *  (2) Unavailable, blocked
 *  (3) Unavailable, (failed or inactive) and blocked
 *  (4) Unavailable, inhibited
 *  (5) Unavailable, inhibited and (failed or inactive)
 *  (6) Unavailable, inhibited and blocked
 *  (7) Unavailable, (failed or inactive), blocked, and inhibited
 *
 *  The concerned link can be used to carry signalling traffic only if it is available except test andmanagement
 *  messages (e.g., SLT, TFx, TRA and TRW).  Eight possible events can change the status of a link: signalling link
 *  failure, restoration, deactivation, activation, blocking, unblocking, inhibiting, anduninhibiting: they are
 *  described in 3.2.2 through 3.2.9.
 * 
 *  3.2.2 Signalling Link Failure.  A signalling link (in-service or blocked, see 3.2.6) is recognized by level 3 as
 *  failed when:
 *
 *  (1) A link failure indication is obtained from level 2.
 *
 *      For MTP Level 2 links, the indication may be caused by:
 *
 *      (a) Intolerably high signal unit error  rate (see Section 10 of Chapter T1.111.3)
 *      (b) Excessive length of the realignment period (see 4.1 and Section 7 of Chapter T1.111.3)
 *      (c) Excessive delay of acknowledgments (see 5.3 and 6.3 of Chapter T1.111.3)
 *      (d) Excessive period of level 2 congestion (se e Section 9 of Chapter T1.111.3)
 *      (e) Failure of signalling terminal equipment
 *      (f) Two out of three unreasonable backward sequence numbers or forward indicator bits (see 5.3and 6.3 of
 *          Chapter T1.111.3)
 *      (g) Reception of consecutive link status signal units  indicating out-of-alignment, out-of-service,normal or
 *          emergency terminal status (see 1.7 of Chapter T1.111.3) The first two conditions are detected by the
 *          signal unit error rate monitor (see Section 10 of ChapterT1.111.3).
 *
 *      For SAAL links, failure indic ations include:
 *
 *      (a) Intolerably high protocol data unit error rate (see Section 8.1.1 of T1.652 [7])
 *      (b) Excessive length of the realignment period (see Section 6.2.2 of T1.645 [8])
 *      (c) No receipt of a status (STAT) protocol data unit - timer No_Response e xpiration (see Section 7.6 of
 *          T1.637 [2])
 *      (d) Excessive period of no credit (see Section 8.1.2 of T1.652)
 *      (e) Local or remote processor outage condition (see Section 6.2.1 of T1.645)
 *
 *  (2) A request (automatic or manual) is obtained from a management or maintenance system.
 *
 *  (3) The signalling link test fails (see Section 2.2 of Chapter T1.111.7).
 *
 *  (4) False link congestion is detected (see 3.8.2.2).  Moreover, a signalling link that is available (not
 *      blocked) is recognized by level 3 as failed wh en achangeover order is received.
 *
 *  3.2.3 Signalling Link Restoration.   A previously failed signalling link is restored when both ends of
 *  thesignalling link have successfully completed an initial alignment procedure (see Section 7 of Chapter T1.111.3
 *  or Section 6.2.2 of T1.645).
 *
 *  3.2.4 Signalling Link Deactivation.  A signalling link (in-service, failed or blocked) is recognized bylevel 3
 *  as deactivated (i.e., removed from operation) when:
 *
 *  (1) A request is obtained from the signalling link management f unction (see Section 12)
 *
 *  (2) A request (automatic or manual) is obtained from an external management or maintenance system
 *
 *  3.2.5 Signalling Link Activation.  A previously inactive signalling link is recognized by level 3 as
 *  activatedwhen both ends of the signalling link have successfully completed an initial alignment procedure (see
 *  Section 7 of Chapter T1.111.3 or Section 6.2.2 of T1.645).
 *
 *  3.2.6 Signalling Link Blocking.  A signalling link (in service, failed, or inactive) is recognized as
 *  blockedwhen an indication is obtained from the signalling terminal that a processor outage condition exists at
 *  the remote terminal (i.e., link status signal units with processor outage indication are received, see Section 8
 *  ofChapter T1.111.3).
 *
 *  NOTE: A link becomes unavailable when it is failed or deactivated or ((failed or deactivated) and blocked) (see
 *  Figure 6/T1.111.4).
 *
 *  NOTE:  For SAAL links, a processor outage condition causes the link to fail.
 *
 *  3.2.7 Signalling Link Unblocking.  A signalling link previously blocked is unblocked when an indicationis
 *  obtained from the signalling terminal that the processor outage condition has ceased at the remote terminal.
 *  (Applies in the case when the processor outage condition was initiated by the remote terminal.).
 *
 *  NOTE: A link becomes available when it is restored, or activated, or unblocked, or ((restored or activated) and
 *  unblocked) (see Figure 6/T1.111.4).
 *
 *  3.2.8 Signalling Link Inhibiting.  In U. S. networks the standard for management control of signallinglinks is
 *  the management inhibit procedure.  A signalling link is recognized as inhibited when:
 *
 *  (1) An acknowledgment is received from a remote signalling point in response to an inhibit request sentto the
 *      remote end by the local signalling link management.  Level 3 has marked the link locally inhibited.
 *
 *  (2) Upon receipt of a request from a remote signalling point to inhibit a link and successful determinationthat
 *      no destination will become inaccessible by inhibiting the link, the link has been marked remotely inhibited
 *      by Level 3.
 *  
 *  3.2.9 Signalling Link Uninhibiting.  A signalling link previously inhibited is uninhibited when:
 *
 *  (1) A request is received to uninhibit the link from a remote end or from a local routing function.
 *
 *  (2) An acknowledgment is received from  a remote signalling point in response to an uninhibit requestsent to the
 *      remote end by the local signalling link management.
 *
 *  3.3 Procedures Used in Connection with Link Status Changes.  In 3.3, the procedures relating  toeach signalling
 *  management function, which are applied in connection with link status changes, are listed. (See also Figures
 *  6-8/T1.111.4.) Typical examples of the application of the procedures to the particularnetwork cases appear in
 *  Chapter T1.111.5.
 *
 *  3.3.1 Signalling Link Failed
 *
 *  3.3.1.1 Signalling Traffic Management.  The changeover procedure (see Section 5) is applied, ifrequired, to
 *  divert signalling traffic from the unavailable link to one or more alternative links with the objective of
 *  avoiding message loss, repetition or mis-sequencing; it includes determination of the alternativelink or links
 *  to which the affected traffic can be transferred, and procedures to retrieve messages sent over the failed link
 *  but not received by the far end.
 *
 *  3.3.1.2 Signalling Link Management.  The procedures described in Section 12 are used to restore asignalling link
 *  and to make it available for signalling.  Moreover, depending on the link set status, the procedures can also be
 *  used to activate another signalling link in the same link set to which the unavailablelink belongs, and to make
 *  it available for signalling.  
 * 
 *  3.3.1.3 Signalling Route Management.  In the case when the failure of a signalling link causes asignalling route
 *  set to become unavailable or restricted, the signalling transfer point that can no longer route the concerned
 *  signalling traffic applies the transfer-prohibited or transfer-restricted procedures describedin Section 13.  
 *
 *  3.3.2 Signalling Link Restored
 *
 *  3.3.2.1 Signalling Traffic Management.  The changeback procedure (see Section 6) is applied, ifrequired, to
 *  divert signalling traffic from one or more links to a link which has become available; it includes determination
 *  of the traffic to be diverted and procedures for maintaining the correct message sequence.
 *
 *  3.3.2.2 Signalling Link Management.  The signalling link deactivation procedure (see Section 12) isused if,
 *  during the signalling link failure, another signalling link of the same link set was activated; it is used to
 *  assure that the link set status is returned to the same state as before the failure.  This requires that
 *  theactive link activated during the link failure be deactivated and considered no longer available for
 *  signalling.  
 *
 *  3.3.2.3 Signalling Route Management.  In the case when the restoration of a signalling link causes a signalling
 *  route set to become available, the signalling transfer point which can once again route theconcerned signalling
 *  traffic applies the transfer-allowed or transfer-restricted procedures described in Section 13 as appropriate.
 *  When a "danger of congestion" situation subsides, which had restricted a normalroute to a destination, the
 *  status of the route is changed to available, but the transfer-allowed procedure (broadcast method) is not
 *  invoked (transfer-allowed messages are sent according to Section 13.5.4).  
 *
 *  3.3.3 Signalling Link Deactivated
 *
 *  3.3.3.1 Signalling Traffic Management.    As specified in 3.3.1.1.
 *
 *  NOTE: The signalling traffic has normally already been removed when signalling link deactivation is initiated.
 *
 *  3.3.3.2 Signalling Link Management.  If the number of active signalling links in the link set to whichthe
 *  deactivated signalling link belongs has become less than the normal number of active signalling links in that
 *  link set, the procedures described in Section 12 may be used to activate another signalling link in thelink set.  
 *
 *  3.3.3.3 Signalling Route Management.  As specified in 3.3.1.3.
 *
 *  3.3.4 Signalling Link Activated
 *
 *  3.3.4.1 Signalling Traffic Management. As specified in 3.3.2.1.
 *
 *  3.3.4.2 Signalling Link Management.  If the number of active signalling links in the link set to whichthe
 *  activated signalling link belongs has become greater than the normal number of active signalling links in that
 *  link set, the procedures described in Section 12 may be used to deactivate another signalling link in thelink
 *  set.
 *
 *  3.3.4.3 Signalling Route Management.  As specified in 3.3.2.3.
 *
 *  3.3.5 Signalling Link Blocked
 *
 *  3.3.5.1 Signalling Traffic Management.10  As specified in 3.3.1.1.
 *
 *  3.3.5.2 Signalling Route Management.  If the blocking of the link causes a signalling route set tobecome
 *  unavailable or restricted, the signalling transfer point which can no longer route the concerned signalling
 *  traffic applies the transfer-prohibited or transfer-restricted procedures described in Section 13.
 *
 *  3.3.6 Signalling Link Unblocked
 *
 *  3.3.6.1 Signalling Traffic Management.  As specified in 3.3.2.1.
 *
 *  3.3.6.2 Signalling Route Management.  If the link unblocked causes a signalling route set to becomeavailable,
 *  the signalling transfer point which can once again route the signalling traffic in that route set applies the
 *  transfer-allowed or transfer-restricted procedures described in Section 13, as appropriate.  10CCITT Blue Book
 *  text specifying an option to use local processor outage to stabilize the link state during traffic management
 *  actions has been deleted from this section.
 *
 *  3.3.7 Signalling Link Inhibite d
 *
 *  3.3.7.1 Signalling Traffic Management.  As specified in 3.3.1.1.
 *
 *  3.3.7.2 Signalling Link Management.  As specified in 3.3.3.2.
 *
 *  3.3.8 Signalling Link Uninhibited
 *
 *  3.3.8.1 Signalling Traffic Management.  As specified in 3.3.2.1.
 *
 *  3.3.8.2 Signalling Link Management.  As specified in 3.3.4.2.
 *
 *  3.3.8.3 Signalling Route Management.  If the link uninhibited causes a signalling route set to becomeavailable,
 *  the Signalling Transfer Point, which can once again route the signalling traffic in that route set, applies the
 *  transfer-allowed or transfer-restricted procedures described in Section 13, as appropriate. 3.4 Status of
 *  Signalling Routes.  A signalling route can be in three states for signalling traffic havingthe concerned
 *  destination; these are available, restricted, and unavailable.  A route may become available or unavailable
 *  because of changes in the availability of the local links in that route ; however, theprocedures already given
 *  in 3.3 handle such cases.  Subsections 3.4 and 3.5 deal with route status changes caused by receipt of
 *  signalling route management messages.
 */
STATIC int mtp_dec_msg(queue_t *q, mblk_t *mp, struct mtp_msg *m, struct na *na);

STATIC int
sl_stop_restore(queue_t *q, struct sl *sl)
{
	int err;
	switch (sl_get_l_state(sl)) {
	case SLS_WIND_INSI:
		if ((err = sl_stop_req(q, sl)))
			goto error;
		if ((err = sl_set_state(q, sl, SL_FAILED)) < 0)
			goto error;
		/* 
		   T19(ANSI) is a maintenance guard timer */
		sl_timer_stop(sl, t19a);
		/* 
		   wait for T17 before attempting restoration again */
		sl_timer_start(sl, t17);
		/* 
		   If we are waiting for in service then we do not change the routing state of the
		   link.  Also, we do not need to perform retrieval because we have not sent
		   anything on the link (other than SLTM). */
		return (QR_DONE);
	case SLS_WCON_RET:
	{
		struct lk *lk = sl->lk.lk;
		struct ls *ls = lk->ls.ls;
		struct sp *sp = ls->sp.sp;
		struct na *na = sp->na.na;
		struct cb *cb;
		/* 
		   We are already retrieving messages on the link */
		/* 
		   find all the changeover buffers */
		for (cb = lk->cb.list; cb; cb = cb->lk.next) {
			/* 
			   only consider changeover buffers that changeover from the failed link */
			if (cb->sl.from == sl) {
				mblk_t *bp, *bp_prev = bufq_tail(&sl->rbuf);
				cb_timer_stop(cb, tall);
				while ((bp = bp_prev)) {
					struct mtp_msg msg = { NULL, };
					bp_prev = bp->b_prev;
					if (mtp_dec_msg(q, bp, &msg, na) >= 0) {
						if (sls_index
						    (msg.sls, ls->lk.sls_mask,
						     ls->lk.sls_bits) != cb->index)
							continue;
						/* 
						   unlink from retrieval buffer and add to head of
						   changeover buffer */
						fixme(("See if we have the correct sls\n"));
						bp = bufq_unlink(&sl->rbuf, bp);
						bufq_queue_head(&cb->buf, bp);
						continue;
					}
					/* 
					   discard messages that can't be decoded */
					swerr();
					freemsg(bufq_unlink(&sl->rbuf, bp));
					continue;
				}
				/* 
				   release the changeback buffer */
				cb_reroute_buffer(cb);
			}
		}
	}
		/* 
		   purge any remaining unclaimed messages */
		bufq_purge(&sl->rbuf);
		if ((err = sl_set_state(q, sl, SL_UPDATED)) < 0)
			goto error;
		/* 
		   fall through */
	case SLS_PROC_OUTG:
	case SLS_IN_SERVICE:
		/* 
		   T31(ANSI) is a false congesiton detection timer */
		sl_timer_stop(sl, t31a);
		if (sl->state < SL_UNUSABLE && (sl->flags & SLF_TRAFFIC)) {
			if ((err = sl_stop_req(q, sl)))
				goto error;
			if ((err = sl_set_state(q, sl, SL_FAILED)) < 0)
				goto error;
			if (sl->flags & SLF_BLOCKED)
				if ((err = sl_set_state(q, sl, SL_UNBLOCKED)) < 0)
					goto error;
			if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
				goto error;
			if ((err = sl_retrieve_bsnt_req(q, sl)) < 0)
				goto error;
			sl->flags &= ~SLF_TRAFFIC;
			return (QR_DONE);
		}
	case SLS_WACK_SLTM:
		if ((err = sl_stop_req(q, sl)))
			goto error;
		if ((err = sl_set_state(q, sl, SL_FAILED)) < 0)
			goto error;
		if (sl->flags & SLF_BLOCKED) {
			if ((err = sl_set_state(q, sl, SL_UNBLOCKED)) < 0)
				goto error;
			/* 
			   flush old signalling link buffers */
			if ((err = sl_clear_buffers_req(NULL, sl)))
				goto error;
		}
		/* 
		   purge any remaining unclaimed messages */
		bufq_purge(&sl->rbuf);
		if ((err = sl_set_state(q, sl, SL_UPDATED)) < 0)
			goto error;
		if ((sl->lk.lk->sp.loc->flags & SPF_LOSC_PROC_A) && sl->timers.t32a) {
			/* 
			   ANSI link oscillation procedure A */
			/* 
			   link was in probation, now in suspension */
		} else if ((sl->lk.lk->sp.loc->flags & SPF_LOSC_PROC_B) && sl->timers.t33a) {
			/* 
			   ANSI link oscillation procedure B */
			/* 
			   link was in probation, now place it in suspension */
			sl_timer_stop(sl, t33a);
			sl_timer_start(sl, t34a);
		} else {
			/* 
			   start restoration */
			if ((err = sl_start_req(NULL, sl)))
				goto error;
			/* 
			   T19(ANSI) is a maintenance guard timer */
			sl_timer_start(sl, t19a);
		}
		sl->flags &= ~(SLF_WACK_SLTM | SLF_WACK_SLTM2);
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
      error:
	rare();
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timeout functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  TIMER T1  -  changeover
 *  -----------------------------------
 *  IMPLEMENTATION NOTE:-  Similar analysis as for T2 timer: we discard the buffer and restart traffic.
 */
STATIC int
cb_t1_timeout(struct cb *cb)
{
	struct sl *sl = cb->sl.from;
	/* 
	   complete the changeover but discard buffer contents first */
	bufq_purge(&cb->buf);
	cb_reroute_buffer(cb);
	/* 
	   Also purge and reroute any other buffers pending on t1 for the same link */
	for (cb = sl->lk.lk->cb.list; cb; cb = cb->lk.next) {
		if (cb->sl.from == sl && cb->timers.t1) {
			cb_timer_stop(cb, t1);
			bufq_purge(&cb->buf);
			cb_reroute_buffer(cb);
		}
	}
	return sl_stop_restore(NULL, sl);
}

/*
 *  TIMER T2  -  changeover ack
 *  -----------------------------------
 *  5.7.2  If no changeover message in response to a changeover order is received within a timer T2 (see 16.8), new
 *  traffic is started on the alternative signalling link(s).
 *
 *  IMPLEMENTATION NOTE: There is a problem here.  We cannot check the link for congestion when trasferring the
 *  buffer.  If we a transferring a very large changeover buffer (up to 2 seconds worth of messages at 1 Erlang) we
 *  could congest the new link leading to a cascading failure.  Section 5.7.2 above, does not say to transfer the
 *  buffer on T2 expiry.  Instead it says to start "new traffic" on the alternative signalling link(s).  Taking this
 *  literally, we will flush the buffer and start new traffic only.
 */
STATIC int
cb_t2_timeout(struct cb *cb)
{
	struct sl *sl = cb->sl.from;
	/* 
	   complete the changeover but discard buffer contents first */
	bufq_purge(&cb->buf);
	cb_reroute_buffer(cb);
	/* 
	   Also purge and reroute any other buffers pending on t2 for the same link */
	for (cb = sl->lk.lk->cb.list; cb; cb = cb->lk.next) {
		if (cb->sl.from == sl && cb->timers.t2) {
			cb_timer_stop(cb, t2);
			bufq_purge(&cb->buf);
			cb_reroute_buffer(cb);
		}
	}
	return sl_stop_restore(NULL, sl);
}

/*
 *  TIMER T3  -  changeback
 *  -----------------------------------
 *  6.2.5  If the signalling point at the far end of the link made available currently is inaccessible, from the
 *  siganlling point initiating changeback (see clause 9 on MTP Restart), the sequence control procedure specified
 *  in 6.3 (which requires communication betwen the two concerned signalling points) does not apply; instead, the
 *  time-controlled diversion specified in 6.4 is performed.  This is made also when the concerned signalling points
 *  are accessible, but there is no signalling route to it using the same outgoing signalling link(s) (or one of the
 *  same signalling links) from which traffic will be diverted.
 *
 *  6.4.1  The timer-controlled diversion procedure is sued at the end of the MTP restart procedure (see clause 9)
 *  when an adjacent signalling point becomes available, as well as for the reasons given in 6.2.5. ...
 *
 *  6.4.2  When changeback is intiated after the MTP restart procedure, the adjacent signalling point of the point
 *  whose MTP is restarting stops traffic to be directed from the alternative signalling link(s) for a time T3,
 *  after which it starts traffic on the signalling link(s) made available.  The time delay minimizes the
 *  probability of out-of-sequence delivery to the destination point(s).
 */
STATIC int
cb_t3_timeout(struct cb *cb)
{
	/* 
	   restart traffic */
	return cb_reroute_buffer(cb);
}

/*
 *  TIMER T4  -  changeback ack
 *  -----------------------------------
 *  6.5.3   If no changeback-acknowledgement is received in response to a changeback declaration within a timer T4,
 *  the changeback declaration is repeanted and a new timer T5 is started.  If no changeback acknowledgement is
 *  received before the expiry of T5, the maintenance functions are alerted and traffic on the link made available
 *  is started.  The changeback code contained in the changeback-acknowledgement message makes it possible to
 *  determine, in the case of parallel changebacks, from more than one reserve path, which changeback-declaration is
 *  unacknowledged and has therefore to be repeated.
 */
STATIC int
cb_t4_timeout(struct cb *cb)
{
	struct sl *sl_from = cb->sl.from;
	struct sl *sl_onto = cb->sl.onto;
	struct lk *lk = sl_onto->lk.lk;
	struct sp *sp = lk->sp.loc;
	struct rs *rs = lk->sp.adj;
	mtp_send_cbd(NULL, sp, lk->ni, rs->dest, sp->pc, sl_onto->slc, sl_onto->slc, cb->cbc,
		     sl_from);
	cb_timer_start(cb, t5);
	return (QR_DONE);
}

/*
 *  TIMER T5  -  changeback ack second attempt
 *  -----------------------------------
 */
STATIC int
cb_t5_timeout(struct cb *cb)
{
	/* 
	   restart traffic */
	return cb_reroute_buffer(cb);
}

/*
 *  TIMER T6  -  controlled rerouting
 *  -----------------------------------
 *  Controlled rerouting is performed with a special controlled rerouting buffer which is attached to both the route
 *  from which traffic is being rerouted and the route to which traffic is being rerouted.  If the routeset has
 *  become unavailable while the timer was running, this will result in purging the controlled rerouting buffer.  If
 *  the unavailability was transient and the routeset is now available again, these messages will not be lost.  This
 *  is not mentioned in the MTP specifications.
 */
STATIC int
cr_t6_timeout(struct cr *cr)
{
	/* 
	   restart traffic */
	return cr_reroute_buffer(cr);
}

/*
 *  TIMER T7  -  waiting for SDLC ack
 *  -----------------------------------
 */
STATIC int
lk_t7_timeout(struct lk *lk)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T8  -  transfer prohibited inhibitition timer
 *  -----------------------------------
 */
STATIC int
rs_t8_timeout(struct rs *rs)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T9 - (not used)
 *  -----------------------------------
 */

/*
 *  TIMER T10  -  waiting to repeat routeset test message
 *  -----------------------------------
 *  This timer is started after receiving a TFP, TCP, TFR or TCR for the route.  It runs and sends a RST message on
 *  the route every T10 seconds until stopped.  This timer is stopped after receiving a TFA or TCA message for the
 *  route.
 *
 *  13.5.2  A signalling route set test message is sent from a signalling point after a transfer prohibited or
 *  transfer restricted message is received from an adjacent signalling transfer point (see 13.2.4 and 13.4.4).  In
 *  this case, a signalling route set test message is sent to that signalling transfer point referring to the
 *  destination declared inaccessible or restricted by the transfer prohibited or transfer restricted message, every
 *  T10 period (see clause 16) until a transfer-allowed message, indicating that the destination has become
 *  accessible, is received.
 *
 *  Start:  receive TFP, TCP, TFR, TCR for first time
 *  Stop:   receive TFA, TCA
 *  Expiry: send RST, RCP, RSR, RCR and start T10
 */
STATIC int
rt_t10_timeout(struct rt *rt)
{
	int err;
	struct lk *lk = rt->lk.lk;
	struct rs *rs = rt->rl.rl->rs.rs;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	switch (rt->state) {
	case RT_PROHIBITED:
		if (rt->type != RT_TYPE_CLUSTER) {
			if ((err =
			     mtp_send_rst(NULL, loc, lk->ni, adj->dest, loc->pc, 0, rs->dest)) < 0)
				goto error;
		} else {
			if ((err =
			     mtp_send_rcp(NULL, loc, lk->ni, adj->dest, loc->pc, 0, rs->dest)) < 0)
				goto error;
		}
		break;
	case RT_RESTRICTED:
		if (rt->type != RT_TYPE_CLUSTER) {
			if ((err =
			     mtp_send_rsr(NULL, loc, lk->ni, adj->dest, loc->pc, 0, rs->dest)) < 0)
				goto error;
		} else {
			if ((err =
			     mtp_send_rcr(NULL, loc, lk->ni, adj->dest, loc->pc, 0, rs->dest)) < 0)
				goto error;
		}
		break;
	default:
	case RT_ALLOWED:
		return (QR_DONE);
	}
	rt_timer_start(rt, t10);
	return (QR_DONE);
      error:
	rare();
	return (err);
}

/*
 *  TIMER T11  -  transfer restricted timer
 *  -----------------------------------
 *  ANSI T1.111.4 (1996-2000)  13.4.2 ... (1) When the normal route(s) to destination X become unavailable and
 *  signalling transfer point Y begins using a lower priority route to destination X, timer T11 is started.  When
 *  timer T11 expires or the lower priority route expreriences danger of congestion (whichever occurs first),
 *  signalling transfer point Y notifies accessible adjacent signalling points using one of two possible methods:
 *
 *  (a) Broadcast Method
 *
 *  Signalling transfer point Y sends transfer-restricted messages concerning destination X to all accessible
 *  adjacent signalling points, except those signalling points that receive a transfer-prohibited message according
 *  to 13.2.2 (1) and except destination X if it is an adjacent point.  Timer T18 is started after the broadcast is
 *  complete.  If the failure is still present and T18 expires, a transfer-restricted message is sent once per
 *  incoming link or link set by signalling transfer point Y in response to a message for destination X.
 *
 *  As a network option, the rate that transfer-restricted messages are broadcast may be regulated to minimize the
 *  burst of traffic that can occur due to controlled rerouting in adjacent nodes.  This regulation is not performed
 *  for the broadcasting of transfer-restricted messages indicating that a previously isolated destination is
 *  accessible.
 *
 */
STATIC int
rs_t11_timeout(struct rs *rs)
{
	int err;
	struct sp *sp = rs->sp.sp;
	struct na *na = sp->na.na;
	switch (rs->state) {
	case RS_RESTRICTED:
	case RS_CONGESTED:
	case RS_DANGER:
		switch (na->option.popt & (SS7_POPT_TFR | SS7_POPT_TFRB | SS7_POPT_TFRR)) {
		case 0:
			break;
		case SS7_POPT_TFR:
			/* 
			   old broadcast method - no responsive */
			if ((err = mtp_tfr_broadcast(NULL, rs)))
				return (err);
			break;
		case SS7_POPT_TFRB:
		case SS7_POPT_TFR | SS7_POPT_TFRB:
			/* 
			   We don't acrtually regulate broadcast of TFR per ANSI T1.111.4-2000
			   13.4.2 (1)(a).  If the operator wishes to regular the rate of TFRs
			   initially sent, the Responsive method should be used (i.e. with
			   SS7_POPT_TFRR */
			if ((err = mtp_tfr_broadcast(NULL, rs)))
				return (err);
			if (sp->flags & SPF_XFER_FUNC)
				/* 
				   prepare for final TFR response */
				rs_timer_start(rs, t18a);
			break;
		case SS7_POPT_TFRR:
		case SS7_POPT_TFRR | SS7_POPT_TFRB:
		case SS7_POPT_TFR | SS7_POPT_TFRR:
		case SS7_POPT_TFR | SS7_POPT_TFRR | SS7_POPT_TFRB:
			/* 
			   If both methods are specified, we will go with Responsive method because 
			   it is a little more stable than non-regulated broadcasts */
			if (sp->flags & SPF_XFER_FUNC) {
				/* 
				   prepare for initial TFR response */
				struct ls *ls;
				struct lk *lk;
				struct rr *rr;
				/* 
				   What we have to do here is establish a structure linking every
				   incoming linkset structure to which we offer a route to this
				   routeset structure. Then if a message arrives for the routeset
				   on a linkset matching one of these structures, we would send
				   repsonsive TFR and then delete the structure. */
				/* 
				   establish new route restriction list */
				for (ls = sp->ls.list; ls; ls = ls->sp.next)
					for (lk = ls->lk.list; lk; lk = lk->ls.next)
						if (!(rr = mtp_alloc_rr(rs, lk)))
							return (-ENOMEM);
				/* 
				   we have to wait for the first TFR to start timer T18a */
				rs->flags |= RSF_TFR_PENDING;
				return (QR_DONE);
			}
			break;
		}
		return (QR_DONE);
	}
	rare();
	ptrace(("Stale timeout for timer t11\n"));
	return (QR_DONE);
}

/*
 *  TIMER T18(ANSI)  -  awaiting TFR final repsonse
 *  -----------------------------------
 */
STATIC int
rs_t18a_timeout(struct rs *rs)
{
	struct rr *rr, *rr_next = rs->rr.list;
	/* 
	   purge any existing route restriction list */
	while ((rr = rr_next)) {
		rr_next = rr->rs.next;
		mtp_free_rr(rr);
	}
	switch (rs->state) {
	case RS_RESTRICTED:
	case RS_CONGESTED:
	case RS_DANGER:
	{
		struct ls *ls;
		struct lk *lk;
		/* 
		   establish a new route restriction list */
		for (ls = rs->sp.sp->ls.list; ls; ls = ls->sp.next)
			for (lk = ls->lk.list; lk; lk = lk->ls.next)
				if (!(rr = mtp_alloc_rr(rs, lk)))
					return (-ENOMEM);
		return (QR_DONE);
	}
	}
	rare();
	ptrace(("Stale timeout for timer T18a\n"));
	return (QR_DONE);
}

/*
 *  TIMER T12  -  uninhibit ack
 *  -----------------------------------
 */
STATIC int
sl_t12_timeout(struct sl *sl)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T13  -  waiting for force uninhibit
 *  -----------------------------------
 */
STATIC int
sl_t13_timeout(struct sl *sl)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T14  -  waiting for inhibition ack
 *  -----------------------------------
 */
STATIC int
sl_t14_timeout(struct sl *sl)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T15  -  waiting to start routeset congestion test
 *  -------------------------------------------------------------------------
 *  13.7.5  If T15 expires after the last update of the signalling route set towards destination X by a
 *  transfer-controlled message relating to the same destination, the signalling route set congestion test rocedure
 *  is invoked (see 13.9).
 *
 *  13.9.4  Provided that the signalling route set towards destination X is not in the "unavailable" state, a
 *  signalling route set congestion test message is sent from an originating signalling point to destination X in
 *  the following cases: i) When T15 expires after the last update of the congestion status of the signalling route
 *  set toward destination X by a transfer controlled message relating to the same destination.
 *
 *  Start:  TFC received
 *  Stop:   --
 *  Expiry: Send RCT, start T16
 */
STATIC int
rs_t15_timeout(struct rs *rs)
{
	int err;
	if (rs->cong_status) {
		struct sp *sp = rs->sp.sp;
		rs_timer_start(rs, t16);
		if ((err =
		     mtp_send_rct(NULL, sp, sp->ni, rs->dest, sp->pc, 0, rs->cong_status)) < 0)
			goto error;
	}
	return (QR_DONE);
      error:
	return (err);
}

/*
 *  TIMER T16  -  waiting for routeset congestion status update
 *  -------------------------------------------------------------------------
 *  13.9.3  If T16 expires after sending a signalling route set congestion test message without a transfer
 *  controlled message relating to the concerned destination having been received, the signalling point changes the
 *  congestion status associated with the signalling route set towards the concerned destination to the next lower
 *  status.
 *
 *  13.9.4  Provided that the signalling route set towards destination X is not in the "unavailable" state, a
 *  signalling route set congestion test message is sent from an originating signalling point to destination X in
 *  the following cases: i) ... ii) When T16 expires after sending a signalling route set congestion test message to
 *  destination X without a transfer controlled message relating to the same destination having been received.
 *  After the congestion status have been decremented by one, the test is repeated, unless the congestion status is
 *  zero.
 *
 *  Start:  T15 expires
 *  Stop:   TFC received
 *  Expiry: Decrement cong status, inform users, send RCT and restart if still
 *          congested
 */
STATIC int
rs_t16_timeout(struct rs *rs)
{
	int err;
	if (rs->cong_status) {
		struct sp *sp = rs->sp.sp;
		uint newstatus;
		if ((newstatus = rs->cong_status - 1)) {
			rs_timer_start(rs, t16);
			if ((err =
			     mtp_send_rct(NULL, sp, sp->ni, rs->dest, sp->pc, 0, newstatus)) < 0)
				goto error;
		}
		if ((err = mtp_cong_status_ind_all_local(NULL, rs, newstatus)) < 0)
			goto error;
		rs->cong_status = newstatus;
	}
	return (QR_DONE);
      error:
	return (err);
}

/*
 *  TIMER T17  -  link restart
 *  -----------------------------------
 *  12.2.1.2    When a decision is taken to activate an inactive signalling link; initial alignment starts.  If the
 *  initial alignment procedure is successful, the signalling link is active and a signalling link test is started.
 *  If the signalling link test is successful the link becomes ready to convey signalling traffic.  In the case when
 *  initial alignment is not possible, as determined at Message Transfer Part level 2 (see clause 7/Q.703), new
 *  initial alignment procedures are started on the same signalling link after a timer T17 (delay to avoid the
 *  oscillation of initial alignment failure and link restart.  The value of T17 should be greater than the loop
 *  delay and less than timer T2, see 7.3/Q.703).  If the signalling link test fails, link restoration starts until
 *  the signalling link is activated or a manual intervention is made.
 *
 *  12.2.2  Signalling link restoration
 *
 *  After a signalling link failure is detected, signalling link intial alignment will take place.  In the case when
 *  the initial alignment procedure is successful, a signalling link test is started.  If the signalling link test
 *  is successful the link becomes restored and thus available for signalling.
 *
 *  If the initial alignmeent is not possible, as determined at Message Transfer Part level 2 (see clause 7/Q.703),
 *  new initial alignment procedures may be started on the same signalling link after a time T17 until the
 *  signalling link is restored or a manual intervention is made, e.g. to replace the signalling data link or the
 *  signalling terminal.
 *
 *  If the signalling link test fails, the restoration procedure is repeated until the link is restored or a manual
 *  intervention is made.
 */
STATIC int
sl_t17_timeout(struct sl *sl)
{
	/* 
	   simply attempt to restart the link */
	/* 
	   T19(ANSI) is a maintenance guard timer */
	sl_timer_start(sl, t19a);
	return sl_start_req(NULL, sl);
}

/*
 *  TIMER T1R  -  MTP restart timer
 *  -----------------------------------
 *  This is the same as timer T1 for signalling links, but is used to determine the amount of time from the last
 *  failed signalling link until restart is required.
 */
STATIC int
sp_t1r_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T18  -  MTP restart timer
 *  -----------------------------------
 *  Timer T18 is a phase 1 restart timer and is started by a restarting signalling point with the transfer function
 *  when the first link goes into service at level 2.
 *
 *  9.2.2 ... Timer T18 is implementation and network dependent, and is stopped when:
 *
 *  1)  sufficient links and link sets are available to carry the expected signalling traffic; and
 *
 *  2)  enough TRA messages (and therefore routing data) have been received to give a high level of confidence in
 *  the MTP routing tables.
 *
 *  NOTE -- In normal circumstances the restarting MTP should wait for TRA messgaes from all adjacent nodes.  There
 *  are, however, other situations where this might not be useful, e.g. for a long-term equipment failure.
 *
 *  When T18 is stopped or expires, these second phase begins, which includes as a major part a broadcast of
 *  non-preventitive transfer prohibited messages [i.e. those TFPs according to 13.2.2 v)] and transfer-restricted
 *  messages, taking into account signalling link sets which are not available and any TFP, TFA and TFR messages
 *  received during phase 1.  Note that timer T18 is determined such that during phase 2 the broadcast of TFP and
 *  TFR messages may be completed in normal situations.
 */
STATIC int
sp_t18_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T19  -  MTP restart timer
 *  -----------------------------------
 *  9.2.4 ... In addition, [to sending TRA] timer T19 is started (see 9.5.2) for all signalling points to which a
 *  TRA message has just been sent.  Normal operation is then resumed.
 *
 *  9.5.1  If a signalling point X receives an unexpected TRA message from an adjacent node Y and no associated T19
 *  timer is running, X sends to Y any necessary TFP and TFR messages if X has the transfer function, and a TRA
 *  message to Y.  In addition, X starts timer T19 associated with Y.
 *
 *  9.5.2  If a signalling point receives a TRA message from an adjacent node and an associated T19 is running, this
 *  TRA is discarded and no further action is taken.
 */
STATIC int
sp_t19_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T19(ANSI)  -  Link Maintenance timer
 *  -----------------------------------
 *  ANSI T1.111.4 (1996 and 2000) 12.2.2.  After a signalling link failure is detected, signalling link initial
 *  alignment will take place.  When the initial alignment procedure is successful, a signalling link test is
 *  started.  If the signalling link test is successful, the signalling link is regarded as restored and thus
 *  available for siganlling.
 *
 *  If initial alignment is not possible, as determined at level 2 (see Section 7 of Chapter T1.111.3 or Section
 *  6.2.2 of T1.645), new initial alignment procedures may be started on the same signalling link after a timer T17
 *  until the signalling link is restored or a manual intervention is made, e.g., to replace the signalling data
 *  link or the signalling terminal.  If after time T19 the signalling link has not activated, a management function
 *  is notified, and, optionally, T19 may be restarted.
 */
STATIC int
sl_t19a_timeout(struct sl *sl)
{
	switch (sl_get_l_state(sl)) {
	case SLS_WIND_INSI:
		todo(("Management notification\n"));
		sl_timer_start(sl, t19a);
		/* 
		   kick it again */
		return sl_start_req(NULL, sl);
	}
	rare();
	return (-EFAULT);
}

/*
 *  TIMER T20  -  MTP restart timer
 *  -----------------------------------
 *  Timer T20 is an overall restart timer that is started by a restarting signalling point when the first link goes
 *  into service at level 2.
 *
 *  9.2.2 ... When all TFP and TFR messages have been sent, the overall restart timer T20 is stopped and phase 2 is
 *  finished.  Note that preventative TFP messages [i.e. those according to 13.2.2 i)], except possibly those for
 *  highest priority routes, must have been sent before normal User traffic is carried.  This might be done during
 *  or after phase 2.
 *
 *  9.2.3 If the restarting MTP has no transfer function, phase 1 (see 9.2.2) but not phase 2 is present.  In this
 *  case, the whole restart time is available for phase 1.  The overall restart timer T20 is stopped when:
 *
 *  1)  sufficient links and link sets are available to carry the expected signalling traffic; and
 *
 *  2)  enough TRA messages (and therefore routing data) have been received to give a high level of confidence in
 *  the MTP routing tables.
 *
 *  9.2.4  When T20 is stopped or expires, the restaring MTP of the signalling point or signalling transfer point
 *  sends traffic restart allowed messages to all adjacent signalling points via corresponding available direct link
 *  sets, and an indication of the end of MTP restart is sent to all local MTP Users showing each signalling point's
 *  accessibility or inaccessibility.  The means of doing the latter is implementation dependent.
 *
 *  In addition, [to sending TRA] timer T19 is started (see 9.5.2) for all signalling points to which a TRA message
 *  has just been sent.  Normal operation is then resumed.
 *
 *  When T20 expires the transmission of TFP and TFR messages is stopped.  However, preventative TFP messages [i.e.
 *  those according to 13.2.2 i)] except possibly those for highest priority routes, must have been sent before MTP
 *  User traffic is restarted.
 *
 */
STATIC int
sp_t20_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T20(ANSI)
 *  -----------------------------------
 */
STATIC int
sl_t20a_timeout(struct sl *sl)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T21(ANSI)
 *  -----------------------------------
 *  ANSI T1.111.4-2000  10.2.  Inihibiting Initiation and Actions.  When at signalling point X, a request is
 *  received from a management function to inhibit a signalling link to signalling point Y, the following actions
 *  take place:
 *
 *  (1) A check is performed at signalling point X to determine whether, in the case of an unavailable link,
 *  inhibiting will result in a destination becoming inaccessible, or in the case of an unavailable link, signalling
 *  point Y is inaccessible.  If either is the case, management is informed that the inihibiting request is denied.
 *
 *  (2) If inhibiting is permitted, signalling point X sends an inhibit message to signaling point Y, indicating
 *  that it wishes to inhibit the signalling link identified in the message.
 *
 *  (3) Signalling point Y, on receiving the inhibit message from X, checks whether, in the case of an available
 *  link, inhibiting will result in a destination becomming inaccessible; and, if so, an inhibit denied message is
 *  returned to signalling point X.  The latter then informs the management function which requested inhibiting that
 *  the request cannot be granted.
 *
 *  (4) If signalling point Y finds the inhibiting of the concerned link is permissible, it sends an inhibit
 *  acknowledgement to signalling point X, marks the link remotely inhibited, and starts inhibit test procedure
 *  timer T21. [10]  If the link concerned is currently carrying traffic, signalling point Y sends the inhibit
 *  acknowledgement via that link and diverts subsequent traffic for it, using the time controlled changeover
 *  procedure.
 *
 *  (5) On receiving an inhibit acknowledgement message, signalling point X marks the link locally inhibited,
 *  informs management that the link is inhibited, and starts inhibit test procedure timer T20. [10]  If the link
 *  concerned is currently carrying traffic, signalling point X diverts subsequent traffic for the link, using the
 *  time controlled changeover procedure.
 *
 *  (6) When changeover has been completed, the link while inhibited, is unavailable for the transfer of
 *  user-generated traffic but still permits the exchange of test messages.
 *
 *  (7) If, for any reason, the inhibit acknowledgement message is not received, a timer T14 [10] expires and the
 *  procedure is restarted including inspection of the status of the destination of the inhibit message.  If the
 *  destination is not available, management is informed.
 *
 *  At most two consecutive automatic attempts may be made to inhibit a particular link.
 *
 *  ...
 *
 *  10.3A  Inhibit Test Procedure Actions.  When a signalling link becomes inhibited, either locally, remotely, or
 *  both, the following test procedure actions take place, as appropriate:
 *
 *  (1) If timer T20 expires at signalling point X and the signalling link is locally inhibited at signalling point
 *  X, signalling point X transmits a local inhibit test message to the signalling point at the other end of the
 *  locally inhibited link and restarts timer T20.
 *
 *  (2) If timer T20 expires at signalling point X and the signalling link is no longer locally inhibited, no action
 *  is taken.
 *
 *  (3) If timer T21 expires at signalling point X and the signalling link is remotely inhibited at signalling point
 *  X, singalling
 *
 */
STATIC int
sl_t21a_timeout(struct sl *sl)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T21  -  MTP restart timer
 *  -----------------------------------
 *  9.3.1  A signalling point X considers that MTP of an inaccessible adjacent signalling point Y is restarting
 *  when:
 *
 *  -   the first link in a direct link set is in the "in service" state at level 2; or
 *
 *  -   another route becomes available due either to reception of a corresponding TFA, TFR or TRA message, or by
 *  the corresponding link set becoming available (see 3.6.2.2).
 *
 *  9.3.2  When the first link in a direct linkset towards singlling point Y, whose MTP is restarting, is in the "in
 *  service" state at level 2, signalling point X starts a timer T21 and takes account of any TFP, TFA and TFR
 *  messages received from signalling point Y.  In addition, X takes the following actions:
 *
 *  -   if X has the transfer function, when the direct linkset is available at level 3, X sends any necessary TFP
 *  and TFR message to Y; then
 *
 *  -   X sends a traffic restart allowed mesage to signalling point Y.
 *
 *  If a signalling point, previously declared to be inaccessible, becomes available again before T21 is stopped or
 *  expires, a corresponding TFA or TFR message is sent to the signalling point Y whose MTP is restarting.
 *
 *  If a signalling point becomes prohibited or restricted to signalling X after a TRA message has been sent by X to
 *  Y, X sends a corresponding TFP or TFR message to Y.
 *
 *  When a traffic restart allowed message has been received by X from signalling point Y, and a TRA message has
 *  been sent by X to Y, X stops timer T21.
 *
 *  ...
 *
 *  When T21 is stopped or expires, signalling point X sends a MTP-RESUME primitive concerning Y, and all signalling
 *  points made available via Y, to all local MTP Users.  If X has the transfer function, it broadcasts to adjacent
 *  available signalling points transfer-allowed and/or transfer restricted messages concerning Y and all signalling
 *  points made accessible by Y.
 *
 *  9.6.2  After the MTP of an adjacent node X has restarted, and if T21 has been started (see 9.3.2), all routes
 *  using X are considered to be available unless corresponding TFP or TFR message have been received whilst T21 was
 *  running.
 */
STATIC int
sp_t21_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T22  -  local inhibit test timer
 *  -----------------------------------
 */
STATIC int
sl_t22_timeout(struct sl *sl)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T22(ANSI)
 *  -----------------------------------
 */
STATIC int
sp_t22a_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T23  -  remote inhibit test timer
 *  -----------------------------------
 */
STATIC int
sl_t23_timeout(struct sl *sl)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T23(ANSI)
 *  -----------------------------------
 */
STATIC int
sp_t23a_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T24  -  LPO timer
 *  -----------------------------------
 */
STATIC int
sl_t24_timeout(struct sl *sl)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T24(ANSI)
 *  -----------------------------------
 */
STATIC int
sp_t24a_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T25(ANSI)
 *  -----------------------------------
 */
STATIC int
sp_t25a_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T26(ANSI)
 *  -----------------------------------
 */
STATIC int
sp_t26a_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T27(ANSI)
 *  -----------------------------------
 */
STATIC int
sp_t27a_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T28(ANSI)
 *  -----------------------------------
 */
STATIC int
sp_t28a_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T29(ANSI)
 *  -----------------------------------
 */
STATIC int
sp_t29a_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T30(ANSI)
 *  -----------------------------------
 */
STATIC int
sp_t30a_timeout(struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 *  TIMER T31 - False link congestion detection timer
 *  -----------------------------------
 *  ANSI T1.111.4 (2000) 3.8.2.2.  ...  If the link remains at a congestion level, as defined below, for greater
 *  than T31 [10] (a false link congestion condition), and it also remains in service, an audit should trigger the
 *  link to be restarted.  T31 is started when buffer occupancy increases through onset level 1.  Any change of
 *  congestion level [defined as a change ofsignalling link congestion status (see Section 3.8.2.1) and optionally
 *  including the first time the discard status is increased to n after signalling link congestion status is
 *  increased to  n (n=1, 2, 3)] restarts the timer.  T31 is stopped when the signalling link congestion status is
 *  zero.  The management should be notified if the link is restarted due to false link congestion.
 */
STATIC int
sl_t31a_timeout(struct sl *sl)
{
	if (sl_get_l_state(sl) == SLS_IN_SERVICE) {
		todo(("Notify management of false congestion\n"));
		return sl_stop_restore(NULL, sl);
	}
	rare();
	return (-EFAULT);
}

/*
 *  TIMER T32  -  Link oscillation timer (Procedure A)
 *  -----------------------------------
 *  ANSI T1.111.4 (1996 and 2000) 12.2.2 ... In order to [sic] prevent links from oscillating rapidly between the
 *  in-service and out-of-service states, a link oscillation filtering procedure shall be provdided.  This procedure
 *  is applicable in all cases where automatic restoration and removal of signalling links to and from service is
 *  used.  The procedure is not invoked if restoration to or removal from service is initiated manually.
 *
 *  Link oscillation filtering is a local procedure.  One of the following procedures, A or B, is used.
 *
 *  (A) When restoration begins, the oscillation timer T32 is started.  If the link fails within T32, the link is
 *  placed in the "suspension" state until T32 expires.
 *
 *  When a link exits in the "suspension" state, link restoration is to be started and T32 is restarted.
 *
 *  When a link is in "suspension," the link remains in the out-of-service state at level 2, restoration procedures
 *  to bring the link back into alignment are not invoked and realignment attempts initiated by the remote end are
 *  ignored.
 *
 *  The oscillation timer T32 value chosen in a given signalling point may be modified by management action.  The
 *  expiration of the timer does not change the link status.
 *
 */
STATIC int
sl_t32a_timeout(struct sl *sl)
{
	switch (sl_get_l_state(sl)) {
	case SLS_OUT_OF_SERVICE:
		/* 
		   we can now restore the link */
		/* 
		   T19(ANSI) is a maintenance guard timer */
		sl_timer_start(sl, t19a);
		return sl_start_req(NULL, sl);
	case SLS_IN_SERVICE:
		/* 
		   nothing to do */
		return (QR_DONE);
	}
	rare();
	return (-EFAULT);
}

/*
 *  TIMER T33  -  Link oscillation probation timer (Procedure B)
 *  -----------------------------------
 *  ANSI T1.111.4 (1996 and 2000) 12.2.2 ...
 *
 *  (B) When activation or restoration of a signalling link has been completed, it is put on probation for a period
 *  controlled by timer T33 which starts running when the link enters the in-service state and is stopped either on
 *  time-out or on link failure.
 *
 *  If a link fails during the probation, i.e. before expiration of timer T33, it is put on suspension.  While
 *  suspended the link remains in the out-of-service state at level 2, restoration procedures to bring the link back
 *  into alignment are not invoked and realignment attempts initiated by the remote end are ignored.
 *
 *  The suspension time period is controlled by timer T34 which starts running when the link enters the
 *  out-of-service state.  Link restoration is resumed, on expiry of timer T34.
 *
 *  The values chosen in a given network node for the probation and suspesion time periods are dependent on the type
 *  and function of the node in the network and may be modified by management action.
 */
STATIC int
sl_t33a_timeout(struct sl *sl)
{
	if (sl_get_l_state(sl) == SLS_IN_SERVICE)
		/* 
		   link has passed probation */
		return (QR_DONE);
	rare();
	return (-EFAULT);
}

/*
 *  TIMER T34  -  Link oscillation suspension timer (Procedure B)
 *  -----------------------------------
 */
STATIC int
sl_t34a_timeout(struct sl *sl)
{
	switch (sl_get_l_state(sl)) {
	case SLS_OUT_OF_SERVICE:
		/* 
		   link has exited suspension */
		/* 
		   T19(ANSI) is a maintenance guard timer */
		sl_timer_start(sl, t19a);
		return sl_start_req(NULL, sl);
	}
	rare();
	return (-EFAULT);
}

/*
 *  TIMER T1T  -  SLTM ack
 *  -----------------------------------
 *  Q.707 2.2 ...  In the case when the criteria given above are not met or a signalling link test acknowledgement
 *  message is not received on the link being tested within T1 (see 5.5) after the signalling link test message has
 *  been sent, the test is considered to have failed and is repeated once.  In the case when also the repeat test
 *  fails, the following actions have to be taken:
 *
 *  -   SLT applied on activation/restoration, the link is put out of service, restoration is attempted and a
 *      management system must be informed.
 *
 *  -   SLT applied periodically, for further study.
 *
 *  IMPLEMENTATION NOTE:- The action when the periodic test fails is for further study.  Taking the link out of
 *  service can be an especially bad idea during times of congestion. This is because the buffers may be filled with
 *  management messages already and the signaling link test might fail due to timeout. We merely report the
 *  information to management and leave the link in service. If the test failed because of an inserted loopback
 *  while the link is in service, the link will fail rapidly due to other causes. So we do nothing more than report
 *  to management.
 */
STATIC int
sl_t1t_timeout(struct sl *sl)
{
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;
	struct rs *rs = lk->sp.adj;
	int err;
	if (sl->flags & SLF_WACK_SLTM) {
		/* 
		   first test failed -- repeat test */
		if ((err =
		     mtp_send_sltm(NULL, sp, lk->ni, rs->dest, sp->pc, sl->slc, sl->slc, sl->tdata,
				   sl->tlen, sl)) < 0)
			return (err);
		sl->flags &= ~SLF_WACK_SLTM;
		sl_timer_start(sl, t1t);
		sl->flags |= SLF_WACK_SLTM2;
	} else if (sl->flags & SLF_WACK_SLTM2) {
		/* 
		   second test failed */
		if (sl_get_l_state(sl) == SLS_WACK_SLTM) {
			/* 
			   initial testing failed, fail link */
			if ((err = sl_stop_restore(NULL, sl)) < 0)
				return (err);
		} else {
			/* 
			   periodic testing failed, just report to management */
			todo(("Report periodic test failure to management\n"));
			sl_timer_start(sl, t2t);	/* restart periodic test */
		}
		sl->flags &= ~SLF_WACK_SLTM2;
	}
	return (QR_DONE);
}

/*
 *  TIMER T2T  -  SLTM interval
 *  -----------------------------------
 *  Q.707 2.2 ... In the case the signalling link test (SLT) is applied while the signalling link is in service the
 *  signalling link test mesage is sent at regular intervals T2 (see 5.5).  The testing of a signalling link is
 *  performed independently from each end.
 */
STATIC int
sl_t2t_timeout(struct sl *sl)
{
	if (sl_get_l_state(sl) == SLS_IN_SERVICE) {
		struct lk *lk = sl->lk.lk;
		struct sp *loc = lk->sp.loc;
		struct rs *adj = lk->sp.adj;
		int err, i;
		/* 
		   generate new test data */
		sl->tlen = jiffies & 0x0f;
		for (i = 0; i < sl->tlen; i++)
			sl->tdata[i] ^= ((jiffies >> 8) & 0xff);
		/* 
		   start new signalling link test */
		if ((err =
		     mtp_send_sltm(NULL, loc, lk->ni, adj->dest, loc->pc, sl->slc, sl->slc,
				   sl->tdata, sl->tlen, sl)))
			return (err);
		sl_timer_start(sl, t1t);
		sl->flags &= ~SLF_WACK_SLTM2;
		sl->flags |= SLF_WACK_SLTM;
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  TIMER T1S  -  SSLTM ack
 *  -----------------------------------
 */
STATIC int
sl_ssltm_failed(queue_t *q, struct sl *sl)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}
STATIC int
sl_t1s_timeout(struct sl *sl)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;
	if (sl->flags & (SLF_WACK_SSLTM | SLF_WACK_SSLTM2)) {
		if (sl->flags & SLF_WACK_SSLTM) {
			sl_timer_start(sl, t1s);
			if ((err =
			     mtp_send_ssltm(NULL, loc, lk->ni, adj->dest, loc->pc, sl->slc, sl->slc,
					    sl->tdata, sl->tlen, sl)) < 0)
				goto error;
			sl->flags &= ~SLF_WACK_SSLTM;
			sl->flags |= SLF_WACK_SSLTM2;
			return (QR_DONE);
		}
		if ((err = sl_ssltm_failed(NULL, sl)) < 0)
			goto error;
		sl->flags &= ~SLF_WACK_SSLTM2;
		return (QR_DONE);
	}
	swerr();
	return (-EFAULT);
      error:
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Receive message functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  5 Changeover
 *
 *  5.1.2 Changeover includes the procedure to be used in the case of unavailability (due to failure, blocking or
 *  inhibiting) of a signalling link, in order to diver the traffic pertaining to that signalling link to one or
 *  more alternative signalling links.
 *
 *  5.2.1 Siganlling traffic diverted from an unavailable signalling link is routed by the concerned signalling
 *  point according to the rules specified in clause 4.  In summary, two alternative situations may arise (either
 *  for the whole diverted traffic or for traffic relating to each particular destination): i) traffic is diverted
 *  to one or more signalling links of the same link set; or, ii) traffic is diverted to one or more different link
 *  sets.
 *
 *  5.2.2 As a result of these arrangements, and of the message routing function described in clause 2, three
 *  different relationships between the new signalling link and the unavailable one can be identified, for each
 *  particular traffic flow.  These three basic cases may be summarized as follows: a) the new signalling link is
 *  parallel to the unavailable one (see Figure 9); b) the new signalling link belongs to a signalling route other
 *  than that to which the unavailable signalling link belongs, but this signalling route still passes through the
 *  signaling point at the far end of the unavailable signalling link (see Figure 10); c) the new signalling link
 *  belongs to a signalling route other than that to which the unavailable signalling link belongs, and this
 *  signalling route does not pass through the signalling point acting as signalling transfer point, at the far end
 *  of the unavailable signalling link (see Figure 11).
 *
 *  5.3 Changeover initiation and actions
 *
 *  5.3.1  Changeover is initiated at a signalling
 *
 *
 */
/*
 *  RECV COO
 *  -----------------------------------
 */
STATIC int
mtp_recv_coo(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;
	int err;
	/* 
	   first find the signalling link it pertains to */
	if (!(sl = mtp_lookup_sl(sl, m)))
		goto eproto;
	switch (sl_get_l_state(sl)) {
	case SLS_WIND_INSI:
	case SLS_IN_SERVICE:
	case SLS_PROC_OUTG:
	case SLS_WACK_SLTM:
		return sl_stop_restore(q, sl);
	case SLS_WIND_BSNT:
		/* 
		   haven't launched COO/ECO yet */
		sl->flags |= SLF_COO_RECV;
		/* 
		   we will ack the COO when BSNT is retrieved or fails */
		return (QR_DONE);
	case SLS_WACK_COO:
	case SLS_WACK_ECO:
	case SLS_WCON_RET:
		if (sl->fsnc != -1) {
			/* 
			   BSNT already retrieved -- reply anyway with COA */
			if ((err =
			     mtp_send_coa(q, sp, m->ni, m->opc, m->dpc, m->sls, m->slc, sl->fsnc)))
				goto error;
		} else {
			/* 
			   BSNT failed retrieval -- reply anyway with ECA */
			if ((err = mtp_send_eca(q, sp, m->ni, m->opc, m->dpc, m->sls, m->slc)))
				goto error;
		}
		if (sl_get_l_state(sl) != SLS_WCON_RET) {
			/* 
			   initiate retrieval and wait for result */
			if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
				goto error;
			return sl_retrieval_request_and_fsnc_req(q, sl, m->arg.fsnl);
		}
		return (QR_DONE);
	      error:
		return (err);
	default:
		swerr();
		return (-EFAULT);
	}
      eproto:
	__ptrace(("\n"));
	return (-EPROTO);
}

/*
 *  RECV COA
 *  -----------------------------------
 */
STATIC int
mtp_recv_coa(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	if (!(sl = mtp_lookup_sl(sl, m)))
		goto eproto;
	switch (sl_get_l_state(sl)) {
	case SLS_WACK_COO:
	case SLS_WACK_ECO:
		/* 
		   We were expecting a response */
		/* 
		   leave ack timer running to cover retrieval */
		/* 
		   do buffer updating because it is COA */
		if ((err = sl_clear_rtb_req(q, sl)))
			goto error;
		/* 
		   initiate retrieval and wait for result */
		if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
			goto error;
		return sl_retrieval_request_and_fsnc_req(q, sl, m->arg.fsnl);
	}
	return (QR_DONE);	/* discard */
      eproto:
	rare();
	return (-EPROTO);
      error:
	rare();
	return (err);
}

/*
 *  RECV CDB
 *  -----------------------------------
 *
 *  6.2.1  Changeback is initiated at a signalling point when a signalling link is restored, unblocked or
 *  uninhibited, and therefore it becomes once again available, according to the criteria listed in 3.2.3 and 3.2.7.
 *  The following actions are then performed:
 *
 *  a)  the alternative signalling link(s) to which traffic normally carried by the siganalling link make available
 *  was previously diverted (e.g. on occurence of a changeover), are determined.  To this set are added, if
 *  applicable, other links determined as defined in 4.4.2;
 *
 *  b)  signalling traffic is diverted (if appropriate, according to the criteria specified in clause 4) to the
 *  concerned signalling link by means of the sequence control procedure specified in 6.3; traffic diversion can be
 *  performed at the discretion of the siganlling point initiating changeback, as follows:
 *
 *  i)      individually for each traffic flow (i.e. on destination basis);
 *
 *  ii)     individually for each alternative signalling link (i.e. for all the destinations previously diverted on
 *  that alternative signalling link);
 *
 *  iii)    at the same time for a number of, or for all the alternative signalling links.
 *
 *  6.3.2   ... The remote signalling point will send the changeback acknowledgement to the signalling point
 *  initiated changeback in response to the changeback declaration; any available signalling route between the two
 *  signalling points can be used to carry the changeback acknowledgement.
 *
 *  6.5.2   If a changeback-declaration is received after the completion of the changeback procedure, a
 *  changeback-acknowledgement is sent in response, without taking any further action.  This corresponds to the
 *  normal action described in 6.3.2 above.
 */
STATIC int
mtp_recv_cbd(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;
	return mtp_send_cba(q, sp, lk->ni, m->opc, sp->pc, 0, m->slc, m->arg.cbc);
}

/*
 *  RECV CBA
 *  -----------------------------------
 *  6.5.1   If a changeback-acknowledgement is received by a signalling point which has not previously sent a
 *  changeback-declaration, no action is taken.
 */
STATIC int
mtp_recv_cba(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct cb *cb;
	for (cb = master.cb.list; cb; cb = cb->next) {
		struct lk *lk = cb->lk.lk;
		if (cb->cbc == m->arg.cbc && cb->slc == m->slc && m->opc == lk->sp.adj->dest)
			/* 
			   restart traffic */
			return cb_reroute_buffer(cb);
	}
	return (-EPROTO);
}

/*
 *  RECV ECO
 *  -----------------------------------
 */
STATIC int
mtp_recv_eco(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;
	int err;
	if (!(sl = mtp_lookup_sl(sl, m)))
		goto eproto;
	switch (sl_get_l_state(sl)) {
	case SLS_WIND_INSI:
	case SLS_IN_SERVICE:
	case SLS_PROC_OUTG:
	case SLS_WACK_SLTM:
		return sl_stop_restore(q, sl);
	case SLS_WIND_BSNT:
		/* 
		   haven't launched COO/ECO yet */
		sl->flags |= SLF_ECO_RECV;
		/* 
		   we will ack the ECO when BSNT is retrieved or fails */
		return (QR_DONE);
	case SLS_WACK_COO:
	case SLS_WACK_ECO:
	case SLS_WCON_RET:
		if (sl->fsnc != -1) {
			/* 
			   BSNT already retrieved -- reply anyway with COA */
			if ((err =
			     mtp_send_coa(q, sp, m->ni, m->opc, m->dpc, m->sls, m->slc, sl->fsnc)))
				goto error;
		} else {
			/* 
			   BSNT failed retrieval -- reply anyway with ECA */
			if ((err = mtp_send_eca(q, sp, m->ni, m->opc, m->dpc, m->sls, m->slc)))
				goto error;
		}
		if (sl_get_l_state(sl) != SLS_WCON_RET) {
			/* 
			   We didn't get FSNL in the ECO, therefore, we need to flush the
			   retransmission buffer of potential duplicates before retrieving the
			   transmission buffer. */
			/* 
			   do buffer flushing, and initiate retrieval */
			if ((err = sl_clear_rtb_req(q, sl)))
				goto error;
			/* 
			   initiate retrieval and wait for result */
			if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
				goto error;
			return sl_retrieval_request_and_fsnc_req(q, sl, -1UL);
		}
		return (QR_DONE);
	      error:
		return (err);
	default:
		swerr();
		return (-EFAULT);
	}
	rare();
	return (-EFAULT);
      eproto:
	rare();
	return (-EPROTO);
}

/*
 *  RECV ECA
 *  -----------------------------------
 */
STATIC int
mtp_recv_eca(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	if (!(sl = mtp_lookup_sl(sl, m)))
		goto eproto;
	switch (sl_get_l_state(sl)) {
	case SLS_WACK_COO:
	case SLS_WACK_ECO:
		/* 
		   We were expecting a response */
		/* 
		   leave ack timer running to cover retrieval */
		/* 
		   we can't do buffer updating because it is ECA */
		if ((err = sl_clear_rtb_req(q, sl)))
			goto error;
		/* 
		   initiate retrieval and wait for result */
		if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
			goto error;
		return sl_retrieval_request_and_fsnc_req(q, sl, -1UL);
	}
	return (QR_DONE);	/* discard */
      eproto:
	rare();
	return (-EPROTO);
      error:
	rare();
	return (err);
}

/*
 *  RECV ECA
 *  -----------------------------------
 *  13.9.6.  When a signalling route set congestion test message reaches its destination, it is discarded.
 */
STATIC int
mtp_recv_rct(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	(void) q;
	(void) mp;
	(void) m;
	return (QR_DONE);
}

/*
 *  TFC - Transfer Controlled
 *  -------------------------------------------------------------------------
 *  Multiple congestion priorities:
 *  -----------------------------------
 *  13.7.2   A transfer-controlled message relating to a given destination X is sent from a signalling transfer
 *  point Y to a received message originating from signalling point Z destined to signalling point X when the
 *  congestion prioirty of the concerned message is less than the current congesiton status of the signalling link
 *  selected to transmit the concerned message from Y to X.
 *
 *  In this case, the transfer-controlled message is sent to the originating point Z with the congestion status
 *  filed set to the current congestion status of the signalling link.
 *
 *  13.7.3.  When the originating signalling point Z receives a transfer-controlled message relating to destination
 *  X, it the current congestion status of the signalling route set towards destination X is less than the
 *  congestion status in the transfer-controlled message, it updates the congestion status of the signalling route
 *  set towards destination X with the value of the congestion status carried in the transfer-controlled message.
 *
 *  13.9.3.  If within T16 after sending a signalling route set congestion test message, a transfer controlled
 *  message relating to the concerned destination is received, the signalling point updates the congestion status of
 *  the signalling route set towards the concerned destination withi the value of the congestion status carried in
 *  the transfer controlled message.  Following this, the procedures pecified in 13.9.4 and 13.9.5 are performed.
 *
 *  Multiple congestion states:
 *  -----------------------------------
 *  13.8 The only use made of the TFC procedure by the national signalling network, using multiple congestion states
 *  without congestion priorities, it to convey the congestion indication primitive from the SP where congestion was
 *  detected to the originating SP in a transfer controlled message.
 *
 *  11.2.5 For national signalling networks using multiple signalling link congestion states without congestion
 *  priority, S + 1 (1 <= S <= 3) levels of route set congestion status are provided.  The procedure is the same as
 *  that specified in 11.2.3 [below], except that the MTP-STATUS primitive contains the congestion status as a
 *  parameter in addition to the DPC of the affected destination.
 *
 *  International:
 *  -----------------------------------
 *  13.6 The only use of the transfer controlled procedure in the international signalling network is to convey the
 *  congestion indication from the SP where congestion was detected to the originating SP in a transfer controlled
 *  message.
 *
 *  11.2.3.2 After the reception of a transfer controlled message, the receiving signalling point informs each level
 *  4 User Part of the affected destination by means of an MTP-STATUS primitive specified in 11.2.3.1 i).
 */
STATIC int
mtp_recv_tfc(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rs *rs;
	if ((rs = mtp_lookup_rs(sl, m, RS_TYPE_MEMBER))) {
		int err;
		uint newstatus, popt = sl->lk.lk->sp.loc->na.na->option.popt;
		/* 
		   13.7.6 In some circumstances it may happen that a signalling point receives a
		   transfer-controlled message relating to a destination which is already
		   inaccessible due to previous failures; in this case the transfer-controlled
		   message is ignored. */
		if (rs->state < RS_RESTART) {
			if (popt & SS7_POPT_MPLEV) {
				newstatus = m->arg.stat;
				if (rs->cong_status >= newstatus)
					goto done;
				rs_timer_stop(rs, t15);
				rs_timer_stop(rs, t16);
			} else if (popt & SS7_POPT_MCSTA) {
				newstatus = m->arg.stat;
			} else {
				newstatus = -1UL;
			}
			if ((err = mtp_cong_status_ind_all_local(q, rs, newstatus)))
				goto error;
			if (!(popt & SS7_POPT_MPLEV) && !rs->cong_status) {
				rs->cong_msg_count = 0;	/* new congestion, zero counts */
				rs->cong_oct_count = 0;	/* new congestion, zero counts */
			}
			rs->cong_status = newstatus;
			if (popt & SS7_POPT_MPLEV)
				rs_timer_start(rs, t15);
		}
	      done:
		return (QR_DONE);
	      error:
		return (err);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV TFP
 *  -----------------------------------
 *  13.5.2  A signalling-route-set-test message is sent from a signalling point after a transfer-prohibited or
 *  trasfer-restricted message is received from an adjacent signalling trasfer point (see 13.2.4 and
 *
 *  13.4.4).  In this case, a signalling-route-set-test message is sent to that signalling transfer point referring
 *  to the destination declared inaccessible or restricted by the transfer-prohibited or transfer-restricted
 *  message, every T10 period (see clause 16) until a transfer-allowed message, indicating that the desination has
 *  become accessible, is received.
 *
 *  13.2.4  In some circumstances it may happen that a signalling point receives either a repeated
 *  transfer-prohibited message relating to a non-existent route (i.e., there is no route from that signalling point
 *  to the concerned destination via signalling transfer point Y, according to the signalling network configuration)
 *  or to a destination which is already inaccessible, due to previous failures; in this case, no actions are taken.
 */
STATIC int
mtp_recv_tfp(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt(sl, m, RT_TYPE_MEMBER))) {
		int err;
		if ((rt->flags & RTF_PROHIBITED) || (err = rt_set_state(q, rt, RT_PROHIBITED)) >= 0)
			return (QR_DONE);
		return (err);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV TCP
 *  -----------------------------------
 */
STATIC int
mtp_recv_tcp(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt(sl, m, RT_TYPE_CLUSTER))) {
		int err;
		if ((rt->state & RTF_PROHIBITED) || (err = rt_set_state(q, rt, RT_PROHIBITED)) >= 0)
			return (QR_DONE);
		return (err);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV TFR
 *  -----------------------------------
 *  13.5.2  A signalling-route-set-test message is sent from a signalling point after a transfer-prohibited or
 *  trasfer-restricted message is received from an adjacent signalling trasfer point (see 13.2.4 and
 *
 *  13.4.4).  In this case, a signalling-route-set-test message is sent to that signalling transfer point referring
 *  to the destination declared inaccessible or restricted by the transfer-prohibited or transfer-restricted
 *  message, every T10 period (see clause 16) until a transfer-allowed message, indicating that the desination has
 *  become accessible, is received.
 *
 *  13.4.4  In some cirumstances, it may happen that a signalling point receives either a repeated
 *  transfer-restricted message or a transfer-retricted message message relating to a non-existent route (i.e.
 *  there is not route from that signalling point to the concerned destination via signalling transfer point Y,
 *  according to the signalling network configuration); in this case, no actions are taken.
 */
STATIC int
mtp_recv_tfr(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt(sl, m, RT_TYPE_MEMBER))) {
		int err;
		if ((rt->state & RTF_RESTRICTED) || (err = rt_set_state(q, rt, RT_RESTRICTED)) >= 0)
			return (QR_DONE);
		return (err);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV TCR
 *  -----------------------------------
 */
STATIC int
mtp_recv_tcr(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt(sl, m, RT_TYPE_CLUSTER))) {
		int err;
		if ((rt->state & RTF_RESTRICTED) || (err = rt_set_state(q, rt, RT_RESTRICTED)) >= 0)
			return (QR_DONE);
		return (err);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV TFA
 *  -----------------------------------
 *  13.3.2  A transfer-allowed message relating to a given destination X is sent from a signalling transfer point Y
 *  in the following cases:
 *
 *  i)  When signalling transfer point Y stops routing (at changeback or controlled re-routing), signalling traffic
 *  destination to signalling point X via a signalling transfer point Z (to which the concerned traffic was
 *  previously divered as a consequence of changeover or forced rerouting).  In this case the transfer-allowed
 *  message is sent to signalling transfer point Z.
 *
 *  ii) When signalling transfer point Y recognizes that it is again able to transfer signalling traffic destined to
 *  signalling point Y (see 6.2.3 and 8.2.3).  In this case a transfer-allowed message is sent to all accessible
 *  adjacent signalling points, except those signalling points that receive a transfer-prohibited message according
 *  to 13.2.2 i) and except signalling point X if it is an adjacent signalling point.  (Broadcast Method).
 *
 *  13.3.4  In some circumstances it may happen that a signalling point receives either a repeated transfer-allowed
 *  message or a transfer allowed message relating to a non-existent signalling route (i.e. there is not route from
 *  that signalling point to the concerned destination via signalling transfer point Y according to the signalling
 *  network configuration); in this case no actions are taken.
 */
STATIC int
mtp_recv_tfa(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt(sl, m, RT_TYPE_MEMBER))) {
		int err;
		if ((rt->state & RTF_ALLOWED) || (err = rt_set_state(q, rt, RT_ALLOWED)) >= 0)
			return (QR_DONE);
		return (err);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV TCA
 *  -----------------------------------
 */
STATIC int
mtp_recv_tca(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt(sl, m, RT_TYPE_CLUSTER))) {
		int err;
		if ((rt->state & RTF_ALLOWED) || (err = rt_set_state(q, rt, RT_ALLOWED)) >= 0)
			return (QR_DONE);
		return (err);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV RST
 *  -----------------------------------
 *  13.5.4  At the reception of a signalling route set test message, a signalling transfer point will compare the
 *  status of the destination to the received message with the actual status of the destination.  If they are the
 *  same, no further action is taken.  If they are the same, no action is taken, If they are different, one of the
 *  following message is sent in response, dictated by the actual status of the destination:
 *
 *  -   a transfer-allowed message, referring to the destination the accesibility of which is tested, if the
 *  signalling transfer point can reach the indicated destination via a signalling link not connected to the
 *  signalling point from which the signalling route set test message was received (and if the transfer restricted
 *  procedure is used in the the network, the signalling link is on the normal route or an equally efficient
 *  alternative route);
 *
 *  -   a transfer-restricted message when access to the destination is possible via an alternative to the normal
 *  routing which is less efficient, but still not via the signalling point from which the signalling route set test
 *  was originated;
 *
 *  -   a transfer-prohibited message in all other caes (including the inaccessiblity of the destination).
 *
 *  9.6.3  A signalling route test message received in a restarting MTP is ignored during the MTP restart procedure.
 *
 *  Signalling route set test messages received in a signalling point adjacent to signalling point Y whose MTP is
 *  restarting before T21 expires are handled, but the replies assume that all signalling routes using Y are
 *  prohibited.
 */
STATIC int
mtp_recv_rst(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt_test(sl, m, RT_TYPE_MEMBER))) {
		struct lk *lk = sl->lk.lk;
		struct sp *sp = lk->sp.loc;
		struct rl *rl = rt->rl.rl;
		struct rs *rs = rl->rs.rs;
		/* 
		   treate a non-primary route that we are using as the current route (C-Links) as
		   prohibited */
		if (rs->rl.curr == rl && rs->rl.list != rl)
			goto prohibited;
		if (rs->flags & RSF_ALLOWED)
			return mtp_send_tfa(q, sp, lk->ni, m->opc, sp->pc, 0, rs->dest);
		else if (rs->flags & RSF_RESTRICTED)
			return mtp_send_tfr(q, sp, lk->ni, m->opc, sp->pc, 0, rs->dest);
		else if (rs->flags & RSF_PROHIBITED)
		      prohibited:
			return (QR_DONE);
		else
			return (QR_DONE);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV RSR
 *  -----------------------------------
 */
STATIC int
mtp_recv_rsr(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt_test(sl, m, RT_TYPE_MEMBER))) {
		struct lk *lk = sl->lk.lk;
		struct sp *sp = lk->sp.loc;
		struct rl *rl = rt->rl.rl;
		struct rs *rs = rl->rs.rs;
		/* 
		   treate a non-primary route that we are using as the current route (C-Links) as
		   prohibited */
		if (rs->rl.curr == rl && rs->rl.list != rl)
			goto prohibited;
		if (rs->flags & RSF_ALLOWED)
			return mtp_send_tfa(q, sp, lk->ni, m->opc, sp->pc, 0, rs->dest);
		else if (rs->flags & RSF_RESTRICTED)
			return (QR_DONE);
		else if (rs->flags & RSF_PROHIBITED)
		      prohibited:
			return mtp_send_tfp(q, sp, lk->ni, m->opc, sp->pc, 0, rs->dest);
		else
			return (QR_DONE);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV RCP
 *  -----------------------------------
 */
STATIC int
mtp_recv_rcp(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt_test(sl, m, RT_TYPE_CLUSTER))) {
		struct lk *lk = sl->lk.lk;
		struct sp *sp = lk->sp.loc;
		struct rl *rl = rt->rl.rl;
		struct rs *rs = rl->rs.rs;
		/* 
		   treate a non-primary route that we are using as the current route (C-Links) as
		   prohibited */
		if (rs->rl.curr == rl && rs->rl.list != rl)
			goto prohibited;
		if (rs->flags & RSF_ALLOWED)
			return mtp_send_tca(q, sp, lk->ni, m->opc, sp->pc, 0, rs->dest);
		else if (rs->flags & RSF_RESTRICTED)
			return mtp_send_tcr(q, sp, lk->ni, m->opc, sp->pc, 0, rs->dest);
		else if (rs->flags & RSF_PROHIBITED)
		      prohibited:
			return (QR_DONE);
		else
			return (QR_DONE);
	}
	/* 
	   discard */
	return (-EPROTO);
}

/*
 *  RECV RCR
 *  -----------------------------------
 */
STATIC int
mtp_recv_rcr(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt_test(sl, m, RT_TYPE_CLUSTER))) {
		struct lk *lk = sl->lk.lk;
		struct sp *sp = lk->sp.loc;
		struct rl *rl = rt->rl.rl;
		struct rs *rs = rl->rs.rs;
		/* 
		   treate a non-primary route that we are using as the current route (C-Links) as
		   prohibited */
		if (rs->rl.curr == rl && rs->rl.list != rl)
			goto prohibited;
		if (rs->flags & RSF_ALLOWED)
			return mtp_send_tca(q, sp, lk->ni, m->opc, sp->pc, 0, rs->dest);
		else if (rs->flags & RSF_RESTRICTED)
			return (QR_DONE);
		else if (rs->flags & RSF_PROHIBITED)
		      prohibited:
			return mtp_send_tcp(q, sp, lk->ni, m->opc, sp->pc, 0, rs->dest);
		else
			return (QR_DONE);
	}
	/* 
	   discard */
	return (-EPROTO);
}

STATIC int
mtp_recv_lin(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	if ((sl = mtp_lookup_sl(sl, m))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_lun(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	if ((sl = mtp_lookup_sl(sl, m))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_lia(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	if ((sl = mtp_lookup_sl(sl, m))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_lua(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	if ((sl = mtp_lookup_sl(sl, m))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_lid(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	if ((sl = mtp_lookup_sl(sl, m))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_lfu(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	if ((sl = mtp_lookup_sl(sl, m))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_llt(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	if ((sl = mtp_lookup_sl(sl, m))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_lrt(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	if ((sl = mtp_lookup_sl(sl, m))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_tra(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt(sl, m, RT_TYPE_MEMBER))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_trw(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	if ((rt = mtp_lookup_rt(sl, m, RT_TYPE_MEMBER))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_dlc(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	__ptrace(("Unimplemeneted\n"));
	return (-EFAULT);
}

STATIC int
mtp_recv_css(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	__ptrace(("Unimplemeneted\n"));
	return (-EFAULT);
}

STATIC int
mtp_recv_cns(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	__ptrace(("Unimplemeneted\n"));
	return (-EFAULT);
}

STATIC int
mtp_recv_cnp(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	__ptrace(("Unimplemeneted\n"));
	return (-EFAULT);
}

STATIC int
mtp_recv_upu(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rs *rs;
	if ((rs = mtp_lookup_rs(sl, m, RS_TYPE_MEMBER))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_upa(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rs *rs;
	if ((rs = mtp_lookup_rs(sl, m, RS_TYPE_MEMBER))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

STATIC int
mtp_recv_upt(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rs *rs;
	if ((rs = mtp_lookup_rs(sl, m, RS_TYPE_MEMBER))) {
		fixme(("Write this function\n"));
		return (-EFAULT);
	}
	return (-EPROTO);
}

/*
 *  RECV SLTM
 *  -----------------------------------
 *  Signalling link test messages must be received on the same signalling link to which they pertain and must have
 *  the correct originating and destination point codes and signalling link code.  Otherwise they are discarded.
 */
STATIC int
mtp_recv_sltm(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	if (m->slc != sl->slc || m->opc != adj->dest || m->dpc != loc->pc) {
		if (sl->timers.t1t && m->slc == sl->slc && m->opc == loc->pc && m->dpc == adj->dest)
			goto loopback;
		else
			goto discard;
	}
	return mtp_send_slta(q, loc, lk->ni, adj->dest, loc->pc, sl->slc, sl->slc, m->data,
			     m->dlen);
      discard:
	ptrace(("PROTOCOL ERROR: Invalid SLTM received\n"));
	return (-EPROTO);
      loopback:
	ptrace(("PROTOCOL ERROR: Signalling link loopback detected\n"));
	return (-EPROTO);
}

/*
 *  RECV SLTA
 *  -----------------------------------
 *  Q.707 2.2 ... The signalling link test will be considered successful only if the received signalling link test
 *  acknowledgement message fulfills the following criteria:
 *
 *  a)  the SLC identifies the physical signalling link on which the SLTA was received.
 *
 *  b)  the OPC identifies the signalling point at the other end of the link.
 *
 *  c)  the test pattern is correct.
 *
 *  In the case when the criteria given above are not met or a signalling link test acknowledgement message is not
 *  received on the link being tested within T1 (see 5.5) after the signalling link test message has been sent, the
 *  test is considered to have failed and is repeated once.  In the case when also the repeat test fails, the
 *  following actions have to be taken:
 *
 *  -   SLT applied on activation/restoration, the link is put out of service, restoration is attempted and a
 *  management system must be informed.
 *
 *  -   SLT applied periodically, for further study.
 *
 *  (Same for ANSI.)
 */
STATIC int
mtp_recv_slta(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;
	if (sl_get_l_state(sl) != SLS_IN_SERVICE && sl_get_l_state(sl) != SLS_WACK_SLTM)
		goto discard;
	if (!(sl->flags & (SLF_WACK_SLTM | SLF_WACK_SLTM2)))
		goto discard;
	sl_timer_stop(sl, t1t);
	if (m->slc == sl->slc && m->opc == adj->dest && m->dpc == loc->pc) {
		int i;
		for (i = 0; i < m->dlen; i++)
			if (m->data[i] != sl->tdata[i])
				goto failed;
		if (sl_get_l_state(sl) == SLS_WACK_SLTM) {
			/* 
			   the signalling link is now able to carry traffic */
			sl_timer_stop(sl, t17);
			if (sl->lk.lk->sp.loc->flags & SPF_LOSC_PROC_B) {
				/* 
				   link is in probation */
				sl_timer_start(sl, t33a);
			}
			if ((err = sl_set_state(q, sl, SL_ACTIVE)))
				return (err);
			/* 
			   in service at level 3 */
			sl_set_l_state(sl, SLS_IN_SERVICE);
			return (QR_DONE);
		}
		/* 
		   no further action taken on success */
		sl_timer_start(sl, t2t);	/* start periodic test */
		return (QR_DONE);
	}
      failed:
	if (sl->flags & SLF_WACK_SLTM) {
		/* 
		   repeat test */
		if ((err =
		     mtp_send_sltm(q, loc, lk->ni, adj->dest, loc->pc, sl->slc, sl->slc, sl->tdata,
				   sl->tlen, sl)))
			return (err);
		sl_timer_start(sl, t1t);
		sl->flags &= ~SLF_WACK_SLTM;
		sl->flags |= SLF_WACK_SLTM2;	/* second attempt */
		return (QR_DONE);
	}
	if (sl_get_l_state(sl) == SLS_WACK_SLTM)
		return sl_stop_restore(q, sl);
	sl->flags &= ~SLF_WACK_SLTM2;
	/* 
	   IMPLEMENTATION NOTE:- The action when the periodic test fails is for further study.
	   Taking the link out of service can be an especially bad idea during times of congestion. 
	   This is because the buffers may be filled with management messages already and the
	   signaling link test might fail due to timeout. We merely report the information to
	   management and leave the link in service. If the test failed because of an inserted
	   loopback while the link is in service, the link will fail rapidly due to other causes.
	   So we do nothing more than report to management. */
	sl_timer_start(sl, t2t);	/* restart periodic test */
	ptrace(("PROTOCOL ERROR: Periodic SLT failed\n"));
	return (-EPROTO);
      discard:
	ptrace(("PROTOCOL ERROR: Received unexpected SLTA\n"));
	return (-EPROTO);
}

/*
 *  RECV SSLTM
 *  -----------------------------------
 */
STATIC int
mtp_recv_ssltm(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	if (m->slc != sl->slc || m->opc != adj->dest || m->dpc != loc->pc)
		goto eproto;
	return mtp_send_sslta(q, loc, lk->ni, m->opc, m->dpc, sl->slc, sl->slc, m->data, m->dlen);
      eproto:
	rare();
	return (-EPROTO);
}

/*
 *  RECV SSLTA
 *  -----------------------------------
 */
STATIC int
sl_ssltm_success(queue_t *q, struct sl *sl)
{
	fixme(("Write this function\n"));
	return (QR_DONE);
}
STATIC int
mtp_recv_sslta(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int i;
	if (m->slc != sl->slc || m->opc != adj->dest || m->dpc != loc->pc)
		goto eproto;
	if (sl->flags & (SLF_WACK_SSLTM | SLF_WACK_SSLTM2))
		goto eproto;
	sl_timer_stop(sl, t1s);
	if (m->slc != sl->slc || m->opc != adj->dest || m->dpc != loc->pc)
		goto failed;
	for (i = 0; i < m->dlen; i++)
		if (m->data[i] != sl->tdata[i])
			goto failed;
	return sl_ssltm_success(q, sl);
      failed:
	if (sl->flags & SLF_WACK_SSLTM) {
		int err;
		if ((err =
		     mtp_send_ssltm(q, loc, lk->ni, adj->dest, loc->pc, sl->slc, sl->slc, sl->tdata,
				    sl->tlen, sl)) < 0)
			return (err);
		sl_timer_start(sl, t1s);
		sl->flags &= ~SLF_WACK_SSLTM;
		sl->flags |= ~SLF_WACK_SSLTM2;
		return (QR_DONE);
	}
	sl->flags &= ~SLF_WACK_SSLTM2;
	return sl_ssltm_failed(q, sl);
      eproto:
	return (-EPROTO);
}

/*
 *  RECV USER
 *  -----------------------------------
 *  Note: if we are restarting, we never get here.
 */
STATIC int
mtp_recv_user(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct sp *sp = sl->lk.lk->sp.loc;
	return mtp_transfer_ind_local(q, sp, m);
}

/*
 *  =========================================================================
 *
 *  MESSAGE DECODING
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Process message functions
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_proc_msg(queue_t *q, mblk_t *mp, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	unsigned char tag;
	if (!(sl->lk.lk->sp.loc->flags & (SPF_RESTART | SPF_RESTART_PHASE_1 | SPF_RESTART_PHASE_2))) {
		switch (m->si) {
		default:	/* USER PART */
			return mtp_recv_user(q, mp, m);
		case 0:	/* SNMM */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* coo */
				return mtp_recv_coo(q, mp, m);
			case 0x12:	/* coa */
				return mtp_recv_coa(q, mp, m);
			case 0x15:	/* cbd */
				return mtp_recv_cbd(q, mp, m);
			case 0x16:	/* cba */
				return mtp_recv_cba(q, mp, m);
			case 0x21:	/* eco */
				return mtp_recv_eco(q, mp, m);
			case 0x22:	/* eca */
				return mtp_recv_eca(q, mp, m);
			case 0x31:	/* rct */
				return mtp_recv_rct(q, mp, m);
			case 0x32:	/* tfc */
				return mtp_recv_tfc(q, mp, m);
			case 0x41:	/* tfp */
				return mtp_recv_tfp(q, mp, m);
			case 0x42:	/* tcp */
				return mtp_recv_tcp(q, mp, m);
			case 0x43:	/* tfr */
				return mtp_recv_tfr(q, mp, m);
			case 0x44:	/* tcr */
				return mtp_recv_tcr(q, mp, m);
			case 0x45:	/* tfa */
				return mtp_recv_tfa(q, mp, m);
			case 0x46:	/* tca */
				return mtp_recv_tca(q, mp, m);
			case 0x51:	/* rst */
				return mtp_recv_rst(q, mp, m);
			case 0x52:	/* rsr */
				return mtp_recv_rsr(q, mp, m);
			case 0x53:	/* rcp */
				return mtp_recv_rcp(q, mp, m);
			case 0x54:	/* rcr */
				return mtp_recv_rcr(q, mp, m);
			case 0x61:	/* lin */
				return mtp_recv_lin(q, mp, m);
			case 0x62:	/* lun */
				return mtp_recv_lun(q, mp, m);
			case 0x63:	/* lia */
				return mtp_recv_lia(q, mp, m);
			case 0x64:	/* lua */
				return mtp_recv_lua(q, mp, m);
			case 0x65:	/* lid */
				return mtp_recv_lid(q, mp, m);
			case 0x66:	/* lfu */
				return mtp_recv_lfu(q, mp, m);
			case 0x67:	/* llt */
				return mtp_recv_llt(q, mp, m);
			case 0x68:	/* lrt */
				return mtp_recv_lrt(q, mp, m);
			case 0x71:	/* tra */
				return mtp_recv_tra(q, mp, m);
			case 0x72:	/* trw */
				return mtp_recv_trw(q, mp, m);
			case 0x81:	/* dlc */
				return mtp_recv_dlc(q, mp, m);
			case 0x82:	/* css */
				return mtp_recv_css(q, mp, m);
			case 0x83:	/* cns */
				return mtp_recv_cns(q, mp, m);
			case 0x84:	/* cnp */
				return mtp_recv_cnp(q, mp, m);
			case 0xa1:	/* upu */
				return mtp_recv_upu(q, mp, m);
			case 0xa2:	/* upa *//* ansi91 only */
				return mtp_recv_upa(q, mp, m);
			case 0xa3:	/* upt *//* ansi91 only */
				return mtp_recv_upt(q, mp, m);
			}
			break;
		case 1:	/* SNTM */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* sltm */
				return mtp_recv_sltm(q, mp, m);
			case 0x12:	/* slta */
				return mtp_recv_slta(q, mp, m);
			}
			break;
		case 2:	/* SSNTM */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* ssltm */
				return mtp_recv_ssltm(q, mp, m);
			case 0x12:	/* sslta */
				return mtp_recv_sslta(q, mp, m);
			}
			break;
		}
	} else {
		switch (m->si) {
		default:	/* USER PART */
			/* 
			   9.6.6 ... All messages received during the restart procedure concerning
			   a local MTP User (service indicator != 0000 and != 0001) are discarded. */
			goto discard;
		case 0:	/* SNMM */
			/* 
			   9.6.6 ... All messages received with service indicator == 0000 in a
			   restarting MTP for the signalling point itself are treated as described
			   in the MTP restart procedure. Those messages not described elsewhere in
			   the procedure are discarded and no further action is taken on them
			   (message groups CHM, ECM, FCM, RSM, UFC, MIM and DLM). */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* coo */
			case 0x12:	/* coa */
			case 0x15:	/* cbd */
			case 0x16:	/* cba */
			case 0x21:	/* eco */
			case 0x22:	/* eca */
			case 0x31:	/* rct */
			case 0x32:	/* tfc */
				goto discard;
			case 0x41:	/* tfp */
				return mtp_recv_tfp(q, mp, m);
			case 0x42:	/* tcp */
				return mtp_recv_tcp(q, mp, m);
			case 0x43:	/* tfr */
				return mtp_recv_tfr(q, mp, m);
			case 0x44:	/* tcr */
				return mtp_recv_tcr(q, mp, m);
			case 0x45:	/* tfa */
				return mtp_recv_tfa(q, mp, m);
			case 0x46:	/* tca */
				return mtp_recv_tca(q, mp, m);
			case 0x51:	/* rst */
			case 0x52:	/* rsr */
			case 0x53:	/* rcp */
			case 0x54:	/* rcr */
				/* 
				   9.6.3 A signalling route set test message received in a
				   restarting MTP is ignored during the MTP restart procedure. */
			case 0x61:	/* lin */
			case 0x62:	/* lun */
			case 0x63:	/* lia */
			case 0x64:	/* lua */
			case 0x65:	/* lid */
			case 0x66:	/* lfu */
			case 0x67:	/* llt */
			case 0x68:	/* lrt */
				goto discard;
			case 0x71:	/* tra */
				return mtp_recv_tra(q, mp, m);
			case 0x72:	/* trw */
				return mtp_recv_trw(q, mp, m);
			case 0x81:	/* dlc */
			case 0x82:	/* css */
			case 0x83:	/* cns */
			case 0x84:	/* cnp */
			case 0xa1:	/* upu */
			case 0xa2:	/* upa *//* ansi91 only */
			case 0xa3:	/* upt *//* ansi91 only */
				goto discard;
			}
			break;
		case 1:	/* SNTM */
			/* 
			   9.6.7 ... Messages received with service indicator == 0001 are handled
			   normally during the restart procedure. */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* sltm */
				return mtp_recv_sltm(q, mp, m);
			case 0x12:	/* slta */
				return mtp_recv_slta(q, mp, m);
			}
			break;
		case 2:	/* SSNTM */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* ssltm */
				return mtp_recv_ssltm(q, mp, m);
			case 0x12:	/* sslta */
				return mtp_recv_sslta(q, mp, m);
			}
			break;
		}
	}
	__ptrace(("%s: %p: DECODE: Invalid message type\n", DRV_NAME, sl));
	return (-EPROTO);
      discard:
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Decode message functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Decode Changeover messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +-v-------------+
 *  |0|    FSNL     |
 *  +-^-------------+
 *  ANSI Format:
 *  +---------v-----+-------v-------+
 *  |    0    |    FSNL     |  SLC  |
 *  +---------^-----+-------^-------+
 */
STATIC int
mtp_dec_com(mblk_t *mp, struct mtp_msg *m)
{
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		m->slc = m->sls;
		m->arg.fsnl = *mp->b_rptr++ & 0x7f;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		m->slc = *mp->b_rptr & 0x0f;
		m->arg.fsnl = (*mp->b_rptr++ >> 4) | ((*mp->b_rptr++ & 0x7) << 4);
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode Changeback messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +---------------+
 *  |      CBC      |
 *  +---------------+
 *  ANSI Format:
 *  +-------v-------+-------v-------+
 *  |   0   |      CBC      |  SLC  |
 *  +-------^-------+-------^-------+
 */
STATIC int
mtp_dec_cbm(mblk_t *mp, struct mtp_msg *m)
{
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		m->slc = m->sls;
		m->arg.cbc = *mp->b_rptr++;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		m->slc = *mp->b_rptr & 0x0f;
		m->arg.cbc = (*mp->b_rptr++ >> 4) | ((*mp->b_rptr++ & 0xf) << 4);
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode Link Management messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +
 *  | (SLC contained in SLS in RL)
 *  +
 *  ANSI Format:
 *  +-------v-------+
 *  |   0   |  SLC  |
 *  +-------^-------+
 */
STATIC int
mtp_dec_slm(mblk_t *mp, struct mtp_msg *m)
{
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
		m->slc = m->sls;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		m->slc = *mp->b_rptr++ & 0x0f;
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode Transfer Controlled messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +---v-----------+---------------+
 *  | S |          DEST             |
 *  +---^-----------+---------------+
 *  ANSI, JTTC, CHIN Format:
 *  +-----------v---+---------------+---------------+---------------+
 *  |     0     | S |                     DEST                      |
 *  +-----------^---+---------------+---------------+---------------+
 */
STATIC int
mtp_dec_tfc(mblk_t *mp, struct mtp_msg *m)
{
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ITUT:
	case SS7_PVAR_ETSI:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		m->dest = (*mp->b_rptr++) | ((*mp->b_rptr & 0x3f) << 8);
		m->arg.stat = *mp->b_rptr++ >> 6;
		return (0);
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		if (mp->b_wptr < mp->b_rptr + 4)
			break;
		m->dest = (*mp->b_rptr++) | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16);
		m->arg.stat = (*mp->b_rptr++ & 0x3);
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode Traffic Flow Control and Test messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +---v-----------+---------------+
 *  | 0 |          DEST             |
 *  +---^-----------+---------------+
 *  ANSI, JTTC, CHIN Format:
 *  +---------------+---------------+---------------+
 *  |                     DEST                      |
 *  +---------------+---------------+---------------+
 */
STATIC int
mtp_dec_tfm(mblk_t *mp, struct mtp_msg *m)
{
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ITUT:
	case SS7_PVAR_ETSI:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		m->dest = (*mp->b_rptr++) | ((*mp->b_rptr & 0x3f) << 8);
		return (0);
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		if (mp->b_wptr < mp->b_rptr + 3)
			break;
		m->dest = (*mp->b_rptr++) | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16);
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode Data Link Connection message
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format: (12-bit SDLI)
 *  +-------v-------+---------------+
 *  |   0   |          SDLI         |
 *  +-------^-------+---------------+
 *  ANSI Format: (14-bit SDLI)
 *  +-----------v---+---------------+-------v-------+
 *  |     0     |           SDLI            |  SLC  |
 *  +-----------^---+---------------+-------^-------+
 */
STATIC int
mtp_dec_dlc(mblk_t *mp, struct mtp_msg *m)
{
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		m->slc = m->sls;
		m->arg.sdli = (*mp->b_rptr++) | ((*mp->b_rptr++ & 0x0f) << 8);
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 3)
			break;
		m->slc = (*mp->b_rptr & 0x0f);
		m->arg.sdli =
		    (*mp->b_rptr++ >> 4) | (*mp->b_rptr++ << 4) | ((*mp->b_rptr++ & 0x03) << 12);
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode User Part Flow Control messages
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +-------v-------+---v-----------+---------------+
 *  |   0   |  UPI  | 0 |          DEST             |
 *  +-------^-------+---^-----------+---------------+
 *  ANSI, JTTC, CHIN Format:
 *  +-------v-------+---------------+---------------+---------------+
 *  |   0   |  UPI  |                     DEST                      |
 *  +-------^-------+---------------+---------------+---------------+
 *
 */
STATIC int
mtp_dec_upm(mblk_t *mp, struct mtp_msg *m)
{
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
		if (mp->b_wptr < mp->b_rptr + 3)
			break;
		m->dest = ((*mp->b_rptr++)) | ((*mp->b_rptr++) & 0x3f << 8);
		m->arg.upi = (*mp->b_rptr++ & 0x0f);
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 4)
			break;
		m->dest = ((*mp->b_rptr++)) | ((*mp->b_rptr++ << 8)) | ((*mp->b_rptr++ << 16));
		m->arg.upi = (*mp->b_rptr++ & 0x0f);
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode Signalling Link Test Messages/Acknowledgements
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  ------------------------+-------v-------+
 *   ...   Test Data        |  TLI  |   0   |
 *  ------------------------+-------^-------+
 *  ANSI Format:
 *  ------------------------+-------v-------+
 *   ...   Test Data        |  TLI  |  SLC  |
 *  ------------------------+-------^-------+
 */
STATIC int
mtp_dec_sltm(mblk_t *mp, struct mtp_msg *m)
{
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
		if (m->si == 2)
			break;
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		m->slc = m->sls;
		m->dlen = *mp->b_rptr++ >> 4;
		m->data = mp->b_rptr;
		if (mp->b_rptr + m->dlen > mp->b_wptr)
			break;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		m->slc = *mp->b_rptr & 0x0f;
		m->dlen = *mp->b_rptr++ >> 4;
		m->data = mp->b_rptr;
		if (mp->b_wptr < mp->b_rptr + m->dlen)
			break;
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode User Part
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  All Formats:
 *  ------------------------+
 *   ...   User Part Data   |
 *  ------------------------+
 */
STATIC int
mtp_dec_user(mblk_t *mp, struct mtp_msg *m)
{
	if (mp->b_wptr < mp->b_rptr)
		goto error;
	m->data = mp->b_rptr;
	m->dlen = mp->b_wptr - mp->b_rptr;
	return (0);
      error:
	return (-EPROTO);
}

/*
 *  Decode Service Information Octet
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format:
 *  +---v---v-------+---v-----------+
 *  | NI| 0 |  SI   | 0 |           |
 *  +---^---^-------+---^-----------+
 *  ANSI Format:
 *  +---v---v-------+---v-----------+
 *  | NI| MP|  SI   | 0 |           |
 *  +---^---^-------+---^-----------+
 *  JTTC Format:
 *  +---v---v-------+---v-----------+
 *  | NI| 0 |  SI   | MP|           |
 *  +---^---^-------+---^-----------+
 */
STATIC int
mtp_dec_sio(mblk_t *mp, struct mtp_msg *m)
{
	/* 
	   decode si, mp and ni */
	switch (m->pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_JTTC:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		m->si = *mp->b_rptr & 0x0f;
		m->mp = (mp->b_rptr[-1] >> 6);	/* get mp from header */
		m->ni = (*mp->b_rptr++ >> 6) & 0x3;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		m->si = *mp->b_rptr & 0x0f;
		m->mp = (*mp->b_rptr >> 4) & 0x3;
		m->ni = (*mp->b_rptr++ >> 6) & 0x3;
		return (0);
	default:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		m->si = *mp->b_rptr & 0x0f;
		if (m->popt & SS7_POPT_MPLEV)
			m->mp = (*mp->b_rptr >> 4) & 0x3;
		else
			m->mp = 0;
		m->ni = (*mp->b_rptr++ >> 6) & 0x3;
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode Routing Label
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  ITUT, ETSI Format: (14-bit PC, 4-bit SLS)
 *  +-------v-------+---------------+---v-----------+---------------+
 *  |  SLS  |            OPC            |            DPC            |
 *  +-------^-------+---------------+---^-----------+---------------+
 *  ANSI, JTTC, CHIN Format: (24-bit PC, 5-bit SLS)
 *  +-----v---------+---------------+---------------+---------------+---------------+---------------+---------------+
 *  |     |   SLS   |                      OPC                      |                     DPC                       |
 *  +-----^---------+---------------+---------------+---------------+---------------+---------------+---------------+
 *  ANSI Format: (24-bit PC, 8-bit SLS)
 *  +---------------+---------------+---------------+---------------+---------------+---------------+---------------+
 *  |      SLS      |                      OPC                      |                     DPC                       |
 *  +---------------+---------------+---------------+---------------+---------------+---------------+---------------+
 */
STATIC int
mtp_dec_rl(mblk_t *mp, struct mtp_msg *m)
{
	/* 
	   decode the routing label */
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ETSI:
	case SS7_PVAR_ITUT:
		/* 
		   14-bit point codes - 32-bit RL */
		if (mp->b_wptr < mp->b_rptr + 4)
			break;
		m->dpc = (*mp->b_rptr++ | ((*mp->b_rptr & 0x3f) << 8));
		m->opc =
		    ((*mp->b_rptr++ >> 6) | (*mp->b_rptr++ << 2) | ((*mp->b_rptr & 0x0f) << 10));
		m->sls = (*mp->b_rptr++ >> 4) & 0x0f;
		return (0);
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		/* 
		   24-bit point codes - 56-bit RL */
		if (mp->b_wptr < mp->b_rptr + 7)
			break;
		m->dpc = (*mp->b_rptr++ | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16));
		m->opc = (*mp->b_rptr++ | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16));
		m->sls = *mp->b_rptr++;
		if ((m->pvar & SS7_PVAR_YR) != SS7_PVAR_00)
			m->sls &= 0x1f;
		else
			m->sls &= 0xff;
		return (0);
	}
	return (-EPROTO);
}

/*
 *  Decode Service Information Field
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  All Formats:
 *  --------------------+-------v-------+
 *         signal       |   H1  |  H0   |
 *  --------------------+-------^-------+
 */
STATIC int
mtp_dec_sif(mblk_t *mp, struct mtp_msg *m)
{
	unsigned char tag;
	switch (m->si) {
	case 0:		/* SNMM */
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		tag = (*mp->b_rptr << 4) | (*mp->b_rptr >> 4);
		m->h0 = *mp->b_rptr & 0x0f;
		m->h1 = (*mp->b_rptr++ >> 4) & 0x0f;
		switch (tag) {
		case 0x11:	/* coo */
		case 0x12:	/* coa */
			return mtp_dec_com(mp, m);
		case 0x15:	/* cbd */
		case 0x16:	/* cba */
			return mtp_dec_cbm(mp, m);
		case 0x71:	/* tra */
		case 0x72:	/* trw */
		case 0x31:	/* rct */
			return (0);
		case 0x32:	/* tfc */
			return mtp_dec_tfc(mp, m);
		case 0x41:	/* tfp */
		case 0x42:	/* tcp */
		case 0x43:	/* tfr */
		case 0x44:	/* tcr */
		case 0x45:	/* tfa */
		case 0x46:	/* tca */
		case 0x51:	/* rst */
		case 0x52:	/* rsr */
		case 0x53:	/* rcp */
		case 0x54:	/* rcr */
			return mtp_dec_tfm(mp, m);
		case 0x81:	/* dlc */
			return mtp_dec_dlc(mp, m);
		case 0x82:	/* css */
		case 0x83:	/* cns */
		case 0x84:	/* cnp */
		case 0x21:	/* eco */
		case 0x22:	/* eca */
		case 0x61:	/* lin */
		case 0x62:	/* lun */
		case 0x63:	/* lia */
		case 0x64:	/* lua */
		case 0x65:	/* lid */
		case 0x66:	/* lfu */
		case 0x67:	/* llt */
		case 0x68:	/* lrt */
			return mtp_dec_slm(mp, m);
		case 0xa1:	/* upu */
		case 0xa2:	/* upa *//* ansi91 only */
		case 0xa3:	/* upt *//* ansi91 only */
			return mtp_dec_upm(mp, m);
		}
		break;
	case 1:		/* SNTM */
	case 2:		/* SSNTM */
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		tag = (*mp->b_rptr << 4) | (*mp->b_rptr >> 4);
		m->h0 = *mp->b_rptr & 0x0f;
		m->h1 = (*mp->b_rptr++ >> 4) & 0x0f;
		switch (tag) {
		case 0x11:	/* sltm */
		case 0x12:	/* slta */
			return mtp_dec_sltm(mp, m);
		}
		break;
	default:		/* USER PART */
		return mtp_dec_user(mp, m);
	}
	return (-EPROTO);
}

/*
 *  Decode message
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mtp_dec_msg(queue_t *q, mblk_t *mp, struct mtp_msg *m, struct na *na)
{
	int err = -EMSGSIZE;
	uchar *b_rptr = mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + 1)
		goto error;
	m->bp = mp;
	m->xq = q;
	m->timestamp = jiffies;
	m->pvar = na->option.pvar;
	m->popt = na->option.popt;
	if ((err = mtp_dec_sio(mp, m)))
		goto error;
	if ((err = mtp_dec_rl(mp, m)))
		goto error;
	if ((err = mtp_dec_sif(mp, m)))
		goto error;
	mp->b_rptr = b_rptr;	/* restore read pointer */
	return (QR_DONE);
      error:
	ptrace(("%s: %p: DECODE: decoding error\n", DRV_NAME, SL_PRIV(q)));
	mp->b_rptr = b_rptr;	/* restore read pointer */
	return (err);
}

/*
 *  SL RECEIVE MESSAGE
 *  -----------------------------------
 *  9.6.6  All messages to another destination received at a signalling point
 *  whose MTP is restarting are discarded.
 */
STATIC int
sl_recv_msg(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	struct mtp_msg msg = { NULL, };
	struct lk *lk = sl->lk.lk;	/* the link (set) on which the message arrived */
	struct rs *adj = lk->sp.adj;	/* the route set to the adjacent of the arriving link */
	struct sp *loc = lk->sp.loc;	/* the local signalling point the message arrived for */
	struct na *na = loc->na.na;	/* the local network appearance, protocol and options */
	int err;
	if ((err = mtp_dec_msg(q, mp, &msg, na)))
		goto error;
	if (lk->ni != msg.ni)
		/* 
		   network indicator must match */
		goto screened;
	if (msg.opc == adj->dest)
		/* 
		   always listen to adjacent signalling point */
		goto passed;
	if (!(adj->flags & RSF_XFER_FUNC))
		/* 
		   originator not adjacent, adjacent must be STP */
		goto screened;
	if (!mtp_lookup_rt_local(sl, msg.opc, RT_TYPE_MEMBER)) {
		/* 
		   do not accept from originators to which we don't have a local route on the
		   receiving signalling link */
		fixme(("Must also check for circular routes\n"));
		goto screened;
	}
      passed:
	if (loc->pc == msg.dpc) {
		/* 
		   message is for us, process it */
		todo(("Also check local aliases\n"));
		return mtp_proc_msg(q, mp, &msg);
	}
	if ((loc->flags & SPF_XFER_FUNC)) {
		/* 
		   message is not for us, transfer it */
		return mtp_xfer_route(q, mp, &msg);
	}
	/* 
	   if we do not transfer, we cannot process messages not for us */
	goto screened;
      screened:
	todo(("Deliver screened message to MGMT\n"));
	return (-EPROTO);
      error:
	ptrace(("%s: %p: DECODE: decoding error %d\n", DRV_NAME, sl, err));
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from below
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA:
 *  -------------------------------------------------------------------
 *  When we receive a PDU from below, we want to decode it and then determine what to do based on the decoding of
 *  the message.
 */
STATIC int
sl_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	if ((1 << sl_get_l_state(sl)) & ~(SLSF_IN_SERVICE | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	if ((err = sl_recv_msg(q, mp)) == QR_ABSORBED)
		return (QR_ABSORBED);
	return (err);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
}

/*
 *  SL_PDU_IND:
 *  -------------------------------------------------------------------
 *  When we receive a PDU from below, we want to decode it and then determine what to do based on the decoding of
 *  the message.
 */
STATIC int
sl_pdu_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_pdu_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) || !mp->b_cont)
		goto einval;
	if ((1 << sl_get_l_state(sl)) & ~(SLSF_IN_SERVICE | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	if ((err = sl_recv_msg(q, mp->b_cont)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_LINK_CONGESTED_IND:
 *  -------------------------------------------------------------------
 *  We keep track of overall link congestion status primarily so that we can restrict routes when link congestion
 *  onsets.  This ensures that when a link set becomes congestion (premably from loss of links or general signalling
 *  overload) that we attempt to reroute what traffic we can to another available route.  Also, this is necessary
 *  for triggering congestion related transfer restricted procedure in an STP than sends TFR.
 */
STATIC int
sl_link_congested_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int old_status, new_status;
	const sl_link_cong_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << sl_get_l_state(sl)) & ~(SLSF_IN_SERVICE | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	old_status = sl->cong_status;
	sl->cong_status = p->sl_cong_status <= 3 ? p->sl_cong_status : 3;
	new_status = sl->cong_status;
	sl->disc_status = p->sl_disc_status <= 3 ? p->sl_disc_status : 3;
	if (!old_status && new_status) {
		sl_set_state(q, sl, SL_CONGESTED);
		/* 
		   T31(ANSI) is a false congesiton detection timer */
		sl_timer_start(sl, t31a);
	}
	if (!new_status && old_status) {
		sl_set_state(q, sl, SL_UNCONGESTED);
		/* 
		   T31(ANSI) is a false congesiton detection timer */
		sl_timer_stop(sl, t31a);
	}
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND:
 *  -------------------------------------------------------------------
 *  We keep track of overall link congestion status primarily so that we can unrestrict routes when link congestion
 *  abates.  This ensures that when a link set becomes congested (presumably from loss of links or general
 *  signalling overload) that we attempt to reroute what traffic we can to another available route.  Also, this is
 *  necessary for triggering congestion related transfer restricted procedure in an STP that sends TFR.
 */
STATIC int
sl_link_congestion_ceased_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int old_status, new_status;
	const sl_link_cong_ceased_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << sl_get_l_state(sl)) & ~(SLSF_IN_SERVICE | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	old_status = sl->cong_status;
	sl->cong_status = p->sl_cong_status <= 3 ? p->sl_cong_status : 3;
	new_status = sl->cong_status;
	sl->disc_status = p->sl_disc_status <= 3 ? p->sl_disc_status : 3;
	if (!new_status && old_status) {
		sl_set_state(q, sl, SL_UNCONGESTED);
		/* 
		   T31(ANSI) is a false congesiton detection timer */
		sl_timer_stop(sl, t31a);
	}
	if (!old_status && new_status) {
		sl_set_state(q, sl, SL_CONGESTED);
		/* 
		   T31(ANSI) is a false congesiton detection timer */
		sl_timer_start(sl, t31a);
	}
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND:
 *  -------------------------------------------------------------------
 *  As messages are retrieved, we simply tack the messages onto the end of the retrieval buffer.  If we are not in
 *  the retrieval state, then we discard the messages and complain.
 */
STATIC int
sl_retrieved_message_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_retrieved_msg_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) != SLS_WCON_RET)
		goto outstate;
	if (!mp->b_cont)
		goto efault;
	if (sl->lk.lk->ls.ls->lk.numb > 1) {
		/* 
		   When the SLS was rotated for transmission, we need to unrotate on retrieval. */
		switch (sl->lk.lk->ls.ls->sp.sp->na.na->option.pvar) {
		case SS7_PVAR_ANSI:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_CHIN:	/* ??? */
		{
			/* 
			   XXX Actually, for 5-bit and 7-bit compatibility, we rotate based on 5
			   sls bits regardless of the number of bits in the SLS */
			ulong sls = mp->b_cont->b_rptr[7];
			if (sls & 0x10)
				sls = (sls & 0xe0) | ((sls & 0x0f) << 1) | 0x01;
			else
				sls = (sls & 0xe0) | ((sls & 0x0f) << 1) | 0x00;
			/* 
			   rewrite sls in routing label */
			mp->b_cont->b_rptr[7] = sls;
		}
		}
	}
	bufq_queue(&sl->rbuf, mp->b_cont);
	mp->b_cont = NULL;
	return (QR_DONE);
      efault:
	ptrace(("%s: %p: ERROR: no data\n", DRV_NAME, sl));
	return (-EFAULT);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND:
 *  -------------------------------------------------------------------
 *  5.5 Retrieval and diversion of traffic
 *
 *  When the procedure to update the retransmission buffer content is completed, the following actions are
 *  performed: the routing of the signalling traffic to be diverted is changed; the signal traffic already stored in
 *  the transmission buffers and retransmission buffer of the unavailable signalling link is sent directly towards
 *  the new signalling link(s), according to the modified routing.
 *
 *  The diverted signalling traffic will be sent towards the new signalling link(s) in such a way that the correct
 *  message sequence is maintained.  The diverted traffic has no priority in relation to normal traffic already
 *  conveyed on the signalling link(s).
 */
STATIC int
sl_retrieval_complete_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_retrieval_comp_ind_t *p = (typeof(p)) mp->b_rptr;
	struct lk *lk = sl->lk.lk;
	struct ls *ls = lk->ls.ls;
	struct sp *sp = ls->sp.sp;
	struct na *na = sp->na.na;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) != SLS_WCON_RET)
		goto outstate;
	if (mp->b_cont) {
		if (ls->lk.numb > 1) {
			/* 
			   When the SLS was rotated for transmission, we need to unrotate on
			   retrieval. */
			switch (na->option.pvar) {
			case SS7_PVAR_ANSI:
			case SS7_PVAR_JTTC:
			case SS7_PVAR_CHIN:	/* ??? */
			{
				/* 
				   XXX Actually, for 5-bit and 7-bit compatibility, we rotate based 
				   on 5 sls bits regardless of the number of bits in the SLS */
				ulong sls = mp->b_cont->b_rptr[7];
				if (sls & 0x10)
					sls = (sls & 0xe0) | ((sls & 0x0f) << 1) | 0x01;
				else
					sls = (sls & 0xe0) | ((sls & 0x0f) << 1) | 0x00;
				/* 
				   rewrite sls in routing label */
				mp->b_cont->b_rptr[7] = sls;
			}
			}
		}
		bufq_queue(&sl->rbuf, mp->b_cont);
		mp->b_cont = NULL;
	}
	return sl_stop_restore(q, sl);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_RB_CLEARED_IND:
 *  -------------------------------------------------------------------
 */
STATIC int
sl_rb_cleared_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_rb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) == SLS_WIND_CLRB) {
		sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
		return (QR_DONE);
	}
	if ((1 << sl_get_l_state(sl)) &
	    ~(SLSF_WCON_RET | SLSF_WIND_INSI | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_BSNT_IND:
 *  -------------------------------------------------------------------
 */
STATIC int
sl_bsnt_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_bsnt_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;
	struct sp *loc = sl->lk.lk->sp.loc;
	struct rs *adj = sl->lk.lk->sp.adj;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) != SLS_WIND_BSNT)
		goto outstate;
	sl->bsnt = p->sl_bsnt;
	if (sl->flags & (SLF_COO_RECV | SLF_ECO_RECV)) {
		/* 
		   send COA */
		if ((err =
		     mtp_send_coa(q, loc, loc->ni, adj->dest, loc->pc, sl->slc, sl->slc, sl->bsnt)))
			return (err);
		if (sl->fsnc == -1UL) {
			if ((err = sl_clear_rtb_req(q, sl)))
				return (err);
		}
		if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
			return (err);
		return sl_retrieval_request_and_fsnc_req(q, sl, sl->fsnc);
	} else {
		/* 
		   send COO */
		if ((err =
		     mtp_send_coo(q, loc, loc->ni, adj->dest, loc->pc, sl->slc, sl->slc, sl->bsnt)))
			return (err);
		sl_set_l_state(sl, SLS_WACK_COO);
		return (QR_DONE);
	}
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_IN_SERVICE_IND:
 *  -------------------------------------------------------------------
 *  3.3.2 Signalling link restored
 *
 *  3.3.2.1 Signalling traffic management: the changeback procedure (see clause 6) is applied, if required, to
 *  divert signalling traffic from one or more links to a link which has become available; it includes determination
 *  of the traffic to be diverted and procedures for maintaining the correct message sequence.
 *
 *  3.3.2.2 Signalling link management: the signalling link deactivation procedure (see clause 12) is used if,
 *  during the signalling link failure, another signalling link of the same link was activated; it is used to assure
 *  that the link set status is returned to the same state as before the failure.  This requires that the active
 *  link activated during the link failure is deactivated and considered no longer available for signalling.
 *
 *  3.3.2.3 Signalling route management: in the case when the restoration of a signalling link causes a signalling
 *  route set to become available, the signalling transfer point which can once again route the concerned signalling
 *  traffic applies the transfer-allowed procedures described in clause 13.
 *
 *  4.4 Signalling link availablity
 *
 *  4.4.1 When a previously unavailable signalling link becomes available again (see 3.2), signalling traffic may be
 *  transferred to the available signalling link by means of the changeback procedure.  The traffic to be
 *  transferred is determined in accordance with the following criteria.
 *
 *  4.4.2 In the case when the link set to which the available signalling link belongs, already carries signalling
 *  traffic on other signalling links in the link set, the traffic to be transferred includes the traffic for which
 *  the available signalling link is the normal one.  Note that the assignment of the normal traffic to a signalling
 *  link may be changed during the changeback process taking into account, for example, system performance.
 *
 *  The normal traffic is transferred from one or more signalling links, depending on the criteria applied when the
 *  signalling link became unavailable (see 4.3.2), and upon the criteria applied if any of the alternative
 *  signalling links(s) themselves became unavailable, or available, in the meantime.
 *
 *  If signalling links in the linkset are still unavailable, and if it is required for load balancing purposes,
 *  signalling traffic extra to that normally carried by any link might also be identified for diversion to the
 *  signalling link made available, and to other available links in the linkset.
 *
 *  This extra traffic is transferred from one or more signalling links.
 *
 *  4.4.3  In the case when the link set (combined link set) to which the available signalling links belong, does
 *  not cary any signalling traffic [i.e., a link set (combined link set) has become available], the traffic to be
 *  trasferred is the traffic for which the available link set (combined link set) has higher priority than the link
 *  set (combined link set) currently used.
 *
 *  The traffic is transferred from one or more link sets (combined link sets) and from one or more signalling links
 *  within each link set.
 */
STATIC int
sl_in_service_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;
	struct rs *rs = lk->sp.adj;
	int i, err;
	const sl_in_service_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) != SLS_WIND_INSI)
		goto outstate;
	/* 
	   generate new test data */
	sl->tlen = jiffies & 0x0f;
	for (i = 0; i < sl->tlen; i++)
		sl->tdata[i] ^= ((jiffies >> 8) & 0xff);
	/* 
	   start new signalling link test */
	if ((err =
	     mtp_send_sltm(q, sp, lk->ni, rs->dest, sp->pc, sl->slc, sl->slc, sl->tdata, sl->tlen,
			   sl)))
		return (err);
	sl_timer_stop(sl, t17);
	sl_timer_start(sl, t1t);
	sl->flags &= ~SLF_WACK_SLTM2;
	sl->flags |= SLF_WACK_SLTM;
	sl_set_l_state(sl, SLS_WACK_SLTM);
	return (QR_DONE);
      outstate:
	/* 
	   FIXME: stop the link */
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_OUT_OF_SERVICE_IND:
 *  -------------------------------------------------------------------
 *  3.3.1 Signalling link failed
 *
 *  3.3.1.1 Signalling traffic management: the changeover procedure (see clause 5) is applied, if required, to diver
 *  signalling traffic from the unavailable link to one or more alternative links with the objective of avoiding
 *  message loss, repetition or mis-sequencing; it includes determination of the alternative link or links where the
 *  affected traffic can be transferred and procedures to retrieve messages sent over the failed link but not
 *  received by the far end.
 *
 *  3.3.1.2 Signalling link management: the procedures described in clause 12 are used to restore a signalling link
 *  and to make it available for signalling.  Moreover, depending on the link set status, the procedures can also be
 *  used to activate another signalling link in the same link set to which the unavailable link belongs and to make
 *  it available for signalling.
 *
 *  3.3.1.3 Signalling route management: in the case when the failure of a signalling link causes a signalling route
 *  set to become unavailable or restricted, the signalling transfer point which can no longer route the concerned
 *  signalling traffic applies the transfer-prohibited procedures or transfer-restricted procedures described in
 *  clause 13.
 *
 *  4.3 Signalling link unavailability
 *
 *  4.3.1 When a signalling link becomes unavailable, signalling traffic carried by the link is transferred to one
 *  ore more alternative links by means of a changeover procedure.  The alternative link or links are determined in
 *  accordance with the following criteria.
 *
 *  4.3.2 In thc case when there is one or more alternative signalling links available in the link set to which the
 *  unavailable link belongs, the signalling traffic is transferred within the link set to: a) an active and
 *  unblocked signalling link, currently not carrying any traffic.  If no such signalling link exists, the
 *  signalling traffic is transferred to; b) one ore possibly more than one signalling link currently carrying
 *  traffic.  In the case of transfer to one signalling link, the alternative signalling link is that having the
 *  highest priority of the signalling links in service.
 *
 *  4.3.3 In the case when there is no alternative signalling link within the link set to which the unavailable
 *  signalling link belongs, the signalling traffic is transferred to one or more alternative link sets (combined
 *  link sets) in accordance with the alternative routing defined for each destination.  For a particular
 *  destination, the alternative link set (combined link set) si the link set (combined link set) in service having
 *  the highest priority.
 *
 *  Within a new link set, signalling traffic is distributed over the signalling links in accordance with the
 *  routing currently applicable for that link set; i.e. the trasferred traffic is routed in the same way as the
 *  traffic already using the link set.
 *
 *  5.3 Changeover initiation and actions
 *
 *  5.3.1  Changeover is initiated at a signalling point when a signalling link is recognized as unavailable
 *  according to the criteria listed in 3.2.2.
 *
 *  The following actions are then performed: a) transmission and acceptance of message signal unites on the
 *  concerned signalling link is terminated; b) transmission of link status signal units or fill in signal units, as
 *  described in 5.3/Q.703 takes place; c) the alternative signalling link(s) are determined according to the rules
 *  specified in clause 4; d) a procedure to update the content of the retransmission buffer of the unavailable
 *  signalling link is performed as specified in 5.4 below; e) signalling traffic is diverted to the alternative
 *  signalling link(s) as specified in 5.5 below.
 *
 *  In addition, if traffic towards a given destination is diverted to an alternative signalling link terminating in
 *  a signalling transfer point not currently used to carry traffic towards that destination, a transfer-prohibited
 *  procedure is performed as specified in 13.2.
 *
 *  5.3.2 In the case when there is no traffic to transfer from the unavailable signalling link action, only item b)
 *  of 5.3.1 is required.
 *
 *  5.3.3 If no alternative signalling link exists for signalling traffic towards one or more destinations, the
 *  concerned destination(s) are declared inaccessible and the following actions apply: i) the routing of the
 *  concerned signalling traffic is blocked and the concerned messages already stored in the transmission and
 *  retransmission buffers of the unavailable signalling link, as well as those received subsequently, are
 *  discarded; ii) a command is sent to the User Part(s) (if any) in order to stop generating the concerned
 *  signalling traffic; iii) the transfer-prohibited procedure is performed, as specified in 13.2; iv) the
 *  appropriate signalling link management proceudres are performed, as specified in clause 12.
 *
 *  5.3.4  In some cases of failures or in some network configuration, the normal buffer updating and retrieval
 *  procedures described in 5.4 and 5.5 cannot be accomplisehd.  In such cases, the emergency changeover procedures
 *  described in 5.6 apply.
 *
 *  Other proceudres to cover possible abnormal cases appear in 5.7.
 *
 *  5.4 Buffer updating procedure
 *
 *  5.4.1  When a decision to changeover is made, a changeover order is sent to the remote signalling point.  In the
 *  case that the changeover was initiated by the reception of a changeover order (see 5.2), a changeover
 *  acknowledgement is sent instead.
 *
 *  A changeover order is always acknowledged by a changeover acknowledgement, even when changeover has already been
 *  initiated in accordance with another criterion.
 *
 *  No priority is given to the changeover order or changeover acknowledgement in relation to the normal traffic of
 *  the signalling link on which the message is sent.
 *
 *  5.4.2  The changeover order and changeover acknowledgement are signalling network management messages and
 *  contain the following information: the label, indicating the destination and originating signalling points and
 *  the identity of the unavailable signalling link; the changeover order (or changeover order acknowledgement)
 *  signal; and the forward sequence number of the last message signal unit accepted from the unavailable signalling
 *  link.
 *
 *  5.4.3  Upon reception of a changeover order or changeover acknowledgement, the retransmission buffer of the
 *  unavailable signalling link is updated (except as noted in 5.6), according to the information contained in the
 *  message.  The message signal units successive to that indicated by the message are those which have to be
 *  retransmitted on the alternative signalling link(s), according to the retrieval and diversion procedure.
 *
 *  12.2.1.2    When a decision is taken to activate an inactive signalling link; initiali alignment starts.  If the
 *  initial alignment procedure is successful, the signalling link is active and a signalling link test is started.
 *  If the signalling link test is successful the link becomes ready to convey signalling traffic.  In the case when
 *  initial alignment is not possible, as determined at Message Transfer Part level 2 (see clause 7/Q.703), new
 *  initial alignment procedures are started on the same signalling link after a timer T17 (delay to avoid the
 *  oscillation of initial alignment failure and link restart.  The value of T17 should be greater than the loop
 *  delay and less than timer T2, see 7.3/Q.703).  If the signalling link test fails, link restoration starts until
 *  the signalling link is activated or a manual intervention is made.
 *
 *  12.2.2  Signalling link restoration
 *
 *  After a signalling link failure is detected, signalling link intial alignment will take place.  In the case when
 *  the initial alignment procedure is successful, a signalling link test is started.  If the signalling link test
 *  is successful the link becomes restored and thus available for signalling.
 *
 *  If the initial alignmeent is not possible, as determined at Message Transfer Part level 2 (see clause 7/Q.703),
 *  new initial alignment procedures may be started on the same signalling link after a time T17 until the
 *  signalling link is restored ro a manual intervention is made, e.g. to replace the signalling data link or the
 *  signalling terminal.
 *
 *  If the signalling link test fails, the restoration procedure is repeated until the link is
 *
 *  ANSI T1.111.4 (1996 and 2000) 12.2.2.  After a signalling link failure is detected, signalling link initial
 *  alignment will take place.  When the initial alignment procedure is successful, a signalling link test is
 *  started.  If the signalling link test is successful, the signalling link is regarded as restored and thus
 *  available for siganlling.
 *
 *  If initial alignment is not possible, as determined at level 2 (see Section 7 of Chapter T1.111.3 or Section
 *  6.2.2 of T1.645), new initial alignment procedures may be started on the same signalling link after a timer T17
 *  until the signalling link is restored or a manual intervention is made, e.g., to replace the signalling data
 *  link or the signalling terminal.  If after time T19 the signalling link has not activated, a management function
 *  is notified, and, optionally, T19 may be restarted.  restored or a manual intervention is made.
 */
STATIC int
sl_out_of_service_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_out_of_service_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	switch (sl_get_l_state(sl)) {
	case SLS_WIND_INSI:
	case SLS_IN_SERVICE:
	case SLS_PROC_OUTG:
	case SLS_WACK_SLTM:
		return sl_stop_restore(q, sl);
	default:
		goto outstate;
	}
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND:
 *  -------------------------------------------------------------------
 *  3.3.5 Signalling link blocked
 *
 *  3.3.5.1 Signalling traffic management: as specified in 3.3.1.1.  (See SL_OUT_OF_SERVICE_IND).
 *
 *  As a natioanl option, local processor outage may also be applied to the affected signalling link before
 *  commencement of the appropriate signalling traffic management option.  On completion of that signalling traffic
 *  management action, local processor outage is removed from the affected signalling link.  No further signalling
 *  traffic management will be performed on that affected signalling link until a timer T24 (see 16.8) has expired
 *  or been cancelled, thus allowing time for indications from the remote end to stabilize as it carries out any
 *  signalling traffic management of its own.
 *
 *  3.3.5.2 Signalling route management: if the blocking of the link causes a signalling route set to become
 *  unavailable or restricted, the signalling transfer point which can no longer route the concerned signalling
 *  traffic applies the transfer-prohibited procedures described in clause 13.
 */
STATIC int
sl_remote_processor_outage_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_rem_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) != SLS_IN_SERVICE)
		goto outstate;
	if ((err = sl_set_state(q, sl, SL_BLOCKED)))
		return (err);
	sl_set_l_state(sl, SLS_PROC_OUTG);
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND:
 *  -------------------------------------------------------------------
 *  3.3.6 Signalling link unblocked
 *
 *  3.3.6.1 Signalling traffic management: the actions will be the same as in 3.3.2.1. (See SL_IN_SERIVCE_IND)
 *
 *  3.3.6.2 Signalling route management: if the link unblocked causes a signalling route set to become available,
 *  the signalling transfer point which can once again route the signalling traffic in that route set applies the
 *  transfer-allowed procedures described in clause 13.
 */
STATIC int
sl_remote_processor_recovered_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_rem_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) != SLS_PROC_OUTG)
		goto outstate;
	if ((err = sl_set_state(q, sl, SL_UNBLOCKED)))
		return (err);
	sl_set_l_state(sl, SLS_IN_SERVICE);
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_RTB_CLEARED_IND:
 *  -------------------------------------------------------------------
 *  We receive this indication when we send sl_clear_buffers_req in a state other than processor outage, or in
 *  response to a sl_clear_rtb_req after the remote processor has recovered.  For ITU, sl_clear_buffers_req will
 *  place us back to in service from the local processor outage state, but not for ANSI.  Effectively, it a
 *  processor outage has been of a long duration (i.e. T1 has expired) then we should flush buffers before resuming
 *  operation on the link.  If we are failing a remotely blocked link (again because of T1 expiry) in ANSI, we
 *  should fail the link and then flush buffers (which will result in only the RTB being flushed) before buffer
 *  updating.  Or, we could choose to flush buffers completely in the processor outage state.
 */
STATIC int
sl_rtb_cleared_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_rtb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	switch (sl_get_l_state(sl)) {
	case SLS_WIND_CLRB:
		sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
		return (QR_DONE);
	case SLS_WCON_RET:
		/* 
		   we oft times blindly clear RTB before retrieval */
		return (QR_DONE);
	default:
		goto outstate;
	}
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND:
 *  -------------------------------------------------------------------
 *  5.5 Retrieval and diversion of traffic
 *
 *  When the procedure to update the retransmission buffer content is completed, the following actions are
 *  performed: the routing of the signalling traffic to be diverted is changed; the signal traffic already stored in
 *  the transmission buffers and retransmission buffer of the unavailable signalling link is sent directly towards
 *  the new signalling link(s), according to the modified routing.
 *
 *  The diverted signalling traffic will be sent towards the new signalling link(s) in such a way that the correct
 *  message sequence is maintained.  The diverted traffic has no priority in relation to normal traffic already
 *  conveyed on the signalling link(s).
 *
 *  NOTE: we actually never send this.  Retrieval is always possible.  Where it might not be possible is in the case
 *  where M2UA is used and it is not possible to talk to the signalling link.   We treat this the same as a
 *  SL_RETRIEVAL_COMPLETE_IND, we just may have an empty retrieval buffer.
 */
STATIC int
sl_retrieval_not_possible_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_retrieval_not_poss_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) != SLS_WCON_RET)
		goto outstate;
	/* 
	   purge any partial retrieval buffer */
	bufq_purge(&sl->rbuf);
	return sl_stop_restore(q, sl);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND:
 *  -------------------------------------------------------------------
 */
STATIC int
sl_bsnt_not_retrievable_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_bsnt_not_retr_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;
	struct sp *loc = sl->lk.lk->sp.loc;
	struct rs *adj = sl->lk.lk->sp.adj;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) != SLS_WIND_BSNT)
		goto outstate;
	sl->bsnt = -1UL;
	if (sl->flags & (SLF_COO_RECV | SLF_ECO_RECV)) {
		/* 
		   send ECA */
		if ((err = mtp_send_eca(q, loc, loc->ni, adj->dest, loc->pc, sl->slc, sl->slc)))
			return (err);
		if ((sl->flags & SLF_ECO_RECV) || sl->fsnc == -1UL) {
			/* 
			   If we were not given the FSNC from the other end, because of ECO, then
			   we clear the retransmission buffer before retrieval.  We can still
			   retrieve the contents of the transmission buffer. */
			if ((err = sl_clear_rtb_req(q, sl)))
				return (err);
			sl->fsnc = -1UL;
		}
		if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
			return (err);
		return sl_retrieval_request_and_fsnc_req(q, sl, sl->fsnc);
	} else {
		/* 
		   send ECO */
		if ((err = mtp_send_eco(q, loc, loc->ni, adj->dest, loc->pc, sl->slc, sl->slc)))
			return (err);
		sl_set_l_state(sl, SLS_WACK_ECO);
		return (QR_DONE);
	}
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

#if 0
/*
 *  SL_OPTMGMT_ACK:
 *  -------------------------------------------------------------------
 */
STATIC int
sl_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
	return (-EOPNOTSUPP);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  SL_NOTIFY_IND:
 *  -------------------------------------------------------------------
 */
STATIC int
sl_notify_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_notify_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
	return (-EOPNOTSUPP);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}
#endif
/*
 *  LMI_INFO_ACK:
 *  -------------------------------------------------------------------
 */
STATIC int
lmi_info_ack(queue_t *q, mblk_t *mp)
{
	const lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, SL_PRIV(q)));
	return (-EINVAL);
}

/*
 *  LMI_OK_ACK:
 *  -------------------------------------------------------------------
 */
STATIC int
lmi_ok_ack(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const lmi_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	switch (sl_get_i_state(sl)) {
	case LMI_ATTACH_PENDING:
		sl_set_i_state(sl, LMI_DISABLED);
		break;
	case LMI_DETACH_PENDING:
		sl_set_i_state(sl, LMI_UNATTACHED);
		break;
	default:
		goto outstate;
	}
	fixme(("Write this function\n"));
	return (-EFAULT);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  LMI_ERROR_ACK:
 *  -------------------------------------------------------------------
 */
STATIC int
lmi_error_ack(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	switch (sl_get_i_state(sl)) {
	case LMI_ATTACH_PENDING:
		sl_set_i_state(sl, LMI_UNATTACHED);
		break;
	case LMI_DETACH_PENDING:
	case LMI_ENABLE_PENDING:
		sl_set_i_state(sl, LMI_DISABLED);
		break;
	case LMI_DISABLE_PENDING:
		sl_set_i_state(sl, LMI_ENABLED);
		break;
	default:
		goto outstate;
	}
	fixme(("Write this function\n"));
	return (-EFAULT);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  LMI_ENABLE_CON:
 *  -------------------------------------------------------------------
 */
STATIC int
lmi_enable_con(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_i_state(sl) != LMI_ENABLE_PENDING)
		goto outstate;
	fixme(("Write this function\n"));
	return (-EFAULT);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  LMI_DISABLE_CON:
 *  -------------------------------------------------------------------
 */
STATIC int
lmi_disable_con(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_i_state(sl) != LMI_DISABLE_PENDING)
		goto outstate;
	fixme(("Write this function\n"));
	return (-EFAULT);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, sl));
	return (-EPROTO);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, sl));
	return (-EINVAL);
}

/*
 *  LMI_OPTMGMT_ACK:
 *  -------------------------------------------------------------------
 */
STATIC int
lmi_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	const lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, SL_PRIV(q)));
	return (-EINVAL);
}

/*
 *  LMI_ERROR_IND:
 *  -------------------------------------------------------------------
 */
STATIC int
lmi_error_ind(queue_t *q, mblk_t *mp)
{
	const lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, SL_PRIV(q)));
	return (-EINVAL);
}

/*
 *  LMI_STATS_IND:
 *  -------------------------------------------------------------------
 */
STATIC int
lmi_stats_ind(queue_t *q, mblk_t *mp)
{
	const lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, SL_PRIV(q)));
	return (-EINVAL);
}

/*
 *  LMI_EVENT_IND:
 *  -------------------------------------------------------------------
 */
STATIC int
lmi_event_ind(queue_t *q, mblk_t *mp)
{
	const lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, SL_PRIV(q)));
	return (-EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from above
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -------------------------------------------------------------------
 */
STATIC int
m_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int dlen = msgdsize(mp);
	if (mtp->prot->SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == MTPS_IDLE)
		goto discard;
	if (mtp_get_state(mtp) != MTPS_CONNECTED)
		goto outstate;
	if (dlen == 0 || dlen > mtp->prot->TSDU_size || dlen > mtp->prot->TIDU_size)
		goto baddata;
	return mtp_send_msg(q, mtp, NULL, &mtp->dst, mp);
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", DRV_NAME, mtp, dlen));
	goto error;
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      discard:
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, mtp));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: %p: ERROR: primitive not supported for T_CLTS\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  MTP_BIND_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
m_bind_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct MTP_bind_req *p = (typeof(p)) mp->b_rptr;
	if (mtp_get_state(mtp) != MTPS_UNBND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
		goto badprim;
	if (!p->mtp_addr_length)
		goto noaddr;
	if (p->mtp_addr_length < sizeof(struct mtp_addr))
		goto badaddr;
	{
		struct mtp_addr *src = (typeof(src)) (mp->b_rptr + p->mtp_addr_offset);
		struct sp *loc;
		if (src->family != AF_MTP)
			goto badaddr;
		if (!src->si || !src->pc)
			goto noaddr;
		if (src->si < 3 && mtp->cred.cr_uid != 0)
			goto access;
		if (!(loc = mtp_check_src(mtp, src, &err)))
			goto error;
		if (!mtp->sp.loc)
			mtp->sp.loc = sp_get(loc);
		return m_bind_ack(q, mtp, src);
	}
      access:
	err = MACCESS;
	ptrace(("%s: %p: ERROR: no permission for requested address\n", DRV_NAME, mtp));
	goto error;
      badaddr:
	err = MBADADDR;
	ptrace(("%s: %p: ERROR: address is invalid\n", DRV_NAME, mtp));
	goto error;
      noaddr:
	err = MNOADDR;
	ptrace(("%s: %p: ERROR: could not allocate address\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = MBADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = MOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error_ack(q, mtp, p->mtp_primitive, err);
}

/*
 *  MTP_UNBIND_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
m_unbind_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mtp_get_state(mtp) != MTPS_IDLE)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	mtp_set_state(mtp, MTPS_WACK_UREQ);
	return m_ok_ack(q, mtp, p->mtp_primitive);
      badprim:
	err = MBADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = MOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error_ack(q, mtp, p->mtp_primitive, err);
}

/*
 *  MTP_CONN_REQ:
 *  -------------------------------------------------------------------
 *  MTP is really a connectionless protocol.  When we form a connection we simply remember the destination address.
 *  Some interim MTPs had the ability to send a UPT (User Part Test) message.  If the protocol variant has this
 *  ability, we wait for the result of the User Part Test before confirming the connection.
 */
STATIC int
m_conn_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct MTP_conn_req *p = (typeof(p)) mp->b_rptr;
	switch (mtp_get_state(mtp)) {
	default:
		goto outstate;
	case MTPS_IDLE:
		if (mtp->sp.loc->na.na->prot[mtp->src.si]->SERV_type == T_CLTS)
			goto notsupp;
		if (mp->b_wptr < mp->b_rptr + sizeof(*p))
			goto badprim;
		if (mp->b_wptr < mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
			goto badprim;
		if (!p->mtp_addr_length)
			goto noaddr;
		if (p->mtp_addr_length < sizeof(struct mtp_addr))
			goto badaddr;
		{
			struct mtp_addr *dst = (typeof(dst)) (mp->b_rptr + p->mtp_addr_offset);
			if (dst->family != AF_MTP)
				goto badaddr;
			if (dst->si == 0 && mtp->src.si == 0)
				goto noaddr;
			if (dst->si < 3 && mtp->cred.cr_uid != 0)
				goto access;
			if (dst->si != mtp->src.si)
				goto badaddr;
			if (!mtp_check_dst(mtp, dst))
				goto badaddr;
			mtp->dst = *dst;
		}
		mtp_set_state(mtp, MTPS_WACK_CREQ);
		/* 
		   fall through */
	case MTPS_WACK_CREQ:
		/* 
		 *  There is another thing to do once the connection has been established: that is to deliver MTP
		 *  restart begins indication or to deliver MTP resume or MTP pause indications for the peer
		 *  depending on the peer's state.
		 */
		switch (mtp->sp.rem->state) {
		case RS_RESTART:
		case RS_PROHIBITED:
		case RS_BLOCKED:
			if ((err = mtp_pause_ind(q, mtp, mtp->sp.rem)))
				goto error;
			break;
		case RS_ALLOWED:
			if ((err = mtp_resume_ind(q, mtp, mtp->sp.rem)))
				goto error;
			break;
		case RS_CONGESTED:
			if ((err = mtp_resume_ind(q, mtp, mtp->sp.rem)))
				goto error;
			if (mtp->sp.rem->cong_status)
				if ((err =
				     mtp_cong_status_ind(q, mtp, mtp->sp.rem,
							 mtp->sp.rem->cong_status)))
					goto error;
			break;
		default:
			swerr();
			break;
		}
		return m_ok_ack(q, mtp, p->mtp_primitive);
	}
      access:
	err = MACCESS;
	ptrace(("%s: %p: ERROR: no permission for requested address\n", DRV_NAME, mtp));
	goto error;
      badaddr:
	err = MBADADDR;
	ptrace(("%s: %p: ERROR: address is invalid\n", DRV_NAME, mtp));
	goto error;
      noaddr:
	err = MNOADDR;
	ptrace(("%s: %p: ERROR: could not allocate address\n", DRV_NAME, mtp));
	goto error;
      notsupp:
	err = MNOTSUPP;
	ptrace(("%s: %p: ERROR: primitive not supported\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = MBADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = MOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error_ack(q, mtp, p->mtp_primitive, err);
}

/*
 *  MTP_DISCON_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
m_discon_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_discon_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mtp->sp.loc->na.na->prot[mtp->src.si]->SERV_type == T_CLTS)
		goto notsupp;
	switch (mtp_get_state(mtp)) {
	case MTPS_WCON_CREQ:
		if (mp->b_wptr < mp->b_rptr + sizeof(*p))
			goto badprim;
		mtp_set_state(mtp, MTPS_WACK_DREQ6);
		break;
	case MTPS_CONNECTED:
		if (mp->b_wptr < mp->b_rptr + sizeof(*p))
			goto badprim;
		mtp_set_state(mtp, MTPS_WACK_DREQ9);
		break;
	default:
		goto outstate;
	}
	/* 
	   change state and let m_ok_ack do all the work */
	return m_ok_ack(q, mtp, p->mtp_primitive);
      notsupp:
	err = MNOTSUPP;
	ptrace(("%s: %p: ERROR: primitive not supported\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = MBADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = MOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error_ack(q, mtp, p->mtp_primitive, err);
}

/*
 *  MTP_ADDR_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
m_addr_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_addr_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (mtp_get_state(mtp)) {
	case MTPS_UNBND:
		return m_addr_ack(q, mtp, NULL, NULL);
	case MTPS_IDLE:
		return m_addr_ack(q, mtp, &mtp->src, NULL);
	case MTPS_WCON_CREQ:
	case MTPS_CONNECTED:
	case MTPS_WIND_ORDREL:
	case MTPS_WREQ_ORDREL:
		return m_addr_ack(q, mtp, &mtp->src, &mtp->dst);
	case MTPS_WRES_CIND:
		return m_addr_ack(q, mtp, NULL, &mtp->dst);
	}
	goto outstate;
      outstate:
	err = MOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = MBADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error_ack(q, mtp, p->mtp_primitive, err);
}

/*
 *  MTP_INFO_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
m_info_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_info_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return m_info_ack(q, mtp);
      badprim:
	err = MBADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error_ack(q, mtp, p->mtp_primitive, err);
}

/*
 *  MTP_OPTMGMT_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
m_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct mtp_opts opts;
	int err;
	if (mtp_get_state(mtp) == MTPS_IDLE)
		mtp_set_state(mtp, MTPS_WACK_OPTREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->mtp_opt_offset + p->mtp_opt_length)
		goto badprim;
	if (mtp_parse_opts(mtp, &opts, mp->b_rptr + p->mtp_opt_offset, p->mtp_opt_length))
		goto badopt;
	switch (p->mtp_mgmt_flags) {
	case MTP_CHECK:
		if ((err = mtp_opt_check(mtp, &opts)))
			goto error;
		break;
	case MTP_NEGOTIATE:
		if (!opts.flags)
			mtp_opt_default(mtp, &opts);
		else if ((err = mtp_opt_check(mtp, &opts)))
			goto error;
		if ((err = mtp_opt_negotiate(mtp, &opts)))
			goto error;
		break;
	case MTP_DEFAULT:
		if ((err = mtp_opt_default(mtp, &opts)))
			goto error;
		break;
	case MTP_CURRENT:
		if ((err = mtp_opt_current(mtp, &opts)))
			goto error;
		break;
	default:
		goto badflag;
	}
	return m_optmgmt_ack(q, mtp, &opts, p->mtp_mgmt_flags);
      badprim:
	err = MBADPRIM;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      badflag:
	err = MBADFLAG;
	ptrace(("%s: %p: ERROR: invalid flags\n", DRV_NAME, mtp));
	goto error;
      badopt:
	err = MBADOPT;
	ptrace(("%s: %p: ERROR: invalid options\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error_ack(q, mtp, p->mtp_primitive, err);
}

/*
 *  MTP_TRANSFER_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
m_transfer_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_transfer_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgsize(mp->b_cont) : 0;
	int err;
	if (mtp->prot->SERV_type == T_CLTS)
		goto notsupp;
	if (mtp_get_state(mtp) == MTPS_IDLE)
		goto discard;
	if (mtp_get_state(mtp) != MTPS_CONNECTED)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (dlen == 0 || dlen > mtp->prot->TSDU_size || dlen > mtp->prot->TIDU_size)
		goto baddata;
	if ((err = mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont)) == QR_ABSORBED)
		return (QR_TRIMMED);
	if (err < 0)
		goto error;
	return (err);
      baddata:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: amount of data is invalid\n", DRV_NAME, mtp));
	goto error;
      notsupp:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: primitive not supported\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      discard:
	rare();
	return (QR_DONE);
      error:
	return m_error(q, mtp, err);
}

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
t_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int dlen = msgdsize(mp);
	int err;
	if (mtp->prot->SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (dlen == 0 || dlen > mtp->prot->TSDU_size || dlen > mtp->prot->TIDU_size)
		goto baddata;
	if ((err = mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont)) == QR_ABSORBED)
		return (QR_ABSORBED);
	return (err);
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", DRV_NAME, mtp, dlen));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      discard:
	ptrace(("%s: ERROR: ignore in idle state\n", DRV_NAME));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  T_CONN_REQ
 *  -----------------------------------
 *  As MTP is really a connectionless protocol, when we form a connection we simply remember the destination address.
 *  Some interim MTPs had the abilitty to send a UPT (User Part Test) message.  If the protocol variant has this
 *  ability, we wait for the result of the User Part Test before confirming the connection.
 */
STATIC int
t_conn_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err = -EFAULT;
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	if (mtp->prot->SERV_type == T_CLTS)
		goto notsupport;
	if (mlen < sizeof(*p))
		goto einval;
	if (mlen < p->DEST_offset + p->DEST_length)
		goto einval;
	if (mlen < p->OPT_offset + p->OPT_length)
		goto einval;
	{
		struct mtp_addr *dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		if (dst->family != AF_MTP)
			goto badaddr;
		if (dst->si == 0 && mtp->src.si == 0)
			goto noaddr;
		if (dst->si < 3 && mtp->cred.cr_uid != 0)
			goto acces;
		if (dst->si != mtp->src.si)
			goto badaddr;
		if (!mtp_check_dst(mtp, dst))
			goto badaddr;
		{
			unsigned char *opt = mp->b_rptr + p->OPT_offset;
			struct mtp_opts opts = { 0L, NULL, };
			if (mtp_parse_opts(mtp, &opts, opt, p->OPT_length))
				goto badopt;
			/* 
			   TODO: set options first */
			if (mp->b_cont) {
				putbq(q, mp->b_cont);	/* hold back data */
				mp->b_cont = NULL;	/* abosrbed mp->b_cont */
			}
			mtp->dst = *dst;
			mtp_set_state(mtp, TS_WACK_CREQ);
			{
				struct sp *sp = mtp->sp.loc;
				if ((err =
				     mtp_send_upt(q, sp, sp->ni, mtp->dst.pc, mtp->src.pc, 0,
						  mtp->dst.pc, mtp->dst.si)) < 0)
					goto error;
			}
			if ((err = t_ok_ack(q, mtp, T_CONN_REQ)) < 0)
				goto error;
			if ((err = t_conn_con(q, mtp, dst, NULL, NULL)) < 0)
				goto error;
			return (QR_DONE);
		}
	}
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options\n", DRV_NAME, mtp));
	goto error;
      acces:
	err = TACCES;
	ptrace(("%s: %p: ERROR: no permission for address\n", DRV_NAME, mtp));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: address is unusable\n", DRV_NAME, mtp));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: couldn't allocate address\n", DRV_NAME, mtp));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid message format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for T_CLTS\n", DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_DISCON_REQ         2 - TC disconnection request
 *  -------------------------------------------------------------------
 */
STATIC int
t_discon_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	if (mtp->prot->SERV_type == T_CLTS)
		goto notsupport;
	switch (mtp_get_state(mtp)) {
	case TS_WCON_CREQ:
		if (mp->b_wptr < mp->b_rptr + sizeof(*p))
			goto einval;
		mtp_set_state(mtp, TS_WACK_DREQ6);
		break;
	case TS_DATA_XFER:
		if (mp->b_wptr < mp->b_rptr + sizeof(*p))
			goto einval;
		mtp_set_state(mtp, TS_WACK_DREQ9);
		break;
	default:
		goto outstate;
	}
	/* 
	   change state and let t_ok_ack do all the work */
	return t_ok_ack(q, mtp, T_DISCON_REQ);
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------
 */
STATIC int
t_data_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct T_data_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	if (mtp->prot->SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (dlen == 0 || dlen > mtp->prot->TSDU_size || dlen > mtp->prot->TIDU_size)
		goto baddata;
	if ((err = mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", DRV_NAME, mtp, dlen));
	goto error;
      outstate:
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      einval:
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      discard:
	ptrace(("%s: ERROR: ignore in idle state\n", DRV_NAME));
	return (QR_DONE);
      notsupport:
	ptrace(("%s: ERROR: primitive not supported for T_CLTS\n", DRV_NAME));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------
 */
STATIC int
t_exdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	(void) mp;
	return m_error(q, mtp, -EPROTO);
}

/*
 *  T_INFO_REQ           5 - Information Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_info_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	(void) mp;
	return t_info_ack(q, mtp);
}

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -------------------------------------------------------------------
 */
STATIC int
t_bind_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	if (mtp_get_state != TS_UNBND)
		goto outstate;
	mtp_set_state(mtp, TS_WACK_BREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto einval;
	if (p->CONIND_number)
		goto notsupport;
	if (!p->ADDR_length)
		goto noaddr;
	if (p->ADDR_length < sizeof(struct mtp_addr))
		goto badaddr;
	{
		struct mtp_addr *src = (typeof(src)) (mp->b_rptr + p->ADDR_offset);
		struct sp *loc;
		/* 
		   we don't allow wildcards yet. */
		if (src->family != AF_MTP)
			goto badaddr;
		if (!src->si || !src->pc)
			goto noaddr;
		if (src->si < 3 && mtp->cred.cr_uid != 0)
			goto acces;
		if ((loc = mtp_check_src(mtp, src, &err)))
			goto error;
		if (!mtp->sp.loc)
			mtp->sp.loc = sp_get(loc);
		return t_bind_ack(q, mtp, src);
	}
      acces:
	err = TACCES;
	ptrace(("%s: %p: ERROR: no permission for address\n", DRV_NAME, mtp));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: couldn't allocate address\n", DRV_NAME, mtp));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: address is invalid\n", DRV_NAME, mtp));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not support for T_CLTS\n", DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -------------------------------------------------------------------
 */
STATIC int
t_unbind_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	mtp_set_state(mtp, TS_WACK_UREQ);
	return t_ok_ack(q, mtp, T_UNBIND_REQ);
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_UNITDATA_REQ       8 -Unitdata Request 
 *  -------------------------------------------------------------------
 */
STATIC int
t_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;
	if (mtp->prot->SERV_type != T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	if (dlen == 0 && !(mtp->prot->PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (dlen > mtp->prot->TSDU_size || dlen > mtp->prot->TIDU_size)
		goto baddata;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	else {
		struct mtp_addr *dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		if (dst->si < 3 && mtp->cred.cr_uid != 0)
			goto acces;
		if (!mtp_check_dst(mtp, dst))
			goto badaddr;
		else {
			struct mtp_opts opts = { 0L, NULL, };
			if (mtp_parse_opts(mtp, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
				goto badopt;
			if ((err = mtp_send_msg(q, mtp, &opts, dst, mp->b_cont)) == QR_ABSORBED)
				return (QR_TRIMMED);
			return (err);
		}
	}
      badopt:
	ptrace(("%s: %p: ERROR: bad options\n", DRV_NAME, mtp));
	goto error;
      acces:
	ptrace(("%s: %p: ERROR: no permission to address\n", DRV_NAME, mtp));
	goto error;
      badaddr:
	ptrace(("%s: %p: ERROR: bad destination address\n", DRV_NAME, mtp));
	goto error;
      einval:
	ptrace(("%s: %p: ERROR: invalid parameter\n", DRV_NAME, mtp));
	goto error;
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", DRV_NAME, mtp, dlen));
	goto error;
      notsupport:
	ptrace(("%s: %p: ERROR: primitive not supported for T_COTS or T_COTS_ORD\n", DRV_NAME,
		mtp));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------
 */
STATIC int
t_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err = 0;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
#ifdef TS_WACK_OPTREQ
	if (mtp_get_state(mtp) == TS_IDLE)
		mtp_set_state(mtp, TS_WACK_OPTREQ);
#endif
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	{
		struct mtp_opts opts = { 0L, NULL, };
		if (mtp_parse_opts(mtp, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
		switch (p->MGMT_flags) {
		case T_CHECK:
			err = mtp_opt_check(mtp, &opts);
			break;
		case T_NEGOTIATE:
			if (!opts.flags)
				mtp_opt_default(mtp, &opts);
			else if ((err = mtp_opt_check(mtp, &opts)))
				break;
			err = mtp_opt_negotiate(mtp, &opts);
			break;
		case T_DEFAULT:
			err = mtp_opt_default(mtp, &opts);
			break;
		case T_CURRENT:
			err = mtp_opt_current(mtp, &opts);
			break;
		default:
			goto badflag;
		}
		if (err)
			goto provspec;
		return t_optmgmt_ack(q, mtp, p->MGMT_flags, &opts);
	}
      provspec:
	err = err;
	ptrace(("%s: %p: ERROR: provider specific\n", DRV_NAME, mtp));
	goto error;
      badflag:
	err = TBADFLAG;
	ptrace(("%s: %p: ERROR: bad options flags\n", DRV_NAME, mtp));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options\n", DRV_NAME, mtp));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -------------------------------------------------------------------
 */
STATIC int
t_ordrel_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	if (mtp->prot->SERV_type != T_COTS_ORD)
		goto notsupport;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	switch (mtp_get_state(mtp)) {
	case TS_DATA_XFER:
		mtp_set_state(mtp, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		goto error;
	}
	return t_ordrel_ind(q, mtp);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      notsupport:
	ptrace(("%s: %p: ERROR: primitive not supported for T_CLTS or T_COTS\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error(q, mtp, EPROTO);
}

/*
 *  T_OPTDATA_REQ       24 - Data with options request
 *  -------------------------------------------------------------------
 */
STATIC int
t_optdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	if (mtp->prot->SERV_type == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (p->DATA_flag & T_ODF_EX || p->DATA_flag & T_ODF_MORE)
		goto notsupport;
	else {
		struct mtp_opts opts = { 0L, NULL, };
		if (mtp_parse_opts(mtp, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
		if ((err = mtp_send_msg(q, mtp, &opts, &mtp->dst, mp->b_cont)) == QR_ABSORBED)
			return (QR_TRIMMED);
		return (err);
	}
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	return m_error(q, mtp, EPROTO);
      discard:
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, mtp));
	return (QR_DONE);
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for T_CLTS\n", DRV_NAME, mtp));
	goto error;
      error:
	return t_error_ack(q, mtp, p->PRIM_type, err);
}

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ          25 - Address Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_addr_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct T_addr_req *p = (typeof(p)) mp->b_rptr;
	(void) mp;
	switch (mtp_get_state(mtp)) {
	case TS_UNBND:
		return t_addr_ack(q, mtp, NULL, NULL);
	case TS_IDLE:
		return t_addr_ack(q, mtp, &mtp->src, NULL);
	case TS_WCON_CREQ:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		return t_addr_ack(q, mtp, &mtp->src, &mtp->dst);
	case TS_WRES_CIND:
		return t_addr_ack(q, mtp, NULL, &mtp->dst);
	}
	return t_error_ack(q, mtp, p->PRIM_type, TOUTSTATE);
}
#endif

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ    ?? - Capability Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_capability_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct T_capability_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	return t_capability_ack(q, mtp, p->CAP_bits1);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	return t_error_ack(q, mtp, p->PRIM_type, -EINVAL);
}
#endif

/*
 *  M_DATA
 *  -------------------------------------------------------------------
 */
STATIC int
n_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const size_t dlen = msgdsize(mp);
	if (!mtp->prot || mtp->prot->SERV_type == T_CLTS)
		goto notsupp;
	if (dlen == 0 || dlen > mtp->prot->TSDU_size || dlen > mtp->prot->TIDU_size)
		goto baddata;
	switch (mtp_get_state(mtp)) {
	case NS_DATA_XFER:
		return mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont);
	case NS_IDLE:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	/* 
	   If we are in the idle state this is just spurious data, ignore it */
	rare();
	return (QR_DONE);
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported\n", DRV_NAME, mtp));
	goto error;
      baddata:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: bad data\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  N_CONN_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_conn_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_conn_req_t *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr *dst;
	// unsigned char *qos;
	struct mtp_opts opts;
	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	if (mtp->sp.loc->na.na->prot[mtp->src.si]->SERV_type == T_CLTS)
		goto notsupp;
	if (mp->b_wptr < mp->b_rptr)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
		goto badprim;
	dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);
	if (!p->DEST_length)
		goto noaddr;
	if (p->DEST_length < sizeof(*dst))
		goto badaddr;
	if (dst->family != AF_MTP)
		goto badaddr;
	if (dst->si == 0 && mtp->src.si == 0)
		goto noaddr;
	if (dst->si < 3 && mtp->src.si != 0)
		goto badaddr;
	if (!mtp_check_dst(mtp, dst))
		goto badaddr;
	if (mtp_parse_qos(mtp, &opts, mp->b_rptr + p->QOS_offset, p->QOS_length))
		goto badqostype;
	/* 
	   TODO: set options first */
	mtp->dst = *dst;
	mtp_set_state(mtp, NS_WCON_CREQ);
	return n_conn_con(q, mtp, 0, &mtp->dst, NULL);
      badqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: ERROR: bad qos type\n", DRV_NAME, mtp));
	goto error;
#if 0
      badqosparam:
	err = NBADQOSPARAM;
	ptrace(("%s: %p: ERROR: bad qos parameter\n", DRV_NAME, mtp));
	goto error;
#endif
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: bad destination address\n", DRV_NAME, mtp));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: couldn't allocate destination address\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported for N_CLNS\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  N_CONN_RES:
 *  -------------------------------------------------------------------
 */
STATIC int
n_conn_res(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_conn_res_t *p = (typeof(p)) mp->b_rptr;
	if (mtp->sp.loc->na.na->prot[mtp->src.si]->SERV_type == T_CLTS)
		goto notsupp;
	if (mtp_get_state(mtp) != NS_WRES_CIND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* 
	   We never give an N_CONN_IND, so there is no reason for a N_CONN_RES.  We probably could
	   do this * (issue an N_CONN_IND on a listening stream when there is no other MTP user for 
	   the SI value and * send a UPU on an N_DISCON_REQ or just redirect all traffic for that
	   user on a N_CONN_RES) but * that is for later. */
	goto eopnotsupp;
      eopnotsupp:
	err = -EOPNOTSUPP;
	ptrace(("%s: %p: ERROR: operation not supported\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported\n", DRV_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  N_DISCON_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_discon_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_discon_req_t *p = (typeof(p)) mp->b_rptr;
	if (mtp->sp.loc->na.na->prot[mtp->src.si]->SERV_type == T_CLTS)
		goto notsupp;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* 
	   Currently there are only three states we can disconnect from.  The first does not
	   happen. Only the second one is normal.  The third should occur during simulteneous
	   diconnect only. */
	switch (mtp_get_state(mtp)) {
	case NS_WCON_CREQ:
		mtp_set_state(mtp, NS_WACK_DREQ6);
		break;
	case NS_DATA_XFER:
		mtp_set_state(mtp, NS_WACK_DREQ9);
		break;
	case NS_IDLE:
		rare();
		break;
	default:
		goto outstate;
	}
	return n_ok_ack(q, mtp, p->PRIM_type);
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported\n", DRV_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  N_DATA_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_data_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_data_req_t *p = (typeof(p)) mp->b_rptr;
	const size_t dlen = msgdsize(mp);
	const struct T_info_ack *i = mtp->sp.loc->na.na->prot[mtp->src.si];
	if (i->SERV_type == T_CLTS)
		goto notsupp;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->DATA_xfer_flags)
		/* 
		   N_MORE_DATA_FLAG and N_RC_FLAG not supported yet.  We could do N_MORE_DATA_FLAG
		   pretty easily by accumulating the packet until the last data request is
		   received, but this would be rather pointless for small MTP packet sizes.
		   N_RC_FLAG cannot be supported until the DLPI link driver is done and zero-loss
		   operation is completed. */
		goto notsupp;
	if (dlen == 0 || dlen > i->TSDU_size || dlen > i->TIDU_size)
		goto baddata;
	switch (mtp_get_state(mtp)) {
	case NS_DATA_XFER:
		if ((err = mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont) == QR_ABSORBED))
			return (QR_TRIMMED);
		if (err < 0)
			goto error;
		return (err);
	case NS_IDLE:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	/* 
	   If we are in the idle state this is just spurious data, ignore it */
	rare();
	return (QR_DONE);
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not supported\n", DRV_NAME, mtp));
	goto error;
      baddata:
	err = -EPROTO;
	ptrace(("%s: %p: ERROR: bad data\n", DRV_NAME, mtp));
	goto error;
      error:
	return m_error(q, mtp, -EPROTO);
}

/*
 *  N_EXDATA_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_exdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	(void) mp;
	return m_error(q, mtp, -EPROTO);
}

/*
 *  N_INFO_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_info_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	(void) mp;
	return n_info_ack(q, mtp);
}

/*
 *  N_BIND_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_bind_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_bind_req_t *p = (typeof(p)) mp->b_rptr;
	struct mtp_addr src;
	struct sp *loc;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length)
		goto badprim;
	if (mtp_get_state(mtp) != NS_UNBND)
		goto outstate;
	if (p->PROTOID_length)
		goto badaddr;
	if (!p->ADDR_length)
		goto noaddr;
	if (p->ADDR_length < sizeof(src))
		goto badaddr;
	bcopy(mp->b_rptr + p->ADDR_offset, &src, sizeof(src));
	if (src.family != AF_MTP)
		goto badaddr;
	if (!src.si || !src.pc)
		goto noaddr;
	if (src.si < 3 || mtp->cred.cr_uid != 0)
		goto access;
	if ((loc = mtp_check_src(mtp, &src, &err)))
		goto error;
	if (!mtp->sp.loc)
		mtp->sp.loc = sp_get(loc);
	mtp_set_state(mtp, NS_WACK_BREQ);
	return n_bind_ack(q, mtp, &src);
      access:
	err = NACCESS;
	ptrace(("%s: %p: ERROR: no priviledge for requested address\n", DRV_NAME, mtp));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: could not allocate address\n", DRV_NAME, mtp));
	goto error;
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: requested address invalid\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  N_UNBIND_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_unbind_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	mtp_set_state(mtp, NS_WACK_UREQ);
	return n_ok_ack(q, mtp, p->PRIM_type);
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  N_UNITDATA_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	const struct T_info_ack *i = mtp->sp.loc->na.na->prot[mtp->src.si];
	const size_t dlen = msgdsize(mp);
	struct mtp_addr dst;
	if (i->SERV_type != T_CLTS)
		goto notsupp;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	if (dlen == 0 && !(i->PROVIDER_flag & T_SNDZERO))
		goto baddata;
	if (dlen > i->TSDU_size || dlen > i->TIDU_size)
		goto baddata;
	if (!p->DEST_length)
		goto noaddr;
	if (p->DEST_length < sizeof(dst))
		goto badaddr;
	bcopy(mp->b_rptr + p->DEST_length, &dst, sizeof(dst));
	if (dst.family != AF_MTP)
		goto badaddr;
	if (!dst.si || !dst.pc)
		goto badaddr;
	if (dst.si < 3 && mtp->cred.cr_uid != 0)
		goto access;
	if (dst.si != mtp->src.si)
		goto badaddr;
	if (!mtp_check_dst(mtp, &dst))
		goto badaddr;
	if ((err = mtp_send_msg(q, mtp, NULL, &dst, mp->b_cont) == QR_ABSORBED))
		return (QR_TRIMMED);
	if (err < 0)
		goto error;
	return (err);
      access:
	err = NACCESS;
	ptrace(("%s: %p: ERROR: no priviledge for requested address\n", DRV_NAME, mtp));
	goto error;
      badaddr:
	err = NBADADDR;
	ptrace(("%s: %p: ERROR: requested address invalid\n", DRV_NAME, mtp));
	goto error;
      noaddr:
	err = NNOADDR;
	ptrace(("%s: %p: ERROR: could not allocate address\n", DRV_NAME, mtp));
	goto error;
      baddata:
	err = NBADDATA;
	ptrace(("%s: %p: ERROR: invalid amount of data\n", DRV_NAME, mtp));
	goto error;
      outstate:
	err = NOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive type not supported\n", DRV_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  N_OPTMGMT_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	struct mtp_opts opts;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
		goto badprim;
#ifdef NS_WACK_OPTREQ
	if (mtp_get_state(mtp) == NS_IDLE)
		mtp_set_state(mtp, NS_WACK_OPTREQ);
#endif
	if (p->OPTMGMT_flags)
		/* 
		   Can't support DEFAULT_RC_SEL yet */
		goto badflags;
	if (mtp_parse_qos(mtp, &opts, mp->b_rptr + p->QOS_offset, p->QOS_length))
		goto badqostype;
	if ((err = mtp_opt_check(mtp, &opts)))
		goto error;
	if ((err = mtp_opt_negotiate(mtp, &opts)))
		goto error;
	return n_ok_ack(q, mtp, p->PRIM_type);
      badqostype:
	err = NBADQOSTYPE;
	ptrace(("%s: %p: ERROR: invalid qos type\nn", DRV_NAME, mtp));
	goto error;
#if 0
      badqosparam:
	err = NBADQOSPARAM;
	ptrace(("%s: %p: ERROR: invalid qos parameter\nn", DRV_NAME, mtp));
	goto error;
#endif
      badflags:
	err = NBADFLAG;
	ptrace(("%s: %p: ERROR: invalid flag\nn", DRV_NAME, mtp));
	goto error;
      badprim:
	err = -EMSGSIZE;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mtp));
	goto error;
      error:
	return n_error_ack(q, mtp, p->PRIM_type, err);
}

/*
 *  N_DATACK_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_datack_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/* 
	   We don't support DATACK yet.  With zero loss operation we will. */
	rare();
	return (QR_DONE);
}

/*
 *  N_RESET_REQ:
 *  -------------------------------------------------------------------
 */
STATIC int
n_reset_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	todo(("Accept resets with reason from the user\n"));
	rare();
	return (QR_DONE);
}

/*
 *  N_RESET_RES:
 *  -------------------------------------------------------------------
 */
STATIC int
n_reset_res(queue_t *q, mblk_t *mp)
{
	(void) q;
	(void) mp;
	/* 
	   ignore.  if the user wishes to respond to our reset indications that's fine. */
	rare();
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IO Controls
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  GET Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mtp_get_na(struct mtp_config *arg, struct na *na, int size)
{
	struct sp *sp;
	mtp_conf_na_t *cnf = (typeof(cnf)) (arg + 1);
	mtp_conf_sp_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!na || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write list of local signalling points */
	chd = (typeof(chd)) (arg + 1);
	for (sp = na->sp.list; sp && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sp = sp->na.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MTP_OBJ_TYPE_SP;
		arg->id = sp->id;
		chd->naid = sp->na.na ? sp->na.na->id : 0;
	}
	/* 
	   end list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mtp_get_sp(struct mtp_config *arg, struct sp *sp, int size)
{
	struct rs *rs;
	struct ls *ls;
	mtp_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);
	mtp_conf_rs_t *chr;
	mtp_conf_ls_t *chl;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!sp || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write list of routesets */
	chr = (typeof(chr)) (arg + 1);
	for (rs = sp->rs.list; rs && size >= sizeof(*arg) + sizeof(*chr) + sizeof(*arg);
	     rs = rs->sp.next, size -= sizeof(*arg) + sizeof(*chr), arg =
	     (typeof(arg)) (chr + 1), chr = (typeof(chr)) (arg + 1)) {
		arg->type = MTP_OBJ_TYPE_RS;
		arg->id = rs->id;
		chr->spid = rs->sp.sp ? rs->sp.sp->id : 0;
	}
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write list of linksets */
	chl = (typeof(chl)) (arg + 1);
	for (ls = sp->ls.list; ls && size >= sizeof(*arg) + sizeof(*chl) + sizeof(*arg);
	     ls = ls->sp.next, size -= sizeof(*arg) + sizeof(*chl), arg =
	     (typeof(arg)) (chl + 1), chl = (typeof(chl)) (arg + 1)) {
		arg->type = MTP_OBJ_TYPE_LS;
		arg->id = ls->id;
		chr->spid = ls->sp.sp ? ls->sp.sp->id : 0;
	}
	/* 
	   end list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mtp_get_rs(struct mtp_config *arg, struct rs *rs, int size)
{
	struct rl *rl;
	mtp_conf_rs_t *cnf = (typeof(cnf)) (arg + 1);
	mtp_conf_rl_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!rs || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write list of routelists */
	chd = (typeof(chd)) (arg + 1);
	for (rl = rs->rl.list; rl && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     rl = rl->rs.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MTP_OBJ_TYPE_RL;
		arg->id = rl->id;
		chd->rsid = rl->rs.rs ? rl->rs.rs->id : 0;
	}
	/* 
	   end list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mtp_get_rl(struct mtp_config *arg, struct rl *rl, int size)
{
	struct rt *rt;
	mtp_conf_rl_t *cnf = (typeof(cnf)) (arg + 1);
	mtp_conf_rt_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!rl || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write list of routes */
	chd = (typeof(chd)) (arg + 1);
	for (rt = rl->rt.list; rt && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     rt = rt->rl.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MTP_OBJ_TYPE_RT;
		arg->id = rt->id;
		chd->rlid = rt->rl.rl ? rt->rl.rl->id : 0;
	}
	/* 
	   end list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mtp_get_rt(struct mtp_config *arg, struct rt *rt, int size)
{
	mtp_conf_rt_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!rt || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   end list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mtp_get_ls(struct mtp_config *arg, struct ls *ls, int size)
{
	struct lk *lk;
	mtp_conf_ls_t *cnf = (typeof(cnf)) (arg + 1);
	mtp_conf_lk_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!ls || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write list of links */
	chd = (typeof(chd)) (arg + 1);
	for (lk = ls->lk.list; lk && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     lk = lk->ls.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MTP_OBJ_TYPE_LK;
		arg->id = lk->id;
		chd->lsid = lk->ls.ls ? lk->ls.ls->id : 0;
	}
	/* 
	   end list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mtp_get_lk(struct mtp_config *arg, struct lk *lk, int size)
{
	struct sl *sl;
	mtp_conf_lk_t *cnf = (typeof(cnf)) (arg + 1);
	mtp_conf_sl_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!lk || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write list of signalling links */
	chd = (typeof(chd)) (arg + 1);
	for (sl = lk->sl.list; sl && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     sl = sl->lk.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MTP_OBJ_TYPE_SL;
		arg->id = sl->id;
		chd->lkid = sl->lk.lk ? sl->lk.lk->id : 0;
	}
	/* 
	   end list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mtp_get_sl(struct mtp_config *arg, struct sl *sl, int size)
{
	mtp_conf_sl_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!sl || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   end list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mtp_get_df(struct mtp_config *arg, struct df *df, int size)
{
	struct na *na;
	mtp_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	mtp_conf_na_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!df || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   write list of network appearances */
	chd = (typeof(chd)) (arg + 1);
	for (na = df->na.list; na && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     na = na->next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MTP_OBJ_TYPE_NA;
		arg->id = na->id;
	}
	/* 
	   end list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mtp_get_conf(struct mtp_config *arg, int size)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_NA:
		return mtp_get_na(arg, na_lookup(arg->id), size);
	case MTP_OBJ_TYPE_SP:
		return mtp_get_sp(arg, sp_lookup(arg->id), size);
	case MTP_OBJ_TYPE_RS:
		return mtp_get_rs(arg, rs_lookup(arg->id), size);
	case MTP_OBJ_TYPE_RL:
		return mtp_get_rl(arg, rl_lookup(arg->id), size);
	case MTP_OBJ_TYPE_RT:
		return mtp_get_rt(arg, rt_lookup(arg->id), size);
	case MTP_OBJ_TYPE_LS:
		return mtp_get_ls(arg, ls_lookup(arg->id), size);
	case MTP_OBJ_TYPE_LK:
		return mtp_get_lk(arg, lk_lookup(arg->id), size);
	case MTP_OBJ_TYPE_SL:
		return mtp_get_sl(arg, sl_lookup(arg->id), size);
	case MTP_OBJ_TYPE_DF:
		return mtp_get_df(arg, df_lookup(arg->id), size);
	default:
		rare();
		return (-EINVAL);
	}
}

/*
 *  ADD Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mtp_add_na(struct mtp_config *arg, struct na *na, int size, const int force, const int test)
{
	mtp_conf_na_t *cnf = (typeof(cnf)) (arg + 1);
	if (na || (size -= sizeof(*cnf)) < 0)
		return (-EMSGSIZE);
	if (!test) {
		if (!
		    (na =
		     mtp_alloc_na(na_get_id(arg->id), cnf->mask.member, cnf->mask.cluster,
				  cnf->mask.network, cnf->sls_bits, &cnf->options)))
			return (-ENOMEM);
		arg->id = na->id;
	}
	return (QR_DONE);
}
STATIC int
mtp_add_sp(struct mtp_config *arg, struct sp *sp, int size, const int force, const int test)
{
	struct na *na;
	mtp_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);
	if (sp || (size -= sizeof(*cnf)) < 0)
		return (-EMSGSIZE);
	for (na = master.na.list; na && na->id != cnf->naid; na = na->next) ;
	if (!na)
		return (-EINVAL);
	/* 
	   signalling point point code must not be assigned yet */
	for (sp = na->sp.list; sp && sp->pc != cnf->pc; sp = sp->na.next) ;
	if (sp)
		return (-EADDRINUSE);
	if (!test) {
		if (!(sp = mtp_alloc_sp(sp_get_id(arg->id), na, cnf->pc, cnf->users, cnf->flags)))
			return (-ENOMEM);
		arg->id = sp->id;
	}
	return (QR_DONE);
}
STATIC int
mtp_add_rs(struct mtp_config *arg, struct rs *rs, int size, const int force, const int test)
{
	struct sp *sp;
	mtp_conf_rs_t *cnf = (typeof(cnf)) (arg + 1);
	if (rs || (size -= sizeof(*cnf)) < 0)
		return (-EMSGSIZE);
	for (sp = master.sp.list; sp && sp->id != cnf->spid; sp = sp->next) ;
	if (!sp)
		return (-EINVAL);
	/* 
	   route set point code must not be assigned yet */
	for (rs = sp->rs.list; rs && rs->dest != cnf->dest; rs = rs->sp.next) ;
	if (rs)
		return (-EADDRINUSE);
	if (!test) {
		if (!(rs = mtp_alloc_rs(rs_get_id(arg->id), sp, cnf->dest, cnf->flags)))
			return (-ENOMEM);
		arg->id = rs->id;
	}
	return (QR_DONE);
}
STATIC int
mtp_add_rl(struct mtp_config *arg, struct rl *rl, int size, const int force, const int test)
{
	struct rs *rs;
	struct ls *ls;
	mtp_conf_rl_t *cnf = (typeof(cnf)) (arg + 1);
	if (rl || (size -= sizeof(*cnf)) < 0)
		return (-EMSGSIZE);
	for (ls = master.ls.list; ls && ls->id != cnf->lsid; ls = ls->next) ;
	if (!ls)
		return (-EINVAL);
	for (rs = master.rs.list; rs && rs->id != cnf->rsid; rs = rs->next) ;
	if (!rs)
		return (-EINVAL);
	if (!test) {
		if (!(rl = mtp_alloc_rl(rl_get_id(arg->id), rs, ls, cnf->cost)))
			return (-ENOMEM);
		arg->id = rl->id;
	}
	return (QR_DONE);
}
STATIC int
mtp_add_rt(struct mtp_config *arg, struct rt *rt, int size, const int force, const int test)
{
	struct rl *rl;
	struct lk *lk = NULL;
	struct ls *ls;
	mtp_conf_rt_t *cnf = (typeof(cnf)) (arg + 1);
	if (rt || (size -= sizeof(*cnf)) < 0)
		return (-EMSGSIZE);
	for (rl = master.rl.list; rl && rl->id != cnf->rlid; rl = rl->next) ;
	if (!rl)
		return (-EINVAL);
	for (ls = rl->rs.rs->sp.sp->ls.list; ls; ls = ls->sp.next)
		for (lk = ls->lk.list; lk; lk = lk->ls.next)
			if (lk->id == cnf->lkid)
				goto found_lk;
	if (!lk)
		return (-EINVAL);
      found_lk:
	if (!test) {
		if (!(rt = mtp_alloc_rt(rt_get_id(arg->id), rl, lk, cnf->slot)))
			return (-ENOMEM);
		arg->id = rt->id;
	}
	return (QR_DONE);
}
STATIC int
mtp_add_ls(struct mtp_config *arg, struct ls *ls, int size, const int force, const int test)
{
	struct sp *sp;
	mtp_conf_ls_t *cnf = (typeof(cnf)) (arg + 1);
	if (ls || (size -= sizeof(*cnf)) < 0)
		return (-EMSGSIZE);
	for (sp = master.sp.list; sp && sp->id != cnf->spid; sp = sp->next) ;
	if (!sp)
		return (-EINVAL);
	if (!test) {
		if (!(ls = mtp_alloc_ls(ls_get_id(arg->id), sp, cnf->sls_mask)))
			return (-ENOMEM);
		arg->id = ls->id;
	}
	return (QR_DONE);
}
STATIC int
mtp_add_lk(struct mtp_config *arg, struct lk *lk, int size, const int force, const int test)
{
	struct ls *ls, *l;
	struct sp *sp;
	struct rs *rs;
	mtp_conf_lk_t *cnf = (typeof(cnf)) (arg + 1);
	if (lk || (size -= sizeof(*cnf)) < 0)
		return (-EMSGSIZE);
	for (ls = master.ls.list; ls && ls->id != cnf->lsid; ls = ls->next) ;
	if (!ls)
		return (-EINVAL);
	if (!(sp = ls->sp.sp))
		return (-EFAULT);
	for (rs = sp->rs.list; rs && rs->id != cnf->rsid; rs = rs->sp.next) ;
	if (!rs)
		return (-EINVAL);
	for (l = sp->ls.list; l; l = l->sp.next)
		for (lk = l->lk.list; lk; lk = lk->ls.next)
			if (lk->sp.adj == rs)
				return (-EBUSY);
	for (lk = ls->lk.list; lk; lk = lk->ls.next)
		if (lk->slot == cnf->slot)
			return (-EBUSY);
	if (!test) {
		if (!(lk = mtp_alloc_lk(lk_get_id(arg->id), ls, sp, rs, cnf->ni, cnf->slot)))
			return (-ENOMEM);
		arg->id = lk->id;
	}
	return (QR_DONE);
}
STATIC int
mtp_add_sl(struct mtp_config *arg, struct sl *sl, int size, const int force, const int test)
{
	struct lk *lk;
	mtp_conf_sl_t *cnf = (typeof(cnf)) (arg + 1);
	if (sl || (size -= sizeof(*cnf)) < 0)
		return (-EMSGSIZE);
	for (lk = master.lk.list; lk && lk->id != cnf->lkid; lk = lk->next) ;
	if (!lk)
		return (-EINVAL);
	for (sl = lk->sl.list; sl; sl = sl->lk.next)
		if (sl->slc == cnf->slc)
			return (-EBUSY);
	for (sl = master.sl.list; sl && sl->u.mux.index != cnf->muxid; sl = sl->next) ;
	if (!sl)
		return (-EINVAL);
	if (!test) {
		sl->id = sl_get_id(arg->id);
		if (lk) {
			/* 
			   add to link (set) list */
			if ((sl->lk.next = lk->sl.list))
				sl->lk.next->lk.prev = &sl->lk.next;
			sl->lk.prev = &lk->sl.list;
			sl->lk.lk = lk_get(lk);
			lk->sl.list = sl_get(sl);
			lk->sl.numb++;
		}
		sl->slc = cnf->slc;
		{
			/* 
			   defaults inherited from lk */
			sl->config.t1 = lk->config.t1;
			sl->config.t2 = lk->config.t2;
			sl->config.t3 = lk->config.t3;
			sl->config.t4 = lk->config.t4;
			sl->config.t5 = lk->config.t5;
			sl->config.t12 = lk->config.t12;
			sl->config.t13 = lk->config.t13;
			sl->config.t14 = lk->config.t14;
			sl->config.t17 = lk->config.t17;
			sl->config.t19a = lk->config.t19a;
			sl->config.t20a = lk->config.t20a;
			sl->config.t21a = lk->config.t21a;
			sl->config.t22 = lk->config.t22;
			sl->config.t23 = lk->config.t23;
			sl->config.t24 = lk->config.t24;
			sl->config.t31a = lk->config.t31a;
			sl->config.t32a = lk->config.t32a;
			sl->config.t33a = lk->config.t33a;
			sl->config.t34a = lk->config.t34a;
			sl->config.t1t = lk->config.t1t;
			sl->config.t2t = lk->config.t2t;
			sl->config.t1s = lk->config.t1s;
		}
		arg->id = sl->id;
	}
	return (QR_DONE);
}
STATIC int
mtp_add_df(struct mtp_config *arg, struct df *df, int size, const int force, const int test)
{
	mtp_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if (df || (size -= sizeof(*cnf)) < 0)
		return (-EMSGSIZE);
	/* 
	   DF objects are not added statically */
	return (-EINVAL);
}
STATIC int
mtp_add_conf(struct mtp_config *arg, int size, const int force, const int test)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_NA:
		return mtp_add_na(arg, na_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_SP:
		return mtp_add_sp(arg, sp_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_RS:
		return mtp_add_rs(arg, rs_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_RL:
		return mtp_add_rl(arg, rl_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_RT:
		return mtp_add_rt(arg, rt_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_LS:
		return mtp_add_ls(arg, ls_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_LK:
		return mtp_add_lk(arg, lk_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_SL:
		return mtp_add_sl(arg, sl_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_DF:
		return mtp_add_df(arg, df_lookup(arg->id), size, force, test);
	default:
		rare();
		return (-EINVAL);
	}
}

/*
 *  CHA Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mtp_cha_na(struct mtp_config *arg, struct na *na, int size, const int force, const int test)
{
	mtp_conf_na_t *cnf = (typeof(cnf)) (arg + 1);
	if (!na || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		na->option = cnf->options;
		na->mask.member = cnf->mask.member;
		na->mask.cluster = cnf->mask.cluster;
		na->mask.network = cnf->mask.network;
		na->sp.sls_bits = cnf->sls_bits;
		na->sp.sls_mask = ((1 << cnf->sls_bits) - 1);
	}
	return (QR_DONE);
}
STATIC int
mtp_cha_sp(struct mtp_config *arg, struct sp *sp, int size, const int force, const int test)
{
	struct sp *s;
	mtp_conf_sp_t *cnf = (typeof(cnf)) (arg + 1);
	if (!sp || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->naid && cnf->naid != sp->na.na->id)
		return (-EINVAL);
	for (s = sp->na.na->sp.list; s; s = s->na.next)
		if (s != sp && s->pc == cnf->pc)
			return (-EINVAL);
	if (!force) {
		int i;
		/* 
		   have users */
		for (i = 0; i < 16; i++)
			if (sp->mtp.lists[i])
				return (-EBUSY);
		/* 
		   have linksets */
		if (sp->ls.list)
			return (-EBUSY);
	}
	if (!test) {
		ulong mask =
		    (SPF_CLUSTER | SPF_LOSC_PROC_A | SPF_LOSC_PROC_B | SPF_SECURITY |
		     SPF_XFER_FUNC);
		sp->pc = cnf->pc;
		sp->mtp.equipped = cnf->users | sp->mtp.available;
		sp->flags &= ~mask;
		sp->flags |= cnf->flags & mask;
	}
	return (QR_DONE);
}
STATIC int
mtp_cha_rs(struct mtp_config *arg, struct rs *rs, int size, const int force, const int test)
{
	struct rs *r;
	mtp_conf_rs_t *cnf = (typeof(cnf)) (arg + 1);
	if (!rs || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid && cnf->spid != rs->sp.sp->id)
		return (-EINVAL);
	for (r = rs->sp.sp->rs.list; r; r = r->sp.next)
		if (r != rs && r->dest == cnf->dest)
			return (-EINVAL);
	if (!force) {
		/* 
		   have route lists */
		if (rs->rl.list)
			return (-EBUSY);
	}
	if (!test) {
		ulong mask = (RSF_TFR_PENDING | RSF_ADJACENT | RSF_CLUSTER | RSF_XFER_FUNC);
		rs->dest = cnf->dest;
		rs->flags &= ~mask;
		rs->flags |= cnf->flags & mask;
	}
	return (QR_DONE);
}
STATIC int
mtp_cha_rl(struct mtp_config *arg, struct rl *rl, int size, const int force, const int test)
{
	struct rl *r;
	mtp_conf_rl_t *cnf = (typeof(cnf)) (arg + 1);
	if (!rl || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->rsid && cnf->rsid != rl->rs.rs->id)
		return (-EINVAL);
	if (cnf->lsid && cnf->lsid != rl->ls.ls->id)
		return (-EINVAL);
	for (r = rl->rs.rs->rl.list; r; r = r->rs.next)
		if (r != rl && r->cost == cnf->cost)
			return (-EINVAL);
	if (!force) {
		if (rl->rt.list || rl->cr.list)
			return (-EBUSY);
	}
	if (!test) {
		rl->cost = cnf->cost;
	}
	return (QR_DONE);
}
STATIC int
mtp_cha_rt(struct mtp_config *arg, struct rt *rt, int size, const int force, const int test)
{
	struct rt *r;
	mtp_conf_rt_t *cnf = (typeof(cnf)) (arg + 1);
	if (!rt || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->rlid && cnf->rlid != rt->rl.rl->id)
		return (-EINVAL);
	if (cnf->lkid && cnf->lkid != rt->lk.lk->id)
		return (-EINVAL);
	for (r = rt->rl.rl->rt.list; r; r = r->next)
		if (r != rt && r->slot == cnf->slot)
			return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		rt->slot = cnf->slot;
	}
	return (QR_DONE);
}
STATIC int
mtp_cha_ls(struct mtp_config *arg, struct ls *ls, int size, const int force, const int test)
{
	mtp_conf_ls_t *cnf = (typeof(cnf)) (arg + 1);
	if (!ls || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->spid && cnf->spid != ls->sp.sp->id)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		struct sp *sp = ls->sp.sp;
		ulong mask = cnf->sls_mask;
		ls->lk.sls_mask = cnf->sls_mask;
		ls->lk.sls_bits = 0;
		/* 
		   count the 1's in the mask */
		while (mask) {
			if (mask & 0x01)
				ls->lk.sls_bits++;
			mask >>= 1;
		}
		ls->rl.sls_mask = ~cnf->sls_mask & sp->ls.sls_mask;
		ls->rl.sls_bits = sp->ls.sls_bits - ls->lk.sls_bits;
	}
	return (QR_DONE);
}
STATIC int
mtp_cha_lk(struct mtp_config *arg, struct lk *lk, int size, const int force, const int test)
{
	mtp_conf_lk_t *cnf = (typeof(cnf)) (arg + 1);
	if (!lk || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->lsid && cnf->lsid != lk->ls.ls->id)
		return (-EINVAL);
	if (cnf->rsid && cnf->rsid != lk->sp.adj->id)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		lk->ni = cnf->ni;
		lk->slot = cnf->slot;
	}
	return (QR_DONE);
}
STATIC int
mtp_cha_sl(struct mtp_config *arg, struct sl *sl, int size, const int force, const int test)
{
	struct sl *s;
	mtp_conf_sl_t *cnf = (typeof(cnf)) (arg + 1);
	if (!sl || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (cnf->muxid && cnf->muxid != sl->u.mux.index)
		return (-EINVAL);
	if (cnf->lkid && cnf->lkid != sl->lk.lk->id)
		return (-EINVAL);
	for (s = sl->lk.lk->sl.list; s; s = s->lk.next)
		if (s != sl && s->slc == cnf->slc)
			return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		sl->slc = cnf->slc;
	}
	return (QR_DONE);
}
STATIC int
mtp_cha_df(struct mtp_config *arg, struct df *df, int size, const int force, const int test)
{
	mtp_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if (!df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
	}
	return (QR_DONE);
}
STATIC int
mtp_cha_conf(struct mtp_config *arg, int size, const int force, const int test)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_NA:
		return mtp_cha_na(arg, na_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_SP:
		return mtp_cha_sp(arg, sp_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_RS:
		return mtp_cha_rs(arg, rs_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_RL:
		return mtp_cha_rl(arg, rl_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_RT:
		return mtp_cha_rt(arg, rt_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_LS:
		return mtp_cha_ls(arg, ls_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_LK:
		return mtp_cha_lk(arg, lk_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_SL:
		return mtp_cha_sl(arg, sl_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_DF:
		return mtp_cha_df(arg, df_lookup(arg->id), size, force, test);
	default:
		rare();
		return (-EINVAL);
	}
}

/*
 *  DEL Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mtp_del_na(struct mtp_config *arg, struct na *na, int size, const int force, const int test)
{
	if (!na)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal data structures */
		if (na->sp.list)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_na(na);
	}
	return (QR_DONE);
}
STATIC int
mtp_del_sp(struct mtp_config *arg, struct sp *sp, int size, const int force, const int test)
{
	if (!sp)
		return (-EINVAL);
	if (!force) {
		int i;
		/* 
		   bound to internal data structures */
		if (sp->ls.list || sp->rs.list)
			return (-EBUSY);
		for (i = 0; i < 16; i++)
			if (sp->mtp.lists[i])
				return (-EBUSY);
	}
	if (!test) {
		mtp_free_sp(sp);
	}
	return (QR_DONE);
}
STATIC int
mtp_del_rs(struct mtp_config *arg, struct rs *rs, int size, const int force, const int test)
{
	if (!rs)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal data structures */
		if (rs->rl.list || rs->rr.list)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_rs(rs);
	}
	return (QR_DONE);
}
STATIC int
mtp_del_rl(struct mtp_config *arg, struct rl *rl, int size, const int force, const int test)
{
	if (!rl)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal data structures */
		if (rl->rt.list || rl->cr.list)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_rl(rl);
	}
	return (QR_DONE);
}
STATIC int
mtp_del_rt(struct mtp_config *arg, struct rt *rt, int size, const int force, const int test)
{
	if (!rt)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal data structures */
	}
	if (!test) {
		mtp_free_rt(rt);
	}
	return (QR_DONE);
}
STATIC int
mtp_del_ls(struct mtp_config *arg, struct ls *ls, int size, const int force, const int test)
{
	if (!ls)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal data structures */
	}
	if (!test) {
		mtp_free_ls(ls);
	}
	return (QR_DONE);
}
STATIC int
mtp_del_lk(struct mtp_config *arg, struct lk *lk, int size, const int force, const int test)
{
	if (!lk)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal data structures */
		if (lk->rt.list || lk->cb.list || lk->sl.list)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_lk(lk);
	}
	return (QR_DONE);
}
STATIC int
mtp_del_sl(struct mtp_config *arg, struct sl *sl, int size, const int force, const int test)
{
	if (!sl)
		return (-EINVAL);
	if (!force) {
		/* 
		   bound to internal datastructures */
		if (sl->lk.lk)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_sl(sl);
	}
	return (QR_DONE);
}
STATIC int
mtp_del_df(struct mtp_config *arg, struct df *df, int size, const int force, const int test)
{
	mtp_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if (!df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	if (!force) {
		return (-EBUSY);
	}
	if (!test) {
		/* 
		   can't delete default */
	}
	return (QR_DONE);
}
STATIC int
mtp_del_conf(struct mtp_config *arg, int size, const int force, const int test)
{
	switch (arg->type) {
	case MTP_OBJ_TYPE_NA:
		return mtp_del_na(arg, na_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_SP:
		return mtp_del_sp(arg, sp_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_RS:
		return mtp_del_rs(arg, rs_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_RL:
		return mtp_del_rl(arg, rl_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_RT:
		return mtp_del_rt(arg, rt_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_LS:
		return mtp_del_ls(arg, ls_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_LK:
		return mtp_del_lk(arg, lk_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_SL:
		return mtp_del_sl(arg, sl_lookup(arg->id), size, force, test);
	case MTP_OBJ_TYPE_DF:
		return mtp_del_df(arg, df_lookup(arg->id), size, force, test);
	default:
		rare();
		return (-EINVAL);
	}
}

STATIC int mtp_mgmt_na(queue_t *q, struct na *, ulong);
STATIC int mtp_mgmt_sp(queue_t *q, struct sp *, ulong);
STATIC int mtp_mgmt_rs(queue_t *q, struct rs *, ulong);
STATIC int mtp_mgmt_rl(queue_t *q, struct rl *, ulong);
STATIC int mtp_mgmt_rt(queue_t *q, struct rt *, ulong);
STATIC int mtp_mgmt_ls(queue_t *q, struct ls *, ulong);
STATIC int mtp_mgmt_lk(queue_t *q, struct lk *, ulong);
STATIC int mtp_mgmt_sl(queue_t *q, struct sl *, ulong);
STATIC int mtp_mgmt_df(queue_t *q, struct df *, ulong);
/*
 *  MGMT - Manage Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mtp_mgmt_na(queue_t *q, struct na *na, ulong action)
{
	int ret, err = 0;
	struct sp *sp;
	/* 
	   we actually don't manage network appearances, just all of the signalling points that
	   make up a network appearance. */
	for (sp = na->sp.list; sp; sp = sp->na.next)
		if ((ret = mtp_mgmt_sp(q, sp, action)) < 0) {
			if (ret != -EOPNOTSUPP)
				return (ret);
			err = ret;
		}
	return (err);
}
STATIC int
mtp_mgmt_sp(queue_t *q, struct sp *sp, ulong action)
{
	int ret, err = 0;
	struct rs *rs;
	struct ls *ls;
	switch (action) {
	case MTP_MGMT_ALLOW:
		break;
	case MTP_MGMT_RESTRICT:
		break;
	case MTP_MGMT_PROHIBIT:
		break;
	case MTP_MGMT_ACTIVATE:
		break;
	case MTP_MGMT_DEACTIVATE:
		break;
	case MTP_MGMT_BLOCK:
		break;
	case MTP_MGMT_UNBLOCK:
		break;
	case MTP_MGMT_INHIBIT:
		break;
	case MTP_MGMT_UNINHIBIT:
		break;
	case MTP_MGMT_CONGEST:
		break;
	case MTP_MGMT_UNCONGEST:
		break;
	case MTP_MGMT_DANGER:
		break;
	case MTP_MGMT_NODANGER:
		break;
	case MTP_MGMT_RESTART:
		break;
	case MTP_MGMT_RESTARTED:
		break;
	default:
		rare();
		return (-EOPNOTSUPP);
	}
	for (rs = sp->rs.list; rs; rs = rs->sp.next)
		if ((ret = mtp_mgmt_rs(q, rs, action)) < 0) {
			if (ret != -EOPNOTSUPP)
				return (ret);
			err = ret;
		}
	for (ls = sp->ls.list; ls; ls = ls->sp.next)
		if ((ret = mtp_mgmt_ls(q, ls, action)) < 0) {
			if (ret != -EOPNOTSUPP)
				return (ret);
			err = ret;
		}
	return (err);
}
STATIC int
mtp_mgmt_rs(queue_t *q, struct rs *rs, ulong action)
{
	int ret, err = 0;
	struct rl *rl;
	switch (action) {
	case MTP_MGMT_ALLOW:
		break;
	case MTP_MGMT_RESTRICT:
		break;
	case MTP_MGMT_PROHIBIT:
		break;
	case MTP_MGMT_ACTIVATE:
		break;
	case MTP_MGMT_DEACTIVATE:
		break;
	case MTP_MGMT_BLOCK:
		break;
	case MTP_MGMT_UNBLOCK:
		break;
	case MTP_MGMT_INHIBIT:
		break;
	case MTP_MGMT_UNINHIBIT:
		break;
	case MTP_MGMT_CONGEST:
		break;
	case MTP_MGMT_UNCONGEST:
		break;
	case MTP_MGMT_DANGER:
		break;
	case MTP_MGMT_NODANGER:
		break;
	case MTP_MGMT_RESTART:
		break;
	case MTP_MGMT_RESTARTED:
		break;
	default:
		rare();
		return (-EOPNOTSUPP);
	}
	for (rl = rs->rl.list; rl; rl = rl->rs.next)
		if ((ret = mtp_mgmt_rl(q, rl, action)) < 0) {
			if (ret != -EOPNOTSUPP)
				return (ret);
			err = ret;
		}
	return (err);
}
STATIC int
mtp_mgmt_rl(queue_t *q, struct rl *rl, ulong action)
{
	int ret, err = 0;
	struct rt *rt;
	switch (action) {
	case MTP_MGMT_ALLOW:
		break;
	case MTP_MGMT_RESTRICT:
		break;
	case MTP_MGMT_PROHIBIT:
		break;
	case MTP_MGMT_ACTIVATE:
		break;
	case MTP_MGMT_DEACTIVATE:
		break;
	case MTP_MGMT_BLOCK:
		break;
	case MTP_MGMT_UNBLOCK:
		break;
	case MTP_MGMT_INHIBIT:
		break;
	case MTP_MGMT_UNINHIBIT:
		break;
	case MTP_MGMT_CONGEST:
		break;
	case MTP_MGMT_UNCONGEST:
		break;
	case MTP_MGMT_DANGER:
		break;
	case MTP_MGMT_NODANGER:
		break;
	case MTP_MGMT_RESTART:
		break;
	case MTP_MGMT_RESTARTED:
		break;
	default:
		rare();
		return (-EOPNOTSUPP);
	}
	for (rt = rl->rt.list; rt; rt = rt->rl.next)
		if ((ret = mtp_mgmt_rt(q, rt, action)) < 0) {
			if (ret != -EOPNOTSUPP)
				return (ret);
			err = ret;
		}
	return (err);
}
STATIC int
mtp_mgmt_rt(queue_t *q, struct rt *rt, ulong action)
{
	switch (action) {
	case MTP_MGMT_ALLOW:
		return rt_set_state(q, rt, RT_ALLOWED);
	case MTP_MGMT_RESTRICT:
		return rt_set_state(q, rt, RT_RESTRICTED);
	case MTP_MGMT_PROHIBIT:
		return rt_set_state(q, rt, RT_PROHIBITED);
	case MTP_MGMT_ACTIVATE:
		return rt_set_state(q, rt, RT_ACTIVE);
	case MTP_MGMT_DEACTIVATE:
		return rt_set_state(q, rt, RT_INACTIVE);
	case MTP_MGMT_BLOCK:
		return rt_set_state(q, rt, RT_BLOCKED);
	case MTP_MGMT_UNBLOCK:
		return rt_set_state(q, rt, RT_UNBLOCKED);
	case MTP_MGMT_INHIBIT:
		return rt_set_state(q, rt, RT_INHIBITED);
	case MTP_MGMT_UNINHIBIT:
		return rt_set_state(q, rt, RT_UNINHIBITED);
	case MTP_MGMT_CONGEST:
		return rt_set_state(q, rt, RT_CONGESTED);
	case MTP_MGMT_UNCONGEST:
		return rt_set_state(q, rt, RT_UNCONGESTED);
	case MTP_MGMT_DANGER:
		return rt_set_state(q, rt, RT_DANGER);
	case MTP_MGMT_NODANGER:
		return rt_set_state(q, rt, RT_NODANGER);
	case MTP_MGMT_RESTART:
		return rt_set_state(q, rt, RT_RESTART);
	case MTP_MGMT_RESTARTED:
		return rt_set_state(q, rt, RT_RESTARTED);
	default:
		rare();
		return (-EOPNOTSUPP);
	}
	return (QR_DONE);
}
STATIC int
mtp_mgmt_ls(queue_t *q, struct ls *ls, ulong action)
{
	int ret, err = 0;
	struct lk *lk;
	switch (action) {
	case MTP_MGMT_ALLOW:
		break;
	case MTP_MGMT_RESTRICT:
		break;
	case MTP_MGMT_PROHIBIT:
		break;
	case MTP_MGMT_ACTIVATE:
		break;
	case MTP_MGMT_DEACTIVATE:
		break;
	case MTP_MGMT_BLOCK:
		break;
	case MTP_MGMT_UNBLOCK:
		break;
	case MTP_MGMT_INHIBIT:
		break;
	case MTP_MGMT_UNINHIBIT:
		break;
	case MTP_MGMT_CONGEST:
		break;
	case MTP_MGMT_UNCONGEST:
		break;
	case MTP_MGMT_DANGER:
		break;
	case MTP_MGMT_NODANGER:
		break;
	case MTP_MGMT_RESTART:
		break;
	case MTP_MGMT_RESTARTED:
		break;
	default:
		rare();
		return (-EOPNOTSUPP);
	}
	for (lk = ls->lk.list; lk; lk = lk->ls.next)
		if ((ret = mtp_mgmt_lk(q, lk, action)) < 0) {
			if (ret != -EOPNOTSUPP)
				return (ret);
			err = ret;
		}
	return (err);
}
STATIC int
mtp_mgmt_lk(queue_t *q, struct lk *lk, ulong action)
{
	int ret, err = 0;
	struct sl *sl;
	switch (action) {
	case MTP_MGMT_ALLOW:
		break;
	case MTP_MGMT_RESTRICT:
		break;
	case MTP_MGMT_PROHIBIT:
		break;
	case MTP_MGMT_ACTIVATE:
		break;
	case MTP_MGMT_DEACTIVATE:
		break;
	case MTP_MGMT_BLOCK:
		break;
	case MTP_MGMT_UNBLOCK:
		break;
	case MTP_MGMT_INHIBIT:
		break;
	case MTP_MGMT_UNINHIBIT:
		break;
	case MTP_MGMT_CONGEST:
		break;
	case MTP_MGMT_UNCONGEST:
		break;
	case MTP_MGMT_DANGER:
		break;
	case MTP_MGMT_NODANGER:
		break;
	case MTP_MGMT_RESTART:
		break;
	case MTP_MGMT_RESTARTED:
		break;
	default:
		rare();
		return (-EOPNOTSUPP);
	}
	for (sl = lk->sl.list; sl; sl = sl->lk.next)
		if ((ret = mtp_mgmt_sl(q, sl, action)) < 0) {
			if (ret != -EOPNOTSUPP)
				return (ret);
			err = ret;
		}
	return (err);
}
STATIC int
mtp_mgmt_sl(queue_t *q, struct sl *sl, ulong action)
{
	switch (action) {
	case MTP_MGMT_ALLOW:
		return sl_set_state(q, sl, SL_RESTORED);
	case MTP_MGMT_RESTRICT:
		return sl_set_state(q, sl, SL_UNUSABLE);
	case MTP_MGMT_PROHIBIT:
		return sl_set_state(q, sl, SL_FAILED);
	case MTP_MGMT_ACTIVATE:
		return sl_set_state(q, sl, SL_ACTIVE);
	case MTP_MGMT_DEACTIVATE:
		return sl_set_state(q, sl, SL_INACTIVE);
	case MTP_MGMT_BLOCK:
		return sl_set_state(q, sl, SL_BLOCKED);
	case MTP_MGMT_UNBLOCK:
		return sl_set_state(q, sl, SL_UNBLOCKED);
	case MTP_MGMT_INHIBIT:
		return sl_set_state(q, sl, SL_INHIBITED);
	case MTP_MGMT_UNINHIBIT:
		return sl_set_state(q, sl, SL_UNINHIBITED);
	case MTP_MGMT_CONGEST:
		return sl_set_state(q, sl, SL_CONGESTED);
	case MTP_MGMT_UNCONGEST:
		return sl_set_state(q, sl, SL_UNCONGESTED);
	case MTP_MGMT_DANGER:
		return sl_set_state(q, sl, SL_DANGER);
	case MTP_MGMT_NODANGER:
		return sl_set_state(q, sl, SL_NODANGER);
	case MTP_MGMT_RESTART:
		return sl_set_state(q, sl, SL_RETRIEVAL);
	case MTP_MGMT_RESTARTED:
		return sl_set_state(q, sl, SL_UPDATED);
	default:
		rare();
		return (-EOPNOTSUPP);
	}
	return (QR_DONE);
}
STATIC int
mtp_mgmt_df(queue_t *q, struct df *df, ulong action)
{
	int ret, err = 0;
	struct na *na;
	/* 
	   we actually don't manage the default object, just all of the network appearances that
	   make up the stack. */
	for (na = df->na.list; na; na = na->next)
		if ((ret = mtp_mgmt_na(q, na, action)) < 0) {
			if (ret != -EOPNOTSUPP)
				return (ret);
			err = ret;
		}
	return (err);
}

/*
 *  MTP_IOCGOPTION  - mtp_option_t
 *  -------------------------------------------------------------------
 *  Get configuration options by object type and id.
 *
 */
STATIC int
mtp_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		mtp_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case MTP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				mtp_opt_conf_na_t *opt = (typeof(opt)) (arg + 1);
				if (!na || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = na->config;
				break;
			}
			case MTP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				mtp_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);
				if (!sp || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = sp->config;
				break;
			}
			case MTP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				mtp_opt_conf_rs_t *opt = (typeof(opt)) (arg + 1);
				if (!rs || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = rs->config;
				break;
			}
			case MTP_OBJ_TYPE_RL:
			{
				struct rl *rl = rl_lookup(arg->id);
				mtp_opt_conf_rl_t *opt = (typeof(opt)) (arg + 1);
				if (!rl || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = rl->config;
				break;
			}
			case MTP_OBJ_TYPE_RT:
			{
				struct rt *rt = rt_lookup(arg->id);
				mtp_opt_conf_rt_t *opt = (typeof(opt)) (arg + 1);
				if (!rt || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = rt->config;
				break;
			}
			case MTP_OBJ_TYPE_LS:
			{
				struct ls *ls = ls_lookup(arg->id);
				mtp_opt_conf_ls_t *opt = (typeof(opt)) (arg + 1);
				if (!ls || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = ls->config;
				break;
			}
			case MTP_OBJ_TYPE_LK:
			{
				struct lk *lk = lk_lookup(arg->id);
				mtp_opt_conf_lk_t *opt = (typeof(opt)) (arg + 1);
				if (!lk || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = lk->config;
				break;
			}
			case MTP_OBJ_TYPE_SL:
			{
				struct sl *sl = sl_lookup(arg->id);
				mtp_opt_conf_sl_t *opt = (typeof(opt)) (arg + 1);
				if (!sl || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = sl->config;
				break;
			}
			case MTP_OBJ_TYPE_DF:
			{
				struct df *df = df_lookup(arg->id);
				mtp_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
				if (!df || (size -= sizeof(*opt)) < 0)
					goto einval;
				*opt = df->config;
				break;
			}
			default:
				rare();
			      einval:
				return (-EINVAL);
			}
			return (QR_DONE);
		}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MTP_IOCSOPTION  - mtp_option_t
 *  -------------------------------------------------------------------
 *  Set configuration options by object type and id.
 */
STATIC int
mtp_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		mtp_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case MTP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				mtp_opt_conf_na_t *opt = (typeof(opt)) (arg + 1);
				if (!na || (size -= sizeof(*opt)) < 0)
					goto einval;
				na->config = *opt;
				break;
			}
			case MTP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				mtp_opt_conf_sp_t *opt = (typeof(opt)) (arg + 1);
				if (!sp || (size -= sizeof(*opt)) < 0)
					goto einval;
				sp->config = *opt;
				break;
			}
			case MTP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				mtp_opt_conf_rs_t *opt = (typeof(opt)) (arg + 1);
				if (!rs || (size -= sizeof(*opt)) < 0)
					goto einval;
				rs->config = *opt;
				break;
			}
			case MTP_OBJ_TYPE_RL:
			{
				struct rl *rl = rl_lookup(arg->id);
				mtp_opt_conf_rl_t *opt = (typeof(opt)) (arg + 1);
				if (!rl || (size -= sizeof(*opt)) < 0)
					goto einval;
				rl->config = *opt;
				break;
			}
			case MTP_OBJ_TYPE_RT:
			{
				struct rt *rt = rt_lookup(arg->id);
				mtp_opt_conf_rt_t *opt = (typeof(opt)) (arg + 1);
				if (!rt || (size -= sizeof(*opt)) < 0)
					goto einval;
				rt->config = *opt;
				break;
			}
			case MTP_OBJ_TYPE_LS:
			{
				struct ls *ls = ls_lookup(arg->id);
				mtp_opt_conf_ls_t *opt = (typeof(opt)) (arg + 1);
				if (!ls || (size -= sizeof(*opt)) < 0)
					goto einval;
				ls->config = *opt;
				break;
			}
			case MTP_OBJ_TYPE_LK:
			{
				struct lk *lk = lk_lookup(arg->id);
				mtp_opt_conf_lk_t *opt = (typeof(opt)) (arg + 1);
				if (!lk || (size -= sizeof(*opt)) < 0)
					goto einval;
				lk->config = *opt;
				break;
			}
			case MTP_OBJ_TYPE_SL:
			{
				struct sl *sl = sl_lookup(arg->id);
				mtp_opt_conf_sl_t *opt = (typeof(opt)) (arg + 1);
				if (!sl || (size -= sizeof(*opt)) < 0)
					goto einval;
				sl->config = *opt;
				break;
			}
			case MTP_OBJ_TYPE_DF:
			{
				struct df *df = df_lookup(arg->id);
				mtp_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
				if (!df || (size -= sizeof(*opt)) < 0)
					goto einval;
				df->config = *opt;
				break;
			}
			default:
				rare();
			      einval:
				return (-EINVAL);
			}
			return (QR_DONE);
		}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MTP_IOCGCONFIG  - struct mtp_config
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct mtp_config *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			return mtp_get_conf(arg, size);
	}
	rare();
	return (-EINVAL);
}

/*
 *  MTP_IOCSCONFIG  - struct mtp_config
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct mtp_config *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case MTP_ADD:
				return mtp_add_conf(arg, size, 0, 0);
			case MTP_CHA:
				return mtp_cha_conf(arg, size, 0, 0);
			case MTP_DEL:
				return mtp_del_conf(arg, size, 0, 0);
			default:
				rare();
				return (-EINVAL);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MTP_IOCTCONFIG  - struct mtp_config
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct mtp_config *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case MTP_ADD:
				return mtp_add_conf(arg, size, 0, 1);
			case MTP_CHA:
				return mtp_cha_conf(arg, size, 0, 1);
			case MTP_DEL:
				return mtp_del_conf(arg, size, 0, 1);
			default:
				rare();
				return (-EINVAL);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MTP_IOCCCONFIG  - struct mtp_config
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct mtp_config *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case MTP_ADD:
				return mtp_add_conf(arg, size, 1, 0);
			case MTP_CHA:
				return mtp_cha_conf(arg, size, 1, 0);
			case MTP_DEL:
				return mtp_del_conf(arg, size, 1, 0);
			default:
				rare();
				return (-EINVAL);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MTP_IOCGSTATEM  - struct mtp_statem
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct mtp_statem *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MTP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			mtp_statem_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = na->flags;
			arg->state = na->state;
			sta->timers = na->timers;
			break;
		}
		case MTP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			mtp_statem_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = sp->flags;
			arg->state = sp->state;
			sta->timers = sp->timers;
			break;
		}
		case MTP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			mtp_statem_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = rs->flags;
			arg->state = rs->state;
			sta->timers = rs->timers;
			break;
		}
		case MTP_OBJ_TYPE_RL:
		{
			struct rl *rl = rl_lookup(arg->id);
			mtp_statem_rl_t *sta = (typeof(sta)) (arg + 1);
			if (!rl || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = rl->flags;
			arg->state = rl->state;
			sta->timers = rl->timers;
			break;
		}
		case MTP_OBJ_TYPE_RT:
		{
			struct rt *rt = rt_lookup(arg->id);
			mtp_statem_rt_t *sta = (typeof(sta)) (arg + 1);
			if (!rt || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = rt->flags;
			arg->state = rt->state;
			sta->timers = rt->timers;
			break;
		}
		case MTP_OBJ_TYPE_LS:
		{
			struct ls *ls = ls_lookup(arg->id);
			mtp_statem_ls_t *sta = (typeof(sta)) (arg + 1);
			if (!ls || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = ls->flags;
			arg->state = ls->state;
			sta->timers = ls->timers;
			break;
		}
		case MTP_OBJ_TYPE_LK:
		{
			struct lk *lk = lk_lookup(arg->id);
			mtp_statem_lk_t *sta = (typeof(sta)) (arg + 1);
			if (!lk || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = lk->flags;
			arg->state = lk->state;
			sta->timers = lk->timers;
			break;
		}
		case MTP_OBJ_TYPE_SL:
		{
			struct sl *sl = sl_lookup(arg->id);
			mtp_statem_sl_t *sta = (typeof(sta)) (arg + 1);
			if (!sl || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = sl->flags;
			arg->state = sl->state;
			sta->timers = sl->timers;
			break;
		}
		case MTP_OBJ_TYPE_DF:
		{
			struct df *df = df_lookup(arg->id);
			mtp_statem_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			arg->flags = 0;
			arg->state = 0;
			sta->timers = df->timers;
			break;
		}
		default:
			rare();
		      einval:
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
 *  MTP_IOCCMRESET  - struct mtp_statem
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct mtp_statem *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		(void) arg;
		return (-EOPNOTSUPP);
	}
	rare();
	return (-EINVAL);
}

/*
 *  MTP_IOCGSTATSP  - struct mtp_stats
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct mtp_stats *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MTP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			mtp_stats_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = na->statsp;
			break;
		}
		case MTP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			mtp_stats_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sp->statsp;
			break;
		}
		case MTP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			mtp_stats_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = rs->statsp;
			break;
		}
		case MTP_OBJ_TYPE_RL:
		{
			struct rl *rl = rl_lookup(arg->id);
			mtp_stats_rl_t *sta = (typeof(sta)) (arg + 1);
			if (!rl || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = rl->statsp;
			break;
		}
		case MTP_OBJ_TYPE_RT:
		{
			struct rt *rt = rt_lookup(arg->id);
			mtp_stats_rt_t *sta = (typeof(sta)) (arg + 1);
			if (!rt || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = rt->statsp;
			break;
		}
		case MTP_OBJ_TYPE_LS:
		{
			struct ls *ls = ls_lookup(arg->id);
			mtp_stats_ls_t *sta = (typeof(sta)) (arg + 1);
			if (!ls || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = ls->statsp;
			break;
		}
		case MTP_OBJ_TYPE_LK:
		{
			struct lk *lk = lk_lookup(arg->id);
			mtp_stats_lk_t *sta = (typeof(sta)) (arg + 1);
			if (!lk || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = lk->statsp;
			break;
		}
		case MTP_OBJ_TYPE_SL:
		{
			struct sl *sl = sl_lookup(arg->id);
			mtp_stats_sl_t *sta = (typeof(sta)) (arg + 1);
			if (!sl || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sl->statsp;
			break;
		}
		case MTP_OBJ_TYPE_DF:
		{
			struct df *df = df_lookup(arg->id);
			mtp_stats_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = df->statsp;
			break;
		}
		default:
			rare();
		      einval:
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
 *  MTP_IOCSSTATSP  - struct mtp_stats
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct mtp_stats *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MTP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			mtp_stats_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			na->statsp = *sta;
			break;
		}
		case MTP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			mtp_stats_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			sp->statsp = *sta;
			break;
		}
		case MTP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			mtp_stats_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			rs->statsp = *sta;
			break;
		}
		case MTP_OBJ_TYPE_RL:
		{
			struct rl *rl = rl_lookup(arg->id);
			mtp_stats_rl_t *sta = (typeof(sta)) (arg + 1);
			if (!rl || (size -= sizeof(*sta)) < 0)
				goto einval;
			rl->statsp = *sta;
			break;
		}
		case MTP_OBJ_TYPE_RT:
		{
			struct rt *rt = rt_lookup(arg->id);
			mtp_stats_rt_t *sta = (typeof(sta)) (arg + 1);
			if (!rt || (size -= sizeof(*sta)) < 0)
				goto einval;
			rt->statsp = *sta;
			break;
		}
		case MTP_OBJ_TYPE_LS:
		{
			struct ls *ls = ls_lookup(arg->id);
			mtp_stats_ls_t *sta = (typeof(sta)) (arg + 1);
			if (!ls || (size -= sizeof(*sta)) < 0)
				goto einval;
			ls->statsp = *sta;
			break;
		}
		case MTP_OBJ_TYPE_LK:
		{
			struct lk *lk = lk_lookup(arg->id);
			mtp_stats_lk_t *sta = (typeof(sta)) (arg + 1);
			if (!lk || (size -= sizeof(*sta)) < 0)
				goto einval;
			lk->statsp = *sta;
			break;
		}
		case MTP_OBJ_TYPE_SL:
		{
			struct sl *sl = sl_lookup(arg->id);
			mtp_stats_sl_t *sta = (typeof(sta)) (arg + 1);
			if (!sl || (size -= sizeof(*sta)) < 0)
				goto einval;
			sl->statsp = *sta;
			break;
		}
		case MTP_OBJ_TYPE_DF:
		{
			struct df *df = df_lookup(arg->id);
			mtp_stats_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			df->statsp = *sta;
			break;
		}
		default:
			rare();
		      einval:
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
 *  MTP_IOCGSTATS   - struct mtp_stats
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct mtp_stats *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MTP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			mtp_stats_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = na->stats;
			arg->header = jiffies;
			break;
		}
		case MTP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			mtp_stats_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sp->stats;
			arg->header = jiffies;
			break;
		}
		case MTP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			mtp_stats_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = rs->stats;
			arg->header = jiffies;
			break;
		}
		case MTP_OBJ_TYPE_RL:
		{
			struct rl *rl = rl_lookup(arg->id);
			mtp_stats_rl_t *sta = (typeof(sta)) (arg + 1);
			if (!rl || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = rl->stats;
			arg->header = jiffies;
			break;
		}
		case MTP_OBJ_TYPE_RT:
		{
			struct rt *rt = rt_lookup(arg->id);
			mtp_stats_rt_t *sta = (typeof(sta)) (arg + 1);
			if (!rt || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = rt->stats;
			arg->header = jiffies;
			break;
		}
		case MTP_OBJ_TYPE_LS:
		{
			struct ls *ls = ls_lookup(arg->id);
			mtp_stats_ls_t *sta = (typeof(sta)) (arg + 1);
			if (!ls || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = ls->stats;
			arg->header = jiffies;
			break;
		}
		case MTP_OBJ_TYPE_LK:
		{
			struct lk *lk = lk_lookup(arg->id);
			mtp_stats_lk_t *sta = (typeof(sta)) (arg + 1);
			if (!lk || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = lk->stats;
			arg->header = jiffies;
			break;
		}
		case MTP_OBJ_TYPE_SL:
		{
			struct sl *sl = sl_lookup(arg->id);
			mtp_stats_sl_t *sta = (typeof(sta)) (arg + 1);
			if (!sl || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = sl->stats;
			arg->header = jiffies;
			break;
		}
		case MTP_OBJ_TYPE_DF:
		{
			struct df *df = df_lookup(arg->id);
			mtp_stats_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			*sta = df->stats;
			arg->header = jiffies;
			break;
		}
		default:
			rare();
		      einval:
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
 *  MTP_IOCSSTATS   - struct mtp_stats
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_iocsstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		struct mtp_stats *arg = (typeof(arg)) mp->b_cont->b_rptr;
		uchar *d, *s = (typeof(s)) (arg + 1);
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		arg->header = jiffies;
		switch (arg->type) {
		case MTP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			mtp_stats_na_t *sta = (typeof(sta)) (arg + 1);
			if (!na || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & na->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case MTP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			mtp_stats_sp_t *sta = (typeof(sta)) (arg + 1);
			if (!sp || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & sp->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case MTP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			mtp_stats_rs_t *sta = (typeof(sta)) (arg + 1);
			if (!rs || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & rs->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case MTP_OBJ_TYPE_RL:
		{
			struct rl *rl = rl_lookup(arg->id);
			mtp_stats_rl_t *sta = (typeof(sta)) (arg + 1);
			if (!rl || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & rl->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case MTP_OBJ_TYPE_RT:
		{
			struct rt *rt = rt_lookup(arg->id);
			mtp_stats_rt_t *sta = (typeof(sta)) (arg + 1);
			if (!rt || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & rt->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case MTP_OBJ_TYPE_LS:
		{
			struct ls *ls = ls_lookup(arg->id);
			mtp_stats_ls_t *sta = (typeof(sta)) (arg + 1);
			if (!ls || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & ls->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case MTP_OBJ_TYPE_LK:
		{
			struct lk *lk = lk_lookup(arg->id);
			mtp_stats_lk_t *sta = (typeof(sta)) (arg + 1);
			if (!lk || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & lk->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case MTP_OBJ_TYPE_SL:
		{
			struct sl *sl = sl_lookup(arg->id);
			mtp_stats_sl_t *sta = (typeof(sta)) (arg + 1);
			if (!sl || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & sl->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		case MTP_OBJ_TYPE_DF:
		{
			struct df *df = df_lookup(arg->id);
			mtp_stats_df_t *sta = (typeof(sta)) (arg + 1);
			if (!df || (size -= sizeof(*sta)) < 0)
				goto einval;
			d = (typeof(d)) & df->stats;
			for (size = sizeof(*sta); size--; *d &= *s) ;
			break;
		}
		default:
			rare();
		      einval:
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
 *  MTP_IOCGNOTIFY  - struct mtp_notify
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct mtp_notify *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case MTP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				mtp_notify_na_t *not = (typeof(not)) (arg + 1);
				if (!na || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = na->notify.events;
				break;
			}
			case MTP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				mtp_notify_sp_t *not = (typeof(not)) (arg + 1);
				if (!sp || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = sp->notify.events;
				break;
			}
			case MTP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				mtp_notify_rs_t *not = (typeof(not)) (arg + 1);
				if (!rs || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = rs->notify.events;
				break;
			}
			case MTP_OBJ_TYPE_RL:
			{
				struct rl *rl = rl_lookup(arg->id);
				mtp_notify_rl_t *not = (typeof(not)) (arg + 1);
				if (!rl || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = rl->notify.events;
				break;
			}
			case MTP_OBJ_TYPE_RT:
			{
				struct rt *rt = rt_lookup(arg->id);
				mtp_notify_rt_t *not = (typeof(not)) (arg + 1);
				if (!rt || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = rt->notify.events;
				break;
			}
			case MTP_OBJ_TYPE_LS:
			{
				struct ls *ls = ls_lookup(arg->id);
				mtp_notify_ls_t *not = (typeof(not)) (arg + 1);
				if (!ls || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = ls->notify.events;
				break;
			}
			case MTP_OBJ_TYPE_LK:
			{
				struct lk *lk = lk_lookup(arg->id);
				mtp_notify_lk_t *not = (typeof(not)) (arg + 1);
				if (!lk || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = lk->notify.events;
				break;
			}
			case MTP_OBJ_TYPE_SL:
			{
				struct sl *sl = sl_lookup(arg->id);
				mtp_notify_sl_t *not = (typeof(not)) (arg + 1);
				if (!sl || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = sl->notify.events;
				break;
			}
			case MTP_OBJ_TYPE_DF:
			{
				struct df *df = df_lookup(arg->id);
				mtp_notify_df_t *not = (typeof(not)) (arg + 1);
				if (!df || (size -= sizeof(*not)) < 0)
					goto einval;
				not->events = df->notify.events;
				break;
			}
			default:
				rare();
				goto einval;
			}
			return (QR_DONE);
		}
	}
	rare();
      einval:
	return (-EINVAL);
}

/*
 *  MTP_IOCSNOTIFY  - struct mtp_notify
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct mtp_notify *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case MTP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				mtp_notify_na_t *not = (typeof(not)) (arg + 1);
				if (!na || (size -= sizeof(*not)) < 0)
					goto einval;
				na->notify.events |= not->events;
				break;
			}
			case MTP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				mtp_notify_sp_t *not = (typeof(not)) (arg + 1);
				if (!sp || (size -= sizeof(*not)) < 0)
					goto einval;
				sp->notify.events |= not->events;
				break;
			}
			case MTP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				mtp_notify_rs_t *not = (typeof(not)) (arg + 1);
				if (!rs || (size -= sizeof(*not)) < 0)
					goto einval;
				rs->notify.events |= not->events;
				break;
			}
			case MTP_OBJ_TYPE_RL:
			{
				struct rl *rl = rl_lookup(arg->id);
				mtp_notify_rl_t *not = (typeof(not)) (arg + 1);
				if (!rl || (size -= sizeof(*not)) < 0)
					goto einval;
				rl->notify.events |= not->events;
				break;
			}
			case MTP_OBJ_TYPE_RT:
			{
				struct rt *rt = rt_lookup(arg->id);
				mtp_notify_rt_t *not = (typeof(not)) (arg + 1);
				if (!rt || (size -= sizeof(*not)) < 0)
					goto einval;
				rt->notify.events |= not->events;
				break;
			}
			case MTP_OBJ_TYPE_LS:
			{
				struct ls *ls = ls_lookup(arg->id);
				mtp_notify_ls_t *not = (typeof(not)) (arg + 1);
				if (!ls || (size -= sizeof(*not)) < 0)
					goto einval;
				ls->notify.events |= not->events;
				break;
			}
			case MTP_OBJ_TYPE_LK:
			{
				struct lk *lk = lk_lookup(arg->id);
				mtp_notify_lk_t *not = (typeof(not)) (arg + 1);
				if (!lk || (size -= sizeof(*not)) < 0)
					goto einval;
				lk->notify.events |= not->events;
				break;
			}
			case MTP_OBJ_TYPE_SL:
			{
				struct sl *sl = sl_lookup(arg->id);
				mtp_notify_sl_t *not = (typeof(not)) (arg + 1);
				if (!sl || (size -= sizeof(*not)) < 0)
					goto einval;
				sl->notify.events |= not->events;
				break;
			}
			case MTP_OBJ_TYPE_DF:
			{
				struct df *df = df_lookup(arg->id);
				mtp_notify_df_t *not = (typeof(not)) (arg + 1);
				if (!df || (size -= sizeof(*not)) < 0)
					goto einval;
				df->notify.events |= not->events;
				break;
			}
			default:
				rare();
				goto einval;
			}
			return (QR_DONE);
		}
	}
	rare();
      einval:
	return (-EINVAL);
}

/*
 *  MTP_IOCCNOTIFY  - struct mtp_notify
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		int size = msgdsize(mp);
		struct mtp_notify *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0) {
			switch (arg->type) {
			case MTP_OBJ_TYPE_NA:
			{
				struct na *na = na_lookup(arg->id);
				mtp_notify_na_t *not = (typeof(not)) (arg + 1);
				if (!na || (size -= sizeof(*not)) < 0)
					goto einval;
				na->notify.events &= ~not->events;
				break;
			}
			case MTP_OBJ_TYPE_SP:
			{
				struct sp *sp = sp_lookup(arg->id);
				mtp_notify_sp_t *not = (typeof(not)) (arg + 1);
				if (!sp || (size -= sizeof(*not)) < 0)
					goto einval;
				sp->notify.events &= ~not->events;
				break;
			}
			case MTP_OBJ_TYPE_RS:
			{
				struct rs *rs = rs_lookup(arg->id);
				mtp_notify_rs_t *not = (typeof(not)) (arg + 1);
				if (!rs || (size -= sizeof(*not)) < 0)
					goto einval;
				rs->notify.events &= ~not->events;
				break;
			}
			case MTP_OBJ_TYPE_RL:
			{
				struct rl *rl = rl_lookup(arg->id);
				mtp_notify_rl_t *not = (typeof(not)) (arg + 1);
				if (!rl || (size -= sizeof(*not)) < 0)
					goto einval;
				rl->notify.events &= ~not->events;
				break;
			}
			case MTP_OBJ_TYPE_RT:
			{
				struct rt *rt = rt_lookup(arg->id);
				mtp_notify_rt_t *not = (typeof(not)) (arg + 1);
				if (!rt || (size -= sizeof(*not)) < 0)
					goto einval;
				rt->notify.events &= ~not->events;
				break;
			}
			case MTP_OBJ_TYPE_LS:
			{
				struct ls *ls = ls_lookup(arg->id);
				mtp_notify_ls_t *not = (typeof(not)) (arg + 1);
				if (!ls || (size -= sizeof(*not)) < 0)
					goto einval;
				ls->notify.events &= ~not->events;
				break;
			}
			case MTP_OBJ_TYPE_LK:
			{
				struct lk *lk = lk_lookup(arg->id);
				mtp_notify_lk_t *not = (typeof(not)) (arg + 1);
				if (!lk || (size -= sizeof(*not)) < 0)
					goto einval;
				lk->notify.events &= ~not->events;
				break;
			}
			case MTP_OBJ_TYPE_SL:
			{
				struct sl *sl = sl_lookup(arg->id);
				mtp_notify_sl_t *not = (typeof(not)) (arg + 1);
				if (!sl || (size -= sizeof(*not)) < 0)
					goto einval;
				sl->notify.events &= ~not->events;
				break;
			}
			case MTP_OBJ_TYPE_DF:
			{
				struct df *df = df_lookup(arg->id);
				mtp_notify_df_t *not = (typeof(not)) (arg + 1);
				if (!df || (size -= sizeof(*not)) < 0)
					goto einval;
				df->notify.events &= ~not->events;
				break;
			}
			default:
				rare();
				goto einval;
			}
		}
	}
	rare();
      einval:
	return (-EINVAL);
}

/*
 *  MTP_IOCCMGMT    - mtp_mgmt_t
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_ioccmgmt(queue_t *q, mblk_t *mp)
{
	int err;
	if (mp->b_cont) {
		mtp_mgmt_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		switch (arg->type) {
		case MTP_OBJ_TYPE_NA:
		{
			struct na *na = na_lookup(arg->id);
			if (!na)
				goto einval;
			if ((err = mtp_mgmt_na(q, na, arg->cmd)))
				goto error;
			break;
		}
		case MTP_OBJ_TYPE_SP:
		{
			struct sp *sp = sp_lookup(arg->id);
			if (!sp)
				goto einval;
			if ((err = mtp_mgmt_sp(q, sp, arg->cmd)))
				goto error;
			break;
		}
		case MTP_OBJ_TYPE_RS:
		{
			struct rs *rs = rs_lookup(arg->id);
			if (!rs)
				goto einval;
			if ((err = mtp_mgmt_rs(q, rs, arg->cmd)))
				goto error;
			break;
		}
		case MTP_OBJ_TYPE_RL:
		{
			struct rl *rl = rl_lookup(arg->id);
			if (!rl)
				goto einval;
			if ((err = mtp_mgmt_rl(q, rl, arg->cmd)))
				goto error;
			break;
		}
		case MTP_OBJ_TYPE_RT:
		{
			struct rt *rt = rt_lookup(arg->id);
			if (!rt)
				goto einval;
			if ((err = mtp_mgmt_rt(q, rt, arg->cmd)))
				goto error;
			break;
		}
		case MTP_OBJ_TYPE_LS:
		{
			struct ls *ls = ls_lookup(arg->id);
			if (!ls)
				goto einval;
			if ((err = mtp_mgmt_ls(q, ls, arg->cmd)))
				goto error;
			break;
		}
		case MTP_OBJ_TYPE_LK:
		{
			struct lk *lk = lk_lookup(arg->id);
			if (!lk)
				goto einval;
			if ((err = mtp_mgmt_lk(q, lk, arg->cmd)))
				goto error;
			break;
		}
		case MTP_OBJ_TYPE_SL:
		{
			struct sl *sl = sl_lookup(arg->id);
			if (!sl)
				goto einval;
			if ((err = mtp_mgmt_sl(q, sl, arg->cmd)))
				goto error;
			break;
		}
		case MTP_OBJ_TYPE_DF:
		{
			struct df *df = df_lookup(arg->id);
			if (!df)
				goto einval;
			if ((err = mtp_mgmt_df(q, df, arg->cmd)))
				goto error;
			break;
		}
		default:
			rare();
		      einval:
			return (-EINVAL);
		}
		return (QR_DONE);
	}
	rare();
	return (-EINVAL);
      error:
	rare();
	return (err);
}

/*
 *  MTP_IOCCPASS    - mtp_pass_t
 *  -------------------------------------------------------------------
 */
STATIC int
mtp_ioccpass(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		mtp_pass_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		mblk_t *bp, *dp;
		struct sl *sl;
		for (sl = master.sl.list; sl && sl->u.mux.index != arg->muxid; sl = sl->next) ;
		if (!sl || !sl->oq)
			return (-EINVAL);
		if (arg->type < QPCTL && !canput(sl->oq))
			return (-EBUSY);
		if (!(bp = ss7_dupb(q, mp)))
			return (-ENOBUFS);
		if (!(dp = ss7_dupb(q, mp))) {
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
		ss7_oput(sl->oq, bp);
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
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
mtp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;
	(void) mtp;
	switch (type) {
	case _IOC_TYPE(__SID):
	{
		struct sl *sl;
		struct linkblk *lb;
		if (!(lb = arg)) {
			swerr();
			ret = (-EINVAL);
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n", DRV_NAME,
					mtp));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			if ((sl =
			     mtp_alloc_link(lb->l_qbot, &master.sl.list, lb->l_index,
					    iocp->ioc_cr)))
				break;
			ret = -ENOMEM;
			break;
		case _IOC_NR(I_PUNLINK):
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n", DRV_NAME,
					mtp));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			for (sl = master.sl.list; sl; sl = sl->next)
				if (sl->u.mux.index == lb->l_index)
					break;
			if (!sl) {
				ret = -EINVAL;
				ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n", DRV_NAME,
					mtp));
				break;
			}
			mtp_free_link(sl);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %d\n", DRV_NAME, mtp,
				nr));
			ret = (-EOPNOTSUPP);
			break;
		}
		break;
	}
	case MTP_IOC_MAGIC:
	{
		if (count < size) {
			ret = (-EINVAL);
			break;
		}
		switch (nr) {
		case _IOC_NR(MTP_IOCGOPTION):	/* struct lmi_option */
			printd(("%s: %p: -> MTP_IOCGOPTION\n", DRV_NAME, mtp));
			ret = mtp_iocgoptions(q, mp);
			break;
		case _IOC_NR(MTP_IOCSOPTION):	/* struct lmi_option */
			printd(("%s: %p: -> MTP_IOCSOPTION\n", DRV_NAME, mtp));
			ret = mtp_iocsoptions(q, mp);
			break;
		case _IOC_NR(MTP_IOCGCONFIG):	/* struct mtp_config */
			printd(("%s: %p: -> MTP_IOCGCONFIG\n", DRV_NAME, mtp));
			ret = mtp_iocgconfig(q, mp);
			break;
		case _IOC_NR(MTP_IOCSCONFIG):	/* struct mtp_config */
			printd(("%s: %p: -> MTP_IOCSCONFIG\n", DRV_NAME, mtp));
			ret = mtp_iocsconfig(q, mp);
			break;
		case _IOC_NR(MTP_IOCTCONFIG):	/* struct mtp_config */
			printd(("%s: %p: -> MTP_IOCTCONFIG\n", DRV_NAME, mtp));
			ret = mtp_ioctconfig(q, mp);
			break;
		case _IOC_NR(MTP_IOCCCONFIG):	/* struct mtp_config */
			printd(("%s: %p: -> MTP_IOCCCONFIG\n", DRV_NAME, mtp));
			ret = mtp_ioccconfig(q, mp);
			break;
		case _IOC_NR(MTP_IOCGSTATEM):	/* struct mtp_statem */
			printd(("%s: %p: -> MTP_IOCGSTATEM\n", DRV_NAME, mtp));
			ret = mtp_iocgstatem(q, mp);
			break;
		case _IOC_NR(MTP_IOCCMRESET):	/* struct mtp_statem */
			printd(("%s: %p: -> MTP_IOCCMRESET\n", DRV_NAME, mtp));
			ret = mtp_ioccmreset(q, mp);
			break;
		case _IOC_NR(MTP_IOCGSTATSP):	/* struct mtp_stats */
			printd(("%s: %p: -> MTP_IOCGSTATSP\n", DRV_NAME, mtp));
			ret = mtp_iocgstatsp(q, mp);
			break;
		case _IOC_NR(MTP_IOCSSTATSP):	/* struct mtp_stats */
			printd(("%s: %p: -> MTP_IOCSSTATSP\n", DRV_NAME, mtp));
			ret = mtp_iocsstatsp(q, mp);
			break;
		case _IOC_NR(MTP_IOCGSTATS):	/* struct mtp_stats */
			printd(("%s: %p: -> MTP_IOCGSTATS\n", DRV_NAME, mtp));
			ret = mtp_iocgstats(q, mp);
			break;
		case _IOC_NR(MTP_IOCSSTATS):	/* struct mtp_stats */
			printd(("%s: %p: -> MTP_IOCSSTATS\n", DRV_NAME, mtp));
			ret = mtp_iocsstats(q, mp);
			break;
		case _IOC_NR(MTP_IOCGNOTIFY):	/* struct mtp_notify */
			printd(("%s: %p: -> MTP_IOCGNOTIFY\n", DRV_NAME, mtp));
			ret = mtp_iocgnotify(q, mp);
			break;
		case _IOC_NR(MTP_IOCSNOTIFY):	/* struct mtp_notify */
			printd(("%s: %p: -> MTP_IOCSNOTIFY\n", DRV_NAME, mtp));
			ret = mtp_iocsnotify(q, mp);
			break;
		case _IOC_NR(MTP_IOCCNOTIFY):	/* struct mtp_notify */
			printd(("%s: %p: -> MTP_IOCCNOTIFY\n", DRV_NAME, mtp));
			ret = mtp_ioccnotify(q, mp);
			break;
		case _IOC_NR(MTP_IOCCMGMT):	/* struct mtp_ctl */
			printd(("%s: %p: -> MTP_IOCCMGMT\n", DRV_NAME, mtp));
			ret = mtp_ioccmgmt(q, mp);
			break;
		case _IOC_NR(MTP_IOCCPASS):	/* ulong */
			printd(("%s: %p: -> MTP_IOCCPASS\n", DRV_NAME, mtp));
			ret = mtp_ioccpass(q, mp);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported MTP ioctl %d\n", DRV_NAME, mtp, nr));
			ret = (-EOPNOTSUPP);
			break;
		}
		break;
	}
		/* 
		   TODO: Need to add standard TPI/NPI ioctls */
	default:
		ret = (-EOPNOTSUPP);
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
STATIC int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sl *sl = SL_PRIV(q);
	ulong oldstate = sl_get_l_state(sl);
	if ((prim = *(ulong *) mp->b_rptr) == SL_PDU_IND) {
		printd(("%s: %p: SL_PDU_IND [%d] <-\n", DRV_NAME, sl, msgdsize(mp->b_cont)));
		if ((rtn = sl_pdu_ind(q, mp)) < 0)
			sl_set_l_state(sl, oldstate);
		return (rtn);
	}
	switch (prim) {
	case SL_PDU_IND:
		printd(("%s: %p: SL_PDU_IND [%d] <-\n", DRV_NAME, sl, msgdsize(mp->b_cont)));
		rtn = sl_pdu_ind(q, mp);
		break;
	case SL_LINK_CONGESTED_IND:
		printd(("%s: %p: SL_LINK_CONGESTED_IND <-\n", DRV_NAME, sl));
		rtn = sl_link_congested_ind(q, mp);
		break;
	case SL_LINK_CONGESTION_CEASED_IND:
		printd(("%s: %p: SL_LINK_CONGESTION_CEASED_IND <-\n", DRV_NAME, sl));
		rtn = sl_link_congestion_ceased_ind(q, mp);
		break;
	case SL_RETRIEVED_MESSAGE_IND:
		printd(("%s: %p: SL_RETRIEVED_MESSAGE_IND <-\n", DRV_NAME, sl));
		rtn = sl_retrieved_message_ind(q, mp);
		break;
	case SL_RETRIEVAL_COMPLETE_IND:
		printd(("%s: %p: SL_RETRIEVAL_COMPLETE_IND <-\n", DRV_NAME, sl));
		rtn = sl_retrieval_complete_ind(q, mp);
		break;
	case SL_RB_CLEARED_IND:
		printd(("%s: %p: SL_RB_CLEARED_IND <-\n", DRV_NAME, sl));
		rtn = sl_rb_cleared_ind(q, mp);
		break;
	case SL_BSNT_IND:
		printd(("%s: %p: SL_BSNT_IND <-\n", DRV_NAME, sl));
		rtn = sl_bsnt_ind(q, mp);
		break;
	case SL_IN_SERVICE_IND:
		printd(("%s: %p: SL_IN_SERVICE_IND <-\n", DRV_NAME, sl));
		rtn = sl_in_service_ind(q, mp);
		break;
	case SL_OUT_OF_SERVICE_IND:
		printd(("%s: %p: SL_OUT_OF_SERVICE_IND <-\n", DRV_NAME, sl));
		rtn = sl_out_of_service_ind(q, mp);
		break;
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		printd(("%s: %p: SL_REMOTE_PROCESSOR_OUTAGE_IND <-\n", DRV_NAME, sl));
		rtn = sl_remote_processor_outage_ind(q, mp);
		break;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		printd(("%s: %p: SL_REMOTE_PROCESSOR_RECOVERED_IND <-\n", DRV_NAME, sl));
		rtn = sl_remote_processor_recovered_ind(q, mp);
		break;
	case SL_RTB_CLEARED_IND:
		printd(("%s: %p: SL_RTB_CLEARED_IND <-\n", DRV_NAME, sl));
		rtn = sl_rtb_cleared_ind(q, mp);
		break;
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		printd(("%s: %p: SL_RETRIEVAL_NOT_POSSIBLE_IND <-\n", DRV_NAME, sl));
		rtn = sl_retrieval_not_possible_ind(q, mp);
		break;
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		printd(("%s: %p: SL_BSNT_NOT_RETRIEVABLE_IND <-\n", DRV_NAME, sl));
		rtn = sl_bsnt_not_retrievable_ind(q, mp);
		break;
#if 0
	case SL_OPTMGMT_ACK:
		printd(("%s: %p: SL_OPTMGMT_ACK <-\n", DRV_NAME, sl));
		rtn = sl_optmgmt_ack(q, mp);
		break;
	case SL_NOTIFY_IND:
		printd(("%s: %p: SL_NOTIFY_IND <-\n", DRV_NAME, sl));
		rtn = sl_notify_ind(q, mp);
		break;
#endif
	case LMI_INFO_ACK:
		printd(("%s: %p: LMI_INFO_ACK <-\n", DRV_NAME, sl));
		rtn = lmi_info_ack(q, mp);
		break;
	case LMI_OK_ACK:
		printd(("%s: %p: LMI_OK_ACK <-\n", DRV_NAME, sl));
		rtn = lmi_ok_ack(q, mp);
		break;
	case LMI_ERROR_ACK:
		printd(("%s: %p: LMI_ERROR_ACK <-\n", DRV_NAME, sl));
		rtn = lmi_error_ack(q, mp);
		break;
	case LMI_ENABLE_CON:
		printd(("%s: %p: LMI_ENABLE_CON <-\n", DRV_NAME, sl));
		rtn = lmi_enable_con(q, mp);
		break;
	case LMI_DISABLE_CON:
		printd(("%s: %p: LMI_DISABLE_CON <-\n", DRV_NAME, sl));
		rtn = lmi_disable_con(q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		printd(("%s: %p: LMI_OPTMGMT_ACK <-\n", DRV_NAME, sl));
		rtn = lmi_optmgmt_ack(q, mp);
		break;
	case LMI_ERROR_IND:
		printd(("%s: %p: LMI_ERROR_IND <-\n", DRV_NAME, sl));
		rtn = lmi_error_ind(q, mp);
		break;
	case LMI_STATS_IND:
		printd(("%s: %p: LMI_STATS_IND <-\n", DRV_NAME, sl));
		rtn = lmi_stats_ind(q, mp);
		break;
	case LMI_EVENT_IND:
		printd(("%s: %p: LMI_EVENT_IND <-\n", DRV_NAME, sl));
		rtn = lmi_event_ind(q, mp);
		break;
	default:
		/* 
		   reject what we don't recognize */
		printd(("%s: %p: ???? <-\n", DRV_NAME, sl));
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		sl_set_l_state(sl, oldstate);
	return (rtn);
}

STATIC int
mtp_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct mtp *mtp = MTP_PRIV(q);
	ulong oldstate = mtp_get_state(mtp);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case MTP_BIND_REQ:
		printd(("%s: %p: -> MTP_BIND_REQ\n", DRV_NAME, mtp));
		rtn = m_bind_req(q, mp);
		break;
	case MTP_UNBIND_REQ:
		printd(("%s: %p: -> MTP_UNBIND_REQ\n", DRV_NAME, mtp));
		rtn = m_unbind_req(q, mp);
		break;
	case MTP_CONN_REQ:
		printd(("%s: %p: -> MTP_CONN_REQ\n", DRV_NAME, mtp));
		rtn = m_conn_req(q, mp);
		break;
	case MTP_DISCON_REQ:
		printd(("%s: %p: -> MTP_DISCON_REQ\n", DRV_NAME, mtp));
		rtn = m_discon_req(q, mp);
		break;
	case MTP_ADDR_REQ:
		printd(("%s: %p: -> MTP_ADDR_REQ\n", DRV_NAME, mtp));
		rtn = m_addr_req(q, mp);
		break;
	case MTP_INFO_REQ:
		printd(("%s: %p: -> MTP_INFO_REQ\n", DRV_NAME, mtp));
		rtn = m_info_req(q, mp);
		break;
	case MTP_OPTMGMT_REQ:
		printd(("%s: %p: -> MTP_OPTMGMT_REQ\n", DRV_NAME, mtp));
		rtn = m_optmgmt_req(q, mp);
		break;
	case MTP_TRANSFER_REQ:
		printd(("%s: %p: -> MTP_TRANSFER_REQ\n", DRV_NAME, mtp));
		rtn = m_transfer_req(q, mp);
		break;
	default:
		rtn = m_error_ack(q, mtp, prim, -EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		mtp_set_state(mtp, oldstate);
	return (rtn);
}

STATIC int
tpi_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct mtp *mtp = MTP_PRIV(q);
	ulong oldstate = mtp_get_state(mtp);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case T_CONN_REQ:
		printd(("%s: %p: -> T_CONN_REQ\n", DRV_NAME, mtp));
		rtn = t_conn_req(q, mp);
		break;
	case T_CONN_RES:
		printd(("%s: %p: -> T_CONN_RES\n", DRV_NAME, mtp));
		rtn = t_error_ack(q, mtp, prim, TNOTSUPPORT);
		break;
	case T_DISCON_REQ:
		printd(("%s: %p: -> T_DISCON_REQ\n", DRV_NAME, mtp));
		rtn = t_discon_req(q, mp);
		break;
	case T_DATA_REQ:
		printd(("%s: %p: -> T_DATA_REQ\n", DRV_NAME, mtp));
		rtn = t_data_req(q, mp);
		break;
	case T_EXDATA_REQ:
		printd(("%s: %p: -> T_EXDATA_REQ\n", DRV_NAME, mtp));
		rtn = t_exdata_req(q, mp);
		break;
	case T_INFO_REQ:
		printd(("%s: %p: -> T_INFO_REQ\n", DRV_NAME, mtp));
		rtn = t_info_req(q, mp);
		break;
	case T_BIND_REQ:
		printd(("%s: %p: -> T_BIND_REQ\n", DRV_NAME, mtp));
		rtn = t_bind_req(q, mp);
		break;
	case T_UNBIND_REQ:
		printd(("%s: %p: -> T_UNBIND_REQ\n", DRV_NAME, mtp));
		rtn = t_unbind_req(q, mp);
		break;
	case T_UNITDATA_REQ:
		printd(("%s: %p: -> T_UNITDATA_REQ\n", DRV_NAME, mtp));
		rtn = t_unitdata_req(q, mp);
		break;
	case T_OPTMGMT_REQ:
		printd(("%s: %p: -> T_OPTMGMT_REQ\n", DRV_NAME, mtp));
		rtn = t_optmgmt_req(q, mp);
		break;
	case T_ORDREL_REQ:
		printd(("%s: %p: -> T_ORDREL_REQ\n", DRV_NAME, mtp));
		rtn = t_ordrel_req(q, mp);
		break;
	case T_OPTDATA_REQ:
		printd(("%s: %p: -> T_OPTDATA_REQ\n", DRV_NAME, mtp));
		rtn = t_optdata_req(q, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		printd(("%s: %p: -> T_ADDR_REQ\n", DRV_NAME, mtp));
		rtn = t_addr_req(q, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		printd(("%s: %p: -> T_CAPABILITY_REQ\n", DRV_NAME, mtp));
		rtn = t_capability_req(q, mp);
		break;
#endif
	default:
		rtn = t_error_ack(q, mtp, prim, TNOTSUPPORT);
		break;
	}
	if (rtn < 0)
		mtp_set_state(mtp, oldstate);
	return (rtn);
}

STATIC int
npi_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct mtp *mtp = MTP_PRIV(q);
	ulong oldstate = mtp_get_state(mtp);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case N_CONN_REQ:
		printd(("%s: %p: -> N_CONN_REQ\n", DRV_NAME, mtp));
		rtn = n_conn_req(q, mp);
		break;
	case N_CONN_RES:
		printd(("%s: %p: -> N_CONN_RES\n", DRV_NAME, mtp));
		rtn = n_conn_res(q, mp);
		break;
	case N_DISCON_REQ:
		printd(("%s: %p: -> N_DISCON_REQ\n", DRV_NAME, mtp));
		rtn = n_discon_req(q, mp);
		break;
	case N_DATA_REQ:
		printd(("%s: %p: -> N_DATA_REQ\n", DRV_NAME, mtp));
		rtn = n_data_req(q, mp);
		break;
	case N_EXDATA_REQ:
		printd(("%s: %p: -> N_EXDATA_REQ\n", DRV_NAME, mtp));
		rtn = n_exdata_req(q, mp);
		break;
	case N_INFO_REQ:
		printd(("%s: %p: -> N_INFO_REQ\n", DRV_NAME, mtp));
		rtn = n_info_req(q, mp);
		break;
	case N_BIND_REQ:
		printd(("%s: %p: -> N_BIND_REQ\n", DRV_NAME, mtp));
		rtn = n_bind_req(q, mp);
		break;
	case N_UNBIND_REQ:
		printd(("%s: %p: -> N_UNBIND_REQ\n", DRV_NAME, mtp));
		rtn = n_unbind_req(q, mp);
		break;
	case N_UNITDATA_REQ:
		printd(("%s: %p: -> N_UNITDATA_REQ\n", DRV_NAME, mtp));
		rtn = n_unitdata_req(q, mp);
		break;
	case N_OPTMGMT_REQ:
		printd(("%s: %p: -> N_OPTMGMT_REQ\n", DRV_NAME, mtp));
		rtn = n_optmgmt_req(q, mp);
		break;
	case N_DATACK_REQ:
		printd(("%s: %p: -> N_DATACK_REQ\n", DRV_NAME, mtp));
		rtn = n_datack_req(q, mp);
		break;
	case N_RESET_REQ:
		printd(("%s: %p: -> N_RESET_REQ\n", DRV_NAME, mtp));
		rtn = n_reset_req(q, mp);
		break;
	case N_RESET_RES:
		printd(("%s: %p: -> N_RESET_RES\n", DRV_NAME, mtp));
		rtn = n_reset_res(q, mp);
		break;
	default:
		rtn = n_error_ack(q, mtp, prim, NNOTSUPPORT);
		break;
	}
	if (rtn < 0)
		mtp_set_state(mtp, oldstate);
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
mtp_w_data(queue_t *q, mblk_t *mp)
{
	return m_data(q, mp);
}
STATIC int
tpi_w_data(queue_t *q, mblk_t *mp)
{
	return t_data(q, mp);
}
STATIC int
npi_w_data(queue_t *q, mblk_t *mp)
{
	return n_data(q, mp);
}
STATIC int
sl_r_data(queue_t *q, mblk_t *mp)
{
	return sl_data(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a signalling link has failed badly.  Move link to the out-of-service state,
 *  notify management, and perform normal link failure actions.
 */
STATIC int
sl_r_error(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_set_i_state(sl, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a signalling link has failed badly.  Move link to the out-of-service state,
 *  notify management, and perform normal link failure actions.
 */
STATIC int
sl_r_hangup(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	sl_set_i_state(sl, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC int
mtp_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC int
mtp_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mtp_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mtp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
tpi_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return tpi_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return tpi_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tpi_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
npi_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return npi_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return npi_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return npi_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
mgm_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mtp_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mtp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
sl_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sl_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return sl_r_error(q, mp);
	case M_HANGUP:
		return sl_r_hangup(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC int
sl_w_prim(queue_t *q, mblk_t *mp)
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
 *
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int mtp_majors[MTP_CMAJORS] = { MTP_CMAJOR_0, };
STATIC int
mtp_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	minor_t bminor = cminor;
	struct mtp *mtp, **mtpp = &master.mtp.list;
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
	if (cmajor != MTP_CMAJOR_0 || cminor >= MTP_CMINOR_FREE) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/* 
	   allocate a new device */
	cminor = MTP_CMINOR_FREE;
	spin_lock_irqsave(&master.lock, flags);
	for (; *mtpp; mtpp = &(*mtpp)->next) {
		major_t dmajor = (*mtpp)->u.dev.cmajor;
		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*mtpp)->u.dev.cminor;
			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= MTP_CMAJORS
					    || !(cmajor = mtp_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= MTP_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(mtp = mtp_alloc_priv(q, mtpp, devp, crp, bminor))) {
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
mtp_close(queue_t *q, int flag, cred_t *crp)
{
	struct mtp *mtp = MTP_PRIV(q);
	psw_t flags;
	(void) flag;
	(void) crp;
	(void) mtp;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, mtp->u.dev.cmajor,
		mtp->u.dev.cminor));
	spin_lock_irqsave(&master.lock, flags);
	mtp_free_priv(mtp);
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
STATIC kmem_cache_t *mtp_mt_cachep = NULL;	/* MTP User cache */
STATIC kmem_cache_t *mtp_na_cachep = NULL;	/* Network Appearance cache */
STATIC kmem_cache_t *mtp_sp_cachep = NULL;	/* Signalling Point cache */
STATIC kmem_cache_t *mtp_rs_cachep = NULL;	/* Route Set cache */
STATIC kmem_cache_t *mtp_cr_cachep = NULL;	/* Controlled Rerouting Buffer cache */
STATIC kmem_cache_t *mtp_rl_cachep = NULL;	/* Route List cache */
STATIC kmem_cache_t *mtp_rt_cachep = NULL;	/* Route cache */
STATIC kmem_cache_t *mtp_cb_cachep = NULL;	/* Changeback Buffer cache */
STATIC kmem_cache_t *mtp_ls_cachep = NULL;	/* (Combined) Link Set cache */
STATIC kmem_cache_t *mtp_lk_cachep = NULL;	/* Link (Set) cache */
STATIC kmem_cache_t *mtp_sl_cachep = NULL;	/* Signalling Link cache */
STATIC int
mtp_init_caches(void)
{
	if (!mtp_mt_cachep
	    && !(mtp_mt_cachep =
		 kmem_cache_create("mtp_mt_cachep", sizeof(struct mtp), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_mt_cachep", DRV_NAME);
		goto failed_mt;
	} else
		printd(("%s: initialized mt structure cache\n", DRV_NAME));
	if (!mtp_na_cachep
	    && !(mtp_na_cachep =
		 kmem_cache_create("mtp_na_cachep", sizeof(struct na), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_na_cachep", DRV_NAME);
		goto failed_na;
	} else
		printd(("%s: initialized sp structure cache\n", DRV_NAME));
	if (!mtp_sp_cachep
	    && !(mtp_sp_cachep =
		 kmem_cache_create("mtp_sp_cachep", sizeof(struct sp), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_sp_cachep", DRV_NAME);
		goto failed_sp;
	} else
		printd(("%s: initialized sp structure cache\n", DRV_NAME));
	if (!mtp_rs_cachep
	    && !(mtp_rs_cachep =
		 kmem_cache_create("mtp_rs_cachep", sizeof(struct rs), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_rs_cachep", DRV_NAME);
		goto failed_rs;
	} else
		printd(("%s: initialized rs structure cache\n", DRV_NAME));
	if (!mtp_cr_cachep
	    && !(mtp_cr_cachep =
		 kmem_cache_create("mtp_cr_cachep", sizeof(struct cr), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_cr_cachep", DRV_NAME);
		goto failed_cr;
	} else
		printd(("%s: initialized cr structure cache\n", DRV_NAME));
	if (!mtp_rl_cachep
	    && !(mtp_rl_cachep =
		 kmem_cache_create("mtp_rl_cachep", sizeof(struct rl), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_rl_cachep", DRV_NAME);
		goto failed_rl;
	} else
		printd(("%s: initialized rl structure cache\n", DRV_NAME));
	if (!mtp_rt_cachep
	    && !(mtp_rt_cachep =
		 kmem_cache_create("mtp_rt_cachep", sizeof(struct rt), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_rt_cachep", DRV_NAME);
		goto failed_rt;
	} else
		printd(("%s: initialized rt structure cache\n", DRV_NAME));
	if (!mtp_cb_cachep
	    && !(mtp_cb_cachep =
		 kmem_cache_create("mtp_cb_cachep", sizeof(struct cb), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_cb_cachep", DRV_NAME);
		goto failed_cb;
	} else
		printd(("%s: initialized cb structure cache\n", DRV_NAME));
	if (!mtp_ls_cachep
	    && !(mtp_ls_cachep =
		 kmem_cache_create("mtp_ls_cachep", sizeof(struct ls), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_ls_cachep", DRV_NAME);
		goto failed_ls;
	} else
		printd(("%s: initialized ls structure cache\n", DRV_NAME));
	if (!mtp_lk_cachep
	    && !(mtp_lk_cachep =
		 kmem_cache_create("mtp_lk_cachep", sizeof(struct lk), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_lk_cachep", DRV_NAME);
		goto failed_lk;
	} else
		printd(("%s: initialized lk structure cache\n", DRV_NAME));
	if (!mtp_sl_cachep
	    && !(mtp_sl_cachep =
		 kmem_cache_create("mtp_sl_cachep", sizeof(struct sl), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_sl_cachep", DRV_NAME);
		goto failed_sl;
	} else
		printd(("%s: initialized sl structure cache\n", DRV_NAME));
	return (0);
      failed_sl:
	kmem_cache_destroy(mtp_lk_cachep);
      failed_lk:
	kmem_cache_destroy(mtp_ls_cachep);
      failed_ls:
	kmem_cache_destroy(mtp_cb_cachep);
      failed_cb:
	kmem_cache_destroy(mtp_rt_cachep);
      failed_rt:
	kmem_cache_destroy(mtp_rl_cachep);
      failed_rl:
	kmem_cache_destroy(mtp_cr_cachep);
      failed_cr:
	kmem_cache_destroy(mtp_rs_cachep);
      failed_rs:
	kmem_cache_destroy(mtp_sp_cachep);
      failed_sp:
	kmem_cache_destroy(mtp_na_cachep);
      failed_na:
	kmem_cache_destroy(mtp_mt_cachep);
      failed_mt:
	return (-ENOMEM);
}
STATIC int
mtp_term_caches(void)
{
	int err = 0;
	if (mtp_mt_cachep) {
		if (kmem_cache_destroy(mtp_mt_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_mt_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_mt_cachep\n", DRV_NAME));
	}
	if (mtp_na_cachep) {
		if (kmem_cache_destroy(mtp_na_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_na_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_na_cachep\n", DRV_NAME));
	}
	if (mtp_sp_cachep) {
		if (kmem_cache_destroy(mtp_sp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_sp_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_sp_cachep\n", DRV_NAME));
	}
	if (mtp_rs_cachep) {
		if (kmem_cache_destroy(mtp_rs_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_rs_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_rs_cachep\n", DRV_NAME));
	}
	if (mtp_cr_cachep) {
		if (kmem_cache_destroy(mtp_cr_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_cr_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_cr_cachep\n", DRV_NAME));
	}
	if (mtp_rl_cachep) {
		if (kmem_cache_destroy(mtp_rl_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_rl_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_rl_cachep\n", DRV_NAME));
	}
	if (mtp_rt_cachep) {
		if (kmem_cache_destroy(mtp_rt_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_rt_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_rt_cachep\n", DRV_NAME));
	}
	if (mtp_cb_cachep) {
		if (kmem_cache_destroy(mtp_cb_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_cb_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_cb_cachep\n", DRV_NAME));
	}
	if (mtp_ls_cachep) {
		if (kmem_cache_destroy(mtp_ls_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_ls_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_ls_cachep\n", DRV_NAME));
	}
	if (mtp_lk_cachep) {
		if (kmem_cache_destroy(mtp_lk_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_lk_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_lk_cachep\n", DRV_NAME));
	}
	if (mtp_sl_cachep) {
		if (kmem_cache_destroy(mtp_sl_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_sl_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_sl_cachep\n", DRV_NAME));
	}
	return (err);
}

/*
 *  MT allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct mtp *
mtp_lookup(ulong id)
{
	struct mtp *mtp = NULL;
	if (id)
		for (mtp = master.mtp.list; mtp && mtp->id != id; mtp = mtp->next) ;
	return (mtp);
}
STATIC INLINE ulong
mtp_get_id(ulong id)
{
	STATIC ulong sequence = 0;
	if (!id)
		for (id = ++sequence; mtp_lookup(id); id = ++sequence) ;
	return (id);
}
STATIC struct mtp *
mtp_get(struct mtp *mtp)
{
	atomic_inc(&mtp->refcnt);
	return (mtp);
}
STATIC void
mtp_put(struct mtp *mtp)
{
	if (atomic_dec_and_test(&mtp->refcnt)) {
		kmem_cache_free(mtp_mt_cachep, mtp);
		printd(("%s: %s: %p: deallocated mtp private structure", DRV_NAME, __FUNCTION__,
			mtp));
	}
}
STATIC struct mtp *
mtp_alloc_priv(queue_t *q, struct mtp **mtpp, dev_t *devp, cred_t *crp, minor_t bminor)
{
	struct mtp *mt;
	if ((mt = kmem_cache_alloc(mtp_mt_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated mt private structure\n", DRV_NAME, mt));
		bzero(mt, sizeof(*mt));
		mt->priv_put = &mtp_put;
		mt->u.dev.cmajor = getmajor(*devp);
		mt->u.dev.cminor = getminor(*devp);
		mt->cred = *crp;
		(mt->oq = RD(q))->q_ptr = mtp_get(mt);
		(mt->iq = WR(q))->q_ptr = mtp_get(mt);
		spin_lock_init(&mt->qlock);	/* "mt-queue-lock" */
		mt->o_prim = &mtp_r_prim;
		/* 
		   style of interface depends on bminor */
		switch (bminor) {
		case MTP_STYLE_MTPI:
			mt->i_prim = &mtp_w_prim;
			break;
		case MTP_STYLE_TPI:
			mt->i_prim = &tpi_w_prim;
			break;
		case MTP_STYLE_NPI:
			mt->i_prim = &npi_w_prim;
			break;
		case MTP_STYLE_MGMT:
			mt->i_prim = &mgm_w_prim;
			break;
		}
		mt->i_state = 0;
		mt->i_style = bminor;
		mt->i_version = 1;
		spin_lock_init(&mt->lock);	/* "mt-queue-lock" */
		if ((mt->next = *mtpp))
			mt->next->prev = &mt->next;
		mt->prev = mtpp;
		*mtpp = mtp_get(mt);
		/* 
		   not attached to sp yet */
		mt->sp.loc = NULL;
		mt->sp.rem = NULL;
		mt->sp.next = NULL;
		mt->sp.prev = &mt->sp.next;
		/* 
		   set defaults */
		printd(("%s: %p: linked mt private structure\n", DRV_NAME, mt));
	} else
		ptrace(("%s: ERROR: Could not allocate mt private structure\n", DRV_NAME));
	return (mt);
}
STATIC void
mtp_free_priv(struct mtp *mtp)
{
	psw_t flags;
	ensure(mtp, return);
	spin_lock_irqsave(&mtp->lock, flags);
	{
		ss7_unbufcall((str_t *) mtp);
		if (mtp->sp.loc) {
			if ((*mtp->sp.prev = mtp->sp.next))
				mtp->sp.next->sp.prev = mtp->sp.prev;
			mtp->sp.next = NULL;
			mtp->sp.prev = &mtp->sp.next;
			sp_put(xchg(&mtp->sp.loc, NULL));
			mtp_put(mtp);
		}
		if (mtp->sp.rem) {
			rs_put(xchg(&mtp->sp.rem, NULL));
		}
		if (mtp->next || mtp->prev != &mtp->next) {
			/* 
			   remove from master list */
			if ((*mtp->prev = mtp->next))
				mtp->next->prev = mtp->prev;
			mtp->next = NULL;
			mtp->prev = &mtp->next;
			mtp_put(mtp);
		}
		mtp->oq->q_ptr = NULL;
		flushq(mtp->oq, FLUSHALL);
		mtp->oq = NULL;
		mtp_put(mtp);
		mtp->iq->q_ptr = NULL;
		flushq(mtp->iq, FLUSHALL);
		mtp->iq = NULL;
	}
	spin_unlock_irqrestore(&mtp->lock, flags);
	mtp_put(mtp);		/* final put */
}

/*
 *  SL allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC struct sl *
sl_lookup(ulong id)
{
	struct sl *sl = NULL;
	if (id)
		for (sl = master.sl.list; sl && sl->id != id; sl = sl->next) ;
	return (sl);
}
STATIC ulong
sl_get_id(ulong id)
{
	STATIC ulong sequence = 0;
	if (!id)
		for (id = ++sequence; sl_lookup(id); id = ++sequence) ;
	return (id);
}
STATIC struct sl *
sl_get(struct sl *sl)
{
	assure(sl);
	if (sl)
		atomic_inc(&sl->refcnt);
	return (sl);
}
STATIC void
sl_put(struct sl *sl)
{
	assure(sl);
	if (sl) {
		if (atomic_dec_and_test(&sl->refcnt)) {
			kmem_cache_free(mtp_sl_cachep, sl);
			printd(("%s: %p: freed sl structure\n", DRV_NAME, sl));
		}
	}
}
STATIC struct sl *
mtp_alloc_link(queue_t *q, struct sl **slp, ulong index, cred_t *crp)
{
	struct sl *sl;
	if ((sl = kmem_cache_alloc(mtp_sl_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated sl private structure %lu\n", DRV_NAME, sl, index));
		bzero(sl, sizeof(*sl));
		sl->priv_put = &sl_put;
		sl_get(sl);	/* first get */
		sl->u.mux.index = index;
		sl->cred = *crp;
		spin_lock_init(&sl->qlock);	/* "sl-queue-lock" */
		(sl->iq = RD(q))->q_ptr = sl_get(sl);
		(sl->oq = WR(q))->q_ptr = sl_get(sl);
		sl->o_prim = sl_w_prim;
		sl->i_prim = sl_r_prim;
		sl->i_state = LMI_UNATTACHED;
		sl->i_style = LMI_STYLE2;
		sl->i_version = 1;
		sl->l_state = SLS_OUT_OF_SERVICE;
		spin_lock_init(&sl->lock);	/* "sl-priv-lock" */
		/* 
		   place in master list */
		if ((sl->next = *slp))
			sl->next->prev = &sl->next;
		sl->prev = slp;
		*slp = sl_get(sl);
		master.sl.numb++;
		sl->id = index;	/* just use mux index for id */
		sl->type = MTP_OBJ_TYPE_SL;
	} else
		ptrace(("%s: ERROR: Could not allocate sl private structure %lu\n", DRV_NAME,
			index));
	return (sl);
}
STATIC void
mtp_free_link(struct sl *sl)
{
	psw_t flags;
	ensure(sl, return);
	spin_lock_irqsave(&sl->lock, flags);
	{
		ss7_unbufcall((str_t *) sl);
		flushq(sl->oq, FLUSHALL);
		flushq(sl->iq, FLUSHALL);
		sl_timer_stop(sl, tall);
		if (sl->next || sl->prev != &sl->next) {
			/* 
			   remove from master list */
			if ((*sl->prev = sl->next))
				sl->next->prev = sl->prev;
			sl->next = NULL;
			sl->prev = &sl->next;
			ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
			sl_put(sl);
			ensure(master.sl.numb > 0, master.sl.numb++);
			master.sl.numb--;
		}
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(xchg(&sl->iq->q_ptr, NULL));
		ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
		sl_put(xchg(&sl->oq->q_ptr, NULL));
		/* 
		   done, check final count */
		if (atomic_read(&sl->refcnt) != 1) {
			pswerr(("%s: %p: ERROR: sl lingering reference count = %d\n", DRV_NAME, sl,
				atomic_read(&sl->refcnt)));
			atomic_set(&sl->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	sl_put(sl);		/* final put */
}
STATIC void
mtp_free_sl(struct sl *sl)
{
	psw_t flags;
	ensure(sl, return);
	spin_lock_irqsave(&sl->lock, flags);
	{
		struct lk *lk;
		/* 
		   stop timers */
		__sl_timer_stop(sl, tall);
		/* 
		   remove from lk list */
		if ((lk = sl->lk.lk)) {
			if ((*sl->lk.prev = sl->lk.next))
				sl->lk.next->lk.prev = sl->lk.prev;
			sl->lk.next = NULL;
			sl->lk.prev = &sl->lk.next;
			ensure(atomic_read(&sl->refcnt) > 1, sl_get(sl));
			sl_put(sl);
			assure(lk->sl.numb > 0);
			lk->sl.numb--;
			lk_put(xchg(&sl->lk.lk, NULL));
		}
	}
	spin_unlock_irqrestore(&sl->lock, flags);
}

/*
 *  CB allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC INLINE struct cb *
cb_lookup(ulong id)
{
	struct cb *cb = NULL;
	if (id)
		for (cb = master.cb.list; cb && cb->id != id; cb = cb->next) ;
	return (cb);
}
STATIC ulong
cb_get_id(ulong id)
{
	STATIC ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct cb *
cb_get(struct cb *cb)
{
	atomic_inc(&cb->refcnt);
	return (cb);
}
STATIC void
cb_put(struct cb *cb)
{
	if (atomic_dec_and_test(&cb->refcnt)) {
		kmem_cache_free(mtp_cb_cachep, cb);
		printd(("%s: %p: freed cb structure\n", DRV_NAME, cb));
	}
}
STATIC struct cb *
mtp_alloc_cb(ulong id, struct lk *lk, struct sl *from, struct sl *onto, ulong index)
{
	struct cb *cb;
	if ((cb = kmem_cache_alloc(mtp_cb_cachep, SLAB_ATOMIC))) {
		bzero(cb, sizeof(*cb));
		cb->priv_put = &cb_put;
		bufq_init(&cb->buf);
		spin_lock_init(&cb->lock);	/* "cb-lock" */
		cb_get(cb);	/* first get */
		/* 
		   add to master list */
		if ((cb->next = master.cb.list))
			cb->next->prev = &cb->next;
		cb->prev = &master.cb.list;
		master.cb.list = cb_get(cb);
		master.cb.numb++;
		/* 
		   add to link changeover buffer list */
		if (lk) {
			if ((cb->lk.next = lk->cb.list))
				cb->lk.next->lk.prev = &cb->lk.next;
			cb->lk.prev = &lk->cb.list;
			lk->cb.list = cb_get(cb);
			cb->lk.lk = lk_get(lk);
		}
		cb->id = cb_get_id(id);
		cb->type = MTP_OBJ_TYPE_CB;
		cb->state = 0;
		cb->index = index;
		cb->slc = from->slc;
		cb->sl.from = sl_get(from);
		cb->sl.onto = sl_get(onto);
		printd(("%s: %p: linked cb structure %lu\n", DRV_NAME, cb, cb->id));
	} else
		ptrace(("%s: ERROR: Could not allocate cb structure %lu\n", DRV_NAME, id));
	return (cb);
}
STATIC void
mtp_free_cb(struct cb *cb)
{
	psw_t flags;
	ensure(cb, return);
	spin_lock_irqsave(&cb->lock, flags);
	{
		struct lk *lk;
		struct sl *sl;
		/* 
		   stop timers and purge buffers */
		cb_timer_stop(cb, tall);
		bufq_purge(&cb->buf);
		/* 
		   unlink from signalling links */
		if ((sl = xchg(&cb->sl.onto, NULL))) {
			sl->load--;
			sl_put(sl);
		}
		if ((sl = xchg(&cb->sl.from, NULL))) {
			sl->load++;
			sl_put(sl);
		}
		if ((lk = cb->lk.lk)) {
			/* 
			   unlink from lk */
			if ((*cb->lk.prev = cb->lk.next))
				cb->lk.next->lk.prev = cb->lk.prev;
			cb->lk.next = NULL;
			cb->lk.prev = &cb->lk.next;
			ensure(atomic_read(&cb->refcnt) > 1, cb_get(cb));
			cb_put(cb);
			assure(lk->cb.numb > 0);
			lk->cb.numb--;
			lk_put(xchg(&cb->lk.lk, NULL));
		}
		if (cb->next || cb->prev != &cb->next) {
			/* 
			   remove from master list */
			if ((*cb->prev = cb->next))
				cb->next->prev = cb->prev;
			cb->next = NULL;
			cb->prev = &cb->next;
			cb_put(cb);
			assure(master.cb.numb > 0);
			master.cb.numb--;
		}
		/* 
		   done, check final count */
		cb->id = 0;
		if (atomic_read(&cb->refcnt) != 1) {
			pswerr(("%s: %p: ERROR: cb lingering reference count = %d\n", DRV_NAME, cb,
				atomic_read(&cb->refcnt)));
			atomic_set(&cb->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&cb->lock, flags);
	cb_put(cb);		/* final put */
}

/*
 *  LK allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC struct lk *
lk_lookup(ulong id)
{
	struct lk *lk = NULL;
	if (id)
		for (lk = master.lk.list; lk && lk->id != id; lk = lk->next) ;
	return (lk);
}
STATIC ulong
lk_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct lk *
lk_get(struct lk *lk)
{
	atomic_inc(&lk->refcnt);
	return (lk);
}
STATIC void
lk_put(struct lk *lk)
{
	if (atomic_dec_and_test(&lk->refcnt)) {
		kmem_cache_free(mtp_lk_cachep, lk);
		printd(("%s: %p: freed lk structure\n", DRV_NAME, lk));
	}
}
STATIC struct lk *
mtp_alloc_lk(ulong id, struct ls *ls, struct sp *loc, struct rs *adj, ulong ni, ulong slot)
{
	struct lk *lk;
	if ((lk = kmem_cache_alloc(mtp_lk_cachep, SLAB_ATOMIC))) {
		bzero(lk, sizeof(*lk));
		lk->priv_put = &lk_put;
		spin_lock_init(&lk->lock);	/* "lk-lock" */
		lk_get(lk);	/* first get */
		/* 
		   add to master list */
		if ((lk->next = master.lk.list))
			lk->next->prev = &lk->next;
		lk->prev = &master.lk.list;
		master.lk.list = lk_get(lk);
		master.lk.numb++;
		if (ls) {
			/* 
			   add to linkset list */
			if ((lk->ls.next = ls->lk.list))
				lk->ls.next->ls.prev = &lk->ls.next;
			lk->ls.prev = &ls->lk.list;
			lk->ls.ls = ls_get(ls);
			ls->lk.list = lk_get(lk);
			ls->lk.numb++;
		}
		/* 
		   fill out structure */
		lk->id = lk_get_id(id);
		lk->type = MTP_OBJ_TYPE_LK;
		lk->ni = ni;
		lk->slot = slot;
		lk->sp.loc = sp_get(loc);
		lk->sp.adj = rs_get(adj);
		/* 
		   defaults inherited from ls */
		if (ls) {
			/* 
			   sls bits and mask */
			lk->sl.sls_bits = ls->lk.sls_bits;
			lk->sl.sls_mask = ls->lk.sls_mask;
			/* 
			   signalling link timer defaults */
			lk->config.t1 = ls->config.t1;
			lk->config.t2 = ls->config.t2;
			lk->config.t3 = ls->config.t3;
			lk->config.t4 = ls->config.t4;
			lk->config.t5 = ls->config.t5;
			lk->config.t12 = ls->config.t12;
			lk->config.t13 = ls->config.t13;
			lk->config.t14 = ls->config.t14;
			lk->config.t17 = ls->config.t17;
			lk->config.t19a = ls->config.t19a;
			lk->config.t20a = ls->config.t20a;
			lk->config.t21a = ls->config.t21a;
			lk->config.t22 = ls->config.t22;
			lk->config.t23 = ls->config.t23;
			lk->config.t24 = ls->config.t24;
			lk->config.t31a = ls->config.t31a;
			lk->config.t32a = ls->config.t32a;
			lk->config.t33a = ls->config.t33a;
			lk->config.t34a = ls->config.t34a;
			lk->config.t1t = ls->config.t1t;
			lk->config.t2t = ls->config.t2t;
			lk->config.t1s = ls->config.t1s;
			/* 
			   link timer defaults */
			lk->config.t7 = ls->config.t7;
		}
		if (ls) {
			/* 
			   automatically allocate routes if the linkset already has routesets */
			struct rl *rl;
			struct rt *rt;
			for (rl = ls->rl.list; rl; rl = rl->ls.next)
				if (!(rt = mtp_alloc_rt(0, rl, lk, lk->slot)))
					goto free_error;
		}
		if (ls && adj) {
			/* 
			   ensure that there is a route to the adjacent via this linkset */
			struct rl *rl;
			for (rl = adj->rl.list; rl; rl = rl->rs.next)
				if (rl->ls.ls == ls)
					break;
			/* 
			   cost is alway zero (0) because route is to adjacent */
			if (!rl && !(rl = mtp_alloc_rl(0, adj, ls, 0)))
				goto free_error;
			adj->flags |= RSF_ADJACENT;
		}
	}
	return (lk);
      free_error:
	mtp_free_lk(lk);
	return (NULL);
}
STATIC void
mtp_free_lk(struct lk *lk)
{
	psw_t flags;
	ensure(lk, return);
	spin_lock_irqsave(&lk->lock, flags);
	{
		struct cb *cb;
		struct sl *sl;
		struct rt *rt;
		struct ls *ls;
		/* 
		   stop timers */
		__lk_timer_stop(lk, tall);
		/* 
		   remove all changeback buffers */
		while ((cb = lk->cb.list))
			mtp_free_cb(cb);
		/* 
		   remove all routes */
		while ((rt = lk->rt.list))
			mtp_free_rt(rt);
		/* 
		   remove all signalling links */
		while ((sl = lk->sl.list))
			mtp_free_sl(sl);
		/* 
		   remove from ls list */
		if ((ls = lk->ls.ls)) {
			if ((*lk->ls.prev = lk->ls.next))
				lk->ls.next->ls.prev = lk->ls.prev;
			lk->ls.next = NULL;
			lk->ls.prev = &lk->ls.next;
			lk_put(lk);
			ls->lk.numb--;
			ls_put(xchg(&lk->ls.ls, NULL));
		}
		if (lk->next || lk->prev != &lk->next) {
			/* 
			   remove from master list */
			if ((*lk->prev = lk->next))
				lk->next->prev = lk->prev;
			lk->next = NULL;
			lk->prev = &lk->next;
			master.lk.numb--;
			lk_put(lk);
		}
	}
	spin_unlock_irqrestore(&lk->lock, flags);
	lk_put(lk);		/* final put */
}

/*
 *  LS allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC struct ls *
ls_lookup(ulong id)
{
	struct ls *ls = NULL;
	if (id)
		for (ls = master.ls.list; ls && ls->id != id; ls = ls->next) ;
	return (ls);
}
STATIC ulong
ls_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct ls *
ls_get(struct ls *ls)
{
	atomic_inc(&ls->refcnt);
	return (ls);
}
STATIC void
ls_put(struct ls *ls)
{
	if (atomic_dec_and_test(&ls->refcnt)) {
		kmem_cache_free(mtp_ls_cachep, ls);
		printd(("%s: %p: freed ls structure\n", DRV_NAME, ls));
	}
}
STATIC struct ls *
mtp_alloc_ls(ulong id, struct sp *sp, ulong sls_mask)
{
	struct ls *ls;
	if ((ls = kmem_cache_alloc(mtp_ls_cachep, SLAB_ATOMIC))) {
		bzero(ls, sizeof(*ls));
		ls->priv_put = &ls_put;
		spin_lock_init(&ls->lock);	/* "ls-lock" */
		ls_get(ls);	/* first get */
		/* 
		   add to master list */
		if ((ls->next = master.ls.list))
			ls->next->prev = &ls->next;
		ls->prev = &master.ls.list;
		master.ls.list = ls_get(ls);
		master.ls.numb++;
		/* 
		   add to signalling point list */
		if ((ls->sp.next = sp->ls.list))
			ls->sp.next->sp.prev = &ls->sp.next;
		ls->sp.prev = &sp->ls.list;
		ls->sp.sp = sp_get(sp);
		sp->ls.list = ls_get(ls);
		sp->ls.numb++;
		/* 
		   fill out structure */
		ls->id = ls_get_id(id);
		ls->type = MTP_OBJ_TYPE_LS;
		ls->lk.sls_mask = sls_mask;
		ls->lk.sls_bits = 0;
		{
			ulong mask = sls_mask;
			/* 
			   count the 1's in the mask */
			while (mask) {
				if (mask & 0x01)
					ls->lk.sls_bits++;
				mask >>= 1;
			}
		}
		ls->rl.sls_mask = ~sls_mask & sp->ls.sls_mask;
		ls->rl.sls_bits = sp->ls.sls_bits - ls->lk.sls_bits;
		/* 
		   defaults inherited from sp */
		{
			/* 
			   signalling link timer defaults */
			ls->config.t1 = sp->config.t1;
			ls->config.t2 = sp->config.t2;
			ls->config.t3 = sp->config.t3;
			ls->config.t4 = sp->config.t4;
			ls->config.t5 = sp->config.t5;
			ls->config.t12 = sp->config.t12;
			ls->config.t13 = sp->config.t13;
			ls->config.t14 = sp->config.t14;
			ls->config.t17 = sp->config.t17;
			ls->config.t19a = sp->config.t19a;
			ls->config.t20a = sp->config.t20a;
			ls->config.t21a = sp->config.t21a;
			ls->config.t22 = sp->config.t22;
			ls->config.t23 = sp->config.t23;
			ls->config.t24 = sp->config.t24;
			ls->config.t31a = sp->config.t31a;
			ls->config.t32a = sp->config.t32a;
			ls->config.t33a = sp->config.t33a;
			ls->config.t34a = sp->config.t34a;
			ls->config.t1t = sp->config.t1t;
			ls->config.t2t = sp->config.t2t;
			ls->config.t1s = sp->config.t1s;
			/* 
			   link timer defaults */
			ls->config.t7 = sp->config.t7;
		}
	}
	return (ls);
}
STATIC void
mtp_free_ls(struct ls *ls)
{
	psw_t flags;
	ensure(ls, return);
	spin_lock_irqsave(&ls->lock, flags);
	{
		struct sp *sp;
		struct rl *rl;
		struct lk *lk;
		/* 
		   stop all timers */
		__ls_timer_stop(ls, tall);
		/* 
		   remove links */
		while ((lk = ls->lk.list))
			mtp_free_lk(lk);
		/* 
		   remove route lists */
		while ((rl = ls->rl.list))
			mtp_free_rl(rl);
		if ((sp = ls->sp.sp)) {
			/* 
			   remove from signalling point list */
			if ((*ls->sp.prev = ls->sp.next))
				ls->sp.next->sp.prev = ls->sp.prev;
			ls->sp.next = NULL;
			ls->sp.prev = &ls->sp.next;
			ensure(atomic_read(&ls->refcnt) > 1, ls_get(ls));
			ls_put(ls);
			assure(sp->ls.numb > 0);
			sp->ls.numb--;
			sp_put(xchg(&ls->sp.sp, NULL));
		}
		if (ls->next || ls->prev != &ls->next) {
			/* 
			   remove from master list */
			if ((*ls->prev = ls->next))
				ls->next->prev = ls->prev;
			ls->next = NULL;
			ls->prev = &ls->next;
			ls_put(ls);
			assure(master.ls.numb > 0);
			master.ls.numb--;
		}
		/* 
		   done, check final count */
		ls->id = 0;
		if (atomic_read(&ls->refcnt) != 1) {
			pswerr(("%s: %p: ERROR: ls lingering reference count = %d\n", DRV_NAME, ls,
				atomic_read(&ls->refcnt)));
			atomic_set(&ls->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&ls->lock, flags);
	ls_put(ls);		/* final put */
}

/*
 *  CR allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC INLINE struct cr *
cr_lookup(ulong id)
{
	struct cr *cr = NULL;
	if (id)
		for (cr = master.cr.list; cr && cr->id != id; cr = cr->next) ;
	return (cr);
}
STATIC ulong
cr_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct cr *
cr_get(struct cr *cr)
{
	atomic_inc(&cr->refcnt);
	return (cr);
}
STATIC void
cr_put(struct cr *cr)
{
	if (atomic_dec_and_test(&cr->refcnt)) {
		kmem_cache_free(mtp_cr_cachep, cr);
		printd(("%s: %p: freed cr structure\n", DRV_NAME, cr));
	}
}
STATIC struct cr *
mtp_alloc_cr(ulong id, struct rl *rl, struct rt *from, struct rt *onto, ulong index)
{
	struct cr *cr;
	if ((cr = kmem_cache_alloc(mtp_cr_cachep, SLAB_ATOMIC))) {
		bzero(cr, sizeof(*cr));
		cr->priv_put = &cr_put;
		bufq_init(&cr->buf);
		spin_lock_init(&cr->lock);	/* "cr-lock" */
		cr_get(cr);	/* first get */
		/* 
		   add to master list */
		if ((cr->next = master.cr.list))
			cr->next->prev = &cr->next;
		cr->prev = &master.cr.list;
		master.cr.list = cr_get(cr);
		master.cr.numb++;
		/* 
		   add to route list controlled rerouting buffer list */
		if (rl) {
			if ((cr->rl.next = rl->cr.list))
				cr->rl.next->rl.prev = &cr->rl.next;
			cr->rl.prev = &rl->cr.list;
			rl->cr.list = cr_get(cr);
			cr->rl.rl = rl_get(rl);
		}
		cr->id = cr_get_id(id);
		cr->type = MTP_OBJ_TYPE_CR;
		cr->state = 0;
		cr->index = index;
		assure(from);
		assure(onto);
		if (from)
			cr->rt.from = rt_get(from);
		if (onto)
			cr->rt.onto = rt_get(onto);
	}
	return (cr);
}
STATIC void
mtp_free_cr(struct cr *cr)
{
	psw_t flags;
	ensure(cr, return);
	spin_lock_irqsave(&cr->lock, flags);
	{
		struct rt *rt;
		struct rl *rl;
		/* 
		   stop timers and purge queues */
		__cr_timer_stop(cr, tall);
		bufq_purge(&cr->buf);
		/* 
		   unlink from route sets */
		if ((rt = xchg(&cr->rt.onto, NULL))) {
			rt->load--;
			rt_put(rt);
		}
		if ((rt = xchg(&cr->rt.from, NULL))) {
			rt->load++;
			rt_put(rt);
		}
		if ((rl = cr->rl.rl)) {
			/* 
			   remove from route list */
			if ((*cr->rl.prev = cr->rl.next))
				cr->rl.next->rl.prev = cr->rl.prev;
			cr->rl.next = NULL;
			cr->rl.prev = &cr->rl.next;
			ensure(atomic_read(&cr->refcnt) > 1, cr_get(cr));
			cr_put(cr);
			assure(rl->cr.numb > 0);
			rl->cr.numb--;
			rl_put(xchg(&cr->rl.rl, NULL));
		}
		if (cr->next || cr->prev != &cr->next) {
			/* 
			   remove from master list */
			if ((*cr->prev = cr->next))
				cr->next->prev = cr->prev;
			cr->next = NULL;
			cr->prev = &cr->next;
			cr_put(cr);
			assure(master.cr.numb > 0);
			master.cr.numb--;
		}
		/* 
		   done, check final count */
		if (atomic_read(&cr->refcnt) != 1) {
			pswerr(("%s: %p: ERROR: cr lingering reference count = %d\n", DRV_NAME, cr,
				atomic_read(&cr->refcnt)));
			atomic_set(&cr->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&cr->lock, flags);
	cr_put(cr);		/* final put */
}

/*
 *  RR allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC struct rr *
mtp_alloc_rr(struct rs *rs, struct lk *lk)
{
	struct rr *rr;
	if ((rr = kmalloc(sizeof(*rr), GFP_ATOMIC))) {
		bzero(rr, sizeof(*rr));
		if ((rr->rs.next = rs->rr.list))
			rr->rs.next->rs.prev = &rr->rs.next;
		rr->rs.prev = &rs->rr.list;
		rs->rr.list = rr;
		rr->rs.rs = rs_get(rs);
		rr->lk = lk_get(lk);
	}
	return (rr);
}
STATIC void
mtp_free_rr(struct rr *rr)
{
	/* 
	   remove from rs list */
	if (rr->rs.rs) {
		if ((*rr->rs.prev = rr->rs.next))
			rr->rs.next->rs.prev = rr->rs.prev;
		rr->rs.next = NULL;
		rr->rs.prev = &rr->rs.next;
		rs_put(xchg(&rr->rs.rs, NULL));
	}
	if (rr->lk)
		lk_put(xchg(&rr->lk, NULL));
	kfree(rr);
}

/*
 *  RT allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC struct rt *
rt_lookup(ulong id)
{
	struct rt *rt = NULL;
	if (id)
		for (rt = master.rt.list; rt && rt->id != id; rt = rt->next) ;
	return (rt);
}
STATIC ulong
rt_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct rt *
rt_get(struct rt *rt)
{
	atomic_inc(&rt->refcnt);
	return (rt);
}
STATIC void
rt_put(struct rt *rt)
{
	if (atomic_dec_and_test(&rt->refcnt)) {
		kmem_cache_free(mtp_rt_cachep, rt);
		printd(("%s: %p: freed rt structure\n", DRV_NAME, rt));
	}
}
STATIC struct rt *
mtp_alloc_rt(ulong id, struct rl *rl, struct lk *lk, ulong slot)
{
	struct rt *rt;
	if ((rt = kmem_cache_alloc(mtp_rt_cachep, SLAB_ATOMIC))) {
		bzero(rt, sizeof(*rt));
		rt->priv_put = &rt_put;
		spin_lock_init(&rt->lock);	/* "rt-lock" */
		rt_get(rt);	/* first get */
		/* 
		   add to master list */
		if ((rt->next = master.rt.list))
			rt->next->prev = &rt->next;
		rt->prev = &master.rt.list;
		master.rt.list = rt_get(rt);
		master.rt.numb++;
		if (rl) {
			/* 
			   add to routelist list */
			if ((rt->rl.next = rl->rt.list))
				rt->rl.next->rl.prev = &rt->rl.next;
			rt->rl.prev = &rl->rt.list;
			rt->rl.rl = rl_get(rl);
			rl->rt.list = rt_get(rt);
			rl->rt.numb++;
		}
		if (lk) {
			/* 
			   add to linkset list */
			if ((rt->lk.next = lk->rt.list))
				rt->lk.next->lk.prev = &rt->lk.next;
			rt->lk.prev = &lk->rt.list;
			rt->lk.lk = lk_get(lk);
			lk->rt.list = rt_get(rt);
			lk->rt.numb++;
		}
		/* 
		   fill out structure */
		rt->id = rt_get_id(id);
		rt->type = MTP_OBJ_TYPE_RT;
		rt->state = RT_ALLOWED;
		rt->flags = 0;
		rt->load = 0;	/* not loaded yet */
		rt->slot = slot;
		/* 
		   defaults inherited from rl */
		{
			/* 
			   route timer defaults */
			rt->config.t6 = rl->config.t6;
			rt->config.t10 = rl->config.t10;
		}
	}
	return (rt);
}
STATIC void
mtp_free_rt(struct rt *rt)
{
	psw_t flags;
	ensure(rt, return);
	spin_lock_irqsave(&rt->lock, flags);
	{
		struct lk *lk;
		struct rl *rl;
		/* 
		   stop timers */
		__rt_timer_stop(rt, tall);
		/* 
		   remove from rl list */
		if ((rl = rt->rl.rl)) {
			if ((*rt->rl.prev = rt->rl.next))
				rt->rl.next->rl.prev = rt->rl.prev;
			rt->rl.next = NULL;
			rt->rl.prev = &rt->rl.next;
			ensure(atomic_read(&rt->refcnt) > 1, rt_get(rt));
			rt_put(rt);
			assure(rl->rt.numb > 0);
			rl->rt.numb--;
			rl_put(xchg(&rt->rl.rl, NULL));
		}
		/* 
		   remove from lk list */
		if ((lk = rt->lk.lk)) {
			if ((*rt->lk.prev = rt->lk.next))
				rt->lk.next->lk.prev = rt->lk.prev;
			rt->lk.next = NULL;
			rt->lk.prev = &rt->lk.next;
			ensure(atomic_read(&rt->refcnt) > 1, rt_get(rt));
			rt_put(rt);
			assure(lk->rt.numb > 0);
			lk->rt.numb--;
			lk_put(xchg(&rt->lk.lk, NULL));
		}
		if (rt->next || rt->prev != &rt->next) {
			/* 
			   remove from master list */
			if ((*rt->prev = rt->next))
				rt->next->prev = rt->prev;
			rt->next = NULL;
			rt->prev = &rt->next;
			master.rt.numb--;
			rt_put(rt);
		}
	}
	spin_unlock_irqrestore(&rt->lock, flags);
	rt_put(rt);		/* final put */
}

/*
 *  RL allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC struct rl *
rl_lookup(ulong id)
{
	struct rl *rl = NULL;
	if (id)
		for (rl = master.rl.list; rl && rl->id != id; rl = rl->next) ;
	return (rl);
}
STATIC ulong
rl_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct rl *
rl_get(struct rl *rl)
{
	atomic_inc(&rl->refcnt);
	return (rl);
}
STATIC void
rl_put(struct rl *rl)
{
	if (atomic_dec_and_test(&rl->refcnt)) {
		kmem_cache_free(mtp_rl_cachep, rl);
		printd(("%s: %p: freed rl structure\n", DRV_NAME, rl));
	}
}
STATIC struct rl *
mtp_alloc_rl(ulong id, struct rs *rs, struct ls *ls, ulong cost)
{
	struct rl *rl;
	ensure(rs && ls, return (NULL));
	if ((rl = kmem_cache_alloc(mtp_rl_cachep, SLAB_ATOMIC))) {
		struct rl **rlp;
		bzero(rl, sizeof(*rl));
		rl->priv_put = &rl_put;
		spin_lock_init(&rl->lock);	/* "rl-lock" */
		rl_get(rl);	/* first get */
		/* 
		   add to master list */
		if ((rl->next = master.rl.list))
			rl->next->prev = &rl->next;
		rl->prev = &master.rl.list;
		master.rl.list = rl_get(rl);
		master.rl.numb++;
		if (rs) {
			/* 
			   add to routset list (descending cost) */
			for (rlp = &rs->rl.list; (*rlp) && (*rlp)->cost < cost;
			     rlp = &(*rlp)->rs.next) ;
			if ((rl->rs.next = *rlp))
				rl->rs.next->rs.prev = &rl->rs.next;
			rl->rs.prev = rlp;
			rl->rs.rs = rs_get(rs);
			*rlp = rl_get(rl);
			rs->rl.numb++;
		}
		if (ls) {
			/* 
			   add to linkset list */
			if ((rl->ls.next = ls->rl.list))
				rl->ls.next->ls.prev = &rl->ls.next;
			rl->ls.prev = &ls->rl.list;
			rl->ls.ls = ls_get(ls);
			ls->rl.list = rl_get(rl);
			ls->rl.numb++;
		}
		rl->id = rl_get_id(id);
		rl->type = MTP_OBJ_TYPE_RL;
		rl->cost = cost;
		/* 
		   defaults inherited from rs */
		if (rs) {
			/* 
			   route timer defaults */
			rl->config.t6 = rs->config.t6;
			rl->config.t10 = rs->config.t10;
		}
		/* 
		   defaults inherited from ls */
		if (ls) {
			rl->rt.sls_bits = ls->rl.sls_bits;
			rl->rt.sls_mask = ls->rl.sls_mask;
		}
		if (rs && ls) {
			struct lk *lk;
			struct rt *rt;
			/* 
			   automatically allocate routes */
			for (lk = ls->lk.list; lk; lk = lk->ls.next)
				if (!(rt = mtp_alloc_rt(0, rl, lk, lk->slot)))
					goto free_error;
		}
	}
	return (rl);
      free_error:
	mtp_free_rl(rl);
	return (NULL);
}
STATIC void
mtp_free_rl(struct rl *rl)
{
	psw_t flags;
	ensure(rl, return);
	spin_lock_irqsave(&rl->lock, flags);
	{
		struct cr *cr;
		struct rt *rt;
		struct ls *ls;
		struct rs *rs;
		/* 
		   stop timers */
		// __rl_timer_stop(rl, tall); /* no timers */
		/* 
		   remove all controlled rerouting buffers */
		while ((cr = rl->cr.list))
			mtp_free_cr(cr);
		/* 
		   remove all routes */
		while ((rt = rl->rt.list))
			mtp_free_rt(rt);
		/* 
		   remove from rs list */
		if ((rs = rl->rs.rs)) {
			if ((*rl->rs.prev = rl->rs.next))
				rl->rs.next->rs.prev = rl->rs.prev;
			rl->rs.next = NULL;
			rl->rs.prev = &rl->rs.next;
			rl_put(rl);
			rs->rl.numb--;
			rs_put(xchg(&rl->rs.rs, NULL));
		}
		/* 
		   remove from ls list */
		if ((ls = rl->ls.ls)) {
			if ((*rl->ls.prev = rl->ls.next))
				rl->ls.next->ls.prev = rl->ls.prev;
			rl->ls.next = NULL;
			rl->ls.prev = &rl->ls.next;
			rl_put(rl);
			ls->rl.numb--;
			ls_put(xchg(&rl->ls.ls, NULL));
		}
		if (rl->next || rl->prev != &rl->next) {
			/* 
			   remove from master list */
			if ((*rl->prev = rl->next))
				rl->next->prev = rl->prev;
			rl->next = NULL;
			rl->prev = &rl->next;
			master.rl.numb--;
			rl_put(rl);
		}
	}
	spin_unlock_irqrestore(&rl->lock, flags);
	rl_put(rl);		/* final put */
}

/*
 *  RS allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC struct rs *
rs_lookup(ulong id)
{
	struct rs *rs = NULL;
	if (id)
		for (rs = master.rs.list; rs && rs->id != id; rs = rs->next) ;
	return (rs);
}
STATIC ulong
rs_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct rs *
rs_get(struct rs *rs)
{
	atomic_inc(&rs->refcnt);
	return (rs);
}
STATIC void
rs_put(struct rs *rs)
{
	if (atomic_dec_and_test(&rs->refcnt)) {
		kmem_cache_free(mtp_rs_cachep, rs);
		printd(("%s: %p: freed rs structure\n", DRV_NAME, rs));
	}
}
STATIC struct rs *
mtp_alloc_rs(ulong id, struct sp *sp, ulong dest, ulong flags)
{
	struct rs *rs;
	if ((rs = kmem_cache_alloc(mtp_rs_cachep, SLAB_ATOMIC))) {
		bzero(rs, sizeof(*rs));
		rs->priv_put = &rs_put;
		spin_lock_init(&rs->lock);	/* "rs-lock" */
		rs_get(rs);	/* first get */
		/* 
		   add to master list */
		if ((rs->next = master.rs.list))
			rs->next->prev = &rs->next;
		rs->prev = &master.rs.list;
		master.rs.list = rs_get(rs);
		master.rs.numb++;
		if (sp) {
			/* 
			   add to sp list */
			if ((rs->sp.next = sp->rs.list))
				rs->sp.next->sp.prev = &rs->sp.next;
			rs->sp.prev = &sp->rs.list;
			rs->sp.sp = sp_get(sp);
			sp->rs.list = rs_get(rs);
			sp->rs.numb++;
		}
		/* 
		   fill out structure */
		rs->id = rs_get_id(id);
		rs->type = MTP_OBJ_TYPE_RS;
		rs->flags = flags & (RSF_TFR_PENDING | RSF_ADJACENT | RSF_CLUSTER | RSF_XFER_FUNC);
		rs->dest = dest;
		/* 
		   defaults inherited from sp */
		{
			/* 
			   route timer defaults */
			rs->config.t6 = sp->config.t6;
			rs->config.t10 = sp->config.t10;
			/* 
			   route set timer defaults */
			rs->config.t8 = sp->config.t8;
			rs->config.t11 = sp->config.t11;
			rs->config.t15 = sp->config.t15;
			rs->config.t16 = sp->config.t16;
			rs->config.t18a = sp->config.t18a;
		}
	}
	return (rs);
}
STATIC void
mtp_free_rs(struct rs *rs)
{
	psw_t flags;
	ensure(rs, return);
	spin_lock_irqsave(&rs->lock, flags);
	{
		struct rr *rr;
		struct rl *rl;
		struct sp *sp;
		/* 
		   stop timers */
		__rs_timer_stop(rs, tall);
		/* 
		   remove all route restrictions */
		while ((rr = rs->rr.list))
			mtp_free_rr(rr);
		/* 
		   remove all route lists */
		while ((rl = rs->rl.list))
			mtp_free_rl(rl);
		/* 
		   remove from sp list */
		if ((sp = rs->sp.sp)) {
			if ((*rs->sp.prev = rs->sp.next))
				rs->sp.next->sp.prev = rs->sp.prev;
			rs->sp.next = NULL;
			rs->sp.prev = &rs->sp.next;
			rs_put(rs);
			sp->rs.numb--;
			sp_put(xchg(&rs->sp.sp, NULL));
		}
		if (rs->next || rs->prev != &rs->next) {
			/* 
			   remove from master list */
			if ((*rs->prev = rs->next))
				rs->next->prev = rs->prev;
			rs->next = NULL;
			rs->prev = &rs->next;
			master.rs.numb--;
			rs_put(rs);
		}
	}
	spin_unlock_irqrestore(&rs->lock, flags);
	rs_put(rs);		/* final put */
}

/*
 *  SP allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC struct sp *
sp_lookup(ulong id)
{
	struct sp *sp = NULL;
	if (id)
		for (sp = master.sp.list; sp && sp->id != id; sp = sp->next) ;
	return (sp);
}
STATIC ulong
sp_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct sp *
sp_get(struct sp *sp)
{
	atomic_inc(&sp->refcnt);
	return (sp);
}
STATIC void
sp_put(struct sp *sp)
{
	if (atomic_dec_and_test(&sp->refcnt)) {
		kmem_cache_free(mtp_sp_cachep, sp);
		printd(("%s: %p: freed sp structure\n", DRV_NAME, sp));
	}
}
STATIC struct sp *
mtp_alloc_sp(ulong id, struct na *na, ulong pc, ulong equipped, ulong flags)
{
	struct sp *sp;
	if ((sp = kmem_cache_alloc(mtp_sp_cachep, SLAB_ATOMIC))) {
		bzero(sp, sizeof(*sp));
		sp->priv_put = &sp_put;
		spin_lock_init(&sp->lock);	/* "sp-lock" */
		sp_get(sp);	/* first get */
		/* 
		   add to master list */
		if ((sp->next = master.sp.list))
			sp->next->prev = &sp->next;
		sp->prev = &master.sp.list;
		master.sp.list = sp_get(sp);
		master.sp.numb++;
		if (na) {
			/* 
			   add to na list */
			if ((sp->na.next = na->sp.list))
				sp->na.next->na.prev = &sp->na.next;
			sp->na.prev = &na->sp.list;
			sp->na.na = na_get(na);
			na->sp.list = sp_get(sp);
			na->sp.numb++;
		}
		/* 
		   fill out structure */
		sp->id = sp_get_id(id);
		sp->type = MTP_OBJ_TYPE_SP;
		sp->flags =
		    flags & (SPF_CLUSTER | SPF_LOSC_PROC_A | SPF_LOSC_PROC_B | SPF_SECURITY |
			     SPF_XFER_FUNC);
		sp->pc = pc;
		sp->mtp.equipped = equipped;
		/* 
		   defaults inherited from na */
		{
			/* 
			   sls bits and mask */
			sp->ls.sls_bits = na->sp.sls_bits;
			sp->ls.sls_mask = na->sp.sls_mask;
			/* 
			   signalling link timer defaults */
			sp->config.t1 = na->config.t1;
			sp->config.t2 = na->config.t2;
			sp->config.t3 = na->config.t3;
			sp->config.t4 = na->config.t4;
			sp->config.t5 = na->config.t5;
			sp->config.t12 = na->config.t12;
			sp->config.t13 = na->config.t13;
			sp->config.t14 = na->config.t14;
			sp->config.t17 = na->config.t17;
			sp->config.t19a = na->config.t19a;
			sp->config.t20a = na->config.t20a;
			sp->config.t21a = na->config.t21a;
			sp->config.t22 = na->config.t22;
			sp->config.t23 = na->config.t23;
			sp->config.t24 = na->config.t24;
			sp->config.t31a = na->config.t31a;
			sp->config.t32a = na->config.t32a;
			sp->config.t33a = na->config.t33a;
			sp->config.t34a = na->config.t34a;
			sp->config.t1t = na->config.t1t;
			sp->config.t2t = na->config.t2t;
			sp->config.t1s = na->config.t1s;
			/* 
			   link timer defaults */
			sp->config.t7 = na->config.t7;
			/* 
			   route timer defaults */
			sp->config.t6 = na->config.t6;
			sp->config.t10 = na->config.t10;
			/* 
			   route set timer defaults */
			sp->config.t8 = na->config.t8;
			sp->config.t11 = na->config.t11;
			sp->config.t15 = na->config.t15;
			sp->config.t16 = na->config.t16;
			sp->config.t18a = na->config.t18a;
			/* 
			   signalling point timer defaults */
			sp->config.t1r = na->config.t1r;
			sp->config.t18 = na->config.t18;
			sp->config.t19 = na->config.t19;
			sp->config.t20 = na->config.t20;
			sp->config.t21 = na->config.t21;
			sp->config.t22a = na->config.t22a;
			sp->config.t23a = na->config.t23a;
			sp->config.t24a = na->config.t24a;
			sp->config.t25a = na->config.t25a;
			sp->config.t26a = na->config.t26a;
			sp->config.t27a = na->config.t27a;
			sp->config.t28a = na->config.t28a;
			sp->config.t29a = na->config.t29a;
			sp->config.t30a = na->config.t30a;
		}
	}
	return (sp);
}
STATIC void
mtp_free_sp(struct sp *sp)
{
	psw_t flags;
	ensure(sp, return);
	spin_lock_irqsave(&sp->lock, flags);
	{
		struct ls *ls;
		struct rs *rs;
		struct na *na;
		/* 
		   stop timers */
		__sp_timer_stop(sp, tall);
		fixme(("Hangup on all mtp users\n"));
		/* 
		   remove all route sets */
		while ((rs = sp->rs.list))
			mtp_free_rs(rs);
		/* 
		   remove all link sets */
		while ((ls = sp->ls.list))
			mtp_free_ls(ls);
		/* 
		   remove from na list */
		if ((na = sp->na.na)) {
			if ((*sp->na.prev = sp->na.next))
				sp->na.next->na.prev = sp->na.prev;
			sp->na.next = NULL;
			sp->na.prev = &sp->na.next;
			sp_put(sp);
			na->sp.numb--;
			na_put(xchg(&sp->na.na, NULL));
		}
		if (sp->next || sp->prev != &sp->next) {
			/* 
			   remove from master list */
			if ((*sp->prev = sp->next))
				sp->next->prev = sp->prev;
			sp->next = NULL;
			sp->prev = &sp->next;
			master.sp.numb--;
			sp_put(sp);
		}
	}
	spin_unlock_irqrestore(&sp->lock, flags);
	sp_put(sp);		/* final put */
}

/*
 *  NA allocation and deallocaiton
 *  -------------------------------------------------------------------------
 */
STATIC struct na *
mtp_alloc_na(ulong id, uint32_t member, uint32_t cluster, uint32_t network, uint sls_bits,
	     struct lmi_option *option)
{
	struct na *na;
	if ((na = kmem_cache_alloc(mtp_na_cachep, SLAB_ATOMIC))) {
		bzero(na, sizeof(*na));
		na->priv_put = &na_put;
		spin_lock_init(&na->lock);	/* "na-lock" */
		na_get(na);	/* first get */
		/* 
		   add to master list */
		if ((na->next = master.na.list))
			na->next->prev = &na->next;
		na->prev = &master.na.list;
		master.na.list = na_get(na);
		master.na.numb++;
		/* 
		   fill out structure */
		na->id = na_get_id(id);
		na->type = MTP_OBJ_TYPE_NA;
		na->mask.member = member;
		na->mask.cluster = cluster;
		na->mask.network = network;
		na->sp.sls_bits = sls_bits;
		na->sp.sls_mask = (1 << sls_bits) - 1;
		na->option = *option;
		/* 
		   populate defaults based on protoocl variant */
		switch ((na->option.pvar & SS7_PVAR_MASK)) {
		default:
		case SS7_PVAR_ITUT:
			na->config = itut_na_config_default;
			break;
		case SS7_PVAR_ETSI:
			na->config = etsi_na_config_default;
			break;
		case SS7_PVAR_ANSI:
			na->config = ansi_na_config_default;
			break;
		case SS7_PVAR_JTTC:
			na->config = jttc_na_config_default;
			break;
		}
	}
	return (na);
}
STATIC void
mtp_free_na(struct na *na)
{
	psw_t flags;
	ensure(na, return);
	spin_lock_irqsave(&na->lock, flags);
	{
		struct sp *sp;
		/* 
		   remove all attached signalling points */
		while ((sp = na->sp.list))
			mtp_free_sp(sp);
		if (na->next || na->prev != &na->next) {
			/* 
			   remove from master list */
			if ((*na->prev = na->next))
				na->next->prev = na->prev;
			na->next = NULL;
			na->prev = &na->next;
			master.na.numb--;
			na_put(na);
		}
	}
	spin_unlock_irqrestore(&na->lock, flags);
	na_put(na);		/* final put */
	return;
}
STATIC struct na *
na_lookup(ulong id)
{
	struct na *na = NULL;
	if (id)
		for (na = master.na.list; na && na->id != id; na = na->next) ;
	return (na);
}
STATIC ulong
na_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct na *
na_get(struct na *na)
{
	atomic_inc(&na->refcnt);
	return (na);
}
STATIC void
na_put(struct na *na)
{
	if (atomic_dec_and_test(&na->refcnt)) {
		kmem_cache_free(mtp_na_cachep, na);
		printd(("%s: %p: freed na structure\n", DRV_NAME, na));
	}
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
MODULE_PARM_DESC(modid, "Module ID for the INET driver. (0 for allocation.)");

unsigned short major = CMAJOR_0;
MODULE_PARM(major, "h");
MODULE_PARM_DESC(major, "Device number for the INET driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw mtp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &mtpinfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
mtp_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&mtp_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
mtp_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&mtp_cdev, major)) < 0)
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
mtp_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &mtpinfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
mtp_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
mtpterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (mtp_majors[mindex]) {
			if ((err = mtp_unregister_strdev(mtp_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					mtp_majors[mindex]);
			if (mindex)
				mtp_majors[mindex] = 0;
		}
	}
	if ((err = mtp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
mtpinit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = mtp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		mtpterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = mtp_register_strdev(mtp_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					mtp_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				mtpterminate();
				return (err);
			}
		}
		if (mtp_majors[mindex] == 0)
			mtp_majors[mindex] = err;
#ifdef LIS
		LIS_DEVFLAGS(mtp_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = mtp_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(mtpinit);
module_exit(mtpterminate);

#endif				/* LINUX */
