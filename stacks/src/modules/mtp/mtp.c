/*****************************************************************************

 @(#) $RCSfile: mtp.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2007/07/14 01:34:33 $

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

 Last Modified $Date: 2007/07/14 01:34:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtp.c,v $
 Revision 0.9.2.20  2007/07/14 01:34:33  brian
 - make license explicit, add documentation

 Revision 0.9.2.19  2007/05/18 00:00:44  brian
 - check for nf_reset

 Revision 0.9.2.18  2007/03/25 18:59:40  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.17  2007/02/26 07:25:33  brian
 - synchronizing changes

 Revision 0.9.2.16  2007/02/21 01:09:07  brian
 - updating mtp.c driver, better mi_open allocators

 Revision 0.9.2.15  2007/02/17 02:49:13  brian
 - first clean recompile of MTP modules on LFS

 Revision 0.9.2.14  2006/05/08 11:00:58  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.13  2006/03/07 01:10:36  brian
 - binary compatible callouts

 Revision 0.9.2.12  2006/03/04 13:00:12  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: mtp.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2007/07/14 01:34:33 $"

static char const ident[] =
    "$RCSfile: mtp.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2007/07/14 01:34:33 $";

/*
 *  This an MTP (Message Transfer Part) multiplexing driver which can have SL
 *  (Signalling Link) streams I_LINK'ed or I_PLINK'ed underneath it to form a
 *  complete Message Transfer Part protocol layer for SS7.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>
#include <linux/socket.h>

#undef DB_TYPE
#define DB_TYPE(mp) ((mp)->b_datap->db_type)

#define mi_timer mi_timer_MAC

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

#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

#define MTP_DESCRIP	"SS7 MESSAGE TRANSFER PART (MTP) STREAMS MULTIPLEXING DRIVER."
#define MTP_REVISION	"LfS $RCSfile: mtp.c,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2007/07/14 01:34:33 $"
#define MTP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define MTP_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MTP_LICENSE	"GPL v2"
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
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mtp");
#endif
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
		union { \
			struct { \
				size_t allowed;		/* numb of __s1 allowed */ \
				size_t danger;		/* numb of __s1 in danger */ \
				size_t congested;	/* numb of __s1 congested */ \
				size_t restricted;	/* numb of __s1 restricted */ \
				size_t prohibited;	/* numb of __s1 prohibited */ \
				size_t inhibited;	/* numb of __s1 inhibited */ \
				size_t blocked;		/* numb of __s1 blocked */ \
				size_t inactive;	/* numb of __s1 inactive */ \
				size_t restart;		/* numb of __s1 restart */ \
			}; \
			size_t states[9]; \
		}; \
	} __s2; \

/*
   generic counters structure 
 */
struct counters {
	SLIST_COUNTERS (head, c);	/* counter declaration */
};

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
struct df {
	rwlock_t lock;			/* master lock */
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
	struct mtp_notify_df notify;	/* default notifications */
};

static struct df master = {
	.lock = RW_LOCK_UNLOCKED,
};

static struct df *
df_lookup(uint id)
{
	if (id)
		return (NULL);
	return (&master);
}

/*
 *  MT - MTP User
 *  -----------------------------------
 *  The MTP structure corresponds to an MTP User stream.  This stream is bound to a source address,
 *  and possibly a destination address if the SI is a connection-oriented SI (e.g. ISUP).  The
 *  source address defines a local signalling point and the destination address defines a route set
 *  to the remote signalling point for pseudo- connection-oriented service indicators.
 */
struct mtp {
	queue_t *rq;			/* RD queue */
	dev_t dev;			/* device number */
	cred_t cred;			/* credentials of creator */
	struct {
		uint type;		/* interface type */
		uint state;		/* interface state */
		uint style;		/* interface style */
		uint version;		/* interface version */
	} i;
	struct mtp_addr src;		/* srce address */
	struct mtp_addr dst;		/* dest address */
	struct {
		struct sp *loc;		/* this mtp user for local signalling point */
		struct rs *rem;		/* this mtp user for remote signalling point 
					 */
		struct mtp *next;	/* next mtp user for local signalling point */
		struct mtp **prev;	/* prev mtp user for local signalling point */
	} sp;
	struct {
		t_uscalar_t sls;	/* default options */
		t_uscalar_t mp;		/* default options */
		t_uscalar_t debug;	/* default options */
	} options;
	struct T_info_ack *prot;	/* Protocol parameters */
};

#define MTP_PRIV(__q) ((struct mtp *)(__q)->q_ptr)

static struct mtp *mtp_alloc_priv(queue_t *, dev_t *, cred_t *, minor_t);
static void mtp_free_priv(struct mtp *);
static struct mtp *mtp_lookup(uint);
static uint mtp_get_id(uint);

/*
 *  NA - Network Apperance
 *  -----------------------------------
 *  The NA struct defines a network appearance.  A network appearance is a combination of protocol
 *  variant, protocol options and point code format.  The network appearance identifies a point code
 *  numbering space and defines the service indicators within that numbering space.  Signalling
 *  points belong to a single network apperance.
 */
struct na {
	HEAD_DECLARATION (struct na);	/* head declaration */
	struct {
		uint32_t member;	/* pc member mask */
		uint32_t cluster;	/* pc cluster mask */
		uint32_t network;	/* pc network mask */
	} mask;
	SLIST_COUNTERS (sp, sp);	/* signalling points in this na */
	struct lmi_option option;	/* protocol variant and options */
	struct T_info_ack *prot[16];	/* protocol profiles for si values */
	struct mtp_timers_na timers;	/* network appearance timers */
	struct mtp_opt_conf_na config;	/* network appearance configuration */
	struct mtp_stats_na statsp;	/* network appearance statistics periods */
	struct mtp_stats_na stats;	/* network appearance statistics */
	struct mtp_notify_na notify;	/* network appearance notifications */
};

static struct na *mtp_alloc_na(uint, struct mtp_conf_na *);
static void mtp_free_na(struct na *);
static struct na *na_lookup(uint);
static uint na_get_id(uint);

/*
 *  RR - Routeset restriction
 *  -----------------------------------
 *  The route set restriction (RR) structure...
 */
struct rr {
	struct lk *lk;			/* link (set) that this route restriction
					   corresponds to */
	SLIST_LINKAGE (rs, rr, rs);	/* route restriction list linkage */
};

static struct rr *mtp_alloc_rr(struct rs *, struct lk *);
static void mtp_free_rr(struct rr *);

/*
 *  RS - Routeset
 *  -----------------------------------
 *  The route set structure defines the SS7 route set to a remote signalling
 *  point within the network appearance and belonging to a signalling point.
 */
struct rs {
	HEAD_DECLARATION (struct rs);	/* head declaration */
	int rs_type;			/* type of route set (cluster, member) */
	int cong_status;		/* congestion status */
	int disc_status;		/* discard status */
	uint32_t dest;			/* remote signalling point/cluster */
	size_t cong_msg_count;		/* msg counts under congestion */
	size_t cong_oct_count;		/* msg octets under congestion */
	SLIST_LINKAGE (sp, rs, sp);	/* signalling point list linkage */
	SLIST_COUNTERS (rl, rl);	/* list and counts of route lists */
	SLIST_HEAD (rr, rr);		/* route set restriction list */
	struct mtp_timers_rs timers;	/* route set timers */
	struct mtp_opt_conf_rs config;	/* route set configuration */
	struct mtp_stats_rs statsp;	/* route set statistics periods */
	struct mtp_stats_rs stats;	/* route set statistics */
	struct mtp_notify_rs notify;	/* route set notifications */
};

static struct rs *mtp_alloc_rs(uint, struct sp *, struct mtp_conf_rs *);
static void mtp_free_rs(struct rs *);
static struct rs *rs_lookup(uint);
static uint rs_get_id(uint);

/*
 *  CR - Controlled rerouting buffer
 *  -----------------------------------
 *  The controlled rerouting buffer structure is used to hold MSUs during the
 *  controlled rerouting procedure between a equal priority alternate route
 *  (from) and an normal route (onto).  MSUs are accumulated in the buffer
 *  until controlled rerouting is cancelled or the associated t6 timer expires
 *  and buffered signalling traffic is rerouted to the normal route.
 */
struct cr {
	HEAD_DECLARATION (struct cr);	/* head declaration */
	uint index;			/* sls index in route sls map */
	struct {
		struct rt *from;	/* route we are buffering from */
		struct rt *onto;	/* route we are buffering onto */
	} rt;
	SLIST_LINKAGE (rl, cr, rl);	/* route list list linkage */
	struct {
		mblk_t *t6;		/* controlled rerouting timer */
	} timers;
	struct bufq buf;		/* message buffer */
};

static struct cr *mtp_alloc_cr(uint, struct rl *, struct rt *, struct rt *, uint);
static void mtp_free_cr(struct cr *);
static struct cr *cr_lookup(uint);
static uint cr_get_id(uint);

/*
 *  RL - Routelist
 *  -----------------------------------
 *  The route list structure contains a list of routes of identical cost.  A
 *  route list represents a route via a combined link set.  Route lists belong
 *  to one route set and contain several equal cost routes.  Cost is
 *  associated with a route list (all contained routes have the same priority
 *  as the route list).  An SLS map is used to route messages to the
 *  appropriate route within the route list whenever the route list is
 *  actively carrying traffic.
 */
#define RT_SMAP_SIZE	32
struct rl {
	HEAD_DECLARATION (struct rl);	/* head declaration */
	uint cost;			/* priority of this route list */
	SLIST_LINKAGE (rs, rl, rs);	/* route list linkage */
	SLIST_LINKAGE (ls, rl, ls);	/* link set list linkage */
	SLIST_COUNTERS (rt, rt);	/* list of routes and counts */
	SLIST_HEAD (cr, cr);		/* list of controlled rerouting buffers */
	SLIST_SMAP (rt, cr, RT_SMAP_SIZE);	/* route list sls map */
	struct mtp_timers_rl timers;	/* route list timers */
	struct mtp_opt_conf_rl config;	/* route list configuration */
	struct mtp_stats_rl statsp;	/* route list statistics periods */
	struct mtp_stats_rl stats;	/* route list statistics */
	struct mtp_notify_rl notify;	/* route list notifications */
};

static struct rl *mtp_alloc_rl(uint, struct rs *, struct ls *, struct mtp_conf_rl *);
static void mtp_free_rl(struct rl *);
static struct rl *rl_lookup(uint);
static uint rl_get_id(uint);

/*
 *  RT - Route
 *  -----------------------------------
 *  The route structure represents a route over a link (set) to an adjacent
 *  signalling point.  Routes belong to route lists and are associated with a
 *  link (LK) structure.  Each route occupies a slot within the parent route
 *  list SLS map and has a load factor indicating the loading of this route
 *  within the route list.  Load factors are used to determine danger of
 *  congestion for ANSI transfer restricted procedures as well as when
 *  balancing load due to a destination which has become inaccessible or
 *  accessible.
 */
struct rt {
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
};

static struct rt *mtp_alloc_rt(uint, struct rl *, struct lk *, struct mtp_conf_rt *);
static void mtp_free_rt(struct rt *);
static struct rt *rt_lookup(uint);
static uint rt_get_id(uint);

/*
 *  SP - Signalling point
 *  -----------------------------------
 *  The signalling point structure represents a local (real, virtual or alias)
 *  signalling point within a network appearance.
 */
struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	int users;
	queue_t *waitq;
	uint32_t ni;			/* network indicator */
	uint32_t pc;			/* point code */
	uint sls;			/* sls for load sharing of management
					   messages */
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
};

static struct sp *mtp_alloc_sp(uint, struct na *, struct mtp_conf_sp *);
static void mtp_free_sp(struct sp *);
static struct sp *sp_lookup(uint);
static uint sp_get_id(uint);

/*
 *  CB - Changeback buffer
 *  -----------------------------------
 *  The changeback buffer structure is used for the MTP changeover and
 *  changeback procedures, where MSUs are buffered and transferred from one
 *  signalling link in the (combined) link set (from) to another signalling
 *  link the combined link set (onto).
 */
struct cb {
	HEAD_DECLARATION (struct cb);	/* head declaration */
	uint cbc;			/* changeback code */
	uint slc;			/* signalling link code */
	uint index;			/* sls index in signalling link sls map */
	struct {
		struct sl *from;	/* signalling link we are buffering from */
		struct sl *onto;	/* signalling link we are buffering onto */
	} sl;
	SLIST_LINKAGE (lk, cb, lk);	/* link list linkage */
	struct {
		mblk_t *t1;		/* changeover timer */
		mblk_t *t2;		/* changeover ack timer */
		mblk_t *t3;		/* changeback timer */
		mblk_t *t4;		/* changeback ack timer */
		mblk_t *t5;		/* changeback ack second attempt timer */
	} timers;
	struct bufq buf;		/* message buffer */
};

static struct cb *mtp_alloc_cb(uint, struct lk *, struct sl *, struct sl *, uint);
static void mtp_free_cb(struct cb *);
static struct cb *cb_lookup(uint);
static uint cb_get_id(uint);

/*
 *  LS - (Combined) Link Set
 *  -----------------------------------
 *  The link set structure represents a combined link set.  Each link set
 *  belongs to a local signalling point and contains a number of link (LK)
 *  structures.  An SLS map is used to select a link within the link set.
 */
struct ls {
	HEAD_DECLARATION (struct ls);	/* head declaration */
	uint sls;			/* sls for mgmt load sharing */
	SLIST_LINKAGE (sp, ls, sp);	/* signalling point list linkage */
	SLIST_COUNTERS (rl, rl);	/* list and counts of route lists */
	SLIST_COUNTERS (lk, lk);	/* list and counts of links */
	struct mtp_timers_ls timers;	/* link set timers */
	struct mtp_opt_conf_ls config;	/* link set configuration */
	struct mtp_stats_ls statsp;	/* link set statistics periods */
	struct mtp_stats_ls stats;	/* link set statistics */
	struct mtp_notify_ls notify;	/* link set notifications */
};

static struct ls *mtp_alloc_ls(uint, struct sp *, struct mtp_conf_ls *);
static void mtp_free_ls(struct ls *);
static struct ls *ls_lookup(uint);
static uint ls_get_id(uint);

/*
 *  LK - Link (set)
 *  -----------------------------------
 *  The link structure represents a link set.  Each link belongs to a combined
 *  link set and contains a number of signalling link (SL) structure.  An SLS
 *  map is ued to select a signalling link within the link.
 */
#define SL_SMAP_SIZE	256
struct lk {
	HEAD_DECLARATION (struct lk);	/* head declaration */
	uint ni;			/* network indicator for link set */
	uint slot;			/* slot in this link set */
	uint load;			/* load in this link set */
	struct {
		struct sp *loc;		/* local signalling point for this link set */
		struct rs *adj;		/* adjacent signalling point for this link
					   set */
	} sp;
	SLIST_LINKAGE (ls, lk, ls);	/* link set list linkage */
	SLIST_COUNTERS (rt, rt);	/* list and counts of routes */
	SLIST_COUNTERS (sl, sl);	/* list and counts of signalling links */
	SLIST_HEAD (cb, cb);		/* list of changeback buffers */
	SLIST_SMAP (sl, cb, SL_SMAP_SIZE);	/* link sls map */
	struct mtp_timers_lk timers;	/* link timers */
	struct mtp_opt_conf_lk config;	/* link configuration */
	struct mtp_stats_lk statsp;	/* link statistics periods */
	struct mtp_stats_lk stats;	/* link statistics */
	struct mtp_notify_lk notify;	/* link notifications */
};

static struct lk *mtp_alloc_lk(uint, struct ls *, struct rs *, struct sp *,
			       struct mtp_conf_lk *);
static void mtp_free_lk(struct lk *);
static struct lk *lk_lookup(uint);
static uint lk_get_id(uint);

/*
 *  SL - Signalling link
 *  -----------------------------------
 *  The signalling link structure represents a lower multiplexing driver
 *  stream associated with a Signalling Link (SL) driver.  Each signalling
 *  link has a unique signalling link code (SLC) within a link and has a
 *  Signalling Data Link Identifier identifying the signalling data link
 *  (channel).  Each signalling link has a load factor which indicates how
 *  many Signalling Link Selection (SLS) codes of traffic are assigned to this
 *  signalling link.
 */
struct sl {
	queue_t *wq;			/* WR queue */
	int index;			/* multiplex index */
	cred_t cred;			/* credentials of creator */
	struct {
		uint type;		/* interface type */
		uint state;		/* interface state */
		uint style;		/* interface style */
		uint version;		/* interface version */
	} i;
	uint l_state;			/* signalling link state */
	uint load;			/* load in this link */
	uint slot;			/* slot in this link */
	SLIST_LINKAGE (lk, sl, lk);	/* link list linkage */
	uint slc;			/* signalling link code */
	uint sdli;			/* signalling data link identifier */
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
};

#define SL_PRIV(__q) ((struct sl *)(__q)->q_ptr)

static struct sl *mtp_alloc_link(queue_t *, int, cred_t *, minor_t);
static void mtp_free_link(struct sl *);
static struct sl *mtp_alloc_sl(uint, struct sl *, struct lk *, struct mtp_conf_sl *);
static void mtp_free_sl(struct sl *);
static struct sl *sl_looup_mux(int);
static struct sl *sl_lookup(uint);
static uint sl_get_id(uint);

/*
 *  SL interface state flags
 */
#define SLSF_OUT_OF_SERVICE	(1<< SLS_OUT_OF_SERVICE)	/* out of service */
#define SLSF_PROC_OUTG		(1<< SLS_PROC_OUTG)	/* processor outage */
#define SLSF_IN_SERVICE		(1<< SLS_IN_SERVICE)	/* in service */
#define SLSF_WACK_COO		(1<< SLS_WACK_COO)	/* waiting COA/ECA in
							   response to COO */
#define SLSF_WACK_ECO		(1<< SLS_WACK_ECO)	/* waiting COA/ECA in
							   response to ECO */
#define SLSF_WCON_RET		(1<< SLS_WCON_RET)	/* waiting for retrieval
							   confirmation */
#define SLSF_WIND_CLRB		(1<< SLS_WIND_CLRB)	/* waiting for clear buffers 
							   indication */
#define SLSF_WIND_BSNT		(1<< SLS_WIND_BSNT)	/* waiting for BSNT
							   indication */
#define SLSF_WIND_INSI		(1<< SLS_WIND_INSI)	/* waiting for in service
							   indication */
#define SLSF_WACK_SLTM		(1<< SLS_WACK_SLTM)	/* waiting SLTA in response
							   to 1st SLTM */

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

static rwlock_t mtp_mux_lock = RW_LOCK_UNLOCKED;

/*
 *  =========================================================================
 *
 *  Locking
 *
 *  =========================================================================
 */
static inline int
sp_trylock(queue_t *q, struct sp *sp)
{
	unsigned long flags;
	queue_t *oldq = NULL;
	int result = 0;

	spin_lock_irqsave(&sp->lock, flags);
	if (sp->users == 0) {
		sp->users = result = 1;
	} else {
		oldq = sp->waitq;
		sp->waitq = q;
	}
	spin_unlock_irqrestore(&sp->lock, flags);
	if (oldq)
		qenable(oldq);
	return (result);
}
static inline void
sp_unlock(struct sp *sp)
{
	unsigned long flags;
	queue_t *newq = NULL;

	spin_lock_irqsave(&sp->lock, flags);
	sp->users = 0;
	newq = sp->waitq;
	sp->waitq = NULL;
	spin_unlock_irqrestore(&sp->lock, flags);
	if (newq)
		qenable(newq);
}

static inline struct mtp *
mtp_acquire(queue_t *q)
{
	struct mtp *mtp;
	struct sp *sp;

	read_lock(&mtp_mux_lock);
	if ((mtp = (void *) mi_trylock(q))) {
		read_unlock(&mtp_mux_lock);
		if ((sp = mtp->sp.loc)) {
			if (!sp_trylock(q, sp)) {
				mi_unlock((caddr_t) mtp);
				return (0);
			}
		}
		return (mtp);
	}
	read_unlock(&mtp_mux_lock);
	return (NULL);
}
static inline void
mtp_release(struct mtp *mtp)
{
	struct sp *sp;

	if ((sp = mtp->sp.loc))
		sp_unlock(sp);
	mi_unlock((caddr_t) mtp);
}
static inline struct sl *
sl_acquire(queue_t *q)
{
	struct sl *sl;
	struct sp *sp;

	read_lock(&mtp_mux_lock);
	if ((sl = (void *) mi_trylock(q))) {
		read_unlock(&mtp_mux_lock);
		if ((sp = sl->lk.lk->ls.ls->sp.sp)) {
			if (!sp_trylock(q, sp)) {
				mi_unlock((caddr_t) sl);
				return (0);
			}
		}
		return (sl);
	}
	read_unlock(&mtp_mux_lock);
	return (NULL);
}
static inline void
sl_release(struct sl *sl)
{
	struct sp *sp;

	if ((sp = sl->lk.lk->ls.ls->sp.sp))
		sp_unlock(sp);
	mi_unlock((caddr_t) sl);
}

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */
struct mtp_opts {
	uint flags;			/* success flags */
	t_uscalar_t *sls;
	t_uscalar_t *mp;
	t_uscalar_t *debug;
};

static struct {
	t_uscalar_t sls;
	t_uscalar_t mp;
	t_uscalar_t debug;
} opt_defaults = {
0, 0, 0};

#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))
static size_t
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
static void
mtp_build_opts(struct mtp *mtp, struct mtp_opts *ops, unsigned char *p)
{
	if (ops) {
		struct t_opthdr *oh;
		const size_t hlen = sizeof(struct t_opthdr);

		if (ops->sls) {
			oh = (typeof(oh)) p++;
			oh->len = hlen + sizeof(*(ops->sls));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_SLS;
			oh->status =
			    (ops->flags & (1 << T_MTP_SLS)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->sls)) p) = *(ops->sls);
			p += _T_ALIGN_SIZEOF(*ops->sls);
		}
		if (ops->mp) {
			oh = (typeof(oh)) p++;
			oh->len = hlen + sizeof(*(ops->mp));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_MP;
			oh->status =
			    (ops->flags & (1 << T_MTP_MP)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->mp)) p) = *(ops->mp);
			p += _T_ALIGN_SIZEOF(*ops->mp);
		}
		if (ops->debug) {
			oh = (typeof(oh)) p++;
			oh->len = hlen + sizeof(*(ops->debug));
			oh->level = T_SS7_MTP;
			oh->name = T_MTP_DEBUG;
			oh->status =
			    (ops->
			     flags & (1 << T_MTP_DEBUG)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->debug)) p) = *(ops->debug);
			p += _T_ALIGN_SIZEOF(*ops->debug);
		}
	}
}
static int
mtp_parse_opts(struct mtp *mtp, struct mtp_opts *ops, unsigned char *op, size_t len)
{
	struct t_opthdr *oh;

	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh;
	     oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
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
static int
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
static int
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
static int
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
static int
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
static int
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
static const char *
mtp_state_name(struct mtp *mtp, uint state)
{
	switch (mtp->i.type) {
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
static const char *
sl_state_name(struct sl *sl, uint state)
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
static const char *
sl_i_state_name(struct sl *sl, uint state)
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
static uint
mtp_set_state(struct mtp *mtp, uint newstate)
{
	uint oldstate = mtp->i.state;

	if (newstate != oldstate) {
		mi_strlog(mtp->rq, STRLOGST, SL_TRACE, "%s <- %s",
			  mtp_state_name(mtp, newstate),
			  mtp_state_name(mtp, oldstate));
		mtp->i.state = newstate;
	}
	return (oldstate);
}

static uint
mtp_get_state(struct mtp *mtp)
{
	return (mtp->i.state);
}

static uint
sl_set_l_state(struct sl *sl, uint newstate)
{
	uint oldstate = sl->l_state;

	if (newstate != oldstate) {
		mi_strlog(sl->wq, STRLOGST, SL_TRACE, "%s <- %s",
			  sl_state_name(sl, newstate), sl_state_name(sl, oldstate));
		sl->l_state = newstate;
	}
	return (oldstate);
}

static uint
sl_get_l_state(struct sl *sl)
{
	return (sl->l_state);
}

static uint
sl_set_i_state(struct sl *sl, uint newstate)
{
	uint oldstate = sl->i.state;

	if (newstate != oldstate) {
		mi_strlog(sl->wq, STRLOGST, SL_TRACE, "%s <- %s",
			  sl_i_state_name(sl, newstate),
			  sl_i_state_name(sl, oldstate));
		sl->i.state = newstate;
	}
	return (oldstate);
}

static uint
sl_get_i_state(struct sl *sl)
{
	return (sl->i.state);
}

/*
 *  =========================================================================
 *
 *  MTP Functions
 *
 *  =========================================================================
 */
/**
 * mtp_check_src: - check souce (bind) address
 * @q: active queue
 * @mtp: MTP private structure
 * @src: the source (bind) address to check
 * @errp: pointer to error return
 *
 * Check the source (bind) address. This must be an address local to the stack
 * and the bind must be for an SI value which is not already bound (T_CLTS).
 */
static struct sp *
mtp_check_src(queue_t *q, struct mtp *mtp, struct mtp_addr *src, int *errp)
{
	struct sp *sp;
	struct T_info_ack *i;

	for (sp = master.sp.list; sp; sp = (struct sp *) sp->next)
		if (sp->ni == src->ni
		    && sp->pc == (src->pc & sp->na.na->mask.member))
			goto check;
	goto noaddr;
      check:
	if (!(i = sp->na.na->prot[src->si]))
		goto badaddr;
	if (i->SERV_type == T_CLTS && sp->mtp.lists[src->si])
		goto addrbusy;
	return (sp);
      noaddr:
	*errp = (TNOADDR);
	mi_strlog(q, 0, SL_TRACE, "bind: could not allocate source address");
	return (NULL);
      addrbusy:
	*errp = (TADDRBUSY);
	mi_strlog(q, 0, SL_TRACE, "bind: source address in use");
	return (NULL);
      badaddr:
	*errp = (TBADADDR);
	mi_strlog(q, 0, SL_TRACE, "bind: bad address");
	return (NULL);
}

/**
 * mtp_check_dst: - check destination (connect) address
 * @q: active queue
 * @mtp: MTP private structure
 * @dst: destination address (to connect)
 *
 * Check the destination (connect) address.  This may be a local, adjacent or
 * remote address and the connection must be for a destination point code and
 * SI value which is not already connected.
 */
static int
mtp_check_dst(queue_t *q, struct mtp *mtp, struct mtp_addr *dst)
{
	struct sp *sp;
	struct mtp *m2;

	for (sp = master.sp.list; sp; sp = (struct sp *) sp->next)
		if (sp->ni == dst->ni
		    && sp->pc == (dst->pc & sp->na.na->mask.member))
			goto check;
	goto noaddr;
      check:
	if (mtp->i.style == T_CLTS)
		goto efault;
	for (m2 = sp->mtp.lists[dst->si & 0x0f]; m2; m2 = m2->next)
		if (m2->src.ni == dst->ni
		    && m2->src.pc == (dst->pc & m2->sp.loc->na.na->mask.member))
			goto addrbusy;
	return (0);
      noaddr:
	mi_strlog(q, 0, SL_TRACE, "could not allocate source address");
	return (TNOADDR);
      addrbusy:
	mi_strlog(q, 0, SL_TRACE, "source address in use");
	return (TADDRBUSY);
      efault:
	mi_strlog(q, 0, SL_ERROR, "software error");
	return (-EFAULT);
}

/**
 * mtp_bind: - bind a local address
 * @q: active queue
 * @mtp: MTP private structure
 * @src: souce address (to bind)
 *
 * Add the MTP user to the local signalling point hashes if T_CLTS.
 */
static int
mtp_bind(queue_t *q, struct mtp *mtp, struct mtp_addr *src)
{
	struct sp *loc;
	int err = 0;

	if (!(loc = mtp_check_src(q, mtp, src, &err)))
		goto error;
	mtp->src = *src;
	if (!mtp->sp.loc)
		mtp->sp.loc = loc;
	mtp->prot = loc->na.na->prot[src->si];	/* point to protocol profile */
	loc->mtp.equipped |= (1 << src->si);
	mtp->i.style = mtp->prot->SERV_type;
	if (mtp->i.style == T_CLTS) {
		/* connectionless, add to user lists now */
		loc->mtp.available |= (1 << src->si);
		if ((mtp->sp.next = loc->mtp.lists[src->si]))
			mtp->sp.next->sp.prev = &mtp->sp.next;
		mtp->sp.prev = &loc->mtp.lists[src->si];
		loc->mtp.lists[src->si] = mtp;
	}
	/* connection oriented, add to user lists on connect only */
	return (0);
      error:
	return (err);
}

/**
 * mtp_unbind: - unbind an MTP endpoint
 * @mtp: MTP private structure
 *
 * Remove the MTP user from the local signalling point hashes if T_CLTS.
 */
static int
mtp_unbind(struct mtp *mtp)
{
	struct sp *loc;
	struct na *na;

	if (!mtp || !mtp->prot || !(loc = mtp->sp.loc) || !(na = loc->na.na))
		goto efault;
	if (mtp->i.style == T_CLTS) {
		/* connectionless, remove from user lists now */
		if ((*(mtp->sp.prev) = mtp->sp.next))
			mtp->sp.next->sp.prev = mtp->sp.prev;
		else
			loc->mtp.available &= ~(1 << mtp->src.si);
		mtp->sp.next = NULL;
	}
	/* connection oriented, already removed from user lists on disconnect */
	mtp->sp.loc = NULL;
	mtp->prot = NULL;
	return (0);
      efault:
	swerr();
	return (-EFAULT);
}

/**
 * mtp_connect: - connect an MTP endpoint
 * @mtp: MTP private structure
 * @dst: destination address (to connect)
 *
 * Add the MTP user to the local signalling point service hashes.
 */
static int
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
	mtp->sp.rem = rem;
	loc->mtp.available |= (1 << dst->si);
	if ((mtp->sp.next = loc->mtp.lists[dst->si]))
		mtp->sp.next->sp.prev = &mtp->sp.next;
	mtp->sp.prev = &loc->mtp.lists[dst->si];
	loc->mtp.lists[dst->si] = mtp;
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

/**
 * mtp_disconnect: - disconnect an MTP endpoint
 * @mtp: MTP private structure
 *
 * Remove the MTP user from the local signalling point service hashes.
 */
static int
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
	mtp->sp.rem = NULL;
	return (0);
      efault:
	swerr();
	return (-EFAULT);
}

static int mtp_send_user(queue_t *, struct sp *, uint, uint32_t, uint32_t, uint,
			 uint, uint, mblk_t *);
/**
 * mtp_end_msg: - send an MTP message
 * @q: active queue
 * @mtp: MTP private structure
 * @opt: MTP options
 * @dst: destination address (to which to send)
 * @dp: user data
 *
 * Send a user message.
 */
static int
mtp_send_msg(queue_t *q, struct mtp *mtp, struct mtp_opts *opt, struct mtp_addr *dst,
	     mblk_t *dp)
{
	struct sp *sp = mtp->sp.loc;
	uint mp = (opt && opt->mp) ? *opt->mp : 0;
	uint sls = (opt && opt->sls) ? *opt->sls : (sp->sls++ & sp->ls.sls_mask);

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
/**
 *  m_error: - issue M_ERROR message upstream
 *  @q: service queue
 *  @mtp: mtp private structure
 *  @msg: message to free upon success
 *  @error: error number
 */
static int
m_error(queue_t *q, struct mtp *mtp, mblk_t *msg, int error)
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
	if (likely((mp = mi_allocb(q, 2, BPRI_MED)) != NULL)) {
		if (hangup) {
			DB_TYPE(mp) = M_HANGUP;
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- M_HANGUP");
			freemsg(msg);
			putnext(mtp->rq, mp);
			return (0);
		} else {
			DB_TYPE(mp) = M_ERROR;
			*mp->b_wptr++ = error < 0 ? -error : error;
			*mp->b_wptr++ = error < 0 ? -error : error;
			mtp_set_state(mtp, TS_NO_STATES);
			freemsg(msg);
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- M_ERROR");
			putnext(mtp->rq, mp);
			return (0);
		}
	}
	return (-ENOBUFS);
}

/**
 * m_error_ack: - issue an MTP_ERROR_ACK -10 - Negative acknowledgement
 * @q: service queue
 * @mtp: mtp private structure
 * @prim: primitive in error
 * @err: error type
 */
static int
m_error_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, uint prim, uint err)
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
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ERROR_ACK;
		p->mtp_error_primitive = prim;
		p->mtp_mtpi_error = err < 0 ? MSYSERR : err;
		p->mtp_unix_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_ERROR_ACK");
		putnext(mtp->rq, mp);
		/* Returning -EPROTO here will make sure that the old state is
		   restored correctly (in mtp_w_proto).  If we return 0, then the
		   state will never be restored. */
		if (err >= 0)
			err = EPROTO;
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * m_ok_ack: - issue an MTP_OK_ACK - 9 - Positive acknowledgement
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
m_ok_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, uint prim)
{
	int err = -EFAULT;
	mblk_t *mp;
	struct MTP_ok_ack *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
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
			/* Note: if we are not in a WACK state we simply do not
			   change state.  This occurs normally when we are
			   responding to a MTP_OPTMGMT_REQ in other than MTPS_IDLE
			   state. */
			break;
		}
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_OK_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
      error:
	freemsg(mp);
	return m_error_ack(q, mtp, msg, prim, err);
}

/**
 * m_bind_ack: - issue an MTP_BIND_ACK - 11 - Bind acknowledgement
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @add: address bound
 */
static int
m_bind_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, struct mtp_addr *add)
{
	int err;
	mblk_t *mp;
	struct MTP_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		if ((err = mtp_bind(q, mtp, add)))
			goto error;
		mtp_set_state(mtp, MTPS_IDLE);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_BIND_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
      error:
	freemsg(mp);
	return m_error_ack(q, mtp, msg, MTP_BIND_REQ, err);
}

/**
 * m_addr_ack: - issue an MTP_ADDR_ACK - 12 - Address acknowledgement
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @loc: local address (or NULL)
 * @rem: remote address (or NULL)
 */
static int
m_addr_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, struct mtp_addr *loc,
	   struct mtp_addr *rem)
{
	mblk_t *mp;
	struct MTP_addr_ack *p;
	size_t loc_len = loc ? sizeof(*loc) : 0;
	size_t rem_len = rem ? sizeof(*rem) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ADDR_ACK;
		p->mtp_loc_length = loc_len;
		p->mtp_loc_offset = sizeof(*p);
		p->mtp_rem_length = rem_len;
		p->mtp_rem_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(loc, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
		bcopy(rem, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_ADDR_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * m_info_ack: - issue an MTP_INFO_ACK - 13 - Information acknowledgement
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 */
static int
m_info_ack(queue_t *q, struct mtp *mtp, mblk_t *msg)
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
	if ((mp = mi_allocb(q, sizeof(*p) + src_len + dst_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_msu_size = 279;
		p->mtp_addr_size = sizeof(struct mtp_addr);
		p->mtp_addr_length = src_len + dst_len;
		p->mtp_addr_offset = sizeof(*p);
		p->mtp_current_state = mtp_get_state(mtp);
		p->mtp_serv_type = (mtp->i.style == T_CLTS) ? M_CLMS : M_COMS;
		p->mtp_version = MTP_CURRENT_VERSION;
		mp->b_wptr += sizeof(*p);
		bcopy(&mtp->src, mp->b_wptr, sizeof(mtp->src));
		mp->b_wptr += sizeof(mtp->src);
		bcopy(&mtp->dst, mp->b_wptr, sizeof(mtp->dst));
		mp->b_wptr += sizeof(mtp->dst);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_INFO_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * m_optmgmt_ack: - issue an MTP_OPTMGMT_ACK - 14 - Options management acknowledgement
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @opt: MTP options
 * @flags: management flags
 */
static int
m_optmgmt_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, struct mtp_opts *opt,
	      mtp_ulong flags)
{
	mblk_t *mp;
	struct MTP_optmgmt_ack *p;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OPTMGMT_ACK;
		p->mtp_opt_length = opt_len;
		p->mtp_opt_offset = sizeof(*p);
		p->mtp_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		mtp_build_opts(mtp, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
		if (mtp_get_state(mtp) == MTPS_WACK_OPTREQ)
			mtp_set_state(mtp, MTPS_IDLE);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_OPTMGMT_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * m_transfer_ind: - issue an MTP_TRANSFER_IND - 15 - MTP data transfer indication
 * @q: active queue
 * @mtp: MTP private structure
 * @src: source address
 * @sls: signalling link selection
 * @pri: MTP message priority
 * @dp: user data
 */
static int
m_transfer_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *src, mtp_ulong sls,
	       mtp_ulong pri, mblk_t *dp)
{
	mblk_t *mp;
	struct MTP_transfer_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + src_len, BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mtp_primitive = MTP_TRANSFER_IND;
			p->mtp_srce_length = src_len;
			p->mtp_srce_offset = sizeof(*p);
			p->mtp_mp = pri;
			p->mtp_sls = sls;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;	/* absorb user data */
			mi_strlog(q, STRLOGRX, SL_TRACE, "<- MTP_TRANSFER_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);	/* free allocated message block */
		freeb(dp);	/* free user data block */
		return (-EBUSY);
	}
	freeb(dp);		/* free user data block */
	return (-ENOBUFS);
}

/**
 * m_pause_ind: - issue an MTP_PAUSE_IND - 16 - MTP pause (stop) indication
 * @q: active queue
 * @mtp: MTP private structure
 * @add: address that is inaccessible
 * @dp: user data (returned message if any)
 */
static int
m_pause_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *add, mblk_t *dp)
{
	mblk_t *mp;
	struct MTP_pause_ind *p;
	const size_t add_len = sizeof(*add);

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mtp_primitive = MTP_PAUSE_IND;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = sizeof(*p);
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		mp->b_cont = dp;
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_PAUSE_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * m_resume_ind: - issue an MTP_RESUME_IND - 17 - MTP resume (start) indication
 * @q: active queue
 * @mtp: MTP private structure
 * @add: address that is now acessible
 */
static int
m_resume_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *add)
{
	mblk_t *mp;
	struct MTP_resume_ind *p;
	const size_t add_len = sizeof(*add);

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mtp_primitive = MTP_RESUME_IND;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = sizeof(*p);
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_RESUME_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * m_status_ind: - issue an MTP_STATUS_IND - 18 - MTP status indication
 * @q: active queue
 * @mtp: MTP private structure
 * @add: address for which status is reported
 * @opt: MTP options
 * @dp: user data (returned data if any)
 * @etype: error type
 */
static int
m_status_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *add, struct mtp_opts *opt,
	     mblk_t *dp, mtp_ulong etype)
{
	mblk_t *mp;
	struct MTP_status_ind *p;
	const size_t add_len = sizeof(*add);

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mtp_primitive = MTP_STATUS_IND;
		p->mtp_addr_length = add_len;
		p->mtp_addr_offset = sizeof(*p);
		if ((etype & 0x07) == 0x03) {
			p->mtp_type = MTP_STATUS_TYPE_CONG;
			p->mtp_status = T_MTP_CONGESTION_STATUS(etype);
		} else {
			p->mtp_type = MTP_STATUS_TYPE_UPU;
			p->mtp_status = (etype & 0x07) - 4;
		}
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		mp->b_cont = dp;
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_STATUS_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * m_restart_complete_ind: - issue an MTP_RESTART_COMPLETE_IND - 19 - MTP restart complete (impl. dep.)
 * @q: active queue
 * @mtp: MTP private structure
 */
static inline int
m_restart_complete_ind(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	struct MTP_restart_complete_ind *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mtp_primitive = MTP_RESTART_COMPLETE_IND;
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- MTP_RESTART_COMPLETE_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_ind: - issue an N_CONN_IND - 11 - Incoming connection indication
 * @q: active queue
 * @mtp: MTP private structure
 * @seq: sequence number
 * @flags: connect flags
 * @src: source address (or NULL)
 * @dst: destination address (or NULL)
 * @qos: quality of service parameters
 */
static inline int
n_conn_ind(queue_t *q, struct mtp *mtp, np_ulong seq, np_ulong flags,
	   struct mtp_addr *src, struct mtp_addr *dst, N_qos_sel_conn_mtp_t *qos)
{
	mblk_t *mp;
	N_conn_ind_t *p;
	const size_t src_len = src ? sizeof(*src) : 0;
	const size_t dst_len = dst ? sizeof(*dst) : 0;
	const size_t qos_len = qos ? sizeof(*qos) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + src_len + dst_len + qos_len, BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = N_CONN_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p) + src_len;
			p->QOS_length = qos_len;
			p->QOS_offset = sizeof(*p) + src_len + dst_len;
			p->SEQ_number = seq;
			p->CONN_flags = flags;
			bcopy(mp->b_wptr, src, src_len);
			mp->b_wptr += src_len;
			bcopy(mp->b_wptr, dst, dst_len);
			mp->b_wptr += dst_len;
			bcopy(mp->b_wptr, qos, qos_len);
			mp->b_wptr += qos_len;
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_CONN_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_ind: - issue an N_CONN_CON - 12 - Connection established
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @flags: connect flags
 * @res: responding address
 * @qos: quality of service parameters
 */
static int
n_conn_con(queue_t *q, struct mtp *mtp, mblk_t *msg, np_ulong flags,
	   struct mtp_addr *res, N_qos_sel_conn_mtp_t *qos)
{
	mblk_t *mp;
	N_conn_con_t *p;
	const size_t res_len = res ? sizeof(*res) : 0;
	const size_t qos_len = qos ? sizeof(*qos) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + res_len, BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = sizeof(*p) + res_len;
			mp->b_wptr += sizeof(*p);
			bcopy(mp->b_wptr, res, res_len);
			mp->b_wptr += res_len;
			bcopy(mp->b_wptr, qos, qos_len);
			mp->b_wptr += qos_len;
			if (mtp_get_state(mtp) != NS_WCON_CREQ)
				swerr();
			mtp_set_state(mtp, NS_DATA_XFER);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_CONN_CON");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_ind: - issue an N_DISCON_IND - 13 - NC disconnected
 * @q: active queue
 * @mtp: MTP private structure
 * @orig: origin of disconnect
 * @reason: reason for disconnect
 * @seq: sequence number
 * @res: responding address
 * @dp: user data
 */
static inline int
n_discon_ind(queue_t *q, struct mtp *mtp, np_ulong orig, np_ulong reason,
	     np_ulong seq, struct mtp_addr *res, mblk_t *dp)
{
	mblk_t *mp;
	N_discon_ind_t *p;
	const size_t res_len = res ? sizeof(*res) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + res_len, BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_IND;
			p->DISCON_orig = orig;
			p->DISCON_reason = reason;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			bcopy(mp->b_wptr, res, res_len);
			mp->b_wptr += res_len;
			mp->b_cont = dp;
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DISCON_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_data_ind: - issue an N_DATA_IND - 14 - Incoming connection-mode data indication
 * @q: active queue
 * @mtp: MTP private structure
 * @flags: data transfer flags
 * @dp: user data
 *
 * Note that this function always frees the passed in user data message.
 */
static int
n_data_ind(queue_t *q, struct mtp *mtp, np_ulong flags, mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;
	int err = -ENOBUFS;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_IND;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;	/* absorb user data */
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- N_DATA_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);	/* free allocated message block */
		err = -EBUSY;
	}
	freeb(dp);		/* free user data block */
	return (err);
}

/**
 * n_exdata_ind: - issue an N_EXDATA_IND - 15 - Incoming expedited data indication
 * @q: active queue
 * @mtp: MTP private structure
 * @dp: user data
 *
 * Note that this function always frees the passed in user data message.
 */
static inline int
n_exdata_ind(queue_t *q, struct mtp *mtp, mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_ind_t *p;
	int err = -ENOBUFS;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(mtp->rq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- N_EXDATA_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		err = -EBUSY;
	}
	freeb(dp);
	return (err);
}

/**
 * n_info_ack: - issue an N_INFO_ACK - 16 - Information Acknowledgement
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 */
static int
n_info_ack(queue_t *q, struct mtp *mtp, mblk_t *msg)
{
	mblk_t *mp;
	N_info_ack_t *p;
	size_t src_len, dst_len, qos_len, qor_len;
	N_qos_sel_info_mtp_t *qos;
	N_qos_range_info_mtp_t *qor;
	struct sp *sp = mtp->sp.loc;
	struct na *na;

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
	if ((mp =
	     mi_allocb(q, sizeof(*p) + src_len + dst_len + qos_len + qor_len,
		       BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = N_INFO_ACK;
		p->NSDU_size = 279;
		p->ENSDU_size = 0;
		p->CDATA_size = 0;
		p->DDATA_size = 0;
		p->ADDR_size = sizeof(struct mtp_addr);
		p->ADDR_length = src_len + dst_len;
		p->ADDR_offset = sizeof(*p);
		p->QOS_length = qos_len;
		p->QOS_offset = sizeof(*p) + src_len + dst_len;
		p->QOS_range_length = qor_len;
		p->QOS_range_offset = sizeof(*p) + src_len + dst_len + qos_len;
		p->OPTIONS_flags = 0;
		p->NIDU_size = 279;
		p->SERV_type = mtp->i.style;
		p->CURRENT_state = mtp->i.state;
		p->PROVIDER_type = N_SNICFP;
		p->NODU_size = 279;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		p->NPI_version = N_VERSION_2;
		bcopy(&mtp->src, mp->b_wptr, src_len);
		mp->b_wptr += src_len;
		bcopy(&mtp->dst, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		if (qos_len) {
			qos = (typeof(qos)) mp->b_wptr;
			mp->b_wptr += sizeof(*qos);
			qos->n_qos_type = N_QOS_SEL_INFO_MTP;
			qos->pvar = na->option.pvar;
			qos->popt = na->option.popt;
		}
		if (qor_len) {
			qor = (typeof(qor)) mp->b_wptr;
			mp->b_wptr += sizeof(*qor);
			qor->n_qos_type = N_QOS_RANGE_INFO_MTP;
			qor->sls_range = sp->ls.sls_mask;
			qor->mp_range = (na->option.popt & SS7_POPT_MPLEV) ? 3 : 0;
		}
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_INFO_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_error_ack: - issue an N_ERROR_ACK - 18 - Error Acknowledgement
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @prim: primitive in error
 * @err: positive NPI error, negative UNIX error
 */
static int
n_error_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, np_ulong prim, np_long err)
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
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = err < 0 ? NSYSERR : err;
		p->UNIX_error = err < 0 ? -err : 0;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
		putnext(mtp->rq, mp);
		/* Retruning EPROTO here will make sure that the old state is
		   restored correctly. If we return 0, then the state will never be
		   restored. */
		return (EPROTO);
	}
	return (-ENOBUFS);
}

/**
 * n_ok_ack: - issue an N_OK_ACK - 19 - Success Acknowledgement
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
n_ok_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, np_ulong prim)
{
	mblk_t *mp;
	N_ok_ack_t *p;
	int err;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
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
			/* Note: if we are not in a WACK state we simply do not
			   change state.  This occurs normally when we are
			   responding to a T_OPTMGMT_REQ in other than TS_IDLE
			   state. */
			seldom();
			break;
		}
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_OK_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_bind_ack: - issue an N_BIND_ACK - 17 - NS User bound to network address
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @add: bound address
 */
static int
n_bind_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, struct mtp_addr *add)
{
	int err;
	mblk_t *mp;
	N_bind_ack_t *p;
	size_t add_len = add ? sizeof(*add) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = 0;
		p->TOKEN_value = (np_ulong) (ulong) mtp->rq;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		if ((err = mtp_bind(q, mtp, add)))
			goto free_error;
		mtp_set_state(mtp, NS_IDLE);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_BIND_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return n_error_ack(q, mtp, msg, N_BIND_REQ, err);
}

/**
 * n_unitdata_ind: - issue an N_UNITDATA_IND - 20 - Connection-less data receive indication
 * @q: active queue
 * @mtp: MTP private structure
 * @src: source address
 * @dst: destination address
 * @dp: user data
 *
 * Note that this function always frees the user data message.
 */
static int
n_unitdata_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *src,
	       struct mtp_addr *dst, mblk_t *dp)
{
	mblk_t *mp;
	N_unitdata_ind_t *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	int err = -ENOBUFS;

	if ((mp = mi_allocb(q, sizeof(*p) + src_len + dst_len, BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p) + src_len;
			mp->b_wptr += sizeof(*p);
			bcopy(src, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;	/* absorb user data */
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- N_UNITDATA_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);	/* free allocated message block */
		err = -EBUSY;
	}
	freeb(dp);		/* free user data block */
	return (err);
}

/**
 * n_uderror_ind: - issue an N_UDERROR_IND - 21 - UNITDATA Error Indication
 * @q: active queue
 * @mtp: MTP private structure
 * @dst: destination address of message
 * @opt: options associated with message
 * @dp: returned message
 * @etype: error type
 */
static int
n_uderror_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *dst,
	      struct mtp_opts *opt, mblk_t *dp, np_ulong etype)
{
	mblk_t *mp;
	N_uderror_ind_t *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
		if (bcanputnext(mtp->rq, 2)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_UDERROR_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_datack_ind: - issue an N_DATACK_IND - 24 - Data acknowledgement indication
 * @q: active queue
 * @mtp: MTP private structure
 */
static inline int
n_datack_ind(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	N_datack_ind_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (bcanputnext(mtp->rq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATACK_IND;
			mp->b_wptr += sizeof(*p);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_DATACK_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_reset_ind: - issue an N_RESET_IND - 26 - Incoming NC reset request indication
 * @q: active queue
 * @mtp: MTP private structure
 * @orig: origin of reset
 * @reason: reason for reset
 */
static int
n_reset_ind(queue_t *q, struct mtp *mtp, np_ulong orig, np_ulong reason)
{
	mblk_t *mp;
	N_reset_ind_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = orig;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_RESET_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_reset_con: - issue an N_RESET_CON - 28 - Reset processing complete
 * @q: active queue
 * @mtp: MTP private structure
 */
static inline int
n_reset_con(queue_t *q, struct mtp *mtp)
{
	mblk_t *mp;
	N_reset_con_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_CON;
			mp->b_wptr += sizeof(*p);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- N_RESET_CON");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_conn_ind: - issue a T_CONN_IND - 11 - 
 * @q: active queue
 * @mtp: MTP private structure
 */
static inline int
t_conn_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", DRV_NAME, mtp));
	return (-EFAULT);
}

/**
 * t_conn_con: - issue a T_CONN_CON - 12 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @res: responding address
 * @opt: connected options
 * @dp: user data
 */
static int
t_conn_con(queue_t *q, struct mtp *mtp, mblk_t *msg, struct mtp_addr *res,
	   struct mtp_opts *opt, mblk_t *dp)
{
	mblk_t *mp;
	struct T_conn_con *p;
	size_t res_len = res ? sizeof(*res) : 0;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + opt_len;
			mp->b_wptr += sizeof(*p);
			bcopy(res, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			mtp_build_opts(mtp, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CONN_CON");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_discon_ind: - issue a T_DISCON_IND - 13 - 
 * @q: active queue
 * @mtp: MTP private structure
 */
static inline int
t_discon_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", DRV_NAME, mtp));
	return (-EFAULT);
}

/**
 * t_data_ind: - issue a T_DATA_IND - 14 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @more: more flag
 * @dp: user data
 *
 * Note that the user data message is always freed.
 */
static int
t_data_ind(queue_t *q, struct mtp *mtp, t_uscalar_t more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_ind *p;
	int err = -ENOBUFS;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DATA_IND;
			p->MORE_flag = more;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;	/* absorb user data */
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- T_DATA_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);	/* free allocated message block */
		err = -EBUSY;
	}
	freeb(dp);		/* free user data block */
	return (err);
}

/**
 * t_exdata_ind: - issue a T_EXDATA_IND - 15 - 
 * @q: active queue
 * @mtp: MTP private structure
 */
static inline int
t_exdata_ind(queue_t *q, struct mtp *mtp)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", DRV_NAME, mtp));
	return (-EFAULT);
}

/**
 * t_info_ack: - issue a T_INFO_ACK - 16 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 */
static int
t_info_ack(queue_t *q, struct mtp *mtp, mblk_t *msg)
{
	mblk_t *mp;
	struct T_info_ack *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = T_INFO_ACK;
		if (mtp->prot) {
			*p = *(mtp->prot);
		} else {
			p->PRIM_type = T_INFO_ACK;
			p->TSDU_size = 272;
			p->ETSDU_size = T_INVALID;
			p->CDATA_size = T_INVALID;
			p->DDATA_size = T_INVALID;
			p->ADDR_size = sizeof(struct sockaddr_mtp);
			p->OPT_size =
			    4 * (sizeof(struct t_opthdr) + sizeof(t_scalar_t));
			p->TIDU_size = 272;
			p->PROVIDER_flag = T_XPG4_1;
		}
		p->SERV_type = mtp->i.style;
		p->CURRENT_state = mtp->i.state;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_INFO_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_error_ack: - issue a T_ERROR_ACK - 18 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: positive TLI error, negative UNIX error
 */
static int
t_error_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, t_uscalar_t prim,
	    t_scalar_t error)
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
	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = error < 0 ? TSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ERROR_ACK");
		putnext(mtp->rq, mp);
		/* Retruning EPROTO here will make sure that the old state is
		   restored correctly. If we return 0, then the state will never be
		   restored. */
		return (EPROTO);
	}
	return (-ENOBUFS);
}

/**
 * t_ok_ack: - issue a T_OK_ACK - 19 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
t_ok_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, t_uscalar_t prim)
{
	int err = -EFAULT;
	mblk_t *mp;
	struct T_ok_ack *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
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
			/* Note: if we are not in a WACK state we simply do not
			   change state.  This occurs normally when we are
			   responding to a T_OPTMGMT_REQ in other than TS_IDLE
			   state. */
			break;
		}
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OK_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(q, mtp, msg, prim, err);
}

/**
 * t_bind_ack: - issue a T_BIND_ACK - 17 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @add: bound address
 */
static int
t_bind_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, struct mtp_addr *add)
{
	int err;
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = sizeof(*p);
		p->CONIND_number = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		if ((err = mtp_bind(q, mtp, add)))
			goto free_error;
		mtp_set_state(mtp, TS_IDLE);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_BIND_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(q, mtp, msg, T_BIND_REQ, err);
}

/**
 * t_unitdata_ind: - issue a T_UNITDATA_IND - 20 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @src: source address
 * @opt: message options
 * @dp: user data
 *
 * Note that this function always frees the user data message.
 */
static int
t_unitdata_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *src,
	       struct mtp_opts *opt, mblk_t *dp)
{
	mblk_t *mp;
	struct T_unitdata_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	int err = -ENOBUFS;

	if ((mp = mi_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
		if (canputnext(mtp->rq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UNITDATA_IND;
			p->SRC_length = src_len;
			p->SRC_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + src_len;
			mp->b_wptr += sizeof(*p);
			bcopy(src, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mtp_build_opts(mtp, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;	/* absorb user data */
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- T_UNITDATA_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);	/* free allocated message block */
		err = -EBUSY;
	}
	freeb(dp);		/* free user data block */
	return (err);
}

/**
 * t_uderror_ind: - issue a T_UDERROR_IND - 21 - Unitdata error indication
 * @q: active queue
 * @mtp: MTP private structure
 * @dst: destination of (original) message
 * @opt: options of (original) message
 * @dp: user data (original message)
 * @etype: error type
 *
 * This primitive indicates to the MTP user that a message with the specified
 * destination address and options produced an error.
 */
static int
t_uderror_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *dst,
	      struct mtp_opts *opt, mblk_t *dp, t_uscalar_t etype)
{
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		if (bcanputnext(mtp->rq, 2)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 2;	/* XXX move ahead of data indications */
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_UDERROR_IND;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p) + dst_len;
			p->ERROR_type = etype;
			mp->b_wptr += sizeof(*p);
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mtp_build_opts(mtp, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- T_UDERROR_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * t_optmgmt_ack: - issue a T_OPTMGMT_ACK - 22 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @flags: management flags
 * @opt: options
 */
static int
t_optmgmt_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, t_uscalar_t flags,
	      struct mtp_opts *opt)
{
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = sizeof(*p);
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		mtp_build_opts(mtp, opt, mp->b_wptr);
		mp->b_wptr += opt_len;
#ifdef TS_WACK_OPTREQ
		if (mtp_get_state(mtp) == TS_WACK_OPTREQ)
			mtp_set_state(mtp, TS_IDLE);
#endif
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_OPTMGMT_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_ordrel_ind: - issue a T_ORDREL_IND - 23 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 */
static int
t_ordrel_ind(queue_t *q, struct mtp *mtp, mblk_t *msg)
{
	pswerr(("%s: %p: ERROR: unsupported primitive\n", DRV_NAME, mtp));
	freemsg(msg);
	return (EPROTO);
}

/**
 * t_optdata_ind: - issue a T_OPTDATA_IND - 26 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @flags: data transfer flags
 * @opt: options
 * @dp: user data
 *
 * Note that this function always frees the user data message.
 */
static inline int
t_optdata_ind(queue_t *q, struct mtp *mtp, t_uscalar_t flags, struct mtp_opts *opt,
	      mblk_t *dp)
{
	mblk_t *mp;
	struct T_optdata_ind *p;
	size_t opt_len = opt ? mtp_opts_size(mtp, opt) : 0;
	int err = -ENOBUFS;
	int band = (flags & T_ODF_EX) ? 1 : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		if (bcanputnext(mtp->rq, band)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = band;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_OPTDATA_IND;
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p);
			p->DATA_flag = flags;
			mp->b_wptr += sizeof(*p);
			mtp_build_opts(mtp, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			mi_strlog(q, STRLOGDA, SL_TRACE, "<- T_OPTDATA_IND");
			putnext(mtp->rq, mp);
			return (0);
		}
		freeb(mp);
		err = -EBUSY;
	}
	freemsg(dp);
	return (err);
}

#ifdef T_ADDR_ACK
/**
 * t_addr_ack: - issue a T_ADDR_ACK - 27 - 
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @loc: local (bound) address
 * @rem: remote (connected) address
 */
static int
t_addr_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, struct mtp_addr *loc,
	   struct mtp_addr *rem)
{
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = loc ? sizeof(*loc) : 0;
	size_t rem_len = rem ? sizeof(*rem) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = sizeof(*p);
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(loc, mp->b_wptr, loc_len);
		mp->b_wptr += loc_len;
		bcopy(rem, mp->b_wptr, rem_len);
		mp->b_wptr += rem_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_ADDR_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#ifdef T_CAPABILITY_ACK
/**
 * t_capability_ack: - issue a T_CAPABILITY_ACK
 * @q: active queue
 * @mtp: MTP private structure
 * @msg: message to free upon success
 * @caps: capability bits
 */
static int
t_capability_ack(queue_t *q, struct mtp *mtp, mblk_t *msg, t_uscalar_t caps)
{
	mblk_t *mp;
	struct T_capability_ack *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = TC1_INFO;
		p->ACCEPTOR_id =
		    (caps & TC1_ACCEPTOR_ID) ? (t_uscalar_t) (ulong) mtp->rq : 0;
		if (caps & TC1_INFO) {
			if (mtp->prot) {
				p->INFO_ack = *(mtp->prot);
			} else {
				p->INFO_ack.PRIM_type = T_INFO_ACK;
				p->INFO_ack.TSDU_size = 272;
				p->INFO_ack.ETSDU_size = T_INVALID;
				p->INFO_ack.CDATA_size = T_INVALID;
				p->INFO_ack.DDATA_size = T_INVALID;
				p->INFO_ack.ADDR_size = sizeof(struct sockaddr_mtp);
				p->INFO_ack.OPT_size =
				    4 * (sizeof(struct t_opthdr) +
					 sizeof(t_scalar_t));
				p->INFO_ack.TIDU_size = 272;
				p->INFO_ack.PROVIDER_flag = T_XPG4_1;
			}
			p->INFO_ack.SERV_type = mtp->i.style;
			p->INFO_ack.CURRENT_state = mtp->i.state;
		} else {
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- T_CAPABILITY_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
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
/**
 * sl_pdu_req: - issue an SL_PDU_REQ
 * @q: active queue
 * @sl: SL private structure
 * @dp: user data
 *
 * Note that this function always frees the user data message.
 */
static inline int
sl_pdu_req(queue_t *q, struct sl *sl, mblk_t *dp)
{
	mblk_t *mp;
	sl_pdu_req_t *p;
	int err = -ENOBUFS;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->wq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_PDU_REQ;
			p->sl_mp = 0;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			mi_strlog(q, STRLOGDA, SL_TRACE, "SL_DPU_DEQ [%d] ->",
				  (int) msgdsize(dp));
			putnext(sl->wq, mp);
			return (0);
		}
		freeb(mp);
		err = -EBUSY;
	}
	freemsg(dp);
	return (err);
}

/**
 * sl_emergency_req: - issue an SL_EMERGENCY_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static inline int
sl_emergency_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_emergency_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_EMERGENCY_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_EMERGENCY_REQ ->");
		/* does not change interface state */
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_emergency_ceases_req: - issue an SL_EMERGENCY_CEASES_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static inline int
sl_emergency_ceases_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_emergency_ceases_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_EMEGENCY_CEASES_REQ ->");
		/* does not change interface state */
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_start_req: - issue an SL_START_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static int
sl_start_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_start_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_START_REQ;
		mp->b_wptr += sizeof(*p);
		/* routing state of link does not change */
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_START_REQ ->");
		sl_set_l_state(sl, SLS_WIND_INSI);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_stop_req: - issue an SL_STOP_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static int
sl_stop_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_stop_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_STOP_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_STOP_REQ ->");
		sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieve_bsnt_req: - issue an * SL_RETRIEVE_BSNT_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static int
sl_retrieve_bsnt_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_retrieve_bsnt_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVE_BSNT_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_RETRIEVE_BSNT_REQ ->");
		sl_set_l_state(sl, SLS_WIND_BSNT);
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_request_and_fsnc_req: - issue an SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 * @q: active queue
 * @sl: SL private structure
 * @fsnc: FSNC from which to retrieve
 */
static int
sl_retrieval_request_and_fsnc_req(queue_t *q, struct sl *sl, lmi_ulong fsnc)
{
	mblk_t *mp;
	sl_retrieval_req_and_fsnc_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->wq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RETRIEVAL_REQUEST_AND_FSNC_REQ;
			p->sl_fsnc = fsnc;
			mp->b_wptr += sizeof(*p);
			mi_strlog(q, STRLOGTX, SL_TRACE,
				  "SL_RETRIEVAL_REQUEST_AND_FSNC_REQ ->");
			sl_set_l_state(sl, SLS_WCON_RET);
			putnext(sl->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_resume_req: - issue an SL_RESUME_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static inline int
sl_resume_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_resume_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->wq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_RESUME_REQ;
			mp->b_wptr += sizeof(*p);
			mi_strlog(q, STRLOGTX, SL_TRACE, "SL_RESUME_REQ ->");
			putnext(sl->wq, mp);
			return (0);
		}
		freemsg(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_clear_buffers_req: - issue an SL_CLEAR_BUFFERS_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static int
sl_clear_buffers_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_clear_buffers_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->wq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
			mp->b_wptr += sizeof(*p);
			mi_strlog(q, STRLOGTX, SL_TRACE, "SL_CLEAR_BUFFERS_REQ ->");
			sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
			putnext(sl->wq, mp);
			return (0);
		}
		freemsg(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_clear_rtb_req: - issue an SL_CLEAR_RTB_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static int
sl_clear_rtb_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_clear_rtb_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->wq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_CLEAR_RTB_REQ;
			mp->b_wptr += sizeof(*p);
			mi_strlog(q, STRLOGTX, SL_TRACE, "SL_CLEAR_RTB_REQ ->");
			sl_set_l_state(sl, SLS_WIND_CLRB);
			putnext(sl->wq, mp);
			return (0);
		}
		freemsg(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_local_processor_outage_req: - issue an SL_LOCAL_PROCESSOR_OUTAGE_REQ
 * @q: active queue
 * @sl: SL private structure
 * @msg: message to free upon success
 */
static inline int
sl_local_processor_outage_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_local_proc_outage_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_LOCAL_PROCESSOR_OUTAGE_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_congestion_discard_req: - issue an SL_CONGESTION_DISCARD_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static inline int
sl_congestion_discard_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_cong_discard_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_CONGESTION_DISCARD_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_congestion_accept_req: - issue an SL_CONGESTION_ACCEPT_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static inline int
sl_congestion_accept_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_cong_accept_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_CONGESTION_ACCEPT_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_no_congestion_req: - issue an SL_NO_CONGESTION_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static inline int
sl_no_congestion_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_no_cong_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_NO_CONGESTION_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_NO_CONGESTION_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_power_on_req: - issue an SL_POWER_ON_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static inline int
sl_power_on_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_power_on_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(sl->wq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_POWER_ON_REQ;
			mp->b_wptr += sizeof(*p);
			/* should maybe set routing state to active */
			mi_strlog(q, STRLOGTX, SL_TRACE, "SL_POWER_ON_REQ ->");
			sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
			putnext(sl->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * sl_optmgmt_req: - issue an SL_OPTMGMT_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static int
sl_optmgmt_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_optmgmt_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OPTMGMT_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_OPTMGMT_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_notify_req: - issue an SL_NOTIFY_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static int
sl_notify_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_notify_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_NOTIFY_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "SL_NOTIFY_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/**
 * lmi_info_req: - issue an LMI_INFO_REQ
 * @q: active queue
 * @sl: SL private structure
 * @msg: message to free upon success
 */
static inline int
lmi_info_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_info_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "LMI_INFO_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_attach_req: - issue an LMI_ATTACH_REQ
 * @q: active queue
 * @sl: SL private structure
 * @ppa_ptr: PPA pointer
 * @ppa_len: PPA length
 */
static inline int
lmi_attach_req(queue_t *q, struct sl *sl, caddr_t ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_attach_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ATTACH_REQ;
		mp->b_wptr += sizeof(*p);
		if (ppa_ptr && ppa_len) {
			bcopy(ppa_ptr, mp->b_wptr, ppa_len);
			mp->b_wptr += ppa_len;
		}
		mi_strlog(q, STRLOGTX, SL_TRACE, "LMI_ATTACH_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_detach_req: - issue an LMI_DETACH_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static inline int
lmi_detach_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_detach_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "LMI_DETACH_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_enable_req: - issue an LMI_ENABLE_REQ
 * @q: active queue
 * @sl: SL private structure
 * @dst_ptr: destination pointer
 * @dst_len: destination length
 */
static inline int
lmi_enable_req(queue_t *q, struct sl *sl, caddr_t dst_ptr, size_t dst_len)
{
	mblk_t *mp;
	lmi_enable_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		if (dst_ptr && dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		mi_strlog(q, STRLOGTX, SL_TRACE, "LMI_ENABLE_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_req: - issue an LMI_DISABLE_REQ
 * @q: active queue
 * @sl: SL private structure
 */
static inline int
lmi_disable_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	lmi_disable_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(q, STRLOGTX, SL_TRACE, "LMI_DISABLE_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_optmgmt_req: - issue an LMI_OPTMGMT_REQ
 * @q: active queue
 * @sl: SL private structure
 * @flags: management flags
 * @opt_ptr: options pointer
 * @opt_len: options length
 */
static inline int
lmi_optmgmt_req(queue_t *q, struct sl *sl, lmi_ulong flags, caddr_t opt_ptr,
		size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_REQ;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = sizeof(*p);
		p->lmi_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mi_strlog(q, STRLOGTX, SL_TRACE, "LMI_OPTMTMT_REQ ->");
		putnext(sl->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  MESSAGE ENCODING
 *
 *  =========================================================================
 */
struct mtp_msg {
	mblk_t *bp;			/* identd */
	queue_t *eq;			/* queue to write errors to */
	queue_t *xq;			/* queue to write results to */
	struct mtp *mtp;		/* MTP-User to which this message belongs */
	struct sl *sl;			/* Signalling Link to which these message
					   belongs */
	unsigned long timestamp;	/* jiffy clock time stamp */
	uint pvar;			/* protocol variant */
	uint popt;			/* protocol options */
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
		uint fsnl;		/* forward sequence number last acknowledged 
					 */
		uint cbc;		/* changeback code */
		uint stat;		/* status */
		uint sdli;		/* signalling data link identifier */
		uint upi;		/* user part identifier */
	} arg;
	caddr_t data;			/* user data */
	size_t dlen;			/* user data length */
};

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
static int
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
static int
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
static int
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
static int
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
static int
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
static int
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
static int
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
static int
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
static int
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
static void
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
			*bp->b_wptr++ =
			    (si & 0x0f) | ((mp & 0x3) << 4) | ((ni & 0x3) << 6);
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
static void
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
static void
mtp_enc_sif(mblk_t *mp, struct sp *sp, const uint h0, const uint h1)
{
	(void) sp;
	*mp->b_wptr++ = (h0 & 0x0f) | ((h1 & 0x0f) << 4);
}

#define max_msg_size (6 + 1 + 7 + 1 + 3 + 1)
static mblk_t *
mtp_enc_msg(queue_t *q, struct sp *sp, uint ni, uint mp, uint si, uint dpc, uint opc,
	    uint sls)
{
	mblk_t *bp;

	if ((bp = mi_allocb(q, max_msg_size, BPRI_MED))) {
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

static struct T_info_ack t_cots_prot_default = {
	.PRIM_type = T_INFO_ACK,
	.TSDU_size = 272,
	.ETSDU_size = T_INVALID,
	.CDATA_size = T_INVALID,
	.DDATA_size = T_INVALID,
	.ADDR_size = sizeof(struct sockaddr_mtp),
	.OPT_size = 4 * (sizeof(struct t_opthdr) + sizeof(t_scalar_t)),
	.TIDU_size = 272,
	.SERV_type = T_CLTS,
	.CURRENT_state = TS_UNBND,
	.PROVIDER_flag = T_XPG4_1,
};
static struct T_info_ack t_clts_prot_default = {
	.PRIM_type = T_INFO_ACK,
	.TSDU_size = 272,
	.ETSDU_size = T_INVALID,
	.CDATA_size = T_INVALID,
	.DDATA_size = T_INVALID,
	.ADDR_size = sizeof(struct sockaddr_mtp),
	.OPT_size = 4 * (sizeof(struct t_opthdr) + sizeof(t_scalar_t)),
	.TIDU_size = 272,
	.SERV_type = T_COTS,
	.CURRENT_state = TS_UNBND,
	.PROVIDER_flag = T_XPG4_1,
};

static mtp_opt_conf_na_t itut_na_config_default = {
	.t1 = 800,			/* (sl) T1 0.5 (0.8) 1.2 sec */
	.t1r = 800,			/* (sp) T1R 0.5 (0.8) 1.2 sec */
	.t2 = 1400,			/* (sl) T2 0.7 (1.4) 2.0 sec */
	.t3 = 800,			/* (sl) T3 0.5 (0.8) 1.2 sec */
	.t4 = 800,			/* (sl) T4 0.5 (0.8) 1.2 sec */
	.t5 = 800,			/* (sl) T5 0.5 (0.8) 1.2 sec */
	.t6 = 800,			/* (rt) T6 0.5 (0.8) 1.2 sec */
	.t7 = 1500,			/* (lk) T7 1 to 2 sec */
	.t8 = 1000,			/* (rs) T8 0.8 to 1.2 sec */
	.t10 = 45000,			/* (rt) T10 30 to 60 sec */
	.t11 = 60000,			/* (rs) T11 30 to 90 sec */
	.t12 = 1150,			/* (sl) T12 0.8 to 1.5 sec */
	.t13 = 1150,			/* (sl) T13 0.8 to 1.5 sec */
	.t14 = 2500,			/* (sl) T14 2 to 3 sec */
	.t15 = 2500,			/* (rs) T15 2 to 3 sec */
	.t16 = 1700,			/* (rs) T16 1.4 to 2.0 sec */
	.t17 = 1150,			/* (sl) T17 0.8 to 1.5 sec */
	.t18 = 600000,			/* (sp) T18 network dependent */
	.t18a = 12000,			/* (rs) T18A 2 to 20 sec */
	.t19 = 68000,			/* (sp) T19 67 to 69 sec */
	.t19a = 540000,			/* (sl) T19A 480 to 600 sec */
	.t20 = 60000,			/* (sp) T20 59 to 61 sec */
	.t20a = 105000,			/* (sl) T20A 90 to 120 sec */
	.t21 = 64000,			/* (sp) T21 63 to 65 sec */
	.t21a = 105000,			/* (sl) T21A 90 to 120 sec */
	.t22 = 270000,			/* (sl) T22 3 to 6 min */
	.t22a = 600000,			/* (sp) T22A network dependent */
	.t23 = 270000,			/* (sl) T23 3 to 6 min */
	.t23a = 600000,			/* (sp) T23A network dependent */
	.t24 = 500,			/* (sl) T24 500 ms */
	.t24a = 600000,			/* (sp) T24A network dependent */
	.t25a = = 32000,		/* (sp) T25 32 to 35 sec */
	.t26a = 13000,			/* (sp) T26 12 to 15 sec */
	.t27a = 3000,			/* (sp) T27 2 (3) 5 sec */
	.t28a = 19000,			/* (sp) T28 3 to 35 sec */
	.t29a = 63000,			/* (sp) T29 60 to 65 sec */
	.t30a = 33000,			/* (sp) T30 30 to 35 sec */
	.t31a = 60000,			/* (sl) T31 10 to 120 sec */
	.t32a = 60000,			/* (sl) T32 5 to 120 sec */
	.t33a = 360000,			/* (sl) T33 60 to 600 sec */
	.t34a = 60000,			/* (sl) T34 5 to 120 sec */
	.t1t = 4000,			/* (sl) T1T 4 to 12 sec */
	.t2t = 60000,			/* (sl) T2T 30 to 90 sec */
	.t1s = 4000,			/* (sl) T1S 4 to 12 sec */
};

static mtp_opt_conf_na_t etsi_na_config_default = {
	.t1 = 800,			/* (sl) T1 0.5 (0.8) 1.2 sec */
	.t1r = 800,			/* (sp) T1R 0.5 (0.8) 1.2 sec */
	.t2 = 1400,			/* (sl) T2 0.7 (1.4) 2.0 sec */
	.t3 = 800,			/* (sl) T3 0.5 (0.8) 1.2 sec */
	.t4 = 800,			/* (sl) T4 0.5 (0.8) 1.2 sec */
	.t5 = 800,			/* (sl) T5 0.5 (0.8) 1.2 sec */
	.t6 = 800,			/* (rt) T6 0.5 (0.8) 1.2 sec */
	.t7 = 1500,			/* (lk) T7 1 to 2 sec */
	.t8 = 1000,			/* (rs) T8 0.8 to 1.2 sec */
	.t10 = 45000,			/* (rt) T10 30 to 60 sec */
	.t11 = 60000,			/* (rs) T11 30 to 90 sec */
	.t12 = 1150,			/* (sl) T12 0.8 to 1.5 sec */
	.t13 = 1150,			/* (sl) T13 0.8 to 1.5 sec */
	.t14 = 2500,			/* (sl) T14 2 to 3 sec */
	.t15 = 2500,			/* (rs) T15 2 to 3 sec */
	.t16 = 1700,			/* (rs) T16 1.4 to 2.0 sec */
	.t17 = 1150,			/* (sl) T17 0.8 to 1.5 sec */
	.t18 = 600000,			/* (sp) T18 network dependent */
	.t18a = 12000,			/* (rs) T18A 2 to 20 sec */
	.t19 = 68000,			/* (sp) T19 67 to 69 sec */
	.t19a = 540000,			/* (sl) T19A 480 to 600 sec */
	.t20 = 60000,			/* (sp) T20 59 to 61 sec */
	.t20a = 105000,			/* (sl) T20A 90 to 120 sec */
	.t21 = 64000,			/* (sp) T21 63 to 65 sec */
	.t21a = 105000,			/* (sl) T21A 90 to 120 sec */
	.t22 = 270000,			/* (sl) T22 3 to 6 min */
	.t22a = 600000,			/* (sp) T22A network dependent */
	.t23 = 270000,			/* (sl) T23 3 to 6 min */
	.t23a = 600000,			/* (sp) T23A network dependent */
	.t24 = 500,			/* (sl) T24 500 ms */
	.t24a = 600000,			/* (sp) T24A network dependent */
	.t25a = 32000,			/* (sp) T25 32 to 35 sec */
	.t26a = 13000,			/* (sp) T26 12 to 15 sec */
	.t27a = 3000,			/* (sp) T27 2 (3) 5 sec */
	.t28a = 19000,			/* (sp) T28 3 to 35 sec */
	.t29a = 63000,			/* (sp) T29 60 to 65 sec */
	.t30a = 33000,			/* (sp) T30 30 to 35 sec */
	.t31a = 60000,			/* (sl) T31 10 to 120 sec */
	.t32a = 60000,			/* (sl) T32 5 to 120 sec */
	.t33a = 360000,			/* (sl) T33 60 to 600 sec */
	.t34a = 60000,			/* (sl) T34 5 to 120 sec */
	.t1t = 4000,			/* (sl) T1T 4 to 12 sec */
	.t2t = 60000,			/* (sl) T2T 30 to 90 sec */
	.t1s = 4000,			/* (sl) T1S 4 to 12 sec */
};

static mtp_opt_conf_na_t ansi_na_config_default = {
	.t1 = 800,			/* (sl) T1 0.5 (0.8) 1.2 sec */
	.t1r = 800,			/* (sp) T1R 0.5 (0.8) 1.2 sec */
	.t2 = 1400,			/* (sl) T2 0.7 (1.4) 2.0 sec */
	.t3 = 800,			/* (sl) T3 0.5 (0.8) 1.2 sec */
	.t4 = 800,			/* (sl) T4 0.5 (0.8) 1.2 sec */
	.t5 = 800,			/* (sl) T5 0.5 (0.8) 1.2 sec */
	.t6 = 800,			/* (rt) T6 0.5 (0.8) 1.2 sec */
	.t7 = 1500,			/* (lk) T7 1 to 2 sec */
	.t8 = 1000,			/* (rs) T8 0.8 to 1.2 sec */
	.t10 = 45000,			/* (rt) T10 30 to 60 sec */
	.t11 = 60000,			/* (rs) T11 30 to 90 sec */
	.t12 = 1150,			/* (sl) T12 0.8 to 1.5 sec */
	.t13 = 1150,			/* (sl) T13 0.8 to 1.5 sec */
	.t14 = 2500,			/* (sl) T14 2 to 3 sec */
	.t15 = 2500,			/* (rs) T15 2 to 3 sec */
	.t16 = 1700,			/* (rs) T16 1.4 to 2.0 sec */
	.t17 = 1150,			/* (sl) T17 0.8 to 1.5 sec */
	.t18 = 600000,			/* (sp) T18 network dependent */
	.t18a = 12000,			/* (rs) T18A 2 to 20 sec */
	.t19 = 68000,			/* (sp) T19 67 to 69 sec */
	.t19a = 540000,			/* (sl) T19A 480 to 600 sec */
	.t20 = 60000,			/* (sp) T20 59 to 61 sec */
	.t20a = 105000,			/* (sl) T20A 90 to 120 sec */
	.t21 = 64000,			/* (sp) T21 63 to 65 sec */
	.t21a = 105000,			/* (sl) T21A 90 to 120 sec */
	.t22 = 270000,			/* (sl) T22 3 to 6 min */
	.t22a = 600000,			/* (sp) T22A network dependent */
	.t23 = 270000,			/* (sl) T23 3 to 6 min */
	.t23a = 600000,			/* (sp) T23A network dependent */
	.t24 = 500,			/* (sl) T24 500 ms */
	.t24a = 600000,			/* (sp) T24A network dependent */
	.t25a = 32000,			/* (sp) T25 32 to 35 sec */
	.t26a = 13000,			/* (sp) T26 12 to 15 sec */
	.t27a = 3000,			/* (sp) T27 2 (3) 5 sec */
	.t28a = 19000,			/* (sp) T28 3 to 35 sec */
	.t29a = 63000,			/* (sp) T29 60 to 65 sec */
	.t30a = 33000,			/* (sp) T30 30 to 35 sec */
	.t31a = 60000,			/* (sl) T31 10 to 120 sec */
	.t32a = 60000,			/* (sl) T32 5 to 120 sec */
	.t33a = 360000,			/* (sl) T33 60 to 600 sec */
	.t34a = 60000,			/* (sl) T34 5 to 120 sec */
	.t1t = 4000,			/* (sl) T1T 4 to 12 sec */
	.t2t = 60000,			/* (sl) T2T 30 to 90 sec */
	.t1s = 4000,			/* (sl) T1S 4 to 12 sec */
};

static mtp_opt_conf_na_t jttc_na_config_default = {
	.t1 = 800,			/* (sl) T1 0.5 (0.8) 1.2 sec */
	.t1r = 800,			/* (sp) T1R 0.5 (0.8) 1.2 sec */
	.t2 = 1400,			/* (sl) T2 0.7 (1.4) 2.0 sec */
	.t3 = 800,			/* (sl) T3 0.5 (0.8) 1.2 sec */
	.t4 = 800,			/* (sl) T4 0.5 (0.8) 1.2 sec */
	.t5 = 800,			/* (sl) T5 0.5 (0.8) 1.2 sec */
	.t6 = 800,			/* (rt) T6 0.5 (0.8) 1.2 sec */
	.t7 = 1500,			/* (lk) T7 1 to 2 sec */
	.t8 = 1000,			/* (rs) T8 0.8 to 1.2 sec */
	.t10 = 45000,			/* (rt) T10 30 to 60 sec */
	.t11 = 60000,			/* (rs) T11 30 to 90 sec */
	.t12 = 1150,			/* (sl) T12 0.8 to 1.5 sec */
	.t13 = 1150,			/* (sl) T13 0.8 to 1.5 sec */
	.t14 = 2500,			/* (sl) T14 2 to 3 sec */
	.t15 = 2500,			/* (rs) T15 2 to 3 sec */
	.t16 = 1700,			/* (rs) T16 1.4 to 2.0 sec */
	.t17 = 1150,			/* (sl) T17 0.8 to 1.5 sec */
	.t18 = 600000,			/* (sp) T18 network dependent */
	.t18a = 12000,			/* (rs) T18A 2 to 20 sec */
	.t19 = 68000,			/* (sp) T19 67 to 69 sec */
	.t19a = 540000,			/* (sl) T19A 480 to 600 sec */
	.t20 = 60000,			/* (sp) T20 59 to 61 sec */
	.t20a = 105000,			/* (sl) T20A 90 to 120 sec */
	.t21 = 64000,			/* (sp) T21 63 to 65 sec */
	.t21a = 105000,			/* (sl) T21A 90 to 120 sec */
	.t22 = 270000,			/* (sl) T22 3 to 6 min */
	.t22a = 600000,			/* (sp) T22A network dependent */
	.t23 = 270000,			/* (sl) T23 3 to 6 min */
	.t23a = 600000,			/* (sp) T23A network dependent */
	.t24 = 500,			/* (sl) T24 500 ms */
	.t24a = 600000,			/* (sp) T24A network dependent */
	.t25a = 32000,			/* (sp) T25 32 to 35 sec */
	.t26a = 13000,			/* (sp) T26 12 to 15 sec */
	.t27a = 3000,			/* (sp) T27 2 (3) 5 sec */
	.t28a = 19000,			/* (sp) T28 3 to 35 sec */
	.t29a = 63000,			/* (sp) T29 60 to 65 sec */
	.t30a = 33000,			/* (sp) T30 30 to 35 sec */
	.t31a = 60000,			/* (sl) T31 10 to 120 sec */
	.t32a = 60000,			/* (sl) T32 5 to 120 sec */
	.t33a = 360000,			/* (sl) T33 60 to 600 sec */
	.t34a = 60000,			/* (sl) T34 5 to 120 sec */
	.t1t = 4000,			/* (sl) T1T 4 to 12 sec */
	.t2t = 60000,			/* (sl) T2T 30 to 90 sec */
	.t1s = 4000,			/* (sl) T1S 4 to 12 sec */
};

struct mtp_timer {
	int timer;
	uint count;
	union {
		struct sp *sp;
		struct rs *rs;
		struct rt *rt;
		struct mtp *mtp;
		struct cb *cb;
		struct sr *sr;
		struct sl *sl;
		struct lk *lk;
		struct cr *cr;
	};
};

/*
 *  -------------------------------------------------------------------------
 *
 *  MTP timers
 *
 *  -------------------------------------------------------------------------
 */
#if 0
static void
mtp_timer_stop(struct mtp *mtp, const uint timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
static void
mtp_timer_start(queue_t *q, struct mtp *mtp, const uint timer)
{
	switch (timer) {
	default:
		swerr();
		break;
	}
}
static uint
mtp_timer_remain(struct mtp *mtp, const uint timer)
{
	switch (timer) {
	default:
		break;
	}
	swerr();
	return (0);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  RS timers
 *
 *  -------------------------------------------------------------------------
 */
/* t8 - transfer prohibited inhibitition timer */
/* t11 - waiting to begin transfer restricted */
/* t15 - waiting to start route set congestion test */
/* t16 - waiting for route set congestion status update */
/* t18a - wiating for final TFR response */

static void
rs_timer_stop(struct rs *rs, const uint timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
	case t8:
		mi_timer_stop(rs->timers.t8);
		if (single)
			break;
		/* fall through */
	case t11:
		mi_timer_stop(rs->timers.t11);
		if (single)
			break;
		/* fall through */
	case t15:
		mi_timer_stop(rs->timers.t15);
		if (single)
			break;
		/* fall through */
	case t16:
		mi_timer_stop(rs->timers.t16);
		if (single)
			break;
		/* fall through */
	case t18a:
		mi_timer_stop(rs->timers.t18a);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
static void
rs_timer_start(queue_t *q, struct rs *rs, const uint timer)
{
	switch (timer) {
	case t8:
		mi_timer(q, rs->timers.t8, rs->config.t8);
		break;
	case t11:
		mi_timer(q, rs->timers.t11, rs->config.t11);
		break;
	case t15:
		mi_timer(q, rs->timers.t15, rs->config.t15);
		break;
	case t16:
		mi_timer(q, rs->timers.t16, rs->config.t16);
		break;
	case t18a:
		mi_timer(q, rs->timers.t18a, rs->config.t18a);
		break;
	default:
		swerr();
		break;
	}
}
static uint
rs_timer_remain(struct rs *rs, const uint timer)
{
	switch (timer) {
	case t8:
		return mi_timer_remain(rs->timers.t8);
	case t11:
		return mi_timer_remain(rs->timers.t11);
	case t15:
		return mi_timer_remain(rs->timers.t15);
	case t16:
		return mi_timer_remain(rs->timers.t16);
	case t18a:
		return mi_timer_remain(rs->timers.t18a);
	}
	swerr();
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CR timers
 *
 *  -------------------------------------------------------------------------
 */
static void
cr_timer_stop(struct cr *cr, const uint timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
	case t6:
		mi_timer_stop(cr->timers.t6);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
static void
cr_timer_start(queue_t *q, struct cr *cr, const uint timer)
{
	switch (timer) {
	case t6:
		mi_timer(q, cr->timers.t6, cr->rt.onto->config.t6);
		break;
	default:
		swerr();
		break;
	}
}
#if 0
static uint
cr_timer_remain(struct cr *cr, const uint timer)
{
	switch (timer) {
	case t6:
		return mi_timer_remain(cr->timers.t6);
	}
	swerr();
	return (0);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  RT timers
 *
 *  -------------------------------------------------------------------------
 */
/* t10 - waiting to repeat RST message */

static void
rt_timer_stop(struct rt *rt, const uint timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
	case t10:
		mi_timer_stop(rt->timers.t10);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
static void
rt_timer_start(queue_t *q, struct rt *rt, const uint timer)
{
	switch (timer) {
	case t10:
		mi_timer(q, rt->timers.t10, rt->config.t10);
		break;
	default:
		swerr();
		break;
	}
}
static uint
rt_timer_remain(struct rt *rt, const uint timer)
{
	switch (timer) {
	case t10:
		return mi_timer_remain(rt->timers.t10);
	}
	swerr();
	return (0);
}

/*
 * -------------------------------------------------------------------------
 *
 * SP timers
 *
 * -------------------------------------------------------------------------
 * All signalling point object timers are related to the ITU-T or ANSI MTP
 * Restart procedures.  T1R is a version of T1 that times whether MTP Restart
 * is required for short duration outages in ITU-T applications.  T18 is the
 * MTP Restart Phase 1 timer, and T20 is the overall MTP Restart timer for
 * ITU-T applications.  For ANSI applications, T22 is the first part of Phase
 * 1 Restart timer, T23 is the second start of Phase 1 Restart timer, T24 is
 * the Phase 2 timer.  T26 times the rebroadcast of TRW messages during
 * Restart.  When a signalling point decides to restart, it extends its
 * inaccessibility to the network to T27.  See the corresponding _timeout()
 * function and the _stop() and _start() calls for more detail on each timer.
 */
static void
sp_timer_stop(struct sp *sp, const uint timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
	case t1r:
		mi_timer_stop(sp->timers.t1r);
		if (single)
			break;
		/* fall through */
	case t18:
		mi_timer_stop(sp->timers.t18);
		if (single)
			break;
		/* fall through */
	case t20:
		mi_timer_stop(sp->timers.t20);
		if (single)
			break;
		/* fall through */
	case t22a:
		mi_timer_stop(sp->timers.t22a);
		if (single)
			break;
		/* fall through */
	case t23a:
		mi_timer_stop(sp->timers.t23a);
		if (single)
			break;
		/* fall through */
	case t24a:
		mi_timer_stop(sp->timers.t24a);
		if (single)
			break;
		/* fall through */
	case t26a:
		mi_timer_stop(sp->timers.t26a);
		if (single)
			break;
		/* fall through */
	case t27a:
		mi_timer_stop(sp->timers.t27a);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
static void
sp_timer_start(queue_t *q, struct sp *sp, const uint timer)
{
	switch (timer) {
	case t1r:
		mi_timer(q, sp->timers.t1r, sp->config.t1r);
		break;
	case t18:
		mi_timer(q, sp->timers.t18, sp->config.t18);
		break;
	case t20:
		mi_timer(q, sp->timers.t20, sp->config.t20);
		break;
	case t22a:
		mi_timer(q, sp->timers.t22a, sp->config.t22a);
		break;
	case t23a:
		mi_timer(q, sp->timers.t23a, sp->config.t23a);
		break;
	case t24a:
		mi_timer(q, sp->timers.t24a, sp->config.t24a);
		break;
	case t26a:
		mi_timer(q, sp->timers.t26a, sp->config.t26a);
		break;
	case t27a:
		mi_timer(q, sp->timers.t27a, sp->config.t27a);
		break;
	default:
		swerr();
		break;
	}
}
static uint
sp_timer_remain(struct sp *sp, const uint timer)
{
	switch (timer) {
	case t1r:
		return mi_timer_remain(sp->timers.t1r);
	case t18:
		return mi_timer_remain(sp->timers.t18);
	case t20:
		return mi_timer_remain(sp->timers.t20);
	case t22a:
		return mi_timer_remain(sp->timers.t22a);
	case t23a:
		return mi_timer_remain(sp->timers.t23a);
	case t24a:
		return mi_timer_remain(sp->timers.t24a);
	case t26a:
		return mi_timer_remain(sp->timers.t26a);
	case t27a:
		return mi_timer_remain(sp->timers.t27a);
	}
	swerr();
	return (0);
}

/*
 * -------------------------------------------------------------------------
 *
 * CB timers
 *
 * -------------------------------------------------------------------------
 * All changeover/changeback buffer object timers are related to the
 * changeover or changeback procedure for both ITU-T and ANSI.  T1 is the
 * amount of time that buffer contents will be held awaiting unblocking of a
 * link.  T2 is the time after which buffer contents will be restarted on the
 * alternate link when time-controlled changeover is performed.  T3 is the
 * timer after which buffer contents will be restarted on the alternate link
 * when time-controlled diversion is performed.  T4 is the time waiting for a
 * changeback-acknowledgement, T5 the time after which traffic will be
 * restarted on the alternate link when changeback-acknowledgement is not
 * forthcoming.  See the _timeout() function as well as _start() and _stop()
 * calls for more detail.
 */
static void
cb_timer_stop(struct cb *cb, const uint timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
	case t1:
		mi_timer_stop(cb->timers.t1);
		if (single)
			break;
		/* fall through */
	case t2:
		mi_timer_stop(cb->timers.t2);
		if (single)
			break;
		/* fall through */
	case t3:
		mi_timer_stop(cb->timers.t3);
		if (single)
			break;
		/* fall through */
	case t4:
		mi_timer_stop(cb->timers.t4);
		if (single)
			break;
		/* fall through */
	case t5:
		mi_timer_stop(cb->timers.t5);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
static void
cb_timer_start(queue_t *q, struct cb *cb, const uint timer)
{
	switch (timer) {
	case t1:
		mi_timer(q, cb->timers.t1, cb->sl.from->config.t1);
		break;
	case t2:
		mi_timer(q, cb->timers.t2, cb->sl.from->config.t2);
		break;
	case t3:
		mi_timer(q, qb->timers.t3, cb->sl.from->config.t3);
		break;
	case t4:
		mi_timer(q, cb->timers.t4, cb->sl.from->config.t4);
		break;
	case t5:
		mi_timer(q, cb->timers.t5, cb->sl.from->config.t5);
		break;
	default:
		swerr();
		break;
	}
}
static uint
cb_timer_remain(struct cb *cb, const uint timer)
{
	switch (timer) {
	case t1:
		return mi_timer_remain(cb->timers.t1);
	case t2:
		return mi_timer_remain(cb->timers.t2);
	case t3:
		return mi_timer_remain(qb->timers.t3);
	case t4:
		return mi_timer_remain(cb->timers.t4);
	case t5:
		return mi_timer_remain(cb->timers.t5);
	}
	swerr();
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LS timers
 *
 *  -------------------------------------------------------------------------
 */

#if 0
static void
ls_timer_stop(struct ls *ls, const uint timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
static void
ls_timer_start(queue_t *q, struct ls *ls, const uint timer)
{
	switch (timer) {
	default:
		swerr();
		break;
	}
}
static inline uint
ls_timer_remain(struct ls *ls, const uint timer)
{
	switch (timer) {
	default:
		break;
	}
	swerr();
	return (0);
}
#endif

/*
 * -------------------------------------------------------------------------
 *
 * LK timers
 *
 * -------------------------------------------------------------------------
 * Most link set object related timers are for MTP Restart.  All link set
 * object timers are related to the adjacent signalling point.  T7 is the
 * signalling data link control acknowledgement timer.  ITU-T T19 and T21 are
 * adjacent restart expected and unexpected timers.  ANSI T25, T28, T29 and
 * T30 are expected and unexpeted timers for adjacent restart for ANSI
 * applications.  The MTP Restart timers get set against the signalling link
 * lower read queue upon which the corresponding message arrived or preceding
 * timeout occurred.
 */
static void
lk_timer_stop(struct lk *lk, const uint timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
	case t7:
		mi_timer_stop(lk->timers.t7);
		if (single)
			break;
		/* fall through */
		break;
	case t19:
		mi_timer_stop(lk->timers.t19);
		if (single)
			break;
		/* fall through */
	case t21:
		mi_timer_stop(lk->timers.t21);
		if (single)
			break;
		/* fall through */
	case t25a:
		mi_timer_stop(lk->timers.t25a);
		if (single)
			break;
		/* fall through */
		break;
	case t28a:
		mi_timer_stop(lk->timers.t28a);
		if (single)
			break;
		/* fall through */
	case t29a:
		mi_timer_stop(lk->timers.t29a);
		if (single)
			break;
		/* fall through */
	case t30a:
		mi_timer_stop(lk->timers.t30a);
		if (single)
			break;
		/* fall through */
		break;
	default:
		break;
	}
	swerr();
}
static inline void
lk_timer_start(queue_t *q, struct lk *lk, const uint timer)
{
	switch (timer) {
	case t7:
		mi_timer(q, lk->timers.t7, lk->config.t7);
		break;
	case t19:
		mi_timer(q, lk->timers.t19, lk->config.t19);
		break;
	case t21:
		mi_timer(q, lk->timers.t21, lk->config.t21);
		break;
	case t25a:
		mi_timer(q, lk->timers.t25a, lk->config.t25a);
		break;
	case t28a:
		mi_timer(q, lk->timers.t28a, lk->config.t28a);
		breakq,;
	case t29a:
		mi_timer(q, lk->timers.t29a, lk->config.t29a);
		break;
	case t30a:
		mi_timer(q, lk->timers.t30a, lk->config.t30a);
		break;
	default:
		break;
	}
	swerr();
}

static inline uint
lk_timer_remain(struct lk *lk, const uint timer)
{
	switch (timer) {
	case t7:
		return mi_timer_remain(lk->timers.t7);
	case t19:
		return mi_timer_remain(lk->timers.t19);
	case t21:
		return mi_timer_remain(lk->timers.t21);
	case t25a:
		return mi_timer_remain(lk->timers.t25a);
	case t28a:
		return mi_timer_remain(lk->timers.t28a);
	case t29a:
		return mi_timer_remain(lk->timers.t29a);
	case t30a:
		return mi_timer_remain(lk->timers.t30a);
	default:
		break;
	}
	swerr();
	return (0);
}

/*
 * -------------------------------------------------------------------------
 *
 * SL timers
 *
 * -------------------------------------------------------------------------
 *
 * Timers T12, T13 and T14 are uninhibit, force uninhibit and inihibit
 * acknowledgement timers.
 *
 * ANSI T19 is the maximum amount of time that a
 * signalling link will be allowed to be in the failed state without informing
 * management.  ANSI T20 and T21 are local and remote inhibit test timers:
 * under ITU-T these timers are named T22 and T23.
 */
static void
sl_timer_stop(struct sl *sl, const uint t)
{
	int single = 1;

	switch (t) {
	case tall:
		single = 0;
		/* fall through */
	case t12:
		mi_timer_stop(sl->timers.t12);
		if (single)
			break;
		/* fall through */
	case t13:
		mi_timer_stop(sl->timers.t13);
		if (single)
			break;
		/* fall through */
	case t14:
		mi_timer_stop(sl->timers.t14);
		if (single)
			break;
		/* fall through */
	case t17:
		mi_timer_stop(sl->timers.t17);
		if (single)
			break;
		/* fall through */
	case t19a:
		mi_timer_stop(sl->timers.t19a);
		if (single)
			break;
		/* fall through */
	case t20a:
		mi_timer_stop(sl->timers.t20a);
		if (single)
			break;
		/* fall through */
	case t21a:
		mi_timer_stop(sl->timers.t21a);
		if (single)
			break;
		/* fall through */
	case t22:
		mi_timer_stop(sl->timers.t22);
		if (single)
			break;
		/* fall through */
	case t23:
		mi_timer_stop(sl->timers.t23);
		if (single)
			break;
		/* fall through */
	case t24:
		mi_timer_stop(sl->timers.t24);
		if (single)
			break;
		/* fall through */
	case t31a:
		mi_timer_stop(sl->timers.t31a);
		if (single)
			break;
		/* fall through */
	case t32a:
		mi_timer_stop(sl->timers.t32a);
		if (single)
			break;
		/* fall through */
	case t33a:
		mi_timer_stop(sl->timers.t33a);
		if (single)
			break;
		/* fall through */
	case t34a:
		mi_timer_stop(sl->timers.t34a);
		if (single)
			break;
		/* fall through */
	case t1t:
		mi_timer_stop(sl->timers.t1t);
		if (single)
			break;
		/* fall through */
	case t2t:
		mi_timer_stop(sl->timers.t2t);
		if (single)
			break;
		/* fall through */
	case t1s:
		mi_timer_stop(sl->timers.t1s);
		if (single)
			break;
		/* fall through */
		break;
	default:
		swerr();
		break;
	}
}
static void
sl_timer_restart(queue_t *q, struct sl *sl, const uint timer)
{
	switch (timer) {
	case t12:
		mi_timer(q, sl->timers.t12, sl->config.t12);
		break;
	case t13:
		mi_timer(q, sl->timers.t13, sl->config.t13);
		break;
	case t14:
		mi_timer(q, sl->timers.t14, sl->config.t14);
		break;
	case t17:
		mi_timer(q, sl->timers.t17, sl->config.t17);
		break;
	case t19a:
		mi_timer(q, sl->timers.t19a, sl->config.t19a);
		break;
	case t20a:
		mi_timer(q, sl->timers.t20a, sl->config.t20a);
		break;
	case t21a:
		mi_timer(q, sl->timers.t21a, sl->config.t21a);
		break;
	case t22:
		mi_timer(q, sl->timers.t22, sl->config.t22);
		break;
	case t23:
		mi_timer(q, sl->timers.t23, sl->config.t23);
		break;
	case t24:
		mi_timer(q, sl->timers.t24, sl->config.t24);
		break;
	case t31a:
		mi_timer(q, sl->timers.t31a, sl->config.t31a);
		break;
	case t32a:
		mi_timer(q, sl->timers.t32a, sl->config.t32a);
		break;
	case t33a:
		mi_timer(q, sl->timers.t33a, sl->config.t33a);
		break;
	case t34a:
		mi_timer(q, sl->timers.t34a, sl->config.t34a);
		break;
	case t1t:
		mi_timer(q, sl->timers.t1t, sl->config.t1t);
		break;
	case t2t:
		mi_timer(q, sl->timers.t2t, sl->config.t2t);
		break;
	case t1s:
		mi_timer(q, sl->timers.t1s, sl->config.t1s);
		break;
	default:
		swerr();
		break;
	}
}
static void
sl_timer_start(queue_t *q, struct sl *sl, const uint timer)
{
	switch (timer) {
	case t12:
		((struct mtp_timer *)sl->timers.t12->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t12, sl->config.t12);
		break;
	case t13:
		((struct mtp_timer *)sl->timers.t13->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t13, sl->config.t13);
		break;
	case t14:
		((struct mtp_timer *)sl->timers.t14->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t14, sl->config.t14);
		break;
	case t17:
		((struct mtp_timer *)sl->timers.t17->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t17, sl->config.t17);
		break;
	case t19a:
		((struct mtp_timer *)sl->timers.t19a->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t19a, sl->config.t19a);
		break;
	case t20a:
		((struct mtp_timer *)sl->timers.t20a->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t20a, sl->config.t20a);
		break;
	case t21a:
		((struct mtp_timer *)sl->timers.t21a->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t21a, sl->config.t21a);
		break;
	case t22:
		((struct mtp_timer *)sl->timers.t22->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t22, sl->config.t22);
		break;
	case t23:
		((struct mtp_timer *)sl->timers.t23->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t23, sl->config.t23);
		break;
	case t24:
		((struct mtp_timer *)sl->timers.t24->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t24, sl->config.t24);
		break;
	case t31a:
		((struct mtp_timer *)sl->timers.t31a->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t31a, sl->config.t31a);
		break;
	case t32a:
		((struct mtp_timer *)sl->timers.t32a->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t32a, sl->config.t32a);
		break;
	case t33a:
		((struct mtp_timer *)sl->timers.t33a->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t33a, sl->config.t33a);
		break;
	case t34a:
		((struct mtp_timer *)sl->timers.t34a->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t34a, sl->config.t34a);
		break;
	case t1t:
		((struct mtp_timer *)sl->timers.t1t->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t1t, sl->config.t1t);
		break;
	case t2t:
		((struct mtp_timer *)sl->timers.t2t->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t2t, sl->config.t2t);
		break;
	case t1s:
		((struct mtp_timer *)sl->timers.t1s->b_rptr)->count = 0;
		mi_timer(q, sl->timers.t1s, sl->config.t1s);
		break;
	default:
		swerr();
		break;
	}
}
static uint
sl_timer_remain(struct sl *sl, const uint t)
{
	switch (t) {
	case t12:
		return mi_timer_remain(sl->timers.t12);
	case t13:
		return mi_timer_remain(sl->timers.t13);
	case t14:
		return mi_timer_remain(sl->timers.t14);
	case t17:
		return mi_timer_remain(sl->timers.t17);
	case t19a:
		return mi_timer_remain(sl->timers.t19a);
	case t20a:
		return mi_timer_remain(sl->timers.t20a);
	case t21a:
		return mi_timer_remain(sl->timers.t21a);
	case t22:
		return mi_timer_remain(sl->timers.t22);
	case t23:
		return mi_timer_remain(sl->timers.t23);
	case t24:
		return mi_timer_remain(sl->timers.t24);
	case t31a:
		return mi_timer_remain(sl->timers.t31a);
	case t32a:
		return mi_timer_remain(sl->timers.t32a);
	case t33a:
		return mi_timer_remain(sl->timers.t33a);
	case t34a:
		return mi_timer_remain(sl->timers.t34a);
	case t1t:
		return mi_timer_remain(sl->timers.t1t);
	case t2t:
		return mi_timer_remain(sl->timers.t2t);
	case t1s:
		return mi_timer_remain(sl->timers.t1s);
	default:
		break;
	}
	swerr();
	return (0);
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
static int
mtp_status_ind(queue_t *q, struct mtp *mtp, struct mtp_addr *dst, int status)
{
	switch (mtp->i.type) {
	case MTP_STYLE_MTPI:
	case MTP_STYLE_MGMT:
		switch (status) {
		default:
			return m_status_ind(q, mtp, dst, NULL, NULL, status);
		case T_MTP_PROHIBITED:
			return m_pause_ind(q, mtp, dst, NULL);
		case T_MTP_AVAILABLE:
			return m_resume_ind(q, mtp, dst);
		}
		break;
	case MTP_STYLE_TPI:
		if (mtp->i.style == T_CLTS) {
			/* T_CLTS */
			return t_uderror_ind(q, mtp, dst, NULL, NULL, status);
		} else {
			/* T_COTS */
			fixme(("We should send an optdata indication with null data instead\n"));
			return t_uderror_ind(q, mtp, dst, NULL, NULL, status);
		}
		break;
	case MTP_STYLE_NPI:
		if (mtp->i.style == T_CLTS) {
			/* N_CLNS */
			return n_uderror_ind(q, mtp, dst, NULL, NULL, status);
		} else {
			/* N_CONS */
			todo(("We should actually set the originator properly\n"));
			return n_reset_ind(q, mtp, N_UNDEFINED, status);
		}
		break;
	}
	swerr();
	return (-EFAULT);
}
static int
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
static inline int
mtp_up_status_ind_all_local(queue_t *q, struct rs *rs, uint si, uint status)
{
	struct mtp *mtp;
	int err;

	for (mtp = rs->sp.sp->mtp.lists[si & 0xf]; mtp; mtp = mtp->sp.next)
		if ((err = mtp_up_status_ind(q, mtp, rs, si, status)))
			return (err);
	return (0);
}
static int
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
static int
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
static int
mtp_restr_status_ind(queue_t *q, struct mtp *mtp, struct rs *rs)
{
	struct mtp_addr dest = mtp->src;
	uint error = T_MTP_RESTRICTED;

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
static int
mtp_restr_status_ind_all_local(queue_t *q, struct rs *rs)
{
	int i, err;
	struct mtp *mtp;

	for (i = 0; i < 16; i++)
		for (mtp = rs->sp.sp->mtp.lists[i]; mtp; mtp = mtp->sp.next)
			if ((err = mtp_restr_status_ind(q, mtp, rs)))
				return (err);
	return (0);
}

/*
 *  MTP-PAUSE-IND
 *  -----------------------------------
 */
static int
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
static int
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
static int
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
static int
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
static int
mtp_transfer_ind(queue_t *q, struct mtp *mtp, struct mtp_msg *m)
{
	mblk_t *dp;

	if ((dp = ss7_dupb(q, m->bp))) {
		struct mtp_addr src = { ni:m->ni, pc:m->opc, si:m->si };
		struct mtp_addr dst = { ni:m->ni, pc:m->dpc, si:m->si };

		dp->b_rptr = m->data;
		dp->b_wptr = dp->b_rptr + m->dlen;
		switch (mtp->i.type) {
		default:
		case MTP_STYLE_MTPI:
		case MTP_STYLE_MGMT:
			return m_transfer_ind(q, mtp, &src, m->sls, m->mp, dp);
		case MTP_STYLE_TPI:
			if (mtp->i.style == T_CLTS) {
				/* T_CLTS */
				return t_unitdata_ind(q, mtp, &src, NULL, dp);
			} else {
				/* T_COTS */
				return t_data_ind(q, mtp, 0, dp);
			}
			break;
		case MTP_STYLE_NPI:
			if (mtp->i.style == T_CLTS) {
				/* N_CLNS */
				return n_unitdata_ind(q, mtp, &src, &dst, dp);
			} else {
				/* N_CONS */
				return n_data_ind(q, mtp, 0, dp);
			}
			break;
		}
		freeb(dp);
		return (0);
	}
	return (-ENOBUFS);
}
static int
mtp_transfer_ind_local(queue_t *q, struct sp *sp, struct mtp_msg *m)
{
	struct mtp *mtp;

	for (mtp = sp->mtp.lists[m->si & 0xf]; mtp; mtp = mtp->sp.next) {
		if (mtp->i.style == T_CLTS) {
			/* T_CLTS */
			if (mtp->state != TS_IDLE || mtp->src.pc != m->dpc)
				continue;
		} else {
			/* T_COTS */
			if (mtp->state != TS_DATA_XFER || mtp->src.pc != m->dpc
			    || mtp->dst.pc != m->opc)
				continue;
		}
		break;
	}
	if (mtp)
		return mtp_transfer_ind(q, mtp, m);
#if 0
	if (!(sp->mtp.known & (1 << m->si))) {
		fixme(("Reply with UPU unknown\n"));
		return (0);
	}
#endif
	if (!(sp->mtp.equipped & (1 << m->si))) {
		fixme(("Reply with UPU unequipped\n"));
		return (0);
	}
	if (!(sp->mtp.available & (1 << m->si))) {
		fixme(("Reply with UPU inaccessible\n"));
		return (0);
	}
	swerr();
	return (0);
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
static uchar
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
/**
 * mtp_send_link: - send a message on a specific link
 * @q: active queue
 * @dp: the message
 * @sl: the signalling link on which to send the message
 *
 * For sending a message on a specific link.  This is used for CBD which must
 * be sent on the alternate link.
 */
static int
mtp_send_link(queue_t *q, mblk_t *dp, struct sl *sl)
{
	(void) q;
	if (canputnext(sl->wq)) {
		if (sl->lk.lk->ls.ls->lk.numb > 1) {
			/* only rotate on combined linksets (i.e. not C-links) */
			struct sp *sp = sl->lk.lk->sp.loc;

			switch (sp->na.na->option.pvar) {
			case SS7_PVAR_ANSI:
			case SS7_PVAR_JTTC:
			case SS7_PVAR_CHIN:	/* ??? */
			{
				uint sls = dp->b_rptr[7] & sp->ls.sls_mask;

				/* Actually, for 5-bit and 8-bit compatability, we
				   rotate based on 5 sls bits regardless of the
				   number of bits in the SLS */
				if (sls & 0x01)
					sls =
					    (sls & 0xe0) | ((sls & 0x1f) >> 1) |
					    0x10;
				else
					sls =
					    (sls & 0xe0) | ((sls & 0x1f) >> 1) |
					    0x00;
				/* rewrite sls in routing label */
				dp->b_rptr[7] = sls;
				break;
			}
			}
		}
		putnext(sl->wq, dp);	/* send message */
		return (0);
	}
	rare();
	freemsg(dp);
	return (-EBUSY);
}

/**
 * mtp_send_route: - send a message with full routing
 * @q: active queue
 * @sp: signalling point from which to send
 * @mp: the message
 * @priority: message priority
 * @dpc: destination to which to send
 * @sls: signalling link selection
 *
 *
 * Full routing for management messages sent from a signalling point.  This is
 * for management only message because it does not report congestion back to
 * the user.
 */
static int
mtp_send_route(queue_t *q, struct sp *sp, mblk_t *mp, uint priority, uint32_t dpc,
	       uint sls)
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
		if (rs->rs_type == RT_TYPE_CLUSTER
		    && rs->dest == (dpc & na->mask.cluster))
			goto rs_found;
	goto efault;
      rs_found:
	if (rs->state > RS_RESTRICTED)
		goto discard;
	if (!(rl = rs->rl.curr))
		goto efault;
	if (!(ls = rl->ls.ls))
		goto efault;
	/* select active route based on sls */
	if (!(rt = rl->smap[sls_index(sls, ls->rl.sls_mask, ls->rl.sls_bits)].rt))
		goto efault;
	if (rt->type == MTP_OBJ_TYPE_CR) {
		/* we are performing controlled rerouting, buffer message */
		bufq_queue(&((struct cr *) rt)->buf, mp);
		return (0);
	}
	/* Selection signalling link in link (set): for ITU-T we need to use the
	   bits that were not used in route selection; for ANSI we use the least
	   significant 4 bits (but could be 7 for 8-bit SLSs) after route selection
	   and sls rotation.  Again, the following algorithm does the job for both
	   ITU-T and ANSI and will accomodate quad linksets to quad STPs. */
	/* select active signalling link based on sls */
	if (!
	    (sl =
	     rt->lk.lk->smap[sls_index(sls, ls->lk.sls_mask, ls->lk.sls_bits)].sl))
		goto efault;
	/* Check for changeback on the signalling link index: normally the pointer
	   is to a signalling link object not a changeback buffer object. This
	   approach is faster than the previous approach. */
	if (sl->type == MTP_OBJ_TYPE_CB) {
		/* we are performing changeback or changeover, buffer message */
		bufq_queue(&((struct cb *) sl)->buf, mp);
		return (0);
	}
	if (!(na->option.popt & SS7_POPT_MPLEV))
		priority = 0;
	if (sl->cong_status > priority) {
		/* As the message originated from us, we should treat this as though 
		   we received a TFC for the RCT message.  This means that the
		   congestion priority on the route set is increased to the
		   congestion of the link set. */
		rs_timer_stop(rs, t15);
		rs_timer_stop(rs, t16);
		/* signal congestion (just for RCT) */
		if ((err = mtp_cong_status_ind_all_local(q, rs, sl->cong_status)))
			return (err);
		rs->cong_status = sl->cong_status;
		rs_timer_start(q, rs, t15);
	}
	if (sl->disc_status > priority)
		goto discard;
	if (!canputnext(sl->wq))
		goto ebusy;
	/* Note: we do not do SLS bit rotation for messages sent from this
	   signalling point, even if it is an STP.  We only perform SLS bit rotation 
	   on messages transferred across a Signalling Transfer Point (see
	   mtp_xfer_route()). However, because we must unrotate all ANSI messages
	   retrieved from the signalling link on changeover, we still need to
	   rotate. */
	if (sl->lk.lk->ls.ls->lk.numb > 1) {
		/* Only rotate when part of the sls was actually used for route
		   selection (i.e. not on C-Links or F-Links, but only on combined
		   link sets (A-, B-, D-, E- Links) */
		switch (na->option.pvar) {
		case SS7_PVAR_ANSI:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_CHIN:	/* ??? */
			/* XXX Actually, for 5-bit and 8-bit compatability, we
			   rotate based on 5 sls bits regardless of the number of
			   bits in the SLS */
			if (sls & 0x01)
				sls = (sls & 0xe0) | ((sls & 0x1f) >> 1) | 0x10;
			else
				sls = (sls & 0xe0) | ((sls & 0x1f) >> 1) | 0x00;
			/* rewrite sls in routing label */
			mp->b_rptr[7] = sls;
			break;
		}
	}
	sl->flags |= SLF_TRAFFIC;
	putnext(sl->wq, mp);	/* send message */
	return (0);
      discard:
	/* This is for management messages only, so we do not report congestion or
	   prohibited routes back to the user. Oft times, we may blindly send
	   management messages not considering the state of the route set with the
	   anticipation that they will be discarded if the route set is not
	   accessible. */
	freemsg(mp);
	return (0);		/* discard message */
      ebusy:
	return (-EBUSY);
      efault:
	mi_strlog(q, 0, SL_ERROR, "discard: no outgoing route for message");
	goto discard;
}

/**
 * mtp_send_route_loadshare: - send with route loadsharing
 * @q: active queue
 * @sp: signalling point from which to send
 * @bp: the message
 * @mp: message priority
 * @dpc: destination
 * @sls: signalling link selection
 *
 * Full routing for a message we created with load sharing.
 */
static int
mtp_send_route_loadshare(queue_t *q, struct sp *sp, mblk_t *bp, uint mp,
			 uint32_t dpc, uint sls)
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
static int mtp_send_link(queue_t *q, mblk_t *dp, struct sl *sl);

static int
mtp_send_coo(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc, uint fsnl)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 1);
		mtp_enc_com(bp, sp, slc, fsnl);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_coa(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc, uint fsnl)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 2);
		mtp_enc_com(bp, sp, slc, fsnl);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}

/*
 * 6.3.1   ... a changeback declaration is sent to the remote signalling point
 * of the signalling link make available via the concerned alternative
 * signalling link; this message indicates that no more message signal units
 * relating to the traffic being diverted to the link made available will be
 * sent on the alternative signalling link.
 */
static int
mtp_send_cbd(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc, uint cbc, struct sl *sl)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 5);
		mtp_enc_cbm(bp, sp, slc, cbc);
		return mtp_send_link(q, bp, sl);
	}
	return (-ENOBUFS);
}

/*
 * 6.3.2   ... any available signalling route between the two signalling
 * points can be used to carry the changeback acknowledgement.
 */
static int
mtp_send_cba(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc, uint cbc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 6);
		mtp_enc_cbm(bp, sp, slc, cbc);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_eco(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 2, 1);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_eca(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 2, 2);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_rct(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint mp)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, mp, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 3, 1);
		return mtp_send_route(q, sp, bp, mp, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_tfc(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest, uint stat)
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
 * 13.2.1 ... Transfer-prohibited messages are always addressed to an adjacent
 * signalling point.  They may use any available signalling route that leads
 * to that signalling point.
 */
static int
mtp_send_tfp(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 4, 1);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_tcp(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
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
 * 13.4.1 ... Transfer-restricted messages are always address to an adjacent
 * signalling point.  They may use any available signalling route that leads
 * to that signalling point.
 */
static int
mtp_send_tfr(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 4, 3);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_tcr(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
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
 * 13.3.1 ... Transfer-allowed message are always address to an adjacent
 * signalling point.  They may use any available signalling route that leads
 * to that signalling point.
 */
static int
mtp_send_tfa(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 4, 5);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_tca(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
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
 * 13.5.3  A signalling-route-set-test message is sent to the adjacent
 * signalling transfer point as an ordinary signalling network management
 * message.
 */
static int
mtp_send_rst(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 5, 1);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_rsr(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 5, 2);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_rcp(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 5, 3);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_rcr(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 5, 4);
		mtp_enc_tfm(bp, sp, dest);
		return mtp_send_route_loadshare(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_lin(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 1);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_lun(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 2);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_lia(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 3);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_lua(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 4);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_lid(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 5);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_lfu(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 6);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_llt(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 7);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_lrt(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 6, 8);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_tra(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 7, 1);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_trw(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 7, 2);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_dlc(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc, uint sdli)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 8, 1);
		mtp_enc_dlc(bp, sp, slc, sdli);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_css(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 8, 2);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_cns(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 8, 3);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_cnp(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint slc)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 8, 4);
		mtp_enc_slm(bp, sp, slc);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_upu(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest, uint upi)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 10, 1);
		mtp_enc_upm(bp, sp, dest, upi);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static inline int
mtp_send_upa(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest, uint upi)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 10, 2);
		mtp_enc_upm(bp, sp, dest, upi);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_upt(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	     uint sls, uint32_t dest, uint upi)
{
	mblk_t *bp;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 0, dpc, opc, sls))) {
		mtp_enc_sif(bp, sp, 10, 3);
		mtp_enc_upm(bp, sp, dest, upi);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_sltm(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	      uint sls, uint slc, unsigned char *data, size_t dlen, struct sl *sl)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 1, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 1);
		mtp_enc_sltm(bp, sp, slc, dlen, data);
		return mtp_send_link(q, bp, sl);
	}
	return (-ENOBUFS);
}
static int
mtp_send_slta(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	      uint sls, uint slc, unsigned char *data, size_t dlen)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 1, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 2);
		mtp_enc_sltm(bp, sp, slc, dlen, data);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_ssltm(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	       uint sls, uint slc, unsigned char *data, size_t dlen, struct sl *sl)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 2, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 1);
		mtp_enc_sltm(bp, sp, slc, dlen, data);
		return mtp_send_link(q, bp, sl);
	}
	return (-ENOBUFS);
}
static int
mtp_send_sslta(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	       uint sls, uint slc, unsigned char *data, size_t dlen)
{
	mblk_t *bp;
	uint rl_sls =
	    ((sp->na.na->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) ? sls : slc;

	if ((bp = mtp_enc_msg(q, sp, ni, 3, 2, dpc, opc, rl_sls))) {
		mtp_enc_sif(bp, sp, 1, 2);
		mtp_enc_sltm(bp, sp, slc, dlen, data);
		return mtp_send_route(q, sp, bp, 3, dpc, sls);
	}
	return (-ENOBUFS);
}
static int
mtp_send_user(queue_t *q, struct sp *sp, uint ni, uint32_t dpc, uint32_t opc,
	      uint sls, uint mp, uint si, mblk_t *dp)
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

/**
 * mtp_tfp_broadcast: - TFP/TCP broadcast
 * @q: active queue
 * @rs: route set for which to broadcast
 *
 * Perform local and adjacent (if STP) broadcast of route set prohibited,
 * except on a primary link set which receives nothing, and except for an old
 * non-primary link set which has already received TFP.
 *
 * 13.2.1 ... Transfer-prohibited messages are always addressed to an adjacent
 * signalling point.  They may use any available signalling route that leads
 * to that signalling point.
 *
 * 13.2.2  A transfer prohibited message relating to a given destination X is
 * sent from a signalling transfer point Y in the following cases:
 *
 * i)   When signalling transfer point Y starts to route (at changeover,
 *      changeback, forced or controlled rerouting) signalling destined to
 *      signalling point X via a signalling transfer point Z not currently
 *      used by signalling transfer point Y for this traffic.  In this case,
 *      the transfer prohibited message is sent to signalling transfer point
 *      Z.
 *
 * ii)  When signalling transfer point Y recognizes the inaccessibility of
 *      signalling point X (see 5.3.3 and 7.2.3).  In this case a
 *      transfer-prohibited message is sent to all accessible adjacent
 *      signalling points (Broadcast method) and timer T8 (see clause 16) is
 *      started concerning SP X.
 *
 * iii) When a message destined to signalling point X is received at
 *      signalling transfer point Y and Y is unable to transfer the message,
 *      and if no corresponding timer T8 is running.  In this case the
 *      transfer-prohibited message is sent to the adjacent signalling point
 *      from which the message concerned was received (Response Method).  In
 *      addition, timer T8 is started concerning SP X.
 *
 * iv)  When an adjacent signalling point Z becomes accessible, STP Y sends to
 *      Z a transfer prohibited message concerning destination X, if X is
 *      inaccessible from Y (clause 9).
 *
 * v)   When a signalling transfer point Y restarts, it broadcasts to all
 *      accessible adjacent signalling points transfer prohibited messages
 *      concerning destination X, if X is inaccessible from Y (see clause 9).
 */
static int
mtp_tfp_broadcast(queue_t *q, struct rs *rs)
{
	struct sp *sp = rs->sp.sp;
	struct ls *ls;
	uint sls = 0;
	int err;

	/* inform users */
	if ((err = mtp_pause_ind_all_local(q, rs)))
		return (err);

	/* adjacent SP broadcast is only for STPs */
	/* do not broadcast during restart phase 1 */
	if (!(sp->flags & SPF_XFER_FUNC) || (sp->flags & SPF_RESTART_PHASE_1))
		return (0);

	for (ls = sp->ls.list; ls; ls = ls->sp.next) {
		struct lk *lk;

		for (lk = ls->lk.list; lk; lk = lk->ls.next) {
			struct rs *adj = lk->sp.adj;

			if (!(rs->flags & RSF_CLUSTER)) {
				if (adj->dest == rs->dest)
					continue;
				mtp_send_tfp(q, sp, lk->ni, adj->dest, sp->pc, sls,
					     rs->dest);
			} else
				mtp_send_tcp(q, sp, lk->ni, adj->dest, sp->pc, sls,
					     rs->dest);
			sls = (sls + 1) & ((1 << sp->rs.sls_bits) - 1);
		}
	}
	return (0);
}

/**
 * mtp_tfp_adjacent: - send TFP/TCP to adjacent restarting signalling point
 * @q: active queue (likely lower read queue)
 * @adj: adjacent route set
 *
 * Q.704/1996 9.3.3 When signalling point Y becomes accessible by means other
 * than via a direct link set between X and Y, X sends an MTP-RESUME primitive
 * concerning Y to all local MTP users.  In addition, if signalling point X
 * has the transfer function, X sends to Y any required transfer-prohibited
 * and transfer-restricted messages on the available route.  X then broadcasts
 * TFA and/or TFR messages (see clause 13) concerning Y.  Note that X should
 * not in this case alter any routing data other than that for Y.
 *
 * IMPLEMENTATION NOTES:- If the currently available route is via the direct
 * link set to the adjacent signalling point, then this procedure has already
 * been performed when the signalling link in the direct link set became
 * active.  This procedure is only for when the adjacent becomes available by
 * an alternate route.
 */
static void
mtp_tfp_adjacent(queue_t *q, struct rs *adj)
{
	struct sp *sp = rs->sp.sp;
	uint sls = 0;

	if (!(sp->flags & SPF_XFER_FUNC) || (sp->flags & SPF_RESTART_PHASE_1))
		return;

	dassert(rs->rl.curr && rs->rl.curr->rt.curr);
	/* must be restarting via alternate route */
	if (rs->rl.curr->rt.curr->lk.lk->sp.adj == rs)
		return;

	for (rs = sp->rs.list; rs; rs = rs->sp.next) {
		if (rs == adj)
			continue;
		if (rs->state >= RS_RESTART)
			continue;
		if (!(rs->flags & RSF_CLUSTER))
			mtp_send_tfp(q, sp, sp->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		else
			mtp_send_tcp(q, sp, sp->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		sls = (sls + 1) & ((1 << sp->rs.sls_bits) - 1);
	}
}

/**
 * mtp_tfp_reroute: - perform TFP procedures for STPs during rerouting
 * @q: active queue
 * @rl: route list to prohibit traffic from
 *
 * This performs special TFP procedures for STPs.  When rerouting traffic
 * toward a route list, send TFP/TCP for the rerouted traffic to let the
 * adjacent signalling point know that it cannot route traffic via this STP.
 *
 * Note that we ignore the error on sending TFP and TCP, because if we fail to
 * send it now, the responsive procedure will eventually correct the
 * situation.
 */
static void
mtp_tfp_reroute(queue_t *q, struct rl *rl)
{
	struct rs *rs = rl->rs.rs;
	struct sp *sp = rs->sp.sp;
	struct rt *rt;
	uint sls = 0;

	if (!(sp->flags & SPF_XFER_FUNC) || (sp->flags & SPF_RESTART_PHASE_1))
		return;

	for (rt = rl->rt.list; rt; rt = rt->rl.next) {
		struct lk *lk = rt->lk.lk;
		struct rs *adj = lk->sp.adj;

		if (!(rs->flags & RSF_CLUSTER)) {
			if (adj->dest == rs->dest)
				continue;
			mtp_send_tfp(q, sp, lk->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		} else
			mtp_send_tcp(q, sp, lk->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		sls = (sls + 1) & ((1 << sp->rs.sls_bits) - 1);
	}
}

/**
 * mtp_tfr_broadcast: - TFC/TCR broadcast
 * @q: active queue
 * @rs: route set for which to broadcast
 *
 * Perform adjacent (if STP) broadcast of route set restricted, including an
 * old non-primary link set, but not a new non-primary link set which receives
 * a directed TFP, and not on a primary link set which receives nothing.
 *
 * 13.4.1 ... Transfer-restricted messages are always address to an adjacent
 * signalling point.  They may use any available signalling route that leads
 * to that signalling point.
 *
 * 13.4.2  A transfer-restricted message relating to a given destination X is
 * sent from a signalling transfer point Y when the normal link set (combined
 * link set) used by signalling point Y to route to destination X experiences
 * a long-term failure such as an equipment failure, or there is congestion on
 * an alternate link set currently being used to destination X.  In this case,
 * a transfer-restricted message is sent to all accessible signalling points
 * except those that receive a TFP message according to 13.2.2 i), and except
 * signalling point X if it is an adjacent point (Broadcast Method).
 *
 * When and adjacent signalling point X becomes accessible, the STP Y sends to
 * X transfer-restricted message concerning destinations that are restricted
 * from Y (see clause 9).
 *
 * When a signalling point Y restarts, it broadcasts to all accessible
 * adjacent signalling points transfer restricted messages concerning
 * destinations that are restricted from Y (see clause 9).
 */
static int
mtp_tfr_broadcast(queue_t *q, struct rs *rs)
{
	struct sp *sp = rs->sp.sp;
	struct ls *lc, *lp, *ls;
	uint sls = 0;
	int err;

	/* inform users (for completeness and M3UA) */
	if ((err = mtp_restr_status_ind_all_local(q, rs)))
		return (err);

	/* adjacent SP broadcast is only for STPs */
	/* do not broadcast during restart phase 1 */
	if (!(sp->flags & SPF_XFER_FUNC) || (sp->flags & SPF_RESTART_PHASE_1))
		return (0);

	/* current and primary linksets were addressed during rerouting */
	lc = rs->rl.curr ? rs->rl.curr->ls.ls : NULL;	/* current link set */
	lp = rs->rl.list ? rs->rl.list->ls.ls : NULL;	/* primary link set */

	for (ls = sp->ls.list; ls; ls = ls->sp.next) {
		struct lk *lk;

		for (lk = ls->lk.list; lk; lk = lk->ls.next) {
			struct rs *adj = lk->sp.adj;

			if (ls == lc || ls == lp)
				continue;
			if (!(rs->flags & RSF_CLUSTER)) {
				if (adj->dest == rs->dest)
					continue;
				mtp_send_tfr(q, sp, lk->ni, adj->dest, sp->pc, sls,
					     rs->dest);
			} else
				mtp_send_tcr(q, sp, lk->ni, adj->dest, sp->pc, sls,
					     rs->dest);
			sls = (sls + 1) & ((1 << sp->rs.sls_bits) - 1);
		}
	}
	return (0);
}

/**
 * mtp_tfr_adjacent: - send TFR/TCR to adjacent restarting signalling point
 * @q: active queue (likely lower read queue)
 * @adj: adjacent route set
 *
 * Q.704/1996 9.3.3 When signalling point Y becomes accessible by means other
 * than via a direct link set between X and Y, X sends an MTP-RESUME primitive
 * concerning Y to all local MTP users.  In addition, if signalling point X
 * has the transfer function, X sends to Y any required transfer-prohibited
 * and transfer-restricted messages on the available route.  X then broadcasts
 * TFA and/or TFR messages (see clause 13) concerning Y.  Note that X should
 * not in this case alter any routing data other than that for Y.
 *
 * IMPLEMENTATION NOTES:- If the currently available route is via the direct
 * link set to the adjacent signalling point, then this procedure has already
 * been performed when the signalling link in the direct link set became
 * active.  This procedure is only for when the adjacent becomes available by
 * an alternate route.
 */
static void
mtp_tfr_adjacent(queue_t *q, struct rs *adj)
{
	struct sp *sp = rs->sp.sp;
	uint sls = 0;

	if (!(sp->flags & SPF_XFER_FUNC) || (sp->flags & SPF_RESTART_PHASE_1))
		return;

	dassert(rs->rl.curr && rs->rl.curr->rt.curr);
	/* must be restarting via alternate route */
	if (rs->rl.curr->rt.curr->lk.lk->sp.adj == rs)
		return;

	for (rs = sp->rs.list; rs; rs = rs->sp.next) {
		if (rs == adj)
			continue;
		if (rs->state > RS_RESTRICTED || rs->state < RS_DANGER)
			continue;
		if (!(rs->flags & RSF_CLUSTER))
			mtp_send_tfr(q, sp, sp->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		else
			mtp_send_tcr(q, sp, sp->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		sls = (sls + 1) & ((1 << sp->rs.sls_bits) - 1);
	}
}

/**
 * mtp_tfr_reroute: - perform TFR procedure for STPs during rerouting
 * @q: active queue
 * @rl: route list to restrict traffic from
 *
 * This performs special TFR procedures for STPs.  When rerouting traffic away
 * from a route list, to a higher cost route list, send TFR/TCR for the
 * rerouted traffic to let the adjacent signalling point know that it can now
 * route traffic via this STP, but inefficiently.
 *
 * Note that we ignore the error on sending TFR or TCR, because if we fail to
 * send it now, the route set test procedures of the adjacent signalling point
 * will discover the situation.  Also, if we are short of buffers, we do not
 * mind that the adjacent signalling point does not yet know of our
 * availability, however inefficient, for these routes.
 */
static void
mtp_tfr_reroute(queue_t *q, struct rl *rl)
{
	struct rs *rs = rl->rs.rs;
	struct sp *sp = rs->sp.sp;
	struct rt *rt;
	uint sls = 0;

	if (!(sp->flags & SPF_XFER_FUNC) || (sp->flags & SPF_RESTART_PHASE_1))
		return;

	for (rt = rl->rt.list; rt; rt = rt->rl.next) {
		struct lk *lk = rt->lk.lk;
		struct rs *adj = lk->sp.adj;

		if (!(rs->flags * RSF_CLUSTER)) {
			if (adj->dest == rs->dest)
				continue;
			mtp_send_tfr(q, sp, lk->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		} else
			mtp_send_tcr(q, sp, lk->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		sls = (sls + 1) & ((1 << sp->rs.sls_bits) - 1);
	}
}

/**
 * mtp_tfa_broadcast: - TFA/TCA broadcast
 * @q: active queue
 * @rs: routset for which to broadcast
 *
 * Perform local and adjacent (if STP) broadcast of route set allowed,
 * including an old non-primary link set, but not a new non-primary link set
 * which receives a directed TFP, and not on a primary link set which receives
 * nothing.
 *
 * 13.3.1 ... Transfer-allowed message are always address to an adjacent
 * signalling point.  They may use any available signalling route that leads
 * to that signalling point.
 *
 * 13.3.2  A transfer-allowed message relating to a given destination X is
 * sent form signalling transfer point Y in the following cases:
 *
 * i)  When signalling transfer point Y stops routing (at changeback or
 *     controlled re-routing), signalling traffic destined to signalling point
 *     X via a signalling transfer point Z (to which the concerned traffic was
 *     previously diverted as a consequence of changeover or forced
 *     rerouting).  In this case, the transfer-allowed message is sent to
 *     signalling transfer point Z.
 *
 * ii) When signalling transfer point Y recognizes that it is again able to
 *     transfer signalling traffic destined to signalling point X (see 6.2.3
 *     and 8.2.3).  In this case a transfer-allowed message is sent to all
 *     accessible adjacent signalling points, except those signalling points
 *     that receive a TFP message according to 13.2.2 i) and except signalling
 *     point X if it is an adjacent point. (Broadcast Method).
 */
static int
mtp_tfa_broadcast(queue_t *q, struct rs *rs)
{
	struct sp *sp = rs->sp.sp;
	struct ls *lc, *lp, *ls;
	uint sls = 0;
	int err;

	/* inform users */
	if ((err = mtp_resume_ind_all_local(q, rs)))
		return (err);

	/* adjacent SP broadcast is only for STPs */
	/* do not broadcast during restart phase 1 */
	if (!(sp->flags & SPF_XFER_FUNC) || (sp->flags & SPF_RESTART_PHASE_1))
		return (0);

	/* current and primary linksets were addressed during rerouting below */
	lc = rs->rl.curr ? rs->rl.curr->ls.ls : NULL;	/* current link set */
	lp = rs->rl.list ? rs->rl.list->ls.ls : NULL;	/* primary link set */

	for (ls = sp->ls.list; ls; ls = ls->sp.next) {
		struct lk *lk;

		for (lk = ls->lk.list; lk; lk = lk->ls.next) {
			struct rs *adj = lk->sp.adj;

			if (ls == lc || ls == lp)
				continue;
			if (!(rs->flags & RSF_CLUSTER)) {
				if (adj->dest == rs->dest)
					continue;
				mtp_send_tfa(q, sp, lk->ni, adj->dest, sp->pc, sls,
					     rs->dest);
			} else
				mtp_send_tca(q, sp, lk->ni, adj->dest, sp->pc, sls,
					     rs->dest);
			sls = (sls + 1) & ((1 << sp->rs.sls_bits) - 1);

			/* Note that errors are ignored on sending TFA/TCA because
			   remote route set test procedures will discover the loss. */
		}
	}
	return (0);
}

/**
 * mtp_tfa_reroute: - perform TFA procedure for STPs during rerouting
 * @q: active queue
 * @rl: route list to allow traffic from
 *
 * This performs special TFA procedures for STPs.  When rerouting traffic away
 * from a route list, to a lower cost route list, send TFA/TCA for the
 * rerouted traffic to let the adjacent signalling point know that it can now
 * route traffic via this STP.
 *
 * Note that we ignore the error on sending TFA or TCA, because if we fail to
 * send it now, the route set test procedures of the adjacent signalling point
 * will discover the situation.  Also, if we are short of buffers, we do not
 * mind that the adjacent signalling point does not yet know of our
 * availability for these routes.
 */
static void
mtp_tfa_reroute(queue_t *q, struct rl *rl)
{
	struct rs *rs = rl->rs.rs;
	struct sp *sp = rs->sp.sp;
	struct rt *rt;
	uint sls = 0;

	if (!(sp->flags & SPF_XFER_FUNC) || (sp->flags & SPF_RESTART_PHASE_1))
		return;

	for (rt = rl->rt.list; rt; rt = rt->rl.next) {
		struct lk *lk = rt->lk.lk;
		struct rs *adj = lk->sp.adj;

		if (!(rs->flags & RSF_CLUSTER)) {
			if (adj->dest == rs->dest)
				continue;
			mtp_send_tfa(q, sp, lk->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		} else
			mtp_send_tca(q, sp, lk->ni, adj->dest, sp->pc, sls,
				     rs->dest);
		sls = (sls + 1) & ((1 << sp->rs.sls_bits) - 1);
	}
}

/**
 * mtp_trw_broadcast:- broadcast TRW to all adjacent nodes
 * @q: active queue
 * @sp: signalling point for which to broadcast
 *
 * ANSI T1.111.4/20000 9.2 ... When the first signalling link goes into the in
 * service state at level 2, the restarting signalling point begins taking
 * into account any transfer-prohibited, transfer-restricted,
 * transfer-allowed, traffic restart allowed, and traffic restart waiting
 * messages received.  The restarting point starts timers T22 and T26 either
 * when the first signalling link goes into the in service state at level 2 or
 * when the first signalling link becomes available at level 3.  When a
 * traffic restart waiting message is received before user traffic is
 * restarted on the link(s) to the point that sent the traffic restart waiting
 * message, timer T25 is started and user traffic is not restarted on that
 * link set until after a traffic restart allowed message is received or timer
 * T25 expires (see 9.3).
 *
 * When the first signalling link of a signalling link set is available, MTP
 * message traffic terminating at the far end of the signalling link set is
 * immediately restarted (see also 9.5), and a traffic restart waiting message
 * is sent to the point at the end of the link set.
 *
 * Whenever timer T26 expires, the restarting signalling point restarts timer
 * T26 and broadcasts a traffic restart waiting message to those adjacent
 * signalling points connected by an available link.
 */
static int
mtp_trw_broadcast(queue_t *q, struct sp *sp)
{
	struct rs *rs;
	uint sls = 0;

	for (rs = sp->rs.list; rs; rs = rs->sp.next) {
		/* only send to adjacent */
		if (!(rs->flags & RSF_ADJACENT))
			continue;
		/* only send to adjacent on direct link set */
		if (!rs->rl.curr || !rs->rl.curr->rt.curr
		    || rs->rl.curr->rt.curr->lk.lk->sp.adj != rs)
			continue;
		mtp_send_trw(q, sp, sp->ni, rs->dest, sp->pc, sls);
		sls = (sls + 1) & ((1 << sp->rs.sls_bits) - 1);
	}
}

/**
 * mtp_xfer_route: - route messages through a signalling transfer point
 * @q: active queue
 * @sl: signalling link from which message arrived
 * @mp: the message
 * @m: the message decoding
 *
 * Full routing for messages transfered through a signalling transfer point.
 * This is for normal transfer messages (including the possibility of circular
 * route tests).
 */
static int
mtp_xfer_route(queue_t *q, struct sl *sl, mblk_t *mp, struct mtp_msg *m)
{
	struct lk *lk = sl->lk.lk;	/* the link (set) on which the message
					   arrived */
	struct rs *rs;			/* the outgoing route set when transferring */
	struct rl *rl;			/* the outgoing route list when transferring 
					 */
	struct rt *rt;			/* the outgoing route when transferring */
	struct ls *ls;			/* the outgoing link set when transferring */
	struct sp *loc = lk->sp.loc;	/* the local signalling point the message
					   arrived for */
	struct rs *adj = lk->sp.adj;	/* the route set to the adjacent of the
					   arriving link */
	struct na *na = loc->na.na;	/* the local network appearance, protocol
					   and options */
	int err;

	/* 9.6.6 All messages to another destination received at a signalling point
	   whose MTP is restarting are discarded. */
	if (loc->state >= SP_RESTART)
		goto discard;
	/* this linear search could be changed to a hash later */
	for (rs = loc->rs.list; rs; rs = rs->sp.next)
		if (rs->rs_type == RT_TYPE_MEMBER && rs->dest == m->dpc)
			goto rs_found;
	for (rs = loc->rs.list; rs; rs = rs->sp.next)
		if (rs->rs_type == RT_TYPE_CLUSTER
		    && rs->dest == (m->dpc & loc->na.na->mask.cluster))
			goto rs_found;
	/* Somebody could be probing our route sets.  There is an additional reason
	   for not sending responsive TFP here and it is described in Q.704: if we
	   send TFP, we will get a lot of uninvited RST messages every T10 inquiring 
	   as to the state of the route, when a configuration error clearly exists.
	   We should screen this message and inform management instead. */
	goto screened;
      rs_found:
	todo(("This is where we could do enhanced gateway screening\n"));
	switch (rs->state) {
	case RS_RESTRICTED:
		/* Note: rr.list will be null if we do not send TFR */
		if (rs->rr.list) {
			/* if the message came in on a link set to which we have not 
			   yet sent responsive TFR, then we should send responsive
			   TFR */
			struct rr *rr;

			for (rr = rs->rr.list; rr; rr = rr->rs.next) {
				if (rr->lk == lk) {
					/* send responsive TFR on this link */
					fixme(("Send responsive TFR on this link\n"));
					mtp_free_rr(rr);
					break;
				}
			}
			/* Note: if we are using TFR responsive method and this is
			   the very first TFR sent, we need to start timer T18a. */
			if ((rs->flags & RSF_TFR_PENDING)) {
				rs->flags &= ~RSF_TFR_PENDING;
				rs_timer_start(q, rs, t18a);
			}
		}
		/* fall through */
	case RS_CONGESTED:
	case RS_DANGER:
	case RS_ALLOWED:
		break;
	case RS_RESTART:
		/* 9.6.7 In adjacent signalling points during the restart procedure, 
		   message not part of the restart procedure but which are destined
		   to or through the signalling point whose MTP is restarting, are
		   discarded. */
		goto discard;
	case RS_PROHIBITED:
	case RS_BLOCKED:
	case RS_INHIBITED:
	case RS_INACTIVE:
		/* if the destination is currently inaccessible and we have not sent 
		   a TFP within the last t8, send a responsive TFP */
		if (!rs_timer_remain(rs, t8)) {
			if (!(rs->flags & RSF_CLUSTER))
				mtp_send_tfp(q, loc, m->ni, adj->dest, loc->pc,
					     m->sls, rs->dest);
			else
				mtp_send_tcp(q, loc, m->ni, adj->dest, loc->pc,
					     m->sls, rs->dest);
			rs_timer_start(q, rs, t8);
		}
		return (0);
	}
	if (!(rl = rs->rl.curr))
		goto efault;
	if (!(ls = rl->ls.ls))
		goto efault;
	/* select active route based on sls */
	if (!(rt = rl->smap[sls_index(m->sls, ls->rl.sls_mask, ls->rl.sls_bits)].rt))
		goto efault;
	if (rt->type == MTP_OBJ_TYPE_CR) {
		/* we are performing controlled rerouting, buffer message */
		bufq_queue(&((struct cr *) rt)->buf, mp);
		return (0);
	}
	/* Selection signalling link in link (set): for ITU-T we need to use the
	   bits that were not used in route selection; for ANSI we use the least
	   significant 4 bits (but could be 7 for 8-bit SLSs) after route selection
	   and sls rotation.  Again, the following algorithm does the job for both
	   ITU-T and ANSI and will accomodate quad linksets to quad STPs. */
	/* select active signalling link based on sls */
	if (!
	    (sl =
	     rt->lk.lk->smap[sls_index(m->sls, ls->lk.sls_mask, ls->lk.sls_bits)].
	     sl))
		goto efault;
	/* Check for changeback on the signalling link index: normally the pointer
	   is to a signalling link object not a changeback buffer object. This
	   approach is faster than the previous approach. */
	if (sl->type == MTP_OBJ_TYPE_CB) {
		/* we are performing changeback or changeover, buffer message */
		bufq_queue(&((struct cb *) sl)->buf, mp);
		return (0);
	}
	if (!(na->option.popt & SS7_POPT_MPLEV))
		m->mp = 0;
	/* ANSI T1.111.4 (2000) 3.8.2.2.  ...  When the transmit buffer is full, all 
	   messages destined for the link should be discarded.  Transfer controlled
	   messages indicating level 3 congestion should be sent to the originators
	   of messages destined for the congested link when the messages are
	   received, if the received messages are not priority 3.  When priority 3
	   messages destined for a congested link are received and the transmit
	   buffer is full, transfer-controlled messages are not sent to the
	   originators of the received priority 3 messages. */
	if (sl->cong_status > m->mp && m->mp != 3)
		if ((err =
		     mtp_send_tfc(q, loc, lk->ni, m->opc, loc->pc, m->sls, m->dpc,
				  sl->cong_status)))
			goto error;
	if (sl->disc_status > m->mp)
		goto discard;
	if (!canputnext(sl->wq))
		goto ebusy;
	if (rl->rt.numb > 1) {
		/* Only rotate when part of the sls was actually used for route
		   selection (i.e. not on C-Links or F-Links. */
		switch (na->option.pvar) {
		case SS7_PVAR_ANSI:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_CHIN:	/* ??? */
			/* Actually, for 5-bit and 8-bit compatability, we rotate
			   based on 5 sls bits regardless of the number of bits in
			   the SLS */
			if (m->sls & 0x01)
				m->sls =
				    (m->sls & 0xe0) | ((m->sls & 0x1f) >> 1) | 0x10;
			else
				m->sls =
				    (m->sls & 0xe0) | ((m->sls & 0x1f) >> 1) | 0x00;
			/* rewrite sls in routing label */
			mp->b_rptr[7] = m->sls;
			break;
		}
	}
	sl->flags |= SLF_TRAFFIC;
	putnext(sl->wq, mp);	/* send message */
	return (0);
      screened:
	todo(("Deliver screened message to MGMT\n"));
	goto discard;
      efault:
	mi_strlog(q, 0, SL_ERROR, "discard: no transfer route");
	goto discard;
      discard:
	freemsg(mp);
	return (0);		/* discard message */
      ebusy:
	return (-EBUSY);
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
/**
 * cr_reroute_buffer: - reroute controlled rerouting buffer contents
 * @cr: controlled rerouting buffer
 *
 * Controlled rerouting is performed with a special controlled rerouting
 * buffer which is attached to both the route from which traffic is being
 * rerouted and the route to which traffic is being rerouted.  If the route
 * set has become unavailable while the timer was running, this will result in
 * purging the controlled rerouting buffer.  If the unavailability was
 * transient and the route set is now available again, these messages will not
 * be lost.  This is not mentioned in the MTP specifications.
 *
 * Rerouting a controlled rerouting buffer consists of diverting the buffer
 * contents on the normal route onto which traffic was being held for
 * diversion.
 */
static void
cr_reroute_buffer(struct cr *cr)
{
	struct cr **cr_slot = &cr->rl.rl->smap[cr->index].cr;
	struct rt **rt_slot = &cr->rl.rl->smap[cr->index].rt;

	cr_timer_stop(cr, tall);
	if (cr->rt.onto) {
		if (*cr_slot == cr) {
			*cr_slot = NULL;
			*rt_slot = cr->rt.onto;
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
	return;
}

/**
 * cr_cancel_buffer: - cancel a controlled rerouting buffer
 * @cr: controlled rerouting buffer
 *
 * Cancelling a controlled rerouting buffer consists of sending the buffer
 * contents back on the original route from which traffic was being held for
 * diversion.  This occurs when the route becomes aviailable again before the
 * controlled rerouting timer expires.
 */
static void
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
			*cr_slot = NULL;
			*rt_slot = cr->rt.from;
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
	/* can't cancel, reroute forward */
	pswerr(("Forward routing non-cancellable controller rerouting buffer.\n"));
	cr_reroute_buffer(cr);
	return;
}

/**
 * cb_divert_buffer: - divert changeover/changeback buffer contents
 * @cb: changeover/changeback buffer
 *
 * Rerouting changeback/changeover buffer consists of diverting the buffer
 * contents on the normal signalling link onto which traffic was being held
 * for diversion (changeover or changeback).
 */
static void
cb_divert_buffer(struct cb *cb)
{
	struct cb **cb_slot = &cb->lk.lk->smap[cb->index].cb;
	struct sl **sl_slot = &cb->lk.lk->smap[cb->index].sl;

	cb_timer_stop(cb, tall);
	if (cb->sl.onto) {
		if (*cb_slot == cb) {
			cb_slot = NULL;
			*sl_slot = cb->sl.onto;
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
	return;
}

/**
 * cb_cancel_buffer: - cancel a changeover/changeback buffer
 * @cb: changeover/changeback buffer
 *
 * Cancelling a changeback/changeover buffer consists of sending the buffer
 * contents back on the original signalling link from which traffic was being
 * held for diversion (changeover or changeback).
 */
static void
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
			*cb_slot = NULL;
			*sl_slot = cb->sl.from;
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
	/* can't cancel, change forward */
	pswerr(("Forward change non-cancellable changeover/changeback buffer.\n"));
	cb_divert_buffer(cb);
	return;
}

/*
 *  Rerouting Traffic
 *  =========================================================================
 */

/**
 * rl_reroute: - reroute traffic between route lists
 * @q: active queue
 * @rs: route set to which route list belongs
 * @rl_onto: the route list to which to reroute traffic
 * @force: forced or controlled rerouting
 *
 * Reroute traffic from the current route-list (rl_from) onto the specified
 * route-list (rl_onto).  Any controlled rerouting buffers will be reused and
 * T6 timers left running.  Any traffic flows that do not already have
 * controlled rerouting buffers will have buffers allocated (controlled
 * rerouting) or not (forced rerouting).
 */
static void
rl_reroute(queue_t *q, struct rs *rs, struct rl *rl_onto, const bool force)
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
		int total = (1 << rs->rl.sls_bits);

		while ((cr = rl_from->cr.list)) {
			/* if there are existing controlled rerouting buffers in the 
			   SLS map, move them to the new route list, leaving
			   messages and timers running */
			/* remove from old list */
			rl_from->smap[cr->index].rt = cr->rt.onto;
			if ((*cr->rl.prev = cr->rl.next))
				cr->rl.next->rl.prev = cr->rl.prev;
			cr->rl.rl = NULL;
			/* insert into new list */
			if ((cr->rl.next = rl_onto->cr.list))
				cr->rl.next->rl.prev = &cr->rl.next;
			cr->prev = &rl_onto->cr.list;
			rl_onto->cr.list = cr;
			cr->rl.rl = rl_onto;
			cr->rt.onto = rl_onto->smap[cr->index].rt;
			rl_onto->smap[cr->index].cr = cr;
		}
		for (index = 0; index < total; index++) {
			if (rl_onto->smap[index].rt->type == MTP_OBJ_TYPE_RT
			    && !force) {
				/* if there are not controlled rerouting buffers in
				   the resulting SLS map, create them if we are
				   doing controlled rerouting */
				struct rt *rt_from = rl_from->smap[index].rt;
				struct rt *rt_onto = rl_onto->smap[index].rt;

				if ((cr =
				     mtp_alloc_cr(0, rl_onto, rt_from, rt_onto,
						  index))) {
					cr_timer_start(q, cr, t6);
					rl_onto->smap[index].cr = cr;
				}
			}
		}
	}
	/* special transfer-allowed, -restricted, and -prohibited procedures only
	   apply to STPs. */
	if (!sp->flags & SPF_XFER_FUNC) {
		if (rl_from && rl_from != rl_prim) {
			/* rerouting from a non-primary route list */
			switch (rs->state) {
			case RS_ALLOWED:
			case RS_DANGER:
			case RS_CONGESTED:
				/* perform STP transfer-allowed procedure */
				mtp_tfa_reroute(q, rl_from);
				break;
			case RS_RESTRICTED:
				/* perform STP transfer-restricted procedure */
				mtp_tfr_reroute(q, rl_from);
				break;
			}
		}
		if (rl_onto && rl_onto != rl_prim) {
			/* rerouting onto a non-primary route list */
			/* perform STP transfer-prohibited procedure */
			mtp_tfp_reroute(q, rl_onto);
		}
	}
	/* make new route list current */
	rs->rl.curr = rl_onto;
}

/**
 * rt_reroute: - reroute between routes
 * @q: active queue
 * @rl: route list to which routes belong
 * @rt_onto: route to which to reroute traffic
 * @index: traffic selection
 * @force: forced or controlled rerouting
 *
 * Reroute traffic from the current route for the index (rt_from) onto the
 * specified route (rl_onto).  Any controlled rerouting buffers are
 * automatically assumed and T6 timers left running.  Any traffic flows that
 * do not already have controlled rerouting buffers will have buffers
 * allocated (controlled rerouting) or not (forced rerouting).
 */
static void
rt_reroute(queue_t *q, struct rl *rl, struct rt *rt_onto, uint index,
	   const bool force)
{
	struct rt *rt;
	struct cr *cr;

	if (!(rt = rl->smap[index].rt) || rt->type == MTP_OBJ_TYPE_RT) {
		if (rt != rt_onto) {
			if (rt) {
				/* unload existing route */
				rt->load--;
				rl->smap[index].rt = NULL;
			}
			if (rl == rl->rs.rs->rl.curr && !force
			    && (cr = mtp_alloc_cr(0, rl, rt, rt_onto, index))) {
				cr_timer_start(q, cr, t6);
				rl->smap[index].cr = cr;
			} else {
				/* force reroute anyway */
				rl->smap[index].rt = rt_onto;
			}
			rt_onto->load++;
		}
	} else if ((cr = rl->smap[index].cr)->type == MTP_OBJ_TYPE_CR) {
		rt = cr->rt.onto;
		if (rt != rt_onto) {
			/* move reroute buffer to new target and reload regardless
			   of force */
			if (rt) {
				/* unload existing route */
				rt->load--;
				cr->rt.onto = NULL;
			}
			cr->rt.onto = rt_onto;
			rt_onto->load++;
		}
	} else {
		mi_strlog(q, 0, SL_ERROR, "rt_reroute() bad object type in SMAP");
	}
}

/**
 * rt_reroute_all: - reroute all traffic to a single route
 * @q: active queue
 * @rl: route list to which route belongs
 * @rt_onto: route onto which to reroute traffic
 * @force: forced or controlled rerouting
 *
 * The specified route takes over all traffic for the route-list.  This is
 * used when the specified route is the only accessible route.
 *
 * If this will also be the only active route list in the route set and the
 * route set is to an adjacent signalling point, and the route is not via a
 * direct link set (handled separately), then the adjacent signalling point is
 * restarting via a non-direct route.
 *
 * Q.704/1996 9.3.3 When signalling point Y becomes accessible by means other
 * than via a direct link set between X and Y, X sends an MTP-RESUME primitive
 * concerning Y to all local MTP Users.  In addition, if signalling point X
 * has the transfer function, X sends to Y any required transfer-prohibited
 * and transfer restricted messages on the available route.  X then broadcasts
 * TFA and/or TFR messages (see clause 13) concerning Y.  Note that X should
 * not in this
 *
 * ANSI T1.111.4/2000 9.3 .. When signalling point Y becomes accessible via a
 * route other than a direct link set between X and Y, X sends an MTP-RESUME
 * primitive concerning Y to all local MTP users.  In addition, if signalling
 * point X has the transfer function, X send to Y any required
 * transfer-prohibited and transfer-restricted message by the available route
 * and broadcasts transfer-allowed or transfer-restricted messages concerning
 * Y.  case alter any routing data other than that for Y.
 */
static void
rt_reroute_all(queue_t *q, struct rl *rl, struct rt *rt_onto, const bool force)
{
	int i;
	int total = (1 << rl->rt.sls_bits);

	for (i = 0; i < total; i++)
		rt_reroute(q, rl, rt_onto, i, force);
	return;
}

/**
 * rt_reroute_add: - add traffic to a newly accesible route
 * @q: active queue
 * @rl: route list to which the route belongs
 * @rt_onto: route to which to add traffic
 * @active: the number of active routes including this one
 * 
 * Add the specified accessible or restricted route to hand traffic within the
 * route-list.  Assume traffic dedicated to the specified route as well as any
 * non-normal excess traffic from other active routes in the route-list.
 *
 * The newly restored route is loaded only to maxload and not maxload +
 * excess.  This is to avoid unnecessarily rerouting 1 slot's worth of traffic
 * under some conditions.  Existing active routes are left loaded to maxload +
 * excess, to narrow the load disparity between routes to 1/total from
 * 2/total.
 *
 * When rerouting back traffic, reroute the traffic selection when, (1) no
 * route is assigned for the traffic selection (this only occurs when the
 * newly available route is the only route serving the route list), (2) the
 * traffic selection is normally sent to the restored route, (3) the traffic
 * selection is not normally sent to the route to which it is currently send,
 * and the route to which it is currently send would be excessively loaded.
 */
static void
rt_reroute_add(queue_t *q, struct rl *rl, struct rt *rt_onto, int active)
{
	int i, n = rl->rt.numb;
	int total = (1 << rl->rt.sls_bits);
	int maxload = total / active;
	int excess = ((total % active) != 0) ? 1 : 0;

	for (i = 0; i < total && rt_onto->load <= maxload; i++) {
		struct rt *rt = rl->smap[i].rt;

		if (!rt) {
			rl->smap[i].rt = rt_onto;
			rt_onto->load++;
		}
	}
	for (i = 0; i < total && rt_onto->load <= maxload; i++) {
		struct rt *rt = rl->smap[i].rt;

		if (rt->type == MTP_OBJ_TYPE_CR)
			rt = ((struct cr *) rt)->rt.onto;
		if (rt == rt_onto)
			continue;
		if (rt_onto->slot == (i % n))
			/* controlled rerouting for addition */
			rt_reroute(q, rl, rt_onto, i, false);
	}
	for (i = 0; i < total && rt_onto->load <= maxload; i++) {
		struct rt *rt = rl->smap[i].rt;

		if (rt->type == MTP_OBJ_TYPE_CR)
			rt = ((struct cr *) rt)->rt.onto;
		if (rt == rt_onto)
			continue;
		if (rt->load > maxload + excess && rt->slot != (i % n))
			/* controlled rerouting for addition */
			rt_reroute(q, rl, rt_onto, i, false);
	}
	if (rt_onto->load < maxload)
		mi_strlog(q, 0, SL_ERROR, "rt_reroute_add() route not fully loaded");
}

/**
 * rt_reroute_can: - cancel controlled rerouting for a route
 * @rl: route list containing the route
 * @rt: route for which to cancel controlled rerouting
 *
 * When a route fails, this procedure is used to cancel any controlled
 * rerouting which is being performed onto the failed route.  At any given
 * point in time the number of controlled rerouting buffers that are allocated
 * should rarely be greater than zero.
 */
static void
rt_reroute_can(struct rl *rl, struct rt *rt)
{
	struct cr *cr, *cr_next = rl->cr.list;

	while (rt->load && (cr = cr_next)) {
		cr_next = cr->rl.next;
		if (cr->rt.onto == rt)
			cr_cancel_buffer(cr);
	}
}

/**
 * rt_reroute_sub: - remove route from handling traffic
 * @q: active queue
 * @rl: route list to which the route belongs
 * @rt_from: the route from which to divert traffic
 * @accessible: the number of accessible routes after the reroute
 *
 * Remove the specified route from handling traffic within the route-list.
 * Offload the traffic from the specified route to other routes currently
 * handling traffic.
 *
 * First, cancel any controlled rerouting procedures for the route.
 */
static void
rt_reroute_sub(queue_t *q, struct rl *rl, struct rt *rt_from, int accessible)
{
	int i;
	int total = (1 << rl->rt.sls_bits);
	int maxload = (total + accessible - 1) / accessible;

	/* first cancel any route controlled rerouting to the failed route */
	rt_reroute_can(rl, rt_from);
	for (i = 0; i < total && rt_from->load; i++) {
		struct rt *rt = rl->smap[i].rt;

		if (rt && rt->type == MTP_OBJ_TYPE_CR)
			rt = ((struct cr *) rt)->rt.onto;
		if (!rt || rt != rt_from)
			continue;
		for (rt = rl->rt.list; rt; rt = rt->rl.next)
			if (rt->load && rt->load < maxload)
				/* force rerouting for subtraction */
				rt_reroute(q, rl, rt, i, true);
	}
	if (rt_from->load != 0)
		mi_strlog(q, 0, SL_ERROR, "rt_reroute_sub() did not unload route %u",
			  rt_from->id);
}

/**
 * rt_reroute_res: - remove route from handling traffic
 * @q: active queue
 * @rl: route list to which the route belongs
 * @rt_from: the route from which to divert traffic
 * @restricted: the number of restricted routes after the reroute
 *
 * Remove the specified accessible route from handling traffic within the
 * route-list.  Offload the traffic from the specified route to restricted
 * routes currently handling traffic.
 *
 * First, cancel any controlled rerouting procedures for the route.
 */
static void
rt_reroute_res(queue_t *q, struct rl *rl, struct rt *rt_from, int restricted)
{
	int i;
	int total = (1 << rl->rt.sls_bits);
	int maxload = (total + restricted - 1) / restricted;

	/* first cancel any route controlled rerouting to the failed route */
	rt_reroute_can(rl, rt_from);
	for (i = 0; i < RT_SMAP_SIZE && rt_from->load; i++) {
		struct rt *rt = rl->smap[i].rt;

		if (rt && rt->type == MTP_OBJ_TYPE_CR)
			rt = ((struct cr *) rt)->rt.onto;
		if (!rt || rt != rt_from)
			continue;
		for (rt = rl->rt.list; rt; rt = rt->rl.next)
			if (rt->state == RT_RESTRICTED && rt->load < maxload)
				/* force rerouting under failure */
				rt_reroute(q, rl, rt, i, true);
	}
	assure(rt_from->load == 0);
}

/*
 *  Diverting Traffic
 *  =========================================================================
 */

/**
 * sl_changeback: - changeback onto a newly restored signalling link
 * @q: active queue
 * @lk: link set
 * @sl_onto: newly restored signalling link
 * @index: sls index to changeback
 */
static void
sl_changeback(queue_t *q, struct lk *lk, struct sl *sl_onto, uint index)
{
	struct cb *cb;
	struct sl *sl, **slp;

	if (!sl_onto) {
		mi_strlog(q, 0, SL_ERROR, "sl_changeback() passed NULL pointer");
		return;
	}
	if (!(sl = lk->smap[index].sl)) {
		mi_strlog(q, 0, SL_ERROR, "sl_changeback() no signalling link");
		return;
	}
	if (sl->type == MTP_OBJ_TYPE_CB) {
		cb = ((struct cb *) sl);
		slp = &cb->sl.onto;
	} else {
		cb = NULL;
		slp = &lk->smap[index].sl;
	}
	if ((sl = *slp)) {
		if (sl == sl_onto) {
			/* we would be taking over our own change: just cancel the
			   change */
			cb_cancel_buffer(cb);
			return;
		}
		if (sl->load)
			sl->load--;
		*slp = NULL;
	}
	*slp = sl_onto;
	sl_onto->load++;
	/* if carrying traffic for the link set */
	if (lk->load && sl) {
		struct rt *rt = NULL;
		struct rl *rl;
		struct lk *lk = sl_onto->lk.lk;
		struct sp *loc = lk->sp.loc;
		struct rs *adj = lk->sp.adj;
		int err;

		if (cb)
			cb_timer_stop(cb, tall);
		else if (!(cb = mtp_alloc_cb(0, lk, sl, sl_onto, index))) {
			/* forced changeback */
			return;
		}
		for (rl = adj->rl.list; rl; rl = rl->rs.next)
			for (rt = rl->rt.list; rt; rt = rt->rl.next)
				if (rt->lk.lk == lk)
					break;
		if (!rt) {
			mi_strlog(q, 0, SL_ERROR,
				  "sl_changeback() no route for link set");
			/* forced changeback */
			return;
		}
		if (rt->state < RT_RESTART) {
			/* sequence changeback */
			if ((err =
			     mtp_send_cbd(q, loc, lk->ni, adj->dest, loc->pc,
					  sl_onto->slc, sl_onto->slc, cb->cbc, sl)))
				return (err);
			cb_timer_start(q, cb, t4);
		} else {
			/* time controlled changeback */
			cb_timer_start(q, cb, t3);
		}
	}
	return;
}

/**
 * sl_changeover: - changeover from a newly unusable signalling link
 * @q: active queue
 * @lk: link set
 * @sl_onto: signalling link to changeover to
 * @index: traffic selection to changeover
 */
static void
sl_changeover(queue_t *q, struct lk *lk, struct sl *sl_onto, uint index)
{
	struct cb *cb;
	struct sl *sl, **slp;

	if (!sl_onto) {
		mi_strlog(q, 0, SL_ERROR, "sl_changeover() passed NULL pointer");
		return;
	}
	if (!(sl = lk->smap[index].sl)) {
		mi_strlog(q, 0, SL_ERROR, "sl_changeover() no signalling link");
		return;
	}
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
		*slp = NULL;
	}
	*slp = sl_onto;
	sl_onto->load++;
	if (lk->load && sl) {
		struct lk *lk = sl->lk.lk;
		struct rs *adj = lk->sp.adj;

		if (cb)
			cb_timer_stop(cb, tall);
		else if (!(cb = mtp_alloc_cb(0, lk, sl, sl_onto, index))) {
			/* forced changeover */
			return;
		}
		if (adj->state < RS_RESTART) {
			/* sequence changeover */
			/* the changeover (COO or ECO) should be sent after BSNT
			   retrieval succeeds or fails */
			cb_timer_start(q, cb, t2);
		} else {
			/* time controlled changeover */
			cb_timer_start(q, cb, t1);
		}
	}
	return;
}

/**
 * sl_changeback_add: - add and changeback to a newly active signalling link
 * @lk: link set containing the signalling link
 * @sl_onto: signalling link to changeback onto
 * @active: number of active signalling links (in link set) after changeback
 *
 * Note that the entire smap is not necessarily significant, just the number
 * of significant SLS values for this Signalling Point.
 *
 * Some notes:
 *
 * The newly restored signalling link is loaded only to maxload and not
 * maxload + excess.  This is to avoid unnecessarily changing over 1 slot's
 * worth of traffic under some conditions.  Existing active links are left
 * loaded to maxload + excess, to narrow the load disparity between signalling
 * links to 1/total from 2/total.
 *
 * When changing back traffic, changeback the traffic selection when, (1) no
 * signalling link is assigned for the traffic selection (this only occurs
 * when the restored signalling link is the only signalling link serving the
 * link (set)), (2) the traffic selection is normally sent to the restored
 * signalling link, (3) the traffic selection is not normally sent to the link
 * to which it is currently sent, and the link to which it is currently sent
 * would be excessively loaded.
 */
static void
sl_changeback_add(queue_t *q, struct lk *lk, struct sl *sl_onto, int active)
{
	int i, n = lk->sl.numb;
	int total = (1 << lk->sl.sls_bits);
	int maxload = total / active;
	int excess = ((total % active) != 0) ? 1 : 0;

	for (i = 0; i < total && sl_onto->load <= maxload; i++) {
		struct sl *sl = lk->smap[i].sl;

		if (!sl) {
			lk->smap[i].sl = sl_onto;
			sl_onto->load++;
		}
	}
	for (i = 0; i < total && sl_onto->load <= maxload; i++) {
		struct sl *sl = lk->smap[i].sl;

		if (sl->type == MTP_OBJ_TYPE_CB)
			sl = ((struct cb *) sl)->sl.onto;
		if (sl == sl_onto)
			continue;
		if (sl_onto->slot == (i % n))
			/* changeback for addition */
			sl_changeback(q, lk, sl_onto, i);
	}
	for (i = 0; i < total && sl_onto->load <= maxload; i++) {
		struct sl *sl = lk->smap[i].sl;

		if (sl->type == MTP_OBJ_TYPE_CB)
			sl = ((struct cb *) sl)->sl.onto;
		if (sl == sl_onto)
			continue;
		if (sl->load > maxload + excess && sl->slot != (i % n))
			/* changeback for addition */
			sl_changeback(q, lk, sl_onto, i);
	}
	if (sl_onto->load < maxload)
		mi_strlog(q, 0, SL_ERROR,
			  "sl_changeback_add() link not fully loaded");
}

/**
 * sl_changeover_can: - cancel changebacks and changeovers for a signalling link
 * @lk: link set containing the signalling link
 * @sl: signalling link for which to cance changeovers and changebacks
 *
 * When a signalling link fails, this procedure is used to cancel any
 * changeovers or changebacks which are being performed onto the failed
 * signalling link.  At any given point in time the number of
 * changeback/changeover buffers that are allocated should rarely be greater
 * than zero.
 */
static void
sl_changeover_can(struct lk *lk, struct sl *sl)
{
	struct cb *cb, *cb_next = lk->cb.list;

	while (sl->load && (cb = cb_next)) {
		cb_next = cb->lk.next;
		if (cb->sl.onto == sl)
			cb_cancel_buffer(cb);
	}
}

/**
 * sl_changeover_sub: - subtract and changeover a newly failed signalling link to active links
 * @lk: link set containing the signalling link
 * @sl_from: signalling link to changeover from
 * @active: number of active signalling links (in link set) after changeover
 *
 * This procedure changes over to active signalling links only.  When there
 * are no active signalling links to which to change over, but there are
 * inhibited signalling links that can be force uninhibited, the
 * sl_changeover_res() procedure below is used instead.
 *
 * First, cancel any current changeover or changeback procedures for the
 * signalling link.
 */
static void
sl_changeover_sub(queue_t *q, struct lk *lk, struct sl *sl_from, int active)
{
	int i;
	int total = (1 << lk->sl.sls_bits);
	int maxload = (total + active - 1) / active;

	/* first cancel any signalling link changing over or back to the failed
	   signalling link */
	sl_changeover_can(lk, sl_from);
	for (i = 0; i < total && sl_from->load; i++) {
		struct sl *sl = lk->smap[i].sl;

		if (sl && sl->type == MTP_OBJ_TYPE_CB)
			sl = ((struct cb *) sl)->sl.onto;
		if (!sl || sl != sl_from)
			continue;
		for (sl = lk->sl.list; sl; sl = sl->lk.next)
			if (sl->load && sl->load < maxload)
				/* changeover for subtraction */
				sl_changeover(q, lk, sl, i);
	}
	if (sl_from->load != 0)
		mi_strlog(q, 0, SL_ERROR,
			  "sl_changeover_sub() did not unload link %u", sl_from->id);
}

/**
 * sl_changeover_res: - subtract and changeover a newly failed signalling link to inhibited links
 * @lk: link set containing the signalling link
 * @sl_from: signalling link to changeover from
 * @active: number of inhibited signalling links (in link set) after changeover
 *
 * This procedure changes over to inhibited signalling links only.  When there
 * are active signalling links to which to change over, the
 * sl_changeover_sub() procedure above is used instead.
 */
static void
sl_changeover_res(queue_t *q, struct lk *lk, struct sl *sl_from, int inhibited)
{
	int i;
	int total = (1 << lk->sl.sls_bits);
	int maxload = (total + inhibited - 1) / inhibited;

	/* first cancel any signalling link changing over or back to the failed
	   signalling link */
	sl_changeover_can(lk, sl_from);
	for (i = 0; i < total && sl_from->load; i++) {
		struct sl *sl = lk->smap[i].sl;

		if (sl && sl->type == MTP_OBJ_TYPE_CB)
			sl = ((struct cb *) sl)->sl.onto;
		if (!sl || sl != sl_from)
			continue;
		for (sl = lk->sl.list; sl; sl = sl->lk.next)
			if (sl->state == SL_INHIBITED && sl->load < maxload)
				/* changeover for subtraction */
				sl_changeover(q, lk, sl, i);
	}
	if (sl_from->load != 0)
		mi_strlog(q, 0, SL_ERROR, "sl_changeover_res() did not unload link");
}

/*
 *  -------------------------------------------------------------------------
 *
 *  State Change Functions
 *
 *  -------------------------------------------------------------------------
 */

static uint
mtp_sta_flags(const uint state, uint oldflags)
{
	uint newflags = oldflags;
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

static uint
mtp_flg_state(uint newflags)
{
	/* *INDENT-OFF* */
	uint newstate =
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

static uint
mtp_cnt_state(struct counters *c, uint oldstate)
{
	/* *INDENT-OFF* */
	uint newstate =
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

/**
 * sp_set_state: - set new signalling point state
 * @q: active queue
 * @sp: signalling point whose state to change
 * @state: the new state
 *
 * ANSI T1.111.4/2000
 * 9.1 ...
 *
 * If the management function determines that a full restart procedure is not
 * needed when the first signalling link(s) become available, then traffic
 * restart allowed message are sent to the adjacent points at the remote ends
 * of the available links, MTP-RESUME primitives are given to users for all
 * destinations accessible via the available links, and the procedure
 * terminates. Any traffic restart allowed or traffic restart waiting messages
 * received from adjacent points are considered unexpected; see 9.4. Messages
 * that were buffered in level 2 (transmit buffer, retransmit buffer, or
 * receive buffer) during the period of unavailability on links that were
 * transmitting or receiving process outage status units (MTP Level 2 links)
 * are discarded unless it can be determined that they were buffers for a time
 * of less than T1.
 *
 * If the management function determines that a full restart procedures is
 * needed, then it ensures, for example, by using local processor outage, that
 * the period of unavailability persists for a time of at least T27. The
 * purpose of remaining unavailable for time T27 is to increase the
 * probability that adjacent points are aware of the unavailability of the
 * restarting point and initiate the appropriate restart procedures (see 9.3).
 *
 * As the first step in restarting, the signalling point attempts to bring a
 * predetermined number of links into the available state in each of its link
 * sets (see Section 12).  Links that are transmitting or receiving processor
 * outage status units become available when reception and transmission of the
 * processor outage status units ceases.  Messages that were buffered in level
 * 2 (transmit buffer, retransmit buffer, or receive buffer) during the period
 * of unavailability on links that were transmitting ore receiving processor
 * outage status units (MTP Level 2 links) are discarded unless it can be
 * determined that they were buffered for a time of less than T1.  Link
 * activation procedures are carried out in parallel on as many other
 * unavailable links as possible (see Section 12).
 */
static int
sp_set_state(queue_t *q, struct sp *sp, const uint state)
{
	uint sp_oldstate = sp->state;
	uint sp_newstate = sp->state;
	uint sp_oldflags = sp->flags;
	uint sp_newflags = sp->flags;

	if ((sp_newflags = mtp_sta_flags(state, sp_oldflags)) == sp_oldflags)
		goto no_flags_change;
	if ((sp_newstate = mtp_flg_state(sp_newflags)) == sp_oldstate)
		goto no_state_change;
	if (sp_oldstate > SP_RESTART && sp_newstate < SP_RESTART) {
		/* This is the first link in service (usable) at Level 3.  Because
		   SPF_RESTART is not set, this is a simple recovery.  No actions
		   are necessary. */
	}
	if (sp_oldstate > SP_RESTART && sp_newstate == SP_RESTART) {
		/* This is the first link in service (usable) at level 3: move to
		   Restart Phase 1, where we collect information about available
		   routes in the network but do not send any information ourselves. */
		sp->flags |= SPF_RESTART_PHASE_1;
		sp->flags &= ~SPF_RESTART_PHASE_2;
		if ((sp->na.na->options.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			/* For ANSI the first half of Phase 1 (waiting for
			   sufficient number of signalling links to move in service) 
			   persists for T22 and the second half of Phase 1 (waiting
			   for a sufficient number of TRAs from adjacent signalling
			   points) persists for T23. T26 is a timer to continually
			   rebroadcast TRW to each adjacent signalling point while
			   the restart process is continuing. */
			sp_timer_start(q, sp, t22a);
			sp_timer_start(q, sp, t26a);
		} else {
			/* For ITU-T, Phase 1 persists for a duration of T18. */
			sp_timer_start(q, sp, t18);
		}
	}
	if (sp_oldstate < SP_RESTART && sp_newstate > SP_RESTART) {
		/* This is the last link in service (usable) at level 3. We need to
		   start T1 for the SP to determine whether MTP Restart is necessary 
		   on recovery.  We have already set emergency on all signalling
		   links and have initiated restoration of any signalling links that 
		   have not been activated. */
		if (!sp_timer_remain(sp, t1r))
			sp_timer_start(q, sp, t1r);
	}
	&sp->na.na->sp.states[sp_oldstate]--;
	&sp->na.na->sp.states[sp_newstate]++;
	sp->state = sp_newstate;
      no_state_change:
	sp->flags = sp_newflags;
      no_flags_change:
	return (0);
}

/**
 * rs_set_state: - set new route set state
 * @q: active queue
 * @rs: route set whose state to change
 * @state: the new state
 *
 * A change to a route set state affects no other state. 
 *
 * Unfortunately, this is one of the only state transition functions that can
 * return an error.  The others have a timer fall-back.  The difficulty here
 * is with local broadcast of MTP-PAUSE, MTP-RESUME and MTP-STATUS.  If a
 * buffer is not available, an error must be returned.
 *
 * ANSI T1.111.4/2000 9.3 A signalling point X considers that the MTP of an
 * inaccessible adjacent signalling point Y is restarting when: (1) The first
 * link in a direct link set is in the in service state at level 2, or (2) A
 * route other than a direct link set becomes available, e.g.  through receipt
 * of a transfer allowed or traffic restart allowed message or the
 * availability of the corresponding link set.
 *
 * When the first link in a direct link set toward the restarting signalling
 * point Y goes into the in services state at level 2, signalling point X
 * begins taking into account any traffic restart waiting, traffic restart
 * allowed, transfer-prohibited, transfer-restricted, and transfer-allowed
 * messages from Y. Signalling point X starts timer T28 either when the first
 * link goes into the in service state at level 2 or when the first signalling
 * link becomes available at level 3.  IN addition it takes the following
 * actions:
 *
 * (1) If a TRW message is received from Y while T28 is running or before it
 *     is started, X starts T25.  X stops T28 if it is running.
 *
 * (2) If a TRW message is received from Y while T25 is running, X restarts T25.
 *
 * (3) When the first link in a link set to Y becomes available, signalling
 *     point X sends to Y a traffic restart allowed message or, if X has the
 *     transfer function, a traffic restart waiting message followed by
 *     transfer-prohibited (note that all transfer prohibited messages
 *     according to 13.2.2 (1) must be sent) and transfer-restricted messages
 *     and a traffic restart allowed message.
 *
 * (4) If a destination becomes prohibited, restricted, or available at X,
 *     after X has sent a traffic restart allowed message to Y, X notifies Y of
 * the status change by the normal procedures in Section 13.
 *
 * (5) When a traffic restart allowed message has been sent to Y and a traffic
 *     restart allowed message has been received from Y, X stops T25 or T28,
 *     whichever is running, and restarts traffic on the link set to Y.  X
 *     gives MTP-RESUME primitives to users concerning Y and any destinations
 *     made accessible via Y.  If X has the transfer function it also
 *     broadcasts transfer-allowed or transfer-restricted messages concerning
 *     the same destinations.
 *
 * (6) If T28 expires, X restarts traffic on the link set to Y as in (5),
 *     unless a traffic restart allowed message has not been sent to Y.  In
 *     that case, X starts T25 and completes the sending of transfer
 *     prohibited and transfer-restricted messages, followed by a traffic
 *     restart allowed message.  Then, unless a traffic restart waiting
 *     message has been received from Y without a subsequent traffic restart
 *     allowed message, X stops 25 and restarts traffic on the link set to Y.
 *
 * (7) If T25 expires, X restarts traffic on the link set toward Y. In the
 *     abnormal case when X has not completed sending transfer-prohibited and
 *     transfer-restricted messages to Y, X completes sending the
 *     transfer-prohibited messages required by 13.2.2 (1) and sends a traffic
 *     restart allowed message before restarting user traffic.
 *
 * (8) If no traffic restart allowed message has been received from Y when
 *     traffic is restarted to Y, timer T9 (see 9.4) is started.
 *
 * When signalling point Y becomes accessible via a route other than a direct
 * link set between X and Y, X sends a MTP-RESUME primitive concerning Y to
 * all local MTP users. In addition, if signalling point X has the transfer
 * function, X sends to Y any required transfer-prohibited and
 * transfer-restricted message by the available route and broadcasts
 * transfer-allowed or transfer-restricted messages concerning Y.
 *
 * ANSI T1.111.4/2000 9.5 (General Rules) When a signalling point restarts, it
 * considers, at the beginning of the restart procedure, all signalling routes
 * to be allowed and all signalling links to be uninhibited.  A signalling
 * route set test message received in a restarting signalling point during the
 * restart procedure is ignored.
 *
 * Signalling route set test messages received in a signalling point adjacent
 * to a restarting signalling point while T25 or T28 is running are handled,
 * but the relies consider that all signalling routes using the restarting
 * signalling point are prohibited. When T28 expires or T25 is stopped or
 * expires, these signalling routes are allowed unless a transfer-prohibited
 * or transfer-restricted message was received from the restarting signalling
 * point while T28 or T25 was running.
 *
 * While T25 or T28 is running, all traffic from local Level 4 or other
 * signalling points destined to the adjacent restarting point is discarded.
 *
 * When an adjacent signalling point restarts, all signalling links to the
 * restarting point are marked as uninhibited.
 *
 * In the case that an adjacent signalling point becomes inaccessible, but
 * routing control initiates a successful uninhibiting (see 10.3), no restart
 * procedure is performed on either side of the link.  If the inihibiting is
 * unsuccessful because the link has failed or was blocked after it was
 * inihibitied, then MTP restart procedures should apply based on the criteria
 * in Section 9.3.
 *
 * Message traffic is restarted on newly available links by using the time
 * controlled changeback procedure (see 6.4).
 *
 * If a link becomes unavailable during MTP restart, after having been
 * successfully activated during the restart, time controlled changeover is
 * performed (see 5.6.2).
 *
 * If a message concerning another destination is received at a restarting
 * point before TRA messages have been sent out, the restarting point may
 * discard the message or it may route the message if it is able according to
 * the current routing data.  If the restarting point discards the message, it
 * sends a transfer prohibited message to the adjacent signalling point from
 * which the message was received.  If a transfer prohibited message is sent
 * in these circumstances, and a signalling route is established to the
 * concerned destination before the traffic restart allowed messages are
 * broadcast, then a transfer-allowed message is sent to the adjacent point
 * after traffic restart allowed messages are broadcast.
 *
 * A message concerning a local MTP user with service indicator (SI) of 0010
 * is handled normally when received in a restarting signalling point.
 * Treatments for some message with SI = 0000 receive in a restarting
 * signalling point have been specified already in 9.1 through 9.5; other
 * messages with SI = 0000 may be treated normally or discarded when received
 * in a restarting signalling point.  Message with other values of service
 * indicators may be treated normally or discarded when received in the
 * restarting point (appropriate treatment may depend on the application
 * resident at the particular restarting point).
 */
static int
rs_set_state(queue_t *q, struct rs *rs, const uint state)
{
	struct sp *sp = rs->sp.sp;
	uint rs_oldstate = rs->state;
	uint rs_newstate = rs->state;
	uint rs_oldflags = rs->flags;
	uint rs_newflags = rs->flags;
	int err;

	if ((rs_newflags = mtp_sta_flags(state, rs_oldflags)) == rs_oldflags)
		goto no_flags_change;
	if ((rs_newstate = mtp_flg_state(rs_newflags)) == rs_oldstate)
		goto no_state_change;
	if (rs_oldstate > RS_RESTART && rs_newstate <= RS_RESTART) {
		if ((rs->flags & RSF_ADJACENT)) {
			/* perform adjacent restart procedure */
			mtp_tfp_adjacent(q, rs);
			mtp_tfr_adjacent(q, rs);
		}
	}
	if (rs_oldstate == RS_RESTRICTED) {
		struct rr *rr, *rr_next = rs->rr.list;

		rs_timer_stop(rs, t11);
		rs_timer_stop(rs, t18a);
		rs->flags &= ~RSF_TFR_PENDING;
		/* purge responsive TFR list */
		while ((rr = rr_next)) {
			rr_next = rr->rs.next;
			mtp_free_rr(rr);
		}
	}
	if (rs_newstate < RS_RESTRICTED && rs_oldstate >= RS_RESTRICTED) {
		rs_timer_stop(rs, t8);
		/* transfer allowed broadcast */
		if ((err = mtp_tfa_broadcast(q, rs)) < 0)
			return (err);
	}
	if (rs_newstate == RS_RESTRICTED && (sp->na.na->option.popt & SS7_POPT_TFR)) {
		if (rs_oldstate < RS_RESTRICTED) {
			/* prepare for transfer restricted broadcast */
			rs_timer_start(q, rs, t11);
		}
		if (rs_oldstate > RS_RESTRICTED) {
			/* transfer restricted broadcast */
			if ((err = mtp_tfr_broadcast(q, rs)) < 0)
				return (err);
		}
	}
	if (rs_newstate > RS_RESTART && rs_oldstate <= RS_RESTART) {
		rs_timer_start(q, rs, t8);
		/* transfer prohibited broadcast */
		if ((err = mtp_tfp_broadcast(q, rs)))
			return (err);
	}
	rs->state = rs_newstate;
      no_state_change:
	rs->flags = rs_newflags;
      no_flags_change:
	return (0);
}

/**
 * rl_set_state: - set new route list state
 * @q: active queue
 * @rl: route list whose state to change
 * @state: the new state
 *
 * A change to a route list state might also affect the state of a route set.
 *
 * In this driver, a route list is a collection of routes via link sets in a
 * combined link set.  A route set is a collection of route lists.  In an SP,
 * the only case of multiplex route lists is where A-, E- and/or F-links exist
 * that provide access to the same destination.  In an STP, all route sets
 * normally have multiplex route lists, one for each combined B/D link set and
 * one for the C-link set to its associated STP in the pair.  Each route list
 * within a route set has a different cost.  Traffic is directed to the route
 * list with the lowest cost and other route lists do not receive traffic.
 */
static int
rl_set_state(queue_t *q, struct rl *rl, const uint state)
{
	struct rs *rs = rl->rs.rs;
	uint rl_oldstate = rl->state;
	uint rl_newstate = rl->state;
	uint rl_oldflags = rl->flags;
	uint rl_newflags = rl->flags;
	int err;

	if ((rl_newflags = mtp_sta_flags(state, rl_oldflags)) == rl_oldflags)
		goto no_flags_change;
	if ((rl_newstate = mtp_flg_state(rl_newflags)) == rl_oldstate)
		goto no_state_change;
	if (rl_newstate > RL_RESTRICTED && rl_oldstate < RL_RESTART) {
		/* route list moved from accessible to inaccessible */
		if (rl == rs->rl.curr) {
			/* inaccessible route-list was current route-list */
			struct rl *ra;

			for (ra = rs->rl.list;
			     ra && (ra == rl || ra->state > RL_RESTRICTED);
			     ra = ra->rs.next) ;
			rl_reroute(q, rs, ra, true);
			goto reroute;
		}
	}
	if (rl_oldstate > RL_RESTRICTED && rl_newstate < RL_RESTART) {
		/* route list moved from inaccessible to accessible */
		if (!rs->rl.curr || rs->rl.curr->cost > rl->cost) {
			/* accessible route-list is highest priority available
			   route-list */
			rl_reroute(q, rs, rl, false);
			goto reroute;
		}
	}
	if (rl == rs->rl.curr) {
	      reroute:
		rs->rl.states[rl_oldstate]--;
		rs->rl.states[rl_newstate]++;
		if ((err = rs_set_state(q, rs, state))) {
			rs->rl.states[rl_oldstate]++;
			rs->rl.states[rl_newstate]--;
			return (err);
		}
	} else {
		rs->rl.states[rl_oldstate]--;
		rs->rl.states[rl_newstate]++;
	}
	/* push counts to ls */
	rl->ls.ls->rl.states[rl_oldstate]--;
	rl->ls.ls->rl.states[rl_newstate]++;
	/* What we are more interested in than pushing state here is determining the 
	   route loading of the link, and whether the link is in danger of
	   congestion due to route loading.  That is a difficult thing to calculate, 
	   however.  The link can only be in danger of congestion if the routes
	   assigned to it are actually active. This might be better considered at
	   the route-list/link-set level rather than at the route/link level */
	rl->state = rl_newstate;
      no_state_change:
	rl->flags = rl_newflags;
      no_flags_change:
	return (0);
}

/**
 * rt_set_state: - set new route state
 * @q: active queue
 * @rt: route whose state to change
 * @state: the new state
 *
 * A change to a route state might also affect the state of the route list.
 */
static int
rt_set_state(queue_t *q, struct rt *rt, const uint state)
{
	struct rl *rl = rt->rl.rl;
	uint rl_newstate;
	uint rt_oldstate = rt->state;
	uint rt_newstate = rt->state;
	uint rt_oldflags = rt->flags;
	uint rt_newflags = rt->flags;
	int err;

	if ((rt_newflags = mtp_sta_flags(state, rt_oldflags)) == rt_oldflags)
		goto no_flags_change;
	if ((rt_newstate = mtp_flg_state(rt_newflags)) == rt_oldstate)
		goto no_state_change;
	/* ---------------------------------------------- */
	if (rt_newflags & (RTF_PROHIBITED | RTF_RESTRICTED)) {
		/* start signalling route set test procedure */
		rt_timer_start(q, rt, t10);
	} else {
		/* stop signalling route set test procedure */
		rt_timer_stop(rt, t10);
	}
	if (rt_oldstate < RT_RESTART && rt_newstate > RT_RESTRICTED) {
		int accessible = rl->rt.allowed + rl->rt.danger + rl->rt.congested;
		int restricted = rl->rt.restricted;

		if (rt_newstate != RT_RESTRICTED) {
			/* route has moved from accessible to inaccessible */
			if (accessible > 1) {
				/* offload traffic to other accessible routes */
				rt_reroute_sub(q, rl, rt, accessible - 1);
			} else if (restricted) {
				/* offload traffic to restricted routes */
				rt_reroute_res(q, rl, rt, restricted);
			} else {
				/* we are the last accessible route in route list */
				/* empty route list */
				rt_reroute_all(q, rl, NULL, true);
			}
		} else {
			/* route has moved from restricted to inaccessible */
			if (!accessible && restricted > 1) {
				/* route was carrying traffic */
				/* offload traffic to other restricted routes */
				rt_reroute_sub(q, rl, rt, restricted - 1);
			} else if (!accessible) {
				/* we are the last accessible route in route list */
				/* empty route list */
				rt_reroute_all(q, rl, NULL, true);
			}
		}
	}
	if (rt_newstate < RT_RESTART && rt_oldstate > RT_RESTRICTED) {
		int accessible = rl->rt.allowed + rl->rt.danger + rl->rt.congested;
		int restricted = rl->rt.restricted;

		if (rt_oldstate != RT_RESTRICTED) {
			/* route has moved from inaccessible to accessible */
			if (accessible == 0) {
				/* only allowed route in route list */
				/* take over all traffic */
				rt_reroute_all(q, rl, rt, false);
			} else {
				/* other routes are allowed */
				/* rebalance the route load */
				rt_reroute_add(q, rl, rt, accessible + 1);
			}
		} else {
			/* route has moved from inaccessible to restricted */
			if (accessible + restricted == 0) {
				/* only accessible route in route list */
				/* take over all traffic */
				rt_reroute_all(q, rl, rt, false);
			} else if (!accessible) {
				/* other routes are restricted but none are allowed */
				/* rebalance the route load */
				rt_reroute_add(q, rl, rt, restricted + 1);
			}
		}
	}
	/* ---------------------------------------------- */
	/* push state change to rl */
	rl->rt.states[rt_oldstate]--;
	rl->rt.states[rt_newstate]++;
	if ((rl_newstate =
	     mtp_cnt_state((struct counters *) (&rl->rt), rl->state)) != rl->state)
		if ((err = rl_set_state(q, rl, rl_newstate)) < 0) {
			rl->rt.states[rt_oldstate]++;
			rl->rt.states[rt_newstate]--;
			return (err);
		}
	/* push counts to lk */
	rt->lk.lk->rt.states[rt_oldstate]--;
	rt->lk.lk->rt.states[rt_newstate]++;
	/* What we are more interested in than pushing state here is determining the 
	   route loading of the link, and whether the link is in danger of
	   congestion due to route loading.  That is a difficult thing to calculate, 
	   however.  The link can only be in danger of congestion if the routes
	   assigned to it are actually active. This might be better considered at
	   the route-list/link-set level rather than at the route/link level */
#if 0
	if ((lk_newstate =
	     mtp_cnt_state((struct counters *) (&lk->rt), lk->state)) != lk->state) {
		/* don't know quite what to do with this... */
	}
#endif
	rt->state = rt_newstate;
      no_state_change:
	rt->flags = rt_newflags;
      no_flags_change:
	return (0);
}

/**
 * ls_set_state: - set new combined link set state
 * @q: active queue
 * @ls: combined link set
 * @state: new state
 *
 * A link set state change might also change a route list or signalling point
 * state or both.
 */
static int
ls_set_state(queue_t *q, struct ls *ls, uint state)
{
	struct sp *sp = ls->sp.sp;
	struct rl *rl;
	uint sp_newstate;
	uint rl_newstate;
	uint ls_oldstate = ls->state;
	uint ls_newstate = ls->state;
	uint ls_oldflags = ls->flags;
	uint ls_newflags = ls->flags;
	int err;

	if ((ls_newflags = mtp_sta_flags(state, ls_oldflags)) == ls_oldflags)
		goto no_flags_change;
	if ((ls_newstate = mtp_flg_state(ls_newflags)) == ls_oldstate)
		goto no_state_change;
	/* --------------------------------------------- */
	/* --------------------------------------------- */
	/* push state change to rl */
	if (ls_newstate == SL_INACTIVE)
		rl_newstate = MTP_BLOCKED;
	else
		rl_newstate = MTP_UNBLOCKED;
	for (rl = ls->rl.list; rl; rl = rl->ls.next)
		if (rl_newstate != rl->state)
			if ((err = rl_set_state(q, rl, rl_newstate)) < 0)
				return (err);
	/* --------------------------------------------- */
	/* push state change to sp */
	sp->ls.states[ls_oldstate]--;
	sp->ls.states[ls_newstate]++;
	if ((sp_newstate =
	     mtp_cnt_state((struct counters *) (&sp->ls), sp->state)) != sp->state)
		if ((err = sp_set_state(q, sp, sp_newstate)) < 0) {
			sp->ls.states[ls_oldstate]++;
			sp->ls.states[ls_newstate]--;
			return (err);
		}
	ls->state = ls_newstate;
      no_state_change:
	ls->flags = ls_newflags;
      no_flags_change:
	return (0);
}

/**
 * lk_set_state: - set new link set state
 * @q: active queue
 * @lk: link set
 * @state: new state
 *
 * A link set state change might also change a route or combined link set
 * state or both.
 */
static int
lk_set_state(queue_t *q, struct lk *lk, uint state)
{
	struct ls *ls = lk->ls.ls;
	struct rt *rt;
	uint ls_newstate;
	uint rt_newstate;
	uint lk_oldstate = lk->state;
	uint lk_newstate = lk->state;
	uint lk_oldflags = lk->flags;
	uint lk_newflags = lk->flags;
	int err;

	if ((lk_newflags = mtp_sta_flags(state, lk_oldflags)) == lk_oldflags)
		goto no_flags_change;
	if ((lk_newstate = mtp_flg_state(lk_newflags)) == lk_oldstate)
		goto no_state_change;
	/* --------------------------------------------- */
	/* ANSI T1.111.4/2000 9.3 A signalling point X considers that the MTP of an
	   inaccessible adjacent signalling point Y is restarting when: (1) the
	   first link in a direct link set is in the in service state at level 2, or 
	   (2) a route other than a direct link set becomes available. e.g. through
	   receipt of a transfer allowed or traffic restart allowed message or the
	   availability of the corresponding link set.

	   ...  When a signalling point Y becomes accessible via a route other than
	   a direct link set between X and Y, X sends a MTP-RESUME primitive
	   concerning Y to all local MTP users. In addition, if signalling point X
	   has the transfer function, X sends to Y any required transfer-prohibited
	   and transfer-restricted messages by the available route and broadcasts
	   transfer-allowed or transfer-restricted messages concerning Y. */
	/* --------------------------------------------- */
	if (lk_oldstate > LK_RESTART && lk_newstate <= LK_RESTART) {
		/* The first link in the direct link set to lk->sp.adj is active a
		   level 3. */
		if (lk->sp.adj->state > RS_RESTART) {
			/* If the route set lk->sp.adj is currently inactive, then
			   the adjacent signalling point is restarting and ANSI
			   T1.111.4/2000 9.3 applies. */

			/* ANSI T1.111.4/2000 9.2 ... If the first link in a
			   previously unavailable link set becomes available while
			   T23 or T24 is running, a traffic restart waiting message
			   is sent to the point at the far end of the link. The
			   necessary transfer-prohibited and transfer-restricted
			   messages and a traffic restart allowed message are sent
			   either during the restart procedure or thereafter.
			   Whether user traffic is restarted on such available links 
			   when T24 stops or at some time thereafter is
			   implementation dependent.  If changes in the availability 
			   of links or the reception of signalling route management
			   messages causes the status of a destination to change
			   during T24, it is implementation dependent whether this
			   status change is reflected in the broadcast of
			   transfer-prohibited and transfer-restricted messages
			   during T24 or is handled outside the restart procedure. */

			/* ANSI T1.111.4/2000 9.3 ... When the first link in a
			   direct link set toward the restarting signalling point Y
			   goes into the in services state at level 2, signalling
			   point X begins taking into account any traffic restart
			   waiting, traffic restart allowed, transfer-prohibited,
			   transfer-restricted, and transfer-allowed messages from
			   Y. Signalling point X starts timer T28 either when the
			   first link goes into the in service state at level 2 or
			   when the first signalling link becomes available at level 
			   3. */

			if (!lk_timer_remain(lk, t28a))
				lk_timer_start(q, lk, t28a);

			/* ANSI T1.111.4/2000 9.3 ... In addition it takes the
			   following actions:

			   (1) If a TRW message is received from Y while T28 is
			   running or before it is started, X starts T25.  X stops
			   T28 if it is running.

			   (2) If a TRW message is received from Y while T25 is
			   running, X restarts T25.

			   (3) When the first link in a link set to Y becomes
			   available, signalling point X sends to Y a traffic
			   restart allowed message or, if X has the transfer
			   function, a traffic restart waiting message followed by
			   transfer-prohibited (note that all transfer prohibited
			   messages according to 13.2.2 (1) must be sent) and
			   transfer-restricted messages and a traffic restart
			   allowed message.

			   (4) If a destination becomes prohibited, restricted, or
			   available at X, after X has sent a traffic restart
			   allowed message to Y, X notifies Y of the status change
			   by the normal procedures in Section 13.

			   (5) When a traffic restart allowed message has been sent
			   to Y and a traffic restart allowed message has been
			   received from Y, X stops T25 or T28, whichever is
			   running, and restarts traffic on the link set to Y.  X
			   gives MTP-RESUME primitives to users concerning Y and any 
			   destinations made accessible via Y.  If X has the
			   transfer function it also broadcasts transfer-allowed or
			   transfer-restricted messages concerning the same
			   destinations.

			   (6) If T28 expires, X restarts traffic on the link set to 
			   Y as in (5), unless a traffic restart allowed message has 
			   not been sent to Y.  In that case, X starts T25 and
			   completes the sending of transfer prohibited and
			   transfer-restricted messages, followed by a traffic
			   restart allowed message.  Then, unless a traffic restart
			   waiting message has been received from Y without a
			   subsequent traffic restart allowed message, X stops 25
			   and restarts traffic on the link set to Y.

			   (7) If T25 expires, X restarts traffic on the link set
			   toward Y.  In the abnormal case when X has not completed
			   sending transfer-prohibited and transfer-restricted
			   messages to Y, X completes sending the
			   transfer-prohibited messages required by 13.2.2 (1) and
			   sends a traffic restart allowed message before restarting 
			   user traffic.

			   (8) If no traffic restart allowed message has been
			   received from Y when traffic is restarted to Y, timer T9
			   (see 9.4) is started. */
		}
	}
	/* --------------------------------------------- */
	/* push state change to rt */
	if (lk_newstate == SL_INACTIVE)
		rt_newstate = MTP_BLOCKED;
	else
		rt_newstate = MTP_UNBLOCKED;
	for (rt = lk->rt.list; rt; rt = rt->lk.next)
		if (rt_newstate != rt->state)
			if ((err = rt_set_state(q, rt, rt_newstate)) < 0)
				return (err);
	/* --------------------------------------------- */
	/* push state change to ls */
	ls->lk.states[lk_oldstate]--;
	ls->lk.states[lk_newstate]++;
	if ((ls_newstate =
	     mtp_cnt_state((struct counters *) (&ls->lk), ls->state)) != ls->state)
		if ((err = ls_set_state(q, ls, ls_newstate)) < 0) {
			ls->lk.states[lk_oldstate]++;
			ls->lk.states[lk_newstate]--;
			return (err);
		}
	lk->state = lk_newstate;
      no_state_change:
	lk->flags = lk_newflags;
      no_flags_change:
	return (0);
}

/**
 * sl_set_state: - set new signalling link state
 * @q: active quee
 * @sl: signalling link whose state to change
 * @state: new state
 *
 * A signalling link state change might also change the link set state.
 */
static int
sl_set_state(queue_t *q, struct sl *sl, uint state)
{
	struct lk *lk = sl->lk.lk;
	uint lk_newstate;
	uint sl_oldstate = sl->state;
	uint sl_newstate = sl->state;
	uint sl_oldflags = sl->flags;
	uint sl_newflags = sl->flags;
	int err;

	if ((sl_newflags = mtp_sta_flags(state, sl_oldflags)) == sl_oldflags)
		goto no_flags_change;
	if ((sl_newstate = mtp_flg_state(sl_newflags)) == sl_oldstate)
		goto no_state_change;
	/* --------------------------------------------- */
	if (sl_newstate < SL_UNUSABLE && sl_oldstate > SL_CONGESTED) {
		int active =
		    lk->sl.allowed + lk->sl.danger + lk->sl.congested +
		    lk->sl.restricted + 1;

		/* The link will become usable (active) from unusable (blocked or
		   inactive).  In all cases when the signalling link moves to
		   active, redistribute traffic to the new link as required. */
		sl_changeback_add(q, lk, sl, active);
	}
	if (sl_newstate == SL_INHIBITED && sl_oldstate < SL_UNUSABLE) {
		/* When the link become inhibited from usable, there should already
		   be a changeover buffer that was allocated at the beginning of the 
		   inhibition process.  Setting the state is just the final state
		   change.  Perhaps it should be checked whether there are any
		   changeovers or changebacks to this signalling link and redirect
		   them. */
		mi_strlog(q, 0, SL_TRACE, "sl_set_state() check for cb operations?");
	}
	if (sl_oldstate < SL_UNUSABLE && sl_newstate > SL_CONGESTED) {
		/* The link will become unusable (blocked or inactive) from usable
		   (active or inibited). */

		if (sl_oldstate != SL_INHIBITED) {
			int active =
			    lk->sl.allowed + lk->sl.danger + lk->sl.congested - 1;
			int inhibited = lk->sl.restricted;

			/* The link has moved from active (uninhibbited) to
			   unusable. */
			if (active > 0) {
				/* When the link becomes unusable (blocked or
				   inactive) and there are other active signalling
				   links, offload traffic to those other active
				   signalling links. */
				sl_changeover_sub(q, lk, sl, active);
			} else if (inhibited) {
				/* When the link becomes unusable (blocked or
				   inactive) and there are only inhibited signalling 
				   links available, offload traffic to the inhibited
				   links using forced unihibiting. */
				sl_changeover_res(q, lk, sl, inhibited);
			}
		} else {
			/* When the link moves from inihibited to unusable (blocked
			   or inactive), the link was not carrying traffic while
			   inhibited and no action is required to keep it from
			   carrying further traffic. */
		}
	}
	/* --------------------------------------------- */
	/* push state change to lk */
	lk->sl.states[sl_oldstate]--;
	lk->sl.states[sl_newstate]++;
	if ((lk_newstate =
	     mtp_cnt_state((struct counters *) (&lk->sl), lk->state)) != lk->state)
		if ((err = lk_set_state(q, lk, lk_newstate)) < 0) {
			lk->sl.states[sl_oldstate]++;
			lk->sl.states[sl_newstate]--;
			return (err);
		}
	sl->state = sl_newstate;
      no_state_change:
	sl->flags = sl_newflags;
      no_flags_change:
	return (0);
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
static struct rs *
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
static struct rl *
rl_find(struct rs *rs, int state)
{
	struct rl *rl;

	for (rl = rs->rl.list; rl && rl->state != state; rl = rl->rs.next) ;
	return (rl);
}

/*
 *  Select the route to be used for routing messages in the specified route list.
 */
static struct rt *
rt_find(struct rl *rl, int state)
{
	struct rt *rt;

	for (rt = rl->rt.list; rt && rt->state != state; rt = rt->rl.next) ;
	return (rt);
}
#endif

/**
 * mtp_lookup_rs_local: - lookup a route set (local)
 * @sl: signalling link for the local route set
 * @dest: destination of the route set
 * @type: type of route set
 *
 * Find a local route set via the signalling link with the specified
 * destination and type.
 */
static struct rs *
mtp_lookup_rs_local(struct sl *sl, uint32_t dest, int type)
{
	struct rt *rt;
	struct rl *rl;
	struct rs *rs = NULL;

	for (rt = sl->lk.lk->rt.list; rt; rt = rt->lk.next) {
		if ((rl = rt->rl.rl) && (rs = rl->rs.rs) && rs->dest == dest
		    && rs->rs_type == type)
			break;
		rs = NULL;
	}
	return (rs);
}

/**
 * mtp_lookup_rs: - lookup a route set for a route set-related message
 * @q: active queue
 * @sl: the signalling link the message came in on
 * @m: the decoded message
 * @type: type of destination in message
 *
 * Lookup the route set to which a route set related message pertains with
 * appropriate security screening.  Route set related messages include TFC,
 * UPU, UPA, and UPT.
 *
 * Route set related message which arrive on link sets upon which we have no
 * existing route to the specified destination are suspect and should be
 * ignored.  Several DoS attacks are possible without this screening.
 */
static struct rs *
mtp_lookup_rs(queue_t *q, struct sl *sl, struct mtp_msg *m, uint type)
{
	struct rs *rs;
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;

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
			break;
	if (!rs)
		goto error7;
	return (rs);
      error1:
	mi_strlog(q, 0, SL_ERROR,
		  "Route set message: no local route to destination");
	goto error;
      error2:
	mi_strlog(q, 0, SL_ERROR, "Route set message: no local route to originator");
	goto error;
      error3:
	mi_strlog(q, 0, SL_ERROR, "Route set message: originator non-STP");
	goto error;
      error7:
	mi_strlog(q, 0, SL_ERROR, "Route set message: no route to destination");
	goto error;
      error:
	todo(("Deliver screened message to MTP management\n"));
	return (NULL);
}

/**
 * mtp_lookup_rt_local: lookup a route (local)
 * @sl: signalling link message came in on
 * @dest: destination of the route
 * @type: type of the route
 * 
 * Find a local route via the signalling link with the specified destination
 * and type.
 */
static struct rt *
mtp_lookup_rt_local(struct sl *sl, uint32_t dest, int type)
{
	struct rt *rt;
	struct rl *rl;
	struct rs *rs;

	for (rt = sl->lk.lk->rt.list; rt; rt = rt->lk.next)
		if ((rl = rt->rl.rl) && (rs = rl->rs.rs) && rs->dest == dest
		    && rs->rs_type == type)
			break;
	return (rt);
}

/**
 * mtp_lookup_rt_test: - lookup route for a route test message
 * @sl: the signalling link the message came in on
 * @m: the decoded message
 * @type: type of destination in message
 *
 * Lookup the route to which a route related test message pertains with
 * appropriate screening.  Route related test messages include RST, RSR, RCP,
 * and RCR.
 */
static struct rt *
mtp_lookup_rt_test(queue_t *q, struct sl *sl, struct mtp_msg *m, uint type)
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
			    &&
			    ((!(rs->flags & RSF_CLUSTER) && type == RT_TYPE_MEMBER)
			     || ((rs->flags & RSF_CLUSTER)
				 && type == RT_TYPE_CLUSTER)))
				for (rt = lk->rt.list; rt; rt = rt->lk.next)
					if ((rs = rt->rl.rl->rs.rs)
					    && rs->dest == m->dest
					    && rs->rs_type == type)
						goto found;
      found:
	if (rt)
		return (rt);
	goto error7;
      error1:
	/* If we are not equipped with the transfer function, we never expect to
	   receive route related test messages. Can be an attempt to probe screening 
	   tables. */
	mi_strlog(q, 0, SL_ERROR, "Route set test message: local is non-STP");
	goto error;
      error2:
	/* If the message is a cluster message and we do not support cluster
	   routing, it is an obvious error. Can be an attempt to probe screening
	   tables. */
	mi_strlog(q, 0, SL_ERROR, "Route set test message: no cluster support");
	goto error;
      error3:
	/* If we do not have a route set for the originator of the message, then it
	   is another obvious error. The true originator might be spoofing the
	   originating point code. */
	mi_strlog(q, 0, SL_ERROR,
		  "Route set test message: no local route to originator");
	goto error;
      error4:
	/* If the message is not sent by a known adjacent signalling point, then it
	   is another obvious error. The originator may be attempting to bounce test 
	   messages around.  */
	mi_strlog(q, 0, SL_ERROR, "Route set test message: originator not adjacent");
	goto error;
      error5:
	/* If the message is testing the local point code, then it is an error.
	   Adjacent point codes never test the adjacent point code, it is assumed
	   available when there is a signalling link available to it. */
	mi_strlog(q, 0, SL_ERROR, "Route set test message: testing local adjacent");
	goto error;
      error6:
	mi_strlog(q, 0, SL_ERROR,
		  "Route set test message: no local route to destination");
	goto error;
      error7:
	mi_strlog(q, 0, SL_ERROR, "Route set test message: no route to destination");
	goto error;
      error:
	todo(("Deliver screened message to MTP management\n"));
	return (NULL);
}

/**
 * mtp_lookup_rt: - lookup route for a route-related message
 * @q: active queue
 * @sl: the signalling link the message came in on
 * @m: the message
 * @type: type of destination in message
 *
 * Lookup the route to which a route related message pertains with appropriate
 * security screening.  Route related messages include TFA, TFR, TFP, TCA,
 * TCR, TCP.
 *
 * The following characteristics of route-related (TFA, TFR, TFP, TCA, TCR,
 * TCP) messages are enforced when security is set against the local or
 * adjacent signalling point:
 *
 * 01. route-related messages are sent only from a known adjacent signalling point
 * 02. route-related messages must have the local (non-alias) signalling point code as the DPC
 * 03. route-related messages must have the adjacent (non-alias) signalling point code as the OPC
 * 04. route-related messages are only sent by a signalling point with the transfer function (i.e an STP)
 * 05. route-related messages are only relayed by a signalling point with the transfer function (i.e an STP)
 * 06. route-related messages can only affect clusters if the local signalling point supports clusters
 * 07. route-related messages can only affect clusters if the adjacent signalling point supports clusters
 * 08. route-related messages can only affect clusters if the relaying signalling point supports clusters.
 * 09. route-related messages are never affecting the cluster containing the local signalling point
 * 10. route-related messages are never affecting the cluster containing the adjacent signalling point 
 * 11. route-related messages are never affecting the cluster containing the relaying signalling point 
 * 12. route-related messages are never affecting the local signalling point code
 * 13. route-related messages are never affecting the adjacent signalling point code
 * 14. route-related messages will not arrive on a link set with no local route to the originator
 * 15. route-related messages will not arrive on a link set with no route to the affected destination/cluster
 * 16. route-related messages will not affect a destination/cluster with no route on the direct link set to originator
 *
 */
static struct rt *
mtp_lookup_rt(queue_t *q, struct sl *sl, struct mtp_msg *m, uint type)
{
	struct sp *loc = sl->lk.lk->sp.loc;
	struct rs *rel = sl->lk.lk->sp.adj;
	struct rs *adj;
	struct rs *dst;
	struct ls *ls;
	struct lk *lk, *direct;
	struct rl *rl;
	struct rt *rt = NULL;

	if ((loc->flags & SPF_SECURITY) || (rel->flags & RSF_SECURITY)) {
		/* 01. route-related messages are sent only from a known adjacent
		   signalling point */
		if (!(rel->flags & RSF_ADJACENT))
			goto error01;
		/* 02. route-related messages must have the local (non-alias)
		   signalling point code as the DPC */
		if (loc->pc != m->dpc)
			goto error02;
		/* 03. route-related messages must have the adjacent (non-alias)
		   signalling point code as the OPC */
		if ((adj = rel)->dest != m->opc) {
			for (adj = loc->rs.list; adj; adj = adj->sp.next)
				if (adj->rs_type == RT_TYPE_MEMBER
				    && adj->dest == m->opc)
					break;
			if (!adj)
				goto error03;
			if (!(adj->flags & RSF_ADJACENT))
				goto error03;
		}
		/* 04. route-related messages are only sent by a signalling point
		   with the transfer function (i.e an STP) */
		if (!(adj->flags & RSF_XFER_FUNC))
			goto error04;
		/* 05. route-related messages are only relayed by a signalling point 
		   with the transfer function (i.e an STP) */
		if (!(rel->flags & RSF_XFER_FUNC))
			goto error05;
		if (type == RT_TYPE_CLUSTER) {
			/* 06. route-related messages can only affect clusters if
			   the local signalling point supports clusters */
			if (!(loc->flags & SPF_CLUSTER))
				goto error06;
			/* 07. route-related messages can only affect clusters if
			   the adjacent signalling point supports clusters */
			if (!(adj->flags & RSF_CLUSTER))
				goto error07;
			/* 08. route-related messages can only affect clusters if
			   the relaying signalling point supports clusters. */
			if (!(rel->flags & RSF_CLUSTER))
				goto error08;
			/* 09. route-related messages are never affecting the
			   cluster containing the local signalling point */
			if ((m->dest == (loc->pc & loc->na.na->mask.cluster)))
				goto error09;
			/* 10. route-related messages are never affecting the
			   cluster containing the adjacent signalling point */
			if ((m->dest == (adj->dest & loc->na.na->mask.cluster)))
				goto error10;
			/* 11. route-related messages are never affecting the
			   cluster containing the relaying signalling point */
			if ((m->dest == (rel->dest & loc->na.na->mask.cluster)))
				goto error11;
		} else {
			/* 12. route-related messages are never affecting the local
			   signalling point code */
			if (m->dest == loc->pc)
				goto error12;
			/* 13. route-related messages are never affecting the
			   adjacent signalling point code */
			if (m->dest == adj->dest)
				goto error13;
		}
		/* 14. route-related messages will not arrive on a link set with no
		   local route to the originator */
		for (rt = lk->rl.list; rt && (rt->rl.rl->rs.rs != adj);
		     rt = rt->lk.next) ;
		if (!rt)
			goto error14;
		/* 15. route-related messages will not arrive on a link set with no
		   route to the affected destination/cluster */
		for (dst = NULL, rt = lk->rl.list; rt; rt = rt->lk.next)
			if ((dst = rt->rl.rl->rs.rs)->rs_type == type
			    && dst->dest == m->dest)
				break;
		if (!dst)
			goto error15;
		/* 16. route-related messages will not affect a destination/cluster
		   with no route on the direct link set to originator */
		for (rt = NULL, rl = dst->rl.list; rl; rl = rl->rs.next)
			for (rt = rl->rt.list; rt; rt = rt->rl.next)
				if (rt->lk.lk->sp.adj == adj)
					break;
		if (!rt)
			goto error16;
		return (rt);
	}
	for (rt == NULL, ls = loc->ls.list; ls; ls = ls->sp.next) {
		for (lk = ls->lk.list; lk; lk == lk->ls.next) {
			if (lk->sp.adj->dest != m->opc || lk->sp.loc->pc != m->dpc)
				continue;
			for (rt = lk->rt.list; rt; rt = rt->lk.next)
				if (rt->rl.rl->rs.rs->dest == m->dest
				    && rt->rl.rl->rs.rs->rs_type == type)
					break;
			break;
		}
	}
	if (!rt)
		goto error16;
	return (rt);
      error01:
	mi_strlog(q, 0, SL_ERROR, "Route message: non-adjacent SP");
	goto error;
      error02:
	mi_strlog(q, 0, SL_ERROR, "Route message: not addressed for local SP");
	goto error;
      error03:
	mi_strlog(q, 0, SL_ERROR, "Route message: not addressed from adjacent SP");
	goto error;
      error04:
	mi_strlog(q, 0, SL_ERROR, "Route message: addressed from non-STP");
	goto error;
      error05:
	mi_strlog(q, 0, SL_ERROR, "Route message: relayed by non-STP");
	goto error;
      error06:
	mi_strlog(q, 0, SL_ERROR, "Route message: to non-cluster SP");
	goto error;
      error07:
	mi_strlog(q, 0, SL_ERROR, "Route message: from non-cluster adjacent STP");
	goto error;
      error08:
	mi_strlog(q, 0, SL_ERROR, "Route message: from non-cluster relaying STP");
	goto error;
      error09:
	mi_strlog(q, 0, SL_ERROR, "Route message: affects local cluster");
	goto error;
      error10:
	mi_strlog(q, 0, SL_ERROR, "Route message: affects adjacent cluster");
	goto error;
      error11:
	mi_strlog(q, 0, SL_ERROR, "Route message: affects relaying cluster");
	goto error;
      error12:
	mi_strlog(q, 0, SL_ERROR, "Route message: affects local SP");
	goto error;
      error13:
	mi_strlog(q, 0, SL_ERROR, "Route message: affects adjacent SP");
	goto error;
      error14:
	mi_strlog(q, 0, SL_ERROR,
		  "Route message: no route to originator through relay");
	goto error;
      error15:
	mi_strlog(q, 0, SL_ERROR,
		  "Route message: no route to adjacent through relay");
	goto error;
      error16:
	mi_strlog(q, 0, SL_ERROR, "Route message: no route to affected SP");
	goto error;
      error:
	todo(("Deliver screened message to MTP management\n"));
	return (NULL);
}

/**
 * mtp_lookup_adj: - lookup adjacent route for an adjacent signalling point related message
 * @q: active queue (lower read queue)
 * @sl: the signalling link the message came in on
 * @m: the decoded message
 *
 * Lookup the route to which an adjacent signalling point related message
 * pertains with appropriate security screening.  Adjacent signalling point
 * related messages include: TRA, TRW.
 *
 * The following characteristics of adjacent signalling point related messages
 * (TRA, TRW) are enforced when security screening is set against the local or
 * adjacent signalling point:
 *
 * 1. TRA and TRW are always sent by the adjacent signalling point on a direct link set
 * 2. TRA and TRW must have the adjacent signalling point code as the OPC
 * 3. TRA and TRW must have the local (non-alias) signalling point code as the DPC
 */
static struct rt *
mtp_lookup_adj(queue_t *q, struct sl *sl, struct mtp_msg *m)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	struct rl *rl;
	struct rt *rt;

	if ((loc->flags & SPF_SECURITY) || (adj->flags & RSF_SECURITY)) {
		/* 1. TRA and TRW are always sent by the adjacent signalling point
		   on a direct link set */
		if (!(adj->flags & RSC_ADJACENT))
			goto error1;
		/* 2. TRA and TRW must have the adjacent signalling point code as
		   the OPC */
		if (adj->dest != m->opc)
			goto error2;
		/* 3. TRA and TRW must have the local (non-alias) signalling point
		   code as the DPC */
		if (loc->pc != m->dpc)
			goto error3;
	}
	/* find the route for the adjacent signalling point leading to the link set
	   from which the message arrived */
	for (rl = adj->rl.list; rl; rl = rl->rs.next)
		for (rt = rl->rt.list; rt && rt->lk.lk != lk; rt = rt->rl.next) ;
	if (!rt)
		goto error4;
	return (rt);
      error1:
	mi_strlog(q, 0, SL_ERROR, "Adjacent SP message: from non-adjacent SP");
	goto error;
      error2:
	mi_strlog(q, 0, SL_ERROR,
		  "Adjacent SP message: not addressed from adjacent SP");
	goto error;
      error3:
	mi_strlog(q, 0, SL_ERROR, "Adjacent SP message: not addressed to local SP");
	goto error;
      error4:
	mi_strlog(q, 0, SL_ERROR, "Adjacent SP message: no route to adjacent");
	goto error;
      error:
	todo(("Deliver screened message to MTP management."));
	return (NULL);
}

/**
 * mtp_lookup_sl: - lookup signalling-link related message
 * @q: active queue
 * @sl: the signalling link the message came in on
 * @m: the message
 *
 * Lookup the signalling link to which a signalling link related message pertains.
 */
static struct sl *
mtp_lookup_sl(queue_t *q, struct sl *sl, struct mtp_msg *m)
{
	struct sp *sp = sl->lk.lk->sp.loc;
	struct ls *ls;
	struct lk *lk;
	struct sl *s2 = NULL;

	if (sp->flags & SPF_SECURITY)
		if (!mtp_lookup_rt_local(sl, m->opc, RT_TYPE_MEMBER)
		    && !mtp_lookup_rt_local(sl, m->opc & sp->na.na->mask.cluster,
					    RT_TYPE_CLUSTER))
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
	/* Signalling link related messages which arrive on link sets upon which we
	   have no existing route to the adjacent signalling point are suspect and
	   should be ignored. Several DoS attacks are possible without this
	   screening. */
	mi_strlog(q, 0, SL_ERROR,
		  "Signallingl link message: no local signalling link");
	goto error;
      error:
	todo(("Deliver screened message to MTP management\n"));
	return (NULL);
}

/*
 * -------------------------------------------------------------------------
 *
 * Signalling Point Functions
 *
 * -------------------------------------------------------------------------
 * 3.6 Status of signalling points.  A signalling point can be in two states:
 * available and unavailable (see Figure 6).  However, implementation
 * congestion states may exist.
 *
 * 3.6.1 Signalling point unavailability
 *
 * 3.6.1.1 Unavailability of a signalling point itself.  A signalling point
 * becomes unavailable when all connected signalling links are unavailable.
 *
 * 3.6.1.2 Unavailability of an adjacent signalling point.  A signalling point
 * considers that an adjacent signalling point becomes unavailable when: all
 * signalling links connected to the adjacent signalling point are
 * unavailable; and, the adjacent signalling point is inaccessible.
 *
 * 3.6.2 Signalling point availability
 *
 * 3.6.2.1 Availability of a signalling point itself.  A signalling point
 * becomes available when at least one signalling link connected to this
 * signalling point becomes available.
 *
 * 3.6.2.2 Availability of an adjacent signalling point.  A signalling point
 * considers that an adjacent signalling point Y becomes available when: 1) at
 * least one signalling link connected to Y becomes available at level 3 and
 * the MTP restart procedure (see clause 9) has been completed; or, 2) the
 * adjacent signalling point Y becomes accessible: on the receipt of a
 * transfer allowed message or a transfer restricted message; if an
 * alternative route becomes available again via the corresponding local link
 * set; or, if a traffic restart allowed message from another adjacent
 * signalling point Z, whose MTP is restarting, is received so that a route
 * toward Y using Z becomes available.
 *
 * 3.7 Procedures used in connection with point status changes.
 *
 * 3.7.1 Signalling point unavailable.
 *
 * There is no specific procedures used when a signalling point becomes
 * unavailable.  The transfer prohibited procedure is used to update the
 * status of the recovered routes in all nodes of the signalling network (see
 * 13.2).
 *
 * 3.7.2. Signalling point available.
 *
 * 3.7.2.1 Signalling traffic management: the MTP restart procedures (see
 * clause 9) is applied; it is used to restart the traffic between the
 * signalling network and the signalling point which becomes available.  This
 * restart is based on the following criteria: avoid loss of messages; limit
 * the level 3 load due to the restart of a signalling point's MTP; restart,
 * as much as possible, simultaneously in both directions of the signalling
 * relations.
 *
 * 3.7.2.2 Signalling link management: The first step of the MTP restart
 * procedure consists of updating the signalling route states before carrying
 * traffic to the point which becomes available and in all adjacent points;
 * the transfer prohibited and transfer restricted procedures are used (see
 * clause 13).
 *
 * 3.7.3 Signalling point congested: (implementation-dependent option, see
 * 11.2.6).
 *
 * 4.8 Signalling point availability
 *
 * When a previously unavailable signalling point becomes available (see 3.6),
 * signalling traffic may be transferred to the available point by means of
 * the MTP restart procedure (see clause 9).
 */

/*
 * -------------------------------------------------------------------------
 *
 * Route Functions
 *
 * -------------------------------------------------------------------------
 *
 * 3.4 Status of signalling routes.  A signalling route can be in three states
 * for signalling traffic having the concerned destination; these are
 * available, restricted, unavailable (see also Figure 6).
 *
 * 3.4.1 Signalling route unavailability.  A signalling route becomes
 * unavailable when a transfer-prohibited message, indicating that signalling
 * traffic toward a particular destination cannot be transferred via the
 * signalling transfer point sending the concerned message is received (see
 * clause 13).
 *
 * 3.4.2 Signalling route availability.  A signalling route becomes available
 * when a transfer-allowed message, indicating that signalling traffic toward
 * a particular destination can be transferred via the signalling transfer
 * point sending the concerned message, is received (see clause 13).
 *
 * 3.4.3 Signalling route restricted.  A signalling route becomes restricted
 * when a transfer-restricted message, indicating that the signalling traffic
 * toward a particular destination is being transferred with some difficulty
 * via the signalling transfer point sending the concerned message is received
 * (see clause 13).
 *
 * 3.5 Procedures used in connection with route status changes.  In this
 * sub-clause, the procedures related to each signalling management function,
 * which in general are applied in connection with route status changes, are
 * listed.  See also Figures 6 and 8.  Typical examples of the application of
 * the procedures to particular network cases appear in Recommendation Q.705.
 *
 * 3.5.1 Signalling route unavailable
 *
 * 3.5.1.1  Signalling traffic management: the forced rerouting procedure (see
 * clause 7) is applied; it is used to transfer signalling traffic to the
 * concerned destination from the link set, belonging to the unavailable
 * route, to an alternative link set which terminates in another signalling
 * transfer point.  It includes actions to determine the alternative route.
 *
 * 3.5.1.2  Signalling route management: because of the unavailability of the
 * signalling route, the network is reconfigured; in the case that a
 * signalling transfer point can no longer route the concerned signalling
 * traffic, it applies the procedures described in clause 13.
 *
 * 3.5.2 Signalling route available
 *
 * 3.5.2.1 Signalling traffic management: the controlled rerouting procedure
 * (see clause 8) is applied; it is used to transfer signalling traffic to the
 * concerned destination from a signalling link or link set belonging to an
 * available route, to another link set which terminates in another signalling
 * transfer point.  It includes the determination of which traffic should be
 * diverted and procedure for maintaining the correct message sequence.
 *
 * 3.5.2.2 Signalling route management: because of the restored availability
 * of the signalling route, the network is reconfigured; in the case that a
 * signalling transfer point can once again route the concerned signalling
 * traffic, it applies the procedures described in clause 13.
 *
 * 3.5.3 Signalling route restricted
 *
 * 3.5.3.1 Signalling traffic management: the controlled rerouting procedure
 * (see clause 8) is applied; it is used to transfer signalling traffic to the
 * concerned destination from the link set belonging to the restricted route,
 * to an alternative link set if one is available to give more, if possible,
 * efficient routing.  It includes actions to determine the alternative route.
 *
 * 3.5.3.2 Signalling route management: because of restricted availability of
 * the signalling route, the network routine is, if possible, reconfigured;
 * procedure described in clause 13 are used to advise adjacent signalling
 * points.
 *
 * 4.5 Signalling route unavailability
 *
 * When a signalling route becomes unavailable (see 3.4), signalling traffic
 * currently carried by the unavailable route is transferred to an alternative
 * route by means of forced re-routing procedure.  The alternative route (i.e,
 * the alternative link set or link sets) is determined in accordance with the
 * alternative routing defined for the concerned destination (see 4.3.3).
 *
 * 4.6 Signalling route availability
 *
 * When a previously unavailable signalling route becomes available again (see
 * 3.4), signalling traffic may be transferred to the available route by means
 * of a controlled rerouting procedure.  This is applicable in the case when
 * the available route (link set) has higher priority than the route (link
 * set) currently used for traffic to the concerned destination (see 4.4.3).
 *
 * The transferred traffic is distributed over the links of the new link set
 * in accordance with the routing currently applicable for that link set.
 *
 * 4.7 Signalling route restriction
 *
 * When a signalling route becomes restricted (see 3.4), signalling carried by
 * the restricted route is, if possible, transferred to an alternative route
 * by means of the controlled rerouting procedure, if an equal priority
 * alternative route is available and not restricted.  The alternative route
 * is determined in accordance with the alternate routing defined for the
 * concerned destination (see 4.3.3).
 */

/*
 * -------------------------------------------------------------------------
 *
 * Signalling Link Functions
 *
 * -------------------------------------------------------------------------
 * ANSI T1.111.4-2000 ...
 *
 * 3.2 Status of Signalling Links
 * 
 * 3.2.1 A signalling link is always considered by level 3 in one of two
 * possible major states: available and unavailable.  Depending on the cause
 * of unavailability, the unavailable state can be subdivided into seven
 * possible cases as follows (see also Figure 6/T1.111.4):
 *
 *  (1) Unavailable, failed or inactive
 *  (2) Unavailable, blocked
 *  (3) Unavailable, (failed or inactive) and blocked
 *  (4) Unavailable, inhibited
 *  (5) Unavailable, inhibited and (failed or inactive)
 *  (6) Unavailable, inhibited and blocked
 *  (7) Unavailable, (failed or inactive), blocked, and inhibited
 *
 * The concerned link can be used to carry signalling traffic only if it is
 * available except test and management messages (e.g., SLT, TFx, TRA and
 * TRW).  Eight possible events can change the status of a link: signalling
 * link failure, restoration, deactivation, activation, blocking, unblocking,
 * inhibiting, and uninhibiting: they are described in 3.2.2 through 3.2.9.
 * 
 * 3.2.2 Signalling Link Failure.  A signalling link (in-service or blocked,
 * see 3.2.6) is recognized by level 3 as failed when:
 *
 * (1) A link failure indication is obtained from level 2.
 *
 *      For MTP Level 2 links, the indication may be caused by:
 *
 *      (a) Intolerably high signal unit error  rate (see Section 10 of
 *          Chapter T1.111.3)
 *
 *      (b) Excessive length of the realignment period (see 4.1 and Section 7
 *          of Chapter T1.111.3)
 *
 *      (c) Excessive delay of acknowledgements (see 5.3 and 6.3 of Chapter
 *          T1.111.3)
 *
 *      (d) Excessive period of level 2 congestion (se e Section 9 of Chapter
 *          T1.111.3)
 *
 *      (e) Failure of signalling terminal equipment
 *
 *      (f) Two out of three unreasonable backward sequence numbers or forward
 *          indicator bits (see 5.3and 6.3 of Chapter T1.111.3)
 *
 *      (g) Reception of consecutive link status signal units  indicating
 *          out-of-alignment, out-of-service,normal or emergency terminal
 *          status (see 1.7 of Chapter T1.111.3) The first two conditions are
 *          detected by the signal unit error rate monitor (see Section 10 of
 *          ChapterT1.111.3).
 *
 *      For SAAL links, failure indications include:
 *
 *      (a) Intolerably high protocol data unit error rate (see Section 8.1.1
 *          of T1.652 [7])
 *
 *      (b) Excessive length of the realignment period (see Section 6.2.2 of
 *          T1.645 [8])
 *
 *      (c) No receipt of a status (STAT) protocol data unit - timer
 *          No_Response expiration (see Section 7.6 of T1.637 [2])
 *
 *      (d) Excessive period of no credit (see Section 8.1.2 of T1.652)
 *
 *      (e) Local or remote processor outage condition (see Section 6.2.1 of
 *          T1.645)
 *
 * (2) A request (automatic or manual) is obtained from a management or
 *     maintenance system.
 *
 * (3) The signalling link test fails (see Section 2.2 of Chapter T1.111.7).
 *
 * (4) False link congestion is detected (see 3.8.2.2).  Moreover, a
 *     signalling link that is available (not blocked) is recognized by level
 *     3 as failed when a changeover order is received.
 *
 * 3.2.3 Signalling Link Restoration.   A previously failed signalling link is
 * restored when both ends of the signalling link have successfully completed
 * an initial alignment procedure (see Section 7 of Chapter T1.111.3 or
 * Section 6.2.2 of T1.645).
 *
 * 3.2.4 Signalling Link Deactivation.  A signalling link (in-service, failed
 * or blocked) is recognized by level 3 as deactivated (i.e., removed from
 * operation) when:
 *
 * (1) A request is obtained from the signalling link management f unction
 *     (see Section 12)
 *
 * (2) A request (automatic or manual) is obtained from an external management
 *     or maintenance system
 *
 * 3.2.5 Signalling Link Activation.  A previously inactive signalling link is
 * recognized by level 3 as activated when both ends of the signalling link
 * have successfully completed an initial alignment procedure (see Section 7
 * of Chapter T1.111.3 or Section 6.2.2 of T1.645).
 *
 * 3.2.6 Signalling Link Blocking.  A signalling link (in service, failed, or
 * inactive) is recognized as blocked when an indication is obtained from the
 * signalling terminal that a processor outage condition exists at the remote
 * terminal (i.e., link status signal units with processor outage indication
 * are received, see Section 8 of Chapter T1.111.3).
 *
 * NOTE: A link becomes unavailable when it is failed or deactivated or
 * ((failed or deactivated) and blocked) (see Figure 6/T1.111.4).
 *
 * NOTE:  For SAAL links, a processor outage condition causes the link to
 * fail.
 *
 * 3.2.7 Signalling Link Unblocking.  A signalling link previously blocked is
 * unblocked when an indication is obtained from the signalling terminal that
 * the processor outage condition has ceased at the remote terminal.  (Applies
 * in the case when the processor outage condition was initiated by the remote
 * terminal.).
 *
 * NOTE: A link becomes available when it is restored, or activated, or
 * unblocked, or ((restored or activated) and unblocked) (see Figure
 * 6/T1.111.4).
 *
 * 3.2.8 Signalling Link Inhibiting.  In U. S. networks the standard for
 * management control of signalling links is the management inhibit procedure.
 * A signalling link is recognized as inhibited when:
 *
 * (1) An acknowledgement is received from a remote signalling point in
 *     response to an inhibit request sent to the remote end by the local
 *     signalling link management.  Level 3 has marked the link locally
 *     inhibited.
 *
 * (2) Upon receipt of a request from a remote signalling point to inhibit a
 *     link and successful determination that no destination will become
 *     inaccessible by inhibiting the link, the link has been marked remotely
 *     inhibited by Level 3.
 *  
 * 3.2.9 Signalling Link Uninhibiting.  A signalling link previously inhibited
 * is uninhibited when:
 *
 * (1) A request is received to uninhibit the link from a remote end or from a
 *     local routing function.
 *
 * (2) An acknowledgement is received from  a remote signalling point in
 *     response to an uninhibit request sent to the remote end by the local
 *     signalling link management.
 *
 * 3.3 Procedures Used in Connection with Link Status Changes.  In 3.3, the
 * procedures relating to each signalling management function, which are
 * applied in connection with link status changes, are listed. (See also
 * Figures 6-8/T1.111.4.) Typical examples of the application of the
 * procedures to the particular network cases appear in Chapter T1.111.5.
 *
 * 3.3.1 Signalling Link Failed
 *
 * 3.3.1.1 Signalling Traffic Management.  The changeover procedure (see
 * Section 5) is applied, if required, to divert signalling traffic from the
 * unavailable link to one or more alternative links with the objective of
 * avoiding message loss, repetition or mis-sequencing; it includes
 * determination of the alternative link or links to which the affected
 * traffic can be transferred, and procedures to retrieve messages sent over
 * the failed link but not received by the far end.
 *
 * 3.3.1.2 Signalling Link Management.  The procedures described in Section 12
 * are used to restore a signalling link and to make it available for
 * signalling.  Moreover, depending on the link set status, the procedures can
 * also be used to activate another signalling link in the same link set to
 * which the unavailable link belongs, and to make it available for
 * signalling.  
 * 
 * 3.3.1.3 Signalling Route Management.  In the case when the failure of a
 * signalling link causes a signalling route set to become unavailable or
 * restricted, the signalling transfer point that can no longer route the
 * concerned signalling traffic applies the transfer-prohibited or
 * transfer-restricted procedures described in Section 13.  
 *
 *  3.3.2 Signalling Link Restored
 *
 * 3.3.2.1 Signalling Traffic Management.  The changeback procedure (see
 * Section 6) is applied, if required, to divert signalling traffic from one
 * or more links to a link which has become available; it includes
 * determination of the traffic to be diverted and procedures for maintaining
 * the correct message sequence.
 *
 * 3.3.2.2 Signalling Link Management.  The signalling link deactivation
 * procedure (see Section 12) is used if, during the signalling link failure,
 * another signalling link of the same link set was activated; it is used to
 * assure that the link set status is returned to the same state as before the
 * failure.  This requires that the active link activated during the link
 * failure be deactivated and considered no longer available for signalling.  
 *
 * 3.3.2.3 Signalling Route Management.  In the case when the restoration of a
 * signalling link causes a signalling route set to become available, the
 * signalling transfer point which can once again route the concerned
 * signalling traffic applies the transfer-allowed or transfer-restricted
 * procedures described in Section 13 as appropriate.  When a "danger of
 * congestion" situation subsides, which had restricted a normal route to a
 * destination, the status of the route is changed to available, but the
 * transfer-allowed procedure (broadcast method) is not invoked
 * (transfer-allowed messages are sent according to Section 13.5.4).  
 *
 * 3.3.3 Signalling Link Deactivated
 *
 * 3.3.3.1 Signalling Traffic Management.    As specified in 3.3.1.1.
 *
 * NOTE: The signalling traffic has normally already been removed when
 * signalling link deactivation is initiated.
 *
 * 3.3.3.2 Signalling Link Management.  If the number of active signalling
 * links in the link set to which the deactivated signalling link belongs has
 * become less than the normal number of active signalling links in that link
 * set, the procedures described in Section 12 may be used to activate another
 * signalling link in the link set.  
 *
 * 3.3.3.3 Signalling Route Management.  As specified in 3.3.1.3.
 *
 * 3.3.4 Signalling Link Activated
 *
 * 3.3.4.1 Signalling Traffic Management. As specified in 3.3.2.1.
 *
 * 3.3.4.2 Signalling Link Management.  If the number of active signalling
 * links in the link set to which the activated signalling link belongs has
 * become greater than the normal number of active signalling links in that
 * link set, the procedures described in Section 12 may be used to deactivate
 * another signalling link in the link set.
 *
 * 3.3.4.3 Signalling Route Management.  As specified in 3.3.2.3.
 *
 * 3.3.5 Signalling Link Blocked
 *
 * 3.3.5.1 Signalling Traffic Management.10  As specified in 3.3.1.1.
 *
 * 3.3.5.2 Signalling Route Management.  If the blocking of the link causes a
 * signalling route set to become unavailable or restricted, the signalling
 * transfer point which can no longer route the concerned signalling traffic
 * applies the transfer-prohibited or transfer-restricted procedures described
 * in Section 13.
 *
 * 3.3.6 Signalling Link Unblocked
 *
 * 3.3.6.1 Signalling Traffic Management.  As specified in 3.3.2.1.
 *
 * 3.3.6.2 Signalling Route Management.  If the link unblocked causes a
 * signalling route set to become available, the signalling transfer point
 * which can once again route the signalling traffic in that route set applies
 * the transfer-allowed or transfer-restricted procedures described in Section
 * 13, as appropriate.  10CCITT Blue Book text specifying an option to use
 * local processor outage to stabilize the link state during traffic
 * management actions has been deleted from this section.
 *
 * 3.3.7 Signalling Link Inhibited
 *
 * 3.3.7.1 Signalling Traffic Management.  As specified in 3.3.1.1.
 *
 * 3.3.7.2 Signalling Link Management.  As specified in 3.3.3.2.
 *
 * 3.3.8 Signalling Link Uninhibited
 *
 * 3.3.8.1 Signalling Traffic Management.  As specified in 3.3.2.1.
 *
 * 3.3.8.2 Signalling Link Management.  As specified in 3.3.4.2.
 *
 * 3.3.8.3 Signalling Route Management.  If the link uninhibited causes a
 * signalling route set to become available, the Signalling Transfer Point,
 * which can once again route the signalling traffic in that route set,
 * applies the transfer-allowed or transfer-restricted procedures described in
 * Section 13, as appropriate. 3.4 Status of Signalling Routes.  A signalling
 * route can be in three states for signalling traffic having the concerned
 * destination; these are available, restricted, and unavailable.  A route may
 * become available or unavailable because of changes in the availability of
 * the local links in that route ; however, the procedures already given in
 * 3.3 handle such cases.  Subsections 3.4 and 3.5 deal with route status
 * changes caused by receipt of signalling route management messages.
 */
static int mtp_dec_msg(queue_t *q, mblk_t *mp, struct mtp_msg *m, struct na *na);

static int
sl_stop_restore(queue_t *q, struct sl *sl)
{
	int err;

	switch (sl_get_l_state(sl)) {
	case SLS_WIND_INSI:
		if ((err = sl_stop_req(q, sl)))
			goto error;
		if ((err = sl_set_state(q, sl, SL_FAILED)) < 0)
			goto error;
		/* T19(ANSI) is a maintenance guard timer */
		sl_timer_stop(sl, t19a);
		/* wait for T17 before attempting restoration again */
		sl_timer_start(q, sl, t17);
		/* If we are waiting for in service then we do not change the
		   routing state of the link. Also, we do not need to perform
		   retrieval because we have not sent anything on the link (other
		   than SLTM). */
		return (0);
	case SLS_WCON_RET:
	{
		struct lk *lk = sl->lk.lk;
		struct ls *ls = lk->ls.ls;
		struct sp *sp = ls->sp.sp;
		struct na *na = sp->na.na;
		struct cb *cb;

		/* We are already retrieving messages on the link */
		/* find all the changeover buffers */
		for (cb = lk->cb.list; cb; cb = cb->lk.next) {
			/* only consider changeover buffers that changeover from the 
			   failed link */
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
						/* unlink from retrieval buffer and
						   add to head of changeover buffer */
						fixme(("See if we have the correct sls\n"));
						bp = bufq_unlink(&sl->rbuf, bp);
						bufq_queue_head(&cb->buf, bp);
						continue;
					}
					/* discard messages that can't be decoded */
					swerr();
					freemsg(bufq_unlink(&sl->rbuf, bp));
					continue;
				}
				/* release the changeback buffer */
				cb_divert_buffer(cb);
			}
		}
		/* purge any remaining unclaimed messages */
		bufq_purge(&sl->rbuf);
		if ((err = sl_set_state(q, sl, SL_UPDATED)) < 0)
			goto error;
		/* fall through */
	}
	case SLS_PROC_OUTG:
	case SLS_IN_SERVICE:
		/* T31(ANSI) is a false congesiton detection timer */
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
			return (0);
		}
	case SLS_WACK_SLTM:
		if ((err = sl_stop_req(q, sl)))
			goto error;
		if ((err = sl_set_state(q, sl, SL_FAILED)) < 0)
			goto error;
		if (sl->flags & SLF_BLOCKED) {
			if ((err = sl_set_state(q, sl, SL_UNBLOCKED)) < 0)
				goto error;
			/* flush old signalling link buffers */
			if ((err = sl_clear_buffers_req(q, sl)))
				goto error;
		}
		/* purge any remaining unclaimed messages */
		bufq_purge(&sl->rbuf);
		if ((err = sl_set_state(q, sl, SL_UPDATED)) < 0)
			goto error;
		if ((sl->lk.lk->sp.loc->flags & SPF_LOSC_PROC_A)
		    && mi_timer_reamin(sl->timers.t32a)) {
			/* ANSI link oscillation procedure A */
			/* link was in probation, now in suspension */
		} else if ((sl->lk.lk->sp.loc->flags & SPF_LOSC_PROC_B)
			   && sl_timer_remain(sl, t33a)) {
			/* ANSI link oscillation procedure B */
			/* link was in probation, now place it in suspension */
			sl_timer_stop(sl, t33a);
			sl_timer_start(q, sl, t34a);
		} else {
			/* start restoration */
			if ((err = sl_start_req(q, sl)))
				goto error;
			/* T19(ANSI) is a maintenance guard timer */
			sl_timer_start(q, sl, t19a);
		}
		sl->flags &= ~(SLF_WACK_SLTM | SLF_WACK_SLTM2);
		return (0);
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
/**
 * cb_t1_timeout: - TIMER T1 - changeover
 * @q: active queue
 * @cb: related changeover/changeback buffer
 *
 * IMPLEMENTATION NOTE:-  Similar analysis as for T2 timer: we discard the
 * buffer and restart traffic.
 */
static int
cb_t1_timeout(queue_t *q, struct cb *cb)
{
	struct sl *sl = cb->sl.from;

	/* complete the changeover but discard buffer contents first */
	bufq_purge(&cb->buf);
	cb_divert_buffer(cb);
	/* Also purge and reroute any other buffers pending on t1 for the same link */
	for (cb = sl->lk.lk->cb.list; cb; cb = cb->lk.next) {
		if (cb->sl.from == sl && cb_timer_remain(cb, t1)) {
			cb_timer_stop(cb, t1);
			bufq_purge(&cb->buf);
			cb_divert_buffer(cb);
		}
	}
	return sl_stop_restore(q, sl);
}

/**
 * cb_t2_timeout: - TIMER T2 - changeover ack
 * @q: active queue
 * @cb: related changeover/changeback buffer
 *
 * 5.7.2  If no changeover message in response to a changeover order is
 * received within a timer T2 (see 16.8), new traffic is started on the
 * alternative signalling link(s).
 *
 * IMPLEMENTATION NOTE: There is a problem here.  We cannot check the link for
 * congestion when transferring the buffer.  If we a transferring a very large
 * changeover buffer (up to 2 seconds worth of messages at 1 Erlang) we could
 * congest the new link leading to a cascading failure.  Section 5.7.2 above,
 * does not say to transfer the buffer on T2 expiry.  Instead it says to start
 * "new traffic" on the alternative signalling link(s).  Taking this
 * literally, we will flush the buffer and start new traffic only.
 */
static int
cb_t2_timeout(queue_t *q, struct cb *cb)
{
	struct sl *sl = cb->sl.from;

	/* complete the changeover but discard buffer contents first */
	bufq_purge(&cb->buf);
	cb_divert_buffer(cb);
	/* Also purge and reroute any other buffers pending on t2 for the same link */
	for (cb = sl->lk.lk->cb.list; cb; cb = cb->lk.next) {
		if (cb->sl.from == sl && cb_timer_remain(cb, t2)) {
			cb_timer_stop(cb, t2);
			bufq_purge(&cb->buf);
			cb_divert_buffer(cb);
		}
	}
	return sl_stop_restore(q, sl);
}

/**
 * cb_t3_timeout: - TIMER T3 - changeback
 * @q: active queue
 * @cb: related changeover/changeback buffer
 *
 * 6.2.5  If the signalling point at the far end of the link made available
 * currently is inaccessible, from the signalling point initiating changeback
 * (see clause 9 on MTP Restart), the sequence control procedure specified in
 * 6.3 (which requires communication between the two concerned signalling
 * points) does not apply; instead, the time-controlled diversion specified in
 * 6.4 is performed.  This is made also when the concerned signalling points
 * are accessible, but there is no signalling route to it using the same
 * outgoing signalling link(s) (or one of the same signalling links) from
 * which traffic will be diverted.
 *
 * 6.4.1  The timer-controlled diversion procedure is used at the end of the
 * MTP restart procedure (see clause 9) when an adjacent signalling point
 * becomes available, as well as for the reasons given in 6.2.5. ...
 *
 * 6.4.2  When changeback is initiated after the MTP restart procedure, the
 * adjacent signalling point of the point whose MTP is restarting stops
 * traffic to be directed from the alternative signalling link(s) for a time
 * T3, after which it starts traffic on the signalling link(s) made available.
 * The time delay minimizes the probability of out-of-sequence delivery to the
 * destination point(s).
 */
static int
cb_t3_timeout(queue_t *q, struct cb *cb)
{
	/* restart traffic */
	cb_divert_buffer(cb);
	return (0);
}

/**
 * cb_t4_timeout: - TIMER T4 - changeback ack
 * @q: active queue
 * @cb: related changeover/changeback buffer
 *
 * 6.5.3   If no changeback-acknowledgement is received in response to a
 * changeback declaration within a timer T4, the changeback declaration is
 * repeated and a new timer T5 is started.  If no changeback acknowledgement
 * is received before the expiry of T5, the maintenance functions are alerted
 * and traffic on the link made available is started.  The changeback code
 * contained in the changeback-acknowledgement message makes it possible to
 * determine, in the case of parallel changebacks, from more than one reserve
 * path, which changeback-declaration is unacknowledged and has therefore to
 * be repeated.
 */
static int
cb_t4_timeout(queue_t *q, struct cb *cb)
{
	struct sl *sl_from = cb->sl.from;
	struct sl *sl_onto = cb->sl.onto;
	struct lk *lk = sl_onto->lk.lk;
	struct sp *sp = lk->sp.loc;
	struct rs *rs = lk->sp.adj;

	mtp_send_cbd(q, sp, lk->ni, rs->dest, sp->pc, sl_onto->slc, sl_onto->slc,
		     cb->cbc, sl_from);
	cb_timer_start(q, cb, t5);
	return (0);
}

/**
 * cb_t5_timeout: - TIMER T5 - changeback ack second attempt
 * @q: active queue
 * @cb: related changeover/changeback buffer
 */
static int
cb_t5_timeout(queue_t *q, struct cb *cb)
{
	/* restart traffic */
	cb_divert_buffer(cb);
	return (0);
}

/**
 * cr_t6_timeout: - TIMER T6 - controlled rerouting
 * @q: active queue
 * @cr: related controlled rerouting buffer
 * 
 * Controlled rerouting is performed with a special controlled rerouting
 * buffer which is attached to both the route from which traffic is being
 * rerouted and the route to which traffic is being rerouted.  If the route
 * set has become unavailable while the timer was running, this will result in
 * purging the controlled rerouting buffer.  If the unavailability was
 * transient and the route set is now available again, these messages will not
 * be lost.  This is not mentioned in the MTP specifications.
 */
static int
cr_t6_timeout(queue_t *q, struct cr *cr)
{
	/* restart traffic */
	cr_reroute_buffer(cr);
	return (0);
}

/**
 * lk_t7_timeout: - TIMER T7 - waiting for SDLC ack
 * @q: active queue
 * @lk: related link set
 */
static int
lk_t7_timeout(queue_t *q, struct lk *lk)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/**
 * rs_t8_timeout: - TIMER T8 - transfer prohibited inhibitition timer
 * @q: active queue
 * @rs: related route set
 */
static int
rs_t8_timeout(queue_t *q, struct rs *rs)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/*
 * xx_t9_timeout: - TIMER T9 - (not used)
 * @q: active queue
 * @xx: related xx
 */

/**
 * rt_t10_timeout: - TIMER T10 - waiting to repeat route set test message
 * @q: active queue
 * @rt: related route
 * 
 * This timer is started after receiving a TFP, TCP, TFR or TCR for the route.
 * It runs and sends a RST message on the route every T10 seconds until
 * stopped.  This timer is stopped after receiving a TFA or TCA message for
 * the route.
 *
 * 13.5.2  A signalling route set test message is sent from a signalling point
 * after a transfer prohibited or transfer restricted message is received from
 * an adjacent signalling transfer point (see 13.2.4 and 13.4.4).  In this
 * case, a signalling route set test message is sent to that signalling
 * transfer point referring to the destination declared inaccessible or
 * restricted by the transfer prohibited or transfer restricted message, every
 * T10 period (see clause 16) until a transfer-allowed message, indicating
 * that the destination has become accessible, is received.
 *
 * Start:  receive TFP, TCP, TFR, TCR for first time Stop:   receive TFA, TCA
 * Expiry: send RST, RCP, RSR, RCR and start T10
 */
static int
rt_t10_timeout(queue_t *q, struct rt *rt)
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
			     mtp_send_rst(q, loc, lk->ni, adj->dest, loc->pc, 0,
					  rs->dest)) < 0)
				goto error;
		} else {
			if ((err =
			     mtp_send_rcp(q, loc, lk->ni, adj->dest, loc->pc, 0,
					  rs->dest)) < 0)
				goto error;
		}
		break;
	case RT_RESTRICTED:
		if (rt->type != RT_TYPE_CLUSTER) {
			if ((err =
			     mtp_send_rsr(q, loc, lk->ni, adj->dest, loc->pc, 0,
					  rs->dest)) < 0)
				goto error;
		} else {
			if ((err =
			     mtp_send_rcr(q, loc, lk->ni, adj->dest, loc->pc, 0,
					  rs->dest)) < 0)
				goto error;
		}
		break;
	default:
	case RT_ALLOWED:
		return (0);
	}
	rt_timer_start(q, rt, t10);
	return (0);
      error:
	rare();
	return (err);
}

/**
 * rs_t11_timeout: - TIMER T11 - transfer restricted timer
 * @q: active queue
 * @rs: related route set
 *
 * ANSI T1.111.4 (1996-2000)  13.4.2 ... (1) When the normal route(s) to destination X become
 * unavailable and signalling transfer point Y begins using a lower priority
 * route to destination X, timer T11 is started.  When timer T11 expires or
 * the lower priority route experiences danger of congestion (whichever occurs
 * first), signalling transfer point Y notifies accessible adjacent signalling
 * points using one of two possible methods:
 *
 * (a) Broadcast Method
 *
 * Signalling transfer point Y sends transfer-restricted messages concerning
 * destination X to all accessible adjacent signalling points, except those
 * signalling points that receive a transfer-prohibited message according to
 * 13.2.2 (1) and except destination X if it is an adjacent point.  Timer T18
 * is started after the broadcast is complete.  If the failure is still
 * present and T18 expires, a transfer-restricted message is sent once per
 * incoming link or link set by signalling transfer point Y in response to a
 * message for destination X.
 *
 * As a network option, the rate that transfer-restricted messages are
 * broadcast may be regulated to minimize the burst of traffic that can occur
 * due to controlled rerouting in adjacent nodes.  This regulation is not
 * performed for the broadcasting of transfer-restricted messages indicating
 * that a previously isolated destination is accessible.
 *
 */
static int
rs_t11_timeout(queue_t *q, struct rs *rs)
{
	int err;
	struct sp *sp = rs->sp.sp;
	struct na *na = sp->na.na;

	switch (rs->state) {
	case RS_RESTRICTED:
	case RS_CONGESTED:
	case RS_DANGER:
		switch (na->option.
			popt & (SS7_POPT_TFR | SS7_POPT_TFRB | SS7_POPT_TFRR)) {
		case 0:
			break;
		case SS7_POPT_TFR:
			/* old broadcast method - no responsive */
			if ((err = mtp_tfr_broadcast(q, rs)))
				return (err);
			break;
		case SS7_POPT_TFRB:
		case SS7_POPT_TFR | SS7_POPT_TFRB:
			/* We don't acrtually regulate broadcast of TFR per ANSI
			   T1.111.4-2000 13.4.2 (1)(a). If the operator wishes to
			   regular the rate of TFRs initially sent, the Responsive
			   method should be used (i.e. with SS7_POPT_TFRR */
			if ((err = mtp_tfr_broadcast(q, rs)))
				return (err);
			if (sp->flags & SPF_XFER_FUNC)
				/* prepare for final TFR response */
				rs_timer_start(q, rs, t18a);
			break;
		case SS7_POPT_TFRR:
		case SS7_POPT_TFRR | SS7_POPT_TFRB:
		case SS7_POPT_TFR | SS7_POPT_TFRR:
		case SS7_POPT_TFR | SS7_POPT_TFRR | SS7_POPT_TFRB:
			/* If both methods are specified, we will go with Responsive 
			   method because it is a little more stable than
			   non-regulated broadcasts */
			if (sp->flags & SPF_XFER_FUNC) {
				/* prepare for initial TFR response */
				struct ls *ls;
				struct lk *lk;
				struct rr *rr;

				/* What we have to do here is establish a structure
				   linking every incoming link set structure to
				   which we offer a route to this route set
				   structure. Then if a message arrives for the
				   route set on a link set matching one of these
				   structures, we would send responsive TFR and then 
				   delete the structure. */
				/* establish new route restriction list */
				for (ls = sp->ls.list; ls; ls = ls->sp.next)
					for (lk = ls->lk.list; lk; lk = lk->ls.next)
						if (!(rr = mtp_alloc_rr(rs, lk)))
							return (-ENOMEM);
				/* we have to wait for the first TFR to start timer
				   T18a */
				rs->flags |= RSF_TFR_PENDING;
				return (0);
			}
			break;
		}
		return (0);
	}
	rare();
	ptrace(("Stale timeout for timer t11\n"));
	return (0);
}

/**
 * rs_t18a_timeout: - TIMER T18(ANSI) - awaiting TFR final repsonse
 * @q: active queue
 * @rs: related route set
 */
static int
rs_t18a_timeout(queue_t *q, struct rs *rs)
{
	struct rr *rr, *rr_next = rs->rr.list;

	/* purge any existing route restriction list */
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

		/* establish a new route restriction list */
		for (ls = rs->sp.sp->ls.list; ls; ls = ls->sp.next)
			for (lk = ls->lk.list; lk; lk = lk->ls.next)
				if (!(rr = mtp_alloc_rr(rs, lk)))
					return (-ENOMEM);
		return (0);
	}
	}
	rare();
	ptrace(("Stale timeout for timer T18a\n"));
	return (0);
}

/**
 * sl_t12_timeout: - TIMER T12 - uninhibit ack
 * @q: active queue
 * @sl: related signalling link
 * @count: number of times expired
 *
 * Q.704/1996 10.2
 *
 * g) If, for any reason, the inhibit acknowledgement message is not received,
 *    a timer T14 expires and the procedure is restarted including inspection
 *    of the status of the destination of the inhibit message.  If the
 *    destination is not available, management is informed.
 *
 * At most two consecutive automatic attempts may be made to inhibit a
 * particular signalling link.
 */
static int
sl_t12_timeout(queue_t *q, struct sl *sl, uint count)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;

	if (count >= 2) {
		/* cancel link uninhibit procedure */
		fixme(("Implement this function\n"));
		return (-EFAULT);
	} else {
		sl_timer_restart(q, sl, t12);
		fixme(("Implement this function\n"));
		return (-EFAULT);
	}
}

/**
 * sl_t13_timeout: - TIMER T13 - waiting for force uninhibit
 * @q: active queue
 * @sl: related signalling link
 * @count: number of times expired
 *
 * Q.704/1996 10.3.2 (Signalling routing control initiated uninhibiting)
 *
 * e) If for any reason, an uninhibit signalling link message is not received
 *    in response to the force uninhibit message, a timer T13 expires.  If
 *    this is the first expiry of T13 for this uninhibition attempt on this
 *    link, the procedure is restarted including inspection of the status of
 *    the inhibited link.  If the link is marked failed or blocked, or timer
 *    T13 has expired for the second time during uninhibition of this link,
 *    management is informed and the uninhibition is abandoned.
 *
 * f) If an attempt to uninhibit a signalling link is abandoned, signalling
 *    routing control attempts to uninhibit the next inhibited link to
 *    signalling point Y, starting from a) above.  The search continues until
 *    either a link is sucessfully uninhibited or all possible links to Y in
 *    the routing table have been exhausted, or the destination has become
 *    available for other reasons.
 */
static int
sl_t13_timeout(queue_t *q, struct sl *sl, uint count)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;

	if (count >= 2) {
		/* cancel force uninhibit procedure */
		fixme(("Implement this function\n"));
		return (-EFAULT);
	} else {
		sl_timer_restart(q, sl, t13);
		fixme(("Implement this function\n"));
		return (-EFAULT);
	}
}

/**
 * sl_t14_timeout: - TIMER T14 - waiting for inhibition ack
 * @q: active queue
 * @sl: related signalling link
 * @count: number of times expired
 *
 * Q.704/1996 10.2
 *
 * g) If, for any reason, the inihibit acknowledgement message is not
 *    received, a timer T14 epires and the procedure is restarted including
 *    inspection of the status of the destination of the inhibit mesage.  If
 *    the destination is not available, management is informed.
 *
 * At most two consecutive atuomatic attempts may be made to inhibit a
 * particular signalling link.
 */
static int
sl_t14_timeout(queue_t *q, struct sl *sl, uint count)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;

	if (count >= 2) {
		/* cancel inhibit procedure */
		fixme(("Implement this function\n"));
		return (-EFAULT);
	} else {
		sl_timer_restart(q, sl, t14);
		fixme(("Implement this function\n"));
		return (-EFAULT);
	}
}

/**
 * rs_t15_timeout: - TIMER T15 - waiting to start route set congestion test
 * @q: active queue
 * @rs: related route set
 *
 * 13.7.5  If T15 expires after the last update of the signalling route set
 * toward destination X by a transfer-controlled message relating to the same
 * destination, the signalling route set congestion test procedure is invoked
 * (see 13.9).
 *
 * 13.9.4  Provided that the signalling route set toward destination X is not
 * in the "unavailable" state, a signalling route set congestion test message
 * is sent from an originating signalling point to destination X in the
 * following cases: i) When T15 expires after the last update of the
 * congestion status of the signalling route set toward destination X by a
 * transfer controlled message relating to the same destination.
 *
 * Start:  TFC received
 * Stop:   --
 * Expiry: Send RCT, start T16
 */
static int
rs_t15_timeout(queue_t *q, struct rs *rs)
{
	int err;

	if (rs->cong_status) {
		struct sp *sp = rs->sp.sp;

		rs_timer_start(q, rs, t16);
		if ((err =
		     mtp_send_rct(q, sp, sp->ni, rs->dest, sp->pc, 0,
				  rs->cong_status)) < 0)
			goto error;
	}
	return (0);
      error:
	return (err);
}

/**
 * rs_t16_timeout: - TIMER T16 - waiting for route set congestion status update
 * @q: active queue
 * @rs: related route set
 *
 * 13.9.3  If T16 expires after sending a signalling route set congestion test
 * message without a transfer controlled message relating to the concerned
 * destination having been received, the signalling point changes the
 * congestion status associated with the signalling route set toward the
 * concerned destination to the next lower status.
 *
 * 13.9.4  Provided that the signalling route set toward destination X is not
 * in the "unavailable" state, a signalling route set congestion test message
 * is sent from an originating signalling point to destination X in the
 * following cases: i) ... ii) When T16 expires after sending a signalling
 * route set congestion test message to destination X without a transfer
 * controlled message relating to the same destination having been received.
 * After the congestion status have been decremented by one, the test is
 * repeated, unless the congestion status is zero.
 *
 * Start:  T15 expires
 * Stop:   TFC received
 * Expiry: Decrement cong status, inform users, send RCT and restart if still congested
 */
static int
rs_t16_timeout(queue_t *q, struct rs *rs)
{
	int err;

	if (rs->cong_status) {
		struct sp *sp = rs->sp.sp;
		uint newstatus;

		if ((newstatus = rs->cong_status - 1)) {
			rs_timer_start(q, rs, t16);
			if ((err =
			     mtp_send_rct(q, sp, sp->ni, rs->dest, sp->pc, 0,
					  newstatus)) < 0)
				goto error;
		}
		if ((err = mtp_cong_status_ind_all_local(q, rs, newstatus)) < 0)
			goto error;
		rs->cong_status = newstatus;
	}
	return (0);
      error:
	return (err);
}

/**
 * sl_t17_timeout: - TIMER T17 - link restart
 * @q: active queue
 * @sl: related signalling link
 *
 * 12.2.1.2    When a decision is taken to activate an inactive signalling
 * link; initial alignment starts.  If the initial alignment procedure is
 * successful, the signalling link is active and a signalling link test is
 * started.  If the signalling link test is successful the link becomes ready
 * to convey signalling traffic.  In the case when initial alignment is not
 * possible, as determined at Message Transfer Part level 2 (see clause
 * 7/Q.703), new initial alignment procedures are started on the same
 * signalling link after a timer T17 (delay to avoid the oscillation of
 * initial alignment failure and link restart.  The value of T17 should be
 * greater than the loop delay and less than timer T2, see 7.3/Q.703).  If the
 * signalling link test fails, link restoration starts until the signalling
 * link is activated or a manual intervention is made.
 *
 * 12.2.2  Signalling link restoration
 *
 * After a signalling link failure is detected, signalling link initial
 * alignment will take place.  In the case when the initial alignment
 * procedure is successful, a signalling link test is started.  If the
 * signalling link test is successful the link becomes restored and thus
 * available for signalling.
 *
 * If the initial alignment is not possible, as determined at Message Transfer
 * Part level 2 (see clause 7/Q.703), new initial alignment procedures may be
 * started on the same signalling link after a time T17 until the signalling
 * link is restored or a manual intervention is made, e.g. to replace the
 * signalling data link or the signalling terminal.
 *
 * If the signalling link test fails, the restoration procedure is repeated
 * until the link is restored or a manual intervention is made.
 */
static int
sl_t17_timeout(queue_t *q, struct sl *sl)
{
	/* simply attempt to restart the link */
	/* T19(ANSI) is a maintenance guard timer */
	sl_timer_start(q, sl, t19a);
	return sl_start_req(q, sl);
}

/**
 * sp_t1r_timeout: - TIMER T1R - MTP restart timer
 * @q: active queue
 * @sp: related signalling point
 *
 * This is the same as timer T1 for signalling links, but is used to determine
 * the amount of time from the last failed signalling link until restart is
 * required.
 *
 * When the timer expires, mark restart.  (ANSI procedures call for the
 * extension of the MTP unavailability period by T27a so that adjacent nodes
 * are aware of the restart condition.  Note that this is about the time of T1
 * + T2 which is the amount of time taken for the remote end to institute and
 * complete changeover procedures for the restarting node.) When the first
 * link is in service at level 2 (and T27a is not running or expires),
 * additional actions will be performed.
 */
static int
sp_t1r_timeout(queue_t *q, struct sp *sp)
{
	struct ls *ls;
	struct lk *lk;
	struct sl *sl;
	int err;

	for (ls = sp->ls.list; ls; ls = ls->sp.next) {
		for (lk = ls->lk.list; lk; lk = lk->ls.next) {
			for (sl = lk->sl.list; sl; sl = sl->lk.next) {
				if ((err = sl_local_processor_outage_req(q, sl)))
					return (err);
				noenable(sl->rq);
				sl->flags |= SLF_RESTART_LOCKOUT;
			}
		}
	}
	sp->flags |= (SPF_RESTART | SPF_RESTART_LOCKOUT);
	mi_timer(q, sp->timers.t27a, sp->config.t27a);
	return (0);
}

/**
 * sp_t18_timeout: - TIMER T18 - MTP restart timer
 * @q: active queue
 * @sp: related signalling point
 *
 * Timer T18 is a phase 1 restart timer and is started by a restarting
 * signalling point with the transfer function when the first link goes into
 * service at level 2.
 *
 * 9.2.2 ... Timer T18 is implementation and network dependent, and is stopped
 * when:
 *
 * 1) sufficient links and link sets are available to carry the expected
 *    signalling traffic; and
 *
 * 2) enough TRA messages (and therefore routing data) have been received to
 *    give a high level of confidence in the MTP routing tables.
 *
 * NOTE -- In normal circumstances the restarting MTP should wait for TRA
 * messages from all adjacent nodes.  There are, however, other situations
 * where this might not be useful, e.g. for a long-term equipment failure.
 *
 * When T18 is stopped or expires, these second phase begins, which includes
 * as a major part a broadcast of non-preventative transfer prohibited
 * messages [i.e. those TFPs according to 13.2.2 v)] and transfer-restricted
 * messages, taking into account signalling link sets which are not available
 * and any TFP, TFA and TFR messages received during phase 1.  Note that timer
 * T18 is determined such that during phase 2 the broadcast of TFP and TFR
 * messages may be completed in normal situations.
 */
static int
sp_t18_timeout(queue_t *q, struct sp *sp)
{
	sp->flags &= ~SPF_RESTART_PHASE_1;
	sp->flags |= SPF_RESTART_PHASE_2;
	fixme(("complete this function\n"));
	return (-EFAULT);
}

/**
 * lk_t19_timeout: - TIMER T19 - MTP restart timer
 * @q: active queue
 * @sp: related signalling point
 *
 * 9.2.4 ... In addition, [to sending TRA] timer T19 is started (see 9.5.2)
 * for all signalling points to which a TRA message has just been sent.
 * Normal operation is then resumed.
 *
 * 9.5.1  If a signalling point X receives an unexpected TRA message from an
 * adjacent node Y and no associated T19 timer is running, X sends to Y any
 * necessary TFP and TFR messages if X has the transfer function, and a TRA
 * message to Y.  In addition, X starts timer T19 associated with Y.
 *
 * 9.5.2  If a signalling point receives a TRA message from an adjacent node
 * and an associated T19 is running, this TRA is discarded and no further
 * action is taken.
 *
 * IMPLEMENTATION NOTES:- ITU T19 is like ANSI T29 and T30 and is set on a
 * link set basis.
 */
static int
lk_t19_timeout(queue_t *q, struct lk *lk)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/**
 * sl_t19a_timeout: - TIMER T19(ANSI) - Link Maintenance timer
 * @q: active queue
 * @sl: related signalling link
 *
 * ANSI T1.111.4 (1996 and 2000) 12.2.2.  After a signalling link failure is
 * detected, signalling link initial alignment will take place.  When the
 * initial alignment procedure is successful, a signalling link test is
 * started.  If the signalling link test is successful, the signalling link is
 * regarded as restored and thus available for signalling.
 *
 * If initial alignment is not possible, as determined at level 2 (see Section
 * 7 of Chapter T1.111.3 or Section 6.2.2 of T1.645), new initial alignment
 * procedures may be started on the same signalling link after a timer T17
 * until the signalling link is restored or a manual intervention is made,
 * e.g., to replace the signalling data link or the signalling terminal.  If
 * after time T19 the signalling link has not activated, a management function
 * is notified, and, optionally, T19 may be restarted.
 */
static int
sl_t19a_timeout(queue_t *q, struct sl *sl)
{
	switch (sl_get_l_state(sl)) {
	case SLS_WIND_INSI:
		todo(("Management notification\n"));
		sl_timer_start(q, sl, t19a);
		/* kick it again */
		return sl_start_req(q, sl);
	}
	rare();
	return (-EFAULT);
}

/**
 * sp_t20_timeout: - TIMER T20 - MTP restart timer
 * @q: active queue
 * @sp: related signalling point
 *
 * Timer T20 is an overall restart timer that is started by a restarting
 * signalling point when the first link goes into service at level 2.
 *
 * 9.2.2 ... When all TFP and TFR messages have been sent, the overall restart
 * timer T20 is stopped and phase 2 is finished.  Note that preventative TFP
 * messages [i.e. those according to 13.2.2 i)], except possibly those for
 * highest priority routes, must have been sent before normal User traffic is
 * carried.  This might be done during or after phase 2.
 *
 * 9.2.3 If the restarting MTP has no transfer function, phase 1 (see 9.2.2)
 * but not phase 2 is present.  In this case, the whole restart time is
 * available for phase 1.  The overall restart timer T20 is stopped when:
 *
 * 1) sufficient links and link sets are available to carry the expected
 *    signalling traffic; and
 *
 * 2) enough TRA messages (and therefore routing data) have been received to
 *    give a high level of confidence in the MTP routing tables.
 *
 * 9.2.4  When T20 is stopped or expires, the restarting MTP of the signalling
 * point or signalling transfer point sends traffic restart allowed messages
 * to all adjacent signalling points via corresponding available direct link
 * sets, and an indication of the end of MTP restart is sent to all local MTP
 * Users showing each signalling point's accessibility or inaccessibility.
 * The means of doing the latter is implementation dependent.
 *
 * In addition, [to sending TRA] timer T19 is started (see 9.5.2) for all
 * signalling points to which a TRA message has just been sent.  Normal
 * operation is then resumed.
 *
 * When T20 expires the transmission of TFP and TFR messages is stopped.
 * However, preventative TFP messages [i.e.  those according to 13.2.2 i)]
 * except possibly those for highest priority routes, must have been sent
 * before MTP User traffic is restarted.
 *
 */
static int
sp_t20_timeout(queue_t *q, struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/**
 * sl_t20a_timeout: - TIMER T20(ANSI)
 * @q: active queue
 * @sl: related signalling link
 *
 * ANSI T1.111.4/2000 10.3A (Inhibit Test Procedure Actions.)  When a
 * signalling link becomes inhibited, either locally, remotely or both, the
 * following test procedure actions take place, as appropriate:
 *
 * (1) If timer T20 expires att signalling point X and the signalling link is
 *     locally inhibited at signalling point X, signalling point X transmits a
 *     local inhibit test messae to the signalling point at the other end of
 *     the locally inhiited link and restarts timer T20.
 *
 * (2) If timer T20 expires at signalling point X and the signalling link is
 *     no longer locally inhibited, no action is taken.
 *
 * (3) If timer T21 expires at signalling point X and the signalling link is
 *     remotetly inhibited at signalling point X, signalling point X transmits
 *     a remote inhibit test messsage to the signalling point at the other end
 *     of the remotely inhibited link and restarts timer T21.
 *
 * (4) If timer T21 expires at signalling point X and the signalling link is
 *     no longer remotely inhibited, no action is taken.
 *
 * (5) If signalling point Y receives a local inhibit test message from
 *     signalling point X and the concerned link is marekd remotely inhibited,
 *     no action is taken.
 *
 * (6) If signalling point Y receives a local inhibit test message from
 *     signalling point X and the concerned link is not marked remotely
 *     inhibited, the forced uninhibiting procedure is invoked at signalling
 *     point Y, beginning with the transmission of a force uninhibit
 *     signlaling link message to signalling point X.  As a result of this
 *     procedure, the local inhibit indicator is canceled at signalling point
 *     X.
 *
 * (7) If signalling point Y receives a remote inhibit test mesage from
 *     signalling point X and the concerned link is marked locally inhibited,
 *     no action is taken.
 *
 * (8) If signalling point Y receives a remote inhibit test message from
 *     signalling point X and the concerned signalling link is not marked
 *     locally inhibited, the uninhibiting procedure is invoked at signalling
 *     point Y, beginning with the transmission of an uninhibit signalling
 *     link message to signalling point X.  As a result of this procedure, the
 *     remote inhibit indicator is canceled at signalling point X.
 *
 * IMPLEMENTATION NOTE:- ANSI Timer T20 is simply ITU-T Timer T22 renumbered.
 */
static int
sl_t20a_timeout(queue_t *q, struct sl *sl)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;

	if ((sl->flags & SLF_LOC_INHIBIT)) {
		if ((err = mtp_send_llt(q, loc, lk->ni, adj->dest, loc->pc, 0, sl->slc)))
			return (err);
		sl_timer_start(q, sl, t20a);
	}
	return (0);
}

/**
 * sl_t21a_timeout: - TIMER T21(ANSI)
 * @q: active queue
 * @sl: related signalling link
 *
 * ANSI T1.111.4-2000  10.2.  Inihibiting Initiation and Actions.  When at
 * signalling point X, a request is received from a management function to
 * inhibit a signalling link to signalling point Y, the following actions take
 * place:
 *
 * (1) A check is performed at signalling point X to determine whether, in the
 *     case of an unavailable link, inhibiting will result in a destination
 *     becoming inaccessible, or in the case of an unavailable link,
 *     signalling point Y is inaccessible.  If either is the case, management
 *     is informed that the inihibiting request is denied.
 *
 * (2) If inhibiting is permitted, signalling point X sends an inhibit message
 *     to signalling point Y, indicating that it wishes to inhibit the
 *     signalling link identified in the message.
 *
 * (3) Signalling point Y, on receiving the inhibit message from X, checks
 *     whether, in the case of an available link, inhibiting will result in a
 *     destination becoming inaccessible; and, if so, an inhibit denied
 *     message is returned to signalling point X.  The latter then informs the
 *     management function which requested inhibiting that the request cannot
 *     be granted.
 *
 * (4) If signalling point Y finds the inhibiting of the concerned link is
 *     permissible, it sends an inhibit acknowledgement to signalling point X,
 *     marks the link remotely inhibited, and starts inhibit test procedure
 *     timer T21. [10]  If the link concerned is currently carrying traffic,
 *     signalling point Y sends the inhibit acknowledgement via that link and
 *     diverts subsequent traffic for it, using the time controlled changeover
 *     procedure.
 *
 * (5) On receiving an inhibit acknowledgement message, signalling point X
 *     marks the link locally inhibited, informs management that the link is
 *     inhibited, and starts inhibit test procedure timer T20. [10]  If the
 *     link concerned is currently carrying traffic, signalling point X
 *     diverts subsequent traffic for the link, using the time controlled
 *     changeover procedure.
 *
 * (6) When changeover has been completed, the link while inhibited, is
 *     unavailable for the transfer of user-generated traffic but still
 *     permits the exchange of test messages.
 *
 * (7) If, for any reason, the inhibit acknowledgement message is not
 *     received, a timer T14 [10] expires and the procedure is restarted
 *     including inspection of the status of the destination of the inhibit
 *     message.  If the destination is not available, management is informed.
 *
 * At most two consecutive automatic attempts may be made to inhibit a
 * particular link.
 *
 * ...
 *
 * ANSI T1.111.4-2000 10.3A  (Inhibit Test Procedure Actions.)  When a
 * signalling link becomes inhibited, either locally, remotely, or both, the
 * following test procedure actions take place, as appropriate:
 *
 * ...
 *
 * (3) If timer T21 expires at signalling point X and the signalling link is
 *     remotetly inhibited at signalling point X, signalling point X transmits
 *     a remote inhibit test messsage to the signalling point at the other end
 *     of the remotely inhibited link and restarts timer T21.
 *
 * (4) If timer T21 expires at signalling point X and the signalling link is
 *     no longer remotely inhibited, no action is taken.
 *
 * IMPLEMENTATION NOTE:- ANSI Timer T21 is simply ITU-T Timer T23 renumbered.
 */
static int
sl_t21a_timeout(queue_t *q, struct sl *sl)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;


	if ((sl->flags & SLF_REM_INHIBIT)) {
		if ((err = mtp_send_lrt(q, loc, lk->ni, adj->dest, loc->pc, 0, sl->slc)))
			return (err);
		sl_timer_start(q, sl, t21a);
	}
	return (0);
}

/**
 * lk_t21_timeout: - TIMER T21 - MTP restart timer
 * @q: active queue
 * @sp: related signalling point
 *
 * 9.3.1  A signalling point X considers that MTP of an inaccessible adjacent
 * signalling point Y is restarting when:
 *
 * - the first link in a direct link set is in the "in service" state at level
 * 2; or
 *
 * - another route becomes available due either to reception of a
 * corresponding TFA, TFR or TRA message, or by the corresponding link set
 * becoming available (see 3.6.2.2).
 *
 * 9.3.2  When the first link in a direct link set toward signalling point Y,
 * whose MTP is restarting, is in the "in service" state at level 2,
 * signalling point X starts a timer T21 and takes account of any TFP, TFA and
 * TFR messages received from signalling point Y.  In addition, X takes the
 * following actions:
 *
 * - if X has the transfer function, when the direct link set is available at
 * level 3, X sends any necessary TFP and TFR message to Y; then
 *
 * - X sends a traffic restart allowed message to signalling point Y.
 *
 * If a signalling point, previously declared to be inaccessible, becomes
 * available again before T21 is stopped or expires, a corresponding TFA or
 * TFR message is sent to the signalling point Y whose MTP is restarting.
 *
 * If a signalling point becomes prohibited or restricted to signalling X
 * after a TRA message has been sent by X to Y, X sends a corresponding TFP or
 * TFR message to Y.
 *
 * When a traffic restart allowed message has been received by X from
 * signalling point Y, and a TRA message has been sent by X to Y, X stops
 * timer T21.
 *
 * ...
 *
 * When T21 is stopped or expires, signalling point X sends a MTP-RESUME
 * primitive concerning Y, and all signalling points made available via Y, to
 * all local MTP Users.  If X has the transfer function, it broadcasts to
 * adjacent available signalling points transfer-allowed and/or transfer
 * restricted messages concerning Y and all signalling points made accessible
 * by Y.
 *
 * 9.6.2  After the MTP of an adjacent node X has restarted, and if T21 has
 * been started (see 9.3.2), all routes using X are considered to be available
 * unless corresponding TFP or TFR message have been received whilst T21 was
 * running.
 *
 * IMPLEMENTATION NOTES:- ITU-T T21 is similar to ANSI T28 and T25.
 */
static int
lk_t21_timeout(queue_t *q, struct lk *lk)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/**
 * sl_t22_timeout: - TIMER T22 - local inhibit test timer
 * @q: active queue
 * @sl: related signalling link
 *
 * Q.704/1996 10.6 (Inhibit test procedure)  When a signalling link becomes
 * management inhibited, periodic tests are started to guard the inhibition
 * status at each end of the link.
 *
 * 10.6.1  A local ihibit test is performed when timer T22 exires at
 * signalling poin X and the concernted link is marked locally inhibited.  In
 * this case, a local inhibit test message is sent to the signalling point Y
 * at the other end of the link, and timer T22 is restarted.
 *
 * Reception of a local inhibit test message causes:
 *
 * i)  no action, if the concerned link is marked remotely inhibited at the
 *     receiving signalling point Y; or
 *
 * ii) the force unhibit procedure to be invoked at the receiving signalling
 *     point Y, if the concerned link is not marked remotely inhibited at Y.
 *     This procedure causes the locally inhibited status of the link at X to
 *     be cancelled.
 *
 * If a timer T22 expires and the concerned link is not locally inhibited, no
 * further action is taken.
 */
static int
sl_t22_timeout(queue_t *q, struct sl *sl)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;

	if ((sl->flags & SLF_LOC_INHIBIT)) {
		if ((err = mtp_send_llt(q, loc, lk->ni, adj->dest, loc->pc, 0, sl->slc)))
			return (err);
		sl_timer_start(q, sl, t22);
	}
	return (0);
}

/**
 * sp_22a_timeout: - TIMER T22(ANSI)
 * @q: active queue
 * @sp: related signalling point
 *
 * ANSI T1.111.4/20000 9.2 ... When the first signalling link goes into the in
 * service state at level 2, the restarting signalling point begins taking
 * into account any transfer-prohibited, transfer-restricted,
 * transfer-allowed, traffic restart allowed, and traffic restart waiting
 * messages received.  The restarting point starts timers T22 and T26 either
 * when the first signalling link goes into the in service state at level 2 or
 * when the first signalling link becomes available at level 3.  When a
 * traffic restart waiting message is received before user traffic is
 * restarted on the link(s) to the point that sent the traffic restart waiting
 * message, timer T25 is started and user traffic is not restarted on that
 * link set until after a traffic restart allowed message is received or timer
 * T25 expires (see 9.3).
 *
 * When the first signalling link of a signalling link set is available, MTP
 * message traffic terminating at the far end of the signalling link set is
 * immediately restarted (see also 9.5), and a traffic restart waiting message
 * is sent to the point at the end of the link set.
 *
 * Whenever timer T26 expires, the restarting signalling point restarts timer
 * T26 and broadcasts a traffic restart waiting message to those adjacent
 * signalling points connected by an available link.
 *
 * T22 is stopped when sufficient links are available to carry the expected
 * signalling traffic: stopping T22 is a management decision that may take
 * into account factors such as the known long term unavailability of certain
 * signalling links because of equipment failures.
 *
 * When T22 is stopped or expires, the signalling point starts a timer T23,
 * during which it expects to receive additional transfer-prohibited,
 * transfer-restricted, transfer-allowed (see Section 13), traffic restart
 * waiting, and traffic restart allowed messages.  When traffic restart
 * allowed message have been received for all available links or the
 * management function determines that sufficient traffic restart allowed
 * messages have been received that the expected traffic can be handled, T23
 * is stopped.
 *
 * IMPLEMENTATION NOTES:- T22 is used to place an upper limit on the
 * availability of sufficient signalling links to handle the expected load.  A
 * good rule of thumb would be that when at least half of the signalling links
 * in each signalling link set are available, that there are sufficient links
 * available to handle traffic; however, this does not include the knowledge
 * of long term failures.  Note that T22 is set to about 10 minutes, and it
 * would be really bad if there was a signalling link set will long term link
 * failures that kept it below the rule of thumb causing the restart to take
 * longer than 10 minutes.  Therefore a better rule of thumb might be that
 * half the signalling links are either active or management blocked or in
 * link oscillation lockout.  With reference to ITU, this is still Restart
 * Phase 1 and the next phase is also Restart Phase 1.
 */
static int
sp_t22a_timeout(queue_t *q, struct sp *sp)
{
	/* Automatically move from the first part of Phase 1 to the second part of
	   Phase 1.  The coming available or oscillation lockout of signalling links 
	   should terminate this part earlier. */
	sp_timer_start(q, sp, t23a);
	return (0);
}

/**
 * sl_t23_timeout: - TIMER T23 - remote inhibit test timer
 * @q: active queue
 * @sl: related signalling link
 *
 * Q.704/1996 10.6 (Inhibit test procedure)  When a signalling link becomes
 * management inhibited, periodic tests are started to guard the inhibition
 * status at each end of the link.
 *
 * 10.6.2  A remote inhibit test is performed when timer T23 expires at
 * signalling point Y and the concerned link is marked remotely inhibited.  In
 * this case a remote inhibit test message is sent to signalling point X at
 * the other end of the link, and timer T23 is restarted.
 *
 */
static int
sl_t23_timeout(queue_t *q, struct sl *sl)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;


	if ((sl->flags & SLF_REM_INHIBIT)) {
		if ((err = mtp_send_lrt(q, loc, lk->ni, adj->dest, loc->pc, 0, sl->slc)))
			return (err);
		sl_timer_start(q, sl, t23);
	}
	return (0);
}

/**
 * sp_t23a_timeout: - TIMER T23(ANSI)
 * @q: active queue
 * @sp: related signalling point
 *
 * ANSI T1.111.4/2000 9.2 ... When T23 is stopped or expires, the signalling
 * point starts a timer T24, during which it broadcasts transfer-prohibited
 * and transfer-restricted messages (see Section 13), taking into account
 * signalling links which are not available and at least those transfer
 * prohibited, transfer-restricted and transfer-allowed message that were
 * received before T23 expired or was stopped.  Before user traffic is
 * restarted, preventative transfer-prohibited messages according to 13.2.2
 * (1) must be sent.  Whether these preventative transfer-prohibited messages
 * are sent for all routes or only for alternate routes is a network option.
 * When the broadcast of transfer-prohibited and transfer-restricted messages
 * is completed, timer T24 is stopped.
 *
 * IMPLEMENTATION NOTES:- In ANSI T22 and T23 are similar to ITU-T T18.  After
 * expiry of T23 or T18, the broadcast of transfer-prohibited,
 * transfer-restricted and transfer-allowed messages begins.  ANSI starts T24
 * in the same way that ITU-T (use to) starts T19.  ANSI T26 is the overall
 * timer that is roughly equivalent to ITU T20 (T26 rebroadcasts TRW during
 * the entire restart period).
 */
static int
sp_t23a_timeout(queue_t *q, struct sp *sp)
{
	sp->flags &= ~SPF_RESTART_PHASE_1;
	sp->flags |= ~SPF_RESTART_PHASE_2;
	sp_timer_start(q, sp, t24a);
	/* FIXME: start broadcast */
	return (0);
}

/**
 * sl_t24_timeout: - TIMER T24 - LPO timer
 * @q: active queue
 * @sl: related signalling link
 *
 * Q.704/3.3.5.1 Signalling traffic management: as specified in 3.3.1.1.  (See
 * SL_OUT_OF_SERVICE_IND).
 *
 * As a national option, local processor outage may also be applied to the
 * affected signalling link before commencement of the appropriate signalling
 * traffic management option.  On completion of that signalling traffic
 * management action, local processor outage is removed from the affected
 * signalling link.  No further signalling traffic management will be
 * performed on that affected signalling link until a timer T24 (see 16.8) has
 * expired or been cancelled, thus allowing time for indications from the
 * remote end to stabilize as it carries out any signalling traffic management
 * of its own.
 */
static int
sl_t24_timeout(queue_t *q, struct sl *sl)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/**
 * sp_t24a_timeout: - TIMER T24(ANSI)
 * @q: active queue
 * @sp: related signalling point
 *
 * ANSI T1.111.4/2000 9.2 ... When T24 is stopped, timer T26 is also stopped,
 * and the signalling point broadcasts traffic restart allowed message to all
 * adjacent signalling points and restarts the remaining traffic by giving
 * MTP-RESUME primitives to users for all accessible destinations.  It also
 * starts timer T29 for those points to which it has sent a traffic restart
 * allowed message.
 *
 * If T24 expires, the sending of any transfer-prohibited messages required by
 * 13.2.2 (1) is completed, timer T26 is stopped, and the signalling point
 * broadcasts traffic restart allowed messages to all adjacent signalling
 * points and restarts the remaining traffic by giving MTP-RESUME primitives
 * to users for all accessible destinations.  It also starts timer T29 for
 * those points to which it has sent a traffic restart allowed message.
 */
static int
sp_t24a_timeout(queue_t *q, struct sp *sp)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/**
 * lk_t25a_timeout: - TIMER T25(ANSI)
 * @q: active queue
 * @lk: related link set
 *
 * ANSI T1.111.4/2000 9.1 When the first signalling link goes into the in
 * service state at level 2, the restarting signalling point begins taking
 * into account any transfer-prohibited, transfer-restricted,
 * transfer-allowed, traffic restart allowed, and traffic restart waiting
 * messages received.  The restarting point starts timers T22 and T26 either
 * when the first signalling link goes in the in service state at level 2 or
 * when the first signalling link becomes available at level 3.  When a
 * traffic restart waiting message is receive before user traffic is restarted
 * on the link(s) to the point that sent the traffic restart waiting message,
 * timer T25 is started and user traffic is not restarted on that link set
 * until a traffic restart allowed message is received or timer T25 expires
 * (see 9.3).
 *
 * ANSI T1.111.4/2000 9.3 A signalling point X considers that the MTP of an
 * inaccessible adjacent signalling point Y is restarting when: (1) The first
 * link in a direct link set is in the in service state at level 2, or (2) A
 * route other than a direct link set becomes available, e.g.  through receipt
 * of a transfer allowed or traffic restart allowed message or the
 * availability of the corresponding link set.
 *
 * When the first link in a direct link set toward the restarting signalling
 * point Y goes into the in services state at level 2, signalling point X
 * begins taking into account any traffic restart waiting, traffic restart
 * allowed, transfer-prohibited, transfer-restricted, and transfer-allowed
 * messages from Y.  Signalling point X starts timer T28 either when the first
 * link goes into the in service state at level 2 or when the first signalling
 * link becomes available at level 3.  IN addition it takes the following
 * actions:
 *
 * (1) If a TRW message is received from Y while T28 is running or before it
 *     is started, X starts T25.  X stops T28 if it is running.
 *
 * (2) If a TRW message is received from Y while T25 is running, X restarts
 *     T25.
 *
 * (3) When the first link in a link set to Y becomes available, signalling
 *     point X sends to Y a traffic restart allowed message or, if X has the
 *     transfer function, a traffic restart waiting message followed by
 *     transfer-prohibited (note that all transfer prohibited messages
 *     according to 13.2.2 (1) must be sent) and transfer-restricted messages
 *     and a traffic restart allowed message.
 *
 * (4) If a destination becomes prohibited, restricted, or available at X,
 *     after X has sent a traffic restart allowed message to Y, X notifies Y
 *     of the status change by the normal procedures in Section 13.
 *
 * (5) When a traffic restart allowed message has been sent to Y and a traffic
 *     restart allowed message has been received from Y, X stops T25 or T28,
 *     whichever is running, and restarts traffic on the link set to Y.  X
 *     gives MTP-RESUME primitives to users concerning Y and any destinations
 *     made accessible via Y.  If X has the transfer function it also
 *     broadcasts transfer-allowed or transfer-restricted messages concerning
 *     the same destinations.
 *
 * (6) If T28 expires, X restarts traffic on the link set to Y as in (5),
 *     unless a traffic restart allowed message has not been sent to Y.  In
 *     that case, X starts T25 and completes the sending of transfer
 *     prohibited and transfer-restricted messages, followed by a traffic
 *     restart allowed message.  Then, unless a traffic restart waiting
 *     message has been received from Y without a subsequent traffic restart
 *     allowed message, X stops 25 and restarts traffic on the link set to Y.
 *
 * (7) If T25 expires, X restarts traffic on the link set toward Y.  In the
 *     abnormal case when X has not completed sending transfer-prohibited and
 *     transfer-restricted messages to Y, X completes sending the
 *     transfer-prohibited messages required by 13.2.2 (1) and sends a traffic
 *     restart allowed message before restarting user traffic.
 *
 * (8) If no traffic restart allowed message has been received from Y when
 *     traffic is restarted to Y, timer T9 (see 9.4) is started.
 *
 * When signalling point Y becomes accessible via a route other than a direct
 * link set between X and Y, X sends a MTP-RESUME primitive concerning Y to
 * all local MTP users.  In addition, if signalling point X has the transfer
 * function, X sends to Y any required transfer-prohibited and
 * transfer-restricted message by the available route and broadcasts
 * transfer-allowed or transfer-restricted messages concerning Y.
 */
static int
lk_t25a_timeout(queue_t *q, struct lk *lk)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/**
 * sp_t26a_timeout: - TIMER T26(ANSI)
 * @q: active queue
 * @sp: related signalling point
 *
 * ANSI T1.111.4/20000 9.2 ... When the first signalling link goes into the in
 * service state at level 2, the restarting signalling point begins taking
 * into account any transfer-prohibited, transfer-restricted,
 * transfer-allowed, traffic restart allowed, and traffic restart waiting
 * messages received.  The restarting point starts timers T22 and T26 either
 * when the first signalling link goes into the in service state at level 2 or
 * when the first signalling link becomes available at level 3.  When a
 * traffic restart waiting message is received before user traffic is
 * restarted on the link(s) to the point that sent the traffic restart waiting
 * message, timer T25 is started and user traffic is not restarted on that
 * link set until after a traffic restart allowed message is received or timer
 * T25 expires (see 9.3).
 *
 * When the first signalling link of a signalling link set is available, MTP
 * message traffic terminating at the far end of the signalling link set is
 * immediately restarted (see also 9.5), and a traffic restart waiting message
 * is sent to the point at the end of the link set.
 *
 * Whenever timer T26 expires, the restarting signalling point restarts timer
 * T26 and broadcasts a traffic restart waiting message to those adjacent
 * signalling points connected by an available link.
 */
static int
sp_t26a_timeout(queue_t *q, struct sp *sp)
{
	mtp_trw_broadcast(q, sp);
	sp_timer_start(q, sp, t26a);
	return (0);
}

/**
 * sp_t27a_timeout: - TIMER T27(ANSI)
 * @q: active queue
 * @sp: related signalling point
 *
 * ANSI T1.111.4/2000 9.1 (Actions in a Signalling Point Having the Transfer
 * Function Which Restarts.) ... If the management function determines that a
 * full restart procedure is needed, then it ensures, for example, by using
 * local processor outage, that the period of unavailability persists for a
 * time of at least T27.  The purpose of remaining unavailable for time T27 is
 * to increase the probability that adjacent points are aware of the
 * unavailability of the restarting point and initiate the appropriate restart
 * procedures (see 9.3).
 *
 * Timer T27:
 * Start:   When SP becomes unavailable (SPF_RESTART_LOCKOUT flag set)
 *
 * Stop:    When SP restarts without requiring restart procedure
 *          (SPF_RESTART_LOCKOUT flag cleared)
 *
 * Timeout: Deassert local processor outage condition on signalling links that
 *          realigned during the period that T27 was running.
 *
 * Signalling links that are in restart lockout are signalling links that
 * aligned during the T27a lockout period and were placed into local processor
 * outage state to affect the lockout.  Other options include suppressing
 * activation of the signalling link, or stopping signalling link that was in
 * a processor outage situation.  (Note that not all types of signalling links
 * support processor outage.) At any rate, depending on the state of the
 * signalling link at the time that the outage is to be cleared, the next
 * action is determined.  The purpose is always to return the signalling link
 * to the active state unless it is locally blocked by management.
 *
 */
static int
sp_t27a_timeout(queue_t *q, struct sp *sp)
{
	struct ls *ls;
	struct lk *lk;
	struct sl *sl;
	int err;

	sp->flags &= ~SPF_RESTART_LOCKOUT;

	/* Go looking for signalling links that are in restart lockout. */
	for (ls = sp->ls.list; ls; ls = ls->sp.next) {
		for (lk = ls->lk.list; lk; lk = lk->ls.next) {
			for (sl = lk->sl.list; sl; sl = sl->lk.next) {
				if (!(sl->flags & SLF_RESTART_LOCKOUT))
					continue;
				enableok(sl->rq);
				if ((err = sl_clear_buffers_req(q, sl)))
					return (err);
				if ((err = sl_resume_req(q, sl)))
					return (err);
				sl->flags &= ~SLF_RESTART_LOCKOUT;
				/* XXX: Might need to change state of link here, but 
				   probably not. I believe that in-service
				   indication from the aligned-not-ready state and
				   link-failure indication from failures during
				   local processor outage will be delivered
				   automatically. */
			}
		}
	}
}

/**
 * lk_t28a_timeout: - TIMER T28(ANSI)
 * @q: active queue
 * @sp: related signalling point
 *
 * ANSI T1.111.4/2000 9.3 (Actions in Signalling Point X Adjacent to a
 * Restarting Signalling Point Y.)  A signalling point X considers that the
 * MTP of an inaccessible adjacent signalling point Y is restarting when:
 *
 * (1) The first link in a direct link set is in the in service state at level
 *     2, or
 *
 * (2) A route other than a direct link set becomes available, e.g., through
 *     receipt of a transfer allowed or traffic restart allowed message or the
 *     availability of the corresponding link set.
 *
 * When the first link in a direct link set toward the restarting signalling
 * point Y goes into the in service state at level 2, signalling point X
 * begins taking into account any traffic restart waiting, traffic restart
 * allowed, transfer-prohibited, transfer-restricted, and transfer-allowed
 * messages from Y.  Signalling point X starts timer T28 either when the first
 * link goes into the in services state at level 2 or when the first
 * signalling link becomes available at level 3.  In addition, it takes the
 * following actions:
 *
 * (1) If a TRW message is received from Y while T28 is running or before it
 *     is started, X starts T25.  X stops T28 if it is running.
 *
 * (2) If a TRW message is received from Y while T25 is running, X restarts
 *     T25.
 *
 * (3) When the first link in a link set to Y becomes available, signalling
 *     point X sends to Y a traffic restart allowed message or, if X has the
 *     transfer function, a traffic restart waiting message followed by
 *     transfer-prohibited (note that all transfer-prohibited messages
 *     according to 13.2.2 (1) must be sent) and transfer restricted messages
 *     and a traffic restart allowed message.
 *
 * (4) If a destination becomes prohibited, restricted, or available at X,
 *     after X has sent a traffic restart allowed message to Y, X notifies Y
 *     of the status change by the normal procedures in Section 13.
 *
 * (5) When a traffic restart allowed message has been sent to Y and a traffic
 *     restart allowed message has been received from Y, X stops T25 or T28,
 *     whichever is running, and restarts traffic on the link set to Y.  X
 *     gives MTP-RESUME primitives to users concerning Y and any destinations
 *     made accessible via Y.  If X has the transfer function it also
 *     broadcasts transfer-allowed or transfer-restricted messages concerning
 *     the same destinations.
 *
 * (6) If T28 expires, X restarts traffic on the link set to Y as in (5),
 *     unless a traffic allowed message has not been sent to Y.  In that case,
 *     X starts T25 and completes the sending of transfer-prohibited and
 *     transfer-restricted messages, followed by a traffic restart allowed
 *     message.  Then, unless a traffic restart waiting message has been
 *     received from Y without a subsequent traffic restart allowed message, X
 *     stops T25 and restarts traffic on the link set to Y.
 *
 * (7) If T25 expires, X restarts traffic on the link set toward Y.  In the
 *     abnormal case when X has not completed sending transfer-prohibited and
 *     transfer-restricted messages to Y, X completes sending the
 *     transfer-prohibited messages required by 13.2.2 (1) and sends a traffic
 *     restart allowed message before restarting user traffic.
 *
 * (8) If no traffic restart allowed message has been received from Y when
 *     traffic is restarted to Y, timer T29 (see 9.4) is started.
 *
 * When signalling point Y becomes accessible via a route other than a direct
 * link set between X and Y, X sends an MTP-RESUME primitive concerning Y to
 * all local MTP users.  In addition, if signalling point X has the transfer
 * function, X sends to Y any required transfer-prohibited and
 * transfer-restricted messages by the available route and broadcasts
 * transfer-allowed or transfer-restricted messages concerning Y.
 *
 */
static int
lk_t28a_timeout(queue_t *q, struct lk *lk)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/**
 * lk_t29a_timeout: - TIMER T29(ANSI)
 * @q: active queue
 * @sp: related signalling point
 */
static int
lk_t29a_timeout(queue_t *q, struct lk *lk)
{
	fixme(("Implement this function\n"));
	return (-EFAULT);
}

/**
 * lk_t30a_timeout: - TIMER T30(ANSI)
 * @q: active queue
 * @sp: related signalling point
 *
 * ANSI T1.111.4/2000 9.4 ... If the receiving point has the transfer
 * function, it starts timer T30, sends a traffic restart waiting message
 * followed by the necessary transfer-restricted and transfer-prohibited
 * messages (preventive transfer-prohibited messages according to 13.2.2 (1)
 * are required for traffic currently being routed via the point from which
 * the unexpected traffic restart allowed or traffic restart waiting message
 * was received), and a traffic restart allowed message.  It then stops T30
 * and starts T29.  In the abnormal case that T30 expires before the sending
 * of transfer-prohibited and transfer-restricted messages is completed, it
 * sends a traffic restart allowed message, starts T29, and then completes
 * sending any preventative transfer-prohibited messages according to 13.2.2
 * (1) for traffic currently being routed via the point from which the
 * unexpected traffic restart allowed or traffic restart waiting message was
 * received.
 */
static int
lk_t30a_timeout(queue_t *q, struct lk *lk)
{
	struct ls *ls = lk->ls.ls;
	struct sp *sp = ls->sp.sp;
	struct rs *rs = lk->sp.adj;
	int err;

	if ((err = mtp_send_tra(q, sp, lk->ni, rs->dest, sp->pc, 0)))
		return (err);

	if ((sp->flags & SPF_XFER_FUNC) && (rs->flags & RSF_XFER_FUNC)) {
		for (rs = sp->rs.list; rs; rs = rs->sp.next) {
			/* If the route set it active and the current route is via
			   the signalling point that sent the TRA or TRW message,
			   send a transfer-prohibited message to the adjacent
			   signalling point. NOTE: this is only required when both
			   this and the adjacent signalling points are equipped with 
			   the transfer function. */
		}
	}
	return (0);
}

/**
 * sl_t31a_timeout: - TIMER T31 - False link congestion detection timer
 * @q: active queue
 * @sl: related signalling link
 * 
 * ANSI T1.111.4 (2000) 3.8.2.2.  ...  If the link remains at a congestion
 * level, as defined below, for greater than T31 [10] (a false link congestion
 * condition), and it also remains in service, an audit should trigger the
 * link to be restarted.  T31 is started when buffer occupancy increases
 * through onset level 1.  Any change of congestion level [defined as a change
 * of signalling link congestion status (see Section 3.8.2.1) and optionally
 * including the first time the discard status is increased to n after
 * signalling link congestion status is increased to  n (n=1, 2, 3)] restarts
 * the timer.  T31 is stopped when the signalling link congestion status is
 * zero.  The management should be notified if the link is restarted due to
 * false link congestion.
 */
static int
sl_t31a_timeout(queue_t *q, struct sl *sl)
{
	if (sl_get_l_state(sl) == SLS_IN_SERVICE) {
		todo(("Notify management of false congestion\n"));
		return sl_stop_restore(q, sl);
	}
	rare();
	return (-EFAULT);
}

/**
 * sl_t32a_timeout: - TIMER T32 - Link oscillation timer (Procedure A)
 * @q: active queue
 * @sl: related signalling link
 *
 * ANSI T1.111.4 (1996 and 2000) 12.2.2 ... In order to [sic] prevent links
 * from oscillating rapidly between the in-service and out-of-service states,
 * a link oscillation filtering procedure shall be provided.  This procedure
 * is applicable in all cases where automatic restoration and removal of
 * signalling links to and from service is used.  The procedure is not invoked
 * if restoration to or removal from service is initiated manually.
 *
 * Link oscillation filtering is a local procedure.  One of the following
 * procedures, A or B, is used.
 *
 * (A) When restoration begins, the oscillation timer T32 is started.  If the
 * link fails within T32, the link is placed in the "suspension" state until
 * T32 expires.
 *
 * When a link exits in the "suspension" state, link restoration is to be
 * started and T32 is restarted.
 *
 * When a link is in "suspension," the link remains in the out-of-service
 * state at level 2, restoration procedures to bring the link back into
 * alignment are not invoked and realignment attempts initiated by the remote
 * end are ignored.
 *
 * The oscillation timer T32 value chosen in a given signalling point may be
 * modified by management action.  The expiration of the timer does not change
 * the link status.
 */
static int
sl_t32a_timeout(queue_t *q, struct sl *sl)
{
	switch (sl_get_l_state(sl)) {
	case SLS_OUT_OF_SERVICE:
		/* we can now restore the link */
		/* T19(ANSI) is a maintenance guard timer */
		sl_timer_start(q, sl, t19a);
		return sl_start_req(q, sl);
	case SLS_IN_SERVICE:
		/* nothing to do */
		return (0);
	}
	rare();
	return (-EFAULT);
}

/**
 * sl_t33a_timeout: - TIMER T33 - Link oscillation probation timer (Procedure B)
 * @q: active queue
 * @sl: related signalling link
 *
 * ANSI T1.111.4 (1996 and 2000) 12.2.2 ...
 *
 * (B) When activation or restoration of a signalling link has been completed,
 * it is put on probation for a period controlled by timer T33 which starts
 * running when the link enters the in-service state and is stopped either on
 * time-out or on link failure.
 *
 * If a link fails during the probation, i.e. before expiration of timer T33,
 * it is put on suspension.  While suspended the link remains in the
 * out-of-service state at level 2, restoration procedures to bring the link
 * back into alignment are not invoked and realignment attempts initiated by
 * the remote end are ignored.
 *
 * The suspension time period is controlled by timer T34 which starts running
 * when the link enters the out-of-service state.  Link restoration is
 * resumed, on expiry of timer T34.
 *
 * The values chosen in a given network node for the probation and suspension
 * time periods are dependent on the type and function of the node in the
 * network and may be modified by management action.
 */
static int
sl_t33a_timeout(queue_t *q, struct sl *sl)
{
	if (sl_get_l_state(sl) == SLS_IN_SERVICE)
		/* link has passed probation */
		return (0);
	rare();
	return (-EFAULT);
}

/**
 * sl_t34a_timeout: - TIMER T34 - Link oscillation suspension timer (Procedure B)
 * @q: active queue
 * @sl: related signalling link
 */
static int
sl_t34a_timeout(queue_t *q, struct sl *sl)
{
	switch (sl_get_l_state(sl)) {
	case SLS_OUT_OF_SERVICE:
		/* link has exited suspension */
		/* T19(ANSI) is a maintenance guard timer */
		sl_timer_start(q, sl, t19a);
		return sl_start_req(q, sl);
	}
	rare();
	return (-EFAULT);
}

/**
 * sl_t1t_timeout: - TIMER T1T - SLTM ack
 * @q: active queue
 * @sl: related signalling link
 * 
 * Q.707 2.2 ...  In the case when the criteria given above are not met or a
 * signalling link test acknowledgement message is not received on the link
 * being tested within T1 (see 5.5) after the signalling link test message has
 * been sent, the test is considered to have failed and is repeated once.  In
 * the case when also the repeat test fails, the following actions have to be
 * taken:
 *
 * - SLT applied on activation/restoration, the link is put out of service,
 *   restoration is attempted and a management system must be informed.
 *
 * - SLT applied periodically, for further study.
 *
 * IMPLEMENTATION NOTE:- The action when the periodic test fails is for
 * further study.  Taking the link out of service can be an especially bad
 * idea during times of congestion. This is because the buffers may be filled
 * with management messages already and the signalling link test might fail
 * due to timeout. We merely report the information to management and leave
 * the link in service. If the test failed because of an inserted loop-back
 * while the link is in service, the link will fail rapidly due to other
 * causes. So we do nothing more than report to management.
 */
static int
sl_t1t_timeout(queue_t *q, struct sl *sl)
{
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;
	struct rs *rs = lk->sp.adj;
	int err;

	if (sl->flags & SLF_WACK_SLTM) {
		/* first test failed -- repeat test */
		if ((err =
		     mtp_send_sltm(q, sp, lk->ni, rs->dest, sp->pc, sl->slc, sl->slc,
				   sl->tdata, sl->tlen, sl)) < 0)
			return (err);
		sl->flags &= ~SLF_WACK_SLTM;
		sl_timer_start(q, sl, t1t);
		sl->flags |= SLF_WACK_SLTM2;
	} else if (sl->flags & SLF_WACK_SLTM2) {
		/* second test failed */
		if (sl_get_l_state(sl) == SLS_WACK_SLTM) {
			/* initial testing failed, fail link */
			if ((err = sl_stop_restore(q, sl)) < 0)
				return (err);
		} else {
			/* periodic testing failed, just report to management */
			todo(("Report periodic test failure to management\n"));
			sl_timer_start(q, sl, t2t);	/* restart periodic test */
		}
		sl->flags &= ~SLF_WACK_SLTM2;
	}
	return (0);
}

/**
 * sl_t2t_timeout: - TIMER T2T - SLTM interval
 * @q: active queue
 * @sl: related signalling link
 * 
 * Q.707 2.2 ... In the case the signalling link test (SLT) is applied while
 * the signalling link is in service the signalling link test message is sent
 * at regular intervals T2 (see 5.5).  The testing of a signalling link is
 * performed independently from each end.
 */
static int
sl_t2t_timeout(queue_t *q, struct sl *sl)
{
	if (sl_get_l_state(sl) == SLS_IN_SERVICE) {
		struct lk *lk = sl->lk.lk;
		struct sp *loc = lk->sp.loc;
		struct rs *adj = lk->sp.adj;
		int err, i;

		/* generate new test data */
		sl->tlen = jiffies & 0x0f;
		for (i = 0; i < sl->tlen; i++)
			sl->tdata[i] ^= ((jiffies >> 8) & 0xff);
		/* start new signalling link test */
		if ((err =
		     mtp_send_sltm(q, loc, lk->ni, adj->dest, loc->pc, sl->slc,
				   sl->slc, sl->tdata, sl->tlen, sl)))
			return (err);
		sl_timer_start(q, sl, t1t);
		sl->flags &= ~SLF_WACK_SLTM2;
		sl->flags |= SLF_WACK_SLTM;
		return (0);
	}
	swerr();
	return (-EFAULT);
}

static int
sl_ssltm_failed(queue_t *q, struct sl *sl)
{
	fixme(("Write this function\n"));
	return (-EFAULT);
}

/**
 * sl_t1s_timeout: - TIMER T1S - SSLTM ack
 * @q: active queue
 * @sl: related signalling link
 */
static int
sl_t1s_timeout(queue_t *q, struct sl *sl)
{
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;

	if (sl->flags & (SLF_WACK_SSLTM | SLF_WACK_SSLTM2)) {
		if (sl->flags & SLF_WACK_SSLTM) {
			sl_timer_start(q, sl, t1s);
			if ((err =
			     mtp_send_ssltm(q, loc, lk->ni, adj->dest, loc->pc,
					    sl->slc, sl->slc, sl->tdata, sl->tlen,
					    sl)) < 0)
				goto error;
			sl->flags &= ~SLF_WACK_SSLTM;
			sl->flags |= SLF_WACK_SSLTM2;
			return (0);
		}
		if ((err = sl_ssltm_failed(q, sl)) < 0)
			goto error;
		sl->flags &= ~SLF_WACK_SSLTM2;
		return (0);
	}
	swerr();
	return (-EFAULT);
      error:
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 * Receive message functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 * 5 Changeover
 *
 * 5.1.2 Changeover includes the procedure to be used in the case of
 * unavailability (due to failure, blocking or inhibiting) of a signalling
 * link, in order to diver the traffic pertaining to that signalling link to
 * one or more alternative signalling links.
 *
 * 5.2.1 Signalling traffic diverted from an unavailable signalling link is
 * routed by the concerned signalling point according to the rules specified
 * in clause 4.  In summary, two alternative situations may arise (either for
 * the whole diverted traffic or for traffic relating to each particular
 * destination): i) traffic is diverted to one or more signalling links of the
 * same link set; or, ii) traffic is diverted to one or more different link
 * sets.
 *
 * 5.2.2 As a result of these arrangements, and of the message routing
 * function described in clause 2, three different relationships between the
 * new signalling link and the unavailable one can be identified, for each
 * particular traffic flow.  These three basic cases may be summarized as
 * follows: a) the new signalling link is parallel to the unavailable one (see
 * Figure 9); b) the new signalling link belongs to a signalling route other
 * than that to which the unavailable signalling link belongs, but this
 * signalling route still passes through the signalling point at the far end
 * of the unavailable signalling link (see Figure 10); c) the new signalling
 * link belongs to a signalling route other than that to which the unavailable
 * signalling link belongs, and this signalling route does not pass through
 * the signalling point acting as signalling transfer point, at the far end of
 * the unavailable signalling link (see Figure 11).
 *
 * 5.3 Changeover initiation and actions
 *
 * 5.3.1  Changeover is initiated at a signalling
 *
 *
 */
/**
 * mtp_recv_coo: - RECV COO
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_coo(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;
	int err;

	/* first find the signalling link it pertains to */
	if (!(sl = mtp_lookup_sl(q, sl, m)))
		goto eproto;
	switch (sl_get_l_state(sl)) {
	case SLS_WIND_INSI:
	case SLS_IN_SERVICE:
	case SLS_PROC_OUTG:
	case SLS_WACK_SLTM:
		return sl_stop_restore(q, sl);
	case SLS_WIND_BSNT:
		/* haven't launched COO/ECO yet */
		sl->flags |= SLF_COO_RECV;
		/* we will ack the COO when BSNT is retrieved or fails */
		return (0);
	case SLS_WACK_COO:
	case SLS_WACK_ECO:
	case SLS_WCON_RET:
		if (sl->fsnc != -1) {
			/* BSNT already retrieved -- reply anyway with COA */
			if ((err =
			     mtp_send_coa(q, sp, m->ni, m->opc, m->dpc, m->sls,
					  m->slc, sl->fsnc)))
				goto error;
		} else {
			/* BSNT failed retrieval -- reply anyway with ECA */
			if ((err =
			     mtp_send_eca(q, sp, m->ni, m->opc, m->dpc, m->sls,
					  m->slc)))
				goto error;
		}
		if (sl_get_l_state(sl) != SLS_WCON_RET) {
			/* initiate retrieval and wait for result */
			if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
				goto error;
			return sl_retrieval_request_and_fsnc_req(q, sl, m->arg.fsnl);
		}
		goto discard;
	default:
		swerr();
		goto discard;
	}
      error:
	return (err);
      eproto:
	__ptrace(("\n"));
	goto discard;
      discard:
	return (0);
}

/**
 * mtp_recv_coa: - RECV COA
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_coa(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	int err;

	if (!(sl = mtp_lookup_sl(q, sl, m)))
		return (EPROTO);
	switch (sl_get_l_state(sl)) {
	case SLS_WACK_COO:
	case SLS_WACK_ECO:
		/* We were expecting a response */
		/* leave ack timer running to cover retrieval */
		/* do buffer updating because it is COA */
		if ((err = sl_clear_rtb_req(q, sl)))
			return (err);
		/* initiate retrieval and wait for result */
		if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
			return (err);
		return sl_retrieval_request_and_fsnc_req(q, sl, m->arg.fsnl);
	}
	return (0);		/* discard */
}

/**
 * mtp_recv_cbd: - RECV CDB
 * @q: active queue
 * @m: decoded message
 *
 * 6.2.1  Changeback is initiated at a signalling point when a signalling link
 * is restored, unblocked or uninhibited, and therefore it becomes once again
 * available, according to the criteria listed in 3.2.3 and 3.2.7.  The
 * following actions are then performed:
 *
 * a)  the alternative signalling link(s) to which traffic normally carried by
 *     the signalling link make available was previously diverted (e.g. on
 *     occurrence of a changeover), are determined.  To this set are added, if
 *     applicable, other links determined as defined in 4.4.2;
 *
 * b)  signalling traffic is diverted (if appropriate, according to the
 *     criteria specified in clause 4) to the concerned signalling link by
 *     means of the sequence control procedure specified in 6.3; traffic
 *     diversion can be performed at the discretion of the signalling point
 *     initiating changeback, as follows:
 *
 * i)   individually for each traffic flow (i.e. on destination basis);
 *
 * ii)  individually for each alternative signalling link (i.e. for all the
 *      destinations previously diverted on that alternative signalling link);
 *
 * iii) at the same time for a number of, or for all the alternative
 *      signalling links.
 *
 * 6.3.2   ... The remote signalling point will send the changeback
 * acknowledgement to the signalling point initiated changeback in response to
 * the changeback declaration; any available signalling route between the two
 * signalling points can be used to carry the changeback acknowledgement.
 *
 * 6.5.2   If a changeback-declaration is received after the completion of the
 * changeback procedure, a changeback-acknowledgement is sent in response,
 * without taking any further action.  This corresponds to the normal action
 * described in 6.3.2 above.
 */
static int
mtp_recv_cbd(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;

	return mtp_send_cba(q, sp, lk->ni, m->opc, sp->pc, 0, m->slc, m->arg.cbc);
}

/**
 * mtp_recv_cba: - RECV CBA
 * @q: active queue
 * @m: decoded message
 *
 * 6.5.1   If a changeback-acknowledgement is received by a signalling point
 * which has not previously sent a changeback-declaration, no action is taken.
 */
static int
mtp_recv_cba(queue_t *q, struct mtp_msg *m)
{
	struct cb *cb;

	for (cb = master.cb.list; cb; cb = cb->next) {
		struct lk *lk = cb->lk.lk;

		if (cb->cbc == m->arg.cbc && cb->slc == m->slc
		    && m->opc == lk->sp.adj->dest) {
			/* restart traffic */
			cb_divert_buffer(cb);
			return (0);
		}
	}
	mi_strlog(q, 0, SL_ERROR, "CBA with no matching CBD");
	return (EPROTO);
}

/**
 * mtp_recv_eco: - RECV ECO
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_eco(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *sp = lk->sp.loc;
	int err;

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		switch (sl_get_l_state(sl)) {
		case SLS_WIND_INSI:
		case SLS_IN_SERVICE:
		case SLS_PROC_OUTG:
		case SLS_WACK_SLTM:
			return sl_stop_restore(q, sl);
		case SLS_WIND_BSNT:
			/* haven't launched COO/ECO yet */
			sl->flags |= SLF_ECO_RECV;
			/* we will ack the ECO when BSNT is retrieved or fails */
			break;
		case SLS_WACK_COO:
		case SLS_WACK_ECO:
		case SLS_WCON_RET:
			if (sl->fsnc != -1) {
				/* BSNT already retrieved -- reply anyway with COA */
				if ((err =
				     mtp_send_coa(q, sp, m->ni, m->opc, m->dpc,
						  m->sls, m->slc, sl->fsnc)))
					return (err);
			} else {
				/* BSNT failed retrieval -- reply anyway with ECA */
				if ((err =
				     mtp_send_eca(q, sp, m->ni, m->opc, m->dpc,
						  m->sls, m->slc)))
					return (err);
			}
			if (sl_get_l_state(sl) != SLS_WCON_RET) {
				/* We didn't get FSNL in the ECO, therefore, we need 
				   to flush the retransmission buffer of potential
				   duplicates before retrieving the transmission
				   buffer. */
				/* do buffer flushing, and initiate retrieval */
				if ((err = sl_clear_rtb_req(q, sl)))
					return (err);
				/* initiate retrieval and wait for result */
				if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
					return (err);
				return sl_retrieval_request_and_fsnc_req(q, sl,
									 -1UL);
			}
			break;
		default:
			swerr();
			break;
		}
	} else
		rare();
	return (0);
}

/**
 * mtp_recv_eca: - RECV ECA
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_eca(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	int err;

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		switch (sl_get_l_state(sl)) {
		case SLS_WACK_COO:
		case SLS_WACK_ECO:
			/* We were expecting a response */
			/* leave ack timer running to cover retrieval */
			/* we can't do buffer updating because it is ECA */
			if ((err = sl_clear_rtb_req(q, sl)))
				return (err);
			/* initiate retrieval and wait for result */
			if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
				return (err);
			return sl_retrieval_request_and_fsnc_req(q, sl, -1UL);
		default:
			swerr();
			break;
		}
	} else
		rare();
	return (0);
}

/**
 * mtp_recv_eca: - RECV ECA
 * @q: active queue
 * @m: decoded message
 *
 * 13.9.6.  When a signalling route set congestion test message reaches its destination, it is discarded.
 */
static int
mtp_recv_rct(queue_t *q, struct mtp_msg *m)
{
	return (0);
}

/**
 * mtp_recv_tfc: - TFC - Transfer Controlled
 * @q: active queue
 * @m: decoded message
 *
 * Multiple congestion priorities:
 * -----------------------------------
 * 13.7.2   A transfer-controlled message relating to a given destination X is
 * sent from a signalling transfer point Y to a received message originating
 * from signalling point Z destined to signalling point X when the congestion
 * priority of the concerned message is less than the current congestion
 * status of the signalling link selected to transmit the concerned message
 * from Y to X.
 *
 * In this case, the transfer-controlled message is sent to the originating
 * point Z with the congestion status filed set to the current congestion
 * status of the signalling link.
 *
 * 13.7.3.  When the originating signalling point Z receives a
 * transfer-controlled message relating to destination X, it the current
 * congestion status of the signalling route set toward destination X is less
 * than the congestion status in the transfer-controlled message, it updates
 * the congestion status of the signalling route set toward destination X with
 * the value of the congestion status carried in the transfer-controlled
 * message.
 *
 * 13.9.3.  If within T16 after sending a signalling route set congestion test
 * message, a transfer controlled message relating to the concerned
 * destination is received, the signalling point updates the congestion status
 * of the signalling route set toward the concerned destination with the value
 * of the congestion status carried in the transfer controlled message.
 * Following this, the procedures specified in 13.9.4 and 13.9.5 are
 * performed.
 *
 * Multiple congestion states:
 * -----------------------------------
 * 13.8 The only use made of the TFC procedure by the national signalling
 * network, using multiple congestion states without congestion priorities, it
 * to convey the congestion indication primitive from the SP where congestion
 * was detected to the originating SP in a transfer controlled message.
 *
 * 11.2.5 For national signalling networks using multiple signalling link
 * congestion states without congestion priority, S + 1 (1 <= S <= 3) levels
 * of route set congestion status are provided.  The procedure is the same as
 * that specified in 11.2.3 [below], except that the MTP-STATUS primitive
 * contains the congestion status as a parameter in addition to the DPC of the
 * affected destination.
 *
 * International:
 * -----------------------------------
 * 13.6 The only use of the transfer controlled procedure in the international
 * signalling network is to convey the congestion indication from the SP where
 * congestion was detected to the originating SP in a transfer controlled
 * message.
 *
 * 11.2.3.2 After the reception of a transfer controlled message, the
 * receiving signalling point informs each level 4 User Part of the affected
 * destination by means of an MTP-STATUS primitive specified in 11.2.3.1 i).
 */
static int
mtp_recv_tfc(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rs *rs;

	if ((rs = mtp_lookup_rs(q, sl, m, RS_TYPE_MEMBER))) {
		int err;
		uint newstatus, popt = sl->lk.lk->sp.loc->na.na->option.popt;

		/* 13.7.6 In some circumstances it may happen that a signalling
		   point receives a transfer-controlled message relating to a
		   destination which is already inaccessible due to previous
		   failures; in this case the transfer-controlled message is
		   ignored. */
		if (rs->state < RS_RESTART) {
			if (popt & SS7_POPT_MPLEV) {
				newstatus = m->arg.stat;
				if (rs->cong_status >= newstatus)
					goto discard;
				rs_timer_stop(rs, t15);
				rs_timer_stop(rs, t16);
			} else if (popt & SS7_POPT_MCSTA) {
				newstatus = m->arg.stat;
			} else {
				newstatus = -1;
			}
			if ((err = mtp_cong_status_ind_all_local(q, rs, newstatus)))
				return (err);
			if (!(popt & SS7_POPT_MPLEV) && !rs->cong_status) {
				rs->cong_msg_count = 0;	/* new congestion, zero
							   counts */
				rs->cong_oct_count = 0;	/* new congestion, zero
							   counts */
			}
			rs->cong_status = newstatus;
			if (popt & SS7_POPT_MPLEV)
				rs_timer_start(q, rs, t15);
		}
		goto discard;
	}
	rare();
      discard:
	return (0);
}

/**
 * mtp_recv_tfp: - RECV TFP
 * @q: active queue
 * @m: decoded message
 *
 * 13.5.2  A signalling-route-set-test message is sent from a signalling point
 * after a transfer-prohibited or transfer-restricted message is received from
 * an adjacent signalling transfer point (see 13.2.4 and
 *
 * 13.4.4).  In this case, a signalling-route-set-test message is sent to that
 * signalling transfer point referring to the destination declared
 * inaccessible or restricted by the transfer-prohibited or
 * transfer-restricted message, every T10 period (see clause 16) until a
 * transfer-allowed message, indicating that the destination has become
 * accessible, is received.
 *
 * 13.2.4  In some circumstances it may happen that a signalling point
 * receives either a repeated transfer-prohibited message relating to a
 * non-existent route (i.e., there is no route from that signalling point to
 * the concerned destination via signalling transfer point Y, according to the
 * signalling network configuration) or to a destination which is already
 * inaccessible, due to previous failures; in this case, no actions are taken.
 */
static int
mtp_recv_tfp(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt(q, sl, m, RT_TYPE_MEMBER))) {
		int err;

		if ((rt->flags & RTF_PROHIBITED)
		    || (err = rt_set_state(q, rt, RT_PROHIBITED)) >= 0)
			goto discard;
		return (err);
	}
	mi_strlog(q, 0, SL_ERROR, "TFP received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_tcp: - RECV TCP
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_tcp(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt(q, sl, m, RT_TYPE_CLUSTER))) {
		int err;

		if ((rt->state & RTF_PROHIBITED)
		    || (err = rt_set_state(q, rt, RT_PROHIBITED)) >= 0)
			goto discard;
		return (err);
	}
	mi_strlog(q, 0, SL_ERROR, "TCP received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_tfr: - RECV TFR
 * @q: active queue
 * @m: decoded message
 *
 * 13.5.2  A signalling-route-set-test message is sent from a signalling point
 * after a transfer-prohibited or transfer-restricted message is received from
 * an adjacent signalling transfer point (see 13.2.4 and 13.4.4).  In this
 * case, a signalling-route-set-test message is sent to that signalling
 * transfer point referring to the destination declared inaccessible or
 * restricted by the transfer-prohibited or transfer-restricted message, every
 * T10 period (see clause 16) until a transfer-allowed message, indicating
 * that the destination has become accessible, is received.
 *
 * 13.4.4  In some circumstances, it may happen that a signalling point
 * receives either a repeated transfer-restricted message or a
 * transfer-restricted message message relating to a non-existent route (i.e.
 * there is not route from that signalling point to the concerned destination
 * via signalling transfer point Y, according to the signalling network
 * configuration); in this case, no actions are taken.
 */
static int
mtp_recv_tfr(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt(q, sl, m, RT_TYPE_MEMBER))) {
		int err;

		if ((rt->state & RTF_RESTRICTED)
		    || (err = rt_set_state(q, rt, RT_RESTRICTED)) >= 0)
			goto discard;
		return (err);
	}
	mi_strlog(q, 0, SL_ERROR, "TFR received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_tcr: - RECV TCR
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_tcr(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt(q, sl, m, RT_TYPE_CLUSTER))) {
		int err;

		if ((rt->state & RTF_RESTRICTED)
		    || (err = rt_set_state(q, rt, RT_RESTRICTED)) >= 0)
			goto discard;
		return (err);
	}
	mi_strlog(q, 0, SL_ERROR, "TCR received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_tfa: - RECV TFA
 * @q: active queue
 * @m: decoded message
 *
 * 13.3.2  A transfer-allowed message relating to a given destination X is
 * sent from a signalling transfer point Y in the following cases:
 *
 * i)  When signalling transfer point Y stops routing (at changeback or
 *     controlled re-routing), signalling traffic destination to signalling
 *     point X via a signalling transfer point Z (to which the concerned
 *     traffic was previously delivered as a consequence of changeover or
 *     forced rerouting).  In this case the transfer-allowed message is sent
 *     to signalling transfer point Z.
 *
 * ii) When signalling transfer point Y recognizes that it is again able to
 *     transfer signalling traffic destined to signalling point Y (see 6.2.3
 *     and 8.2.3).  In this case a transfer-allowed message is sent to all
 *     accessible adjacent signalling points, except those signalling points
 *     that receive a transfer-prohibited message according to 13.2.2 i) and
 *     except signalling point X if it is an adjacent signalling point.
 *     (Broadcast Method).
 *
 * 13.3.4  In some circumstances it may happen that a signalling point
 * receives either a repeated transfer-allowed message or a transfer allowed
 * message relating to a non-existent signalling route (i.e. there is not
 * route from that signalling point to the concerned destination via
 * signalling transfer point Y according to the signalling network
 * configuration); in this case no actions are taken.
 */
static int
mtp_recv_tfa(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt(q, sl, m, RT_TYPE_MEMBER))) {
		int err;

		if ((rt->state & RTF_ALLOWED)
		    || (err = rt_set_state(q, rt, RT_ALLOWED)) >= 0)
			goto discard;
		return (err);
	}
	mi_strlog(q, 0, SL_ERROR, "TFA received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_tca: - RECV TCA
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_tca(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt(q, sl, m, RT_TYPE_CLUSTER))) {
		int err;

		if ((rt->state & RTF_ALLOWED)
		    || (err = rt_set_state(q, rt, RT_ALLOWED)) >= 0)
			goto discard;
		return (err);
	}
	mi_strlog(q, 0, SL_ERROR, "TCA received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_rst: - RECV RST
 * @q: active queue
 * @m: decoded message
 *
 * 13.5.4  At the reception of a signalling route set test message, a
 * signalling transfer point will compare the status of the destination to the
 * received message with the actual status of the destination.  If they are
 * the same, no further action is taken.  If they are the same, no action is
 * taken, If they are different, one of the following message is sent in
 * response, dictated by the actual status of the destination:
 *
 * - a transfer-allowed message, referring to the destination the
 *   accessibility of which is tested, if the signalling transfer point can
 *   reach the indicated destination via a signalling link not connected to
 *   the signalling point from which the signalling route set test message was
 *   received (and if the transfer restricted procedure is used in the the
 *   network, the signalling link is on the normal route or an equally
 *   efficient alternative route);
 *
 * - a transfer-restricted message when access to the destination is possible
 *   via an alternative to the normal routing which is less efficient, but
 *   still not via the signalling point from which
 *   the signalling route set test was originated;
 *
 * - a transfer-prohibited message in all other cases (including the
 *   inaccessibility of the destination).
 *
 * 9.6.3  A signalling route test message received in a restarting MTP is
 * ignored during the MTP restart procedure.
 *
 * Signalling route set test messages received in a signalling point adjacent
 * to signalling point Y whose MTP is restarting before T21 expires are
 * handled, but the replies assume that all signalling routes using Y are
 * prohibited.
 */
static int
mtp_recv_rst(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt_test(q, sl, m, RT_TYPE_MEMBER))) {
		struct lk *lk = sl->lk.lk;
		struct sp *sp = lk->sp.loc;
		struct rl *rl = rt->rl.rl;
		struct rs *rs = rl->rs.rs;

		/* treate a non-primary route that we are using as the current route 
		   (C-Links) as prohibited */
		if (rs->rl.curr == rl && rs->rl.list != rl)
			goto prohibited;
		if (rs->flags & RSF_ALLOWED)
			return mtp_send_tfa(q, sp, lk->ni, m->opc, sp->pc, 0,
					    rs->dest);
		else if (rs->flags & RSF_RESTRICTED)
			return mtp_send_tfr(q, sp, lk->ni, m->opc, sp->pc, 0,
					    rs->dest);
		else if (rs->flags & RSF_PROHIBITED)
		      prohibited:
			goto discard;
		else
			goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "RST received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_rsr: - RECV RSR
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_rsr(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt_test(q, sl, m, RT_TYPE_MEMBER))) {
		struct lk *lk = sl->lk.lk;
		struct sp *sp = lk->sp.loc;
		struct rl *rl = rt->rl.rl;
		struct rs *rs = rl->rs.rs;

		/* treate a non-primary route that we are using as the current route 
		   (C-Links) as prohibited */
		if (rs->rl.curr == rl && rs->rl.list != rl)
			goto prohibited;
		if (rs->flags & RSF_ALLOWED)
			return mtp_send_tfa(q, sp, lk->ni, m->opc, sp->pc, 0,
					    rs->dest);
		else if (rs->flags & RSF_RESTRICTED)
			goto discard;
		else if (rs->flags & RSF_PROHIBITED)
		      prohibited:
			return mtp_send_tfp(q, sp, lk->ni, m->opc, sp->pc, 0,
					    rs->dest);
		else
			goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "RSR received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_rcp: - RECV RCP
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_rcp(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt_test(q, sl, m, RT_TYPE_CLUSTER))) {
		struct lk *lk = sl->lk.lk;
		struct sp *sp = lk->sp.loc;
		struct rl *rl = rt->rl.rl;
		struct rs *rs = rl->rs.rs;

		/* treate a non-primary route that we are using as the current route 
		   (C-Links) as prohibited */
		if (rs->rl.curr == rl && rs->rl.list != rl)
			goto prohibited;
		if (rs->flags & RSF_ALLOWED)
			return mtp_send_tca(q, sp, lk->ni, m->opc, sp->pc, 0,
					    rs->dest);
		else if (rs->flags & RSF_RESTRICTED)
			return mtp_send_tcr(q, sp, lk->ni, m->opc, sp->pc, 0,
					    rs->dest);
		else if (rs->flags & RSF_PROHIBITED)
		      prohibited:
			goto discard;
		else
			goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "RCP received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_rcr: 0 RECV RCR
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_rcr(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_rt_test(q, sl, m, RT_TYPE_CLUSTER))) {
		struct lk *lk = sl->lk.lk;
		struct sp *sp = lk->sp.loc;
		struct rl *rl = rt->rl.rl;
		struct rs *rs = rl->rs.rs;

		/* treate a non-primary route that we are using as the current route 
		   (C-Links) as prohibited */
		if (rs->rl.curr == rl && rs->rl.list != rl)
			goto prohibited;
		if (rs->flags & RSF_ALLOWED)
			return mtp_send_tca(q, sp, lk->ni, m->opc, sp->pc, 0,
					    rs->dest);
		else if (rs->flags & RSF_RESTRICTED)
			goto discard;
		else if (rs->flags & RSF_PROHIBITED)
		      prohibited:
			return mtp_send_tcp(q, sp, lk->ni, m->opc, sp->pc, 0,
					    rs->dest);
		else
			goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "RCR received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_lin: - RECV LIN
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_lin(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "LIN received for unknown signalling link");
      discard:
	return (0);
}

/**
 * mtp_recv_lun: RECV LUN
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_lun(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "LUN received for unknown signalling link");
      discard:
	return (0);
}

/**
 * mtp_recv_lia: - RECV LIA
 * @q: active queue
 * @m: decoded message
 *
 * ANSI T1.111.4-2000  10.2.  Inihibiting Initiation and Actions.  When at
 * signalling point X, a request is received from a management function to
 * inhibit a signalling link to signalling point Y, the following actions take
 * place:
 *
 * ...
 *
 * (5) On receiving an inhibit acknowledgement message, signalling point X
 *     marks the link locally inhibited, informs management that the link is
 *     inhibited, and starts inhibit test procedure timer T20. [10]  If the
 *     link concerned is currently carrying traffic, signalling point X
 *     diverts subsequent traffic for the link, using the time controlled
 *     changeover procedure.
 *
 * (6) When changeover has been completed, the link while inhibited, is
 *     unavailable for the transfer of user-generated traffic but still
 *     permits the exchange of test messages.
 *
 */
static int
mtp_recv_lia(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		fixme(("Write this function\n"));
		sl_timer_start(q, sl, t20a);
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "LIA received for unknown signalling link");
      discard:
	return (0);
}

/**
 * mtp_recv_lua: - RECV LUA
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_lua(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "LUA received for unknown signalling link");
      discard:
	return (0);
}

/**
 * mtp_recv_lid: RECV LID
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_lid(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "LID received for unknown signalling link");
      discard:
	return (0);
}

/**
 * mtp_recv_lfu: - RECV LFU
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_lfu(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "LFU received for unknown signalling link");
      discard:
	return (0);
}

/**
 * mtp_recv_llt: - RECV LLT
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_llt(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "LLT received for unknown signalling link");
      discard:
	return (0);
}

/**
 * mtp_recv_lrt: - RECV LRT
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_lrt(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);

	if ((sl = mtp_lookup_sl(q, sl, m))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "LRT received for unknown signalling link");
      discard:
	return (0);
}

/**
 * mtp_recv_tra: - RECV TRA
 * @q: active queue
 * @m: decoded message
 *
 * ANSI T1.111.4/2000 9.4 (Actions in Signalling Point X on Receipt of an
 * Unexpected TRA or TRW Message.)  If an unexpected traffic restart allowed
 * message or traffic restart waiting message is received from an adjacent
 * point,
 *
 * (1) If the receiving point has no transfer function it returns a traffic
 *     restart allowed message to the adjacent point from which the unexpected
 *     traffic restart allowed or traffic restart waiting message was received
 *     and starts time T29 concerning that point.
 *
 * (2) If the receiving point has the transfer function function, it starts
 *     timer T30, sends a traffic restart waiting message followed by the
 *     necessary transfer-restricted and transfer-prohibited messages
 *     (preventative transfer prohibited messages according to 13.2.2 (1) are
 *     required for traffic currently being routed via the point from which
 *     the unexpected traffic restart allowed or traffic restart waiting
 *     message was received), and a traffic restart allowed message.  It then
 *     stops T30 and starts T29.  In the abnormal case that T30 expires before
 *     the sending of transfer-prohibited and transfer-restricted messages is
 *     complete, it sends a traffic restart allowed message, starts T29, and
 *     then completes sending any preventative transfer-prohibited messages
 *     according to 13.2.2 (1) for traffic currently being routed via the
 *     point from which the unexpected traffic restart allowed or traffic
 *     restart waiting message was received.
 *
 * NOTE: A received traffic restart waiting or traffic restart allowed message
 * is not unexpected if T22, T23 or T24 is running and a direct link is in
 * service at level 2 to the point from which the message is received or if
 * T25, T28, T29 or T30 is running for the point from which the message is
 * received.
 *
 * Q.704/1996 9.5 (TRA messages and timer T19).
 *
 * 9.5.1  If a signalling point X receives an unexpected TRA message from an
 * adjacent node Y and no associated T19 timer is running, X sends to Y any
 * necessary TFP and TFR message if X has the transfer function, and a TRA
 * message to Y.  In addition, X starts timer T19 associated with Y.
 *
 * 9.5.2  If a signalling point receives a TRA message from an adjacent node
 * and an associated T19 is running, this TRA is discarded and no further
 * action is taken.
 */
static int
mtp_recv_tra(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;

	if ((rt = mtp_lookup_adj(q, sl, m))) {
		struct lk *lk = rt->lk.lk;
		struct rs *rs = rt->rs.rs;
		struct sp *sp = rs->sp.sp;
		int err;

		if ((sp->flags & SPF_RESTART) || (lk->flags & LKF_RESTART)) {
			if (lk_timer_remain(lk, t19))
				goto discard;
			if (lk_timer_remain(lk, t29a))
				goto discard;
			if (lk_timer_remain(lk, t30a))
				goto discard;
			if ((sp->flag & SPF_XFER_FUNC)) {
				/* FIXME: send necessary TFP and TFR messages. */
			}
			if ((err = mtp_send_tra()))
				return (err);
			if (ansi)
				lk_timer_start(q, lk, t29a);
			else
				lk_timer_start(q, lk, t19);
			return (0);
		} else if ((sp->flags & SPF_XFER_FUNC)) {
			/* unexpected per 9.4 with transfer function */
			if (ansi) {
				/* ANSI T1.111.4/2000 9.4 (2) If the receiving point 
				   has the transfer function function, it starts
				   timer T30, sends a traffic restart waiting
				   message followed by the necessary
				   transfer-restricted and transfer-prohibited
				   messages (preventative transfer prohibited
				   messages according to 13.2.2 (1) are required for 
				   traffic currently being routed via the point from 
				   which the unexpected traffic restart allowed or
				   traffic restart waiting message was received),
				   and a traffic restart allowed message.  It then
				   stops T30 and starts T29.  In the abnormal case
				   that T30 expires before the sending of
				   transfer-prohibited and transfer-restricted
				   messages is complete, it sends a traffic restart
				   allowed message, starts T29, and then completes
				   sending any preventative transfer-prohibited
				   messages according to 13.2.2 (1) for traffic
				   currently being routed via the point from which
				   the unexpected traffic restart allowed or traffic
				   restart waiting message was received. */
				if (!lk_timer_remain(lk, t29a)) {
					if (!lk_timer_remain(lk, t30a)) {
						if ((err =
						     mtp_send_trw(q, sp, lk->ni,
								  m->opc, m->dpc,
								  m->sls)))
							return (err);
						lk_timer_start(q, lk, t30a);
					}
					for (rs = sp->rs.list; rs; rs = rs->sp.next) {
						if (rs->rl.curr->rt.curr->lk.lk ==
						    lk) {
							if ((err =
							     mtp_send_tfp(q,...)))
								return (err);
							continue;
						}
						switch (rs->state) {
						case MTP_ALLOWED:
							continue;
						case MTP_DANGER:
						case MTP_CONGESTED:
						case MTP_RESTRICTED:
							if ((err =
							     mtp_send_tfr(q,...)))
								return (err);
							continue;
						default:
							swerr();
						case MTP_RESTART:
						case MTP_PROHIBITED:
						case MTP_INHIBITED:
						case MTP_BLOCKED:
						case MTP_INACTIVE:
							if ((err =
							     mtp_send_tfp(q,...)))
								return (err);
							continue;
						}
					}
					lk_timer_stop(lk, t30a);
					lk_timer_start(q, lk, t29a);
				}
			} else {
				/* Q.704/1996 9.5.1 If a signalling point X receives 
				   an unexpected TRA message from an adjacent node Y 
				   and no associated T19 timer is running, X sends
				   to Y any necessary TFP and TFR messages if X has
				   the transfer function, and a TRA message to Y. In 
				   addition X starts a timer T19 associated with Y.
				   9.5.2 If a signalling point receives a TRA
				   message from an adjacent node and an associated
				   timer T19 is running, this TRA is discarded and
				   no further action is taken. */
				if (!lk_timer_remain(lk, t19)) {
					for (rs = sp->rs.list; rs; rs = rs->sp.next) {
					}
					if ((err =
					     mtp_send_tra(q, sp, lk->ni, m->opc,
							  m->dpc, m->sls)))
						return (err);
					lk_timer_start(q, lk, t19);
				}
			}
			goto discard;
		} else {
			/* unexpected per 9.4 without transfer function */
			if (ansi) {
				/* ANSI T.111.4/2000 9.4 (1) If the receiving point
				   has no transfer function it returns a traffic
				   restart allowed message to the adjacent point
				   from which the unexpected traffic restart allowed 
				   or traffic restart waiting message was received
				   and starts time T29 concerning that point. */
				if (!lk_timer_remain(lk, t29a)) {
					if ((err =
					     mtp_send_tra(q, sp, lk->ni, m->opc,
							  m->dpc, m->sls)))
						return (err);
					lk_timer_start(q, lk, t29a);
				}
			} else {
				/* Q.704/1996 9.5.1 If a signalling point X receives 
				   an unexpected TRA message from an adjacent node Y 
				   and no associated T19 timer is running, X sends
				   ... a TRA message to Y.  In addition, X starts a
				   timer T19 associated with Y. 9.5.2 If a
				   signalling point receives a TRA message from an
				   adjacent node and an associated timer T19 is
				   running, this TRA is discarded and no further
				   action is taken. */
				if (!lk_timer_remain(lk, t19)) {
					if ((err =
					     mtp_send_tra(q, sp, lk->ni, m->opc,
							  m->dpc, m->sls)))
						return (err);
					lk_timer_start(q, lk, t19);
				}
			}
			goto discard;
		}
	}
	mi_strlog(q, 0, SL_ERROR, "TRA received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_trw: - RECV TRW
 * @q: active queue
 * @m: decoded message
 *
 * ANSI T1.111.4/2000 9.4 (Actions in Signalling Point X on Receipt of an
 * Unexpected TRA or TRW Message.)  If an unexpected traffic restart allowed
 * message or traffic restart waiting message is received from an adjacent
 * point,
 *
 * (1) If the receiving point has no transfer function it returns a traffic
 *     restart allowed message to the adjacent point from which the unexpected
 *     traffic restart allowed or traffic restart waiting message was received
 *     and starts time T29 concerning that point.
 *
 * (2) If the receiving point has the transfer function function, it starts
 *     timer T30, sends a traffic restart waiting message followed by the
 *     necessary transfer-restricted and transfer-prohibited messages
 *     (preventative transfer prohibited messages according to 13.2.2 (1) are
 *     required for traffic currently being routed via the point from which
 *     the unexpected traffic restart allowed or traffic restart waiting
 *     message was received), and a traffic restart allowed message.  It then
 *     stops T30 and starts T29.  In the abnormal case that T30 expires before
 *     the sending of transfer-prohibited and transfer-restricted messages is
 *     complete, it sends a traffic restart allowed message, starts T29, and
 *     then completes sending any preventative transfer-prohibited messages
 *     according to 13.2.2 (1) for traffic currently being routed via the
 *     point from which the unexpected traffic restart allowed or traffic
 *     restart waiting message was received.
 *
 * NOTE: A received traffic restart waiting or traffic restart allowed message
 * is not unexpected if T22, T23 or T24 is running and a direct link is in
 * service at level 2 to the point from which the message is received or if
 * T25, T28, T29 or T30 is running for the point from which the message is
 * received.
 *
 * ANSI T1.111.4/2000 9.3 ... (1) If a TRW message is received from Y while
 * T28 is running, or before it is started, X starts T25.  X stops T28 if it
 * is running.  (2) If a TRW message is received from Y while T25 is running,
 * X restarts T25.
 *
 * IMPLEMENTATION NOTES:-
 * - TRW is only sent from an signalling point to an adjacent signalling
 *   point, only on a direct link set.
 * - TRW message is addressed to the remote adjacent signalling point from the
 *   local adjacent signalling point.
 * - TRW is only sent by a signalling point having the transfer function (i.e.
 *   and STP).
 * - TRW is sent followed by transfer-prohibited and transfer-restricted
 *   messages, followed by TRA.
 * - TRW is sent by the adjacent signalling point only when it, or the local
 *   signalling point, or both, are restarting (i.e. after the first
 *   signalling link in the direct link set becomes available at level 2).
 *
 * mtp_lookup_rt() performs appropriate screening for TRW and TRA messages.
 */
static int
mtp_recv_trw(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rt *rt;
	int err;

	/* mtp_lookup_rt() checks the following: - the message arrives on a direct
	   link set - the message is address from the adjacent signalling point to
	   the local signalling poin - the message is address from the adjacent
	   signalling point for the arriving link set - the adjacent signalling
	   point has the transfer function */
	if ((rt = mtp_lookup_adj(q, sl, m))) {
		struct lk *lk = rt->lk.lk;
		struct rs *rs = rt->rs.rs;
		struct sp *sp = rs->sp.sp;

		/* Note: lk is the direct link set to the adjacent signalling point
		   sending the TRW message. */

		/* IMPLEMENTATION NOTE: When T22, T23 or T24 is running the RESTART
		   flag is set on the signalling point; when T25, T28, T29 or T30 is 
		   running, the RESTART flag is set on the link; when neither flag
		   is set, the TRW is unexpected. */

		if ((sp->flags & SPF_RESTART) || (lk->flags & LKF_RESTART)) {
			if (ansi) {
			} else {
			}
			if (sp_timer_remain(sp, t22a)) {
			} else if (sp_timer_remain(sp, t23a)) {
			} else if (sp_timer_remain(sp, t24a)) {
			} else if (lk_timer_remain(lk, t25a)) {
				lk_timer_start(q, lk, t25a);
				return (0);
			} else if (lk_timer_remain(lk, t28a)) {
				/* FIXME: start sending messages */
				lk_timer_stop(lk, t28a);
				lk_timer_start(lk, t25a);
				return (0);
			} else if (lk_timer_remain(lk, t29a)) {
			} else if (lk_timer_remain(lk, t30a)) {
			}
		} else if ((sp->flags & SPF_XFER_FUNC)) {
			/* unexpected per 9.4 with transfer function */
/*
 * ANSI T1.111.4/2000 9.4 (2) If the receiving point has the transfer function
 * function, it starts timer T30, sends a traffic restart waiting message
 * followed by the necessary transfer-restricted and transfer-prohibited
 * messages (preventative transfer prohibited messages according to 13.2.2 (1)
 * are required for traffic currently being routed via the point from which
 * the unexpected traffic restart allowed or traffic restart waiting message
 * was received), and a traffic restart allowed message.  It then stops T30
 * and starts T29.  In the abnormal case that T30 expires before the sending
 * of transfer-prohibited and transfer-restricted messages is complete, it
 * sends a traffic restart allowed message, starts T29, and then completes
 * sending any preventative transfer-prohibited messages according to 13.2.2
 * (1) for traffic currently being routed via the point from which the
 * unexpected traffic restart allowed or traffic restart waiting message was
 * received.
 */
			if (!lk_timer_remain(lk, t29a)) {
				if (!lk_timer_remain(lk, t30a)) {
					if ((err =
					     mtp_send_trw(q, sp, lk->ni, m->opc,
							  m->dpc, m->sls)))
						return (err);
					lk_timer_start(q, lk, t30a);
				}
				for (rs = sp->rs.list; rs; rs = rs->sp.next) {
					if (rs->rl.curr->rt.curr->lk.lk == lk) {
						if ((err = mtp_send_tfp(q,...)))
							return (err);
						continue;
					}
					switch (rs->state) {
					case MTP_ALLOWED:
						continue;
					case MTP_DANGER:
					case MTP_CONGESTED:
					case MTP_RESTRICTED:
						if ((err = mtp_send_tfr(q,...)))
							return (err);
						continue;
					default:
						swerr();
					case MTP_RESTART:
					case MTP_PROHIBITED:
					case MTP_INHIBITED:
					case MTP_BLOCKED:
					case MTP_INACTIVE:
						if ((err = mtp_send_tfp(q,...)))
							return (err);
						continue;
					}
				}
				lk_timer_stop(lk, t30a);
				lk_timer_start(q, lk, t29a);
			}
			goto discard;
		} else {
			/* unexpected per 9.4 without transfer function */
/*
 * ANSI T.111.4/2000 9.4 (1) If the receiving point has no transfer function
 * it returns a traffic restart allowed message to the adjacent point from
 * which the unexpected traffic restart allowed or traffic restart waiting
 * message was received and starts time T29 concerning that point.
 */
			if (!lk_timer_remain(lk, t29a)) {
				if ((err =
				     mtp_send_tra(q, sp, lk->ni, m->opc, m->dpc,
						  m->sls)))
					return (err);
				lk_timer_start(q, lk, t29a);
			}
			goto discard;
		}

	}
	mi_strlog(q, 0, SL_ERROR, "TRW received for unknown route");
      discard:
	return (0);
}

/**
 * mtp_recv_dlc: - RECV DLC
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_dlc(queue_t *q, struct mtp_msg *m)
{
	mi_strlog(q, 0, SL_ERROR, "DLC received (unimplemented)");
	return (0);
}

/**
 * mtp_recv_css: - RECV CSS
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_css(queue_t *q, struct mtp_msg *m)
{
	mi_strlog(q, 0, SL_ERROR, "CSS received (unimplemented)");
	return (0);
}

/**
 * mtp_recv_cns: - RECV CNS
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_cns(queue_t *q, struct mtp_msg *m)
{
	mi_strlog(q, 0, SL_ERROR, "CNS received (unimplemented)");
	return (0);
}

/**
 * mtp_recv_cnp: - RECV CNP
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_cnp(queue_t *q, struct mtp_msg *m)
{
	mi_strlog(q, 0, SL_ERROR, "CNP received (unimplemented)");
	return (0);
}

/**
 * mtp_recv_upu: - RECV UPU
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_upu(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rs *rs;

	if ((rs = mtp_lookup_rs(q, sl, m, RS_TYPE_MEMBER))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "UPU received for unknown route set");
      discard:
	return (0);
}

/**
 * mtp_recv_upa: - RECV UPA
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_upa(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rs *rs;

	if ((rs = mtp_lookup_rs(q, sl, m, RS_TYPE_MEMBER))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "UPA received for unknown route set");
      discard:
	return (0);
}

/**
 * mtp_recv_upt: - RECV UPT
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_upt(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct rs *rs;

	if ((rs = mtp_lookup_rs(q, sl, m, RS_TYPE_MEMBER))) {
		fixme(("Write this function\n"));
		goto discard;
	}
	mi_strlog(q, 0, SL_ERROR, "UPT received for unknown route set");
      discard:
	return (0);
}

/**
 * mtp_recv_sltm: - RECV SLTM
 * @q: active queue
 * @m: decoded message
 *
 * Signalling link test messages must be received on the same signalling link
 * to which they pertain and must have the correct originating and destination
 * point codes and signalling link code.  Otherwise they are discarded.
 */
static int
mtp_recv_sltm(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;

	if (m->slc != sl->slc || m->opc != adj->dest || m->dpc != loc->pc) {
		if (sl_timer_remain(sl, t1t) && m->slc == sl->slc
		    && m->opc == loc->pc && m->dpc == adj->dest)
			goto loopback;
		else
			goto eproto;
	}
	return mtp_send_slta(q, loc, lk->ni, adj->dest, loc->pc, sl->slc, sl->slc,
			     m->data, m->dlen);
      eproto:
	mi_strlog(q, 0, SL_ERROR, "invalid SLTM received");
	goto discard;
      loopback:
	mi_strlog(q, 0, SL_ERROR, "signalling link loop-back detected");
	goto discard;
      discard:
	return (0);
}

/**
 * mtp_recv_slta: - RECV SLTA
 * @q: active queue
 * @m: decoded message
 *
 * Q.707 2.2 ... The signalling link test will be considered successful only
 * if the received signalling link test acknowledgement message fullfill the
 * following criteria:
 *
 * a)  the SLC identifies the physical signalling link on which the SLTA was
 *     received.
 *
 * b)  the OPC identifies the signalling point at the other end of the link.
 *
 * c)  the test pattern is correct.
 *
 * In the case when the criteria given above are not met or a signalling link
 * test acknowledgement message is not received on the link being tested
 * within T1 (see 5.5) after the signalling link test message has been sent,
 * the test is considered to have failed and is repeated once.  In the case
 * when also the repeat test fails, the following actions have to be taken:
 *
 * - SLT applied on activation/restoration, the link is put out of service,
 *   restoration is attempted and a management system must be informed.
 *
 * - SLT applied periodically, for further study.
 *
 * (Same for ANSI.)
 */
static int
mtp_recv_slta(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int err;

	if (sl_get_l_state(sl) != SLS_IN_SERVICE
	    && sl_get_l_state(sl) != SLS_WACK_SLTM)
		goto unexpected;
	if (!(sl->flags & (SLF_WACK_SLTM | SLF_WACK_SLTM2)))
		goto unexpected;
	sl_timer_stop(sl, t1t);
	if (m->slc == sl->slc && m->opc == adj->dest && m->dpc == loc->pc) {
		int i;

		for (i = 0; i < m->dlen; i++)
			if (m->data[i] != sl->tdata[i])
				goto failed;
		if (sl_get_l_state(sl) == SLS_WACK_SLTM) {
			/* the signalling link is now able to carry traffic */
			sl_timer_stop(sl, t17);
			if (sl->lk.lk->sp.loc->flags & SPF_LOSC_PROC_B) {
				/* link is in probation */
				sl_timer_start(q, sl, t33a);
			}
			if ((err = sl_set_state(q, sl, SL_RESTORED)))
				return (err);
			/* in service at level 3 */
			sl_set_l_state(sl, SLS_IN_SERVICE);
			goto discard;
		}
		/* no further action taken on success */
		sl_timer_start(q, sl, t2t);	/* start periodic test */
		goto discard;
	}
      failed:
	if (sl->flags & SLF_WACK_SLTM) {
		/* repeat test */
		if ((err =
		     mtp_send_sltm(q, loc, lk->ni, adj->dest, loc->pc, sl->slc,
				   sl->slc, sl->tdata, sl->tlen, sl)))
			return (err);
		sl_timer_start(q, sl, t1t);
		sl->flags &= ~SLF_WACK_SLTM;
		sl->flags |= SLF_WACK_SLTM2;	/* second attempt */
		goto discard;
	}
	if (sl_get_l_state(sl) == SLS_WACK_SLTM) {
		if ((err = sl_stop_restore(q, sl)))
			return (err);
		goto discard;
	}
	sl->flags &= ~SLF_WACK_SLTM2;
	/* IMPLEMENTATION NOTE:- The action when the periodic test fails is for
	   further study. Taking the link out of service can be an especially bad
	   idea during times of congestion. This is because the buffers may be
	   filled with management messages already and the signalling link test
	   might fail due to timeout. We merely report the information to management 
	   and leave the link in service. If the test failed because of an inserted
	   loop-back while the link is in service, the link will fail rapidly due to 
	   other causes. So we do nothing more than report to management. */
	sl_timer_start(q, sl, t2t);	/* restart periodic test */
	mi_strlog(q, 0, SL_ERROR, "periodic SLT failed");
	goto discard;
      unexpected:
	mi_strlog(q, 0, SL_ERROR, "received unexpected SLTA");
	goto discard;
      discard:
	return (0);
}

/**
 * mtp_recv_ssltm: - RECV SSLTM
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_ssltm(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;

	if (m->slc != sl->slc || m->opc != adj->dest || m->dpc != loc->pc)
		goto eproto;
	return mtp_send_sslta(q, loc, lk->ni, m->opc, m->dpc, sl->slc, sl->slc,
			      m->data, m->dlen);
      eproto:
	mi_strlog(q, 0, SL_ERROR, "received invalid SSLTM");
	return (0);
}

static int
sl_ssltm_success(queue_t *q, struct sl *sl)
{
	fixme(("Write this function\n"));
	return (0);
}

/**
 * mtp_recv_sslta: - RECV SSLTA
 * @q: active queue
 * @m: decoded message
 */
static int
mtp_recv_sslta(queue_t *q, struct mtp_msg *m)
{
	struct sl *sl = SL_PRIV(q);
	struct lk *lk = sl->lk.lk;
	struct sp *loc = lk->sp.loc;
	struct rs *adj = lk->sp.adj;
	int i, err;

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
		     mtp_send_ssltm(q, loc, lk->ni, adj->dest, loc->pc, sl->slc,
				    sl->slc, sl->tdata, sl->tlen, sl)) < 0)
			return (err);
		sl_timer_start(q, sl, t1s);
		sl->flags &= ~SLF_WACK_SSLTM;
		sl->flags |= ~SLF_WACK_SSLTM2;
		goto discard;
	}
	sl->flags &= ~SLF_WACK_SSLTM2;
	if ((err = sl_ssltm_failed(q, sl)))
		return (err);
	goto discard;
      eproto:
	mi_strlog(q, 0, SL_ERROR, "received invalid SSLTA");
	goto discard;
      discard:
	return (0);
}

/**
 * mtp_recv_user: - RECV USER
 * @q: active queue
 * @m: decoded message
 *
 * Note: if we are restarting, we never get here.
 */
static int
mtp_recv_user(queue_t *q, struct mtp_msg *m)
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
static int
mtp_proc_msg(queue_t *q, struct sl *sl, struct mtp_msg *m)
{
	unsigned char tag;

	if (!
	    (sl->lk.lk->sp.loc->
	     flags & (SPF_RESTART | SPF_RESTART_PHASE_1 | SPF_RESTART_PHASE_2))) {
		switch (m->si) {
		default:	/* USER PART */
			return mtp_recv_user(q, m);
		case 0:	/* SNMM */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* coo */
				return mtp_recv_coo(q, m);
			case 0x12:	/* coa */
				return mtp_recv_coa(q, m);
			case 0x15:	/* cbd */
				return mtp_recv_cbd(q, m);
			case 0x16:	/* cba */
				return mtp_recv_cba(q, m);
			case 0x21:	/* eco */
				return mtp_recv_eco(q, m);
			case 0x22:	/* eca */
				return mtp_recv_eca(q, m);
			case 0x31:	/* rct */
				return mtp_recv_rct(q, m);
			case 0x32:	/* tfc */
				return mtp_recv_tfc(q, m);
			case 0x41:	/* tfp */
				return mtp_recv_tfp(q, m);
			case 0x42:	/* tcp */
				return mtp_recv_tcp(q, m);
			case 0x43:	/* tfr */
				return mtp_recv_tfr(q, m);
			case 0x44:	/* tcr */
				return mtp_recv_tcr(q, m);
			case 0x45:	/* tfa */
				return mtp_recv_tfa(q, m);
			case 0x46:	/* tca */
				return mtp_recv_tca(q, m);
			case 0x51:	/* rst */
				return mtp_recv_rst(q, m);
			case 0x52:	/* rsr */
				return mtp_recv_rsr(q, m);
			case 0x53:	/* rcp */
				return mtp_recv_rcp(q, m);
			case 0x54:	/* rcr */
				return mtp_recv_rcr(q, m);
			case 0x61:	/* lin */
				return mtp_recv_lin(q, m);
			case 0x62:	/* lun */
				return mtp_recv_lun(q, m);
			case 0x63:	/* lia */
				return mtp_recv_lia(q, m);
			case 0x64:	/* lua */
				return mtp_recv_lua(q, m);
			case 0x65:	/* lid */
				return mtp_recv_lid(q, m);
			case 0x66:	/* lfu */
				return mtp_recv_lfu(q, m);
			case 0x67:	/* llt */
				return mtp_recv_llt(q, m);
			case 0x68:	/* lrt */
				return mtp_recv_lrt(q, m);
			case 0x71:	/* tra */
				return mtp_recv_tra(q, m);
			case 0x72:	/* trw */
				return mtp_recv_trw(q, m);
			case 0x81:	/* dlc */
				return mtp_recv_dlc(q, m);
			case 0x82:	/* css */
				return mtp_recv_css(q, m);
			case 0x83:	/* cns */
				return mtp_recv_cns(q, m);
			case 0x84:	/* cnp */
				return mtp_recv_cnp(q, m);
			case 0xa1:	/* upu */
				return mtp_recv_upu(q, m);
			case 0xa2:	/* upa *//* ansi91 only */
				return mtp_recv_upa(q, m);
			case 0xa3:	/* upt *//* ansi91 only */
				return mtp_recv_upt(q, m);
			}
			break;
		case 1:	/* SNTM */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* sltm */
				return mtp_recv_sltm(q, m);
			case 0x12:	/* slta */
				return mtp_recv_slta(q, m);
			}
			break;
		case 2:	/* SSNTM */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* ssltm */
				return mtp_recv_ssltm(q, m);
			case 0x12:	/* sslta */
				return mtp_recv_sslta(q, m);
			}
			break;
		}
	} else {
		switch (m->si) {
		default:	/* USER PART */
			/* 9.6.6 ... All messages received during the restart
			   procedure concerning a local MTP User (service indicator
			   != 0000 and != 0001) are discarded. */
			goto discard;
		case 0:	/* SNMM */
			/* 9.6.6 ... All messages received with service indicator == 
			   0000 in a restarting MTP for the signalling point itself
			   are treated as described in the MTP restart procedure.
			   Those messages not described elsewhere in the procedure
			   are discarded and no further action is taken on them
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
				return mtp_recv_tfp(q, m);
			case 0x42:	/* tcp */
				return mtp_recv_tcp(q, m);
			case 0x43:	/* tfr */
				return mtp_recv_tfr(q, m);
			case 0x44:	/* tcr */
				return mtp_recv_tcr(q, m);
			case 0x45:	/* tfa */
				return mtp_recv_tfa(q, m);
			case 0x46:	/* tca */
				return mtp_recv_tca(q, m);
			case 0x51:	/* rst */
			case 0x52:	/* rsr */
			case 0x53:	/* rcp */
			case 0x54:	/* rcr */
				/* 9.6.3 A signalling route set test message
				   received in a restarting MTP is ignored during
				   the MTP restart procedure. */
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
				return mtp_recv_tra(q, m);
			case 0x72:	/* trw */
				return mtp_recv_trw(q, m);
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
			/* 9.6.7 ... Messages received with service indicator ==
			   0001 are handled normally during the restart procedure. */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* sltm */
				return mtp_recv_sltm(q, m);
			case 0x12:	/* slta */
				return mtp_recv_slta(q, m);
			}
			break;
		case 2:	/* SSNTM */
			tag = ((m->h0 & 0x0f) << 4) | (m->h1 & 0x0f);
			switch (tag) {
			case 0x11:	/* ssltm */
				return mtp_recv_ssltm(q, m);
			case 0x12:	/* sslta */
				return mtp_recv_sslta(q, m);
			}
			break;
		}
	}
	__ptrace(("%s: %p: DECODE: Invalid message type\n", DRV_NAME, sl));
	return (-EPROTO);
      discard:
	return (0);
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
static int
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
static int
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
static int
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
static int
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
		m->dest =
		    (*mp->b_rptr++) | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16);
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
static int
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
		m->dest =
		    (*mp->b_rptr++) | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16);
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
static int
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
		    (*mp->b_rptr++ >> 4) | (*mp->
					    b_rptr++ << 4) | ((*mp->
							       b_rptr++ & 0x03) <<
							      12);
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
static int
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
		m->dest =
		    ((*mp->b_rptr++)) | ((*mp->b_rptr++ << 8)) | ((*mp->
								   b_rptr++ << 16));
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
static int
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
static int
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
static int
mtp_dec_sio(mblk_t *mp, struct mtp_msg *m)
{
	/* decode si, mp and ni */
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
static int
mtp_dec_rl(mblk_t *mp, struct mtp_msg *m)
{
	/* decode the routing label */
	switch (m->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ETSI:
	case SS7_PVAR_ITUT:
		/* 14-bit point codes - 32-bit RL */
		if (mp->b_wptr < mp->b_rptr + 4)
			break;
		m->dpc = (*mp->b_rptr++ | ((*mp->b_rptr & 0x3f) << 8));
		m->opc =
		    ((*mp->b_rptr++ >> 6) | (*mp->
					     b_rptr++ << 2) | ((*mp->
								b_rptr & 0x0f) <<
							       10));
		m->sls = (*mp->b_rptr++ >> 4) & 0x0f;
		return (0);
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		/* 24-bit point codes - 56-bit RL */
		if (mp->b_wptr < mp->b_rptr + 7)
			break;
		m->dpc =
		    (*mp->b_rptr++ | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16));
		m->opc =
		    (*mp->b_rptr++ | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16));
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
static int
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
static int
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
	return (0);
      error:
	mi_strlog(q, 0, SL_ERROR, "DECODE: decoding error");
	mp->b_rptr = b_rptr;	/* restore read pointer */
	return (err);
}

/**
 * sl_recv_msg: - receive signalling link message
 * @q: active queue (lower read queue)
 * @sl: receiving signalling link
 * @mp: the message
 *
 * 9.6.6  All messages to another destination received at a signalling point
 * whose MTP is restarting are discarded.
 */
static int
sl_recv_msg(queue_t *q, struct sl *sl, mblk_t *mp)
{
	struct mtp_msg msg = { NULL, };
	struct lk *lk = sl->lk.lk;	/* the link (set) on which the message
					   arrived */
	struct rs *adj = lk->sp.adj;	/* the route set to the adjacent of the
					   arriving link */
	struct sp *loc = lk->sp.loc;	/* the local signalling point the message
					   arrived for */
	struct na *na = loc->na.na;	/* the local network appearance, protocol
					   and options */
	int err;

	mblk_t *dp = (DB_TYPE(mp) == M_PROTO) ? mp->b_cont : mp;

	if (unlikely((err = mtp_dec_msg(q, dp, &msg, na))))
		goto error;

	if (lk->ni != msg.ni)
		/* network indicator must match */
		goto screened;
	if (msg.opc == adj->dest)
		/* always listen to adjacent signalling point */
		goto passed;
	if (!(adj->flags & RSF_XFER_FUNC))
		/* originator not adjacent, adjacent must be STP */
		goto screened;
	if (!mtp_lookup_rt_local(sl, msg.opc, RT_TYPE_MEMBER)) {
		/* do not accept from originators to which we don't have a local
		   route on the receiving signalling link */
		fixme(("Must also check for circular routes\n"));
		goto screened;
	}
      passed:
	if (loc->pc == msg.dpc) {
		/* message is for us, process it */
		todo(("Also check local aliases\n"));
		if ((err = mtp_proc_msg(q, sl, &msg)) < 0)
			return (err);
		freemsg(mp);
		return (0);
	}
	if ((loc->flags & SPF_XFER_FUNC)) {
		/* message is not for us, transfer it */
		if ((err = mtp_xfer_route(q, sl, mp, &msg)) < 0)
			return (err);
		freemsg(mp);
		return (0);
	}
	/* if we do not transfer, we cannot process messages not for us */
	goto screened;
      screened:
	todo(("Deliver screened message to MGMT\n"));
	mi_strlog(q, 0, SL_ERROR, "discard: message screened");
	goto discard;
      error:
	mi_strlog(q, 0, SL_ERROR, "discard: message decode error %d", err);
	goto discard;
      discard:
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from below
 *
 *  -------------------------------------------------------------------------
 */
/**
 * sl_data: - process M_DATA
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * When we receive a PDU from below, we want to decode it and then determine
 * what to do based on the decoding of the message.
 */
static int
sl_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	if ((1 << sl_get_l_state(sl)) &
	    ~(SLSF_IN_SERVICE | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	return sl_recv_msg(q, sl, mp);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "M_DATA: would place i/f out of state");
	return (EPROTO);
}

/**
 * sl_pdu_ind: - process SL_PDU_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * When we receive a PDU from below, we want to decode it and then determine
 * what to do based on the decoding of the message.
 */
static int
sl_pdu_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_pdu_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p) || !mp->b_cont)
		goto einval;
	if ((1 << sl_get_l_state(sl)) &
	    ~(SLSF_IN_SERVICE | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	return sl_recv_msg(q, sl, mp->b_cont);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "SL_PDU_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "SL_PDU_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_link_congested_ind: - process SL_LINK_CONGESTED_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * We keep track of overall link congestion status primarily so that we can
 * restrict routes when link congestion onsets.  This ensures that when a link
 * set becomes congestion (presumably from loss of links or general signalling
 * overload) that we attempt to reroute what traffic we can to another
 * available route.  Also, this is necessary for triggering congestion related
 * transfer restricted procedure in an STP than sends TFR.
 */
static int
sl_link_congested_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int old_status, new_status;
	const sl_link_cong_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << sl_get_l_state(sl)) &
	    ~(SLSF_IN_SERVICE | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	old_status = sl->cong_status;
	sl->cong_status = p->sl_cong_status <= 3 ? p->sl_cong_status : 3;
	new_status = sl->cong_status;
	sl->disc_status = p->sl_disc_status <= 3 ? p->sl_disc_status : 3;
	if (!old_status && new_status) {
		sl_set_state(q, sl, SL_CONGESTED);
		/* T31(ANSI) is a false congesiton detection timer */
		sl_timer_start(q, sl, t31a);
	}
	if (!new_status && old_status) {
		sl_set_state(q, sl, SL_UNCONGESTED);
		/* T31(ANSI) is a false congesiton detection timer */
		sl_timer_stop(sl, t31a);
	}
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_LINK_CONGESTED_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "SL_LINK_CONGESTED_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_link_congestion_ceased_ind: - process SL_LINK_CONGESTION_CEASED_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * We keep track of overall link congestion status primarily so that we can
 * unrestrict routes when link congestion abates.  This ensures that when a
 * link set becomes congested (presumably from loss of links or general
 * signalling overload) that we attempt to reroute what traffic we can to
 * another available route.  Also, this is necessary for triggering congestion
 * related transfer restricted procedure in an STP that sends TFR.
 */
static int
sl_link_congestion_ceased_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int old_status, new_status;
	const sl_link_cong_ceased_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << sl_get_l_state(sl)) &
	    ~(SLSF_IN_SERVICE | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	old_status = sl->cong_status;
	sl->cong_status = p->sl_cong_status <= 3 ? p->sl_cong_status : 3;
	new_status = sl->cong_status;
	sl->disc_status = p->sl_disc_status <= 3 ? p->sl_disc_status : 3;
	if (!new_status && old_status) {
		sl_set_state(q, sl, SL_UNCONGESTED);
		/* T31(ANSI) is a false congesiton detection timer */
		sl_timer_stop(sl, t31a);
	}
	if (!old_status && new_status) {
		sl_set_state(q, sl, SL_CONGESTED);
		/* T31(ANSI) is a false congesiton detection timer */
		sl_timer_start(q, sl, t31a);
	}
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_LINK_CONGESTION_CEASED_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_LINK_CONGESTION_CEASED_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_retrieved_message_ind: - process SL_RETRIEVED_MESSAGE_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * As messages are retrieved, we simply tack the messages onto the end of the
 * retrieval buffer.  If we are not in the retrieval state, then we discard
 * the messages and complain.
 */
static int
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
		/* When the SLS was rotated for transmission, we need to unrotate on 
		   retrieval. */
		switch (sl->lk.lk->ls.ls->sp.sp->na.na->option.pvar) {
		case SS7_PVAR_ANSI:
		case SS7_PVAR_JTTC:
		case SS7_PVAR_CHIN:	/* ??? */
		{
			/* XXX Actually, for 5-bit and 7-bit compatibility, we
			   rotate based on 5 sls bits regardless of the number of
			   bits in the SLS */
			uint sls = mp->b_cont->b_rptr[7];

			if (sls & 0x10)
				sls = (sls & 0xe0) | ((sls & 0x0f) << 1) | 0x01;
			else
				sls = (sls & 0xe0) | ((sls & 0x0f) << 1) | 0x00;
			/* rewrite sls in routing label */
			mp->b_cont->b_rptr[7] = sls;
		}
		}
	}
	bufq_queue(&sl->rbuf, mp->b_cont);
	mp->b_cont = NULL;
	return (0);
      efault:
	mi_strlog(q, 0, SL_ERROR, "SL_RETRIEVED_MESSAGE_IND: no data");
	return (EFAULT);
      outstate:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_RETRIEVED_MESSAGE_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_RETRIEVED_MESSAGE_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_retrieval_complete_ind: - process SL_RETRIEVAL_COMPLETE_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 *
 * 5.5 Retrieval and diversion of traffic
 *
 * When the procedure to update the retransmission buffer content is
 * completed, the following actions are performed: the routing of the
 * signalling traffic to be diverted is changed; the signal traffic already
 * stored in the transmission buffers and retransmission buffer of the
 * unavailable signalling link is sent directly toward the new signalling
 * link(s), according to the modified routing.
 *
 * The diverted signalling traffic will be sent toward the new signalling
 * link(s) in such a way that the correct message sequence is maintained.  The
 * diverted traffic has no priority in relation to normal traffic already
 * conveyed on the signalling link(s).
 */
static int
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
			/* When the SLS was rotated for transmission, we need to
			   unrotate on retrieval. */
			switch (na->option.pvar) {
			case SS7_PVAR_ANSI:
			case SS7_PVAR_JTTC:
			case SS7_PVAR_CHIN:	/* ??? */
			{
				/* XXX Actually, for 5-bit and 7-bit compatibility,
				   we rotate based on 5 sls bits regardless of the
				   number of bits in the SLS */
				uint sls = mp->b_cont->b_rptr[7];

				if (sls & 0x10)
					sls =
					    (sls & 0xe0) | ((sls & 0x0f) << 1) |
					    0x01;
				else
					sls =
					    (sls & 0xe0) | ((sls & 0x0f) << 1) |
					    0x00;
				/* rewrite sls in routing label */
				mp->b_cont->b_rptr[7] = sls;
			}
			}
		}
		bufq_queue(&sl->rbuf, mp->b_cont);
		mp->b_cont = NULL;
	}
	return sl_stop_restore(q, sl);
      outstate:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_RETRIEVAL_COMPLETE_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_RETRIEVAL_COMPLETE_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_rb_cleared_ind: - process SL_RB_CLEARED_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
sl_rb_cleared_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_rb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) == SLS_WIND_CLRB) {
		sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
		return (0);
	}
	if ((1 << sl_get_l_state(sl)) &
	    ~(SLSF_WCON_RET | SLSF_WIND_INSI | SLSF_WACK_SLTM | SLSF_PROC_OUTG))
		goto outstate;
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "SL_RB_CLEARED_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "SL_RB_CLEARED_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_bsnt_ind: - process SL_BSNT_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
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
		/* send COA */
		if ((err =
		     mtp_send_coa(q, loc, loc->ni, adj->dest, loc->pc, sl->slc,
				  sl->slc, sl->bsnt)))
			return (err);
		if (sl->fsnc == -1) {
			if ((err = sl_clear_rtb_req(q, sl)))
				return (err);
		}
		if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
			return (err);
		return sl_retrieval_request_and_fsnc_req(q, sl, sl->fsnc);
	} else {
		/* send COO */
		if ((err =
		     mtp_send_coo(q, loc, loc->ni, adj->dest, loc->pc, sl->slc,
				  sl->slc, sl->bsnt)))
			return (err);
		sl_set_l_state(sl, SLS_WACK_COO);
		return (0);
	}
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "SL_BSNT_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "SL_BSNT_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_in_service_ind: - process SL_IN_SERVICE_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * 3.3.2 Signalling link restored
 *
 * 3.3.2.1 Signalling traffic management: the changeback procedure (see clause
 * 6) is applied, if required, to divert signalling traffic from one or more
 * links to a link which has become available; it includes determination of
 * the traffic to be diverted and procedures for maintaining the correct
 * message sequence.
 *
 * 3.3.2.2 Signalling link management: the signalling link deactivation
 * procedure (see clause 12) is used if, during the signalling link failure,
 * another signalling link of the same link was activated; it is used to
 * assure that the link set status is returned to the same state as before the
 * failure.  This requires that the active link activated during the link
 * failure is deactivated and considered no longer available for signalling.
 *
 * 3.3.2.3 Signalling route management: in the case when the restoration of a
 * signalling link causes a signalling route set to become available, the
 * signalling transfer point which can once again route the concerned
 * signalling traffic applies the transfer-allowed procedures described in
 * clause 13.
 *
 * 4.4 Signalling link availability
 *
 * 4.4.1 When a previously unavailable signalling link becomes available again
 * (see 3.2), signalling traffic may be transferred to the available
 * signalling link by means of the changeback procedure.  The traffic to be
 * transferred is determined in accordance with the following criteria.
 *
 * 4.4.2 In the case when the link set to which the available signalling link
 * belongs, already carries signalling traffic on other signalling links in
 * the link set, the traffic to be transferred includes the traffic for which
 * the available signalling link is the normal one.  Note that the assignment
 * of the normal traffic to a signalling link may be changed during the
 * changeback process taking into account, for example, system performance.
 *
 * The normal traffic is transferred from one or more signalling links,
 * depending on the criteria applied when the signalling link became
 * unavailable (see 4.3.2), and upon the criteria applied if any of the
 * alternative signalling links(s) themselves became unavailable, or
 * available, in the meantime.
 *
 * If signalling links in the link set are still unavailable, and if it is
 * required for load balancing purposes, signalling traffic extra to that
 * normally carried by any link might also be identified for diversion to the
 * signalling link made available, and to other available links in the link
 * set.
 *
 * This extra traffic is transferred from one or more signalling links.
 *
 * 4.4.3  In the case when the link set (combined link set) to which the
 * available signalling links belong, does not carry any signalling traffic
 * [i.e., a link set (combined link set) has become available], the traffic to
 * be transferred is the traffic for which the available link set (combined
 * link set) has higher priority than the link set (combined link set)
 * currently used.
 *
 * The traffic is transferred from one or more link sets (combined link sets)
 * and from one or more signalling links within each link set.
 */
static int
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
	/* generate new test data */
	sl->tlen = jiffies & 0x0f;
	for (i = 0; i < sl->tlen; i++)
		sl->tdata[i] ^= ((jiffies >> 8) & 0xff);
	/* start new signalling link test */
	if ((err =
	     mtp_send_sltm(q, sp, lk->ni, rs->dest, sp->pc, sl->slc, sl->slc,
			   sl->tdata, sl->tlen, sl)))
		return (err);
	sl_timer_stop(sl, t17);
	sl_timer_start(q, sl, t1t);
	sl->flags &= ~SLF_WACK_SLTM2;
	sl->flags |= SLF_WACK_SLTM;
	sl_set_l_state(sl, SLS_WACK_SLTM);
	return (0);
      outstate:
	/* FIXME: stop the link */
	mi_strlog(q, 0, SL_ERROR, "SL_IN_SERVICE_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "SL_IN_SERVICE_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_out_of_service_ind: - process SL_OUT_OF_SERVICE_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * 3.3.1 Signalling link failed
 *
 * 3.3.1.1 Signalling traffic management: the changeover procedure (see clause
 * 5) is applied, if required, to diver signalling traffic from the
 * unavailable link to one or more alternative links with the objective of
 * avoiding message loss, repetition or mis-sequencing; it includes
 * determination of the alternative link or links where the affected traffic
 * can be transferred and procedures to retrieve messages sent over the failed
 * link but not received by the far end.
 *
 * 3.3.1.2 Signalling link management: the procedures described in clause 12
 * are used to restore a signalling link and to make it available for
 * signalling.  Moreover, depending on the link set status, the procedures can
 * also be used to activate another signalling link in the same link set to
 * which the unavailable link belongs and to make it available for signalling.
 *
 * 3.3.1.3 Signalling route management: in the case when the failure of a
 * signalling link causes a signalling route set to become unavailable or
 * restricted, the signalling transfer point which can no longer route the
 * concerned signalling traffic applies the transfer-prohibited procedures or
 * transfer-restricted procedures described in clause 13.
 *
 * 4.3 Signalling link unavailability
 *
 * 4.3.1 When a signalling link becomes unavailable, signalling traffic
 * carried by the link is transferred to one ore more alternative links by
 * means of a changeover procedure.  The alternative link or links are
 * determined in accordance with the following criteria.
 *
 * 4.3.2 In the case when there is one or more alternative signalling links
 * available in the link set to which the unavailable link belongs, the
 * signalling traffic is transferred within the link set to: a) an active and
 * unblocked signalling link, currently not carrying any traffic.  If no such
 * signalling link exists, the signalling traffic is transferred to; b) one
 * ore possibly more than one signalling link currently carrying traffic.  In
 * the case of transfer to one signalling link, the alternative signalling
 * link is that having the highest priority of the signalling links in
 * service.
 *
 * 4.3.3 In the case when there is no alternative signalling link within the
 * link set to which the unavailable signalling link belongs, the signalling
 * traffic is transferred to one or more alternative link sets (combined link
 * sets) in accordance with the alternative routing defined for each
 * destination.  For a particular destination, the alternative link set
 * (combined link set) si the link set (combined link set) in service having
 * the highest priority.
 *
 * Within a new link set, signalling traffic is distributed over the
 * signalling links in accordance with the routing currently applicable for
 * that link set; i.e. the transferred traffic is routed in the same way as
 * the traffic already using the link set.
 *
 * 5.3 Changeover initiation and actions
 *
 * 5.3.1  Changeover is initiated at a signalling point when a signalling link
 * is recognized as unavailable according to the criteria listed in 3.2.2.
 *
 * The following actions are then performed: a) transmission and acceptance of
 * message signal unites on the concerned signalling link is terminated; b)
 * transmission of link status signal units or fill in signal units, as
 * described in 5.3/Q.703 takes place; c) the alternative signalling link(s)
 * are determined according to the rules specified in clause 4; d) a procedure
 * to update the content of the retransmission buffer of the unavailable
 * signalling link is performed as specified in 5.4 below; e) signalling
 * traffic is diverted to the alternative signalling link(s) as specified in
 * 5.5 below.
 *
 * In addition, if traffic toward a given destination is diverted to an
 * alternative signalling link terminating in a signalling transfer point not
 * currently used to carry traffic toward that destination, a
 * transfer-prohibited procedure is performed as specified in 13.2.
 *
 * 5.3.2 In the case when there is no traffic to transfer from the unavailable
 * signalling link action, only item b) of 5.3.1 is required.
 *
 * 5.3.3 If no alternative signalling link exists for signalling traffic
 * toward one or more destinations, the concerned destination(s) are declared
 * inaccessible and the following actions apply: i) the routing of the
 * concerned signalling traffic is blocked and the concerned messages already
 * stored in the transmission and retransmission buffers of the unavailable
 * signalling link, as well as those received subsequently, are discarded; ii)
 * a command is sent to the User Part(s) (if any) in order to stop generating
 * the concerned signalling traffic; iii) the transfer-prohibited procedure is
 * performed, as specified in 13.2; iv) the appropriate signalling link
 * management procedures are performed, as specified in clause 12.
 *
 * 5.3.4  In some cases of failures or in some network configuration, the
 * normal buffer updating and retrieval procedures described in 5.4 and 5.5
 * cannot be accomplished.  In such cases, the emergency changeover procedures
 * described in 5.6 apply.
 *
 * Other procedures to cover possible abnormal cases appear in 5.7.
 *
 * 5.4 Buffer updating procedure
 *
 * 5.4.1  When a decision to changeover is made, a changeover order is sent to
 * the remote signalling point.  In the case that the changeover was initiated
 * by the reception of a changeover order (see 5.2), a changeover
 * acknowledgement is sent instead.
 *
 * A changeover order is always acknowledged by a changeover acknowledgement,
 * even when changeover has already been initiated in accordance with another
 * criterion.
 *
 * No priority is given to the changeover order or changeover acknowledgement
 * in relation to the normal traffic of the signalling link on which the
 * message is sent.
 *
 * 5.4.2  The changeover order and changeover acknowledgement are signalling
 * network management messages and contain the following information: the
 * label, indicating the destination and originating signalling points and the
 * identity of the unavailable signalling link; the changeover order (or
 * changeover order acknowledgement) signal; and the forward sequence number
 * of the last message signal unit accepted from the unavailable signalling
 * link.
 *
 * 5.4.3  Upon reception of a changeover order or changeover acknowledgement,
 * the retransmission buffer of the unavailable signalling link is updated
 * (except as noted in 5.6), according to the information contained in the
 * message.  The message signal units successive to that indicated by the
 * message are those which have to be retransmitted on the alternative
 * signalling link(s), according to the retrieval and diversion procedure.
 *
 * 12.2.1.2    When a decision is taken to activate an inactive signalling
 * link; initial alignment starts.  If the initial alignment procedure is
 * successful, the signalling link is active and a signalling link test is
 * started.  If the signalling link test is successful the link becomes ready
 * to convey signalling traffic.  In the case when initial alignment is not
 * possible, as determined at Message Transfer Part level 2 (see clause
 * 7/Q.703), new initial alignment procedures are started on the same
 * signalling link after a timer T17 (delay to avoid the oscillation of
 * initial alignment failure and link restart.  The value of T17 should be
 * greater than the loop delay and less than timer T2, see 7.3/Q.703).  If the
 * signalling link test fails, link restoration starts until the signalling
 * link is activated or a manual intervention is made.
 *
 * 12.2.2  Signalling link restoration
 *
 * After a signalling link failure is detected, signalling link initial
 * alignment will take place.  In the case when the initial alignment
 * procedure is successful, a signalling link test is started.  If the
 * signalling link test is successful the link becomes restored and thus
 * available for signalling.
 *
 * If the initial alignment is not possible, as determined at Message Transfer
 * Part level 2 (see clause 7/Q.703), new initial alignment procedures may be
 * started on the same signalling link after a time T17 until the signalling
 * link is restored ro a manual intervention is made, e.g. to replace the
 * signalling data link or the signalling terminal.
 *
 * If the signalling link test fails, the restoration procedure is repeated
 * until the link is
 *
 * ANSI T1.111.4 (1996 and 2000) 12.2.2.  After a signalling link failure is
 * detected, signalling link initial alignment will take place.  When the
 * initial alignment procedure is successful, a signalling link test is
 * started.  If the signalling link test is successful, the signalling link is
 * regarded as restored and thus available for signalling.
 *
 * If initial alignment is not possible, as determined at level 2 (see Section
 * 7 of Chapter T1.111.3 or Section 6.2.2 of T1.645), new initial alignment
 * procedures may be started on the same signalling link after a timer T17
 * until the signalling link is restored or a manual intervention is made,
 * e.g., to replace the signalling data link or the signalling terminal.  If
 * after time T19 the signalling link has not activated, a management function
 * is notified, and, optionally, T19 may be restarted.  restored or a manual
 * intervention is made.
 */
static int
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
	mi_strlog(q, 0, SL_ERROR,
		  "SL_OUT_OF_SERVICE_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "SL_OUT_OF_SERVICE_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_remote_processor_outage_ind: - process SL_REMOTE_PROCESSOR_OUTAGE_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * 3.3.5 Signalling link blocked
 *
 * 3.3.5.1 Signalling traffic management: as specified in 3.3.1.1.  (See
 * SL_OUT_OF_SERVICE_IND).
 *
 * As a national option, local processor outage may also be applied to the
 * affected signalling link before commencement of the appropriate signalling
 * traffic management option.  On completion of that signalling traffic
 * management action, local processor outage is removed from the affected
 * signalling link.  No further signalling traffic management will be
 * performed on that affected signalling link until a timer T24 (see 16.8) has
 * expired or been cancelled, thus allowing time for indications from the
 * remote end to stabilize as it carries out any signalling traffic management
 * of its own.
 *
 * 3.3.5.2 Signalling route management: if the blocking of the link causes a
 * signalling route set to become unavailable or restricted, the signalling
 * transfer point which can no longer route the concerned signalling traffic
 * applies the transfer-prohibited procedures described in clause 13.
 */
static int
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
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_REMOTE_PROCESSOR_OUTAGE_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_REMOTE_PROCESSOR_OUTAGE_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_remote_processor_recovered_ind: - process SL_REMOTE_PROCESSOR_RECOVERED_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * 3.3.6 Signalling link unblocked
 *
 * 3.3.6.1 Signalling traffic management: the actions will be the same as in
 * 3.3.2.1. (See SL_IN_SERIVCE_IND)
 *
 * 3.3.6.2 Signalling route management: if the link unblocked causes a
 * signalling route set to become available, the signalling transfer point
 * which can once again route the signalling traffic in that route set applies
 * the transfer-allowed procedures described in clause 13.
 */
static int
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
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_REMOTE_PROCESSOR_RECOVERED_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_REMOTE_PROCESSOR_RECOVERED_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_rtb_cleared_ind: - process SL_RTB_CLEARED_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * We receive this indication when we send sl_clear_buffers_req in a state
 * other than processor outage, or in response to a sl_clear_rtb_req after the
 * remote processor has recovered.  For ITU, sl_clear_buffers_req will place
 * us back to in service from the local processor outage state, but not for
 * ANSI.  Effectively, it a processor outage has been of a long duration (i.e.
 * T1 has expired) then we should flush buffers before resuming operation on
 * the link.  If we are failing a remotely blocked link (again because of T1
 * expiry) in ANSI, we should fail the link and then flush buffers (which will
 * result in only the RTB being flushed) before buffer updating.  Or, we could
 * choose to flush buffers completely in the processor outage state.
 */
static int
sl_rtb_cleared_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_rtb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	switch (sl_get_l_state(sl)) {
	case SLS_WIND_CLRB:
		sl_set_l_state(sl, SLS_OUT_OF_SERVICE);
		return (0);
	case SLS_WCON_RET:
		/* we oft times blindly clear RTB before retrieval */
		return (0);
	default:
		goto outstate;
	}
      outstate:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_RTB_CLEARED_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "SL_RTB_CLEARED_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_retrieval_not_possible_ind: - process SL_RETRIEVAL_NOT_POSSIBLE_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 * 
 * 5.5 Retrieval and diversion of traffic
 *
 * When the procedure to update the retransmission buffer content is
 * completed, the following actions are performed: the routing of the
 * signalling traffic to be diverted is changed; the signal traffic already
 * stored in the transmission buffers and retransmission buffer of the
 * unavailable signalling link is sent directly toward the new signalling
 * link(s), according to the modified routing.
 *
 * The diverted signalling traffic will be sent toward the new signalling
 * link(s) in such a way that the correct message sequence is maintained.  The
 * diverted traffic has no priority in relation to normal traffic already
 * conveyed on the signalling link(s).
 *
 * NOTE: we actually never send this.  Retrieval is always possible.  Where it
 * might not be possible is in the case where M2UA is used and it is not
 * possible to talk to the signalling link.   We treat this the same as a
 * SL_RETRIEVAL_COMPLETE_IND, we just may have an empty retrieval buffer.
 */
static int
sl_retrieval_not_possible_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const sl_retrieval_not_poss_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_l_state(sl) != SLS_WCON_RET)
		goto outstate;
	/* purge any partial retrieval buffer */
	bufq_purge(&sl->rbuf);
	return sl_stop_restore(q, sl);
      outstate:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_RETRIEVAL_NOT_POSSIBLE_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_RETRIEVAL_NOT_POSSIBLE_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * sl_bsnt_not_retrievable_ind: - process SL_BSNT_NOT_RETRIEVABLE_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
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
	sl->bsnt = -1;
	if (sl->flags & (SLF_COO_RECV | SLF_ECO_RECV)) {
		/* send ECA */
		if ((err =
		     mtp_send_eca(q, loc, loc->ni, adj->dest, loc->pc, sl->slc,
				  sl->slc)))
			return (err);
		if ((sl->flags & SLF_ECO_RECV) || sl->fsnc == -1) {
			/* If we were not given the FSNC from the other end, because 
			   of ECO, then we clear the retransmission buffer before
			   retrieval.  We can still retrieve the contents of the
			   transmission buffer. */
			if ((err = sl_clear_rtb_req(q, sl)))
				return (err);
			sl->fsnc = -1;
		}
		if ((err = sl_set_state(q, sl, SL_RETRIEVAL)) < 0)
			return (err);
		return sl_retrieval_request_and_fsnc_req(q, sl, sl->fsnc);
	} else {
		/* send ECO */
		if ((err =
		     mtp_send_eco(q, loc, loc->ni, adj->dest, loc->pc, sl->slc,
				  sl->slc)))
			return (err);
		sl_set_l_state(sl, SLS_WACK_ECO);
		return (0);
	}
	return (0);
      outstate:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_BSNT_NOT_RETRIEVABLE_IND: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR,
		  "SL_BSNT_NOT_RETRIEVABLE_IND: invalid primitive format");
	return (EINVAL);
}

#if 0
/**
 * sl_optmgmt_ack: - process SL_OPTMGMT_ACK
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
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
	mi_strlog(q, 0, SL_ERROR, "SL_OPTMGMT_ACK: invalid primitive format");
	return (-EINVAL);
}

/**
 * sl_notify_ind: - process SL_NOTIFY_IND:
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
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
	mi_strlog(q, 0, SL_ERROR, "SL_NOTIFY_IND: invalid primitive format");
	return (-EINVAL);
}
#endif
/**
 * lmi_info_ack: - process LMI_INFO_ACK
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
lmi_info_ack(queue_t *q, mblk_t *mp)
{
	const lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (EFAULT);
      einval:
	mi_strlog(q, 0, SL_ERROR, "LMI_INFO_ACK: invalid primitive format");
	return (EINVAL);
}

/**
 * lmi_ok_ack: - process LMI_OK_ACK
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
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
	return (EFAULT);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "LMI_OK_ACK: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "LMI_OK_ACK: invalid primitive format");
	return (EINVAL);
}

/**
 * lmi_error_ack: - process LMI_ERROR_ACK
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
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
	return (EFAULT);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "LMI_ERROR_ACK: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "LMI_ERROR_ACK: invalid primitive format");
	return (EINVAL);
}

/**
 * lmi_enable_con: - process LMI_ENABLE_CON
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
lmi_enable_con(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_i_state(sl) != LMI_ENABLE_PENDING)
		goto outstate;
	fixme(("Write this function\n"));
	return (EFAULT);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "LMI_ENABLE_CON: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "LMI_ENABLE_CON: invalid primitive format");
	return (EINVAL);
}

/**
 * lmi_disable_con: - process LMI_DISABLE_CON
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
lmi_disable_con(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	const lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl_get_i_state(sl) != LMI_DISABLE_PENDING)
		goto outstate;
	fixme(("Write this function\n"));
	return (EFAULT);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "LMI_DISABLE_CON: would place i/f out of state");
	return (EPROTO);
      einval:
	mi_strlog(q, 0, SL_ERROR, "LMI_DISABLE_CON: invalid primitive format");
	return (EINVAL);
}

/**
 * lmi_optmgmt_ack: - process LMI_OPTMGMT_ACK
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
lmi_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	const lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (EFAULT);
      einval:
	mi_strlog(q, 0, SL_ERROR, "LMI_OPTMGMT_ACK: invalid primitive format");
	return (EINVAL);
}

/**
 * lmi_error_ind: - process LMI_ERROR_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
lmi_error_ind(queue_t *q, mblk_t *mp)
{
	const lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (EFAULT);
      einval:
	mi_strlog(q, 0, SL_ERROR, "LMI_ERROR_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * lmi_stats_ind: - process LMI_STATS_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
lmi_stats_ind(queue_t *q, mblk_t *mp)
{
	const lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (EFAULT);
      einval:
	mi_strlog(q, 0, SL_ERROR, "LMI_STATS_IND: invalid primitive format");
	return (EINVAL);
}

/**
 * lmi_event_ind: - process LMI_EVENT_IND
 * @q: active queue (lower read queue)
 * @mp: the message
 */
static int
lmi_event_ind(queue_t *q, mblk_t *mp)
{
	const lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (EFAULT);
      einval:
	mi_strlog(q, 0, SL_ERROR, "LMI_EVENT_IND: invalid primitive format");
	return (EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from above
 *
 *  -------------------------------------------------------------------------
 */
/**
 * m_data: - process M_DATA message
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
m_data(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int dlen = msgdsize(mp);

	if (mtp->i.style == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == MTPS_IDLE)
		goto discard;
	if (mtp_get_state(mtp) != MTPS_CONNECTED)
		goto outstate;
	if (dlen == 0 || !mtp->prot || dlen > mtp->prot->TSDU_size
	    || dlen > mtp->prot->TIDU_size)
		goto baddata;
	return mtp_send_msg(q, mtp, NULL, &mtp->dst, mp);
      baddata:
	mi_strlog(q, 0, SL_TRACE, "M_DATA: bad data size %d", dlen);
	goto error;
      outstate:
	mi_strlog(q, 0, SL_TRACE, "M_DATA: would place interface out of state");
	goto error;
      discard:
	mi_strlog(q, 0, SL_TRACE, "M_DATA: data ignored in idle state");
	freemsg(mp);
	return (0);
      notsupport:
	mi_strlog(q, 0, SL_TRACE, "M_DATA: primitive not supported for MCLMS");
	goto error;
      error:
	return m_error(q, mtp, mp, EPROTO);
}

/**
 * mtp_bind_req: - process MTP_BIND_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
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
		struct mtp_addr *src =
		    (typeof(src)) (mp->b_rptr + p->mtp_addr_offset);
		struct sp *loc;

		if (src->family && src->family != AF_MTP)
			goto badaddr;
		if (!src->si || !src->pc)
			goto noaddr;
		if (src->si < 3 && mtp->cred.cr_uid != 0)
			goto access;
		if (!(loc = mtp_check_src(q, mtp, src, &err)))
			goto error;
		if (!mtp->sp.loc)
			mtp->sp.loc = loc;
		return m_bind_ack(q, mtp, mp, src);
	}
      access:
	err = MACCESS;
	mi_strlog(q, 0, SL_TRACE,
		  "MTP_BIND_REQ: no permission for requested address");
	goto error;
      badaddr:
	err = MBADADDR;
	mi_strlog(q, 0, SL_TRACE, "MTP_BIND_REQ: address is invalid");
	goto error;
      noaddr:
	err = MNOADDR;
	mi_strlog(q, 0, SL_TRACE, "MTP_BIND_REQ: could not allocate address");
	goto error;
      badprim:
	err = MBADPRIM;
	mi_strlog(q, 0, SL_TRACE, "MTP_BIND_REQ: invalid primitive format");
	goto error;
      outstate:
	err = MOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "MTP_BIND_REQ: would place i/f out of state");
	goto error;
      error:
	return m_error_ack(q, mtp, mp, p->mtp_primitive, err);
}

/**
 * mtp_unbind_req: - process MTP_UNBIND_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
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
	return m_ok_ack(q, mtp, mp, p->mtp_primitive);
      badprim:
	err = MBADPRIM;
	mi_strlog(q, 0, SL_TRACE, "MTP_UNBIND_REQ: invalid primitive format");
	goto error;
      outstate:
	err = MOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "MTP_UNBIND_REQ: would place i/f out of state");
	goto error;
      error:
	return m_error_ack(q, mtp, mp, p->mtp_primitive, err);
}

/**
 * mtp_conn_req: - process MTP_CONN_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 * 
 * MTP is really a connectionless protocol.  When we form a connection we
 * simply remember the destination address.  Some interim MTPs had the ability
 * to send a UPT (User Part Test) message.  If the protocol variant has this
 * ability, we wait for the result of the User Part Test before confirming the
 * connection.
 */
static int
m_conn_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct MTP_conn_req *p = (typeof(p)) mp->b_rptr;

	switch (mtp_get_state(mtp)) {
	default:
		goto outstate;
	case MTPS_IDLE:
		if (mtp->i.style == T_CLTS)
			goto notsupp;
		if (mp->b_wptr < mp->b_rptr + sizeof(*p))
			goto badprim;
		if (mp->b_wptr <
		    mp->b_rptr + p->mtp_addr_offset + p->mtp_addr_length)
			goto badprim;
		if (!p->mtp_addr_length)
			goto noaddr;
		if (p->mtp_addr_length < sizeof(struct mtp_addr))
			goto badaddr;
		{
			struct mtp_addr *dst =
			    (typeof(dst)) (mp->b_rptr + p->mtp_addr_offset);

			if (dst->family && dst->family != AF_MTP)
				goto badaddr;
			if (dst->si == 0 && mtp->src.si == 0)
				goto noaddr;
			if (dst->si < 3 && mtp->cred.cr_uid != 0)
				goto access;
			if (dst->si != mtp->src.si)
				goto badaddr;
			if (!mtp_check_dst(q, mtp, dst))
				goto badaddr;
			mtp->dst = *dst;
		}
		mtp_set_state(mtp, MTPS_WACK_CREQ);
		/* fall through */
	case MTPS_WACK_CREQ:
		/* * There is another thing to do once the connection has been
		   established: that is to deliver MTP restart begins indication or
		   to deliver MTP resume or MTP pause indications for the peer
		   depending on the peer's state. */
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
		return m_ok_ack(q, mtp, mp, p->mtp_primitive);
	}
      access:
	err = MACCESS;
	mi_strlog(q, 0, SL_TRACE,
		  "MTP_CONN_REQ: no permission for requested address");
	goto error;
      badaddr:
	err = MBADADDR;
	mi_strlog(q, 0, SL_TRACE, "MTP_CONN_REQ: address is invalid");
	goto error;
      noaddr:
	err = MNOADDR;
	mi_strlog(q, 0, SL_TRACE, "MTP_CONN_REQ: could not allocate address");
	goto error;
      notsupp:
	err = MNOTSUPP;
	mi_strlog(q, 0, SL_TRACE, "MTP_CONN_REQ: primitive not supported");
	goto error;
      badprim:
	err = MBADPRIM;
	mi_strlog(q, 0, SL_TRACE, "MTP_CONN_REQ: invalid primitive format");
	goto error;
      outstate:
	err = MOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "MTP_CONN_REQ: would place i/f out of state");
	goto error;
      error:
	return m_error_ack(q, mtp, mp, p->mtp_primitive, err);
}

/**
 * mtp_discon_req: - process MTP_DISCON_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
m_discon_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_discon_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mtp->i.style == T_CLTS)
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
	/* change state and let m_ok_ack do all the work */
	return m_ok_ack(q, mtp, mp, p->mtp_primitive);
      notsupp:
	err = MNOTSUPP;
	mi_strlog(q, 0, SL_TRACE, "MTP_DISCON_REQ: primitive not supported");
	goto error;
      badprim:
	err = MBADPRIM;
	mi_strlog(q, 0, SL_TRACE, "MTP_DISCON_REQ: invalid primitive format");
	goto error;
      outstate:
	err = MOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "MTP_DISCON_REQ: would place i/f out of state");
	goto error;
      error:
	return m_error_ack(q, mtp, mp, p->mtp_primitive, err);
}

/**
 * mtp_addr_req: - process MTP_ADDR_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
m_addr_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_addr_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (mtp_get_state(mtp)) {
	case MTPS_UNBND:
		return m_addr_ack(q, mtp, mp, NULL, NULL);
	case MTPS_IDLE:
		return m_addr_ack(q, mtp, mp, &mtp->src, NULL);
	case MTPS_WCON_CREQ:
	case MTPS_CONNECTED:
	case MTPS_WIND_ORDREL:
	case MTPS_WREQ_ORDREL:
		return m_addr_ack(q, mtp, mp, &mtp->src, &mtp->dst);
	case MTPS_WRES_CIND:
		return m_addr_ack(q, mtp, mp, NULL, &mtp->dst);
	}
	goto outstate;
      outstate:
	err = MOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "MTP_ADDR_REQ: would place i/f out of state");
	goto error;
      badprim:
	err = MBADPRIM;
	mi_strlog(q, 0, SL_TRACE, "MTP_ADDR_REQ: invalid primitive format");
	goto error;
      error:
	return m_error_ack(q, mtp, mp, p->mtp_primitive, err);
}

/**
 * mtp_info_req: - process MTP_INFO_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
m_info_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return m_info_ack(q, mtp, mp);
      badprim:
	err = MBADPRIM;
	mi_strlog(q, 0, SL_TRACE, "MTP_INFO_REQ: invalid primitive format");
	goto error;
      error:
	return m_error_ack(q, mtp, mp, p->mtp_primitive, err);
}

/**
 * mtp_optmgmt_req: - process MTP_OPTMGMT_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
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
	if (mtp_parse_opts
	    (mtp, &opts, mp->b_rptr + p->mtp_opt_offset, p->mtp_opt_length))
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
	return m_optmgmt_ack(q, mtp, mp, &opts, p->mtp_mgmt_flags);
      badprim:
	err = MBADPRIM;
	mi_strlog(q, 0, SL_TRACE, "MTP_OPTMGMT_REQ: invalid primitive format");
	goto error;
      badflag:
	err = MBADFLAG;
	mi_strlog(q, 0, SL_TRACE, "MTP_OPTMGMT_REQ: invalid flags");
	goto error;
      badopt:
	err = MBADOPT;
	mi_strlog(q, 0, SL_TRACE, "MTP_OPTMGMT_REQ: invalid options");
	goto error;
      error:
	return m_error_ack(q, mtp, mp, p->mtp_primitive, err);
}

/**
 * mtp_transfer_req: - process MTP_TRANSFER_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
m_transfer_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct MTP_transfer_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgsize(mp->b_cont) : 0;
	int err;

	if (mtp->i.style == T_CLTS)
		goto notsupp;
	if (mtp_get_state(mtp) == MTPS_IDLE)
		goto discard;
	if (mtp_get_state(mtp) != MTPS_CONNECTED)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (dlen == 0 || !mtp->prot || dlen > mtp->prot->TSDU_size
	    || dlen > mtp->prot->TIDU_size)
		goto baddata;
	if ((err = mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont)) < 0)
		return (err);
	freeb(mp);
	return (0);
      baddata:
	err = -EPROTO;
	mi_strlog(q, 0, SL_TRACE, "MTP_TRANSFER_REQ: amount of data is invalid");
	goto error;
      notsupp:
	err = -EPROTO;
	mi_strlog(q, 0, SL_TRACE, "MTP_TRANSFER_REQ: primitive not supported");
	goto error;
      badprim:
	err = -EPROTO;
	mi_strlog(q, 0, SL_TRACE, "MTP_TRANSFER_REQ: invalid primitive format");
	goto error;
      outstate:
	err = -EPROTO;
	mi_strlog(q, 0, SL_TRACE, "MTP_TRANSFER_REQ: would place i/f out of state");
	goto error;
      discard:
	rare();
	return (0);
      error:
	return m_error(q, mtp, mp, err);
}

/**
 * t_data: - process M_DATA message
 * @q: active queue (upper write queue)
 * @mtp: MTP private structure
 * @mp: the message
 */
static int
t_data(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int dlen = msgdsize(mp);
	int err;

	if (mtp->i.style == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (dlen == 0 || !mtp->prot || dlen > mtp->prot->TSDU_size
	    || dlen > mtp->prot->TIDU_size)
		goto baddata;
	if ((err = mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont)) < 0)
		return (err);
	freeb(mp);
	return (0);
      baddata:
	mi_strlog(q, 0, SL_TRACE, "M_DATA: bad data size %d", (int) msgdsize(mp));
	goto error;
      outstate:
	mi_strlog(q, 0, SL_TRACE, "M_DATA i/f out of state");
	goto error;
      discard:
	mi_strlog(q, 0, SL_TRACE, "M_DATA idle state");
	return (0);
      notsupport:
	mi_strlog(q, 0, SL_TRACE, "M_DATA supported for T_CLTS");
	goto error;
      error:
	return m_error(q, mtp, mp, EPROTO);
}

/**
 * t_conn_req: - process T_CONN_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 *
 * As MTP is really a connectionless protocol, when we form a connection we
 * simply remember the destination address.  Some interim MTPs had the ability
 * to send a UPT (User Part Test) message.  If the protocol variant has this
 * ability, we wait for the result of the User Part Test before confirming the
 * connection.
 */
static int
t_conn_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err = -EFAULT;
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;

	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	if (mtp->i.style == T_CLTS)
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
		if (dst->family && dst->family != AF_MTP)
			goto badaddr;
		if (dst->si == 0 && mtp->src.si == 0)
			goto noaddr;
		if (dst->si < 3 && mtp->cred.cr_uid != 0)
			goto acces;
		if (dst->si != mtp->src.si)
			goto badaddr;
		if (!mtp_check_dst(q, mtp, dst))
			goto badaddr;
		{
			unsigned char *opt = mp->b_rptr + p->OPT_offset;
			struct mtp_opts opts = { 0L, NULL, };

			if (mtp_parse_opts(mtp, &opts, opt, p->OPT_length))
				goto badopt;
			/* TODO: set options first */
			if (mp->b_cont) {
				putbq(q, mp->b_cont);	/* hold back data */
				mp->b_cont = NULL;	/* abosrbed mp->b_cont */
			}
			mtp->dst = *dst;
			mtp_set_state(mtp, TS_WACK_CREQ);
			{
				struct sp *sp = mtp->sp.loc;

				if ((err =
				     mtp_send_upt(q, sp, sp->ni, mtp->dst.pc,
						  mtp->src.pc, 0, mtp->dst.pc,
						  mtp->dst.si)) < 0)
					goto error;
			}
			if ((err = t_ok_ack(q, mtp, NULL, T_CONN_REQ)) < 0)
				goto error;
			return t_conn_con(q, mtp, mp, dst, NULL, NULL);
		}
	}
      badopt:
	err = TBADOPT;
	mi_strlog(q, 0, SL_TRACE, "T_CONN_REQ: bad options");
	goto error;
      acces:
	err = TACCES;
	mi_strlog(q, 0, SL_TRACE, "T_CONN_REQ: no permission for address");
	goto error;
      badaddr:
	err = TBADADDR;
	mi_strlog(q, 0, SL_TRACE, "T_CONN_REQ: address is unusable");
	goto error;
      noaddr:
	err = TNOADDR;
	mi_strlog(q, 0, SL_TRACE, "T_CONN_REQ: couldn't allocate address");
	goto error;
      einval:
	err = -EINVAL;
	mi_strlog(q, 0, SL_TRACE, "T_CONN_REQ: invalid message format");
	goto error;
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "T_CONN_REQ: would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "T_CONN_REQ: primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * t_discon_req: - process T_DISCON_REQ - 2 - TC disconnection request
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_discon_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct T_discon_req *p = (typeof(p)) mp->b_rptr;

	if (mtp->i.style == T_CLTS)
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
	/* change state and let t_ok_ack do all the work */
	return t_ok_ack(q, mtp, mp, T_DISCON_REQ);
      einval:
	err = -EINVAL;
	mi_strlog(q, 0, SL_TRACE, "T_DISCON_REQ: invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "T_DISCON_REQ: would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE,
		  "T_DISCON_REQ: primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * t_data_req: - process T_DATA_REQ - 3 - Connection-Mode data transfer request
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_data_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct T_data_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;

	if (mtp->i.style == T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << mtp_get_state(mtp)) & ~(TSF_DATA_XFER | TSF_WREQ_ORDREL))
		goto outstate;
	if (dlen == 0 || !mtp->prot || dlen > mtp->prot->TSDU_size
	    || dlen > mtp->prot->TIDU_size)
		goto baddata;
	if ((err = mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont)) < 0)
		return (err);
	freeb(mp);
	return (0);
      baddata:
	mi_strlog(q, 0, SL_TRACE, "T_DATA_REQ: bad data size %d",
		  (int) msgdsize(mp->b_cont));
	goto error;
      outstate:
	mi_strlog(q, 0, SL_TRACE, "T_DATA_REQ: would place i/f out of state");
	goto error;
      einval:
	mi_strlog(q, 0, SL_TRACE, "T_DATA_REQ: invalid primitive format");
	goto error;
      discard:
	mi_strlog(q, 0, SL_TRACE, "T_DATA_REQ: ignore in idle state");
	return (0);
      notsupport:
	mi_strlog(q, 0, SL_TRACE, "T_DATA_REQ: primitive not supported for T_CLTS");
	goto error;
      error:
	return m_error(q, mtp, mp, EPROTO);
}

/**
 * t_exdata_req: - process T_EXDATA_REQ - 4 - Expedited data request
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_exdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);

	return m_error(q, mtp, mp, EPROTO);
}

/**
 * t_info_req: - process T_INFO_REQ -5 - Information Request
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_info_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);

	return t_info_ack(q, mtp, mp);
}

/**
 * t_bind_req: - process T_BIND_REQ - 6 - Bind a TS user to a transport address
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
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

		/* we don't allow wildcards yet. */
		if (src->family && src->family != AF_MTP)
			goto badaddr;
		if (!src->si || !src->pc)
			goto noaddr;
		if (src->si < 3 && mtp->cred.cr_uid != 0)
			goto acces;
		if ((loc = mtp_check_src(q, mtp, src, &err)))
			goto error;
		if (!mtp->sp.loc)
			mtp->sp.loc = loc;
		return t_bind_ack(q, mtp, mp, src);
	}
      acces:
	err = TACCES;
	mi_strlog(q, 0, SL_TRACE, "T_BIND_REQ: no permission for address");
	goto error;
      noaddr:
	err = TNOADDR;
	mi_strlog(q, 0, SL_TRACE, "T_BIND_REQ: couldn't allocate address");
	goto error;
      badaddr:
	err = TBADADDR;
	mi_strlog(q, 0, SL_TRACE, "T_BIND_REQ: address is invalid");
	goto error;
      einval:
	err = -EINVAL;
	mi_strlog(q, 0, SL_TRACE, "T_BIND_REQ: invalid primitive format");
	goto error;
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "T_BIND_REQ: would place i/f out of state");
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "T_BIND_REQ: primitive not support for T_CLTS");
	goto error;
      error:
	return t_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * t_unbind_req: - process T_UNBIND_REQ - 7 - Unbind TS user from transport address
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_unbind_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	mtp_set_state(mtp, TS_WACK_UREQ);
	return t_ok_ack(q, mtp, mp, T_UNBIND_REQ);
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "T_UNBIND_REQ: would place i/f out of state");
	goto error;
      error:
	return t_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * t_unitdata_req: - process T_UNITDATA_REQ - 8 -Unitdata Request 
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;

	if (mtp->i.style != T_CLTS)
		goto notsupport;
	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	if (dlen == 0 || !mtp->prot || dlen > mtp->prot->TSDU_size
	    || dlen > mtp->prot->TIDU_size)
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
		if (!mtp_check_dst(q, mtp, dst))
			goto badaddr;
		else {
			struct mtp_opts opts = { 0L, NULL, };

			if (mtp_parse_opts
			    (mtp, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
				goto badopt;
			if ((err = mtp_send_msg(q, mtp, &opts, dst, mp->b_cont)) < 0)
				return (err);
			freeb(mp);
			return (0);
		}
	}
      badopt:
	mi_strlog(q, 0, SL_TRACE, "T_UNITDATA_REQ: bad options");
	goto error;
      acces:
	mi_strlog(q, 0, SL_TRACE, "T_UNITDATA_REQ: no permission to address");
	goto error;
      badaddr:
	mi_strlog(q, 0, SL_TRACE, "T_UNITDATA_REQ: bad destination address");
	goto error;
      einval:
	mi_strlog(q, 0, SL_TRACE, "T_UNITDATA_REQ: invalid parameter");
	goto error;
      outstate:
	mi_strlog(q, 0, SL_TRACE, "T_UNITDATA_REQ: would place i/f out of state");
	goto error;
      baddata:
	mi_strlog(q, 0, SL_TRACE, "T_UNITDATA_REQ: bad data size %d",
		  (int) msgdsize(mp->b_cont));
	goto error;
      notsupport:
	mi_strlog(q, 0, SL_TRACE,
		  "T_UNITDATA_REQ: primitive not supported for T_COTS or T_COTS_ORD");
	goto error;
      error:
	return m_error(q, mtp, mp, EPROTO);
}

/**
 * t_optmgmt_req: - process T_OPTMGMT_REQ - 9 - Options management request
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
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

		if (mtp_parse_opts
		    (mtp, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
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
		return t_optmgmt_ack(q, mtp, mp, p->MGMT_flags, &opts);
	}
      provspec:
	err = err;
	mi_strlog(q, 0, SL_TRACE, "T_OPTMGMT_REQ: provider specific");
	goto error;
      badflag:
	err = TBADFLAG;
	mi_strlog(q, 0, SL_TRACE, "T_OPTMGMT_REQ: bad options flags");
	goto error;
      badopt:
	err = TBADOPT;
	mi_strlog(q, 0, SL_TRACE, "T_OPTMGMT_REQ: bad options");
	goto error;
      einval:
	err = -EINVAL;
	mi_strlog(q, 0, SL_TRACE, "T_OPTMGMT_REQ: invalid primitive format");
	goto error;
      error:
	return t_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * t_ordrel_req: - process T_ORDREL_REQ - 10 - TS user is finished sending
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_ordrel_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;

	if (mtp->i.style != T_COTS_ORD)
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
	return t_ordrel_ind(q, mtp, mp);
      outstate:
	mi_strlog(q, 0, SL_TRACE, "T_ORDREL_REQ: would place i/f out of state");
	goto error;
      notsupport:
	mi_strlog(q, 0, SL_TRACE,
		  "T_ORDREL_REQ: primitive not supported for T_CLTS or T_COTS");
	goto error;
      error:
	return m_error(q, mtp, mp, EPROTO);
}

/**
 * t_optdata_req: - process T_OPTDATA_REQ  - 24 - Data with options request
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_optdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const struct T_optdata_req *p = (typeof(p)) mp->b_rptr;

	if (mtp->i.style == T_CLTS)
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

		if (mtp_parse_opts
		    (mtp, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
		if ((err = mtp_send_msg(q, mtp, &opts, &mtp->dst, mp->b_cont)) < 0)
			return (err);
		freeb(mp);
		return (0);
	}
      badopt:
	err = TBADOPT;
	mi_strlog(q, 0, SL_TRACE, "T_OPTDATA_REQ: bad options");
	goto error;
      outstate:
	err = TOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "T_OPTDATA_REQ: would place i/f out of state");
	goto error;
      einval:
	mi_strlog(q, 0, SL_TRACE, "T_OPTDATA_REQ: invalid primitive format");
	return m_error(q, mtp, mp, EPROTO);
      discard:
	mi_strlog(q, 0, SL_TRACE, "T_OPTDATA_REQ: ignore in idle state");
	return (0);
      notsupport:
	err = TNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE,
		  "T_OPTDATA_REQ: primitive not supported for T_CLTS");
	goto error;
      error:
	return t_error_ack(q, mtp, mp, p->PRIM_type, err);
}

#ifdef T_ADDR_REQ
/**
 * t_addr_req: - process T_ADDR_REQ - 25 - Address Request
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_addr_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct T_addr_req *p = (typeof(p)) mp->b_rptr;

	(void) mp;
	switch (mtp_get_state(mtp)) {
	case TS_UNBND:
		return t_addr_ack(q, mtp, mp, NULL, NULL);
	case TS_IDLE:
		return t_addr_ack(q, mtp, mp, &mtp->src, NULL);
	case TS_WCON_CREQ:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		return t_addr_ack(q, mtp, mp, &mtp->src, &mtp->dst);
	case TS_WRES_CIND:
		return t_addr_ack(q, mtp, mp, NULL, &mtp->dst);
	}
	return t_error_ack(q, mtp, mp, p->PRIM_type, TOUTSTATE);
}
#endif

#ifdef T_CAPABILITY_REQ
/**
 * t_capability_req: - process T_CAPABILITY_REQ - ?? - Capability Request
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
t_capability_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	const struct T_capability_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	return t_capability_ack(q, mtp, mp, p->CAP_bits1);
      einval:
	mi_strlog(q, 0, SL_TRACE, "T_ADDR_REQ: invalid primitive format");
	return t_error_ack(q, mtp, mp, p->PRIM_type, -EINVAL);
}
#endif

/**
 * n_data: - process M_DATA message
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_data(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	int err;
	const size_t dlen = msgdsize(mp);

	if (mtp->i.style == T_CLTS)
		goto notsupp;
	if (dlen == 0 || !mtp->prot || dlen > mtp->prot->TSDU_size
	    || dlen > mtp->prot->TIDU_size)
		goto baddata;
	switch (mtp_get_state(mtp)) {
	case NS_DATA_XFER:
		if ((err = mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont)) < 0)
			return (err);
		freeb(mp);
		return (0);
	case NS_IDLE:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	/* If we are in the idle state this is just spurious data, ignore it */
	rare();
	return (0);
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "M_DATA: would place i/f out of state");
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "M_DATA: primitive not supported");
	goto error;
      baddata:
	err = -EPROTO;
	mi_strlog(q, 0, SL_TRACE, "M_DATA: bad data");
	goto error;
      error:
	return m_error(q, mtp, mp, EPROTO);
}

/**
 * n_conn_req: - process N_CONN_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
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
	if (mtp->i.style == T_CLTS)
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
	if (dst->family && dst->family != AF_MTP)
		goto badaddr;
	if (dst->si == 0 && mtp->src.si == 0)
		goto noaddr;
	if (dst->si < 3 && mtp->src.si != 0)
		goto badaddr;
	if (!mtp_check_dst(q, mtp, dst))
		goto badaddr;
	if (mtp_parse_qos(mtp, &opts, mp->b_rptr + p->QOS_offset, p->QOS_length))
		goto badqostype;
	/* TODO: set options first */
	mtp->dst = *dst;
	mtp_set_state(mtp, NS_WCON_CREQ);
	return n_conn_con(q, mtp, mp, 0, &mtp->dst, NULL);
      badqostype:
	err = NBADQOSTYPE;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_REQ: bad qos type");
	goto error;
#if 0
      badqosparam:
	err = NBADQOSPARAM;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_REQ: bad qos parameter");
	goto error;
#endif
      badaddr:
	err = NBADADDR;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_REQ: bad destination address");
	goto error;
      noaddr:
	err = NNOADDR;
	mi_strlog(q, 0, SL_TRACE,
		  "N_CONN_REQ: couldn't allocate destination address");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_REQ: invalid primitive format");
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_REQ: primitive not supported for N_CLNS");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_REQ: would place i/f out of state");
	goto error;
      error:
	return n_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * n_conn_res: - process N_CONN_RES
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_conn_res(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_conn_res_t *p = (typeof(p)) mp->b_rptr;

	if (mtp->i.style == T_CLTS)
		goto notsupp;
	if (mtp_get_state(mtp) != NS_WRES_CIND)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* We never give an N_CONN_IND, so there is no reason for a N_CONN_RES.  We
	   probably could do this * (issue an N_CONN_IND on a listening stream when
	   there is no other MTP user for the SI value and * send a UPU on an
	   N_DISCON_REQ or just redirect all traffic for that user on a N_CONN_RES)
	   but * that is for later. */
	goto eopnotsupp;
      eopnotsupp:
	err = -EOPNOTSUPP;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_RES: operation not supported");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_RES: invalid primitive format");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_RES: would place i/f out of state");
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "N_CONN_RES: primitive not supported");
	goto error;
      error:
	return n_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * n_discon_req: - process N_DISCON_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_discon_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_discon_req_t *p = (typeof(p)) mp->b_rptr;

	if (mtp->i.style == T_CLTS)
		goto notsupp;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	/* Currently there are only three states we can disconnect from.  The first
	   does not happen. Only the second one is normal.  The third should occur
	   during simulteneous diconnect only. */
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
	return n_ok_ack(q, mtp, mp, p->PRIM_type);
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "N_DISCON_REQ: invalid primitive format");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "N_DISCON_REQ: would place i/f out of state");
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "N_DISCON_REQ: primitive not supported");
	goto error;
      error:
	return n_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * n_data_req: - process N_DATA_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_data_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_data_req_t *p = (typeof(p)) mp->b_rptr;
	const size_t dlen = msgdsize(mp);

	if (mtp->i.style == T_CLTS)
		goto notsupp;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->DATA_xfer_flags)
		/* N_MORE_DATA_FLAG and N_RC_FLAG not supported yet.  We could do
		   N_MORE_DATA_FLAG pretty easily by accumulating the packet until
		   the last data request is received, but this would be rather
		   pointless for small MTP packet sizes. N_RC_FLAG cannot be
		   supported until the DLPI link driver is done and zero-loss
		   operation is completed. */
		goto notsupp;
	if (dlen == 0 || !mtp->prot || dlen > mtp->prot->TSDU_size
	    || dlen > mtp->prot->TIDU_size)
		goto baddata;
	switch (mtp_get_state(mtp)) {
	case NS_DATA_XFER:
		if ((err = mtp_send_msg(q, mtp, NULL, &mtp->dst, mp->b_cont)) < 0)
			return (err);
		freeb(mp);
		return (0);
	case NS_IDLE:
		goto ignore;
	default:
		goto outstate;
	}
      ignore:
	/* If we are in the idle state this is just spurious data, ignore it */
	rare();
	return (0);
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "N_DATA_REQ: invalid primitive format");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "N_DATA_REQ: would place i/f out of state");
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "N_DATA_REQ: primitive not supported");
	goto error;
      baddata:
	err = -EPROTO;
	mi_strlog(q, 0, SL_TRACE, "N_DATA_REQ: bad data");
	goto error;
      error:
	return m_error(q, mtp, mp, EPROTO);
}

/**
 * n_exdata_req: - process N_EXDATA_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_exdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);

	return m_error(q, mtp, mp, EPROTO);
}

/**
 * n_info_req: - process N_INFO_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_info_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);

	return n_info_ack(q, mtp, mp);
}

/**
 * n_bind_req: - process N_BIND_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
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
	if (src.family && src.family != AF_MTP)
		goto badaddr;
	if (!src.si || !src.pc)
		goto noaddr;
	if (src.si < 3 || mtp->cred.cr_uid != 0)
		goto access;
	if ((loc = mtp_check_src(q, mtp, &src, &err)))
		goto error;
	if (!mtp->sp.loc)
		mtp->sp.loc = loc;
	mtp_set_state(mtp, NS_WACK_BREQ);
	return n_bind_ack(q, mtp, mp, &src);
      access:
	err = NACCESS;
	mi_strlog(q, 0, SL_TRACE, "N_BIND_REQ: no privilege for requested address");
	goto error;
      noaddr:
	err = NNOADDR;
	mi_strlog(q, 0, SL_TRACE, "N_BIND_REQ: could not allocate address");
	goto error;
      badaddr:
	err = NBADADDR;
	mi_strlog(q, 0, SL_TRACE, "N_BIND_REQ: requested address invalid");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "N_BIND_REQ: would place i/f out of state");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "N_BIND_REQ: invalid primitive format");
	goto error;
      error:
	return n_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * n_unbind_req: - process N_UNBIND_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
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
	return n_ok_ack(q, mtp, mp, p->PRIM_type);
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "N_UNBIND_REQ: would place i/f out of state");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "N_UNBIND_REQ: invalid primitive format");
	goto error;
      error:
	return n_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * n_unitdata_req: - process N_UNITDATA_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp = MTP_PRIV(q);
	int err;
	const N_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	const size_t dlen = msgdsize(mp);
	struct mtp_addr dst;

	if (mtp->i.style != T_CLTS)
		goto notsupp;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badprim;
	if (mtp_get_state(mtp) != NS_IDLE)
		goto outstate;
	if (dlen == 0 || !mtp->prot || dlen > mtp->prot->TSDU_size
	    || dlen > mtp->prot->TIDU_size)
		goto baddata;
	if (!p->DEST_length)
		goto noaddr;
	if (p->DEST_length < sizeof(dst))
		goto badaddr;
	bcopy(mp->b_rptr + p->DEST_length, &dst, sizeof(dst));
	if (dst.family && dst.family != AF_MTP)
		goto badaddr;
	if (!dst.si || !dst.pc)
		goto badaddr;
	if (dst.si < 3 && mtp->cred.cr_uid != 0)
		goto access;
	if (dst.si != mtp->src.si)
		goto badaddr;
	if (!mtp_check_dst(q, mtp, &dst))
		goto badaddr;
	if ((err = mtp_send_msg(q, mtp, NULL, &dst, mp->b_cont)) < 0)
		return (err);
	freeb(mp);
	return (0);
      access:
	err = NACCESS;
	mi_strlog(q, 0, SL_TRACE,
		  "N_UNITDATA_REQ: no privilege for requested address");
	goto error;
      badaddr:
	err = NBADADDR;
	mi_strlog(q, 0, SL_TRACE, "N_UNITDATA_REQ: requested address invalid");
	goto error;
      noaddr:
	err = NNOADDR;
	mi_strlog(q, 0, SL_TRACE, "N_UNITDATA_REQ: could not allocate address");
	goto error;
      baddata:
	err = NBADDATA;
	mi_strlog(q, 0, SL_TRACE, "N_UNITDATA_REQ: invalid amount of data");
	goto error;
      outstate:
	err = NOUTSTATE;
	mi_strlog(q, 0, SL_TRACE, "N_UNITDATA_REQ: would place i/f out of state");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "N_UNITDATA_REQ: invalid primitive format");
	goto error;
      notsupp:
	err = NNOTSUPPORT;
	mi_strlog(q, 0, SL_TRACE, "N_UNITDATA_REQ: primitive type not supported");
	goto error;
      error:
	return n_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * n_optmgmt_req: - process N_OPTMGMT_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
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
		/* Can't support DEFAULT_RC_SEL yet */
		goto badflags;
	if (mtp_parse_qos(mtp, &opts, mp->b_rptr + p->QOS_offset, p->QOS_length))
		goto badqostype;
	if ((err = mtp_opt_check(mtp, &opts)))
		goto error;
	if ((err = mtp_opt_negotiate(mtp, &opts)))
		goto error;
	return n_ok_ack(q, mtp, mp, p->PRIM_type);
      badqostype:
	err = NBADQOSTYPE;
	mi_strlog(q, 0, SL_TRACE, "N_OPTMGMT_REQ: invalid qos type");
	goto error;
#if 0
      badqosparam:
	err = NBADQOSPARAM;
	mi_strlog(q, 0, SL_TRACE, "N_OPTMGMT_REQ: invalid qos parameter");
	goto error;
#endif
      badflags:
	err = NBADFLAG;
	mi_strlog(q, 0, SL_TRACE, "N_OPTMGMT_REQ: invalid flag");
	goto error;
      badprim:
	err = -EMSGSIZE;
	mi_strlog(q, 0, SL_TRACE, "N_OPTMGMT_REQ: invalid primitive format");
	goto error;
      error:
	return n_error_ack(q, mtp, mp, p->PRIM_type, err);
}

/**
 * n_datack_req: - process N_DATACK_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_datack_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	/* We don't support DATACK yet.  With zero loss operation we will. */
	rare();
	freemsg(mp);
	return (0);
}

/**
 * n_reset_req: - process N_RESET_REQ
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_reset_req(queue_t *q, mblk_t *mp)
{
	(void) q;
	todo(("Accept resets with reason from the user\n"));
	rare();
	freemsg(mp);
	return (0);
}

/**
 * n_reset_res: - process N_RESET_RES
 * @q: active queue (upper write queue)
 * @mp: the message
 */
static int
n_reset_res(queue_t *q, mblk_t *mp)
{
	(void) q;
	/* ignore.  if the user wishes to respond to our reset indications that's
	   fine. */
	rare();
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IO Controls
 *
 *  -------------------------------------------------------------------------
 */

static int
mtp_do_options_na(struct mtp_option *p, struct na *na, int cmd)
{
	struct mtp_opt_conf_na *c = (typeof(c)) (p + 1);

	if (!na)
		return (-ESRCH);
	p->id = na->id;
	if (cmd == MTP_CHA)
		na->config = *c;
	*c = na->config;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_options_sp(struct mtp_option *p, struct sp *sp, int cmd)
{
	struct mtp_opt_conf_sp *c = (typeof(c)) (p + 1);

	if (!sp)
		return (-ESRCH);
	p->id = sp->id;
	if (cmd == MTP_CHA)
		sp->config = *c;
	*c = sp->config;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_options_rs(struct mtp_option *p, struct rs *rs, int cmd)
{
	struct mtp_opt_conf_rs *c = (typeof(c)) (p + 1);

	if (!rs)
		return (-ESRCH);
	p->id = rs->id;
	if (cmd == MTP_CHA)
		rs->config = *c;
	*c = rs->config;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_options_rl(struct mtp_option *p, struct rl *rl, int cmd)
{
	struct mtp_opt_conf_rl *c = (typeof(c)) (p + 1);

	if (!rl)
		return (-ESRCH);
	p->id = rl->id;
	if (cmd == MTP_CHA)
		rl->config = *c;
	*c = rl->config;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_options_rt(struct mtp_option *p, struct rt *rt, int cmd)
{
	struct mtp_opt_conf_rt *c = (typeof(c)) (p + 1);

	if (!rt)
		return (-ESRCH);
	p->id = rt->id;
	if (cmd == MTP_CHA)
		rt->config = *c;
	*c = rt->config;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_options_ls(struct mtp_option *p, struct ls *ls, int cmd)
{
	struct mtp_opt_conf_ls *c = (typeof(c)) (p + 1);

	if (!ls)
		return (-ESRCH);
	p->id = ls->id;
	if (cmd == MTP_CHA)
		ls->config = *c;
	*c = ls->config;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_options_lk(struct mtp_option *p, struct lk *lk, int cmd)
{
	struct mtp_opt_conf_lk *c = (typeof(c)) (p + 1);

	if (!lk)
		return (-ESRCH);
	p->id = lk->id;
	if (cmd == MTP_CHA)
		lk->config = *c;
	*c = lk->config;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_options_sl(struct mtp_option *p, struct sl *sl, int cmd)
{
	struct mtp_opt_conf_sl *c = (typeof(c)) (p + 1);

	if (!sl)
		return (-ESRCH);
	p->id = sl->id;
	if (cmd == MTP_CHA)
		sl->config = *c;
	*c = sl->config;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_options_df(struct mtp_option *p, struct df *df, int cmd)
{
	struct mtp_opt_conf_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	p->id = df->id;
	if (cmd == MTP_CHA)
		df->config = *c;
	*c = df->config;
	return (sizeof(*p) + sizeof(*c));
}

static int
mtp_do_options(mblk_t *dp, int cmd)
{
	struct mtp_option *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (cmd) {
	case MTP_GET:		/* get */
	case MTP_CHA:		/* set */
		switch (p->type) {
		case MTP_OBJ_TYPE_NA:
			rtn = mtp_do_options_na(p, na_lookup(p->id), cmd);
			break;
		case MTP_OBJ_TYPE_SP:
			rtn = mtp_do_options_sp(p, sp_lookup(p->id), cmd);
			break;
		case MTP_OBJ_TYPE_RS:
			rtn = mtp_do_options_rs(p, rs_lookup(p->id), cmd);
			break;
		case MTP_OBJ_TYPE_RL:
			rtn = mtp_do_options_rl(p, rl_lookup(p->id), cmd);
			break;
		case MTP_OBJ_TYPE_RT:
			rtn = mtp_do_options_rt(p, rt_lookup(p->id), cmd);
			break;
		case MTP_OBJ_TYPE_LS:
			rtn = mtp_do_options_ls(p, ls_lookup(p->id), cmd);
			break;
		case MTP_OBJ_TYPE_LK:
			rtn = mtp_do_options_lk(p, lk_lookup(p->id), cmd);
			break;
		case MTP_OBJ_TYPE_SL:
			rtn = mtp_do_options_sl(p, sl_lookup(p->id), cmd);
			break;
		case MTP_OBJ_TYPE_DF:
			rtn = mtp_do_options_df(p, df_lookup(p->id), cmd);
			break;
		default:
			return (EINVAL);
		}
		if (rtn < 0)
			return (-rtn);
		dp->b_wptr = dp->b_rptr + rtn;
		return (0);
	default:
		return (EINVAL);
	}
}
static int
mtp_get_options(mblk_t *dp)
{
	return mtp_do_options(dp, MTP_GET);
}
static int
mtp_set_options(mblk_t *dp)
{
	return mtp_do_options(dp, MTP_SET);
}

static int
mtp_get_statem_na(struct mtp_statem *p, struct na *na)
{
	struct mtp_statem_na *c = (typeof(c)) (p + 1);

	if (!na)
		return (-ESRCH);
	p->id = na->id;
	p->flags = na->flags;
	p->state = na->state;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_statem_sp(struct mtp_statem *p, struct sp *sp)
{
	struct mtp_statem_sp *c = (typeof(c)) (p + 1);

	if (!sp)
		return (-ESRCH);
	p->id = sp->id;
	p->flags = sp->flags;
	p->state = sp->state;
	c->timers.t1r = (mtp_timer_t) sp_timer_remain(sp, t1r);
	c->timers.t18 = (mtp_timer_t) sp_timer_remain(sp, t18);
	c->timers.t20 = (mtp_timer_t) sp_timer_remain(sp.t20);
	c->timers.t22a = (mtp_timer_t) sp_timer_remain(sp, t22a);
	c->timers.t23a = (mtp_timer_t) sp_timer_remain(sp, t23a);
	c->timers.t24a = (mtp_timer_t) sp_timer_remain(sp, t24a);
	c->timers.t26a = (mtp_timer_t) sp_timer_remain(sp, t26a);
	c->timers.t27a = (mtp_timer_t) sp_timer_remain(sp, t27a);
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_statem_rs(struct mtp_statem *p, struct rs *rs)
{
	struct mtp_statem_rs *c = (typeof(c)) (p + 1);

	if (!rs)
		return (-ESRCH);
	p->id = rs->id;
	p->flags = rs->flags;
	p->state = rs->state;
	c->timers.t8 = (mtp_timer_t) rs_timer_remain(rs, t8);
	c->timers.t11 = (mtp_timer_t) rs_timer_remain(rs, t11);
	c->timers.t15 = (mtp_timer_t) rs_timer_remain(rs, t15);
	c->timers.t16 = (mtp_timer_t) rs_timer_remain(rs, t16);
	c->timers.t18a = (mtp_timer_t) rs_timer_remain(rs, t18a);
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_statem_rl(struct mtp_statem *p, struct rl *rl)
{
	struct mtp_statem_rl *c = (typeof(c)) (p + 1);

	if (!rl)
		return (-ESRCH);
	p->id = rl->id;
	p->flags = rl->flags;
	p->state = rl->state;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_statem_rt(struct mtp_statem *p, struct rt *rt)
{
	struct mtp_statem_rt *c = (typeof(c)) (p + 1);

	if (!rt)
		return (-ESRCH);
	p->id = rt->id;
	p->flags = rt->flags;
	p->state = rt->state;
	c->timers.t6 = (mtp_timer_t) rt_timer_remain(rt, t6);
	c->timers.t10 = (mtp_timer_t) rt_timer_remain(rt, t10);
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_statem_ls(struct mtp_statem *p, struct ls *ls)
{
	struct mtp_statem_ls *c = (typeof(c)) (p + 1);

	if (!ls)
		return (-ESRCH);
	p->id = ls->id;
	p->flags = ls->flags;
	p->state = ls->state;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_statem_lk(struct mtp_statem *p, struct lk *lk)
{
	struct mtp_statem_lk *c = (typeof(c)) (p + 1);

	if (!lk)
		return (-ESRCH);
	p->id = lk->id;
	p->flags = lk->flags;
	p->state = lk->state;
	c->timers.t7 = (mtp_timer_t) lk_timer_remain(lk, t7);
	c->timers.t19 = (mtp_timer_t) lk_timer_remain(lk, t19);
	c->timers.t21 = (mtp_timer_t) lk_timer_remain(lk, t21);
	c->timers.t25a = (mtp_timer_t) lk_timer_remain(lk, t25a);
	c->timers.t28a = (mtp_timer_t) lk_timer_remain(lk, t28a);
	c->timers.t29a = (mtp_timer_t) lk_timer_remain(lk, t29a);
	c->timers.t30a = (mtp_timer_t) lk_timer_remain(lk, t30a);
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_statem_sl(struct mtp_statem *p, struct sl *sl)
{
	struct mtp_statem_sl *c = (typeof(c)) (p + 1);

	if (!sl)
		return (-ESRCH);
	p->id = sl->id;
	p->flags = sl->flags;
	p->state = sl->state;
	c->timers.t1 = (mtp_timer_t) sl_timer_remain(sl, t1);
	c->timers.t2 = (mtp_timer_t) sl_timer_remain(sl, t2);
	c->timers.t3 = (mtp_timer_t) sl_timer_remain(sl, t3);
	c->timers.t4 = (mtp_timer_t) sl_timer_remain(sl, t4);
	c->timers.t5 = (mtp_timer_t) sl_timer_remain(sl, t5);
	c->timers.t12 = (mtp_timer_t) sl_timer_remain(sl, t12);
	c->timers.t13 = (mtp_timer_t) sl_timer_remain(sl, t13);
	c->timers.t14 = (mtp_timer_t) sl_timer_remain(sl, t14);
	c->timers.t17 = (mtp_timer_t) sl_timer_remain(sl, t17);
	c->timers.t19a = (mtp_timer_t) sl_timer_remain(sl, t19a);
	c->timers.t20a = (mtp_timer_t) sl_timer_remain(sl, t20a);
	c->timers.t21a = (mtp_timer_t) sl_timer_remain(sl, t21a);
	c->timers.t22 = (mtp_timer_t) sl_timer_remain(sl, t22);
	c->timers.t23 = (mtp_timer_t) sl_timer_remain(sl, t23);
	c->timers.t24 = (mtp_timer_t) sl_timer_remain(sl, t24);
	c->timers.t31a = (mtp_timer_t) sl_timer_remain(sl, t31a);
	c->timers.t32a = (mtp_timer_t) sl_timer_remain(sl, t32a);
	c->timers.t33a = (mtp_timer_t) sl_timer_remain(sl, t33a);
	c->timers.t34a = (mtp_timer_t) sl_timer_remain(sl, t34a);
	c->timers.t1t = (mtp_timer_t) sl_timer_remain(sl, t1t);
	c->timers.t2t = (mtp_timer_t) sl_timer_remain(sl, t2t);
	c->timers.t1s = (mtp_timer_t) sl_timer_remain(sl, t1s);
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_statem_df(struct mtp_statem *p, struct df *df)
{
	struct mtp_statem_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	p->id = df->id;
	p->flags = df->flags;
	p->state = df->state;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_statem(mblk_t *dp)
{
	struct mtp_statem *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		rtn = mtp_get_statem_na(p, na_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_SP:
		rtn = mtp_get_statem_sp(p, sp_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_RS:
		rtn = mtp_get_statem_rs(p, rs_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_RL:
		rtn = mtp_get_statem_rl(p, rl_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_RT:
		rtn = mtp_get_statem_rt(p, rt_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_LS:
		rtn = mtp_get_statem_ls(p, ls_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_LK:
		rtn = mtp_get_statem_lk(p, lk_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_SL:
		rtn = mtp_get_statem_sl(p, sl_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_DF:
		rtn = mtp_get_statem_df(p, df_lookup(p->id));
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

static int
mtp_get_stats_na(struct mtp_stats *p, struct na *na, bool clear)
{
	struct mtp_stats_na *c = (typeof(c)) (p + 1);

	if (!na)
		return (-ESRCH);
	p->id = na->id;
	*c = na->stats;
	if (clear)
		bzero(&na->stats, sizeof(na->stats));
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_stats_sp(struct mtp_stats *p, struct sp *sp, bool clear)
{
	struct mtp_stats_sp *c = (typeof(c)) (p + 1);

	if (!sp)
		return (-ESRCH);
	p->id = sp->id;
	*c = sp->stats;
	if (clear)
		bzero(&sp->stats, sizeof(sp->stats));
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_stats_rs(struct mtp_stats *p, struct rs *rs, bool clear)
{
	struct mtp_stats_rs *c = (typeof(c)) (p + 1);

	if (!rs)
		return (-ESRCH);
	p->id = rs->id;
	*c = rs->stats;
	if (clear)
		bzero(&rs->stats, sizeof(rs->stats));
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_stats_rl(struct mtp_stats *p, struct rl *rl, bool clear)
{
	struct mtp_stats_rl *c = (typeof(c)) (p + 1);

	if (!rl)
		return (-ESRCH);
	p->id = rl->id;
	*c = rl->stats;
	if (clear)
		bzero(&rl->stats, sizeof(rl->stats));
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_stats_rt(struct mtp_stats *p, struct rt *rt, bool clear)
{
	struct mtp_stats_rt *c = (typeof(c)) (p + 1);

	if (!rt)
		return (-ESRCH);
	p->id = rt->id;
	*c = rt->stats;
	if (clear)
		bzero(&rt->stats, sizeof(rt->stats));
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_stats_ls(struct mtp_stats *p, struct ls *ls, bool clear)
{
	struct mtp_stats_ls *c = (typeof(c)) (p + 1);

	if (!ls)
		return (-ESRCH);
	p->id = ls->id;
	*c = ls->stats;
	if (clear)
		bzero(&ls->stats, sizeof(ls->stats));
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_stats_lk(struct mtp_stats *p, struct lk *lk, bool clear)
{
	struct mtp_stats_lk *c = (typeof(c)) (p + 1);

	if (!lk)
		return (-ESRCH);
	p->id = lk->id;
	*c = lk->stats;
	if (clear)
		bzero(&lk->stats, sizeof(lk->stats));
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_stats_sl(struct mtp_stats *p, struct sl *sl, bool clear)
{
	struct mtp_stats_sl *c = (typeof(c)) (p + 1);

	if (!sl)
		return (-ESRCH);
	p->id = sl->id;
	*c = sl->stats;
	if (clear)
		bzero(&sl->stats, sizeof(sl->stats));
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_stats_df(struct mtp_stats *p, struct df *df, bool clear)
{
	struct mtp_stats_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	p->id = df->id;
	*c = df->stats;
	if (clear)
		bzero(&df->stats, sizeof(df->stats));
	return (sizeof(*p) + sizeof(*c));
}

static int
mtp_get_stats(mblk_t *dp, const bool clear)
{
	struct mtp_stats *p = (typeof(p)) dp->b_rptr;
	int rtn;

	p->header = (mtp_ulong) drv_hztomsec(jiffies);
	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		rtn = mtp_get_stats_na(p, na_lookup(p->id), clear);
		break;
	case MTP_OBJ_TYPE_SP:
		rtn = mtp_get_stats_sp(p, sp_lookup(p->id), clear);
		break;
	case MTP_OBJ_TYPE_RS:
		rtn = mtp_get_stats_rs(p, rs_lookup(p->id), clear);
		break;
	case MTP_OBJ_TYPE_RL:
		rtn = mtp_get_stats_rl(p, rl_lookup(p->id), clear);
		break;
	case MTP_OBJ_TYPE_RT:
		rtn = mtp_get_stats_rt(p, rt_lookup(p->id), clear);
		break;
	case MTP_OBJ_TYPE_LS:
		rtn = mtp_get_stats_ls(p, ls_lookup(p->id), clear);
		break;
	case MTP_OBJ_TYPE_LK:
		rtn = mtp_get_stats_lk(p, lk_lookup(p->id), clear);
		break;
	case MTP_OBJ_TYPE_SL:
		rtn = mtp_get_stats_sl(p, sl_lookup(p->id), clear);
		break;
	case MTP_OBJ_TYPE_DF:
		rtn = mtp_get_stats_df(p, df_lookup(p->id), clear);
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

static int
mtp_do_notify_na(struct mtp_notify *p, struct na *na, int size, int cmd)
{
	struct mtp_notify_na *c = (typeof(c)) (p + 1);

	if (!na)
		return (-ESRCH);
	p->id = na->id;
	switch (cmd) {
	case MTP_ADD:		/* set */
		na->notify.events |= c->events;
		break;
	case MTP_DEL:		/* clear */
		na->notify.events &= ~c->events;
		break;
	}
	c->events = na->notify.events;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_notify_sp(struct mtp_notify *p, struct sp *sp, int size, int cmd)
{
	struct mtp_notify_sp *c = (typeof(c)) (p + 1);

	if (!sp)
		return (-ESRCH);
	p->id = sp->id;
	switch (cmd) {
	case MTP_ADD:		/* set */
		sp->notify.events |= c->events;
		break;
	case MTP_DEL:		/* clear */
		sp->notify.events &= ~c->events;
		break;
	}
	c->events = sp->notify.events;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_notify_rs(struct mtp_notify *p, struct rs *rs, int size, int cmd)
{
	struct mtp_notify_rs *c = (typeof(c)) (p + 1);

	if (!rs)
		return (-ESRCH);
	p->id = rs->id;
	switch (cmd) {
	case MTP_ADD:		/* set */
		rs->notify.events |= c->events;
		break;
	case MTP_DEL:		/* clear */
		rs->notify.events &= ~c->events;
		break;
	}
	c->events = rs->notify.events;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_notify_rl(struct mtp_notify *p, struct rl *rl, int size, int cmd)
{
	struct mtp_notify_rl *c = (typeof(c)) (p + 1);

	if (!rl)
		return (-ESRCH);
	p->id = rl->id;
	switch (cmd) {
	case MTP_ADD:		/* set */
		rl->notify.events |= c->events;
		break;
	case MTP_DEL:		/* clear */
		rl->notify.events &= ~c->events;
		break;
	}
	c->events = rl->notify.events;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_notify_rt(struct mtp_notify *p, struct rt *rt, int size, int cmd)
{
	struct mtp_notify_rt *c = (typeof(c)) (p + 1);

	if (!rt)
		return (-ESRCH);
	p->id = rt->id;
	switch (cmd) {
	case MTP_ADD:		/* set */
		rt->notify.events |= c->events;
		break;
	case MTP_DEL:		/* clear */
		rt->notify.events &= ~c->events;
		break;
	}
	c->events = rt->notify.events;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_notify_ls(struct mtp_notify *p, struct ls *ls, int size, int cmd)
{
	struct mtp_notify_ls *c = (typeof(c)) (p + 1);

	if (!ls)
		return (-ESRCH);
	p->id = ls->id;
	switch (cmd) {
	case MTP_ADD:		/* set */
		ls->notify.events |= c->events;
		break;
	case MTP_DEL:		/* clear */
		ls->notify.events &= ~c->events;
		break;
	}
	c->events = ls->notify.events;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_notify_lk(struct mtp_notify *p, struct lk *lk, int size, int cmd)
{
	struct mtp_notify_lk *c = (typeof(c)) (p + 1);

	if (!lk)
		return (-ESRCH);
	p->id = lk->id;
	switch (cmd) {
	case MTP_ADD:		/* set */
		lk->notify.events |= c->events;
		break;
	case MTP_DEL:		/* clear */
		lk->notify.events &= ~c->events;
		break;
	}
	c->events = lk->notify.events;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_notify_sl(struct mtp_notify *p, struct sl *sl, int size, int cmd)
{
	struct mtp_notify_sl *c = (typeof(c)) (p + 1);

	if (!sl)
		return (-ESRCH);
	p->id = sl->id;
	switch (cmd) {
	case MTP_ADD:		/* set */
		sl->notify.events |= c->events;
		break;
	case MTP_DEL:		/* clear */
		sl->notify.events &= ~c->events;
		break;
	}
	c->events = sl->notify.events;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_notify_df(struct mtp_notify *p, struct df *df, int size, int cmd)
{
	struct mtp_notify_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	p->id = df->id;
	switch (cmd) {
	case MTP_ADD:		/* set */
		df->notify.events |= c->events;
		break;
	case MTP_DEL:		/* clear */
		df->notify.events &= ~c->events;
		break;
	}
	c->events = df->notify.events;
	return (sizeof(*p) + sizeof(*c));
}

static int
mtp_do_notify(mblk_t *dp, int size, int cmd)
{
	struct mtp_notify *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (cmd) {
	case MTP_GET:		/* get */
	case MTP_ADD:		/* set */
	case MTP_DEL:		/* clear */
		break;
	default:
		return (EFAULT);
	}
	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		rtn = mtp_do_notify_na(p, na_lookup(p->id), size, cmd);
		break;
	case MTP_OBJ_TYPE_SP:
		rtn = mtp_do_notify_sp(p, sp_lookup(p->id), size, cmd);
		break;
	case MTP_OBJ_TYPE_RS:
		rtn = mtp_do_notify_rs(p, rs_lookup(p->id), size, cmd);
		break;
	case MTP_OBJ_TYPE_RL:
		rtn = mtp_do_notify_rl(p, rl_lookup(p->id), size, cmd);
		break;
	case MTP_OBJ_TYPE_RT:
		rtn = mtp_do_notify_rt(p, rt_lookup(p->id), size, cmd);
		break;
	case MTP_OBJ_TYPE_LS:
		rtn = mtp_do_notify_ls(p, ls_lookup(p->id), size, cmd);
		break;
	case MTP_OBJ_TYPE_LK:
		rtn = mtp_do_notify_lk(p, lk_lookup(p->id), size, cmd);
		break;
	case MTP_OBJ_TYPE_SL:
		rtn = mtp_do_notify_sl(p, sl_lookup(p->id), size, cmd);
		break;
	case MTP_OBJ_TYPE_DF:
		rtn = mtp_do_notify_df(p, df_lookup(p->id), size, cmd);
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

static int
mtp_do_statsp_na(struct mtp_stats *p, struct na *na, int cmd)
{
	struct mtp_stats_na *c = (typeof(c)) (p + 1);

	if (!na)
		return (-ESRCH);
	p->id = na->id;
	switch (cmd) {
	case MTP_GET:		/* get */
		*c = na->statsp;
		break;
	case MTP_CHA:		/* set */
		na->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_statsp_sp(struct mtp_stats *p, struct sp *sp, int cmd)
{
	struct mtp_stats_sp *c = (typeof(c)) (p + 1);

	if (!sp)
		return (-ESRCH);
	p->id = sp->id;
	switch (cmd) {
	case MTP_GET:		/* get */
		*c = sp->statsp;
		break;
	case MTP_CHA:		/* set */
		sp->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_statsp_rs(struct mtp_stats *p, struct rs *rs, int cmd)
{
	struct mtp_stats_rs *c = (typeof(c)) (p + 1);

	if (!rs)
		return (-ESRCH);
	p->id = rs->id;
	switch (cmd) {
	case MTP_GET:		/* get */
		*c = rs->statsp;
		break;
	case MTP_CHA:		/* set */
		rs->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_statsp_rl(struct mtp_stats *p, struct rl *rl, int cmd)
{
	struct mtp_stats_rl *c = (typeof(c)) (p + 1);

	if (!rl)
		return (-ESRCH);
	p->id = rl->id;
	switch (cmd) {
	case MTP_GET:		/* get */
		*c = rl->statsp;
		break;
	case MTP_CHA:		/* set */
		rl->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_statsp_rt(struct mtp_stats *p, struct rt *rt, int cmd)
{
	struct mtp_stats_rt *c = (typeof(c)) (p + 1);

	if (!rt)
		return (-ESRCH);
	p->id = rt->id;
	switch (cmd) {
	case MTP_GET:		/* get */
		*c = rt->statsp;
		break;
	case MTP_CHA:		/* set */
		rt->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_statsp_ls(struct mtp_stats *p, struct ls *ls, int cmd)
{
	struct mtp_stats_ls *c = (typeof(c)) (p + 1);

	if (!ls)
		return (-ESRCH);
	p->id = ls->id;
	switch (cmd) {
	case MTP_GET:		/* get */
		*c = ls->statsp;
		break;
	case MTP_CHA:		/* set */
		ls->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_statsp_lk(struct mtp_stats *p, struct lk *lk, int cmd)
{
	struct mtp_stats_lk *c = (typeof(c)) (p + 1);

	if (!lk)
		return (-ESRCH);
	p->id = lk->id;
	switch (cmd) {
	case MTP_GET:		/* get */
		*c = lk->statsp;
		break;
	case MTP_CHA:		/* set */
		lk->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_statsp_sl(struct mtp_stats *p, struct sl *sl, int cmd)
{
	struct mtp_stats_sl *c = (typeof(c)) (p + 1);

	if (!sl)
		return (-ESRCH);
	p->id = sl->id;
	switch (cmd) {
	case MTP_GET:		/* get */
		*c = sl->statsp;
		break;
	case MTP_CHA:		/* set */
		sl->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_do_statsp_df(struct mtp_stats *p, struct df *df, int cmd)
{
	struct mtp_stats_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	p->id = df->id;
	switch (cmd) {
	case MTP_GET:		/* get */
		*c = df->statsp;
		break;
	case MTP_CHA:		/* set */
		df->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}

static int
mtp_do_statsp(mblk_t *dp, int cmd)
{
	struct mtp_stats *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (cmd) {
	case MTP_GET:		/* get */
	case MTP_CHA:		/* set */
		break;
	default:
		return (EFAULT);
	}
	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		rtn = mtp_do_statsp_na(p, na_lookup(p->id), cmd);
		break;
	case MTP_OBJ_TYPE_SP:
		rtn = mtp_do_statsp_sp(p, sp_lookup(p->id), cmd);
		break;
	case MTP_OBJ_TYPE_RS:
		rtn = mtp_do_statsp_rs(p, rs_lookup(p->id), cmd);
		break;
	case MTP_OBJ_TYPE_RL:
		rtn = mtp_do_statsp_rl(p, rl_lookup(p->id), cmd);
		break;
	case MTP_OBJ_TYPE_RT:
		rtn = mtp_do_statsp_rt(p, rt_lookup(p->id), cmd);
		break;
	case MTP_OBJ_TYPE_LS:
		rtn = mtp_do_statsp_ls(p, ls_lookup(p->id), cmd);
		break;
	case MTP_OBJ_TYPE_LK:
		rtn = mtp_do_statsp_lk(p, lk_lookup(p->id), cmd);
		break;
	case MTP_OBJ_TYPE_SL:
		rtn = mtp_do_statsp_sl(p, sl_lookup(p->id), cmd);
		break;
	case MTP_OBJ_TYPE_DF:
		rtn = mtp_do_statsp_df(p, df_lookup(p->id), cmd);
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

/*
 *  LIST Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
/**
 * mtp_lst_na: - list network appearance
 * @p: configuration header
 * @na: id lookup network appearance structure
 *
 * Lists the signalling point code identifiers associated with a specified network appearance, or if
 * no network appearance is specified, lists all network appearance identifiers.
 */
static int
mtp_lst_na(struct mtp_config *p, struct na *na)
{
	struct mtp_config *o = p;
	struct sp *sp;
	int num = p->cmd;

	if (na) {
		o++;
		/* write list of local signalling points */
		for (sp = na->sp.list; sp && num > 0; sp = sp->na.next, num--, o++) {
			o->type = MTP_OBJ_TYPE_SP;
			o->id = sp->id;
			o->cmd = MTP_GET;
		}
	} else {
		num++;
		/* write out list of network appearances */
		for (na = master.na.list; na && num > 0; na = na->next, num--, o++) {
			o->type = MTP_OBJ_TYPE_NA;
			o->id = na->id;
			o->cmd = MTP_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/**
 * mtp_lst_sp: - list signalling point
 * @p: configuration header
 * @sp: id lookup signalling point structure
 *
 * Lists the route set and link set identifiers associated with a specified signalling point, or if
 * no signalling point is specified, lists all signalling point identifiers.
 */
static int
mtp_lst_sp(struct mtp_config *p, struct sp *sp)
{
	struct mtp_config *o = p;
	struct rs *rs;
	struct ls *ls;
	int num = p->cmd;

	if (sp) {
		o++;
		/* write list of routesets */
		for (rs = sp->rs.list; rs && num > 0; rs = rs->sp.next, num--, o++) {
			o->type = MTP_OBJ_TYPE_RS;
			o->id = rs->id;
			o->cmd = MTP_GET;
		}
		/* write list of linksets */
		for (ls = sp->ls.list; ls && num > 0; ls = ls->sp.next, num--, o++) {
			o->type = MTP_OBJ_TYPE_LS;
			o->id = ls->id;
			o->cmd = MTP_GET;
		}
	} else {
		num++;
		/* write out list of signalling points */
		for (sp = master.sp.list; sp && num > 0; sp = sp->next, num--, o++) {
			o->type = MTP_OBJ_TYPE_SP;
			o->id = sp->id;
			o->cmd = MTP_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/**
 * mtp_lst_rs: - list route sets
 * @p: configuration header
 * @rs: id lookup route set structure
 *
 * Lists the route list identifiers associated with a specified route set, or if no route set is
 * specified,  lists all route set identifiers.
 */
static int
mtp_lst_rs(struct mtp_config *p, struct rs *rs)
{
	struct mtp_config *o = p;
	struct rl *rl;
	int num = p->cmd;

	if (rs) {
		o++;
		/* write list of routelists */
		for (rl = rs->rl.list; rl && num > 0; rl = rl->rs.next, num--, o++) {
			o->type = MTP_OBJ_TYPE_RL;
			o->id = rl->id;
			o->cmd = MTP_GET;
		}
	} else {
		num++;
		/* write list of route sets */
		for (rs = master.rs.list; rs && num > 0; rs = rs->next, num--, o++) {
			o->type = MTP_OBJ_TYPE_RS;
			o->id = rs->id;
			o->cmd = MTP_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/**
 * mtp_lst_rl: - list route list
 * @p: configuration header
 * @rl: id lookup of route list structure
 *
 * Lists the route identifiers associated with a specified route list, or if no route list is
 * specified, lists all route list identifiers.
 */
static int
mtp_lst_rl(struct mtp_config *p, struct rl *rl)
{
	struct mtp_config *o = p;
	struct rt *rt;
	int num = p->cmd;

	if (rl) {
		o++;
		/* write list of routes */
		for (rt = rl->rt.list; rt && num > 0; rt = rt->rl.next, num--, o++) {
			o->type = MTP_OBJ_TYPE_RT;
			o->id = rt->id;
			o->cmd = MTP_GET;
		}
	} else {
		num++;
		/* write list of route lists */
		for (rl = master.rl.list; rl && num > 0; rl = rl->next, num--, o++) {
			o->type = MTP_OBJ_TYPE_RL;
			o->id = rl->id;
			o->cmd = MTP_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/**
 * mtp_lst_rt: - list route
 * @p: configuration header
 * @rt: id lookup of route structure
 *
 * Lists nothing associated with a specified route, or if no route is specified, lists all route
 * identifiers.
 */
static int
mtp_lst_rt(struct mtp_config *p, struct rt *rt)
{
	struct mtp_config *o = p;
	int num = p->cmd;

	if (rt) {
		o++;
	} else {
		num++;
		/* write out list of routes */
		for (rt = master.rt.list; rt && num > 0; rt = rt->next, num--, o++) {
			o->type = MTP_OBJ_TYPE_RT;
			o->id = rt->id;
			o->cmd = MTP_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/**
 * mtp_lst_ls: - list combined link set
 * @p: configuration header
 * @ls: id lookup of combined link set
 *
 * Lists link sets associated with a specified combined link set, or if no combined link set is
 * specified, lists all combined link set identifiers.
 */
static int
mtp_lst_ls(struct mtp_config *p, struct ls *ls)
{
	struct mtp_config *o = p;
	struct lk *lk;
	int num = p->cmd;

	if (ls) {
		o++;
		/* write list of links */
		for (lk = ls->lk.list; lk && num > 0; lk = lk->ls.next, num--, o++) {
			o->type = MTP_OBJ_TYPE_LK;
			o->id = lk->id;
			o->cmd = MTP_GET;
		}
	} else {
		num++;
		/* write list of (combined) link sets */
		for (ls = master.ls.list; sl && num > 0; ls = ls->next, num--, o++) {
			o->type = MTP_OBJ_TYPE_LS;
			o->id = ls->id;
			o->cmd = MTP_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/**
 * mtp_lst_lk: - list link set
 * @p: configuration header
 * @lk: id lookup of link set structure
 *
 * Lists signalling links associated with a specified link set, or if no link set is specified,
 * lists all link set identifiers.
 */
static int
mtp_lst_lk(struct mtp_config *p, struct lk *lk)
{
	struct mtp_config *o = p;
	struct sl *sl;
	int num = p->cmd;

	if (lk) {
		o++;
		/* write list of signalling links */
		for (sl = lk->sl.list; sl && num > 0; sl = sl->lk.next, num--, o++) {
			o->type = MTP_OBJ_TYPE_SL;
			o->id = sl->id;
			o->cmd = MTP_GET;
		}
	} else {
		num++;
		/* write list of link (set)s */
		for (lk = master.lk.list; lk && num > 0; lk = lk->next, num--, o++) {
			o->type = MTP_OBJ_TYPE_LK;
			o->id = lk->id;
			o->cmd = MTP_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/**
 * mtp_lst_sl: - list signalling link
 * @p: configuration header
 * @sl: id lookup of signalling link structure
 *
 * Lists nothing associated with a specified signalling link, or if no signalling link is specified,
 * lists all signalling link identifiers.
 */
static int
mtp_lst_sl(struct mtp_config *p, struct sl *sl)
{
	struct mtp_config *o = p;
	int num = p->cmd;

	if (sl) {
		o++;
	} else {
		num++;
		/* write list of signalling links */
		for (sl = master.sl.list; sl && num > 0; sl = sl->next, num--, o++) {
			o->type = MTP_OBJ_TYPE_SL;
			o->id = sl->id;
			o->cmd = MTP_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/**
 * mtp_lst_df: - list default
 * @p: configuration header
 * @df: id lookup of default structure
 *
 * Lists the network appearance identifiers associated with the default, or if no default is
 * specified, lists the default identifier.
 */
static int
mtp_lst_df(struct mtp_config *p, struct df *df)
{
	struct mtp_config *o = p;
	struct na *na;
	int num = p->cmd;

	if (df) {
		o++;
		/* write list of network appearances */
		for (na = df->na.list; na && num > 0; na = na->next, num--, o++) {
			o->type = MTP_OBJ_TYPE_NA;
			o->id = na->id;
			o->cmd = MTP_GET;
		}
	} else {
		num++;
		/* write list of default objects */
		df = &master;
		o->type = MTP_OBJ_TYPE_DF;
		o->id = 0;
		o->cmd = MTP_GET;
		num--;
		o++;
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/**
 * mtp_lst_conf: - list configuration
 * @dp: data block to fill
 *
 * Lists the information associated with the configuration header structure contained in the data
 * buffer.
 */
static int
mtp_lst_conf(mblk_t *dp)
{
	struct mtp_config *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		rtn = mtp_lst_na(p, na_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_SP:
		rtn = mtp_lst_sp(p, sp_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_RS:
		rtn = mtp_lst_rs(p, rs_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_RL:
		rtn = mtp_lst_rl(p, rl_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_RT:
		rtn = mtp_lst_rt(p, rt_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_LS:
		rtn = mtp_lst_ls(p, ls_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_LK:
		rtn = mtp_lst_lk(p, lk_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_SL:
		rtn = mtp_lst_sl(p, sl_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_DF:
		rtn = mtp_lst_df(p, df_lookup(p->id));
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

/*
 *  GET Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
mtp_get_na(struct mtp_config *p, struct na *na)
{
	struct mtp_conf_na *c = (typeof(c)) (p + 1);

	if (!na)
		return (-ESRCH);
	c->options = na->option;
	c->mask = na->mask;
	c->sls_bits = na->sp.sls_bits;
	p->id = na->id;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_sp(struct mtp_config *p, struct sp *sp)
{
	struct mtp_conf_sp *c = (typeof(c)) (p + 1);

	if (!sp)
		return (-ESRCH);
	c->naid = sp->na.na->id;
	c->pc = sp->pc;
	c->users = sp->equipped;
	c->flags = sp->flags;
	p->id = sp->id;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_rs(struct mtp_config *p, struct rs *rs)
{
	struct mtp_conf_rs *c = (typeof(c)) (p + 1);

	if (!rs)
		return (-ESRCH);
	c->spid = rs->sp.sp->id;
	c->dest = rs->dest;
	c->flags = rs->flags;
	p->id = rs->id;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_rl(struct mtp_config *p, struct rl *rl)
{
	struct mtp_conf_rl *c = (typeof(c)) (p + 1);

	if (!rl)
		return (-ESRCH);
	c->rsid = rl->rs.rs->id;
	c->lsid = rl->ls.ls->id;
	c->cost = rl->cost;
	p->id = rl->id;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_rt(struct mtp_config *p, struct rt *rt)
{
	struct mtp_conf_rt *c = (typeof(c)) (p + 1);

	if (!rt)
		return (-ESRCH);
	c->rlid = rt->rl.rl->id;
	c->lsid = rt->ls.ls->id;
	c->slot = rt->slot;
	p->id = rt->id;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_ls(struct mtp_config *p, struct ls *ls)
{
	struct mtp_conf_ls *c = (typeof(c)) (p + 1);

	if (!ls)
		return (-ESRCH);
	c->spid = ls->sp.sp->id;
	c->sls_mask = ls->lk.sls_mask;
	p->id = ls->id;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_lk(struct mtp_config *p, struct lk *lk)
{
	struct mtp_conf_lk *c = (typeof(c)) (p + 1);

	if (!lk)
		return (-ESRCH);
	c->lsid = lk->ls.ls->id;
	c->rsid = lk->rs.rs->id;
	c->ni = lk->ni;
	c->slot = lk->slot;
	p->id = lk->id;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_sl(struct mtp_config *p, struct sl *sl)
{
	struct mtp_conf_sl *c = (typeof(c)) (p + 1);

	if (!sl)
		return (-ESRCH);
	c->muxid = sl->u.mux.index;
	c->lkid = sl->lk.lk->id;
	c->slc = sl->slc;
	p->id = sl->id;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_df(struct mtp_config *p, struct df *df)
{
	struct mtp_conf_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	p->id = df->id;
	return (sizeof(*p) + sizeof(*c));
}
static int
mtp_get_conf(mblk_t *dp)
{
	struct mtp_config *p = (typeof(p)) dp->b_rptr;
	int rtn;

	p->cmd = MTP_GET;
	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		rtn = mtp_get_na(p, na_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_SP:
		rtn = mtp_get_sp(p, sp_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_RS:
		rtn = mtp_get_rs(p, rs_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_RL:
		rtn = mtp_get_rl(p, rl_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_RT:
		rtn = mtp_get_rt(p, rt_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_LS:
		rtn = mtp_get_ls(p, ls_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_LK:
		rtn = mtp_get_lk(p, lk_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_SL:
		rtn = mtp_get_sl(p, sl_lookup(p->id));
		break;
	case MTP_OBJ_TYPE_DF:
		rtn = mtp_get_df(p, df_lookup(p->id));
		break;
	default:
		rare();
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

/*
 *  ADD Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
mtp_add_na(struct mtp_config *p, struct na *na, const bool force, const bool test)
{
	struct mtp_conf_na *c = (typeof(c)) (p + 1);

	if (na)
		return (-EEXIST);
	if (!test) {
		if (!(na = mtp_alloc_na(na_get_id(p->id), c)))
			return (-ENOMEM);
		p->id = na->id;
	}
	return (sizeof(*p));
}
static int
mtp_add_sp(struct mtp_config *p, struct sp *sp, const bool force, const bool test)
{
	struct na *na;
	struct mtp_conf_sp *c = (typeof(c)) (p + 1);

	if (sp)
		return (-EEXIST);
	/* network appearance must exist */
	if (!(na = na_lookup(c->naid)))
		return (-ESRCH);
	if (!force) {
		/* signalling point point code must not be assigned yet */
		for (sp = na->sp.list; sp && sp->pc != c->pc; sp = sp->na.next) ;
		if (sp)
			return (-EADDRINUSE);
	}
	if (!test) {
		if (!(sp = mtp_alloc_sp(sp_get_id(p->id), na, c)))
			return (-ENOMEM);
		p->id = sp->id;
	}
	return (sizeof(*p));
}
static int
mtp_add_rs(struct mtp_config *p, struct rs *rs, const bool force, const bool test)
{
	struct sp *sp;
	struct mtp_conf_rs *c = (typeof(c)) (p + 1);

	if (rs)
		return (-EEXIST);
	/* signalling point must exist */
	if (!(sp = sp_lookup(c->spid)))
		return (-ESRCH);
	if (!force) {
		/* route set point code must not be assigned yet */
		for (rs = sp->rs.list; rs && rs->dest != c->dest; rs = rs->sp.next) ;
		if (rs)
			return (-EADDRINUSE);
	}
	if (!test) {
		if (!(rs = mtp_alloc_rs(rs_get_id(p->id), sp, c)))
			return (-ENOMEM);
		p->id = rs->id;
	}
	return (sizeof(*p));
}
static int
mtp_add_rl(struct mtp_config *p, struct rl *rl, const bool force, const bool test)
{
	struct rs *rs;
	struct ls *ls;
	struct mtp_conf_rl *c = (typeof(c)) (p + 1);

	if (rl)
		return (-EEXIST);
	/* combined link set must exist */
	if (!(ls = ls_lookup(c->lsid)))
		return (-ESRCH);
	/* route set must exist */
	if (!(rs = rs_lookup(c->rsid)))
		return (-ESRCH);
	/* combined link set must belong to the same sp */
	if (rs->sp.sp != ls->sp.sp)
		return (-EINVAL);
	if (!test) {
		if (!(rl = mtp_alloc_rl(rl_get_id(p->id), rs, ls, c)))
			return (-ENOMEM);
		p->id = rl->id;
	}
	return (sizeof(*p));
}
static int
mtp_add_rt(struct mtp_config *p, struct rt *rt, const bool force, const bool test)
{
	struct rl *rl;
	struct lk *lk = NULL;
	struct ls *ls;
	struct mtp_conf_rt *c = (typeof(c)) (p + 1);

	if (rt)
		return (-EEXIST);
	/* route list must exist */
	if (!(rl = rl_lookup(c->rlid)))
		return (-ESRCH);
	/* link set must exist */
	if (!(lk = lk_lookup(c->lkid)))
		return (-ESRCH);
	/* link must belong to same combined link set */
	if (rl->rs.rs->ls.ls != lk->ls.ls)
		return (-EINVAL);
	if (!test) {
		if (!(rt = mtp_alloc_rt(rt_get_id(p->id), rl, lk, c)))
			return (-ENOMEM);
		p->id = rt->id;
	}
	return (sizeof(*p));
}
static int
mtp_add_ls(struct mtp_config *p, struct ls *ls, const bool force, const bool test)
{
	struct sp *sp;
	struct mtp_conf_ls *c = (typeof(c)) (p + 1);

	if (ls)
		return (-EEXIST);
	/* signalling point must exist */
	if (!(sp = sp_lookup(c->spid)))
		return (-ESRCH);
	if (!test) {
		if (!(ls = mtp_alloc_ls(ls_get_id(p->id), sp, c)))
			return (-ENOMEM);
		p->id = ls->id;
	}
	return (sizeof(*p));
}
static int
mtp_add_lk(struct mtp_config *p, struct lk *lk, const bool force, const bool test)
{
	struct ls *ls;
	struct rs *rs;
	struct mtp_conf_lk *c = (typeof(c)) (p + 1);

	if (lk)
		return (-EEXIST);
	/* route set must exist */
	if (!(rs = rs_lookup(c->rsid)))
		return (-ESRCH);
	/* adjacent route set must not already be assigned */
	for (ls = sp->ls.list; ls; ls = ls->sp.next)
		for (lk = ls->lk.list; lk; lk = lk->ls.next)
			if (lk->sp.adj == rs)
				return (-EADDRINUSE);
	/* (combined) link set must exist */
	if (!(ls = ls_lookup(c->lsid)))
		return (-ESRCH);
	/* link set must belong to the same sp */
	if (ls->sp.sp != rs->sp.sp)
		return (-EINVAL);
	/* no other link must be assigned to same slot */
	for (lk = ls->lk.list; lk; lk = lk->ls.next)
		if (lk->slot == c->slot)
			return (-EBUSY);
	if (!test) {
		if (!(lk = mtp_alloc_lk(lk_get_id(p->id), ls, rs, rs->sp.sp, c)))
			return (-ENOMEM);
		p->id = lk->id;
	}
	return (sizeof(*p));
}
static int
mtp_add_sl(struct mtp_config *p, struct sl *sl, const bool force, const bool test)
{
	struct lk *lk;
	struct mtp_conf_sl *c = (typeof(c)) (p + 1);

	if (sl)
		return (-EEXIST);
	/* link (set) must already exist */
	if (!(lk = lk_lookup(c->lkid)))
		return (-ESRCH);
	/* signalling link code must be unique */
	for (sl = lk->sl.list; sl; sl = sl->lk.next)
		if (sl->slc == c->slc)
			return (-EBUSY);
	/* signalling link must be linked */
	if (!(sl = sl_lookup_mux(c->muxid)))
		return (-ESRCH);
	if (!test) {
		if (!(sl = mtp_alloc_sl(sl_get_id(p->id), sl, lk, c)))
			return (-ENOMEM);
		p->id = lk->id;
	}
	return (sizeof(*p));
}
static int
mtp_add_df(struct mtp_config *p, struct df *df, const bool force, const bool test)
{
	struct mtp_conf_df *c = (typeof(c)) (p + 1);

	if (df)
		return (-EEXIST);
	/* DF objects are not added statically */
	return (-EINVAL);
}
static int
mtp_add_conf(struct mtp_config *p, const bool force, const bool test)
{
	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		return mtp_add_na(p, na_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_SP:
		return mtp_add_sp(p, sp_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_RS:
		return mtp_add_rs(p, rs_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_RL:
		return mtp_add_rl(p, rl_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_RT:
		return mtp_add_rt(p, rt_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_LS:
		return mtp_add_ls(p, ls_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_LK:
		return mtp_add_lk(p, lk_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_SL:
		return mtp_add_sl(p, sl_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_DF:
		return mtp_add_df(p, df_lookup(p->id), force, test);
	default:
		rare();
		return (-EINVAL);
	}
}

/*
 *  CHA Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
mtp_cha_na(struct mtp_config *p, struct na *na, const bool force, const bool test)
{
	struct mtp_conf_na *c = (typeof(c)) (p + 1);

	if (!na)
		return (-ESRCH);
	if (!force) {
	}
	if (!test) {
		na->option = c->options;
		na->mask.member = c->mask.member;
		na->mask.cluster = c->mask.cluster;
		na->mask.network = c->mask.network;
		na->sp.sls_bits = c->sls_bits;
		na->sp.sls_mask = ((1 << c->sls_bits) - 1);
	}
	return (0);
}
static int
mtp_cha_sp(struct mtp_config *p, struct sp *sp, const bool force, const bool test)
{
	struct sp *s;
	struct mtp_conf_sp *c = (typeof(c)) (p + 1);

	if (!sp)
		return (-ESRCH);
	if (c->naid && c->naid != sp->na.na->id)
		return (-EINVAL);
	for (s = sp->na.na->sp.list; s; s = s->na.next)
		if (s != sp && s->pc == c->pc)
			return (-EINVAL);
	if (!force) {
		int i;

		/* have users */
		for (i = 0; i < 16; i++)
			if (sp->mtp.lists[i])
				return (-EBUSY);
		/* have linksets */
		if (sp->ls.list)
			return (-EBUSY);
	}
	if (!test) {
		uint mask =
		    (SPF_CLUSTER | SPF_LOSC_PROC_A | SPF_LOSC_PROC_B | SPF_SECURITY |
		     SPF_XFER_FUNC);

		sp->pc = c->pc;
		sp->mtp.equipped = c->users | sp->mtp.available;
		sp->flags &= ~mask;
		sp->flags |= c->flags & mask;
	}
	return (0);
}
static int
mtp_cha_rs(struct mtp_config *p, struct rs *rs, const bool force, const bool test)
{
	struct rs *r;
	struct mtp_conf_rs *c = (typeof(c)) (p + 1);

	if (!rs)
		return (-ESRCH);
	if (c->spid && c->spid != rs->sp.sp->id)
		return (-EINVAL);
	for (r = rs->sp.sp->rs.list; r; r = r->sp.next)
		if (r != rs && r->dest == c->dest)
			return (-EINVAL);
	if (!force) {
		/* have route lists */
		if (rs->rl.list)
			return (-EBUSY);
	}
	if (!test) {
		uint mask =
		    (RSF_TFR_PENDING | RSF_ADJACENT | RSF_CLUSTER | RSF_XFER_FUNC);

		rs->dest = c->dest;
		rs->flags &= ~mask;
		rs->flags |= c->flags & mask;
	}
	return (0);
}
static int
mtp_cha_rl(struct mtp_config *p, struct rl *rl, const bool force, const bool test)
{
	struct rl *r;
	struct mtp_conf_rl *c = (typeof(c)) (p + 1);

	if (!rl)
		return (-ESRCH);
	if (c->rsid && c->rsid != rl->rs.rs->id)
		return (-EINVAL);
	if (c->lsid && c->lsid != rl->ls.ls->id)
		return (-EINVAL);
	for (r = rl->rs.rs->rl.list; r; r = r->rs.next)
		if (r != rl && r->cost == c->cost)
			return (-EINVAL);
	if (!force) {
		if (rl->rt.list || rl->cr.list)
			return (-EBUSY);
	}
	if (!test) {
		rl->cost = c->cost;
	}
	return (0);
}
static int
mtp_cha_rt(struct mtp_config *p, struct rt *rt, const bool force, const bool test)
{
	struct rt *r;
	struct mtp_conf_rt *c = (typeof(c)) (p + 1);

	if (!rt)
		return (-ESRCH);
	if (c->rlid && c->rlid != rt->rl.rl->id)
		return (-EINVAL);
	if (c->lkid && c->lkid != rt->lk.lk->id)
		return (-EINVAL);
	for (r = rt->rl.rl->rt.list; r; r = r->next)
		if (r != rt && r->slot == c->slot)
			return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		rt->slot = c->slot;
	}
	return (0);
}
static int
mtp_cha_ls(struct mtp_config *p, struct ls *ls, const bool force, const bool test)
{
	struct mtp_conf_ls *c = (typeof(c)) (p + 1);

	if (!ls)
		return (-ESRCH);
	if (c->spid && c->spid != ls->sp.sp->id)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		struct sp *sp = ls->sp.sp;
		uint mask = c->sls_mask;

		ls->lk.sls_mask = c->sls_mask;
		ls->lk.sls_bits = 0;
		/* count the 1's in the mask */
		while (mask) {
			if (mask & 0x01)
				ls->lk.sls_bits++;
			mask >>= 1;
		}
		ls->rl.sls_mask = ~c->sls_mask & sp->ls.sls_mask;
		ls->rl.sls_bits = sp->ls.sls_bits - ls->lk.sls_bits;
	}
	return (0);
}
static int
mtp_cha_lk(struct mtp_config *p, struct lk *lk, const bool force, const bool test)
{
	struct mtp_conf_lk *c = (typeof(c)) (p + 1);

	if (!lk)
		return (-ESRCH);
	if (c->lsid && c->lsid != lk->ls.ls->id)
		return (-EINVAL);
	if (c->rsid && c->rsid != lk->sp.adj->id)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		lk->ni = c->ni;
		lk->slot = c->slot;
	}
	return (0);
}
static int
mtp_cha_sl(struct mtp_config *p, struct sl *sl, const bool force, const bool test)
{
	struct sl *s;
	struct mtp_conf_sl *c = (typeof(c)) (p + 1);

	if (!sl)
		return (-ESRCH);
	if (!sl->lk.lk)
		return (-EINVAL);
	if (c->muxid && c->muxid != sl->u.mux.index)
		return (-EINVAL);
	if (c->lkid && c->lkid != sl->lk.lk->id)
		return (-EINVAL);
	for (s = sl->lk.lk->sl.list; s; s = s->lk.next)
		if (s != sl && s->slc == c->slc)
			return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		sl->slc = c->slc;
	}
	return (0);
}
static int
mtp_cha_df(struct mtp_config *p, struct df *df, const bool force, const bool test)
{
	struct mtp_conf_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	if (!force) {
	}
	if (!test) {
	}
	return (0);
}
static int
mtp_cha_conf(struct mtp_config *p, const bool force, const bool test)
{
	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		return mtp_cha_na(p, na_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_SP:
		return mtp_cha_sp(p, sp_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_RS:
		return mtp_cha_rs(p, rs_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_RL:
		return mtp_cha_rl(p, rl_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_RT:
		return mtp_cha_rt(p, rt_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_LS:
		return mtp_cha_ls(p, ls_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_LK:
		return mtp_cha_lk(p, lk_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_SL:
		return mtp_cha_sl(p, sl_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_DF:
		return mtp_cha_df(p, df_lookup(p->id), force, test);
	default:
		rare();
		return (-EINVAL);
	}
}

/*
 *  DEL Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
mtp_del_na(struct mtp_config *p, struct na *na, const bool force, const bool test)
{
	if (!na)
		return (-ESRCH);
	if (!force) {
		/* bound to internal data structures */
		if (na->sp.list)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_na(na);
	}
	return (0);
}
static int
mtp_del_sp(struct mtp_config *p, struct sp *sp, const bool force, const bool test)
{
	if (!sp)
		return (-ESRCH);
	if (!force) {
		int i;

		/* bound to internal data structures */
		if (sp->ls.list || sp->rs.list)
			return (-EBUSY);
		for (i = 0; i < 16; i++)
			if (sp->mtp.lists[i])
				return (-EBUSY);
	}
	if (!test) {
		mtp_free_sp(sp);
	}
	return (0);
}
static int
mtp_del_rs(struct mtp_config *p, struct rs *rs, const bool force, const bool test)
{
	if (!rs)
		return (-ESRCH);
	if (!force) {
		/* bound to internal data structures */
		if (rs->rl.list || rs->rr.list)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_rs(rs);
	}
	return (0);
}
static int
mtp_del_rl(struct mtp_config *p, struct rl *rl, const bool force, const bool test)
{
	if (!rl)
		return (-ESRCH);
	if (!force) {
		/* bound to internal data structures */
		if (rl->rt.list || rl->cr.list)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_rl(rl);
	}
	return (0);
}
static int
mtp_del_rt(struct mtp_config *p, struct rt *rt, const bool force, const bool test)
{
	if (!rt)
		return (-ESRCH);
	if (!force) {
		/* bound to internal data structures */
	}
	if (!test) {
		mtp_free_rt(rt);
	}
	return (0);
}
static int
mtp_del_ls(struct mtp_config *p, struct ls *ls, const bool force, const bool test)
{
	if (!ls)
		return (-ESRCH);
	if (!force) {
		/* bound to internal data structures */
	}
	if (!test) {
		mtp_free_ls(ls);
	}
	return (0);
}
static int
mtp_del_lk(struct mtp_config *p, struct lk *lk, const bool force, const bool test)
{
	if (!lk)
		return (-ESRCH);
	if (!force) {
		/* bound to internal data structures */
		if (lk->rt.list || lk->cb.list || lk->sl.list)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_lk(lk);
	}
	return (0);
}
static int
mtp_del_sl(struct mtp_config *p, struct sl *sl, const bool force, const bool test)
{
	if (!sl)
		return (-ESRCH);
	if (!force) {
		/* bound to internal datastructures */
		if (sl->lk.lk)
			return (-EBUSY);
	}
	if (!test) {
		mtp_free_sl(sl);
	}
	return (0);
}
static int
mtp_del_df(struct mtp_config *p, struct df *df, const bool force, const bool test)
{
	struct mtp_conf_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	if (!force) {
		return (-EBUSY);
	}
	if (!test) {
		/* can't delete default */
	}
	return (0);
}
static int
mtp_del_conf(struct mtp_config *p, const bool force, const bool test)
{
	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		return mtp_del_na(p, na_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_SP:
		return mtp_del_sp(p, sp_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_RS:
		return mtp_del_rs(p, rs_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_RL:
		return mtp_del_rl(p, rl_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_RT:
		return mtp_del_rt(p, rt_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_LS:
		return mtp_del_ls(p, ls_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_LK:
		return mtp_del_lk(p, lk_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_SL:
		return mtp_del_sl(p, sl_lookup(p->id), force, test);
	case MTP_OBJ_TYPE_DF:
		return mtp_del_df(p, df_lookup(p->id), force, test);
	default:
		rare();
		return (-EINVAL);
	}
}

static int
mtp_do_conf(mblk_t *dp, const bool force, const bool test)
{
	struct mtp_config *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (p->cmd) {
	case MTP_ADD:
		rtn = mtp_add_conf(p, force, test);
		break;
	case MTP_CHA:
		rtn = mtp_cha_conf(p, force, test);
		break;
	case MTP_DEL:
		rtn = mtp_del_conf(p, force, test);
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}
static int
mtp_set_conf(mblk_t *dp)
{
	return mtp_do_conf(dp, false, false);
}
static int
mtp_test_conf(mblk_t *dp)
{
	return mtp_do_conf(dp, false, true);
}
static int
mtp_commit_conf(mblk_t *dp)
{
	return mtp_do_conf(dp, true, false);
}

/*
 *  MGMT - Manage Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
mtp_mgmt_sl(queue_t *q, struct sl *sl, uint action)
{
	if (!sl)
		return (-ESRCH);
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
	}
	return (0);
}
static int
mtp_mgmt_lk(queue_t *q, struct lk *lk, uint action)
{
	int err = 0;
	struct sl *sl;

	if (!lk)
		return (-ESRCH);
	switch (action) {
	case MTP_MGMT_ALLOW:
	case MTP_MGMT_RESTRICT:
	case MTP_MGMT_PROHIBIT:
	case MTP_MGMT_ACTIVATE:
	case MTP_MGMT_DEACTIVATE:
	case MTP_MGMT_BLOCK:
	case MTP_MGMT_UNBLOCK:
	case MTP_MGMT_INHIBIT:
	case MTP_MGMT_UNINHIBIT:
	case MTP_MGMT_CONGEST:
	case MTP_MGMT_UNCONGEST:
	case MTP_MGMT_DANGER:
	case MTP_MGMT_NODANGER:
	case MTP_MGMT_RESTART:
	case MTP_MGMT_RESTARTED:
	default:
		return (0);
	}
	for (sl = lk->sl.list; sl; sl = sl->lk.next)
		if ((err = mtp_mgmt_sl(q, sl, action)))
			return (err);
	return (err);
}
static int
mtp_mgmt_ls(queue_t *q, struct ls *ls, uint action)
{
	int err = 0;
	struct lk *lk;

	if (!ls)
		return (-ESRCH);
	switch (action) {
	case MTP_MGMT_ALLOW:
	case MTP_MGMT_RESTRICT:
	case MTP_MGMT_PROHIBIT:
	case MTP_MGMT_ACTIVATE:
	case MTP_MGMT_DEACTIVATE:
	case MTP_MGMT_BLOCK:
	case MTP_MGMT_UNBLOCK:
	case MTP_MGMT_INHIBIT:
	case MTP_MGMT_UNINHIBIT:
	case MTP_MGMT_CONGEST:
	case MTP_MGMT_UNCONGEST:
	case MTP_MGMT_DANGER:
	case MTP_MGMT_NODANGER:
	case MTP_MGMT_RESTART:
	case MTP_MGMT_RESTARTED:
	default:
		return (0);
	}
	for (lk = ls->lk.list; lk; lk = lk->ls.next)
		if ((err = mtp_mgmt_lk(q, lk, action)))
			return (err);
	return (err);
}
static int
mtp_mgmt_rt(queue_t *q, struct rt *rt, uint action)
{
	if (!rt)
		return (-ESRCH);
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
	}
	return (0);
}
static int
mtp_mgmt_rl(queue_t *q, struct rl *rl, uint action)
{
	int err = 0;
	struct rt *rt;

	if (!rl)
		return (-ESRCH);
	switch (action) {
	case MTP_MGMT_ALLOW:
	case MTP_MGMT_RESTRICT:
	case MTP_MGMT_PROHIBIT:
	case MTP_MGMT_ACTIVATE:
	case MTP_MGMT_DEACTIVATE:
	case MTP_MGMT_BLOCK:
	case MTP_MGMT_UNBLOCK:
	case MTP_MGMT_INHIBIT:
	case MTP_MGMT_UNINHIBIT:
	case MTP_MGMT_CONGEST:
	case MTP_MGMT_UNCONGEST:
	case MTP_MGMT_DANGER:
	case MTP_MGMT_NODANGER:
	case MTP_MGMT_RESTART:
	case MTP_MGMT_RESTARTED:
	default:
		return (0);
	}
	for (rt = rl->rt.list; rt; rt = rt->rl.next)
		if ((err = mtp_mgmt_rt(q, rt, action)))
			return (err);
	return (err);
}
static int
mtp_mgmt_rs(queue_t *q, struct rs *rs, uint action)
{
	int err = 0;
	struct rl *rl;

	if (!rs)
		return (-ESRCH);
	switch (action) {
	case MTP_MGMT_ALLOW:
	case MTP_MGMT_RESTRICT:
	case MTP_MGMT_PROHIBIT:
	case MTP_MGMT_ACTIVATE:
	case MTP_MGMT_DEACTIVATE:
	case MTP_MGMT_BLOCK:
	case MTP_MGMT_UNBLOCK:
	case MTP_MGMT_INHIBIT:
	case MTP_MGMT_UNINHIBIT:
	case MTP_MGMT_CONGEST:
	case MTP_MGMT_UNCONGEST:
	case MTP_MGMT_DANGER:
	case MTP_MGMT_NODANGER:
	case MTP_MGMT_RESTART:
	case MTP_MGMT_RESTARTED:
	default:
		return (0);
	}
	for (rl = rs->rl.list; rl; rl = rl->rs.next)
		if ((err = mtp_mgmt_rl(q, rl, action)))
			return (err);
	return (err);
}
static int
mtp_mgmt_sp(queue_t *q, struct sp *sp, uint action)
{
	int err = 0;
	struct rs *rs;
	struct ls *ls;

	if (!sp)
		return (-ESRCH);
	switch (action) {
	case MTP_MGMT_ALLOW:
	case MTP_MGMT_RESTRICT:
	case MTP_MGMT_PROHIBIT:
	case MTP_MGMT_ACTIVATE:
	case MTP_MGMT_DEACTIVATE:
	case MTP_MGMT_BLOCK:
	case MTP_MGMT_UNBLOCK:
	case MTP_MGMT_INHIBIT:
	case MTP_MGMT_UNINHIBIT:
	case MTP_MGMT_CONGEST:
	case MTP_MGMT_UNCONGEST:
	case MTP_MGMT_DANGER:
	case MTP_MGMT_NODANGER:
	case MTP_MGMT_RESTART:
	case MTP_MGMT_RESTARTED:
		break;
	default:
		return (0);
	}
	for (rs = sp->rs.list; rs; rs = rs->sp.next)
		if ((err = mtp_mgmt_rs(q, rs, action)))
			return (err);
	for (ls = sp->ls.list; ls; ls = ls->sp.next)
		if ((err = mtp_mgmt_ls(q, ls, action)))
			return (err);
	return (err);
}
static int
mtp_mgmt_na(queue_t *q, struct na *na, uint action)
{
	int err = 0;
	struct sp *sp;

	if (!na)
		return (-ESRCH);
	/* we actually don't manage network appearances, just all of the signalling
	   points that make up a network appearance. */
	for (sp = na->sp.list; sp; sp = sp->na.next)
		if ((err = mtp_mgmt_sp(q, sp, action)))
			return (err);
	return (err);
}
static int
mtp_mgmt_df(queue_t *q, struct df *df, uint action)
{
	int err = 0;
	struct na *na;

	if (!df)
		return (-ESRCH);
	/* we actually don't manage the default object, just all of the network
	   appearances that make up the stack. */
	for (na = df->na.list; na; na = na->next)
		if ((err = mtp_mgmt_na(q, na, action)))
			return (err);
	return (err);
}

static int
mtp_action(queue_t *q, mblk_t *dp)
{
	struct mtp_mgmt *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		rtn = mtp_mgmt_na(q, na_lookup(p->id), p->cmd);
		break;
	case MTP_OBJ_TYPE_SP:
		rtn = mtp_mgmt_sp(q, sp_lookup(p->id), p->cmd);
		break;
	case MTP_OBJ_TYPE_RS:
		rtn = mtp_mgmt_rs(q, rs_lookup(p->id), p->cmd);
		break;
	case MTP_OBJ_TYPE_RL:
		rtn = mtp_mgmt_rl(q, rl_lookup(p->id), p->cmd);
		break;
	case MTP_OBJ_TYPE_RT:
		rtn = mtp_mgmt_rt(q, rt_lookup(p->id), p->cmd);
		break;
	case MTP_OBJ_TYPE_LS:
		rtn = mtp_mgmt_ls(q, ls_lookup(p->id), p->cmd);
		break;
	case MTP_OBJ_TYPE_LK:
		rtn = mtp_mgmt_lk(q, lk_lookup(p->id), p->cmd);
		break;
	case MTP_OBJ_TYPE_SL:
		rtn = mtp_mgmt_sl(q, sl_lookup(p->id), p->cmd);
		break;
	case MTP_OBJ_TYPE_DF:
		rtn = mtp_mgmt_df(q, df_lookup(p->id), p->cmd);
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	return (0);
}

static int
mtp_size_conf(unsigned char *arg, int *sizep)
{
	struct mtp_config *p = (typeof(p)) arg;

	switch (p->cmd) {
	case MTP_ADD:
		break;
	case MTP_CHA:
	case MTP_DEL:
		break;
	default:
	case MTP_GET:
		return (EINVAL);
	}
	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		if (p->cmd != MTP_ADD && !na_lookup(p->id))
			return (ESRCH);
		*sizep = sizeof(struct mtp_conf_na);
		return (0);
	case MTP_OBJ_TYPE_SP:
		if (p->cmd != MTP_ADD && !sp_lookup(p->id))
			return (ESRCH);
		*sizep = sizeof(struct mtp_conf_sp);
		return (0);
	case MTP_OBJ_TYPE_RS:
		if (p->cmd != MTP_ADD && !rs_lookup(p->id))
			return (ESRCH);
		*sizep = sizeof(struct mtp_conf_rs);
		return (0);
	case MTP_OBJ_TYPE_RL:
		if (p->cmd != MTP_ADD && !rl_lookup(p->id))
			return (ESRCH);
		*sizep = sizeof(struct mtp_conf_rl);
		return (0);
	case MTP_OBJ_TYPE_RT:
		if (p->cmd != MTP_ADD && !rt_lookup(p->id))
			return (ESRCH);
		*sizep = sizeof(struct mtp_conf_rt);
		return (0);
	case MTP_OBJ_TYPE_LS:
		if (p->cmd != MTP_ADD && !ls_lookup(p->id))
			return (ESRCH);
		*sizep = sizeof(struct mtp_conf_ls);
		return (0);
	case MTP_OBJ_TYPE_LK:
		if (p->cmd != MTP_ADD && !lk_lookup(p->id))
			return (ESRCH);
		*sizep = sizeof(struct mtp_conf_lk);
		return (0);
	case MTP_OBJ_TYPE_SL:
		if (p->cmd != MTP_ADD && !sl_lookup(p->id))
			return (ESRCH);
		*sizep = sizeof(struct mtp_conf_sl);
		return (0);
	case MTP_OBJ_TYPE_DF:
		if (p->cmd != MTP_ADD && !df_lookup(p->id))
			return (ESRCH);
		*sizep = sizeof(struct mtp_conf_df);
		return (0);
	default:
		return (EINVAL);
	}
}
static int
mtp_size_opt_conf(unsigned char *arg, int *sizep)
{
	struct mtp_option *p = (typeof(p)) arg;

	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		*sizep = sizeof(struct mtp_opt_conf_na);
		return (0);
	case MTP_OBJ_TYPE_SP:
		*sizep = sizeof(struct mtp_opt_conf_sp);
		return (0);
	case MTP_OBJ_TYPE_RS:
		*sizep = sizeof(struct mtp_opt_conf_rs);
		return (0);
	case MTP_OBJ_TYPE_RL:
		*sizep = sizeof(struct mtp_opt_conf_rl);
		return (0);
	case MTP_OBJ_TYPE_RT:
		*sizep = sizeof(struct mtp_opt_conf_rt);
		return (0);
	case MTP_OBJ_TYPE_LS:
		*sizep = sizeof(struct mtp_opt_conf_ls);
		return (0);
	case MTP_OBJ_TYPE_LK:
		*sizep = sizeof(struct mtp_opt_conf_lk);
		return (0);
	case MTP_OBJ_TYPE_SL:
		*sizep = sizeof(struct mtp_opt_conf_sl);
		return (0);
	case MTP_OBJ_TYPE_DF:
		*sizep = sizeof(struct mtp_opt_conf_df);
		return (0);
	default:
		return (EINVAL);
	}
}
static int
mtp_size_stats(unsigned char *arg, int *sizep)
{
	struct mtp_stats *p = (typeof(p)) arg;

	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		*sizep = sizeof(struct mtp_stats_na);
		return (0);
	case MTP_OBJ_TYPE_SP:
		*sizep = sizeof(struct mtp_stats_sp);
		return (0);
	case MTP_OBJ_TYPE_RS:
		*sizep = sizeof(struct mtp_stats_rs);
		return (0);
	case MTP_OBJ_TYPE_RL:
		*sizep = sizeof(struct mtp_stats_rl);
		return (0);
	case MTP_OBJ_TYPE_RT:
		*sizep = sizeof(struct mtp_stats_rt);
		return (0);
	case MTP_OBJ_TYPE_LS:
		*sizep = sizeof(struct mtp_stats_ls);
		return (0);
	case MTP_OBJ_TYPE_LK:
		*sizep = sizeof(struct mtp_stats_lk);
		return (0);
	case MTP_OBJ_TYPE_SL:
		*sizep = sizeof(struct mtp_stats_sl);
		return (0);
	case MTP_OBJ_TYPE_DF:
		*sizep = sizeof(struct mtp_stats_df);
		return (0);
	default:
		return (EINVAL);
	}
}
static int
mtp_size_statem(unsigned char *arg, int *sizep)
{
	struct mtp_statem *p = (typeof(p)) arg;

	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		*sizep = sizeof(struct mtp_statem_na);
		return (0);
	case MTP_OBJ_TYPE_SP:
		*sizep = sizeof(struct mtp_statem_sp);
		return (0);
	case MTP_OBJ_TYPE_RS:
		*sizep = sizeof(struct mtp_statem_rs);
		return (0);
	case MTP_OBJ_TYPE_RL:
		*sizep = sizeof(struct mtp_statem_rl);
		return (0);
	case MTP_OBJ_TYPE_RT:
		*sizep = sizeof(struct mtp_statem_rt);
		return (0);
	case MTP_OBJ_TYPE_LS:
		*sizep = sizeof(struct mtp_statem_ls);
		return (0);
	case MTP_OBJ_TYPE_LK:
		*sizep = sizeof(struct mtp_statem_lk);
		return (0);
	case MTP_OBJ_TYPE_SL:
		*sizep = sizeof(struct mtp_statem_sl);
		return (0);
	case MTP_OBJ_TYPE_DF:
		*sizep = sizeof(struct mtp_statem_df);
		return (0);
	default:
		return (EINVAL);
	}
}
static int
mtp_size_notify(unsigned char *arg, int *sizep)
{
	struct mtp_notify *p = (typeof(p)) arg;

	switch (p->type) {
	case MTP_OBJ_TYPE_NA:
		*sizep = sizeof(struct mtp_notify_na);
		return (0);
	case MTP_OBJ_TYPE_SP:
		*sizep = sizeof(struct mtp_notify_sp);
		return (0);
	case MTP_OBJ_TYPE_RS:
		*sizep = sizeof(struct mtp_notify_rs);
		return (0);
	case MTP_OBJ_TYPE_RL:
		*sizep = sizeof(struct mtp_notify_rl);
		return (0);
	case MTP_OBJ_TYPE_RT:
		*sizep = sizeof(struct mtp_notify_rt);
		return (0);
	case MTP_OBJ_TYPE_LS:
		*sizep = sizeof(struct mtp_notify_ls);
		return (0);
	case MTP_OBJ_TYPE_LK:
		*sizep = sizeof(struct mtp_notify_lk);
		return (0);
	case MTP_OBJ_TYPE_SL:
		*sizep = sizeof(struct mtp_notify_sl);
		return (0);
	case MTP_OBJ_TYPE_DF:
		*sizep = sizeof(struct mtp_notify_df);
		return (0);
	default:
		return (EINVAL);
	}
}

/*
 *  MTP_IOCCPASS    - mtp_pass_t
 *  -------------------------------------------------------------------
 */
static int
mtp_ioccpass(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		mtp_pass_t *p = (typeof(p)) mp->b_cont->b_rptr;
		mblk_t *bp, *dp;
		struct sl *sl;

		for (sl = master.sl.list; sl && sl->u.mux.index != p->muxid;
		     sl = sl->next) ;
		if (!sl || !sl->wq)
			return (-EINVAL);
		if (p->type < QPCTL && !canputnext(sl->wq))
			return (-EBUSY);
		if (!(bp = ss7_dupb(q, mp)))
			return (-ENOBUFS);
		if (!(dp = ss7_dupb(q, mp))) {
			freeb(bp);
			return (-ENOBUFS);
		}
		bp->b_datap->db_type = p->type;
		bp->b_band = p->band;
		bp->b_cont = dp;
		bp->b_rptr += sizeof(*p);
		bp->b_wptr = bp->b_rptr + p->ctl_length;
		dp->b_datap->db_type = M_DATA;
		dp->b_rptr += sizeof(*p) + p->ctl_length;
		dp->b_wptr = dp->b_rptr + p->dat_length;
		putnext(sl->wq, bp);
		return (0);
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
/**
 * mtp_i_link: - perform I_LINK operation
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * The driver spuports I_LINK operations; however, any SL Stream that is
 * linked with an I_LINK operation can only be managed by the control Stream
 * linking the lower Stream and cannot be shared across other upper Streams
 * unless configured against a SPID.
 *
 * Note that if this is not the first SL linked and there are running Users,
 * this SL will not be available to them until it is conifgured and brought to
 * the active state.  If this is the first SL, there cannot be running users.
 */
static int
mtp_i_link(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	lmi_info_req_t *p;
	unsigned long flags;
	struct sp *sp;
	struct sl *sl = NULL;
	mblk_t *rp = NULL;
	int err;

	if (!(rp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		err = -ENOBUFS;
		goto error;
	}
	if (!(sl = mi_open_alloc(sizeof(*sl)))) {
		err = ENOMEM;
		goto error;
	} else {
		bzero(sl, sizeof(*sl));
		sl->wq = WR(l->l_qtop);
		sl->id = l->l_index;
		sl->i.state = LMI_UNATTACHED;
		sl->i.style = LMI_STYLE2;
		sl->i.version = 1;
		sl->i.state = SLS_OUT_OF_SERVICE;
	}

	write_lock_irqsave(&mtp_mux_lock, flags);

	if (!(sp = mtp->sp.sp)) {
		write_unlock_irqrestore(&mtp_mux_lock, flags);
		err = ENXIO;
		goto error;
	}
	if (!sp_trylock(q, sp)) {
		err = -EDEADLK;
		write_unlock_irqrestore(&mtp_mux_lock, flags);
		goto error;
	}
	/* Note that there can only be one layer management Stream per SP.  For
	   temporary links, that must be the same layer management Stream used to
	   create the SP. */
	if (sp->lm.lm != mtp) {
		err = EPERM;
		sp_unlock(sp);
		write_unlock_irqrestore(&mtp_mux_lock, flags);
		goto error;
	}

	if ((sl->sp.next = sp->sl.list))
		sl->sp.next->sp.prev = &sl->sp.next;
	sl->sp.prev = &sp->sl.list;
	sl->sp.sp = sp;
	sp->sl.list = sl;

	mi_attach(l->l_qtop, (caddr_t) sl);
	sp_unlock(sp);
	write_unlock_irqrestore(&mtp_mux_lock, flags);

	mi_copy_done(q, mp, 0);

	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->lmi_primitive = LMI_INFO_REQ;
	rp->b_wptr = rp->b_rptr + sizeof(*p);
	putnext(sl->wq, rp);	/* immediate info request */

	return (0);
      error:
	if (rp)
		freeb(rp);
	if (sl) {
		mtp_unlink_free(sl);
	}
	if (err >= 0) {
		mi_copy_done(q, mp, err);
		return (0);
	}
	return (err);
}
static int
mtp_i_plink(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct sl *sl;
	int err = EPERM;

	if (ioc->ioc_cr->cr_uid == 0) {
		err = ENOMEM;
		if ((sl = mtp_alloc_link(l->l_qbot, l->l_index, iocp->ioc_cr, 0)))
			err = 0;
	}
	mi_copy_done(q, mp, err);
	return (0);
}
static int
mtp_i_unlink(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct sl *sl;

	sl = SL_PRIV(l->l_qtop);
	mtp_free_link(sl);
	mi_copy_done(q, mp, 0);
	return (0);
}
static int
mtp_i_punlink(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct sl *sl;
	int err = EPERM;

	if (ioc->ioc_cr->cr_uid == 0) {
		sl = SL_PRIV(l->l_qtop);
		mtp_free_link(sl);
		err = 0;
	}
	mi_copy_done(q, mp, err);
	return (0);
}
static int
mtp_i_ioctl(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_LINK):
		return mtp_i_link(q, mtp, mp);
	case _IOC_NR(I_PLINK):
		return mtp_i_plink(q, mtp, mp);
	case _IOC_NR(I_UNLINK):
		return mtp_i_unlink(q, mtp, mp);
	case _IOC_NR(I_PUNLINK):
		return mtp_i_punlink(q, mtp, mp);
	}
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/**
 * mtp_ioctl: - process MTP M_IOCTL message
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the input-output control operation.  Step 1 consists of
 * copying in the necessary prefix structure that identifies the object type
 * and id being managed.
 */
static int
mtp_ioctl(queue_t *q, struct mtp *mtp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(MTP_IOCGOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCGOPTION)");
		size = sizeof(struct mtp_option);
		break;
	case _IOC_NR(MTP_IOCSOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCSOPTION)");
		size = sizeof(struct mtp_option);
		break;
	case _IOC_NR(MTP_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCSCONFIG)");
		size = sizeof(struct mtp_config);
		break;
	case _IOC_NR(MTP_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCGCONFIG)");
		size = sizeof(struct mtp_config);
		break;
	case _IOC_NR(MTP_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCTCONFIG)");
		size = sizeof(struct mtp_config);
		break;
	case _IOC_NR(MTP_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCCCONFIG)");
		size = sizeof(struct mtp_config);
		break;
	case _IOC_NR(MTP_IOCLCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCLCONFIG)");
		size = sizeof(struct mtp_config);
		break;
	case _IOC_NR(MTP_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCGSTATEM)");
		size = sizeof(struct mtp_statem);
		break;
	case _IOC_NR(MTP_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCCMRESET)");
		size = sizeof(struct mtp_statem);
		break;
	case _IOC_NR(MTP_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCGSTATSP)");
		size = sizeof(struct mtp_stats);
		break;
	case _IOC_NR(MTP_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCSSTATSP)");
		size = sizeof(struct mtp_stats);
		break;
	case _IOC_NR(MTP_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCGSTATS)");
		size = sizeof(struct mtp_stats);
		break;
	case _IOC_NR(MTP_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCCSTATS)");
		size = sizeof(struct mtp_stats);
		break;
	case _IOC_NR(MTP_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCGNOTIFY)");
		size = sizeof(struct mtp_notify);
		break;
	case _IOC_NR(MTP_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCSNOTIFY)");
		size = sizeof(struct mtp_notify);
		break;
	case _IOC_NR(MTP_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCCNOTIFY)");
		size = sizeof(struct mtp_notify);
		break;
	case _IOC_NR(MTP_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCCMGMT)");
		size = sizeof(struct mtp_mgmt);
		break;
	case _IOC_NR(MTP_IOCCPASS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(MTP_IOCCPASS)");
		size = sizeof(struct mtp_pass);
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

/* this structure is just to determine the maximum size of an ioctl */
union mtp_ioctls {
	struct {
		struct mtp_option option;
		union mtp_option_obj obj;
	} opt_conf;
	struct {
		struct mtp_config config;
		union mtp_config_obj obj;
	} conf;
	struct {
		struct mtp_statem statem;
		union mtp_statem_obj obj;
	} statem;
	struct {
		struct mtp_stats stats;
		union mtp_stats_obj obj;
	} stats;
	struct {
		struct mtp_notify notify;
		union mtp_notify_obj obj;
	} notify;
	struct mtp_mgmt mgmt;
	struct mtp_pass pass;
};

/**
 * mtp_copyin: - process MTP M_IOCDATA message
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step 2 of the MTP input-output control operation.  Step 2 consists
 * of copying out for GET operations, and processing an additional copy in
 * operation of object specific information for SET operations.
 */
static int
mtp_copyin(queue_t *q, struct mtp *mtp, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
		int len;

	case _IOC_NR(MTP_IOCGOPTION):
	case _IOC_NR(MTP_IOCGCONFIG):
	case _IOC_NR(MTP_IOCGSTATEM):
	case _IOC_NR(MTP_IOCGSTATSP):
	case _IOC_NR(MTP_IOCGSTATS):
	case _IOC_NR(MTP_IOCCSTATS):
	case _IOC_NR(MTP_IOCGNOTIFY):
		len = sizeof(union mtp_ioctls);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	case _IOC_NR(MTP_IOCLCONFIG):
	{
		struct mtp_config *p = (typeof(p)) dp->b_rptr;

		len = (p->cmd + 1) * sizeof(*p);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	}
	}
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(MTP_IOCGOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGOPTION)");
		err = mtp_get_options(bp);
		break;
	case _IOC_NR(MTP_IOCSOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCSOPTION)");
		err = mtp_size_opt_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(MTP_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGCONFIG)");
		err = mtp_get_conf(bp);
		break;
	case _IOC_NR(MTP_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCSCONFIG)");
		err = mtp_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(MTP_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCTCONFIG)");
		err = mtp_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(MTP_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCCONFIG)");
		err = mtp_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(MTP_IOCLCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCLCONFIG)");
		err = mtp_lst_conf(bp);
		break;
	case _IOC_NR(MTP_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGSTATEM)");
		err = mtp_get_statem(bp);
		break;
	case _IOC_NR(MTP_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCMRESET)");
		err = EOPNOTSUPP; /* later */ ;
		break;
	case _IOC_NR(MTP_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGSTATSP)");
		err = mtp_do_statsp(bp, MTP_GET);
		break;
	case _IOC_NR(MTP_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCSSTATSP)");
		err = mtp_size_stats(dp->b_rptr, &size);
		break;
	case _IOC_NR(MTP_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGSTATS)");
		err = mtp_get_stats(bp, false);
		break;
	case _IOC_NR(MTP_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCSTATS)");
		err = mtp_get_stats(bp, true);
		break;
	case _IOC_NR(MTP_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCSTATS)");
		err = mtp_do_notify(bp, MTP_GET);
		break;
	case _IOC_NR(MTP_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCSNOTIFY)");
		err = mtp_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(MTP_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCNOTIFY)");
		err = mtp_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(MTP_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCMGMT)");
		err = mtp_action(q, dp);
		break;
	case _IOC_NR(MTP_IOCPASS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCPASS)");
		break;
	default:
		err = EOPNOTSUPP;
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
			mi_copyin_n(q, mp, 0, size);
	}
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}

/**
 * mtp_copyin2: - process MTP M_IOCDATA message
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * Step 3 of the input-output control operation is an optional step that is
 * used for SET operations.  After the second copyin we now have the object
 * specific structure that was passed by the user and can complete the SET
 * operation.  All SET operations also include a last copyout step that copies
 * out the information actually set (e.g. the assigned id on MTP_ADD
 * operations).
 */
static int
mtp_copyin2(queue_t *q, struct mtp *mtp, mblk_t *mp, mblk_t *dp)
{
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(((struct copyresp *) mp->b_rptr)->cp_cmd)) {
	case _IOC_NR(MTP_IOCGOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGOPTION)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCSOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCSOPTION)");
		err = mtp_set_options(bp);
		break;
	case _IOC_NR(MTP_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGCONFIG)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCSCONFIG)");
		err = mtp_set_conf(bp);
		break;
	case _IOC_NR(MTP_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCTCONFIG)");
		err = mtp_test_conf(bp);
		break;
	case _IOC_NR(MTP_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCCONFIG)");
		err = mtp_commit_conf(bp);
		break;
	case _IOC_NR(MTP_IOCLCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCLCONFIG)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGSTATEM)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCMRESET)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGSTATSP)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCSSTATSP)");
		err = mtp_do_statsp(bp, MTP_CHA);
		break;
	case _IOC_NR(MTP_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGSTATS)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCSTATS)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCGNOTIFY)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCSNOTIFY)");
		err = mtp_do_notify(bp, MTP_ADD);
		break;
	case _IOC_NR(MTP_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCNOTIFY)");
		err = mtp_do_notify(bp, MTP_DEL);
		break;
	case _IOC_NR(MTP_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCMGMT)");
		err = EPROTO;
		break;
	case _IOC_NR(MTP_IOCCPASS):
	{
		struct mtp_pass *p = (typeof(p)) dp->b_rptr;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(MTP_IOCCPASS)");
		break;
	}
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}

/**
 * mtp_copyout: - process MTP M_IOCDATA message
 * @mtp: MTP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: data part
 * 
 * Step 4 and the final step of the input-output control operation is a final copyout step.
 */
static int
mtp_copyout(queue_t *q, struct mtp *mtp, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}

/**
 * mtp_w_ioctl: - process M_IOCTL message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 */
static int
mtp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct mtp *mtp;
	int err = 0;

	if (!mp->b - cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if (!(mtp = mtp_acquire(q)))
		return (-EAGAIN);

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		err = mtp_i_ioctl(q, mtp, mp);
		break;
	case MTP_IOC_MAGIC:
		err = mtp_ioctl(q, mtp, mp);
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	mtp_release(mtp);
	return (err);

}

/**
 * mtp_w_iocdata: - process M_IOCDATA message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 */
static int
mtp_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct mtp *mtp;
	int err = 0;
	mblk_t *dp;

	if (!(mtp = mtp_acquire(q)))
		return (-EAGAIN);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case MTP_IOC_MAGIC:
			err = mtp_copyin(q, mtp, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case MTP_IOC_MAGIC:
			err = mtp_copyin2(q, mtp, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case MTP_IOC_MAGIC:
			err = mtp_copyout(q, mtp, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		break;
	}
	mtp_release(mtp);
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
static noinline fastcall int
mtpi_w_proto(queue_t *q, mblk_t *mp)
{
	switch (*(mtp_ulong *) mp->b_rptr) {
	case MTP_BIND_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MTP_BIND_REQ");
		return m_bind_req(q, mp);
	case MTP_UNBIND_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MTP_UNBIND_REQ");
		return m_unbind_req(q, mp);
	case MTP_CONN_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MTP_CONN_REQ");
		return m_conn_req(q, mp);
	case MTP_DISCON_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MTP_DISCON_REQ");
		return m_discon_req(q, mp);
	case MTP_ADDR_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MTP_ADDR_REQ");
		return m_addr_req(q, mp);
	case MTP_INFO_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MTP_INFO_REQ");
		return m_info_req(q, mp);
	case MTP_OPTMGMT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MTP_OPTMGMT_REQ");
		return m_optmgmt_req(q, mp);
	case MTP_TRANSFER_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MTP_TRANSFER_REQ");
		return m_transfer_req(q, mp);
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> MTP_????_???");
		return m_error_ack(q, MTP_PRIV(q), mp, *(mtp_ulong *) mp->b_rptr,
				   -EOPNOTSUPP);
	}
}

static noinline fastcall int
tpi_w_proto(queue_t *q, mblk_t *mp)
{
	switch (*(t_scalar_t *) mp->b_rptr) {
	case T_CONN_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_CONN_REQ");
		return t_conn_req(q, mp);
	case T_CONN_RES:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_CONN_RES");
		return t_error_ack(q, MTP_PRIV(q), mp, *(t_scalar_t *) mp->b_rptr,
				   TNOTSUPPORT);
	case T_DISCON_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_DISCON_REQ");
		return t_discon_req(q, mp);
	case T_DATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_DATA_REQ");
		return t_data_req(q, mp);
	case T_EXDATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_EXDATA_REQ");
		return t_exdata_req(q, mp);
	case T_INFO_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_INFO_REQ");
		return t_info_req(q, mp);
	case T_BIND_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_BIND_REQ");
		return t_bind_req(q, mp);
	case T_UNBIND_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_UNBIND_REQ");
		return t_unbind_req(q, mp);
	case T_UNITDATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_UNITDATA_REQ");
		return t_unitdata_req(q, mp);
	case T_OPTMGMT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_OPTMGMT_REQ");
		return t_optmgmt_req(q, mp);
	case T_ORDREL_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_ORDREL_REQ");
		return t_ordrel_req(q, mp);
	case T_OPTDATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_OPTDATA_REQ");
		return t_optdata_req(q, mp);
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_ADDR_REQ");
		return t_addr_req(q, mp);
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_CAPABILITY_REQ");
		return t_capability_req(q, mp);
#endif
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> T_????_???");
		return t_error_ack(q, MTP_PRIV(q), mp, *(t_scalar_t *) mp->b_rptr,
				   TNOTSUPPORT);
	}
}

static noinline fastcall int
npi_w_proto(queue_t *q, mblk_t *mp)
{
	switch (*(np_ulong *) mp->b_rptr) {
	case N_CONN_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_CONN_REQ");
		return n_conn_req(q, mp);
	case N_CONN_RES:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_CONN_RES");
		return n_conn_res(q, mp);
	case N_DISCON_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_DISCON_REQ");
		return n_discon_req(q, mp);
	case N_DATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_DATA_REQ");
		return n_data_req(q, mp);
	case N_EXDATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_EXDATA_REQ");
		return n_exdata_req(q, mp);
	case N_INFO_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_INFO_REQ");
		return n_info_req(q, mp);
	case N_BIND_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_BIND_REQ");
		return n_bind_req(q, mp);
	case N_UNBIND_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_UNBIND_REQ");
		return n_unbind_req(q, mp);
	case N_UNITDATA_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_UNITDATA_REQ");
		return n_unitdata_req(q, mp);
	case N_OPTMGMT_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_OPTMGMT_REQ");
		return n_optmgmt_req(q, mp);
	case N_DATACK_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_DATACK_REQ");
		return n_datack_req(q, mp);
	case N_RESET_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_RESET_REQ");
		return n_reset_req(q, mp);
	case N_RESET_RES:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_RESET_RES");
		return n_reset_res(q, mp);
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> N_????_???");
		return n_error_ack(q, MTP_PRIV(q), mp, *(np_ulong *) mp->b_rptr,
				   NNOTSUPPORT);
	}
}

static inline fastcall int
mtp_w_proto(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	int err = -EAGAIN;

	if (likely((mtp = mtp_acquire(q)) != NULL)) {
		uint oldstate = mtp_get_state(mtp);

		switch (mtp->i.type) {
		default:
		case MTP_STYLE_MTPI:
		case MTP_STYLE_MGMT:
			err = mtpi_w_proto(q, mp);
			break;
		case MTP_STYLE_NPI:
			err = npi_w_proto(q, mp);
			break;
		case MTP_STYLE_TPI:
			err = tpi_w_proto(q, mp);
			break;
		}
		if (err)
			mtp_set_state(mtp, oldstate);
		mtp_release(mtp);
	}
	return (err < 0 ? err : 0);
}

static int
sli_r_proto(queue_t *q, mblk_t *mp)
{
	switch (*(lmi_ulong *) mp->b_rptr) {
	case SL_PDU_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "SL_PDU_IND [%u] <-",
			  (uint) msgdsize(mp->b_cont));
		return sl_pdu_ind(q, mp);
	case SL_LINK_CONGESTED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_LINK_CONGESTED_IND <-");
		return sl_link_congested_ind(q, mp);
	case SL_LINK_CONGESTION_CEASED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_LINK_CONGESTION_CEASED_IND <-");
		return sl_link_congestion_ceased_ind(q, mp);
	case SL_RETRIEVED_MESSAGE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_RETRIEVED_MESSAGE_IND <-");
		return sl_retrieved_message_ind(q, mp);
	case SL_RETRIEVAL_COMPLETE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_RETRIEVAL_COMPLETE_IND <-");
		return sl_retrieval_complete_ind(q, mp);
	case SL_RB_CLEARED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_RB_CLEARED_IND <-");
		return sl_rb_cleared_ind(q, mp);
	case SL_BSNT_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_BSNT_IND <-");
		return sl_bsnt_ind(q, mp);
	case SL_IN_SERVICE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_IN_SERVICE_IND <-");
		return sl_in_service_ind(q, mp);
	case SL_OUT_OF_SERVICE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_OUT_OF_SERVICE_IND <-");
		return sl_out_of_service_ind(q, mp);
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE,
			  "SL_REMOTE_PROCESSOR_OUTAGE_IND <-");
		return sl_remote_processor_outage_ind(q, mp);
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE,
			  "SL_REMOTE_PROCESSOR_RECOVERED_IND <-");
		return sl_remote_processor_recovered_ind(q, mp);
	case SL_RTB_CLEARED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_RTB_CLEARED_IND <-");
		return sl_rtb_cleared_ind(q, mp);
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_RETRIEVAL_NOT_POSSIBLE_IND <-");
		return sl_retrieval_not_possible_ind(q, mp);
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_BSNT_NOT_RETRIEVABLE_IND <-");
		return sl_bsnt_not_retrievable_ind(q, mp);
#if 0
	case SL_OPTMGMT_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_OPTMGMT_ACK <-");
		return sl_optmgmt_ack(q, mp);
	case SL_NOTIFY_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "SL_NOTIFY_IND <-");
		return sl_notify_ind(q, mp);
#endif
	case LMI_INFO_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_INFO_ACK <-");
		return lmi_info_ack(q, mp);
	case LMI_OK_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_OK_ACK <-");
		return lmi_ok_ack(q, mp);
	case LMI_ERROR_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_ERROR_ACK <-");
		return lmi_error_ack(q, mp);
	case LMI_ENABLE_CON:
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_ENABLE_CON <-");
		return lmi_enable_con(q, mp);
	case LMI_DISABLE_CON:
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_DISABLE_CON <-");
		return lmi_disable_con(q, mp);
	case LMI_OPTMGMT_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_OPTMGMT_ACK <-");
		return lmi_optmgmt_ack(q, mp);
	case LMI_ERROR_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_ERROR_IND <-");
		return lmi_error_ind(q, mp);
	case LMI_STATS_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_STATS_IND <-");
		return lmi_stats_ind(q, mp);
	case LMI_EVENT_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_EVENT_IND <-");
		return lmi_event_ind(q, mp);
	default:
		/* reject what we don't recognize */
		mi_strlog(q, STRLOGRX, SL_TRACE, "LMI_????_??? <-");
		return EOPNOTSUPP;
	}
}
static int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	int err = -EAGAIN;

	if ((sl = sl_acquire(q))) {
		uint oldstate = sl_get_l_state(sl);

		if ((err = sli_r_proto(q, mp)))
			sl_set_state(q, sl, oldstate);

		sl_release(sl);
	}
	return (err < 0 ? err : 0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
static inline fastcall int
mtp_w_data(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	int err = -EAGAIN;

	if (likely((mtp = mtp_acquire(q)) != NULL)) {
		switch (mtp->i.type) {
		default:
		case MTP_STYLE_MTPI:
		case MTP_STYLE_MGMT:
			err = m_data(q, mtp, mp);
			break;
		case MTP_STYLE_NPI:
			err = n_data(q, mtp, mp);
			break;
		case MTP_STYLE_TPI:
			err = t_data(q, mtp, mp);
			break;
		}
		mtp_release(mtp);
	}
	return (err < 0 ? err : 0);
}

static int
sl_r_data(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	int err = -EAGAIN;

	if (likely((sl = sl_acquire(q)))) {
		err = sl_data(q, mp);
		sl_release(sl);
	}
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_SIG, M_PCSIG Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
do_timeout(queue_t *q, mblk_t *mp)
{
	struct mtp_timer *t = (typeof(t)) mp->b_rptr;
	int rtn;

	switch (t->timer) {
	case t1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t1 expiry at %lu", jiffies);
		return cb_t1_timeout(q, t->cb);
	case t1r:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t1r expiry at %lu", jiffies);
		return sp_t1r_timeout(q, t->sp);
	case t2:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t2 expiry at %lu", jiffies);
		return cb_t2_timeout(q, t->cb);
	case t3:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t3 expiry at %lu", jiffies);
		return cb_t3_timeout(q, t->cb);
	case t4:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t4 expiry at %lu", jiffies);
		return cb_t4_timeout(q, t->cb);
	case t5:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t5 expiry at %lu", jiffies);
		return cb_t5_timeout(q, t->cb);
	case t6:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t6 expiry at %lu", jiffies);
		return cr_t6_timeout(q, t->cr);
	case t7:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t7 expiry at %lu", jiffies);
		return lk_t7_timeout(q, t->lk);
	case t8:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t8 expiry at %lu", jiffies);
		return rs_t8_timeout(q, t->rs);
	case t10:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t10 expiry at %lu", jiffies);
		return rt_t10_timeout(q, t->rt);
	case t11:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t11 expiry at %lu", jiffies);
		return rs_t11_timeout(q, t->rs);
	case t12:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t12 expiry at %lu", jiffies);
		t->count++;
		if ((err = sl_t12_timeout(q, t->sl, t->count)))
			t->count--;
		return (err);
	case t13:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t13 expiry at %lu", jiffies);
		t->count++;
		if ((err = sl_t13_timeout(q, t->sl, t->count)))
			t->count--;
		return (err);
	case t14:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t14 expiry at %lu", jiffies);
		t->count++;
		if ((err = sl_t14_timeout(q, t->sl, t->count)))
			t->count--;
		return (err);
	case t15:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t15 expiry at %lu", jiffies);
		return rs_t15_timeout(q, t->rs);
	case t16:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t16 expiry at %lu", jiffies);
		return rs_t16_timeout(q, t->rs);
	case t17:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t17 expiry at %lu", jiffies);
		return sl_t17_timeout(q, t->sl);
	case t18:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t18 expiry at %lu", jiffies);
		return sp_t18_timeout(q, t->sp);
	case t18a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t18a expiry at %lu", jiffies);
		return rs_t18a_timeout(q, t->rs);
	case t19:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t19 expiry at %lu", jiffies);
		return lk_t19_timeout(q, t->lk);
	case t19a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t19a expiry at %lu", jiffies);
		return sl_t19a_timeout(q, t->sl);
	case t20:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t20 expiry at %lu", jiffies);
		return sp_t20_timeout(q, t->sp);
	case t20a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t20a expiry at %lu", jiffies);
		return sl_t20a_timeout(q, t->sl);
	case t21:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t21 expiry at %lu", jiffies);
		return lk_t21_timeout(q, t->lk);
	case t21a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t21a expiry at %lu", jiffies);
		return sl_t21a_timeout(q, t->sl);
	case t22:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t22 expiry at %lu", jiffies);
		return sl_t22_timeout(q, t->sl);
	case t22a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t22a expiry at %lu", jiffies);
		return sp_t22a_timeout(q, t->sp);
	case t23:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t23 expiry at %lu", jiffies);
		return sl_t23_timeout(q, t->sl);
	case t23a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t23a expiry at %lu", jiffies);
		return sp_t23a_timeout(q, t->sp);
	case t24:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t24 expiry at %lu", jiffies);
		return sl_t24_timeout(q, t->sl);
	case t24a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t24a expiry at %lu", jiffies);
		return sp_t24a_timeout(q, t->sp);
	case t25a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t25a expiry at %lu", jiffies);
		return lk_t25a_timeout(q, t->lk);
	case t26a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t26a expiry at %lu", jiffies);
		return sp_t26a_timeout(q, t->sp);
	case t27a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t27a expiry at %lu", jiffies);
		return sp_t27a_timeout(q, t->sp);
	case t28a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t28a expiry at %lu", jiffies);
		return lk_t28a_timeout(q, t->lk);
	case t29a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t29a expiry at %lu", jiffies);
		return lk_t29a_timeout(q, t->lk);
	case t30a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t30a expiry at %lu", jiffies);
		return lk_t30a_timeout(q, t->lk);
	case t31a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t31a expiry at %lu", jiffies);
		return sl_t31a_timeout(q, t->sl);
	case t32a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t32a expiry at %lu", jiffies);
		return sl_t32a_timeout(q, t->sl);
	case t33a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t33a expiry at %lu", jiffies);
		return sl_t33a_timeout(q, t->sl);
	case t34a:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t34a expiry at %lu", jiffies);
		return sl_t34a_timeout(q, t->sl);
	case t1t:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t1t expiry at %lu", jiffies);
		return sl_t1t_timeout(q, t->sl);
	case t2t:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t2t expiry at %lu", jiffies);
		return sl_t2t_timeout(q, t->sl);
	case t1s:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t1s expiry at %lu", jiffies);
		return sl_t1s_timeout(q, t->sl);
	default:
		return (0);
	}
	return (rtn);
}
static int
mtp_w_sig(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	uint oldstate;
	int err;

	if (!(mtp = mtp_acquire(q)))
		return (mi_timer_requeue(mp) ? -EAGAIN : 0);

	oldstate = mtp_get_state(mtp);

	if (likely(mi_timer_valid(mp))) {
		err = do_timeout(q, mp);

		if (err) {
			mtp_set_state(mtp, oldstate);
			err = mi_timer_requeue(mp) ? err : 0;
		}
	}
	mtp_release(mtp);
	return (err < 0 ? err : 0);
}

static int
sl_r_sig(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	uint oldstate;
	int err = 0;

	if (!(sl = sl_acquire(q)))
		return (mi_timer_requeue(mp) ? -EAGAIN : 0);

	oldstate = sl_get_l_state(sl);

	if (likely(mi_timer_valid(mp))) {
		err = do_timeout(q, mp);

		if (err) {
			sl_set_state(q, sl, oldstate);
			err = mi_timer_requeue(mp) ? err : 0;
		}
	}
	sl_release(sl);
	return (err < 0 ? err : 0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_RSE, M_PCRSE Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
mtp_w_rse(queue_t *q, mblk_t *mp)
{
	return (EPROTO);
}

static int
sl_r_rse(queue_t *q, mblk_t *mp)
{
	return (EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Unknown STREAMS Message Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
mtp_w_unknown(queue_t *q, mblk_t *mp)
{
	return (EOPNOTSUPP);
}

static int
sl_r_unknown(queue_t *q, mblk_t *mp)
{
	return (EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
mtp_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static int
sl_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a signalling link has failed badly.
 *  Move link to the out-of-service state, notify management, and perform
 *  normal link failure actions.
 */
static int
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
 *  A hangup from below indicates that a signalling link has failed badly.
 *  Move link to the out-of-service state, notify management, and perform
 *  normal link failure actions.
 */
static int
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

static noinline fastcall int
mtp_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return mtp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mtp_w_sig(q, mp);
	case M_FLUSH:
		return mtp_w_flush(q, mp);
	case M_IOCTL:
		return mtp_w_ioctl(q, mp);
	case M_IOCDATA:
		return mtp_w_iocdata(q, mp);
	case M_RSE:
	case M_PCRSE:
		return mtp_w_rse(q, mp);
	default:
		return mtp_w_unknown(q, mp);
	}
}

static noinline fastcall int
sl_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return sl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return sl_r_sig(q, mp);
	case M_FLUSH:
		return sl_r_flush(q, mp);
	case M_ERROR:
		return sl_r_error(q, mp);
	case M_HANGUP:
		return sl_r_hangup(q, mp);
#if 0
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return sl_r_copy(q, mp);
#endif
	case M_RSE:
	case M_PCRSE:
		return sl_r_rse(q, mp);
	default:
		return sl_r_unknown(q, mp);
	}
}

static inline fastcall int
mtp_w_msg(queue_t *q, mblk_t *mp)
{
	/* TODO: write a fast path */
	return mtp_w_msg_slow(q, mp);
}
static inline fastcall int
sl_r_msg(queue_t *q, mblk_t *mp)
{
	/* TODO: write a fast path */
	return sl_r_msg_slow(q, mp);
}

/*
 *  QUEUE PUT and SERVICE procedures
 *  =========================================================================
 *  Canonical STREAMS multiplexing driver processing.
 */
static streamscall __hot_put int
mtp_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || mtp_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
mtp_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mtp_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_get int
mtp_rsrv(queue_t *q)
{
	/* FIXME: backenable across the mux */
	return (0);
}
static streamscall __unlikely int
mtp_rput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "mtp_rput() called");
	putnext(q, mp);
	return (0);
}
static streamscall __unlikely int
sl_wput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "sl_wput() called");
	putnext(q, mp);
	return (0);
}
static streamscall __hot_out int
sl_wsrv(queue_t *q)
{
	/* FIXME: backenable across the mux */
	return (0);
}
static streamscall __hot_get int
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
static streamscall __hot_in int
sl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || sl_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
static int mtp_majors[MTP_CMAJORS] = { MTP_CMAJOR_0, };

/**
 * mtp_qopen: - STREAMS driver open routine
 * @q: read queue of newly created queue pair
 * @devp: pointer to device number associated with Stream
 * @oflags: flags to the open(2s) call
 * @sflag: STREAMS flag
 * @crp: pointer to the credentials of the opening process
 *
 * When a Stream is opened on the driver it corresponds to an SP associated
 * with the driver.  The SP is determined from the minor device opened.  All
 * minor devices corresponding to SP are clone or auto-clone devices.  There
 * may be several SL lower Streams for each SP.  If an SP structure has not
 * been allocated for the corresponding minor device number, we allocate one.
 * When an SL Stream is I_LINK'ed under the driver, it is associated with the
 * SP structure.  An MTP structure is allocated and associated with each upper
 * Stream.
 *
 * This driver cannot be pushed as a module.
 *
 * (cminor == 0) && (sflag == DRVOPEN)
 *	When minor device number 0 is opened with DRVOPEN (non-clone), a
 *	control Stream is opened.  If a control Stream has already been
 *	opened, the open is refused.  The @sflag is changed from %DRVOPEN to
 *	%CLONEOPEN and a new minor device number above NUM_SP is assigned.
 *	This uses the autocloning features of Linux Fast-STREAMS.  This
 *	corresponds to the /dev/streams/mtp/lm minor device node.
 *
 * (cminor == 0) && (sflag == CLONEOPEN)
 *	This is a normal clone open using the clone(4) driver.  A
 *	disassociated user Stream is opened.  A new unique minor device number
 *	above NUM_SP is assigned.  This corresponds to the
 *	/dev/streams/clone/mtp clone device node.
 *
 * (1 <= cminor && cminor <= NUM_SP)
 *	This is a normal non-clone open.  Where the minor device number is
 *	between 1 and NUM_SP, an associated user Stream is opened.  If there
 *	is no SP structure to associate, one is created with default values
 *	(and associated with the default NA).  A new minor device number above
 *	NUM_SP is assigned.  This uses the autocloning features of Linux
 *	Fast-STREAMS.  This corresponds to the /dev/streams/mtp/NNNN minor
 *	device node where NNNN is the minor device number.
 */
static streamscall int
mtp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	minor_t bminor = cminor;
	struct mtp *mtp;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		mi_strlog(q, 0, SL_ERROR, "cannot push as module");
		return (ENXIO);
	}
	if (cminor > NUM_SP) {
		mi_strlog(q, 0, SL_ERROR, "cannot open dynamic minor device number");
		return (ENXIO);
	}
	if (cmajor != MTP_CMAJOR_0 || cminor >= MTP_CMINOR_FREE) {
		mi_strlog(q, 0, SL_ERROR, "cannot open cloned minors");
		return (ENXIO);
	}
	if (!(mtp = mtp_open_alloc(q, devp, crp, cminor)))
		return (ENOMEM);

	*devp = makedevice(cmajor, NUM_SP + 1);
	/* start assigning minors at NUM_SP + 1 */

	write_lock_irqsave(&mtp_mux_lock, flags);

	if (cminor == 0) {
		/* When a zero minor device number was opened, the Stream is either
		   a clone open or an attempt to open the master control Stream. The 
		   difference is whether the @sflag was %DRVOPEN or %CLONEOPEN. */
		if (sflag == DRVOPEN) {
			/* When the master control Stream is opened, another master
			   control Stream must not yet exist. If this is the only
			   master control Stream then it is created. */
			if (lm_ctrl != NULL) {
				write_unlock_irqrestore(&mtp_mux_lock, flags);
				mtp_close_free(mtp);
				return (ENXIO);
			}
		}
		if ((err =
		     mi_open_link(&mtp_opens, (caddr_t) mtp, devp, oflags, CLONEOPEN,
				  crp))) {
			write_unlock_irqrestore(&mtp_mux_lock, flags);
			mtp_close_free(mtp);
			return (ENXIO);
		}
		if (sflag == DRVOPEN)
			lm_ctrl = mtp;
		/* Both master control Streams and clone user Streams are
		   disassociated with any specific SP. Master control Streams are
		   never associated with a specific SP. User Streams are associated
		   with an SP using the spid in the MTP addres to the bind
		   primitive, or when an SL Stream is temporarily linked under the
		   driver using the I_LINK input-output control. */
	} else {
		DECLARE_WAITQUEUE(wait, current);

		/* When a non-zero minor device number was opened, the Stream is
		   automatically associated with the SP to which the minor device
		   number corresponds.  It cannot be disassociated except when it is 
		   closed. */
		if (!(sp = sp_lookup(cminor))) {
			write_unlock_irqrestore(&mtp_mux_lock, flags);
			mtp_close_free(mtp);
			return (ENXIO);
		}
		/* Locking: need to wait until a lock on the SP structure can be
		   acquired, or a signal is received, or the SP structure is
		   deallocated.  If the lock can be acquired, associate the User
		   Stream with the SP structure; in all other cases, return an
		   error.  Note that it is a likely event that the lock can be
		   acquired without waiting. */
		err = 0;
		add_wait_queue(&mtp_waitq, &wait);
		spin_lock(&sp->sq.lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (signal_pending(current)) {
				err = EINTR;
				spin_unlock(&sp->sq.lock);
				break;
			}
			if (sp->sq.users != 0) {
				spin_unlock(&sp->sq.lock);
				write_unlock_irqrestore(&mtp_mux_lock, flags);
				if (!(sp = sp_lookup(cminor))) {
					err = ENXIO;
					break;
				}
				spin_lock(&sp->sq.lock);
				continue;
			}
			err =
			    mi_open_link(&mtp_opens, (caddr_t) mtp, devp, oflags,
					 CLONEOPEN, crp);
			if (err == 0)
				mtp_attach(mtp, sp);
			spin_unlock(&sp->sq.lock);
			break;
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&mtp_waitq, &wait);
		if (err) {
			write_unlock_irqrestore(&mtp_mux_lock, flags);
			mtp_close_free(mtp);
			return (err);
		}
	}
	write_unlock_irqrestore(&mtp_mux_lock, flags);

	mi_attach(q, (caddr_t) mtp);
	qprocson(q);
	return (0);
}

/**
 * mtp_qclose: - STREAMS driver close routine
 * @q: queue pair
 * @oflags: flags to the open(2s) call
 * @crp: pointer to the credentials of the closing process
 */
static streamscall int
mtp_qclose(queue_t *q, int flag, cred_t *crp)
{
	struct mtp *mtp = MTP_PRIV(q);
	psw_t flags;
	struct sp *sp;

	qprocsoff(q);
	mi_detach(q, (caddr_t) mtp);

	write_lock_irqsave(&mtp_mux_lock, flags);

	if ((sp = mtp->sp.sp)) {
		DECLARE_WAITQUEUE(wait, current);

		/* Locking: need to wait until an SP lock can be acquired, or the SP 
		   structure is deallocated.  If a lock can be acquired, the closing 
		   Stream is disassociated with the SP; otherwise, if the SP
		   structure is deallocated, there is no further need to
		   disassociate. Note that it is a likely event that the lock can be 
		   acquired without waiting. */
		add_wait_queue(&mtp_waitq, &wait);
		spin_lock(&sp->sq.lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (sp->sq.users == 0) {
				mtp_detach(mtp);
				spin_unlock(&sp->sq.lock);
				break;
			}
			spin_unlock(&sp->sq.lock);
			write_unlock_irqrestore(&mtp_mux_lock, flags);
			schedule();
			write_lock_irqsave(&mtp_mux_lock, flags);
			if (!(sp = mtp->sp.sp))
				break;
			spin_lock(&sp->sq.lock);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&mtp_waitq, &wait);
	}
	mi_close_unlink(&mtp_opens, (caddr_t) mtp);

	write_unlock_irqrestore(&mtp_opens, (caddr_t) mtp);

	mtp_close_free(mtp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
static kmem_cachep_t mtp_mt_cachep = NULL;	/* MTP User cache */
static kmem_cachep_t mtp_na_cachep = NULL;	/* Network Appearance cache */
static kmem_cachep_t mtp_sp_cachep = NULL;	/* Signalling Point cache */
static kmem_cachep_t mtp_rs_cachep = NULL;	/* Route Set cache */
static kmem_cachep_t mtp_cr_cachep = NULL;	/* Controlled Rerouting Buffer cache 
						 */
static kmem_cachep_t mtp_rl_cachep = NULL;	/* Route List cache */
static kmem_cachep_t mtp_rt_cachep = NULL;	/* Route cache */
static kmem_cachep_t mtp_cb_cachep = NULL;	/* Changeback Buffer cache */
static kmem_cachep_t mtp_ls_cachep = NULL;	/* (Combined) Link Set cache */
static kmem_cachep_t mtp_lk_cachep = NULL;	/* Link (Set) cache */
static kmem_cachep_t mtp_sl_cachep = NULL;	/* Signalling Link cache */
static int
mtp_init_caches(void)
{
	size_t sizeof_struct_mt = mi_open_size(sizeof(struct mtp));
	size_t sizeof_struct_sl = mi_open_size(sizeof(struct sl));

	if (!mtp_mt_cachep
	    && !(mtp_mt_cachep =
		 kmem_cache_create("mtp_mt_cachep", sizeof_struct_mt, 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_mt_cachep", DRV_NAME);
		goto failed_mt;
	} else
		printd(("%s: initialized mt structure cache\n", DRV_NAME));
	if (!mtp_na_cachep
	    && !(mtp_na_cachep =
		 kmem_cache_create("mtp_na_cachep", sizeof(struct na), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_na_cachep", DRV_NAME);
		goto failed_na;
	} else
		printd(("%s: initialized sp structure cache\n", DRV_NAME));
	if (!mtp_sp_cachep
	    && !(mtp_sp_cachep =
		 kmem_cache_create("mtp_sp_cachep", sizeof(struct sp), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_sp_cachep", DRV_NAME);
		goto failed_sp;
	} else
		printd(("%s: initialized sp structure cache\n", DRV_NAME));
	if (!mtp_rs_cachep
	    && !(mtp_rs_cachep =
		 kmem_cache_create("mtp_rs_cachep", sizeof(struct rs), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_rs_cachep", DRV_NAME);
		goto failed_rs;
	} else
		printd(("%s: initialized rs structure cache\n", DRV_NAME));
	if (!mtp_cr_cachep
	    && !(mtp_cr_cachep =
		 kmem_cache_create("mtp_cr_cachep", sizeof(struct cr), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_cr_cachep", DRV_NAME);
		goto failed_cr;
	} else
		printd(("%s: initialized cr structure cache\n", DRV_NAME));
	if (!mtp_rl_cachep
	    && !(mtp_rl_cachep =
		 kmem_cache_create("mtp_rl_cachep", sizeof(struct rl), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_rl_cachep", DRV_NAME);
		goto failed_rl;
	} else
		printd(("%s: initialized rl structure cache\n", DRV_NAME));
	if (!mtp_rt_cachep
	    && !(mtp_rt_cachep =
		 kmem_cache_create("mtp_rt_cachep", sizeof(struct rt), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_rt_cachep", DRV_NAME);
		goto failed_rt;
	} else
		printd(("%s: initialized rt structure cache\n", DRV_NAME));
	if (!mtp_cb_cachep
	    && !(mtp_cb_cachep =
		 kmem_cache_create("mtp_cb_cachep", sizeof(struct cb), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_cb_cachep", DRV_NAME);
		goto failed_cb;
	} else
		printd(("%s: initialized cb structure cache\n", DRV_NAME));
	if (!mtp_ls_cachep
	    && !(mtp_ls_cachep =
		 kmem_cache_create("mtp_ls_cachep", sizeof(struct ls), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_ls_cachep", DRV_NAME);
		goto failed_ls;
	} else
		printd(("%s: initialized ls structure cache\n", DRV_NAME));
	if (!mtp_lk_cachep
	    && !(mtp_lk_cachep =
		 kmem_cache_create("mtp_lk_cachep", sizeof(struct lk), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mtp_lk_cachep", DRV_NAME);
		goto failed_lk;
	} else
		printd(("%s: initialized lk structure cache\n", DRV_NAME));
	if (!mtp_sl_cachep
	    && !(mtp_sl_cachep =
		 kmem_cache_create("mtp_sl_cachep", sizeof_struct_sl, 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL))) {
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
static int
mtp_term_caches(void)
{
	int err = 0;

	if (mtp_mt_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_mt_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_mt_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_mt_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_mt_cachep);
#endif
	}
	if (mtp_na_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_na_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_na_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_na_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_na_cachep);
#endif
	}
	if (mtp_sp_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_sp_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_sp_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_sp_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_sp_cachep);
#endif
	}
	if (mtp_rs_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_rs_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_rs_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_rs_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_rs_cachep);
#endif
	}
	if (mtp_cr_cachep) {
		if (kmem_cache_destroy(mtp_cr_cachep)) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
			cmn_err(CE_WARN, "%s: did not destroy mtp_cr_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_cr_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_cr_cachep);
#endif
	}
	if (mtp_rl_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_rl_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_rl_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_rl_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_rl_cachep);
#endif
	}
	if (mtp_rt_cachep) {
		if (kmem_cache_destroy(mtp_rt_cachep)) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
			cmn_err(CE_WARN, "%s: did not destroy mtp_rt_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_rt_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_rt_cachep);
#endif
	}
	if (mtp_cb_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_cb_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_cb_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_cb_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_cb_cachep);
#endif
	}
	if (mtp_ls_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_ls_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_ls_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_ls_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_ls_cachep);
#endif
	}
	if (mtp_lk_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_lk_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_lk_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_lk_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_lk_cachep);
#endif
	}
	if (mtp_sl_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mtp_sl_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mtp_sl_cachep",
				__FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mtp_sl_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mtp_sl_cachep);
#endif
	}
	return (err);
}

/*
 *  MT allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct mtp *
mtp_lookup(uint id)
{
	struct mtp *mtp = NULL;

	if (id)
		for (mtp = master.mtp.list; mtp && mtp->id != id; mtp = mtp->next) ;
	return (mtp);
}
static inline uint
mtp_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		for (id = ++sequence; mtp_lookup(id); id = ++sequence) ;
	return (id);
}
static struct mtp *
mtp_alloc_priv(queue_t *q, dev_t *devp, cred_t *crp, minor_t bminor)
{
	struct mtp *mt;

	if ((mt = (struct mtp *) mi_open_alloc_cache(mtp_mt_cachep, GFP_ATOMIC))) {
		printd(("%s: %p: allocated mt private structure\n", DRV_NAME, mt));
		bzero(mt, sizeof(*mt));
		mt->rq = RD(q);
		mt->dev = *devp;
		mt->cred = *crp;
		mt->i.state = 0;
		mt->i.type = bminor;
		mt->i.style = T_CLTS;
		mt->i.version = 1;
		/* not attached to sp yet */
		mt->sp.loc = NULL;
		mt->sp.rem = NULL;
		mt->sp.next = NULL;
		mt->sp.prev = &mt->sp.next;
		/* set defaults */
		printd(("%s: %p: linked mt private structure\n", DRV_NAME, mt));
	} else
		ptrace(("%s: ERROR: Could not allocate mt private structure\n",
			DRV_NAME));
	return (mt);
}
static void
mtp_free_priv(struct mtp *mtp)
{

	ensure(mtp, return);
	if (mtp->sp.loc) {
		if ((*mtp->sp.prev = mtp->sp.next))
			mtp->sp.next->sp.prev = mtp->sp.prev;
		mtp->sp.next = NULL;
		mtp->sp.prev = &mtp->sp.next;
		mtp->sp.loc = NULL;
	}
	if (mtp->sp.rem) {
		mtp->sp.rem = NULL;
	}
	if (mtp->next || mtp->prev != &mtp->next) {
		/* remove from master list */
		if ((*mtp->prev = mtp->next))
			mtp->next->prev = mtp->prev;
		mtp->next = NULL;
		mtp->prev = &mtp->next;
	}
	mi_close_free_cache(mtp_mt_cachep, (caddr_t) mtp);
}

/*
 *  SL allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct sl *
sl_lookup_mux(int index)
{
	struct sl *sl = NULL;

	for (sl = master.sl.list; sl && sl->u.mux.index != index; sl = sl->next) ;
	return (sl);
}
static struct sl *
sl_lookup(uint id)
{
	struct sl *sl = NULL;

	if (id)
		for (sl = master.sl.list; sl && sl->id != id; sl = sl->next) ;
	return (sl);
}

static uint
sl_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		for (id = ++sequence; sl_lookup(id); id = ++sequence) ;
	return (id);
}
static void
mtp_free_link(struct sl *sl)
{
	/* Note: both mi_timer_stop and mi_timer_free can handle a NULL pointer. */
	sl_timer_stop(sl, tall);
	mi_timer_free(sl->timers.t1);
	mi_timer_free(sl->timers.t2);
	mi_timer_free(sl->timers.t3);
	mi_timer_free(sl->timers.t4);
	mi_timer_free(sl->timers.t5);
	mi_timer_free(sl->timers.t12);
	mi_timer_free(sl->timers.t13);
	mi_timer_free(sl->timers.t14);
	mi_timer_free(sl->timers.t17);
	mi_timer_free(sl->timers.t19a);
	mi_timer_free(sl->timers.t20a);
	mi_timer_free(sl->timers.t21a);
	mi_timer_free(sl->timers.t22);
	mi_timer_free(sl->timers.t23);
	mi_timer_free(sl->timers.t24);
	mi_timer_free(sl->timers.t31a);
	mi_timer_free(sl->timers.t32a);
	mi_timer_free(sl->timers.t33a);
	mi_timer_free(sl->timers.t34a);
	mi_timer_free(sl->timers.t1t);
	mi_timer_free(sl->timers.t2t);
	mi_timer_free(sl->timers.t1s);
	mi_close_free_cache(mtp_sl_cachep, (caddr_t) sl);
}
static struct sl *
mtp_alloc_link(queue_t *q, int index, cred_t *crp, minor_t unit)
{
	struct sl *sl;
	struct mtp_timer *t;

	if ((sl = (struct sl *) mi_open_alloc_cache(mtp_sl_cachep, GFP_ATOMIC))) {
		printd(("%s: %p: allocated sl private structure %lu\n", DRV_NAME, sl,
			index));
		bzero(sl, sizeof(*sl));
		sl->wq = WR(q);
		sl->index = index;
		sl->cred = *crp;
		sl->i.state = LMI_UNATTACHED;
		sl->i.style = LMI_STYLE2;
		sl->i.version = 1;
		sl->l_state = SLS_OUT_OF_SERVICE;
		/* allocate timers */
		if (!(sl->timers.t1 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t1->b_rptr)->timer = t1;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t2 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t2->b_rptr)->timer = t2;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t3 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t3->b_rptr)->timer = t3;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t4 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t4->b_rptr)->timer = t4;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t5 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t5->b_rptr)->timer = t5;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t12 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t12->b_rptr)->timer = t12;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t13 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t13->b_rptr)->timer = t13;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t14 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t14->b_rptr)->timer = t14;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t17 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t17->b_rptr)->timer = t17;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t19a = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t19a->b_rptr)->timer = t19a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t20a = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t20a->b_rptr)->timer = t20a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t21a = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t21a->b_rptr)->timer = t21a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t22 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t22a->b_rptr)->timer = t22a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t23 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t23a->b_rptr)->timer = t23a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t24 = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t24a->b_rptr)->timer = t24a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t31a = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t31a->b_rptr)->timer = t31a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t32a = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t32a->b_rptr)->timer = t32a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t33a = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t33a->b_rptr)->timer = t33a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t34a = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t34a->b_rptr)->timer = t34a;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t1t = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t1t->b_rptr)->timer = t1t;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t2t = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t2t->b_rptr)->timer = t2t;
		t->count = 0;
		t->sl = sl;
		if (!(sl->timers.t1s = mi_timer_alloc(sizeof(*t))))
			goto enobufs;
		(t = (typeof(t)) sl->timers.t1s->b_rptr)->timer = t1s;
		t->count = 0;
		t->sl = sl;
		/* assign defaults */
	} else
		ptrace(("%s: ERROR: Could not allocate sl private structure %lu\n",
			DRV_NAME, index));
	return (sl);
      enobufs:
	mtp_free_link(sl);
	return (NULL);
}
static struct sl *
mtp_alloc_sl(uint id, struct sl *sl, struct lk *lk, struct mtp_conf_sl *c)
{
	if (!sl)
		return (sl);
	if (lk) {
		/* add to link (set) list */
		if ((sl->lk.next = lk->sl.list))
			sl->lk.next->lk.prev = &sl->lk.next;
		sl->lk.prev = &lk->sl.list;
		sl->lk.lk = lk;
		lk->sl.list = sl;
		lk->sl.numb++;
	}
	sl->id = sl_get_id(id);
	sl->slc = c->slc;
	if (lk) {
		/* defaults inherited from lk */
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
	return (sl);
}
static void
mtp_free_sl(struct sl *sl)
{
	struct lk *lk;

	ensure(sl, return);
	/* stop timers */
	sl_timer_stop(sl, tall);
	/* remove from lk list */
	if ((lk = sl->lk.lk)) {
		if ((*sl->lk.prev = sl->lk.next))
			sl->lk.next->lk.prev = sl->lk.prev;
		sl->lk.next = NULL;
		sl->lk.prev = &sl->lk.next;
		assure(lk->sl.numb > 0);
		lk->sl.numb--;
		sl->lk.lk = NULL;
	}
}

/*
 *  CB allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static inline struct cb *
cb_lookup(uint id)
{
	struct cb *cb = NULL;

	if (id)
		for (cb = master.cb.list; cb && cb->id != id; cb = cb->next) ;
	return (cb);
}

static uint
cb_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		id = ++sequence;
	return (id);
}
static struct cb *
mtp_alloc_cb(uint id, struct lk *lk, struct sl *from, struct sl *onto, uint index)
{
	struct cb *cb;

	if ((cb = kmem_cache_alloc(mtp_cb_cachep, GFP_ATOMIC))) {
		bzero(cb, sizeof(*cb));
		bufq_init(&cb->buf);
		/* add to master list */
		if ((cb->next = master.cb.list))
			cb->next->prev = &cb->next;
		cb->prev = &master.cb.list;
		master.cb.list = cb;
		master.cb.numb++;
		/* add to link changeover buffer list */
		if (lk) {
			if ((cb->lk.next = lk->cb.list))
				cb->lk.next->lk.prev = &cb->lk.next;
			cb->lk.prev = &lk->cb.list;
			lk->cb.list = cb;
			cb->lk.lk = lk;
		}
		cb->id = cb_get_id(id);
		cb->type = MTP_OBJ_TYPE_CB;
		cb->state = 0;
		cb->index = index;
		cb->slc = from->slc;
		cb->sl.from = from;
		cb->sl.onto = onto;
		printd(("%s: %p: linked cb structure %lu\n", DRV_NAME, cb, cb->id));
	} else
		ptrace(("%s: ERROR: Could not allocate cb structure %lu\n", DRV_NAME,
			id));
	return (cb);
}
static void
mtp_free_cb(struct cb *cb)
{
	struct lk *lk;
	struct sl *sl;

	/* stop timers and purge buffers */
	cb_timer_stop(cb, tall);
	bufq_purge(&cb->buf);
	/* unlink from signalling links */
	if ((sl = xchg(&cb->sl.onto, NULL))) {
		sl->load--;
	}
	if ((sl = xchg(&cb->sl.from, NULL))) {
		sl->load++;
	}
	if ((lk = cb->lk.lk)) {
		/* unlink from lk */
		if ((*cb->lk.prev = cb->lk.next))
			cb->lk.next->lk.prev = cb->lk.prev;
		cb->lk.next = NULL;
		cb->lk.prev = &cb->lk.next;
		assure(lk->cb.numb > 0);
		lk->cb.numb--;
		cb->lk.lk = NULL;
	}
	if (cb->next || cb->prev != &cb->next) {
		/* remove from master list */
		if ((*cb->prev = cb->next))
			cb->next->prev = cb->prev;
		cb->next = NULL;
		cb->prev = &cb->next;
		assure(master.cb.numb > 0);
		master.cb.numb--;
	}
	cb->id = 0;
	kmem_cache_free(mtp_cb_cachep, cb);
}

/*
 *  LK allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct lk *
lk_lookup(uint id)
{
	struct lk *lk = NULL;

	if (id)
		for (lk = master.lk.list; lk && lk->id != id; lk = lk->next) ;
	return (lk);
}

static uint
lk_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		id = ++sequence;
	return (id);
}
static struct lk *
mtp_alloc_lk(uint id, struct ls *ls, struct sp *loc, struct rs *adj,
	     struct mtp_conf_lk *c)
{
	struct mtp_timer *t;
	struct lk *lk;

	if ((lk = kmem_cache_alloc(mtp_lk_cachep, GFP_ATOMIC))) {
		bzero(lk, sizeof(*lk));
		/* add to master list */
		if ((lk->next = master.lk.list))
			lk->next->prev = &lk->next;
		lk->prev = &master.lk.list;
		master.lk.list = lk;
		master.lk.numb++;
		if (ls) {
			/* add to link set list */
			if ((lk->ls.next = ls->lk.list))
				lk->ls.next->ls.prev = &lk->ls.next;
			lk->ls.prev = &ls->lk.list;
			lk->ls.ls = ls;
			ls->lk.list = lk;
			ls->lk.numb++;
		}
		/* fill out structure */
		lk->id = lk_get_id(id);
		lk->type = MTP_OBJ_TYPE_LK;
		lk->ni = c->ni;
		lk->slot = c->slot;
		lk->sp.loc = loc;
		lk->sp.adj = adj;
		/* timer allocation */
		if (!(lk->timers.t7 = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) lk->timers.t7;
		t->timer = t7;
		t->count = 0;
		t->lk = lk;
		if (!(lk->timers.t19 = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) lk->timers.t19;
		t->timer = t19;
		t->count = 0;
		t->lk = lk;
		if (!(lk->timers.t21 = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) lk->timers.t21;
		t->timer = t21;
		t->count = 0;
		t->lk = lk;
		if (!(lk->timers.t25a = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) lk->timers.t25a;
		t->timer = t25a;
		t->count = 0;
		t->lk = lk;
		if (!(lk->timers.t28a = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) lk->timers.t28a;
		t->timer = t28a;
		t->count = 0;
		t->lk = lk;
		if (!(lk->timers.t29a = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) lk->timers.t29a;
		t->timer = t29a;
		t->count = 0;
		t->lk = lk;
		if (!(lk->timers.t30a = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) lk->timers.t30a;
		t->timer = t30a;
		t->count = 0;
		t->lk = lk;
		/* defaults inherited from ls */
		if (ls) {
			/* sls bits and mask */
			lk->sl.sls_bits = ls->lk.sls_bits;
			lk->sl.sls_mask = ls->lk.sls_mask;
			/* signalling link timer defaults */
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
			/* link timer defaults */
			lk->config.t7 = ls->config.t7;
			lk->config.t19 = ls->config.t19;
			lk->config.t21 = ls->config.t21;
			lk->config.t25a = ls->config.t25a;
			lk->config.t28a = ls->config.t28a;
			lk->config.t29a = ls->config.t29a;
			lk->config.t30a = ls->config.t30a;
		}
		if (ls) {
			/* automatically allocate routes if the link set already has 
			   routesets */
			struct rl *rl;
			struct rt *rt;

			for (rl = ls->rl.list; rl; rl = rl->ls.next)
				if (!(rt = mtp_alloc_rt(0, rl, lk, lk->slot)))
					goto free_error;
		}
		if (ls && adj) {
			/* ensure that there is a route to the adjacent via this
			   link set */
			struct rl *rl;
			struct mtp_conf_rl c = {.cost = 0, };

			for (rl = adj->rl.list; rl; rl = rl->rs.next)
				if (rl->ls.ls == ls)
					break;
			/* cost is alway zero (0) because route is to adjacent */
			if (!rl && !(rl = mtp_alloc_rl(0, adj, ls, &c)))
				goto free_error;
			adj->flags |= RSF_ADJACENT;
		}
	}
	return (lk);
      free_error:
	mtp_free_lk(lk);
	return (NULL);
}
static void
mtp_free_lk(struct lk *lk)
{
	struct cb *cb;
	struct sl *sl;
	struct rt *rt;
	struct ls *ls;

	/* stop timers */
	lk_timer_stop(lk, tall);
	/* remove all changeback buffers */
	while ((cb = lk->cb.list))
		mtp_free_cb(cb);
	/* remove all routes */
	while ((rt = lk->rt.list))
		mtp_free_rt(rt);
	/* remove all signalling links */
	while ((sl = lk->sl.list))
		mtp_free_sl(sl);
	/* remove from ls list */
	if ((ls = lk->ls.ls)) {
		if ((*lk->ls.prev = lk->ls.next))
			lk->ls.next->ls.prev = lk->ls.prev;
		lk->ls.next = NULL;
		lk->ls.prev = &lk->ls.next;
		ls->lk.numb--;
		lk->ls.ls = NULL;
	}
	if (lk->next || lk->prev != &lk->next) {
		/* remove from master list */
		if ((*lk->prev = lk->next))
			lk->next->prev = lk->prev;
		lk->next = NULL;
		lk->prev = &lk->next;
		master.lk.numb--;
	}
	/* free timer */
	mi_timer_free(lk->timers.t7);
	mi_timer_free(lk->timers.t19);
	mi_timer_free(lk->timers.t21);
	mi_timer_free(lk->timers.t25a);
	mi_timer_free(lk->timers.t28a);
	mi_timer_free(lk->timers.t29a);
	mi_timer_free(lk->timers.t30a);
	kmem_cache_free(mtp_lk_cachep, lk);
}

/*
 *  LS allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct ls *
ls_lookup(uint id)
{
	struct ls *ls = NULL;

	if (id)
		for (ls = master.ls.list; ls && ls->id != id; ls = ls->next) ;
	return (ls);
}

static uint
ls_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		id = ++sequence;
	return (id);
}
static struct ls *
mtp_alloc_ls(uint id, struct sp *sp, struct mtp_conf_ls *c)
{
	struct ls *ls;

	if ((ls = kmem_cache_alloc(mtp_ls_cachep, GFP_ATOMIC))) {
		bzero(ls, sizeof(*ls));
		/* add to master list */
		if ((ls->next = master.ls.list))
			ls->next->prev = &ls->next;
		ls->prev = &master.ls.list;
		master.ls.list = ls;
		master.ls.numb++;
		/* add to signalling point list */
		if ((ls->sp.next = sp->ls.list))
			ls->sp.next->sp.prev = &ls->sp.next;
		ls->sp.prev = &sp->ls.list;
		ls->sp.sp = sp;
		sp->ls.list = ls;
		sp->ls.numb++;
		/* fill out structure */
		ls->id = ls_get_id(id);
		ls->type = MTP_OBJ_TYPE_LS;
		ls->lk.sls_mask = c->sls_mask;
		ls->lk.sls_bits = 0;
		{
			uint mask = c->sls_mask;

			/* count the 1's in the mask */
			while (mask) {
				if (mask & 0x01)
					ls->lk.sls_bits++;
				mask >>= 1;
			}
		}
		ls->rl.sls_mask = ~c->sls_mask & sp->ls.sls_mask;
		ls->rl.sls_bits = sp->ls.sls_bits - ls->lk.sls_bits;
		/* defaults inherited from sp */
		{
			/* signalling link timer defaults */
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
			/* link timer defaults */
			ls->config.t7 = sp->config.t7;
			ls->config.t19 = sp->config.t19;
			ls->config.t21 = sp->config.t21;
			ls->config.t25a = sp->config.t25a;
			ls->config.t28a = sp->config.t28a;
			ls->config.t29a = sp->config.t29a;
			ls->config.t30a = sp->config.t30a;
		}
	}
	return (ls);
}
static void
mtp_free_ls(struct ls *ls)
{
	struct sp *sp;
	struct rl *rl;
	struct lk *lk;

#if 0
	/* stop all timers */
	ls_timer_stop(ls, tall);
#endif
	/* remove links */
	while ((lk = ls->lk.list))
		mtp_free_lk(lk);
	/* remove route lists */
	while ((rl = ls->rl.list))
		mtp_free_rl(rl);
	if ((sp = ls->sp.sp)) {
		/* remove from signalling point list */
		if ((*ls->sp.prev = ls->sp.next))
			ls->sp.next->sp.prev = ls->sp.prev;
		ls->sp.next = NULL;
		ls->sp.prev = &ls->sp.next;
		assure(sp->ls.numb > 0);
		sp->ls.numb--;
		ls->sp.sp = NULL;
	}
	if (ls->next || ls->prev != &ls->next) {
		/* remove from master list */
		if ((*ls->prev = ls->next))
			ls->next->prev = ls->prev;
		ls->next = NULL;
		ls->prev = &ls->next;
		assure(master.ls.numb > 0);
		master.ls.numb--;
	}
	ls->id = 0;
	kmem_cache_free(mtp_ls_cachep, ls);
}

/*
 *  CR allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static inline struct cr *
cr_lookup(uint id)
{
	struct cr *cr = NULL;

	if (id)
		for (cr = master.cr.list; cr && cr->id != id; cr = cr->next) ;
	return (cr);
}

static uint
cr_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		id = ++sequence;
	return (id);
}
static struct cr *
mtp_alloc_cr(uint id, struct rl *rl, struct rt *from, struct rt *onto, uint index)
{
	struct mtp_timer *t;
	struct cr *cr;

	if ((cr = kmem_cache_alloc(mtp_cr_cachep, GFP_ATOMIC))) {
		bzero(cr, sizeof(*cr));
		bufq_init(&cr->buf);
		/* add to master list */
		if ((cr->next = master.cr.list))
			cr->next->prev = &cr->next;
		cr->prev = &master.cr.list;
		master.cr.list = cr;
		master.cr.numb++;
		/* allocate timers */
		if (!(cr->timers.t6 = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) cr -.timers.t6->b_rptr;
		t->timer = t6;
		t->count = 0;
		t->cr = cr;
		/* add to route list controlled rerouting buffer list */
		if (rl) {
			if ((cr->rl.next = rl->cr.list))
				cr->rl.next->rl.prev = &cr->rl.next;
			cr->rl.prev = &rl->cr.list;
			rl->cr.list = cr;
			cr->rl.rl = rl;
		}
		cr->id = cr_get_id(id);
		cr->type = MTP_OBJ_TYPE_CR;
		cr->state = 0;
		cr->index = index;
		assure(from);
		assure(onto);
		if (from)
			cr->rt.from = from;
		if (onto)
			cr->rt.onto = onto;
	}
	return (cr);
      free_error:
	mtp_free_cr(cr);
	return (NULL);
}
static void
mtp_free_cr(struct cr *cr)
{
	struct rt *rt;
	struct rl *rl;

	/* stop timers and purge queues */
	cr_timer_stop(cr, tall);
	bufq_purge(&cr->buf);
	/* unlink from route sets */
	if ((rt = xchg(&cr->rt.onto, NULL))) {
		rt->load--;
	}
	if ((rt = xchg(&cr->rt.from, NULL))) {
		rt->load++;
	}
	if ((rl = cr->rl.rl)) {
		/* remove from route list */
		if ((*cr->rl.prev = cr->rl.next))
			cr->rl.next->rl.prev = cr->rl.prev;
		cr->rl.next = NULL;
		cr->rl.prev = &cr->rl.next;
		assure(rl->cr.numb > 0);
		rl->cr.numb--;
		cr->rl.rl = NULL;
	}
	if (cr->next || cr->prev != &cr->next) {
		/* remove from master list */
		if ((*cr->prev = cr->next))
			cr->next->prev = cr->prev;
		cr->next = NULL;
		cr->prev = &cr->next;
		assure(master.cr.numb > 0);
		master.cr.numb--;
	}
	/* free timers */
	mi_timer_free(cr->timers.t6);
	kmem_cache_free(mtp_cr_cachep, cr);
}

/*
 *  RR allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct rr *
mtp_alloc_rr(struct rs *rs, struct lk *lk)
{
	struct rr *rr;

	if ((rr = kmalloc(sizeof(*rr), GFP_ATOMIC))) {
		bzero(rr, sizeof(*rr));
		if ((rr->rs.next = rs->rr.list))
			rr->rs.next->rs.prev = &rr->rs.next;
		rr->rs.prev = &rs->rr.list;
		rs->rr.list = rr;
		rr->rs.rs = rs;
		rr->lk = lk;
	}
	return (rr);
}
static void
mtp_free_rr(struct rr *rr)
{
	/* remove from rs list */
	if (rr->rs.rs) {
		if ((*rr->rs.prev = rr->rs.next))
			rr->rs.next->rs.prev = rr->rs.prev;
		rr->rs.next = NULL;
		rr->rs.prev = &rr->rs.next;
		rr->rs.rs = NULL;
	}
	if (rr->lk)
		rr->lk = NULL;
	kfree(rr);
}

/*
 *  RT allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct rt *
rt_lookup(uint id)
{
	struct rt *rt = NULL;

	if (id)
		for (rt = master.rt.list; rt && rt->id != id; rt = rt->next) ;
	return (rt);
}

static uint
rt_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		id = ++sequence;
	return (id);
}
static struct rt *
mtp_alloc_rt(uint id, struct rl *rl, struct lk *lk, struct mtp_conf_rt *c)
{
	struct mtp_timer *t;
	struct rt *rt;

	if ((rt = kmem_cache_alloc(mtp_rt_cachep, GFP_ATOMIC))) {
		bzero(rt, sizeof(*rt));
		/* add to master list */
		if ((rt->next = master.rt.list))
			rt->next->prev = &rt->next;
		rt->prev = &master.rt.list;
		master.rt.list = rt;
		master.rt.numb++;
		if (rl) {
			/* add to route list list */
			if ((rt->rl.next = rl->rt.list))
				rt->rl.next->rl.prev = &rt->rl.next;
			rt->rl.prev = &rl->rt.list;
			rt->rl.rl = rl;
			rl->rt.list = rt;
			rl->rt.numb++;
		}
		if (lk) {
			/* add to link set list */
			if ((rt->lk.next = lk->rt.list))
				rt->lk.next->lk.prev = &rt->lk.next;
			rt->lk.prev = &lk->rt.list;
			rt->lk.lk = lk;
			lk->rt.list = rt;
			lk->rt.numb++;
		}
		/* fill out structure */
		rt->id = rt_get_id(id);
		rt->type = MTP_OBJ_TYPE_RT;
		rt->state = RT_ALLOWED;
		rt->flags = 0;
		rt->load = 0;	/* not loaded yet */
		rt->slot = c->slot;
		/* allocate timers */
		if (!(rt->timers.t10 = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) rt->timers.t10->b_rptr;
		t->timer = t10;
		t->count = 0;
		t->rt = rt;
		/* defaults inherited from rl */
		{
			/* route timer defaults */
			rt->config.t6 = rl->config.t6;
			rt->config.t10 = rl->config.t10;
		}
	}
	return (rt);
      free_error:
	mtp_free_rt(rt);
	return (NULL);
}
static void
mtp_free_rt(struct rt *rt)
{
	struct lk *lk;
	struct rl *rl;

	/* stop timers */
	rt_timer_stop(rt, tall);
	/* remove from rl list */
	if ((rl = rt->rl.rl)) {
		if ((*rt->rl.prev = rt->rl.next))
			rt->rl.next->rl.prev = rt->rl.prev;
		rt->rl.next = NULL;
		rt->rl.prev = &rt->rl.next;
		assure(rl->rt.numb > 0);
		rl->rt.numb--;
		rt->rl.rl = NULL;
	}
	/* remove from lk list */
	if ((lk = rt->lk.lk)) {
		if ((*rt->lk.prev = rt->lk.next))
			rt->lk.next->lk.prev = rt->lk.prev;
		rt->lk.next = NULL;
		rt->lk.prev = &rt->lk.next;
		assure(lk->rt.numb > 0);
		lk->rt.numb--;
		rt->lk.lk = NULL;
	}
	if (rt->next || rt->prev != &rt->next) {
		/* remove from master list */
		if ((*rt->prev = rt->next))
			rt->next->prev = rt->prev;
		rt->next = NULL;
		rt->prev = &rt->next;
		master.rt.numb--;
	}
	/* free timers */
	mi_timer_free(rt->timers.t10);
	kmem_cache_free(mtp_rt_cachep, rt);
}

/*
 *  RL allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct rl *
rl_lookup(uint id)
{
	struct rl *rl = NULL;

	if (id)
		for (rl = master.rl.list; rl && rl->id != id; rl = rl->next) ;
	return (rl);
}

static uint
rl_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		id = ++sequence;
	return (id);
}
static struct rl *
mtp_alloc_rl(uint id, struct rs *rs, struct ls *ls, struct mtp_conf_rl *c)
{
	struct rl *rl;

	ensure(rs && ls, return (NULL));
	if ((rl = kmem_cache_alloc(mtp_rl_cachep, GFP_ATOMIC))) {
		struct rl **rlp;

		bzero(rl, sizeof(*rl));
		/* add to master list */
		if ((rl->next = master.rl.list))
			rl->next->prev = &rl->next;
		rl->prev = &master.rl.list;
		master.rl.list = rl;
		master.rl.numb++;
		if (rs) {
			/* add to routset list (descending cost) */
			for (rlp = &rs->rl.list; (*rlp) && (*rlp)->cost < c->cost;
			     rlp = &(*rlp)->rs.next) ;
			if ((rl->rs.next = *rlp))
				rl->rs.next->rs.prev = &rl->rs.next;
			rl->rs.prev = rlp;
			rl->rs.rs = rs;
			*rlp = rl;
			rs->rl.numb++;
		}
		if (ls) {
			/* add to link set list */
			if ((rl->ls.next = ls->rl.list))
				rl->ls.next->ls.prev = &rl->ls.next;
			rl->ls.prev = &ls->rl.list;
			rl->ls.ls = ls;
			ls->rl.list = rl;
			ls->rl.numb++;
		}
		rl->id = rl_get_id(id);
		rl->type = MTP_OBJ_TYPE_RL;
		rl->cost = c->cost;
		/* defaults inherited from rs */
		if (rs) {
			/* route timer defaults */
			rl->config.t6 = rs->config.t6;
			rl->config.t10 = rs->config.t10;
		}
		/* defaults inherited from ls */
		if (ls) {
			rl->rt.sls_bits = ls->rl.sls_bits;
			rl->rt.sls_mask = ls->rl.sls_mask;
		}
		if (rs && ls) {
			struct lk *lk;
			struct rt *rt;

			/* automatically allocate routes */
			for (lk = ls->lk.list; lk; lk = lk->ls.next) {
				struct mtp_conf_rt c = {.slot = lk->slot, };

				if (!(rt = mtp_alloc_rt(0, rl, lk, &c)))
					goto free_error;
			}
		}
	}
	return (rl);
      free_error:
	mtp_free_rl(rl);
	return (NULL);
}
static void
mtp_free_rl(struct rl *rl)
{
	struct cr *cr;
	struct rt *rt;
	struct ls *ls;
	struct rs *rs;

	/* stop timers */
	// __rl_timer_stop(rl, tall); /* no timers */
	/* remove all controlled rerouting buffers */
	while ((cr = rl->cr.list))
		mtp_free_cr(cr);
	/* remove all routes */
	while ((rt = rl->rt.list))
		mtp_free_rt(rt);
	/* remove from rs list */
	if ((rs = rl->rs.rs)) {
		if ((*rl->rs.prev = rl->rs.next))
			rl->rs.next->rs.prev = rl->rs.prev;
		rl->rs.next = NULL;
		rl->rs.prev = &rl->rs.next;
		rs->rl.numb--;
		rl->rs.rs = NULL;
	}
	/* remove from ls list */
	if ((ls = rl->ls.ls)) {
		if ((*rl->ls.prev = rl->ls.next))
			rl->ls.next->ls.prev = rl->ls.prev;
		rl->ls.next = NULL;
		rl->ls.prev = &rl->ls.next;
		ls->rl.numb--;
		rl->ls.ls = NULL;
	}
	if (rl->next || rl->prev != &rl->next) {
		/* remove from master list */
		if ((*rl->prev = rl->next))
			rl->next->prev = rl->prev;
		rl->next = NULL;
		rl->prev = &rl->next;
		master.rl.numb--;
	}
	kmem_cache_free(mtp_rl_cachep, rl);
}

/*
 *  RS allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct rs *
rs_lookup(uint id)
{
	struct rs *rs = NULL;

	if (id)
		for (rs = master.rs.list; rs && rs->id != id; rs = rs->next) ;
	return (rs);
}

static uint
rs_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		id = ++sequence;
	return (id);
}
static struct rs *
mtp_alloc_rs(uint id, struct sp *sp, struct mtp_conf_rs *c)
{
	struct mtp_timer *t;
	struct rs *rs;

	if ((rs = kmem_cache_alloc(mtp_rs_cachep, GFP_ATOMIC))) {
		bzero(rs, sizeof(*rs));
		/* add to master list */
		if ((rs->next = master.rs.list))
			rs->next->prev = &rs->next;
		rs->prev = &master.rs.list;
		master.rs.list = rs;
		master.rs.numb++;
		if (sp) {
			/* add to sp list */
			if ((rs->sp.next = sp->rs.list))
				rs->sp.next->sp.prev = &rs->sp.next;
			rs->sp.prev = &sp->rs.list;
			rs->sp.sp = sp;
			sp->rs.list = rs;
			sp->rs.numb++;
		}
		/* fill out structure */
		rs->id = rs_get_id(id);
		rs->type = MTP_OBJ_TYPE_RS;
		rs->flags =
		    c->
		    flags & (RSF_TFR_PENDING | RSF_ADJACENT | RSF_CLUSTER |
			     RSF_XFER_FUNC);
		rs->dest = c->dest;
		/* allocate timers */
		if (!(rs->timers.t8 = mi_alloc_timer(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) rs->timers.t8->b_rptr;
		t->timer = t8;
		t->count = 0;
		t->rs = rs;
		if (!(rs->timers.t11 = mi_alloc_timer(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) rs->timers.t11->b_rptr;
		t->timer = t11;
		t->count = 0;
		t->rs = rs;
		if (!(rs->timers.t15 = mi_alloc_timer(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) rs->timers.t15->b_rptr;
		t->timer = t15;
		t->count = 0;
		t->rs = rs;
		if (!(rs->timers.t16 = mi_alloc_timer(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) rs->timers.t16->b_rptr;
		t->timer = t16;
		t->count = 0;
		t->rs = rs;
		if (!(rs->timers.t18a = mi_alloc_timer(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) rs->timers.t18a->b_rptr;
		t->timer = t18a;
		t->count = 0;
		t->rs = rs;
		/* defaults inherited from sp */
		{
			/* route timer defaults */
			rs->config.t6 = sp->config.t6;
			rs->config.t10 = sp->config.t10;
			/* route set timer defaults */
			rs->config.t8 = sp->config.t8;
			rs->config.t11 = sp->config.t11;
			rs->config.t15 = sp->config.t15;
			rs->config.t16 = sp->config.t16;
			rs->config.t18a = sp->config.t18a;
		}
	}
	return (rs);
      free_error:
	mtp_free_rs(rs);
	return (NULL);
}
static void
mtp_free_rs(struct rs *rs)
{
	struct rr *rr;
	struct rl *rl;
	struct sp *sp;

	/* stop timers */
	rs_timer_stop(rs, tall);
	/* remove all route restrictions */
	while ((rr = rs->rr.list))
		mtp_free_rr(rr);
	/* remove all route lists */
	while ((rl = rs->rl.list))
		mtp_free_rl(rl);
	/* remove from sp list */
	if ((sp = rs->sp.sp)) {
		if ((*rs->sp.prev = rs->sp.next))
			rs->sp.next->sp.prev = rs->sp.prev;
		rs->sp.next = NULL;
		rs->sp.prev = &rs->sp.next;
		sp->rs.numb--;
		rs->sp.sp = NULL;
	}
	if (rs->next || rs->prev != &rs->next) {
		/* remove from master list */
		if ((*rs->prev = rs->next))
			rs->next->prev = rs->prev;
		rs->next = NULL;
		rs->prev = &rs->next;
		master.rs.numb--;
	}
	/* free timers */
	mi_timer_free(rs->timers.t8);
	mi_timer_free(rs->timers.t11);
	mi_timer_free(rs->timers.t15);
	mi_timer_free(rs->timers.t16);
	mi_timer_free(rs->timers.t18a);
	kmem_cache_free(mtp_rs_cachep, rs);
}

/*
 *  SP allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct sp *
sp_lookup(uint id)
{
	struct sp *sp = NULL;

	if (id)
		for (sp = master.sp.list; sp && sp->id != id; sp = sp->next) ;
	return (sp);
}

static uint
sp_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		id = ++sequence;
	return (id);
}
static struct sp *
mtp_alloc_sp(uint id, struct na *na, struct mtp_conf_sp *c)
{
	struct mtp_timer *t;
	struct sp *sp;

	if ((sp = kmem_cache_alloc(mtp_sp_cachep, GFP_ATOMIC))) {
		bzero(sp, sizeof(*sp));
		/* add to master list */
		if ((sp->next = master.sp.list))
			sp->next->prev = &sp->next;
		sp->prev = &master.sp.list;
		master.sp.list = sp;
		master.sp.numb++;
		if (na) {
			/* add to na list */
			if ((sp->na.next = na->sp.list))
				sp->na.next->na.prev = &sp->na.next;
			sp->na.prev = &na->sp.list;
			sp->na.na = na;
			na->sp.list = sp;
			na->sp.numb++;
		}
		/* fill out structure */
		sp->id = sp_get_id(id);
		sp->type = MTP_OBJ_TYPE_SP;
		sp->flags =
		    c->
		    flags & (SPF_CLUSTER | SPF_LOSC_PROC_A | SPF_LOSC_PROC_B |
			     SPF_SECURITY | SPF_XFER_FUNC);
		sp->pc = c->pc;
		sp->mtp.equipped = c->users;
		/* allocate timers */
		if (!(sp->timers.t1r = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) sp->timers.t1r->b_rptr;
		t->timer = t1r;
		t->count = 0;
		t->sp = sp;
		if (!(sp->timers.t18 = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) sp->timers.t18->b_rptr;
		t->timer = t18;
		t->count = 0;
		t->sp = sp;
		if (!(sp->timers.t20 = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) sp->timers.t20->b_rptr;
		t->timer = t20;
		t->count = 0;
		t->sp = sp;
		if (!(sp->timers.t22a = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) sp->timers.t22a->b_rptr;
		t->timer = t22a;
		t->count = 0;
		t->sp = sp;
		if (!(sp->timers.t23a = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) sp->timers.t23a->b_rptr;
		t->timer = t23a;
		t->count = 0;
		t->sp = sp;
		if (!(sp->timers.t24a = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) sp->timers.t24a->b_rptr;
		t->timer = t24a;
		t->count = 0;
		t->sp = sp;
		if (!(sp->timers.t26a = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) sp->timers.t26a->b_rptr;
		t->timer = t26a;
		t->count = 0;
		t->sp = sp;
		if (!(sp->timers.t27a = mi_timer_alloc(sizeof(*t))))
			goto free_error;
		t = (typeof(t)) sp->timers.t27a->b_rptr;
		t->timer = t27a;
		t->count = 0;
		t->sp = sp;
		/* defaults inherited from na */
		{
			/* sls bits and mask */
			sp->ls.sls_bits = na->sp.sls_bits;
			sp->ls.sls_mask = na->sp.sls_mask;
			/* signalling link timer defaults */
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
			/* link timer defaults */
			sp->config.t7 = na->config.t7;
			sp->config.t19 = na->config.t19;
			sp->config.t21 = na->config.t21;
			sp->config.t25a = na->config.t25a;
			sp->config.t28a = na->config.t28a;
			sp->config.t29a = na->config.t29a;
			sp->config.t30a = na->config.t30a;
			/* route timer defaults */
			sp->config.t6 = na->config.t6;
			sp->config.t10 = na->config.t10;
			/* route set timer defaults */
			sp->config.t8 = na->config.t8;
			sp->config.t11 = na->config.t11;
			sp->config.t15 = na->config.t15;
			sp->config.t16 = na->config.t16;
			sp->config.t18a = na->config.t18a;
			/* signalling point timer defaults */
			sp->config.t1r = na->config.t1r;
			sp->config.t18 = na->config.t18;
			sp->config.t20 = na->config.t20;
			sp->config.t22a = na->config.t22a;
			sp->config.t23a = na->config.t23a;
			sp->config.t24a = na->config.t24a;
			sp->config.t26a = na->config.t26a;
			sp->config.t27a = na->config.t27a;
		}
	}
	return (sp);
      free_error:
	mtp_free_sp(sp);
	return (NULL);
}
static void
mtp_free_sp(struct sp *sp)
{
	struct ls *ls;
	struct rs *rs;
	struct na *na;

	/* stop timers */
	sp_timer_stop(sp, tall);
	fixme(("Hangup on all mtp users\n"));
	/* remove all route sets */
	while ((rs = sp->rs.list))
		mtp_free_rs(rs);
	/* remove all link sets */
	while ((ls = sp->ls.list))
		mtp_free_ls(ls);
	/* remove from na list */
	if ((na = sp->na.na)) {
		if ((*sp->na.prev = sp->na.next))
			sp->na.next->na.prev = sp->na.prev;
		sp->na.next = NULL;
		sp->na.prev = &sp->na.next;
		na->sp.numb--;
		sp->na.na = NULL;
	}
	if (sp->next || sp->prev != &sp->next) {
		/* remove from master list */
		if ((*sp->prev = sp->next))
			sp->next->prev = sp->prev;
		sp->next = NULL;
		sp->prev = &sp->next;
		master.sp.numb--;
	}
	/* free timers */
	mi_timer_free(sp->timers.t1r);
	mi_timer_free(sp->timers.t18);
	mi_timer_free(sp->timers.t20);
	mi_timer_free(sp->timers.t22a);
	mi_timer_free(sp->timers.t23a);
	mi_timer_free(sp->timers.t24a);
	mi_timer_free(sp->timers.t26a);
	mi_timer_free(sp->timers.t27a);
	kmem_cache_free(mtp_sp_cachep, sp);
}

/*
 *  NA allocation and deallocation
 *  -------------------------------------------------------------------------
 */
static struct na *
mtp_alloc_na(uint id, struct mtp_conf_na *c)
mtp_alloc_na(uint id, uint32_t member, uint32_t cluster, uint32_t network,
	     uint sls_bits, struct lmi_option *option)
{
	struct na *na;

	if ((na = kmem_cache_alloc(mtp_na_cachep, GFP_ATOMIC))) {
		bzero(na, sizeof(*na));
		/* add to master list */
		if ((na->next = master.na.list))
			na->next->prev = &na->next;
		na->prev = &master.na.list;
		master.na.list = na;
		master.na.numb++;
		/* fill out structure */
		na->id = na_get_id(id);
		na->type = MTP_OBJ_TYPE_NA;
		na->mask.member = c->mask.member;
		na->mask.cluster = c->mask.cluster;
		na->mask.network = c->mask.network;
		na->sp.sls_bits = c->sls_bits;
		na->sp.sls_mask = (1 << sls_bits) - 1;
		na->option = c->options;
		/* populate defaults based on protoocl variant */
		switch ((na->option.pvar & SS7_PVAR_MASK)) {
		default:
		case SS7_PVAR_ITUT:
			na->config = itut_na_config_default;
			na->prot[0] = &t_clts_prot_default;
			na->prot[1] = &t_clts_prot_default;
			na->prot[2] = &t_clts_prot_default;
			na->prot[3] = &t_clts_prot_default;
			na->prot[4] = &t_clts_prot_default;
			na->prot[5] = &t_cots_prot_default;
			na->prot[8] = &t_clts_prot_default;
			break;
		case SS7_PVAR_ETSI:
			na->config = etsi_na_config_default;
			na->prot[0] = &t_clts_prot_default;
			na->prot[1] = &t_clts_prot_default;
			na->prot[2] = &t_clts_prot_default;
			na->prot[3] = &t_clts_prot_default;
			na->prot[4] = &t_clts_prot_default;
			na->prot[5] = &t_cots_prot_default;
			na->prot[8] = &t_clts_prot_default;
			break;
		case SS7_PVAR_ANSI:
			na->config = ansi_na_config_default;
			na->prot[0] = &t_clts_prot_default;
			na->prot[1] = &t_clts_prot_default;
			na->prot[2] = &t_clts_prot_default;
			na->prot[3] = &t_clts_prot_default;
			na->prot[4] = &t_clts_prot_default;
			na->prot[5] = &t_cots_prot_default;
			na->prot[8] = &t_clts_prot_default;
			break;
		case SS7_PVAR_JTTC:
			na->config = jttc_na_config_default;
			na->prot[0] = &t_clts_prot_default;
			na->prot[1] = &t_clts_prot_default;
			na->prot[2] = &t_clts_prot_default;
			na->prot[3] = &t_clts_prot_default;
			na->prot[4] = &t_clts_prot_default;
			na->prot[5] = &t_cots_prot_default;
			na->prot[8] = &t_clts_prot_default;
			break;
		}
	}
	return (na);
}
static void
mtp_free_na(struct na *na)
{
	struct sp *sp;

	/* remove all attached signalling points */
	while ((sp = na->sp.list))
		mtp_free_sp(sp);
	if (na->next || na->prev != &na->next) {
		/* remove from master list */
		if ((*na->prev = na->next))
			na->next->prev = na->prev;
		na->next = NULL;
		na->prev = &na->next;
		master.na.numb--;
	}
	kmem_cache_free(mtp_na_cachep, na);
}
static struct na *
na_lookup(uint id)
{
	struct na *na = NULL;

	if (id)
		for (na = master.na.list; na && na->id != id; na = na->next) ;
	return (na);
}

static uint
na_get_id(uint id)
{
	static uint sequence = 0;

	if (!id)
		id = ++sequence;
	return (id);
}

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

static struct module_stat mtp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mtp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct module_info mtp_winfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME "-wr",	/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};
static struct module_info mtp_rinfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME "-rd",	/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};
static struct module_info sl_winfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME "-muxw",	/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};
static struct module_info sl_rinfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME "-muxr",	/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct qinit mtp_rinit = {
	.qi_putp = mtp_rput,		/* Write put (message from below) */
	.qi_srvp = mtp_rsrv,		/* Write queue service */
	.qi_qopen = mtp_qopen,		/* Each open */
	.qi_qclose = mtp_qclose,	/* Last close */
	.qi_minfo = &mtp_rinfo,		/* Information */
	.qi_mstat = &mtp_rstat,		/* Statistics */
};

static struct qinit mtp_winit = {
	.qi_putp = mtp_wput,		/* Write put (message from above) */
	.qi_srvp = mtp_wsrv,		/* Write queue service */
	.qi_minfo = &mtp_winfo,		/* Information */
	.qi_mstat = &mtp_wstat,		/* Statistics */
};

static struct qinit sl_rinit = {
	.qi_putp = sl_rput,		/* Write put (message from below) */
	.qi_srvp = sl_rsrv,		/* Write queue service */
	.qi_minfo = &sl_rinfo,		/* Information */
	.qi_mstat = &sl_rstat,		/* Statistics */
};

static struct qinit sl_winit = {
	.qi_putp = sl_wput,		/* Write put (message from above) */
	.qi_srvp = sl_wsrv,		/* Write queue service */
	.qi_minfo = &sl_winfo,		/* Information */
	.qi_mstat = &sl_wstat,		/* Statistics */
};

MODULE_STATIC struct streamtab mtpinfo = {
	.st_rdinit = &mtp_rinit,	/* Upper read queue */
	.st_wrinit = &mtp_winit,	/* Upper write queue */
	.st_muxrinit = &sl_rinit,	/* Lower read queue */
	.st_muxwinit = &sl_winit,	/* Lower write queue */
};

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
MODULE_PARM_DESC(modid, "Module ID for the MTP driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the MTP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static struct cdevsw mtp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &mtpinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
mtpinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = mtp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME,
			err);
		return (err);
	}
	if ((err = register_strdev(&mtp_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver, err = %d", DRV_NAME,
			err);
		mtp_term_caches();
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

MODULE_STATIC void __exit
mtpterminate(void)
{
	int err;

	if (major && (err = unregister_strdev(&mtp_cdev, major)) < 0)
		cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME, major);
	if ((err = mtp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(mtpinit);
module_exit(mtpterminate);

#endif				/* LINUX */
