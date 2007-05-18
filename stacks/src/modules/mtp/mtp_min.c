/*****************************************************************************

 @(#) $RCSfile: mtp_min.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2007/05/18 00:00:50 $

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

 Last Modified $Date: 2007/05/18 00:00:50 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtp_min.c,v $
 Revision 0.9.2.16  2007/05/18 00:00:50  brian
 - check for nf_reset

 Revision 0.9.2.15  2007/03/25 18:59:48  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.14  2007/02/17 02:49:16  brian
 - first clean recompile of MTP modules on LFS

 Revision 0.9.2.13  2006/05/14 06:58:08  brian
 - removed redundant or unused QR_ definitions

 Revision 0.9.2.12  2006/03/04 13:00:14  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: mtp_min.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2007/05/18 00:00:50 $"

static char const ident[] = "$RCSfile: mtp_min.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2007/05/18 00:00:50 $";

/*
 *  This an MTP (Message Transfer Part) multiplexing driver which can have SL (Signalling Link)
 *  streams I_LINK'ed or I_PLINK'ed underneath it to form a complete Message Transfer Part protocol
 *  layer for SS7.  This is a minimal implementation which is suitable for GSM-A or F-Links only
 *  between SEPs.
 */
#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>
#include <linux/socket.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>

#include <sys/tpi.h>
#include <sys/tpi_sctp.h>
#include <sys/xti_ss7.h>
#include <sys/xti_mtp.h>

#define MTP_MIN_DESCRIP		"SS7 MESSAGE TRANSFER PART (MTP) STREAMS MULTIPLEXING DRIVER."
#define MTP_MIN_REVISION	"OpenSS7 $RCSfile: mtp_min.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2007/05/18 00:00:50 $"
#define MTP_MIN_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define MTP_MIN_DEVICE		"Part of the OpenSS7 Stack for Linux STREAMS."
#define MTP_MIN_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MTP_MIN_LICENSE		"GPL"
#define MTP_MIN_BANNER		MTP_MIN_DESCRIP		"\n" \
				MTP_MIN_REVISION	"\n" \
				MTP_MIN_COPYRIGHT	"\n" \
				MTP_MIN_DEVICE		"\n" \
				MTP_MIN_CONTACT
#define MTP_MIN_SPLASH		MTP_MIN_DESCRIP		"\n" \
				MTP_MIN_REVISION

#ifdef LINUX
MODULE_AUTHOR(MTP_MIN_CONTACT);
MODULE_DESCRIPTION(MTP_MIN_DESCRIP);
MODULE_SUPPORTED_DEVICE(MTP_MIN_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MTP_MIN_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mtp_min");
#endif
#endif				/* LINUX */

#ifdef LFS
#define MTP_MIN_DRV_ID		CONFIG_STREAMS_MTP_MIN_MODID
#define MTP_MIN_DRV_NAME	CONFIG_STREAMS_MTP_MIN_NAME
#define MTP_MIN_CMAJORS		CONFIG_STREAMS_MTP_MIN_NMAJORS
#define MTP_MIN_CMAJOR_0	CONFIG_STREAMS_MTP_MIN_MAJOR
#define MTP_MIN_UNITS		CONFIG_STREAMS_MTP_MIN_NMINORS
#endif

#define DRV_ID		MTP_MIN_DRV_ID
#define DRV_NAME	MTP_MIN_DRV_NAME
#define CMAJORS		MTP_MIN_CMAJORS
#define CMAJOR_0	MTP_MIN_CMAJOR_0
#define UNITS		MTP_MIN_UNITS
#ifdef MODULE
#define DRV_BANNER	MTP_MIN_BANNER
#else				/* MODULE */
#define DRV_BANNER	MTP_MIN_SPLASH
#endif				/* MODULE */

/*
 *  =========================================================================
 *
 *  Private structures
 *
 *  =========================================================================
 */

typedef struct mtp_opts {
	uint flags;			/* success flags */
	t_uscalar_t *pvar;
	t_uscalar_t *mplev;
	t_uscalar_t *debug;
	t_uscalar_t *cluster;
	t_uscalar_t *sls;
	t_uscalar_t *mp;
} mtp_opts_t;

typedef struct mtp_options {
	uint flags;			/* success flags */
	t_uscalar_t pvar;
	t_uscalar_t mplev;
	t_uscalar_t debug;
	t_uscalar_t cluster;
	t_uscalar_t sls;
	t_uscalar_t mp;
} mtp_options_t;

static mtp_options_t mtp_opt_defaults = { SS7_PVAR_ETSI_00, 0, 0, 0, 0, 0 };

/*
   list linkage 
 */
#define HEAD_DECLARATION(__type) \
	__type *next;			/* list linkage */ \
	__type **prev;			/* list linkage */ \
	size_t refcnt;			/* reference count */ \
	spinlock_t lock;		/* structure lock */ \
	ulong id;			/* structure id */ \


typedef struct head {
	HEAD_DECLARATION (struct head);	/* head declaration */
} head_t;

/*
   stream related structures 
 */
#define STR_DECLARATION(__type) \
	HEAD_DECLARATION(__type);	/* list linkage */ \
	union { \
		struct { \
			major_t cmajor;	/* device major */ \
			minor_t cminor;	/* device minor */ \
		} dev; \
		struct { \
			ulong index;	/* linked index */ \
		} mux; \
	} u; \
	queue_t *rq;			/* rd queue */ \
	queue_t *wq;			/* wr queue */ \
	spinlock_t qlock;		/* queue lock */ \
	uint rbid;			/* rd bufcall id */ \
	uint wbid;			/* wr bufcall id */ \
	queue_t *rwait;			/* rd queue waiting */ \
	queue_t *wwait;			/* wr queue waiting */ \
	ulong state;			/* interface state */ \
	ulong flags;			/* interface flags */ \
	ulong style;			/* interface style */ \
	ulong version			/* interface version */ \


/*
   generic stream structure 
 */
typedef struct str {
	STR_DECLARATION (struct str);	/* stream declaration */
} str_t;

#define PRIV(__q) ((str_t *)(__q)->q_ptr)

struct mtp;				/* MTP User */
struct na;				/* Network Appearance */
struct sp;				/* Signalling Point (Local/Adjacent/Remote) */
struct rs;				/* Route Set */
struct rl;				/* Route List */
struct rt;				/* Route */
struct ls;				/* (Combined) Link Set */
struct lk;				/* Link (Set) */
struct sl;				/* Signalling Link */

/*
   mtp user 
 */
struct mt {
	queue_t *rq;
	queue_t *wq;
	cred_t cred;
	struct na *na;			/* network appearance */
	struct mtp_addr src;		/* srce address */
	struct mtp_addr dst;		/* dest address */
	struct {
		struct sp *loc;		/* this mtp user for local signalling point */
		struct sp *rem;		/* this mtp user for remote signalling point */
		struct mtp *next;	/* next mtp user for local signalling point */
		struct mtp **prev;	/* prev mtp user for local signalling point */
	} sp;
	struct sl *sl;			/* signalling link */
	mtp_options_t options;
	// mtp_opt_conf_mtp_t timers; /* MTP timers */
	// mtp_opt_conf_mtp_t config; /* MTP configuration */
};

#define MT_PRIV(__q) ((struct mt *)(__q)->q_ptr)

static struct mt *mtp_list = NULL;

/*
   network appearance 
 */
typedef struct na {
	HEAD_DECLARATION (struct na);	/* head declaration */
	struct {
		uint32_t member;	/* pc member mask */
		uint32_t cluster;	/* pc cluster mask */
		uint32_t network;	/* pc network mask */
	} mask;
	lmi_option_t option;		/* protocol variant and options */
	struct T_info_ack *prot[16];	/* protoocl profiles for si values */
} na_t;

static ulong na_index = 0;
static na_t *na_list = NULL;

/*
   route set 
 */
typedef struct rs {
	HEAD_DECLARATION (struct rs);	/* head declaration */
	int type;			/* type of routeset (cluster, member) */
	int cong_status;		/* congestion status */
	int disc_status;		/* discard status */
	uint32_t dest;			/* remote signalling point/cluster */
	struct {
		struct sp *sp;		/* this routeset for this signalling point */
		struct rs *next;	/* prev routeset for this signalling point */
		struct rs **prev;	/* next routeset for this signalling point */
	} sp;
	struct {
		size_t numb;		/* numb of route lists in this routeset */
		struct rl *list;	/* list of route lists in this routeset */
		struct rl *rl;		/* current route list for this routeset */
	} rl;
	mtp_opt_conf_rs_t timers;	/* RS timers */
	mtp_opt_conf_rs_t config;	/* RS configuration */
} rs_t;

static ulong rs_index = 0;
static rs_t *rs_list = NULL;

/*
   route list 
 */
typedef struct rl {
	HEAD_DECLARATION (struct rl);	/* head declaration */
	ulong cost;			/* priority of this route list */
	struct {
		struct sp *sp;		/* signalling point for this route list */
	} sp;
	struct {
		ulong cost;		/* cost of route list in routset */
		struct rs *rs;		/* this route list for this route set */
		struct rl *next;	/* next route list for this route set */
		struct rl **prev;	/* prev route list for this route set */
	} rs;
	struct {
		size_t numb;		/* numb of routes in this route list */
		struct rt *list;	/* list of routes in this route list */
		size_t actv;		/* active routes in this route list */
		struct rt *smap[4];	/* smap of routes in this route list */
	} rt;
	struct {
		struct ls *ls;		/* this route list for this linkset */
		struct rl *next;	/* next route list for this linkset */
		struct rl **prev;	/* prev route list for this linkset */
	} ls;
} rl_t;

static ulong rl_index = 0;
static rl_t *rl_list = NULL;

/*
   route 
 */
typedef struct rt {
	HEAD_DECLARATION (struct rt);	/* head declaration */
	struct {
		struct sp *sp;		/* signalling point for this route */
	} sp;
	struct {
		struct rl *rl;		/* this route in this route list */
		struct rt *next;	/* next route in this route list */
		struct rt **prev;	/* prev route in this route list */
	} rl;
	struct {
		struct lk *lk;		/* this route to this link */
		struct rt *next;	/* next route to this link */
		struct rt **prev;	/* prev route to this link */
	} lk;
	struct bufq *buf;		/* time controlled rerouting buffer */
	mtp_opt_conf_rt_t timers;	/* RT timers */
	mtp_opt_conf_rt_t config;	/* RT configuration */
} rt_t;

static ulong rt_index = 0;
static rt_t *rt_list = NULL;

/*
   signalling point 
 */
typedef struct sp {
	HEAD_DECLARATION (struct sp);	/* head declaration */
	uint type;			/* point code type */
	uint32_t ni;			/* network indicator */
	uint32_t pc;			/* point code */
	uint sls;			/* sls for loadsharing of management messages */
	struct na *na;			/* associated network appearance */
	struct {
		ushort equipped;	/* bit mask of equipped users */
		ushort available;	/* bit mask of available users */
		struct mtp *lists[16];	/* lists of MTP users */
	} mtp;
	struct {
		size_t numb;		/* numb of route sets */
		struct rs *list;	/* list of route sets */
		struct rs **hash;	/* hash of route sets */
	} rs;
	struct {
		size_t numb;		/* numb of linksets */
		struct ls *list;	/* list of linksets */
	} ls;
	mtp_opt_conf_sp_t timers;	/* SP timers */
	mtp_opt_conf_sp_t config;	/* SP configuration */
} sp_t;

static ulong sp_index = 0;
struct sp *sp_list = NULL;

/*
   link set 
 */
typedef struct ls {
	HEAD_DECLARATION (struct ls);	/* head declaration */
	struct {
		struct sp *sp;		/* this link set on this signalling point */
		struct ls *next;	/* next link set on this signalling point */
		struct ls **prev;	/* prev link set on this signalling point */
	} sp;
	struct {
		struct rl *list;	/* list of route lists using this linkset */
	} rl;
	struct {
		struct lk *list;	/* list of links in this linkset */
		uint sls_mask;		/* mask for selecting links in this linkset */
	} lk;
	struct sl **sls_map;		/* sls map for link set */
} ls_t;

static ulong ls_index = 0;
static ls_t *ls_list = NULL;

/*
   link 
 */
typedef struct lk {
	HEAD_DECLARATION (struct lk);	/* head declaration */
	struct {
		struct sp *sp;		/* signalling point for this link */
	} sp;
	struct {
		struct ls *ls;		/* this link in this linkset */
		struct lk *next;	/* next link in this linkset */
		struct lk **prev;	/* prev link in this linkset */
		uint sls_slot;		/* sls slot for this linkset */
	} ls;
	struct {
		struct rt *list;	/* list of routes using this link */
	} rt;
	struct {
		struct sl *list;	/* list of signalling links in this link */
		struct sl **smap;	/* sls map of signalling links in this link */
		uint smap_mask;		/* mask of sls for sls map */
	} sl;
	ulong ni;			/* network indicator for link */
	struct sp *loc;			/* local signalling point */
	struct sp *adj;			/* adjacent signalling point */
	mtp_opt_conf_lk_t timers;	/* link timers */
	mtp_opt_conf_lk_t config;	/* link configuration */
} lk_t;

static ulong lk_index = 0;
static lk_t *lk_list = NULL;

/*
   signalling link 
 */
struct sl {
	struct {
		struct sp *loc;		/* local signalling point for this signalling link */
		struct sp *adj;		/* adjacent signalling point for this signalling link */
	} sp;
	struct {
		struct lk *lk;		/* this signalling link in this link */
		struct sl *next;	/* next signalling link in this link */
		struct sl **prev;	/* prev signalling link in this link */
	} lk;
	ulong slc;			/* signalling link code */
	int cong_status;		/* congestion status */
	int disc_status;		/* discard status */
	struct bufq buff;		/* retrieval buffer */
	uint fsnc;			/* retreived BSNT */
	uint reason;			/* reason for failure */
	unsigned char tdata[16];	/* test data */
	size_t tlen;			/* test length */
	mtp_opt_conf_sl_t timers;	/* SL timers */
	mtp_opt_conf_sl_t config;	/* SL configuration */
};

static struct sl *sl_list = NULL;

#define SL_PRIV(__q) ((struct sl *)(__q)->q_ptr)

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */
#ifndef _T_ALIGN_SIZE
#define _T_ALIGN_SIZE sizeof(t_uscalar_t)
#endif
#ifndef _T_ALIGN_SIZEOF
#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))
#endif
static size_t
mtp_opts_size(struct mt *mt, mtp_opts_t * ops)
{
	size_t len = 0;

	if (ops) {
		const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */

		if (ops->pvar)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->pvar));
		if (ops->mplev)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->mplev));
		if (ops->debug)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->debug));
		if (ops->cluster)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->cluster));
		if (ops->sls)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->sls));
		if (ops->mp)
			len += hlen + _T_ALIGN_SIZEOF(*(ops->mp));
	}
	return (len);
}
static void
mtp_build_opts(struct mt *mt, mtp_opts_t * ops, unsigned char *p)
{
	if (ops) {
		struct t_opthdr *oh;
		const size_t hlen = sizeof(struct t_opthdr);

		if (ops->pvar) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->pvar));
			oh->level = T_SS7_SS7;
			oh->name = T_SS7_PVAR;
			oh->status = (ops->flags & (1 << T_SS7_PVAR)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->pvar)) p) = *(ops->pvar);
			p += _T_ALIGN_SIZEOF(*ops->pvar);
		}
		if (ops->mplev) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->mplev));
			oh->level = T_SS7_SS7;
			oh->name = T_SS7_MPLEV;
			oh->status = (ops->flags & (1 << T_SS7_MPLEV)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->mplev)) p) = *(ops->mplev);
			p += _T_ALIGN_SIZEOF(*ops->mplev);
		}
		if (ops->debug) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->debug));
			oh->level = T_SS7_SS7;
			oh->name = T_SS7_DEBUG;
			oh->status = (ops->flags & (1 << T_SS7_DEBUG)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->debug)) p) = *(ops->debug);
			p += _T_ALIGN_SIZEOF(*ops->debug);
		}
		if (ops->cluster) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->cluster));
			oh->level = T_SS7_SS7;
			oh->name = T_SS7_CLUSTER;
			oh->status = (ops->flags & (1 << T_SS7_CLUSTER)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->cluster)) p) = *(ops->cluster);
			p += _T_ALIGN_SIZEOF(*ops->cluster);
		}
		if (ops->sls) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->sls));
			oh->level = T_SS7_SS7;
			oh->name = T_SS7_SEQ_CTRL;
			oh->status = (ops->flags & (1 << T_SS7_SEQ_CTRL)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->sls)) p) = *(ops->sls);
			p += _T_ALIGN_SIZEOF(*ops->sls);
		}
		if (ops->mp) {
			oh = ((typeof(oh)) p)++;
			oh->len = hlen + sizeof(*(ops->mp));
			oh->level = T_SS7_SS7;
			oh->name = T_SS7_PRIORITY;
			oh->status = (ops->flags & (1 << T_SS7_PRIORITY)) ? T_SUCCESS : T_FAILURE;
			*((typeof(ops->mp)) p) = *(ops->mp);
			p += _T_ALIGN_SIZEOF(*ops->mp);
		}
	}
}
static int
mtp_parse_opts(struct mt *mt, mtp_opts_t * ops, unsigned char *op, size_t len)
{
	struct t_opthdr *oh;

	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
		switch (oh->level) {
		case T_SS7_SS7:
			switch (oh->name) {
			case T_SS7_PVAR:
				ops->pvar = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_SS7_PVAR);
				continue;
			case T_SS7_MPLEV:
				ops->mplev = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_SS7_MPLEV);
				continue;
			case T_SS7_DEBUG:
				ops->debug = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_SS7_DEBUG);
				continue;
			case T_SS7_CLUSTER:
				ops->cluster = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_SS7_CLUSTER);
				continue;
			case T_SS7_SEQ_CTRL:
				ops->sls = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_SS7_SEQ_CTRL);
				continue;
			case T_SS7_PRIORITY:
				ops->mp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= (1 << T_SS7_PRIORITY);
				continue;
			}
		}
	}
	if (oh)
		return (TBADOPT);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPTIONS handling
 *
 *  =========================================================================
 */
static int
mtp_opt_check(struct mt *mt, mtp_opts_t * ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->pvar)
			ops->flags |= (1 << T_SS7_PVAR);
		if (ops->mplev)
			ops->flags |= (1 << T_SS7_MPLEV);
		if (ops->debug)
			ops->flags |= (1 << T_SS7_DEBUG);
		if (ops->cluster)
			ops->flags |= (1 << T_SS7_CLUSTER);
		if (ops->sls)
			ops->flags |= (1 << T_SS7_SEQ_CTRL);
		if (ops->mp)
			ops->flags |= (1 << T_SS7_PRIORITY);
	}
	return (0);
}
static int
mtp_opt_default(struct mt *mt, mtp_opts_t * ops)
{
	if (ops) {
		int flags = ops->flags;

		ops->flags = 0;
		if (!flags || ops->pvar) {
			ops->pvar = &mtp_opt_defaults.pvar;
			ops->flags |= (1 << T_SS7_PVAR);
		}
		if (!flags || ops->mplev) {
			ops->mplev = &mtp_opt_defaults.mplev;
			ops->flags |= (1 << T_SS7_MPLEV);
		}
		if (!flags || ops->debug) {
			ops->debug = &mtp_opt_defaults.debug;
			ops->flags |= (1 << T_SS7_DEBUG);
		}
		if (!flags || ops->cluster) {
			ops->cluster = &mtp_opt_defaults.cluster;
			ops->flags |= (1 << T_SS7_CLUSTER);
		}
		if (!flags || ops->sls) {
			ops->sls = &mtp_opt_defaults.sls;
			ops->flags |= (1 << T_SS7_SEQ_CTRL);
		}
		if (!flags || ops->mp) {
			ops->mp = &mtp_opt_defaults.mp;
			ops->flags |= (1 << T_SS7_PRIORITY);
		}
		return (0);
	}
	swerr();
	return (-EFAULT);
}
static int
mtp_opt_current(struct mt *mt, mtp_opts_t * ops)
{
	int flags = ops->flags;

	ops->flags = 0;
	if (!flags || ops->pvar) {
		ops->pvar = &mt->options.pvar;
		ops->flags |= (1 << T_SS7_PVAR);
	}
	if (!flags || ops->mplev) {
		ops->mplev = &mt->options.mplev;
		ops->flags |= (1 << T_SS7_MPLEV);
	}
	if (!flags || ops->debug) {
		ops->debug = &mt->options.debug;
		ops->flags |= (1 << T_SS7_DEBUG);
	}
	if (!flags || ops->cluster) {
		ops->cluster = &mt->options.cluster;
		ops->flags |= (1 << T_SS7_CLUSTER);
	}
	if (!flags || ops->sls) {
		ops->sls = &mt->options.sls;
		ops->flags |= (1 << T_SS7_SEQ_CTRL);
	}
	if (!flags || ops->mp) {
		ops->mp = &mt->options.mp;
		ops->flags |= (1 << T_SS7_PRIORITY);
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
state_name(long state)
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
	case TS_NOSTATES:
		return ("TS_NOSTATES");
	default:
		return ("(unknown)");
	}
}
#endif
static void
mtp_set_state(struct mt *mt, long state)
{
	printd(("%s: %p: %s <- %s\n", DRV_NAME, mt, state_name(state), state_name(mt->state)));
	mt->state = state;
}
static long
mtp_get_state(struct mt *mt)
{
	return (mt->state);
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
static int
mtp_check_src(struct mt *mt, mtp_addr_t * src)
{
	struct sl *sl;

	for (sl = sl_list; sl; sl = sl->next)
		if (sl->lk.lk->ni == src->ni && sl->sp.loc == src->pc)
			goto check;
	goto noaddr;
      check:
	if (sl->users[src->si & 0xff])
		goto addrbusy;
	return (0);
      noaddr:
	printd(("%s: %p: ERROR: Couldn't allocate source address\n", DRV_NAME, mt));
	return (TNOADDR);
      addrbusy:
	printd(("%s: %p: ERROR: Source address in use\n", DRV_NAME, mt));
	return (TADDRBUSY);
}

/*
 *  CHECK DST (CONN) ADDRESS
 *  -----------------------------------
 *  Check the destination (connect) address.  This may be a local, adjacent or
 *  remote address and the connection must be for a destination point code and
 *  SI value which is not already connected.
 */
static int
mtp_check_dst(struct mt *mt, mtp_addr_t * dst)
{
	struct sl *sl;

	if (!(sl = mt->sl))
		goto noaddr;
	if (sl->adj != dst->pc)
		goto noaddr;
	if (!sl->users[dst->si & 0xff])
		goto noaddr;
	if (sl->users[dst->si & 0xff] != mt)
		goto addrbusy;
	return (0);
      noaddr:
	printd(("%s: %p: ERROR: Couldn't allocate destination address\n", DRV_NAME, mt));
	return (TNOADDR);
      addrbusy:
	printd(("%s: %p: ERROR: Destination address in use\n", DRV_NAME, mt));
	return (TADDRBUSY);
}

/*
 *  MTP BIND
 *  -------------------------------------------------------------------------
 *  Add the MTP user to the local signalling point hashes if T_CLTS.
 */
static int
mtp_bind(struct mt *mt, mtp_addr_t * src)
{
	struct sl *sl;

	for (sl = sl_list; sl; sl = sl->next)
		if (sl->ni == src->ni && sl->loc == src->pc)
			break;
	if (sl) {
		sl->users[src->si & 0xff] = mt;
		mt->src = *src;
		mt->sl = sl;
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MTP UNBIND
 *  -------------------------------------------------------------------------
 *  Remove the MTP user from the local signalling point hashes if T_CLTS.
 */
static int
mtp_unbind(struct mt *mt)
{
	struct sl *sl;

	if ((sl = mt->sl)) {
		sl->users[mt->src.mtp_si & 0xff] = NULL;
		mt->sl = NULL;
		bzero(&mt->src, sizeof(mt->src));
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MTP CONNECT
 *  -------------------------------------------------------------------------
 *  Add the MTP user to the local signalling point service hashes.
 */
static int
mtp_connect(struct mt *mt, mtp_addr_t * dst)
{
	mt->dst = *dst;
	return (0);
}

/*
 *  MTP DISCONNECT
 *  -------------------------------------------------------------------------
 *  Remove the MTP user from the local signalling point service hashes.
 */
static int
mtp_disconnect(struct mt *mt)
{
	bzero(&mt->dst, sizeof(mt->dst));
	return (0);
}

/*
 *  MTP SEND MSG
 *  -------------------------------------------------------------------------
 */
static int
tp_send_msg(struct mt *mt, mtp_opts_t * opt, mtp_addr_t * dst, mblk_t *dp)
{
	fixme(("Send message\n"));
	return (-EFAULT);
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
/*
 *  M_ERROR
 *  -----------------------------------
 */
static int
m_error(queue_t *q, int error, mblk_t *sg)
{
	struct mt *mt = MT_PRIV(q);
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = error < 0 ? -error : error;
		*(mp->b_wptr)++ = error < 0 ? -error : error;
		mtp_set_state(mt, TS_NOSTATES);
		freemsg(msg);
		printd(("%s; %p: <- M_ERROR\n", DRV_NAME, mt));
		putnext(mt->rq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

static int t_error_ack(queue_t *q, ulong prim, long error);

/*
 *  T_INFO_ACK
 *  -----------------------------------
 */
static int
t_info_ack(queue_t *q)
{
	struct mt *mt = MT_PRIV(q);
	mblk_t *mp;
	struct T_info_ack *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = 272;
		p->ETSDU_size = T_INVALID;
		p->CDATA_size = T_INVALID;
		p->DDATA_size = T_INVALID;
		p->ADDR_size = sizeof(mtp_addr_t);
		p->OPT_size = T_INFINITE;
		p->SERV_type = T_CLTS;
		p->CURRENT_state = mt->state;
		p->PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
		printd(("%s: %p: <- T_INFO_ACK\n", DRV_NAME, mt));
		putnext(mt->rq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mt));
	return (-ENOBUFS);
}

/*
 *  T_BIND_ACK
 *  -----------------------------------
 */
static int
t_bind_ack(queue_t *q, mtp_addr_t * add)
{
	struct mt *mt = MT_PRIV(q);
	int err;
	mblk_t *mp;
	struct T_bind_ack *p;
	size_t add_len = add ? sizeof(*add) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_INFO_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		if (add_len) {
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		if ((err = mtp_bind(mt, add)))
			goto free_error;
		mtp_set_state(mt, TS_IDLE);
		printd(("%s: %p: <- T_BIND_ACK\n", DRV_NAME, mt));
		putnext(mt->rq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mt));
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(q, T_BIND_REQ, err);
}

/*
 *  T_ERROR_ACK
 *  -----------------------------------
 */
static int
t_error_ack(queue_t *q, ulong prim, long error)
{
	struct mt *mt = MT_PRIV(q);
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
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = error < 0 ? TSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		if (error != TOUTSTATE) {
			switch (mtp_get_state(mt)) {
#ifdef TS_WACK_OPTREQ
			case TS_WACK_OPTREQ:
				mtp_set_state(mt, TS_IDLE);
				break;
#endif
			case TS_WACK_UREQ:
				mtp_set_state(mt, TS_IDLE);
				break;
			case TS_WACK_BREQ:
				mtp_set_state(mt, TS_UNBND);
				break;
			default:
				/* 
				   Note: if we are not in a WACK state we simply do not change
				   state.  This occurs normally when we are responding to a
				   T_OPTMGMT_REQ in other than TS_IDLE state. */
				break;
			}
		}
		printd(("%s: %p: <- T_ERROR_ACK\n", DRV_NAME, mt));
		putnext(mt->rq, mp);
		/* 
		   Retruning -EPROTO here will make sure that the old state is restored correctly.
		   If we return QR_DONE, then the state will never be restored. */
		if (error < 0)
			return (error);
		return (-EPROTO);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mt));
	return (-ENOBUFS);
}

/*
 *  T_OK_ACK
 *  -----------------------------------
 */
static int
t_ok_ack(queue_t *q, ulong prim)
{
	int err = -EFAULT;
	struct mt *mt = MT_PRIV(q);
	mblk_t *mp;
	struct T_ok_ack *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		switch (mtp_get_state(mt)) {
#if 0
		case TS_WACK_CREQ:
			if ((err = mtp_connect(mt, &mt->dst)))
				goto free_error;
			mtp_set_state(mt, TS_WCON_CREQ);
			if ((err = t_conn_con(mt, &mt->dst)))
				goto free_error;
			mtp_set_state(mt, TS_DATA_XFER);
			break;
#endif
		case TS_WACK_UREQ:
			if ((err = mtp_unbind(mt)))
				goto free_error;
			mtp_set_state(mt, TS_UNBND);
			break;
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ9:
			if ((err = mtp_disconnect(mt)))
				goto free_error;
			mtp_set_state(mt, TS_IDLE);
			break;
		default:
			/* 
			   Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a T_OPTMGMT_REQ in other than
			   TS_IDLE state. */
			break;
		}
		printd(("%s: %p: <- T_OK_ACK\n", DRV_NAME, mt));
		putnext(mt->rq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mt));
	return (-ENOBUFS);
      free_error:
	freemsg(mp);
	return t_error_ack(q, prim, err);
}

/*
 *  T_UNITDATA_IND
 *  -----------------------------------
 */
static int
t_unitdata_ind(queue_t *q, mtp_addr_t * src, mtp_opts_t * opt, mblk_t *dp)
{
	struct mt *mt = MT_PRIV(q);
	mblk_t *mp;
	struct T_unitdata_ind *p;
	size_t src_len = src ? sizeof(*src) : 0;
	size_t opt_len = opt ? mtp_opts_size(mt, opt) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + src_len + opt_len, BPRI_MED))) {
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
			mtp_build_opts(mt, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- T_UNITDATA_IND\n", DRV_NAME, mt));
		putnext(mt->rq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mt));
	return (-ENOBUFS);
}

/*
 *  T_UDERROR_IND
 *  -----------------------------------
 *  This primitive indicatest to the MTP user that a message with the
 *  specified destination address and options produced an error.
 */
static int
t_uderror_ind(queue_t *q, struct mt *mt, mtp_addr_t * dst, mtp_opts_t * opt, mblk_t *dp, int etype)
{
	mblk_t *mp;
	struct T_uderror_ind *p;
	size_t dst_len = dst ? sizeof(*dst) : 0;
	size_t opt_len = opt ? mtp_opts_size(mt, opt) : 0;

	if (canputnext(mt->rq)) {
		if ((mp = mi_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
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
				mtp_build_opts(mt, opt, mp->b_wptr);
				mp->b_wptr += opt_len;
			}
			mp->b_cont = dp;
			printd(("%s: %p: <- T_UDERROR_IND\n", DRV_NAME, mt));
			putnext(mt->rq, mp);
			return (QR_DONE);
		}
		ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mt));
		return (-ENOBUFS);
	}
	ptrace(("%s: %p: ERROR: Flow controlled\n", DRV_NAME, mt));
	return (-EBUSY);
}

/*
 *  T_OPTMGMT_ACK
 *  -----------------------------------
 */
static int
t_optmgmt_ack(queue_t *q, ulong flags, mtp_opts_t * opt)
{
	struct mt *mt = MT_PRIV(q);
	mblk_t *mp;
	struct T_optmgmt_ack *p;
	size_t opt_len = opt ? mtp_opts_size(mt, opt) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		if (opt_len) {
			mtp_build_opts(mt, opt, mp->b_wptr);
			mp->b_wptr += opt_len;
		}
#ifdef TS_WACK_OPTREQ
		if (mtp_get_state(mt) == TS_WACK_OPTREQ)
			mtp_set_state(mt, TS_IDLE);
#endif
		printd(("%s: %p: <- T_OPTMGMT_ACK\n", DRV_NAME, mt));
		putnext(mt->rq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mt));
	return (-ENOBUFS);
}

#ifdef T_ADDR_ACK
/*
 *  T_ADDR_ACK
 *  -----------------------------------
 */
static int
t_addr_ack(queue_t *q, mtp_addr_t * loc, mtp_addr_t * rem)
{
	struct mt *mt = MT_PRIV(q);
	mblk_t *mp;
	struct T_addr_ack *p;
	size_t loc_len = loc ? sizeof(*loc) : 0;
	size_t rem_len = rem ? sizeof(*rem) : 0;

	if ((mp = mi_allocb(q, sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
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
		printd(("%s: %p: <- T_ADDR_ACK\n", DRV_NAME, mt));
		putnext(mt->rq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mt));
	return (-ENOBUFS);
}
#endif
#ifdef T_CAPABILITY_ACK
/*
 *  T_CAPABILITY_ACK
 *  -----------------------------------
 */
static int
t_capability_ack(queue_t *q, ulong caps)
{
	struct mt *mt = MT_PRIV(q);
	mblk_t *mp;
	struct T_capability_ack *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = TC1_INFO;
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (ulong) mt->rq : 0;
		if (caps & TC1_INFO) {
			p->INFO_ack->PRIM_type = T_INFO_ACK;
			p->INFO_ack->TSDU_size = 272;
			p->INFO_ack->ETSDU_size = T_INVALID;
			p->INFO_ack->CDATA_size = T_INVALID;
			p->INFO_ack->DDATA_size = T_INVALID;
			p->INFO_ack->ADDR_size = sizeof(mtp_addr_t);
			p->INFO_ack->OPT_size = T_INFINITE;
			p->INFO_ack->SERV_type = T_CLTS;
			p->INFO_ack->CURRENT_state = mt->state;
			p->INFO_ack->PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
		} else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		printd(("%s: %p: <- T_CAPABILITY_ACK\n", DRV_NAME, mt));
		putnext(mt->rq, mp);
		return (QR_DONE);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, mt));
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
static int
sl_pdu_req(queue_t *q, mblk_t *dp)
{
	struct sl *sl = SL_PRIV(q);

	if (canputnext(sl->wq)) {
		mblk_t *mp;
		sl_pdu_req_t *p;

		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((typeof(p)) mp->b_wptr)++;
			p->sl_primitive = SL_PDU_REQ;
			p->sl_mp = 0;
			mp->b_cont = dp;
			printd(("%s: %p: SL_PDU_REQ [%d] ->\n", DRV_NAME, sl, msgdsize(dp)));
			putnext(sl->wq, mp);
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
static int
sl_emergency_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_emergency_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_EMERGENCY_REQ;
		printd(("%s: %p: SL_EMERGENCY_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
static int
sl_emergency_ceases_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_emergency_ceases_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
		printd(("%s: %p: SL_EMERGENCY_CEASES_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 */
static int
sl_start_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_start_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_START_REQ;
		printd(("%s: %p: SL_START_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 */
static int
sl_stop_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_stop_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_STOP_REQ;
		printd(("%s: %p: SL_STOP_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
static int
sl_retrieve_bsnt_req(queue_t *q, struct sl *sl)
{
	mblk_t *mp;
	sl_retrieve_bsnt_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVE_BSNT_REQ;
		printd(("%s: %p: SL_RETRIEVE_BSNT_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
static int
sl_retrieval_request_and_fsnc_req(queue_t *q, ulong fsnc)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_retrieval_req_and_fsnc_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RETRIEVAL_REQUEST_AND_FSNC_REQ;
		p->sl_fsnc = fsnc;
		printd(("%s: %p: SL_RETRIEVAL_REQUEST_AND_FSNC_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
static int
sl_resume_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_resume_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_RESUME_REQ;
		printd(("%s: %p: SL_RESUME_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
static int
sl_clear_buffers_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_clear_buffers_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
		printd(("%s: %p: SL_CLEAR_BUFFERS_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
static int
sl_clear_rtb_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_clear_rtb_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_CLEAR_RTB_REQ;
		printd(("%s: %p: SL_CLEAR_RTB_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
static int
sl_local_processor_outage_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_local_proc_outage_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		printd(("%s: %p: SL_LOCAL_PROCESSOR_OUTAGE_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
static int
sl_congestion_discard_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_cong_discard_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
		printd(("%s: %p: SL_CONGESTION_DISCARD_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
static int
sl_congestion_accept_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_cong_accept_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		printd(("%s: %p: SL_CONGESTION_ACCEPT_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
static int
sl_no_congestion_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_no_cong_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_NO_CONGESTION_REQ;
		printd(("%s: %p: SL_NO_CONGESTION_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
static int
sl_power_on_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_power_on_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_POWER_ON_REQ;
		printd(("%s: %p: SL_POWER_ON_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

#if 0
/*
 *  SL_OPTMGMT_REQ
 *  -----------------------------------
 */
static int
sl_optmgmt_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_optmgmt_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_OPTMGMT_REQ;
		printd(("%s: %p: SL_OPTMGMT_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  SL_NOTIFY_REQ
 *  -----------------------------------
 */
static int
sl_notify_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	sl_notify_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->sl_primitive = SL_NOTIFY_REQ;
		printd(("%s: %p: SL_NOTIFY_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}
#endif
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
static int
lmi_info_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	lmi_info_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_INFO_REQ;
		printd(("%s: %p: LMI_INFO_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 */
static int
lmi_attach_req(queue_t *q, caddr_t ppa_ptr, size_t ppa_len)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	lmi_attach_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ATTACH_REQ;
		if (ppa_ptr && ppa_len) {
			bcopy(ppa_ptr, mp->b_wptr, ppa_len);
			mp->b_wptr += ppa_len;
		}
		printd(("%s: %p: LMI_ATTACH_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 */
static int
lmi_detach_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	lmi_detach_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_DETACH_REQ;
		printd(("%s: %p: LMI_DETACH_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 */
static int
lmi_enable_req(queue_t *q, caddr_t dst_ptr, size_t dst_len)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	lmi_enable_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_ENABLE_REQ;
		if (dst_ptr && dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		printd(("%s: %p: LMI_ENABLE_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 */
static int
lmi_disable_req(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	lmi_disable_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_DISABLE_REQ;
		printd(("%s: %p: LMI_DISABLE_REQ ->\n", DRV_NAME, sl));
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
	}
	ptrace(("%s: %p: ERROR: No buffers\n", DRV_NAME, sl));
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
static int
lmi_optmgmt_req(queue_t *q, ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *mp;
	lmi_optmgmt_req_t *p;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
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
		putnext(sl->wq, mp);
		return (QR_ABSORBED);
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
struct mtp_msg {
	queue_t *eq;			/* queue to write errors to */
	queue_t *xq;			/* queue to write results to */
	struct mt *mtp;			/* MTP-User to which this message belongs */
	struct sl *sl;			/* Signalling Link to which thes message belongs */
	unsigned long timestamp;	/* jiffie clock timestamp */
	uint pvar;			/* protocol variant */
	uint popt;			/* protocol variant */
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
static void
mtp_enc_com(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = msg->arg.fsnl & 0x7f;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = (msg->slc & 0x0f) | (msg->arg.fsnl << 4);
		*mp->b_wptr++ = (msg->arg.fsnl >> 4) & 0x7;
		break;
	}
	return;
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
static void
mtp_enc_cbm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = msg->arg.cbc;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = (msg->slc & 0x0f) | (msg->arg.cbc << 4);
		*mp->b_wptr++ = (msg->arg.cbc >> 4) & 0x0f;
		break;
	}
	return;
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
static void
mtp_enc_slm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = msg->slc & 0x0f;
		break;
	}
	return;
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
static void
mtp_enc_tfc(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ITUT:
	case SS7_PVAR_ETSI:
		*mp->b_wptr++ = msg->dest;
		*mp->b_wptr++ = ((msg->dest >> 8) & 0x3f) | (msg->arg.stat << 6);
		break;
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		*mp->b_wptr++ = msg->dest;
		*mp->b_wptr++ = (msg->dest >> 8);
		*mp->b_wptr++ = (msg->dest >> 16);
		*mp->b_wptr++ = (msg->arg.stat & 0x3);
		break;
	}
	return;
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
static void
mtp_enc_tfm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ITUT:
	case SS7_PVAR_ETSI:
		*mp->b_wptr++ = msg->dest;
		*mp->b_wptr++ = (msg->dest >> 8) & 0x3f;
		break;
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		*mp->b_wptr++ = msg->dest;
		*mp->b_wptr++ = (msg->dest >> 8);
		*mp->b_wptr++ = (msg->dest >> 16);
		break;
	}
	return;
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
static void
mtp_enc_dlc(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = msg->arg.sdli;
		*mp->b_wptr++ = (msg->arg.sdli >> 8) & 0x0f;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = (msg->slc & 0x0f) | (msg->arg.sdli << 4);
		*mp->b_wptr++ = msg->arg.sdli >> 4;
		*mp->b_wptr++ = (msg->arg.sdli >> 12) & 0x03;
		break;
	}
	return;
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
static void
mtp_enc_upm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = msg->dest;
		*mp->b_wptr++ = (msg->dest >> 8) & 0x3f;
		*mp->b_wptr++ = msg->arg.upi & 0x0f;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = msg->dest;
		*mp->b_wptr++ = msg->dest >> 8;
		*mp->b_wptr++ = msg->dest >> 16;
		*mp->b_wptr++ = msg->arg.upi & 0x0f;
		break;
	}
	return;
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
static void
mtp_enc_sltm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		*mp->b_wptr++ = msg->dlen << 4;
		bcopy(mp->b_wptr, msg->data, msg->dlen);
		mp->b_wptr += msg->dlen;
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = (msg->slc & 0x0f) | (msg->dlen << 4);
		bcopy(mp->b_wptr, msg->data, msg->dlen);
		mp->b_wptr += msg->dlen;
		break;
	}
	return;
}

/*
 *  Encode User Part
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  All Formats:
 *  ------------------------+
 *   ...   User Part Data   |
 *  ------------------------+
 */
static void
mtp_enc_user(mblk_t *mp, struct mtp_msg *msg)
{
	bcopy(mp->b_wptr, msg->data, msg->dlen);
	mp->b_wptr += msg->dlen;
	return;
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
mtp_enc_sio(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_JTTC:
		mp->b_wptr[-1] = msg->mp << 6;	/* put message priority in header */
		*mp->b_wptr++ = (msg->si & 0x0f) | ((msg->ni & 0x3) << 6);
		break;
	case SS7_PVAR_ANSI:
		*mp->b_wptr++ = (msg->si & 0x0f) | ((msg->mp & 0x3) << 4) | ((msg->ni & 0x3) << 6);
		break;
	default:
		if (msg->popt & SS7_POPT_MPLEV)
			*mp->b_wptr++ =
			    (msg->si & 0x0f) | ((msg->mp & 0x3) << 4) | ((msg->ni & 0x3) << 6);
		else
			*mp->b_wptr++ = (msg->si & 0x0f) | ((msg->ni & 0x3) << 6);
		break;
	}
	return;
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
mtp_enc_rl(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ETSI:
	case SS7_PVAR_ITUT:
		*mp->b_wptr++ = msg->dpc;
		*mp->b_wptr++ = ((msg->dpc >> 8) & 0x3f) | (msg->opc << 6);
		*mp->b_wptr++ = (msg->opc >> 2);
		*mp->b_wptr++ = ((msg->opc >> 10) & 0x0f) | (msg->sls << 4);
		break;
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		*mp->b_wptr++ = msg->dpc;
		*mp->b_wptr++ = msg->dpc >> 8;
		*mp->b_wptr++ = msg->dpc >> 16;
		*mp->b_wptr++ = msg->opc;
		*mp->b_wptr++ = msg->opc >> 8;
		*mp->b_wptr++ = msg->opc >> 16;
		*mp->b_wptr++ = msg->sls;
		break;
	}
	return;
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
mtp_enc_sif(mblk_t *mp, struct mtp_msg *msg)
{
	unsigned char tag;

	switch (msg->si) {
	default:		/* USER */
		return mtp_enc_user(mp, msg);
	case 0:		/* SNMM */
		tag = ((msg->h0 & 0x0f) << 4) | (msg->h1 & 0x0f);
		*mp->b_wptr++ = (msg->h0 & 0x0f) | ((msg->h1 & 0x0f) << 4);
		switch (tag) {
		case 0x11:	/* coo */
		case 0x12:	/* coa */
			return mtp_enc_com(mp, msg);
		case 0x15:	/* cbd */
		case 0x16:	/* cba */
			return mtp_enc_cbm(mp, msg);
		case 0x71:	/* tra */
		case 0x72:	/* trw */
		case 0x31:	/* rct */
			return (0);
		case 0x32:	/* tfc */
			return mtp_enc_tfc(mp, msg);
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
			return mtp_enc_tfm(mp, msg);
		case 0x81:	/* dlc */
			return mtp_enc_dlc(mp, msg);
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
			return mtp_enc_slm(mp, msg);
		case 0xa1:	/* upu */
		case 0xa2:	/* upa *//* ansi91 only */
		case 0xa3:	/* upt *//* ansi91 only */
			return mtp_enc_upm(mp, msg);
		}
		break;
	case 1:		/* SNTM */
	case 2:		/* SNSM */
		tag = ((msg->h0 & 0x0f) << 4) | (msg->h1 & 0x0f);
		*mp->b_wptr++ = (msg->h0 & 0x0f) | ((msg->h1 & 0x0f) << 4);
		switch (tag) {
		case 0x11:	/* sltm */
		case 0x12:	/* slta */
			return mtp_enc_sltm(mp, msg);
		}
		break;
	}
	swerr();
	return (-EFAULT);
}

#define max_msg_size (6 + 1 + 7 + 1 + 3 + 1)
static mblk_t *
mtp_enc_msg(queue_t *q, struct mtp_msg *msg)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, max_msg_size + msg->dlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		bzero(mp->b_rptr, 6);	/* zero header */
		mp->b_rptr += 6;	/* reserve header room for Level 2 */
		mp->b_wptr = mp->b_rptr;
		mtp_enc_sio(mp, msg);
		mtp_enc_rl(mp, msg);
		mtp_enc_sif(mp, msg);
	}
	return (mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Build message functions
 *
 *  -------------------------------------------------------------------------
 *  These are partial build functions: they just "finish off" the pertinent
 *  message parameters.  All routing label items and arguments, and especially
 *  protocol variant and options, should be filled out by calling mtp_build_rl
 *  first.  After these function is called, mtp_enc_msg will generate the
 *  actual message.  The message can then be routed.
 */
static inline void
mtp_build_rl(struct mtp_msg *msg, uint pvar, uint ni, uint mp, uint si, uint32_t dpc, uint32_t opc,
	     uint sls)
{
	msg->pvar = pvar;
	msg->ni = ni;
	msg->mp = mp;
	msg->si = si;
	msg->dpc = dpc;
	msg->opc = opc;
	msg->sls = sls;
}
static inline void
mtp_build_slc(struct mtp_msg *msg, uint slc)
{
	if ((msg->pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
		msg->sls = slc;
	else
		msg->slc = slc;
}
static inline mblk_t *
mtp_build_coo(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc, uint fsnl)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 1;
	msg->h1 = 1;
	msg->arg.fsnl = fsnl;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_coa(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc, uint fsnl)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 1;
	msg->h1 = 2;
	msg->arg.fsnl = fsnl;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_cbd(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc, uint cbc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 1;
	msg->h1 = 5;
	msg->arg.cbc = cbc;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_cba(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc, uint cbc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 1;
	msg->h1 = 6;
	msg->arg.cbc = cbc;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_eco(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 2;
	msg->h1 = 1;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_eca(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_slc(msg, slc);
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 2;
	msg->h1 = 2;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_rct(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint mp)
{
	mtp_build_rl(msg, pvar, ni, mp, 0, dpc, opc, sls);
	msg->h0 = 3;
	msg->h1 = 1;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_tfc(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest, uint stat)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 3;
	msg->h1 = 2;
	msg->dest = dest;
	msg->arg.stat = stat;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_tfp(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 4;
	msg->h1 = 1;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_tcp(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 4;
	msg->h1 = 2;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_tfr(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 4;
	msg->h1 = 3;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_tcr(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 4;
	msg->h1 = 4;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_tfa(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 4;
	msg->h1 = 5;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_tca(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 4;
	msg->h1 = 6;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_rst(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 5;
	msg->h1 = 1;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_rsr(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 5;
	msg->h1 = 2;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_rcp(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 5;
	msg->h1 = 3;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_rcr(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 5;
	msg->h1 = 4;
	msg->dest = dest;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_lin(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 6;
	msg->h1 = 1;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_lun(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 6;
	msg->h1 = 2;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_lia(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 6;
	msg->h1 = 3;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_lua(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 6;
	msg->h1 = 4;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_lid(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 6;
	msg->h1 = 5;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_lfu(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 6;
	msg->h1 = 6;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_llt(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 6;
	msg->h1 = 7;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_lrt(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 6;
	msg->h1 = 8;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_tra(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 7;
	msg->h1 = 1;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_trw(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 7;
	msg->h1 = 2;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_dlc(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc, uint sdli)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 8;
	msg->h1 = 1;
	msg->arg.sdli = sdli;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_css(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 8;
	msg->h1 = 2;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_cns(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 8;
	msg->h1 = 3;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_cnp(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint slc)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 8;
	msg->h1 = 4;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_upu(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest, uint upi)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 10;
	msg->h1 = 1;
	msg->dest = dest;
	msg->arg.upi = upi;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_upa(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest, uint upi)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 10;
	msg->h1 = 2;
	msg->dest = dest;
	msg->arg.upi = upi;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_upt(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	      uint32_t opc, uint sls, uint32_t dest, uint upi)
{
	mtp_build_rl(msg, pvar, ni, 3, 0, dpc, opc, sls);
	msg->h0 = 10;
	msg->h1 = 3;
	msg->dest = dest;
	msg->arg.upi = upi;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_sltm(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	       uint32_t opc, uint sls, uint slc, unsigned char *data, size_t dlen)
{
	mtp_build_rl(msg, pvar, ni, 3, 1, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 1;
	msg->h1 = 1;
	msg->data = data;
	msg->dlen = dlen;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_slta(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	       uint32_t opc, uint sls, uint slc, unsigned char *data, size_t dlen)
{
	mtp_build_rl(msg, pvar, ni, 3, 1, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 1;
	msg->h1 = 2;
	msg->data = data;
	msg->dlen = dlen;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_ssltm(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
		uint32_t opc, uint sls, uint slc, unsigned char *data, size_t dlen)
{
	mtp_build_rl(msg, pvar, ni, 3, 2, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 1;
	msg->h1 = 1;
	msg->data = data;
	msg->dlen = dlen;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_sslta(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
		uint32_t opc, uint sls, uint slc, unsigned char *data, size_t dlen)
{
	mtp_build_rl(msg, pvar, ni, 3, 2, dpc, opc, sls);
	mtp_build_slc(msg, slc);
	msg->h0 = 1;
	msg->h1 = 2;
	msg->data = data;
	msg->dlen = dlen;
	return mtp_enc_msg(q, msg);
}
static inline mblk_t *
mtp_build_user(queue_t *q, struct mtp_msg *msg, uint pvar, uint ni, uint32_t dpc,
	       uint32_t opc, uint sls, uint mp, uint si, unsigned char *data, size_t dlen)
{
	mtp_build_rl(msg, pvar, ni, mp, si, dpc, opc, sls);
	msg->data = data;
	msg->dlen = dlen;
	return mtp_enc_msg(q, msg);
}

/*
 *  Send message functions
 *  -------------------------------------------------------------------------
 */
static inline int
mtp_send_route(queue_t *q, mblk_t *bp, struct mtp_msg *msg, mblk_t *mp)
{
	struct sl *sl;

	for (sl = sl_list; sl; sl = sl->next)
		if (sl->adj == msg->dpc)
			break;
	if (sl) {
		if (canputnext(sl->wq)) {
			if (bp)
				freeb(bp);
			putnext(sl->wq, mp);
			return (0);
		}
		return (-EBUSY);
	}
	mi_strlog(q, 0, SL_ERROR, "Discarding message for DPC = %x",, msg->dpc);
	goto error;
      error:
	if (bp)
		freeb(bp);
	freemsg(mp);
	return (0);
}
static int
mtp_send_coo(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc, uint fsnl)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_coo(q, &msg, pvar, ni, dpc, opc, sls, slc, fsnl)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_coa(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc, uint fsnl)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_coa(q, &msg, pvar, ni, dpc, opc, sls, slc, fsnl)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_cbd(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc, uint cbc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_cbd(q, &msg, pvar, ni, dpc, opc, sls, slc, cbc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_cba(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc, uint cbc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_cba(q, &msg, pvar, ni, dpc, opc, sls, slc, cbc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_eco(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_eco(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_eca(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_eca(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_rct(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint mp)
{
	mblk_t *bp;
	struct mtp_msg msg = { 0, };

	if ((bp = mtp_build_rct(q, &msg, pvar, ni, dpc, opc, sls, mp)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_tfc(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest, uint stat)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_tfc(q, &msg, pvar, ni, dpc, opc, sls, dest, stat)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_tfp(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_tfp(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_tcp(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_tcp(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_tfr(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_tfr(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_tcr(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_tcr(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_tfa(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_tfa(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_tca(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_tca(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_rst(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_rst(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_rsr(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_rsr(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_rcp(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_rcp(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_rcr(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_rcr(q, &msg, pvar, ni, dpc, opc, sls, dest)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_lin(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_lin(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_lun(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_lun(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_lia(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_lia(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_lua(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_lua(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_lid(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_lid(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_lfu(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_lfu(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_llt(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_llt(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_lrt(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_lrt(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_tra(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_tra(q, &msg, pvar, ni, dpc, opc, sls)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_trw(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_trw(q, &msg, pvar, ni, dpc, opc, sls)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_dlc(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc, uint sdli)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_dlc(q, &msg, pvar, ni, dpc, opc, sls, slc, sdli)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_css(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_css(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_cns(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_cns(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_cnp(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint slc)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_cnp(q, &msg, pvar, ni, dpc, opc, sls, slc)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_upu(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest, uint upi)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_upu(q, &msg, pvar, ni, dpc, opc, sls, dest, upi)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_upa(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest, uint upi)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_upa(q, &msg, pvar, ni, dpc, opc, sls, dest, upi)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_upt(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	     uint32_t dest, uint upi)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_upt(q, &msg, pvar, ni, dpc, opc, sls, dest, upi)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_sltm(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	      uint slc, unsigned char *data, size_t dlen)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_sltm(q, &msg, pvar, ni, dpc, opc, sls, slc, data, dlen)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_slta(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	      uint slc, unsigned char *data, size_t dlen)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_slta(q, &msg, pvar, ni, dpc, opc, sls, slc, data, dlen)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_ssltm(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	       uint slc, unsigned char *data, size_t dlen)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_ssltm(q, &msg, pvar, ni, dpc, opc, sls, slc, data, dlen)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_sslta(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	       uint slc, unsigned char *data, size_t dlen)
{
	mblk_t *mp;
	struct mtp_msg msg = { 0, };

	if ((mp = mtp_build_sslta(q, &msg, pvar, ni, dpc, opc, sls, slc, data, dlen)))
		return mtp_send_route(q, bp, &msg, mp);
	return (-ENOBUFS);
}
static int
mtp_send_user(queue_t *q, mblk_t *bp, uint pvar, uint ni, uint32_t dpc, uint32_t opc, uint sls,
	      uint mp, uint si, unsigned char *data, size_t dlen)
{
	mblk_t *bp;
	struct mtp_msg msg = { 0, };

	if ((bp = mtp_build_user(q, &msg, pvar, ni, dpc, opc, sls, mp, si, data, dlen)))
		return mtp_send_route(q, bp, &msg, mp);
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
 *  MTP User procedures
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP-STATUS-IND
 *  -----------------------------------
 *  Indicate to an MTP User an MTP-STATUS-IND with respect to the concerned
 *  routesets.
 */
static inline int
mtp_status_ind(queue_t *q, struct mt *mtp, mtp_addr_t * dst, int status)
{
	return t_uderror_ind(q, mtp, dst, NULL, NULL, status);
}
static int
mtp_up_status_ind(queue_t *q, struct mt *mtp, uint32_t dest, uint user, uint status)
{
	mtp_addr_t dst = mtp->src;
	uint error;

	dst.mtp_pc = dest;
	dst.mtp_si = user;
	return mtp_status_ind(q, mtp, &dst, error);
}
static int
mtp_up_status_ind_all_local(queue_t *q, uint32_t dest, uint si, uint status)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	struct mt *mtp;

	if ((mtp = sl->users[si & 0xf]))
		if ((err = mtp_up_status_ind(q, mtp, dest, si, status)))
			return (err);
	return (0);
}
static int
mtp_cong_status_ind(queue_t *q, struct mt *mtp, uint32_t dest, uint status)
{
	mtp_addr_t dst = mtp->src;
	uint error = T_MTP_CONGESTED(status) | T_MTP_M_MEMBER;

	dst.mtp_pc = dest;
	return mtp_status_ind(q, mtp, &dst, error);
}
static int
mtp_cong_status_ind_all_local(queue_t *q, uint32_t dest, uint status)
{
	struct sl *sl = SL_PRIV(q);
	int i, err;
	struct mt *mtp;

	for (i = 0; i < 16; i++)
		if ((mtp = sl->users[i]))
			if ((err = mtp_cong_status_ind(q, mtp, dest, status)))
				return (err);
	return (0);
}

/*
 *  MTP-PAUSE-IND
 *  -----------------------------------
 */
static int
mtp_pause_ind(queue_t *q, struct mt *mtp, uint32_t dest)
{
	mtp_addr_t dst = mtp->src;
	uint error = T_MTP_PROHIBITED | T_MTP_M_MEMBER;

	dst.mtp_pc = dest;
	return mtp_status_ind(q, mtp, &dst, error);
}
static int
mtp_pause_ind_all_local(queue_t *q, uint32_t dest)
{
	struct sl *sl = SL_PRIV(q);
	int i, err;
	struct mt *mtp;

	for (i = 0; i < 16; i++)
		if ((mtp = sl->users[i]))
			if ((err = mtp_pause_ind(q, mtp, dest)))
				return (err);
	return (0);
}

/*
 *  MTP-RESUME-IND
 *  -----------------------------------
 */
static int
mtp_resume_ind(queue_t *q, struct mt *mtp, uint32_t dest)
{
	mtp_addr_t dst = mtp->src;
	uint error = T_MTP_AVAILABLE | T_MTP_M_MEMBER;

	dst.mtp_pc = dest;
	return mtp_status_ind(q, mtp, &dst, error);
}
static int
mtp_resume_ind_all_local(queue_t *q, uint32_t dest)
{
	struct sl *sl = SL_PRIV(q);
	int i, err;
	struct mt *mtp;

	for (i = 0; i < 16; i++)
		if ((mtp = sl->users[i]))
			if ((err = mtp_resume_ind(q, mtp, dest)))
				return (err);
	return (0);
}

/*
 *  MTP-TRANSFER-IND
 *  -----------------------------------
 */
static int
mtp_transfer_ind(queue_t *q, struct mt *mtp, struct mtp_msg *msg)
{
	if (mtp) {
	}
	return (QR_DONE);
}
static int
mtp_transfer_ind_local(queue_t *q, struct mtp_msg *msg)
{
	struct sl *sl = SL_PRIV(q);
	struct mt *mtp;

	if ((mtp = sl->users[msg->si & 0x0f]))
		return mtp_transfer_ind(q, mtp, msg);
	/* 
	   FIXME: should send unqeuipped user */
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timers
 *
 *  -------------------------------------------------------------------------
 */
enum {
	tcon,
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	t8,
	t9,				/* not used */
	t10,
	t11,
	t12,
	t13,
	t14,
	t15,
	t16,
	t17,
	t18,
	t19,
	t20,
	t21,
	t22,
	t23,
	t24,
	t1t,
	t2t,
	t1s
};

/*
 *  MTP timers
 *  -----------------------------------
 */
static inline void
sl_timer_stop(struct sl *sl, const uint t)
{
	switch (t) {
	case t1t:
		mi_timer_stop(sl->timers.t1t);
		if (sl->timers.t1t) {
			mi_strlog(sl->rq, STRLOGTE, "stopping t1t at %lu", jiffies);
			mi_timer_stop(sl->timers.t1t);
		}
		break;
	case t2t:
		if (sl->timers.t2t) {
			mi_strlog(sl->rq, STRLOGTE, "stopping t2t at %lu", jiffies);
			mi_timer_stop(sl->timers.t2t);
		}
		break;
	default:
		swerr();
		break;
	}
}
static inline void
sl_timer_start(struct sl *sl, const uint t)
{
	switch (t) {
	case t1t:
		if (sl->timers.t1t) {
			mi_strlog(sl->rq, STRLOGTE, "starting t1t at %lu", jiffies);
			mi_timer(sl->rq, sl->timers.t1t, sl->config.t1t);
		}
		break;
	case t2t:
		if (sl->timers.t2t) {
			mi_strlog(sl->rq, STRLOGTE, "starting t2t at %lu", jiffies);
			mi_timer(sl->rq, sl->timers.t2t, sl->config.t2t);
		}
		break;
	default:
		swerr();
		break;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timeout functions
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Receive message functions
 *
 *  -------------------------------------------------------------------------
 */
static int
sl_recv_coo(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("COO Unimplemented\n"));
	return (-EFAULT);
}
static int
sl_recv_coa(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("COA Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_cbd(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("CBD Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_cba(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("CBA Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_eco(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("ECO Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_eca(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("ECA Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_rct(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	/* 
	   13.9.6.  When a signalling route set congestino test message reaches its destination, it 
	   is discarded. */
	return (QR_DONE);
}
static int
sl_recv_tfc(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("TFC Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_tfp(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("TFP Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_tcp(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("TCP Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_tfr(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("TFR Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_tcr(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("TCR Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_tfa(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("TFA Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_tca(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("TCA Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_rst(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("RST Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_rsr(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("RSR Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_rcp(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("RCP Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_rcr(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("RCR Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_lin(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("LIN Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_lun(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("LUN Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_lia(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("LIA Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_lua(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("LUA Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_lid(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("LID Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_lfu(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("LFU Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_llt(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("LLT Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_lrt(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("LRT Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_tra(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("TRA Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_trw(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("TRW Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_dlc(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("DLC Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_css(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("CSS Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_cns(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("CNS Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_cnp(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("CNP Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_upu(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	fixme(("Handle UPU\n"));
	return (-EFAULT);
}
static int
sl_recv_upa(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("UPA Unimplemeneted\n"));
	return (-EFAULT);
}
static int
sl_recv_upt(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("UPT Unimplemeneted\n"));
	return (-EFAULT);
}

#define SLS_OUT_OF_SERVICE	    0
// #define SLS_IN_SERVICE 1
#define SLS_PENDING_ACTIVATION	    2
#define SLS_PENDING_DEACTIVATION    3

#define SL_F_WACK_SLTM	0x00000001
#define SL_F_WACK_SLTM2	0x00000002

/*
 *  RECV SLTM
 *  -----------------------------------
 */
static int
sl_recv_sltm(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	struct sl *sl = SL_PRIV(q);

	if (msg->slc != sl->slc || msg->opc != sl->adj || msg->dpc != sl->loc)
		goto eproto;
	return mtp_send_slta(q, bp, SS7_PVAR_ETSI_00, 0, msg->opc, msg->dpc, msg->sls, sl->slc,
			     msg->data, msg->dlen);
      eproto:
	return (-EPROTO);
}

/*
 *  TIMER T1T  -  SLTM ack
 *  -----------------------------------
 */
STATIC inline int
sl_t1t_timeout(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);

	if (sl->flags & (SL_F_WACK_SLTM | SL_F_WACK_SLTM2)) {
		if (sl->flags & SL_F_WACK_SLTM) {
			mtp_send_sltm(q, NULL, SS7_PVAR_ETSI_00, 0, sl->adj, sl->loc, 0, sl->slc,
				      sl->tdata, sl->tlen);
			sl_timer_start(sl, t1t);
			sl->flags &= ~SL_F_WACK_SLTM;
			sl->flags |= SL_F_WACK_SLTM2;
			return (0);
		}
		sl_sltm_failed(sl->rq, sl);
		sl->flags &= ~SL_F_WACK_SLTM2;
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  TIMER T2T  -  SLTM interval
 *  -----------------------------------
 */
STATIC inline int
sl_t2t_timeout(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);

	if (sl->state == SLS_IN_SERVICE) {
		int i;
		unsigned long random = jiffies;

		sl->flags &= ~SL_F_WACK_SLTM2;
		sl->flags |= SL_F_WACK_SLTM;
		/* 
		   generate new test pattern */
		sl->tlen = jiffies & 0xf;
		for (i = 0; i < 4; i++)
			bcopy(&random, sl->tdata + (i << 2), 4);
		mtp_send_sltm(q, NULL, SS7_PVAR_ETSI_00, 0, sl->adj, sl->loc, 0, sl->slc, sl->tdata,
			      sl->tlen);
		sl_timer_start(sl, t1t);
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  RECV SLTA
 *  -----------------------------------
 */
static int
sl_recv_slta(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	struct sl *sl = SL_PRIV(q);
	int err, i;

	if (sl->state != SLS_IN_SERVICE)
		goto eproto;
	if (!(sl->flags & (SL_F_WACK_SLTM | SL_F_WACK_SLTM2)))
		goto eproto;
	sl_timer_stop(sl, t1t);
	if (msg->slc != sl->slc || msg->opc != sl->adj || msg->dpc != sl->loc)
		goto failed;
	if (sl->tlen != msg->dlen)
		goto failed;
	for (i = 0; i < msg->dlen; i++)
		if (msg->data[i] != sl->tdata[i])
			goto failed;
	if (bp)
		freeb(bp);
      wait_next:
	sl_timer_start(sl, t2t);
	return (0);
      failed:
	if (sl->flags & SL_F_WACK_SLTM) {
		if ((err =
		     mtp_send_sltm(q, bp, SS7_PVAR_ETSI_00, 0, sl->adj, sl->loc, 0, sl->slc,
				   sl->tdata, sl->tlen)))
			return (err);
		sl_timer_start(sl, t1t);
		sl->flags &= ~SL_F_WACK_SLTM;
		sl->flags |= ~SL_F_WACK_SLTM2;
		return (0);
	}
	sl->flags &= ~SL_F_WACK_SLTM2;
	fixme(("Implement link failure function\n"));
	goto wait_next;
      eproto:
	mi_strlog(q, 0, SL_ERROR, "Unexpected SLTA");
	goto error;
      error:
	if (bp)
		freeb(bp);
	freemsg(dp);
	return (0);
}

/*
 *  RECV SSLTM
 *  -----------------------------------
 */
static int
sl_recv_ssltm(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	fixme(("Handle SSLTM\n"));
	return (-EFAULT);
}

/*
 *  RECV SSLTA
 *  -----------------------------------
 */
static int
sl_recv_sslta(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	__ptrace(("SSLTA Unimplemeneted\n"));
	return (-EFAULT);
}

/*
 *  RECV USER
 *  -----------------------------------
 */
static int
sl_recv_user(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	struct mt *mt;

	if ((mt = sl->users[msg->si])) {
		if (canputnext(mt->rq)) {
			freeb(bp);
			putnext(mt->rq, dp);
			return (0);
		}
		return (-EBUSY);
	}
	return sl_reply_upu(sl, q, bp, dp, msg);
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
sl_proc_msg(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp, struct mtp_msg *msg)
{
	unsigned char tag;

	switch (msg->si) {
	default:		/* USER PART */
		return sl_recv_user(sl, q, bp, dp, msg);
	case 0:		/* SNMM */
		tag = ((msg->h0 & 0x0f) << 4) | (msg->h1 & 0x0f);
		switch (tag) {
		case 0x11:	/* coo */
			return sl_recv_coo(sl, q, bp, dp, msg);
		case 0x12:	/* coa */
			return sl_recv_coa(sl, q, bp, dp, msg);
		case 0x15:	/* cbd */
			return sl_recv_cbd(sl, q, bp, dp, msg);
		case 0x16:	/* cba */
			return sl_recv_cba(sl, q, bp, dp, msg);
		case 0x21:	/* eco */
			return sl_recv_eco(sl, q, bp, dp, msg);
		case 0x22:	/* eca */
			return sl_recv_eca(sl, q, bp, dp, msg);
		case 0x31:	/* rct */
			return sl_recv_rct(sl, q, bp, dp, msg);
		case 0x32:	/* tfc */
			return sl_recv_tfc(sl, q, bp, dp, msg);
		case 0x41:	/* tfp */
			return sl_recv_tfp(sl, q, bp, dp, msg);
		case 0x42:	/* tcp */
			return sl_recv_tcp(sl, q, bp, dp, msg);
		case 0x43:	/* tfr */
			return sl_recv_tfr(sl, q, bp, dp, msg);
		case 0x44:	/* tcr */
			return sl_recv_tcr(sl, q, bp, dp, msg);
		case 0x45:	/* tfa */
			return sl_recv_tfa(sl, q, bp, dp, msg);
		case 0x46:	/* tca */
			return sl_recv_tca(sl, q, bp, dp, msg);
		case 0x51:	/* rst */
			return sl_recv_rst(sl, q, bp, dp, msg);
		case 0x52:	/* rsr */
			return sl_recv_rsr(sl, q, bp, dp, msg);
		case 0x53:	/* rcp */
			return sl_recv_rcp(sl, q, bp, dp, msg);
		case 0x54:	/* rcr */
			return sl_recv_rcr(sl, q, bp, dp, msg);
		case 0x61:	/* lin */
			return sl_recv_lin(sl, q, bp, dp, msg);
		case 0x62:	/* lun */
			return sl_recv_lun(sl, q, bp, dp, msg);
		case 0x63:	/* lia */
			return sl_recv_lia(sl, q, bp, dp, msg);
		case 0x64:	/* lua */
			return sl_recv_lua(sl, q, bp, dp, msg);
		case 0x65:	/* lid */
			return sl_recv_lid(sl, q, bp, dp, msg);
		case 0x66:	/* lfu */
			return sl_recv_lfu(sl, q, bp, dp, msg);
		case 0x67:	/* llt */
			return sl_recv_llt(sl, q, bp, dp, msg);
		case 0x68:	/* lrt */
			return sl_recv_lrt(sl, q, bp, dp, msg);
		case 0x71:	/* tra */
			return sl_recv_tra(sl, q, bp, dp, msg);
		case 0x72:	/* trw */
			return sl_recv_trw(sl, q, bp, dp, msg);
		case 0x81:	/* dlc */
			return sl_recv_dlc(sl, q, bp, dp, msg);
		case 0x82:	/* css */
			return sl_recv_css(sl, q, bp, dp, msg);
		case 0x83:	/* cns */
			return sl_recv_cns(sl, q, bp, dp, msg);
		case 0x84:	/* cnp */
			return sl_recv_cnp(sl, q, bp, dp, msg);
		case 0xa1:	/* upu */
			return sl_recv_upu(sl, q, bp, dp, msg);
		case 0xa2:	/* upa *//* ansi91 only */
			return sl_recv_upa(sl, q, bp, dp, msg);
		case 0xa3:	/* upt *//* ansi91 only */
			return sl_recv_upt(sl, q, bp, dp, msg);
		}
		break;
	case 1:		/* SNTM */
		tag = ((msg->h0 & 0x0f) << 4) | (msg->h1 & 0x0f);
		switch (tag) {
		case 0x11:	/* sltm */
			return sl_recv_sltm(sl, q, bp, dp, msg);
		case 0x12:	/* slta */
			return sl_recv_slta(sl, q, bp, dp, msg);
		}
		break;
	case 2:		/* SSNTM */
		tag = ((msg->h0 & 0x0f) << 4) | (msg->h1 & 0x0f);
		switch (tag) {
		case 0x11:	/* ssltm */
			return sl_recv_ssltm(sl, q, bp, dp, msg);
		case 0x12:	/* sslta */
			return sl_recv_sslta(sl, q, bp, dp, msg);
		}
		break;
	}
	swerr();
	return (-EPROTO);
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
mtp_dec_com(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		msg->slc = msg->sls;
		msg->arg.fsnl = *mp->b_rptr++ & 0x7f;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		msg->slc = *mp->b_rptr & 0x0f;
		msg->arg.fsnl = (*mp->b_rptr++ >> 4) | ((*mp->b_rptr++ & 0x7) << 4);
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
mtp_dec_cbm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		msg->slc = msg->sls;
		msg->arg.cbc = *mp->b_rptr++;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		msg->slc = *mp->b_rptr & 0x0f;
		msg->arg.cbc = (*mp->b_rptr++ >> 4) | ((*mp->b_rptr++ & 0xf) << 4);
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
mtp_dec_slm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		msg->slc = msg->sls;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		msg->slc = *mp->b_rptr++ & 0x0f;
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
mtp_dec_tfc(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ITUT:
	case SS7_PVAR_ETSI:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		msg->dest = (*mp->b_rptr++) | ((*mp->b_rptr & 0x3f) << 8);
		msg->arg.stat = *mp->b_rptr++ >> 6;
		return (0);
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		if (mp->b_wptr < mp->b_rptr + 4)
			break;
		msg->dest = (*mp->b_rptr++) | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16);
		msg->arg.stat = (*mp->b_rptr++ & 0x3);
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
mtp_dec_tfm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ITUT:
	case SS7_PVAR_ETSI:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		msg->dest = (*mp->b_rptr++) | ((*mp->b_rptr & 0x3f) << 8);
		return (0);
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		if (mp->b_wptr < mp->b_rptr + 3)
			break;
		msg->dest = (*mp->b_rptr++) | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16);
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
mtp_dec_dlc(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		if (mp->b_wptr < mp->b_rptr + 2)
			break;
		msg->slc = msg->sls;
		msg->arg.sdli = (*mp->b_rptr++) | ((*mp->b_rptr++ & 0x0f) << 8);
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 3)
			break;
		msg->slc = (*mp->b_rptr & 0x0f);
		msg->arg.sdli =
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
static int
mtp_dec_upm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		if (mp->b_wptr < mp->b_rptr + 3)
			break;
		msg->dest = ((*mp->b_rptr++)) | ((*mp->b_rptr++) & 0x3f << 8);
		msg->arg.upi = (*mp->b_rptr++ & 0x0f);
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 4)
			break;
		msg->dest = ((*mp->b_rptr++)) | ((*mp->b_rptr++ << 8)) | ((*mp->b_rptr++ << 16));
		msg->arg.upi = (*mp->b_rptr++ & 0x0f);
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
mtp_dec_sltm(mblk_t *mp, struct mtp_msg *msg)
{
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
		if (msg->si == 2)
			break;
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		msg->slc = msg->sls;
		msg->dlen = *mp->b_rptr++ >> 4;
		msg->data = mp->b_rptr;
		if (mp->b_rptr + msg->dlen > mp->b_wptr)
			break;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		msg->slc = *mp->b_rptr & 0x0f;
		msg->dlen = *mp->b_rptr++ >> 4;
		msg->data = mp->b_rptr;
		if (mp->b_wptr < mp->b_rptr + msg->dlen)
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
mtp_dec_user(mblk_t *mp, struct mtp_msg *msg)
{
	if (mp->b_wptr < mp->b_rptr)
		goto error;
	msg->data = mp->b_rptr;
	msg->dlen = mp->b_wptr - mp->b_rptr;
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
mtp_dec_sio(mblk_t *mp, struct mtp_msg *msg)
{
	/* 
	   decode si, mp and ni */
	switch (msg->pvar & SS7_PVAR_MASK) {
	case SS7_PVAR_JTTC:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		msg->si = *mp->b_rptr & 0x0f;
		msg->mp = (mp->b_rptr[-1] >> 6);	/* get mp from header */
		msg->ni = (*mp->b_rptr++ >> 6) & 0x3;
		return (0);
	case SS7_PVAR_ANSI:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		msg->si = *mp->b_rptr & 0x0f;
		msg->mp = (*mp->b_rptr >> 4) & 0x3;
		msg->ni = (*mp->b_rptr++ >> 6) & 0x3;
		return (0);
	default:
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		msg->si = *mp->b_rptr & 0x0f;
		if (msg->popt & SS7_POPT_MPLEV)
			msg->mp = (*mp->b_rptr >> 4) & 0x3;
		else
			msg->mp = 0;
		msg->ni = (*mp->b_rptr++ >> 6) & 0x3;
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
mtp_dec_rl(mblk_t *mp, struct mtp_msg *msg)
{
	/* 
	   decode the routing label */
	switch (msg->pvar & SS7_PVAR_MASK) {
	default:
	case SS7_PVAR_ETSI:
	case SS7_PVAR_ITUT:
		/* 
		   14-bit point codes - 32-bit RL */
		if (mp->b_wptr < mp->b_rptr + 4)
			break;
		msg->dpc = (*mp->b_rptr++ | ((*mp->b_rptr & 0x3f) << 8));
		msg->opc =
		    (((*mp->b_rptr++ >> 6) & 0x3) | (*mp->
						     b_rptr++ << 2) | ((*mp->b_rptr & 0x0f) << 10));
		msg->sls = (*mp->b_rptr++ >> 4) & 0x0f;
		return (0);
	case SS7_PVAR_ANSI:
	case SS7_PVAR_JTTC:
	case SS7_PVAR_CHIN:
		/* 
		   24-bit point codes - 56-bit RL */
		if (mp->b_wptr < mp->b_rptr + 7)
			break;
		msg->dpc =
		    ((*mp->b_rptr++ | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16))) & 0x00ffffff;
		msg->opc =
		    ((*mp->b_rptr++ | (*mp->b_rptr++ << 8) | (*mp->b_rptr++ << 16))) & 0x00ffffff;
		msg->sls = *mp->b_rptr++;
		if ((msg->pvar & SS7_PVAR_YR) != SS7_PVAR_00)
			msg->sls &= 0x1f;
		else
			msg->sls &= 0xff;
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
mtp_dec_sif(mblk_t *mp, struct mtp_msg *msg)
{
	unsigned char tag;

	switch (msg->si) {
	case 0:		/* SNMM */
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		tag = (*mp->b_rptr << 4) | (*mp->b_rptr >> 4);
		msg->h0 = *mp->b_rptr & 0x0f;
		msg->h1 = (*mp->b_rptr++ >> 4) & 0x0f;
		switch (tag) {
		case 0x11:	/* coo */
		case 0x12:	/* coa */
			return mtp_dec_com(mp, msg);
		case 0x15:	/* cbd */
		case 0x16:	/* cba */
			return mtp_dec_cbm(mp, msg);
		case 0x71:	/* tra */
		case 0x72:	/* trw */
		case 0x31:	/* rct */
			return (0);
		case 0x32:	/* tfc */
			return mtp_dec_tfc(mp, msg);
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
			return mtp_dec_tfm(mp, msg);
		case 0x81:	/* dlc */
			return mtp_dec_dlc(mp, msg);
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
			return mtp_dec_slm(mp, msg);
		case 0xa1:	/* upu */
		case 0xa2:	/* upa *//* ansi91 only */
		case 0xa3:	/* upt *//* ansi91 only */
			return mtp_dec_upm(mp, msg);
		}
		break;
	case 1:		/* SNTM */
	case 2:		/* SSNTM */
		if (mp->b_wptr < mp->b_rptr + 1)
			break;
		tag = (*mp->b_rptr << 4) | (*mp->b_rptr >> 4);
		msg->h0 = *mp->b_rptr & 0x0f;
		msg->h1 = (*mp->b_rptr++ >> 4) & 0x0f;
		switch (tag) {
		case 0x11:	/* sltm */
		case 0x12:	/* slta */
			return mtp_dec_sltm(mp, msg);
		}
		break;
	default:		/* USER PART */
		return mtp_dec_user(mp, msg);
	}
	return (-EPROTO);
}

static int
mtp_dec_msg(queue_t *q, mblk_t *mp, struct mtp_msg *msg)
{
	struct sl *sl = SL_PRIV(q);
	int err;

	if (mp->b_wptr < mp->b_rptr + 1)
		goto emsgsize;
	msg->xq = q;
	msg->timestamp = jiffies;
	msg->pvar = SS7_PVAR_ETSI_00;
	msg->popt = 0;
	if ((err = mtp_dec_sio(mp, msg)))
		goto error;
	if (msg->ni != sl->ni)
		goto discard;
	if ((err = mtp_dec_rl(mp, msg)))
		goto error;
	if ((err = mtp_dec_sif(mp, msg)))
		goto error;
	return (0);
      discard:
	return (-EPROTO);
      error:
	ptrace(("%s: %p: DECODE: decoding error\n", DRV_NAME, sl));
	return (err);
      emsgsize:
	ptrace(("%s: %p: DECODE: decoding error\n", DRV_NAME, sl));
	return (-EMSGSIZE);
}

/**
 * sl_recv_msg: - process received message
 * @sl: SL private structure
 * @q: active queue (lower read queue)
 * @bp: message block to free upon success
 * @dp: user data
 */
static int
sl_recv_msg(struct sl *sl, queue_t *q, mblk_t *bp, mblk_t *dp)
{
	int err;
	struct mtp_msg msg = { 0, };

	if ((err = mtp_dec_msg(q, mp, &msg)))
		goto decode;
	if (sl->ni != msg.ni)
		goto eproto;
	if (sl->loc != msg.dpc)
		goto eproto;
	if (sl->adj != msg.opc)
		goto eproto;
	/* message is for us, process it */
	return sl_proc_msg(sl, q, bp, mp, &msg);
      eproto:
	mi_strlog(q, 0, SL_ERROR, "message not for us NI=%x, DPC=%x, OPC=%x, SI=%x\n",
		  msg.ni, msg.dpc, msg.opc, msg.si);
	goto error;
      decode:
	mi_strlog(q, 0, SL_ERROR, "decoding error %d", err);
	goto error;
      error:
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
/*
 *  M_DATA:
 *  -------------------------------------------------------------------
 */
static int
sl_data_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	if (sl->state != SLS_IN_SERVICE)
		goto outstate;
	return sl_recv_msg(sl, q, NULL, mp);
      outstate:
	mi_strlog(q, 0, SL_ERROR, "would place interface out of state");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  SL_PDU_IND:
 *  -------------------------------------------------------------------
 *  When we receive a PDU from below, we want to decode it and then determine
 *  what to do based on the decoding of the message.
 */
static int
sl_pdu_ind(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_pdu_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_IN_SERVICE)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) || !mp->b_cont)
		goto badprim;
	return sl_recv_msg(sl, q, mp, mp->b_cont);
      badprim:
	mi_strlog(q, 0, SL_ERROR, "invalid primitive format");
	goto error;
      outstate:
	mi_strlog(q, 0, SL_ERROR, "would place interface out of state");
	goto error;
      error:
	freemsg(mp);
	return (0);
}

/*
 *  SL_LINK_CONGESTED_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_link_congested_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_link_cong_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_IN_SERVICE)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl->cong != p->sl_cong_state) {
		/* 
		   check route and routeset */
		sl->cong = p->sl_cong_status;
	}
	if (sl->disc != p->sl_disc_state) {
		/* 
		   check route and routeset */
		sl->disc = p->sl_disc_status;
	}
	fixme(("Write this function\n"));
	return (QR_DONE);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_link_congestion_ceased_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_link_cong_ceased_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_IN_SERVICE)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sl->cong != p->sl_cong_state) {
		/* 
		   check route and routeset */
		sl->cong = p->sl_cong_status;
	}
	if (sl->disc != p->sl_disc_state) {
		/* 
		   check route and routeset */
		sl->disc = p->sl_disc_status;
	}
	fixme(("Write this function\n"));
	return (QR_DONE);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_retrieved_message_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_retrieved_msg_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_WIND_RETR)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (!mp->b_cont)
		goto efault;
	bufq_queue(&sl->sl.buf, mp->b_cont);
	fixme(("Write this function\n"));
	return (QR_TRIMMED);
      efault:
	ptrace(("%s: %p: ERROR: no data\n", DRV_NAME, mt));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_retrieval_complete_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_retrieval_comp_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_WIND_RETR)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_RB_CLEARED_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_rb_cleared_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_rb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_WIND_CLRB)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_BSNT_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_bsnt_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_bsnt_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_WIND_BSNT)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_IN_SERVICE_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_in_service_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_in_service_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_WIND_INSI)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_OUT_OF_SERVICE_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_out_of_service_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_out_of_service_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_IN_SERVICE)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_remote_processor_outage_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_rem_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_IN_SERVICE)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_remote_processor_recovered_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_rem_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_PROC_OUTG)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_RTB_CLEARED_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_rtb_cleared_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_rtb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_WIND_CLRB)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_retrieval_not_possible_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_retrieval_not_poss_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_WIND_RETR)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_bsnt_not_retrievable_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_bsnt_not_retr_ind_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != SLS_WIND_BSNT)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

#if 0
/*
 *  SL_OPTMGMT_ACK:
 *  -------------------------------------------------------------------
 */
static int
sl_optmgmt_ack(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
	return (-EOPNOTSUPP);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  SL_NOTIFY_IND:
 *  -------------------------------------------------------------------
 */
static int
sl_notify_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const sl_notify_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
	return (-EOPNOTSUPP);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}
#endif
/*
 *  LMI_INFO_ACK:
 *  -------------------------------------------------------------------
 */
static int
lmi_info_ack(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  LMI_OK_ACK:
 *  -------------------------------------------------------------------
 */
static int
lmi_ok_ack(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const lmi_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	switch (sl->state) {
	case LMI_ATTACH_PENDING:
		sl_set_state(LMI_DISABLED);
		break;
	case LMI_DETACH_PENDING:
		sl_set_state(LMI_UNATTACHED);
		break;
	default:
		goto outstate;
	}
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  LMI_ERROR_ACK:
 *  -------------------------------------------------------------------
 */
static int
lmi_error_ack(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	switch (sl->state) {
	case LMI_ATTACH_PENDING:
		sl_set_state(LMI_UNATTACHED);
		break;
	case LMI_DETACH_PENDING:
	case LMI_ENABLE_PENDING:
		sl_set_state(LMI_DISABLED);
		break;
	case LMI_DISABLE_PENDING:
		sl_set_state(LMI_ENABLED);
		break;
	default:
		goto outstate;
	}
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  LMI_ENABLE_CON:
 *  -------------------------------------------------------------------
 */
static int
lmi_enable_con(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != LMI_ENABLED_PENDING)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  LMI_DISABLE_CON:
 *  -------------------------------------------------------------------
 */
static int
lmi_disable_con(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;

	if (sl->state != LMI_DISABLE_PENDING)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	return (-EPROTO);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  LMI_OPTMGMT_ACK:
 *  -------------------------------------------------------------------
 */
static int
lmi_optmgmt_ack(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  LMI_ERROR_IND:
 *  -------------------------------------------------------------------
 */
static int
lmi_error_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  LMI_STATS_IND:
 *  -------------------------------------------------------------------
 */
static int
lmi_stats_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
}

/*
 *  LMI_EVENT_IND:
 *  -------------------------------------------------------------------
 */
static int
lmi_event_ind(queue_t *q, mblk_t *mp)
{
#ifdef MTP_COMPLETE
	struct sl *sl = SL_PRIV(q);
	int err;
	const lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	fixme(("Write this function\n"));
	return (-EFAULT);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive formant\n", DRV_NAME, mt));
	return (-EINVAL);
#endif
	__ptrace(("Unimplemented\n"));
	return (-EFAULT);
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
static int
mtp_data_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	int err;
	int dlen = msgdsize(mp);

	if (mtp_get_state(mt) == TS_IDLE)
		goto discard;
	if (mtp_get_state(mt) != TS_DATA_XFER && mtp_get_state(mt) != TS_WREQ_ORDREL)
		goto outstate;
	if (dlen == 0 || dlen > 272)
		goto baddata;
	return mtp_send_msg(mt, NULL, &mt->dst, mp);
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", DRV_NAME, mt, dlen));
	goto error;
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	goto error;
      discard:
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, mt));
	return (QR_DONE);
      error:
	return m_error(q, mp, EPROTO);
}

/*
 *  T_CONN_REQ
 *  -----------------------------------
 *  As MTP is really a connectionless protocol, when we form a connection we
 *  simply remember the destination address.  Some interim MTPs had the
 *  abilitty to send a UPT (User Part Test) message.  If the protocol variant
 *  has this ability, we wait for the result of the User Part Test before
 *  confirming the connection.
 */
static int
t_conn_req(queue_t *q, mblk_t *mp)
{
	struct mt *mtp = MT_PRIV(q);
	int err = -EFAULT;
	const struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;

	if (mtp_get_state(mtp) != TS_IDLE)
		goto outstate;
	if (mlen < sizeof(*p))
		goto einval;
	if (mlen < p->DEST_offset + p->DEST_length)
		goto einval;
	if (mlen < p->OPT_offest + p->OPT_length)
		goto einval;
	{
		mtp_addr_t *dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);

		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		if (dst->mtp_family != AF_MTP)
			goto badaddr;
		if (dst->si == 0 && mtp->src.mtp_si == 0)
			goto noaddr;
		if (dst->si < 3 && mtp->cred.cr_uid != 0)
			goto acces;
		if (dst->si != mtp->src.mtp_si)
			goto baddaddr;
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
			return t_ok_ack(q, T_CONN_REQ);
		}
	}
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options\n", DRV_NAME, mt));
	goto error;
      acces:
	err = TACCES;
	ptrace(("%s: %p: ERROR: no permission for address\n", DRV_NAME, mt));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: address is unusable\n", DRV_NAME, mt));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid message format\n", DRV_NAME, mt));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	goto error;
      error:
	return t_error_ack(q, T_CONN_REQ, err);
}

/*
 *  T_DISCON_REQ         2 - TC disconnection request
 *  -------------------------------------------------------------------
 */
static int
t_discon_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	int err;
	const struct T_discon_req *p = (typeof(p)) mp->b_rptr;

	if ((1 << mtp_get_state(mt)) & ~TSM_CONNECTED)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	switch (mtp_get_state(mt)) {
	case TS_WCON_CREQ:
		mtp_set_state(mt, TS_WACK_DREQ6);
		break;
	case TS_DATA_XFER:
		mtp_set_state(mt, TS_WACK_DREQ9);
		break;
	}
	return t_ok_ack(q, T_DISCON_REQ);
      einval:
	err = -EINVAL;
	ptrace(("%s: ERROR: invalid primitive format\n", DRV_NAME));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: ERROR: would place i/f out of state\n", DRV_NAME));
	goto error;
      error:
	return t_error_ack(q, T_DISCON_REQ, err);
}

/*
 *  T_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------
 */
static int
t_data_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = PRIV(q);
	int err;
	const struct T_data_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;

	if (mtp_get_state(mt) == TS_IDLE)
		goto discard;
	if (dlen == 0 || dlen > 272)
		goto baddata;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << mtp_get_state(mt)) & ~TSM_OUTDATA)
		goto outstate;
	if ((err = mtp_send_msg(mt, NULL, &mt->dst, mp->b_cont)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", DRV_NAME, mt, dlen));
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
      error:
	return m_error(q, mp, EPROTO);
}

/*
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------
 */
static int
t_exdata_req(queue_t *q, mblk_t *mp)
{
	(void) mp;
	return m_error(q, mp, EPROTO);
}

/*
 *  T_INFO_REQ           5 - Information Request
 *  -------------------------------------------------------------------
 */
static int
t_info_req(queue_t *q, mblk_t *mp)
{
	(void) mp;
	return t_info_ack(q);
}

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -------------------------------------------------------------------
 */
static int
t_bind_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	int err;
	const struct T_bind_req *p = (typeof(p)) mp->b_rptr;

	if (mtp_get_state != TS_UNBND)
		goto outstate;
	mtp_set_state(mt, TS_WACK_BREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->CONIND_number)
		goto notsupport;
	if ((mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_lenth)
	    || (p->ADDR_length != sizeof(mtp_addr_t)))
		goto badaddr;
	{
		mtp_addr_t *src = (typeof(src)) (mp->b_rptr + p->ADDR_offset);

		/* 
		   we don't allow wildcards yet. */
		if (src->mtp_family != AF_MTP)
			goto badaddr;
		if (!src->si || !src->pc)
			goto noaddr;
		if (src->si < 3 && mt->cred.cr_uid != 0)
			goto acces;
		if ((err = mtp_check_src(mt, src)))
			goto error;
		return t_bind_ack(q, src);
	}
      acces:
	err = TACCES;
	ptrace(("%s: %p: ERROR: no permission for address\n", DRV_NAME, mt));
	goto error;
      noaddr:
	err = TNOADDR;
	ptrace(("%s: %p: ERROR: couldn't allocate address\n", DRV_NAME, mt));
	goto error;
      badaddr:
	err = TBADADDR;
	ptrace(("%s: %p: ERROR: address is invalid\n", DRV_NAME, mt));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mt));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	goto error;
      notsupport:
	err = TNOTSUPPORT;
	ptrace(("%s: %p: ERROR: primitive not support for T_CLTS\n", DRV_NAME, mt));
	goto error;
      error:
	return t_error_ack(q, T_BIND_REQ, err);
}

/*
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -------------------------------------------------------------------
 */
static int
t_unbind_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	int err;

	if (mtp_get_state(mt) != TS_IDLE)
		goto outstate;
	mtp_set_state(mt, TS_WACK_UREQ);
	return t_ok_ack(q, T_UNBIND_REQ);
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	goto error;
      error:
	return t_error_ack(q, T_UNBIND_REQ, err);
}

/*
 *  T_UNITDATA_REQ       8 -Unitdata Request 
 *  -------------------------------------------------------------------
 */
static int
t_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	int err;
	const struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	size_t dlen = mp->b_cont ? msgdsize(mp->b_cont) : 0;

	if (mtp_get_state(mt) != TS_IDLE)
		goto outstate;
	if (dlen == 0)
		goto baddata;
	if (dlen > 272)
		goto baddata;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	else {
		mtp_addr_t *dst = (typeof(dst)) (mp->b_rptr + p->DEST_offset);

		if (p->DEST_length < sizeof(*dst))
			goto badaddr;
		if (dst->si < 3 && mt->cred.cr_uid != 0)
			goto acces;
		if (!mtp_check_dst(mt, dst))
			goto badaddr;
		else {
			struct mtp_opts opts = { 0L, NULL, };

			if (mtp_parse_opts(mt, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
				goto badopt;
			if ((err = mtp_send_msg(mt, &opts, dst, mp->b_cont)) == QR_ABSORBED)
				return (QR_TRIMMED);
			return (err);
		}
	}
      badopt:
	ptrace(("%s: %p: ERROR: bad options\n", DRV_NAME, mt));
	goto error;
      acces:
	ptrace(("%s: %p: ERROR: no permission to address\n", DRV_NAME, mt));
	goto error;
      badadd:
	ptrace(("%s: %p: ERROR: bad destination address\n", DRV_NAME, mt));
	goto error;
      einval:
	ptrace(("%s: %p: ERROR: invalid parameter\n", DRV_NAME, mt));
	goto error;
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	goto error;
      baddata:
	ptrace(("%s: %p: ERROR: bad data size %d\n", DRV_NAME, mt, dlen));
	goto error;
      error:
	return m_error(q, mp, EPROTO);
}

/*
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------
 */
static int
t_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	int err = 0;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;

#ifdef TS_WACK_OPTREQ
	if (mtp_get_state(mt) == TS_IDLE)
		mtp_set_state(mt, TS_WACK_OPTREQ);
#endif
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	{
		struct mtp_opts opts = { 0L, NULL, };

		if (mtp_parse_opts(mt, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
		switch (p->MGMT_flags) {
		case T_CHECK:
			err = mtp_opt_check(mt, &opts);
			break;
		case T_NEGOTIATE:
			if (!opts.flags)
				mtp_opt_default(mt, &opts);
			else if ((err = mtp_opt_check(mt, &opts)))
				break;
			err = mtp_opt_negotiate(mt, &opts);
			break;
		case T_DEFAULT:
			err = mtp_opt_default(mt, &opts);
			break;
		case T_CURRENT:
			err = mtp_opt_current(mt, &opts);
			break;
		default:
			goto badflag;
		}
		if (err)
			goto provspec;
		return t_optmgmt_ack(q, p->MGMT_flags, &opts);
	}
      provspec:
	err = err;
	ptrace(("%s: %p: ERROR: provider specific\n", DRV_NAME, mt));
	goto error;
      badflag:
	err = TBADFLAG;
	ptrace(("%s: %p: ERROR: bad options flags\n", DRV_NAME, mt));
	goto error;
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options\n", DRV_NAME, mt));
	goto error;
      einval:
	err = -EINVAL;
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mt));
	goto error;
      error:
	return t_error_ack(q, T_OPTMGMT_REQ, err);
}

/*
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -------------------------------------------------------------------
 */
static int
t_ordrel_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = PRIV(q);
	const struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;

	if ((1 << mtp_get_state(mt)) & ~TSM_OUTDATA)
		goto outstate;
	switch (mtp_get_state(mt)) {
	case TS_DATA_XFER:
		mtp_set_state(mt, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		goto error;
	}
	return t_ordrel_ind(q);
      outstate:
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	goto error;
      error:
	return m_error(q, mp, EPROTO);
}

/*
 *  T_OPTDATA_REQ       24 - Data with options request
 *  -------------------------------------------------------------------
 */
static int
t_optdata_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	int err;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_optdata_req *p = (typeof(p)) mp->b_rptr;

	if (mtp_get_state(mt) == TS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p)
	    || mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto einval;
	if ((1 << mtp_get_state(mt)) & ~TSM_OUTDATA)
		goto outstate;
	if (p->DATA_flags & T_ODF_EX || p->DATA_flags & T_ODF_MORE)
		goto notsupport;
	else {
		struct mtp_opts opts = { 0L, NULL, };

		if (mtp_parse_opts(mt, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
		if ((err = mtp_send_msg(mt, &opts, &mt->dst, mp->b_cont)) == QR_ABSORBED)
			return (QR_TRIMMED);
		return (err);
	}
      badopt:
	err = TBADOPT;
	ptrace(("%s: %p: ERROR: bad options\n", DRV_NAME, mt));
	goto error;
      outstate:
	err = TOUTSTATE;
	ptrace(("%s: %p: ERROR: would place i/f out of state\n", DRV_NAME, mt));
	goto error;
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mt));
	return m_error(q, mp, EPROTO);
      discard:
	ptrace(("%s: %p: ERROR: ignore in idle state\n", DRV_NAME, mt));
	return (QR_DONE);
      error:
	return t_error_ack(q, T_OPTDATA_REQ, err);
}

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ          25 - Address Request
 *  -------------------------------------------------------------------
 */
static int
t_addr_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = PRIV(q);

	(void) mp;
	switch (mtp_get_state(mt)) {
	case TS_UNBND:
		return t_addr_ack(q, NULL, NULL);
	case TS_IDLE:
		return t_addr_ack(q, &mt->src, NULL);
	case TS_WCON_CREQ:
	case TS_DATA_XFER:
	case TS_WIND_ORDREL:
	case TS_WREQ_ORDREL:
		return t_addr_ack(q, &mt->src, &mt->dst);
	case TS_WRES_CIND:
		return t_addr_ack(q, NULL, &mt->dst);
	}
	return t_error_ack(q, T_ADDR_REQ, TOUTSTATE);
}
#endif

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ    ?? - Capability Request
 *  -------------------------------------------------------------------
 */
static int
t_capability_req(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	int err;
	const struct T_capability_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	return t_capability_ack(q, p->CAP_bits1);
      einval:
	ptrace(("%s: %p: ERROR: invalid primitive format\n", DRV_NAME, mt));
	return t_error_ack(q, T_CAPABILITY_REQ, -EINVAL);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  IO Controls
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  MTP_IOCGOPTION  - lmi_ioption_t
 *  -------------------------------------------------------------------
 *  Gets the current default protocol variant and options for Signalling
 *  Points.  The default default is SS7_PVAR_ETSI_00 with no protocol options.
 */
static int
mtp_iocgoption(queue_t *q, mblk_t *mp)
{
	mblk_t *dp;

	if ((dp = mp->b_cont)) {
		lmi_option_t *arg = (typeof(arg)) dp->b_rptr;
		struct mt *mtp = MT_PRIV(q);
		int ret = 0;
		int flags = 0;

		if (dp->b_wptr < dp->b_rptr + sizeof(*arg))
			goto einval;
		*arg = mtp_default_option;
		return (ret);
	}
      einval:
	rare();
	return (-EINVAL);
}

/*
 *  MTP_IOCSOPTION  - lmi_ioption_t
 *  -------------------------------------------------------------------
 *  Sets the default protocol variant and options for Signalling Points.  The
 *  default default is SS7_PVAR_ETSI_00 with no protocol options.
 */
static int
mtp_iocsoption(queue_t *q, mblk_t *mp)
{
	mblk_t *dp;

	if ((dp = mp->b_cont)) {
		lmi_option_t *arg = (typeof(arg)) dp->b_rptr;
		struct mt *mtp = MT_PRIV(q);
		int ret = 0;
		int flags = 0;

		if (dp->b_wptr < dp->b_rptr + sizeof(*arg))
			goto einval;
		switch (arg->pvar) {
		case SS7_PVAR_ITUT_88:
		case SS7_PVAR_ITUT_93:
		case SS7_PVAR_ITUT_96:
		case SS7_PVAR_ITUT_00:
		case SS7_PVAR_ETSI_88:
		case SS7_PVAR_ETSI_93:
		case SS7_PVAR_ETSI_96:
		case SS7_PVAR_ETSI_00:
		case SS7_PVAR_ANSI_88:
		case SS7_PVAR_ANSI_92:
		case SS7_PVAR_ANSI_96:
		case SS7_PVAR_ANSI_00:
		case SS7_PVAR_JTTC_94:
		case SS7_PVAR_CHIN_00:
			/* 
			   these are fully supported */
			break;
		default:
			todo(("Implement and test other protocol variants\n"));
			goto einval;
		}
		mtp_default_option = *arg;;
		return (ret);
	}
      einval:
	rare();
	return (-EINVAL);
}

/*
 *  MTP_IOCGCONFIG  - mtp_config_t
 *  -------------------------------------------------------------------
 */
static inline int
mtp_iocgconfig(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCSCONFIG  - mtp_config_t
 *  -------------------------------------------------------------------
 */

static int
mtp_iocsconfig(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCTCONFIG  - mtp_config_t
 *  -------------------------------------------------------------------
 */
static int
mtp_ioctconfig(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCCCONFIG  - mtp_config_t
 *  -------------------------------------------------------------------
 */
static int
mtp_ioccconfig(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCGSTATEM  - mtp_statem_t
 *  -------------------------------------------------------------------
 */
static int
mtp_iocgstatem(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCCMRESET  - mtp_statem_t
 *  -------------------------------------------------------------------
 */
static int
mtp_ioccmreset(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCGSTATSP  - mtp_stats_t
 *  -------------------------------------------------------------------
 */
static int
mtp_iocgstatsp(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCSSTATSP  - mtp_stats_t
 *  -------------------------------------------------------------------
 */
static int
mtp_iocsstatsp(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCGSTATS   - mtp_stats_t
 *  -------------------------------------------------------------------
 */
static int
mtp_iocgstats(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCSSTATS   - mtp_stats_t
 *  -------------------------------------------------------------------
 */
static int
mtp_iocsstats(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCGNOTIFY  - mtp_notify_t
 *  -------------------------------------------------------------------
 */
static int
mtp_iocgnotify(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCSNOTIFY  - mtp_notify_t
 *  -------------------------------------------------------------------
 */
static int
mtp_iocsnotify(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCCNOTIFY  - mtp_notify_t
 *  -------------------------------------------------------------------
 */
static int
mtp_ioccnotify(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCCMGMT    - mtp_ctl_t
 *  -------------------------------------------------------------------
 */
static int
mtp_ioccmgmt(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
}

/*
 *  MTP_IOCCPASS    - mtp_ulong
 *  -------------------------------------------------------------------
 */
static int
mtp_ioccpass(queue_t *q, mblk_t *mp)
{
	return (-EOPNOTSUPP);
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
static int
mt_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmn), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;

	switch (type) {
	case __SID:
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
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n",
					DRV_NAME, mt));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			if ((sl = sl_alloc_priv(lb->l_qbot, &sl_list, lb->l_index, iocp->ioc_cr)))
				break;
			ret = -ENOMEM;
			break;
		case _IOC_NR(I_PUNLINK):
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n",
					DRV_NAME, mt));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			for (sl = sl_list; sl; sl = sl->next)
				if (sl->u.mux.index == lb->l_index)
					break;
			if (!sl) {
				ret = -EINVAL;
				ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n"));
				break;
			}
			sl_free_priv(sl->rq);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %d\n", DRV_NAME, nr));
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
		case _IOC_NR(MTP_IOCGOPTION):	/* lmi_option_t */
			rtn = mtp_iocgoption(q, mp);
			break;
		case _IOC_NR(MTP_IOCSOPTION):	/* lmi_option_t */
			rtn = mtp_iocsoption(q, mp);
			break;
		case _IOC_NR(MTP_IOCGCONFIG):	/* mtp_config_t */
			rtn = mtp_iocgconfig(q, mp);
			break;
		case _IOC_NR(MTP_IOCSCONFIG):	/* mtp_config_t */
			rtn = mtp_iocsconfig(q, mp);
			break;
		case _IOC_NR(MTP_IOCTCONFIG):	/* mtp_config_t */
			rtn = mtp_ioctconfig(q, mp);
			break;
		case _IOC_NR(MTP_IOCCCONFIG):	/* mtp_config_t */
			rtn = mtp_ioccconfig(q, mp);
			break;
		case _IOC_NR(MTP_IOCGSTATEM):	/* mtp_statem_t */
			rtn = mtp_iocgstatem(q, mp);
			break;
		case _IOC_NR(MTP_IOCCMRESET):	/* mtp_statem_t */
			rtn = mtp_ioccmreset(q, mp);
			break;
		case _IOC_NR(MTP_IOCGSTATSP):	/* mtp_stats_t */
			rtn = mtp_iocgstatsp(q, mp);
			break;
		case _IOC_NR(MTP_IOCSSTATSP):	/* mtp_stats_t */
			rtn = mtp_iocsstatsp(q, mp);
			break;
		case _IOC_NR(MTP_IOCGSTATS):	/* mtp_stats_t */
			rtn = mtp_iocgstats(q, mp);
			break;
		case _IOC_NR(MTP_IOCSSTATS):	/* mtp_stats_t */
			rtn = mtp_iocsstats(q, mp);
			break;
		case _IOC_NR(MTP_IOCGNOTIFY):	/* mtp_notify_t */
			rtn = mtp_iocgnotify(q, mp);
			break;
		case _IOC_NR(MTP_IOCSNOTIFY):	/* mtp_notify_t */
			rtn = mtp_iocsnotify(q, mp);
			break;
		case _IOC_NR(MTP_IOCCNOTIFY):	/* mtp_notify_t */
			rtn = mtp_ioccnotify(q, mp);
			break;
		case _IOC_NR(MTP_IOCCMGMT):	/* mtp_ctl_t */
			rtn = mtp_ioccmgmt(q, mp);
			break;
		case _IOC_NR(MTP_IOCCPASS):	/* mtp_ulong */
			rtn = mtp_ioccpass(q, mp);
			break;
		default:
			ptrace(("%s: %d: ERROR: Unsupported MTP ioctl %d\n", nr));
			ret = (-EOPNOTSUPP);
			break;
		}
		break;
	}
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
 *  M_IOCACK, M_IOCNAK Handling
 *  -------------------------------------------------------------------------
 */
static int
sl_r_iocack(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	mblk_t *dp;
	queue_t *mq;
	struct mt *mtp;
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	struct mtp_pass *pass = mp->b_cont ? ((typeof(pass)) mp->b_cont->b_rptr) - 1 : NULL;

	if (!mtp_lmq || !(lm = LM_PRIV(mtp_lmq)) || lm->ioc_id != iocp->ioc_id)
		goto discard;
	lm->ioc_id == NULL;
	if (!(dp = mp->b_cont) || dp->b_rptr <= dp->b_datap->db_base + sizeof(*pass))
		goto discard;
	dp->b_rptr -= sizeof(*pass);
	for (mq = mtp_list; mq && mq != *(typeof(mq) *) dp->b_rptr; mq = mq->next) ;
	if (!mq || mq->ioc_id != iocp->ioc_id)
		goto discard;
	pass->pass_cmd = xchg(&iocp->ioc_cmd, MTP_IOCPASS);
	putnext(lm->rq, mp);
	return (QR_ABSORBED);
      discard:
	return (QR_DONE);
}

/*
 *  M_PROTO, M_PCPROTO Handling
 *  -------------------------------------------------------------------------
 */
static int
sl_r_proto(queue_t *q, mblk_t *mp)
{
	int err;
	ulong prim;
	str_t *s = PRIV(q);
	ulong oldstate = s->state;

	if ((prim = *(ulong *) mp->b_rptr) == SL_PDU_IND) {
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- SL_PDU_IND");
		if ((err = sl_pdu_ind(q, mp)) < 0)
			s->state = oldstate;
		return (err);
	}
	switch (prim) {
	case SL_PDU_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- SL_PDU_IND");
		err = sl_pdu_ind(q, mp);
		break;
	case SL_LINK_CONGESTED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_LINK_CONGESTED_IND");
		err = sl_link_congested_ind(q, mp);
		break;
	case SL_LINK_CONGESTION_CEASED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_LINK_CONGESTION_CEASED_IND");
		err = sl_link_congestion_ceased_ind(q, mp);
		break;
	case SL_RETRIEVED_MESSAGE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_RETRIEVED_MESSAGE_IND");
		err = sl_retrieved_message_ind(q, mp);
		break;
	case SL_RETRIEVAL_COMPLETE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_RETRIEVAL_COMPLETE_IND");
		err = sl_retrieval_complete_ind(q, mp);
		break;
	case SL_RB_CLEARED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_RB_CLEARED_IND");
		err = sl_rb_cleared_ind(q, mp);
		break;
	case SL_BSNT_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_BSNT_IND");
		err = sl_bsnt_ind(q, mp);
		break;
	case SL_IN_SERVICE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_IN_SERVICE_IND");
		err = sl_in_service_ind(q, mp);
		break;
	case SL_OUT_OF_SERVICE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_OUT_OF_SERVICE_IND");
		err = sl_out_of_service_ind(q, mp);
		break;
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_OUTAGE_IND");
		err = sl_remote_processor_outage_ind(q, mp);
		break;
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_OUTAGE_RECOVERED_IND");
		err = sl_remote_processor_recovered_ind(q, mp);
		break;
	case SL_RTB_CLEARED_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_RTB_CLEARED_IND");
		err = sl_rtb_cleared_ind(q, mp);
		break;
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_RETRIEVAL_NOT_POSSIBLE_IND");
		err = sl_retrieval_not_possible_ind(q, mp);
		break;
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_BSNT_NOT_RETRIEVABLE_IND");
		err = sl_bsnt_not_retrievable_ind(q, mp);
		break;
#if 0
	case SL_OPTMGMT_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_OPTMGMT_ACK");
		err = sl_optmgmt_ack(q, mp);
		break;
	case SL_NOTIFY_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- SL_NOTIFY_IND");
		err = sl_notify_ind(q, mp);
		break;
#endif
	case LMI_INFO_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_INFO_ACK");
		err = lmi_info_ack(q, mp);
		break;
	case LMI_OK_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_OK_ACK");
		err = lmi_ok_ack(q, mp);
		break;
	case LMI_ERROR_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_ERROR_ACK");
		err = lmi_error_ack(q, mp);
		break;
	case LMI_ENABLE_CON:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_ENABLE_CON");
		err = lmi_enable_con(q, mp);
		break;
	case LMI_DISABLE_CON:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_DISABLE_CON");
		err = lmi_disable_con(q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_OPTMGMT_ACK");
		err = lmi_optmgmt_ack(q, mp);
		break;
	case LMI_ERROR_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_ERROR_IND");
		err = lmi_error_ind(q, mp);
		break;
	case LMI_STATS_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_STATS_IND");
		err = lmi_stats_ind(q, mp);
		break;
	case LMI_EVENT_IND:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_EVENT_IND");
		err = lmi_event_ind(q, mp);
		break;
	default:
		/* reject what we don't recognize */
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- LMI_????_IND");
		err = -EOPNOTSUPP;
		break;
	}
	if (err < 0)
		s->state = oldstate;
	return (err);
}

static int
mt_w_proto(queue_t *q, mblk_t *mp)
{
	int err;
	ulong prim;
	str_t *s = PRIV(q);
	ulong oldstate = s->state;

	switch ((prim = *(ulong *) mp->b_rptr)) {
	case T_CONN_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_CONN_REQ");
		err = t_conn_req(q, mp);
		break;
	case T_CONN_RES:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_CONN_RES");
		err = t_error_ack(q, prim, TNOTSUPPORT);
		break;
	case T_DISCON_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_DISCON_REQ");
		err = t_discon_req(q, mp);
		break;
	case T_DATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> T_DATA_REQ");
		err = t_data_req(q, mp);
		break;
	case T_EXDATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> T_EXDATA_REQ");
		err = t_exdata_req(q, mp);
		break;
	case T_INFO_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_INFO_REQ");
		err = t_info_req(q, mp);
		break;
	case T_BIND_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_BIND_REQ");
		err = t_bind_req(q, mp);
		break;
	case T_UNBIND_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_UNBIND_REQ");
		err = t_unbind_req(q, mp);
		break;
	case T_UNITDATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> T_UNITDATA_REQ");
		err = t_unitdata_req(q, mp);
		break;
	case T_OPTMGMT_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_OPTMGMT_REQ");
		err = t_optmgmt_req(q, mp);
		break;
	case T_ORDREL_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_ORDREL_REQ");
		err = t_ordrel_req(q, mp);
		break;
	case T_OPTDATA_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_OPTDATA_REQ");
		err = t_optdata_req(q, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_ADDR_REQ");
		err = t_addr_req(q, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> T_CAPABILITY_REQ");
		err = t_capability_req(q, mp);
		break;
#endif
	default:
		err = t_error_ack(q, prim, TNOTSUPPORT);
		break;
	}
	if (err < 0)
		s->state = oldstate;
	return (err);
}

/*
 *  M_SIG, M_PCSIG Handling
 *  -------------------------------------------------------------------------
 */
static int
mt_w_sig(queue_t *q, mblk_t *mp)
{
	struct mt *mt;
	int err = 0;

	read_lock(&mt_mux_lock);
	if (!(mt = (struct mt *) mi_trylock(q))) {
		read_unlock(&mt_mux_lock);
		return (mi_timer_requeue(mp) ? -EAGAIN : 0);
	}
	read_unlock(&mt_mux_lock);

	if (likely(mi_timer_valid(mp))) {
		switch (*(uint *) mp->b_rptr) {
		case t1:
			mi_strlog(q, STRLOGTO, SL_TRACE, "t1 expirty at %lu", jiffies);
			err = mt_t1_timeout(q);
			break;
		default:
			mi_strlog(q, 0, SL_ERROR, "unknown timer %u", *(uint *) mp->b_rptr);
			break;
		}
		err = (err && mi_timer_requeue(mp)) ? err : 0;
	}

	mi_unlock((caddr_t) mt);
	return (err);
}
static int
sl_r_sig(queue_t *q, mblk_t *mp)
{
	struct sl *sl;
	int err;

	read_lock(&mt_mux_lock);
	if (!(sl = (struct sl *) mi_trylock(q))) {
		read_unlock(&mt_mux_lock);
		return (mi_timer_requeue(mp) ? -EAGAIN : 0);
	}
	read_unlock(&mt_mux_lock);

	if (likely(mi_timer_valid(mp))) {
		switch (*(uint *) mp->b_rptr) {
		case t1t:
			mi_strlog(q, STRLOGTO, SL_TRACE, "t1t expiry at %lu", jiffies);
			err = sl_t1t_timeout(q);
			break;
		case t2t:
			mi_strlog(q, STRLOGTO, SL_TRACE, "t2t expiry at %lu", jiffies);
			err = sl_t1t_timeout(q);
			break;
		default:
			mi_strlog(q, 0, SL_ERROR, "unknown timer %u", *(uint *) mp->b_rptr);
			break;
		}
		err = (err && mi_timer_requeue(mp)) ? err : 0;
	}

	mi_unlock((caddr_t) sl);
	return (QR_ABSORBED);
}

/*
 *  M_DATA Handling
 *  -------------------------------------------------------------------------
 */
static int
sl_r_data(queue_t *q, mblk_t *mp)
{
	return sl_data_ind(q, mp);
}
static int
mt_w_data(queue_t *q, mblk_t *mp)
{
	return mtp_data_req(q, mp);
}

/*
 *  M_ERROR Handling
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a signalling link has failed badly.
 *  Move link to the out-of-service state, notify management, and perform
 *  normal link failure actions.
 */
static int
sl_r_error(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	sl->state = -*mp->b_rptr;
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 *  M_HANGUP Handling
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a signalling link has failed badly.
 *  Move link to the out-of-service state, notify management, and perform
 *  normal link failure actions.
 */
static int
sl_r_hangup(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	sl->state = -EPIPE;
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 *  M_FLUSH handling
 *  -------------------------------------------------------------------------
 */
static int
sl_r_flush(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	queue_t *oq;

	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		read_lock(&mt_mux_lock);
		if (sl->other && (oq = sl->other->rq)) {
			putnext(oq, mp);
			read_unlock(&mt_mux_lock);
			return (0);
		}
		read_unlock(&mt_mux_lock);
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

static int
mt_w_flush(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	queue_t *oq;

	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		read_lock(&mt_mux_lock);
		if (mt->sl && (oq = mt->sl->wq)) {
			putnext(oq, mp);
			read_unlock(&mt_mux_lock);
			return (0);
		}
		read_unlock(&mt_mux_lock);
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

/*
 *  Unrecognized STREAMS message handling
 *  -------------------------------------------------------------------------
 */
static int
mt_w_next(queue_t *q, mblk_t *mp)
{
	struct mt *mt = MT_PRIV(q);
	queue_t *oq;

	read_lock(&mt_mux_lock);
	if (mt->sl && (oq = mt->sl->wq)) {
		if (pcmsg(mp) || bcanputnext(oq, mp->b_band)) {
			putnext(oq, mp);
			read_unlock(&mt_mux_lock);
			return (0);
		}
		read_unlock(&mt_mux_lock);
		return (-EBUSY);
	}
	read_unlock(&mt_mux_lock);
	freemsg(mp);
	return (0);
}

static int
sl_r_next(queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);
	queue_t *oq;

	read_lock(&mt_mux_lock);
	if (sl->other && (oq = sl->other->wq)) {
		if (pcmsg(mp) || bcanputnext(oq, mp->b_band)) {
			putnext(oq, mp);
			read_unlock(&mt_mux_lock);
			return (0);
		}
		read_unlock(&mt_mux_lock);
		return (-EBUSY);
	}
	read_unlock(&mt_mux_lock);
	freemsg(mp);
	return (0);
}

/*
 *  STREAMS message handling
 *  =========================================================================
 */
static int
mt_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mt_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mt_w_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mt_w_sig(q, mp);
	case M_FLUSH:
	case M_IOCTL:
		return mt_w_ioctl(q, mp);
	case M_IOCDATA:
		return mt_w_iocdata(q, mp);
	}
	return mt_w_next(q, mp);
}
static inline fastcall int
mt_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return mt_w_data(q, mp);
	if (DB_TYPE(mp) == M_PROTO)
		if (mp->b_wptr >= mp->b_rptr + sizeof(uint32_t))
			if (*(uint32_t *) mp->b_rptr == MTP_TRANSFER_REQ)
				return mtp_transfer_req(MT_PRIV(q), q, mp);
	return mt_msg_slow(q, mp);
}
static int
sl_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
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
	case M_IOCACK:
	case M_IOCNAK:
		return sl_r_iocack(q, mp);
	}
	return sl_r_next(q, mp);
}
static inline fastcall int
sl_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return sl_r_data(q, mp);
	if (DB_TYPE(mp) == M_PROTO)
		if (mp->b_wptr >= mp->b_rptr + sizeof(uint32_t))
			if (*(uint32_t *) mp->b_rptr == SL_PDU_IND)
				return sl_pdu_ind(SL_PRIV(q), q, mp);
	return sl_msg_slow(q, mp);
}

/*
 *  QUEUE PUT and SERVICE routines
 *  =========================================================================
 */

/**
 * mt_wput: - upper write put procedure
 * @q: queue to put to
 * @mp: message to put
 *
 * Canonical put procedure.  Attempts to process message or place it on the queue.
 */
static streamscall __hot_put int
mt_wput(queue_t *q, mblk_t *mp)
{
	if ((pcmsg(DB_TYPE(mp)) || (q->q_first || (q->q_flag & QSVCBUSY))) || mt_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * mt_wsrv: - upper write service procedure
 * @q: queue to service
 *
 * Canoncial draining service procedure.
 */
static streamscall __hot_out int
mt_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mt_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/**
 * mt_rsrv: - upper read service procedure
 * @q: queue to service
 *
 * Multiplexing service procedure, simply used for back-enabling across the multiplexing driver.
 */
static streamscall __hot_read int
mt_rsrv(queue_t *q)
{
	struct mt *mt = MT_PRIV(q);

	read_lock(&mt_mux_lock)
	    if (mt->sl)
		qenable(mt->sl->rq);
	read_unlock(&mt_mux_lock);
	return (0);
}

/**
 * mt_rput: - upper read put procedure
 * @q: queue to put message to
 * @mp: the message
 *
 * This is never used, the driver always checks flow control upstream and uses putnext().
 */
static streamscall __unlikely int
mt_rput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "invalid call to mt_rput()");
	putnext(q, mp);
	return (0);
}

/**
 * sl_wput: - lower write put procedure
 * @q: queue to put message to
 * @mp: the message
 *
 * This is never used, the driver always checks flow control downstream and uses putnext().
 */
static streamscall __unlikely int
sl_wput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "invalid call to sl_wput()");
	putnext(q, mp);
	return (0);
}

/**
 * sl_wsrv: - lower write service procedure
 * @q: queue to service
 *
 * Multiplexing service procedure, simply used for back-enabling across the multiplexing driver.
 */
static streamscall __hot_read int
sl_wsrv(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);

	read_lock(&mt_mux_lock)
	    if (sl->other)
		qenable(sl->other->wq);
	read_unlock(&mt_mux_lock)
	    return (0);
}

/**
 * sl_rsrv: - lower read service procedure
 * @q: queue to service
 *
 * Canoncial draining service procedure.
 */
static streamscall __hot_read int
sl_rsrv(queue_t *q)
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
 * sl_rput: - lower read put procedure
 * @q: queue to put to
 * @mp: message to put
 *
 * Canonical put procedure.  Attempts to process message or place it on the queue.
 */
static streamscall __hot_in int
sl_rput(queue_t *q, mblk_t *mp)
{
	if ((pcmsg(DB_TYPE(mp)) || (q->q_first || (q->q_flag & QSVCBUSY))) || sl_msg(q, mp))
		putq(q, mp);
	return (0);
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
static int
mt_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct mt *mt;
	mblk_t *mp;
	int err;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		mi_strlog(q, 0, SL_ERROR, "cannot push as module");
		return (ENXIO);
	}
	if (!(mp = mi_timer_alloc(sizeof(uint))))
		return (ENOBUFS);
	*(uint *) mp->b_rptr = t1;

	if (!cminor)
		sflag = CLONEOPEN;
	if ((err = mi_open_comm(&mt_opens, q, devp, oflags, slfag, crp))) {
		freeb(t1);
		return (err);
	}
	mi_strlog(q, 0, SL_TRACE, "opened character device %d:%d", (int) getmajor(*devp),
		  (int) getminor(*devp));

	mt = MT_PRIV(q);
	bzero(mt, sizeof(*mt));
	mt->rq = RD(q);
	mt->wq = WR(q);
	mt->cred = *crp;
	mt->timers.t1 = mp;
	/* FIXME: initialize some more */
	qprocson(q);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
static int
mt_qclose(queue_t *q, int flag, cred_t *crp)
{
	struct mt *mt;
	unsigned long flags;

	qprocsoff(q);
	mi_strlog(q, 0, SL_TRACE, "closing character device");
	while (!(mt = (struct mt *) mi_sleeplock(q))) ;
	write_lock_irqsave(&mt_mux_lock, flags);
	if (mt->sl) {
		mt->sl->mtp = NULL;
		mt->sl = NULL;
	}
	mi_detach(q, (caddr_t) mt);
	write_unlock_irqrestore(&mt_mux_lock, flags);
	mi_close_unlink(&mt_opens, (caddr_t) mt);
	mi_close_free((caddr_t) mt);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

static struct module_info mt_winfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};
static struct module_info mt_rinfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};
static struct module_info sl_winfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};
static struct module_info sl_rinfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = 272 + 1,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat mt_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mt_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit mtp_rinit = {
	.qi_putp = mt_rput,		/* Write put (message from below) */
	.qi_srvp = mt_rsrv,		/* Write queue service */
	.qi_qopen = mt_qopen,		/* Each open */
	.qi_qclose = mt_qclose,		/* Last close */
	.qi_minfo = &mt_rinfo,		/* Information */
	.qi_mstat = &mt_rstat,		/* Statistics */
};
static struct qinit mtp_winit = {
	.qi_putp = mt_wput,		/* Write put (message from above) */
	.qi_srvp = mt_wsrv,		/* Write queue service */
	.qi_minfo = &mt_winfo,		/* Information */
	.qi_mstat = &mt_wstat,		/* Statistics */
};
static struct qinit sl_rinit = {
	.qi_putp = sl_rput,		/* Write put (message from below) */
	.qi_srvp = sl_rsrv,		/* Write queue service */
	.qi_minfo = &sl_rinfo,		/* Information */
};
static struct qinit sl_winit = {
	.qi_putp = sl_wput,		/* Write put (message from above) */
	.qi_srvp = sl_wsrv,		/* Write queue service */
	.qi_minfo = &sl_winfo,		/* Information */
};

static struct streamtab mtp_mininfo = {
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
MODULE_PARM_DESC(modid, "Module ID for the MTP-MIN driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the MTP-MIN driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct cdevsw mt_cdev = {
	.d_name = DRV_NAME,
	.d_str = &mtp_mininfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
mtp_mininit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = register_strdev(&mt_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver, err = %d", DRV_NAME, err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

MODULE_STATIC void __exit
mtp_minterminate(void)
{
	int err;

	if ((err = unregister_strdev(&mt_cdev, major)) < 0)
		cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME, major);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(mtp_mininit);
module_exit(mtp_minterminate);

#endif				/* LINUX */
